#include "stdafx.h"
#include "signnet.h"

/////////////////////////////////////////////////
// Signed network
PSignNet TSignNet::GetEdgeSubNet(const int& EDat1, const int& EDat2, const int& EDat3) const {
  TIntSet EDatSet;
  EDatSet.AddKey(EDat1);
  if (EDat2!=TInt::Mn) { EDatSet.AddKey(EDat2); }
  if (EDat3!=TInt::Mn) { EDatSet.AddKey(EDat3); }
  PSignNet Net = TSignNet::New();
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (! EDatSet.IsKey(EI())) { continue; }
    if (! Net->IsNode(EI.GetSrcNId())) {
      Net->AddNode(EI.GetSrcNId(), EI.GetSrcNDat()); }
    if (! Net->IsNode(EI.GetDstNId())) {
      Net->AddNode(EI.GetDstNId(), EI.GetDstNDat()); }
    Net->AddEdge(EI);
  }
  return Net;
}

// randomly reassing edge signs (keep network structure intact)
void TSignNet::PermuteEdgeSigns() {
  TIntV EDatV(GetEdges(), 0);
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    EDatV.Add(EI());
  }
  EDatV.Shuffle(TInt::Rnd);
  int i = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    EI() = EDatV[i++];
  }
}

// keep number of +/- edges out of a node unchanged but rewire the network
void TSignNet::RewireNetwork() {
  TIntH NIdDatH;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NIdDatH.AddDat(NI.GetId(), NI()); }
  // rewire plus and minus networks
  PNGraph PlusG = TSnap::ConvertGraph<PNGraph>(GetSignSubNet(+1));
  PNGraph MinusG = TSnap::ConvertGraph<PNGraph>(GetSignSubNet(-1));
  PlusG = TSnap::GenRewire(PlusG);
  MinusG = TSnap::GenRewire(MinusG);
  // create network
  Clr(false);
  for (TNGraph::TNodeI NI = PlusG->BegNI(); NI < PlusG->EndNI(); NI++) {
    AddNode(NI.GetId()); }
  for (TNGraph::TNodeI NI = MinusG->BegNI(); NI < MinusG->EndNI(); NI++) {
    AddNode(NI.GetId(), NIdDatH.GetDat(NI.GetId())); }
  for (TNGraph::TEdgeI EI = PlusG->BegEI(); EI < PlusG->EndEI(); EI++) {
    AddEdge(EI.GetSrcNId(), EI.GetDstNId(), +1); }
  for (TNGraph::TEdgeI EI = MinusG->BegEI(); EI < MinusG->EndEI(); EI++) {
    AddEdge(EI.GetSrcNId(), EI.GetDstNId(), -1); }
}

// remove self-edges and remove bi-dir edges (keep the edge in a random direction)
void TSignNet::SimplifyNet() {
  TIntPrV DelV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI.GetSrcNId() > EI.GetDstNId()) { continue; }
    if (EI.GetSrcNId() == EI.GetDstNId()) {
      DelV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId())); }
    else if (EI.GetSrcNId() < EI.GetDstNId() && IsEdge(EI.GetDstNId(), EI.GetSrcNId())) {
      if (TInt::Rnd.GetUniDev() < 0.5) {
        DelV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId())); }
      else {
        DelV.Add(TIntPr(EI.GetDstNId(), EI.GetSrcNId())); }
    }
  }
  printf("Deleting %d edges\n", DelV.Len());
  for (int e = 0; e < DelV.Len(); e++) {
    DelEdge(DelV[e].Val1, DelV[e].Val2);
  }
}

// flip direction and sign of OldSign edges
void TSignNet::FlipMinusEdges(const int&  OldSign, const int& NewSign) {
  TIntPrV MinusEV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI() == OldSign) {
      MinusEV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId())); }
  }
  int NFlip = 0;
  for (int e = 0; e < MinusEV.Len(); e++) {
    DelEdge(MinusEV[e].Val1, MinusEV[e].Val2);
    if (! IsEdge(MinusEV[e].Val2, MinusEV[e].Val1)) {
      AddEdge(MinusEV[e].Val2, MinusEV[e].Val1, NewSign);
      NFlip++;
    }
  }
  printf("%d/%d (%.4f) edges flipped\n", NFlip, MinusEV.Len(), NFlip/double(MinusEV.Len()));
}

void TSignNet::CountStructBalance() const {
  TIntSet NbhIdSet;
  THash<TIntTr, TInt> TriadCntH;
  TIntH SignH;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    SignH.AddDat(EI()) += 1;
  }
  printf("Structural balance triads: %d nodes, %d edges\n  background sign distribution:\n", GetNodes(), GetEdges());
  SignH.SortByKey(false);
  for (int i = 0; i < SignH.Len(); i++) {
    printf("\t%2d\t%d\n", SignH.GetKey(i)(), SignH[i]());
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const TNodeI SrcNI = GetNI(EI.GetSrcNId());
    const TNodeI DstNI = GetNI(EI.GetDstNId());
    const TInt E1Dat = EI();
    NbhIdSet.Clr(false);
    for (int es = 0; es < SrcNI.GetDeg(); es++) {
      NbhIdSet.AddKey(SrcNI.GetNbhNId(es));
    }
    for (int ed = 0; ed < DstNI.GetDeg(); ed++) {
      const int nbh = DstNI.GetNbhNId(ed);
      if (! NbhIdSet.IsKey(nbh)) { continue; }
      const TInt E3Dat = DstNI.GetNbhEDat(ed);
      for (int ed2 = 0; ed2 < SrcNI.GetDeg(); ed2++) {
        if (nbh != SrcNI.GetNbhNId(ed2)) { continue; }
        const TInt E2Dat = SrcNI.GetNbhEDat(ed2);
        // printf("%d-%d-%d  %2d %2d %2d\n", EI.GetSrcNId(), EI.GetDstNId(), nbh, E1Dat, E2Dat, E3Dat);
        TriadCntH.AddDat(TIntTr(TMath::Mx(E1Dat, E2Dat, E3Dat),
          TMath::Median(E1Dat, E2Dat, E3Dat), TMath::Mn(E1Dat, E2Dat, E3Dat))) += 1;
      }
    }
  }
  TriadCntH.SortByKey(false);
  printf("triad counts (all counts are real, not times 3):\n");
  int SumTriad = 0, SignTriad=0;
  for (int i = 0; i < TriadCntH.Len(); i++) {
    SumTriad += TriadCntH[i];
    TIntTr SignTr = TriadCntH.GetKey(i);
    if (SignTr.Val1!=0 && SignTr.Val2!=0 && SignTr.Val3!=0) {
      SignTriad += TriadCntH[i]; }
  }
  for (int i = 0; i < TriadCntH.Len(); i++) {
    TIntTr SignTr = TriadCntH.GetKey(i);
    printf("\t%2d %2d %2d\t%8d\t%f", SignTr.Val1(), SignTr.Val2(), SignTr.Val3(),
      TriadCntH[i]()/3, TriadCntH[i]()/double(SumTriad));
    if (SignTr.Val1!=0 && SignTr.Val2!=0 && SignTr.Val3!=0) {
      printf("\t%f", TriadCntH[i]()/double(SignTriad)); }
    printf("\n");
  }
}

// count the number of balanced and unbalanced triads an edge (NId1, NId2) participates in
void TSignNet::CountBalUnBal(const int& NId1, const int& NId2, int& BalTriads, int& UnBalTriads) const {
  BalTriads=0; UnBalTriads=0;
  int Sign=0;
  if (IsEdge(NId1, NId2)) { Sign=GetEDat(NId1, NId2); }
  else if (IsEdge(NId2, NId1)) { Sign=GetEDat(NId2, NId1); }
  else { return; }
  TNodeI NI1 = GetNI(NId1);
  TNodeI NI2 = GetNI(NId2);
  TIntH NbhH(NI1.GetDeg());
  for (int i = 0; i < NI1.GetDeg(); i++) {
    const int nid = NI1.GetNbhNId(i);
    if (nid!=NId1 && nid!=NId2) {
      NbhH.AddDat(nid, NI1.GetNbhEDat(i));
    }
  }
  for (int i = 0; i < NI2.GetDeg(); i++) {
    const int nid = NI2.GetNbhNId(i);
    if (NbhH.IsKey(nid)) {
      if (Sign*NbhH.GetDat(nid)*NI2.GetNbhEDat(i) == 1) { BalTriads++; }
      else { UnBalTriads++; }
    }
  }
}

void TSignNet::SetNodePart(const int& PartId) {
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NI() = PartId;
  }
}

// partition -1 means a node does not belong to any partition
void TSignNet::SetNodePart(TVec<TIntV>& PartNIdV) {
  SetNodePart(-1);
  for (int p = 0; p < PartNIdV.Len(); p++) {
    for (int n = 0; n < PartNIdV[p].Len(); n++) {
      if (IsNode(PartNIdV[p][n])) {
        GetNDat(PartNIdV[p][n]) = p+1; }
    }
  }
}

// balance statistics over node partitions
void TSignNet::GetPartStat(const TVec<TIntV>& PartNIdV, const TStr& Desc) const {
  THash<TIntPr, TIntPr> PartEdgeH;
  TIntH NIdPartH;
  int PartSz=0;
  for (int p = 0; p < PartNIdV.Len(); p++) {
    for (int n = 0; n < PartNIdV[p].Len(); n++) {
      NIdPartH.AddDat(PartNIdV[p][n], p);
    }
    PartSz += PartNIdV[p].Len();
  }
  TInt DstPart;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (! NIdPartH.IsKey(NI.GetId())) { continue; }
    const int SrcPart = NIdPartH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int Sign = NI.GetOutEDat(e);
      TIntPr& IOCnt = PartEdgeH.AddDat(TIntPr(SrcPart, Sign));
      if (NIdPartH.IsKeyGetDat(NI.GetOutNId(e), DstPart) && SrcPart==DstPart) {
        if (Sign>0) { IOCnt.Val1++; } else { IOCnt.Val2++; } }
      else {
        if (Sign<0) { IOCnt.Val1++; } else { IOCnt.Val2++; } }
    }
  }
  PartEdgeH.SortByKey();
  printf("%s: Satisfied edges (n:%d, e:%d, %d in partitions)\n", Desc.CStr(), GetNodes(), GetEdges(), PartSz);
  int OkEdges=0, AllEdges=0;
  for (int p = 0; p < PartEdgeH.Len(); p++) {
    double frac = 0.0;
    if (PartEdgeH[p].Val1+PartEdgeH[p].Val2>0) {
      frac=PartEdgeH[p].Val1/double(PartEdgeH[p].Val1+PartEdgeH[p].Val2); }
    OkEdges += PartEdgeH[p].Val1;
    AllEdges += PartEdgeH[p].Val1 + PartEdgeH[p].Val2;
    if (p==0 || PartEdgeH.GetKey(p).Val1!=PartEdgeH.GetKey(p-1).Val1) {
      printf("  %2d  %2d : %6d : %6d  =  %f     size: %d\n", PartEdgeH.GetKey(p).Val1(), PartEdgeH.GetKey(p).Val2(),
        PartEdgeH[p].Val1(), PartEdgeH[p].Val2(), frac,
        PartNIdV[PartEdgeH.GetKey(p).Val1].Len());
    } else {
      printf("      %2d : %6d : %6d  =  %f\n", PartEdgeH.GetKey(p).Val2(),
        PartEdgeH[p].Val1(), PartEdgeH[p].Val2(), frac);
    }
  }
  printf("  all ok edges: %d / %d = %f\n\n", OkEdges, AllEdges, OkEdges/double(AllEdges));
}

void TSignNet::MakeStatusConsistent() {
  printf("Make status consistent:\n");
  PSignNet Net2 = TSignNet::New();
  *Net2 = *this;
  Net2->PrintInfo("NETWORK");
  Net2->FlipMinusEdges(-1, 1);
  TIntV NIdOrderV;
  //TSnap::GetGreedyMxDag(TSnap::ConvertGraph<PNGraph>(Net2), Mega(1), NIdOrderV);
  Fail;
  TIntH NIdPosH;
  for (int i = 0; i < NIdOrderV.Len(); i++) {
    NIdPosH.AddDat(NIdOrderV[i], i); }
  TIntPrV DelEdgeV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (NIdPosH.GetDat(EI.GetSrcNId()) > NIdPosH.GetDat(EI.GetDstNId())) {
      DelEdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
    }
  }
  printf("  Deleting %d/%d edges\n", DelEdgeV.Len(), GetEdges());
  for (int e = 0; e < DelEdgeV.Len(); e++) {
    DelEdge(DelEdgeV[e].Val1, DelEdgeV[e].Val2);
  }
  PrintInfo("STATUS CONSISTENT");
}

void TSignNet::EdgeSignStat() const {
  int Plus=0, Minus=0;
  int OneWayP=0, OneWayM=0;
  int BWayPP=0, BWayPM=0, BWayMM=0;
  TEdgeI EndE = EndEI();
  for (TEdgeI EI = BegEI(); EI < EndE; EI++) {
    const int Sign = EI();
    TEdgeI EIX = GetEI(EI.GetDstNId(), EI.GetSrcNId());
    if (Sign==1) { Plus++; } else { Minus++; }
    if (EIX == EndE) {
      if (Sign==1) { OneWayP++; } else { OneWayM++; }
    } else {
      if (Sign==EIX()) {
        if (Sign==1) { BWayPP++; } else { BWayMM++; }
      } else { BWayPM++; }
    }
  }
  printf("Edges +1: %d\n", Plus);
  printf("Edges -1: %d\n", Minus);
  printf("One way +1:  %d\n", OneWayP);
  printf("One way -1:  %d\n", OneWayM);
  printf("Both way +1: %d\n", BWayPP);
  printf("Both way -1: %d\n", BWayMM);
  printf("Both way +1/-1: %d\n\n", BWayPM);
}

// plot the span of a particular edge:
//   number of common friends of the endpoints
void TSignNet::PlotSignCmnNbhs(const TStr& OutFNm) const {
  TFltFltH SupCmnH, SupCmnH2, OppCmnH, OppCmnH2;    // common friends
  THash<TFlt, TMom> CmnFracH, CmnFracH2;  // fraction of supporters
  PSignNet ThisPt = PSignNet((TSignNet*) this);
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int C = TSnap::GetCmnNbhs(ThisPt, EI.GetSrcNId(), EI.GetDstNId());
    if (EI() == 1) { SupCmnH.AddDat(C)++;  CmnFracH.AddDat(C).Add(1);
    } else if (EI() == -1) { OppCmnH.AddDat(C)++;  CmnFracH.AddDat(C).Add(0);  }
  }
  PSignNet PermNet = TSignNet::New();  *PermNet = *this;
  PermNet->PermuteEdgeSigns();
  for (TEdgeI EI = PermNet->BegEI(); EI < PermNet->EndEI(); EI++) {
    const int C = TSnap::GetCmnNbhs(PermNet, EI.GetSrcNId(), EI.GetDstNId());
    if (EI() == 1) { SupCmnH2.AddDat(C)++;  CmnFracH2.AddDat(C).Add(1);
    } else if (EI() == -1) { OppCmnH2.AddDat(C)++;  CmnFracH2.AddDat(C).Add(0);  }
  }
  { TGnuPlot GP("cmnNbrs-"+OutFNm, "Number of common friends of votes");
  GP.SetXYLabel("Number of common friends", "Number of votes"); GP.SetScale(gpsLog);
  GP.AddPlot(SupCmnH, gpwLinesPoints, "Support vote");
  GP.AddPlot(OppCmnH, gpwLinesPoints, "Oppose vote");
  GP.AddPlot(SupCmnH2, gpwLinesPoints, "PERMUTED: Support vote");
  GP.AddPlot(OppCmnH2, gpwLinesPoints, "PERMUTED: Oppose vote");
  GP.SavePng(); }

  TFltTrV V1, V2;
  for (int i = 0; i < CmnFracH.Len(); i++) {
    CmnFracH[i].Def();
    const double StdErr = CmnFracH[i].GetSDev()/sqrt((double)CmnFracH[i].GetVals());
    V1.Add(TFltTr(CmnFracH.GetKey(i), CmnFracH[i].GetMean(), StdErr));
  }
  for (int i = 0; i < CmnFracH2.Len(); i++) {
    CmnFracH2[i].Def();
    const double StdErr = CmnFracH2[i].GetSDev()/sqrt((double)CmnFracH2[i].GetVals());
    V2.Add(TFltTr(CmnFracH2.GetKey(i), CmnFracH2[i].GetMean(), StdErr));
  }
  V1.Sort();  V2.Sort();
  { TGnuPlot GP("cmnNbrsFrac-"+OutFNm, "Fraction of support votes");
  GP.SetXYLabel("Number of common friends", "Fraction of support votes"); GP.SetScale(gpsLog);
  GP.AddErrBar(V1, "True votes", "standard error");
  GP.AddErrBar(V2, "PERMUTED edge signs", "standard error");
  GP.SavePng(); }
  //TGnuPlot::PlotValMomH(CmnFracH, "fracCmn-"+OutFNm, "fraction of support edges having X common neighbors", "number of common neighbors", "fraction of support edges", gpsLog);

  //TGnuPlot::PlotValCntH(SupRngH, "rngSup-"+OutFNm, "range of support votes", "range (path length after edge is removed)", "count");
  //TGnuPlot::PlotValCntH(OppRngH, "rngOpp-"+OutFNm, "range of opposing votes", "range (path length after edge is removed)", "count");
  //TGnuPlot::PlotValMomH(RngFracH, "fracRng-"+OutFNm, "fraction of support edges spanning range X", "range", "fraction of support edges");
}

// get a induced subgraph on nodes N1, N2, N3
PSignNet TSignNet::GetTriad(const int& N1, const int& N2, const int& N3) const {
  PSignNet Net2 = TSignNet::New();
  Net2->AddNode(0);  Net2->AddNode(1);  Net2->AddNode(2);
  if (IsEdge(N1, N2)) { Net2->AddEdge(0, 1, GetEDat(N1, N2)); }
  if (IsEdge(N2, N1)) { Net2->AddEdge(1, 0, GetEDat(N2, N1)); }
  if (IsEdge(N1, N3)) { Net2->AddEdge(0, 2, GetEDat(N1, N3)); }
  if (IsEdge(N3, N1)) { Net2->AddEdge(2, 0, GetEDat(N3, N1)); }
  if (IsEdge(N2, N3)) { Net2->AddEdge(1, 2, GetEDat(N2, N3)); }
  if (IsEdge(N3, N2)) { Net2->AddEdge(2, 1, GetEDat(N3, N2)); }
  return Net2;
}

bool TSignNet::IsClosedTriad() const {
  if (GetNodes() != 3) { return false; }
  TNodeI NI = BegNI();
  const int nid1 = NI.GetId(); NI++;
  const int nid2 = NI.GetId(); NI++;
  const int nid3 = NI.GetId();
  if (IsEdge(nid1, nid2, false) && IsEdge(nid2, nid3, false) && IsEdge(nid1, nid3, false)) {
    return true; }
  return false;
}

bool TSignNet::IsBalanced() const {
  if (GetEdges()!=3) { printf("%d\n", GetEdges()); }
  IAssert(GetEdges()==3 && IsClosedTriad());
  int Sig=1;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    Sig *= EI();
  }
  if (Sig == 1) { return true; }
  else { return false; }
}

// only simple triads
double TSignNet::GetTriadProb(const double& PlusProb) const {
  const int E = GetEdges();
  int P = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int sign = EI();  IAssert(sign==1 || sign==-1);
    if (sign==1) { P++; }
  }
  if (GetNodes()==3) { // simple triad
    TNodeI NI = BegNI();
    int Homo = 1;
    if (P!=3 && P!=0 && NI.GetOutDeg()==1 && (NI++).GetOutDeg()==1 && (NI++).GetOutDeg()==1) {
      NI = BegNI();
      if (NI.GetId() == GetNI(GetNI(NI.GetOutNId(0)).GetOutNId(0)).GetOutNId(0)) {
        Homo=3; // cycle with one minus has three symmetries
      }
    }
    return Homo * pow(PlusProb, P) * pow(1-PlusProb, E-P); // * TMath::Choose(E, P);
  }
  return pow(PlusProb, P) * pow(1-PlusProb, E-P);
}

// triad is OldTriad with one additional edge, which one is it
TIntPr TSignNet::GetNewEdge(const PSignNet& OldTriad) const {
  IAssert(GetNodes()==3 && OldTriad->GetNodes()==3 && GetEdges()-OldTriad->GetEdges()==1);
  PSignNet New = TSignNet::New(); *New=*this;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    New->DelEdge(EI.GetSrcNId(), EI.GetDstNId());
    if (TSignNet::IsSameTriad(OldTriad, New)!=-1) {
      return TIntPr(EI.GetSrcNId(), EI.GetDstNId());
    }
    New->AddEdge(EI);
  }
  Fail;
  return TIntPr(-1,-1);
}

int TSignNet::GetEdgeSig(const int& nid1, const int& nid2) const {
  int e1, e2;
  if (IsEdge(nid1, nid2)) { e1=GetEDat(nid1,nid2); } else { e1=0; }
  if (IsEdge(nid2, nid1)) { e2=GetEDat(nid2,nid1); } else { e2=0; }
  if (e1==0 && e2==0) { return 0; }
  if (e1==1 && e2==0) { return 1; }
  if (e1==-1 && e2==0) { return 2; }
  if (e1==0 && e2==1) { return 3; }
  if (e1==0 && e2==-1) { return 4; }
  if (e1==1 && e2==1) { return 5; }
  if (e1==1 && e2==-1) { return 6; }
  if (e1==-1 && e2==1) { return 7; }
  if (e1==-1 && e2==-1) { return 8; }
  Fail; return -1;
}

TChA TSignNet::GetEdgeStr(const int& nid1, const int& nid2) const {
  const int sig = GetEdgeSig(nid1, nid2);
  return GetEdgeStr(sig);
}

TIntTr TSignNet::GetTriadSig(const bool& Canonical) const {
  if (GetNodes()!=3) { return TIntTr(); }
  TNodeI NI = BegNI();
  const int nid1 = NI.GetId(); NI++;
  const int nid2 = NI.GetId(); NI++;
  const int nid3 = NI.GetId();
  const int e1 = GetEdgeSig(nid1, nid2);
  const int e2 = GetEdgeSig(nid2, nid3);
  const int e3 = GetEdgeSig(nid3, nid1);
  if (! Canonical) {
    return TIntTr(e1, e2, e3); }
  else { // so that isomorphic triads have same signature
    const TIntTr MinTr = TMath::Mn(TIntTr(e1, e2, e3), TIntTr(e2, e3, e1), TIntTr(e3, e1, e2));
    const int e1a = GetEdgeSig(nid1, nid3);
    const int e2a = GetEdgeSig(nid3, nid2);
    const int e3a = GetEdgeSig(nid2, nid1);
    return TMath::Mn(MinTr, TMath::Mn(TIntTr(e1a, e2a, e3a), TIntTr(e2a, e3a, e1a), TIntTr(e3a, e1a, e2a)));
  }
}

TChA TSignNet::GetTriadStr(const bool& Canonical) const {
  return GetTriadStr(GetTriadSig(Canonical));
}

TChA TSignNet::GetEdgeStr(const int& EdgeSig) {
  switch(EdgeSig) {
    case 1 : return "+o";
    case 2 : return "-o";
    case 3 : return "o+";
    case 4 : return "o-";
    case 5 : return "++";
    case 6 : return "+-";
    case 7 : return "-+";
    case 8 : return "--";
    default : return TChA();
  }
}

TChA TSignNet::GetTriadStr(const TIntTr& TriadSig) {
  return GetEdgeStr(TriadSig.Val1)+"\t"+GetEdgeStr(TriadSig.Val2)+"\t"+GetEdgeStr(TriadSig.Val3);
}

/*// Sig1 is a 3 node closed triad, Sig2 is Sig1 with additional edge, what is the closed edge
static TIntPr TSignNet::GetSigNewEdge(const TIntTr& Sig1, const TIntTr& Sig2) const {
}

int TSignNet::GetEdgeSigSign(const int& EdgeSig) {
  if (EdgeSig==1 || EdgeSig==3) { return +1; }
  else if (EdgeSig==2 || EdgeSig==4) { return -1; }
  else { Fail; }
  return 0;
}

bool TSignNet::IsBalancedSig(const TIntTr& TriadSig) {
  if (GetEdgeSigSign(Tr.Val1)*GetEdgeSigSign(Tr.Val2)*GetEdgeSigSign(Tr.Val3) == 1) { return true; }
  return false;
}

bool IsCloseBalanced(TIntTr ClosedSig1, TIntTr ClosedSig2) {
  TInt& O = ClosedSig1.Val1>4 ? ClosedSig1.Val1 : (ClosedSig1.Val2>4 ? ClosedSig1.Val2 : ClosedSig1.Val3);
  TInt& S = ClosedSig2.Val1>4 ? ClosedSig2.Val1 : (ClosedSig2.Val2>4 ? ClosedSig2.Val2 : ClosedSig2.Val3);
  if (O==5 && S==6 || O==6 && S==5) { S-=2; }
  else if (O==7 && S==8 || O==8 && S==7) { S-=4; }
  else if (O==5 && S==7 || O==7 && S==5) { S=S/2-1; }
  else if (O==6 && S==8 || O==8 && S==6) { S=S/2-2; }
  else {
    printf("%s\n", TSignNet::GetTriadStr(ClosedSig1).CStr());
    return false;
  }
  return IsBalancedSig(ClosedSig2);
}
*/

int TSignNet::IsSameTriad(const PSignNet& Net1, const PSignNet& Net2) {
  static THash<TInt, TVec<TIntV> > PermH;
  if (PermH.Empty()) {
    PermH.AddDat(2).Add(TIntV::GetV(0,1));
    PermH.AddDat(2).Add(TIntV::GetV(1,0));
    PermH.AddDat(3).Add(TIntV::GetV(0,1,2));
    PermH.AddDat(3).Add(TIntV::GetV(0,2,1));
    PermH.AddDat(3).Add(TIntV::GetV(1,0,2));
    PermH.AddDat(3).Add(TIntV::GetV(1,2,0));
    PermH.AddDat(3).Add(TIntV::GetV(2,0,1));
    PermH.AddDat(3).Add(TIntV::GetV(2,1,0));
  }
  if (Net1->GetNodes() != Net2->GetNodes() || Net1->GetEdges() != Net2->GetEdges()) { return -1; }
  IAssert(Net1->GetNodes()==2 || Net1->GetNodes()==3);
  const TVec<TIntV>& PermV = PermH.GetDat(Net1->GetNodes());
  for (int p = 0; p < PermV.Len(); p++) {
    const TIntV& Perm = PermV[p];
    TSignNet::TEdgeI EndEI2 = Net2->EndEI();
    int i = 0;
    bool Ok = true;
    for (TSignNet::TNodeI NI1 = Net1->BegNI(); NI1 < Net1->EndNI(); NI1++, i++) {
      TSignNet::TNodeI NI2 = Net2->GetNI(Perm[i]);
      if (NI1.GetOutDeg() != NI2.GetOutDeg() || NI1.GetInDeg() != NI2.GetInDeg()) { Ok=false; break; }
      for (int e = 0; e < NI1.GetOutDeg(); e++) {
        const TSignNet::TEdgeI EI2 = Net2->GetEI(NI2.GetId(), Perm[NI1.GetOutNId(e)]);
        if (EI2 == EndEI2 || NI1.GetOutEDat(e) != EI2()) { Ok=false; break; }
      }
    }
    if (Ok) { return p; }
  }
  return -1;
}

// count signed triads
void TSignNet::CountSignedTriads(const TStr& OutFNm) const {
  printf("Count signed triads");
  // permutations
  TVec<PSignNet> TriadIdV;
  TVec<PSignNet> TriadIdV2;
  TIntH TriadIdCntH;
  TIntV NbhV;
  PSignNet ThisPt = PSignNet((TSignNet*) this);
  double AllPlusE=0, AllE = GetEdges();
  int c=0, Decile=int(AllE/100);
  TIntH UnSignCntH;
  TIntH SignToUnsignH;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    TSnap::GetCmnNbhs(ThisPt, EI.GetSrcNId(), EI.GetDstNId(), NbhV);
    for (int n = 0; n < NbhV.Len(); n++) {
      PSignNet TriadNet = GetTriad(EI.GetSrcNId(), EI.GetDstNId(), NbhV[n]);
      // count signed triad
      int TriadId = -1;
      for (int i = 0; i < TriadIdV.Len() && TriadId==-1; i++) {
        if (IsSameTriad(TriadIdV[i], TriadNet)!=-1) { TriadId=i; break; } }
      if (TriadId==-1) { TriadId=TriadIdV.Len();  TriadIdV.Add(TriadNet); }
      TriadIdCntH.AddDat(TriadId) += 1;
      // count unsigned triads
      TriadNet = GetTriad(EI.GetSrcNId(), EI.GetDstNId(), NbhV[n]);
      TriadNet->SetAllEDat(1);
      int TriadId2 = -1;
      for (int i = 0; i < TriadIdV2.Len() && TriadId2==-1; i++) {
        if (IsSameTriad(TriadIdV2[i], TriadNet)!=-1) { TriadId2=i; break; } }
      if (TriadId2==-1) { TriadId2=TriadIdV2.Len();  TriadIdV2.Add(TriadNet); }
      UnSignCntH.AddDat(TriadId2) += 1;
      SignToUnsignH.AddDat(TriadId, TriadId2);
    }
    if (EI() == 1) { AllPlusE += 1; }
    if (++c % Decile ==0) { printf("."); }
  }
  TVec<TTriple<TInt, TChA, TInt> > SortIdV;
  for (int t = 0; t < TriadIdCntH.Len(); t++) {
    PSignNet Net = TriadIdV[TriadIdCntH.GetKey(t)];
    const int E = Net->GetEdges();
    TriadIdCntH[t].Val /= E;
    //EdgTriadCnt.AddDat(E) += TriadIdCntH[t];
    SortIdV.Add(TTriple<TInt, TChA, TInt>(Net->GetEdges(), Net->GetTriadStr(true), TriadIdCntH.GetKey(t)));
  }
  for (int u=0; u < UnSignCntH.Len(); u++) {
    const int E = TriadIdV2[UnSignCntH.GetKey(u)]->GetEdges();
    UnSignCntH[u].Val /= E;
    //TriadIdV2[UnSignCntH.GetKey(t)]->Dump();
    printf("unsigned count:  %d\t%d\t%d\n", UnSignCntH.GetKey(u)(), E, UnSignCntH[u]());
  }
  SortIdV.Sort();
  // draw
  const double PlusProb = AllPlusE / AllE;
  FILE *T = fopen(TStr::Fmt("SignTriad-%s.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(T, "AB\tBC\tCA\tCount\tE[Count]\tSurprise\tTriadProb\n");
  printf("%d triads\n", TriadIdCntH.Len());
  printf("plus prob %d / %d = %f\n", int(AllPlusE), int(AllE), PlusProb);
  for (int t = 0; t < SortIdV.Len(); t++) {
    //if (TriadIdCntH[t] < 100) { continue; }
    const int Id = SortIdV[t].Val3;
    int PlusE = 0;
    PSignNet TriadNet = TriadIdV[Id];
    TStr FNm = TStr::Fmt("SignTriad-%s-%02d", OutFNm.CStr(), t+1);
    FILE *F = fopen(TStr(FNm+".dot").CStr(), "wt");
    fprintf(F, "digraph G {\n");
    fprintf(F, "  graph [splines=true, overlap=false]\n  node  [shape=ellipse, width=0.3, height=0.3 label=\"\"]\n");
    for (TEdgeI EI = TriadNet->BegEI(); EI < TriadNet->EndEI(); EI++) {
      fprintf(F, "  n%d -> n%d [label=\"%s\" len=2];\n", EI.GetSrcNId(), EI.GetDstNId(),  EI()==1?"+":"--");
      if (EI() == 1) { PlusE++; }
    }
    const double TriadCnt = TriadIdCntH.GetDat(Id);
    const double TriadProb = TriadNet->GetTriadProb(PlusProb);
    const double UnSignCnt = UnSignCntH.GetDat(SignToUnsignH.GetDat(Id)).Val;
    const double ExpCnt = TriadProb * UnSignCnt; // number of all isorphic nonsigned triads
    const double Surp = (TriadCnt-ExpCnt) / sqrt(UnSignCnt*TriadProb*(1.0-TriadProb));
    fprintf(F, "  label = \"T=%d, E[T]=%d, S=%.1f\";\n}\n", int(TriadCnt), int(ExpCnt), Surp);
    fclose(F);
    TGraphViz::DoLayout(FNm+".dot", FNm+".gif",  gvlNeato);
    fprintf(T, "%s\t%d\t%d\t%.2f\t%f\n", TriadNet->GetTriadStr(true).CStr(), int(TriadCnt), int(ExpCnt), Surp, TriadProb);
  }
  fclose(T);
  printf("\n");
}

void TSignNet::PlotGraphProp(const TStr& OutFNm) const {
  // plus
  PSignNet Net = GetSignSubNet(+1);
  while (Net->GetNodes() < GetNodes()) { Net->AddNode(); }
  TSnap::PlotOutDegDistr(Net, OutFNm+"-plus");
  TSnap::PlotInDegDistr(Net, OutFNm+"-plus");
  TSnap::PlotWccDistr(Net, OutFNm+"-plus");
  TSnap::PlotClustCf(Net, OutFNm+"-plus");
  const int NPlus = Net->GetEdges();
  // rnd plus
  Net = TSnap::GetRndESubGraph(PSignNet((TSignNet*) this), NPlus);
  while (Net->GetNodes() < GetNodes()) { Net->AddNode(); }
  TSnap::PlotOutDegDistr(Net, OutFNm+"-plusRnd");
  TSnap::PlotInDegDistr(Net, OutFNm+"-plusRnd");
  TSnap::PlotWccDistr(Net, OutFNm+"-plusRnd");
  TSnap::PlotClustCf(Net, OutFNm+"-plusRnd");
  // minus
  Net = GetSignSubNet(-1);
  while (Net->GetNodes() < GetNodes()) { Net->AddNode(); }
  TSnap::PlotOutDegDistr(Net, OutFNm+"-minus");
  TSnap::PlotInDegDistr(Net, OutFNm+"-minus");
  TSnap::PlotWccDistr(Net, OutFNm+"-minus");
  TSnap::PlotClustCf(Net, OutFNm+"-minus");
  const int NMinus = Net->GetEdges();
  // rnd minus
  Net = TSnap::GetRndESubGraph(PSignNet((TSignNet*) this), NMinus);
  while (Net->GetNodes() < GetNodes()) { Net->AddNode(); }
  TSnap::PlotOutDegDistr(Net, OutFNm+"-minusRnd");
  TSnap::PlotInDegDistr(Net, OutFNm+"-minusRnd");
  TSnap::PlotWccDistr(Net, OutFNm+"-minusRnd");
  TSnap::PlotClustCf(Net, OutFNm+"-minusRnd");
}

void TSignNet::PlotInOutPlusFrac(const TStr& OutFNm) const {
  THash<TFltPr, TInt> CntH, Cnt2H;
  THash<TFltPr, TInt> FracH, Frac2H;
  const int FracRes = 100;
  /*for (int i = 0; i <= FracRes; i++) {
    for (int j = 0; j <= FracRes; j++) {
      FracH.AddDat(TIntPr(i,j));
  } }*/
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    int PlusIn=1, PlusOut=1, MinusIn=1, MinusOut=1;
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (NI.GetOutEDat(i)==1) { PlusOut++; } else { MinusOut++; } }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      if (NI.GetInEDat(i)==1) { PlusIn++; } else { MinusIn++; } }
    //CntH.AddDat(TIntPr(PlusOut, PlusIn)) += 1;
    CntH.AddDat(TFltPr(PlusOut+TInt::Rnd.GetUniDev(), PlusIn+TInt::Rnd.GetUniDev())) += 1;
    Cnt2H.AddDat(TFltPr(MinusOut+TInt::Rnd.GetUniDev(), MinusIn+TInt::Rnd.GetUniDev())) += 1;
    FracH.AddDat(TFltPr(TInt::Rnd.GetUniDev()+int(FracRes*PlusOut/double(NI.GetOutDeg()+1)), TInt::Rnd.GetUniDev()+int(FracRes*PlusIn/double(NI.GetInDeg()+1)))) += 1;
    Frac2H.AddDat(TFltPr(TInt::Rnd.GetUniDev()+int(FracRes*MinusOut/double(NI.GetOutDeg()+1)), TInt::Rnd.GetUniDev()+int(FracRes*MinusIn/double(NI.GetInDeg()+1)))) += 1;
  }
  TFltPrV V1;  TFltPrV V2;
  CntH.GetKeyV(V1);
  FracH.GetKeyV(V2);
  TGnuPlot::PlotValV(V1, "plusCnt."+OutFNm, OutFNm, "number of pluses out", "number of pluses in", gpsLog10XY, false, gpwPoints);
  TGnuPlot::PlotValV(V2, "plusFrac."+OutFNm, OutFNm, "fraction of pluses out", "fraction of pluses in", gpsAuto, false, gpwPoints);
  Cnt2H.GetKeyV(V1);
  Frac2H.GetKeyV(V2);
  TGnuPlot::PlotValV(V1, "minusCnt."+OutFNm, OutFNm, "number of minuses out", "number of minuses in", gpsLog10XY, false, gpwPoints);
  TGnuPlot::PlotValV(V2, "minusFrac."+OutFNm, OutFNm, "fraction of minuses out", "fraction of minuses in", gpsAuto, false, gpwPoints);
  //{ FILE* F = fopen(TStr::Fmt("plusCnt.%s.txt", OutFNm.CStr()).CStr(), "wt");
  //for (int i = 0; i < CntH.Len(); i++) { fprintf(F, "%d\t%d\t%d\n", CntH.GetKey(i).Val1(), CntH.GetKey(i).Val2(), CntH[i].Val); }
  //fclose(F); }
  /*{ FILE* F = fopen(TStr::Fmt("plusFrac.%s.txt", OutFNm.CStr()).CStr(), "wt");
  FracH.SortByKey();
  //for (int i = 0; i < FracH.Len(); i++) { fprintf(F, "%d\t%d\t%d\n", FracH.GetKey(i).Val1(), FracH.GetKey(i).Val2(), FracH[i].Val); }
  for (int i = 0; i <= FracRes; i++) {
    fprintf(F, "%d", FracH.GetDat(TIntPr(i,0)));
    for (int j = 1; j <= FracRes; j++) {
      fprintf(F, "\t%d", FracH.GetDat(TIntPr(i,j)));
    }
    fprintf(F, "\n");
  }
  fclose(F); }//*/
}

void TSignNet::SavePajek(const TStr& OutFNm) {
  const TStrV ClrV = TStrV::GetV("Gray45", "Green", "Magenta", "Cyan", "OliveGreen", "NavyBlue", "Orange", "Purple", "Brown");
  const int Clrs = ClrV.Len();
  // node colors
  TIntH NodeClr;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { NodeClr.AddKey(NI()); }
  // save
  FILE *F = fopen(OutFNm.CStr(), "wt");
  TIntH NIdToIdH(GetNodes(), true);
  fprintf(F, "*Vertices %d\n", GetNodes());
  int i = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, i++) {
    fprintf(F, "%d  \"%d\" ic %s fos 10\n", i+1, NI.GetId(), ClrV[NodeClr.GetKeyId(NI())%Clrs].CStr());
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  fprintf(F, "*Arcs %d\n", GetEdges()); // arcs are directed, edges are undirected
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int NId = NIdToIdH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      //fprintf(F, "%d %d %g c %s\n", NId, NIdToIdH.GetDat(NI.GetOutNId(e)).Val, 1.0,
      //  EdgeClr.GetDat(NI.GetOutEDat(e)).CStr());
      TStr EdgeColor = "Gray45";
      if (NI()!=-1 && NI.GetOutNDat(e)!=-1) { // both nodes belong to partitions
        if (NI.GetOutEDat(e)==-1) {
          EdgeColor = NI()!=NI.GetOutNDat(e) ? "Blue":"Red"; }  // blue is satisfied edge
        else if (NI.GetOutEDat(e)==1) {
          EdgeColor = NI()==NI.GetOutNDat(e) ? "Blue":"Red"; }  // red is unsatisfied edge
      }
      fprintf(F, "%d %d %g c %s\n", NId, NIdToIdH.GetDat(NI.GetOutNId(e)).Val, 1.0, EdgeColor.CStr());
    }
  }
  fclose(F);
}

void TSignNet::SaveTxt(const TStr& OutFNm) const {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Signed network. %d nodes, %d edges\n", GetNodes(), GetEdges());
  fprintf(F, "# Source\tDestination\tEdgeSign\n");
  THashSet<TInt> NIdSet;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    NIdSet.AddKey(EI.GetSrcNId());  NIdSet.AddKey(EI.GetDstNId());
    fprintf(F, "%d\t%d\t%d\n", NIdSet.GetKeyId(EI.GetSrcNId()), NIdSet.GetKeyId(EI.GetDstNId()), EI());
    //fprintf(F, "%d\t%d\t%d\n", EI.GetSrcNId(), EI.GetDstNId(), EI());
  }
  fclose(F);
}

void TSignNet::DrawGraphViz(const TStr& OutFNm, const TStr& Desc, const bool& NodeLabels, const bool& EdgeLabels, const bool& SaveDir) {
  FILE *F = fopen(TStr(OutFNm+".dot").CStr(), "wt");
  if (! SaveDir) { fprintf(F, "graph G {\n"); }
  else { fprintf(F, "digraph G {\n"); }
  fprintf(F, "  graph [splines=true, overlap=false]\n");
  fprintf(F, "  node  [shape=ellipse, width=0.3, height=0.3 %s]\n", NodeLabels?"":", label=\"\"");
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    fprintf(F, "  %d %s;\n", NI.GetId(), NodeLabels?TStr::Fmt("[label=\"%d : %d\"]", NI.GetId(), NI()).CStr():"");
  }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (! SaveDir) {
        if (! IsEdge(NI.GetOutNId(e), NI.GetId())) {
          fprintf(F, "  %d -- %d [dir=\"forward\"%s];\n", NI.GetId(), NI.GetOutNId(e),
            EdgeLabels?TStr::Fmt(", label=\"%d\"", GetEDat(NI.GetId(), NI.GetOutNId(e))).CStr():"");  //==1?"+":"--"
        } else if (NI.GetId() < NI.GetOutNId(e)) {
          fprintf(F, "  %d -- %d %s;\n", NI.GetId(), NI.GetOutNId(e),
            EdgeLabels?TStr::Fmt("[label=\"%d/%d\"]", GetEDat(NI.GetId(), NI.GetOutNId(e)), //==1?"+":"--"
            GetEDat(NI.GetOutNId(e), NI.GetId())).CStr():"");  // ==1?"+":"--"
        }
      } else {
        fprintf(F, "  %d -> %d %s;\n", NI.GetId(), NI.GetOutNId(e),
          EdgeLabels?TStr::Fmt("[label=\"%d\"]", GetEDat(NI.GetId(), NI.GetOutNId(e))).CStr():"");
      }
    }
  }
  if (! Desc.Empty()) {
    fprintf(F, "  label = \"\\n%s\\n\";\n", Desc.CStr());
  }
  fprintf(F, "  fontsize=24;\n");
  fprintf(F, "}\n");
  fclose(F);
  TGraphViz::DoLayout(OutFNm+".dot", OutFNm+".gif",  gvlNeato);
}

void TSignNet::PrintInfo(const TStr& Desc) const {
  if (! Desc.Empty()) { printf("%s\n", Desc.CStr()); }
  else { printf("Signed network\n"); }
  TIntH SignH;
  int edges = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    SignH.AddDat(EI()) += 1;
    edges++;
  }
  SignH.SortByKey();
  printf("  nodes: %d\n", GetNodes());
  printf("  edges: %d\n", edges);
  for (int i = 0; i < SignH.Len(); i++) {
    printf("  %2d: %d\n", SignH.GetKey(i), SignH[i]);
  }
  //printf("\n  triads: %d\n", TSnap::GetTriads(PSignNet((TSignNet*) this)));
  printf("\n");
}

void TSignNet::Dump() const {
  printf("Net (%d, %d)\n", GetNodes(), GetEdges());
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    printf("  %d\t%d\t%d\n", EI.GetSrcNId(), EI.GetDstNId(), EI());
  }
  printf("\n");
}

bool TSignMicroEvol::IsSameTriad(const PSignNet& Net1, const PSignNet& Net2, const TIntV& Perm) const {
  int i = 0;
  TSignNet::TEdgeI EndEI2 = Net2->EndEI();
  for (TSignNet::TNodeI NI1 = Net1->BegNI(); NI1 < Net1->EndNI(); NI1++, i++) {
    TSignNet::TNodeI NI2 = Net2->GetNI(Perm[i]);
    if (NI1.GetOutDeg() != NI2.GetOutDeg() || NI1.GetInDeg() != NI2.GetInDeg()) {
      return false;
    }
    for (int e = 0; e < NI1.GetOutDeg(); e++) {
      const TSignNet::TEdgeI EI2 = Net2->GetEI(NI2.GetId(), Perm[NI1.GetOutNId(e)]);
      if (EI2 == EndEI2 || NI1.GetOutEDat(e) != EI2()) {
        return false;
      }
    }
  }
  return true;
}

void TSignNet::GetPartOverlap(const TVec<TIntV>& PartNIdV1, const TVec<TIntV>& PartNIdV2) {
  printf("Partition overlap:\n");
  for (int p2 = 0; p2 < PartNIdV2.Len(); p2++) {
    printf("\t%6d(%d)", p2, PartNIdV2[p2].Len());
  }
  printf("\n");
  for (int p1 = 0; p1 < PartNIdV1.Len(); p1++) {
    printf("%6d(%d)\t", p1, PartNIdV1[p1].Len());
    for (int p2 = 0; p2 < PartNIdV2.Len(); p2++) {
      printf("%9d\t", PartNIdV1[p1].IntrsLen(PartNIdV2[p2]));
    }
    printf("\n");
  }
}

PSignNet TSignNet::LoadEpinions(const TStr& FNm) {
  TSsParser Ss(FNm, ssfTabSep);
  PSignNet Net = TSignNet::New();
  while (Ss.Next()) {
    const int src = Ss.GetInt(0);
    const int dst = Ss.GetInt(1);
    if (src == dst) { continue; } // skip self edges
    Net->AddNode(Ss.GetInt(0));
    Net->AddNode(Ss.GetInt(1));
    Net->AddEdge(Ss.GetInt(0), Ss.GetInt(1), Ss.GetInt(2));
  }
  return Net;
}

PSignNet TSignNet::LoadSlashdot(const TStr& InFNm) {
  THashSet<TChA> NIdSet;
  TChA LnStr;
  TVec<char *> WrdV;
  int Sign;
  PSignNet Net = TSignNet::New();
  for (TFIn FIn(InFNm); FIn.GetNextLn(LnStr); ) {
    if (LnStr.Empty() || LnStr[0]=='#') { continue; }
    LnStr.ToLc();
    TStrUtil::SplitOnCh(LnStr, WrdV, '\t', false);
    NIdSet.AddKey(WrdV[0]);
    if (strcmp(WrdV[1], "friends")==0) { Sign = 1; }
    else if (strcmp(WrdV[1], "fans")==0) { continue; } // skip (fans are in-friends)
    else if (strcmp(WrdV[1], "foes")==0) { Sign = -1; } else { Fail; }
    const int SrcNId = NIdSet.AddKey(WrdV[0]);
    if (! Net->IsNode(SrcNId)) {
      Net->AddNode(SrcNId); }
    for (int e = 2; e < WrdV.Len(); e++) {
      const int DstNId = NIdSet.AddKey(WrdV[e]);
      if (SrcNId != DstNId && ! Net->IsEdge(SrcNId, DstNId)) {
        Net->AddNode(DstNId);
        Net->AddEdge(SrcNId, DstNId, Sign);
      }
    }
  }
  TSnap::PrintInfo(Net, InFNm);
  return Net;
}

PSignNet TSignNet::GetSmallNet() {
  PSignNet Net = TSignNet::New();
  for (int i = 0; i < 5; i ++) { Net->AddNode(); }
  Net->AddEdge(0,1, +1); Net->AddEdge(1,2, +1); Net->AddEdge(2,0, -1);  Net->AddEdge(0,2, -1);
  Net->AddEdge(1,3, +1);
  Net->AddEdge(3,4, +1); Net->AddEdge(4,3, +1);
   Net->AddEdge(1,4, +1);
  return Net;
}

/////////////////////////////////////////////////
// Hopfield energy minimization (find sets that satisfy structural balance)

// Start with random assignment of nodes to groups. Pick random node and assign it to best group.
// Keep doing this until convergence
int THopfield::MinEnergy(const int& NPart) {
  TIntV NIdV(Net->GetNodes(), 0);
  // randomly assign partitions
  for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NIdPartH.AddDat(NI.GetId(), TInt::Rnd.GetUniDevInt(NPart));
    NIdV.Add(NI.GetId());
  }
  TIntPrV EnergyV(NPart); // (enery, partition id)
  int Flips = 0;
  for (int iter = 0; iter < 100; iter++) {
    NIdV.Shuffle(TInt::Rnd);
    Flips = 0;
    for (int n = 0; n < NIdV.Len(); n++) {
      TSignNet::TNodeI NI = Net->GetNI(NIdV[n]);
      for (int p = 0; p < NPart; p++) {
        EnergyV[p].Val1=0;  EnergyV[p].Val2=p; }
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int DstPart = NIdPartH.GetDat(NI.GetOutNId(e));
        const int Sign = NI.GetOutEDat(e);
        EnergyV[DstPart].Val1 += Sign;
      }
      EnergyV.Sort(false); // take group with highest score (little - edges, lots + edges)
      const int NewPart = EnergyV[0].Val2;
      if (NIdPartH.GetDat(NI.GetId()) != NewPart) { // swap
        NIdPartH.AddDat(NI.GetId(), NewPart);
        Flips++;
      }
    }
    //printf(" %d", Flips);
  }
  // calculate overall energy
  int OkMns=0, OkPls=0, AllMns=0, AllPls=0;
  for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    const int SrcPart = NIdPartH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int DstPart = NIdPartH.GetDat(NI.GetOutNId(e));
      const int Vote = NI.GetOutEDat(e);
      if (Vote < 0) {
        if (DstPart != SrcPart) { OkMns++; }
        AllMns++; }
      if (Vote > 0) {
        if (DstPart == SrcPart) { OkPls++; }
        AllPls++;
      }
    }
  }
  if (OkPls+OkMns > MxEnergy) {
    MxEnergy = OkPls+OkMns;
    BestPart = NIdPartH;
  }
  return OkPls+OkMns;
}

int THopfield::FindStableSet(const int& NPart, const int& NRuns) {
  NIdPartH.Clr();
  for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NIdPartH.AddDat(NI.GetId(), 0);
  }
  TIntPrV PartSzV(NPart);
  int MxOk=0, SumOk=0;
  printf("Find stable set %d runs\n", NRuns);
  for (int runs = 0; runs < NRuns; runs++) {
    const int OkEdges = MinEnergy(NPart);
    EnergDistr.AddDat(OkEdges) += 1;
    for (int i = 0; i < NPart; i++) {
      PartSzV[i]=TIntPr(0,i); }
    for (int i = 0; i < NIdPartH.Len(); i++) {
      PartSzV[NIdPartH[i]].Val1 += 1; }
    PartSzV.Sort(false); // largest partition is partition 0
    for (int i = 0; i < NIdPartH.Len(); i++) {
      const int PartId = PartSzV[NIdPartH[i]].Val2;
      NIdPartCnt.AddDat(NIdPartH.GetKey(i))[PartId] += 1;
    }
    //printf("  %d:%d", runs, OkEdges);
    SumOk += OkEdges;
    MxOk = TMath::Mx(MxOk, OkEdges);
  }
  printf("  Best: %d   Average: %d\n", MxOk, SumOk/NRuns);
  return MxOk;
}

void THopfield::GetStableSet(const int& Tresh, TVec<TIntV>& PartNIdV) {
  PartNIdV.Gen(NIdPartCnt[0].Len());
  for (int i = 0; i < NIdPartCnt.Len(); i++) {
    const int MxPart = NIdPartCnt[i].FindMx();
    if (NIdPartCnt[i][MxPart] >= Tresh) {
      PartNIdV[MxPart].Add(NIdPartCnt.GetKey(i));
    }
  }
  while (! PartNIdV.Empty() && PartNIdV.Last().Empty()) {
    PartNIdV.DelLast();
  }
  for (int p = 0; p < PartNIdV.Len(); p++) {
    PartNIdV[p].Sort();
  }
  //printf("Stable set sizes:\n");
  //for (int p = 0; p < PartNIdV.Len(); p++) {
  //  printf("  %d\t%d\n", p, PartNIdV[p].Len());
  //}
}

void THopfield::PlotPartStab(const TStr& FNm, TStr Desc) const {
  int runs = 0;
  for (int r = 0; r < EnergDistr.Len(); r++) {
    runs+=EnergDistr[r];
  }
  if (Desc.Empty()) { Desc = FNm; }
  for (int part = 0; part < NIdPartCnt[0].Len(); part++) {
    TIntH PartH;
    for (int n = 0; n < NIdPartCnt.Len(); n++) {
      if (NIdPartCnt[n][part]>0) {
        PartH.AddDat(NIdPartCnt[n][part]) += 1;
      }
    }
    if (PartH.Len() <= 1) { continue; }
    TGnuPlot::PlotValCntH(PartH, TStr::Fmt("partStab-%s-%d", FNm.CStr(), part), TStr::Fmt("%s: Partition %d stability. %d runs. Best energy: %d",
      Desc.CStr(), part, runs, MxEnergy), "Number of times in the partition", "Number of nodes", gpsAuto, false);
  }
  TGnuPlot::PlotValCntH(EnergDistr, "hopfEnerg-"+FNm, TStr::Fmt("%s: Hopfield energy distribution over %d runs. Max: %d", Desc.CStr(), runs, MxEnergy),
    TStr::Fmt("Energy (number of satisfied edges, out of %d", Net->GetEdges()), "Number of solutions");
}

void THopfield::DumpPartStat() const {
  int OkMns=0, OkPls=0, AllMns=0, AllPls=0;
  TIntH OkPlsH, AllPlsH, OkMnsH, AllMnsH;
  int NPart=0;
  for (int i = 0; i < NIdPartH.Len(); i++) {
    NPart = TMath::Mx(NIdPartH[i].Val, NPart);
  }
  for (int p = 0; p < NPart; p++) {
    OkPlsH.AddDat(p,0); AllPlsH.AddDat(p,0);
    OkMnsH.AddDat(p,0); AllMnsH.AddDat(p,0);
  }
  for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    const int SrcPart = NIdPartH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int DstPart = NIdPartH.GetDat(NI.GetOutNId(e));
      const int Vote = NI.GetOutEDat(e);
      if (Vote == -1) {
        if (DstPart != SrcPart) { OkMnsH.AddDat(SrcPart) += 1;  OkMns++; }
        AllMnsH.AddDat(SrcPart) += 1;  AllMns++; }
      if (Vote == +1) {
        if (DstPart == SrcPart) { OkPlsH.AddDat(SrcPart) += 1;  OkPls++; }
        AllPlsH.AddDat(SrcPart) += 1;  AllPls++;
      }
    }
  }
  printf("\nSatisfied edges: + : %5d / %5d  = %f\n", OkPls, AllPls, double(OkPls)/double(AllPls));
  printf(  "                 - : %5d / %5d  = %f\n", OkMns, AllMns, double(OkMns)/double(AllMns));
  TIntH PartCntH;
  for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    PartCntH.AddDat(NIdPartH.GetDat(NI.GetId())) += 1; }
  PartCntH.SortByKey();
  for (int p = 0; p < PartCntH.Len(); p++) {
    printf("  part %2d : %5d (%.4f) nodes: +: %25s    -: %25s\n", p, PartCntH[p], PartCntH[p]/double(NIdPartH.Len()),
      TStr::Fmt("%5d/%d=%.4f", OkPlsH.GetDat(PartCntH.GetKey(p)), AllPlsH.GetDat(PartCntH.GetKey(p)), OkPlsH.GetDat(PartCntH.GetKey(p))/double(AllPlsH.GetDat(PartCntH.GetKey(p)))).CStr(),
      TStr::Fmt("%5d/%d=%.4f", OkMnsH.GetDat(PartCntH.GetKey(p)), AllMnsH.GetDat(PartCntH.GetKey(p)), OkMnsH.GetDat(PartCntH.GetKey(p))/double(AllMnsH.GetDat(PartCntH.GetKey(p)))).CStr() );
  }
}

/////////////////////////////////////////////////
// SignNet-Micro-Evolution
void TSignMicroEvol::BuildPerms() {
  PermH.Clr();
  PermH.AddDat(2).Add(TIntV::GetV(0,1));
  PermH.AddDat(2).Add(TIntV::GetV(1,0));
  PermH.AddDat(3).Add(TIntV::GetV(0,1,2));
  PermH.AddDat(3).Add(TIntV::GetV(0,2,1));
  PermH.AddDat(3).Add(TIntV::GetV(1,0,2));
  PermH.AddDat(3).Add(TIntV::GetV(1,2,0));
  PermH.AddDat(3).Add(TIntV::GetV(2,0,1));
  PermH.AddDat(3).Add(TIntV::GetV(2,1,0));
}

TSignMicroEvol::TSignMicroEvol(PSignNet NetPt) : Network(NetPt) {
  TransNet = TTransitionNet::New();
  BuildPerms();
}

TSignMicroEvol::TSignMicroEvol(TSIn& SIn) {
  TInt Nets(SIn);
  for (int n = 0; n < Nets; n++) {
    TriadIdV.Add(TSignNet::Load(SIn));
  }
  TransNet = TTransitionNet::Load(SIn);
  TInt Diads(SIn);
  for (int d = 0; d < Diads; d++) {
    OpnTriadV.Add(TSignNet::Load(SIn));
  }
  CloseTriadH.Load(SIn);
  Triad3to4H.Load(SIn);
  BuildPerms();
}

void TSignMicroEvol::Save(TSOut& SOut) const {
  SOut.Save(TriadIdV.Len());
  for (int n = 0; n < TriadIdV.Len(); n++) {
    TriadIdV[n]->Save(SOut);
  }
  TransNet->Save(SOut);
  SOut.Save(OpnTriadV.Len());
  for (int d = 0; d < OpnTriadV.Len(); d++) {
    OpnTriadV[d]->Save(SOut);
  }
  CloseTriadH.Save(SOut);
  Triad3to4H.Save(SOut);
}

// get a induced subgraph on nodes N1 and N2
PSignNet TSignMicroEvol::GetSubGraph(const int& N1, const int& N2) {
  PSignNet Net2 = TSignNet::New();
  Net2->AddNode(0);  Net2->AddNode(1);
  if (Network->IsEdge(N1, N2)) { Net2->AddEdge(0, 1, Network->GetEDat(N1, N2)); }
  if (Network->IsEdge(N2, N1)) { Net2->AddEdge(1, 0, Network->GetEDat(N2, N1)); }
  return Net2;
}

// get a induced subgraph on nodes N1, N2, N3: (0,1) is new edge
PSignNet TSignMicroEvol::GetSubGraph(const int& N1, const int& N2, const int& N3) {
  PSignNet Net2 = TSignNet::New();
  Net2->AddNode(0);  Net2->AddNode(1);  Net2->AddNode(2);
  if (Network->IsEdge(N1, N2)) { Net2->AddEdge(0, 1, Network->GetEDat(N1, N2)); }
  if (Network->IsEdge(N2, N1)) { Net2->AddEdge(1, 0, Network->GetEDat(N2, N1)); }
  if (Network->IsEdge(N1, N3)) { Net2->AddEdge(0, 2, Network->GetEDat(N1, N3)); }
  if (Network->IsEdge(N3, N1)) { Net2->AddEdge(2, 0, Network->GetEDat(N3, N1)); }
  if (Network->IsEdge(N2, N3)) { Net2->AddEdge(1, 2, Network->GetEDat(N2, N3)); }
  if (Network->IsEdge(N3, N2)) { Net2->AddEdge(2, 1, Network->GetEDat(N3, N2)); }
  return Net2;
}

// count only triads (transitions) where the edge (SrcNId, DstNId) closes the triad
// SrcOutPlusProb : background prob. of Src giving a plus sign
// DstInPlusProb : background prob. of Dst receiving a plus sign
void TSignMicroEvol::CountTriadClose(const int& SrcNId, const int& DstNId, const int& Sign, const double& SrcOutPlusProb,
                             const double& DstInPlusProb, const bool& OnlySimpleTriads) {
  IAssert(! Network->IsEdge(SrcNId, DstNId));
  // edge that closes an open triad
  if (! Network->IsEdge(DstNId, SrcNId)) {
    TIntV NbhV;
    TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV);
    for (int n = 0; n < NbhV.Len(); n++) {
      PSignNet Net1 = GetSubGraph(SrcNId, DstNId, NbhV[n]);
      PSignNet Net2 = GetSubGraph(SrcNId, DstNId, NbhV[n]);
      Net2->AddEdge(0, 1, Sign); // new edge is between nodes 0 and 1
      // triad transition
      const int Net1Id = GetTriadId(Net1);
      const int Net2Id = GetTriadId(Net2);
      if (! TransNet->IsNode(Net1Id)) { TransNet->AddNode(Net1Id, 0); }
      if (! TransNet->IsNode(Net2Id)) { TransNet->AddNode(Net2Id, 0); }
      if (! TransNet->IsEdge(Net1Id, Net2Id)) { TransNet->AddEdge(Net1Id, Net2Id); }
      TransNet->GetNDat(Net1Id) += 1;
      TransNet->GetNDat(Net2Id) += 1;
      { TTransStat& Stat = TransNet->GetEDat(Net1Id, Net2Id);
      if (Sign == +1) { Stat.SumWgt += SrcOutPlusProb; }
      else { Stat.SumWgt+= 1-SrcOutPlusProb; }
      Stat.Cnt++;  Stat.VarSq += SrcOutPlusProb*(1-SrcOutPlusProb); }
      // open to closed triad: triad closing edge is (0,1)
      { const int TrId = GetOpnTriadId(Net1);
      TCloseStat& ClsStat = CloseTriadH.AddDat(TrId);
      if (Sign == +1) { ClsStat.PlsCnt++; }
      else { ClsStat.MnsCnt++; }
      ClsStat.OutSumWgt += SrcOutPlusProb;
      ClsStat.OutVarSq += SrcOutPlusProb*(1-SrcOutPlusProb);
      ClsStat.InSumWgt += DstInPlusProb;
      ClsStat.InVarSq += DstInPlusProb*(1-DstInPlusProb);
      ClsStat.OutPlusPH.AddDat(int(SrcOutPlusProb*50)*2) += 1;
      ClsStat.InPlusPH.AddDat(int(DstInPlusProb*50)*2) += 1; }
    }
  } else { // reciprocal link
    RepLinkCntH.AddDat(TIntPr(Network->GetEDat(DstNId, SrcNId), Sign)).Val1 += 1;
    RepLinkCntH.AddDat(TIntPr(Network->GetEDat(DstNId, SrcNId), Sign)).Val2 += SrcOutPlusProb;
  }
  if (OnlySimpleTriads && Network->IsEdge(DstNId, SrcNId)) {
    return; }// edge in the opposite direction already exists
  else {
    Network->AddNode(SrcNId);
    Network->AddNode(DstNId);
    Network->AddEdge(SrcNId, DstNId, Sign);
  }
}

// given a simple directed triad, count how is reciprocal edge added
// do reciprocal edges make triads more balanced
void TSignMicroEvol::CountTriad3to4Edges(const int& SrcNId, const int& DstNId, const int& Sign, const double& SrcOutPlusProb, const double& DstInPlusProb) {
  IAssert(! Network->IsEdge(SrcNId, DstNId));
  TIntV NbhV;
  TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV);
  for (int n = 0; n < NbhV.Len(); n++) {
    PSignNet Net1 = GetSubGraph(SrcNId, DstNId, NbhV[n]);
    if (! (Net1->GetEdges()==3 && Net1->IsClosedTriad())) { continue; }
    PSignNet Net2 = GetSubGraph(SrcNId, DstNId, NbhV[n]);
    Net2->AddEdge(0, 1, +1); // pretend new edge is +
    const int Net1Id = GetTriadId(Net1);
    const int Net2Id = GetTriadId(Net2);
    TCloseStat& ClsStat = Triad3to4H.AddDat(TIntPr(Net1Id, Net2Id));
    if (Sign == +1) { ClsStat.PlsCnt++; }
    else { ClsStat.MnsCnt++; }
    ClsStat.OutSumWgt += SrcOutPlusProb;
    ClsStat.OutVarSq += SrcOutPlusProb*(1-SrcOutPlusProb);
    ClsStat.InSumWgt += DstInPlusProb;
    ClsStat.InVarSq += DstInPlusProb*(1-DstInPlusProb);
    ClsStat.OutPlusPH.AddDat(int(SrcOutPlusProb*50)*2) += 1;
    ClsStat.InPlusPH.AddDat(int(DstInPlusProb*50)*2) += 1;
  }
  Network->AddNode(SrcNId);
  Network->AddNode(DstNId);
  Network->AddEdge(SrcNId, DstNId, Sign);
}

void TSignMicroEvol::CountTriadCloseVec(const int& SrcNId, const int& DstNId, const int& Sign) {
  TIntV NbhV;
  TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV);
  TTuple<TInt, 16> CntV;
  for (int n = 0; n < NbhV.Len(); n++) {
    PSignNet OpnTriadNet = GetSubGraph(SrcNId, DstNId, NbhV[n]);
    if (OpnTriadNet->GetEdges() != 2) { continue; }
    const int TdId = GetOpnTriadId(OpnTriadNet);
    IAssert(TdId < 16);
    CntV[TdId]++;
  }
  TIntPr& Cnt = TriadVecV.AddDat(CntV);
  if (Sign==+1) { Cnt.Val1 += 1; }
  else { Cnt.Val2 += 1; }
  IAssert(! Network->IsEdge(SrcNId, DstNId));
  Network->AddNode(SrcNId);  Network->AddNode(DstNId);
  Network->AddEdge(SrcNId, DstNId, Sign);
}

int TSignMicroEvol::IsSameOpnTriad(const PSignNet& Net1, const PSignNet& Net2) {
  if (Net1->GetNodes() != Net2->GetNodes() || Net1->GetEdges() != Net2->GetEdges()) {
    return false;
  }
  TSignNet::TEdgeI EI1 = Net1->BegEI();
  TSignNet::TEdgeI EI2 = Net2->BegEI();
  while (EI1 < Net1->EndEI()) {
    if (EI1.GetSrcNId() != EI2.GetSrcNId()) { return false; }
    if (EI1.GetDstNId() != EI2.GetDstNId()) { return false; }
    if (EI1() != EI2()) { return false; }
    EI1++;  EI2++;
  }
  return true;
}

// no isomorphism testing
int TSignMicroEvol::GetOpnTriadId(const PSignNet& Net1) {
  for (int i = 0; i < OpnTriadV.Len(); i++) {
    if (IsSameOpnTriad(OpnTriadV[i], Net1)) { return i; }
  }
  OpnTriadV.Add(Net1);
  return OpnTriadV.Len()-1;
}

bool TSignMicroEvol::IsSameTriad(const PSignNet& Net1, const PSignNet& Net2, int& PermId) const {
  if (Net1->GetNodes() != Net2->GetNodes() || Net1->GetEdges() != Net2->GetEdges()) {
    return false; }
  const TVec<TIntV>& PermV = PermH.GetDat(Net1->GetNodes());
  for (int p = 0; p < PermV.Len(); p++) {
    if (IsSameTriad(Net1, Net2, PermV[p])) {
      PermId=p; return true;
    }
  }
  return false;
}

int TSignMicroEvol::GetTriadId(const PSignNet& Net1) {
  int PermId;
  for (int i = 0; i < TriadIdV.Len(); i++) {
    if (IsSameTriad(TriadIdV[i], Net1, PermId)) { TriadIdV[i]=Net1; return i; }
  }
  TriadIdV.Add(Net1);
  TransNet->AddNode(TriadIdV.Len()-1, 0);
  return TriadIdV.Len()-1;
}

double TSignMicroEvol::GetStatusDelta(const PSignNet& Net, const int& NId1, const int& NId2) {
  int S1, S2;
  if (! Net->IsEdge(NId1, NId2)) { S1 = 0; }
  else if (Net->GetEDat(NId1, NId2)>0) { S1 = 1; } else { S1 = -1; }
  if (! Net->IsEdge(NId2, NId1)) { S2 = 0; }
  else if (Net->GetEDat(NId2, NId1)>0) { S2 = 1; } else { S2 = -1; }
  if (S1==+1 && S2==0) { return +0.2; }
  if (S1==-1 && S2==0) { return -0.8; }
  if (S1==0 && S2==+1) { return -0.2; }
  if (S1==0 && S2==-1) { return +0.8; }
  if (S1==+1 && S2==+1) { return +0.4; }
  if (S1==-1 && S2==-1) { return -1.6; }
  if (S1==+1 && S2==-1) { return +1; }
  if (S1==-1 && S2==+1) { return -1; }
  Fail;  return -1;
}

double TSignMicroEvol::GetStatus(const PSignNet& Net, const int& NId) {
  if (NId == 2) { return 0; }
  return GetStatusDelta(Net, 2, NId);
}

bool TSignMicroEvol::IsStatusFrac(const int& OpnTriadId, const bool& DstNode) const {
  const TCloseStat Stat = CloseTriadH.GetDat(OpnTriadId); // edge that closes the triad is (0,1)
  PSignNet SrcNet = OpnTriadV[OpnTriadId];
  //const double Sts0 = GetStatus(SrcNet, 0); // 2 is the middle node
  //const double Sts1 = GetStatus(SrcNet, 1);
  //const double Plus = Stat.GetPlsFrac();
  //if (Sts1 < 0 && Plus < 0.8) { return true; }
  //if (Sts1 > 0 && Plus >=0.8) { return true; }
  if (DstNode) {
    if (GetStatus(SrcNet, 1) > 0) { if (Stat.GetPlsFrac()>0.8) { return true; } }
    else { if(Stat.GetPlsFrac()<0.2) { return true; } }
  } else {
    if (GetStatus(SrcNet, 1) > 0) { if (Stat.GetPlsFrac()<0.2) { return true; } }
    else { if(Stat.GetPlsFrac()>0.8) { return true; } }
  }
  return false;
}

bool TSignMicroEvol::IsStatusSurp(const int& OpnTriadId, const bool& DstNode) const {
  const TCloseStat Stat = CloseTriadH.GetDat(OpnTriadId); // edge that closes the triad is (0,1)
  PSignNet SrcNet = OpnTriadV[OpnTriadId];
  if (DstNode) {
    if (GetStatus(SrcNet, 1)*Stat.GetOutPlsSurp() > 0) { return true; } // same sign
  } else {
    if (GetStatus(SrcNet, 0)*Stat.GetInPlsSurp() < 0) { return true; }
  }
  return false;
}

int GetUnDirEdgeSign(const PSignNet& Net, const int& NId1, const int& NId2) {
  int S1, S2, S;
  if (! Net->IsEdge(NId1, NId2)) { S1 = 0; }
  else if (Net->GetEDat(NId1, NId2)>0) { S1 = 1; } else { S1 = -1; }
  if (! Net->IsEdge(NId2, NId1)) { S2 = 0; }
  else if (Net->GetEDat(NId2, NId1)>0) { S2 = 1; } else { S2 = -1; }
  if (S1==0) { S=S2; }
  else if (S2==0) { S=S1; }
  else { S=TMath::Mx(S1,S2); }
  return S;
}

bool TSignMicroEvol::IsBalanceFrac(const int& OpnTriadId) const {
  const TCloseStat Stat = CloseTriadH.GetDat(OpnTriadId); // edge that closes the triad is (0,1)
  PSignNet SrcNet = OpnTriadV[OpnTriadId];
  const int S1 = GetUnDirEdgeSign(SrcNet, 0,2);
  const int S2 = GetUnDirEdgeSign(SrcNet, 1,2);
  const double Plus = Stat.GetPlsFrac();
  if (S1*S2 > 0 && Plus > 0.8) { return true; }
  if (S1*S2 < 0 && Plus < 0.8) { return true; }
  return false;
}

bool TSignMicroEvol::IsBalanceSurp(const int& OpnTriadId) const {
 const TCloseStat Stat = CloseTriadH.GetDat(OpnTriadId); // edge that closes the triad is (0,1)
  PSignNet SrcNet = OpnTriadV[OpnTriadId];
  const int S1 = GetUnDirEdgeSign(SrcNet, 0,2);
  const int S2 = GetUnDirEdgeSign(SrcNet, 1,2);
  const double Surp = Stat.GetOutPlsSurp();
  if (S1*S2 > 0 && Surp > 0) { return true; }
  if (S1*S2 < 0 && Surp < 0) { return true; }
  return false;
}

void TSignMicroEvol::PrintInfo() const {
  TSnap::PrintInfo(Network);
  Network->PrintInfo();
  THash<TIntPr, TFltPr> H = RepLinkCntH;
  H.SortByDat(false);
  printf("*** RECIPROCAL Links\n");
  for (int i = 0; i < H.Len(); i++) {
    printf(" %c is reciprocated by %c :  cnt %g  surp: %f\n",
      H.GetKey(i).Val1==1?'+':'-', H.GetKey(i).Val2==1?'+':'-', H[i].Val1, H[i].Val2);
  }
}

char TSignMicroEvol::GetEdgeChar(const PSignNet& Network, const int& n1, const int& n2) {
  if (! Network->IsEdge(n1,n2)) { return 'o'; }
  else if (Network->GetEDat(n1,n2)>0) { return '+'; }
  else { return '-'; }
}

// save open to closed triad evolution
void TSignMicroEvol::SaveCloseTriadTxt(const TStr& FNmPref, const bool& DrawNets, const bool& PlotHists) {
  printf("*** OPEN TRIADS %d\n", OpnTriadV.Len());
  TVec<TTriple<TInt, TStr, TInt> > OpnIdV;
  for (int n = 0; n < OpnTriadV.Len(); n++) {
    PSignNet SrcNet = OpnTriadV[n];
    OpnIdV.Add(TTriple<TInt, TStr, TInt>(SrcNet->GetEdges(), TStr::Fmt("%c%c%c%c", GetEdgeChar(SrcNet, 0,2), GetEdgeChar(SrcNet, 2,0), GetEdgeChar(SrcNet, 2,1), GetEdgeChar(SrcNet, 1,2)), n));
  }
  OpnIdV.Sort();
  FILE* F = fopen(TStr::Fmt("%s-diad.tab", FNmPref.CStr()).CStr(), "wt");
  fprintf(F, "AB\tBC\tAC+\tAC-\tPctAC+\tOUT-Surprise\tIN-Surprise\tDst-StsFrac\tDst-StsSurp\tSrc-StsFrac\tSrc-StsSurp\tBalFrac\tBalSurp\n");
  for (int n = 0; n < OpnIdV.Len(); n++) {
    const int Id = OpnIdV[n].Val3;
    PSignNet SrcNet = OpnTriadV[Id];
    const TCloseStat Stat = CloseTriadH.GetDat(Id); // edge that closes the triad is (0,1)
    fprintf(F, "%c%c\t%c%c", GetEdgeChar(SrcNet, 0,2), GetEdgeChar(SrcNet, 2,0), GetEdgeChar(SrcNet, 2,1), GetEdgeChar(SrcNet, 1,2));
    fprintf(F, "\t%d\t%d\t%.4f\t%.4f\t%.4f\t%s\t%s\t%s\t%s\t%s\t%s\n", Stat.PlsCnt, Stat.MnsCnt, Stat.GetPlsFrac(), Stat.GetOutPlsSurp(), Stat.GetInPlsSurp(),
      IsStatusFrac(Id,true)?"1":"0", IsStatusSurp(Id,true)?"1":"0", IsStatusFrac(Id,false)?"1":"0", IsStatusSurp(Id,false)?"1":"0",
      IsBalanceFrac(Id)?"1":"0", IsBalanceSurp(Id)?"1":"0");
  }
  fclose(F);
  if (DrawNets) {
    for (int i = 0; i < 16; i++) {
      const int Id = OpnIdV[i].Val3;
      TStr Label = TStr::Fmt("dst-status: p+:%s  s+:%s\\nsrc-status: p+:%s  s+:%s\\nbalance: p+:%s s+:%s",
        IsStatusFrac(Id,true)?"Y":"N", IsStatusSurp(Id,true)?"Y":"N",
        IsStatusFrac(Id,false)?"Y":"N", IsStatusSurp(Id,false)?"Y":"N",
        IsBalanceFrac(Id)?"Y":"N", IsBalanceSurp(Id)?"Y":"N");
      DrawCloseTriad(TStr::Fmt("%s_%02d", FNmPref.CStr(), i), Id, Label);
    }
  }
  if (PlotHists) {
    for (int i = 0; i < 16; i++) {
      const int Id = OpnIdV[i].Val3;
      CloseTriadH.GetDat(Id).SaveHist(TStr::Fmt("%s_%02d", FNmPref.CStr(), i));
    }
  }
  printf("done.\n");
}

// draw nets
void TSignMicroEvol::DrawCloseTriad(const TStr& FNmPref, const int& OpnTriadId, const TStr& Label) {
  PSignNet Network = OpnTriadV[OpnTriadId];
  TStr FNm = TStr::Fmt("%s-OpnTriad.dot", FNmPref.CStr());
  FILE *F = fopen(FNm.CStr(), "wt");
  fprintf(F, "  digraph G {\n");
  fprintf(F, "    graph [splines=true, overlap=false];\n");
  fprintf(F, "    node  [shape=ellipse, width=0.3, height=0.3];\n");
  fprintf(F, "    edge  [len=4];\n");
  for (TSignNet::TNodeI NI = Network->BegNI(); NI < Network->EndNI(); NI++) {
    fprintf(F, "    node%d [label=\"%g\"];\n", NI.GetId(), GetStatus(Network, NI.GetId())); }
  for (TSignNet::TEdgeI EI = Network->BegEI(); EI < Network->EndEI(); EI++) {
    fprintf(F, "    node%d -> node%d [label=\"%s\"];\n",
      EI.GetSrcNId(), EI.GetDstNId(), EI()==1?"+":"--"); }
  // triad closing edge
  const TCloseStat Stat = CloseTriadH.GetDat(OpnTriadId);
  fprintf(F, "    node0 -> node1 [label=\"+:%d -:%d\\np+:%.3f  so: %.1f  si: %.1f\", color=\"red\"];\n",
    Stat.PlsCnt(), Stat.MnsCnt(), Stat.GetPlsFrac(), Stat.GetOutPlsSurp(), Stat.GetInPlsSurp());
  if (! Label.Empty()) { fprintf(F, "label = \"%s\";\n", Label.CStr()); }
  fprintf(F, "}\n");
  fclose(F);
  TGraphViz::DoLayout(FNm, FNm.GetFMid()+".gif", gvlNeato);
}

bool IsBalancedSwapEdge(const PSignNet& Net, const TIntPr& BackEdge, const int& Sign) {
  const int s1 = Net->GetEDat(BackEdge.Val2, BackEdge.Val1);
  const int s2 = Net->GetEDat(BackEdge.Val1, BackEdge.Val2);
  Net->DelEdge(BackEdge.Val2, BackEdge.Val1);
  Net->GetEDat(BackEdge.Val1, BackEdge.Val2) = Sign;
  const bool IsBal = Net->IsBalanced();
  Net->AddEdge(BackEdge.Val2, BackEdge.Val1, s1);
  Net->GetEDat(BackEdge.Val1, BackEdge.Val2) = s2;
  return IsBal;
}

void TSignMicroEvol::SaveTriad3to4Edges(const TStr& FNmPref) {
  TVec<TPair<TIntTr, TInt> > SigTIdV;
  for (int t = 0; t < Triad3to4H.Len(); t++) {
    SigTIdV.Add(TPair<TIntTr, TInt>(TriadIdV[Triad3to4H.GetKey(t).Val1]->GetTriadSig(), t));
  }
  SigTIdV.Sort();
  FILE *F = fopen(TStr::Fmt("tr3to4-%s2.tab", FNmPref.CStr()).CStr(), "wt");
  fprintf(F, "Balanced\tSrcTriad\t\t\t\tDstTriad\t\t\tPlus-BAL\tMns-BAL\tPlusCnt\tPlusFrac\tOUT-Supr\tIN-Surp\tBal-Frac\tBal-OutSurp\tBal-InSurp\n");
  for (int t = 0; t < SigTIdV.Len(); t++) {
    const int SrcTId = Triad3to4H.GetKey(SigTIdV[t].Val2).Val1;
    const int DstTId = Triad3to4H.GetKey(SigTIdV[t].Val2).Val2;
    const TCloseStat& Stat = Triad3to4H[SigTIdV[t].Val2];
    TIntTr SrcSig = TriadIdV[SrcTId]->GetTriadSig();
    TIntTr DstSig = TriadIdV[DstTId]->GetTriadSig();
    TIntPr NewEdge = TriadIdV[DstTId]->GetNewEdge(TriadIdV[SrcTId]);
    const bool SrcBal = TriadIdV[SrcTId]->IsBalanced();
    const bool DstPlsBal = IsBalancedSwapEdge(TriadIdV[DstTId], NewEdge, 1);
    const bool DstMnsBal = IsBalancedSwapEdge(TriadIdV[DstTId], NewEdge, -1);
    IAssert(int(DstPlsBal)+int(DstMnsBal)==1);
    fprintf(F, "%d\t%s\t->\t%s\t%d\t%d\t%d\t%.3f\t%.3f\t%.3f",
      SrcBal?1:0, TSignNet::GetTriadStr(SrcSig).CStr(),
      TSignNet::GetTriadStr(DstSig).CStr(), DstPlsBal?1:0, DstMnsBal?1:0,
      Stat.PlsCnt, Stat.GetPlsFrac(), Stat.GetOutPlsSurp(), Stat.GetInPlsSurp());
    const bool IsPlusBal = (DstPlsBal && Stat.GetPlsFrac()>0.8) || (! DstPlsBal && Stat.GetPlsFrac()<0.8);
    const bool IsOutSur = (DstPlsBal && Stat.GetOutPlsSurp()>2) || (! DstPlsBal && Stat.GetOutPlsSurp()<-2);
    const bool IsInSur = (DstPlsBal && Stat.GetInPlsSurp()>2) || (! DstPlsBal && Stat.GetInPlsSurp()<-2);
    fprintf(F, "\t%d\t%d\t%d\n", IsPlusBal?1:0, IsOutSur?1:0, IsInSur?1:0);
  }
  fclose(F);
}

void TSignMicroEvol::SaveTriadCloseVec(const TStr& FNmPref) {
  IAssert(OpnTriadV.Len() == 16);
  TVec<TTriple<TInt, TStr, TInt> > OpnIdV;
  for (int n = 0; n < OpnTriadV.Len(); n++) {
    PSignNet SrcNet = OpnTriadV[n];
    OpnIdV.Add(TTriple<TInt, TStr, TInt>(SrcNet->GetEdges(), TStr::Fmt("%c%c%c%c", GetEdgeChar(SrcNet, 0,2), GetEdgeChar(SrcNet, 2,0), GetEdgeChar(SrcNet, 2,1), GetEdgeChar(SrcNet, 1,2)), n));
  }
  OpnIdV.Sort();
  //TriadVecV.SortByDat(false);
  TriadVecV.SortByKey(false);
  FILE *F = fopen(TStr::Fmt("trVec2-%s.tab", FNmPref.CStr()).CStr(), "wt");
  for (int i = 0; i < OpnIdV.Len(); i++) { fprintf(F, "%s\t", OpnIdV[i].Val2.CStr()); }
  fprintf(F, "PlsCnt\tMnsCnt\n");
  printf("%d vectors\n", TriadVecV.Len());
  for (int v = 0; v < TMath::Mn(1000, TriadVecV.Len()); v++) {
    const TTuple<TInt, 16>& V = TriadVecV.GetKey(v);
    for (int i = 0; i < V.Len(); i++) {
      fprintf(F, "%d\t", V[OpnIdV[i].Val3]);
    }
    fprintf(F, "%d\t%d\n", TriadVecV[v].Val1, TriadVecV[v].Val2);
  }
  fclose(F);
}

/*
void TSignMicroEvol::SaveAll(const TStr& FNmPref) const {
  printf("*** %d nets\n", TriadIdV.Len());
  //for (int i = 0; i < TriadIdV.Len(); i++) { TriadIdV[i]->DrawGraphViz(TStr::Fmt("%s-%03d", FNmPref.CStr(), i), false, true); }
  /*TransNet->DrawGraphViz(FNmPref+"-Trans", FNmPref, true, true);
  PSignNet Trans5 = TransNet->GetSubNet(5);
  Trans5->DrawGraphViz(FNmPref+"-Trans5", FNmPref, true, true);
  printf("saving %d nets\n", Trans5->GetNodes());
  for (TSignNet::TNodeI NI = Trans5->BegNI(); NI < Trans5->EndNI(); NI++) {
    TriadIdV[NI.GetId()]->DrawGraphViz(TStr::Fmt("%s-%03d", FNmPref.CStr(), NI.GetId()),
      TStr::Fmt("%d : %d", NI.GetId(), NI()), false, true);
  }*/
/*}

// save connected components
void TSignMicroEvol::SaveAll2(const TStr& FNmPref) const {
  // save transition graph and all little graphs
  //PSignNet Trans5 = TransNet->GetSubNet(5);
  //TransNet->DrawGraphViz(FNmPref+"-Trans5", FNmPref, true, true);
  /*for (TSignNet::TNodeI NI = Trans5->BegNI(); NI < Trans5->EndNI(); NI++) {
    TriadIdV[NI.GetId()]->DrawGraphViz(TStr::Fmt("%s-%03d", FNmPref.CStr(), NI.GetId()),
      TStr::Fmt("%d : %d", NI.GetId(), NI()), false, true);
  }
  // full graph
  FILE *F = fopen(TStr::Fmt("%s-Trans5.dot", FNmPref.CStr()).CStr(), "wt");
  fprintf(F, "graph G {\n");
  fprintf(F, "  graph [splines=false overlap=false];\n");
  for (TSignNet::TNodeI NI = Trans5->BegNI(); NI < Trans5->EndNI(); NI++) {
    SaveSubGraph(F, TriadIdV[NI.GetId()], TStr::Fmt("cluster%d", NI.GetId()), TStr::Fmt("node%d", NI.GetId()), false);
  }
  for (TSignNet::TEdgeI EI = Trans5->BegEI(); EI < Trans5->EndEI(); EI++) {
    fprintf(F, "  cluster%d -- cluster%d [dir=\"forward\", label=\"%d\"];\n", EI.GetSrcNId(), EI.GetDstNId(), EI());
  }
  fprintf(F, "}\n");
  fclose(F);
  // connected components
  TCnComV CnComV;  TCnCom::GetWccs(Trans5, CnComV);
  for (int c = 0; c < CnComV.Len(); c++) {
    PSignNet SubNet = TSnap::GetSubGraph(Trans5, CnComV[c]());
    FILE *F = fopen(TStr::Fmt("%s-Trans5_%02d.dot", FNmPref.CStr(), c+1).CStr(), "wt");
    fprintf(F, "graph G {\n");
    fprintf(F, "  graph [splines=false overlap=false];\n");
    for (TSignNet::TNodeI NI = SubNet->BegNI(); NI < SubNet->EndNI(); NI++) {
      SaveSubGraph(F, TriadIdV[NI.GetId()], TStr::Fmt("cluster%d", NI.GetId()), TStr::Fmt("node%d", NI.GetId()), false);
    }
    for (TSignNet::TEdgeI EI = SubNet->BegEI(); EI < SubNet->EndEI(); EI++) {
      fprintf(F, "  cluster%d -- cluster%d [dir=\"forward\", label=\"%d\"];\n", EI.GetSrcNId(), EI.GetDstNId(), EI());
    }
    fprintf(F, "}\n");
    fclose(F);
  }*/
/*}

// save diad and all the transitioned triads, each source triad is a separate graph
void TSignMicroEvol::SaveAll3(const TStr& FNmPref) const {
  int c = 0;
  for (TTransNet::TNodeI NI = TransNet->BegNI(); NI < TransNet->EndNI(); NI++) {
    if (NI.GetOutDeg() > 0) {
      double SumWgt = 0;
      TIntV NIdV;  NIdV.Add(NI.GetId());
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        NIdV.Add(NI.GetOutNId(e));
        SumWgt += NI.GetOutEDat(e).Val1;
      }
      PTransNet SubNet = TSnap::GetSubGraph(TransNet, NIdV);
      FILE *F = fopen(TStr::Fmt("%s-Trans_%02d.dot", FNmPref.CStr(), ++c).CStr(), "wt");
      fprintf(F, "digraph G {\n");
      fprintf(F, "  graph [splines=false overlap=false];\n");
      for (TTransNet::TNodeI NI = SubNet->BegNI(); NI < SubNet->EndNI(); NI++) {
        SaveSubGraph(F, TriadIdV[NI.GetId()], TStr::Fmt("cluster%d", NI.GetId()), TStr::Fmt("node%d", NI.GetId()), true);
      }
      for (TTransNet::TEdgeI EI = SubNet->BegEI(); EI < SubNet->EndEI(); EI++) {
        fprintf(F, "  cluster%d -> cluster%d [label=\"%g  %g\\n%.2f\", len=2];\n",
          EI.GetSrcNId(), EI.GetDstNId(), EI().Val1(), EI().Val2(), EI().Val1/SumWgt);
      }
      fprintf(F, "}\n");
      fclose(F);
    }
  }
}

// Net2 has one more edge than Net1, return the endpoints (node ids if Net1) of the edge
void TSignMicroEvol::GetEdgeDiff(const PSignNet& Net1, const PSignNet& Net2, int& SrcNId, int& DstNId, int& Sign) {
  for (TSignNet::TEdgeI EI = Net2->BegEI(); EI < Net2->EndEI(); EI++) {
    PSignNet Net = TSignNet::New(); *Net = *Net2;
    SrcNId=EI.GetSrcNId();
    DstNId=EI.GetDstNId();
    Net->DelEdge(SrcNId, DstNId);
    int PermId;
    if (IsSameNet(Net, Net1, PermId)) {
      const TVec<TIntV>& PermV = PermH.GetDat(Net2->GetNodes());
      SrcNId = PermV[PermId][SrcNId];
      DstNId = PermV[PermId][DstNId];
      Sign = EI();
      if(SrcNId+DstNId!=1) {
        printf("orig\n"); Net1->Dump();
        printf("net \n"); Net2->Dump();
        printf("edge %d  %d\n", SrcNId, DstNId);
      }
      return;
    }
  }
  SrcNId = -1;  DstNId = -1;
  Fail;
}

// save how triad closes (impose all transitions of a single triad on a single graph)
// new edge is always edge 0,1
void TSignMicroEvol::SaveAll4(const TStr& FNmPref) {
  typedef TQuad<TInt, TInt, TInt, TFltPr> TIntFltQu;
  int c = 0;
  for (TTransNet::TNodeI NI = TransNet->BegNI(); NI < TransNet->EndNI(); NI++) {
    if (NI.GetOutDeg() == 0) { continue; }
    PSignNet Net = TSignNet::New();
    *Net = *TriadIdV[NI.GetId()];
    // add transitions
    int src, dst, sign;
    TVec<TIntFltQu> SrcDstV;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      //printf("%d --> %d\n", NI.GetId(), NI.GetOutNId(e));
      if (NI.GetId() == NI.GetOutNId(e)) { continue; }
      GetEdgeDiff(Net, TriadIdV[NI.GetOutNId(e)], src, dst, sign);
      SrcDstV.Add(TIntFltQu(src, dst, sign, NI.GetOutEDat(e)));
    }
    SrcDstV.Sort();
    // save
    TStr FNm = TStr::Fmt("%s-Trans2_%02d.dot", FNmPref.CStr(), ++c);
    FILE *F = fopen(FNm.CStr(), "wt");
    fprintf(F, "  digraph G {\n");
    fprintf(F, "    graph [splines=true, overlap=false];\n");
    fprintf(F, "    node  [shape=ellipse, width=0.3, height=0.3, label=\"\"];\n");
    fprintf(F, "    edge  [len=4];\n");
    for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
      fprintf(F, "    node%d;\n", NI.GetId()); }
    for (TSignNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
      fprintf(F, "    node%d -> node%d [label=\"%s\"];\n",
        EI.GetSrcNId(), EI.GetDstNId(), EI()==1?"+":"--"); }
    // new edges
    for (int e = 0; e < SrcDstV.Len(); e+=2) {
      const double cnt1 = SrcDstV[e].Val4.Val1;   // minus
      const double cnt2 = SrcDstV[e+1].Val4.Val1; // plus
      const double wgt1 = SrcDstV[e].Val4.Val2;   // minus
      const double wgt2 = SrcDstV[e+1].Val4.Val2; // plus
      const double Cnt = TMath::Mx(cnt1, cnt2);
      const double Frac = Cnt/double(cnt1+cnt2);
      const double Surprise = wgt2-wgt1;
      const double SurpriseF = Surprise/(cnt1+cnt2);
      const TStr Label = cnt2 > cnt1 ? "+" : "--";
      fprintf(F, "    node%d -> node%d [label=\"%s  %g %.2f s:%.4f\", color=\"red\"];\n",
        SrcDstV[e].Val1, SrcDstV[e].Val2, Label.CStr(), Cnt, Frac, SurpriseF);
    }
    fprintf(F, "  }\n");
    fclose(F);
    TGraphViz::DoLayout(FNm, FNm.GetFMid()+".gif", gvlNeato);
  }
}

void TSignMicroEvol::SaveSubGraph(FILE *F, const PSignNet& Net, const TStr& SubGraphNm, const TStr& NodePref, const bool& SaveDir) {
  fprintf(F, "  subgraph %s {\n", SubGraphNm.CStr());
  fprintf(F, "    graph [splines=true, overlap=false];\n");
  fprintf(F, "    node  [shape=ellipse, width=0.2, height=0.2, label=\"\"];\n");
  for (TSignNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    fprintf(F, "    %s_%d;\n", NodePref.CStr(), NI.GetId());
  }
  for (TSignNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    if (! SaveDir) { // save reciprocal edges as unidirected edges
      if (! Net->IsEdge(EI.GetDstNId(), EI.GetSrcNId())) {
        fprintf(F, "    %s_%d -- %s_%d [dir=\"forward\", label=\"%s\"];\n", NodePref.CStr(), EI.GetSrcNId(),
          NodePref.CStr(), EI.GetDstNId(), EI()==1?"+":"--");
      } else if (EI.GetSrcNId() < EI.GetDstNId()) {
        fprintf(F, "    %s_%d -- %s_%d [label=\"%s / %s\"];\n", NodePref.CStr(), EI.GetSrcNId(),
          NodePref.CStr(), EI.GetDstNId(), EI()==1?"+":"--",
          Net->GetEDat(EI.GetDstNId(), EI.GetSrcNId())==1?"+":"--");
      }
    } else { // save reciprocal edges as 2 different edges
      fprintf(F, "    %s_%d -> %s_%d [label=\"%s\"];\n", NodePref.CStr(), EI.GetSrcNId(),
        NodePref.CStr(), EI.GetDstNId(), EI()==1?"+":"--");
    }
  }
  fprintf(F, "  }\n");
}*/

void TSignMicroEvol::EpinionsTriadEvolution(const bool& OnlySimpleTriads) {
  TSsParser Ss("W:\\Data\\Epinions\\user_rating.txt", ssfTabSep);
  TVec<TIntQu> EdgeV;
  for (int c = 0; Ss.Next(); c++) {
    TChA ChA(Ss[3]);
    TVec<char*> V;  TStrUtil::SplitOnCh(ChA, V, '/');
    TSecTm Tm(atoi(V[0]), atoi(V[1]), atoi(V[2]));
    EdgeV.Add(TIntQu(Tm.GetAbsSecs(), Ss.GetInt(0), Ss.GetInt(1), Ss.GetInt(2))); // (tm,src,dst,sign)
  }
  EdgeV.Sort();
  //// skip people with only plus edges
  /*{ TIntH UIdMnsCntH;
  TVec<TIntQu> EdgeV2;
  for (int e1=0; e1 < EdgeV.Len(); e1++) { UIdMnsCntH.AddDat(EdgeV[e1].Val2) += EdgeV[e1].Val4==-1?1:0; }
  for (int e1=0; e1 < EdgeV.Len(); e1++) { if (UIdMnsCntH.GetDat(EdgeV[e1].Val2)>0) { EdgeV2.Add(EdgeV[e1]); } } //else { SkipOutNIdE.AddKey(EdgeV[e1].Val2); } }
  printf("Skip edges of %d plus-out nodes: %d --> %d\n", -1, EdgeV.Len(), EdgeV2.Len());
  EdgeV.Swap(EdgeV2); } // */
  //// skip people with lots of in-minuses
  /*{ THash<TInt, TIntPr> PlsMnsCnt;
  TIntSet SkipNIdSet;
  TVec<TIntQu> EdgeV2;
  for (int e1=0; e1 < EdgeV.Len(); e1++) {
    if (EdgeV[e1].Val4==1) { PlsMnsCnt.AddDat(EdgeV[e1].Val3).Val1++; }
    else { PlsMnsCnt.AddDat(EdgeV[e1].Val3).Val2++; }
  }
  for (int i=0; i < PlsMnsCnt.Len(); i++) {
    if (PlsMnsCnt[i].Val2/double(PlsMnsCnt[i].Val1+PlsMnsCnt[i].Val2)>0.9) {
      SkipNIdSet.AddKey(PlsMnsCnt.GetKey(i)); }
  }
  for (int e1=0; e1 < EdgeV.Len(); e1++) {
    if (! SkipNIdSet.IsKey(EdgeV[e1].Val2) && ! SkipNIdSet.IsKey(EdgeV[e1].Val3)) {
      EdgeV2.Add(EdgeV[e1]); }
  }
  EdgeV.Swap(EdgeV2); } // */
  PSignNet Net = TSignNet::New();
  int e = 0;
  const TSecTm BegTm(2001, 01, 10);
  //// skip early edges
  for (; e < EdgeV.Len() && (uint)EdgeV[e].Val1 <= BegTm.GetAbsSecs(); e++) {
    if (OnlySimpleTriads &&  Net->IsEdge(EdgeV[e].Val3, EdgeV[e].Val2)) { continue; }
    Net->AddNode(EdgeV[e].Val2);  Net->AddNode(EdgeV[e].Val3);
    Net->AddEdge(EdgeV[e].Val2, EdgeV[e].Val3, EdgeV[e].Val4);
  }
  TSnap::PrintInfo(Net);
  TSignMicroEvol SME(Net);
  //// randomize the edge signs
  //TIntV SignV; for (int e1=e; e1 < EdgeV.Len(); e1++) { SignV.Add(EdgeV[e1].Val4); } SignV.Shuffle(TInt::Rnd);
  //for (int e1=e; e1 < EdgeV.Len(); e1++) { EdgeV[e1].Val4=SignV[e1-e]; } //*/
  //// randomize out edge signs
  /*THash<TInt, TIntV> NIdEH;
  for (int e1=e; e1 < EdgeV.Len(); e1++) { NIdEH.AddDat(EdgeV[e1].Val2).Add(e1);  }
  for (int n = 0; n < NIdEH.Len(); n++) {
    TIntV SignV;
    for (int a = 0; a < NIdEH[n].Len(); a++) { SignV.Add(EdgeV[NIdEH[n][a]].Val4); }
    SignV.Shuffle(TInt::Rnd);
    for (int a = 0; a < NIdEH[n].Len(); a++) { EdgeV[NIdEH[n][a]].Val4 = SignV[a]; }
  } //*/
  //// plus prob of a node
  TIntFltH OutPlusProbH, InPlusProbH;
  { THash<TInt, TIntPr> PlsMnsCnt1, PlsMnsCnt2;
  for (int e1=0; e1 < EdgeV.Len(); e1++) {
    if (EdgeV[e1].Val4==1) { PlsMnsCnt1.AddDat(EdgeV[e1].Val2).Val1++; } // out-prob of +/-
    else { PlsMnsCnt1.AddDat(EdgeV[e1].Val2).Val2++; }
    if (EdgeV[e1].Val4==1) { PlsMnsCnt2.AddDat(EdgeV[e1].Val3).Val1++; } // in-prob of +/-
    else { PlsMnsCnt2.AddDat(EdgeV[e1].Val3).Val2++; }
  }
  for (int u = 0; u < PlsMnsCnt1.Len(); u++) {
    OutPlusProbH.AddDat(PlsMnsCnt1.GetKey(u), PlsMnsCnt1[u].Val1/double(PlsMnsCnt1[u].Val1+PlsMnsCnt1[u].Val2)); }
  for (int u = 0; u < PlsMnsCnt2.Len(); u++) {
    InPlusProbH.AddDat(PlsMnsCnt2.GetKey(u), PlsMnsCnt2[u].Val1/double(PlsMnsCnt2[u].Val1+PlsMnsCnt2[u].Val2));
  } } //*/
  printf("*** skip fist %d edges\n", e);
  int Plus=0, Minus=0, SkipEdges=e;
  const TStr OutFNm = TStr::Fmt("epin851-%s", OnlySimpleTriads?"SmpTr":"AllTr");
  for (; e < EdgeV.Len(); e++) {
    const int n1 = EdgeV[e].Val2;
    const int n2 = EdgeV[e].Val3;
    const int Sign = EdgeV[e].Val4;
    if (n1 == n2 || Net->IsEdge(n1,n2))  { continue; }
    if (Sign==1) { Plus++; } else { Minus++; }
    const double SrcOutPlusProb = OutPlusProbH.GetDat(n1);  // prob. of Src giving a plus
    const double DstInPlusProb = InPlusProbH.GetDat(n2);    // prob. of Dst receiving a plus
    //SME.CountTriadClose(n1, n2, Sign, SrcOutPlusProb, DstInPlusProb, OnlySimpleTriads);
    SME.CountTriad3to4Edges(n1, n2, Sign, SrcOutPlusProb, DstInPlusProb);
    //SME.CountTriadCloseVec(n1, n2, Sign);
    if ((e+1) % 1000 == 0) { printf("."); }
  }
  printf("\n\n*** %s\n", OutFNm.CStr());
  printf("In %d good edges %d plus %d minus.\n", EdgeV.Len()-SkipEdges, Plus, Minus);
  SME.PrintInfo();
  //SME.Save(TFOut(OutFNm+".SME"));
  //SME.SaveCloseTriadTxt(OutFNm, true, true); // CountTriadClose
  SME.SaveTriad3to4Edges(OutFNm);            // CountTriad3to4Edges
  //SME.SaveTriadCloseVec(OutFNm);
  //SME.Network->CountSignedTriads("epin");
}

void GetKeyPrV(const THash<TIntPr, TInt>& H, const int& MinCnt, TFltPrV& V) {
  V.Clr(false);
  for (int i = 0; i < H.Len(); i++) {
    if (H[i] >= MinCnt) {
      V.Add(TFltPr(H.GetKey(i).Val1+TInt::Rnd.GetUniDev()+1.0,
        H.GetKey(i).Val2+TInt::Rnd.GetUniDev()+1.0));
    }
  }
}

void TSignMicroEvol::EpinionsStat() {
  TSsParser Ss("W:\\Data\\Epinions\\user_rating.txt", ssfTabSep);
  TVec<TIntQu> EdgeV;
  for (int c = 0; Ss.Next(); c++) {
    TChA ChA(Ss[3]);
    TVec<char*> V;  TStrUtil::SplitOnCh(ChA, V, '/');
    TSecTm Tm(atoi(V[0]), atoi(V[1]), atoi(V[2]));
    EdgeV.Add(TIntQu(Tm.GetAbsSecs(), Ss.GetInt(0), Ss.GetInt(1), Ss.GetInt(2))); // (tm,src,dst,sign)
  }
  THash<TInt, TIntQu> NIdPmOutInH;
  for (int e = 0; e < EdgeV.Len(); e++) {
    TIntQu& Src = NIdPmOutInH.AddDat(EdgeV[e].Val2);
    TIntQu& Dst = NIdPmOutInH.AddDat(EdgeV[e].Val3);
    if (EdgeV[e].Val4 == 1) {
      Src.Val1++; Dst.Val3++; // plus out, plus in
    } else {
      Src.Val2++; Dst.Val4++; // minus out, minus in
    }
  }
  TFltPrV PiPo, PiMi, PiMo, PoMi, PoMo, MiMo;
  THash<TIntPr, TInt> PiPoH, PiMiH, PiMoH, PoMiH, PoMoH, MiMoH;
  for (int i = 0; i < NIdPmOutInH.Len(); i++) {
    TIntQu PMOI = NIdPmOutInH[i];
    PiPo.Add(TFltPr(PMOI.Val3+TInt::Rnd.GetUniDev()+1.0, PMOI.Val1+TInt::Rnd.GetUniDev()+1.0));
    PiMi.Add(TFltPr(PMOI.Val3+TInt::Rnd.GetUniDev()+1.0, PMOI.Val4+TInt::Rnd.GetUniDev()+1.0));
    PiMo.Add(TFltPr(PMOI.Val3+TInt::Rnd.GetUniDev()+1.0, PMOI.Val2+TInt::Rnd.GetUniDev()+1.0));
    PoMi.Add(TFltPr(PMOI.Val1+TInt::Rnd.GetUniDev()+1.0, PMOI.Val4+TInt::Rnd.GetUniDev()+1.0));
    PoMo.Add(TFltPr(PMOI.Val1+TInt::Rnd.GetUniDev()+1.0, PMOI.Val2+TInt::Rnd.GetUniDev()+1.0));
    MiMo.Add(TFltPr(PMOI.Val4+TInt::Rnd.GetUniDev()+1.0, PMOI.Val2+TInt::Rnd.GetUniDev()+1.0));//*/
    PiPoH.AddDat(TIntPr(PMOI.Val3, PMOI.Val1)) += 1;
    PiMiH.AddDat(TIntPr(PMOI.Val3, PMOI.Val4)) += 1;
    PiMoH.AddDat(TIntPr(PMOI.Val3, PMOI.Val2)) += 1;
    PoMiH.AddDat(TIntPr(PMOI.Val1, PMOI.Val4)) += 1;
    PoMoH.AddDat(TIntPr(PMOI.Val1, PMOI.Val2)) += 1;
    MiMoH.AddDat(TIntPr(PMOI.Val4, PMOI.Val2)) += 1;
  }
  /*GetKeyPrV(PiPoH, 1, PiPo);
  GetKeyPrV(PiMiH, 1, PiMi);
  GetKeyPrV(PiMoH, 1, PiMo);
  GetKeyPrV(PoMiH, 1, PoMi);
  GetKeyPrV(PoMoH, 1, PoMo);
  GetKeyPrV(MiMoH, 1, MiMo);*/
  TStr OutFNm = "EpinAll";
  TGnuPlot::PlotValV(PiPo, "pipo-"+OutFNm, OutFNm, "Number of + IN", "Number of + OUT", gpsLog, false, gpwPoints);
  TGnuPlot::PlotValV(PiMi, "pimi-"+OutFNm, OutFNm, "Number of + IN", "Number of - IN", gpsLog, false, gpwPoints);
  TGnuPlot::PlotValV(PiMo, "pimo-"+OutFNm, OutFNm, "Number of + IN", "Number of - OUT", gpsLog, false, gpwPoints);
  TGnuPlot::PlotValV(PoMi, "pomi-"+OutFNm, OutFNm, "Number of + OUT", "Number of - IN", gpsLog, false, gpwPoints);
  TGnuPlot::PlotValV(PoMo, "pomo-"+OutFNm, OutFNm, "Number of + OUT", "Number of - OUT", gpsLog, false, gpwPoints);
  TGnuPlot::PlotValV(MiMo, "mimo-"+OutFNm, OutFNm, "Number of - IN", "Number of - OUT", gpsLog, false, gpwPoints);
}

