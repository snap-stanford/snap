//////////////////////////////////////////////////
// Web-Net-Timer
class TWebNetClt;

class TWebNetTimer: public TTTimer{
private:
  TWebNetClt* WebNetClt;
  UndefDefaultCopyAssign(TWebNetTimer);
public:
  TWebNetTimer(TWebNetClt* _WebNetClt):
    TTTimer(1000), WebNetClt(_WebNetClt){}
  static PTimer New(TWebNetClt* WebNetClt){
    return new TWebNetTimer(WebNetClt);}

  void OnTimeOut();
};

/////////////////////////////////////////////////
// Web-Net-Server
ClassTPE(TWebNetSrv, PWebNetSrv, TNetSrv)//{
private:
  PNotify Notify;
  UndefDefaultCopyAssign(TWebNetSrv);
public:
  TWebNetSrv(const int& _PortN, const bool& FixedPortNP=true, const PNotify& _Notify=NULL);
  virtual ~TWebNetSrv(){}

  void OnNetObj(const PNetObj& NetObj);
  void OnNetMem(const PNetObj& NetObj);

  virtual void OnHttpRq(const int& SockId, const PHttpRq& HttpRq)=0;
  virtual void OnHttpRqError(const int& SockId, const TStr& MsgStr)=0;
  void SendHttpResp(const int& SockId, const PHttpResp& HttpResp);
};

/////////////////////////////////////////////////
// Web-Net-Client
class TWebNetCltV;

ClassTPE(TWebNetClt, PWebNetClt, TNetClt)//{
private:
  typedef TTriple<TInt, PHttpRq, TTm> TFetchIdHttpRqCreateTmTr;
  typedef TPair<TInt, TTm> TFetchIdCreateTmPr;
  PNotify Notify;
  TWebNetCltV* WebNetCltV;
  bool ConnectedP;
  TQQueue<TFetchIdHttpRqCreateTmTr> Wait_FetchIdHttpRqCreateTmTrQ;
  TQQueue<TFetchIdCreateTmPr> Sent_FetchIdCreateTmPrQ;
  PTimer Timer;
  TStr ConnIdStr; // "host:port#client" string based on GetHostPortStr() for hashing
  UndefDefaultCopyAssign(TWebNetClt);
public:
  TWebNetClt(const TStr& HostNm, const int& PortN,
   TWebNetCltV* _WebNetCltV, const TStr& _IdStr, const PNotify& _Notify=NULL);
  static PWebNetClt New(const TStr& HostNm, const int& PortN,
   TWebNetCltV* WebNetCltV, const TStr& ConnIdStr, const PNotify& Notify=NULL){
    return PWebNetClt(new TWebNetClt(HostNm, PortN, WebNetCltV, ConnIdStr, Notify));}
  ~TWebNetClt();

  TStr GetConnIdStr() const {return ConnIdStr;}
  static TStr GetConnIdStr(const TStr& HostNm, const int& PortN, const TStr& Client){
    return TStr::Fmt("%s:%d#%s", HostNm.CStr(), PortN,Client.CStr());}

  void OnNetObj(const PNetObj& NetObj);
  void OnNetMem(const PNetObj& NetObj);

  virtual void OnHttpResp(const PHttpResp& HttpResp);
  void SendHttpRq(const PHttpRq& HttpRq);
  void SendWaitQ();
  void OnCheckTimeOut();

  bool IsConnected() const {return ConnectedP;}
  void OnDisconn(const TStr& MsgStr);

  // wait-queue
  bool IsWaitQEmpty() const {return Wait_FetchIdHttpRqCreateTmTrQ.Empty();}
  void PushToWaitQ(const int& FetchId, const PHttpRq& HttpRq){
    Wait_FetchIdHttpRqCreateTmTrQ.Push(
     TFetchIdHttpRqCreateTmTr(FetchId, HttpRq, TTm::GetCurUniTm()));}
  void TopFromWaitQ(int& FetchId, PHttpRq& HttpRq, TTm& CreateTm){
    FetchId=Wait_FetchIdHttpRqCreateTmTrQ.Top().Val1;
    HttpRq=Wait_FetchIdHttpRqCreateTmTrQ.Top().Val2;
    CreateTm=Wait_FetchIdHttpRqCreateTmTrQ.Top().Val3;}
  void PopFromWaitQ(){
    Wait_FetchIdHttpRqCreateTmTrQ.Pop();}
  void PopFromWaitQ(int& FetchId, PHttpRq& HttpRq, TTm& CreateTm){
    TopFromWaitQ(FetchId, HttpRq, CreateTm);
    PopFromWaitQ();}

  // sent-queue
  bool IsSentQEmpty() const {return Sent_FetchIdCreateTmPrQ.Empty();}
  void PushToSentQ(const int& FetchId){
    Sent_FetchIdCreateTmPrQ.Push(
     TFetchIdCreateTmPr(FetchId, TTm::GetCurUniTm()));}
  void TopFromSentQ(int& FetchId, TTm& CreateTm){
    FetchId=Sent_FetchIdCreateTmPrQ.Top().Val1;
    CreateTm=Sent_FetchIdCreateTmPrQ.Top().Val2;}
  void PopFromSentQ(){
    Sent_FetchIdCreateTmPrQ.Pop();}
  void PopFromSentQ(int& FetchId, TTm& CreateTm){
    TopFromSentQ(FetchId, CreateTm);
    PopFromSentQ();}
};

/////////////////////////////////////////////////
// Web-Net-Client-Vector
ClassTP(TWebNetCltV, PWebNetCltV)//{
private:
  PNotify Notify;
  THash<TStr, PWebNetClt> ConnIdStrToWebNetCltH;
  //!bool FetchingP;
  int LastFetchId;
  int CltTimeout;
  int GetNextFetchId(){
    LastFetchId++; if (LastFetchId<=0){LastFetchId=1;} return LastFetchId;}
  UndefCopyAssign(TWebNetCltV);
public:
  TWebNetCltV(const PNotify& Notify=NULL);
  virtual ~TWebNetCltV(){}

  virtual void OnHttpResp(const int& FetchId, const PHttpResp& HttpResp)=0;
  virtual void OnHttpRespError(const int& FetchId, const TStr& MsgStr)=0;
  void SendHttpRq(const TStr& ConnIdStr, const PHttpRq& HttpRq);
  int FetchUrl(const PUrl& Url, const int& _FetchId=-1, TStr ConnUid=TStr());

  bool IsGetWebNetClt(const TStr& ConnIdStr, PWebNetClt& WebNetClt) const {
    return ConnIdStrToWebNetCltH.IsKeyGetDat(ConnIdStr, WebNetClt);}
  void AddWebNetClt(const TStr& ConnIdStr, const PWebNetClt& WebNetClt){
    ConnIdStrToWebNetCltH.AddDat(ConnIdStr, WebNetClt);}
  void DelIfWebNetClt(const TStr& ConnIdStr){
    ConnIdStrToWebNetCltH.DelIfKey(ConnIdStr);}
  //bool IsFetching() const {return FetchingP;}
  
  static const int DfCltTimeout = 10000;
  int GetCltTimeout() const {return CltTimeout;}
  void SetCltTimeout(const int &Timeout) {CltTimeout=Timeout;}
};


/////////////////////////////////////////////////
// Web-Net-Proxy-Connection: client return information
class TWebNetProxy;
ClassTP(TProxyConn, PProxyConn) //{
private:
  int RqSockId;
  int RqFetchId, RespFetchId;
public:
  TProxyConn(int _RqSockId, int _RqFetchId, int _RespFetchId):
    RqSockId(_RqSockId), RqFetchId(_RqFetchId), RespFetchId(RespFetchId) {}
  void SendHttpResp(TWebNetSrv &WebNetSrv, PHttpResp HttpResp);
  int GetRqSockId() const {return RqSockId;}
  int GetRqFetchId() const {return RqFetchId;}
  int GetRespFetchId() const {return RespFetchId;}
};

/////////////////////////////////////////////////
// Web-Net-Proxy
ClassTPEE(TWebNetProxy, PWebNetProxy, TWebNetSrv, TWebNetCltV)//{
private:
  THash<TInt, PProxyConn> RqSockIdToProxyH;
  THash<TInt, PProxyConn> RespFetchIdToProxyH;
  TStr DfSrv;
public:
  TWebNetProxy(const int &PortN, const TStr &DefaultServer=TStr()) :
   TWebNetSrv(PortN, true), DfSrv(DefaultServer) {SetCltTimeout(GetCltTimeout()-2000);}
  // http request
  void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
  void OnHttpRqError(const int& SockId, const TStr& MsgStr);
  // http response
  void OnHttpResp(const int& FId, const PHttpResp& HttpResp);
  void OnHttpRespError(const int& FId, const TStr& _MsgStr);
  // default port-number setting
};

