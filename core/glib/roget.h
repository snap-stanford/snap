#ifndef roget_h
#define roget_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "irsw.h"
#include "web.h"
#include "webmb.h"

/////////////////////////////////////////////////
// Roget-Base-Category
class TRBaseCtg{
private:
  TCRef CRef;
private:
  TStr Nm;
  TBool Bottom;
  TInt Lev;
  TInt ParentId;
  TIntV ChildIdV;
public:
  TRBaseCtg(const TStr& _Nm, const bool& _Bottom):
    Nm(_Nm), Bottom(_Bottom), Lev(-1), ParentId(-1), ChildIdV(){}
  ~TRBaseCtg(){}
  TRBaseCtg(TSIn& SIn):
    Nm(SIn), Bottom(SIn), Lev(SIn), ParentId(SIn), ChildIdV(SIn){}
  static TPt<TRBaseCtg> Load(TSIn& SIn){return new TRBaseCtg(SIn);}
  void Save(TSOut& SOut){
    Nm.Save(SOut); Bottom.Save(SOut); Lev.Save(SOut);
    ParentId.Save(SOut); ChildIdV.Save(SOut);}

  TRBaseCtg& operator=(const TRBaseCtg&){Fail; return *this;}

  TStr GetNm(){return Nm;}
  TBool IsBottom(){return Bottom;}
  void PutLev(const int& _Lev){Lev=_Lev;}
  int GetLev(){return Lev;}
  void PutParent(const int& _ParentId){ParentId=_ParentId;}
  int GetParent(){return ParentId;}
  int GetChilds(){return ChildIdV.Len();}
  int GetChild(const int& ChildIdN){return ChildIdV[ChildIdN];}

  friend TPt<TRBaseCtg>;
};
typedef TPt<TRBaseCtg> PRBaseCtg;

/////////////////////////////////////////////////
// Roget-Base
class TRBase{
private:
  TCRef CRef;
private:
  TVec<PRBaseCtg> CtgV;
  TStrIntVH WordToCtgIdH;
  PNotify Notify;
  void LoadArtfl(const TStr& WebBaseFPath);
public:
  TRBase(const PNotify& _Notify=NULL);
  ~TRBase(){}
  TRBase(TSIn& SIn): CtgV(SIn), WordToCtgIdH(SIn){}
  static TPt<TRBase> Load(TSIn& SIn){return new TRBase(SIn);}
  void Save(TSOut& SOut){CtgV.Save(SOut); WordToCtgIdH.Save(SOut);}

  TRBase& operator=(const TRBase&){Fail; return *this;}

  void SaveTxt(const PSOut& SOut);

  friend TPt<TRBase>;
};
typedef TPt<TRBase> PRBase;

#endif
