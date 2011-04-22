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
    TGnuPlot::PlotValCntH(ValCntH, "ning-appEvents", "Ning App number of events distribution", "Number of events in the app", "Number of such apps", gpsLog); }
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
// Ning network base
void TNingNetBs::ParseNetworks(const TStr& InFNmWc, const TNingUsrBs& UsrBs, const TStr& LinkTy) {
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
  int MultiEdgeCnt=0, BadUsr=0;
  while (J.Next()) {
    EAssertR(J.IsKey("appId") && J.IsKey(SrcKey) && J.IsKey(DstKey), TStr::Fmt("Bad JSON object in %s line %d.", J.GetCurFNm(), J.GetLineNo()));
    const int AppId = atoi(J.GetDat("appId").CStr());
    const int SrcNId = UsrBs.GetUId(J.GetDat(SrcKey));
    const int DstNId = UsrBs.GetUId(J.GetDat(DstKey));
    if (SrcNId==-1 || DstNId==-1) { 
      //printf("Unknown User:\n  %s: %d\n  %s: %d\n",  J.GetDat(SrcKey).CStr(), SrcNId, J.GetDat(DstKey).CStr(), DstNId); 
      BadUsr++; continue; }
    if (SrcNId == DstNId) { continue; } // skip self edges
    // NingTmNet
    const TSecTm Tm = TSecTm::GetDtTmFromStr(J.GetDat("createdDate")); //"createdDate": "2009-10-06 07:48:08.287",
    PNingTmNet& G = AppNetH.AddDat(AppId);
    if (G.Empty()) { G = TNingTmNet::New(); }
    TSecTm ETm;
    if (! G->IsNode(SrcNId)) { G->AddNode(SrcNId, Tm); }
    if (! G->IsNode(DstNId)) { G->AddNode(DstNId, Tm); }
    G->AddEdge(SrcNId, DstNId, -1, Tm);
  }
  printf("\nLoading done.\n");
  printf("  %d  authors\n", UsrBs.Len());
  printf("  %d  networks\n", AppNetH.Len());
  printf("  %d  bad-users\n", BadUsr);
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

#ifdef XXXXXXXXXXXXX

/////////////////////////////////////////////////
// Ning time network
void TNingTmNet::PlotGrowthStat(const TStr& OutFNm, FILE* StatF) const {
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
  PNet ThisPt = PNet((TNingTmNet*)this);
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
    // NingTmNet
    const TSecTm Tm = TSecTm::GetDtTmFromStr(J.GetDat("createdDate")); //"createdDate": "2009-10-06 07:48:08.287",
    PNingTmNet& G = AppIdNetH.AddDat(AppId);
    if (G.Empty()) { G = TNingTmNet::New(); }
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
    PNingTmNet G = AppIdNetH[SzAIdV[i].Val2];
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
