#ifndef google_h
#define google_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "mine.h"

/////////////////////////////////////////////////
// Google-Scholar-Reference
ClassTPV(TGgSchRef, PGgSchRef, TGgSchRefV)//{
public:
  TStr TitleStr;
  TStrV AuthNmV;
  TStr PubNm;
  TStr YearStr;
  TInt Citations;
  TStr CitedByUrlStr;
  UndefCopyAssign(TGgSchRef);
public:
  TGgSchRef():
    TitleStr(), AuthNmV(), PubNm(), YearStr(), Citations(), CitedByUrlStr(){}
  TGgSchRef(
   const TStr& _TitleStr, const TStrV& _AuthNmV,
   const TStr& _PubNm, const TStr& _YearStr,
   const int& _Citations, const TStr& _CitedByUrlStr):
    TitleStr(_TitleStr), AuthNmV(_AuthNmV),
    PubNm(_PubNm), YearStr(_YearStr),
    Citations(_Citations), CitedByUrlStr(_CitedByUrlStr){}
  static PGgSchRef New(
   const TStr& TitleStr, const TStrV& AuthNmV,
   const TStr& PubNm, const TStr& YearStr,
   const int& Citations, const TStr& CitedByUrlStr){
    return new TGgSchRef(TitleStr, AuthNmV, PubNm, YearStr, Citations, CitedByUrlStr);}
  ~TGgSchRef(){}
  TGgSchRef(TSIn& SIn):
    TitleStr(SIn), AuthNmV(SIn),
    PubNm(SIn), YearStr(SIn),
    Citations(SIn), CitedByUrlStr(SIn){}
  static PGgSchRef Load(TSIn& SIn){return new TGgSchRef(SIn);}
  void Save(TSOut& SOut){
    TitleStr.Save(SOut); AuthNmV.Save(SOut);
    PubNm.Save(SOut); YearStr.Save(SOut);
    Citations.Save(SOut), CitedByUrlStr.Save(SOut);}

  TMd5Sig GetMd5Sig() const;

  static void GetAuthNmVPubStr(
   const TStr& AuthVPubStr, TStrV& AuthNmV, TStr& PubNm, TStr& PubYearStr);

  void SaveXml(FILE* fOut, const int& RefN=-1);
};

/////////////////////////////////////////////////
// Google-Scholar-Result-Set
ClassTPV(TGgSchRSet, PGgSchRSet, TGgSchRSetV)//{
private:
  TStr UrlStr;
  TStr NextUrlStr;
  TStr QueryStr;
  TInt AllHits;
  TGgSchRefV RefV;
  UndefAssign(TGgSchRSet);
public:
  TGgSchRSet():
    UrlStr(), NextUrlStr(), QueryStr(), AllHits(), RefV(){}
  TGgSchRSet(const TGgSchRSet& RSet):
    UrlStr(RSet.UrlStr), NextUrlStr(RSet.NextUrlStr),
    QueryStr(RSet.QueryStr), AllHits(RSet.AllHits), RefV(RSet.RefV){}
  static PGgSchRSet New(){return new TGgSchRSet();}
  static PGgSchRSet NewScholar(const TStr& UrlStr, const TStr& HtmlStr);
  static PGgSchRSet NewScholar(const PWebPg& WebPg);
  static PGgSchRSet New(const PGgSchRSet& RSet){
    return new TGgSchRSet(*RSet);}
  ~TGgSchRSet(){}
  TGgSchRSet(TSIn& SIn):
    UrlStr(SIn), NextUrlStr(SIn), QueryStr(SIn),
    AllHits(SIn), RefV(SIn){}
  static PGgSchRSet Load(TSIn& SIn){return new TGgSchRSet(SIn);}
  void Save(TSOut& SOut){
    UrlStr.Save(SOut); NextUrlStr.Save(SOut); QueryStr.Save(SOut);
    AllHits.Save(SOut); RefV.Save(SOut);}

  // components
  void PutUrlStr(const TStr& _UrlStr){UrlStr=_UrlStr;}
  TStr GetUrlStr() const {return UrlStr;}
  void PutNextUrlStr(const TStr& _NextUrlStr){NextUrlStr=_NextUrlStr;}
  TStr GetNextUrlStr() const {return NextUrlStr;}
  void PutQueryStr(const TStr& _QueryStr){QueryStr=_QueryStr;}
  TStr GetQueryStr() const {return QueryStr;}
  void PutAllHits(const int& _AllHits){AllHits=_AllHits;}
  int GetAllHits() const {return AllHits;}
  const TGgSchRefV& GetRefV() const {return RefV;}

  // hits
  int GetHits() const {return RefV.Len();}
  void AddHit(const PGgSchRef& Ref){
    RefV.Add(Ref);}
  PGgSchRef GetHit(const int& HitN) const {
    return RefV[HitN];}

  // managment
  void Merge(const PGgSchRSet& RSet);
  void Trunc(const int& MxHits){
    RefV.Trunc(MxHits);}

  // bag-of-words
  PBowDocBs GetBowDocBs() const;

  // files
  static PGgSchRSet LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  static void SaveBin(const TStr& FNm, const PGgSchRSet& GgSchRSet);
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  static void SaveXml(const TStr& FNm, const PGgSchRSet& GgSchRSet);
  void SaveXml(const TStr& FNm);

  // result-set-vectors (Result-Sets)
  static void LoadRSetsBin(const TStr& FNm, TGgSchRSetV& SchRSetV);
  static void SaveRSetsBin(const TStr& FNm, TGgSchRSetV& SchRSetV);
};

/////////////////////////////////////////////////
// Google-Scholar-Base
ClassTP(TGgSchBs, PGgSchBs)//{
private:
  THash<TMd5Sig, PGgSchRef> RefMd5ToRefH; // set of references
  THash<TMd5Sig, TMd5SigV> RefMd5ToRefMd5VH; // cited by references
  THash<TMd5Sig, TBool> RefMd5ToRefCiteCrawlPH; // citers to crawl
  TStrBoolH AuthNmToCrawlPH; // authors to crawl
  TStrBoolH PubNmToCrawlPH; // publications to crawl
public:
  TGgSchBs():
    RefMd5ToRefH(), RefMd5ToRefMd5VH(), RefMd5ToRefCiteCrawlPH(),
    AuthNmToCrawlPH(), PubNmToCrawlPH(){}
  static PGgSchBs New(){return new TGgSchBs();}
  TGgSchBs(TSIn& SIn):
    RefMd5ToRefH(SIn), RefMd5ToRefMd5VH(SIn), RefMd5ToRefCiteCrawlPH(SIn),
    AuthNmToCrawlPH(SIn), PubNmToCrawlPH(SIn){}
  static PGgSchBs Load(TSIn& SIn){return new TGgSchBs(SIn);}
  void Save(TSOut& SOut){
    RefMd5ToRefH.Save(SOut); RefMd5ToRefMd5VH.Save(SOut);
    RefMd5ToRefCiteCrawlPH.Save(SOut);
    AuthNmToCrawlPH.Save(SOut); PubNmToCrawlPH.Save(SOut);}

  void AddRef(const PGgSchRef& Ref);
  void AddRSet(const PGgSchRSet& RSet);
  TStr GetAuthNmToCrawl() const;
  void SetAuthCrawled(const TStr& AuthNm);

  int GetRefs() const {return RefMd5ToRefH.Len();}
  PGgSchRef GetRef(const int& RefN) const {return RefMd5ToRefH[RefN];}

  // files
  static PGgSchBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveXml(const TStr& FNm);
};

/////////////////////////////////////////////////
// Result-Set
ClassTPV(TRSet, PRSet, TRSetV)//{
private:
  TStr UrlStr;
  TStr NextUrlStr;
  TStr QueryStr;
  TInt AllHits;
  TStrQuV UrlTitleSrcCtxQuV;
  UndefAssign(TRSet);
public:
  TRSet():
    UrlStr(), NextUrlStr(), QueryStr(), AllHits(), UrlTitleSrcCtxQuV(){}
  TRSet(const TRSet& RSet):
    UrlStr(RSet.UrlStr), NextUrlStr(RSet.NextUrlStr),
    QueryStr(RSet.QueryStr), AllHits(RSet.AllHits),
    UrlTitleSrcCtxQuV(RSet.UrlTitleSrcCtxQuV){}
  static PRSet New(){return new TRSet();}
  static PRSet NewWeb(const TStr& UrlStr, const TStr& HtmlStr);
  static PRSet NewWeb(const PWebPg& WebPg);
  static PRSet NewNews(const TStr& UrlStr, const TStr& HtmlStr);
  static PRSet NewNews(const PWebPg& WebPg);
  static PRSet New(const PRSet& RSet){
    return new TRSet(*RSet);}
  ~TRSet(){}
  TRSet(TSIn& SIn):
    UrlStr(SIn), NextUrlStr(SIn), QueryStr(SIn),
    AllHits(SIn), UrlTitleSrcCtxQuV(SIn){}
  static PRSet Load(TSIn& SIn){return new TRSet(SIn);}
  void Save(TSOut& SOut){
    UrlStr.Save(SOut); NextUrlStr.Save(SOut); QueryStr.Save(SOut);
    AllHits.Save(SOut); UrlTitleSrcCtxQuV.Save(SOut);}

  // components
  void PutUrlStr(const TStr& _UrlStr){UrlStr=_UrlStr;}
  TStr GetUrlStr() const {return UrlStr;}
  void PutNextUrlStr(const TStr& _NextUrlStr){NextUrlStr=_NextUrlStr;}
  TStr GetNextUrlStr() const {return NextUrlStr;}
  void PutQueryStr(const TStr& _QueryStr){QueryStr=_QueryStr;}
  TStr GetQueryStr() const {return QueryStr;}
  void PutAllHits(const int& _AllHits){AllHits=_AllHits;}
  int GetAllHits() const {return AllHits;}
  const TStrQuV& GetUrlTitleSrcCtxQuV() const {return UrlTitleSrcCtxQuV;}

  // hits
  int GetHits() const {return UrlTitleSrcCtxQuV.Len();}
  int GetHitN(const TStr& UrlStr, const bool& LcP=true) const;
  void AddHit(const TStr& UrlStr, const TStr& TitleStr, const TStr& SrcNm, const TStr& CtxStr){
    UrlTitleSrcCtxQuV.Add(TStrQu(UrlStr, TitleStr, SrcNm, CtxStr));}
  void GetHit(const int& HitN, TStr& UrlStr, TStr& TitleStr, TStr& SrcNm, TStr& CtxStr) const {
    UrlStr=UrlTitleSrcCtxQuV[HitN].Val1;
    TitleStr=UrlTitleSrcCtxQuV[HitN].Val2;
    SrcNm=UrlTitleSrcCtxQuV[HitN].Val3;
    CtxStr=UrlTitleSrcCtxQuV[HitN].Val4;}
  TStr GetHitUrlStr(const int& HitN) const {return UrlTitleSrcCtxQuV[HitN].Val1;}
  TStr GetHitTitleStr(const int& HitN) const {return UrlTitleSrcCtxQuV[HitN].Val2;}
  TStr GetHitSrcNm(const int& HitN) const {return UrlTitleSrcCtxQuV[HitN].Val3;}
  TStr GetHitCtxStr(const int& HitN) const {return UrlTitleSrcCtxQuV[HitN].Val4;}

  // managment
  void Merge(const PRSet& RSet);
  void Trunc(const int& MxHits){
    UrlTitleSrcCtxQuV.Trunc(MxHits);}

  // bag-of-words
  PBowDocBs GetBowDocBs(
   const TStr& SwSetTypeNm="en523", const TStr& StemmerTypeNm="porter",
   const int& MxNGramLen=3, const int& MnNGramFq=5) const;

  // files
  static PRSet LoadBin(const TStr& FNm, const bool& MultiRSetsP=false);
  static void SaveBin(const TStr& FNm, const PRSet& RSet, const bool& Append=false);
  void SaveBin(const TStr& FNm, const bool& Append=false){
    TFOut SOut(FNm, Append); Save(SOut);}
  static void SaveXml(const TStr& FNm, const PRSet& RSet);
  void SaveXml(const TStr& FNm);

  // result-set-vectors (Result-Sets)
  static void LoadRSetsBin(const TStr& FNm, TRSetV& RSetV);
  static void SaveRSetsBin(const TStr& FNm, TRSetV& RSetV);
};

/////////////////////////////////////////////////
// Google-Web-Fetch-Saver
class TGgWebFetchSaver: public TWebPgFetch{
private:
  TWebPgV WebPgV;
  UndefDefaultCopyAssign(TGgWebFetchSaver);
public:
  TGgWebFetchSaver(const int& _MxConns):
    TWebPgFetch(){PutMxConns(_MxConns);}

  void OnFetch(const int&, const PWebPg& WebPg);
  void OnError(const int&, const TStr& MsgStr);

  int GetWebPgs() const {return WebPgV.Len();}
  PWebPg GetWebPg(const int& WebPgN) const {return WebPgV[WebPgN];}
  PWebPg GetWebPg(const TStr& UrlStr) const;
};

/////////////////////////////////////////////////
// Google-Focused-Crawl
ClassTPV(TGgFCrawl, PGgFCrawl, TGgFCrawlV)//{
private:
  bool Ok;
  TStr SrcUrlStr;
  PWebPg SrcWebPg;
  int SrcDId;
  PRSet DstRSet;
  THash<TStr, PWebPg> UrlStrToWebPgH;
  PBowDocBs BowDocBs;
  double SumSim;
  TFltIntKdV SimDIdKdV;
  UndefCopyAssign(TGgFCrawl);
public:
  TGgFCrawl(): Ok(false){}
  static PGgFCrawl New(){return new TGgFCrawl();}

  bool IsOk() const {return Ok;}

  static PGgFCrawl GetFCrawl(
   const TStr& UrlStr, const int& MxCands=-1, const TStr& ProxyStr="");

  void SaveXml(const TStr& FNm, const bool& SaveDocP=false);
  void SaveTxt(const TStr& FNm);
};

/////////////////////////////////////////////////
// Google-Web-Context-Graph
ClassTPV(TGgCtxGraph, PGgCtxGraph, TGgCtxGraphV)//{
private:
  bool Ok;
  TStr FocusUrlStr;
  PWebPg FocusWebPg;
  TStrPrV InUrlCtxStrPrV;
  TStrPrV OutUrlCtxStrPrV;
  UndefCopyAssign(TGgCtxGraph);
public:
  TGgCtxGraph(): Ok(false){}
  static PGgCtxGraph New(){return new TGgCtxGraph();}

  bool IsOk() const {return Ok;}

  static PGgCtxGraph GetCtxGraph(const TStr& UrlStr);

  void SaveXml(const TStr& FNm, const bool& SaveDocP=false);
  void SaveTxt(const TStr& FNm);
};

/////////////////////////////////////////////////
// Google
ClassTP(TGg, PGg)//{
public:
  // url constructor
  static TStr GetWebSearchUrlStr(const TStr& QueryStr);
  static TStr GetNewsSearchUrlStr(const TStr& QueryStr);
  static TStr GetScholarSearchUrlStr(const TStr& QueryStr);
  static TStr GetScholarAuthorSearchUrlStr(const TStr& QueryStr);
  static TStr GetScholarPublicationSearchUrlStr(const TStr& QueryStr);

  // google-web search
  static PRSet WebSearch(const TStr& QueryStr, const int& MxHits=-1,
   const PNotify& Notify=NULL, const TStr& ProxyStr="");
  static PRSet WebSearchExternal(
   const TStr& QueryStr, const int& MxHits=-1, const PNotify& Notify=NULL);

  // google-news search
  static PRSet NewsSearch(const TStr& QueryStr, const int& MxHits=-1, const PNotify& Notify=NULL);

  // google-scholar search
  static PGgSchRSet _ScholarSearch(const TStr& SearchUrlStr, const PNotify& Notify=NULL);
  static PGgSchRSet ScholarSearch(const TStr& QueryStr, const PNotify& Notify=NULL);
  static PGgSchRSet ScholarAuthorSearch(const TStr& AuthorNm, const PNotify& Notify=NULL);
  static PGgSchRSet ScholarPublicationSearch(const TStr& PublicationNm, const PNotify& Notify=NULL);
};

#endif
