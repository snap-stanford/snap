#ifndef nyta_h
#define nyta_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// NYT-Archive-Document
ClassTPV(TNytaDoc, PNytaDoc, TNytaDocV)//{
public:
  TStr UrlStr;
  TStr TitleXmlStr;
  TStr SummaryXmlStr;
  TStr DateStr;
  TIntV TopicIdV;
public:
  TNytaDoc():
    UrlStr(), TitleXmlStr(), SummaryXmlStr(), DateStr(), TopicIdV(){}
  TNytaDoc(const TNytaDoc& NytaDoc):
    UrlStr(NytaDoc.UrlStr),
    TitleXmlStr(NytaDoc.TitleXmlStr), SummaryXmlStr(NytaDoc.SummaryXmlStr),
    DateStr(NytaDoc.DateStr), TopicIdV(NytaDoc.TopicIdV){}
  TNytaDoc(const TStr& _UrlStr,
   const TStr& _TitleXmlStr, const TStr& _SummaryXmlStr,
   const TStr& _DateStr, const TIntV& _TopicIdV=TIntV()):
    UrlStr(_UrlStr),
    TitleXmlStr(_TitleXmlStr), SummaryXmlStr(_SummaryXmlStr),
    DateStr(_DateStr), TopicIdV(_TopicIdV){}
  static PNytaDoc New(){
    return new TNytaDoc();}
  static PNytaDoc New(const TStr& UrlStr,
   const TStr& TitleXmlStr, const TStr& SummaryXmlStr,
   const TStr& DateStr, const TIntV& TopicIdV=TIntV()){
    return new TNytaDoc(UrlStr, TitleXmlStr, SummaryXmlStr, DateStr, TopicIdV);}
  ~TNytaDoc(){}
  TNytaDoc(TSIn& SIn):
    UrlStr(SIn), TitleXmlStr(SIn), SummaryXmlStr(SIn), DateStr(SIn), TopicIdV(SIn){}
  static PNytaDoc Load(TSIn& SIn){return new TNytaDoc(SIn);}
  void Save(TSOut& SOut){
    UrlStr.Save(SOut);
    TitleXmlStr.Save(SOut); SummaryXmlStr.Save(SOut);
    DateStr.Save(SOut); TopicIdV.Save(SOut);}

  bool operator==(const TNytaDoc& NytaDoc) const {
    return (DateStr==NytaDoc.DateStr)&&(UrlStr==NytaDoc.UrlStr);}
  bool operator<(const TNytaDoc& NytaDoc) const {
    return (DateStr<NytaDoc.DateStr)||
     ((DateStr==NytaDoc.DateStr)&&(UrlStr<NytaDoc.UrlStr));}

  TStr GetYMDDateStr() const {
    return DateStr.GetSubStr(0, 3)+"-"+DateStr.GetSubStr(4, 5)+"-"+
     DateStr.GetSubStr(6, 7);}

  void AddTopicId(const int& TopicId){TopicIdV.AddUnique(TopicId);}
  int GetTopicIds() const {return TopicIdV.Len();}
  int GetTopicId(const int& TopicIdN) const {return TopicIdV[TopicIdN];}
};

/////////////////////////////////////////////////
// NYT-Topic
class TNytTopic{
public:
  TStrV TopicTagNmV;
  TIntV DocIdV;
public:
  TNytTopic(): TopicTagNmV(), DocIdV(){}
  TNytTopic(const TNytTopic& NytTopic):
    TopicTagNmV(NytTopic.TopicTagNmV), DocIdV(NytTopic.DocIdV){}
  ~TNytTopic(){}
  TNytTopic(TSIn& SIn): TopicTagNmV(SIn), DocIdV(SIn){}
  void Save(TSOut& SOut){TopicTagNmV.Save(SOut); DocIdV.Save(SOut);}

  TNytTopic& operator=(const TNytTopic& NytTopic){
    if (this!=&NytTopic){
      TopicTagNmV=NytTopic.TopicTagNmV; DocIdV=NytTopic.DocIdV;}
    return *this;}

  // topic-tags
  void AddTopicTag(const TStr& TopicTagNm){
    TopicTagNmV.AddUnique(TopicTagNm);}
  int GetTopicTags() const {return TopicTagNmV.Len();}
  TStr GetTopicTagNm(const int& TopicTagN) const {
    return TopicTagNmV[TopicTagN];}
};

/////////////////////////////////////////////////
// NYT-Archive-Base
ClassTPV(TNytaBs, PNytaBs, TNytaBsV)//{
private:
  TNytaDocV NytaDocV;
  THash<TStrV, TNytTopic> TokStrVToTopicH;
  TStrIntVH TopicNmFirstTokStrToToksVH;
  UndefAssign(TNytaBs);
public:
  TNytaBs(): NytaDocV(){}
  TNytaBs(const TNytaBs& NytaBs): NytaDocV(NytaBs.NytaDocV){}
  static PNytaBs New(){return new TNytaBs();}
  ~TNytaBs(){}
  TNytaBs(TSIn& SIn): NytaDocV(SIn){}
  static PNytaBs Load(TSIn& SIn){return new TNytaBs(SIn);}
  void Save(TSOut& SOut){NytaDocV.Save(SOut);}

  // documents
  int GetDocs() const {return NytaDocV.Len();}
  void AddDoc(const PNytaDoc& NytaDoc){NytaDocV.Add(NytaDoc);}
  PNytaDoc GetDoc(const int& DocN) const {return NytaDocV[DocN];}
  void SortDocs(){NytaDocV.Sort();}

  // nyt-topics
  void LoadNytTopics(const TStr& FPath);
  void LoadGeoNames(const TStr& FPath);
  void SaveNytTopicsTxt(const TStr& FNm);
  void AnnotateWithNytTopics();


  // topics
  int AddTopicNm(const TStrV& TopicNmTokStrV,
   const TStr& TopicTagNm1=TStr(), const TStr& TopicTagNm2=TStr(),
   const TStr& TopicTagNm3=TStr());
  int GetTopicNms() const {return TokStrVToTopicH.Len();}
  TStr GetTopicNm(const int& TopicId) const;
  bool IsTopic(const TStrV& TopicNmTokStrV, int& TopicId) const {
    return TokStrVToTopicH.IsKey(TopicNmTokStrV, TopicId);}
  int GetTopicId(const TStrV& TopicNmTokStrV) const {
    return TokStrVToTopicH.GetKeyId(TopicNmTokStrV);}
  void AddTopicDocId(const int& TopicId, const int& DocId){
    TokStrVToTopicH[TopicId].DocIdV.AddUnique(DocId);}
  int GetTopicDocIds(const int& TopicId) const {
    return TokStrVToTopicH[TopicId].DocIdV.Len();}
  int GetTopicDocId(const int& TopicId, const int& DocIdN) const {
    return TokStrVToTopicH[TopicId].DocIdV[DocIdN];}
  int GetTopicTags(const int& TopicId) const {
    return TokStrVToTopicH[TopicId].TopicTagNmV.Len();}
  TStr GetTopicTagNm(const int& TopicId, const int& TopicTagN) const {
    return TokStrVToTopicH[TopicId].TopicTagNmV[TopicTagN];}

  // files
  static PNytaBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm);
  void SaveR2KXml(const TStr& FNm);
};

#endif
