//////////////////////////////////////////////////////////////////////////
#define EPSILON 1e-4

typedef TVec<double> TVecDbl;
typedef TVec<int> TVecInt;
typedef TVec<bool> TVecBool;

//////////////////////////////////////////////////////////////////////////
// Floating-Point-Type
//   Floating point type used to store real numbers in cache. Float
//   is less accurate than double but also half the size of double
typedef float TFltType;

//////////////////////////////////////////////////////////////////////////
// SVM-Cache-Vector
//   Represents one vector stored in cache. It only contains a pointer
//   to cache where components are stored, doesn't alocate any memory!
class TSVMCacheVec {
public:
    //Points to memory location in cache, where this vector is stored
    TFltType *valT;
    //For each element in array cache it tells if its value valid
    bool *validT;
    /** For each i idsT[i] is the position of i-th element of
        this vector in valT. If element is not in valT, than
        idsT[i] == -1. Example: we want to read j-th element from
        vector. Than: if idsT[i] == -1, we don't have this element
        in cache; otherwise valT[idsT[i]] is that element. */
    int *idsT;
    //Dimension of space this vectors lives in
    int dim;
public:
    TSVMCacheVec(): valT(NULL), idsT(NULL), dim(0) {};
    TSVMCacheVec(TFltType *_valT, bool* _validT, int *_idsT, int _dim):
            valT(_valT), validT(_validT), idsT(_idsT), dim(_dim) {}

    TSVMCacheVec(const TSVMCacheVec& vec):
            valT(vec.valT), validT(vec.validT), idsT(vec.idsT), dim(vec.dim) {}
    TSVMCacheVec& operator=(const TSVMCacheVec& vec) {
        valT = vec.valT; validT = vec.validT; idsT = vec.idsT; dim = vec.dim;
        return *this;
    }
};

//////////////////////////////////////////////////////////////////////////
// SVM-Cache
//   Implements cache for SVM. At beginning gets as a parameter how much
//   memory it can use and dimension of vectors it has to store. From that
//   calculates number of vectors it can store at once. If QPSolver uses
//   shrinking, than chache reorganizes it's structure acording to it.
//   Elements in cache are stored acording to LRU. To keep track of this
//   cache has a list of all vectors stored organized by their use. When
//   vector is read from cache it's put on top, same when new vector is
//   added to cache. For faster searching on this list we use vector
//   vecPt, i-th element of this vector points to a node of -th
//   vector in list.
class TSVMCache {
private:
    //How talkative is cache
    int verbosity;

    //Memory reservated for cache
    TFltType *cache;
    //For each element of cache tells if still valid
    bool *valid;
    //Number of rows we keep per column
    int numRows;
    /** i-th element has position of i-th row in cache vector
        if not in cache, than value == -1 */
    int *idsT;

    /** Structure of nodes used for linked list organized
        in Most-Receantly-Used way. Each node represent one
        vector in cache. When we read vector, it's moved to
        top of this list*/
    struct TListNode {
        int ID;
        int position;
        TFltType *addr;
        bool *vaddr;
        TListNode *prev;
        TListNode *next;
    };

    //Points to top of linked list
    TListNode *top;
    //Points to bottom of linked list
    TListNode *bottom;

    //i-th elements points to node in list belonging to i-th vector
    TVec<TListNode *> vecPt;
    /** i-th elements points to node in list belonging to vector stored
        on (i*numRows)-th place in cache. This list is used when
        reoganizing cahce after shrinking. */
    TVec<TListNode *> vecShrink;
    //Tells if element is in cache
    TBoolV saved;

    //Current number of vectors in CACHE
    int size;
    //Max size of vectors in CACHE
    int maxsize;
    //Dimension of vectors (actually dim x dim is dimesnion of matrix)
    int dim;

    /** Moves node to top of list.
        @param node node, we want to move to top of the list */
    void toTop(TListNode *node);

    //copy constructor and some operators we don't want to use
    UndefCopyAssign(TSVMCache);
public:
    /** Makes empty CACHE. Before using it you have to call method Reset
        @param verb how talkative is cache */
    TSVMCache(int verb = 0):
        verbosity(verb), cache(NULL), valid(NULL),
        numRows(0), idsT(NULL), top(NULL), bottom(NULL), vecPt(0),
        vecShrink(0), saved(0), size(0), maxsize(0), dim(0) {}

    /** Initializes cache.
        @param memory_size how much space can we reservate for CACHE (in MB)
        @param d dimension of vectors (d * d is size of matrix Q!)
        @param verb how talkative is cache */
    TSVMCache(const int& memory_size, const int& d, int verb):
        verbosity(verb) { Reset(memory_size, d); }

    ~TSVMCache() { Clear(); }

    /** Resets CACHE.
        @param memory_size how much space can we reservate for CACHE (in MB)
        @param d dimension of vectors (d * d is size of matrix Q!) */
    void Reset(const int& memory_size, const int& d);
    /** Dealocates all memory used. CACHE can be reused by calling Reset.
        Clear puts CACHE to same state it is after calling TSVMCache(). */
    void Clear();

    //Tells if column col is in CACHE
    bool Exists(const int& col) const
        { Assert(col >= 0 && col < dim); return saved[col]; }
    /** Tells if value stored for row-th element of col-th vector is valid
        @param col number of vector (column)
        @param row number of element (row) */
    bool IsRowValid(const int &col, const int &row) const {
        Assert(0 <= col && col < dim && 0 <= row && row < dim);
        if (saved[col])
            return idsT[row] >= 0 ? vecPt[col]->vaddr[idsT[row]] : false;
        else
            return false;
    }

    //Maximal number of elements CACHE can store
    int MaxSize() { return maxsize; }
    //Current number of elements stored in CACHE
    int CurrentSize() { return size; }
    //Is CACHE full?
    bool Full() { return maxsize == size; }

    /** Reorganizes cache acodring shrinking. We rewrite cache so it can
        store more vectors. To achive that we throw away some rows, we
        keep only ones in ids.
        @param n number of rows still relevant for QPSolver (numUSV)
        @param ids IDs of rows still relevant for QPSolver (USV -
                   Unbounded Support Vectors) */
    void Shrink(const int& n, int *ids);

    //Returns i-th column
    TSVMCacheVec Get(const int& i);
    //Returns j-th element from i-th column
    TFltType GetVal(const int& i, const int& j) const;
    /** Adds vector to CACHE. It reservates space in it, adds it to
        linked list and returns pointer to it's position.
        This function only reservates space, elements of vectors
        must be added by the one who calles this functions. Until
        than values of elements of this vector are undefined!
        @param i ID of vector we want to add to CACHE.
        @return TSVMCacheVec with pointer to vector's place in cache */
    TSVMCacheVec Add(const int& i);
};

//////////////////////////////////////////////////////////////////////////
// SVM-Train-Set
typedef enum {ststSparse, ststDense, ststBow, ststString, ststSimMatrix} TSVMTrainSetType;

ClassTP(TSVMTrainSet, PSVMTrainSet) //{
protected:
    typedef PSVMTrainSet (*TSVMTrainSetLoad)(TSIn& SIn);
    typedef TFunc<TSVMTrainSetLoad> TSVMTrainSetLoadF;
    static THash<TStr, TSVMTrainSetLoadF> TypeToLoadFH;
    static bool Reg(const TStr& TypeNm, const TSVMTrainSetLoadF& LoadF);
private:
    TInt SVMTrainSetType;
    UndefDefaultCopyAssign(TSVMTrainSet);
private:
    TIntFltKdV FailReturnIntFltKdV;
    TFltV FailReturnFltV;
    TStr FailReturnStr;
    TIntV FailReturnIntV;
public:
    TSVMTrainSet(const TSVMTrainSetType& _Type): SVMTrainSetType(_Type) { };
    virtual ~TSVMTrainSet() { };

    TSVMTrainSet(TSIn& SIn) { SVMTrainSetType.Load(SIn); }
    static PSVMTrainSet Load(TSIn &SIn);
    static PSVMTrainSet LoadBin(const TStr& FNm) {
        TFIn FIn(FNm); return TSVMTrainSet::Load(FIn); }

    virtual void Save(TSOut& SOut) const {
        GetTypeNm(*this).Save(SOut); SVMTrainSetType.Save(SOut); }
    void SaveBin(const TStr& FNm) { TFOut FOut(FNm); Save(FOut); }

    virtual void SaveTxt(const TStr& OutFNm) const { Fail; }
    virtual void SaveMatlab(const TStr& OutFNm) const { Fail; }

    TSVMTrainSetType Type() const { return TSVMTrainSetType(SVMTrainSetType.Val); }

public: // functions which must be written each time
    //number of vectors in set
    virtual int Len() const = 0;
    //dimension of space
    virtual int Dim() const = 0;
    //square of euclidian norm of vector
    virtual double GetNorm2(const int& VecId) const = 0;
    //parameter of this vector (-1 or 1 at SVM, value at SVR)
    virtual double GetVecParam(const int& VecId) const = 0;

    //dot product between two vectors in set
  virtual double DotProduct(const int& VecId1, const int& VecId2) const = 0;
  //dot product between vector in set and vector vec2 of length n
    virtual double DotProduct(const int& VecId1, double* vec2, const int& n) const = 0;
  //dot product between vector in set and vector vec2
    virtual double DotProduct(const int& VecId1, const TFltV& vec2) const = 0;

  //adds vector * K to vector vec2 of length n ( vec := vec + K*this )
  virtual void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const = 0;
    //adds vector * K to vector vec2 ( vec := vec + K*this )
  virtual void AddVec(const int& VecId1, TFltV& vec2, const double& K) const = 0;

    //gets a clone of the train set (necessary for remembering support vectors)
    virtual PSVMTrainSet Clone(const TIntV& VecIdV) const = 0;

public: // supportive functions, not all need to be written
    // functions for adding vectors
    virtual int AddAttrV(const TIntFltKdV& AttrV, const double& Cls, const bool& Normalize) { Fail; return 0; }
    virtual int AddAttrV(const TFltV& AttrV, const double& Cls, const bool& Normalize) { Fail; return 0; }
    virtual int AddAttrV(const int& DId, PBowSpV AttrV, const double& Cls) { Fail; return 0; }
    virtual int AddAttrV(const TStr& AttrV, const double& Cls) { Fail; return 0; }
    virtual int AddAttrV(const TIntV& AttrV, const double& Cls) { Fail; return 0; }
    // functions for reading vectors in the set
    virtual const TIntFltKdV& GetAttrSparseV(const int& VecId) const { Fail; return FailReturnIntFltKdV; }
    virtual const TFltV&      GetAttrDenseV(const int& VecId) const { Fail; return FailReturnFltV; }
    virtual PBowSpV           GetAttrBowV(const int& VecId) const { Fail; return NULL; }
    virtual int               GetVecDId(const int& VecId) const { Fail; return 0;  }
    virtual const TIntV&      GetAttrStringV(const int& VecId) const { Fail; return FailReturnIntV; }
    // remove vector
    virtual void RemoveAttrV(const int& VecId) { Fail; }
    // function for setting vector parameters
    virtual void SetVecParam(const int& VecId, const double& NewCls) { Fail; }
    // dot products with vectors of specific types
    virtual double DotProduct(const int& VecId1, const TIntFltKdV& Vec2) const { Fail; return 0.0; }
    virtual double DotProduct(const int& VecId1, PBowSpV Vec2) const { Fail; return 0.0; }
    virtual double DotProduct(const int& VecId1, const TStr& Vec2) const { Fail; return 0.0; }
    virtual double DotProduct(const int& VecId1, const TIntV& Vec2) const { Fail; return 0.0; }
    // shuffels vectors
    void Shuffle(TRnd& Rnd) { Fail; }

public: // extra stuff, independent of the implementation

    // calculates linear combination of vectors in this training
    // set using coeficients from vector AlphV
    void LinComb(const TFltV& AlphV, TFltV& Result);
    // calculates linear combination of a subset of vectors in
    // this training set using coeficients from vector AlphV
    void LinComb(const TIntV& VecIdV, const TFltV& AlphV, TFltV& Result);

    // calculates euclidian distance between two vectors
    double EuclDist(const int& VecId1, const int& VecId2) {
        return GetNorm2(VecId1) - 2*DotProduct(VecId1, VecId2) + GetNorm2(VecId2); }

    // calculates which elements in normal vector are on average the most
    // important when classifying a document. Sign which documents
    // to check (+1 for positive, -1 for negative) and SortDirection tells
    // from what side it returns the keywords. For example, for paramteres
    // Sign = 1.0 and SortDirection = false this function tells which elements
    // in the normal vector are most strongly voting that positive document
    // is positive. For parameters Sing = 1.0 and SortDirection = true tells
    // what elements are most strongly voting against a positive doucment
    // being classified as positive (what are the must trubled attributes)
    void GetKeywords(const TFltV& NormalV, TIntFltKdV& WIdWgtV,
        const TIntV& VecIdV = TIntV(), const int& Wd = 10,
        const double& VecSign = 1.0, const double& WgtSign = 1.0,
        const bool& AvgOverSet = true);

    // calculates parameter j for SVM
    //   j = PosDocs/AllDocs
    double CalcSvmUnblParam(const double& MxVal = 10, const double& MnVal = 0.1);

    // counts vectors with pareameter of given sign
    int GetSignVecs(const double& Sign);
    // returns true if there is at least MnDoc of positive
    // and of negative vectors in this train set
    bool HasPosNegVecs(const int& MnVec);
    // returns true if there is at least MnDoc of positive
    bool HasPosVecs(const int& MnPosVec);
    bool HasNegVecs(const int& MnNegVec);
};

//////////////////////////////////////////////////////////////////////////
// Training-Sub-Set
class TSVMTrainSubSet : public TSVMTrainSet {
private:
    PSVMTrainSet TrainSet;
    TIntV SubSetIdV;
public:
    TSVMTrainSubSet(PSVMTrainSet TrSet, const TIntV& TrVecIdV):
      TSVMTrainSet(TrSet->Type()), TrainSet(TrSet), SubSetIdV(TrVecIdV) {};
    static PSVMTrainSet New(PSVMTrainSet TrSet, const TIntV& TrVecIdV) {
        return new TSVMTrainSubSet(TrSet, TrVecIdV); }

    int Len() const { return SubSetIdV.Len(); }
    int Dim() const { return TrainSet->Dim(); }
    double GetNorm2(const int& VecId) const { return TrainSet->GetNorm2(SubSetIdV[VecId]); }
    double GetVecParam(const int& VecId) const { return TrainSet->GetVecParam(SubSetIdV[VecId]); }

    double DotProduct(const int& VecId1, const int& VecId2) const {
        return TrainSet->DotProduct(SubSetIdV[VecId1], SubSetIdV[VecId2]); }
    double DotProduct(const int& VecId1, double* vec2, const int& n) const {
        return TrainSet->DotProduct(SubSetIdV[VecId1], vec2, n); }
    double DotProduct(const int& VecId1, const TFltV& vec2) const {
        return TrainSet->DotProduct(SubSetIdV[VecId1], vec2); }

    void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const {
        TrainSet->AddVec(SubSetIdV[VecId1], vec2, n, K); }
  void AddVec(const int& VecId1, TFltV& vec2, const double& K) const {
        TrainSet->AddVec(SubSetIdV[VecId1], vec2, K); }

    PSVMTrainSet Clone(const TIntV& VecIdV) const { Fail; return NULL; };
};

///////////////////////////////////////////////////////////////////////
// Sparse-Train-Set-Column-Matrix
class TSTSetMatrix: public TMatrix {
private:
  PSVMTrainSet ColVV;
protected:
  // Result = A * B(:,ColId)
  virtual void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const;
  // Result = A * Vec
  virtual void PMultiply(const TFltV& Vec, TFltV& Result) const;
  // Result = A' * B(:,ColId)
  virtual void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const;
  // Result = A' * Vec
  virtual void PMultiplyT(const TFltV& Vec, TFltV& Result) const;

  int PGetRows() const { return ColVV->Dim(); }
  int PGetCols() const { return ColVV->Len(); }

public:
  TSTSetMatrix(PSVMTrainSet Set): TMatrix(), ColVV(Set) {
    IAssert(Set->Type() == ststSparse); };
  TSTSetMatrix(PSVMTrainSet Set, TFltV& ClsV);

  void Load(TSparseColMatrix& Matrix, TFltV& y, PSVMTrainSet TrainSet,
    const double& PWgt = 0.99, const double& NWgt = -0.99);
};

//////////////////////////////////////////////////////////////////////////
// Kernel: Virtual class representing kernel function.
ClassTP(TKernel, PKernel)//{
protected:
  typedef PKernel (*TKernelLoad)(TSIn& SIn);
  typedef TFunc<TKernelLoad> TKernelLoadF;
  static THash<TStr, TKernelLoadF> TypeToLoadFH;
  static bool Reg(const TStr& TypeNm, const TKernelLoadF& LoadF);
public:
  virtual ~TKernel(){}
  //calculates kernel with vectors v1 and v2
  virtual double CalcSet(const PSVMTrainSet& set,
    const int& VecId1, const int& VecId2) const = 0;
  virtual double Calc(const double& DotProduct,
  const double& Norm1, const double& Norm2) const = 0;

  static PKernel Load(TSIn& SIn);
  virtual void Save(TSOut& SOut) {
    GetTypeNm(*this).Save(SOut); }
};

//////////////////////////////////////////////////////////////////////////
// Polynomial-Kernel: K(a,b) = (s*(a'*b) + c)^p
class TPolynomialKernel: public TKernel {
private:
  static bool IsReg;
  static bool MkReg(){return TKernel::Reg(TTypeNm<TPolynomialKernel>(), &Load);}
private:
    double s, c, p;
public:
    TPolynomialKernel(double _p = 3.0, double _s = 1.0,
        double _c = 1.0): s(_s), c(_c), p(_p) {}
    // K(a,b) = (s*(a'*b) + c)^p
    static PKernel New(double _p = 3.0, double _s = 1.0, double _c = 1.0) {
        return PKernel(new TPolynomialKernel(_p, _s, _c)); }
    static PKernel NewLinKernel() {
        return PKernel(new TPolynomialKernel(1.0, 1.0, 0.0)); }
    explicit TPolynomialKernel(TSIn& SIn) {
        SIn.Load(s); SIn.Load(c); SIn.Load(p); }
    static PKernel Load(TSIn& SIn){
        return PKernel(new TPolynomialKernel(SIn));}
    void Save(TSOut& SOut) {
        TKernel::Save(SOut); SOut.Save(s); SOut.Save(c); SOut.Save(p); }

    //calculates kernel with vectors v1 and v2
    double CalcSet(const PSVMTrainSet& set, const int& VecId1, const int& VecId2) const {
        return pow(s * set->DotProduct(VecId1, VecId2) + c, p); }
    double Calc(const double& DotProduct, const double& Norm1, const double& Norm2) const {
        return pow(s * DotProduct + c, p); }
};

//////////////////////////////////////////////////////////////////////////
// Radial-Kernel: K(a,b) = exp(-gamma ||a-b||^2)
class TRadialKernel: public TKernel {
private:
  static bool IsReg;
  static bool MkReg(){return TKernel::Reg(TTypeNm<TRadialKernel>(), &Load);}
private:
    double gama;
public:
    TRadialKernel(double _gama = 1.0): gama(_gama) {}
    static PKernel New(double _gama = 1.0) {
        return PKernel(new TRadialKernel(_gama)); }
    explicit TRadialKernel(TSIn& SIn) { SIn.Load(gama); }
    static PKernel Load(TSIn& SIn){
        return PKernel(new TRadialKernel(SIn));}
    void Save(TSOut& SOut) {
        TKernel::Save(SOut); SOut.Save(gama); }

    //calculates kernel with vectors v1 and v2
    double CalcSet(const PSVMTrainSet& set, const int& VecId1, const int& VecId2) const {
        return exp(-gama * (set->GetNorm2(VecId1) + set->GetNorm2(VecId2) -
                   2*set->DotProduct(VecId1, VecId2)));
    }
    double Calc(const double& DotProduct, const double& Norm1, const double& Norm2) const {
        return exp(-gama * (Norm1 + Norm2 - 2*DotProduct)); }
};

//////////////////////////////////////////////////////////////////////////
// Sigmoid-Kernel: K(a,b) = tanh(s a*b + c)
class TSigmoidKernel: public TKernel {
private:
  static bool IsReg;
  static bool MkReg(){return TKernel::Reg(TTypeNm<TSigmoidKernel>(), &Load);}
private:
    double s, c;
public:
    TSigmoidKernel(double _s = 1.0, double _c = 1.0): s(_s), c(_c) {}
    static PKernel New(double _s = 1.0, double _c = 1.0) {
        return PKernel(new TSigmoidKernel(_s, _c)); }
    explicit TSigmoidKernel(TSIn& SIn) { SIn.Load(s); SIn.Load(c); }
    static PKernel Load(TSIn& SIn){
        return PKernel(new TSigmoidKernel(SIn));}
    void Save(TSOut& SOut) {
        TKernel::Save(SOut); SOut.Save(s); SOut.Save(c); }

    //calculates kernel with vectors v1 and v2
    double CalcSet(const PSVMTrainSet& set, const int& VecId1, const int& VecId2) const {
        return tanh(s * set->DotProduct(VecId1, VecId2) + c); }
    double Calc(const double& DotProduct, const double& Norm1, const double& Norm2) const {
        return tanh(s * DotProduct + c); }
};

//////////////////////////////////////////////////////////////////////////
// SVM-Quadratic-Problem-Solver for small problems
//   Not good for larger problems because it requiers to keep whole
//   matrix Q in memory while optimizing. Library pr_loqo (written
//   by Alex Smola) is used for solving the problem.
//   It can find minimum (where x is variable) for
//        c'*x + 1/2 x'*Q*x
//   subject to:
//        A*x = b
//        l <= x <= u
//   where: Q is n x n s.p.matrix, x and c are vectors of length n,
//          A is n x m matrix, b is vector of length m, u is a scalar.
//          In this case m == 1
class TSVMQP {
private:
    //number of primial variables
    int n;
    //number of constrains (typically 1)
    int m;
    //linear part
    double *c;
    //dot product matrix
    double *Q;
    //constrain matrix (v nasem primeru == y)
    double *A;

    //for input to pr_loqo
    double *pl;

    //locate place for results
    double *primal;
    //locate place for results
    double *dual;

    //copy constructor and some operators we don't want to use
    UndefDefaultCopyAssign(TSVMQP);
public:
    TSVMQP(const int& nn, const int& mm = 1, const double& ll = 0.0);
    ~TSVMQP();

    /** Solves given problem
         size -- n from formulation
         bb -- b from formulation (in our case only sclar)
         u -- u from formulation (vector!)
         Q -- Q from formulation
         A -- A from formulation (in our case only vector of lentght n)
         c -- c from formulation
         resultV -- pointer to array where result is stored */
    bool solve(const int& size, const double& bb, double *u,
               double *Q, double *A, double *c, double *resultV, const int& verbosity);
};

//////////////////////////////////////////////////////////////////////////
// SVM-Quadratic-Problem-Solver for big problems
//  Decomposes minimization problem and sends it to QP solver for small
//  problems. It also uses shrinking and special optimization for linear
//  case. It can solve problems in style
//       W(a) = 1/2 a'Qa + p'a
//       y'a = D and 0 <= a <= C,
//  where Q is matrix with elements
//       q_ij = y_i * y_j * kernel(x_i, x_j)
//  and y_i element of {-1, 1} for each i
//  Function kernel and vectors p, y, C and constant D are given
//  as input, output is vector a. x_i are elements of training set.
//     -- For more information about how this work, check my notes.
class TSVMQPSolver {
private:
    //data type used to store current state at shrinking
    struct TShrinkState {
        TShrinkState(): BSVcount(NULL), NSVcount(NULL), histID(NULL),
            aOld(NULL), sOld(NULL) {}
        ~TShrinkState() {
            if (BSVcount != NULL) delete[] BSVcount;
            if (NSVcount != NULL) delete[] NSVcount;
            if (histID != NULL) delete[] histID;
            if (aOld != NULL) delete[] aOld;
            if (sOld != NULL) delete[] sOld;
        }

        double epsilon;
        //For how many iterations one vector was BSV
        int *BSVcount;
        //For how many iterations one vector was NSV
        int *NSVcount;
        //number of current candidates for shrinking
        int count;
        //History for non-linear svm
        //History of alphas for faster checking of shrinking (non-linear case)
        TVec<PFltV> aHist;
        //In what set to look in history (non-linear case)
        int *histID;
        //Next empty place in history (non-linear case)
        int hcount;
        //History for linear svm
        //Alphas at last reactivation (linear case)
        double *aOld;
        //gradV at last reactivation time (linear case)
        double *sOld;
    };

    template <class TVal>
    class THeap {
    private:
        int size;
        int n;
        TVal *array;
    public:
        THeap(const int& ss): size(ss), n(0) {
            if (size > 0) array = new TVal[size+1];
            else array = NULL;
        }
        ~THeap() { if (array != NULL) delete[] array; }

        TVal Top() { Assert(n > 0); return array[1]; }
        bool Full() { return size == n; }
        int Size() { return n; }
        TVal* Array() { return array; }

        void Add(const TVal& elt);
        void ChangeTop(const TVal& elt);
    };

    struct TMaxHeapVal {
        int id; double val;
        bool operator>(const TMaxHeapVal& heapVal) const { return val > heapVal.val; }
    };
    struct TMinHeapVal {
        int id; double val;
        bool operator>(const TMinHeapVal& heapVal) const { return val < heapVal.val; }
    };

    typedef THeap<TMaxHeapVal> TMaxHeap;
    typedef THeap<TMinHeapVal> TMinHeap;

private:
    //How talkative is solver
    int verbosity;
    //when to stop optimizing
    double epsilon_ter;

    //if we're training linear SVM
    bool is_linear;
    //kernel
    PKernel nonlin_kernel;
    //for generating random working sets
    TRnd rnd;

    //lower varialbes are used at optimizing
    //pointer to documents we learn on
    PSVMTrainSet docs;
    //parameters of function
    //coeficients of linear part of QP function
    double *p;
    //normal of hyper-plane on which we search for soltion
    double *y;
    //constant parameter of hyper-plane y'x = D
    double D;
    //upper bound (cost parameter)
    double *C;
    //current solution
    //lagrange multipliers
    double *alphaV;
    //some other usefull things
    //trashold
    double base;
    //hyperplane (used in linear SVM)
    double *weightV;
    //part of gradient of objecive function (vector s in articles)
    //gradient[i] = y[i]*gradV[i] + p[i]
    double *gradV;
    //listV[i] := gradV[i] + p[i]*y[i] = y[i] *. gardient[i]
    double *listV;
    //id-s of documents that are optimizedin current iteration
    int *workingSet;
    //which vectors are already in working set
    bool *used;
    //holds columns from cache of elemennts in working set
    TVec<double *> cols;
    //number of iterations so far
    int iter;

    //tells if shrinking is activated
    bool shrinking;
    //parameters for guessing what to shrink
    TShrinkState *shrink;
    //unbounded support vectors
    int *USV;
    //bounded support vectors
    int *BSV;
    //non-support vectors
    int *NSV;
    //number of each type of vectors
    int numUSV, numBSV, numNSV;
    //number of iterations variable must pass to be recognized as B/NSV
    int h;
    //min. number of elemeants for shrinking
    int minNumElts;
    //max violation of KKT condition in current iteration
    double max_violation;

    //cache
    TSVMCache cache;
    //some statistics about cache
    int cache_yes, cache_no;
    //counts kernel evaluations
    int kernel_count;

    //threads
    //TSVMThreadPV ThreadV;
    //number of threads
    //int ThreadN;

    //calculates kernel function for i-th and j-th training vector
    double kernel(const int& i, const int& j) const;

    /** reads a column of Hessian matrix from cache, calculates
        it if it's not there. Doesn't return whole column, only
        n elements listed in array ids. Column is stored to
        array result. If column is not in cache, than it's calculated,
        if not all required elements of column are in cache, than they
        are calculated.
         i -- ID of column from matrix (or ID of row as matrix is
                 symetric
         len -- how many elements of column we want to read
                   (size of array ids)
         ids -- array with IDs of elements from column we want
         result -- array to which column is read
         store -- if column is not in cache already than:
                     if stored is true, than calculated column is stored
                     to cahce, if stored is false, than it's not.
         sync -- tells if elements of column we want are a subset
                    of elements we keep in cache (this is used becasue
                    of shrinking, we only shrink columns in cache if it's
                    full; when we need elements that are not neccesary all
                    stored in cache, than some slower algorithm is used!)*/
    void readColumnRows(const int& i, const int& len, int *ids,
                        double *result, const bool& store, const bool& sync);
    //reads-ith column from cache (it uses readColumnRows)
    void column(const int& i, double *result);
    //calculates using more processors (threads)
    void calcColumns(const int& n);

    /** initializes cache before optimization starts
         memory_size -- size in MB of how much memory cache can use
         size -- number of elements in training set (size of matrix
                    we'll store in cache) */
    void initializeCache(const int& memory_size, int& size);
    /** initializes main variables used at training.
         _pV -- vector p from formulation
         _yV -- vector y from formulation
         _D -- scalar D from formulation
         _CV -- scalar C from formulation
         size -- size of working set */
    void initialize(const TFltV& _pV, const TFltV& _yV,
                    const double& _D, const TFltV& _CV, const int& size);
    //dealocates memory allocated in initialize
    void dealocate();

    // initializes gradient for re-learning (used previsous result for initializing);
    void initRelearning(TFltV& alphas);
    // initializes gardient when D is nonzero (e.a. Oneclass svm)
    void initNonzeroD(const int& size);

    // initializes threads at startup (number of threads is in ThreadN)
    //void initializeThreads(const int& size);
    // kills all threads
    //void killThreads();

    /** selects n elements for working set using stepiest gradient
        or randomly
         n -- size of working set
         random -- if true, than working set is selected randomly*/
    int selectWorkingSet(const int& n, const bool& random);

    /** adjusts vector gradV after runing QP solver on n variables from
        workingSet
         n -- size of working set
         newAplhaV -- new values for n alphas for which we used
                         QP solver */
    void adjustS(const int& n, double* newAlphaV);

    /** checks if current set of alphas satisfies Khun-Tucker conditions
         leq -- lambda^eq from Torsten's article, otherwise TODO in
                   my notes*/
    double checkOptimality(double leq);

    //initializes data structures used for shrinking:
    void shrinkInitialize();
    /** checks optimality conditions (same as checkOptimailty plus
        conditions for shrinking for each active variable -- checks
        if it's close to 0 or C and increases counter in shrink
        structure for that variable).
         step -- how much we add to counter
         leq -- same as in checkOptimality */
    double shrinkCheckOptimality(int step, const double& leq);
    //removes variables that fulfill conditions from active set
    void shrinkReduce();
    //reactivates all variables and recalculates gradV
    void shrinkReactivate();

    //for measuring time
    typedef int64 TSVMTime;
    //returens current time (used for measuring time)
    TSVMTime GetCurrentTime();
    //chages from format TSVMTime to seconds
    double ToSeconds(TSVMTime time);

    //copy constructor and some operators we don't want to use
    UndefDefaultCopyAssign(TSVMQPSolver);
public:
    /** lin -- true if problem linear
        ker -- kernel function for case when problem is not linear
        verb -- how much info we output while optimizing
        _shrinking -- if true shrinking is used at optimizing
        _h -- number of iterations variable must pass to be
                  recognized as B/NSV
        _minNumElts -- min. number of elemeants for shrinking */
    TSVMQPSolver(const bool& lin, const PKernel& ker, const int& verb = 0,
      const double& eps_ter = 1e-3, const bool& _shrinking = true,
      const int& _h = 10, const int& _minNumElts = 10):
        verbosity(verb),
        epsilon_ter(eps_ter), is_linear(lin), nonlin_kernel(ker),
        shrinking(_shrinking), h(_h), minNumElts(_minNumElts), cache(verbosity) {};

    /** Finds minimum for given problem
        alphas -- vector to which calculated alphas will be stored
        _base -- place to which b will be stored (also called leq upper)
        _docs -- traing set
        _pV vector -- p from formulation
        _yV vector -- y from formulation
        _D scalar -- D from formulation
        _CV scalar -- C from formulation
        sub_size -- size of subproblems that are sent to TSVMQP
        memory_size -- size of memory (in MB) cache can use
        time -- limit for learning, if minimun is not find in this time,
                    optimization stops and returns an exception */
    void optimize(TFltV& alphas, double& _base, const PSVMTrainSet& _docs,
            const TFltV& _pV, const TFltV& _yV, const double& _D,
            const TFltV& _CV, const int& sub_size, const int& memory_size,
            const int& time);
};

//////////////////////////////////////////////////////////////////////////
// Linear-Large-Scale-SVM
class TSVMLargeScale {
public:
    static void Solve(PSVMTrainSet TrainSet, const double& SvmCost,
        const double& AccuracyEps, const int& MxTime, const bool& TillMxIter,
        const int& ProcN, TFltV& WgtV, PNotify Notify = TStdNotify::New());
};

//////////////////////////////////////////////////////////////////////////
// SVM-Learning-Parameters
//   Parameters controling optimization performance
class TSVMLearnParam {
public:
    TInt Time;    // maximal time for learning
    TInt MemSize; // size of cache in MB
    TInt Verbosity;
    TInt SubSize; // subproblem size
    TFlt EpsTer;  // KKT condition valiation threshold
    TBool Shrink; // do shrinking
public:
    TSVMLearnParam(const int& _Time = -1, const int& _MemSize = 50, const int _Verbosity = 0,
        const int& _SubSize = 10, const double& _EpsTer = 1e-3, const bool& _Shrink = true):
            Time(_Time), MemSize(_MemSize), Verbosity(_Verbosity),
            SubSize(_SubSize), EpsTer(_EpsTer), Shrink(_Shrink) { };

    TSVMLearnParam(TSIn &SIn): Time(SIn), MemSize(SIn), Verbosity(SIn),
        SubSize(SIn), EpsTer(SIn), Shrink(SIn) { }
    static TSVMLearnParam Load(TSIn &SIn) { return TSVMLearnParam(SIn); }
    void Save(TSOut &SOut) const {
        Time.Save(SOut); MemSize.Save(SOut); Verbosity.Save(SOut);
        SubSize.Save(SOut); EpsTer.Save(SOut); Shrink.Save(SOut);
    }

    // typical parameters for linear svm
    static TSVMLearnParam Lin(const int& _Time = -1, const int _Verbosity = 0,
        const int& _SubSize = 30, const double& _EpsTer = 1e-3, const bool& _Shrink = true) {
            return TSVMLearnParam(_Time, 1, _Verbosity, _SubSize, _EpsTer, _Shrink); }
    // typical parameters for nonlinear svm
    static TSVMLearnParam NonLin(const int& _Time = -1, const int& _MemSize = 50, const int _Verbosity = 0,
        const int& _SubSize = 10, const double& _EpsTer = 1e-3, const bool& _Shrink = true) {
            return TSVMLearnParam(_Time, _MemSize, _Verbosity, _SubSize, _EpsTer, _Shrink); }
};

//////////////////////////////////////////////////////////////////////////
// SVM-Model-Parameters
//   Parameters model learning
typedef enum { smtClassifier, smtOneClass, smtRegression } TSVMModelType;

class TSVMModelParam {
public:
    TSVMModelType ModelType;
    TFlt C; // classificatio and regression -- cost parameter
    TFlt j; // classification -- for unbalanced sets
    TFlt nu; // oneclass -- cost parameter
    TFlt E; // regression -- epsilon area around points which doesn't bring error

public:
    TSVMModelParam(const TSVMModelType& _ModelType, const double& _CVal,
      const double& _j, const double& _nu, const double& _E):
        ModelType(_ModelType), C(_CVal), j(_j), nu(_nu), E(_E) { };

    static TSVMModelParam Classification(const double& _CVal, const double& _j) {
        return TSVMModelParam(smtClassifier, _CVal, _j, 0.0, 0.0); }
    static TSVMModelParam OneClass(const double& _nu) {
        return TSVMModelParam(smtOneClass, 0.0, 0.0, _nu, 0.0); }
    static TSVMModelParam Regression(const double& _E, const double& _CVal) {
        return  TSVMModelParam(smtRegression, _CVal, 0.0, 0.0, _E); }
};

//////////////////////////////////////////////////////////////////////////
// SVM-Factory
//   Trains classifiers, one-class SVM, regression...
class TSVMFactory {
public:
    static void trainClassifier(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const double& C, const double& j, const TSVMLearnParam& LearnParam);

    static void trainOneClass(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const double& nu, const TSVMLearnParam& LearnParam);

    static void trainRegression(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const double& E, const double& C, const TSVMLearnParam& LearnParam);

    static void train(TFltV& alphas, double& threshold,
        const bool& is_linear, const PKernel& ker, const PSVMTrainSet& docs,
        const TSVMModelParam& ModelParam, const TSVMLearnParam& LearnParam);
};
