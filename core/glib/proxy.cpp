/////////////////////////////////////////////////
// Includes
#include "proxy.h"
#include "http.h"

/////////////////////////////////////////////////
// Proxy-Socket-Event
void TProxySockEvent::OnRead(const int& SockId, const PSIn& SIn){
  TMOut SockMOut;
  SockMOut.Save(*SIn);
  Proxy->OnRead(SockId, SIn);
}

void TProxySockEvent::OnWrite(const int& SockId){
  Proxy->OnWrite(SockId);
}

void TProxySockEvent::OnOob(const int& SockId){
  Proxy->OnOob(SockId);
}

void TProxySockEvent::OnAccept(const int& SockId, const PSock& Sock){
  Proxy->SockIdToSockH.AddDat(SockId, Sock);
  Proxy->OnAccept(SockId, Sock);
}

void TProxySockEvent::OnConnect(const int& SockId){
  Proxy->OnConnect(SockId);
}

void TProxySockEvent::OnClose(const int& SockId){
  Proxy->OnClose(SockId);
}

void TProxySockEvent::OnTimeOut(const int& SockId){
  Proxy->OnTimeOut(SockId);
}

void TProxySockEvent::OnError(const int& SockId, const TStr& MsgStr){
  Proxy->OnError(SockId, MsgStr);
}

void TProxySockEvent::OnGetHost(const PSockHost& SockHost){
  Proxy->OnGetHost(SockHost);
}

/////////////////////////////////////////////////
// Proxy
TProxy::TProxy(const int& _PortN):
  PortN(_PortN),
  SockEvent(), Sock(), SockMOut(),
  SockIdToSockH(100){
  SockEvent=PSockEvent(new TProxySockEvent(this));
  Sock=PSock(new TSock(SockEvent));
  Sock->Listen(PortN);
}

/////////////////////////////////////////////////
// Client-Socket-Event
const int TCltSockSockEvent::MxGetHostTrys=3;

TCltSockSockEvent::TCltSockSockEvent(TCltSock* _CltSock):
  Mode(csmGetingHost), GetHostTrys(0),
  CltSock(_CltSock){}

void TCltSockSockEvent::OnRead(const int& SockId, const PSIn& SIn){
  IAssert(Mode==csmConnected);
  CltSock->SockMOut.Save(*SIn);
}

void TCltSockSockEvent::OnConnect(const int& SockId){
  IAssert(Mode==csmConnecting);
  Mode=csmConnected;
}

void TCltSockSockEvent::OnClose(const int&){
  IAssert(Mode==csmConnected);
  Mode=csmAfterClose;
  CltSock->Sock->PutTimeOut(1);
}

void TCltSockSockEvent::OnTimeOut(const int& SockId){
  switch (Mode){
    case csmConnecting: // timeout waiting for connect
      OnError(SockId, "Connecting timeout"); break;
    case csmConnected: // timeout after successful OnConnect
      break;
    case csmAfterClose: // safety timeout after OnClose
      {CltSock->OnBf(CltSock->SockMOut.GetSIn());
      break;}
    default: Fail;                    
  }
}

void TCltSockSockEvent::OnError(const int& SockId, const TStr& MsgStr){
  CltSock->OnErr(MsgStr);
}

void TCltSockSockEvent::OnGetHost(const PSockHost& SockHost){
  IAssert(Mode==csmGetingHost);
  if (SockHost->IsOk()){
    Mode=csmConnecting;
    CltSock->Sock=PSock(new TSock(this));
    CltSock->Sock->Connect(SockHost, CltSock->GetPortN());
    CltSock->Sock->PutTimeOut(1000);
  } else
  if (SockHost->GetStatus()==shsTryAgain){
    GetHostTrys++;
    if (GetHostTrys<MxGetHostTrys){
      TSockHost::GetAsyncSockHost(CltSock->GetHostNm(), this);
    } else {
      OnError(-1, "Can't get host info");
    }
  } else {
    OnError(-1, "Invalid host");
  }
}

/////////////////////////////////////////////////
// Client-Socket
TCltSock::TCltSock(
 const TStr& _HostNm, const int& _PortN,
 const TStr& _UsrNm, const TStr& _PwdStr):
  HostNm(_HostNm), PortN(_PortN),
  SockEvent(), Sock(), SockMOut(),
  UsrNm(_UsrNm), PwdStr(_PwdStr){
  SockEvent=PSockEvent(new TCltSockSockEvent(this));
  TSockHost::GetAsyncSockHost(HostNm, SockEvent);
}

void TCltSock::OnBf(const PSIn& SIn){
  PHttpResp HttpResp=PHttpResp(new THttpResp(SIn));
  int L=HttpResp->GetBodyStr().Len();
  printf("%d", L);
}

