
#include "stdafx.h"
#include "agmfast.h"
#include "Snap.h"
#include "agm.h"

void TAGMFast::Save(TSOut& SOut) {
  G->Save(SOut);
  F.Save(SOut);
  NIDV.Save(SOut);
  RegCoef.Save(SOut);
  SumFV.Save(SOut);
  NodesOk.Save(SOut);
  MinVal.Save(SOut);
  MaxVal.Save(SOut);
  NegWgt.Save(SOut);
  NumComs.Save(SOut);
  HOVIDSV.Save(SOut);
  PNoCom.Save(SOut);
}

void TAGMFast::Load(TSIn& SIn, const int& RndSeed) {
  G->Load(SIn);
  F.Load(SIn);
  NIDV.Load(SIn);
  RegCoef.Load(SIn);
  SumFV.Load(SIn);
  NodesOk.Load(SIn);
  MinVal.Load(SIn);
  MaxVal.Load(SIn);
  NegWgt.Load(SIn);
  NumComs.Load(SIn);
  HOVIDSV.Load(SIn);
  PNoCom.Load(SIn);
  Rnd.PutSeed(RndSeed);
}

void TAGMFast::RandomInit(const int InitComs) {
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
}

void TAGMFast::NeighborComInit(const int InitComs) {
  //initialize with best neighborhood communities (Gleich et.al. KDD'12)
  F.Gen(G->GetNodes());
  SumFV.Gen(InitComs);
  NumComs = InitComs;
  const int Edges = G->GetEdges();
  //TIntFltH NCPhiH(F.Len());
  TFltIntPrV NIdPhiV(F.Len(), 0);
  TIntSet InvalidNIDS(F.Len());
  TIntV ChosenNIDV(InitComs, 0); //FOR DEBUG
  TExeTm RunTm;
  //compute conductance of neighborhood community
  for (int u = 0; u < F.Len(); u++) {
    TIntSet NBCmty(G->GetNI(u).GetDeg() + 1);
    double Phi;
    if (G->GetNI(u).GetDeg() < 5) { //do not include nodes with too few degree
      Phi = 1.0; 
    } else {
      TAGMUtil::GetNbhCom(G, u, NBCmty);
      IAssert(NBCmty.Len() == G->GetNI(u).GetDeg() + 1);
      Phi = TAGMUtil::GetConductance(G, NBCmty, Edges);
    }
    //NCPhiH.AddDat(u, Phi);
    NIdPhiV.Add(TFltIntPr(Phi, u));
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
}

void TAGMFast::SetCmtyVV(const TVec<TIntV>& CmtyVV) {
  F.Gen(G->GetNodes());
  SumFV.Gen(CmtyVV.Len());
  NumComs = CmtyVV.Len();
  TIntH NIDIdxH(NIDV.Len());
  if (! NodesOk) {
    for (int u = 0; u < NIDV.Len(); u++) {
      NIDIdxH.AddDat(NIDV[u], u);
    }
  }
  for (int c = 0; c < CmtyVV.Len(); c++) {
    for (int u = 0; u < CmtyVV[c].Len(); u++) {
      int UID = CmtyVV[c][u];
      if (! NodesOk) { UID = NIDIdxH.GetDat(UID); }
      if (G->IsNode(UID)) { 
        AddCom(UID, c, 1.0);
      }
    }
  }
}

void TAGMFast::SetGraph(const PUNGraph& GraphPt) {
  G = GraphPt;
  HOVIDSV.Gen(G->GetNodes());  
  NodesOk = true;
  GraphPt->GetNIdV(NIDV);
  // check that nodes IDs are {0,1,..,Nodes-1}
  for (int nid = 0; nid < GraphPt->GetNodes(); nid++) {
    if (! GraphPt->IsNode(nid)) { 
      NodesOk = false; 
      break; 
    } 
  }
  if (! NodesOk) {
    printf("rearrage nodes\n");
    G = TSnap::GetSubGraph(GraphPt, NIDV, true);
    for (int nid = 0; nid < G->GetNodes(); nid++) {
      IAssert(G->IsNode(nid)); 
    }
  }
  TSnap::DelSelfEdges(G);
  
  PNoCom = 1.0 / (double) G->GetNodes();
  DoParallel = false;
  if (1.0 / PNoCom > sqrt(TFlt::Mx)) { PNoCom = 0.99 / sqrt(TFlt::Mx); } // to prevent overflow
  NegWgt = 1.0;
}

double TAGMFast::Likelihood(const bool _DoParallel) { 
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

double TAGMFast::LikelihoodForRow(const int UID) {
  return LikelihoodForRow(UID, F[UID]);
}


double TAGMFast::LikelihoodForRow(const int UID, const TIntFltH& FU) {
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
  if (DoParallel && NI.GetDeg() > 10) {
#pragma omp parallel for schedule(static, 1)
    for (int e = 0; e < NI.GetDeg(); e++) {
      int v = NI.GetNbrNId(e);
      if (v == UID) { continue; }
      if (HOVIDSV[UID].IsKey(v)) { continue; }
      double LU = log (1.0 - Prediction(FU, F[v])) + NegWgt * DotProduct(FU, F[v]);
#pragma omp atomic
      L += LU;
    }
    for (TIntFltH::TIter HI = FU.BegI(); HI < FU.EndI(); HI++) {
      double HOSum = HOVIDSV[UID].Len() > 0?  HOSumFV[HI.GetKey()].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
      double LU = NegWgt * (SumFV[HI.GetKey()] - HOSum - GetCom(UID, HI.GetKey())) * HI.GetDat();
      L -= LU;
    }
  } else {
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
  }
  //add regularization
  if (RegCoef > 0.0) { //L1
    L -= RegCoef * Sum(FU);
  }
  if (RegCoef < 0.0) { //L2
    L += RegCoef * Norm2(FU);
  }

  return L;
}

void TAGMFast::GradientForRow(const int UID, TIntFltH& GradU, const TIntSet& CIDSet) {
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
  if (DoParallel && Deg + CIDSet.Len() > 10) {
#pragma omp parallel for schedule(static, 1)
    for (int e = 0; e < Deg; e++) {
      if (NI.GetNbrNId(e) == UID) { continue; }
      if (HOVIDSV[UID].IsKey(NI.GetNbrNId(e))) { continue; }
      PredV[e] = Prediction(UID, NI.GetNbrNId(e));
    }
  
#pragma omp parallel for schedule(static, 1)
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
  } 
  else {
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

double TAGMFast::GradientForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val) {
  TUNGraph::TNodeI UI = G->GetNI(UID);
  double Grad = 0.0, PNoEdge;
  int VID = 0;
  for (int e = 0; e < UI.GetDeg(); e++) {
    VID = UI.GetNbrNId(e);
    if (HOVIDSV[UID].IsKey(UI.GetNbrNId(e))) { continue; }
    if (! F[VID].IsKey(CID)) { continue; }
    PNoEdge = AlphaKV[e] * exp (- F[VID].GetDat(CID) * Val);
    IAssert(PNoEdge <= 1.0 && PNoEdge >= 0.0);
    //PNoEdge = PNoEdge >= 1.0 - PNoCom? 1 - PNoCom: PNoEdge;
    Grad += ((PNoEdge * F[VID].GetDat(CID)) / (1.0 - PNoEdge) + NegWgt * F[VID].GetDat(CID));
  }
  Grad -= NegWgt * (SumFV[CID] - GetCom(UID, CID));
  //add regularization
  if (RegCoef > 0.0) { //L1
    Grad -= RegCoef; 
  }
  if (RegCoef < 0.0) { //L2
    Grad += 2 * RegCoef * Val; 
  }

  return Grad;
}

double TAGMFast::LikelihoodForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val) {
  TUNGraph::TNodeI UI = G->GetNI(UID);
  double L = 0.0, PNoEdge;
  int VID = 0;
  for (int e = 0; e < UI.GetDeg(); e++) {
    VID = UI.GetNbrNId(e);
    if (HOVIDSV[UID].IsKey(UI.GetNbrNId(e))) { continue; }
    if (! F[VID].IsKey(CID)) { 
      PNoEdge = AlphaKV[e];
    } else {
      PNoEdge = AlphaKV[e] * exp (- F[VID].GetDat(CID) * Val);
    }
    IAssert(PNoEdge <= 1.0 && PNoEdge >= 0.0);
    //PNoEdge = PNoEdge >= 1.0 - PNoCom? 1 - PNoCom: PNoEdge;
    L += log(1.0 - PNoEdge) + NegWgt * GetCom(VID, CID) * Val;

    //  += ((PNoEdge * F[VID].GetDat(CID)) / (1.0 - PNoEdge) + NegWgt * F[VID].GetDat(CID));
  }
  L -= NegWgt * (SumFV[CID] - GetCom(UID, CID)) * Val;
  //add regularization
  if (RegCoef > 0.0) { //L1
    L -= RegCoef * Val;
  }
  if (RegCoef < 0.0) { //L2
    L += RegCoef * Val * Val; 
  }

  return L;
}

double TAGMFast::HessianForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val) {
  TUNGraph::TNodeI UI = G->GetNI(UID);
  double H = 0.0, PNoEdge;
  int VID = 0;
  for (int e = 0; e < UI.GetDeg(); e++) {
    VID = UI.GetNbrNId(e);
    if (HOVIDSV[UID].IsKey(UI.GetNbrNId(e))) { continue; }
    if (! F[VID].IsKey(CID)) { continue; }
    PNoEdge = AlphaKV[e] * exp (- F[VID].GetDat(CID) * Val);
    IAssert(PNoEdge <= 1.0 && PNoEdge >= 0.0);
    //PNoEdge = PNoEdge == 1.0? 1 - PNoCom: PNoEdge;
    H += (- PNoEdge * F[VID].GetDat(CID) * F[VID].GetDat(CID)) / (1.0 - PNoEdge) / (1.0 - PNoEdge);
  }
  //add regularization
  if (RegCoef < 0.0) { //L2
    H += 2 * RegCoef; 
  }
  IAssert (H <= 0.0);
  return H;
}

/// Newton method: DEPRECATED
int TAGMFast::MLENewton(const double& Thres, const int& MaxIter, const TStr PlotNm) {
  TExeTm ExeTm;
  int iter = 0, PrevIter = 0;
  TIntFltPrV IterLV;
  double PrevL = TFlt::Mn, CurL;
  TUNGraph::TNodeI UI;
  TIntV NIdxV;
  G->GetNIdV(NIdxV);
  int CID, UID, NewtonIter;
  double Fuc, PrevFuc, Grad, H;
  while(iter < MaxIter) {
    NIdxV.Shuffle(Rnd);
    for (int ui = 0; ui < F.Len(); ui++, iter++) {
      if (! PlotNm.Empty() && iter % G->GetNodes() == 0) {
        IterLV.Add(TIntFltPr(iter, Likelihood(false)));
      }
      UID = NIdxV[ui];
      //find set of candidate c (we only need to consider c to which a neighbor of u belongs to)
      TIntSet CIDSet;
      UI = G->GetNI(UID);
      if (UI.GetDeg() == 0) { //if the node is isolated, clear its membership and skip
        if (! F[UID].Empty()) { F[UID].Clr(); }
        continue;
      }
      for (int e = 0; e < UI.GetDeg(); e++) {
        if (HOVIDSV[UID].IsKey(UI.GetNbrNId(e))) { continue; }
        TIntFltH& NbhCIDH = F[UI.GetNbrNId(e)];
        for (TIntFltH::TIter CI = NbhCIDH.BegI(); CI < NbhCIDH.EndI(); CI++) {
          CIDSet.AddKey(CI.GetKey());
        }
      }
      for (TIntFltH::TIter CI = F[UID].BegI(); CI < F[UID].EndI(); CI++) { //remove the community membership which U does not share with its neighbors
        if (! CIDSet.IsKey(CI.GetKey())) {
          DelCom(UID, CI.GetKey());
        }
      }
      if (CIDSet.Empty()) { continue; }
      for (TIntSet::TIter CI = CIDSet.BegI(); CI < CIDSet.EndI(); CI++) {
        CID = CI.GetKey();
        //optimize for UID, CID
        //compute constants
        TFltV AlphaKV(UI.GetDeg());
        for (int e = 0; e < UI.GetDeg(); e++) {
          if (HOVIDSV[UID].IsKey(UI.GetNbrNId(e))) { continue; }
          AlphaKV[e] = (1 - PNoCom) * exp(- DotProduct(UID, UI.GetNbrNId(e)) + GetCom(UI.GetNbrNId(e), CID) * GetCom(UID, CID));
          IAssertR(AlphaKV[e] <= 1.0, TStr::Fmt("AlphaKV=%f, %f, %f", AlphaKV[e].Val, PNoCom.Val, GetCom(UI.GetNbrNId(e), CID)));
        }
        Fuc = GetCom(UID, CID);
        PrevFuc = Fuc;
        Grad = GradientForOneVar(AlphaKV, UID, CID, Fuc), H = 0.0;
        if (Grad <= 1e-3 && Grad >= -0.1) { continue; }
        NewtonIter = 0;
        while (NewtonIter++ < 10) {
          Grad = GradientForOneVar(AlphaKV, UID, CID, Fuc), H = 0.0;
          H = HessianForOneVar(AlphaKV, UID, CID, Fuc);
          if (Fuc == 0.0 && Grad <= 0.0) { Grad = 0.0; }
          if (fabs(Grad) < 1e-3) { break; }
          if (H == 0.0) { Fuc = 0.0; break; }
          double NewtonStep = - Grad / H;
          if (NewtonStep < -0.5) { NewtonStep = - 0.5; }
          Fuc += NewtonStep;
          if (Fuc < 0.0) { Fuc = 0.0; }
        }
        if (Fuc == 0.0) {
          DelCom(UID, CID);
        }
        else {
          AddCom(UID, CID, Fuc);
        }
      }
    }
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
  if (! PlotNm.Empty()) {
    printf("\nMLE for Lambda completed with %d iterations(%s)\n", iter, ExeTm.GetTmStr());
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
  }
  return iter;
}

void TAGMFast::GetCmtyVV(TVec<TIntV>& CmtyVV) {
  GetCmtyVV(CmtyVV, sqrt(2.0 * (double) G->GetEdges() / G->GetNodes() / G->GetNodes()), 3);
}

/// extract community affiliation from F_uc
void TAGMFast::GetCmtyVV(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
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
      int NID = u;
      if (! NodesOk) { NID = NIDV[u]; }
      if (GetCom(u, CID) >= Thres) { NIDFucH.AddDat(NID, GetCom(u, CID)); }
    }
    NIDFucH.SortByDat(false);
    NIDFucH.GetKeyV(CmtyV);
    if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}

/// estimate number of communities using cross validation
int TAGMFast::FindComsByCV(const int NumThreads, const int MaxComs, const int MinComs, const int DivComs, const TStr OutFNm, const double StepAlpha, const double StepBeta) {
    double ComsGap = exp(TMath::Log((double) MaxComs / (double) MinComs) / (double) DivComs);
    TIntV ComsV;
    ComsV.Add(MinComs);
    while (ComsV.Len() < DivComs) {
      int NewComs = int(ComsV.Last() * ComsGap);
      if (NewComs == ComsV.Last().Val) { NewComs++; }
      ComsV.Add(NewComs);
    }
    if (ComsV.Last() < MaxComs) { ComsV.Add(MaxComs); }
    return FindComsByCV(ComsV, 0.1, NumThreads, OutFNm + ".CV.likelihood", StepAlpha, StepBeta);
}

int TAGMFast::FindComsByCV(TIntV& ComsV, const double HOFrac, const int NumThreads, const TStr PlotLFNm, const double StepAlpha, const double StepBeta) {
  if (ComsV.Len() == 0) {
    int MaxComs = G->GetNodes() / 5;
    ComsV.Add(2);
    while(ComsV.Last() < MaxComs) { ComsV.Add(ComsV.Last() * 2); }
  }
  TIntPrV EdgeV(G->GetEdges(), 0);
  for (TUNGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
    EdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
  }
  EdgeV.Shuffle(Rnd);
  int MaxIterCV = 3;

  TVec<TVec<TIntSet> > HoldOutSets(MaxIterCV);
  if (EdgeV.Len() > 50) { //if edges are many enough, use CV
    printf("generating hold out set\n");
    TIntV NIdV1, NIdV2;
    G->GetNIdV(NIdV1);
    G->GetNIdV(NIdV2);
    for (int IterCV = 0; IterCV < MaxIterCV; IterCV++) {
      // generate holdout sets
      HoldOutSets[IterCV].Gen(G->GetNodes());
      const int HOTotal = int(HOFrac * G->GetNodes() * (G->GetNodes() - 1) / 2.0);
      int HOCnt = 0;
      int HOEdges = (int) TMath::Round(HOFrac * G->GetEdges());
      printf("holding out %d edges...\n", HOEdges);
      for (int he = 0; he < (int) HOEdges; he++) {
        HoldOutSets[IterCV][EdgeV[he].Val1].AddKey(EdgeV[he].Val2);
        HoldOutSets[IterCV][EdgeV[he].Val2].AddKey(EdgeV[he].Val1);
        HOCnt++;
      }
      printf("%d Edges hold out\n", HOCnt);
      while(HOCnt++ < HOTotal) {
        int SrcNID = Rnd.GetUniDevInt(G->GetNodes());
        int DstNID = Rnd.GetUniDevInt(G->GetNodes());
        HoldOutSets[IterCV][SrcNID].AddKey(DstNID);
        HoldOutSets[IterCV][DstNID].AddKey(SrcNID);
      }
    }
    printf("hold out set generated\n");
  }

  TFltV HOLV(ComsV.Len());
  TIntFltPrV ComsLV;
  for (int c = 0; c < ComsV.Len(); c++) {
    const int Coms = ComsV[c];
    printf("Try number of Coms:%d\n", Coms);
    NeighborComInit(Coms);
    printf("Initialized\n");

    if (EdgeV.Len() > 50) { //if edges are many enough, use CV
      for (int IterCV = 0; IterCV < MaxIterCV; IterCV++) {
        HOVIDSV = HoldOutSets[IterCV];

        if (NumThreads == 1) {
          printf("MLE without parallelization begins\n");
          MLEGradAscent(0.05, 10 * G->GetNodes(), "", StepAlpha, StepBeta);
        } else {
          printf("MLE with parallelization begins\n");
          MLEGradAscentParallel(0.05, 100, NumThreads, "", StepAlpha, StepBeta);
        }
        double HOL = LikelihoodHoldOut();
        HOL = HOL < 0? HOL: TFlt::Mn;
        HOLV[c] += HOL;
      }
    }
    else {
      HOVIDSV.Gen(G->GetNodes());
      MLEGradAscent(0.0001, 100 * G->GetNodes(), "");
      double BIC = 2 * Likelihood() - (double) G->GetNodes() * Coms * 2.0 * log ( (double) G->GetNodes());
      HOLV[c] = BIC;
    }
  }
  int EstComs = 2;
  double MaxL = TFlt::Mn;
  printf("\n");
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
  if (! PlotLFNm.Empty()) {
    TGnuPlot::PlotValV(ComsLV, PlotLFNm, "hold-out likelihood", "communities", "likelihood");
  }
  return EstComs;
}

double TAGMFast::LikelihoodHoldOut(const bool DoParallel) { 
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
    }
  }
  return L;
}

double TAGMFast::GetStepSizeByLineSearch(const int UID, const TIntFltH& DeltaV, const TIntFltH& GradV, const double& Alpha, const double& Beta, const int MaxIter) {
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

int TAGMFast::MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm, const double StepAlpha, const double StepBeta) {
  time_t InitTime = time(NULL);
  TExeTm ExeTm, CheckTm;
  int iter = 0, PrevIter = 0;
  TIntFltPrV IterLV;
  TUNGraph::TNodeI UI;
  double PrevL = TFlt::Mn, CurL = 0.0;
  TIntV NIdxV(F.Len(), 0);
  for (int i = 0; i < F.Len(); i++) { NIdxV.Add(i); }
  IAssert(NIdxV.Len() == F.Len());
  TIntFltH GradV;
  while(iter < MaxIter) {
    NIdxV.Shuffle(Rnd);
    for (int ui = 0; ui < F.Len(); ui++, iter++) {
      int u = NIdxV[ui]; //
      //find set of candidate c (we only need to consider c to which a neighbor of u belongs to)
      UI = G->GetNI(u);
      TIntSet CIDSet(5 * UI.GetDeg());
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

int TAGMFast::MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const int ChunkSize, const TStr PlotNm, const double StepAlpha, const double StepBeta) {
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
        if (ui > NIdxV.Len()) { continue; }
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
          double LearnRate = GetStepSizeByLineSearch(u, GradV, GradV, StepAlpha, StepBeta, 5);
          if (LearnRate <= 1e-5) { NewNIDV[ui] = -2; continue; }
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
    if (! PlotNm.Empty()) {
      printf("\r%d iterations [%s] %d secs", iter * ChunkSize * ChunkNum, ExeTm.GetTmStr(), int(TSecTm::GetCurTm().GetAbsSecs() - StartTm));
      if (PrevL > TFlt::Mn) { printf(" (%f) %d g %d s %d OPT", PrevL, NumNoChangeGrad, NumNoChangeStepSize, OPTCnt); }
      fflush(stdout);
    }
    if ((iter - PrevIter) * ChunkSize * ChunkNum >= G->GetNodes()) {
      PrevIter = iter;
      double CurL = Likelihood(true);
      IterLV.Add(TIntFltPr(iter * ChunkSize * ChunkNum, CurL));
      printf("\r%d iterations, Likelihood: %f, Diff: %f [%d secs]", iter, CurL,  CurL - PrevL, int(time(NULL) - InitTime));
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
    printf("\nMLE completed with %d iterations(%d secs)\n", iter, int(TSecTm::GetCurTm().GetAbsSecs() - StartTm));
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
  } else {
    printf("\rMLE completed with %d iterations(%d secs)", iter, int(time(NULL) - InitTime));
    fflush(stdout);
  }
  return iter;
}
