/////////////////////////////////////////////////
// Includes
#include "crawler.h"

/////////////////////////////////////////////////
// Crawler-Def
bool TCrawlerDef::IsHostNmOk(const TStr& HostNm){
  bool HostNmOkP=false;
  // check requested-domains
  if (RqDmNmV.Empty()){
    HostNmOkP=true;
  } else {
    for (int RqDmNmN=0; RqDmNmN<RqDmNmV.Len(); RqDmNmN++){
      if (HostNm.IsSuffix(RqDmNmV[RqDmNmN])){
        HostNmOkP=true; break;
      }
    }
  }
  // check bad-domains
  if (HostNmOkP){
    for (int BadDmNmN=0; BadDmNmN<BadDmNmV.Len(); BadDmNmN++){
      if (HostNm.IsSuffix(BadDmNmV[BadDmNmN])){
        HostNmOkP=false; break;
      }
    }
  }
  return HostNmOkP;
}

bool TCrawlerDef::IsContTypeNmOk(const TStr& ContTypeNm){
  if (RqContTypeNmV.Empty()){
    return true;
  } else {
    TStr LcContTypeNm=ContTypeNm.GetLc();
    for (int ContTypeNmN=0; ContTypeNmN<RqContTypeNmV.Len(); ContTypeNmN++){
      if (LcContTypeNm.IsWcMatch(RqContTypeNmV[ContTypeNmN])){return true;}
    }
    return false;
  }
}

bool TCrawlerDef::IsFExtOk(const PUrl& Url) const {
  if (BadFExtV.Empty()||(Url->GetPathSegs()==0)){
    return true;
  } else {
    TStr UrlStr=Url->GetUrlStr();
    TStr FBase=Url->GetPathSeg(Url->GetPathSegs()-1);
    TStr FExt=FBase.GetFExt().GetUc();
    return (!BadFExtH.IsKey(FExt));
  }
}

void TCrawlerDef::OptHtmlCrawling(){
  // image formats
  AddBadFExt(".JPEG"); AddBadFExt(".JPG"); AddBadFExt(".GIF"); AddBadFExt(".BMP");
  // video formats
  AddBadFExt(".MPEG"); AddBadFExt(".MPG"); AddBadFExt(".AVI"); AddBadFExt(".MOV");
  // audio formats
  AddBadFExt(".MP3"); AddBadFExt(".WAV"); AddBadFExt(".MID"); AddBadFExt(".RA");
  // archive formats
  AddBadFExt(".ZIP"); AddBadFExt(".RAR"); AddBadFExt(".GZ"); AddBadFExt(".TAR");
  // document formats
  AddBadFExt(".PDF"); AddBadFExt(".DOC"); AddBadFExt(".XSL"); AddBadFExt(".PPT");
  // restrict content-type
  AddRqContTypeNm("text/*");
}

void TCrawlerDef::AddSiteUrlStr(const TStr& SiteUrlStr){
  PUrl Url=TUrl::New(SiteUrlStr);
  if (Url->IsOk(usHttp)){
    AddRqDmNm(Url->GetHostNm());
    AddStartUrlStr(SiteUrlStr);
  }
}

void TCrawlerDef::AddSiteUrlStrV(const TStrV& SiteUrlStrV){
  for (int UrlStrN=0; UrlStrN<SiteUrlStrV.Len(); UrlStrN++){
    AddSiteUrlStr(SiteUrlStrV[UrlStrN]);
  }
}

void TCrawlerDef::LoadUrlStrV(const TStr& SiteUrlStrVFNm){
  if (SiteUrlStrVFNm.Empty()){return;}
  PSIn SIn=TFIn::New(SiteUrlStrVFNm);
  TILx Lx(SIn);
  while (Lx.GetSym(syLn, syEof)!=syEof){
    TStr UrlStr=TStr(Lx.Str).GetTrunc();
    AddSiteUrlStr(UrlStr);
  }
}

TStr TCrawlerDef::GetStr(){
  TChA ChA;
  ChA+=TStr::Fmt("                  Max. Urls: %d\n", GetMxUrls());
  ChA+=TStr::Fmt("           Min. Connections: %d\n", GetMnConns());
  ChA+=TStr::Fmt("           Max. Connections: %d\n", GetMxConns());
  ChA+=TStr::Fmt("Max. Connections Per Server: %d\n", GetMxConnsPerSrv());
  ChA+=TStr::Fmt("          After Fetch Delay: %d\n", GetAfterFetchDelaySecs());
  ChA+=TStr::Fmt("                 Max. Level: %d\n", GetMxLev());
  ChA+=TStr::Fmt("                 Max. Depth: %d\n", GetMxDep());
  ChA+=TStr::Fmt("        Max. Content Length: %d\n", GetMxContLen());
  ChA+=TStr::Fmt("          Min. Queue Length: %d\n", GetMnQLen());
  ChA+=TStr::Fmt("         Queue Reset Modulo: %d\n", GetQResetMod());
  ChA+=TStr::Fmt("  Max. Queue Segment Length: %d\n", GetMxQSegLen());
  ChA+=TStr::Fmt("               Max. Retries: %d\n", GetMxRetries());
  ChA+=TStr::Fmt("           Revisits Seconds: %d\n", GetRevisitSecs());
  ChA+=TStr::Fmt("Redirection Domains Allowed: %d\n", TBool::GetYesNoStr(IsRedirDmAllowed()));
  ChA+=TStr::Fmt("                 Start Urls:");
  for (int UrlN=0; UrlN<GetStartUrls(); UrlN++){
    ChA+=" "; ChA+=GetStartUrl(UrlN);}
  ChA+="\n";
  ChA+=TStr::Fmt("           Required Domains:");
  for (int DmNmN=0; DmNmN<RqDmNmV.Len(); DmNmN++){
    ChA+=" "; ChA+=RqDmNmV[DmNmN];}
  ChA+="\n";
  ChA+=TStr::Fmt("                Bad Domains:");
  for (int DmNmN=0; DmNmN<BadDmNmV.Len(); DmNmN++){
    ChA+=" "; ChA+=BadDmNmV[DmNmN];}
  ChA+="\n";
  ChA+=TStr::Fmt("                Geo-IP Base: %s\n", TBool::GetYesNoStr(IsGeoIpBs()).CStr());
  ChA+=TStr::Fmt("                  Countries:");
  for (int CountryNmN=0; CountryNmN<RqCountryNmV.Len(); CountryNmN++){
    ChA+=" "; ChA+=RqCountryNmV[CountryNmN];}
  ChA+="\n";
  ChA+=TStr::Fmt("              Content Types:");
  for (int ContTypeNmN=0; ContTypeNmN<RqContTypeNmV.Len(); ContTypeNmN++){
    ChA+=" "; ChA+=RqContTypeNmV[ContTypeNmN];}
  ChA+="\n";
  ChA+=TStr::Fmt("        Bad File Extensions:");
  for (int FExtN=0; FExtN<BadFExtV.Len(); FExtN++){
    ChA+=" "; ChA+=BadFExtV[FExtN];}
  ChA+="\n";

  return ChA;
}

/////////////////////////////////////////////////
// Crawler-Url-Base
TCrawlerUrlDescBs::TCrawlerUrlDescBs(TCrawler* _Crawler):
  Crawler(_Crawler){
  // set max. urls
  IAssert(Crawler->GetDef()->GetMxUrls()!=-1);
  UrlMd5SigToDescH.Gen(Crawler->GetDef()->GetMxUrls());
}

TStr TCrawlerUrlDescBs::GetFNm() const {
  return
   Crawler->GetDocBs()->GetDocBsNrFPath()+
   Crawler->GetDocBs()->GetDocBsNm()+".UrlBs";
}

void TCrawlerUrlDescBs::Load(){
  TFIn SIn(GetFNm()); UrlMd5SigToDescH.Load(SIn);
}

void TCrawlerUrlDescBs::Save(){
  TFOut SOut(GetFNm()); UrlMd5SigToDescH.Save(SOut);
}

/////////////////////////////////////////////////
// Crawler-Host-Base
TCrawlerHostBs::TCrawlerHostBs(TCrawler* _Crawler):
  Crawler(_Crawler){
}

PCrawlerHost TCrawlerHostBs::AddGetHost(const TStr& HostNm){
  int HostId;
  if (!NmToHostH.IsKey(HostNm, HostId)){
    HostId=NmToHostH.AddKey(HostNm);
    NmToHostH[HostId]=TCrawlerHost::New(HostNm);
  }
  return NmToHostH[HostId];
}

void TCrawlerHostBs::GetSortedHostV(TCrawlerHostV& HostV, const TStr& SortOrderNm){
  typedef TKeyDat<TStr, PCrawlerHost> TNmHostKd;
  TVec<TNmHostKd> NmHostKdV;
  // collect hosts info
  int Hosts=GetHosts(); bool SortAscP=true;
  for (int HostN=0; HostN<Hosts; HostN++){
    // get data
    PCrawlerHost Host=GetHost(HostN);
    // get key
    TStr KeyStr;
    if (SortOrderNm=="Host"){KeyStr=Host->GetRevHostNm(); SortAscP=true;}
    else if (SortOrderNm=="Active"){KeyStr=TInt::GetStr(Host->GetActiveConns(), "%012d"); SortAscP=false;}
    else if (SortOrderNm=="Fetched"){KeyStr=TInt::GetStr(Host->GetFetchedUrls(), "%012d"); SortAscP=false;}
    else if (SortOrderNm=="Errors"){KeyStr=TInt::GetStr(Host->GetFetchErrors(), "%012d"); SortAscP=false;}
    else if (SortOrderNm=="Queue"){KeyStr=TInt::GetStr(Host->GetQueueUrls(), "%012d"); SortAscP=false;}
    else if (SortOrderNm=="Bytes"){KeyStr=TFlt::GetStr(double(Host->GetTransferBytes()), "%012.0f"); SortAscP=false;}
    else if (SortOrderNm=="Time"){KeyStr=TFlt::GetStr(Host->GetTransferMSecs(), "%012.0f"); SortAscP=false;}
    else if (SortOrderNm=="PageLen"){KeyStr=TFlt::GetStr(Host->GetAvgHttpRespLen(), "%012.0f"); SortAscP=false;}
    else if (SortOrderNm=="Speed"){KeyStr=TFlt::GetStr(Host->GetAvgTransferBps(), "%012.0f"); SortAscP=false;}
    else {KeyStr=Host->GetRevHostNm(); SortAscP=true;}
    // create pair
    NmHostKdV.Add(TNmHostKd(KeyStr, Host));
  }
  // sort key-data pairs
  NmHostKdV.Sort(SortAscP);
  // create host list
  HostV.Gen(NmHostKdV.Len(), 0);
  for (int HostN=0; HostN<NmHostKdV.Len(); HostN++){
    HostV.Add(NmHostKdV[HostN].Dat);
  }
}

void TCrawlerHostBs::GetSummaryInfo(
 int& ActiveConns, int& FetchedUrls, int& FetchErrors, int& QueueUrls, 
 double& TransferBytes, double& TransferMSecs, double& AvgHttpRespLen, 
 double& AvgTransferBps){
  // prepare counters
  ActiveConns=0; FetchedUrls=0; FetchErrors=0; QueueUrls=0;
  TransferBytes=0; TransferMSecs=0; 
  AvgHttpRespLen=0; AvgTransferBps=0;
  // traverse hosts
  int Hosts=GetHosts();
  for (int HostN=0; HostN<Hosts; HostN++){
    PCrawlerHost Host=GetHost(HostN);
    ActiveConns+=Host->GetActiveConns();
    FetchedUrls+=Host->GetFetchedUrls();
    FetchErrors+=Host->GetFetchErrors();
    QueueUrls+=Host->GetQueueUrls();
    TransferBytes+=Host->GetTransferBytes();
    TransferMSecs+=Host->GetTransferMSecs();
    AvgHttpRespLen+=Host->GetAvgHttpRespLen();
    AvgTransferBps+=Host->GetAvgTransferBps();
  }
  if (Hosts>0){
    AvgHttpRespLen=AvgHttpRespLen/Hosts;
    AvgTransferBps=AvgTransferBps/Hosts;
  }
}

/////////////////////////////////////////////////
// Crawler-Queue
TCrawlerQueue::TCrawlerQueue(TCrawler* _Crawler):
  Crawler(_Crawler), UrlMd5SigToUrlStrHostNmLevDepQuH(){
}

void TCrawlerQueue::GetQUrlsHostPrV(TIntStrPrV& QUrlsHostPrV){
  QUrlsHostPrV.Clr();
  TStrIntH HostToQUrlsH;
  int QUrlP=UrlMd5SigToUrlStrHostNmLevDepQuH.FFirstKeyId();
  while (UrlMd5SigToUrlStrHostNmLevDepQuH.FNextKeyId(QUrlP)){
    TStr HostNm=GetQHostNm(QUrlP);
    int Lev=GetQUrlLev(QUrlP);
    HostToQUrlsH.AddDat(HostNm+":"+TInt::GetStr(Lev))++;
  }
  HostToQUrlsH.GetDatKeyPrV(QUrlsHostPrV);
  QUrlsHostPrV.Sort(false);
}

void TCrawlerQueue::PushQUrl(const PUrl& Url, const int& Lev, const int& Dep){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerUrlDescBs UrlDescBs=Crawler->GetUrlDescBs();
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  // check url
  IAssert(Url->IsOk(usHttp));
  // get url-md5
  TMd5Sig UrlMd5Sig=TCrawler::GetUrlMd5Sig(Url->GetUrlStr());
  // checks if url is new else push in the queue
  if (UrlDescBs->IsUrl(UrlMd5Sig)){ // if url already downloaded
    if (UrlDescBs->GetUrlDesc(UrlMd5Sig).DocLev>Lev){
      UrlDescBs->GetUrlDesc(UrlMd5Sig).DocLev=Lev;}
  } else
  if (IsQUrl(UrlMd5Sig)){ // if url waits in queue
  } else
  if ((Def->GetMxLev()==-1)||(Lev<=Def->GetMxLev())&&
   ((Def->GetMxDep()==-1)||(Dep<=Def->GetMxDep()))){
    int QUrlId=AddQUrl(UrlMd5Sig, Url->GetUrlStr(), Url->GetHostNm(), Lev, Dep);
    while (LevUrlIdQV.Len()-1<Lev){LevUrlIdQV.Add();}
    LevUrlIdQV[Lev].Push(QUrlId);
    HostBs->AddGetHost(Url->GetHostNm())->GetQueueUrls()++;
  }
}

void TCrawlerQueue::PushQUrl(const TStr& UrlStr, const int& Lev, const int& Dep){
  // check url
  PUrl Url=TUrl::New(UrlStr);
  if (Url->IsOk(usHttp)){
    PushQUrl(Url, Lev, Dep);
  }
}

bool TCrawlerQueue::PopQUrl(int& QUrlId, TStr& UrlStr){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  // traverse levels
  int MxCheckedUrlsPerLev=100; 
  while (MxCheckedUrlsPerLev<=100000){
    for (int Lev=0; Lev<LevUrlIdQV.Len(); Lev++){
      int CheckedUrls=0;
      if (!LevUrlIdQV[Lev].Empty()){
        bool FirstP=true; int FirstQUrlId;
        forever {
          // get queue-url-id
          QUrlId=LevUrlIdQV[Lev].Top();
          // if non of url at current level is suitable jump to next level
          if (FirstP){FirstQUrlId=QUrlId; FirstP=false;}
          else if (QUrlId==FirstQUrlId){break;}
          // if more then max. urls per level checked, jump to the next level
          CheckedUrls++;
          if (CheckedUrls>MxCheckedUrlsPerLev){break;}
          // pop queue-url-id
          LevUrlIdQV[Lev].Pop();
          // get url
          UrlStr=GetQUrlStr(QUrlId);
          TStr HostNm=GetQHostNm(QUrlId);
          // collect host-info
          PCrawlerHost Host=HostBs->AddGetHost(HostNm);
          int ActiveConns=Host->GetActiveConns();
          int MxConnsPerSrv=Def->GetMxConnsPerSrv();
          TSecTm LastFetchTm=Host->GetLastFetchTm();
          TSecTm CurTm=TSecTm::GetCurTm();
          int AfterFetchDelaySecs=Def->GetAfterFetchDelaySecs();
		  int FetchDSecs=TInt::Abs(TSecTm::GetDSecs(LastFetchTm, CurTm));
          // if host constraints are ok return else push queue-url-id at back
          if ((ActiveConns<MxConnsPerSrv)&&(FetchDSecs>=AfterFetchDelaySecs)){
            Host->GetQueueUrls()--;
            return true;
          } else {
            LevUrlIdQV[Lev].Push(QUrlId);
          }
        }
      }
    }
    MxCheckedUrlsPerLev=MxCheckedUrlsPerLev*10;
  }
  // return no-urls-in-queue
  return false;
}

void TCrawlerQueue::ShuffleUrlQ(){
  if (Crawler->GetUrlDescBs()->GetUrls()%1000==0){
    printf("*** Shuffle Queues\n");
    for (int Lev=0; Lev<LevUrlIdQV.Len(); Lev++){
      TRnd Rnd(0); LevUrlIdQV[Lev].Shuffle(Rnd);
      printf("    Queue Level %d: %d\n", Lev, LevUrlIdQV[Lev].Len());
    }
  }
}

void TCrawlerQueue::ResetQUrlBs(){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerUrlDescBs UrlDescBs=Crawler->GetUrlDescBs();
  // reset if modulo
  if ((Def->GetQResetMod()>0)&&(UrlDescBs->GetUrls()%Def->GetQResetMod()==0)){
    PopSaveQUrlBs();
    PushQUrlBs();
  }
}

TStr TCrawlerQueue::GetUrlQBBsFNm() const {
  TStr UrlQBBsFNm=
   Crawler->GetDocBs()->GetDocBsNrFPath()+
   Crawler->GetDocBs()->GetDocBsNm()+"."+
   TTm::GetUniqueCurUniTm().GetIdStr()+".UrlQBs";
  return UrlQBBsFNm;
}

TStr TCrawlerQueue::GetUrlQBBsWcFNm() const {
  TStr UrlQBBsFNm=
   Crawler->GetDocBs()->GetDocBsNrFPath()+
   Crawler->GetDocBs()->GetDocBsNm()+".*.UrlQBs";
  return UrlQBBsFNm;
}

void TCrawlerQueue::PushQUrlBs(){
  // if no urls in queue reset queue and check if any url-queue-base
  if (Empty()){
    UrlMd5SigToUrlStrHostNmLevDepQuH.Clr(); // clear url-hash
    LevUrlIdQV.Clr(); // clear queue-vector
  }
  UrlQBBs=NULL; // close current url-queue-base
  if (!ToDelUrlQBBsFNm.Empty()){ // delete file if exists
    TFile::Del(ToDelUrlQBBsFNm); ToDelUrlQBBsFNm="";
  }
  // find url-queue-base if exists
  TFFile FFile(GetUrlQBBsWcFNm(), false); TStr UrlQBBsFNm;
  if (FFile.Next(UrlQBBsFNm)){
    PBlobBs UrlQBBs=TGBlobBs::New(UrlQBBsFNm);
    ToDelUrlQBBsFNm=UrlQBBsFNm;
    // traverse queued urls from file and push them into memory queue
    TBlobPt TrvUrlQBlobPt=UrlQBBs->FFirstBlobPt();
    TBlobPt UrlQBlobPt; PSIn UrlQBlobSIn;
    while (UrlQBBs->FNextBlobPt(TrvUrlQBlobPt, UrlQBlobPt, UrlQBlobSIn)){
      TStr UrlStr=TStr(*UrlQBlobSIn);
      int Lev=TInt(*UrlQBlobSIn);
      int Dep=TInt(*UrlQBlobSIn);
      PushQUrl(UrlStr, Lev, Dep);
    }
  }
}

void TCrawlerQueue::PushQUrlToUrlQBs(const PUrl& Url, const int& Lev, const int& Dep){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerUrlDescBs UrlDescBs=Crawler->GetUrlDescBs();
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  // check url
  IAssert(Url->IsOk(usHttp));
  // get url-md5
  TMd5Sig UrlMd5Sig=TCrawler::GetUrlMd5Sig(Url->GetUrlStr());
  // checks if url is new else push in the queue
  if (UrlDescBs->IsUrl(UrlMd5Sig)){ // if url already downloaded
    if (UrlDescBs->GetUrlDesc(UrlMd5Sig).DocLev>Lev){
      UrlDescBs->GetUrlDesc(UrlMd5Sig).DocLev=Lev;}
  } else
  if (IsQUrl(UrlMd5Sig)){ // if url waits in queue
  } else
  if ((Def->GetMxLev()==-1)||(Lev<=Def->GetMxLev())&&
   ((Def->GetMxDep()==-1)||(Dep<=Def->GetMxDep()))){
    // if url-queue-base empty then create
    if (UrlQBBs.Empty()){
      TStr UrlQBBsFNm=GetUrlQBBsFNm(); printf("*** %s\n", UrlQBBsFNm.CStr());
      UrlQBBs=TGBlobBs::New(UrlQBBsFNm, faCreate, Def->GetMxQSegLen());
    }
    // prepare url-queue-entry
    TMOut UrlQSOut(Url->GetUrlStr().Len()+100);
    Url->GetUrlStr().Save(UrlQSOut);
    TInt(Lev).Save(UrlQSOut);
    TInt(Dep).Save(UrlQSOut);
    PSIn UrlQSIn=UrlQSOut.GetSIn();
    TBlobPt UrlQBlobPt=UrlQBBs->PutBlob(UrlQSIn);
    // if segment too large then create new url-queue-base and repeat
    if (UrlQBlobPt.Empty()){
      TStr UrlQBBsFNm=GetUrlQBBsFNm(); printf("*** %s\n", UrlQBBsFNm.CStr());
      UrlQBBs=TGBlobBs::New(UrlQBBsFNm, faCreate, Def->GetMxQSegLen());
      UrlQBlobPt=UrlQBBs->PutBlob(UrlQSIn);
      IAssert(!UrlQBlobPt.Empty());
    }
  }
}

void TCrawlerQueue::PushQUrlToUrlQBs(const TStr& UrlStr, const int& Lev, const int& Dep){
  // check url
  PUrl Url=TUrl::New(UrlStr);
  if (Url->IsOk(usHttp)){
    PushQUrlToUrlQBs(Url, Lev, Dep);
  }
}

void TCrawlerQueue::PopSaveQUrlBs(){
  // shortcuts
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  PCrawlerFetcher Fetcher=Crawler->GetFetcher();
  // prepare queue-file
  TStr UrlQBBsFNm=GetUrlQBBsFNm(); 
  printf("*** Saving Active Queue-Url-Base To %s\n", UrlQBBsFNm.CStr());
  UrlQBBs=TGBlobBs::New(UrlQBBsFNm, faCreate);
  // traverse queue-levels
  for (int Lev=0; Lev<LevUrlIdQV.Len(); Lev++){
    while (!LevUrlIdQV[Lev].Empty()){
      // get queue-url-id & host
      int QUrlId=LevUrlIdQV[Lev].Top();
      TStr HostNm=GetQHostNm(QUrlId);
      PCrawlerHost Host=HostBs->GetHost(HostNm);
      // get url
      TStr UrlStr=GetQUrlStr(QUrlId);
      // get depth
      int Dep=GetQUrlDep(QUrlId);
      // pop queue-url-id
      LevUrlIdQV[Lev].Pop();
      Host->GetQueueUrls()--;
      DelQUrl(QUrlId);
      // prepare&save url-queue-entry
      TMOut UrlQSOut(UrlStr.Len()+100);
      UrlStr.Save(UrlQSOut);
      TInt(Lev).Save(UrlQSOut);
      TInt(Dep).Save(UrlQSOut);
      PSIn UrlQSIn=UrlQSOut.GetSIn();
      TBlobPt UrlQBlobPt=UrlQBBs->PutBlob(UrlQSIn);
    }
  }
}

/////////////////////////////////////////////////
// Crawler-Fetcher
void TCrawlerFetcher::DelCrawlerFetcherConn(const int& ConnId){
  // shortcuts
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  // get and delete queue-url-id from connection-id
  int QUrlId; PUrl Url; int FetchMSecs; GetDelConn(ConnId, QUrlId, Url, FetchMSecs);
  Crawler->GetQueue()->DelQUrl(QUrlId);
  // update host-info
  PCrawlerHost Host=HostBs->GetHost(Url->GetHostNm());
  Host->GetActiveConns()--;
  Host->GetFetchErrors()++;
  Host->GetLastFetchTm()=TSecTm::GetCurTm();
}

TCrawlerFetcher::TCrawlerFetcher(TCrawler* _Crawler):
  TWebPgFetch(), Crawler(_Crawler), CurMxConns(-1), 
  LastClrZombiesTm(), Zombies(0), ZombieConnIdToFetchSizeH(){
  // set max. connections
  IAssert(Crawler->GetDef()->GetMxConns()!=-1);
  PutMxConns(Crawler->GetDef()->GetMxConns());
  CurMxConns=1;
  // set max. content-length
  PutMxContLen(Crawler->GetDef()->GetMxContLen());
  // set max. retries
  PutMxRetries(Crawler->GetDef()->GetMxRetries());
  // set user-agent
  if (Crawler->GetDef()->GetUserAgentStr()=="ie8"){
    PutUserAgentStrIE8();
  } else {
    PutUserAgentStr(Crawler->GetDef()->GetUserAgentStr());
  }
}

void TCrawlerFetcher::GetConnV(TIntStrIntIntQuV& ConnIdUrlStrMSecsSizeQuV){
  ConnIdUrlStrMSecsSizeQuV.Gen(GetConns(), 0);
  int ConnP=ConnIdToQUrlIdUrlTmTrH.FFirstKeyId();
  while (ConnIdToQUrlIdUrlTmTrH.FNextKeyId(ConnP)){
    int ConnId=ConnIdToQUrlIdUrlTmTrH.GetKey(ConnP);
    TStr UrlStr=ConnIdToQUrlIdUrlTmTrH[ConnP].Val2->GetUrlStr();
    int FetchMSecs=int(TTm::GetCurUniMSecs()-ConnIdToQUrlIdUrlTmTrH[ConnP].Val3);
    int FetchSize=GetConnBytesRead(ConnId);
    ConnIdUrlStrMSecsSizeQuV.Add(TIntStrIntIntQu(ConnId, UrlStr, FetchMSecs, FetchSize));
  }
  ConnIdUrlStrMSecsSizeQuV.Sort();
}

void TCrawlerFetcher::ClrZombies(){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerQueue Queue=Crawler->GetQueue();
  // return if less then 100secs
  if ((LastClrZombiesTm.IsDef())&&(TTm::GetDiffMSecs(LastClrZombiesTm, TTm::GetCurUniTm())<100000)){return;}
  LastClrZombiesTm=TTm::GetCurUniTm();
  // get current-connections
  TIntStrIntIntQuV ConnIdUrlStrMSecsSizeQuV; GetConnV(ConnIdUrlStrMSecsSizeQuV);
  int ConnIds=ConnIdUrlStrMSecsSizeQuV.Len();
  // traverse current-connections
  for (int ConnN=0; ConnN<ConnIds; ConnN++){
    int ConnId=ConnIdUrlStrMSecsSizeQuV[ConnN].Val1;
    TStr UrlStr=ConnIdUrlStrMSecsSizeQuV[ConnN].Val2;
    int FetchSize=ConnIdUrlStrMSecsSizeQuV[ConnN].Val4;
    if (ZombieConnIdToFetchSizeH.IsKey(ConnId)){
      int PrevFetchSize=ZombieConnIdToFetchSizeH.GetDat(ConnId);
      // delete connection if sizes didn't change
      if (FetchSize==PrevFetchSize){
        printf("*** Zombie: %s\n", UrlStr.CStr()); // report
        Zombies++; // increment number of zombies
        DelConn(ConnId); // delete connection
        DelCrawlerFetcherConn(ConnId); // delete cralwer connection
        Queue()->PushQUrl(UrlStr, 0, 0); // re-push url
        PutCurMxConns(Def->GetMnConns()); // reset number of connections
      }
    }
  }
  // update connection-list
  ZombieConnIdToFetchSizeH.Clr(false);
  for (int ConnN=0; ConnN<ConnIds; ConnN++){
    int ConnId=ConnIdUrlStrMSecsSizeQuV[ConnN].Val1;
    int FetchSize=ConnIdUrlStrMSecsSizeQuV[ConnN].Val4;
    ZombieConnIdToFetchSizeH.AddDat(ConnId)=FetchSize;
  }
}

void TCrawlerFetcher::Fetch(){
  // shortcuts
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  PCrawlerQueue Queue=Crawler->GetQueue();
  // get and fetch url
  int QUrlId; TStr UrlStr;
  while ((GetConns()<=GetCurMxConns())&&(GetWaitUrls()==0)&&
   (Queue->PopQUrl(QUrlId, UrlStr))){
    PUrl IpUrl=TUrl::New(UrlStr);
    PCrawlerHost Host=HostBs->AddGetHost(IpUrl->GetHostNm());
    if (Host->IsIpNum()){
      IpUrl->PutIpNum(Host->GetIpNum());}
    int ConnId=FetchUrl(IpUrl);
    PUrl Url=TUrl::New(UrlStr);
    AddConn(ConnId, QUrlId, Url);
    Host->GetActiveConns()++;
  }
  // extend queue if under limit
  int QUrls=Queue->GetQUrls();
  int MnQLen=Crawler->GetDef()->GetMnQLen();
  if ((QUrls%100==0)&&(QUrls<MnQLen)){
    Queue->PushQUrlBs();
  }
}

void TCrawlerFetcher::OnFetch(const int& ConnId, const PWebPg& WebPg){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerUrlDescBs UrlDescBs=Crawler->GetUrlDescBs();
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  PCrawlerQueue Queue=Crawler->GetQueue();
  PCrawlerDocBs DocBs=Crawler->GetDocBs();
  // get queue-url-id from connection-id
  int QUrlId; PUrl Url; int FetchMSecs; GetDelConn(ConnId, QUrlId, Url, FetchMSecs);
  // move url-md5 and level from queue to url-base
  TMd5Sig UrlMd5Sig; int Lev; int Dep;
  Queue->GetQUrl(QUrlId, UrlMd5Sig, Lev, Dep);
  Queue->DelQUrl(QUrlId);
  if (!UrlDescBs->IsUrl(UrlMd5Sig)){
    // assert queue-url equals first-url in the list
    //**IAssert(UrlMd5Sig==TCrawler::GetUrlMd5Sig(WebPg->GetUrlStr(0)));
    if (UrlMd5Sig!=TCrawler::GetUrlMd5Sig(WebPg->GetUrlStr(0))){
      SaveToErrLog("Begin:IAssert(UrlMd5Sig==TCrawler::GetUrlMd5Sig(WebPg->GetUrlStr(0)));");
      SaveToErrLog(TStr::Fmt("Urls: %d", WebPg->GetUrls()).CStr());
      SaveToErrLog(TStr::Fmt("Md5: %s", UrlMd5Sig.GetStr().CStr()).CStr());
      for (int WebPgUrlN=0; WebPgUrlN<WebPg->GetUrls(); WebPgUrlN++){
        SaveToErrLog(TStr::Fmt("Url #%d: %s",
         WebPgUrlN,
         WebPg->GetUrlStr(WebPgUrlN).CStr()).CStr());
      }
      SaveToErrLog("End:IAssert(UrlMd5Sig==TCrawler::GetUrlMd5Sig(WebPg->GetUrlStr(0)));");
    }
    // traverse urls from web-page
    for (int WebPgUrlN=0; WebPgUrlN<WebPg->GetUrls(); WebPgUrlN++){
      // get url & url-md5
      TStr DocUrlStr=WebPg->GetUrlStr(WebPgUrlN);
      TMd5Sig DocUrlMd5Sig=TCrawler::GetUrlMd5Sig(DocUrlStr);
      // save content if url is new
      if (!UrlDescBs->IsUrl(DocUrlMd5Sig)){
        // save connect or redirection
        TBlobPt DocBlobPt; TMd5Sig DocMemMd5Sig;
        // save content if content-type allowed
        TStr ContTypeNm=WebPg->GetHttpResp()->GetFldVal(THttp::ContTypeFldNm);
        if (Def->IsContTypeNmOk(ContTypeNm)){
          if (WebPgUrlN==WebPg->GetUrls()-1){
            // url is final in the list - save content (http-response)
            TMem DocMem; WebPg->GetHttpResp()->GetAsMem(DocMem);
            DocMemMd5Sig=TMd5Sig(DocMem);
            DocBlobPt=DocBs->AddDoc(DocUrlStr, DocMem);
          } else {
            // url is not final in the list - save redirection
            TStr FinalUrlStr=WebPg->GetUrlStr(WebPg->GetUrls()-1);
            PHttpResp RedirHttpResp=THttpResp::New(
             THttp::RedirStatusCd, THttp::TextPlainFldVal, false, NULL, FinalUrlStr);
            TMem DocMem; RedirHttpResp->GetAsMem(DocMem);
            DocMemMd5Sig=TMd5Sig(DocMem);
            DocBlobPt=DocBs->AddDoc(DocUrlStr, DocMem);
          }
        }
        // expand allowed domains
        if (Def->IsRedirDmAllowed()){
          PUrl Url=TUrl::New(DocUrlStr);
          TStr HostNm=Url->GetHostNm();
          Def->AddRqDmNm(HostNm);
        }
        // save url-md5 & descriptor
        TCrawlerUrlDesc UrlDesc(
         DocBlobPt, DocMemMd5Sig, TSecTm::GetCurTm(), TUCh(Lev), TUCh(Dep));
        UrlDescBs->AddUrlDesc(DocUrlMd5Sig, UrlDesc);
      }
    }
  }
  // update host-info
  PCrawlerHost Host=HostBs->GetHost(Url->GetHostNm());
  Host->GetIpNum()=WebPg->GetIpNum(0);
  Host->GetActiveConns()--;
  Host->GetFetchedUrls()++;
  Host->GetTransferBytes()+=WebPg->GetHttpResp()->Len();
  Host->GetTransferMSecs()+=int(WebPg->GetFetchMSecs());
  Host->GetLastFetchTm()=TSecTm::GetCurTm();
  // extract and push urls if fetched text document
  TStr HostNm=Url->GetHostNm();
  if (WebPg->GetHttpResp()->IsContType(THttp::TextFldVal)){
    TUrlV OutUrlV; TUrlV OutRedirUrlV; WebPg->GetOutUrlV(OutUrlV, OutRedirUrlV);
    if (Def->IsRedirDmAllowed()){
      for (int UrlN=0; UrlN<OutRedirUrlV.Len(); UrlN++){
        TStr HostNm=OutRedirUrlV[UrlN]->GetHostNm();
        Def->AddRqDmNm(HostNm);
      }
    }
    for (int UrlN=0; UrlN<OutUrlV.Len(); UrlN++){
      TStr OutHostNm=OutUrlV[UrlN]->GetHostNm();
      if (Def->IsHostNmOk(OutHostNm)){
        if (Def->IsFExtOk(OutUrlV[UrlN])){
          if (HostNm==OutHostNm){
            Queue->PushQUrlToUrlQBs(OutUrlV[UrlN], Lev+1, Dep+1);
          } else {
            Queue->PushQUrlToUrlQBs(OutUrlV[UrlN], 0, Dep+1);
          }
        }
      }
    }
  }
  // notify
  printf("Fetched [Lev:%d Dep:%d Queue:%d Fetched:%d Time:%d Size:%d]: %s\n",
   Lev, Dep, Queue->GetQUrls(), UrlDescBs->GetUrls(), FetchMSecs,
   WebPg->GetHttpResp()->Len(), WebPg->GetUrlStr().CStr());
  // reset queue
  Queue->ResetQUrlBs();
  // shuffle queues
  Queue->ShuffleUrlQ();
  // clear zombies
  ClrZombies();
  // continue fetching
  Fetch();
  // stop if reached max. urls
  if (UrlDescBs->GetUrls()>=Def->GetMxUrls()){
    TSysMsg::Quit();
  }
}

void TCrawlerFetcher::OnError(const int& ConnId, const TStr& MsgStr){
  // delete connection
  DelCrawlerFetcherConn(ConnId);
  // continue fetching
  Fetch();
  // notify
  printf("Error: %s\n", MsgStr.CStr());
}

//////////////////////////////////////////////////
// Crawler-Timer
void TCrawlerTimer::OnTimeOut(){
  // shortcuts
  PCrawlerDef Def=Crawler->GetDef();
  PCrawlerFetcher Fetcher=Crawler->GetFetcher();
  PCrawlerStat Stat=Crawler->GetStat();
  // time intervals
  uint64 CurEventMSecs=TTm::GetCurUniMSecs();
  uint64 InterEventMSecs=CurEventMSecs-LastEventMSecs;
  uint64 ToutMSecs=GetTimeOut();
  double TimerEventPrecision=(ToutMSecs==0) ? 0 : double(InterEventMSecs)/ToutMSecs;
  // set current max. number of connections
  if (TimerEventPrecision>1.01){
    Fetcher->PutCurMxConns(int(Fetcher->GetCurMxConns()*0.90));
  } else {
    Fetcher->PutCurMxConns(Fetcher->GetCurMxConns()+1);
  }
  if (Fetcher->GetCurMxConns()<Def->GetMnConns()){
    Fetcher->PutCurMxConns(Def->GetMnConns());}
  if (Fetcher->GetCurMxConns()>Def->GetMxConns()){
    Fetcher->PutCurMxConns(Def->GetMxConns());}
  printf("---------------------------------------------------------------\n"); 
  printf("*** Crawler-Timer: Timeout-Precision=%.3f CurMxConns=%d\n", 
   TimerEventPrecision, Fetcher->GetCurMxConns());
  // manage queue
  if (Crawler->GetFetcher()->Empty()&&Crawler->GetQueue()->Empty()){
    Crawler->GetQueue()->PushQUrlBs();
    if (Crawler->GetQueue()->Empty()){
      TSysMsg::Quit();
    }
  } else {
    // fill queue if below minimum
    int QUrls=Crawler->GetQueue()->GetQUrls();
    int MnQLen=Crawler->GetDef()->GetMnQLen();
    if ((QUrls%100==0)&&(QUrls<MnQLen)){
      Crawler->GetQueue()->PushQUrlBs();
    }
    // re-activate fetcher
    Crawler->GetFetcher()->Fetch();
  }
  // 100-sec timer
  if (Last100SecsEventMSecs==0){
    Last100SecsEventMSecs=CurEventMSecs;
  } else 
  if (CurEventMSecs-Last100SecsEventMSecs>=100000){
    Stat->On100Secs();
    Last100SecsEventMSecs=CurEventMSecs;
  }
  // 1000-sec timer
  if (Last1000SecsEventMSecs==0){
    Last1000SecsEventMSecs=CurEventMSecs;
  } else 
  if (CurEventMSecs-Last1000SecsEventMSecs>=1000000){
    Stat->On1000Secs();
    Last1000SecsEventMSecs=CurEventMSecs;
  }
  // update this-event's time
  LastEventMSecs=CurEventMSecs;
}

/////////////////////////////////////////////////
// Crawler-Web-Server
TStr TCrawlerWebSrv::GetUrlStr(const int& RefreshSecs, const TStr& SortOrderNm){
  TChA UrlChA;
  UrlChA+='/';
  UrlChA+='?';
  UrlChA+=TStr::Fmt("Refresh=%d", RefreshSecs);
  UrlChA+='&';
  UrlChA+=TStr::Fmt("Sort=%s", SortOrderNm.CStr());
  return UrlChA;
}

TStr TCrawlerWebSrv::GetAnchorStr(const TStr& DescStr, 
 const int& RefreshSecs, const TStr& SortOrderNm){
  TChA AnchorChA;
  TStr UrlStr=GetUrlStr(RefreshSecs, SortOrderNm);
  AnchorChA+=TStr::Fmt("<a href=\"%s\">", UrlStr.CStr());
  AnchorChA+=DescStr;
  AnchorChA+="</a>";
  return AnchorChA;
}

void TCrawlerWebSrv::OnHttpRq(const int& SockId, const PHttpRq& HttpRq){
  // shortcuts
  PCrawlerHostBs HostBs=Crawler->GetHostBs();
  PCrawlerUrlDescBs UrlDescBs=Crawler->GetUrlDescBs();
  PCrawlerFetcher Fetcher=Crawler->GetFetcher();
  PCrawlerQueue Queue=Crawler->GetQueue();
  PCrawlerStat Stat=Crawler->GetStat();
  TStr UrlStr=HttpRq->GetUrl()->GetUrlStr();
  TStr UrlPathStr=HttpRq->GetUrl()->GetPathStr();
  PUrlEnv UrlEnv=HttpRq->GetUrlEnv();

  // check stop
  bool StopCrawlingP=false;
  if (UrlPathStr=="/Stop"){
    StopCrawlingP=true;
    TSysMsg::Quit();
  }

  // get parameters
  int RefreshSecs=UrlEnv->GetVal("Refresh").GetInt(100);
  TStr SortOrderNm=UrlEnv->GetVal("Sort");

  // prepare default html with time
  TChA HtmlChA;

  // summary-host information
  int ActiveConns; int FetchedUrls; int FetchErrors; int QueueUrls;
  double TransferBytes; double TransferMSecs; 
  double AvgHttpRespLen; double AvgTransferBps;
  HostBs->GetSummaryInfo(ActiveConns, FetchedUrls, FetchErrors, QueueUrls, 
   TransferBytes, TransferMSecs, AvgHttpRespLen, AvgTransferBps);

  // header
  {HtmlChA+="<html><head><title>Crawling</title>";
  if ((!StopCrawlingP)&&(RefreshSecs>0)){
    HtmlChA+=TStr::Fmt("<meta http-equiv=\"refresh\" content=\"%d\">", RefreshSecs);}
  HtmlChA+="</head><body>";
  HtmlChA+="<h1>WebBird-Crawler</h1>";
  HtmlChA+=TSecTm::GetCurTm().GetStr(); HtmlChA+="<br>";
  // stop-form
  HtmlChA+="<form method=get action=\"/Stop\">";
  HtmlChA+="<input type=submit value=Stop>";
  HtmlChA+="</form>";
  // refresh-form
  HtmlChA+="<form method=get action=\"/\">";
  HtmlChA+="<input type=submit value=\"Refresh Seconds\">";
  HtmlChA+=TStr::Fmt("<input type=text name=\"Refresh\" value=\"%d\" size=2>", RefreshSecs);
  HtmlChA+="</form>";}

  // general
  {HtmlChA+="<h2>General Info</h2>\n";
  uint64 StartMSecs=TTm::GetMSecsFromTm(Crawler->GetStartTm());
  uint64 CurMSecs=TTm::GetCurUniMSecs();
  uint64 CrawlingMSecs=CurMSecs-StartMSecs;
  double CrawlingSecs=CrawlingMSecs/1000.0;
  double CrawlingPpsSpeed=CrawlingSecs<=0 ? 0 : FetchedUrls/CrawlingSecs;
  double AvgPgLen=(FetchedUrls>0)&&(TransferBytes>0) ? TransferBytes/FetchedUrls : 0;
  HtmlChA+="<table border=1>\n";
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Urls Fetched<td align=right>%d</tr>\n", UrlDescBs->GetUrls());
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Queue Length<td align=right>%d</tr>\n", Queue->GetQUrls());
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Crawling Time<td align=right>%.0fs</tr>\n", CrawlingSecs);
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Crawling Speed<td align=right>%.3fpps</tr>\n", CrawlingPpsSpeed);
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Crawling Speed (last 1000secs)<td align=right>%.3fpps</tr>\n", 
    Stat->GetLast1000SecsCrawlingSpeed());
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Crawling Speed (last 100secs)<td align=right>%.3fpps</tr>\n", 
    Stat->GetLast100SecsCrawlingSpeed());
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Avg. Page Length<td align=right>%sb</tr>\n", 
   TFlt::GetGigaStr(AvgPgLen).CStr());
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Cleaned Zombies<td align=right>%d</tr>\n", Fetcher->GetZombies());
  HtmlChA+="</table>\n";}

  // hosts
  {HtmlChA+="<h2>Hosts</h2>\n";
  HtmlChA+="<table border=1>\n";
  HtmlChA+=TStr::Fmt("<tr><th>%s (%d)<th>%s<th>%s<th>%s<th>%s<th>%s<th>%s<th>%s<th>%s</tr>\n", 
   GetAnchorStr("Hosts", RefreshSecs, "Host").CStr(),
   HostBs->GetHosts(),
   GetAnchorStr("Active", RefreshSecs, "Active").CStr(),
   GetAnchorStr("Fetched", RefreshSecs, "Fetched").CStr(),
   GetAnchorStr("Errors", RefreshSecs, "Errors").CStr(),
   GetAnchorStr("Queue", RefreshSecs, "Queue").CStr(),
   GetAnchorStr("Bytes", RefreshSecs, "Bytes").CStr(),
   GetAnchorStr("Time", RefreshSecs, "Time").CStr(),
   GetAnchorStr("PageLen", RefreshSecs, "PageLen").CStr(),
   GetAnchorStr("Speed", RefreshSecs, "Speed").CStr());
  HtmlChA+=TStr::Fmt("<tr><td align=right><b>Summary</b>"
    "<td align=right><b>%d</b><td align=right><b>%d</b><td align=right><b>%d</b><td align=right><b>%d</b>"
    "<td align=right><b>%sb</b><td align=right><b>%ss</b><td align=right><b>%sb</b><td align=right><b>%sbps</b></tr>\n",
   ActiveConns, FetchedUrls, FetchErrors, QueueUrls, 
   TFlt::GetGigaStr(TransferBytes).CStr(), 
   TFlt::GetGigaStr(TransferMSecs/1000).CStr(), 
   TFlt::GetGigaStr(AvgHttpRespLen).CStr(),
   TFlt::GetGigaStr(AvgTransferBps).CStr());
  // single-host-information
  TCrawlerHostV HostV; HostBs->GetSortedHostV(HostV, SortOrderNm);
  for (int HostN=0; HostN<HostV.Len(); HostN++){
    PCrawlerHost Host=HostV[HostN];
    HtmlChA+=TStr::Fmt("<tr><td align=right><a href=\"http://%s\">%s<a>"
     "<td align=right>%d<td align=right>%d<td align=right>%d<td align=right>%d"
     "<td align=right>%sb<td align=right>%ss<td align=right>%sb<td align=right>%sbps</tr>\n",
     Host->GetHostNm().CStr(), 
     Host->GetHostNm().CStr(), 
     Host->GetActiveConns(),
     Host->GetFetchedUrls(), 
     Host->GetFetchErrors(), 
     Host->GetQueueUrls(),
     TFlt::GetGigaStr(double(Host->GetTransferBytes())).CStr(), 
     TFlt::GetGigaStr(Host->GetTransferMSecs()/1000).CStr(),
     TFlt::GetGigaStr(Host->GetAvgHttpRespLen()).CStr(),
     TFlt::GetGigaStr(Host->GetAvgTransferBps()).CStr()); 
  }
  HtmlChA+="</table>";}

  // queue-levels
  {HtmlChA+="<h2>Queue Levels</h2>\n";
  HtmlChA+="<table border=1>\n";
  int AllQUrls=Queue->GetQUrls();
  HtmlChA+=TStr::Fmt("<tr><td align=right>%s<td align=right>%d</tr>\n",
   "All Queue Urls", AllQUrls);
  HtmlChA+=TStr::Fmt("<tr><td align=right>%s<td align=right>%d</tr>\n",
    "Unconn Queue Urls", Queue->GetUnconnQUrls());
  for (int Lev=0; Lev<Queue->GetQLevs(); Lev++){
    HtmlChA+=TStr::Fmt("<tr><td align=right>Level-%d Urls<td align=right>%d</tr>\n",
     Lev, Queue->GetQLevUrls(Lev));
  }
  HtmlChA+="</table>";}

  // queues
  {HtmlChA+="<h2>Queue</h2>\n";
  TIntStrPrV QUrlsHostPrV; Queue->GetQUrlsHostPrV(QUrlsHostPrV);
  HtmlChA+="<table border=1>\n";
  HtmlChA+=TStr::Fmt("<tr><th>#<th>Host<th>Queue-Urls<th>%%</tr>\n");
  int AllQUrls=Queue->GetQUrls();
  for (int HostN=0; HostN<QUrlsHostPrV.Len(); HostN++){
    TStr HostLevNm=QUrlsHostPrV[HostN].Val2;
    TStr HostNm; TStr LevStr; HostLevNm.SplitOnCh(HostNm, ':', LevStr);
    int QUrls=QUrlsHostPrV[HostN].Val1;
    double QUrlsPrc=double(QUrls)/double(AllQUrls);
    HtmlChA+=TStr::Fmt("<tr><td align=right>%d<td align=right><a href=\"http://%s/\">%s</a> / %s"
     "<td align=right>%d<td align=right>%.3f%%</tr>\n",
     1+HostN, HostNm.CStr(), HostNm.CStr(), LevStr.CStr(), QUrls, 100*QUrlsPrc);
  }
  HtmlChA+="</table>";}

  // connections
  {TIntStrIntIntQuV ConnIdUrlStrMSecsSizeQuV;
  Fetcher->GetConnV(ConnIdUrlStrMSecsSizeQuV);
  int ConnIds=ConnIdUrlStrMSecsSizeQuV.Len();
  HtmlChA+="<h2>Connections</h2>\n";
  HtmlChA+="<table border=1>\n";
  HtmlChA+="<tr><th>#<th>Connection<th>Time<th>Size<th>Url</tr>\n";
  for (int ConnN=0; ConnN<ConnIds; ConnN++){
    int ConnId=ConnIdUrlStrMSecsSizeQuV[ConnN].Val1;
    TStr UrlStr=ConnIdUrlStrMSecsSizeQuV[ConnN].Val2;
    int FetchMSecs=ConnIdUrlStrMSecsSizeQuV[ConnN].Val3;
    int FetchSize=ConnIdUrlStrMSecsSizeQuV[ConnN].Val4;
    HtmlChA+=TStr::Fmt("<tr><td>%d<td>%d<td>%ss<td>%sb<td><a href=\"%s\">%s</a></tr>\n",
     1+ConnN, ConnId, 
     TFlt::GetGigaStr(FetchMSecs/1000).CStr(), TFlt::GetGigaStr(FetchSize).CStr(), 
     UrlStr.CStr(), UrlStr.CStr());
  }
  HtmlChA+="</table>\n";}

  // footer
  HtmlChA+="</body></html>";

  // send response
  PSIn BodySIn=TMIn::New(HtmlChA);
  PHttpResp HttpResp=THttpResp::New(
   THttp::OkStatusCd, THttp::TextHtmlFldVal, false, BodySIn);
  // construct & send response
  SendHttpResp(SockId, HttpResp);
}

/////////////////////////////////////////////////
// Crawler-Document-Base
TFAccess TCrawlerDocBs::Open(){
  TFAccess DocBsFAccess=TFRnd::GetFAccessFromStr(DocBsFAccessNm);
  IAssert((DocBsFAccess==faCreate)||(DocBsFAccess==faUpdate)||(DocBsFAccess==faRestore));
  DocBBs=TMBlobBs::New(DocBsNrFPath+DocBsNm, DocBsFAccess, DocBsMxSegLen);
  return DocBsFAccess;
}

void TCrawlerDocBs::Close(){
  DocBBs=NULL;
}

TBlobPt TCrawlerDocBs::AddDoc(const TStr& DocUrlStr, const TMem& DocMem){
  // save the document as time+name+contents
  TMOut SOut(DocUrlStr.Len()+DocMem.Len()+100);
  TTm::GetCurUniTm().GetWebLogDateTimeStr().Save(SOut);
  DocUrlStr.Save(SOut);
  DocMem.Save(SOut);
  PSIn SIn=SOut.GetSIn();
  TBlobPt BPt=DocBBs->PutBlob(SIn);
  // return
  return BPt;
}

bool TCrawlerDocBs::FNextDocBlobPt(TBlobPt& TrvDocBlobPt,
 TBlobPt& DocBlobPt, TStr& DateTimeStr, TStr& DocUrlStr, TMem& DocMem){
  PSIn DocBlobSIn;
  if (DocBBs->FNextBlobPt(TrvDocBlobPt, DocBlobPt, DocBlobSIn)){
    DateTimeStr=TStr(*DocBlobSIn); // construct document-date&time
    DocUrlStr=TStr(*DocBlobSIn); // construct document-url
    DocMem=TMem(*DocBlobSIn); // construct document-contents
    return true;
  } else {
    return false;
  }
}

/////////////////////////////////////////////////
// Crawler-Statistics
void TCrawlerStat::On100Secs(){
  uint64 CurEventMSecs=TTm::GetCurUniMSecs();
  uint64 CurFetchedUrls=Crawler->GetUrlDescBs()->GetUrls();
  double DiffSecs=(CurEventMSecs-Last100SecsEventMSecs)/double(1000);
  if (DiffSecs<1){
    Last100SecsCrawlingSpeed=0;
  } else {
    Last100SecsCrawlingSpeed=(CurFetchedUrls-Last100SecsFetchedUrls)/DiffSecs;
  }
  Last100SecsEventMSecs=CurEventMSecs;
  Last100SecsFetchedUrls=CurFetchedUrls;
}

void TCrawlerStat::On1000Secs(){
  uint64 CurEventMSecs=TTm::GetCurUniMSecs();
  uint64 CurFetchedUrls=Crawler->GetUrlDescBs()->GetUrls();
  double DiffSecs=(CurEventMSecs-Last1000SecsEventMSecs)/double(1000);
  if (DiffSecs<1){
    Last1000SecsCrawlingSpeed=0;
  } else {
    Last1000SecsCrawlingSpeed=(CurFetchedUrls-Last1000SecsFetchedUrls)/DiffSecs;
  }
  Last1000SecsEventMSecs=CurEventMSecs;
  Last1000SecsFetchedUrls=CurFetchedUrls;
}

/////////////////////////////////////////////////
// Crawler
TCrawler::TCrawler():
  Def(TCrawlerDef::New()),
  UrlDescBs(), HostBs(), Queue(), Fetcher(), DocBs(), Timer(), WebSrv(),
  StartTm(TTm::GetCurUniTm()){
  DocBs=TCrawlerDocBs::New(this);
}

void TCrawler::DefCrawlBs(const TStr& CrawlBsNm, const TStr& CrawlBsFPath,
 const TStr& CrawlBsFAccessNm, const int& CrawlBsMxSegLen){
  // shortcuts
  PCrawlerDocBs DocBs=GetDocBs();
  // define crawling-base
  DocBs->PutDocBsNm(CrawlBsNm);
  DocBs->PutDocBsNrFPath(TStr::GetNrFPath(CrawlBsFPath));
  DocBs->PutDocBsFAccessNm(CrawlBsFAccessNm);
  DocBs->PutDocBsMxSegLen(CrawlBsMxSegLen);
}

void TCrawler::StartCrawling(){
  // prepare url-desc-base
  UrlDescBs=TCrawlerUrlDescBs::New(this);
  // prepare host-base
  HostBs=TCrawlerHostBs::New(this);
  // prepare queue
  Queue=TCrawlerQueue::New(this);
  // prepare fetcher
  Fetcher=TCrawlerFetcher::New(this);
  // prepare statistics
  Stat=TCrawlerStat::New(this);
  // open document-base
  TFAccess DocBsFAccess=GetDocBs()->Open();

  // if 'create' start with start-urls
  if (DocBsFAccess==faCreate){
    // push start urls
    PCrawlerDef Def=GetDef();
    int StartUrls=Def->GetStartUrls();
    for (int UrlN=0; UrlN<StartUrls; UrlN++){
      Queue->PushQUrl(Def->GetStartUrl(UrlN), 0, 0);}
  }

  // if 'restore' collect crawled-urls
  if (DocBsFAccess==faRestore){
    printf("Restoring Crawled Documents...\n");
    TBlobPt TrvDocBlobPt=GetDocBs()->FFirstDocBlobPt();
    TBlobPt DocBlobPt; TStr DateTimeStr; TStr DocUrlStr; TMem DocMem; int DocN=0;
    while (GetDocBs()->FNextDocBlobPt(TrvDocBlobPt, DocBlobPt, DateTimeStr, DocUrlStr, DocMem)){
      DocN++; printf("%d\r", DocN);
      // save url-md5
      TMd5Sig DocUrlMd5Sig=TCrawler::GetUrlMd5Sig(DocUrlStr);
      TMd5Sig DocMemMd5Sig(DocMem);
      TCrawlerUrlDesc UrlDesc(DocBlobPt, DocMemMd5Sig, TSecTm::GetCurTm(), TUCh(0), TUCh(0));
      UrlDescBs->AddUrlDesc(DocUrlMd5Sig, UrlDesc);
    }
    printf("\nDone.\n");
  }

  // if 'update' load crawled-urls
  if (DocBsFAccess==faUpdate){
    GetUrlDescBs()->Load();
  }

  // start-timer
  Timer=TCrawlerTimer::New(this, 1000);
  // start web-server
  WebSrv=TCrawlerWebSrv::New(this);
  // start-fetching
  Fetcher->Fetch();
}

void TCrawler::StopCrawling(){
  // stop web-server
  WebSrv=NULL;
  // stop timer
  Timer->StopTimer();
  // close document-base
  GetDocBs()->Close();
  // save url-base
  GetUrlDescBs()->Save();
  // save active queue
  GetQueue()->PopSaveQUrlBs();
}

TMd5Sig TCrawler::GetUrlMd5Sig(const TStr& UrlStr){
  // turn url-string to lower-case
  TStr LcUrlStr=UrlStr.GetLc();
  // calculate & return md5
  return TMd5Sig(LcUrlStr);
}
