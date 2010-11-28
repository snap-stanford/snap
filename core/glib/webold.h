#ifndef webold_h
#define webold_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "net.h"

/////////////////////////////////////////////////
// Web-Page
class TXWebPg;
typedef TPt<TXWebPg> PXWebPg;

class TXWebPg{
private:
  TCRef CRef;
private:
  static const char SepCh;
  TStrV UrlStrV;
  TStrV RefUrlStrV;
  PHttpResp HttpResp;
public:
  TXWebPg(): UrlStrV(), HttpResp(){}
  TXWebPg(const PSIn& SIn);
  TXWebPg(const TStrV& _UrlStrV, const TStrV& _RefUrlStrV,
   const PHttpResp& _HttpResp):
    UrlStrV(_UrlStrV), RefUrlStrV(_RefUrlStrV), HttpResp(_HttpResp){}
  ~TXWebPg(){}
  TXWebPg(TSIn&){Fail;}
  static PXWebPg Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TXWebPg& operator=(const TXWebPg&){Fail; return *this;}

  int GetUrls(){return UrlStrV.Len();}
  TStr GetUrlStr(const int& UrlN=0){return UrlStrV[UrlN];}
  TStr GetLastUrlStr(){return UrlStrV[GetUrls()-1];}
  TStrV GetUrlStrV(){return UrlStrV;}
  TStrV GetRefUrlStrV(){return RefUrlStrV;}
  PHttpResp GetHttpResp(){return HttpResp;}
  TStr GetBodyStr(){return GetHttpResp()->GetBodyAsStr();}

  bool IsText();
  static PXWebPg LoadTxt(const PSIn& SIn){return new TXWebPg(SIn);}
  static PXWebPg LoadTxt(const PSIn& SIn, const TStrV& UrlStrV,
   const TStrV& RefUrlStrV);
  void SaveTxt(const PSOut& SOut);

  friend PXWebPg;
};

/////////////////////////////////////////////////
// Web-Fetch
class TXWebFetch{
private:
  static const TStr WebFetchFNm;
  static const int ExpectedUnconns;
  static const int ExpectedConns;
  TInt MxConns;
  TInt MxSize;
  TInt LastWebPgId;
  TQQueue<TIntStrPr> UnconnWebPgIdToUrlStrQ;
  TIntStrH ConnWebPgIdToUrlStrH;
  TStrIntH ConnWebPgUrlStrToIdH;
  THash<TInt, PSockEvent> ConnWebPgIdToSockEventH;
public:
  TXWebFetch(const int& _MxConns=100):
    MxConns(_MxConns), MxSize(-1), LastWebPgId(0),
    UnconnWebPgIdToUrlStrQ(ExpectedUnconns),
    ConnWebPgIdToUrlStrH(ExpectedConns),
    ConnWebPgUrlStrToIdH(ExpectedConns),
    ConnWebPgIdToSockEventH(ExpectedConns){}
  TXWebFetch(const TStr& FPath);
  void SaveTxt(const TStr& FPath);
  virtual ~TXWebFetch();
  TXWebFetch(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  TXWebFetch& operator=(const TXWebFetch&){Fail; return *this;}

  void ConnWebPg(const int& WebPgId=-1, const TStr& UrlStr=TStr());
  void UnconnWebPg(const int& WebPgId);
  void Continue(){ConnWebPg();}
  void PutMxSize(const int& _MxSize){MxSize=_MxSize;}
  int GetUnconns(){return UnconnWebPgIdToUrlStrQ.Len();}
  int GetConns(){return ConnWebPgIdToUrlStrH.Len();}
  int GetNextWebPgId(){LastWebPgId++; return LastWebPgId;}

  virtual void OnFetch(const int& WebPgId, const PXWebPg& WebPg)=0;
  virtual void OnNotify(const int& WebPgId, const TStr& MsgStr)=0;
  virtual void OnError(const int& WebPgId, const TStr& ErrStr)=0;
  virtual void OnStatus(const TStr& StatusStr)=0;

  int FetchWebPg(const TStr& RelUrlStr, const TStr& BaseUrlStr);
  virtual bool DoStillFetchUrl(const TStr& UrlStr)=0;
  virtual bool DoFetchRedirUrl(const PUrl& RedirUrl, const TUrlV& UrlV)=0;
  virtual bool IsCheckMode()=0;
  virtual bool IsWebPg(const TStr& UrlStr)=0;
  virtual bool IsWebPg(const TStr& UrlStr, const bool& Check)=0;
  virtual PXWebPg GetWebPg(const int& WebPgId)=0;
  virtual PXWebPg GetWebPg(const TStr& UrlStr)=0;
  virtual TStr GetStatusStr()=0;
};

/////////////////////////////////////////////////
// Web-Base-Event
class TXWebBsEvent{
private:
  TCRef CRef;
public:
  TXWebBsEvent(){}
  virtual ~TXWebBsEvent(){}
  TXWebBsEvent(TSIn&){Fail;}
  static TPt<TXWebBsEvent> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TXWebBsEvent& operator=(const TXWebBsEvent&){Fail; return *this;}

  virtual void OnWebPg(const int& WebPgId, const PXWebPg& WebPg,
   bool& DoSave)=0;
  virtual void OnNotify(const int& WebPgId, const TStr& MsgStr)=0;
  virtual void OnError(const int& WebPgId, const TStr& ErrStr)=0;
  virtual void OnStatus(const TStr& StatusStr)=0;

  friend TPt<TXWebBsEvent>;
};
typedef TPt<TXWebBsEvent> PXWebBsEvent;

/////////////////////////////////////////////////
// Web-Base
class TXWebBs: public TXWebFetch{
private:
  TCRef CRef;
private:
  static const TStr WebBsFNm;
  TStr FPath;
  bool CheckMode;
  bool GenRef;
  PXWebBsEvent WebBsEvent;
public:
  TXWebBs(const int& MxConns, const TStr& _FPath, const bool& _GenRef):
    TXWebFetch(MxConns), FPath(TStr::GetNrFPath(_FPath)),
    CheckMode(false), GenRef(_GenRef){}
  TXWebBs(const TStr& _FPath, const bool& _CheckMode);
  virtual void SaveTxt();
  virtual ~TXWebBs(){}
  TXWebBs(TSIn&){Fail;}
  static TPt<TXWebBs> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TXWebBs& operator=(const TXWebBs&){Fail; return *this;}

  TStr GetFPath(){return FPath;}
  bool IsCheckMode(){return CheckMode;}
  bool IsGenRef(){return GenRef;}
  void PutWebBsEvent(const PXWebBsEvent& _WebBsEvent){
    WebBsEvent=_WebBsEvent;}
  TStr GetStatusStr();

  virtual int GetWebPgs()=0;
  virtual int GetCheckedUrls()=0;
  virtual int AddWebPg(const TStr& RelUrlStr, const TStr& BaseUrlStr=TStr())=0;
  virtual void AddWebPgRef(const TStr& UrlStr, const TStr& RefUrlStr)=0;
  virtual bool DoStillFetchUrl(const TStr& UrlStr)=0;

  virtual bool IsUrlStr(const TStr& UrlStr)=0;
  virtual int GetUrlStrs(const int& WebPgId)=0;
  virtual TStr GetUrlStr(const int& WebPgId, const int& UrlStrN=0)=0;
  virtual TStr GetUrlStr(const TStr& UrlStr)=0;
  virtual int GetWebPgId(const TStr& UrlStr)=0;

  virtual int FFirstWebPg()=0;
  virtual bool FNextWebPg(int& WebPgP, int& WebPgId)=0;

  void OnWebPg(const int& WebPgId, const PXWebPg& WebPg, bool& DoSave){
    if (WebBsEvent.Empty()){DoSave=true;}
    else {WebBsEvent->OnWebPg(WebPgId, WebPg, DoSave);}}
  void OnNotify(const int& WebPgId, const TStr& MsgStr){
    if (!WebBsEvent.Empty()){WebBsEvent->OnNotify(WebPgId, MsgStr);}}
  void OnError(const int& WebPgId, const TStr& ErrStr){
    if (!WebBsEvent.Empty()){WebBsEvent->OnError(WebPgId, ErrStr);}}
  void OnStatus(const TStr& StatusStr){
    if (!WebBsEvent.Empty()){WebBsEvent->OnStatus(StatusStr);}}

  TStr GetBodyStr(const int& WebPgId){
    return GetWebPg(WebPgId)->GetBodyStr();}
  TStr GetBodyStr(const TStr& UrlStr){
    return GetBodyStr(GetWebPgId(UrlStr));}

  static TPt<TXWebBs> LoadTxt(const TStr& FPath);

  friend TPt<TXWebBs>;
};
typedef TPt<TXWebBs> PXWebBs;

/////////////////////////////////////////////////
// Web-Travel-Event
class TXWebTravelEvent{
private:
  TCRef CRef;
public:
  TXWebTravelEvent(){}
  virtual ~TXWebTravelEvent(){}
  TXWebTravelEvent(TSIn&){Fail;}
  static TPt<TXWebTravelEvent> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TXWebTravelEvent& operator=(const TXWebTravelEvent&){Fail; return *this;}

  virtual void OnBusy(const PXWebPg& WebPg, const bool& DoSave)=0;
  virtual void OnFree()=0;
  virtual void OnNotify(const TStr& ErrStr)=0;
  virtual void OnStatus(const TStr& StatusStr)=0;

  friend TPt<TXWebTravelEvent>;
};
typedef TPt<TXWebTravelEvent> PXWebTravelEvent;

/////////////////////////////////////////////////
// Web-Travel
typedef enum {wtmStart, wtmCont, wtmCheck} TXWebTravelMode;

class TXWebTravelBaseEvent;
class TXWebTravel{
private:
  TCRef CRef;
private:
  TStrIntH BadFExtH;
  bool SingleFetch;
  TStrPrV HostPathStrV;
  PXWebBsEvent WebBsEvent;
  PXWebBs WebBs;
  PXWebTravelEvent Event;
  void AddBadFExt(const TStr& BadFExt){BadFExtH.AddKey(BadFExt);}
  void GenBadFExtH();
private:
  void OnWebPg(const int& WebPgId, const PXWebPg& WebPg, bool& DoSave);
  void OnNotify(const int&, const TStr& MsgStr){OnNotify(MsgStr);}
  void OnError(const int&, const TStr& ErrStr){OnNotify(ErrStr);}
protected:
  void ClrConstr(){
    HostPathStrV.Clr(); WebBs->PutMxSize(-1);}
  void PutSingleFetchConstr(const bool& _SingleFetch){
    SingleFetch=_SingleFetch;}
  void AddHostPathConstr(const TStr& HostStr, const TStr& PathStr){
    HostPathStrV.Add(TStrPr(HostStr, PathStr));}
  void AddSizeConstr(const int& MxSize){WebBs->PutMxSize(MxSize);}
protected:
  void PutWebBs(const PXWebBs& _WebBs){
    WebBs=_WebBs; WebBs->PutWebBsEvent(WebBsEvent);}
  void PutEvent(const PXWebTravelEvent& _Event){Event=_Event;}
public:
  TXWebTravel(const PXWebBs& _WebBs=NULL, const PXWebTravelEvent& _Event=NULL);
  virtual ~TXWebTravel(){}
  TXWebTravel(TSIn&){Fail;}
  static TPt<TXWebTravel> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TXWebTravel& operator=(const TXWebTravel&){Fail; return *this;}

  TStr GetStatusStr(){return WebBs->GetStatusStr();}

  bool IsUrlTok(const PHtmlTok& Tok, TStr& RelUrlStr);
  bool IsUrlOk(const TStr& RelUrlStr, const TStr& BaseUrlStr=TStr());

  void Go(){WebBs->Continue();}
  void Go(const TStr& UrlStr){WebBs->AddWebPg(UrlStr);}

  void OnBusy(const PXWebPg& WebPg, const bool& DoSave){
    if (!Event.Empty()){Event->OnBusy(WebPg, DoSave);}}
  void OnFree(){
    if (!Event.Empty()){Event->OnFree();}}
  void OnNotify(const TStr& MsgStr){
    if (!Event.Empty()){Event->OnNotify(MsgStr);}}
  void OnStatus(const TStr& StatusStr){
    if (!Event.Empty()){Event->OnStatus(StatusStr);}}

  virtual void StartTravel(){Fail;}
  virtual void ContTravel(){Fail;}
  virtual void CheckTravel(){Fail;}

  friend TXWebTravelBaseEvent;
  friend TPt<TXWebTravel>;
};
typedef TPt<TXWebTravel> PXWebTravel;

#endif
