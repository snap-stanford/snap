#ifndef snaplib_logreg_light_h
#define snaplib_logreg_light_h


//Light version of logistic regression (originally in Snap)-- Jaewon Yang, 04/07/12
#include "Snap.h"

// forward
class TLogRegPredict;
typedef TPt<TLogRegPredict> PLogRegPredict;

///////////////////////////////////////////////////////////////////////
// Logistic Regression using gradient descent
// X: N * M matrix where N = number of examples and M = number of features.
class TLogRegFit {
private:
	TVec<TFltV> X;
	TFltV Y;
	TFltV Theta;
	int M; // number of features
public:
	TLogRegFit() {}
	~TLogRegFit() {}
  // returns model for logistic regression
  PLogRegPredict CalcLogRegGradient(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm = TStr(), const double& ChangeEps = 0.01, const int& MaxStep = 200, const bool InterceptPt = false);
	PLogRegPredict CalcLogRegNewton(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm = TStr(), const double& ChangeEps = 0.01, const int& MaxStep = 200, const bool InterceptPt = false);
	int MLEGradient(const double& ChangeEps, const int& MaxStep, const TStr PlotNm);
	int MLENewton(const double& ChangeEps, const int& MaxStep, const TStr PlotNm);
	double GetStepSizeByLineSearch(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta);
	double Likelihood(const TFltV& NewTheta);
	double Likelihood() { return Likelihood(Theta); }
	void Gradient(TFltV& GradV);
	void Hessian(TFltVV& HVV);
	void GetNewtonStep(TFltVV& HVV, const TFltV& GradV, TFltV& DeltaLV);
};


///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model
class TLogRegPredict {
private: 
    TCRef CRef;
private:
    TFltV Theta;
public:
    TLogRegPredict(const TFltV& _bb): Theta(_bb) { };
    //static PLogRegMd New(PSparseTrainSet Set) { return TLogReg::CalcLogReg(Set); }; //TODO

    TLogRegPredict(TSIn& SIn) { Theta.Load(SIn); };
    static PLogRegPredict Load(TSIn& SIn) { return new TLogRegPredict(SIn); };
    void Save(TSOut& SOut) const { Theta.Save(SOut); };

    UndefDefaultCopyAssign(TLogRegPredict);
public:    
    // classifies vector, returns probability that AttrV is positive
	static void GetCfy(const TVec<TFltV>& X, TFltV& OutV, const TFltV& NewTheta);
	static double GetCfy(const TFltV& AttrV, const TFltV& NewTheta);
	double GetCfy(const TFltV& AttrV) { return GetCfy(AttrV, Theta); }
	void GetTheta(TFltV& _Theta) { _Theta = Theta; }
	void GetCfy(const TVec<TFltV>& X, TFltV& OutV) { GetCfy(X, OutV, Theta); }
	void PrintTheta() { for (int t = 0; t < Theta.Len(); t++) { printf("Theta[%d] = %f\n", t, Theta[t].Val); } }
  friend class TPt<TLogRegPredict>;
};





#endif