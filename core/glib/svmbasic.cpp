//////////////////////////////////////////////////////////////////////////
// SVM-Cache
void TSVMCache::Reset(const int& memory_size, const int& d) {
    IAssert(memory_size > 0 && d > 0);

    top = bottom = NULL;
    size = 0; dim = numRows = d;

    //how much space takes one full vector in cache:
    int vecsize = (sizeof(TFltType) + sizeof(bool)) * dim;
    //caluculates how much memory still aviable for cache
    //here we don't count vecShrink and linked list as we
    //can't "predict" it's size yet (we calculate it later!)
    int aviable_memory = memory_size*1024*1024
                         - dim * sizeof(int)    //idsT
                         - dim * sizeof(bool)   //saved
                         - dim * sizeof(int);   //vecPt
    //calculates how many vectors we can store in cache
    maxsize = aviable_memory / vecsize;
    //normalize so we don't reservate to much space if less enough
    maxsize = maxsize < dim ? maxsize : dim;
    if (verbosity > 0) {
        printf("cache size = %d (%.3f MB)\n",
               maxsize, maxsize * vecsize / (1024.0 * 1024.0));
    }

    //we reservate space for cache and initialize some vectors
    cache = (maxsize > 0) ? new TFltType[maxsize * dim] : NULL;
    valid = (maxsize > 0) ? new bool[maxsize * dim] : NULL;
    idsT = (maxsize > 0) ? new int[dim] : NULL;
    vecShrink.Reserve(maxsize+1, maxsize+1);

    int i;
    saved.Reserve(dim, dim);
    vecPt.Reserve(dim, dim);
    for (i = 0; i < dim; i++) {
        saved[i] = false;
        idsT[i] = i;
    }
    for (i = 0; i < maxsize+1; i++) {
        vecShrink[i] = NULL;
    }
}

void TSVMCache::Clear() {
    if (cache != NULL) delete[] cache;
    if (valid != NULL) delete[] valid;
    if (idsT != NULL) delete[] idsT;
    cache = NULL; valid = NULL; idsT = NULL;

    //we erase linked list
    TListNode *node = bottom, *tmp;
    while (node != NULL) {
        tmp = node;
        node = node->next;
        delete tmp;
    }
    bottom = top = NULL;

    vecPt.Clr(); saved.Clr();
    numRows = size = maxsize = dim = 0;
}

void TSVMCache::toTop(TListNode *node) {
    //we move i-th column on top of the list
    if (node->prev != NULL && node->next != NULL) {
    //node is somewhere in the middle of the list
        //remove node from the middle
        node->prev->next = node->next;
        node->next->prev = node->prev;
        //move node to the top
        node->next = NULL;
        node->prev = top;
        top->next = node;
        top = node;
    } else if (node == bottom && node->next != NULL) {
    //node is at the bottom of the list and is not alone
        //change bottom to the next node
        bottom = bottom->next;
        bottom->prev = NULL;
        //move node to the top
        node->next = NULL;
        node->prev = top;
        top->next = node;
        top = node;
    } //else node is already at the top
}

TSVMCacheVec TSVMCache::Get(const int& i) {
    Assert(0 <= i && i < dim && saved[i]);
    toTop(vecPt[i]);
    return TSVMCacheVec(vecPt[i]->addr, vecPt[i]->vaddr, idsT, numRows);
}

TFltType TSVMCache::GetVal(const int& i, const int& j) const {
    Assert(0 <= i && i < dim && saved[i] && 0 <= j && j < dim && idsT[j] >= 0);
    return vecPt[i]->addr[idsT[j]];
}

TSVMCacheVec TSVMCache::Add(const int& i) {
    Assert(0 <= i && i < dim && cache != NULL);

    //first check if i is already in cache
    if (saved[i]) {
        //first we move node to top
        toTop(vecPt[i]);
        //than we just replace old one with new one
        return TSVMCacheVec(vecPt[i]->addr, vecPt[i]->vaddr, idsT, numRows);
    }

    TFltType *vecAddr; bool *vecVAddr;
    TListNode *node = new TListNode();

    Assert(size >= 0 && size <= maxsize);
    if (size == 0) {
        //we allocate memory for it (first element => beginning of cache array)
        vecAddr = cache;
        vecVAddr = valid;

        //remeber position of this node in cache
        node->position = 0;

        //first time we add element to the list
        node->next = node->prev = NULL;
        top = bottom = node;

        size++;
    } else if (size < maxsize) {
        //we allocate memory for it
        vecAddr = &cache[size * numRows];
        vecVAddr = &valid[size * numRows];
        //remeber position of this node in cache
        node->position = size;

        //there is still some free space to add elements
        node->next = NULL;
        node->prev = top;
        top->next = node;
        top = node;

        size++;
    } else {
        //list is full, we overwrite bottom element
        //we allocate memory for it
        vecAddr = bottom->addr;
        vecVAddr = bottom->vaddr;
        //remeber position of this node in cache
        node->position = bottom->position;

        //we remove last element in the list
        saved[bottom->ID] = false;
        bottom = bottom->next;
        delete bottom->prev;
        bottom->prev = NULL;

        //add new one to the top
        node->next = NULL;
        node->prev = top;
        top->next = node;
        top = node;
    }

    node->ID = i;
    node->addr = vecAddr;
    node->vaddr = vecVAddr;

    saved[i] = true;
    vecPt[i] = node;
    vecShrink[node->position] = node;

    return TSVMCacheVec(vecAddr, vecVAddr, idsT, numRows);
}

//we rewrite cache so it can store more vectors
//to achive that we throw away some rows, we keep only ones in ids
void TSVMCache::Shrink(const int& n, int *ids) {
    //we bug with this only, if we don't have enough memory to store all rows
    //and if new number of rows to remember is smaller than previous one
    if (n < numRows) {
        // new max size = current size of cache / number of rows in new vectors
        int newMaxSize = (maxsize * numRows) / n;

        int pos = 0; //pos points to next free cell in cache
        int i = 0;   //i points to current node in vecShrink
        while (vecShrink[i] != NULL) { //we have guard at the end of vecShrink!
            TListNode *node = vecShrink[i];
            Assert(i == node->position);
            TFltType *newAddr = &cache[pos];
            //gets rid of old rows, copys only ones that are in ids
            //we suppose that ids is sorted and that idsT is sorted (except -1s)
            int jNew, jOld;
            for (jNew = 0; jNew < n; jNew++) {
                jOld = idsT[ids[jNew]];
                Assert(jOld >= 0); //all rows in ids must already be in cache!
                cache[pos] = node->addr[jOld];
                valid[pos] = node->vaddr[jOld];
                pos++;
            }

            node->addr = newAddr;   //remember new address of this node
            i++;                    //go to next node (if any:)
        }

        //copy ids to idsT
        numRows = n;
        for (i = 0; i < dim; i++) idsT[i] = -1;
        for (i = 0; i < numRows; i++) idsT[ids[i]] = i;

        //resize vecShrink
        vecShrink.Reserve(newMaxSize + 1, newMaxSize + 1);
        for (i = size; i < newMaxSize+1; i++)
            vecShrink[i] = NULL;

        maxsize = newMaxSize;
        if (verbosity > 1) printf("<%d>", maxsize);
    }
}

//////////////////////////////////////////////////////////////////////////
// SVM-Train-Set
THash<TStr, TSVMTrainSet::TSVMTrainSetLoadF> TSVMTrainSet::TypeToLoadFH;

bool TSVMTrainSet::Reg(const TStr& TypeNm, const TSVMTrainSetLoadF& LoadF){
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

PSVMTrainSet TSVMTrainSet::Load(TSIn& SIn){
    TStr TypeNm(SIn);
    int TypeKeyId=-1;
    if (TypeToLoadFH.IsKey(TypeNm, TypeKeyId)){
        TSVMTrainSetLoadF LoadF=TypeToLoadFH[TypeKeyId];
        return (*LoadF())(SIn);
    } else {
        return NULL;
    }
}

void TSVMTrainSet::LinComb(const TFltV& AlphV, TFltV& Result) {
    IAssert(AlphV.Len() == Len());
    int d = Dim(); Result.Gen(d); Result.PutAll(0.0);
    for (int VecC = 0, VecN = AlphV.Len(); VecC < VecN; VecC++)
        AddVec(VecC, Result, AlphV[VecC]);
}

void TSVMTrainSet::LinComb(const TIntV& VecIdV, const TFltV& AlphV, TFltV& Result) {
    IAssert(VecIdV.Len() == AlphV.Len());
    Result.Gen(Dim()); Result.PutAll(0.0);
    for (int VecC = 0, VecN = VecIdV.Len(); VecC < VecN; VecC++)
        AddVec(VecIdV[VecC], Result, AlphV[VecC]);
}

void TSVMTrainSet::GetKeywords(const TFltV& NormalV, TIntFltKdV& WIdWgtV,
        const TIntV& VecIdV, const int& WdN,  const double& VecSign,
        const double& WgtSign, const bool& AvgOverSet) {

    TFltV WgtV(NormalV.Len());
    if (AvgOverSet) {
        // calculate average document
        WgtV.PutAll(0.0);
        if (VecIdV.Empty()) {
            const int Docs = Len();
            for (int DId = 0; DId < Docs; DId++) {
                if (VecSign*GetVecParam(DId) > 0)
                    AddVec(DId, WgtV, 1.0);
            }
        } else {
            for (int DocN = 0; DocN < VecIdV.Len(); DocN++) {
                const int DId = VecIdV[DocN];
                if (VecSign*GetVecParam(DId) > 0)
                    AddVec(DId, WgtV, 1.0);
            }
        }
    } else {
        WgtV.PutAll(1.0);
    }
    // calculate weights for each word
    TFltIntKdV WdWgtV(NormalV.Len(), 0);
    for (int AttrC = 0; AttrC < NormalV.Len(); AttrC++) {
        WdWgtV.Add(TFltIntKd(WgtSign*NormalV[AttrC]*WgtV[AttrC], AttrC));
    }
    // save weights
    if (WdN > 0) {
        // save only top WdN positive weights
        WdWgtV.Sort(false);
        WIdWgtV.Gen(WdN, 0);
        IAssert(WdN <= NormalV.Len());
        for (int WdC = 0; WdC < WdN; WdC++) {
            const double WdWgt = WdWgtV[WdC].Key;
            const int WdId = WdWgtV[WdC].Dat;
            if (WdWgt > 0) {
                WIdWgtV.Add(TIntFltKd(WdId, WdWgt));
            }
        }
    } else {
        // save all positive weights
        WIdWgtV.Clr();
        for (int WdC = 0; WdC < WdWgtV.Len(); WdC++) {
            const double WdWgt = WdWgtV[WdC].Key;
            const int WdId = WdWgtV[WdC].Dat;
            if (WdWgt > 0.0) {
                WIdWgtV.Add(TIntFltKd(WdId, WdWgt));
            }
        }
    }
}

double TSVMTrainSet::CalcSvmUnblParam(const double& MxVal, const double& MnVal) {
    int PosVecs = 0;
    int NegVecs = 0;
    for (int VecN = 0; VecN < Len(); VecN++) {
        const double VecParam = GetVecParam(VecN);
        if (VecParam > 0.0) {
            PosVecs++;
        } else if (VecParam < 0.0) {
            NegVecs++;
        }
    }

    if (PosVecs > 0 && NegVecs > 0) {
        const double CalcVal = double(NegVecs)/double(PosVecs);
        return TMath::Median(MnVal, CalcVal, MxVal);
    } else {
        return 1.0;
    }
}

int TSVMTrainSet::GetSignVecs(const double& Sign) {
    int SignVecs = 0;
    for (int VecN = 0; VecN < Len(); VecN++) {
        if ((Sign*GetVecParam(VecN)) > 0.0) SignVecs++;
    }
    return SignVecs;
}

bool TSVMTrainSet::HasPosNegVecs(const int& MnVec) {
    int PosVecs = GetSignVecs(1.0);
    int NegVecs = GetSignVecs(-1.0);
    return (PosVecs >= MnVec) && (NegVecs >= MnVec);
}

bool TSVMTrainSet::HasPosVecs(const int& MnPosVec) {
    int PosVecs = GetSignVecs(1.0);
    return (PosVecs >= MnPosVec);
}

bool TSVMTrainSet::HasNegVecs(const int& MnNegVec) {
    int NegVecs = GetSignVecs(-1.0);
    return (NegVecs >= MnNegVec);
}

///////////////////////////////////////////////////////////////////////
// Sparse-Train-Set-Column-Matrix
TSTSetMatrix::TSTSetMatrix(PSVMTrainSet Set, TFltV& ClsV): TMatrix(), ColVV(Set) {
    IAssert(Set->Type() == ststSparse);
    ClsV.Gen(Set->Len(), 0);
    for (int i = 0; i < Set->Len(); i++)
        ClsV.Add(Set->GetVecParam(i) * 0.99);
}

void TSTSetMatrix::PMultiply(const TFltVV& B, int ColId, TFltV& Result) const {
    IAssert(B.GetXDim() >= PGetCols() && Result.Len() >= PGetRows());
    int RowN = PGetRows(), ColN = PGetCols();
    int i, j; TFlt *ResV = Result.BegI();
    for (i = 0; i < RowN; i++) ResV[i] = 0.0;
    for (j = 0; j < ColN; j++) {
        const TIntFltKdV& ColV = ColVV->GetAttrSparseV(j); int len = ColV.Len();
        for (i = 0; i < len; i++) {
            if (ColV[i].Key < Result.Len())
                ResV[ColV[i].Key] += ColV[i].Dat * B(j,ColId);
        }
    }
}

void TSTSetMatrix::PMultiply(const TFltV& Vec, TFltV& Result) const {
    IAssert(Vec.Len() >= PGetCols() && Result.Len() >= PGetRows());
    int RowN = PGetRows(), ColN = PGetCols();
    int i, j; TFlt *ResV = Result.BegI();
    for (i = 0; i < RowN; i++) ResV[i] = 0.0;
    for (j = 0; j < ColN; j++) {
        const TIntFltKdV& ColV = ColVV->GetAttrSparseV(j); int len = ColV.Len();
        for (i = 0; i < len; i++) {
            if (ColV[i].Key < Result.Len())
                ResV[ColV[i].Key] += ColV[i].Dat * Vec[j];
        }
    }
}

void TSTSetMatrix::PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const {
    IAssert(B.GetXDim() >= PGetRows() && Result.Len() >= PGetCols());
    int ColN = PGetCols();
    int i, j, len; TFlt *ResV = Result.BegI();
    for (j = 0; j < ColN; j++) {
        const TIntFltKdV& ColV = ColVV->GetAttrSparseV(j);
        len = ColV.Len(); ResV[j] = 0.0;
        for (i = 0; i < len; i++) {
            if (ColV[i].Key < B.GetXDim())
                ResV[j] += ColV[i].Dat * B(ColV[i].Key, ColId);
        }
    }
}

void TSTSetMatrix::PMultiplyT(const TFltV& Vec, TFltV& Result) const {
    IAssert(Vec.Len() >= PGetRows() && Result.Len() >= PGetCols());
    int ColN = PGetCols();
    int i, j, len; TFlt *VecV = Vec.BegI(), *ResV = Result.BegI();
    for (j = 0; j < ColN; j++) {
        const TIntFltKdV& ColV = ColVV->GetAttrSparseV(j);
        len = ColV.Len(); ResV[j] = 0.0;
        for (i = 0; i < len; i++) {
            if (ColV[i].Key < Vec.Len())
                ResV[j] += ColV[i].Dat * VecV[ColV[i].Key];
        }
    }
}

void TSTSetMatrix::Load(TSparseColMatrix& Matrix, TFltV& y,
        PSVMTrainSet TrainSet, const double& PWgt, const double& NWgt) {

    IAssert(TrainSet->Type() == ststSparse);
    Matrix.ColN = TrainSet->Len();
    Matrix.RowN = TrainSet->Dim();
    for (int i = 0; i < Matrix.ColN; i++) {
        Matrix.DocSpVV.Add(TrainSet->GetAttrSparseV(i));
        y.Add(TrainSet->GetVecParam(i) > 0.0 ? PWgt : NWgt);
    }
    Matrix.DocSpVV.Pack();
}

//////////////////////////////////////////////////////////////////////////
// Kernel
THash<TStr, TKernel::TKernelLoadF> TKernel::TypeToLoadFH;

bool TKernel::Reg(const TStr& TypeNm, const TKernelLoadF& LoadF){
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

PKernel TKernel::Load(TSIn& SIn){
    TStr TypeNm(SIn);
    int TypeKeyId=-1;
    if (TypeToLoadFH.IsKey(TypeNm, TypeKeyId)){
        TKernelLoadF LoadF=TypeToLoadFH[TypeKeyId];
        return (*LoadF())(SIn);
    } else {
        return NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
// other kernels
bool TPolynomialKernel::IsReg = TPolynomialKernel::MkReg();
bool TRadialKernel::IsReg = TRadialKernel::MkReg();
bool TSigmoidKernel::IsReg = TSigmoidKernel::MkReg();

//////////////////////////////////////////////////////////////////////////
// SVM-Quadratic-Problem-Solver for small problems
TSVMQP::TSVMQP(const int& nn, const int& mm, const double& ll):
  n(nn), m(mm) {

    //create array as pr_loqo requests it
    pl = new double[n];
    for (int i = 0; i < n; i++)
        pl[i] = ll;

    //locate place for results
    primal = new double[3*n];
    dual = new double[m + 2*n];
}

TSVMQP::~TSVMQP() {
    delete[] pl; delete[] primal; delete[] dual;
}

bool TSVMQP::solve(const int& size, const double& bb, double *u, double *Q,
                   double *A, double *c, double *resultV, const int& verbosity) {
    IAssert(size <= n);
    int i;

    //some other parameters
    double b = bb;
    int verb = 0;
    double init_margin = 0.15;
    long init_iter = 500;
    double sigdig = 8;

    //minimize it
    int result = TPrLoqo::pr_loqo(size, m, c, Q, A, &b, pl, u,
                         primal, dual, verb, sigdig,
                         init_iter, init_margin,
                         u[0] / 4.0, 0);

    if (!TFlt::IsNum(dual[0]) && verbosity > 0) printf("ERR[nan]");

    if (result == OPTIMAL_SOLUTION) {
        for (i = 0; i < size; i++) resultV[i] = primal[i];
        return true;
    };

    if (verbosity > 0) printf("ERR[%d]", result);
    return false;
}

//////////////////////////////////////////////////////////////////////////
// SVM-Quadratic-Problem-Solver for big problems
inline double TSVMQPSolver::kernel(const int& i1, const int& i2) const {
    if (is_linear) {
        return docs->DotProduct(i1, i2);
    } else {
        return nonlin_kernel->CalcSet(docs, i1, i2);
    }
}

void TSVMQPSolver::readColumnRows(const int& i, const int& len, int *ids,
                        double *result, const bool& store, const bool& sync) {
    int j,k;

    //first we check if we have this column already in cache
    if (cache.Exists(i))  {
        cache_yes++;
        //read vector from cache
        TSVMCacheVec vec = cache.Get(i);
        TFltType *vecVals = vec.valT;
        bool *vecValid = vec.validT;
        int *vecIds = vec.idsT;
        int vecLen = vec.dim;

        if (sync) {
            //if rows stored in cache are synchronised with active set, that
            //means that all values from active set (+some more) are in cache
            if (len == vecLen) {
                for (j = 0; j < len; j++) {
                    Assert(vecIds[ids[j]] == j);
                    result[j] = vecVals[j];
                }
            } else {
                for (j = 0; j < len; j++)  {
                    k = vecIds[ids[j]];
                    Assert(0 <= k && k <= vecLen && vecValid[k]);
                    result[j] = vecVals[k];
                }
            }
        } else {
            //usualy this part is used at and after reactivating
            //bool thisRowInCache = cache.IsRowInCache(i);
            for (j = 0; j < len; j++) {
                k = vecIds[ids[j]];
                //first check if we cache this row at all
                if (k >= 0) {
                    if (vecValid[k]) {
                        Assert(k <= vecLen);
                        result[j] = vecVals[k];
                    } else {
                        //if (thisRowInCache && cache.Exists(k)) {
                        if (cache.IsRowValid(ids[j], i)) {
                            result[j] = cache.GetVal(ids[j], i);
                        } else {
                            result[j] = kernel(i, ids[j]);
                            kernel_count++;
                        }
                        vecVals[k] = (TFltType)result[j];
                        vecValid[k] = true;
                    }
                } else {
                    //we don't cache this row, all we can do is calculate it
                    result[j] = kernel(i, ids[j]);
                    kernel_count++;
                }
            } // for
        } // if (sync)
    } else {
        //if not we calculate it
        cache_no++;
        for (j = 0; j < len; j++) {
            k = ids[j];
            if (cache.IsRowValid(ids[j], i)) {
                result[j] = cache.GetVal(k, i);
            } else {
                result[j] = kernel(i, ids[j]);
                kernel_count++;
            }
        }

        if (store) {
            TSVMCacheVec vec = cache.Add(i);
            TFltType *vecVals = vec.valT;
            bool *vecValid = vec.validT;
            int *vecIds = vec.idsT;
            int vecLen = vec.dim;

            if (sync) {
                if (len == vecLen) {
                    for (j = 0; j < len; j++) {
                        Assert(vecIds[ids[j]] != -1);
                        vecVals[j] = (TFltType)result[j];
                        vecValid[j] = true;
                    }
                } else {
                    //we didn't calculate all components of cashed vector
                    for (j = 0; j < vecLen; j++) vecValid[j] = false;
                    for (j = 0; j < len; j++) {
                        k = vecIds[ids[j]];
                        Assert(0 <= k && k <= vecLen);
                        vecVals[k] = (TFltType)result[j];
                        vecValid[k] = true; //we mark this component valid
                    }
                }
            } else {
                int l = docs->Len();
                for (j = 0; j < vecLen; j++) vecValid[j] = false;
                for (j = k = 0; j < l; j++) {
                    if (j < ids[k]) {
                        //we didn't calculate this value yet
                        //first check if we need to cache it
                        if (vecIds[j] >= 0) {
                            Assert(vecIds[j] <= vecLen);
                            if (cache.IsRowValid(j, i)) {
                                vecVals[vecIds[j]] = cache.GetVal(j, i);
                            } else {
                                vecVals[vecIds[j]] = (TFltType)kernel(i, j);
                                kernel_count++;
                            }
                            vecValid[vecIds[j]] = true;
                        }
                    } else {
                        Assert(j == ids[k]);
                        //this one we already calculated
                        //first check if we need to cache it
                        if (vecIds[j] >= 0) {
                            vecVals[vecIds[j]] = (TFltType)result[k];
                            vecValid[vecIds[j]] = true;
                        }
                        k++;
                    }
                }
            } // if (sync)
        } // if (store)
    } // if (exists)
}

inline void TSVMQPSolver::column(const int& i, double *result) {
    //we read column from cache with rows USV and store it to cache if not yet there
    readColumnRows(i, numUSV, USV, result, true, shrinking);
}

void TSVMQPSolver::calcColumns(const int& n) {
    //TODO here we could exploit threades...
    for (int i = 0; i < n; i++)
        column(USV[workingSet[i]], cols[i]);
}

void TSVMQPSolver::optimize(TFltV& alphas, double& _base, const PSVMTrainSet& _docs,
       const TFltV& _pV, const TFltV& _yV, const double& _D, const TFltV& _CV,
       const int& sub_size, const int& memory_size, const int& time) {

    IAssert(_docs->Len() > 0 && _docs->Dim() > 0);
    IAssert(_pV.Len() == _yV.Len() && _yV.Len() == _CV.Len());

    docs = _docs;
    int i, ii, j, jj;       //counters
    int len = docs->Len();  //number of training vectors
    //int dim = docs->Dim();  //dimension of training vectors

    //size of sub-qp problem for QPSolver (must be even)
    int size = sub_size + sub_size % 2;
    size = size < len ? size : len;
    IAssert(size >= 2);

    //allocates memory and initializes QP solver
    double *QQ = new double[size*size];
    double *Q = new double[size * size]; //array for storing dot matrix
    double *A = new double[size]; //for storing constraint matrix
    double *cc = new double[size]; //for storing linear part of objective function
    double *u = new double[size]; //upper bound for constrains
    TSVMQP qp(size);

    //cache
    if (!is_linear) initializeCache(memory_size, size);

    //shrinking
    bool old_shrinking = shrinking;
    shrink = new TShrinkState;
    shrinkInitialize();

    //main variables (like alphaV, gradV...)
    initialize(_pV, _yV, _D, _CV, size);
    int n; //number of elements in current working set
    // initial state of alphas is not zero => gradient is not zero!!
    initRelearning(alphas);
    // D > 0.0 ONLY FOR ONE CLASS SVM
    initNonzeroD(size);

    //threads
    //ThreadN = 1;
    //initializeThreads(size);

    //for some statistic about performance
    TSVMTime time_tmp, time_workingset = 0, time_kernel = 0, time_prepare = 0,
             time_solve = 0, time_s = 0, time_analize = 0, time_shrink = 0;

    //some extra varibles in use
    double leq, res; //leq == lambda^ep, res is for temporary results
    double old_violation = 0.0;
    double absChange = 1.0; //sum of absolute values of changes of alphas in one iteration
    int noChangeCount = 0; //count how many times in a row we had no change
    int numUnbnd;   //number of unboudned elements

    TSVMTime trainingTime = GetCurrentTime();
    bool overTime = false; iter = 0;
    do {
        if (verbosity > 0 && (verbosity > 1 || iter % 100 == 0)) printf(".");
        iter++;

        time_tmp = GetCurrentTime();
        bool DoingRandomStep = false;
        //select working set
        //if (iter > 0 && ((absChange < EPSILON) || (iter % 100 == 0))) {
        //    //no change with working set selected last time
        //    //we select new working set randomly!
        //    //we do this also from time to time to escame cycling
        //    if (verbosity > 1) printf("*");
        //    n = selectWorkingSet(size, true);
        //    if (iter % 100 == 0)
        //        DoingRandomStep = true;
        //} else {
            //otherwise we use standard way (gradient)
            n = selectWorkingSet(size, false);
        //}
        IAssert(n >= 2 && n <= size);
        time_workingset += GetCurrentTime() - time_tmp;

        //we check cache for n columns of Hessian matrix
        time_tmp= GetCurrentTime();
        if (!is_linear) {
            calcColumns(n);
        }
        time_kernel += GetCurrentTime() - time_tmp;

        //prepares data for QP
        time_tmp = GetCurrentTime();
        double b = 0.0;
        for (i = 0; i < numUSV; i++) b -= alphaV[USV[i]] * y[USV[i]];
        for (i = 0; i < numBSV; i++) b -= alphaV[BSV[i]] * y[BSV[i]]; // C -> alphaV

        for (i = 0; i < n; i++) {
            ii = USV[workingSet[i]];
            Assert(0 <= ii && ii < len);

            b += alphaV[ii] * y[ii];  //add what we subtracted to much
            A[i] = y[ii];
            cc[i] = gradV[ii];
            u[i] = C[ii];

            for (j = 0; j < n; j++) {
                jj = USV[workingSet[j]];
                Assert(0 <= jj && jj < len);

                if (is_linear) {
                    kernel_count++;
                    if (j >= i) {
                        QQ[i*n + j] = res = kernel(ii,jj);
                    } else {
                        res = QQ[j*n + i];
                    }
                } else {
                    res = cols[i][workingSet[j]];
                }

                cc[i] -= alphaV[jj]*y[jj]*res;
                Q[i*n + j] = y[ii]*y[jj]*res;
            }

            cc[i] = y[ii]*cc[i] + p[ii];
        }
        b = D + b;
        time_prepare += GetCurrentTime() - time_tmp;

        time_tmp= GetCurrentTime();
        double *newAlphaV = new double[n];   //allocate memory for result
        //we call QP
        if (!qp.solve(n, b, u, Q, A, cc, newAlphaV, verbosity)) {
            //error, load old values as new so next step will have random workset selection
            for(int i = 0; i < n; i++)
                newAlphaV[i] = alphaV[USV[workingSet[i]]];
        }

        for (i = 0; i < n; i++) {
            if (C[USV[workingSet[i]]] - newAlphaV[i] < EPSILON)
                newAlphaV[i] = C[USV[workingSet[i]]];
            else if (newAlphaV[i] < EPSILON)
                newAlphaV[i] = 0.0;
        }
        time_solve += GetCurrentTime() - time_tmp;

        time_tmp = GetCurrentTime();
        //adjust vector s
        adjustS(n, newAlphaV);
        time_s += GetCurrentTime() - time_tmp;

        time_tmp= GetCurrentTime();
        //replace old alphas with new ones and delete newAlphaV
        absChange = 0.0;
        for (i = 0; i < n; i++) {
            ii = USV[workingSet[i]];
            used[ii] = false;
            absChange += fabs(alphaV[ii] - newAlphaV[i]);
            alphaV[ii] = newAlphaV[i];
        }
        delete[] newAlphaV;
        if (DoingRandomStep) {
            absChange = 1.0; noChangeCount = 0;
        } else if (absChange < EPSILON) {
            noChangeCount++;
        } else {
            noChangeCount = 0;
        }

        //refreshes listV and calculates lambda^eq
        leq = 0.0;
        numUnbnd = 0;
        for (i = numUnbnd = 0; i < numUSV; i++) {
            ii = USV[i];
            res = gradV[ii] + y[ii] * p[ii];
            listV[ii] = res;
            if (EPSILON < alphaV[ii] && alphaV[ii] < C[ii] - EPSILON) {
                //leq += - y[ii]*p[ii] - gradV[ii];
                leq += -res;
                numUnbnd++;
            }
        }

        if (numUnbnd > 0) {  //if we have any unbodunded alphas
            base = leq = leq / numUnbnd;

            //check terminating conditions
            old_violation = max_violation;
            max_violation = shrinking ? shrinkCheckOptimality(1, leq) :
                                        checkOptimality(leq);

            //shrinking
            if (shrinking) {
                if (is_linear && iter > 30 && iter % 3 == 0 && numUSV < len &&
                    max_violation - old_violation > 0.1) {
                    //(max_violation - old_violation)/max_violation > 0.2) {

                    //if max_violation increased, we reactivate variables (only linearSVM!)
                    if (verbosity > 1) printf("#");
                    shrinkReactivate();
                    shrinkCheckOptimality(h, leq);
                    if (shrink->count >= minNumElts)
                        shrinkReduce();

                } else if (max_violation > epsilon_ter) {
                    //if turn and if any variable found, shrink
                    if ((shrink->count >= minNumElts) && (iter % h == 0) &&
                        (is_linear || shrink->hcount < shrink->aHist.Reserved())) {

                          shrinkReduce();
                    }

                } else  {
                    time_analize += GetCurrentTime() - time_tmp;

                    time_tmp= GetCurrentTime();
                    //if terminating conditions are ok than we check shrinked variables
                    if (verbosity > 0) printf("\nchecking unactive variables...");
                    shrinkReactivate();
                    time_shrink += GetCurrentTime() - time_tmp;

                    time_tmp= GetCurrentTime();
                    //check optimality again
                    old_violation = max_violation;
                    max_violation = checkOptimality(leq);
                    if (verbosity > 0 && max_violation > epsilon_ter) printf("\n");

                    if (!is_linear) shrinking = false;
                }
            }

            if (verbosity > 2) printf("<%.5f,%.5f>", max_violation, absChange);
        }
        time_analize += GetCurrentTime() - time_tmp;

        if (time != -1) overTime = ToSeconds(GetCurrentTime() - trainingTime) > time;
    } while ((max_violation > epsilon_ter || numUnbnd == 0) && (!overTime) && (noChangeCount < 30));

    shrinking = old_shrinking;

    alphas.Reserve(len,len);
    for (i = 0; i < len; i++)
        alphas[i] = alphaV[i];
    _base = base;

    if (verbosity > 0) {
        printf("\n");
        if (verbosity == 1) {
            printf("Max-Violation = %.6f\n", max_violation);
            double time_all = ToSeconds(time_workingset + time_kernel +
                time_prepare + time_solve + time_s + time_analize + time_shrink);
            printf("Iterations = %d, Time=%.3f\n", iter, time_all);
        } else {
            printf("Max-Violation = %.6f\n", max_violation);
            printf("Iterations = %d, Kernel-Count=%d\n", iter, kernel_count);
            if (verbosity > 1) {
                printf("Time: workset=%.3f, kernel=%.3f, prepare=%.3f, solve=%.3f,\n"
                       "      s=%.3f, analize=%.3f, shrink=%.3f\n",
                    ToSeconds(time_workingset), ToSeconds(time_kernel), ToSeconds(time_prepare),
                    ToSeconds(time_solve), ToSeconds(time_s), ToSeconds(time_analize),
                    ToSeconds(time_shrink));
            }

            //if not linear we show cache statistics
            if (!is_linear)
                printf("Cache: yes = %d, no = %d\n", cache_yes, cache_no);
        }
    }

    //dealocate memory
    dealocate();
    //killThreads();
    delete[] QQ; delete[] Q; delete[] A; delete[] cc; delete[] u;
    delete shrink;
}

void TSVMQPSolver::initializeCache(const int& memory_size, int& size) {
    cache.Reset(memory_size, docs->Len());
    //cache should be at least big enough to store one working set
    if (cache.MaxSize() < size) {
        if (cache.MaxSize() < 2) {
            printf("cache size is to small, can't even store two columns!\n");
            //have to trow exception here!
        } else {
            size = cache.MaxSize() - cache.MaxSize() % 2;
            if (verbosity > 0)
                printf("cache size is to small, size of subQP = %d", size);
        }
    }
    cache_yes = cache_no = 0;
}

void TSVMQPSolver::initialize(const TFltV& _pV, const TFltV& _yV,
           const double& _D, const TFltV& _CV, const int& size) {

    int len = docs->Len();
    int dim = docs->Dim();

    max_violation = 0.0;       //bigest KKT violation
    kernel_count = 0;          //counting how many times we evaluate kernel function

    D = _D;
    p = new double[len];
    y = new double[len];
    C = new double[len];
    gradV = new double[len];   //reservates place for gradient
    listV = new double[len];
    alphaV = new double[len];  //reservates place for alphas
    used = new bool[len];       //so we don't repeat elements in working set
    workingSet = new int[size]; //holds indexes of elements in current working set
    for (int i = 0; i < len; i++) {
        alphaV[i] = gradV[i] = 0.0;
        used[i] = false;
        p[i] = _pV[i];
        y[i] = _yV[i];
        listV[i] = y[i] * p[i];
        IAssert(_CV[i] > 0); C[i] = _CV[i];
    }

    if (is_linear) {
        //hyperplane, we use it in linear SVM for calculating gradV
        weightV = new double[dim];
        for (int j = 0; j < dim; j++) weightV[j] = 0.0;
    }

    if (!is_linear) {
        cols.Reserve(size, size);
        for (int i = 0; i < size; i++) cols[i] = new double[len];
    }
}

void TSVMQPSolver::initRelearning(TFltV& alphas) {
    int len = docs->Len();  //number of training vectors
    int dim = docs->Dim();  //dimension of training vectors
    int i, ii;
    if (alphas.Len() != 0 && alphas.Len() == len) {
        printf("relearning...\n");
        double res;
        if (is_linear) {
            //first we calculate diference in weight vector (w)
            for (i = 0; i < dim; i++) weightV[i] = 0.0;
            for (i = 0; i < len; i++) {
                res = (alphas[i] - alphaV[i]) * y[i];
                docs->AddVec(i, weightV, dim, res);
            }

            //from that we can calculate s
            for (i = 0; i < numUSV; i++) {
                ii = USV[i];
                //place to do some optimazing ...
                gradV[ii] += docs->DotProduct(ii, weightV, dim);
                listV[ii] += gradV[ii];
            }
        } else {
            printf("relearning not implemented for non-linear svm!\n"); Fail;
        }
    }
}

void TSVMQPSolver::initNonzeroD(const int& size) {
    int len = docs->Len();  //number of training vectors
    int dim = docs->Dim();  //dimension of training vectors
    int i, j;

    //we set alphas so they satisfy conditions y'a = D and 0 <= a <= C
    //if D = 0, than we leave all alphas to zero, otherwise we set
    //M := 2*D/C alphas for which y[i] > 0 to D/M.
    //after this we also have to adjust gradV to reflect current state!
    if (D > 0.0) {
        if (is_linear)
            for (i = 0; i < dim; i++) weightV[i] = 0.0;

        double minC = C[0];
        for (i = 0; i < len; i++)
            minC = TFlt::GetMn(minC, C[i]);
        int M1 = (int)(2*D/minC) + size, M2 = 0;
        for (i = 0; i < len; i++) if (y[i] > 0.0) M2++;
        int M = M1 < M2 ? M1 : M2;
        double beginningAlpha = D/M;

        double res;
        i = 0;
        while (i < len) {
            if (y[i] > 0.0) {
                alphaV[i] = beginningAlpha;
                M--;

                if (is_linear) {
                    docs->AddVec(i, weightV, dim, alphaV[i] * y[i]);
                } else {
                    double *ker = new double[len];
                    column(i, ker);
                    res = alphaV[i] * y[i];
                    for (j = 0; j < len; j++) {
                        gradV[j] += res * ker[j];
                    }
                    delete[] ker;
                }

                if (M == 0) break;
            }
            i++;
        }
        IAssert(M == 0);

        //now we set gradV
        if (is_linear) {
            for (i = 0; i < len; i++) {
                gradV[i] += docs->DotProduct(i, weightV, dim);
            }
        }

        for (i = 0; i < len; i++)
            listV[i] += gradV[i];
    }
}

void TSVMQPSolver::dealocate() {
    if (!is_linear) {
        cache.Clear();
        int size = cols.Len();
        for (int i = 0; i < size; i++) delete cols[i];
    }

    delete[] p, delete[] y; delete[] C;
    delete[] gradV; delete[] listV;
    delete[] alphaV;
    if (is_linear) delete[] weightV;
    delete[] used; delete[] workingSet;
    delete[] USV; delete[] BSV; delete[] NSV;
}

int TSVMQPSolver::selectWorkingSet(const int& n, const bool& random) {
    int i, j, jj;   //counters
    int q = n / 2;

    if (random) {
        TIntV WorkSetTmpV(numUSV, 0);
        for (i = 0; i < numUSV; i++) {
            if (!used[USV[i]])
                WorkSetTmpV.Add(i);
        }
        WorkSetTmpV.Shuffle(rnd);
        const int Len = TInt::GetMn(numUSV, n);
        for (int i = 0; i < Len; i++) {
            j = WorkSetTmpV[i];
            jj = USV[j];
            workingSet[i] = j;
            used[jj] = true;
        }
        return n;
    }

    TMaxHeap maxHeap(q); TMaxHeapVal maxElt;
    TMinHeap minHeap(q); TMinHeapVal minElt; //TODO - q -> n (!!)

    //check for min and max element in list that is not used already (used[i] == false)
    for (j = 0; j < numUSV; j++) {
        jj = USV[j];

        if (EPSILON < alphaV[jj] && alphaV[jj] < C[jj] - EPSILON) {
            minElt.id = maxElt.id = j;
            minElt.val = maxElt.val = listV[jj];

            if (!maxHeap.Full()) { maxHeap.Add(maxElt); }
            else if (maxElt > maxHeap.Top()) { maxHeap.ChangeTop(maxElt); }

            if (!minHeap.Full()) { minHeap.Add(minElt);}
            else if (minElt > minHeap.Top()) { minHeap.ChangeTop(minElt); }

        } else {
            if ((alphaV[jj] < EPSILON && y[jj] < 0) ||
                (alphaV[jj] > C[jj] - EPSILON && y[jj] > 0)) {

                    maxElt.id = j; maxElt.val = listV[jj];
                    if (!maxHeap.Full()) { maxHeap.Add(maxElt); }
                    else if (maxElt > maxHeap.Top()) { maxHeap.ChangeTop(maxElt); }
            }

            if ((alphaV[jj] < EPSILON && y[jj] > 0) ||
                (alphaV[jj] > C[jj] - EPSILON && y[jj] < 0)) {

                    minElt.id = j; minElt.val = listV[jj];
                    if (!minHeap.Full()) { minHeap.Add(minElt); }
                    else if (minElt > minHeap.Top()) { minHeap.ChangeTop(minElt); }
            }
        }
    } //for

    //current size of working set
    int cs = 0;

    //copy maxHeap to working set
    int heapSize = maxHeap.Size();
    TMaxHeapVal *maxArray = maxHeap.Array();
    for (i = 1; i <= heapSize; i++) {
        Assert(0 <= maxArray[i].id && maxArray[i].id < numUSV);
        workingSet[cs++] = maxArray[i].id;
        used[USV[maxArray[i].id]] = true;
    }

    //copy minHeap to working set
    heapSize = minHeap.Size();
    TMinHeapVal *minArray = minHeap.Array();
    for (i = 1; i <= heapSize; i++) {  //J: VC++ ima bug pri for zankah, zato gre 'int i' ven
        if (!used[USV[minArray[i].id]]) {
            Assert(0 <= minArray[i].id && minArray[i].id < numUSV);
            workingSet[cs++] = minArray[i].id;
            used[USV[minArray[i].id]] = true;
        } else {
            //printf("^");
        }
    }
    return cs;
}

void TSVMQPSolver::adjustS(const int& n, double *newAlphaV) {
    int dim = docs->Dim();
    int i, ii, j ,jj;
    double res;

    if (is_linear) {
        //first we calculate diference in weight vector (w)
        for (i = 0; i < dim; i++) weightV[i] = 0.0;
        for (i = 0; i < n; i++) {
            ii = USV[workingSet[i]];
            res = (newAlphaV[i] - alphaV[ii]) * y[ii];
            docs->AddVec(ii, weightV, dim, res);
        }

        //from that we can calculate s
        for (i = 0; i < numUSV; i++) {
            ii = USV[i];
            //place to do some optimazing ...
            gradV[ii] += docs->DotProduct(ii, weightV, dim);
        }
    } else {
        //brute force for s
        double *ker;
        for (i = 0; i < n; i++) {
            ii = USV[workingSet[i]];
            res = (newAlphaV[i] - alphaV[ii]) * y[ii];
            ker = cols[i];
            for (j = 0; j < numUSV; j++) {
                jj = USV[j];
                gradV[jj] += res * ker[j];
            }
        }
    }
}

double TSVMQPSolver::checkOptimality(double leq) {
    int i, ii;
    double res, max = 0.0;

    for (i = 0; i < numUSV; i++) {
        ii = USV[i];
        if (EPSILON < alphaV[ii] && alphaV[ii] < C[ii] - EPSILON) {
            // -y[i]*gradF(a)[i] == leq
            res = fabs(leq + listV[ii]);
            //max = (max > res) ? max : res;
            max = TFlt::GetMx(max, res);
        } else if (alphaV[ii] < EPSILON) {
            //res >= 0
            res = y[ii]*(gradV[ii] + leq) + p[ii];  // fix -(..) -> ..
            //max = (max > -res) ? max : -res;
            max = TFlt::GetMx(max, -res);
        } else {
            Assert(alphaV[ii] > C[ii] - EPSILON);
            //res <= 0
            res = y[ii]*(gradV[ii] + leq) + p[ii];
            //max = (max > res) ? max : res;
            max = TFlt::GetMx(max, res);
        }
    }

    return max;
}

void TSVMQPSolver::shrinkInitialize() {
    int len = docs->Len();
    int i;

    //initializes shrinking
    USV = new int[len];     //unbounded support vectors
    BSV = new int[len];     //bounded support vectors
    NSV = new int[len];     //non-support vectors
    //number of each type of vectors
    numUSV = len;
    numBSV = numNSV = 0;

    shrink->BSVcount = new int[len];
    shrink->NSVcount = new int[len];
    if (is_linear) {
        shrink->epsilon = 0.01; //TODO check how in SVM light old == 1e-1
        shrink->aOld = new double[len];
        shrink->sOld = new double[len];

        for (i = 0; i < len; i++) {
            //at the beginning all are assumed to be USV
            USV[i] = i;
            shrink->BSVcount[i] = shrink->NSVcount[i] = 0;
            shrink->aOld[i] = shrink->sOld[i] = 0.0;
        }
    } else {
        shrink->epsilon = 2.0;
        shrink->aHist.Reserve(100,0);
        shrink->histID = new int[len];
        shrink->hcount = 0;

        for (i = 0; i < len; i++) {
            //at the beginning all are assumed to be USV
            USV[i] = i;
            shrink->BSVcount[i] = shrink->NSVcount[i] = 0;
            shrink->histID[i] = -1;
        }
    }
}

double TSVMQPSolver::shrinkCheckOptimality(int step, const double& leq) {
    int i, ii; // int maxId = -1;
    double res, max = 0.0;

    if (!is_linear)
        shrink->epsilon = 0.7*shrink->epsilon + 0.3*max_violation;

    shrink->count = 0;
    for (i = 0; i < numUSV; i++) {
        ii = USV[i];

        if (EPSILON < alphaV[ii] && alphaV[ii] < C[ii] - EPSILON) {
            //if unbounded only check optimality
            // -y[i]*gradF(a)[i] == leq
            res = fabs(leq + listV[ii]);
            //max = (max > res) ? max : res;
            //if (res > max) maxId = ii;
            max = TFlt::GetMx(max, res);
        } else if (alphaV[ii] < EPSILON) {
            res = y[ii]*(gradV[ii] + leq) + p[ii];

            //checking condition for shrinking
            if (res > shrink->epsilon) {
                shrink->NSVcount[ii] += step;
                if (shrink->NSVcount[ii] >= h) shrink->count++;
            } else {
                shrink->NSVcount[ii] = 0;
            }

            //checking optimality (res >= 0)
            //max = (max > -res) ? max : -res;
            //if (-res > max) maxId = ii;
            max = TFlt::GetMx(max, -res);
        } else if (alphaV[ii] > C[ii] - EPSILON) {
            res = y[ii]*(gradV[ii] + leq) + p[ii];

            //checking for shrinking
            if (res < -shrink->epsilon) {
                shrink->BSVcount[ii] += step;
                if (shrink->BSVcount[ii] >= h) shrink->count++;
            } else {
                shrink->BSVcount[ii] = 0;
            }

            //checking optimality (res <= 0)
            //max = (max > res) ? max : res;
            //if (res > max) maxId = ii;
            max = TFlt::GetMx(max, res);
        } else {
            printf("we shouldn't be here... %d\n", __LINE__); Fail;
        }
    }

    //printf("<%d:%.2f>", maxId, max);
    return max;
}

void TSVMQPSolver::shrinkReduce() {
    int len = docs->Len();
    int i, ii, j = 0;
    int newnumUSV = 0;

    //make history
    if (!is_linear) {
        TFltV ha(len);
        for (i = 0; i < len; i++) ha[i] = alphaV[i];
        shrink->aHist.Add(TFltVP::New(ha));
    } else {
        //TODO save aOld and sOld -- does it help?
    }

    for (i = 0; i < numUSV; i++) {
        ii = USV[i];
        if (shrink->NSVcount[ii] >= h) {
            //this vector is probably NSV
            NSV[numNSV] = ii;
            if (!is_linear) {
                shrink->histID[ii] = shrink->hcount;
            }
            numNSV++;
        } else if (shrink->BSVcount[ii] >= h) {
            //this vector is probably BSV
            BSV[numBSV] = ii;
            if (!is_linear) {
                shrink->histID[ii] = shrink->hcount;
            }
            numBSV++;
        } else {
            //we keep this vector in training set
            USV[j] = ii;
            j++; newnumUSV++;
        }
    }

    if (!is_linear) {
        if (cache.Full())
            cache.Shrink(newnumUSV, USV);

        if (verbosity > 1) printf("[%d:%d]", shrink->hcount, len - newnumUSV);
        shrink->hcount++;
    } else {
        if (verbosity > 1) printf("[%d]", len - newnumUSV);
    }

    numUSV = newnumUSV;
}

void TSVMQPSolver::shrinkReactivate() {
    int i, j;
    int len = docs->Len();
    int dim = docs->Dim();

    //recalculate gradV
    if (is_linear) {
        double res;

        for (i = 0; i < dim; i++) weightV[i] = 0.0;
        //and upgrade as necessary
        for (i = 0; i < len; i++) {
            if (shrink->aOld[i] != alphaV[i]) {
                res = (alphaV[i] - shrink->aOld[i]) * y[i];
                docs->AddVec(i, weightV, dim, res);
                //docs->vecSet[i]->AddVec(weightV, dim, res);
            }

            shrink->aOld[i] = alphaV[i];
        }

        //now we recalculate gradV
        for (i = 0, j = 0; i < len; i++) {
            if (USV[j] > i)
                gradV[i] = shrink->sOld[i] + docs->DotProduct(i, weightV, dim);
                //docs->vecSet[i]->DotProduct(weightV, dim);
            else
                j++;

            shrink->sOld[i] = gradV[i];
        }
    } else {
        int k, kk;
        PFltV a_old;
        double res;
        int numInactive;    //number of variables that were
                            //deactiavted in i-th iteration
        int *inactiveIds = new int[len];    //ids of this variables
        double *col = new double[len];      //for storing columns from matrix

        for (i = shrink->hcount - 1; i >= 0; i--) {
            if (verbosity > 1) printf("..%d", i);
            a_old = shrink->aHist[i];

            //we find variables that were shrinked in i-th turn
            numInactive = 0;
            for (j = 0; j < len; j++) {
                if (shrink->histID[j] == i) {
                    inactiveIds[numInactive] = j;
                    numInactive++;
                    shrink->histID[j] = -1;
                }
            }

            //goes over all changed alphas and updates gradient s
            for (j = 0; j < len; j++) {
                a_old->Len();
                if (a_old->GetVal(j) != alphaV[j]) {
                    readColumnRows(j, numInactive, inactiveIds, col, false, false);

                    res = (alphaV[j] - a_old->GetVal(j)) * y[j];
                    //updates s that were shrinked in i-th iteration
                    for (k = 0; k < numInactive; k++) {
                        kk = inactiveIds[k];
                        gradV[kk] += res * col[k];
                    }
                }
            }
        }

        delete[] col;
        delete[] inactiveIds;

        shrink->hcount = 0;
        shrink->aHist.Reserve(100,0);
    }

    //activate all variables
    numUSV = len; numBSV = numNSV = 0;
    for (i = 0; i < len; i++) {
        USV[i] = i;
        shrink->BSVcount[i] = shrink->NSVcount[i] = 0;
        listV[i] = gradV[i] + y[i] * p[i];;
    }
}

// for measuring time
TSVMQPSolver::TSVMTime TSVMQPSolver::GetCurrentTime() {
#ifdef GLib_WIN32
    FILETIME lpCreationTime; // process creation time
    FILETIME lpExitTime;     // process exit time
    FILETIME lpKernelTime;   // process kernel-mode time
    FILETIME lpUserTime;     // process user-mode time

    GetProcessTimes(GetCurrentProcess(), &lpCreationTime, &lpExitTime,
                                         &lpKernelTime, &lpUserTime);

    return lpUserTime.dwLowDateTime +
           ((unsigned __int64)lpUserTime.dwHighDateTime << 32);
#elif defined(GLib_UNIX)
    return TSysTm::GetCurUniMSecs();            // !bn: mogoce bi tole kr za default uporabl? al pa bl natancno verzijo tega
#endif
}

inline double TSVMQPSolver::ToSeconds(TSVMTime time) {
    return time / 1e7;
}
//////////////////////////////////////////////////////////////////////////
// TSVMQPSolver::Heap
template <class TVal>
void TSVMQPSolver::THeap<TVal>::Add(const TVal& elt) {
    n++; int i = n;
    while (i > 1 && array[i/2] > elt) {
        array[i] = array[i/2]; i = i/2;
    }
    array[i] = elt;
}

template <class TVal>
void TSVMQPSolver::THeap<TVal>::ChangeTop(const TVal& elt) {
    int left, right, smallest, i = 1; TVal tmp; array[1] = elt;
    forever {
        left = 2*i; right = 2*i + 1;

        if (left <= n && elt > array[left])
            smallest = left;
        else
            smallest = i;

        if (right <= n && array[smallest] > array[right])
            smallest = right;

        if (smallest != i) {
            tmp = array[i];
            array[i] = array[smallest];
            array[smallest] = tmp;

            i = smallest;
        } else break;
    }
}

//////////////////////////////////////////////////////////////////////////
// Linear-Large-Scale-SVM
void TSVMLargeScale::Solve(PSVMTrainSet TrainSet, const double& SvmCost,
        const double& AccuracyEps, const int& MxTime, const bool& TillMxIter,
        const int& ProcN, TFltV& WgtV, PNotify Notify) {

    // asserts for input parameters
    EAssertR(TrainSet->Len() > 0, "Empty training set!");
    EAssertR(SvmCost > 0.0, "Cost parameter must be nonzero!");
    EAssertR(AccuracyEps > 0.0, "Accuaryc epsilon must be nonzero!");

    // initialization
    const int Dims = TrainSet->Dim(); // dimension of space
    const int Vecs = TrainSet->Len(); // number of training documents
    const double InvVecs = 1.0 / double(Vecs); // inverse of number of docs
    WgtV.Gen(Dims); WgtV.PutAll(0.0); // noraml vector
    double Slack = 0.0; // slack variable ksi
    TIntV ConstrLenV; // number of nonzero elements in each constraint
    TVec<TFltV> ConstrVecsVV; // vectors x_c for each constraint in dual
    // parameters for the dual quatratic program
    TVVec<double> QPQuad; TVec<double> QPLin, QPConstr;

    // estimation of maximal number of iterations
    double MxVecNorm2 = 0.0;
    for (int VecId = 0; VecId < Vecs; VecId++) {
        MxVecNorm2 = TMath::Mx(MxVecNorm2, TrainSet->GetNorm2(VecId)); }
    int MxIters = TFlt::Round(TMath::Mx(2/AccuracyEps, (8*SvmCost*MxVecNorm2)/TMath::Sqr(AccuracyEps)));
    Notify->OnStatus(TInt::GetStr(MxIters, "Maximal number of iterations: %d"));

    // main iteration loop
    int IterN = 0; TExeTm Timer;
    forever {
        // solve dual for current subset of constraints from ConstraintVV
        // as a result we get a new vector WgtV and variable Slack


        // find next constraint and main part of stoping criteria
        ConstrVecsVV.Add(TFltV()); TFltV& ConstrVecsV = ConstrVecsVV.Last();
        ConstrVecsV.Gen(Vecs); ConstrVecsV.PutAll(0.0);
        int ConstrLen = 0; double ConstrSum = 0.0;
        for (int VecId = 0; VecId < Vecs; VecId++) {
            const double VecParam = TrainSet->GetVecParam(VecId);
            const double Val = VecParam * TrainSet->DotProduct(VecId, WgtV);
            if (Val < 1.0) {
                // add VecId to the new constrant
                TrainSet->AddVec(VecId, ConstrVecsV, VecParam);
                ConstrLen++; ConstrSum += Val;
            }
        }
        // finish the new constraint
        ConstrLenV.Add(ConstrLen);
        for (int DimN = 0; DimN < Dims; DimN++) {
            ConstrVecsV[DimN] = InvVecs * ConstrVecsV[DimN]; }

        // prepare next dual QP linear parameters
        QPLin.Add(-  InvVecs * double(ConstrLen));
        // prepare next dual QP quadratic parameters
        TVVec<double> NewQPQuad(IterN+1, IterN+1);
        for (int i = 0; i < IterN; i++) {
            for (int j = 0; j < IterN; j++) {
                NewQPQuad(i,j) = QPQuad(i,j); }
        }
        for (int i = 0; i < IterN; i++) {
            const double EltVal = TLinAlg::DotProduct(ConstrVecsVV[i], ConstrVecsV);
            NewQPQuad(IterN, i) = EltVal; NewQPQuad(i, IterN) = EltVal;
        }
        NewQPQuad(IterN, IterN) = TLinAlg::DotProduct(ConstrVecsV, ConstrVecsV);
        QPQuad = NewQPQuad;
        // prepare next dual QP sum constraint
        QPConstr.Add(1.0);

        // calculate stopping crietria
        const double StopCrit = InvVecs*ConstrLen - InvVecs*ConstrSum;
        if (StopCrit > (Slack + AccuracyEps)) { break; } // we reached the accuracy
        if ((MxTime != -1) && (Timer.GetSecs() > MxTime)) { break; } // we ran out of time
        IterN++; if (TillMxIter && (IterN > MxIters)) { break; } // enough iterations
        // status notification
        Notify->OnStatus(TStr::Fmt("[IterN:%5d, Accr:%8.5f, Time:%5d s]\r",
            IterN, (StopCrit - Slack), Timer.GetSecs()));
    };
}


//////////////////////////////////////////////////////////////////////////
// SVM Factory
void TSVMFactory::trainClassifier(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const double& C, const double& j, const TSVMLearnParam& LearnParam) {

    IAssert(C > 0.0 && j > 0.0);
    int len = docs->Len();
    TFltV p(len), y(len), CV(len);
    for (int i = 0; i < len; i++) {
        p[i] = -1;
        y[i] = docs->GetVecParam(i);
        CV[i] = (y[i] > 0) ? C*j : C;
    }

    int h = 10, minNumElts = is_linear ? 10 : 100;
    TSVMQPSolver solver(is_linear, ker, LearnParam.Verbosity,
        LearnParam.EpsTer, LearnParam.Shrink, h, minNumElts);
    solver.optimize(alphas, threshold, docs, p, y, 0, CV,
        LearnParam.SubSize, LearnParam.MemSize, LearnParam.Time);
}

void TSVMFactory::trainOneClass(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const double& nu, const TSVMLearnParam& LearnParam) {

    IAssert(0.0 < nu && nu < 1.0);

    int len = docs->Len();
    double D = nu*len;
    TFltV p(len), y(len), C(len);
    for (int i = 0; i < len; i++) {
        p[i] = 0;
        y[i] = 1;
        C[i] = 1.0;
    }

    int h = 10, minNumElts = is_linear ? 10 : 100;
    TSVMQPSolver solver(is_linear, ker, LearnParam.Verbosity,
        LearnParam.EpsTer, LearnParam.Shrink, h, minNumElts);
    solver.optimize(alphas, threshold, docs, p, y, D, C,
        LearnParam.SubSize, LearnParam.MemSize, LearnParam.Time);
}

void TSVMFactory::trainRegression(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const double& E, const double& C, const TSVMLearnParam& LearnParam) {

    IAssert(E > 0.0 && C > 0.0);

    int len = docs->Len();
    TIntV IdV(2*len, 0);
    TFltV p(2*len), y(2*len), CV(2*len);
    for (int i = 0; i < len; i++) {
        p[i] = E + docs->GetVecParam(i);
        y[i] = 1; CV[i] = C; IdV.Add(i);
    }
    for (int i = len; i < 2*len; i++) {
        p[i] = E - docs->GetVecParam(i-len);
        y[i] = -1; CV[i] = C; IdV.Add(i-len);
    }

    PSVMTrainSet docs2 = TSVMTrainSubSet::New(docs, IdV);
    int h = 10, minNumElts = is_linear ? 10 : 100;
    TSVMQPSolver solver(is_linear, ker, LearnParam.Verbosity,
        LearnParam.EpsTer, LearnParam.Shrink, h, minNumElts);
    solver.optimize(alphas, threshold, docs2, p, y, 0, CV,
        LearnParam.SubSize, LearnParam.MemSize, LearnParam.Time);
}

void TSVMFactory::train(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const TSVMModelParam& ModelParam, const TSVMLearnParam& LearnParam) {

    if (ModelParam.ModelType == smtClassifier) {
        trainClassifier(alphas, threshold, is_linear, ker, docs, ModelParam.C, ModelParam.j, LearnParam);
    } else if (ModelParam.ModelType == smtRegression) {
        trainRegression(alphas, threshold, is_linear, ker, docs, ModelParam.E, ModelParam.C, LearnParam);
    } else if (ModelParam.ModelType == smtOneClass) {
        trainOneClass(alphas, threshold, is_linear, ker, docs, ModelParam.nu, LearnParam);
    }
}
