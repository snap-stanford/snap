/////////////////////////////////////////////////
// Forward
class TSmtpClt;
class TSmtpSrv;

/////////////////////////////////////////////////
// Smtp
class TSmtp{
public:
  static const int DfSmtpPortN;
};

/////////////////////////////////////////////////
// Smtp-Request
ClassTP(TSmtpRq, PSmtpRq)//{
private:
  TSmtpClt* SmtpClt;
  int MailId;
  TStr SrvHostNm;
  int SrvPortN;
  TStrV FromStrV;
  TStrV ToStrV;
  TMem DataMem;
public:
  TSmtpRq(TSmtpClt* _SmtpClt, const int& _MailId,
   const TStr& _SrvHostNm, const int& _SrvPortN,
   const TStr& _FromStr, const TStr& _ToStr, const TMem& _DataMem):
    SmtpClt(_SmtpClt), MailId(_MailId),
    SrvHostNm(_SrvHostNm), SrvPortN(_SrvPortN),
    FromStrV(), ToStrV(), DataMem(_DataMem){
    FromStrV.Add(_FromStr); ToStrV.Add(_ToStr);}
  TSmtpRq(const TStrV& _FromStrV, const TStrV& _ToStrV, const TMem& _DataMem):
    SmtpClt(NULL), MailId(-1), SrvHostNm(), SrvPortN(-1),
    FromStrV(_FromStrV), ToStrV(_ToStrV), DataMem(_DataMem){}
  ~TSmtpRq(){}
  TSmtpRq(TSIn&){Fail;}
  static PSmtpRq Load(TSIn& SIn){return new TSmtpRq(SIn);}
  void Save(TSOut&){Fail;}

  TSmtpRq& operator=(const TSmtpRq&){Fail; return *this;}

  int GetMailId() const {return MailId;}
  TStr GetSrvHostNm() const {return SrvHostNm;}
  int GetSrvPortN() const {return SrvPortN;}
  int GetFromStrs() const {return FromStrV.Len();}
  TStr GetFromStr(const int& FromStrN) const {return FromStrV[FromStrN];}
  int GetToStrs() const {return ToStrV.Len();}
  TStr GetToStr(const int& ToStrN) const {return ToStrV[ToStrN];}
  TMem& GetDataMem(){return DataMem;}
};
typedef TQQueue<PSmtpRq> TSmtpRqQ;

/////////////////////////////////////////////////
// Smtp-Socket-Event
typedef enum {scmUndef,
 scmWaitConnAck, scmWaitHeloAck, scmWaitMailAck, scmWaitRcptAck,
 scmWaitDataAck, scmWaitTxtAck, scmWaitQuitAck, scmEnd} TSmtpConnMode;

class TSmtpSockEvent: public TSockEvent{
private:
  static const int MxGetHostTrys;
  TSmtpClt* SmtpClt;
  PSmtpRq SmtpRq;
  TSmtpConnMode SmtpConnMode;
  int GetHostTrys;
  PSockHost SockHost;
  PSock Sock;
  TMOut SockMOut;
public:
  TSmtpSockEvent(TSmtpClt* _SmtpClt, const PSmtpRq& _SmtpRq):
    TSockEvent(),
    SmtpClt(_SmtpClt), SmtpRq(_SmtpRq),
    SmtpConnMode(scmUndef), GetHostTrys(0),
    SockHost(), Sock(), SockMOut(10000){}
  ~TSmtpSockEvent(){}

  TSockEvent& operator=(const TSockEvent&){Fail; return *this;}

  void CloseConn(){SockHost=NULL; Sock=NULL;}

  int GetRespCd(const TStr& RespCrLfLn);
  void SendSmtpRq(const TStr& RqStr, const TSmtpConnMode& NewSmtpConnMode);
  void OnSmtpError(const TStr& ErrStr);

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int&);
  void OnClose(const int&);
  void OnTimeOut(const int&);
  void OnError(const int&, const int&, const TStr&);
  void OnGetHost(const PSockHost& _SockHost);
};

/////////////////////////////////////////////////
// Smtp-Client
ClassTP(TSmtpClt, PSmtpClt)//{
private:
  PNotify Notify;
  TStr SrvHostNm;
  int SrvPortN;
  int LastMailId;
  TSmtpRqQ SmtpRqQ;
  PSmtpRq CurSmtpRq;
  PSockEvent CurSmtpSockEvent;
  int SendIfAvail();
  void OnClose();
public:
  TSmtpClt(const TStr& _SrvHostNm, const PNotify& _Notify=TNotify::StdNotify,
   const int& _SrvPortN=TSmtp::DfSmtpPortN):
    Notify(_Notify), SrvHostNm(_SrvHostNm), SrvPortN(_SrvPortN),
    LastMailId(0), SmtpRqQ(), CurSmtpRq(), CurSmtpSockEvent(){}
  virtual ~TSmtpClt(){}
  TSmtpClt(TSIn&){Fail;}
  static PSmtpClt Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSmtpClt& operator=(const TSmtpClt&){Fail; return *this;}

  // sending mails
  int Send(const TStr& FromStr, const TStr& ToStr, TMem& DataMem);
  bool IsSend() const {return !SmtpRqQ.Empty();}

  virtual void OnDone(const TStr& MsgStr){
    TNotify::OnNotify(Notify, ntInfo, MsgStr);}
  virtual void OnError(const TStr& MsgStr){
    TNotify::OnNotify(Notify, ntErr, MsgStr);}

  friend class TSmtpSockEvent;
};

/////////////////////////////////////////////////
// Smtp-Server-Socket-Event
class TSmtpSrvSockEvent: public TSockEvent{
private:
  TSmtpSrv* SmtpSrv;
public:
  TSmtpSrvSockEvent(TSmtpSrv* _SmtpSrv): SmtpSrv(_SmtpSrv){}
  ~TSmtpSrvSockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int& SockId);
  void OnOob(const int& SockId);
  void OnAccept(const int& SockId, const PSock& Sock);
  void OnConnect(const int& SockId);
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const int& ErrCd, const TStr& ErrStr);
  void OnGetHost(const PSockHost& SockHost);
};

/////////////////////////////////////////////////
// Smtp-Server-Connection
ClassTP(TSmtpSrvConn, PSmtpSrvConn)//{
private:
  TSmtpSrv* SmtpSrv;
  PSock Sock;
  TMOut SockMOut;
  TStrV FromStrV;
  TStrV ToStrV;
  bool DataModeP;
  TMem DataMem;
public:
  TSmtpSrvConn(const PSock& _Sock, TSmtpSrv* _SmtpSrv):
    SmtpSrv(_SmtpSrv), Sock(_Sock), SockMOut(10000),
    FromStrV(), ToStrV(), DataModeP(false), DataMem(){}
  ~TSmtpSrvConn(){}
  TSmtpSrvConn(TSIn&){Fail;}
  static PSmtpSrvConn Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSmtpSrvConn& operator=(const TSmtpSrvConn&){Fail; return *this;}

  PSock GetSock() const {return Sock;}
  void Send(const TStr& Str) const {
    PSIn SIn=TStrIn::New(Str); Sock->SendSafe(SIn);}
  PSmtpRq GetSmtpRq() const {
    return PSmtpRq(new TSmtpRq(FromStrV, ToStrV, DataMem));}

  friend class TSmtpSrv;
};

/////////////////////////////////////////////////
// Smtp-Server
ClassTP(TSmtpSrv, PSmtpSrv)//{
private:
  PNotify Notify;
  int PortN;
  TStr HomeFPath;
  PSockEvent SockEvent;
  PSock Sock;
  THash<TInt, PSmtpSrvConn> SockIdToConnH;
private:
  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int& SockId);
  void OnOob(const int&){Fail;}
  void OnAccept(const int& SockId, const PSock& Sock);
  void OnConnect(const int&){Fail;}
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const int& ErrCd, const TStr& ErrStr);
  void OnGetHost(const PSockHost&){Fail;}
public:
  TSmtpSrv(const PNotify& _Notify=TNotify::StdNotify,
   const int& _PortN=TSmtp::DfSmtpPortN);
  static PSmtpSrv New(const PNotify& Notify=TNotify::StdNotify,
   const int& PortN=TSmtp::DfSmtpPortN){
    return PSmtpSrv(new TSmtpSrv(Notify, PortN));}
  virtual ~TSmtpSrv();
  TSmtpSrv(TSIn&){Fail;}
  static PSmtpSrv Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSmtpSrv& operator=(const TSmtpSrv&){Fail; return *this;}

  int GetPortN() const {return PortN;}
  int GetConns() const {return SockIdToConnH.Len();}
  PSmtpSrvConn GetConn(const int& SockId) const {
    return SockIdToConnH.GetDat(SockId);}
  bool IsConn(const int& SockId, PSmtpSrvConn& Conn) const {
    if (SockIdToConnH.IsKey(SockId)){Conn=GetConn(SockId); return true;}
    else {return false;}}
  TStr GetPeerIpNum(const int& SockId) const {
    return GetConn(SockId)->Sock->GetPeerIpNum();}

  virtual void OnSmtpRq(const int& SockId, const PSmtpRq& SmtpRq);

  friend class TSmtpSrvSockEvent;
};

