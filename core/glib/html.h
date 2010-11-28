/////////////////////////////////////////////////
// Forward
ClassHdTP(THtmlTok, PHtmlTok)
ClassHdTP(THtmlDoc, PHtmlDoc)

/////////////////////////////////////////////////
// Html-Lexical-Chars
typedef enum {
  hlctSpace, hlctAlpha, hlctNum, hlctSym,
  hlctLTag, hlctRTag, hlctEof} THtmlLxChTy;

ClassTP(THtmlLxChDef, PHtmlLxChDef)//{
private:
  TIntV ChTyV;
  TChV UcChV;
  TChV LcChV;
  TStrStrH EscStrH;
  void SetUcCh(const char& UcCh, const char& LcCh);
  void SetUcCh(const TStr& Str);
  void SetChTy(const THtmlLxChTy& ChTy, const TStr& Str);
  void SetEscStr(const TStr& SrcStr, const TStr& DstStr);
public:
  THtmlLxChDef();
  THtmlLxChDef(TSIn& SIn): ChTyV(SIn), UcChV(SIn), LcChV(SIn), EscStrH(SIn){}
  static PHtmlLxChDef Load(TSIn& SIn){return new THtmlLxChDef(SIn);}
  void Save(TSOut& SOut){
    ChTyV.Save(SOut); UcChV.Save(SOut); LcChV.Save(SOut); EscStrH.Save(SOut);}

  THtmlLxChDef& operator=(const THtmlLxChDef&){Fail; return *this;}

  // character type operations
  int GetChTy(const char& Ch) const {return ChTyV[Ch-TCh::Mn];}
  bool IsEoln(const char& Ch) const {return (Ch==TCh::CrCh)||(Ch==TCh::LfCh);}
  bool IsWs(const char& Ch) const {
    return (Ch==' ')||(Ch==TCh::TabCh)||(Ch==TCh::CrCh)||(Ch==TCh::LfCh);}
  bool IsSpace(const char& Ch) const {return int(ChTyV[Ch-TCh::Mn])==hlctSpace;}
  bool IsAlpha(const char& Ch) const {return int(ChTyV[Ch-TCh::Mn])==hlctAlpha;}
  bool IsNum(const char& Ch) const {return int(ChTyV[Ch-TCh::Mn])==hlctNum;}
  bool IsAlNum(const char& Ch) const {
    return (int(ChTyV[Ch-TCh::Mn])==hlctAlpha)||(int(ChTyV[Ch-TCh::Mn])==hlctNum);}
  bool IsSym(const char& Ch) const {return int(ChTyV[Ch-TCh::Mn])==hlctSym;}
  bool IsUrl(const char& Ch) const {
    int ChTy=ChTyV[Ch-TCh::Mn];
    return (ChTy==hlctAlpha)||(ChTy==hlctNum)||
     (Ch=='.')||(Ch=='-')||(Ch==':')||(Ch=='/')||(Ch=='~');}

  // upper/lower-case & escape-string operations
  bool IsUc(const char& Ch) const {return Ch==UcChV[Ch-TCh::Mn];}
  bool IsLc(const char& Ch) const {return Ch==LcChV[Ch-TCh::Mn];}
  char GetUc(const char& Ch) const {return UcChV[Ch-TCh::Mn];}
  char GetLc(const char& Ch) const {return LcChV[Ch-TCh::Mn];}
  void GetUcChA(TChA& ChA) const {
    for (int ChN=0; ChN<ChA.Len(); ChN++){ChA.PutCh(ChN, GetUc(ChA[ChN]));}}
  void GetLcChA(TChA& ChA) const {
    for (int ChN=0; ChN<ChA.Len(); ChN++){ChA.PutCh(ChN, GetLc(ChA[ChN]));}}
  TStr GetUcStr(const TStr& Str) const {
    TChA ChA(Str); GetUcChA(ChA); return ChA;}
  TStr GetLcStr(const TStr& Str) const {
    TChA ChA(Str); GetLcChA(ChA); return ChA;}
  TStr GetEscStr(const TStr& Str) const;

  // standard entry points
  static PHtmlLxChDef ChDef;
  static PHtmlLxChDef GetChDef(){IAssert(!ChDef.Empty()); return ChDef;}
  static THtmlLxChDef& GetChDefRef(){IAssert(!ChDef.Empty()); return *ChDef;}

  // character-set transformations
  static TStr GetCSZFromYuascii(const TChA& ChA);
  static TStr GetCSZFromWin1250(const TChA& ChA);
  static TStr GetWin1250FromYuascii(const TChA& ChA);
  static TStr GetIsoCeFromYuascii(const TChA& ChA);
};

/////////////////////////////////////////////////
// Html-Lexical
typedef enum {
  hsyUndef, hsyStr, hsyNum, hsySSym, hsyUrl,
  hsyBTag, hsyETag, hsyMTag, hsyEof} THtmlLxSym;

class THtmlLx{
private:
  static THtmlLxChDef ChDef;
  PSIn SIn;
  TSIn& RSIn;
  bool DoParseArg;
  TChA ChStack;
  char Ch;
  int ChX;
  bool EscCh;
  TChA EscChA;
  TChA ArgNm;
  TChA ArgVal;
  void GetCh(){
    if (ChStack.Empty()){
      if (RSIn.Eof()){Ch=TCh::EofCh;} else {Ch=RSIn.GetCh(); ChX++;}
    } else {
      Ch=ChStack.Pop(); ChX++;
    }
    SymChA+=Ch;
  }
  void GetEscCh();
  void GetMetaTag();
  void GetTag();
public:
  THtmlLxSym Sym;
  int SymBChX, SymEChX;
  TChA ChA;
  TChA UcChA;
  TChA SymChA;
  int PreSpaces;
  TChA PreSpaceChA;
  typedef TStrKdV TArgNmValV;
  TArgNmValV ArgNmValV;
public:
  THtmlLx(const PSIn& _SIn, const bool& _DoParseArg=true):
    SIn(_SIn), RSIn(*SIn), DoParseArg(_DoParseArg),
    ChStack(), Ch(' '), ChX(0), EscCh(false),
    EscChA(), ArgNm(), ArgVal(),
    Sym(hsyUndef), SymBChX(0), SymEChX(0), ChA(), UcChA(),
    PreSpaces(0), PreSpaceChA(), ArgNmValV(){}

  THtmlLx& operator=(const THtmlLx&){Fail; return *this;}

  void PutCh(const char& _Ch){
    ChStack.Push(Ch); if (!SymChA.Empty()){SymChA.Pop();} Ch=_Ch; ChX--;}
  void PutStr(const TStr& Str){
    for (int ChN=Str.Len()-1; ChN>=0; ChN--){PutCh(Str[ChN]);}}
  THtmlLxSym GetSym();
  PHtmlTok GetTok(const bool& DoUc=true);
  TStr GetPreSpaceStr() const {
    return TStr::GetSpaceStr(PreSpaces);}

  int GetArgs() const {return ArgNmValV.Len();}
  TStr GetArgNm(const int& ArgN) const {return ArgNmValV[ArgN].Key;}
  TStr GetArgVal(const int& ArgN) const {return ArgNmValV[ArgN].Dat;}
  bool IsArg(const TStr& ArgNm) const {return ArgNmValV.IsIn(TStrKd(ArgNm));}
  TStr GetArg(const TStr& ArgNm, const TStr& DfArgVal=TStr()) const {
    int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
    if (ArgN==-1){return DfArgVal;} else {return ArgNmValV[ArgN].Dat;}}
  void PutArg(const TStr& ArgNm, const TStr& ArgVal){
    int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
    if (ArgN==-1){ArgNmValV.Add(TStrKd(ArgNm, ArgVal));}
    else {ArgNmValV[ArgN]=TStrKd(ArgNm, ArgVal);}}
  TStr GetFullBTagStr() const;

  void MoveToStrOrEof(const TStr& Str);
  void MoveToBTagOrEof(const TStr& TagNm);
  void MoveToBTag2OrEof(const TStr& TagNm1, const TStr& TagNm2);
  void MoveToBTag3OrEof(const TStr& TagNm1, const TStr& TagNm2, const TStr& TagNm3);
  void MoveToBTagArgOrEof(
   const TStr& TagNm, const TStr& ArgNm, const TStr& ArgVal);
  void MoveToBTagArg2OrEof(const TStr& TagNm,
   const TStr& ArgNm1, const TStr& ArgVal1,
   const TStr& ArgNm2, const TStr& ArgVal2);
  void MoveToBTagOrEof(
   const TStr& TagNm1, const TStr& ArgNm1, const TStr& ArgVal1,
   const TStr& TagNm2, const TStr& ArgNm2, const TStr& ArgVal2);
  void MoveToETagOrEof(const TStr& TagNm);
  TStr GetStrToBTag(const TStr& TagNm, const bool& TxtOnlyP=false);
  TStr GetStrToBTag(const TStr& TagNm, const TStr& ArgNm,
   const TStr& ArgVal, const bool& TxtOnlyP=false);
  TStr GetStrToETag(const TStr& TagNm, const bool& TxtOnlyP=false);
  TStr GetStrInTag(const TStr& TagNm, const bool& TxtOnlyP=false);
  TStr GetHRefBeforeStr(const TStr& Str);
  bool IsGetBTag(const TStr& TagNm);

  static TStr GetSymStr(const THtmlLxSym& Sym);
  static TStr GetEscapedStr(const TChA& ChA);
  static TStr GetAsciiStr(const TChA& ChA, const char& GenericCh='_');
  static void GetTokStrV(const TStr& Str, TStrV& TokStrV);
};

/////////////////////////////////////////////////
// Html-Token
ClassTPV(THtmlTok, PHtmlTok, THtmlTokV)//{
private:
  THtmlLxSym Sym;
  TStr Str;
  THtmlLx::TArgNmValV ArgNmValV;
public:
  THtmlTok(): Sym(hsyUndef), Str(), ArgNmValV(){}
  THtmlTok(const THtmlLxSym& _Sym):
    Sym(_Sym), Str(), ArgNmValV(){}
  THtmlTok(const THtmlLxSym& _Sym, const TStr& _Str):
    Sym(_Sym), Str(_Str), ArgNmValV(){}
  THtmlTok(const THtmlLxSym& _Sym, const TStr& _Str,
   const THtmlLx::TArgNmValV& _ArgNmValV):
    Sym(_Sym), Str(_Str), ArgNmValV(_ArgNmValV){}
  THtmlTok(TSIn&){Fail;}
  static PHtmlTok Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  THtmlTok& operator=(const THtmlTok&){Fail; return *this;}

  THtmlLxSym GetSym() const {return Sym;}
  TStr GetStr() const {return Str;}
  TStr GetFullStr() const;
  bool IsArg(const TStr& ArgNm) const {
    return ArgNmValV.SearchForw(TStrKd(ArgNm))!=-1;}
  TStr GetArg(const TStr& ArgNm) const {
    return ArgNmValV[ArgNmValV.SearchForw(TStrKd(ArgNm))].Dat;}
  TStr GetArg(const TStr& ArgNm, const TStr& DfArgVal) const {
    int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
    if (ArgN==-1){return DfArgVal;} else {return ArgNmValV[ArgN].Dat;}}
  bool IsUrlTok(TStr& RelUrlStr) const;
  bool IsRedirUrlTok() const;

  void SaveTxt(const PSOut& SOut, const bool& TxtMode=true);

  static const TStr ATagNm;
  static const TStr AreaTagNm;
  static const TStr BrTagNm;
  static const TStr CardTagNm;
  static const TStr CenterTagNm;
  static const TStr FrameTagNm;
  static const TStr H1TagNm;
  static const TStr H2TagNm;
  static const TStr H3TagNm;
  static const TStr H4TagNm;
  static const TStr H5TagNm;
  static const TStr H6TagNm;
  static const TStr ImgTagNm;
  static const TStr LiTagNm;
  static const TStr MetaTagNm;
  static const TStr PTagNm;
  static const TStr UlTagNm;
  static const TStr TitleTagNm;
  static const TStr TitleETagNm;

  static const TStr AltArgNm;
  static const TStr HRefArgNm;
  static const TStr SrcArgNm;
  static const TStr TitleArgNm;
  static const TStr HttpEquivArgNm;

  static bool IsBreakTag(const TStr& TagNm);
  static bool IsBreakTok(const PHtmlTok& Tok);
  static bool IsHTag(const TStr& TagNm, int& HTagN);
  static PHtmlTok GetHTok(const bool& IsBTag, const int& HTagN);
};

/////////////////////////////////////////////////
// Html-Document
typedef enum {
  hdtAll, hdtStr, hdtStrNum, hdtTag, hdtA, hdtHRef, hdtUL} THtmlDocType;

ClassTPV(THtmlDoc, PHtmlDoc, THtmlDocV)//{
private:
  THtmlTokV TokV;
public:
  THtmlDoc(): TokV(){}
  THtmlDoc(
   const PSIn& SIn, const THtmlDocType& Type=hdtAll, const bool& DoUc=true);
  static PHtmlDoc New(
   const PSIn& SIn, const THtmlDocType& Type=hdtAll, const bool& DoUc=true){
    return PHtmlDoc(new THtmlDoc(SIn, Type, DoUc));}
  THtmlDoc(TSIn&){Fail;}
  static PHtmlDoc Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  THtmlDoc& operator=(const THtmlDoc&){Fail; return *this;}

  int GetToks() const {return TokV.Len();}
  PHtmlTok GetTok(const int& TokN) const {return TokV[TokN];}
  PHtmlTok GetTok(const int& TokN, THtmlLxSym& Sym, TStr& Str) const {
    Sym=TokV[TokN]->GetSym(); Str=TokV[TokN]->GetStr(); return TokV[TokN];}
  void AddTokV(const THtmlTokV& _TokV){TokV.AddV(_TokV);}

  static TStr GetTxtLnDoc(const TStr& HtmlStr);
  static TStr GetTxtLnDoc(const TStr& HtmlStr, const TStr& BaseUrlStr,
   const bool& OutUrlP, const bool& OutTagsP);

  static PHtmlDoc LoadTxt(
   const TStr& FNm, const THtmlDocType& Type=hdtAll, const bool& DoUc=true){
    PSIn SIn=TFIn::New(FNm); return PHtmlDoc(new THtmlDoc(SIn, Type, DoUc));}
  void SaveTxt(const PSOut& SOut, const bool& TxtMode=true) const;

  static void SaveHtmlToTxt(
   const TStr& HtmlStr, const PSOut& TxtSOut, const TStr& BaseUrlStr,
   const bool& OutUrlP, const bool& OutToksP);
  static void SaveHtmlToTxt(
   const TStr& HtmlStr, const TStr& TxtFNm, const TStr& BaseUrlStr,
   const bool& OutUrlP, const bool& OutToksP);
  static void SaveHtmlToXml(
   const TStr& HtmlStr, const PSOut& XmlSOut, const TStr& BaseUrlStr,
   const bool& OutTextP, const bool& OutUrlP, const bool& OutToksP,
   const bool& OutTagsP, const bool& OutArgsP);
  static void SaveHtmlToXml(
   const TStr& HtmlStr, const TStr& XmlFNm, const TStr& BaseUrlStr,
   const bool& OutTextP, const bool& OutUrlP, const bool& OutToksP,
   const bool& OutTagsP, const bool& OutArgsP);

  static TLxSym GetLxSym(const THtmlLxSym& HtmlLxSym, const TChA& ChA);

  static bool _IsTagRedir(
   const TStr& TagStr, const TStr& ArgNm, THtmlLx& Lx,
   const TStr& BaseUrlStr, const TStr& RedirUrlStr);
  static TStr GetRedirHtmlDocStr(const TStr& HtmlStr,
   const TStr& BaseUrlStr, const TStr& RedirUrlStr);
};

/////////////////////////////////////////////////
// Html-Hyper-Link-Document-Vector
ClassTP(THtmlHldV, PHtmlHldV)//{
private:
  PHtmlDoc RefHtmlDoc;
  THtmlDocV HldV;
public:
  THtmlHldV(const PHtmlDoc& _RefHtmlDoc, const int& HldWnLen=10);
  THtmlHldV(TSIn&){Fail;}
  static PHtmlHldV Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  THtmlHldV& operator=(const THtmlHldV&){Fail; return *this;}

  PHtmlDoc GetRefHtmlDoc(){return RefHtmlDoc;}
  int GetHlds(){return HldV.Len();}
  PHtmlDoc GetHld(const int& HldN){return HldV[HldN];}
};

/////////////////////////////////////////////////
// Web-Page
ClassTPV(TWebPg, PWebPg, TWebPgV)//{
private:
  TStrV UrlStrV;
  TStrV IpNumV;
  PHttpResp HttpResp;
  uint64 FetchMSecs;
public:
  TWebPg(): UrlStrV(), IpNumV(), HttpResp(){}
  TWebPg(const TStrV& _UrlStrV, const TStrV& _IpNumV, const PHttpResp& _HttpResp):
    UrlStrV(_UrlStrV), IpNumV(_IpNumV), HttpResp(_HttpResp){}
  static PWebPg New(const TStrV& UrlStrV, const TStrV& IpNumV, const PHttpResp& HttpResp){
    return new TWebPg(UrlStrV, IpNumV, HttpResp);}
  static PWebPg New(const TStrV& UrlStrV, const PHttpResp& HttpResp){
    return new TWebPg(UrlStrV, TStrV(), HttpResp);}
  static PWebPg New(const TStr& UrlStr, const PHttpResp& HttpResp){
    TStrV UrlStrV; UrlStrV.Add(UrlStr);
    return new TWebPg(UrlStrV, TStrV(), HttpResp);}
  ~TWebPg(){}
  TWebPg(TSIn&){Fail;}
  static PWebPg Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TWebPg& operator=(const TWebPg&){Fail; return *this;}

  int GetUrls() const {return UrlStrV.Len();}
  TStr GetUrlStr(const int& UrlN=-1) const {
    if (UrlN==-1){return UrlStrV.Last();} else {return UrlStrV[UrlN];}}
  PUrl GetUrl(const int& UrlN=-1) const {
    TStr UrlStr;
    if (UrlN==-1){UrlStr=UrlStrV.Last();} else {UrlStr=UrlStrV[UrlN];}
    return TUrl::New(UrlStr);}

  int GetIps() const {return IpNumV.Len();}
  TStr GetIpNum(const int& IpN=-1) const {
    if (IpN==-1){return IpNumV.Last();} else {return IpNumV[IpN];}}

  PHttpResp GetHttpResp() const {return HttpResp;}
  TStr GetHttpHdStr() const {return GetHttpResp()->GetHdStr();}
  TStr GetHttpBodyAsStr() const {return GetHttpResp()->GetBodyAsStr();}
  //void GetOutUrlStrV(TStrV& OutUrlStrV) const;
  void GetOutUrlV(TUrlV& OutUrlV, TUrlV& OutRedirUrlV) const;
  void GetOutUrlV(TUrlV& OutUrlV) const {
    TUrlV OutRedirUrlV; GetOutUrlV(OutUrlV, OutRedirUrlV);}
  void GetOutDescUrlStrKdV(TStrKdV& OutDescUrlStrKdV) const;

  // fetch time
  void PutFetchMSecs(const uint64& _FetchMSecs){FetchMSecs=_FetchMSecs;}
  uint64 GetFetchMSecs() const {return FetchMSecs;}

  void SaveAsHttpBody(const TStr& FNm) const;
  void SaveAsHttp(const TStr& FNm) const;

  bool IsTxt() const;
};
