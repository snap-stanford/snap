#include "stdafx.h"
#include "agmfit.h"
#include "agm.h"

/////////////////////////////////////////////////
// AGM fitting

void TAGMFit::Save(TSOut& SOut) {
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

void TAGMFit::Load(TSIn& SIn, const int& RndSeed) {
  G = TUNGraph::Load(SIn);
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

// Randomly initialize bipartite community affiliation graph.
void TAGMFit::RandomInitCmtyVV(const int InitComs, const double ComSzAlpha, const double MemAlpha, const int MinComSz, const int MaxComSz, const int MinMem, const int MaxMem) {
  TVec<TIntV> InitCmtyVV(InitComs, 0);
  TAGMUtil::GenCmtyVVFromPL(InitCmtyVV, G, G->GetNodes(), InitComs, ComSzAlpha, MemAlpha, MinComSz, MaxComSz,
      MinMem, MaxMem, Rnd);
  SetCmtyVV(InitCmtyVV);
}

// For each (u, v) in edges, precompute C_uv (the set of communities u and v share).
void TAGMFit::GetEdgeJointCom() {
  ComEdgesV.Gen(CIDNSetV.Len());
  EdgeComVH.Gen(G->GetEdges());
  for (TUNGraph::TNodeI SrcNI = G->BegNI(); SrcNI < G->EndNI(); SrcNI++) {
    int SrcNID = SrcNI.GetId();
    for (int v = 0; v < SrcNI.GetDeg(); v++) {
      int DstNID = SrcNI.GetNbrNId(v);
      if (SrcNID >= DstNID) { continue; }
      TIntSet JointCom;
      IAssert(NIDComVH.IsKey(SrcNID));
      IAssert(NIDComVH.IsKey(DstNID));
      TAGMUtil::GetIntersection(NIDComVH.GetDat(SrcNID), NIDComVH.GetDat(DstNID), JointCom);
      EdgeComVH.AddDat(TIntPr(SrcNID,DstNID),JointCom);
      for (int k = 0; k < JointCom.Len(); k++) {
        ComEdgesV[JointCom[k]]++;
      }
    }
  }
  IAssert(EdgeComVH.Len() == G->GetEdges());
}

// Set epsilon by the default value.
void TAGMFit::SetDefaultPNoCom() {
  PNoCom = 1.0 / (double) G->GetNodes() / (double) G->GetNodes();
}

double TAGMFit::Likelihood(const TFltV& NewLambdaV, double& LEdges, double& LNoEdges) {
  IAssert(CIDNSetV.Len() == NewLambdaV.Len());
  IAssert(ComEdgesV.Len() == CIDNSetV.Len());
  LEdges = 0.0; LNoEdges = 0.0;
  for (int e = 0; e < EdgeComVH.Len(); e++) {
    TIntSet& JointCom = EdgeComVH[e];
    double LambdaSum = SelectLambdaSum(NewLambdaV, JointCom);
    double Puv = 1 - exp(- LambdaSum);
    if (JointCom.Len() == 0) {  Puv = PNoCom;  }
    IAssert(! _isnan(log(Puv)));
    LEdges += log(Puv);
  }
  for (int k = 0; k < NewLambdaV.Len(); k++) {
    int MaxEk = CIDNSetV[k].Len() * (CIDNSetV[k].Len() - 1) / 2;
    int NotEdgesInCom = MaxEk - ComEdgesV[k];
    if(NotEdgesInCom > 0) {
      if (LNoEdges >= TFlt::Mn + double(NotEdgesInCom) * NewLambdaV[k]) { 
        LNoEdges -= double(NotEdgesInCom) * NewLambdaV[k];
      }
    }
  }
  double LReg = 0.0;
  if (RegCoef > 0.0) {
    LReg = - RegCoef * TLinAlg::SumVec(NewLambdaV);
  }
  return LEdges + LNoEdges + LReg;
}

double TAGMFit::Likelihood() { 
  return Likelihood(LambdaV); 
}

// Step size search for updating P_c (which is parametarized by lambda).
double TAGMFit::GetStepSizeByLineSearchForLambda(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta) {
  double StepSize = 1.0;
  double InitLikelihood = Likelihood();
  IAssert(LambdaV.Len() == DeltaV.Len());
  TFltV NewLambdaV(LambdaV.Len());
  for (int iter = 0; ; iter++) {
    for (int i = 0; i < LambdaV.Len(); i++) {
      NewLambdaV[i] = LambdaV[i] + StepSize * DeltaV[i];
      if (NewLambdaV[i] < MinLambda) { NewLambdaV[i] = MinLambda; }
      if (NewLambdaV[i] > MaxLambda) { NewLambdaV[i] = MaxLambda; }
    }
    if (Likelihood(NewLambdaV) < InitLikelihood + Alpha * StepSize * TLinAlg::DotProduct(GradV, DeltaV)) {
      StepSize *= Beta;
    } else {
      break;
    }
  }
  return StepSize;
}

// Gradient descent for p_c while fixing community affiliation graph (CAG).
int TAGMFit::MLEGradAscentGivenCAG(const double& Thres, const int& MaxIter, const TStr PlotNm) {
  int Edges = G->GetEdges();
  TExeTm ExeTm;
  TFltV GradV(LambdaV.Len());
  int iter = 0;
  TIntFltPrV IterLV, IterGradNormV;
  double GradCutOff = 1000;
  for (iter = 0; iter < MaxIter; iter++) {
    GradLogLForLambda(GradV);    //if gradient is going out of the boundary, cut off
    for (int i = 0; i < LambdaV.Len(); i++) {
      if (GradV[i] < -GradCutOff) { GradV[i] = -GradCutOff; }
      if (GradV[i] > GradCutOff) { GradV[i] = GradCutOff; }
      if (LambdaV[i] <= MinLambda && GradV[i] < 0) { GradV[i] = 0.0; }
      if (LambdaV[i] >= MaxLambda && GradV[i] > 0) { GradV[i] = 0.0; }
    }
    double Alpha = 0.15, Beta = 0.2;
    if (Edges > Kilo(100)) { Alpha = 0.00015; Beta = 0.3;}
    double LearnRate = GetStepSizeByLineSearchForLambda(GradV, GradV, Alpha, Beta);
    if (TLinAlg::Norm(GradV) < Thres) { break; }
    for (int i = 0; i < LambdaV.Len(); i++) {
      double Change = LearnRate * GradV[i];
      LambdaV[i] += Change;
      if(LambdaV[i] < MinLambda) { LambdaV[i] = MinLambda;}
      if(LambdaV[i] > MaxLambda) { LambdaV[i] = MaxLambda;}
    }
    if (! PlotNm.Empty()) {
      double L = Likelihood();
      IterLV.Add(TIntFltPr(iter, L));
      IterGradNormV.Add(TIntFltPr(iter, TLinAlg::Norm(GradV)));
    }
  }
  if (! PlotNm.Empty()) {
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
    TGnuPlot::PlotValV(IterGradNormV, PlotNm + ".gradnorm_Q");
    printf("MLE for Lambda completed with %d iterations(%s)\n",iter,ExeTm.GetTmStr());
  }
  return iter;
}

void TAGMFit::RandomInit(const int& MaxK) {
  CIDNSetV.Clr();
  for (int c = 0; c < MaxK; c++) {
    CIDNSetV.Add();
    int NC = Rnd.GetUniDevInt(G -> GetNodes());
    TUNGraph::TNodeI NI = G -> GetRndNI();
    CIDNSetV.Last().AddKey(NI.GetId());
    for (int v = 0; v < NC; v++) {
      NI = G->GetNI(NI.GetNbrNId(Rnd.GetUniDevInt(NI.GetDeg())));
      CIDNSetV.Last().AddKey(NI.GetId());
    }
  }
  InitNodeData();
  SetDefaultPNoCom();
}

// Initialize node community memberships using best neighborhood communities (see D. Gleich et al. KDD'12).
void TAGMFit::NeighborComInit(const int InitComs) {
  CIDNSetV.Gen(InitComs);
  const int Edges = G->GetEdges();
  TFltIntPrV NIdPhiV(G->GetNodes(), 0);
  TIntSet InvalidNIDS(G->GetNodes());
  TIntV ChosenNIDV(InitComs, 0); //FOR DEBUG
  TExeTm RunTm;
  //compute conductance of neighborhood community
  TIntV NIdV;
  G->GetNIdV(NIdV);
  for (int u = 0; u < NIdV.Len(); u++) {
    TIntSet NBCmty(G->GetNI(NIdV[u]).GetDeg() + 1);
    double Phi;
    if (G->GetNI(NIdV[u]).GetDeg() < 5) { //do not include nodes with too few degree
      Phi = 1.0; 
    } else {
      TAGMUtil::GetNbhCom(G, NIdV[u], NBCmty);
      IAssert(NBCmty.Len() == G->GetNI(NIdV[u]).GetDeg() + 1);
      Phi = TAGMUtil::GetConductance(G, NBCmty, Edges);
    }
    NIdPhiV.Add(TFltIntPr(Phi, NIdV[u]));
  }
  NIdPhiV.Sort(true);
  printf("conductance computation completed [%s]\n", RunTm.GetTmStr());
  fflush(stdout);
  //choose nodes with local minimum in conductance
  int CurCID = 0;
  for (int ui = 0; ui < NIdPhiV.Len(); ui++) {
    int UID = NIdPhiV[ui].Val2;
    fflush(stdout);
    if (InvalidNIDS.IsKey(UID)) { continue; }
    ChosenNIDV.Add(UID); //FOR DEBUG
    //add the node and its neighbors to the current community
    CIDNSetV[CurCID].AddKey(UID);
    TUNGraph::TNodeI NI = G->GetNI(UID);
    fflush(stdout);
    for (int e = 0; e < NI.GetDeg(); e++) {
      CIDNSetV[CurCID].AddKey(NI.GetNbrNId(e));
    }
    //exclude its neighbors from the next considerations
    for (int e = 0; e < NI.GetDeg(); e++) {
      InvalidNIDS.AddKey(NI.GetNbrNId(e));
    }
    CurCID++;
    fflush(stdout);
    if (CurCID >= InitComs) { break;  }
  }
  if (InitComs > CurCID) {
    printf("%d communities needed to fill randomly\n", InitComs - CurCID);
  }
  //assign a member to zero-member community (if any)
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    if (CIDNSetV[c].Len() == 0) {
      int ComSz = 10;
      for (int u = 0; u < ComSz; u++) {
        int UID = G->GetRndNI().GetId();
        CIDNSetV[c].AddKey(UID);
      }
    }
  }
  InitNodeData();
  SetDefaultPNoCom();
}

// Add epsilon community (base community which includes all nodes) into community affiliation graph. It means that we fit for epsilon.
void TAGMFit::AddBaseCmty() {
  TVec<TIntV> CmtyVV;
  GetCmtyVV(CmtyVV);
  TIntV TmpV = CmtyVV[0];
  CmtyVV.Add(TmpV);
  G->GetNIdV(CmtyVV[0]);
  IAssert(CIDNSetV.Len() + 1 == CmtyVV.Len());
  SetCmtyVV(CmtyVV);
  InitNodeData();
  BaseCID = 0;
  PNoCom = 0.0;
}

void TAGMFit::InitNodeData() {
  TSnap::DelSelfEdges(G);
  NIDComVH.Gen(G->GetNodes());
  for (TUNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    NIDComVH.AddDat(NI.GetId());
  }
  TAGMUtil::GetNodeMembership(NIDComVH, CIDNSetV);
  GetEdgeJointCom();
  LambdaV.Gen(CIDNSetV.Len());
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    int MaxE = (CIDNSetV[c].Len()) * (CIDNSetV[c].Len() - 1) / 2;
    if (MaxE < 2) {
      LambdaV[c] = MaxLambda;
    }
    else{
      LambdaV[c] = -log((double) (MaxE - ComEdgesV[c]) / MaxE);
    }
    if (LambdaV[c] > MaxLambda) {  LambdaV[c] = MaxLambda;  }
    if (LambdaV[c] < MinLambda) {  LambdaV[c] = MinLambda;  }
  }
  NIDCIDPrS.Gen(G->GetNodes() * 10);
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    for (TIntSet::TIter SI = CIDNSetV[c].BegI(); SI < CIDNSetV[c].EndI(); SI++) {
      NIDCIDPrS.AddKey(TIntPr(SI.GetKey(), c));
    }
  }
}

// After MCMC, NID leaves community CID.
void TAGMFit::LeaveCom(const int& NID, const int& CID) {
  TUNGraph::TNodeI NI = G->GetNI(NID);
  for (int e = 0; e < NI.GetDeg(); e++) {
    int VID = NI.GetNbrNId(e);
    if (NIDComVH.GetDat(VID).IsKey(CID)) {
      TIntPr SrcDstNIDPr = TIntPr(TMath::Mn(NID,VID), TMath::Mx(NID,VID));
      EdgeComVH.GetDat(SrcDstNIDPr).DelKey(CID);
      ComEdgesV[CID]--;
    }
  }
  CIDNSetV[CID].DelKey(NID);
  NIDComVH.GetDat(NID).DelKey(CID);
  NIDCIDPrS.DelKey(TIntPr(NID, CID));
}

// After MCMC, NID joins community CID.
void TAGMFit::JoinCom(const int& NID, const int& JoinCID) {
  TUNGraph::TNodeI NI = G->GetNI(NID);
  for (int e = 0; e < NI.GetDeg(); e++) {
    int VID = NI.GetNbrNId(e);
    if (NIDComVH.GetDat(VID).IsKey(JoinCID)) {
      TIntPr SrcDstNIDPr = TIntPr(TMath::Mn(NID,VID), TMath::Mx(NID,VID));
      EdgeComVH.GetDat(SrcDstNIDPr).AddKey(JoinCID);
      ComEdgesV[JoinCID]++;
    }
  }
  CIDNSetV[JoinCID].AddKey(NID);
  NIDComVH.GetDat(NID).AddKey(JoinCID);
  NIDCIDPrS.AddKey(TIntPr(NID, JoinCID));
}

// Sample transition: Choose among (join, leave, switch), and then sample (NID, CID).
void TAGMFit::SampleTransition(int& NID, int& JoinCID, int& LeaveCID, double& DeltaL) {
  int Option = Rnd.GetUniDevInt(3); //0:Join 1:Leave 2:Switch
  if (NIDCIDPrS.Len() <= 1) {    Option = 0;  } //if there is only one node membership, only join is possible.
  int TryCnt = 0;
  const int MaxTryCnt = G->GetNodes();
  DeltaL = TFlt::Mn;
  if (Option == 0) {
    do {
      JoinCID = Rnd.GetUniDevInt(CIDNSetV.Len());
      NID = G->GetRndNId();
    } while (TryCnt++ < MaxTryCnt && NIDCIDPrS.IsKey(TIntPr(NID, JoinCID)));
    if (TryCnt < MaxTryCnt) { //if successfully find a move
      DeltaL = SeekJoin(NID, JoinCID);
    }
  }
  else if (Option == 1) {
    do {
      TIntPr NIDCIDPr = NIDCIDPrS.GetKey(NIDCIDPrS.GetRndKeyId(Rnd));
      NID = NIDCIDPr.Val1;
      LeaveCID = NIDCIDPr.Val2;
    } while (TryCnt++ < MaxTryCnt && LeaveCID == BaseCID);
    if (TryCnt < MaxTryCnt) {//if successfully find a move
      DeltaL = SeekLeave(NID, LeaveCID);
    }
  }
  else{
    do {
      TIntPr NIDCIDPr = NIDCIDPrS.GetKey(NIDCIDPrS.GetRndKeyId(Rnd));
      NID = NIDCIDPr.Val1;
      LeaveCID = NIDCIDPr.Val2;
    } while (TryCnt++ < MaxTryCnt && (NIDComVH.GetDat(NID).Len() == CIDNSetV.Len() || LeaveCID == BaseCID));
    do {
      JoinCID = Rnd.GetUniDevInt(CIDNSetV.Len());
    } while (TryCnt++ < G->GetNodes() && NIDCIDPrS.IsKey(TIntPr(NID, JoinCID)));
    if (TryCnt < MaxTryCnt) {//if successfully find a move
      DeltaL = SeekSwitch(NID, LeaveCID, JoinCID);
    }
  }
}

/// MCMC fitting
void TAGMFit::RunMCMC(const int& MaxIter, const int& EvalLambdaIter, const TStr& PlotFPrx) {
  TExeTm IterTm, TotalTm;
  double PrevL = Likelihood(), DeltaL = 0;
  double BestL = PrevL;
  printf("initial likelihood = %f\n",PrevL);
  TIntFltPrV IterTrueLV, IterJoinV, IterLeaveV, IterAcceptV, IterSwitchV, IterLBV;
  TIntPrV IterTotMemV;
  TIntV IterV;
  TFltV BestLV;
  TVec<TIntSet> BestCmtySetV;
  int SwitchCnt = 0, LeaveCnt = 0, JoinCnt = 0, AcceptCnt = 0, ProbBinSz;
  int Nodes = G->GetNodes(), Edges = G->GetEdges();
  TExeTm PlotTm;
  ProbBinSz = TMath::Mx(1000, G->GetNodes() / 10); //bin to compute probabilities
  IterLBV.Add(TIntFltPr(1, BestL));

  for (int iter = 0; iter < MaxIter; iter++) {
    IterTm.Tick();
    int NID = -1;
    int JoinCID = -1, LeaveCID = -1;
    SampleTransition(NID, JoinCID, LeaveCID, DeltaL); //sample a move
    double OptL = PrevL;
    if (DeltaL > 0 || Rnd.GetUniDev() < exp(DeltaL)) { //if it is accepted
      IterTm.Tick();
      if (LeaveCID > -1 && LeaveCID != BaseCID) { LeaveCom(NID, LeaveCID); }
      if (JoinCID > -1 && JoinCID != BaseCID) { JoinCom(NID, JoinCID); }
      if (LeaveCID > -1 && JoinCID > -1 && JoinCID != BaseCID && LeaveCID != BaseCID) { SwitchCnt++; }
      else if (LeaveCID > -1  && LeaveCID != BaseCID) { LeaveCnt++;}
      else if (JoinCID > -1 && JoinCID != BaseCID) { JoinCnt++;}
      AcceptCnt++;
      if ((iter + 1) % EvalLambdaIter == 0) {
        IterTm.Tick();
        MLEGradAscentGivenCAG(0.01, 3);
        OptL = Likelihood();
      }
      else{
        OptL = PrevL + DeltaL;
      }
      if (BestL <= OptL && CIDNSetV.Len() > 0) {
        BestCmtySetV = CIDNSetV;
        BestLV = LambdaV;
        BestL = OptL;
      }
    }
    if (iter > 0 && (iter % ProbBinSz == 0) && PlotFPrx.Len() > 0) { 
      IterLBV.Add(TIntFltPr(iter, OptL));
      IterSwitchV.Add(TIntFltPr(iter, (double) SwitchCnt / (double) AcceptCnt));
      IterLeaveV.Add(TIntFltPr(iter, (double) LeaveCnt / (double) AcceptCnt));
      IterJoinV.Add(TIntFltPr(iter, (double) JoinCnt / (double) AcceptCnt));
      IterAcceptV.Add(TIntFltPr(iter, (double) AcceptCnt / (double) ProbBinSz));
      SwitchCnt = JoinCnt = LeaveCnt = AcceptCnt = 0;
    }
    PrevL = OptL;
    if ((iter + 1) % 10000 == 0) {
      printf("\r%d iterations completed [%.2f]", iter, (double) iter / (double) MaxIter);
    }
  }
  
  // plot the likelihood and acceptance probabilities if the plot file name is given
  if (PlotFPrx.Len() > 0) {
    TGnuPlot GP1;
    GP1.AddPlot(IterLBV, gpwLinesPoints, "likelihood");
    GP1.SetDataPlotFNm(PlotFPrx + ".likelihood.tab", PlotFPrx + ".likelihood.plt");
    TStr TitleStr = TStr::Fmt(" N:%d E:%d", Nodes, Edges);
    GP1.SetTitle(PlotFPrx + ".likelihood" + TitleStr);
    GP1.SavePng(PlotFPrx + ".likelihood.png");

    TGnuPlot GP2;
    GP2.AddPlot(IterSwitchV, gpwLinesPoints, "Switch");
    GP2.AddPlot(IterLeaveV, gpwLinesPoints, "Leave");
    GP2.AddPlot(IterJoinV, gpwLinesPoints, "Join");
    GP2.AddPlot(IterAcceptV, gpwLinesPoints, "Accept");
    GP2.SetTitle(PlotFPrx + ".transition");
    GP2.SetDataPlotFNm(PlotFPrx + "transition_prob.tab", PlotFPrx + "transition_prob.plt");
    GP2.SavePng(PlotFPrx + "transition_prob.png");
  }
  CIDNSetV = BestCmtySetV;
  LambdaV = BestLV;

  InitNodeData();
  MLEGradAscentGivenCAG(0.001, 100);
  printf("\nMCMC completed (best likelihood: %.2f) [%s]\n", BestL, TotalTm.GetTmStr());
}

// Returns \v QV, a vector of (1 - p_c) for each community c.
void TAGMFit::GetQV(TFltV& OutV) {
  OutV.Gen(LambdaV.Len());
  for (int i = 0; i < LambdaV.Len(); i++) {
    OutV[i] = exp(- LambdaV[i]);
  }
}

// Remove empty communities.
int TAGMFit::RemoveEmptyCom() {
  int DelCnt = 0;
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    if (CIDNSetV[c].Len() == 0) {
      CIDNSetV[c] = CIDNSetV.Last();
      CIDNSetV.DelLast();
      LambdaV[c] = LambdaV.Last();
      LambdaV.DelLast();
      DelCnt++;
      c--;
    }
  }
  return DelCnt;
}

// Compute the change in likelihood (Delta) if node UID leaves community CID.
double TAGMFit::SeekLeave(const int& UID, const int& CID) {
  IAssert(CIDNSetV[CID].IsKey(UID));
  IAssert(G->IsNode(UID));
  double Delta = 0.0;
  TUNGraph::TNodeI NI = G->GetNI(UID);
  int NbhsInC = 0;
  for (int e = 0; e < NI.GetDeg(); e++) {
    const int VID = NI.GetNbrNId(e);
    if (! NIDComVH.GetDat(VID).IsKey(CID)) { continue; }
    TIntPr SrcDstNIDPr(TMath::Mn(UID,VID), TMath::Mx(UID,VID));
    TIntSet& JointCom = EdgeComVH.GetDat(SrcDstNIDPr);
    double CurPuv, NewPuv, LambdaSum = SelectLambdaSum(JointCom);
    CurPuv = 1 - exp(- LambdaSum);
    NewPuv = 1 - exp(- LambdaSum + LambdaV[CID]);
    IAssert(JointCom.Len() > 0);
    if (JointCom.Len() == 1) {
      NewPuv = PNoCom;
    }
    Delta += (log(NewPuv) - log(CurPuv));
    IAssert(!_isnan(Delta));
    NbhsInC++;
  }
  Delta += LambdaV[CID] * (CIDNSetV[CID].Len() - 1 - NbhsInC);
  return Delta;
}

// Compute the change in likelihood (Delta) if node UID joins community CID.
double TAGMFit::SeekJoin(const int& UID, const int& CID) {
  IAssert(! CIDNSetV[CID].IsKey(UID));
  double Delta = 0.0;
  TUNGraph::TNodeI NI = G->GetNI(UID);
  int NbhsInC = 0;
  for (int e = 0; e < NI.GetDeg(); e++) {
    const int VID = NI.GetNbrNId(e);
    if (! NIDComVH.GetDat(VID).IsKey(CID)) { continue; }
    TIntPr SrcDstNIDPr(TMath::Mn(UID,VID), TMath::Mx(UID,VID));
    TIntSet& JointCom = EdgeComVH.GetDat(SrcDstNIDPr);
    double CurPuv, NewPuv, LambdaSum = SelectLambdaSum(JointCom);
    CurPuv = 1 - exp(- LambdaSum);
    if (JointCom.Len() == 0) { CurPuv = PNoCom; }
    NewPuv = 1 - exp(- LambdaSum - LambdaV[CID]);
    Delta += (log(NewPuv) - log(CurPuv));
    IAssert(!_isnan(Delta));
    NbhsInC++;
  }
  Delta -= LambdaV[CID] * (CIDNSetV[CID].Len() - NbhsInC);
  return Delta;
}

// Compute the change in likelihood (Delta) if node UID switches from CurCID to NewCID.
double TAGMFit::SeekSwitch(const int& UID, const int& CurCID, const int& NewCID) {
  IAssert(! CIDNSetV[NewCID].IsKey(UID));
  IAssert(CIDNSetV[CurCID].IsKey(UID));
  double Delta = SeekJoin(UID, NewCID) + SeekLeave(UID, CurCID);
  //correct only for intersection between new com and current com
  TUNGraph::TNodeI NI = G->GetNI(UID);
  for (int e = 0; e < NI.GetDeg(); e++) {
    const int VID = NI.GetNbrNId(e);
    if (! NIDComVH.GetDat(VID).IsKey(CurCID) || ! NIDComVH.GetDat(VID).IsKey(NewCID)) {continue;}
    TIntPr SrcDstNIDPr(TMath::Mn(UID,VID), TMath::Mx(UID,VID));
    TIntSet& JointCom = EdgeComVH.GetDat(SrcDstNIDPr);
    double CurPuv, NewPuvAfterJoin, NewPuvAfterLeave, NewPuvAfterSwitch, LambdaSum = SelectLambdaSum(JointCom);
    CurPuv = 1 - exp(- LambdaSum);
    NewPuvAfterLeave = 1 - exp(- LambdaSum + LambdaV[CurCID]);
    NewPuvAfterJoin = 1 - exp(- LambdaSum - LambdaV[NewCID]);
    NewPuvAfterSwitch = 1 - exp(- LambdaSum - LambdaV[NewCID] + LambdaV[CurCID]);
    if (JointCom.Len() == 1 || NewPuvAfterLeave == 0.0) {
      NewPuvAfterLeave = PNoCom;
    }
    Delta += (log(NewPuvAfterSwitch) + log(CurPuv) - log(NewPuvAfterLeave) - log(NewPuvAfterJoin));
    if (_isnan(Delta)) {
      printf("NS:%f C:%f NL:%f NJ:%f PNoCom:%f", NewPuvAfterSwitch, CurPuv, NewPuvAfterLeave, NewPuvAfterJoin, PNoCom.Val);
    }
    IAssert(!_isnan(Delta));
  }
  return Delta;
}

// Get communities whose p_c is higher than 1 - QMax.
void TAGMFit::GetCmtyVV(TVec<TIntV>& CmtyVV, const double QMax) {
  TFltV TmpQV;
  GetCmtyVV(CmtyVV, TmpQV, QMax);
}

void TAGMFit::GetCmtyVV(TVec<TIntV>& CmtyVV, TFltV& QV, const double QMax) {
  CmtyVV.Gen(CIDNSetV.Len(), 0);
  QV.Gen(CIDNSetV.Len(), 0);
  TIntFltH CIDLambdaH(CIDNSetV.Len());
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    CIDLambdaH.AddDat(c, LambdaV[c]);
  }
  CIDLambdaH.SortByDat(false);
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    int CID = CIDLambdaH.GetKey(c);
    IAssert(LambdaV[CID] >= MinLambda);
    double Q = exp( - (double) LambdaV[CID]);
    if (Q > QMax) { continue; }
    TIntV CmtyV;
    CIDNSetV[CID].GetKeyV(CmtyV);
    if (CmtyV.Len() == 0) { continue; }
    if (CID == BaseCID) { //if the community is the base community(epsilon community), discard
      IAssert(CmtyV.Len() == G->GetNodes());
    } else {
      CmtyVV.Add(CmtyV);
      QV.Add(Q);
    }
  }
}

void TAGMFit::SetCmtyVV(const TVec<TIntV>& CmtyVV) {
  CIDNSetV.Gen(CmtyVV.Len());
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    CIDNSetV[c].AddKeyV(CmtyVV[c]);
    for (int j = 0; j < CmtyVV[c].Len(); j++) { IAssert(G->IsNode(CmtyVV[c][j])); } // check whether the member nodes exist in the graph
  }
  InitNodeData();
  SetDefaultPNoCom();
}

// Gradient of likelihood for P_c.
void TAGMFit::GradLogLForLambda(TFltV& GradV) {
  GradV.Gen(LambdaV.Len());
  TFltV SumEdgeProbsV(LambdaV.Len());
  for (int e = 0; e < EdgeComVH.Len(); e++) {
    TIntSet& JointCom = EdgeComVH[e];
    double LambdaSum = SelectLambdaSum(JointCom);
    double Puv = 1 - exp(- LambdaSum);
    if (JointCom.Len() == 0) {  Puv = PNoCom;  }
    for (TIntSet::TIter SI = JointCom.BegI(); SI < JointCom.EndI(); SI++) {
      SumEdgeProbsV[SI.GetKey()] += (1 - Puv) / Puv;
    }
  }
  for (int k = 0; k < LambdaV.Len(); k++) {
    int MaxEk = CIDNSetV[k].Len() * (CIDNSetV[k].Len() - 1) / 2;
    int NotEdgesInCom = MaxEk - ComEdgesV[k];
    GradV[k] = SumEdgeProbsV[k] - (double) NotEdgesInCom;
    if (LambdaV[k] > 0.0 && RegCoef > 0.0) { //if regularization exists
      GradV[k] -= RegCoef;
    }
  }
}

// Compute sum of lambda_c (which is log (1 - p_c)) over C_uv (ComK). It is used to compute edge probability P_uv.
double TAGMFit::SelectLambdaSum(const TIntSet& ComK) { 
  return SelectLambdaSum(LambdaV, ComK); 
}

double TAGMFit::SelectLambdaSum(const TFltV& NewLambdaV, const TIntSet& ComK) {
  double Result = 0.0;
  for (TIntSet::TIter SI = ComK.BegI(); SI < ComK.EndI(); SI++) {
    IAssert(NewLambdaV[SI.GetKey()] >= 0);
    Result += NewLambdaV[SI.GetKey()];
  }
  return Result;
}

// Compute the empirical edge probability between a pair of nodes who share no community (epsilon), based on current community affiliations.
double TAGMFit::CalcPNoComByCmtyVV(const int& SamplePairs) {
  TIntV NIdV;
  G->GetNIdV(NIdV);
  uint64 PairNoCom = 0, EdgesNoCom = 0;
  for (int u = 0; u < NIdV.Len(); u++) {
    for (int v = u + 1; v < NIdV.Len(); v++) {
      int SrcNID = NIdV[u], DstNID = NIdV[v];
      TIntSet JointCom;
      TAGMUtil::GetIntersection(NIDComVH.GetDat(SrcNID),NIDComVH.GetDat(DstNID),JointCom);
      if(JointCom.Len() == 0) {
        PairNoCom++;
        if (G->IsEdge(SrcNID, DstNID)) { EdgesNoCom++; }
        if (SamplePairs > 0 && PairNoCom >= (uint64) SamplePairs) { break; }
      }
    }
    if (SamplePairs > 0 && PairNoCom >= (uint64) SamplePairs) { break; }
  }
  double DefaultVal = 1.0 / (double)G->GetNodes() / (double)G->GetNodes();
  if (EdgesNoCom > 0) {
    PNoCom = (double) EdgesNoCom / (double) PairNoCom;
  } else {
    PNoCom = DefaultVal;
  }
  printf("%s / %s edges without joint com detected (PNoCom = %f)\n", TUInt64::GetStr(EdgesNoCom).CStr(), TUInt64::GetStr(PairNoCom).CStr(), PNoCom.Val);
  return PNoCom;
}

void TAGMFit::PrintSummary() {
  TIntFltH CIDLambdaH(CIDNSetV.Len());
  for (int c = 0; c < CIDNSetV.Len(); c++) {
    CIDLambdaH.AddDat(c, LambdaV[c]);
  }
  CIDLambdaH.SortByDat(false);
  int Coms = 0;
  for (int i = 0; i < LambdaV.Len(); i++) {
    int CID = CIDLambdaH.GetKey(i);
    if (LambdaV[CID] <= 0.0001) { continue; }
    printf("P_c : %.3f Com Sz: %d, Total Edges inside: %d \n", 1.0 - exp(- LambdaV[CID]), CIDNSetV[CID].Len(), (int) ComEdgesV[CID]);
    Coms++;
  }
  printf("%d Communities, Total Memberships = %d, Likelihood = %.2f, Epsilon = %f\n", Coms, NIDCIDPrS.Len(), Likelihood(), PNoCom.Val);
}
