////////////////////////////////////////////////
// Lexical-Chars
void TLxChDef::SetUcCh(const TStr& Str){
  for (int CC=1; CC<Str.Len(); CC++){
    UcChV[Str[CC]-TCh::Mn]=TCh(Str[0]);}
}

void TLxChDef::SetChTy(const TLxChTy& ChTy, const TStr& Str){
  for (int CC=0; CC<Str.Len(); CC++){
    ChTyV[Str[CC]-TCh::Mn]=TInt(ChTy);}
}

TLxChDef::TLxChDef(const TLxChDefTy& ChDefTy):
  ChTyV(TCh::Vals), UcChV(TCh::Vals){

  if (ChDefTy==lcdtUsAscii){
    // Character-Types
    ChTyV.PutAll(TInt(lctSpace));
    SetChTy(lctNum, "0123456789");
    SetChTy(lctAlpha, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetChTy(lctAlpha, "abcdefghijklmnopqrstuvwxyz");
    SetChTy(lctAlpha, "@_");
    SetChTy(lctSSym, "\"'.,:;+-*/%!#|&<=>?()[]{}");
    SetChTy(lctTerm, TStr(TCh::CrCh));
    SetChTy(lctTerm, TStr(TCh::LfCh));
    SetChTy(lctTerm, TStr(TCh::EofCh));

    // Upper-Case
    for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){UcChV[Ch-TCh::Mn]=TCh(char(Ch));}
    SetUcCh("Aa"); SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("Dd"); SetUcCh("Ee");
    SetUcCh("Ff"); SetUcCh("Gg"); SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("Jj");
    SetUcCh("Kk"); SetUcCh("Ll"); SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("Oo");
    SetUcCh("Pp"); SetUcCh("Qq"); SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("Tt");
    SetUcCh("Uu"); SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx"); SetUcCh("Yy");
    SetUcCh("Zz");
  } else
  if (ChDefTy==lcdtYuAscii){
    // Character-Types
    ChTyV.PutAll(TInt(lctSpace));
    SetChTy(lctNum, "0123456789");
    SetChTy(lctAlpha, "ABC^]D\\EFGHIJKLMNOPQRS[TUVWXYZ@");
    SetChTy(lctAlpha, "abc~}d|efghijklmnopqrs{tuvwxyz`");
    SetChTy(lctAlpha, "_");
    SetChTy(lctSSym, "\".,:;+-*/%!#&<=>?()");
    SetChTy(lctTerm, TStr(TCh::CrCh));
    SetChTy(lctTerm, TStr(TCh::LfCh));
    SetChTy(lctTerm, TStr(TCh::EofCh));

    // Upper-Case
    for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){UcChV[Ch-TCh::Mn]=TCh(char(Ch));}
    SetUcCh("Aa"); SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("^~"); SetUcCh("]}");
    SetUcCh("Dd"); SetUcCh("\\|"); SetUcCh("Ee"); SetUcCh("Ff"); SetUcCh("Gg");
    SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("Jj"); SetUcCh("Kk"); SetUcCh("Ll");
    SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("Oo"); SetUcCh("Pp"); SetUcCh("Qq");
    SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("[{"); SetUcCh("Tt"); SetUcCh("Uu");
    SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx"); SetUcCh("Yy"); SetUcCh("Zz");
    SetUcCh("@`");
  } else {
    Fail;
  }
}

bool TLxChDef::IsNmStr(const TStr& Str) const {
  if (Str.Len()==0){return false;}
  if (!IsAlpha(Str.GetCh(0))){return false;}
  for (int ChN=1; ChN<Str.Len(); ChN++){
    if (!IsAlNum(Str.GetCh(ChN))){return false;}}
  return true;
}

TStr TLxChDef::GetUcStr(const TStr& Str) const {
  TChA UcStr;
  for (int ChN=0; ChN<Str.Len(); ChN++){
    UcStr.AddCh(GetUc(Str.GetCh(ChN)));}
  return UcStr;
}


PLxChDef TLxChDef::GetChDef(const TLxChDefTy& ChDefTy){
  static PLxChDef UsAsciiChDef=NULL;
  static PLxChDef YuAsciiChDef=NULL;
  switch (ChDefTy){
    case lcdtUsAscii:
      if (UsAsciiChDef.Empty()){UsAsciiChDef=TLxChDef::New(lcdtUsAscii);}
      return UsAsciiChDef;
    case lcdtYuAscii:
      if (YuAsciiChDef.Empty()){YuAsciiChDef=TLxChDef::New(lcdtYuAscii);}
      return YuAsciiChDef;
    default: Fail; return NULL;
  }
}

//TLxChDef& TLxChDef::GetChDefRef(const TLxChDefTy& ChDefTy){
//  switch (ChDefTy){
//    case lcdtUsAscii: return *UsAsciiChDef;
//    case lcdtYuAscii: return *YuAsciiChDef;
//    default: Fail; return *UsAsciiChDef;;
//  }
//}

/////////////////////////////////////////////////
// Lexical-Symbols
const TStr TLxSymStr::UndefStr="<undefined>";
const TStr TLxSymStr::LnStr="<line>";
const TStr TLxSymStr::TabStr="<tab>";
const TStr TLxSymStr::IntStr="<integer>";
const TStr TLxSymStr::FltStr="<float>";
const TStr TLxSymStr::StrStr="<string>";
const TStr TLxSymStr::IdStrStr="<id-string>";
const TStr TLxSymStr::QStrStr="<q-string>";
const TStr TLxSymStr::PeriodStr=".";
const TStr TLxSymStr::DPeriodStr="..";
const TStr TLxSymStr::CommaStr=",";
const TStr TLxSymStr::ColonStr=":";
const TStr TLxSymStr::DColonStr="::";
const TStr TLxSymStr::SemicolonStr=";";
const TStr TLxSymStr::PlusStr="+";
const TStr TLxSymStr::MinusStr="-";
const TStr TLxSymStr::AsteriskStr="*";
const TStr TLxSymStr::SlashStr="/";
const TStr TLxSymStr::PercentStr="%";
const TStr TLxSymStr::ExclamationStr="!";
const TStr TLxSymStr::VBarStr="|";
const TStr TLxSymStr::AmpersandStr="&";
const TStr TLxSymStr::QuestionStr="?";
const TStr TLxSymStr::HashStr="#";
const TStr TLxSymStr::EqStr="=";
const TStr TLxSymStr::NEqStr="<>";
const TStr TLxSymStr::LssStr="<";
const TStr TLxSymStr::GtrStr=">";
const TStr TLxSymStr::LEqStr="<=";
const TStr TLxSymStr::GEqStr=">=";
const TStr TLxSymStr::LParenStr="(";
const TStr TLxSymStr::RParenStr=")";
const TStr TLxSymStr::LBracketStr="[";
const TStr TLxSymStr::RBracketStr="]";
const TStr TLxSymStr::LBraceStr="{";
const TStr TLxSymStr::RBraceStr="}";
const TStr TLxSymStr::EolnStr="<end-of-line>";
const TStr TLxSymStr::EofStr="<end-of-file>";

TStr TLxSymStr::GetSymStr(const TLxSym& Sym){
  switch (Sym){
    case syUndef: return UndefStr;
    case syLn: return LnStr;
    case syTab: return TabStr;
    case syInt: return IntStr;
    case syFlt: return FltStr;
    case syStr: return StrStr;
    case syIdStr: return IdStrStr;
    case syQStr: return QStrStr;
    case syPeriod: return PeriodStr;
    case syDPeriod: return DPeriodStr;
    case syComma: return CommaStr;
    case syColon: return ColonStr;
    case syDColon: return DColonStr;
    case sySemicolon: return SemicolonStr;
    case syPlus: return PlusStr;
    case syMinus: return MinusStr;
    case syAsterisk: return AsteriskStr;
    case sySlash: return SlashStr;
    case syPercent: return PercentStr;
    case syExclamation: return ExclamationStr;
    case syVBar: return VBarStr;
    case syAmpersand: return AmpersandStr;
    case syQuestion: return QuestionStr;
    case syHash: return HashStr;
    case syEq: return EqStr;
    case syNEq: return NEqStr;
    case syLss: return LssStr;
    case syGtr: return GtrStr;
    case syLEq: return LEqStr;
    case syGEq: return GEqStr;
    case syLParen: return LParenStr;
    case syRParen: return RParenStr;
    case syLBracket: return LBracketStr;
    case syRBracket: return RBracketStr;
    case syLBrace: return LBraceStr;
    case syRBrace: return RBraceStr;
    case syEoln: return EolnStr;
    case syEof: return EofStr;
    default: Fail; return TStr();
  }
}

TLxSym TLxSymStr::GetSSym(const TStr& Str){
  static TStrIntH StrToLxSymH(100);
  if (StrToLxSymH.Len()==0){
    StrToLxSymH.AddDat(PeriodStr, syPeriod);
    StrToLxSymH.AddDat(DPeriodStr, syDPeriod);
    StrToLxSymH.AddDat(CommaStr, syComma);
    StrToLxSymH.AddDat(ColonStr, syColon);
    StrToLxSymH.AddDat(DColonStr, syDColon);
    StrToLxSymH.AddDat(SemicolonStr, sySemicolon);
    StrToLxSymH.AddDat(PlusStr, syPlus);
    StrToLxSymH.AddDat(MinusStr, syMinus);
    StrToLxSymH.AddDat(AsteriskStr, syAsterisk);
    StrToLxSymH.AddDat(SlashStr, sySlash);
    StrToLxSymH.AddDat(PercentStr, syPercent);
    StrToLxSymH.AddDat(ExclamationStr, syExclamation);
    StrToLxSymH.AddDat(VBarStr, syVBar);
    StrToLxSymH.AddDat(AmpersandStr, syAmpersand);
    StrToLxSymH.AddDat(QuestionStr, syQuestion);
    StrToLxSymH.AddDat(HashStr, syHash);
    StrToLxSymH.AddDat(EqStr, syEq);
    StrToLxSymH.AddDat(NEqStr, syNEq);
    StrToLxSymH.AddDat(LssStr, syLss);
    StrToLxSymH.AddDat(GtrStr, syGtr);
    StrToLxSymH.AddDat(LEqStr, syLEq);
    StrToLxSymH.AddDat(GEqStr, syGEq);
    StrToLxSymH.AddDat(LParenStr, syLParen);
    StrToLxSymH.AddDat(RParenStr, syRParen);
    StrToLxSymH.AddDat(LBracketStr, syLBracket);
    StrToLxSymH.AddDat(RBracketStr, syRBracket);
    StrToLxSymH.AddDat(LBraceStr, syLBrace);
    StrToLxSymH.AddDat(RBraceStr, syRBrace);
  }
  int KeyId=StrToLxSymH.GetKeyId(Str);
  if (KeyId==-1){
    return syUndef;
  } else {
    return TLxSym(int(StrToLxSymH[KeyId]));
  }
}

bool TLxSymStr::IsSep(const TLxSym& PrevSym, const TLxSym& Sym){
  static TFSet SepPrevSymSet=TFSet()|
    syUndef|syColon|syDColon|syEq|
    syLParen|syRParen|syLBracket|syRBracket|syLBrace|syRBrace|
    syEoln|syEof;

  static TFSet SepSymSet=TFSet()|
    syPeriod|syComma|syColon|syDColon|sySemicolon|
    syEq|
    syExclamation|syQuestion|
    syLParen|syRParen|syLBracket|syRBracket|syLBrace|syRBrace|
    syEoln|syEof;

  return !SepPrevSymSet.In(PrevSym) && !SepSymSet.In(Sym);
}

/////////////////////////////////////////////////
// Lexical-Symbol-State
TILxSymSt::TILxSymSt():
  Sym(syUndef),
  Str(), UcStr(), CmtStr(),
  Bool(false), Int(0), Flt(0),
  SymLnN(-1), SymLnChN(-1), SymChN(-1){}

TILxSymSt::TILxSymSt(const TILxSymSt& SymSt):
  Sym(SymSt.Sym),
  Str(SymSt.Str), UcStr(SymSt.UcStr), CmtStr(SymSt.CmtStr),
  Bool(SymSt.Bool), Int(SymSt.Int), Flt(SymSt.Flt),
  SymLnN(SymSt.SymLnN), SymLnChN(SymSt.SymLnChN), SymChN(SymSt.SymChN){Fail;}

TILxSymSt::TILxSymSt(TILx& Lx):
  Sym(Lx.Sym),
  Str(Lx.Str), UcStr(Lx.UcStr), CmtStr(Lx.CmtStr),
  Bool(Lx.Bool), Int(Lx.Int), Flt(Lx.Flt),
  SymLnN(Lx.SymLnN), SymLnChN(Lx.SymLnChN), SymChN(Lx.SymChN){}

void TILxSymSt::Restore(TILx& Lx){
  Lx.Sym=Sym;
  Lx.Str=Str; Lx.UcStr=UcStr; Lx.CmtStr=CmtStr;
  Lx.Bool=Bool; Lx.Int=Int; Lx.Flt=Flt;
  Lx.SymLnN=SymLnN; Lx.SymLnChN=SymLnChN; Lx.SymChN=Lx.SymChN;}

/////////////////////////////////////////////////
// Lexical-Input
TILx::TILx(const PSIn& _SIn, const TFSet& OptSet, const TLxChDefTy& ChDefTy):
  ChDef(TLxChDef::GetChDef(ChDefTy)),
  SIn(_SIn), RSIn(*SIn),
  PrevCh(' '), Ch(' '), LnN(0), LnChN(0-1), ChN(0-1),
  PrevSymStStack(), RwStrH(50),
  IsCmtAlw(false), IsRetEoln(false), IsSigNum(false),
  IsUniStr(false), IsCsSens(false), IsExcept(false),
  IsTabSep(false), IsList(false),
  Sym(syUndef),
  Str(), UcStr(), CmtStr(),
  Bool(false), Int(0), Flt(0),
  SymLnN(-1), SymLnChN(-1), SymChN(-1){
  for (int Opt=0; Opt<iloMx; Opt++){
    if (OptSet.In(Opt)){SetOpt(Opt, true);}}
}

void TILx::SetOpt(const int& Opt, const bool& Val){
  switch (Opt){
    case iloCmtAlw: IsCmtAlw=Val; break;
    case iloRetEoln: IsRetEoln=Val; break;
    case iloSigNum: IsSigNum=Val; break;
    case iloUniStr: IsUniStr=Val; break;
    case iloCsSens: IsCsSens=Val; break;
    case iloExcept: IsExcept=Val; break;
    case iloTabSep: IsTabSep=Val; break;
    case iloList: IsList=Val; break;
    default: Fail;
  }
}

TLxSym TILx::AddRw(const TStr& Str){
  IAssert(RwStrH.Len()<syMxRw-syMnRw+1);
  TStr UcStr=ChDef->GetUcStr(Str);
  IAssert(!RwStrH.IsKey(UcStr));
  TLxSym RwSym=TLxSym(syMnRw+RwStrH.Len());
  RwStrH.AddDat(Str, TInt(int(RwSym)));
  return RwSym;
}

PSIn TILx::GetSIn(const char& SepCh){
  IAssert(PrevSymStStack.Empty());
  while ((Ch!=TCh::EofCh)&&(Ch!=SepCh)){GetCh();}
  return SIn;
}

TLxSym TILx::GetSym(const TFSet& Expect){
  CmtStr.Clr();
  if (!PrevSymStStack.Empty()){
    // symbols already on the stack
    PrevSymStStack.Top().Restore(*this); PrevSymStStack.Pop();
  } else
  if (Expect.In(syLn)){
    // symbol is the whole line string
    if (Ch==TCh::EofCh){
      Sym=syEof;
    } else {
      Str.Clr();
      if (IsBof()){GetCh();}
      while (!ChDef->IsTerm(Ch)){Str.AddCh(Ch); GetCh();}
      bool _IsRetEoln=IsRetEoln; IsRetEoln=true;
      GetSym(TFSet()|syEoln|syEof); Sym=syLn;
      IsRetEoln=_IsRetEoln;
    }
  } else
  if (IsTabSep){
    // symbol is between tab characters
    if (IsBof()){GetCh();}
    if (Ch==TCh::TabCh){ // tab character
      Sym=syTab; GetCh();
    } else
    if (ChDef->IsTerm(Ch)){ // eoln & eof characters
      bool _IsRetEoln=IsRetEoln; IsRetEoln=true; IsTabSep=false;
      GetSym(TFSet()|syEoln|syEof);
      IsRetEoln=_IsRetEoln; IsTabSep=true;
    } else {
      Str.Clr();
      while ((!ChDef->IsTerm(Ch))&&(Ch!=TCh::TabCh)){
        Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh();}
      Sym=syStr; QuoteP=false;
    }
  } else {
    // usual symbol
    while (ChDef->IsSpace(Ch)){GetCh();}
    SymLnN=LnN; SymLnChN=LnChN; SymChN=ChN;

    if (ChDef->IsAlpha(Ch)){
      if (IsUniStr){Sym=syStr;} else {Sym=syIdStr;}
      Str.Clr(); UcStr.Clr(); QuoteP=false;
      do {Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch));}
      while (ChDef->IsAlNum(GetCh()));
      if (!RwStrH.Empty()){
        TStr RwStr=Str; if (!IsCsSens){RwStr=UcStr;}
        int SymKeyId=RwStrH.GetKeyId(RwStr);
        if (SymKeyId!=-1){Sym=TLxSym(int(RwStrH[SymKeyId]));}
      }
      if (Expect.In(syBool)){
        Sym=syBool; IAssert(TBool::IsValStr(Str));
        Bool=TBool::GetValFromStr(Str);
      }
    } else
    if ((Ch=='"')||(Ch=='\'')){
      if (IsUniStr){Sym=syStr;} else {Sym=syQStr;}
      Str.Clr(); UcStr.Clr(); QuoteP=true; QuoteCh=Ch;
      GetCh();
      forever{
        while ((Ch!=QuoteCh)&&(Ch!='\\')&&(Ch!=TCh::EofCh)){
          Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh();}
        if (Ch==TCh::EofCh){
          Sym=syUndef; break;
        } else if (Ch==QuoteCh){
          GetCh(); break;
        } else {
          GetCh();
          switch (Ch){
            case '"': Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case '\'': Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case '/': Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case 'b': Str.AddCh('\b'); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case 'f': Str.AddCh('\f'); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case 'n': Str.AddCh('\n'); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case 'r': Str.AddCh('\r'); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case 't': Str.AddCh('\t'); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh(); break;
            case 'u': 
              // needs unicode support to be JSON compatible - now it replaces the code with blank
              GetCh(); GetCh(); GetCh(); Str.AddCh(' '); UcStr.AddCh(ChDef->GetUc(' ')); GetCh(); break; 
            default: Sym=syUndef; break;
          }
          if (Sym==syUndef){
            throw PExcept(new TExcept("Invalid Escape Sequence in Quoted String"));}
        }
      }
    } else
    if ((ChDef->IsNum(Ch))||(IsSigNum&&((Ch=='+')||(Ch=='-')))){
      Str.Clr(); bool IntP=true;
      do {Str.AddCh(Ch);} while (ChDef->IsNum(GetCh()));
      if (Expect.In(syFlt)){
        if (Ch=='.'){
          Str.AddCh(Ch); IntP=false;
          while (ChDef->IsNum(GetCh())){Str.AddCh(Ch);}
        }
        if ((Ch=='e')||(Ch=='E')){
          Str.AddCh(Ch); GetCh(); IntP=false;
          if ((Ch=='+')||(Ch=='-')){Str.AddCh(Ch); GetCh();}
          while (ChDef->IsNum(Ch)){Str.AddCh(Ch); GetCh();}
        }
      }
      UcStr=Str;
      if (IntP&&(Expect.In(syInt))){
        Sym=syInt; Int=atoi(Str.CStr());
      } else {
        Sym=syFlt; Flt=atof(Str.CStr());
      }
    } else
    if ((Ch==TCh::CrCh)||(Ch==TCh::LfCh)){
      Sym=syEoln;
      if (Ch==TCh::CrCh){if (GetCh()==TCh::LfCh){GetCh();}} else
      if (Ch==TCh::LfCh){if (GetCh()==TCh::CrCh){GetCh();}}
      LnN++; LnChN=0; if (!IsRetEoln){GetSym(Expect);}
    } else
    if (Ch=='/'){
      GetCh();
      if ((IsCmtAlw)&&(Ch=='/')){
        TChA _CmtStr;
        do {_CmtStr+=GetCh();} while (!ChDef->IsTerm(Ch));
        _CmtStr.Pop(); _CmtStr.Trunc();
        if (Ch==TCh::CrCh){
          if (GetCh()==TCh::LfCh){GetCh();}
        } else
        if (Ch==TCh::LfCh){
          if (GetCh()==TCh::CrCh){GetCh();}
        }
        if (IsRetEoln){Sym=syEoln;} else {GetSym(Expect);}
        CmtStr=_CmtStr;
      } else
      if (Ch=='*'){
        TChA _CmtStr;
        do {
          while (GetCh()!='*'){_CmtStr+=Ch;}
          _CmtStr+=GetCh();
        } while (Ch!='/');
        _CmtStr.Pop(); _CmtStr.Pop(); _CmtStr.Trunc();
        GetCh(); GetSym(Expect);
        CmtStr=_CmtStr;
      } else {
        Sym=sySlash;
      }
    } else
    if (Ch==TCh::EofCh){
      Sym=syEof;
    } else {
      switch (Ch){
        case '.':
          if (GetCh()=='.'){Sym=syDPeriod; GetCh();}
          else {Sym=syPeriod;} break;
        case ',': Sym=syComma; GetCh(); break;
        case ':':
          if (GetCh()==':'){Sym=syDColon; GetCh();}
          else {Sym=syColon;} break;
        case ';': Sym=sySemicolon; GetCh(); break;
        case '+': Sym=syPlus; GetCh(); break;
        case '-': Sym=syMinus; GetCh(); break;
        case '*': Sym=syAsterisk; GetCh(); break;
        case '/': Sym=sySlash; GetCh(); break;
        case '%': Sym=syPercent; GetCh(); break;
        case '!': Sym=syExclamation; GetCh(); break;
        case '|': Sym=syVBar; GetCh(); break;
        case '&': Sym=syAmpersand; GetCh(); break;
        case '=': Sym=syEq; GetCh(); break;
        case '<':
          GetCh();
          if (Ch=='='){Sym=syLEq; GetCh();}
          else if (Ch=='>'){Sym=syNEq; GetCh();}
          else {Sym=syLss;} break;
        case '>':
          if (GetCh()=='='){Sym=syGEq; GetCh();}
          else {Sym=syGtr;} break;
        case '?': Sym=syQuestion; GetCh(); break;
        case '#':
          if (IsCmtAlw){
            TChA _CmtStr;
            do {_CmtStr+=GetCh();} while (!ChDef->IsTerm(Ch));
            _CmtStr.Pop(); _CmtStr.Trunc();
            if (Ch==TCh::CrCh){
              if (GetCh()==TCh::LfCh){GetCh();}
            } else
            if (Ch==TCh::LfCh){
              if (GetCh()==TCh::CrCh){GetCh();}
            }
            if (IsRetEoln){Sym=syEoln;} else {GetSym(Expect);}
            CmtStr=_CmtStr;
          } else {
            Sym=syHash; GetCh();
          }
          break;
        case '(': Sym=syLParen; GetCh(); break;
        case ')': Sym=syRParen; GetCh(); break;
        case '[': Sym=syLBracket; GetCh(); break;
        case ']': Sym=syRBracket; GetCh(); break;
        case '{': Sym=syLBrace; GetCh(); break;
        case '}': Sym=syRBrace; GetCh(); break;
        default: Sym=syUndef; GetCh(); break;
      }
    }
  }

  if ((!Expect.In(Sym))&&(!Expect.Empty())){
    if (IsExcept){
     TStr MsgStr=
      TStr("Unexpected symbol (")+GetSymStr()+") ["+GetFPosStr()+"]";
     throw PExcept(new TExcept(MsgStr));
    } else {
      Fail;
    }
  }
  return Sym;
}

TStr TILx::GetStrToCh(const char& ToCh){
  Sym=syStr; Str.Clr(); UcStr.Clr();
  while ((Ch!=ToCh)&&(Ch!=TCh::EofCh)){
    Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh();}
  return Str;
}

TStr TILx::GetStrToEolnOrCh(const char& ToCh){
  Sym=syStr; Str.Clr(); UcStr.Clr();
  while ((Ch!=ToCh)&&(Ch!=TCh::CrCh)&&(Ch!=TCh::LfCh)&&(Ch!=TCh::EofCh)){
    Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh();}
  return Str;
}

TStr TILx::GetStrToEoln(const bool& DoTrunc){
  Sym=syStr; Str.Clr(); UcStr.Clr();
  while ((Ch!=TCh::CrCh)&&(Ch!=TCh::LfCh)&&(Ch!=TCh::EofCh)){
    Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh();}
  if (DoTrunc){Str.ToTrunc(); UcStr.ToTrunc();}
  return Str;
}

TStr TILx::GetStrToEolnAndCh(const char& ToCh){
  Sym=syStr; Str.Clr(); UcStr.Clr();
  if (IsBof()){GetCh();}
  forever {
    if (Ch==TCh::EofCh){break;}
    if (((ChN==0)||(PrevCh==TCh::CrCh)||(PrevCh==TCh::LfCh))&&(Ch==ToCh)){
      GetCh(); break;}
    else {Str.AddCh(Ch); UcStr.AddCh(ChDef->GetUc(Ch)); GetCh();}
  }
  return Str;
}

void TILx::SkipToEoln(){
  while ((Ch!=TCh::CrCh)&&(Ch!=TCh::LfCh)&&(Ch!=TCh::EofCh)){
    GetCh();}
  if (Ch==TCh::CrCh){if (GetCh()==TCh::LfCh){GetCh();}} else
  if (Ch==TCh::LfCh){if (GetCh()==TCh::CrCh){GetCh();}}
}

TLxSym TILx::PeekSym(const int& Syms){
  TILxSymSt CurSymSt(*this);
  TSStack<TILxSymSt> SymStStack;
  for (int SymN=0; SymN<Syms; SymN++){
    GetSym(); SymStStack.Push(TILxSymSt(*this));}
  TLxSym PeekedSym=Sym;
  while (!SymStStack.Empty()){
    SymStStack.Top().Restore(*this); SymStStack.Pop();
    PutSym();
  }
  CurSymSt.Restore(*this);
  return PeekedSym;
}

TStr TILx::GetSymStr() const {
  switch (Sym){
    case syInt: return Str;
    case syFlt: return Str;
    case syStr: return Str;
    case syIdStr: return Str;
    case syQStr: return Str;
    default:
      if ((syMnRw<=Sym)&&(Sym<=syMxRw)){return Str;}
      else {return TLxSymStr::GetSymStr(Sym);}
  }
}

TStr TILx::GetFPosStr() const {
  TChA ChA;
  ChA+="File:"; ChA+=SIn->GetSNm();
  ChA+=" Line:"; ChA+=TInt::GetStr(LnN+1);
  ChA+=" Char:"; ChA+=TInt::GetStr(LnChN);
  return ChA;
}

TStr TILx::GetQStr(const TStr& Str, const bool& QuoteP, const char& QuoteCh){
  if (QuoteP){
    TChA ChA;
    ChA+=QuoteCh;
    int StrLen=Str.Len();
    for (int ChN=0; ChN<StrLen; ChN++){
      char Ch=Str.CStr()[ChN];
      if (Ch==QuoteCh){ChA+=QuoteCh; ChA+=QuoteCh;}
      else {ChA+=Ch;}
    }
    ChA+=QuoteCh;
    return ChA;
  } else {
    return Str;
  }
}

void TILx::GetVarBoolV(const TStr& VarNm, TBoolV& BoolV, const bool& NewLn){
  BoolV.Clr();
  GetVar(VarNm, true, NewLn);
  while (GetSym(syRBracket, syBool)==syQStr){
    BoolV.Add(Bool); if (NewLn){GetEoln();}}
  if (NewLn){GetEoln();}
}

void TILx::GetVarIntV(const TStr& VarNm, TIntV& IntV, const bool& NewLn){
  IntV.Clr();
  GetVar(VarNm, true, NewLn);
  while (GetSym(syRBracket, syInt)==syInt){
    IntV.Add(Int); if (NewLn){GetEoln();}}
  if (NewLn){GetEoln();}
}

void TILx::GetVarFltV(const TStr& VarNm, TFltV& FltV, const bool& NewLn){
  FltV.Clr();
  GetVar(VarNm, true, NewLn);
  while (GetSym(syRBracket, syFlt)==syFlt){
    FltV.Add(Flt); if (NewLn){GetEoln();}}
  if (NewLn){GetEoln();}
}

void TILx::GetVarStrV(const TStr& VarNm, TStrV& StrV, const bool& NewLn){
  StrV.Clr();
  GetVar(VarNm, true, NewLn);
  while (GetSym(syRBracket, syQStr)==syQStr){
    StrV.Add(Str); if (NewLn){GetEoln();}}
  if (NewLn){GetEoln();}
}

void TILx::GetVarStrPrV(const TStr& VarNm, TStrPrV& StrPrV, const bool& NewLn){
  StrPrV.Clr();
  GetVar(VarNm, true, NewLn);
  while (GetSym(syRBracket, syLBracket)==syLBracket){
    TStr Str1=GetQStr(); TStr Str2=GetQStr();
    GetSym(syRBracket);
    StrPrV.Add(TStrPr(Str1, Str2)); if (NewLn){GetEoln();}
  }
  if (NewLn){GetEoln();}
}

void TILx::GetVarStrVV(const TStr& VarNm, TVec<TStrV>& StrVV, const bool& NewLn){
  StrVV.Clr();
  GetVar(VarNm, true, NewLn);
  while (GetSym(syRBracket, syLBracket)==syLBracket){
    StrVV.Add();
    while (GetSym(syQStr, syRBracket)==syQStr){
      StrVV.Last().Add(Str);}
    if (NewLn){GetEoln();}
  }
  if (NewLn){GetEoln();}
}

void TILx::GetLnV(const TStr& FNm, TStrV& LnV){
  TFIn SIn(FNm); LnV.Clr(); TChA Ln;
  if (!SIn.Eof()){
    char Ch=SIn.GetCh();
    while (!SIn.Eof()){
      if ((Ch==TCh::CrCh)||(Ch==TCh::LfCh)){
        if (!SIn.Eof()){
          char PrevCh=Ch; Ch=SIn.GetCh();
          if (!SIn.Eof()){
            if (PrevCh==TCh::CrCh){if (Ch==TCh::LfCh){Ch=SIn.GetCh();}} else
            if (PrevCh==TCh::LfCh){if (Ch==TCh::CrCh){Ch=SIn.GetCh();}}
          }
        }
        LnV.Add(Ln); Ln.Clr();
      } else {
        Ln+=Ch; Ch=SIn.GetCh();
      }
    }
    if (!Ln.Empty()){
      LnV.Add(Ln);}
  }
}

/////////////////////////////////////////////////
// Lexical-Output
void TOLx::PutSep(const TLxSym& Sym){
  if (TLxSymStr::IsSep(PrevSym, Sym)){
    if (IsTabSep){RSOut.PutCh(TCh::TabCh);} else {RSOut.PutCh(' ');}}
  PrevSym=Sym;
}

TOLx::TOLx(const PSOut& _SOut, const TFSet& OptSet, const TLxChDefTy& ChDefTy):
  ChDef(TLxChDef::GetChDef(ChDefTy)), SOut(_SOut), RSOut(*SOut),
  IsCmtAlw(false), IsFrcEoln(false), IsSigNum(false),
  IsUniStr(false), IsCsSens(false), IsTabSep(false), IsVarIndent(false),
  VarIndentLev(0),
  RwStrH(50), RwSymH(50), PrevSym(syUndef){
  for (int Opt=0; Opt<oloMx; Opt++){
    if (OptSet.In(Opt)){SetOpt(Opt, true);}}
}

void TOLx::SetOpt(const int& Opt, const bool& Val){
  switch (Opt){
    case oloCmtAlw: IsCmtAlw=Val; break;
    case oloFrcEoln: IsFrcEoln=Val; break;
    case oloSigNum: IsSigNum=Val; break;
    case oloUniStr: IsUniStr=Val; break;
    case oloCsSens: IsCsSens=Val; break;
    case oloTabSep: IsTabSep=Val; break;
    case oloVarIndent: IsVarIndent=Val; break;
    default: Fail;
  }
}

TLxSym TOLx::AddRw(const TStr& Str){
  IAssert(RwStrH.Len()<syMxRw-syMnRw+1);
  TStr UcStr=ChDef->GetUcStr(Str);
  IAssert(!RwStrH.IsKey(UcStr));
  TLxSym RwSym=TLxSym(syMnRw+RwStrH.Len());
  RwStrH.AddDat(Str, TInt(int(RwSym)));
  RwSymH.AddDat(TInt(int(RwSym)), Str);
  return RwSym;
}

void TOLx::PutSym(const TLxSym& Sym){
  TStr Str;
  if ((syMnRw<=Sym)&&(Sym<=syMxRw)){
    Str=Str=RwSymH[Sym];
  } else {
    Str=TLxSymStr::GetSymStr(Sym);
  }
  PutSep(Sym); RSOut.PutStr(Str);
}

void TOLx::PutVarBoolV(const TStr& VarNm, const TBoolV& BoolV,
 const bool& NewLn, const bool& CheckIdStr){
  PutVar(VarNm, true, NewLn, CheckIdStr);
  for (int BoolN=0; BoolN<BoolV.Len(); BoolN++){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutBool(BoolV[BoolN]);
    if (NewLn){PutLn();}
  }
  PutVarEnd(true, NewLn);
}

void TOLx::PutVarIntV(const TStr& VarNm, const TIntV& IntV,
 const bool& NewLn, const bool& CheckIdStr){
  PutVar(VarNm, true, NewLn, CheckIdStr);
  for (int IntN=0; IntN<IntV.Len(); IntN++){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutInt(IntV[IntN]);
    if (NewLn){PutLn();}
  }
  PutVarEnd(true, NewLn);
}

void TOLx::PutVarFltV(const TStr& VarNm, const TFltV& FltV,
 const bool& NewLn, const bool& CheckIdStr){
  PutVar(VarNm, true, NewLn, CheckIdStr);
  for (int FltN=0; FltN<FltV.Len(); FltN++){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutFlt(FltV[FltN]);
    if (NewLn){PutLn();}
  }
  PutVarEnd(true, NewLn);
}

void TOLx::PutVarStrV(const TStr& VarNm, const TStrV& StrV,
 const bool& NewLn, const bool& CheckIdStr){
  PutVar(VarNm, true, NewLn, CheckIdStr);
  for (int StrN=0; StrN<StrV.Len(); StrN++){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutQStr(StrV[StrN]);
    if (NewLn){PutLn();}
  }
  PutVarEnd(true, NewLn);
}

void TOLx::PutVarStrPrV(const TStr& VarNm, const TStrPrV& StrPrV,
 const bool& NewLn, const bool& CheckIdStr){
  PutVar(VarNm, true, NewLn, CheckIdStr);
  for (int StrPrN=0; StrPrN<StrPrV.Len(); StrPrN++){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutSym(syLBracket);
    PutQStr(StrPrV[StrPrN].Val1); PutQStr(StrPrV[StrPrN].Val2);
    PutSym(syRBracket);
    if (NewLn){PutLn();}
  }
  PutVarEnd(true, NewLn);
}

void TOLx::PutVarStrVV(const TStr& VarNm, const TVec<TStrV>& StrVV,
 const bool& NewLn, const bool& CheckIdStr){
  PutVar(VarNm, true, NewLn, CheckIdStr);
  for (int StrVN=0; StrVN<StrVV.Len(); StrVN++){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutSym(syLBracket);
    for (int StrN=0; StrN<StrVV[StrVN].Len(); StrN++){
      PutQStr(StrVV[StrVN][StrN]);}
    PutSym(syRBracket);
    if (NewLn){PutLn();}
  }
  PutVarEnd(true, NewLn);
}

/////////////////////////////////////////////////
// Preprocessor
char TPreproc::GetCh(){
  Assert(Ch!=TCh::EofCh);
  PrevCh=Ch;
  Ch=((SIn->Eof()) ? TCh::EofCh : SIn->GetCh());
  //putchar(Ch);
  return Ch;
}

bool TPreproc::IsSubstId(const TStr& SubstId, TStr& SubstValStr) const {
  if (SubstIdToKeyIdValPrVH.IsKey(SubstId)){
    const TStrPrV& KeyIdValPrV=SubstIdToKeyIdValPrVH.GetDat(SubstId);
    for (int KeyN=0; KeyN<KeyIdValPrV.Len(); KeyN++){
      if (SubstKeyIdV.IsIn(KeyIdValPrV[KeyN].Val1)){
        SubstValStr=KeyIdValPrV[KeyN].Val2;
        return true;
      }
    }
    return false;
  } else {
    return false;
  }
}

TPreproc::TPreproc(const TStr& InFNm, const TStr& OutFNm,
 const TStr& SubstFNm, const TStrV& _SubstKeyIdV):
  SIn(), SubstKeyIdV(_SubstKeyIdV),
  PrevCh('\0'), Ch('\0'){
  // load substitution file
  if (!SubstFNm.Empty()){
    PXmlDoc XmlDoc=TXmlDoc::LoadTxt(SubstFNm);
    // get list of substitutions
    TXmlTokV SubstTokV; XmlDoc->GetTok()->GetTagTokV("Subst", SubstTokV);
    for (int SubstTokN=0; SubstTokN<SubstTokV.Len(); SubstTokN++){
      PXmlTok SubstTok=SubstTokV[SubstTokN];
      // get substitution-id
      TStr SubstId=SubstTok->GetArgVal("Id", "");
      if (!SubstId.Empty()){
        // create substitution
        TStrPrV& KeyIdValPrV=SubstIdToKeyIdValPrVH.AddDat(SubstId);
        // get list of substitution-strings
        TXmlTokV StrTokV; SubstTok->GetTagTokV("Str", StrTokV);
        for (int StrTokN=0; StrTokN<StrTokV.Len(); StrTokN++){
          PXmlTok StrTok=StrTokV[StrTokN];
          // get key-value pair
          TStr KeyId=StrTok->GetArgVal("Key", "");
          TStr ValStr=StrTok->GetTokStr(false);
          // assign key-value-pair
          if (!KeyId.Empty()){
            KeyIdValPrV.Add(TStrPr(KeyId, ValStr));
          }
        }
      }
    }
  }
  // substitution
  // open files
  SIn=TFIn::New(InFNm);
  PSOut SOut=TFOut::New(OutFNm);
  // set copy & ignore mode
  bool CopyModeP=false; bool IgnoreModeP=false;
  GetCh();
  while (Ch!=TCh::EofCh){
    if (isalpha(Ch)||(((PrevCh=='\0')||(PrevCh=='\r')||(PrevCh=='\n'))&&(Ch=='#'))){
      // collect identifier
      TChA IdChA;
      do {
        IdChA+=Ch; GetCh();
      } while ((Ch!=TCh::EofCh)&&(isalnum(Ch)));
      // check identifier
      if (IdChA=="#ifdef"){
        // collect condition-key-id
        TChA CondKeyIdChA;
        while ((Ch!=TCh::EofCh)&&(Ch!='\n')&&(Ch!='\r')){
          CondKeyIdChA+=Ch; GetCh();}
        // skip eoln
        if (Ch=='\n'){GetCh(); if (Ch=='\r'){GetCh();}}
        else if (Ch=='\r'){GetCh(); if (Ch=='\n'){GetCh();}}
        // check for key
        CondKeyIdChA.Trunc();
        IAssert(CopyModeP==false);
        IAssert(IgnoreModeP==false);
        if (SubstKeyIdV.IsIn(CondKeyIdChA)){
          CopyModeP=true; IgnoreModeP=false;
        } else {
          CopyModeP=false; IgnoreModeP=true;
        }
      } else
      if (IdChA=="#endif"){
        // move to eoln
        while ((Ch!=TCh::EofCh)&&(Ch!='\n')&&(Ch!='\r')){
          GetCh();}
        // skip eoln
        if (Ch=='\n'){GetCh(); if (Ch=='\r'){GetCh();}}
        else if (Ch=='\r'){GetCh(); if (Ch=='\n'){GetCh();}}
        // reset copy&ignore modes
        IAssert(CopyModeP||IgnoreModeP);
        CopyModeP=false; IgnoreModeP=false;
      } else {
        // substitution or add id-as-seen
        TStr SubstValStr;
        if ((!CopyModeP)&&(IsSubstId(IdChA, SubstValStr))){
          if (!IgnoreModeP){SOut->PutStr(SubstValStr);}
        } else {
          if (!IgnoreModeP){SOut->PutStr(IdChA);}
        }
      }
    } else {
      // single character
      if (!IgnoreModeP){SOut->PutCh(Ch);}
      GetCh();
    }
  }
}

