#ifndef snap_agmfast_h
#define snap_agmfast_h
#include "Snap.h"

/////////////////////////////////////////////////
/// Community detection with AGM. Sparse AGM-fast with coordinate ascent.
class TAGMFast { 
private:
  PUNGraph G; //graph to fit
  TVec<TIntFltH> F; // membership for each user (Size: Nodes * Coms)
  TRnd Rnd; // random number generator
  TIntV NIDV; // original node ID vector
  TFlt RegCoef; //Regularization coefficient when we fit for P_c +: L1, -: L2
  TFltV SumFV; // sum_u F_uc for each community c. Needed for efficient calculation
  TBool NodesOk; // Node ID is from 0 ~ N-1
  TInt NumComs; // number of communities
public:
  TVec<TIntSet> HOVIDSV; //NID pairs to hold out for cross validation
  TFlt MinVal; // minimum value of F (0)
  TFlt MaxVal; // maximum value of F (for numerical reason)
  TFlt NegWgt; // weight of negative example (a pair of nodes without an edge)
  TFlt PNoCom; // base probability \varepsilon (edge probability between a pair of nodes sharing no community
  TBool DoParallel; // whether to use parallelism for computation

  TAGMFast(const PUNGraph& GraphPt, const int& InitComs, const int RndSeed = 0): Rnd(RndSeed), RegCoef(0), 
    NodesOk(true), MinVal(0.0), MaxVal(1000.0), NegWgt(1.0) { SetGraph(GraphPt); RandomInit(InitComs); }
  void SetGraph(const PUNGraph& GraphPt);
  void SetRegCoef(const double _RegCoef) { RegCoef = _RegCoef; }
  double GetRegCoef() { return RegCoef; }
  void RandomInit(const int InitComs);
  void NeighborComInit(const int InitComs);
  void SetCmtyVV(const TVec<TIntV>& CmtyVV);
  double Likelihood(const bool DoParallel = false);
  double LikelihoodForRow(const int UID);
  double LikelihoodForRow(const int UID, const TIntFltH& FU);
  int MLENewton(const double& Thres, const int& MaxIter, const TStr PlotNm = TStr());
  void GradientForRow(const int UID, TIntFltH& GradU, const TIntSet& CIDSet);
  double GradientForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val);
  double HessianForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val);
  double LikelihoodForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val);
  void GetCmtyVV(TVec<TIntV>& CmtyVV);
  void GetCmtyVV(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz = 3);
  int FindComsByCV(TIntV& ComsV, const double HOFrac = 0.2, const int NumThreads = 20, const TStr PlotLFNm = TStr(), const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int FindComsByCV(const int NumThreads, const int MaxComs, const int MinComs, const int DivComs, const TStr OutFNm, const double StepAlpha = 0.3, const double StepBeta = 0.3);
  double LikelihoodHoldOut(const bool DoParallel = false);
  double GetStepSizeByLineSearch(const int UID, const TIntFltH& DeltaV, const TIntFltH& GradV, const double& Alpha, const double& Beta, const int MaxIter = 10);
  int MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const int ChunkSize, const TStr PlotNm, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const TStr PlotNm = TStr(), const double StepAlpha = 0.3, const double StepBeta = 0.1) {
    int ChunkSize = G->GetNodes() / 10 / ChunkNum;
    if (ChunkSize == 0) { ChunkSize = 1; }
    return MLEGradAscentParallel(Thres, MaxIter, ChunkNum, ChunkSize, PlotNm, StepAlpha, StepBeta);
  }
  //double FindOptimalThres(const TVec<TIntV>& TrueCmtyVV, TVec<TIntV>& CmtyVV);
  void Save(TSOut& SOut);
  void Load(TSIn& SIn, const int& RndSeed = 0);
  double inline GetCom(const int& NID, const int& CID) {
    if (F[NID].IsKey(CID)) {
      return F[NID].GetDat(CID);
    } else {
      return 0.0;
    }
  }
  void inline AddCom(const int& NID, const int& CID, const double& Val) {
    if (F[NID].IsKey(CID)) {
      SumFV[CID] -= F[NID].GetDat(CID);
    }
    F[NID].AddDat(CID) = Val;
    SumFV[CID] += Val;
  }

  void inline DelCom(const int& NID, const int& CID) {
    if (F[NID].IsKey(CID)) {
      SumFV[CID] -= F[NID].GetDat(CID);
      F[NID].DelKey(CID);
    }
  }
  double inline DotProduct(const TIntFltH& UV, const TIntFltH& VV) {
    double DP = 0;
    if (UV.Len() > VV.Len()) {
      for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
        if (VV.IsKey(HI.GetKey())) { 
          DP += VV.GetDat(HI.GetKey()) * HI.GetDat(); 
        }
      }
    } else {
      for (TIntFltH::TIter HI = VV.BegI(); HI < VV.EndI(); HI++) {
        if (UV.IsKey(HI.GetKey())) { 
          DP += UV.GetDat(HI.GetKey()) * HI.GetDat(); 
        }
      }
    }
    return DP;
  }
  double inline DotProduct(const int& UID, const int& VID) {
    return DotProduct(F[UID], F[VID]);
  }
  double inline Prediction(const TIntFltH& FU, const TIntFltH& FV) {
    double DP = log (1.0 / (1.0 - PNoCom)) + DotProduct(FU, FV);
    IAssertR(DP > 0.0, TStr::Fmt("DP: %f", DP));
    return exp(- DP);
  }
  double inline Prediction(const int& UID, const int& VID) {
    return Prediction(F[UID], F[VID]);
  }
  double inline Sum(const TIntFltH& UV) {
    double N = 0.0;
    for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
      N += HI.GetDat();
    }
    return N;
  }
  double inline Norm2(const TIntFltH& UV) {
    double N = 0.0;
    for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
      N += HI.GetDat() * HI.GetDat();
    }
    return N;
  }
};

#endif
