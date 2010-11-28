/////////////////////////////////////////////////
// Forward
class TWebFilter;
typedef TPt<TWebFilter> PWebFilter;

/////////////////////////////////////////////////
// Web-Filter
ClassTP(TWebFilter, PWebFilter)//{
public:
  static const int MxMxConns;
  static const int MxMxConnsPerSrv;
  static const int MxMxDocSize;
  static const int MxMxPathSegs;
  static const TStr FExt;
private:
  TInt MxConns;
  TInt MxConnsPerSrv;
  TInt MxDocSize;
  TInt MxPathSegs;
  TBool DoAllowScripts;
  TStrV StartUrlStrV;
  TStrV DmConstrStrV;
  TStrV PassConstrUrlStrV;
  TStrV PosConstrUrlStrV;
  TStrV NegConstrUrlStrV;
  TStrH BadFExtH;
  void AddBadFExt(const TStr& BadFExt);
  void GenDfBadFExtH();
public:
  TWebFilter():
    MxConns(20), MxConnsPerSrv(5), MxDocSize(100000),
    MxPathSegs(10), DoAllowScripts(false),
    StartUrlStrV(),
    DmConstrStrV(), PassConstrUrlStrV(),
    PosConstrUrlStrV(), NegConstrUrlStrV(),
    BadFExtH(200){GenDfBadFExtH();}
  ~TWebFilter(){}
  TWebFilter(TSIn& SIn):
    MxConns(SIn), MxConnsPerSrv(SIn), MxDocSize(SIn),
    MxPathSegs(SIn), DoAllowScripts(SIn),
    StartUrlStrV(SIn),
    DmConstrStrV(SIn), PassConstrUrlStrV(SIn),
    PosConstrUrlStrV(SIn), NegConstrUrlStrV(SIn),
    BadFExtH(SIn){}
  static PWebFilter Load(TSIn& SIn){return PWebFilter(new TWebFilter(SIn));}
  void Save(TSOut& SOut){
    MxConns.Save(SOut); MxConnsPerSrv.Save(SOut); MxDocSize.Save(SOut);
    MxPathSegs.Save(SOut); DoAllowScripts.Save(SOut);
    StartUrlStrV.Save(SOut);
    DmConstrStrV.Save(SOut); PassConstrUrlStrV.Save(SOut);
    PosConstrUrlStrV.Save(SOut); NegConstrUrlStrV.Save(SOut);
    BadFExtH.Save(SOut);}

  TWebFilter& operator=(const TWebFilter&){Fail; return *this;}

  void PutMxConns(const int& _MxConns){
    if ((0<_MxConns)&&(_MxConns<=MxMxConns)){MxConns=_MxConns;}}
  int GetMxConns(){return MxConns;}
  bool IsOkConns(const int& Conns){
    return Conns<MxConns;}
  bool IsFullConns(const int& Conns){
    return Conns>=MxConns;}

  void PutMxConnsPerSrv(const int& _MxConnsPerSrv){
    if ((0<_MxConnsPerSrv)&&(_MxConnsPerSrv<=MxMxConnsPerSrv)){
      MxConnsPerSrv=_MxConnsPerSrv;}}
  int GetMxConnsPerSrv(){return MxConnsPerSrv;}
  bool IsOkConnsPerSrv(const int& ConnsPerSrv){
    return ConnsPerSrv<MxConnsPerSrv;}

  void PutUnlimDocSize(){MxDocSize=0;}
  void PutMxDocSize(const int& _MxDocSize){
    if ((0<=_MxDocSize)&&(_MxDocSize<=MxMxDocSize)){MxDocSize=_MxDocSize;}}
  int GetMxDocSize(){return MxDocSize;}
  bool IsOkDocSize(const int& DocSize){
    return (MxDocSize==0)||(DocSize<=MxDocSize);}

  void PutMxPathSegs(const int& _MxPathSegs){
    if ((0<=_MxPathSegs)&&(_MxPathSegs<=MxMxPathSegs)){MxPathSegs=_MxPathSegs;}}
  int GetMxPathSegs(){return MxPathSegs;}
  bool IsOkPathSegs(const int& PathSegs){
    return (MxPathSegs==0)||(PathSegs<=MxPathSegs);}

  void PutDoAllowScripts(const bool& _DoAllowScripts){
    DoAllowScripts=_DoAllowScripts;}
  int GetDoAllowScripts(){return DoAllowScripts;}
  bool IsOkDoAllowScripts(const TStr& UrlStr){
    return DoAllowScripts||(!UrlStr.IsChIn('?'));}

  int GetStartUrls(){return StartUrlStrV.Len();}
  TStr GetStartUrl(const int& UrlN){return StartUrlStrV[UrlN];}
  void AddStartUrl(const TStr& UrlStr){
    PUrl Url=PUrl(new TUrl(UrlStr));
    if (Url->IsOk()){StartUrlStrV.AddMerged(Url->GetUrlStr());}}
  void DelStartUrl(const TStr& UrlStr){
    int UrlN=StartUrlStrV.SearchForw(UrlStr);
    if (UrlN!=-1){StartUrlStrV.Del(UrlN);}}

  int GetDmConstrs(){return DmConstrStrV.Len();}
  TStr GetDmConstr(const int& DmN){return DmConstrStrV[DmN];}
  void AddDmConstr(const TStr& DmConstrStr){
    DmConstrStrV.AddMerged(DmConstrStr);}
  void DelDmConstr(const TStr& DmConstrStr){
    int DmN=DmConstrStrV.SearchForw(DmConstrStr);
    if (DmN!=-1){DmConstrStrV.Del(DmN);}}
  bool IsOkDmConstr(const TStr& DmStr){
    if (DmConstrStrV.Len()==0){return true;}
    for (int DmN=0; DmN<DmConstrStrV.Len(); DmN++){
      if (DmStr.IsSuffix(DmConstrStrV[DmN])){return true;}}
    return false;}

  int GetPassConstrUrls(){return PassConstrUrlStrV.Len();}
  TStr GetPassConstrUrl(const int& UrlN){return PassConstrUrlStrV[UrlN];}
  void AddPassConstrUrl(const TStr& PassConstrUrlStr){
    PassConstrUrlStrV.AddMerged(PassConstrUrlStr);}
  void DelPassConstrUrl(const TStr& PassConstrUrlStr){
    int UrlN=PassConstrUrlStrV.SearchForw(PassConstrUrlStr);
    if (UrlN!=-1){PassConstrUrlStrV.Del(UrlN);}}
  bool IsOkPassConstrUrl(const TStr& UrlStr){
    if (PassConstrUrlStrV.Len()==0){return false;}
    for (int UrlN=0; UrlN<PassConstrUrlStrV.Len(); UrlN++){
      if (UrlStr.IsWcMatch(PassConstrUrlStrV[UrlN], '*', '.')){return true;}}
    return false;}

  int GetPosConstrUrls(){return PosConstrUrlStrV.Len();}
  TStr GetPosConstrUrl(const int& UrlN){return PosConstrUrlStrV[UrlN];}
  void AddPosConstrUrl(const TStr& PosConstrUrlStr){
    PosConstrUrlStrV.AddMerged(PosConstrUrlStr);}
  void DelPosConstrUrl(const TStr& PosConstrUrlStr){
    int UrlN=PosConstrUrlStrV.SearchForw(PosConstrUrlStr);
    if (UrlN!=-1){PosConstrUrlStrV.Del(UrlN);}}
  bool IsOkPosConstrUrl(const TStr& UrlStr){
    if (PosConstrUrlStrV.Len()==0){return true;}
    for (int UrlN=0; UrlN<PosConstrUrlStrV.Len(); UrlN++){
      if (UrlStr.IsWcMatch(PosConstrUrlStrV[UrlN], '*', '.')){return true;}}
    return false;}

  int GetNegConstrUrls(){return NegConstrUrlStrV.Len();}
  TStr GetNegConstrUrl(const int& UrlN){return NegConstrUrlStrV[UrlN];}
  void AddNegConstrUrl(const TStr& NegConstrUrlStr){
    NegConstrUrlStrV.AddMerged(NegConstrUrlStr);}
  void DelNegConstrUrl(const TStr& NegConstrUrlStr){
    int UrlN=NegConstrUrlStrV.SearchForw(NegConstrUrlStr);
    if (UrlN!=-1){NegConstrUrlStrV.Del(UrlN);}}
  bool IsOkNegConstrUrl(const TStr& UrlStr){
    for (int UrlN=0; UrlN<NegConstrUrlStrV.Len(); UrlN++){
      if (UrlStr.IsWcMatch(NegConstrUrlStrV[UrlN], '*', '.')){return false;}}
    return true;}

  bool IsOkFExt(const TStr& FExt);
  bool IsUrlTok(const PHtmlTok& Tok, TStr& RelUrlStr);
  bool IsUrlOk(const PUrl& Url);

  // file loading & saving
  static PWebFilter Load(const TStr& FNm){
    bool Exists=false; TFIn SIn(FNm, Exists);
    if (Exists){return Load(SIn);} else {return NULL;}}
  void Save(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  static PWebFilter LoadTxt(const PSIn& SIn);
  void SaveTxt(const PSOut& SOut);
  static PWebFilter LoadTxt(const TStr& FNm){
    bool Exists=false; PSIn SIn=TFIn::New(FNm, Exists);
    if (Exists){return LoadTxt(SIn);} else {return NULL;}}
  void SaveTxt(const TStr& FNm){
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut);}
};

/////////////////////////////////////////////////
// Web-Base
class TWebBs{
public:
  TWebBs(){}
  virtual ~TWebBs(){}

  TWebBs& operator=(const TWebBs&){Fail; return *this;}

  virtual bool IsUrlStr(const TStr& UrlStr)=0;
  virtual TStr GetFetchStatusStr()=0;
  virtual TStr GetStatusStr()=0;

  virtual void OnFetch(const int& FId, const PWebPg& WebPg)=0;
  virtual void OnInfo(const int& FId, const TStr& MsgStr)=0;
  virtual void OnError(const int& FId, const TStr& ErrStr)=0;
  virtual void OnInfo(const TStr& MsgStr)=0;
  virtual void OnStatus(const TStr& StatusStr)=0;
};

/////////////////////////////////////////////////
// Web-Host-Name-Base
ClassTP(TWebHostNmBs, PWebHostNmBs)//{
private:
  TStrStrH AliasToFinalNmH;
public:
  TWebHostNmBs(const int& Ports=1000): AliasToFinalNmH(Ports){}
  TWebHostNmBs(TSIn& SIn): AliasToFinalNmH(SIn){}
  void Save(TSOut& SOut){AliasToFinalNmH.Save(SOut);}

  TWebHostNmBs& operator=(const TWebHostNmBs&){Fail; return *this;}

  void AddAliasToFinalNm(const TStr& AliasNm, const TStr& FinalNm){
    IAssert(!AliasToFinalNmH.IsKey(AliasNm));
    AliasToFinalNmH.AddDat(AliasNm, FinalNm);}
  TStr GetFinalNm(const TStr& AliasNm) const {
    int AliasId;
    if (AliasToFinalNmH.IsKey(AliasNm, AliasId)){
      return AliasToFinalNmH[AliasId];}
    else {return AliasNm;}}

  static PWebHostNmBs LoadTxt(const PSIn& SIn);
  static PWebHostNmBs LoadTxt(const TStr& FNm){
    PSIn SIn=TFIn::New(FNm); return LoadTxt(SIn);}
  void SaveTxt(const PSOut& SOut) const;
  void SaveTxt(const TStr& FNm){
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut);}
};

/////////////////////////////////////////////////
// Web-Base-Fetch-Host-Description
class TWebBsFetchHostDesc{
private:
  TStr FinalHostNm;
  TStr IpNum;
  TInt Conns;
public:
  TWebBsFetchHostDesc():
    FinalHostNm(), IpNum(), Conns(0){}
  TWebBsFetchHostDesc(TSIn& SIn):
    FinalHostNm(SIn), IpNum(SIn), Conns(SIn){}
  void Save(TSOut& SOut){
    FinalHostNm.Save(SOut); IpNum.Save(SOut); Conns.Save(SOut);}

  TWebBsFetchHostDesc& operator=(const TWebBsFetchHostDesc& HostDesc){
    if (this!=&HostDesc){
      FinalHostNm=HostDesc.FinalHostNm; IpNum=HostDesc.IpNum;
      Conns=HostDesc.Conns;}
    return *this;}

  bool IsDefFinalHostNm() const {
    return !FinalHostNm.Empty();}
  bool IsAliasHostNm(const TStr& HostNm) const {
    IAssert(IsDefFinalHostNm()); return HostNm!=FinalHostNm;}
  void PutFinalHostNm(const TStr& _FinalHostNm){
    IAssert(!IsDefFinalHostNm()); IAssert(!_FinalHostNm.Empty());
    FinalHostNm=_FinalHostNm;}
  TStr GetFinalHostNm() const {
    IAssert(IsDefFinalHostNm()); return FinalHostNm;}
  TStr GetIpNum() const {
    IAssert(!IpNum.Empty()); return IpNum;}

  bool IsDefConns() const {return Conns!=-1;}
  void UndefConns(){Conns=-1;}
  int GetConns() const {IAssert(IsDefConns()); return Conns;}
  void IncConns(){IAssert(IsDefConns()); Conns++;}
  void AddConns(const int& _Conns){
    IAssert(IsDefConns()&&(_Conns>=0)); Conns+=_Conns;}
  void DecConns(){IAssert(IsDefConns()&&(int(Conns)>0)); Conns--;}
};

/////////////////////////////////////////////////
// Web-Base-Fetch
ClassTP(TWebBsFetch, PWebBsFetch)//{
public:
  static const TStr WebBsFetchFExt;
  static const TStr WebBsFetchUrlFExt;
  static const TStr WebHostBsFExt;
  static const int EstWebPgs;
  static const int EstHostNms;
  static const int UrlStrHCacheSize;
private:
  typedef TFHash<TStr, TInt, TVoid> TUrlStrToFIdH;
  typedef TPt<TUrlStrToFIdH> PUrlStrToFIdH;
  TWebBs* WebBs;
  TStr WebBsFetchFNm;
  TStr WebHostBsFNm;
  TFAccess Access;
  TInt LastFId;
  PWebFilter WebFilter;
  bool IndexTxtBsP;
  PUrlStrToFIdH UrlStrToFIdH;
  TBlobPtQ UrlStrIdQ;
  THash<TBlobPt, PSockEvent> UrlStrIdToSockEventH;
  THash<TStr, TWebBsFetchHostDesc> HostNmToDescH;
private:
  int GetNextFId(){LastFId++; return LastFId;}

  void DefFinalHostDesc(const TStr& AliasHostNm, const TStr& FinalHostNm);
  TWebBsFetchHostDesc& GetAsFinalHostDesc(const PUrl& Url);

  void EnqueueUrl(const int& FId, const PUrl& Url);
  void DequeueUrl(const TBlobPt& UrlStrId, const PUrl& Url);
  int FetchUrl(const PUrl& Url, const bool& GoFetching);

  bool DoStillFetchUrl(const PUrl& Url);
  TStr GetStatusStr(){
    return WebBs->GetFetchStatusStr();}

  void OnFetch(const int& FId, const PWebPg& WebPg){
    WebBs->OnFetch(FId, WebPg);}
  void OnInfo(const int& FId, const TStr& MsgStr){
    WebBs->OnInfo(FId, MsgStr);}
  void OnError(const int& FId, const TStr& ErrStr){
    WebBs->OnError(FId, ErrStr);}
  void OnInfo(const TStr& MsgStr){
    WebBs->OnInfo(MsgStr);}
  void OnStatus(const TStr& StatusStr){
    WebBs->OnStatus(StatusStr);}
public:
  TWebBsFetch(
   const TStr& Nm, const TStr& FPath, const TFAccess& _Access, TWebBs* _WebBs,
   const PWebFilter& _WebFilter=NULL, const bool& _IndexTxtBsP=false);
  ~TWebBsFetch();
  TWebBsFetch(TSIn&){Fail;}
  static PWebBsFetch Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TWebBsFetch& operator=(const TWebBsFetch&){Fail; return *this;}

  bool IsIndexTxtBs() const {return IndexTxtBsP;}

  bool SetUrlAsFinal(const PUrl& Url);
  PWebHostNmBs GetHostNmBs() const;

  int GetConns() const {return UrlStrIdToSockEventH.Len();}
  void GetHostNmConnsPrV(TStrIntPrV& HostNmConnsPrV) const;
  TStr GetHostNmConnsPrVStr() const;
  void GetHostNmQueuedPrV(TStrIntPrV& HostNmQueuedPrV) const;
  TStr GetHostNmQueuedPrVStr(const int& ShowLen) const;
  bool IsUrlEnqueued(const PUrl& Url) const {
    return UrlStrToFIdH->IsKey(Url->GetUrlStr());}
  int GetUrlQLen() const {return UrlStrIdQ.Len();}
  PWebFilter GetWebFilter() const {return WebFilter;}

  int AddFetchUrl(const PUrl& Url){
    return FetchUrl(Url, false);}
  int AddFetchUrl(const TStr& RelUrlStr, const TStr& BaseUrlStr=TStr()){
    PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
    return FetchUrl(Url, false);}
  int GoFetchUrl(const TStr& RelUrlStr, const TStr& BaseUrlStr=TStr()){
    PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
    return FetchUrl(Url, true);}
  void GoFetch();

  static TStr GetFExt(){return WebBsFetchFExt;}
  static void GetFNms(
   const TStr& Nm, const TStr& FPath,
   TStr& WebBsFetchFNm, TStr& WebBsFetchUrlFNm, TStr& WebHostBsFNm);
  static bool Exists(const TStr& Nm, const TStr& FPath=TStr());
  static void Del(const TStr& Nm, const TStr& FPath=TStr());

  friend class TWebBsFetchEvent;
};

