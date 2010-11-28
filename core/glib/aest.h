/////////////////////////////////////////////////
// Attribute-Estimator
ClassTPV(TAttrEst, PAttrEst, TAttrEstV)//{
private:
  TStr Nm;
  UndefDefaultCopyAssign(TAttrEst);
public:
  TAttrEst(const TStr& _Nm): Nm(_Nm){}
  virtual ~TAttrEst(){}
  TAttrEst(TSIn& SIn): Nm(SIn){}
  static PAttrEst Load(TSIn&);
  virtual void Save(TSOut& SOut){
    GetTypeNm(*this).Save(SOut); Nm.Save(SOut);}

  virtual double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs)=0;
  TStr GetNm() const {return Nm;}

  static PTbValDs GetCSValDs(const int& AttrN, const int& SplitN,
   const PTbValSplit& ValSplit, const PDmDs& DmDs);
  static PTbValDs GetSValDs(const int& AttrN,
   const PTbValSplit& ValSplit, const PDmDs& DmDs);
  static PTbValDs GetSCValDs(const int& CDsc, const int& AttrN,
   const PTbValSplit& ValSplit, const PDmDs& DmDs);
  static double GetCEntropy(
   const PDmDs& DmDs, const PDmDs& PriorDmDs, const PPrbEst& PrbEst);
  static double GetAEntropy(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs, const PPrbEst& PrbEst);
  static double GetCAEntropy(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs, const PPrbEst& PrbEst);
  static PPrbEst GetPrbEst(const PPrbEst& PrbEst);
  static PPp GetPrbEstPp(const TStr& Nm, const TStr& DNm);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static PAttrEst New(const PPp& Pp);
};

/////////////////////////////////////////////////
// Attribute-Estimator-Random
class TAttrEstRnd: public TAttrEst{
private:
  TRnd Rnd;
public:
  TAttrEstRnd(const int& Seed=1): TAttrEst(DNm), Rnd(Seed){}
  static PAttrEst New(const int& Seed=1){
    return PAttrEst(new TAttrEstRnd(Seed));}
  TAttrEstRnd(const PPp& Pp): TAttrEst(DNm), Rnd(Pp->GetValInt("Seed")){}
  TAttrEstRnd(TSIn& SIn): TAttrEst(SIn), Rnd(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); Rnd.Save(SOut);}

  double GetAttrQ(const int&, const PTbValSplit&, const PDmDs&, const PDmDs&){
    return Rnd.GetUniDev();}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

/////////////////////////////////////////////////
// Attribute-Estimator-Information-Gain
class TAttrEstIGain: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstIGain(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstIGain(PrbEst));}
  TAttrEstIGain(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstIGain(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return TAttrEst::GetCEntropy(DmDs, PriorDmDs, PrbEst)+
     TAttrEst::GetAEntropy(AttrN, ValSplit, DmDs, PriorDmDs, PrbEst)-
     TAttrEst::GetCAEntropy(AttrN, ValSplit, DmDs, PriorDmDs, PrbEst);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Information-Gain-Normalized
class TAttrEstIGainNorm: public TAttrEst{
private:
  TAttrEstIGain IGain;
public:
  TAttrEstIGainNorm(const PPrbEst& PrbEst=NULL):
    TAttrEst(DNm), IGain(GetPrbEst(PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstIGainNorm(PrbEst));}
  TAttrEstIGainNorm(const PPp& Pp): TAttrEst(DNm), IGain(Pp){}
  TAttrEstIGainNorm(TSIn& SIn): TAttrEst(SIn), IGain(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); IGain.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return IGain.GetAttrQ(AttrN, ValSplit, DmDs, PriorDmDs)/
     TMath::Log2(ValSplit->GetSplits());}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){
    return TAttrEstIGain::GetPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Information-Gain-Ratio
class TAttrEstIGainRatio: public TAttrEst{
private:
  PPrbEst PrbEst;
  TAttrEstIGain IGain;
public:
  TAttrEstIGainRatio(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)), IGain(PrbEst){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstIGainRatio(PrbEst));}
  TAttrEstIGainRatio(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))),
    IGain(PrbEst){}
  TAttrEstIGainRatio(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn), IGain(SIn){}
  void Save(TSOut& SOut){
    TAttrEst::Save(SOut); PrbEst->Save(SOut); IGain.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return IGain.GetAttrQ(AttrN, ValSplit, DmDs, PriorDmDs)/
     TAttrEst::GetAEntropy(AttrN, ValSplit, DmDs, PriorDmDs, PrbEst);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Mantaras-Distance
class TAttrEstMantarasDist: public TAttrEst{
private:
  PPrbEst PrbEst;
  TAttrEstIGain IGain;
public:
  TAttrEstMantarasDist(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)), IGain(PrbEst){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstMantarasDist(PrbEst));}
  TAttrEstMantarasDist(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))),
    IGain(PrbEst){}
  TAttrEstMantarasDist(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn), IGain(SIn){}
  void Save(TSOut& SOut){
    TAttrEst::Save(SOut); PrbEst->Save(SOut); IGain.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
    return IGain.GetAttrQ(AttrN, ValSplit, DmDs, PriorDmDs)/
     TAttrEst::GetCAEntropy(AttrN, ValSplit, DmDs, PriorDmDs, PrbEst);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-MDL
class TAttrEstMdl: public TAttrEst{
private:
  PPrbEst PrbEst;
  TAttrEstIGain IGain;
public:
  TAttrEstMdl(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)), IGain(PrbEst){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstMdl(PrbEst));}
  TAttrEstMdl(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))),
    IGain(PrbEst){}
  TAttrEstMdl(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn), IGain(SIn){}
  void Save(TSOut& SOut){
    TAttrEst::Save(SOut); PrbEst->Save(SOut); IGain.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-G-Statistics
class TAttrEstGStat: public TAttrEst{
private:
  TAttrEstIGain IGain;
public:
  TAttrEstGStat(const PPrbEst& PrbEst=NULL):
    TAttrEst(DNm), IGain(GetPrbEst(PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstGStat(PrbEst));}
  TAttrEstGStat(const PPp& Pp): TAttrEst(DNm), IGain(Pp){}
  TAttrEstGStat(TSIn& SIn): TAttrEst(SIn), IGain(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); IGain.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs){
     return 2*TMath::LogOf2*DmDs->GetSumW()*
     IGain.GetAttrQ(AttrN, ValSplit, DmDs, PriorDmDs);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){
    return TAttrEstIGain::GetPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Chi-Square
class TAttrEstChiSquare: public TAttrEst{
public:
  TAttrEstChiSquare(): TAttrEst(DNm){}
  static PAttrEst New(){return PAttrEst(new TAttrEstChiSquare());}
  TAttrEstChiSquare(const PPp&): TAttrEst(DNm){}
  TAttrEstChiSquare(TSIn& SIn): TAttrEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-ORT
class TAttrEstOrt: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstOrt(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstOrt(PrbEst));}
  TAttrEstOrt(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstOrt(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Gini
class TAttrEstGini: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstGini(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstGini(PrbEst));}
  TAttrEstGini(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstGini(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Weight-Of-Evidence
class TAttrEstWgtEvd: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstWgtEvd(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstWgtEvd(PrbEst));}
  TAttrEstWgtEvd(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstWgtEvd(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Text-Weight-Of-Evidence
class TAttrEstTextWgtEvd: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstTextWgtEvd(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstTextWgtEvd(PrbEst));}
  TAttrEstTextWgtEvd(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstTextWgtEvd(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Odds-Ratio
class TAttrEstOddsRatio: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstOddsRatio(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstOddsRatio(PrbEst));}
  TAttrEstOddsRatio(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstOddsRatio(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Weighted-Odds-Ratio
class TAttrEstWgtOddsRatio: public TAttrEst{
private:
  TAttrEstOddsRatio OddsRatio;
public:
  TAttrEstWgtOddsRatio(const PPrbEst& PrbEst=NULL):
    TAttrEst(DNm), OddsRatio(GetPrbEst(PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstWgtOddsRatio(PrbEst));}
  TAttrEstWgtOddsRatio(const PPp& Pp): TAttrEst(DNm), OddsRatio(Pp){}
  TAttrEstWgtOddsRatio(TSIn& SIn): TAttrEst(SIn), OddsRatio(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); OddsRatio.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){
    return TAttrEstOddsRatio::GetPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Conditional-Odds-Ratio
class TAttrEstCondOddsRatio: public TAttrEst{
private:
  PPrbEst PrbEst;
  TFlt InvTsh;
  TFlt InvWgt;
public:
  TAttrEstCondOddsRatio(const PPrbEst& _PrbEst=NULL,
   const double& _InvTsh=0.9, const double& _InvWgt=0.1):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)),
    InvTsh(_InvTsh), InvWgt(_InvWgt){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL,
   const double& InvTsh=0.9, const double& InvWgt=0.1){
    return PAttrEst(new TAttrEstCondOddsRatio(PrbEst, InvTsh, InvWgt));}
  TAttrEstCondOddsRatio(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))),
    InvTsh(Pp->GetValFlt("InvTsh")), InvWgt(Pp->GetValFlt("InvWgt")){}
  TAttrEstCondOddsRatio(TSIn& SIn):
    TAttrEst(SIn), PrbEst(SIn), InvTsh(SIn), InvWgt(SIn){}
  void Save(TSOut& SOut){
    TAttrEst::Save(SOut); PrbEst->Save(SOut);
    InvTsh.Save(SOut); InvWgt.Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

/////////////////////////////////////////////////
// Attribute-Estimator-Log-Probability-Ratio
class TAttrEstLogPRatio: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstLogPRatio(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstLogPRatio(PrbEst));}
  TAttrEstLogPRatio(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstLogPRatio(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Exp-Probability-Difference
class TAttrEstExpPDiff: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstExpPDiff(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstExpPDiff(PrbEst));}
  TAttrEstExpPDiff(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstExpPDiff(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Mutual-Information
class TAttrEstMutInf: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstMutInf(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstMutInf(PrbEst));}
  TAttrEstMutInf(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstMutInf(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Cross-Entropy
class TAttrEstCrossEnt: public TAttrEst{
private:
  PPrbEst PrbEst;
public:
  TAttrEstCrossEnt(const PPrbEst& _PrbEst=NULL):
    TAttrEst(DNm), PrbEst(GetPrbEst(_PrbEst)){}
  static PAttrEst New(const PPrbEst& PrbEst=NULL){
    return PAttrEst(new TAttrEstCrossEnt(PrbEst));}
  TAttrEstCrossEnt(const PPp& Pp):
    TAttrEst(DNm), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}
  TAttrEstCrossEnt(TSIn& SIn): TAttrEst(SIn), PrbEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut); PrbEst->Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return GetPrbEstPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Attribute-Estimator-Term-Frequency
class TAttrEstTermFq: public TAttrEst{
public:
  TAttrEstTermFq(): TAttrEst(DNm){}
  static PAttrEst New(){return PAttrEst(new TAttrEstTermFq());}
  TAttrEstTermFq(const PPp&): TAttrEst(DNm){}
  TAttrEstTermFq(TSIn& SIn): TAttrEst(SIn){}
  void Save(TSOut& SOut){TAttrEst::Save(SOut);}

  double GetAttrQ(const int& AttrN, const PTbValSplit& ValSplit,
   const PDmDs& DmDs, const PDmDs& PriorDmDs);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

