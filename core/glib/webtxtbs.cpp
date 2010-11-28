/////////////////////////////////////////////////
// Includes
//#include "stdafx.h"
#include "webtxtbs.h"

/////////////////////////////////////////////////
// Web-Text-Base-Terminal-Server
void TWebTxtBsTrmSrv::PutWebTxtBsPp(const TStr& PpNm, const TStr& PpVal) const {
  WebTxtBs->GetPp()->PutValStr(PpNm, PpVal);
}

void TWebTxtBsTrmSrv::GetWebTxtBsPp_FPath(TStr& FPath) const {
  FPath=WebTxtBs->GetPp()->GetValStr(TWebTxtBs::PpNm_WebTxtBsFPath);
  FPath=TStr::GetNrFPath(FPath);
}

void TWebTxtBsTrmSrv::GetWebTxtBsPp_FPath_Nm(TStr& FPath, TStr& Nm) const {
  GetWebTxtBsPp_FPath(FPath);
  Nm=WebTxtBs->GetPp()->GetValStr(TWebTxtBs::PpNm_WebTxtBsNm);
}

void TWebTxtBsTrmSrv::GetWebTxtBsPp_WebFilterFNm(TStr& FNm) const {
  FNm=WebTxtBs->GetPp()->GetValStr(TWebTxtBs::PpNm_WebFilterFNm);
}

TStr TWebTxtBsTrmSrv::GetHelpStr() const {
  return
   "TRALALA - the well known Tralala command\r\n"
   "SYSECHO [ACCEPT|CLOSE|String] - write welcome/nothing/String\r\n"
   "ECHO String - write String\r\n"
   "HELP - this help\r\n"
   "MEM - show memory status\r\n"
   "QUIT - quit the session\r\n"
   "SET [field [value]] - set/show the value of field(s)\r\n"
   "NEW - create new web-base from settings and make it active\r\n"
   "OPEN - open existing web-base from settings and make it active\r\n"
   "CLOSE - close active web-base\r\n"
   "DEL - delete web-base from settings\r\n"
   "START - start fetching of active web-base\r\n"
   "STOP - stop fetching of active web-base\r\n"
   "CONT - continue fetching of active web-base\r\n"
   "GO FilterName - create web-base and start fetching\r\n"
   "FETCH \"Domain\" [Segments] - create filter and web-base and start fetching Domain\r\n"
   "STAT - show status of active web-base and fetching\r\n"
   "SOCKS - show status of active sockets\r\n"
   "HOSTS - show active hosts\r\n"
   "QUEUE [length] - show waiting hosts\r\n"
   "SEARCH Query - activates Query on the active web-base and write results\r\n";
}

void TWebTxtBsTrmSrv::ParseAndExeCmLn(
 const int& CltSockId, TILx& Lx, bool& SendMsgToClt, TChA& MsgChA){
  SendMsgToClt=true;
  if (Lx.UcStr=="TRALALA"){
    MsgChA+=TStr("Hopsasa")+Lx.GetStrToEoln()+"!\r\n";
  } else
  if (Lx.UcStr=="SYSECHO"){
    TStr SysMsgStr=Lx.GetStrToEoln(true).GetUc();
    if (SysMsgStr=="ACCEPT"){
      MsgChA+="\r\n";
      MsgChA+=TStr("      Welcome at WebBird/WebFly Index Engine (V1.0)!\r\n");
      //MsgChA+=TStr("          From:")+GetPeerIpNum()(CltSockId)+"\r\n";
      MsgChA+=TStr("          Time:")+TSecTm::GetCurTm().GetStr()+"\r\n";
      MsgChA+="\r\n";
    } else
    if (SysMsgStr=="CLOSE"){
      SendMsgToClt=false;
    } else {
      MsgChA+=SysMsgStr; MsgChA+="\r\n";
    }
  } else
  if (Lx.UcStr=="ECHO"){
    MsgChA+=Lx.GetStrToEoln(true);
  } else
  if (Lx.UcStr=="HELP"){
    Lx.GetSym(syEof);
    MsgChA+=GetHelpStr();
  } else
  if (Lx.UcStr=="MEM"){
    Lx.GetSym(syEof);
    PSysMemStat MemStat=PSysMemStat(new TSysMemStat());
    MsgChA+=MemStat->GetInfoStr();
  } else
  if (Lx.UcStr=="QUIT"){
    Lx.GetSym(syEof);
    CloseClt(CltSockId);
    SendMsgToClt=false;
  } else
  if (Lx.UcStr=="SET"){
    PPp WebTxtBsPp=WebTxtBs->GetPp();
    Lx.GetSym(syIdStr, syEof);
    if (Lx.Sym==syIdStr){
      TStr PpNm=Lx.UcStr; PPp Pp;
      if (WebTxtBsPp->IsPp(PpNm, Pp)){
        Lx.GetSym(syIdStr, syQStr, syInt, syEof);
        if (Lx.Sym==syEof){
          MsgChA+=Pp->GetStr();
        } else
        if (((Lx.Sym==syIdStr)||(Lx.Sym==syQStr))&&(Pp->GetTag()==ptStr)){
          Pp->PutValStr(Lx.Str);
        } else
        if ((Lx.Sym==syInt)&&(Pp->GetTag()==ptInt)){
          Pp->PutValInt(Lx.Int);
        } else {
          MsgChA+="Error: Bad value type.\r\n";
        }
      } else {
        MsgChA+=TStr("Error: Property '")+PpNm+"' not defined.\r\n";
      }
    } else
    if (Lx.Sym==syEof){
      MsgChA+=WebTxtBsPp->GetStr();
    } else {
      Fail;
    }
  } else
  if (Lx.UcStr=="NEW"){
    Lx.GetSym(syEof);
    TStr FPath; TStr Nm; GetWebTxtBsPp_FPath_Nm(FPath, Nm);
    WebTxtBs->TxtBsNew(Nm, FPath);
  } else
  if (Lx.UcStr=="OPEN"){
    Lx.GetSym(syEof);
    TStr FPath; TStr Nm; GetWebTxtBsPp_FPath_Nm(FPath, Nm);
    WebTxtBs->TxtBsOpenForUpdate(Nm, FPath);
  } else
  if (Lx.UcStr=="CLOSE"){
    Lx.GetSym(syEof);
    WebTxtBs->TxtBsClose();
  } else
  if (Lx.UcStr=="DEL"){
    Lx.GetSym(syEof);
    TStr FPath; TStr Nm; GetWebTxtBsPp_FPath_Nm(FPath, Nm);
    WebTxtBs->TxtBsDel(Nm, FPath);
  } else
  if (Lx.UcStr=="START"){
    Lx.GetSym(syEof);
    TStr FPath; TStr Nm; GetWebTxtBsPp_FPath_Nm(FPath, Nm);
    TStr WebFilterFNm; GetWebTxtBsPp_WebFilterFNm(WebFilterFNm);
    WebTxtBs->FetchStart(Nm, FPath, WebFilterFNm);
  } else
  if (Lx.UcStr=="STOP"){
    Lx.GetSym(syEof);
    WebTxtBs->FetchStop();
  } else
  if (Lx.UcStr=="CONT"){
    Lx.GetSym(syEof);
    TStr FPath; TStr Nm; GetWebTxtBsPp_FPath_Nm(FPath, Nm);
    WebTxtBs->FetchContinue(Nm, FPath);
  } else
  if (Lx.UcStr=="GO"){
    TStr WebFilterFNm=Lx.GetStrToEoln(true);
    WebFilterFNm=TStr::PutFExtIfEmpty(WebFilterFNm, TWebFilter::FExt);
    TStr WebTxtBsFPath=TStr::GetNrFPath(WebFilterFNm.GetFPath());
    TStr WebTxtBsNm=TStr::GetNrFMid(WebFilterFNm.GetFMid());
    WebTxtBs->TxtBsNew(WebTxtBsNm, WebTxtBsFPath);
    WebTxtBs->FetchStart(WebTxtBsNm, WebTxtBsFPath, WebFilterFNm);
  } else
  if (Lx.UcStr=="FETCH"){
    Lx.GetSym(syQStr); TStr ShortcutUrlStr=Lx.Str;
    Lx.GetSym(syInt, syEof); int MxDmSegs=-1;
    if (Lx.Sym==syInt){MxDmSegs=Lx.Int;}
    else {Lx.GetSym(syEof);}
    PUrl StartUrl=TUrl::GetUrlFromShortcut(ShortcutUrlStr, "www", "si");
    if (StartUrl->IsOk(usHttp)){
      // create filter
      PWebFilter WebFilter=PWebFilter(new TWebFilter());
      WebFilter->AddStartUrl(StartUrl->GetUrlStr());
      TStr DmConstrStr=StartUrl->GetDmNm(MxDmSegs);
      WebFilter->AddDmConstr(DmConstrStr);
      // create new web-text-base name
      TStr WebTxtBsFPath; GetWebTxtBsPp_FPath(WebTxtBsFPath);
      TStr WebTxtBsNm(DmConstrStr);
      WebTxtBsNm.ChangeStrAll(".", "-");
      PutWebTxtBsPp(TWebTxtBs::PpNm_WebTxtBsNm, WebTxtBsNm);
      // save filter to file
      TStr WebFilterFPath; GetWebTxtBsPp_FPath(WebFilterFPath);
      TStr WebFilterFNm=WebFilterFPath+WebTxtBsNm+TWebFilter::FExt;
      WebFilter->SaveTxt(WebFilterFNm);
      PutWebTxtBsPp(TWebTxtBs::PpNm_WebFilterFNm, WebFilterFNm);
      // create new web-text-base name
      WebTxtBs->TxtBsNew(WebTxtBsNm, WebTxtBsFPath);
      WebTxtBs->FetchStart(WebTxtBsNm, WebTxtBsFPath, WebFilterFNm);
    } else {
      MsgChA+=TStr("Error: Invalid url shortcut '")+ShortcutUrlStr+"'.\r\n";
    }
  } else
  if (Lx.UcStr=="STAT"){
    Lx.GetSym(syEof);
    WebTxtBs->ReportStatus();
  } else
  if (Lx.UcStr=="SOCKS"){
    Lx.GetSym(syEof);
    WebTxtBs->ReportSocks();
  } else
  if (Lx.UcStr=="HOSTS"){
    Lx.GetSym(syEof);
    WebTxtBs->ReportHosts();
  } else
  if (Lx.UcStr=="QUEUE"){
    Lx.GetSym(syInt, syEof); int ShowLen=10;
    if (Lx.Sym==syInt){ShowLen=Lx.Int;}
    else {Lx.GetSym(syEof);}
    WebTxtBs->ReportQueue(ShowLen);
  } else
  if (Lx.UcStr=="SEARCH"){
    TStr QueryStr=Lx.GetStrToEoln(true);
    WebTxtBs->ReportSearch(QueryStr);
  } else {
    MsgChA+="Error: Bad command keyword ("+Lx.UcStr+").\r\n";
  }
}

void TWebTxtBsTrmSrv::OnLn(const int& CltSockId, const TStr& Ln){
  TChA InNotifyChA;
  InNotifyChA+=TStr("[")+TInt::GetStr(CltSockId)+"] ";
  //InNotifyChA+=TStr("[")+GetCltPeerNm(CltSockId)+"] ";
  InNotifyChA+=TStr("[")+TSecTm::GetCurTm().GetStr()+"] ";
  InNotifyChA+=Ln;
  TNotify::OnNotify(WebTxtBs->GetNotify(), ntInfo, InNotifyChA);

  PSIn SIn=TMIn::New(Ln);
  TILx Lx(SIn, TFSet()|iloExcept);
  TChA MsgChA; bool SendMsgToClt=true;
  Lx.GetSym();
  if (Lx.Sym==syIdStr){
    try {
      ParseAndExeCmLn(CltSockId, Lx, SendMsgToClt, MsgChA);
    }
    catch (PExcept Except){
      MsgChA=TStr("Error: ")+Except->GetMsgStr()+"\r\n";
    }
  } else {
    if (Lx.Sym!=syEof){
      MsgChA="Error: No command keyword.\r\n";}
  }
  if (SendMsgToClt){
    SendTxtToClt(CltSockId, MsgChA);
    SendTxtToClt(CltSockId, ">");
  }
}

/////////////////////////////////////////////////
// Web-Text-Base
TStr TWebTxtBs::PpNm_WebTxtBs="WebTxtBs";
TStr TWebTxtBs::PpNm_WebTxtBsFPath="BPath";
TStr TWebTxtBs::PpNm_WebTxtBsNm="BName";
TStr TWebTxtBs::PpNm_WebFilterFNm="Filter";

void TWebTxtBs::SendToNotifyAndTrm(const TStr& MsgStr, const bool& LnAtTrm){
  TNotify::OnNotify(Notify, ntInfo, MsgStr);
  if (LnAtTrm){TNotify::OnLn(TrmSrv->GetBcstNotify(), MsgStr);}
  else {TNotify::OnTxt(TrmSrv->GetBcstNotify(), MsgStr);}
}

void TWebTxtBs::SendToStatusAndTrm(const TStr& MsgStr, const bool& LnAtTrm){
  TNotify::OnStatus(Notify, MsgStr);
  if (LnAtTrm){TNotify::OnLn(TrmSrv->GetBcstNotify(), MsgStr);}
  else {TNotify::OnTxt(TrmSrv->GetBcstNotify(), MsgStr);}
}

TWebTxtBs::TWebTxtBs(const PNotify& _Notify):
  Notify(_Notify), TrmSrv(), Pp(), TxtBs(), WebBsFetch(){
  // terminal server
  TrmSrv=PTrmSrv(new TWebTxtBsTrmSrv(this));
  // properties
  Pp=PPp(new TPp(PpNm_WebTxtBs, "Web-Text-Base", ptSet));
  Pp->AddPpStr(PpNm_WebTxtBsFPath, "Web-Text-Base File-Path", "../WebBs/");
  Pp->AddPpStr(PpNm_WebTxtBsNm, "Web-Text-Base Name", "si");
  Pp->AddPpStr(PpNm_WebFilterFNm, "Web-Filter File-Name", "../WebBs/si.wbf");
}

void TWebTxtBs::TxtBsNew(const TStr& Nm, const TStr& FPath){
  TxtBsClose(true);
  TxtBs=PTxtBs(new TTxtBs(Nm, FPath, faCreate));
  SendToStatusAndTrm(TStr("Web-Base '")+Nm+"' created at '"+FPath+"'.");
}

void TWebTxtBs::TxtBsOpenForUpdate(const TStr& Nm, const TStr& FPath){
  TxtBsClose(true);
  TxtBs=PTxtBs(new TTxtBs(Nm, FPath, faUpdate));
  SendToStatusAndTrm(TStr("Web-Base '")+Nm+"' opened for update at '"+FPath+"'.");
}

void TWebTxtBs::TxtBsOpenForRdOnly(const TStr& Nm, const TStr& FPath){
  TxtBsClose(true);
  TxtBs=PTxtBs(new TTxtBs(Nm, FPath, faRdOnly));
  SendToStatusAndTrm(TStr("Web-Base '")+Nm+"' opened for reading at '"+FPath+"'.");
}

void TWebTxtBs::TxtBsDel(const TStr& Nm, const TStr& FPath){
  TxtBsClose(true);
  TTxtBs::Del(Nm, FPath);
  SendToStatusAndTrm(TStr("Web-Base '")+Nm+"' deleted at '"+FPath+"'.");
}

void TWebTxtBs::TxtBsClose(const bool& IsClr){
  if (IsTxtBsActive()){
    FetchStop(IsClr);
    SendToStatusAndTrm("Closing Web-Base...", false);
    TxtBs=NULL;
    SendToStatusAndTrm("Web-Base closed.");
  } else {
    if (!IsClr){
      SendToStatusAndTrm("Web-Base already closed.");
    }
  }
}

bool TWebTxtBs::IsFetchEnd() const {
  if (IsFetchActive()){
    return (WebBsFetch->GetConns()==0)&&(WebBsFetch->GetUrlQLen()==0);
  } else {
    return true;
  }
}

void TWebTxtBs::FetchStart(
 const TStr& Nm, const TStr& FPath,
 const TStr& WebFilterFNm, const bool& IndexTxtBsP){
  if (IsTxtBsActive()){
    if (IsFetchActive()){
      SendToStatusAndTrm("Web-Fetch already active.");
    } else {
      PWebFilter WebFilter=TWebFilter::LoadTxt(WebFilterFNm);
      if (WebFilter.Empty()){
        SendToStatusAndTrm(TStr("Web-Filter '")+WebFilterFNm+"' does not exist.");
      } else {
        WebBsFetch=PWebBsFetch(new
         TWebBsFetch(Nm, FPath, faCreate, this, WebFilter, IndexTxtBsP));
        WebBsFetch->GoFetch();
      }
    }
  } else {
    SendToStatusAndTrm("Web-Base not active.");
  }
}

void TWebTxtBs::FetchContinue(const TStr& Nm, const TStr& FPath){
  if (IsTxtBsActive()){
    if (IsFetchActive()){
      SendToStatusAndTrm("Web-Fetch already active.");
    } else {
      WebBsFetch=PWebBsFetch(new TWebBsFetch(Nm, FPath, faUpdate, this));
      WebBsFetch->GoFetch();
    }
  } else {
    SendToStatusAndTrm("Web-Base not active.");
  }
}

void TWebTxtBs::FetchStop(const bool& IsClr){
  if (IsTxtBsActive()){
    if (IsFetchActive()){
      SendToStatusAndTrm("Stopping fetching...", false);
      WebBsFetch=NULL;
      SendToStatusAndTrm("Fetching stopped.");
    } else {
      if (!IsClr){
        SendToStatusAndTrm("Web-Fetch not active.");
      }
    }
  } else {
    SendToStatusAndTrm("Web-Base not active.");
  }
}

TStr TWebTxtBs::GetFetchStatusStr(){
  static const TStr ActiveStr="Active:";
  static const TStr QueueStr=" Queue:";
  static const TStr PgsStr=" Pages:";
  return
   ActiveStr+TInt::GetStr(WebBsFetch->GetConns())+
   QueueStr+TInt::GetStr(WebBsFetch->GetUrlQLen())+
   PgsStr+TInt::GetStr(TxtBs->GetDocs());
}

TStr TWebTxtBs::GetStatusStr(){
  TChA StatusChA;
  if (IsTxtBsActive()){
    StatusChA+="Web-Base '"; StatusChA+=GetNrNm(); StatusChA+="' active.\r\n";
    if (IsFetchActive()){
      StatusChA+="Web-Fetch active.\r\n";
      StatusChA+=GetFetchStatusStr(); StatusChA+="\r\n";
    } else {
      StatusChA+="Web-Fetch not active.\r\n";
      StatusChA+="Pages: ";
      StatusChA+=TInt::GetStr(TxtBs->GetDocs()); StatusChA+="\r\n";
    }
  } else {
    StatusChA+="Web-Base not active.\r\n";
  }
  return StatusChA;
}

void TWebTxtBs::ReportStatus(){
  SendToNotifyAndTrm(GetStatusStr());
}

void TWebTxtBs::ReportSocks(){
  SendToNotifyAndTrm(TSock::GetSockSysStatusStr());
}

void TWebTxtBs::ReportHosts(){
  SendToNotifyAndTrm(WebBsFetch->GetHostNmConnsPrVStr());
}

void TWebTxtBs::ReportQueue(const int& ShowLen){
  SendToNotifyAndTrm(WebBsFetch->GetHostNmQueuedPrVStr(ShowLen));
}

void TWebTxtBs::ReportSearch(const TStr& QueryStr){
  if (IsTxtBsActive()){
    PTxtBsRes TxtBsRes=TxtBs->Search(QueryStr);
    if (TxtBsRes->IsOk()){
      SendToNotifyAndTrm(TStr("Results for query: ")+TxtBsRes->GetWixExpStr());
      for (int DocN=0; DocN<TInt::GetMn(10, TxtBsRes->GetDocs()); DocN++){
        TStr DocNm; TStr DocTitleStr; TStr DocStr; TStr DocCtxStr;
        TxtBsRes->GetDocInfo(
         DocN, 65, 200, DocNm, DocTitleStr, DocStr, DocCtxStr);
        SendToNotifyAndTrm("-------------------------");
        SendToNotifyAndTrm(TInt::GetStr(DocN+1)+". "+DocTitleStr);
        SendToNotifyAndTrm(DocCtxStr);
        SendToNotifyAndTrm(DocNm);
      }
      if (TxtBsRes->GetDocs()>0){
        SendToNotifyAndTrm("---End-------------------");}
    } else {
      SendToNotifyAndTrm(TStr("Error: ")+TxtBsRes->GetErrMsg());
    }
  } else {
    SendToStatusAndTrm("Web-Base not active.");
  }
}

void TWebTxtBs::OnFetch(const int& /*FId*/, const PWebPg& WebPg){
  if (!IsUrlStr(WebPg->GetUrlStr())){
    TMem HttpRespMem; WebPg->GetHttpResp()->GetAsMem(HttpRespMem);
    TxtBs->AddDocMem(WebPg->GetUrlStr(), HttpRespMem, WebBsFetch->IsIndexTxtBs());
    if (WebPg->IsTxt()){
      PWebFilter WebFilter=WebBsFetch->GetWebFilter();
      PSIn HtmlDocSIn=TStrIn::New(WebPg->GetHttpBodyAsStr());
      PHtmlDoc HtmlDoc=THtmlDoc::New(HtmlDocSIn, hdtHRef);
      TStr BaseUrlStr=WebPg->GetUrlStr(); TStr RelUrlStr;
      for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
        if (WebFilter->IsUrlTok(HtmlDoc->GetTok(TokN), RelUrlStr)){
          PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
          if (Url->IsOk(usHttp)){
            WebBsFetch->SetUrlAsFinal(Url);
            if (WebFilter->IsUrlOk(Url)&&
             (!WebBsFetch->IsUrlEnqueued(Url))&&
             (!IsUrlStr(Url->GetAsFinalUrlStr()))){
              WebBsFetch->AddFetchUrl(Url);
            }
          }
        }
      }
      WebBsFetch->GoFetch();
    }
  }
}

/*PGraph TWebTxtBs::GetGraph() const {
  PWebHostNmBs WebHostNmBs=GetWebHostNmBs();
  PGraph Graph=TGGraph::New();
  int Docs=TxtBs->GetDocs();
  // vertices
  {TBlobPt TrvBlobPt=TxtBs->FFirstDocId(); TBlobPt DocId;
  int DocN=0; printf("\n");
  while (TxtBs->FNextDocId(TrvBlobPt, DocId)){
    TStr DocNm=TxtBs->GetDocNm(DocId);
    PVrtx Vrtx=TGVrtx::New(DocNm);
    Graph->AddVrtx(Vrtx);
    DocN++; printf("%3.0f%%\r", 100*double(DocN)/double(Docs));
  }}
  // edges
  {TBlobPt TrvBlobPt=TxtBs->FFirstDocId(); TBlobPt DocId;
  int DocN=0; printf("\n");
  while (TxtBs->FNextDocId(TrvBlobPt, DocId)){
    TStr DocNm; TStr DocStr;
    TxtBs->GetDocNmStr(DocId, DocNm, DocStr);
    PVrtx SrcVrtx=Graph->GetVrtx(DocNm);
    PSIn HtmlSIn=TMIn::New(DocStr);
    // generate tokens with hyper-references
    PHtmlDoc HtmlDoc=THtmlDoc::New(HtmlSIn, hdtHRef);
    for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
      PHtmlTok Tok=HtmlDoc->GetTok(TokN); TStr RelUrlStr;
      if (Tok->IsUrlTok(RelUrlStr)){
        PUrl Url=TUrl::New(RelUrlStr, DocNm);
        if (Url->IsOk(usHttp)){
          // define final host-name
          TStr AliasHostNm=Url->GetHostNm();
          TStr FinalHostNm=WebHostNmBs->GetFinalNm(AliasHostNm);
          Url->DefFinalUrl(FinalHostNm);
          // generate final url
          TStr UrlStr=Url->GetFinalUrlStr();
          // generate lower-case url
          PUrl LcUrl=TUrl::New(UrlStr);
          IAssert(LcUrl->IsOk(usHttp));
          LcUrl->ToLcPath();
          TStr LcUrlStr=LcUrl->GetUrlStr();
          // retrieve destination vertex
          PVrtx DstVrtx;
          if ((Graph->IsVrtx(UrlStr, DstVrtx)||
           Graph->IsVrtx(LcUrlStr, DstVrtx))&&(SrcVrtx()!=DstVrtx())){
            int SrcVId=SrcVrtx->GetVId(); int DstVId=DstVrtx->GetVId();
            // test if the connection already exists
            if (!Graph->IsVrtxsEdge(SrcVId, DstVId, true)){
              // generate & add new edge
              PEdge Edge=PEdge(new TGEdge(SrcVrtx, DstVrtx));
              Graph->AddEdge(Edge);
            }
          }
        }
      }
    }
    DocN++; printf("%3.0f%%\r", 100*double(DocN)/double(Docs));
  }}
  return Graph;
}*/

void TWebTxtBs::SaveTxt(const PSOut& SOut){
  TxtBs->SaveTxt(SOut);
}

/////////////////////////////////////////////////
// Web-Text-Base-Server

// html macros
const TStr TWebTxtBsSrv::HostNmMacro="$localhost$";
const TStr TWebTxtBsSrv::PortMacro="$port$";
const TStr TWebTxtBsSrv::QueryMacro="$query$";
const TStr TWebTxtBsSrv::HitsMacro="$hits$";
const TStr TWebTxtBsSrv::HitNumMacro="$hit_num$";
const TStr TWebTxtBsSrv::DocAddrMacro="$doc_addr$";
const TStr TWebTxtBsSrv::DocTitleMacro="$doc_title$";
const TStr TWebTxtBsSrv::DocCtxMacro="$doc_context$";
const TStr TWebTxtBsSrv::UrlMacro="$url$";
const TStr TWebTxtBsSrv::NmMacro="$name$";
// url field names
const TStr TWebTxtBsSrv::QueryUrlFldNm="Query";
const TStr TWebTxtBsSrv::HitSetUrlFldNm="HitSet";
const TStr TWebTxtBsSrv::AcceptUrlFldNm="Accept";
// file extensions
const TStr TWebTxtBsSrv::TplFExt=".tpl";
const TStr TWebTxtBsSrv::QueryLogFExt=".log";
// file names
const TStr TWebTxtBsSrv::DfTplFNm="default.tpl";

TStr TWebTxtBsSrv::GetRqContType(const PHttpRq& HttpRq) const {
  TStr AcceptValStr=HttpRq->GetFldVal(THttp::AcceptFldNm);
  if (AcceptValStr.IsStrIn(THttp::TextWmlFldVal)){
    return THttp::TextWmlFldVal;
  } else {
    return THttp::TextHtmlFldVal;
  }
}

TStr TWebTxtBsSrv::GetDfTplSetStr() const {
  TChA ChA;
  ChA+=".Remark /////////////////////////////////////////////////////\r\n";
  ChA+=".Remark // Web\r\n";
  ChA+=".Variable ContentType=\"text/html\" HitSetDocs=\"10\"\r\n";
  ChA+=".Variable ContentType=\"text/html\" StrHitSets=\"10\"\r\n";
  ChA+=".Variable ContentType=\"text/html\" MxDocTitleLen=\"65\"\r\n";
  ChA+=".Variable ContentType=\"text/html\" MxDocCtxLen=\"200\"\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"Header\"\r\n";
  ChA+="<HTML>\r\n";
  ChA+="<HEAD>\r\n";
  ChA+="  <META http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1250\">\r\n";
  ChA+="  <TITLE>Web-Prophet</TITLE>\r\n";
  ChA+="</HEAD>\r\n";
  ChA+="<BODY>\r\n";
  ChA+="<CENTER>\r\n";
  ChA+="  <FORM NAME=\"Search\" ACTION=\"http://$localhost$:$port$/\" METHOD=\"GET\">\r\n";
  ChA+="    <TABLE BORDER=0 BGCOLOR=\"Orange\" CELLPADDING=0 CELLSPACING=0 WIDTH=600>\r\n";
  ChA+="      <TR>\r\n";
  ChA+="        <TD WIDTH=100 ALIGN=MIDDLE ROWSPAN=3>\r\n";
  ChA+="          <A HREF=\"/\">\r\n";
  ChA+="            <!--<IMG SRC=\"http://$localhost$:$port$/Prophet.gif\" VSPACE=0 WIDTH=80 HEIGHT=60 ALIGN=MIDDLE BORDER=0>-->\r\n";
  ChA+="          </A>\r\n";
  ChA+="        </TD>\r\n";
  ChA+="        <TD WIDTH=300 ALIGN=LEFT VALIGN=MIDDLE ROWSPAN=3>\r\n";
  ChA+="          <B><FONT size=-1 color=\"Yellow\" FACE=\"VERDANA,HELVETICA,ARIAL\">Search for:</FONT></B><BR>\r\n";
  ChA+="          &nbsp;&nbsp;&nbsp;&nbsp;\r\n";
  ChA+="          <INPUT TYPE=text NAME=Query VALUE=\"$query$\" SIZE=32>\r\n";
  ChA+="          <!-- <INPUT TYPE=Image SRC=\"http://$localhost$:$port$/Go.gif\" BORDER=0>-->\r\n";
  ChA+="        </TD>\r\n";
  ChA+="        <TD WIDTH=100 ALIGN=LEFT VALIGN=MIDDLE ROWSPAN=3>\r\n";
  ChA+="          <FONT FACE=\"VERDANA,HELVETICA,ARIAL\" SIZE=-1 COLOR=\"Yellow\">\r\n";
  ChA+="            <A HREF=\"Help.html\">Help</A><BR>\r\n";
  ChA+="            <A HREF=\"Advanced.html\">Advanced</A><BR>\r\n";
  ChA+="            <A HREF=\"Feedback.html\">Feedback</A>\r\n";
  ChA+="          </FONT>\r\n";
  ChA+="        </TD>\r\n";
  ChA+="      </TR>\r\n";
  ChA+="    </TABLE>\r\n";
  ChA+="\r\n";
  ChA+="    <TABLE WIDTH=600 BGCOLOR=\"Lavender\" CELLSPACING=1 CELLPADDING=1 BORDER=0>\r\n";
  ChA+="      <TR>\r\n";
  ChA+="        <TD>\r\n";
  ChA+="          <FONT FACE=Verdana,Helvetica,Arial SIZE=-2>\r\n";
  ChA+="          <A HREF=\"Network.html\"><FONT COLOR=\"Black\">Network</FONT></A>:\r\n";
  ChA+="          <A HREF=\"http://www.yahoo.com\"><FONT COLOR=\"Blue\">Yahoo!</FONT></A> |\r\n";
  ChA+="          <A HREF=\"http://www.google.com\"><FONT COLOR=\"Blue\">Google</FONT></A> |\r\n";
  ChA+="        </TD>\r\n";
  ChA+="        <TD>\r\n";
  ChA+="          <FONT FACE=Verdana,Helvetica,Arial SIZE=-2>\r\n";
  ChA+="          <A HREF=\"Network.html\"><FONT COLOR=\"Blue\">All Sites...</FONT></A></FONT>\r\n";
  ChA+="        </TD>\r\n";
  ChA+="      </TR>\r\n";
  ChA+="    </TABLE>\r\n";
  ChA+="  </FORM>\r\n";
  ChA+="</CENTER>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"Footer\"\r\n";
  ChA+="<BR>\r\n";
  ChA+="<BR>\r\n";
  ChA+="<CENTER>\r\n";
  ChA+="  <FONT FACE=\"VERDANA,HELVETICA,ARIAL\" COLOR=\"Black\" SIZE=\"-2\">\r\n";
  ChA+="    <A HREF=\"Copyright.html\">Copyright &copy</A>; 1999 <BR>\r\n";
  ChA+="    <BR>\r\n";
  ChA+="    All Rights Reserved.\r\n";
  ChA+="    <BR>\r\n";
  ChA+="  </FONT>\r\n";
  ChA+="</CENTER>\r\n";
  ChA+="\r\n";
  ChA+="</BODY>\r\n";
  ChA+="</HTML>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"ResultHd\"\r\n";
  ChA+="<b>$hits$ hits for query:</b> <i>$query$</i><br>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"ResultRec\"\r\n";
  ChA+="<br><b>$hit_num$. </b><a href=\"$doc_addr$\">$doc_title$</a><br>\r\n";
  ChA+="$doc_context$<br>\r\n";
  ChA+="<a href=\"$doc_addr$\">$doc_addr$</a><br>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"ResultFt\"\r\n";
  ChA+="<br><br>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"HitSetHd\"\r\n";
  ChA+="<CENTER><B>Result Pages:&nbsp;\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"HitSetPrev\"\r\n";
  ChA+="<a href=\"$url$\">[&lt;&lt; Prev]</a>&nbsp;\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"HitSetRecLinkedNm\"\r\n";
  ChA+="<a href=\"$url$\">$name$</a>&nbsp;\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"HitSetRecNm\"\r\n";
  ChA+="$name$&nbsp;\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"HitSetNext\"\r\n";
  ChA+="<a href=\"$url$\">[Next &gt;&gt;]</a>&nbsp;\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"HitSetFt\"\r\n";
  ChA+="</B></CENTER>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"BadQuery\"\r\n";
  ChA+="<p><b>Invalid query:</b><i>$query$</i></p>\r\n";
  ChA+=".End\r\n";
  ChA+="\r\n";
  ChA+=".Template ContentType=\"text/html\" Category=\"BadHttpRq\"\r\n";
  ChA+="<p><b>Bad Http Request.</b></p>\r\n";
  ChA+=".End\r\n";

  return ChA;
}

void TWebTxtBsSrv::PrepTplSet(const TStr& FNm, const TStr& DfFNm){
  // get template input-stream (priority order: FNm, DfFNm, DfTplSetStr)
  PSIn SIn;
  bool FExists=false; SIn=TFIn::New(FNm, FExists);
  if (!FExists){SIn=TFIn::New(DfFNm, FExists);}
  if (!FExists){SIn=TStrIn::New(GetDfTplSetStr());}

  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloUniStr);
  Lx.GetStrToEolnAndCh('.');
  do {
    TStr SecNm=Lx.GetStr();
    if (SecNm=="End"){Lx.GetStrToEolnAndCh('.');}
    else if (SecNm=="Stop"){break;}
    else if (SecNm=="Template"){
      Lx.GetStr("ContentType"); Lx.GetSym(syEq); TStr ContTypeStr=Lx.GetStr();
      Lx.GetStr("Category"); Lx.GetSym(syEq); TStr CatStr=Lx.GetStr();
      Lx.GetEoln();
      TStr Tpl=Lx.GetStrToEolnAndCh('.');
      Tpl.ChangeStrAll(HostNmMacro, HostNm);
      Tpl.ChangeStrAll(PortMacro, PortNStr);
      AddTpl(ContTypeStr, CatStr, Tpl);
    } else if (SecNm=="Variable"){
      Lx.GetStr("ContentType"); Lx.GetSym(syEq); TStr ContTypeStr=Lx.GetStr();
      TStr VarNm=Lx.GetStr(); Lx.GetSym(syEq); TStr VarVal=Lx.GetStr();
      Lx.GetEoln();
      AddVar(ContTypeStr, VarNm, VarVal);
      Lx.GetStrToEolnAndCh('.');
    } else if (SecNm=="Remark"){
      Lx.GetStrToEolnAndCh('.');
    } else {
      Fail;
    }
  } while (!Lx.IsEof());
}

TWebTxtBsSrv::TWebTxtBsSrv(const PWebTxtBs& _WebTxtBs,
 const int& PortN, const PNotify& _Notify):
  TWebSrv(PortN, true, _Notify),
  Notify(_Notify),
  WebTxtBs(_WebTxtBs),
  HostNm(), PortNStr(TInt::GetStr(PortN)),
  TplNmToValH(100), VarNmToValH(100),
  SLog(){
  // file names
  TStr TplFNm; TStr QueryLogFNm;
  GetFNms(WebTxtBs->GetNrNm(), WebTxtBs->GetNrFPath(), TplFNm, QueryLogFNm);

  // local host name
  PSockHost LocalSockHost=TSockHost::GetLocalSockHost();
  HostNm=LocalSockHost->GetHostNm();

  // templates
  PrepTplSet(TplFNm, DfTplFNm);

  // log file
  SLog=TFOut::New(QueryLogFNm, true);
}

void TWebTxtBsSrv::AddHitSetChA(
 const PTxtBsRes& TxtBsRes, const TStr& RqContTypeStr,
 const int& HitSetN, const int& HitSetDocs, const int& StrHitSets,
 const PUrlEnv& UrlEnv, TChA& OutChA){
  // get hit-set data
  TStr PrevUrlStr; TStrPrV NmUrlStrPrV; TStr NextUrlStr;
  TxtBsRes->GetHitSet(
   HitSetN, HitSetDocs, StrHitSets, HitSetUrlFldNm, UrlEnv,
   PrevUrlStr, NmUrlStrPrV, NextUrlStr);
  // hit-set header
  TStr HitSetHdTpl=GetTplVal(RqContTypeStr, "HitSetHd");
  OutChA+=HitSetHdTpl;
  // hit-set 'previous'
  if (!PrevUrlStr.Empty()){
    TStr HitSetPrevTpl=GetTplVal(RqContTypeStr, "HitSetPrev");
    HitSetPrevTpl.ChangeStrAll(UrlMacro, PrevUrlStr);
    OutChA+=HitSetPrevTpl;
  }
  // hit-set records
  for (int NmN=0; NmN<NmUrlStrPrV.Len(); NmN++){
    TStr Nm=NmUrlStrPrV[NmN].Val1;
    TStr UrlStr=NmUrlStrPrV[NmN].Val2;
    if (UrlStr.Empty()){
      // hit-set record with name
      TStr HitSetRecTpl=GetTplVal(RqContTypeStr, "HitSetRecNm");
      HitSetRecTpl.ChangeStrAll(NmMacro, Nm);
      OutChA+=HitSetRecTpl;
    } else {
      // hit-set record with linked name
      TStr HitSetRecTpl=GetTplVal(RqContTypeStr, "HitSetRecLinkedNm");
      HitSetRecTpl.ChangeStrAll(NmMacro, Nm);
      HitSetRecTpl.ChangeStrAll(UrlMacro, UrlStr);
      OutChA+=HitSetRecTpl;
    }
  }
  // hit-set 'next'
  if (!NextUrlStr.Empty()){
    TStr HitSetNextTpl=GetTplVal(RqContTypeStr, "HitSetNext");
    HitSetNextTpl.ChangeStrAll(UrlMacro, NextUrlStr);
    OutChA+=HitSetNextTpl;
  }
  // hit-set footer
  TStr HitSetFtTpl=GetTplVal(RqContTypeStr, "HitSetFt");
  OutChA+=HitSetFtTpl;
}

void TWebTxtBsSrv::OnHttpRq(const int& SockId, const PHttpRq& HttpRq){
  // request parameters
  TStr RqContTypeStr=THttp::TextHtmlFldVal;
  PUrlEnv UrlEnv;
  TStr QueryStr; TStr EQueryStr;
  TStr HitSetStr; TStr AcceptStr;
  // prepare & extract search-environment
  if (HttpRq->IsOk()){
    // prepare search-environment
    PUrl Url=HttpRq->GetUrl();
    UrlEnv=HttpRq->GetUrlEnv();
    // if empty search-environment and url-path is not empty
    if (UrlEnv->Empty()&&
     (Url->GetPathSegs()>0)&&(!Url->GetPathSeg(0).Empty())){
      // get document name
      TStr DocNm=Url->GetPathSeg(Url->GetPathSegs()-1);
      if (WebTxtBs->GetTxtBs()->IsDoc(DocNm)){
        // document exists in text-base
        TStr DocStr=WebTxtBs->GetTxtBs()->GetDocStr(DocNm);
        PSIn HttpBodySIn=TMIn::New(DocStr);
        PHttpResp HttpResp=
         THttpResp::New(THttp::OkStatusCd, RqContTypeStr, false, HttpBodySIn);
        SendHttpResp(SockId, HttpResp);
      } else {
        // ordinary http request
        TWebSrv::OnHttpRq(SockId, HttpRq);
      }
      // end if no search request
      return;
    }
    // extract fields from search-environment
    QueryStr=UrlEnv->GetVal(QueryUrlFldNm).GetTrunc();
    EQueryStr=THtmlLx::GetEscapedStr(QueryStr);
    HitSetStr=UrlEnv->GetVal(HitSetUrlFldNm).GetTrunc();
    AcceptStr=UrlEnv->GetVal(AcceptUrlFldNm).GetTrunc();
    if (AcceptStr.Empty()){RqContTypeStr=GetRqContType(HttpRq);}
    else {RqContTypeStr=AcceptStr;}
  }

  // hit-set
  int HitSetN=1;
  HitSetStr.IsInt(true, 1, TInt::Mx, HitSetN);
  int HitSetDocs=GetVarVal(RqContTypeStr, "HitSetDocs").GetInt();
  int StrHitSets=GetVarVal(RqContTypeStr, "StrHitSets").GetInt();

  // output buffer
  TChA OutChA(10000);
  // header
  TStr HdTpl=GetTplVal(RqContTypeStr, "Header");
  HdTpl.ChangeStrAll(QueryMacro, EQueryStr);
  OutChA+=HdTpl;
  // html body
  if (HttpRq->IsOk()){
    if (!QueryStr.Empty()){
      // execute query
      PTxtBsRes TxtBsRes=WebTxtBs->Search(QueryStr);
      TStr EWixExpStr=THtmlLx::GetEscapedStr(TxtBsRes->GetWixExpStr());
      // log string
      TChA QueryInfoChA;
      QueryInfoChA+="Query: "+QueryStr;
      //QueryInfoChA+=" ["+GetPeerNm(SockId)+"]";
      QueryInfoChA+=" ["+TSecTm::GetCurTm().GetStr()+"]";
      TNotify::OnNotify(Notify, ntInfo, QueryInfoChA);
      SLog->PutStr(QueryInfoChA); SLog->PutLn(); SLog->Flush();
      // query-results processing
      if (TxtBsRes->IsOk()){
        // result header
        TStr ResultHdTpl=GetTplVal(RqContTypeStr, "ResultHd");
        ResultHdTpl.ChangeStrAll(QueryMacro, EWixExpStr);
        ResultHdTpl.ChangeStrAll(HitsMacro, TInt::GetStr(TxtBsRes->GetDocs()));
        OutChA+=ResultHdTpl;
        // result records
        int MnDocN; int MxDocN;
        TxtBsRes->GetHitSetMnMxDocN(HitSetN, HitSetDocs, MnDocN, MxDocN);
        for (int DocN=MnDocN; DocN<=MxDocN; DocN++){
          // get result document data
          int MxDocTitleLen=GetVarVal(RqContTypeStr, "MxDocTitleLen").GetInt();
          int MxDocCtxLen=GetVarVal(RqContTypeStr, "MxDocCtxLen").GetInt();
          TStr DocNm; TStr DocTitleStr; TStr DocStr; TStr DocCtxStr;
          TxtBsRes->GetDocInfo(DocN, MxDocTitleLen, MxDocCtxLen,
           DocNm, DocTitleStr, DocStr, DocCtxStr);
          if (DocTitleStr.Empty()){DocTitleStr=DocNm;}
          // result record
          TStr ResultRecTpl=GetTplVal(RqContTypeStr, "ResultRec");
          ResultRecTpl.ChangeStrAll(HitNumMacro, TInt::GetStr(DocN+1));
          ResultRecTpl.ChangeStrAll(DocAddrMacro, DocNm);
          ResultRecTpl.ChangeStrAll(DocTitleMacro, DocTitleStr);
          ResultRecTpl.ChangeStrAll(DocCtxMacro, DocCtxStr);
          OutChA+=ResultRecTpl;
        }
        // result footer
        TStr ResultFtTpl=GetTplVal(RqContTypeStr, "ResultFt");
        OutChA+=ResultFtTpl;
        // hit-set
        AddHitSetChA(TxtBsRes, RqContTypeStr, HitSetN, HitSetDocs, StrHitSets,
         UrlEnv, OutChA);
      } else {
        // bad query
        TStr BadQueryTpl=GetTplVal(RqContTypeStr, "BadQuery");
        BadQueryTpl.ChangeStrAll(QueryMacro, EWixExpStr);
        OutChA+=BadQueryTpl;
      }
    }
  } else {
    // bad http-request
    TStr BadHttpRqTpl=GetTplVal(RqContTypeStr, "BadHttpRq");
    OutChA+=BadHttpRqTpl;
  }

  // footer
  TStr FtTpl=GetTplVal(RqContTypeStr, "Footer");
  FtTpl.ChangeStrAll(QueryMacro, EQueryStr);
  OutChA+=FtTpl;

  // construct & send response
  PSIn HttpBodySIn=TMIn::New(OutChA);
  PHttpResp HttpResp=
   THttpResp::New(THttp::OkStatusCd, RqContTypeStr, false, HttpBodySIn);
  SendHttpResp(SockId, HttpResp);
}

void TWebTxtBsSrv::GetFNms(
 const TStr& Nm, const TStr& FPath, TStr& TplFNm, TStr& QueryLogFNm){
  TStr NrFPath=TStr::GetNrFPath(FPath);
  TStr NrNm=TStr::GetNrFMid(Nm);
  TplFNm=NrFPath+NrNm+TplFExt;
  QueryLogFNm=NrFPath+NrNm+QueryLogFExt;
}

