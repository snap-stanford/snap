#include "btaserver.h"

/////////////////////////////////////////////////
// BTAlarms-Web-Server
int TBTAServer::MnSeverity = 3;
int TBTAServer::ListItemsPerPage = 20;
int TBTAServer::DetailItemsPerPage = 10;
int TBTAServer::ListEventsPerPage = 10;

int64 TBTAServer::RootCouseTimeWnd = int64(24*3600) * int64(1000);  // one day
int TBTAServer::RootCouseEventWnd = 300;

int64 TBTAServer::BoardTimeWnd = int64(4*3600) * int64(1000);  // four hours
int TBTAServer::BoardEventWnd = 5000;

int TBTAServer::MnBeforeCount = 5;
double TBTAServer::MxChiProb = 0.05;
double TBTAServer::MnChiVal = 0.01;
double TBTAServer::MnCorrProb = 0.001;
int TBTAServer::MnIpNumsPerPlot = 20;


int TBTAServer::GetTotalFq(const TIntPrV& IdFqV) {
    int TotalFq = 0;
    for (int IdFqN = 0; IdFqN < IdFqV.Len(); IdFqN++) {
        TotalFq += IdFqV[IdFqN].Val2; }
    return TotalFq;
}

TStr TBTAServer::TruncStr(const TStr& FullStr, const int& MxLen) {
    if (FullStr.Len() > MxLen) {
        return FullStr.GetSubStr(0, MxLen-1) + "...";
    } else { return FullStr; }
}

TTm TBTAServer::GetBreakDate(const int& Days) const { 
    TTm BreakDate = BtaEventBs->GetLastTime();
    BreakDate.DelDays(Days);
    return BreakDate;
}

void TBTAServer::GetIpNumEventCount(const int& IpNumId, const TTm& StartDate, 
        const TTm& BreakDate, const TTm& EndDate, double& AvgCount, int& LastCount) const {

    int OldCount = 0; LastCount = 0;
    const TIntV& EventV = BtaCorrBs->GetIpNumEventV(IpNumId);
    for (int EventN = 0; EventN < EventV.Len(); EventN++) {
        TTm EventDate = BtaEventBs->GetEvent(EventV[EventN]).GetFirstTm();
        if (EventDate < StartDate || EventDate > EndDate) { continue; }
        if (EventDate < BreakDate) { OldCount++; }
        else { LastCount++; }
    }
    const double NormFactor = TBtaLongProj::GetNormFact(StartDate, BreakDate, EndDate);
    AvgCount = NormFactor * double(OldCount);
}


void TBTAServer::MakeEventsPage(const TIntV& EventV, const TStr& SortTypeStr,
        const int& PageNum, TChA& HtmlChA) {

    // prepare the header with histogram and some basic statistics
    HtmlChA += "<h1>Alarms</h1>\n";                   
    HtmlChA += TStr::Fmt("<p>Number of alarms: <b>%d</b> (severity > 2)<br>\n", EventV.Len());
    TStr LatestAlarm = BtaEventBs->GetEvent(EventV.Last()).GetFirstTm().GetWebLogDateTimeStr();
    HtmlChA += "Time of the latest alarm: <b>" + LatestAlarm + "</b><br>\n";
    TStr OldestAlarm = BtaEventBs->GetEvent(EventV[0]).GetFirstTm().GetWebLogDateTimeStr();
    HtmlChA += "Time of the oldest alarm: <b>" + OldestAlarm + "</b></p>\n";
    TStr PlotFNm = TFile::GetUniqueFNm("img/EventPlot.plt");
    PlotEventV("", EventV, PlotFNm);
    PlotFNm.ChangeChAll('/', '-');
    HtmlChA += "<p><img src=\"" + PlotFNm + ".png\"></p>";

    // list of recent alarms
    HtmlChA += "<h2>List of recent alarms</h2>\n";
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/events?page=%d&sort=%s\">Previous</a>\n", PageNum-1, SortTypeStr.CStr()); }
    HtmlChA += TStr::Fmt("<a href=\"/events?page=%d&sort=%s\">Next</a>\n", PageNum+1, SortTypeStr.CStr());
    // show the list
    MakeEventList(EventV, true, PageNum, HtmlChA);
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/events?page=%d&sort=%s\">Previous</a>\n", PageNum-1, SortTypeStr.CStr()); }
    HtmlChA += TStr::Fmt("<a href=\"/events?page=%d&sort=%s\">Next</a>\n", PageNum+1, SortTypeStr.CStr());
}

void TBTAServer::MakeEventPage(const int& EventId, TChA& HtmlChA) {
    if (EventId < 0 || EventId >= BtaEventBs->GetEvents()) { 
        HtmlChA += "<p>Wrong event Id!</p>"; return; }
    HtmlChA += "<h1>Alarm statistics</h1>\n";       
    // load event fields
    EventStat(EventId, HtmlChA);
    HtmlChA += "<p><a href=\"/event_full?id=" + 
        TInt::GetStr(EventId) + "\">Show all fields</a></p>";
    // show related alarms
    HtmlChA += "<h2>Pattern of related events</h2>\n";       
    EventRootCause(EventId, HtmlChA);
    // predict alarms
    HtmlChA += "<h2>Devices predicted to fail</h2>\n";
    EventPredIpNums(EventId, HtmlChA);
}

void TBTAServer::MakeEventFullPage(const int& EventId, TChA& HtmlChA) {
    HtmlChA += "<h1>Alarm database fields</h1>\n";       
    HtmlChA += "<p><a href=\"/event?id=" + 
        TInt::GetStr(EventId) + "\">Back to alarm statistics</a></p>";
    EventFullStat(EventId, HtmlChA);
}

void TBTAServer::MakeIpNumPage(const int& IpNumId, TChA& HtmlChA) {
    HtmlChA += "<h1>Device statistics</h1>\n";       
    if (BtaCorrBs->IsIpNumId(IpNumId)) { 
        TStr IpNumNm = BtaDevDefBs->GetIpNumNm(IpNumId);
        HtmlChA += TStr::Fmt("<p><b>Device IP:</b> %s<br>\n", IpNumNm.CStr());
        HtmlChA += TStr::Fmt("<b>Device project:</b> %s<br>\n", GetProjLink(IpNumId).CStr());
        HtmlChA += TStr::Fmt("<b>Number of alarms:</b> %d</p>\n", BtaCorrBs->GetIpNumIdEvents(IpNumId));
        // device alarms
        HtmlChA += "<h2>Recent alarms</h2>\n";       
        HtmlChA += TStr::Fmt("<p><a href=\"/d_events?id=%d\">Show all alarms</a></p>\n", IpNumId);
        TIntV EventV = BtaCorrBs->GetIpNumEventV(IpNumId);
        TStr PlotFNm = TFile::GetUniqueFNm("img/EventPlot.plt");
        PlotEventV(BtaDevDefBs->GetIpNumNm(IpNumId), EventV, PlotFNm);
        PlotFNm.ChangeChAll('/', '-');
        HtmlChA += "<p><img src=\"" + PlotFNm + ".png\"></p>";
        MakeEventList(EventV, false, 0, HtmlChA);
        // device correlations
        HtmlChA += "<h2>Correlated devices</h2>\n";       
        IpNumDevCorrStat(IpNumId, HtmlChA);
    } else {
        HtmlChA += "<p><b>Device not in the database!</b></p>";                    
    }   
}

void TBTAServer::MakeProjectPage(const int& ProjId, TChA& HtmlChA) {
    HtmlChA += "<h1>Project statistics</h1>\n";
    // project fields
    ShortProjStat(ProjId, HtmlChA);
    // project alarms
    HtmlChA += "<h2>Recent alarms</h2>\n";
    HtmlChA += TStr::Fmt("<p><a href=\"/p_events?id=%d\">Show all alarms</a></p>\n", ProjId);
    TIntV EventV = BtaCorrBs->GetProjEventV(ProjId);
    TStr PlotFNm = TFile::GetUniqueFNm("img/EventPlot.plt");
    PlotEventV(BtaDevDefBs->GetProjNm(ProjId), EventV, PlotFNm);
    PlotFNm.ChangeChAll('/', '-');
    HtmlChA += "<p><img src=\"" + PlotFNm + ".png\"></p>";
    MakeEventList(EventV, false, 0, HtmlChA);
    // predictions for alarms
    HtmlChA += "<h2>Devices predicted to fail</h2>\n";
    EventShortPredIpNums(EventV.Last(), HtmlChA);
    // device correlations within the project
    //TODO: make it scalable to projects with many devices
    //HtmlChA += "<h2>Correlations of devices within the project</h2>\n";
    //ProjWithinCorrStat(ProjId, HtmlChA);
}

void TBTAServer::MakeIpNumEventsPage(const int& IpNumId, 
        const TIntV& EventV, const int& PageNum, TChA& HtmlChA) {

    HtmlChA += "<h1>Device's alarms</h1>\n";                   
    ShortIpNumStat(IpNumId, HtmlChA);

    HtmlChA += "<h2>List of alarms</h2>\n";                   
    //HtmlChA += TStr::Fmt("<p>Number of alarms: <b>%d</b> (severity > 2)<br>\n", EventV.Len());
    TStr LatestAlarm = BtaEventBs->GetEvent(EventV.Last()).GetFirstTm().GetWebLogDateTimeStr();
    HtmlChA += "Time of the latest alarm: <b>" + LatestAlarm + "</b><br>\n";
    TStr OldestAlarm = BtaEventBs->GetEvent(EventV[0]).GetFirstTm().GetWebLogDateTimeStr();
    HtmlChA += "Time of the oldest alarm: <b>" + OldestAlarm + "</b></p>\n";

    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/d_events?id=%d&page=%d\">Previous</a>\n", IpNumId, PageNum-1); }
    HtmlChA += TStr::Fmt("<a href=\"/d_events?id=%d&page=%d\">Next</a>\n", IpNumId, PageNum+1);
    // show the list
    MakeEventList(EventV, false, PageNum, HtmlChA);
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/d_events?id=%d&page=%d\">Previous</a>\n", IpNumId, PageNum-1); }
    HtmlChA += TStr::Fmt("<a href=\"/d_events?id=%d&page=%d\">Next</a>\n", IpNumId, PageNum+1);
}

void TBTAServer::MakeProjEventsPage(const int& ProjId, const TIntV& EventV,
        const int& PageNum, TChA& HtmlChA) {

    HtmlChA += "<h1>Project's alarms</h1>\n";                   
    ShortProjStat(ProjId, HtmlChA);

    HtmlChA += "<h2>List of alarms</h2>\n";                   
    //HtmlChA += TStr::Fmt("<p>Number of alarms: <b>%d</b> (severity > 2)<br>\n", EventV.Len());
    TStr LatestAlarm = BtaEventBs->GetEvent(EventV.Last()).GetFirstTm().GetWebLogDateTimeStr();
    HtmlChA += "Time of the latest alarm: <b>" + LatestAlarm + "</b><br>\n";
    TStr OldestAlarm = BtaEventBs->GetEvent(EventV[0]).GetFirstTm().GetWebLogDateTimeStr();
    HtmlChA += "Time of the oldest alarm: <b>" + OldestAlarm + "</b></p>\n";

    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/p_events?id=%d&page=%d\">Previous</a>\n", ProjId, PageNum-1); }
    HtmlChA += TStr::Fmt("<a href=\"/p_events?id=%d&page=%d\">Next</a>\n", ProjId, PageNum+1);
    // show the list
    MakeEventList(EventV, false, PageNum, HtmlChA);
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/p_events?id=%d&page=%d\">Previous</a>\n", ProjId, PageNum-1); }
    HtmlChA += TStr::Fmt("<a href=\"/p_events?id=%d&page=%d\">Next</a>\n", ProjId, PageNum+1);
}

void TBTAServer::MakeIpNumCorrEventPage(const int& IpNumId, 
        const int& CorrIpNumId, const int& PageNum, TChA& HtmlChA) {

    HtmlChA += "<h1>Alarms behind device-device correlation</h1>\n";
    // show device properties
    HtmlChA += "<h2>First device</h2>\n";
    ShortIpNumStat(IpNumId, HtmlChA);
    HtmlChA += "<h2>Second device</h2>\n";
    ShortIpNumStat(CorrIpNumId, HtmlChA);
    // generate event image
    HtmlChA += "<h2>List of correlating alarms</h2>\n";
    const TIntV& EventV = BtaCorrBs->GetIpNumEventV(IpNumId);
    const TIntV& CorrEventV = BtaCorrBs->GetIpNumEventV(CorrIpNumId);
    TStr PlotFNm = TFile::GetUniqueFNm("img/EventPlot.plt");
    PlotEventV(BtaDevDefBs->GetIpNumNm(IpNumId), EventV, 
        BtaDevDefBs->GetIpNumNm(CorrIpNumId), CorrEventV, PlotFNm);
    PlotFNm.ChangeChAll('/', '-');
    HtmlChA += "<p><img src=\"" + PlotFNm + ".png\"></p>";
    if (BtaCorrBs->IsIpNumIdPrCorr(IpNumId, CorrIpNumId)) {
        const TIntPrV& EventPrV = BtaCorrBs->GetIpNumCorrIpNumPrEventV(IpNumId, CorrIpNumId);
        HtmlChA += TStr::Fmt("<p><b>Number of correlated alarm pairs:</b>%d</p>\n", EventPrV.Len());
        // moving between pages
        if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/dd_events?page=%d&id=%d&corrid=%d\">Previous</a>\n", PageNum-1, IpNumId, CorrIpNumId); }
        HtmlChA += TStr::Fmt("<a href=\"/dd_events?page=%d&id=%d&corrid=%d\">Next</a>\n", PageNum+1, IpNumId, CorrIpNumId);
        // show list
        MakeEventPrList(EventPrV, PageNum, HtmlChA);
        // moving between pages
        if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/dd_events?page=%d&id=%d&corrid=%d\">Previous</a>\n", PageNum-1, IpNumId, CorrIpNumId); }
        HtmlChA += TStr::Fmt("<a href=\"/dd_events?page=%d&id=%d&corrid=%d\">Next</a>\n", PageNum+1, IpNumId, CorrIpNumId);
    }
}

void TBTAServer::MakeLongPage(const TIntV& SortProjIdV, 
        const int& PageNum, const TStr& PeriodStr, TChA& HtmlChA) {

    HtmlChA += "<h1>Projects with change trend</h1>\n";                   
    if (PeriodStr == "week") { HtmlChA += "<p><b>[last week vs. history]</b>, "; } 
    else { HtmlChA += "<p><a href=\"/long?period=week\">[last week vs. history]</a>, "; }
    if (PeriodStr == "month") { HtmlChA += "<b>[last month vs. history]</b></p>"; }
    else {HtmlChA += "<a href=\"/long?period=month\">[last month vs. history]</a></p>"; }
    
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/long?page=%d&period=%s\">Previous</a>\n", PageNum-1, PeriodStr.CStr()); }
    HtmlChA += TStr::Fmt("<a href=\"/long?page=%d&period=%s\">Next</a>\n", PageNum+1, PeriodStr.CStr());
    // list most different projects
    MakeLongProjList(SortProjIdV, PageNum, PeriodStr, HtmlChA);
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/long?page=%d&period=%s\">Previous</a>\n", PageNum-1, PeriodStr.CStr()); }
    HtmlChA += TStr::Fmt("<a href=\"/long?page=%d&period=%s\">Next</a>\n", PageNum+1, PeriodStr.CStr());    
}

void TBTAServer::MakeProjLongPage(const int& ProjId,
        const int& PageNum, const TStr& PeriodStr, TChA& HtmlChA) {

    HtmlChA += "<h1>Devices with change trends</h1>\n";                   
    // project fields
    ShortProjStat(ProjId, HtmlChA, true);
    // show device trends
    HtmlChA += "<h2>List of device trends</h2>\n";                   
    if (PeriodStr == "week") { HtmlChA += "<p><b>[last week vs. history]</b>, "; } 
    else { HtmlChA += TStr::Fmt("<p><a href=\"/p_long?id=%d&period=week\">[last week vs. history]</a>, ", ProjId); }
    if (PeriodStr == "month") { HtmlChA += "<b>[last month vs. history]</b></p>"; }
    else {HtmlChA += TStr::Fmt("<a href=\"/p_long?id=%d&period=month\">[last month vs. history]</a></p>", ProjId); }
    // make plot
    TStr PlotFNm = TFile::GetUniqueFNm("IpNum.plt");
    TTm StartDate = BtaEventBs->GetFirstTime();
    TTm BreakDate = GetBreakDate(PeriodStr == "week" ? 7 : 30);
    TTm EndDate = BtaEventBs->GetLastTime();
    PlotLongIpNum(BtaDevDefBs->GetProjNm(ProjId), PlotFNm, ProjId, 
        StartDate, BreakDate, EndDate, PeriodStr, MnIpNumsPerPlot, true);
    HtmlChA += "<p><img src=\"" + PlotFNm + ".png\"></p>";
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/p_long?id=%d&page=%d&period=%s\">Previous</a>\n", ProjId, PageNum-1, PeriodStr.CStr()); }
    HtmlChA += TStr::Fmt("<a href=\"/p_long?id=%d&page=%d&period=%s\">Next</a>\n", ProjId, PageNum+1, PeriodStr.CStr());
    // list most different devices
    MakeLongIpNumList(ProjId, PageNum, PeriodStr, HtmlChA);
    // moving between pages
    if (PageNum > 0) { HtmlChA += TStr::Fmt("<a href=\"/long?id=%d&page=%d&period=%s\">Previous</a>\n", ProjId, PageNum-1, PeriodStr.CStr()); }
    HtmlChA += TStr::Fmt("<a href=\"/p_long?id=%d&page=%d&period=%s\">Next</a>\n", ProjId, PageNum+1, PeriodStr.CStr());    
}

void TBTAServer::MakeShortEventPage(const int& EventId, TChA& HtmlChA) {
	EventShortStat(EventId, HtmlChA);
    HtmlChA += "<hr>";
    HtmlChA += "<b>Related Traps:</b><br>";
    EventShortRootCause(EventId, HtmlChA);
}

void TBTAServer::MakeShortPredIpNumsPage(const int& EventId, TChA& HtmlChA) {
	EventShortStat(EventId, HtmlChA);
    HtmlChA += "<hr>";
    HtmlChA += "<b>Devices predicted to fail:</b><br>";
    EventShortPredIpNums(EventId, HtmlChA);
}

void TBTAServer::MakeShortIpNumPage(const int& EventId, TChA& HtmlChA) {
    const int IpNumId = BtaEventBs->GetEvent(EventId).GetIpNumId();
    const TIntV& IpNumEventV = BtaCorrBs->GetIpNumEventV(IpNumId);
	EventShortStat(EventId, HtmlChA);
    HtmlChA += "<hr>";
    HtmlChA += "<b>Device event trend</b><br>";
    TStr PlotFNm = TFile::GetUniqueFNm("img/EventPlot.plt");
    PlotEventV("", IpNumEventV, PlotFNm, 500, 250);
    PlotFNm.ChangeChAll('/', '-');
    HtmlChA += "<img src=\"" + PlotFNm + ".png\"><br>";
    HtmlChA += "<hr>";
    HtmlChA += "<b>Recent events</b><br>";
    MakeShortEventList(IpNumEventV, HtmlChA);
}

void TBTAServer::MakeBoardPage(const int& ProjCount, const int& RefreshTmSec, TChA& HtmlChA) {
    // prepare the header
    HtmlChA += "<html><head>\n";
    HtmlChA += "<title>Prediction Board</title>\n";
    HtmlChA += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
    HtmlChA += TStr::Fmt("<meta http-equiv=\"REFRESH\" content=\"%d;url=board?count=%d&refresh=%d\"></HEAD>", 
        RefreshTmSec, ProjCount + 1, RefreshTmSec);
    HtmlChA += "<link href=\"board.css\" type=\"text/css\" rel=\"stylesheet\">\n";
    HtmlChA += "</head><body>\n";
    // first we need to find the most active in last 5000 alarms
    TIntKdV AllProjIdFqV;
    BtaEventBs->GetMostActiveProjs(BoardTimeWnd, 
        BoardEventWnd, MnSeverity, AllProjIdFqV);
    // go from top of the list, and find the projects for which we can predict things
    TFltIntKdV ScoreProjIdV; THash<TInt, TFltIntPrV> ProbIpNumIdVH;
    const int MxPredIpNums = 2; const int MxProjs = 6;
    for (int ProjIdN = 0; ProjIdN < AllProjIdFqV.Len(); ProjIdN++) {
        // get predictions for current project
        const int ProjId = AllProjIdFqV[ProjIdN].Key;
        const int LastEventN = BtaCorrBs->GetProjEventV(ProjId).Last();
        TIntFltH IpNumIdProbH;
        BtaCorrBs->GetNextIpNumIdProb(LastEventN, ProjId, 
            BtaEventBs, 100, 600, IpNumIdProbH);
        // if no prediction, we skip the project
        if (IpNumIdProbH.Empty()) { continue;}
        // sort according to probabilities
        TFltIntPrV ProbIpNumIdV; 
        IpNumIdProbH.GetDatKeyPrV(ProbIpNumIdV); 
        ProbIpNumIdV.Sort(false);        
        // just keep the top few
        if (ProbIpNumIdV.Len() > MxPredIpNums) { 
            ProbIpNumIdV.Trunc(MxPredIpNums); }
        // good, remember predictions
        ScoreProjIdV.Add(TFltIntKd(ProbIpNumIdV[0].Val1, ProjId));
        ProbIpNumIdVH.AddDat(ProjId, ProbIpNumIdV);
        // if more then max projects, stop
        if (ScoreProjIdV.Len() >= 2*MxProjs) { break; }
    } 
    // sort and select top projects
    ScoreProjIdV.Sort(false);
    if (ScoreProjIdV.Len() > MxProjs) { ScoreProjIdV.Trunc(MxProjs); }
    // display the projects
    if (ScoreProjIdV.Len() > 0) {
        // display the top predictions
        const int SelProjIdN = ProjCount % ScoreProjIdV.Len();
	    HtmlChA += "<div class=\"window\">\n";
        // first aggregated list of top projects
  	    HtmlChA += "<div class=\"project_list\">\n";
        for (int ProjIdN = 0; ProjIdN < ScoreProjIdV.Len(); ProjIdN++) {
            const int ProjId = ScoreProjIdV[ProjIdN].Dat;
            const TFltIntPrV& ProbIpNumIdV = ProbIpNumIdVH.GetDat(ProjId);
            // are we displaying the details of this project?
            if (ProjIdN == SelProjIdN) { 
    	        HtmlChA += "<div class=\"project selected\">\n";
            } else {
    	        HtmlChA += "<div class=\"project\">\n";
            }
            // show project name
            TStr ProjNm = BtaDevDefBs->GetProjNm(ProjId);
	        HtmlChA += "<div class=\"project_title\">" + ProjNm + "</div>\n";
            // show the list of predicted devices
            for (int IpNumN = 0; IpNumN < ProbIpNumIdV.Len(); IpNumN++) {
                const int IpNumId = ProbIpNumIdV[IpNumN].Val2;
                TStr IpNumNm = BtaDevDefBs->GetIpNumNm(IpNumId);
	            HtmlChA += "<div class=\"project_device\">" + IpNumNm + "</div>\n";
            }
	        HtmlChA += "</div>\n";
        }
        HtmlChA += "</div>\n";
        // then zoom-in on the selected project
        HtmlChA += "<div class=\"project_details\">\n";
        // show project name
        const int SelProjId = ScoreProjIdV[SelProjIdN].Dat;
        TStr SelProjNm = BtaDevDefBs->GetProjNm(SelProjId);
        HtmlChA += "<div class=\"project_detail_title\">" + SelProjNm + "</div>\n";
        // show histogram
        HtmlChA += "<div class=\"project_detail_hist\">\n";
        // prepare histogram
        const TIntV& ProjEventV = BtaCorrBs->GetProjEventV(SelProjId);
        TStr PlotFNm = TFile::GetUniqueFNm("img/EventPlot.plt");
        PlotEventV("", ProjEventV, PlotFNm, 680, 200); PlotFNm.ChangeChAll('/', '-');
        HtmlChA += "<p><img src=\"" + PlotFNm + ".png\"></p>";
        HtmlChA += "</div>\n";
        HtmlChA += "<div class=\"project_detail_h2\">Last trap:</div>\n";
        HtmlChA += "<div class=\"project_detail_event_list\">\n";
        {
            // get the last event this project has fired
            const int LastEventN = ProjEventV.Last();
            const TBtaEvent& LastEvent = BtaEventBs->GetEvent(LastEventN);
            TStr IpNumNm = BtaDevDefBs->GetIpNumNm(LastEvent.GetIpNumId());       
            TBtaEventDat LastEventDat; 
            BtaEventBs->GetEventDat(LastEvent.GetBlobPt(), LastEventDat);
            // show it 
            HtmlChA += "<div class=\"project_detail_event\">\n";
            HtmlChA += "<b>Device ID:</b> " + IpNumNm + "<br>\n";
            HtmlChA += "<b>First Time:</b> " + LastEventDat.GetFirstTm() + "<br>\n";
            HtmlChA += "<b>Last Time:</b> " + LastEventDat.GetLastTm() + "<br>\n";
            HtmlChA += "<b>Severity:</b> " + LastEventDat.GetSeverity() + "<br>\n";
            HtmlChA += "<b>Summary:</b> " + LastEventDat.GetSummary() + "\n";
            HtmlChA += "</div>\n";
        }
        HtmlChA += "</div>\n";
        HtmlChA += "<div class=\"project_detail_h2\">Devices predicted to fail:</div>\n";
        HtmlChA += "<div class=\"project_detail_device_list\">\n";
        const TFltIntPrV& ProbIpNumIdV = ProbIpNumIdVH.GetDat(SelProjId);
        for (int IpNumN = 0; IpNumN < ProbIpNumIdV.Len(); IpNumN++) {
            const int IpNumId = ProbIpNumIdV[IpNumN].Val2;
            TStr IpNumNm = BtaDevDefBs->GetIpNumNm(IpNumId);       
            HtmlChA += "<div class=\"project_detail_device\">\n";
            HtmlChA += "<b>Device ID:</b> " + IpNumNm + "<br>\n";
            // get the last event this device has fired
            const int LastEventN = BtaCorrBs->GetIpNumEventV(IpNumId).Last();
            const TBtaEvent& LastEvent = BtaEventBs->GetEvent(LastEventN);
            TBtaEventDat LastEventDat; 
            BtaEventBs->GetEventDat(LastEvent.GetBlobPt(), LastEventDat);
            HtmlChA += "<b>Last trap:</b> " + LastEventDat.GetFirstTm() + "<br>\n";
            HtmlChA += "<b>Summary:</b> " + LastEventDat.GetSummary() + "\n";
            HtmlChA += "</div>\n";
        }
        HtmlChA += "</div>\n";
        HtmlChA += "</div>\n";
    }
    // prepare the footer
	HtmlChA += "</body></html>";
}


TStr TBTAServer::GetIpNumLink(const int& IpNumId) {
    TStr IpNumNm = BtaDevDefBs->GetIpNumNm(IpNumId);
    return TStr::Fmt("<a href=\"/device?id=%d\">%s</a>", IpNumId, IpNumNm.CStr());
} 

TStr TBTAServer::GetProjLink(const int& IpNumId) {
    TStr ProjStr;
    if (BtaCorrBs->IsIpNumProj(IpNumId)) {
        const int ProjId = BtaCorrBs->GetIpNumProjId(IpNumId);
        TStr ProjNm = BtaDevDefBs->GetProjNm(ProjId);
        ProjStr += TStr::Fmt("<a href=\"/project?id=%d\">%s</a>", ProjId, ProjNm.CStr());
    } else {
        ProjStr = "&nbsp;";
    }
    return ProjStr;
} 

TStr TBTAServer::GetProjLinkFromProj(const int& ProjId) {
    TStr ProjNm = BtaDevDefBs->GetProjNm(ProjId);
    return TStr::Fmt("<a href=\"/project?id=%d\">%s</a>", ProjId, ProjNm.CStr());
} 

TStr TBTAServer::GetBar(const double& Val, const double& _MxVal) {
    double MxVal = TFlt::GetMx(_MxVal, TFlt::Eps);
    if (MxVal < 1.0) { MxVal *= 1.2; }
    return TStr::Fmt("<div style=\"width: %d%%; background-color: #cc0033;\">&nbsp;</div>", 
        TInt::GetMn(TFlt::Round(100.0 * Val / MxVal), 100));
}

void TBTAServer::EventStat(const int& EventId, TChA& HtmlChA) {
    TBtaEvent Event = BtaEventBs->GetEvent(EventId);
    TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);
    HtmlChA += "<p><b>#</b> " + TInt(EventId+1).GetStr() + "</br>";
    HtmlChA += "<b>NodeId:</b> " + EventDat.GetNodeId() + "<br>";
    HtmlChA += "<b>First occurance:</b> " + EventDat.GetFirstTm() + "<br>";
    HtmlChA += "<b>Last occurance:</b> " + EventDat.GetLastTm() + "<br>";
    HtmlChA += "<b>IP:</b> " + GetIpNumLink(Event.GetIpNumId()) + "<br>";
    HtmlChA += "<b>Case Id:</b> " + EventDat.GetIR() + "<br>";
    HtmlChA += "<b>Severity:</b> " + EventDat.GetSeverity() + "<br>";
    HtmlChA += "<b>Project:</b> " + GetProjLinkFromProj(Event.GetProjId()) + "<br>";
    HtmlChA += "<b>Summary:</b> " + EventDat.GetSummary() + "</p>";
}

void TBTAServer::EventShortStat(const int& EventId, TChA& HtmlChA) {
    TBtaEvent Event = BtaEventBs->GetEvent(EventId);
    TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);
	HtmlChA += "<small><b>Trap " + TInt(EventId+1).GetStr() + ":</b></br>";
	HtmlChA += "<b>IP:</b> " + BtaDevDefBs->GetIpNumNm(Event.GetIpNumId()) + ", ";
    HtmlChA += "<b>Severity:</b> " + EventDat.GetSeverity() + ", ";
    HtmlChA += "<b>Project:</b> " + BtaDevDefBs->GetProjNm(Event.GetProjId()) + "<br>";
    HtmlChA += "<b>Summary:</b> " + EventDat.GetSummary() + "</small><br>";
}

void TBTAServer::EventFullStat(const int& EventId, TChA& HtmlChA) {
    TBtaEvent Event = BtaEventBs->GetEvent(EventId);
    TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);
    const TStrStrH& FldNmToValH = EventDat.FldNmToValH;
    int KeyId = FldNmToValH.FFirstKeyId();
    HtmlChA += "<p>";
    while (FldNmToValH.FNextKeyId(KeyId)) {
        TStr FldNm = FldNmToValH.GetKey(KeyId);
        TStr FldVal = FldNmToValH[KeyId];
        HtmlChA += "<b>" + FldNm + ":</b> " + FldVal + "<br>\n";
    }
    HtmlChA += "</p>";
}

void TBTAServer::EventRootCause(const int& EventId, TChA& HtmlChA) {
    // get list of related events
    TIntFltKdV EventIdScoreV; TStrV CorrTypeV;
    BtaCorrBs->EventRootCause(BtaEventBs, EventId, RootCouseEventWnd, 
        RootCouseTimeWnd, EventIdScoreV, CorrTypeV);
    // display
    HtmlChA += "<table border=\"1\">\n";
    DisplayEventHeader(false, true, true, HtmlChA);
    const int IpNumId = BtaEventBs->GetEvent(EventId).GetIpNumId();
    for (int EventIdScoreN = 0; EventIdScoreN < EventIdScoreV.Len(); EventIdScoreN++) {
        const int ScoreEventId = EventIdScoreV[EventIdScoreN].Key;
        const double Score = EventIdScoreV[EventIdScoreN].Dat;
        const int ScoreIpNumId = BtaEventBs->GetEvent(ScoreEventId).GetIpNumId();
        TBtaEvent ScoreEvent = BtaEventBs->GetEvent(ScoreEventId);
        TBtaEventDat ScoreEventDat; 
        BtaEventBs->GetEventDat(ScoreEvent.GetBlobPt(), ScoreEventDat);
        if (CorrTypeV[EventIdScoreN] == "Prob") {
            DisplayEventRow(ScoreEventId, ScoreEvent, ScoreEventDat, 
                false, true, Score, 1.0, ScoreIpNumId, IpNumId, HtmlChA);
        } else {
            DisplayEventRow(ScoreEventId, ScoreEvent, ScoreEventDat, 
                false, true, Score, 1.0, ScoreIpNumId, -1, HtmlChA);
        }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::EventShortRootCause(const int& EventId, TChA& HtmlChA) {
    // get list of related events
    TIntFltKdV EventIdScoreV; TStrV CorrTypeV;
    BtaCorrBs->EventRootCause(BtaEventBs, EventId, RootCouseEventWnd, 
        RootCouseTimeWnd, EventIdScoreV, CorrTypeV);
    // display
    HtmlChA += "<table border=\"1\">\n";
    DisplayShortEventHeader(true, HtmlChA);
    for (int EventIdScoreN = 0; EventIdScoreN < EventIdScoreV.Len(); EventIdScoreN++) {
        const int ScoreEventId = EventIdScoreV[EventIdScoreN].Key;
        const double Score = EventIdScoreV[EventIdScoreN].Dat;
        TBtaEvent ScoreEvent = BtaEventBs->GetEvent(ScoreEventId);
        TBtaEventDat ScoreEventDat; 
        BtaEventBs->GetEventDat(ScoreEvent.GetBlobPt(), ScoreEventDat);
        DisplayShortEventRow(ScoreEventId, ScoreEvent, 
            ScoreEventDat, true, Score, 1.0, HtmlChA);
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::EventPredIpNums(const int& EventId, TChA& HtmlChA) {
    // current event device
    const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
    const int CurrIpNumId = Event.GetIpNumId();
    const int CurrProjId = Event.GetProjId();
    // predict future devices
    TIntFltH IpNumIdProbH;
    BtaCorrBs->GetNextIpNumIdProb(EventId, 
        CurrProjId, BtaEventBs, 100, 600, IpNumIdProbH);
    if (IpNumIdProbH.Empty()) { return; }
    // sort according to probabilities
    TFltIntPrV ProbIpNumIdV; IpNumIdProbH.GetDatKeyPrV(ProbIpNumIdV); ProbIpNumIdV.Sort(false);
    // make a table
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>Device</td>";
    HtmlChA += "<td>Probability</td>";
    HtmlChA += "<td>Next alarm</td>";
    //HtmlChA += "<td>Time till next alarm</td>";
    HtmlChA += "</tr>\n";
    double MxProb = 0.0; int ProbIpNumIdN = 0, RowCount = 0;
    while (ProbIpNumIdN < ProbIpNumIdV.Len()) {
        const int IpNumId = ProbIpNumIdV[ProbIpNumIdN].Val2; 
        const int ProjId = BtaCorrBs->GetIpNumProjId(IpNumId);
        ProbIpNumIdN++; if (ProjId != CurrProjId) { continue; }
        const double Prob = ProbIpNumIdV[ProbIpNumIdN].Val1;
        if (RowCount == 0) { MxProb = Prob; }
        HtmlChA += "<tr>";
        HtmlChA += "<td>" + TInt(RowCount+1).GetStr() + "</td>";
        HtmlChA += "<td>" + GetIpNumLink(IpNumId) + " (" + GetProjLink(IpNumId) + ")</td>";
        HtmlChA += "<td>" + GetBar(Prob, MxProb) + "</td>";
        const int NextEventN = BtaEventBs->GetNextEvent(EventId+1, IpNumId);
        if (NextEventN != -1) {
            const TBtaEvent& NextEvent = BtaEventBs->GetEvent(NextEventN);
            //const uint64 TimeDiffMSec = NextEvent.GetFirstTmMSecs() - Event.GetFirstTmMSecs();
            //const int TimeDiffSec = int(TimeDiffMSec / uint64(1000));
            HtmlChA += TStr::Fmt("<td><a href=\"/event?id=%d\">%d</a></td>", NextEventN, NextEventN);
            //HtmlChA += "<td>" + TInt(TimeDiffSec).GetStr() + " seconds</td>";
        } else {
            HtmlChA += "<td>--</td>";
            //HtmlChA += "<td>--</td>";
        }
        HtmlChA += "</tr>\n";
        RowCount++; if (RowCount >= 5) { break; }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::EventShortPredIpNums(const int& EventId, TChA& HtmlChA) {
    // current event device
    const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
    const int CurrIpNumId = Event.GetIpNumId();
    const int CurrProjId = Event.GetProjId();
    // predict future devices
    TIntFltH IpNumIdProbH;
    BtaCorrBs->GetNextIpNumIdProb(EventId, 
        CurrProjId, BtaEventBs, 100, 600, IpNumIdProbH);
    if (IpNumIdProbH.Empty()) { return; }
    // sort according to probabilities
    TFltIntPrV ProbIpNumIdV; IpNumIdProbH.GetDatKeyPrV(ProbIpNumIdV); ProbIpNumIdV.Sort(false);
    // make a table
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>Device</td>";
    HtmlChA += "<td>Probability</td>";
    HtmlChA += "</tr>\n";
    double MxProb = 0.0; int ProbIpNumIdN = 0, RowCount = 0;
    while (ProbIpNumIdN < ProbIpNumIdV.Len()) {
        const int IpNumId = ProbIpNumIdV[ProbIpNumIdN].Val2; 
        const int ProjId = BtaCorrBs->GetIpNumProjId(IpNumId);
        ProbIpNumIdN++; if (ProjId != CurrProjId) { continue; }
        const double Prob = ProbIpNumIdV[ProbIpNumIdN].Val1;
        if (RowCount == 0) { MxProb = Prob; }
        HtmlChA += "<tr>";
        HtmlChA += "<td>" + TInt(RowCount+1).GetStr() + "</td>";
        HtmlChA += "<td>" + GetIpNumLink(IpNumId) + " (" + GetProjLink(IpNumId) + ")</td>";
        HtmlChA += "<td>" + GetBar(Prob, MxProb) + "</td>";
        HtmlChA += "</tr>\n";
        RowCount++; if (RowCount >= 5) { break; }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::DisplayEventHeader(const bool& HdSortP, 
        const bool& ScoreP, const bool& LinkP, TChA& HtmlChA) {

    HtmlChA += "<tr>";
    if (HdSortP) {
        HtmlChA += "<td># (<a href=\"/events?sort=all\">all</a>)</td>";
        HtmlChA += "<td>First</td>";
        HtmlChA += "<td>Last</td>";
        HtmlChA += "<td>Device IP</td>";
        HtmlChA += "<td>CaseID (<a href=\"/events?sort=case_id\">filter</a>)</td>";
    } else { 
        HtmlChA += "<td>#</td>";
        HtmlChA += "<td>First</td>";
        HtmlChA += "<td>Device IP</td>";
        HtmlChA += "<td>CaseID (IR)</td>";
    }
    HtmlChA += "<td>Severity</td>";
    HtmlChA += "<td>Project</td>";
    HtmlChA += "<td width=\"250\">Summary</td>";
    if (ScoreP) { HtmlChA += "<td>Score</td>"; }
    if (LinkP) { HtmlChA += "<td></td>"; }
    HtmlChA += "</tr>\n";
}

void TBTAServer::DisplayEventRow(const int& EventN, const TBtaEvent& Event, 
        const TBtaEventDat& EventDat, const bool& HdSortP, const bool& ScoreP, 
        const double& Score, const double& MxScore, const int& IpNumId, 
        const int& CorrIpNumId, TChA& HtmlChA) {

    HtmlChA += "<tr>";
    HtmlChA += TStr::Fmt("<td><a href=\"/event?id=%d\">%d</a> ", EventN, EventN);
    HtmlChA += TStr::Fmt("(<a onClick=\"window.open('/related_traps?id=%d\','omnibus_view','menubar=0,resizable=1,scrollbars=1,width=600,height=500');\">Omnibus</a>)</td>", EventN);
    HtmlChA += "<td>" + EventDat.GetFirstTm() + "</td>";
    if (HdSortP) { HtmlChA += "<td>" + EventDat.GetLastTm() + "</td>"; }
    HtmlChA += "<td>" + GetIpNumLink(Event.GetIpNumId()) + "</td>";
    HtmlChA += "<td>" + EventDat.GetIR() + "</td>";
    HtmlChA += "<td>" + EventDat.GetSeverity() + "</td>";
    HtmlChA += "<td>" + GetProjLinkFromProj(Event.GetProjId()) + "</td>";
    HtmlChA += "<td><small>" + EventDat.GetSummary() + "</small></td>";
    if (ScoreP) { 
        HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(Score, MxScore).CStr()); 
    }
    if (IpNumId != -1 && CorrIpNumId != -1) {
        HtmlChA += TStr::Fmt("<td><a href=\"/dd_events?id=%d&corrid=%d\">historic co-occucerence</a></td>",
            IpNumId, CorrIpNumId);
    } else if (IpNumId != -1 && CorrIpNumId == -1) {
        HtmlChA += "<td>similar summary text</td>";
    }
    HtmlChA += "</tr>\n";
}

void TBTAServer::DisplayShortEventHeader(const bool& ScoreP, TChA& HtmlChA) {
    HtmlChA += "<tr>";
    HtmlChA += "<td>Trap</td>";
    HtmlChA += "<td>Device</td>";
    HtmlChA += "<td width=\"150\">Summary</td>";
    if (ScoreP) { HtmlChA += "<td width=\"20\"></td>"; }
    HtmlChA += "</tr>\n";
}

void TBTAServer::DisplayShortEventRow(const int& EventN, const TBtaEvent& Event, 
        const TBtaEventDat& EventDat, const bool& ScoreP, const double& Score, 
        const double& MxScore, TChA& HtmlChA) {

    HtmlChA += "<tr>";
    HtmlChA += "<td><a href=\"/related_traps?id=" + TInt::GetStr(EventN) + "\">" + 
        TInt(EventN).GetStr() + "</a></td>";
    HtmlChA += "<td>" + BtaDevDefBs->GetIpNumNm(Event.GetIpNumId()) + "</td>";
    HtmlChA += "<td><small>" + EventDat.GetSummary() + "</small></td>";
    if (ScoreP) { HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(Score, MxScore).CStr()); }
    HtmlChA += "</tr>\n";
}

void TBTAServer::MakeEventList(const TIntV& EventV, 
        const bool& HdSortP, const int& DispPageN, TChA& HtmlChA) {

    // header
    HtmlChA += "<table border=\"1\">\n";
    DisplayEventHeader(HdSortP, false, false, HtmlChA);
    // list devices 
    const int Events = EventV.Len(); 
    int EventN = Events - DispPageN*ListEventsPerPage;
    const int MnEventN = TInt::GetMx(Events - (DispPageN+1)*ListEventsPerPage, 0);
    while (EventN > MnEventN) {
        EventN--;
        // get event and the data
        TBtaEvent Event = BtaEventBs->GetEvent(EventV[EventN]);
        TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);
        if (Event.GetIpNumId() != -1 && Event.GetProjId() != -1) {
            DisplayEventRow(EventV[EventN], Event, 
                EventDat, HdSortP, false, 0.0, 0.1, -1, -1, HtmlChA);
        } else { 
            printf("Error: EventN == %d, IpNumId == %d, ProjId  == %d\n", 
                EventV[EventN].Val, Event.GetIpNumId(), Event.GetProjId()); 
        }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::MakeEventPrList(const TIntPrV& EventPrV, const int& DispPageN, TChA& HtmlChA) {
    // group alarms by first alarm
    TIntIntVH EventIdIdVH; 
    const int EventPrs = EventPrV.Len();
    for (int EventPrN = 0; EventPrN < EventPrs; EventPrN++) {
        const TIntPr& EventPr = EventPrV[EventPrs - EventPrN - 1];
        EventIdIdVH.AddDat(EventPr.Val1).Add(EventPr.Val2);
    }
    // header
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>Time</td>";
    HtmlChA += "<td>Device IP</td>";
    HtmlChA += "<td>Summary</td>";
    HtmlChA += "<td>Correlated with event sequence</td>";
    HtmlChA += "</tr>\n";
    // list devices 
    const int Events = EventIdIdVH.Len(); 
    int EventKeyId = EventIdIdVH.FFirstKeyId(), PageN = 0, EventsOnLastPage = 0;
    while (EventIdIdVH.FNextKeyId(EventKeyId) && !(PageN > DispPageN)) {
        if (PageN == DispPageN) {
            const int EventId = EventIdIdVH.GetKey(EventKeyId);
            TBtaEvent Event = BtaEventBs->GetEvent(EventId);
            TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);

            HtmlChA += "<tr valign=\"top\">";
            HtmlChA += "<td><a href=\"/event?id=" + TInt::GetStr(EventId) + 
                "\">" + TInt(EventId).GetStr() + "</a></td>";
            HtmlChA += "<td>" + EventDat.GetFirstTm() + "</td>";
            HtmlChA += "<td>" + GetIpNumLink(Event.GetIpNumId()) + "</td>";
            HtmlChA += "<td>" + TruncStr(EventDat.GetSummary(), 60) + "</td>";
            HtmlChA += "<td>\n";
            // print correlated events
            HtmlChA += "<table border=\"1\">";
            HtmlChA += "<tr>";
            HtmlChA += "<td>#</td>";
            HtmlChA += "<td>Time</td>";
            HtmlChA += "<td>Device IP</td>";
            HtmlChA += "<td>Summary</td>";
            HtmlChA += "</tr>\n";
            const TIntV& CorrEventIdV = EventIdIdVH[EventKeyId];
            for (int CorrEventIdN = 0; CorrEventIdN < CorrEventIdV.Len(); CorrEventIdN++) {
                const int CorrEventId = CorrEventIdV[CorrEventIdN];
                TBtaEvent CorrEvent = BtaEventBs->GetEvent(CorrEventId);
                TBtaEventDat CorrEventDat; BtaEventBs->GetEventDat(
                    CorrEvent.GetBlobPt(), CorrEventDat);
                HtmlChA += "<tr valign=\"top\">";
                HtmlChA += "<td><a href=\"/event?id=" + TInt::GetStr(CorrEventId) + 
                    "\">" + TInt(CorrEventId).GetStr() + "</a></td>";
                HtmlChA += "<td>" + CorrEventDat.GetFirstTm() + "</td>";
                HtmlChA += "<td>" + GetIpNumLink(CorrEvent.GetIpNumId()) + "</td>";
                HtmlChA += "<td>" + TruncStr(CorrEventDat.GetSummary(), 60) + "</td>";
                HtmlChA += "</tr>";
            }
            HtmlChA += "</table>\n";
            HtmlChA += "</td></tr>\n";
            // space between correlations
            HtmlChA += "<tr><td colspan=\"5\">&nbsp;</td></tr>";
        }
        // do the counting
        EventsOnLastPage += EventIdIdVH[EventKeyId].Len();
        if (EventsOnLastPage > ListEventsPerPage) { 
            PageN++; EventsOnLastPage = 0; // move to next page
        }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::MakeShortEventList(const TIntV& EventV, TChA& HtmlChA) {
    // header
    HtmlChA += "<table border=\"1\">\n";
    DisplayShortEventHeader(false, HtmlChA);
    // list devices 
    const int Events = TInt::GetMn(ListEventsPerPage, EventV.Len());
    for (int EventN = 0; EventN < Events; EventN++) {
        // get event and the data
        TBtaEvent Event = BtaEventBs->GetEvent(EventV[EventN]);
        TBtaEventDat EventDat; BtaEventBs->GetEventDat(Event.GetBlobPt(), EventDat);
        if (Event.GetIpNumId() != -1 && Event.GetProjId() != -1) {
            DisplayShortEventRow(EventV[EventN], Event, EventDat, false, 0.0, 1.0, HtmlChA);
        } else { 
            printf("Error: EventN == %d, IpNumId == %d, ProjId  == %d\n", 
                EventV[EventN].Val, Event.GetIpNumId(), Event.GetProjId()); 
        }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::IpNumDevCorrStat(const int& IpNumId, TChA& HtmlChA) {
    TIntFltKdV CorrIpNumIdV;
    BtaCorrBs->GetIpNumCorrDevices(IpNumId, true, DetailItemsPerPage, CorrIpNumIdV);
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>Device</td>";
    HtmlChA += "<td>Correlation</td>";
    HtmlChA += "<td>When does it fire alarms?</td>";
    HtmlChA += "</tr>\n";
    for (int CorrIpNumIdN = 0; CorrIpNumIdN < CorrIpNumIdV.Len(); CorrIpNumIdN++) {
        const int CorrIpNumId = CorrIpNumIdV[CorrIpNumIdN].Key;
        const double CorrProb = TFlt::Abs(CorrIpNumIdV[CorrIpNumIdN].Dat);
        const int Sign = TFlt::Sign(CorrIpNumIdV[CorrIpNumIdN].Dat);
        const double MxCorrProb = TFlt::Abs(CorrIpNumIdV[0].Dat);
        if (CorrProb < MnCorrProb) { break; }
        HtmlChA += "<tr>";
        HtmlChA += "<td>" + TInt(CorrIpNumIdN+1).GetStr() + "</td>";
        HtmlChA += "<td>" + GetIpNumLink(CorrIpNumId) + " (" + GetProjLink(CorrIpNumId) + ")</td>";
        HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(CorrProb, MxCorrProb).CStr());            
        if (Sign > 0) { 
            HtmlChA += TStr::Fmt("<td>after (<a href=\"/dd_events?id=%d&corrid=%d\">correlated alarms</a>)</td>",
                IpNumId, CorrIpNumId);
        } else { 
            HtmlChA += TStr::Fmt("<td>before (<a href=\"/dd_events?id=%d&corrid=%d\">correlated alarms</a>)</td>",
                CorrIpNumId, IpNumId);
        }

        HtmlChA += "</tr>\n";
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::ShortIpNumStat(const int& IpNumId, TChA& HtmlChA) {
    TStr IpNumNm = BtaDevDefBs->GetIpNumNm(IpNumId);
    HtmlChA += TStr::Fmt("<p><b>Device IP:</b> %s<br>\n", 
        GetIpNumLink(IpNumId).CStr());
    HtmlChA += TStr::Fmt("<b>Device project:</b> %s<br>\n", 
        GetProjLink(IpNumId).CStr());
    HtmlChA += TStr::Fmt("<b>Number of alarms:</b> %d</p>\n", 
        BtaCorrBs->GetIpNumIdEvents(IpNumId));
}

void TBTAServer::MakeIpNumPrList(const TIntV& DevKeyIdV, const int& DispPageN, TChA& HtmlChA) {
    // header
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>First device</td>";
    HtmlChA += "<td>Second device</td>";
    HtmlChA += "<td>Correlation</td>";
    HtmlChA += "<td></td>";
    HtmlChA += "</tr>\n";
    // get first correlation
    const TIntPrIntPrVH& IpNumIdPrToFqH = BtaCorrBs->GetIpNumIdPrH();
    const TIntPr& FirstIpNumIdPr = IpNumIdPrToFqH.GetKey(DevKeyIdV[0]);
    const double MxCorrProb = BtaCorrBs->ProbCorrIpNumIdGivenIpNumId(
        FirstIpNumIdPr.Val1, FirstIpNumIdPr.Val2);
    // list devices 
    const int StartItemN = DispPageN*ListItemsPerPage;
    const int EndItemN = (DispPageN+1)*ListItemsPerPage;
    const int DevKeyIds = DevKeyIdV.Len();
    int DevKeyIdN = 0, ItemN = 0; 
    while (DevKeyIdN < DevKeyIds && ItemN < EndItemN) {
        const TIntPr& IpNumIdPr = IpNumIdPrToFqH.GetKey(DevKeyIdV[DevKeyIdN]);
        const int IpNumId = IpNumIdPr.Val1, CorrIpNumId = IpNumIdPr.Val2;
        DevKeyIdN++;
        // check if we should display it
        if (IpNumId == CorrIpNumId) { continue; }
        ItemN++; if (ItemN < StartItemN) { continue; }
        // we should ...
        HtmlChA += "<tr>";
        HtmlChA += "<td>" + TInt(ItemN).GetStr() + "</td>";
        HtmlChA += "<td>" + GetIpNumLink(IpNumId) + " (" + GetProjLink(IpNumId) + ")</td>";
        HtmlChA += "<td>" + GetIpNumLink(CorrIpNumId) + " (" + GetProjLink(CorrIpNumId) + ")</td>";
        // device correlations
        if (BtaCorrBs->IsIpNumId(IpNumId)) {
            const double CorrProb = BtaCorrBs->ProbCorrIpNumIdGivenIpNumId(IpNumId, CorrIpNumId);
            HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(CorrProb, MxCorrProb).CStr());
        } else {
            HtmlChA += "<td></td>";
        }
        HtmlChA += TStr::Fmt("<td><a href=\"/dd_events?id=%d&corrid=%d\">correlated alarms</a></td>",
            IpNumId, CorrIpNumId);
        HtmlChA += "</tr>\n";
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::ProjWithinCorrStat(const int& ProjId, TChA& HtmlChA) {
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>First device</td>";
    HtmlChA += "<td>Second device</td>";
    HtmlChA += "<td>Correlation</td>";
    HtmlChA += "<td></td>";
    HtmlChA += "</tr>\n";
    // calculate the most correlated devices
    TIntPrFltKdV CorrIpNumIdPrProbV;
    BtaCorrBs->GetProjCorrDevices(ProjId, 
        DetailItemsPerPage, CorrIpNumIdPrProbV);
    const int IpNumPrs = CorrIpNumIdPrProbV.Len();
    if (IpNumPrs > 0) {
        double MxCorrProb = CorrIpNumIdPrProbV[0].Dat;
        for (int IpNumPrN = 0; IpNumPrN < IpNumPrs; IpNumPrN++) {
            // get the devices
            const int IpNumId = CorrIpNumIdPrProbV[IpNumPrN].Key.Val1;
            const int CorrIpNumId = CorrIpNumIdPrProbV[IpNumPrN].Key.Val2;
            // device correlations
            const double CorrProb = CorrIpNumIdPrProbV[IpNumPrN].Dat;
            if (CorrProb < MnCorrProb) { break; }
            // print row
            HtmlChA += "<tr>";
            HtmlChA += "<td>" + TInt(IpNumPrN+1).GetStr() + "</td>";
            HtmlChA += "<td>" + GetIpNumLink(IpNumId) + "</td>";
            HtmlChA += "<td>" + GetIpNumLink(CorrIpNumId) + "</td>";
            HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(CorrProb, MxCorrProb).CStr());
            HtmlChA += TStr::Fmt("<td><a href=\"/dd_events?id=%d&corrid=%d\">correlated alarms</a></td>",
                IpNumId, CorrIpNumId);
            HtmlChA += "</tr>\n";
        }
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::ShortProjStat(const int& ProjId, TChA& HtmlChA, const bool& ProjNmLinkP) {
    TStr ProjNm = BtaDevDefBs->GetProjNm(ProjId);
    if (!ProjNmLinkP) { 
        HtmlChA += TStr::Fmt("<p>Project name: <b>%s</b></p>\n", 
            ProjNm.CStr()); 
    } else { 
        HtmlChA += TStr::Fmt("<p>Project name: <b>%s</b></p>\n", 
            GetProjLinkFromProj(ProjId).CStr()); 
    }
    // get trend counts
    int BeforeCount, WeekAfterCount, MonthAfterCount;
    TTm StartDate = BtaEventBs->GetFirstTime(), EndDate = BtaEventBs->GetLastTime();
    TTm WeekBreakDate = GetBreakDate(7), MonthBreakDate = GetBreakDate(30);
    PBtaLongProj LongProj = BtaLongBs->GetLongProjFromId(ProjId);
    LongProj->Count(StartDate, WeekBreakDate, EndDate, BeforeCount, WeekAfterCount);
    LongProj->Count(StartDate, MonthBreakDate, EndDate, BeforeCount, MonthAfterCount);
    const int TotalCount = BeforeCount + MonthAfterCount;
    const bool IsWeekTrendPosP = LongProj->IsTrendPos(StartDate, WeekBreakDate, EndDate);
    const bool IsMonthTrendPosP = LongProj->IsTrendPos(StartDate, MonthBreakDate, EndDate);
    const double WeekChiProb = LongProj->ChiSquare(StartDate, WeekBreakDate, EndDate);
    const double MonthChiProb = LongProj->ChiSquare(StartDate, MonthBreakDate, EndDate);  
    // print trend counts
    HtmlChA += TStr::Fmt("</p>Number of alarms: <b>%d</b> (severity > 2)<br>\n", TotalCount);
    if (WeekChiProb < 0.05) {
        HtmlChA += TStr::Fmt("Number of alarms in last 7 days: <b>%d</b> (%s)<br>\n", 
            WeekAfterCount, IsWeekTrendPosP ? "increasing" : "decreasing");
    } else {
        HtmlChA += TStr::Fmt("Number of alarms in last 7 days: <b>%d</b><br>\n", WeekAfterCount);
    }
    if (MonthChiProb < 0.05) {
        HtmlChA += TStr::Fmt("Number of alarms in last 30 days: <b>%d</b> (%s)</p>\n", 
            MonthAfterCount, IsMonthTrendPosP ? "increasing" : "decreasing");
    } else {
        HtmlChA += TStr::Fmt("Number of alarms in last 30 days: <b>%d</b></p>\n", MonthAfterCount);
    }
}

void TBTAServer::MakeLongProjList(const TIntV& SortProjIdV, 
        const int& DispPageN, const TStr& PeriodStr, TChA& HtmlChA) {

    // header
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>Project</td>";
    HtmlChA += "<td>Significance</td>";
    //HtmlChA += "<td>Trend</td>";
    HtmlChA += "<td></td>";
    HtmlChA += "</tr>\n";
    // list projects 
    TTm StartDate = BtaEventBs->GetFirstTime();
    TTm BreakDate = GetBreakDate(PeriodStr == "week" ? 7 : 30);
    TTm EndDate = BtaEventBs->GetLastTime();
    const int Projs = TInt::GetMn((DispPageN+1)*ListItemsPerPage, SortProjIdV.Len());
    for (int ProjN = DispPageN*ListItemsPerPage; ProjN < Projs; ProjN++) {
        // get project and the data
        const int ProjId = SortProjIdV[ProjN];
        PBtaLongProj LongProj = BtaLongBs->GetLongProjFromId(ProjId);
        const double ChiProb = MxChiProb - LongProj->ChiSquare(StartDate, BreakDate, EndDate);
        int BeforeCount, AfterCount; LongProj->Count(
            StartDate, BreakDate, EndDate, BeforeCount, AfterCount);
        const bool IsTrendPosP = LongProj->IsTrendPos(StartDate, BreakDate, EndDate);
        HtmlChA += "<tr>";
        HtmlChA += TStr::Fmt("<td>%d</td>", ProjN + 1);
        HtmlChA += TStr::Fmt("<td>%s</td>", GetProjLinkFromProj(ProjId).CStr());
        HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(ChiProb, MxChiProb).CStr());
        //HtmlChA += TStr::Fmt("<td>%s</td>", IsTrendPosP ? "+" : "-");
        HtmlChA += TStr::Fmt("<td><a href=\"/p_long?id=%d&period=%s\">Show Details</td>",
            ProjId, PeriodStr.CStr());
        HtmlChA += "</tr>\n";
    }
    HtmlChA += "</table>\n";
}

void TBTAServer::MakeLongIpNumList(const int& ProjId, 
        const int& DispPageN, const TStr& PeriodStr, TChA& HtmlChA) {

    // header
    HtmlChA += "<table border=\"1\">\n";
    HtmlChA += "<tr>";
    HtmlChA += "<td>#</td>";
    HtmlChA += "<td>Device IP</td>";
    HtmlChA += "<td>Significance</td>";
    HtmlChA += "<td>Avg. alarms</td>";
    HtmlChA += "<td>In last " + PeriodStr + "</td>";
    HtmlChA += "<td>Trend</td>";
    HtmlChA += "</tr>\n";
    // prepare dates
    TTm StartDate = BtaEventBs->GetFirstTime();
    TTm BreakDate = GetBreakDate(PeriodStr == "week" ? 7 : 30);
    TTm EndDate = BtaEventBs->GetLastTime();
    // get weigthed list of devices
    PBtaLongProj LongProj = BtaLongBs->GetLongProjFromId(ProjId);
    TIntFltKdV IpNumIdWgtV; LongProj->GetIpNumWgtV(StartDate, BreakDate, EndDate, IpNumIdWgtV);
    // list devices 
    const double MxIpNumWgt = IpNumIdWgtV[0].Dat;
    const int IpNums = TInt::GetMn((DispPageN+1)*ListItemsPerPage, IpNumIdWgtV.Len());
    for (int IpNumN = DispPageN*ListItemsPerPage; IpNumN < IpNums; IpNumN++) {
        const int IpNumId = IpNumIdWgtV[IpNumN].Key;
        const double IpNumWgt = IpNumIdWgtV[IpNumN].Dat;
        if ((IpNumWgt / MxIpNumWgt) < MnChiVal) { break; }
        double AvgCount; int LastCount; 
        GetIpNumEventCount(IpNumId, StartDate, BreakDate, EndDate, AvgCount, LastCount);
        const bool IsTrendPosP = (double(LastCount) > AvgCount);
        HtmlChA += "<tr>";
        HtmlChA += TStr::Fmt("<td>%d</td>", IpNumN + 1);
        HtmlChA += TStr::Fmt("<td>%s</td>", GetIpNumLink(IpNumId).CStr());
        HtmlChA += TStr::Fmt("<td>%s</td>", GetBar(IpNumWgt, MxIpNumWgt).CStr());
        HtmlChA += TStr::Fmt("<td>%.2f</td>", AvgCount);
        HtmlChA += TStr::Fmt("<td>%d</td>", LastCount);
        HtmlChA += TStr::Fmt("<td>%s</td>", IsTrendPosP ? "+" : "-");
        HtmlChA += "</tr>\n";
    }
    HtmlChA += "</table>\n";
}


void TBTAServer::RunGnuPlot(const TStr& PlotFNm) const {
  TStr GPPath = GnuPlotPath;
  IAssertR(TFile::Exists(GPPath), TStr("GnuPlot not found at '")+GnuPlotPath+"'. Set TGnuPlot::GnuPlotPath.");
  system(TStr::Fmt("%s %s", GPPath.CStr(), PlotFNm.CStr()).CStr());
}

void TBTAServer::PlotEventV(const TStr& TitleStr, const TIntV& EventV, 
        const TStr& PlotFNm, const int& Width, const int& Height) {

    // count events int buckets of days
    TUInt64H DateCountH;
    for (int EventIdN = 0; EventIdN < EventV.Len(); EventIdN++) {
        const int EventId = EventV[EventIdN];
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
        TTm EventTm = Event.GetFirstTm();
        TTm DayEventTm(EventTm.GetYear(), EventTm.GetMonth(), EventTm.GetDay());
        DateCountH.AddDat(TTm::GetMSecsFromTm(DayEventTm))++;
    }
    // sort by date
    TUInt64IntPrV DateCountV;
    DateCountH.GetKeyDatPrV(DateCountV);
    DateCountV.Sort(true);
    // save data and get max count
    int MxCount = 0;
    {
        TFOut DataFOut(PlotFNm + ".tab"); 
        DataFOut.PutStrLn("# day count");
        for (int DayN = 0; DayN < DateCountV.Len(); DayN++) {
            // get mx count
            MxCount = TInt::GetMx(MxCount, DateCountV[DayN].Val2); 
            // save data
            DataFOut.PutStrLn(TStr::Fmt("%d %d", DayN+1, DateCountV[DayN].Val2));
        }
        DataFOut.Flush();
    }
    double MxCountFlt = (double)MxCount * 1.15;
    // save ploting program
    {
        TFOut PlotFOut(PlotFNm);
        PlotFOut.PutStrLn("# event plot");
        PlotFOut.PutStrLn(TStr::Fmt("set autoscale"));
        PlotFOut.PutStrLn(TStr::Fmt("set xrange [0:%d]", DateCountV.Len() + 1));
        PlotFOut.PutStrLn(TStr::Fmt("set yrange [0:%.2f]", MxCountFlt));
        PlotFOut.PutStrLn(TStr::Fmt("set terminal png large size %d,%d", Width, Height));
        PlotFOut.PutStrLn(TStr::Fmt("set output '%s.png'", PlotFNm.CStr()));
        PlotFOut.PutStrLn(TStr::Fmt("set boxwidth 0.8"));
        // date marks
        PlotFOut.PutStr(TStr::Fmt("set xtics rotate  ( "));
        const int Dates = DateCountV.Len() > 10 ? 10 : DateCountV.Len();
        double DayDiff = double(DateCountV.Len()) / double(Dates);
        for (int DateN = 0; DateN < Dates; DateN++) {
            const int DayN = TFlt::Round(DateN * DayDiff);
            IAssertR(0 <= DayN && DayN < DateCountV.Len(), TInt::GetStr(DayN));
            const uint64 DateMSecs = DateCountV[DayN].Val1.Val;
            TStr DateStr = TTm::GetTmFromMSecs(DateMSecs).GetWebLogDateStr();
            if (DateN != 0) { PlotFOut.PutStr(", "); }
            PlotFOut.PutStr(TStr::Fmt("\" %s\" %d", DateStr.CStr(), DayN+1));
        }
        // finish 
        PlotFOut.PutStrLn(")");
        PlotFOut.PutStrLn("set xlabel \"Time\"");
        //PlotFOut.PutStrLn("set ylabel \"Number of alarms\"");
        PlotFOut.PutStrLn(TStr::Fmt("plot \"%s.tab\" using 1:2 title \"%s\" with boxes", 
            PlotFNm.CStr(), TitleStr.CStr()));
    }
    // get png
    RunGnuPlot(PlotFNm);
}

void TBTAServer::PlotEventV(const TStr& TitleStr, const TIntV& EventV, 
        const TStr& CorrTitleStr, const TIntV& CorrEventV, const TStr& PlotFNm) {

    // count events int buckets of days
    TUInt64IntPrH DateCountH;
    for (int EventIdN = 0; EventIdN < EventV.Len(); EventIdN++) {
        const int EventId = EventV[EventIdN];
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
        TTm EventTm = Event.GetFirstTm();
        TTm DayEventTm(EventTm.GetYear(), EventTm.GetMonth(), EventTm.GetDay());
        DateCountH.AddDat(TTm::GetMSecsFromTm(DayEventTm)).Val1++;
    }
    for (int EventIdN = 0; EventIdN < CorrEventV.Len(); EventIdN++) {
        const int EventId = CorrEventV[EventIdN];
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
        TTm EventTm = Event.GetFirstTm();
        TTm DayEventTm(EventTm.GetYear(), EventTm.GetMonth(), EventTm.GetDay());
        DateCountH.AddDat(TTm::GetMSecsFromTm(DayEventTm)).Val2++;
    }
    // sort by date
    TUInt64IntPrPrV DateCountV;
    DateCountH.GetKeyDatPrV(DateCountV);
    DateCountV.Sort(true);
    // save data and get max count
    int MxCount = 0;
    {
        TFOut DataFOut(PlotFNm + ".tab"); 
        DataFOut.PutStrLn("# day count");
        for (int DayN = 0; DayN < DateCountV.Len(); DayN++) {
            // get mx count
            MxCount = TInt::GetMx(MxCount, DateCountV[DayN].Val2.Val1); 
            MxCount = TInt::GetMx(MxCount, DateCountV[DayN].Val2.Val2); 
            // save data
            DataFOut.PutStrLn(TStr::Fmt("%d %d %d", DayN+1, 
                DateCountV[DayN].Val2.Val1, DateCountV[DayN].Val2.Val2));
        }
        DataFOut.Flush();
    }
    double MxCountFlt = (double)MxCount * 1.25;
    // save ploting program
    {
        TFOut PlotFOut(PlotFNm);
        PlotFOut.PutStrLn("# event plot");
        PlotFOut.PutStrLn(TStr::Fmt("set autoscale"));
        PlotFOut.PutStrLn(TStr::Fmt("set xrange [0:%d]", DateCountV.Len() + 1));
        PlotFOut.PutStrLn(TStr::Fmt("set yrange [0:%.2f]", MxCountFlt));
        PlotFOut.PutStrLn(TStr::Fmt("set terminal png large size 700,300"));
        PlotFOut.PutStrLn(TStr::Fmt("set output '%s.png'", PlotFNm.CStr()));
        PlotFOut.PutStrLn(TStr::Fmt("set boxwidth 0.4"));
        // date marks
        PlotFOut.PutStr(TStr::Fmt("set xtics rotate  ( "));
        const int Dates = DateCountV.Len() > 10 ? 10 : DateCountV.Len();
        double DayDiff = double(DateCountV.Len()) / double(Dates);
        for (int DateN = 0; DateN < Dates; DateN++) {
            const int DayN = TFlt::Round(DateN * DayDiff);
            IAssertR(0 <= DayN && DayN < DateCountV.Len(), TInt::GetStr(DayN));
            const uint64 DateMSecs = DateCountV[DayN].Val1.Val;
            TStr DateStr = TTm::GetTmFromMSecs(DateMSecs).GetWebLogDateStr();
            if (DateN != 0) { PlotFOut.PutStr(", "); }
            PlotFOut.PutStr(TStr::Fmt("\" %s\" %d", DateStr.CStr(), DayN+1));
        }
        // finish 
        PlotFOut.PutStrLn(")");
        PlotFOut.PutStrLn("set xlabel \"Time\"");
        PlotFOut.PutStrLn("set ylabel \"Number of alarms\"");
        PlotFOut.PutStr(TStr::Fmt("plot \"%s.tab\" using ($1-0.22):2 title \"%s\" with boxes, ", 
            PlotFNm.CStr(), TitleStr.CStr()));
        PlotFOut.PutStrLn(TStr::Fmt("\"%s.tab\" using ($1+0.22):3 title \"%s\" with boxes", 
            PlotFNm.CStr(), CorrTitleStr.CStr()));
    }
    // get png
    RunGnuPlot(PlotFNm);
}

void TBTAServer::PlotLongIpNum(const TStr& TitleStr, const TStr& PlotFNm,
        const int& ProjId, const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, const TStr& PeriodStr, const int& MxIpNums, 
        const bool& BigP) {

    // get weigthed sorted list of devices
    PBtaLongProj LongProj = BtaLongBs->GetLongProjFromId(ProjId);
    TIntFltKdV IpNumIdWgtV; LongProj->GetIpNumWgtV(
        StartDate, BreakDate, EndDate, IpNumIdWgtV);
    if (IpNumIdWgtV.Len() > MxIpNums) { IpNumIdWgtV.Trunc(MxIpNums); }
    // save data and get max count
    int MxCount = 0;
    {
        TFOut DataFOut(PlotFNm + ".tab"); 
        DataFOut.PutStrLn("# ipnum count");
        for (int IpNumN = 0; IpNumN < IpNumIdWgtV.Len(); IpNumN++) {
            // get counts
            const int IpNumId = IpNumIdWgtV[IpNumN].Key;
            double AvgCount; int LastCount; 
            GetIpNumEventCount(IpNumId, StartDate, 
                    BreakDate, EndDate, AvgCount, LastCount);
            // get mx count
            MxCount = TInt::GetMx(MxCount, int(ceil(AvgCount))); 
            MxCount = TInt::GetMx(MxCount, LastCount); 
            // save data
            DataFOut.PutStrLn(TStr::Fmt("%d %.2f %d", IpNumN+1, AvgCount, LastCount));
        }
        DataFOut.Flush();
    }
    double MxCountFlt = (double)MxCount * 1.25;
    // save ploting program
    {
        TFOut PlotFOut(PlotFNm);
        PlotFOut.PutStrLn("# event plot");
        PlotFOut.PutStrLn(TStr::Fmt("set autoscale"));
        PlotFOut.PutStrLn(TStr::Fmt("set xrange [0:%d]", IpNumIdWgtV.Len() + 1));
        PlotFOut.PutStrLn(TStr::Fmt("set yrange [0:%.2f]", MxCountFlt));
        if (BigP) { PlotFOut.PutStrLn(TStr::Fmt("set terminal png large size 700,300")); }
        else { PlotFOut.PutStrLn(TStr::Fmt("set terminal png large size 300,100")); }
        PlotFOut.PutStrLn(TStr::Fmt("set output '%s.png'", PlotFNm.CStr()));
        PlotFOut.PutStrLn(TStr::Fmt("set boxwidth 0.4"));
        if (BigP) {
            // date marks
            PlotFOut.PutStr(TStr::Fmt("set xtics rotate  ( "));
            for (int IpNumN = 0; IpNumN < IpNumIdWgtV.Len(); IpNumN++) {
                const int IpNumId = IpNumIdWgtV[IpNumN].Key;
                TStr IpNumStr = BtaEventBs->GetDevDefBs()->GetIpNumNm(IpNumId);
                if (IpNumN != 0) { PlotFOut.PutStr(", "); }
                PlotFOut.PutStr(TStr::Fmt("\" %s\" %d", IpNumStr.CStr(), IpNumN+1));
            }
            PlotFOut.PutStrLn(")");
        }
        // finish 
        PlotFOut.PutStrLn("set xlabel \"Time\"");
        PlotFOut.PutStrLn("set ylabel \"Number of alarms\"");
        TStr TitleStr = "Average", LastTitleStr = "In last " + PeriodStr;
        PlotFOut.PutStr(TStr::Fmt("plot \"%s.tab\" using ($1-0.22):2 title \"%s\" with boxes, ", 
            PlotFNm.CStr(), TitleStr.CStr()));
        PlotFOut.PutStrLn(TStr::Fmt("\"%s.tab\" using ($1+0.22):3 title \"%s\" with boxes", 
            PlotFNm.CStr(), LastTitleStr.CStr()));
    }
    // get png
    RunGnuPlot(PlotFNm);    
}

int TBTAServer::ProcessNewBuffer() {
    // load events from the new buffer
    TIntV EventIdV; BtaEventBs->AddNewBuffer(EventIdV);
    // add them to the correlation engine
    for (int EventIdN = 0; EventIdN < EventIdV.Len(); EventIdN++) {
        const int EventId = EventIdV[EventIdN];
        // add the event to the correlations
        BtaCorrBs->AddEvent(BtaEventBs, EventId);
        // add it to the interface
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
        if (Event.GetIpNumId() == -1 || Event.GetProjId() == -1) { continue; }
        if (Event.GetSeverity() < MnSeverity) { continue; }
        AllEventIdV.Add(EventId);
        if (Event.GetCaseId() == -1) { continue; }
        CaseIdEventIdV.Add(EventId);
    }
    // log the additions
    TFOut LogFOut("btalarms.log", true);
    LogFOut.PutStrLn(TStr::Fmt("[%s] processed %d new alarms!", 
        TTm::GetCurLocTm().GetWebLogDateTimeStr().CStr(), EventIdV.Len()));
    // return the number of new events
    return EventIdV.Len();
}

void TBTAServer::ProcessDumpBuffer() {
    // first we process any potential new stuff
    ProcessNewBuffer();
    // first save all the current states to disk
    BtaEventBs->SaveBin(TFile::GetUniqueFNm(DumpFPath + "BTAlarms.bta"));
    BtaCorrBs->SaveBin(TFile::GetUniqueFNm(DumpFPath + "BTAlarms.btc"));
    BtaLongBs->SaveBin(TFile::GetUniqueFNm(DumpFPath + "BTAlarms.btl"));
    // move the alarms to the final resting place in the database
    BtaEventBs->SaveDumpBuffer();
    // confirm this dump
    TFOut LogFOut("btalarms.log", true);
    LogFOut.PutStrLn(TStr::Fmt("[%s] dump completed!", 
        TTm::GetCurLocTm().GetWebLogDateTimeStr().CStr()));
}

int TBTAServer::RescueFromDumpBuffer() {
    // load events from the new buffer
    TIntV EventIdV; BtaEventBs->RescueDumpBuffer(EventIdV);
    // add them to the correlation engine
    for (int EventIdN = 0; EventIdN < EventIdV.Len(); EventIdN++) {
        const int EventId = EventIdV[EventIdN];
        BtaCorrBs->AddEvent(BtaEventBs, EventId);
        // add it to the interface
        const TBtaEvent& Event = BtaEventBs->GetEvent(EventId);
        if (Event.GetIpNumId() == -1 || Event.GetProjId() == -1) { continue; }
        if (Event.GetSeverity() < MnSeverity) { continue; }
        AllEventIdV.Add(EventId);
        if (Event.GetCaseId() == -1) { continue; }
        CaseIdEventIdV.Add(EventId);
    }
    // log the additions
    TFOut LogFOut("btalarms.log", true);
    LogFOut.PutStrLn(TStr::Fmt("[%s] rescued %d alarms!", 
        TTm::GetCurLocTm().GetWebLogDateTimeStr().CStr(), EventIdV.Len()));
    // return the number of new events
    return EventIdV.Len();
}

void TBTAServer::SaveAndExit() {
    // first we process any potential new stuff
    ProcessNewBuffer();
    // then we save all the current states to disk
    BtaEventBs->SaveBin(BinFPath + "BTAlarms.bta");
    BtaCorrBs->SaveBin(BinFPath + "BTAlarms.btc");
    BtaLongBs->SaveBin(BinFPath + "BTAlarms.btl");
    // move the alarms to the final resting place in the database
    BtaEventBs->SaveDumpBuffer();
    // confirm this save
    TFOut LogFOut("btalarms.log", true);
    LogFOut.PutStrLn(TStr::Fmt("[%s] save completed!", 
        TTm::GetCurLocTm().GetWebLogDateTimeStr().CStr()));
    // quit
    ExitProcess(0);
}

TBTAServer::TBTAServer(const int& WebSrvPortN, const TStr& _BinFPath, 
        const TStr& _DumpFPath, const POdbcDb& OdbcDb, const TStr& _GnuPlotPath):   
            TWebSrv(WebSrvPortN, true, TNotify::StdNotify) {

    // load data from the disk
    printf("Starting server \n");
    BinFPath = _BinFPath; DumpFPath = _DumpFPath; GnuPlotPath = _GnuPlotPath;
    BtaEventBs = TBtaEventBs::LoadBin(BinFPath + "BTAlarms.bta", OdbcDb);
    BtaDevDefBs = BtaEventBs->GetDevDefBs();
    BtaCorrBs = TBtaCorrBs::LoadBin(BinFPath + "BTAlarms.btc");
    BtaLongBs = TBtaLongBs::LoadBin(BinFPath + "BTAlarms.btl");

    // rescue potential stuff from dump_buffer
    RescueFromDumpBuffer();

    // load ids of events into vectors
    const int Events = BtaEventBs->GetEvents();
    printf(" No. of events: %d\n", Events);
    {printf("Loading events ");   
    for (int EventN = 0; EventN < Events; EventN++) {
        TBtaEvent Event = BtaEventBs->GetEvent(EventN);
        if (Event.GetIpNumId() == -1 || Event.GetProjId() == -1) { continue; }
        if (Event.GetSeverity() < MnSeverity) { continue; }
        // we display it!
        AllEventIdV.Add(EventN);
        // has case id?
        if (Event.GetCaseId() == -1) { continue; }
        CaseIdEventIdV.Add(EventN);
    } printf(".");}

    // prepare stuff for long-term trends
    {printf(" LongProj ");
    TFltIntKdV WeekChiProjIdV, WeekKsProjIdV, MonthChiProjIdV, MonthKsProjIdV; printf(".");
    TTm StartDate = BtaEventBs->GetFirstTime(), EndDate = BtaEventBs->GetLastTime(); printf(".");
    TTm WeekBreakDate = GetBreakDate(7), MonthBreakDate = GetBreakDate(30); printf(".");
    int ProjKeyId = BtaLongBs->FFirstKeyId(); printf(".");
    int AllCount = 0, WeekSkipCount = 0, MonthSkipCount = 0;
    while (BtaLongBs->FNextKeyId(ProjKeyId)) {
        AllCount++;
        const int ProjId = BtaLongBs->GetProjId(ProjKeyId);
        PBtaLongProj LongProj = BtaLongBs->GetLongProj(ProjKeyId);
        int BeforeCount, AfterCount;
        // calcualte trends for last week vs. before
        LongProj->Count(StartDate, WeekBreakDate, EndDate, BeforeCount, AfterCount);
        const bool IsWeekTrendPosP = LongProj->IsTrendPos(StartDate, WeekBreakDate, EndDate);
        if (BeforeCount > MnBeforeCount /*&& IsWeekTrendPosP*/) {
            const double WeekChi = LongProj->ChiSquare(StartDate, WeekBreakDate, EndDate);
            WeekChiProjIdV.Add(TFltIntKd(WeekChi, ProjId));
        } else { WeekSkipCount++; }
        // calculate trends for last month vs. before
        LongProj->Count(StartDate, MonthBreakDate, EndDate, BeforeCount, AfterCount);
        const bool IsMonthTrendPosP = LongProj->IsTrendPos(StartDate, MonthBreakDate, EndDate);
        if (BeforeCount > MnBeforeCount /*&& IsMonthTrendPosP*/) {
            const double MonthChi = LongProj->ChiSquare(StartDate, MonthBreakDate, EndDate);
            MonthChiProjIdV.Add(TFltIntKd(MonthChi, ProjId));
        } else { MonthSkipCount++; }
    }
    printf("(%d/%d/%d)", AllCount, WeekSkipCount, MonthSkipCount);
    WeekChiProjIdV.Sort(true); WeekChiSortProjIdV.Clr();
    for (int EltN = 0; EltN < WeekChiProjIdV.Len(); EltN++) {
        if (WeekChiProjIdV[EltN].Key > MxChiProb) { break; }
        WeekChiSortProjIdV.Add(WeekChiProjIdV[EltN].Dat); }
    WeekChiSortProjIdV.Pack(); 
    MonthChiProjIdV.Sort(true); MonthChiSortProjIdV.Clr();
    for (int EltN = 0; EltN < MonthChiProjIdV.Len(); EltN++) {
        if (MonthChiProjIdV[EltN].Key > MxChiProb) { break; }
        MonthChiSortProjIdV.Add(MonthChiProjIdV[EltN].Dat); }
    MonthChiSortProjIdV.Pack();
    printf(".");}

    printf(" Done\n");
}

void TBTAServer::OnHttpRq(const int& SockId, const PHttpRq& HttpRq) {
    // check http-request correctness - return if error
    if (!HttpRq->IsOk()) { return; }
    // check url correctness - return if error
    PUrl RqUrl=HttpRq->GetUrl();
    if (!RqUrl->IsOk()) { return; }

    PUrl HttpRqUrl = HttpRq->GetUrl();
    TStr UrlStr = HttpRqUrl->GetUrlStr();
    TStr CmdNm = HttpRqUrl->GetPathSeg(0);
    TStr FullQueryStr = HttpRqUrl->GetSearchStr();
    PUrlEnv HttpRqUrlEnv=HttpRq->GetUrlEnv();

    TStr TimeNow = TTm::GetCurLocTm().GetWebLogDateTimeStr(true);
    printf("[%s] Request %s %s\n", TimeNow.CStr(), CmdNm.CStr(), FullQueryStr.CStr());

    PHttpResp HttpResp;
    if (CmdNm == "style.css") {
        PSIn BodySIn = TFIn::New("style.css");
        HttpResp = THttpResp::New(THttp::OkStatusCd, "text/css", false, BodySIn);
    } else if (CmdNm == "board.css") {
        PSIn BodySIn = TFIn::New("board.css");
        HttpResp = THttpResp::New(THttp::OkStatusCd, "text/css", false, BodySIn);
    } else if (CmdNm.Right(4) == ".png") {
        TStr ImgFNm = "./" + CmdNm; 
        ImgFNm.ChangeChAll('-', '/');
        PSIn BodySIn = TFIn::New(ImgFNm);
        HttpResp = THttpResp::New(THttp::OkStatusCd, "image/png", false, BodySIn);
    } else if (CmdNm == "process") { 
        // processing of alarm in the database
        TStr MsgStr; 
        try {
            TStr ProcessType = HttpRqUrlEnv->GetVal("type", 0, "new");
            if (ProcessType == "new") {
                // check for new alarms in the buffer
                const int NewEvents = ProcessNewBuffer();
                MsgStr = TStr::Fmt("Loaded %d new alarms!", NewEvents);
            } else if (ProcessType == "dump") {
                // dumps current data to disk and moves dump buffera alarms to final stage
                ProcessDumpBuffer();
                MsgStr = "Dump successful!";
            } else if (ProcessType == "exit") {
                // saves current state to the initial file used when starting the program
                SaveAndExit();
                MsgStr = "Exit successful!";
            }
        } catch (PExcept Except) {
            printf("Exception: %s", Except->GetMsgStr().CStr());
        }
        // finish the page
		PSIn BodySIn = TMIn::New(MsgStr);
		HttpResp = THttpResp::New(THttp::OkStatusCd, 
			THttp::TextHtmlFldVal, false, BodySIn);
    } else if (CmdNm == "board") { 
        // read parameters
        TStr ProjCountStr = HttpRqUrlEnv->GetVal("count", 0, "0");
        const int ProjCount = ProjCountStr.IsInt() ? ProjCountStr.GetInt() : 0;
        TStr RefreshTmSecStr = HttpRqUrlEnv->GetVal("refresh", 0, "10");
        const int RefreshTmSec = RefreshTmSecStr.IsInt() ? RefreshTmSecStr.GetInt() : 10;
        // prepare the board page
        TChA HtmlChA;
        MakeBoardPage(ProjCount, RefreshTmSec, HtmlChA);
        // make a response
		PSIn BodySIn = TMIn::New(HtmlChA);
		HttpResp = THttpResp::New(THttp::OkStatusCd, 
			THttp::TextHtmlFldVal, false, BodySIn);
    } else if ((CmdNm == "related_traps") || 
        (CmdNm == "predicted_traps") || (CmdNm == "trap_device")) {
		// special small version of the explorer showing just details of one event
		const bool RelatedTrapsP = (CmdNm == "related_traps");
        const bool PredictedTrapsP = (CmdNm == "predicted_traps");
		const bool TrapDeviceP = (CmdNm == "trap_device");
        TStr EventIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
        if (EventIdStr.IsInt()) {
			// read the parameters
            const int EventId = EventIdStr.GetInt();
			// output the page
			TChA HtmlChA;
			HtmlChA += "<html><head>\n";
			HtmlChA += "<title>BT Alarms Explorer</title>\n";
			HtmlChA += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
			HtmlChA += "<link href=\"style.css\" type=\"text/css\" rel=\"stylesheet\">\n";
			HtmlChA += "</head><body>\n";
			// tabs
			if (RelatedTrapsP) { 
				HtmlChA += "[ Related Traps | ";
				HtmlChA += "<a href=\"/predicted_traps?id=" + 
                    EventIdStr + "\">Predicted Traps</a> |";
				HtmlChA += "<a href=\"/trap_device?id=" + 
                    EventIdStr + "\">Device Histroy and Trends</a> ]";
            } else if (PredictedTrapsP) {
				HtmlChA += "[ <a href=\"/related_traps?id=" +
                    EventIdStr + "\">Related Traps</a> |";
				HtmlChA += "Predicted Traps |";
				HtmlChA += "<a href=\"/trap_device?id=" + 
                    EventIdStr + "\">Device Histroy and Trends</a> ]";
			} else {
				HtmlChA += "[ <a href=\"/related_traps?id=" +
                    EventIdStr + "\">Related Traps</a> |";
				HtmlChA += "<a href=\"/predicted_traps?id=" + 
                    EventIdStr + "\">Predicted Traps</a> |";
				HtmlChA += "Device Histroy and Trends ]";
			}
			HtmlChA += "<br><hr>";
			// content
			if (RelatedTrapsP) {
				MakeShortEventPage(EventId, HtmlChA);
            } else if (PredictedTrapsP) {
                MakeShortPredIpNumsPage(EventId, HtmlChA);
            } else if (TrapDeviceP) {
				MakeShortIpNumPage(EventId, HtmlChA);
			}
			// finish the page
			HtmlChA += "</body></html>";
			PSIn BodySIn = TMIn::New(HtmlChA);
			HttpResp = THttpResp::New(THttp::OkStatusCd, 
				THttp::TextHtmlFldVal, false, BodySIn);
        } else {
            printf("Wrong parameter id '%s' ..;\n", EventIdStr.CStr());
        }
    } else if (CmdNm == "report") { 
        TStr ReportType = HttpRqUrlEnv->GetVal("type", 0, "related");
        TChA HtmlChA;
        HtmlChA += "<html><head>\n";
        HtmlChA += "<title>BT Alarms Explorer</title>\n";
        HtmlChA += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
        HtmlChA += "<link href=\"style.css\" type=\"text/css\" rel=\"stylesheet\">\n";
        HtmlChA += "</head><body>\n";
        if (ReportType == "related") {
            // read parameters
            TStr ReportSubType = HttpRqUrlEnv->GetVal("subtype", 0, "all");
            const bool ProbP = ((ReportSubType == "all") || (ReportSubType == "prob"));
            const bool TextP = ((ReportSubType == "all") || (ReportSubType == "text"));
            TStr MxEventsStr = HttpRqUrlEnv->GetVal("events", 0, "");
            const int MxEvents = MxEventsStr.IsInt() ? MxEventsStr.GetInt() : 1000;
            TStr MnSeverityStr = HttpRqUrlEnv->GetVal("mnseverity", 0, "");
            const int MnSeverity = MnSeverityStr.IsInt() ? MnSeverityStr.GetInt() : 3;
            // get the histogram of relatednes
            int AllEvents = 0;
            TIntV AllScoresCountV, ProbScoresCountV, TextScoresCountV; 
            BtaCorrBs->GetRootCauseStat(BtaEventBs, MxEvents, MnSeverity,
                RootCouseEventWnd, RootCouseTimeWnd, ProbP, TextP, 
                AllScoresCountV, ProbScoresCountV, TextScoresCountV, 
                AllEvents);
            // prepare the final result
            HtmlChA += TStr::Fmt(
                "<b>Number of severe events (Severity > %d):</b> %d<br>\n", 
                MnSeverity, AllEvents);
            HtmlChA += "<hr>\n";
            HtmlChA += "<table><tr>";
            HtmlChA += "<td>#related</td>";
            HtmlChA += "<td>#all_alarms</td>";
            HtmlChA += "<td>#prob_alarms</td>";
            HtmlChA += "<td>#text_alarms</td>";
            HtmlChA += "</tr>\n";
            for (int ScoresCountN = 0; ScoresCountN < AllScoresCountV.Len(); ScoresCountN++) {
                HtmlChA += "<tr>\n";
                HtmlChA += TStr::Fmt("<td>%d</td>\n", ScoresCountN);
                HtmlChA += TStr::Fmt("<td>%d</td>\n", AllScoresCountV[ScoresCountN].Val);
                HtmlChA += TStr::Fmt("<td>%d</td>\n", ProbScoresCountV[ScoresCountN].Val);
                HtmlChA += TStr::Fmt("<td>%d</td>\n", TextScoresCountV[ScoresCountN].Val);
                HtmlChA += "</tr>\n";
            }
            HtmlChA += "</table>";
        }
        // finish the page
		HtmlChA += "</body></html>";
        printf("\n\n%s\n\n", HtmlChA.CStr());
		PSIn BodySIn = TMIn::New(HtmlChA);
		HttpResp = THttpResp::New(THttp::OkStatusCd, 
			THttp::TextHtmlFldVal, false, BodySIn);
	} else {
        // main web interface of the alarms explorer
        TChA HtmlChA;
        HtmlChA += "<html><head>\n";
        HtmlChA += "<title>BT Alarms Explorer</title>\n";
        HtmlChA += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
        HtmlChA += "<link href=\"style.css\" type=\"text/css\" rel=\"stylesheet\">\n";
        HtmlChA += "</head><body>\n";
        HtmlChA += "<form action=\"/search\"><b><a href=\"/events\">Alarms</a></b>, ";
        HtmlChA += "<b><a href=\"/long\">Long-Term Trends</a></b> | ";
        HtmlChA += "Search: <input maxLength=\"256\" size=\"23\" name=\"q\"";
        if (!HttpRqUrlEnv->GetVal("q", 0, "").Empty()) {
            HtmlChA += "value=\"" + HttpRqUrlEnv->GetVal("q", 0, "") + "\""; }
        HtmlChA += ">";
        HtmlChA += "<input type=submit name=\"type\" value=\"device\">\n";
        HtmlChA += "<input type=submit name=\"type\" value=\"project\">\n";
        HtmlChA += "</form>";
        HtmlChA += "<hr>\n";
        if (CmdNm.Empty() || CmdNm == "events") {
            // read parameters
            TStr SortTypeStr = HttpRqUrlEnv->GetVal("sort", 0, "all");
            TStr PageNumStr = HttpRqUrlEnv->GetVal("page", 0, "0");
            if (PageNumStr.IsInt()) {
                const int PageNum = PageNumStr.GetInt();
                if (SortTypeStr == "all") {           
                    MakeEventsPage(AllEventIdV, SortTypeStr, PageNum, HtmlChA);
                } else if (SortTypeStr == "case_id") {
                    MakeEventsPage(CaseIdEventIdV, SortTypeStr, PageNum, HtmlChA);
                } else {
                    printf("Wrong parameter sort '%s' ..;\n", SortTypeStr.CStr());
                }
            } else {
                printf("Wrong parameter page '%s' ..;\n", PageNumStr.CStr());
            }
        } else if (CmdNm == "search") {
            TStr QueryStr = HttpRqUrlEnv->GetVal("q", 0, "");
            TStr TypeStr = HttpRqUrlEnv->GetVal("type", 0, "");
            if (!QueryStr.Empty() && !TypeStr.Empty()) {
                bool SearchOkP = true;
                if (TypeStr == "device") {
                    PBtaDevDefBs BtaDevDefBs = BtaEventBs->GetDevDefBs();
                    if (BtaDevDefBs->IsIpNumId(QueryStr)) {
                        const int IpNumId = BtaDevDefBs->GetIpNumId(QueryStr);
                        MakeIpNumPage(IpNumId, HtmlChA);
                    } else { SearchOkP = false; }
                } else if (TypeStr == "project") {
                    PBtaDevDefBs BtaDevDefBs = BtaEventBs->GetDevDefBs();
                    if (BtaDevDefBs->IsProjId(QueryStr)) {
                        const int ProjId = BtaDevDefBs->GetProjId(QueryStr);
                        MakeProjectPage(ProjId, HtmlChA);
                    } else { SearchOkP = false; }
                } else { SearchOkP = false; }
                if (!SearchOkP) { HtmlChA += "<b>No results found, try again.</b>\n"; }
            } else { 
                printf("Wrong parameter page '%s' and '%s' ..;\n", 
                    QueryStr.CStr(), TypeStr.CStr());
            }            
        } else if (CmdNm == "event") {
            // header
            TStr EventIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            if (EventIdStr.IsInt()) {
                const int EventId = EventIdStr.GetInt();
                MakeEventPage(EventId, HtmlChA);
            } else {
                printf("Wrong parameter id '%s' ..;\n", EventIdStr.CStr());
            }
        } else if (CmdNm == "event_full") {
            // header
            TStr EventIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            if (EventIdStr.IsInt()) {
                const int EventId = EventIdStr.GetInt();
                MakeEventFullPage(EventId, HtmlChA);
            } else {
                printf("Wrong parameter id '%s' ..;\n", EventIdStr.CStr());
            }
        } else if (CmdNm == "device") {
            // header
            TStr IpNumIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            if (IpNumIdStr.IsInt()) {
                const int IpNumId = IpNumIdStr.GetInt();
                MakeIpNumPage(IpNumId, HtmlChA);                
            } else {
                printf("Wrong parameter id '%s' ..;\n", IpNumIdStr.CStr());
            }
        } else if (CmdNm == "project") {
            // header
            // correlation for a specific device
            TStr ProjIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            if (ProjIdStr.IsInt()) {
                const int ProjId = ProjIdStr.GetInt();
                MakeProjectPage(ProjId, HtmlChA);
            } else {
                printf("Wrong parameter id '%s' ..;\n", ProjIdStr.CStr());
            }
        } else if (CmdNm == "d_events") {
            TStr PageNumStr = HttpRqUrlEnv->GetVal("page", 0, "0");
            TStr IpNumIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            if (!PageNumStr.IsInt()) {
                printf("Wrong parameter page '%s' ..;\n", PageNumStr.CStr());
            } else if (!IpNumIdStr.IsInt()) {
                printf("Wrong parameter id '%s' ..;\n", IpNumIdStr.CStr());
            } else {              
                const int PageNum = PageNumStr.GetInt();
                const int IpNumId = IpNumIdStr.GetInt();
                if (BtaCorrBs->IsIpNumId(IpNumId)) {
                    const TIntV& EventV = BtaCorrBs->GetIpNumEventV(IpNumId); 
                    MakeIpNumEventsPage(IpNumId, EventV, PageNum, HtmlChA);
                }
            }
        } else if (CmdNm == "p_events") {
            TStr PageNumStr = HttpRqUrlEnv->GetVal("page", 0, "0");
            TStr ProjIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            if (!PageNumStr.IsInt()) {
                printf("Wrong parameter page '%s' ..;\n", PageNumStr.CStr());
            } else if (!ProjIdStr.IsInt()) {
                printf("Wrong parameter id '%s' ..;\n", ProjIdStr.CStr());
            } else {              
                const int PageNum = PageNumStr.GetInt();
                const int ProjId = ProjIdStr.GetInt();
                if (BtaCorrBs->IsProjId(ProjId)) {
                    const TIntV& EventV = BtaCorrBs->GetProjEventV(ProjId); 
                    MakeProjEventsPage(ProjId, EventV, PageNum, HtmlChA);
                }
            }
        } else if (CmdNm == "dd_events") {
            TStr PageNumStr = HttpRqUrlEnv->GetVal("page", 0, "0");
            TStr IpNumIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            TStr CorrIpNumIdStr = HttpRqUrlEnv->GetVal("corrid", 0, "0");
            if (!PageNumStr.IsInt()) {
                printf("Wrong parameter page '%s' ..;\n", PageNumStr.CStr());
            } else if (!IpNumIdStr.IsInt()) {
                printf("Wrong parameter id '%s' ..;\n", IpNumIdStr.CStr());
            } else if (!CorrIpNumIdStr.IsInt()) {
                printf("Wrong parameter corrid '%s' ..;\n", CorrIpNumIdStr.CStr());
            } else {              
                const int PageNum = PageNumStr.GetInt();
                const int IpNumId = IpNumIdStr.GetInt();
                const int CorrIpNumId = CorrIpNumIdStr.GetInt();
                MakeIpNumCorrEventPage(IpNumId, CorrIpNumId, PageNum, HtmlChA);
            }
        } else if (CmdNm == "long") {
            TStr PageNumStr = HttpRqUrlEnv->GetVal("page", 0, "0");
            if (PageNumStr.IsInt()) {
                TStr PeriodStr = HttpRqUrlEnv->GetVal("period", 0, "week");
                const int PageNum = PageNumStr.GetInt();
                // show list of projects with the biggest difference
                if (PeriodStr == "week") {
                    MakeLongPage(WeekChiSortProjIdV, PageNum, PeriodStr, HtmlChA);
                } else if (PeriodStr == "month") {
                    MakeLongPage(MonthChiSortProjIdV, PageNum, PeriodStr, HtmlChA);
                }
            } else {
                printf("Wrong parameter page '%s' ..;\n", PageNumStr.CStr());
            }
        } else if (CmdNm == "p_long") {
            TStr ProjIdStr = HttpRqUrlEnv->GetVal("id", 0, "0");
            TStr PageNumStr = HttpRqUrlEnv->GetVal("page", 0, "0");
            if (!PageNumStr.IsInt()) {
                printf("Wrong parameter page '%s' ..;\n", PageNumStr.CStr());
            } else if (!ProjIdStr.IsInt()) {
                printf("Wrong parameter id '%s' ..;\n", ProjIdStr.CStr());
            } else {              
                TStr PeriodStr = HttpRqUrlEnv->GetVal("period", 0, "week");
                const int PageNum = PageNumStr.GetInt();
                const int ProjId = ProjIdStr.GetInt();
                // show list of devices within the project with the biggest difference
                if (PeriodStr == "week") {
                    MakeProjLongPage(ProjId, PageNum, PeriodStr, HtmlChA);
                } else if (PeriodStr == "month") {
                    MakeProjLongPage(ProjId, PageNum, PeriodStr, HtmlChA);
                }
            }
        }
        HtmlChA += "</body></html>";

        PSIn BodySIn = TMIn::New(HtmlChA);
        HttpResp = THttpResp::New(THttp::OkStatusCd, 
            THttp::TextHtmlFldVal, false, BodySIn);
    }

    // construct & send response
    SendHttpResp(SockId, HttpResp);
}
