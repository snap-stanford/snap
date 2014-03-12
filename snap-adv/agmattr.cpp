
#include "stdafx.h"
#include "agmattr.h"
#include "Snap.h"
#include "agm.h"



void TCesna::RandomInit(const int InitComs) {
  F.Gen(G->GetNodes());
  SumFV.Gen(InitComs);
  NumComs = InitComs;
  for (int u = 0; u < F.Len(); u++) {
    //assign to just one community
    int Mem = G->GetNI(u).GetDeg();
    if (Mem > 10) { Mem = 10; }
    for (int c = 0; c < Mem; c++) {
      int CID = Rnd.GetUniDevInt(InitComs);
      AddCom(u, CID, Rnd.GetUniDev());
    }
  }
  //assign a member to zero-member community (if any)
  for (int c = 0; c < SumFV.Len(); c++) {
    if (SumFV[c] == 0.0) {
      int UID = Rnd.GetUniDevInt(G->GetNodes());
      AddCom(UID, c, Rnd.GetUniDev());
    }
  }
  InitW();
}

void TCesna::NeighborComInit(const int InitComs) {
  //initialize with best neighborhood communities (Gleich et.al. KDD'12)
  TFltIntPrV NIdPhiV(F.Len(), 0);
  TCesnaUtil::GetNIdPhiV<PUNGraph>(G, NIdPhiV);
  NeighborComInit(NIdPhiV, InitComs);
}

void TCesna::NeighborComInit(TFltIntPrV& NIdPhiV, const int InitComs) {
  //initialize with best neighborhood communities (Gleich et.al. KDD'12)
  NIdPhiV.Sort(true);
  F.Gen(G->GetNodes());
  SumFV.Gen(InitComs);
  NumComs = InitComs;
  TIntSet InvalidNIDS(F.Len());
  TIntV ChosenNIDV(InitComs, 0); //FOR DEBUG
  //choose nodes with local minimum in conductance
  int CurCID = 0;
  for (int ui = 0; ui < NIdPhiV.Len(); ui++) {
    int UID = NIdPhiV[ui].Val2;
    fflush(stdout);
    if (InvalidNIDS.IsKey(UID)) { continue; }
    ChosenNIDV.Add(UID); //FOR DEBUG
    //add the node and its neighbors to the current community
    AddCom(UID, CurCID, 1.0);
    TUNGraph::TNodeI NI = G->GetNI(UID);
    fflush(stdout);
    for (int e = 0; e < NI.GetDeg(); e++) {
      AddCom(NI.GetNbrNId(e), CurCID, 1.0);
    }
    //exclude its neighbors from the next considerations
    for (int e = 0; e < NI.GetDeg(); e++) {
      InvalidNIDS.AddKey(NI.GetNbrNId(e));
    }
    CurCID++;
    fflush(stdout);
    if (CurCID >= NumComs) { break;  }
  }
  if (NumComs > CurCID) {
    printf("%d communities needed to fill randomly\n", NumComs - CurCID);
  }
  //assign a member to zero-member community (if any)
  for (int c = 0; c < SumFV.Len(); c++) {
    if (SumFV[c] == 0.0) {
      int ComSz = 10;
      for (int u = 0; u < ComSz; u++) {
        int UID = Rnd.GetUniDevInt(G->GetNodes());
        AddCom(UID, c, Rnd.GetUniDev());
      }
    }
  }
  InitW();
}

void TCesna::SetCmtyVV(const TVec<TIntV>& CmtyVV) {
  F.Gen(G->GetNodes());
  SumFV.Gen(CmtyVV.Len());
  NumComs = CmtyVV.Len();
  InitW();
  for (int c = 0; c < CmtyVV.Len(); c++) {
    for (int u = 0; u < CmtyVV[c].Len(); u++) {
      int UID = CmtyVV[c][u];
      if (! NIDToIdx.IsKey(UID)) { continue; }
      AddCom(NIDToIdx.GetKeyId(UID), c, 1.0);
    }
  }
}

void TCesna::SetGraph(const PUNGraph& GraphPt, const THash<TInt, TIntV>& NIDAttrH) {
  HOVIDSV.Gen(GraphPt->GetNodes());  
  HOKIDSV.Gen(GraphPt->GetNodes());  
  X.Gen(GraphPt->GetNodes());
  TIntV NIDV;
  GraphPt->GetNIdV(NIDV);
  NIDToIdx.Gen(NIDV.Len());
  NIDToIdx.AddKeyV(NIDV);
  // check that nodes IDs are {0,1,..,Nodes-1}
  printf("rearrage nodes\n");
  G = TSnap::GetSubGraph(GraphPt, NIDV, true);
  for (int nid = 0; nid < G->GetNodes(); nid++) {
    IAssert(G->IsNode(nid)); 
  }
  TSnap::DelSelfEdges(G);
  
  PNoCom = 1.0 / (double) G->GetNodes();
  DoParallel = false;
  if (1.0 / PNoCom > sqrt(TFlt::Mx)) { PNoCom = 0.99 / sqrt(TFlt::Mx); } // to prevent overflow
  NegWgt = 1.0;

  // add node attributes, find number of attributes
  int NumAttr = 0;
  for (int u = 0; u < NIDAttrH.Len(); u++) {
    int UID = NIDAttrH.GetKey(u);
    if (! NIDToIdx.IsKey(UID)) { continue; }
    X[NIDToIdx.GetKeyId(UID)].Gen(NIDAttrH[u].Len());
    for (int k = 0; k < NIDAttrH[u].Len(); k++) {
      int KID = NIDAttrH[u][k];
      IAssert (KID >= 0);
      X[NIDToIdx.GetKeyId(UID)].AddKey(KID);
      if (NumAttr < KID + 1) { NumAttr = KID + 1; } 
    }
  }
  Attrs = NumAttr;
  InitW();
}

double TCesna::Likelihood(const bool _DoParallel) { 
  TExeTm ExeTm;
  double L = 0.0;
  if (_DoParallel) {
  #pragma omp parallel for 
    for (int u = 0; u < F.Len(); u++) {
      double LU = LikelihoodForRow(u);
      #pragma omp atomic
        L += LU;
    }
  }
  else {
    for (int u = 0; u < F.Len(); u++) {
      double LU = LikelihoodForRow(u);
        L += LU;
    }
  }
  return L;
}

double TCesna::LikelihoodForRow(const int UID) {
  return LikelihoodForRow(UID, F[UID]);
}

double TCesna::LikelihoodForRow(const int UID, const TIntFltH& FU) {
  double L = 0.0;
  TFltV HOSumFV; //adjust for Fv of v hold out
  if (HOVIDSV[UID].Len() > 0) {
    HOSumFV.Gen(SumFV.Len());
    
    for (int e = 0; e < HOVIDSV[UID].Len(); e++) {
      for (int c = 0; c < SumFV.Len(); c++) {
        HOSumFV[c] += GetCom(HOVIDSV[UID][e], c);
      }
    }
  }

  TUNGraph::TNodeI NI = G->GetNI(UID);
  for (int e = 0; e < NI.GetDeg(); e++) {
    int v = NI.GetNbrNId(e);
    if (v == UID) { continue; }
    if (HOVIDSV[UID].IsKey(v)) { continue; }
    L += log (1.0 - Prediction(FU, F[v])) + NegWgt * DotProduct(FU, F[v]);
  }
  for (TIntFltH::TIter HI = FU.BegI(); HI < FU.EndI(); HI++) {
    double HOSum = HOVIDSV[UID].Len() > 0?  HOSumFV[HI.GetKey()].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    L -= NegWgt * (SumFV[HI.GetKey()] - HOSum - GetCom(UID, HI.GetKey())) * HI.GetDat();
  }
  //add regularization
  if (RegCoef > 0.0) { //L1
    L -= RegCoef * Sum(FU);
  }
  if (RegCoef < 0.0) { //L2
    L += RegCoef * Norm2(FU);
  }
  L *= (1.0 - WeightAttr);
  // add attribute part
  for (int k = 0; k < Attrs; k++) {
    if (HOKIDSV[UID].IsKey(k)) { continue; }
    L += WeightAttr * LikelihoodAttrKForRow(UID, k, FU);
  }
  return L;
}


double TCesna::LikelihoodAttrKForRow(const int UID, const int K, const TIntFltH& FU, const TFltV& WK) {
  double Prob = PredictAttrK(FU, WK);
  double L = 0.0;
  if (GetAttr(UID, K)) { 
    L = Prob == 0.0? -100.0: log(Prob);
  } else {
    L = Prob == 1.0? -100.0: log(1.0 - Prob);
  }
  return L;
}

void TCesna::GradientForRow(const int UID, TIntFltH& GradU, const TIntSet& CIDSet) {
  GradU.Gen(CIDSet.Len());
  TFltV HOSumFV; //adjust for Fv of v hold out
  if (HOVIDSV[UID].Len() > 0) {
    HOSumFV.Gen(SumFV.Len());
    
    for (int e = 0; e < HOVIDSV[UID].Len(); e++) {
      for (int c = 0; c < SumFV.Len(); c++) {
        HOSumFV[c] += GetCom(HOVIDSV[UID][e], c);
      }
    }
  }
  TUNGraph::TNodeI NI = G->GetNI(UID);
  int Deg = NI.GetDeg();
  TFltV PredV(Deg), GradV(CIDSet.Len());
  TIntV CIDV(CIDSet.Len());
  for (int e = 0; e < Deg; e++) {
    if (NI.GetNbrNId(e) == UID) { continue; }
    if (HOVIDSV[UID].IsKey(NI.GetNbrNId(e))) { continue; }
    PredV[e] = Prediction(UID, NI.GetNbrNId(e));
  }
  
  for (int c = 0; c < CIDSet.Len(); c++) {
    int CID = CIDSet.GetKey(c);
    double Val = 0.0;
    for (int e = 0; e < Deg; e++) {
      int VID = NI.GetNbrNId(e);
      if (VID == UID) { continue; }
      if (HOVIDSV[UID].IsKey(VID)) { continue; }
      Val += PredV[e] * GetCom(VID, CID) / (1.0 - PredV[e]) + NegWgt * GetCom(VID, CID);
    }
    double HOSum = HOVIDSV[UID].Len() > 0?  HOSumFV[CID].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    Val -= NegWgt * (SumFV[CID] - HOSum - GetCom(UID, CID));
    CIDV[c] = CID;
    GradV[c] = Val;
  }
  //add regularization
  if (RegCoef > 0.0) { //L1
    for (int c = 0; c < GradV.Len(); c++) {
      GradV[c] -= RegCoef; 
    }
  }
  if (RegCoef < 0.0) { //L2
    for (int c = 0; c < GradV.Len(); c++) {
      GradV[c] += 2 * RegCoef * GetCom(UID, CIDV[c]); 
    }
  }
  for (int c = 0; c < GradV.Len(); c++) {
    GradV[c] *= (1.0 - WeightAttr);
  }
  //add attribute part
  TFltV AttrPredV(Attrs);
  for (int k = 0; k < Attrs; k++) {
    if (HOKIDSV[UID].IsKey(k)) { continue; }
    AttrPredV[k] = PredictAttrK(F[UID], W[k]);
  }
  for (int c = 0; c < GradV.Len(); c++) {
    for (int k = 0; k < Attrs; k++) {
      if (HOKIDSV[UID].IsKey(k)) { continue; }
      GradV[c] += WeightAttr * (GetAttr(UID, k) - AttrPredV[k]) * GetW(CIDV[c], k);
    }
  }


  for (int c = 0; c < GradV.Len(); c++) {
    if (GetCom(UID, CIDV[c]) == 0.0 && GradV[c] < 0.0) { continue; }
    if (fabs(GradV[c]) < 0.0001) { continue; }
    GradU.AddDat(CIDV[c], GradV[c]);
  }
  for (int c = 0; c < GradU.Len(); c++) {
    if (GradU[c] >= 10) { GradU[c] = 10; }
    if (GradU[c] <= -10) { GradU[c] = -10; }
    IAssert(GradU[c] >= -10);
  }
}

void TCesna::GetCmtyVV(TVec<TIntV>& CmtyVV) {
  TVec<TFltV> TmpV;
  GetCmtyVV(CmtyVV, TmpV, sqrt(2.0 * (double) G->GetEdges() / G->GetNodes() / G->GetNodes()), 3);
}


/// extract community affiliation from F_uc Wck[c][k] = W_c for k-th attribute
void TCesna::GetCmtyVV(TVec<TIntV>& CmtyVV, TVec<TFltV>& Wck, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
  Wck.Gen(NumComs, 0);
  TIntFltH CIDSumFH(NumComs);
  for (int c = 0; c < SumFV.Len(); c++) {
    CIDSumFH.AddDat(c, SumFV[c]);
  }
  CIDSumFH.SortByDat(false);
  for (int c = 0; c < NumComs; c++) {
    int CID = CIDSumFH.GetKey(c);
    TIntFltH NIDFucH(F.Len() / 10);
    TIntV CmtyV;
    IAssert(SumFV[CID] == CIDSumFH.GetDat(CID));
    if (SumFV[CID] < Thres) { continue; }
    for (int u = 0; u < F.Len(); u++) {
      int NID = NIDToIdx[u];
      if (GetCom(u, CID) >= Thres) { NIDFucH.AddDat(NID, GetCom(u, CID)); }
    }
    NIDFucH.SortByDat(false);
    NIDFucH.GetKeyV(CmtyV);
    if (CmtyV.Len() < MinSz) { continue; }
    CmtyVV.Add(CmtyV); 
    TFltV WV(Attrs);
    for (int k = 0; k < Attrs; k++) {
      WV[k] = GetW(CID, k);
    }
    Wck.Add(WV);
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}

void TCesna::GetCmtyVVUnSorted(TVec<TIntV>& CmtyVV) {
  GetCmtyVVUnSorted(CmtyVV, sqrt(2.0 * (double) G->GetEdges() / G->GetNodes() / G->GetNodes()), 3);
}

void TCesna::GetCmtyVVUnSorted(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
  for (int c = 0; c < NumComs; c++) {
    TIntV CmtyV;
    for (int u = 0; u < G->GetNodes(); u++) {
      if (GetCom(u, c) > Thres) { CmtyV.Add(NIDToIdx[u]); }
    }
    if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}

/// estimate number of communities using cross validation
int TCesna::FindComs(const int NumThreads, const int MaxComs, const int MinComs, const int DivComs, const TStr OutFNm, const bool UseBIC, const double HOFrac, const double StepAlpha, const double StepBeta) {
    double ComsGap = exp(TMath::Log((double) MaxComs / (double) MinComs) / (double) DivComs);
    TIntV ComsV;
    ComsV.Add(MinComs);
    while (ComsV.Len() < DivComs) {
      int NewComs = int(ComsV.Last() * ComsGap);
      if (NewComs == ComsV.Last().Val) { NewComs++; }
      ComsV.Add(NewComs);
    }
    if (ComsV.Last() < MaxComs) { ComsV.Add(MaxComs); }
    return FindComs(ComsV, UseBIC, HOFrac, NumThreads, OutFNm, StepAlpha, StepBeta);
}

int TCesna::FindComs(TIntV& ComsV, const bool UseBIC, const double HOFrac, const int NumThreads, const TStr PlotLFNm, const double StepAlpha, const double StepBeta) {
  if (ComsV.Len() == 0) {
    int MaxComs = G->GetNodes() / 5;
    ComsV.Add(2);
    while(ComsV.Last() < MaxComs) { ComsV.Add(ComsV.Last() * 2); }
  }
  int MaxIterCV = 3;

  TVec<TVec<TIntSet> > HoldOutSets(MaxIterCV), HoldOutSetsAttr(MaxIterCV);
  TFltIntPrV NIdPhiV;
  TCesnaUtil::GetNIdPhiV<PUNGraph>(G, NIdPhiV);
  if (! UseBIC) { //if edges are many enough, use CV
    //printf("generating hold out set\n");
    TIntV NIdV1, NIdV2;
    G->GetNIdV(NIdV1);
    G->GetNIdV(NIdV2);
    for (int IterCV = 0; IterCV < MaxIterCV; IterCV++) {
      // generate holdout sets
      TCesnaUtil::GenHoldOutPairs(G, HoldOutSets[IterCV], HOFrac, Rnd);
      GenHoldOutAttr(HOFrac, HoldOutSetsAttr[IterCV]);
    }
    //printf("hold out set generated\n");
  }

  TFltV HOLV(ComsV.Len());
  TIntFltPrV ComsLV;
  for (int c = 0; c < ComsV.Len(); c++) {
    const int Coms = ComsV[c];
    //printf("Try number of Coms:%d\n", Coms);

    if (! UseBIC) { //if edges are many enough, use CV
      for (int IterCV = 0; IterCV < MaxIterCV; IterCV++) {
        HOVIDSV = HoldOutSets[IterCV];
        HOKIDSV = HoldOutSetsAttr[IterCV];
        NeighborComInit(NIdPhiV, Coms);
        //printf("Initialized\n");

        if (NumThreads == 1) {
          //printf("MLE without parallelization begins\n");
          MLEGradAscent(0.01, 100 * G->GetNodes(), "", StepAlpha, StepBeta);
          //printf("likelihood: train:%f, attr:%f, hold:%f\n", Likelihood(), LikelihoodAttr(), LikelihoodHoldOut());
        } else {
          //printf("MLE with parallelization begins\n");
          MLEGradAscentParallel(0.01, 100, NumThreads, "", StepAlpha, StepBeta);
        }
        double HOL = LikelihoodHoldOut();
        HOL = HOL < 0? HOL: TFlt::Mn;
        HOLV[c] += HOL;
      }
    }
    else {
      HOVIDSV.Gen(G->GetNodes());
      HOKIDSV.Gen(G->GetNodes());
      if (NumThreads == 1) {
        MLEGradAscent(0.005, 100 * G->GetNodes(), "", StepAlpha, StepBeta);
        printf("likelihood: train:%f, attr:%f, hold:%f\n", Likelihood(), LikelihoodAttr(), LikelihoodHoldOut());
      } else {
        MLEGradAscentParallel(0.005, 100, NumThreads, "", StepAlpha, StepBeta);
      }
      //int NumParams = G->GetNodes() * Coms + Coms * Attrs;
      double NumParams = (1.0 - WeightAttr) * Coms + WeightAttr * Coms * Attrs;
      double Observations = (1.0 - WeightAttr) * G->GetNodes() * (G->GetNodes() - 1) / 2 + WeightAttr * G->GetNodes() * Attrs;
      double BIC = 2 * Likelihood() - NumParams * log (Observations);
      HOLV[c] = BIC;
    }
  }
  int EstComs = 2;
  double MaxL = TFlt::Mn;
  if (UseBIC) {
    printf("Number of communities vs likelihood (criterion: BIC)\n");
  } else {
    printf("Number of communities vs likelihood (criterion: Cross validation)\n");
  }
  for (int c = 0; c < ComsV.Len(); c++) {
    ComsLV.Add(TIntFltPr(ComsV[c].Val, HOLV[c].Val));
    printf("%d(%f)\t", ComsV[c].Val, HOLV[c].Val);
    if (MaxL < HOLV[c]) {
      MaxL = HOLV[c];
      EstComs = ComsV[c];
    }
  }
  printf("\n");
  RandomInit(EstComs);
  HOVIDSV.Gen(G->GetNodes());
  HOKIDSV.Gen(G->GetNodes());
  if (! PlotLFNm.Empty()) {
    TGnuPlot::PlotValV(ComsLV, PlotLFNm, "hold-out likelihood", "communities", "likelihood");
  }
  return EstComs;
}

double TCesna::LikelihoodHoldOut() { 
  double L = 0.0;
  for (int u = 0; u < HOVIDSV.Len(); u++) {
    for (int e = 0; e < HOVIDSV[u].Len(); e++) {
      int VID = HOVIDSV[u][e];
      if (VID == u) { continue; } 
      double Pred = Prediction(u, VID);
      if (G->IsEdge(u, VID)) {
        L += log(1.0 - Pred);
      }
      else {
        L += NegWgt * log(Pred);
      }
      //printf("NODE %d, %d: Dot Prod: %f, Prediction: %f L:%f\n", u, VID, DotProduct(F[u], F[VID]), Pred, L);
    } 
  }
  L *= (1.0 - WeightAttr);
  //printf("likelihood for hold out network:%f\n", L);
  for (int u = 0; u < HOKIDSV.Len(); u++) {
    for (int e = 0; e < HOKIDSV[u].Len(); e++) {
      IAssert(HOKIDSV[u][e] < Attrs);
      L += WeightAttr * LikelihoodAttrKForRow(u, HOKIDSV[u][e]);
      //printf("asbefsafd ATTRIBUTE %d, NODE %d:%f\n", u, HOKIDSV[u][e].Val, LikelihoodAttrKForRow(u, HOKIDSV[u][e]));
    }
  }
  return L;
}

double TCesna::GetStepSizeByLineSearch(const int UID, const TIntFltH& DeltaV, const TIntFltH& GradV, const double& Alpha, const double& Beta, const int MaxIter) {
  double StepSize = 1.0;
  double InitLikelihood = LikelihoodForRow(UID);
  TIntFltH NewVarV(DeltaV.Len());
  for(int iter = 0; iter < MaxIter; iter++) {
    for (int i = 0; i < DeltaV.Len(); i++){
      int CID = DeltaV.GetKey(i);
      double NewVal = GetCom(UID, CID) + StepSize * DeltaV.GetDat(CID);
      if (NewVal < MinVal) { NewVal = MinVal; }
      if (NewVal > MaxVal) { NewVal = MaxVal; }
      NewVarV.AddDat(CID, NewVal);
    }
    if (LikelihoodForRow(UID, NewVarV) < InitLikelihood + Alpha * StepSize * DotProduct(GradV, DeltaV)) {
      StepSize *= Beta;
    } else {
      break;
    }
    if (iter == MaxIter - 1) { 
      StepSize = 0.0;
      break;
    }
  }
  return StepSize;
}

int TCesna::MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm, const double StepAlpha, const double StepBeta) {
  time_t InitTime = time(NULL);
  TExeTm ExeTm, CheckTm;
  int iter = 0, PrevIter = 0;
  TIntFltPrV IterLV;
  TUNGraph::TNodeI UI;
  double PrevL = TFlt::Mn, CurL = 0.0;
  TIntV NIdxV(F.Len(), 0);
  for (int i = 0; i < F.Len(); i++) { NIdxV.Add(i); }
  TIntFltH GradV;
  //consider all C
  TIntSet CIDSet(NumComs);
  for (int c = 0; c < NumComs; c++) { CIDSet.AddKey(c); }
  while(iter < MaxIter) {
    NIdxV.Shuffle(Rnd);
    for (int ui = 0; ui < F.Len(); ui++, iter++) {
      int u = NIdxV[ui]; //
      /*
      //find set of candidate c (we only need to consider c to which a neighbor of u belongs to)
      UI = G->GetNI(u);
      TIntSet CIDSet(20 * UI.GetDeg());
      for (int e = 0; e < UI.GetDeg(); e++) {
        if (HOVIDSV[u].IsKey(UI.GetNbrNId(e))) { continue; }
        TIntFltH& NbhCIDH = F[UI.GetNbrNId(e)];
        for (TIntFltH::TIter CI = NbhCIDH.BegI(); CI < NbhCIDH.EndI(); CI++) {
          CIDSet.AddKey(CI.GetKey());
        }
      }
      for (TIntFltH::TIter CI = F[u].BegI(); CI < F[u].EndI(); CI++) { //remove the community membership which U does not share with its neighbors
        if (! CIDSet.IsKey(CI.GetKey())) {
          DelCom(u, CI.GetKey());
        }
      }
      if (CIDSet.Empty()) { continue; }
      */
      
      GradientForRow(u, GradV, CIDSet);
      if (Norm2(GradV) < 1e-4) { continue; }
      double LearnRate = GetStepSizeByLineSearch(u, GradV, GradV, StepAlpha, StepBeta);
      if (LearnRate == 0.0) { continue; }
      for (int ci = 0; ci < GradV.Len(); ci++) {
        int CID = GradV.GetKey(ci);
        double Change = LearnRate * GradV.GetDat(CID);
        double NewFuc = GetCom(u, CID) + Change;
        if (NewFuc <= 0.0) {
          DelCom(u, CID);
        } else {
          AddCom(u, CID, NewFuc);
        }
      }
      if (! PlotNm.Empty() && (iter + 1) % G->GetNodes() == 0) {
        IterLV.Add(TIntFltPr(iter, Likelihood(false)));
      }
    }
    // fit W (logistic regression)
    for (int k = 0; k < Attrs; k++) {
      TFltV GradWV(NumComs);
      GradientForWK(GradWV, k);
      if (TLinAlg::Norm2(GradWV) < 1e-4) { continue; }
      double LearnRate = GetStepSizeByLineSearchForWK(k, GradWV, GradWV, StepAlpha, StepBeta);
      if (LearnRate == 0.0) { continue; }
      for (int c = 0; c < GradWV.Len(); c++){
        W[k][c] += LearnRate * GradWV[c];
        if (W[k][c] < MinValW) { W[k][c] = MinValW; }
        if (W[k][c] > MaxValW) { W[k][c] = MaxValW; }
      }
    }
    printf("\r%d iterations (%f) [%lu sec]", iter, CurL, time(NULL) - InitTime);
    fflush(stdout);
    if (iter - PrevIter >= 2 * G->GetNodes() && iter > 10000) {
      PrevIter = iter;
      CurL = Likelihood();
      if (PrevL > TFlt::Mn && ! PlotNm.Empty()) {
        printf("\r%d iterations, Likelihood: %f, Diff: %f", iter, CurL,  CurL - PrevL);
      }
      fflush(stdout);
      if (CurL - PrevL <= Thres * fabs(PrevL)) { break; }
      else { PrevL = CurL; }
    }
    
  }
  printf("\n");
  printf("MLE for Lambda completed with %d iterations(%s)\n", iter, ExeTm.GetTmStr());
  if (! PlotNm.Empty()) {
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
  }
  return iter;
}

int TCesna::MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const int ChunkSize, const TStr PlotNm, const double StepAlpha, const double StepBeta) {
  //parallel
  time_t InitTime = time(NULL);
  uint64 StartTm = TSecTm::GetCurTm().GetAbsSecs();
  TExeTm ExeTm, CheckTm;
  double PrevL = Likelihood(true);
  TIntFltPrV IterLV;
  int PrevIter = 0;
  int iter = 0;
  TIntV NIdxV(F.Len(), 0);
  for (int i = 0; i < F.Len(); i++) { NIdxV.Add(i); }
  TIntV NIDOPTV(F.Len()); //check if a node needs optimization or not 1: does not require optimization
  NIDOPTV.PutAll(0);
  TVec<TIntFltH> NewF(ChunkNum * ChunkSize);
  TIntV NewNIDV(ChunkNum * ChunkSize);
  for (iter = 0; iter < MaxIter; iter++) {
    NIdxV.Clr(false);
    for (int i = 0; i < F.Len(); i++) { 
      if (NIDOPTV[i] == 0) {  NIdxV.Add(i); }
    }
    IAssert (NIdxV.Len() <= F.Len());
    NIdxV.Shuffle(Rnd);
    // compute gradient for chunk of nodes
#pragma omp parallel for schedule(static, 1)
    for (int TIdx = 0; TIdx < ChunkNum; TIdx++) {
      TIntFltH GradV;
      for (int ui = TIdx * ChunkSize; ui < (TIdx + 1) * ChunkSize; ui++) {
        NewNIDV[ui] = -1;
        if (ui >= NIdxV.Len()) { continue; }
        int u = NIdxV[ui]; //
        //find set of candidate c (we only need to consider c to which a neighbor of u belongs to)
        TUNGraph::TNodeI UI = G->GetNI(u);
        TIntSet CIDSet(5 * UI.GetDeg());
        TIntFltH CurFU = F[u];
        for (int e = 0; e < UI.GetDeg(); e++) {
          if (HOVIDSV[u].IsKey(UI.GetNbrNId(e))) { continue; }
          TIntFltH& NbhCIDH = F[UI.GetNbrNId(e)];
          for (TIntFltH::TIter CI = NbhCIDH.BegI(); CI < NbhCIDH.EndI(); CI++) {
            CIDSet.AddKey(CI.GetKey());
          }
        }
        if (CIDSet.Empty()) { 
          CurFU.Clr();
        }
        else {
          for (TIntFltH::TIter CI = CurFU.BegI(); CI < CurFU.EndI(); CI++) { //remove the community membership which U does not share with its neighbors
            if (! CIDSet.IsKey(CI.GetKey())) {
              CurFU.DelIfKey(CI.GetKey());
            }
          }
          GradientForRow(u, GradV, CIDSet);
          if (Norm2(GradV) < 1e-4) { NIDOPTV[u] = 1; continue; }
          double LearnRate = GetStepSizeByLineSearch(u, GradV, GradV, StepAlpha, StepBeta);
          if (LearnRate == 0.0) { NewNIDV[ui] = -2; continue; }
          for (int ci = 0; ci < GradV.Len(); ci++) {
            int CID = GradV.GetKey(ci);
            double Change = LearnRate * GradV.GetDat(CID);
            double NewFuc = CurFU.IsKey(CID)? CurFU.GetDat(CID) + Change : Change;
            if (NewFuc <= 0.0) {
              CurFU.DelIfKey(CID);
            } else {
              CurFU.AddDat(CID) = NewFuc;
            }
          }
          CurFU.Defrag();
        }
        //store changes
        NewF[ui] = CurFU;
        NewNIDV[ui] = u;
      }
    }
    int NumNoChangeGrad = 0;
    int NumNoChangeStepSize = 0;
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID == -1) { NumNoChangeGrad++; continue; }
      if (NewNID == -2) { NumNoChangeStepSize++; continue; }
      for (TIntFltH::TIter CI = F[NewNID].BegI(); CI < F[NewNID].EndI(); CI++) {
        SumFV[CI.GetKey()] -= CI.GetDat();
      }
    }
#pragma omp parallel for
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID < 0) { continue; }
      F[NewNID] = NewF[ui];
    }
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID < 0) { continue; }
      for (TIntFltH::TIter CI = F[NewNID].BegI(); CI < F[NewNID].EndI(); CI++) {
        SumFV[CI.GetKey()] += CI.GetDat();
      }
    }
    // update the nodes who are optimal
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID < 0) { continue; }
      TUNGraph::TNodeI UI = G->GetNI(NewNID);
      NIDOPTV[NewNID] = 0;
      for (int e = 0; e < UI.GetDeg(); e++) {
        NIDOPTV[UI.GetNbrNId(e)] = 0;
      }
    }
    int OPTCnt = 0;
    for (int i = 0; i < NIDOPTV.Len(); i++) { if (NIDOPTV[i] == 1) { OPTCnt++; } }
    // fit W (logistic regression)
    if (! PlotNm.Empty()) {
      printf("\r%d iterations [%s] %s secs", iter * ChunkSize * ChunkNum, ExeTm.GetTmStr(), TUInt64::GetStr(TSecTm::GetCurTm().GetAbsSecs()-StartTm).CStr());
      if (PrevL > TFlt::Mn) { printf(" (%f) %d g %d s %d OPT", PrevL, NumNoChangeGrad, NumNoChangeStepSize, OPTCnt); }
      fflush(stdout);
    }
    if (iter == 0 || (iter - PrevIter) * ChunkSize * ChunkNum >= G->GetNodes()) {
  #pragma omp parallel for
      for (int k = 0; k < Attrs; k++) {
        TFltV GradWV(NumComs);
        GradientForWK(GradWV, k);
        if (TLinAlg::Norm2(GradWV) < 1e-4) { continue; }
        double LearnRate = GetStepSizeByLineSearchForWK(k, GradWV, GradWV, StepAlpha, StepBeta);
        if (LearnRate == 0.0) { continue; }
        for (int c = 0; c < GradWV.Len(); c++){
          W[k][c] += LearnRate * GradWV[c];
          if (W[k][c] < MinValW) { W[k][c] = MinValW; }
          if (W[k][c] > MaxValW) { W[k][c] = MaxValW; }
        }
      }
      PrevIter = iter;
      double CurL = Likelihood(true);
      IterLV.Add(TIntFltPr(iter * ChunkSize * ChunkNum, CurL));
      printf("\r%d iterations, Likelihood: %f, Diff: %f [%lu secs]", iter, CurL,  CurL - PrevL, time(NULL) - InitTime);
       fflush(stdout);
      if (CurL - PrevL <= Thres * fabs(PrevL)) { 
        break;
      }
      else {
        PrevL = CurL;
      }
    }
  }
  if (! PlotNm.Empty()) {
    printf("\nMLE completed with %d iterations(%s secs)\n", iter, TUInt64::GetStr(TSecTm::GetCurTm().GetAbsSecs()-StartTm).CStr());
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
  } else {
    printf("\rMLE completed with %d iterations(%lu secs)", iter, time(NULL) - InitTime);
    fflush(stdout);
  }
  return iter;
}
