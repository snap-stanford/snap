/////////////////////////////////////////////////
// Net-Object
THash<TStr, TNetObj::TNetObjLoadF> TNetObj::TypeToLoadFH;

PNetObj TNetObj::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  int TypeKeyId=-1;
  if (TypeToLoadFH.IsKey(TypeNm, TypeKeyId)){
    TNetObjLoadF LoadF=TypeToLoadFH[TypeKeyId];
    return (*LoadF())(SIn);
  } else {
    return NULL;
  }
}

bool TNetObj::Reg(const TStr& TypeNm, const TNetObjLoadF& LoadF){
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

PSIn TNetObj::GetPckSIn(const PNetObj& NetObj){
  // serialize net-object
  TMOut NetObjMOut; NetObj->Save(NetObjMOut);
  // prepare packet header
  TNetObjPckHd PckHd(NetObjMOut.Len());
  // prepare packet contents
  TMOut PckMOut; PckHd.Save(PckMOut); PckMOut.Save(NetObjMOut.GetSIn(true));
  return PckMOut.GetSIn(true);
}

PNetObj TNetObj::GetPckNetObj(TMOut& MOut, bool& Ok){
  Ok=true;
  while (MOut.Len()>=TNetObjPckHd::GetSize()){
    PSIn PckSIn=MOut.GetSIn(false, TNetObjPckHd::GetSize());
    TNetObjPckHd PckHd(*PckSIn, Ok);
    if (!Ok){break;}
    if (MOut.Len()<TNetObjPckHd::GetSize()+PckHd.GetPckLen()){break;}
    PSIn VoidPckSIn=MOut.GetSIn(true, TNetObjPckHd::GetSize());
    TNetObjPckHd VoidPckHd(*VoidPckSIn, Ok);
    PSIn NetObjSIn=MOut.GetSIn(true, PckHd.GetPckLen());
    return TNetObj::Load(*NetObjSIn);
  }
  return NULL;
}

/*
PSIn TNetObj::GetPckSIn(const PNetObj& NetObj){
  // serialize net-object
  TMOut NetObjMOut; NetObj->Save(NetObjMOut);
  // prepare packet header
  TNetObjPckHd PckHd(NetObjMOut.Len(), TMd5Sig(NetObjMOut));
  // prepare packet contents
  TMOut PckMOut; PckHd.Save(PckMOut); PckMOut.Save(NetObjMOut.GetSIn(true));
  return PckMOut.GetSIn(true);
}

PNetObj TNetObj::GetPckNetObj(TMOut& MOut, bool& Ok){
  //** test
  TMem Mem;
  for (int BfC=0; BfC<MOut.Len(); BfC++){
    Mem+=MOut.GetCh(BfC);}

  Ok=true;
  while (MOut.Len()>=TNetObjPckHd::GetSize()){
    // create packet-header to check if enough bytes for net-object
    PSIn PckSIn=MOut.GetSIn(false, TNetObjPckHd::GetSize());
    TNetObjPckHd PckHd(*PckSIn, Ok);
    if (!Ok){break;}
    // if not enough bytes for net-object, stop creating object
    if (MOut.Len()<TNetObjPckHd::GetSize()+PckHd.GetPckLen()){break;}
    // create void packet-header
    PSIn VoidPckSIn=MOut.GetSIn(true, TNetObjPckHd::GetSize());
    TNetObjPckHd VoidPckHd(*VoidPckSIn, Ok);
    // md5 signature test
    TMd5Sig Md5Sig(MOut);
    if ((!Md5Sig.IsNull())&&(!PckHd.GetMd5Sig().IsNull())){
      if (Md5Sig!=PckHd.GetMd5Sig()){
        Fail;}
    }
    // create net-object
    PSIn NetObjSIn=MOut.GetSIn(true, PckHd.GetPckLen());
    PNetObj NetObj=TNetObj::Load(*NetObjSIn);
    // return net-object
    return NetObj;
  }
  return NULL;
}*/

/////////////////////////////////////////////////
// Net-Client-Acknowledge
bool TNetAck::IsReg=TNetAck::MkReg();

/////////////////////////////////////////////////
// Net-Object-Notify
bool TNetNotify::IsReg=TNetNotify::MkReg();

/////////////////////////////////////////////////
// Net-Object-Error
bool TNetErr::IsReg=TNetErr::MkReg();

/////////////////////////////////////////////////
// Net-Object-Accept
bool TNetAccept::IsReg=TNetAccept::MkReg();

/////////////////////////////////////////////////
// Net-Object-Connect
bool TNetConn::IsReg=TNetConn::MkReg();

/////////////////////////////////////////////////
// Net-Object-Close
bool TNetClose::IsReg=TNetClose::MkReg();

/////////////////////////////////////////////////
// Net-Object-TimeOut
bool TNetTimeOut::IsReg=TNetTimeOut::MkReg();

/////////////////////////////////////////////////
// Net-Object-Registration
bool TNetReg::IsReg=TNetReg::MkReg();

TStr TNetReg::GetRegTypeStr(const int& NetRegType){
  switch (NetRegType){
    case nrtLogin: return "Login";
    case nrtConfirm: return "Confirm";
    case nrtRefuse: return "Refuse";
    case nrtLogout: return "Logout";
    default: Fail; return TStr();
  }
}

/////////////////////////////////////////////////
// Net-Object-Chat-Message
bool TNetChatMsg::IsReg=TNetChatMsg::MkReg();

TStr TNetChatMsg::GetLogStr(){
  TChA LogChA;
  LogChA+=FromAddrStr;
  LogChA+=" > [";
  for (int ToAddrStrN=0; ToAddrStrN<GetToAddrStrs(); ToAddrStrN++){
    if (ToAddrStrN>0){LogChA+=", ";}
    LogChA+=GetToAddrStr(ToAddrStrN);
  }
  LogChA+="]: ";
  LogChA+=MsgStr;
  return LogChA;
}

/////////////////////////////////////////////////
// Net-Object-Memory
bool TNetMem::IsReg=TNetMem::MkReg();

/////////////////////////////////////////////////
// Net-Server-Socket-Event
void TNetSrvSockEvent::OnAccept(const int& SockId, const PSock& Sock){
  NetSrv->CltSockIdToSockH.AddDat(SockId, Sock);
  PNetObj NetAccept=PNetObj(new TNetAccept(SockId));
  NetAccept->PutSockId(SockId);
  NetSrv->OnNetObj(NetAccept);
}

void TNetSrvSockEvent::OnRead(const int& SockId, const PSIn& SIn){
  NetSrv->SockMOut.Save(*SIn);
  forever {
    bool Ok; PNetObj NetObj=TNetObj::GetPckNetObj(NetSrv->SockMOut, Ok);
    if (!Ok){OnError(SockId, -1, "Error reading net-object."); break;}
    if (NetObj.Empty()){break;}
    else {NetObj->PutSockId(SockId); NetSrv->OnNetObj(NetObj);}
    if (!TSock::IsSockId(SockId)){
      break;}
  }
}


void TNetSrvSockEvent::OnWrite(const int& SockId){
  PSock Sock=NetSrv->CltSockIdToSockH.GetDat(SockId);
  if (!Sock.Empty()&&Sock->IsUnsent()){Sock->Send(PSIn());}
}

void TNetSrvSockEvent::OnClose(const int& SockId){
  PSock CltSock=NetSrv->CltSockIdToSockH.GetDat(SockId);
  CltSock->PutTimeOut(1);
}

void TNetSrvSockEvent::OnTimeOut(const int& SockId){
  PNetObj NetClose=PNetObj(new TNetClose());
  NetClose->PutSockId(SockId);
  NetSrv->OnNetObj(NetClose);
  NetSrv->DisconnClt(SockId);
}

void TNetSrvSockEvent::OnError(
 const int& SockId, const int& ErrCd, const TStr& MsgStr){
  PNetObj NetErr=PNetObj(new TNetErr(ErrCd, MsgStr));
  NetErr->PutSockId(SockId);
  NetSrv->OnNetObj(NetErr);
  NetSrv->DisconnClt(SockId);
}

/////////////////////////////////////////////////
// Net-Server
TNetSrv::TNetSrv(const int& _PortN, const bool& FixedPortNP):
  PortN(_PortN),
  SrvSockEvent(), SrvSock(),
  CltSockIdToSockH(100),
  SockMOut(){
  SrvSockEvent=PSockEvent(new TNetSrvSockEvent(this));
  TSockEvent::Reg(SrvSockEvent);
  SrvSock=TSock::New(SrvSockEvent);
  try {
    if (FixedPortNP){
      SrvSock->Listen(PortN);
    } else {
      PortN=SrvSock->GetPortAndListen(PortN);
    }
  } catch (...){TSockEvent::UnReg(SrvSockEvent); throw;}
}

TNetSrv::~TNetSrv(){
  TSockEvent::UnReg(SrvSockEvent);
}

void TNetSrv::Disconn(){
  // close all clients
  int SockIdToSockP=CltSockIdToSockH.FFirstKeyId();
  while (CltSockIdToSockH.FNextKeyId(SockIdToSockP)){
    int CltSockId=CltSockIdToSockH.GetKey(SockIdToSockP);
    DisconnClt(CltSockId);
  }
  // close server socket
  SrvSock=NULL;
}

void TNetSrv::DisconnClt(const int& CltSockId){
  CltSockIdToSockH.DelKey(CltSockId);
}

void TNetSrv::SendNetObj(const int& CltSockId, const PNetObj& NetObj){
  // send net-object to one client
  NetObj->PutSockId(CltSockId);
  PSock CltSock=CltSockIdToSockH.GetDat(CltSockId);
  CltSock->Send(TNetObj::GetPckSIn(NetObj));
}

void TNetSrv::BroadcastNetObj(const PNetObj& NetObj){
  // send net-object to all clients
  int SockIdToSockP=CltSockIdToSockH.FFirstKeyId();
  while (CltSockIdToSockH.FNextKeyId(SockIdToSockP)){
    int CltSockId=CltSockIdToSockH.GetKey(SockIdToSockP);
    SendNetObj(CltSockId, NetObj);
  }
}

/////////////////////////////////////////////////
// Net-Client-Socket-Event
const int TNetCltSockEvent::MxGetHostTrys=3;

TNetCltSockEvent::TNetCltSockEvent(TNetClt* _NetClt):
  Mode(csmGetingHost), GetHostTrys(0),
  NetClt(_NetClt){}

void TNetCltSockEvent::OnRead(const int& SockId, const PSIn& SIn){
  IAssert(Mode==csmConnected);
  NetClt->SockMOut.Save(*SIn);
  forever {
    bool Ok; PNetObj NetObj=TNetObj::GetPckNetObj(NetClt->SockMOut, Ok);
    if (!Ok){OnError(SockId, -1, "Error reading net-object."); break;}
    if (NetObj.Empty()){break;}
    else {NetObj->PutSockId(SockId); NetClt->OnNetObj(NetObj);}
    if (!TSock::IsSockId(SockId)){
      break;}
  }
}

void TNetCltSockEvent::OnWrite(const int& SockId){
  if (!NetClt->Sock.Empty()&&NetClt->Sock->IsUnsent()){
    NetClt->Sock->Send(PSIn());
  }
}

void TNetCltSockEvent::OnConnect(const int& SockId){
  IAssert(Mode==csmConnecting);
  Mode=csmConnected;
  PNetObj NetConn=PNetObj(new TNetConn());
  NetConn->PutSockId(SockId);
  NetClt->OnNetObj(NetConn);
  NetClt->Sock->DelTimeOut();
}

void TNetCltSockEvent::OnClose(const int&){
  Mode=csmAfterClose;
  NetClt->Sock->PutTimeOut(1);
}

void TNetCltSockEvent::OnTimeOut(const int& SockId){
  NetClt->Sock->DelTimeOut();
  switch (Mode){
    case csmConnecting: // timeout waiting for connect
      OnError(SockId, -1, "Connecting timeout"); break;
    case csmConnected: // timeout after successful OnConnect
      break;
    case csmAfterClose: // safety timeout after OnClose
      {PNetObj NetClose=PNetObj(new TNetClose());
      NetClose->PutSockId(SockId);
      NetClt->OnNetObj(NetClose); break;}
    default: Fail;
  }
}

void TNetCltSockEvent::OnError(
 const int& SockId, const int& ErrCd, const TStr& MsgStr){
  PNetObj NetErr=PNetObj(new TNetErr(ErrCd, MsgStr));
  NetErr->PutSockId(SockId);
  NetClt->OnNetObj(NetErr);
}

void TNetCltSockEvent::OnGetHost(const PSockHost& SockHost){
  IAssert(Mode==csmGetingHost);
  if (SockHost->IsOk()){
    Mode=csmConnecting;
    NetClt->Sock=PSock(new TSock(this));
    NetClt->Sock->Connect(SockHost, NetClt->GetPortN());
    NetClt->Sock->PutTimeOut(10*1000);
  } else
  if (SockHost->GetStatus()==shsTryAgain){
    GetHostTrys++;
    if (GetHostTrys<MxGetHostTrys){
      TSockHost::GetAsyncSockHost(NetClt->GetHostNm(), this);
    } else {
      OnError(-1, -1, "Can't get host info");
    }
  } else {
    OnError(-1, -1, "Invalid host");
  }
}

/////////////////////////////////////////////////
// Net-Client
TNetClt::TNetClt(
 const TStr& _HostNm, const int& _PortN,
 const TStr& _UsrNm, const TStr& _PwdStr):
  HostNm(_HostNm), PortN(_PortN),
  SockEvent(), Sock(), SockMOut(),
  UsrNm(_UsrNm), PwdStr(_PwdStr){
  SockEvent=PSockEvent(new TNetCltSockEvent(this));
  TSockEvent::Reg(SockEvent);
  TSockHost::GetAsyncSockHost(HostNm, SockEvent);
}

TNetClt::~TNetClt(){
  TSockEvent::UnReg(SockEvent);
}

void TNetClt::SendNetObj(const PNetObj& NetObj){
  NetObj->PutSockId(Sock->GetSockId());
  Sock->Send(TNetObj::GetPckSIn(NetObj));
}

