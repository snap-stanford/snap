//////////////////////////////////////////////////
// Application-Client-Server
class TAppCS{
public:
  // generic arguments
  static TStr A_Result;
  static TStr A_Msg;
  static TStr A_MsgType;
  static TStr A_Continue;
  static TStr A_Browse;
  static TStr A_SesId;
  static TStr A_FetchId;
  static TStr A_Redir;
  static TStr A_Agent;
  static TStr A_HostNm;
  static TStr A_PortN;
  static TStr A_UsrNm;
  static TStr A_PwdStr;
  static TStr A_PrivNm;
  static TStr A_PrivId;
  static TStr A_CltNm;
  static TStr A_CltType;
  static TStr A_State;
  static TStr A_PrevState;
  static TStr A_OldState;
  static TStr A_NewState;
  static TStr A_AbortState;
  static TStr A_SetStateTm;
  static TStr A_LogNm;

  // functions
  static TStr F_Info;
  static TStr F_GetStateInfo;
  static TStr F_GetCltList;
  static TStr F_GetCltInfo;
  static TStr F_GetCltDesc;
  static TStr F_IsAlive;
  static TStr F_IsConn;
  static TStr F_Log;
  static TStr F_Connect;
  static TStr F_Disconnect;
  static TStr F_DelayTimer;
  static TStr F_RegAgent;
  static TStr F_Confirm;
  static TStr F_Cancel;
  static TStr F_Notify;
  static TStr F_NotifyStateChange;
  static TStr F_Abort;

  // function hbeat-timer
  static TStr F_HBeatTimer;
  static TStr TmA_TimerId;
  static TStr TmA_CtxNm;
  static TStr TmA_Ticks;

  // function-broadcast
  static TStr F_Broadcast;
  static TStr BcA_Cm;
  static TStr BcA_FldNm;
  static TStr BcA_FldVal;
  static TStr BcA_Msg;

  // logging
  static TStr F_NotifyEvent;
  static TStr F_NotifyRawEvent;
  static TStr LogA_EventTypeNm;
  static TStr LogA_TestMode;
  static TStr LogA_CreatorNm;
  static TStr LogA_CreatorTime;

  // settings
  static int DTmrA_DelayTimerTout;

  // states
  static TStr S_Start;
  static TStr S_End;
  static TStr S_ConnectRq;
  static TStr S_ConnectResp;
  static TStr S_Connected;
  static TStr S_Server;
};

//////////////////////////////////////////////////
// Application-Client-Server-Logging
ClassTP(TAppCSLog, PAppCSLog)//{
private:
  PNotify Notify;
  TStr LogFNm;
  PSOut LogSOut; FILE* fLog;
  int64 MxFLen;
  UndefDefaultCopyAssign(TAppCSLog);
public:
  TAppCSLog(const TStr& _LogFNm, const int64& _MxFLen, const PNotify& _Notify);
  static PAppCSLog New(
   const TStr& LogFNm, const int64& MxFLen, const PNotify& Notify){
    return PAppCSLog(new TAppCSLog(LogFNm, MxFLen, Notify));}
  ~TAppCSLog(){}
  TAppCSLog(TSIn&){Fail;}
  static PAppCSLog Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TStr GetLogFNm() const {return LogFNm;}
  void ResetLogIfTooBig();

  void PutLogHd();
  void PutLogRec(const PHttpRq& HttpRq, const PHttpResp& HttpResp,
   const TStr& PeerHostNm);
  void PutLogMsg(const TStr& MsgType, const TStr& MsgStr);
  void PutLogMsg(const TStr& MsgType, const TStr& MsgStr, const TStr& MsgArg);

  static TStr GetHomeNrFPath(){
    return TStr::GetNrFPath(TDir::GetExeDir());}
};

//////////////////////////////////////////////////
// Application-Client-Server-Timer
class TAppCSTimer: public TTTimer{
private:
  TWebNetSrv* WebNetSrv;
  TStr TimerNm;
  bool RepeatP;
  TStr CtxNm;
public:
  TAppCSTimer(const TStr& _TimerNm, const TStr& _CtxNm,
   const int& TimerTout, const bool& _RepeatP, TWebNetSrv* _WebNetSrv);
  static PTimer New(const TStr& TimerNm, const TStr& CtxNm,
   const int& TimerTout, const bool& RepeatP, TWebNetSrv* WebNetSrv){
    return new TAppCSTimer(TimerNm, CtxNm, TimerTout, RepeatP, WebNetSrv);}

  TStr GetTimerNm() const {return TimerNm;}
  bool IsRepeat() const {return RepeatP;}
  TStr GetCtxNm() const {return CtxNm;}

  void OnTimeOut();
};

//////////////////////////////////////////////////
// Application-Client-Server-State
typedef enum {
  setUndef, setExe, setSoapRq, setSoapResp} TAppCSStateExeType;
typedef TAppCSStateExeType TAppCsSET;

ClassTP(TAppCSState, PAppCSState)//{
private:
  TStr StateId;
  PSoapEnv SoapEnv;
  PAppCSLog Log;
  UndefDefaultCopyAssign(TAppCSState);
public:
  TAppCSState(const PAppCSLog& _Log);
  static PAppCSState New(const PAppCSLog& Log){
    return new TAppCSState(Log);}

  // component retrieval
  TStr GetStateId() const {return StateId;}
  PSoapEnv GetSoapEnv() const {return SoapEnv;}
  void PutLog(const PAppCSLog& _Log){Log=_Log;}
  PAppCSLog GetLog() const {return Log;}

  // setting state
  void PrepState();
  void SetState(const TStr& _StateId);
  void PutStateId(const TStr& _StateId){StateId=_StateId;}
};

/////////////////////////////////////////////////
// Application-Client-Server-Active-Connection
ClassTP(TAppCSActConn, PAppCSActConn)//{
private:
  PSoapRq SoapRq;
  TStr HostNm;
  int PortN;
  int RetryCnt;
  UndefDefaultCopyAssign(TAppCSActConn);
public:
  TAppCSActConn(
   const PSoapRq& _SoapRq, const TStr& _HostNm, const int& _PortN, const int& _RetryCnt):
    SoapRq(_SoapRq), HostNm(_HostNm), PortN(_PortN), RetryCnt(_RetryCnt) {}
  static PAppCSActConn New(
   const PSoapRq& SoapRq, const TStr& HostNm, const int& PortN, const int& RetryCnt){
    return new TAppCSActConn(SoapRq, HostNm, PortN, RetryCnt);}

  PSoapRq GetSoapRq() const {return SoapRq;}
  TStr GetHostNm() const {return HostNm;}
  int GetPortN() const {return PortN;}
  int GetRetryCnt() const {return RetryCnt;}
};

/////////////////////////////////////////////////
// Application-Client-Server-Active-Connection-Base
ClassTP(TAppCSActConnBs, PAppCSActConnBs)//{
private:
  THash<TInt, PAppCSActConn> FetchIdToActConnBsH;
  UndefCopyAssign(TAppCSActConnBs);
public:
  TAppCSActConnBs():
    FetchIdToActConnBsH(){}
  static PAppCSActConnBs New(){
    return new TAppCSActConnBs();}

  int GetActConns() const {
    return FetchIdToActConnBsH.Len();}
  void AddActConn(const int& FetchId, const PAppCSActConn& ActConn){
    IAssert(!FetchIdToActConnBsH.IsKey(FetchId));
    FetchIdToActConnBsH.AddDat(FetchId, ActConn);}
  PAppCSActConn GetActConn(const int& FetchId) const {
    IAssert(FetchIdToActConnBsH.IsKey(FetchId));
    return FetchIdToActConnBsH.GetDat(FetchId);}
  void DelActConn(const int& FetchId){
    IAssert(FetchIdToActConnBsH.IsKey(FetchId));
    FetchIdToActConnBsH.DelKey(FetchId);}
};

/////////////////////////////////////////////////
// Application-Client-Descriptor
ClassTPV(TAppCltDesc, PAppCltDesc, TAppCltDescV)//{
public:
  TStr SesId;
  TStr StateId;
  TStr HostNm;
  int PortN;
  int SockId;
  TStr CltNm;
  TStr CltType;
  TTm LastHBeatTm;
  bool ActiveHBeatRqP;
  UndefDefaultCopyAssign(TAppCltDesc);
public:
  TAppCltDesc(const TStr& _SesId, const TStr& _HostNm, const int& _PortN):
    SesId(_SesId), StateId(), HostNm(_HostNm), PortN(_PortN), SockId(-1),
    LastHBeatTm(TTm::GetCurUniTm()), ActiveHBeatRqP(false){}
  static PAppCltDesc New(const TStr& SesId, const TStr& HostNm, const int& PortN){
    return new TAppCltDesc(SesId, HostNm, PortN);}
  ~TAppCltDesc(){}
};

/////////////////////////////////////////////////
// Application-Server
ClassTPEE(TAppSrv, PAppSrv, TWebNetSrv, TWebNetCltV)//{
private:
  PAppCSLog Log;
  THash<TStr, PAppCltDesc> SesIdToCltDescH;
  TStrStrH CltNmToSesIdH;
  PTimer HBeatTimer;
  PAppCSActConnBs ActConnBs;
  THash<TStr, PAppCSLog> NmToLogH;
  PWebNetProxy Proxy;
public:
  TAppSrv(
   const int& PortN, const TStr& LogFNm,
   const int& HBeatTimerTout, const PNotify& Notify);
  static PAppSrv New(
   const int& PortN=DfPortN,
   const TStr& LogFNm="AppSrv.Log",
   const int& HBeatTimerTout=10*1000,
   const PNotify& Notify=TNotify::StdNotify){
    return new TAppSrv(PortN, LogFNm, HBeatTimerTout, Notify);}
  ~TAppSrv();

  // heart-beat
  PTimer GetHBeatTimer() const {return HBeatTimer;}

  // logging
  void PutLog(const PAppCSLog& _Log){Log=_Log;}
  PAppCSLog GetLog() const {return Log;}
  static bool IsLogSoapFuncNm(const TStr& SoapFuncNm);

  // clients
  int GetClts() const {return SesIdToCltDescH.Len();}
  void GetCltDescV(TAppCltDescV& CltDescV){
    SesIdToCltDescH.GetDatV(CltDescV);}
  TStr GetNewSesId();
  bool IsCltByCltNm(const TStr& CltNm) const {
    return CltNmToSesIdH.IsKey(CltNm);}
  bool IsGetCltDescBySesId(const TStr& SesId, PAppCltDesc& CltDesc) const;
  bool IsGetCltDescBySockId(const int& SockId, PAppCltDesc& CltDesc) const;
  bool IsGetCltDescByCltNm(const TStr& CltNm, PAppCltDesc& CltDesc) const;
  PAppCltDesc AddNewCltDesc(
   const TStr& HostNm, const int& PortN, const int& SockId,
   const TStr& CltNm, const TStr& CltType);
  void DelIfCltDesc(const TStr& SesId);
  void ResolveCltIP(const PAppCltDesc CltDesc, int SockId, TStr &RedirHostNm, TStr &ProxyIP=TStr());

  // http request
  void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
  void OnHttpRqError(const int& SockId, const TStr& MsgStr);
  TStr GetPeerHostNm(const int& SockId) const;
  bool IsCltOk(const PSoapRq& SoapRq, const TStr& PeerHostNm) const;
  PHttpResp OnHttpRq_Info() const;
  PHttpResp OnHttpRq_GetCltList(const PSoapRq& SoapRq);
  PHttpResp OnHttpRq_GetCltInfo(const PSoapRq& SoapRq);
  PHttpResp OnHttpRq_Connect(const PSoapRq& SoapRq, const TStr& PeerHostNm, const int& SockId);
  PHttpResp OnHttpRq_Disconnect(const PSoapRq& SoapRq, const TStr& PeerHostNm);
  PHttpResp OnHttpRq_OnLog(const PSoapRq& SoapRq);
  void OnHttpRq_OnHBeatTimer();
  PHttpResp OnHttpRq_Broadcast(const PSoapRq& SoapRq);
  PHttpResp OnFuncRq(const int &SockId, const PSoapRq& SoapRq, const PUrl& Url);

  // http response
  int FetchSoapRq(const PAppCltDesc& CltDesc, const PSoapRq& SoapRq);
  void OnHttpResp(const int& FId, const PHttpResp& HttpResp);
  void OnHttpRespError(const int& FId, const TStr& _MsgStr);

  // default port-number setting
  static const int DfPortN;
};

/////////////////////////////////////////////////
// Application-Client
ClassTPEE(TAppClt, PAppClt, TWebNetSrv, TWebNetCltV)//{
private:
  PAppCSLog Log; // logging
  TStr CltNm; // unique client name
  TStr CltType; // client type
  TStr SrvHostNm; // server host
  int SrvPortN; // server port
  bool AgentP; // is this client an agent
  TStrIntPrV AgentHostNmPortNPrV; // list of agents connected to this client
  PTimer HBeatTimer; // hbeat timer
  TTm LastHBeatTm; // time of last hbeat
  int ActiveIsAliveFId; // fetch-id of last is-alive fetch
  PAppCSActConnBs ActConnBs; // active-connection-base
  TStr SesId; // client session-id
  PAppCSState State; // client state
  PTimer DelayTimer; // delay timer
  TStrStrH FuncFetchIdH; // fetch-id
public:
  TAppClt(
   const TStr& LogFNm="AppClt.Log",
   const TStr& _CltNm="",
   const TStr& _CltType="",
   const int& HBeatTimerTout=10*1000,
   const bool& _AgentP=false,
   const TStr& _SrvHostNm="127.0.0.1",
   const int& _SrvPortN=TAppSrv::DfPortN,
   const int& CltPortN=DfPortN,
   const PNotify& Notify=TNotify::StdNotify);
  virtual ~TAppClt();

  // default port-number setting
  static const int DfPortN;

  // component retrieval
  void PutLog(const PAppCSLog& _Log){Log=_Log; State->PutLog(_Log);}
  PAppCSLog GetLog() const {return Log;}
  TStr GetCltNm() const {return CltNm;}
  TStr GetSrvHostNm() const {return SrvHostNm;}
  int GetSrvPortN() const {return SrvPortN;}
  bool IsAgent() const {return AgentP;}
  int GetAgents() const {return AgentHostNmPortNPrV.Len();}
  void GetAgentHostNmPortN(const int& AgentN, TStr& HostNm, int& PortN) const {
    HostNm=AgentHostNmPortNPrV[AgentN].Val1;
    PortN=AgentHostNmPortNPrV[AgentN].Val2;}
  PTimer GetHBeatTimer() const {return HBeatTimer;}
  TStr GetSesId() const {return SesId;}
  bool IsSesId() const {return !SesId.Empty();}
  PAppCSState GetState() const {return State;}
  PTimer GetDelayTimer() const {return DelayTimer;}

  // http request
  void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
  void OnHttpRqError(const int& SockId, const TStr& MsgStr);
  TStr GetPeerHostNm(const int& SockId) const;
  PHttpResp OnHttpRq_Info() const;
  PHttpResp OnHttpRq_GetStateInfo(const PSoapRq& SoapRq) const;
  void OnHttpRq_OnHBeatTimer();
  PHttpResp OnHttpRq_RegAgent(const PSoapRq& SoapRq, const TStr& PeerHostNm);
  virtual PHttpResp OnFuncRq(const PSoapRq& SoapRq)=0;

  // http response
  void OnHttpResp(const int& FId, const PHttpResp& HttpResp);
  void OnHttpRespError(const int& FId, const TStr& _MsgStr);
  int FetchSoapRq(
   const PSoapRq& SoapRq, const TStr& HostNm="", const int& PortN=-1, const int& RetryCnt=3, const int& FetchId=-1);
  void OnFetchSoapResp(const PSoapRq& SoapRq, const PSoapResp& SoapResp);
  virtual void OnFuncResp(const PSoapRq& SoapRq, const PSoapResp& SoapResp)=0;

  // logging
  void PutLogMsg(
   const TStr& MsgType, const TStr& MsgStr, const bool& SendToSrvP);

  // state-machine
  virtual PHttpResp OnStateExe(const TAppCsSET& ExeType,
   const PSoapRq& SoapRq, const PSoapResp& SoapResp)=0;
  PHttpResp OnConnectRqAndResp(
   const TStr& NextSId,
   const TAppCsSET& ExeType, const PSoapRq& SoapRq, const PSoapResp& SoapResp,
   bool& BreakLoopP);
  void StartDelayTimer(const int& DelayTout=-1);
  void StopDelayTimer();
  int FetchSoapRqSaveFId(
   const PSoapRq& SoapRq, const TStr& HostNm="", const int& PortN=-1);
  void PutFId(const TStr& FuncNm, const TStr& FetchIdStr){
    FuncFetchIdH.AddDat(FuncNm, FetchIdStr);}
  int GetFIds() const {return FuncFetchIdH.Len();}
  TStr GetFId(const TStr& FuncNm){
    return FuncFetchIdH.IsKey(FuncNm)?FuncFetchIdH.GetDat(FuncNm):TStr();}
  void GetFuncFId(const int& FuncN, TStr& FuncNm, TStr& FetchIdStr) const { 
    FuncFetchIdH.GetKeyDat(FuncN, FuncNm, FetchIdStr);}
  void GetStateExeShortcuts(
   PAppCSLog& Log,
   PAppCSState& State, TStr& StateId,
   const PSoapRq& SoapRq, TStr& SoapRqFuncNm,
   const PSoapResp& SoapResp, TStr& SoapRespFuncNm, TStr& SoapRespFetchId,
   bool& SoapRespResultP);
  void BroadcastToAgents(const PSoapRq& SoapRq);
  void SetState(const TStr& StateId, const TStr& MsgStr="",
   const TStr& TbNm1="", const PSoapTb& Tb1=NULL,
   const TStr& TbNm2="", const PSoapTb& Tb2=NULL);
};

