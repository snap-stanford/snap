/////////////////////////////////////////////////
// Includes
#include "webold.h"
#include "webmb.h"

/////////////////////////////////////////////////
// Web-Page
const char TXWebPg::SepCh='#';

TXWebPg::TXWebPg(const PSIn& SIn):
  UrlStrV(), RefUrlStrV(), HttpResp(){
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum);
  Lx.GetSym(syQStr, syEoln);
  while (Lx.Sym==syQStr){UrlStrV.Add(Lx.Str); Lx.GetSym(syQStr, syEoln);}
  Lx.GetSym(syQStr, syEoln);
  while (Lx.Sym==syQStr){RefUrlStrV.Add(Lx.Str); Lx.GetSym(syQStr, syEoln);}
  PSIn HttpRespSIn=Lx.GetSIn(SepCh);
  HttpResp=THttpResp::LoadTxt(HttpRespSIn);
}

bool TXWebPg::IsText(){
  if ((!HttpResp->IsContType())||HttpResp->IsContType("text/")){
    TStr Str=HttpResp->GetBodyAsStr();
    int StrLen=Str.Len(); int ChN=0; int PrintChs=0;
    while ((ChN<100)&&(ChN<StrLen)){
      char Ch=Str[ChN++];
      if (((' '<=Ch)&&(Ch<='~'))||(Ch==TCh::TabCh)||(Ch==TCh::LfCh)||(Ch==TCh::CrCh)){
        PrintChs++;}
    }
    double PrintPrc=double(PrintChs)/double(ChN+1);
    return PrintPrc>0.9;
  } else {
    return false;
  }
}

PXWebPg TXWebPg::LoadTxt(const PSIn& SIn, const TStrV& UrlStrV,
 const TStrV& RefUrlStrV){
  PXWebPg WebPg=PXWebPg(new TXWebPg(SIn));
  WebPg->UrlStrV=UrlStrV; WebPg->RefUrlStrV=RefUrlStrV;
  return WebPg;
}

void TXWebPg::SaveTxt(const PSOut& SOut){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  for (int UrlStrN=0; UrlStrN<UrlStrV.Len(); UrlStrN++){
    Lx.PutQStr(UrlStrV[UrlStrN]);}
  Lx.PutLn();
  for (int RefUrlStrN=0; RefUrlStrN<RefUrlStrV.Len(); RefUrlStrN++){
    Lx.PutQStr(RefUrlStrV[RefUrlStrN]);}
  Lx.PutLn();
  HttpResp->SaveTxt(Lx.GetSOut(SepCh));
}

/////////////////////////////////////////////////
// Web-Fetch-Socket-Event
class TXWebFetchSockEvent: public TSockEvent{
private:
  static const int MxGetHostTrys;
  static const int MxFetchTrys;
  TXWebFetch* Fetch;
  TInt WebPgId;
  PUrl WebPgUrl;
  TUrlV WebPgUrlV;
  int MxSize;
  int GetHostTrys;
  int FetchTrys;
  PSockHost SockHost;
  PSock Sock;
  TBool OppSockClosed;
  TMOut SockMOut;
public:
  TXWebFetchSockEvent(TXWebFetch* _Fetch, const int& _WebPgId,
   const PUrl& _WebPgUrl, const int& _MxSize=-1):
    TSockEvent(),
    Fetch(_Fetch), WebPgId(_WebPgId),
    WebPgUrl(_WebPgUrl), WebPgUrlV(),
    MxSize(_MxSize), GetHostTrys(0), FetchTrys(0),
    SockHost(NULL), Sock(NULL),
    OppSockClosed(true), SockMOut(10000){}
  void PreDelete(){SockHost=NULL; Sock=NULL;}
  ~TXWebFetchSockEvent(){}

  TSockEvent& operator=(const TSockEvent&){Fail; return *this;}

  void OnWebFetchError(const TStr& ErrStr);
  void OnWebFetchEnd(const PHttpResp& HttpResp);

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int&);
  void OnClose(const int&);
  void OnTimeOut(const int&);
  void OnError(const int&, const int&, const TStr&);
  void OnGetHost(const PSockHost& _SockHost);

  TStr GetMsg(){
    return TInt::GetStr(WebPgId)+" "+WebPgUrlV[0]->GetUrlStr();}
};
const int TXWebFetchSockEvent::MxGetHostTrys=3;
const int TXWebFetchSockEvent::MxFetchTrys=3;

void TXWebFetchSockEvent::OnWebFetchError(const TStr& ErrStr){
  TStr UrlStr=WebPgUrl->GetUrlStr();
  Fetch->UnconnWebPg(WebPgId);
  Fetch->OnError(WebPgId, ErrStr+" ["+UrlStr+"]");
  PreDelete();
}

void TXWebFetchSockEvent::OnWebFetchEnd(const PHttpResp& HttpResp){
  IAssert(HttpResp->IsOk());
  int StatusCd=HttpResp->GetStatusCd();
  if (StatusCd/100==2){
    TStrV UrlStrV; TStrV RefUrlStrV;
    for (int UrlN=0; UrlN<WebPgUrlV.Len(); UrlN++){
      UrlStrV.Add(WebPgUrlV[UrlN]->GetUrlStr());
      if (WebPgUrlV[UrlN]->IsBaseUrl()){
        RefUrlStrV.AddMerged(WebPgUrlV[UrlN]->GetBaseUrlStr());}
    }
    PXWebPg WebPg=new TXWebPg(UrlStrV, RefUrlStrV, HttpResp);
    Fetch->UnconnWebPg(WebPgId);
    Fetch->OnFetch(WebPgId, WebPg);
    Fetch->OnStatus(Fetch->GetStatusStr());
    PreDelete();
  } else
  if (StatusCd/100==3){
    if (WebPgUrlV.Len()<5){
      TStr RedirUrlStr=HttpResp->GetFldVal("Location");
      PUrl RedirUrl=new TUrl(RedirUrlStr);
      if (RedirUrl->IsOk(usHttp)){
        WebPgUrl=RedirUrl; GetHostTrys=0; FetchTrys=0;
        SockHost=NULL; Sock=NULL;
        if (Fetch->DoFetchRedirUrl(WebPgUrl, WebPgUrlV)){
          TSockHost::GetAsyncSockHost(WebPgUrl->GetHostNm(), this);
        } else {
          Fetch->OnNotify(WebPgId,
           TStr("Redirection already fetched [")+WebPgUrl->GetUrlStr()+"]");
          Fetch->UnconnWebPg(WebPgId);
        }
      } else {
        OnWebFetchError(TStr("Invalid Redirection URL (")+RedirUrlStr+")");
      }
    } else {
      OnWebFetchError("Cycling Redirection");
    }
  } else {
    OnWebFetchError(TStr("Http Error (")+HttpResp->GetReasonPhrase()+")");
  }
}

void TXWebFetchSockEvent::OnRead(const int&, const PSIn& SIn){
  SockMOut.Save(*SIn);
  if ((MxSize!=-1)&&(SockMOut.Len()>MxSize)){
    OnWebFetchError(TStr("Page too big (max. ")+TInt::GetStr(MxSize)+")");
  }
//  else
//  if (SockMOut.Len()<20000){
//    PHttpResp HttpResp=THttpResp::LoadTxt(SockMOut.GetSIn(false));
//    if (HttpResp->IsContLenOk()){OnWebFetchEnd(HttpResp);}
//  }
}

void TXWebFetchSockEvent::OnConnect(const int&){
  TStr AbsPath=WebPgUrl->GetPathStr()+WebPgUrl->GetSearchStr();
  TStr RqStr=TStr("GET ")+AbsPath+" HTTP/1.0\r\n\r\n";
  Sock->Send(TStrIn::New(RqStr));
  Sock->PutTimeOut(10000);
}

void TXWebFetchSockEvent::OnClose(const int&){
  OppSockClosed=true;
  Sock->PutTimeOut(1);
}

void TXWebFetchSockEvent::OnTimeOut(const int&){
  if (OppSockClosed){
    PSIn SIn=SockMOut.GetSIn();
    PHttpResp HttpResp=THttpResp::LoadTxt(SIn);
    if (HttpResp->IsOk()){
      OnWebFetchEnd(HttpResp);
    } else {
      OnWebFetchError("Invalid Http Response");
    }
  } else {
    FetchTrys++;
    if (FetchTrys<MxFetchTrys){
      OnGetHost(SockHost);
    } else {
      OnWebFetchError("Timeout");
    }
  }
}

void TXWebFetchSockEvent::OnError(const int&, const int&, const TStr& SockErrStr){
  FetchTrys++;
  if (FetchTrys<MxFetchTrys){
    OnGetHost(SockHost);
  } else {
    OnWebFetchError(TStr("Socket Error (")+SockErrStr+")");
  }
}

void TXWebFetchSockEvent::OnGetHost(const PSockHost& _SockHost){
  if (_SockHost->IsOk()){
    SockHost=_SockHost;
    WebPgUrlV.Add(WebPgUrl);
    OppSockClosed=false;
    SockMOut.Clr();
    Sock=PSock(new TSock(this));
    Sock->Connect(SockHost, WebPgUrl->GetPortN());
    Sock->PutTimeOut(10000);
  } else
  if (_SockHost->GetStatus()==shsTryAgain){
    GetHostTrys++;
    if (GetHostTrys<MxGetHostTrys){
      TSockHost::GetAsyncSockHost(WebPgUrl->GetHostNm(), this);
    } else {
      OnWebFetchError("Can't get host info");
    }
  } else {
    OnWebFetchError("Invalid Host");
  }
}

/////////////////////////////////////////////////
// Web-Fetch-Report-Event
class TXWebFetchReportEvent: public TReportEvent{
private:
  TXWebFetch* Fetch;
  int WebPgId;
  PUrl WebPgUrl;
public:
  TXWebFetchReportEvent(TXWebFetch* _WebFetch, const int& _WebPgId,
   const PUrl& _WebPgUrl):
    TReportEvent(),
    Fetch(_WebFetch), WebPgId(_WebPgId), WebPgUrl(_WebPgUrl){}
  ~TXWebFetchReportEvent(){}

  TXWebFetchReportEvent& operator=(const TXWebFetchReportEvent&){
    Fail; return *this;}

  void OnReport(){
    PXWebPg WebPg=Fetch->GetWebPg(WebPgUrl->GetUrlStr());
    Fetch->UnconnWebPg(WebPgId);
    Fetch->OnFetch(WebPgId, WebPg);
    Fetch->OnStatus(Fetch->GetStatusStr());
    Fetch->Continue();
  }
};

/////////////////////////////////////////////////
// Web-Fetch
const TStr TXWebFetch::WebFetchFNm="WebFetch.Txt";
const int TXWebFetch::ExpectedUnconns=10000;
const int TXWebFetch::ExpectedConns=1000;

TXWebFetch::TXWebFetch(const TStr& FPath):
  MxConns(), MxSize(), LastWebPgId(),
  UnconnWebPgIdToUrlStrQ(ExpectedUnconns),
  ConnWebPgIdToUrlStrH(ExpectedConns),
  ConnWebPgUrlStrToIdH(ExpectedConns),
  ConnWebPgIdToSockEventH(ExpectedConns){
  PSIn SIn=new TFIn(TStr::GetNrFPath(FPath)+WebFetchFNm);
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum);
  MxConns=Lx.GetVarInt("MxConns");
  MxSize=Lx.GetVarInt("MxSize");
  LastWebPgId=Lx.GetVarInt("LastWebPageId");
  int Unconns=Lx.GetVarInt("UnconnWebPageIdToUrlStrQ");
  for (int UnconnN=0; UnconnN<Unconns; UnconnN++){
    int WebPgId=Lx.GetInt(); TStr UrlStr=Lx.GetQStr(); Lx.GetEoln();
    UnconnWebPgIdToUrlStrQ.Push(TIntStrPr(WebPgId, UrlStr));
  }
}

void TXWebFetch::SaveTxt(const TStr& FPath){
  PSOut SOut=new TFOut(TStr::GetNrFPath(FPath)+WebFetchFNm);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  Lx.PutVarInt("MxConns", MxConns);
  Lx.PutVarInt("MxSize", MxSize);
  Lx.PutVarInt("LastWebPageId", LastWebPgId);
  Lx.PutVarInt("UnconnWebPageIdToUrlStrQ",
    ConnWebPgIdToUrlStrH.Len()+UnconnWebPgIdToUrlStrQ.Len());
  {TIntStrH& H=ConnWebPgIdToUrlStrH;
  int P=H.FFirstKeyId();
  while (H.FNextKeyId(P)){
    int WebPgId=H.GetKey(P); TStr UrlStr=H[P];
    Lx.PutInt(WebPgId); Lx.PutQStr(UrlStr); Lx.PutLn();
  }}
  {TQQueue<TIntStrPr>& Q=UnconnWebPgIdToUrlStrQ;
  for (int UnconnN=0; UnconnN<Q.Len(); UnconnN++){
    int WebPgId=Q[UnconnN].Val1; TStr UrlStr=Q[UnconnN].Val2;
    Lx.PutInt(WebPgId); Lx.PutQStr(UrlStr); Lx.PutLn();
  }}
}

TXWebFetch::~TXWebFetch(){
  THash<TInt, PSockEvent>& H=ConnWebPgIdToSockEventH;
  int P=H.FFirstKeyId();
  while (H.FNextKeyId(P)){
    int WebPgId=H.GetKey(P);
    PSockEvent SockEvent=H.GetDat(WebPgId);
    H.GetDat(WebPgId)=NULL;
//    SockEvent->PreDelete();
  }
}

void TXWebFetch::ConnWebPg(const int& WebPgId, const TStr& UrlStr){
  if (WebPgId!=-1){
    UnconnWebPgIdToUrlStrQ.Push(TIntStrPr(WebPgId, UrlStr));}
  while ((ConnWebPgIdToUrlStrH.Len()<MxConns)&&
   (UnconnWebPgIdToUrlStrQ.Len()>0)){
    int WebPgId=UnconnWebPgIdToUrlStrQ.Top().Val1;
    TStr WebPgUrlStr=UnconnWebPgIdToUrlStrQ.Top().Val2;
    UnconnWebPgIdToUrlStrQ.Pop();
    if ((!ConnWebPgUrlStrToIdH.IsKey(WebPgUrlStr))&&
     DoStillFetchUrl(WebPgUrlStr)){
      ConnWebPgIdToUrlStrH.AddDat(WebPgId, WebPgUrlStr);
      ConnWebPgUrlStrToIdH.AddDat(WebPgUrlStr, WebPgId);
      PUrl WebPgUrl=new TUrl(WebPgUrlStr);
      IAssert(WebPgUrl->IsOk());
      if (IsCheckMode()&&IsWebPg(WebPgUrl->GetUrlStr())){
        PReportEvent WebFetchReportEvent=
         new TXWebFetchReportEvent(this, WebPgId, WebPgUrl);
        WebFetchReportEvent->SendReport();
      } else {
        PSockEvent SockEvent=PSockEvent(new
         TXWebFetchSockEvent(this, WebPgId, WebPgUrl, MxSize));
        ConnWebPgIdToSockEventH.AddDat(WebPgId, SockEvent);
        TSockHost::GetAsyncSockHost(WebPgUrl->GetHostNm(), SockEvent);
      }
    }
  }
}

void TXWebFetch::UnconnWebPg(const int& WebPgId){
  if (WebPgId!=-1){
    TStr WebPgUrlStr=ConnWebPgIdToUrlStrH.GetDat(WebPgId);
    ConnWebPgIdToUrlStrH.DelKey(WebPgId);
    ConnWebPgUrlStrToIdH.DelKey(WebPgUrlStr);
    if (!IsCheckMode()){
      ConnWebPgIdToSockEventH.DelKey(WebPgId);}
  }
  ConnWebPg();
}

int TXWebFetch::FetchWebPg(const TStr& RelUrlStr, const TStr& BaseUrlStr){
  int WebPgId=-1;
  PUrl Url=new TUrl(RelUrlStr, BaseUrlStr);
  if (Url->IsOk(usHttp)){
    WebPgId=GetNextWebPgId();
    ConnWebPg(WebPgId, Url->GetUrlStr());
  } else {
    TStr ErrStr=TStr("Invalid URL [")+RelUrlStr+"] + ["+BaseUrlStr+"]";
    OnError(WebPgId, ErrStr);
  }
  return WebPgId;
}

/////////////////////////////////////////////////
// Web-Travel-Base-Event
class TXWebTravelBaseEvent: public TXWebBsEvent{
private:
  TXWebTravel* WebTravel;
public:
  TXWebTravelBaseEvent(TXWebTravel* _WebTravel): WebTravel(_WebTravel){}
  ~TXWebTravelBaseEvent(){}

  void OnWebPg(const int& WebPgId, const PXWebPg& WebPg, bool& DoSave){
    WebTravel->OnWebPg(WebPgId, WebPg, DoSave);}
  void OnNotify(const int& WebPgId, const TStr& MsgStr){
    WebTravel->OnNotify(WebPgId, MsgStr);}
  void OnError(const int& WebPgId, const TStr& ErrStr){
    WebTravel->OnError(WebPgId, ErrStr);}
  void OnStatus(const TStr& StatusStr){
    WebTravel->OnStatus(StatusStr);}
};

/////////////////////////////////////////////////
// Web-Base
const TStr TXWebBs::WebBsFNm="WebBs.Txt";

TXWebBs::TXWebBs(const TStr& _FPath, const bool& _CheckMode):
  TXWebFetch(_FPath),
  FPath(TStr::GetNrFPath(_FPath)),
  CheckMode(_CheckMode), GenRef(){
  PSIn SIn=new TFIn(FPath+WebBsFNm);
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum);
  TStr OldFPath=Lx.GetVarStr("FPath");
  GenRef=Lx.GetVarBool("GenRef");
}

void TXWebBs::SaveTxt(){
  TXWebFetch::SaveTxt(FPath);
  PSOut SOut=new TFOut(FPath+WebBsFNm);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  Lx.PutVarStr("FPath", FPath);
  Lx.PutVarBool("GenRef", GenRef);
}

TStr TXWebBs::GetStatusStr(){
  static const TStr ActiveStr="Active:";
  static const TStr QueueStr=" Queue:";
  static const TStr PgsStr=" Pages:";
  static const TStr CheckedStr=" Checked-Urls:";
  if (IsCheckMode()){
    return
     ActiveStr+TInt::GetStr(GetConns())+
     QueueStr+TInt::GetStr(GetUnconns())+
     PgsStr+TInt::GetStr(GetWebPgs())+
     CheckedStr+TInt::GetStr(GetCheckedUrls());
  } else {
    return
     ActiveStr+TInt::GetStr(GetConns())+
     QueueStr+TInt::GetStr(GetUnconns())+
     PgsStr+TInt::GetStr(GetWebPgs());
  }
}

/////////////////////////////////////////////////
// Web-Travel
void TXWebTravel::GenBadFExtH(){
  // audio formats
  AddBadFExt(".AIFF");
  AddBadFExt(".AU");
  AddBadFExt(".AVI");
  AddBadFExt(".MP2");
  AddBadFExt(".MP3");
  AddBadFExt(".MPG");
  AddBadFExt(".MPEG");
  AddBadFExt(".RA");
  AddBadFExt(".WAV");
  // image formats
  AddBadFExt(".BMP");
  AddBadFExt(".EPS");
  AddBadFExt(".GIF");
  AddBadFExt(".JPEG");
  AddBadFExt(".JPG");
  AddBadFExt(".TIF");
  AddBadFExt(".TIFF");
  AddBadFExt(".DVI");
  AddBadFExt(".PS");
  AddBadFExt(".PDF");
  // compresses formats
  AddBadFExt(".ARJ");
  AddBadFExt(".GZ");
  AddBadFExt(".RAR");
  AddBadFExt(".TAR");
  AddBadFExt(".TGZ");
  AddBadFExt(".UU");
  AddBadFExt(".UUE");
  AddBadFExt(".Z");
  AddBadFExt(".ZIP");
  // executable formats
  AddBadFExt(".COM");
  AddBadFExt(".EXE");
  // programming languages
  AddBadFExt(".H");
  AddBadFExt(".C");
  AddBadFExt(".CC");
  AddBadFExt(".CPP");
  AddBadFExt(".PAS");
  AddBadFExt(".LGO");
  AddBadFExt(".BAS");
  AddBadFExt(".JAVA");
  AddBadFExt(".CLASS");
  // general & unknown formats
  AddBadFExt(".BIN"); // general binary format
  AddBadFExt(".HQX"); // ???
}

void TXWebTravel::OnWebPg(const int&, const PXWebPg& WebPg, bool& DoSave){
  TStr UrlStr=WebPg->GetUrlStr();
  DoSave=(!WebBs->IsWebPg(UrlStr))&&(WebPg->IsText());
  bool DoParse=(!SingleFetch)&&
   (DoSave||!WebBs->IsWebPg(UrlStr, WebBs->IsCheckMode()));
  OnBusy(WebPg, DoSave);
  if (DoParse){
    PSIn SIn=TStrIn::New(WebPg->GetBodyStr());
    PHtmlDoc HtmlDoc=THtmlDoc::New(SIn, hdtHRef);
    TStr BaseUrlStr=WebPg->GetLastUrlStr(); TStr RelUrlStr;
    for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
      if (IsUrlTok(HtmlDoc->GetTok(TokN), RelUrlStr)){
        if (IsUrlOk(RelUrlStr, BaseUrlStr)){
          WebBs->AddWebPg(RelUrlStr, BaseUrlStr);
        }
      }
    }
  }
  OnFree();
}

TXWebTravel::TXWebTravel(const PXWebBs& _WebBs, const PXWebTravelEvent& _Event):
  BadFExtH(50), SingleFetch(true), HostPathStrV(),
  WebBsEvent(), WebBs(_WebBs), Event(_Event){
  GenBadFExtH();
  WebBsEvent=PXWebBsEvent(new TXWebTravelBaseEvent(this));
  if (!WebBs.Empty()){WebBs->PutWebBsEvent(WebBsEvent);}
}

bool TXWebTravel::IsUrlTok(const PHtmlTok& Tok, TStr& RelUrlStr){
  if (Tok->GetSym()==hsyBTag){
    TStr TagNm=Tok->GetStr();
    if ((TagNm==THtmlTok::ATagNm)&&(Tok->IsArg(THtmlTok::HRefArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::HRefArgNm); return true;}
    else if ((TagNm==THtmlTok::AreaTagNm)&&(Tok->IsArg(THtmlTok::HRefArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::HRefArgNm); return true;}
    else if ((TagNm==THtmlTok::FrameTagNm)&&(Tok->IsArg(THtmlTok::SrcArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::SrcArgNm); return true;
    }
  }
  return false;
}

bool TXWebTravel::IsUrlOk(const TStr& RelUrlStr, const TStr& BaseUrlStr){
  PUrl Url=new TUrl(RelUrlStr, BaseUrlStr);
  if (!Url->IsOk(usHttp)){
    OnNotify(TStr("Invalid URL [")+RelUrlStr+"] + ["+BaseUrlStr+"]");
    return false;
  }

  TStr FExt=Url->GetPathStr().GetFExt().GetUc();
  if (BadFExtH.IsKey(FExt)){return false;}

  if (HostPathStrV.Len()==0){return true;}
  for (int HostPathStrN=0; HostPathStrN<HostPathStrV.Len(); HostPathStrN++){
    TStr HostStr=HostPathStrV[HostPathStrN].Val1;
    TStr PathStr=HostPathStrV[HostPathStrN].Val2;
    if ((Url->IsInHost(HostStr))&&(Url->IsInPath(PathStr))){return true;}
  }
  return false;
}

