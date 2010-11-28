#ifndef xql_h
#define xql_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Xql-Expression-Value-Type
typedef enum {xevtUndef, xevtFlt, xevtStr, xevtVec} TXqlExpValType;

/////////////////////////////////////////////////
// Xql-Expression-Value
ClassTPVL(TXqlExpVal, PXqlExpVal, TXqlExpValV, TXqlExpValL, TXqlExpValLN)//{
private:
  TInt ValType;
  TFlt Flt;
  TStr Str;
  TXqlExpValV ValV;
  UndefCopyAssign(TXqlExpVal);
public:
  TXqlExpVal(const TXqlExpValType& _ValType=xevtUndef): ValType(_ValType){}
  TXqlExpVal(const TFlt& _Flt): ValType(xevtFlt), Flt(_Flt){}
  TXqlExpVal(const double& _Flt): ValType(xevtFlt), Flt(TFlt(_Flt)){}
  TXqlExpVal(const TStr& _Str): ValType(xevtStr), Str(_Str){}
  static PXqlExpVal New(const TXqlExpValType& ValType=xevtUndef){
    return PXqlExpVal(new TXqlExpVal(ValType));}
  static PXqlExpVal New(const TFlt& Flt){return PXqlExpVal(new TXqlExpVal(Flt));}
  static PXqlExpVal New(const double& Flt){return PXqlExpVal(new TXqlExpVal(Flt));}
  static PXqlExpVal New(const TStr& Str){return PXqlExpVal(new TXqlExpVal(Str));}
  ~TXqlExpVal(){}
  TXqlExpVal(TSIn& SIn):
    ValType(SIn), Flt(SIn), Str(SIn), ValV(SIn){}
  static PXqlExpVal Load(TSIn& SIn){return PXqlExpVal(new TXqlExpVal(SIn));}
  void Save(TSOut& SOut) const {
    ValType.Save(SOut); Flt.Save(SOut); Str.Save(SOut); ValV.Save(SOut);}

  bool operator==(const TXqlExpVal& ExpVal) const;
  bool operator<(const TXqlExpVal& ExpVal) const;

  void AddToVec(const PXqlExpVal& ExpVal){
    IAssert(TXqlExpValType(static_cast<int>(ValType))==xevtVec); ValV.Add(ExpVal);}

  TXqlExpValType GetValType() const {return TXqlExpValType(int(ValType));}
  bool IsFltVal(TFlt& _Flt) const {
    if (GetValType()==xevtFlt){_Flt=Flt; return true;} else {return false;}}
  bool IsFltVal(double& _Flt) const {
    if (GetValType()==xevtFlt){_Flt=Flt; return true;} else {return false;}}
  double GetFltVal() const {
    if (GetValType()==xevtFlt){return Flt;} else {return 0;}}
  TStr GetStrVal() const {IAssert(GetValType()==xevtStr); return Str;}
};

/////////////////////////////////////////////////
// Xql-BuilIn
typedef enum {
  xbi_Undef, 
  xbi_SELECT, xbi_ALL, xbi_DISTINCT, xbi_TOP, xbi_PERCENT,
  xbi_FROM, xbi_AS, 
  xbi_WHERE, xbi_AND, xbi_OR,
  xbi_ORDER, xbi_BY, xbi_ASC, xbi_DESC,
  xbi_INTO,
  xbi_Mx
} TXqlBiId;

/////////////////////////////////////////////////
// Xql-BuiltIn-Argument-Type
typedef enum {
  xbatUndef, xbatVoid, xbatFlt, xbatFltFlt
} TXqlBiArgType;

/////////////////////////////////////////////////
// Xql-BuiltIn
class TXqlBi{
private:
  TStrIntH XqlBiNmToIdH;
  TIntIntH XqlBiIdToArgTypeH;
  void AddBi(const TStr& XqlBiNm, const TXqlBiId& XqlBiId,
   const TXqlBiArgType& XqlBiArgType=xbatUndef);
  UndefCopyAssign(TXqlBi);
public:
  TXqlBi();

  bool IsXqlBiId(const TStr& XqlBiNm, TXqlBiId& XqlBiId);
  bool IsXqlBiId(const TStr& XqlBiNm){
    TXqlBiId XqlBiId; return IsXqlBiId(XqlBiNm, XqlBiId);}
  TXqlBiArgType GetXqlBiArgType(const TXqlBiId& XqlBiId);

  void AssertArgs(const int& RqArgs, const int& ActArgs);
};

/////////////////////////////////////////////////
// Xql-Expression-Type
typedef enum {
  xetUndef,
  xetVal, xetVec, xetLst,
  xetOp, xetVar, xetBiConst, xetFunc, xetBiFunc} TXqlExpType;

/////////////////////////////////////////////////
// Xql-Expression-Operator
typedef enum {
  xeopUndef,
  xeopUPlus, xeopUMinus,
  xeopPlus, xeopMinus, xeopMul, xeopDiv, xeopIDiv, xeopMod,
  xeopNot, xeopAnd, xeopOr,
  xeopEq, xeopNEq, xeopLss, xeopGtr, xeopLEq, xeopGEq
} TXqlExpOp;

/////////////////////////////////////////////////
// Xql-Expression
ClassTPV(TXqlExp, PXqlExp, TXqlExpV)//{
private:
  static TXqlBi XqlBi;
  TInt ExpType;
  PXqlExpVal ExpVal;
  TStr ExpNm;
  TInt ExpOp;
  TInt ExpBiId;
  TXqlExpV ArgExpV;
private:
  static const TFSet MulOpSymSet, UAddOpSymSet, AddOpSymSet, RelOpSymSet;
  static const TFSet FactExpExpect, MulExpExpect, AddExpExpect;
  static const TFSet RelExpExpect, ExpExpect;
  static TExpOp GetExpOpFromLxSym(const TLxSym& LxSym);
  static TLxSym GetLxSymFromExpOp(const TExpOp& ExpOp);
  static PXqlExp LoadTxtFact(TILx& Lx, const TFSet& Expect);
  static PXqlExp LoadTxtMulExp(TILx& Lx, const TFSet& Expect);
  static PXqlExp LoadTxtAddExp(TILx& Lx, const TFSet& Expect);
  static PXqlExp LoadTxtRelExp(TILx& Lx, const TFSet& Expect);
  static PXqlExp LoadTxtExp(TILx& Lx, const TFSet& Expect);
  void SaveTxtOp(TOLx& Lx) const;
  PExpVal EvalExpOp(
   const PExpEnv& ExpEnv, const bool& DbgP, TChA& DbgChA);
  PExpVal EvalExp(
   const PExpEnv& ExpEnv, const bool& DbgP, TChA& DbgChA);
public:
  TXqlExp(const TExpType& _ExpType=etUndef):
    ExpType(_ExpType), 
    ExpVal(), ExpNm(), ExpOp(), ExpBiId(), ArgExpV(){}
  TXqlExp(const TExpOp& _ExpOp,
   const PXqlExp& Exp1, const PXqlExp& Exp2=NULL, const PXqlExp& Exp3=NULL);
  TXqlExp(const PExpVal& _ExpVal);
  TXqlExp(const TStr& _VarNm);
  TXqlExp(const TStr& _FuncNm, const TXqlExpV& _ArgExpV);
  ~TXqlExp(){}
  TXqlExp(TSIn& SIn):
    ExpType(SIn), 
    ExpVal(SIn), ExpNm(SIn), ExpOp(SIn), ExpBiId(SIn), ArgExpV(SIn){}
  static PXqlExp Load(TSIn& SIn){return PXqlExp(new TXqlExp(SIn));}
  void Save(TSOut& SOut) const {
    ExpType.Save(SOut); 
    ExpVal.Save(SOut); ExpNm.Save(SOut); ExpOp.Save(SOut);
    ExpBiId.Save(SOut); ArgExpV.Save(SOut);}

  TXqlExp& operator=(const TXqlExp&){Fail; return *this;}
  bool operator==(const TXqlExp&) const {Fail; return false;}

  static bool IsExpStrOk(const TStr& ExpStr, TStr& MsgStr){
    bool Ok; LoadTxt(ExpStr, Ok, MsgStr); return Ok;}
  static PXqlExp LoadTxt(const PSIn& SIn, bool& Ok, TStr& MsgStr,
   const TFSet& Expect=TFSet()|syEof);
  static PXqlExp LoadTxt(const TStr& ExpStr, bool& Ok, TStr& MsgStr){
    PSIn SIn=TStrIn::New(ExpStr); return LoadTxt(SIn, Ok, MsgStr);}
  static PXqlExp LoadTxt(const TStr& ExpStr){
    bool Ok; TStr MsgStr; return LoadTxt(ExpStr, Ok, MsgStr);}

  void SaveTxt(TOLx& Lx) const;
  void SaveTxt(const PSOut& SOut) const {
    TOLx Lx(SOut, TFSet()|oloCmtAlw|oloSigNum|oloCsSens); SaveTxt(Lx);}
  TStr GetStr() const;
  TStr GetTopObjNm() const;
  int GetArgExps() const;
  TStr GetArgExpStr(const int& ArgExpN) const;
};

#endif
