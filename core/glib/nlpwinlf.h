#ifndef nlpwinlf_h
#define nlpwinlf_h

/////////////////////////////////////////////////
// Includes
#include "tmine.h"

/////////////////////////////////////////////////
// Logical-Form-Substitutions
ClassTPV(TLfSubst, PLfSubst, TLfSubstV)//{
public:
  TStr SubstId;
  TStrV TargetStrV;
  TStrV SelBitNmV;
  TStr SubstStr;
public:
  TLfSubst(const TStr& _SubstId): SubstId(_SubstId){}
  static PLfSubst New(const TStr& SubstId){return new TLfSubst(SubstId);}
  TLfSubst(const TLfSubst& LfSubst){Fail;}
  TLfSubst(TSIn& SIn){Fail;}
  static PLfSubst Load(TSIn& SIn){return new TLfSubst(SIn);}
  void Save(TSOut& SOut){Fail;}

  TLfSubst& operator=(const TLfSubst&){Fail; return *this;}

  void AddTarget(const TStr& TargetStr){TargetStrV.AddSorted(TargetStr.GetUc());}
  void AddSelBit(const TStr& SelBitNm){SelBitNmV.AddSorted(SelBitNm);}

  static void GetLfSubstV(TLfSubstV& LfSubstV);

  static void Dump(const bool& DumpP, const TLfSubstV& LfSubstV);
};

/////////////////////////////////////////////////
// Logical-Form-Alias
class TLfAlias{
private:
  TStr SubstId;
  int Fq;
public:
  TLfAlias(): SubstId(), Fq(-1){}
  TLfAlias(const TStr& _SubstId): SubstId(_SubstId), Fq(0){}
  TLfAlias(const TLfAlias& LfAlias): SubstId(LfAlias.SubstId), Fq(LfAlias.Fq){}
  TLfAlias(TSIn& SIn){Fail;}
  void Save(TSOut& SOut){Fail;}

  TLfAlias& operator=(const TLfAlias& LfAlias){
    SubstId=LfAlias.SubstId; Fq=LfAlias.Fq; return *this;}

  TStr GetSubstId() const {return SubstId;}
  void IncFq(const int& IncVal=1){Fq+=IncVal;}
  int GetFq() const {return Fq;}
};

/////////////////////////////////////////////////
// Logical-Form-Alias-Base
ClassTPV(TLfAliasBs, PLfAliasBs, TLfAliasBsV)//{
public:
  THash<TStrV, TLfAlias> WordStrVToAliasH;
  TStrVStrH WordStrVToAliasStrH;
  PSwSet SwSet;
public:
  TLfAliasBs():
    WordStrVToAliasH(), WordStrVToAliasStrH(),
    SwSet(TSwSet::New(swstEnglish523)){}
  static PLfAliasBs New(){return new TLfAliasBs();}
  TLfAliasBs(const TLfAliasBs& LfAliasBs):
    WordStrVToAliasH(LfAliasBs.WordStrVToAliasH),
    WordStrVToAliasStrH(LfAliasBs.WordStrVToAliasStrH),
    SwSet(LfAliasBs.SwSet){}
  static PLfAliasBs New(const PLfAliasBs& LfAliasBs){
    return new TLfAliasBs(*LfAliasBs);}
  TLfAliasBs(TSIn& SIn){Fail;}
  static PLfAliasBs Load(TSIn& SIn){return new TLfAliasBs(SIn);}
  void Save(TSOut& SOut){Fail;}

  TLfAliasBs& operator=(const TLfAliasBs&){Fail; return *this;}

  bool IsGenericWord(const TStr& WordStr) const;
  void PutAliasStr(const TStr& SrcStr, const TStr& SubstId, TStr& DstStr);
  void UpdateLfSubstV(const TStr& AliasBsStr, const TLfSubstV& LfSubstV) const;
  void AddAlias(const TStrV& WordStrV, const TStr& SubstId, const TStr& AliasStr);

  void MergeAliasBs(const PLfAliasBs& LfAliasBs);

  void Dump(const bool& DumpP=false, const int& MnAliasFq=0) const;

};

/////////////////////////////////////////////////
// Logical-Subject-Predicate-Object
ClassTPV(TLfSpo, PLfSpo, TLfSpoV)//{
private:
  TStr LSubjStr;
  TStr SubjStr;
  TStr PredStr;
  TStr ObjStr;
  TStr LObjStr;
  static TStrH UnusableWordStrH;
public:
  TLfSpo(): LSubjStr(), SubjStr(), PredStr(), ObjStr(), LObjStr(){}
  TLfSpo(const TStr& _LSubjStr, const TStr& _SubjStr,
   const TStr& _PredStr, const TStr& _ObjStr, const TStr& _LObjStr):
    LSubjStr(_LSubjStr), SubjStr(_SubjStr),
    PredStr(_PredStr), ObjStr(_ObjStr), LObjStr(_LObjStr){}
  TLfSpo(const TLfSpo&);
  static PLfSpo New(const TStr& LSubjStr, const TStr& SubjStr,
   const TStr& PredStr, const TStr& ObjStr, const TStr& LObjStr){
    return new TLfSpo(LSubjStr, SubjStr, PredStr, ObjStr, LObjStr);}
  TLfSpo(TSIn& SIn):
    LSubjStr(SIn), SubjStr(SIn), PredStr(SIn), ObjStr(SIn), LObjStr(SIn){}
  static PLfSpo Load(TSIn& SIn){return new TLfSpo(SIn);}
  void Save(TSOut& SOut){
    LSubjStr.Save(SOut); SubjStr.Save(SOut);
    PredStr.Save(SOut); ObjStr.Save(SOut); LObjStr.Save(SOut);}

  TLfSpo& operator=(const TLfSpo&){Fail; return *this;}

  // retrieve components
  TStr GetLSubjStr() const {return LSubjStr;}
  TStr GetSubjStr() const {return SubjStr;}
  TStr GetPredStr() const {return PredStr;}
  TStr GetObjStr() const {return ObjStr;}
  TStr GetLObjStr() const {return LObjStr;}

  bool IsUsable() const;
  TStr GetSent() const;
  TStr GetLSent() const;
};

/////////////////////////////////////////////////
// Logical-Form-Tree
ClassTPV(TLfTree, PLfTree, TLfTreeV)//{
private:
public:
  TStr SentStr;
  TStr ArSentStr;
  TStr LfTypeNm;
  TInt Lev;
  TStr HeadStr, HeadTypeNm, HeadSubstStr;
  TStrV BitNmV;
  TLfTreeV SubLfTreeV;
public:
  TLfTree():
    SentStr(), ArSentStr(),
    LfTypeNm(), Lev(-1),
    HeadStr(), HeadTypeNm(), HeadSubstStr(),
    BitNmV(), SubLfTreeV(){}
  static PLfTree New(){return new TLfTree();}
  TLfTree(const TLfTree& LfTree){Fail;}
  TLfTree(TSIn& SIn):
    SentStr(SIn), ArSentStr(SIn),
    LfTypeNm(SIn), Lev(SIn),
    HeadStr(SIn), HeadTypeNm(SIn), HeadSubstStr(SIn),
    BitNmV(SIn), SubLfTreeV(SIn){}
  static PLfTree Load(TSIn& SIn){return new TLfTree(SIn);}
  void Save(TSOut& SOut){
    SentStr.Save(SOut); ArSentStr.Save(SOut);
    LfTypeNm.Save(SOut); Lev.Save(SOut);
    HeadStr.Save(SOut); HeadTypeNm.Save(SOut); HeadSubstStr.Save(SOut);
    BitNmV.Save(SOut); SubLfTreeV.Save(SOut);}

  TLfTree& operator=(const TLfTree&){Fail; return *this;}

  // retrieve components
  TStr GetSentStr() const {return SentStr;}
  TStr GetArSentStr() const {return ArSentStr;}

  // subject-predicate-object
  bool IsSubLfTree(const TStr& SelLfTypeNm, const TStr& SelHeadTypeNm,
   TStr& SubHeadStr, TStr& SubSentStr, PLfTree& SubLfTree) const;
  bool IsSubLfTree(const TStr& SelLfTypeNm, const TStr& SelHeadTypeNm,
   TStr& SubHeadStr, TStr& SubSentStr) const;
  void GetLfSpoV(
   TLfSpoV& LfSpoV, const int& MxLev,
   const TStr& UpSubjStr, const TStr& UpLSubjStr) const;
  void GetLfSpoV(TLfSpoV& LfSpoV, const int& MxLev=-1) const;

  // anaphora resolution
  void PutSubst(
   const TLfSubstV& LfSubstV, const PLfAliasBs& LfAliasBs,
   TStrPrV& HeadStrSubstStrPrV, const bool& DumpP=false);
  void ClrSubst();
  TStr GetHeadOrSubstStr() const {
    return HeadSubstStr.Empty() ? HeadStr : HeadSubstStr;}

  static PLfTree GetLfTree(const PXmlTok& TreeTok, const int& Lev=0);
  void Dump();
};

/////////////////////////////////////////////////
// Logical-Form-Document
ClassTPV(TLfDoc, PLfDoc, TLfDocV)//{
public:
  TStr DocId;
  TStrPrV SentStrArSentStrPrV;
  TLfTreeV SentLfTreeV;
public:
  TLfDoc(const TStr& _DocId):
    DocId(_DocId), SentStrArSentStrPrV(), SentLfTreeV(){}
  static PLfDoc New(const TStr& DocId){return new TLfDoc(DocId);}
  TLfDoc(const TLfDoc& LfDoc){Fail;}
  TLfDoc(TSIn& SIn):
    DocId(SIn), SentStrArSentStrPrV(SIn), SentLfTreeV(SIn){}
  static PLfDoc Load(TSIn& SIn){return new TLfDoc(SIn);}
  void Save(TSOut& SOut){
    DocId.Save(SOut); SentStrArSentStrPrV.Save(SOut); SentLfTreeV.Save(SOut);}

  TLfDoc& operator=(const TLfDoc&){Fail; return *this;}

  TStr GetDocId() const {return DocId;}

  void AddSent(const TStr& SentStr, const PLfTree& LfTree){
    if (!LfTree.Empty()){
      SentStrArSentStrPrV.Add(TStrPr(SentStr, "")); SentLfTreeV.Add(LfTree);}}
  int GetSents() const {
    return SentLfTreeV.Len();}
  TStr GetSentStr(const int& SentN) const {
    return SentStrArSentStrPrV[SentN].Val1;}
  TStr GetArSentStr(const int& SentN) const {
    return SentStrArSentStrPrV[SentN].Val2;}
  PLfTree GetSentLfTree(const int& SentN) const {
    return SentLfTreeV[SentN];}

  static TStr GenArSentStr(
   const TStr& SentStr, const TStrPrV& HeadStrSubstStrPrV, const bool& PhraseP);
  void GenHeadsSubst(const PLfAliasBs& CtxLfAliasBs=NULL, const bool& DumpP=false);
  void ClrHeadsSubst();

  void Dump(const int& DumpSentN=-1) const;
};

/////////////////////////////////////////////////
// Logical-Form-Document-Base
ClassTPV(TLfDocBs, PLfDocBs, TLfDocBsV)//{
public:
  TLfDocV LfDocV;
public:
  TLfDocBs(): LfDocV(){}
  static PLfDocBs New(){return new TLfDocBs();}
  TLfDocBs(const TLfDocBs& LfDocBs){Fail;}
  TLfDocBs(TSIn& SIn): LfDocV(SIn){}
  static PLfDocBs Load(TSIn& SIn){return new TLfDocBs(SIn);}
  void Save(TSOut& SOut){LfDocV.Save(SOut);}

  TLfDocBs& operator=(const TLfDocBs&){Fail; return *this;}

  void AddLfDocBs(const PLfDocBs& LfDocBs){LfDocV.AddV(LfDocBs->LfDocV);}
  void AddLfDoc(const PLfDoc& LfDoc){LfDocV.Add(LfDoc);}
  int GetLfDocs() const {return LfDocV.Len();}
  PLfDoc GetLfDoc(const int& LfDocN) const {return LfDocV[LfDocN];}
  PLfDoc GetLfDoc(const TStr& DocId) const;
  bool IsLfDoc(const TStr& DocId, PLfDoc& LfDoc) const {
    LfDoc=GetLfDoc(DocId); return !LfDoc.Empty();}

  void GenHeadsSubst(const bool& DumpP=false);
  void GenHeadsSubst(const TStrV& DocIdV, const bool& DumpP=false);

  void GetCompClV(const PXmlTok& XmlTok, TStrV& CompClStrV) const;

  static PLfDocBs LoadXml(const TStr& FNm, const bool& DumpP=false);
  static PLfDocBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  static PLfDocBs LoadBinSet(const TStr& FPath, const TStr& FExt=".ldb");
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  static void ConvXmlToLdb(const TStr& FNm, const bool& DumpP=false);
  static void ConvSetXmlToLdb(const TStr& FPath, const bool& DumpP=false);


  void Dump() const;
};

#endif
