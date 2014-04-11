#ifndef snap_agmfit_h
#define snap_agmfit_h

#include "Snap.h"

/////////////////////////////////////////////////
/// Fitting the Affilialiton Graph Model (AGM).
class TAGMFit {
private:
  PUNGraph G; ///< Graph to fit.
  TVec<TIntSet> CIDNSetV;  ///< Community ID -> Member Node ID Sets.
  THash<TIntPr,TIntSet> EdgeComVH; ///< Edge -> Shared Community ID Set.
  THash<TInt, TIntSet> NIDComVH; ///< Node ID -> Communitie IDs the node belongs to.
  TIntV ComEdgesV; ///< The number of edges in each community.
  TFlt PNoCom; ///< Probability of edge when two nodes share no community (epsilon in the paper).
  TFltV LambdaV; ///< Parametrization of P_c (edge probability in community c), P_c = 1 - exp(-lambda).
  TRnd Rnd;
  THash<TIntPr,TFlt> NIDCIDPrH; ///< <Node ID, Community ID> pairs (for sampling MCMC moves).
  THash<TIntPr,TInt> NIDCIDPrS; ///< <Node ID, Community ID> pairs (for sampling MCMC moves).
  TFlt MinLambda; ///< Minimum value of regularization parameter lambda (default = 1e-5).
  TFlt MaxLambda; ///< Maximum value of regularization parameter lambda (default = 10).
  TFlt RegCoef; ///< Regularization parameter when we fit for P_c (for finding # communities).
  TInt BaseCID; ///< ID of the Epsilon-community (in case we fit P_c of the epsilon community). We do not fit for the Epsilon-community in general.

public:
  TAGMFit() { }
  ~TAGMFit() { }
  /// COMMENT. Use @Param to describribe parameters.
  TAGMFit(const PUNGraph& GraphPt, const TVec<TIntV>& CmtyVVPt, const int RndSeed = 0): G(GraphPt), PNoCom(0.0), Rnd(RndSeed), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), BaseCID(-1) { SetCmtyVV(CmtyVVPt);  }
  /// COMMENT. Use @Param to describribe parameters.
  TAGMFit(const PUNGraph& GraphPt, const int InitComs, const int RndSeed = 0): G(GraphPt), PNoCom(0.0), Rnd(RndSeed), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), BaseCID(-1) { NeighborComInit(InitComs); }//RandomInitCmtyVV(InitComs);  }
  /// COMMENT. Use @Param to describribe parameters.
  TAGMFit(const PUNGraph& GraphPt, const TVec<TIntV>& CmtyVVPt, const TRnd& RndPt): G(GraphPt), PNoCom(0.0), Rnd(RndPt), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), BaseCID(-1) { SetCmtyVV(CmtyVVPt); }
  void Save(TSOut& SOut);
  void Load(TSIn& SIn, const int& RndSeed = 0);

  /// Randomly initialize bipartite community affiliation graph.
  void RandomInitCmtyVV(const int InitComs, const double ComSzAlpha = 1.3, const double MemAlpha = 1.8, const int MinComSz = 8, const int MaxComSz = 200, const int MinMem = 1, const int MaxMem = 10);
  /// Add Epsilon community (base community which includes all nodes) into community affiliation graph. This means that we will later fit the value of epsilon.
  void AddBaseCmty(); 
  /// COMMENT.
  double Likelihood();
  double Likelihood(const TFltV& NewLambdaV) { double Tmp1, Tmp2; return Likelihood(NewLambdaV, Tmp1, Tmp2); }
  double Likelihood(const TFltV& NewLambdaV, double& LEdges, double& LNoEdges);
  void SetRegCoef(const double Val) { RegCoef = Val; }
  /// For each <tt>(u, v)</tt> in edges, precompute \c C_uv (the set of communities nodes u and v share).
  void GetEdgeJointCom();
  /// Initialize node community memberships using best neighborhood communities (see D. Gleich et al. KDD'12).
  void NeighborComInit(const int InitComs);
  // Gradient of likelihood for \c P_c.
  void GradLogLForLambda(TFltV& GradV);
  /// Gradient descent for \c p_c while keeping the community affiliation graph (CAG) fixed.
  int MLEGradAscentGivenCAG(const double& Thres=0.001, const int& MaxIter=10000, const TStr PlotNm = TStr());
  /// Set Epsilon (the probability that two nodes sharing no communities connect) to the default value.
  void SetDefaultPNoCom();
  /// Set Epsilon (the probability that two nodes sharing no communities connect) to the default value.
  void SetPNoCom(const double& Epsilon) { if (BaseCID == -1 && Epsilon > 0.0) { PNoCom = Epsilon; } }
  double GetPNoCom() { return PNoCom; }
  /// Compute the empirical edge probability between a pair of nodes who share no community (epsilon), based on current community affiliations.
  double CalcPNoComByCmtyVV(const int& SamplePairs = -1);
  // OP RS 2014/04/10 commented out since there is no implementation
  //void GetNewtonStep(TFltVV& HVV, TFltV& GradV, TFltV& DeltaLV);
  /// Compute sum of \c lambda_c (which is log (1 - \c p_c)) over \c C_uv (<tt>ComK</tt>). The function is used to compute edge probability \c P_uv.
  double SelectLambdaSum(const TIntSet& ComK);
  /// COMMENT.
  double SelectLambdaSum(const TFltV& NewLambdaV, const TIntSet& ComK);
  
  /// COMMENT.
  void RandomInit(const int& MaxK);
  /// Main procedure for fitting the AGM to a given graph using MCMC.
  void RunMCMC(const int& MaxIter, const int& EvalLambdaIter, const TStr& PlotFPrx = TStr());
  /// Sample MMCM transitions: Choose among (<tt>join, leave, switch</tt>), and then sample (<tt>NID, CID</tt>).
  void SampleTransition(int& NID, int& JoinCID, int& LeaveCID, double& DeltaL);
  /// COMMENT.
  void InitNodeData();
  /// After MCMC, \c NID leaves community \c CID.
  void LeaveCom(const int& NID, const int& CID);
  // After MCMC, \c NID joins community \c CID.
  void JoinCom(const int& NID, const int& JoinCID);
  /// Remove all communities with no members.
  int RemoveEmptyCom();
  /// Compute the change in likelihood (Delta) if node \c UID leaves community \c CID.
  double SeekLeave(const int& UID, const int& CID);
  /// Compute the change in likelihood (Delta) if node \c UID joins community \c CID.
  double SeekJoin(const int& UID, const int& CID);
  // Compute the change in likelihood (Delta) if node \c UID switches from \c CurCID to \c NewCID.
  double SeekSwitch(const int& UID, const int& CurCID, const int& NewCID);
  
  /// Step size search for updating P_c (which is parametarized by regularization parameter lambda).
  double GetStepSizeByLineSearchForLambda(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta);
  /// COMMENT.
  void SetLambdaV(const TFltV& LambdaPt) {LambdaV = LambdaPt;}
  /// COMMENT.
  void GetLambdaV(TFltV& OutV) {OutV = LambdaV;}
  /// Returns \c QV, a vector of (1 - \c p_c) for each community \c c.
  void GetQV(TFltV& OutV);
  /// Get communities whose \v p_c is higher than 1 - \c QMax.
  void GetCmtyVV(TVec<TIntV>& CmtyVV, const double QMax = 2.0);
  /// COMMENT.
  void GetCmtyVV(TVec<TIntV>& CmtyVV, TFltV& QV, const double QMax = 2.0);
  /// COMMENT.
  void SetCmtyVV(const TVec<TIntV>& CmtyVV);
  /// COMMENT.
  void PrintSummary();
};

#endif
