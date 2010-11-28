/////////////////////////////////////////////////
// Includes
#include "casino.h"

//////////////////////////////////////////////////
// Rulette-Timer-Event
void TRuletteTimerEvent::OnTimeOut(){
  Rulette->OnTimeOut();
}

//////////////////////////////////////////////////
// Rulette
TRulette::TRulette(
 const TStr& _TbId,
 const int& _Chips,
 const int& RndSeed,
 const TStr& SrvHostNm, const int& SrvPortN, const PNotify& _Notify):
  TbId(_TbId), RuletteMode(rmUndef), PrevChips(_Chips), Chips(_Chips),
  Rnd(RndSeed), AppWebClt(), TimerEvent(), Notify(_Notify){
  // create application client
  AppWebClt=TAppWebClt::New(
   SrvHostNm, SrvPortN,
   TbId, TAppWebClt::DfPortN,
   true,
   TbId+".Log", TNotify::StdNotify);
  // create timer event
  TimerEvent=TRuletteTimerEvent::New(1, this);
}

void TRulette::OnTimeOut(){
  switch (RuletteMode){
    case rmUndef:
      // next state
      TimerEvent->StartTimer(1); RuletteMode=rmOpen;
      break;
    case rmOpen:{
      // generate bids
      FldBidV.Gen(37);
      for (int BidN=0; BidN<37; BidN++){
        FldBidV[Rnd.GetUniDevInt(37)]++;}
      // log
      AppWebClt->FetchSoapRq(TSoapRq::New("Log", "TbId", TbId,
       "Chips", TInt::GetStr(Chips), "Bid", GetBidStr()));
      TNotify::OnStatus(Notify, "Open");
      // next state
      TimerEvent->StartTimer(1000); RuletteMode=rmClose;
      break;}
    case rmClose:
      // log
      TNotify::OnStatus(Notify, "Close");
      // next state
      TimerEvent->StartTimer(1000); RuletteMode=rmBall;
    case rmBall:{
      // get ball-fall-field
      BallFldN=Rnd.GetUniDevInt(37);
      // log
      AppWebClt->FetchSoapRq(TSoapRq::New("Log", "TbId", TbId,
       "Ball", TInt::GetStr(BallFldN)));
      TNotify::OnStatus(Notify, "Ball");
      // next state
      TimerEvent->StartTimer(3000); RuletteMode=rmPay;
      break;}
    case rmPay:{
      // save previous number of chips
      PrevChips=Chips;
      // get bid at ball-fall-field
      int BidAtBallFldN=FldBidV[BallFldN];
      // collect bids from the table
      for (int BidFldN=0; BidFldN<37; BidFldN++){
        Chips+=FldBidV[BidFldN];}
      // payoff
      Chips-=36*BidAtBallFldN;
      // log
      AppWebClt->FetchSoapRq(TSoapRq::New("Log", "TbId", TbId,
       "Chips", TInt::GetStr(Chips), "Profit", TInt::GetStr(Chips-PrevChips)));
      TNotify::OnStatus(Notify, "Pay");
      // next state
      TimerEvent->StartTimer(5000); RuletteMode=rmOpen;
      break;}
    default: Fail;
  }
}

TStr TRulette::GetBidStr() const {
  TChA BidChA;
  for (int BidFldN=0; BidFldN<37; BidFldN++){
    if (FldBidV[BidFldN]!=0){
      if (!BidChA.Empty()){BidChA+=';';}
      BidChA+=TInt::GetStr(BidFldN); BidChA+=':';
      BidChA+=TInt::GetStr(FldBidV[BidFldN]);
    }
  }
  return BidChA;
}

