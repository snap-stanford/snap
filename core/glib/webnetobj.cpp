//////////////////////////////////////////////////
// Web-Net-Timer
void TWebNetTimer::OnTimeOut(){
  WebNetClt->OnCheckTimeOut();
}

/////////////////////////////////////////////////
// Web-Net-Server
TWebNetSrv::TWebNetSrv(
 const int& PortN, const bool& FixedPortNP, const PNotify& _Notify):
  TNetSrv(PortN, FixedPortNP), Notify(_Notify) {
}

void TWebNetSrv::OnNetObj(const PNetObj& NetObj){
  TStr NetObjTypeNm=GetTypeNm(*NetObj);
  if (NetObjTypeNm==TTypeNm<TNetClose>()){
    OnHttpRqError(NetObj->GetSockId(), "Connection Closed by Client");
  } else
  if (NetObjTypeNm==TTypeNm<TNetErr>()){
    TNetErr& NetErr=*(TNetErr*)NetObj();
    OnHttpRqError(NetObj->GetSockId(), TStr("Error: ")+NetErr.GetMsgStr());
  } else
  if (NetObjTypeNm==TTypeNm<TNetMem>()){
    OnNetMem(NetObj);
  }
}

void TWebNetSrv::OnNetMem(const PNetObj& NetObj){
  TNetMem& NetMem=*(TNetMem*)NetObj();
  const TMem& Mem=NetMem.GetMem();
  PSIn MemIn=TMemIn::New(Mem);
  PHttpRq HttpRq=THttpRq::New(MemIn);
  OnHttpRq(NetObj->GetSockId(), HttpRq);
}

void TWebNetSrv::SendHttpResp(const int& SockId, const PHttpResp& HttpResp){
  TMem Mem; HttpResp->GetAsMem(Mem);
  PNetObj NetObj=PNetObj(new TNetMem(Mem));
  SendNetObj(SockId, NetObj);
}

/////////////////////////////////////////////////
// Web-Net-Client
TWebNetClt::TWebNetClt(
 const TStr& HostNm, const int& PortN,
  TWebNetCltV* _WebNetCltV, const TStr& _IdStr, const PNotify& _Notify):
  TNetClt(HostNm, PortN),
  ConnIdStr(_IdStr),
  Notify(_Notify),
  WebNetCltV(_WebNetCltV),
  ConnectedP(false),
  Wait_FetchIdHttpRqCreateTmTrQ(),
  Timer(){
  Timer=TWebNetTimer::New(this);
}

TWebNetClt::~TWebNetClt(){}

void TWebNetClt::OnNetObj(const PNetObj& NetObj){
  TStr NetObjTypeNm=GetTypeNm(*NetObj);
  TNotify::OnNotify(Notify, ntInfo, TStr("TWebNetClt: ")+NetObj->GetStr());
  if (NetObjTypeNm==TTypeNm<TNetConn>()){
    ConnectedP=true; SendWaitQ();
  } else
  if (NetObjTypeNm==TTypeNm<TNetClose>()){
    OnDisconn("Socket closed.");
  } else
  if (NetObjTypeNm==TTypeNm<TNetErr>()){
    TNetErr& NetErr=*(TNetErr*)NetObj();
    OnDisconn(TStr("Error: ")+NetErr.GetMsgStr());
  } else
  if (NetObjTypeNm==TTypeNm<TNetMem>()){
    OnNetMem(NetObj);
  } else {Fail;}
}

void TWebNetClt::OnNetMem(const PNetObj& NetObj){
  TNetMem& NetMem=*(TNetMem*)NetObj();
  const TMem& Mem=NetMem.GetMem();
  IAssert("Screwed NetMem"&&!memchr(Mem(),0,Mem.Len()));
  PSIn MemIn=TMemIn::New(Mem);
  PHttpResp HttpResp=THttpResp::New(MemIn);
  OnHttpResp(HttpResp);
}

void TWebNetClt::OnHttpResp(const PHttpResp& HttpResp){
  IAssert(HttpResp->IsOk());
  // get fetch-id
  TStr FetchIdStr=HttpResp->GetFldVal(THttp::FetchIdFldNm);
  int FetchId=FetchIdStr.GetInt(-1);

  // check if response has correct fetch-id
  if (IsSentQEmpty()){return;} // queue can be emptied by timeout...
  int TopFetchId; TTm TopCreateTm; TopFromSentQ(TopFetchId, TopCreateTm);
  IAssert(FetchId==TopFetchId);
  PopFromSentQ();

  // get status-code
  int StatusCd=HttpResp->GetStatusCd();
  // process http-response
  if (StatusCd/100==2){ // codes 2XX - ok
    WebNetCltV->OnHttpResp(FetchId, HttpResp);
  } else
  if (StatusCd/100==3){ // codes 3XX - redirection
    TStr RedirUrlStr=HttpResp->GetFldVal("Location");
    PUrl RedirUrl=TUrl::New(RedirUrlStr);
    if (RedirUrl->IsOk(usHttp)){
      //SaveToErrLog((TStr("Redirect: ")+HttpResp->GetHdStr()).CStr());
      TStr Host=HttpResp->GetFldVal(THttp::HostFldNm);
      WebNetCltV->FetchUrl(RedirUrl, FetchId, Host);
    } else {
      TStr MsgStr=TStr("Invalid Redirection URL (")+RedirUrlStr+")";
      WebNetCltV->OnHttpRespError(FetchId, MsgStr);
    }
  } else { // all other codes - error
    TStr MsgStr=TStr("Http Error (")+
     TInt::GetStr(StatusCd)+"/"+HttpResp->GetReasonPhrase()+")";
    WebNetCltV->OnHttpRespError(FetchId, MsgStr);
  }
}

void TWebNetClt::SendHttpRq(const PHttpRq& HttpRq){
  // push fetch-id & create time to waiting-list
  int FetchId=HttpRq->GetFldVal(THttp::FetchIdFldNm).GetInt(-1);
  IAssert(FetchId!=-1);
  TMem Mem; HttpRq->GetAsMem(Mem);
  IAssert("Screwed NetMem"&&!memchr(Mem(),0,Mem.Len()));
  PNetObj NetObj=PNetObj(new TNetMem(Mem));
  SendNetObj(NetObj);
  PushToSentQ(FetchId);
}

void TWebNetClt::SendWaitQ(){
  while (!IsWaitQEmpty()){
    int FetchId; PHttpRq HttpRq; TTm CreateTm;
    PopFromWaitQ(FetchId, HttpRq, CreateTm);
    IAssert(FetchId!=-1);
    SendHttpRq(HttpRq);
  }
}

void TWebNetClt::OnCheckTimeOut(){
  // get current-time
  TTm CurTm=TTm::GetCurUniTm();
  int Timeout=WebNetCltV->GetCltTimeout();
  // by default do not disconnect
  bool DisconnP=false;
  // sent-queue
  if (!IsSentQEmpty()){
    int FetchId; TTm CreateTm;
    TopFromSentQ(FetchId, CreateTm);
    // check if timeout
    uint64 DiffMSecs=TTm::GetDiffMSecs(CurTm, CreateTm);
    DisconnP = DiffMSecs>Timeout;
  }
  // wait-queue
  if (!IsWaitQEmpty()){
    int FetchId; PHttpRq HttpRq; TTm CreateTm;
    TopFromWaitQ(FetchId, HttpRq, CreateTm);
    // check if http-requests expired
    uint64 DiffMSecs=TTm::GetDiffMSecs(CurTm, CreateTm);
    DisconnP |= DiffMSecs>Timeout;
  }
  // disconnect if time-out appeared
  if (DisconnP){
    OnDisconn("Timeout.");
  }
}

void TWebNetClt::OnDisconn(const TStr& MsgStr){
  //if (WebNetCltV->IsFetching()){return;} // wait for the time-out to disconnect
  // delete client
  PWebNetClt KeepRef=this;
  WebNetCltV->DelIfWebNetClt(GetConnIdStr());

  // sent-queue
  while (!IsSentQEmpty()){
    int FetchId; TTm CreateTm;
    PopFromSentQ(FetchId, CreateTm);
    WebNetCltV->OnHttpRespError(FetchId,MsgStr);
  }
  // wait-queue
  while (!IsWaitQEmpty()){
    int FetchId; PHttpRq HttpRq; TTm CreateTm;
    PopFromWaitQ(FetchId, HttpRq, CreateTm);
    WebNetCltV->OnHttpRespError(FetchId,MsgStr);
  }
}

/////////////////////////////////////////////////
// Web-Net-Client-Vector
TWebNetCltV::TWebNetCltV(const PNotify& _Notify):
  Notify(_Notify),
  ConnIdStrToWebNetCltH(),
  /*FetchingP(false), */LastFetchId(0),
  CltTimeout(TWebNetCltV::DfCltTimeout) {
}

void TWebNetCltV::SendHttpRq(const TStr& ConnIdStr, const PHttpRq& HttpRq){
  PWebNetClt WebNetClt=ConnIdStrToWebNetCltH.GetDat(ConnIdStr);
  WebNetClt->SendHttpRq(HttpRq);
}

int TWebNetCltV::FetchUrl(const PUrl& Url, const int& _FetchId, TStr ConnUid){
  IAssert(Url->IsOk(usHttp));
  //FetchingP=true;
  // prepare fetch-id & http-request
  int FetchId=_FetchId;
  if (_FetchId==-1){
    FetchId=GetNextFetchId();}
  PHttpRq HttpRq=THttpRq::New(Url, FetchId);
  // prepare host-port-string
  TStr HostNm=Url->GetHostNm();
  int PortN=Url->GetPortN();
  TStr ConnIdStr=TWebNetClt::GetConnIdStr(HostNm, PortN, ConnUid);
  // check if already connected
  PWebNetClt WebNetClt;
  if (IsGetWebNetClt(ConnIdStr, WebNetClt)){
    if (WebNetClt->IsConnected()){
      // already connected; send http-request
      WebNetClt->SendHttpRq(HttpRq);
    } else {
      // not connected; process of connecting
      WebNetClt->PushToWaitQ(FetchId, HttpRq);
    }
  } else {
    // not connected; connect
    //SaveToErrLog((TStr("NetCltConnect: ")+ConnIdStr).CStr());
    WebNetClt=TWebNetClt::New(HostNm, PortN, this, ConnIdStr, Notify);
    AddWebNetClt(ConnIdStr, WebNetClt);
    WebNetClt->PushToWaitQ(FetchId, HttpRq);
  }
  //FetchingP=false;
  return FetchId;
}

/////////////////////////////////////////////////
// Web-Net-Proxy-Connection
void TProxyConn::SendHttpResp(TWebNetSrv &WebNetSrv, PHttpResp HttpResp) {
  if (WebNetSrv.IsCltSock(RqSockId)) {
    //!!warning - two fetch id entries in HTTP header
    // rebuild http header to replace 'fetchid'

    HttpResp->AddFldVal(THttp::FetchIdFldNm,TInt(RqFetchId).GetStr());
    WebNetSrv.SendHttpResp(RqSockId, HttpResp);
    //SaveToErrLog((TStr("ProxyRqOk: ")+HttpResp->GetHdStr()).CStr());
  }
}

/////////////////////////////////////////////////
// Web-Net-Proxy
void TWebNetProxy::OnHttpRq(const int& SockId, const PHttpRq& HttpRq) {
  TStr RqFetchIdStr=HttpRq->GetFldVal(THttp::FetchIdFldNm);
  int RqFetchId=RqFetchIdStr.GetInt(-1);
  TStr Host;//=HttpRq->GetFldVal(THttp::HostFldNm);
  if (Host.Empty() || (Host=="Tralala")) Host = DfSrv;
  if (Host.Empty()) {OnHttpRqError(SockId,"Missing proxy target."); return;}
  if (RqFetchId==-1) {OnHttpRqError(SockId,"Missing fetch-id."); return;}
  // build new url
  TStr RelUrlStr=HttpRq->GetUrl()->GetRelUrlStr();
  PUrl RedirUrl=TUrl::New(Host+RelUrlStr);
  //SaveToErrLog((TStr("ProxyTo: ")+RedirUrl->GetUrlStr()).CStr());
  int RespFetchId = FetchUrl(RedirUrl);

  PProxyConn Conn = new TProxyConn(SockId,RqFetchId,RespFetchId);
  RqSockIdToProxyH.AddDat(SockId,Conn);
  RespFetchIdToProxyH.AddDat(RespFetchId,Conn);
};
void TWebNetProxy::OnHttpRqError(const int& SockId, const TStr& MsgStr) {
  SaveToErrLog((TStr("ProxyRequestError: ")+MsgStr).CStr());
  PProxyConn Conn;
  if (RqSockIdToProxyH.IsKeyGetDat(SockId,Conn)) {
    // send some kind of an error http response immediately to avoid timeouts
    PHttpResp HttpResp=THttpResp::New(503, "", false, NULL, TStr());
    Conn->SendHttpResp(*this,HttpResp);
    RqSockIdToProxyH.DelIfKey(SockId);
    RespFetchIdToProxyH.DelIfKey(Conn->GetRespFetchId());
  }
}
void TWebNetProxy::OnHttpResp(const int& RespFetchId, const PHttpResp& HttpResp) {
  PProxyConn Conn;
  if (RespFetchIdToProxyH.IsKeyGetDat(RespFetchId,Conn)) {
    Conn->SendHttpResp(*this,HttpResp);
    RespFetchIdToProxyH.DelKey(RespFetchId);
    RqSockIdToProxyH.DelIfKey(Conn->GetRqSockId());
  }
}
void TWebNetProxy::OnHttpRespError(const int& RespFetchId, const TStr& MsgStr) {
  SaveToErrLog((TStr("ProxyRequestFailed: ")+MsgStr).CStr());
  PProxyConn Conn;
  if (RespFetchIdToProxyH.IsKeyGetDat(RespFetchId,Conn)) {
    // send some kind of an error http response immediately to avoid timeouts
    PHttpResp HttpResp=THttpResp::New(503, "", false, NULL, TStr());
    Conn->SendHttpResp(*this,HttpResp);
    RespFetchIdToProxyH.DelKey(RespFetchId);
    RqSockIdToProxyH.DelIfKey(Conn->GetRqSockId());
  }
}

