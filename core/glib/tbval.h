/////////////////////////////////////////////////
// Table-Value
typedef enum {
  tvtUndef, tvtUnknw, tvtUnimp, tvtUnapp, tvtDsc, tvtFlt, tvtMx} TTbValTag;

class TTbVal{
private:
  char ValTag;
  union{
    double All;
    int Dsc;
    double Flt;} Val;
public:
  TTbVal(){ValTag=tvtUndef;}
  TTbVal(const TTbVal& TbVal){ValTag=TbVal.ValTag; Val=TbVal.Val;}
  TTbVal(const TTbValTag& _TbValTag){ValTag=(char)_TbValTag;
    Assert((ValTag==tvtUnknw)||(ValTag==tvtUnimp)||(ValTag==tvtUnapp));}
  TTbVal(const bool& Dsc){ValTag=tvtDsc; Val.Dsc=Dsc;}
  TTbVal(const int& Dsc){ValTag=tvtDsc; Val.Dsc=Dsc;}
  TTbVal(const double& Flt){ValTag=tvtFlt; Val.Flt=Flt;}
  TTbVal(TSIn& SIn){SIn.Load(ValTag); SIn.Load(Val.All);}
  void Save(TSOut& SOut){SOut.Save(ValTag); SOut.Save(Val.All);}

  TTbVal& operator=(const TTbVal& TbVal){
    ValTag=TbVal.ValTag; Val=TbVal.Val; return *this;}
  bool operator==(const TTbVal& TbVal) const;
  bool operator<(const TTbVal& TbVal) const;

  bool IsUndef() const {return ValTag==tvtUndef;}
  bool IsUnknw() const {return ValTag==tvtUnknw;}
  bool IsUnimp() const {return ValTag==tvtUnimp;}
  bool IsUnapp() const {return ValTag==tvtUnapp;}
  bool IsDsc() const {return ValTag==tvtDsc;}
  bool IsFlt() const {return ValTag==tvtFlt;}

  TTbValTag GetValTag() const {return (TTbValTag)ValTag;}
  int GetDsc() const {Assert(ValTag==tvtDsc); return Val.Dsc;}
  double GetFlt() const {Assert(ValTag==tvtFlt); return Val.Flt;}

  void PutUndef(){ValTag=tvtUndef;}
  void PutUnknw(){ValTag=tvtUnknw;}
  void PutUnimp(){ValTag=tvtUnimp;}
  void PutUnapp(){ValTag=tvtUnapp;}
  void PutDsc(const int& Dsc){ValTag=tvtDsc; Val.Dsc=Dsc;}
  void PutFlt(const double& Flt){ValTag=tvtFlt; Val.Flt=Flt;}

  static TStr GetValTagStr(const int& ValTag);
  TStr GetStr() const;

  static const TTbVal NegVal;
  static const TTbVal PosVal;
  static const int BoolVals;
};
typedef TVec<TTbVal> TTbValV;

/////////////////////////////////////////////////
// Table-Value-Selector
class TTbValSel{
private:
  TInt ROp;
  TTbVal Val;
public:
  TTbValSel(): ROp(), Val(){}
  TTbValSel(const TTbVal& _Val): ROp(roEq), Val(_Val){}
  TTbValSel(const TRelOp& _Op, const TTbVal& _Val): ROp(_Op), Val(_Val){}
  TTbValSel(TSIn& SIn): ROp(SIn), Val(SIn){}
  void Save(TSOut& SOut){ROp.Save(SOut); Val.Save(SOut);}

  TTbValSel& operator=(const TTbValSel& Sel){
    ROp=Sel.ROp; Val=Sel.Val; return *this;}
  bool operator==(const TTbValSel& Sel) const {
    return (ROp==Sel.ROp)&&(Val==Sel.Val);}

  bool In(const TTbVal& _Val){return Cmp(int(ROp), _Val, Val);}
};

/////////////////////////////////////////////////
// Table-Value-Set
ClassTPV(TTbValSet, PTbValSet, TTbValSetV)//{
private:
  TBool DefP;
  UndefCopyAssign(TTbValSet);
public:
  TTbValSet(){}
  virtual ~TTbValSet(){}
  TTbValSet(TSIn& SIn): DefP(SIn){}
  static PTbValSet Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){Assert(DefP);
    GetTypeNm(*this).Save(SOut); DefP.Save(SOut);}

  virtual void Def(){Assert(!DefP); DefP=true;}
  bool IsDef() const {return DefP;}

  virtual bool In(const TTbVal& Val)=0;
  virtual int InN(const TTbVal& Val)=0;
  virtual void Print()=0;
};

/////////////////////////////////////////////////
// Table-Value-Set-Expression
class TTbValSetExp: public TTbValSet{
private:
  TInt LOp;
  TTbValSetV TbValSetV;
public:
  TTbValSetExp(const TLogOp& _LOp): TTbValSet(), LOp(_LOp), TbValSetV(){}
  TTbValSetExp(TSIn& SIn): TTbValSet(SIn), LOp(SIn), TbValSetV(SIn){}
  void Save(TSOut& SOut){
    TTbValSet::Save(SOut); LOp.Save(SOut); TbValSetV.Save(SOut);}

  void Add(const PTbValSet& TbValSet){
    Assert(!IsDef()); TbValSetV.Add(TbValSet);}
  bool In(const TTbVal& Val);
  int InN(const TTbVal& Val);
  void Print(){printf("[TTbValSetExp]");}
};

/////////////////////////////////////////////////
// Table-Value-Set-Selector
class TTbValSetSel: public TTbValSet{
private:
  TTbValSel Sel;
public:
  TTbValSetSel(const TTbValSel& _Sel): TTbValSet(), Sel(_Sel){Def();}
  TTbValSetSel(TSIn& SIn): TTbValSet(SIn), Sel(SIn){}
  void Save(TSOut& SOut){TTbValSet::Save(SOut); Sel.Save(SOut);}

  bool In(const TTbVal& Val){return Sel.In(Val);}
  int InN(const TTbVal& Val){if (Sel.In(Val)){return 0;} else {return -1;}}
  void Print(){printf("[TTbValSetSel]");}
};

/////////////////////////////////////////////////
// Table-Value-Split
ClassTPV(TTbValSplit, PTbValSplit, TTbValSplitV)//{
private:
  TVec<TTbValV> TbValVV;
public:
  TTbValSplit(const int& Splits=0): TbValVV(Splits){}
  static PTbValSplit New(const int& Splits=0){
    return PTbValSplit(new TTbValSplit(Splits));}
  TTbValSplit(TSIn& SIn): TbValVV(SIn){}
  static PTbValSplit Load(TSIn& SIn){return new TTbValSplit(SIn);}
  void Save(TSOut& SOut){TbValVV.Save(SOut);}

  TTbValSplit& operator=(const TTbValSplit& TbValSplit){
    TbValVV=TbValSplit.TbValVV; return *this;}

  int AddSplit(){return TbValVV.Add();}
  void AddSplitVal(const int& SplitN, const TTbVal& TbVal){
    TbValVV[SplitN].Add(TbVal);}
  void AddBinSplitDscValsPerBits(const int& Dscs, const TB32Set& BSet);

  int GetSplits() const {return TbValVV.Len();}
  int GetSplitVals(const int& SplitN) const {return TbValVV[SplitN].Len();}
  TTbVal GetSplitVal(const int& SplitN, const int& ValN) const {
    return TbValVV[SplitN][ValN];}
  int GetSplitN(const TTbVal& TbVal) const;

  TStr GetStr() const;

  static PTbValSplit GetBoolValSplit();
};

