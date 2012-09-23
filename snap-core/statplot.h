namespace TSnap {

/////////////////////////////////////////////////
// Plot graph properties

/// Plots the in-degree distribution of a Graph.
/// @param PlotCCdf Plots the distribution as a Complementary Cummulative distribution function.
/// @param PowerFit Fits a Power-Law to the distribution.
template <class PGraph> void PlotInDegDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr(), const bool& PlotCCdf=false, const bool& PowerFit=false);
/// Plots the out-degree distribution of a Graph.
/// @param PlotCCdf Plots the distribution as a Complementary Cumulative Distribution Function (CCDF).
/// @param PowerFit Fits a Power-Law to the distribution.
template <class PGraph> void PlotOutDegDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr(), const bool& PlotCCdf=false, const bool& PowerFit=false);
/// Plots the distribution of sizes of weakly connected components of a Graph.
template <class PGraph> void PlotWccDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the distribution of sizes of strongly connected components of a Graph.
template <class PGraph> void PlotSccDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the distribution of clustering coefficient of a Graph.
template <class PGraph> void PlotClustCf(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the cumulative distribution of the shortest path lengths of a Graph. Implementation is based on ANF.
/// @param IsDir false: ignore edge directions and consider graph as undirected.
template <class PGraph> void PlotHops(const PGraph& Graph, const TStr& FNmPref, const TStr& DescStr, const bool& IsDir=false, const int& NApprox=32);
/// Plots the distribution of the shortest path lengths of a Graph. Implementation is based on BFS.
template <class PGraph> void PlotShortPathDistr(const PGraph& Graph, const TStr& FNmPref, TStr DescStr=TStr(), int TestNodes=TInt::Mx);

/// Plots the eigen-value rank distribution of the Graph adjacency matrix. Plots first EigVals eigenvalues.
void PlotEigValRank(const PUNGraph& Graph, const int& EigVals, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the distribution of components of the leading eigen-vector of the Graph adjacency matrix. Plots first EigVals values.
void PlotEigValDistr(const PUNGraph& Graph, const int& EigVals, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the inverse participation ratio.
/// See Spectra of "real-world" graphs: Beyond the semicircle law by Farkas, Derenyi, Barabasi and Vicsek.
void PlotInvParticipRat(const PUNGraph& Graph, const int& MaxEigVecs, const int& TimeLimit, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the rank distribution of singular values of the Graph adjacency matrix. Plots first SngVals values.
void PlotSngValRank(const PNGraph& Graph, const int& SngVals, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the rank distribution of singular values of the Graph adjacency matrix. Plots first SngVals values.
void PlotSngValDistr(const PNGraph& Graph, const int& SngVals, const TStr& FNmPref, TStr DescStr=TStr());
/// Plots the distribution of the values of the leading left singular vector of the Graph adjacency matrix. Plots first SngVals values.
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
  TIntFltKdV DistNbrsV;
  TSnap::GetAnf(Graph, DistNbrsV, -1, IsDir, NApprox);
  const double EffDiam = TSnap::TSnapDetail::CalcEffDiam(DistNbrsV, 0.9);
  TGnuPlot GnuPlot("hop."+FNmPref, TStr::Fmt("%s. Hop plot. EffDiam: %g, G(%d, %d)",
    DescStr.CStr(), EffDiam, Graph->GetNodes(), Graph->GetEdges()));
  GnuPlot.SetXYLabel("Number of hops", "Number of pairs of nodes");
  GnuPlot.SetScale(gpsLog10Y);
  GnuPlot.AddPlot(DistNbrsV, gpwLinesPoints, "");
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
  TFltPrV DistNbrsPdfV;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbrsPdfV.Add(TFltPr(DistToCntH.GetKey(i)(), DistToCntH[i]()));
  }
  const double EffDiam = TSnap::TSnapDetail::CalcEffDiamPdf(DistNbrsPdfV, 0.9);
  const double AvgDiam = TSnap::TSnapDetail::CalcAvgDiamPdf(DistNbrsPdfV);
  const int FullDiam = (int) DistNbrsPdfV.Last().Val1;
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(DistNbrsPdfV, "diam."+FNmPref,
    TStr::Fmt("%s. G(%d, %d). Diam: avg:%.2f  eff:%.2f  max:%d", DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(),
    AvgDiam, EffDiam, FullDiam), "Number of hops", "Number of shortest paths", gpsLog10Y, false, gpwLinesPoints);
}

} // namespace TSnap
