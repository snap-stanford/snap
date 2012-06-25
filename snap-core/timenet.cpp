/////////////////////////////////////////////////
// Time Network
TTimeNet& TTimeNet::operator = (const TTimeNet& TimeNet) {
  if (this != &TimeNet) {
    TNet::operator=(TimeNet);
  }
  return *this;
}

PTimeNet TTimeNet::GetSubGraph(const TIntV& NIdV) const {
  PTimeNet NewNetPt = TTimeNet::New();
  TTimeNet& NewNet = *NewNetPt;
  NewNet.Reserve(NIdV.Len(), -1);
  int node, edge;
  TNodeI NI;
  for (node = 0; node < NIdV.Len(); node++) {
    NewNet.AddNode(NIdV[node], GetNDat(NIdV[node])); // also copy the node data
  }
  for (node = 0; node < NIdV.Len(); node++) {
    NI = GetNI(NIdV[node]);
    const int SrcNId = NI.GetId();
    for (edge = 0; edge < NI.GetOutDeg(); edge++) {
      const int OutNId = NI.GetOutNId(edge);
      if (NewNet.IsNode(OutNId)) {
        NewNet.AddEdge(SrcNId, OutNId); }
    }
  }
  NewNet.Defrag();
  return NewNetPt;
}

PTimeNENet TTimeNet::GetTimeNENet() const {
  TIntV NIdV;  GetNIdByTm(NIdV);
  PTimeNENet OutNet = TTimeNENet::New(GetNodes(), GetEdges());
  for (int i = 0; i < NIdV.Len(); i++) {
    const int Src = NIdV[i];
    const TTimeNet::TNodeI NI = GetNI(Src);
    const TSecTm SrcTm = NI.GetDat();
    if (! OutNet->IsNode(Src)) { OutNet->AddNode(Src, SrcTm); }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (! OutNet->IsNode(NI.GetOutNId(e))) { OutNet->AddNode(NI.GetOutNId(e), SrcTm); }
      OutNet->AddEdge(Src, NI.GetOutNId(e), -1, SrcTm);
    }
  }
  return OutNet;
}

void TTimeNet::GetNIdByTm(TIntV& NIdV) const {
  TVec<TKeyDat<TSecTm, TInt> > TmToNIdV(GetNodes(), 0);
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    TmToNIdV.Add(TKeyDat<TSecTm, TInt>(NodeI.GetDat(), NodeI.GetId())); }
  TmToNIdV.Sort();
  NIdV.Gen(GetNodes(), 0);
  for (int i = 0; i < TmToNIdV.Len(); i++) {
    NIdV.Add(TmToNIdV[i].Dat); }
}

// put nodes into buckets by TmUnit
void TTimeNet::GetTmBuckets(const TTmUnit& TmUnit, TTmBucketV& TmBucketV) const {
  THash<TInt, TIntV> TmIdToNIdVH;
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    const int TmId = NodeI().Round(TmUnit);
    if (! TmIdToNIdVH.IsKey(TmId)) TmIdToNIdVH.AddKey(TmId);
    TmIdToNIdVH.GetDat(TmId).Add(NodeI.GetId());
  }
  TVec<TPair<TInt, TIntV> > TmIdNIdVV;
  TmIdToNIdVH.GetKeyDatPrV(TmIdNIdVV);
  TmIdNIdVV.Sort();
  TmBucketV.Gen(TmIdNIdVV.Len());
  for (int i = 0; i < TmIdNIdVV.Len(); i++) {
    TTmBucket& Bucket = TmBucketV[i];
    Bucket.BegTm = TmIdNIdVV[i].Val1;
    Bucket.NIdV = TmIdNIdVV[i].Val2;
  }
}

void TTimeNet::GetNodeBuckets(const int NodesPerBucket, TTimeNet::TTmBucketV& TmBucketV) const {
  TIntV NIdV;
  GetNIdByTm(NIdV);
  TmBucketV.Gen(NIdV.Len() / NodesPerBucket + 1, 0);
  for (int i = 0; i < NIdV.Len(); i++) {
    const int b = i/NodesPerBucket;
    if (TmBucketV.Len() <= b) { TmBucketV.Add(TTimeNet::TTmBucket(TSecTm(b))); }
    TmBucketV[b].NIdV.Add(NIdV[i]);
  }
}

PGStatVec TTimeNet::TimeGrowth(const TTmUnit& TmUnit, const TFSet& TakeStat, const TSecTm& StartTm) const {
  PGStatVec GrowthStat = new TGStatVec(TmUnit, TakeStat);
  TTmBucketV TmBucketV;
  GetTmBuckets(TmUnit, TmBucketV);
  TIntV NodeIdV;
  TExeTm ExeTm;
  for (int t = 0; t < TmBucketV.Len(); t++) {
    NodeIdV.AddV(TmBucketV[t].NIdV); // nodes up to time T
    printf("\n=== %d/%d] %s (%d nodes)\n", t+1, TmBucketV.Len(),
      TmBucketV[t].BegTm.GetStr().CStr(), NodeIdV.Len());  ExeTm.Tick();
    if (TmBucketV[t].BegTm < StartTm) continue;
    //PNGraph PreGraph = GetSubGraph(NodeIdV, true); // renumber nodes
    PNGraph PreGraph = TSnap::ConvertSubGraph<PNGraph>(PTimeNet((TTimeNet*)this), NodeIdV); // don't renumber nodes
    GrowthStat->Add(PreGraph, TmBucketV[t].BegTm);
  }
  return GrowthStat;
}

void TTimeNet::PlotEffDiam(const TStr& FNmPref, const TStr& Desc, const TTmUnit& TmUnit,
                           const TSecTm& StartTm, const int& NDiamRuns, const bool& OnlyWcc, const bool& AlsoRewire) const {
  const TStr WccStr = OnlyWcc ? "WCC " : TStr::GetNullStr();
  TTmBucketV TmBucketV;
  GetTmBuckets(TmUnit, TmBucketV);
  TIntV NodeIdV;
  TExeTm ExeTm, Run1Tm;
  TFltTrV TmDiamV, NdsDiamV;
  TFltTrV RwTmDiamV, RwNdsDiamV;
  for (int t = 0; t < TmBucketV.Len(); t++) {
    NodeIdV.AddV(TmBucketV[t].NIdV); // nodes up to time T
    printf("\n*** %d/%d] at %s (%d nodes)\n", t+1, TmBucketV.Len(),
      TmBucketV[t].BegTm.GetStr(TmUnit).CStr(), NodeIdV.Len());  ExeTm.Tick();
    if (TmBucketV[t].BegTm < StartTm) continue;
    //PUNGraph PreGraph = GetSubUNGraph(NodeIdV, true);
    PUNGraph PreGraph = TSnap::ConvertSubGraph<PUNGraph>(PTimeNet((TTimeNet*)this), NodeIdV);
    { TMom Mom;
    for (int r = 0; r < NDiamRuns; r++) {
      printf("%d...", r+1);  Run1Tm.Tick();
      const double EffDiam = TSnap::GetAnfEffDiam(OnlyWcc ? TSnap::GetMxWcc(PreGraph) : PreGraph);
      Mom.Add(EffDiam);  printf("[%s]\r", Run1Tm.GetTmStr());
    }
    Mom.Def();
    TmDiamV.Add(TFltTr((int)TmBucketV[t].BegTm.GetInUnits(TmUnit), Mom.GetMean(), Mom.GetSDev()));
    NdsDiamV.Add(TFltTr(PreGraph->GetNodes(), Mom.GetMean(), Mom.GetSDev()));
    NdsDiamV.Sort();
    printf("  [%s]          \n", ExeTm.GetTmStr()); }
    if (AlsoRewire) {
      //PUNGraph RwGraph = TGGen::GenRndDegSeqS(PreGraph, 50, TInt::Rnd); // edge switching model
      TIntV DegSeqV(PreGraph->GetNodes(), 0);
      for (TUNGraph::TNodeI NI = PreGraph->BegNI(); NI < PreGraph->EndNI(); NI++) { DegSeqV.Add(NI.GetDeg()); }
      PUNGraph RwGraph = TSnap::GenConfModel(DegSeqV, TInt::Rnd);
      printf("Configuration model: (%d, %d) --> (%d, %d)\n", PreGraph->GetNodes(), PreGraph->GetEdges(), RwGraph->GetNodes(), RwGraph->GetEdges());
      TMom Mom;
      for (int r = 0; r < NDiamRuns; r++) {
        printf("  diam run %d...", r+1);  Run1Tm.Tick();
        const double EffDiam = TSnap::GetAnfEffDiam(OnlyWcc ? TSnap::GetMxWcc(PreGraph):PreGraph);
        Mom.Add(EffDiam);  printf(" current run [%s]\n", Run1Tm.GetTmStr());
      }
      Mom.Def();
      RwTmDiamV.Add(TFltTr((int)TmBucketV[t].BegTm.GetInUnits(TmUnit), Mom.GetMean(), Mom.GetSDev()));
      RwNdsDiamV.Add(TFltTr(PreGraph->GetNodes(), Mom.GetMean(), Mom.GetSDev()));
      RwNdsDiamV.Sort();
      printf("done with diameter. Total time [%s] \n", ExeTm.GetTmStr());
    }
    // plot
    { TGnuPlot GnuPlot("diamEff-T."+FNmPref, TStr::Fmt("%s. G(%d, %d)", Desc.CStr(), GetNodes(), GetEdges()));
    GnuPlot.SetXYLabel(TStr::Fmt("TIME [%s]", TTmInfo::GetTmUnitStr(TmUnit).CStr()), WccStr+"Effective Diameter");
    GnuPlot.AddErrBar(TmDiamV, "True", "");
    if (! RwTmDiamV.Empty()) { GnuPlot.AddErrBar(RwTmDiamV, "Rewired", "");}
    GnuPlot.SavePng(); }
    { TGnuPlot GnuPlot("diamEff-N."+FNmPref, TStr::Fmt("%s. G(%d, %d)", Desc.CStr(), GetNodes(), GetEdges()));
    GnuPlot.SetXYLabel("NODES", WccStr+"Effective Diameter");
    GnuPlot.AddErrBar(NdsDiamV, "True", "");
    if (! RwNdsDiamV.Empty()) { GnuPlot.AddErrBar(RwNdsDiamV, "Rewired", "");}
    GnuPlot.SavePng(); }
  }
}

// pretend we only have link data starting in PostTmDiam
// compare the diameter of the nodes after PostTmDiam with diameter
// of the nodes after PostTmDiam but *also* using nodes and edges
// from before PostTmDiam
void TTimeNet::PlotMissingPast(const TStr& FNmPref, const TStr& Desc, const TTmUnit& TmUnit,
                               const TSecTm& DelPreTmEdges, const TSecTm& PostTmDiam) const {
  printf("\nGrowth over time: degree distribution, Growth Power Law, Diameter.\n  %s group by %s.\n",
    FNmPref.CStr(), TTmInfo::GetTmUnitStr(TmUnit).CStr());
  printf("  Delete out-edges of pre time %s nodes.\n  Take subgraph of post year %s subgraph.\n\n",
    DelPreTmEdges.GetStr().CStr(), PostTmDiam.GetStr().CStr());
  const int NDiamRuns = 10;
  const int NSamples = 100;
  //PUNGraph FullGraph = GetUNGraph();
  PUNGraph FullGraph = TSnap::ConvertGraph<PUNGraph>(PTimeNet((TTimeNet*)this));
  // delete past
  if (DelPreTmEdges.IsDef()) {
    int NDelNodes = 0, NDelEdges = 0;
    printf("Deleting pre-%s node out-links\n", DelPreTmEdges.GetStr().CStr());
    for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
      if (NodeI() < DelPreTmEdges) {
        const int NodeId = NodeI.GetId();
        for (int edge = 0; edge < NodeI.GetOutDeg(); edge++) {
          FullGraph->DelEdge(NodeId, NodeI.GetOutNId(edge)); }
        NDelEdges += NodeI.GetOutDeg();  NDelNodes++;
      }
    }
    printf("  Deleted %d nodes out-edges (%d edges total).\n", NDelNodes, NDelEdges);
    FullGraph->Defrag(true);
  }
  PGStatVec GrowthStat = TGStatVec::New(TmUnit);
  TFltV PreDiamSDev, PreEffDiamSDev, WccDiamSDev, WccEffDiamSDev;
  TIntV NodeIdV;
  TExeTm ExeTm;
  TTmBucketV TmBucketV;
  GetTmBuckets(TmUnit, TmBucketV);
  for (int t = 0; t < TmBucketV.Len(); t++) {
    printf("\nGraph: %s (%d / %d) [%s]\n", TmBucketV[t].BegTm.GetTmStr().CStr(),
      t+1, TmBucketV.Len(), TExeTm::GetCurTm());
    // up-to-year subgraph
    NodeIdV.AddV(TmBucketV[t].NIdV); // nodes up to time T
    if (TmBucketV[t].BegTm < PostTmDiam) { continue; }
    const PUNGraph PreGraph = TSnap::GetSubGraph(FullGraph, NodeIdV, true);
    const PUNGraph WccGraph = TSnap::GetMxWcc(PreGraph);
    TIntV PostYearNIdV, WccPostYearNIdV;
    for (TUNGraph::TNodeI NI = PreGraph->BegNI(); NI < PreGraph->EndNI(); NI++) {
      if (GetNDat(NI.GetId()) >= PostTmDiam) {
        PostYearNIdV.Add(NI.GetId());
        if (WccGraph->IsNode(NI.GetId())) { WccPostYearNIdV.Add(NI.GetId()); }
      }
    }
    TMom PreDiamMom, PreEffDiamMom, WccDiamMom, WccEffDiamMom;
    // diameter of PostYearDiam subgraph using whole graph (all edges)
    int FullDiam; double EffDiam;
    for (int r = 0; r < NDiamRuns; r++) {
      if (! PostYearNIdV.Empty()) {
        TSnap::GetBfsEffDiam(PreGraph, NSamples, PostYearNIdV, false, EffDiam, FullDiam);
        PreDiamMom.Add(FullDiam);  PreEffDiamMom.Add(EffDiam);
      }
      if (! WccPostYearNIdV.Empty()) {
        TSnap::GetBfsEffDiam(WccGraph, NSamples, WccPostYearNIdV, false, EffDiam, FullDiam);
        WccDiamMom.Add(FullDiam);  WccEffDiamMom.Add(EffDiam);
      }
      printf("  diam: %d  [%s]  \r", r+1, ExeTm.GetTmStr());  ExeTm.Tick();
    }
    PreDiamMom.Def();  PreEffDiamMom.Def();
    WccDiamMom.Def();  WccEffDiamMom.Def();
    // save stat
    PGStat GraphStatPt = GrowthStat->Add(TmBucketV[t].BegTm);
    TGStat& GS = *GraphStatPt;
    GS.TakeBasicStat(PreGraph, false);
    GS.TakeBasicStat(WccGraph, true);
    GS.SetVal(gsvFullDiam, PreDiamMom.GetMean()); // mean
    GS.SetVal(gsvEffDiam, PreEffDiamMom.GetMean());
    GS.SetVal(gsvFullWccDiam, WccDiamMom.GetMean());
    GS.SetVal(gsvEffWccDiam, WccEffDiamMom.GetMean());
    GS.SetVal(gsvFullDiamDev, PreDiamMom.GetSDev()); // variance
    GS.SetVal(gsvEffDiamDev, PreEffDiamMom.GetSDev());
    GS.SetVal(gsvFullWccDiamDev, WccDiamMom.GetSDev());
    GS.SetVal(gsvEffWccDiamDev, WccEffDiamMom.GetSDev());
    { TFOut FOut("growth."+FNmPref+".gStatVec");  GrowthStat->Save(FOut); }
    GrowthStat->SaveTxt(FNmPref, TStr::Fmt("%s. MISSING PAST DIAMETER\nDelPreEdges\t%s\nPostYearDiam\t%s\n",
      Desc.CStr(), DelPreTmEdges.GetStr().CStr(), PostTmDiam.GetStr().CStr()));
  }
  // diameter plots
  //GrowthStat->PlotDiam(FNmPref, Desc + TStr::Fmt(" MISSING PAST. DelPre:%d PostYear:%d.",
  //  DelPreEdges, PostYearDiam));*/
}

void TTimeNet::PlotCCfOverTm(const TStr& FNmPref, TStr Desc, const TTmUnit& TmUnit, const int& NodesBucket) const {
  if (Desc.Empty()) { Desc = FNmPref; }
  TTimeNet::TTmBucketV TmBucketV;
  TStr XLbl;
  if (TmUnit == tmuNodes) {
    XLbl = "Number of nodes (time)";
    IAssert(NodesBucket > 0);
    GetNodeBuckets(NodesBucket, TmBucketV); }
  else {
    XLbl = TStr::Fmt("Time (%s)", TTmInfo::GetTmUnitStr(TmUnit).CStr());
    GetTmBuckets(TmUnit, TmBucketV);
  }
  TIntV NodeIdV;
  TFltPrV DegToCCfV, CcfV, OpClV, OpV;
  TVec<TTuple<TFlt, 4> > OpenClsV;
  TTuple<TFlt, 4> Tuple;
  TExeTm ExeTm;
  int XVal = 0;
  printf("Clustering coefficient over time:\n  %d edges, %d edges per bucket, %d buckets \n", GetEdges(), 100000, TmBucketV.Len());
  PUNGraph UNGraph = TSnap::ConvertGraph<PUNGraph>(PTimeNet((TTimeNet*)this));
  for (int t = 0; t < TmBucketV.Len(); t++) {
    printf("\r  %d/%d: ", t+1, TmBucketV.Len());
    NodeIdV.AddV(TmBucketV[t].NIdV); // edges up to time T
    int Open=0, Close=0;
    const PUNGraph Graph = TSnap::GetSubGraph(UNGraph, NodeIdV);
    const double CCf = TSnap::GetClustCf(Graph, DegToCCfV, Open, Close);
    if (TmUnit == tmuNodes) { XVal = Graph->GetNodes(); }
    else { XVal = TmBucketV[t].BegTm.GetInUnits(TmUnit); }
    CcfV.Add(TFltPr(XVal, CCf));
    OpClV.Add(TFltPr(XVal, Open+Close==0?0:Close/(Open+Close)));
    OpV.Add(TFltPr(XVal, Open==0?0:Close/Open));
    Tuple[0]=Graph->GetNodes();
    Tuple[1]=Graph->GetEdges();
    Tuple[2]=Close;  Tuple[3]=Open;
    OpenClsV.Add(Tuple);
    printf(" %s", ExeTm.GetStr());
    TGnuPlot::PlotValV(DegToCCfV, TStr::Fmt("ccfAt%02dtm.%s", t+1, FNmPref.CStr()),
      TStr::Fmt("%s. At time %d. Clustering Coefficient. G(%d,%d)", Desc.CStr(), t+1, Graph->GetNodes(), Graph->GetEdges()),
      "Degree", "Clustering coefficient", gpsLog10XY, false);
  }
  TGnuPlot::PlotValV(CcfV, "ccfOverTm."+FNmPref, Desc+". Average Clustering Coefficient", XLbl, "Average clustering coefficient", gpsAuto, false);
  TGnuPlot::PlotValV(OpClV, "ClsOpnTr1."+FNmPref, Desc+". Close/(Open+Closed) triads", XLbl, "Close / (Open+Closed) triads", gpsAuto, false);
  TGnuPlot::PlotValV(OpV, "ClsOpnTr2."+FNmPref, Desc+". Close/Open triads", XLbl, "Close / Open triads", gpsAuto, false);
  TGnuPlot::SaveTs(OpenClsV, "ClsOpnTr."+FNmPref+".tab", TStr::Fmt("#%s\n#Nodes\tEdges\tClosed\tOpenTriads", Desc.CStr()));
  printf("\n");
}

void TTimeNet::PlotMedianDegOverTm(const TStr& FNmPref, const TTmUnit& TmUnit, const int& NodesPerBucket) const {
  TTimeNet::TTmBucketV TmBucketV;
  TStr XLbl;
  if (TmUnit == tmuNodes) {
    XLbl = "Number of nodes (time)";  IAssert(NodesPerBucket > 0);
    GetNodeBuckets(NodesPerBucket, TmBucketV); }
  else {
    XLbl = TStr::Fmt("Time (%s)", TTmInfo::GetTmUnitStr(TmUnit).CStr());
    GetTmBuckets(TmUnit, TmBucketV); }
  printf("\n\n%s\nMedian degree over time:\n  %d edges, %d edges per bucket, %d buckets \n", FNmPref.CStr(), GetEdges(), NodesPerBucket, TmBucketV.Len());
  TFltPrV MedDegV, MedInDegV, MedOutDegV;
  TIntV NodeIdV;
  int XVal;
  PUNGraph UNGraph = TSnap::ConvertGraph<PUNGraph>(PTimeNet((TTimeNet*)this));
  PNGraph NGraph = TSnap::ConvertGraph<PNGraph>(PTimeNet((TTimeNet*)this));
  FILE  *F = fopen(("gStat-"+FNmPref+".tab").CStr(), "wt");
  fprintf(F, "UndirNodes\tUndirEdges\tUndirNonZNodes\tMedianDeg\tMeanDeg\tDirNodes\tDirEdges\tDirNonzNodes\tMedInDeg\tMedOutDeg\tMeanInDeg\tMeanOutDeg\n");
  for (int t = 0; t < TmBucketV.Len(); t++) {
    printf("\r  %d/%d: ", t+1, TmBucketV.Len());
    NodeIdV.AddV(TmBucketV[t].NIdV); // edges up to time T
    if (TmUnit == tmuNodes) { XVal = NodeIdV.Len(); }
    else { XVal = TmBucketV[t].BegTm.GetInUnits(TmUnit); }
    // un graph
    { const PUNGraph Graph = TSnap::GetSubGraph(UNGraph, NodeIdV);  TMom Mom;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) { if (NI.GetOutDeg()>0) { Mom.Add(NI.GetOutDeg());} }
    Mom.Def();  MedDegV.Add(TFltPr(XVal, Mom.GetMedian()));
    fprintf(F, "%d\t%d\t%d\t%f\t%f", Graph->GetNodes(), Graph->GetEdges(), TSnap::CntNonZNodes(Graph), (float)Mom.GetMedian(), (float)Mom.GetMean()); }
    // directed graph
    { const PNGraph Graph = TSnap::GetSubGraph<PNGraph>(NGraph, NodeIdV); TMom MomOut, MomIn;
    for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      if (NI.GetOutDeg()>0) { MomOut.Add(NI.GetOutDeg()); }
      if (NI.GetInDeg()>0) { MomIn.Add(NI.GetInDeg()); } }
    MomOut.Def();  MedOutDegV.Add(TFltPr(XVal, MomOut.GetMedian()));
    MomIn.Def();  MedInDegV.Add(TFltPr(XVal, MomIn.GetMedian()));
    fprintf(F, "\t%d\t%d\t%d\t%f\t%f\t%f\t%f\n", Graph->GetNodes(), Graph->GetEdges(), (int)TSnap::CntNonZNodes(Graph), (float)MomIn.GetMedian(), (float)MomOut.GetMedian(), (float)MomIn.GetMean(), (float)MomOut.GetMean()); }
  }
  fclose(F);
  TGnuPlot::PlotValV(MedDegV, "medDeg."+FNmPref, FNmPref+" Median degree", TTmInfo::GetTmUnitStr(TmUnit), "Median degree");
  TGnuPlot::PlotValV(MedOutDegV, "medOutDeg."+FNmPref, FNmPref+" Median OUT degree", TTmInfo::GetTmUnitStr(TmUnit), "Median OUT degree");
  TGnuPlot::PlotValV(MedInDegV, "medInDeg."+FNmPref, FNmPref+" Median IN degree", TTmInfo::GetTmUnitStr(TmUnit), "Median IN degree");
}

// load Arxiv affiliation network (papers to authors bipartite graph)
// <set1_id>  <time>  <set2_id1>  <set2_id2> ... (all ids have to be unique)
PTimeNet TTimeNet::LoadBipartite(const TStr& InFNm) {
  PTimeNet TimeNetPt = TTimeNet::New();
  TTimeNet& TimeNet = *TimeNetPt;
  PSs Ss = TSs::LoadTxt(ssfTabSep, InFNm.CStr());
  TIntH Set1IdH; // paper ids
  TStrV StrTimeV;
  for (int y = 0; y < Ss->GetYLen(); y++) {
    if (Ss->At(0, y)[0] == '#') continue; // skip comments
    if (Ss->GetXLen(y) < 3) continue;     // there must be at least one author
    const int& SrcId = Ss->At(0, y).GetInt();
    IAssert(! Set1IdH.IsKey(SrcId));
    IAssert(! TimeNet.IsNode(SrcId));
    Set1IdH.AddKey(SrcId);
    Ss->At(1, y).SplitOnAllCh('-', StrTimeV);
    const int Year = StrTimeV[0].GetInt();
    const int Month = StrTimeV[1].GetInt();
    const int Day = StrTimeV[2].GetInt();
    const TSecTm NodeTm(Year, Month, Day);
    TimeNet.AddNode(SrcId, NodeTm);
    for (int dst = 2; dst < Ss->GetXLen(y); dst++) {
      const int DstId = Ss->At(dst, y).GetInt();
      IAssert(! Set1IdH.IsKey(DstId));
      if (! TimeNet.IsNode(DstId)) { TimeNet.AddNode(DstId, NodeTm); }
      else { TimeNet.GetNDat(DstId) = TMath::Mn(NodeTm, TimeNet.GetNDat(DstId)); }
      if (! TimeNet.IsEdge(SrcId, DstId)) { TimeNet.AddEdge(SrcId, DstId); }
    }
  }
  TimeNet.Defrag();
  printf("Bipartate graph: nodes: %d  edges: %d\n", TimeNet.GetNodes(), TimeNet.GetEdges());
  printf("  Bipartate sets: %d nodes --> %d nodes\n", TSnap::CntInDegNodes(TimeNetPt, 0),
    TSnap::CntOutDegNodes(TimeNetPt, 0));
  return TimeNetPt;
}

// load Arxiv paper citation network
//   PaperFNm: <paper> <time>
//   CiteFNm:  <source> <destination>
PTimeNet TTimeNet::LoadArxiv(const TStr& PaperFNm, const TStr& CiteFNm) {
  TExeTm ExeTm;
  PTimeNet TimeNetPt = TTimeNet::New();
  TTimeNet& TimeNet = *TimeNetPt;
  printf("Arxiv citation graph (paper publication year)...\n");
  // load time data (file hep-ph-slacdates)
  char Line [1024];
  FILE *PprF = fopen(PaperFNm.CStr(), "rt");
  TStr StrId, StrTime;
  TStrV StrV, StrTimeV;
  int N = 0, DuplicateNode = 0;
  while (! feof(PprF)) {
    Line[0] = 0;
    fgets(Line, 1024, PprF);
    if (strlen(Line) == 0 || Line[0] == '#') continue;
    Line[strlen(Line)-1] = 0; // delete trailing '\n'
    TStr(Line).SplitOnWs(StrV);  IAssert(StrV.Len() == 2);
    StrId = StrV[0];  StrTime = StrV[1];  IAssert(!StrId.Empty() && !StrTime.Empty());
    StrTime.SplitOnAllCh('-', StrTimeV);  IAssert(StrTimeV.Len() == 3);
    const int NodeId = StrId.GetInt();
    if (! TimeNet.IsNode(NodeId)) {
      const int Year = StrTimeV[0].GetInt();
      const int Month = StrTimeV[1].GetInt();
      const int Day = StrTimeV[2].GetInt();
      TimeNet.AddNode(NodeId, TSecTm(Year, Month, Day));
    } else { DuplicateNode++; }
    if (++N % 10000 == 0) printf("\r  %dk", N/1000);
  }
  printf("\r  %d nodes read. %d duplicate nodes. %s\n", N, DuplicateNode, ExeTm.GetTmStr());
  fclose(PprF);
  // load citations (file hep-ph-citations)
  int NewSrcIds=0, NewDstIds=0, DupLinks=0, NewCits=0;
  FILE *CiteF = fopen(CiteFNm.CStr(), "rt");
  N = 0;  ExeTm.Tick();
  printf("Loading Arxiv citations...\n");
  TIntPrV EdgeV;
  THash<TInt, TSecTm> NIdToTimeH;
  while (! feof(CiteF)) {
    Line[0] = 0;
    fgets(Line, 1024, CiteF);
    if (strlen(Line) == 0 || Line[0] == '#') continue;
    Line[strlen(Line)-1] = 0; // delete trailing '\n'
    TStr(Line).SplitOnWs(StrV);  IAssert(StrV.Len() == 2);
    const int SrcNId = StrV[0].GetInt();
    const int DstNId = StrV[1].GetInt();
    EdgeV.Add(TIntPr(SrcNId, DstNId));
    // infer time of destination node -- earliest paper that cites the node (paper)
    if (! TimeNet.IsNode(DstNId) && TimeNet.IsNode(SrcNId)) {
      const TSecTm& SrcTm = TimeNet.GetNDat(SrcNId);
      if (! NIdToTimeH.IsKey(DstNId)) {
        NIdToTimeH.AddDat(DstNId, SrcTm);
        NewDstIds++;
      }
      else if (NIdToTimeH.GetDat(DstNId) < SrcTm) {
        NIdToTimeH.GetDat(DstNId) = SrcTm; }
    }
    if (++N % 10000 == 0) printf("\r  %dk", N/1000);
  }
  fclose(CiteF);
  // add infeered time nodes (nodes which are cited by papers with known time)
  for (int i = 0; i < NIdToTimeH.Len(); i++) {
    TimeNet.AddNode(NIdToTimeH.GetKey(i), NIdToTimeH[i]);
  }
  // add links
  for (int i = 0; i < EdgeV.Len(); i++) {
    const int SrcNId = EdgeV[i].Val1;
    const int DstNId = EdgeV[i].Val2;
    if (TimeNet.IsNode(SrcNId) && TimeNet.IsNode(DstNId)) {
      if (! TimeNet.IsEdge(SrcNId, DstNId)) { TimeNet.AddEdge(SrcNId, DstNId); }
      else { DupLinks++; }
    } else {
      if (! TimeNet.IsNode(SrcNId)) {
        NewSrcIds++;
        if (! TimeNet.IsNode(DstNId)) { NewCits++; }
      }
    }
  }
  printf("\r  %d citations read. %s\n", N, ExeTm.GetTmStr());
  printf("Graph: nodes: %d    edges: %d\n", TimeNet.GetNodes(), TimeNet.GetEdges());
  printf("Removing 0-degree nodes: %d nodes\n", TSnap::CntDegNodes(TimeNetPt, 0));
  TIntV RmNIdV;
  for (TTimeNet::TNodeI ni = TimeNet.BegNI(); ni < TimeNet.EndNI(); ni++) {
    if (ni.GetDeg() == 0) { RmNIdV.Add(ni.GetId()); }
  }
  for (int i = 0; i < RmNIdV.Len(); i++) {
    TimeNet.DelNode(RmNIdV[i]);
  }
  TimeNet.Defrag(true);
  printf("\nFinal graph: nodes: %d    edges: %d\n", TimeNet.GetNodes(), TimeNet.GetEdges());
  printf("  Duplicate citations                    : %d\n", DupLinks);
  printf("  Nodes without time which are cited     : %d (add them to graph, use time of the earliest source node)\n", NewDstIds);
  printf("  Citations between unknown time nodes   : %d\n", NewCits);
  printf("  Nodes without time which make citations: %d (do not add them into the graph)\n", NewSrcIds);
  return TimeNetPt;
}

// Patent citation network. Time units are seconds even though they mean years
PTimeNet TTimeNet::LoadPatents(const TStr& PatentFNm, const TStr& CiteFNm) {
  int N = 0;
  TExeTm ExeTm;
  PTimeNet TimeNetPt = TTimeNet::New();
  TTimeNet& TimeNet = *TimeNetPt;
  TimeNet.Reserve(4000000, 160000000);
  printf("parsing patent data (patent grant year)...\n");
  // load time data (file pat63_99.txt)
  const int& PatIdCol = 0;
  const int& GYearCol = 1;
  TStrV ColV;
  char Line [1024];
  FILE *PatF = fopen(PatentFNm.CStr(), "rt");
  fgets(Line, 1024, PatF); // skip 1st line
  while (! feof(PatF)) {
    Line[0] = 0;
    fgets(Line, 1024, PatF);
    if (strlen(Line) == 0) break;
    TStr(Line).SplitOnAllCh(',', ColV, false);
    IAssert(ColV.Len() == 23);
    const int PatentId = ColV[PatIdCol].GetInt();
    const int GrantYear = ColV[GYearCol].GetInt();
    IAssert(! TimeNet.IsNode(PatentId));
    TimeNet.AddNode(PatentId, TSecTm(GrantYear)); // pretend year is a second
    if (++N % 100000 == 0) printf("\r  %dk", N/1000);
  }
  printf("\r  %d patents read. %s\n", N, ExeTm.GetTmStr());
  fclose(PatF);
  // load citations (file cite75_99.txt)
  printf("\nLoading patent citations...\n");
  int NewSrcIds=0, NewDstIds=0, DupLinks=0, NewCits=0;
  N = 0;  ExeTm.Tick();
  TStr SrcId, DstId;
  FILE *CiteF = fopen(CiteFNm.CStr(), "rt");
  fgets(Line, 1024, CiteF); // skip 1st line
  while (! feof(CiteF)) {
    Line[0] = 0;
    fgets(Line, 1024, CiteF);
    if (strlen(Line) == 0) break;
    Line[strlen(Line)-1] = 0; // delete trailing '\n'
    TStr(Line).SplitOnCh(SrcId, ',', DstId);
    const int SrcNId = SrcId.GetInt();
    const int DstNId = DstId.GetInt();
    if (! TimeNet.IsNode(SrcNId) && ! TimeNet.IsNode(DstNId)) {
      //TimeNet.AddNode(SrcNId, TSecTm(1, 1, 1));  NewSrcIds++;
      //TimeNet.AddNode(DstNId, TSecTm(1, 1, 1));  NewDstIds++;
      NewCits++;
      continue;
    }
    else if (TimeNet.IsNode(SrcNId) && ! TimeNet.IsNode(DstNId)) {
      TimeNet.AddNode(DstNId, TimeNet.GetNDat(SrcNId));  NewDstIds++;
    }
    else if (! TimeNet.IsNode(SrcNId) && TimeNet.IsNode(DstNId)) {
      TimeNet.AddNode(SrcNId, TimeNet.GetNDat(DstNId));  NewSrcIds++;
    }
    if (! TimeNet.IsEdge(SrcNId, DstNId)) {
      TimeNet.AddEdge(SrcNId, DstNId);
    } else { DupLinks++; }
    if (++N % 100000 == 0) printf("\r  %dk", N/1000);
  }
  fclose(CiteF);
  printf("\r  %d citations read. %s\n\n", N, ExeTm.GetTmStr());
  printf("Graph: nodes: %d    edges: %d\n", TimeNet.GetNodes(), TimeNet.GetEdges());
  printf("Removing 0-degree nodes: %d nodes\n", TSnap::CntDegNodes(TimeNetPt, 0));
  TIntV RmNIdV;
  for (TTimeNet::TNodeI ni = TimeNet.BegNI(); ni < TimeNet.EndNI(); ni++) {
    if (ni.GetDeg() == 0) { RmNIdV.Add(ni.GetId()); }
  }
  for (int i = 0; i < RmNIdV.Len(); i++) {
    TimeNet.DelNode(RmNIdV[i]);
  }
  TimeNet.Defrag(true);
  printf("\nFinal graph: nodes: %d    edges: %d\n", TimeNet.GetNodes(), TimeNet.GetEdges());
  printf("  Duplicate citations                    : %d\n", DupLinks);
  printf("  Citations between unknown time nodes   : %d\n", NewCits);
  printf("  Nodes without time which make citations: %d\n", NewSrcIds);
  printf("  Nodes without time which are cited     : %d\n", NewDstIds);
  return TimeNetPt;
}

// Amazon ShareTheLove: nodes are people, we add an edge when there is a first
// recommendation between 2 people (we count each edge only once (regardless
// of how many recommendations there were between 2 persons)
PTimeNet TTimeNet::LoadAmazon(const TStr& StlFNm) {
  PTimeNet TimeNetPt = TTimeNet::New();
  TTimeNet& TimeNet = *TimeNetPt;
  TimeNet.Reserve(3953993, -1);
  printf("Amazon Share-the-Love...\n");
  char line [2024], MonthStr[4];
  int NLines=0;
  TStrV ColV;
  FILE *F = fopen(StlFNm.CStr(), "rt");
  while (! feof(F)) {
    memset(line, 0, 2024);
    fgets(line, 2024, F);
    if (strlen(line) == 0) break;
    TStr(line).SplitOnAllCh(',', ColV);
    const int SrcNId = ColV[0].GetInt();
    const int DstNId = ColV[1].GetInt();
    // time data
    TStr TmStr = ColV[2]; // time-format: 29JAN02:21:55:23
    int Year = TmStr.GetSubStr(5, 6).GetInt();
    if (Year < 10) { Year += 2000; } else { Year += 1900; }
    MonthStr[0]=toupper(TmStr[2]);  MonthStr[1]=tolower(TmStr[3]);
    MonthStr[2]=tolower(TmStr[4]);  MonthStr[3]=0;
    const int Month = TTmInfo::GetMonthN(MonthStr, lUs);
    const int Day = TmStr.GetSubStr(0, 1).GetInt();
    const int Hour = TmStr.GetSubStr(8, 9).GetInt();
    const int Min = TmStr.GetSubStr(11, 12).GetInt();
    const int Sec = TmStr.GetSubStr(14, 15).GetInt();
    // add nodes and links
    if (! TimeNet.IsNode(SrcNId)) { TimeNet.AddNode(SrcNId, TSecTm(Year, Month, Day, Hour, Min, Sec)); }
    if (! TimeNet.IsNode(DstNId)) { TimeNet.AddNode(DstNId, TSecTm(Year, Month, Day, Hour, Min, Sec)); }
    if (! TimeNet.IsEdge(SrcNId, DstNId)) { TimeNet.AddEdge(SrcNId, DstNId); }
    if (++NLines % 100000 == 0) printf("\r  %dk", NLines/1000);
  }
  fclose(F);
  printf("\r  %d lines read\n", NLines);
  printf("Graph: nodes: %d  edges: %d\n", TimeNet.GetNodes(), TimeNet.GetEdges());
  TimeNet.Defrag(true);
  return TimeNetPt;
}

/////////////////////////////////////////////////
// Time Node-Edge Network
TTimeNENet& TTimeNENet::operator = (const TTimeNENet& TimeNet) {
  if (this != &TimeNet) {
    TNet::operator=(TimeNet);
  }
  return *this;
}

PTimeNet TTimeNENet::GetTimeNet() const {
  PTimeNet NewNet = TTimeNet::New();
  NewNet->Reserve(GetNodes(), -1);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NewNet->AddNode(NI.GetId(), NI.GetDat());
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int src = EI.GetSrcNId();
    const int dst = EI.GetDstNId();
    if (! NewNet->IsEdge(src, dst)) {
      NewNet->AddEdge(src, dst); }
  }
  NewNet->Defrag();
  return NewNet;
}

// only take earliest edge between the nodes
PTimeNENet TTimeNENet::Get1stEdgeNet() const {
  PTimeNENet Net = TTimeNENet::New();
  Net->Reserve(GetNodes(), -1);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    Net->AddNode(NI.GetId(), NI.GetDat()); }
  TIntV EIdV;  GetEIdByTm(EIdV);
  TIntPrSet EdgeSet(GetEdges());
  for (int edge = 0; edge < EIdV.Len(); edge++) {
    const TEdgeI EI = GetEI(EIdV[edge]);
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (Src==Dst || EdgeSet.IsKey(TIntPr(TMath::Mn(Src, Dst), TMath::Mx(Src, Dst)))) { continue; } // take only 1st edge
    EdgeSet.AddKey(TIntPr(TMath::Mn(Src, Dst), TMath::Mx(Src, Dst)));
    Net->AddEdge(EI);
  }
  return Net;
}

PTimeNENet TTimeNENet::GetSubGraph(const TIntV& NIdV) const {
  PTimeNENet NewNetPt = TTimeNENet::New();
  TTimeNENet& NewNet = *NewNetPt;
  NewNet.Reserve(NIdV.Len(), -1);
  int node, edge;
  TNodeI NI;
  for (node = 0; node < NIdV.Len(); node++) {
    NewNet.AddNode(NIdV[node], GetNDat(NIdV[node]));
  }
  for (node = 0; node < NIdV.Len(); node++) {
    NI = GetNI(NIdV[node]);
    for (edge = 0; edge < NI.GetOutDeg(); edge++) {
      const TEdgeI EI = GetEI(NI.GetOutEId(edge));
      if (NewNet.IsNode(EI.GetDstNId())) {
        NewNet.AddEdge(EI); }
    }
  }
  NewNet.Defrag();
  return NewNetPt;
}

PTimeNENet TTimeNENet::GetESubGraph(const TIntV& EIdV) const {
  PTimeNENet NewNetPt = TTimeNENet::New();
  TTimeNENet& NewNet = *NewNetPt;
  NewNet.Reserve(-1, EIdV.Len());
  for (int edge = 0; edge < EIdV.Len(); edge++) {
    const TEdgeI Edge = GetEI(EIdV[edge]);
    if (! NewNet.IsNode(Edge.GetSrcNId()))
      NewNet.AddNode(GetNI(Edge.GetSrcNId()));
    if (! NewNet.IsNode(Edge.GetDstNId()))
      NewNet.AddNode(GetNI(Edge.GetDstNId()));
    NewNet.AddEdge(Edge);
  }
  NewNet.Defrag();
  return NewNetPt;
}

// assumes that the edges of the network are sorted in time
PTimeNENet TTimeNENet::GetGraphUpToTm(const TSecTm& MaxEdgeTm) const {
  PTimeNENet NewNetPt = TTimeNENet::New();
  TTimeNENet& NewNet = *NewNetPt;
  TSecTm PrevTm;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI() > MaxEdgeTm) { break; }
    if (! NewNet.IsNode(EI.GetSrcNId()))
      NewNet.AddNode(GetNI(EI.GetSrcNId()));
    if (! NewNet.IsNode(EI.GetDstNId()))
      NewNet.AddNode(GetNI(EI.GetDstNId()));
    NewNet.AddEdge(EI);
    IAssert(! PrevTm.IsDef() || PrevTm <= EI()); // edge times must be sorted
    PrevTm = EI();
  }
  NewNet.Defrag();
  return NewNetPt;
}

void TTimeNENet::SortNodeEdgeTimes() {
  NodeH.SortByDat(true);
  EdgeH.SortByDat(true);
}

// node time must be smaller than times of incoming or outgoing edges
void TTimeNENet::UpdateNodeTimes() {
  int Cnt = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TSecTm& NodeTm = NI();
    for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
      const TSecTm& EdgeTm = GetEDat(NI.GetOutEId(edge));
      if (! NodeTm.IsDef() || EdgeTm < NodeTm) { NodeTm = EdgeTm; Cnt++; }
    }
    for (int edge = 0; edge < NI.GetInDeg(); edge++) {
      const TSecTm& EdgeTm = GetEDat(NI.GetInEId(edge));
      if (! NodeTm.IsDef() || EdgeTm < NodeTm) { NodeTm = EdgeTm; Cnt++; }
    }
  }
  printf("Update node times: %d/%d updates\n", Cnt, GetNodes());
}

void TTimeNENet::SetNodeTmToFirstEdgeTm() {
  int Cnt = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (NI.GetDeg() == 0) { continue; }
    TSecTm NodeTm;
    for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
      const TSecTm& EdgeTm = GetEDat(NI.GetOutEId(edge));  IAssert(EdgeTm.IsDef());
      if (! NodeTm.IsDef() || EdgeTm < NodeTm) { NodeTm = EdgeTm; Cnt++; }
    }
    for (int edge = 0; edge < NI.GetInDeg(); edge++) {
      const TSecTm& EdgeTm = GetEDat(NI.GetInEId(edge));  IAssert(EdgeTm.IsDef());
      if (! NodeTm.IsDef() || EdgeTm < NodeTm) { NodeTm = EdgeTm; Cnt++; }
    }
    GetNDat(NI.GetId()) = NodeTm;
  }
  printf("Node times set: %d/%d updates\n", Cnt, GetNodes());
}

// shuffle edge arrivals
void TTimeNENet::SetRndEdgeTimes(const int& MinTmEdge) {
  printf("Shuffling last %d (%d%%) edge arrival times..\n", GetEdges()-MinTmEdge, int(100.0*(GetEdges()-MinTmEdge)/double(GetEdges())));
  TIntV RndEIdV;  GetEIdByTm(RndEIdV);
  TIntV TrueEIdV = RndEIdV;
  TSecTmV TrueTmV;
  const int SwapLen = RndEIdV.Len()-MinTmEdge;
  for (int R = 0; R < 10; R++) {
    for (int i = MinTmEdge; i < RndEIdV.Len(); i++) {
      RndEIdV.Swap(TInt::Rnd.GetUniDevInt(SwapLen)+MinTmEdge, TInt::Rnd.GetUniDevInt(SwapLen)+MinTmEdge); }
  }
  for (int e = 0; e < TrueEIdV.Len(); e++) {
    TrueTmV.Add(GetEDat(TrueEIdV[e])); }
  for (int e = 0; e < RndEIdV.Len(); e++) {
    GetEDat(RndEIdV[e]) = TrueTmV[e]; }
  UpdateNodeTimes();
}

void TTimeNENet::DumpTimeStat() const {
  TSecTm MnNodeTm, MxNodeTm;
  TSecTm MnEdgeTm, MxEdgeTm;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const TSecTm NodeTm = NI();
    if (! MnNodeTm.IsDef() || MnNodeTm>NodeTm) { MnNodeTm = NodeTm; }
    if (! MxNodeTm.IsDef() || MxNodeTm<NodeTm) { MxNodeTm = NodeTm; }
  }
  printf("Node times:\n  %s\n  %s\n", MnNodeTm.GetStr().CStr(), MxNodeTm.GetStr().CStr());
  for (TEdgeI EI= BegEI(); EI < EndEI(); EI++) {
    const TSecTm EdgeTm = EI();
    if (! MnEdgeTm.IsDef() || MnEdgeTm>EdgeTm) { MnEdgeTm = EdgeTm; }
    if (! MxEdgeTm.IsDef() || MxEdgeTm<EdgeTm) { MxEdgeTm = EdgeTm; }
  }
  printf("Edge times:\n  %s\n  %s\n", MnEdgeTm.GetStr().CStr(), MxEdgeTm.GetStr().CStr());
}

void TTimeNENet::GetNIdByTm(TIntV& NIdV) const {
  TVec<TKeyDat<TSecTm, TInt> > TmToNIdV(GetNodes(), 0);
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    TmToNIdV.Add(TKeyDat<TSecTm, TInt>(NodeI.GetDat(), NodeI.GetId())); }
  TmToNIdV.Sort();
  NIdV.Gen(GetNodes(), 0);
  for (int i = 0; i < TmToNIdV.Len(); i++) {
    NIdV.Add(TmToNIdV[i].Dat); }
}

void TTimeNENet::GetEIdByTm(TIntV& EIdV) const {
  TVec<TKeyDat<TSecTm, TInt> > TmToEIdV(GetEdges(), 0);
  for (TEdgeI EI= BegEI(); EI < EndEI(); EI++) {
    TmToEIdV.Add(TKeyDat<TSecTm, TInt>(EI.GetDat(), EI.GetId())); }
  TmToEIdV.Sort();
  EIdV.Gen(GetEdges(), 0);
  for (int i = 0; i < TmToEIdV.Len(); i++) {
    EIdV.Add(TmToEIdV[i].Dat); }
}

void TTimeNENet::GetTmBuckets(const TTmUnit& TmUnit, TTimeNet::TTmBucketV& TmBucketV) const {
  THash<TInt, TIntV> TmIdToNIdVH;
  TIntV NIdV;  GetNIdByTm(NIdV);
  for (int n = 0; n < NIdV.Len(); n++) {
    const int TmId = GetNDat(NIdV[n]).Round(TmUnit).GetAbsSecs();
    if (! TmIdToNIdVH.IsKey(TmId)) { TmIdToNIdVH.AddKey(TmId); }
    TmIdToNIdVH.GetDat(TmId).Add(NIdV[n]);
  }
  TVec<TPair<TInt, TIntV> > TmIdNIdVV;
  TmIdToNIdVH.GetKeyDatPrV(TmIdNIdVV);
  TmIdNIdVV.Sort();
  TmBucketV.Gen(TmIdNIdVV.Len());
  for (int i = 0; i < TmIdNIdVV.Len(); i++) {
    TTimeNet::TTmBucket& Bucket = TmBucketV[i];
    Bucket.BegTm = TmIdNIdVV[i].Val1;
    Bucket.NIdV = TmIdNIdVV[i].Val2;
  }
}

void TTimeNENet::GetEdgeTmBuckets(const TTmUnit& TmUnit, TTimeNet::TTmBucketV& TmBucketV) const {
  THash<TInt, TIntV> TmIdToEIdVH;
  TIntV EIdV;  GetEIdByTm(EIdV);
  for (int e = 0; e < EIdV.Len(); e++) {
    const int TmId = GetEDat(EIdV[e]).Round(TmUnit).GetAbsSecs();
    if (! TmIdToEIdVH.IsKey(TmId)) { TmIdToEIdVH.AddKey(TmId); }
    TmIdToEIdVH.GetDat(TmId).Add(EIdV[e]);
  }
  TVec<TPair<TInt, TIntV> > TmIdEIdVV;
  TmIdToEIdVH.GetKeyDatPrV(TmIdEIdVV);
  TmIdEIdVV.Sort();
  TmBucketV.Gen(TmIdEIdVV.Len());
  for (int i = 0; i < TmIdEIdVV.Len(); i++) {
    TTimeNet::TTmBucket& Bucket = TmBucketV[i];
    Bucket.BegTm = TmIdEIdVV[i].Val1;
    Bucket.NIdV = TmIdEIdVV[i].Val2;
  }
}

void TTimeNENet::GetNodeBuckets(const int NodesPerBucket, TTimeNet::TTmBucketV& TmBucketV) const {
  TIntV NIdV;  GetNIdByTm(NIdV);
  TmBucketV.Gen(NIdV.Len() / NodesPerBucket + 1, 0);
  for (int i = 0; i < NIdV.Len(); i++) {
    const int b = i/NodesPerBucket;
    if (TmBucketV.Len() <= b) { TmBucketV.Add(TTimeNet::TTmBucket(TSecTm(b))); }
    TmBucketV[b].NIdV.Add(NIdV[i]);
  }
}

void TTimeNENet::GetEdgeBuckets(const int EdgesPerBucket, TTimeNet::TTmBucketV& TmBucketV) const {
  TIntV EIdV;  GetEIdByTm(EIdV);
  TmBucketV.Gen(EIdV.Len()/EdgesPerBucket + 1, 0);
  for (int i = 0; i < EIdV.Len(); i++) {
    const int b = i/EdgesPerBucket;
    if (TmBucketV.Len() <= b) { TmBucketV.Add(TTimeNet::TTmBucket(TSecTm(b))); }
    TmBucketV[b].NIdV.Add(EIdV[i]);
  }
}

// get edges that close triangles over time
int TTimeNENet::GetTriadEdges(TIntV& TriadEIdV) const {
  PUNGraph Graph = TUNGraph::New(GetNodes(), GetEdges());
  TIntV EIdV;  GetEIdByTm(EIdV);
  TriadEIdV.Clr();
  TExeTm ExeTm;
  for (int edge = 0; edge < EIdV.Len(); edge++) {
    const TEdgeI EI = GetEI(EIdV[edge]);
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (Src==Dst || Graph->IsEdge(Src, Dst)) { continue; } // take only 1st edge
    if (! Graph->IsNode(Src)) { Graph->AddNode(Src); }
    if (! Graph->IsNode(Dst)) { Graph->AddNode(Dst); }
    if (TSnap::GetCmnNbrs(Graph, Src, Dst) > 0) { TriadEIdV.Add(EIdV[edge]); }
    Graph->AddEdge(Src, Dst);
    if (edge % 10000 == 0) {
      printf("\redges %dk / %dk: triangle edges: %dk [total %s]", edge/1000, EIdV.Len()/1000,
        TriadEIdV.Len()/1000, ExeTm.GetStr()); }
  }
  return Graph->GetEdges();
}

PGStatVec TTimeNENet::TimeGrowth(const TTmUnit& TimeStep, const TFSet& TakeStat, const TSecTm& StartTm) const {
  TExeTm ExeTm;
  PGStatVec GStatVec = TGStatVec::New(TimeStep, TakeStat);
  TTimeNet::TTmBucketV TmBucketV;
  GetEdgeTmBuckets(TimeStep, TmBucketV);
  const PNEGraph FullGraph = TSnap::ConvertGraph<PNEGraph>(PTimeNENet((TTimeNENet*)this));
  TIntV EdgeIdV;
  for (int t = 0; t < TmBucketV.Len(); t++) {
    EdgeIdV.AddV(TmBucketV[t].NIdV); // edges up to time T
    printf("\n***%d/%d: %s (%d edges) ", t+1, TmBucketV.Len(), TmBucketV[t].BegTm.GetStr().CStr(), EdgeIdV.Len());  ExeTm.Tick();
    if (TmBucketV[t].BegTm < StartTm) { continue; }
    const PNEGraph PreGraph = TSnap::GetESubGraph(FullGraph, EdgeIdV);
    GStatVec->Add(PreGraph, TmBucketV[t].BegTm);
    printf("  [%s]\n", ExeTm.GetTmStr());
    //{ TFOut FOut("LinkedIn.GStatVec");  GStatVec->Save(FOut); }
  }
  return GStatVec;
}

// take network from last TakeNTmUnits and measure properties
PGStatVec TTimeNENet::TimeGrowth(const TStr& FNmPref, const TStr& Desc, const TFSet& TakeStat, const int& NDiamRuns,
                            const TTmUnit& TmUnit, const int& TakeNTmUnits, const bool& LinkBWays) const {
  TGStat::NDiamRuns = NDiamRuns;
  PGStatVec GrowthStat = TGStatVec::New(TmUnit, TakeStat);
  TTimeNet::TTmBucketV TmBucketV;
  GetEdgeTmBuckets(TmUnit, TmBucketV);
  TIntV EdgeIdV;
  TExeTm ExeTm;
  for (int t = 0; t < TmBucketV.Len(); t++) {
    // take graph over last TakeNTmUnits time units
    if (TakeNTmUnits == -1) {
      EdgeIdV.AddV(TmBucketV[t].NIdV); }
    else {
      if (t < TakeNTmUnits) { continue; }
      EdgeIdV.Clr(false);
      for (int i = t-TakeNTmUnits; i < t; i++) { EdgeIdV.AddV(TmBucketV[i].NIdV); }
    }
    printf("*** %s (%d edges)\n", TmBucketV[t].BegTm.GetStr().CStr(), EdgeIdV.Len());  ExeTm.Tick();
    PNEGraph PreGraph = TSnap::ConvertESubGraph<PNEGraph>(PTimeNENet((TTimeNENet*)this), EdgeIdV);
    if (LinkBWays) {
      TIntV KeepEIdV; // keep only nodes that have in- and out-links (send and receive email)
      for (TNEGraph::TEdgeI EI = PreGraph->BegEI(); EI < PreGraph->EndEI(); EI++) {
        if (PreGraph->IsEdge(EI.GetDstNId(), EI.GetSrcNId(), true)) { KeepEIdV.Add(EI.GetId()); }
      }
      PreGraph = TSnap::GetESubGraph(PreGraph, KeepEIdV);
    }
    // take statistics
    GrowthStat->Add(PreGraph, TmBucketV[t].BegTm);
    { TFOut FOut(TStr::Fmt("growth.%s.gStatVec", FNmPref.CStr()));
    GrowthStat->Save(FOut); }
    GrowthStat->SaveTxt(FNmPref, Desc);
    printf("  [%s]\n", ExeTm.GetTmStr());
  }
  return GrowthStat;
}

void TTimeNENet::PlotEffDiam(const TStr& FNmPref, const TStr& Desc, const TTmUnit& TmUnit,
                             const TSecTm& StartTm, const int& NDiamRuns, const bool& OnlyWcc) const {
  TTimeNet::TTmBucketV TmBucketV;
  GetEdgeTmBuckets(TmUnit, TmBucketV);
  PNEGraph FullGraph = TSnap::ConvertGraph<PNEGraph>(PTimeNENet((TTimeNENet*)this));
  TIntV EdgeIdV;
  TExeTm ExeTm, Run1Tm;
  TFltTrV TmDiamV, NdsDiamV;
  for (int t = 0; t < TmBucketV.Len(); t++) {
    EdgeIdV.AddV(TmBucketV[t].NIdV); // edges up to time T
    printf("\n*** %s (%d edges)\n", TmBucketV[t].BegTm.GetStr(TmUnit).CStr(), EdgeIdV.Len());  ExeTm.Tick();
    if (TmBucketV[t].BegTm < StartTm) continue;
    PNGraph PreGraph = TSnap::ConvertESubGraph<PNGraph>(FullGraph, EdgeIdV);
    TMom Mom;
    double EffDiam = 0.0;
    for (int r = 0; r < NDiamRuns; r++) {
      printf("%d...", r+1);  Run1Tm.Tick();
      if (OnlyWcc) { EffDiam = TSnap::GetAnfEffDiam(TSnap::GetMxWcc(PreGraph)); }
      else { EffDiam = TSnap::GetAnfEffDiam(PreGraph); }
      Mom.Add(EffDiam);
      printf("[%s]\r", Run1Tm.GetTmStr());
    }
    Mom.Def();
    TmDiamV.Add(TFltTr(TmBucketV[t].BegTm.Round(TmUnit).GetAbsSecs(), Mom.GetMean(), Mom.GetSDev()));
    NdsDiamV.Add(TFltTr(PreGraph->GetNodes(), Mom.GetMean(), Mom.GetSDev()));
    NdsDiamV.Sort();
    printf("  [%s]          \n", ExeTm.GetTmStr());
    const TStr WccStr = OnlyWcc ? "WCC " : TStr::GetNullStr();
    { TGnuPlot GnuPlot("diamEff1."+FNmPref, TStr::Fmt("%s. G(%d, %d). %d RUNS.", Desc.CStr(), GetNodes(), GetEdges(), NDiamRuns));
    GnuPlot.SetXYLabel(TStr::Fmt("TIME [%s]", TTmInfo::GetTmUnitStr(TmUnit).CStr()), "AVERAGE "+WccStr+"Effective Diameter");
    GnuPlot.AddErrBar(TmDiamV, "", "");
    GnuPlot.SavePng(); }
    { TGnuPlot GnuPlot("diamEff2."+FNmPref, TStr::Fmt("%s. G(%d, %d). %d RUNS.", Desc.CStr(), GetNodes(), GetEdges(), NDiamRuns));
    GnuPlot.SetXYLabel("NODES", "AVERAGE "+WccStr+"Effective Diameter");
    GnuPlot.AddErrBar(NdsDiamV, "", "");
    GnuPlot.SavePng(); }
  }
}

// pretend we only have link data starting in PostTmDiam
// compare the diameter of the nodes after PostTmDiam with diameter
// of the nodes after PostTmDiam but *also* using nodes and edges
// from before PostTmDiam
void TTimeNENet::PlotMissingPast(const TStr& FNmPref, const TStr& Desc, const TTmUnit& TmUnit,
                                 const TSecTm& DelPreTmEdges, const TSecTm& PostTmDiam, const bool& LinkBWays) {
  printf("\nGrowth over time: degree distribution, Growth Power Law, Diameter.\n  %s group by %s.\n",
    FNmPref.CStr(), TTmInfo::GetTmUnitStr(TmUnit).CStr());
  printf("  Delete out-edges of pre time %s nodes.\n  Take subgraph of post year %s subgraph.\n\n",
    DelPreTmEdges.GetStr().CStr(), PostTmDiam.GetStr().CStr());
  // run diameter
  /*const int NSamples = 100;
  const int NDiamRuns = 10;
  // delete past
  if (DelPreEdges != -1) {
    TIntV EdgeIdV;
    printf("Deleting pre-year %d edges\n", DelPreEdges);
    for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
      if (EI().GetYear() < DelPreEdges) { EdgeIdV.Add(EI.GetId()); } }
    for (int e = 0; e < EdgeIdV.Len(); e++) { DelEdge(EdgeIdV[e]); }
    printf("  Deleted %d edges (out of %d edges total).\n", EdgeIdV.Len(), GetEdges());
  }
  PNEGraph FullGraph = GetNEGraph();
  TSnap::DelZeroDegNodes(FullGraph);

  PGrowthStat GrowthStat = TGrowthStat::New(TmUnit, TGraphStat::AllStat());
  TFltV PreDiamSDev, PreEffDiamSDev, WccDiamSDev, WccEffDiamSDev;
  TIntV EdgeIdV;
  TExeTm ExeTm;
  TTimeNet::TTmBucketV EdgeTmBucketV;
  GetEdgeTmBuckets(TmUnit, EdgeTmBucketV);
  for (int t = 0; t < EdgeTmBucketV.Len(); t++) {
    printf("\nGraph: %s (%d / %d) [%s]\n", EdgeTmBucketV[t].BegTm.GetTmStr().CStr(),
      t+1, EdgeTmBucketV.Len(), TExeTm::GetCurTm());
    // up-to-year subgraph
    EdgeIdV.AddV(EdgeTmBucketV[t].NIdV); // nodes up to time T
    if (EdgeTmBucketV[t].BegTm.GetYear() < PostYearDiam) continue;

    const PNEGraph PreNEGraph = EdgeBothWays ? FullGraph->GetEdgeSubGraph(EdgeIdV) : FullGraph;
    PNGraph PreGraph = PreNEGraph->GetBWayGraph();
    PNGraph WccGraph = TSnap::GetMxWcc(PreGraph);

    // find nodes that are not in missing past
    TIntV PostYearNIdV, WccPostYearNIdV;
    THash<TIntPr, TInt> PostYearEdgeH;
    for (TNEGraph::TEdgeI EI = PreNEGraph->BegEI(); EI < PreNEGraph->EndEI(); EI++) {
      if (GetEDat(EI.GetId()).GetYear() >= PostYearDiam) {
        PostYearEdgeH.AddKey(TIntPr(EI.GetSrcNId(), EI.GetDstNId())); }
    }
    TIntH PostYearNIdH;
    for (int i = 0; i < PostYearEdgeH.Len(); i++) {
      if ((! EdgeBothWays) || (EdgeBothWays && PostYearEdgeH.IsKey(TIntPr(PostYearEdgeH.GetKey(i).Val2, PostYearEdgeH.GetKey(i).Val1)))) { //reverse edge exists
        PostYearNIdH.AddKey(PostYearEdgeH.GetKey(i).Val1);
        PostYearNIdH.AddKey(PostYearEdgeH.GetKey(i).Val2);
      }
    }
    PostYearNIdH.GetKeyV(PostYearNIdV);
    for (int i = 0; i < PostYearNIdV.Len(); i++) {
      if (WccGraph->IsNode(PostYearNIdV[i])) {
        WccPostYearNIdV.Add(PostYearNIdV[i]); }
    }

    // diameter of PostYearDiam subgraph using whole graph (all edges)
    TMom PreDiamMom, PreEffDiamMom, WccDiamMom, WccEffDiamMom;
    for (int r = 0; r < NDiamRuns; r++) {
      if (! PostYearNIdV.Empty()) {
        PreDiamMom.Add(-1); //PreDiamMom.Add(TSnap::GetBfsFullDiam(PreGraph, NSamples, PostYearNIdV, false));
        PreEffDiamMom.Add(TSnap::GetBfsEffDiam(PreGraph, NSamples, PostYearNIdV, false));
      }
      if (! WccPostYearNIdV.Empty()) {
        //WccDiamMom.Add(TSnap::GetBfsFullDiam(WccGraph, NSamples, WccPostYearNIdV, false));
        //WccEffDiamMom.Add(TSnap::GetBfsEffDiam(WccGraph, NSamples, WccPostYearNIdV, false));
        WccDiamMom.Add(-1);  WccEffDiamMom.Add(-1);
      }
      printf("  diam: %d  [%s]  %.2f\r", r+1, ExeTm.GetTmStr(), PreEffDiamMom.GetValV().Last().Val);  ExeTm.Tick();
    }
    PreDiamMom.Def();  PreEffDiamMom.Def();
    WccDiamMom.Def();  WccEffDiamMom.Def();
    // save stat
    PGraphStat GraphStatPt = GrowthStat->Add(EdgeTmBucketV[t].BegTm);
    TGraphStat& GraphStat = *GraphStatPt;
    GraphStat.Nodes = PreGraph->GetNodes();
    GraphStat.NonZNodes = PreGraph->GetNodes() - TSnap::CntDegNodes<PNGraph>(PreGraph, 0);
    GraphStat.Srcs =  GraphStat.Nodes - TSnap::CntOutDegNodes<PNGraph>(PreGraph, 0);
    GraphStat.Edges = PreGraph->GetEdges();
    GraphStat.WccNodes= WccGraph->GetNodes();
    GraphStat.WccEdges = WccGraph->GetEdges();
    GraphStat.FullDiam = PreDiamMom.GetMean(); // mean
    GraphStat.EffDiam = PreEffDiamMom.GetMean();
    GraphStat.FullWccDiam = WccDiamMom.GetMean();
    GraphStat.EffWccDiam = WccEffDiamMom.GetMean();
    GraphStat.FullDiamDev = PreDiamMom.GetSDev(); // variance
    GraphStat.EffDiamDev = PreEffDiamMom.GetSDev();
    GraphStat.FullWccDiamDev = WccDiamMom.GetSDev();
    GraphStat.EffWccDiamDev = WccEffDiamMom.GetSDev();
    { TFOut FOut("growth."+FNmPref+".bin");
    GrowthStat->Save(FOut); }
    const TStr BigDesc = TStr::Fmt("%s. MISSING PAST DIAMETER\nDelPreEdges\t%d\nPostYearDiam\t%d\n",
      Desc.CStr(), DelPreEdges, PostYearDiam);
    GrowthStat->SaveTxt(FNmPref, BigDesc);
  }
  // diameter plots
  GrowthStat->PlotDiam(FNmPref, Desc + TStr::Fmt(" MISSING PAST. DelPre:%d PostYear:%d.",
    DelPreEdges, PostYearDiam));
    */
}

// plot time difference of the node ages when an edge arrives
/*void TTimeNENet::PlotEdgeNodeTimeDiff(const TStr& FNmPref, TStr Desc) const {
  if (Desc.Empty()) { Desc = FNmPref; }
  printf("Edge node time differences:\n");
  THash<TInt, TInt> NodeAgeH, Node1AgeH; // (time_dst - time_src), only 1st edge
  THash<TInt, TInt> SrcTmH, DstTmH; // time_edge - time_src/time_dst)
  PUNGraph Graph = TUNGraph::New(GetNodes(), -1);
  TIntV EIdV;  GetEIdByTm(EIdV);
  TIntH DegCntH(10, true);
  TExeTm ExeTm;
  for (int edge = 0; edge < EIdV.Len(); edge++) {
    const TEdgeI EI = GetEI(EIdV[edge]);
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (Src==Dst || Graph->IsEdge(Src, Dst)) { continue; } // take only 1st edge
    if (! Graph->IsNode(Src)) { Graph->AddNode(Src); }
    if (! Graph->IsNode(Dst)) { Graph->AddNode(Dst); }
    const int SrcDeg = Graph->GetNode(Src).GetOutDeg();
    const int DstDeg = Graph->GetNode(Dst).GetInDeg();
    const int EdgeTm = EI.GetDat().GetAbsSecs();
    const int SrcTm = EI.GetSrcNDat().GetAbsSecs();
    const int DstTm = EI.GetDstNDat().GetAbsSecs();
    NodeAgeH.AddDat((DstTm-SrcTm)/3600) += 1;
    if (SrcDeg == 0 || DstDeg == 0) {
      Node1AgeH.AddDat((DstTm-SrcTm)/3600) += 1; }
    SrcTmH.AddDat((EdgeTm-SrcTm)/3600) += 1;
    DstTmH.AddDat((EdgeTm-DstTm)/3600) += 1;
    // add edge
    Graph->AddEdge(Src, Dst);
    if (edge % 1000 == 0) {
      printf("\r%dk / %dk [total %s]", edge/1000, EIdV.Len()/1000, ExeTm.GetStr()); }
  }
  TGnuPlot::PlotValCntH(NodeAgeH, "tmDstSrc."+FNmPref, TStr::Fmt("%s, Node time difference.", Desc.CStr()),
    "Time(destination) - Time(source) [hours]", "Count");
  TGnuPlot::PlotValCntH(Node1AgeH, "tmDstSrc1."+FNmPref, TStr::Fmt("%s. 1-ST EDGE of one of the nodes.", Desc.CStr()),
    "Time(destination) - Time(source) [hours]", "Count");
  TGnuPlot::PlotValCntH(SrcTmH, "tmEdgeSrc."+FNmPref, TStr::Fmt("%s. Edge time - source node time.", Desc.CStr()),
    "Edge time - source node time [hours]", "Count");
  TGnuPlot::PlotValCntH(DstTmH, "tmEdgeDst."+FNmPref, TStr::Fmt("%s. Edge time - destination node time.", Desc.CStr()),
    "Edge time - destination node time [hours]", "Count");
  printf("\n");
}

// plot the distribution whether the edge to node that closes the triad was
// chosen random or preferentially
void TTimeNENet::PlotCloseTriad(const TStr& FNmPref, TStr Desc, const bool& OnlyBackEdges) const {
  if (Desc.Empty()) { Desc = FNmPref; }
  printf("Random vs. preferential triad closing: %s\n", OnlyBackEdges?"OnlyBackEdges":"");
  THash<TInt, TInt> NodeAgeH, Node1AgeH; // (time_dst - time_src), only 1st edge
  THash<TInt, TInt> SrcTmH, DstTmH; // time_edge - time_src/time_dst)
  PUNGraph Graph = TUNGraph::New(GetNodes(), -1);
  TIntV EIdV;  GetEIdByTm(EIdV);
  TIntV CmnV, SrcNbrV, DstNbrV;
  TExeTm ExeTm;
  TFltV RndRdnV, PrefPrefV, PrefRndV, RndPrefV;
  TFltFltH RndRndH, PrefPrefH, PrefRndH, RndPrefH;
  int TriadEdges = 0;
  FILE *F = fopen(TStr::Fmt("%s-prob.tab", FNmPref.CStr()).CStr(), "wt");
  fprintf(F, "#Probability of picking a node that closes the triangle (pick preferentially vs. uniformly at random)\n");
  fprintf(F, "#i\tEId\tRndRnd\tRndPref\tPrefRnd\tPrefPref\n");
  for (int edge = 0; edge < EIdV.Len(); edge++) {
    const TEdgeI EI = GetEI(EIdV[edge]);
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (Src==Dst || Graph->IsEdge(Src, Dst)) { continue; } // take only 1st edge
    if (! Graph->IsNode(Src)) { Graph->AddNode(Src); }
    if (! Graph->IsNode(Dst)) { Graph->AddNode(Dst); }
    if (OnlyBackEdges && EI.GetSrcNDat().GetAbsSecs()-EI.GetDstNDat().GetAbsSecs() < 0) {
      Graph->AddEdge(Src, Dst);  continue;
    }
    // find common neighbors
    const TUNGraph::TNodeI SrcNI = Graph->GetNI(Src);
    const TUNGraph::TNodeI DstNI = Graph->GetNI(Dst);
    SrcNbrV.Clr(false);  DstNbrV.Clr(false);
    for (int i = 0; i < SrcNI.GetOutDeg(); i++) { SrcNbrV.Add(SrcNI.GetOutNId(i)); }
    for (int i = 0; i < DstNI.GetOutDeg(); i++) { DstNbrV.Add(DstNI.GetOutNId(i)); }
    SrcNbrV.Sort();  DstNbrV.Sort();
    SrcNbrV.Intrs(DstNbrV, CmnV);
    if (! CmnV.Empty()) {
      // triad completion
      const int SrcDeg = Graph->GetNode(Src).GetOutDeg();
      const int DstDeg = Graph->GetNode(Dst).GetInDeg();
      double RndRnd=0, RndPref=0, PrefRnd=0, PrefPref=0;
      int AllNbrDegSum=0;//, NbrDegSum = 0;
      //for (int i = 0; i < CmnV.Len(); i++) {
      //  NbrDegSum += Graph->GetNode(CmnV[i]).GetOutDeg(); }
      for (int i = 0; i < SrcNI.GetOutDeg(); i++) {
        AllNbrDegSum += Graph->GetNode(SrcNI.GetOutNId(i)).GetOutDeg(); }
      // uniform-uniform node selection = 1/|cmn| sum_{i\in cmn} 1/(d_i-1)
      // preferential-uniform
      for (int i = 0; i < CmnV.Len(); i++) {
        const int Deg = Graph->GetNode(CmnV[i]).GetOutDeg();
        RndRnd += (1.0/double(SrcDeg)) * (1.0/double(Deg-1));
        PrefRnd +=  (Deg/double(AllNbrDegSum)) * (1.0/double(Deg-1));
      }
      // uniform-preferential selection = 1/|cmn| sum_{i\in cmn} d_t / sum_{i--j} d_j
      // preferential-preferential
      for (int i = 0; i < CmnV.Len(); i++) {
        const TUNGraph::TNode N = Graph->GetNode(CmnV[i]);
        const int Deg = N.GetOutDeg();
        int DegSum = 0;
        for (int j = 0; j < N.GetOutDeg(); j++) {
          if (N.GetOutNId(j) != Src) {
            DegSum += Graph->GetNode(N.GetOutNId(j)).GetOutDeg(); }
        }
        RndPref += 1.0/double(SrcDeg) * DstDeg/double(DegSum);
        PrefPref += (Deg/double(AllNbrDegSum)) * (DstDeg/double(DegSum));
      }
      IAssert(0.0<RndRnd && RndRnd<=1.0);   IAssert(0.0<RndPref && RndPref<=1.0);
      IAssert(0.0<PrefRnd && PrefRnd<=1.0);  IAssert(0.0<PrefPref && PrefPref<=1.0);
      RndRndH.AddDat(TMath::Round(RndRnd, 2)) += 1.0;
      RndPrefH.AddDat(TMath::Round(RndPref, 2)) += 1.0;
      PrefRndH.AddDat(TMath::Round(PrefRnd, 2)) += 1.0;
      PrefPrefH.AddDat(TMath::Round(PrefPref, 2)) += 1.0;
      fprintf(F, "%d\t%d\t%g\t%g\t%g\t%g\n", edge, EI.GetId(), RndRnd, RndPref, PrefRnd, PrefPref);
      TriadEdges++;
    }
    Graph->AddEdge(Src, Dst);
    if (edge % 1000 == 0) {
      printf("\r%dk / %dk triad edges %dk [total %s]", edge/1000, EIdV.Len()/1000, TriadEdges/1000, ExeTm.GetStr()); }
  }
  fclose(F);
  printf("\nRandom vs. preferential triad closing\n");
  printf("All edges:            %d\n", Graph->GetEdges());
  printf("Triad closing edges:  %d\n", TriadEdges);
  // plot distribution
  TFltPrV RndRndPrV;   RndRndH.GetKeyDatPrV(RndRndPrV);     RndRndPrV.Sort();
  TFltPrV RndPrefPrV;  RndPrefH.GetKeyDatPrV(RndPrefPrV);   RndPrefPrV.Sort();
  TFltPrV PrefRndPrV;  PrefRndH.GetKeyDatPrV(PrefRndPrV);   PrefRndPrV.Sort();
  TFltPrV PrefPrefPrV; PrefPrefH.GetKeyDatPrV(PrefPrefPrV); PrefPrefPrV.Sort();
  { TGnuPlot GP(TStr::Fmt("probPdf.%s", FNmPref.CStr()), TStr::Fmt("%s. %d/%d edges. CDF. Uniform vs. Preferential triangle closure.", Desc.CStr(), TriadEdges,Graph->GetEdges()));
  GP.AddPlot(RndRndPrV, gpwLinesPoints, "Random--Random");
  GP.AddPlot(RndPrefPrV, gpwLinesPoints, "Random--Preferential");
  GP.AddPlot(PrefRndPrV, gpwLinesPoints, "Preferential--Random");
  GP.AddPlot(PrefPrefPrV, gpwLinesPoints, "Preferential--Preferential");
  GP.SetXYLabel("Probability", "Count");
  GP.SavePng(); }
  { TGnuPlot GP(TStr::Fmt("probCdf.%s", FNmPref.CStr()), TStr::Fmt("%s. %d/%d edges. CDF. Uniform vs. Preferential triangle closure.", Desc.CStr(), TriadEdges,Graph->GetEdges()));
  GP.AddPlot(TGUtil::GetCdf(RndRndPrV), gpwLinesPoints, "Random--Random");
  GP.AddPlot(TGUtil::GetCdf(RndPrefPrV), gpwLinesPoints, "Random--Preferential");
  GP.AddPlot(TGUtil::GetCdf(PrefRndPrV), gpwLinesPoints, "Preferential--Random");
  GP.AddPlot(TGUtil::GetCdf(PrefPrefPrV), gpwLinesPoints, "Preferential--Preferential");
  GP.SetXYLabel("Cumulative probability", "Count");
  GP.SavePng(); }
}*/

PTimeNENet TTimeNENet::GetGnmRndNet(const int& Nodes, const int& Edges) {
  printf("Generating G_nm(%d, %d)\n", Nodes, Edges);
  int Src, Dst;
  PTimeNENet Net = TTimeNENet::New();
  Net->Reserve(Nodes, Edges);
  for (int e = 0; e < Edges; e++) {
    Src = TInt::Rnd.GetUniDevInt(Nodes);
    Dst = TInt::Rnd.GetUniDevInt(Nodes);
    while (Dst == Src || Net->IsEdge(Src, Dst)) {
      Dst = TInt::Rnd.GetUniDevInt(Nodes); }
    if (! Net->IsNode(Src)) { Net->AddNode(Src, TSecTm(e)); }
    if (! Net->IsNode(Dst)) { Net->AddNode(Dst, TSecTm(e)); }
    Net->AddEdge(Src, Dst, -1, TSecTm(e));
  }
  return Net;
}

// ACL, model B: Aiello, Chung, Lu: Random evolution in massive graphs
PTimeNENet TTimeNENet::GetPrefAttach(const int& Nodes, const int& Edges, const double& GammaIn, const double& GammaOut) {
  const double Alpha = Nodes/double(Edges);
  printf("Generating PA(%d, %d), with slope in:%.1f, out: %.1f\n", Nodes, Edges,
    2+GammaIn/(Alpha/(1-Alpha)), 2+GammaOut/(Alpha/(1-Alpha)));
  // init
  int nodes=0, edges=0, time=0, iter=0;
  TIntV OutW(Edges, 0), InW(Edges, 0);
  PTimeNENet Net = TTimeNENet::New();
  Net->Reserve(Nodes, Edges);
  // 1st node
  Net->AddNode(0, TSecTm(time++));  nodes++;
  OutW.Add(0);  InW.Add(0);
  while (edges < Edges) {
    int Src=-1, Dst=-1;  iter++;
    if (TInt::Rnd.GetUniDev() < Alpha) {
      if (nodes < Nodes) {
        IAssert(Net->AddNode(nodes, TSecTm(time++)));
        nodes++; }
    } else {
      if (TInt::Rnd.GetUniDev() < nodes*GammaIn/double(edges+nodes*GammaIn)) {
        Src = TInt::Rnd.GetUniDevInt(nodes); }
      else { Src = OutW[TInt::Rnd.GetUniDevInt(OutW.Len())]; }
      if (TInt::Rnd.GetUniDev() < nodes*GammaOut/double(edges+nodes*GammaOut)) {
        Dst = TInt::Rnd.GetUniDevInt(nodes); }
      else { Dst = InW[TInt::Rnd.GetUniDevInt(InW.Len())]; }
    }
    if (Src == Dst || Net->IsEdge(Src, Dst)) {
      continue;
    }
    //printf("%d/%d %d %d\n", edges, iter, Src, Dst);
    if (! Net->IsNode(Src)) { Net->AddNode(Src, TSecTm(time++)); nodes++; }
    if (! Net->IsNode(Dst)) { Net->AddNode(Dst, TSecTm(time++)); nodes++; }
    Net->AddEdge(Src, Dst, -1, TSecTm(time++));
    OutW.Add(Src); InW.Add(Dst); edges++;
  }
  for (int node = 0; node < Nodes; node++) {
    if (! Net->IsNode(node)) {
      Net->AddNode(node, TSecTm(time++)); }
  }
  return Net;
}

PTimeNENet TTimeNENet::GetPrefAttach(const int& Nodes, const int& OutDeg) {
  printf("Generating PA, nodes:%d, out-deg:%d\n", Nodes, OutDeg);
  // init
  int time=0;
  PTimeNENet Net = TTimeNENet::New();
  Net->Reserve(Nodes, OutDeg*Nodes);
  Net->AddNode(0, TSecTm(++time));  Net->AddNode(1, TSecTm(++time));
  Net->AddEdge(0, 1, -1, TSecTm(++time));
  TIntV NIdV;  NIdV.Add(0);  NIdV.Add(1);
  TIntSet NodeSet;
  for (int node = 2; node <= Nodes; node++) {
    NodeSet.Clr(false);
    while (NodeSet.Len() < OutDeg && NodeSet.Len() < node) {
      NodeSet.AddKey(NIdV[TInt::Rnd.GetUniDevInt(NIdV.Len())]);
    }
    const int N = Net->AddNode(node, TSecTm(++time));
    for (int i = 0; i < NodeSet.Len(); i++) {
      Net->AddEdge(node, NodeSet[i], -1, TSecTm(++time));
      NIdV.Add(N);  NIdV.Add(NodeSet[i]);
    }
  }
  return Net;
}

void TTimeNENet::SaveEdgeTm(const TStr& EdgeFNm, const bool& RenumberNId, const bool& RelativeTm) const {
  TIntV EIdV;  GetEIdByTm(EIdV);
  const int BegTm = RelativeTm ? GetEDat(EIdV[0]).GetAbsSecs() : 0;
  TIntSet NIdMap;
  if (RenumberNId) { NIdMap.Gen(GetNodes()); }
  FILE *F = fopen(EdgeFNm.CStr(), "wt");
  //fprintf(F, "#Nodes\t%d\n#Edges\t%d\n", GetNodes(), GetEdges());
  //fprintf(F, "#<src>\t<dst>\t<time>\n");
  for (int e =0; e < EIdV.Len(); e++) {
    const TEdgeI EI = GetEI(EIdV[e]);
    if (RenumberNId) {
      const int src = EI.GetSrcNId();
      const int dst = EI.GetDstNId();
      NIdMap.AddKey(src);  NIdMap.AddKey(dst);
      fprintf(F, "%d\t%d\t%d\n", NIdMap.GetKeyId(src), NIdMap.GetKeyId(dst), EI().GetAbsSecs()-BegTm);
    }else {
      fprintf(F, "%d\t%d\t%d\n", EI.GetSrcNId(), EI.GetDstNId(), EI().GetAbsSecs()-BegTm); }
  }
  fclose(F);
}

PTimeNENet TTimeNENet::GetSmallNet() {
  PTimeNENet Net = TTimeNENet::New();
  for (int i = 1; i <= 6; i++) {
    Net->AddNode(i, TSecTm(0)); }
  int tm = 1;
  Net->AddEdge(1, 2, -1, TSecTm(tm++));
  Net->AddEdge(3, 4, -1, TSecTm(tm++));
  Net->AddEdge(3, 1, -1, TSecTm(tm++));
  Net->AddEdge(5, 6, -1, TSecTm(tm++));
  Net->AddEdge(6, 4, -1, TSecTm(tm++));
  Net->AddEdge(5, 3, -1, TSecTm(tm++));
  Net->AddEdge(5, 4, -1, TSecTm(tm++));
  Net->AddEdge(5, 2, -1, TSecTm(tm++));
  return Net;
}

PTimeNENet TTimeNENet::LoadFlickr(const TStr& NodeFNm, const TStr& EdgeFNm) {
  const int BegOfTm = 1047369600; // Tue Mar 11 01:00:00 2003 (begining of Flickr)
  PTimeNENet Net = TTimeNENet::New();
  printf("Adding nodes...");
  { TSsParser Ss(NodeFNm, ssfWhiteSep);
  while (Ss.Next()) {
    const int NId = Ss.GetInt(0);
    const int Tm = Ss.GetInt(1)+BegOfTm;
    if (TSecTm(Tm) < TSecTm(2002, 1, 1)) {
      printf("  skip node %g (time %d)\n", (double) Ss.GetLineNo(), Ss.GetInt(1)); continue; }
    Net->AddNode(NId, TSecTm(Tm));
  } }
  printf(" %d nodes\n", Net->GetNodes());
  printf("Adding edges...");
  int SkipCnt=0;
  //TIntH SkipDiffCnt;
  { TSsParser Ss(EdgeFNm, ssfWhiteSep);
  while (Ss.Next()) {
    const int NId1 = Ss.GetInt(0);
    const int NId2 = Ss.GetInt(1);
    const TSecTm Tm = TSecTm(Ss.GetInt(2)+BegOfTm);
    if (! Net->IsNode(NId1) || ! Net->IsNode(NId2)) { printf("not node\n"); continue; }
    if (Tm < TSecTm(2002, 1, 1)) { SkipCnt++;
      printf("  skip edge %g (time %s)\n", (double) Ss.GetLineNo(), Tm.GetStr().CStr()); continue; }
    if (Tm+600 < Net->GetNDat(NId1)) {
      printf("  1:skip edge %g (time %s < %s)\n", (double) Ss.GetLineNo(), Tm.GetStr().CStr(), Net->GetNDat(NId1).GetStr().CStr());
      //SkipDiffCnt.AddDat(-Tm.GetAbsSecs()+Net->GetNDat(NId1).GetAbsSecs()) += 1;
      SkipCnt++;  continue; }
    if (Tm+600 < Net->GetNDat(NId2)) { SkipCnt++;
      printf("  2:skip edge %g (time %s < %s)\n", (double) Ss.GetLineNo(), Tm.GetStr().CStr(), Net->GetNDat(NId2).GetStr().CStr());
      //SkipDiffCnt.AddDat(-Tm.GetAbsSecs()+Net->GetNDat(NId2).GetAbsSecs()) += 1;
      SkipCnt++;  continue; }
    Net->AddEdge(NId1, NId2, -1, TSecTm(Tm));
  } }
  //TGnuPlot::PlotValCntH(SkipDiffCnt, "flickr-edgeNodeDiff", "", "seconds", "count");
  printf("  %d edges\n", Net->GetEdges());
  printf("  %d edges skipped (edge time < node time)\n", SkipCnt);
  Net->UpdateNodeTimes();
  return Net;
}

// load network where fields are separated by Separator and each line contains (*Fld are column indexes)
// .. <source> .. <destination> .. <time>
PTimeNENet TTimeNENet::LoadEdgeTm(const TStr& EdgeFNm, const int& SrcFld, const int& DstFld, const int& TimeFld, const TSsFmt& Separator) {
  printf("Loading %s\n", EdgeFNm.CStr());
  PTimeNENet Net = TTimeNENet::New();
  TStrHash<TInt> StrToId(Mega(1), true); // node id to string
  int LineCnt=0;
  TExeTm ExeTm;
  TSsParser Ss(EdgeFNm, Separator);
  TSecTm MinTm=TSecTm::GetCurTm(), MaxTm=TSecTm(100);
  while (Ss.Next()) {
    if (Ss.IsCmt()) { continue; }
    IAssert(Ss.Len() > TimeFld);
    const char* Node1 = Ss.GetFld(SrcFld);
    const char* Node2 = Ss.GetFld(DstFld);
    const char* TmStr = Ss.GetFld(TimeFld);
    if (strcmp(TmStr,"NULL")==0) { continue; }
    //const TSecTm Tm = TSecTm::GetDtTmFromYmdHmsStr(TmStr);
    const TSecTm Tm(atoi(TmStr));
    const int NId1 = StrToId.AddKey(Node1);
    const int NId2 = StrToId.AddKey(Node2);
    if (! Net->IsNode(NId1)) { Net->AddNode(NId1, TSecTm()); }
    if (! Net->IsNode(NId2)) { Net->AddNode(NId2, TSecTm()); }
    MinTm=TMath::Mn(MinTm, Tm);
    MaxTm=TMath::Mx(MaxTm, Tm);
    Net->AddEdge(NId1, NId2, -1, Tm);
    if (++LineCnt % 1000 == 0) {
      printf("\r  %dk lines processed: %d %d [%s]", LineCnt/1000, Net->GetNodes(), Net->GetEdges(), ExeTm.GetStr()); }
  }
  printf("\r  %d lines processed: %d %d [%s]\n", LineCnt, Net->GetNodes(), Net->GetEdges(), ExeTm.GetStr());
  printf("  Data range %s -- %s\n", MinTm.GetStr().CStr(), MaxTm.GetStr().CStr());
  //TSnap::PrintInfo(Net, "", "", false);
  Net->UpdateNodeTimes();
  return Net;
}
