#ifndef Ts_h
#define Ts_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "mg.h"

/////////////////////////////////////////////////
// Test-Prediction
class TTsPred{
private:
  TCRef CRef;
private:
  PTbValDs PredValDs;
  TTbVal CorrVal;
  TFlt Wgt;
public:
  TTsPred(const PTbValDs& _PredValDs, const TTbVal& _CorrVal, const double& _Wgt):
    PredValDs(_PredValDs), CorrVal(_CorrVal), Wgt(_Wgt){}
  TTsPred(TSIn& SIn): PredValDs(SIn), CorrVal(SIn), Wgt(SIn){}
  static TPt<TTsPred> Load(TSIn& SIn){return new TTsPred(SIn);}
  void Save(TSOut& SOut){
    PredValDs.Save(SOut); CorrVal.Save(SOut); Wgt.Save(SOut);}

  TTsPred& operator=(const TTsPred&){Fail; return *this;}

  PTbValDs GetPredValDs() const {return PredValDs;}
  TTbVal GetCorrVal() const {return CorrVal;}
  double GetWgt() const {return Wgt;}

  friend TPt<TTsPred>;
};
typedef TPt<TTsPred> PTsPred;

/////////////////////////////////////////////////
// Test-Model
typedef enum {tmeAcc, tmePrbAcc, tmeInfScore,
 tmeGeoPrecision, tmeGeoRecall, tmePrecision, tmeRecall,
 tmePrbPrecision, tmePrbRecall, tmeSec, tmeF2, tmeMx} TTsMdEst;

class TTsMd{
private:
  TCRef CRef;
  TBool DefP;
private:
  PTbValDs PriorValDs;
  PTbValDs CorrValDs;
  TVec<PTsPred> PredV;
  TVec<TBoolFltPr> EstActValPrV;
  void AddEst(const TTsMdEst& Est, const double& Val);
  void DefDscEst();
  void DefFltEst();
public:
  TTsMd(const PTbValDs& _PriorValDs):
    DefP(),
    PriorValDs(_PriorValDs), CorrValDs(new TTbValDs(_PriorValDs->GetDscs())),
    PredV(), EstActValPrV(tmeMx){}
  TTsMd(TSIn& SIn):
    DefP(SIn),
    PriorValDs(SIn), CorrValDs(SIn),
    PredV(SIn), EstActValPrV(SIn){}
  static TPt<TTsMd> Load(TSIn& SIn){return new TTsMd(SIn);}
  void Save(TSOut& SOut){
    IAssert(DefP); DefP.Save(SOut);
    PriorValDs.Save(SOut); CorrValDs.Save(SOut);
    PredV.Save(SOut); EstActValPrV.Save(SOut);}

  void Def();
  bool IsDef() const {return DefP;}

  TTsMd& operator=(const TTsMd&){Fail; return *this;}

  void AddPred(const PTsPred& Pred){
    IAssert(!DefP);
    CorrValDs->AddVal(Pred->GetCorrVal(), Pred->GetWgt());
    PredV.Add(Pred);}

  bool IsEstAct(const TTsMdEst& Est) const {
    IAssert(DefP); return EstActValPrV[Est].Val1;}
  double GetEstVal(const TTsMdEst& Est) const {
    IAssert(DefP&&IsEstAct(Est)); return EstActValPrV[Est].Val2;}

  static TStr GetTsMdEstDNm(const TTsMdEst& Est);
  static TPt<TTsMd> GetTsMd(
   const PMd& Md, const PDm& Dm, const int& ClassN, const PExSet& ExSet);

  friend TPt<TTsMd>;
};
typedef TPt<TTsMd> PTsMd;

/////////////////////////////////////////////////
// Test-Model-Generator
class TTsMg{
private:
  TCRef CRef;
  TBool DefP;
public:
  TTsMg(){}
  virtual ~TTsMg(){}
  TTsMg(TSIn& SIn): DefP(SIn){}
  static TPt<TTsMg> Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){
    GetTypeNm(*this).Save(SOut); DefP.Save(SOut);}

  void Def(){Assert(!DefP); DefP=true;}
  bool IsDef() const {return DefP;}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static TPt<TTsMg> New(const PPp& Pp, const PDm& Dm=NULL, const int& _ClassN=-1);
  virtual void Wr()=0;

  friend TPt<TTsMg>;
};
typedef TPt<TTsMg> PTsMg;

/////////////////////////////////////////////////
// Test-Model-Generator-With-Cross-Validation
class TTsMgCV: public TTsMg{
private:
  TVec<PTsMd> TsMdV;
  void GetTsMdV(const int& Folds, const PMg& Mg,
   const PDm& Dm, const int& ClassN);
public:
  TTsMgCV(const int& Folds, const PMg& Mg, const PDm& Dm, const int& ClassN):
    TTsMg(), TsMdV(){GetTsMdV(Folds, Mg, Dm, ClassN);}
  TTsMgCV(const PPp& Pp, const PDm& _Dm=NULL, const int& _ClassN=-1);
  TTsMgCV(TSIn& SIn): TTsMg(SIn), TsMdV(SIn){}
  void Save(TSOut& SOut){TTsMg::Save(SOut); TsMdV.Save(SOut);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  void Wr();
};

/////////////////////////////////////////////////
// Test-Model-Generator-With-Hold-Out
class TTsMgHO: public TTsMg{
private:
  TVec<PTsMd> TsMdV;
  void GetTsMdV(const int& Trials, const double& TrnPrc, const PMg& Mg,
   const PDm& Dm, const int& ClassN);
public:
  TTsMgHO(const int& Trials, const double& TrnPrc, const PMg& Mg,
   const PDm& Dm, const int& ClassN):
    TTsMg(), TsMdV(){GetTsMdV(Trials, TrnPrc, Mg, Dm, ClassN);}
  TTsMgHO(const PPp& Pp, const PDm& _Dm=NULL, const int& _ClassN=-1);
  TTsMgHO(TSIn& SIn): TTsMg(SIn), TsMdV(SIn){}
  void Save(TSOut& SOut){TTsMg::Save(SOut); TsMdV.Save(SOut);}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  void Wr();
};

#endif

