#include "stdafx.h"
#include "wgtnet.h"
#include "dblp.h"
#include "arxiv.h"


/////////////////////////////////////////////////
// Weighted network
double TWgtNet::GetEdgeWgt() const {
  double wgt = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      wgt += NI.GetOutEDat(e);
    }
  }
  return wgt;
}

void TWgtNet::MulEdgeWgt(const double& MulBy) {
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NI.GetOutEDat(e) *= MulBy;
    }
  }
}

void TWgtNet::PermEdgeWgt() {
  TFltV WgtV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      WgtV.Add(NI.GetOutEDat(e)); 
    }
  }
  WgtV.Shuffle(TInt::Rnd);
  int w = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NI.GetOutEDat(e) = WgtV[w++]; 
    }
  }
}

void TWgtNet::PermOutEdgeWgt() {
  TFltV WgtV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    WgtV.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      WgtV.Add(NI.GetOutEDat(e)); 
    }
    WgtV.Shuffle(TInt::Rnd);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NI.GetOutEDat(e) = WgtV[e]; 
    }
  }
}

void TWgtNet::PutAllWgts() {
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NI.GetOutEDat(e) = TInt::Rnd.GetUniDev();
    }
  }
}

void TWgtNet::PutRnd01Wgts() {
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NI.GetOutEDat(e) = TInt::Rnd.GetUniDev();
    }
  }
}

// Wgt == -1 : take the weight of the edge in the opposite direction
void TWgtNet::AddBiDirEdges(const double& Wgt) {
  TIntPrV EdgeV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) { 
      if (! IsEdge(NI.GetOutNId(e), NI.GetId())) {
        EdgeV.Add(TIntPr(NI.GetOutNId(e), NI.GetId())); }
    }
  }
  for (int e = 0; e < EdgeV.Len(); e++) {
    if (Wgt != -1) {
      AddEdge(EdgeV[e].Val1, EdgeV[e].Val2, Wgt);
    } else { // edge weight in the opposite direction
      AddEdge(EdgeV[e].Val1, EdgeV[e].Val2, GetEDat(EdgeV[e].Val2, EdgeV[e].Val1));
    }
  }
}

void TWgtNet::DelMinWgtNodes(const double MinWgt) {
  printf("Deleting Min Wgt %g nodes\n", MinWgt);
  printf("  (%d,%d)  -->", GetNodes(), GetEdges());
  TIntV DelNIdV;  
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    double wgt = 0;
    for (int e = 0; e < NI.GetOutDeg(); e++) { 
      wgt += NI.GetOutEDat(e);
    }
    if (wgt < MinWgt) { DelNIdV.Add(NI.GetId()); }
  }
  for (int d = 0; d < DelNIdV.Len(); d++) {
    DelNode(DelNIdV[d]);
  }
  printf("  (%d,%d)\n", GetNodes(), GetEdges());
}

// IN-OUT edges are swapped (so that the prog runs faster)
// Send message via IN edge proportional to the OUT edge weight
void TWgtNet::ReinforceEdges(const int& NIters) {
  THash<TInt, TFlt> OutWgtSumH;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    double wgt = 0;
    for (int e = 0; e < NI.GetOutDeg(); e++) { 
      wgt += NI.GetOutEDat(e); }
    OutWgtSumH.AddDat(NI.GetId(), wgt);
  }
  printf("Reinforcing edges for %d iterations\n", NIters);
  // iterate
  TExeTm ExeTm;
  for (int iter = 0; iter < NIters; iter++) {
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      const double X = TInt::Rnd.GetUniDev() * OutWgtSumH.GetDat(NI.GetId());
      double x = 0;  int e = 0;
      for ( ; x + NI.GetOutEDat(e) < X; e++) {
        x += NI.GetOutEDat(e); }
      IAssert(IsEdge(NI.GetOutNId(e), NI.GetId()));
      GetEDat(NI.GetOutNId(e), NI.GetId()) += 1; // reinforce the edge
      OutWgtSumH.GetDat(NI.GetOutNId(e)) += 1; 
    }
    if (iter % (NIters/100) == 0) {
      printf("\r%d [%s]", iter, ExeTm.GetStr()); 
    }
  }
  printf(" done.\n");
}

PWgtNet TWgtNet::GetRngWgtNet(const int& Nodes, const int& Edges, const double& Wgt) {
  PWgtNet Net = TWgtNet::New();
  for (int n = 0; n < Nodes; n++) {
    Net->AddNode();
  }
  for (int edges = 0; edges < Edges; ) {
    int nid1 = TInt::Rnd.GetUniDevInt(Nodes);
    int nid2 = TInt::Rnd.GetUniDevInt(Nodes);
    if (nid1 == nid2) { continue; }
    if (nid1 > nid2) { Swap(nid1, nid2); }
    if (Net->AddEdge(nid1, nid2, Wgt==-1?TInt::Rnd.GetUniDev():Wgt) == -1) { // new edge
      edges++; 
    }
    if (Net->AddEdge(nid2, nid1, Wgt==-1?TInt::Rnd.GetUniDev():Wgt) == -1) { // new edge
      edges++; 
    }
  }
  return Net;
}

// "W:\\Data\\CiteSeer\\old\\citeseer-links.csv"
PWgtNet TWgtNet::LoadCiteSeerCoAuth(const TStr& FNm) {
  PWgtNet Net = TWgtNet::New();
  TStrSet AuthorSet;
  TSsParser Ss(FNm, ssfCommaSep);
  while (Ss.Next()) {
    for (int a1 = 2; a1 < Ss.Len(); a1++) {
      const int n1 = AuthorSet.AddKey(Ss[a1]);
      for (int a2 = 2; a2 < Ss.Len(); a2++) {
        if (a1 == a2) { continue; }
        const int n2 = AuthorSet.AddKey(Ss[a2]);
        if (! Net->IsNode(n1)) { Net->AddNode(n1, Ss[a1]); }
        if (! Net->IsNode(n2)) { Net->AddNode(n2, Ss[a2]); }
        if (Net->IsEdge(n1, n2)) { Net->GetEDat(n1, n2) += 1; }
        else { Net->AddEdge(n1, n2, 1); }
      }
    }
  }
  TGBase::PrintInfo(Net);
  printf("  Edge weight: %f\n", Net->GetEdgeWgt());
  return Net;
}

// Network is undirected (edges of equal weight go both ways)
// "W:\\Data\\DBLP\\dblp.xml.gz"
PWgtNet TWgtNet::LoadDblpCoAuth(const TStr& FNm) {
  TDblpLoader Dblp(FNm);
  TStrSet AuthorSet;
  PWgtNet Net = TWgtNet::New();
  for (int c = 0; Dblp.Next(); c++) {
    for (int a1 = 0; a1 < Dblp.AuthorV.Len(); a1++) {
      const int n1 = AuthorSet.AddKey(Dblp.AuthorV[a1]);
      for (int a2 = 0; a2 < Dblp.AuthorV.Len(); a2++) {
        if (a1 == a2) { continue; }
        const int n2 = AuthorSet.AddKey(Dblp.AuthorV[a2]);
        if (! Net->IsNode(n1)) { Net->AddNode(n1, Dblp.AuthorV[a1]); }
        if (! Net->IsNode(n2)) { Net->AddNode(n2, Dblp.AuthorV[a2]); }
        if (Net->IsEdge(n1, n2)) { Net->GetEDat(n1, n2) += 1; }
        else { Net->AddEdge(n1, n2, 1); }
      }
    }
    if (c % 1000 == 0) { printf("\r%d", c); }
  }
  printf("\n");
  TGBase::PrintInfo(Net);
  printf("  Edge weight: %f\n", Net->GetEdgeWgt());
  return Net;
}

// Arxiv co-authorship network
// Network is undirected (edges of equal weight go both ways)
// "W:\\Data\\Arxiv\\Arxiv-CoAuth\\gr-qc.lis"
PWgtNet TWgtNet::LoadArxivCoAuth(const TStr& FNm) {
  TArxivPaperList Arxiv(FNm);
  PWgtNet Net = TWgtNet::New();
  TStrSet AuthorSet;
  while (Arxiv.Next()) {
    for (int a1 = 0; a1 < Arxiv.AuthorV.Len(); a1++) {
      const int n1 = AuthorSet.AddKey(Arxiv.AuthorV[a1]);
      for (int a2 = 0; a2 < Arxiv.AuthorV.Len(); a2++) {
        if (a1 == a2) { continue; }
        const int n2 = AuthorSet.AddKey(Arxiv.AuthorV[a2]);
        if (! Net->IsNode(n1)) { Net->AddNode(n1, Arxiv.AuthorV[a1]); }
        if (! Net->IsNode(n2)) { Net->AddNode(n2, Arxiv.AuthorV[a2]); }
        if (Net->IsEdge(n1, n2)) { Net->GetEDat(n1, n2) += 1; }
        else { Net->AddEdge(n1, n2, 1); }
      }
    }
  }
  TGBase::PrintInfo(Net);
  printf("  Edge weight: %f\n", Net->GetEdgeWgt());
  return Net;
}

// Eve communication network
PWgtNet TWgtNet::LoadEveCommNet(const TStr& FNm) {
  PWgtNet Net = TWgtNet::New();
  TStrSet AuthorSet;
  TChA Ln;
  TVec<char*> WrdV;
  TFIn FIn(FNm);
  for (int c=0; FIn.GetNextLn(Ln); c++) {
    TStrUtil::SplitOnCh(Ln, WrdV, ';');
    const int n1 = AuthorSet.AddKey(WrdV[0]);
    const int n2 = AuthorSet.AddKey(WrdV[1]);
    if (! Net->IsNode(n1)) { Net->AddNode(n1, WrdV[0]); }
    if (! Net->IsNode(n2)) { Net->AddNode(n2, WrdV[1]); }
    if (Net->IsEdge(n1, n2)) { Net->GetEDat(n1, n2) += 1; }
    else { Net->AddEdge(n1, n2, 1); }
    if (c % Kilo(10) == 0) { printf("\r%dk", c/1000); }
  }
  printf("\n");
  TGBase::PrintInfo(Net);
  printf("  Edge weight: %f\n", Net->GetEdgeWgt());
  return Net;
}

/*
// get a subgraph of a weighted graph where each node links to top K heaviest neighbors
// TGetWeight must implement: static double TGetWeight::GetWgt(const TEdgeDat& )
template<class PGraph, class TGetWeight> 
PGraph GetTopKEdgeSubGraph(const PGraph& Graph, const int& TopKEdges) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  TFltIntPrV WgtNIdV;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    WgtNIdV.Clr();
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      WgtNIdV.Add(TFltIntPr(TGetWeight::GetWgt(NI.GetOutEDat(e)), e));
    }
    WgtNIdV.Sort(false);
    if (! NewGraph.IsNode(NI.GetId())) { NewGraph.AddNode(NI); }
    for (int k = 0; k < TMath::Mn(TopKEdges, WgtNIdV.Len()); k++) {
      const int dst = NI.GetOutNId(WgtNIdV[k].Val2);
      if (! NewGraph.IsNode(dst)) { NewGraph.AddNode(Graph->GetNI(dst)); }
      NewGraph.AddEdge(NI.GetId(), dst, NI.GetOutEDat(WgtNIdV[k].Val2));
    }
  }
  return NewGraphPt;
}

template<class PGraph, class TGetWeight> 
PGraph GetMinEdgeWgtSubGraph(const PGraph& Graph, const double& MinWgt) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  TFltIntPrV WgtNIdV;
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    if (TGetWeight::GetWgt(EI()) < MinWgt) { continue; }
    const int src = EI.GetSrcNId();
    const int dst = EI.GetDstNId();
    if (! NewGraph.IsNode(src)) { NewGraph.AddNode(Graph->GetNI(src)); }
    if (! NewGraph.IsNode(dst)) { NewGraph.AddNode(Graph->GetNI(dst)); }
    NewGraph.AddEdge(EI);
  }
  return NewGraphPt;
}*/

/////////////////////////////////////////////////
// Top2 Friends network
void TTop2FriendNet::SetTop2() {
  Top2NIdH.Gen(Net->GetNodes());
  TFltIntPrV WgtNIdV;
  for (TWgtNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    WgtNIdV.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      WgtNIdV.Add(TFltIntPr(NI.GetOutEDat(e), NI.GetOutNId(e)));
    }
    WgtNIdV.Shuffle(TInt::Rnd); // so that ties are broken randomly
    WgtNIdV.Sort(false);
    if (WgtNIdV.Len() == 0) { Top2NIdH.AddDat(NI.GetId(), TIntPr(-1, -1)); }
    else if (WgtNIdV.Len() == 1) { Top2NIdH.AddDat(NI.GetId(), TIntPr(WgtNIdV[0].Val2, -1)); } 
    else if (WgtNIdV.Len() >= 2) {
      Top2NIdH.AddDat(NI.GetId(), TIntPr(WgtNIdV[0].Val2, WgtNIdV[1].Val2)); }
  }
  // create union find structure
  PNGraph Top1Net = GetTop1Net();
  Top1UF = TUnionFind(Top1Net->GetNodes());
  TCnComV CnComV; 
  TCnCom::GetWccs(Top1Net, CnComV);
  for (TWgtNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Top1UF.Add(NI.GetId());
  }
  for (int c = 0; c < CnComV.Len(); c++) {
    for (int i = 1; i < CnComV[c].Len(); i++) {
      Top1UF.Union(CnComV[c][0], CnComV[c][i]); }
  }
}

PNGraph TTop2FriendNet::GetTop1Net() {
  PNGraph Top1Net = TNGraph::New();
  for (int i = 0; i < Top2NIdH.Len(); i++) {
    const int n1 = Top2NIdH.GetKey(i);
    const int n2 = Top2NIdH[i].Val1;
    if (n2 == -1) { continue; }
    if (! Top1Net->IsNode(n1)) { Top1Net->AddNode(n1); }
    if (! Top1Net->IsNode(n2)) { Top1Net->AddNode(n2); }
    Top1Net->AddEdge(n1, n2);
  }
  return Top1Net;
}

int TTop2FriendNet::GetTop2WccSz(const double ProbPick2nd) const {
  TUnionFind UF(Top1UF);
  for (int n = 0; n < Top2NIdH.Len(); n++) {
    if (TInt::Rnd.GetUniDev() <= ProbPick2nd && Top2NIdH[n].Val2 != -1) {
      UF.Union(Top2NIdH.GetKey(n), Top2NIdH[n].Val2);
    }
  }
  TIntH CcSzH;
  for (int i = 0; i < UF.Len(); i++) {
    CcSzH.AddDat(UF.Find(UF.GetKeyI(i))) += 1;
  }
  int MxCcSz = -1;
  for (int c = 0; c < CcSzH.Len(); c++) {
    if (CcSzH[c] > MxCcSz) { MxCcSz = CcSzH[c]; }
  }
  return MxCcSz;
}

int TTop2FriendNet::GetRnd2WccSz(const double ProbPick2nd) const {
  TCnComV CnComV;
  PNGraph G = TNGraph::New();
  for (TWgtNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    if (NI.GetOutDeg() == 0) { continue; }
    const int NId1 = NI.GetOutNId(TInt::Rnd.GetUniDevInt(NI.GetOutDeg()));
    G->AddNode(NI.GetId());
    G->AddNode(NId1);
    G->AddEdge(NI.GetId(), NId1);
    if (NI.GetOutDeg() > 1 && TInt::Rnd.GetUniDev() <= ProbPick2nd) {
      int NId2 = NI.GetOutNId(TInt::Rnd.GetUniDevInt(NI.GetOutDeg()));
      while (NId2 == NId1) { NId2 = NI.GetOutNId(TInt::Rnd.GetUniDevInt(NI.GetOutDeg())); }
      G->AddNode(NId2);
      G->AddEdge(NI.GetId(), NId2);
    }
  }
  TCnCom::GetWccs(G, CnComV);
  return CnComV[0].Len();
}

void TTop2FriendNet::GetAvgSDevV(const THash<TFlt, TMom>& MomH, TFltTrV& ValAvgSDevV) {
  ValAvgSDevV.Clr(false);
  for (int i = 0; i < MomH.Len(); i++) {
    TMom Mom=MomH[i];
    Mom.Def();
    ValAvgSDevV.Add(TFltTr(MomH.GetKey(i), Mom.GetMean(), Mom.GetSDev()));
  }
  ValAvgSDevV.Sort();
}

void TTop2FriendNet::PlotPick2VsProb2nd(const PWgtNet& Net, const int& NRuns, const double& StepP, const TStr& OutFNm, 
                                        TStr Desc, bool PlotTop2, bool PlotBtm2, bool PlotRnd2) {
  TTop2FriendNet Top2(Net);  Net->MulEdgeWgt(-1.0); 
  TTop2FriendNet Btm2(Net);  Net->MulEdgeWgt(-1.0); // change back
  THash<TFlt, TMom> Top2H, Btm2H, Rnd2H;
  for (int run = 0; run < NRuns; run++) {
    TExeTm ExeTm;
    printf("run %d\n", run);
    for (double p = 0; p <= 1; p += StepP) {
      if (PlotTop2) { Top2H.AddDat(p).Add(Top2.GetTop2WccSz(p)); }
      if (PlotBtm2) { Btm2H.AddDat(p).Add(Btm2.GetTop2WccSz(p)); }
      if (PlotRnd2) { Rnd2H.AddDat(p).Add(Top2.GetRnd2WccSz(p)); }
      printf(".");
    }
    printf("[%s]\n", ExeTm.GetStr());
    TFltTrV Top2V, Btm2V, Rnd2V;
    GetAvgSDevV(Top2H, Top2V);
    GetAvgSDevV(Btm2H, Btm2V);
    GetAvgSDevV(Rnd2H, Rnd2V);
    TGnuPlot GP("ccVsP-"+OutFNm, TStr::Fmt("%s (%d, %d, %f)", Desc.CStr(), Net->GetNodes(), 
      Net->GetEdges(), Net->GetEdgeWgt()));
    GP.SetXYLabel("Prob of taking 2nd edge", "Size of largest connected component");
    if (! Top2V.Empty()) { GP.AddErrBar(Top2V, "TOP", ""); }
    if (! Rnd2V.Empty()) { GP.AddErrBar(Rnd2V, "RND", ""); }
    if (! Btm2V.Empty()) { GP.AddErrBar(Btm2V, "BTM", ""); }
    GP.SavePng();
  }
}


/*
void LoadEmailUserIdH(TStrSet& UsrSet, THash<TStr, TInt>& EmUIdH, THash<TInt, TStr>& UIdDeptH) {
  TSsParser Ss("W:\\Data\\EmailGraph\\JSI-People.Tab", ssfTabSep);
  while (Ss.Next()) {
    IAssert(! UsrSet.IsKey(Ss[0]));
    if (strlen(Ss[0])==0) { continue; }
    const int UId = UsrSet.AddKey(Ss[0]);
    UIdDeptH.AddDat(UId, Ss[1]);
    for (int e = 2; e < Ss.Len(); e++) {
      TStr U = TStr(Ss[e]).GetLc();
      if (U.Empty()) { continue; }
      if (EmUIdH.IsKey(U)) { printf("%s\t%s\n", U.CStr(), Ss[0]); continue; }
      EmUIdH.AddDat(U, UId);
    }
  }
}

void LoadEmailIdUserId(const THash<TStr, TInt>& EmStrUIdH, TIntH& EmIdUIdH) {
  TSsParser Ss("W:\\Data\\EmailGraph\\EMailPersonFq.Txt", ssfSpaceSep, true);
  while (Ss.Next()) {
    TStr email = Ss[2];  email.ToLc();
    if (email.SearchCh('@')==-1) { 
      printf("bad email: %s\n", Ss[0]); continue; }
    if (! EmStrUIdH.IsKey(email)) { continue; }
    const int EmailId = TStr(Ss[3]).Slice(1,-1).GetInt();
    EmIdUIdH.AddDat(EmailId, EmStrUIdH.GetDat(email));
  }
}

void BuildEmailNet() {
  TStrSet UsrSet;
  THash<TStr, TInt> EmStrUIdH;
  THash<TInt, TStr> UIdDeptH;
  TIntH EmIdUIdH;
  LoadEmailUserIdH(UsrSet, EmStrUIdH, UIdDeptH);
  LoadEmailIdUserId(EmStrUIdH, EmIdUIdH);
  TPt<TNodeEDatNet<TStr, TInt> > EmNet = TNodeEDatNet<TStr, TInt>::New();
  TSsParser Ss("W:\\Data\\EmailGraph\\EMailTsactAnon.Txt", ssfSpaceSep, true);
  for (int c = 0; Ss.Next(); c++) {
    if (! EmIdUIdH.IsKey(Ss.GetInt(1))) { continue; }
    const int Src = EmIdUIdH.GetDat(Ss.GetInt(1));
    for (int d = 3; d < Ss.Len(); d++) {
      if (! EmIdUIdH.IsKey(Ss.GetInt(d))) { continue; }
      const int Dst = EmIdUIdH.GetDat(Ss.GetInt(d));
      if (Src == Dst) { continue; }
      if (! EmNet->IsNode(Src)) { EmNet->AddNode(Src, UsrSet[Src]+":"+UIdDeptH.GetDat(Src)); }
      if (! EmNet->IsNode(Dst)) { EmNet->AddNode(Dst, UsrSet[Dst]+":"+UIdDeptH.GetDat(Dst)); }
      if (EmNet->IsEdge(Src, Dst)) { EmNet->GetEDat(Src, Dst) += 1; }
      else { EmNet->AddEdge(Src, Dst, 1); }
    }
    if (c%1000==0) { printf("\r%d", c); }
  }
  printf("\n");
  TGBase::PrintInfo(EmNet);
  EmNet->Save(TFOut("email.net"));
}
*/