#ifndef skygrid_h
#define skygrid_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "mine.h"

class TSkyGridBs;

/////////////////////////////////////////////////
// SkyGrid-Binary-Document
class TSkyGridBinDoc{
private:
  TStr SwIdStr;
  TUInt64 Tm;
  TStr TitleStr;
  TStr HeadlineStr;
  TIntStrPrV FqEntNmPrV;
public:
  TSkyGridBinDoc(){}
  TSkyGridBinDoc(const TSkyGridBinDoc& Doc):
    SwIdStr(Doc.SwIdStr), Tm(Doc.Tm),
    TitleStr(Doc.TitleStr), HeadlineStr(Doc.HeadlineStr),
    FqEntNmPrV(Doc.FqEntNmPrV){}
  TSkyGridBinDoc(
   const TStr& _SwIdStr, const TUInt64& _Tm,
   const TStr& _TitleStr, const TStr& _HeadlineStr,
   const TIntStrPrV& _FqEntNmPrV):
    SwIdStr(_SwIdStr), Tm(_Tm),
    TitleStr(_TitleStr), HeadlineStr(_HeadlineStr),
    FqEntNmPrV(_FqEntNmPrV){}
  ~TSkyGridBinDoc(){}
  TSkyGridBinDoc(TSIn& SIn):
    SwIdStr(SIn), Tm(SIn),
    TitleStr(SIn), HeadlineStr(SIn), FqEntNmPrV(SIn){}
  void Save(TSOut& SOut) const {
    SwIdStr.Save(SOut); Tm.Save(SOut);
    TitleStr.Save(SOut); HeadlineStr.Save(SOut);
    FqEntNmPrV.Save(SOut);}

  TSkyGridBinDoc& operator=(const TSkyGridBinDoc& Doc){
    if (this!=&Doc){
      SwIdStr=Doc.SwIdStr; Tm=Doc.Tm;
      TitleStr=Doc.TitleStr; HeadlineStr=Doc.HeadlineStr;
      FqEntNmPrV=Doc.FqEntNmPrV;}
    return *this;}

  // components
  TStr GetSwIdStr() const {return SwIdStr;}
  uint64 GetTm() const {return Tm;}
  TStr GetTitleStr() const {return TitleStr;}
  TStr GetHeadlineStr() const {return HeadlineStr;}
  const TIntStrPrV& GetFqEntNmPrV() const {return FqEntNmPrV;}
  int GetEnts() const {return FqEntNmPrV.Len();}
  void GetEntNmFq(const int& EntN, TStr& EntNm, int& EntFq) const {
    EntNm=FqEntNmPrV[EntN].Val2; EntFq=FqEntNmPrV[EntN].Val1;}

  static void SaveBinDocV(
   const TStr& InXmlFPath, const TStr& OutBinFNm, const int& MxDocs=-1);
  static void LoadBinDocV(const TStr& InBinFNm);
};

/////////////////////////////////////////////////
// SkyGrid-Document
ClassTPV(TSkyGridDoc, PSkyGridDoc, TSkyGridDocV)//{
private:
  TStr SwIdStr;
  TUInt64 Tm;
  TStr TitleStr;
  TStr HeadlineStr;
  TIntPrV EntIdFqPrV;
  UndefCopyAssign(TSkyGridDoc);
public:
  TSkyGridDoc(){}
  static PSkyGridDoc New(){
    return new TSkyGridDoc();}
  static PSkyGridDoc New(
   const TStr& SwIdStr, const uint64& Tm,
   const TStr& TitleStr, const TStr& HeadlineStr,
   const TIntPrV& EntIdFqPrV){
    PSkyGridDoc Doc=TSkyGridDoc::New();
    Doc->SwIdStr=SwIdStr; Doc->Tm=Tm;
    Doc->TitleStr=TitleStr; Doc->HeadlineStr=HeadlineStr;
    Doc->EntIdFqPrV=EntIdFqPrV; return Doc;}
  ~TSkyGridDoc(){}
  TSkyGridDoc(TSIn& SIn):
    SwIdStr(SIn), Tm(SIn), TitleStr(SIn), EntIdFqPrV(SIn){}
  static PSkyGridDoc Load(TSIn& SIn){return new TSkyGridDoc(SIn);}
  void Save(TSOut& SOut) const {
    SwIdStr.Save(SOut); Tm.Save(SOut);
    TitleStr.Save(SOut); EntIdFqPrV.Save(SOut);}

  // components
  TStr GetSwIdStr() const {return SwIdStr;}
  uint64 GetTm() const {return Tm;}
  TStr GetTitleStr() const {return TitleStr;}
  TStr GetHeadlineStr() const {return HeadlineStr;}
  int GetEnts() const {return EntIdFqPrV.Len();}
  void GetEntNmFq(const int& EntN, int& EntId, int& EntFq) const {
    EntId=EntIdFqPrV[EntN].Val1; EntFq=EntIdFqPrV[EntN].Val2;}
};

typedef TPair<TInt, PSkyGridDoc> TSkyGridIdDocPr;
typedef TVec<TSkyGridIdDocPr> TSkyGridIdDocPrV;

/////////////////////////////////////////////////
// SkyGrid-Entity-Link-Context
class TSkyGridEntLinkCtx{
public:
  TInt LinkWgt;
  TInt DocId;
  TUInt64 Tm;
public:
  TSkyGridEntLinkCtx(): LinkWgt(), DocId(), Tm(){}
  TSkyGridEntLinkCtx(const TSkyGridEntLinkCtx& EntLinkCtx):
    LinkWgt(EntLinkCtx.LinkWgt), DocId(EntLinkCtx.DocId), Tm(EntLinkCtx.Tm){}
  TSkyGridEntLinkCtx(const int& _LinkWgt, const int& _DocId, const uint64& _Tm):
    LinkWgt(_LinkWgt), DocId(_DocId), Tm(_Tm){}
  ~TSkyGridEntLinkCtx(){}
  TSkyGridEntLinkCtx(TSIn& SIn):
    LinkWgt(SIn), DocId(SIn), Tm(SIn){}
  void Save(TSOut& SOut) const {
    LinkWgt.Save(SOut); DocId.Save(SOut); Tm.Save(SOut);}

  TSkyGridEntLinkCtx& operator=(const TSkyGridEntLinkCtx& EntLinkCtx){
    if (this!=&EntLinkCtx){
      LinkWgt=EntLinkCtx.LinkWgt; DocId=EntLinkCtx.DocId; Tm=EntLinkCtx.Tm;}
    return *this;}

  // components
  int GetLinkWgt() const {return LinkWgt;}
  int GetDocId() const {return DocId;}
  uint64 GetTm() const {return Tm;}
};
typedef TQQueue<TSkyGridEntLinkCtx> TSkyGridEntLinkCtxQ;

/////////////////////////////////////////////////
// SkyGrid-Entity
class TSkyGridEnt{
public:
  TIntQ DocIdQ;
  THash<TInt, TSkyGridEntLinkCtxQ> DstEntIdToLinkCtxQH;
public:
  TSkyGridEnt(): DocIdQ(), DstEntIdToLinkCtxQH(){}
  TSkyGridEnt(const TSkyGridEnt& Ent):
    DocIdQ(Ent.DocIdQ), DstEntIdToLinkCtxQH(Ent.DstEntIdToLinkCtxQH){}
  ~TSkyGridEnt(){}
  TSkyGridEnt(TSIn& SIn):
    DocIdQ(SIn), DstEntIdToLinkCtxQH(SIn){}
  void Save(TSOut& SOut) const {
    DocIdQ.Save(SOut); DstEntIdToLinkCtxQH.Save(SOut);}

  TSkyGridEnt& operator=(const TSkyGridEnt& Ent){
    if (this!=&Ent){
      DocIdQ=Ent.DocIdQ; DstEntIdToLinkCtxQH=Ent.DstEntIdToLinkCtxQH;}
    return *this;}

  // doc-id queue
  void PushDocId(const int& DocId){DocIdQ.Push(DocId);}
  void PopDocId(){DocIdQ.Pop();}
  int GetTopDocId() const {return DocIdQ.Top();}
  int GetDocIds() const {return DocIdQ.Len();}
  int GetDocId(const int& DocN) const {return DocIdQ[DocN];}
  void GetDocIdV(const TSkyGridBs* SkyGridBs,
   const uint64& MnTm, const uint64& MxTm, TIntV& DocIdV) const;
  void GetDocsPerDateV(
   const TSkyGridBs* SkyGridBs, TStrIntPrV& DateStrDocsPrV, int& Docs) const;

  void GetDocCentroid(const TSkyGridBs* SkyGridBs,
   const int& TopWords, const double& TopWordsWgtSumPrc,
   TStrFltPrV& WordStrWgtPrV) const;
  void GetDocCentroid(const TSkyGridBs* SkyGridBs,
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs,
   const uint64& MnTm, const int& TopWords, const double& TopWordsWgtSumPrc,
   int& Docs, TStrFltPrV& WordStrWgtPrV) const;
  void GetEntClustV(const TSkyGridBs* SkyGridBs,
   const uint64& MnTm, const int& MnDocs, const int& MxDocs, const int& Clusts,
   TVec<TStrFltPrV>& EntNmWgtPrVV) const;

  // entity-links
  void AddLink(const int& DstEntId, const TSkyGridEntLinkCtx& LinkCtx){
    DstEntIdToLinkCtxQH.AddDat(DstEntId).Push(LinkCtx);}
  int GetLinkEnts() const {
    return DstEntIdToLinkCtxQH.Len();}
  int GetLinkEntId(const int& EntIdN) const {
    return DstEntIdToLinkCtxQH.GetKey(EntIdN);}
  int GetEntLinks(const int& EntIdN) const {
    return DstEntIdToLinkCtxQH[EntIdN].Len();}
  const TSkyGridEntLinkCtx& GetEntLinkCtx(const int& EntIdN, const int& LinkN) const {
    return DstEntIdToLinkCtxQH[EntIdN][LinkN];}

  void GetSorted_LinkWgtDstEntIdPrV(
   const uint64& MnTm, const double& TopWgtSumPrc, TIntPrV& LinkWgtDstEntIdPrV) const;
};

/////////////////////////////////////////////////
// SkyGrid-Base
ClassTPV(TSkyGridBs, PSkyGridBs, TSkyGridBsV)//{
private:
  TStrH EntNmH;
  THash<TInt, TSkyGridEnt> IdToEntH;
  THash<TInt, PSkyGridDoc> IdToDocH;
  TInt LastDocId;
  TInt MnEntsPerDoc, MnEntFqPerDoc;
  UndefCopyAssign(TSkyGridBs);
public:
  TSkyGridBs():
    EntNmH(), IdToEntH(), IdToDocH(),
    LastDocId(0), MnEntsPerDoc(0), MnEntFqPerDoc(0){}
  static PSkyGridBs New(){return new TSkyGridBs();}
  ~TSkyGridBs(){}
  TSkyGridBs(TSIn& SIn):
    EntNmH(SIn), IdToEntH(SIn), IdToDocH(SIn),
    LastDocId(SIn), MnEntsPerDoc(SIn), MnEntFqPerDoc(SIn){}
  static PSkyGridBs Load(TSIn& SIn){return new TSkyGridBs(SIn);}
  void Save(TSOut& SOut) const {
    EntNmH.Save(SOut); IdToEntH.Save(SOut); IdToDocH.Save(SOut);
    LastDocId.Save(SOut); MnEntsPerDoc.Save(SOut); MnEntFqPerDoc.Save(SOut);}

  // parameters
  int GetNewDocId(){LastDocId++; return LastDocId;}

  void PutMnEntsPerDoc(const int& _MnEntsPerDoc){MnEntsPerDoc=_MnEntsPerDoc;}
  int GetMnEntsPerDoc() const {return MnEntsPerDoc;}

  void PutMnEntFqPerDoc(const int& _MnEntFqPerDoc){MnEntFqPerDoc=_MnEntFqPerDoc;}
  int GetMnEntFqPerDoc() const {return MnEntFqPerDoc;}

  // entities
  int GetEnts() const {return EntNmH.Len();}
  int AddEntNm(const TStr& EntNm){return EntNmH.AddKey(EntNm);}
  TStr GetEntNm(const int& EntId) const {return EntNmH.GetKey(EntId);}
  int GetEntId(const TStr& EntNm) const {return EntNmH.GetKeyId(EntNm);}
  TSkyGridEnt& GetEnt(const int& EntId){
    if (!IdToEntH.IsKey(EntId)){IdToEntH.AddKey(EntId);}
    return IdToEntH.GetDat(EntId);}
  TSkyGridEnt& GetEnt(const TStr& EntNm){return GetEnt(GetEntId(EntNm));}

  void GetSorted_DocsEntIdPrV(TIntPrV& DocsEntIdPrV);

  // documents
  int AddDoc(
   const TStr& SwIdStr, const uint64& Tm,
   const TStr& TitleStr, const TStr& HeadlineStr,
   const TIntStrPrV FqEntNmPrV);
  PSkyGridDoc GetDoc(const int& DocId) const {
    return IdToDocH.GetDat(DocId);}
  void GetIdDocPrV(TSkyGridIdDocPrV& IdDocPrV) const {
    IdToDocH.GetKeyDatPrV(IdDocPrV);}

  PBowDocBs GetBowDocBs(const int& MxNGramLen=1, const int& MnNGramFq=1) const;
  PBowDocWgtBs GetBowDocWgtBs(const PBowDocBs& BowDocBs) const;

  // output
  static void GetWordStrWgtPrVDiff(
   const TStrFltPrV& OldWordStrWgtPrV, const TStrFltPrV& NewWordStrWgtPrV,
   TStrFltPrV& NegDiffWordStrWgtPrV, TStrFltPrV& PosDiffWordStrWgtPrV);
  static void GetWordStrWgtPrVChA(
   const TStrFltPrV& WordStrWgtPrV, TChA& WordStrWgtPrVChA);
  static void TSkyGridBs::GetLinkWgtDstEntIdPrVDiff(
   const TIntPrV& OldLinkWgtDstEntIdPrV, const TIntPrV& NewLinkWgtDstEntIdPrV,
   TIntPrV& NegDiffLinkWgtDstEntIdPrV, TIntPrV& PosDiffLinkWgtDstEntIdPrV);
  void GetLinkWgtDstEntIdPrVChA(
   const TIntPrV& LinkWgtDstEntIdPrV, TChA& LinkWgtDstEntIdPrVChA);

  // files
  static PSkyGridBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm, const uint64& CurTm);
};

#endif
