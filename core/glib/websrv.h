/////////////////////////////////////////////////
// Forward
class TWebSrv;

/////////////////////////////////////////////////
// Web-Server-Socket-Event
class TWebSrvSockEvent: public TSockEvent{
private:
  TWebSrv* WebSrv;
public:
  TWebSrvSockEvent(TWebSrv* _WebSrv):
    TSockEvent(), WebSrv(_WebSrv){}
  ~TWebSrvSockEvent(){}

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
// Web-Server-Connection
typedef enum {
  wsctUndef, wsctReceiving, wsctWaitingToRespond, wsctSending} TWebSrvConnType;

ClassTP(TWebSrvConn, PWebSrvConn)//{
private:
  TWebSrv* WebSrv;
  TWebSrvConnType Type;
  PSock Sock;
  TChA HttpRqChA;
  UndefDefaultCopyAssign(TWebSrvConn);
public:
  TWebSrvConn(const PSock& _Sock, TWebSrv* _WebSrv):
    Type(wsctUndef), Sock(_Sock), WebSrv(_WebSrv){}
  static PWebSrvConn New(const PSock& Sock, TWebSrv* WebSrv){
    return PWebSrvConn(new TWebSrvConn(Sock, WebSrv));}
  ~TWebSrvConn(){}
  TWebSrvConn(TSIn&){Fail;}
  static PWebSrvConn Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  void PutType(const TWebSrvConnType& _Type){Type=_Type;}
  TWebSrvConnType GetType() const {return Type;}

  PSock GetSock() const {return Sock;}
  void Send(const PSIn& SIn) const {Sock->SendSafe(SIn);}

  TChA& GetHttpRqChA(){return HttpRqChA;}

  friend class TWebSrv;
};

/////////////////////////////////////////////////
// Web-Server
ClassTPV(TWebSrv, PWebSrv, TWebSrvV)//{
private:
  PNotify Notify;
  int PortN;
  TStr HomeNrFPath;
  PSockEvent SockEvent;
  PSock Sock;
  THash<TInt, PWebSrvConn> SockIdToConnH;
  UndefDefaultCopyAssign(TWebSrv);
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
  TWebSrv(
   const int& _PortN, const bool& FixedPortNP=true, const PNotify& _Notify=NULL);
  static PWebSrv New(
   const int& PortN, const bool& FixedPortNP=true, const PNotify& Notify=NULL){
    return PWebSrv(new TWebSrv(PortN, FixedPortNP, Notify));}
  virtual ~TWebSrv();
  TWebSrv(TSIn&){Fail;}
  static PWebSrv Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  PNotify GetNotify() const {return Notify;}
  int GetPortN() const {return PortN;}
  TStr GetHomeNrFPath() const {return HomeNrFPath;}

  int GetConns() const {return SockIdToConnH.Len();}
  bool IsConn(const int& SockId, PWebSrvConn& Conn) const {
    if (SockIdToConnH.IsKey(SockId)){Conn=GetConn(SockId); return true;}
    else {return false;}}
  void AddConn(const int& SockId, const PWebSrvConn& Conn){
    SockIdToConnH.AddDat(SockId, Conn);}
  void DelConn(const int& SockId){
    SockIdToConnH.DelKey(SockId);}
  PWebSrvConn GetConn(const int& SockId) const {
    return SockIdToConnH.GetDat(SockId);}
  TStr GetPeerIpNum(const int& SockId) const {
    return GetConn(SockId)->Sock->GetPeerIpNum();}

  virtual void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
  void SendHttpResp(const int& SockId, const PHttpResp& HttpResp);

  friend class TWebSrvSockEvent;
};

