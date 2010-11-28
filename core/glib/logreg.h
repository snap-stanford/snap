// forward
class TLogRegMd;
typedef TPt<TLogRegMd> PLogRegMd;

///////////////////////////////////////////////////////////////////////
// Fast-Robust-Logistic-Regression
//
//   Input: matrix X with documents on rows and attributes on columns
//          vector y with binary values for documents from X
//   Output: Logistic-Regression-Model
//
//   Algorithm from article "Fast Robust Logistic Regression for
//   Large Sparse Datasets with Binary Operators" by Paul R. Kamarek
//   and Andrew W. Moore. CG algorith written with help from
//   "Templates for the Solutions of Linear System: Building Blocks
//   for Iterative Methods".
class TLogReg {
private:
    // solves (X'*W*X)*bb = (X'*W)*z for bb where W = diag(w)
    static void CG(const TMatrix& Matrix, const TFltV& w, const TFltV& b, 
        TFltV& x, const int& MaxStep, const int& verb);

    // Iteratively Re-weighted Least Squares
    static void IRLS(const TMatrix& Matrix, TFltV& y, TFltV& bb, 
        const double& ChangeEps, const int& MaxStep, const int& verb);
public:
    // returns model for logistic regression
    static PLogRegMd CalcLogReg(PSVMTrainSet Set, const double& ChangeEps = 0.01, const int& MaxStep = 200);
    static PLogRegMd CalcLogReg(TMatrix& Set, TFltV& y, const double& ChangeEps = 0.01, const int& MaxStep = 200);

    // learns model from BOW for category CatNm
    static PBowMd CalcLogReg(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& DIdV, 
        const double& Treshold, const double& ChangeEps = 0.01, const int& MaxStep = 200, const int& Verbosity = 0);
};

///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model
class TLogRegMd {
private: 
    TCRef CRef;
private:
    TFltV bb;
public:
    TLogRegMd(const TFltV& _bb): bb(_bb) { };
    //static PLogRegMd New(PSparseTrainSet Set) { return TLogReg::CalcLogReg(Set); }; //TODO

    TLogRegMd(TSIn& SIn) { bb.Load(SIn); };
    static PLogRegMd Load(TSIn& SIn) { return new TLogRegMd(SIn); };
    void Save(TSOut& SOut) const { bb.Save(SOut); };

    UndefDefaultCopyAssign(TLogRegMd);
public:    
    // classifies vector, returns probability that AttrV is positive
    double GetCfy(const TIntFltKdV& AttrV);
    // classifies columns of matrix and compars results to y
    TCfyRes Test(PSVMTrainSet Set, const double& threshold);

    friend class TPt<TLogRegMd>;
};

///////////////////////////////////////////////////////////////////////
// BagOfWords-Logistic-Regression-Model
class TBowLogRegMd: public TBowMd {
private:
    static bool IsReg;
    static bool MkReg(){return TBowMd::Reg(TTypeNm<TBowLogRegMd>(), &Load);}
private:
	TStr CatNm;
	TFlt Treshold;

	TFltV WeightV;
public:
	TBowLogRegMd(const PBowDocBs& BowDocBs, const TStr& _CatNm,
		const double& _Treshold, const TFltV& _WeightV):
		TBowMd(BowDocBs), CatNm(_CatNm), Treshold(_Treshold), WeightV(_WeightV) { }
	TBowLogRegMd(TSIn& SIn): TBowMd(SIn) { Treshold.Load(SIn); CatNm.Load(SIn); WeightV.Load(SIn); }
  	static PBowMd Load(TSIn& SIn) { return PBowMd(new TBowLogRegMd(SIn)); }
	void Save(TSOut& SOut) { TBowMd::Save(SOut); Treshold.Save(SOut); CatNm.Save(SOut); WeightV.Save(SOut); }

    // model creation
    static PBowMd New(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
    	const TStr& _CatNm, const TIntV& _DIdV, const double& _Treshold = 0.8,
        const double& ChangeEps = 0.01, const int& MaxStep = 100, const int& Verbosity = 0) {
			return TLogReg::CalcLogReg(BowDocBs, BowDocWgtBs, _CatNm, _DIdV, _Treshold, ChangeEps, MaxStep, Verbosity); }

    // classification
    void GetCfy(const PBowSpV& QueryBowSpV, TFltStrPrV& WgtCatNmPrV);
};

