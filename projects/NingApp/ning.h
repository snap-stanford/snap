#pragma once

/////////////////////////////////////////////////
// Ning User Base
class TNingUsrBs {
private:
  TStrHash<TInt> UIdH;
  //THash<TInt, THash<TInt, TNingEventStat> > StatH; // (app-id, (uid, stat))
public:
  TNingUsrBs() { }
  TNingUsrBs(TSIn& SIn, const bool& LoatStat=true) { Load(SIn, LoatStat); }
  void Save(TSOut& SOut) const { UIdH.Save(SOut); } 
  void Load(TSIn& SIn, const bool& LoadStat=true) { UIdH.Load(SIn); }
  int Len() const { return UIdH.Len(); }
  int AddUId(const TChA& UsrHash) { return UIdH.AddDatId(UsrHash); }
  int GetUId(const char* UsrHash) const { return UIdH.GetKeyId(UsrHash); }
  int GetUId(const TChA& UsrHash) const { return UIdH.GetKeyId(UsrHash); }
  int GetUId(const TStr& UsrHash) const { return UIdH.GetKeyId(UsrHash); }
  bool IsUId(const char* UsrHash) const { return UIdH.IsKey(UsrHash); }
  bool IsUId(const TChA& UsrHash) const { return UIdH.IsKey(UsrHash); }
  void ParseUsers(const TStr& InFNmWc, const TStr& PlotOutFNm="");
};

/////////////////////////////////////////////////
// Ning time network
class TNingNet;
typedef TPt<TNingNet> PNingNet;

class TNingNet : public TTimeNENet {
public:
  static const int MnTm; // Feb 8 2007
  static const int MxTm; // Jun 10 2010
public:
  TNingNet() : TTimeNENet() { }
  TNingNet(const int& Nodes, const int& Edges) : TTimeNENet(Nodes, Edges) { }
  TNingNet(TSIn& SIn) : TTimeNENet(SIn) { }
  TNingNet(const TNingNet& NingNet) : TTimeNENet(NingNet) { }
  void Save(TSOut& SOut) const { TTimeNENet::Save(SOut); }
  static PNingNet New() { return new TNingNet(); }
  static PNingNet New(const int& Nodes, const int& Edges) { return new TNingNet(Nodes, Edges); }
  static PNingNet Load(TSIn& SIn) { return new TNingNet(SIn); }
  bool operator < (const TNingNet& Net) const { Fail; return false; }
  TSecTm GetMnTm() const { return EdgeH[0].GetDat(); }
  TSecTm GetMxTm() const { return EdgeH[EdgeH.Len()-1].GetDat(); }
  int GetAge(const TTmUnit& TmUnit) const { return GetMxTm()!=GetMnTm() ? TSecTm(GetMxTm()-GetMnTm()).GetInUnits(TmUnit) : 0; }
  int GetDeadTm(const TTmUnit& TmUnit) const { return TSecTm(MxTm - GetMxTm()).GetInUnits(TmUnit); }
  TStr GetTitle() const;
  void GetNodesOverTm(const TTmUnit TmUnit, const int& TmSteps, TFltV& NodesTmV, const bool Cummulative=true, const bool& Relative=false);

  void PlotOverTime(const TStr& OutFNmPref) const;
  
  static double GetL2Dist(const TFltV& V1, const TFltV& V2);

  friend class TPt<TNingNet>;
};

/////////////////////////////////////////////////
// Ning network base
// Link types: attended event, requested friendship, invited to group, comment
class TNingNetBs;
typedef TPt<TNingNetBs> PNingNetBs;

class TNingNetBs {
private:
  TCRef CRef;
  THash<TInt, PNingNet> AppNetH;   // app-id to network
public:
  TNingNetBs() { }
  TNingNetBs(TSIn& SIn) : AppNetH(SIn) { }
  void Save(TSOut& SOut) const { AppNetH.Save(SOut); }
  void Load(TSIn& SIn) { AppNetH.Load(SIn); }
  static PNingNetBs New() { return new TNingNetBs(); }
  static PNingNetBs New(TSIn& SIn) { return new TNingNetBs(SIn); }
  
  int Len() const { return AppNetH.Len(); }
  int GetAppId(const int& KeyId) const { return AppNetH.GetKey(KeyId); }
  bool IsNet(const int& AppId) const { return AppNetH.IsKey(AppId); }
  PNingNet GetNet(const int& KeyId) const { return AppNetH[KeyId]; }
  PNingNet GetNetId(const int& AppId) const { return AppNetH.GetDat(AppId); }
  void ParseNetworks(const TStr& InFNmWc, TNingUsrBs& UsrBs, const TStr& LinkTy);
  PNingNetBs GetSubBs(const TIntV& AppIdV) const;
  PNingNetBs GetSubBs(const int& MinNodes, const int& MinAge, const int& MinDeadTm) const;
  // plots & statistics
  void SaveTxtStat(const TStr& OutFNm, const int& MnSz, const int& MxSz) const;
  void PlotDeadStat(const TStr& OutFNmPref) const;
  void PlotSimNodesEvol(const int& NetId, const int& PlotN, const int& TmSteps, const int& MinAge, const int& MinNodes) const;
    
  friend class TPt<TNingNetBs>;
};

//////////////////////////////////////////////////
// Ning groups
class TNingGroupBs;
typedef TPt<TNingGroupBs> PNingGroupBs;

class TNingGroup {
private:
  THash<TInt, TSecTm> NIdJoinTmH;
public:
  TNingGroup() { }
  TNingGroup(TSIn& SIn) : NIdJoinTmH(SIn){ }
  void Save(TSOut& SOut) const { NIdJoinTmH.Save(SOut); }
  void Load(TSIn& SIn) { NIdJoinTmH.Load(SIn); }
  int Len() const { return NIdJoinTmH.Len(); }
  bool IsIn(const int& NId) const { return NIdJoinTmH.IsKey(NId); }
  int GetNId(const int& KeyId) const { return NIdJoinTmH.GetKey(KeyId); }
  int operator [] (const int& KeyId) { return GetNId(KeyId); }
  TSecTm GetTm(const int& NId) const { return NIdJoinTmH.GetDat(NId); }
  TSecTm GetTm2(const int& KeyId) const { return NIdJoinTmH[KeyId]; }
  bool operator < (const TNingGroup& Group) const { Fail; return false; }
  TSecTm GetMnTm() const { return NIdJoinTmH[0]; }
  TSecTm GetMxTm() const { return NIdJoinTmH[NIdJoinTmH.Len()-1]; }
  int GetAge(const TTmUnit& TmUnit) const { IAssert(Len()>0); IAssert(GetMnTm().IsDef() && GetMxTm().IsDef()); 
    IAssert(GetMxTm() >= GetMnTm()); return TSecTm(GetMxTm()-GetMnTm()).GetInUnits(TmUnit); }
  int GetDeadTm(const TTmUnit& TmUnit) const { return TSecTm(TNingNet::MxTm - GetMxTm()).GetInUnits(TmUnit); }
  void AddUsr(const int& UId, const TSecTm& JoinTm);
  void SortByTm() { NIdJoinTmH.SortByDat(true); }
};

typedef TVec<TNingGroup> TNingGroupV;

class TNingGroupBs {
private:
  TCRef CRef;
  THash<TInt, TVec<TNingGroup> > GroupsH; // (appid, node group memberships over time)
public:
  TNingGroupBs() { }  
  TNingGroupBs(TSIn& SIn) : GroupsH(SIn) { }
  void Save(TSOut& SOut) const { GroupsH.Save(SOut); }
  void Load(TSIn& SIn) { GroupsH.Load(SIn); }
  static PNingGroupBs New() { return new TNingGroupBs(); }
  static PNingGroupBs New(TSIn& SIn) { return new TNingGroupBs(SIn); }
  
  int Len() const { return GroupsH.Len(); }
  bool HasGroups(const int& AppId) const { return GroupsH.IsKey(AppId); }
  int GetGroups(const int& AppId) const { return GroupsH.GetDat(AppId).Len(); }
  const TNingGroup& GetGroup(const int& AppId, const int& GroupId) const { return GroupsH.GetDat(AppId)[GroupId]; }
  const TNingGroupV& GetGroupV(const int& AppId) const { return GroupsH.GetDat(AppId); }
  void ParseGroups(const TStr& InFNmWc, const TNingUsrBs& UsrBs);
  PNingGroupBs GetSubBs(const PNingNetBs& NetBs, const int& MinSz, const double& MaxFracSz, const int& MinAge);
  void PlotGroupStat(const TStr& OutFNm) const;
  friend class TPt<TNingGroupBs>;
};


/////////////////////////////////////////////////
// Ning group evolution
class TNingGroupEvol { // for every group join event
public:
  THash<TInt, TFltPr> JoinProbH;
  THash<TInt, THash<TInt, TIntPr> > DegJoinAdjH, DegJoinCcfH;
public:
  TNingGroupEvol() { }
  void AddNet(const PNingNet& Net, const TNingGroupV& GroupV);
  void OnNodeJoined(const PUNGraph& G, const TNingGroup& EndGroup, const TIntSet& CurGroup, 
    const int& JoinNId, const TIntH& NIdInEH, const TSecTm& JoinTm);
  void PlotAll(const TStr& OutFNmPref) const;
};

//   InGroupTriads ... triads (g1, g2, g3), where g1, g2 and g3 are in GroupSet


class TNingGroupEvol2 { // month-by-month group/network snapshots
public:
  TStr OutFNmPref;
  double NetCnt, GrpCnt, MemCnt;
  THash<TInt, TFltPr> JoinCntH, OutEdgeCntH;
  THash<TInt, THash<TInt, TFltPr> > DegAdjEdgeH, DegInOutEdgeH, DegOutEdgeH, InOutRatH, InOutFracRatH;
public:
  TNingGroupEvol2(const TStr& OutFNm) : OutFNmPref(OutFNm), NetCnt(0), GrpCnt(0), MemCnt(0) { }
  void AddNet(const PNingNet& Net, const TNingGroupV& GroupV);
  void OnGroupTimeStep(const PUNGraph& Graph, const TNingGroup& Group, const TIntSet& CurGroup, 
    const TIntSet& FringeSet, const TIntSet& JoinSet, const TIntH& NodeInEH, const TSecTm& CurTm);
  void PlotAll(const TStr& Title="") const;
  static void PlotRatioHash(const THash<TInt, TFltPr>& XYRatH, const TStr& OutFNm, const TStr& Title, const TStr& XLabel);
  static void PlotRatioHash(const THash<TInt, THash<TInt, TFltPr> >& DegXYRatH, const TStr& OutFNm, const TStr& Title, const TStr& XLabel);
};

/*
class TNingEventStat;

/////////////////////////////////////////////////
// Ning Event Statistics
class TNingEventStat {
public:
  typedef enum { Activitylogitem, Album, Blockedcontactlist, Blockedcontactlist_senders, Blogpost, Category, Comment,
    Contactlist, Event, Eventattendee, Friendrequestmessage, Group, Groupicon, Groupinvitationrequest, Groupmembership,
    Imageattachment, Invitationrequest, Note, Opensocialapp, Opensocialappdata, Opensocialappreview, Opensocialdevapp,
    Orphaned_photo, Orphaned_track, Orphaned_video, Page, Photo, Playlist, Role, Slideshowplayerimage, Topic,
    Topiccommenterlink, Track, User, Video, Videopreviewframe, MxEventId
  } TEventId;
private:
  TTuple<TInt, 36> EventStat; // event count
public:
  TNingEventStat() { }
  TNingEventStat(TSIn& SIn) : EventStat(SIn) { }
  void Save(TSOut& SOut) const { EventStat.Save(SOut); }
  void Load(TSIn& SIn) { EventStat.Load(SIn); }
  const int& operator[] (const TEventId& EventId) const { return EventStat[(int)EventId].Val; }
  int& operator[] (const TEventId& EventId) { return EventStat[(int)EventId].Val; }
  static TEventId GetId(const TStr& EventIdStr);
  static TStr GetStr(const TEventId& EventId);
};

//////////////////////////////////////////////////
// Ning Net
class TNingNodeDat {
private:
  TSecTm Tm;
public:
  TNingNodeDat() : Tm() { }
  TNingNodeDat(const TSecTm& Time) : Tm(Time) { }
  TNingNodeDat(const TNingNodeDat& Node) : Tm(Node.Tm) { }
  TNingNodeDat(TSIn& SIn) : Tm(SIn) { }
  void Save(TSOut& SOut) const { Tm.Save(SOut); }
  const TSecTm& GetTm() const { return TSecTm; }
};

class TNingEdgeDat {
private:
  TSecTm Tm;
  TInt Wgt;
public:
  TNingEdgeDat() : Tm(), Wgt(0) { }
  TNingEdgeDat(const TSecTm& Time) : Tm(Time), Wgt(0) { }
  TNingEdgeDat(const TNingEdgeDat& Edge) : Tm(Edge.Tm), Wgt(Edge.Wgt) { }
  TNingEdgeDat(TSIn& SIn) : Tm(SIn)u, EventCnt(SIn) { }
  void Save(TSOut& SOut) const { Tm.Save(SOut); Wgt.Save(SOut); }
  const TSecTm& GetTm() const { return TSecTm; }
  int GetWgt() const { return TSecTm; }
};

class TNingNet : public TNodeEDatNet<TSecTm, TNingEdgeDat> {
public:
  typedef TNodeEDatNet<TSecTm, TSecTm> TNet;
  typedef TPt<TNodeEDatNet<TSecTm, TSecTm> > PNet;
public:
  TNingNet() { }
  TNingNet(const int& Nodes, const int& Edges) : TNet(Nodes, Edges) { }
  TNingNet(const TNingNet& TimeNet) : TNet(TimeNet) { }
  TNingNet(TSIn& SIn) : TNet(SIn) { }
  void Save(TSOut& SOut) const { TNet::Save(SOut); }
  static PNingNet New() { return new TNingNet(); }
  static PNingNet New(const int& Nodes, const int& Edges) { return new TNingNet(Nodes, Edges); }
  static PNingNet Load(TSIn& SIn) { return new TNingNet(SIn); }
  TNingNet& operator = (const TNingNet& TimeNet) { Fail; return *this; }

  void PlotGrowthStat(const TStr& OutFNm, FILE* StatF=NULL) const;

  friend class TPt<TNingNet>;
};

//////////////////////////////////////////////////
// Ning network statistics and app_stats table
class TNingAppStat {
private:
  THash<TInt, TNingEventStat> Stat; // counts
  TStrHash<TInt> AuthIdH;           // user to user_id (key_id)
public:
  TNingAppStat() { }
  void Save(TSOut& SOut) const { Stat.Save(SOut); AuthIdH.Save(SOut); }
  void Load(TSIn& SIn) { Stat.Load(SIn); AuthIdH.Load(SIn); }
  int GetAuthId(const TChA& AuthHash) { return AuthIdH.AddKey(AuthHash); }
  void CollectStat();
  void SaveTab(const TStr& OutFNm, const int& MinUsrs) const;
  void PlotAll(const TStr& OutFNm) const;
};

class TNingAppStatsDat { // parse "app_stats.dat"
public:
  class TAppStat {
  public:
    TInt RAW_PAGEVIEWS, SIGNED_IN_PAGEVIEWS, UNIQUE_PROFILES, SESSIONS, SIGNED_IN_SESSIONS;
    TInt TOTAL_SESSION_PAGES, MEMBERS, REVENUE;
    TSecTm APP_CREATED_DT, TOTAL_SESSION_DURATION;
    TChA Category, Domain;
  public:
    TAppStat() { }
    TAppStat(TSIn& SIn) { Load(SIn); }
    void Load(TSIn& SIn) { RAW_PAGEVIEWS.Load(SIn); SIGNED_IN_PAGEVIEWS.Load(SIn); UNIQUE_PROFILES.Load(SIn); SESSIONS.Load(SIn);
      SIGNED_IN_SESSIONS.Load(SIn); TOTAL_SESSION_PAGES.Load(SIn); MEMBERS.Load(SIn);  REVENUE.Load(SIn);
      APP_CREATED_DT.Load(SIn); TOTAL_SESSION_DURATION.Load(SIn);  Category.Load(SIn); Domain.Load(SIn); }
    void Save(TSOut& SOut) const { RAW_PAGEVIEWS.Save(SOut); SIGNED_IN_PAGEVIEWS.Save(SOut); UNIQUE_PROFILES.Save(SOut); SESSIONS.Save(SOut);
      SIGNED_IN_SESSIONS.Save(SOut); TOTAL_SESSION_PAGES.Save(SOut); MEMBERS.Save(SOut); REVENUE.Save(SOut);
      APP_CREATED_DT.Save(SOut); TOTAL_SESSION_DURATION.Save(SOut); Category.Save(SOut); Domain.Save(SOut); }
    int GetVal(const int& ValId) const { IAssert(ValId<9);  return *((int*)this+ValId); }
    static TStr GetValNm(const int& ValId)  {
      static char* ValNm [] = { "RAW_PAGEVIEWS", "SIGNED_IN_PAGEVIEWS", "UNIQUE_PROFILES", "SESSIONS", "SIGNED_IN_SESSIONS", "TOTAL_SESSION_PAGES", "MEMBERS", "REVENUE", "APP_CREATED_DT", "TOTAL_SESSION_DURATION" };
      return TStr(ValNm[ValId]); }
  };
private:
  THash<TInt, TAppStat> AppStatH;
public:
  TNingAppStatsDat() { }
  void Parse(const TStr& FNm);
  void SaveTab(const TStr& OutFNm, const int& MinUsrs) const;
  void PlotAll(const TStr& OutFNm) const;
};

//*/