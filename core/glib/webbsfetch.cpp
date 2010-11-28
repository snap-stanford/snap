/////////////////////////////////////////////////
// Web-Filter
const int TWebFilter::MxMxConns=1000;
const int TWebFilter::MxMxConnsPerSrv=100;
const int TWebFilter::MxMxDocSize=100000000;
const int TWebFilter::MxMxPathSegs=100;
const TStr TWebFilter::FExt=".wbf";

void TWebFilter::AddBadFExt(const TStr& BadFExt){
  BadFExtH.AddKey(TStr::GetNrFExt(BadFExt.GetUc()));
}

void TWebFilter::GenDfBadFExtH(){
  // audio formats
  AddBadFExt(".AIFF");
  AddBadFExt(".AU");
  AddBadFExt(".AVI");
  AddBadFExt(".MID");
  AddBadFExt(".MOV");
  AddBadFExt(".MV");
  AddBadFExt(".MP2");
  AddBadFExt(".MP3");
  AddBadFExt(".M3U");
  AddBadFExt(".MPG");
  AddBadFExt(".MPEG");
  AddBadFExt(".WAV");
  // Real-Audio
  AddBadFExt(".RA");
  AddBadFExt(".RAM");
  AddBadFExt(".RM");
  AddBadFExt(".RMD");

  // image formats
  AddBadFExt(".BMP");
  AddBadFExt(".CDR");
  AddBadFExt(".DWG");
  AddBadFExt(".EPS");
  AddBadFExt(".GIF");
  AddBadFExt(".JPEG");
  AddBadFExt(".JPG");
  AddBadFExt(".TIF");
  AddBadFExt(".TIFF");
  AddBadFExt(".PCX");
  AddBadFExt(".WRL");
  AddBadFExt(".VRML");
  AddBadFExt(".VRL");
  AddBadFExt(".XBM");

  // compresses/archive/patch formats
  AddBadFExt(".ARJ");
  AddBadFExt(".CAB");
  AddBadFExt(".GZ");
  AddBadFExt(".LHA");
  AddBadFExt(".RAR");
  AddBadFExt(".TAR");
  AddBadFExt(".TGZ");
  AddBadFExt(".UU");
  AddBadFExt(".UUE");
  AddBadFExt(".Z");
  AddBadFExt(".ZIP");
  // Linux
  AddBadFExt(".DEB");
  AddBadFExt(".CHANGES");
  AddBadFExt(".1-M68K");
  AddBadFExt(".SIGN");
  AddBadFExt(".RPM");

  // executable formats
  AddBadFExt(".BAT"); // MS-DOS batch file
  AddBadFExt(".CGI"); // CGI script
  AddBadFExt(".CMD"); // Command file
  AddBadFExt(".COM"); // Command file/DOS Executable
  //AddBadFExt(".EXE"); // MS Executable
  AddBadFExt(".LIB"); // Library
  AddBadFExt(".OBJ"); // Object
  AddBadFExt(".DLL"); // Dynamic-Link-Library

  // document formats
  AddBadFExt(".DOC"); // MS-Word Document
  AddBadFExt(".DOT"); // MS-Word Document Template
  AddBadFExt(".XLS"); // MS-Excel Workbook
  AddBadFExt(".PPT"); // MS-Power-Point Presentation
  AddBadFExt(".PPS"); // MS-Power-Point Show
  AddBadFExt(".RTF"); // Rich-Text-Format
  AddBadFExt(".TEX"); // TeX Document
  AddBadFExt(".STY"); // TeX Style
  AddBadFExt(".DVI"); // TeX Device Independent file
  AddBadFExt(".DEF"); // TeX Definition
  AddBadFExt(".TOC"); // TeX Table-of-contents
  AddBadFExt(".IDX"); // TeX Index
  AddBadFExt(".IND"); // TeX Index
  AddBadFExt(".ILG"); // TeX Index Messages
  AddBadFExt(".AUX"); // TeX Auxilary
  AddBadFExt(".BBL"); // TeX Bibliography
  AddBadFExt(".DVI"); // TeX Device independent
  AddBadFExt(".PS"); // PostScript
  AddBadFExt(".PDF"); // Acrobat
  AddBadFExt(".HLP"); // Help file
  // SGML
  AddBadFExt(".DCL");
  AddBadFExt(".DTD");

  // programming languages
  // Assembler
  AddBadFExt(".ASM"); // Assembler source code
  AddBadFExt(".A"); // Assembler source code
  AddBadFExt(".LST"); // Assembler listing
  // Awk
  AddBadFExt(".AWK"); // AWK source code
  // C/C++
  AddBadFExt(".H"); // C/C++ header source code
  AddBadFExt(".C"); // C source code
  AddBadFExt(".CC"); // C/C++ source code
  AddBadFExt(".CPP"); // C++ source code
  AddBadFExt(".INC"); // include file
  AddBadFExt(".PRJ"); // Borland-C Project file
  AddBadFExt(".RES"); // Resource file
  // Pascal
  AddBadFExt(".PAS"); // Pascal source code
  AddBadFExt(".P"); // Pascal source code
  AddBadFExt(".DPR"); // Delphi-Project
  AddBadFExt(".DFM"); // Delphi-Form
  AddBadFExt(".DCU"); // Delphi-Unit
  AddBadFExt(".DSK"); // Delphi-Desktop
  AddBadFExt(".BGI"); // Delphi-Desktop
  AddBadFExt(".TPU"); // Turbo-Pascal Unit
  // Java
  AddBadFExt(".JAVA"); // Java source code
  AddBadFExt(".CLASS"); // Java Class file
  // Logo
  AddBadFExt(".LGO"); // Logo source code
  AddBadFExt(".LG");// Logo source code
  AddBadFExt(".LOG"); // Logo source code/Logging file
  // Prolog
  AddBadFExt(".PRO"); // Prolog source code
  AddBadFExt(".ARI"); // Prolog source code
  // Perl
  AddBadFExt(".PL"); // Perl/Prolog source code
  AddBadFExt(".PERL"); // Perl source code
  // other
  AddBadFExt(".COB"); // Cobol source code
  AddBadFExt(".CBL"); // Cobol source code
  AddBadFExt(".F"); // Fortran source code
  AddBadFExt(".FOR"); // Fortran source code
  AddBadFExt(".M"); // Mathematica source code
  AddBadFExt(".BAS"); // Basic source code
  AddBadFExt(".JS"); // JavaScript
  AddBadFExt(".SPS"); // SPSS
  AddBadFExt(".SH"); // shell
  AddBadFExt(".KSH"); // shell

  // general & data & unknown formats
  // data formats
  AddBadFExt(".BAK"); // backup file
  AddBadFExt(".$$$"); // backup file
  AddBadFExt(".DAT"); // general data file
  AddBadFExt(".OUT"); // output data
  AddBadFExt(".BIN"); // general binary format
  AddBadFExt(".IMG"); // general image binary format
  AddBadFExt(".MSG"); // general message
  AddBadFExt(".TMP"); // temporary file
  AddBadFExt(".PRN"); // print file
  // databeses
  AddBadFExt(".PX");
  AddBadFExt(".DB");
  AddBadFExt(".DBF");
  AddBadFExt(".MDB");
  // fonts
  AddBadFExt(".FON"); // general font
  AddBadFExt(".TTF"); // True-Type
  AddBadFExt(".AFM"); // Adobe
  // Vega (Tomo/Vlado)
  AddBadFExt(".IDB"); // Vega-Data file
  AddBadFExt(".VGR"); // Vega-Data file
  AddBadFExt(".VG2"); // Vega-Data file
  AddBadFExt(".NET"); // Vega-Data file
  // Multext-East
  AddBadFExt(".HASH");
  AddBadFExt(".WFL");
  AddBadFExt(".CS");
  AddBadFExt(".EL");
  AddBadFExt(".LEX");
  AddBadFExt(".TOK");
  AddBadFExt(".BLG");
  AddBadFExt(".RO");
  AddBadFExt(".HU");
  // other
  AddBadFExt(".TT"); // Genealogy
  AddBadFExt(".GED"); // Genealogy
  AddBadFExt(".INI"); // Configuration
  AddBadFExt(".CFG"); // Configuration
  AddBadFExt(".CONF"); // Configuration
  AddBadFExt(".CDF"); // Channel
  AddBadFExt(".ASC"); // PGP
  AddBadFExt(".DSC"); // ?PGP
  AddBadFExt(".INF"); // Windows config file
  AddBadFExt(".CIR"); // Spice-software
  AddBadFExt(".MOT"); // &&Macro-Assembler output
  // unknown
  AddBadFExt(".PUD");
  AddBadFExt(".UID");
  AddBadFExt(".PPZ");
  AddBadFExt(".HQX");
  AddBadFExt(".HLB");
  AddBadFExt(".FLI");
  AddBadFExt(".MCD");
  AddBadFExt(".SKR");
  AddBadFExt(".SCR");
}

bool TWebFilter::IsOkFExt(const TStr& FExt){
  return
   (!BadFExtH.IsKey(TStr::GetNrFExt(FExt.GetUc()))) &&
   (!FExt.IsChIn(';'));
}

bool TWebFilter::IsUrlTok(const PHtmlTok& Tok, TStr& RelUrlStr){
  if (Tok->GetSym()==hsyBTag){
    TStr TagNm=Tok->GetStr();
    if ((TagNm==THtmlTok::ATagNm)&&(Tok->IsArg(THtmlTok::HRefArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::HRefArgNm); return true;}
    else if ((TagNm==THtmlTok::AreaTagNm)&&(Tok->IsArg(THtmlTok::HRefArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::HRefArgNm); return true;}
    else if ((TagNm==THtmlTok::FrameTagNm)&&(Tok->IsArg(THtmlTok::SrcArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::SrcArgNm); return true;}
    else if ((TagNm==THtmlTok::ImgTagNm)&&(Tok->IsArg(THtmlTok::SrcArgNm))){
      RelUrlStr=Tok->GetArg(THtmlTok::SrcArgNm); return true;}
    else if ((TagNm==THtmlTok::MetaTagNm)&&(Tok->IsArg(THtmlTok::HttpEquivArgNm))){
      TStr HttpEquivArgVal=Tok->GetArg(THtmlTok::HttpEquivArgNm).GetUc();
      if (HttpEquivArgVal=="REFRESH"){
        TStr ContentStr=Tok->GetArg("CONTENT");
        TStr LeftStr; TStr RightStr; TStr UrlEqStr="URL=";
        ContentStr.GetUc().SplitOnStr(LeftStr, UrlEqStr, RightStr);
        RelUrlStr=ContentStr.GetSubStr(
         LeftStr.Len()+UrlEqStr.Len(), ContentStr.Len());
        return !RelUrlStr.Empty();
      } else {
        return false;
      }
    }
  }
  return false;
}

bool TWebFilter::IsUrlOk(const PUrl& Url){
  // url in the whole
  if (!Url->IsOk(usHttp)){return false;}
  // pass
  if (IsOkPassConstrUrl(Url->GetAsFinalUrlStr())){return true;}
  // scripts
  if (!IsOkDoAllowScripts(Url->GetAsFinalUrlStr())){return false;}
  // domain
  if (!IsOkDmConstr(Url->GetHostNm())){return false;}
  // path length
  if (Url->GetPathSegs()>int(MxPathSegs)){return false;}
  // file extension
  //if (!IsOkFExt(Url->GetPathStr().GetFExt())){return false;}
  // wildcard pattern on final url
  if (Url->IsDefFinalUrl()){
    return
     IsOkPosConstrUrl(Url->GetFinalUrlStr())&&
     IsOkNegConstrUrl(Url->GetFinalUrlStr());
  } else {
    return true;
  }
}

PWebFilter TWebFilter::LoadTxt(const PSIn& SIn){
  PWebFilter WebFilter=PWebFilter(new TWebFilter());
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloCsSens);
  Lx.GetVar("WebFilter", true, true);
  WebFilter->MxConns=Lx.GetVarInt("MxConns");
  WebFilter->MxConnsPerSrv=Lx.GetVarInt("MxConnsPerSrv");
  WebFilter->MxDocSize=Lx.GetVarInt("MxDocSize");
  WebFilter->MxPathSegs=Lx.GetVarInt("MxPathSegs");
  WebFilter->DoAllowScripts=Lx.GetVarBool("DoAllowScripts");
  Lx.GetVarStrV("StartUrlStrV", WebFilter->StartUrlStrV);
  Lx.GetVarStrV("DmConstrStrV", WebFilter->DmConstrStrV);
  Lx.GetVarStrV("PassConstrUrlStrV", WebFilter->PassConstrUrlStrV);
  Lx.GetVarStrV("PosConstrUrlStrV", WebFilter->PosConstrUrlStrV);
  Lx.GetVarStrV("NegConstrUrlStrV", WebFilter->NegConstrUrlStrV);
  Lx.GetVarEnd(true, true);
  return WebFilter;
}

void TWebFilter::SaveTxt(const PSOut& SOut){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens|oloVarIndent);
  Lx.PutVar("WebFilter", true, true);
  Lx.PutVarInt("MxConns", MxConns);
  Lx.PutVarInt("MxConnsPerSrv", MxConnsPerSrv);
  Lx.PutVarInt("MxDocSize", MxDocSize);
  Lx.PutVarInt("MxPathSegs", MxPathSegs);
  Lx.PutVarBool("DoAllowScripts", DoAllowScripts);
  Lx.PutVarStrV("StartUrlStrV", StartUrlStrV);
  Lx.PutVarStrV("DmConstrStrV", DmConstrStrV);
  Lx.PutVarStrV("PassConstrUrlStrV", PassConstrUrlStrV);
  Lx.PutVarStrV("PosConstrUrlStrV", PosConstrUrlStrV);
  Lx.PutVarStrV("NegConstrUrlStrV", NegConstrUrlStrV);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Web-Base-Fetch-Event
class TWebBsFetchEvent: public TSockEvent{
private:
  static const int MxRetries;
  TWebBsFetch* Fetch;
  TBlobPt UrlStrId;
  int FId;
  PUrl TopUrl;
  PUrl CurUrl;
  TStrV UrlStrV;
  TStrV IpNumV;
  TStrStrH CookieNmToValH;
  PSockHost SockHost;
  PSock Sock;
  int Retries;
  TBool OppSockClosed;
  TMOut SockMOut;
  void ChangeLastUrlToLc(const PHttpResp& HttpResp);
  void CloseConn(){SockHost.Clr(); Sock.Clr();}
public:
  TWebBsFetchEvent(TWebBsFetch* _WebBsFetch,
   const TBlobPt& _UrlStrId, const TInt& _FId, const PUrl& _Url);
  ~TWebBsFetchEvent(){CloseConn();}

  TSockEvent& operator=(const TSockEvent&){Fail; return *this;}

  void OnFetchError(const TStr& MsgStr);
  void OnFetchEnd(const PHttpResp& HttpResp);
  void OnFetchEnd();

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int&);
  void OnClose(const int&);
  void OnTimeOut(const int&);
  void OnError(const int&, const int&, const TStr&);
  void OnGetHost(const PSockHost& _SockHost);
};
const int TWebBsFetchEvent::MxRetries=5;

void TWebBsFetchEvent::ChangeLastUrlToLc(const PHttpResp& HttpResp){
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

TWebBsFetchEvent::TWebBsFetchEvent(TWebBsFetch* _Fetch,
 const TBlobPt& _UrlStrId, const TInt& _FId, const PUrl& _Url):
  TSockEvent(),
  Fetch(_Fetch), UrlStrId(_UrlStrId), FId(_FId),
  TopUrl(_Url), CurUrl(_Url), UrlStrV(), IpNumV(),
  SockHost(NULL), Sock(NULL),
  Retries(0), OppSockClosed(true),
  SockMOut(10000){}

void TWebBsFetchEvent::OnFetchError(const TStr& MsgStr){
  Fetch->DequeueUrl(UrlStrId, TopUrl);
  CloseConn();
  Fetch->OnError(FId, MsgStr+" ["+CurUrl->GetUrlStr()+"]");
  Fetch->OnStatus(Fetch->GetStatusStr());
}

void TWebBsFetchEvent::OnFetchEnd(const PHttpResp& HttpResp){
  IAssert(HttpResp->IsOk());
  int StatusCd=HttpResp->GetStatusCd();
  if (StatusCd/100==2){ // codes 2XX - ok
    ChangeLastUrlToLc(HttpResp);
    PWebPg WebPg=TWebPg::New(UrlStrV, IpNumV, HttpResp);
    Fetch->DequeueUrl(UrlStrId, TopUrl);
    Fetch->OnFetch(FId, WebPg);
    Fetch->OnInfo(FId, WebPg->GetUrlStr());
    Fetch->OnStatus(Fetch->GetStatusStr());
    CloseConn();
  } else
  if (StatusCd/100==3){ // codes 3XX - redirection
    ChangeLastUrlToLc(HttpResp);
    if (UrlStrV.Len()<5){
      TStr RedirUrlStr=HttpResp->GetFldVal(THttp::LocFldNm);
      PUrl RedirUrl=TUrl::New(RedirUrlStr, CurUrl->GetUrlStr());
      if (RedirUrl->IsOk(usHttp)){
        CurUrl=RedirUrl; Retries=0;
        CloseConn();
        Fetch->SetUrlAsFinal(CurUrl);
        if (Fetch->DoStillFetchUrl(CurUrl)){
          TSockHost::GetAsyncSockHost(CurUrl->GetIpNumOrHostNm(), this);
        } else {
          Fetch->OnInfo(FId,
           TStr("Redirection already fetched [")+CurUrl->GetUrlStr()+"]");
          Fetch->OnStatus(Fetch->GetStatusStr());
          Fetch->DequeueUrl(UrlStrId, TopUrl);
        }
      } else {
        OnFetchError(TStr("Invalid Redirection URL (")+RedirUrlStr+")");
      }
    } else {
      OnFetchError("Cycling Redirection");
    }
  } else { // all other codes - error
    TStr MsgStr=TStr("Http Error (")+HttpResp->GetReasonPhrase()+")";
    OnFetchError(MsgStr);
  }
}

void TWebBsFetchEvent::OnFetchEnd(){
  int MxDocSize=-1;
  if (!Fetch->WebFilter->IsOkDocSize(SockMOut.Len())){
    if (!Fetch->WebFilter->IsOkPassConstrUrl(CurUrl->GetFinalUrlStr())){
      MxDocSize=Fetch->WebFilter->GetMxDocSize();
    }
  }
  PSIn SIn=SockMOut.GetSIn(true, MxDocSize);
  PHttpResp HttpResp=THttpResp::LoadTxt(SIn);
  if (HttpResp->IsOk()){
    OnFetchEnd(HttpResp);
  } else {
    OnFetchError("Invalid Http Response");
  }
}

void TWebBsFetchEvent::OnRead(const int&, const PSIn& SIn){
  SockMOut.Save(*SIn);
  if (!Fetch->WebFilter->IsOkDocSize(SockMOut.Len())){
    if (!Fetch->WebFilter->IsOkPassConstrUrl(CurUrl->GetFinalUrlStr())){
      OnFetchEnd();
    }
  }
}

void TWebBsFetchEvent::OnConnect(const int&){
  // get http components
  TStr HostNm=CurUrl->GetHostNm();
  TStr AbsPath=CurUrl->GetPathStr()+CurUrl->GetSearchStr();
  // compose http request
  TChA RqChA;
  RqChA+="GET "; RqChA+=AbsPath; RqChA+=" HTTP/1.0\r\n";
  RqChA+="Host: "; RqChA+=HostNm; RqChA+="\r\n";
  RqChA+="\r\n";
  // send http request
  PSIn RqSIn=TMIn::New(RqChA);
  bool Ok; int ErrCd; Sock->Send(RqSIn, Ok, ErrCd);
  if (Ok){
    Sock->PutTimeOut(10000);
  } else {
    OnFetchError("Unable to send the data");
  }
}

void TWebBsFetchEvent::OnClose(const int&){
  OppSockClosed=true;
  Sock->PutTimeOut(1);
}

void TWebBsFetchEvent::OnTimeOut(const int&){
  if (OppSockClosed){
    OnFetchEnd();
  } else {
    Retries++;
    if (Retries<MxRetries){
      OnGetHost(SockHost);
    } else {
      OnFetchError("Timeout");
    }
  }
}

void TWebBsFetchEvent::OnError(const int&, const int&, const TStr& SockErrStr){
  Retries++;
  if (Retries<MxRetries){
    OnGetHost(SockHost);
  } else {
    OnFetchError(TStr("Socket Error (")+SockErrStr+")");
  }
}

void TWebBsFetchEvent::OnGetHost(const PSockHost& _SockHost){
  if (_SockHost->IsOk()){
    SockHost=_SockHost;
    TStr FinalHostNm=SockHost->GetHostNm();
    Fetch->SetUrlAsFinal(CurUrl);
    if (CurUrl->IsDefFinalUrl()){
      if (FinalHostNm!=CurUrl->GetFinalHostNm()){
        TChA MsgChA="Different final host names [";
        MsgChA+=FinalHostNm; MsgChA+="] [";
        MsgChA+=CurUrl->GetFinalHostNm(); MsgChA+="]";
        Fetch->OnError(FId, MsgChA);
      }
    } else {
      Fetch->DefFinalHostDesc(CurUrl->GetHostNm(), FinalHostNm);
      CurUrl->DefFinalUrl(FinalHostNm);
    }
    if (Fetch->DoStillFetchUrl(CurUrl)){
      UrlStrV.Add(CurUrl->GetFinalUrlStr());
      IpNumV.Add(SockHost->GetIpNum());
      OppSockClosed=false;
      SockMOut.Clr();
      Sock=TSock::New(this);
      Sock->Connect(SockHost, CurUrl->GetPortN());
      Sock->PutTimeOut(10000);
    } else {
      Fetch->DequeueUrl(UrlStrId, TopUrl);
      CloseConn();
      Fetch->OnError(FId, TStr("Bad host name [")+CurUrl->GetUrlStr()+"]");
      Fetch->OnStatus(Fetch->GetStatusStr());
    }
  } else
  if (_SockHost->GetStatus()==shsTryAgain){
    Retries++;
    if (Retries<MxRetries){
      TSockHost::GetAsyncSockHost(CurUrl->GetIpNumOrHostNm(), this);
    } else {
      OnFetchError("Can't get host info");
    }
  } else {
    OnFetchError("Invalid Host");
  }
}

/////////////////////////////////////////////////
// Web-Alias-Final-Host-Name-Hash
PWebHostNmBs TWebHostNmBs::LoadTxt(const PSIn& SIn){
  // load string-pair-vector
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloCsSens);
  TStrPrV HostNmPrV;
  Lx.GetVarStrPrV("HostNmPrV", HostNmPrV);
  // transform string-pair-vector to hash table
  PWebHostNmBs WebHostNmBs=PWebHostNmBs(new TWebHostNmBs());
  for (int HostNmPrN=0; HostNmPrN<HostNmPrV.Len(); HostNmPrN++){
    TStr AliasNm=HostNmPrV[HostNmPrN].Val1;
    TStr FinalNm=HostNmPrV[HostNmPrN].Val2;
    WebHostNmBs->AddAliasToFinalNm(AliasNm, FinalNm);
  }
  return WebHostNmBs;
}

void TWebHostNmBs::SaveTxt(const PSOut& SOut) const {
  // transform hash table to string-pair-vector
  TStrPrV HostNmPrV(AliasToFinalNmH.Len(), 0);
  int HostNmP=AliasToFinalNmH.FFirstKeyId();
  while (AliasToFinalNmH.FNextKeyId(HostNmP)){
    TStr AliasNm=AliasToFinalNmH.GetKey(HostNmP);
    TStr FinalNm=AliasToFinalNmH[HostNmP];
    HostNmPrV.Add(TStrPr(AliasNm, FinalNm));
  }
  // save string-pair-vector
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens|oloVarIndent);
  Lx.PutVarStrPrV("HostNmPrV", HostNmPrV);
}

/////////////////////////////////////////////////
// Web-Fetch
const TStr TWebBsFetch::WebBsFetchFExt=".wfm";
const TStr TWebBsFetch::WebBsFetchUrlFExt=".wfu";
const TStr TWebBsFetch::WebHostBsFExt=".whb";
const int TWebBsFetch::EstWebPgs=10000;
const int TWebBsFetch::EstHostNms=10000;
const int TWebBsFetch::UrlStrHCacheSize=10000000;

void TWebBsFetch::DefFinalHostDesc(
 const TStr& AliasHostNm, const TStr& FinalHostNm){
  TWebBsFetchHostDesc& AliasHostDesc=HostNmToDescH.GetDat(AliasHostNm);
  IAssert(!AliasHostDesc.IsDefFinalHostNm());
  AliasHostDesc.PutFinalHostNm(FinalHostNm);
  if (AliasHostNm!=FinalHostNm){
    TWebBsFetchHostDesc& FinalHostDesc=HostNmToDescH.AddDat(FinalHostNm);
    if (!FinalHostDesc.IsDefFinalHostNm()){
      FinalHostDesc.PutFinalHostNm(FinalHostNm);
    } else {
      IAssert(FinalHostNm==FinalHostDesc.GetFinalHostNm());
    }
    FinalHostDesc.AddConns(AliasHostDesc.GetConns());
    AliasHostDesc.UndefConns();
  }
}

TWebBsFetchHostDesc& TWebBsFetch::GetAsFinalHostDesc(const PUrl& Url){
  SetUrlAsFinal(Url);
  TStr HostNm=Url->GetHostNm();
  if (Url->IsDefFinalUrl()){
    // final host name is known
    TStr FinalHostNm=Url->GetFinalHostNm(); int HostId;
    if (HostNmToDescH.IsKey(FinalHostNm, HostId)){
      // return final host description (most common case)
      return HostNmToDescH[HostId];
    } else {
      // define alias host-description
      HostNmToDescH.AddDat(HostNm).PutFinalHostNm(FinalHostNm);
      // define final host-description
      TWebBsFetchHostDesc& HostDesc=HostNmToDescH.AddDat(FinalHostNm);
      HostDesc.PutFinalHostNm(FinalHostNm);
      return HostDesc;
    }
  } else {
    // final host name is still unknown - define alias host-description
    return HostNmToDescH.AddDat(HostNm);
  }
}

void TWebBsFetch::EnqueueUrl(const int& FId, const PUrl& Url){
  // check if url already exists
  TStr UrlStr=Url->GetUrlStr();
  TBlobPt UrlStrId; UrlStrToFIdH->IsKey(UrlStr, UrlStrId);
  // check if final url already exists
  TBlobPt FinalUrlStrId;
  if (SetUrlAsFinal(Url)){
    TStr FinalUrlStr=Url->GetFinalUrlStr();
    UrlStrToFIdH->IsKey(FinalUrlStr, FinalUrlStrId);
  }
  if ((UrlStrId.Empty())&&(FinalUrlStrId.Empty())){
    // register url
    UrlStrId=UrlStrToFIdH->AddFDat(UrlStr, FId);
    // add url to the queue
    UrlStrIdQ.Push(UrlStrId);
  }
}

void TWebBsFetch::DequeueUrl(const TBlobPt& UrlStrId, const PUrl& Url){
  // delete from url
  UrlStrToFIdH->DelKeyId(UrlStrId);
  // decrement connections per host
  TWebBsFetchHostDesc& HostDesc=GetAsFinalHostDesc(Url);
  HostDesc.DecConns();
  // delete socket event
  PSockEvent SockEvent=UrlStrIdToSockEventH.GetDat(UrlStrId);
  TSockEvent::UnReg(SockEvent);
  UrlStrIdToSockEventH.DelKey(UrlStrId);
  // make new connections
  GoFetch();
  if (GetConns()==0){OnInfo("No Connections.");}
}

int TWebBsFetch::FetchUrl(const PUrl& Url, const bool& GoFetching){
  int FId=-1;
  if (Url->IsOk(usHttp)){
    FId=GetNextFId();
    EnqueueUrl(FId, Url);
    if (GoFetching){GoFetch();}
  } else {
    TStr MsgStr=TStr("Invalid URL [")+
     Url->GetRelUrlStr()+"] + ["+Url->GetBaseUrlStr()+"]";
    OnError(FId, MsgStr);
  }
  return FId;
}

bool TWebBsFetch::DoStillFetchUrl(const PUrl& Url){
  if (!Url->IsDefFinalUrl()){return true;}
  return WebFilter->IsUrlOk(Url)&&(!WebBs->IsUrlStr(Url->GetFinalUrlStr()));
}

TWebBsFetch::TWebBsFetch(
 const TStr& Nm, const TStr& FPath, const TFAccess& _Access, TWebBs* _WebBs,
 const PWebFilter& _WebFilter, const bool& _IndexTxtBsP):
  WebBs(_WebBs), WebBsFetchFNm(), WebHostBsFNm(), Access(_Access),
  LastFId(-1), WebFilter(_WebFilter), IndexTxtBsP(_IndexTxtBsP),
  UrlStrToFIdH(),
  UrlStrIdQ(EstWebPgs),
  UrlStrIdToSockEventH(TWebFilter::MxMxConns),
  HostNmToDescH(EstHostNms){
  // prepare file-names
  TStr WebBsFetchUrlFNm;
  GetFNms(Nm, FPath, WebBsFetchFNm, WebBsFetchUrlFNm, WebHostBsFNm);

  // prepare working set
  if (Access==faCreate){
    IAssert(!WebFilter.Empty());
    // main file
    LastFId=0;
    TFOut WebBsFetchFOut(WebBsFetchFNm);
    LastFId.Save(WebBsFetchFOut);
    WebFilter->Save(WebBsFetchFOut);
    // url strings
    UrlStrToFIdH=PUrlStrToFIdH(new
     TUrlStrToFIdH(WebBsFetchUrlFNm, Access, EstWebPgs, UrlStrHCacheSize));
    // initialize starting urls
    for (int UrlN=0; UrlN<WebFilter->GetStartUrls(); UrlN++){
      AddFetchUrl(WebFilter->GetStartUrl(UrlN));}
  } else {
    IAssert(Access==faUpdate);
    IAssert(Exists(Nm, FPath));
    IAssert(WebFilter.Empty());
    // main file
    TFIn WebBsFetchFIn(WebBsFetchFNm);
    LastFId=TInt(WebBsFetchFIn);
    WebFilter=TWebFilter::Load(WebBsFetchFIn);
    // url strings
    UrlStrToFIdH=PUrlStrToFIdH(new
     TUrlStrToFIdH(WebBsFetchUrlFNm, Access, -1, UrlStrHCacheSize));
    TBlobPt UrlStrToStatusP=UrlStrToFIdH->FFirstKeyId(); TBlobPt UrlStrId;
    while (UrlStrToFIdH->FNextKeyId(UrlStrToStatusP, UrlStrId)){
      UrlStrIdQ.Push(UrlStrId);}
  }
}

TWebBsFetch::~TWebBsFetch(){
  // close all connections
  int UrlStrIdP=UrlStrIdToSockEventH.FFirstKeyId();
  while (UrlStrIdToSockEventH.FNextKeyId(UrlStrIdP)){
    PSockEvent SockEvent=UrlStrIdToSockEventH[UrlStrIdP];
    UrlStrIdToSockEventH[UrlStrIdP].Clr();
    TSockEvent::UnReg(SockEvent);
    SockEvent->CloseConn();
  }

  // save main file
  TFOut WebBsFetchFOut(WebBsFetchFNm);
  LastFId.Save(WebBsFetchFOut);
  WebFilter->Save(WebBsFetchFOut);

  // save host file
  PWebHostNmBs WebHostNmBs=GetHostNmBs();
  PSOut WebHostBsSOut=TFOut::New(WebHostBsFNm);
  WebHostNmBs->SaveTxt(WebHostBsSOut);
}

bool TWebBsFetch::SetUrlAsFinal(const PUrl& Url){
  if (Url->IsDefFinalUrl()){
    // final url is already defined
    return true;
  } else {
    TStr HostNm=Url->GetHostNm(); int HostId;
    if (HostNmToDescH.IsKey(HostNm, HostId)){
      // host-name is already known
      TWebBsFetchHostDesc& HostDesc=HostNmToDescH[HostId];
      if (HostDesc.IsDefFinalHostNm()){
        // host-name has already defined final host-name
        if (HostDesc.IsAliasHostNm(HostNm)){
          // host-name is an alias - define final host-name in url
          Url->DefFinalUrl(HostDesc.GetFinalHostNm());
        } else {
          // host-name is already final - define url as final
          Url->DefUrlAsFinal();
        }
        return true;
      } else {
        // host-name has not defined final host-name
        return false;
      }
    } else {
      // host-name is unknown
      HostNmToDescH.AddDat(HostNm);
      return false;
    }
  }
}

PWebHostNmBs TWebBsFetch::GetHostNmBs() const {
  PWebHostNmBs WebHostNmBs=PWebHostNmBs(new TWebHostNmBs());
  int HostNmToDescP=HostNmToDescH.FFirstKeyId();
  while (HostNmToDescH.FNextKeyId(HostNmToDescP)){
    TStr AliasHostNm=HostNmToDescH.GetKey(HostNmToDescP);
    const TWebBsFetchHostDesc& HostDesc=HostNmToDescH[HostNmToDescP];
    if ((HostDesc.IsDefFinalHostNm())
     /*&&(HostDesc.IsAliasHostNm(AliasHostNm))*/){
      TStr FinalHostNm=HostDesc.GetFinalHostNm();
      WebHostNmBs->AddAliasToFinalNm(AliasHostNm, FinalHostNm);
    }
  }
  return WebHostNmBs;
}

void TWebBsFetch::GetHostNmConnsPrV(TStrIntPrV& HostNmConnsPrV) const {
  // prepare host-name counts
  TStrIntH HostNmToConnsH(UrlStrIdToSockEventH.Len()+1);
  // traverse active connections
  int UrlStrIdP=UrlStrIdToSockEventH.FFirstKeyId();
  while (UrlStrIdToSockEventH.FNextKeyId(UrlStrIdP)){
    // get UrlStrId
    TBlobPt UrlStrId=UrlStrIdToSockEventH.GetKey(UrlStrIdP);
    // prepare UrlStrId & UrlStr & FId & Url
    TStr UrlStr; TInt FId;
    UrlStrToFIdH->GetKeyFDat(UrlStrId, UrlStr, FId);
    PUrl Url=TUrl::New(UrlStr); IAssert(Url->IsOk());
    // increment host connections
    HostNmToConnsH.AddDat(Url->GetHostNm())++;
  }
  // prepare sort vector
  TIntStrPrV ConnsHostNmPrV(HostNmToConnsH.Len(), 0);
  int HostNmP=HostNmToConnsH.FFirstKeyId();
  while (HostNmToConnsH.FNextKeyId(HostNmP)){
    TStr HostNm=HostNmToConnsH.GetKey(HostNmP);
    int Conns=HostNmToConnsH[HostNmP];
    ConnsHostNmPrV.Add(TIntStrPr(Conns, HostNm));
  }
  ConnsHostNmPrV.Sort();
  // copy sort vector to output vector
  HostNmConnsPrV.Gen(ConnsHostNmPrV.Len(), 0);
  for (int ConnsN=0; ConnsN<ConnsHostNmPrV.Len(); ConnsN++){
    int Conns=ConnsHostNmPrV[ConnsN].Val1;
    TStr HostNm=ConnsHostNmPrV[ConnsN].Val2;
    HostNmConnsPrV.Add(TStrIntPr(HostNm, Conns));
  }
}

TStr TWebBsFetch::GetHostNmConnsPrVStr() const {
  // get host-names, connections vector
  TStrIntPrV HostNmConnsPrV; GetHostNmConnsPrV(HostNmConnsPrV);
  // transform vector to string
  TChA ChA;
  for (int HostNmN=0; HostNmN<HostNmConnsPrV.Len(); HostNmN++){
    TStr HostNm=HostNmConnsPrV[HostNmN].Val1;
    int Conns=HostNmConnsPrV[HostNmN].Val2;
    ChA+=HostNm; ChA+=":"; ChA+=TInt::GetStr(Conns); ChA+="   "; //ChA+="\r\n";
  }
  // return string
  return ChA;
}

void TWebBsFetch::GetHostNmQueuedPrV(TStrIntPrV& HostNmQueuedPrV) const {
  // prepare host-name counts
  TStrIntH HostNmToQueuedH(UrlStrIdQ.Len()/10+1);
  // traverse queued urls
  for (int UrlStrIdN=0; UrlStrIdN<UrlStrIdQ.Len() ; UrlStrIdN++){
    // get UrlStrId
    TBlobPt UrlStrId=UrlStrIdQ[UrlStrIdN];
    // prepare UrlStrId & UrlStr & FId & Url
    TStr UrlStr; TInt FId;
    UrlStrToFIdH->GetKeyFDat(UrlStrId, UrlStr, FId);
    PUrl Url=TUrl::New(UrlStr); IAssert(Url->IsOk());
    // increment host connections
    HostNmToQueuedH.AddDat(Url->GetHostNm())++;
  }
  // prepare sort vector
  TIntStrPrV QueuedHostNmPrV(HostNmToQueuedH.Len(), 0);
  int HostNmP=HostNmToQueuedH.FFirstKeyId();
  while (HostNmToQueuedH.FNextKeyId(HostNmP)){
    TStr HostNm=HostNmToQueuedH.GetKey(HostNmP);
    int Queued=HostNmToQueuedH[HostNmP];
    QueuedHostNmPrV.Add(TIntStrPr(Queued, HostNm));
  }
  QueuedHostNmPrV.Sort(false);
  // copy sort vector to output vector
  HostNmQueuedPrV.Gen(QueuedHostNmPrV.Len(), 0);
  for (int QueuedN=0; QueuedN<QueuedHostNmPrV.Len(); QueuedN++){
    int Queued=QueuedHostNmPrV[QueuedN].Val1;
    TStr HostNm=QueuedHostNmPrV[QueuedN].Val2;
    HostNmQueuedPrV.Add(TStrIntPr(HostNm, Queued));
  }
}

TStr TWebBsFetch::GetHostNmQueuedPrVStr(const int& ShowLen) const {
  // get host-names, queued vector
  TStrIntPrV HostNmQueuedPrV; GetHostNmQueuedPrV(HostNmQueuedPrV);
  // transform vector to string
  int UpHostNmN=TInt::GetMn(ShowLen, HostNmQueuedPrV.Len());
  TChA ChA;
  for (int HostNmN=0; HostNmN<UpHostNmN; HostNmN++){
    TStr HostNm=HostNmQueuedPrV[HostNmN].Val1;
    int Queued=HostNmQueuedPrV[HostNmN].Val2;
    ChA+=HostNm; ChA+=":"; ChA+=TInt::GetStr(Queued); ChA+="\r\n";
  }
  // return string
  return ChA;
}

void TWebBsFetch::GoFetch(){
  TBlobPtV RepushedUrlStrIdV;
  while ((WebFilter->IsOkConns(GetConns()))&&(UrlStrIdQ.Len()>0)){
    // prepare UrlStrId & UrlStr & FId & Url
    TBlobPt UrlStrId=UrlStrIdQ.Top(); UrlStrIdQ.Pop();
    TStr UrlStr; TInt FId;
    UrlStrToFIdH->GetKeyFDat(UrlStrId, UrlStr, FId);
    PUrl Url=TUrl::New(UrlStr); IAssert(Url->IsOk());
    SetUrlAsFinal(Url);
    // check if url is still interesting
    if (DoStillFetchUrl(Url)){
      // url is interesting
      // prepare Url & HostNm
      // check host connections
      TWebBsFetchHostDesc& HostDesc=GetAsFinalHostDesc(Url);
      IAssert(HostDesc.IsDefConns());
      if (WebFilter->IsOkConnsPerSrv(HostDesc.GetConns())){
        // increment connections per host
        HostDesc.IncConns();
        // prepare socket-event
        PSockEvent SockEvent=PSockEvent(new
         TWebBsFetchEvent(this, UrlStrId, FId, Url));
        TSockEvent::Reg(SockEvent);
        UrlStrIdToSockEventH.AddDat(UrlStrId, SockEvent);
        // start network communication
        TSockHost::GetAsyncSockHost(Url->GetIpNumOrHostNm(), SockEvent);
      } else {
        UrlStrIdQ.Push(UrlStrId); // repush UrlStrId
        if ((RepushedUrlStrIdV.Len()>100)||
         (RepushedUrlStrIdV.IsIn(UrlStrId))){break;} // prevent cycling
        RepushedUrlStrIdV.Add(UrlStrId); // memorize repushed UrlStrId
      }
    } else {
      // url is not interesting
      UrlStrToFIdH->DelKeyId(UrlStrId);
    }
  }
}

void TWebBsFetch::GetFNms(
 const TStr& Nm, const TStr& FPath,
 TStr& WebBsFetchFNm, TStr& WebBsFetchUrlFNm, TStr& WebHostBsFNm){
  TStr NrFPath=TStr::GetNrFPath(FPath);
  TStr NrNm=TStr::GetNrFMid(Nm);
  WebBsFetchFNm=NrFPath+NrNm+WebBsFetchFExt;
  WebBsFetchUrlFNm=NrFPath+NrNm+WebBsFetchUrlFExt;
  WebHostBsFNm=NrFPath+NrNm+WebHostBsFExt;
}

bool TWebBsFetch::Exists(const TStr& Nm, const TStr& FPath){
  TStr WebBsFetchFNm; TStr WebBsFetchUrlFNm; TStr WebHostBsFNm;
  GetFNms(Nm, FPath, WebBsFetchFNm, WebBsFetchUrlFNm, WebHostBsFNm);
  return
   TFile::Exists(WebBsFetchFNm)&&
   TFile::Exists(WebBsFetchUrlFNm)&&
   TFile::Exists(WebHostBsFNm);
}

void TWebBsFetch::Del(const TStr& Nm, const TStr& FPath){
  if (Exists(Nm, FPath)){
    TStr WebBsFetchFNm; TStr WebBsFetchUrlFNm; TStr WebHostBsFNm;
    GetFNms(Nm, FPath, WebBsFetchFNm, WebBsFetchUrlFNm, WebHostBsFNm);
    TFile::Del(WebBsFetchFNm);
    TFile::Del(WebBsFetchUrlFNm);
    TFile::Del(WebHostBsFNm);
  }
}

