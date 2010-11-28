/////////////////////////////////////////////////
// Smtp
const int TSmtp::DfSmtpPortN=25;

/////////////////////////////////////////////////
// Smtp-Socket-Event
const int TSmtpSockEvent::MxGetHostTrys=3;

int TSmtpSockEvent::GetRespCd(const TStr& RespCrLfLn){
  if (RespCrLfLn.Len()>=3){
    TStr RespCdStr=RespCrLfLn.GetSubStr(0, 2);
    int RespCd=0;
    if (RespCdStr.IsInt(RespCd)){return RespCd;} else {return -1;}
  } else {
    return -1;
  }
}

void TSmtpSockEvent::SendSmtpRq(
 const TStr& RqStr, const TSmtpConnMode& NewSmtpConnMode){
  Sock->Send(TStrIn::New(RqStr));
  Sock->PutTimeOut(100*1000);
  SmtpConnMode=NewSmtpConnMode;
}

void TSmtpSockEvent::OnSmtpError(const TStr& ErrStr){
  CloseConn();
  TStr MailIdStr=TStr("[Id")+TInt::GetStr(SmtpRq->GetMailId())+"]";
  SmtpClt->OnError(TStr("Smtp Error (")+ErrStr+") "+MailIdStr);
  SmtpClt->OnClose();
}

void TSmtpSockEvent::OnRead(const int&, const PSIn& SIn){
  SockMOut.Save(*SIn);
  if (SockMOut.IsCrLfLn()){
    TStr RespCrLfLn=SockMOut.GetCrLfLn();
    int RespCd=GetRespCd(RespCrLfLn);
    bool RespOk=true;
    switch (SmtpConnMode){
      case scmWaitConnAck:
        RespOk=(RespCd==220);
        if (RespOk){
          TStr RqStr=TStr("HELO Tralala\r\n");
          SendSmtpRq(RqStr, scmWaitHeloAck);
        }
        break;
      case scmWaitHeloAck:
        RespOk=(RespCd==250);
        if (RespOk){
          TStr RqStr=TStr("MAIL FROM:<")+SmtpRq->GetFromStr(0)+">\r\n";
          SendSmtpRq(RqStr, scmWaitMailAck);
        }
        break;
      case scmWaitMailAck:
        RespOk=(RespCd==250);
        if (RespOk){
          TStr RqStr=TStr("RCPT TO:<")+SmtpRq->GetToStr(0)+">\r\n";
          SendSmtpRq(RqStr, scmWaitRcptAck);
        }
        break;
      case scmWaitRcptAck:
        RespOk=(RespCd==250)||(RespCd==251);
        if (RespOk){
          SendSmtpRq("DATA\r\n", scmWaitDataAck);
        }
        break;
      case scmWaitDataAck:
        RespOk=(RespCd==354);
        if (RespOk){
          TChA MailLn; char Ch=' '; char PrevCh=' ';
          int DataMemLen=SmtpRq->GetDataMem().Len();
          for (int ChN=0; ChN<DataMemLen; ChN++){
            PrevCh=Ch; Ch=SmtpRq->GetDataMem()[ChN];
            if ((PrevCh==TCh::CrCh)&&(Ch==TCh::LfCh)){
              MailLn+=Ch;
              if (MailLn[0]=='.'){MailLn.Ins(0, ".");}
              Sock->Send(PSIn(new TMIn(MailLn)));
              MailLn.Clr();
            } else {
              MailLn+=Ch;
            }
          }
          if ((PrevCh==TCh::CrCh)&&(Ch==TCh::LfCh)){
            Sock->Send(PSIn(new TMIn(".\r\n")));
          } else {
            Sock->Send(PSIn(new TMIn(MailLn)));
            Sock->Send(PSIn(new TMIn("\r\n.\r\n")));
          }
          Sock->PutTimeOut(100*1000);
          SmtpConnMode=scmWaitTxtAck;
        }
        break;
      case scmWaitTxtAck:
        RespOk=(RespCd==250);
        if (RespOk){
          SendSmtpRq("QUIT\r\n", scmWaitQuitAck);
        }
        break;
      case scmWaitQuitAck:
        RespOk=(RespCd==221);
        if (RespOk){
          CloseConn();
          TStr MailIdStr=TStr("[Id")+TInt::GetStr(SmtpRq->GetMailId())+"]";
          SmtpConnMode=scmEnd;
          SmtpClt->OnDone(TStr("Mail sent ")+MailIdStr);
          SmtpClt->OnClose();
        }
        break;
      default: OnSmtpError("Bad reply line");
    }
    if (!RespOk){
      OnSmtpError("Bad reply code");
    }
  }
}

void TSmtpSockEvent::OnConnect(const int&){
  SmtpConnMode=scmWaitConnAck;
  Sock->PutTimeOut(100*1000);
}

void TSmtpSockEvent::OnClose(const int&){
  if ((SmtpConnMode!=scmWaitQuitAck)&&(SmtpConnMode!=scmEnd)){
    OnSmtpError("Server closed connection");
  }
}

void TSmtpSockEvent::OnTimeOut(const int&){
  OnSmtpError("Timeout");
}

void TSmtpSockEvent::OnError(const int&, const int&, const TStr& SockErrStr){
  CloseConn();
  TStr MailIdStr=TStr("[Id")+TInt::GetStr(SmtpRq->GetMailId())+"]";
  SmtpClt->OnError(TStr("Smtp Socket Error ("+SockErrStr+") "+MailIdStr));
  SmtpClt->OnClose();
}

void TSmtpSockEvent::OnGetHost(const PSockHost& _SockHost){
  if (_SockHost->IsOk()){
    SockHost=_SockHost;
    SockMOut.Clr();
    Sock=PSock(new TSock(this));
    Sock->Connect(SockHost, SmtpRq->GetSrvPortN());
    Sock->PutTimeOut(100*1000);
  } else
  if (_SockHost->GetStatus()==shsTryAgain){
    GetHostTrys++;
    if (GetHostTrys<MxGetHostTrys){
      TSockHost::GetAsyncSockHost(SmtpRq->GetSrvHostNm(), this);
    } else {
      OnSmtpError("Can't get host info");
    }
  } else {
    OnSmtpError("Invalid Host");
  }
}

/////////////////////////////////////////////////
// Smtp-Client
int TSmtpClt::SendIfAvail(){
  // start sending if free connection and waiting send-requests exist
  if (CurSmtpRq.Empty()&&(!SmtpRqQ.Empty())){
    // get current request
    CurSmtpRq=SmtpRqQ.Top(); SmtpRqQ.Pop();
    // create socket event
    CurSmtpSockEvent=PSockEvent(new TSmtpSockEvent(this, CurSmtpRq));
    TSockEvent::Reg(CurSmtpSockEvent);
    TSockHost::GetAsyncSockHost(CurSmtpRq->GetSrvHostNm(), CurSmtpSockEvent);
    return CurSmtpRq->GetMailId();
  } else {
    return -1;
  }
}

void TSmtpClt::OnClose(){
  CurSmtpRq=NULL;
  TSockEvent::UnReg(CurSmtpSockEvent);
  SendIfAvail();
}

int TSmtpClt::Send(const TStr& FromStr, const TStr& ToStr, TMem& DataMem){
  // create send-request
  LastMailId++;
  PSmtpRq SmtpRq=PSmtpRq(new TSmtpRq(
   this, LastMailId, SrvHostNm, SrvPortN, FromStr, ToStr, DataMem));
  // push send-request to queue
  SmtpRqQ.Push(SmtpRq);
  // send if free connection
  SendIfAvail();
  // return created send-request mail-id
  return LastMailId;
}

/////////////////////////////////////////////////
// Smtp-Server-Socket-Event
void TSmtpSrvSockEvent::OnRead(const int& SockId, const PSIn& SIn){
  SmtpSrv->OnRead(SockId, SIn);}
void TSmtpSrvSockEvent::OnWrite(const int& SockId){
  SmtpSrv->OnWrite(SockId);}
void TSmtpSrvSockEvent::OnOob(const int& SockId){
  SmtpSrv->OnOob(SockId);}
void TSmtpSrvSockEvent::OnAccept(const int& SockId, const PSock& Sock){
  SmtpSrv->OnAccept(SockId, Sock);}
void TSmtpSrvSockEvent::OnConnect(const int& SockId){
  SmtpSrv->OnConnect(SockId);}
void TSmtpSrvSockEvent::OnClose(const int& SockId){
  SmtpSrv->OnClose(SockId);}
void TSmtpSrvSockEvent::OnTimeOut(const int& SockId){
  SmtpSrv->OnTimeOut(SockId);}
void TSmtpSrvSockEvent::OnError(
 const int& SockId, const int& ErrCd, const TStr& ErrStr){
  SmtpSrv->OnError(SockId, ErrCd, ErrStr);}
void TSmtpSrvSockEvent::OnGetHost(const PSockHost& SockHost){
  SmtpSrv->OnGetHost(SockHost);}

/////////////////////////////////////////////////
// Smtp-Server
TSmtpSrv::TSmtpSrv(const PNotify& _Notify, const int& _PortN):
  Notify(_Notify),
  PortN(_PortN),
  HomeFPath(TDir::GetExeDir()),
  SockEvent(), Sock(),
  SockIdToConnH(){
  SockEvent=PSockEvent(new TSmtpSrvSockEvent(this));
  TSockEvent::Reg(SockEvent);
  Sock=TSock::New(SockEvent);
  try {Sock->Listen(PortN);}
  catch (...){TSockEvent::UnReg(SockEvent); throw;}
  // notify
  TChA MsgChA;
  MsgChA+="Smtp-Server: Started at port ";
  MsgChA+=TInt::GetStr(PortN); MsgChA+=".";
  TNotify::OnNotify(Notify, ntInfo, MsgChA);
}

TSmtpSrv::~TSmtpSrv(){
  TSockEvent::UnReg(SockEvent);
  TNotify::OnNotify(Notify, ntInfo, "Smtp-Server: Stopped.");
}

void TSmtpSrv::OnRead(const int& SockId, const PSIn& SIn){
  // get current connection
  PSmtpSrvConn Conn=GetConn(SockId);
  // get data to local-connection-buffer
  Conn->SockMOut.Save(*SIn);
  if (Conn->SockMOut.IsCrLfLn()){
    if (Conn->DataModeP){
      // data mode
      do {
        // get data
        TStr RqCrLfLn=Conn->SockMOut.GetCrLfLn();
        // process data
        if (RqCrLfLn==".\r\n"){
          // finish data retrieval
          Conn->Send("250 Ok\r\n");
          Conn->DataModeP=false;
        } else {
          // add data to data buffer
          Conn->DataMem+=RqCrLfLn;
        }
      } while (Conn->SockMOut.IsCrLfLn());
    } else {
      // command mode
      // get lowercase command line
      TStr RqCrLfLn=Conn->SockMOut.GetCrLfLn().GetLc();
      printf("'%s'\n", RqCrLfLn.CStr());
      if (RqCrLfLn.IsPrefix("helo")||RqCrLfLn.IsPrefix("ehlo")){
        Conn->Send("250 Ok\r\n");
      } else
      if (RqCrLfLn.IsPrefix("mail")){
        TStrV FromStrV;
        if (RqCrLfLn.IsWcMatch("mail from:<*>\r\n", FromStrV)){
          Conn->FromStrV.Add(FromStrV[0]);
          Conn->Send("250 Ok\r\n");
        } else {
          Conn->Send("501 Syntax error in parameters or arguments\r\n");
        }
      } else
      if (RqCrLfLn.IsPrefix("rcpt")){
        TStrV ToStrV;
        if (RqCrLfLn.IsWcMatch("rcpt to:<*>\r\n", ToStrV)){
          Conn->ToStrV.Add(ToStrV[0]);
          Conn->Send("250 Ok\r\n");
        } else {
          Conn->Send("501 Syntax error in parameters or arguments\r\n");
        }
      } else
      if (RqCrLfLn.IsPrefix("data")){
        Conn->Send("354 Start mail input; end with <CRLF>.<CRLF>\r\n");
        Conn->DataModeP=true;
      } else
      if (RqCrLfLn.IsPrefix("quit")){
        PSmtpRq SmtpRq=Conn->GetSmtpRq();
        OnSmtpRq(SockId, SmtpRq);
        Conn->Send("221 Service closing transmission channel\r\n");
        OnClose(SockId);
      } else {
        // unrecognized command
      }
    }
  }
  Sock->PutTimeOut(10*1000);
}

void TSmtpSrv::OnWrite(const int& SockId){
  // resend unsent data
  PSock Sock=GetConn(SockId)->GetSock();
  if (Sock->IsUnsent()){
    //int PrevLen=Sock->GetUnsentLen();
    Sock->Send(NULL);
    //int CurLen=Sock->GetUnsentLen();
    //TNotify::OnNotify(
    // Notify, ntInfo, TInt::GetStr(PrevLen)+"->"+TInt::GetStr(CurLen));
    if (!Sock->IsUnsent()){
      SockIdToConnH.DelKey(SockId);
    }
  }
}

void TSmtpSrv::OnAccept(const int& SockId, const PSock& Sock){
  // create new connection
  PSmtpSrvConn Conn=PSmtpSrvConn(new TSmtpSrvConn(Sock, this));
  SockIdToConnH.AddDat(SockId, Conn);
  Conn->Send("220 Tralala SMTP Server\r\n");
  Sock->PutTimeOut(10*1000);
}

void TSmtpSrv::OnClose(const int& SockId){
  // cancle connection
  SockIdToConnH.DelKey(SockId);
}

void TSmtpSrv::OnTimeOut(const int& SockId){
  // cancle connection
  if (SockIdToConnH.IsKey(SockId)){
    SockIdToConnH.DelKey(SockId);}
}

void TSmtpSrv::OnError(const int& SockId, const int& /*ErrCd*/, const TStr& ErrStr){
  // cancle connection
  SockIdToConnH.DelKey(SockId);
  // notify
  TChA MsgChA; MsgChA+="Smtp-Server: Error: '"; MsgChA+=ErrStr; MsgChA+="'.";
  TNotify::OnNotify(Notify, ntInfo, MsgChA);
}

void TSmtpSrv::OnSmtpRq(const int& /*SockId*/, const PSmtpRq& SmtpRq){
  TChA MsgChA;
  // from addresses
  MsgChA+="From: ";
  for (int FromStrN=0; FromStrN<SmtpRq->GetFromStrs(); FromStrN++){
    if (FromStrN>0){MsgChA+=", ";}
    MsgChA+='<'; MsgChA+=SmtpRq->GetFromStr(FromStrN); MsgChA+='>';
  }
  // to addresses
  MsgChA+=" To: ";
  for (int ToStrN=0; ToStrN<SmtpRq->GetToStrs(); ToStrN++){
    if (ToStrN>0){MsgChA+=", ";}
    MsgChA+='<'; MsgChA+=SmtpRq->GetToStr(ToStrN); MsgChA+='>';
  }
  // data
  MsgChA+=" Size: ";
  MsgChA+=TInt::GetStr(SmtpRq->GetDataMem().Len());
  MsgChA+="\n";
  MsgChA+=" Data:\n";
  MsgChA+=SmtpRq->GetDataMem().GetAsStr();
  MsgChA+="\n";
  // notify
  TNotify::OnNotify(Notify, ntInfo, MsgChA);
}

