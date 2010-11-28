/////////////////////////////////////////////////
// Includes
#include "btalarms.h"

/////////////////////////////////////////////////
// BTAlarms-Device-Definition-Base
PBtaDevDefBs TBtaDevDefBs::LoadTxt(const TStr& FNm){
  PBtaDevDefBs DevDefBs=TBtaDevDefBs::New();
  printf("Loading Device Definitions from '%s' ...\n", FNm.CStr());
  PSIn SIn=TFIn::New(FNm);
  char SsCh=' '; TStrV FldNmV;
  // prepare data-fields meta-data
  TSs::LoadTxtFldV(ssfTabSep, SIn, SsCh, FldNmV, false, false);
  int IpNum_FldN=FldNmV.SearchForw("IP");
  int Node_FldN=FldNmV.SearchForw("Node");
  int Zone_FldN=FldNmV.SearchForw("Zone");
  int Loc_FldN=FldNmV.SearchForw("Location");
  int Proj_FldN=FldNmV.SearchForw("Project");
  int Type_FldN=FldNmV.SearchForw("Type");
  // traverse records
  TStrV FldValV; int Recs=0;
  while (!SIn->Eof()){
    Recs++; printf("  %d\r", Recs);
    // get alarm-records
    TSs::LoadTxtFldV(ssfTabSep, SIn, SsCh, FldValV, false, false);
    IAssert(FldNmV.Len()==FldValV.Len());
    // get fields-values
    TStr IpNumNm=FldValV[IpNum_FldN].GetTrunc();
    TStr NodeNm=FldValV[Node_FldN].GetTrunc();
    TStr ZoneNm=FldValV[Zone_FldN].GetTrunc();
    TStr LocNm=FldValV[Loc_FldN].GetTrunc();
    TStr ProjNm=FldValV[Proj_FldN].GetTrunc();
    TStr TypeNm=FldValV[Type_FldN].GetTrunc();
    // get value-id
    int IpNumId=-1; int NodeId=-1; int ZoneId=-1;
    int LocId=-1; int ProjId=-1; int TypeId=-1;
    if (!IpNumNm.Empty()){IpNumId=DevDefBs->AddIpNumNm(IpNumNm);}
    if (!NodeNm.Empty()){NodeId=DevDefBs->AddNodeNm(NodeNm);}
    if (!ZoneNm.Empty()){ZoneId=DevDefBs->AddZoneNm(ZoneNm);}
    if (!LocNm.Empty()){LocId=DevDefBs->AddLocNm(LocNm);}
    if (!ProjNm.Empty()){ProjId=DevDefBs->AddProjNm(ProjNm);}
    if (!TypeNm.Empty()){TypeId=DevDefBs->AddTypeNm(TypeNm);}
    // create and add device definition
    PBtaDevDef DevDef=
     TBtaDevDef::New(IpNumId, NodeId, ZoneId, LocId, ProjId, TypeId);
    if (!DevDefBs->IsDevDef(DevDef)){
      DevDefBs->AddDevDef(DevDef);
    } else {
      printf("  Duplicate [%s]\n", IpNumNm.CStr());
    }
  }
  printf("\nDone.\n");
  return DevDefBs;
}

/////////////////////////////////////////////////
// BTAlarms-Event-Data
TStr TBtaEventDat::GetNodeId() const {
    return IsFld("NODE") ? GetFld("NODE") : "&nbsp;";
}

TStr TBtaEventDat::GetComponentId() const {
    return IsFld("COMPONENTID") ? GetFld("COMPONENTID") : "&nbsp;";
}

TStr TBtaEventDat::GetFirstTm() const {
    return IsFld("FIRSTOCCURRENCE") ? GetFld("FIRSTOCCURRENCE") : "&nbsp;";
}

TStr TBtaEventDat::GetLastTm() const {
    return IsFld("LASTOCCURRENCE") ? GetFld("LASTOCCURRENCE") : "&nbsp;";
}

TStr TBtaEventDat::GetSeverity() const {
    return IsFld("SEVERITY") ? GetFld("SEVERITY") : "&nbsp;";
}

TStr TBtaEventDat::GetSummary() const {
    return IsFld("SUMMARY") ? GetFld("SUMMARY") : "&nbsp;";
}

TStr TBtaEventDat::GetIR() const {
    return IsFld("IR") ? GetFld("IR") : "&nbsp;";
}

/////////////////////////////////////////////////
// BTAlarms-Event
TBtaEvent::TBtaEvent() {}

TBtaEvent::TBtaEvent(const int& _IpNumId, const int& _ProjId, const int& _Severity,
    const uint64& _FirstTmMSecs, const uint64& _LastTmMSecs, const int& _CaseId, 
    const int& _BtaEventDatPt): 
        IpNumId(_IpNumId), ProjId(_ProjId), Severity(_Severity), FirstTmMSecs(_FirstTmMSecs), 
        LastTmMSecs(_LastTmMSecs), CaseId(_CaseId), BtaEventDatPt(_BtaEventDatPt) { }

TBtaEvent::TBtaEvent(TSIn& SIn): 
    IpNumId(SIn), 
    ProjId(SIn), 
    Severity(SIn), 
    FirstTmMSecs(SIn), 
    LastTmMSecs(SIn), 
    CaseId(SIn), 
    BtaEventDatPt(SIn) { }

void TBtaEvent::Save(TSOut& SOut) const { 
  IpNumId.Save(SOut); 
  ProjId.Save(SOut); 
  Severity.Save(SOut);
  FirstTmMSecs.Save(SOut); 
  LastTmMSecs.Save(SOut); 
  CaseId.Save(SOut); 
  BtaEventDatPt.Save(SOut); 
}

/////////////////////////////////////////////////
// BTAlarms-Event-Base
void TBtaEventBs::LoadValidColumnH() {
    ValidColumnH.AddKey("IDENTIFIER");
    ValidColumnH.AddKey("SERIAL");
    ValidColumnH.AddKey("NODEALIAS");
    ValidColumnH.AddKey("ALERTKEY");
    ValidColumnH.AddKey("SEVERITY");
    ValidColumnH.AddKey("SUMMARY");
    ValidColumnH.AddKey("FIRSTOCCURRENCE");
    ValidColumnH.AddKey("LASTOCCURRENCE");
    ValidColumnH.AddKey("TALLY");
    ValidColumnH.AddKey("PROJECT");
    ValidColumnH.AddKey("CLASS");
    ValidColumnH.AddKey("IP");
    ValidColumnH.AddKey("SERVERSERIAL");
    ValidColumnH.AddKey("IR");
    ValidColumnH.AddKey("NODE");
    ValidColumnH.AddKey("ALERTGROUP");
    ValidColumnH.AddKey("MANAGER");
    ValidColumnH.AddKey("AGENT");
    ValidColumnH.AddKey("SUPPRESS");
    ValidColumnH.AddKey("BUSINESSCRITICALITY");
    ValidColumnH.AddKey("COMPONENTID");
    ValidColumnH.AddKey("ALERTITEM");
    ValidColumnH.AddKey("STATECHANGE");
    ValidColumnH.AddKey("LOCATION");
    ValidColumnH.AddKey("OWNERUID");
    ValidColumnH.AddKey("ACKNOWLEDGED");
    ValidColumnH.AddKey("ZONE");
    ValidColumnH.AddKey("SERVERNAME");
    ValidColumnH.AddKey("HELP");
}

void TBtaEventBs::LoadLastEventId() {
    LastEventId = -1;
    // get number of elements in the dump_buffer
    {TStr CountDumpSqlStr = "select count(id) from dump_buffer;";
    if (!Query->ExeSql(CountDumpSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
    POdbcTb CountDumpTb = TOdbcTb::New(Query);
    EAssert(CountDumpTb->GetCols() == 1 && CountDumpTb->GetRows() == 1);
    const int CountDump = CountDumpTb->GetRowVal(0, 0).GetInt();
    // if we have any elements in there we check their max id
    if (CountDump > 0) { 
        TStr MaxIdDumpSqlStr = "select max(id) from dump_buffer;";
        if (!Query->ExeSql(MaxIdDumpSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
        POdbcTb MaxIdDumpTb = TOdbcTb::New(Query);
        EAssert(CountDumpTb->GetCols() == 1 && CountDumpTb->GetRows() == 1);
        const int MaxIdDump = MaxIdDumpTb->GetRowVal(0, 0).GetInt();
        LastEventId = TInt::GetMx(LastEventId, MaxIdDump);
    }}
    // get number of elements in the alarms
    {TStr CountAlarmsSqlStr = "select count(id) from alarms;";
    if (!Query->ExeSql(CountAlarmsSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
    POdbcTb CountAlarmsTb = TOdbcTb::New(Query);
    EAssert(CountAlarmsTb->GetCols() == 1 && CountAlarmsTb->GetRows() == 1);
    const int CountAlarms = CountAlarmsTb->GetRowVal(0, 0).GetInt();
    // if we have any elements in there we check their max id
    if (CountAlarms > 0) { 
        TStr MaxIdAlarmsSqlStr = "select max(id) from alarms;";
        if (!Query->ExeSql(MaxIdAlarmsSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
        POdbcTb MaxIdAlarmsTb = TOdbcTb::New(Query);
        EAssert(MaxIdAlarmsTb->GetCols() == 1 && MaxIdAlarmsTb->GetRows() == 1);
        const int MaxIdAlarms = MaxIdAlarmsTb->GetRowVal(0, 0).GetInt();
        LastEventId = TInt::GetMx(LastEventId, MaxIdAlarms);
    }}
    // we found it
    printf("LastEventId = %d\n", LastEventId.Val);
}

void TBtaEventBs::ParseBtaEventDat(const TBtaEventDat& EventDat, 
        const int& BtaEventDatPt, TBtaEvent& Event) {

    // parse the fields
    const TStrStrH FldNmValH = EventDat.GetFldNmToValH();
    TStr IpNumNm = FldNmValH.IsKey("IP") ? FldNmValH.GetDat("IP") : "";
    TStr FirstOccTmStr = FldNmValH.IsKey("FIRSTOCCURRENCE") ? FldNmValH.GetDat("FIRSTOCCURRENCE") : "";
    TStr LastOccTmStr = FldNmValH.IsKey("LASTOCCURRENCE") ? FldNmValH.GetDat("LASTOCCURRENCE") : "";
    TStr ProjNm = FldNmValH.IsKey("PROJECT") ? FldNmValH.GetDat("PROJECT") : "";
    TStr SeverityStr = FldNmValH.IsKey("SEVERITY") ? FldNmValH.GetDat("SEVERITY") : "";
    TStr CaseNm = FldNmValH.IsKey("IR") ? FldNmValH.GetDat("IR") : "";
    // parse first time
    uint64 FirstOccTmMSecs = 0;
    try {
        TTm FirstOccTm = TTm::GetTmFromWebLogDateTimeStr(FirstOccTmStr);
        EAssert(FirstOccTm.IsDef());
        FirstOccTmMSecs = TTm::GetMSecsFromTm(FirstOccTm);
    } catch (...) { }
    // parse last time
    uint64 LastOccTmMSecs = 0;
    try {
        TTm LastOccTm = TTm::GetTmFromWebLogDateTimeStr(LastOccTmStr);
        EAssert(LastOccTm.IsDef());
        LastOccTmMSecs = TTm::GetMSecsFromTm(LastOccTm);
    } catch (...) { }
    // get ids
    const int IpNumId = DevDefBs->IsIpNumId(IpNumNm) ? DevDefBs->GetIpNumId(IpNumNm) : -1;
    const int ProjId = DevDefBs->IsProjId(ProjNm) ? DevDefBs->GetProjId(ProjNm) : -1;
    const int Severity = SeverityStr.IsInt() ? SeverityStr.GetInt() : -1;
    const int CaseId = CaseNm.Empty() ? -1 : AddCase(CaseNm);
    // create & save event
    Event = TBtaEvent(IpNumId, ProjId, Severity,
        FirstOccTmMSecs, LastOccTmMSecs, CaseId, BtaEventDatPt);
}

int TBtaEventBs::StoreEventDat(const TBtaEventDat& EventDat, const TStr& TableNm) {
    // generate unique id for event
    const int EventId = GetNextEventId();
    // prepare SQL statement
    TChA ColumnNmChA, ColumnValChA;
    const TStrStrH& FldNmToValH = EventDat.GetFldNmToValH();
    int KeyId = FldNmToValH.FFirstKeyId();
    while (FldNmToValH.FNextKeyId(KeyId)) {
        const TStr& FldNm = FldNmToValH.GetKey(KeyId);
        const TStr& FldVal = FldNmToValH[KeyId];
        if (IsValidColumn(FldNm)) { 
            if (!ColumnNmChA.Empty()) { 
                ColumnNmChA += ", "; 
                ColumnValChA += ", ";
            }
            ColumnNmChA += FldNm;
            ColumnValChA += '"'; 
            ColumnValChA += TOdbcQuery::GetSqlStr(FldVal); 
            ColumnValChA += '"'; 
        }
    }
    TStr InsertSqlStr = TStr::Fmt("INSERT INTO %s (Id, %s) VALUES (%d, %s);",
        TableNm.CStr(), ColumnNmChA.CStr(), EventId, ColumnValChA.CStr());
    // execute the sql statement
    if (!Query->ExeSql(InsertSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
    // return the pointer to the row in the table
    return EventId;
}

bool TBtaEventBs::GetEventDat(const int& EventDatPt, 
        const TStr& TableNm, TBtaEventDat& EventDat) const {

    // select the requested row
    TStr SelectSqlStr = TStr::Fmt(
        "SELECT * FROM %s WHERE id=%d LIMIT 1;", TableNm.CStr(), EventDatPt);
    if (!Query->ExeSql(SelectSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
    // retrieve the results
    POdbcTb Table = TOdbcTb::New(Query);
    EAssertR(Table->IsOk(), Table->GetMsgStr());
    // parse the results
    EventDat.Clr();
    if (Table->GetRows() > 0) {
        for (int ColN = 0; ColN < Table->GetCols(); ColN++) {
            TStr ColNm = Table->GetColNm(ColN).GetUc();
            TStr RowVal = Table->GetRowVal(0, ColN);
            if (!RowVal.Empty()) { EventDat.AddFld(ColNm, RowVal); }
        }
    }
    return (Table->GetRows() > 0);
}


int TBtaEventBs::AddCase(const TStr& CaseNm) {
    const int CaseId = CaseNmFqH.AddKey(CaseNm);
    CaseNmFqH[CaseId]++;
    return CaseId;
}

TBtaEventBs::TBtaEventBs(const PBtaDevDefBs& _DevDefBs, 
        const POdbcDb& _OdbcDb, const int& ExpEvents) { 

    DevDefBs = _DevDefBs; 
    if (ExpEvents > 0) { EventV.Gen(ExpEvents, 0); }
    // remember the link to the database
    OdbcDb = _OdbcDb; Query = TOdbcQuery::New(OdbcDb);
    LoadValidColumnH(); LoadLastEventId();
}

TBtaEventBs::TBtaEventBs(TSIn& SIn, const POdbcDb& _OdbcDb) {
    printf("Loading events ... ");
    DevDefBs = PBtaDevDefBs(SIn); printf("(1)"); 
    EventV.Load(SIn); printf("(2)");
    CaseNmFqH.Load(SIn); printf("(3)\n");
    // remember the link to the database
    OdbcDb = _OdbcDb; Query = TOdbcQuery::New(OdbcDb);
    LoadValidColumnH(); LoadLastEventId();
}

void TBtaEventBs::Save(TSOut& SOut) const {
    DevDefBs.Save(SOut); 
    EventV.Save(SOut); 
    CaseNmFqH.Save(SOut);
}

void TBtaEventBs::GetEventDat(const int& EventDatPt, TBtaEventDat& EventDat) const {
    // first we try if the event is still in the dump buffer, 
    // the place where the latest event reside usualy
    const bool DumpBufferP = GetEventDat(EventDatPt, "dump_buffer", EventDat);
    // if not in there we go to the main table to check if it's there
    if (!DumpBufferP) { GetEventDat(EventDatPt, "alarms", EventDat); }    
}

int TBtaEventBs::GetNextEvent(const int& StartEventN, const int& IpNumId) {
    int EventN = StartEventN; const int Events = EventV.Len();
    while (EventN < Events) {
        if (EventV[EventN].GetIpNumId() == IpNumId) { return EventN; }
        EventN++;
    }
    return -1;
}

void TBtaEventBs::GetMostActiveProjs(const int64& TimeWnd, 
        const int& EventWnd, const int& MnSeverity, TIntKdV& ProjIdFqV) {

    TIntH ProjEventsH;
    // iterate over alarms and count their projects
    const int64 NowTmMSec = GetLastEvent().GetFirstTmMSecs();
    const int Events = GetEvents(); int ProcEventN = 0;
    for (int EventN = Events - 1; EventN >= 0; EventN--) {
        // load the event
        const TBtaEvent& Event = GetEvent(EventN);
        const int IpNumId = Event.GetIpNumId();
        const int ProjId = Event.GetProjId();
        const int64 EventTmMSec = Event.GetFirstTmMSecs();
        // first we check the time constraint
        if (EventTmMSec < NowTmMSec) { 
            const int64 TmMSecDiff = NowTmMSec - EventTmMSec;
            if (TmMSecDiff > TimeWnd) { break; }
        }
        // then some cheap stuff
        if (IpNumId == -1 || ProjId == -1) { continue; }
        if (Event.GetSeverity() < MnSeverity) { continue; }
        // check event window stoping critera
        if (ProcEventN > EventWnd) { break; }
        // all good
        ProjEventsH.AddDat(ProjId)++;
        // count this alarm as processed
        ProcEventN++;
    }
    // find the projects with most number severe alarms
    ProjEventsH.SortByDat(false); ProjIdFqV.Clr();
    int KeyId = ProjEventsH.FFirstKeyId();
    while (ProjEventsH.FNextKeyId(KeyId)) {
        const int ProjId = ProjEventsH.GetKey(KeyId);
        const int Events = ProjEventsH[KeyId];
        ProjIdFqV.Add(TIntKd(ProjId, Events));
    }
}

int TBtaEventBs::AddBtEvent(const TBtaEventDat& EventDat, const bool& BatchP) {
    // sotre the event data holder
    const int BtaEventDatPt = StoreEventDat(EventDat, BatchP ? "alarms" : "dump_buffer");
    // parse the event data
    TBtaEvent Event; ParseBtaEventDat(EventDat, BtaEventDatPt, Event);
    // add it to the list
    return AddEvent(Event);
}

void TBtaEventBs::AddNewBuffer(TIntV& EventIdV) {
    EventIdV.Clr();
    // retrieve all new alarms in blocks of 100
    forever {
        // select 100 events from new buffer
        TStr SelectSqlStr = TStr::Fmt("SELECT * FROM new_buffer LIMIT 100;");
        if (!Query->ExeSql(SelectSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
        // retrieve the results
        POdbcTb Table = TOdbcTb::New(Query);
        EAssertR(Table->IsOk(), Table->GetMsgStr());
        // if there are no more new alarms, stop
        const int Rows = Table->GetRows();
        const int Cols = Table->GetCols();
        if (Rows == 0) { break; }
        // iterate over new alarms, store them and move them to the dump-buffer
        for (int RowN = 0; RowN < Rows; RowN++) {
            TStr BufferId;
            TBtaEventDat EventDat;       
            for (int ColN = 0; ColN < Table->GetCols(); ColN++) {
                TStr ColNm = Table->GetColNm(ColN).GetUc();
                TStr RowVal = Table->GetRowVal(RowN, ColN);
                if (ColNm == "ID") { BufferId = RowVal; } 
                else if (!RowVal.Empty()) { EventDat.AddFld(ColNm, RowVal); }
            }
            // we add it in real-time mode, goes to the dump_buffer
            EventIdV.Add(AddBtEvent(EventDat, false));
            // delete event from the new_buffer
            TStr DeleteSqlStr = "DELETE FROM new_buffer WHERE id=" + BufferId + ";";
            if (!Query->ExeSql(DeleteSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
        }
    }
}
    
void TBtaEventBs::SaveDumpBuffer() {
    // first we copy content to the final table
    TStr InsertSqlStr = "INSERT INTO alarms SELECT * FROM dump_buffer;";
    if (!Query->ExeSql(InsertSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
    // then we delete all the content of the dump_buffer table
    TStr TruncateSqlStr = "TRUNCATE TABLE dump_buffer;";
    if (!Query->ExeSql(TruncateSqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
}

void TBtaEventBs::RescueDumpBuffer(TIntV& EventIdV) {
    // select all the events from dump_buffer
    TStr SqlStr = "SELECT * FROM dump_buffer;";
    if (!Query->ExeSql(SqlStr)) { TExcept::Throw(Query->GetMsgStr()); }
    // retrieve the results
    POdbcTb Table = TOdbcTb::New(Query);
    EAssertR(Table->IsOk(), Table->GetMsgStr());
    const int Rows = Table->GetRows();
    const int Cols = Table->GetCols();
    // if there are any in there, we move them to the event base
    printf("Found %d non-archived alarms from previous run.\n", Rows);
    EventIdV.Clr();
    for (int RowN = 0; RowN < Rows; RowN++) {
        // get the row
        TStr EventDatIdStr;
        TBtaEventDat EventDat;       
        for (int ColN = 0; ColN < Table->GetCols(); ColN++) {
            TStr ColNm = Table->GetColNm(ColN).GetUc();
            TStr RowVal = Table->GetRowVal(RowN, ColN);
            if (ColNm == "ID") { EventDatIdStr = RowVal; } 
            else if (!RowVal.Empty()) { EventDat.AddFld(ColNm, RowVal); }
        }
        EAssertR(EventDatIdStr.IsInt(), EventDatIdStr);
        // parse the row into an event
        TBtaEvent Event; 
        ParseBtaEventDat(EventDat, EventDatIdStr.GetInt(), Event);
        // add it to the lists
        EventIdV.Add(AddEvent(Event));
    }
}

PBtaEventBs TBtaEventBs::LoadBtTxt(const PBtaDevDefBs& DevDefBs, const TStr& TxtFNm, 
        const POdbcDb& OdbcDb, const int& ExpEvents, const int& Offset) { 

    printf("Loading BTAlarms-Data from '%s'...\n", TxtFNm.CStr());
    // create event-base
    PBtaEventBs EventBs = TBtaEventBs::New(DevDefBs, OdbcDb, ExpEvents);
    // prepare input & output data-file
    PSIn SIn = TFIn::New(TxtFNm); char SsCh=' ';
    // prepare data-fields meta-data
    TStrV FldNmV; TSs::LoadTxtFldV(ssfTabSep, SIn, SsCh, FldNmV, false, false);
    // traverse records
    TStrV FldValV; int Recs = 0;
    while (!SIn->Eof() && (EventBs->GetEvents() != ExpEvents)){
        Recs++; if (Recs%1000 == 0){printf("  %d / %d\r", EventBs->GetEvents(), Recs);}
        // get alarm-records
        TSs::LoadTxtFldV(ssfTabSep, SIn, SsCh, FldValV, false, false);
        IAssert(FldNmV.Len() == FldValV.Len());
        // we skip the alarm if we didn't reach the offset yet
        if (Offset > Recs) { continue; }
        // gather non-emtpy fields
        TBtaEventDat EventDat;
        for (int FldN = 0; FldN < FldValV.Len(); FldN++) {
            if (!FldValV[FldN].Empty()) { 
                EventDat.AddFld(FldNmV[FldN], FldValV[FldN]); }
        }
        // add it the enent to the event base
        EventBs->AddBtEvent(EventDat, true);
    }
    printf("  %d / %d \nDone.\n", EventBs->GetEvents(), Recs);
    return EventBs;
}

PBtaEventBs TBtaEventBs::LoadBankTxt(const PBtaDevDefBs& DevDefBs, 
        const TStr& InFPath, const POdbcDb& OdbcDb, const int& ExpEvents) {

    // no project, add default
    const int ProjId = DevDefBs->AddProjNm("default");
    // create event-base
    PBtaEventBs EventBs = TBtaEventBs::New(DevDefBs, OdbcDb, ExpEvents);
    // iterate over all the fils and load events from them
    TFFile FFile(InFPath, "", false); 
    TStr InFNm; int FileCount = 1;
    while (FFile.Next(InFNm)) {
        printf("(%d) Loading %s ...\n", FileCount, InFNm.CStr()); FileCount++;
        TFIn FIn(InFNm); int Events = 0;
        TStr LnStr, Rest1, Rest2;
        while (FIn.GetNextLn(LnStr)) {
            if (LnStr.IsWs()) { continue; }
            if (Events % 1000 == 0) { printf("%d\r", Events); }
            LnStr.ChangeStr("  ", " ");
            TStr MonthStr; LnStr.SplitOnCh(MonthStr, ' ', Rest1);
            TStr DayStr; Rest1.SplitOnCh(DayStr, ' ', Rest2);
            TStr TimeStr; Rest2.SplitOnCh(TimeStr, ' ', Rest1);
            TStr DeviceNm; Rest1.SplitOnCh(DeviceNm, ' ', Rest2);
            TStr EventTxt = Rest2;
            // parse time 
            const int Year = 2007;
            const int Month = TTmInfo::GetMonthN(MonthStr);
            if (Month == -1) { printf("MonthStr: %s\n", MonthStr.CStr()); continue; }
            if (!DayStr.IsInt()) { printf("DayStr: %s\n", DayStr.CStr()); continue; }
            const int Day = DayStr.GetInt();
            if (Day < 0 || Day > 31) { printf("Day: %d\n", Day); continue; }
            TStrV TimePartV; TimeStr.SplitOnAllCh(':', TimePartV);
            if (TimePartV.Len() != 3) { printf("TimePartV.Len: %d\n", TimePartV.Len()); continue; };
            if (!TimePartV[0].IsInt()) { printf("TimePartV[0]: %s\n", TimePartV[0].CStr()); continue; }
            if (!TimePartV[1].IsInt()) { printf("TimePartV[1]: %s\n", TimePartV[1].CStr()); continue; }
            if (!TimePartV[2].IsInt()) { printf("TimePartV[2]: %s\n", TimePartV[2].CStr()); continue; }
            const int Hour = TimePartV[0].GetInt();
            const int Minute = TimePartV[1].GetInt();
            const int Second = TimePartV[2].GetInt();
            if (Hour < 0 || Hour > 23) { printf("Hour: %d\n", Hour); continue; }
            if (Minute < 0 || Minute > 59) { printf("Minute: %d\n", Minute); continue; }
            if (Second < 0 || Second > 59) { printf("Second: %d\n", Second); continue; }
            TTm EventTime(Year, Month, Day, -1, Hour, Minute, Second);
            // parse device
            if (!DevDefBs->IsIpNumId(DeviceNm)) { DevDefBs->AddIpNumNm(DeviceNm); }
            const int IpNumId = DevDefBs->GetIpNumId(DeviceNm);
            const int Severity = 3;
            // create event data holder
            TBtaEventDat EventDat; 
            EventDat.AddFld("FIRSTOCCURRENCE", EventTime.GetWebLogDateTimeStr());
            EventDat.AddFld("LASTOCCURRENCE", EventTime.GetWebLogDateTimeStr());
            EventDat.AddFld("SEVERITY", TInt(Severity).GetStr());
            EventDat.AddFld("SUMMARY", EventTxt);
            const int BtaEventDatPt = EventBs->StoreEventDat(EventDat, "alarms");
            // add event
            TBtaEvent Event(IpNumId, ProjId, Severity, TTm::GetMSecsFromTm(EventTime), 
                TTm::GetMSecsFromTm(EventTime), -1, BtaEventDatPt);
            EventBs->AddEvent(Event); Events++;
        }
        printf("%d\n", Events);
    }
    printf("All Events: %d\n", EventBs->GetEvents());
    return EventBs;
}

/////////////////////////////////////////////////
// BTAlarms-Correlations-Base
int TBtaCorrBs::SumFqV(const TIntH& IdFqH) const {
    int Sum = 0, KeyId = IdFqH.FFirstKeyId();
    while (IdFqH.FNextKeyId(KeyId)) { Sum += IdFqH[KeyId]; }
    return Sum;
}

int TBtaCorrBs::CountEvents(const TIntV& EventV, const int& MxEventId) const {
    int Events = 0;
    for (int EventN = 0; EventN < EventV.Len(); EventN++) {
        if (EventV[EventN] <= MxEventId) { Events++; }
    }
    return Events;
}

int TBtaCorrBs::CountEvents(const TIntPrV& EventPrV, const int& MxEventId) const {
    int Events = 0;
    for (int EventPrN = 0; EventPrN < EventPrV.Len(); EventPrN++) {
        const TIntPr& Pr = EventPrV[EventPrN];
        if (Pr.Val1 <= MxEventId && Pr.Val2 <= MxEventId) { Events++; }
    }
    return Events;
}

TBtaCorrBs::TBtaCorrBs(const int& _MnSeverity, const int& _MxEvents, 
        const int& _MxEventWnd, const int64& _MxTmMSecWnd) {

    // set the correlation parameters
    MnSeverity = _MnSeverity; MxEvents = _MxEvents;
    MxEventWnd = _MxEventWnd; MxTmMSecWnd = _MxTmMSecWnd;
    // prepare recentcy window
    RecentEventV.Gen(MxEventWnd);
    RecentEventV.PutAll(-1);
    NextEventN = 0;
}

TBtaCorrBs::TBtaCorrBs(TSIn& SIn) {
    printf("Loading Correlation Parameters ... ");
    MnSeverity.Load(SIn); printf("<1>"); 
    MxEvents.Load(SIn); printf("<2>"); 
    MxEventWnd.Load(SIn); printf("<3>"); 
    MxTmMSecWnd.Load(SIn); printf("<4>\n"); 
    printf("Loading Alarm Counts ... ");
    IpNumIdEvents.Load(SIn); printf("<1>"); 
    ProjIdEvents.Load(SIn); printf("<2>");
    IpNumIdToEventVH.Load(SIn); printf("<3>"); 
    ProjIdToEventVH.Load(SIn); printf("<4>\n");
    printf("Loading Alarm Pair Counts ... ");
    IpNumIdPrEvents.Load(SIn); printf("<1>"); 
    ProjIdPrEvents.Load(SIn); printf("<2>\n");
    printf("Loading Alarm Correlations ... ");
    IpNumIdPrToEventPrVH.Load(SIn); printf("<1>"); 
    ProjIdPrToEventPrVH.Load(SIn); printf("<2>"); 
    IpNumIdToCorrIpNumIdFqHH.Load(SIn); printf("<3>"); 
    ProjIdToCorrProjIdFqHH.Load(SIn); printf("<4>\n"); 
    printf("Loading Alarm Backward Correlations ... ");
    CorrIpNumIdToIpNumIdFqHH.Load(SIn); printf("<1>"); 
    CorrProjIdToProjIdFqHH.Load(SIn); printf("<2>\n"); 
    printf("Loading Mappings ... ");
    IpNumIdToProjIdH.Load(SIn); printf("<1>"); 
    ProjIdToIpNumIdVH.Load(SIn); printf("<2>"); 
    CaseIdToEventIdVH.Load(SIn); printf("<3>\n"); 
    printf("Loading Recent Alarms ... ");
    NextEventN.Load(SIn); printf("<1>"); 
    RecentEventV.Load(SIn); printf("<2>\n");        
}
        
void TBtaCorrBs::Save(TSOut& SOut) const {
    MnSeverity.Save(SOut);  
    MxEvents.Save(SOut);  
    MxEventWnd.Save(SOut);  
    MxTmMSecWnd.Save(SOut);  
    IpNumIdEvents.Save(SOut);  
    ProjIdEvents.Save(SOut); 
    IpNumIdToEventVH.Save(SOut);  
    ProjIdToEventVH.Save(SOut); 
    IpNumIdPrEvents.Save(SOut);  
    ProjIdPrEvents.Save(SOut);  
    IpNumIdPrToEventPrVH.Save(SOut);  
    ProjIdPrToEventPrVH.Save(SOut);  
    IpNumIdToCorrIpNumIdFqHH.Save(SOut);  
    ProjIdToCorrProjIdFqHH.Save(SOut);  
    CorrIpNumIdToIpNumIdFqHH.Save(SOut);  
    CorrProjIdToProjIdFqHH.Save(SOut);  
    IpNumIdToProjIdH.Save(SOut);  
    ProjIdToIpNumIdVH.Save(SOut);  
    CaseIdToEventIdVH.Save(SOut);  
    NextEventN.Save(SOut);  
    RecentEventV.Save(SOut);  
}

void TBtaCorrBs::AddEventBs(const PBtaEventBs& BtaEventBs) {
    // traverse alarms and count frequency pairs
    printf("Processing events ...\n");
    int Events = BtaEventBs->GetEvents(), Skiped = 0; int64 MemUsed = 0;
    int MxEventN = (MxEvents == -1) ? Events : TInt::GetMn(MxEvents, Events);
    for (int EventN = 0; EventN < MxEventN; EventN++){
        // display progress
        if (EventN % 100000 == 0) {
            // shrot summury
            const TBtaEvent& Event = BtaEventBs->GetEvent(EventN);
            TStr TmStr = Event.GetFirstTm().GetWebLogDateTimeStr();
            printf("%dk / %dk (Dev: %dk, Corr: %dk, Date: %s)\n", 
                EventN/1000, MxEventN/1000, IpNumIdEvents/1000, 
                IpNumIdPrEvents/1000, TmStr.CStr());
        }
        if (EventN % 1000000 == 0) {
            // more detailed summury of what takes the memory
            printf("  IpNumIdToEventVH     : %5dk : %sB\n", IpNumIdToEventVH.Len()/1000, 
                TUInt64::GetMegaStr(IpNumIdToEventVH.GetMemUsed()).CStr());
            printf("  ProjIdToEventVH      : %6d : %sB\n", ProjIdToEventVH.Len(), 
                TUInt64::GetMegaStr(ProjIdToEventVH.GetMemUsed()).CStr());
            printf("  IpNumIdToProjIdH     : %5dk : %sB\n", IpNumIdToProjIdH.Len()/1000, 
                TUInt64::GetMegaStr(IpNumIdToProjIdH.GetMemUsed()).CStr());
            printf("  ProjIdToIpNumIdVH    : %6d : %sB\n", ProjIdToIpNumIdVH.Len(), 
                TUInt64::GetMegaStr(ProjIdToIpNumIdVH.GetMemUsed()).CStr());
            printf("  IpNumIdPrToEventPrVH : %5dk : %sB\n", IpNumIdPrToEventPrVH.Len()/1000, 
                TUInt64::GetMegaStr(IpNumIdPrToEventPrVH.GetMemUsed()).CStr());
            printf("  ProjIdPrToEventPrVH  : %6d : %sB\n", ProjIdPrToEventPrVH.Len(), 
                TUInt64::GetMegaStr(ProjIdPrToEventPrVH.GetMemUsed()).CStr());
        }
        // add event
        const TBtaAddEventRes Res = AddEvent(BtaEventBs, EventN);
        // count
        if (Res == baerUnknowIpNum || Res == baerUnknowProjId) { Skiped++; }
    }
    // final disiplay of statistics -- shrot summury
    const TBtaEvent& Event = BtaEventBs->GetEvent(MxEventN - 1);
    TStr TmStr = Event.GetFirstTm().GetWebLogDateTimeStr();
    printf("%dk / %dk (Dev: %dk, Corr: %dk, Date: %s)\n", 
        MxEventN/1000, MxEventN/1000, IpNumIdEvents/1000, 
        IpNumIdPrEvents/1000, TmStr.CStr());
    // more detailed summury of what takes the memory
    printf("  IpNumIdToEventVH     : %5dk : %sB\n", IpNumIdToEventVH.Len()/1000, 
        TUInt64::GetMegaStr(IpNumIdToEventVH.GetMemUsed()).CStr());
    printf("  ProjIdToEventVH      : %6d : %sB\n", ProjIdToEventVH.Len(), 
        TUInt64::GetMegaStr(ProjIdToEventVH.GetMemUsed()).CStr());
    printf("  IpNumIdToProjIdH     : %5dk : %sB\n", IpNumIdToProjIdH.Len()/1000, 
        TUInt64::GetMegaStr(IpNumIdToProjIdH.GetMemUsed()).CStr());
    printf("  ProjIdToIpNumIdVH    : %6d : %sB\n", ProjIdToIpNumIdVH.Len(), 
        TUInt64::GetMegaStr(ProjIdToIpNumIdVH.GetMemUsed()).CStr());
    printf("  IpNumIdPrToEventPrVH : %5dk : %sB\n", IpNumIdPrToEventPrVH.Len()/1000, 
        TUInt64::GetMegaStr(IpNumIdPrToEventPrVH.GetMemUsed()).CStr());
    printf("  ProjIdPrToEventPrVH  : %6d : %sB\n", ProjIdPrToEventPrVH.Len(), 
        TUInt64::GetMegaStr(ProjIdPrToEventPrVH.GetMemUsed()).CStr());
    printf("\nDone.\n");
}

TBtaCorrBs::TBtaAddEventRes TBtaCorrBs::AddEvent(
        const PBtaEventBs& BtaEventBs, const int& EventN) {

    // get event
    const TBtaEvent& Event = BtaEventBs->GetEvent(EventN);
    // get event parameters
    const int IpNumId = Event.GetIpNumId();
    const int ProjId = Event.GetProjId();
    const uint64 TmMSec = Event.GetFirstTmMSecs();
    // check if it talks about known stuff
    if (IpNumId == -1) { return baerUnknowIpNum; }
    if (ProjId == -1) { return baerUnknowProjId; }
    // alarms severe enough?
    const int Severity = Event.GetSeverity();
    if (Severity < MnSeverity) { return baerLowSeverity; }
    // remember event per device and per project level
    IpNumIdToEventVH.AddDat(IpNumId).Add(EventN); IpNumIdEvents++;
    ProjIdToEventVH.AddDat(ProjId).Add(EventN); ProjIdEvents++;
    // update the davice to project mapping 
    IpNumIdToProjIdH.AddDat(IpNumId) = ProjId;
    // update project to devices mapping
    ProjIdToIpNumIdVH.AddDat(ProjId).AddMerged(IpNumId);
    // update case id to event mapping
    const int CaseId = Event.GetCaseId();
    if (CaseId != -1) { CaseIdToEventIdVH.AddDat(CaseId).Add(EventN); }

    // count co-occurences from this alarm till min(MxAlarmWnd,MxTmMSecWnd)
    const int RecentEvents = RecentEventV.Len();
    for (int RecentEventN = 0; RecentEventN < RecentEvents; RecentEventN++) {
        const int CorrEventN = RecentEventV[RecentEventN];
        // if empty, we skip it
        if (CorrEventN == -1) { continue; }
        // get co-occuring alarm
        const TBtaEvent& CorrEvent = BtaEventBs->GetEvent(CorrEventN); 
        // check if still within the time window
        const uint64 CorrTmMSec = CorrEvent.GetFirstTmMSecs();
        if (MxTmMSecWnd > 0) { // do we have a time constraint?
            if ((TmMSec > CorrTmMSec) && ((TmMSec - CorrTmMSec) >  MxTmMSecWnd)) { continue; }
            if ((TmMSec < CorrTmMSec) && ((CorrTmMSec - TmMSec) >  MxTmMSecWnd)) { continue; }
        }        
        // we are still within the window, get event parameters
        const int CorrIpNumId = CorrEvent.GetIpNumId();
        const int CorrProjId = CorrEvent.GetProjId();
        // make sure again all is well...
        IAssertR((IpNumId != -1) && (CorrIpNumId != -1), TStr::Fmt("(%d,%d)", IpNumId, CorrIpNumId));
        IAssertR((ProjId != -1) && (CorrProjId != -1), TStr::Fmt("(%d,%d)", ProjId, CorrProjId));
        IAssert(CorrEvent.GetSeverity() >= MnSeverity);
        // count co-occurances of devices
        if (TmMSec < CorrTmMSec) { // make sure to get the direction in time right
            TIntPrV& EventPrV = IpNumIdPrToEventPrVH.AddDat(TIntPr(IpNumId, CorrIpNumId));
            EventPrV.Add(TIntPr(EventN, CorrEventN)); IpNumIdPrEvents++;
            IpNumIdToCorrIpNumIdFqHH.AddDat(IpNumId).AddDat(CorrIpNumId)++;
            CorrIpNumIdToIpNumIdFqHH.AddDat(CorrIpNumId).AddDat(IpNumId)++;
        } else {
            TIntPrV& EventPrV = IpNumIdPrToEventPrVH.AddDat(TIntPr(CorrIpNumId, IpNumId));
            EventPrV.Add(TIntPr(CorrEventN, EventN)); IpNumIdPrEvents++;
            IpNumIdToCorrIpNumIdFqHH.AddDat(CorrIpNumId).AddDat(IpNumId)++;
            CorrIpNumIdToIpNumIdFqHH.AddDat(IpNumId).AddDat(CorrIpNumId)++;
        }
        // count co-occurances of projects
        if (ProjId != CorrProjId) {
            if (TmMSec < CorrTmMSec) { // make sure to get the direction in time right
                TIntPrV& EventPrV = ProjIdPrToEventPrVH.AddDat(TIntPr(ProjId, CorrProjId));
                EventPrV.Add(TIntPr(EventN, CorrEventN)); ProjIdPrEvents++;
                ProjIdToCorrProjIdFqHH.AddDat(ProjId).AddDat(CorrProjId)++;
                CorrProjIdToProjIdFqHH.AddDat(CorrProjId).AddDat(ProjId)++;
            } else {
                TIntPrV& EventPrV = ProjIdPrToEventPrVH.AddDat(TIntPr(CorrProjId, ProjId));
                EventPrV.Add(TIntPr(CorrEventN, EventN)); ProjIdPrEvents++;
                ProjIdToCorrProjIdFqHH.AddDat(CorrProjId).AddDat(ProjId)++;
                CorrProjIdToProjIdFqHH.AddDat(ProjId).AddDat(CorrProjId)++;
            }
        }
    }

    // add current event among recent events
    RecentEventV[NextEventN] = EventN;
    // increase the count in the circular buffer of recent events
    NextEventN++; NextEventN = NextEventN % RecentEvents;
    // return that all is well in the state of Denmark;
    return baerOk;
}

TStr TBtaCorrBs::GetErrorMsgStr(const TBtaCorrBs::TBtaAddEventRes& ErrorCode) const {
    if (ErrorCode == TBtaCorrBs::baerLowSeverity) { return "To low severity!"; }
    if (ErrorCode == TBtaCorrBs::baerUnknowIpNum) { return "Unknown IpNum!"; }
    if (ErrorCode == TBtaCorrBs::baerUnknowProjId) { return "Unknown ProjId!"; }
    if (ErrorCode == TBtaCorrBs::baerOk) { return "Event OK!"; }
    return "Unknown error when adding event to CorrBs!";
}

double TBtaCorrBs::ProbIpNumId(const int& IpNumId) const {
    const int IpNumIds = IpNumIdToEventVH.Len();
    const int IpNumIdFq = IpNumIdToEventVH.IsKey(IpNumId) ?
        IpNumIdToEventVH.GetDat(IpNumId).Len() : 0;
    return double(IpNumIdFq + 1) / double(IpNumIdEvents + IpNumIds);
}

double TBtaCorrBs::ProbIpNumIdAndCorrIpNumId(const int& IpNumId, const int& CorrIpNumId) const {
    TIntPr IpNumPr(IpNumId, CorrIpNumId), RevIpNumPr(CorrIpNumId, IpNumId);
    const int IpNumIdPrFq = IpNumIdPrToEventPrVH.IsKey(IpNumPr) ?
        IpNumIdPrToEventPrVH.GetDat(IpNumPr).Len() : 0; 
    const int RevIpNumIdPrFq = IpNumIdPrToEventPrVH.IsKey(RevIpNumPr) ? 
        IpNumIdPrToEventPrVH.GetDat(RevIpNumPr).Len() : 0; 
    const int IpNumIds = IpNumIdToEventVH.Len();
    const int AllIpNumIdPrs = IpNumIds*IpNumIds;
    //const double Prior = double(AllIpNumIdPrs) * 
    //    ProbIpNumId(IpNumId) * ProbIpNumId(CorrIpNumId);
    const double Prior = 1.0;
    return (double(IpNumIdPrFq + RevIpNumIdPrFq) + Prior) / 
        double(IpNumIdPrEvents + AllIpNumIdPrs);
}

double TBtaCorrBs::ProbCorrIpNumIdGivenIpNumId(const int& IpNumId, const int& CorrIpNumId) const {
    // thresholds to avoid spurious correlations
    const int IpNumIdFq = IpNumIdToEventVH.GetDat(IpNumId).Len();
    const int CorrIpNumIdFq = IpNumIdToEventVH.GetDat(CorrIpNumId).Len();
    if (CorrIpNumIdFq / IpNumIdFq > 10) { return 0.0; }

    // calc probability
    TIntPr IpNumPr(IpNumId, CorrIpNumId);
    const int IpNumIdPrFq = IpNumIdPrToEventPrVH.GetDat(IpNumPr).Len(); 
    const int AllIpNumIdPrs = SumFqV(IpNumIdToCorrIpNumIdFqHH.GetDat(IpNumId));
    const int IpNumIds = IpNumIdToEventVH.Len();
    return (double(IpNumIdPrFq) + 1.0) / double(AllIpNumIdPrs + IpNumIds);
}

double TBtaCorrBs::GetIpNumMxCorrIpNumProb(const int& IpNumId) const {
    double MxCorrProb = 0.0;
    const TIntH& CorrIdFqH = GetIpNumCorrIpNumH(IpNumId);
    int CorrIdKeyId = CorrIdFqH.FFirstKeyId();
    while (CorrIdFqH.FNextKeyId(CorrIdKeyId)) {
        const int CorrIpNumId = CorrIdFqH.GetKey(CorrIdKeyId);
        if (IsCorrIpNumId(CorrIpNumId)) {
            const double CorrProb = ProbCorrIpNumIdGivenIpNumId(IpNumId, CorrIpNumId);
            MxCorrProb = TFlt::GetMx(CorrProb, MxCorrProb);
        }
    }
    return MxCorrProb;
}

double TBtaCorrBs::ProbIpNumId(const int& IpNumId, const int& MxEventId) const {
    const int IpNumIds = IpNumIdToEventVH.Len();
    const int IpNumIdFq = IpNumIdToEventVH.IsKey(IpNumId) ?
        CountEvents(IpNumIdToEventVH.GetDat(IpNumId), MxEventId) : 0;
    return double(IpNumIdFq + 1) / double((MxEventId + 1) + IpNumIds);
}

double TBtaCorrBs::ProbIpNumIdAndCorrIpNumId(const int& IpNumId, 
        const int& CorrIpNumId, const int& MxEventId) const {

    TIntPr IpNumPr(IpNumId, CorrIpNumId), RevIpNumPr(CorrIpNumId, IpNumId);
    const int IpNumIdPrFq = IpNumIdPrToEventPrVH.IsKey(IpNumPr) ? 
        CountEvents(IpNumIdPrToEventPrVH.GetDat(IpNumPr), MxEventId) : 0; 
    const int RevIpNumIdPrFq = IpNumIdPrToEventPrVH.IsKey(RevIpNumPr) ?
        CountEvents(IpNumIdPrToEventPrVH.GetDat(RevIpNumPr), MxEventId) : 0; 
    const int IpNumIds = IpNumIdToEventVH.Len();
    const int AllIpNumIdPrs = IpNumIds*IpNumIds;
    const double Prior = 1.0;
    return (double(IpNumIdPrFq + RevIpNumIdPrFq) + Prior) / 
        double(IpNumIdPrEvents + AllIpNumIdPrs);
}

double TBtaCorrBs::ProbCorrIpNumIdGivenIpNumId(const int& IpNumId, 
        const int& CorrIpNumId, const int& MxEventId) const {

    const int AllIpNumIdPrs = SumFqV(IpNumIdToCorrIpNumIdFqHH.GetDat(IpNumId));
    const int IpNumIds = IpNumIdToEventVH.Len();
    // thresholds to avoid spurious correlations
    const int IpNumIdFq = IpNumIdToEventVH.IsKey(IpNumId) ?
        CountEvents(IpNumIdToEventVH.GetDat(IpNumId), MxEventId) : 0;
    const int CorrIpNumIdFq = IpNumIdToEventVH.IsKey(CorrIpNumId) ?
        CountEvents(IpNumIdToEventVH.GetDat(CorrIpNumId), MxEventId) : 0;
    if ((IpNumIdFq == 0) || (CorrIpNumIdFq / IpNumIdFq > 10)) {
        // return prior
        return 1.0 / double(AllIpNumIdPrs + IpNumIds);
    } else {
        // calc probability
        TIntPr IpNumPr(IpNumId, CorrIpNumId);
        const int IpNumIdPrFq = IpNumIdPrToEventPrVH.IsKey(IpNumPr) ?
            CountEvents(IpNumIdPrToEventPrVH.GetDat(IpNumPr), MxEventId) : 0; 
        return (double(IpNumIdPrFq) + 1.0) / double(AllIpNumIdPrs + IpNumIds);
    }
}

double TBtaCorrBs::GetProjCorrProjProb(const int& ProjId, const int& CorrProjId) const {
    const int CorrFq = ProjIdPrToEventPrVH.GetDat(TIntPr(ProjId, CorrProjId)).Len();
    const double ForwardProb = double(CorrFq) / double(GetProjCorrProjFqs(ProjId));
    const double BackwardProb = double(CorrFq) / double(GetCorrProjProjFqs(CorrProjId));
    return ForwardProb * BackwardProb;
}

double TBtaCorrBs::GetProjMxCorrProjProb(const int& ProjId) const {
    double MxCorrProb = 0.0;
    const TIntH& CorrIdFqH = GetProjCorrProjH(ProjId);
    int CorrIdKeyId = CorrIdFqH.FFirstKeyId();
    while (CorrIdFqH.FNextKeyId(CorrIdKeyId)) {
        const int CorrProjId = CorrIdFqH.GetKey(CorrIdKeyId);
        if (IsCorrProjId(CorrProjId)) {
            const double CorrProb = GetProjCorrProjProb(ProjId, CorrProjId);
            MxCorrProb = TFlt::GetMx(CorrProb, MxCorrProb);
        }
    }
    return MxCorrProb;
}

void TBtaCorrBs::GetIpNumCorrDevices(const int& IpNumId, const bool& SameProjP, 
        const int& MxItems, TIntFltKdV& CorrIpNumIdProbV) const {

    // check if there is any work at all to do
    CorrIpNumIdProbV.Clr();
    if (!IsIpNumId(IpNumId) || !IsIpNumProj(IpNumId)) { return; }
    // initialize
    const int IpNumProjId = GetIpNumProjId(IpNumId);
    double MxCorrProb = 0.0;
    // find all the correlated devices with the specified Ip
    TVec<TIntBoolPr> CorrIpNumIdForwardV;
    // first one with correlated in the future
    {const TIntH& ForwardCorrIdFqH = GetIpNumCorrIpNumH(IpNumId);
    int ForwardCorrIdKeyId = ForwardCorrIdFqH.FFirstKeyId();
    while (ForwardCorrIdFqH.FNextKeyId(ForwardCorrIdKeyId)) {
        const int CorrIpNumId = ForwardCorrIdFqH.GetKey(ForwardCorrIdKeyId);
        CorrIpNumIdForwardV.Add(TIntBoolPr(CorrIpNumId, true));
    }}
    // then ones correltaed in the past
    {const TIntH& BackwardCorrIdFqH = GetCorrIpNumIpNumH(IpNumId);
    int BackwardCorrIdKeyId = BackwardCorrIdFqH.FFirstKeyId();
    while (BackwardCorrIdFqH.FNextKeyId(BackwardCorrIdKeyId)) {
        const int CorrIpNumId = BackwardCorrIdFqH.GetKey(BackwardCorrIdKeyId);
        CorrIpNumIdForwardV.Add(TIntBoolPr(CorrIpNumId, false));
    }}
    // iterate over them and calculate their probability if satisfies criteria
    TVec<TFltIntBoolPrKd> CorrProbIpNumIdV;
    for (int CorrIpNumIdN = 0; CorrIpNumIdN < CorrIpNumIdForwardV.Len(); CorrIpNumIdN++) {
        const int CorrIpNumId = CorrIpNumIdForwardV[CorrIpNumIdN].Val1;
        const bool ForwardP = CorrIpNumIdForwardV[CorrIpNumIdN].Val2;
        // ignor correlations with itself
        if (IpNumId == CorrIpNumId) { continue; }
        // check we have all necessary parameters of the correlated IpNumId
        if (IsCorrIpNumId(CorrIpNumId) && IsIpNumProj(CorrIpNumId)) {
            // check if the device corresponds to the project constraint (in or out)
            const int CorrIpNumProjId = GetIpNumProjId(CorrIpNumId);            
            if (SameProjP && (IpNumProjId != CorrIpNumProjId)) { continue; }
            if (!SameProjP && (IpNumProjId == CorrIpNumProjId)) { continue; }
            // get the probaility of correlation
            const double CorrProb = ForwardP ? 
                ProbCorrIpNumIdGivenIpNumId(IpNumId, CorrIpNumId) :
                ProbCorrIpNumIdGivenIpNumId(CorrIpNumId, IpNumId);
            if (CorrProb > 0.0) { 
                CorrProbIpNumIdV.Add(TFltIntBoolPrKd(
                    CorrProb, TIntBoolPr(CorrIpNumId, ForwardP))); 
            }
            // remember the maximal correlation for later normalization
            MxCorrProb = TFlt::GetMx(CorrProb, MxCorrProb);
        }
    }
    // sort according to probability
    CorrProbIpNumIdV.Sort(false);
    // prepare final result
    const int Items = TInt::GetMn(CorrProbIpNumIdV.Len(), MxItems);
    CorrIpNumIdProbV.Gen(Items, 0);
    for (int ItemN = 0; ItemN < Items; ItemN++) {
        const double CorrProb = CorrProbIpNumIdV[ItemN].Key;
        const int CorrIpNumId = CorrProbIpNumIdV[ItemN].Dat.Val1;
        const bool ForwardP = CorrProbIpNumIdV[ItemN].Dat.Val2;
        if ((MxCorrProb / CorrProb) < 10.0) {
            CorrIpNumIdProbV.Add(TIntFltKd(CorrIpNumId, 
                ForwardP ? CorrProb : -CorrProb));
        }
    }
}

void TBtaCorrBs::GetProjCorrDevices(const int& ProjId, 
        const int& MxItems, TIntPrFltKdV& CorrIpNumIdPrProbV) const {

    // check if there is any work at all to do
    CorrIpNumIdPrProbV.Clr();
    if (!IsProjIpNums(ProjId)) { return; }
    // prepare a list of all correlations
    double MxCorrProb = 0.0;
    TFltIntIntTrV CorrProbIpNumPrV;
    const TIntV& IpNumIdV = GetProjIpNumV(ProjId);
    for (int IpNumIdN = 0; IpNumIdN < IpNumIdV.Len(); IpNumIdN++) {
        // get the device id
        const int IpNumId = IpNumIdV[IpNumIdN];
        // get all the correlated devices in the future
        const TIntH& CorrIpFqH = GetIpNumCorrIpNumH(IpNumId);
        // iterate over them and calculate correlations
        int CorrIpKeyId = CorrIpFqH.FFirstKeyId(), CorrIpNums = 0;
        while (CorrIpFqH.FNextKeyId(CorrIpKeyId)) {
            const int CorrIpNumId = CorrIpFqH.GetKey(CorrIpKeyId);
            // ignor correlations with itself
            if (IpNumId == CorrIpNumId) { continue; }
            // check we have all necessary parameters of the correlated IpNumId
            if (IsCorrIpNumId(CorrIpNumId) && IsIpNumProj(CorrIpNumId)) {
                // check if the device is from the same project
                const int CorrIpNumProjId = GetIpNumProjId(CorrIpNumId);            
                if (ProjId != CorrIpNumProjId) { continue; }
                // get the probaility of correlation
                const double CorrProb = 
                    ProbCorrIpNumIdGivenIpNumId(IpNumId, CorrIpNumId);
                // remember if non-zero
                if (CorrProb > 0.0) { 
                    CorrProbIpNumPrV.Add(TFltIntIntTr(
                        CorrProb, IpNumId, CorrIpNumId)); 
                }
                // remember the maximal correlation for later normalization
                MxCorrProb = TFlt::GetMx(CorrProb, MxCorrProb);
                CorrIpNums++;
            }
        }
        // status
        printf("%d/%d [%5d / %5d]\r", IpNumIdN, IpNumIdV.Len(), CorrIpNums, CorrIpFqH.Len());
    }
    // sort according to probability
    CorrProbIpNumPrV.Sort(false);
    // prepare final result
    const int Items = TInt::GetMn(CorrProbIpNumPrV.Len(), MxItems);
    CorrIpNumIdPrProbV.Gen(Items, 0);
    for (int ItemN = 0; ItemN < Items; ItemN++) {
        const double CorrProb = CorrProbIpNumPrV[ItemN].Val1;
        const int IpNumId = CorrProbIpNumPrV[ItemN].Val2;
        const int CorrIpNumId = CorrProbIpNumPrV[ItemN].Val3;
        if ((MxCorrProb / CorrProb) < 10.0) {
            CorrIpNumIdPrProbV.Add(TIntPrFltKd(
                TIntPr(IpNumId, CorrIpNumId), CorrProb));
        }
    }
}

void TBtaCorrBs::GetNextIpNumIdProb(const int& LastEventN, const int& ProjId,
        const PBtaEventBs& BtaEventBs, const int& MxEventWnd, 
        const int64& MxTmMSecWnd, TIntFltH& IpNumIdProbH) const {

    IpNumIdProbH.Clr();
    // get history window
    TBtaEventV EventWndV;
    int EventWndN = LastEventN;
    while (EventWndN >= 0) {
        // get alarm
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventWndN); EventWndN--; 
        if ((Event.GetIpNumId() == -1) || (Event.GetProjId() == -1)){ continue; }
        if (Event.GetProjId() != ProjId) { continue; }
        // check if still within the window
        if (!EventWndV.Empty()) {
            const int CountDiff = LastEventN - EventWndN;
            const int64 TmMSecDiff = Event.GetFirstTmMSecs() - EventWndV[0].GetFirstTmMSecs();
            if ((MxEventWnd > 0) && (CountDiff > MxEventWnd)) { break; }
            if ((MxTmMSecWnd > 0) && (TmMSecDiff > MxTmMSecWnd)) { break; }
        }
        // if yes, we add it to the window
        EventWndV.Add(Event);
    }
    if (EventWndV.Empty()) { return; } // no valid events in the window...
    EventWndV.Reverse();

    // calculate probabilities for all possible triplets
    PBtaDevDefBs BtaDevDefBs = BtaEventBs->GetDevDefBs();
    // make a candidate list of all devices with previous 
    // correlations with devices from the window
    TIntH CandIpNumIdH;
    for (int IpNumId = 0; IpNumId < BtaDevDefBs->GetIpNums(); IpNumId++) {
        if (!IsIpNumProj(IpNumId)) { continue; };
        if (GetIpNumProjId(IpNumId) != ProjId) { continue; };
        for (int EventWndN = 0; EventWndN < EventWndV.Len(); EventWndN++) {
            const int PrevIpNumId = EventWndV[EventWndN].GetIpNumId();
            if (IsIpNumIdPrCorr(PrevIpNumId, IpNumId)) {
                CandIpNumIdH.AddDat(IpNumId) += 
                    GetIpNumCorrIpNumPrEventV(PrevIpNumId, IpNumId).Len();
            }
        }
    }
    if (CandIpNumIdH.Empty()) { return; } // no candidates...
    // make a list of all unique pairs of devices from the window
    TIntPrIntH WndIpNumPrH;
    for (int FirstEventWndN = 0; FirstEventWndN < EventWndV.Len(); FirstEventWndN++) {
        for (int SecondEventWndN = FirstEventWndN+1; SecondEventWndN < EventWndV.Len(); SecondEventWndN++) {
            TBtaEvent FirstEvent = EventWndV[FirstEventWndN];
            TBtaEvent SecondEvent = EventWndV[SecondEventWndN];
            TIntPr WndIpNumIdPr(FirstEvent.GetIpNumId(), SecondEvent.GetIpNumId());
            if (!WndIpNumPrH.IsKey(WndIpNumIdPr)) {
                WndIpNumPrH.AddDat(WndIpNumIdPr)++;
            }
        }
    }

    // calculate probabilities for all possible devices
    int CandKeyId = CandIpNumIdH.FFirstKeyId();
    const int IpNumIds = IpNumIdToEventVH.Len();
    const double IpNumIdPrs = double(IpNumIds*(IpNumIds - 1) / 2);
    const double IpNumIdTrs = double(IpNumIds*(IpNumIds - 1)*(IpNumIds - 2) / 6);
    while (CandIpNumIdH.FNextKeyId(CandKeyId)) {
        const int CandIpNumId = CandIpNumIdH.GetKey(CandKeyId);
        // calculate sum of conditional probabilities of candidate
        // given pairs of alarms from the window
        double CandProb = 0.0;
        int WndKeyId = WndIpNumPrH.FFirstKeyId();
        while (WndIpNumPrH.FNextKeyId(WndKeyId)) {
            const int FirstIpNumId = WndIpNumPrH.GetKey(WndKeyId).Val1;
            const int SecondIpNumId = WndIpNumPrH.GetKey(WndKeyId).Val2;
            IAssertR(FirstIpNumId != -1, TInt::GetStr(FirstIpNumId));
            IAssertR(SecondIpNumId != -1, TInt::GetStr(SecondIpNumId));
            const double FirstSecondProb = ProbIpNumIdAndCorrIpNumId(
                FirstIpNumId, SecondIpNumId, LastEventN);
            const double FirstCandProb = ProbIpNumIdAndCorrIpNumId(
                FirstIpNumId, CandIpNumId, LastEventN);
            const double SecondCandProb = ProbIpNumIdAndCorrIpNumId(
                SecondIpNumId, CandIpNumId, LastEventN);
            const double Prob = FirstSecondProb*FirstCandProb*SecondCandProb;
            CandProb += Prob;
        }
        // remember
        IpNumIdProbH.AddDat(CandIpNumId, CandProb);
    }   
}

void TBtaCorrBs::GetListOfFutureIpNums(const PBtaEventBs& BtaEventBs, 
        const int& LastEventN, const int& MxEventWnd, const int64& MxTmMSecWnd, 
        TIntH& FutureIpNumH) const {

    const int MxEvents = TInt::GetMn(BtaEventBs->GetEvents(), LastEventN + MxEventWnd + 1);
    const uint64 FirstTmMSec = BtaEventBs->GetEvent(LastEventN+1).GetFirstTmMSecs();
    int EventWndN = LastEventN+1;
    while (EventWndN < MxEvents) {
        // get alarm
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventWndN); EventWndN++; 
        if ((Event.GetIpNumId() == -1) || (Event.GetProjId() == -1)){ continue; }
        // check if still within the window
        const int CountDiff = EventWndN - LastEventN - 1;
        const int64 TmMSecDiff = Event.GetFirstTmMSecs() - FirstTmMSec;
        if ((MxEventWnd > 0) && (CountDiff > MxEventWnd)) { break; }
        if ((MxTmMSecWnd > 0) && (TmMSecDiff > MxTmMSecWnd)) { break; }
        // if yes, we add it to the window
        FutureIpNumH.AddDat(Event.GetIpNumId())++;
    }    
}

void TBtaCorrBs::GetPredStat(const PBtaEventBs& BtaEventBs, const int& MxEvents, 
        const int& MxEventWnd, const int64& MxTmMSecWnd, const int& MxEvalWnd) const {

    // preapre for results
    const int TopRank = 100; TMomV TopMomV(TopRank), TimeMomV(TopRank);
    for (int RankN = 0; RankN < TopRank; RankN++) {
        TopMomV[RankN] = TMom::New(); TimeMomV[RankN] = TMom::New(); }
    // iterate and predict
    TRnd Rnd(1);
    for (int EventIdN = 0; EventIdN < MxEvents; EventIdN++) {
        printf("%d / %d\r", EventIdN+1, MxEvents);
        const int EventId = Rnd.GetUniDevInt(BtaEventBs->GetEvents() - MxEvalWnd);
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
        // predict
        TIntFltH IpNumIdProbH;
        GetNextIpNumIdProb(EventId, Event.GetProjId(),
            BtaEventBs, MxEventWnd, MxTmMSecWnd, IpNumIdProbH);
        // evaluate
        if (!IpNumIdProbH.Empty()) {
            // sort according to probabilities
            TFltIntPrV ProbIpNumIdV;
            IpNumIdProbH.GetDatKeyPrV(ProbIpNumIdV);
            ProbIpNumIdV.Sort(false);
            // find for the top rank devices, how many will happen in next MxEvalWnd
            TIntH FutureIpNumH;
            GetListOfFutureIpNums(BtaEventBs, EventId+1, MxEvalWnd, -1, FutureIpNumH);
            // check for each top ranked device if it will fire alarm
            TFltV RankCountV(TopRank); RankCountV.PutAll(0.0); double SumTime = 0.0;
            for (int RankN = 0; RankN < TopRank; RankN++) {
                if (RankN >= ProbIpNumIdV.Len()) { break; }
                const int RankIpNumId = ProbIpNumIdV[RankN].Val2;
                // check short term correctnes
                if (FutureIpNumH.IsKey(RankIpNumId)) {
                    for (int RestRankN = RankN; RestRankN < TopRank; RestRankN++) {
                        RankCountV[RestRankN] += 1.0; }
                }
                TopMomV[RankN]->Add(RankCountV[RankN]);
                // check average time
                const int NextEventId = BtaEventBs->GetNextEvent(EventId+1, RankIpNumId);
                if (NextEventId != -1) {
                    const TBtaEvent& NextEvent = BtaEventBs->GetEvent(NextEventId);
                    const uint64 TimeDiffMSec = NextEvent.GetFirstTmMSecs() - Event.GetFirstTmMSecs();
                    const int TimeDiffSec = int(TimeDiffMSec / uint64(1000));
                    SumTime += double(TimeDiffSec);
                }
                TimeMomV[RankN]->Add(SumTime / double(RankN + 1));
            }
        }
    }
    // overall statistics
    TFOut RankFOut(TFile::GetUniqueFNm("PredRes.txt")); 
    for (int RankN = 0; RankN < TopRank; RankN++) {
        TopMomV[RankN]->Def(); TimeMomV[RankN]->Def();
        RankFOut.PutStrLn(TStr::Fmt("%3d: Prec: %3.5f +/- %2.5f    Time: %5.5f +/- %3.5f",
            RankN + 1, TopMomV[RankN]->GetMean(), TopMomV[RankN]->GetSDev(),    
            TimeMomV[RankN]->GetMean(), TimeMomV[RankN]->GetSDev()));
    }
    printf("\n");
}

void TBtaCorrBs::EventRootCauseProb(const PBtaEventBs& BtaEventBs, 
        const int& EventId, const int& RootCouseEventWnd, 
        const int64& RootCouseTimeWnd, TIntFltKdV& EventIdScoreV) const {

    TBtaEvent Event = BtaEventBs->GetEvent(EventId);
    const int IpNumId = Event.GetIpNumId();
    const int ProjId = Event.GetProjId();
    const int64 TmMSec = Event.GetFirstTmMSecs();
    // evaluate previous alarms based on device correlations
    TFltIntPrV ProbEventIdV;
    int ProcEventN = 0, CurrEventId = EventId;
    while (CurrEventId > 0) {
        CurrEventId--; // to next one
        // load it
        TBtaEvent CurrEvent = BtaEventBs->GetEvent(CurrEventId);
        const int CurrIpNumId = CurrEvent.GetIpNumId();
        const int CurrProjId = CurrEvent.GetProjId();
        // first we check the time constraint
        const int64 TmMSecDiff = TmMSec - CurrEvent.GetFirstTmMSecs();
        if (TmMSecDiff > RootCouseTimeWnd) { break; }
        // then some cheap stuff
        if (CurrIpNumId == -1 || CurrProjId == -1) { continue; }
        if (ProjId != CurrProjId) { continue; }
        if (CurrEvent.GetSeverity() < 3) { continue; }
        // some more expencive stuff
        if (!IsIpNumIdPrCorr(CurrIpNumId, IpNumId)) { continue; }
        // check event window stoping critera
        if (ProcEventN > RootCouseEventWnd) { break; }
        // all good, check the correlation
        ProcEventN++;
        const double CorrProb = ProbCorrIpNumIdGivenIpNumId(CurrIpNumId, IpNumId, true);
        ProbEventIdV.Add(TFltIntPr(CorrProb, CurrEventId));
    }
    // make a table of top MxEvents correlated alarms
    ProbEventIdV.Sort(false); 
    const int MxEvents = 5;
    const int ProbEventIds = TInt::GetMn(ProbEventIdV.Len(), MxEvents);
    EventIdScoreV.Clr(); double OldProb = 0.0;
    for (int ProbEventIdN = 0; ProbEventIdN < ProbEventIds; ProbEventIdN++) {
        const int ProbEventId = ProbEventIdV[ProbEventIdN].Val2;
        const double Prob = ProbEventIdV[ProbEventIdN].Val1;
        //if (Prob < 0.0001) { break; }
        if (Prob < OldProb / 3) { break; } OldProb = Prob;
        EventIdScoreV.Add(TIntFltKd(ProbEventId, Prob));
    }
}

void TBtaCorrBs::EventRootCauseText(const PBtaEventBs& BtaEventBs, 
        const int& EventId,const int& RootCouseEventWnd, 
        const int64& RootCouseTimeWnd, TIntFltKdV& EventIdScoreV) const {

    TBtaEvent Event = BtaEventBs->GetEvent(EventId);
    TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);
    const int IpNumId = Event.GetIpNumId();
    const int ProjId = Event.GetProjId();
    const int64 TmMSec = Event.GetFirstTmMSecs();
    // add previous alarms to BOW
    PBowDocBs BowDocBs = TBowDocBs::New();
    const int FirstDId = BowDocBs->AddHtmlDoc(
        TInt::GetStr(EventId), TStrV(), EventDat.GetSummary());
    IAssert(FirstDId == 0); int ProcEventN = 0, CurrEventId = EventId;
    while (CurrEventId > 0) {
        CurrEventId--; // to next one
        // load it
        TBtaEvent CurrEvent = BtaEventBs->GetEvent(CurrEventId);
        const int CurrIpNumId = CurrEvent.GetIpNumId();
        const int CurrProjId = CurrEvent.GetProjId();
        // first we check the time constraint
        const int64 TmMSecDiff = TmMSec - CurrEvent.GetFirstTmMSecs();
        if (TmMSecDiff > RootCouseTimeWnd) { break; }
        // then some cheap stuff
        if (CurrIpNumId == -1 || CurrProjId == -1) { continue; }
        if (ProjId != CurrProjId) { continue; }
        if (CurrEvent.GetSeverity() < 3) { continue; }
        // check event window stoping critera
        if (ProcEventN > RootCouseEventWnd) { break; }
        // all good, check the correlation
        ProcEventN++;
        TBtaEventDat CurrEventDat; BtaEventBs->GetEventDat(CurrEvent.GetBlobPt(), CurrEventDat);
        BowDocBs->AddHtmlDoc(TInt::GetStr(CurrEventId), TStrV(), CurrEventDat.GetSummary());
    }
    // calculate similarity of alarms
    TIntFltIntTrV HeaderSimEventIdV;
    PBowSim BowSim = TBowSim::New(bstCos);
    TStr EventSummary = EventDat.GetSummary(), HeaderStr = "";
    if (EventSummary.SearchCh(':') > 0) {
        HeaderStr = EventSummary.GetSubStr(0, EventSummary.SearchCh(':')-1); 
    }
    for (int DId = 0; DId < BowDocBs->GetDocs(); DId++) {
        if (DId == FirstDId) { continue; }
        const int CurrEventId = BowDocBs->GetDocNm(DId).GetInt();
        TBtaEvent CurrEvent = BtaEventBs->GetEvent(CurrEventId);
        TStr CurrEventSummary = EventDat.GetSummary(), CurrHeaderStr = "";
        const int ColonPos = EventSummary.SearchCh(':');
        if (ColonPos > 0) { CurrHeaderStr = CurrEventSummary.GetSubStr(0, ColonPos-1); }
        TIntFltIntTr HeaderSimEventId;
        HeaderSimEventId.Val1 = (HeaderStr == CurrHeaderStr) ? 1 : 0;
        HeaderSimEventId.Val2 = BowSim->GetSim(
            BowDocBs->GetDocSpV(FirstDId), BowDocBs->GetDocSpV(DId));
        HeaderSimEventId.Val3 = CurrEventId;
        HeaderSimEventIdV.Add(HeaderSimEventId);
    }
    // make a table of top MxEvents correlated alarms
    HeaderSimEventIdV.Sort(false); 
    const int MxEvents = 5;
    const int HeaderSimEventIds = TInt::GetMn(HeaderSimEventIdV.Len(), MxEvents);
    EventIdScoreV.Clr(); double OldSim = 0.0;
    for (int HeaderSimEventIdN = 0; HeaderSimEventIdN < HeaderSimEventIds; HeaderSimEventIdN++) {
        const int HeaderSimEventId = HeaderSimEventIdV[HeaderSimEventIdN].Val3;
        const double Sim = HeaderSimEventIdV[HeaderSimEventIdN].Val2;
        if (Sim < 0.001) { break; }
        if (Sim < OldSim / 3) { break; } OldSim = Sim;
        EventIdScoreV.Add(TIntFltKd(HeaderSimEventId, Sim));
    }
}

void TBtaCorrBs::EventRootCause(const PBtaEventBs& BtaEventBs, const int& EventId, 
        const int& RootCouseEventWnd, const int64& RootCouseTimeWnd,
        TIntFltKdV& EventIdScoreV, TStrV& CorrTypeV) const {

    // get list of probabilisticaly related events
    TIntFltKdV EventIdProbV;
    EventRootCauseProb(BtaEventBs, EventId, 
        RootCouseEventWnd, RootCouseTimeWnd, EventIdProbV);
    // get list of textually related events
    TIntFltKdV EventIdSimV; 
    EventRootCauseText(BtaEventBs, EventId,
        RootCouseEventWnd, RootCouseTimeWnd, EventIdSimV);
    // merge the events
    TIntFltH EventIdScoreH; TIntStrH EventIdTypeH;
    for (int EventIdProbN = 0; EventIdProbN < EventIdProbV.Len(); EventIdProbN++) {
        const int ProbEventId = EventIdProbV[EventIdProbN].Key;
        const double Prob = EventIdProbV[EventIdProbN].Dat;
        const double MxProb = EventIdProbV[0].Dat;
        const double Score = Prob / MxProb;
        EventIdScoreH.AddDat(ProbEventId, Score);
        EventIdTypeH.AddDat(ProbEventId, "Prob");
    }
    for (int EventIdSimN = 0; EventIdSimN < EventIdSimV.Len(); EventIdSimN++) {
        const int SimEventId = EventIdSimV[EventIdSimN].Key;
        const double Sim = EventIdSimV[EventIdSimN].Dat;
        const double MxSim = EventIdSimV[0].Dat;
        const double Score = Sim / MxSim;
        if (EventIdScoreH.IsKey(SimEventId)) {
            const double OldScore = EventIdScoreH.GetDat(SimEventId);
            EventIdScoreH.GetDat(SimEventId) = TFlt::GetMx(OldScore, Score);
            EventIdTypeH.GetDat(SimEventId) = "Text";
        } else {
            EventIdScoreH.AddDat(SimEventId, Score);
            EventIdTypeH.AddDat(SimEventId, "Text");
        }
    }
    // sort and return
    TFltIntKdV ScoreEventIdV;
    int EventKeyId = EventIdScoreH.FFirstKeyId();
    while (EventIdScoreH.FNextKeyId(EventKeyId)) {
        const int EventId = EventIdScoreH.GetKey(EventKeyId);
        const double Score = EventIdScoreH[EventKeyId];
        ScoreEventIdV.Add(TFltIntKd(Score, EventId));
    }
    ScoreEventIdV.Sort(false);
    GetSwitchedKdV<TFlt, TInt>(ScoreEventIdV, EventIdScoreV);
    // sort also the types
    CorrTypeV.Gen(EventIdScoreV.Len(), 0);
    for (int EventIdN = 0; EventIdN < EventIdScoreV.Len(); EventIdN++) {
        CorrTypeV.Add(EventIdTypeH.GetDat(EventIdScoreV[EventIdN].Key));
    }
}

void TBtaCorrBs::GetRootCauseStat(const PBtaEventBs& BtaEventBs, 
        const int& MxEvents, const int& MnSeverity, const int& RootCouseEventWnd, 
        const int64& RootCouseTimeWnd, const bool& PropP, const bool& TextP, 
        TIntV& AllScoresCountV, TIntV& ProbScoresCountV, TIntV& TextScoresCountV, 
        int& AllEvents) const {

    // initialize counter
    const int MxCount = 10;
    AllScoresCountV.Gen(MxCount); 
    ProbScoresCountV.Gen(MxCount);  
    TextScoresCountV.Gen(MxCount);  
    AllEvents = 0;
    TTmStopWatch AllSw, ProbSw, TextSw;
    // iterate over last MxEvents
    int EventN = BtaEventBs->GetEvents();
    while (EventN > 0) {
        // count
        EventN--;
        const int Events = BtaEventBs->GetEvents() - EventN;
        if (Events % 100 == 0) { printf("%d\r", Events ); }
        if (Events >= MxEvents) { break; }
        // check constraints
        const int EventSeverity = BtaEventBs->GetEvent(EventN).GetSeverity();
        if (EventSeverity < MnSeverity) { continue; }
        // get the scores
        TIntFltKdV EventIdScoreV; TStrV CorrTypeV;
        if (PropP && TextP) {
            AllSw.Start();
            EventRootCause(BtaEventBs, EventN, RootCouseEventWnd, 
                RootCouseTimeWnd, EventIdScoreV, CorrTypeV);
            AllSw.Stop();
        } else if (PropP) {
            ProbSw.Start();
            EventRootCauseProb(BtaEventBs, EventN, RootCouseEventWnd, 
                RootCouseTimeWnd, EventIdScoreV);
            ProbSw.Stop();
            CorrTypeV.Gen(EventIdScoreV.Len());
            CorrTypeV.PutAll("Prob");
        } else if (TextP) {
            TextSw.Start();
            EventRootCauseText(BtaEventBs, EventN, RootCouseEventWnd, 
                RootCouseTimeWnd, EventIdScoreV);
            TextSw.Stop();
            CorrTypeV.Gen(EventIdScoreV.Len());
            CorrTypeV.PutAll("Text");
        }
        // add them to the histogram
        const int AllCount = EventIdScoreV.Len();
        const int ProbCount = CorrTypeV.Count("Prob");
        const int TextCount = CorrTypeV.Count("Text");
        AllScoresCountV[TInt::GetMn(AllCount, MxCount-1)]++;
        ProbScoresCountV[TInt::GetMn(ProbCount, MxCount-1)]++;
        TextScoresCountV[TInt::GetMn(TextCount, MxCount-1)]++;
        // count analysed alarm
        AllEvents++;
    }
    // print time statistics
    printf("RootCauseStatTm: [a:%.2fs, p:%.2fs, t:%.2fs]\n", 
        AllSw.GetSec(), ProbSw.GetSec(), TextSw.GetSec());
}

void TBtaCorrBs::GetStat(const PBtaEventBs& BtaEventBs) const {
    // counters
    TIntH SeverityH;
    TStrIntVH CaseIdEventVH;
    int CaseIds = 0;
    // count
    for (int EventN = 0; EventN < BtaEventBs->GetEvents(); EventN++) {
        // get event
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventN);
        // display progress
        if (EventN % 10000 == 0) { 
            printf("%10d / %d\r", EventN, BtaEventBs->GetEvents()); }
        // check severity
        if (Event.GetSeverity() != -1) {
            const int Severity = Event.GetSeverity();
            SeverityH.AddDat(Severity)++;
        }
        // check case id
        if (Event.GetCaseId() != -1) {
            TStr CaseNm = BtaEventBs->GetCase(Event.GetCaseId());
            CaseIdEventVH.AddDat(CaseNm).Add(EventN);
            CaseIds++;
        }
    } printf("\n");
    // severity stats
    TIntPrV SeverityFqV; 
    SeverityH.GetKeyDatPrV(SeverityFqV);
    SeverityFqV.Sort();
    printf("Severity:\n");
    for (int SeverityN = 0; SeverityN < SeverityFqV.Len(); SeverityN++) {
        printf("  %d: %d\n", SeverityFqV[SeverityN].Val1.Val,
            SeverityFqV[SeverityN].Val2.Val);
    }
    // case id stats
    printf("CaseIds: All=%d, Unique=%d\n", CaseIds, CaseIdEventVH.Len());
    PMom SeqDiff = TMom::New(), TmDiff = TMom::New(), CaseFq = TMom::New();
    int CaseIdKeyId = CaseIdEventVH.FFirstKeyId();
    while (CaseIdEventVH.FNextKeyId(CaseIdKeyId)) {
        const TIntV& EventV = CaseIdEventVH[CaseIdKeyId];
        for (int EventN = 1; EventN < EventV.Len(); EventN++) {
            // get sequential difference
            const int EventId = EventV[EventN-1];
            const int CorrEventId = EventV[EventN];
            SeqDiff->Add(CorrEventId - EventId);
            // get time difference (in seconds)
            const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
            const TBtaEvent& CorrEvent = BtaEventBs->GetEvent(CorrEventId);
            const uint64 TmDiffMSec = TTm::GetDiffMSecs(
                Event.GetFirstTm(), CorrEvent.GetFirstTm());
            TmDiff->Add(int(TmDiffMSec/uint64(1000)));
        }
        // case-id frequency
        CaseFq->Add(EventV.Len());
    }
    SeqDiff->Def(); TmDiff->Def(); CaseFq->Def();
    printf(" FQ: %.2f +/- %.2f [Min:%.0f Med:%.0f Max:%.0f]\n:", CaseFq->GetMean(), 
        CaseFq->GetSDev(), CaseFq->GetMn(), CaseFq->GetMedian(), CaseFq->GetMx());
    printf(" SD: %.2f +/- %.2f [Min:%.0f Med:%.0f Max:%.0f]\n:", SeqDiff->GetMean(), 
        SeqDiff->GetSDev(), SeqDiff->GetMn(), SeqDiff->GetMedian(), SeqDiff->GetMx());
    printf(" TD: %.2f +/- %.2f [Min:%.0f Med:%.0f Max:%.0f]\n:", TmDiff->GetMean(), 
        TmDiff->GetSDev(), TmDiff->GetMn(), TmDiff->GetMedian(), TmDiff->GetMx());
}

/////////////////////////////////////////////////
// BTAlarms-LongTerm-Project-Stats
void TBtaLongProj::GetCountV(const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, TIntH& IpNumIdH, TFltV& OldV, TFltV& NewV) const {

    // make a list of all devices
    IpNumIdH.Clr(); 
    {int DayKeyId = DayToIpNumIdFqVH.FFirstKeyId();
    while (DayToIpNumIdFqVH.FNextKeyId(DayKeyId)) {
        const TIntPrV& IpNumIdFqV = DayToIpNumIdFqVH[DayKeyId];
        for (int IpNumN = 0; IpNumN < IpNumIdFqV.Len(); IpNumN++) {
            IpNumIdH.AddKey(IpNumIdFqV[IpNumN].Val1);
        }
    }}
    // count events before and after BreakDate
    const int IpNums = IpNumIdH.Len();
    OldV.Gen(IpNums); OldV.PutAll(1.0);   
    NewV.Gen(IpNums); NewV.PutAll(1.0);
    int OldTotalCount = IpNums, NewTotalCount = IpNums;
    // iterate over all the days and check devices per day
    {int DayKeyId = DayToIpNumIdFqVH.FFirstKeyId();
    while (DayToIpNumIdFqVH.FNextKeyId(DayKeyId)) {
        // get date
        const int DateInt = DayToIpNumIdFqVH.GetKey(DayKeyId);
        TTm Date = TTm::GetTmFromDateTimeInt(DateInt);
        if (Date < StartDate || Date > EndDate) { continue; }
        // iterate over all the devices and count alarms
        const TIntPrV& IpNumIdFqV = DayToIpNumIdFqVH[DayKeyId];
        for (int IpNumN = 0; IpNumN < IpNumIdFqV.Len(); IpNumN++) {
            const int IpNumId = IpNumIdH.GetKeyId(IpNumIdFqV[IpNumN].Val1);
            const int IpNumFq = IpNumIdFqV[IpNumN].Val2;
            if (Date < BreakDate) { OldV[IpNumId] += double(IpNumFq); OldTotalCount++; }
            else { NewV[IpNumId] += double(IpNumFq); NewTotalCount++; }
        }
    }}
    // calculate normalization factors
    int OldDayDiff = TTm::GetDiffDays(StartDate, BreakDate);
    int NewDayDiff = TTm::GetDiffDays(BreakDate, EndDate);
    IAssert(OldDayDiff > 0 && NewDayDiff > 0);
    double NormFact = double(NewDayDiff) / double(OldDayDiff);
    // normalize counts so they are comparable
    if (OldTotalCount > 0 && NewTotalCount > 0) {
        TLinAlg::MultiplyScalar(NormFact, OldV, OldV);
    } else {
        OldV.Clr(); NewV.Clr();
    }
}

void TBtaLongProj::AddEvent(const uint64& DateMSecs, const int& IpNumId) { 
    const int DateInt = TTm::GetDateIntFromTm(TTm::GetTmFromMSecs(DateMSecs));
    DayToIpNumIdFqHH.AddDat(DateInt).AddDat(IpNumId)++;
}

void TBtaLongProj::Def() {
    // iterate over days and load sparse vectors
    int DayKeyId = DayToIpNumIdFqHH.FFirstKeyId();
    while (DayToIpNumIdFqHH.FNextKeyId(DayKeyId)) {
        const int DayInt = DayToIpNumIdFqHH.GetKey(DayKeyId);
        TIntPrV& IpNumIdFqV = DayToIpNumIdFqVH.AddDat(DayInt);
        DayToIpNumIdFqHH[DayKeyId].GetKeyDatPrV(IpNumIdFqV);
        DayToIpNumIdFqHH[DayKeyId].Clr();
    }
    DayToIpNumIdFqHH.Clr();
}

void TBtaLongProj::Count(const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, int& BeforeCount, int& AfterCount) const {

    // iterate over all the days and check devices per day
    BeforeCount = 0; AfterCount = 0;
    int DayKeyId = DayToIpNumIdFqVH.FFirstKeyId();
    while (DayToIpNumIdFqVH.FNextKeyId(DayKeyId)) {
        // get date
        const int DateInt = DayToIpNumIdFqVH.GetKey(DayKeyId);
        TTm Date = TTm::GetTmFromDateTimeInt(DateInt);
        if (Date < StartDate || Date > EndDate) { continue; }
        // count events of the day
        int DayEvents = 0;
        const TIntPrV& IpNumIdFqV = DayToIpNumIdFqVH[DayKeyId];
        for (int IpNumIdN = 0; IpNumIdN < IpNumIdFqV.Len(); IpNumIdN++) {
            DayEvents += IpNumIdFqV[IpNumIdN].Val2; }
        if (Date < BreakDate) { BeforeCount += DayEvents; }
        else { AfterCount += DayEvents; }
    }
}

double TBtaLongProj::ChiSquare(const TTm& StartDate, 
        const TTm& BreakDate, const TTm& EndDate) const { 

    // prepare expected values and actuall vaulues
    TIntH IpNumIdH; TFltV OldV, NewV; 
    GetCountV(StartDate, BreakDate, EndDate, IpNumIdH, OldV, NewV);
    // if any is empty, than we ignore this
    if (OldV.Empty() || NewV.Empty()) { return 1.0; }
    // calcualte the probability that distributions are the same
    double Val, Prb; TStatTest::ChiSquareOne(NewV, OldV, Val, Prb);
    return Prb;
}

bool TBtaLongProj::IsTrendPos(const TTm& StartDate, 
        const TTm& BreakDate, const TTm& EndDate) const {

    // count
    const double NormFact = GetNormFact(StartDate, BreakDate, EndDate);
    int BeforeCount, AfterCount; 
    Count(StartDate, BreakDate, EndDate, BeforeCount, AfterCount);
    const bool IsTrendPosP = ((NormFact * double(BeforeCount)) < AfterCount);
    return IsTrendPosP;
}

void TBtaLongProj::GetIpNumWgtV(const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, TIntFltKdV& IpNumIdWgtV) const {

    // prepare expected values and actuall vaulues
    TIntH IpNumIdH; TFltV OldV, NewV; 
    GetCountV(StartDate, BreakDate, EndDate, IpNumIdH, OldV, NewV);
    // if any is empty, than we ignore this
    if (OldV.Empty() || NewV.Empty()) { IpNumIdWgtV.Clr(); return; }
    // calculate contribution of each device and see what happened
    IAssert(OldV.Len() == NewV.Len());
    const int IpNumIds = OldV.Len(); TFltIntKdV IpNumWgtIdV;
    for (int IpNumIdN = 0; IpNumIdN < IpNumIds; IpNumIdN++){
        IAssert(OldV[IpNumIdN] > 0);
        const int IpNumId = IpNumIdH.GetKey(IpNumIdN);
        const double BinDiff = NewV[IpNumIdN] - OldV[IpNumIdN];
        const double ChiSquareVal = TMath::Sqr(BinDiff) / OldV[IpNumIdN];
        IpNumWgtIdV.Add(TFltIntKd(ChiSquareVal, IpNumId));
    }
    // sort by the weigth
    IpNumWgtIdV.Sort(false);
    GetSwitchedKdV<TFlt, TInt>(IpNumWgtIdV, IpNumIdWgtV);
}

double TBtaLongProj::GetNormFact(const TTm& StartDate, 
        const TTm& BreakDate, const TTm& EndDate) {

    // calculate normalization factors
    int OldDayDiff = TTm::GetDiffDays(StartDate, BreakDate);
    int NewDayDiff = TTm::GetDiffDays(BreakDate, EndDate);
    IAssert(OldDayDiff > 0 && NewDayDiff > 0);
    return double(NewDayDiff) / double(OldDayDiff);;
}

/////////////////////////////////////////////////
// BTAlarms-LongTerm-Base
TBtaLongBs::TBtaLongBs(const PBtaEventBs& BtaEventBs, 
        const PBtaCorrBs& BtaCorrBs, const int& MnSeverity) {

    printf("Trends over projects ...\n");
    const int Events = BtaEventBs->GetEvents(); int Skiped = 0;
    for (int EventN = 0; EventN < Events; EventN++) {
        // get event
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventN);
        // display progress
        if (EventN % 10000 == 0) { printf("%10d / %d [%d]\r", EventN, Events, Events-Skiped); }
        // add event to the stats
        const int ProjId = Event.GetProjId();
        const int IpNumId = Event.GetIpNumId();
        if ((ProjId != -1) && (IpNumId != -1) && 
                //BtaCorrBs->IsProjId(ProjId) && 
                BtaCorrBs->IsIpNumId(IpNumId) &&
                Event.GetSeverity() >= MnSeverity) {

            const uint64 DateMSecs = TTm::GetMSecsFromTm(Event.GetFirstTm());
            if (!ProjIdToProfileH.IsKey(ProjId)) {
                ProjIdToProfileH.AddDat(ProjId, TBtaLongProj::New()); }
            ProjIdToProfileH.GetDat(ProjId)->AddEvent(DateMSecs, IpNumId);
        } else { 
            Skiped++; 
        }
    } printf("\n");
    printf("Summarizing ...");
    int ProjKeyId = ProjIdToProfileH.FFirstKeyId();
    while (ProjIdToProfileH.FNextKeyId(ProjKeyId)) {
        ProjIdToProfileH[ProjKeyId]->Def();
    }
    printf("\nDone\n");
}

