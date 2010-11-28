#ifndef bix_h
#define bix_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "stopword.h"
#include "tql.h"

/////////////////////////////////////////////////
// Forward
class TBix;
typedef TPt<TBix> PBix;

class TBook;
typedef TPt<TBook> PBook;

/////////////////////////////////////////////////
// Book-Index-Context
class TBixCtx{
private:
  TInt SecId;
  TB32Set FSet;
public:
  TBixCtx(): SecId(-1), FSet(){}
  TBixCtx(const int& _SecId): SecId(_SecId), FSet(){}
  ~TBixCtx(){}
  TBixCtx(TSIn& SIn): SecId(SIn), FSet(SIn){}
  void Save(TSOut& SOut){SecId.Save(SOut); FSet.Save(SOut);}

  TBixCtx& operator=(const TBixCtx& BixCtx){
    SecId=BixCtx.SecId; FSet=BixCtx.FSet; return *this;}
  bool operator==(const TBixCtx& BixCtx) const {
    return SecId==BixCtx.SecId;}
  bool operator<(const TBixCtx& BixCtx) const {
    return SecId<BixCtx.SecId;}

  int GetSecId() const {return SecId;}
  TB32Set GetFSet() const {return FSet;}

  bool IsFlag(const int& FlagN) const {return FSet.In(FlagN);}
  void SetFlag(const int& FlagN){FSet.Incl(FlagN);}
  void PutInt(
   const int& SignFlagN, const int& MnFlagN, const int& MxFlagN, const int& Val){
    if (Val<0){FSet.Incl(SignFlagN);}
    FSet.PutInt(MnFlagN, MxFlagN, abs(Val));}
  int GetInt(
   const int& SignFlagN, const int& MnFlagN, const int& MxFlagN) const {
    int Val=FSet.GetInt(MnFlagN, MxFlagN);
    if (FSet.In(SignFlagN)){Val=-Val;}
    return Val;}

  void Merge(const TBixCtx& Ctx){
    IAssert(SecId==Ctx.SecId); FSet|=Ctx.FSet;}

  TStr GetStr() const;

  static int TitleFlagN;
  static int ContinentFlagN;
  static int AreaFlagN;
};
typedef TVec<TBixCtx> TBixCtxV;

/////////////////////////////////////////////////
// Book-Index-Context-Set
class TBixCtxSet;
typedef TPt<TBixCtxSet> PBixCtxSet;

class TBixCtxSet: public TTqlCtxSet{
private:
  TCRef CRef;
private:
  TBixCtxV BixCtxV;
public:
  TBixCtxSet(): BixCtxV(){}
  TBixCtxSet(const TBixCtxV& _BixCtxV): BixCtxV(_BixCtxV){}
  virtual ~TBixCtxSet(){}
  TBixCtxSet(TSIn&){Fail;}
  static PTqlCtxSet Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TBixCtxSet& operator=(const TBixCtxSet&){Fail; return *this;}

  int GetCtxs() const {return BixCtxV.Len();}
  int GetCtxSecId(const int& CtxN) const {return BixCtxV[CtxN].GetSecId();}

  PTqlCtxSet GetAndCtxSet(const PTqlCtxSet& CtxSet);
  PTqlCtxSet GetOrCtxSet(const PTqlCtxSet& CtxSet);
  PTqlCtxSet GetTagCtxSet(const TStr& TagNm);
  PTqlCtxSet GetTitleCtxSet();

  TStr GetStr() const;

  friend PTqlCtxSet;
};

/////////////////////////////////////////////////
// Book-Index-Results
class TBixRes;
typedef TPt<TBixRes> PBixRes;

class TBixRes{
private:
  TCRef CRef;
private:
  bool Ok;
  TStr MsgStr;
  PBix Bix;
  TStr QueryStr;
  PTqlCtxSet CtxSet;
public:
  TBixRes(const PBix& _Bix, const TStr& _QueryStr, const PTqlCtxSet& _CtxSet);
  TBixRes(const TStr& _ErrMsg):
    Ok(false), MsgStr(_ErrMsg), Bix(), QueryStr(), CtxSet(){}
  ~TBixRes(){}
  TBixRes(TSIn&){Fail;}
  static PBixRes Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TBixRes& operator=(const TBixRes&){Fail; return *this;}

  bool IsOk() const {return Ok;}
  TStr GetMsgStr() const {return MsgStr;}
  PBix GetBix() const {return Bix;}
  TStr GetQueryStr() const {return QueryStr;}
  int GetHits() const {return CtxSet->GetCtxs();}
  void GetHitInfo(
   const PBook& Book,
   const int& HitN, const int& MxTitleLen, const int& MxCtxLen,
   TStr& SecIdStr, TStr& TitleStr, TStr& SecStr, TStr& CtxStr) const;

  void GetHitSetMnMxHitN(
   const int& HitSetN, const int& HitSetSecs, int& MnHitN, int& MxHitN) const;
  void GetHitSet(
   const int& HitSetN, const int& HitSetSecs, const int& TocHitSets,
   const TStr& HitSetUrlFldNm, const PUrlSearchEnv& UrlSearchEnv,
   TStr& PrevTocUrlStr, TStrPrV& TocNmUrlStrPrV, TStr& NextTocUrlStr) const;

  friend TBix;
  friend PBixRes;
};

/////////////////////////////////////////////////
// Book-Index
class TBix;
typedef TPt<TBix> PBix;

class TBix: public TTqlIx{
private:
  TCRef CRef;
private:
  TFAccess Access;
  PHtmlLxChDef ChDef;
  PSwSet SwSet;
  TStrV WordV;
  THash<TStr, TBixCtxV> WordToCtxVH;
  TIntStrIntTrV SecIdKeyNmValTrV;
  void GenWordV(){WordToCtxVH.GetKeyV(WordV); WordV.Sort();}
  void MergeBixCtx(const TStr& WordStr, const TBixCtx& BixCtx);
public:
  TBix(const TSwSetTy& SwSetTy=swstNone):
   TTqlIx(), Access(faCreate),
   ChDef(THtmlLxChDef::GetChDef()), SwSet(TSwSet::GetSwSet(SwSetTy)),
   WordV(), WordToCtxVH(10000), SecIdKeyNmValTrV(){}
  ~TBix(){}
  TBix(TSIn& SIn):
    TTqlIx(SIn), Access(faRdOnly), ChDef(SIn), SwSet(SIn),
    WordV(SIn), WordToCtxVH(SIn), SecIdKeyNmValTrV(SIn){}
  static PBix Load(TSIn& SIn){return new TBix(SIn);}
  void Save(TSOut& SOut){
    TTqlIx::Save(SOut); ChDef.Save(SOut); SwSet.Save(SOut);
    GenWordV(); WordV.Save(SOut);
    WordToCtxVH.Save(SOut); SecIdKeyNmValTrV.Save(SOut);}

  TBix& operator=(const TBix&){Fail; return *this;}

  PTqlCtxSet GetEmptyCtxSet() const;
  PTqlCtxSet GetCtxSet(
   const TStr& WordStr, const bool& IsWc, const int& MxWcWords) const;
  PTqlCtxSet GetCtxSet(
   const TStr& TagNm, const TRelOp& RelOp, const TStr& ArgStr,
   const int& MxWcWords) const;

  void AddSec(const int& SecId, const TStr& SecStr, const bool& TitleP=false);
  void AddKeyVal(const int& SecId, const TStr& KeyNm, const int& KeyVal){
    SecIdKeyNmValTrV.Add(TIntStrIntTr(SecId, KeyNm, KeyVal));}
  PBixRes Search(const TStr& QueryStr);

  static PBix New(const TSwSetTy& SwSetTy=swstNone){
    return PBix(new TBix(SwSetTy));}

  static TStr ContinentTagNm;
  static TStr AreaTagNm;

  friend PBix;
};

#endif
