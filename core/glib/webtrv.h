#ifndef webtrv_h
#define webtrv_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "webold.h"
#include "webmb.h"

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Uk
class TWebTravelYahooUk: public TXWebTravel{
public:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelYahooUk(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-Yahoo-De
class TWebTravelYahooDe: public TXWebTravel{
public:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelYahooDe(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Se
class TWebTravelYahooSe: public TXWebTravel{
public:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelYahooSe(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Fr
class TWebTravelYahooFr: public TXWebTravel{
public:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelYahooFr(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Dk
class TWebTravelYahooDk: public TXWebTravel{
public:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelYahooDk(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-ARTFL-Roget
class TWebTravelRogetARTFL: public TXWebTravel{
private:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelRogetARTFL(const TXWebTravelMode& Mode,
   const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-Ijs
class TWebTravelIjs: public TXWebTravel{
private:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelIjs(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-Cia
class TWebTravelCia: public TXWebTravel{
private:
//  static const TStr DDfFPath; // BCB3.0 Bug
//  static const TStr SStartUrlStr;
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelCia(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

/////////////////////////////////////////////////
// Web-Travel-HomeNet
class TWebTravelHomeNet: public TXWebTravel{
private:
  int TgUserId;
  TStr InFNm;
  TStr OutFPath;
public:
  TWebTravelHomeNet(
   const TXWebTravelMode& Mode, const PXWebTravelEvent& Event,
   const int& _TgUserId, const TStr& _InFNm, const TStr& _OutFPath);

  void PutConstrs();

  void StartTravel();
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){Fail;}
};

/////////////////////////////////////////////////
// Web-Travel-CmuPww
class TWebTravelCmuPww: public TXWebTravel{
private:
  TStr InFNm;
  TStr OutFPath;
public:
  TWebTravelCmuPww(
   const TXWebTravelMode& Mode, const PXWebTravelEvent& Event,
   const TStr& _InFNm, const TStr& _OutFPath);

  void PutConstrs();

  void StartTravel();
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){Fail;}
};

/////////////////////////////////////////////////
// Web-Travel-Test
class TWebTravelTest: public TXWebTravel{
private:
  TStr DfFPath;
  TStr StartUrlStr;
public:
  TWebTravelTest(const TXWebTravelMode& Mode, const PXWebTravelEvent& Event);

  void PutConstrs();

  void StartTravel(){PutConstrs(); Go(StartUrlStr);}
  void ContTravel(){PutConstrs(); Go();}
  void CheckTravel(){PutConstrs(); Go(StartUrlStr);}
};

#endif
