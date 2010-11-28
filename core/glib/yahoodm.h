#ifndef YahooDm_h
#define YahooDm_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "dmdata.h"
#include "yahoobs.h"
#include "mg.h"
#include "md.h"

/////////////////////////////////////////////////
// Yahoo-Table-Variable
class TYTbVar: public TTbVar{
private:
  TStr Nm;
  PTbVarType VarType;
public:
  TYTbVar(const TStr& _Nm, const PTbVarType& _VarType):
    TTbVar(), Nm(_Nm), VarType(_VarType){}
  TYTbVar(TSIn& SIn): TTbVar(SIn), Nm(SIn){}
  void Save(TSOut& SOut){TTbVar::Save(SOut); Nm.Save(SOut);}

  TYTbVar& operator=(const TYTbVar& YTbVar){
    TTbVar::operator=(YTbVar); Nm=YTbVar.Nm; VarType=YTbVar.VarType;
    return *this;}

  TStr GetNm() const {return Nm;}
  PTbVarType GetVarType() const {return VarType;}
  TFSet GetActVTSet() const {return VarType->GetAlwVTSet();}
  void DefVarType(){Fail;}

  TTbVal GetDscVal(const TStr& Str){return VarType->GetDscVal(Str);}
  TTbVal GetDscVal(const int& Dsc){return VarType->GetDscVal(Dsc);}
  TTbVal GetFltVal(const double& Flt){return VarType->GetFltVal(Flt);}
  TTbVal GetVal(const TTbVal& Val){return VarType->GetVal(Val);}
};

/////////////////////////////////////////////////
// Yahoo-Table
class TYTb: public TTb{
protected:
  PYBs YBs;
  PYDsBs YDsBs;
  TIntV VarNToWordIdV, WordIdToVarNV;
  TIntV TupNToDocIdV, DocIdToTupNV;
public:
  TYTb(const PYBs& _YBs, const PYDsBs& _YDsBs);
  TYTb(TSIn& SIn):
    TTb(SIn), YBs(SIn), YDsBs(SIn),
    VarNToWordIdV(SIn), WordIdToVarNV(SIn),
    TupNToDocIdV(SIn), DocIdToTupNV(SIn){}
  void Save(TSOut& SOut){
    TTb::Save(SOut); YBs.Save(SOut); YDsBs.Save(SOut);
    VarNToWordIdV.Save(SOut); WordIdToVarNV.Save(SOut);
    TupNToDocIdV.Save(SOut); DocIdToTupNV.Save(SOut);}

  int AddVar(const PTbVar&){Fail; return -1;}
  void DefVarTypes(){Fail;}
  int GetVars(){return VarNToWordIdV.Len();}
  PTbVar GetVar(const int& VarN){
    TStr WordStr=YBs->GetWordStr(VarNToWordIdV[VarN]);
    return new TYTbVar(WordStr, TTbVarType::GetFltVarType());}
  int GetVarN(const TStr& Nm){return WordIdToVarNV[YBs->GetWordId(Nm)];}

  int AddTup(const TStr&){Fail; return -1;}
  int GetTups(){return TupNToDocIdV.Len();}
  TStr GetTupNm(const int& TupN){return YBs->GetDocUrlStr(TupNToDocIdV[TupN]);}
  int GetTupN(const TStr& Nm){return DocIdToTupNV[YBs->GetDocId(Nm)];}

  void PutDsc(const int&, const int&, const TStr&){Fail;}
  void PutDsc(const int&, const int&, const int&){Fail;}
  void PutFlt(const int&, const int&, const double&){Fail;}
  void PutVal(const int&, const int&, const TTbVal&){Fail;}
  TTbVal GetVal(const int& TupN, const int& VarN);
  TStr GetValStr(const int& TupN, const int& VarN){
    return GetVal(TupN, VarN).GetStr();}
};

/////////////////////////////////////////////////
// Yahoo-Domain-Header-Variable
typedef TGDmHdVar TYDmHdVar;

/////////////////////////////////////////////////
// Yahoo-Domain-Header
class TYDmHd: public TDmHd{
private:
  PYBs YBs;
  PYDsBs YDsBs;
public:
  TYDmHd(const PYBs& _YBs, const PYDsBs& _YDsBs):
    TDmHd(), YBs(_YBs), YDsBs(_YDsBs){IAssert(YBs->IsIdEqN());}

  TYDmHd& operator=(const TYDmHd&){Fail; return *this;}

  int GetClasses() const {return YBs->GetDocs();}
  int GetClassN(const TStr& Nm) const {return YBs->GetDocId(Nm);}
  TStr GetClassNm(const int& ClassN) const {return YBs->GetDocUrlStr(ClassN);}
  PDmHdVar GetClass(const int& ClassN) const {
    TStr ClassNm=YBs->GetDocUrlStr(ClassN);
    return new TYDmHdVar(ClassNm, TTbVarType::GetDscBoolVarType());}

  int GetAttrs() const {return YBs->GetWords();}
  int GetAttrN(const TStr& Nm) const {return YBs->GetWordId(Nm);}
  TStr GetAttrNm(const int& AttrN) const {return YBs->GetWordStr(AttrN);}
  PDmHdVar GetAttr(const int& AttrN) const {
    TStr AttrNm=YBs->GetWordStr(AttrN);
    return new TYDmHdVar(AttrNm, TTbVarType::GetFltVarType());}
};

/////////////////////////////////////////////////
// Yahoo-Domain-Distribution
typedef enum {yndtEmpty, yndtRoot, yndtAll} TYNegDsType;
typedef enum {yptWords, yptSects, yptDocs} TYPriorType;

class TYDmDs: public TDmDs{
private:
  bool DoPriorDmDs;
  int YNegDsType;
  int YPriorType;
  PYBs YBs;
  PYDsBs YDsBs;
  PDmHd DmHd;
  PYWordDs NegWordDs;
  PYWordDs PosWordDs;
  PTbValDs CValDs;
  double NegCValPrb;
  double PosCValPrb;
public:
  TYDmDs(
   const bool& _DoPriorDmDs, const int& ClassN,
   const int& _YNegDsType, const int& _YPriorType,
   const PYBs& _YBs, const PYDsBs& _YDsBs, const PDmHd& _DmHd);
  TYDmDs(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  TYDmDs& operator=(const TYDmDs&){Fail; return *this;}

  double GetSumW();
  PTbValDs GetCDs();
  PTbValDs GetAVDs(const int& AttrN);
  PTbValDs GetCAVDs(const TTbVal& CVal, const int& AttrN);

  static PYWordDs GetNegWordDs(
   const int& YNegDsType, const PYBs& YBs, const PYDsBs& YDsBs);
  static PTbValDs GetPriorValDs(const int& YPriorType,
   const PYWordDs& NegWordDs, const PYWordDs& PosWordDs);

  static TStr GetYNegDsTypeStr(const TYNegDsType& YNegDsType);
  static TStr GetYPriorTypeStr(const TYPriorType& YPriorType);
};

/////////////////////////////////////////////////
// Yahoo-Feature-Selection
typedef enum {yfstFix, yfstPosPrc, yfstUnionPrc} TYFSelType;

class TYFSelBs{
private:
  TCRef CRef;
private:
  TInt YNegDsType;
  TVec<TIntFltKdV> DocIdToWordIdEstVV;
public:
  TYFSelBs(
   const TYFSelType& FSelType, const double& FSels,
   const bool& FSelPosWords, const PAttrEst& AttrEst,
   const TYNegDsType& YNegDsType, const TYPriorType& YPriorType,
   const PYBs& YBs, const PYDsBs& YDsBs, const PNotify& Notify=NULL);
  ~TYFSelBs(){}
  TYFSelBs(TSIn& SIn):
    YNegDsType(SIn), DocIdToWordIdEstVV(SIn){SIn.LoadCs();}
  static TPt<TYFSelBs> Load(TSIn& SIn){return new TYFSelBs(SIn);}
  void Save(TSOut& SOut){
    YNegDsType.Save(SOut); DocIdToWordIdEstVV.Save(SOut); SOut.SaveCs();}

  TYFSelBs& operator=(const TYFSelBs&){Fail; return *this;}

  int GetWords(const int& DocId){
    return DocIdToWordIdEstVV[DocId].Len();}
  TIntFltKdV& GetWordIdEstV(const int& DocId){
    return DocIdToWordIdEstVV[DocId];}
  void GetBestWordIdV(
   const int& DocId, const double& EstExp, const double& SumEstPrb,
   const PYWordDs& IntrsWordDs, TIntV& BestWordIdV);
  bool IsWordId(const TIntFltKdV& WordIdEstV, const int& WordId){
    return WordIdEstV.SearchBin(TIntFltKd(WordId))!=-1;}

  void SaveTxt(
   const PSOut& SOut, const PYBs& YBs, const PYDsBs& YDsBs);

  static TStr GetYFSelTypeStr(const TYFSelType& YFSelType);

  friend TPt<TYFSelBs>;
};
typedef TPt<TYFSelBs> PYFSelBs;

/////////////////////////////////////////////////
// Yahoo-Inverted-Index
class TYInvIx{
private:
  TCRef CRef;
private:
  TIntIntH WordIdToFirstDocIdNH;
  TIntV DocIdVHeap;
  TIntV AllDocIdV;
public:
  TYInvIx(
   const double& EstExp, const double& SumEstPrb,
   const PYBs& YBs, const PYDsBs& YDsBs,
   const PYFSelBs& YFSelBs, const PNotify& Notify);
  TYInvIx(TSIn& SIn):
    WordIdToFirstDocIdNH(SIn), DocIdVHeap(SIn),
    AllDocIdV(SIn){SIn.LoadCs();}
  static TPt<TYInvIx> Load(TSIn& SIn){return new TYInvIx(SIn);}
  void Save(TSOut& SOut){
    WordIdToFirstDocIdNH.Save(SOut); DocIdVHeap.Save(SOut);
    AllDocIdV.Save(SOut); SOut.SaveCs();}

  TYInvIx& operator=(const TYInvIx&){Fail; return *this;}

  void GetDocIdV(const PYWordDs& WordDs, const int& MnDocFq, TIntV& DocIdV);

  int FFirstDocId(const int& WordId){
    return WordIdToFirstDocIdNH.GetDat(WordId);}
  bool FNextWordId(int& DocIdN, int& DocId){
    if (DocIdN==-1){return false;}
    else {DocId=DocIdVHeap[DocIdN++]; return DocId!=-1;}}

  void SaveTxt(const PSOut& SOut, const PYBs& YBs);

  friend TPt<TYInvIx>;
};
typedef TPt<TYInvIx> PYInvIx;

/////////////////////////////////////////////////
// Yahoo-Value-Retriever
class TYValRet: public TValRet{
private:
  double MnWordPrb;
  PYBs YBs;
  PYWordDs WordDs;
  TIntV WordIdToAttrNV;
public:
  TYValRet(const double& _MnWordPrb, const PDmHd& _DmHd,
   const PYBs& _YBs, const PYWordDs& _WordDs):
    TValRet(_DmHd), MnWordPrb(_MnWordPrb), YBs(_YBs), WordDs(_WordDs){}

  TTbVal GetVal(const int& AttrN) const {
    return TTbVal(WordDs->IsWordId(AttrN));}

  int FFirstAttrN() const {return WordDs->FFirstWordId();}
  bool FNextAttrN(int& AttrP, int& AttrN, TTbVal& AttrVal) const;
};

/////////////////////////////////////////////////
// Model-Yahoo-Bayes
class TMdYBayes: public TMd{
private:
  TInt YNegDsType;
  TInt YPriorType;
  PYBs YBs;
  PYDsBs YDsBs;
  PYFSelBs YFSelBs;
  PYInvIx YInvIx;
  PYWordDs NegWordDs;
public:
  TMdYBayes(
   const TYNegDsType& _YNegDsType, const TYPriorType& _YPriorType,
   const PYBs& _YBs, const PYDsBs& _YDsBs,
   const PYFSelBs& _YFSelBs, const PYInvIx& _YInvIx);
  static PMd Load(TSIn& SIn);
  void Save(TSOut& SOut);

  PTbValDs GetPriorValDs(const int& ClassN) const {
    PYWordDs PosWordDs=YDsBs->GetWordDs(ClassN);
    return TYDmDs::GetPriorValDs(YPriorType, NegWordDs, PosWordDs);}
  PTbValDs GetPostrValDs(const PValRet& ValRet, const int& ClassN) const;
  PTbValDs GetPostrValDs(const PValRet& ValRet, const int& ClassN,
   TFltIntKdV& WordPrbIdV) const;

  PYBs GetYBs() const {return YBs;}
  PYDsBs GetYDsBs() const {return YDsBs;}
  PYFSelBs GetYFSelBs() const {return YFSelBs;}
  PYInvIx GetYInvIx() const {return YInvIx;}

  void Print(const int&) const {}
};

#endif
