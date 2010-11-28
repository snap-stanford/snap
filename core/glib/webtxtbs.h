#ifndef webtxtbs_h
#define webtxtbs_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "mine.h"
#include "graph.h"

/////////////////////////////////////////////////
// Forward
ClassHdTP(TWebTxtBs, PWebTxtBs);

/////////////////////////////////////////////////
// Web-Text-Base-Terminal-Server
class TWebTxtBsTrmSrv: public TTrmSrv{
private:
  TWebTxtBs* WebTxtBs;
  void PutWebTxtBsPp(const TStr& PpNm, const TStr& PpVal) const;
  void GetWebTxtBsPp_FPath(TStr& FPath) const;
  void GetWebTxtBsPp_FPath_Nm(TStr& FPath, TStr& Nm) const;
  void GetWebTxtBsPp_WebFilterFNm(TStr& FNm) const;
public:
  TWebTxtBsTrmSrv(TWebTxtBs* _WebTxtBs):
    TTrmSrv(7777), WebTxtBs(_WebTxtBs){}
  ~TWebTxtBsTrmSrv(){}

  TStr GetHelpStr() const;
  void ParseAndExeCmLn(
   const int& CltSockId, TILx& Lx, bool& SendMsgToClt, TChA& MsgChA);

  void OnLn(const int& CltSockId, const TStr& Ln);
  void OnTxt(const int& /*CltSockId*/, const TStr& /*Ln*/){}
};

/////////////////////////////////////////////////
// Web-Text-Base
ClassTPE(TWebTxtBs, PWebTxtBs, TWebBs)//{
private:
  PNotify Notify;
  PTrmSrv TrmSrv;
  PPp Pp;
  PTxtBs TxtBs;
  PWebBsFetch WebBsFetch;
  void SendToNotifyAndTrm(const TStr& MsgStr, const bool& LnAtTrm=true);
  void SendToStatusAndTrm(const TStr& MsgStr, const bool& LnAtTrm=true);
public: // properties
  static TStr PpNm_WebTxtBs;
  static TStr PpNm_WebTxtBsFPath;
  static TStr PpNm_WebTxtBsNm;
  static TStr PpNm_WebFilterFNm;
  PPp GetPp() const {return Pp;}
public:
  TWebTxtBs(const PNotify& _Notify=NULL);
  static PWebTxtBs New(const PNotify& Notify=NULL){
    return PWebTxtBs(new TWebTxtBs(Notify));}
  ~TWebTxtBs(){}
  TWebTxtBs(TSIn&){Fail;}
  static PWebTxtBs Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TWebTxtBs& operator=(const TWebTxtBs&){Fail; return *this;}
  int GetMemUsed(){return TxtBs->GetMemUsed();}

  // component retrieval
  void PutNotify(const PNotify& _Notify){Notify=_Notify;}
  PNotify GetNotify() const {return Notify;}
  PTrmSrv GetTrmSrv() const {return TrmSrv;}
  PTxtBs GetTxtBs() const {return TxtBs;}

  // text base
  TStr GetNrNm() const {IAssert(IsTxtBsActive()); return TxtBs->GetNrNm();}
  TStr GetNrFPath() const {IAssert(IsTxtBsActive()); return TxtBs->GetNrFPath();}
  bool IsTxtBsActive() const {return !TxtBs.Empty();}
  void TxtBsNew(const TStr& Nm, const TStr& FPath);
  void TxtBsOpenForUpdate(const TStr& Nm, const TStr& FPath);
  void TxtBsOpenForRdOnly(const TStr& Nm, const TStr& FPath);
  void TxtBsDel(const TStr& Nm, const TStr& FPath);
  void TxtBsClose(const bool& IsClr=false);

  // fetching
  bool IsFetchActive() const {return !WebBsFetch.Empty();}
  bool IsFetchEnd() const;
  void FetchStart(const TStr& Nm, const TStr& FPath,
   const TStr& WebFilterFNm, const bool& IndexTxtBsP=false);
  void FetchContinue(const TStr& Nm, const TStr& FPath);
  void FetchStop(const bool& IsClr=false);

  // status
  bool IsUrlStr(const TStr& UrlStr){
    return TxtBs->IsDoc(UrlStr);}
  TStr GetFetchStatusStr();
  TStr GetStatusStr();
  void ReportStatus();
  void ReportSocks();
  void ReportHosts();
  void ReportQueue(const int& ShowLen);

  // searching
  PTxtBsRes Search(const TStr& QueryStr){
    return TxtBs->Search(QueryStr);}
  void ReportSearch(const TStr& QueryStr);

  // events
  void OnFetch(const int& /*FId*/, const PWebPg& WebPg);
  void OnInfo(const int& /*FId*/, const TStr& MsgStr){
    TNotify::OnNotify(Notify, ntInfo, MsgStr);}
  void OnError(const int& /*FId*/, const TStr& ErrStr){
    TNotify::OnNotify(Notify, ntErr, ErrStr);}
  void OnInfo(const TStr& MsgStr){
    TNotify::OnNotify(Notify, ntInfo, MsgStr);}
  void OnStatus(const TStr& StatusStr){
    TNotify::OnStatus(Notify, StatusStr);}

  // web host-name base
  PWebHostNmBs GetWebHostNmBs() const {
    TStr FNm=GetNrFPath()+GetNrNm()+TWebBsFetch::WebHostBsFExt;
    return TWebHostNmBs::LoadTxt(FNm);}

  // graph
  //PGraph GetGraph() const;

  // saving
  void SaveTxt(const TStr& FNm){SaveTxt(PSOut(new TFOut(FNm)));}
  void SaveTxt(const PSOut& SOut);

  static TStr GetFExt(){return TTxtBs::GetFExt();}
};

/////////////////////////////////////////////////
// Web-Text-Base-Server
typedef enum {wstUndef, wstSearchHd, wstSearchFt} TWebTxtBsSrvTempl;

class TWebTxtBsSrv: public TWebSrv{
private:
  // template macro names
  static const TStr HostNmMacro;
  static const TStr PortMacro;
  static const TStr QueryMacro;
  static const TStr HitsMacro;
  static const TStr HitNumMacro;
  static const TStr DocAddrMacro;
  static const TStr DocTitleMacro;
  static const TStr DocCtxMacro;
  static const TStr UrlMacro;
  static const TStr NmMacro;
  // url field names
  static const TStr QueryUrlFldNm;
  static const TStr HitSetUrlFldNm;
  static const TStr AcceptUrlFldNm;
  // file extensions
  static const TStr TplFExt;
  static const TStr QueryLogFExt;
  // file names
  static const TStr DfTplFNm;
private:
  PNotify Notify;
  PWebTxtBs WebTxtBs;
  TStr HostNm;
  TStr PortNStr;
  TDbStrStrH TplNmToValH;
  TDbStrStrH VarNmToValH;
  PSOut SLog;
private:
  TStr GetRqContType(const PHttpRq& HttpRq) const;
  TStr GetDfTplSetStr() const;
  void PrepTplSet(const TStr& FNm, const TStr& DfFNm);
  void AddTpl(const TStr& ContTypeStr, const TStr& CatStr, const TStr TplStr){
    TplNmToValH.AddDat(TDbStr(ContTypeStr, CatStr), TplStr);}
  TStr GetTplVal(const TStr& ContTypeStr, const TStr& CatStr) const {
    return TplNmToValH.GetDat(TDbStr(ContTypeStr, CatStr));}
  void AddVar(const TStr& ContTypeStr, const TStr& VarNm, const TStr& VarVal){
    VarNmToValH.AddDat(TDbStr(ContTypeStr, VarNm), VarVal);}
  TStr GetVarVal(const TStr& ContTypeStr, const TStr& VarNm) const {
    return VarNmToValH.GetDat(TDbStr(ContTypeStr, VarNm));}
  void AddHitSetChA(
   const PTxtBsRes& TxtBsRes, const TStr& RqContTypeStr,
   const int& HitSetN, const int& HitSetDocs, const int& StrHitSets,
   const PUrlEnv& UrlEnv, TChA& OutChA);
public:
  TWebTxtBsSrv(
   const PWebTxtBs& _WebTxtBs, const int& PortN, const PNotify& _Notify=NULL);
  static PWebSrv New(
   const PWebTxtBs& WebTxtBs, const int& PortN, const PNotify& Notify=NULL){
    return PWebSrv(new TWebTxtBsSrv(WebTxtBs, PortN, Notify));}

  void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);

  static void GetFNms(
   const TStr& Nm, const TStr& FPath, TStr& TplFNm, TStr& QueryLogFNm);
};

#endif
