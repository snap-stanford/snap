/////////////////////////////////////////////////
// Forward
class TPlBs;

/////////////////////////////////////////////////
// Prolog-Value
typedef enum {pvtUndef, pvtBool, pvtInt, pvtFlt, pvtAtom, pvtTup} TPlValType;

#pragma pack(push, 1) // pack class size
ClassTV(TPlVal, TPlValV)//{
private:
  char ValType;
  union{
    int All;
    bool Bool;
    int Int;
    sdouble Flt;
    int AtomId;
    int TupId;} Val;
public:
  TPlVal(){ValType=pvtUndef;}
  TPlVal(const TPlVal& PlVal){ValType=PlVal.ValType; Val=PlVal.Val;}
  TPlVal(TSIn& SIn){SIn.Load(ValType); SIn.Load(Val.All);}
  void Save(TSOut& SOut){SOut.Save(ValType); SOut.Save(Val.All);}

  TPlVal& operator=(const TPlVal& PlVal){
    ValType=PlVal.ValType; Val=PlVal.Val; return *this;}
  bool operator==(const TPlVal& PlVal) const;
  bool operator<(const TPlVal& PlVal) const;

  bool IsUndef() const {return ValType==pvtUndef;}
  bool IsBool() const {return ValType==pvtBool;}
  bool IsInt() const {return ValType==pvtInt;}
  bool IsFlt() const {return ValType==pvtFlt;}
  bool IsAtom() const {return ValType==pvtAtom;}
  bool IsTup() const {return ValType==pvtTup;}

  TPlValType GetValType() const {return (TPlValType)ValType;}
  int GetBool() const {Assert(ValType==pvtBool); return Val.Int;}
  int GetInt() const {Assert(ValType==pvtInt); return Val.Int;}
  double GetFlt() const {Assert(ValType==pvtFlt); return Val.Flt;}
  int GetAtomId() const {Assert(ValType==pvtAtom); return Val.AtomId;}
  int GetTupId() const {Assert(ValType==pvtTup); return Val.TupId;}

  void PutUndef(){ValType=pvtUndef;}
  void PutBool(const bool& Bool){ValType=pvtBool; Val.Bool=Bool;}
  void PutInt(const int& Int){ValType=pvtInt; Val.Int=Int;}
  void PutFlt(const double& Flt){ValType=pvtFlt; Val.Flt=sdouble(Flt);}
  void PutAtomId(const int& AtomId){ValType=pvtAtom; Val.AtomId=AtomId;}
  void PutTupId(const int& TupId){ValType=pvtTup; Val.TupId=TupId;}

  static TStr GetValTypeStr(const int& ValType);
  TStr GetStr(TPlBs* PlBs) const;
};
#pragma pack(pop)

/////////////////////////////////////////////////
// Prolog-Tuple
ClassTPV(TPlTup, PPlTup, TPlTupV)//{
private:
  TInt FuncId; // AtomId
  TPlValV ValV;
public:
  TPlTup(): FuncId(), ValV(){}
  TPlTup(const TPlTup& PlTup):
    FuncId(PlTup.FuncId), ValV(PlTup.ValV){}
  TPlTup(const int& _FuncId, const TPlValV& _PlValV):
    FuncId(_FuncId), ValV(_PlValV){}
  static PPlTup New(){
    return new TPlTup();}
  static PPlTup New(const int& FuncId, const TPlValV& ValV){
    return new TPlTup(FuncId, ValV);}
  ~TPlTup(){}
  TPlTup(TSIn& SIn): FuncId(SIn), ValV(SIn){}
  static PPlTup Load(TSIn& SIn){return new TPlTup(SIn);}
  void Save(TSOut& SOut){FuncId.Save(SOut); ValV.Save(SOut);}

  void PutFuncId(const int& _FuncId){FuncId=_FuncId;}
  int GetFuncId() const {return FuncId;}
  void AddVal(const TPlVal& Val){ValV.Add(Val);}
  int GetVals() const {return ValV.Len();}
  TPlVal GetVal(const int& ValN) const {return ValV[ValN];}

  TStr GetStr(TPlBs* PlBs) const;
};

/////////////////////////////////////////////////
// Prolog-Base
ClassTPV(TPlBs, PPlBs, TPlBsV)//{
private:
  TStrSH AtomH;
  TPlTupV TupV;
  TIntPrIntVH FuncIdArityPrToTupIdVH;
  UndefAssign(TPlBs);
public:
  TPlBs():
    AtomH(), TupV(), FuncIdArityPrToTupIdVH(){}
  TPlBs(const TPlBs& PlBs):
    AtomH(PlBs.AtomH), TupV(PlBs.TupV),
    FuncIdArityPrToTupIdVH(PlBs.FuncIdArityPrToTupIdVH){}
  static PPlBs New(){return new TPlBs();}
  ~TPlBs(){}
  TPlBs(TSIn& SIn):
    AtomH(SIn), TupV(SIn), FuncIdArityPrToTupIdVH(SIn){}
  static PPlBs Load(TSIn& SIn){return new TPlBs(SIn);}
  void Save(TSOut& SOut){
    AtomH.Save(SOut); TupV.Save(SOut); FuncIdArityPrToTupIdVH.Save(SOut);}

  // atoms
  int GetAtoms() const {return AtomH.Len();}
  TStr GetAtomStr(const int& AtomId) const {
    return AtomH.GetKey(AtomId);}
  int GetAtomId(const TStr& AtomStr) const {
    return AtomH.GetKeyId(AtomStr);}
  bool IsAtomQuoted(const int& AtomId) const {
    return !GetAtomStr(AtomId).IsWord(false, false);}
  TStr GetAtomQStr(const int& AtomId) const {
    return TILx::GetQStr(GetAtomStr(AtomId), IsAtomQuoted(AtomId), '\'');}

  // tuples
  int GetTups() const {return TupV.Len();}
  int AddTup(const PPlTup& Tup){
    return TupV.Add(Tup);}
  PPlTup GetTup(const int& TupId) const {
    return TupV[TupId];}

  // relations
  int GetRels() const {return FuncIdArityPrToTupIdVH.Len();}
  int GetRelId(const int& FuncId, const int& Arity){
    return FuncIdArityPrToTupIdVH.GetKeyId(TIntPr(FuncId, Arity));}
  int GetRelId(const TStr& FuncStr, const int& Arity){
    return GetRelId(GetAtomId(FuncStr), Arity);}
  int GetRelFuncId(const int& RelId) const {
    return FuncIdArityPrToTupIdVH.GetKey(RelId).Val1;}
  TStr GetRelFuncStr(const int& RelId) const {
    return GetAtomStr(GetRelFuncId(RelId));}
  int GetRelArity(const int& RelId) const {
    return FuncIdArityPrToTupIdVH.GetKey(RelId).Val2;}
  TStr GetFuncArityStr(const int& RelId) const {
    return GetAtomQStr(GetRelFuncId(RelId))+"/"+TInt::GetStr(GetRelArity(RelId));}
  int GetRelTups(const int& RelId) const {
    return FuncIdArityPrToTupIdVH[RelId].Len();}
  int GetRelTupId(const int& RelId, const int& TupN) const {
    return FuncIdArityPrToTupIdVH[RelId][TupN];}

  // files
  static PPlBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm);
public:
  // parsing
  static const TFSet TupExpect, ExpExpect;
  static TPlVal ParseTup(TILx& Lx, const TFSet& Expect, const PPlBs& PlBs);
  static TPlVal ParseExp(TILx& Lx, const TFSet& Expect, const PPlBs& PlBs);
  static PPlBs LoadTxtPl(const TStr& FNmWc);
};

