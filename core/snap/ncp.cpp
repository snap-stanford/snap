#include "stdafx.h"
#include "ncp.h"

//////////////////////////////////////////////////
// Local Spectral Clustering

bool TLocClust::Verbose = true;

int TLocClust::ApproxPageRank(const int& StartNId, const double& Eps) {
  for (int i = 0; i < ProbH.Len(); i++) { ProbH[i]=0.0; }
  for (int i = 0; i < ResH.Len(); i++) { ResH[i]=0.0; }
  ProbH.Clr(false, -1, false);
  ResH.Clr(false, -1, false);
  ResH.AddDat(StartNId, 1.0);
  int iter = 0;
  double OldRes = 0.0;
  NodeQ.Clr(false);
  NodeQ.Push(StartNId);
  TExeTm ExeTm;
  while (! NodeQ.Empty()) {
    const int NId = NodeQ.Top(); NodeQ.Pop();
    const TUNGraph::TNodeI& Node = Graph->GetNI(NId);
    const int NIdDeg = Node.GetOutDeg();
    const double PushVal = ResH.GetDat(NId) - 0.5*Eps*NIdDeg;
    const double PutVal = (1.0-Alpha) * PushVal / double(NIdDeg);
    ProbH.AddDat(NId) += Alpha*PushVal;
    ResH.AddDat(NId) = 0.5 * Eps * NIdDeg;
    for (int e = 0; e < NIdDeg; e++) {
      const int DstNId = Node.GetOutNId(e);
      const int DstDeg = Graph->GetNI(DstNId).GetOutDeg();
      double& ResVal = ResH.AddDat(DstNId).Val;
      OldRes = ResVal;
      ResVal += PutVal;
      if (ResVal >= Eps*DstDeg && OldRes < Eps*DstDeg) {
        NodeQ.Push(DstNId); }
    }
    iter++;
    if (iter % Mega(1) == 0) { 
      printf(" %d[%s]", NodeQ.Len(), ExeTm.GetStr());
      if (iter/1000 > Graph->GetNodes() || ExeTm.GetSecs() > 4*3600) { // more than 2 hours
        printf("Too many iterations! Stop to save time.\n");
        return iter; }
    }
  }
  // check that the residuals are sufficiently small
  /*for (int i =0; i < ProbH.Len(); i++) {
    const int Deg = Graph->GetNI(ResH.GetKey(i)).GetOutDeg();
    IAssert(ResH[i] < Eps*Deg); } //*/
  return iter;
}

void TLocClust::SupportSweep() {
  TExeTm ExeTm;
  VolV.Clr(false);  CutV.Clr(false);  PhiV.Clr(false);
  if (ProbH.Empty()) { return; }
  const int TopNId = ProbH.GetKey(0);
  const int TopNIdDeg = Graph->GetNI(TopNId).GetOutDeg();
  int Vol = TopNIdDeg, Cut = TopNIdDeg;
  double Phi = Cut/double(Vol);
  VolV.Add(Vol);  CutV.Add(Cut);  PhiV.Add(1.0);
  for (int i = 1; i < ProbH.Len(); i++) {
    const int NId = ProbH.GetKey(i);
    const TUNGraph::TNodeI& Node = Graph->GetNI(NId);
    const int OutDeg = Node.GetOutDeg();
    int CutSz = OutDeg; // edges outside
    for (int e = 0; e < OutDeg; e++) {
      const int Rank = ProbH.GetKeyId(Node.GetOutNId(e));
      if ( Rank > -1 && Rank < i) { CutSz -= 2;  }
    }
    Vol += OutDeg;  Cut += CutSz;
    if (Vol < Edges2) {
      if (2*Vol > Edges2) { Phi = Cut / double(Edges2-Vol); }
      else { Phi = Cut / double(Vol); }
    } else {
      Phi = 1.0;
    }
    IAssert((Phi+1e-6) >= double(1.0)/double(i*(i+1)+1)); // conductance is worse than the best possible
    VolV.Add(Vol);  CutV.Add(Cut);  PhiV.Add(Phi);
  }}

void TLocClust::FindBestCut(const int& SeedNode, const int& ClustSz, const double& MinSizeFrac) {
  double MaxPhi = TFlt::Mx;
  BestCutIdx = -1;
  SeedNId = SeedNode;
  // calculate pagerank and cut sets
  ApproxPageRank(SeedNId, 1.0/double(ClustSz));
  for (int i = 0; i < ProbH.Len(); i++) {
    ProbH[i] /= Graph->GetNI(ProbH.GetKey(i)).GetOutDeg(); }
  ProbH.SortByDat(false);
  SupportSweep();
  // find best cut
  NIdV.Clr(false);
  for (int i = 0; i < PhiV.Len(); i++) {
    const double Phi = PhiV[i];
    NIdV.Add(ProbH.GetKey(i));
    if (Phi < MaxPhi) { MaxPhi = Phi;  BestCutIdx = i; }
  }
}

void TLocClust::PlotVolDistr(const TStr& OutFNm, TStr Desc) const {
  TFltPrV RankValV(VolV.Len(), 0);
  for (int i = 0; i < VolV.Len(); i++) {
    RankValV.Add(TFltPr(i+1, VolV[i].Val)); }
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV NewV;  TLocClustStat::MakeExpBins(RankValV, NewV);
  TGnuPlot GP(OutFNm+"-VOL", TStr::Fmt("VOLUME. Seed node %d.", SeedNId, Desc.CStr()));
  GP.AddPlot(NewV, gpwLinesPoints, ""); //, "pointtype 6 pointsize 1.5"
  GP.SetXYLabel("Rank", "Volume");
  //GP.SetScale(gpsLog10X);
  GP.SavePng();
}

void TLocClust::PlotCutDistr(const TStr& OutFNm, TStr Desc) const {
  TFltPrV RankValV(CutV.Len(), 0);
  for (int i = 0; i < CutV.Len(); i++) {
    RankValV.Add(TFltPr(i+1, CutV[i].Val)); }
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV NewV;  TLocClustStat::MakeExpBins(RankValV, NewV);
  TGnuPlot GP(OutFNm+"-CUT", TStr::Fmt("CUT SIZE. Seed node %d.", SeedNId, Desc.CStr()));
  GP.AddPlot(NewV, gpwLinesPoints, ""); //, "pointtype 6 pointsize 1.5"
  GP.SetXYLabel("Rank", "Cut size");
  //GP.SetScale(gpsLog10X);
  GP.SavePng();
}

void TLocClust::PlotPhiDistr(const TStr& OutFNm, TStr Desc) const {
  TFltPrV RankValV(PhiV.Len(), 0);
  for (int i = 0; i < PhiV.Len(); i++) {
    RankValV.Add(TFltPr(i+1, PhiV[i])); }
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV NewV;  TLocClustStat::MakeExpBins(RankValV, NewV);
  TGnuPlot GP(OutFNm+"-PHI", TStr::Fmt("CONDUCTANCE (Cut size / Volume). Seed node %d.", SeedNId, Desc.CStr()));
  GP.AddPlot(NewV, gpwLinesPoints, ""); //, "pointtype 6 pointsize 1.5"
  GP.SetXYLabel("Rank", "Conductance (Cut size / Volume)");
  //GP.SetScale(gpsLog10X);
  GP.SavePng();
}

void TLocClust::SavePajek(const TStr& OutFNm) const {
  FILE *F = fopen(TStr::Fmt("%s.net", OutFNm.CStr()).CStr(), "wt");
  TIntH NIdToIdH(Graph->GetNodes(), true);
  TIntH ClustSet(BestCut()+1);
  const int BucketSz = BestCutNodes()/ 4;
  TStrV ClrV = TStrV::GetV("Black", "Gray80", "Gray60", "Gray40", "Gray20", "RedViolet");
  for (int a = 0; a < BestCutNodes(); a++) {
    ClustSet.AddDat(NIdV[a], (a+1)/BucketSz);
  }
  fprintf(F, "*Vertices %d\n", Graph->GetNodes());
  int i = 0;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int NId = NI.GetId();
    if (NId == SeedNId) {
      fprintf(F, "%d  \"%d\" diamond x_fact 2 y_fact 2 ic Green fos 10\n", i+1, NId); }
    else if (ClustSet.IsKey(NId)) {
      //fprintf(F, "%d  \"%d\" box x_fact 1 y_fact 1 ic Red fos 10\n", i+1, NId); }
      fprintf(F, "%d  \"%d\" box x_fact 2 y_fact 2 ic %s fos 10\n", i+1, NId, ClrV[ClustSet.GetDat(NId)].CStr()); }
    else {
      fprintf(F, "%d  \"%d\" ellipse x_fact 2 y_fact 2 ic Blue fos 10\n", i+1, NId); }
    NIdToIdH.AddDat(NId, i+1);
    i++;
  }
  fprintf(F, "*Arcs %d\n", Graph->GetEdges()); // arcs are directed, edges are undirected
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int NId = NIdToIdH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      fprintf(F, "%d %d %g c Tan\n", NId, NIdToIdH.GetDat(NI.GetOutNId(e)).Val, 1.0);
    }
  }
  fclose(F);
}

void TLocClust::DrawWhiskers(const PUNGraph& Graph, TStr FNmPref, const int& PlotN=10) {
  TCnComV CnComV;  TSnap::Get1CnCom(Graph, CnComV);
  CnComV.Sort(false);
  // plot size distribution
  { TIntH SzCntH;
  for (int i = 0; i < CnComV.Len(); i++) { SzCntH.AddDat(CnComV[i].Len()) += 1; }
  TGnuPlot::PlotValCntH(SzCntH, "whiskSz."+FNmPref, TStr::Fmt("%s. G(%d, %d)", FNmPref.CStr(), Graph->GetNodes(),
    Graph->GetEdges()), "Whisker size (Maximal component connected with a bridge edge)", "Count", gpsLog10XY, false); }
  // draw them
  int BrNodeId = -1;
  for (int c = 0; c < TMath::Mn(CnComV.Len(), PlotN); c++) {
    const PUNGraph BrClust = TSnap::GetSubGraph(Graph, CnComV[c].NIdV);
    for (TUNGraph::TNodeI NI = BrClust->BegNI(); NI < BrClust->EndNI(); NI++) {
      if (NI.GetOutDeg() != Graph->GetNI(NI.GetId()).GetOutDeg()) { BrNodeId=NI.GetId(); break; } }
    TIntStrH ClrH;  ClrH.AddDat(BrNodeId, "red");
    TGraphViz::Plot(BrClust, gvlNeato, TStr::Fmt("whisk-%s-%02d.gif", FNmPref.CStr(), c+1),
      TStr::Fmt("Bridge node id: %d, n=%d, e=%d", BrNodeId, BrClust->GetNodes(), BrClust->GetEdges()), false, ClrH);
  }
}

void TLocClust::GetCutStat(const PUNGraph& Graph, const TIntV& NIdV, int& Vol, int& Cut, double& Phi, int GraphEdges) {
  TIntSet NIdSet(NIdV.Len());
  for (int i = 0; i < NIdV.Len(); i++) { NIdSet.AddKey(NIdV[i]); }
  GetCutStat(Graph, NIdSet, Vol, Cut, Phi, GraphEdges);
}

void TLocClust::GetCutStat(const PUNGraph& Graph, const TIntSet& NIdSet, int& Vol, int& Cut, double& Phi, int GraphEdges) {
  const int Edges2 = GraphEdges>=0 ? 2*GraphEdges : Graph->GetEdges();
  Vol=0;  Cut=0; Phi=0.0;
  for (int i = 0; i < NIdSet.Len(); i++) {
    if (! Graph->IsNode(NIdSet[i])) { continue; }
    TUNGraph::TNodeI NI = Graph->GetNI(NIdSet[i]);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (! NIdSet.IsKey(NI.GetOutNId(e))) { Cut += 1; }
    }
    Vol += NI.GetOutDeg();
  }
  // get conductance
  if (Vol != Edges2) {
    if (2*Vol > Edges2) { Phi = Cut / double(Edges2-Vol); }
    else if (Vol == 0) { Phi = 0.0; }
    else { Phi = Cut / double(Vol); }
  } else {
    if (Vol == Edges2) { Phi = 1.0; }
  }
}

void TLocClust::PlotNCP(const PUNGraph& Graph, const TStr& FNm, const TStr Desc, const bool& BagOfWhiskers, const bool& RewireNet, const int& KMin, const int& KMax, const int& Coverage, const bool& SaveTxtStat, const bool& PlotBoltzman) {
  const double Alpha = 0.001, KFac = 1.5, SizeFrac = 0.001;
  //const int KMin = 2, KMax = Mega(100), Coverage = 10;
  TLocClustStat ClusStat1(Alpha, KMin, KMax, KFac, Coverage, SizeFrac);
  ClusStat1.Run(Graph, false, PlotBoltzman, SaveTxtStat);
  if (BagOfWhiskers) { ClusStat1.AddBagOfWhiskers(); }
  TLocClustStat ClusStat2(Alpha, KMin, KMax, KFac, Coverage, SizeFrac);
  ClusStat1.ImposeNCP(ClusStat2, FNm, Desc, "ORIGINAL", "REWIRED"); // plot before rewiring
  if (SaveTxtStat) { // for every piece size store modularity
    ClusStat1.SaveTxtInfo(FNm, Desc, false);
  }
  if (PlotBoltzman) {
    ClusStat1.PlotBoltzmanCurve(FNm, Desc);
  }
  if (RewireNet) {
    ClusStat2.Run(TSnap::GenRewire(Graph, 50, TInt::Rnd), false, false, false);
    if (BagOfWhiskers) { ClusStat2.AddBagOfWhiskers(); }
    ClusStat1.ImposeNCP(ClusStat2, FNm, Desc, "ORIGINAL", "REWIRED"); // if rewire, plot again
  }
}

//////////////////////////////////////////////////
// Local Clustering Statistics

double TLocClustStat::BinFactor = 1.01;

double TLocClustStat::TCutInfo::GetFracDegOut(const PUNGraph& Graph, double& MxFrac, double& AvgFrac, double& MedianFrac, double& Pct9Frac, double& Flake) const {
  if (CutNIdV.Empty()) {
    IAssert(Nodes<100 || ! CutNIdV.Empty());
    MxFrac=1; AvgFrac=1; MedianFrac=1; Pct9Frac=1; Flake=1; 
    return 1;
  }
  TMom FracDegMom;
  TIntSet InNIdSet(CutNIdV.Len());
  int NHalfIn=0;
  for (int i = 0; i < CutNIdV.Len(); i++) { 
    InNIdSet.AddKey(CutNIdV[i]); }
  for (int n = 0; n < CutNIdV.Len(); n++) {
    const TUNGraph::TNodeI NI = Graph->GetNI(CutNIdV[n]);
    int EdgesOut = 0;
    for (int i = 0; i < NI.GetDeg(); i++) {
      if (! InNIdSet.IsKey(NI.GetNbhNId(i))) { EdgesOut++; }
    }
    const double FracOut = EdgesOut/double(NI.GetDeg());
    if (FracOut <= 0.5) { NHalfIn++; }
    FracDegMom.Add(FracOut);
  }
  FracDegMom.Def();
  MxFrac = FracDegMom.GetMx();
  AvgFrac = FracDegMom.GetMean();
  MedianFrac = FracDegMom.GetMedian();
  Pct9Frac = FracDegMom.GetDecile(9);
  Flake = 1.0 - double(NHalfIn)/double(CutNIdV.Len());
  return MxFrac;
}

TLocClustStat::TLocClustStat(const double& _Alpha, const int& _KMin, const int& _KMax, const double& _KFac,
  const int& _Coverage, const double& _SizeFrac) : Alpha(_Alpha), SizeFrac(_SizeFrac), KFac(_KFac),
  KMin(_KMin), KMax(_KMax), Coverage(_Coverage) {
}

void TLocClustStat::Save(TSOut& SOut) const {
  // parameters
  Alpha.Save(SOut);
  SizeFrac.Save(SOut);
  KFac.Save(SOut);
  KMin.Save(SOut);
  KMax.Save(SOut);
  Coverage.Save(SOut);
  // cluster stat
  //SweepsV.Save(SOut);
  //SizePhiH.Save(SOut);
}

void TLocClustStat::Clr() {
  SweepsV.Clr(false);      // node ids and conductances for each run of local clustering
  SizePhiH.Clr(false);     // conductances at cluster size K
  BestCutH.Clr(false);     // best cut (min conductance) at size K
  BagOfWhiskerV.Clr(false); // (Size, Conductance) of bag of whiskers clusters
}

void TLocClustStat::Run(const PUNGraph& _Graph, const bool& SaveAllSweeps, const bool& SaveAllCond, const bool& SaveBestNodesAtK) {
  Graph = TSnap::GetMxWcc(_Graph);
  const int Nodes = Graph->GetNodes();
  const int Edges = Graph->GetEdges();
  printf("\nLocal clustering: Graph(%d, %d)\n", Nodes, Edges);
  printf("  Alpha:    %g\n", Alpha());
  printf("  K: %d -- %g -- %dm\n", KMin(), KFac(), int(KMax/Mega(1)));
  printf("  Coverage: %d\n", Coverage());
  printf("  SizeFrac: %g\n\n", SizeFrac());
  TExeTm TotTm;
  Clr();
  TLocClust Clust(Graph, Alpha);
  BestCut.CutNIdV.Clr(false); 
  BestCut.CutSz=-1;  BestCut.Edges=-1;
  double BestPhi = TFlt::Mx;
  int prevK=-1;
  bool NextDone=false;
  if (SaveBestNodesAtK) { // fill buckets (only store nodes in clusters for sizes in SizeBucketSet)
    SizeBucketSet.Clr();
    double PrevBPos = 1, BPos = 1;
    while (BPos <= Mega(100)) {
      PrevBPos = (uint) floor(BPos);
      BPos *= BinFactor;
      if (floor(BPos) == PrevBPos) { BPos = PrevBPos + 1; }
      SizeBucketSet.AddKey(int(floor(BPos) - 1));
    }
  }
  for (int K = KMin, cnt=1; K < KMax; K = int(KFac * double(K))+1, cnt++) {
    if (K == prevK) { K++; } prevK = K;
    const int Runs = 2 + int(Coverage /**pow(1.1, cnt)*/ * floor(double(Graph->GetEdges()) / double(K)));
    printf("%d] K: %d, %d runs\n", cnt+1, K, Runs);
    if (NextDone) { break; } // done
    if (K+1 > 2*Graph->GetEdges()) { K = Graph->GetEdges(); NextDone=true; }
    //if (K+1 > Graph->GetEdges()) { K = Graph->GetEdges(); NextDone=true; }
    TExeTm ExeTm, IterTm;
    double MeanSz=0.0, MeanVol=0.0, Count=0.0;
    for (int run = 0; run < Runs; run++) {
      const int SeedNId = Graph->GetRndNId();  IterTm.Tick();
      Clust.FindBestCut(SeedNId, K, SizeFrac);
      const int Sz = Clust.BestCutNodes();
      const int Vol = Clust.GetCutVol();
      const double Phi = TMath::Round(Clust.GetCutPhi(), 4);
      if (Sz == 0 || Vol == 0 || Phi == 0) { continue; }
      MeanSz+=Sz;  MeanVol+=Vol;  Count+= 1;
      if (SaveAllSweeps) { // save the full cut set and conductances for all trials
        SweepsV.Add(TNodeSweep(SeedNId, Clust.GetNIdV(), Clust.GetPhiV())); }
      int SAtBestPhi=-1;
      for (int s = 0; s < Clust.Len(); s++) {
        const int size = s+1;
        const int cut = Clust.GetCut(s);
        const int edges = (Clust.GetVol(s)-cut)/2;
        const double phi = Clust.GetPhi(s);
        if (( Clust.GetPhi(s) != double(cut)/double(2*edges+cut))) { continue; } // more than half of the edges
        IAssert((Clust.GetVol(s) - cut) % 2 == 0);
        IAssert(phi == double(cut)/double(2*edges+cut));
        IAssert(phi >= 1.0/double((1+s)*s+1));
        //// If we want to take pieces that minimize some other community goodness measure
        // TCutInfo CutInfo(size, edges, cut); Clust.GetNIdV().GetSubValV(0, size-1, CutInfo.CutNIdV);
        //double MxFrac=0, AvgFrac=0, MedianFrac=0, Pct9Frac=0, Flake=0;
        //CutInfo.GetFracDegOut(Graph, MxFrac, AvgFrac, MedianFrac, Pct9Frac, Flake);
        //const double phi = MxFrac;
        if (BestPhi >= phi) {
          BestPhi = phi;
          BestCut = TCutInfo(size, edges, cut);
          SAtBestPhi = s;
        }
        //// If we want to take pieces that minimize some other community goodness measure
        //bool TAKE=false;  if (! BestCutH.IsKey(size)) { TAKE=true; }
        //else { BestCutH.GetDat(size).GetFracDegOut(Graph, MxFrac, AvgFrac, MedianFrac, Pct9Frac, Flake);  if (MxFrac >= phi) { TAKE = true; } }
        // if (TAKE) {
        if (! BestCutH.IsKey(size) || BestCutH.GetDat(size).GetPhi() >= phi) { //new best cut (size, edges inside and nodes)
          BestCutH.AddDat(size, TCutInfo(size, edges, cut));  // for every size store best cut (NIds inside the cut)
          if (SaveBestNodesAtK) { // store node ids in best community for each size k
            if (! SizeBucketSet.Empty() && ! SizeBucketSet.IsKey(size)) { continue; } // only save best clusters at SizeBucketSet
            Clust.GetNIdV().GetSubValV(0, size-1, BestCutH.GetDat(size).CutNIdV); }
        }
        if (SaveAllCond) { // for every size store all conductances
          SizePhiH.AddDat(size).Add(phi); }
      }
      if (SAtBestPhi != -1) { // take nodes in best cluster
        const int size = SAtBestPhi+1;
        Clust.GetNIdV().GetSubValV(0, size-1, BestCut.CutNIdV); 
      }
      if (TLocClust::Verbose) {
        printf(".");
        if (run % 50 == 0) {
          printf("\r                                                   %d / %d \r", run, Runs); }
      }
    }
    if (TLocClust::Verbose) {
      printf("\r  %d / %d: %s                                                   \n", Runs, Runs, ExeTm.GetStr()); 
    }
    MeanSz/=Count;  MeanVol/=Count;
    printf("  Graph(%d, %d)  ", Nodes, Edges);
    printf("       mean:  sz: %.2f  vol: %.2f [%s] %s\n", MeanSz, MeanVol, ExeTm.GetStr(), TExeTm::GetCurTm());
  }
  SizePhiH.SortByKey();
  for (int k = 0; k < SizePhiH.Len(); k++) { 
    SizePhiH[k].Sort(); }
  SweepsV.Sort();
  BestCutH.SortByKey();
  printf("DONE. Graph(%d, %d): Total run time: %s\n\n", Nodes, Edges, TotTm.GetStr());
}

void TLocClustStat::AddBagOfWhiskers() {
  BagOfWhiskers(Graph, BagOfWhiskerV, BestWhisk); // slow but exact
  //BagOfWhiskers2(Graph, BagOfWhiskerV);
}

// add a cut on NIdV nodes
void TLocClustStat::AddCut(const TIntV& NIdV) {
  int Vol, Cut; double Phi;
  TLocClust::GetCutStat(Graph, NIdV, Vol, Cut, Phi, Graph->GetEdges());
  SizePhiH.AddDat(NIdV.Len()).Add(Phi);
}

// add a cut of particular conductance
void TLocClustStat::AddCut(const int& ClustSz, const double& Phi) {
  SizePhiH.AddDat(ClustSz).Add(Phi);
}

// add the cut on particular set of nodes (calculate conductance and modularity)
void TLocClustStat::AddToBestCutH(const PUNGraph& Graph, const TIntV& NIdV, const bool& AddBestCond) {
  const int K = NIdV.Len();
  TCutInfo CutInfo(Graph, NIdV, true);
  printf("new: %d\t%d\t%d\t%f\t%f\n", CutInfo.GetNodes(), CutInfo.GetEdges(), 
    CutInfo.GetCutSz(), CutInfo.GetPhi(), CutInfo.GetModular(Graph->GetEdges()));
  if (! BestCutH.IsKey(K)) { BestCutH.AddDat(K, CutInfo);  return; }
  TCutInfo& CurCut = BestCutH.GetDat(K);
  // keep the cluster with best conductance
  if (AddBestCond && CurCut.GetPhi() > CutInfo.GetPhi()) { CurCut=CutInfo; }
  // keep the cluster with best modularity
  //const int E = Graph->GetEdges();  
  //if (! AddBestCond && CurCut.GetModular(E) < CutInfo.GetModular(E)) { CurCut=CutInfo; }
}

const TLocClustStat::TCutInfo& TLocClustStat::FindBestCut(const int& Nodes) const {
  double bestPhi = 1;
  int CutId = -1;
  if (Nodes > 0) {
    IAssert(BestCutH.IsKey(Nodes));
    return BestCutH.GetDat(Nodes);
  } else {
    for (int n = 0; n < BestCutH.Len(); n++) {
      if (BestCutH[n].GetPhi() <= bestPhi) {
        bestPhi = BestCutH[n].GetPhi();  CutId = n; }
    }
    IAssert(CutId != -1);
    IAssert(! BestCutH[CutId].CutNIdV.Empty());
    return BestCutH[CutId];
  }
}

// find the cut with the lowest Phi of particular size (if Nodes=-1 find absolute best cut)
double TLocClustStat::FindBestCut(const int& Nodes, TIntV& ClustNIdV) const {
  const TCutInfo& Cut = FindBestCut(Nodes);
  ClustNIdV = Cut.CutNIdV;
  return Cut.GetPhi();
}

int TLocClustStat::FindBestCut(const int& Nodes, int& Vol, int& Cut, double& Phi) const {
  const TCutInfo& CutInfo = FindBestCut(Nodes);
  Vol = CutInfo.GetVol();
  Cut = CutInfo.GetCutSz();
  Phi = CutInfo.GetPhi();
  return CutInfo.GetNodes();
}

// find best phi where the cut has N nodes, and the nodes in the cluster are not in the TabuSet
double TLocClustStat::FindBestCut(const int& Nodes, const TIntSet& TabuNIdSet, int& BestCutId) const {
  double bestPhi = 1;
  BestCutId = -1;
  bool Tabu;
  IAssert(! SweepsV.Empty());
  for (int c = 0; c < SweepsV.Len(); c++) {
    const TNodeSweep& Sweep = SweepsV[c];
    if (Sweep.Len() < Nodes) { continue; }
    if (Sweep.Phi(Nodes-1) > bestPhi) { continue; }
    Tabu = false;
    for (int i = 0; i < Nodes; i++) {
      if (TabuNIdSet.IsKey(Sweep.NId(i))) { Tabu=true; break; }
    }
    if (Tabu) { continue; }
    bestPhi = Sweep.Phi(Nodes-1);
    BestCutId = c;
  }
  return bestPhi;
}

// get statistics over all conductances at all cluster sizes
void TLocClustStat::GetCurveStat(TFltPrV& MeanV, TFltPrV& MedV, TFltPrV& Dec1V, TFltPrV& MinV, TFltPrV& MaxV) const {
  TFltPrV BucketV;
  MeanV.Clr(false); MedV.Clr(false); Dec1V.Clr(false); MinV.Clr(false); MaxV.Clr(false);
  if (! SizePhiH.Empty()) { // stores conductances of all little clusters
    const THash<TInt, TFltV>& KvH = SizePhiH; 
    for (int i = 0; i < KvH.Len(); i++) {
      const double X = KvH.GetKey(i).Val;  IAssert(X >= 1.0);
      const TFltV& YVec = KvH[i];
      TMom Mom;
      for (int j = 0; j < YVec.Len(); j++) { Mom.Add(YVec[j]); }
      Mom.Def();
      /*IAssert(Mom.GetMean()>=0 && Mom.GetMean()<=1);
      IAssert(Mom.GetMedian()>=0 && Mom.GetMedian()<=1);
      IAssert(Mom.GetDecile(1)>=0 && Mom.GetDecile(1)<=1);
      IAssert(Mom.GetMn()>=0 && Mom.GetMn()<=1);
      IAssert(Mom.GetMx()>=0 && Mom.GetMx()<=1);*/
      MeanV.Add(TFltPr(X, Mom.GetMean()));
      MedV.Add(TFltPr(X, Mom.GetMedian()));
      Dec1V.Add(TFltPr(X, Mom.GetDecile(1)));
      MinV.Add(TFltPr(X, Mom.GetMn()));
      MaxV.Add(TFltPr(X, Mom.GetMx()));
    }
    MeanV.Sort(); MedV.Sort();  Dec1V.Sort();  MinV.Sort();  MaxV.Sort();
    // create log buckets (makes plots smaller and smoother)
    TLocClustStat::MakeExpBins(MeanV, BucketV);  MeanV.Swap(BucketV);
    TLocClustStat::MakeExpBins(MedV, BucketV);  MedV.Swap(BucketV);
    TLocClustStat::MakeExpBins(Dec1V, BucketV);  Dec1V.Swap(BucketV);
    TLocClustStat::MakeExpBins(MinV, BucketV);  MinV.Swap(BucketV);
    TLocClustStat::MakeExpBins(MaxV, BucketV);  MaxV.Swap(BucketV);
  } else {
    //const int GEdges = Graph->GetEdges();
    for (int i = 0; i < BestCutH.Len(); i++) {
      MinV.Add(TFltPr(BestCutH.GetKey(i).Val, BestCutH[i].GetPhi()));
    }
    MinV.Sort();
    TLocClustStat::MakeExpBins(MinV, BucketV);  MinV.Swap(BucketV);
  }
}

void TLocClustStat::GetBoltzmanCurveStat(const TFltV& TempV, TVec<TFltPrV>& NcpV) const {
  IAssert(! SizePhiH.Empty()); // stores conductances of all little clusters
  NcpV.Gen(TempV.Len());
  TFltPrV BucketV;
  for (int t = 0; t < TempV.Len(); t++) {
    const double T = TempV[t];
    for (int i = 0; i < SizePhiH.Len(); i++) {
      const double X = SizePhiH.GetKey(i).Val;  IAssert(X >= 1.0);
      const TFltV& PhiV = SizePhiH[i];
      double V = 0.0, SumW = 0.0;
      for (int j = 0; j < PhiV.Len(); j++) { 
        V += PhiV[j] * exp(-PhiV[j]/T); 
        SumW += exp(-PhiV[j]/T); 
      }
      //V /= PhiV.Len();
      V /= SumW;
      NcpV[t].Add(TFltPr(X, V));
    }
    TLocClustStat::MakeExpBins(NcpV[t], BucketV);  NcpV[t].Swap(BucketV);
    // normalize to start at (1,1)
    //for (int i = 0; i < NcpV[t].Len(); i++) {
    //  NcpV[t][i].Val2 /= NcpV[t][0].Val2;
    //}
  }
}

TStr TLocClustStat::ParamStr() const {
  if (Graph.Empty()) {
    return TStr::Fmt("A=%g, K=%d-%g-%s, Cvr=%d, SzFrc=%g", Alpha(), KMin(), KFac(), TInt::GetMegaStr(KMax()).CStr(), Coverage(), SizeFrac()); }
  else {
    return TStr::Fmt("A=%g, K=%d-%g-%s, Cvr=%d, SzFrc=%g G(%d, %d)", Alpha(), KMin(), KFac(), TInt::GetMegaStr(KMax()).CStr(), Coverage(), SizeFrac(),
      Graph->GetNodes(), Graph->GetEdges());
  }
}

void TLocClustStat::PlotNCP(const TStr& OutFNm, TStr Desc) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV MeanV, MedV, Dec1V, MinV, MaxV;
  GetCurveStat(MeanV, MedV, Dec1V, MinV, MaxV);
  TGnuPlot GP("ncp."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  if (! MaxV.Empty()) { GP.AddPlot((MaxV), gpwLines, "MAX"); }
  if (! MedV.Empty()) { GP.AddPlot((MedV), gpwLines, "MEDIAN"); }
  if (! MeanV.Empty()) { GP.AddPlot((MeanV), gpwLines, "MEAN"); }
  if (! Dec1V.Empty()) { GP.AddPlot((Dec1V), gpwLines, "1-st DECILE"); }
  if (! MinV.Empty()) { 
    GP.AddPlot((MinV), gpwLines, "MIN"); 
    //GP.AddPlot(MinV, gpwLines, "smooth MIN", "lw 2 smooth bezier");
  }
  if (! BagOfWhiskerV.Empty()) {
    GP.AddPlot(BagOfWhiskerV, gpwLines, "Whiskers", "lw 1"); 
    TFltPrV BestWhiskV;  BestWhiskV.Add(TFltPr(BestWhisk));
    GP.AddPlot(BestWhiskV, gpwPoints, "Best whisker", "pt 5 ps 2");
  }
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  GP.SetXRange(1, Graph->GetNodes()/2);
  GP.SavePng();
}

// NCP but with modularity on Y-axis
void TLocClustStat::PlotNCPModul(const TStr& OutFNm, TStr Desc) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV MinV, BucketV;
  const int GEdges = Graph->GetEdges();
  for (int i = 0; i < BestCutH.Len(); i++) {
    MinV.Add(TFltPr(BestCutH.GetKey(i).Val, BestCutH[i].GetModular(GEdges))); }
  MinV.Sort();
  TLocClustStat::MakeExpBins(MinV, BucketV);  MinV.Swap(BucketV);
  TGnuPlot GP("ncpMod."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  if (! MinV.Empty()) { 
    GP.AddPlot((MinV), gpwLines, "MIN"); }
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "Q (modularity)");
  GP.SetXRange(1, Graph->GetNodes()/2);
  GP.SavePng();
}

// plot top 10 surface/volume curves (disjont clusters of particular size)
void TLocClustStat::PlotNcpTop10(const TStr& OutFNm, TStr Desc, const int& TakeMinN) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  const double BinFactor = 1.05;
  double BinPos=1;
  int PrevBPos=1, CurBPos=1, CutId;
  bool AddSome;
  TVec<TFltPrV> Curves(TakeMinN);
  while (true) {
    PrevBPos = CurBPos;
    BinPos *= BinFactor; // do exponential binning
    CurBPos = (int) floor(BinPos);
    if (CurBPos == PrevBPos) { CurBPos=PrevBPos+1;  BinPos=CurBPos; }
    const int Nodes = CurBPos;
    TIntSet TabuNIdSet(Graph->GetNodes());
    AddSome = false;
    for (int t = 0; t < TakeMinN; t++) {
      const double Phi = FindBestCut(Nodes, TabuNIdSet, CutId);
      if (CutId == -1) { break; }
      Curves[t].Add(TFltPr(Nodes, Phi));
      for (int n = 0; n < Nodes; n++) {
        TabuNIdSet.AddKey(SweepsV[CutId].NId(n)); }
      AddSome = true;
    }
    if (! AddSome) { break; }
  }
  TGnuPlot GP("ncpTop."+OutFNm, TStr::Fmt("%s. Top disjoint clusters. Take:%d, %s", Desc.CStr(), TakeMinN, ParamStr().CStr()));
  for (int i = 0; i < Curves.Len(); i++) {
    GP.AddPlot(Curves[i], gpwLines, TStr::Fmt("MIN %d", i+1), "lw 1"); }
  //if (! BagOfWhiskerV.Empty()) { GP.AddPlot(BagOfWhiskerV, gpwLines, " Whiskers", "lw 2"); }
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  GP.SetXRange(1, Graph->GetNodes()/2);
  GP.SavePng();
}

// plot conductance on the boundary / counductance inside vs. cluster size
void TLocClustStat::PlotPhiInOut(const TStr& OutFNm, TStr Desc) const {
  IAssert(! BestCutH.Empty() && ! Graph.Empty());
  TFltPrV PhiInV, PhiBoundV, PhiRatV;
  FILE *F = fopen(TStr::Fmt("phiInOut.%s-all.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#Nodes\tEdges\tVol\tCut\tPhi\tInNodes\tInEdges\tInVol\tInCut\tInPhi\n");
  TLocClustStat ClustStat2(Alpha, 1, KMax, KFac, Coverage, SizeFrac);
  const double IdxFact = 1.05;
  int curIdx=1, prevIdx=1;
  while (curIdx <= BestCutH.Len()) {
    const TCutInfo& CutInfo = BestCutH[curIdx-1];
    if (CutInfo.GetNodes() > 1) {
      PUNGraph ClustG = TSnap::GetSubGraph(Graph, CutInfo.CutNIdV);
      ClustStat2.Run(ClustG);
      const TCutInfo& InCut = ClustStat2.FindBestCut(-1);
      PhiInV.Add(TFltPr(CutInfo.GetNodes(), InCut.GetPhi()));
      PhiBoundV.Add(TFltPr(CutInfo.GetNodes(), CutInfo.GetPhi()));
      PhiRatV.Add(TFltPr(CutInfo.GetNodes(), InCut.GetPhi()/CutInfo.GetPhi()));
      fprintf(F, "%d\t%d\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%f\n", CutInfo.GetNodes(), CutInfo.GetEdges(), CutInfo.GetVol(),
        CutInfo.GetCutSz(), CutInfo.GetPhi(),  InCut.GetNodes(), InCut.GetEdges(), InCut.GetVol(), InCut.GetCutSz(), InCut.GetPhi());
      fflush(F);
    }
    prevIdx = curIdx;
    curIdx = (int) TMath::Round(double(curIdx)*IdxFact);
    if (prevIdx == curIdx) { curIdx++; }
  }
  fclose(F);
  { TGnuPlot GP("phiInOut."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  GP.AddPlot(PhiBoundV, gpwLines, "CUT conductance", "lw 1");
  GP.AddPlot(PhiInV, gpwLines, "INSIDE conductance", "lw 1");
  //GP.AddPlot(PhiRatV, gpwLines, "RATIO (inside/boundary)", "lw 1");
  GP.SetXRange(1, Graph->GetNodes()/2);  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  //GP.AddCmd("set logscale xyy2 10");
  //GP.AddCmd("set y2label \"Conductance ratio (inside/boundary -- higher better)\"");
  GP.SavePng(); }
  //system(TStr(TStr("replace_all.py phiInOut.")+OutFNm+".plt \"title \\\"RATIO (inside/boundary)\" \"axis x1y2 title \\\"RATIO (inside/boundary)\"").CStr());
  //GP.RunGnuPlot();
  { TGnuPlot GP("phiInOutRat."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  GP.AddPlot(PhiRatV, gpwLines, "RATIO (Inside / Boundary)", "lw 1");
  GP.SetXRange(1, Graph->GetNodes()/2);  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("Nodes", "Conductance ratio (inside/boundary) -- higher better");
  GP.SavePng(); }
}

// Plot edges inside, edges cut and conductance.
// run: replace_all.py cutEdges.*.plt "title \"Conductance" "axis x1y2 title \"Conductance"
void TLocClustStat::PlotBestClustDens(TStr OutFNm, TStr Desc) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  const int len = BestCutH.Len();
  TFltPrV CutV(len, 0), EdgesV(len, 0), PhiV(len,0);
  for (int i = 0; i < BestCutH.Len(); i++) {
    const double size = BestCutH.GetKey(i).Val;
    CutV.Add(TFltPr(size, BestCutH[i].GetCutSz()));
    EdgesV.Add(TFltPr(size, BestCutH[i].GetEdges()));
    PhiV.Add(TFltPr(size, BestCutH[i].GetPhi()));
  }
  TGnuPlot GP("cutEdges."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  TFltPrV NewV;  TLocClustStat::MakeExpBins(EdgesV, NewV);
  GP.AddPlot(NewV, gpwLines, "Edges inside");
  TLocClustStat::MakeExpBins(CutV, NewV);
  GP.AddPlot(NewV, gpwLines, "Cut edges");
  TLocClustStat::MakeExpBins(PhiV, NewV);
  GP.AddPlot(NewV, gpwLines, "Conductance");
  GP.SetXYLabel("Cluster size", "Edges"); GP.SetScale(gpsLog10XY);
  GP.AddCmd("set logscale xyy2 10");
  GP.AddCmd("set y2label \"Conductance\"");
  GP.SavePng();
  system(TStr(TStr("replace_all.py cutEdges.")+OutFNm+".plt \"title \\\"Conductance\" \"axis x1y2 title \\\"Conductance\"").CStr());
  GP.RunGnuPlot();
}

// all different conducances of all sizes (not just lower envelope)
void TLocClustStat::PlotNCPScatter(const TStr& OutFNm, TStr Desc) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  THashSet<TFltPr> PhiSzH;
  IAssertR(! SizePhiH.Empty(), "Set SaveAllCond=true in TLocClustStat::Run()");
  for (int k = 0; k < SizePhiH.Len(); k++) {
    const int K = SizePhiH.GetKey(k);
    const TFltV& PhiV = SizePhiH[k];
    for (int p = 0; p < PhiV.Len(); p++) {
      PhiSzH.AddKey(TFltPr(K, TMath::Round(PhiV[p], 3))); }
  }
  TFltPrV PhiSzV;  PhiSzH.GetKeyV(PhiSzV);
  TGnuPlot GP("ncpScatter."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  GP.AddPlot(PhiSzV, gpwPoints, "", "pt 5 ps 0.2");
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  GP.SetXRange(1, Graph->GetNodes()/2);
  GP.SavePng();
}

// histogram of conductances for a fixed CmtySz
void TLocClustStat::PlotPhiDistr(const int& CmtySz, const TStr& OutFNm, TStr Desc) const {
  IAssert(! SizePhiH.Empty());
  const TFltV& PhiV = SizePhiH.GetDat(CmtySz);
  THash<TFlt, TInt> PhiCntH;
  for (int i = 0; i < PhiV.Len(); i++) {
    const double Buck =  TMath::Round(PhiV[i], 3);
    PhiCntH.AddDat(Buck) += 1;
  }
  TGnuPlot::PlotValCntH(PhiCntH, TStr::Fmt("phiDistr%03d.%s", CmtySz, OutFNm.CStr()), 
    TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()), "{/Symbol \\F} (conductance)",  "Count");
}

void TLocClustStat::PlotBoltzmanCurve(const TStr& OutFNm, TStr Desc) const {
  TFltPrV MeanV1, MedV1, Dec1V1, MinV1, MaxV1;
  GetCurveStat(MeanV1, MedV1, Dec1V1, MinV1, MaxV1);
  TVec<TFltPrV> NcpV;
  //const TFltV TempV = TFltV::GetV(0.05, 0.01, 0.1, 10, 100, 1000);
  const TFltV TempV = TFltV::GetV(0.001, 0.005, 0.01, 0.02, 0.05, 0.1, 0.5, 1);
  GetBoltzmanCurveStat(TempV, NcpV);
  TGnuPlot GP("ncp."+OutFNm+"-B", TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));  
  GP.AddPlot(MinV1, gpwLines, TStr::Fmt("%s MIN (%d, %d)", Desc.CStr(), Graph->GetNodes(), Graph->GetEdges()), "lw 1");
  GP.AddPlot(MeanV1, gpwLines, "Avg", "lw 1");
  GP.AddPlot(MedV1, gpwLines, "Median", "lw 1");
  GP.AddPlot(Dec1V1, gpwLines, "Decile-1", "lw 1");
  for (int t = 0; t < TempV.Len(); t++) {
    GP.AddPlot(NcpV[t], gpwLines, TStr::Fmt("Temp %g", TempV[t]()), "lw 1");
  }
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  GP.SavePng();
  //
  TFltPrV SzNClustV;
  int kCnt=1;
  for (int i = 0; i < SizePhiH.Len(); i++) {
    const int K = SizePhiH.GetKey(i);
    const TFltV& PhiV = SizePhiH[i];
    SzNClustV.Add(TFltPr(K, PhiV.Len()));
    if (K>2 && (pow(10.0, (int)log10((double)K))==K || (K >=10 && K<=100 && K%10==0) || (K >=100 && K<=1000 && K%100==0))) {
      THash<TFlt, TFlt> CntH;
      for (int p = 0; p < PhiV.Len(); p++) {
        CntH.AddDat(TMath::Round(log10(PhiV[p].Val),1)) += 1;
      }
      TGnuPlot::PlotValCntH(CntH, TStr::Fmt("ncp.%s-c%02d", OutFNm.CStr(), kCnt++), TStr::Fmt("%s. K=%d, NPieces=%d, %s", 
        Desc.CStr(), K, PhiV.Len(), ParamStr().CStr()), "log_10 {/Symbol \\F} (conductance)", 
        TStr::Fmt("Number of pieces of such conductance, K=%d, NPieces=%d)", K, PhiV.Len()));
    }
  }
  TGnuPlot::PlotValV(SzNClustV, "ncp."+OutFNm+"-ClSz", TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()),
    "k (cluster size)", "c(k) (number of extracted clusters)", gpsLog);
}

void TLocClustStat::ImposeNCP(const TLocClustStat& LcStat2, TStr OutFNm, TStr Desc, TStr Desc1, TStr Desc2) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV MeanV1, MedV1, Dec1V1, MinV1, MaxV1;
  TFltPrV MeanV2, MedV2, Dec1V2, MinV2, MaxV2;
  GetCurveStat(MeanV1, MedV1, Dec1V1, MinV1, MaxV1);
  LcStat2.GetCurveStat(MeanV2, MedV2, Dec1V2, MinV2, MaxV2);
  // plot
  TGnuPlot GP("ncp."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  if (! MinV1.Empty()) { GP.AddPlot(MinV1, gpwLines, TStr::Fmt("%s MIN (%d, %d)", Desc1.CStr(), Graph->GetNodes(), Graph->GetEdges()), "lw 1"); }
  if (! MinV2.Empty()) { GP.AddPlot(MinV2, gpwLines, TStr::Fmt("%s MIN (%d, %d)", Desc2.CStr(), LcStat2.Graph->GetNodes(), LcStat2.Graph->GetEdges()), "lw 1"); }
  if (! BagOfWhiskerV.Empty()) { 
    GP.AddPlot(BagOfWhiskerV, gpwLines, Desc1+" whiskers", "lw 1"); 
    TFltPrV BestWhiskV;  BestWhiskV.Add(BestWhisk);
    GP.AddPlot(BestWhiskV, gpwPoints, Desc1+" Best whisker", "pt 5 ps 2"); }
  if (! LcStat2.BagOfWhiskerV.Empty()) {
    GP.AddPlot(LcStat2.BagOfWhiskerV, gpwLines, Desc2+" whiskers", "lw 1"); 
    TFltPrV BestWhiskV;  BestWhiskV.Add(LcStat2.BestWhisk);
    GP.AddPlot(BestWhiskV, gpwPoints, Desc2+" Best whisker", "pt 5 ps 2"); }
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  //GP.SetXRange(1, Graph->GetNodes()/2);
  GP.SavePng();
}

void TLocClustStat::ImposeNCP(const TLocClustStat& LcStat2, const TLocClustStat& LcStat3, TStr OutFNm, TStr Desc, TStr Desc1, TStr Desc2, TStr Desc3) const {
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV MeanV1, MedV1, Dec1V1, MinV1, MaxV1;
  TFltPrV MeanV2, MedV2, Dec1V2, MinV2, MaxV2;
  TFltPrV MeanV3, MedV3, Dec1V3, MinV3, MaxV3;
  GetCurveStat(MeanV1, MedV1, Dec1V1, MinV1, MaxV1);
  LcStat2.GetCurveStat(MeanV2, MedV2, Dec1V2, MinV2, MaxV2);
  LcStat3.GetCurveStat(MeanV3, MedV3, Dec1V3, MinV3, MaxV3);
  // plot
  TGnuPlot GP("phiTR."+OutFNm, TStr::Fmt("%s. %s", Desc.CStr(), ParamStr().CStr()));
  if (! MinV1.Empty()) { GP.AddPlot(MinV1, gpwLines, Desc1+" MIN", "lw 1"); }
  if (! MinV2.Empty()) { GP.AddPlot(MinV2, gpwLines, Desc2+" MIN", "lw 1"); }
  if (! MinV3.Empty()) { GP.AddPlot(MinV3, gpwLines, Desc3+" MIN", "lw 1"); }
  if (! BagOfWhiskerV.Empty()) { 
    GP.AddPlot(BagOfWhiskerV, gpwLines, Desc1+" whiskers", "lw 1"); 
    TFltPrV BestWhiskV;  BestWhiskV.Add(BestWhisk);
    GP.AddPlot(BestWhiskV, gpwPoints, Desc1+" Best whisker", "pt 5 ps 2"); }
  if (! LcStat2.BagOfWhiskerV.Empty()) {
    GP.AddPlot(LcStat2.BagOfWhiskerV, gpwLines, Desc2+" whiskers", "lw 1"); 
    TFltPrV BestWhiskV;  BestWhiskV.Add(LcStat2.BestWhisk);
    GP.AddPlot(BestWhiskV, gpwPoints, Desc2+" Best whisker", "pt 5 ps 2"); }
  if (! LcStat3.BagOfWhiskerV.Empty()) { 
    GP.AddPlot(LcStat3.BagOfWhiskerV, gpwLines, Desc3+" whiskers", "lw 1"); 
    TFltPrV BestWhiskV;  BestWhiskV.Add(LcStat3.BestWhisk);
    GP.AddPlot(BestWhiskV, gpwPoints, Desc3+" Best whisker", "pt 5 ps 2"); }
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "{/Symbol \\F} (conductance)");
  GP.SetXRange(1, Graph->GetNodes()/2);
  GP.SavePng();
}

void TLocClustStat::SaveTxtInfo(const TStr& OutFNmPref, const TStr& Desc, const bool& SetMaxAt1) const {
  printf("Save text info...");
  TExeTm ExeTm;
  const int GNodes = Graph->GetNodes();
  const int GEdges = Graph->GetEdges();
  TVec<TFltV> ColV(17);
  double MxFrac=0, AvgFrac=0, MedianFrac=0, Pct9Frac=0, Flake=0;
  double PrevBPos = 1, BPos = 1;
  for (int i = 0; i < SizeBucketSet.Len(); i++) {
    if ( !BestCutH.IsKey(SizeBucketSet[i])) { continue; }
    const TLocClustStat::TCutInfo& C = GetKNodeCut(SizeBucketSet[i]);
    C.GetFracDegOut(Graph, MxFrac, AvgFrac, MedianFrac, Pct9Frac, Flake);
    ColV[0].Add(C.Nodes());  ColV[1].Add(C.Edges()); 
    ColV[2].Add(C.CutSz());  ColV[3].Add(C.GetPhi());
    ColV[4].Add(C.GetExpansion());       ColV[5].Add(C.GetIntDens()); 
    ColV[6].Add(C.GetCutRatio(GNodes));  ColV[7].Add(C.GetNormCut(GEdges));
    ColV[8].Add(MxFrac);       ColV[9].Add(AvgFrac); 
    ColV[10].Add(MedianFrac);  ColV[11].Add(Pct9Frac);  ColV[12].Add(Flake);
    ColV[13].Add(double(2.0*C.Edges));  ColV[14].Add(C.GetExpEdgesIn(GEdges));
    ColV[15].Add(C.GetModular(GEdges)); ColV[16].Add(C.GetModRat(GEdges));
    printf(".");
  }
  // normalize so that maximum value is at 1
  if (SetMaxAt1) {
    for (int c = 1; c < ColV.Len(); c++) {
      double MaxVal=1e-10;
      for (int r = 0; r < ColV[c].Len(); r++) { MaxVal = TMath::Mx(MaxVal, ColV[c][r]()); }
      for (int r = 0; r < ColV[c].Len(); r++) { ColV[c][r] /= MaxVal; }
    }
  }
  // save
  const TStr DatFNm = TStr::Fmt("ncp.%s.INFO.tab", OutFNmPref.CStr());
  FILE *F = fopen(DatFNm.CStr(), "wt");
  fprintf(F, "# %s %s\n", Desc.CStr(), ParamStr().CStr());
  fprintf(F, "#N_inside\tE_inside\tE_across\tConductance\tExpansion\tIntDensity\tCutRatio\tNormCut\tMx_FracDegOut\tAvg_FDO\tMedian_FDO\t90Pct_FDO\tFlake_FDO\tVolume\tExpVolume\tModularity\tModRatio\n");
  for (int r = 0; r < ColV[0].Len(); r++) {
    fprintf(F, "%g", ColV[0][r]());
    for (int c = 1; c < ColV.Len(); c++) {
      fprintf(F, "\t%g", ColV[c][r]()); }
    fprintf(F, "\n");
  }
  fclose(F);
  printf("[%s]\n", ExeTm.GetStr());
  TGnuPlot GP(TStr::Fmt("ncp.%s.All", OutFNmPref.CStr()), TStr::Fmt("%s %s", 
    Desc.CStr(), ParamStr().CStr()));
  GP.AddPlot(DatFNm, 1, 4, gpwLines, "Conductance", "lw 2");
  GP.AddPlot(DatFNm, 1, 5, gpwPoints, "Expansion", "pt 3");
  GP.AddPlot(DatFNm, 1, 6, gpwPoints, "Internal Density", "pt 5 ps 0.8");
  GP.AddPlot(DatFNm, 1, 7, gpwPoints, "Cut Ratio", "pt 6");
  GP.AddPlot(DatFNm, 1, 8, gpwPoints, "Normalized Cut", "pt 7");
  GP.AddPlot(DatFNm, 1, 9, gpwPoints, "Maximum FDO", "pt 9");
  GP.AddPlot(DatFNm, 1, 10, gpwPoints, "Avg FDO", "pt 11");
  GP.AddPlot(DatFNm, 1, 13, gpwPoints, "Flake FDO", "pt 13");
  GP.SetScale(gpsLog10XY);
  GP.SetXYLabel("k (number of nodes in the cluster)", "Normalized community score (lower is better)");
  GP.SavePng();
}

// conductances if clusters are composed of disjoint pieces that can be separated
// from the graph by a single edge
void TLocClustStat::BagOfWhiskers(const PUNGraph& Graph, TFltPrV& SizePhiV, TFltPr& MaxWhisk) {
  TCnComV Cn1ComV;
  TSnap::Get1CnCom(Graph, Cn1ComV);
  TIntPrV SzVolV;
  int MxSize=0;
  if (Cn1ComV.Empty()) { printf("** No bridges\n"); SizePhiV.Clr();  return; }
  //  Graph->SaveEdgeList("g-vol.txt");  TGraphViz::Plot(Graph, gvlNeato, "g-vol.gif");  Fail; } IAssert(vol <= sz*(sz-1));
  printf("  1-connected components: %d\n", Cn1ComV.Len());
  MaxWhisk = TFltPr(1,1);
  for (int c = 0; c < Cn1ComV.Len(); c++) {
    const TIntV& NIdV = Cn1ComV[c].NIdV;
    const int sz = NIdV.Len();
    if (sz < 2) { continue; }
    int vol = 0; // volume is the size of degrees
    for (int n = 0; n < sz; n++) {
      vol += Graph->GetNI(NIdV[n]).GetOutDeg(); }
    SzVolV.Add(TIntPr(sz, vol));
    MxSize += sz;
    if (1.0/double(vol) < MaxWhisk.Val2) { MaxWhisk=TFltPr(NIdV.Len(), 1.0/double(vol)); }
  }
  SzVolV.Sort(false);
  // compose clusters
  THash<TInt, TIntSet> ItemSetH(MxSize, true);
  THash<TInt, TInt> VolH(MxSize, true);
  THash<TInt, TFlt> CostH(MxSize, true);
  ItemSetH.AddKey(0);  VolH.AddKey(0);
  TExeTm ExeTm;
  // exact up to size 1000
  for (int size = 2; size <= TMath::Mn(MxSize, 1000); size++) {
    for (int item = 0; item <SzVolV.Len(); item++) {
      const int smallSz = size-SzVolV[item].Val1;
      if (ItemSetH.IsKey(smallSz)) {
        const TIntSet& SmallSet = ItemSetH.GetDat(smallSz);
        if (SmallSet.IsKey(item)) { continue; }
        const int SmallVol = VolH.GetDat(smallSz);
        // combine smaller nad new solution
        const double curCost = CostH.IsKey(size) ? double(CostH.GetDat(size)) : double(10e10);
        const double newCost = double(SmallSet.Len()+1) / double(SmallVol+SzVolV[item].Val2);
        if (curCost < newCost) { continue; }
        VolH.AddDat(size, SmallVol+SzVolV[item].Val2);
        ItemSetH.AddDat(size, SmallSet);
        ItemSetH.GetDat(size).AddKey(item);
        CostH.AddDat(size, newCost);
      }
    }
    if (VolH.IsKey(size) && size%100==0) {
      printf("\rSize: %d/%d: vol: %d,  items: %d/%d [%s]", size, MxSize, VolH.GetDat(size).Val,
        ItemSetH.GetDat(size).Len(), SzVolV.Len(), ExeTm.GetStr());
    }
  }
  // add sizes larger than 1000
  printf("\nAdding sizes > 1000 nodes...");
  int partSz=0; double partVol=0.0;
  for (int i = 0; i < SzVolV.Len(); i++) {
    partSz += SzVolV[i].Val1();
    partVol += SzVolV[i].Val2();
    if (partSz < 1000) { continue; }
    const double curCost = CostH.IsKey(partSz) ? double(CostH.GetDat(partSz)) : double(10e10);
    const double partPhi = double(i+1)/partVol;
    if (partPhi < curCost) {
      CostH.AddDat(partSz, partPhi);
    }
  }
  VolH.SortByKey();
  CostH.SortByKey();
  SizePhiV.Gen(CostH.Len(), 0);
  SizePhiV.Add(TFltPr(1, 1));
  for (int s = 0; s < CostH.Len(); s++) {
    const int size = CostH.GetKey(s);
    const double cond = CostH[s]; //double(ItemSetH.GetDat(size).Len())/double(VolH[s]);
    SizePhiV.Add(TFltPr(size, cond));
  }
  printf("done\n");
}

// faster greedy version
void TLocClustStat::BagOfWhiskers2(const PUNGraph& Graph, TFltPrV& SizePhiV) {
  TCnComV Cn1ComV;
  TSnap::Get1CnCom(Graph, Cn1ComV);
  TIntPrV SzVolV;
  int MxSize=0, TotVol=0;
  if (Cn1ComV.Empty()) { printf("** No bridges\n");  SizePhiV.Clr();  return; }
  printf("  1-connected components: %d\n", Cn1ComV.Len());
  for (int c = 0; c < Cn1ComV.Len(); c++) {
    const TIntV& NIdV = Cn1ComV[c].NIdV;
    const int sz = NIdV.Len();
    if (sz < 2) { continue; }
    int vol = 0; // volume is the size of degrees
    for (int n = 0; n < sz; n++) {
      vol += Graph->GetNI(NIdV[n]).GetOutDeg(); }
    SzVolV.Add(TIntPr(sz, vol));
    MxSize += sz;  TotVol += vol;
  }
  printf("  Total size: %d\t Total vol: %d\n", MxSize, TotVol);
  SzVolV.Sort(false);
  // compose clusters
  THash<TFlt, TFlt> SizePhiH(MxSize, true);
  for (int i = 0; i < SzVolV.Len(); i++) {
    const int Sz = SzVolV[i].Val1();
    const double Phi = 1.0/double(SzVolV[i].Val2());
    if ((! SizePhiH.IsKey(Sz)) || SizePhiH.GetDat(Sz) > Phi) {
      SizePhiH.AddDat(Sz, Phi);  }
  }
  double partSz=0.0, partVol=0.0;
  for (int i = 0; i < SzVolV.Len(); i++) {
    partSz += SzVolV[i].Val1();
    partVol += SzVolV[i].Val2();
    const double partPhi = double(i+1)/partVol;
    if ((! SizePhiH.IsKey(partSz)) || partPhi < SizePhiH.GetDat(partSz)) {
      SizePhiH.AddDat(partSz, partPhi); }
  }
  SizePhiV.Gen(SizePhiH.Len()+1, 0);
  SizePhiV.Add(TFltPr(1, 1));
  SizePhiH.SortByKey();
  for (int s = 0; s < SizePhiH.Len(); s++) {
    SizePhiV.Add(TFltPr(SizePhiH.GetKey(s), SizePhiH[s]));
  }
}

void TLocClustStat::MakeExpBins(const TFltPrV& ValV, TFltPrV& NewV) {
  if (ValV.Empty()) { NewV.Clr(false); return; }
  NewV.Gen(1000, 0);
  double PrevBPos = 1, BPos = 1;
  int i = 0;
  while (BPos <= ValV.Last().Val1) {
    //if (TakeValAt == 1) {
    //  while (i < ValV.Len() && ValV[i].Val1 <= BPos) { i++; }
    //  NewV.Add(ValV[i-1]); }
    //else if (TakeValAt == 2) {
    int MinI=-1;  double MinCnt=TFlt::Mx;
    while (i < ValV.Len() && ValV[i].Val1 <= BPos) {
      if (ValV[i].Val2 < MinCnt) { MinCnt=ValV[i].Val2; MinI=i; } i++; }
    if (MinI>=0 && MinI<ValV.Len()) {
      NewV.Add(ValV[MinI]); }
    PrevBPos = (uint) floor(BPos);
    BPos *= BinFactor;
    if (floor(BPos) == PrevBPos) { BPos = PrevBPos + 1; }
  }
  NewV.Add(ValV.Last());
}

void TLocClustStat::MakeExpBins(const TFltV& ValV, TFltV& NewV) {
  if (ValV.Empty()) { NewV.Clr(false); return; }
  NewV.Gen(1000, 0);
  double PrevBPos = 1, BPos = 1;
  int i = 1;
  NewV.Add(ValV[0]);
  while (BPos <= ValV.Len()) {
    int MinI=-1;  double MinCnt=TFlt::Mx;
    while (i < ValV.Len() && i <= BPos) {
      if (ValV[i] < MinCnt) { MinCnt=ValV[i]; MinI=i; } i++; }
    if (MinI>=0 && MinI<ValV.Len()) {
      NewV.Add(ValV[MinI]); }
    PrevBPos = (uint) floor(BPos);
    BPos *= BinFactor;
    if (floor(BPos) == PrevBPos) { BPos = PrevBPos + 1; }
  }
  NewV.Add(ValV.Last());
}


//////////////////////////////////////////////////
// Local clustering for a set of graphs (loads ncp-*.tab files)
TNcpGraphsBase::TNcpGraphsBase(const TStr& FNmWc) {
  TStr FNm;
  for (TFFile FFile(FNmWc); FFile.Next(FNm); ) {
    TSsParser Ss(FNm, ssfTabSep, true, false);
    int TrueNcpId=-1, WhiskId=-1, RewBestWhiskId=-1, RewId=-1, BestWhiskId=-1;
    while (Ss.Next()) {
      for (int f = 0; f < Ss.GetFlds(); f++) {
        // load ForestFire parameter (fwd burn prob)
        if (strstr(Ss[f], "FWD:")) { 
          TStr S(Ss[f]); const int x = S.SearchStr("FWD:");
          ParamValV.Add(S.GetSubStr(x+4, S.SearchCh(' ', x+1)-1).GetFlt());
        }
        // extract column names
        if (strstr(Ss[f], "ORIGINAL MIN")!=NULL) { 
          GNmV.Add(TStr::Fmt("%s %s", FNm.GetSubStr(FNm.SearchCh('.')+1, FNm.SearchChBack('.')-1).CStr(), strchr(Ss[f], '('))); 
          int Nodes=0,Edges=0; sscanf(strchr(Ss[f], '(')+1, "%d,%d)", &Nodes, &Edges);
          GSizeV.Add(TIntPr(Nodes, Edges));
          printf("%s: %d %d\n", GNmV.Last().CStr(), Nodes, Edges);
          TrueNcpId=f;
        }
        if (strstr(Ss[f], "ORIGINAL whisker")!=NULL || strstr(Ss[f], "TRUE whisker")!=NULL) { WhiskId=f; } 
        if (strstr(Ss[f], "ORIGINAL Best whisker")!=NULL || strstr(Ss[f], "TRUE Best whisker")!=NULL) { BestWhiskId=f; }
        if (strstr(Ss[f], "REWIRED MIN")!=NULL || strstr(Ss[f], "RAND MIN")!=NULL) { RewId=f; } 
        if (strstr(Ss[f], "REWIRED Best whisker")!=NULL || strstr(Ss[f], "RAND Best whisker")!=NULL) { RewBestWhiskId=f; }
      }
      if (TrueNcpId!=-1 || WhiskId!=-1) { break; }
    }
    if (TrueNcpId < 0) { printf("%s\n", FNm.GetFMid().CStr()); break; }
    if (BestWhiskId < 0) { WhiskerV.Add(TFltPr(1,1)); }
    if (RewBestWhiskId < 0) { RewWhiskerV.Add(TFltPr(1,1)); }
    NcpV.Add(); WhiskNcpV.Add(); RewNcpV.Add();
    TFltPrV& Ncp = NcpV.Last();
    TFltPrV& WhiskNcp = WhiskNcpV.Last();
    TFltPrV& RewNcp = RewNcpV.Last();
    bool Once=false, Once2=false;
    while (Ss.Next()) { 
      if (TrueNcpId < Ss.GetFlds()&& Ss.IsFlt(TrueNcpId)) { Ncp.Add(TFltPr(Ss.GetFlt(TrueNcpId-1), Ss.GetFlt(TrueNcpId))); }
      if (WhiskId>=0 && WhiskId < Ss.GetFlds() && Ss.IsFlt(WhiskId)) { WhiskNcp.Add(TFltPr(Ss.GetFlt(WhiskId-1), Ss.GetFlt(WhiskId))); }
      if (RewId >=0 && RewId < Ss.GetFlds()&& Ss.IsFlt(RewId)) { RewNcp.Add(TFltPr(Ss.GetFlt(RewId-1), Ss.GetFlt(RewId))); }
      if (BestWhiskId>=0 && BestWhiskId < Ss.GetFlds() && ! Once) {  Once=true;
        int W2=BestWhiskId-1;  while (W2 > 0 && Ss.GetFlt(W2)!=(double)int(Ss.GetFlt(W2))) { W2--; }
        WhiskerV.Add(TFltPr(Ss.GetFlt(W2), Ss.GetFlt(BestWhiskId))); }
      if (RewBestWhiskId>=0 && RewBestWhiskId < Ss.GetFlds() && ! Once2) {  Once2=true;
        int W2=RewBestWhiskId-1;  while (W2 > 0 && Ss.GetFlt(W2)!=(double)int(Ss.GetFlt(W2))) { W2--; }
        RewWhiskerV.Add(TFltPr(Ss.GetFlt(W2), Ss.GetFlt(RewBestWhiskId))); }
    }
    printf("  ncp:%d  whisk:%d  rew:%d\n", NcpV.Last().Len(), WhiskNcpV.Last().Len(), RewNcpV.Last().Len());
  }
  IAssert(NcpV.Len() == GSizeV.Len());
}
TNcpGraphsBase::TNcpGraphsBase(TSIn& SIn) : GNmV(SIn), GSizeV(SIn), WhiskerV(SIn), 
  RewWhiskerV(SIn),NcpV(SIn), RewNcpV(SIn),WhiskNcpV(SIn) { 
}

void TNcpGraphsBase::Save(TSOut& SOut) const { 
  GNmV.Save(SOut); GSizeV.Save(SOut); 
  WhiskerV.Save(SOut);  RewWhiskerV.Save(SOut); NcpV.Save(SOut); 
  RewNcpV.Save(SOut); WhiskNcpV.Save(SOut); 
}

void TNcpGraphsBase::Impose(const TStr& OutFNm, const int& TopN, const bool& Smooth) { 
  TGnuPlot GP(OutFNm);
  for (int i = 0; i < TMath::Mn(NcpV.Len(), TopN); i++) {
    GP.AddPlot(NcpV[i], gpwLines, GNmV[i], Smooth?"smooth csplines":"");
  }
  GP.SetScale(gpsLog10XY);
  GP.SavePng();
}

double TNcpGraphsBase::GetXAtMinY(const TFltPrV& Ncp, const int& NNodes) {
  double MinX1=1, MinY1=1;
  for (int k = 0; k < Ncp.Len(); k++) {
    if (Ncp[k].Val2<MinY1) { MinX1=Ncp[k].Val1; MinY1=Ncp[k].Val2; } }
  return MinX1<1 ? 1 : MinX1;
  //if (NNodes < 1000) return MinX1;
  // smooth
  /*const int WndSize = 50;
  double MinX=1, MinY=1;
  TFltPrV Ncp2V;
  for (int k = 0; k < Ncp.Len(); k++) {
    int WndSz = k > WndSize ? WndSize : k;
    double SmoothVal=0.0, SmoothCnt=0;
    for (int i = -WndSz; i <= WndSz; i++) {
      if (k+i > -1 && k+i < Ncp.Len()) { SmoothCnt+=pow(1.1, -abs(i));
        SmoothVal+=pow(1.1, -abs(i)) * Ncp[k+i].Val2; }
    }
    SmoothVal = SmoothVal/SmoothCnt;
    Ncp2V.Add(TFltPr(Ncp[k].Val1, SmoothVal));
    if (SmoothVal<MinY) { MinX=Ncp[k].Val1; MinY=SmoothVal; }
  }
  static int cnt = 0;
  if (Ncp2V.Len() > 10 && cnt < 10) { 
    TGnuPlot GP(TStr::Fmt("test-%03d", ++cnt));
    GP.SetScale(gpsLog10XY);
    GP.AddPlot(Ncp, gpwLines, "true");
    GP.AddPlot(Ncp2V, gpwLines, "smooth");
    GP.SavePng();
  }
  if (MinX < 1) { return 1; } else if (MinX > 1000) { return 1000; }
  return MinX;*/
}

TFltPr TNcpGraphsBase::GetXYAtMinY(const TFltPrV& Ncp, const int& NNodes) {
  double MinX1=1, MinY1=1;
  for (int k = 0; k < Ncp.Len(); k++) {
    if (Ncp[k].Val2<MinY1) { MinX1=Ncp[k].Val1; MinY1=Ncp[k].Val2; } }
  return TFltPr(MinX1<1?1:MinX1, MinY1);
}

void TNcpGraphsBase::PlotNcpMin(const TStr& OutFNm, const bool& VsGraphN) {
  TFltPrV GSzMinK, GSzMinY;
  for (int i = 0; i < NcpV.Len(); i++) {
    const TFltPr XYAtMinY = GetXYAtMinY(NcpV[i], GSizeV[i].Val1);
    const double X = VsGraphN ? (!ParamValV.Empty()?ParamValV[i]():i+1) : GSizeV[i].Val1();
    GSzMinK.Add(TFltPr(X, XYAtMinY.Val1));
    GSzMinY.Add(TFltPr(X, XYAtMinY.Val2));
  }
  GSzMinK.Sort();  GSzMinY.Sort();
  const TStr XLabel = VsGraphN ? (!ParamValV.Empty()?"parameter value":"network number") : "network size";
  TGnuPlot::PlotValV(GSzMinK, TStr("bestK-")+OutFNm, "Network", XLabel, "size of best cluster", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
  TGnuPlot::PlotValV(GSzMinY, TStr("condAtBestK-")+OutFNm, "Network", XLabel, "conductance of best cluster", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
}

void TNcpGraphsBase::SaveTxtNcpMin(const TStr& OutFNm, const bool& VsGraphN) {
  TVec<TQuad<TInt, TInt, TFlt, TStr> > GSzMinK;
  for (int i = 0; i < NcpV.Len(); i++) {
    const TFltPr XYAtMinY = GetXYAtMinY(NcpV[i], GSizeV[i].Val1);
    const double X = VsGraphN ? (!ParamValV.Empty()?ParamValV[i]():i+1) : GSizeV[i].Val1();
    GSzMinK.Add(TQuad<TInt, TInt, TFlt, TStr>((int)X, (int)XYAtMinY.Val1(), XYAtMinY.Val2, GNmV[i]));
  }
  GSzMinK.Sort();
  FILE *F = fopen(TStr::Fmt("bestK-%s.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#nodes\tbestK\tcondAtBestK\tgraph name\n");
  for (int i = 0; i < GSzMinK.Len(); i++) {
    fprintf(F, "%d\t%d\t%f\t%s\n", GSzMinK[i].Val1(), GSzMinK[i].Val2(), GSzMinK[i].Val3(), GSzMinK[i].Val4.CStr());
  }
  fclose(F);
}

void TNcpGraphsBase::PlotRewNcpMin(const TStr& OutFNm, const bool& VsGraphN) {
  TFltPrV GSzMinK, GSzMinY;
  for (int i = 0; i < NcpV.Len(); i++) {
    const TFltPr XYAtMinY = GetXYAtMinY(RewNcpV[i], GSizeV[i].Val1);
    const double X = VsGraphN ? (!ParamValV.Empty()?ParamValV[i]():i+1) : GSizeV[i].Val1();
    GSzMinK.Add(TFltPr(X, XYAtMinY.Val1));
    GSzMinY.Add(TFltPr(X, XYAtMinY.Val2));
  }
  GSzMinK.Sort();  GSzMinY.Sort();
  const TStr XLabel = VsGraphN ? (!ParamValV.Empty()?"parameter value":"network number") : "network size";
  TGnuPlot::PlotValV(GSzMinK, TStr("bestR-")+OutFNm, "Rewired network", XLabel, "size of best cluster", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
  TGnuPlot::PlotValV(GSzMinY, TStr("condAtBestR-")+OutFNm, "Rewired network", XLabel, "conductance of best cluster", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
}

void TNcpGraphsBase::PlotBestWhisker(const TStr& OutFNm, const bool& VsGraphN) {
  TFltPrV GSzMinK, GSzMinY;
  for (int i = 0; i < GSizeV.Len(); i++) {
    if (WhiskerV[i].Val1()>0) {
      const double X = VsGraphN ? (!ParamValV.Empty()?ParamValV[i]():i+1) : GSizeV[i].Val1();
      GSzMinK.Add(TFltPr(X, WhiskerV[i].Val1()));
      GSzMinY.Add(TFltPr(X, WhiskerV[i].Val2()));
    }
  }
  GSzMinK.Sort();  GSzMinY.Sort();
  const TStr XLabel = VsGraphN ? (!ParamValV.Empty()?"parameter value":"network number") : "network size";
  TGnuPlot::PlotValV(GSzMinK, TStr("bestW-")+OutFNm, "Network", XLabel, "size of best whisker", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
  TGnuPlot::PlotValV(GSzMinY, TStr("condAtBestW-")+OutFNm, "Network", XLabel, "conductance of best whisker", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
}

void TNcpGraphsBase::PlotRewBestWhisker(const TStr& OutFNm, const bool& VsGraphN) {
  TFltPrV GSzMinK, GSzMinY;
  for (int i = 0; i < GSizeV.Len(); i++) {
    if (WhiskerV[i].Val1()>0) {
      const double X = VsGraphN ? (!ParamValV.Empty()?ParamValV[i]():i+1) : GSizeV[i].Val1();
      GSzMinK.Add(TFltPr(X, RewWhiskerV[i].Val1()));
      GSzMinY.Add(TFltPr(X, RewWhiskerV[i].Val2()));
    }
  }
  GSzMinK.Sort();  GSzMinY.Sort();
  const TStr XLabel = VsGraphN ? (!ParamValV.Empty()?"parameter value":"network number") : "network size";
  TGnuPlot::PlotValV(GSzMinK, TStr("bestWR-")+OutFNm, "Rewired network", XLabel, "size of best rewired whisker", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
  TGnuPlot::PlotValV(GSzMinY, TStr("condAtBestWR-")+OutFNm, "Rewired network", XLabel, "conductance of best rewired whisker", VsGraphN?gpsLog10Y:gpsLog, false, gpwLinesPoints);
}

void TNcpGraphsBase::PlotAvgNcp(const TStr& OutFNm, const TVec<TFltPrV>& NcpVec, const int& MinSz, const double& MaxMinY) {
  THash<TFlt, TMom> MomH;
  int Cnt=0;
  for (int i = 0; i < NcpVec.Len(); i++) {
    if (GSizeV[i].Val1 < MinSz) { continue; }
    const TFltPrV& Ncp = NcpVec[i];
    double MinX=1, MinY=1;
    for (int k = 0; k < Ncp.Len(); k++){
      if (Ncp[k].Val2<MinY) { MinX=Ncp[k].Val1; MinY=Ncp[k].Val2; } }
    if (MinY > MaxMinY) { continue; }  Cnt++;
    //const double Coef = (1-0.0001)/(1.0-MinY);
    for (int k = 0; k < Ncp.Len(); k++){
      //MomH.AddDat(TMath::Round(exp(TMath::Round(log(Ncp[k].Val1()), 2)),2)).Add(0.0001+(Ncp[k].Val2-MinY)*Coef);
      MomH.AddDat(TMath::Round(exp(TMath::Round(log(Ncp[k].Val1()), 1)),0)).Add(Ncp[k].Val2);
    }
  }
  TGnuPlot::PlotValMomH(MomH, OutFNm, "", "size of the cluster, k", "phi(k)", gpsLog, gpwLines, true, true,true,true);
  printf("  minSz: %d, miny %g\t%d\n", MinSz, MaxMinY, Cnt);
}

void TNcpGraphsBase::SaveTxt(const TStr& OutFNm) {
  FILE *F=fopen(OutFNm.CStr(), "wt");
  fprintf(F, "#Nodes\tEdges\tBestK\tPhi(BestK)\tMaxWhiskN\tPhi(MaxWhisk)\tGraph\n");
  for (int i = 0; i < NcpV.Len(); i++) {
    const TFltPrV& Ncp = NcpV[i];
    double MinX=1, MinY=1;
    for (int k = 0; k < Ncp.Len(); k++){
      if (Ncp[k].Val2<MinY) { MinX=Ncp[k].Val1; MinY=Ncp[k].Val2; } }
    fprintf(F, "%d\t%d\t%d\t%f\t%d\t%f\t%s\n", GSizeV[i].Val1(), GSizeV[i].Val2(), 
      int(MinX), MinY, int(WhiskerV[i].Val1), WhiskerV[i].Val2(), GNmV[i].CStr());
  }
  fclose(F);
}

void TNcpGraphsBase::PlotDataset(const TStr& InFNmWc, const TStr& OutFNm, const bool& ImposeNcp, const bool& VsGraphN) {
  TNcpGraphsBase NcpBs(InFNmWc);  
  //NcpBs.Save(TFOut(OutFNm+".NcpBase"));
  //TNcpGraphsBase NcpBs(TFIn(OutFNm+".NcpBase"));
  if (ImposeNcp) {
    NcpBs.Impose(OutFNm+"5R", 5, false);  NcpBs.Impose(OutFNm+"5S", 5, true); 
    NcpBs.Impose(OutFNm+"R", 10, false);  NcpBs.Impose(OutFNm+"S", 10, true); 
  }
  NcpBs.PlotNcpMin(OutFNm, VsGraphN);
  //NcpBs.SaveTxtNcpMin(OutFNm, VsGraphN);
  NcpBs.PlotRewNcpMin(OutFNm, VsGraphN);
  NcpBs.PlotBestWhisker(OutFNm, VsGraphN);
  NcpBs.PlotRewBestWhisker(OutFNm, VsGraphN);
  
  //NcpBs.PlotAvgNcp(OutFNm+"AvgNcp", NcpBs.NcpV, 1, 1);
  //NcpBs.PlotAvgNcp(OutFNm+"AvgRewNcp", NcpBs.RewNcpV, 1, 1);
  /*NcpBs.PlotAvgNcp(OutFNm+"AvgNcp2", NcpBs.NcpV, 100, 0.1);
  NcpBs.PlotAvgNcp(OutFNm+"AvgNcp3", NcpBs.NcpV, 100, 0.01);
  NcpBs.PlotAvgNcp(OutFNm+"AvgNcp4", NcpBs.NcpV, 100, 0.001);
  NcpBs.PlotAvgNcp(OutFNm+"AvgNcp5", NcpBs.NcpV, 100, 0.0001);
  NcpBs.PlotAvgNcp(OutFNm+"RewNcp1", NcpBs.RewNcpV, 1000, 1);
  NcpBs.PlotAvgNcp(OutFNm+"RewNcp2", NcpBs.RewNcpV, 100, 0.1);
  NcpBs.PlotAvgNcp(OutFNm+"RewNcp3", NcpBs.RewNcpV, 100, 0.01);
  NcpBs.PlotAvgNcp(OutFNm+"RewNcp4", NcpBs.RewNcpV, 100, 0.001);
  NcpBs.PlotAvgNcp(OutFNm+"RewNcp5", NcpBs.RewNcpV, 100, 0.0001);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp1", NcpBs.WhiskNcpV, 1000, 1);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp2", NcpBs.WhiskNcpV, 100, 0.1);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp3", NcpBs.WhiskNcpV, 100, 0.01);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp4", NcpBs.WhiskNcpV, 100, 0.001);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp5", NcpBs.WhiskNcpV, 100, 0.0001);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp6", NcpBs.WhiskNcpV, 100, 0.00004);
  NcpBs.PlotAvgNcp(OutFNm+"WhiskNcp7", NcpBs.WhiskNcpV, 100, 0.00005);*/
  //NcpBs.SaveTxt(OutFNm+"bestK.txt");
}
