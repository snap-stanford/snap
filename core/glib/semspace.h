/////////////////////////////////////////////////
// Semantic-Space
ClassTP(TSemSpace, PSemSpace) //{
protected:
    typedef PSemSpace (*TSemSpcLoad)(TSIn& SIn);
    typedef TFunc<TSemSpcLoad> TSemSpcLoadF;
    static THash<TStr, TSemSpcLoadF> TypeToLoadFH;
    static bool Reg(const TStr& TypeNm, const TSemSpcLoadF& LoadF);
protected:
    TStr MethodeType;
public:
    TSemSpace(const TStr& MtdTyp): MethodeType(MtdTyp) { }
    TSemSpace(TSIn& SIn) { MethodeType.Load(SIn); }
    virtual ~TSemSpace() { };

    static PSemSpace Load(TSIn &SIn);
    virtual void Save(TSOut& SOut) const { 
        GetTypeNm(*this).Save(SOut); MethodeType.Save(SOut); }

  // projections
    // projection of BowDocBs
    virtual PBowDocBs Project(const PBowDocBs& BowDocBs, 
        const PBowDocWgtBs& BowDocWgtBs, const TIntV& DIdV, 
        const int& Vecs, const bool& NormP) const = 0;
    // projections of bow sparse vectors
    virtual PBowSpV ProjectSpV(const PBowSpV& SpV, 
        const int& Vecs, const bool& NormP) const = 0;
    virtual void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const = 0;
    virtual void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjSpV) const = 0;
    // projection of TIntFltKd sparse vectors
    virtual void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV)const  = 0;
    virtual void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjSpV)const  = 0;
    // projection of full vectors
    virtual void ProjectV(const TFltV& FullV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const = 0;

  // basis vectors and their dimensions
    // stores VecId-th basis vector into Vec
    virtual void GetVec(const int& VecN, TFltV& Vec)const  = 0;
    // returns dimensionality of Semantic-Space
    virtual int GetVecs() const = 0;
    // returns dimensionality in which the basis vectors live
    virtual int GetVecLen() const = 0;
    
    // get average approximation of original
    double GetAvgApprox(const TVec<PBowSpV>& BowSpV, const int& Dim) const;
    // get number of dimensions needed for desired average approximation
    int GetDimNeededForApprox(const TVec<PBowSpV>& BowSpV, 
        const double& ApproxThresh, double& ApproxVal, const int& MxDimDiff = 5) const;

    // transforms semantic-space to bow-partition by treating the
    // basis vectors as centroids (transformed using GetBowSpV)
    PBowDocPart GetBowDocPart(const PBowDocBs& BowDocBs, 
        const int& StartVecN = 0, const int _Vecs = -1,
        const double& Eps = 0.0) const;

    // converts VecN-th vector to two Bow-sparse-vectors, one only with
    // positive weights and one onyl with negative weights
    void GetVecBowSpV(const int& VecN, PBowSpV& PosSpV, PBowSpV& NegSpV,
        const double& Eps = 0.0) const;
    // returnes most caracteristic words for VecN-th vector
    void GetWords(PBowDocBs BowDocBs, const int& VecN, 
        const int& TopWords, const double& TopWordsWgtPrc, 
        TStrFltPrV& PosWordStrWgtV, TStrFltPrV& NegWordStrWgtV) const;

    // returnes the name of the method used to generate this semantic space
    const TStr& GetMethodeType() const { return MethodeType; }

    // serilization
    static PSemSpace LoadBin(const TStr& FNm) { TFIn FIn(FNm); return Load(FIn); }
    void SaveBin(const TStr& FNm) const { TFOut FOut(FNm); this->Save(FOut); }
    void SaveTxt(const TStr& FNm, PBowDocBs BowDocBs, const int& TopWords, 
        const double& TopWordsWgtPrc, const bool& ShowWgt = true) const;

    // prints statistics of aligned dataset in the new semantic space into text file
    static void SaveTxtStat(const TStr& FNm, PSemSpace SemSp1, PBowDocBs BowBs1, 
        PBowDocWgtBs BowWgt1, PSemSpace SemSp2, PBowDocBs BowBs2, PBowDocWgtBs BowWgt2, 
        const bool& NormP, const TIntV& DIdV = TIntV()) ;
    void SaveApproxStat(const TStr& FNm, const TVec<PBowSpV>& BowSpV, 
        const int& Step = 5) const;
};

/////////////////////////////////////////////////
// Vector-Semantic-Space
class TVecSemSpace: TSemSpace {
private:
    static bool IsReg;
    static bool MkReg(){return TSemSpace::Reg(TTypeNm<TVecSemSpace>(), &Load);}

private:
    TVec<TFltV> BasisV;
public:
    // create subspace spanned by vectors from _BasisV
    TVecSemSpace(const TVec<TFltV>& _BasisV, const TStr& _MethodeType): 
      TSemSpace(_MethodeType), BasisV(_BasisV) { };
    static PSemSpace New(TVec<TFltV>& _BasisV, const TStr& _MethodeType) { 
        return new TVecSemSpace(_BasisV, _MethodeType); }

    // create subspace spanned by columns of matrix BasisVV
    TVecSemSpace(const TFltVV& BasisVV, const TStr& _MethodeType);
    static PSemSpace New(const TFltVV& BasisVV, const TStr& _MethodeType) { 
        return new TVecSemSpace(BasisVV, _MethodeType); }

    // create subspace spanned by vectors DrctV written in ON 
    // basis created with PartialGS R from vectors from Set
    TVecSemSpace(PSVMTrainSet Set, PPartialGS R, 
        const TVec<TFltV>& DrctV, const TStr& _MethodeType);
    static PSemSpace New(PSVMTrainSet Set, PPartialGS R, 
        const TVec<TFltV>& DrctV, const TStr& _MethodeType) {
            return new TVecSemSpace(Set, R, DrctV, _MethodeType); }

    // crate subspace from Matlab matrix, columns are basis vectors
    TVecSemSpace(const TStr& MatlabFName);
    static PSemSpace LoadMatlab(const TStr& MatlabFName) {
        return new TVecSemSpace(MatlabFName); }

    // crate subspace from Matlab matrix, columns are basis vectors
    TVecSemSpace(const TStr& MatlabFName, const TStr& MapFName);
    static PSemSpace LoadMatlab(const TStr& MatlabFName, const TStr& MapFName) {
        return new TVecSemSpace(MatlabFName, MapFName); }

    TVecSemSpace(TSIn &SIn): TSemSpace(SIn) { BasisV.Load(SIn); };
    static PSemSpace Load(TSIn &SIn) { return new TVecSemSpace(SIn); }
    void Save(TSOut &SOut) const { TSemSpace::Save(SOut); BasisV.Save(SOut); }

    PBowDocBs Project(const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, 
        const TIntV& DIdV, const int& Vecs, const bool& NormP) const;
    PBowSpV ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP) const;
    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const;
    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const;
    void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const;
    void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const;
    void ProjectV(const TFltV& FullV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const;

    void GetVec(const int& VecN, TFltV& Vec) const { Vec = BasisV[VecN]; }
    int GetVecs() const { return BasisV.Len(); }
    int GetVecLen() const { return BasisV.Empty() ? 0 : BasisV[0].Len(); };
};

/////////////////////////////////////////////////
// Sparse-Vector-Semantic-Space
class TSpVecSemSpace: TSemSpace {
private:
    static bool IsReg;
    static bool MkReg(){return TSemSpace::Reg(TTypeNm<TSpVecSemSpace>(), &Load);}

private:
    TInt VecDim;
    TVec<TIntFltKdV> BasisV;
public:
    // create subspace spanned by vectors from _BasisV
    TSpVecSemSpace(const TVec<TIntFltKdV>& _BasisV, const TStr& _MethodeType);
    static PSemSpace New(TVec<TIntFltKdV>& _BasisV, const TStr& _MethodeType) { 
        return new TSpVecSemSpace(_BasisV, _MethodeType); }

    TSpVecSemSpace(TSIn &SIn): TSemSpace(SIn) { BasisV.Load(SIn); };
    static PSemSpace Load(TSIn &SIn) { return new TSpVecSemSpace(SIn); }
    void Save(TSOut &SOut) const { TSemSpace::Save(SOut); BasisV.Save(SOut); }

    PBowDocBs Project(const PBowDocBs& BowDocBs, 
        const PBowDocWgtBs& BowDocWgtBs, const TIntV& DIdV, 
        const int& Vecs, const bool& NormP) const;
    PBowSpV ProjectSpV(const PBowSpV& SpV, 
        const int& Vecs, const bool& NormP) const;
    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const;
    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const;
    void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const;
    void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const;
    void ProjectV(const TFltV& FullV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const;

    void GetVec(const int& VecN, TFltV& Vec) const {
        TLAMisc::ToVec(BasisV[VecN], Vec, VecDim); }
    int GetVecs() const { return BasisV.Len(); }
    int GetVecLen() const;
};

/////////////////////////////////////////////////
// KCCA-Semantic-Space
class TKCCASemSpace: TSemSpace {
private:
    static bool IsReg;
    static bool MkReg(){return TSemSpace::Reg(TTypeNm<TKCCASemSpace>(), &Load);}
private:
    // basis of semantic space written in basis obtian with Partial GS
    TVec<TFltV> BasisV;
    // basis vectors from TPartialGS as linear combination of documents from training set
    //   q_n = c_1 * x_1 + ... + c_l * x_l, kjer je l == no. of steps of Partial G.S.
    TVec<TFltV> GSBasisV;
    // set used for training KCCA
    PSVMTrainSet TrainSet;

    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltPrV& ProjSpV) const;

public:
    // create subspace spanned by vectors DrctV written in ON 
    // basis created with PartialGS R from vectors from Set
    TKCCASemSpace(PSVMTrainSet Set, PPartialGS R, const TVec<TFltV>& DrctV, const TStr& _MethodeType);
    static PSemSpace New(PSVMTrainSet Set, PPartialGS R, const TVec<TFltV>& DrctV, const TStr& _MethodeType) {
        return new TKCCASemSpace(Set, R, DrctV, _MethodeType); }

    TKCCASemSpace(TSIn &SIn): TSemSpace(SIn) { 
        BasisV.Load(SIn); GSBasisV.Load(SIn); TrainSet = TSVMTrainSet::Load(SIn); }
    static PSemSpace Load(TSIn &SIn) { return new TKCCASemSpace(SIn); }
    void Save(TSOut &SOut) const { 
        TSemSpace::Save(SOut); BasisV.Save(SOut); GSBasisV.Save(SOut); TrainSet->Save(SOut); }

    PBowDocBs Project(const PBowDocBs& BowDocBs, 
        const PBowDocWgtBs& BowDocWgtBs, const TIntV& DIdV, 
        const int& Vecs, const bool& NormP) const;
    PBowSpV ProjectSpV(const PBowSpV& SpV, 
        const int& Vecs, const bool& NormP) const;
    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const;
    void ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const { Fail; }
    void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const { Fail; }
    void ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const { Fail; }
    void ProjectV(const TFltV& FullV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const { Fail; }
    
    void GetVec(const int& VecN, TFltV& VecV) const;
    int GetVecs() const { return BasisV.Len(); }
    int GetVecLen() const { return TrainSet->Dim(); }
};

/////////////////////////////////////////////////
// Semantic-Space-Algorithms
// type of normalization of directions at the end of KCCA
typedef enum { kcntNone, kcntOne, kcntEigVal } TKCCACorrNrmType;
class TSemSpaceAlg { 
private: // algoriths
    // Kernel Canonical Correlation Analysis
    // ------------------------------------------------------------------------------
    //  Performs canonical correlation analysis on documents from SetA and SetB
    //  and returns solution to dual formulation. For more information check book
    //  'Kernel Methods and Pattern Analysis',page 164. Algorithm is of order
    //  O(ApproxDim^3) where ApproxDim is number of steps we do at Incomplete
    //  Cholesky. Also: |SetA|.Lines >= |SetB|.Lines !! Directions are returend as
    //  coeficients for linear combiantion of feature vectors from SetA or SetB
    //  (like alphas in SVM, to get normal use LinComb at TrainSet)
    //
    static void CalcKCCA(const TFltVV& Ra, const TFltVV& Rb, const double& ta, const double& tb,
        const TKCCACorrNrmType& CorrNrmType, TVec<TFltV>& DrctAV, TVec<TFltV>& DrctBV, TFltV& EigV);
    static void CalcKCCA(PSVMTrainSet SetA, PSVMTrainSet SetB, const double& t, const int& SpaceDim, 
        const double& Eps, PPartialGS& Ra, PPartialGS& Rb, const TKCCACorrNrmType& CorrNrmType,
        TVec<TFltV>& DrctAV, TVec<TFltV>& DrctBV, TFltV& EigV);

    // Decompose kernel matrix
    // ------------------------------------------------------------------------------
    //  calculates eigne vectors of kernel matrix. Algorithm is of
    //  order O(Set->Len()^3), it also calculates whole kernel matrix! (ouch)
    static void DecomposeKernelMatrix(PSVMTrainSet Set, const int& Dim, 
        TVec<TFltV>& CompV, const bool& CenterMatrix);

    // Kernel Principal Component Analysis
    // ------------------------------------------------------------------------------
    static PSemSpace CalcKPCA(PSVMTrainSet Set);

    // Full Latent Semantic Indexing
    // ------------------------------------------------------------------------------
    // calculates all eigenvectors of kernel matrix (=> V) and uses  that for U
    static PSemSpace CalcFullLSI(PSVMTrainSet Set, const int& SpaceDim);

    // Sparse Latent Semantic Indexing (calls TSparseSVD for Lanczos algorith!)
    // ------------------------------------------------------------------------------
    static PSemSpace CalcSparseLSI(TMatrix& TermDocM, const int& SpaceDim, 
        const int& MaxLanczos, const TSpSVDReOrtoType& ReOrtoType);
    // just in case if we need it once...
    static PSemSpace CalcSparseLSI(PSVMTrainSet Set, const int& SpaceDim, 
        const int& MaxLanczos = 300, const TSpSVDReOrtoType& ReOrtoType = ssotSelective);

public: // interface to algorithsms
    // Kernel Canonical Correlation Analysis
    //  BowDocWgtBsA, BowDocWgtBsB -- datasets A and B
    //  DIdVA, DIdVB -- indecies of documents for set A and B of documents that will  be  
    //      used for KCCA, docuemnts which index appear in same position in DIdVA and 
    //      DIdVB are paired!! Also this documents are saved into TSemSpace class
    //  t -- regulisation parameter (0.0 <= t <= 1.0) 
    //  SpaceDim -- size of generated semantic space
    //  SpA, SpB -- generated sematic spaces for SetA and SetB
    static void CalcKCCA(
        const TVec<PBowSpV>& BowSpVA, const TVec<PBowSpV>& BowSpVB, 
        const double& t, const int& SpaceDim, const double& Eps, 
        PSemSpace& SpA, PSemSpace& SpB, TFltV& EigV, 
        const TKCCACorrNrmType& CorrNrmType = kcntOne);
    // transforms the basis for semantic space to vectors living in BOW-space
    //   (needs much more memory and is slower...)
    static void CalcVecKCCA(
        const TVec<PBowSpV>& BowSpVA, const TVec<PBowSpV>& BowSpVB,
        const double& t, const int& SpaceDim, const double& Eps, 
        PSemSpace& SpA, PSemSpace& SpB, 
        const TKCCACorrNrmType& CorrNrmType = kcntOne);

    // Kernel Principal Component Analysis
    static PSemSpace CalcKPCA(const TVec<PBowSpV>& BowSpV);
    static PSemSpace CalcKPCA(PBowDocWgtBs BowDocWgtBs, const TIntV& DIdV);

    // Full Latent Semantic Indexing (saves full vectors)
    static PSemSpace CalcFullLSI(const TVec<PBowSpV>& BowSpV, const int& SpaceDim);
    static PSemSpace CalcFullLSI(PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, const int& SpaceDim);

    // Latent Semantic Indexing (training set is not saved but calcualted vectors may take lots of space!)
    //   SpaceDim -- size of generated semantic space
    //   MaxLanczos -- size of calculated subspace (should be 3*SpaceDim or 4*SpaceDim)
    //   ReOrtoType -- type of reortogonalization ('ssotNoOrto' is the fastest but not so
    //                 numaricaly stable, 'ssotSelective' is usualy best, 'ssotFull' is
    //                 most sure)
    static PSemSpace CalcSparseLSI(const TVec<PBowSpV>& BowSpV, 
        const int& SpaceDim, const int& MaxLanczos = 300, 
        const TSpSVDReOrtoType& ReOrtoType = ssotSelective);
    static PSemSpace CalcSparseLSI(PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, const int& SpaceDim, const int& MaxLanczos = 300, 
        const TSpSVDReOrtoType& ReOrtoType = ssotSelective);
};
