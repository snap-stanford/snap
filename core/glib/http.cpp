/////////////////////////////////////////////////
// Http-General

// general strings
const TStr THttp::HttpStr="http";
const TStr THttp::SlashStr="/";
const TStr THttp::ColonStr=":";

// fields names
const TStr THttp::ContTypeFldNm="Content-Type";
const TStr THttp::ContLenFldNm="Content-Length";
const TStr THttp::HostFldNm="Host";
const TStr THttp::AcceptRangesFldNm="Accept-Ranges";
const TStr THttp::CacheCtrlFldNm="Cache-Control";
const TStr THttp::AcceptFldNm="Accept";
const TStr THttp::SrvFldNm="Server";
const TStr THttp::ConnFldNm="Connection";
const TStr THttp::FetchIdFldNm="FetchId";
const TStr THttp::LocFldNm="Location";
const TStr THttp::SetCookieFldNm="Set-Cookie";
const TStr THttp::CookieFldNm="Cookie";

// content-type field-values
const TStr THttp::TextFldVal="text/";
const TStr THttp::TextPlainFldVal="text/plain";
const TStr THttp::TextHtmlFldVal="text/html";
const TStr THttp::TextXmlFldVal="text/xml";
const TStr THttp::TextWmlFldVal="text/vnd.wap.wml";
const TStr THttp::ImageGifFldVal="image/gif";
const TStr THttp::AppOctetFldVal="application/octet-stream";
const TStr THttp::AppSoapXmlFldVal="application/soap+xml";
const TStr THttp::AppW3FormFldVal="application/x-www-form-urlencoded";
const TStr THttp::ConnKeepAliveFldVal="keep-alive";

// file extensions
bool THttp::IsHtmlFExt(const TStr& FExt){
  TStr UcFExt=FExt.GetUc();
  return ((UcFExt==TFile::HtmlFExt.GetUc())||(UcFExt==TFile::HtmFExt.GetUc()));
}

bool THttp::IsGifFExt(const TStr& FExt){
  return (FExt.GetUc()==TFile::GifFExt.GetUc());
}

// port number
const int THttp::DfPortN=80;

// status codes
const int THttp::OkStatusCd=200;
const int THttp::RedirStatusCd=300;
const int THttp::BadRqStatusCd=400;
const int THttp::ErrStatusCd=400;
const int THttp::ErrNotFoundStatusCd=404;

TStr THttp::GetReasonPhrase(const int& StatusCd){
  switch (StatusCd){
    case 200: return "OK";
    case 201: return "Created";
    case 202: return "Accepted";
    case 204: return "No Content";
    case 300: return "Multiple Choices";
    case 301: return "Moved Permanently";
    case 302: return "Moved Temporarily";
    case 304: return "Not Modified";
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    case 502: return "Bad Gateway";
    case 503: return "Service Unavailable";
    default: return TStr("Unknown Status Code ")+TInt::GetStr(StatusCd);
  }
}

// method names
const TStr THttp::GetMethodNm="GET";
const TStr THttp::HeadMethodNm="HEAD";
const TStr THttp::PostMethodNm="POST";

/////////////////////////////////////////////////
// Http-Chars
typedef enum {hpctUndef, hpctAlpha, hpctDigit, hpctCtl, hpctSpec} THttpChTy;

class THttpChDef{
private:
  TIntV ChTyV;
  TChV LcChV;
  void SetLcCh(const TStr& Str);
  void SetChTy(const THttpChTy& ChTy, const char& Ch);
  void SetChTy(const THttpChTy& ChTy, const TStr& Str);
public:
  THttpChDef();

  THttpChDef& operator=(const THttpChDef&){Fail; return *this;}

  int GetChTy(const char& Ch){return ChTyV[Ch-TCh::Mn];}
  bool IsAlpha(const char& Ch){return ChTyV[Ch-TCh::Mn]==int(hpctAlpha);}
  bool IsDigit(const char& Ch){return ChTyV[Ch-TCh::Mn]==int(hpctDigit);}
  bool IsCtl(const char& Ch){return ChTyV[Ch-TCh::Mn]==int(hpctCtl);}
  bool IsLws(const char& Ch){
    return (Ch==' ')||(Ch==TCh::TabCh)||(Ch==TCh::CrCh)||(Ch==TCh::LfCh);}
  bool IsText(const char& Ch){return !IsCtl(Ch)||IsLws(Ch);}
  bool IsSpec(const char& Ch){
    return (ChTyV[Ch-TCh::Mn]==int(hpctSpec))||(Ch==9)||(Ch==32);}
  bool IsCr(const char& Ch){return Ch==13;}
  bool IsLf(const char& Ch){return Ch==10;}
  bool IsSp(const char& Ch){return Ch==32;}
  bool IsHt(const char& Ch){return Ch==9;}
  bool IsDQuote(const char& Ch){return Ch=='"';}

  char GetLcCh(const char& Ch){return LcChV[Ch-TCh::Mn];}
  TStr GetLcStr(const TStr& Str);
};

void THttpChDef::SetChTy(const THttpChTy& ChTy, const char& Ch){
  IAssert(ChTyV[Ch-TCh::Mn]==int(hpctUndef)); ChTyV[Ch-TCh::Mn]=TInt(ChTy);}

void THttpChDef::SetChTy(const THttpChTy& ChTy, const TStr& Str){
  for (int ChN=0; ChN<Str.Len(); ChN++){SetChTy(ChTy, Str[ChN]);}}

void THttpChDef::SetLcCh(const TStr& Str){
  for (int ChN=1; ChN<Str.Len(); ChN++){LcChV[Str[ChN]-TCh::Mn]=TCh(Str[0]);}}

THttpChDef::THttpChDef():
  ChTyV(TCh::Vals), LcChV(TCh::Vals){

  // Character-Types
  ChTyV.PutAll(TInt(hpctUndef));
  SetChTy(hpctAlpha, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  SetChTy(hpctAlpha, "abcdefghijklmnopqrstuvwxyz");
  SetChTy(hpctDigit, "0123456789");
  for (char Ch=0; Ch<=31; Ch++){SetChTy(hpctCtl, Ch);}
  SetChTy(hpctCtl, 127);
  SetChTy(hpctSpec, "()<>@,;:\\\"/[]?={}"); // +char(9)+char(32)

  // Lower-Case
  {for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){LcChV[Ch-TCh::Mn]=TCh(char(Ch));}}
  SetLcCh("aA"); SetLcCh("bB"); SetLcCh("cC"); SetLcCh("dD"); SetLcCh("eE");
  SetLcCh("fF"); SetLcCh("gG"); SetLcCh("hH"); SetLcCh("iI"); SetLcCh("jJ");
  SetLcCh("kK"); SetLcCh("lL"); SetLcCh("mM"); SetLcCh("nN"); SetLcCh("oO");
  SetLcCh("pP"); SetLcCh("qQ"); SetLcCh("rR"); SetLcCh("sS"); SetLcCh("tT");
  SetLcCh("uU"); SetLcCh("vV"); SetLcCh("wW"); SetLcCh("xX"); SetLcCh("yY");
  SetLcCh("zZ");
}

TStr THttpChDef::GetLcStr(const TStr& Str){
  TChA LcStr;
  for (int ChN=0; ChN<Str.Len(); ChN++){LcStr+=GetLcCh(Str[ChN]);}
  return LcStr;
}

/////////////////////////////////////////////////
// Http-Exception
typedef enum {
  heUnexpectedEof, hePeriodExpected, heTokenExpected, heInvalidToken,
  heTSpecExpected, heInvalidTSpec, heNumExpected, heInvalidNumPlaces,
  heCrLfExpected, heMethodNmExpected, heUrlEmpty, heBadUrl,
  heBadSearchStr} THttpExCd;

class THttpEx{
private:
  THttpExCd HttpExCd;
public:
  THttpEx(const THttpExCd& _HttpExCd): HttpExCd(_HttpExCd){}
};

/////////////////////////////////////////////////
// Http-Lexical
class THttpLx{
private:
  static THttpChDef ChDef;
  PSIn SIn;
  //TChA ChStack;
  TBoolChS EofChPrS;
  char Ch;
  bool AtEof;
  TMem SfMem;
public:
  THttpLx(const PSIn& _SIn):
    SIn(_SIn), EofChPrS(), Ch(' '), AtEof(false), SfMem(50000){
    GetFirstCh();}

  THttpLx& operator=(const THttpLx&){Fail; return *this;}

  // basic
  bool Eof(){return AtEof;}
  int Len(){return EofChPrS.Len()+SIn->Len();}
  char GetFirstCh();
  char GetCh();
  void GetRest();
  void PutCh(const char& _Ch){
    EofChPrS.Push(TBoolChPr(AtEof, Ch)); Ch=_Ch; AtEof=false; SfMem.Pop();}
  void ClrMemSf(){SfMem.Clr();}
  TMem& GetMemSf(){return SfMem;}

  // http request
  THttpRqMethod GetRqMethod();
  PUrl GetUrl();
  TStr GetUrlStr();
  // http response
  bool IsRespStatusLn();
  TStr GetRespReasonPhrase();
  // spacing
  void GetWs();
  bool IsLws();
  void GetLws();
  bool IsCrLf();
  void GetCrLf();
  // tokens
  void GetPeriod();
  TStr GetToken(const TStr& ExpectStr=TStr());
  TStr GetSpec(const TStr& ExpectStr=TStr());
  int GetInt(const int& RqPlaces=-1);
  TStr GetFldVal();

  static TStr GetNrStr(const TStr& Str){return ChDef.GetLcStr(Str);}
};
THttpChDef THttpLx::ChDef;

char THttpLx::GetFirstCh(){
  if (SIn->Eof()){
    if (AtEof){throw THttpEx(heUnexpectedEof);}
    AtEof=true; return 0;
  } else {
    Ch=SIn->GetCh(); return Ch;
  }
}

char THttpLx::GetCh(){
  if (EofChPrS.Empty()){
    if (SIn->Eof()){
      if (AtEof){throw THttpEx(heUnexpectedEof);}
      AtEof=true; SfMem+=Ch; Ch=TCh::NullCh; return Ch;
    } else {
      SfMem+=Ch; Ch=SIn->GetCh(); return Ch;
    }
  } else {
    SfMem+=Ch;
    AtEof=EofChPrS.Top().Val1; Ch=EofChPrS.Top().Val2; EofChPrS.Pop();
    return Ch;
  }
}

void THttpLx::GetRest(){
  while ((!SIn->Eof())&&(!EofChPrS.Empty())){GetCh();}
  if (!SIn->Eof()){SfMem+=Ch;}
  TMem RestMem; TMem::LoadMem(SIn, RestMem);
  SfMem+=RestMem;
}

THttpRqMethod THttpLx::GetRqMethod(){
  TChA MethodNm;
  while (!Eof() && ChDef.IsAlpha(Ch)){
    MethodNm+=Ch; GetCh();}
  THttpRqMethod Method=hrmUndef;
  if (MethodNm==THttp::GetMethodNm){Method=hrmGet;}
  else if (MethodNm==THttp::HeadMethodNm){Method=hrmHead;}
  else if (MethodNm==THttp::PostMethodNm){Method=hrmPost;}
  if (Method==hrmUndef){throw THttpEx(heMethodNmExpected);}
  return Method;
}

PUrl THttpLx::GetUrl(){
  TChA UrlChA;
  while ((!Eof())&&(!ChDef.IsSp(Ch))){
    UrlChA+=Ch; GetCh();}
  if (UrlChA.Empty()){
    throw THttpEx(heUrlEmpty);}
  static TStr LocalBaseUrlStr="http://localhost/";
  PUrl Url=PUrl(new TUrl(UrlChA, LocalBaseUrlStr));
  if (!Url->IsOk()){
    throw THttpEx(heBadUrl);}
  return Url;
}

TStr THttpLx::GetUrlStr(){
  TChA UrlChA;
  while ((!Eof())&&(!ChDef.IsSp(Ch))){
    UrlChA+=Ch; GetCh();}
  if (UrlChA.Empty()){
    throw THttpEx(heUrlEmpty);}
  return UrlChA;
}

bool THttpLx::IsRespStatusLn(){
  static const TChA MouldChA="http/N.N NNN ";
  TChA TestChA(MouldChA);
  int TestLen=TestChA.Len();
  if (1+Len()<TestLen){return false;}
  TestChA.PutCh(0, ChDef.GetLcCh(Ch));
  {for (int ChN=1; ChN<TestLen; ChN++){
    TestChA.PutCh(ChN, ChDef.GetLcCh(GetCh()));}}
  {for (int ChN=1; ChN<TestLen; ChN++){
    PutCh(TestChA[TestLen-ChN-1]);}}
  {for (int ChN=0; ChN<MouldChA.Len(); ChN++){
    if (MouldChA[ChN]=='N'){
      if (!ChDef.IsDigit(TestChA[ChN])){return false;}
    } else {
      if (MouldChA[ChN]!=TestChA[ChN]){return false;}
    }
  }}
  return true;
}

TStr THttpLx::GetRespReasonPhrase(){
  GetLws();
  TChA RPStr;
  while (!Eof()&&ChDef.IsText(Ch)&&(Ch!=TCh::CrCh)&&(Ch!=TCh::LfCh)){
    RPStr+=Ch; GetCh();}
  return RPStr;
}

void THttpLx::GetWs(){
  while (!Eof()&&((Ch==' ')||(Ch==TCh::TabCh))){GetCh();}
}

bool THttpLx::IsLws(){
  if ((Ch==' ')||(Ch==TCh::TabCh)){
    return true;
  } else
  if (Ch==TCh::CrCh){
    GetCh();
    if (Ch==TCh::LfCh){
      GetCh(); bool Ok=(Ch==' ')||(Ch==TCh::TabCh);
      PutCh(TCh::LfCh); PutCh(TCh::CrCh); return Ok;
    } else {
      PutCh(TCh::CrCh); return false;
    }
  } else
  if (Ch==TCh::LfCh){
    GetCh(); bool Ok=(Ch==' ')||(Ch==TCh::TabCh);
    PutCh(TCh::LfCh); return Ok;
  } else {
    return false;
  }
}

void THttpLx::GetLws(){
  forever {
    while ((Ch==' ')||(Ch==TCh::TabCh)){GetCh();}
    if (Ch==TCh::CrCh){
      GetCh();
      if (Ch==TCh::LfCh){
        GetCh();
        if ((Ch==' ')||(Ch==TCh::TabCh)){GetCh();}
        else {PutCh(TCh::LfCh); PutCh(TCh::CrCh); break;}
      } else {
        PutCh(TCh::CrCh); break;
      }
    } else
    if (Ch==TCh::LfCh){
      GetCh();
      if ((Ch==' ')||(Ch==TCh::TabCh)){GetCh();}
      else {PutCh(TCh::LfCh); break;}
    } else {
      break;
    }
  }
}

bool THttpLx::IsCrLf(){
  if (Ch==TCh::CrCh){
    GetCh(); bool Ok=(Ch==TCh::LfCh); PutCh(TCh::CrCh); return Ok;
  } else
  if (Ch==TCh::LfCh){
    return true;
  } else {
    return false;
  }
}

void THttpLx::GetCrLf(){
  if (Ch==TCh::CrCh){
    GetCh();
    if (Ch==TCh::LfCh){GetCh();} else {throw THttpEx(heCrLfExpected);}
  } else
  if (Ch==TCh::LfCh){
    GetCh();
  } else {
    throw THttpEx(heCrLfExpected);
  }
}

void THttpLx::GetPeriod(){
  GetWs();
  if (Ch!='.'){throw THttpEx(hePeriodExpected);}
  GetCh();
}

TStr THttpLx::GetToken(const TStr& ExpectStr){
  GetLws();
  TChA TokenStr;
  while (!Eof() && !ChDef.IsCtl(Ch) && !ChDef.IsSpec(Ch)){
    TokenStr+=Ch; GetCh();}
  if (TokenStr.Empty()){throw THttpEx(heTokenExpected);}
  if (!ExpectStr.Empty()){
    if (GetNrStr(ExpectStr)!=GetNrStr(TokenStr)){
      throw THttpEx(heInvalidToken);}
  }
  return TokenStr;
}

TStr THttpLx::GetSpec(const TStr& ExpectStr){
  GetLws();
  if (!ChDef.IsSpec(Ch)){throw THttpEx(heTSpecExpected);}
  TStr SpecStr(Ch); GetCh();
  if (!ExpectStr.Empty()){
    if (ExpectStr!=SpecStr){throw THttpEx(heInvalidTSpec);}}
  return SpecStr;
}

int THttpLx::GetInt(const int& RqPlaces){
  GetLws();
  if (!ChDef.IsDigit(Ch)){throw THttpEx(heNumExpected);}
  int Int=0; int CurPlaces=0;
  do {Int=Int*10+Ch-'0'; CurPlaces++; GetCh();
  } while ((CurPlaces<RqPlaces)&&(ChDef.IsDigit(Ch)));
  if (RqPlaces!=-1){
    if (CurPlaces!=RqPlaces){throw THttpEx(heInvalidNumPlaces);}}
  return Int;
}

TStr THttpLx::GetFldVal(){
  TChA FldValStr;
  do {
    GetLws();
    while (!Eof()&&ChDef.IsText(Ch)&&(Ch!=TCh::CrCh)&&(Ch!=TCh::LfCh)){
      FldValStr+=Ch; GetCh();}
    if (IsLws()){FldValStr+=' ';}
  } while (IsLws());
  return FldValStr;
}

/////////////////////////////////////////////////
// Http-Character-Returner
class THttpChRet{
  PSIn SIn;
  int Chs, ChN;
  THttpExCd HttpExCd;
public:
  THttpChRet(const PSIn& _SIn, const THttpExCd& _HttpExCd):
    SIn(_SIn), Chs(SIn->Len()), ChN(0), HttpExCd(_HttpExCd){}
  THttpChRet& operator=(const THttpChRet&){Fail; return *this;}
  bool Eof(){return ChN==Chs;}
  char GetCh(){
    if (ChN>=Chs){throw THttpEx(HttpExCd);}
    ChN++; return SIn->GetCh();}
};

/////////////////////////////////////////////////
// Http-Request
void THttpRq::ParseSearch(const TStr& SearchStr){
  PSIn SIn=TStrIn::New(SearchStr);
  THttpChRet ChRet(SIn, heBadSearchStr);

  // check empty search string
  if (ChRet.Eof()){return;}
  // require '?' at the beginning
  if (ChRet.GetCh()!='?'){
    throw THttpEx(heBadSearchStr);}
  // parse key=val{&...} pairs
  TChA KeyNm; TChA ValStr;
  while (!ChRet.Eof()){
    char Ch; KeyNm.Clr(); ValStr.Clr();
    // key
    while ((Ch=ChRet.GetCh())!='='){
      switch (Ch){
        case '%':{
          char Ch1=ChRet.GetCh(); char Ch2=ChRet.GetCh();
          KeyNm.AddCh(char(16*TCh::GetHex(Ch1)+TCh::GetHex(Ch2)));} break;
        case '+': KeyNm.AddCh(' '); break;
        case '&': throw THttpEx(heBadSearchStr);
        default: KeyNm.AddCh(Ch);
      }
    }
    // equals
    if (Ch!='='){
      throw THttpEx(heBadSearchStr);}
    // value
    while ((!ChRet.Eof())&&((Ch=ChRet.GetCh())!='&')){
      switch (Ch){
        case '%':{
          char Ch1=ChRet.GetCh();
          char Ch2=ChRet.GetCh();
          ValStr.AddCh(char(16*TCh::GetHex(Ch1)+TCh::GetHex(Ch2)));} break;
        case '+': ValStr.AddCh(' '); break;
        case '&': throw THttpEx(heBadSearchStr);
        default: ValStr.AddCh(Ch);
      }
    }
    // save key-value pair
    UrlEnv->AddToKeyVal(KeyNm, ValStr);
  }
}

void THttpRq::ParseHttpRq(const PSIn& SIn){
  THttpLx Lx(SIn);
  // initial status
  Ok=false;
  CompleteP=false;
  // request-line
  Method=Lx.GetRqMethod();
  Lx.GetWs();
  //Url=Lx.GetUrl();
  TStr UrlStr=Lx.GetUrlStr();
  Lx.GetWs();
  Lx.GetToken(THttp::HttpStr); Lx.GetSpec(THttp::SlashStr);
  MajorVerN=Lx.GetInt(1); Lx.GetPeriod(); MinorVerN=Lx.GetInt(1);
  Lx.GetCrLf();
  // header fields & values
  while ((!Lx.Eof())&&(!Lx.IsCrLf())){
    TStr FldNm=Lx.GetToken(); Lx.GetSpec(THttp::ColonStr);
    TStr FldVal=Lx.GetFldVal();
    Lx.GetCrLf();
    TStr NrFldNm=THttpLx::GetNrStr(FldNm);
    FldNmToValH.AddDat(NrFldNm, FldVal);
  }
  // separator CrLf
  if (!Lx.IsCrLf()){return;} // to avoid exceptions
  Lx.GetCrLf();
  // header & body strings
  HdStr=Lx.GetMemSf().GetAsStr();
  Lx.ClrMemSf();
  Lx.GetRest();
  BodyMem=Lx.GetMemSf();
  // completeness
  int ContLen=GetFldVal(THttp::ContLenFldNm).GetInt(-1);
  if (ContLen==-1){
    // if not content-len is given we assume http-request is ok
    CompleteP=true;
  } else {
    if (ContLen<=BodyMem.Len()){
      // if we read enough data, we claim completeness
      CompleteP=true;
      BodyMem.Trunc(ContLen);
    } else {
      // if we read not enough data we claim incompleteness
      CompleteP=false;
    }
  }
  // url
  if (CompleteP){
    const TStr LocalBaseUrlStr="http://localhost/";
    Url=TUrl::New(UrlStr, LocalBaseUrlStr);
    if (!Url->IsOk()){
      throw THttpEx(heBadUrl);}
  }
  // search string
  TStr SearchStr;
  if (Method==hrmGet){
    SearchStr=Url->GetSearchStr();
  } else
  if ((Method==hrmPost)&&(
   (!IsFldNm(THttp::ContTypeFldNm))||
   (GetFldVal(THttp::ContTypeFldNm)==THttp::TextHtmlFldVal)||
   (GetFldVal(THttp::ContTypeFldNm)==THttp::AppW3FormFldVal))){
    SearchStr=TStr("?")+BodyMem.GetAsStr();
  }
  ParseSearch(SearchStr);
  // at this point ok=true
  Ok=true;
}

THttpRq::THttpRq(const PSIn& SIn):
  Ok(false), MajorVerN(0), MinorVerN(0), Method(hrmUndef),
  FldNmToValH(), UrlEnv(TUrlEnv::New()),
  HdStr(), BodyMem(){
  try {
    ParseHttpRq(SIn);
  }
  catch (THttpEx){Ok=false;}
}

THttpRq::THttpRq(
 const THttpRqMethod& _Method, const PUrl& _Url,
 const TStr& ContTypeFldVal, const TMem& _BodyMem, const int& FetchId):
  Ok(false),
  MajorVerN(1), MinorVerN(0),
  Method(_Method),
  Url(_Url),
  FldNmToValH(),
  UrlEnv(TUrlEnv::New()),
  HdStr(), BodyMem(_BodyMem){
  // compose head-http-request
  TChA HdChA;
  if (Url->IsOk()){
    TStr AbsPath=Url->GetPathStr()+Url->GetSearchStr();
    HdChA+=GetMethodNm(); HdChA+=' '; HdChA+=AbsPath; HdChA+=" HTTP/1.0\r\n";
  }
  // add content-type
  if (!ContTypeFldVal.Empty()){
    FldNmToValH.AddDat(THttpLx::GetNrStr(THttp::ContTypeFldNm), ContTypeFldVal);
    HdChA+=THttpLx::GetNrStr(THttp::ContTypeFldNm); HdChA+=": ";
    HdChA+=ContTypeFldVal; HdChA+="\r\n";
  }
  // add host
  if (Url->IsOk()){
    TStr HostNm=Url->GetHostNm();
    FldNmToValH.AddDat(THttpLx::GetNrStr(THttp::HostFldNm), HostNm);
    HdChA+=THttpLx::GetNrStr(THttp::HostFldNm); HdChA+=": ";
    HdChA+=HostNm; HdChA+="\r\n";
    ParseSearch(Url->GetSearchStr());
  }
  // add fetch-id
  if (Url->IsOk()&&(FetchId!=-1)){
    TStr FetchIdStr=TInt::GetStr(FetchId);
    FldNmToValH.AddDat(THttpLx::GetNrStr(THttp::FetchIdFldNm), FetchIdStr);
    HdChA+=THttpLx::GetNrStr(THttp::FetchIdFldNm); HdChA+=": ";
    HdChA+=FetchIdStr; HdChA+="\r\n";
  }
  // finish head-http-request
  if (Url->IsOk()){
    HdChA+="\r\n";
    HdStr=HdChA;
  }
  // set http-request ok
  Ok=true;
}

TStr THttpRq::GetMethodNm() const {
  switch (Method){
    case hrmGet: return THttp::GetMethodNm;
    case hrmHead: return THttp::HeadMethodNm;
    case hrmPost: return THttp::PostMethodNm;
    default: return "UndefinedMethod";
  }
}

bool THttpRq::IsFldNm(const TStr& FldNm) const {
  return FldNmToValH.IsKey(THttpLx::GetNrStr(FldNm));
}

TStr THttpRq::GetFldVal(const TStr& FldNm) const {
  TStr NrFldNm=THttpLx::GetNrStr(FldNm);
  if (FldNmToValH.IsKey(NrFldNm)){
    return FldNmToValH.GetDat(NrFldNm);
  } else {
    return TStr();
  }
}

bool THttpRq::IsFldVal(const TStr& FldNm, const TStr& FldVal) const {
  return THttpLx::GetNrStr(FldVal)==THttpLx::GetNrStr(GetFldVal(FldNm));
}

TStr THttpRq::GetStr() const {
  TChA ChA;
  ChA+=GetMethodNm(); ChA+=' ';
  ChA+=Url->GetUrlStr(); ChA+=' ';
  ChA+="HTTP/1.0\r\n";
  for (int FldN=0; FldN<FldNmToValH.Len(); FldN++){
    ChA+=FldNmToValH.GetKey(FldN); ChA+=": ";
    ChA+=FldNmToValH[FldN]; ChA+="\r\n";
  }
  ChA+="\r\n";
  ChA+=BodyMem.GetAsStr();
  return ChA;
}

/////////////////////////////////////////////////
// Http-Response
void THttpResp::AddHdFld(const TStr& FldNm, const TStr& FldVal, TChA& HdChA){
  TStr NrFldNm=THttpLx::GetNrStr(FldNm);
  FldNmToValVH.AddDat(NrFldNm).Add(FldVal);
  HdChA+=FldNm; HdChA+=": "; HdChA+=FldVal; HdChA+="\r\n";
}

void THttpResp::ParseHttpResp(const PSIn& SIn){
  THttpLx Lx(SIn);
  if (Lx.Eof()){
    // no content
    MajorVerN=0; MinorVerN=9; StatusCd=204;
    HdStr.Clr(); BodyMem.Clr();
  } else {
    if (Lx.IsRespStatusLn()){
      // status-line
      Lx.GetToken(THttp::HttpStr); Lx.GetSpec(THttp::SlashStr);
      MajorVerN=Lx.GetInt(1); Lx.GetPeriod(); MinorVerN=Lx.GetInt(1);
      StatusCd=Lx.GetInt(3);
      ReasonPhrase=Lx.GetRespReasonPhrase();
      Lx.GetCrLf();
      // header fields & values
      while (!Lx.IsCrLf()){
        TStr FldNm=Lx.GetToken(); Lx.GetSpec(THttp::ColonStr);
        TStr FldVal=Lx.GetFldVal();
        Lx.GetCrLf();
        TStr NrFldNm=THttpLx::GetNrStr(FldNm);
        FldNmToValVH.AddDat(NrFldNm).Add(FldVal);
      }
      // separator CrLf
      Lx.GetCrLf();
      // header & body strings
      HdStr=Lx.GetMemSf().GetAsStr();
      Lx.ClrMemSf();
      Lx.GetRest();
      BodyMem=Lx.GetMemSf();
    } else {
      // old fashion format
      MajorVerN=0; MinorVerN=9; StatusCd=200;
      HdStr.Clr();
      Lx.ClrMemSf();
      Lx.GetRest();
      BodyMem=Lx.GetMemSf();
    }
  }
  Ok=true;
}

THttpResp::THttpResp(const int& _StatusCd, const TStr& ContTypeVal,
 const bool& CacheCtrlP, const PSIn& BodySIn, const TStr LocStr):
  Ok(true), MajorVerN(1), MinorVerN(0), StatusCd(_StatusCd), ReasonPhrase(),
  FldNmToValVH(20), HdStr(), BodyMem(){
  ReasonPhrase=THttp::GetReasonPhrase(StatusCd);
  TChA HdChA;
  // first line
  HdChA+="HTTP/"; HdChA+=TInt::GetStr(MajorVerN); HdChA+=".";
  HdChA+=TInt::GetStr(MinorVerN); HdChA+=' ';
  HdChA+=TInt::GetStr(StatusCd); HdChA+=' ';
  HdChA+=ReasonPhrase;
  HdChA+="\r\n";
  // header fields
  // server
  //AddHdFld(THttp::SrvFldNm, "Tralala", HdChA);
  if (!LocStr.Empty()){
    AddHdFld("Location", LocStr, HdChA);}
  if (!BodySIn.Empty()){
    // content-type
    AddHdFld(THttp::ContTypeFldNm, ContTypeVal, HdChA);
    // accept-ranges
    AddHdFld(THttp::AcceptRangesFldNm, "bytes", HdChA);
    // content-length
    TStr ContLenVal=TInt::GetStr(BodySIn->Len());
    AddHdFld(THttp::ContLenFldNm, ContLenVal, HdChA);
    // cache-control
    if (!CacheCtrlP){
      AddHdFld(THttp::CacheCtrlFldNm, "no-cache", HdChA);}
  }
  // header/body separator
  HdChA+="\r\n";
  // header/body data
  HdStr=HdChA;
  if (!BodySIn.Empty()){
    TMem::LoadMem(BodySIn, BodyMem);}
}

THttpResp::THttpResp(const PSIn& SIn):
  Ok(false), MajorVerN(0), MinorVerN(0), StatusCd(-1), ReasonPhrase(),
  FldNmToValVH(20), HdStr(), BodyMem(){
  try {
    ParseHttpResp(SIn);
  }
  catch (THttpEx){Ok=false;}
}

bool THttpResp::IsFldNm(const TStr& FldNm) const {
  return FldNmToValVH.IsKey(THttpLx::GetNrStr(FldNm));
}

TStr THttpResp::GetFldVal(const TStr& FldNm, const int& ValN) const {
  TStr NrFldNm=THttpLx::GetNrStr(FldNm);
  if (FldNmToValVH.IsKey(NrFldNm)){
    const TStrV& ValV=FldNmToValVH.GetDat(NrFldNm);
    if (ValV.Len()>0){return ValV[ValN];} else {return TStr();}
  } else {
    return TStr();
  }
}

void THttpResp::GetFldValV(const TStr& FldNm, TStrV& FldValV) const {
  TStr NrFldNm=THttpLx::GetNrStr(FldNm);
  if (FldNmToValVH.IsKey(NrFldNm)){
    FldValV=FldNmToValVH.GetDat(NrFldNm);
  } else {
    FldValV.Clr();
  }
}

bool THttpResp::IsFldVal(const TStr& FldNm, const TStr& FldVal) const {
  return THttpLx::GetNrStr(FldVal)==THttpLx::GetNrStr(GetFldVal(FldNm));
}

void THttpResp::AddFldVal(const TStr& FldNm, const TStr& FldVal){
  TStr NrFldNm=THttpLx::GetNrStr(FldNm);
  FldNmToValVH.AddDat(NrFldNm).Add(FldVal);
  if (HdStr.IsSuffix("\r\n\r\n")){
    TChA HdChA=HdStr;
    HdChA.Trunc(HdChA.Len()-2);
    HdChA+=NrFldNm; HdChA+=": "; HdChA+=FldVal;
    HdChA+="\r\n\r\n";
    HdStr=HdChA;
  }
}

void THttpResp::GetCookieKeyValDmPathQuV(TStrQuV& CookieKeyValDmPathQuV){
  CookieKeyValDmPathQuV.Clr();
  TStrV CookieFldValV; GetFldValV(THttp::SetCookieFldNm, CookieFldValV);
  for (int CookieN=0; CookieN<CookieFldValV.Len(); CookieN++){
    TStr CookieFldVal=CookieFldValV[CookieN];
    TStrV KeyValStrV;
    CookieFldVal.SplitOnAllCh(';', KeyValStrV, true);
    TStrPrV KeyValPrV; TStr DmNm; TStr PathStr;
    for (int KeyValStrN=0; KeyValStrN<KeyValStrV.Len(); KeyValStrN++){
      TStr KeyValStr=KeyValStrV[KeyValStrN];
      TStr KeyNm; TStr ValStr; 
      if (KeyValStr.IsChIn('=')){
        KeyValStrV[KeyValStrN].SplitOnCh(KeyNm, '=', ValStr);
        KeyNm.ToTrunc(); ValStr.ToTrunc();
      } else {
        KeyNm=KeyValStr.GetTrunc();
      }
      if (KeyNm=="expires"){}
      else if (KeyNm=="domain"){DmNm=ValStr;}
      else if (KeyNm=="path"){PathStr=ValStr;}
      else if (KeyNm=="expires"){}
      else if (KeyNm=="secure"){}
      else if (KeyNm=="httponly"){}
      else if (!KeyNm.Empty()){
        KeyValPrV.Add(TStrPr(KeyNm, ValStr));
      }
    }
    for (int KeyValPrN=0; KeyValPrN<KeyValPrV.Len(); KeyValPrN++){
      TStr KeyNm=KeyValPrV[KeyValPrN].Val1;
      TStr ValStr=KeyValPrV[KeyValPrN].Val2;
      CookieKeyValDmPathQuV.Add(TStrQu(KeyNm, ValStr, DmNm, PathStr));
    }
  }
}

PSIn THttpResp::GetSIn() const {
  TMOut MOut(HdStr.Len()+BodyMem.Len());
  MOut.PutStr(HdStr); MOut.PutMem(BodyMem);
  return MOut.GetSIn();
}

