#ifndef Hc_h
#define Hc_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Html-Coded-Context-Word
class THcWord{
private:
  TB32Set FSet;
  TInt WordId;
public:
  THcWord(): FSet(), WordId(-1){}
  THcWord(const THcWord& HcWord): FSet(HcWord.FSet), WordId(HcWord.WordId){}
  THcWord(const TB32Set& _FSet, const int& _WordId):
    FSet(_FSet), WordId(_WordId){}
  THcWord(const THcWord& HcWord, int _WordId):
    FSet(HcWord.FSet), WordId(_WordId){}
  THcWord(TSIn& SIn): FSet(SIn), WordId(SIn){}
  void Save(TSOut& SOut){FSet.Save(SOut); WordId.Save(SOut);}

  THcWord& operator=(const THcWord& HcWord){
    if (this!=&HcWord){FSet=HcWord.FSet; WordId=HcWord.WordId;} return *this;}
  bool operator==(const THcWord& HcWord) const {
    return (FSet==HcWord.FSet)&&(WordId==HcWord.WordId);}

  bool IsFlag(const int& FN){return FSet.GetBit(FN)==1;}
  void SetFlag(const int& FN, const bool& Val){FSet.SetBit(FN, Val);}
  bool GetFlag(const int& FN){return FSet.GetBit(FN);}
  TInt GetWordId(){return WordId;}
};

/////////////////////////////////////////////////
// Html-Coded-Document
class THcDoc{
private:
  TCRef CRef;
private:
  TVec<THcWord> WordV;
public:
  THcDoc(): WordV(100, 0){}
  THcDoc(const THcDoc& HcDoc): WordV(HcDoc.WordV){Fail;}
  static TPt<THcDoc> Load(TSIn& SIn){return new THcDoc(SIn);}
  THcDoc(TSIn& SIn): WordV(SIn){}
  void Save(TSOut& SOut){WordV.Save(SOut);}

  THcDoc& operator=(const THcDoc& HcDoc){
    if (this!=&HcDoc){WordV=HcDoc.WordV;} return *this;}
  bool operator==(const THcDoc& HcDoc) const {return this==&HcDoc;}

  void Add(const THcWord& HcWord){WordV.Add(HcWord);}
  void Add(const TVec<THcWord>& HcWordV){WordV.AddV(HcWordV);}
  THcWord& Get(const int& WordN){return WordV[WordN];}
  int Len(){return WordV.Len();}

  friend TPt<THcDoc>;
};
typedef TPt<THcDoc> PHcDoc;

/////////////////////////////////////////////////
// Html-Coded-Base
class THcBase{
private:
  TCRef CRef;
private:
  THash<TStr, TIntPr> WordH;
  THash<TStr, PHcDoc> DocH;
  THash<TInt, TVoid> StopStrH, FlagTagH, ContTagH, SkipTagH, BreakTagH;
  TInt StrFN, DStrFN, BTagFN, ETagFN, UrlFN, AnchFN;
  TInt TitleTagId, AnchTagId;
  TInt H1TagId, H2TagId, H3TagId, H4TagId, H5TagId, H6TagId;
  TInt FxWordIds;
  void GenStopStr(const TStr& Str){StopStrH.AddKey(TInt(WordH.AddKey(Str)));}
  void GenFlagTag(const TStr& Str){FlagTagH.AddKey(TInt(WordH.AddKey(Str)));}
  void GenContTag(const TStr& Str){ContTagH.AddKey(TInt(WordH.AddKey(Str)));}
  void GenSkipTag(const TStr& Str){SkipTagH.AddKey(TInt(WordH.AddKey(Str)));}
  void GenBreakTag(const TStr& Str){BreakTagH.AddKey(TInt(WordH.AddKey(Str)));}
  int AddDStrKey(const int& WordId1, const int& WordId2){
    return WordH.AddKey(WordH.GetKey(WordId1)+" "+WordH.GetKey(WordId2));}
  void ClrAnchFlag(const TVec<THcWord>& HcWordV);
public:
  THcBase();
  THcBase(TSIn& SIn);
  static TPt<THcBase> Load(TSIn& SIn){return new THcBase(SIn);}
  void Save(TSOut& SOut);

  int GetWords(){return WordH.Len();}
  int GetMxWordIds(){return WordH.GetMxKeyIds();}
  TStr GetWordStr(const int& WordId){return WordH.GetKey(WordId);}
  int GetWordId(const TStr& WordStr){return WordH.GetKeyId(WordStr);}
  int FFirstWordId(){return WordH.FFirstKeyId();}
  bool FNextWordId(int& WordId){return WordH.FNextKeyId(WordId);}

  void AddDoc(const TStr& FNm, const TStr& _Nm=TStr());
  PHcDoc GetDoc(const TStr& Nm){return DocH.GetDat(Nm);}
  PHcDoc GetDoc(const int& DocId){return DocH[DocId];}
  void DelDoc(const TStr& Nm);
  void DelDocs();

  int GetDocs(){return DocH.Len();}
  int GetMxDocIds(){return DocH.GetMxKeyIds();}
  TStr GetDocNm(const int& DocId){return DocH.GetKey(DocId);}
  int GetDocId(const TStr& Nm){return DocH.GetKeyId(Nm);}
  int FFirstDocId(){return DocH.FFirstKeyId();}
  bool FNextDocId(int& DocId){return DocH.FNextKeyId(DocId);}

  TVec<PHcDoc> GetHLDocV(const int& DocId);

  void Wr();
  void WrDoc(const TStr& UrlStr, const PHcDoc& Doc);

  friend TPt<THcBase>;
};
typedef TPt<THcBase> PHcBase;

#endif

