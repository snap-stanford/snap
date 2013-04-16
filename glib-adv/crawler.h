#ifndef crawler_h
#define crawler_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "mine.h"

/////////////////////////////////////////////////
// Forward
ClassHdTP(TCrawler, PCrawler);

/////////////////////////////////////////////////
// Crawler-Def
ClassTP(TCrawlerDef, PCrawlerDef)//{
private:
  int MxUrls;
  int MnConns;
  int MxConns;
  int MxConnsPerSrv;
  int AfterFetchDelaySecs;
  int MxLev;
  int MxDep;
  int MxContLen;
  int MnQLen;
  int QResetMod;
  int MxQSegLen;
  int MxRetries;
  int RevisitSecs;
  bool RedirDmAllowedP;
  TStrV StartUrlStrV;
  TStrV RqDmNmV;
  TStrV BadDmNmV;
  PGeoIpBs GeoIpBs;
  TStrV RqCountryNmV;
  TStrV RqContTypeNmV;
  TStrV BadFExtV;
  TStrH BadFExtH;
  TStr UserAgentStr;
  UndefCopyAssign(TCrawlerDef);
public:
  TCrawlerDef():
    MxUrls(100000), MnConns(1), MxConns(10), MxConnsPerSrv(1),
    AfterFetchDelaySecs(1), MxLev(3), MxDep(3), MxContLen(100000),
    MnQLen(1000), QResetMod(1000), MxQSegLen(1000000),
    MxRetries(2), RevisitSecs(-1), RedirDmAllowedP(false),
    StartUrlStrV(), RqDmNmV(), BadDmNmV(), GeoIpBs(), RqCountryNmV(),
    RqContTypeNmV(), BadFExtV(), BadFExtH(), UserAgentStr(){}
  static PCrawlerDef New(){
    return PCrawlerDef(new TCrawlerDef());}

  // max. urls
  void PutMxUrls(const int& _MxUrls){MxUrls=_MxUrls;}
  int GetMxUrls() const {return MxUrls;}

  // min. connections
  void PutMnConns(const int& _MnConns){MnConns=_MnConns;}
  int GetMnConns() const {return MnConns;}

  // max. connections
  void PutMxConns(const int& _MxConns){MxConns=_MxConns;}
  int GetMxConns() const {return MxConns;}

  // max. connections-per-server
  void PutMxConnsPerSrv(const int& _MxConnsPerSrv){
    MxConnsPerSrv=_MxConnsPerSrv;}
  int GetMxConnsPerSrv() const {return MxConnsPerSrv;}

  // after-fetch-delay
  void PutAfterFetchDelaySecs(const int& _AfterFetchDelaySecs){
    AfterFetchDelaySecs=_AfterFetchDelaySecs;}
  int GetAfterFetchDelaySecs() const {return AfterFetchDelaySecs;}

  // max. level
  void PutMxLev(const int& _MxLev){MxLev=_MxLev;}
  int GetMxLev() const {return MxLev;}

  // max. depth
  void PutMxDep(const int& _MxDep){MxDep=_MxDep;}
  int GetMxDep() const {return MxDep;}

  // max. content-lenght
  void PutMxContLen(const int& _MxContLen){MxContLen=_MxContLen;}
  int GetMxContLen() const {return MxContLen;}

  // min. queue-length
  void PutMnQLen(const int& _MnQLen){MnQLen=_MnQLen;}
  int GetMnQLen() const {return MnQLen;}

  // queue reset modulo
  void PutQResetMod(const int& _QResetMod){QResetMod=_QResetMod;}
  int GetQResetMod() const {return QResetMod;}

  // max. queue-segment-length
  void PutMxQSegLen(const int& _MxQSegLen){MxQSegLen=_MxQSegLen;}
  int GetMxQSegLen() const {return MxQSegLen;}

  // max. retries
  void PutMxRetries(const int& _MxRetries){MxRetries=_MxRetries;}
  int GetMxRetries() const {return MxRetries;}

  // revisit-time
  void PutRevisitSecs(const int& _RevisitSecs){RevisitSecs=_RevisitSecs;}
  int GetRevisitSecs() const {return RevisitSecs;}

  // redirection-domains-allowed
  void PutRedirDmAllowed(const bool& _RedirDmAllowedP){RedirDmAllowedP=_RedirDmAllowedP;}
  bool IsRedirDmAllowed() const {return RedirDmAllowedP;}

  // start urls
  void AddStartUrlStr(const TStr& UrlStr){
    PUrl Url=TUrl::New(UrlStr);
    EAssertRA(Url->IsOk(usHttp), "Url must have HTTP schema", UrlStr);
    StartUrlStrV.Add(UrlStr);}
  void AddStartUrlStrV(const TStrV& UrlStrV){
    for (int UrlStrN=0; UrlStrN<UrlStrV.Len(); UrlStrN++){AddStartUrlStr(UrlStrV[UrlStrN]);}}
  int GetStartUrls() const {
    return StartUrlStrV.Len();}
  TStr GetStartUrl(const int& StartUrlN) const {
    return StartUrlStrV[StartUrlN];}

  // required & bad domain-names
  void AddRqDmNm(const TStr& DmNm){RqDmNmV.AddUnique(DmNm);}
  void AddRqDmNmV(const TStrV& DmNmV){
    for (int DmNmN=0; DmNmN<DmNmV.Len(); DmNmN++){AddRqDmNm(DmNmV[DmNmN]);}}
  void AddBadDmNm(const TStr& DmNm){BadDmNmV.AddUnique(DmNm);}
  void AddBadDmNmV(const TStrV& DmNmV){
    for (int DmNmN=0; DmNmN<DmNmV.Len(); DmNmN++){AddBadDmNm(DmNmV[DmNmN]);}}
  bool IsHostNmOk(const TStr& DmNm);

  // geoip
  void LoadGeoIpBs(const TStr& GeoIpBsFNm){GeoIpBs=TGeoIpBs::LoadBin(GeoIpBsFNm);}
  PGeoIpBs GetGeoIpBs() const {return GeoIpBs;}
  bool IsGeoIpBs() const {return GetGeoIpBs().Empty();}

  // required-countries
  void AddRqCountryNm(const TStr& CountryNm){RqCountryNmV.Add(CountryNm);}
  void AddRqCountryNmV(const TStrV& CountryNmV){
    for (int CountryNmN=0; CountryNmN<CountryNmV.Len(); CountryNmN++){
      AddRqCountryNm(CountryNmV[CountryNmN]);}}

  // required content-types
  void AddRqContTypeNm(const TStr& ContTypeNm){RqContTypeNmV.Add(ContTypeNm);}
  void AddRqContTypeNmV(const TStrV& ContTypeNmV){
    for (int ContTypeNmN=0; ContTypeNmN<ContTypeNmV.Len(); ContTypeNmN++){
      AddRqContTypeNm(ContTypeNmV[ContTypeNmN]);}}
  bool IsContTypeNmOk(const TStr& ContTypeNm);

  // required file-extensions
  void AddBadFExt(const TStr& FExt){
    BadFExtV.Add(FExt.GetUc()); BadFExtH.AddKey(FExt.GetUc());}
  void AddBadFExtV(const TStrV& FExtV){
    for (int FExtN=0; FExtN<FExtV.Len(); FExtN++){AddBadFExt(FExtV[FExtN]);}}
  bool IsFExtOk(const PUrl& Url) const;

  // user-agent
  void PutUserAgentStr(const TStr& _UserAgentStr){UserAgentStr=_UserAgentStr;}
  TStr GetUserAgentStr() const {return UserAgentStr;}
  
  // optimize crawling
  void OptHtmlCrawling();

  // URLs of sites to crawl
  void AddSiteUrlStr(const TStr& SiteUrlStr);
  void AddSiteUrlStrV(const TStrV& SiteUrlStrV);
  void LoadUrlStrV(const TStr& SiteUrlStrVFNm);

  // string
  TStr GetStr();
};

/////////////////////////////////////////////////
// Crawler-Url-Desc
class TCrawlerUrlDesc{
public:
  TBlobPt DocBlobPt;
  TMd5Sig DocMemMd5Sig;
  TSecTm DocSecTm;
  TUCh DocLev;
  TUCh DocDep;
public:
  TCrawlerUrlDesc():
    DocBlobPt(), DocMemMd5Sig(), DocSecTm(), DocLev(), DocDep(){}
  TCrawlerUrlDesc(
   const TBlobPt& _DocBlobPt, const TMd5Sig& _DocMemMd5Sig,
   const TSecTm& _DocSecTm, const TUCh& _DocLev, const TUCh& _DocDep):
    DocBlobPt(_DocBlobPt), DocMemMd5Sig(_DocMemMd5Sig),
    DocSecTm(_DocSecTm), DocLev(_DocLev), DocDep(_DocDep){}
  TCrawlerUrlDesc(TSIn& SIn):
    DocBlobPt(SIn), DocMemMd5Sig(SIn), DocSecTm(SIn), DocLev(SIn), DocDep(SIn){}
  void Save(TSOut& SOut) const {
    DocBlobPt.Save(SOut); DocMemMd5Sig.Save(SOut);
    DocSecTm.Save(SOut); DocLev.Save(SOut); DocDep.Save(SOut);}

  TCrawlerUrlDesc& operator=(const TCrawlerUrlDesc& UrlDesc){
    DocBlobPt=UrlDesc.DocBlobPt; DocMemMd5Sig=UrlDesc.DocMemMd5Sig;
    DocSecTm=UrlDesc.DocSecTm; DocLev=UrlDesc.DocLev; DocDep=UrlDesc.DocDep;
    return *this;}
};

/////////////////////////////////////////////////
// Crawler-Url-Descriptor-Base
ClassTP(TCrawlerUrlDescBs, PCrawlerUrlDescBs)//{
private:
  TCrawler* Crawler;
  typedef TPair<TInt, TBlobPt> TIntBlobPtPr;
  THash<TMd5Sig, TCrawlerUrlDesc> UrlMd5SigToDescH;
  UndefDefaultCopyAssign(TCrawlerUrlDescBs);
public:
  TCrawlerUrlDescBs(TCrawler* _Crawler);
  static PCrawlerUrlDescBs New(TCrawler* Crawler){
    return new TCrawlerUrlDescBs(Crawler);}

  int GetUrls() const {return UrlMd5SigToDescH.Len();}
  bool IsUrl(const TMd5Sig& UrlMd5Sig) const {
    return UrlMd5SigToDescH.IsKey(UrlMd5Sig);}
  void AddUrlDesc(const TMd5Sig& UrlMd5Sig, const TCrawlerUrlDesc& UrlDesc){
    UrlMd5SigToDescH.AddDat(UrlMd5Sig, UrlDesc);}
  TCrawlerUrlDesc& GetUrlDesc(const TMd5Sig& UrlMd5Sig){
    return UrlMd5SigToDescH.GetDat(UrlMd5Sig);}

  // files
  TStr GetFNm() const;
  void Load();
  void Save();
};

/////////////////////////////////////////////////
// Crawler-Host
ClassTPV(TCrawlerHost, PCrawlerHost, TCrawlerHostV)//{
private:
  TStr HostNm, RevHostNm;
  TStr IpNum;
  int ActiveConns;
  int FetchedUrls;
  int FetchErrors;
  int QueueUrls;
  uint64 TransferBytes;
  int TransferMSecs;
  TSecTm LastFetchTm;
  UndefDefaultCopyAssign(TCrawlerHost);
public:
  TCrawlerHost(const TStr& _HostNm):
    HostNm(_HostNm), RevHostNm(_HostNm.Reverse()), IpNum(),
    ActiveConns(0), FetchedUrls(0), FetchErrors(0),
    QueueUrls(0), TransferBytes(0), TransferMSecs(0), LastFetchTm(0){}
  static PCrawlerHost New(const TStr& HostNm){
    return new TCrawlerHost(HostNm);}

  TStr GetHostNm() const {return HostNm;}
  TStr GetRevHostNm() const {return RevHostNm;}
  bool IsIpNum() const {return !IpNum.Empty();}
  TStr& GetIpNum(){return IpNum;}
  int& GetActiveConns(){return ActiveConns;}
  int& GetFetchedUrls(){return FetchedUrls;}
  int& GetFetchErrors(){return FetchErrors;}
  int& GetQueueUrls(){return QueueUrls;}
  uint64& GetTransferBytes(){return TransferBytes;}
  int& GetTransferMSecs(){return TransferMSecs;}
  double GetAvgHttpRespLen(){
    return FetchedUrls==0 ? 0 : TransferBytes/double(FetchedUrls);}
  double GetAvgTransferBps(){
    return TransferMSecs==0 ? 0 : (TransferBytes*8.0)/(TransferMSecs/1000.0);}
  TSecTm& GetLastFetchTm(){return LastFetchTm;}
};

/////////////////////////////////////////////////
// Crawler-Host-Base
ClassTP(TCrawlerHostBs, PCrawlerHostBs)//{
private:
  TCrawler* Crawler;
  THash<TStr, PCrawlerHost> NmToHostH;
  UndefDefaultCopyAssign(TCrawlerHostBs);
public:
  TCrawlerHostBs(TCrawler* _Crawler);
  static PCrawlerHostBs New(TCrawler* Crawler){
    return new TCrawlerHostBs(Crawler);}

  // hosts-base
  int GetHosts() const {return NmToHostH.Len();}
  bool IsHost(const TStr& HostNm) const {
    return NmToHostH.IsKey(HostNm);}
  PCrawlerHost AddGetHost(const TStr& HostNm);
  PCrawlerHost GetHost(const TStr& HostNm){
    return NmToHostH.GetDat(HostNm);}
  PCrawlerHost GetHost(const int& HostN){
    return NmToHostH[HostN];}

  // sorted
  void GetSortedHostV(TCrawlerHostV& HostV, const TStr& SortOrderNm);

  // summary
  void GetSummaryInfo(
   int& ActiveConns, int& FetchedUrls, int& FetchErrors, int& QueueUrls,
   double& TransferBytes, double& TransferMSecs, 
   double& AvgHttpRespLen, double& AvgTransferBps);
};

/////////////////////////////////////////////////
// Crawler-Queue
ClassTP(TCrawlerQueue, PCrawlerQueue)//{
private:
  TCrawler* Crawler;
  THash<TMd5Sig, TStrStrIntIntQu> UrlMd5SigToUrlStrHostNmLevDepQuH;
  TVec<TIntQ> LevUrlIdQV;
  TStr ToDelUrlQBBsFNm;
  PBlobBs UrlQBBs;
  UndefDefaultCopyAssign(TCrawlerQueue);
public:
  TCrawlerQueue(TCrawler* _Crawler);
  static PCrawlerQueue New(TCrawler* Crawler){
    return new TCrawlerQueue(Crawler);}

  // url-queue-database
  int GetQLevs() const {return LevUrlIdQV.Len();}
  int GetQLevUrls(const int& Lev) const {return LevUrlIdQV[Lev].Len();}
  int GetQUrls() const {return UrlMd5SigToUrlStrHostNmLevDepQuH.Len();}
  int GetUnconnQUrls() const {
    int QUrls=0; int Levs=GetQLevs();
    for (int Lev=0; Lev<Levs; Lev++){QUrls+=GetQLevUrls(Lev);}
    return QUrls;}
  bool Empty() const {return GetQUrls()==0;}
  void GetQUrlsHostPrV(TIntStrPrV& QUrlsHostPrV);
  bool IsQUrl(const TMd5Sig& UrlMd5Sig) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH.IsKey(UrlMd5Sig);}
  int AddQUrl(const TMd5Sig& UrlMd5Sig, const TStr& UrlStr, const TStr& HostNm, const int& Lev, const int& Dep){
    int QUrlId=UrlMd5SigToUrlStrHostNmLevDepQuH.AddKey(UrlMd5Sig);
    UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId]=TStrStrIntIntQu(UrlStr, HostNm, Lev, Dep);
    return QUrlId;}
  void DelQUrl(const int& QUrlId){
    UrlMd5SigToUrlStrHostNmLevDepQuH.DelKeyId(QUrlId);}
  TStr GetQUrlStr(const TMd5Sig& UrlMd5Sig) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH.GetDat(UrlMd5Sig).Val1;}
  TStr GetQUrlStr(const int& QUrlId) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId].Val1;}
  TStr GetQHostNm(const int& QUrlId) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId].Val2;}
  int GetQUrlLev(const TMd5Sig& UrlMd5Sig) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH.GetDat(UrlMd5Sig).Val3;}
  int GetQUrlLev(const int& QUrlId) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId].Val3;}
  int GetQUrlDep(const TMd5Sig& UrlMd5Sig) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH.GetDat(UrlMd5Sig).Val4;}
  int GetQUrlDep(const int& QUrlId) const {
    return UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId].Val4;}
  void GetQUrl(const int& QUrlId, TMd5Sig& UrlMd5Sig, int& Lev, int& Dep){
    UrlMd5Sig=UrlMd5SigToUrlStrHostNmLevDepQuH.GetKey(QUrlId);
    Lev=UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId].Val3;
    Dep=UrlMd5SigToUrlStrHostNmLevDepQuH[QUrlId].Val4;}

  // url-queue
  void PushQUrl(const TStr& UrlStr, const int& Lev, const int& Dep);
  void PushQUrl(const PUrl& Url, const int& Lev, const int& Dep);
  bool PopQUrl(int& QUrlId, TStr& UrlStr);
  void ShuffleUrlQ();
  void ResetQUrlBs();

  // url-queue-base
  TStr GetUrlQBBsFNm() const;
  TStr GetUrlQBBsWcFNm() const;
  void PushQUrlBs();
  void PushQUrlToUrlQBs(const PUrl& Url, const int& Lev, const int& Dep);
  void PushQUrlToUrlQBs(const TStr& UrlStr, const int& Lev, const int& Dep);
  void PopSaveQUrlBs();
};

/////////////////////////////////////////////////
// Crawler-Fetcher
ClassTPE(TCrawlerFetcher, PCrawlerFetcher, TWebPgFetch)//{
private:
  TCrawler* Crawler;
  typedef TTriple<TInt, PUrl, TUInt64> TQUrlIdUrlTmTr;
  THash<TInt, TQUrlIdUrlTmTr> ConnIdToQUrlIdUrlTmTrH;
  int CurMxConns;
  TTm LastClrZombiesTm;
  int Zombies;
  TIntIntH ZombieConnIdToFetchSizeH;
  UndefDefaultCopyAssign(TCrawlerFetcher);
  void DelCrawlerFetcherConn(const int& ConnId);
public:
  TCrawlerFetcher(TCrawler* _Crawler);
  static PCrawlerFetcher New(TCrawler* Crawler){
    return new TCrawlerFetcher(Crawler);}

  // connection-id to queue-url-id
  void AddConn(const int& ConnId, const int& QUrlId, const PUrl& Url){
    ConnIdToQUrlIdUrlTmTrH.AddDat(ConnId, TQUrlIdUrlTmTr(QUrlId, Url, TTm::GetCurUniMSecs()));}
  void GetDelConn(const int& ConnId, int& QUrlId, PUrl& Url, int& MSecs){
    QUrlId=ConnIdToQUrlIdUrlTmTrH.GetDat(ConnId).Val1;
    Url=ConnIdToQUrlIdUrlTmTrH.GetDat(ConnId).Val2;
    MSecs=int(TTm::GetCurUniMSecs()-ConnIdToQUrlIdUrlTmTrH.GetDat(ConnId).Val3);
    ConnIdToQUrlIdUrlTmTrH.DelKey(ConnId);}
  int GetConns() const {return ConnIdToQUrlIdUrlTmTrH.Len();}
  void GetConnV(TIntStrIntIntQuV& ConnIdUrlStrMSecsSizeQuV);

  // current-max-connections
  void PutCurMxConns(const int& _CurMxConns){CurMxConns=_CurMxConns;}
  int GetCurMxConns() const {return CurMxConns;}

  // zombies
  void ClrZombies();
  int GetZombies() const {return Zombies;}

  // fetching
  void Fetch();
  void OnFetch(const int& /*ConnId*/, const PWebPg& WebPg);
  void OnError(const int& /*ConnId*/, const TStr& MsgStr);
};

/////////////////////////////////////////////////
// Crawler-Document-Base
ClassTP(TCrawlerDocBs, PCrawlerDocBs)//{
private:
  TCrawler* Crawler;
  TStr DocBsNm;
  TStr DocBsNrFPath;
  TStr DocBsFAccessNm;
  int DocBsMxSegLen;
  PBlobBs DocBBs;
  UndefDefaultCopyAssign(TCrawlerDocBs);
public:
  TCrawlerDocBs(TCrawler* _Crawler):
    Crawler(_Crawler),
    DocBsNm(), DocBsNrFPath(), DocBsFAccessNm(), DocBBs(){}
  static PCrawlerDocBs New(TCrawler* Crawler){
    return new TCrawlerDocBs(Crawler);}
  virtual ~TCrawlerDocBs(){}

  // set & get components
  void PutDocBsNm(const TStr& _DocBsNm){DocBsNm=_DocBsNm;}
  TStr GetDocBsNm() const {return DocBsNm;}
  void PutDocBsNrFPath(const TStr& _DocBsNrFPath){DocBsNrFPath=_DocBsNrFPath;}
  TStr GetDocBsNrFPath() const {return DocBsNrFPath;}
  void PutDocBsFAccessNm(const TStr& _DocBsFAccessNm){DocBsFAccessNm=_DocBsFAccessNm;}
  TStr GetDocBsFAccessNm() const {return DocBsFAccessNm;}
  void PutDocBsMxSegLen(const int& _DocBsMxSegLen){DocBsMxSegLen=_DocBsMxSegLen;}
  int GetDocBsMxSegLen() const {return DocBsMxSegLen;}
  PBlobBs GetDocBBs() const {return DocBBs;}

  // actions
  TFAccess Open();
  void Close();
  TBlobPt AddDoc(const TStr& DocUrlStr, const TMem& DocMem);
  TBlobPt FFirstDocBlobPt(){return DocBBs->FFirstBlobPt();}
  bool FNextDocBlobPt(TBlobPt& TrvDocBlobPt,
   TBlobPt& DocBlobPt, TStr& DateTimeStr, TStr& DocUrlStr, TMem& DocMem);
};

//////////////////////////////////////////////////
// Crawler-Timer
class TCrawlerTimer: public TTTimer{
private:
  TCrawler* Crawler;
  uint64 LastEventMSecs;
  uint64 Last100SecsEventMSecs;
  uint64 Last1000SecsEventMSecs;
  UndefDefaultCopyAssign(TCrawlerTimer);
public:
  TCrawlerTimer(TCrawler* _Crawler, const int& TimerTout):
    TTTimer(TimerTout), Crawler(_Crawler), LastEventMSecs(0),
    Last100SecsEventMSecs(0), Last1000SecsEventMSecs(0){}
  static PTimer New(TCrawler* Crawler, const int& TimerTout){
    return new TCrawlerTimer(Crawler, TimerTout);}

  void OnTimeOut();
};

/////////////////////////////////////////////////
// Crawler-Web-Server
ClassTPE(TCrawlerWebSrv, PCrawlerWebSrv, TWebSrv)//{
private:
  TCrawler* Crawler;
  UndefCopyAssign(TCrawlerWebSrv);
public:
  TCrawlerWebSrv(TCrawler* _Crawler, const int& PortN):
    TWebSrv(PortN, false), Crawler(_Crawler){}
  static PCrawlerWebSrv New(TCrawler* Crawler, const int& PortN=7777){
    return new TCrawlerWebSrv(Crawler, PortN);}
  virtual ~TCrawlerWebSrv(){}

  // url & anchor
  TStr GetUrlStr(const int& RefreshSecs, const TStr& SortOrderNm);
  TStr GetAnchorStr(const TStr& DescStr, 
   const int& RefreshSecs, const TStr& SortOrderNm);

  // http request
  void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
};

/////////////////////////////////////////////////
// Crawler-Statistics
ClassTP(TCrawlerStat, PCrawlerStat)//{
private:
  TCrawler* Crawler;
  uint64 Last100SecsEventMSecs;
  uint64 Last1000SecsEventMSecs;
  uint64 Last100SecsFetchedUrls;
  uint64 Last1000SecsFetchedUrls;
  double Last100SecsCrawlingSpeed;
  double Last1000SecsCrawlingSpeed;
  UndefDefaultCopyAssign(TCrawlerStat);
public:
  TCrawlerStat(TCrawler* _Crawler):
    Crawler(_Crawler),
    Last100SecsEventMSecs(0), Last1000SecsEventMSecs(0),
    Last100SecsFetchedUrls(0), Last1000SecsFetchedUrls(0),
    Last100SecsCrawlingSpeed(0), Last1000SecsCrawlingSpeed(0){}
  static PCrawlerStat New(TCrawler* Crawler){
    return new TCrawlerStat(Crawler);}
  ~TCrawlerStat(){}

  // timer events
  void On100Secs();
  void On1000Secs();

  // retrieve statistics
  double GetLast100SecsCrawlingSpeed(){
    return Last100SecsCrawlingSpeed;}
  double GetLast1000SecsCrawlingSpeed(){
    return Last1000SecsCrawlingSpeed;}
};

/////////////////////////////////////////////////
// Crawler
ClassTP(TCrawler, PCrawler)//{
private:
  PCrawlerDef Def;
  PCrawlerUrlDescBs UrlDescBs;
  PCrawlerHostBs HostBs;
  PCrawlerQueue Queue;
  PCrawlerFetcher Fetcher;
  PCrawlerDocBs DocBs;
  PCrawlerStat Stat;
  PTimer Timer;
  PCrawlerWebSrv WebSrv;
  TTm StartTm;
  UndefCopyAssign(TCrawler);
public:
  TCrawler();
  static PCrawler New(){
    return PCrawler(new TCrawler());}
  ~TCrawler(){}
  TCrawler(TSIn&){Fail;}
  static PCrawler Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  // components
  PCrawlerDef GetDef() const {return Def;}
  PCrawlerUrlDescBs GetUrlDescBs() const {return UrlDescBs;}
  PCrawlerHostBs GetHostBs() const {return HostBs;}
  PCrawlerQueue GetQueue() const {return Queue;}
  PCrawlerFetcher GetFetcher() const {return Fetcher;}
  PCrawlerDocBs GetDocBs() const {return DocBs;}
  PCrawlerStat GetStat() const {return Stat;}
  PTimer GetTimer() const {return Timer;}
  PCrawlerWebSrv GetWebSrv() const {return WebSrv;}
  TTm GetStartTm() const {return StartTm;}

  // crawl-base
  void DefCrawlBs(const TStr& CrawlBsNm, const TStr& CrawlBsFPath,
   const TStr& CrawlBsFAccessNm, const int& CrawlBsMxSegLen);

  // crawling
  void StartCrawling();
  void StopCrawling();

  // url normalization
  static TMd5Sig GetUrlMd5Sig(const TStr& UrlStr);
};

typedef PBlobBs PCrawlBs;

#endif
