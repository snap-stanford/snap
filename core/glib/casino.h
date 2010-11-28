#ifndef casino_h
#define casino_h

/////////////////////////////////////////////////
// Includes
#include "net.h"

/////////////////////////////////////////////////
// Forward
ClassHdTP(TRulette, PRulette);

//////////////////////////////////////////////////
// Rulette-Timer-Event
class TRuletteTimerEvent: public TTimerEvent{
private:
  TRulette* Rulette;
public:
  TRuletteTimerEvent(const int& TimeOut, TRulette* _Rulette):
    TTimerEvent(TimeOut), Rulette(_Rulette){}
  static PTimerEvent New(const int& TimeOut, TRulette* Rulette){
    return new TRuletteTimerEvent(TimeOut, Rulette);}

  void OnTimeOut();
};

//////////////////////////////////////////////////
// Rulette
typedef enum {rmUndef, rmOpen, rmClose, rmBall, rmPay} TRuletteMode;

ClassTPV(TRulette, PRulette, TRuletteV)//{
private:
  TStr TbId;
  TRuletteMode RuletteMode;
  int PrevChips, Chips;
  TIntV FldBidV;
  int BallFldN;
  TRnd Rnd;
  PAppWebClt AppWebClt;
  PTimerEvent TimerEvent;
  PNotify Notify;
  UndefDefaultCopyAssign(TRulette);
public:
  TRulette(
   const TStr& _TbId, const int& _Chips, const int& RndSeed,
   const TStr& SrvHostNm="localhost", const int& SrvPortN=TAppWebSrv::DfPortN,
   const PNotify& Notify=NULL);
  static PRulette New(
   const TStr& TbId, const int& Chips, const int& RndSeed,
   const TStr& SrvHostNm, const int& SrvPortN,
   const PNotify& Notify){
    return new TRulette(TbId, Chips, RndSeed, SrvHostNm, SrvPortN, Notify);}
  ~TRulette(){}
  TRulette(TSIn&){Fail;}
  static PRulette Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  void OnTimeOut();

  TRuletteMode GetRuletteMode() const {return RuletteMode;}
  int GetPrevChips() const {return PrevChips;}
  int GetChips() const {return Chips;}
  void GetBids(TIntV& _FldBidV) const {_FldBidV=FldBidV;}
  TStr GetBidStr() const;
  int GetBallFldN() const {return BallFldN;}
};

#endif
