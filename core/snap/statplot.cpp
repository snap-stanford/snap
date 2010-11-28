namespace TSnap {

/////////////////////////////////////////////////
// Spectral graph properties
void PlotEigValRank(const PUNGraph& Graph, const int& EigVals, const TStr& FNmPref, TStr DescStr) {
  TFltV EigValV;
  TSnap::GetEigVals(Graph, EigVals, EigValV);
  EigValV.Sort(false);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(EigValV, "eigVal."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest eig val = %f",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(), EigValV[0].Val), "Rank", "Eigen value", gpsLog10XY, false, gpwLinesPoints);
}

void PlotEigValDistr(const PUNGraph& Graph, const int& EigVals, const TStr& FNmPref, TStr DescStr) {
  const int NBuckets = 50;
  TFltV EigValV;
  for (int f = 1; EigValV.Empty() && f < 4; f++) {
    TSnap::GetEigVals(Graph, f*EigVals, EigValV);
  }
  EigValV.Sort(true);
  THash<TFlt, TFlt> BucketCntH;
  double Step = (EigValV.Last()-EigValV[0]) / double(NBuckets-1);
  for (int i = 0; i < NBuckets; i++) {
    BucketCntH.AddDat(EigValV[0]+Step*(i+0.5), 0);
  }
  for (int i = 0; i < EigValV.Len(); i++) {
    const int Bucket = (int) floor((EigValV[i]-EigValV[0]) / Step);
    BucketCntH[Bucket] += 1;
  }
  TFltPrV EigCntV;
  BucketCntH.GetKeyDatPrV(EigCntV);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(EigCntV, "eigDistr."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest eig val = %f", DescStr.CStr(),
    Graph->GetNodes(), Graph->GetEdges(), EigValV.Last().Val), "Eigen value", "Count", gpsAuto, false, gpwLinesPoints);
}

// Inverse participation ratio: normalize EigVec to have L2=1 and then I=sum_k EigVec[i]^4
// see Spectra of "real-world" graphs: Beyond the semicircle law by Farkas, Derenyi, Barabasi and Vicsek
void PlotInvParticipRat(const PUNGraph& Graph, const int& MaxEigVecs, const int& TimeLimit, const TStr& FNmPref, TStr DescStr) {
  TFltPrV EigIprV;
  GetInvParticipRat(Graph, MaxEigVecs, TimeLimit, EigIprV);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  if (EigIprV.Empty()) { DescStr+=". FAIL"; EigIprV.Add(TFltPr(-1,-1)); return; }
  TGnuPlot::PlotValV(EigIprV, "eigIPR."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest eig val = %f (%d values)",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(), EigIprV.Last().Val1(), EigIprV.Len()),
    "Eigenvalue", "Inverse Participation Ratio of corresponding Eigenvector", gpsLog10Y, false, gpwPoints);
}

void PlotSngValRank(const PNGraph& Graph, const int& SngVals, const TStr& FNmPref, TStr DescStr) {
  TFltV SngValV;
  TSnap::GetSngVals(Graph, SngVals, SngValV);
  SngValV.Sort(false);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(SngValV, "sngVal."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest eig val = %f",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges(), SngValV[0].Val), "Rank", "Singular value", gpsLog10XY, false, gpwLinesPoints);
}

void PlotSngValDistr(const PNGraph& Graph, const int& SngVals, const TStr& FNmPref, TStr DescStr) {
  const int NBuckets = 50;
  TFltV SngValV;
  for (int f = 1; SngValV.Empty() && f < 4; f++) {
    TSnap::GetSngVals(Graph, f*SngVals, SngValV);
  }
  SngValV.Sort(true);
  THash<TFlt, TFlt> BucketCntH;
  double Step = (SngValV.Last()-SngValV[0]) / double(NBuckets-1);
  for (int i = 0; i < NBuckets; i++) {
    BucketCntH.AddDat(SngValV[0]+Step*(i+0.5), 0);
  }
  for (int i = 0; i < SngValV.Len(); i++) {
    const int Bucket = (int) floor((SngValV[i]-SngValV[0]) / Step);
    BucketCntH[Bucket] += 1;
  }
  TFltPrV EigCntV;
  BucketCntH.GetKeyDatPrV(EigCntV);
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGnuPlot::PlotValV(EigCntV, "sngDistr."+FNmPref, TStr::Fmt("%s. G(%d, %d). Largest eig val = %f", DescStr.CStr(),
    Graph->GetNodes(), Graph->GetEdges(), SngValV.Last().Val), "Singular value", "Count", gpsAuto, false, gpwLinesPoints);
}

void PlotSngVec(const PNGraph& Graph, const TStr& FNmPref, TStr DescStr) {
  TFltV LeftSV, RightSV;
  TSnap::GetSngVec(Graph, LeftSV, RightSV);
  LeftSV.Sort(false);
  RightSV.Sort(false);
  TFltV BinV;
  if (DescStr.Empty()) { DescStr = FNmPref; }
  TGUtil::MakeExpBins(LeftSV, BinV, 1.01);
  TGnuPlot::PlotValV(BinV, "sngVecL."+FNmPref, TStr::Fmt("%s. G(%d, %d). Left signular vector",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges()), "Rank", "Component of left singular vector", gpsLog10XY, false, gpwLinesPoints);
  TGnuPlot::PlotValV(BinV, "sngVecL."+FNmPref, TStr::Fmt("%s. G(%d, %d). Right signular vector",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges()), "Rank", "Component of right singular vector", gpsLog10XY, false, gpwLinesPoints);
}

} // namespace TSnap
