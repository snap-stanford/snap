/////////////////////////////////////////////////
// Forward
ClassHdTP(TSockEvent, PSockEvent);
typedef TVec<PSockEvent> TSockEventV;
typedef THash<TInt, PSockEvent> TIdToSockEventH;

ClassHdTP(TSock, PSock);
ClassHdTP(TSockHost, PSockHost);

/////////////////////////////////////////////////
// Socket-Event
ClassTP(TSockEvent, PSockEvent)//{
private:
  static int LastSockEventId;
  int SockEventId;
  UndefCopyAssign(TSockEvent);
public:
  TSockEvent():
    SockEventId(++LastSockEventId){}
  virtual ~TSockEvent();
  TSockEvent(TSIn&){Fail;}
  static PSockEvent Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  int GetSockEventId() const {return SockEventId;}

  virtual void CloseConn(){}
  virtual int GetBytesRead() const {return -1;}

  virtual void OnRead(const int& /*SockId*/, const PSIn& /*SIn*/){}
  virtual void OnWrite(const int& /*SockId*/){}
  virtual void OnOob(const int& /*SockId*/){}
  virtual void OnAccept(const int& /*SockId*/, const PSock& /*Sock*/){}
  virtual void OnConnect(const int& /*SockId*/){}
  virtual void OnClose(const int& /*SockId*/){}
  virtual void OnTimeOut(const int& /*SockId*/){}
  virtual void OnError(
   const int& /*SockId*/, const int& /*ErrCd*/, const TStr& /*ErrStr*/){}
  virtual void OnGetHost(const PSockHost& /*SockHost*/){}

  static bool IsReg(const PSockEvent& SockEvent);
  static void Reg(const PSockEvent& SockEvent);
  static void UnReg(const PSockEvent& SockEvent);
};

/////////////////////////////////////////////////
// Socket-Host
typedef enum {
  shsUndef, shsOk, shsHostNotFound, shsTryAgain, shsError} TSockHostStatus;

ClassTP(TSockHost, PSockHost)//{
private:
  TSockHostStatus Status;
  TStrV HostNmV;
  TStrV IpNumV;
  char HostEntBf[MAXGETHOSTSTRUCT]; // needs defined NO_WIN32_LEAN_AND_MEAN;
  int SockEventId;
  UndefCopyAssign(TSockHost);
private:
  void GetFromHostEnt(const TSockHostStatus& Status, const hostent* HostEnt);
public:
  TSockHost():
    Status(shsUndef), HostNmV(), IpNumV(), SockEventId(-1){}
  TSockHost(const PSockEvent& SockEvent):
    Status(shsUndef), HostNmV(), IpNumV(),
    SockEventId(SockEvent->GetSockEventId()){}
  TSockHost(TSIn&){Fail;}
  static PSockHost Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  bool IsOk() const {return Status==shsOk;}
  int GetStatus() const {return Status;}
  int GetHostNms() const {return HostNmV.Len();}
  TStr GetHostNm() const {
    if (GetHostNms()==0){return GetIpNum();} else {return GetHostNm(0);}}
  TStr GetHostNm(const int& HostNmN) const {return HostNmV[HostNmN];}
  int GetIpNums() const {return IpNumV.Len();}
  TStr GetIpNum(const int& IpNumN=0) const {return IpNumV[IpNumN];}
  bool HasIpNum(const TStr &IpNum) const {return IpNumV.IsIn(IpNum);}

  int GetSockEventId() const {return SockEventId;}
  PSockEvent GetSockEvent() const;

  static bool IsIpNum(const TStr& HostNm);
  static TStr GetIpNum(const uint& IpNumN);

  static PSockHost GetSyncSockHost(const TStr& HostNm);
  static void GetAsyncSockHost(const TStr& HostNm, const PSockEvent& SockEvent);

  static TSockHostStatus GetStatus(const int& ErrCd);
  static PSockHost GetLocalSockHost();

  static const TStr LocalHostNm;

  friend class TSockSys;
};

/////////////////////////////////////////////////
// Socket
typedef SOCKET TSockHnd;

ClassTP(TSock, PSock)//{
private:
  static int LastSockId;
  int SockId;
  TSockHnd SockHnd;
  int SockEventId;
  TMem UnsentBf;
  UndefDefaultCopyAssign(TSock);
public:
  TSock(const PSockEvent& SockEvent);
  static PSock New(const PSockEvent& SockEvent){
    return PSock(new TSock(SockEvent));}
  TSock(const TSockHnd& _SockHnd, const PSockEvent& SockEvent);
  ~TSock();
  TSock(TSIn&){Fail;}
  static PSock Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  int GetSockId() const {return SockId;}
  TSockHnd GetSockHnd() const {return SockHnd;}
  int GetSockEventId() const {return SockEventId;}
  PSockEvent GetSockEvent() const;
  bool IsUnsent() const {return UnsentBf.Len()>0;}
  int GetUnsentLen() const {return UnsentBf.Len();}

  void Listen(const int& PortN);
  int GetPortAndListen(const int& MnPortN);
  void Connect(const PSockHost& SockHost, const int& PortN);
  void Send(const PSIn& SIn, bool& Ok, int& ErrCd);
  void Send(const PSIn& SIn);
  void SendSafe(const PSIn& SIn);
  TStr GetPeerIpNum() const;
  TStr GetLocalIpNum() const;
  void PutTimeOut(const int& MSecs);
  void DelTimeOut();

  static PSock Accept(const TSockHnd& SockHnd, const PSockEvent& SockEvent);

  static TStr GetSockSysStatusStr();
  static uint64 GetSockSysBytesRead();
  static uint64 GetSockSysBytesWritten();
  static bool IsSockId(const int& SockId);
};

/////////////////////////////////////////////////
// Report-Event
ClassTP(TReportEvent, PReportEvent)//{
private:
  static int LastReportEventId;
  int ReportEventId;
  UndefCopyAssign(TReportEvent);
public:
  TReportEvent(): ReportEventId(++LastReportEventId){}
  virtual ~TReportEvent(){}
  TReportEvent(TSIn&){Fail;}
  static PReportEvent Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  int GetReportEventId(){return ReportEventId;}
  void SendReport();

  virtual void OnReport(){}
};

/////////////////////////////////////////////////
// Timer
ClassTP(TTTimer, PTimer)//{
private:
  static int LastTimerId;
  int TimerId;
  uint TimerHnd;
  int TimeOut;
  int Ticks;
  TSecTm StartTm;
public:
  TTTimer(const int& _TimeOut=0);
  virtual ~TTTimer();
  TTTimer(TSIn&){Fail;}
  static PTimer Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTTimer& operator=(const TTTimer&){Fail; return *this;}

  int GetTimerId() const {return TimerId;}
  int GetTimeOut() const {return TimeOut;}

  void StartTimer(const int& _TimeOut=-1);
  void StopTimer();
  void IncTicks(){Ticks++;}
  int GetTicks() const {return Ticks;}
  int GetSecs() const {
    return TSecTm::GetDSecs(StartTm, TSecTm::GetCurTm());}

  virtual void OnTimeOut(){}
};
typedef TAPt<TTTimer> ATimer;

