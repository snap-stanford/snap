#ifndef snaplib_agmfit_h
#define snaplib_agmfit_h
#include "Snap.h"

/////////////////////////////////////////////////////////////////////////////////////
//Fitting AGM
class TAGMFit{
private:
	PUNGraph G; //graph to fit
	TVec<TIntSet> CIDNSetV; //Community ID -> Member Node ID Sets
	THash<TIntPr,TIntSet> EdgeComVH; // Edge -> Shared Community ID Set
	THash<TInt, TIntSet> NIDComVH; // Node ID -> Communitie IDs the node belongs to
	TIntV ComEdgesV; // The number of edges in each community
	TFlt PNoCom; //probability of edge when two nodes share no community (epsilon in the paper)
	TFltV LambdaV; // Parametrization of P_c (edge probability in community c), P_c = 1 - exp(-lambda)
	TRnd Rnd;
	THash<TIntPr,TFlt> NIDCIDPrH; //
	THash<TIntPr,TInt> NIDCIDPrS;

public:
	TFlt MinLambda; // Minimum value of lambda (default = 1e-5)
	TFlt MaxLambda; // Maximum value of lambda (default = 10)
	TFlt RegCoef; //Regularization coefficient when we fit for P_c (for finding # communities)
	TInt BaseCID; // ID of the epsilon community (in case we fit P_c of the epsilon community). We do not fit for the epsilon community in general.
	TAGMFit(){}
	~TAGMFit(){}
	TAGMFit(const PUNGraph& GraphPt, const TVec<TIntV>& CmtyVVPt, const int RndSeed = 0): Rnd(RndSeed), PNoCom(0.0), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), G(GraphPt), BaseCID(-1) { SetCmtyVV(CmtyVVPt);  }
	TAGMFit(const PUNGraph& GraphPt, const int InitComs, const int RndSeed = 0): Rnd(RndSeed), PNoCom(0.0), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), G(GraphPt), BaseCID(-1) { RandomInitCmtyVV(InitComs);  }
	TAGMFit(const PUNGraph& GraphPt, const TVec<TIntV>& CmtyVVPt, const TRnd& RndPt): Rnd(RndPt), PNoCom(0.0), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), G(GraphPt), BaseCID(-1) { SetCmtyVV(CmtyVVPt); }
	void RandomInitCmtyVV(const int InitComs, const double ComSzAlpha = 1.3, const double MemAlpha = 1.8, const int MinComSz = 8, const int MaxComSz = 200, const int MinMem = 1, const int MaxMem = 10);
	void AddBaseCmty(); // Include the epsilon community to fitting
	double Likelihood();
	double Likelihood(const TFltV& NewLambdaV) { double Tmp1, Tmp2; return Likelihood(NewLambdaV, Tmp1, Tmp2); }
	double Likelihood(const TFltV& NewLambdaV, double& LEdges, double& LNoEdges);
	void GetEdgeJointCom();
	void GradLogLForLambda(TFltV& GradV);
	int MLEGradAscentGivenCAG(const double& Thres=0.001, const int& MaxIter=10000, const TStr PlotNm = TStr());
	void SetDefaultPNoCom();
	void SetPNoCom(const double& Input) { if (BaseCID == -1) { PNoCom = Input; } }// only if we do not use eps-community
	double GetPNoCom() { return PNoCom; }
	void GetNewtonStep(TFltVV& HVV, TFltV& GradV, TFltV& DeltaLV);
	double SelectLambdaSum(const TIntSet& ComK);
	double SelectLambdaSum(const TFltV& NewLambdaV, const TIntSet& ComK);
	void RandomInit(const int& MaxK);
	void RunMCMC(const int& MaxIter, const int& EvalLambdaIter, const TStr& PlotFPrx);
	void SampleTransition(int& NID, int& JoinCID, int& LeaveCID, double& DeltaL);
	void InitNodeData();
	void LeaveCom(const int& NID, const int& CID);
	void JoinCom(const int& NID, const int& JoinCID);
	int RemoveEmptyCom();
	double GetStepSizeByLineSearchForLambda(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta);
	void SetLambdaV(const TFltV& LambdaPt) {LambdaV = LambdaPt;}
	void GetLambdaV(TFltV& OutV) {OutV = LambdaV;}
	void GetQV(TFltV& OutV);
	void GetCmtyVV(TVec<TIntV>& CmtyVV, const double QMax = 2.0);
	void GetCmtyVV(TVec<TIntV>& CmtyVV, TFltV& QV, const double QMax = 2.0);
	void SetCmtyVV(const TVec<TIntV>& CmtyVV);
	double SeekLeave(const int& UID, const int& CID);
	double SeekJoin(const int& UID, const int& CID);
	double SeekSwitch(const int& UID, const int& CurCID, const int& NewCID);
	void Save(TSOut& SOut) {
		G->Save(SOut);
		CIDNSetV.Save(SOut);
		EdgeComVH.Save(SOut);
		NIDComVH.Save(SOut);
		ComEdgesV.Save(SOut);
		PNoCom.Save(SOut);
		LambdaV.Save(SOut);
		NIDCIDPrH.Save(SOut);
		NIDCIDPrS.Save(SOut);
		MinLambda.Save(SOut);
		MaxLambda.Save(SOut);
		RegCoef.Save(SOut);
		BaseCID.Save(SOut);
	}
	void Load(TSIn& SIn, const int& RndSeed = 0) {
		G->Load(SIn);
		CIDNSetV.Load(SIn);
		EdgeComVH.Load(SIn);
		NIDComVH.Load(SIn);
		ComEdgesV.Load(SIn);
		PNoCom.Load(SIn);
		LambdaV.Load(SIn);
		NIDCIDPrH.Load(SIn);
		NIDCIDPrS.Load(SIn);
		MinLambda.Load(SIn);
		MaxLambda.Load(SIn);
		RegCoef.Load(SIn);
		BaseCID.Load(SIn);
		Rnd.PutSeed(RndSeed);
	}
};



#endif
