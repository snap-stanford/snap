//////////////////////////////////////////////////
// Application-Client-Server

// generic arguments
TStr TAppCS::A_Result="Result";
TStr TAppCS::A_Msg="Msg";
TStr TAppCS::A_MsgType="MsgType";
TStr TAppCS::A_Continue="Continue";
TStr TAppCS::A_Browse="Browse";
TStr TAppCS::A_SesId="SesId";
TStr TAppCS::A_FetchId="FetchId";
TStr TAppCS::A_Redir="Redir";
TStr TAppCS::A_Agent="Agent";
TStr TAppCS::A_HostNm="HostNm";
TStr TAppCS::A_PortN="PortN";
TStr TAppCS::A_UsrNm="UsrNm";
TStr TAppCS::A_PwdStr="PwdStr";
TStr TAppCS::A_PrivNm="PrivNm";
TStr TAppCS::A_PrivId="PrivId";
TStr TAppCS::A_CltNm="CltNm";
TStr TAppCS::A_CltType="CltType";
TStr TAppCS::A_State="State";
TStr TAppCS::A_PrevState="PrevState";
TStr TAppCS::A_OldState="OldState";
TStr TAppCS::A_NewState="NewState";
TStr TAppCS::A_AbortState="AbortState";
TStr TAppCS::A_SetStateTm="SetStateTm";
TStr TAppCS::A_LogNm="LogNm";

// functions
TStr TAppCS::F_Info="Info";
TStr TAppCS::F_GetStateInfo="GetStateInfo";
TStr TAppCS::F_GetCltList="GetCltList";
TStr TAppCS::F_GetCltInfo="GetCltInfo";
TStr TAppCS::F_GetCltDesc="GetCltDesc";
TStr TAppCS::F_IsAlive="IsAlive";
TStr TAppCS::F_IsConn="IsConn";
TStr TAppCS::F_Log="Log";
TStr TAppCS::F_Connect="Connect";
TStr TAppCS::F_Disconnect="Disconnect";
TStr TAppCS::F_DelayTimer="DelayTimer";
TStr TAppCS::F_RegAgent="RegAgent";
TStr TAppCS::F_Confirm="Confirm";
TStr TAppCS::F_Cancel="Cancel";
TStr TAppCS::F_Notify="Notify";
TStr TAppCS::F_NotifyStateChange="NotifyStateChange";
TStr TAppCS::F_Abort="Abort";

// function hbeat-timer
TStr TAppCS::F_HBeatTimer="HBeatTimer";
TStr TAppCS::TmA_TimerId="TimerId";
TStr TAppCS::TmA_CtxNm="CtxNm";
TStr TAppCS::TmA_Ticks="Ticks";

// function-broadcast
TStr TAppCS::F_Broadcast="Broadcast";
TStr TAppCS::BcA_Cm="Cm";
TStr TAppCS::BcA_FldNm="FldNm";
TStr TAppCS::BcA_FldVal="FldVal";
TStr TAppCS::BcA_Msg="Msg";

// logging
TStr TAppCS::F_NotifyEvent="Log::NotifyEvent";
TStr TAppCS::F_NotifyRawEvent="Log::NotifyRawEvent";
TStr TAppCS::LogA_EventTypeNm="EventTypeNm";
TStr TAppCS::LogA_TestMode="TestMode";
TStr TAppCS::LogA_CreatorNm="CreatorNm";
TStr TAppCS::LogA_CreatorTime="CreatorTime";

// settings
int TAppCS::DTmrA_DelayTimerTout=2*1000;

// states
TStr TAppCS::S_Start="Start";
TStr TAppCS::S_End="End";
TStr TAppCS::S_ConnectRq="ConnectRq";
TStr TAppCS::S_ConnectResp="ConnectResp";
TStr TAppCS::S_Connected="Connected";
TStr TAppCS::S_Server="Server";

//////////////////////////////////////////////////
// Application-Client-Server-Logging
TAppCSLog::TAppCSLog(
const TStr& _LogFNm, const int64& _MxFLen, const PNotify& _Notify):
  Notify(_Notify),
  LogFNm(_LogFNm), LogSOut(), fLog(NULL), MxFLen(_MxFLen){
  if (LogFNm.Empty()){
    TNotify::OnNotify(Notify, ntInfo, "Log-File Not Generated.");
  } else {
    // prepare Log-FileName
    if (LogFNm.GetFPath().Empty()){
      LogFNm=GetHomeNrFPath()+LogFNm;}
    // check existence and create/open
    if (false&&TFile::Exists(LogFNm)){
      LogSOut=TFOut::New(LogFNm, true); fLog=LogSOut->GetFileId();
      TNotify::OnNotify(Notify, ntInfo, TStr("Log-File ")+LogFNm+" opened.");
    } else {
      LogSOut=TFOut::New(LogFNm); fLog=LogSOut->GetFileId();
      TNotify::OnNotify(Notify, ntInfo, TStr("Log-File ")+LogFNm+" created.");
    }
    // write log-file header
    PutLogHd();
  }
}

void TAppCSLog::ResetLogIfTooBig(){
  if ((fLog!=NULL)&&(MxFLen!=-1)){
    int64 FLen=ftell(fLog);
    if (FLen>MxFLen){
      LogSOut=NULL; // close the file
      try {
        TStr OldLogFNm=LogFNm+".Old";
        TFile::Del(OldLogFNm, false);
        TNotify::OnNotify(Notify, ntInfo, TStr("Log-File ")+OldLogFNm+" deleted.");
        TFile::Rename(LogFNm, OldLogFNm);
        TNotify::OnNotify(Notify, ntInfo, TStr("Log-File ")+LogFNm+" renamed.");
        LogSOut=TFOut::New(LogFNm, true);
        TNotify::OnNotify(Notify, ntInfo, TStr("Log-File ")+LogFNm+" opened.");
        PutLogHd();
      } catch (PExcept Except){
        TNotify::OnNotify(Notify, ntErr, Except->GetMsgStr());
      }
    }
  }
}

void TAppCSLog::PutLogHd(){
  // get current time
  TTm CurTm=TTm::GetCurUniTm();
  // output log-file line
  if (fLog!=NULL){
    fprintf(fLog, "#Software: Quintelligence Application-Web-Server 1.0\n");
    fprintf(fLog, "#Version: 1.0\n");
    fprintf(fLog, "#Date: %s %s\n",
     CurTm.GetWebLogDateStr().CStr(), CurTm.GetWebLogTimeStr().CStr());
    fprintf(fLog, "#Fields: date time c-ip cs-method cs-uri sc-status\n");
    fflush(fLog);
  }
  ResetLogIfTooBig();
}

void TAppCSLog::PutLogRec(
 const PHttpRq& HttpRq, const PHttpResp& HttpResp, const TStr& PeerHostNm){
  // get current time
  TTm CurTm=TTm::GetCurUniTm();
  // output log-file line
  if (fLog!=NULL){
    fprintf(fLog, "%s %s %s %s %s %s\n",
     CurTm.GetWebLogDateStr().CStr(),
     CurTm.GetWebLogTimeStr().CStr(),
     PeerHostNm.CStr(),
     HttpRq->GetMethodNm().CStr(),
     HttpRq->GetUrl()->GetUrlStr().CStr(),
     TInt::GetStr(HttpResp->GetStatusCd()).CStr());
    fflush(fLog);
  }
  // notification
  TStr MsgStr=TStr("Http: ")+
   PeerHostNm+" requested "+HttpRq->GetUrl()->GetUrlStr()+
   " responding "+THttp::GetReasonPhrase(HttpResp->GetStatusCd())
   +" ["+CurTm.GetStr()+"]";
//   /*+" ["+CurTm.GetStr()+"]"*/;
  TNotify::OnNotify(Notify, ntInfo, MsgStr);
  ResetLogIfTooBig();
}

void TAppCSLog::PutLogMsg(const TStr& MsgType, const TStr& MsgStr){
  // get current time
  TTm CurTm=TTm::GetCurUniTm();
  // output log-file line
  if (fLog!=NULL){
    fprintf(fLog, "#%s: %s\n",
     MsgType.CStr(),
     MsgStr.CStr());
    /*fprintf(fLog, "#%s: %s [%s %s]\n",
     MsgType.CStr(),
     MsgStr.CStr(),
     CurTm.GetWebLogDateStr().CStr(),
     CurTm.GetWebLogTimeStr().CStr());*/
    fflush(fLog);
  }
  // notification
  TNotify::OnNotify(
   Notify, ntInfo, MsgType+": "+MsgStr/*+" ["+CurTm.GetStr()+"]"*/);
  ResetLogIfTooBig();
}

void TAppCSLog::PutLogMsg(
 const TStr& MsgType, const TStr& MsgStr, const TStr& MsgArg){
  PutLogMsg(MsgType, MsgStr+" ("+MsgArg+").");
}

//////////////////////////////////////////////////
// Application-Client-Server-Timer
TAppCSTimer::TAppCSTimer(
 const TStr& _TimerNm, const TStr& _CtxNm,
 const int& TimerTout, const bool& _RepeatP, TWebNetSrv* _WebNetSrv):
  TTTimer(TimerTout), WebNetSrv(_WebNetSrv),
  TimerNm(_TimerNm), RepeatP(_RepeatP), CtxNm(_CtxNm){
  IAssertR(TXmlLx::IsTagNm(TimerNm),
   TStr("Timer-name is not XML-Tag-Name (")+TimerNm+").");
}

void TAppCSTimer::OnTimeOut(){
  // stop timer if not repeatable
  if (!RepeatP){
    StopTimer();}
  // prepare soap-request
  PSoapRq SoapRq=TSoapRq::New(TimerNm,
   TAppCS::TmA_TimerId, TInt::GetStr(GetTimerId()),
   TAppCS::TmA_CtxNm, CtxNm,
   TAppCS::TmA_Ticks, TInt::GetStr(GetTicks()));
  // prepare url
  PUrl Url=SoapRq->GetAsUrl("localhost");
  // prepare http-request
  PHttpRq HttpRq=THttpRq::New(Url);
  // trigger timer-event
  WebNetSrv->OnHttpRq(-1, HttpRq);
}

//////////////////////////////////////////////////
// Application-Client-Server-State
TAppCSState::TAppCSState(const PAppCSLog& _Log):
  StateId(), Log(_Log){
  PrepState();
}

void TAppCSState::PrepState(){
  // create environment
  SoapEnv=TSoapEnv::New();
  // set start-state
  SetState(TAppCS::S_Start);
}

void TAppCSState::SetState(const TStr& _StateId){
  // save previous state
  TStr PrevStateId=StateId;
  // set new state
  StateId=_StateId;
  // report message
  if (!Log.Empty()){
    Log->PutLogMsg("SetState", "From '"+PrevStateId+"' to '"+StateId+"'");
  }
}

/////////////////////////////////////////////////
// Application-Server
TAppSrv::TAppSrv(
 const int& PortN, const TStr& LogFNm, const int& HBeatTimerTout,
 const PNotify& Notify):
  TWebNetSrv(PortN, true), // inherited constructor
  TWebNetCltV(), // inherited constructor
  Log(TAppCSLog::New(LogFNm, -1, Notify)), // logging
  SesIdToCltDescH(), // active clients by session-id
  CltNmToSesIdH(), // active clients by unique-name
  HBeatTimer(), // heart-beat-timer (moved because of usage of 'this')
  ActConnBs(TAppCSActConnBs::New()), // active-connections
  NmToLogH(),
  Proxy(new TWebNetProxy(PortN+11,TStr("http://127.0.0.1:")+TInt::GetStr(PortN))) {
  // heart-beat-timer
  HBeatTimer=TAppCSTimer::New(TAppCS::F_HBeatTimer,
   "Srv", HBeatTimerTout, true, this);
}

TAppSrv::~TAppSrv(){
  HBeatTimer->StopTimer();
}

bool TAppSrv::IsLogSoapFuncNm(const TStr& SoapFuncNm){
  return
   (!SoapFuncNm.IsPrefix(TAppCS::F_Log))&&
   (SoapFuncNm!=TAppCS::F_HBeatTimer)&&
   (SoapFuncNm!=TAppCS::F_IsAlive);
}

TStr TAppSrv::GetNewSesId(){
  // prepare session-id
  TStr SesId;
  // repeat until session id changes
  do {
    //SesId=TMd5::GetMd5SigStr(TTm::GetUniTm().GetStr());
    SesId=TTm::GetCurUniTm().GetIdStr();
  } while (SesIdToCltDescH.IsKey(SesId));
  // return session-id
  return SesId;
}

bool TAppSrv::IsGetCltDescBySesId(
 const TStr& SesId, PAppCltDesc& CltDesc) const {
  return SesIdToCltDescH.IsKeyGetDat(SesId, CltDesc);
}

bool TAppSrv::IsGetCltDescBySockId(
 const int& SockId, PAppCltDesc& CltDesc) const {
  int SesIdToCltDescP=SesIdToCltDescH.FFirstKeyId();
  while (SesIdToCltDescH.FNextKeyId(SesIdToCltDescP)){
    CltDesc=SesIdToCltDescH[SesIdToCltDescP];
    if (CltDesc->SockId==SockId){
      return true;}
  }
  CltDesc=NULL;
  return false;
}

bool TAppSrv::IsGetCltDescByCltNm(
 const TStr& CltNm, PAppCltDesc& CltDesc) const {
  TStr SesId;
  if (CltNmToSesIdH.IsKeyGetDat(CltNm, SesId)){
    return IsGetCltDescBySesId(SesId, CltDesc);
  } else {
    return false;
  }
}

PAppCltDesc TAppSrv::AddNewCltDesc(
 const TStr& HostNm, const int& PortN, const int& SockId,
 const TStr& CltNm, const TStr& CltType){
  // create new session-id
  TStr SesId=GetNewSesId();
  // create new client-descriptor
  PAppCltDesc CltDesc=TAppCltDesc::New(SesId, HostNm, PortN);
  CltDesc->SockId=SockId;
  CltDesc->CltNm=CltNm;
  CltDesc->CltType=CltType;
  // add client-descriptor under session-id
  SesIdToCltDescH.AddDat(SesId, CltDesc);
  // add client-name
  TStr PrevSesId;
  if (CltNmToSesIdH.IsKeyGetDat(CltNm, PrevSesId)){
    DelIfCltDesc(PrevSesId);}
  CltNmToSesIdH.AddDat(CltNm, SesId);
  // add client-type
  CltDesc->CltType=CltType;
  // return client descriptor
  return CltDesc;
}

void TAppSrv::DelIfCltDesc(const TStr& SesId){
  // delete client from client-name table
  PAppCltDesc CltDesc; TStr CltNm;
  if (IsGetCltDescBySesId(SesId, CltDesc)){
    CltNm=CltDesc->CltNm;
    CltNmToSesIdH.DelIfKey(CltDesc->CltNm);
  }
  // delete client from session-id table
  SesIdToCltDescH.DelIfKey(SesId);
  // report to log
  Log->PutLogMsg("Info",
   TStr::Fmt("Delete client '%s' with session '%s'.", CltNm.CStr(), SesId.CStr()));
  /*// report event
  FetchSoapRq(TSoapRq::New(TAppCS::F_NotifyEvent,
   TAppCS::LogA_CreatorTime, TTm::GetCurUniTm().GetWebLogDateTimeStr(),
   TAppCS::LogA_EventTypeNm, "CancelSession",
   "SesId", SesId,
   "CltNm", CltNm));*/
}

void TAppSrv::OnHttpRq(const int& SockId, const PHttpRq& HttpRq){
  // collect important information
  TTm CurTm=TTm::GetCurUniTm(); // get current-time
  TStr FetchIdStr=HttpRq->GetFldVal(THttp::FetchIdFldNm); // fetch-id
  PUrl Url=HttpRq->GetUrl(); // get url
  TStr UrlStr=Url->GetUrlStr(); // get url-string
  TStr PeerHostNm=GetPeerHostNm(SockId); // get peer-name
  PSoapRq SoapRq=TSoapRq::New(HttpRq); // get soap-request
  TStr SoapFuncNm=SoapRq->GetFuncNm();
  TStr SoapRqMsgStr=UrlStr+" from "+PeerHostNm; // get soap-request message

  // report to log
  if (IsLogSoapFuncNm(SoapFuncNm)){
    Log->PutLogMsg("SoapRequest", SoapRqMsgStr);}

  // process soap-request and generate http-response
  PHttpResp HttpResp;
  if (SoapRq->IsOk()){
    if (SoapFuncNm==TAppCS::F_Info){
      HttpResp=OnHttpRq_Info();
    } else
    if (SoapFuncNm==TAppCS::F_GetCltList){
      HttpResp=OnHttpRq_GetCltList(SoapRq);
    } else
    if ((SoapFuncNm==TAppCS::F_GetCltInfo)||(SoapFuncNm==TAppCS::F_GetCltDesc)){
      HttpResp=OnHttpRq_GetCltInfo(SoapRq);
    } else
    if (SoapFuncNm==TAppCS::F_Connect){
      HttpResp=OnHttpRq_Connect(SoapRq, PeerHostNm, SockId);
    } else
    if (SoapFuncNm==TAppCS::F_Disconnect){
      HttpResp=OnHttpRq_Disconnect(SoapRq, PeerHostNm);
    } else
    if (SoapFuncNm==TAppCS::F_Log){
      HttpResp=OnHttpRq_OnLog(SoapRq);
    } else
    if (SoapFuncNm==TAppCS::F_HBeatTimer){
      OnHttpRq_OnHBeatTimer();
    } else
    if (IsCltOk(SoapRq, PeerHostNm)){
      if (SoapFuncNm==TAppCS::F_IsAlive){
        HttpResp=TSoapResp::GetHttpResp(TAppCS::F_IsAlive);
      } else
      if (SoapFuncNm==TAppCS::F_Broadcast){
        HttpResp=OnHttpRq_Broadcast(SoapRq);
      } else {
        HttpResp=OnFuncRq(SockId, SoapRq, Url);
      }
    } else {
      HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
       "Client admin info mismatch.");
    }
  } else {
    HttpResp=TSoapResp::GetFaultHttpResp(TSoap::DataEncodingUnknownCodeNm,
     "Invalid SOAP HTTP Request.");
  }

  // send response and/or log
  if (SockId!=-1){
    if (HttpResp.Empty()){
      Log->PutLogMsg("Warning", TStr("Empty Http-Response for ")+SoapRqMsgStr);
      HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm, "Empty Response.");
    }
    // fetch-id
    if (!FetchIdStr.Empty()){
      HttpResp->AddFldVal(THttp::FetchIdFldNm, FetchIdStr);}
    // send response
    SendHttpResp(SockId, HttpResp);
    // write log
    if (IsLogSoapFuncNm(SoapFuncNm)){
      // write log record
      Log->PutLogRec(HttpRq, HttpResp, PeerHostNm);
      // write soap-response
      PSoapResp SoapResp=TSoapResp::New(HttpResp);
      if (SoapResp->IsOk()){
        Log->PutLogMsg("SoapResponse", SoapResp->GetAsUrlPathStr());
      }
    }
  }
}

void TAppSrv::OnHttpRqError(const int& SockId, const TStr& MsgStr){
  Log->PutLogMsg("HttpRqError", MsgStr);
  PAppCltDesc CltDesc;
  if (IsGetCltDescBySockId(SockId, CltDesc)){
    DelIfCltDesc(CltDesc->SesId);
  }
}

TStr TAppSrv::GetPeerHostNm(const int& SockId) const {
  if (SockId==-1){
    return "internal";
  } else {
    TStr PeerHostNm=GetCltSock(SockId)->GetPeerIpNum();
#if 0
    // doesn't work on hosts with multiple adapters, fixed by ResolveCltIP
    if ((PeerHostNm.GetLc()=="localhost")||(PeerHostNm=="127.0.0.1")){
      PSockHost LocalSockHost=TSockHost::GetLocalSockHost();
      if (LocalSockHost->IsOk()){
        return LocalSockHost->GetIpNum();
      }
    }
#else
    if (PeerHostNm.GetLc()=="localhost") PeerHostNm="127.0.0.1";
#endif
    return PeerHostNm;
  }
}

void TAppSrv::ResolveCltIP(const PAppCltDesc CltDesc, int SockId, TStr &DestHost, TStr &DestProxy) {
  static const TStr LoopbackIP("127.0.0.1");
  static const TStr LocalhostNm("localhost");
  if (SockId==-1) {
    DestHost=LoopbackIP;
  } else {
    PSockHost Localhost=TSockHost::GetLocalSockHost();
    PSock Sock=GetCltSock(SockId);
    TStr PeerIP=Sock->GetPeerIpNum();
    TStr AdapterIP=Sock->GetLocalIpNum();
    TStr CltHostNm = CltDesc->HostNm;
    bool LocalPeerP = (Localhost->HasIpNum(PeerIP)||(PeerIP==LoopbackIP)||(PeerIP==LocalhostNm));
    bool LocalRedirP = (Localhost->HasIpNum(CltHostNm)||(CltHostNm==LoopbackIP)||(CltHostNm==LocalhostNm));
    if (LocalRedirP) { // redirect to local computer
      DestHost=LocalPeerP?LoopbackIP:AdapterIP;
      IAssert(LocalPeerP||(AdapterIP!=LoopbackIP));
    } else { // redirect to remote host
      DestHost=CltHostNm;
      if (!LocalPeerP) {
        unsigned long PeerAddr = ntohl(inet_addr(PeerIP.CStr()));
        unsigned long DestAddr = ntohl(inet_addr(DestHost.CStr()));
        if (((PeerAddr^DestAddr) & 0xFFFFFF00) != 0) {
          DestProxy=AdapterIP;
        }
        //test - use proxy for all remote machines regardless of network routing
        //DestProxy=AdapterIP;
      }
    }
  }
}


bool TAppSrv::IsCltOk(const PSoapRq& SoapRq, const TStr& PeerHostNm) const {
  TStr SesId=SoapRq->GetFldVal(TAppCS::A_SesId);
  PAppCltDesc CltDesc;
  if (IsGetCltDescBySesId(SesId, CltDesc)){
#if 0
    // following code doesn't work as some peers might have multiple IP adapters
    // and use round robin algorithms on them. There are also problems with
    // proxied requests.
    if (PeerHostNm==CltDesc->HostNm) {
      return true;
    }
#else
    return true;
#endif
  }
  return false;
}

PHttpResp TAppSrv::OnHttpRq_Info() const {
  TTm CurTm=TTm::GetCurUniTm();
  TChA HtmlChA;
  HtmlChA+="<html><head><title>Application Server Status</title></head>";
  HtmlChA+="<body>";
  HtmlChA+="<h1>Application Server Status</h1>";
  HtmlChA+="GMT-Time: ";
  HtmlChA+=CurTm.GetWebLogDateStr()+" "+CurTm.GetWebLogTimeStr();
  HtmlChA+="<br>";
  HtmlChA+="Active Clients: "; HtmlChA+=TInt::GetStr(GetClts());
  HtmlChA+="<br>";
  HtmlChA+="Server Port: ";
  HtmlChA+=TInt::GetStr(GetPortN());
  HtmlChA+="<br>";
  HtmlChA+="Server Up-Time (secs): ";
  HtmlChA+=TInt::GetStr(GetHBeatTimer()->GetSecs());
  HtmlChA+="<br>";
  HtmlChA+="Log-File-Name: ";
  HtmlChA+=GetLog()->GetLogFNm();
  HtmlChA+="<br>";
  HtmlChA+="Heart-Beat-TimeOut (msecs): ";
  HtmlChA+=TInt::GetStr(GetHBeatTimer()->GetTimeOut());
  HtmlChA+="<br>";
  HtmlChA+="Heart-Beat-Ticks: ";
  HtmlChA+=TInt::GetStr(GetHBeatTimer()->GetTicks());
  HtmlChA+="<br>";
  HtmlChA+="</body>";
  HtmlChA+="</html>";
  PHttpResp HttpResp=THttpResp::New(
   THttp::OkStatusCd, THttp::TextHtmlFldVal, false, TStrIn::New(HtmlChA));
  return HttpResp;
}

PHttpResp TAppSrv::OnHttpRq_GetCltList(const PSoapRq& SoapRq){
  // create soap-response
  PSoapResp SoapResp=TSoapResp::New(SoapRq->GetFuncNm());
  TAppCltDescV CltDescV; GetCltDescV(CltDescV);
  for (int CltDescN=0; CltDescN<CltDescV.Len(); CltDescN++){
    PAppCltDesc CltDesc=CltDescV[CltDescN];
    if (!CltDesc->CltNm.Empty()){
      SoapResp->AddFldNmVal(CltDesc->CltNm,
       CltDesc->HostNm+"/"+TInt::GetStr(CltDesc->PortN));
    }
  }
  // return http-reposnse
  return SoapResp->GetHttpResp();
}

PHttpResp TAppSrv::OnHttpRq_GetCltInfo(const PSoapRq& SoapRq){
  PHttpResp HttpResp;
  // prepare client-name
  TStr CltNm=SoapRq->GetFldVal(TAppCS::A_CltNm);
  // search for client-descriptor
  PAppCltDesc CltDesc;
  if (IsGetCltDescByCltNm(CltNm, CltDesc)){
    // create http response with host name and port
    HttpResp=TSoapResp::GetHttpResp(SoapRq->GetFuncNm(),
     TAppCS::A_Result, "T",
     TAppCS::A_HostNm, CltDesc->HostNm,
     TAppCS::A_PortN, TInt::GetStr(CltDesc->PortN),
     TAppCS::A_CltNm, CltDesc->CltNm,
     TAppCS::A_CltType, CltDesc->CltType);
  } else {
    // create http-response with message
    HttpResp=TSoapResp::GetHttpResp(SoapRq->GetFuncNm(),
     TAppCS::A_Result, "F",
     TAppCS::A_Msg, TStr::Fmt("Client '%s' not registered.", CltNm.CStr()));
  }
  // return http-reponse
  return HttpResp;
}

PHttpResp TAppSrv::OnHttpRq_Connect(
 const PSoapRq& SoapRq, const TStr& PeerHostNm, const int& SockId){
  IAssert(SoapRq->GetFuncNm()==TAppCS::F_Connect);
  // get client-port
  int CltPortN=SoapRq->GetFldVal(TAppCS::A_PortN).GetInt(-1);
  if (CltPortN==-1){
    return TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
     "Port parameter missing in Login function call.");
  } else {
    // collect client-info
    TStr CltNm=SoapRq->GetFldVal(TAppCS::A_CltNm);
    TStr CltType=SoapRq->GetFldVal(TAppCS::A_CltType);
    if (!IsCltByCltNm(CltNm)){
      // create client descriptor
      PAppCltDesc CltDesc=AddNewCltDesc(PeerHostNm, CltPortN, SockId, CltNm, CltType);
      // create & return http-response
      return TSoapResp::GetHttpResp(SoapRq->GetFuncNm(),
       TAppCS::A_SesId, CltDesc->SesId);
    } else {
      // create & return http-response
      return TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
       TStr::Fmt("Client '%s' already registered.", CltNm.CStr()));
    }
  }
}

PHttpResp TAppSrv::OnHttpRq_Disconnect( 
 const PSoapRq& SoapRq, const TStr& PeerHostNm){
  IAssert(SoapRq->GetFuncNm()==TAppCS::F_Disconnect);
  // collect client-info
  TStr SesId=SoapRq->GetFldVal(TAppCS::A_SesId); PAppCltDesc CltDesc;
  if (!SesId.Empty()&&IsGetCltDescBySesId(SesId, CltDesc)){
    DelIfCltDesc(CltDesc->SesId);
    return TSoapResp::GetHttpResp(SoapRq->GetFuncNm());
  } else {
    // create & return http-response
    return TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
     "Invalid disconnect request.");
  }
}

PHttpResp TAppSrv::OnHttpRq_OnLog(const PSoapRq& SoapRq){
  IAssert(SoapRq->GetFuncNm()==TAppCS::F_Log);
  // prepare log
  PAppCSLog CltLog;
  TStr LogNm=SoapRq->GetFldVal(TAppCS::A_LogNm);
  if (!NmToLogH.IsKeyGetDat(LogNm, CltLog)){
    CltLog=TAppCSLog::New(LogNm+".Log", -1, NULL);
    NmToLogH.AddDat(LogNm, CltLog);
  }
  // save message
  TStr MsgTypeNm=SoapRq->GetFldVal(TAppCS::A_MsgType);
  TStr MsgStr=SoapRq->GetFldVal(TAppCS::A_Msg);
  CltLog->PutLogMsg(MsgTypeNm, MsgStr);
  // return ok
  return TSoapResp::GetHttpResp(SoapRq->GetFuncNm());
}

void TAppSrv::OnHttpRq_OnHBeatTimer(){
  // send is-alive requests to clients
  TTm CurTm=TTm::GetCurUniTm();
  TAppCltDescV CltDescV; GetCltDescV(CltDescV);
  for (int CltDescN=0; CltDescN<CltDescV.Len(); CltDescN++){
    PAppCltDesc CltDesc=CltDescV[CltDescN];
    TTm LastHBeatTm=CltDesc->LastHBeatTm;
    if (TTm::GetDiffMSecs(CurTm, LastHBeatTm)>HBeatTimer->GetTimeOut()){
      if (CltDesc->ActiveHBeatRqP){
        DelIfCltDesc(CltDesc->SesId);
      } else {
        CltDesc->ActiveHBeatRqP=true;
        FetchSoapRq(CltDesc, TSoapRq::New(TAppCS::F_IsAlive));
      }
    }
  }
}

PHttpResp TAppSrv::OnHttpRq_Broadcast(const PSoapRq& SoapRq){
  IAssert(SoapRq->GetFuncNm()=="Broadcast");
  TStr BcCmNm=SoapRq->GetFldVal("Cm");
  TStr BcFldNm=SoapRq->GetFldVal("FldNm");
  TStr BcFldVal=SoapRq->GetFldVal("FldVal");
  TStr BcMsgStr=SoapRq->GetFldVal("Msg");
  TAppCltDescV CltDescV; GetCltDescV(CltDescV);
  for (int CltDescN=0; CltDescN<CltDescV.Len(); CltDescN++){
    PAppCltDesc CltDesc=CltDescV[CltDescN];
    FetchSoapRq(CltDesc, TSoapRq::New("Broadcast",
     "Cm", BcCmNm, "FldNm", BcFldNm, "FldVal", BcFldVal, "Msg", BcMsgStr));
  }
  // return ok
  return TSoapResp::GetHttpResp(SoapRq->GetFuncNm());
}

PHttpResp TAppSrv::OnFuncRq(const int &SockId, const PSoapRq& SoapRq, const PUrl& Url){
  // get function-name
  TStr FuncNm=SoapRq->GetFuncNm();
  // two ways of detecting client: (1) client-parameter (2) FuncNm=CltNm::FuncNm
  TStr CltNm;
  // check for 'client' parameter in soap-request
  if (SoapRq->IsFldNm(TAppCS::A_CltNm)){
    CltNm=SoapRq->GetFldVal(TAppCS::A_CltNm);
  }
  // check for client-name in function-name
  if (CltNm.Empty()){
    // split function-name to client-name and client-function-name
    TStr _CltFuncNm; FuncNm.SplitOnStr(CltNm, "::", _CltFuncNm);
  }
  // call function
  PHttpResp HttpResp;
  if (!CltNm.Empty()){
    // get client
    PAppCltDesc CltDesc;
    if (IsGetCltDescByCltNm(CltNm, CltDesc)){
      // add soap-request-creator client-info
      {TStr SesId=SoapRq->GetFldVal(TAppCS::A_SesId);
      PAppCltDesc SoapRqCltDesc;
      if (IsGetCltDescBySesId(SesId, SoapRqCltDesc)){
        SoapRq->AddFldNmVal(TAppCS::LogA_CreatorNm, SoapRqCltDesc->CltNm);
      }}
      // prepare redirection-url
      SoapRq->AddFldNmVal(TAppCS::A_Redir, "T");
#if 0
      TStr RedirUrlStr=TStr()+
       "http://"+CltDesc->HostNm+":"+TInt::GetStr(CltDesc->PortN)+"/"+
       SoapRq->GetAsUrlPathStr();
      // prepare redirection-http-response
      HttpResp=THttpResp::New(300, "", false, NULL, RedirUrlStr);
#else
      TStr DestHost, DestProxy, RedirHost;
      ResolveCltIP(CltDesc,SockId,DestHost,DestProxy);
      DestHost += ":"+TInt::GetStr(CltDesc->PortN);
      if (!DestProxy.Empty()) {
        DestProxy += ":"+TInt::GetStr(Proxy->GetPortN());
        RedirHost=DestProxy;
      } else {
        RedirHost=DestHost;
      }
      // prepare redirection-http-response
      TStr RedirUrlStr=TStr()+"http://"+RedirHost+"/"+SoapRq->GetAsUrlPathStr();
      Log->PutLogMsg("Redirect", TStr()+GetPeerHostNm(SockId)+" -> "+RedirUrlStr);
      HttpResp=THttpResp::New(300, "", false, NULL, RedirUrlStr);
      if (!DestProxy.Empty()) HttpResp->AddFldVal(THttp::HostFldNm,DestHost);
#endif
    } else {
      HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
       TStr()+"Client for function call not active: "+CltNm, FuncNm);
    }
  } else {
    HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
     "Unknown SOAP function call", FuncNm);
  }
  return HttpResp;
}

int TAppSrv::FetchSoapRq(const PAppCltDesc& CltDesc, const PSoapRq& SoapRq){
  // add session-id for current client
  SoapRq->AddFldNmVal(TAppCS::A_SesId, CltDesc->SesId);
  // create url
  PUrl SoapRqUrl=SoapRq->GetAsUrl(CltDesc->HostNm, CltDesc->PortN);
  // fetch url
  int FetchId=FetchUrl(SoapRqUrl);
  // add active-connection
  ActConnBs->AddActConn(FetchId,
   TAppCSActConn::New(SoapRq, CltDesc->HostNm, CltDesc->PortN,1));
  // fetch url
  Log->PutLogMsg("FetchSoapRq",
   TInt::GetStr(FetchId, "[FetchId:%d]")+SoapRq->GetAsUrlPathStr());
  // return fetch-id
  return FetchId;
}

void TAppSrv::OnHttpResp(const int& FetchId, const PHttpResp& HttpResp){
  // get soap-response
  PSoapResp SoapResp=TSoapResp::New(HttpResp);
  // analyse soap-response
  if (SoapResp->IsOk()){
    if (SoapResp->IsResult()){
      // get soap-response function-name and result-value
      TStr SoapRespFuncNm=SoapResp->GetFuncNm();
      TStr SoapRespResultVal=SoapResp->GetFldVal(TAppCS::A_Result);
      // get matching soap-request and its function-name
      PSoapRq SoapRq=ActConnBs->GetActConn(FetchId)->GetSoapRq();
      TStr SoapRqFuncNm=SoapRq->GetFuncNm();
      IAssert(SoapRqFuncNm==SoapRespFuncNm);
      // delete matching soap-request
      ActConnBs->DelActConn(FetchId);
      // process known functions
      if (SoapRespFuncNm==TAppCS::F_IsAlive){
        TStr SoapRqSesId=SoapRq->GetFldVal(TAppCS::A_SesId); // retrieve session-id
        TStr SoapRespSesId=SoapResp->GetFldVal(TAppCS::A_SesId); // retrieve session-id
        if (SoapRqSesId==SoapRespSesId){
          PAppCltDesc CltDesc;
          if (IsGetCltDescBySesId(SoapRespSesId, CltDesc)){ // get client-descriptor
            CltDesc->LastHBeatTm=TTm::GetCurUniTm(); // update touch-time
            CltDesc->ActiveHBeatRqP=false; // unactivate touch wait-flag
          }
        } else {
          Log->PutLogMsg("Warning",
           "Session numbers (request & response) for IsAlive don't match.");
        }
      } else {
        // soap-response not handeled
        Log->PutLogMsg("Warning",
         "Unhandled Soap-Response", SoapResp->GetAsUrlPathStr());
      }
    } else {
      IAssert(SoapResp->IsFault());
      // fail-soap-response - pass to on-error handler
      OnHttpRespError(FetchId, TStr("SoapFault: ")+SoapResp->GetFaultReasonStr());
    }
  } else {
    // invalid soap-response - pass to on-error handler
    PSoapResp SoapResp=TSoapResp::New(HttpResp);
    OnHttpRespError(FetchId, "Invalid SOAP Response");
  }
}

void TAppSrv::OnHttpRespError(const int& FetchId, const TStr& MsgStr){
  // get matching soap-request
  PSoapRq SoapRq=ActConnBs->GetActConn(FetchId)->GetSoapRq();
  // report error
  Log->PutLogMsg("Error", MsgStr, SoapRq->GetAsUrlPathStr());
  // delete matching soap-request
  ActConnBs->DelActConn(FetchId);
  // retrieve & delete session-id
  TStr SesId=SoapRq->GetFldVal(TAppCS::A_SesId);
  DelIfCltDesc(SesId);
}

const int TAppSrv::DfPortN=8888;

/////////////////////////////////////////////////
// Application-Client
TAppClt::TAppClt(
 const TStr& LogFNm,
 const TStr& _CltNm,
 const TStr& _CltType,
 const int& HBeatTimerTout,
 const bool& _AgentP,
 const TStr& _SrvHostNm,
 const int& _SrvPortN,
 const int& CltPortN,
 const PNotify& Notify):
  TWebNetSrv(CltPortN, false), // inherited constructor
  TWebNetCltV(), // inherited constructor
  Log(TAppCSLog::New(LogFNm, -1, Notify)), // logging
  CltNm(_CltNm), // unique client identification
  CltType(_CltType), // client type
  SrvHostNm(_SrvHostNm), SrvPortN(_SrvPortN), // server info
  AgentP(_AgentP), // agent client
  AgentHostNmPortNPrV(), // list of agents connected to client
  HBeatTimer(), // hear-beat-timer (moved because of usage of 'this')
  LastHBeatTm(TTm::GetCurUniTm()), // last hbeat time
  ActiveIsAliveFId(-1), // active hbeat fetch-id
  ActConnBs(TAppCSActConnBs::New()), // active-connections
  SesId(), // session-id
  State(TAppCSState::New(Log)), // state
  DelayTimer(), // waiting for next connect trial
  FuncFetchIdH(){ // fetch-ids
  // heart-beat-timer
  HBeatTimer=TAppCSTimer::New(TAppCS::F_HBeatTimer,
   CltNm, HBeatTimerTout, true, this);
  // delay-timer
  DelayTimer=TAppCSTimer::New(TAppCS::F_DelayTimer, CltNm, 0, false, this);
}

TAppClt::~TAppClt(){
  HBeatTimer->StopTimer();
}

const int TAppClt::DfPortN=TAppSrv::DfPortN+1;

void TAppClt::OnHttpRq(const int& SockId, const PHttpRq& HttpRq){
  // collect important information
  TTm CurTm=TTm::GetCurUniTm(); // get current-time
  TStr FetchIdStr=HttpRq->GetFldVal(THttp::FetchIdFldNm);
  PUrl Url=HttpRq->GetUrl(); // get url
  TStr UrlStr=Url->GetUrlStr(); // get url-string
  TStr PeerHostNm=GetPeerHostNm(SockId); // get peer-name
  PSoapRq SoapRq=TSoapRq::New(HttpRq); // get soap-request
  TStr SoapFuncNm=SoapRq->GetFuncNm();
  TStr SoapRqMsgStr=UrlStr+" from "+PeerHostNm; // get soap-request message

  // report to log
  if (TAppSrv::IsLogSoapFuncNm(SoapFuncNm)){
    PutLogMsg("SoapRequest", SoapRqMsgStr, true);}

  // process soap-request and generate http-response
  PHttpResp HttpResp;
  if (SoapRq->IsOk()){
    if (SoapFuncNm==TAppCS::F_Info){
      HttpResp=OnHttpRq_Info();
    } else
    if (SoapFuncNm==TAppCS::F_GetStateInfo){
      HttpResp=OnHttpRq_GetStateInfo(SoapRq);
    } else
    if (SoapFuncNm==TAppCS::F_HBeatTimer){
      OnHttpRq_OnHBeatTimer();
    } else
    if ((SoapFuncNm==TAppCS::F_IsAlive)||(SoapFuncNm==TAppCS::F_IsConn)){
      HttpResp=TSoapResp::GetHttpResp(SoapFuncNm, TAppCS::A_SesId, SesId);
    } else
    if (SoapFuncNm==TAppCS::F_RegAgent){
      HttpResp=OnHttpRq_RegAgent(SoapRq, PeerHostNm);
    } else {
      bool RedirP=SoapRq->GetFldVal(TAppCS::A_Redir)=="T";
      bool AgentP=SoapRq->GetFldVal(TAppCS::A_Agent)=="T";
      if ((SockId!=-1)&&(!RedirP)&&(!AgentP)&&(!IsSesId())){
        HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
         "Session not defined.");
      } else
      if ((SockId!=-1)&&(!RedirP)&&(!AgentP)&&
       (GetSesId()!=SoapRq->GetFldVal(TAppCS::A_SesId))){
        TStr SesId1=GetSesId();
        TStr SesId2=SoapRq->GetFldVal(TAppCS::A_SesId);
        HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
         "Session-ids don't match.");
      } else {
        HttpResp=OnFuncRq(SoapRq);
      }
    }
  } else {
    HttpResp=TSoapResp::GetFaultHttpResp(TSoap::DataEncodingUnknownCodeNm,
     "Invalid SOAP HTTP Request.");
  }

  // send response and/or log
  if (SockId!=-1){
    if (HttpResp.Empty()){
      PutLogMsg("Warning", TStr("Empty Http-Response for ")+SoapRqMsgStr, true);
      HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm, "Empty Response.");
    }
    // fetch-id
    if (!FetchIdStr.Empty()){
      HttpResp->AddFldVal(THttp::FetchIdFldNm, FetchIdStr);}
    // send response
    SendHttpResp(SockId, HttpResp);
    // write log
    if (TAppSrv::IsLogSoapFuncNm(SoapFuncNm)){
      // write log record
      Log->PutLogRec(HttpRq, HttpResp, PeerHostNm);
      // write soap-response
      PSoapResp SoapResp=TSoapResp::New(HttpResp);
      if (SoapResp->IsOk()){
        PutLogMsg("SoapResponse", SoapResp->GetAsUrlPathStr(), true);
      }
    }
  }
}

void TAppClt::OnHttpRqError(const int& SockId, const TStr& MsgStr){
}

TStr TAppClt::GetPeerHostNm(const int& SockId) const {
  if (SockId==-1){
    return "internal";
  } else {
    TStr PeerHostNm=GetCltSock(SockId)->GetPeerIpNum();
    if ((PeerHostNm.GetLc()=="localhost")||(PeerHostNm=="127.0.0.1")){
      PSockHost LocalSockHost=TSockHost::GetLocalSockHost();
      if (LocalSockHost->IsOk()){
        return LocalSockHost->GetIpNum();
      } else {
        return PeerHostNm;
      }
    } else {
      return PeerHostNm;
    }
  }
}

PHttpResp TAppClt::OnHttpRq_Info() const {
  TTm CurTm=TTm::GetCurUniTm();
  TChA HtmlChA;
  HtmlChA+="<html><head><title>Application Client Status</title></head>";
  HtmlChA+="<body>";
  HtmlChA+="<h1>Application Client Status</h1>";
  HtmlChA+="GMT-Time: ";
  HtmlChA+=CurTm.GetWebLogDateStr()+" "+CurTm.GetWebLogTimeStr();
  HtmlChA+="<br>";
  HtmlChA+="Client Port: ";
  HtmlChA+=TInt::GetStr(GetPortN());
  HtmlChA+="<br>";
  HtmlChA+="Client Up-Time (secs): ";
  HtmlChA+=TInt::GetStr(GetHBeatTimer()->GetSecs());
  HtmlChA+="<br>";
  HtmlChA+="Server-Host-Name: ";
  HtmlChA+=GetSrvHostNm();
  HtmlChA+="<br>";
  HtmlChA+="Server-Host-Name: ";
  HtmlChA+=TInt::GetStr(SrvPortN);
  HtmlChA+="<br>";
  HtmlChA+="Log-File-Name: ";
  HtmlChA+=GetLog()->GetLogFNm();
  HtmlChA+="<br>";
  HtmlChA+="Heart-Beat-TimeOut (msecs): ";
  HtmlChA+=TInt::GetStr(GetHBeatTimer()->GetTimeOut());
  HtmlChA+="<br>";
  HtmlChA+="Heart-Beat-Ticks: ";
  HtmlChA+=TInt::GetStr(GetHBeatTimer()->GetTicks());
  HtmlChA+="<br>";
  HtmlChA+="</body>";
  HtmlChA+="</html>";
  PHttpResp HttpResp=THttpResp::New(
   THttp::OkStatusCd, THttp::TextHtmlFldVal, false, TStrIn::New(HtmlChA));
  return HttpResp;
}

PHttpResp TAppClt::OnHttpRq_GetStateInfo(const PSoapRq& SoapRq) const {
  // prepare result
  PSoapResp SoapResp=TSoapResp::New(SoapRq->GetFuncNm());
  // add state-info
  SoapResp->AddFldNmVal("StateId", State->GetStateId());
  //SoapResp->AddSoapEnv(State->GetCtxEnv());
  // return result
  return SoapResp->GetHttpResp();
}

void TAppClt::OnHttpRq_OnHBeatTimer(){
  // check if connection alive
  if (IsSesId()){
    if (ActiveIsAliveFId==-1){
      ActiveIsAliveFId=FetchSoapRq(TSoapRq::New(TAppCS::F_IsAlive));
    }
  }
}

PHttpResp TAppClt::OnHttpRq_RegAgent(
 const PSoapRq& SoapRq, const TStr& PeerHostNm){
  // get agent host-name & ip
  TStr AgentHostNm=PeerHostNm;
  int AgentPortN=SoapRq->GetFldVal(TAppCS::A_PortN).GetInt(0);
  // register agent
  AgentHostNmPortNPrV.AddUnique(TStrIntPr(AgentHostNm, AgentPortN));
  // return ok
  return TSoapResp::GetHttpResp(SoapRq->GetFuncNm(), TAppCS::A_Result, "T");
}

/*PHttpResp TAppClt::OnFuncRq(const PSoapRq& SoapRq){
  // empty function handler
  return TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
   "Unknown SOAP function call", SoapRq->GetFuncNm());
}*/

void TAppClt::OnHttpResp(const int& FetchId, const PHttpResp& HttpResp){
  // get matching soap request
  PAppCSActConn ActConn=ActConnBs->GetActConn(FetchId);
  PSoapRq SoapRq=ActConn->GetSoapRq();
  TStr SoapFuncNm=SoapRq->GetFuncNm();
  // get soap-response
  PSoapResp SoapResp=TSoapResp::New(HttpResp);
  // delete matching soap-request
  ActConnBs->DelActConn(FetchId);

  if (ActConn->GetRetryCnt()!=3) {
    PutLogMsg("OnFetch",
     TStr::Fmt("[FetchId:%d] %s", FetchId,
      (TStr()+"SoapRq retry count="+TInt::GetStr(3-ActConn->GetRetryCnt())).CStr()), false);
  }
  // report to log
  if (TAppSrv::IsLogSoapFuncNm(SoapFuncNm)){
    PutLogMsg("OnFetch",
     TStr::Fmt("[FetchId:%d] %s", FetchId, SoapResp->GetAsUrlPathStr().CStr()), false);
  }
  // prepare fault soap-response if fetched-soap-response is ill-formed
  if (!SoapResp->IsOk()){
    PHttpResp HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm,
     "Invalid SOAP Response format.");
    SoapResp=TSoapResp::New(HttpResp);
  }
  // process is-alive-response
  if ((ActiveIsAliveFId!=-1)&&(FetchId==ActiveIsAliveFId)){
    TStr SoapRespFuncNm=SoapResp->GetFuncNm();
    if (SoapRespFuncNm==TAppCS::F_IsAlive){
      // ok
      LastHBeatTm=TTm::GetCurUniTm(); // update is-alive update-time
      ActiveIsAliveFId=-1; // deactivate is-alive fetch-id
    } else {
      // error
      SesId=""; // cancel session
      ActiveIsAliveFId=-1; // deactivate is-alive fetch-id
      SetState(TAppCS::S_Start); // set start-state
    }
  }
  // add fetch-id
  SoapResp->AddFldNmVal(TAppCS::A_FetchId, TInt::GetStr(FetchId));
  // process soap-response
  OnFetchSoapResp(SoapRq, SoapResp);
}

void TAppClt::OnHttpRespError(const int& FetchId, const TStr& MsgStr){
  // get matching soap request
  PAppCSActConn ActConn=ActConnBs->GetActConn(FetchId);
  PSoapRq SoapRq=ActConn->GetSoapRq();
  // retry...
  int RetryCnt=ActConn->GetRetryCnt();
  if (--RetryCnt>0) {
    ActConnBs->DelActConn(FetchId);
    IAssert(FetchId==FetchSoapRq(SoapRq,ActConn->GetHostNm(),ActConn->GetPortN(),RetryCnt,FetchId));
    return;
  }
  // create fault http-response
  PHttpResp HttpResp=TSoapResp::GetFaultHttpResp(TSoap::SenderCodeNm, MsgStr);
  PSoapResp SoapResp=TSoapResp::New(HttpResp);
  // report to log
  PutLogMsg("OnFetchError",
   TStr::Fmt("[FetchId:%d] %s", FetchId, MsgStr.CStr()), false);
  // cancel session if error appeared in is-alive function
  if ((ActiveIsAliveFId!=-1)&&(FetchId==ActiveIsAliveFId)){
    SesId=""; // cancel session
    ActiveIsAliveFId=-1; // deactivate is-alive fetch-id
    SetState(TAppCS::S_Start); // set start-state
  }
  // cancel agent if error //** not resolved yet
  //if (AgentHostNmPortNPrV.Len()>0){
  //  TStr HostNm=ActConnBs->GetActConn(FetchId)->GetHostNm();
  //  int PortN=ActConnBs->GetActConn(FetchId)->GetPortN();
  //  AgentHostNmPortNPrV.DelIfIn(TStrIntPr(HostNm, PortN));
  //}
  // delete matching soap-request
  ActConnBs->DelActConn(FetchId);
  // add fetch-id
  SoapResp->AddFldNmVal(TAppCS::A_FetchId, TInt::GetStr(FetchId));
  // process soap-response
  OnFetchSoapResp(SoapRq, SoapResp);
}

int TAppClt::FetchSoapRq(
 const PSoapRq& SoapRq, const TStr& HostNm, const int& PortN, const int &RetryCnt, const int &_FetchId){
  // add session-id if already logged-in
  if (IsSesId()){
    SoapRq->AddFldNmVal(TAppCS::A_SesId, GetSesId());}
  if (IsAgent()){
    SoapRq->AddFldNmVal(TAppCS::A_Agent, "T");}
  // get url
  PUrl SoapRqUrl;
  if (HostNm.Empty()){
    SoapRqUrl=SoapRq->GetAsUrl(SrvHostNm, SrvPortN);
  } else {
    SoapRqUrl=SoapRq->GetAsUrl(HostNm, PortN);
  }
#if 0
  // fetch url
  TStr CltNm, FuncNm=SoapRq->GetFuncNm();
  // check for 'client' parameter in soap-request
  if ((FuncNm!=TAppCS::F_Connect)&&SoapRq->IsFldNm(TAppCS::A_CltNm)){
    CltNm=SoapRq->GetFldVal(TAppCS::A_CltNm);
  }
  // check for client-name in function-name
  if (CltNm.Empty()){
    // split function-name to client-name and client-function-name
    TStr _CltFuncNm;
    FuncNm.SplitOnStr(CltNm, "::", _CltFuncNm);
    if (_CltFuncNm.Empty()) CltNm.Clr();
  }
  int FetchId=FetchUrl(SoapRqUrl,_FetchId,CltNm);
#else
  // fetch url
  int FetchId=FetchUrl(SoapRqUrl,_FetchId);
#endif  
  if (TAppSrv::IsLogSoapFuncNm(SoapRq->GetFuncNm())){
    PutLogMsg("FetchSoapRq",
     TInt::GetStr(FetchId, "[FetchId:%d]")+SoapRq->GetAsUrlPathStr(), false);
  }
  // add active-connection
  ActConnBs->AddActConn(FetchId,
   TAppCSActConn::New(SoapRq, SoapRqUrl->GetHostNm(), SoapRqUrl->GetPortN(), RetryCnt));
  // return fetch-id
  return FetchId;
}

void TAppClt::OnFetchSoapResp(const PSoapRq& SoapRq, const PSoapResp& SoapResp){
  IAssert(SoapRq->IsOk());
  IAssert(SoapResp->IsOk());
  if (SoapResp->IsResult()){
    // functions
    TStr SoapRespFuncNm=SoapResp->GetFuncNm();
    if (SoapRespFuncNm==TAppCS::F_Connect){
      SesId=SoapResp->GetFldVal(TAppCS::A_SesId); // set session-id
      OnStateExe(setSoapResp, SoapRq, SoapResp); // process state
    } else
    if (SoapRespFuncNm==TAppCS::F_IsAlive){
    } else
    if (SoapRespFuncNm==TAppCS::F_NotifyStateChange){
    } else
    if (SoapRespFuncNm==TAppCS::F_NotifyEvent){
    } else
    if (SoapRespFuncNm==TAppCS::F_Log){
    } else {
      OnFuncResp(SoapRq, SoapResp);
    }
  } else {
    IAssert(SoapResp->IsFault());
    OnStateExe(setSoapResp, SoapRq, SoapResp); // process state
  }
}

/*void TAppClt::OnFuncResp(const PSoapRq& SoapRq, const PSoapResp& SoapResp){
  // soap-response not handeled
  TStr FuncNm=SoapResp->GetFuncNm();
  GetLog()->PutLogMsg("Warning", "Unhandeled Soap-Response", FuncNm);
}*/

void TAppClt::PutLogMsg(
 const TStr& MsgType, const TStr& MsgStr, const bool& SendToSrvP){
  Log->PutLogMsg(MsgType, MsgStr);
  if ((false)&&(SendToSrvP)){
    // sending log-message to server
    FetchSoapRq(TSoapRq::New(TAppCS::F_Log,
     TAppCS::A_LogNm, CltNm,
     TAppCS::A_MsgType, MsgType,
     TAppCS::A_Msg, MsgStr));
  }
}

PHttpResp TAppClt::OnConnectRqAndResp(
 const TStr& NextSId,
 const TAppCsSET& ExeType, const PSoapRq& SoapRq, const PSoapResp& SoapResp,
 bool& BreakLoopP){
  // get shortcuts: log, state, state-id, rq/resp-func-name, fetch-id, result
  PAppCSLog Log; PAppCSState State; TStr StateId;
  TStr RqFuncNm; TStr RespFuncNm; TStr RespFetchId; bool RespResultP;
  GetStateExeShortcuts(Log, State, StateId,
   SoapRq, RqFuncNm, SoapResp, RespFuncNm, RespFetchId, RespResultP);
  BreakLoopP=true;
  // Connect-Request
  if (StateId==TAppCS::S_ConnectRq){
    // cancel session if still exists
    if (IsSesId()){
      SesId="";
    }
    // send connect-request
    FetchSoapRqSaveFId(TSoapRq::New(TAppCS::F_Connect,
     TAppCS::A_PortN, TInt::GetStr(GetPortN()),
     TAppCS::A_CltNm, GetCltNm()));
    // move to connect-wait state
    SetState(TAppCS::S_ConnectResp);
    return NULL;
  } else
  // Connect-Response
  if (StateId==TAppCS::S_ConnectResp){
    if ((GetFId(TAppCS::F_Connect)==RespFetchId)&&(RespResultP)){
      // move to connected state
      SetState(NextSId);
    } else {
      // start delay timer
      StartDelayTimer();
      // move to connect-request state
      SetState(TAppCS::S_ConnectRq);
      return NULL;
    }
  }
  BreakLoopP=false;
  return NULL;
}

void TAppClt::StartDelayTimer(const int& DelayTout){
  if (DelayTout==-1){
    DelayTimer->StartTimer(TAppCS::DTmrA_DelayTimerTout);
  } else {
    DelayTimer->StartTimer(DelayTout);
  }
}

void TAppClt::StopDelayTimer(){
  DelayTimer->StopTimer();
}

int TAppClt::FetchSoapRqSaveFId(
 const PSoapRq& SoapRq, const TStr& HostNm, const int& PortN){
  int FetchId=FetchSoapRq(SoapRq, HostNm, PortN);
  PutFId(SoapRq->GetFuncNm(), TInt::GetStr(FetchId));
  return FetchId;
}

void TAppClt::GetStateExeShortcuts(
 PAppCSLog& Log,
 PAppCSState& State, TStr& StateId,
 const PSoapRq& SoapRq, TStr& SoapRqFuncNm,
 const PSoapResp& SoapResp, TStr& SoapRespFuncNm, TStr& SoapRespFetchId,
 bool& SoapRespResultP){
  // get logging
  Log=GetLog();
  // get state-info
  State=GetState();
  StateId=State->GetStateId();
  // get soap-request function-name
  if (SoapRq.Empty()){
    SoapRqFuncNm="";
  } else {
    SoapRqFuncNm=SoapRq->GetFuncNm();
  }
  // get soap-response function-name & fetch-id
  if (SoapResp.Empty()){
    SoapRespFuncNm="";
    SoapRespFetchId="";
    SoapRespResultP=false;
  } else {
    SoapRespFuncNm=SoapResp->GetFuncNm();
    SoapRespFetchId=SoapResp->GetFldVal(TAppCS::A_FetchId);
    SoapRespResultP=SoapResp->IsResult();
  }
}

void TAppClt::BroadcastToAgents(const PSoapRq& SoapRq){
  if (GetAgents()>0){
    SoapRq->AddFldNmVal(TAppCS::A_Agent, "T");
    for (int AgentN=0; AgentN<GetAgents(); AgentN++){
      TStr HostNm; int PortN; GetAgentHostNmPortN(AgentN, HostNm, PortN);
      FetchSoapRq(SoapRq, HostNm, PortN);
    }
  }
}

void TAppClt::SetState(const TStr& StateId, const TStr& MsgStr,
 const TStr& TbNm1, const PSoapTb& Tb1,
 const TStr& TbNm2, const PSoapTb& Tb2){
  // set new state
  TStr OldStateId=State->GetStateId();
  State->SetState(StateId);
  TStr NewStateId=State->GetStateId();

  /*if (NewStateId=="Server"){
    FetchSoapRq(TSoapRq::New(TAppCS::F_NotifyEvent,
     TAppCS::LogA_CreatorTime, TTm::GetCurUniTm().GetWebLogDateTimeStr(),
     TAppCS::LogA_EventTypeNm, "StateChange",
     "OldState", OldStateId,
     "NewState", NewStateId));
  }*/

  // notify agents about state-change
  if (GetAgents()>0){
    /*// prepare soap-request
    PSoapRq SoapRq=TSoapRq::New(TAppCS::F_NotifyStateChange,
     TAppCS::A_OldState, OldStateId,
     TAppCS::A_NewState, NewStateId);
    // add message
    if (!MsgStr.Empty()){
      SoapRq->AddFldNmVal(TAppCS::A_Msg, MsgStr);}
    // add tables
    if ((!TbNm1.Empty())&&(!Tb1.Empty())){
      SoapRq->AddTbFld(TbNm1, Tb1);}
    if ((!TbNm2.Empty())&&(!Tb2.Empty())){
      SoapRq->AddTbFld(TbNm2, Tb2);}
    // broadcast state-change
    BroadcastToAgents(SoapRq);*/
  }
}

