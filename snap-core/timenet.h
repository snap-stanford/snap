////////////////////////////////////////////////
// Forward declarations
class TGrowthStat;
typedef TPt<TGrowthStat> PGrowthStat;
class TGStatVec;
typedef TPt<TGStatVec> PGStatVec;

class TTimeNet;
typedef TPt<TTimeNet> PTimeNet;

class TTimeNENet;
typedef TPt<TTimeNENet> PTimeNENet;

/////////////////////////////////////////////////
// Time Network
class TTimeNet : public TNodeNet<TSecTm> {
public:
  typedef TNodeNet<TSecTm> TNet;
  typedef TPt<TNodeNet<TSecTm> > PNet;
public:
  class TTmBucket {
  public:
    TSecTm BegTm; // start time of the bucket, end time is determined by GroupBy
    TIntV NIdV;
  public:
    TTmBucket() { }
    TTmBucket(const TSecTm& BucketTm) : BegTm(BucketTm) { }
    TTmBucket(const TTmBucket& TmBucket) : BegTm(TmBucket.BegTm), NIdV(TmBucket.NIdV) { }
    TTmBucket& operator = (const TTmBucket& TmBucket) { if (this!=&TmBucket) {
      BegTm=TmBucket.BegTm; NIdV=TmBucket.NIdV; } return *this; }
  };
  typedef TVec<TTmBucket> TTmBucketV;
public:
  TTimeNet() { }
  TTimeNet(const int& Nodes, const int& Edges) : TNet(Nodes, Edges) { }
  TTimeNet(const TTimeNet& TimeNet) : TNet(TimeNet) { }
  TTimeNet(TSIn& SIn) : TNet(SIn) { }
  void Save(TSOut& SOut) const { TNet::Save(SOut); }
  static PTimeNet New() { return new TTimeNet(); }
  static PTimeNet New(const int& Nodes, const int& Edges) { return new TTimeNet(Nodes, Edges); }
  static PTimeNet Load(TSIn& SIn) { return new TTimeNet(SIn); }
  TTimeNet& operator = (const TTimeNet& TimeNet);

  PTimeNet GetSubGraph(const TIntV& NIdV) const;
  PTimeNENet GetTimeNENet() const;

  void GetNIdByTm(TIntV& NIdV) const;
  void GetTmBuckets(const TTmUnit& GroupBy, TTmBucketV& TmBucketV) const;
  void GetNodeBuckets(const int NodesPerBucket, TTimeNet::TTmBucketV& TmBucketV) const;

  PGStatVec TimeGrowth(const TTmUnit& TmUnit, const TFSet& TakeStat, const TSecTm& StartTm) const;
  void PlotEffDiam(const TStr& FNmPref, const TStr& Desc, const TTmUnit& GroupBy, const TSecTm& StartTm,
    const int& NDiamRuns=10, const bool& OnlyWcc=false, const bool& AlsoRewire=false) const;
  void PlotMissingPast(const TStr& FNmPref, const TStr& Desc, const TTmUnit& GroupBy,
    const TSecTm& DelPreTmEdges, const TSecTm& PostTmDiam) const;
  void PlotCCfOverTm(const TStr& FNmPref, TStr Desc, const TTmUnit& TmUnit, const int& NodesBucket=-1) const;
  void PlotMedianDegOverTm(const TStr& FNmPref, const TTmUnit& TmUnit, const int& NodesPerBucket=-1) const;

  // networks
  static PTimeNet LoadBipartite(const TStr& InFNm);
  static PTimeNet LoadArxiv(const TStr& PaperFNm, const TStr& CiteFNm);
  static PTimeNet LoadPatents(const TStr& PatentFNm, const TStr& CiteFNm);
  static PTimeNet LoadAmazon(const TStr& StlFNm);

  friend class TPt<TTimeNet>;
};

// set flags
namespace TSnap {
template <> struct IsDirected<TTimeNet> { enum { Val = 1 }; };
template <> struct IsNodeDat<TTimeNet> { enum { Val = 1 }; };
}

/////////////////////////////////////////////////
// Time Node-Edge Network
class TTimeNENet : public TNodeEdgeNet<TSecTm, TSecTm> {
public:
  typedef TNodeEdgeNet<TSecTm, TSecTm> TNet;
  typedef TPt<TNodeEdgeNet<TSecTm, TSecTm> > PNet;
public:
  TTimeNENet() { }
  TTimeNENet(const int& Nodes, const int& Edges) : TNet(Nodes, Edges) { }
  TTimeNENet(TSIn& SIn) : TNet(SIn) { }
  TTimeNENet(const TTimeNENet& TimeNet) : TNet(TimeNet) { }
  void Save(TSOut& SOut) const { TNet::Save(SOut); }
  static PTimeNENet New() { return new TTimeNENet(); }
  static PTimeNENet New(const int& Nodes, const int& Edges) { return new TTimeNENet(Nodes, Edges); }
  static PTimeNENet Load(TSIn& SIn) { return new TTimeNENet(SIn); }
  TTimeNENet& operator = (const TTimeNENet& TimeNet);

  PTimeNet GetTimeNet() const;
  PTimeNENet Get1stEdgeNet() const;
  PTimeNENet GetSubGraph(const TIntV& NIdV) const;
  PTimeNENet GetESubGraph(const TIntV& EIdV) const;
  PTimeNENet GetGraphUpToTm(const TSecTm& MaxEdgeTm) const;

  void SortNodeEdgeTimes();
  void UpdateNodeTimes();
  void SetNodeTmToFirstEdgeTm();
  void SetRndEdgeTimes(const int& MinTmEdge=0);
  void DumpTimeStat() const;

  void GetNIdByTm(TIntV& NIdV) const;
  void GetEIdByTm(TIntV& EIdV) const;
  void GetTmBuckets(const TTmUnit& GroupBy, TTimeNet::TTmBucketV& TmBucketV) const;
  void GetEdgeTmBuckets(const TTmUnit& GroupBy, TTimeNet::TTmBucketV& TmBucketV) const;
  void GetNodeBuckets(const int NodesPerBucket, TTimeNet::TTmBucketV& TmBucketV) const;
  void GetEdgeBuckets(const int EdgesPerBucket, TTimeNet::TTmBucketV& TmBucketV) const;
  int GetTriadEdges(TIntV& TriadEIdV) const;

  PGStatVec TimeGrowth(const TTmUnit& TimeStep, const TFSet& TakeStat, const TSecTm& StartTm=TSecTm(1)) const;
  PGStatVec TimeGrowth(const TStr& FNmPref, const TStr& Desc, const TFSet& TakeStat, const int& NDiamRuns,
    const TTmUnit& TmUnit, const int& TakeNTmUnits, const bool& LinkBWays) const;
  void PlotEffDiam(const TStr& FNmPref, const TStr& Desc, const TTmUnit& GroupBy, const TSecTm& StartTm,
    const int& NDiamRuns=10, const bool& OnlyWcc=false) const;
  void PlotMissingPast(const TStr& FNmPref, const TStr& Desc, const TTmUnit& TmUnit,
    const TSecTm& DelPreTmEdges, const TSecTm& PostTmDiam, const bool& LinkBWays);

  void SaveEdgeTm(const TStr& EdgeFNm, const bool& RenumberNId=false, const bool& RelativeTm=false) const;
  static PTimeNENet GetSmallNet();
  static PTimeNENet GetGnmRndNet(const int& Nodes, const int& Edges);
  static PTimeNENet GetPrefAttach(const int& Nodes, const int& Edges, const double& GammaIn, const double& GammaOut);
  static PTimeNENet GetPrefAttach(const int& Nodes, const int& OutDeg);
  static PTimeNENet LoadFlickr(const TStr& NodeFNm, const TStr& EdgeFNm);
  static PTimeNENet LoadEdgeTm(const TStr& EdgeFNm, const int& SrcFld=0, const int& DstFld=1, const int& TimeFld=2, const TSsFmt& Separator=ssfTabSep);

  friend class TPt<TTimeNENet>;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TTimeNENet> { enum { Val = 1 }; };
template <> struct IsDirected<TTimeNENet> { enum { Val = 1 }; };
template <> struct IsNodeDat<TTimeNENet> { enum { Val = 1 }; };
template <> struct IsEdgeDat<TTimeNENet> { enum { Val = 1 }; };
}
