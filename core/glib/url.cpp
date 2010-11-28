/////////////////////////////////////////////////
// Url-Lexical-Chars
class TUrlLxChDef{
private:
  TBoolV IsLoAlphaV, IsHiAlphaV, IsAlphaV;
  TBoolV IsDigitV, IsSafeV, IsExtraV;
  TBoolV IsNationalV, IsPunctuationV;
  TBoolV IsReservedV, IsHexV;
  TBoolV IsUnreservedV, IsUCharV, IsXCharV;
  TBoolV IsSchemeV, IsHostV, IsHSegmentV;
  void InclCh(TBoolV& BoolV, const char& Ch);
  void InclStr(TBoolV& BoolV, const TStr& Str);
  void InclBoolV(TBoolV& BoolV, const TBoolV& OrBoolV);
public:
  static const char EofCh;
  static const char EscCh;
  TUrlLxChDef();

  bool IsDigitCh(const char& Ch) const {return (Ch>=0)&&IsDigitV[Ch];}
  bool IsSchemeCh(const char& Ch) const {return (Ch>=0)&&IsSchemeV[Ch];}
  bool IsHostCh(const char& Ch) const {return (Ch>=0)&&IsHostV[Ch];}
  bool IsHSegmentCh(const char& Ch) const {
    return (Ch<0)||((Ch>=0)&&IsHSegmentV[Ch]);}
};
const char TUrlLxChDef::EofCh=0;
const char TUrlLxChDef::EscCh='%';

void TUrlLxChDef::InclCh(TBoolV& BoolV, const char& Ch){BoolV[Ch]=true;}

void TUrlLxChDef::InclStr(TBoolV& BoolV, const TStr& Str){
  for (int CC=0; CC<Str.Len(); CC++){BoolV[Str.GetCh(CC)]=true;}}

void TUrlLxChDef::InclBoolV(TBoolV& BoolV, const TBoolV& OrBoolV){
  for (int BoolN=0; BoolN<BoolV.Len(); BoolN++){
    BoolV[BoolN]=BoolV[BoolN]||OrBoolV[BoolN];}}

TUrlLxChDef::TUrlLxChDef():
  IsLoAlphaV(TCh::Vals), IsHiAlphaV(TCh::Vals), IsAlphaV(TCh::Vals),
  IsDigitV(TCh::Vals), IsSafeV(TCh::Vals), IsExtraV(TCh::Vals),
  IsNationalV(TCh::Vals), IsPunctuationV(TCh::Vals),
  IsReservedV(TCh::Vals), IsHexV(TCh::Vals),
  IsUnreservedV(TCh::Vals), IsUCharV(TCh::Vals), IsXCharV(TCh::Vals),
  IsSchemeV(TCh::Vals), IsHostV(TCh::Vals), IsHSegmentV(TCh::Vals){

  InclStr(IsLoAlphaV, "abcdefghijklmnopqrstuvwxyz");
  InclStr(IsHiAlphaV, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  InclBoolV(IsAlphaV, IsLoAlphaV); InclBoolV(IsAlphaV, IsHiAlphaV);
  InclStr(IsDigitV, "0123456789");
  InclStr(IsSafeV, "$-_.+");
  InclStr(IsExtraV, "!*'(),");
  InclStr(IsNationalV, "{}|\\^~[]`");
  InclStr(IsPunctuationV, "<>#%\"");
  InclStr(IsReservedV, ";/?:@&=");
  InclBoolV(IsHexV, IsDigitV); InclStr(IsHexV, "ABCDEFabcdef");

  InclBoolV(IsUnreservedV, IsAlphaV); InclBoolV(IsUnreservedV, IsDigitV);
  InclBoolV(IsUnreservedV, IsSafeV); InclBoolV(IsUnreservedV, IsExtraV);

  InclBoolV(IsUCharV, IsUnreservedV); InclStr(IsUCharV, TStr(EscCh));

  InclBoolV(IsXCharV, IsUnreservedV); InclBoolV(IsXCharV, IsReservedV);
  InclStr(IsXCharV, TStr(EscCh));

  InclBoolV(IsSchemeV, IsAlphaV); InclBoolV(IsSchemeV, IsDigitV);
  InclStr(IsSchemeV, "+-.");

  InclBoolV(IsHostV, IsAlphaV); InclBoolV(IsHostV, IsDigitV);
  InclStr(IsHostV, "-_");

  InclBoolV(IsHSegmentV, IsUCharV); InclStr(IsHSegmentV, ";:@&=");
  InclBoolV(IsHSegmentV, IsNationalV); InclStr(IsHSegmentV, " ");
}

/////////////////////////////////////////////////
// Url-Lexical
class TUrlLx{
private:
  static const char EofCh;
  TChA Bf;
  int BfC;
public:
  static const TUrlLxChDef ChDef;
  TUrlLx(const TStr& _Str): Bf(_Str), BfC(0){}
  bool Eof() const {return BfC==Bf.Len();};
  char GetCh(){if (Eof()){return EofCh;} else {return Bf[BfC++];}}
  char PeekCh() const {if (Eof()){return EofCh;} else {return Bf[BfC];}}
  char GetCh(const char& Ch){EAssertR(GetCh()==Ch, ""); return Ch;}
  TStr GetStr(const TStr& Str){
    for (int ChN=0; ChN<Str.Len(); ChN++){GetCh(Str[ChN]);} return Str;}

  bool IsSchemeCh() const {return ChDef.IsSchemeCh(PeekCh());}
  char GetSchemeCh(){EAssertR(IsSchemeCh(), ""); return GetCh();}
  bool IsDigitCh() const {return ChDef.IsDigitCh(PeekCh());}
  char GetDigitCh(){EAssertR(IsDigitCh(), ""); return GetCh();}
  bool IsHSegmentCh() const {return ChDef.IsHSegmentCh(PeekCh());}
  char GetHSegmentCh(){EAssertR(IsHSegmentCh(), ""); return GetCh();}
  TStr GetToCh(const char& Ch=TUrlLxChDef::EofCh){TChA Str;
    while ((PeekCh()!=EofCh)&&(PeekCh()!=Ch)){Str+=GetCh();} return Str;}

  TStr GetScheme(){TChA Str;
    Str+=GetSchemeCh(); while (IsSchemeCh()){Str+=GetCh();}
    Str.ToLc(); return Str;}
  TStr GetHost();
  TStr GetDigits(){TChA Str;
    do {Str+=GetDigitCh();} while (IsDigitCh()); return Str;}
  TStr GetHostPort(TStr& HostNm, TStr& PortStr, int& PortN);
  TStr GetHPath(TStrV& PathSegV);
  TStr GetSearch(){return GetToCh('#');}
};
const TUrlLxChDef TUrlLx::ChDef;
const char TUrlLx::EofCh=TUrlLxChDef::EofCh;

TStr TUrlLx::GetHost(){TChA Str;
  EAssertR(ChDef.IsHostCh(PeekCh()), "");
  do {
    while (ChDef.IsHostCh(PeekCh())){Str+=GetCh();}
    if (PeekCh()=='.'){Str+=GetCh('.');}
    else if (PeekCh()=='@'){GetCh('@'); Str.Clr();} // still unexplained
  } while (ChDef.IsHostCh(PeekCh()));
  Str.ToLc();
  return Str;
}

TStr TUrlLx::GetHostPort(TStr& HostNm, TStr& PortStr, int& PortN){TChA Str;
  Str+=HostNm=GetHost();
  if (PeekCh()==':'){
    Str+=GetCh(':');
    if (IsDigitCh()){Str+=PortStr=GetDigits(); PortN=PortStr.GetInt();}
  }
  return Str;
}

TStr TUrlLx::GetHPath(TStrV& PathSegV){TChA Str; TChA HSegStr; bool Cont;
  do {
    while (PeekCh()=='/'){GetCh('/');} // prevent multiple '/'
    HSegStr.Clr(); while (IsHSegmentCh()){HSegStr+=GetHSegmentCh();}
    Str+=HSegStr; PathSegV.Add(HSegStr);
    Cont=(PeekCh()=='/'); if (Cont){Str+=GetCh('/');}
  } while (Cont);
  return Str;
}

/////////////////////////////////////////////////
// Url
const TStr TUrl::UrlHttpPrefixStr="http:";
const TStr TUrl::UrlHttpAbsPrefixStr="http://";

void TUrl::GetAbs(const TStr& AbsUrlStr){
  EAssertR(IsAbs(AbsUrlStr), AbsUrlStr);
  TUrlLx Lx(AbsUrlStr); TChA Str;
  Str+=SchemeNm=Lx.GetScheme(); Str+=Lx.GetCh(':');
  if (SchemeNm=="http"){
    Scheme=usHttp;
    static const TStr DbSlashStr="//";
    Str+=Lx.GetStr(DbSlashStr);
    Str+=Lx.GetHostPort(HostNm, PortStr, PortN);
    if (PortN==-1){PortN=THttp::DfPortN; PortStr.Clr();}
    else if (PortN==THttp::DfPortN){PortStr.Clr();}
    //**if (!PortStr.Empty()){Str+=':'; Str+=PortStr;}
    if (Lx.PeekCh()=='/'){
      PathStr=Lx.GetCh('/'); PathStr+=Lx.GetHPath(PathSegV); Str+=PathStr;}
    if (PathStr.Empty()){PathStr="/"; Str+=PathStr;}
    if (Lx.PeekCh()=='?'){
      SearchStr=Lx.GetCh('?'); SearchStr+=Lx.GetSearch(); Str+=SearchStr;}
  } else {
    Scheme=usOther; Str+=Lx.GetToCh();
  }
  while (Lx.PeekCh()==' '){Lx.GetCh();}
  if (Lx.PeekCh()=='#'){
    FragIdStr=Lx.GetCh('#'); FragIdStr+=Lx.GetToCh();
  }
  EAssertR(Lx.Eof(), "");
  UrlStr=Str;
}

void TUrl::GetAbsFromBase(const TStr& RelUrlStr, const TStr& BaseUrlStr){
  EAssertR(!BaseUrlStr.Empty(), "");
  PUrl Url=TUrl::New(BaseUrlStr); EAssertR(Url->IsOk(), "");
  EAssertR(IsAbs(BaseUrlStr), "");
  TStr AbsUrlStr=BaseUrlStr;
  TStr NrRelUrlStr=RelUrlStr;
  if (NrRelUrlStr.GetLc().IsPrefix(UrlHttpPrefixStr)){
    NrRelUrlStr.DelSubStr(0, UrlHttpPrefixStr.Len()-1);}
  if (NrRelUrlStr.Len()>0){
    if (NrRelUrlStr[0]=='/'){
      TStr SlashStr; int SlashChN=0;
      while ((SlashChN<NrRelUrlStr.Len())&&(NrRelUrlStr[SlashChN]=='/')){
        SlashChN++; SlashStr+="/";}
      int ChN=0; bool Found=false;
      while ((!Found)&&((ChN=AbsUrlStr.SearchStr(SlashStr, ChN))!=-1)){
        TStr Str=AbsUrlStr.GetSubStr(ChN-1, ChN+SlashStr.Len()-1+1);
        Found=((ChN==0)||(Str[0]!='/'))&&
         ((ChN+SlashStr.Len()-1==AbsUrlStr.Len()-1)||(Str[Str.Len()-1]!='/'));
        if (!Found){ChN++;}
      }
      if (Found){
        AbsUrlStr.DelSubStr(ChN, AbsUrlStr.Len()-1);
        AbsUrlStr+=NrRelUrlStr;
      }
    } else {
      int ChN=AbsUrlStr.Len()-1;
      while ((ChN>=0)&&(AbsUrlStr[ChN]!='/')){ChN--;}
      AbsUrlStr.DelSubStr(ChN+1, AbsUrlStr.Len()-1);
      AbsUrlStr+=NrRelUrlStr;
    }
  }

  static const TStr PrevDirStr="/../";
  {int ChN;
  while ((ChN=AbsUrlStr.SearchStr(PrevDirStr))!=-1){
    int BChN=ChN; int EChN=ChN+PrevDirStr.Len()-1;
    while ((BChN-1>=0)&&(AbsUrlStr[BChN-1]!='/')){BChN--;}
    AbsUrlStr.DelSubStr(BChN, EChN);
  }}

  static const TStr CurDirStr="/.";
  while (AbsUrlStr.DelStr(CurDirStr)){}

  GetAbs(AbsUrlStr);
}

TUrl::TUrl(const TStr& _RelUrlStr, const TStr& _BaseUrlStr):
  Scheme(usUndef),
  UrlStr(), RelUrlStr(_RelUrlStr), BaseUrlStr(_BaseUrlStr),
  SchemeNm(), HostNm(),
  PortStr(), PathStr(), SearchStr(), FragIdStr(),
  PortN(-1), PathSegV(),
  IpNum(),
  FinalUrlStr(), FinalHostNm(),
  HttpRqStr(){
  RelUrlStr.ToTrunc();
  RelUrlStr.ChangeStrAll(" ", "%20");
  try {
    if (IsAbs(RelUrlStr)){
      GetAbs(RelUrlStr);
    } else
    if (IsAbs(BaseUrlStr)){
      GetAbsFromBase(RelUrlStr, BaseUrlStr);
    } else {
      Scheme=usUndef;
    }
  }
  catch (PExcept&){Scheme=usUndef;}

  //** old version
  /*
  PUrl BaseUrl;
  if (!BaseUrlStr.Empty()){ // must be outside try-block (CBuilder3.0 bug)
    BaseUrl=TUrl::New(BaseUrlStr);}
  try {
    if (!BaseUrlStr.Empty()){
      EAssertR(BaseUrl->IsOk(), "");}
    if (IsAbs(RelUrlStr)){
      GetAbs(RelUrlStr);
    } else {
      GetAbsFromBase(RelUrlStr, BaseUrlStr);
    }
  }
  catch (PExcept&){Scheme=usUndef;}
  */
}

TStr TUrl::GetDmNm(const int& MxDmSegs) const {
  IAssert(IsOk());
  TChA DmChA; int DmSegs=0;
  for (int ChN=HostNm.Len()-1; ChN>=0; ChN--){
    if (HostNm[ChN]=='.'){
      DmSegs++;
      if (DmSegs==MxDmSegs){break;} else {DmChA+='.';}
    } else {
      DmChA+=HostNm[ChN];
    }
  }
  DmChA.Reverse();
  return DmChA;
}

void TUrl::DefFinalUrl(const TStr& _FinalHostNm){
  IAssert(IsOk(usHttp));
  IAssert(!IsDefFinalUrl());
  FinalHostNm=_FinalHostNm.GetLc();
  if (HostNm==FinalHostNm){
    FinalUrlStr=UrlStr;
  } else {
    TChA FinalUrlChA;
    FinalUrlChA+=SchemeNm; FinalUrlChA+="://";
    FinalUrlChA+=FinalHostNm;
    if (!PortStr.Empty()){
      FinalUrlChA+=":"; FinalUrlChA+=PortStr;}
    FinalUrlChA+=PathStr;
    FinalUrlChA+=SearchStr;
    FinalUrlStr=FinalUrlChA;
  }
}

void TUrl::ToLcPath(){
  // test if the conversion is needed
  if (!PathStr.IsLc()){
    // convert path strings to lower-case
    PathStr.ToLc();
    for (int PathSegN=0; PathSegN<PathSegV.Len(); PathSegN++){
      PathSegV[PathSegN].ToLc();}
    // recompose url
    TChA UrlChA;
    UrlChA+=SchemeNm; UrlChA+="://";
    UrlChA+=HostNm;
    if (!PortStr.Empty()){
      UrlChA+=":"; UrlChA+=PortStr;}
    UrlChA+=PathStr;
    UrlChA+=SearchStr;
    UrlStr=UrlChA;
    // recompose final-url
    if (IsDefFinalUrl()){
      FinalUrlStr.Clr(); DefFinalUrl(FinalHostNm);}
  }
}

bool TUrl::IsAbs(const TStr& UrlStr){
  if (UrlStr.GetLc().IsPrefix(UrlHttpPrefixStr)){
    return UrlStr.GetLc().IsPrefix(UrlHttpAbsPrefixStr);
  } else {
    int ColonChN=UrlStr.SearchCh(':'); int SlashChN=UrlStr.SearchCh('/');
    return (ColonChN!=-1)&&((SlashChN==-1)||(SlashChN!=-1)&&(ColonChN<SlashChN));
  }
}

bool TUrl::IsScript(const TStr& UrlStr){
  return UrlStr.IsChIn('?');
}

bool TUrl::IsSite(const TStr& UrlStr){
  PUrl Url=TUrl::New(UrlStr);
  return Url->IsOk(usHttp) && (Url->GetPathStr()=="/") &&
   Url->GetSearchStr().Empty() && Url->GetFragIdStr().Empty();
}

PUrl TUrl::GetUrlFromShortcut(const TStr& ShortcutUrlStr,
 const TStr& DfHostNmPrefix, const TStr& DfHostNmSufix){
  // shortcut is already correct url
  TStr UrlStr=ShortcutUrlStr;
  PUrl Url=TUrl::New(UrlStr);
  if (Url->IsOk()){return Url;}
  // add 'http://' to shortcut (if shortcut is from more segments)
  if (ShortcutUrlStr.IsChIn('.')){
    UrlStr=TUrl::UrlHttpAbsPrefixStr+ShortcutUrlStr;
    Url=TUrl::New(UrlStr);
    if (Url->IsOk()){return Url;}
  }
  // add 'http://' and '/' to shortcut (if shortcut is from more segments)
  if (ShortcutUrlStr.IsChIn('.')){
    UrlStr=TUrl::UrlHttpAbsPrefixStr+ShortcutUrlStr+"/";
    Url=TUrl::New(UrlStr);
    if (Url->IsOk()){return Url;}
  }
  // add 'http://', prefix, postfix and '/' to shortcut
  UrlStr=UrlHttpAbsPrefixStr+
   DfHostNmPrefix+"."+ShortcutUrlStr+"."+DfHostNmSufix+"/";
  Url=TUrl::New(UrlStr);
  return Url;
}

TStr TUrl::GetUrlSearchStr(const TStr& Str){
  TChA InChA=Str; TChA OutChA;
  for (int ChN=0; ChN<InChA.Len(); ChN++){
    char Ch=InChA[ChN];
    if (Ch==' '){
      OutChA+='+';
    } else
    if ((' '<Ch)&&(Ch<='~')&&(Ch!='+')&&(Ch!='&')&&(Ch!='%')){
      OutChA+=Ch;
    } else {
      OutChA+='%';
      OutChA+=TInt::GetHexStr(uchar(Ch)/16);
      OutChA+=TInt::GetHexStr(uchar(Ch)%16);
    }
  }
  return OutChA;
}

TStr TUrl::GetDocStrFromUrlStr(const TStr& UrlStr, const int& Copies){
  TStrV StrV; UrlStr.SplitOnNonAlNum(StrV);
  TChA DocChA;
  for (int StrN=0; StrN<StrV.Len(); StrN++){
    TStr UcStr=StrV[StrN].GetUc();
    if ((UcStr.Len()>3)&&(UcStr!="HTTP")&&(UcStr!="HTML")&&(UcStr!="INDEX")&&(UcStr!="DEFAULT")){
      for (int CopyN=0; CopyN<Copies; CopyN++){
        if (!DocChA.Empty()){DocChA+=' ';} DocChA+=StrV[StrN];
      }
    }
  }
  return DocChA;
}

TStr TUrl::GetTopDownDocNm(
 const TStr& UrlStr, const int& MxLen, const bool& HostOnlyP){
  PUrl Url=TUrl::New(UrlStr);
  TChA DocNm;
  if (Url->IsOk()){
    TStr HostNm=Url->GetHostNm().GetLc();
    TStrV HostNmSegV; HostNm.SplitOnAllCh('.', HostNmSegV, false);
    for (int HostNmSegN=0; HostNmSegN<HostNmSegV.Len(); HostNmSegN++){
      if (HostNmSegN>0){DocNm+='.';}
      DocNm+=HostNmSegV[HostNmSegV.Len()-HostNmSegN-1];
    }
    if (!HostOnlyP){
      DocNm+=Url->GetPathStr().GetLc();
    }
  } else {
    DocNm=UrlStr.GetLc();
  }
  if (MxLen!=-1){
    DocNm.Trunc(MxLen);}
  return DocNm;
}

/////////////////////////////////////////////////
// Url-Search-Environment
TStr TUrlEnv::GetFullUrlStr() const {
  if (GetKeys()==0){return TStr();}
  TChA SearchChA;
  SearchChA+=BaseUrlStr;
  SearchChA+="?";
  int KeyVals=0;
  for (int KeyN=0; KeyN<GetKeys(); KeyN++){
    TStr KeyNm=GetKeyNm(KeyN);
    TStrV ValStrV=KeyNmToValH.GetDat(KeyNm);
    for (int ValStrN=0; ValStrN<ValStrV.Len(); ValStrN++){
      if (KeyVals>0){SearchChA+="&";}
      SearchChA+=TUrl::GetUrlSearchStr(KeyNm);
      SearchChA+='=';
      SearchChA+=TUrl::GetUrlSearchStr(ValStrV[ValStrN]);
      KeyVals++;
    }
  }
  return SearchChA;
}

PUrlEnv TUrlEnv::MkClone(const PUrlEnv& UrlEnv){
  PUrlEnv CloneUrlEnv=
   PUrlEnv(new TUrlEnv(*UrlEnv));
  return CloneUrlEnv;
}

