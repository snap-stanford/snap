#ifndef snap_spinn3r_h
#define snap_spinn3r_h

#include "Snap.h"

/////////////////////////////////////////////////
// Spinn3r RSS feed item (feed.getDelta api call)
class TSpinn3rFeedItem {
private:
  TFFile FFile;
  PSIn SIn;
  TXmlLx* XmlLxPt;
  TExeTm ExeTm;
public:
  TStr CurFNm;
  TInt PostCnt, GoodPostCnt, LinkPostCnt, LinksCnt;
  TExeTm TotExeTm;
  TChA PostTitleStr;
  TChA PostUrlStr;
  TSecTm PubTm;
  TChA BlogUrlStr;
  TChA BlogTitleStr;
  TChA BlogDesc;
  TChA BlogLang;
  TChA ContentStr;
  TVec<TChA> LinkV;
private:
  bool LoadItem(TXmlLx& XmlLx);
public:
  TSpinn3rFeedItem(const TStr& InFNmWc) : FFile(InFNmWc), XmlLxPt(NULL) { }
  virtual ~TSpinn3rFeedItem() { if (XmlLxPt!=NULL) { delete XmlLxPt; } }
  void Clr();
  bool Next();
  void ProcessPosts(const TStr& XmlFNmWc, int LoadN=-1);
  // events
  virtual void StartProcess() { }
  virtual void EndProcess() { }
  virtual void StartProcFile(const TStr& FNm) { }
  virtual void EndProcFile(const TStr& FNm) { }
  virtual void ProcessPost(const bool& GoodItem) { }
};

/////////////////////////////////////////////////
// Spinn3r full post item (permalink.getDelta api call)
class TSpinn3rFullItem {
protected:
  TStr InFNmWc;
  PFFile FFile;
  PSIn SIn;
  TXmlLx* XmlLxPt;
  TExeTm ExeTm;
public:
  TStr CurFNm;
  TInt PostCnt, GoodPostCnt, LinkPostCnt, LinksCnt;
  TExeTm TotExeTm;
  TChA PostTitleStr;
  TChA PostUrlStr;
  TSecTm PubTm;
  TChA BlogUrlStr;
  TChA BlogTitleStr;
  TChA BlogDesc;
  TChA BlogLang;
  TChA PageHtmlStr;
  TChA ContentStr;
  TVec<TChA> LinkV;
private:
  bool LoadItem(TXmlLx& XmlLx);
public:
  TSpinn3rFullItem() : FFile(NULL), XmlLxPt(NULL) { }
  TSpinn3rFullItem(const TStr& _InFNmWc) : InFNmWc(_InFNmWc), XmlLxPt(NULL) {
    if (InFNmWc.SearchCh('?')!=-1 || InFNmWc.SearchCh('*')!=-1) {
	  FFile = TFFile::New(InFNmWc, false); } }
  virtual ~TSpinn3rFullItem() { if (XmlLxPt!=NULL) { delete XmlLxPt; } }
  TSpinn3rFullItem(const TSpinn3rFullItem& Item);
  TSpinn3rFullItem& operator = (const TSpinn3rFullItem& Item);
  TSpinn3rFullItem(TSIn& SIn);
  void Save(TSOut& SOut) const;
  void Load(TSIn& SIn);
  void Clr();
  bool Next();
  void ProcessPosts(const TStr& XmlFNmWc, int LoadN=-1);
  // events
  virtual void StartProcess() { }
  virtual void EndProcess() { }
  virtual void StartProcFile(const TStr& FNm) { }
  virtual void EndProcFile(const TStr& FNm) { }
  virtual void ProcessPost(const bool& GoodItem) { }
};

/////////////////////////////////////////////////
// Spinner Quote Extractor
class TQuoteExtractor : public TSpinn3rFullItem {
public:
  TInt BadPostCnt, NoContentCnt, NoLatinCnt, GoodLatinCnt, QuotePostCnt, QuotesCnt;
  TStr CurInFNm;
  TExeTm ExeTm;
  TBool DoSave;
  TStr OutFNmPref;
  FILE *FOut;
  PSOut BinFOut;
  // data
  TChA PostText;
  TVec<TChA> QuoteV;
public:
  TQuoteExtractor(const TStr& InFNmWc) : TSpinn3rFullItem(InFNmWc), DoSave(false), FOut(NULL) { }
  TQuoteExtractor(const bool& SaveContent);
  ~TQuoteExtractor();

  void GetInsideQuotes();
  bool Next();
  //events
  void ProcessPost(const bool& GoodItem);
  void StartProcFile(const TStr& FNm);
  void DumpStat();
  void SaveToXml();
  void SaveToBin();

  virtual void OnQuotesExtracted(const TQuoteExtractor& QuoteExtractor) { }
};

#endif
