#include "stdafx.h"

// For more details and motivation what this code is trying to achive see 
// "Correcting for Missing Data in Information Cascades" by E. Sadikov, M. Medina, J. Leskovec, H. Garcia-Molina. WSDM, 2011

/////////////////////////////////////////////////
// Structural properties of the cascades (propagation trees)
class TCascadeStat {
public:
  THash<TFlt, TMom> NCascInf, NCascNet;   // number of cascades
  THash<TFlt, TMom> MxSzInf, MxSzNet;     // size of the largest cascade
  THash<TFlt, TMom> AvgSzInf, AvgSzNet;   // average cascade size (number of nodes)
  THash<TFlt, TMom> NIsoInf, NIsoNet;     // number of isolated nodes in the cascade
  THash<TFlt, TMom> NLfInf, NLfNet;       // number of leaves in a cascade
  THash<TFlt, TMom> NRtInf, NRtNet;       // number of roots in a cascade
  THash<TFlt, TMom> OutDegInf, OutDegNet; // average out-degree of a cascade
  THash<TFlt, TMom> InDegInf, InDegNet;   // average in-degree of a cascade
  // requires the root node (largest connected component)
  THash<TFlt, TMom> DepthInf, DepthNet;   // average depth (avg. distance from leaves to the root)
  THash<TFlt, TMom> MxWidInf, MxWidNet;   // cascade width (max number of nodes at any depth d)
  THash<TFlt, TMom> MxLevInf, MxLevNet;   // level of max width (depth of max width)
  THash<TFlt, TMom> IncLevInf, IncLevNet; // number of levels with increasing width
public:
  TCascadeStat() { }
  void PlotAll(const TStr& OutFNm, const TStr& Desc, const bool& DivByM=true);
  static double NonZ(double C) { return C==0?1.0:C; }
  // randomly remove nodes from the cascade and store cascade properties as a function of the fraction of removed nodes
  // for more details see "Correcting for Missing Data in Information Cascades" by E. Sadikov, M. Medina, J. Leskovec, H. Garcia-Molina. WSDM, 2011
  template <class PGraph>
  void SampleCascade(const PGraph& InfCasc, const PGraph& NetCasc, const TIntH& NIdInfTmH, const double& PStep=0.05, const int& NRuns=1, const bool& DivByM=true) {
    for (int Run=0; Run < NRuns; Run++) {
      for (double P = PStep; P <= 1.01; P += PStep) {
        TIntV NIdV;
        for (typename PGraph::TObj::TNodeI NI = InfCasc->BegNI(); NI < InfCasc->EndNI(); NI++) {
          if (TInt::Rnd.GetUniDev() < P) { NIdV.Add(NI.GetId()); } }
        PGraph InfG = TSnap::GetSubGraph(InfCasc, NIdV);
        PGraph NetG = TSnap::GetSubGraph(NetCasc, NIdV);
        if (InfG->GetNodes()==0) { continue; }
        TakeStat(InfG, NetG, NIdInfTmH, P, DivByM);
      }
    }
  }
  template<class PGraph>
  int FindCascadeRoot(const PGraph& G, const TIntH& NIdInfTmH) { // earliest infected node
    int Min=TInt::Mx, MinNId=-1;
    for (typename PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      const int t = NIdInfTmH.GetDat(NI.GetId());
      if (t < Min && NI.GetInDeg()==0) { Min=t; MinNId=NI.GetId(); } }
    IAssert(MinNId!=-1);  return MinNId;
  }
  template<class PGraph>
  double GetAvgDepthFromRoot(const PGraph& G) {
    TMom Mom;
    TIntPrV HopCntV;
    for (typename PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      if (NI.GetOutDeg()>0 && NI.GetInDeg()==0) {
        TSnap::GetNodesAtHops(G, NI.GetId(), HopCntV, true);
        Mom.Add(HopCntV.Last().Val1()); }
    }
    Mom.Def();  return Mom.GetMean();
  }
  // collect statistics of the structural properties of the cascade
  template <class PGraph>
  void TakeStat(const PGraph& InfG, const PGraph& NetG, const TIntH& NIdInfTmH, const double& P, const bool& DivByM=true) {
    const double M = DivByM ? InfG->GetNodes() : 1;  IAssert(M>=1);
    PGraph CcInf, CcNet; // largest connected component
    // connected components and sizes
    { TCnComV CnComV;  TSnap::GetWccs(InfG, CnComV);
    NCascInf.AddDat(P).Add(CnComV.Len()/M);
    MxSzInf.AddDat(P).Add(CnComV[0].Len()/M);
    { int a=0; for (int i=0; i<CnComV.Len(); i++) { a+=CnComV[i].Len(); }
    AvgSzInf.AddDat(P).Add(a/double(CnComV.Len()*M)); }
    CcInf = TSnap::GetSubGraph(InfG, CnComV[0].NIdV);
    TSnap::GetWccs(NetG, CnComV);
    NCascNet.AddDat(P).Add(CnComV.Len()/M);
    MxSzNet.AddDat(P).Add(CnComV[0].Len()/M);
    { int a=0; for (int i=0; i<CnComV.Len(); i++) { a+=CnComV[i].Len(); }
    AvgSzNet.AddDat(P).Add(a/double(CnComV.Len()*M)); }
    CcNet = TSnap::GetSubGraph(NetG, CnComV[0].NIdV); }
    // count isolated nodes and leaves; average in- and out-degree (skip leaves)
    { int i1=0, i2=0,l1=0,l2=0,r1=0,r2=0,ENet=0,EInf=0; double ci1=0,ci2=0,co1=0,co2=0;
    for (typename PGraph::TObj::TNodeI NI = InfG->BegNI(); NI < InfG->EndNI(); NI++) {
      if (NI.GetOutDeg()==0 && NI.GetInDeg()>0) { l1++; }
      if (NI.GetOutDeg()>0 && NI.GetInDeg()==0) { r1++; }
      if (NI.GetDeg()==0) { i1++; }  if (NI.GetInDeg()>0) { ci1+=1; }
      if (NI.GetOutDeg()>0) { co1+=1; }  EInf+=NI.GetOutDeg(); }
    for (typename PGraph::TObj::TNodeI NI = NetG->BegNI(); NI < NetG->EndNI(); NI++) {
      if (NI.GetOutDeg()==0 && NI.GetInDeg()>0) { l2++; }
      if (NI.GetOutDeg()>0 && NI.GetInDeg()==0) { r2++; }
      if (NI.GetDeg()==0) { i2++; }  if (NI.GetInDeg()>0) { ci2+=1; }
      if (NI.GetOutDeg()>0) { co2+=1; }  ENet+=NI.GetOutDeg(); }
    if(ci1>0)InDegInf.AddDat(P).Add(EInf/ci1);  if(ci2>0)InDegNet.AddDat(P).Add(ENet/ci2);
    if(co1>0)OutDegInf.AddDat(P).Add(EInf/co1); if(co2>0)OutDegNet.AddDat(P).Add(ENet/co2);
    NLfInf.AddDat(P).Add(l1/M);  NLfNet.AddDat(P).Add(l2/M);
    NRtInf.AddDat(P).Add(r1/M);  NRtNet.AddDat(P).Add(r2/M);
    NIsoInf.AddDat(P).Add(i1/M); NIsoNet.AddDat(P).Add(i2/M); }
    // cascade depth
    { const double M1 = DivByM ? CcNet->GetNodes() : 1;  IAssert(M1>=1);
    int Root=FindCascadeRoot(CcInf, NIdInfTmH);  TIntPrV HopCntV;
    TSnap::GetNodesAtHops(CcInf, Root, HopCntV, true);
    int MxN=0, Lev=0, IncL=0;
    for (int i = 0; i < HopCntV.Len(); i++) {
      if (MxN<HopCntV[i].Val2) { MxN=HopCntV[i].Val2; Lev=HopCntV[i].Val1; }
      if (i > 0 && HopCntV[i-1].Val2<=HopCntV[i].Val2) { IncL++; } }
    double D=0; int c=0; TIntH DistH;
    D = HopCntV.Last().Val1; c=1; // maximum depth
    if (c!=0 && D!=0) { D = D/c;
      DepthInf.AddDat(P).Add(D/M1); MxWidInf.AddDat(P).Add(MxN/M1);
      MxLevInf.AddDat(P).Add(Lev/D); IncLevInf.AddDat(P).Add(IncL/D);
    }
    Root=FindCascadeRoot(CcNet, NIdInfTmH);
    TSnap::GetNodesAtHops(CcNet, Root, HopCntV, true);
    MxN=0; Lev=0; IncL=0; D=0; c=0;
    for (int i = 0; i < HopCntV.Len(); i++) {
      if (MxN<HopCntV[i].Val2) { MxN=HopCntV[i].Val2; Lev=HopCntV[i].Val1; }
      if (i > 0 && HopCntV[i-1].Val2<=HopCntV[i].Val2) { IncL++; } }
    D = HopCntV.Last().Val1; c=1; // maximum depth
    if (c!=0 && D!=0) { D = D/c;
      DepthNet.AddDat(P).Add(D/M1); MxWidNet.AddDat(P).Add(MxN/M1);
      MxLevNet.AddDat(P).Add(Lev/D); IncLevNet.AddDat(P).Add(IncL/D); }
    }
  }
};

PNGraph RunSICascade(PUNGraph G, const double& Beta, const int& CascSz, TIntH& NIdInfTmH);
PNGraph RunSICascade2(PUNGraph G, const double& Beta, const int& MxCascSz, TIntH& NIdInfTmH);
PNGraph AddSpuriousEdges(const PUNGraph& Graph, const PNGraph& Casc, TIntH NIdTmH);


int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Cascades. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "demo", "Input undirected graph");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "demo", "Output file name prefix");
  const double Beta = Env.GetIfArgPrefixFlt("-b:", 0.1, "Beta (infection (i.e., cascade propagation) probability)");
  // load
  printf("Loading %s...", InFNm.CStr());
  PUNGraph Graph;
  if (InFNm == "demo") { Graph = TSnap::GenRndGnm<PUNGraph>(100, 200); }
  else { Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm); }
  printf("nodes:%d  edges:%d\n", Graph->GetNodes(), Graph->GetEdges());
  
  // Simulate SI model
  Graph = TSnap::GetMxWcc(Graph);
  bool DivByM = true;
  TCascadeStat CascStat;
  printf("\nGraph:%s -- Beta: %g\n", OutFNm.CStr(), Beta);
  for (int Run = 0; Run < 10; Run++) { // number of runs
    TIntH NIdInfTmH;
    // incluence cascade
    PNGraph InfCasc = RunSICascade(Graph, Beta, 100, NIdInfTmH);
    if (InfCasc->GetNodes() < 10) { printf("."); continue; } // min cascade size
    // network cascade
    PNGraph NetCasc = AddSpuriousEdges(Graph, InfCasc, NIdInfTmH);
    // sample the cascade
    CascStat.SampleCascade(InfCasc, NetCasc, NIdInfTmH, 0.1, 10, DivByM);  // div-by-M
    printf(".");
  }
  CascStat.PlotAll(TStr::Fmt("%s-B%03d", OutFNm.CStr(), int(100*Beta)), TStr::Fmt("%s N=%d  E=%d  Beta=%g", 
    OutFNm.CStr(), Graph->GetNodes(), Graph->GetEdges(), Beta), DivByM);
  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}

/////////////////////////////////////////////////
// Implementation
void TCascadeStat::PlotAll(const TStr& OutFNm, const TStr& Desc, const bool& DivByM) {
  const TStr MStr = DivByM ? " / M (number of observed nodes)" : "";
  { TGnuPlot GP(TStr::Fmt("ncasc-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Number of connected components" + MStr);
  GP.AddPlot(NCascInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(NCascNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("mxSz-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Size of largest connected component" + MStr);
  GP.AddPlot(MxSzInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(MxSzNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("avgSz-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Average connected component size" + MStr);
  GP.AddPlot(AvgSzInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(AvgSzNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("nIso-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Number of isolated nodes" + MStr);
  GP.AddPlot(NIsoInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(NIsoNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("nRt-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Number of root nodes" + MStr);
  GP.AddPlot(NRtInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(NRtNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("nLf-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Number of leaves (nodes of zero out-degree)" + MStr);
  GP.AddPlot(NLfInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(NLfNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("outDeg-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Average Out-Degree (of a non-leaf)");
  GP.AddPlot(OutDegInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(OutDegNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("inDeg-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Average In-Degree (of a non-root)");
  GP.AddPlot(InDegInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(InDegNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("levels-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Average depth of largest component" + MStr);
  GP.AddPlot(DepthInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(DepthNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("width-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Width of largest components (max nodes at any level)" + MStr);
  GP.AddPlot(MxWidInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(MxWidNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("levWidth-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Level with maximum width / Depth");
  GP.AddPlot(MxLevInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(MxLevNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("levInc-%s", OutFNm.CStr()), Desc);
  GP.SetXYLabel("Fraction of observed data (P)", "Number of levels of increasing width / Depth");
  GP.AddPlot(IncLevInf, gpwLinesPoints, "Influence cascade", "lw 2",true, false, false, false, false, false);
  GP.AddPlot(IncLevNet, gpwLinesPoints, "Network cascade", "lw 2",true, false, false, false, false, false);
  GP.SavePng(); }
}

// simulate SI model cascade using infection probability Beta until the cascade reaches size CascSz
PNGraph RunSICascade(PUNGraph G, const double& Beta, const int& CascSz, TIntH& NIdInfTmH) {
  PNGraph Casc = TNGraph::New();
  const int StartId = G->GetRndNId();
  Casc->AddNode(StartId);
  NIdInfTmH.AddDat(StartId, NIdInfTmH.Len());
  for (int X = 0; X < 10*CascSz; X++) {
    TIntV CascNIdV;  Casc->GetNIdV(CascNIdV);
    for (int n = 0; n < CascNIdV.Len(); n++) {
      const TUNGraph::TNodeI NI = G->GetNI(CascNIdV[n]);
      for (int i = 0; i < NI.GetOutDeg(); i++) {
        if (Casc->IsNode(NI.GetOutNId(i))) { continue; }
        if (TInt::Rnd.GetUniDev() < Beta) {
          Casc->AddNode(NI.GetOutNId(i));
          NIdInfTmH.AddDat(NI.GetOutNId(i), NIdInfTmH.Len());
          Casc->AddEdge(NI.GetId(), NI.GetOutNId(i));
          if (Casc->GetNodes() == CascSz) { return Casc; }
        }
      }
    }
  }
  return Casc;
}

// simulate SI model cascade using infection probability Beta until the cascade stops or reaches size MxCascSz
PNGraph RunSICascade2(PUNGraph G, const double& Beta, const int& MxCascSz, TIntH& NIdInfTmH) {
  PNGraph Casc = TNGraph::New();
  const int StartNId = G->GetRndNId();
  Casc->AddNode(StartNId);
  NIdInfTmH.AddDat(StartNId, NIdInfTmH.Len());
  TIntQ Q; Q.Push(StartNId);
  while (! Q.Empty()) {
    const TUNGraph::TNodeI NI = G->GetNI(Q.Top()); Q.Pop();
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (TInt::Rnd.GetUniDev() < Beta && ! NIdInfTmH.IsKey(NI.GetOutNId(i))) {
        Casc->AddNode(NI.GetOutNId(i));
        NIdInfTmH.AddDat(NI.GetOutNId(i), NIdInfTmH.Len());
        Casc->AddEdge(NI.GetId(), NI.GetOutNId(i));
        if (Casc->GetNodes() == MxCascSz) { return Casc; }
        Q.Push(NI.GetOutNId(i));
      }
    }
  }
  return Casc;
}

// network cascade: add spurious edges
// for more details see "Correcting for Missing Data in Information Cascades" by E. Sadikov, M. Medina, J. Leskovec, H. Garcia-Molina. WSDM, 2011
PNGraph AddSpuriousEdges(const PUNGraph& Graph, const PNGraph& Casc, TIntH NIdTmH) {
  TIntPrV EdgeV;
  for (TNGraph::TNodeI NI = Casc->BegNI(); NI < Casc->EndNI(); NI++) {
    TUNGraph::TNodeI GNI = Graph->GetNI(NI.GetId());
    const int Tm = NIdTmH.GetDat(NI.GetId());
    for (int i=0,j=0; i < GNI.GetOutDeg(); i++) {
      const int Dst = GNI.GetOutNId(i);
      if (NIdTmH.IsKey(Dst) && Tm<NIdTmH.GetDat(Dst) && ! NI.IsNbhNId(Dst)) {
        EdgeV.Add(TIntPr(GNI.GetId(), Dst)); }
    }
  }
  PNGraph NetCasc = TNGraph::New();
  *NetCasc = *Casc;
  for (int e = 0; e < EdgeV.Len(); e++) {
    NetCasc->AddEdge(EdgeV[e].Val1, EdgeV[e].Val2); }
  return NetCasc;
}
