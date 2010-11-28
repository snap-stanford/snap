#ifndef btalarms_h
#define btalarms_h

#define BIN_02

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "mine.h"
#include "odbc.h"

ClassHdTP(TBtaEventBs, PBtaEventBs);

/////////////////////////////////////////////////
// BTAlarms-Device-Definition
ClassTPV(TBtaDevDef, PBtaDevDef, TBtaDevDefV)//{
public:
  TInt IpNumId;
  TInt NodeId;
  TInt ZoneId;
  TInt LocId;
  TInt ProjId;
  TInt TypeId;
public:
  TBtaDevDef():
    IpNumId(-1), NodeId(-1), ZoneId(-1), LocId(-1), ProjId(-1), TypeId(-1){}
  TBtaDevDef(const TBtaDevDef& DevDef):
    IpNumId(DevDef.IpNumId), NodeId(DevDef.NodeId), ZoneId(DevDef.ZoneId),
    LocId(DevDef.LocId), ProjId(DevDef.ProjId), TypeId(DevDef.TypeId){}
  static PBtaDevDef New(
   const int& IpNumId, const int& NodeId, const int& ZoneId,
   const int& LocId, const int& ProjId, const int& TypeId){
    PBtaDevDef DevDef=new TBtaDevDef();
    DevDef->IpNumId=IpNumId; DevDef->NodeId=NodeId; DevDef->ZoneId=ZoneId;
    DevDef->LocId=LocId; DevDef->ProjId=ProjId; DevDef->TypeId=TypeId;
    return DevDef;}
  TBtaDevDef(TSIn& SIn):
    IpNumId(SIn), NodeId(SIn), ZoneId(SIn),
    LocId(SIn), ProjId(SIn), TypeId(SIn){}
  static PBtaDevDef Load(TSIn& SIn){return new TBtaDevDef(SIn);}
  void Save(TSOut& SOut) const {
    IpNumId.Save(SOut); NodeId.Save(SOut); ZoneId.Save(SOut);
    LocId.Save(SOut); ProjId.Save(SOut); TypeId.Save(SOut);}

  TBtaDevDef& operator=(const TBtaDevDef& DevDef){
    IpNumId=DevDef.IpNumId; NodeId=DevDef.NodeId; ZoneId=DevDef.ZoneId;
    LocId=DevDef.LocId; ProjId=DevDef.ProjId; TypeId=DevDef.TypeId;
    return *this;}
  bool operator==(const TBtaDevDef& DevDef) const {return IpNumId==DevDef.IpNumId;}
  bool operator<(const TBtaDevDef& DevDef) const {return IpNumId<DevDef.IpNumId;}

  // files
  static PBtaDevDef LoadBin(const TStr& FNm){
    TFIn FIn(FNm); return new TBtaDevDef(FIn);}
  void SaveBin(const TStr& FNm){
    TFOut FOut(FNm); Save(FOut);}
};

/////////////////////////////////////////////////
// BTAlarms-Device-Definition-Base
typedef THash<TStr, TInt, TStrHashF_OldGLib> TStrOldH;

ClassTP(TBtaDevDefBs, PBtaDevDefBs)//{
private:
  TStrOldH IpNumNmH;
  TStrOldH NodeNmH;
  TStrOldH ZoneNmH;
  TStrOldH LocNmH;
  TStrOldH ProjNmH;
  TStrOldH TypeNmH;
  THash<TInt, PBtaDevDef> IpNumToDevDefH;
  UndefCopyAssign(TBtaDevDefBs);
public:
  TBtaDevDefBs(){}
  static PBtaDevDefBs New(){
    return new TBtaDevDefBs();}
  TBtaDevDefBs(TSIn& SIn):
    IpNumNmH(SIn), NodeNmH(SIn), ZoneNmH(SIn),
    LocNmH(SIn), ProjNmH(SIn), TypeNmH(SIn),
    IpNumToDevDefH(SIn){}
  static PBtaDevDefBs Load(TSIn& SIn){return new TBtaDevDefBs(SIn);}
  void Save(TSOut& SOut) const {
    IpNumNmH.Save(SOut); NodeNmH.Save(SOut); ZoneNmH.Save(SOut);
    LocNmH.Save(SOut); ProjNmH.Save(SOut); TypeNmH.Save(SOut);
    IpNumToDevDefH.Save(SOut);}

  // ip-num
  int GetIpNums() const {return IpNumNmH.Len();}
  int AddIpNumNm(const TStr& IpNumNm){return IpNumNmH.AddKey(IpNumNm);}
  bool IsIpNumId(const TStr& IpNumNm) const {return IpNumNmH.IsKey(IpNumNm);}
  int GetIpNumId(const TStr& IpNumNm) const {return IpNumNmH.GetKeyId(IpNumNm);}
  TStr GetIpNumNm(const int& IpNumId) const {return IpNumNmH.GetKey(IpNumId);}
  const TStrOldH& GetIpNumNmH() const {return IpNumNmH;}

  // node
  int GetNodes() const {return NodeNmH.Len();}
  int AddNodeNm(const TStr& NodeNm){return NodeNmH.AddKey(NodeNm);}
  bool IsNodeId(const TStr& NodeNm) const {return NodeNmH.IsKey(NodeNm);}
  int GetNodeId(const TStr& NodeNm) const {return NodeNmH.GetKeyId(NodeNm);}
  TStr GetNodeNm(const int& NodeId) const {return NodeNmH.GetKey(NodeId);}
  const TStrOldH& GetNodeNmH() const {return NodeNmH;}

  // zone
  int GetZones() const {return ZoneNmH.Len();}
  int AddZoneNm(const TStr& ZoneNm){return ZoneNmH.AddKey(ZoneNm);}
  bool IsZoneId(const TStr& ZoneNm) const {return ZoneNmH.IsKey(ZoneNm);}
  int GetZoneId(const TStr& ZoneNm) const {return ZoneNmH.GetKeyId(ZoneNm);}
  TStr GetZoneNm(const int& ZoneId) const {return ZoneNmH.GetKey(ZoneId);}
  const TStrOldH& GetZoneNmH() const {return ZoneNmH;}

  // location
  int GetLocs() const {return LocNmH.Len();}
  int AddLocNm(const TStr& LocNm){return LocNmH.AddKey(LocNm);}
  bool IsLocId(const TStr& LocNm) const {return LocNmH.IsKey(LocNm);}
  int GetLocId(const TStr& LocNm) const {return LocNmH.GetKeyId(LocNm);}
  TStr GetLocNm(const int& LocId) const {return LocNmH.GetKey(LocId);}
  const TStrOldH& GetLocNmH() const {return LocNmH;}

  // project
  int GetProjs() const {return ProjNmH.Len();}
  int AddProjNm(const TStr& ProjNm){return ProjNmH.AddKey(ProjNm);}
  bool IsProjId(const TStr& ProjNm) const {return ProjNmH.IsKey(ProjNm);}
  int GetProjId(const TStr& ProjNm) const {return ProjNmH.GetKeyId(ProjNm);}
  TStr GetProjNm(const int& ProjId) const {return ProjNmH.GetKey(ProjId);}
  const TStrOldH& GetProjNmH() const {return ProjNmH;}

  // type
  int GetTypes() const {return TypeNmH.Len();}
  int AddTypeNm(const TStr& TypeNm){return TypeNmH.AddKey(TypeNm);}
  bool IsTypeId(const TStr& TypeNm) const {return TypeNmH.IsKey(TypeNm);}
  int GetTypeId(const TStr& TypeNm) const {return TypeNmH.GetKeyId(TypeNm);}
  TStr GetTypeNm(const int& TypeId) const {return TypeNmH.GetKey(TypeId);}
  const TStrOldH& GetTypeNmH() const {return TypeNmH;}

  // generic-object
  int GetNms(const TStrOldH& NmH) const {return NmH.Len();}
  bool IsId(const TStrOldH& NmH, const TStr& Nm) const {return NmH.IsKey(Nm);}
  int GetId(const TStrOldH& NmH, const TStr& Nm) const {return NmH.GetKeyId(Nm);}
  TStr GetNm(const TStrOldH& NmH, const int& Id) const {return NmH.GetKey(Id);}

  // device-definitions
  void AddDevDef(const PBtaDevDef& DevDef){
    IpNumToDevDefH.AddDat(DevDef->IpNumId, DevDef);}
  bool IsDevDef(const PBtaDevDef& DevDef){
    return IpNumToDevDefH.IsKey(DevDef->IpNumId);}
  PBtaDevDef GetDevDef(const int& IpNumId){
    return IpNumToDevDefH.GetDat(IpNumId);}

  // files
  static PBtaDevDefBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  static PBtaDevDefBs LoadTxt(const TStr& FNm);
};

/////////////////////////////////////////////////
// BTAlarms-Event-Data
ClassTV(TBtaEventDat, TBtaEventDatV)//{
public:
    TStrStrH FldNmToValH;
public:
  TBtaEventDat() {}
  void Load(TSIn& SIn) { FldNmToValH.Load(SIn); }
  TBtaEventDat(TSIn& SIn) { Load(SIn); }
  void Save(TSOut& SOut) const { FldNmToValH.Save(SOut); }

  void Clr() { FldNmToValH.Clr(); }
  void AddFld(const TStr& FldNm, const TStr& FldVal) { FldNmToValH.AddDat(FldNm, FldVal); }
  bool IsFld(const TStr& FldNm) const { return FldNmToValH.IsKey(FldNm); }
  const TStr& GetFld(const TStr& FldNm) const { return FldNmToValH.GetDat(FldNm); }
  const TStrStrH& GetFldNmToValH() const { return FldNmToValH; }

  // predefined fields
  TStr GetNodeId() const;
  TStr GetComponentId() const;
  TStr GetFirstTm() const;
  TStr GetLastTm() const;
  TStr GetSeverity() const;
  TStr GetSummary() const;
  TStr GetIR() const;
};

/////////////////////////////////////////////////
// BTAlarms-Event
ClassTV(TBtaEvent, TBtaEventV)//{
private:
  TInt IpNumId; // id of the device
  TInt ProjId; // id of the project
  TInt Severity; // severity level (0-5)
  TUInt64 FirstTmMSecs; // time alarm first fired
  TUInt64 LastTmMSecs; // time alarm last fired
  TInt CaseId; // related case id, -1 if none
  TInt BtaEventDatPt; // id of the row from the database with complete alarm record

public:
  TBtaEvent();
  TBtaEvent(const int& _IpNumId, const int& _ProjId, const int& _Severity,
    const uint64& _FirstTmMSecs, const uint64& _LastTmMSecs, 
    const int& _CaseId, const int& _BtaEventDatPt);
  
  TBtaEvent(TSIn& SIn);
  void Save(TSOut& SOut) const;

  int GetIpNumId() const { return IpNumId; }
  int GetProjId() const { return ProjId; }
  int GetSeverity() const { return Severity; }
  uint64 GetFirstTmMSecs() const { return FirstTmMSecs; }
  TTm GetFirstTm() const { return TTm::GetTmFromMSecs(FirstTmMSecs); }
  uint64 GetLastTmMSecs() const { return LastTmMSecs; }
  TTm GetLastTm() const { return TTm::GetTmFromMSecs(LastTmMSecs); }
  int GetCaseId() const { return CaseId; }
  int GetBlobPt() const { return BtaEventDatPt; }

  bool operator==(const TBtaEvent& Event) const { Fail; }
  bool operator<(const TBtaEvent& Event) const { Fail; }
};

/////////////////////////////////////////////////
// BTAlarms-Event-Base
ClassTP(TBtaEventBs, PBtaEventBs)//{
private:
    // device definitions
    PBtaDevDefBs DevDefBs;
    // list of all events with core inforamtion and pointers to the database
    TBtaEventV EventV;
    // hash table of all case names with their frequency as data
    TStrHash<TInt> CaseNmFqH;    
    // database
    POdbcDb OdbcDb;
    POdbcQuery Query;
    TStrHash<TVoid> ValidColumnH;
    // next event id for the database
    TInt LastEventId;

private:
    void LoadValidColumnH();
    bool IsValidColumn(const TStr& ColumnNm) const { return ValidColumnH.IsKey(ColumnNm); }

    // internal adding stuff
    void LoadLastEventId();
    int GetNextEventId() { LastEventId++; return LastEventId; }
    void ParseBtaEventDat(const TBtaEventDat& EventDat, 
        const int& BtaEventDatPt, TBtaEvent& Event);
    int StoreEventDat(const TBtaEventDat& EventDat, const TStr& TableNm);
    bool GetEventDat(const int& EventDatPt, 
        const TStr& TableNm, TBtaEventDat& EventDat) const;
    int AddCase(const TStr& CaseNm);
    int AddEvent(const TBtaEvent& Event) { return EventV.Add(Event); }

    UndefCopyAssign(TBtaEventBs);
public:
    TBtaEventBs(const PBtaDevDefBs& _DevDefBs, const POdbcDb& _OdbcDb, const int& ExpEvents = 10000000);
    static PBtaEventBs New(const PBtaDevDefBs& _DevDefBs, const POdbcDb& OdbcDb, 
        const int& ExpEvents = 10000000) { return new TBtaEventBs(_DevDefBs, OdbcDb, ExpEvents); }

    TBtaEventBs(TSIn& SIn, const POdbcDb& _OdbcDb);
    static PBtaEventBs Load(TSIn& SIn, const POdbcDb& OdbcDb) {
        return new TBtaEventBs(SIn, OdbcDb); }
    void Save(TSOut& SOut) const;

    // device definition
    PBtaDevDefBs GetDevDefBs() const {return DevDefBs;}

    // cases
    TStr GetCase(const int& CaseId) const { return CaseNmFqH.GetKey(CaseId); }

    // events
    int GetEvents() const { return EventV.Len(); }
    TBtaEvent& GetEvent(const int& EventN) { return EventV[EventN]; }
    int GetNextEvent(const int& StartEventN, const int& IpNumId);
    void GetEventDat(const int& EventDatPt, TBtaEventDat& EventDat) const;    

    // alfa in omega of events :-)
    const TBtaEvent& GetFirstEvent() const { return EventV[0]; }
    const TBtaEvent& GetLastEvent() const { return EventV.Last(); }
    TTm GetFirstTime() const { return GetFirstEvent().GetFirstTm(); }
    TTm GetLastTime() const { return GetLastEvent().GetFirstTm(); }

    // most active projects
    void GetMostActiveProjs(const int64& TimeWnd, 
        const int& EventWnd, const int& MnSeverity, TIntKdV& ProjIdFqV);

    // adding events, BatchP tells if this is batch loading of alarms (we store it directly 
    // to final table), or not (we store it to intermedirary dump_buffer)
    int AddBtEvent(const TBtaEventDat& EventDat, const bool& BatchP);
    // processing buffer with new alarms
    void AddNewBuffer(TIntV& EventIdV);
    // moves the alarms from dump_buffer to alarms table, effectively 
    // considering them as processed and archiverd
    void SaveDumpBuffer();
    // rescue stuff from dump buffer
    void RescueDumpBuffer(TIntV& EventIdV);

    // files
    static PBtaEventBs LoadBin(const TStr& FNm, const POdbcDb& OdbcDb) {
        TFIn FIn(FNm); return TBtaEventBs::Load(FIn, OdbcDb); }
    void SaveBin(const TStr& FNm) { TFOut FOut(FNm); Save(FOut); }
    static PBtaEventBs LoadBtTxt(const PBtaDevDefBs& DevDefBs, const TStr& TxtFNm, 
        const POdbcDb& OdbcDb, const int& ExpEvents, const int& Offset);
    static PBtaEventBs LoadBankTxt(const PBtaDevDefBs& DevDefBs, 
        const TStr& TxtFNm, const POdbcDb& OdbcDb, const int& ExpEvents);
};

/////////////////////////////////////////////////
// BTAlarms-Correlations-Base
ClassTP(TBtaCorrBs, PBtaCorrBs)//{
public:
    typedef enum { baerOk, baerUnknowIpNum, 
        baerUnknowProjId, baerLowSeverity } TBtaAddEventRes;
private:
  // correlation parameters
    TInt MnSeverity; // put lower weight to alarms below this severity
    TInt MxEvents; // number of the most recent events to keep in the correlation tables
    TInt MxEventWnd; // size of thw window within which the events are considered correlate
    TUInt64 MxTmMSecWnd; // window size in terms of time (not number of events as higher)
   
  // alarms
    TInt IpNumIdEvents; // number of events caused by devices
    TInt ProjIdEvents; // number of events caused by projects
    TIntIntVH IpNumIdToEventVH; // IpNum -> List of alarms issed by device
    TIntIntVH ProjIdToEventVH; // Proj -> List of alarms issed by project
  // correlated alarm pairs 
    TInt IpNumIdPrEvents; // number of correlated (IpNum,IpNum) pairs
    TInt ProjIdPrEvents; // number of correlated (Proj, Proj) pairs    
    // correlations (a,b) always orderd such that time(a) < time(b)
    TIntPrIntPrVH IpNumIdPrToEventPrVH; // (IpNum,CorrIpNum) -> {(Event1,Event2), ...}
    TIntPrIntPrVH ProjIdPrToEventPrVH; // (Proj,CorrProj) -> {(Event1,Event2), ...}
    TIntIntHH IpNumIdToCorrIpNumIdFqHH; // IpNum -> (CorrIpNum -> Fq)
    TIntIntHH ProjIdToCorrProjIdFqHH; // Proj -> (CorrProj -> Fq)
  // backward correlation counts -- #times device correlates with future device
    TIntIntHH CorrIpNumIdToIpNumIdFqHH; // CorrIpNum -> (IpNum -> Fq)
    TIntIntHH CorrProjIdToProjIdFqHH; // CorrProj -> (Proj -> Fq)
  // mappings    
    TIntIntH IpNumIdToProjIdH; // IpNum->Project map
    TIntIntVH ProjIdToIpNumIdVH; // Project->IpNumV map    
    TIntIntVH CaseIdToEventIdVH; // CaseId->EventIdV
  // recent alarms
    TInt NextEventN; // next free place in the recent events buffer
    TIntV RecentEventV; // circular buffer of recent events being added

private:
    int SumFqV(const TIntH& IdFqH) const;
    int CountEvents(const TIntV& EventV, const int& MxEventId) const;
    int CountEvents(const TIntPrV& EventPrV, const int& MxEventId) const;

public:
    TBtaCorrBs(const int& _MnSeverity, const int& _MxEvents, 
        const int& _MxEventWnd, const int64& _MxTmMSecWnd);
    static PBtaCorrBs New(const int& MnSeverity, const int& MxEvents, 
        const int& MxEventWnd, const int64& MxTmMSecWnd) {
            return new TBtaCorrBs(MnSeverity, MxEvents, MxEventWnd, MxTmMSecWnd); }

    TBtaCorrBs(TSIn& SIn);
    static PBtaCorrBs Load(TSIn& SIn) { return new TBtaCorrBs(SIn); }
    void Save(TSOut& SOut) const;
  
  // adding new events
    void AddEventBs(const PBtaEventBs& BtaEventBs);
    TBtaAddEventRes AddEvent(const PBtaEventBs& BtaEventBs, const int& EventN);
    TStr GetErrorMsgStr(const TBtaAddEventRes& ErrorCode) const;

  // correlation pairs
    const TIntPrIntPrVH& GetIpNumIdPrH() const { return IpNumIdPrToEventPrVH; }
    const TIntPrIntPrVH& GetProjIdPrH() const { return ProjIdPrToEventPrVH; }

  // IpNum correlations
    bool IsIpNumId(const int& IpNumId) const { 
        return IpNumIdToCorrIpNumIdFqHH.IsKey(IpNumId); }
    bool IsCorrIpNumId(const int& CorrIpNumId) const { 
        return CorrIpNumIdToIpNumIdFqHH.IsKey(CorrIpNumId); }
    bool IsIpNumIdPrCorr(const int& IpNumId, const int& CorrIpNumId) const {
        return IpNumIdPrToEventPrVH.IsKey(TIntPr(IpNumId, CorrIpNumId)); }
    // IpNum -> IpNum correlation
    int GetIpNumCorrIpNums(const int& IpNumId) const {
        return IpNumIdToCorrIpNumIdFqHH.GetDat(IpNumId).Len(); }
    int GetIpNumCorrIpNumFqs(const int& IpNumId) const {
        return SumFqV(IpNumIdToCorrIpNumIdFqHH.GetDat(IpNumId)); }
    int GetCorrIpNumIpNumFqs(const int& CorrIpNumId) const {
        return SumFqV(CorrIpNumIdToIpNumIdFqHH.GetDat(CorrIpNumId)); }
    const TIntH& GetIpNumCorrIpNumH(const int& IpNumId) const {
        return IpNumIdToCorrIpNumIdFqHH.GetDat(IpNumId); }
    const TIntH& GetCorrIpNumIpNumH(const int& IpNumId) const {
        return CorrIpNumIdToIpNumIdFqHH.GetDat(IpNumId); }
    const TIntV& GetIpNumEventV(const int& IpNumId) const {
        return IpNumIdToEventVH.GetDat(IpNumId); }
    const TIntPrV& GetIpNumCorrIpNumPrEventV(const int& IpNumId, const int& CorrIpNumId) const {
        return IpNumIdPrToEventPrVH.GetDat(TIntPr(IpNumId, CorrIpNumId)); }
    // IpNum correlation probabilities
    double ProbIpNumId(const int& IpNumId) const;
    double ProbIpNumIdAndCorrIpNumId(const int& IpNumId, const int& CorrIpNumId) const;
    double ProbCorrIpNumIdGivenIpNumId(const int& IpNumId, const int& CorrIpNumId) const;
    double GetIpNumMxCorrIpNumProb(const int& IpNumId) const;
    // ignores events past MxEvent
    double ProbIpNumId(const int& IpNumId, const int& MxEvent) const;
    double ProbIpNumIdAndCorrIpNumId(const int& IpNumId, 
        const int& CorrIpNumId, const int& MxEvent) const;
    double ProbCorrIpNumIdGivenIpNumId(const int& IpNumId, 
        const int& CorrIpNumId, const int& MxEvent) const;

  // Project correlations
    bool IsProjId(const int& ProjId) const { 
        return ProjIdToCorrProjIdFqHH.IsKey(ProjId); }
    bool IsCorrProjId(const int& CorrProjId) const { 
        return CorrProjIdToProjIdFqHH.IsKey(CorrProjId); }
    // Project -> Project correlation (for differnet projects)
    int GetProjCorrProjs(const int& ProjId) const { 
        return ProjIdToCorrProjIdFqHH.GetDat(ProjId).Len(); }
    int GetProjCorrProjFqs(const int& ProjId) const { 
        return SumFqV(ProjIdToCorrProjIdFqHH.GetDat(ProjId)); }
    int GetCorrProjProjFqs(const int& CorrProjId) const {
        return SumFqV(CorrProjIdToProjIdFqHH.GetDat(CorrProjId)); }
    const TIntV& GetProjEventV(const int& ProjId) const {
        return ProjIdToEventVH.GetDat(ProjId); }
    const TIntH& GetProjCorrProjH(const int& ProjId) const {
        return ProjIdToCorrProjIdFqHH.GetDat(ProjId); }

    double GetProjCorrProjProb(const int& ProjId, const int& CorrProjId) const;
    double GetProjMxCorrProjProb(const int& ProjId) const;

    // Map of IpNum to project names
    bool IsIpNumProj(const int& IpNumId) const {
        return IpNumIdToProjIdH.IsKey(IpNumId); }
    int GetIpNumProjId(const int& IpNumId) const {
        return IpNumIdToProjIdH.GetDat(IpNumId); }

    // Map of a project to devices
    bool IsProjIpNums(const int& ProjId) const {
        return ProjIdToIpNumIdVH.IsKey(ProjId); }
    const TIntV& GetProjIpNumV(const int& ProjId) const {
        return ProjIdToIpNumIdVH.GetDat(ProjId); }

    // events
    int GetIpNumIdEvents(const int& IpNumId) const {
        return IpNumIdToEventVH.GetDat(IpNumId).Len(); }
    int GetProjIdEvents(const int& ProjId) const {
        return ProjIdToEventVH.GetDat(ProjId).Len(); }

    // case-ids
    const TIntIntVH& GetCaseIdH() const { return CaseIdToEventIdVH; }

    // correlations
    void GetIpNumCorrDevices(const int& IpNumId, const bool& SameProjP, 
        const int& MxItems, TIntFltKdV& CorrIpNumIdProbV) const;
    void GetProjCorrDevices(const int& ProjId, 
        const int& MxItems, TIntPrFltKdV& CorrIpNumIdPrProbV) const;

    // predictions
    void GetNextIpNumIdProb(const int& LastEventN, const int& ProjId,
        const PBtaEventBs& BtaEventBs, const int& MxEventWnd, 
        const int64& MxTmMSecWnd, TIntFltH& IpNumIdProbH) const;
    void GetListOfFutureIpNums(const PBtaEventBs& BtaEventBs, const int& LastEventN, 
        const int& MxEventWnd, const int64& MxTmMSecWnd, TIntH& FutureIpNumH) const;
    void GetPredStat(const PBtaEventBs& BtaEventBs, const int& MxEvents, 
        const int& MxEventWnd, const int64& MxTmMSecWnd, const int& MxEvalWnd) const;

    // root-cause
    void EventRootCauseProb(const PBtaEventBs& BtaEventBs, 
        const int& EventId, const int& RootCouseEventWnd, 
        const int64& RootCouseTimeWnd, TIntFltKdV& EventIdScoreV) const;
    void EventRootCauseText(const PBtaEventBs& BtaEventBs, 
        const int& EventId,const int& RootCouseEventWnd, 
        const int64& RootCouseTimeWnd, TIntFltKdV& EventIdScoreV) const;
    void EventRootCause(const PBtaEventBs& BtaEventBs, const int& EventId, 
        const int& RootCouseEventWnd, const int64& RootCouseTimeWnd,
        TIntFltKdV& EventIdScoreV, TStrV& CorrTypeV) const;

    // statistics
    void GetRootCauseStat(const PBtaEventBs& BtaEventBs, 
        const int& MxEvents, const int& MnSeverity, const int& RootCouseEventWnd, 
        const int64& RootCouseTimeWnd, const bool& PropP, 
        const bool& TextP, TIntV& AllScoresCountV, TIntV& ProbScoresCountV,
        TIntV& TextScoresCountV, int& AllEvents) const;
    void GetStat(const PBtaEventBs& BtaEventBs) const;

    // files
    static PBtaCorrBs LoadBin(const TStr& FNm) {
        TFIn FIn(FNm); return TBtaCorrBs::Load(FIn); }
    void SaveBin(const TStr& FNm) { TFOut FOut(FNm); Save(FOut); }
};

/////////////////////////////////////////////////
// BTAlarms-LongTerm-Project-Stats
ClassTP(TBtaLongProj, PBtaLongProj)// {
private:
    TIntIntHH DayToIpNumIdFqHH;
    TIntIntPrVH DayToIpNumIdFqVH;

    void GetCountV(const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, TIntH& IpNumIdH, TFltV& OldV, TFltV& NewV) const;
    
public:
    TBtaLongProj() { }
    static PBtaLongProj New() { return new TBtaLongProj(); }    

    TBtaLongProj(TSIn& SIn) { DayToIpNumIdFqVH.Load(SIn); }
    static PBtaLongProj Load(TSIn& SIn){ return new TBtaLongProj(SIn); }
    void Save(TSOut& SOut) const { DayToIpNumIdFqVH.Save(SOut); }

    void AddEvent(const uint64& DateMSecs, const int& IpNumId);
    void Def();

    void Count(const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, int& BeforeCount, int& AfterCount) const;
    double ChiSquare(const TTm& StartDate, const TTm& BreakDate, const TTm& EndDate) const;
    bool IsTrendPos(const TTm& StartDate, const TTm& BreakDate, const TTm& EndDate) const;

    void GetIpNumWgtV(const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, TIntFltKdV& IpNumIdWgtV) const;

    static double GetNormFact(const TTm& StartDate, 
        const TTm& BreakDate, const TTm& EndDate);
};

/////////////////////////////////////////////////
// BTAlarms-LongTerm-Base
ClassTP(TBtaLongBs, PBtaLongBs)//{
private:
    THash<TInt, PBtaLongProj> ProjIdToProfileH;

public:
    TBtaLongBs(const PBtaEventBs& BtaEventBs, 
        const PBtaCorrBs& BtaCorrBs, const int& MnSeverity);
    static PBtaLongBs New(const PBtaEventBs& BtaEventBs, const PBtaCorrBs& BtaCorrBs,
        const int& MnSeverity) { return new TBtaLongBs(BtaEventBs, BtaCorrBs, MnSeverity); }
   
    TBtaLongBs(TSIn& SIn) { printf("Loading trends ... "); ProjIdToProfileH.Load(SIn); printf("(1)\n"); }
    static PBtaLongBs Load(TSIn& SIn) { return new TBtaLongBs(SIn); }
    void Save(TSOut& SOut) const { ProjIdToProfileH.Save(SOut); }

    int FFirstKeyId() const { return ProjIdToProfileH.FFirstKeyId(); }
    bool FNextKeyId(int& KeyId) const { return ProjIdToProfileH.FNextKeyId(KeyId); }
    int GetProjId(const int& KeyId) const { return ProjIdToProfileH.GetKey(KeyId); }
    PBtaLongProj GetLongProj(const int& KeyId) const { 
        return ProjIdToProfileH[KeyId]; }
    PBtaLongProj GetLongProjFromId(const int& ProjId) const { 
        return ProjIdToProfileH.GetDat(ProjId); }

    static PBtaLongBs LoadBin(const TStr& FNm){
        TFIn FIn(FNm); return TBtaLongBs::Load(FIn); }
    void SaveBin(const TStr& FNm){
        TFOut FOut(FNm); Save(FOut);}
};

#endif
