/////////////////////////////////////////////////
// Expression-Value-Type
typedef enum {evtUndef, evtFlt, evtStr, evtVec, evtLst} TExpValType;

/////////////////////////////////////////////////
// Expression-Value
ClassTPVL(TExpVal, PExpVal, TExpValV, TExpValL, TExpValLN)//{
private:
  static PExpVal UndefExpVal;
  static PExpVal ZeroExpVal;
  TInt ValType;
  TAscFlt Flt;
  TStr Str;
  TExpValV ValV;
  TExpValL ValL;
public:
  TExpVal(const TExpValType& _ValType=evtUndef): ValType(_ValType){}
  TExpVal(const TFlt& _Flt): ValType(evtFlt), Flt(_Flt){}
  TExpVal(const double& _Flt): ValType(evtFlt), Flt(TFlt(_Flt)){}
  TExpVal(const TStr& _Str): ValType(evtStr), Str(_Str){}
  static PExpVal New(const TExpValType& ValType=evtUndef){
    return PExpVal(new TExpVal(ValType));}
  static PExpVal New(const TFlt& Flt){return PExpVal(new TExpVal(Flt));}
  static PExpVal New(const double& Flt){return PExpVal(new TExpVal(Flt));}
  static PExpVal New(const TStr& Str){return PExpVal(new TExpVal(Str));}
  ~TExpVal(){}
  TExpVal(TSIn& SIn):
    ValType(SIn), Flt(SIn), Str(SIn), ValV(SIn), ValL(SIn){}
  static PExpVal Load(TSIn& SIn){return PExpVal(new TExpVal(SIn));}
  void Save(TSOut& SOut) const {
    ValType.Save(SOut);
    Flt.Save(SOut); Str.Save(SOut); ValV.Save(SOut); ValL.Save(SOut);}

  TExpVal& operator=(const TExpVal&){Fail; return *this;}
  bool operator==(const TExpVal& ExpVal) const;
  bool operator<(const TExpVal& ExpVal) const;

  void AddToVec(const PExpVal& ExpVal){
    IAssert(TExpValType(static_cast<int>(ValType))==evtVec); ValV.Add(ExpVal);}
  void AddToLst(const PExpVal& ExpVal){
    IAssert(TExpValType(static_cast<int>(ValType))==evtLst); ValL.AddBack(ExpVal);}

  TExpValType GetValType() const {return TExpValType(int(ValType));}
  bool IsFltVal(TFlt& _Flt) const {
    if (GetValType()==evtFlt){_Flt=Flt; return true;} else {return false;}}
  bool IsFltVal(double& _Flt) const {
    if (GetValType()==evtFlt){_Flt=Flt; return true;} else {return false;}}
  double GetFltVal() const {
    if (GetValType()==evtFlt){return Flt;} else {return 0;}}
  int GetFltValAsInt(const bool& ThrowExceptP=true) const;
  TStr GetStrVal() const {IAssert(GetValType()==evtStr); return Str;}

  void SaveTxt(TOLx& Lx) const;
  void SaveTxt(const PSOut& SOut) const;
  TStr GetStr() const;

  static PExpVal MkClone(const PExpVal& ExpVal);
  static PExpVal GetUndefExpVal();
  static PExpVal GetZeroExpVal();
};

/////////////////////////////////////////////////
// Expression-Environment
typedef enum {
  efatUndef, efatVoid,
  efatFlt, efatFltFlt, efatFltStr,
  efatStr, efatStrFlt, efatStrStr,
  efatStrAny
} TExpFuncArgType;

ClassTP(TExpEnv, PExpEnv)//{
private:
  TRnd Rnd;
public:
  TExpEnv(): Rnd(){}
  virtual ~TExpEnv(){}
  TExpEnv(TSIn& SIn): Rnd(SIn){}
  static PExpEnv Load(TSIn& SIn){return new TExpEnv(SIn);}
  virtual void Save(TSOut& SOut) const {Rnd.Save(SOut);}

  TExpEnv& operator=(const TExpEnv&){Fail; return *this;}
  bool operator==(const TExpEnv&) const {Fail; return false;}

  virtual void PutVarVal(const TStr& /*VarNm*/, const PExpVal& /*ExpVal*/){Fail;}
  virtual PExpVal GetVarVal(const TStr& /*VarNm*/, bool& IsVar){
    IsVar=false; return TExpVal::GetUndefExpVal();}
  virtual PExpVal GetVarVal(const TStr& VarNm){
    bool IsVar; return GetVarVal(VarNm, IsVar);}
  virtual int GetVars() const {return 0;}
  virtual void GetVarNmVal(
   const int& /*VarN*/, TStr& /*VarNm*/, PExpVal& /*VarVal*/) const {Fail;}

  virtual PExpVal GetFuncVal(
   const TStr& /*FuncNm*/, const TExpValV& /*ArgValV*/, bool& IsFunc){
    IsFunc=false; return TExpVal::GetUndefExpVal();}

  virtual TRnd& GetRnd(){return Rnd;}

  static bool IsFuncOk(
   const TStr& RqFuncNm, const TExpFuncArgType& RqFuncArgType,
   const TStr& FuncNm, const TExpValV& ArgValV);

  static PExpEnv DfExpEnv;
};

/////////////////////////////////////////////////
// Expression-BuiltIn-Ids
typedef enum {
  // constants
  ebi_Undef, ebi_True, ebi_False, ebi_E, ebi_Pi,

  // trigonometric funcions
  ebi_Sin, ebi_Cos, ebi_Tan,
  ebi_ASin, ebi_ACos, ebi_ATan,
  ebi_SinH, ebi_CosH, ebi_TanH,

  // exponential functions
  ebi_Pow, ebi_Exp, ebi_Sqr, ebi_Sqrt,
  ebi_Log, ebi_Log10,

  // number manipulation functions
  ebi_Ceil, ebi_Floor, ebi_Int, ebi_Frac, ebi_Abs,

  // random deviates
  ebi_UniDev, ebi_NrmDev, ebi_ExpDev,
  ebi_GamDev, ebi_PoiDev, ebi_BinDev,
  ebi_UniDevStep, ebi_NrmDevStep, ebi_ExpDevStep,
} TExpBiId;

/////////////////////////////////////////////////
// Expression-BuiltIn-Argument-Type
typedef enum {
  ebatUndef, ebatVoid, ebatFlt, ebatFltFlt
} TExpBiArgType;

/////////////////////////////////////////////////
// Expression-BuiltIn
class TExpBi{
private:
  TStrIntH ExpBiNmToIdH;
  TIntIntH ExpBiIdToArgTypeH;
  PExpVal Val_Undef, Val_True, Val_False, Val_E, Val_Pi;
  void AddBi(const TStr& ExpBiNm, const TExpBiId& ExpBiId,
   const TExpBiArgType& ExpBiArgType=ebatUndef);
public:
  TExpBi();

  TExpBi& operator=(const TExpBi&){Fail; return *this;}
  bool operator==(const TExpBi&) const {Fail; return false;}

  bool IsExpBiId(const TStr& ExpBiNm, TExpBiId& ExpBiId);
  bool IsExpBiId(const TStr& ExpBiNm){
    TExpBiId ExpBiId; return IsExpBiId(ExpBiNm, ExpBiId);}
  TExpBiArgType GetExpBiArgType(const TExpBiId& ExpBiId);

  void AssertArgs(const int& RqArgs, const int& ActArgs);
  void AssertArgValType(const TExpValType& ExpValType, const PExpVal& ExpVal);

  PExpVal GetBiConstVal(const TExpBiId& ExpBiId);
  PExpVal GetBiFuncVal(
   const TExpBiId& ExpBiId, const TExpValV& ArgValV, const PExpEnv& ExpEnv);
};

/////////////////////////////////////////////////
// Expression-Type
typedef enum {
  etUndef,
  etVal, etVec, etLst,
  etOp, etVar, etBiConst, etFunc, etBiFunc} TExpType;

/////////////////////////////////////////////////
// Expression-Operator
typedef enum {
  eoUndef,
  eoUPlus, eoUMinus,
  eoPlus, eoMinus, eoMul, eoDiv, eoIDiv, eoMod,
  eoNot, eoAnd, eoOr,
  eoEq, eoNEq, eoLss, eoGtr, eoLEq, eoGEq,
  eoIf
} TExpOp;

/////////////////////////////////////////////////
// Expression
ClassTPV(TExp, PExp, TExpV)//{
private:
  static TExpBi ExpBi;
  TInt ExpType;
  TBool IsParen;
  PExpVal ExpVal;
  TStr ExpNm;
  TInt ExpOp;
  TInt ExpBiId;
  TExpV ArgExpV;
private:
  static const TFSet MulOpSymSet, UAddOpSymSet, AddOpSymSet, RelOpSymSet;
  static const TFSet FactExpExpect, MulExpExpect, AddExpExpect;
  static const TFSet RelExpExpect, ExpExpect;
  static TExpOp GetExpOpFromLxSym(const TLxSym& LxSym);
  static TLxSym GetLxSymFromExpOp(const TExpOp& ExpOp);
  static PExp LoadTxtFact(TILx& Lx, const TFSet& Expect);
  static PExp LoadTxtMulExp(TILx& Lx, const TFSet& Expect);
  static PExp LoadTxtAddExp(TILx& Lx, const TFSet& Expect);
  static PExp LoadTxtRelExp(TILx& Lx, const TFSet& Expect);
  static PExp LoadTxtExp(TILx& Lx, const TFSet& Expect);
  void SaveTxtOp(TOLx& Lx) const;
  PExpVal EvalExpOp(
   const PExpEnv& ExpEnv, const bool& DbgP, TChA& DbgChA);
  PExpVal EvalExp(
   const PExpEnv& ExpEnv, const bool& DbgP, TChA& DbgChA);
public:
  TExp(const TExpType& _ExpType=etUndef):
    ExpType(_ExpType), IsParen(false),
    ExpVal(), ExpNm(), ExpOp(), ExpBiId(), ArgExpV(){}
  TExp(const TExpOp& _ExpOp,
   const PExp& Exp1, const PExp& Exp2=NULL, const PExp& Exp3=NULL);
  TExp(const PExpVal& _ExpVal);
  TExp(const TStr& _VarNm);
  TExp(const TStr& _FuncNm, const TExpV& _ArgExpV);
  ~TExp(){}
  TExp(TSIn& SIn):
    ExpType(SIn), IsParen(SIn),
    ExpVal(SIn), ExpNm(SIn), ExpOp(SIn), ExpBiId(SIn), ArgExpV(SIn){}
  static PExp Load(TSIn& SIn){return PExp(new TExp(SIn));}
  void Save(TSOut& SOut) const {
    ExpType.Save(SOut); IsParen.Save(SOut);
    ExpVal.Save(SOut); ExpNm.Save(SOut); ExpOp.Save(SOut);
    ExpBiId.Save(SOut); ArgExpV.Save(SOut);}

  TExp& operator=(const TExp&){Fail; return *this;}
  bool operator==(const TExp&) const {Fail; return false;}

  static bool IsExpStrOk(const TStr& ExpStr, TStr& MsgStr){
    bool Ok; LoadTxt(ExpStr, Ok, MsgStr); return Ok;}
  static PExp LoadTxt(const PSIn& SIn, bool& Ok, TStr& MsgStr,
   const TFSet& Expect=TFSet()|syEof);
  static PExp LoadTxt(const TStr& ExpStr, bool& Ok, TStr& MsgStr){
    PSIn SIn=TStrIn::New(ExpStr); return LoadTxt(SIn, Ok, MsgStr);}
  static PExp LoadTxt(const TStr& ExpStr){
    bool Ok; TStr MsgStr; return LoadTxt(ExpStr, Ok, MsgStr);}

  void SaveTxt(TOLx& Lx) const;
  void SaveTxt(const PSOut& SOut) const {
    TOLx Lx(SOut, TFSet()|oloCmtAlw|oloSigNum|oloCsSens); SaveTxt(Lx);}
  TStr GetStr() const;
  TStr GetTopObjNm() const;
  int GetArgExps() const;
  TStr GetArgExpStr(const int& ArgExpN) const;

  PExpVal Eval(bool& Ok, TStr& MsgStr, const bool& DbgP, TStr& DbgStr,
   const PExpEnv& ExpEnv=TExpEnv::DfExpEnv);
  PExpVal Eval(const PExpEnv& ExpEnv=TExpEnv::DfExpEnv){
    bool Ok; TStr MsgStr; TStr DbgStr;
    return Eval(Ok, MsgStr, false, DbgStr, ExpEnv);}
  PExpVal Eval(bool& Ok, TStr& MsgStr, const PExpEnv& ExpEnv=TExpEnv::DfExpEnv){
    TStr DbgStr; return Eval(Ok, MsgStr, false, DbgStr, ExpEnv);}
  PExpVal Eval(const PExpEnv& ExpEnv, const bool& DbgP, TStr& DbgStr){
    bool Ok; TStr MsgStr; return Eval(Ok, MsgStr, DbgP, DbgStr, ExpEnv);}

  static PExpVal LoadAndEvalExpL(
   const TStr& ExpLStr, bool& Ok, TStr& MsgStr,
   const PExpEnv& ExpEnv=TExpEnv::DfExpEnv);

  static TStr GetExpTypeStr(const TExpType& ExpType);
  static TStr GetExpOpStr(const TExpOp& ExpOp){
    return TLxSymStr::GetSymStr(TExp::GetLxSymFromExpOp(ExpOp));}
  static void GetBiDescV(TStrPrV& BiDescV);
};

/////////////////////////////////////////////////
// Expression-Help-Item
ClassTPV(TExpHelpItem, PExpHelpItem, TExpHelpItemV)//{
private:
  TStr Nm;
  TStr TypeStr;
  TStr DescStr;
  TStr DfValStr;
public:
  TExpHelpItem(): Nm(), TypeStr(), DescStr(), DfValStr(){}
  TExpHelpItem(const TStr& _Nm, const TStr& _TypeStr,
   const TStr& _DescStr, const TStr& _DfValStr):
    Nm(_Nm), TypeStr(_TypeStr), DescStr(_DescStr), DfValStr(_DfValStr){}
  static PExpHelpItem New(const TStr& Nm, const TStr& TypeStr,
   const TStr& DescStr, const TStr& DfValStr){
    return PExpHelpItem(new TExpHelpItem(Nm, TypeStr, DescStr, DfValStr));}
  ~TExpHelpItem(){}
  TExpHelpItem(TSIn& SIn):
    Nm(SIn), TypeStr(SIn), DescStr(SIn), DfValStr(SIn){}
  static PExpHelpItem Load(TSIn& SIn){
    return PExpHelpItem(new TExpHelpItem(SIn));}
  void Save(TSOut& SOut) const {
    Nm.Save(SOut); TypeStr.Save(SOut); DescStr.Save(SOut); DfValStr.Save(SOut);}

  TExpHelpItem& operator=(const TExpHelpItem&){Fail; return *this;}
  bool operator==(const TExpHelpItem&) const {Fail; return false;}

  // component retrieval
  TStr GetNm() const {return Nm;}
  TStr GetTypeStr() const {return TypeStr;}
  TStr GetDescStr() const {return DescStr;}
  TStr GetDfValStr() const {return DfValStr;}
};

/////////////////////////////////////////////////
// Expression-Help-Object
typedef enum {ehotUndef, ehotOp, ehotVar, ehotFunc, ehotTempl} TExpHelpObjType;

ClassTPV(TExpHelpObj, PExpHelpObj, TExpHelpObjV)//{
private:
  TInt Type; // TExpHelpObjType
  TStr CatNm;
  PExpHelpItem HdItem;
  TExpHelpItemV ArgItemV;
public:
  TExpHelpObj(){}
  TExpHelpObj(const TExpHelpObjType& _Type, const TStr& _CatNm,
   const PExpHelpItem& _HdItem, const TExpHelpItemV& _ArgItemV):
    Type(_Type), CatNm(_CatNm), HdItem(_HdItem), ArgItemV(_ArgItemV){}
  static PExpHelpObj New(const TExpHelpObjType& Type, const TStr& CatNm,
   const PExpHelpItem& HdItem, const TExpHelpItemV& ArgItemV=TExpHelpItemV()){
    return PExpHelpObj(new TExpHelpObj(Type, CatNm, HdItem, ArgItemV));}
  ~TExpHelpObj(){}
  TExpHelpObj(TSIn& SIn):
    Type(SIn), CatNm(SIn), HdItem(SIn), ArgItemV(SIn){}
  static PExpHelpObj Load(TSIn& SIn){
    return PExpHelpObj(new TExpHelpObj(SIn));}
  void Save(TSOut& SOut) const {
    Type.Save(SOut); CatNm.Save(SOut); HdItem.Save(SOut); ArgItemV.Save(SOut);}

  TExpHelpObj& operator=(const TExpHelpObj&){Fail; return *this;}
  bool operator==(const TExpHelpObj&) const {Fail; return false;}

  // component retrieval
  TExpHelpObjType GetType() const {return TExpHelpObjType(int(Type));}
  TStr GetCatNm() const {return CatNm;}
  PExpHelpItem GetHdItem() const {return HdItem;}
  int GetArgItems() const {return ArgItemV.Len();}
  PExpHelpItem GetArgItem(const int& ArgItemN) const {
    return ArgItemV[ArgItemN];}

  // strings
  TStr GetHdArgNmStr() const;

  // object type
  static TExpHelpObjType GetObjTypeFromStr(const TStr& TypeStr);
};

/////////////////////////////////////////////////
// Expression-Help
ClassTP(TExpHelp, PExpHelp)//{
private:
  TExpHelpObjV ObjV;
public:
  TExpHelp(): ObjV(){}
  static PExpHelp New(){return PExpHelp(new TExpHelp());}
  ~TExpHelp(){}
  TExpHelp(TSIn& SIn): ObjV(SIn){}
  static PExpHelp Load(TSIn& SIn){return PExpHelp(new TExpHelp(SIn));}
  void Save(TSOut& SOut) const {ObjV.Save(SOut);}

  TExpHelp& operator=(const TExpHelp&){Fail; return *this;}
  bool operator==(const TExpHelp&) const {Fail; return false;}

  static PExpHelp LoadXml(const PSIn& SIn);
  static PExpHelp LoadXml(const TStr& FNm){
    PSIn SIn=TFIn::New(FNm); return LoadXml(SIn);}

  void AddObj(const PExpHelpObj& Obj){ObjV.Add(Obj);}

  void GetCatNmV(TStrV& CatNmV) const;
  void GetObjHdNmV(const TStr& CatNm, TStrV& ObjHdNmV) const;

  PExpHelpObj GetObj(const TStr& ObjNm) const;
};

