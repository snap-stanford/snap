#ifndef tnt_h
#define tnt_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "mine.h"

class TTntBs;

/////////////////////////////////////////////////
// Tnt-Binary-Document
class TTntBinDoc{
private:
  TStr DocNm;
  TUInt64 Tm;
  TStr SubjStr;
  TStr ContStr;
  TIntStrPrV FqEntNmPrV;
public:
  TTntBinDoc(){}
  TTntBinDoc(const TTntBinDoc& Doc):
    DocNm(Doc.DocNm), Tm(Doc.Tm),
    SubjStr(Doc.SubjStr), ContStr(Doc.ContStr),
    FqEntNmPrV(Doc.FqEntNmPrV){}
  TTntBinDoc(
   const TStr& _DocNm, const TUInt64& _Tm,
   const TStr& _SubjStr, const TStr& _ContStr,
   const TIntStrPrV& _FqEntNmPrV):
    DocNm(_DocNm), Tm(_Tm),
    SubjStr(_SubjStr), ContStr(_ContStr),
    FqEntNmPrV(_FqEntNmPrV){}
  ~TTntBinDoc(){}
  TTntBinDoc(TSIn& SIn):
    DocNm(SIn), Tm(SIn),
    SubjStr(SIn), ContStr(SIn), 
    FqEntNmPrV(SIn){}
  void Save(TSOut& SOut) const {
    DocNm.Save(SOut); Tm.Save(SOut);
    SubjStr.Save(SOut); ContStr.Save(SOut);
    FqEntNmPrV.Save(SOut);}

  TTntBinDoc& operator=(const TTntBinDoc& Doc){
    if (this!=&Doc){
      DocNm=Doc.DocNm; Tm=Doc.Tm;
      SubjStr=Doc.SubjStr; ContStr=Doc.ContStr;
      FqEntNmPrV=Doc.FqEntNmPrV;}
    return *this;}

  // components
  TStr GetDocNm() const {return DocNm;}
  uint64 GetTm() const {return Tm;}
  TStr GetSubjStr() const {return SubjStr;}
  TStr GetContStr() const {return ContStr;}
  const TIntStrPrV& GetFqEntNmPrV() const {return FqEntNmPrV;}
  int GetEnts() const {return FqEntNmPrV.Len();}
  void GetEntNmFq(const int& EntN, TStr& EntNm, int& EntFq) const {
    EntNm=FqEntNmPrV[EntN].Val2; EntFq=FqEntNmPrV[EntN].Val1;}

  static void SaveBinDocV(
   const TStr& InXmlFPath, const TStr& OutBinFNm, const int& MxDocs=-1);
  static void LoadBinDocV(const TStr& InBinFNm);
};

/////////////////////////////////////////////////
// Tnt-Document
ClassTPV(TTntDoc, PTntDoc, TTntDocV)//{
private:
  TStr DocNm;
  TUInt64 Tm;
  TStr SubjStr;
  TStr ContStr;
  TIntPrV EntIdFqPrV;
  UndefCopyAssign(TTntDoc);
public:
  TTntDoc(){}
  static PTntDoc New(){
    return new TTntDoc();}
  static PTntDoc New(
   const TStr& DocNm, const uint64& Tm,
   const TStr& SubjStr, const TStr& ContStr,
   const TIntPrV& EntIdFqPrV){
    PTntDoc Doc=TTntDoc::New();
    Doc->DocNm=DocNm; Doc->Tm=Tm;
    Doc->SubjStr=SubjStr; Doc->ContStr=ContStr;
    Doc->EntIdFqPrV=EntIdFqPrV; return Doc;}
  ~TTntDoc(){}
  TTntDoc(TSIn& SIn):
    DocNm(SIn), Tm(SIn), 
    SubjStr(SIn), ContStr(SIn), 
    EntIdFqPrV(SIn){}
  static PTntDoc Load(TSIn& SIn){return new TTntDoc(SIn);}
  void Save(TSOut& SOut) const {
    DocNm.Save(SOut); Tm.Save(SOut); 
    SubjStr.Save(SOut); ContStr.Save(SOut); 
    EntIdFqPrV.Save(SOut);}

  // components
  TStr GetDocNm() const {return DocNm;}
  uint64 GetTm() const {return Tm;}
  TStr GetSubjStr() const {return SubjStr;}
  TStr GetContStr() const {return ContStr;}
  int GetEnts() const {return EntIdFqPrV.Len();}
  void GetEntNmFq(const int& EntN, int& EntId, int& EntFq) const {
    EntId=EntIdFqPrV[EntN].Val1; EntFq=EntIdFqPrV[EntN].Val2;}
};

typedef TPair<TInt, PTntDoc> TTntIdDocPr;
typedef TVec<TTntIdDocPr> TTntIdDocPrV;

/////////////////////////////////////////////////
// Tnt-Entity-Link-Context
class TTntEntLinkCtx{
public:
  TInt LinkWgt;
  TInt DocId;
  TUInt64 Tm;
public:
  TTntEntLinkCtx(): LinkWgt(), DocId(), Tm(){}
  TTntEntLinkCtx(const TTntEntLinkCtx& EntLinkCtx):
    LinkWgt(EntLinkCtx.LinkWgt), DocId(EntLinkCtx.DocId), Tm(EntLinkCtx.Tm){}
  TTntEntLinkCtx(const int& _LinkWgt, const int& _DocId, const uint64& _Tm):
    LinkWgt(_LinkWgt), DocId(_DocId), Tm(_Tm){}
  ~TTntEntLinkCtx(){}
  TTntEntLinkCtx(TSIn& SIn):
    LinkWgt(SIn), DocId(SIn), Tm(SIn){}
  void Save(TSOut& SOut) const {
    LinkWgt.Save(SOut); DocId.Save(SOut); Tm.Save(SOut);}

  TTntEntLinkCtx& operator=(const TTntEntLinkCtx& EntLinkCtx){
    if (this!=&EntLinkCtx){
      LinkWgt=EntLinkCtx.LinkWgt; DocId=EntLinkCtx.DocId; Tm=EntLinkCtx.Tm;}
    return *this;}

  // components
  int GetLinkWgt() const {return LinkWgt;}
  int GetDocId() const {return DocId;}
  uint64 GetTm() const {return Tm;}
};
typedef TQQueue<TTntEntLinkCtx> TTntEntLinkCtxQ;

/////////////////////////////////////////////////
// Tnt-Entity
ClassTPV(TTntEnt, PTntEnt, TTntEntV)//{
public:
  TIntQ DocIdQ;
  THash<TInt, TTntEntLinkCtxQ> DstEntIdToLinkCtxQH;
public:
  TTntEnt(): DocIdQ(), DstEntIdToLinkCtxQH(){}
  static PTntEnt New(){return new TTntEnt();}
  TTntEnt(const TTntEnt& Ent):
    DocIdQ(Ent.DocIdQ), DstEntIdToLinkCtxQH(Ent.DstEntIdToLinkCtxQH){}
  ~TTntEnt(){}
  TTntEnt(TSIn& SIn):
    DocIdQ(SIn), DstEntIdToLinkCtxQH(SIn){}
  static PTntEnt Load(TSIn& SIn){return new TTntEnt(SIn);}
  void Save(TSOut& SOut) const {
    DocIdQ.Save(SOut); DstEntIdToLinkCtxQH.Save(SOut);}

  TTntEnt& operator=(const TTntEnt& Ent){
    if (this!=&Ent){
      DocIdQ=Ent.DocIdQ; DstEntIdToLinkCtxQH=Ent.DstEntIdToLinkCtxQH;}
    return *this;}

  // doc-id queue
  void PushDocId(const int& DocId){DocIdQ.Push(DocId);}
  void PopDocId(){DocIdQ.Pop();}
  int GetTopDocId() const {return DocIdQ.Top();}
  int GetDocIds() const {return DocIdQ.Len();}
  int GetDocId(const int& DocN) const {return DocIdQ[DocN];}
  void GetDocIdV(const TTntBs* TntBs,
   const uint64& MnTm, const uint64& MxTm, TIntV& DocIdV) const;
  void GetDocsPerDateV(
   const TTntBs* TntBs, TStrIntPrV& DateStrDocsPrV, int& Docs) const;

  void GetDocCentroid(const TTntBs* TntBs,
   const int& TopWords, const double& TopWordsWgtSumPrc,
   TStrFltPrV& WordStrWgtPrV) const;
  void GetDocCentroid(const TTntBs* TntBs,
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs,
   const uint64& MnTm, const TStr& QKwStr, 
   const int& TopWords, const double& TopWordsWgtSumPrc,
   int& Docs, TStrFltPrV& WordStrWgtPrV) const;
  void GetEntClustV(const TTntBs* TntBs,
   const uint64& MnTm, const int& MnDocs, const int& MxDocs, const int& Clusts,
   TVec<TStrFltPrV>& EntNmWgtPrVV) const;

  // entity-links
  void AddLink(const int& DstEntId, const TTntEntLinkCtx& LinkCtx){
    DstEntIdToLinkCtxQH.AddDat(DstEntId).Push(LinkCtx);}
  int GetLinkEnts() const {
    return DstEntIdToLinkCtxQH.Len();}
  int GetLinkEntId(const int& EntIdN) const {
    return DstEntIdToLinkCtxQH.GetKey(EntIdN);}
  int GetEntLinks(const int& EntIdN) const {
    return DstEntIdToLinkCtxQH[EntIdN].Len();}
  const TTntEntLinkCtx& GetEntLinkCtx(const int& EntIdN, const int& LinkN) const {
    return DstEntIdToLinkCtxQH[EntIdN][LinkN];}

  void GetSorted_LinkWgtDstEntIdPrV(
   const uint64& MnTm, const double& TopWgtSumPrc, TIntPrV& LinkWgtDstEntIdPrV) const;
};

/////////////////////////////////////////////////
// Tnt-Entity-ResultSet-TimeSlice
ClassTPV(TTntEntRSet_TmSlice, PTntEntRSet_TmSlice, TTntEntRSet_TmSliceV)//{
public:
  // time interval
  TUInt64 StartTmMSecs; 
  TUInt64 EndTmMSecs;
  // social profile
  TStrIntPrV LinkWgtDstEntNmPrV; // entities
  TStrIntPrV PosDiffLinkWgtDstEntNmPrV; // raising entities
  TStrIntPrV NegDiffLinkWgtDstEntNmPrV; // falling entities
  // content profile
  TStrFltPrV WordStrWgtPrV; // keywords
  TStrFltPrV NegDiffWordStrWgtPrV; // raising keywords
  TStrFltPrV PosDiffWordStrWgtPrV; // falling keywords
public:
  TTntEntRSet_TmSlice(){}
  static PTntEntRSet_TmSlice New(){return new TTntEntRSet_TmSlice();}
  ~TTntEntRSet_TmSlice(){}
  TTntEntRSet_TmSlice(TSIn& SIn):
    StartTmMSecs(SIn), EndTmMSecs(SIn),
    LinkWgtDstEntNmPrV(SIn), 
    PosDiffLinkWgtDstEntNmPrV(SIn), NegDiffLinkWgtDstEntNmPrV(SIn),
    WordStrWgtPrV(SIn), 
    NegDiffWordStrWgtPrV(SIn), PosDiffWordStrWgtPrV(SIn){}
  static PTntEntRSet_TmSlice Load(TSIn& SIn){return new TTntEntRSet_TmSlice(SIn);}
  void Save(TSOut& SOut) const {
    StartTmMSecs.Save(SOut); EndTmMSecs.Save(SOut);
    LinkWgtDstEntNmPrV.Save(SOut); 
    PosDiffLinkWgtDstEntNmPrV.Save(SOut); NegDiffLinkWgtDstEntNmPrV.Save(SOut);
    WordStrWgtPrV.Save(SOut);
    NegDiffWordStrWgtPrV.Save(SOut); PosDiffWordStrWgtPrV.Save(SOut);}

  void PutStartEndTm(const uint64& _StartTmMSecs, const uint64& _EndTmMSecs){
    StartTmMSecs=_StartTmMSecs; EndTmMSecs=_EndTmMSecs;}

  void PutLinkWgtDstEntNmPrV(const TStrIntPrV& _LinkWgtDstEntNmPrV){
    LinkWgtDstEntNmPrV=_LinkWgtDstEntNmPrV;}
  void PutPosDiffLinkWgtDstEntNmPrV(const TStrIntPrV& _PosDiffLinkWgtDstEntNmPrV){
    PosDiffLinkWgtDstEntNmPrV=_PosDiffLinkWgtDstEntNmPrV;}
  void PutNegDiffLinkWgtDstEntNmPrV(const TStrIntPrV& _NegDiffLinkWgtDstEntNmPrV){
    NegDiffLinkWgtDstEntNmPrV=_NegDiffLinkWgtDstEntNmPrV;}

  void PutWordStrWgtPrV(const TStrFltPrV& _WordStrWgtPrV){
    WordStrWgtPrV=_WordStrWgtPrV;}
  void PutNegDiffWordStrWgtPrV(const TStrFltPrV& _NegDiffWordStrWgtPrV){
    NegDiffWordStrWgtPrV=_NegDiffWordStrWgtPrV;}
  void PutPosDiffWordStrWgtPrV(const TStrFltPrV& _PosDiffWordStrWgtPrV){
    PosDiffWordStrWgtPrV=_PosDiffWordStrWgtPrV;}
};

/////////////////////////////////////////////////
// Tnt-Entity-ResultSet
ClassTPV(TTntEntRSet, PTntEntRSet, TTntEntRSetV)//{
private:
  TStr EntNm;
  TTntEntRSet_TmSliceV TmSliceV;
public:
  TTntEntRSet(){}
  static PTntEntRSet New(){return new TTntEntRSet();}
  ~TTntEntRSet(){}
  TTntEntRSet(TSIn& SIn):
    EntNm(SIn), TmSliceV(SIn){}
  static PTntEntRSet Load(TSIn& SIn){return new TTntEntRSet(SIn);}
  void Save(TSOut& SOut) const {
    EntNm.Save(SOut); TmSliceV.Save(SOut);}

  void PutEntNm(const TStr& _EntNm){
    EntNm=_EntNm;}
  void AddTmSlice(const PTntEntRSet_TmSlice& TmSlice){
    TmSliceV.Add(TmSlice);}
};

/////////////////////////////////////////////////
// Tnt-ResultSet
ClassTPV(TTntRSet, PTntRSet, TTntRSetV)//{
private:
  TStr QTmStr, QSchNm, QWcEntNm, QKwStr;
  TTm RawPivotTm, PivotTm;
  TTntEntRSetV EntRSetV;
  TVec<TStrFltPrV> Clust_EntNmWgtPrVV;
public:
  TTntRSet(){}
  static PTntRSet New(){return new TTntRSet();}
  ~TTntRSet(){}
  TTntRSet(TSIn& SIn):
    QTmStr(SIn), QSchNm(SIn), QWcEntNm(SIn), QKwStr(SIn),
    RawPivotTm(SIn), PivotTm(SIn),
    EntRSetV(SIn),
    Clust_EntNmWgtPrVV(SIn){}
  static PTntRSet Load(TSIn& SIn){return new TTntRSet(SIn);}
  void Save(TSOut& SOut) const {
    QTmStr.Save(SOut); QSchNm.Save(SOut); QWcEntNm.Save(SOut); QKwStr.Save(SOut);
    RawPivotTm.Save(SOut); PivotTm.Save(SOut);
    EntRSetV.Save(SOut);
    Clust_EntNmWgtPrVV.Save(SOut);}

  void PutQStrs(const TStr& _QTmStr, const TStr& _QSchNm, const TStr& _QWcEntNm, const TStr& _QKwStr){
    QTmStr=_QTmStr; QSchNm=_QSchNm; QWcEntNm=_QWcEntNm; QKwStr=_QKwStr;}
  void PutPivotTm(const TTm& _RawPivotTm, const TTm& _PivotTm){
    RawPivotTm=_RawPivotTm; PivotTm=_PivotTm;}
  void AddEntRSet(const PTntEntRSet& EntRSet){
    EntRSetV.Add(EntRSet);}
  void PutClust(const TVec<TStrFltPrV>& _Clust_EntNmWgtPrVV){
    Clust_EntNmWgtPrVV=_Clust_EntNmWgtPrVV;}

  // files
  static PTntRSet LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
};

/////////////////////////////////////////////////
// Tnt-Base
ClassTPV(TTntBs, PTntBs, TTntBsV)//{
private:
  THash<TStr, PTntEnt> NmToEntH;
  THash<TInt, PTntDoc> IdToDocH;
  TInt LastDocId;
  TInt MnEntsPerDoc, MnEntFqPerDoc;
  UndefCopyAssign(TTntBs);
public:
  TTntBs():
    NmToEntH(), IdToDocH(),
    LastDocId(0), MnEntsPerDoc(0), MnEntFqPerDoc(0){}
  static PTntBs New(){return new TTntBs();}
  ~TTntBs(){}
  TTntBs(TSIn& SIn):
    NmToEntH(SIn), IdToDocH(SIn),
    LastDocId(SIn), MnEntsPerDoc(SIn), MnEntFqPerDoc(SIn){}
  static PTntBs Load(TSIn& SIn){return new TTntBs(SIn);}
  void Save(TSOut& SOut) const {
    NmToEntH.Save(SOut); IdToDocH.Save(SOut);
    LastDocId.Save(SOut); MnEntsPerDoc.Save(SOut); MnEntFqPerDoc.Save(SOut);}

  // parameters
  int GetNewDocId(){LastDocId++; return LastDocId;}

  void PutMnEntsPerDoc(const int& _MnEntsPerDoc){MnEntsPerDoc=_MnEntsPerDoc;}
  int GetMnEntsPerDoc() const {return MnEntsPerDoc;}

  void PutMnEntFqPerDoc(const int& _MnEntFqPerDoc){MnEntFqPerDoc=_MnEntFqPerDoc;}
  int GetMnEntFqPerDoc() const {return MnEntFqPerDoc;}

  // entities
  int GetEnts() const {return NmToEntH.Len();}
  int AddEnt(const TStr& EntNm){return NmToEntH.AddKey(EntNm);}
  int GetEntId(const TStr& EntNm) const {return NmToEntH.GetKeyId(EntNm);}
  TStr GetEntNm(const int& EntId) const {return NmToEntH.GetKey(EntId);}
  PTntEnt GetEnt(const int& EntId){
    PTntEnt Ent=NmToEntH[EntId]; 
    if (Ent.Empty()){Ent=TTntEnt::New(); NmToEntH[EntId]=Ent;}
    return Ent;}
  PTntEnt GetEnt(const TStr& EntNm){return GetEnt(GetEntId(EntNm));}

  void GetSorted_DocsEntIdPrV(TIntPrV& DocsEntIdPrV);

  // documents
  int GetDocs() const {return IdToDocH.Len();}
  int AddDoc(
   const TStr& DocNm, const uint64& Tm,
   const TStr& SubjStr, const TStr& ContStr,
   const TIntStrPrV FqEntNmPrV);
  PTntDoc GetDoc(const int& DocId) const {
    return IdToDocH.GetDat(DocId);}
  void GetDocIdV(TIntV& DocIdV) const {
    IdToDocH.GetKeyV(DocIdV);}
  void GetIdDocPrV(TTntIdDocPrV& IdDocPrV) const {
    IdToDocH.GetKeyDatPrV(IdDocPrV);}
  void GetMnMxDocTm(uint64& MnDocTm, uint64& MxDocTm) const;

  // bag-of-words
  PBowDocBs GetBowDocBs(const int& MxNGramLen=1, const int& MnNGramFq=1) const;
  PBowDocWgtBs GetBowDocWgtBs(const PBowDocBs& BowDocBs) const;

  // output
  static void GetWordStrWgtPrVDiff(
   const TStrFltPrV& OldWordStrWgtPrV, const TStrFltPrV& NewWordStrWgtPrV,
   TStrFltPrV& NegDiffWordStrWgtPrV, TStrFltPrV& PosDiffWordStrWgtPrV);
  static void GetWordStrWgtPrV(
   const TStrFltPrV& WordStrWgtPrV, TChA& WordStrWgtPrVChA, TChA& WordStrWgtPrVXmlChA);
  static void GetLinkWgtDstEntIdPrVDiff(
   const TIntPrV& OldLinkWgtDstEntIdPrV, const TIntPrV& NewLinkWgtDstEntIdPrV,
   TIntPrV& NegDiffLinkWgtDstEntIdPrV, TIntPrV& PosDiffLinkWgtDstEntIdPrV);
  void GetLinkWgtDstEntNmPrV(const TIntPrV& LinkWgtDstEntIdPrV, 
   TStrIntPrV& LinkWgtDstEntNmPrV, TChA& LinkWgtDstEntIdPrVChA, TChA& LinkWgtDstEntIdPrVXmlChA);

  // files
  static PTntBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}

  // search
  static void GetSchTmV(const TStr& SchNm, const uint64& PivotTm, TUInt64V& SchTmV);
  void Search(
   const PBowDocBs& TntBsBowDocBs, const PBowDocWgtBs& TntBsBowDocWgtBs, 
   const TStr& QTmStr, const TStr& QSchNm, 
   const TStr& QWcEntNm, const TStr& QKwStr,
   PTntRSet& TntRSet, const TStr& OutXmlFNm, const TStr& OutTxtFNm);
};

#endif
