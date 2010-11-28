#ifndef prologparser_h
#define prologparser_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Prolog-Characters
typedef enum {
  plctUnDef, plctBl, plctUpCs, plctLwCs, plctNum, plctSym, plctSolo,
  plctMeta, plctUnderLn, plctSQuo, plctDQuo, plctComm, plctEof} TPlChType;

class TPlChDef{
public:
  // important characters
  static const uchar MnChar=0; // min. character code
  static const uchar MxChar=255; // max. character code
  static const uchar MnPrintableCh=33; // min. printable character code
  static const uchar MxPrintableCh=126; // max. printable character code
  static const uchar BsCh=8; // back-space code
  static const uchar HTabCh=9; // horizontal-tab code
  static const uchar LfCh=10; // line-feed code
  static const uchar VTabCh=11; // vertical-tab code
  static const uchar FfCh=12; // form-feed code
  static const uchar CrCh=13; // carriage-return code
  static const uchar EofCh=26; // end-of-file code
  static const uchar EscCh=27; // escape code
  static const uchar DelCh=127; // escape code
  static const uchar StartChEscCh='\\'; // first ch. for escape sequence in char.-escaping mode
  static const uchar SqCh='\''; // single quote character
  static const uchar DqCh='"'; // double quote character
public:
  TIntV ChTypeV; // character types (of TPlChType)
  TBoolV IdChPV; // identifier characters
  TBoolV ChEscChPV; // character-escaping characters
public:
  TPlChDef();

  bool IsIdCh(const char& Ch) const {return IdChPV[Ch];}
  TPlChType GetChType(const char& Ch) const {
    return (TPlChType)ChTypeV[uint(Ch)].Val;}
};

/////////////////////////////////////////////////
// Prolog-Atom-Definition
typedef enum {
  plot_fx, plot_fy, plot_yfx, plot_xfx, plot_xfy, plot_xf, plot_yf} TPlOpType;

class TPlAtomDef{
public:
  // atom flags
  static const uint AFlgNoData=0;
  static const uint AFlgWriteQ=128;
  // masks for operator handling
  static const uint AFlgOps=127;
  static const uint AFlgOpPrefix=3;
  static const uint AFlgOpInfix=28;
  static const uint AFlgOpPostfix=96;
  static const uint AFlgOpPrefixReset=252;
  static const uint AFlgOpInfixReset=227;
  static const uint AFlgOpPostfixReset=159;
  static const uint AFlgOp_fx=1;
  static const uint AFlgOp_fy=2;
  static const uint AFlgOp_yfz=8;
  static const uint AFlgOp_zfy=4;
  static const uint AFlgOp_xfx=16;
  static const uint AFlgOp_xfy=20;
  static const uint AFlgOp_yfx=24;
  static const uint AFlgOp_xf=32;
  static const uint AFlgOp_yf=64;
  /* description of flags in AtomFSet field
     bit 1 - atom is operator of type fx
     bit 2 - atom is operator of type fy
     bit 3 - atom is infix right associative operator
     bit 4 - atom is infix left associative operator
     bit 5 - atom is infix operator
     bit 6 - atom is operator of type xf
     bit 7 - atom is operator of type yf
     bit 8 - for reading, atom must be quoted */
public:
  TB32Set AtomFSet; // flags described under record declaration
  TUInt PrefixOpPrec, InfixOpPrec, PostfixOpPrec; // operator precedence
public:
  TPlAtomDef():
    AtomFSet(), PrefixOpPrec(), InfixOpPrec(), PostfixOpPrec(){}
  TPlAtomDef(const TPlAtomDef& PlAtomDef):
    AtomFSet(PlAtomDef.AtomFSet), PrefixOpPrec(PlAtomDef.PrefixOpPrec),
    InfixOpPrec(PlAtomDef.InfixOpPrec), PostfixOpPrec(PlAtomDef.PostfixOpPrec){}
  TPlAtomDef(TSIn& SIn):
    AtomFSet(SIn), PrefixOpPrec(SIn),
    InfixOpPrec(SIn), PostfixOpPrec(SIn){}
  void Save(TSOut& SOut) const {
    AtomFSet.Save(SOut); PrefixOpPrec.Save(SOut);
    InfixOpPrec.Save(SOut); PostfixOpPrec.Save(SOut);}

  TPlAtomDef& operator=(const TPlAtomDef& PlAtomDef){
    if (this!=&PlAtomDef){
      AtomFSet=PlAtomDef.AtomFSet; PrefixOpPrec=PlAtomDef.PrefixOpPrec;
      InfixOpPrec=PlAtomDef.InfixOpPrec; PostfixOpPrec=PlAtomDef.PostfixOpPrec;}
    return *this;}

  void SetOp(const TPlOpType& OpType, const uint& OpPrec);
};

/////////////////////////////////////////////////
// Prolog-Atom-Base
typedef enum {
  pla_Impl, pla_ImplDCG, pla_Query, pla_Mode, pla_Public, pla_Dynamic,
  pla_MultiFile, pla_MetaPredicate, pla_Semicolon, pla_VerticalBar, pla_If,
  pla_Comma, pla_Not, pla_Spy, pla_NoSpy, pla_Unify, pla_Is, pla_Uni,
  pla_Identical, pla_NotIdentical, pla_SOLess, pla_SOGreater,
  pla_SOLessOrEqual, pla_SOGreaterOrEqual, pla_AEqual, pla_ANotEqual,
  pla_ALess, pla_AGreater, pla_ALessOrEqual, pla_AGreaterOrEqual,
  pla_Colon, pla_Plus, pla_Minus, pla_BitOr, pla_BitAnd, pla_Divide,
  pla_IntDivide, pla_Times, pla_ShiftLeft, pla_ShiftRight, pla_Mod,
  pla_ExistQuantifier, pla_ExclPoint, pla_EmptyList, pla_EmptyBrace,
  pla_EmptyParen, pla_Period, pla_VAR_N, pla_EndOfFile,
  plaMx} TPlImpAtomId;

ClassTP(TPlAtomBs, PPlAtomBs)//{
private:
  static const int MxOpPrec=1200; // max. operator precedence
  static const int MxArgPrec=999; // max. precedence of argument term
  static const int MxTermArgs=255; // max. number of structure arguments
private:
  TStrHash<TPlAtomDef> StrToAtomDefH; // atom hash-table
  TIntV ImpAtomIdV; // index is of type TPlImpAtomId
  void PrepImpAtoms();
  UndefCopyAssign(TPlAtomBs);
public:
  TPlAtomBs();
  static PPlAtomBs TPlAtomBs::New(){
    return new TPlAtomBs();}

  int AddAtomStr(const TStr& AtomStr){
    return StrToAtomDefH.AddKey(AtomStr);}
  int GetAtomId(const TStr& AtomStr){
    return StrToAtomDefH.GetKeyId(AtomStr);}
  int GetAtoms() const {return StrToAtomDefH.Len();}
  TStr GetAtomStr(const int& AtomId){
    return StrToAtomDefH.GetKey(AtomId);}
  TPlAtomDef& GetAtomDef(const int& AtomId){
    return StrToAtomDefH[AtomId];}

  int GetImpAtomId(const TPlImpAtomId& ImpAtomId) const {
    return ImpAtomIdV[ImpAtomId];}
};

/////////////////////////////////////////////////
// Prolog-Variable-Base
ClassTP(TPlVarBs, PPlVarBs)//{
private:
  TStrH VarStrH; // variable hash-table
  UndefCopyAssign(TPlVarBs);
public:
  TPlVarBs(): VarStrH(){}
  static PPlVarBs TPlVarBs::New(){
    return new TPlVarBs();}

  int AddVarStr(const TStr& VarStr){return VarStrH.AddKey(VarStr);}
  int GetVarId(const TStr& VarStr){return VarStrH.GetKeyId(VarStr);}
  int GetVars() const {return VarStrH.Len();}
  int GetVarStr(const int& VarId) const {return VarStrH[VarId];}
};

/////////////////////////////////////////////////
// Prolog-Lexical
typedef enum {
  plstUndef, plsyAtom, plsyFAtom, plsyVar, plsyVoid, plsyInt, plsyFlt,
  plsyLPar, plsyRPar, plsyLBracket, plsyRBracket, plsyLBrace, plsyRBrace,
  plsyVBar, plsyList, plsyFullStop, plsyEof} TPlLxSym;

class TPlLx{
public:
  static const int MxIntBase=36; // max. base for integer-numbers
  TPlChDef ChDef; // character definitions
  PSIn SIn; // input stream
  TSIn& RSIn; // reference to input stream
  char CCh, NCh; // current and next character
  int ChX; // index of current character
  TChA ChEscBf; // character-escaping buffer
  TPlChType ChType; // type of current character
  TPlLxSym Sym; // last symbol
  TChA ChA; // string-value returned when Sym=plsyVar or Sym=plsyAtom or Sym=plsyFAtom or Sym==plsyString
  int Int; // number-value returned when Sym==plsyInt
  double Flt; // number-value returned when Sym==plsyFlt
  int VarId; // variable-id returned when Sym=plsyVar
  int AtomId; // atom-id returned when Sym=plsyAtom or Sym=plsyFAtom
  void GetCh(){
    CCh=NCh;
    if (RSIn.Eof()){NCh=TCh::EofCh;} else {NCh=RSIn.GetCh(); ChX++;}
    ChType=ChDef.GetChType(CCh); printf("%c", CCh);}
private: // flags
  bool ParseInsideListExpP; // parsing inside list expression
  bool ChEscapingP; // character escaping
private:
  PPlAtomBs AtomBs;
  PPlVarBs VarBs;
public:
public:
  TPlLx(const PSIn& _SIn, const PPlAtomBs& _AtomBs, const PPlVarBs& _VarBs);

  void EThrow(const TStr& MsgStr) const;
  void SkipComment();
  void SkipWs();
  void GetChEscBf();
  void GetVar();
  void GetAtomNm();
  void GetAtomSym();
  void GetAtomQ();
  void GetListStr();
  void GetNum();
  TPlLxSym GetSym();
};

#endif
