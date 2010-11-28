/////////////////////////////////////////////////
// Terminal-Server-Notifications
class TTrmSrvNotify: public TNotify{
private:
  TTrmSrv* TrmSrv;
  int CltSockId;
public:
  TTrmSrvNotify(TTrmSrv* _TrmSrv, const int& _CltSockId):
    TNotify(), TrmSrv(_TrmSrv), CltSockId(_CltSockId){}

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr){
    OnLn(TNotify::GetTypeStr(Type)+": "+MsgStr);}
  void OnStatus(const TStr& MsgStr){
    OnLn(TStr("Status: ")+MsgStr);}
  void OnLn(const TStr& MsgStr){
    if (CltSockId==-1){TrmSrv->SendLnToAll(MsgStr);}
    else {TrmSrv->SendLnToClt(CltSockId, MsgStr);}}
  void OnTxt(const TStr& MsgStr){
    if (CltSockId==-1){TrmSrv->SendTxtToAll(MsgStr);}
    else {TrmSrv->SendTxtToClt(CltSockId, MsgStr);}}
};

/////////////////////////////////////////////////
// Terminal-Server-Socket-Event
void TTrmSrvSockEvent::OnAccept(const int& SockId, const PSock& Sock){
  TrmSrv->SockIdToSockH.AddDat(SockId, Sock);
  PNotify Notify=PNotify(new TTrmSrvNotify(TrmSrv, SockId));
  TrmSrv->SockIdToNotifyH.AddDat(SockId, Notify);
  TrmSrv->OnLn(SockId, "SysEcho Accept");
}

void TTrmSrvSockEvent::OnRead(const int& SockId, const PSIn& SIn){
  TChA TxtChA;
  while (!SIn->Eof()){
    char Ch=SIn->GetCh();
    TxtChA+=Ch;
    TrmSrv->SockMOut.PutCh(Ch);
  }
  TrmSrv->OnTxt(SockId, TxtChA);
  while (TrmSrv->SockMOut.IsEolnLn()){
    TStr Ln=TrmSrv->SockMOut.GetEolnLn(false, true);
    TrmSrv->OnLn(SockId, Ln);
  }
}

void TTrmSrvSockEvent::OnClose(const int& SockId){
  PSock CltSock=TrmSrv->SockIdToSockH.GetDat(SockId);
  CltSock->PutTimeOut(1);
}

void TTrmSrvSockEvent::OnTimeOut(const int& SockId){
  TrmSrv->OnLn(SockId, "SysEcho Close");
  TrmSrv->CloseClt(SockId);
}

void TTrmSrvSockEvent::OnError(
 const int& SockId, const int& /*ErrCd*/, const TStr& MsgStr){
  TrmSrv->OnLn(SockId, TStr("Echo ")+MsgStr);
}

/////////////////////////////////////////////////
// Terminal-Server
TTrmSrv::TTrmSrv(const int& _PortN):
  PortN(_PortN),
  SockEvent(), Sock(), SockMOut(),
  BcstNotify(),
  SockIdToSockH(100), SockIdToNotifyH(100){
  SockEvent=PSockEvent(new TTrmSrvSockEvent(this));
  TSockEvent::Reg(SockEvent);
  Sock=PSock(new TSock(SockEvent));
  Sock->Listen(PortN);
  BcstNotify=PNotify(new TTrmSrvNotify(this, -1));
}

TTrmSrv::~TTrmSrv(){
  TSockEvent::UnReg(SockEvent);
}

void TTrmSrv::SendTxtToClt(const int& CltSockId, const TStr& Txt){
  TMOut MOut;
  MOut.PutBf(Txt.CStr(), Txt.Len());
  PSIn SIn=MOut.GetSIn();
  PSock CltSock=SockIdToSockH.GetDat(CltSockId);
  CltSock->SendSafe(SIn);
}

void TTrmSrv::SendLnToClt(const int& CltSockId, const TStr& Ln){
  TMOut MOut;
  MOut.PutBf(Ln.CStr(), Ln.Len());
  MOut.MkEolnLn();
  PSIn SIn=MOut.GetSIn();
  PSock CltSock=SockIdToSockH.GetDat(CltSockId);
  CltSock->SendSafe(SIn);
}

void TTrmSrv::SendTxtToAll(const TStr& Txt){
  int SockIdToSockP=SockIdToSockH.FFirstKeyId();
  while (SockIdToSockH.FNextKeyId(SockIdToSockP)){
    int CltSockId=SockIdToSockH.GetKey(SockIdToSockP);
    SendTxtToClt(CltSockId, Txt);
  }
}

void TTrmSrv::SendLnToAll(const TStr& Ln){
  int SockIdToSockP=SockIdToSockH.FFirstKeyId();
  while (SockIdToSockH.FNextKeyId(SockIdToSockP)){
    int CltSockId=SockIdToSockH.GetKey(SockIdToSockP);
    SendLnToClt(CltSockId, Ln);
  }
}

/////////////////////////////////////////////////
// Terminal-Client-Notifications
class TTrmCltNotify: public TNotify{
private:
  TTrmClt* TrmClt;
public:
  TTrmCltNotify(TTrmClt* _TrmClt):
    TNotify(), TrmClt(_TrmClt){}

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr){
    OnLn(TNotify::GetTypeStr(Type)+": "+MsgStr);}
  void OnStatus(const TStr& MsgStr){
    OnLn(TStr("Status: ")+MsgStr);}
  void OnLn(const TStr& MsgStr){
    TrmClt->SendLn(MsgStr);}
  void OnTxt(const TStr& MsgStr){
    TrmClt->SendTxt(MsgStr);}
};

/////////////////////////////////////////////////
// Terminal-Client-Socket-Event
const int TTrmCltSockEvent::MxGetHostTrys=3;

TTrmCltSockEvent::TTrmCltSockEvent(TTrmClt* _TrmClt):
  Mode(tcsmGetingHost), GetHostTrys(0),
  TrmClt(_TrmClt){}

void TTrmCltSockEvent::OnRead(const int& /*SockId*/, const PSIn& SIn){
  IAssert(Mode==tcsmConnected);
  TChA TxtChA;
  while (!SIn->Eof()){
    char Ch=SIn->GetCh();
    TxtChA+=Ch;
    TrmClt->SockMOut.PutCh(Ch);
  }
  TrmClt->OnTxt(TxtChA);
  while (TrmClt->SockMOut.IsEolnLn()){
    TStr Ln=TrmClt->SockMOut.GetEolnLn(false, true);
    TrmClt->OnLn(Ln);
  }
}

void TTrmCltSockEvent::OnConnect(const int& /*SockId*/){
  IAssert(Mode==tcsmConnecting);
  Mode=tcsmConnected;
  TrmClt->OnLn("Connected");
}

void TTrmCltSockEvent::OnClose(const int&){
  IAssert(Mode==tcsmConnected);
  Mode=tcsmAfterClose;
  TrmClt->Sock->PutTimeOut(1);
}

void TTrmCltSockEvent::OnTimeOut(const int& SockId){
  switch (Mode){
    case tcsmConnecting: // timeout waiting for connect
      OnError(SockId, -1, "Connecting timeout"); break;
    case tcsmConnected: // timeout after successful OnConnect
      break;
    case tcsmAfterClose: // safety timeout after OnClose
      TrmClt->OnLn("Closed"); break;
    default: Fail;
  }
}

void TTrmCltSockEvent::OnError(
 const int& /*SockId*/, const int& /*ErrCd*/, const TStr& MsgStr){
  TrmClt->OnLn(MsgStr);
}

void TTrmCltSockEvent::OnGetHost(const PSockHost& SockHost){
  IAssert(Mode==tcsmGetingHost);
  if (SockHost->IsOk()){
    Mode=tcsmConnecting;
    TrmClt->Sock=PSock(new TSock(this));
    TrmClt->Sock->Connect(SockHost, TrmClt->GetPortN());
    TrmClt->Sock->PutTimeOut(1000);
  } else
  if (SockHost->GetStatus()==shsTryAgain){
    GetHostTrys++;
    if (GetHostTrys<MxGetHostTrys){
      TSockHost::GetAsyncSockHost(TrmClt->GetHostNm(), this);
    } else {
      OnError(-1, -1, "Can't get host info");
    }
  } else {
    OnError(-1, -1, "Invalid host");
  }
}

/////////////////////////////////////////////////
// Terminal-Client
TTrmClt::TTrmClt(
 const TStr& _HostNm, const int& _PortN,
 const TStr& _UsrNm, const TStr& _PwdStr):
  HostNm(_HostNm), PortN(_PortN),
  SockEvent(), Sock(), SockMOut(),
  Notify(),
  UsrNm(_UsrNm), PwdStr(_PwdStr){
  SockEvent=PSockEvent(new TTrmCltSockEvent(this));
  TSockEvent::Reg(SockEvent);
  TSockHost::GetAsyncSockHost(HostNm, SockEvent);
  Notify=PNotify(new TTrmCltNotify(this));
}

TTrmClt::~TTrmClt(){
  TSockEvent::UnReg(SockEvent);
}

void TTrmClt::SendTxt(const TStr& Txt){
  TMOut MOut;
  MOut.PutBf(Txt.CStr(), Txt.Len());
  PSIn SIn=MOut.GetSIn();
  Sock->SendSafe(SIn);
}

void TTrmClt::SendLn(const TStr& Ln){
  TMOut MOut;
  MOut.PutBf(Ln.CStr(), Ln.Len());
  MOut.MkEolnLn();
  PSIn SIn=MOut.GetSIn();
  Sock->SendSafe(SIn);
}

