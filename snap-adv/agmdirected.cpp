
#include "stdafx.h"
#include "agmfast.h"
#include "agmdirected.h"
#include "Snap.h"
#include "agm.h"

void TCoda::Save(TSOut& SOut) {
  G->Save(SOut);
  F.Save(SOut);
  H.Save(SOut);
  NIDV.Save(SOut);
  RegCoef.Save(SOut);
  SumFV.Save(SOut);
  SumHV.Save(SOut);
  NodesOk.Save(SOut);
  MinVal.Save(SOut);
  MaxVal.Save(SOut);
  NegWgt.Save(SOut);
  NumComs.Save(SOut);
  HOVIDSV.Save(SOut);
  PNoCom.Save(SOut);
}

void TCoda::Load(TSIn& SIn, const int& RndSeed) {
  G->Load(SIn);
  F.Load(SIn);
  H.Load(SIn);
  NIDV.Load(SIn);
  RegCoef.Load(SIn);
  SumFV.Load(SIn);
  SumHV.Load(SIn);
  NodesOk.Load(SIn);
  MinVal.Load(SIn);
  MaxVal.Load(SIn);
  NegWgt.Load(SIn);
  NumComs.Load(SIn);
  HOVIDSV.Load(SIn);
  PNoCom.Load(SIn);
  Rnd.PutSeed(RndSeed);
}

void TCoda::RandomInit(const int InitComs) {
  F.Gen(G->GetNodes());
  H.Gen(G->GetNodes());
  SumFV.Gen(InitComs);
  SumHV.Gen(InitComs);
  NumComs = InitComs;
  for (int u = 0; u < F.Len(); u++) {
    //assign to just one community
    int Mem = G->GetNI(u).GetOutDeg();
    if (Mem > 10) { Mem = 10; }
    for (int c = 0; c < Mem; c++) {
      int CID = Rnd.GetUniDevInt(InitComs);
      AddComOut(u, CID, Rnd.GetUniDev());
    }
  }
  for (int u = 0; u < H.Len(); u++) {
    //assign to just one community
    int Mem = G->GetNI(u).GetInDeg();
    if (Mem > 10) { Mem = 10; }
    for (int c = 0; c < Mem; c++) {
      int CID = Rnd.GetUniDevInt(InitComs);
      AddComIn(u, CID, Rnd.GetUniDev());
    }
  }
  //assign a member to zero-member community (if any)
  for (int c = 0; c < SumFV.Len(); c++) {
    if (SumFV[c] == 0.0) {
      int UID = Rnd.GetUniDevInt(G->GetNodes());
      AddComOut(UID, c, Rnd.GetUniDev());
    }
  }
  //assign a member to zero-member community (if any)
  for (int c = 0; c < SumHV.Len(); c++) {
    if (SumHV[c] == 0.0) {
      int UID = Rnd.GetUniDevInt(G->GetNodes());
      AddComIn(UID, c, Rnd.GetUniDev());
    }
  }
}

void TCoda::NeighborComInit(const int InitComs) {
  //initialize with best neighborhood communities (Gleich et.al. KDD'12)
  TExeTm RunTm;
  TFltIntPrV NIdPhiV(F.Len(), 0);
  TAGMFastUtil::GetNIdPhiV<PNGraph>(G, NIdPhiV);
  NeighborComInit(NIdPhiV, InitComs);
}

void TCoda::NeighborComInit(TFltIntPrV& NIdPhiV, const int InitComs) {
  NIdPhiV.Sort(true);
  F.Gen(G->GetNodes());
  H.Gen(G->GetNodes());
  SumFV.Gen(InitComs);
  SumHV.Gen(InitComs);
  NumComs = InitComs;
  //TIntFltH NCPhiH(F.Len());
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
    TNGraph::TNodeI NI = G->GetNI(UID);
    if (NI.GetOutDeg() > 0) { AddComOut(UID, CurCID, 1.0); }
    if (NI.GetInDeg() > 0) { AddComIn(UID, CurCID, 1.0); }
    fflush(stdout);
    //add neighbors depending on whether they have incoming / outgoing edges from the center node (NI)
    for (int e = 0; e < NI.GetDeg(); e++) {
      int VID = NI.GetNbrNId(e);
      TNGraph::TNodeI VI = G->GetNI(VID);
      if (VI.GetOutDeg() > 0) { AddComOut(VID, CurCID, 1.0); }
      if (VI.GetInDeg() > 0) { AddComIn(VID, CurCID, 1.0); }
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
        AddComOut(UID, c, Rnd.GetUniDev());
      }
    }
  }
  //assign a member to zero-member community (if any)
  for (int c = 0; c < SumHV.Len(); c++) {
    if (SumHV[c] == 0.0) {
      int ComSz = 10;
      for (int u = 0; u < ComSz; u++) {
        int UID = Rnd.GetUniDevInt(G->GetNodes());
        AddComIn(UID, c, Rnd.GetUniDev());
      }
    }
  }
}

void TCoda::GetNonEdgePairScores(TFltIntIntTrV& ScoreV) {
  ScoreV.Gen(G->GetNodes() * G->GetNodes(), 0);
  TIntV NIDV;
  G->GetNIdV(NIDV);
  TIntSet Cuv;
  for (int u = 0; u < NIDV.Len(); u++) {
    int UID = NIDV[u];
    for (int v = 0; v < NIDV.Len(); v++) {
      int VID = NIDV[v];
      if (UID == VID) { continue; }
      if (! G->IsEdge(UID, VID)) {
        double Val = 1.0 - Prediction(UID, VID);
        ScoreV.Add(TFltIntIntTr(Val, UID, VID));
      }
    }
  }
}

void TCoda::SetCmtyVV(const TVec<TIntV>& CmtyVVOut, const TVec<TIntV>& CmtyVVIn) {
  IAssert(CmtyVVOut.Len() == CmtyVVIn.Len());
  F.Gen(G->GetNodes());
  H.Gen(G->GetNodes());
  SumFV.Gen(CmtyVVOut.Len());
  SumHV.Gen(CmtyVVIn.Len());
  NumComs = CmtyVVOut.Len();
  TIntH NIDIdxH(NIDV.Len());
  if (! NodesOk) {
    for (int u = 0; u < NIDV.Len(); u++) {
      NIDIdxH.AddDat(NIDV[u], u);
    }
  }
  for (int c = 0; c < CmtyVVOut.Len(); c++) {
    for (int u = 0; u < CmtyVVOut[c].Len(); u++) {
      int UID = CmtyVVOut[c][u];
      if (! NodesOk) { UID = NIDIdxH.GetDat(UID); }
      if (G->IsNode(UID)) { 
        AddComOut(UID, c, 1.0);
      }
    }
  }
  for (int c = 0; c < CmtyVVIn.Len(); c++) {
    for (int u = 0; u < CmtyVVIn[c].Len(); u++) {
      int UID = CmtyVVIn[c][u];
      if (! NodesOk) { UID = NIDIdxH.GetDat(UID); }
      if (G->IsNode(UID)) { 
        AddComIn(UID, c, 1.0);
      }
    }
  }
}

void TCoda::SetGraph(const PNGraph& GraphPt) {
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

double TCoda::Likelihood(const bool _DoParallel) { 
  TExeTm ExeTm;
  double L = 0.0;
  if (_DoParallel) {
  #pragma omp parallel for 
    for (int u = 0; u < F.Len(); u++) {
      double LU = LikelihoodForNode(true, u);
      #pragma omp atomic
        L += LU;
    }
  }
  else {
    for (int u = 0; u < F.Len(); u++) {
      double LU = LikelihoodForNode(true, u);
        L += LU;
    }
  }
  return L;
}

double TCoda::LikelihoodForNode(const bool IsRow, const int UID) {
  if (IsRow) {
    return LikelihoodForNode(IsRow, UID, F[UID]);
  } else {
    return LikelihoodForNode(IsRow, UID, H[UID]);
  }
}

double TCoda::LikelihoodForNode(const bool IsRow, const int UID, const TIntFltH& FU) {
  double L = 0.0;
  TFltV HOSumHV; //adjust for Hv of v hold out
  if (HOVIDSV[UID].Len() > 0) {
    HOSumHV.Gen(NumComs);
    for (int e = 0; e < HOVIDSV[UID].Len(); e++) {
      for (int c = 0; c < SumHV.Len(); c++) {
        HOSumHV[c] += GetCom(! IsRow, HOVIDSV[UID][e], c);
      }
    }
  }
  TNGraph::TNodeI NI = G->GetNI(UID);
  const int Deg = IsRow ? NI.GetOutDeg(): NI.GetInDeg();
  for (int e = 0; e < Deg; e++) {
    const int v = IsRow ? NI.GetOutNId(e): NI.GetInNId(e);
    if (v == UID) { continue; }
    if (HOVIDSV[UID].IsKey(v)) { continue; }
    if (IsRow) {
      L += log (1.0 - Prediction(FU, H[v])) + NegWgt * DotProduct(FU, H[v]);
    } else {
      L += log (1.0 - Prediction(F[v], FU)) + NegWgt * DotProduct(F[v], FU);
    }
  }
  for (TIntFltH::TIter HI = FU.BegI(); HI < FU.EndI(); HI++) {
    double HOSum = HOVIDSV[UID].Len() > 0?  HOSumHV[HI.GetKey()].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    L -= NegWgt * (GetSumVal(! IsRow, HI.GetKey()) - HOSum - GetCom(! IsRow, UID, HI.GetKey())) * HI.GetDat();
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


/*
double TCoda::LikelihoodForRow(const int UID) {
  return LikelihoodForRow(UID, F[UID]);
}


double TCoda::LikelihoodForRow(const int UID, const TIntFltH& FU) {
  double L = 0.0;
  TFltV HOSumHV; //adjust for Hv of v hold out
  if (HOVIDSV[UID].Len() > 0) {
    HOSumHV.Gen(SumFV.Len());
    
    for (int e = 0; e < HOVIDSV[UID].Len(); e++) {
      for (int c = 0; c < SumHV.Len(); c++) {
        HOSumHV[c] += GetComIn(HOVIDSV[UID][e], c);
      }
    }
  }
  TNGraph::TNodeI NI = G->GetNI(UID);
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    int v = NI.GetOutNId(e);
    if (v == UID) { continue; }
    if (HOVIDSV[UID].IsKey(v)) { continue; }
    double LU = log (1.0 - Prediction(FU, H[v])) + NegWgt * DotProduct(FU, H[v]);
    L += LU;
  }
  for (TIntFltH::TIter HI = FU.BegI(); HI < FU.EndI(); HI++) {
    double HOSum = HOVIDSV[UID].Len() > 0?  HOSumHV[HI.GetKey()].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    double LU = NegWgt * (SumHV[HI.GetKey()] - HOSum - GetComIn(UID, HI.GetKey())) * HI.GetDat();
    L -= LU;
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

double TCoda::LikelihoodForCol(const int VID) {
  return LikelihoodForCol(VID, H[VID]);
}


double TCoda::LikelihoodForCol(const int VID, const TIntFltH& HV) {
  double L = 0.0;
  TFltV HOSumFV; //adjust for Fv of v hold out
  if (HOVIDSV[VID].Len() > 0) {
    HOSumFV.Gen(SumFV.Len());
    for (int e = 0; e < HOVIDSV[VID].Len(); e++) {
      for (int c = 0; c < SumFV.Len(); c++) {
        HOSumFV[c] += GetComOut(HOVIDSV[VID][e], c);
      }
    }
  }
  TNGraph::TNodeI NI = G->GetNI(VID);
  for (int e = 0; e < NI.GetInDeg(); e++) {
    int v = NI.GetInNId(e);
    if (v == VID) { continue; }
    if (HOVIDSV[VID].IsKey(v)) { continue; }
    L += log (1.0 - Prediction(F[v], HV)) + NegWgt * DotProduct(F[v], HV);
  }
  for (TIntFltH::TIter HI = HV.BegI(); HI < HV.EndI(); HI++) {
    double HOSum = HOVIDSV[VID].Len() > 0?  HOSumFV[HI.GetKey()].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    L -= NegWgt * (SumFV[HI.GetKey()] - HOSum - GetComOut(VID, HI.GetKey())) * HI.GetDat();
  }
  //add regularization
  if (RegCoef > 0.0) { //L1
    L -= RegCoef * Sum(HV);
  }
  if (RegCoef < 0.0) { //L2
    L += RegCoef * Norm2(HV);
  }

  return L;
}
*/
void TCoda::GradientForNode(const bool IsRow, const int UID, TIntFltH& GradU, const TIntSet& CIDSet) {
  GradU.Gen(CIDSet.Len());

  TFltV HOSumHV; //adjust for Hv of v hold out
  if (HOVIDSV[UID].Len() > 0) {
    HOSumHV.Gen(NumComs);
    for (int e = 0; e < HOVIDSV[UID].Len(); e++) {
      for (int c = 0; c < SumHV.Len(); c++) {
        HOSumHV[c] += GetCom(! IsRow, HOVIDSV[UID][e], c);
      }
    }
  }
    
  TNGraph::TNodeI NI = G->GetNI(UID);
  int Deg = IsRow ? NI.GetOutDeg(): NI.GetInDeg();
  TFltV PredV(Deg), GradV(CIDSet.Len());
  TIntV CIDV(CIDSet.Len());
  for (int e = 0; e < Deg; e++) {
    int VID = IsRow? NI.GetOutNId(e): NI.GetInNId(e);
    if (VID == UID) { continue; }
    if (HOVIDSV[UID].IsKey(VID)) { continue; }
    PredV[e] = IsRow? Prediction(UID, VID): Prediction(VID, UID);
  }
  
  for (int c = 0; c < CIDSet.Len(); c++) {
    int CID = CIDSet.GetKey(c);
    double Val = 0.0;
    for (int e = 0; e < Deg; e++) {
      int VID = IsRow? NI.GetOutNId(e): NI.GetInNId(e);
      if (VID == UID) { continue; }
      if (HOVIDSV[UID].IsKey(VID)) { continue; }
      Val += PredV[e] * GetCom(! IsRow, VID, CID) / (1.0 - PredV[e]) + NegWgt * GetCom(! IsRow, VID, CID);
    }
    double HOSum = HOVIDSV[UID].Len() > 0?  HOSumHV[CID].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    Val -= NegWgt * (GetSumVal(! IsRow, CID) - HOSum - GetCom(! IsRow, UID, CID));
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
      GradV[c] += 2 * RegCoef * GetCom(IsRow, UID, CIDV[c]); 
    }
  }
  for (int c = 0; c < GradV.Len(); c++) {
    if (GetCom(IsRow, UID, CIDV[c]) == 0.0 && GradV[c] < 0.0) { continue; }
    if (fabs(GradV[c]) < 0.0001) { continue; }
    GradU.AddDat(CIDV[c], GradV[c]);
  }
  for (int c = 0; c < GradU.Len(); c++) {
    if (GradU[c] >= 10) { GradU[c] = 10; }
    if (GradU[c] <= -10) { GradU[c] = -10; }
    IAssert(GradU[c] >= -10);
  }
}

/*
void TCoda::GradientForRow(const int UID, TIntFltH& GradU, const TIntSet& CIDSet) {
  GradU.Gen(CIDSet.Len());

  TFltV HOSumHV; //adjust for Hv of v hold out
  if (HOVIDSV[UID].Len() > 0) {
    HOSumHV.Gen(SumHV.Len());
    
    for (int e = 0; e < HOVIDSV[UID].Len(); e++) {
      for (int c = 0; c < SumHV.Len(); c++) {
        HOSumHV[c] += GetComIn(HOVIDSV[UID][e], c);
      }
    }
  }
    
  TNGraph::TNodeI NI = G->GetNI(UID);
  int Deg = NI.GetOutDeg();
  TFltV PredV(Deg), GradV(CIDSet.Len());
  TIntV CIDV(CIDSet.Len());
  for (int e = 0; e < Deg; e++) {
    if (NI.GetOutNId(e) == UID) { continue; }
    if (HOVIDSV[UID].IsKey(NI.GetOutNId(e))) { continue; }
    PredV[e] = Prediction(UID, NI.GetOutNId(e));
  }
  
  for (int c = 0; c < CIDSet.Len(); c++) {
    int CID = CIDSet.GetKey(c);
    double Val = 0.0;
    for (int e = 0; e < Deg; e++) {
      int VID = NI.GetNbrNId(e);
      if (VID == UID) { continue; }
      if (HOVIDSV[UID].IsKey(VID)) { continue; }
      Val += PredV[e] * GetComIn(VID, CID) / (1.0 - PredV[e]) + NegWgt * GetComIn(VID, CID);
    }
    double HOSum = HOVIDSV[UID].Len() > 0?  HOSumHV[CID].Val: 0.0;//subtract Hold out pairs only if hold out pairs exist
    Val -= NegWgt * (SumHV[CID] - HOSum - GetComIn(UID, CID));
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
      GradV[c] += 2 * RegCoef * GetComOut(UID, CIDV[c]); 
    }
  }
  for (int c = 0; c < GradV.Len(); c++) {
    if (GetComOut(UID, CIDV[c]) == 0.0 && GradV[c] < 0.0) { continue; }
    if (fabs(GradV[c]) < 0.0001) { continue; }
    GradU.AddDat(CIDV[c], GradV[c]);
  }
  for (int c = 0; c < GradU.Len(); c++) {
    if (GradU[c] >= 10) { GradU[c] = 10; }
    if (GradU[c] <= -10) { GradU[c] = -10; }
    IAssert(GradU[c] >= -10);
  }
}
*/

void TCoda::GetCmtyVV(const bool IsOut, TVec<TIntV>& CmtyVV) {
  GetCmtyVV(IsOut, CmtyVV, sqrt(1.0 / G->GetNodes()), 3);
}

/// extract community affiliation for given community ID
void TCoda::GetCommunity(TIntV& CmtyVIn, TIntV& CmtyVOut, const int CID, const double Thres) {
  TIntFltH NIDFucH(F.Len() / 10), NIDHucH(F.Len() / 10);
  for (int u = 0; u < NIDV.Len(); u++) {
    int NID = u;
    if (! NodesOk) { NID = NIDV[u]; }
    if (GetCom(true, u, CID) >= Thres) { NIDFucH.AddDat(NID, GetCom(true, u, CID)); }
    if (GetCom(false, u, CID) >= Thres) { NIDHucH.AddDat(NID, GetCom(false, u, CID)); }
  }
  NIDFucH.SortByDat(false);
  NIDHucH.SortByDat(false);
  NIDFucH.GetKeyV(CmtyVOut);
  NIDHucH.GetKeyV(CmtyVIn);
}

void TCoda::GetTopCIDs(TIntV& CIdV, const int TopK, const int IsAverage, const int MinSz) {
  TIntFltH CIdFHH;
  for (int c = 0; c < GetNumComs(); c++) {
    if (IsAverage == 1) {
      TIntV CmtyVIn, CmtyVOut;
      GetCommunity(CmtyVIn, CmtyVOut, c);
      if (CmtyVIn.Len() == 0 || CmtyVOut.Len() == 0) { continue; }
      if (CmtyVIn.Len() < MinSz || CmtyVOut.Len() < MinSz) { continue; }
      CIdFHH.AddDat(c, GetSumVal(true, c) * GetSumVal(false, c) / (double) CmtyVIn.Len() / (double) CmtyVOut.Len());
    } else {
      CIdFHH.AddDat(c, GetSumVal(true, c) * GetSumVal(false, c));
    }
  }
  CIdFHH.SortByDat(false);
  CIdFHH.GetKeyV(CIdV);
  if (TopK < CIdFHH.Len()) { CIdV.Trunc(TopK); }
}

/// extract community affiliation for outgoing edges from F_uc
void TCoda::GetCmtyVV(const bool IsOut, TVec<TIntV>& CmtyVV, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
  TIntFltH CIDSumFH(NumComs);
  for (int c = 0; c < NumComs; c++) {
    CIDSumFH.AddDat(c, GetSumVal(IsOut, c));
  }
  CIDSumFH.SortByDat(false);
  for (int c = 0; c < NumComs; c++) {
    int CID = CIDSumFH.GetKey(c);
    TIntFltH NIDFucH, NIDHucH, NIDInOutH;
    TIntV CmtyV;
    GetNIDValH(NIDInOutH, NIDFucH, NIDHucH, CID, Thres);
    if (IsOut) {
      NIDFucH.GetKeyV(CmtyV);
    } else {
      NIDHucH.GetKeyV(CmtyV);
    }
    if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}

void TCoda::GetCmtyVVUnSorted(const bool IsOut, TVec<TIntV>& CmtyVV, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
  for (int c = 0; c < NumComs; c++) {
    TIntV CmtyV((int) (GetSumVal(IsOut, c) * 10), 0);
    for (int u = 0; u < G->GetNodes(); u++) {
      if (GetCom(IsOut, u, c) > Thres) { CmtyV.Add(NIDV[u]); }
    }
    if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}

PNGraph TCoda::GetGraphRawNID() {
  PNGraph NewG = TNGraph::New(G->GetNodes(), -1);
  for (TNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    //add node
    int NIdx = NI.GetId();
    int NID = NIDV[NIdx];
    if (! NewG->IsNode(NID)) { NewG->AddNode(NID); }
    //add edge
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      int OutNID = NIDV[NI.GetOutNId(e)];
      if (! NewG->IsNode(OutNID)) { NewG->AddNode(OutNID); }
      NewG->AddEdge(NID, OutNID);
    }
  }
  IAssert(G->GetNodes() == NewG->GetNodes());
  IAssert(G->GetEdges() == NewG->GetEdges());
  return NewG;
}

void TCoda::GetNIDValH(TIntFltH& NIdValInOutH, TIntFltH& NIdValOutH, TIntFltH& NIdValInH, const int CID, const double Thres) {
  NIdValOutH.Gen((int) GetSumVal(true, CID) + 1);
  NIdValInH.Gen((int) GetSumVal(false, CID) + 1);
  NIdValInOutH.Gen((int) GetSumVal(false, CID) + 1);
  if (GetSumVal(true, CID) < Thres && GetSumVal(false, CID) < Thres) { return; }
  for (int u = 0; u < NIDV.Len(); u++) {
    if (GetCom(true, u, CID) >= Thres && GetCom(false, u, CID) >= Thres) {
      NIdValInOutH.AddDat(NIDV[u], GetCom(true, u, CID) + GetCom(false, u, CID));
    }
    if (GetCom(true, u, CID) >= Thres) {
      NIdValOutH.AddDat(NIDV[u], GetCom(true, u, CID));
    }
    if (GetCom(false, u, CID) >= Thres) {
      NIdValInH.AddDat(NIDV[u], GetCom(false, u, CID));
    }
  }
  NIdValInH.SortByDat(false);
  NIdValOutH.SortByDat(false);
  NIdValInOutH.SortByDat(false);
}

void TCoda::DumpMemberships(const TStr& OutFNm, const TStrHash<TInt>& NodeNameH, const double Thres) {
  if (NodeNameH.Len() > 0) { IAssert(NodeNameH.Len() == G->GetNodes()); }
  FILE* FId = fopen(OutFNm.CStr(), "wt");
  TIntFltH CIDSumFH(NumComs);
  for (int c = 0; c < NumComs; c++) {
    CIDSumFH.AddDat(c, GetSumVal(true, c) * GetSumVal(false, c));
  }
  CIDSumFH.SortByDat(false);
  for (int c = 0; c < NumComs; c++) {
    int CID = CIDSumFH.GetKey(c);
    TIntFltH NIDOutFH, NIDInFH, NIDInOutFH;
    GetNIDValH(NIDInOutFH, NIDOutFH, NIDInFH, CID, Thres);
    if (NIDOutFH.Len() == 0 || NIDInFH.Len() == 0) { continue; }
    
    /*
    if (GetSumVal(true, CID) < Thres && GetSumVal(false, CID) < Thres) { continue; }
    for (int u = 0; u < NIDV.Len(); u++) {
      if (GetCom(true, u, CID) >= Thres && GetCom(false, u, CID) >= Thres) { 
        NIDInOutFH.AddDat(u, GetCom(true, u, CID) + GetCom(false, u, CID)); 
      } else if (GetCom(true, u, CID) >= Thres) {
        NIDOutFH.AddDat(u, GetCom(true, u, CID));
      } else if (GetCom(false, u, CID) >= Thres) {
        NIDInFH.AddDat(u, GetCom(false, u, CID));
      }
    }
    NIDInOutFH.SortByDat(false);
    NIDInFH.SortByDat(false);
    NIDOutFH.SortByDat(false);
    */
    fprintf(FId, "%d\t%d\t%d\t%f\t%f\t%f\t", NIDInOutFH.Len(), NIDInFH.Len() - NIDInOutFH.Len(), NIDOutFH.Len() - NIDInOutFH.Len(), CIDSumFH.GetDat(CID).Val, GetSumVal(false, CID).Val, GetSumVal(true, CID).Val);
    fprintf(FId, "InOut:\t");
    for (int u = 0; u < NIDInOutFH.Len(); u++) {
      int NIdx = NIDInOutFH.GetKey(u);
      fprintf(FId, "%s (%f)\t", NodeNameH.GetKey(NIdx), NIDInOutFH[u].Val);
    }
    fprintf(FId, "In:\t");
    for (int u = 0; u < NIDInFH.Len(); u++) {
      int NIdx = NIDInFH.GetKey(u);
      fprintf(FId, "%s (%f)\t", NodeNameH.GetKey(NIdx), NIDInFH[u].Val);
    }
    fprintf(FId, "Out:\t");
    for (int u = 0; u < NIDOutFH.Len(); u++) {
      int NIdx = NIDOutFH.GetKey(u);
      fprintf(FId, "%s (%f)\t", NodeNameH.GetKey(NIdx), NIDOutFH[u].Val);
    }
    fprintf(FId, "\n");
  }
  fclose(FId);
}


void TCoda::DumpMemberships(const TStr& OutFNm, const double Thres) {
  TStrHash<TInt> NodeNameH(G->GetNodes(), false);
  for (int u = 0; u < NIDV.Len(); u++) { NodeNameH.AddKey(TStr::Fmt("%d", NIDV[u].Val)); }
  DumpMemberships(OutFNm, NodeNameH, Thres);
}

void TCoda::GetCmtyS(TIntSet& CmtySOut, TIntSet& CmtySIn, const int CID, const double Thres) {
  CmtySOut.Gen(G->GetNodes() / 10);
  CmtySIn.Gen(G->GetNodes() / 10);
  for (int u = 0; u < NIDV.Len(); u++) {
    if (GetCom(true, u, CID) > Thres) {
      //CmtySOut.AddKey(
    }
  }
}

/*
void TCoda::GetCmtyVVIn(TVec<TIntV>& CmtyVV) {
  GetCmtyVVIn(CmtyVV, sqrt(1.0 / G->GetNodes()), 3);
}

/// extract community affiliation for incoming edges from H_uc
void TCoda::GetCmtyVVIn(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
  TIntFltH CIDSumHH(NumComs);
  for (int c = 0; c < SumHV.Len(); c++) {
    CIDSumHH.AddDat(c, SumHV[c]);
  }
  CIDSumHH.SortByDat(false);
  for (int c = 0; c < NumComs; c++) {
    int CID = CIDSumHH.GetKey(c);
    TIntFltH NIDHucH(H.Len() / 10);
    TIntV CmtyV;
    IAssert(SumHV[CID] == CIDSumHH.GetDat(CID));
    if (SumHV[CID] < Thres) { continue; }
    for (int u = 0; u < H.Len(); u++) {
      int NID = u;
      if (! NodesOk) { NID = NIDV[u]; }
      if (GetComIn(u, CID) >= Thres) { NIDHucH.AddDat(NID, GetComIn(u, CID)); }
    }
    NIDHucH.SortByDat(false);
    NIDHucH.GetKeyV(CmtyV);
    if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}
*/
void TCoda::GetCmtyVV(TVec<TIntV>& CmtyVVOut, TVec<TIntV>& CmtyVVIn, const int MinSz) {
  GetCmtyVV(false, CmtyVVIn, sqrt(1.0 / G->GetNodes()), MinSz);
  GetCmtyVV(true, CmtyVVOut, sqrt(1.0 / G->GetNodes()), MinSz);
}

void TCoda::GetCmtyVVUnSorted(TVec<TIntV>& CmtyVVOut, TVec<TIntV>& CmtyVVIn) {
  GetCmtyVVUnSorted(false, CmtyVVIn, sqrt(1.0 / G->GetNodes()));
  GetCmtyVVUnSorted(true, CmtyVVOut, sqrt(1.0 / G->GetNodes()));
}

void TCoda::GetCmtyVV(TVec<TIntV>& CmtyVVOut, TVec<TIntV>& CmtyVVIn, const double ThresOut, const double ThresIn, const int MinSz) {
  GetCmtyVV(false, CmtyVVIn, ThresIn, MinSz);
  GetCmtyVV(true, CmtyVVOut, ThresOut, MinSz);
}

/// estimate number of communities using cross validation
int TCoda::FindComsByCV(const int NumThreads, const int MaxComs, const int MinComs, const int DivComs, const TStr OutFNm, const int EdgesForCV, const double StepAlpha, const double StepBeta) {
    double ComsGap = exp(TMath::Log((double) MaxComs / (double) MinComs) / (double) DivComs);
    TIntV ComsV;
    ComsV.Add(MinComs);
    while (ComsV.Len() < DivComs) {
      int NewComs = int(ComsV.Last() * ComsGap);
      if (NewComs == ComsV.Last().Val) { NewComs++; }
      ComsV.Add(NewComs);
    }
    if (ComsV.Last() < MaxComs) { ComsV.Add(MaxComs); }
    return FindComsByCV(ComsV, 0.1, NumThreads, OutFNm, EdgesForCV, StepAlpha, StepBeta);
}

int TCoda::FindComsByCV(TIntV& ComsV, const double HOFrac, const int NumThreads, const TStr PlotLFNm, const int EdgesForCV, const double StepAlpha, const double StepBeta) {
  if (ComsV.Len() == 0) {
    int MaxComs = G->GetNodes() / 5;
    ComsV.Add(2);
    while(ComsV.Last() < MaxComs) { ComsV.Add(ComsV.Last() * 2); }
  }
  int MaxIterCV = 3;

  TVec<TVec<TIntSet> > HoldOutSets(MaxIterCV);
  TFltIntPrV NIdPhiV;
  TAGMFastUtil::GetNIdPhiV<PNGraph>(G, NIdPhiV);

  if (G->GetEdges() > EdgesForCV) { //if edges are many enough, use CV
    printf("generating hold out set\n");
    TIntV NIdV1, NIdV2;
    G->GetNIdV(NIdV1);
    G->GetNIdV(NIdV2);
    for (int IterCV = 0; IterCV < MaxIterCV; IterCV++) {
      // generate holdout sets
      TAGMFastUtil::GenHoldOutPairs(G, HoldOutSets[IterCV], HOFrac, Rnd);
      /*
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
      */
    }
    
    printf("hold out set generated\n");
  }

  TFltV HOLV(ComsV.Len());
  TIntFltPrV ComsLV;
  for (int c = 0; c < ComsV.Len(); c++) {
    const int Coms = ComsV[c];
    printf("Try number of Coms:%d\n", Coms);

    if (G->GetEdges() > EdgesForCV) { //if edges are many enough, use CV
      for (int IterCV = 0; IterCV < MaxIterCV; IterCV++) {
        HOVIDSV = HoldOutSets[IterCV];
        NeighborComInit(NIdPhiV, Coms);
        printf("Initialized\n");

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

double TCoda::LikelihoodHoldOut(const bool DoParallel) { 
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

double TCoda::GetStepSizeByLineSearch(const bool IsRow, const int UID, const TIntFltH& DeltaV, const TIntFltH& GradV, const double& Alpha, const double& Beta, const int MaxIter) {
  double StepSize = 1.0;
  double InitLikelihood = LikelihoodForNode(IsRow, UID);
  TIntFltH NewVarV(DeltaV.Len());
  for(int iter = 0; iter < MaxIter; iter++) {
    for (int i = 0; i < DeltaV.Len(); i++){
      int CID = DeltaV.GetKey(i);
      double NewVal;
      NewVal = GetCom(IsRow, UID, CID) + StepSize * DeltaV.GetDat(CID);
      if (NewVal < MinVal) { NewVal = MinVal; }
      if (NewVal > MaxVal) { NewVal = MaxVal; }
      NewVarV.AddDat(CID, NewVal);
    }
    if (LikelihoodForNode(IsRow, UID, NewVarV) < InitLikelihood + Alpha * StepSize * DotProduct(GradV, DeltaV)) {
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

int TCoda::MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm, const double StepAlpha, const double StepBeta) {
  time_t InitTime = time(NULL);
  TExeTm ExeTm, CheckTm;
  int iter = 0, PrevIter = 0;
  TIntFltPrV IterLV;
  TNGraph::TNodeI UI;
  double PrevL = TFlt::Mn, CurL = 0.0;
  TIntV NIdxV(F.Len(), 0);
  for (int i = 0; i < F.Len(); i++) { NIdxV.Add(i); }
  IAssert(NIdxV.Len() == F.Len());
  TIntFltH GradV;
  while(iter < MaxIter) {
    NIdxV.Shuffle(Rnd);
    for (int ui = 0; ui < F.Len(); ui++, iter++) {
      const bool IsRow = (ui % 2 == 0);
      int u = NIdxV[ui]; //
      //find set of candidate c (we only need to consider c to which a neighbor of u belongs to)
      UI = G->GetNI(u);
      const int Deg = IsRow? UI.GetOutDeg(): UI.GetInDeg();
      TIntSet CIDSet(5 * Deg);
      for (int e = 0; e < Deg; e++) {
        int VID = IsRow? UI.GetOutNId(e): UI.GetInNId(e);
        if (HOVIDSV[u].IsKey(VID)) { continue; }
        TIntFltH NbhCIDH = IsRow? H[VID]: F[VID];
        for (TIntFltH::TIter CI = NbhCIDH.BegI(); CI < NbhCIDH.EndI(); CI++) {
          CIDSet.AddKey(CI.GetKey());
          //printf("CI.GetKey:%d\n", CI.GetKey());
          IAssert(CI.GetKey() <= NumComs);
        }
      }
      TIntFltH& CurMem = IsRow? F[u]: H[u];
      for (TIntFltH::TIter CI = CurMem.BegI(); CI < CurMem.EndI(); CI++) { //remove the community membership which U does not share with its neighbors
        if (! CIDSet.IsKey(CI.GetKey())) {
          DelCom(IsRow, u, CI.GetKey());
        }
      }
      if (CIDSet.Empty()) { continue; }
      GradientForNode(IsRow, u, GradV, CIDSet);
      if (Norm2(GradV) < 1e-4) { continue; }
      double LearnRate = GetStepSizeByLineSearch(IsRow, u, GradV, GradV, StepAlpha, StepBeta);
      if (LearnRate == 0.0) { continue; }
      for (int ci = 0; ci < GradV.Len(); ci++) {
        int CID = GradV.GetKey(ci);
        double Change = LearnRate * GradV.GetDat(CID);
        double NewFuc = GetCom(IsRow, u, CID) + Change;
        if (NewFuc <= 0.0) {
          DelCom(IsRow, u, CID);
        } else {
          AddCom(IsRow, u, CID, NewFuc);
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

int TCoda::MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const int ChunkSize, const TStr PlotNm, const double StepAlpha, const double StepBeta) {
  //parallel
  time_t InitTime = time(NULL);
  //uint64 StartTm = TSecTm::GetCurTm().GetAbsSecs();
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
  TBoolV IsRowV(ChunkNum * ChunkSize);
  for (iter = 0; iter < MaxIter; iter++) {
    NIdxV.Clr(false);
    for (int i = 0; i < F.Len(); i++) { 
      //if (NIDOPTV[i] == 0) {  NIdxV.Add(i); }
      NIdxV.Add(i);
    }
    IAssert (NIdxV.Len() <= F.Len());
    NIdxV.Shuffle(Rnd);
    // compute gradient for chunk of nodes
#pragma omp parallel for schedule(static, 1)
    for (int TIdx = 0; TIdx < ChunkNum; TIdx++) {
      TIntFltH GradV;
      for (int ui = TIdx * ChunkSize; ui < (TIdx + 1) * ChunkSize; ui++) {
        const bool IsRow = (ui % 2 == 0);
        NewNIDV[ui] = -1;
        if (ui > NIdxV.Len()) { continue; }
        const int u = NIdxV[ui]; //
        //find set of candidate c (we only need to consider c to which a neighbor of u belongs to)
        TNGraph::TNodeI UI = G->GetNI(u);
        const int Deg = IsRow? UI.GetOutDeg(): UI.GetInDeg();
        TIntSet CIDSet(5 * Deg);
        TIntFltH CurFU = IsRow? F[u]: H[u];
        for (int e = 0; e < Deg; e++) {
          int VID = IsRow? UI.GetOutNId(e): UI.GetInNId(e);
          if (HOVIDSV[u].IsKey(VID)) { continue; }
          TIntFltH& NbhCIDH = IsRow? H[VID]: F[VID];
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
          GradientForNode(IsRow, u, GradV, CIDSet);
          if (Norm2(GradV) < 1e-4) { NIDOPTV[u] = 1; continue; }
          double LearnRate = GetStepSizeByLineSearch(IsRow, u, GradV, GradV, StepAlpha, StepBeta);
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
        IsRowV[ui] = IsRow;
      }
    }
    int NumNoChangeGrad = 0;
    int NumNoChangeStepSize = 0;
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID == -1) { NumNoChangeGrad++; continue; }
      if (NewNID == -2) { NumNoChangeStepSize++; continue; }
      if (IsRowV[ui]) {
        for (TIntFltH::TIter CI = F[NewNID].BegI(); CI < F[NewNID].EndI(); CI++) {
          SumFV[CI.GetKey()] -= CI.GetDat();
        }
      } else {
        for (TIntFltH::TIter CI = H[NewNID].BegI(); CI < H[NewNID].EndI(); CI++) {
          SumHV[CI.GetKey()] -= CI.GetDat();
        }
      }
    }
#pragma omp parallel for
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID < 0) { continue; }
      if (IsRowV[ui]) {
        F[NewNID] = NewF[ui];
      } else {
        H[NewNID] = NewF[ui];
      }
    }
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID < 0) { continue; }
      if (IsRowV[ui]) {
        for (TIntFltH::TIter CI = F[NewNID].BegI(); CI < F[NewNID].EndI(); CI++) {
          SumFV[CI.GetKey()] += CI.GetDat();
        }
      } else {
        for (TIntFltH::TIter CI = H[NewNID].BegI(); CI < H[NewNID].EndI(); CI++) {
          SumHV[CI.GetKey()] += CI.GetDat();
        }
      }
    }
    // update the nodes who are optimal
    for (int ui = 0; ui < NewNIDV.Len(); ui++) {
      int NewNID = NewNIDV[ui];
      if (NewNID < 0) { continue; }
      TNGraph::TNodeI UI = G->GetNI(NewNID);
      NIDOPTV[NewNID] = 0;
      for (int e = 0; e < UI.GetDeg(); e++) {
        NIDOPTV[UI.GetNbrNId(e)] = 0;
      }
    }
    int OPTCnt = 0;
    for (int i = 0; i < NIDOPTV.Len(); i++) { if (NIDOPTV[i] == 1) { OPTCnt++; } }
    /*
    if (! PlotNm.Empty()) {
      printf("\r%d iterations [%s] %lu secs", iter * ChunkSize * ChunkNum, ExeTm.GetTmStr(), TSecTm::GetCurTm().GetAbsSecs() - StartTm);
      if (PrevL > TFlt::Mn) { printf(" (%f) %d g %d s %d OPT", PrevL, NumNoChangeGrad, NumNoChangeStepSize, OPTCnt); }
      fflush(stdout);
    }
    */
    if ((iter - PrevIter) * ChunkSize * ChunkNum >= G->GetNodes()) {
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
    printf("\nMLE completed with %d iterations(%lu secs)\n", iter, time(NULL) - InitTime);
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
  } else {
    printf("\rMLE completed with %d iterations(%lu secs)\n", iter, time(NULL) - InitTime);
    fflush(stdout);
  }
  return iter;
}
