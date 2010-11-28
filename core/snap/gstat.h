/////////////////////////////////////////////////
// Graph Statistics
// statistics of a single snapshot of a graph
class TGStat;
typedef TPt<TGStat> PGStat;
typedef TVec<PGStat> TGStatV;

// statistics of a sequence of graph snapshots
class TGStatVec;
typedef TPt<TGStatVec> PGStatVec;

/////////////////////////////////////////////////
// Statistics of a Sigle Graph
// Scalar statistics of the graph
typedef enum {
  gsvNone, gsvTime, gsvNodes, gsvZeroNodes, gsvNonZNodes, gsvSrcNodes, gsvDstNodes,
  gsvEdges, gsvUniqEdges, gsvBiDirEdges,
  gsvWccNodes, gsvWccSrcNodes, gsvWccDstNodes, gsvWccEdges, gsvWccUniqEdges, gsvWccBiDirEdges,
  gsvFullDiam, gsvEffDiam, gsvEffWccDiam, gsvFullWccDiam,
  gsvFullDiamDev, gsvEffDiamDev, gsvEffWccDiamDev, gsvFullWccDiamDev, // diameter+variance
  gsvClustCf, gsvOpenTriads, gsvClosedTriads, gsvWccSize,
  gsvMx
} TGStatVal;

// Distribution statistics of the graph
typedef enum {
  gsdUndef=100, gsdInDeg, gsdOutDeg, gsdWcc, gsdScc,
  gsdHops, gsdWccHops, gsdSngVal, gsdSngVec, gsdClustCf,
  gsdTriadPart, // triad participation
  gsdMx,
} TGStatDistr;

/////////////////////////////////////////////////
// Statistics of a Graph Snapshot
class TGStat {
public:
  static int NDiamRuns;
  static int TakeSngVals;
  typedef TQuad<TStr, TStr, TStr, TGpScaleTy> TPlotInfo; // file prefix, x label, y label, scale
public:
  class TCmpByVal {
  private:
    TGStatVal ValCmp;
    bool SortAsc;
  public:
    TCmpByVal(TGStatVal SortBy, bool Asc) : ValCmp(SortBy), SortAsc(Asc) { }
    bool operator () (const TGStat& GS1, const TGStat& GS2) const;
    bool operator () (const PGStat& GS1, const PGStat& GS2) const;
  };
private:
  const static TFltPrV EmptyV;
  TCRef CRef;
public:
  TSecTm Time;
  TStr GraphNm;
  TIntFltH ValStatH; // scalar statistics
  THash<TInt, TFltPrV> DistrStatH; // distribution statistics
public:
  TGStat(const TSecTm& GraphTm = TSecTm(), const TStr& GraphName=TStr());
  TGStat(const PNGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr());
  TGStat(const PNEGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr());
  template <class PGraph> TGStat(const PGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr()) {
    TakeStat(Graph, Time, StatFSet, GraphName); }
  TGStat(const TGStat& GStat);
  TGStat(TSIn& SIn);
  void Save(TSOut& SOut) const;
  static PGStat New(const TSecTm& Time=TSecTm(), const TStr& GraphName=TStr()) {
    return new TGStat(Time, GraphName); }
  static PGStat New(const PNGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  static PGStat New(const PNEGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  template <class PGraph> PGStat New(const PGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  static PGStat Load(TSIn& SIn) { return new TGStat(SIn); }
  PGStat Clone() const { return new TGStat(*this); }
  TGStat& operator = (const TGStat& GStat);
  bool operator == (const TGStat& GStat) const;
  bool operator < (const TGStat& GStat) const;

  int GetYear() const { return Time.GetYearN(); }
  int GetMonth() const { return Time.GetMonthN(); }
  int GetDay() const { return Time.GetDayN(); }
  int GetHour() const { return Time.GetHourN(); }
  int GetMin() const { return Time.GetMinN(); }
  int GetSec() const { return Time.GetSecN(); }
  TStr GetTmStr() const { return Time.GetStr(); }
  void SetTm(const TSecTm& GraphTm) { Time = GraphTm; }
  TStr GetNm() const { return GraphNm; }
  void SetNm(const TStr& GraphName) { GraphNm=GraphName; }

  int GetVals() const { return ValStatH.Len(); }
  bool HasVal(const TGStatVal& StatVal) const;
  double GetVal(const TGStatVal& StatVal) const;
  void SetVal(const TGStatVal& StatVal, const double& Val);
  int GetDistrs() const { return DistrStatH.Len(); }
  bool HasDistr(const TGStatDistr& Distr) const { return DistrStatH.IsKey(Distr); }
  const TFltPrV& GetDistr(const TGStatDistr& Distr) const;
  void GetDistr(const TGStatDistr& Distr, TFltPrV& FltPrV) const;
  void SetDistr(const TGStatDistr& Distr, const TFltPrV& FltPrV);

  int GetNodes() const { return (int) GetVal(gsvNodes); }
  int GetEdges() const { return (int) GetVal(gsvEdges); }

  void TakeStat(const PNGraph& Graph, const TSecTm& Time, TFSet StatFSet, const TStr& GraphName);
  template <class PGraph> void TakeStat(const PGraph& Graph, const TSecTm& Time, TFSet StatFSet, const TStr& GraphName);
  template <class PGraph> void TakeBasicStat(const PGraph& Graph, const bool& IsMxWcc=false);
  template <class PGraph> void TakeBasicStat(const PGraph& Graph, TFSet FSet, const bool& IsMxWcc=false);
  template <class PGraph> void TakeDegDistr(const PGraph& Graph);
  template <class PGraph> void TakeDegDistr(const PGraph& Graph, TFSet StatFSet);
  template <class PGraph> void TakeDiam(const PGraph& Graph, const bool& IsMxWcc=false);
  template <class PGraph> void TakeDiam(const PGraph& Graph, TFSet StatFSet, const bool& IsMxWcc=false);
  template <class PGraph> void TakeConnComp(const PGraph& Graph);
  template <class PGraph> void TakeConnComp(const PGraph& Graph, TFSet StatFSet);
  template <class PGraph> void TakeClustCf(const PGraph& Graph, const int& SampleNodes=-1);
  template <class PGraph> void TakeTriadPart(const PGraph& Graph);
  void TakeSpectral(const PNGraph& Graph, const int _TakeSngVals = -1);
  void TakeSpectral(const PNGraph& Graph, TFSet StatFSet, int _TakeSngVals = -1);

  void Plot(const TGStatDistr& Distr, const TStr& FNmPref, TStr Desc=TStr(), bool PowerFit=false) const;
  void Plot(const TFSet& FSet, const TStr& FNmPref, TStr Desc=TStr(), bool PowerFit=false) const;
  void PlotAll(const TStr& FNmPref, TStr Desc=TStr(), bool PowerFit=false) const;
  void DumpValStat();

  void AvgGStat(const PGStatVec& GStatVec, const bool& ClipAt1=false);
  void AvgGStat(const TGStatV& GStatV, const bool& ClipAt1=false);

  // take graph statistics (see TTakeGStat)
  static TStr GetDistrStr(const TGStatDistr& Distr);
  static TStr GetValStr(const TGStatVal& Val);
  static TPlotInfo GetPlotInfo(const TGStatVal& Val);
  static TPlotInfo GetPlotInfo(const TGStatDistr& Distr);
  static TFSet NoStat();
  static TFSet BasicStat();
  static TFSet DegDStat();
  static TFSet NoDiamStat();
  static TFSet NoDistrStat();
  static TFSet NoSvdStat();
  static TFSet AllStat();

  friend class TCmpByVal;
  friend class TPt<TGStat>;
};

/////////////////////////////////////////////////
// Graph Statistisc Sequence
class TGStatVec {
public:
  static uint MinNodesEdges;
private:
  TCRef CRef;
  TTmUnit TmUnit;
  TFSet StatFSet;
  TGStatV GStatV; // each snapshot (TVec<PGStat>)
public:
  TGStatVec(const TTmUnit& _TmUnit=tmu1Sec);
  TGStatVec(const TTmUnit& _TmUnit, const TFSet& TakeGrowthStat);
  TGStatVec(const TGStatVec& GStat);
  static PGStatVec New(const TTmUnit& _TmUnit=tmu1Sec);
  static PGStatVec New(const TTmUnit& _TmUnit, const TFSet& TakeGrowthStat);
  static PGStatVec Load(TSIn& SIn) { return new TGStatVec(SIn); }
  TGStatVec(TSIn& SIn);
  void Save(TSOut& SOut) const;
  TGStatVec& operator = (const TGStatVec& GStat);

  PGStat Add();
  PGStat Add(const TSecTm& Time, TStr GraphNm=TStr());
  void Add(const PGStat& Growth) { GStatV.Add(Growth); }
  void Add(const PNGraph& Graph, const TSecTm& Time, const TStr& GraphNm=TStr());
  void Add(const PNEGraph& Graph, const TSecTm& Time, const TStr& GraphNm=TStr());
  void Clr() { GStatV.Clr(); }
  void Sort(const TGStatVal& SortBy=gsvNodes, const bool& Asc=true);

  int Len() const { return GStatV.Len(); }
  bool Empty() const { return GStatV.Empty(); }
  PGStat operator[](const int& ValN) const { return GStatV[ValN]; }
  PGStat At(const int& ValN) const { return GStatV[ValN]; }
  PGStat Last() const { return GStatV.Last(); }
  const TGStatV& GetGStatV() const { return GStatV; }

  void Del(const int& ValN) { GStatV.Del(ValN); }
  void DelLast() { GStatV.DelLast(); }
  void DelBefore(const TSecTm& Tm);
  void DelAfter(const TSecTm& Tm);
  void DelSmallNodes(const int& MinNodes);

  void SetTmUnit(const TTmUnit& TimeUnit) { TmUnit = TimeUnit; }
  TTmUnit GetTmUnit() const { return TmUnit; }
  void SetTakeStat(const TFSet& TakeStatSet) { StatFSet = TakeStatSet; }
  bool HasVal(const TGStatVal& Stat) const { return StatFSet.In(Stat); }
  bool HasDistr(const TGStatDistr& Stat) const { return StatFSet.In(Stat); }

  void GetValV(const TGStatVal& XVal, const TGStatVal& YVal, TFltPrV& ValV) const;
  PGStat GetAvgGStat(const bool& ClipAt1=false);

  void Plot(const TGStatVal& XVal, const TGStatVal& YVal, const TStr& OutFNm, TStr& Desc,
    const TGpScaleTy& Scale=gpsAuto, const bool& PowerFit=false) const;
  void PlotAllVsX(const TGStatVal& XVal, const TStr& OutFNm, TStr Desc=TStr(), const TGpScaleTy& Scale=gpsAuto, const bool& PowerFit=false) const;
  void ImposeDistr(const TGStatDistr& Distr, const TStr& FNmPref, TStr Desc=TStr(), const bool& ExpBin=false,
    const bool& PowerFit=false, const TGpSeriesTy& PlotWith=gpwLinesPoints, const TStr& Style="") const;

  void SaveTxt(const TStr& FNmPref, const TStr& Desc) const;
  friend class TPt<TGStatVec>;
};

/////////////////////////////////////////////////
// Implementation
template <class PGraph>
void TGStat::TakeStat(const PGraph& Graph, const TSecTm& _Time, TFSet StatFSet, const TStr& GraphName) {
  printf("**TakeStat:  G(%u, %u)\n", Graph->GetNodes(), Graph->GetEdges());
  TExeTm ExeTm, FullTm;
  Time = _Time;
  GraphNm = GraphName;
  if (StatFSet.In(gsvNone)) { return; }
  TakeBasicStat(Graph, false);
  if (StatFSet.In(gsdWcc)) {
    TakeBasicStat(TSnap::GetMxWcc(Graph), true);
  }
  // degrees
  TakeDegDistr(Graph, StatFSet);
  if (StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffDiam) || StatFSet.In(gsdHops) ||
   StatFSet.In(gsvEffWccDiam) || StatFSet.In(gsdWccHops) || StatFSet.In(gsdWcc) || StatFSet.In(gsdScc) ||
   StatFSet.In(gsdClustCf) || StatFSet.In(gsvClustCf) || StatFSet.In(gsdTriadPart)) {
    PNGraph NGraph = TSnap::ConvertGraph<PNGraph>(Graph, true);
    // diameter
    TakeDiam(NGraph, StatFSet, false);
    // components
    TakeConnComp(NGraph, StatFSet);
    // spectral
    TakeSpectral(NGraph, StatFSet, -1);
    // clustering coeffient
    if (StatFSet.In(gsdClustCf) || StatFSet.In(gsvClustCf)) {
      TakeClustCf(NGraph); }
    if (StatFSet.In(gsdTriadPart)) {
      TakeTriadPart(NGraph); }
    if (StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffWccDiam)) {
      TakeDiam(TSnap::GetMxWcc(NGraph), StatFSet, true); }
    printf("  [%s]\n", FullTm.GetTmStr());
  }
}

template <class PGraph>
void TGStat::TakeBasicStat(const PGraph& Graph, const bool& IsMxWcc) {
  TakeBasicStat(Graph, TFSet() | gsvBiDirEdges | gsvWccBiDirEdges, IsMxWcc);
}

template <class PGraph>
void TGStat::TakeBasicStat(const PGraph& Graph, TFSet FSet, const bool& IsMxWcc) {
  if (! IsMxWcc) {
    // gsvNodes, gsvZeroNodes, gsvNonZNodes, gsvSrcNodes, gsvDstNodes,
    // gsvEdges, gsvUniqEdges, gsvBiDirEdges
    printf("basic wcc...");
    const int Nodes = Graph->GetNodes();
    SetVal(gsvNodes, Nodes);
    SetVal(gsvZeroNodes, TSnap::CntDegNodes(Graph, 0));
    SetVal(gsvNonZNodes, Nodes - GetVal(gsvZeroNodes));
    SetVal(gsvSrcNodes, Nodes - TSnap::CntOutDegNodes(Graph, 0));
    SetVal(gsvDstNodes, Nodes - TSnap::CntInDegNodes(Graph, 0));
    SetVal(gsvEdges, Graph->GetEdges());
    if (! Graph->HasFlag(gfMultiGraph)) { SetVal(gsvUniqEdges, Graph->GetEdges()); }
    else { SetVal(gsvUniqEdges, TSnap::GetUniqDirEdges(Graph)); }
    if (FSet.In(gsvBiDirEdges)) {
      if (Graph->HasFlag(gfDirected)) { SetVal(gsvBiDirEdges, TSnap::GetUniqBiDirEdges(Graph)); }
      else { SetVal(gsvUniqEdges, GetVal(gsvEdges)); }
    }
    printf("\n");
  } else {
    // gsvWccNodes, gsvWccSrcNodes, gsvWccDstNodes, gsvWccEdges, gsvWccUniqEdges, gsvWccBiDirEdges
    printf("basic...");
    const int Nodes = Graph->GetNodes();
    SetVal(gsvWccNodes, Nodes);
    SetVal(gsvWccSrcNodes, Nodes - TSnap::CntOutDegNodes(Graph, 0));
    SetVal(gsvWccDstNodes, Nodes - TSnap::CntInDegNodes(Graph, 0));
    SetVal(gsvWccEdges, Graph->GetEdges());
    if (! Graph->HasFlag(gfMultiGraph)) { SetVal(gsvWccUniqEdges, Graph->GetEdges()); }
    else { SetVal(gsvWccUniqEdges, TSnap::GetUniqDirEdges(Graph)); }
    if (FSet.In(gsvBiDirEdges)) {
      if (Graph->HasFlag(gfDirected)) { SetVal(gsvWccBiDirEdges, TSnap::GetUniqBiDirEdges(Graph)); }
      else { SetVal(gsvUniqEdges, GetVal(gsvEdges)); }
    }
    printf("\n");
  }
}

template <class PGraph>
void TGStat::TakeDegDistr(const PGraph& Graph) {
  TakeDegDistr(Graph, TFSet() | gsdInDeg | gsdOutDeg);
}

template <class PGraph>
void TGStat::TakeDegDistr(const PGraph& Graph, TFSet StatFSet) {
  // degree distribution
  if (StatFSet.In(gsdOutDeg) || StatFSet.In(gsdOutDeg)) {
    printf("deg "); }
  if (StatFSet.In(gsdInDeg)) {
    printf(" in ");
    TFltPrV& InDegV = DistrStatH.AddDat(gsdInDeg);
    TSnap::GetInDegCnt(Graph, InDegV);
  }
  if (StatFSet.In(gsdOutDeg)) {
    printf(" out ");
    TFltPrV& OutDegV = DistrStatH.AddDat(gsdOutDeg);
    TSnap::GetOutDegCnt(Graph, OutDegV);
  }
  if (StatFSet.In(gsdOutDeg) || StatFSet.In(gsdOutDeg)) {
    printf("\n"); }
}

template <class PGraph>
void TGStat::TakeDiam(const PGraph& Graph, const bool& IsMxWcc) {
  TakeDiam(Graph, TFSet() | gsvFullDiam | gsvEffDiam | gsdHops |
    gsvEffWccDiam| gsdWccHops, IsMxWcc);
}

template <class PGraph>
void TGStat::TakeDiam(const PGraph& Graph, TFSet StatFSet, const bool& IsMxWcc) {
  TExeTm ExeTm;
  if (! IsMxWcc) {
    if (StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffDiam) || StatFSet.In(gsdHops)) {
      printf("ANF diam %d runs ", NDiamRuns); }
    bool Line=false;
    if (StatFSet.In(gsvEffDiam) || StatFSet.In(gsdHops)) {
      TMom DiamMom;  ExeTm.Tick();
      TIntFltKdV DistNbhsV;
      for (int r = 0; r < NDiamRuns; r++) {
        TSnap::GetAnf(Graph, DistNbhsV, -1, false, 32);
        DiamMom.Add(TAnf::CalcEffDiam(DistNbhsV, 0.9));
        printf(".");
      }
      DiamMom.Def();
      SetVal(gsvEffDiam, DiamMom.GetMean());
      SetVal(gsvEffDiamDev, DiamMom.GetSDev());
      TFltPrV& HopsV = DistrStatH.AddDat(gsdHops);
      HopsV.Gen(DistNbhsV.Len(), 0);
      for (int i = 0; i < DistNbhsV.Len(); i++) {
        HopsV.Add(TFltPr(DistNbhsV[i].Key(), DistNbhsV[i].Dat)); }
      printf("  ANF-eff %.1f[%s]", DiamMom.GetMean(), ExeTm.GetTmStr());
      Line=true;
    }
    if (Line) { printf("\n"); }
  } else {
    if (StatFSet.In(gsvEffWccDiam) || StatFSet.In(gsdWccHops)) { printf("wcc diam "); }
    bool Line=false;
    if (StatFSet.In(gsvFullDiam)) {
      TMom DiamMom;  ExeTm.Tick();
      for (int r = 0; r < NDiamRuns; r++) {
        DiamMom.Add(TSnap::GetBfsFullDiam(Graph, 1, false));
        printf("."); }
      DiamMom.Def();
      SetVal(gsvFullDiam, DiamMom.GetMean());
      SetVal(gsvFullDiamDev, DiamMom.GetSDev());
      printf("    BFS-full %g[%s]", DiamMom.GetMean(), ExeTm.GetTmStr());
      Line=true;
    }
    if (StatFSet.In(gsvEffWccDiam) || StatFSet.In(gsdWccHops)) {
      TMom DiamMom; ExeTm.Tick();
      TIntFltKdV DistNbhsV;
      for (int r = 0; r < NDiamRuns; r++) {
        TSnap::GetAnf(Graph, DistNbhsV, -1, false, 32);
        DiamMom.Add(TAnf::CalcEffDiam(DistNbhsV, 0.9));
        printf(".");
      }
      DiamMom.Def();
      SetVal(gsvEffWccDiam, DiamMom.GetMean());
      SetVal(gsvEffWccDiamDev, DiamMom.GetSDev());
      TFltPrV& WccHopsV = DistrStatH.AddDat(gsdWccHops);
      WccHopsV.Gen(DistNbhsV.Len(), 0);
      for (int i = 0; i < DistNbhsV.Len(); i++) {
        WccHopsV.Add(TFltPr(DistNbhsV[i].Key(), DistNbhsV[i].Dat)); }
      printf("  ANF-wccEff %.1f[%s]", DiamMom.GetMean(), ExeTm.GetTmStr());
      Line=true;
    }
    if (Line) { printf("\n"); }
  }
}

template <class PGraph>
void TGStat::TakeConnComp(const PGraph& Graph) {
  TakeConnComp(Graph, TFSet() | gsdWcc | gsdScc);
}

template <class PGraph>
void TGStat::TakeConnComp(const PGraph& Graph, TFSet StatFSet) {
  if (StatFSet.In(gsdWcc)) {
    printf("wcc ");
    TIntPrV WccSzCntV1;
    TSnap::GetWccSzCnt(Graph, WccSzCntV1);
    TFltPrV& WccSzCntV = DistrStatH.AddDat(gsdWcc);
    WccSzCntV.Gen(WccSzCntV1.Len(), 0);
    for (int i = 0; i < WccSzCntV1.Len(); i++)
      WccSzCntV.Add(TFltPr(WccSzCntV1[i].Val1(), WccSzCntV1[i].Val2()));
  }
  if (StatFSet.In(gsdScc)) {
    printf("scc ");
    TIntPrV SccSzCntV1;
    TSnap::GetSccSzCnt(Graph, SccSzCntV1);
    TFltPrV& SccSzCntV = DistrStatH.AddDat(gsdScc);
    SccSzCntV.Gen(SccSzCntV1.Len(), 0);
    for (int i = 0; i < SccSzCntV1.Len(); i++)
      SccSzCntV.Add(TFltPr(SccSzCntV1[i].Val1(), SccSzCntV1[i].Val2()));
  }
  if (StatFSet.In(gsdWcc) || StatFSet.In(gsdScc)) { printf("\n"); }
}

template <class PGraph>
void TGStat::TakeClustCf(const PGraph& Graph, const int& SampleNodes) {
  printf("clustCf ");
  TFltPrV& ClustCfV = DistrStatH.AddDat(gsdClustCf);
  int Open, Close;
  const double ClustCf =  TSnap::GetClustCf(Graph, ClustCfV, Close, Open, SampleNodes);
  SetVal(gsvClustCf, ClustCf);
  SetVal(gsvOpenTriads, Open);
  SetVal(gsvClosedTriads, Close);
}

template <class PGraph>
void TGStat::TakeTriadPart(const PGraph& Graph) {
  printf("triadParticip ");
  TFltPrV& TriadCntV = DistrStatH.AddDat(gsdTriadPart);
  TIntPrV CntV;
  TSnap::GetTriadParticip(Graph, CntV);
  TriadCntV.Gen(CntV.Len(), 0);
  for (int i = 0; i < CntV.Len(); i++) {
    TriadCntV.Add(TFltPr(CntV[i].Val1(), CntV[i].Val2()));
  }
}
