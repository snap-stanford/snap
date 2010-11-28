/////////////////////////////////////////////////
// Forest Fire
void TForestFire::InfectAll() {
  InfectNIdV.Gen(Graph->GetNodes());
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    InfectNIdV.Add(NI.GetId()); }
}

void TForestFire::InfectRnd(const int& NInfect) {
  IAssert(NInfect < Graph->GetNodes());
  TIntV NIdV(Graph->GetNodes(), 0);
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdV.Add(NI.GetId()); }
  NIdV.Shuffle(Rnd);
  InfectNIdV.Gen(NInfect, 0);
  for (int i = 0; i < NInfect; i++) {
    InfectNIdV.Add(NIdV[i]); }
}

// burn each link independently (forward with FwdBurnProb, backward with BckBurnProb)
void TForestFire::BurnExpFire() {
  const double OldFwdBurnProb = FwdBurnProb;
  const double OldBckBurnProb = BckBurnProb;
  const int NInfect = InfectNIdV.Len();
  const TNGraph& G = *Graph;
  TIntH BurnedNIdH;               // burned nodes
  TIntV BurningNIdV = InfectNIdV; // currently burning nodes
  TIntV NewBurnedNIdV;            // nodes newly burned in current step
  bool HasAliveNbhs;              // has unburned neighbors
  int NBurned = NInfect, NDiedFire=0;
  for (int i = 0; i < InfectNIdV.Len(); i++) {
    BurnedNIdH.AddDat(InfectNIdV[i]); }
  NBurnedTmV.Clr(false);  NBurningTmV.Clr(false);  NewBurnedTmV.Clr(false);
  for (int time = 0; ; time++) {
    NewBurnedNIdV.Clr(false);
    // for each burning node
    for (int node = 0; node < BurningNIdV.Len(); node++) {
      const int& BurningNId = BurningNIdV[node];
      const TNGraph::TNodeI Node = G.GetNI(BurningNId);
      HasAliveNbhs = false;
      NDiedFire = 0;
      // burn forward links  (out-links)
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        const int OutNId = Node.GetOutNId(e);
        if (! BurnedNIdH.IsKey(OutNId)) { // not yet burned
          HasAliveNbhs = true;
          if (Rnd.GetUniDev() < FwdBurnProb) {
            BurnedNIdH.AddDat(OutNId);  NewBurnedNIdV.Add(OutNId);  NBurned++; }
        }
      }
      // burn backward links (in-links)
      if (BckBurnProb > 0.0) {
        for (int e = 0; e < Node.GetInDeg(); e++) {
          const int InNId = Node.GetInNId(e);
          if (! BurnedNIdH.IsKey(InNId)) { // not yet burned
            HasAliveNbhs = true;
            if (Rnd.GetUniDev() < BckBurnProb) {
              BurnedNIdH.AddDat(InNId);  NewBurnedNIdV.Add(InNId);  NBurned++; }
          }
        }
      }
      if (! HasAliveNbhs) { NDiedFire++; }
    }
    NBurnedTmV.Add(NBurned);
    NBurningTmV.Add(BurningNIdV.Len() - NDiedFire);
    NewBurnedTmV.Add(NewBurnedNIdV.Len());
    //BurningNIdV.AddV(NewBurnedNIdV);   // node is burning eternally
    BurningNIdV.Swap(NewBurnedNIdV);    // node is burning just 1 time step
    if (BurningNIdV.Empty()) break;
    FwdBurnProb = FwdBurnProb * ProbDecay;
    BckBurnProb = BckBurnProb * ProbDecay;
  }
  BurnedNIdV.Gen(BurnedNIdH.Len(), 0);
  for (int i = 0; i < BurnedNIdH.Len(); i++) {
    BurnedNIdV.Add(BurnedNIdH.GetKey(i)); }
  FwdBurnProb = OldFwdBurnProb;
  BckBurnProb = OldBckBurnProb;
}

// Node selects N~geometric(1.0-FwdBurnProb)-1 out-links and burns them. Then same for in-links.
// geometirc(p) has mean 1/(p), so for given FwdBurnProb, we burn 1/(1-FwdBurnProb)
void TForestFire::BurnGeoFire() {
  const double OldFwdBurnProb=FwdBurnProb;
  const double OldBckBurnProb=BckBurnProb;
  const int& NInfect = InfectNIdV.Len();
  const TNGraph& G = *Graph;
  TIntH BurnedNIdH;               // burned nodes
  TIntV BurningNIdV = InfectNIdV; // currently burning nodes
  TIntV NewBurnedNIdV;            // nodes newly burned in current step
  bool HasAliveInNbhs, HasAliveOutNbhs; // has unburned neighbors
  TIntV AliveNIdV;                // NIds of alive neighbors
  int NBurned = NInfect, time;
  for (int i = 0; i < InfectNIdV.Len(); i++) {
    BurnedNIdH.AddDat(InfectNIdV[i]); }
  NBurnedTmV.Clr(false);  NBurningTmV.Clr(false);  NewBurnedTmV.Clr(false);
  for (time = 0; ; time++) {
    NewBurnedNIdV.Clr(false);
    for (int node = 0; node < BurningNIdV.Len(); node++) {
      const int& BurningNId = BurningNIdV[node];
      const TNGraph::TNodeI Node = G.GetNI(BurningNId);
      // find unburned links
      HasAliveOutNbhs = false;
      AliveNIdV.Clr(false); // unburned links
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        const int OutNId = Node.GetOutNId(e);
        if (! BurnedNIdH.IsKey(OutNId)) {
          HasAliveOutNbhs = true;  AliveNIdV.Add(OutNId); }
      }
      // number of links to burn (geometric coin). Can also burn 0 links
      const int BurnNFwdLinks = Rnd.GetGeoDev(1.0-FwdBurnProb) - 1;
      if (HasAliveOutNbhs && BurnNFwdLinks > 0) {
        AliveNIdV.Shuffle(Rnd);
        for (int i = 0; i < TMath::Mn(BurnNFwdLinks, AliveNIdV.Len()); i++) {
          BurnedNIdH.AddDat(AliveNIdV[i]);
          NewBurnedNIdV.Add(AliveNIdV[i]);  NBurned++; }
      }
      // backward links
      if (BckBurnProb > 0.0) {
        // find unburned links
        HasAliveInNbhs = false;
        AliveNIdV.Clr(false);
        for (int e = 0; e < Node.GetInDeg(); e++) {
          const int InNId = Node.GetInNId(e);
          if (! BurnedNIdH.IsKey(InNId)) {
            HasAliveInNbhs = true;  AliveNIdV.Add(InNId); }
        }
         // number of links to burn (geometric coin). Can also burn 0 links
        const int BurnNBckLinks = Rnd.GetGeoDev(1.0-BckBurnProb) - 1;
        if (HasAliveInNbhs && BurnNBckLinks > 0) {
          AliveNIdV.Shuffle(Rnd);
          for (int i = 0; i < TMath::Mn(BurnNBckLinks, AliveNIdV.Len()); i++) {
            BurnedNIdH.AddDat(AliveNIdV[i]);
            NewBurnedNIdV.Add(AliveNIdV[i]);  NBurned++; }
        }
      }
    }
    NBurnedTmV.Add(NBurned);  NBurningTmV.Add(BurningNIdV.Len());  NewBurnedTmV.Add(NewBurnedNIdV.Len());
    // BurningNIdV.AddV(NewBurnedNIdV);   // node is burning eternally
    BurningNIdV.Swap(NewBurnedNIdV);   // node is burning just 1 time step
    if (BurningNIdV.Empty()) break;
    FwdBurnProb = FwdBurnProb * ProbDecay;
    BckBurnProb = BckBurnProb * ProbDecay;
  }
  BurnedNIdV.Gen(BurnedNIdH.Len(), 0);
  for (int i = 0; i < BurnedNIdH.Len(); i++) {
    BurnedNIdV.Add(BurnedNIdH.GetKey(i)); }
  FwdBurnProb = OldFwdBurnProb;
  BckBurnProb = OldBckBurnProb;
}

/*// exact implementation of the algorithm described in KDD '05 paper
void TForestFire::BurnGeoFire() {
  const double OldFwdBurnProb=FwdBurnProb;
  const double OldBckBurnProb=BckBurnProb;
  const double ProbRatio = BckBurnProb/FwdBurnProb;
  const int NInfect = InfectNIdV.Len();
  const TNGraph& G = *Graph;
  TIntH BurnedNIdH;               // burned nodes
  TIntV BurningNIdV = InfectNIdV; // currently burning nodes
  TIntV NewBurnedNIdV;            // nodes newly burned in current step
  bool HasAliveInNbhs, HasAliveOutNbhs; // has unburned neighbors
  TIntV AliveNIdV;                // NIds of alive neighbors
  int NBurned = NInfect, time;
  for (int i = 0; i < InfectNIdV.Len(); i++) {
    BurnedNIdH.AddDat(InfectNIdV[i]); }
  NBurnedTmV.Clr(false);  NBurningTmV.Clr(false);  NewBurnedTmV.Clr(false);
  for (time = 0; ; time++) {
    NewBurnedNIdV.Clr(false);
    for (int node = 0; node < BurningNIdV.Len(); node++) {
      const int& BurningNId = BurningNIdV[node];
      const int BurnNLinks = Rnd.GetGeoDev(1.0-(FwdBurnProb)) - 1;
      const TNGraph::TNode& Node = G.GetNode(BurningNId);
      // find unburned links
      if (BurnNLinks > 0) {
        AliveNIdV.Clr(false);
        for (int e = 0; e < Node.GetOutDeg(); e++) {
          const int OutNId = Node.GetOutNId(e);
          if (! BurnedNIdH.IsKey(OutNId)) { HasAliveOutNbhs=true;  AliveNIdV.Add(OutNId); }
        }
        for (int e = 0; e < Node.GetInDeg(); e++) {
          const int InNId = Node.GetInNId(e);
          if (! BurnedNIdH.IsKey(InNId)) { HasAliveInNbhs=true;  AliveNIdV.Add(-InNId); }
        }
        AliveNIdV.Shuffle(Rnd);
        // add links
        for (int e = i; i < AliveNIdV.Len(); i++) {
          if (AliveNIdV[i] > 0) BurnedNIdH.AddDat(AliveNIdV[i]);
          NewBurnedNIdV.Add(AliveNIdV[i]);  NBurned++;
        }
      }
      HasAliveOutNbhs = false;
      AliveNIdV.Clr(false); // unburned links
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        const int OutNId = Node.GetOutNId(e);
        if (! BurnedNIdH.IsKey(OutNId)) {
          HasAliveOutNbhs = true;  AliveNIdV.Add(OutNId); }
      }
      // number of links to burn (geometric coin). Can also burn 0 links
      const int BurnNFwdLinks = Rnd.GetGeoDev(1.0-FwdBurnProb) - 1;
      if (HasAliveOutNbhs && BurnNFwdLinks > 0) {
        AliveNIdV.Shuffle(Rnd);
        for (int i = 0; i < TMath::Mn(BurnNFwdLinks, AliveNIdV.Len()); i++) {
          BurnedNIdH.AddDat(AliveNIdV[i]);
          NewBurnedNIdV.Add(AliveNIdV[i]);  NBurned++; }
      }
      // backward links
      if (BckBurnProb > 0.0) {
        // find unburned links
        HasAliveInNbhs = false;
        AliveNIdV.Clr(false);
        for (int e = 0; e < Node.GetInDeg(); e++) {
          const int InNId = Node.GetInNId(e);
          if (! BurnedNIdH.IsKey(InNId)) {
            HasAliveInNbhs = true;  AliveNIdV.Add(InNId); }
        }
         // number of links to burn (geometric coin). Can also burn 0 links
        const int BurnNBckLinks = Rnd.GetGeoDev(1.0-BckBurnProb) - 1;
        if (HasAliveInNbhs && BurnNBckLinks > 0) {
          AliveNIdV.Shuffle(Rnd);
          for (int i = 0; i < TMath::Mn(BurnNBckLinks, AliveNIdV.Len()); i++) {
            BurnedNIdH.AddDat(AliveNIdV[i]);
            NewBurnedNIdV.Add(AliveNIdV[i]);  NBurned++; }
        }
      }
    }
    NBurnedTmV.Add(NBurned);  NBurningTmV.Add(BurningNIdV.Len());  NewBurnedTmV.Add(NewBurnedNIdV.Len());
    // BurningNIdV.AddV(NewBurnedNIdV);   // node is burning eternally
    BurningNIdV.Swap(NewBurnedNIdV);   // node is burning just 1 time step
    if (BurningNIdV.Empty()) break;
    FwdBurnProb = FwdBurnProb * ProbDecay;
    BckBurnProb = BckBurnProb * ProbDecay;
  }
  BurnedNIdV.Gen(BurnedNIdH.Len(), 0);
  for (int i = 0; i < BurnedNIdH.Len(); i++) {
    BurnedNIdV.Add(BurnedNIdH.GetKey(i)); }
  FwdBurnProb = OldFwdBurnProb;
  BckBurnProb = OldBckBurnProb;
}//*/

void TForestFire::PlotFire(const TStr& FNmPref, const TStr& Desc, const bool& PlotAllBurned) {
  TGnuPlot GnuPlot(FNmPref, TStr::Fmt("%s. ForestFire. G(%d, %d). Fwd:%g  Bck:%g  NInfect:%d",
    Desc.CStr(), Graph->GetNodes(), Graph->GetEdges(), FwdBurnProb(), BckBurnProb(), InfectNIdV.Len()));
  GnuPlot.SetXYLabel("TIME EPOCH", "Number of NODES");
  if (PlotAllBurned) GnuPlot.AddPlot(NBurnedTmV, gpwLinesPoints, "All burned nodes till time");
  GnuPlot.AddPlot(NBurningTmV, gpwLinesPoints, "Burning nodes at time");
  GnuPlot.AddPlot(NewBurnedTmV, gpwLinesPoints, "Newly burned nodes at time");
  GnuPlot.SavePng(TFile::GetUniqueFNm(TStr::Fmt("fireSz.%s_#.png", FNmPref.CStr())));
}

PNGraph TForestFire::GenGraph(const int& Nodes, const double& FwdProb, const double& BckProb) {
  TFfGGen Ff(false, 1, FwdProb, BckProb, 1.0, 0.0, 0.0);
  Ff.GenGraph(Nodes);
  return Ff.GetGraph();
}

/////////////////////////////////////////////////
// Forest Fire Graph Generator
int TFfGGen::TimeLimitSec = 30*60; // 30 minutes

TFfGGen::TFfGGen(const bool& BurnExpFireP, const int& StartNNodes, const double& ForwBurnProb,
                 const double& BackBurnProb, const double& DecayProb, const double& Take2AmbasPrb, const double& OrphanPrb) :
 Graph(), BurnExpFire(BurnExpFireP), StartNodes(StartNNodes), FwdBurnProb(ForwBurnProb),
 BckBurnProb(BackBurnProb), ProbDecay(DecayProb), Take2AmbProb(Take2AmbasPrb), OrphanProb(OrphanPrb) {
  //IAssert(ProbDecay == 1.0); // do not need Decay any more
}

TStr TFfGGen::GetParamStr() const {
  return TStr::Fmt("%s  FWD:%g  BCK:%g, StartNds:%d, Take2:%g, Orphan:%g, ProbDecay:%g",
    BurnExpFire?"EXP":"GEO", FwdBurnProb(), BckBurnProb(), StartNodes(), Take2AmbProb(), OrphanProb(), ProbDecay());
}

TFfGGen::TStopReason TFfGGen::AddNodes(const int& GraphNodes, const bool& FloodStop) {
  printf("\n***ForestFire:  %s  Nodes:%d  StartNodes:%d  Take2AmbProb:%g\n", BurnExpFire?"ExpFire":"GeoFire", GraphNodes, StartNodes(), Take2AmbProb());
  printf("                FwdBurnP:%g  BckBurnP:%g  ProbDecay:%g  Orphan:%g\n", FwdBurnProb(), BckBurnProb(), ProbDecay(), OrphanProb());
  TExeTm ExeTm;
  int Burned1=0, Burned2=0, Burned3=0; // last 3 fire sizes
  // create initial set of nodes
  if (Graph.Empty()) { Graph = PNGraph::New(); }
  if (Graph->GetNodes() == 0) {
    for (int n = 0; n < StartNodes; n++) { Graph->AddNode(); }
  }
  int NEdges = Graph->GetEdges();
  // forest fire
  TRnd Rnd(0);
  TForestFire ForestFire(Graph, FwdBurnProb, BckBurnProb, ProbDecay, 0);
  // add nodes
  for (int NNodes = Graph->GetNodes()+1; NNodes <= GraphNodes; NNodes++) {
    const int NewNId = Graph->AddNode(-1);
    IAssert(NewNId == Graph->GetNodes()-1); // node ids have to be 0...N
    // not an Orphan (burn fire)
    if (OrphanProb == 0.0 || Rnd.GetUniDev() > OrphanProb) {
      // infect ambassadors
      if (Take2AmbProb == 0.0 || Rnd.GetUniDev() > Take2AmbProb || NewNId < 2) {
        ForestFire.Infect(Rnd.GetUniDevInt(NewNId)); // take 1 ambassador
      } else {
        const int AmbassadorNId1 = Rnd.GetUniDevInt(NewNId);
        int AmbassadorNId2 = Rnd.GetUniDevInt(NewNId);
        while (AmbassadorNId1 == AmbassadorNId2) {
          AmbassadorNId2 = Rnd.GetUniDevInt(NewNId); }
        ForestFire.Infect(TIntV::GetV(AmbassadorNId1, AmbassadorNId2)); // take 2 ambassadors
      }
      // burn fire
      if (BurnExpFire) { ForestFire.BurnExpFire(); }
      else { ForestFire.BurnGeoFire(); }
      // add edges to burned nodes
      for (int e = 0; e < ForestFire.GetBurned(); e++) {
        Graph->AddEdge(NewNId, ForestFire.GetBurnedNId(e));
        NEdges++;
      }
      Burned1=Burned2;  Burned2=Burned3;  Burned3=ForestFire.GetBurned();
    } else {
      // Orphan (zero out-links)
      Burned1=Burned2;  Burned2=Burned3;  Burned3=0;
    }
    if (NNodes % Kilo(1) == 0) {
      printf("(%d, %d)  burned: [%d,%d,%d]  [%s]\n", NNodes, NEdges, Burned1, Burned2, Burned3, ExeTm.GetStr()); }
    if (FloodStop && NEdges>GraphNodes && (NEdges/double(NNodes)>1000.0)) { // average node degree is more than 500
      printf(". FLOOD. G(%6d, %6d)\n", NNodes, NEdges);  return srFlood; }
    if (NNodes % 1000 == 0 && TimeLimitSec > 0 && ExeTm.GetSecs() > TimeLimitSec) {
      printf(". TIME LIMIT. G(%d, %d)\n", Graph->GetNodes(), Graph->GetEdges());
      return srTimeLimit; }
  }
  IAssert(Graph->GetEdges() == NEdges);
  return srOk;
}

TFfGGen::TStopReason TFfGGen::GenGraph(const int& GraphNodes, const bool& FloodStop) {
  Graph = PNGraph::New();
  return AddNodes(GraphNodes, FloodStop);
}

TFfGGen::TStopReason TFfGGen::GenGraph(const int& GraphNodes, PGStatVec& EvolStat, const bool& FloodStop) {
  int GrowthStatNodes = 100;
  Graph = PNGraph::New();
  AddNodes(StartNodes);
  TStopReason SR = srUndef;
  while (Graph->GetNodes() < GraphNodes) {
    SR = AddNodes(GrowthStatNodes, FloodStop);
    if (SR != srOk) { return SR; }
    EvolStat->Add(Graph, TSecTm(Graph->GetNodes()));
    GrowthStatNodes = int(1.5*GrowthStatNodes);
  }
  return SR;
}

void TFfGGen::PlotFireSize(const TStr& FNmPref, const TStr& DescStr) {
  TGnuPlot GnuPlot("fs."+FNmPref, TStr::Fmt("%s. Fire size. G(%d, %d)",
    DescStr.CStr(), Graph->GetNodes(), Graph->GetEdges()));
  GnuPlot.SetXYLabel("Vertex id (iterations)", "Fire size (node out-degree)");
  TFltPrV IdToOutDegV;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    IdToOutDegV.Add(TFltPr(NI.GetId(), NI.GetOutDeg())); }
  IdToOutDegV.Sort();
  GnuPlot.AddPlot(IdToOutDegV, gpwImpulses, "Node out-degree");
  GnuPlot.SavePng();
}

void TFfGGen::GenFFGraphs(const double& FProb, const double& BProb, const TStr& FNm) {
  const int NRuns = 10;
  const int NNodes = 10000;
  TGStat::NDiamRuns = 10;
  //const double FProb = 0.35, BProb = 0.20;  // ff1
  //const double FProb = 0.37, BProb = 0.32;  // ff2
  //const double FProb = 0.37, BProb = 0.325; // ff22
  //const double FProb = 0.37, BProb = 0.33;  // ff3
  //const double FProb = 0.37, BProb = 0.35;  // ff4
  //const double FProb = 0.38, BProb = 0.35;  // ff5
  TVec<PGStatVec> GAtTmV;
  TFfGGen FF(false, 1, FProb, BProb, 1.0, 0, 0);
  for (int r = 0; r < NRuns; r++) {
    PGStatVec GV = TGStatVec::New(tmuNodes, TGStat::AllStat());
    FF.GenGraph(NNodes, GV, true);
    for (int i = 0; i < GV->Len(); i++) {
      if (i == GAtTmV.Len()) {
        GAtTmV.Add(TGStatVec::New(tmuNodes, TGStat::AllStat()));
      }
      GAtTmV[i]->Add(GV->At(i));
    }
    IAssert(GAtTmV.Len() == GV->Len());
  }
  PGStatVec AvgStat = TGStatVec::New(tmuNodes, TGStat::AllStat());
  for (int i = 0; i < GAtTmV.Len(); i++) {
    AvgStat->Add(GAtTmV[i]->GetAvgGStat(false));
  }
  AvgStat->PlotAllVsX(gsvNodes, FNm, TStr::Fmt("Forest Fire: F:%g  B:%g (%d runs)", FProb, BProb, NRuns));
  AvgStat->Last()->PlotAll(FNm, TStr::Fmt("Forest Fire: F:%g  B:%g (%d runs)", FProb, BProb, NRuns));
}

/*/////////////////////////////////////////////////
// Forest Fire Phase Transition
TFfPhaseTrans::TFfPhaseTrans(const int& NNds, const int& StartNds,  const double& Take2AmbPr,
                             const double& ProbOrphan, const double& ProbIncrement, const int& NRunsPerFB) :
  BurExpFire(false), NNodes(NNds), StartNodes(StartNds), NRuns(NRunsPerFB), Take2AmbProb(Take2AmbPr),
  OrphanProb(ProbOrphan), ProbInc(ProbIncrement), FBPrGSetH(), FBPrGStatH() {
  TakeStatSet = TFSet() | gsEffDiam;
}

PFfPhaseTrans TFfPhaseTrans::New(const int& NNds, const int& StartNds,  const double& Take2AmbPr,
                                 const double& ProbOrphan, const double& ProbIncrement, const int& NRunsPerFB) {
  return new TFfPhaseTrans(NNds, StartNds, Take2AmbPr, ProbOrphan, ProbIncrement, NRunsPerFB);
}

TFfPhaseTrans::TFfPhaseTrans(TSIn& SIn) : BurExpFire(SIn), NNodes(SIn), StartNodes(SIn), NRuns(SIn),
  Take2AmbProb(SIn), OrphanProb(SIn), ProbInc(SIn), FBPrGSetH(SIn), FBPrGStatH(SIn), TakeStatSet(SIn) {
}
PFfPhaseTrans TFfPhaseTrans::Load(TSIn& SIn) {
  return new TFfPhaseTrans(SIn);
}

void TFfPhaseTrans::Save(TFOut& SOut) const {
  BurExpFire.Save(SOut);  NNodes.Save(SOut);
  StartNodes.Save(SOut);  NRuns.Save(SOut);
  Take2AmbProb.Save(SOut);  OrphanProb.Save(SOut);
  ProbInc.Save(SOut);
  FBPrGSetH.Save(SOut);  FBPrGStatH.Save(SOut);
  TakeStatSet.Save(SOut);
}

TStr TFfPhaseTrans::GetTitleStr(const int& ValN) const {
  const PGrowthStat AvgStat = GetAvgGStat(ValN);
  const double AvgDeg = 2.0*AvgStat->Last()->Edges / double(AvgStat->Last()->Nodes);
  const double DiamCf = GetDecDiam(ValN).Val1;
  const TFltPr GplCf = GetGplCf(ValN);
  return TStr::Fmt("FWD: %.4f  BCK: %.4f  DIAM-INC: %.2f  GPL: %.2f (%.2f) AvgDeg:%.1f",
    GetFProb(ValN), GetBProb(ValN), DiamCf, GplCf.Val1(), GplCf.Val2(), AvgDeg);
}

TStr TFfPhaseTrans::GetFNm(const TStr& FNmPref) const {
  return TStr::Fmt("%s.n%dk.s%d", FNmPref.CStr(), int(NNodes/1000.0), StartNodes());
}

TFltPr TFfPhaseTrans::GetGplCf(const int& ValN) const {
  const TGrowthSet& GrowthSet = *GetGSet(ValN);
  TMom GplMom;
  for (int i = 0; i < GrowthSet.Len(); i++) {
    GplMom.Add(GrowthSet[i]->GetGplCf());
  }
  GplMom.Def();
  return TFltPr(GplMom.GetMean(), GplMom.GetSDev());
}

TFltPr TFfPhaseTrans::GetDecDiam(const int& ValN) const {
  const TGrowthSet& GrowthSet = *GetGSet(ValN);
  TMom DiamMom;
  for (int i = 0; i < GrowthSet.Len(); i++) {
    DiamMom.Add(GrowthSet[i]->IsDecEffDiam());
  }
  DiamMom.Def();
  return TFltPr(DiamMom.GetMean(), DiamMom.GetSDev());
}

TFltPr TFfPhaseTrans::GetEffDiam(const int& ValN, const int& AtTick) const {
  const TGrowthSet& GrowthSet = *GetGSet(ValN);
  TMom DiamMom;
  for (int i = 0; i < GrowthSet.Len(); i++) {
    if (AtTick != -1) { DiamMom.Add(GrowthSet[i]->At(AtTick)->EffDiam); }
    else { DiamMom.Add(GrowthSet[i]->Last()->EffDiam); }
  }
  DiamMom.Def();
  return TFltPr(DiamMom.GetMean(), DiamMom.GetSDev());
}

void TFfPhaseTrans::GetFProbV(TFltV& FProbV) const {
  THash<TFlt, TInt> FProbH;
  for (int i = 0; i < Len(); i++) {
    FProbH.AddKey(GetFProb(i)); }
  FProbH.GetKeyV(FProbV);
  FProbV.Sort();
}

TFltPr TFfPhaseTrans::RunForestFire(double FwdProb, double BckProb, const bool& Plot) {
  FwdProb = TMath::Round(FwdProb, 4);
  BckProb = TMath::Round(BckProb, 4);
  printf("\n---FwdBurnProb--%g----Back--%g------------------[%s]\n", FwdProb, BckProb, TExeTm::GetCurTm());
  if (FBPrGStatH.IsKey(TFltPr(FwdProb, BckProb))) {
    printf("*** SKIP -- already have it\n");
    const TGrowthStat& Stat = *FBPrGStatH.GetDat(TFltPr(FwdProb, BckProb));
    return TFltPr(Stat.GetGplCf(), Stat.IsDecEffDiam());
  }
  if (BckProb > 1.0) return TFltPr(-1, -1);
  PGrowthSet GrowthSet = TGrowthSet::New();
  TExeTm ExeTm;
  TFfGGen ForestFire(false, StartNodes, FwdProb, BckProb, 1.0, Take2AmbProb, OrphanProb);
  ForestFire.TakeStat(TakeStatSet); // gsDiam
  GrowthSet->Clr(false);
  for (int run = 0; run < NRuns; run++) {
    printf("\nRUN %d        [last run %s]\n", run+1, ExeTm.GetTmStr());  ExeTm.Tick();
    TFfGGen::TStopReason StopReason =*//* ForestFire.GenGraph(NNodes, true);
    if (! ForestFire.GetGrowthStat()->Empty()) {
      GrowthSet->Add(ForestFire.GetGrowthStat()); }
  }
  IAssert(! GrowthSet.Empty());
  // add stat
  FBPrGSetH.AddDat(TFltPr(FwdProb, BckProb), GrowthSet);
  PGrowthStat AvgStat = TGrowthStat::New();
  AvgStat->AvgGrowthStat(*GrowthSet);
  FBPrGStatH.AddDat(TFltPr(FwdProb, BckProb), AvgStat);
  const double DiamCf = LastDecDiam().Val1;
  const double GplCf = LastGplCf().Val1;
  // plot
  if (Plot && ! AvgStat.Empty()) {
    const TStr FNm = TStr::Fmt("F%04d.B%04d", int(FwdProb*10000), int(BckProb*10000));
    const TStr Title = GetTitleStr(Len()-1);
    AvgStat->PlotDiam(FNm, Title);
    AvgStat->PlotGpl(FNm, Title, true, false, false, false, false);
  }
  return TFltPr(GplCf, DiamCf);
}

void TFfPhaseTrans::FwdProbSteps(const double& MinFwdProb, const double& MaxFwdProb, const double& BckProb) {
  const TStr BinFNm = TFile::GetUniqueFNm(TStr::Fmt("phaseFwd.n%dk.s%d_#.bin", int(NNodes/1000.0), StartNodes()));
  TFfGGen::TimeLimitSec = 10*60;
  for (double FwdProb = MinFwdProb; FwdProb <= MaxFwdProb; FwdProb += ProbInc) {
    RunForestFire(FwdProb, BckProb, true);
    { TFOut FOut(BinFNm);
    Save(FOut); }
  }
}

void TFfPhaseTrans::FwdProbStepsFact(const double& MinFwdProb, const double& MaxFwdProb, const double& BckFact) {
  const TStr BinFNm = TFile::GetUniqueFNm(TStr::Fmt("phaseFwd.n%dk.s%d_#.bin", int(NNodes/1000.0), StartNodes()));
  TFfGGen::TimeLimitSec = 10*60;
  for (double FwdProb = MinFwdProb; FwdProb <= MaxFwdProb; FwdProb += ProbInc) {
    RunForestFire(FwdProb, BckFact*FwdProb, true);
    { TFOut FOut(BinFNm);
    Save(FOut); }
  }
}

void TFfPhaseTrans::FwdBckPhasePlot(const double& MinFwdProb, const double& MaxFwdProb, const double& MinBckFact,
                                    const double& MaxBckFact, const int& TimeLimitSec) {
  const TStr BinFNm = TFile::GetUniqueFNm(TStr::Fmt("phaseFF.n%dk.s%d_#.bin", int(NNodes/1000.0), StartNodes()));
  TFfGGen::TimeLimitSec = TimeLimitSec;
  for (double FwdProb = MinFwdProb; FwdProb <= MaxFwdProb; FwdProb += ProbInc) {
    for (double BckFact = MinBckFact; BckFact < MaxBckFact+0.001; BckFact += ProbInc) {
      const double BckProb = FwdProb * BckFact;
      RunForestFire(FwdProb, BckProb, true);
      { TFOut FOut(BinFNm);
      Save(FOut); }
    }
  }
}

void TFfPhaseTrans::FindGplPhaseTr(const double& StartFProb, const double& FollowGplCf, const TStr& FNmPref, const TStr& Desc) {
  const TStr FNm = TStr::Fmt("phGPL.%s", GetFNm(FNmPref).CStr());
  const double Tolerance = 0.01;
  const double MinDelta = 0.001;
  const bool Plot = false;
  TFfGGen::TimeLimitSec = 10*60;
  TGrowthStat::MinNodesEdges = 2*(StartNodes-1)+100;
  const int OldNDiamRuns = TGraphStat::NDiamRuns;
  TGraphStat::NDiamRuns = 1;  //!!! diameter
  TakeStat(TFSet() | gsEffDiam);
  FILE *F = fopen((FNm+".txt").CStr(), "wt");
  fprintf(F, "FollowGplCf:  %g\n", FollowGplCf);
  fprintf(F, "StartNodes:   %d\n", StartNodes());
  fprintf(F, "Take2AmbProb: %g\n", Take2AmbProb());
  fprintf(F, "OrphanProb:   %g\n", OrphanProb());
  fprintf(F, "Tolerance:    %g\n", Tolerance);
  double FwdProb = StartFProb, LBckRat=0, RBckRat=1, BckRat, GplCf;
  //TFltPr GplPr;
  while (FwdProb < 1.0) {
    while (true) {
      BckRat = (RBckRat+LBckRat) / 2;
      fprintf(F, "FWD: %g, (%f -- %f)", FwdProb, LBckRat, RBckRat);
      GplCf = RunForestFire(FwdProb, FwdProb*BckRat, Plot).Val1;
      IAssert(GplCf != -1);
      fprintf(F, "  %f  gpl: %.4f (%.4f)", BckRat, GplCf, LastGplCf().Val2());
      if (TMath::IsInEps(GplCf - FollowGplCf, Tolerance)) { fprintf(F, "  ***\n"); break; }
      if (RBckRat-LBckRat < MinDelta) { fprintf(F, "  gap\n"); break; }
      if (GplCf > FollowGplCf) { RBckRat = BckRat; } else { LBckRat = BckRat; }
      fprintf(F, "\n");  fflush(F);
    }
    FwdProb += ProbInc;
    RBckRat = BckRat+0.01;
    if (RBckRat > 1.0) RBckRat = 1.0;
    LBckRat = RBckRat - 0.2;
    if (LBckRat < 0.0) LBckRat = 0.0;
    { TFOut FOut(FNm+".bin");
     Save(FOut); }
    SaveGplPhaseTr(FollowGplCf, FNmPref, Desc);
    fprintf(F, "\n");
  }
  fclose(F);
  TGraphStat::NDiamRuns = OldNDiamRuns;
}

void TFfPhaseTrans::SaveGplPhaseTr(const double& FollowGplCf, const TStr& FNmPref, const TStr& Desc) {
  const double Tolerance = 0.02;
  THash<TFlt,  TIntFltPr> FProbH;
  for (int i = 0; i < Len(); i ++) {
    const double FProb = GetFProb(i);
    const double GplCf = GetGplCf(i).Val1;
    if (TMath::IsInEps(GplCf-FollowGplCf, Tolerance)) {
      if (! FProbH.IsKey(FProb)) {
        FProbH.AddDat(FProb, TIntFltPr(i, GplCf)); }
      else {
        const double bestCf = FProbH.GetDat(FProb).Val2;
        if (fabs(bestCf - FollowGplCf) > fabs(GplCf - FollowGplCf)) {
          FProbH.AddDat(FProb, TIntFltPr(i, GplCf)); }
      }
    }
  }
  TVec<TPair<TFlt, TIntFltPr> > FProbV;
  FProbH.GetKeyDatPrV(FProbV);  FProbV.Sort();
  const bool HasDiam = TakeStatSet.In(gsEffDiam);
  FILE *F = fopen(TStr::Fmt("phGPL.%s.tab", GetFNm(FNmPref).CStr()).CStr(), "wt");
  if (! Desc.Empty()) fprintf(F, "%s\n", Desc.CStr());
  fprintf(F, "FollowGplCf:  %g\n", FollowGplCf);
  fprintf(F, "StartNodes:   %d\n", StartNodes());
  fprintf(F, "Take2AmbProb: %g\n", Take2AmbProb());
  fprintf(F, "OrphanProb:   %g\n", OrphanProb());
  fprintf(F, "Tolerance:    %g\n", Tolerance);
  fprintf(F, "id\tFProb\tBProb\tBRatio\tGlp\tGlpDev");
  if (HasDiam) {  fprintf(F, "\tDiamCf\tDiamDev\tEffDiam"); }
  fprintf(F, "\n");
  for (int i = 0; i < FProbH.Len(); i++) {
    const int Id = FProbV[i].Val2.Val1;
    const TFltPr Gpl = GetGplCf(Id);
    fprintf(F, "%d\t%f\t%f\t%f\t", Id, GetFProb(Id), GetBProb(Id), GetBProb(Id)/GetFProb(Id));
    fprintf(F, "%f\t%f", Gpl.Val1(), Gpl.Val2());
    if (HasDiam) {
      const TFltPr DiamCf = GetDecDiam(Id);
      fprintf(F, "\t%f\t%f\t%f", DiamCf.Val1(), DiamCf.Val2(), GetEffDiam(Id, -1).Val1());
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

void TFfPhaseTrans::FindDiamPhaseTr(const double& StartFProb, const double& FollowDiamCf, const TStr& FNmPref, const TStr& Desc) {
  const TStr FNm = TStr::Fmt("phDIAM.%s", GetFNm(FNmPref).CStr());
  const double Tolerance = 0.01;
  const double MinDelta = 0.001;
  const bool Plot = false;
  TFfGGen::TimeLimitSec = 10*60;
  const int OldNDiamRuns = TGraphStat::NDiamRuns;
  TGraphStat::NDiamRuns = 1;
  TGrowthStat::MinNodesEdges = 2*(StartNodes-1)+100;
  TakeStat(TFSet() | gsEffDiam);
  FILE *F = fopen((FNm+".txt").CStr(), "wt");
  fprintf(F, "FollowDiamCf: %g\n", FollowDiamCf);
  fprintf(F, "StartNodes:   %d\n", StartNodes());
  fprintf(F, "Take2AmbProb: %g\n", Take2AmbProb());
  fprintf(F, "OrphanProb:   %g\n", OrphanProb());
  fprintf(F, "Tolerance:    %g\n", Tolerance);
  double FwdProb = StartFProb, LBckRat=0, RBckRat=1, BckRat, DiamCf;
  //TFltPr GplPr;
  while (FwdProb < 1.0) {
    while (true) {
      BckRat = (RBckRat+LBckRat) / 2;
      fprintf(F, "FWD: %g, (%f -- %f)", FwdProb, LBckRat, RBckRat);
      DiamCf = RunForestFire(FwdProb, FwdProb*BckRat, Plot).Val2;
      IAssert(DiamCf != -1);
      fprintf(F, "  %f  diam: %.4f (%.4f)", BckRat, DiamCf, LastDecDiam().Val2());
      if (TMath::IsInEps(DiamCf - FollowDiamCf, Tolerance)) { fprintf(F, "  ***\n"); break; }
      if (RBckRat-LBckRat < MinDelta) { fprintf(F, "  gap\n"); break; }
      if (DiamCf < FollowDiamCf) { RBckRat = BckRat; } else { LBckRat = BckRat; }
      fprintf(F, "\n");  fflush(F);
    }
    FwdProb += ProbInc;
    RBckRat = BckRat+0.05;
    if (RBckRat > 1.0) RBckRat = 1.0;
    LBckRat = RBckRat - 0.15;
    if (LBckRat < 0.0) LBckRat = 0.0;
    { TFOut FOut(FNm+".bin");
    Save(FOut); }
    SaveDiamPhaseTr(FollowDiamCf, FNmPref, Desc);
    fprintf(F, "\n");
  }
  fclose(F);
  TGraphStat::NDiamRuns = OldNDiamRuns;
}

void TFfPhaseTrans::SaveDiamPhaseTr(const double& FollowDiamCf, const TStr& FNmPref, const TStr& Desc) {
  const double Tolerance = 0.03;
  THash<TFlt,  TIntFltPr> FProbH;
  for (int i = 0; i < Len(); i ++) {
    const double FProb = GetFProb(i);
    const double DiamCf = GetDecDiam(i).Val1;
    if (TMath::IsInEps(DiamCf - FollowDiamCf, Tolerance)) {
      if (! FProbH.IsKey(FProb)) {
        FProbH.AddDat(FProb, TIntFltPr(i, DiamCf)); }
      else {
        const double bestCf = FProbH.GetDat(FProb).Val2;
        if (fabs(bestCf - FollowDiamCf) > fabs(DiamCf - FollowDiamCf)) {
          FProbH.AddDat(FProb, TIntFltPr(i, DiamCf)); }
      }
    }
  }
  TVec<TPair<TFlt, TIntFltPr> > FProbV;
  FProbH.GetKeyDatPrV(FProbV);  FProbV.Sort();
  FILE *F = fopen(TStr::Fmt("phDIAM.%s.tab", GetFNm(FNmPref).CStr()).CStr(), "wt");
  if (! Desc.Empty()) fprintf(F, "%s\n", Desc.CStr());
  fprintf(F, "FollowDiamCf: %g\n", FollowDiamCf);
  fprintf(F, "StartNodes:   %d\n", StartNodes());
  fprintf(F, "Take2AmbProb: %g\n", Take2AmbProb());
  fprintf(F, "OrphanProb:   %g\n", OrphanProb());
  fprintf(F, "Tolerance:    %g\n", Tolerance);
  fprintf(F, "id\tFProb\tBProb\tBRatio\tDiamCf\tDiamDev\tGplCf\tGplDev\tEffDiam\n");
  for (int i = 0; i < FProbV.Len(); i++) {
    const int Id = FProbV[i].Val2.Val1;
    const TFltPr DiamCf = GetDecDiam(Id);
    const TFltPr GplCf = GetGplCf(Id);
    const TFltPr EffDiam = GetEffDiam(Id, -1);
    fprintf(F, "%d\t%f\t%f\t%f\t", Id, GetFProb(Id), GetBProb(Id), GetBProb(Id)/GetFProb(Id));
    fprintf(F, "%f\t%f\t%f\t%f\t%f\n", DiamCf.Val1(), DiamCf.Val2(), GplCf.Val1(), GplCf.Val2(), EffDiam.Val1());
  }
  fclose(F);
}

void TFfPhaseTrans::Merge(const PFfPhaseTrans& FfPhaseTrans) {
  Merge(*FfPhaseTrans);
}

void TFfPhaseTrans::Merge(const TFfPhaseTrans& FfPhaseTrans) {
  printf("Merging:\n");
  printf("  source      %6d  (Fwd,Bck) pairs\n", FfPhaseTrans.Len());
  printf("  destination %6d  (Fwd,Bck) pairs\n", Len());
  IAssert(BurExpFire == FfPhaseTrans.BurExpFire);
  IAssert(NNodes == FfPhaseTrans.NNodes);
  IAssert(StartNodes == FfPhaseTrans.StartNodes);
  IAssert(Take2AmbProb == FfPhaseTrans.Take2AmbProb);
  IAssert(FBPrGSetH.Len() == FBPrGStatH.Len());
  IAssert(FfPhaseTrans.FBPrGSetH.Len() == FfPhaseTrans.FBPrGStatH.Len());
  for (int i1 = 0; i1 < FfPhaseTrans.FBPrGSetH.Len(); i1++) {
    IAssert(FfPhaseTrans.FBPrGSetH.GetKey(i1) == FfPhaseTrans.FBPrGStatH.GetKey(i1));
    const TFltPr& Key = FfPhaseTrans.FBPrGSetH.GetKey(i1);
    if (! FBPrGStatH.IsKey(Key)) {
      const PGrowthStat Stat = FfPhaseTrans.FBPrGStatH[i1];
      const PGrowthSet Set = FfPhaseTrans.FBPrGSetH[i1];
      FBPrGStatH.AddDat(Key, Stat);
      FBPrGSetH.AddDat(Key, Set);
    }
  }
  printf("  ** merged   %6d  (Fwd,Bck) pairs\n", Len());
}
//*/

/////////////////////////////////////////////////
// Undirected Forest Fire (does not densify!)

// Node selects N~geometric(1.0-BurnProb)-1 links and burns them. 
// geometirc(p) has mean 1/(p), so for given BurnProb, we burn 1/(1-BurnProb) links in average
int TUndirFFire::BurnGeoFire(const int& StartNId) {
  BurnedSet.Clr(false);
  BurningNIdV.Clr(false);  
  NewBurnedNIdV.Clr(false);
  AliveNIdV.Clr(false);
  const TUNGraph& G = *Graph;
  int NBurned = 1;
  BurnedSet.AddKey(StartNId);
  BurningNIdV.Add(StartNId);
  while (! BurningNIdV.Empty()) {
    for (int node = 0; node < BurningNIdV.Len(); node++) {
      const int& BurningNId = BurningNIdV[node];
      const TUNGraph::TNodeI& Node = G.GetNI(BurningNId);
      // find unburned links
      AliveNIdV.Clr(false); // unburned links
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        const int OutNId = Node.GetOutNId(e);
        if (! BurnedSet.IsKey(OutNId)) {
          AliveNIdV.Add(OutNId); }
      }
      // number of links to burn (geometric coin). Can also burn 0 links
      const int BurnNLinks = Rnd.GetGeoDev(1.0-BurnProb) - 1;
      if (! AliveNIdV.Empty() && BurnNLinks > 0) {
        AliveNIdV.Shuffle(Rnd);
        for (int i = 0; i < TMath::Mn(BurnNLinks, AliveNIdV.Len()); i++) {
          BurnedSet.AddKey(AliveNIdV[i]);
          NewBurnedNIdV.Add(AliveNIdV[i]);
          NBurned++;
        }
      }
    }
    BurningNIdV.Swap(NewBurnedNIdV);   // each node is burning just 1 time step
    // BurningNIdV.AddV(NewBurnedNIdV);   // all nodes are burning eternally
    NewBurnedNIdV.Clr(false);
  }
  IAssert(BurnedSet.Len() == NBurned);
  return NBurned;
}

TFfGGen::TStopReason TUndirFFire::AddNodes(const int& GraphNodes, const bool& FloodStop) {
  printf("\n***Undirected GEO ForestFire: graph(%d,%d) add %d nodes, burn prob %.3f\n", 
    Graph->GetNodes(), Graph->GetEdges(), GraphNodes, BurnProb);
  TExeTm ExeTm;
  int Burned1=0, Burned2=0, Burned3=0; // last 3 fire sizes
  TIntPrV NodesEdgesV;
  // create initial set of nodes
  if (Graph.Empty()) { Graph = PUNGraph::New(); }
  if (Graph->GetNodes() == 0) { Graph->AddNode(); }
  int NEdges = Graph->GetEdges();
  // forest fire
  for (int NNodes = Graph->GetNodes()+1; NNodes <= GraphNodes; NNodes++) {
    const int NewNId = Graph->AddNode(-1);
    IAssert(NewNId == Graph->GetNodes()-1); // node ids have to be 0...N
    const int StartNId = Rnd.GetUniDevInt(NewNId);
    const int NBurned = BurnGeoFire(StartNId);
    // add edges to burned nodes
    for (int e = 0; e < NBurned; e++) {
      Graph->AddEdge(NewNId, GetBurnedNId(e)); }
    NEdges += NBurned;
    Burned1=Burned2;  Burned2=Burned3;  Burned3=NBurned;
    if (NNodes % Kilo(1) == 0) {
      printf("(%d, %d)    burned: [%d,%d,%d]  [%s]\n", NNodes, NEdges, Burned1, Burned2, Burned3, ExeTm.GetStr()); 
      NodesEdgesV.Add(TIntPr(NNodes, NEdges));
    }
    if (FloodStop && NEdges>1000 && NEdges/double(NNodes)>100.0) { // average node degree is more than 50
      printf("!!! FLOOD. G(%6d, %6d)\n", NNodes, NEdges);  return TFfGGen::srFlood; }
  }
  printf("\n");
  IAssert(Graph->GetEdges() == NEdges);
  return TFfGGen::srOk;
}
