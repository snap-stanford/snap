#ifndef term_h
#define term_h

/////////////////////////////////////////////////
// Forward
class TTrmSrv;

/////////////////////////////////////////////////
// Terminal-Server-Socket-Event
class TTrmSrvSockEvent: public TSockEvent{
private:
  TTrmSrv* TrmSrv;
public:
  TTrmSrvSockEvent(TTrmSrv* _TrmSrv): TrmSrv(_TrmSrv){}
  ~TTrmSrvSockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int& SockId, const PSock& Sock);
  void OnConnect(const int&){Fail;}
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const int& ErrCd, const TStr& MsgStr);
  void OnGetHost(const PSockHost&){Fail;}
};

/////////////////////////////////////////////////
// Terminal-Server
ClassTP(TTrmSrv, PTrmSrv)//{
private:
  int PortN;
  PSockEvent SockEvent;
  PSock Sock;
  TMOut SockMOut;
  PNotify BcstNotify;
  THash<TInt, PSock> SockIdToSockH;
  THash<TInt, PNotify> SockIdToNotifyH;
  UndefDefaultCopyAssign(TTrmSrv);
public:
  TTrmSrv(const int& _PortN);
  virtual ~TTrmSrv();
  TTrmSrv(TSIn&){Fail;}
  static PTrmSrv Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  int GetPortN(){return PortN;}
  PNotify GetBcstNotify(){return BcstNotify;}
  int GetClts(){return SockIdToSockH.Len();}
  PSock GetCltSock(const int& SockId){return SockIdToSockH.GetDat(SockId);}
  PNotify GetCltNotify(const int& SockId){return SockIdToNotifyH.GetDat(SockId);}
  TStr GetCltPeerIpNum(const int& SockId){
    return GetCltSock(SockId)->GetPeerIpNum();}
  void CloseClt(const int& SockId){
    SockIdToSockH.DelKey(SockId); SockIdToNotifyH.DelKey(SockId);}

  void SendTxtToClt(const int& CltSockId, const TStr& Txt);
  void SendLnToClt(const int& CltSockId, const TStr& Ln);
  void SendTxtToAll(const TStr& Txt);
  void SendLnToAll(const TStr& Ln);
  virtual void OnLn(const int& CltSockId, const TStr& Ln)=0;
  virtual void OnTxt(const int& CltSockId, const TStr& Txt)=0;

  friend class TTrmSrvSockEvent;
};
typedef TPt<TTrmSrv> PTrmSrv;

/////////////////////////////////////////////////
// Terminal-Client-Socket-Event
class TTrmClt;
typedef enum {tcsmGetingHost, tcsmConnecting, tcsmConnected,
 tcsmAfterClose} TTrmCltSockEventMode;

class TTrmCltSockEvent: public TSockEvent{
private:
  static const int MxGetHostTrys;
  TTrmCltSockEventMode Mode;
  int GetHostTrys;
  TTrmClt* TrmClt;
public:
  TTrmCltSockEvent(TTrmClt* _TrmClt);
  ~TTrmCltSockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int& SockId);
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const int& ErrCd, const TStr& MsgStr);
  void OnGetHost(const PSockHost& SockHost);
};

/////////////////////////////////////////////////
// Terminal-Client
class TTrmClt{
private:
  TCRef CRef;
private:
  TStr HostNm;
  int PortN;
  PSockEvent SockEvent;
  PSock Sock;
  TMOut SockMOut;
  PNotify Notify;
  TStr UsrNm;
  TStr PwdStr;
public:
  TTrmClt(
   const TStr& _HostNm, const int& _PortN,
   const TStr& _UsrNm=TStr(), const TStr& _PwdStr=TStr());
  virtual ~TTrmClt();
  TTrmClt(TSIn&){Fail;}
  static TPt<TTrmClt> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTrmClt& operator=(const TTrmClt&){Fail; return *this;}

  TStr GetHostNm(){return HostNm;}
  int GetPortN(){return PortN;}

  void SendTxt(const TStr& Txt);
  void SendLn(const TStr& Ln);
  virtual void OnLn(const TStr& Ln)=0;
  virtual void OnTxt(const TStr& Txt)=0;

  bool IsConn(){return !Sock.Empty();}
  void Disconn(){Sock=NULL;}

  TStr GetUsrNm(){return UsrNm;}
  TStr GetPwdStr(){return PwdStr;}

  friend class TTrmCltSockEvent;
  friend class TPt<TTrmClt>;
};
typedef TPt<TTrmClt> PTrmClt;

#endif
