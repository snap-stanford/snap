/////////////////////////////////////////////////
// Lexical-Char-Definition
typedef enum {lctUndef, lctSpace, lctNum, lctAlpha, lctSSym, lctTerm} TLxChTy;
typedef enum {lcdtUsAscii, lcdtYuAscii} TLxChDefTy;

ClassTP(TLxChDef, PLxChDef)//{
private:
  TIntV ChTyV;
  TChV UcChV;
  void SetUcCh(const TStr& Str);
  void SetChTy(const TLxChTy& ChTy, const TStr& Str);
public:
  TLxChDef(const TLxChDefTy& ChDefTy);
  static PLxChDef New(const TLxChDefTy& ChDefTy=lcdtUsAscii){
    return PLxChDef(new TLxChDef(ChDefTy));}
  TLxChDef(TSIn& SIn): ChTyV(SIn), UcChV(SIn){}
  static PLxChDef Load(TSIn& SIn){return new TLxChDef(SIn);}
  void Save(TSOut& SOut){ChTyV.Save(SOut); UcChV.Save(SOut);}

  TLxChDef& operator=(const TLxChDef& ChDef){
    ChTyV=ChDef.ChTyV; UcChV=ChDef.UcChV; return *this;}

  int GetChTy(const char& Ch) const {return ChTyV[Ch-TCh::Mn];}
  bool IsTerm(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==TInt(lctTerm);}
  bool IsSpace(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==TInt(lctSpace);}
  bool IsAlpha(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==TInt(lctAlpha);}
  bool IsNum(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==TInt(lctNum);}
  bool IsAlNum(const char& Ch) const {
    return (ChTyV[Ch-TCh::Mn]==TInt(lctAlpha))||(ChTyV[Ch-TCh::Mn]==TInt(lctNum));}
  char GetUc(const char& Ch) const {return UcChV[Ch-TCh::Mn];}

  bool IsNmStr(const TStr& Str) const;
  TStr GetUcStr(const TStr& Str) const;

  // standard entry points
  static PLxChDef GetChDef(const TLxChDefTy& ChDefTy=lcdtUsAscii);
//  static TLxChDef& GetChDefRef(const TLxChDefTy& ChDefTy=lcdtUsAscii);
};

/////////////////////////////////////////////////
// Lexical-Symbols
typedef enum {
  syUndef, syLn, syTab, syBool, syInt, syFlt, syStr, syIdStr, syQStr,
  syPeriod, syDPeriod, syComma, syColon, syDColon, sySemicolon,
  syPlus, syMinus, syAsterisk, sySlash, syPercent,
  syExclamation, syVBar, syAmpersand, syQuestion, syHash,
  syEq, syNEq, syLss, syGtr, syLEq, syGEq,
  syLParen, syRParen, syLBracket, syRBracket, syLBrace, syRBrace,
  syEoln, syEof,
  syMnRw, syRw1, syRw2, syRw3, syRw4, syRw5, syRw6, syRw7, syRw8, syRw9,
  syRw10, syRw11, syRw12, syRw13, syRw14, syRw15, syRw16, syRw17, syMxRw
} TLxSym;

class TLxSymStr{
public:
  static const TStr UndefStr;
  static const TStr LnStr;
  static const TStr TabStr;
  static const TStr BoolStr;
  static const TStr IntStr;
  static const TStr FltStr;
  static const TStr StrStr;
  static const TStr IdStrStr;
  static const TStr QStrStr;
  static const TStr PeriodStr;
  static const TStr DPeriodStr;
  static const TStr CommaStr;
  static const TStr ColonStr;
  static const TStr DColonStr;
  static const TStr SemicolonStr;
  static const TStr PlusStr;
  static const TStr MinusStr;
  static const TStr AsteriskStr;
  static const TStr SlashStr;
  static const TStr PercentStr;
  static const TStr ExclamationStr;
  static const TStr VBarStr;
  static const TStr AmpersandStr;
  static const TStr QuestionStr;
  static const TStr HashStr;
  static const TStr EqStr;
  static const TStr NEqStr;
  static const TStr LssStr;
  static const TStr GtrStr;
  static const TStr LEqStr;
  static const TStr GEqStr;
  static const TStr LParenStr;
  static const TStr RParenStr;
  static const TStr LBracketStr;
  static const TStr RBracketStr;
  static const TStr LBraceStr;
  static const TStr RBraceStr;
  static const TStr EolnStr;
  static const TStr EofStr;
  static TStr GetSymStr(const TLxSym& Sym);
  static TLxSym GetSSym(const TStr& Str);
public:
  static bool IsSep(const TLxSym& PrevSym, const TLxSym& Sym);
};

/////////////////////////////////////////////////
// Lexical-Input-Symbol-State
class TILx;

class TILxSymSt{
private:
  TLxSym Sym;
  TStr Str, UcStr, CmtStr;
  bool Bool; int Int; double Flt;
  int SymLnN, SymLnChN, SymChN;
public:
  TILxSymSt();
  TILxSymSt(const TILxSymSt& SymSt);
  TILxSymSt(TILx& Lx);
  TILxSymSt(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  void Restore(TILx& Lx);
};

/////////////////////////////////////////////////
// Lexical-Input
typedef enum {
  iloCmtAlw, iloRetEoln, iloSigNum, iloUniStr, iloCsSens,
  iloExcept, iloTabSep, iloList, iloMx} TILxOpt;

class TILx{
private:
  PLxChDef ChDef;
  PSIn SIn;
  TSIn& RSIn;
  char PrevCh, Ch;
  int LnN, LnChN, ChN;
  TSStack<TILxSymSt> PrevSymStStack;
  TStrIntH RwStrH;
  bool IsCmtAlw, IsRetEoln, IsSigNum, IsUniStr, IsCsSens;
  bool IsExcept, IsTabSep, IsList;
  char GetCh(){
    Assert(Ch!=TCh::EofCh);
    PrevCh=Ch; LnChN++; ChN++;
    Ch=((RSIn.Eof()) ? TCh::EofCh : RSIn.GetCh());
    if (IsList){putchar(Ch);}
    return Ch;
  }
  char GetCh1(){char Ch=GetCh1(); printf("%c", Ch); return Ch;}
public: // symbol state
  TLxSym Sym;
  TChA Str, UcStr, CmtStr;
  bool Bool; int Int; double Flt;
  int SymLnN, SymLnChN, SymChN;
  bool QuoteP;
  char QuoteCh;
public:
  TILx(const PSIn& _SIn, const TFSet& OptSet=TFSet(),
   const TLxChDefTy& ChDefTy=lcdtUsAscii);

  TILx& operator=(const TILx&){Fail; return *this;}

  void SetOpt(const int& Opt, const bool& Val);
  TLxSym AddRw(const TStr& Str);
  TLxSym GetRw(const TStr& Str){
    return TLxSym(int(RwStrH.GetDat(Str)));}
  PSIn GetSIn(const char& SepCh);
  int GetLnN() const {return LnN;}
  bool IsBof() const {return ChN==-1;}
  bool IsEof() const {return Ch==TCh::EofCh;}

  TLxSym GetSym(const TFSet& Expect);
  TLxSym GetSym(){return GetSym(TFSet());}
  TLxSym GetSym(const TLxSym& Sym){return GetSym(TFSet()|Sym);}
  TLxSym GetSym(const TLxSym& Sym1, const TLxSym& Sym2){
    return GetSym(TFSet()|Sym1|Sym2);}
  TLxSym GetSym(const TLxSym& Sym1, const TLxSym& Sym2, const TLxSym& Sym3){
    return GetSym(TFSet()|Sym1|Sym2|Sym3);}
  TLxSym GetSym(const TLxSym& Sym1, const TLxSym& Sym2, const TLxSym& Sym3,
   const TLxSym& Sym4){
    return GetSym(TFSet()|Sym1|Sym2|Sym3|Sym4);}
  bool GetBool(){GetSym(TFSet()|syBool); return Bool;}
  int GetInt(){GetSym(TFSet()|syInt); return Int;}
  double GetFlt(){GetSym(TFSet()|syFlt); return Flt;}
  TStr GetStr(const TStr& _Str=TStr()){
    GetSym(TFSet()|syStr); IAssert(_Str.Empty()||(_Str==Str)); return Str;}
  TStr GetIdStr(const TStr& IdStr=TStr()){
    GetSym(TFSet()|syIdStr); IAssert(IdStr.Empty()||(IdStr==Str)); return Str;}
  TStr GetQStr(const TStr& QStr=TStr()){
    GetSym(TFSet()|syQStr); IAssert(QStr.Empty()||(Str==QStr)); return Str;}
  void GetEoln(){GetSym(TFSet()|syEoln);}
  TStr GetStrToCh(const char& ToCh);
  TStr GetStrToEolnOrCh(const char& ToCh);
  TStr GetStrToEoln(const bool& DoTrunc=false);
  TStr GetStrToEolnAndCh(const char& ToCh);
  void SkipToEoln();
  void SkipToSym(const TLxSym& SkipToSym){
    while (Sym!=SkipToSym){GetSym();}}

  void PutSym(const TILxSymSt& SymSt){PrevSymStStack.Push(TILxSymSt(SymSt));}
  void PutSym(){PrevSymStStack.Push(TILxSymSt(*this));}
  TLxSym PeekSym(){TLxSym NextSym=GetSym(); PutSym(); return NextSym;}
  TLxSym PeekSym(const int& Syms);

  TStr GetSymStr() const;
  TStr GetFPosStr() const;
  static TStr GetQStr(const TStr& Str, const bool& QuoteP, const char& QuoteCh);

  bool IsVar(const TStr& VarNm){
    GetSym(); bool Var=((Sym==syIdStr)&&(Str==VarNm)); PutSym(); return Var;}
  void GetVar(const TStr& VarNm,
   const bool& LBracket=false, const bool& NewLn=false){
    GetIdStr(VarNm); GetSym(syColon);
    if (LBracket){GetSym(syLBracket);} if (NewLn){GetEoln();}}
  void GetVarEnd(const bool& RBracket=false, const bool& NewLn=false){
    if (RBracket){GetSym(syRBracket);}
    if (NewLn){GetEoln();}}
  bool PeekVarEnd(const bool& RBracket=false, const bool& NewLn=false){
    if (RBracket){return PeekSym()==syRBracket;}
    if (NewLn){return PeekSym()==syEoln;} Fail; return false;}
  bool GetVarBool(const TStr& VarNm, const bool& NewLn=true){
    GetIdStr(VarNm); GetSym(syColon); bool Bool=GetBool();
    if (NewLn){GetEoln();} return Bool;}
  int GetVarInt(const TStr& VarNm, const bool& NewLn=true){
    GetIdStr(VarNm); GetSym(syColon); int Int=GetInt();
    if (NewLn){GetEoln();} return Int;}
  double GetVarFlt(const TStr& VarNm, const bool& NewLn=true){
    GetIdStr(VarNm); GetSym(syColon); double Flt=GetFlt();
    if (NewLn){GetEoln();} return Flt;}
  TStr GetVarStr(const TStr& VarNm, const bool& NewLn=true){
    GetIdStr(VarNm); GetSym(syColon); TStr Str=GetQStr();
    if (NewLn){GetEoln();} return Str;}
  TSecTm GetVarSecTm(const TStr& VarNm, const bool& NewLn=true){
    GetIdStr(VarNm); GetSym(syColon); TSecTm SecTm=TSecTm::LoadTxt(*this);
    if (NewLn){GetEoln();} return SecTm;}
  void GetVarBoolV(const TStr& VarNm, TBoolV& BoolV, const bool& NewLn=true);
  void GetVarIntV(const TStr& VarNm, TIntV& IntV, const bool& NewLn=true);
  void GetVarFltV(const TStr& VarNm, TFltV& FltV, const bool& NewLn=true);
  void GetVarStrV(const TStr& VarNm, TStrV& StrV, const bool& NewLn=true);
  void GetVarStrPrV(const TStr& VarNm, TStrPrV& StrPrV, const bool& NewLn=true);
  void GetVarStrVV(const TStr& VarNm, TVec<TStrV>& StrVV, const bool& NewLn=true);

  // file-of-lines
  static void GetLnV(const TStr& FNm, TStrV& LnV);
};

/////////////////////////////////////////////////
// Lexical-Output
typedef enum {
  oloCmtAlw, oloFrcEoln, oloSigNum, oloUniStr,
  oloCsSens, oloTabSep, oloVarIndent, oloMx} TOLxOpt;

class TOLx{
private:
  PLxChDef ChDef;
  PSOut SOut;
  TSOut& RSOut;
  bool IsCmtAlw, IsFrcEoln, IsSigNum, IsUniStr;
  bool IsCsSens, IsTabSep, IsVarIndent;
  int VarIndentLev;
  TStrIntH RwStrH;
  TIntStrH RwSymH;
  TLxSym PrevSym;
  void PutSep(const TLxSym& Sym);
public:
  TOLx(const PSOut& _SOut, const TFSet& OptSet,
   const TLxChDefTy& ChDefTy=lcdtUsAscii);

  TOLx& operator=(const TOLx&){Fail; return *this;}

  void SetOpt(const int& Opt, const bool& Val);
  TLxSym AddRw(const TStr& Str);
  PSOut GetSOut(const char& SepCh){
    RSOut.PutCh(SepCh); return SOut;}

  void PutSym(const TLxSym& Sym);
  void PutBool(const TBool& Bool){
    PutSep(syIdStr); RSOut.PutStr(TBool::GetStr(Bool));}
  void PutInt(const TInt& Int){
    if (!IsSigNum){Assert(int(Int)>=0);}
    PutSep(syInt); RSOut.PutStr(TInt::GetStr(Int));}
  void PutFlt(const TFlt& Flt, const int& Width=-1, const int& Prec=-1){
    if (!IsSigNum){Assert(Flt>=0);}
    PutSep(syFlt); RSOut.PutStr(TFlt::GetStr(Flt, Width, Prec));}
  void PutStr(const TStr& Str){
    if ((IsUniStr)&&(ChDef->IsNmStr(Str))){PutSep(syIdStr); RSOut.PutStr(Str);}
    else {PutSep(syStr); RSOut.PutCh('"'); RSOut.PutStr(Str); RSOut.PutCh('"');}}
  void PutIdStr(const TStr& Str, const bool& CheckIdStr=true){
    if (CheckIdStr){Assert(ChDef->IsNmStr(Str));}
    PutSep(syIdStr); RSOut.PutStr(Str);}
  void PutQStr(const TStr& Str){
    PutSep(syQStr); RSOut.PutCh('"'); RSOut.PutStr(Str); RSOut.PutCh('"');}
  void PutQStr(const TChA& ChA){
    PutSep(syQStr); RSOut.PutCh('"'); RSOut.PutStr(ChA); RSOut.PutCh('"');}
  void PutUQStr(const TStr& Str){
    PutSep(syIdStr); RSOut.PutStr(Str);}
  void PutLnCmt(const TStr& Str, const int& IndentLev=0){
    Assert(IsCmtAlw); PutStr(" // "); PutStr(Str); PutLn(IndentLev);}
  void PutParCmt(const TStr& Str){
    Assert(IsCmtAlw); PutStr(" /* "); PutStr(Str); PutStr(" */ ");}
  void PutIndent(const int& IndentLev){
    RSOut.PutCh(' ', IndentLev*2);}
  void PutTab() const {RSOut.PutCh(TCh::TabCh);}
  void PutLn(const int& IndentLev=0){
    Assert(IsFrcEoln);
    PutSep(syEoln); RSOut.PutLn(); RSOut.PutCh(' ', IndentLev*2);}
  void PutDosLn(const int& IndentLev=0){
    Assert(IsFrcEoln);
    PutSep(syEoln); RSOut.PutDosLn(); RSOut.PutCh(' ', IndentLev*2);}

  void PutVar(const TStr& VarNm, const bool& LBracket=false,
   const bool& NewLn=false, const bool& CheckIdStr=true){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutIdStr(VarNm, CheckIdStr); PutSym(syColon);
    if (LBracket){PutSym(syLBracket);}
    if (NewLn){PutLn(); VarIndentLev++;}}
  void PutVarEnd(const bool& RBracket=false, const bool& NewLn=false){
    if (IsVarIndent){PutIndent(VarIndentLev-1);}
    if (RBracket){PutSym(syRBracket);}
    if (NewLn){PutLn(); VarIndentLev--;}}
  void PutVarBool(const TStr& VarNm, const bool& Bool,
   const bool& NewLn=true, const bool& CheckIdStr=true){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutIdStr(VarNm, CheckIdStr); PutSym(syColon); PutBool(Bool);
    if (NewLn){PutLn();}}
  void PutVarInt(const TStr& VarNm, const int& Int,
   const bool& NewLn=true, const bool& CheckIdStr=true){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutIdStr(VarNm, CheckIdStr); PutSym(syColon); PutInt(Int);
    if (NewLn){PutLn();}}
  void PutVarFlt(const TStr& VarNm, const double& Flt,
   const bool& NewLn=true, const bool& CheckIdStr=true){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutIdStr(VarNm, CheckIdStr); PutSym(syColon); PutFlt(Flt);
    if (NewLn){PutLn();}}
  void PutVarStr(const TStr& VarNm, const TStr& Str,
   const bool& NewLn=true, const bool& CheckIdStr=true){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutIdStr(VarNm, CheckIdStr); PutSym(syColon); PutQStr(Str);
    if (NewLn){PutLn();}}
  void PutVarSecTm(const TStr& VarNm, const TSecTm& SecTm,
   const bool& NewLn=true, const bool& CheckIdStr=true){
    if (IsVarIndent){PutIndent(VarIndentLev);}
    PutIdStr(VarNm, CheckIdStr); PutSym(syColon); SecTm.SaveTxt(*this);
    if (NewLn){PutLn();}}
  void PutVarBoolV(const TStr& VarNm, const TBoolV& BoolV,
   const bool& NewLn=true, const bool& CheckIdStr=true);
  void PutVarIntV(const TStr& VarNm, const TIntV& IntV,
   const bool& NewLn=true, const bool& CheckIdStr=true);
  void PutVarFltV(const TStr& VarNm, const TFltV& FltV,
   const bool& NewLn=true, const bool& CheckIdStr=true);
  void PutVarStrV(const TStr& VarNm, const TStrV& StrV,
   const bool& NewLn=true, const bool& CheckIdStr=true);
  void PutVarStrPrV(const TStr& VarNm, const TStrPrV& StrPrV,
   const bool& NewLn=true, const bool& CheckIdStr=true);
  void PutVarStrVV(const TStr& VarNm, const TVec<TStrV>& StrVV,
   const bool& NewLn=true, const bool& CheckIdStr=true);
};

/////////////////////////////////////////////////
// Preprocessor
class TPreproc{
private:
  PSIn SIn;
  TStrV SubstKeyIdV;
  char PrevCh, Ch;
  THash<TStr, TStrPrV> SubstIdToKeyIdValPrVH;
  char GetCh();
  bool IsSubstId(const TStr& SubstId, TStr& SubstValStr) const;
  UndefDefaultCopyAssign(TPreproc);
public:
  TPreproc(const TStr& InFNm, const TStr& OutFNm,
   const TStr& SubstFNm, const TStrV& _SubstKeyIdV);

  static void Execute(const TStr& InFNm, const TStr& OutFNm,
   const TStr& InSubstFNm, const TStrV& SubstKeyIdV){
    TPreproc Preproc(InFNm, OutFNm, InSubstFNm, SubstKeyIdV);}
};

/* Sample Subst-File
<SubstList>

<Subst Id="TId">
  <Str Key="MSSQL">TId</Str>
  <Str Key="Oracle">NUMBER(15) NOT NULL</Str>
</Subst>

<Subst Id="TStr2NN">
  <Str Key="MSSQL">TStr2NN</Str>
  <Str Key="Oracle">VARCHAR2(2) NOT NULL</Str>
</Subst>

</SubstList>
*/
