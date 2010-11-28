#ifndef TbHc_h
#define TbHc_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "tb.h"
#include "hc.h"

/////////////////////////////////////////////////
// Html-Coded-Table-Variable
class THTbVar: public TTbVar{
private:
  TStr Nm;
  PTbVarType VarType;
public:
  THTbVar(const TStr& _Nm, const PTbVarType& _VarType):
    TTbVar(), Nm(_Nm), VarType(_VarType){}
  THTbVar(TSIn& SIn): TTbVar(SIn), Nm(SIn){}
  void Save(TSOut& SOut){TTbVar::Save(SOut); Nm.Save(SOut);}

  THTbVar& operator=(const THTbVar& HTbVar){
    TTbVar::operator=(HTbVar); Nm=HTbVar.Nm; VarType=HTbVar.VarType; return *this;}

  TStr GetNm() const {return Nm;}
  PTbVarType GetVarType() const {return VarType;}
  TB32Set GetActVTSet() const {return VarType->GetAlwVTSet();}
  void DefVarType(){Fail;}

  TTbVal GetDscVal(const TStr& Str){return VarType->GetDscVal(Str);}
  TTbVal GetDscVal(const int& Dsc){return VarType->GetDscVal(Dsc);}
  TTbVal GetFltVal(const double& Flt){return VarType->GetFltVal(Flt);}
  TTbVal GetVal(const TTbVal& Val){return VarType->GetVal(Val);}
};

/////////////////////////////////////////////////
// Html-Coded-Table
class THTb: public TTb{
protected:
  PHcBase HcBase;
  TIntV VarNToWordIdV, WordIdToVarNV;
  TIntV TupNToDocIdV, DocIdToTupNV;
public:
  THTb(const PHcBase& _HcBase);
  THTb(TSIn& SIn):
    TTb(SIn), HcBase(SIn),
    VarNToWordIdV(SIn), WordIdToVarNV(SIn),
    TupNToDocIdV(SIn), DocIdToTupNV(SIn){}
  void Save(TSOut& SOut){
    TTb::Save(SOut); HcBase.Save(SOut);
    VarNToWordIdV.Save(SOut); WordIdToVarNV.Save(SOut);
    TupNToDocIdV.Save(SOut); DocIdToTupNV.Save(SOut);}

  int AddVar(const PTbVar&){Fail; return -1;}
  void DefVarTypes(){Fail;}
  int GetVars(){return VarNToWordIdV.Len();}
  int GetVarN(const TStr& Nm){return WordIdToVarNV[HcBase->GetWordId(Nm)];}

  int AddTup(const TStr&){Fail; return -1;}
  int GetTups(){return TupNToDocIdV.Len();}
  TStr GetTupNm(const int& TupN){return HcBase->GetDocNm(TupNToDocIdV[TupN]);}
  int GetTupN(const TStr& Nm){return DocIdToTupNV[HcBase->GetWordId/**/(Nm)];}

  void PutDsc(const int&, const int&, const TStr&){Fail;}
  void PutDsc(const int&, const int&, const int&){Fail;}
  void PutFlt(const int&, const int&, const double&){Fail;}
  void PutVal(const int&, const int&, const TTbVal&){Fail;}
  TStr GetValStr(const int& TupN, const int& VarN){
    return GetVal(TupN, VarN).GetStr();}
};

/////////////////////////////////////////////////
// Html-Coded-Binary-Table
class THBTb: public THTb{
private:
  TVec<PBSet> TupV;
public:
  THBTb(const PHcBase& _HcBase);
  THBTb(TSIn& SIn): THTb(SIn), TupV(SIn){SIn.LoadCs();}
  void Save(TSOut& SOut){THTb::Save(SOut); TupV.Save(SOut); SOut.SaveCs();}

  PTbVar GetVar(const int& VarN){
    return new THTbVar(HcBase->GetWordStr/**/(VarN), TTbVarType::GetDscBoolVarType());}

  TTbVal GetVal(const int& TupN, const int& VarN){
    return TTbVal(TupV[TupN]->In(VarN));}
};

/////////////////////////////////////////////////
// Html-Coded-Sequence-Table
class THSTb: public THTb{
private:
  TVec<TIntV> TupV;
public:
  THSTb(const PHcBase& _HcBase);
  THSTb(TSIn& SIn): THTb(SIn), TupV(SIn){SIn.LoadCs();}
  void Save(TSOut& SOut){THTb::Save(SOut); TupV.Save(SOut); SOut.SaveCs();}

  PTbVar GetVar(const int& VarN){
    return new THTbVar(HcBase->GetWordStr/**/(VarN), TTbVarType::GetFltVarType());}

  TTbVal GetVal(const int& TupN, const int& VarN){
    return TTbVal(TupV[TupN].SearchBin(VarNToWordIdV[VarN])!=-1);}
};

/////////////////////////////////////////////////
// Html-Coded-Frequency-Table
class THFTb: public THTb{
private:
  TVec<TVec<TIntPr> > TupV;
public:
  THFTb(const PHcBase& _HcBase);
  THFTb(TSIn& SIn): THTb(SIn), TupV(SIn){SIn.LoadCs();}
  void Save(TSOut& SOut){THTb::Save(SOut); TupV.Save(SOut); SOut.SaveCs();}

  PTbVar GetVar(const int& VarN){
    return new THTbVar(HcBase->GetWordStr/**/(VarN), TTbVarType::GetFltVarType());}
  TTbVal GetVal(const int& TupN, const int& VarN){
    int WordOccN=TupV[TupN].SearchBin(TIntPr(VarNToWordIdV[VarN], TInt(-1)));
    if (WordOccN==-1){return TTbVal((double)0);}
    else {return TTbVal((double)TupV[TupN][WordOccN].Val2);}}
};

#endif

