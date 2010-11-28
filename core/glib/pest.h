/////////////////////////////////////////////////
// Probability-Estimator
ClassTP(TPrbEst, PPrbEst)//{
private:
  UndefCopyAssign(TPrbEst);
public:
  TPrbEst(){}
  virtual ~TPrbEst(){}
  TPrbEst(TSIn&){}
  static PPrbEst Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut);}

  virtual double GetVPrb(const TTbVal& Val,
   const PTbValDs& ValDs, const PTbValDs& PriorValDs)=0;
  virtual double GetCPrb(const TTbVal& CVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs)=0;
  virtual double GetAVPrb(const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs)=0;
  virtual double GetCAVPrb(
   const TTbVal& CVal, const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs)=0;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static PPrbEst New(const PPp& Pp);

  static PPrbEst GetRelFq();
};

/////////////////////////////////////////////////
// Probability-Estimator-Relative-Frequency
class TPrbEstRelFq: public TPrbEst{
public:
  TPrbEstRelFq(): TPrbEst(){}
  TPrbEstRelFq(const PPp&): TPrbEst(){}
  TPrbEstRelFq(TSIn& SIn): TPrbEst(SIn){}
  void Save(TSOut& SOut){TPrbEst::Save(SOut);}

  double GetVPrb(const TTbVal& Val, const PTbValDs& ValDs, const PTbValDs&){
    return ValDs->GetPrb_RelFq(Val);}
  double GetCPrb(const TTbVal& CVal, const PDmDs& DmDs, const PDmDs&){
    return DmDs->GetCPrb_RelFq(CVal);}
  double GetAVPrb(const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs&){
    return DmDs->GetAVPrb_RelFq(AttrN, AVal);}
  double GetCAVPrb(const TTbVal& CVal, const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs&){
    return DmDs->GetCAVPrb_RelFq(CVal, AttrN, AVal);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Probability-Estimator-Laplace
class TPrbEstLaplace: public TPrbEst{
public:
  TPrbEstLaplace(): TPrbEst(){}
  TPrbEstLaplace(const PPp&): TPrbEst(){}
  TPrbEstLaplace(TSIn& SIn): TPrbEst(SIn){}
  void Save(TSOut& SOut){TPrbEst::Save(SOut);}

  double GetVPrb(const TTbVal& Val, const PTbValDs& ValDs, const PTbValDs&){
    return ValDs->GetPrb_Laplace(Val);}
  double GetCPrb(const TTbVal& CVal, const PDmDs& DmDs, const PDmDs&){
    return DmDs->GetCPrb_Laplace(CVal);}
  double GetAVPrb(const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs&){
    return DmDs->GetAVPrb_Laplace(AttrN, AVal);}
  double GetCAVPrb(const TTbVal& CVal, const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs&){
    return DmDs->GetCAVPrb_Laplace(CVal, AttrN, AVal);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Probability-Estimator-Quinlan-Laplace
class TPrbEstQuinLaplace: public TPrbEst{
public:
  TPrbEstQuinLaplace(): TPrbEst(){}
  TPrbEstQuinLaplace(const PPp&): TPrbEst(){}
  TPrbEstQuinLaplace(TSIn& SIn): TPrbEst(SIn){}
  void Save(TSOut& SOut){TPrbEst::Save(SOut);}

  double GetVPrb(const TTbVal& Val, const PTbValDs& ValDs,
   const PTbValDs& PriorValDs){
    return ValDs->GetPrb_QuinLaplace(Val, PriorValDs->GetPrb_RelFq(Val));}
  double GetCPrb(const TTbVal& CVal, const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return DmDs->GetCPrb_QuinLaplace(CVal, PriorDmDs);}
  double GetAVPrb(const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return DmDs->GetAVPrb_QuinLaplace(AttrN, AVal, PriorDmDs);}
  double GetCAVPrb(const TTbVal& CVal, const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return DmDs->GetCAVPrb_QuinLaplace(CVal, AttrN, AVal, PriorDmDs);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Probability-Estimator-M-Estimate
class TPrbEstMEst: public TPrbEst{
private:
  static const double DfMParam;
  TFlt MParam;
public:
  TPrbEstMEst(const double& _MParam=DfMParam): TPrbEst(), MParam(_MParam){}
  TPrbEstMEst(const PPp& Pp): TPrbEst(), MParam(Pp->GetValFlt("MParam")){}
  TPrbEstMEst(TSIn& SIn): TPrbEst(SIn), MParam(SIn){}
  void Save(TSOut& SOut){TPrbEst::Save(SOut); MParam.Save(SOut);}

  double GetVPrb(const TTbVal& Val, const PTbValDs& ValDs,
   const PTbValDs& PriorValDs){
    return ValDs->GetPrb_MEst(Val, MParam, PriorValDs->GetPrb_RelFq(Val));}
  double GetCPrb(const TTbVal& CVal, const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return DmDs->GetCPrb_MEst(CVal, MParam, PriorDmDs);}
  double GetAVPrb(const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return DmDs->GetAVPrb_MEst(AttrN, AVal, MParam, PriorDmDs);}
  double GetCAVPrb(const TTbVal& CVal, const int& AttrN, const TTbVal& AVal,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return DmDs->GetCAVPrb_MEst(CVal, AttrN, AVal, MParam, PriorDmDs);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

