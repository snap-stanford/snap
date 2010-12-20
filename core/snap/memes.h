#ifndef snap_memes_h
#define snap_memes_h

#include "Snap.h"
//#include "spinn3r.h"

// qtInClust:Part of a cluster, qtRoot:root of a cluster, qtCentr: artificial quote (cluster summary)
typedef enum {
  qtUndef, qtQuote, qtInClust, qtRoot, qtCentr
} TQtTy;

// url type
typedef enum {
  utUndef=0, utBlogLeft=1, utBlogRight=2, utBlog=3, utMedia=4
} TUrlTy; // utBlog is the default type

class TQuoteBs;
typedef TPt<TQuoteBs> PQuoteBs;
class TClustNet;
typedef TPt<TClustNet>  PClustNet;
typedef TNodeEDatNet<TPair<TStr, TInt>, TInt> TQtDomNet;  // domain name, url type
typedef TPt<TQtDomNet> PQtDomNet;

/////////////////////////////////////////////////
// Quote
#pragma pack(push, 1)
class TQuote {
public:
  class TTmUrlCnt;
  typedef TVec<TTmUrlCnt> TTmUrlCntV;
  typedef TVec<TPair<TSecTm, TFlt> > TTmFltPrV;
public:
  class TTmUrlCnt {
  private:
    TSecTm QtTm;
    int QtUrlId;
    uchar QtCnt;
  public:
    TTmUrlCnt() : QtTm(), QtUrlId(0), QtCnt(0) { }
    TTmUrlCnt(const TSecTm& Tm, const int& Url, const int& _QtCnt) : QtTm(Tm), QtUrlId(Url) {
      if (_QtCnt>255) { QtCnt=255; } else { QtCnt=_QtCnt; } }
    TTmUrlCnt(const TTmUrlCnt& UrlCnt) { memcpy((void*)this, (void*)&UrlCnt, sizeof(TTmUrlCnt)); }
    TTmUrlCnt(TSIn& SIn) { SIn.LoadBf(this, sizeof(TTmUrlCnt)); }
    void Load(TSIn& SIn) { SIn.LoadBf(this, sizeof(TTmUrlCnt)); }
    void Save(TSOut& SOut) const { SOut.SaveBf(this, sizeof(TTmUrlCnt)); }
    TSecTm Tm() const { return QtTm; }
    int UrlId() const { return QtUrlId; }
    void SetUrlId(const int& _UrlId) { QtUrlId=_UrlId; }
    int Cnt() const { return (int)QtCnt; }
    TTmUrlCnt& operator = (const TTmUrlCnt& Tuc) { if(this!=&Tuc){memcpy((void*)this, (void*)&Tuc, sizeof(TTmUrlCnt));} return *this; }
    bool operator == (const TTmUrlCnt& Val) const { return memcmp((void*) this, (void*)&Val, sizeof(TTmUrlCnt))==0; }
    bool operator < (const TTmUrlCnt& Val) const {
      return QtTm < Val.QtTm || (QtTm==Val.QtTm && UrlId()<Val.UrlId()) || (QtTm==Val.QtTm && UrlId()==Val.UrlId() && Cnt()<Val.Cnt()); }
  };
  class TQtIdTy {
  public:
    uint Id:26;
    uint Ty:4;
  public:
    TQtIdTy(const int& _Id=0, const TQtTy& _Ty=qtUndef) : Id(_Id), Ty((uint)_Ty) { IAssert(_Id<(int)TMath::Pow2(26) && int(_Ty)<16); }
    TQtIdTy(TSIn& SIn) { SIn.LoadBf(this, sizeof(TQtIdTy)); }
    void Load(TSIn& SIn) { SIn.LoadBf(this, sizeof(TQtIdTy)); }
    void Save(TSOut& SOut) const { SOut.SaveBf(this, sizeof(TQtIdTy)); }
  };
//private:
public:
  TQtIdTy QtCIdTy; // if type qtQuote: quote id; if qtInClust: id of the parent; qtRoot, qtCentr
  TStr QtStr;      // artificial cluster root quote has string QtStr
  TVec<TTmUrlCnt> TmUrlCntV;
public:
  TQuote() { }
  TQuote(TSIn& SIn) : QtCIdTy(SIn), QtStr(SIn), TmUrlCntV(SIn) { }
  void Save(TSOut& SOut) const { QtCIdTy.Save(SOut); QtStr.Save(SOut); TmUrlCntV.Save(SOut); }
  void Load(TSIn& SIn) { QtCIdTy.Load(SIn); QtStr.Load(SIn); TmUrlCntV.Load(SIn); }
  PXmlTok GetXmlTok() const { Fail; return TXmlTok::New("NodeData"); }

  int GetId() const { return QtCIdTy.Id; }
  int GetCId() const { return QtCIdTy.Id; }
  TQtTy GetTy() const { return (TQtTy) QtCIdTy.Ty; }
  TStr GetStr() const { return QtStr; }
  int GetFq() const; // actually returns the number of URLS
  int GetFq(const TSecTm& BegTm, const TSecTm& EndTm) const; // actually returns the number of URLS
  int GetUrls() const { return TmUrlCntV.Len(); }
  int GetDoms(const TQuoteBs& QtBs) const;
  int GetFq(const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  int GetFq(const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  int GetUrls(const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  int GetTimes() const { return TmUrlCntV.Len(); }
  TSecTm GetTm(const int& t) const { return TmUrlCntV[t].Tm(); }
  int GetUrlId(const int& t) const { return TmUrlCntV[t].UrlId(); }
  int GetCnt(const int& t) const { return TmUrlCntV[t].Cnt(); }
  TSecTm GetMinTm() const { return TmUrlCntV.Empty()?TSecTm():TmUrlCntV[0].Tm(); }
  TSecTm GetMaxTm() const { return TmUrlCntV.Empty()?TSecTm():TmUrlCntV.Last().Tm(); }
  TSecTm GetPeakTm(const TTmUnit& TmUnit=tmuDay, const TSecTm& AfterTm=TSecTm(1)) const;
  TSecTm GetPeakTm(const TTmUnit& TmUnit, const TSecTm& AfterTm, TInt& FreqAtPeak) const;
  TSecTm GetPeakTm(const TTmUnit& TmUnit, const TSecTm& AfterTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  TSecTm GetMeanTm(const TTmUnit& TmUnit, const TUrlTy& UrlTy, const TQuoteBs& QtBs, const TSecTm& AfterTm=TSecTm(1)) const;
  TSecTm GetMedianTm(const TTmUnit& TmUnit, const TUrlTy& UrlTy, const TQuoteBs& QtBs, const TSecTm& AfterTm=TSecTm(1)) const;
  bool IsSinglePeak(const TTmUnit& TmUnit, const TSecTm& AfterTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  TStr GetDesc() const;

  void GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit) const;
  void GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  void GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const TSecTm& BegTm, const TSecTm& EndTm) const;
  void GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const;
  void GetSmoothFqOt(TTmFltPrV& FqOtV, const TTmUnit& TmUnit, const int& WndSz, const double& Smooth,
    const TSecTm& BegTm=TSecTm(0), const TSecTm& EndTm=TSecTm(TInt::Mx-1)) const;
  void PlotOverTm(const TStr& OutFNm);

  static void GetSmoothFqOt(TTmFltPrV& FqOtV, const TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const int& WndSz, const double& Smooth, const TSecTm& BegTm=TSecTm(0), const TSecTm& EndTm=TSecTm(TInt::Mx-1));
  static void LoadQtV(const TStr& InFNm, TVec<TQuote>& QtV);
  friend class TQuoteBs;
};
#pragma pack(pop)

typedef TVec<TQuote> TQuoteV;

/////////////////////////////////////////////////
// Quote Base
class TQuoteBs {
//private:
public:
  TCRef CRef;
  //TStrHash<TInt> StrQtIdH;    // stores quotes, domains and urls (dat = -1)  (OLD VERSION)
  TStrHash<TInt, TBigStrPool> StrQtIdH;    // new for QtBs from Aug1 to Dec 31 (NEW VERSION, BIG POOL)
  THash<TInt, TQuote> QuoteH; // QtIds go from 0...
  TIntH UrlInDegH;  // Url/Domain in-degree (only for Domains/URLs that have quotes)
  TIntH UrlTyH;     // UrlId to TypeId (only for known URLs that have quotes)
  THash<TInt, TIntV> ClustQtIdVH; // root quote id --> all quotes in the cluster
public:
  //TQuoteBs() : StrQtIdH(TBigStrPool::New(Mega(2000), Mega(256))) { }
  TQuoteBs() : StrQtIdH() { }
  TQuoteBs(TSIn& SIn);
  void Save(TSOut& SOut) const;
  static PQuoteBs New();
  static PQuoteBs Load(TSIn& SIn);

  int GetStrs() const { return StrQtIdH.Len(); }
  const char *GetStr(const int& StrId) const { return StrQtIdH.GetKey(StrId); }
  bool IsStrId(const int& StrId) const { return StrId < StrQtIdH.Len(); }
  bool IsStr(const char* CStr) const { return StrQtIdH.IsKey(CStr); }
  bool IsStr(const TChA& Str) const { return StrQtIdH.IsKey(Str.CStr()); }
  int GetStrId(const char* CStr) const { return StrQtIdH.GetKeyId(CStr); }
  int GetStrId(const TChA& Str) const { return StrQtIdH.GetKeyId(Str.CStr()); }
  int AddStr(const char* CStr) { return StrQtIdH.AddDat(CStr, -1); }
  int AddStr(const TChA& ChA) { return StrQtIdH.AddDat(ChA.CStr(), -1); }

  int Len() const { return QuoteH.Len(); }
  bool IsQt(const char* CStr) const { return GetQtId(CStr)!=-1; }
  bool IsQtId(const int& QtId) const { return QuoteH.IsKey(QtId); }
  int GetQtId(const int& QtN) const { return QuoteH.GetKey(QtN); }
  int GetQtId(const char* CStr) const { return IsStr(CStr)? StrQtIdH.GetDat(CStr).Val : -1; }
  const TQuote& GetQt(const int& QtId) const { return QuoteH.GetDat(QtId); }
  TQuote& GetQt(const int& QtId) { return QuoteH.GetDat(QtId); }
  const TQuote& GetQtN(const int& QtN) const { return QuoteH[QtN]; }
  TQuote& GetQtN(const int& QtN) { return QuoteH[QtN]; }
  void DelQtId(const int& QtId) { return QuoteH.DelKey(QtId); }

  TUrlTy GetUrlTy(const int& UrlId) const;
  void SetUrlTy(const TStr& InFNm, const TUrlTy& SetTy);

  void GetQtIdV(TIntV& QtIdV) const;
  void GetQtIdVByFq(TIntV& QtIdV, const int& MinWrdLen, const int& MinQtFq, const bool& OnlyClustRoots=true,
    const TStr& HasWord="", const TStr& AppearsAtUrl="", const TUrlTy& OnlyCountTy=utUndef, const TSecTm& BegTm=TSecTm(0), const TSecTm& EndTm=TSecTm(TInt::Mx-1)) const;
  void GetQtIdVByFq(TIntV& QtIdV, const int& MinWrdLen, const int& MinQtFq, const TStrV& FromDomains, const bool& OnlyClustRoots=true, const TStr& HasWord="", int MinDoms=5) const;
  void GetQtIdVByTm(const int& WndSzHr, const int& StepHr, const int& MinWrdLen, const int& MinQtFq, const int& TakePerStep) const;
  void GetCIdVByFq(TIntV& CIdV, const int& MinClFq, const TStr& RootHasWord="", const TUrlTy& OnlyCountTy=utUndef,
    const bool& OnlyAfterRoot=false, const TSecTm& BegTm=TSecTm(0), const TSecTm& EndTm=TSecTm(TInt::Mx-1)) const;
  void GetMinMaxTm(TSecTm& MinTm, TSecTm&MaxTm) const;
  void GetMinMaxTm(const TIntV& QtIdV, TSecTm& MinTm, TSecTm&MaxTm) const;
  void GetQtPageUrl(const TIntV& QtIdV, TIntH& QtUrlIdH) const;

  void AddQuote(const TQuote& Quote, const TQuoteBs& CurQtBs);
  void AddQuote(const TVec<TChA>& QuoteV, const TVec<TChA>& LinkV, const TChA& PostUrlStr, const TSecTm& PubTm, const int& MinQtWrdLen=4);
  PQuoteBs GetQuoteBs(const TIntV& QtIdV) const;

  int GetClusts() const { return ClustQtIdVH.Len(); }
  int GetCId(const int& ClustN) const { return ClustQtIdVH.GetKey(ClustN); }
  bool IsClust(const int& CId) const { return ClustQtIdVH.IsKey(CId); }
  const TIntV& GetClust(const int& CId) const { return ClustQtIdVH.GetDat(CId); } // CId == RootQtId
  const TIntV& GetClustN(const int& N) const { return ClustQtIdVH[N]; } // CId == RootQtId
  int GetQtsInClust() const;
  int GetClustFq(const int& CId) const;
  int GetClustFq(const int& CId, const TUrlTy& UrlTy) const;
  int GetCentrQtId(const int& CId) const;
  int GetCentrQtId(const TIntV& QtIdV) const;
  void GetMergedClustQt(const int& CId, TQuote& NewQt, const bool& OnlyAfterBegTm=true) const;
  void GetMergedClustQt(const TIntV& QtIdV, TQuote& NewQt, const bool& OnlyAfterBegTm=true) const;
  TSecTm GetClustBegTm(const int& CId, const int& CentrQtId) const;
  TSecTm GetClustBegTm(const TIntV& ClustV, const int& CentrQtId) const;
  void CreateClusters(const TVec<TIntV>& ClustQtIdV);
  void ClusterQts(const int& MinRootWrdLen, const int& MinQtFq, const TStr& OutFNmPref, const TStrV& BlackListV=TStrV());
  void ResetClusters();
  void ReassignToClust(const int& QtId, const int& NewCId);
  void Mergec2Clusters(const int& ParentCId, const int& ChildCId);
  PClustNet GetClustNet(const int& MinQtFq, const TStr& OutFNmPref) const;
  PQuoteBs GetMergeClusters(const bool& OnlyClusters=true) const;
  void AddMergedQtsToQtBs();

  void GetTopQtDoms(TStrIntPrV& DomCntV, const int& TakeNClust, const int& MinClFq, const TStr& RootHasWord="", const TUrlTy& OnlyCountTy=utUndef) const;
  void TopDomsByLag(const TTmUnit& TmUnit, const int& TakeNDoms, const int& TakeNClusts, const int& Thresh) const;

  void PlotQtFqCnt(const TStr& OutFNmPref) const;
  void PlotQtMediaVsBlogFq(const int& QtId, const TStr& OutFNmPref) const;
  void PlotClustMediaVsBlogFq(const int& CId, const TStr& OutFNmPref) const;
  void PlotMediaVsBlogLag(const TTmUnit& TmUnit, const bool& TakeClusters, const int& TakeN, const TStr& OutFNmPref) const;
  void PlotFqDecay(const TTmUnit& TmUnit, const bool& TakeClusters, const TUrlTy& CntUrlTy, const int& PlotN, const int& MinValsPerTm, const TStr& OutFNmPref) const;
  void PlotBlogVsMediaFqOt(const TTmUnit& TmUnit, const bool& TakeClusters, int PlotN, const TStr& OutFNmPref) const;
  void PlotBlogFracOt(const TTmUnit& TmUnit, const bool& TakeClusters, const int& PlotN, const TStr& OutFNmPref) const;
  void PlotPopularityCnt(const bool& TakeClusters, const TUrlTy& UrlTy, const int& PlotN, const TStr& OutFNmPref) const;
  void PlotEmptyY(const TTmUnit& TmUnit, const bool& TakeClusters, const int& PlotN, const TStr& OutFNmPref) const;

  PNGraph GetQuotePostNet(const TStr& DatasetFNm) const;
  PQtDomNet GetQuoteDomNet(const PNGraph& PostLinkGraph, const int& CId) const;

  void SaveQuotes(const int& MinQtFq, const TStr& OutFNm) const;
  void SaveQuotes(const TIntV& QtIdV, const TStr& OutFNm) const;
  void SaveClusters(const TStr& OutFNm, const bool& SkipUrls=true) const;
  void SaveClusters(const TIntV& ClustIdV, const TStr& OutFNm, const bool& SkipUrls=true) const;
  void SaveBigBlogMassQt(const TStr& OutFNm) const;
  void SaveForFlash(const TIntV& QtIdV, const TStr& OutFNm, const TTmUnit& TmUnit, int SaveN, const TSecTm& BegTm=TSecTm(1), const TSecTm& EndTm=TSecTm(TInt::Mx-1), const TUrlTy& OnlyCountTy=utUndef) const;
  void SaveForFlash2(const TIntV& QtIdV, const TStr& OutFNm, const TTmUnit& TmUnit, int SaveN, const TSecTm& BegTm=TSecTm(1), const TSecTm& EndTm=TSecTm(TInt::Mx-1), const TUrlTy& OnlyCountTy=utUndef) const;
  void SaveDomainStat(const TStr& OutFNm, const int& MinCnt) const;
  void Dump(const bool& Fast=false) const;

  static int LongestCmnSubSq(const TIntV& WIdV1, const TIntV& WIdV2, int& WIdV1Start, int& WIdV2Start, int& SkipId);
  static PQuoteBs FindQtInTxtQBs(const TStr& InBinFNmWc, const TStrV& QuoteStrV);

  friend class TPt<TQuoteBs>;
};

/////////////////////////////////////////////////
// Make QuoteBase from Spinn3r data
/*class TMakeQtBsFromSpinn3r : public TQuoteExtractor {
public:
  //THashSet<TMd5Sig> QtBlackList;
  PQuoteBs QtBs;
  TInt MinWrdLen;
  // statistics
  TInt Items;
  THashSet<TMd5Sig> DomSet, UrlSet, BlogSet;
  THash<TInt, TFlt> WrdPerH, ItmPerH, UniqItmH, PostPerH, LinkPerH, QtsPerH, BytesPerH;
public:
  TMakeQtBsFromSpinn3r(const PQuoteBs& QtBsPt, const int& MinWordLen);
  void Save(TSOut& SOut) const;
  void SavePlots();
  void OnQuotesExtracted(const TQuoteExtractor& QuoteExtractor);
};*/

/////////////////////////////////////////////////
// Cluster Network
class TClustNet : public TNodeNet<TQuote> {
public:
  TClustNet() { }
  TClustNet(TSIn& SIn) : TNet(SIn) { }
  void Save(TSOut& SOut) const { TNet::Save(SOut); }
  static PClustNet New() { return new TClustNet(); }
  static PClustNet Load(TSIn& SIn) { return new TClustNet(SIn); }

  void AddLink(const TQuote& SrcQt, const TQuote& DstQt);
  PClustNet GetSubGraph(const TIntV& NIdV) const;
  PClustNet GetSubGraph(const int& MinQtWords, const int& MaxQtWords, const int& MinFq) const;

  void RecalcEdges(const double& MinOverlapFrac);
  //void DelLongQts(const int& MxWrdLen);
  void MakeClusters(const TIntPrV& KeepEdgeV);
  void KeepOnlyTree(const TIntPrV& KeepEdgeV);
  void GetClusters(TVec<TIntV>& QtNIdV) const;
  void GetMergedClustQt(const TIntV& QtIdV, TQuote& NewQt) const;

  int EvalPhraseClusters(const TIntPrV& KeepEdgeV, const bool& dump=true) const;
  void ClustKeepSingleEdge(const int& MethodId) const;
  void ClustKeepSingleEdge(const int& MethodId, TIntPrV& KeepEdgeV) const;
  void ClustGreedyTopDown() const;
  void ClustGreedyTopDown(TIntPrV& KeepEdgeV) const;
  void ClustGreedyRandom() const;

  PClustNet GetThis() const { IAssert(CRef.GetRefs()>0); return PClustNet((TClustNet *) this); }
  void DrawNet(const TStr& OutFNm, const int& SaveTopN=10) const;
  void DumpNodes(const TStr& OutFNm, const int& SaveTopN=10) const;
  void DumpClusters(const TStr& OutFNm, int SaveTopN=10) const;
  void DumpClustersByVol(const TStr& OutFNm, const int& MinClustSz, const int& MinVolume) const;

  static PClustNet GetFromQtBs(const PQuoteBs& QtBs, int MinQtFq=5, int MnWrdLen=5);
  friend class TPt<TClustNet>;
};

/////////////////////////////////////////////////
// Quote Loader
class TQuoteLoader {
private:
  TFFile FFile;
  PSIn SIn;
  TExeTm ExeTm;
public:
  TStr CurFNm;
  int PostCnt;
  TSecTm PubTm;
  TChA PostUrlStr;
  TChA PostTitleStr;
  TChA BlogUrlStr;
  TChA BlogTitleStr;
  TChA ContentStr;
  TVec<TChA> QuoteV;
  TVec<TChA> LinkV;
private:
  bool LoadItem(TXmlLx& XmlLx);
public:
  TQuoteLoader(const TStr& InFNmWc) : FFile(InFNmWc), PostCnt(0) { }
  virtual ~TQuoteLoader() { }
  void Save(TSOut& SOut) const;
  void Load(TSIn& SIn);
  void Clr();
  bool Next();
  // events
  void ProcessPosts(const bool& IsXml=false, int LoadN=-1);
  virtual void StartProcess() { }
  virtual void EndProcess(const int& PostCnt) { }
  virtual void StartProcFile(const TStr& FNm) { }
  virtual void EndProcFile(const TStr& FNm) { }
  virtual void ProcessPost(const bool& GoodItem) { }
};

/////////////////////////////////////////////////
// Memes Dataset Loader
// FORMAT:
//U \t Post URL
//D \t Post time
//T \t Post title (optional!)
//C \t Post content
//L \t Index \t URL      (URL starts at Content[Index])
//Q \t Index \t Length \t Quote (Quote starts at Content[Index])
// EXAMPLE:
//U       http://twitter.com/celebritybuzzon/statuses/21622719142
//D       2010-08-19 17:59:42
//C       free energy release free "bang pop" remix mp3, release split 7 ": altsounds.com... and performing live at spin hou...
//L       118     http://bit.ly/aI5wOF
//Q       26      8       bang pop

class TMemesDataLoader {
private:
  PFFile FFile;
  PSIn InFNmF;
  PSIn SInPt;
  TChA CurLn;
  bool GetNextFile();
public:
  uint64 LineCnt;
  TChA PostUrlStr;
  TChA ContentStr;
  TSecTm PubTm;
  TVec<TChA> MemeV;       // quote
  TVec<TIntPr> MemePosV;  // (index, length), quote begins at ContentStr[MemePos[i]]
  TVec<TChA> LinkV;       // link url
  TVec<TInt> LinkPosV;    // url begins at ContentStr[LinkPosV[i]]
public:
  TMemesDataLoader(const TStr& FNmWc, const bool& IsWc=true) {
    if (IsWc) { FFile = TFFile::New(FNmWc, false); } // wild-card expression for loading a list of files
    else { InFNmF = TFIn::New(FNmWc); } // file with one filename in each line
  }
  void Clr();
  bool LoadNext();
  void SaveTxt(TSOut& SOut) const;
  void Dump(const bool& DumpAll=false) const;
};

#endif
