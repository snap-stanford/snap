/////////////////////////////////////////////////
// Forest Fire
class TForestFire {
private:
  TRnd Rnd;
  PNGraph Graph;
  TFlt FwdBurnProb, BckBurnProb, ProbDecay;
  TIntV InfectNIdV;  // nodes to start fire
  TIntV BurnedNIdV;  // nodes that got burned (FF model creates edges to them)
  // statistics
  TIntV NBurnedTmV, NBurningTmV, NewBurnedTmV; // total burned, currently burning, newly burned in current time step
private:
  UndefCopyAssign(TForestFire);
public:
  TForestFire() : Rnd(1), Graph(), FwdBurnProb(0.0), BckBurnProb(0.0), ProbDecay(1.0) { }
  TForestFire(const PNGraph& GraphPt, const double& ForwBurnProb, const double& BackBurnProb, const double& DecayProb=1.0, const int& RndSeed=1) :
    Rnd(RndSeed), Graph(GraphPt), FwdBurnProb(ForwBurnProb), BckBurnProb(BackBurnProb), ProbDecay(DecayProb) { }

  void SetGraph(const PNGraph& GraphPt) { Graph = GraphPt; }
  PNGraph GetGraph() const { return Graph; }
  void SetBurnProb(const double& ForwBurnProb, const double& BackBurnProb) { FwdBurnProb=ForwBurnProb;  BckBurnProb=BackBurnProb; }
  void SetProbDecay(const double& DecayProb) { ProbDecay = DecayProb; }

  void Infect(const int& NodeId) { InfectNIdV.Gen(1,1);  InfectNIdV[0] = NodeId; }
  void Infect(const TIntV& InfectedNIdV) { InfectNIdV = InfectedNIdV; }
  void InfectAll();
  void InfectRnd(const int& NInfect);

  void BurnExpFire();  // burn each link independently with prob BurnProb (burn fixed percentage of links)
  void BurnGeoFire();  // burn fixed number of links

  int GetFireTm() const { return NBurnedTmV.Len(); } // time of fire
  int GetBurned() const { return BurnedNIdV.Len(); }
  int GetBurnedNId(const int& NIdN) const { return BurnedNIdV[NIdN]; }
  const TIntV& GetBurnedNIdV() const { return BurnedNIdV; }
  void GetBurnedNIdV(TIntV& NIdV) const { NIdV = BurnedNIdV; }
  void PlotFire(const TStr& FNmPref, const TStr& Desc, const bool& PlotAllBurned=false);

  static PNGraph GenGraph(const int& Nodes, const double& FwdProb, const double& BckProb);
};


/////////////////////////////////////////////////
// Forest Fire Graph Generator
class TFfGGen {
public:
  typedef enum { srUndef, srOk, srFlood, srTimeLimit } TStopReason;
  static int TimeLimitSec;
private:
  PNGraph Graph;
  // parameters
  TBool BurnExpFire;   // burn Exponential or Geometric fire
  TInt StartNodes;     // start a graph with N isolated nodes
  TFlt FwdBurnProb, BckBurnProb, ProbDecay; // Forest Fire parameters
  TFlt Take2AmbProb, OrphanProb;
public:
  TFfGGen(const bool& BurnExpFireP, const int& StartNNodes, const double& ForwBurnProb,
    const double& BackBurnProb, const double& DecayProb, const double& Take2AmbasPrb, const double& OrphanPrb);

  PNGraph GetGraph() const { return Graph; }
  void SetGraph(const PNGraph& NGraph) { Graph = NGraph; }
  void Clr() { Graph->Clr(); }
  TStr GetParamStr() const;

  TStopReason AddNodes(const int& GraphNodes, const bool& FloodStop = true);
  TStopReason GenGraph(const int& GraphNodes, const bool& FloodStop = true);
  TStopReason GenGraph(const int& GraphNodes, PGStatVec& EvolStat, const bool& FloodStop=true);
  void PlotFireSize(const TStr& FNmPref, const TStr& DescStr);
  static void GenFFGraphs(const double& FProb, const double& BProb, const TStr& FNm);
};

/*/////////////////////////////////////////////////
// Forest Fire Phase Transition
ClassTP(TFfPhaseTrans, PFfPhaseTrans)// {
private:
  TBool BurExpFire;
  TInt NNodes, StartNodes, NRuns;
  TFlt Take2AmbProb, OrphanProb, ProbInc;
  THash<TFltPr, PGrowthSet> FBPrGSetH; // (Fwd, Bck) -> GrowthSet
  THash<TFltPr, PGrowthStat> FBPrGStatH; // (Fwd, Bck) -> AvgGrowthStat
  TFSet TakeStatSet;
public:
  TFfPhaseTrans(const int& NNds, const int& StartNds,  const double& Take2AmbPr,
    const double& ProbOrphan, const double& ProbIncrement, const int& NRunsPerFB);
  static PFfPhaseTrans New(const int& NNds, const int& StartNds,  const double& Take2AmbPr,
    const double& ProbOrphan, const double& ProbIncrement, const int& NRunsPerFB);
  TFfPhaseTrans(TSIn& SIn);
  static PFfPhaseTrans Load(TSIn& SIn);
  static PFfPhaseTrans Load(const TStr& InFNm) { TFIn FIn(InFNm); return TFfPhaseTrans::Load(FIn); }
  void Save(TFOut& SOut) const;

  int Len() const { return FBPrGSetH.Len(); }
  void TakeStat(const TFSet& TakeStatFSet) { TakeStatSet = TakeStatFSet; }
  TStr GetTitleStr(const int& ValN) const;
  TStr GetFNm(const TStr& FNmPref) const;

  double GetFProb(const int& ValN) const { return FBPrGSetH.GetKey(ValN).Val1(); }
  double GetBProb(const int& ValN) const { return FBPrGSetH.GetKey(ValN).Val2(); }
  TFltPr GetFBProb(const int& ValN) const { return FBPrGSetH.GetKey(ValN); }
  PGrowthSet GetGSet(const int& ValN) const { return FBPrGSetH[ValN]; }
  PGrowthStat GetAvgGStat(const int& ValN) const { return FBPrGStatH[ValN]; }
  TFltPr GetGplCf(const int& ValN) const;
  TFltPr GetDecDiam(const int& ValN) const;
  TFltPr GetEffDiam(const int& ValN, const int& AtTick) const;
  TFltPr LastGplCf() const { return GetGplCf(Len()-1); }
  TFltPr LastDecDiam() const { return GetDecDiam(Len()-1); }

  void GetFProbV(TFltV& FProbV) const;
  TFltPr RunForestFire(double FwdProb, double BckProb, const bool& Plot=true);

  void FwdProbSteps(const double& MinFwdProb, const double& MaxFwdProb, const double& BckProb);
  void FwdProbStepsFact(const double& MinFwdProb, const double& MaxFwdProb, const double& BckFact);
  void FwdBckPhasePlot(const double& MinFwdProb, const double& MaxFwdProb,
    const double& MinBckFact, const double& MaxBckFact, const int& TimeLimitSec);

  void FindGplPhaseTr(const double& StartFProb, const double& FollowGplCf, const TStr& FNmPref, const TStr& Desc=TStr());
  void SaveGplPhaseTr(const double& FollowGplCf, const TStr& FNmPref, const TStr& Desc=TStr());
  void FindDiamPhaseTr(const double& StartFProb, const double& FollowDiamCf, const TStr& FNmPref, const TStr& Desc=TStr());
  void SaveDiamPhaseTr(const double& FollowDiamCf, const TStr& FNmPref, const TStr& Desc=TStr());

  void Merge(const PFfPhaseTrans& FfPhaseTrans);
  void Merge(const TFfPhaseTrans& FfPhaseTrans);
};
//*/

/////////////////////////////////////////////////
// Undirected Forest Fire (does not densify!)
class TUndirFFire {
private:
  TRnd Rnd;
  PUNGraph Graph;
  double BurnProb;
  TIntSet BurnedSet; // all burned nodes in the current iteration
  TIntV BurningNIdV, NewBurnedNIdV, AliveNIdV; // temporary
public:
  TUndirFFire(const double& _BurnProb=0.3) : Graph(TUNGraph::New()), BurnProb(_BurnProb) { }
  void SetGraph(const PUNGraph& GraphPt) { Graph = GraphPt; }
  PUNGraph GetGraph() const { return Graph; }
  int GetNBurned() const { return BurnedSet.Len(); }
  int GetBurnedNId(const int& n) const { return BurnedSet[n]; }
  int BurnGeoFire(const int& StartNId);
  TFfGGen::TStopReason AddNodes(const int& GraphNodes, const bool& FloodStop=true);
};

