namespace TSnap {

/////////////////////////////////////////////////
// Plot graph properties
template <class PGraph> void PlotInDegDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr(), const bool& PlotCCdf=false, const bool& PowerFit=false);
template <class PGraph> void PlotOutDegDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr(), const bool& PlotCCdf=false, const bool& PowerFit=false);
template <class PGraph> void PlotWccDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());
template <class PGraph> void PlotSccDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());
template <class PGraph> void PlotClustCf(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());
template <class PGraph> void PlotHops(const PGraph& Graph, const TStr& FNmPref, const TStr& DescStr, const bool& IsDir=false, const int& NApprox=32);
template <class PGraph> void PlotShortPathDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr(), int TestNodes=TInt::Mx);

// Spectral graph properties
void PlotEigValRank(const PUNGraph& Graph, const int& EigVals, const TStr& FNmPref, TStr DescStr=TStr());
void PlotEigValDistr(const PUNGraph& Graph, const int& EigVals, const TStr& FNmPref, TStr DescStr=TStr());
void PlotInvParticipRat(const PUNGraph& Graph, const int& MaxEigVecs, const int& TimeLimit, const TStr& FNmPref, TStr DescStr=TStr());

void PlotSngValRank(const PNGraph& Graph, const int& SngVals, const TStr& FNmPref, TStr DescStr=TStr());
void PlotSngValDistr(const PNGraph& Graph, const int& SngVals, const TStr& FNmPref, TStr DescStr=TStr());
void PlotSngVec(const PNGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());

/////////////////////////////////////////////////
// Implementation
template <class PGraph>
void PlotInDegDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr, const bool& PlotCCdf, const bool& PowerFit) {
  TIntPrV DegCntV;
  TSnap::GetInDegCnt(Graph, DegCntV);
  const double AvgDeg = 2*Graph->GetEdges()/double(Graph->GetNodes());
  int AboveAvg=0, Above2Avg=0;
  for (int i = 0; i < DegCntV.Len(); i++) {
    if (DegCntV[i].Val1 > AvgDeg) { AboveAvg += DegCntV[i].Val2; }
    if (DegCntV[i].Val1 > 2*AvgDeg) { Above2Avg += DegCntV[i].Val2; }
  }
  if (PlotCCdf) {
    DegCntV = TGUtil::GetCCdf(DegCntV); }
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(DegCntV, (PlotCCdf?"inDegC.":"inDeg.")+FNmPref,
    TStr::Fmt("%s. G(%d, %d). %d (%.4f) nodes with in-deg > avg deg (%.1f), %d (%.4f) with >2*avg.deg", DescStr.CStr(),
      Graph->GetNodes(), Graph->GetEdges(), AboveAvg, AboveAvg/double(Graph->GetNodes()), AvgDeg, Above2Avg, Above2Avg/double(Graph->GetNodes())),
    "In-degree", PlotCCdf?"Count (CCDF)":"Count", gpsLog10XY, PowerFit, gpwLinesPoints);
}

template <class PGraph>
void PlotOutDegDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr, const bool& PlotCCdf, const bool& PowerFit) {
  TIntPrV DegCntV;
  TSnap::GetOutDegCnt(Graph, DegCntV);
  const double AvgDeg = 2*Graph->GetEdges()/double(Graph->GetNodes());
  int AboveAvg=0, Above2Avg=0;
  for (int i = 0; i < DegCntV.Len(); i++) {
    if (DegCntV[i].Val1 > AvgDeg) { AboveAvg += DegCntV[i].Val2; }
    if (DegCntV[i].Val1 > 2*AvgDeg) { Above2Avg += DegCntV[i].Val2; }
  }
  if (PlotCCdf) {
    DegCntV = TGUtil::GetCCdf(DegCntV); }
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(DegCntV, (PlotCCdf?"outDegC.":"outDeg.")+FNmPref,
    TStr::Fmt("%s. G(%d, %d). %d (%.4f) nodes with out-deg > avg deg (%.1f), %d (%.4f) with >2*avg.deg", DescStr.CStr(),
      Graph->GetNodes(), Graph->GetEdges(), AboveAvg, AboveAvg/double(Graph->GetNodes()), AvgDeg, Above2Avg, Above2Avg/double(Graph->GetNodes())),
    "Out-degree", PlotCCdf?"Count (CCDF)":"Count", gpsLog10XY, PowerFit, gpwLinesPoints);
}

template <class PGraph>
void PlotWccDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr) {
  TIntPrV WccSzCnt;
  TSnap::GetWccSzCnt(Graph, WccSzCnt);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot GnuPlot("wcc."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest component has %f nodes",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(), WccSzCnt.Last().Val1/double(Graph->GetNodes())));
  GnuPlot.AddPlot(WccSzCnt, gpwLinesPoints, "", "pt 6");
  GnuPlot.SetXYLabel("Size of weakly connected component", "Number of components");
  GnuPlot.SetScale(gpsLog10XY);
  GnuPlot.SavePng();
}

template <class PGraph>
void PlotSccDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr) {
  TIntPrV SccSzCnt;
  TSnap::GetSccSzCnt(Graph, SccSzCnt);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot GnuPlot("scc."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest component has %f nodes",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(), SccSzCnt.Last().Val1/double(Graph->GetNodes())));
  GnuPlot.AddPlot(SccSzCnt, gpwLinesPoints, "", "pt 6");
  GnuPlot.SetXYLabel("Size of strongly connected component", "Number of components");
  GnuPlot.SetScale(gpsLog10XY);
  GnuPlot.SavePng();
}

template <class PGraph>
void PlotClustCf(const PGraph& Graph, const TStr& FNmPref, TStr DescStr) {
  TFltPrV DegToCCfV;
  int ClosedTriads, OpenTriads;
  const double CCF = GetClustCf(Graph, DegToCCfV, ClosedTriads, OpenTriads);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot GnuPlot("ccf."+FNmPref,
    TStr::Fmt("%s. G(%d, %d). Average clustering: %.4f  OpenTriads: %d (%.4f)  ClosedTriads: %d (%.4f)", DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(),
    CCF, OpenTriads, OpenTriads/double(OpenTriads+ClosedTriads), ClosedTriads, ClosedTriads/double(OpenTriads+ClosedTriads)));
  GnuPlot.AddPlot(DegToCCfV, gpwLinesPoints, "", "pt 6");
  GnuPlot.SetXYLabel("Node degree", "Average clustering coefficient");
  GnuPlot.SetScale(gpsLog10XY);
  GnuPlot.SavePng();
}

template <class PGraph>
void PlotHops(const PGraph& Graph, const TStr& FNmPref, const TStr& DescStr, const bool& IsDir, const int& NApprox) {
  TIntFltKdV DistNbhsV;
  TSnap::GetAnf(Graph, DistNbhsV, -1, IsDir, NApprox);
  const double EffDiam = TAnf::CalcEffDiam(DistNbhsV, 0.9);
  TGnuPlot GnuPlot("hop."+FNmPref, TStr::Fmt("%s. Hop plot. EffDiam: %g, G(%d, %d)",
    DescStr.CStr(), EffDiam, Graph->GetNodes(), Graph->GetEdges()));
  GnuPlot.SetXYLabel("Number of hops", "Number of pairs of nodes");
  GnuPlot.SetScale(gpsLog10Y);
  GnuPlot.AddPlot(DistNbhsV, gpwLinesPoints, "");
  GnuPlot.SavePng();
}

template <class PGraph>
void PlotShortPathDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr, int TestNodes) {
  TIntH DistToCntH;
  TBreathFS<PGraph> BFS(Graph);
  // shotest paths
  TIntV NodeIdV;
  Graph->GetNIdV(NodeIdV);  NodeIdV.Shuffle(TInt::Rnd);
  for (int tries = 0; tries < TMath::Mn(TestNodes, Graph->GetNodes()); tries++) {
    const int NId = NodeIdV[tries];
    BFS.DoBfs(NId, true, false, -1, TInt::Mx);
    for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
      DistToCntH.AddDat(BFS.NIdDistH[i]) += 1; }
  }
  DistToCntH.SortByKey(true);
  TFltPrV DistNbhsPdfV;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbhsPdfV.Add(TFltPr(DistToCntH.GetKey(i)(), DistToCntH[i]()));
  }
  const double EffDiam = TAnf::CalcEffDiamPdf(DistNbhsPdfV, 0.9);
  const double AvgDiam = TAnf::CalcAvgDiamPdf(DistNbhsPdfV);
  const int FullDiam = (int) DistNbhsPdfV.Last().Val1;
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(DistNbhsPdfV, "diam."+FNmPref,
    TStr::Fmt("%s. G(%d, %d). Diam: avg:%.2f  eff:%.2f  max:%d", DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(),
    AvgDiam, EffDiam, FullDiam), "Number of hops", "Number of shortest paths", gpsLog10Y, false, gpwLinesPoints);
}

} // namespace TSnap
