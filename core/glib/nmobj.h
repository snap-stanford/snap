#ifndef nmobj_h
#define nmobj_h

/////////////////////////////////////////////////
// Includes
#include "mine.h"
#include "mte.h"

/////////////////////////////////////////////////
// Named-Objects-Documents
ClassTP(TNmObjDoc, PNmObjDoc)//{
public:
  TStr DateStr; // date-time
  TIntPrV NmObjIdFqPrV; // named-object/frequency pair vector
public:
  TNmObjDoc(){}
  static PNmObjDoc New(){
    return new TNmObjDoc();}
  TNmObjDoc(TSIn& SIn):
    DateStr(SIn), NmObjIdFqPrV(SIn){}
  static PNmObjDoc Load(TSIn& SIn){return new TNmObjDoc(SIn);}
  void Save(TSOut& SOut){
    DateStr.Save(SOut); NmObjIdFqPrV.Save(SOut);}

  TNmObjDoc& operator=(const TNmObjDoc&){Fail; return *this;}
};

/////////////////////////////////////////////////
// Named-Objects-Base
typedef enum {
 noaDefined, noaIgnore, noaStandalone, noaAsCapitalized, noaUnperiod,
 noaAcronym, noaFirstName, noaPerson, noaCompany, noaOrganization,
 noaCountry, noaGeography} TNmObjAttr;

ClassTP(TNmObjBs, PNmObjBs)//{
private:
  PHtmlLxChDef ChDef; // character set
  PSwSet SwSet; // stop-words
  TStrStrH WordStrToNrH; // for lemmatization
  THash<TStrV, TB32Set> WordStrVToNmObjAttrSetH; // attribute-set
  TStrVStrVH NmObjWordStrVToNrH; // named-object aliases
  TStrVIntVH NmObjWordStrVToDocIdVH; // named-objects with doc-id-vector
  THash<TStr, PNmObjDoc> DocNmToNmObjDocH; // named-object documents
  PMteLex MteLex; // MulTextEast
public:
  TNmObjBs(
   const TSwSetType& SwSetType, const PSIn& CustSwSetSIn,
   const PSIn& NrWordBsSIn, const PSIn& WordTypeBsSIn, const TStr& MteFNm);
  static PNmObjBs New(
   const TSwSetType& SwSetType, const PSIn& CustSwSetSIn,
   const PSIn& NrWordBsSIn, const PSIn& WordTypeBsSIn, const TStr& MteFNm){
    return new TNmObjBs(
     SwSetType, CustSwSetSIn, NrWordBsSIn, WordTypeBsSIn, MteFNm);}
  TNmObjBs(TSIn& SIn):
    ChDef(SIn), SwSet(SIn),
    WordStrToNrH(SIn), WordStrVToNmObjAttrSetH(SIn),
    NmObjWordStrVToNrH(SIn), NmObjWordStrVToDocIdVH(SIn),
    DocNmToNmObjDocH(SIn), MteLex(SIn){}
  static PNmObjBs Load(TSIn& SIn){return new TNmObjBs(SIn);}
  void Save(TSOut& SOut){
    ChDef.Save(SOut); SwSet.Save(SOut);
    WordStrToNrH.Save(SOut); WordStrVToNmObjAttrSetH.Save(SOut);
    NmObjWordStrVToNrH.Save(SOut); NmObjWordStrVToDocIdVH.Save(SOut);
    DocNmToNmObjDocH.Save(SOut); MteLex.Save(SOut);}

  TNmObjBs& operator=(const TNmObjBs&){Fail; return *this;}

  // meta-tags in candidate list
  static TStr PeriodTagStr;
  static TStr BreakTagStr;
  static TStr ParagraphTagStr;
  static TStr EofTagStr;

  // custom stop-words
  void LoadCustSwSet(const PSIn& SIn);

  // word normalization
  void LoadNrWordBs(const PSIn& SIn);
  TStr GetNrWordStr(const TStr& WordStr) const;

  // named-object-type
  static TNmObjAttr GetNmObjTypeFromStr(const TStr& Str);
  void LoadNmObjTypeBs(const PSIn& SIn);
  bool IsNmObjAttr(const TStr& WordStr, const TNmObjAttr& NmObjAttr) const;
  bool IsNmObjAttr(const TStrV& WordStrV, const TNmObjAttr& NmObjAttr) const;

  // named-object & documents creation
  bool IsFirstCapWordStr(const TStr& Str) const;
  bool IsAllCapWordStr(const TStr& Str) const;
  bool IsNumStr(const TStr& Str) const;
  bool IsTagStr(const TStr& Str) const;
  bool IsMatchPfx(const TStr& Str1, const TStr& Str2,
   const int& MnPfxLen, const int& MxSfxLen) const;
  int GetNmObjId(const TStrV& WordStrV, const bool& DefP=false);
  TStr GetWordStrVStr(const TStrV& WordStrV, const char& SepCh='_') const;
  void GetNrNmObjStrV(const TStrV& NmObjStrV, TStrV& NrNmObjStrV) const;
  void ExtrCandWordStrV(
   const TStr& HtmlStr, TStrV& CandWordStrV, const bool& DumpP);
  void FilterCandToNmObjIdV(
   const TStrV& CandWordStrV, TIntV& NmObjIdV, const bool& DumpP);
  void GetMergedNmObj(TIntV& NewNmObjIdV);
  void PutMergedNmObj(const TIntV& NewNmObjIdV);

  // named-objects retrieval
  int GetNmObjs() const {return NmObjWordStrVToDocIdVH.Len();}
  void GetNmObjStrFqPrV(TStrIntPrV& NmObjStrFqPrV, const int& MnFq=0) const;
  void GetNmObjFqStrPrV(TIntStrPrV& NmObjFqStrPrV, const int& MnFq=0) const;
  void GetNmObjDIdV(const PBowDocBs& BowDocBs, TIntV& BowDIdV, 
   const TStr& NmObjStr1, const TStr& NmObjStr2=TStr()) const;
  PBowSpV GetNmObjConcept(
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs,
   const TStr& NmObjStr1, const TStr& NmObjStr2=TStr()) const;
  void GetFqNmObjIdPrV(
   const TStr& TargetNmObjStr, TIntPrV& FqNmObjIdPrV) const;
  void GetNmObjWordStrV(const int& NmObjId, TStrV& WordStrV) const {
    WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);}
  TStr GetNmObjStr(const int& NmObjId) const {
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    return GetWordStrVStr(WordStrV);}
  int GetNmObjDocs(const int& NmObjId) const {
    return NmObjWordStrVToDocIdVH[NmObjId].Len();}
  int GetNmObjDocId(const int& NmObjId, const int& DocIdN) const {
    return NmObjWordStrVToDocIdVH[NmObjId][DocIdN];}
  void GetNmObjDocIdV(const int& NmObjId, TIntV& DocIdV) const {
    DocIdV=NmObjWordStrVToDocIdVH[NmObjId];}
  bool IsNmObj(const TStr& WordStr) const {
    TStrV WordStrV; WordStrV.Add(WordStr);
    return IsNmObj(WordStrV);}
  bool IsNmObj(const TStrV& WordStrV) const {
    return NmObjWordStrVToDocIdVH.IsKey(WordStrV);}
  int GetNmObjId(const TStrV& WordStrV) const {
    return NmObjWordStrVToDocIdVH.GetKeyId(WordStrV);}
  int GetNmObjId(const TStr& NmObjStr) const {
    TStrV WordStrV; TNmObjBs::GetWordStrV(NmObjStr, WordStrV);
    return GetNmObjId(WordStrV);}
  static void GetWordStrV(const TStr& WordVStr, TStrV& WordStrV){
    WordVStr.SplitOnAllCh('_', WordStrV);}

  // documents retrieval
  int GetDocs() const {return DocNmToNmObjDocH.Len();}
  bool IsDoc(const TStr& DocNm) const {
    return DocNmToNmObjDocH.IsKey(DocNm);}
  int AddDoc(const TStr& DocNm, const TStr& DateStr, const TIntV& NmObjIdV);
  int GetDocId(const TStr& DocNm) const {
    return DocNmToNmObjDocH.GetKeyId(DocNm);}
  TStr GetDocNm(const int& DocId) const {
    return DocNmToNmObjDocH.GetKey(DocId);}
  TStr GetDocDateStr(const int& DocId) const {
    return DocNmToNmObjDocH[DocId]->DateStr;}
  int GetDocNmObjs(const int& DocId) const {
    return DocNmToNmObjDocH[DocId]->NmObjIdFqPrV.Len();}
  void GetDocNmObjId(
   const int& DocId, const int& NmObjN, int& NmObjId, int& Fq) const {
    const TIntPr& NmObjIdFqPr=DocNmToNmObjDocH[DocId]->NmObjIdFqPrV[NmObjN];
    NmObjId=NmObjIdFqPr.Val1; Fq=NmObjIdFqPr.Val2;}
  TIntPrV& GetDoc_NmObjIdFqPrV(const int& DocId) const {
    return DocNmToNmObjDocH[DocId]->NmObjIdFqPrV;}
  TStr GetDoc_NmObjStrVStr(const int& DocId, const char& SepCh=';') const;

  // bag-of-words
  PBowDocBs GetBowDocBs(const int& MnNmObjFq=-1) const;
  PBowDocBs GetNmBowDocBs(
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const int& MnNmObjFq) const;
  PBowDocBs GetRelBowDocBs(
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const int& MnNmObjFq) const;

  // creation
  // ...string-triples
  static PNmObjBs GetFromStrQuV(
   const TStrQuV& IdTitleSrcHtmlQuV,
   const TSwSetType& SwSetType=swstEn523, const TStr& CustSwSetFNm="",
   const TStr& NrWordBsFNm="", const TStr& WordTypeBsFNm="", const TStr& MteFNm="",
   const int& MxDocs=-1, const bool& DumpP=false);
  // ...html-cpd
  static PNmObjBs GetFromCpd(
   const PSIn& CpdSIn,
   const TSwSetType& SwSetType, const PSIn& CustSwSetSIn,
   const PSIn& NrWordBsSIn, const PSIn& WordTypeBsSIn, const TStr& MteFNm="",
   const int& MxDocs=-1, const bool& DumpP=false);
  static PNmObjBs GetFromCpd(
   const TStr& CpdFNm,
   const TSwSetType& SwSetType, const TStr& CustSwSetFNm,
   const TStr& NrWordBsFNm, const TStr& WordTypeBsFNm, const TStr& MteFNm="",
   const int& MxDocs=-1, const bool& DumpP=false);

  // files
  // ...binary
  static PNmObjBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  // ...statistics
  void SaveTxtNmObj(const TStr& FqFNm, const TStr& SwFNm,
   const TStr& AbcFNm, const TStr& DocFNm) const;
};

#endif
