//////////////////////////////////////////////////////////////////////////
// Cross-Validation-Tools
class TCrossValid {
public:
    static void PrepareFolds(const int& Folds, TIntV& DIdV, const int& Seed,
        TVec<TIntV>& DIdVFoldV);
    static void PrepareSplit(const int& FoldN, const TVec<TIntV>& DIdVFoldV,
        TIntV& TrainSubDIdV, TIntV& TestSubDIdV);
  static void PrepareSubDIdV(const TIntV& SubSet, const int& DataSetLen, TIntV& DIdV);
};

//////////////////////////////////////////////////////////////////////////
// Sparse-Training-Set
class TSparseTrainSet : public TSVMTrainSet {
private:
    static bool IsReg;
    static bool MkReg(){return TSVMTrainSet::Reg(TTypeNm<TSparseTrainSet>(), &Load);}
private:
    TInt MaxDim; // max attrs
    TFltV ClsV, NormV;
    TVec<TIntFltKdV> TrainV;

    // for cloning
    TSparseTrainSet(const TIntV& VecIdV, const int& _MaxDim, const TFltV& _ClsV,
        const TFltV& _NormV, const TVec<TIntFltKdV>& _TrainV);
    static PSVMTrainSet New(const TIntV& VecIdV, const int& _MaxDim,
        const TFltV& _ClsV, const TFltV& _NormV, const TVec<TIntFltKdV>& _TrainV) {
            return new TSparseTrainSet(VecIdV, _MaxDim, _ClsV, _NormV, _TrainV); }
public:
    TSparseTrainSet(const int& PredSize = 0):
      TSVMTrainSet(ststSparse), MaxDim(0), ClsV(PredSize, 0),
      NormV(PredSize, 0), TrainV(PredSize, 0) { };
    static PSVMTrainSet New(const int& PredSize = 0) {
        return new TSparseTrainSet(PredSize); }

    TSparseTrainSet(TSIn& SIn): TSVMTrainSet(SIn) {
        MaxDim.Load(SIn); ClsV.Load(SIn); NormV.Load(SIn); TrainV.Load(SIn); }
    static PSVMTrainSet Load(TSIn& SIn) { return new TSparseTrainSet(SIn); }
    void Save(TSOut& SOut) const {
        TSVMTrainSet::Save(SOut); MaxDim.Save(SOut);
        ClsV.Save(SOut); NormV.Save(SOut); TrainV.Save(SOut); }
    void SaveTxt(const TStr& OutFNm) const;
    void SaveMatlab(const TStr& OutFNm) const;

    // neccessary stuff
    int Len() const { return ClsV.Len(); }
    int Dim() const { return MaxDim; }
    double GetNorm2(const int& VecId) const { return NormV[VecId]; }
    double GetVecParam(const int& VecId) const { return ClsV[VecId]; }

    double DotProduct(const int& VecId1, const int& VecId2) const {
        return TLinAlg::DotProduct(TrainV[VecId1], TrainV[VecId2]); }
    double DotProduct(const int& VecId1, double* vec2, const int& n) const;
    double DotProduct(const int& VecId1, const TFltV& vec2) const {
        return TLinAlg::DotProduct(vec2, TrainV[VecId1]); }

    void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const;
    void AddVec(const int& VecId1, TFltV& vec2, const double& K) const {
        TLinAlg::AddVec(K, TrainV[VecId1], vec2); }

    PSVMTrainSet Clone(const TIntV& VecIdV) const {
        return TSparseTrainSet::New(VecIdV, MaxDim, ClsV, NormV, TrainV); }

    // auxiliry stuff
    int AddAttrV(const TIntFltKdV& AttrV, const double& Cls, const bool& Normalize);
    const TIntFltKdV& GetAttrSparseV(const int& VecId) const { return TrainV[VecId]; }
    void RemoveAttrV(const int& VecId) {
        ClsV.Del(VecId); NormV.Del(VecId); TrainV.Del(VecId); }
    void SetVecParam(const int& VecId, const double& Cls) { ClsV[VecId] = Cls; }
    double DotProduct(const int& VecId1, const TIntFltKdV& Vec2) const {
        return TLinAlg::DotProduct(TrainV[VecId1], Vec2); }
    void Shuffle(TRnd& Rnd);

    // to be added if neccessary ...
    //int GetVecLen(const int& VecId) const { return TrainV[VecId].Len(); }
    //void Attr2Cls(const int& AttrId);
    //void Cls2Attr(const int& AttrId);
    //void Attr2Vec(const int& AttrId, TFltV& AttrVec);
    //void Vec2Attr(const int& AttrId, TFltV& AttrVec);

    static PSVMTrainSet LoadTxt(PSIn SIn, const bool& Normalize, const int& verbosity = 1);
    static PSVMTrainSet LoadTxt(const TStr& fname, const bool& Normalize,
        const int& verbosity = 1) { return LoadTxt(TFIn::New(fname), Normalize, verbosity); }
};

//////////////////////////////////////////////////////////////////////////
// Dense-Training-Set
class TDenseTrainSet : public TSVMTrainSet {
private:
    static bool IsReg;
    static bool MkReg(){return TSVMTrainSet::Reg(TTypeNm<TDenseTrainSet>(), &Load);}
private:
    TInt MaxDim; // number of attrs
    TFltV ClsV, NormV;
    TVec<TFltV> TrainV;

    // for cloning
    TDenseTrainSet(const TIntV& VecIdV, const int& _MaxDim,
        const TFltV& _ClsV, const TFltV& _NormV, const TVec<TFltV>& _TrainV);
    static PSVMTrainSet New(const TIntV& VecIdV, const int& _MaxDim,
        const TFltV& _ClsV, const TFltV& _NormV, const TVec<TFltV>& _TrainV) {
            return new TDenseTrainSet(VecIdV, _MaxDim, _ClsV, _NormV, _TrainV); }
public:
    TDenseTrainSet(const int& PredSize = 0):
        TSVMTrainSet(ststDense), MaxDim(0), ClsV(PredSize, 0),
        NormV(PredSize, 0), TrainV(PredSize, 0) { }
    static PSVMTrainSet New(const int& PredSize = 0) {
        return new TDenseTrainSet(PredSize); }

    // loads dense train set from sparse
    static PSVMTrainSet LoadFromSparse(PSVMTrainSet SparseSet);

    TDenseTrainSet(TSIn& SIn): TSVMTrainSet(SIn) {
        MaxDim.Load(SIn); ClsV.Load(SIn); NormV.Load(SIn); TrainV.Load(SIn); }
    static PSVMTrainSet Load(TSIn& SIn) { return new TDenseTrainSet(SIn); }
    void Save(TSOut& SOut) const {
        TSVMTrainSet::Save(SOut); MaxDim.Save(SOut);
        ClsV.Save(SOut); NormV.Save(SOut); TrainV.Save(SOut); }
    void SaveTxt(const TStr& OutFNm) const;

    // neccessary stuff
    int Len() const { return ClsV.Len(); }
    int Dim() const { return MaxDim; }
    double GetNorm2(const int& VecId) const { return NormV[VecId]; }
    double GetVecParam(const int& VecId) const { return ClsV[VecId]; }

    double DotProduct(const int& VecId1, const int& VecId2) const {
        return TLinAlg::DotProduct(TrainV[VecId1], TrainV[VecId2]); }
    double DotProduct(const int& VecId1, double* vec2, const int& n) const;
    double DotProduct(const int& VecId1, const TFltV& vec2) const {
        return TLinAlg::DotProduct(TrainV[VecId1], vec2); }

    void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const;
    void AddVec(const int& VecId1, TFltV& vec2, const double& K) const {
        TLinAlg::AddVec(K, TrainV[VecId1], vec2, vec2); }

    PSVMTrainSet Clone(const TIntV& VecIdV) const {
        return TDenseTrainSet::New(VecIdV, MaxDim, ClsV, NormV, TrainV); }

    // auxiliry stuff
    int AddAttrV(const TFltV& AttrV, const double& Cls, const bool& Normalize);
    const TFltV& GetAttrDenseV(const int& VecId) const { return TrainV[VecId]; }
    void RemoveAttrV(const int& VecId) {
        ClsV.Del(VecId); NormV.Del(VecId); TrainV.Del(VecId); }
    void SetVecParam(const int& VecId, const double& Cls) { ClsV[VecId] = Cls; }
    double DotProduct(const int& VecId1, const TIntFltKdV& Vec2) const {
        return TLinAlg::DotProduct(TrainV[VecId1], Vec2); }
    void Shuffle(TRnd& Rnd);

    // loads dense train set from SVM light file (first loads
    // SparseTrainSet and transforms it to dense)
    static PSVMTrainSet LoadTxt(PSIn SIn, const bool& Normalize, const int& verbosity = 1) {
        PSVMTrainSet SparseSet = TSparseTrainSet::LoadTxt(SIn, Normalize, verbosity);
        return TDenseTrainSet::LoadFromSparse(SparseSet); }
    static PSVMTrainSet LoadTxt(const TStr& fname, const bool& Normalize,
        const int& verbosity = 1) { return LoadTxt(TFIn::New(fname), Normalize, verbosity); }

    // projects elements of Set to columns of matrix Basis
    static PSVMTrainSet ProjectLin(PSVMTrainSet Set, const TFltVV& Basis,
        const bool& Normalize, const int& NewDim = -1);
    static PSVMTrainSet ProjectLin(PSVMTrainSet Set, const TVec<TFltV>& Basis,
        const bool& Normalize, const int& NewDim = -1);
};

//////////////////////////////////////////////////////////////////////////
// BagOfWords-Training-Set
class TBowTrainSet: public TSVMTrainSet {
private:
    static bool IsReg;
    static bool MkReg(){return TSVMTrainSet::Reg(TTypeNm<TBowTrainSet>(), &Load);}
private:
    TInt MaxDim;
    TIntV DIdV;
    TVec<PBowSpV> TrainV;
    TFltV ClsV;

    // for cloning
    TBowTrainSet(const TIntV& VecIdV, const int& _MaxDim,
        const TIntV& _DIdV, const TVec<PBowSpV>& _TrainV, const TFltV& _ClsV);
    static PSVMTrainSet New(const TIntV& VecIdV, const int& _MaxDim,
        const TIntV& _DIdV, const TVec<PBowSpV>& _TrainV, const TFltV& _ClsV) {
            return new TBowTrainSet(VecIdV, _MaxDim, _DIdV, _TrainV, _ClsV); }
public:
    // new empty set
    TBowTrainSet(const int& PredSize = 0): TSVMTrainSet(ststBow), MaxDim(0),
      DIdV(PredSize, 0), TrainV(PredSize, 0), ClsV(PredSize, 0) {};
    static PSVMTrainSet New(const int& PredSize = 0) { return new TBowTrainSet(PredSize); }

    // serialisation
    TBowTrainSet(TSIn& SIn): TSVMTrainSet(SIn) {
        MaxDim.Load(SIn); DIdV.Load(SIn); TrainV.Load(SIn); ClsV.Load(SIn); }
    static PSVMTrainSet Load(TSIn& SIn) { return new TBowTrainSet(SIn); }
    void Save(TSOut& SOut) const {
        TSVMTrainSet::Save(SOut); MaxDim.Save(SOut);
        DIdV.Save(SOut); TrainV.Save(SOut); ClsV.Save(SOut); }
    void SaveTxt(const TStr& OutFNm) const;

    // neccessary stuff
    int Len() const { return TrainV.Len(); }
    int Dim() const { return MaxDim; }
    double GetNorm2(const int& VecId) const {
        return TMath::Sqr(TrainV[VecId]->GetNorm()); }
    double GetVecParam(const int& VecId) const { return ClsV[VecId]; }

    double DotProduct(const int& VecId1, const int& VecId2) const {
        return TBowLinAlg::DotProduct(TrainV[VecId1], TrainV[VecId2]); }
    double DotProduct(const int& VecId1, double* vec2, const int& n) const;
    double DotProduct(const int& VecId1, const TFltV& vec2) const {
        return TBowLinAlg::DotProduct(vec2, TrainV[VecId1]); }
  void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const;
  void AddVec(const int& VecId1, TFltV& vec2, const double& K) const;

    PSVMTrainSet Clone(const TIntV& VecIdV) const {
        return TBowTrainSet::New(VecIdV, MaxDim, DIdV, TrainV, ClsV); }

    // auxiliry stuff
    int AddAttrV(const int& DId, PBowSpV SpVec, const double& Cls);
    PBowSpV GetAttrBowV(const int& VecId) const { return TrainV[VecId]; }
    int GetVecDId(const int& VecId) const { return DIdV[VecId]; }
    void RemoveAttrV(const int& VecId) {
        DIdV.Del(VecId); TrainV.Del(VecId); ClsV.Del(VecId); }
    void SetVecParam(const int& VecId, const double& Cls) { ClsV[VecId] = Cls; }
    double DotProduct(const int& VecId1, PBowSpV Vec2) const {
        return TBowLinAlg::DotProduct(TrainV[VecId1], Vec2); }
    void Shuffle(TRnd& Rnd);
};

//////////////////////////////////////////////////////////////////////////
// String-Training-Set
class TStringTrainSet : public TSVMTrainSet {
private:
    static bool IsReg;
    static bool MkReg(){return TSVMTrainSet::Reg(TTypeNm<TStringTrainSet>(), &Load);}
private:
    TFltV ClsV, NormV;
    TVec<TIntV> TrainV;
    // used for making TIntV vecs from TStr input docs
    PStrParser Parser;
    TInt AlphN; // size of alphabet
    // string kernel engine
    PStringKernel StrKer;

    // updates alphabet size
    void UpdateAlphN(const TIntV& Vec);

    // for cloning
    TStringTrainSet(const TIntV& VecIdV, const TFltV& _ClsV,
        const TFltV& _NormV, const TVec<TIntV>& _TrainV, PStrParser _Parser,
        const int& _AlphN, PStringKernel _StrKer);
    static PSVMTrainSet New(const TIntV& VecIdV, const TFltV& _ClsV,
        const TFltV& _NormV, const TVec<TIntV>& _TrainV, PStrParser _Parser,
        const int& _AlphN, PStringKernel _StrKer) {
            return new TStringTrainSet(VecIdV, _ClsV, _NormV,
                _TrainV, _Parser, _AlphN, _StrKer); }
public:
    TStringTrainSet(PStrParser _Parser, PStringKernel _StrKer, const int& PredSize = 0):
        TSVMTrainSet(ststString), ClsV(PredSize, 0), NormV(PredSize, 0),
        TrainV(PredSize, 0), Parser(_Parser), AlphN(_Parser->GetWords()),
        StrKer(_StrKer) { }
    static PSVMTrainSet New(PStrParser _Parser, PStringKernel _StrKer,
        const int& PredSize = 0) { return new TStringTrainSet(_Parser, _StrKer, PredSize); }

    TStringTrainSet(TSIn& SIn): TSVMTrainSet(SIn) {
        ClsV.Load(SIn); NormV.Load(SIn); TrainV.Load(SIn);
        Parser = TStrParser::Load(SIn); AlphN.Load(SIn);
        StrKer = TStringKernel::Load(SIn); }
    static PSVMTrainSet Load(TSIn& SIn) { return new TStringTrainSet(SIn); }
    void Save(TSOut& SOut) const {
        TSVMTrainSet::Save(SOut); ClsV.Save(SOut); NormV.Save(SOut);
        TrainV.Save(SOut); Parser->Save(SOut); AlphN.Save(SOut);
        StrKer->Save(SOut); }

    // neccessary stuff
    int Len() const { return ClsV.Len(); }
    int Dim() const { return 1; } // only used at linear SVM!!
    double GetNorm2(const int& VecId) const { return 1.0; } // all vectors are normalized!
    double GetVecParam(const int& VecId) const { return ClsV[VecId]; }

    double DotProduct(const int& VecId1, const int& VecId2) const {
        return StrKer->CalcKernel(TrainV[VecId1], TrainV[VecId2]) /
                sqrt(NormV[VecId1] * NormV[VecId2]); }
    double DotProduct(const int& VecId1, double* vec2, const int& n) const  {
        Fail; return 0.0; } // only used at linera svm (=> NOT here)!
    double DotProduct(const int& VecId1, const TFltV& vec2) const {
        Fail; return 0.0; } // only used at linera svm (=> NOT here)!

    void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const {
        Fail; } // only used at linera svm (=> NOT here)!
    void AddVec(const int& VecId1, TFltV& vec2, const double& K) const {
        Fail; } // only used at linera svm (=> NOT here)!

    PSVMTrainSet Clone(const TIntV& VecIdV) const {
        return TStringTrainSet::New(VecIdV, ClsV, NormV, TrainV, Parser, AlphN, StrKer); }

    // auxiliry stuff
    int AddAttrV(const TStr& AttrV, const double& Cls);
    int AddAttrV(const TIntV& AttrV, const double& Cls);
    const TIntV& GetAttrStringV(const int& VecId) const { return TrainV[VecId]; }
    void RemoveAttrV(const int& VecId) {
        ClsV.Del(VecId); NormV.Del(VecId); TrainV.Del(VecId); }
    void SetVecParam(const int& VecId, const double& Cls) { ClsV[VecId] = Cls; }
    double DotProduct(const int& VecId1, const TStr& Vec2) const;
    double DotProduct(const int& VecId1, const TIntV& Vec2) const;
    void Shuffle(TRnd& Rnd);
};

//////////////////////////////////////////////////////////////////////////
// Similarity-Matrix-Training-Set
class TSimMatrixTrainSet : public TSVMTrainSet {
private:
    static bool IsReg;
    static bool MkReg(){return TSVMTrainSet::Reg(TTypeNm<TSimMatrixTrainSet>(), &Load);}
private:
    TFltV ClsV;
    TFltVV SimMatrix;

    // for cloning
    TSimMatrixTrainSet(const TIntV& VecIdV, const TFltV& _ClsV, const TFltVV& _SimMatrix);
    static PSVMTrainSet New(const TIntV& VecIdV, const TFltV& _ClsV, const TFltVV& _SimMatrix) {
            return new TSimMatrixTrainSet(VecIdV, _ClsV, _SimMatrix); }
public:
    TSimMatrixTrainSet(const TFltVV& _SimMatrix, const TFltV& _ClsV = TFltV());
    static PSVMTrainSet New(const TFltVV& _SimMatrix, const TFltV& _ClsV = TFltV()) {
        return new TSimMatrixTrainSet(_SimMatrix, _ClsV); }

    TSimMatrixTrainSet(TSIn& SIn): TSVMTrainSet(SIn) {
        ClsV.Load(SIn); SimMatrix = TFltVV(SIn);}
    static PSVMTrainSet Load(TSIn& SIn) {
        return new TSimMatrixTrainSet(SIn); }
    void Save(TSOut& SOut) const {
        TSVMTrainSet::Save(SOut); ClsV.Save(SOut); SimMatrix.Save(SOut); }

    // neccessary stuff
    int Len() const { return ClsV.Len(); }
    int Dim() const { return 1; } // only used at linear SVM!!
    double GetNorm2(const int& VecId) const { return SimMatrix(VecId,VecId); }
    double GetVecParam(const int& VecId) const { return ClsV[VecId]; }

    double DotProduct(const int& VecId1, const int& VecId2) const {
        return SimMatrix(VecId1, VecId2); }
    double DotProduct(const int& VecId1, double* vec2, const int& n) const  {
        Fail; return 0.0; } // only used at linera svm (=> NOT here)!
    double DotProduct(const int& VecId1, const TFltV& vec2) const {
        Fail; return 0.0; } // only used at linera svm (=> NOT here)!

    void AddVec(const int& VecId1, double* vec2, const int& n, const double& K) const {
        Fail; } // only used at linera svm (=> NOT here)!
    void AddVec(const int& VecId1, TFltV& vec2, const double& K) const {
        Fail; } // only used at linera svm (=> NOT here)!

    PSVMTrainSet Clone(const TIntV& VecIdV) const {
        return TSimMatrixTrainSet::New(VecIdV, ClsV, SimMatrix); }
};

//////////////////////////////////////////////////////////////////////////
// Bag-Of-Words-Base to SVM-Train-Set
class TBowDocBs2TrainSet {
private:
    static void MakeSpVec(PBowSpV BowVec, TIntFltKdV& SpVec);
public:
    // new set with documents from BowDocWgtBs (no cat info)
    static PSVMTrainSet NewBowNoCat(const TVec<PBowSpV>& BowSpV,
        const double& DefParam = 1.0);
    static PSVMTrainSet NewBowNoCat(PBowDocWgtBs BowDocWgtBs,
        const TIntV& DIdV, const double& DefParam = 1.0);
    static PSVMTrainSet NewSparseNoCat(const TVec<PBowSpV>& BowSpV,
        const double& DefParam = 1.0);
    static PSVMTrainSet NewSparseNoCat(PBowDocWgtBs BowDocWgtBs,
        const TIntV& DIdV, const double& DefParam = 1.0);
    // new set with documents from category CatNm (for one class SVM)
    static PSVMTrainSet NewBowOneCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV);
    static PSVMTrainSet NewSparseOneCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV);
    static PSVMTrainSet NewStringOneCat(const PBowDocBs& BowDocBs,
        const int& CatId, const TIntV& DIdV, PStrParser Parser, PStringKernel StrKer);
    // new set with all documents, ones from category CatNm
    // have param 1, others have -1 (for binary SVM classifier)
    static PSVMTrainSet NewBowAllCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV,
        const double& SampleNegP = -1.0);
    static PSVMTrainSet NewSparseAllCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV);
    static PSVMTrainSet NewStringAllCat(const PBowDocBs& BowDocBs,
        const int& CatId, const TIntV& DIdV, PStrParser Parser, PStringKernel StrKer);
    // new set with all documents, ones from CatDIdV
    // have param 1, others have -1 (for binary SVM classifier)
    static PSVMTrainSet NewBowAllCatV(PBowDocWgtBs BowDocWgtBs,
        const TIntV& AllDIdV, const TIntV& CatDIdV);
    static PSVMTrainSet NewSparseAllCatV(PBowDocWgtBs BowDocWgtBs,
        const TIntV& AllDIdV, const TIntV& CatDIdV);
    // new set with documents described with category sparse vectors
    static PSVMTrainSet NewBowFromCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& AllDIdV);
    static PSVMTrainSet NewSparseFromCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& AllDIdV);
    // new set with cluster's centroids as categoires
    static PSVMTrainSet NewBow(PBowDocPart BowDocPart);
    static PSVMTrainSet NewSparse(PBowDocPart BowDocPart);
};

//////////////////////////////////////////////////////////////////////////
// SVM-Model
ClassTP(TSVMModel, PSVMModel) //{
private:
    // for all
    TFltV AlphaV; // lagrangian mutliplylers
    TFlt Thresh; // constant parameter (alias b alias threshold)
    TBool Linear; // is the model linear?
    // if linear
    TFltV WgtV; // normal vector (alias weight vector)
    //if not linear
    PSVMTrainSet SupVecs; // train set with support vectors
    PKernel Kernel; // kernel
    // for probabilistic output
    TBool IsSigDef;
    TSigmoid Sigmoid;
private:
    static PSVMModel MakeModel(const bool& Linear, const PKernel& ker,
        const PSVMTrainSet& TrainSet, const TIntV& SubSet,
        const TSVMModelParam& ModelParam, const TSVMLearnParam& LearnParam);

    static TCfyRes CrossValidation(const int& Folds, const int&Seed,
        const bool& Linear, const PKernel& ker,
        const PSVMTrainSet& TrainSet, const TIntV& SubSet,
        const TSVMModelParam& ModelParam, const TSVMLearnParam& LearnParam);

    TSVMModel(): Thresh(TFlt::Mn), Linear(true), IsSigDef(false) { }
    static PSVMModel New() { return new TSVMModel(); }
public:
    static PSVMModel NewEmptyModel() { return new TSVMModel(); }

    TSVMModel(TSIn& SIn);
    static PSVMModel Load(TSIn& SIn) { return new TSVMModel(SIn); }
    void Save(TSOut& SOut);

    static PSVMModel LoadBin(const TStr& FNm) { TFIn FIn(FNm); return new TSVMModel(FIn); }
    void SaveBin(const TStr& FNm) { TFOut FOut(FNm); Save(FOut); }

    // returns result of the model (works for all!)
    double GetRes(const TIntFltKdV& Vec) const;
    double GetRes(const TFltV& Vec) const;
    double GetRes(const TStr& Vec) const;
    double GetRes(const TIntV& Vec) const;
    double GetRes(PBowSpV Vec) const;
    double GetRes(const int& VecId) const;

    // returnes classification result of the model based on GetRes
    double GetCfy(const TIntFltKdV& Vec) const { return GetRes(Vec) > 0.0 ? 1.0 : -1.0; }
    double GetCfy(const TFltV& Vec) const { return GetRes(Vec) > 0.0 ? 1.0 : -1.0; }
    double GetCfy(const TStr& Vec) const { return GetRes(Vec) > 0.0 ? 1.0 : -1.0; }
    double GetCfy(const TIntV& Vec) const { return GetRes(Vec) > 0.0 ? 1.0 : -1.0; }
    double GetCfy(PBowSpV Vec) const { return GetRes(Vec) > 0.0 ? 1.0 : -1.0; }
    double GetCfy(const int& VecId) const { return GetRes(VecId) > 0.0 ? 1.0 : -1.0; }

    // returnes classification result of the model based on GetRes
    bool GetCfyBool(const TIntFltKdV& Vec) const { return GetRes(Vec) > 0.0 ? true : false; }
    bool GetCfyBool(const TFltV& Vec) const { return GetRes(Vec) > 0.0 ? true : false; }
    bool GetCfyBool(const TStr& Vec) const { return GetRes(Vec) > 0.0 ? true : false; }
    bool GetCfyBool(const TIntV& Vec) const { return GetRes(Vec) > 0.0 ? true : false; }
    bool GetCfyBool(PBowSpV Vec) const { return GetRes(Vec) > 0.0 ? true : false; }
    bool GetCfyBool(const int& VecId) const { return GetRes(VecId) > 0.0 ? true : false; }

    // tests the model on the given data
    TCfyRes Test(const PSVMTrainSet& TestSet, const TIntV& SubSet = TIntV()) const;

    // can this model return probabilistic predictions?
    bool IsProb() const { return IsSigDef; }
    // prepare model for probabilistic predictions based on the provided TrainSet
    void MakeProb(const PSVMTrainSet& TrainSet, const TIntV& SubSet = TIntV());
    // returnes probabilistic classification result of the model based
    // on the GetRes and sigmoid (MakeProb must be called before using this!!)
    double GetProbCfy(const TIntFltKdV& Vec) const { Assert(IsProb()); return Sigmoid(GetRes(Vec)); }
    double GetProbCfy(const TFltV& Vec) const { Assert(IsProb()); return Sigmoid(GetRes(Vec)); }
    double GetProbCfy(const TStr& Vec) const { Assert(IsProb()); return Sigmoid(GetRes(Vec)); }
    double GetProbCfy(const TIntV& Vec) const { Assert(IsProb()); return Sigmoid(GetRes(Vec)); }
    double GetProbCfy(PBowSpV Vec) const { Assert(IsProb()); return Sigmoid(GetRes(Vec)); }
    double GetProbCfy(const int VecId) const { Assert(IsProb()); return Sigmoid(GetRes(VecId)); }

    // access to the parameters of the model
    double GetMargin() const;
    void GetAlphaV(TFltV& _AlphaV) const { _AlphaV = AlphaV; }
    double GetThresh() const { return Thresh; }
    bool IsLinear() const { return Linear; }
    void GetWgtV(TFltV& _WgtV) const { IAssert(IsLinear()); _WgtV = WgtV; }
    PSVMTrainSet GetSupVecs() const { IAssert(!IsLinear()); return SupVecs; }
    PKernel GetKernel() const { IAssert(!IsLinear()); return Kernel; }
    TSigmoid GetSigmoid() const { IAssert(IsProb()); return Sigmoid; }

    //////////////////////////////////////////////////////////
    // Learning Binary SVM (linear/kernelized)
    static PSVMModel NewClsLinear(const PSVMTrainSet& TrainSet, const double& C,
        const double& j = 1.0, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin()) {
            return MakeModel(true, NULL, TrainSet, SubSet,
                TSVMModelParam::Classification(C, j), LearnParam); }
    static PSVMModel NewCls(const PSVMTrainSet& TrainSet, const PKernel& ker,
        const double& C, const double& j = 1.0, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin()) {
            return MakeModel(false, ker, TrainSet, SubSet,
                TSVMModelParam::Classification(C, j), LearnParam); }

    //////////////////////////////////////////////////////////
    // Cross-validation Binary SVM (linear/kernelized)
    static TCfyRes CrossValidClsLinear(const int& Folds, const int&Seed,
        const PSVMTrainSet& TrainSet, const double& C,
        const double& j = 1.0, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin()) {
            return CrossValidation(Folds, Seed, true, NULL, TrainSet, SubSet,
                TSVMModelParam::Classification(C, j), LearnParam); }
    static TCfyRes CrossValidCls(const int& Folds, const int&Seed,
        const PSVMTrainSet& TrainSet, const PKernel& ker, const double& C,
        const double& j = 1.0, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin()) {
            return CrossValidation(Folds, Seed, false, ker, TrainSet, SubSet,
                TSVMModelParam::Classification(C, j), LearnParam); }

    //////////////////////////////////////////////////////////
    // Learning One-Class SVM (linear/kernelized)
    static PSVMModel NewOneClassLinear(const PSVMTrainSet& TrainSet,
        const double& nu, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin()) {
            return MakeModel(true, NULL, TrainSet, SubSet,
                TSVMModelParam::OneClass(nu), LearnParam); }
    static PSVMModel NewOneClass(const PSVMTrainSet& TrainSet, const PKernel& ker,
        const double& nu, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin()) {
            return MakeModel(false, ker, TrainSet, SubSet,
                TSVMModelParam::OneClass(nu), LearnParam); }

    //////////////////////////////////////////////////////////
    // Cross-validation Binary SVM (linear/kernelized)
    static TCfyRes CrossValidOneClassLinear(const int& Folds, const int&Seed,
        const PSVMTrainSet& TrainSet, const double& nu, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin()) {
            return CrossValidation(Folds, Seed, true, NULL, TrainSet, SubSet,
                TSVMModelParam::OneClass(nu), LearnParam); }
    static TCfyRes CrossValidOneClass(const int& Folds, const int&Seed,
        const PSVMTrainSet& TrainSet, const PKernel& ker,
        const double& nu, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin()) {
            return CrossValidation(Folds, Seed, false, ker, TrainSet, SubSet,
                TSVMModelParam::OneClass(nu), LearnParam); }

    //////////////////////////////////////////////////////////
    // Learning Regression SVM (linear/kernelized)
    static PSVMModel NewRegLinear(const PSVMTrainSet& TrainSet,
        const double& E, const double& C, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin()) {
            return MakeModel(true, NULL, TrainSet, SubSet,
                TSVMModelParam::Regression(E, C), LearnParam); }
    static PSVMModel NewReg(const PSVMTrainSet& TrainSet, const PKernel& ker,
        const double& E, const double& C, const TIntV& SubSet = TIntV(),
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin()) {
            return MakeModel(false, ker, TrainSet, SubSet,
                TSVMModelParam::Regression(E, C), LearnParam); }
};

//////////////////////////////////////////////////////////////////////////
// BagOfWords-SVM-Model
class TBowSVMMd: public TBowMd {
private:
    static bool IsReg;
    static bool MkReg(){return TBowMd::Reg(TTypeNm<TBowSVMMd>(), &Load);}
private:
    TStr CatNm;
    PSVMModel Model;
private:
    static PSVMTrainSet PrepareClsTrainSet(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& SampleNegP = -1.0);
    static PSVMTrainSet PrepareOneClassTrainSet(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV);
    static PSVMTrainSet PrepareClsTrainSet(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& AllDIdV, const TIntV& CatDIdV);

    static PBowSpV GetKeywords(const PSVMTrainSet& TrainSet,
        const PSVMModel& SVMModel, const int WdN,
        const double& VecSign, const double& WgtSign, const bool& AvgOverSet);
public:
    TBowSVMMd(const PBowDocBs& BowDocBs, const TStr& _CatNm, PSVMModel _Model):
      TBowMd(BowDocBs), CatNm(_CatNm), Model(_Model) {}
    static PBowMd New(const PBowDocBs& BowDocBs, const TStr& _CatNm,
        PSVMModel _Model) { return new TBowSVMMd(BowDocBs, _CatNm, _Model); }

    TBowSVMMd(TSIn& SIn);

    static PBowMd Load(TSIn& SIn){return PBowMd(new TBowSVMMd(SIn));}
    void Save(TSOut& SOut);

    void GetCfy(const PBowSpV& QueryBowSpV, TFltStrPrV& WgtCatNmPrV);
    static TCfyRes Test(PBowMd BowMd, PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& DIdV, const TStr& CatNm);

  // model information
    bool IsCat() const {return !CatNm.Empty();}
    TStr GetCatNm() const {IAssert(IsCat()); return CatNm;}
    bool IsLinComb() const {return Model->IsLinear();}
    void GetLinComb(
     const PBowDocBs& BowDocBs, TFltStrPrV& WgtStrPrV, double& Tsh) const;

    void SaveTxt(const TStr& FNm, const PBowDocBs& BowDocBs,
        const PBowDocWgtBs& BowDocWgtBs);

    //////////////////////////////////////////////////////////
    // Learning Binary SVM (linear/kernelized)
    static PBowMd NewClsLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& C, const double& j = 1.0,
        const bool& ProbabilisticP = false, const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static PBowMd NewClsLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& TrainDIdV, const TIntV& CatDIdV, const double& C, const double& j = 1.0,
        const bool& ProbabilisticP = false, const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static PBowMd NewCls(const PKernel& ker, const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& C, const double& j = 1.0, const bool& ProbabilisticP = false,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin());

    //////////////////////////////////////////////////////////
    // Cross-validation Binary SVM (linear/kernelized)
    static TCfyRes CrossValidClsLinear(const int& Folds, const int& Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs, const TStr& CatNm,
        const TIntV& TrainDIdV, const double& C, const double& j = 1.0,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static TCfyRes CrossValidClsLinear(const int& Folds, const int& Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs, const TIntV& TrainDIdV,
        const TIntV& CatDIdV, const double& C, const double& j = 1.0,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static TCfyRes CrossValidCls(const int& Folds, const int& Seed,
        const PKernel& ker, const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm,
        const TIntV& TrainDIdV, const double& C, const double& j = 1.0,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin());

    //////////////////////////////////////////////////////////
    // Learning and Cross-validation Multi-Class SVM (linear)
    static PBowMd NewMultiClsLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& TrainDIdV, const double& C, const double& j = 1.0,
        const bool& SampleNegP = false, const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static TCfyRes CrossValidMultiClsLinear(const int& Folds, const int& Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const double& C, const double& j = 1.0, const bool& SampleNegP = false,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());

    //////////////////////////////////////////////////////////
    // Learning One-Class SVM (linear/kernelized)
    static PBowMd NewOneClassLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& nu,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static PBowMd NewOneClass(const PKernel& ker, const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& nu, const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin());

    //////////////////////////////////////////////////////////
    // Cross-validation One-Class SVM (linear/kernelized)
    static TCfyRes CrossValidOneClassLinear(const int& Folds, const int&Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& nu,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::Lin());
    static TCfyRes CrossValidOneClass(const int& Folds, const int&Seed,
        const PKernel& ker, const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& nu,
        const TSVMLearnParam& LearnParam = TSVMLearnParam::NonLin());

    //////////////////////////////////////////////////////////
    // Keyword extraction using Binary SVM (linear)
    static PBowSpV GetKeywords(const PBowDocBs& BowDocBs,
        const TIntV& TrainDIdV, const TIntV& CatDIdV,
        const int WdN = 10, const double& C = 1.0,
        const double& j = 1.0, const int& Time = 10,
        const TBowWordWgtType& SvmWordWgtType = bwwtLogDFNrmTFIDF,
        const double& VecSign = 1.0, const double& WgtSign = 1.0,
        const bool& AvgOverSet = true);
    static PBowSpV GetKeywords(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& TrainDIdV,
        const TIntV& CatDIdV, const int WdN = 10, const double& C = 1.0,
        const double& j = 1.0, const int& Time = 10,
        const double& VecSign = 1.0, const double& WgtSign = 1.0,
        const bool& AvgOverSet = true);
    static PBowSpV GetKeywords(const PBowDocBs& BowDocBs,
        const TIntV& TrainDIdV, const TStr& CatNm,
        const int WdN = 10, const double& C = 1.0,
        const double& j = 1.0, const int& Time = 10,
        const TBowWordWgtType& SvmWordWgtType = bwwtLogDFNrmTFIDF,
        const double& VecSign = 1.0, const double& WgtSign = 1.0,
        const bool& AvgOverSet = true);
    static PBowSpV GetKeywords(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& TrainDIdV,
        const TStr& CatNm, const int WdN = 10, const double& C = 1.0,
        const double& j = 1.0, const int& Time = 10,
        const double& VecSign = 1.0, const double& WgtSign = 1.0,
        const bool& AvgOverSet = true);
};

//////////////////////////////////////////////////////////////////////////
// Csv-Feature-File-Loader
class TSVMTrainSetCsv {
private:

    ///////////////////////////////////////
    // Abstract-Feature-Generator
    ClassTP(TFtrGen, PFtrGen)//{
    public:
        TFtrGen() { }
        virtual ~TFtrGen() { }

        virtual void Update(const TStr& Val) = 0;
        virtual void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const = 0;

        virtual void AddWds(const TStr& Prefix,
            const PBowDocBs& BowDocBs, int& Offset) const;

        // functions which only work in nominal
        virtual int GetVals() const = 0;
        virtual TStr GetVal(const int& ValN) const = 0;
        virtual void GetValV(const TStr& Str, TStrV& ValV) const = 0;
    };

    ///////////////////////////////////////
    // Numeric-Feature-Generator
    class TFtrGenNumeric : public TFtrGen {
    public: static TStr Type;
    private:
        TFlt MnVal;
        TFlt MxVal;

        double Trans(const double& Val) const;
        double GetFlt(const TStr& Str) const;
    public:
        TFtrGenNumeric(): MnVal(TFlt::Mx), MxVal(TFlt::Mn) { }
        static PFtrGen New() { return new TFtrGenNumeric; }
        void Update(const TStr& Val);
        void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const;

        int GetVals() const { return 1; }
        TStr GetVal(const int& ValN) const { return Type; }
        void GetValV(const TStr& Str, TStrV& ValV) const { Fail; }
    };

    ///////////////////////////////////////
    // Nominal-Feature-Generator
    class TFtrGenNominal : public TFtrGen {
    public: static TStr Type;
    private:
        TStrH ValH;
    public:
        TFtrGenNominal() { }
        static PFtrGen New() { return new TFtrGenNominal; }
        void Update(const TStr& Val);
        void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const;

        int GetVals() const { return ValH.Len(); }
        TStr GetVal(const int& ValN) const { return Type + ":" + ValH.GetKey(ValN); }
        void GetValV(const TStr& Str, TStrV& ValV) const { ValV = TStrV::GetV(Str); }
    };

    ///////////////////////////////////////
    // Tokenizable-Feature-Generator
    class TFtrGenToken : public TFtrGen {
    public: static TStr Type;
    private:
        PSwSet SwSet;
        PStemmer Stemmer;
        TInt Docs;
        TStrH TokenH;

        void GetTokenV(const TStr& Str, TStrV& TokenStrV) const;
    public:
        TFtrGenToken(PSwSet _SwSet, PStemmer _Stemmer):
          SwSet(_SwSet), Stemmer(_Stemmer) { }
        static PFtrGen New(PSwSet SwSet, PStemmer Stemmer) {
            return new TFtrGenToken(SwSet, Stemmer); }
        void Update(const TStr& Val);
        void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const;

        int GetVals() const { return TokenH.Len(); }
        TStr GetVal(const int& ValN) const { return Type + ":" + TokenH.GetKey(ValN); }
        void GetValV(const TStr& Str, TStrV& ValV) const { Fail; }
    };

    ///////////////////////////////////////
    // Sparse-Feature-Generator
    class TFtrGenSparseNumeric : public TFtrGen {
    public: static TStr Type;
    private:
        TInt MxId;
        TFtrGenNumeric FtrGen;

        void Split(const TStr& Str, int& Id, TStr& Val) const;
    public:
        TFtrGenSparseNumeric() { }
        static PFtrGen New() { return new TFtrGenSparseNumeric(); }
        void Update(const TStr& Str);
        void Add(const TStr& Str, TIntFltKdV& SpV, int& Offset) const;

        int GetVals() const { return MxId + 1; }
        TStr GetVal(const int& ValN) const;
        void GetValV(const TStr& Str, TStrV& ValV) const { Fail; }
    };

    ///////////////////////////////////////
    // MultiNomial-Feature-Generator
    class TFtrGenMultiNom : public TFtrGen {
    public: static TStr Type;
    private:
        TFtrGenNominal FtrGen;

    public:
        TFtrGenMultiNom() { }
        static PFtrGen New() { return new TFtrGenMultiNom(); }
        void Update(const TStr& Str);
        void Add(const TStr& Str, TIntFltKdV& SpV, int& Offset) const;

        int GetVals() const { return FtrGen.GetVals(); }
        TStr GetVal(const int& ValN) const;
        void GetValV(const TStr& Str, TStrV& ValV) const;
    };

public:
    static PBowDocBs LoadCsv(TStr& FNm, const int& ClassId,
        const TIntV& IgnoreIdV, const int& TrainLen);
};
