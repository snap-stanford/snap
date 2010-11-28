/////////////////////////////////////////////////
// Web-Page-Fetch-Event
class TWebPgFetchEvent: public TSockEvent{
private:
  TWebPgFetch* Fetch;
  int FId;
  PUrl TopUrl;
  TStr ProxyStr;
  PUrl CurUrl;
  TStrV UrlStrV;
  TStrV IpNumV;
  TStrQuV CookieKeyValDmPathQuV;
  PSockHost SockHost;
  PSock Sock;
  int Retries;
  TBool OppSockClosed;
  TMem SockMem;
  uint64 StartMSecs, EndMSecs;
  void AddCookieKeyValDmPathQuV(const TStrQuV& KeyValDmPathQuV);
  int GetCookies() const {return CookieKeyValDmPathQuV.Len();}
  void GetCookie(const int& CookieN, TStr& KeyNm, TStr& ValStr, TStr& DmNm, TStr& PathStr) const {
    KeyNm=CookieKeyValDmPathQuV[CookieN].Val1; ValStr=CookieKeyValDmPathQuV[CookieN].Val2; 
    DmNm=CookieKeyValDmPathQuV[CookieN].Val3; PathStr=CookieKeyValDmPathQuV[CookieN].Val4;}
  void ChangeLastUrlToLc(const PHttpResp& HttpResp);
  void CloseConn(){SockHost.Clr(); Sock.Clr();}
public:
  TWebPgFetchEvent(TWebPgFetch* _Fetch, const int& _FId,
   const PUrl& _Url, const TStr& _ProxyStr);
  ~TWebPgFetchEvent(){}

  TSockEvent& operator=(const TSockEvent&){Fail; return *this;}

  void OnFetchError(const TStr& MsgStr);
  void OnFetchEnd(const PHttpResp& HttpResp);
  void OnFetchEnd();

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int& SockId);
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int&, const int&, const TStr&);
  void OnGetHost(const PSockHost& _SockHost);

  int GetFetchMSecs() const {
    if ((StartMSecs==-1)||(EndMSecs==-1)){return -1;} else {return int(EndMSecs-StartMSecs);}}
  int GetBytesRead() const {
    return SockMem.Len();}
};

void TWebPgFetchEvent::AddCookieKeyValDmPathQuV(const TStrQuV& KeyValDmPathQuV){
  for (int KeyN=0; KeyN<KeyValDmPathQuV.Len(); KeyN++){
    TStr KeyNm=KeyValDmPathQuV[KeyN].Val1;
    TStr ValStr=KeyValDmPathQuV[KeyN].Val2;
    TStr DmNm=KeyValDmPathQuV[KeyN].Val3;
    TStr PathStr=KeyValDmPathQuV[KeyN].Val4;
    bool IsUpdated=false;
    for (int CookieN=0; CookieN<CookieKeyValDmPathQuV.Len(); CookieN++){
      TStr CookieKeyNm=CookieKeyValDmPathQuV[CookieN].Val1;
      TStr CookieDmNm=CookieKeyValDmPathQuV[CookieN].Val3;
      if ((KeyNm==CookieKeyNm)&&(DmNm==CookieDmNm)){
        CookieKeyValDmPathQuV[CookieN].Val2=ValStr;
        IsUpdated=true; break;
      }
    }
    if (!IsUpdated){
      CookieKeyValDmPathQuV.Add(TStrQu(KeyNm, ValStr, DmNm, PathStr));
    }
  }
}

void TWebPgFetchEvent::ChangeLastUrlToLc(const PHttpResp& HttpResp){
  static TStr MsNm="Microsoft";
  static TStr HttpsNm="HTTPS";
  TStr SrvNm=HttpResp->GetSrvNm();
  if ((SrvNm.IsPrefix(MsNm))||(SrvNm.IsPrefix(HttpsNm))){
    if (!UrlStrV.Last().IsLc()){
      PUrl Url=TUrl::New(UrlStrV.Last());
      Url->ToLcPath();
      UrlStrV.Last()=Url->GetUrlStr();
    }
  }
}

TWebPgFetchEvent::TWebPgFetchEvent(
 TWebPgFetch* _Fetch, const int& _FId, const PUrl& _Url, const TStr& _ProxyStr):
  TSockEvent(),
  Fetch(_Fetch), FId(_FId),
  TopUrl(_Url), ProxyStr(_ProxyStr), CurUrl(_Url), UrlStrV(),
  SockHost(NULL), Sock(NULL),
  Retries(0), OppSockClosed(true),
  SockMem(), StartMSecs(TTm::GetCurUniMSecs()), EndMSecs(-1){
}

void TWebPgFetchEvent::OnFetchError(const TStr& MsgStr){
  CloseConn(); Fetch->DisconnUrl(FId);
  Fetch->OnError(FId, MsgStr+" ["+CurUrl->GetUrlStr()+"]");
}

void TWebPgFetchEvent::OnFetchEnd(const PHttpResp& HttpResp){
  IAssert(HttpResp->IsOk());
  EndMSecs=TTm::GetCurUniMSecs();
  int StatusCd=HttpResp->GetStatusCd();
  if (StatusCd/100==2){ // codes 2XX - ok
    ChangeLastUrlToLc(HttpResp);
    PWebPg WebPg=TWebPg::New(UrlStrV, IpNumV, HttpResp);
    WebPg->PutFetchMSecs(GetFetchMSecs());
    CloseConn(); Fetch->DisconnUrl(FId);
    Fetch->OnFetch(FId, WebPg);
  } else
  if (StatusCd/100==3){ // codes 3XX - redirection
    ChangeLastUrlToLc(HttpResp);
    if (UrlStrV.Len()<5){
      TStr RedirUrlStr=HttpResp->GetFldVal("Location");
      PUrl RedirUrl=TUrl::New(RedirUrlStr, CurUrl->GetUrlStr());
      if (RedirUrl->IsOk(usHttp)){
        TStrQuV CookieQuV; HttpResp->GetCookieKeyValDmPathQuV(CookieQuV);
        AddCookieKeyValDmPathQuV(CookieQuV);
        CurUrl=RedirUrl; Retries=0; CloseConn();
        TSockHost::GetAsyncSockHost(CurUrl->GetIpNumOrHostNm(), this);
      } else {
        TStr MsgStr=TStr("Invalid Redirection URL (")+RedirUrlStr+")";
        OnFetchError(MsgStr);
      }
    } else {
      TStr MsgStr=TStr("Cycling Redirection [")+TopUrl->GetUrlStr()+"]";
      OnFetchError(MsgStr);
    }
  } else { // all other codes - error
    TStr MsgStr=TStr("Http Error (")+
     TInt::GetStr(StatusCd)+"/"+HttpResp->GetReasonPhrase()+")";
    OnFetchError(MsgStr);
  }
}

void TWebPgFetchEvent::OnFetchEnd(){
  PSIn SIn=TMemIn::New(SockMem);
  PHttpResp HttpResp=THttpResp::LoadTxt(SIn);
  if (HttpResp->IsOk()){
    OnFetchEnd(HttpResp);
  } else {
    OnFetchError("Invalid Http Response");
  }
}

void TWebPgFetchEvent::OnRead(const int&, const PSIn& SIn){
  Sock->PutTimeOut(10*1000);
  SockMem+=SIn;
  if (!Fetch->IsOkContLen(SockMem.Len())){
    OnFetchEnd();
  }
}

void TWebPgFetchEvent::OnConnect(const int&){
  TChA RqChA;
  if (CurUrl->IsHttpRqStr()){
    RqChA=CurUrl->GetHttpRqStr();
  } else {
    // get http components
    TStr HostNm=CurUrl->GetHostNm();
    TStr AbsPath=CurUrl->GetPathStr()+CurUrl->GetSearchStr();
    // compose http request
    RqChA+="GET "; RqChA+=AbsPath; RqChA+=" HTTP/1.0\r\n";
    RqChA+="Host: "; RqChA+=HostNm; RqChA+="\r\n";
    // add cookies
    if (GetCookies()>0){
      RqChA+="Cookie: ";
      for (int CookieN=0; CookieN<GetCookies(); CookieN++){
        TStr KeyNm; TStr ValStr; TStr DmNm; TStr PathStr;
        GetCookie(CookieN, KeyNm, ValStr, DmNm, PathStr);
        if (HostNm.IsSuffix(DmNm)){
          if (CookieN>0){RqChA+="; ";}
          RqChA+=KeyNm; RqChA+='='; RqChA+=ValStr; 
        }
      }
      RqChA+="\r\n";
    }
    // finish request
    RqChA+="\r\n";
  }
  // send http request
  PSIn RqSIn=TMIn::New(RqChA);
  bool Ok; int ErrCd; Sock->Send(RqSIn, Ok, ErrCd);
  if (Ok){
    Sock->PutTimeOut(10*1000);
  } else {
    OnFetchError("Unable to send the data");
  }
}

void TWebPgFetchEvent::OnClose(const int& SockId){
  OppSockClosed=true;
  Sock->PutTimeOut(1);
}

void TWebPgFetchEvent::OnTimeOut(const int& SockId){
  if (OppSockClosed){
    OnFetchEnd();
  } else {
    Retries++;
    if (Retries<Fetch->GetMxRetries()){
      //**SaveToErrLog(TStr::Fmt(
      // "Retries: %d; TWebPgFetchEvent::OnTimeOut [%s%s%s]",
      // Retries, CurUrl->GetHostNm(), CurUrl->GetPathStr(),
      // CurUrl->GetSearchStr()).CStr());
      OnGetHost(SockHost);
    } else {
      OnFetchError("Timeout");
    }
  }
}

void TWebPgFetchEvent::OnError(const int&, const int&, const TStr& SockErrStr){
  Retries++;
  if (Retries<Fetch->GetMxRetries()){
    //**SaveToErrLog(TStr::Fmt(
    // "Retries: %d; TWebPgFetchEvent::OnError [%s%s%s]",
    // Retries, CurUrl->GetHostNm(), CurUrl->GetPathStr(),
    // CurUrl->GetSearchStr()).CStr());
    OnGetHost(SockHost);
  } else {
    OnFetchError(TStr("Socket Error (")+SockErrStr+")");
  }
}

void TWebPgFetchEvent::OnGetHost(const PSockHost& _SockHost){
  if (_SockHost->IsOk()){
    SockHost=_SockHost;
    TStr HostNm=SockHost->GetHostNm();
    UrlStrV.Add(CurUrl->GetUrlStr());
    IpNumV.Add(SockHost->GetIpNum());
    OppSockClosed=false;
    SockMem.Clr();
    Sock=TSock::New(this);
    int PortN;
    if (ProxyStr.Empty()){
      PortN=CurUrl->GetPortN();
    } else {
      TStr ProxyHostNm; TStr PortNStr;
      ProxyStr.SplitOnCh(ProxyHostNm, ':', PortNStr);
      PortN=PortNStr.GetInt(80);
    }
    Sock->Connect(SockHost, PortN);
    Sock->PutTimeOut(10*1000);
  } else
  if (_SockHost->GetStatus()==shsTryAgain){
    Retries++;
    if (Retries<Fetch->GetMxRetries()){
      //**SaveToErrLog(TStr::Fmt(
      // "Retries: %d; TWebPgFetchEvent::OnGetHost [%s%s%s]",
      // Retries, CurUrl->GetHostNm(), CurUrl->GetPathStr(),
      // CurUrl->GetSearchStr()).CStr());
      TStr HostNm;
      if (ProxyStr.Empty()){HostNm=CurUrl->GetIpNumOrHostNm();}
      else {TStr PortNStr; ProxyStr.SplitOnCh(HostNm, ':', PortNStr);}
      TSockHost::GetAsyncSockHost(HostNm, this);
    } else {
      OnFetchError("Can't get host info");
    }
  } else {
    OnFetchError("Invalid Host");
  }
}

/////////////////////////////////////////////////
// Web-Page-Fetch
void TWebPgFetch::PushWait(const int& FId, const PUrl& Url){
  WaitFIdUrlPrQ.Push(TIdUrlPr(FId, Url));
}

void TWebPgFetch::PopWait(int& FId, PUrl& Url){
  FId=WaitFIdUrlPrQ.Top().Val1;
  Url=WaitFIdUrlPrQ.Top().Val2;
  WaitFIdUrlPrQ.Pop();
}

void TWebPgFetch::OpenConn(const int& FId, const PUrl& Url){
  PSockEvent Event=PSockEvent(new TWebPgFetchEvent(this, FId, Url, ProxyStr));
  TSockEvent::Reg(Event);
  ConnFIdToEventH.AddDat(FId, Event);
  TStr HostNm;
  if (ProxyStr.Empty()){HostNm=Url->GetIpNumOrHostNm();}
  else {TStr PortNStr; ProxyStr.SplitOnCh(HostNm, ':', PortNStr);}
  TSockHost::GetAsyncSockHost(HostNm, Event);
}

void TWebPgFetch::CloseConn(const int& FId){
  PSockEvent Event=ConnFIdToEventH.GetDat(FId);
  TSockEvent::UnReg(Event);
  ConnFIdToEventH.DelKey(FId);
  Event->CloseConn();
  LastDelEvent=Event;
}

void TWebPgFetch::ConnUrl(const int& FId, const PUrl& Url){
  if (FId!=-1){PushWait(FId, Url);}
  while ((IsOkConns(GetConnUrls()))&&(GetWaitUrls()>0)){
    int FId; PUrl Url; PopWait(FId, Url);
    OpenConn(FId, Url);
  }
}

void TWebPgFetch::DisconnUrl(const int& FId){
  CloseConn(FId);
  ConnUrl();
}

TWebPgFetch::~TWebPgFetch(){
  TIntV ConnFIdV; ConnFIdToEventH.GetKeyV(ConnFIdV);
  for (int ConnFIdN=0; ConnFIdN<ConnFIdV.Len(); ConnFIdN++){
    CloseConn(ConnFIdV[ConnFIdN]);
  }
}


int TWebPgFetch::FetchUrl(const PUrl& Url){
  int FId=-1;
  if (Url->IsOk(usHttp)){
    FId=GetNextFId();
    ConnUrl(FId, Url);
  } else {
    TStr MsgStr=TStr("Invalid URL [")+Url->GetUrlStr()+"]";
    OnError(FId, MsgStr);
  }
  return FId;
}

int TWebPgFetch::FetchUrl(const TStr& RelUrlStr, const TStr& BaseUrlStr){
  PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
  return FetchUrl(Url);
}

int TWebPgFetch::FetchHttpRq(const PHttpRq& HttpRq){
  PUrl Url=HttpRq->GetUrl();
  Url->PutHttpRqStr(HttpRq->GetStr()); //**
  return FetchUrl(Url);
}

int TWebPgFetch::EnqueueUrl(const TStr& RelUrlStr, const TStr& BaseUrlStr){
  int FId=-1;
  PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
  if (Url->IsOk(usHttp)){
    FId=GetNextFId();
    PushWait(FId, Url);
  } else {
    TStr MsgStr=TStr("Invalid URL [")+RelUrlStr+"] + ["+BaseUrlStr+"]";
    OnError(FId, MsgStr);
  }
  return FId;
}

/////////////////////////////////////////////////
// Web-Fetch-Blocking
void TWebFetchBlocking::GetWebPg(const PUrl& Url, bool& Ok, TStr& MsgStr,
 PWebPg& WebPg, const PNotify& Notify, const TStr& ProxyStr){
  TNotify::OnNotify(Notify, ntInfo, TStr("Fetching: ")+Url->GetUrlStr());
  TWebFetchBlocking WebFetch;
  WebFetch.PutProxyStr(ProxyStr);
  WebFetch.FetchUrl(Url);
  TSysMsg::Loop();
  Ok=WebFetch.Ok; MsgStr=WebFetch.MsgStr; WebPg=WebFetch.WebPg;
  if (!Ok){
    TNotify::OnNotify(Notify, ntInfo, TStr("Fetching Error: ["+MsgStr+"]"));
  }
}

void TWebFetchBlocking::GetWebPg(const TStr& UrlStr, bool& Ok, TStr& MsgStr,
 PWebPg& WebPg, const PNotify& Notify, const TStr& ProxyStr){
  TNotify::OnNotify(Notify, ntInfo, TStr("Fetching: ")+UrlStr);
  TWebFetchBlocking WebFetch;
  WebFetch.PutProxyStr(ProxyStr);
  WebFetch.FetchUrl(UrlStr);
  TSysMsg::Loop();
  Ok=WebFetch.Ok; MsgStr=WebFetch.MsgStr; WebPg=WebFetch.WebPg;
  if (!Ok){
    TNotify::OnNotify(Notify, ntInfo, TStr("Fetching Error: ["+MsgStr+"]"));
  }
}

/////////////////////////////////////////////////
// Web-Page-To-File-Fetch
void TWebPgToFileFetch::OnFetch(const int&, const PWebPg& WebPg){
  // save web-page as http
  if (!OutHttpFNm.Empty()){
    WebPg->SaveAsHttp(OutHttpFNm);}
  // save http-body
  if (!OutHttpBodyFNm.Empty()){
    WebPg->SaveAsHttpBody(OutHttpBodyFNm);}
  // save web-page as xml
  if (!OutXmlFNm.Empty()){
    TStr HtmlStr=WebPg->GetHttpBodyAsStr();
    THtmlDoc::SaveHtmlToXml(HtmlStr, OutXmlFNm, WebPg->GetUrlStr(),
     OutXmlTextP, OutXmlUrlP, OutXmlToksP, OutXmlTagsP, OutXmlArgsP);
  }
  // save web-page as text
  if (!OutTxtFNm.Empty()){
    TStr HtmlStr=WebPg->GetHttpBodyAsStr();
    THtmlDoc::SaveHtmlToTxt(HtmlStr, OutTxtFNm, WebPg->GetUrlStr(),
     OutTxtUrlP, OutXmlTagsP);
  }
  // output to screen
  if (OutScrP){
    printf("%s\n", WebPg->GetUrlStr().CStr());
    printf("-----------------------\n");
    printf("%s", WebPg->GetHttpHdStr().CStr());
    printf("%s", WebPg->GetHttpBodyAsStr().CStr());
  }
  // send quit message
  if (QuitMsgP){
    TSysMsg::Quit();}
}

void TWebPgToFileFetch::OnError(const int&, const TStr& MsgStr){
  // save error-message as http-body
  if (!OutHttpBodyFNm.Empty()){
    TFOut SOut(OutHttpBodyFNm); TFileId SOutFId=SOut.GetFileId();
    fprintf(SOutFId, "Error: %s\n", MsgStr.CStr());
  }
  // save error-message as http
  if (!OutHttpFNm.Empty()){
    TFOut SOut(OutHttpFNm); TFileId SOutFId=SOut.GetFileId();
    fprintf(SOutFId, "Error: %s\n", MsgStr.CStr());
  }
  // save error-message as xml
  if (!OutXmlFNm.Empty()){
    // create output file
    TFOut SOut(OutXmlFNm); TFileId SOutFId=SOut.GetFileId();
    // open top tag
    fprintf(SOutFId, "<WebPage>\n");
    // save error message
    TStr XmlMsgStr=TXmlLx::GetXmlStrFromPlainStr(MsgStr);
    fprintf(SOutFId, "  <Message>%s</Message>\n", XmlMsgStr.CStr());
    // close top tag
    fprintf(SOutFId, "</WebPage>\n");
  }
  // output to screen
  if (OutScrP){
    printf("%s\n", MsgStr.CStr());
  }
  // send quit message
  if (QuitMsgP){
    TSysMsg::Quit();}
}

