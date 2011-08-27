/////////////////////////////////////////////////
// Sigle Snapshot Graph Statistics
int TGStat::NDiamRuns = 10;
int TGStat::TakeSngVals = 100;
const TFltPrV TGStat::EmptyV = TFltPrV();

bool TGStat::TCmpByVal::operator () (const TGStat& GS1, const TGStat& GS2) const {
  IAssertR(GS1.HasVal(ValCmp) && GS2.HasVal(ValCmp), TStr::Fmt("CmpVal: %d (%s)", 
    int(ValCmp), TGStat::GetValStr(ValCmp).CStr()).CStr());
  bool Res;
  if (ValCmp == gsvTime) { Res = GS1.Time < GS2.Time; }
  else { Res = GS1.GetVal(ValCmp) < GS2.GetVal(ValCmp); }
  if (SortAsc) { return Res; }
  else { return ! Res; }
}

bool TGStat::TCmpByVal::operator () (const PGStat& GS1, const PGStat& GS2) const {
  return operator()(*GS1, *GS2);
}

TGStat::TGStat(const TSecTm& GraphTm, const TStr& GraphName) :
  Time(GraphTm), GraphNm(GraphName), ValStatH(), DistrStatH() {
}

TGStat::TGStat(const PNGraph& Graph, const TSecTm& GraphTm, TFSet StatFSet, const TStr& GraphName) {
  TakeStat(Graph, GraphTm, StatFSet, GraphName);
}

TGStat::TGStat(const PNEGraph& Graph, const TSecTm& GraphTm, TFSet StatFSet, const TStr& GraphName) {
  TakeStat(Graph, GraphTm, StatFSet, GraphName);
}
TGStat::TGStat(const TGStat& GStat) : Time(GStat.Time), GraphNm(GStat.GraphNm),
  ValStatH(GStat.ValStatH), DistrStatH(GStat.DistrStatH) {
}

TGStat::TGStat(TSIn& SIn) : Time(SIn), GraphNm(SIn), ValStatH(SIn), DistrStatH(SIn) { }

void TGStat::Save(TSOut& SOut) const {
  Time.Save(SOut);  GraphNm.Save(SOut);
  ValStatH.Save(SOut);  DistrStatH.Save(SOut);
}

TGStat& TGStat::operator = (const TGStat& GStat) {
  if (this != &GStat) {
    Time = GStat.Time;
    GraphNm = GStat.GraphNm;
    ValStatH = GStat.ValStatH;
    DistrStatH = GStat.DistrStatH;
  }
  return *this;
}

bool TGStat::operator == (const TGStat& GStat) const {
  return Time==GStat.Time && ValStatH==GStat.ValStatH && DistrStatH==GStat.DistrStatH;
}

bool TGStat::operator < (const TGStat& GStat) const {
  if (Time<GStat.Time) { return true; }
  if (Time>GStat.Time) { return false; }
  if (ValStatH.Empty() && ! GStat.ValStatH.Empty()) { return true; }
  if (GStat.ValStatH.Empty()) { return false; }
  for (int v = gsvTime; v < gsvMx; v++) {
    if (! ValStatH.IsKey(v) && ! GStat.ValStatH.IsKey(v)) { continue; }
    if (ValStatH.IsKey(v) && ! GStat.ValStatH.IsKey(v)) { return false; }
    if (! ValStatH.IsKey(v)) { return true; }
    if (ValStatH.GetDat(v) < GStat.ValStatH.GetDat(v)) { return true; }
  }
  return false;
}

bool TGStat::HasVal(const TGStatVal& StatVal) const {
  if (StatVal == gsvTime) { return Time.IsDef(); }
  if (StatVal == gsvWccSize) { return HasVal(gsvWccNodes) && HasVal(gsvNodes); }
  return ValStatH.IsKey(int(StatVal));
}

double TGStat::GetVal(const TGStatVal& StatVal) const {
  if (StatVal == gsvTime) { return Time.GetAbsSecs(); }
  if (StatVal == gsvWccSize) { return GetVal(gsvWccNodes) / GetVal(gsvNodes); }
  if (! ValStatH.IsKey(int(StatVal))) { return -1.0; }
  return ValStatH.GetDat(int(StatVal));
}

void TGStat::SetVal(const TGStatVal& StatVal, const double& Val) {
  ValStatH.AddDat(int(StatVal), Val);
}

const TFltPrV& TGStat::GetDistr(const TGStatDistr& Distr) const {
  if (! DistrStatH.IsKey(int(Distr))) { return EmptyV; }
  return DistrStatH.GetDat(int(Distr));
}

void TGStat::SetDistr(const TGStatDistr& Distr, const TFltPrV& FltPrV) {
  DistrStatH.AddDat(Distr, FltPrV);
}

void TGStat::GetDistr(const TGStatDistr& Distr, TFltPrV& FltPrV) const {
  FltPrV = GetDistr(Distr);
}

void TGStat::TakeStat(const PNGraph& Graph, const TSecTm& _Time, TFSet StatFSet, const TStr& GraphName) {
  printf("\n===TakeStat:  G(%u, %u)\n", Graph->GetNodes(), Graph->GetEdges());
  TExeTm ExeTm, FullTm;
  Time = _Time;
  GraphNm = GraphName;
  if (StatFSet.In(gsvNone)) { return; }
  TakeBasicStat(Graph, false);
  TakeDiam(Graph, StatFSet, false);
  if (StatFSet.In(gsdWcc) || StatFSet.In(gsdWccHops) || StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffWccDiam)) {
    PNGraph WccGraph = TSnap::GetMxWcc(Graph);
    TakeBasicStat(WccGraph, true);
    TakeDiam(WccGraph, StatFSet, true);
  }
  // degrees
  TakeDegDistr(Graph, StatFSet);
  // components
  TakeConnComp(Graph, StatFSet);
  // spectral
  TakeSpectral(Graph, StatFSet, -1);
  // clustering coeffient
  if (StatFSet.In(gsdClustCf) || StatFSet.In(gsvClustCf)) {
    TakeClustCf(Graph); }
  if (StatFSet.In(gsdTriadPart)) {
    TakeTriadPart(Graph); }
  printf("  [%s]\n", FullTm.GetTmStr());
}

void TGStat::TakeSpectral(const PNGraph& Graph, const int _TakeSngVals) {
  TakeSpectral(Graph, TFSet() | gsdSngVal | gsdSngVec, _TakeSngVals);
}

void TGStat::TakeSpectral(const PNGraph& Graph, TFSet StatFSet, int _TakeSngVals) {
  if (_TakeSngVals == -1) { _TakeSngVals = TakeSngVals; }
  // singular values, vectors
  if (StatFSet.In(gsdSngVal)) {
    const int SngVals = TMath::Mn(_TakeSngVals, Graph->GetNodes()/2);
    TFltV SngValV1;
    TSnap::GetSngVals(Graph, SngVals, SngValV1);
    SngValV1.Sort(false);
    TFltPrV& SngValV = DistrStatH.AddDat(gsdSngVal);
    SngValV.Gen(SngValV1.Len(), 0);
    for (int i = 0; i < SngValV1.Len(); i++) {
      SngValV.Add(TFltPr(i+1, SngValV1[i]));
    }
  }
  if (StatFSet.In(gsdSngVec)) {
    TFltV LeftV, RightV;
    TSnap::GetSngVec(Graph, LeftV, RightV);
    LeftV.Sort(false);
    TFltPrV& SngVec = DistrStatH.AddDat(gsdSngVec);
    SngVec.Gen(LeftV.Len(), 0);
    for (int i = 0; i < TMath::Mn(Kilo(10), LeftV.Len()/2); i++) {
      if (LeftV[i] > 0) { SngVec.Add(TFltPr(i+1, LeftV[i])); }
    }
  }
}

void TGStat::Plot(const TGStatDistr& Distr, const TStr& FNmPref, TStr Desc, bool PowerFit) const {
  if (Desc.Empty()) Desc = FNmPref.GetUc();
  if (! HasDistr(Distr) || Distr==gsdUndef || Distr==gsdMx) { return; }
  TPlotInfo Info = GetPlotInfo(Distr);
  TGnuPlot GnuPlot(Info.Val1+TStr(".")+FNmPref, TStr::Fmt("%s. G(%d, %d)", Desc.CStr(), GetNodes(),GetEdges()));
  GnuPlot.SetXYLabel(Info.Val2, Info.Val3);
  GnuPlot.SetScale(Info.Val4);
  const int plotId = GnuPlot.AddPlot(GetDistr(Distr), gpwLinesPoints, "");
  if (PowerFit) { GnuPlot.AddPwrFit(plotId, gpwLines); }
  GnuPlot.SavePng();
}

void TGStat::Plot(const TFSet& FSet, const TStr& FNmPref, TStr Desc, bool PowerFit) const {
  for (int d = gsdUndef; d < gsdMx; d++) {
    const TGStatDistr Distr = TGStatDistr(d);
    if (! FSet.In(Distr)) { continue; }
    Plot(Distr, FNmPref, Desc, PowerFit);
  }
}

void TGStat::PlotAll(const TStr& FNmPref, TStr Desc, bool PowerFit) const {
  for (int d = gsdUndef; d < gsdMx; d++) {
    const TGStatDistr Distr = TGStatDistr(d);
    Plot(Distr, FNmPref, Desc, PowerFit);
  }
}

void TGStat::DumpValStat() {
  for (int val = gsvNone; val < gsvMx; val++) {
    const TGStatVal Val = TGStatVal(val);
    if (! HasVal(Val)) { continue; }
    printf("  %s\t%g\n", GetValStr(Val).CStr(), GetVal(Val));
  }
}

void TGStat::AvgGStat(const PGStatVec& GStatVec, const bool& ClipAt1) {
  AvgGStat(GStatVec->GetGStatV(), ClipAt1);
}

void TGStat::AvgGStat(const TGStatV& GStatV, const bool& ClipAt1) {
  if (GStatV.Empty()) return;
  Time = GStatV[0]->Time;
  GraphNm = GStatV[0]->GraphNm;
  // values
  for (int statVal = 0; statVal > gsvMx; statVal++) {
    const TGStatVal GStatVal = TGStatVal(statVal);
    TMom Mom;
    for (int i = 0; i < GStatV.Len(); i++) {
      if (GStatV[i]->HasVal(GStatVal)) {
        Mom.Add(GStatV[i]->GetVal(GStatVal)); }
    }
    Mom.Def();
    if (Mom.IsUsable()) {
      IAssert(Mom.GetVals() == GStatV.Len()); // all must have the value
      SetVal(GStatVal, Mom.GetMean());
    }
  }
  // distributions
  for (int distr = gsdUndef; distr < gsdMx; distr++) {
    const TGStatDistr GStatDistr = TGStatDistr(distr);
    THash<TFlt, TFlt> ValToSumH;
    int DistrCnt = 0;
    for (int i = 0; i < GStatV.Len(); i++) {
      if (GStatV[i]->HasDistr(GStatDistr)) {
        const TFltPrV& D = GStatV[i]->GetDistr(GStatDistr);
        for (int d = 0; d < D.Len(); d++) {
          ValToSumH.AddDat(D[d].Val1) += D[d].Val2; }
        DistrCnt++;
      }
    }
    IAssert(DistrCnt==0 || DistrCnt==GStatV.Len()); // all must have distribution
    TFltPrV AvgStatV;
    ValToSumH.GetKeyDatPrV(AvgStatV);  AvgStatV.Sort();
    for (int i = 0; i < AvgStatV.Len(); i++) {
      AvgStatV[i].Val2 /= double(DistrCnt);
      if (ClipAt1 && AvgStatV[i].Val2 < 1) { AvgStatV[i].Val2 = 1; }
    }
    SetDistr(GStatDistr, AvgStatV);
  }
}

TStr TGStat::GetDistrStr(const TGStatDistr& Distr) {
  switch (Distr) {
    case gsdUndef : return TStr("Undef");
    case gsdInDeg : return "InDeg";
    case gsdOutDeg : return "OutDeg";
    case gsdWcc : return "WccDist";
    case gsdScc : return "SccDist";
    case gsdHops : return "Hops";
    case gsdWccHops : return "WccHops";
    case gsdSngVal : return "SngVal";
    case gsdSngVec : return "SngVec";
    case gsdClustCf : return "ClustCf";
    case gsdTriadPart : return "TriadPart";
    case gsdMx: return TStr("Mx");
    default: Fail; return TStr();
  };
}

TStr TGStat::GetValStr(const TGStatVal& Val) {
  static TIntStrH ValTyStrH;
  if (ValTyStrH.Empty()) {
    ValTyStrH.AddDat(gsvNone, "None");
    ValTyStrH.AddDat(gsvTime, "Time");
    ValTyStrH.AddDat(gsvNodes, "Nodes");
    ValTyStrH.AddDat(gsvZeroNodes, "ZeroNodes");
    ValTyStrH.AddDat(gsvNonZNodes, "NonZNodes");
    ValTyStrH.AddDat(gsvSrcNodes, "SrcNodes");
    ValTyStrH.AddDat(gsvDstNodes, "DstNodes");
    ValTyStrH.AddDat(gsvEdges, "Edges");
    ValTyStrH.AddDat(gsvUniqEdges, "UniqEdges");
    ValTyStrH.AddDat(gsvBiDirEdges, "BiDirEdges");
    ValTyStrH.AddDat(gsvWccNodes, "WccNodes");
    ValTyStrH.AddDat(gsvWccSrcNodes, "WccSrcNodes");
    ValTyStrH.AddDat(gsvWccDstNodes, "WccDstNodes");
    ValTyStrH.AddDat(gsvWccEdges, "WccEdges");
    ValTyStrH.AddDat(gsvWccUniqEdges, "WccUniqEdges");
    ValTyStrH.AddDat(gsvWccBiDirEdges, "WccBiDirEdges");
    ValTyStrH.AddDat(gsvFullDiam, "FullDiam");
    ValTyStrH.AddDat(gsvEffDiam, "EffDiam");
    ValTyStrH.AddDat(gsvEffWccDiam, "EffWccDiam");
    ValTyStrH.AddDat(gsvFullWccDiam, "FullWccDiam");
    ValTyStrH.AddDat(gsvFullDiamDev, "FullDiamDev");
    ValTyStrH.AddDat(gsvEffDiamDev, "EffDiamDev");
    ValTyStrH.AddDat(gsvEffWccDiamDev, "EffWccDiamDev");
    ValTyStrH.AddDat(gsvFullWccDiamDev, "FullWccDiamDev");
    ValTyStrH.AddDat(gsvClustCf, "ClustCf");
    ValTyStrH.AddDat(gsvOpenTriads, "OpenTr");
    ValTyStrH.AddDat(gsvClosedTriads, "ClosedTr");
    ValTyStrH.AddDat(gsvWccSize, "WccSize");
    ValTyStrH.AddDat(gsvMx, "Mx");
  }
  IAssert(ValTyStrH.IsKey(int(Val)));
  return ValTyStrH.GetDat(int(Val));
}

TGStat::TPlotInfo TGStat::GetPlotInfo(const TGStatVal& Val) {
  //switch (Distr) {
    //case gsdUndef : Fail; return TPlotInfo();
  Fail;
  return TPlotInfo();
}

TGStat::TPlotInfo TGStat::GetPlotInfo(const TGStatDistr& Distr) {
  switch (Distr) {
    case gsdUndef : Fail; return TPlotInfo();
    case gsdInDeg : return TPlotInfo("inDeg", "In-degree, k", "Count", gpsLog10XY);
    case gsdOutDeg : return TPlotInfo("outDeg", "Out-degree, k", "Count", gpsLog10XY);
    case gsdWcc : return TPlotInfo("wcc", "WCC size", "Count", gpsLog10XY);
    case gsdScc : return TPlotInfo("scc", "SCC size", "Count", gpsLog10XY);
    case gsdHops : return TPlotInfo("hop", "Number of hops, h", "Reachable pairs of nodes inside h hops", gpsLog10Y);
    case gsdWccHops : return TPlotInfo("wccHop", "Number of hops, h", "Reachable pairs of nodes inside h hops in WCC", gpsLog10Y);
    case gsdSngVal : return TPlotInfo("sval", "Rank", "Singular value", gpsLog10XY);
    case gsdSngVec : return TPlotInfo("svec", "Rank", "Left singular vector", gpsLog10XY);
    case gsdClustCf : return TPlotInfo("ccf", "Degree, k", "Clustering coefficient, <C(k)>", gpsLog10XY);
    case gsdTriadPart : return TPlotInfo("triad", "Number of triads adjacent to a node", "Number of such nodes", gpsLog10XY);
    case gsdMx : Fail;
    default: Fail; return TPlotInfo();
  };
}

TFSet TGStat::NoStat() {
  return TFSet() | gsvNone;
}

TFSet TGStat::BasicStat() {
  return TFSet();
}

TFSet TGStat::DegDStat() {
  return TFSet() | gsdInDeg |  gsdOutDeg;
}

TFSet TGStat::NoDiamStat() {
  return TFSet() | gsdInDeg |  gsdOutDeg |  gsdWcc |  gsdScc;
}

TFSet TGStat::NoDistrStat() {
  return TFSet() | gsdHops | gsdWccHops;
}

TFSet TGStat::NoSvdStat() {
  return TFSet() | gsdInDeg |  gsdOutDeg |  gsdWcc |  gsdScc |
    gsdHops |  gsdWccHops | gsdClustCf | gsdTriadPart;
}

TFSet TGStat::AllStat() {
  return TFSet() | gsdInDeg |  gsdOutDeg |  gsdWcc |  gsdScc
    | gsdHops |  gsdWccHops | gsdClustCf | gsdTriadPart 
    | gsdSngVec | gsdSngVal | gsvFullDiam;
}

/////////////////////////////////////////////////
// Graph Growth Statistics
uint TGStatVec::MinNodesEdges = 10;

TGStatVec::TGStatVec(const TTmUnit& _TmUnit) : TmUnit(_TmUnit), StatFSet(), GStatV() {
  StatFSet = TGStat::AllStat();
}

TGStatVec::TGStatVec(const TTmUnit& _TmUnit, const TFSet& TakeGrowthStat) :
   TmUnit(_TmUnit), StatFSet(TakeGrowthStat), GStatV() {
}

TGStatVec::TGStatVec(const TGStatVec& GStat) :
  TmUnit(GStat.TmUnit), StatFSet(GStat.StatFSet), GStatV(GStat.GStatV) {
}

TGStatVec::TGStatVec(TSIn& SIn) : TmUnit((TTmUnit) TInt(SIn).Val), StatFSet(SIn), GStatV(SIn) {
}

PGStatVec TGStatVec::New(const TTmUnit& _TmUnit) {
  return new TGStatVec(_TmUnit);
}

PGStatVec TGStatVec::New(const TTmUnit& _TmUnit, const TFSet& TakeGrowthStat) {
  return new TGStatVec(_TmUnit, TakeGrowthStat);
}

void TGStatVec::Save(TSOut& SOut) const {
  TInt(TmUnit).Save(SOut);
  StatFSet.Save(SOut);
  GStatV.Save(SOut);
}

TGStatVec& TGStatVec::operator = (const TGStatVec& GStat) {
  if (this != &GStat) {
    TmUnit = GStat.TmUnit;
    StatFSet = GStat.StatFSet;
    GStatV = GStat.GStatV;
  }
  return *this;
}

PGStat TGStatVec::Add() {
  GStatV.Add(TGStat::New());
  return GStatV.Last();
}

PGStat TGStatVec::Add(const TSecTm& Time, TStr GraphNm) {
  GStatV.Add(TGStat::New(Time, GraphNm));
  return GStatV.Last();
}

void TGStatVec::Add(const PNGraph& Graph, const TSecTm& Time, const TStr& GraphNm) {
  if (Graph->GetNodes() < (int) TGStatVec::MinNodesEdges) {
    printf(" ** TGStatVec::Add: graph too small (%d nodes).SKIP\n", Graph->GetNodes());
    return;
  }
  Add(TGStat::New(Graph, Time, StatFSet, GraphNm));
}

void TGStatVec::Add(const PNEGraph& Graph, const TSecTm& Time, const TStr& GraphNm) {
  if (Graph->GetNodes() < (int) TGStatVec::MinNodesEdges) {
    printf(" ** TGStatVec::Add: graph too small (%d nodes).SKIP\n", Graph->GetNodes());
    return;
  }
  Add(TGStat::New(Graph, Time, StatFSet, GraphNm));
}

void TGStatVec::Sort(const TGStatVal& SortBy, const bool& Asc) {
  GStatV.SortCmp(TGStat::TCmpByVal(SortBy, Asc));
}

void TGStatVec::DelBefore(const TSecTm& Tm) {
  TGStatV NewTickV;
  for (int i = 0; i < Len(); i++) {
    if (At(i)->Time >= Tm) { NewTickV.Add(At(i)); }
  }
  GStatV.Swap(NewTickV);
}

void TGStatVec::DelAfter(const TSecTm& Tm) {
  TGStatV NewTickV;
  for (int i = 0; i < Len(); i++) {
    if (At(i)->Time <= Tm) { NewTickV.Add(At(i)); }
  }
  GStatV.Swap(NewTickV);
}

void TGStatVec::DelSmallNodes(const int& MinNodes) {
  TGStatV NewTickV;
  for (int i = 0; i < Len(); i++) {
    if (At(i)->GetNodes() >= MinNodes) { NewTickV.Add(At(i)); }
  }
  GStatV.Swap(NewTickV);
}

void TGStatVec::GetValV(const TGStatVal& XVal, const TGStatVal& YVal, TFltPrV& ValV) const {
  ValV.Gen(Len(), 0);
  double x;
  for (int t = 0; t < Len(); t++) {
    if (XVal == gsvTime) { x = t+1; }
    else { x = At(t)->GetVal(XVal); }
    ValV.Add(TFltPr(x, At(t)->GetVal(YVal)));
  }
}

PGStat TGStatVec::GetAvgGStat(const bool& ClipAt1) {
  PGStat Stat = TGStat::New();
  Stat->AvgGStat(GStatV, ClipAt1);
  return Stat;
}

void TGStatVec::Plot(const TGStatVal& XVal, const TGStatVal& YVal, const TStr& OutFNm, TStr& Desc, const TGpScaleTy& Scale,const bool& PowerFit) const {
  if (! Last()->HasVal(XVal) || ! Last()->HasVal(YVal)) {
    if (! Last()->HasVal(XVal)) { printf("** Does not have %s statistic\n", TGStat::GetValStr(XVal).CStr()); }
    if (! Last()->HasVal(YVal)) { printf("** Does not have %s statistic\n", TGStat::GetValStr(YVal).CStr()); }
    return;
  }
  if (Desc.Empty()) { Desc = OutFNm; }
  TFltPrV ValV;
  TGStatVec::GetValV(XVal, YVal, ValV);
  TGnuPlot GP(TStr::Fmt("%s-%s.%s", TGStat::GetValStr(XVal).CStr(), TGStat::GetValStr(YVal).CStr(), OutFNm.CStr()),
    TStr::Fmt("%s. %s vs. %s. G(%d,%d)", Desc.CStr(), TGStat::GetValStr(XVal).CStr(), TGStat::GetValStr(YVal).CStr(),
    Last()->GetNodes(), Last()->GetEdges()));
  GP.SetScale(Scale);
  GP.SetXYLabel(TGStat::GetValStr(XVal), TGStat::GetValStr(YVal));
  const int Id = GP.AddPlot(ValV, gpwLinesPoints);
  if (PowerFit) { GP.AddPwrFit(Id); }
  GP.SavePng();
}

void TGStatVec::PlotAllVsX(const TGStatVal& XVal, const TStr& OutFNm, TStr Desc, const TGpScaleTy& Scale, const bool& PowerFit) const {
  const TFSet SkipStat = TFSet() | gsvFullDiamDev | gsvEffDiamDev | gsvEffWccDiamDev | gsvFullWccDiamDev;
  for (int stat = gsvNone; stat < gsvMx; stat++) {
    const TGStatVal Stat = TGStatVal(stat);
    if (SkipStat.In(Stat)) { continue; }
    if (Last()->HasVal(Stat) && Last()->HasVal(XVal) && Stat!=XVal) {
      Plot(XVal, Stat, OutFNm, Desc, Scale, PowerFit);
    }
  }
}

void TGStatVec::ImposeDistr(const TGStatDistr& Distr, const TStr& FNmPref, TStr Desc, const bool& ExpBin, 
    const bool& PowerFit, const TGpSeriesTy& PlotWith, const TStr& Style) const {
  if (Desc.Empty()) Desc = FNmPref.GetUc();
  if (! At(0)->HasDistr(Distr) || Distr==gsdUndef || Distr==gsdMx) { return; }
  TGStat::TPlotInfo Info = At(0)->GetPlotInfo(Distr);
  TGnuPlot GnuPlot(Info.Val1+TStr(".")+FNmPref, TStr::Fmt("%s. G(%d, %d) --> G(%d, %d)", Desc.CStr(),
    At(0)->GetNodes(), At(0)->GetEdges(), Last()->GetNodes(), Last()->GetEdges()));
  GnuPlot.SetXYLabel(Info.Val2, Info.Val3);
  GnuPlot.SetScale(Info.Val4);
  int plotId;
  for (int at = 0; at < Len(); at++) {
    TStr Legend = At(at)->GetNm();
    if (Legend.Empty()) { Legend = At(at)->GetTmStr(); }
    if (! ExpBin) { 
      plotId = GnuPlot.AddPlot(At(at)->GetDistr(Distr), PlotWith, Legend, Style); }
    else { 
      TFltPrV ExpBinV; 
      TGnuPlot::MakeExpBins(At(at)->GetDistr(Distr), ExpBinV, 2, 0);
      plotId = GnuPlot.AddPlot(ExpBinV, PlotWith, Legend, Style);
    }
    if (PowerFit) { GnuPlot.AddPwrFit(plotId, gpwLines); }
  }
  GnuPlot.SavePng();
}

void TGStatVec::SaveTxt(const TStr& FNmPref, const TStr& Desc) const {
  FILE *F = fopen(TStr::Fmt("growth.%s.tab", FNmPref.CStr()).CStr(), "wt");
  fprintf(F, "# %s\n", Desc.CStr());
  fprintf(F, "# %s", TTmInfo::GetTmUnitStr(TmUnit).CStr());
  TIntSet StatValSet;
  for (int i = 0; i < Len(); i++) {
    for (int v = gsvNone; v < gsvMx; v++) {
      if (At(i)->HasVal(TGStatVal(v))) { StatValSet.AddKey(v); }
    }
  }
  TIntV StatValV;  StatValSet.GetKeyV(StatValV);  StatValV.Sort();
  for (int sv = 0; sv < StatValV.Len(); sv++) {
    fprintf(F, "\t%s", TGStat::GetValStr(TGStatVal(StatValV[sv].Val)).CStr()); }
  fprintf(F, "Time\n");
  for (int i = 0; i < Len(); i++) {
    const TGStat& G = *At(i);
    for (int sv = 0; sv < StatValV.Len(); sv++) {
      fprintf(F, "%g\t", G.GetVal(TGStatVal(StatValV[sv].Val))); }
    fprintf(F, "%s\n", G.GetTmStr().CStr());
  }
  fclose(F);
}
