#ifndef btaserver_h
#define btaserver_h

#include "btalarms.h"
#include "mine.h"
#include "net.h"

/////////////////////////////////////////////////
// BTAlarms-Web-Server
class TBTAServer: public TWebSrv {
private:
    static int MnSeverity;
    static int ListItemsPerPage;
    static int DetailItemsPerPage;
    static int ListEventsPerPage;
    static int64 RootCouseTimeWnd;
    static int RootCouseEventWnd;
    static int64 BoardTimeWnd;
    static int BoardEventWnd;
    static int MnBeforeCount;
    static double MxChiProb;
    static double MnChiVal;
    static double MnCorrProb;
    static int MnIpNumsPerPlot;

    typedef TPair<TUInt64, TInt> TUInt64IntPr;
    typedef TVec<TUInt64IntPr> TUInt64IntPrV;

    typedef THash<TUInt64, TIntPr> TUInt64IntPrH;
    typedef TPair<TUInt64, TIntPr> TUInt64IntPrPr;
    typedef TVec<TUInt64IntPrPr> TUInt64IntPrPrV;

private:
    // given
    TStr BinFPath;
    TStr DumpFPath;
    TStr GnuPlotPath;
    // structures
    PBtaEventBs BtaEventBs;
    PBtaDevDefBs BtaDevDefBs;
    PBtaCorrBs BtaCorrBs;
    PBtaLongBs BtaLongBs;
    // event for display sortings
    TIntV AllEventIdV;
    TIntV CaseIdEventIdV;
    // project sorting
    TIntV WeekChiSortProjIdV;
    TIntV MonthChiSortProjIdV;

private:
  // functions
    int GetTotalFq(const TIntPrV& IdFqV);
    TStr TruncStr(const TStr& FullStr, const int& MxLen);
    TTm GetBreakDate(const int& Days) const;
    void GetIpNumEventCount(const int& IpNumId, const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, double& AvgCount, int& LastCount) const;

  // generation of main pages
    void MakeEventsPage(const TIntV& EventV, const TStr& SortTypeStr, 
        const int& PageNum, TChA& HtmlChA);
    void MakeEventPage(const int& EventId, TChA& HtmlChA);
    void MakeEventFullPage(const int& EventId, TChA& HtmlChA);
    void MakeIpNumPage(const int& IpNumId, TChA& HtmlChA);
    void MakeProjectPage(const int& ProjId, TChA& HtmlChA);
    void MakeIpNumEventsPage(const int& IpNumId, const TIntV& EventV,
        const int& PageNum, TChA& HtmlChA);
    void MakeProjEventsPage(const int& ProjId, const TIntV& EventV,
        const int& PageNum, TChA& HtmlChA);
    void MakeIpNumCorrEventPage(const int& IpNumId, 
        const int& CorrIpNumId, const int& PageNum, TChA& HtmlChA);
    void MakeLongPage(const TIntV& SortProjIdV,
        const int& PageNum, const TStr& PeriodStr, TChA& HtmlChA);
    void MakeProjLongPage(const int& ProjId,
        const int& PageNum, const TStr& PeriodStr, TChA& HtmlChA);
	void MakeShortEventPage(const int& EventId, TChA& HtmlChA);
	void MakeShortPredIpNumsPage(const int& EventId, TChA& HtmlChA);
	void MakeShortIpNumPage(const int& EventId, TChA& HtmlChA);
    void MakeBoardPage(const int& ProjCount, const int& RefreshTmSec, TChA& HtmlChA);

  // generation of main components
    // generation of links
    TStr GetIpNumLink(const int& IpNumId);
    TStr GetProjLink(const int& IpNumId);
    TStr GetProjLinkFromProj(const int& ProjId);
    // generation of design widgets
    TStr GetBar(const double& Val, const double& MxVal);
    // event
    void EventStat(const int& EventId, TChA& HtmlChA);
    void EventShortStat(const int& EventId, TChA& HtmlChA);
    void EventFullStat(const int& EventId, TChA& HtmlChA);
    void EventRootCause(const int& EventId, TChA& HtmlChA);
    void EventShortRootCause(const int& EventId, TChA& HtmlChA);
    void EventPredIpNums(const int& EventId, TChA& HtmlChA);
    void EventShortPredIpNums(const int& EventId, TChA& HtmlChA);
    // events
    void DisplayEventHeader(const bool& HdSortP, const bool& ScoreP, 
        const bool& LinkP, TChA& HtmlChA);
    void DisplayEventRow(const int& EventN, const TBtaEvent& Event, 
        const TBtaEventDat& EventDat, const bool& HdSortP, const bool& ScoreP, 
        const double& Score, const double& MxScore, const int& IpNumId, 
        const int& CorrIpNumId, TChA& HtmlChA);
    void DisplayShortEventHeader(const bool& ScoreP, TChA& HtmlChA);
    void DisplayShortEventRow(const int& EventN, const TBtaEvent& Event, 
        const TBtaEventDat& EventDat, const bool& ScoreP, const double& Score, 
        const double& MxScore, TChA& HtmlChA);
	void MakeEventList(const TIntV& EventV, const bool& HdSortP, 
        const int& DispPageN, TChA& HtmlChA);
    void MakeEventPrList(const TIntPrV& EventV, const int& DispPageN, TChA& HtmlChA);
	void MakeShortEventList(const TIntV& EventV, TChA& HtmlChA);

    // ip number
    void IpNumDevCorrStat(const int& IpNumId, TChA& HtmlChA);
    void ShortIpNumStat(const int& IpNumId, TChA& HtmlChA);
    void MakeIpNumPrList(const TIntV& DevKeyIdV, const int& DispPageN, TChA& HtmlChA);
    // project
    void ProjWithinCorrStat(const int& ProjId, TChA& HtmlChA);
    void ProjCorrStat(const int& ProjId, TChA& HtmlChA);
    void ShortProjStat(const int& ProjId, TChA& HtmlChA, const bool& ProjNmLinkP = false);
    // long-term
    void MakeLongProjList(const TIntV& SortProjIdV, 
        const int& DispPageN, const TStr& PeriodStr, TChA& HtmlChA);
    void MakeLongIpNumList(const int& ProjId, 
        const int& DispPageN, const TStr& PeriodStr, TChA& HtmlChA);

    // ploting
    void RunGnuPlot(const TStr& PlotFNm) const;
    void PlotEventV(const TStr& TitleStr, const TIntV& EventV, 
        const TStr& PlotFNm, const int& Width = 700, const int& Height = 300);
    void PlotEventV(const TStr& TitleStr, const TIntV& EventV, 
        const TStr& CorrTitleStr, const TIntV& CorrEventV, 
        const TStr& PlotFNm);
    void PlotLongIpNum(const TStr& TitleStr, const TStr& PlotFNm,
        const int& ProjId, const TTm& StartDate, const TTm& BreakDate, 
        const TTm& EndDate, const TStr& PeriodStr, const int& MxIpNums, 
        const bool& BigP);

    // loads all the alarms from the new-buffer table, adds them to the 
    // correlations and moves them to the dump-buffer
    int ProcessNewBuffer();
    // dumps the current state of the event base and correlation base
    // and moves the alarms from dump-buffer to alarms table
    void ProcessDumpBuffer();
    // this is ran at startup, checks if there is anything to be rescued 
    // from the dump buffer which is not yet in the correlation tables
    int RescueFromDumpBuffer();
    // clean exit
    void SaveAndExit();

public:
    TBTAServer(const int& WebSrvPortN, const TStr& _BinFPath, 
        const TStr& _DumpFPath, const POdbcDb& OdbcDb, const TStr& _GnuPlotPath);
    static PWebSrv New(const int& WebSrvPortN, const TStr& BinFPath, 
        const TStr& DumpFPath, const POdbcDb& OdbcDb, const TStr& GnuPlotPath) { 
            return new TBTAServer(WebSrvPortN, BinFPath, 
                DumpFPath, OdbcDb, GnuPlotPath); }

    void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
};

#endif
