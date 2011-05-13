#include "stdafx.h"
#include "ning.h"

/////////////////////////////////////////////////
// Ning User Base
void TNingUsrBs::ParseUsers(const TStr& InFNmWc, const TStr& PlotOutFNm) {
  //for (int dir = 0; dir < TNingEventStat::MxEventId; dir++) {
  //  const TNingEventStat::TEventId Event = (TNingEventStat::TEventId) dir;
  //  printf("Event %s...", TNingEventStat::GetStr(Event).CStr());
  //  TJsonLoader J(NingPath+TNingEventStat::GetStr(Event)+"\\*.gz");
  THash<TInt,TInt> AppEventsH;
  THash<TInt,TInt> UsrEventsH;
  THashSet<TIntPr> AppUsrH;
  int EvCnt=0, NoAppId=0, NoAuth=0;
  TExeTm ExeTm;
  TJsonLoader J(InFNmWc);
  while (J.Next()) {
    if (! J.IsKey("appId")) { NoAppId++; continue; }
    if (! J.IsKey("author")) { NoAuth++; continue; }
    const int AppId = atoi(J.GetDat("appId").CStr());
    const int UId = AddUId(J.GetDat("author"));
    AppUsrH.AddKey(TIntPr(AppId, UId));
    AppEventsH.AddDat(AppId)++;
    UsrEventsH.AddDat(UId)++;
    if (J.IsKey("attachedToAuthor")) { 
      const int UId2 = AddUId(J.GetDat("attachedToAuthor"));
      AppUsrH.AddKey(TIntPr(AppId, UId2));
      UsrEventsH.AddDat(UId2)++;
    }
    EvCnt++;
  }
  printf("Loading done. Info:\n");
  printf("  events %d, no-appId %d, no-auth %d [%s]\n", EvCnt, NoAppId, NoAuth, ExeTm.GetStr());
  printf("  %d users (in all apps)\n", UsrEventsH.Len());
  printf("  %d different apps\n", AppEventsH.Len());
  printf("  %d unique user-app pairs\n", AppUsrH.Len());
  // create plots
  if (! PlotOutFNm.Empty()) {
    { TIntH ValCntH;
    for (int i = 0; i < AppEventsH.Len(); i++) { ValCntH.AddDat(AppEventsH[i])++; }
    TGnuPlot::PlotValCntH(ValCntH, PlotOutFNm+"-appEvents", "Ning App number of events distribution", "Number of events in the app", "Number of such apps", gpsLog); }
    { TIntH ValCntH;
    for (int i = 0; i < UsrEventsH.Len(); i++) { ValCntH.AddDat(UsrEventsH[i])++; }
    TGnuPlot::PlotValCntH(ValCntH, PlotOutFNm+"-usrEvents", "Number of events of a user (over all apps)", "Number of events of a user", "Number of such users", gpsLog); }
    { TIntH UsrAppCntH, UsrAppDistr;
    for (int i = 0; i < AppUsrH.Len(); i++) { UsrAppCntH.AddDat(AppUsrH[i].Val2)++; }
    for (int i = 0; i < UsrAppCntH.Len(); i++) { UsrAppDistr.AddDat(UsrAppCntH[i])++; }
    TGnuPlot::PlotValCntH(UsrAppDistr, PlotOutFNm+"-usrApps", "Number of apps of a user", "Number of apps a user is memeber of", "Number of such users", gpsLog); }
    { TIntH AppUsrCntH, AppSzDistr;
    for (int i = 0; i < AppUsrH.Len(); i++) { AppUsrCntH.AddDat(AppUsrH[i].Val1)++; }
    for (int i = 0; i < AppUsrCntH.Len(); i++) { AppSzDistr.AddDat(AppUsrCntH[i])++; }
    TGnuPlot::PlotValCntH(AppSzDistr, PlotOutFNm+"-usrSz", "Number of users in the app", "Number of users in an app", "Number of such apps", gpsLog); }  
  }
}

/////////////////////////////////////////////////
// Ning network
const int TNingNet::MnTm = 1170958982; // Feb 8 2007
const int TNingNet::MxTm = 1278405972;

TStr TNingNet::GetTitle() const {
  return TStr::Fmt("NingNet (%d, %d, %d). Age:%d Dead:%d.", GetNodes(), GetUniqEdges(), GetEdges(), 
    GetAge(tmuDay), GetDeadTm(tmuDay));
}

PUNGraph TNingNet::GetUNGraphUpToTm(const TSecTm& MaxEdgeTm) const {
  PUNGraph NewNetPt = TUNGraph::New();
  TUNGraph& NewNet = *NewNetPt;
  TSecTm PrevTm;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI() > MaxEdgeTm) { break; }
    if (! NewNet.IsNode(EI.GetSrcNId()))
      NewNet.AddNode(EI.GetSrcNId());
    if (! NewNet.IsNode(EI.GetDstNId()))
      NewNet.AddNode(EI.GetDstNId());
    NewNet.AddEdge(EI.GetSrcNId(), EI.GetDstNId());
    IAssert(! PrevTm.IsDef() || PrevTm <= EI()); // edge times must be sorted
    PrevTm = EI();
  }
  return NewNetPt;
}

PUNGraph TNingNet::GetSubGraph(const TNingGroup& Group, const int& TakeFirstK, const TSecTm& MxEdgeTm) const {
  PUNGraph NewNetPt = TUNGraph::New();
  TUNGraph& NewNet = *NewNetPt;
  int node, edge;
  TNodeI NI;
  IAssert(TakeFirstK <= Group.Len());
  if (TakeFirstK != -1) {
    for (node = 0; node < TakeFirstK; node++) {
      if (IsNode(Group.GetNId(node))) {
        NewNet.AddNode(Group.GetNId(node)); }
    }
    for (node = 0; node < TakeFirstK; node++) {
      const int NId = Group.GetNId(node);
      if (! IsNode(NId)) { continue; }
      NI = GetNI(NId);
      for (edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        const TSecTm OutETm = NI.GetOutEDat(edge);
        if (NewNet.IsNode(OutNId) && OutETm <= MxEdgeTm) { 
          NewNet.AddEdge(NId, OutNId); 
        }
      }
    }
  } else {
    for (node = 0; node < Group.Len(); node++) {
      if (IsNode(Group.GetNId(node)) && Group.GetTm2(node) <= MxEdgeTm) {
        NewNet.AddNode(Group.GetNId(node)); }
    }
    for (node = 0; node < Group.Len(); node++) {
      const int NId = Group.GetNId(node);
      if (! NewNet.IsNode(NId)) { continue; }
      NI = GetNI(NId);
      for (edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        const TSecTm OutETm = NI.GetOutEDat(edge);
        if (NewNet.IsNode(OutNId) && OutETm <= MxEdgeTm) { 
          NewNet.AddEdge(NId, OutNId); }
      }
    }
  }
  return NewNetPt;
}

void TNingNet::GetNodesOverTm(const TTmUnit TmUnit, const int& TmSteps, TFltV& NodesTmV, const bool Cummulative, const bool& Relative) {
  NodesTmV.Gen(TmSteps+1);
  NodesTmV.PutAll(0);
  const TSecTm StartTm = GetMnTm();
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int NodeAge = TSecTm(NI()-StartTm).GetInUnits(TmUnit);
    if (NodeAge >= NodesTmV.Len()) { break; }
    NodesTmV[NodeAge]++;
  }
  if(Cummulative) {
    for (int i = 1; i < NodesTmV.Len(); i++) { NodesTmV[i] += NodesTmV[i-1]; } }
  if (Relative) {
    for (int i = 0; i < NodesTmV.Len(); i++) { NodesTmV[i] /= double(GetNodes()); } }
}

void TNingNet::PlotOverTime(const TStr& OutFNmPref) const {
  TIntFltH NodesH, EdgesH, EventsH;
  THash<TIntPr, TSecTm> EdgeTmH(GetEdges());
  TIntSet NodeSet(GetNodes());
  // events
  for (TEdgeI EI=BegEI(); EI < EndEI(); EI++) {
    EventsH.AddDat(TSecTm(EI()-GetMnTm()).GetInUnits(tmuDay))++;
    if (! EdgeTmH.IsKey(TIntPr(EI.GetSrcNId(), EI.GetDstNId()))) {
      EdgeTmH.AddDat(TIntPr(EI.GetSrcNId(), EI.GetDstNId()), EI());
    }
    if (! NodeSet.IsKey(EI.GetSrcNId())) {
      NodeSet.AddKey(EI.GetSrcNId());
      NodesH.AddDat(TSecTm(EI()-GetMnTm()).GetInUnits(tmuDay))++;
    }
    if (! NodeSet.IsKey(EI.GetDstNId())) {
      NodeSet.AddKey(EI.GetDstNId());
      NodesH.AddDat(TSecTm(EI()-GetMnTm()).GetInUnits(tmuDay))++;
    }
  }
  for (int i = 0; i < EdgeTmH.Len(); i++) {
    EdgesH.AddDat(TSecTm(EdgeTmH[i]-GetMnTm()).GetInUnits(tmuDay))++;
  }
  // cummulative and relative
  for (int i = 1; i < NodesH.Len(); i++) { NodesH[i] += NodesH[i-1]; }
  for (int i = 1; i < EdgesH.Len(); i++) { EdgesH[i] += EdgesH[i-1]; }
  for (int i = 1; i < EventsH.Len(); i++) { EventsH[i] += EventsH[i-1]; }
  //for (int i = 0; i < NodesH.Len(); i++) { NodesH[i] /= NodesH[NodesH.Len()-1]; }
  //for (int i = 0; i < EdgesH.Len(); i++) { EdgesH[i] /= EdgesH[EdgesH.Len()-1]; }
  //for (int i = 0; i < EventsH.Len(); i++) { EventsH[i] /= EventsH[EventsH.Len()-1]; }
  TGnuPlot GP(OutFNmPref+"-OverTm", GetTitle()+" Nodes, Edges and Events over time.");
  GP.AddPlot(NodesH, gpwLinesPoints, "Nodes");
  GP.AddPlot(EdgesH, gpwLinesPoints, "Edges");
  GP.AddPlot(EventsH, gpwLinesPoints, "Events");
  GP.SavePng();
}

double TNingNet::GetL2Dist(const TFltV& V1, const TFltV& V2) {
  double Dist = 0.0;
  for (int i = 0; i < TMath::Mn(V1.Len(), V2.Len()); i++) {
    Dist += TMath::Sqr(V1[i] - V2[i]); }
  return TMath::Sqrt(Dist);
}

/////////////////////////////////////////////////
// Ning network base
void TNingNetBs::ParseNetworks(const TStr& InFNmWc, TNingUsrBs& UsrBs, const TStr& LinkTy) {
  printf("\nLinkType:: %s\n", LinkTy.CStr()); TExeTm ExeTm;
  IAssert(LinkTy=="eventattendee" || LinkTy=="friendrequestmessage" || LinkTy=="groupinvitationrequest" || LinkTy=="comment");
  TChA SrcKey, DstKey;
  if (LinkTy=="eventattendee") {
    SrcKey="inviter";  DstKey="author";
  } else if (LinkTy=="friendrequestmessage") {
    SrcKey="author";  DstKey="recipient";
  } else if (LinkTy=="groupinvitationrequest") {
    SrcKey="author";  DstKey="requestor";
  } else if (LinkTy=="comment") {
    SrcKey="attachedToAuthor";  DstKey="author";
  }
  TJsonLoader J(InFNmWc);
  int MultiEdgeCnt=0, BadUsr=0, TmNotDef=0;
  while (J.Next()) {
    EAssertR(J.IsKey("appId") && J.IsKey(SrcKey) && J.IsKey(DstKey), TStr::Fmt("Bad JSON object in %s line %d.", J.GetCurFNm(), J.GetLineNo()));
    const int AppId = atoi(J.GetDat("appId").CStr());
    const int SrcNId = UsrBs.AddUId(J.GetDat(SrcKey));
    const int DstNId = UsrBs.AddUId(J.GetDat(DstKey));
    if (SrcNId==-1 || DstNId==-1) { 
      //printf("Unknown User:\n  %s: %d\n  %s: %d\n",  J.GetDat(SrcKey).CStr(), SrcNId, J.GetDat(DstKey).CStr(), DstNId); 
      BadUsr++; continue; }
    if (SrcNId == DstNId) { continue; } // skip self edges
    // NingNet
    const TSecTm Tm = TSecTm::GetDtTmFromStr(J.GetDat("createdDate")); //"createdDate": "2009-10-06 07:48:08.287",
    if (! Tm.IsDef()) { TmNotDef++; continue; }
    PNingNet& G = AppNetH.AddDat(AppId);
    if (G.Empty()) { G = TNingNet::New(); }
    if (! G->IsNode(SrcNId)) { G->AddNode(SrcNId, Tm); }
    if (! G->IsNode(DstNId)) { G->AddNode(DstNId, Tm); }
    G->AddEdge(SrcNId, DstNId, -1, Tm);
  }
  printf("\nLoading done.\n");
  printf("  %d  authors\n", UsrBs.Len());
  printf("  %d  networks\n", AppNetH.Len());
  printf("  %d  bad-users\n", BadUsr);
  printf("  %d  events without time\n", TmNotDef++);
  printf("Sorting...");
  int Nodes=0, Edges=0;
  for (int i=0; i<AppNetH.Len(); i++) {
    IAssert(! AppNetH[i].Empty());
    AppNetH[i]->SortNIdByDat(true); // sort nodes by time
    AppNetH[i]->SortEIdByDat(true); // sort edges by time
    Nodes += AppNetH[i]->GetNodes();
    Edges += AppNetH[i]->GetEdges(); 
  }
  printf("  %d  total nodes in all nets\n", Nodes);
  printf("  %d  total edges in all nets\n", Edges);
  printf("done [%s]\n", ExeTm.GetStr());
}

PNingNetBs TNingNetBs::GetSubBs(const TIntV& AppIdV) const {
  PNingNetBs NewBs = TNingNetBs::New();
  NewBs->AppNetH.Gen(AppIdV.Len());
  for (int i = 0; i < AppIdV.Len(); i++) {
    const int AppId = AppIdV[i];
    NewBs->AppNetH.AddDat(AppId, GetNetId(AppId));
  }
  return NewBs;
}

PNingNetBs TNingNetBs::GetSubBs(const int& MinNodes, const int& MinAge, const int& MinDeadTm) const {
  TIntV AppIdV;
  for (int i = 0; i < Len(); i++) {
    const int AppId = GetAppId(i);
    PNingNet Net = GetNetId(AppId);
    if (Net->GetNodes() >= MinNodes && Net->GetAge(tmuDay) >= MinAge && Net->GetDeadTm(tmuDay) >= MinDeadTm) {
      AppIdV.Add(AppId); }
  }
  return GetSubBs(AppIdV);
}

void TNingNetBs::SaveTxtStat(const TStr& OutFNm, const int& MnSz, const int& MxSz) const {
  TIntPrV SzAIdV(Len(), 0);
  for (int i=0; i < Len(); i++) {
    SzAIdV.Add(TIntPr(GetNet(i)->GetNodes(), i)); }
  //SzAIdV.Sort(false);
  SzAIdV.Shuffle(TInt::Rnd);
  FILE *StatF = fopen(TStr("GrowthStat-"+OutFNm+".tab").CStr(), "wt");
  fprintf(StatF, "AppId\tAge\tDeadTm\tNodes\tEdges\tUniqEdges\tCmtPerEdge");
  fprintf(StatF, "\tAvgDeg\tFracDeg1Nodes\tFracOverAvg");
  fprintf(StatF, "\tTriadEdges\tCccf\tNWccs\tWccSz\tEigVal\tEigValRat\tEffDiam\n");
  for (int i=0; i<Len(); i++) {
    PNingNet Net = GetNet(SzAIdV[i].Val2);
    if (Net->GetNodes() < MnSz || Net->GetNodes() > MxSz) { continue; }
    printf("%d,%d\t", Net->GetNodes(), Net->GetEdges());
    fprintf(StatF, "%d\t%d\t%d\t%d\t%d", GetAppId(SzAIdV[i].Val2), Net->GetAge(tmuDay), Net->GetDeadTm(tmuDay), Net->GetNodes(), Net->GetEdges());
    PUNGraph G = TSnap::ConvertGraph<PUNGraph>(Net, true);
    //PNGraph G = TSnap::ConvertGraph<PNGraph>(Net, true);
    const int GEdges = G->GetEdges();
    const double CmtPerEdge = Net->GetEdges() / double(GEdges);
    fprintf(StatF, "\t%d\t%f", GEdges, CmtPerEdge);
    const double AvgDeg = 2*GEdges / double(G->GetNodes());
    int CntDeg1=0, CntDegOverAvg=0;
    for (TUNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      if (NI.GetDeg() == 1) { CntDeg1++; }
      if (NI.GetDeg() > AvgDeg) { CntDegOverAvg++; }
    }
    fprintf(StatF, "\t%f\t%f\t%f", AvgDeg, CntDeg1/double(G->GetNodes()), CntDegOverAvg/double(G->GetNodes()));
    const int TriadEdges = TSnap::GetTriadEdges(G);
    const double FullCcf = TSnap::GetClustCf(G);
    TCnComV CnComV;  TSnap::GetWccs(G, CnComV);
    const double FullWcc = TSnap::GetMxWccSz(G);
    const int NWccs = CnComV.Len();
    TFltV EigValV;   TSnap::GetEigVals(G, 2, EigValV);
    const double EigVal = abs(EigValV[0]);
    const double EigValRat = abs(EigValV[0])/abs(EigValV[1]);
    const double EffDiam = TSnap::GetBfsEffDiam(G, 100);
    fprintf(StatF, "\t%d\t%f\t%d\t%f\t%f\t%f\t%f", TriadEdges, FullCcf, NWccs, FullWcc, EigVal, EigValRat, EffDiam);
    fprintf(StatF, "\n");
    //G->PlotGrowthStat(OutFNm, StatF);
  }
  fclose(StatF);
}

void TNingNetBs::PlotDeadStat(const TStr& OutFNmPref) const {
  TSecTm MxTm, MnTm;
  TIntH CommentOverTmH;
  for (int n = 0; n < Len(); n++) {
    PNingNet Net = GetNet(n);
    if (! MxTm.IsDef() || MxTm < Net->GetMxTm()) { MxTm=Net->GetMxTm(); }
    if (! MnTm.IsDef() || MnTm > Net->GetMnTm()) { MnTm=Net->GetMnTm(); }
    for (TNingNet::TEdgeI EI = Net->BegEI(); EI <  Net->EndEI(); EI++) {
      CommentOverTmH.AddDat(TSecTm(EI.GetDat()-TNingNet::MnTm).GetInUnits(tmuDay))++; }
  }
  printf("Start of time = %s %d\n", MnTm.GetStr().CStr(), MnTm.GetAbsSecs());
  printf("End of time = %s %d\n", MxTm.GetStr().CStr(), MxTm.GetAbsSecs());
  TGnuPlot::PlotValCntH(CommentOverTmH, OutFNmPref+"-overTime", "Number of comments over time", 
    "Time [days since Feb 8 2007]", "Number of comments", gpsLog10Y);
  TIntH SzDeadH, DeadCntH, DeadCnt10H;
  TIntH NodesH, EdgesH, UniqEdgesH, AgeCntH;
  THash<TInt, TMom> SzDeadMomH;
  int Month1Dead=0, Month3Dead=0, Month6Dead=0;
  for (int n = 0; n < Len(); n++) {
    PNingNet Net = GetNet(n);
    const int DeadTm = TSecTm(MxTm-Net->GetMxTm()).GetInUnits(tmuDay);
    const int Age = TSecTm(Net->GetMxTm()-Net->GetMnTm()).GetInUnits(tmuDay);
    AgeCntH.AddDat(Age)++;
    SzDeadH.AddDat(Net->GetNodes(), DeadTm);
    SzDeadMomH.AddDat(Net->GetNodes()).Add(DeadTm);
    if (DeadTm>30) { Month1Dead++; }
    if (DeadTm>90) { Month3Dead++; }
    if (DeadTm>180) { Month6Dead++; }
    DeadCntH.AddDat(DeadTm)++;
    if (Net->GetNodes()>=10) { 
      DeadCnt10H.AddDat(DeadTm)++; }
    NodesH.AddDat(Net->GetNodes())++;
    EdgesH.AddDat(Net->GetEdges())++;
    UniqEdgesH.AddDat(Net->GetUniqEdges())++;
  }
  const int Nets = Len();
  printf("%d\tNetworks dead more than 1 month \tAlive\t%d\n", Month1Dead, Nets-Month1Dead);
  printf("%d\tNetworks dead more than 3 months\tAlive\t%d\n", Month3Dead, Nets-Month3Dead);
  printf("%d\tNetworks dead more than 6 months\tAlive\t%d\n", Month6Dead, Nets-Month6Dead);
  TGnuPlot::PlotValCntH(AgeCntH, OutFNmPref+"-AgeTime", "Number of days alive", "Number of days network is alive", "Number of such networks", gpsLog);
  TGnuPlot::PlotValCntH(DeadCntH, OutFNmPref+"-DeadTime", "Number of days dead", "Number of days network is dead", "Number of such networks", gpsLog);
  TGnuPlot::PlotValCntH(DeadCnt10H, OutFNmPref+"-DeadTime10", "Number of days dead. Only nets with at least 10 nodes.", "Number of days network is dead", "Number of such networks", gpsLog);
  TGnuPlot::PlotValCntH(NodesH, OutFNmPref+"-Nodes", "Network size distribution", "Number of nodes", "Number of networks", gpsLog);
  TGnuPlot::PlotValCntH(EdgesH, OutFNmPref+"-Comments", "Network number of comments sdistribution", "Number of comments", "Number of networks", gpsLog);
  TGnuPlot::PlotValCntH(UniqEdgesH, OutFNmPref+"-Edges", "Network number of edges distribution", "Number of edges", "Number of networks", gpsLog);
  { TGnuPlot GP(OutFNmPref+"-Size_DeadTime1", "Number of days the network has been dead (no commending activity)");
  GP.AddPlot(SzDeadH, gpwPoints, "Dead time");
  GP.AddPlot(SzDeadMomH, gpwLinesPoints, "Dead time", "", true, true, true, true, false, false);
  GP.SetXYLabel("network size", "number of days the network is dead");
  GP.SavePng(); }
  { TGnuPlot GP(OutFNmPref+"-Size_DeadTime2", "Number of days the network has been dead (no commending activity)");
  GP.AddPlot(SzDeadH, gpwPoints, "Dead time");
  GP.AddPlot(SzDeadMomH, gpwLinesPoints, "Dead time", "", true, true, true, true, false, false, true);
  GP.SetXYLabel("network size", "number of days the network is dead");
  GP.SavePng(); }
}

void TNingNetBs::PlotSimNodesEvol(const int& NetId, const int& PlotN, const int& TmSteps, const int& MinAge, const int& MinNodes) const {
  PNingNet Net = GetNet(NetId);
  TFltV EvolV, Evol2V;  
  Net->GetNodesOverTm(tmuDay, TmSteps, EvolV, true);
  TIntFltH IdSimH;
  IdSimH.AddDat(NetId, 0);
  printf("Calculating similarities...");
  for (int n = 0; n < Len(); n++) {
    PNingNet Net2 = GetNet(n);
    if (n == NetId || Net2->GetAge(tmuDay)<MinAge || Net2->GetNodes()<MinNodes) { continue; }
    Net2->GetNodesOverTm(tmuDay, TmSteps, Evol2V, true);
    const double Dist = TNingNet::GetL2Dist(EvolV, Evol2V);
    IdSimH.AddDat(n, Dist);
  }
  printf("Done. %d/%d networks\n", IdSimH.Len(), Len());
  IdSimH.SortByDat(true);
  TGnuPlot GP(TStr::Fmt("overTime-%d", NetId), "Evolution of networks");
  GP.SetXYLabel("Time [days]", "Number of nodes");
  for (int i = 0; i < PlotN; i++) {
    PNingNet Net = GetNet(IdSimH.GetKey(i));
    Net->GetNodesOverTm(tmuDay, Net->GetAge(tmuDay), EvolV, true);
    GP.AddPlot(EvolV, gpwLines, Net->GetTitle());
  }
  GP.SavePng();
}

//////////////////////////////////////////////////
// Ning groups
void TNingGroup::AddUsr(const int& UId, const TSecTm& JoinTm) {
  if (! NIdJoinTmH.IsKey(UId) || NIdJoinTmH.GetDat(UId) > JoinTm) {
    NIdJoinTmH.AddDat(UId, JoinTm);
  }
}

void TNingGroupBs::ParseGroups(const TStr& InFNmWc, const TNingUsrBs& UsrBs) {
  TJsonLoader J(InFNmWc);
  TStrHash<TInt> GIdH; // group id to index
  while (J.Next()) {
    const int AppId = atoi(J.GetDat("appId").CStr());
    const int UId = UsrBs.GetUId(J.GetDat("author"));
    if (UId == -1) { continue; } // unknown user
    const TSecTm Tm = TSecTm::GetDtTmFromStr(J.GetDat("createdDate")); //"createdDate": "2009-10-06 07:48:08.287",
    TInt& GId = GIdH.AddDat(J.GetDat("groupId"));
    TNingGroupV& GroupV = GroupsH.AddDat(AppId);
    if (GId == 0) { // new group
      GroupV.Add();
      GId = GroupV.Len()-1;
    }
    IAssert(GId < GroupV.Len());
    GroupV[GId].AddUsr(UId, Tm);
  }
  for (int a = 0; a < GroupsH.Len(); a++) {
    TNingGroupV& GV = GroupsH[a];
    for (int g = 0; g < GV.Len(); g++) {
      GV[g].SortByTm(); }
  }
}

PNingGroupBs TNingGroupBs::GetSubBs(const PNingNetBs& NetBs, const int& MinSz, const double& MaxFracSz, const int& MinAge) {
  PNingGroupBs NewBs = TNingGroupBs::New();
  for (int n = 0; n < Len(); n++) {
    const int AppId = GroupsH.GetKey(n);
    const TNingGroupV& GroupV = GroupsH[n];
    if (! NetBs->IsNet(AppId)) { continue; }
    const double AppNodes = NetBs->GetNetId(AppId)->GetNodes();
    TNingGroupV NewGroupV;
    for (int g = 0; g < GroupV.Len(); g++) {
      const TNingGroup& Gr = GroupV[g];
      if (Gr.Len() >= MinSz && Gr.Len()/AppNodes <= MaxFracSz && Gr.GetAge(tmuDay) >= MinAge) {
        NewGroupV.Add(GroupV[g]); }
    }
    if (NewGroupV.Len() > 0) {
      NewBs->GroupsH.AddDat(AppId, NewGroupV);
    }
  }
  return NewBs;
}

void TNingGroupBs::PlotGroupStat(const TStr& OutFNm) const {
  TIntH GroupSzH, NGroupsH;
  for (int i = 0; i < Len(); i++) {
    const TNingGroupV& GV = GroupsH[i];
    NGroupsH.AddDat(GV.Len())++;
    for (int j = 0; j < GV.Len(); j++) {
      GroupSzH.AddDat(GV[j].Len())++; }
  }
  TGnuPlot::PlotValCntH(GroupSzH, OutFNm+"-GroupSz", "Group Size distribution", "Group Size", "Number of such groups", gpsLog);
  TGnuPlot::PlotValCntH(NGroupsH, OutFNm+"-NGroups", "Number of groups distribution", "Number of groups per network", "Number of such networks", gpsLog);
}

/////////////////////////////////////////////////
// Ning group evolution
void TNingGroupEvol::AddNet(const PNingNet& Net, const TNingGroupV& GroupV) {
  static int NJoin = 0;
  THash<TIntPr, TSecTm> EdgeTmSet(Net->GetEdges());
  THash<TSecTm, TIntPrV> TmJoinVH;
  THash<TInt, TIntSet> GroupSet;
  THash<TInt, TIntH> NIdInEH;
  THash<TInt, TIntV> NIdGroupV;
  int TmJoinIdx = 0;
  PUNGraph G = TUNGraph::New(Net->GetNodes(), -1);
  // preprocess edges
  for (TNingNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    const TIntPr Edge(TMath::Mn(EI.GetSrcNId(), EI.GetDstNId()), TMath::Mx(EI.GetSrcNId(), EI.GetDstNId()));
    if (EdgeTmSet.IsKey(Edge)) { continue; }
    EdgeTmSet.AddDat(Edge, EI());
  }
  // find node group membership times
  for (int g = 0; g < GroupV.Len(); g++) {
    for (int i = 0; i < GroupV[g].Len(); i++) {
      TmJoinVH.AddDat(GroupV[g].GetTm2(i)).Add(TIntPr(g, GroupV[g].GetNId(i))); } // (group, node)
  }
  TmJoinVH.SortByKey(true);
  // evolve the network
  for (int i = 0; i < EdgeTmSet.Len(); i++) {
    const TIntPr Edge = EdgeTmSet.GetKey(i);
    const TSecTm EdgeTm = EdgeTmSet[i];
    // add edge to net
    if (! G->IsNode(Edge.Val1)) { G->AddNode(Edge.Val1); }
    if (! G->IsNode(Edge.Val2)) { G->AddNode(Edge.Val2); }
    G->AddEdge(Edge.Val1, Edge.Val2);
    // count node group in-degree
    if (NIdGroupV.IsKey(Edge.Val1)) {
      const TIntV& GroupV = NIdGroupV.GetDat(Edge.Val1);
      for (int g = 0; g < GroupV.Len(); g++) {
        NIdInEH.AddDat(GroupV[g]).AddDat(Edge.Val2)++; }
    }
    if (NIdGroupV.IsKey(Edge.Val2)) {
      const TIntV& GroupV = NIdGroupV.GetDat(Edge.Val2);
      for (int g = 0; g < GroupV.Len(); g++) {
        NIdInEH.AddDat(GroupV[g]).AddDat(Edge.Val1)++; }
    }
    // nodes that joined any of the groups at current time
    for (; TmJoinIdx < TmJoinVH.Len() && TmJoinVH.GetKey(TmJoinIdx) < EdgeTm; TmJoinIdx++) {
      const TIntPrV& JoinV = TmJoinVH[TmJoinIdx];
      for (int j = 0; j < JoinV.Len(); j++) {
        GroupSet.AddDat(JoinV[j].Val1).AddKey(JoinV[j].Val2); 
        NIdGroupV.AddDat(JoinV[j].Val2).Add(JoinV[j].Val1);
      }
      for (int j = 0; j < JoinV.Len(); j++) {
        if (NIdInEH.IsKey(JoinV[j].Val1)) { 
          OnNodeJoined(G, GroupV[JoinV[j].Val1], GroupSet.GetDat(JoinV[j].Val1), JoinV[j].Val2, NIdInEH.GetDat(JoinV[j].Val1), EdgeTm); }
        if (++NJoin % Kilo(1) == 0) { printf("."); }
        if (NJoin % Mega(1) == 0) { PlotAll("ningCommentAll"); }
      }
    }
  }
}

// gets called whenever a new node JoinNId joins the group when the network has structure G
void TNingGroupEvol::OnNodeJoined(const PUNGraph& G, const TNingGroup& EndGroup, const TIntSet& CurGroup, const int& JoinNId, const TIntH& NIdInEH, const TSecTm& JoinTm) {
  if (! NIdInEH.IsKey(JoinNId)) { return; }
  // prob. of joining given number of edges in
  const int JoinInE = NIdInEH.GetDat(JoinNId);
  int NoJoin=0;
  JoinProbH.AddDat(JoinInE).Val1 += 1;
  for (int i = 0; i < NIdInEH.Len(); i++) {
    if (NIdInEH[i] == JoinInE /*&& ! EndGroup.IsIn(NIdInEH.GetKey(i))*/) { NoJoin++; }
  }
  JoinProbH.AddDat(JoinInE).Val2 += NoJoin;
  // prob. of joining given degree and number of edges between nodes in the group
  if (JoinInE==3 || JoinInE==4 || JoinInE==5 || JoinInE==6) {
    //THash<TInt, TIntPr>&  DegJoinH = DegJoinAdjH.AddDat(JoinInE);
  }
}

void TNingGroupEvol::PlotAll(const TStr& OutFNmPref) const { 
  THash<TInt, TFlt> InEProbH;
  THash<TInt, TFlt> InECntH, AllCntH;
  double npos=0, nobs=0;
  for (int e = 0; e < JoinProbH.Len(); e++) {
    InECntH.AddDat(JoinProbH.GetKey(e), JoinProbH[e].Val1);
    AllCntH.AddDat(JoinProbH.GetKey(e), JoinProbH[e].Val2);
    InEProbH.AddDat(JoinProbH.GetKey(e), JoinProbH[e].Val1/(double)JoinProbH[e].Val2);
    npos += JoinProbH[e].Val1;  nobs += JoinProbH[e].Val2;
  }
  TGnuPlot::PlotValCntH(InEProbH, OutFNmPref+"-JoinProb", TStr::Fmt("%.1fm observations, %.1fm join events", nobs/Mega(1), npos/Mega(1)), "Number of friends in the group", "Prob. joining the group");
  TGnuPlot::PlotValCntH(InECntH, OutFNmPref+"-Joins", TStr::Fmt("%.1fm observations, %.1fm join events", nobs/Mega(1), npos/Mega(1)), "Number of friends in the group", "Number of nodes that joined the group");
  TGnuPlot::PlotValCntH(AllCntH, OutFNmPref+"-AllNodes", TStr::Fmt("%.1fm observations, %.1fm join events", nobs/Mega(1), npos/Mega(1)), "Number of friends in the group", "Number of such nodes");
}

// fraction of nodes that joined in 1 month period
// group starts, after i-th month check fringe, how many join till month i+1
void TNingGroupEvol2::AddNet(const PNingNet& Net, const TNingGroupV& GroupV) {
  const TSecTm MonthSecs(1*31*24*3600);
  static int NJoin = 0, NetId = 0;
  THash<TIntPr, TSecTm> EdgeTmSet(Net->GetEdges()); // edges over time
  THash<TInt, TIntH> NIdInEH;                       // group -> (node, number of friends in the group)
  THash<TInt, TIntV> NIdGroupV;                     // node  -> group memberships
  THash<TInt, TIntSet> GroupSet;                    // group -> nodes in the group
  THash<TSecTm, TIntPrV> TmJoinVH;                  // nodes that joined groups at time T
  int TmJoinIdx = 0, FutEdgeIdx=0;
  uint MinNexTm = 0;
  TVec<TSecTm> GroupTmV;
  // preprocess edges
  uint x =0; NetId++;
  for (TNingNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    IAssert(EI.GetSrcNId() != EI.GetDstNId());
    const TIntPr Edge(TMath::Mn(EI.GetSrcNId(), EI.GetDstNId()), TMath::Mx(EI.GetSrcNId(), EI.GetDstNId()));
    if (EdgeTmSet.IsKey(Edge)) { continue; }
    EdgeTmSet.AddDat(Edge, EI());
    IAssert(x <= EI().GetAbsSecs()); x = EI().GetAbsSecs(); // edges are sorted
  }
  // preprocess groups
  for (int g = 0; g < GroupV.Len(); g++) {
    for (int i = 0; i < GroupV[g].Len(); i++) {
      TmJoinVH.AddDat(GroupV[g].GetTm2(i)).Add(TIntPr(g, GroupV[g].GetNId(i))); } // time -> (group, node)
    GroupTmV.Add(TSecTm(GroupV[g].GetMnTm()+MonthSecs));
    MemCnt += GroupV[g].Len();
  }
  NetCnt++; GrpCnt+=GroupV.Len();
  // evolve the network
  PUNGraph CurG = TUNGraph::New(Net->GetNodes(), -1);  // graph at current time
  PUNGraph FutG = TUNGraph::New(Net->GetNodes(), -1);  // graph in 1 month
  TIntSet JoinSet, FringeSet;
  for (int t = 0; t < EdgeTmSet.Len(); t++) {
    const TIntPr Edge = EdgeTmSet.GetKey(t);
    const TSecTm EdgeTm = EdgeTmSet[t];
    // determine which nodes joined up to now
    for (; TmJoinIdx < TmJoinVH.Len() && TmJoinVH.GetKey(TmJoinIdx) < EdgeTm; TmJoinIdx++) {
      const TIntPrV& JoinV = TmJoinVH[TmJoinIdx];
      for (int j = 0; j < JoinV.Len(); j++) {
        GroupSet.AddDat(JoinV[j].Val1).AddKey(JoinV[j].Val2);  // group -> members
        NIdGroupV.AddDat(JoinV[j].Val2).Add(JoinV[j].Val1); }  // node -> groups
      // update the number of edges of a node pointing to the group
      for (int j = 0; j < JoinV.Len(); j++) {
        const int grp = JoinV[j].Val1, nid = JoinV[j].Val2;
        if (! CurG->IsNode(nid)) { continue; }
        const TUNGraph::TNodeI NI = CurG->GetNI(nid);
        for (int i = 0; i < NI.GetOutDeg(); i++) {
          NIdInEH.AddDat(grp).AddDat(NI.GetOutNId(i))++; }
        if (NIdInEH.IsKey(grp) && NIdInEH.GetDat(grp).IsKey(nid)) { continue; }
        const int ein = TSnap::CntEdgesToSet(CurG, nid, GroupSet.GetDat(grp));
        if (ein > 0) { NIdInEH.AddDat(grp).AddDat(nid) = ein; }
      }
    }
    // add edge to the current network
    if (! CurG->IsNode(Edge.Val1)) { CurG->AddNode(Edge.Val1); }
    if (! CurG->IsNode(Edge.Val2)) { CurG->AddNode(Edge.Val2); }
    CurG->AddEdge(Edge.Val1, Edge.Val2);
    // add edges to the future network
    for (; FutEdgeIdx < EdgeTmSet.Len() && EdgeTmSet[FutEdgeIdx] < EdgeTm + MonthSecs; FutEdgeIdx++) {
      const TIntPr FutE = EdgeTmSet.GetKey(FutEdgeIdx);
      if (! FutG->IsNode(FutE.Val1)) { FutG->AddNode(FutE.Val1); }
      if (! FutG->IsNode(FutE.Val2)) { FutG->AddNode(FutE.Val2); }
      FutG->AddEdge(FutE.Val1, FutE.Val2);
    }
    // update the number of edges of a node pointing to the group
    if (NIdGroupV.IsKey(Edge.Val1)) {
      const TIntV& GroupV = NIdGroupV.GetDat(Edge.Val1);
      for (int g = 0; g < GroupV.Len(); g++) {
        NIdInEH.AddDat(GroupV[g]).AddDat(Edge.Val2)++; }
    }
    if (NIdGroupV.IsKey(Edge.Val2)) {
      const TIntV& GroupV = NIdGroupV.GetDat(Edge.Val2);
      for (int g = 0; g < GroupV.Len(); g++) {
        NIdInEH.AddDat(GroupV[g]).AddDat(Edge.Val1)++; }
    }
    // determine which groups to consider
    //if (MinNexTm > EdgeTm.GetAbsSecs()) { continue; }
    for (int g = 0; g < GroupV.Len(); g++) {
      //MinNexTm = TMath::Mn(MinNexTm, GroupTmV[g].GetAbsSecs());
      if (GroupTmV[g] > EdgeTm) { continue; } // group has been checked less than 1 month ago
      if (! NIdInEH.IsKey(g)) { continue; }   // empty fringe
      GroupTmV[g] = EdgeTm + MonthSecs; // check again the group in a month
      //GroupTmV[g] += MonthSecs; // check again the group in a month
      const TSecTm NextMonthTm = GroupTmV[g];
      const TIntSet& CurGroup = GroupSet.GetDat(g);
      const TNingGroup& EndGroup = GroupV[g];
      const TIntH& NodeInEH = NIdInEH.GetDat(g);
      JoinSet.Clr();  FringeSet.Clr();
      // nodes in fringe
      for (int i = 0; i < NodeInEH.Len(); i++) { // non-member nodes with at least 1 edge in the group 
        const int NId = NodeInEH.GetKey(i);
        const bool IsNodeIn = CurGroup.IsKey(NId);
        if (! IsNodeIn || (IsNodeIn && EndGroup.GetTm(NId) > NextMonthTm)) { // not in group (now or will join later) and has >0 edges to the group
          FringeSet.AddKey(NId); }
      }
      // fringe nodes that will join inside a month
      for (int i = 0; i < FringeSet.Len(); i++) {
        if (EndGroup.IsIn(FringeSet[i]) && EndGroup.GetTm(FringeSet[i]) < NextMonthTm) { 
          JoinSet.AddKey(FringeSet[i]); }
      }
      //printf("%d %d G:%d  ", EdgeTm.GetAbsSecs()/MonthSecs, t, g);
      //printf("n1:%d  e1:%d  n2:%d  e2:%d  gr:%d  fr:%d  e:%d  j2:%d\n", CurG->GetNodes(), CurG->GetEdges(), FutG->GetNodes(), FutG->GetEdges(), CurGroup.Len(), FringeSet.Len(), NodeInEH.Len(), JoinSet.Len());
      //OnGroupTimeStep(CurG, GroupV[g], GroupSet.GetDat(g), FringeSet, JoinSet, NodeInEH, EdgeTm); // fringe node join prob
      OnGroupTimeStep2(g, CurG, FutG, GroupV[g], CurGroup, FringeSet, JoinSet, NodeInEH, EdgeTm);  // group growth
      if (++NJoin % 100 == 0) { printf("."); }
      if (NJoin % Kilo(10) == 0) { printf("p"); PlotAll(); }
    }
  }
}

// how do nodes join the group based on the edges they have into the group
void TNingGroupEvol2::OnGroupTimeStep(const PUNGraph& Graph, const TNingGroup& Group, const TIntSet& CurGroup, const TIntSet& FringeSet, const TIntSet& JoinSet, const TIntH& NodeInEH, const TSecTm& CurTm) {
  TIntSet InSet;
  for (int f = 0; f < FringeSet.Len(); f++) {
    const int NId = FringeSet[f];  IAssert(NodeInEH.IsKey(NId));
    const bool Joined = JoinSet.IsKey(NId);
    const int InDeg = NodeInEH.GetDat(NId);
    const int Deg = Graph->GetNI(NId).GetOutDeg();
    if (Deg < InDeg) { return; printf("XXXXXXXXX"); }
    { TFltPr& Pr = JoinCntH.AddDat(InDeg);
    if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
    { TFltPr& Pr = OutEdgeCntH.AddDat(Deg-InDeg);
    if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
    // prob. of joining given degree and number of edges between nodes in the group (SLOW)
    /*if (InDeg>=3 && InDeg<=7) {
      InSet.Clr();
      TUNGraph::TNodeI Node = Graph->GetNI(NId);
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        if (CurGroup.IsKey(Node.GetOutNId(e))) { 
          InSet.AddKey(Node.GetOutNId(e)); } }
      IAssert(InSet.Len() == InDeg);
      int InGroupEdges, InOutGroupEdges, OutGroupEdges;
      TSnap::GetNodeTriads(Graph, NId, InSet, InGroupEdges, InOutGroupEdges, OutGroupEdges);
      const double InAdj = InGroupEdges/double(InDeg*(InDeg-1)/2.0);
      const double OutAdj = OutGroupEdges/double((Deg-InDeg)*(Deg-InDeg-1)/2.0);
      const double InOutAdj = InOutGroupEdges/double(InDeg*(Deg-InDeg)/2.0);
      { TFltPr& Pr = DegAdjEdgeH.AddDat(InDeg).AddDat(int(100*InAdj));
      if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
      { TFltPr& Pr = DegInOutEdgeH.AddDat(InDeg).AddDat(int(100*InOutAdj));
      if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
      { TFltPr& Pr = DegOutEdgeH.AddDat(InDeg).AddDat(int(100*OutAdj));
      if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
      { TFltPr& Pr = InOutRatH.AddDat(InDeg).AddDat(int(100 * InGroupEdges/double(OutGroupEdges+InGroupEdges)));
      if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
      { TFltPr& Pr = InOutFracRatH.AddDat(InDeg).AddDat(int(100 * InAdj/(InAdj+OutAdj)));
      if (Joined) { Pr.Val1++; }  Pr.Val2 += 1; }
    } //*/
  }
}

int MyRat(const double& Fut, const double& Cur) {
  if (Cur>0 && Cur < Fut) { return int(10*Fut/Cur); }
  if (Fut>0 && Cur > Fut) { return int(-10*Cur/Fut); }
  return 0;
}

// how fast will the group grow in the future based on its current clustering (density, etc.)
void TNingGroupEvol2::OnGroupTimeStep2(const int& Id, const PUNGraph& CurGraph, const PUNGraph& FutGraph, const TNingGroup& Group, const TIntSet& CurGroup, const TIntSet& FringeSet, const TIntSet& JoinSet, const TIntH& NodeInEH, const TSecTm& CurTm) {
  if (CurGroup.Len() < 5) { return; }
  if (JoinSet.Len() == 0) { return; } // nobody joined, skip
  TIntV CurMemV(CurGroup.Len()+JoinSet.Len()), FutMemV;  
  CurGroup.GetKeyV(CurMemV);  FutMemV = CurMemV;
  for (int i = 0; i < JoinSet.Len(); i++) {
    IAssert(! CurGroup.IsKey(JoinSet[i]));
    FutMemV.Add(JoinSet[i]); 
  }
  PUNGraph CurG  = TSnap::GetSubGraph(CurGraph, CurMemV, true); // current group subgraph
  PUNGraph CmFutG = TSnap::GetSubGraph(FutGraph, CurMemV, true); // 1 month from now current group subgraph
  //PUNGraph JnFutG = TSnap::GetSubGraph(FutGraph, FutMemV); // 1 month from now future group subgraph
  TFltV EigValV;   
  // current group members graph
  const int    Size = CurGroup.Len();
  const double Growth = JoinSet.Len() / (double) CurGroup.Len();
  const int    CurNodes = CurG->GetNodes();
  const int    CurEdges = CurG->GetEdges();
  const double CurCCf = TSnap::GetClustCf(CurG);
  //printf("%d\t1:%d\te1:%d\tn2:%d\te2:%d\tccf:%g\tgr:%g\n", Id, CurG->GetNodes(), CurG->GetEdges(), CmFutG->GetNodes(), CmFutG->GetEdges(),  CurCCf, Growth);
  const double CurTrEd = CurEdges!=0 ? TSnap::GetTriadEdges(CurG) / double(CurEdges) : 0.0;
  const double CurWcc = TSnap::GetMxWccSz(CurG);  //TSnap::GetEigVals(CurG, 2, EigValV);
  //const double CurEigVal = abs(EigValV[0]);
  const double CurEffDiam = TSnap::GetBfsEffDiam(CurG, 100);
  // future graph of current group members
  const int    FutNodes = CmFutG->GetNodes();
  const int    FutEdges = CmFutG->GetEdges();
  const double FutCCf = TSnap::GetClustCf(CmFutG);
  const double FutTrEd = CurEdges!=0 ? TSnap::GetTriadEdges(CmFutG) / double(CurEdges) : 0.0;
  const double FutWcc = TSnap::GetMxWccSz(CmFutG);  //TSnap::GetEigVals(CmFutG, 2, EigValV);
  //const double FutEigVal = abs(EigValV[0]);
  const double FutEffDiam = TSnap::GetBfsEffDiam(CmFutG, 100);
  //THash<TInt, THash<TInt, TMom> > SzEdgesH, SzCcfH, SzTrEdH, SzWccH, SzEigValH, SzEffDiamH; // growth
  //THash<TInt, THash<TInt, TMom> > SzCcfRatH, SzCCfDiffH, SzEdgeDiffH, 
  AvgDegGrowthH.AddDat(int(100*CurEdges/CurNodes)).Add(Growth);
  TrEdGrowthH.AddDat(int(100*CurTrEd)).Add(Growth);
  CcfGrowthH.AddDat(int(100*CurCCf)).Add(Growth);
  WccGrowthH.AddDat(int(100*CurWcc)).Add(Growth);
  //EigValGrowthH.AddDat(abs(int(10*CurEigVal))).Add(Growth);
  DiamGrowthH.AddDat(int(10*CurEffDiam)).Add(Growth);
  // difference between current and future
  AvgDegDiffH.AddDat(int(100*(FutEdges/(double)FutNodes-CurEdges/(double)CurNodes))).Add(Growth);
  TrEdDiffH.AddDat(int(100*(FutTrEd-CurTrEd))).Add(Growth);
  CcfDiffH.AddDat(int(100*(FutCCf-CurCCf))).Add(Growth);
  WccSzDiffH.AddDat(int(100*(FutWcc-CurWcc))).Add(Growth);
  //EigValDiffH.AddDat(int(100*(CurEigVal-FutEigVal))).Add(Growth);
  EffDiamDiffH.AddDat(int(100*(FutEffDiam-CurEffDiam))).Add(Growth);
  // ratio between current and future
  AvgDegRatH.AddDat(MyRat(FutEdges/(double)FutNodes, CurEdges/(double)CurNodes)).Add(Growth);
  TrEdRatH.AddDat(MyRat(FutTrEd, CurTrEd)).Add(Growth);
  CcfRatH.AddDat(MyRat(FutCCf,CurCCf)).Add(Growth);
  WccSzRatH.AddDat(MyRat(FutWcc, CurWcc)).Add(Growth);
  //EigValRatH.AddDat(int(100*(CurEigVal/FutEigVal))).Add(Growth);
  EffDiamRatH.AddDat(MyRat(FutEffDiam, CurEffDiam)).Add(Growth);
}

void TNingGroupEvol2::PlotAll(const TStr& Title) const {
  TStr TitleX = TStr::Fmt("%g nets, %g groups, %.1fm members,",  NetCnt, GrpCnt, MemCnt/Mega(1));
  PlotRatioHash(JoinCntH, OutFNmPref+"-JoinIn", TitleX, "Number of friends INSIDE the group");
  PlotRatioHash(OutEdgeCntH, OutFNmPref+"-JoinOut", TitleX, "Number of friends OUTSIDE the group");
  PlotRatioHash(DegAdjEdgeH, OutFNmPref+"-InEdge", TitleX, "Number of edges between IN-group friends"); 
  PlotRatioHash(DegInOutEdgeH, OutFNmPref+"-InOutEdge", TitleX, "Number of edges between IN- and OUT-group friends"); 
  PlotRatioHash(DegOutEdgeH, OutFNmPref+"-OutEdge", TitleX, "Number of edges between OUT-group friends"); 
  PlotRatioHash(InOutRatH, OutFNmPref+"-InOutFrac", TitleX, "Frac of IN-group friend edges (over OUT-group friend edges)"); 
  PlotRatioHash(InOutFracRatH, OutFNmPref+"-InOutRat", TitleX, "Ratio of IN-group fraction (over OUT-group fraction)"); 
  //*/
  TGnuPlot::PlotValMomH(AvgDegGrowthH, OutFNmPref+"-Growth_AvgDeg", TitleX, "Average Degree", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TrEdGrowthH, OutFNmPref+"-Growth_TrEd", TitleX, "Fraction of edges supported by 1 triad", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(CcfGrowthH, OutFNmPref+"-Growth_Ccf", TitleX, "Clustering coefficient", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WccGrowthH, OutFNmPref+"-Growth_Wcc", TitleX, "Size of Largest Connected component", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  //TGnuPlot::PlotValMomH(EigValGrowthH, OutFNmPref+"-", TitleX, "", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, true);
  TGnuPlot::PlotValMomH(DiamGrowthH, OutFNmPref+"-Growth_Diam", TitleX, "Effective Diameter", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(AvgDegDiffH, OutFNmPref+"-Growth_AvgDegDiff", TitleX, "Difference in AvgDegree", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TrEdDiffH, OutFNmPref+"-Growth_TrEdDiff", TitleX, "Difference in Frac of triad edges", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(CcfDiffH, OutFNmPref+"-Growth_CcfDiff", TitleX, "Difference in CCF", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WccSzDiffH, OutFNmPref+"-Growth_WccDiff", TitleX, "Difference in WCC Size", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  //TGnuPlot::PlotValMomH(EigValDiffH, OutFNmPref+"-", TitleX, "", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, true);
  TGnuPlot::PlotValMomH(EffDiamDiffH, OutFNmPref+"-Growth_DiamDiff", TitleX, "Difference in Effective diameter", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(AvgDegRatH, OutFNmPref+"-Growth_AvgDegRat", TitleX, "Ratio of AvgDeg", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TrEdRatH, OutFNmPref+"-Growth_TrEdRat", TitleX, "Ratio of Frac of Triad Edges", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(CcfRatH, OutFNmPref+"-Growth_CcfRat", TitleX, "Ratio of CCFs", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WccSzRatH, OutFNmPref+"-Growth_WccRat", TitleX, "Ratio of WCC Size", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  //TGnuPlot::PlotValMomH(EigValRatH, OutFNmPref+"-", TitleX, "", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, true);
  TGnuPlot::PlotValMomH(EffDiamRatH, OutFNmPref+"-Growth_DiamRat", TitleX, "Ratio of Effective Diameter", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
}

void TNingGroupEvol2::PlotRatioHash(const THash<TInt, TFltPr>& XYRatH, const TStr& OutFNm, const TStr& Title, const TStr& XLabel) {
  if (XYRatH.Empty()) { return; }
  TFltPrV ProbV, PosCntV, AllCntV;
  TFltTrV StdErrV;
  double npos=0, nobs=0;
  for (int e = 0; e < XYRatH.Len(); e++) {
    const double Prob = XYRatH[e].Val1/XYRatH[e].Val2;
    ProbV.Add(TFltPr(XYRatH.GetKey(e).Val, Prob));
    StdErrV.Add(TFltTr(XYRatH.GetKey(e).Val, Prob, sqrt(Prob*(1-Prob)/XYRatH[e].Val2))); 
    PosCntV.Add(TFltPr(XYRatH.GetKey(e).Val, XYRatH[e].Val1));
    AllCntV.Add(TFltPr(XYRatH.GetKey(e).Val, XYRatH[e].Val2));
    npos += XYRatH[e].Val1;  nobs += XYRatH[e].Val2;
  }
  ProbV.Sort(); StdErrV.Sort(); PosCntV.Sort(); AllCntV.Sort();
  TStr Desc = Title + TStr::Fmt(" %.1fm joins, %.1fm in fringe.", npos/Mega(1), nobs/Mega(1));
  { TGnuPlot GP(OutFNm+"-Prob", Desc);
  GP.SetXYLabel(XLabel, "Probability of joining the group");
  GP.AddPlot(ProbV, gpwLinesPoints, "Probability");
  GP.AddErrBar(StdErrV, "Standard error");
  GP.AddCmd("set xrange [0:100]");
  GP.SavePng(); }
  //TGnuPlot::PlotValV(PosCntV, OutFNm+"-CntJoin", Desc, XLabel, "Number of nodes that joined the group", gpsLog);
  //TGnuPlot::PlotValV(AllCntV, OutFNm+"-CntFringe", Desc, XLabel, "Number of nodes in the fringe (joined and not)", gpsLog);
}

void TNingGroupEvol2::PlotRatioHash(const THash<TInt, THash<TInt, TFltPr> >& DegXYRatH, const TStr& OutFNm, const TStr& Title, const TStr& XLabel) {
  if (DegXYRatH.Empty()) { return; }
  TGnuPlot GP(OutFNm+"-Prob");
  GP.SetXYLabel(XLabel, "Probability of joining the group");
  TStr Desc;
  TIntV KeyV; DegXYRatH.GetKeyV(KeyV); KeyV.Sort();
  for (int x = 0; x < DegXYRatH.Len(); x++) {
    const int Deg = KeyV[x];
    const THash<TInt, TFltPr>& XYRatH = DegXYRatH.GetDat(Deg);
    TFltPrV ProbV;  TFltTrV StdErrV;
    double npos=0, nobs=0;
    for (int e = 0; e < XYRatH.Len(); e++) {
      const double Prob = XYRatH[e].Val1/XYRatH[e].Val2;
      ProbV.Add(TFltPr(XYRatH.GetKey(e).Val, Prob));
      StdErrV.Add(TFltTr(XYRatH.GetKey(e).Val, Prob, sqrt(Prob*(1-Prob)/XYRatH[e].Val2))); 
      npos += XYRatH[e].Val1;  nobs += XYRatH[e].Val2;
    }
    ProbV.Sort();  StdErrV.Sort();
    Desc = Title + TStr::Fmt(" %.1fm joins, %.1fm in fringe.", npos/Mega(1), nobs/Mega(1));
    GP.AddPlot(ProbV, gpwLinesPoints, TStr::Fmt("Probability. DEGREE %d", Deg));
    GP.AddErrBar(StdErrV, "");
  }
  GP.SetTitle(Desc);
  GP.AddCmd("set xrange [0:100]");
  GP.SavePng(); 
}

// for every group snapshot it when it had GroupSz members and check the 1 month growth
void TNingGroupEvol3::AddNet(const PNingNet& Net, const TNingGroupV& GroupV, const int& GroupSz, const int& MinFinalGroupSz, const int&  MaxFinalGroupSz) {
  const TSecTm MonthSecs(30*24*3600);
  static int NNet = 0;
  for (int g = 0; g < GroupV.Len(); g++) {
    if (GroupV[g].Len() < GroupSz || GroupV[g].Len() < MinFinalGroupSz || GroupV[g].Len() > MaxFinalGroupSz) { continue; }
    const TSecTm CheckTm = GroupV[g].GetTm2(GroupSz-1);
    PUNGraph CurG = Net->GetSubGraph(GroupV[g], GroupSz, CheckTm);
    PUNGraph FutG = Net->GetSubGraph(GroupV[g], -1, TSecTm(CheckTm+MonthSecs));
    // measure the network
    TFltV EigValV;   
    // current group members graph
    const double Growth = (FutG->GetNodes()-CurG->GetNodes()) / (double) CurG->GetNodes();
    const int    CurNodes = CurG->GetNodes();
    const int    CurEdges = CurG->GetEdges();
    const double CurCCf = TSnap::GetClustCf(CurG);
    //printf("%d\t1:%d\te1:%d\tn2:%d\te2:%d\tccf:%g\tgr:%g\n", GroupV[g].Len(), CurG->GetNodes(), CurG->GetEdges(), FutG->GetNodes(), FutG->GetEdges(),  CurCCf, Growth);
    const double CurTrEd = CurEdges!=0 ? TSnap::GetTriadEdges(CurG) / double(CurEdges) : 0.0;
    const double CurWcc = TSnap::GetMxWccSz(CurG);  //TSnap::GetEigVals(CurG, 2, EigValV);
    //const double CurEigVal = abs(EigValV[0]);
    const double CurEffDiam = TSnap::GetBfsEffDiam(CurG, 100);
    // future graph of current group members
    const int    FutNodes = FutG->GetNodes();
    const int    FutEdges = FutG->GetEdges();
    const double FutCCf = TSnap::GetClustCf(FutG);
    const double FutTrEd = CurEdges!=0 ? TSnap::GetTriadEdges(FutG) / double(CurEdges) : 0.0;
    const double FutWcc = TSnap::GetMxWccSz(FutG);  //TSnap::GetEigVals(FutG, 2, EigValV);
    //const double FutEigVal = abs(EigValV[0]);
    const double FutEffDiam = TSnap::GetBfsEffDiam(FutG, 100);
    //THash<TInt, THash<TInt, TMom> > SzEdgesH, SzCcfH, SzTrEdH, SzWccH, SzEigValH, SzEffDiamH; // growth
    //THash<TInt, THash<TInt, TMom> > SzCcfRatH, SzCCfDiffH, SzEdgeDiffH, 
    AvgDegGrowthH.AddDat(int(100*CurEdges/CurNodes)).Add(Growth);
    TrEdGrowthH.AddDat(int(100*CurTrEd)).Add(Growth);
    CcfGrowthH.AddDat(int(100*CurCCf)).Add(Growth);
    WccGrowthH.AddDat(int(100*CurWcc)).Add(Growth);
    //EigValGrowthH.AddDat(abs(int(10*CurEigVal))).Add(Growth);
    DiamGrowthH.AddDat(int(10*CurEffDiam)).Add(Growth);
    // difference between current and future
    AvgDegDiffH.AddDat(int(100*(FutEdges/(double)FutNodes-CurEdges/(double)CurNodes))).Add(Growth);
    TrEdDiffH.AddDat(int(100*(FutTrEd-CurTrEd))).Add(Growth);
    CcfDiffH.AddDat(int(100*(FutCCf-CurCCf))).Add(Growth);
    WccSzDiffH.AddDat(int(100*(FutWcc-CurWcc))).Add(Growth);
    //EigValDiffH.AddDat(int(100*(CurEigVal-FutEigVal))).Add(Growth);
    EffDiamDiffH.AddDat(int(100*(FutEffDiam-CurEffDiam))).Add(Growth);
    // ratio between current and future
    AvgDegRatH.AddDat(MyRat(FutEdges/(double)FutNodes, CurEdges/(double)CurNodes)).Add(Growth);
    TrEdRatH.AddDat(MyRat(FutTrEd, CurTrEd)).Add(Growth);
    CcfRatH.AddDat(MyRat(FutCCf,CurCCf)).Add(Growth);
    WccSzRatH.AddDat(MyRat(FutWcc, CurWcc)).Add(Growth);
    //EigValRatH.AddDat(int(100*(CurEigVal/FutEigVal))).Add(Growth);
    EffDiamRatH.AddDat(MyRat(FutEffDiam, CurEffDiam)).Add(Growth);  
  }
  if (++NNet % 100 == 0) { printf("."); }
  if (NNet % Kilo(1) == 0) { printf("p"); PlotAll(); }
}

void TNingGroupEvol3::PlotAll(const TStr& Title) const {
  TStr TitleX = TStr::Fmt("%g nets, %g groups, %.1fm members,",  NetCnt, GrpCnt, MemCnt/Mega(1));
  TGnuPlot::PlotValMomH(AvgDegGrowthH, OutFNmPref+"-Growth_AvgDeg", TitleX, "Average Degree", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TrEdGrowthH, OutFNmPref+"-Growth_TrEd", TitleX, "Fraction of edges supported by 1 triad", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(CcfGrowthH, OutFNmPref+"-Growth_Ccf", TitleX, "Clustering coefficient", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WccGrowthH, OutFNmPref+"-Growth_Wcc", TitleX, "Size of Largest Connected component", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  //TGnuPlot::PlotValMomH(EigValGrowthH, OutFNmPref+"-", TitleX, "", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, true);
  TGnuPlot::PlotValMomH(DiamGrowthH, OutFNmPref+"-Growth_Diam", TitleX, "Effective Diameter", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(AvgDegDiffH, OutFNmPref+"-Growth_AvgDegDiff", TitleX, "Difference in AvgDegree", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TrEdDiffH, OutFNmPref+"-Growth_TrEdDiff", TitleX, "Difference in Frac of triad edges", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(CcfDiffH, OutFNmPref+"-Growth_CcfDiff", TitleX, "Difference in CCF", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WccSzDiffH, OutFNmPref+"-Growth_WccDiff", TitleX, "Difference in WCC Size", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  //TGnuPlot::PlotValMomH(EigValDiffH, OutFNmPref+"-", TitleX, "", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, true);
  TGnuPlot::PlotValMomH(EffDiamDiffH, OutFNmPref+"-Growth_DiamDiff", TitleX, "Difference in Effective diameter", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(AvgDegRatH, OutFNmPref+"-Growth_AvgDegRat", TitleX, "Ratio of AvgDeg", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TrEdRatH, OutFNmPref+"-Growth_TrEdRat", TitleX, "Ratio of Frac of Triad Edges", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(CcfRatH, OutFNmPref+"-Growth_CcfRat", TitleX, "Ratio of CCFs", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WccSzRatH, OutFNmPref+"-Growth_WccRat", TitleX, "Ratio of WCC Size", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
  //TGnuPlot::PlotValMomH(EigValRatH, OutFNmPref+"-", TitleX, "", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, true);
  TGnuPlot::PlotValMomH(EffDiamRatH, OutFNmPref+"-Growth_DiamRat", TitleX, "Ratio of Effective Diameter", "Growth Rate (frac of new nodes added in 30 days)", gpsAuto, gpwLinesPoints, true, true, false, false, false, false, true);
}

  

#ifdef XXXXXXXXXXXXX

/////////////////////////////////////////////////
// Ning time network
void TNingNet::PlotGrowthStat(const TStr& OutFNm, FILE* StatF) const {
  TIntTrV EdgeV(Mega(1), 0);
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    EdgeV.Add(TIntTr(EI.GetDat().GetAbsSecs(), EI.GetSrcNId(), EI.GetDstNId())); }
  EdgeV.Sort();
  const int HalfTime = EdgeV[0].Val1() + (EdgeV.Last().Val1()-EdgeV[0].Val1())/2;
  const int Age = (EdgeV.Last().Val1()-EdgeV[0].Val1())/(24*3600);
  int HalfN=-1, HalfE=-1, Edges=EdgeV.Len(), ee=100;
  TIntH NTmH(1000), ETmH(1000), NodeSet(GetNodes()), HalfMtf, FullMtf;
  TFltPrV NEV, CCfV, WccV;
  double HalfWcc=0, HalfCcf=0;
  PNGraph G = TNGraph::New();
  for (int e  = 0; e < EdgeV.Len(); e++) {
    NodeSet.AddKey(EdgeV[e].Val2);
    NodeSet.AddKey(EdgeV[e].Val3);
    //if (HalfN==-1) {
      G->AddNode(EdgeV[e].Val2);  G->AddNode(EdgeV[e].Val3);
      G->AddEdge(EdgeV[e].Val2, EdgeV[e].Val3); //}
    if (HalfN==-1 && EdgeV[e].Val1 > HalfTime) {
      HalfN = NodeSet.Len();  HalfE = e; 
      TCnComV CnComV; TSnap::GetWccs(G, CnComV);
      HalfWcc = CnComV[0].Len()/(double)G->GetNodes(); 
      HalfCcf = TSnap::GetClustCf(G);
      printf("h");
      TD34GraphCounter GC(3);
      TSubGraphEnum<TD34GraphCounter> GraphEnum;
      PNGraph NG = TNGraph::New();
      TGraphEnumUtils::GetNormalizedGraph(G, NG);
      GraphEnum.GetSubGraphs(NG, 3, GC);
      for (int i = 0; i < GC.Len(); i++) { const int gid=GC.GetId(i);
        TGraphViz::Plot(GC.GetGraph(gid), gvlNeato, TStr::Fmt("motif-%02d-%04d.gif", i, gid), TStr::Fmt("GId:%d  Count: %llu", gid, GC.GetCnt(gid)));
        HalfMtf.AddDat((int)GC.GetId(i), (int)GC.GetCnt(GC.GetId(i))); }
      printf("c");
    }
    if (e == ee) {
      NEV.Add(TFltPr(NodeSet.Len(), e)); 
      ee = int(1.1*ee); }
    const int Tm = TSecTm(EdgeV[e].Val1()).GetInUnits(tmuMonth);
    NTmH.AddDat(Tm) = NodeSet.Len();
    ETmH.AddDat(Tm) = e;
    /*if (CCfV.Empty() || (int)CCfV.Last().Val1() != Tm) {
      CCfV.Add(TFltPr(Tm, TSnap::GetClustCf(G)));
      TCnComV CnComV; TSnap::GetWccs(G, CnComV);
      WccV.Add(TFltPr(Tm, CnComV[0].Len()/(double)G->GetNodes())); 
    }//*/
  }
  printf("d");
  // graph stats
  NEV.Add(TFltPr(NodeSet.Len(), Edges));
  PNet ThisPt = PNet((TNingNet*)this);
  TCnComV CnComV; TSnap::GetWccs(ThisPt, CnComV);
  const double FullWcc = CnComV[0].Len()/(double)GetNodes();
  const double FullCcf = TSnap::GetClustCf(G);
  double A, B, Siga, SigB, Chi2, R2;
  TSpecFunc::PowerFit(NEV, A, B, Siga, SigB, Chi2, R2);
  //
  /*TD34GraphCounter GC(3);
  TSubGraphEnum<TD34GraphCounter> GraphEnum;
  GraphEnum.GetSubGraphs(G, 3, GC);
  for (int i = 0; i < GC.Len(); i++) {
    FullMtf.AddDat((int)GC.GetId(i), (int)GC.GetCnt(GC.GetId(i))); }*/
  printf("x");
  if (StatF != NULL) {
    fprintf(StatF, "%s\t%d\t%d\t%f\t%f\t%d\t%d\t%f\t%f\t%d\t%f\t%f", 
      OutFNm.CStr(), GetNodes(), GetEdges(), FullWcc, FullCcf, HalfN, HalfE, HalfWcc, HalfCcf, Age, B, R2); 
    HalfMtf.SortByKey();
    for (int i = 0; i < HalfMtf.Len(); i++) { fprintf(StatF, "\t%d#%d", HalfMtf.GetKey(i), HalfMtf[i]); }
    fprintf(StatF, "\n");
  }
  printf("y\n");
  /* //if (GetNodes()>Kilo(10) && GetNodes() < Kilo(11)) {
    const TStr Desc = TStr::Fmt("%01d-%s (%d,%d) %d days", int(10.0*(HalfN/(double)G->GetNodes())), OutFNm.CStr(), GetNodes(), GetEdges(), Age);
    const TStr OF = TStr::Fmt("%01d-%s", int(10.0*(HalfN/(double)G->GetNodes())), OutFNm.CStr());
    TGnuPlot::PlotValV(NEV, OF+"-5dpl", Desc, "Nodes(t)", "Edges(t)", gpsLog, true); 
    TGnuPlot::PlotValV(CCfV, OF+"-3ccf", Desc, "time [months]", "Clustering coefficient"); 
    TGnuPlot::PlotValV(WccV, OF+"-4wcc", Desc, "time [months]", "Size of largest connected component"); 
    TGnuPlot::PlotValCntH(NTmH, OF+"-1nodes", Desc, "time [months]", "Nodes");
    TGnuPlot::PlotValCntH(ETmH, OF+"-2edges", Desc, "time [months]", "Edges");
    // full network properties
    TSnap::PlotInDegDistr(ThisPt, OF, Desc, false, true);
    TSnap::PlotOutDegDistr(ThisPt, OF, Desc, false, true);
    TSnap::PlotClustCf(ThisPt, OF, Desc);
    TSnap::PlotHops(ThisPt, OF, Desc);
    TSnap::PlotWccDistr(ThisPt, OF, Desc);
    TSnap::PlotSccDistr(ThisPt, OF, Desc);
  //}*/
}

/////////////////////////////////////////////////
// Ning network extractor: 
// 4 link types (A,B): A attended B's event, A requested friendship of B, A invited B to join a group, A commented on B's
void TNingNets::ParseNetworks(const TStr& LinkTy, const TNingUsrBs& UsrBs) {
  printf("\nLinkType:: %s\n", LinkTy.CStr()); TExeTm ExeTm;
  IAssert(LinkTy=="eventattendee" || LinkTy=="friendrequestmessage" || LinkTy=="groupinvitationrequest" || LinkTy=="comment");
  TChA SrcKey, DstKey;
  if (LinkTy=="eventattendee") {
    SrcKey="inviter";  DstKey="author";
  } else if (LinkTy=="friendrequestmessage") {
    SrcKey="author";  DstKey="recipient";
  } else if (LinkTy=="groupinvitationrequest") {
    SrcKey="author";  DstKey="requestor";
  } else if (LinkTy=="comment") {
    SrcKey="attachedToAuthor";  DstKey="author";
  }
  //const TStr NingPath = "";
  TJsonLoader J(NingPath+LinkTy+"\\*.gz");
  int MultiEdgeCnt=0, BadUsr=0;
  while (J.Next()) {
    IAssert(J.IsKey("appId") && J.IsKey(SrcKey) && J.IsKey(DstKey));
    const int AppId = atoi(J.GetDat("appId").CStr());
    const int SrcNId = UsrBs.GetUId(J.GetDat(SrcKey));
    const int DstNId = UsrBs.GetUId(J.GetDat(DstKey));
    if (SrcNId==-1 || DstNId==-1) { //printf("UNKNOWN User:\n  %s : %d\n  %s : %d\n",  J.GetDat(SrcKey).CStr(), SrcNId, J.GetDat(DstKey).CStr(), DstNId); 
      BadUsr++; continue; }
    if (SrcNId == DstNId) { continue; } // skip self edges
    // PNGraph
    /*PNGraph& G = AppIdNetH.AddDat(AppId);
    if (G.Empty()) { G = TNGraph::New(); }
    G->AddNode(SrcNId);  G->AddNode(DstNId);
    G->AddEdge(SrcNId, DstNId); //*/
    // NingNet
    const TSecTm Tm = TSecTm::GetDtTmFromStr(J.GetDat("createdDate")); //"createdDate": "2009-10-06 07:48:08.287",
    PNingNet& G = AppIdNetH.AddDat(AppId);
    if (G.Empty()) { G = TNingNet::New(); }
    TSecTm ETm;
    if (G->GetEDat(SrcNId, DstNId, ETm) && ETm > Tm) { // edge exists, update time
      G->GetEDat(SrcNId, DstNId) = Tm;
    } else { // no edge exists
      if (! G->IsNode(SrcNId) || G->GetNDat(SrcNId) > Tm) { G->AddNode(SrcNId, Tm); }
      if (! G->IsNode(DstNId)) { G->AddNode(DstNId, Tm); }
      G->AddEdge(SrcNId, DstNId, Tm);
    } //*/
    MultiEdgeCnt++;
  }
  printf("  %d  authors\n", UsrBs.Len());
  printf("  %d  networks\n", AppIdNetH.Len());
  printf("  %d  multi-edges\n", MultiEdgeCnt);
  printf("  %d  bad-users\n", BadUsr);
  int Nodes=0, Edges=0;
  for (int i=0; i<AppIdNetH.Len(); i++) {
    IAssert(! AppIdNetH[i].Empty());
    Nodes+=AppIdNetH[i]->GetNodes();
    Edges+=AppIdNetH[i]->GetEdges(); }
  printf("  %d  nodes\n", Nodes);
  printf("  %d  edges\n", Edges);
  printf("[%s]\n", ExeTm.GetStr());
}

void TNingNets::SaveTxtStat(const TStr& OutFNm) const {
  TIntPrV SzAIdV(AppIdNetH.Len(), 0);
  for (int i=0; i < AppIdNetH.Len(); i++) {
    SzAIdV.Add(TIntPr(AppIdNetH[i]->GetNodes(), i)); }
  //SzAIdV.Sort(false);
  SzAIdV.Shuffle(TInt::Rnd);
  FILE *StatF = fopen(TStr("GrowthStat2-"+OutFNm+".tab").CStr(), "wt");
  fprintf(StatF, "AppId\tNodes\tEdges\tFullWcc\tFullCcf\tNodes@T2\tEdges@T2\tWcc@T2\tCcf@T2\tAgeDays\tDPL\tR2\n");
  //for (int i=0; i<Kilo(1); i++) {
  for (int i=0; i<Len(); i++) {
    PNingNet G = AppIdNetH[SzAIdV[i].Val2];
    if (G->GetNodes() < Kilo(1) || G->GetNodes()>Kilo(10)) { continue; }
    printf("App: %d\tnodes: %d\tedges: %d\t", AppIdNetH.GetKey(SzAIdV[i].Val2),  G->GetNodes(), G->GetEdges());
    const TStr OutFNm = TStr::Fmt("app%d", AppIdNetH.GetKey(SzAIdV[i].Val2));
    G->PlotGrowthStat(OutFNm, StatF);
  }
  fclose(StatF);
}

//////////////////////////////////////////////////
// Ning groups
void TGroupBs::ParseGroups(const TNingUsrBs& UsrBs) {
  TJsonLoader J(NingPath+"\\groupmembership\\*.gz");
  TStrHash<TInt> GIdH;
  while (J.Next()) {
    const int AppId = atoi(J.GetDat("appId").CStr());
    const int UId = UsrBs.GetUId(J.GetDat("author"));
    TInt& GId = GIdH.AddDat(J.GetDat("groupId"));
    TVec<TIntV>& GroupV = GroupsH.AddDat(AppId);
    if (GId == 0) { // new group
      GroupV.Add();
      GId = GroupV.Len();
    }
    IAssert(GId-1 < GroupV.Len());
    GroupV[GId-1].AddUnique(UId);
  }
}

/////////////////////////////////////////////////
// Ning Event Statistics (per user or per network) 
TNingEventStat::TEventId TNingEventStat::GetId(const TStr& EventIdStr) {
  static TStrIntH StrToEventH;
  if (StrToEventH.Empty()) {
    StrToEventH.AddDat("activitylogitem", Activitylogitem);
    StrToEventH.AddDat("album", Album);
    StrToEventH.AddDat("blockedcontactlist", Blockedcontactlist);
    StrToEventH.AddDat("blockedcontactlist_senders", Blockedcontactlist_senders);
    StrToEventH.AddDat("blogpost", Blogpost);
    StrToEventH.AddDat("category", Category);
    StrToEventH.AddDat("comment", Comment);
    StrToEventH.AddDat("contactlist", Contactlist);
    StrToEventH.AddDat("event", Event);
    StrToEventH.AddDat("eventattendee", Eventattendee);
    StrToEventH.AddDat("friendrequestmessage", Friendrequestmessage);
    StrToEventH.AddDat("group", Group);
    StrToEventH.AddDat("groupicon", Groupicon);
    StrToEventH.AddDat("groupinvitationrequest", Groupinvitationrequest);
    StrToEventH.AddDat("groupmembership", Groupmembership);
    StrToEventH.AddDat("imageattachment", Imageattachment);
    StrToEventH.AddDat("invitationrequest", Invitationrequest);
    StrToEventH.AddDat("note", Note);
    StrToEventH.AddDat("opensocialapp", Opensocialapp);
    StrToEventH.AddDat("opensocialappdata", Opensocialappdata);
    StrToEventH.AddDat("opensocialappreview", Opensocialappreview);
    StrToEventH.AddDat("opensocialdevapp", Opensocialdevapp);
    StrToEventH.AddDat("orphaned_photo", Orphaned_photo);
    StrToEventH.AddDat("orphaned_track", Orphaned_track);
    StrToEventH.AddDat("orphaned_video", Orphaned_video);
    StrToEventH.AddDat("page", Page);
    StrToEventH.AddDat("photo", Photo);
    StrToEventH.AddDat("playlist", Playlist);
    StrToEventH.AddDat("role", Role);
    StrToEventH.AddDat("slideshowplayerimage", Slideshowplayerimage);
    StrToEventH.AddDat("topic", Topic);
    StrToEventH.AddDat("topiccommenterlink", Topiccommenterlink);
    StrToEventH.AddDat("track", Track);
    StrToEventH.AddDat("user", User);
    StrToEventH.AddDat("video", Video);
    StrToEventH.AddDat("videopreviewframe", Videopreviewframe);
  }
  return (TEventId) StrToEventH.GetDat(EventIdStr).Val;
}

TStr TNingEventStat::GetStr(const TNingEventStat::TEventId& EventId) {
  static TIntStrH EventToStrH;
  if (EventToStrH.Empty()) {
    EventToStrH.AddDat(Activitylogitem, "activitylogitem");
    EventToStrH.AddDat(Album, "album");
    EventToStrH.AddDat(Blockedcontactlist, "blockedcontactlist");
    EventToStrH.AddDat(Blockedcontactlist_senders, "blockedcontactlist_senders");
    EventToStrH.AddDat(Blogpost, "blogpost");
    EventToStrH.AddDat(Category, "category");
    EventToStrH.AddDat(Comment, "comment");
    EventToStrH.AddDat(Contactlist, "contactlist");
    EventToStrH.AddDat(Event, "event");
    EventToStrH.AddDat(Eventattendee, "eventattendee");
    EventToStrH.AddDat(Friendrequestmessage, "friendrequestmessage");
    EventToStrH.AddDat(Group, "group");
    EventToStrH.AddDat(Groupicon, "groupicon");
    EventToStrH.AddDat(Groupinvitationrequest, "groupinvitationrequest");
    EventToStrH.AddDat(Groupmembership, "groupmembership");
    EventToStrH.AddDat(Imageattachment, "imageattachment");
    EventToStrH.AddDat(Invitationrequest, "invitationrequest");
    EventToStrH.AddDat(Note, "note");
    EventToStrH.AddDat(Opensocialapp, "opensocialapp");
    EventToStrH.AddDat(Opensocialappdata, "opensocialappdata");
    EventToStrH.AddDat(Opensocialappreview, "opensocialappreview");
    EventToStrH.AddDat(Opensocialdevapp, "opensocialdevapp");
    EventToStrH.AddDat(Orphaned_photo, "orphaned_photo");
    EventToStrH.AddDat(Orphaned_track, "orphaned_track");
    EventToStrH.AddDat(Orphaned_video, "orphaned_video");
    EventToStrH.AddDat(Page, "page");
    EventToStrH.AddDat(Photo, "photo");
    EventToStrH.AddDat(Playlist, "playlist");
    EventToStrH.AddDat(Role, "role");
    EventToStrH.AddDat(Slideshowplayerimage, "slideshowplayerimage");
    EventToStrH.AddDat(Topic, "topic");
    EventToStrH.AddDat(Topiccommenterlink, "topiccommenterlink");
    EventToStrH.AddDat(Track, "track");
    EventToStrH.AddDat(User, "user");
    EventToStrH.AddDat(Video, "video");
    EventToStrH.AddDat(Videopreviewframe, "videopreviewframe");
  }
  return EventToStrH.GetDat((int) EventId);
}

void TNingAppStat::CollectStat() {
  TExeTm ExeTm;
  // TNingEventStat::Blockedcontactlist_senders
  for (int dir = 0; dir < TNingEventStat::MxEventId; dir++) {
    const TNingEventStat::TEventId Event = (TNingEventStat::TEventId) dir;
    printf("Event %s...", TNingEventStat::GetStr(Event).CStr());
    TJsonLoader J(NingPath+TNingEventStat::GetStr(Event)+"\\*.gz");
    int EvCnt=0, NoAppId=0, NoAuth=0;
    while (J.Next()) {
      if (! J.IsKey("appId")) { NoAppId++; continue; }
      if (! J.IsKey("author")) { NoAuth++; continue; }
      const int AppId = atoi(J.GetDat("appId").CStr());
      const int UId = GetAuthId(J.GetDat("author"));
      TNingEventStat& EV = Stat.AddDat(AppId);
      EV[Event]++;  EvCnt++;
    }
    printf("  events %d  [%s]\n", EvCnt, ExeTm.GetStr());
  }
}

void TNingAppStat::SaveTab(const TStr& OutFNm, const int& MinUsrs) const {
  int MinCnt=0;
  for (int i = 0; i < Stat.Len(); i++) {
    if (Stat[i][TNingEventStat::User] < MinUsrs) { MinCnt++; } }
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "#%d\tApps\t%d\tUsers\t%d\ttoo small apps (<%d)\n", Stat.Len(), AuthIdH.Len(), MinCnt, MinUsrs);
  fprintf(F, "#AppId");
  for (int dir = 0; dir < TNingEventStat::MxEventId; dir++) {
    fprintf(F, "\t%s", TNingEventStat::GetStr((TNingEventStat::TEventId) dir).CStr()); }
  fprintf(F, "\n");
  for (int i = 0; i < Stat.Len(); i++) {
    if (Stat[i][TNingEventStat::User] < MinUsrs) { continue; }
    fprintf(F, "%d", Stat.GetKey(i));
    for (int dir = 0; dir < TNingEventStat::MxEventId; dir++) {
      fprintf(F, "\t%d", Stat[i][(TNingEventStat::TEventId)dir]); }
    fprintf(F, "\n");
  }
  fclose(F);
}

void TNingAppStat::PlotAll(const TStr& OutFNm) const {
  TIntH ValCntH;
  /*for (int i = 0; i < Stat.Len(); i++) {
    ValCntH.AddDat(Stat[i].GetUsrs()) += 1; }
  TGnuPlot::PlotValCntH(ValCntH, "all_users."+OutFNm, TStr::Fmt("Number of users. %d apps, %d total users", 
  Stat.Len(), AuthIdH.Len()), "number of users in the App", "Count", gpsLog); // */
  for (int dir = 0; dir < TNingEventStat::MxEventId; dir++) {
    ValCntH.Clr(false);
    for (int i = 0; i < Stat.Len(); i++) {
      ValCntH.AddDat(Stat[i][(TNingEventStat::TEventId)dir]) += 1; }
    const TStr EventStr = TNingEventStat::GetStr((TNingEventStat::TEventId) dir);
    TGnuPlot::PlotValCntH(ValCntH, EventStr+"."+OutFNm, TStr::Fmt("Number of %s. %d apps, %d total users", 
      EventStr.CStr(), Stat.Len(), AuthIdH.Len()), TStr::Fmt("%s events in the App", EventStr.CStr()), "Count", gpsLog);   
  }
}

void TNingAppStatsDat::Parse(const TStr& FNm) {
  const int AppI=0, AppC=5, Raw=7, SignP=8, Uniq=9, Sess=10, SignS=11, TotP=12, TotS=13, Memb=14, Revn=15, Cat=17, Doma=2;
  TSsParser SS(FNm, '|');
  SS.Next();
  while (SS.Next()) {
    //printf("%s\n", SS.DumpStr());
    if (SS.Len() < 18) { continue; }
    const int AppId = SS.GetInt(AppI);
    TAppStat& AS = AppStatH.AddDat(AppId);
    AS.APP_CREATED_DT = TSecTm::GetDtTmFromStr(SS[AppC]);
    AS.RAW_PAGEVIEWS = SS.GetInt(Raw);
    AS.SIGNED_IN_PAGEVIEWS  = SS.GetInt(SignP);
    AS.UNIQUE_PROFILES = SS.GetInt(Uniq);
    AS.SESSIONS = SS.GetInt(Sess);
    AS.SIGNED_IN_SESSIONS = SS.GetInt(SignS);
    AS.TOTAL_SESSION_PAGES = SS.GetInt(TotP);
    int d=0, h=0,m=0;  float s=0;
    if (strstr(SS[TotS], "days")!=NULL) { sscanf(SS[TotS], "%02d:%02d:%f", &h, &m, &s); }
    else { sscanf(SS[TotS], "%d days %02d:%02d:%f", &d, &h, &m, &s); }
    AS.TOTAL_SESSION_DURATION= TSecTm(d*24*360+h*3600+m*60+int(s));
    AS.MEMBERS = atoi(SS[Memb]); // if empty set to 0
    AS.REVENUE = (int) SS.GetFlt(Revn);
    AS.Category = SS[Cat];
    AS.Domain = SS[Doma];
  }
  printf("done. %d apps.\n", AppStatH.Len());
}

void TNingAppStatsDat::SaveTab(const TStr& OutFNm, const int& MinUsrs) const {
  int MinCnt=0;
  for (int i = 0; i < AppStatH.Len(); i++) {
    if (AppStatH[i].UNIQUE_PROFILES < MinUsrs) { MinCnt++; } }
  TStrIntH CatCntH;
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "#%d\tApps\t%d\tsmall apps (<%d)\n", AppStatH.Len(), MinCnt, MinUsrs);
  fprintf(F, "#AppId\tRAW_PAGEVIEWS\tSIGNED_IN_PAGEVIEWS\tUNIQUE_PROFILES\tSESSIONS\tSIGNED_IN_SESSIONS\tTOTAL_SESSION_PAGES\tMEMBERS\tREVENUE\tAPP_CREATED_DT\tTOTAL_SESSION_DURATION\tCATEGORY\tSUBDOMAIN\n");
  for (int i = 0; i < AppStatH.Len(); i++) {
    CatCntH.AddDat(AppStatH[i].Category)+=1;
    if (AppStatH[i].UNIQUE_PROFILES < MinUsrs) { continue; }
    const TAppStat& AS = AppStatH[i];
    fprintf(F, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%s\t%s\n",
      AppStatH.GetKey(i), AS.RAW_PAGEVIEWS, AS.SIGNED_IN_PAGEVIEWS, AS.UNIQUE_PROFILES, AS.SESSIONS, AS.SIGNED_IN_SESSIONS, AS.TOTAL_SESSION_PAGES, AS.MEMBERS, AS.REVENUE, AS.APP_CREATED_DT.GetYmdTmStr().CStr(), AS.TOTAL_SESSION_DURATION.GetAbsSecs(), AS.Category.CStr(), AS.Domain.CStr());
  }
  fprintf(F, "\nCategory\tNumber of Apps\n");
  CatCntH.SortByDat(false);
  for (int c=0; c < CatCntH.Len(); c++) {
    fprintf(F, "%s\t%d\n", CatCntH.GetKey(c).CStr(), CatCntH[c]); }
  fclose(F);
}

void TNingAppStatsDat::PlotAll(const TStr& OutFNm) const {
  TIntH ValCntH;
  for (int v = 0; v < 9; v++) {
    ValCntH.Clr();
    printf("%d ", v);
    for (int i = 0; i < AppStatH.Len(); i++) {
      ValCntH.AddDat(AppStatH[i].GetVal(v)) += 1; }
    const TStr EventStr = TAppStat::GetValNm(v);
    TGnuPlot::PlotValCntH(ValCntH, OutFNm+"-"+EventStr, TStr::Fmt("Number of %s. %d apps", 
      EventStr.CStr(), AppStatH.Len()), TStr::Fmt("%s over all apps", EventStr.CStr()), "Count", gpsLog);   
  }
}

#endif
