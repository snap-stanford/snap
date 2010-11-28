#ifndef proxy_h
#define proxy_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "sock.h"

/////////////////////////////////////////////////
// Proxy-Socket-Event
class TProxy;

class TProxySockEvent: public TSockEvent{
private:
  TProxy* Proxy;
public:
  TProxySockEvent(TProxy* _Proxy): Proxy(_Proxy){}
  ~TProxySockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&);
  void OnOob(const int&);
  void OnAccept(const int& SockId, const PSock& Sock);
  void OnConnect(const int&);
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const TStr& ErrStr);
  void OnGetHost(const PSockHost&);
};

/////////////////////////////////////////////////
// Proxy
class TProxy: public TSockEvent{
private:
  TCRef CRef;
private:
  int PortN;
  PSockEvent SockEvent;
  PSock Sock;
  TMOut SockMOut;
  THash<TInt, PSock> SockIdToSockH;
public:
  TProxy(const int& _PortN);
  virtual ~TProxy(){}
  TProxy(TSIn&){Fail;}
  static TPt<TProxy> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TProxy& operator=(const TProxy&){Fail; return *this;}

  int GetPortN(){return PortN;}
  int GetClts(){return SockIdToSockH.Len();}

  virtual void OnRead(const int& SockId, const PSIn& SIn){}
  virtual void OnWrite(const int&){}
  virtual void OnOob(const int&){}
  virtual void OnAccept(const int& SockId, const PSock& Sock){}
  virtual void OnConnect(const int&){}
  virtual void OnClose(const int& SockId){}
  virtual void OnTimeOut(const int& SockId){}
  virtual void OnError(const int& SockId, const TStr& ErrStr){}
  virtual void OnGetHost(const PSockHost&){}

  PSock GetSock(const int& SockId){return SockIdToSockH.GetDat(SockId);}
  void CloseSock(const int& SockId){SockIdToSockH.DelKey(SockId);}

  friend TProxySockEvent;
  friend TPt<TProxy>;
};
typedef TPt<TProxy> PProxy;

/////////////////////////////////////////////////
// Client-Socket-Event
class TCltSock;
typedef enum {csmGetingHost, csmConnecting, csmConnected,
 csmAfterClose} TCltSockSockEventMode;

class TCltSockSockEvent: public TSockEvent{
private:
  static const int MxGetHostTrys;
  TCltSockSockEventMode Mode;
  int GetHostTrys;
  TCltSock* CltSock;
public:
  TCltSockSockEvent(TCltSock* _CltSock);
  ~TCltSockSockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int& SockId);
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const TStr& ErrStr);
  void OnGetHost(const PSockHost& SockHost);
};

/////////////////////////////////////////////////
// Client-Socket
class TCltSock{
private:
  TCRef CRef;
private:
  TStr HostNm;
  int PortN;
  PSockEvent SockEvent;
  PSock Sock;
  TMOut SockMOut;
  TStr UsrNm;
  TStr PwdStr;
public:
  TCltSock(
   const TStr& _HostNm, const int& _PortN,
   const TStr& _UsrNm=TStr(), const TStr& _PwdStr=TStr());
  virtual ~TCltSock(){}
  TCltSock(TSIn&){Fail;}
  static TPt<TCltSock> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TCltSock& operator=(const TCltSock&){Fail; return *this;}

  TStr GetHostNm(){return HostNm;}
  int GetPortN(){return PortN;}

  void PutBf(const PSIn& SIn){Sock->Send(SIn);}
  virtual void OnBf(const PSIn& SIn);
  virtual void OnErr(const TStr& MsgStr){Fail;}

  bool IsConn(){return !Sock.Empty();}
  void Disconn(){Sock=NULL;}

  TStr GetUsrNm(){return UsrNm;}
  TStr GetPwdStr(){return PwdStr;}

  friend TCltSockSockEvent;
  friend TPt<TCltSock>;
};
typedef TPt<TCltSock> PCltSock;

#endif

