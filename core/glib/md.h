#ifndef Md_h
#define Md_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "dmdata.h"
#include "mtr.h"

/////////////////////////////////////////////////
// Model
class TMd{
private:
  TCRef CRef;
  TBool DefP;
private:
  PDmHd DmHd;
  TIntV ClassNNToNV;
  TIntV ClassNToNNV;
public:
  TMd(const PDmHd& _DmHd):
    DmHd(_DmHd), ClassNNToNV(1, 0), ClassNToNNV(DmHd->GetClasses()){
    ClassNToNNV.PutAll(TInt(-1));}
  virtual ~TMd(){}
  TMd(TSIn& SIn):
    DefP(SIn), DmHd(SIn), ClassNNToNV(SIn), ClassNToNNV(SIn){}
  static TPt<TMd> Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){Assert(DefP);
    GetTypeNm(*this).Save(SOut); DefP.Save(SOut);
    DmHd.Save(SOut); ClassNNToNV.Save(SOut); ClassNToNNV.Save(SOut);}

  virtual void Def(){Assert(!DefP); DefP=true;}
  bool IsDef() const {return DefP;}

  PDmHd GetDmHd() const {return DmHd;}

  int AddClassN(const int& ClassN){
    IAssert(!DefP); IAssert(ClassNToNNV[ClassN]==-1);
    int ClassNN=ClassNNToNV.Add(ClassN); ClassNToNNV[ClassN]=ClassNN;
    return ClassNN;}
  int GetClasses() const {
    IAssert(DefP); return ClassNNToNV.Len();}
  int GetClassN(const int& ClassNN=0) const {
    IAssert(DefP); return ClassNNToNV[ClassNN];}
  int GetClassNN(const int& ClassN) const {
    IAssert(DefP); return ClassNToNNV[ClassN];}

  virtual PTbValDs GetPriorValDs(const int& ClassN=-1) const=0;
  virtual PTbValDs GetPostrValDs(
   const PValRet& ValRet, const int& ClassN=-1) const=0;

  virtual void Print(const int& ClassN=-1) const=0;

  friend TPt<TMd>;
};
typedef TPt<TMd> PMd;

/////////////////////////////////////////////////
// Model-Prior
class TMdPrior: public TMd{
private:
  TVec<PTbValDs> ValDsV;
public:
  TMdPrior(const PDmHd& DmHd): TMd(DmHd), ValDsV(){}
  TMdPrior(const PDmHd& DmHd, const int& ClassN, const PTbValDs& ValDs):
    TMd(DmHd), ValDsV(){AddClassValDs(ClassN, ValDs); Def();}
  ~TMdPrior(){}
  TMdPrior(TSIn& SIn): TMd(SIn), ValDsV(SIn){}
  void Save(TSOut& SOut){TMd::Save(SOut); ValDsV.Save(SOut);}

  void AddClassValDs(const int& ClassN, const PTbValDs& ValDs){
    IAssert(AddClassN(ClassN)==ValDsV.Add(ValDs));}
  PTbValDs GetPriorValDs(const int& ClassN) const {
    return ValDsV[GetClassNN(ClassN)];}
  PTbValDs GetPostrValDs(const PValRet&, const int& ClassN) const {
    return ValDsV[GetClassNN(ClassN)];}

  void Print(const int& /*ClassN*/) const {}
};

/////////////////////////////////////////////////
// Model-Bayes
class TMdBayes: public TMd{
private:
  TVec<PDmDs> DmDsV;
public:
  TMdBayes(const PDmHd& DmHd): TMd(DmHd), DmDsV(){}
  TMdBayes(const PDmHd& DmHd, const int& ClassN, const PDmDs& DmDs):
    TMd(DmHd), DmDsV(){AddClassDmDs(ClassN, DmDs); Def();}
  ~TMdBayes(){}
  TMdBayes(TSIn& SIn): TMd(SIn), DmDsV(SIn){}
  void Save(TSOut& SOut){TMd::Save(SOut); DmDsV.Save(SOut);}

  void AddClassDmDs(const int& ClassN, const PDmDs& DmDs){
    IAssert(AddClassN(ClassN)==DmDsV.Add(DmDs));}
  PTbValDs GetPriorValDs(const int& ClassN) const {
    return DmDsV[GetClassNN(ClassN)]->GetCDs();}
  PTbValDs GetPostrValDs(const PValRet&, const int& ClassN) const;

  void Print(const int& /*ClassN*/) const {}
};

/////////////////////////////////////////////////
// Model-Nearest-Neighbour
class TMdNNbr: public TMd{
private:
  PDm Dm;
  PExSet ExSet;
  PMtr Mtr;
  TInt Nbrs;
  TVec<PTbValDs> PriorValDsV;
public:
  TMdNNbr(
   const PDm& _Dm, const PExSet& _ExSet, const PMtr& _Mtr, const int& _Nbrs);
  ~TMdNNbr(){}
  TMdNNbr(TSIn& SIn):
    TMd(SIn), Dm(SIn), ExSet(SIn), Mtr(SIn), Nbrs(SIn), PriorValDsV(SIn){}
  void Save(TSOut& SOut){
    TMd::Save(SOut); Dm.Save(SOut); ExSet.Save(SOut);
    Mtr.Save(SOut); Nbrs.Save(SOut); PriorValDsV.Save(SOut);}

  PTbValDs GetPriorValDs(const int& ClassN) const {
    return PriorValDsV[GetClassNN(ClassN)];}
  PTbValDs GetPostrValDs(const PValRet&, const int& ClassN) const;
  void Print(const int& /*ClassN*/) const {}
};

#endif
