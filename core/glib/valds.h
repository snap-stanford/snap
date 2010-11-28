/////////////////////////////////////////////////
// Value-Distribution
ClassTPV(TTbValDs, PTbValDs, TTbValDsV)//{
private:
  TBool DefP;
private:
  TFlt SumW;
  TFltV ValTagWV;
  TFltV DscWV;
  PMom FltMom;
public:
  TTbValDs(const int& Dscs=0):
    DefP(), SumW(), ValTagWV(tvtMx), DscWV(Dscs), FltMom(TMom::New()){}
  static PTbValDs New(const int& Dscs=0){
    return PTbValDs(new TTbValDs(Dscs));}
  TTbValDs(TSIn& SIn):
    DefP(SIn), SumW(SIn), ValTagWV(SIn), DscWV(SIn), FltMom(SIn){}
  static PTbValDs Load(TSIn& SIn){return new TTbValDs(SIn);}
  void Save(TSOut& SOut){
    IAssert(DefP); DefP.Save(SOut);
    SumW.Save(SOut); ValTagWV.Save(SOut); DscWV.Save(SOut); FltMom.Save(SOut);}

  void Def(){
    IAssert(!DefP); DefP=true;
    IAssert(!(IsDsc()&&IsFlt())); FltMom->Def();}
  bool IsDef() const {return DefP;}

  TTbValDs& operator=(const TTbValDs&){Fail; return *this;}

  void AddVal(const TTbVal& Val, const double& Wgt=1);
  void ExpW();

  bool IsEmpty() const {Assert(DefP); return double(SumW)==0;}
  bool IsDsc() const {Assert(DefP); return double(ValTagWV[tvtDsc])!=0;}
  bool IsFlt() const {Assert(DefP); return double(ValTagWV[tvtFlt])!=0;}
  int GetDscs() const {Assert(DefP); return DscWV.Len();}

  double GetSumW() const {Assert(DefP); return SumW;}
  double GetSumPrb(const double& PriorSumW) const {
    Assert(DefP&&((double)SumW<=PriorSumW));
    if (PriorSumW==0){return 0;} else {return SumW/PriorSumW;}}
  double GetValTagW(const int& ValTag) const {
    Assert(DefP); return ValTagWV[ValTag];}
  double GetValW(const TTbVal& Val) const;
  PMom GetFltMom() const {Assert(DefP); return FltMom;}

  bool IsOneVal() const;
  TTbVal GetMajorVal() const;

  double GetPrb_RelFq(const TTbVal& Val) const;
  double GetPrb_Laplace(const TTbVal& Val) const;
  double GetPrb_QuinLaplace(const TTbVal& Val, const double& PriorPrb) const;
  double GetPrb_MEst(
   const TTbVal& Val, const double& MParam, const double& PriorPrb) const;

  TStr GetStr(const PTbVarType& VarType) const;

  static PTbValDs GetCValDs(
   const int& ClassN, const PDm& Dm, const PExSet& ExSet);
  static PTbValDs GetBoolValDs(const double& NegValW, const double& PosValW);
};

/////////////////////////////////////////////////
// Domain-Distribution
ClassTP(TDmDs, PDmDs)//{
private:
  UndefCopy(TDmDs);
public:
  TDmDs(){}
  virtual ~TDmDs(){}
  TDmDs(TSIn&){}
  static PDmDs Load(TSIn& SIn);
  void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut);}

  TDmDs& operator=(const TDmDs&){return *this;}

  virtual double GetSumW() const=0;
  virtual PTbValDs GetCDs() const=0;
  virtual PTbValDs GetAVDs(const int& AttrN) const=0;
  virtual PTbValDs GetCAVDs(const TTbVal& CVal, const int& AttrN) const=0;

  double GetCPrb_RelFq(const TTbVal& CVal){
    return GetCDs()->GetPrb_RelFq(CVal);}
  double GetCPrb_Laplace(const TTbVal& CVal){
    return GetCDs()->GetPrb_Laplace(CVal);}
  double GetCPrb_QuinLaplace(const TTbVal& CVal, const PDmDs& PriorDmDs){
    double PriorPrb=PriorDmDs->GetCPrb_RelFq(CVal);
    return GetCDs()->GetPrb_QuinLaplace(CVal, PriorPrb);}
  double GetCPrb_MEst(const TTbVal& CVal,
   const double& MParam, const PDmDs& PriorDmDs){
    double PriorPrb=PriorDmDs->GetCPrb_RelFq(CVal);
    return GetCDs()->GetPrb_MEst(CVal, MParam, PriorPrb);}

  double GetAVPrb_RelFq(const int& AttrN, const TTbVal& AVal){
    return GetAVDs(AttrN)->GetPrb_RelFq(AVal);}
  double GetAVPrb_Laplace(const int& AttrN, const TTbVal& AVal){
    return GetAVDs(AttrN)->GetPrb_Laplace(AVal);}
  double GetAVPrb_QuinLaplace(
   const int& AttrN, const TTbVal& AVal, const PDmDs& PriorDmDs){
    double PriorPrb=PriorDmDs->GetAVPrb_RelFq(AttrN, AVal);
    return GetAVDs(AttrN)->GetPrb_QuinLaplace(AVal, PriorPrb);}
  double GetAVPrb_MEst(const int& AttrN, const TTbVal& AVal,
   const double& MParam, const PDmDs& PriorDmDs){
    double PriorPrb=PriorDmDs->GetAVPrb_RelFq(AttrN, AVal);
    return GetAVDs(AttrN)->GetPrb_MEst(AVal, MParam, PriorPrb);}

  double GetCAVPrb_RelFq(const TTbVal& CVal,
   const int& AttrN, const TTbVal& AVal){
    return GetCAVDs(CVal, AttrN)->GetPrb_RelFq(AVal);}
  double GetCAVPrb_Laplace(const TTbVal& CVal,
   const int& AttrN, const TTbVal& AVal){
    return GetCAVDs(CVal, AttrN)->GetPrb_Laplace(AVal);}
  double GetCAVPrb_QuinLaplace(const TTbVal& CVal,
   const int& AttrN, const TTbVal& AVal, const PDmDs& PriorDmDs){
    double PriorPrb=PriorDmDs->GetCAVPrb_RelFq(CVal, AttrN, AVal);
    return GetCAVDs(CVal, AttrN)->GetPrb_QuinLaplace(AVal, PriorPrb);}
  double GetCAVPrb_MEst(const TTbVal& CVal,
   const int& AttrN, const TTbVal& AVal,
   const double& MParam, const PDmDs& PriorDmDs){
    double PriorPrb=PriorDmDs->GetCAVPrb_RelFq(CVal, AttrN, AVal);
    return GetCAVDs(CVal, AttrN)->GetPrb_MEst(AVal, MParam, PriorPrb);}
};

/////////////////////////////////////////////////
// General-Domain-Distribution
class TGDmDs: public TDmDs{
private:
  TFlt SumW;
  PTbValDs CDs;
  TTbValDsV AVDsV;
  TVVec<PTbValDs> CAVDsVV;
  inline void AddVal(
   const TTbVal& CVal, const int& AttrN, const TTbVal& AVal, const double& Wgt);
public:
  TGDmDs(const PDm& Dm, const int& ClassN, const PExSet& ExSet);
  static PDmDs New(const PDm& Dm, const int& ClassN, const PExSet& ExSet){
    return PDmDs(new TGDmDs(Dm, ClassN, ExSet));}
  TGDmDs(TSIn& SIn):
    SumW(SIn), CDs(SIn), AVDsV(SIn), CAVDsVV(SIn){}
  void Save(TSOut& SOut){
    SumW.Save(SOut); CDs.Save(SOut); AVDsV.Save(SOut); CAVDsVV.Save(SOut);}

  TGDmDs& operator=(const TGDmDs& GDmDs){
    TDmDs::operator=(GDmDs);
    SumW=GDmDs.SumW; CDs=GDmDs.CDs; AVDsV=GDmDs.AVDsV; CAVDsVV=GDmDs.CAVDsVV;
    return *this;}

  double GetSumW() const {return SumW;}
  PTbValDs GetCDs() const {return CDs;}
  PTbValDs GetAVDs(const int& AttrN) const {return AVDsV[AttrN];}
  PTbValDs GetCAVDs(const TTbVal& CVal, const int& AttrN) const {
    return CAVDsVV.At(CVal.GetDsc(), AttrN);}
};

