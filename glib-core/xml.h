/////////////////////////////////////////////////
// Xml-Object-Serialization
class TXmlObjSer{
private:
  static TStrStrH TypeNmToTagNmH;
public:
  static TStr GetTagNm(const TStr& TypeNm);
  static void AssertXmlHd(
   const PXmlTok& XmlTok, const TStr& Nm, const TStr& TypeNm);
  static bool GetBoolArg(const PXmlTok& XmlTok, const TStr& Nm);
  static int GetIntArg(const PXmlTok& XmlTok, const TStr& Nm);
  static int64 GetInt64Arg(const PXmlTok& XmlTok, const TStr& Nm);
  static double GetFltArg(const PXmlTok& XmlTok, const TStr& Nm);
};

/////////////////////////////////////////////////
// Xml-Object-Serialization-Tag-Name
class TXmlObjSerTagNm{
private:
  TStr TagNm;
  TSOut* SOut;
  UndefDefaultCopyAssign(TXmlObjSerTagNm);
public:
  TXmlObjSerTagNm(
   TSOut& _SOut, const bool& ETagP,
   const TStr& Nm, const TStr& TypeNm,
   const TStr& ArgNm="", const TStr& ArgVal="");
  TXmlObjSerTagNm(
   TSOut& _SOut, const bool& ETagP,
   const TStr& Nm, const TStr& TypeNm,
   const TStr& ArgNm1, const TStr& ArgVal1,
   const TStr& ArgNm2, const TStr& ArgVal2,
   const TStr& ArgNm3="", const TStr& ArgVal3="",
   const TStr& ArgNm4="", const TStr& ArgVal4="");
  ~TXmlObjSerTagNm();
};

/////////////////////////////////////////////////
// Xml-Char-Definition
ClassTP(TXmlChDef, PXmlChDef)//{
private:
  TInt Chs;
  TBSet CharChSet, CombChSet, ExtChSet;
  TBSet LetterChSet, DigitChSet, NameChSet, PubidChSet;
  TStrStrH EntityNmToValH;
  void SetChTy(TBSet& ChSet, const int& MnCh, const int& MxCh=-1);
  void SetChTy(TBSet& ChSet, const TStr& Str);
  void SetEntityVal(const TStr& Nm, const TStr& Val);
public:
  TXmlChDef();
  TXmlChDef(TSIn& SIn):
    Chs(SIn),
    CharChSet(SIn), CombChSet(SIn), ExtChSet(SIn),
    LetterChSet(SIn), DigitChSet(SIn), NameChSet(SIn), PubidChSet(SIn),
    EntityNmToValH(SIn){}
  static PXmlChDef Load(TSIn& SIn){return new TXmlChDef(SIn);}
  void Save(TSOut& SOut){
    Chs.Save(SOut);
    CharChSet.Save(SOut); CombChSet.Save(SOut); ExtChSet.Save(SOut);
    LetterChSet.Save(SOut); DigitChSet.Save(SOut); NameChSet.Save(SOut);
    PubidChSet.Save(SOut);
    EntityNmToValH.Save(SOut);}

  TXmlChDef& operator=(const TXmlChDef&){Fail; return *this;}

  bool IsChar(const uchar& Ch) const {return CharChSet.GetBit(Ch);}
  bool IsComb(const uchar& Ch) const {return CombChSet.GetBit(Ch);}
  bool IsExt(const uchar& Ch) const {return ExtChSet.GetBit(Ch);}
  bool IsLetter(const uchar& Ch) const {return LetterChSet.GetBit(Ch);}
  bool IsDigit(const uchar& Ch) const {return DigitChSet.GetBit(Ch);}
  bool IsName(const uchar& Ch) const {return NameChSet.GetBit(Ch);}
  bool IsPubid(const uchar& Ch) const {return PubidChSet.GetBit(Ch);}

  bool IsWs(const uchar& Ch) const {
    return (Ch==' ')||(Ch==TCh::CrCh)||(Ch==TCh::LfCh)||(Ch==TCh::TabCh);}
  bool IsFirstNameCh(const uchar& Ch) const {
    return IsLetter(Ch)||(Ch=='_')||(Ch==':');}
  bool IsEoln(const uchar& Ch) const {
    return (Ch==TCh::CrCh)||(Ch==TCh::LfCh);}

  bool IsEntityNm(const TStr& EntityNm, TStr& EntityVal) const {
    return EntityNmToValH.IsKeyGetDat(EntityNm, EntityVal);}
};

/////////////////////////////////////////////////
// Xml-Lexical
typedef enum {
  xsyUndef, xsyWs, xsyComment,
  xsyXmlDecl, xsyPI,
  xsyDocTypeDecl, xsyElement, xsyAttList, xsyEntity, xsyNotation,
  xsyTag, xsySTag, xsyETag, xsySETag, xsyStr, xsyQStr,
  xsyEof} TXmlLxSym;

typedef enum {xspIntact, xspPreserve, xspSeparate, xspTruncate} TXmlSpacing;

class TXmlLx{
private: // character level functions
  static TXmlChDef ChDef;
  PSIn SIn;
  TSIn& RSIn;
  TChA ChStack;
  uchar PrevCh, Ch;
  int LnN, LnChN, ChN;
  TXmlSpacing Spacing;
  uchar GetCh();
  void PutCh(const uchar& _Ch){ChStack.Push(Ch); Ch=_Ch;}
  void PutStr(const TStr& Str){
    for (int ChN=Str.Len()-1; ChN>=0; ChN--){PutCh(Str[ChN]);}}
  void ToNrSpacing();
private: // part-of-symbol level functions
  TStrStrH EntityNmToValH, PEntityNmToValH;
  void GetWs(const bool& IsRq);
  TStr GetReference();
  void GetEq();
  TStr GetName();
  TStr GetName(const TStr& RqNm);
  void GetComment();
  TStr GetAttValue();
  TStr GetVersionNum();
  TStr GetEncName();
  TStr GetStalVal();
  void GetXmlDecl();
  void GetPI();
  TStr GetSystemLiteral();
  TStr GetPubidLiteral();
  TStr GetPEReference();
  void GetExternalId();
  void GetNData();
  void GetDocTypeDecl();
  void GetElement();
  void GetAttList();
  TStr GetEntityValue();
  void GetEntity();
  void GetNotation();
  void GetCDSect();
public: // symbol state
  TXmlLxSym Sym;
  TChA TxtChA;
  TStr TagNm;
  TStrKdV ArgNmValKdV;
  void SkipWs();
public:
  TXmlLx(const PSIn& _SIn, const TXmlSpacing& _Spacing=xspIntact):
    SIn(_SIn), RSIn(*SIn),
    ChStack(), PrevCh(' '), Ch(' '),
    LnN(1), LnChN(0), ChN(0), Spacing(_Spacing),
    EntityNmToValH(100), PEntityNmToValH(100),
    Sym(xsyUndef), TxtChA(), TagNm(), ArgNmValKdV(10){GetCh();}

  TXmlLx& operator=(const TXmlLx&){Fail; return *this;}

  TXmlLxSym GetSym();
  TStr GetSymStr() const;
  void EThrow(const TStr& MsgStr) const;
  TStr GetFPosStr() const;

  // tag arguments
  void ClrArgV(){ArgNmValKdV.Clr();}
  void AddArg(const TStr& ArgNm, const TStr& ArgVal){
    ArgNmValKdV.Add(TStrKd(ArgNm, ArgVal));}
  bool IsArgNm(const TStr& ArgNm) const {
    return ArgNmValKdV.IsIn(TStrKd(ArgNm));}
  int GetArgs() const {return ArgNmValKdV.Len();}
  void GetArg(const int& ArgN, TStr& ArgNm, TStr& ArgVal) const {
    ArgNm=ArgNmValKdV[ArgN].Key; ArgVal=ArgNmValKdV[ArgN].Dat;}
  TStr GetArgVal(const TStr& ArgNm, const TStr& DfVal=TStr()) const {
    int ArgN=ArgNmValKdV.SearchForw(TStrKd(ArgNm));
    if (ArgN==-1){return DfVal;} else {return ArgNmValKdV[ArgN].Dat;}}
  static char GetArgValQCh(const TStr& ArgVal){
    if (ArgVal.IsChIn('\"')){return '\'';} else {return '\"';}}

  // entities
  bool IsEntityNm(const TStr& EntityNm, TStr& EntityVal) const {
    return EntityNmToValH.IsKeyGetDat(EntityNm, EntityVal);}
  void PutEntityVal(const TStr& Nm, const TStr& Val){
    EntityNmToValH.AddDat(Nm, Val);}
  bool IsPEntityNm(const TStr& EntityNm, TStr& EntityVal) const {
    return PEntityNmToValH.IsKeyGetDat(EntityNm, EntityVal);}
  void PutPEntityVal(const TStr& Nm, const TStr& Val){
    PEntityNmToValH.AddDat(Nm, Val);}

  // symbol to string conversion
  static TStr GetXmlLxSymStr(const TXmlLxSym& XmlLxSym);

  // string conversion
  static bool IsTagNm(const TStr& Str);
  static TStr GetXmlStrFromPlainMem(const TMem& PlainMem);
  static TStr GetXmlStrFromPlainStr(const TChA& PlainChA);
  static TStr GetPlainStrFromXmlStr(const TStr& XmlStr);
  static TStr GetUsAsciiStrFromXmlStr(const TStr& EntRefStr);
  static TStr GetChRefFromYuEntRef(const TStr& YuEntRefStr);
};

/////////////////////////////////////////////////
// Xml-Token
ClassTPV(TXmlTok, PXmlTok, TXmlTokV)//{
private:
  TXmlLxSym Sym;
  TStr Str;
  TStrKdV ArgNmValV;
  TXmlTokV SubTokV;
public:
  TXmlTok(): Sym(xsyUndef), Str(), ArgNmValV(), SubTokV(){}
  TXmlTok(const TXmlLxSym& _Sym):
    Sym(_Sym), Str(), ArgNmValV(), SubTokV(){}
  TXmlTok(const TXmlLxSym& _Sym, const TStr& _Str):
    Sym(_Sym), Str(_Str), ArgNmValV(), SubTokV(){}
  TXmlTok(const TXmlLxSym& _Sym, const TStr& _Str, const TStrKdV& _ArgNmValV):
    Sym(_Sym), Str(_Str), ArgNmValV(_ArgNmValV){}
  static PXmlTok New(){return PXmlTok(new TXmlTok());}
  static PXmlTok New(const TXmlLxSym& Sym){return PXmlTok(new TXmlTok(Sym));}
  static PXmlTok New(const TXmlLxSym& Sym, const TStr& Str){
    return PXmlTok(new TXmlTok(Sym, Str));}
  static PXmlTok New(
   const TXmlLxSym& Sym, const TStr& Str, const TStrKdV& ArgNmValV){
    return PXmlTok(new TXmlTok(Sym, Str, ArgNmValV));}
  static PXmlTok New(
   const TStr& TagNm, const TStrKdV& ArgNmValV){
    return PXmlTok(new TXmlTok(xsyTag, TagNm, ArgNmValV));}
  static PXmlTok New(const TStr& TagNm){
    return PXmlTok(new TXmlTok(xsyTag, TagNm));}
  static PXmlTok New(const TStr& TagNm, const TStr& ValStr){
    PXmlTok TagXmlTok=PXmlTok(new TXmlTok(xsyTag, TagNm));
    PXmlTok StrXmlTok=PXmlTok(new TXmlTok(xsyStr, ValStr));
    TagXmlTok->AddSubTok(StrXmlTok); return TagXmlTok;}
  static PXmlTok New(const TStr& TagNm,
   const PXmlTok& XmlTok1, const PXmlTok& XmlTok2=NULL){
    PXmlTok TagXmlTok=PXmlTok(new TXmlTok(xsyTag, TagNm));
    if (!XmlTok1.Empty()){TagXmlTok->AddSubTok(XmlTok1);}
    if (!XmlTok2.Empty()){TagXmlTok->AddSubTok(XmlTok2);}
    return TagXmlTok;}
  TXmlTok(TSIn&){Fail;}
  static PXmlTok Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TXmlTok& operator=(const TXmlTok&){Fail; return *this;}

  // type-of-token and text value (xsyWs, xsyStr, xsyQStr, xsyTag)
  TXmlLxSym GetSym() const {return Sym;}
  TStr GetSymStr() const {return TXmlLx::GetXmlLxSymStr(Sym);}
  void PutStr(const TStr& _Str){Str=_Str;}
  TStr GetStr() const {return Str;}

  // tags
  bool IsTag() const {return (Sym==xsyTag);}
  bool IsTag(const TStr& TagNm) const {return (Sym==xsyTag)&&(Str==TagNm);}
  TStr GetTagNm() const {IAssert(Sym==xsyTag); return Str;}

  // tag argument
  void AddArg(const TStr& ArgNm, const bool& ArgVal){
    ArgNmValV.Add(TStrKd(ArgNm, TBool::GetStr(ArgVal)));}
  void AddArg(const TStr& ArgNm, const int& ArgVal){
    ArgNmValV.Add(TStrKd(ArgNm, TInt::GetStr(ArgVal)));}
  void AddArg(const TStr& ArgNm, const uint64& ArgVal){
    ArgNmValV.Add(TStrKd(ArgNm, TUInt64::GetStr(ArgVal)));}
  void AddArg(const TStr& ArgNm, const double& ArgVal){
    ArgNmValV.Add(TStrKd(ArgNm, TFlt::GetStr(ArgVal)));}
  void AddArg(const TStr& ArgNm, const TStr& ArgVal){
    ArgNmValV.Add(TStrKd(ArgNm, ArgVal));}
  int GetArgs() const {return ArgNmValV.Len();}
  void GetArg(const int& ArgN, TStr& ArgNm, TStr& ArgVal) const {
    ArgNm=ArgNmValV[ArgN].Key; ArgVal=ArgNmValV[ArgN].Dat;}
  bool IsArg(const TStr& ArgNm) const {
    return ArgNmValV.SearchForw(TStrKd(ArgNm))!=-1;}
  bool IsArg(const TStr& ArgNm, TStr& ArgVal) const {
    int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
    if (ArgN!=-1){ArgVal=ArgNmValV[ArgN].Dat;}
    return ArgN!=-1;}
  void PutArgVal(const TStr& ArgNm, const TStr& ArgVal){
    ArgNmValV[ArgNmValV.SearchForw(TStrKd(ArgNm))].Dat=ArgVal;}
  TStr GetArgVal(const TStr& ArgNm) const {
    return ArgNmValV[ArgNmValV.SearchForw(TStrKd(ArgNm))].Dat;}
  TStr GetArgVal(const TStr& ArgNm, const TStr& DfArgVal) const {
    int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
    return (ArgN==-1) ? DfArgVal : ArgNmValV[ArgN].Dat;}
  bool GetBoolArgVal(const TStr& ArgNm, const bool& DfVal=false) const;
  bool GetBoolArgVal(
   const TStr& ArgNm, const TStr& TrueVal, const bool& DfVal=false) const;
  bool GetBoolArgVal(const TStr& ArgNm,
   const TStr& TrueVal, const TStr& FalseVal, const bool& DfVal=false) const;
  int GetIntArgVal(const TStr& ArgNm, const int& DfVal=0) const;
  double GetFltArgVal(const TStr& ArgNm, const double& DfVal=0) const;
  TStr GetStrArgVal(const TStr& ArgNm, const TStr& DfVal=TStr()) const;

  // sub tokens
  bool IsSubTag(const TStr& TagNm) const {
    return !GetTagTok(TagNm).Empty();}
  bool IsSubTag(const TStr& TagNm, PXmlTok& Tok) const {
    Tok=GetTagTok(TagNm); return !Tok.Empty();}
  void AddSubTok(const PXmlTok& Tok){SubTokV.Add(Tok);}
  int GetSubToks() const {return SubTokV.Len();}
  PXmlTok GetSubTok(const int& SubTokN) const {return SubTokV[SubTokN];}
  void ClrSubTok(){SubTokV.Clr();}
  void PutSubTok(const PXmlTok& Tok, const int& SubTokN=-1);

  // value retrieval
  bool IsTagTok(const TStr& TagPath, PXmlTok& TagTok) const {
    TagTok=GetTagTok(TagPath); return !TagTok.Empty();}
  bool IsTagTok(const TStr& TagPath) const {
    PXmlTok TagTok; return IsTagTok(TagPath, TagTok);}
  PXmlTok GetTagTok(const TStr& TagPath) const;
  void GetTagTokV(const TStr& TagPath, TXmlTokV& XmlTokV) const;
  void GetTagValV(const TStr& TagNm, const bool& XmlP, TStrV& ValV) const;
  TStr GetTagVal(const TStr& TagNm, const bool& XmlP) const;
  TStr GetTagTokStr(const TStr& TagPath) const {
    return GetTagTok(TagPath)->GetTokStr(false);}
  TStr GetTagTokStrOrDf(const TStr& TagPath, const TStr& DfVal=TStr()) const {
    PXmlTok TagTok;
    if (IsTagTok(TagPath, TagTok)){return TagTok->GetTokStr(false);}
    else {return DfVal;}}

  // string representation for the token
  void AddTokToChA(const bool& XmlP, TChA& ChA) const;
  TStr GetTokStr(const bool& XmlP=true) const {
    TChA ChA; AddTokToChA(XmlP, ChA); return ChA;}
  static TStr GetTokVStr(const TXmlTokV& TokV, const bool& XmlP=true);

  // convert current lexical symbol to token
  static PXmlTok GetTok(TXmlLx& Lx);
};

/////////////////////////////////////////////////
// Xml-Document
ClassTPV(TXmlDoc, PXmlDoc, TXmlDocV)//{
private:
  bool Ok;
  TStr MsgStr;
  PXmlTok Tok;
  static void LoadTxtMiscStar(TXmlLx& Lx);
  static PXmlTok LoadTxtElement(TXmlLx& Lx);
public:
  TXmlDoc(): Ok(false), MsgStr(), Tok(){}
  static PXmlDoc New(){return PXmlDoc(new TXmlDoc());}
  TXmlDoc(const PXmlTok& _Tok): Ok(!_Tok.Empty()), MsgStr(), Tok(_Tok){}
  static PXmlDoc New(const PXmlTok& Tok){return PXmlDoc(new TXmlDoc(Tok));}
  TXmlDoc(TSIn&){Fail;}
  static PXmlDoc Load(TSIn& SIn){return new TXmlDoc(SIn);}
  void Save(TSOut&){Fail;}

  TXmlDoc& operator=(const TXmlDoc&){Fail; return *this;}

  // component retrieval
  bool IsOk() const {return Ok;}
  TStr GetMsgStr() const {return MsgStr;}
  PXmlTok GetTok() const {IAssert(Ok); return Tok;}

  // value retrieval
  bool IsTagTok(const TStr& TagPath, PXmlTok& TagTok) const {
    IAssert(Ok); TagTok=GetTagTok(TagPath); return !TagTok.Empty();}
  bool IsTagTok(const TStr& TagPath) const {
    PXmlTok TagTok; return IsTagTok(TagPath, TagTok);}
  PXmlTok GetTagTok(const TStr& TagPath) const;
  void PutTagTokStr(const TStr& TagPath, const TStr& TokStr) const;
  TStr GetTagTokStr(const TStr& TagPath) const {
    return GetTagTok(TagPath)->GetTokStr(false);}
  void GetTagTokV(const TStr& TagPath, TXmlTokV& XmlTokV) const;
  void GetTagValV(const TStr& TagNm, const bool& XmlP, TStrV& ValV) const {
    IAssert(Ok); Tok->GetTagValV(TagNm, XmlP, ValV);}
  TStr GetTagVal(const TStr& TagNm, const bool& XmlP) const {
    TStrV ValV; GetTagValV(TagNm, XmlP, ValV);
    if (ValV.Len()>0){return ValV[0];} else {return "";}}
  bool GetTagTokBoolArgVal(
   const TStr& TagPath, const TStr& ArgNm, const bool& DfVal=false) const;
  int GetTagTokIntArgVal(
   const TStr& TagPath, const TStr& ArgNm, const int& DfVal=0) const;
  double GetTagTokFltArgVal(
   const TStr& TagPath, const TStr& ArgNm, const double& DfVal=0) const;
  TStr GetTagTokStrArgVal(
   const TStr& TagPath, const TStr& ArgNm, const TStr& DfVal=TStr()) const;

  // string representation
  static TStr GetXmlStr(const TStr& Str);

  // loading/saving document
  static bool SkipTopTag(const PSIn& SIn);
  static PXmlDoc LoadTxt(TXmlLx& Lx);
  static PXmlDoc LoadTxt(const PSIn& SIn, const TXmlSpacing& Spacing=xspIntact);
  static PXmlDoc LoadTxt(const TStr& FNm, const TXmlSpacing& Spacing=xspIntact);
  static void LoadTxt(
   const TStr& FNm, TXmlDocV& XmlDocV, const TXmlSpacing& Spacing=xspIntact);
  void SaveTxt(const PSOut& SOut){
    SOut->PutStr(GetTok()->GetTokStr());}
  void SaveTxt(const TStr& FNm, const bool& Append=false){
    PSOut SOut=TFOut::New(FNm, Append); SaveTxt(SOut);}
  static PXmlDoc LoadStr(const TStr& Str);
  void SaveStr(TStr& Str);
};

/////////////////////////////////////////////////
// Fast and dirty XML parser 
// very basic it does only <item>string</item>, no comments, no arguments
class TXmlParser;
typedef TPt<TXmlParser> PXmlParser;

class TXmlParser {  //J:
private:
  TCRef CRef;
  PSIn SIn;
  TSIn& RSIn;
  TChA _SymStr;
public:
  TXmlLxSym Sym, NextSym;
  TChA SymStr, NextSymStr;
private:
  char GetCh() { return (! RSIn.Eof()) ? RSIn.GetCh() : TCh::EofCh; }
public:
  TXmlParser(const PSIn& _SIn) : SIn(_SIn), RSIn(*SIn), Sym(xsyUndef), NextSym(xsyUndef) { }
  static PXmlParser New(const PSIn& SIn) { return new TXmlParser(SIn); }

  TXmlLxSym GetSym();
  TXmlLxSym GetSym(TChA& _SymStr);
  TXmlLxSym PeekSym();
  TXmlLxSym PeekSym(TChA& _SymStr);
  void SkipTillTag(const TChA& _SymStr);
  
  TXmlLxSym GetTag(const TChA& TagStr);
  void GetTagVal(const TChA& TagStr, TChA& TagVal);
  
  static void GetPlainStrFromXmlStr(const TChA& XmlStr, TChA& PlainChA);
  friend class TPt<TXmlParser>;
};
