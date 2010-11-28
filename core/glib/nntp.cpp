/////////////////////////////////////////////////
// Nntp
const int TNntp::DfNntpPortN=119;

/////////////////////////////////////////////////
// Nntp-Socket-Event
const int TNntpSockEvent::MxGetHostTrys=3;

int TNntpSockEvent::GetRespCd(const TStr& RespCrLfLn){
  if (RespCrLfLn.Len()>=3){
    TStr RespCdStr=RespCrLfLn.GetSubStr(0, 2);
    int RespCd=0;
    if (RespCdStr.IsInt(RespCd)){return RespCd;} else {return -1;}
  } else {
    return -1;
  }
}

void TNntpSockEvent::OnNntpError(const TStr& ErrStr){
  CloseConn();
  TStr MailIdStr=TStr("[")+NntpCmStr+"]";
  NntpClt->OnClose();
}

void TNntpSockEvent::OnRead(const int&, const PSIn& SIn){
  SockMOut.Save(*SIn);
  if (SockMOut.IsCrLfLn()){
    TStr RespCrLfLn=SockMOut.GetCrLfLn();
    int RespCd=GetRespCd(RespCrLfLn);
    bool RespOk=true;
    switch (NntpConnMode){
      case ncmWaitConnAck:
        RespOk=(RespCd==200);
        if (RespOk){
          Sock->Send(TStrIn::New(NntpCmStr));
          Sock->PutTimeOut(10*1000);
          NntpConnMode=ncmWaitCmAck;
        } else {
          OnNntpError("Invalid connect code");
        }
        break;
      case ncmWaitCmAck:
        break;
      default: OnNntpError("Bad reply line");
    }
  }
}

void TNntpSockEvent::OnConnect(const int&){
  NntpConnMode=ncmWaitConnAck;
  Sock->PutTimeOut(100*1000);
}

void TNntpSockEvent::OnClose(const int&){
  if (NntpConnMode!=ncmEnd){
    OnNntpError("Server closed connection");
  }
}

void TNntpSockEvent::OnTimeOut(const int&){
  OnNntpError("Timeout");
}

void TNntpSockEvent::OnError(const int&, const int&, const TStr& SockErrStr){
  CloseConn();
  TStr MailIdStr=TStr("[")+NntpCmStr+"]";
  NntpClt->OnClose();
}

void TNntpSockEvent::OnGetHost(const PSockHost& _SockHost){
  if (_SockHost->IsOk()){
    SockHost=_SockHost;
    SockMOut.Clr();
    Sock=PSock(new TSock(this));
    Sock->Connect(SockHost, NntpClt->GetSrvPortN());
    Sock->PutTimeOut(10*1000);
  } else
  if (_SockHost->GetStatus()==shsTryAgain){
    GetHostTrys++;
    if (GetHostTrys<MxGetHostTrys){
      TSockHost::GetAsyncSockHost(NntpClt->GetSrvHostNm(), this);
    } else {
      OnNntpError("Can't get host info");
    }
  } else {
    OnNntpError("Invalid Host");
  }
}

/////////////////////////////////////////////////
// Nntp-Client
void TNntpClt::OnClose(){
  TSockEvent::UnReg(NntpSockEvent);
}

void TNntpClt::GetGroupNmV(){
  NntpSockEvent=PSockEvent(new TNntpSockEvent(this, "list"));
  TSockEvent::Reg(NntpSockEvent);
  TSockHost::GetAsyncSockHost(GetSrvHostNm(), NntpSockEvent);
  TSysMsg::Loop();
}

