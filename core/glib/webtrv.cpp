/////////////////////////////////////////////////
// Includes
#include "webtrv.h"

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Uk
TWebTravelYahooUk::TWebTravelYahooUk(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("Yahoo")),
  StartUrlStr("http://www.yahoo.co.uk"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(100, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelYahooUk::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddSizeConstr(1000000);
  TStr HostNm="www.yahoo.co.uk";
//  AddHostPathConstr(HostNm, "/Arts/");
//  AddHostPathConstr(HostNm, "/Business_and_Economy");
//  AddHostPathConstr(HostNm, "/Computers_and_Internet");
//  AddHostPathConstr(HostNm, "/Education");
//  AddHostPathConstr(HostNm, "/Entertainment");
//  AddHostPathConstr(HostNm, "/Government");
//  AddHostPathConstr(HostNm, "/Health");
//  AddHostPathConstr(HostNm, "/News");
//  AddHostPathConstr(HostNm, "/Recreation");
//  AddHostPathConstr(HostNm, "/Reference");
//  AddHostPathConstr(HostNm, "/Regional");
//  AddHostPathConstr(HostNm, "/Science");
//  AddHostPathConstr(HostNm, "/Social_Science");
//  AddHostPathConstr(HostNm, "/Society_and_Culture");
}

/////////////////////////////////////////////////
// Web-Travel-Yahoo-De
TWebTravelYahooDe::TWebTravelYahooDe(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("Yahoo")),
  StartUrlStr("http://www.yahoo.de"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(100, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelYahooDe::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddSizeConstr(1000000);
  TStr HostNm="www.yahoo.de";
  AddHostPathConstr(HostNm, "/Bildung_und_Ausbildung/");
  AddHostPathConstr(HostNm, "/Computer_und_Internet/");
  AddHostPathConstr(HostNm, "/Geistes__und_Sozialwissenschaft/");
  AddHostPathConstr(HostNm, "/Gesellschaft_und_Soziales/");
  AddHostPathConstr(HostNm, "/Gesundheit/");
  AddHostPathConstr(HostNm, "/Handel_und_Wirtschaft/");
  AddHostPathConstr(HostNm, "/Kunst_und_Kultur/");
  AddHostPathConstr(HostNm, "/Nachrichten_und_Medien/");
  AddHostPathConstr(HostNm, "/Nachschlagewerke/");
  AddHostPathConstr(HostNm, "/Naturwissenschaft_und_Technik/");
  AddHostPathConstr(HostNm, "/Sport_und_Freizeit/");
  AddHostPathConstr(HostNm, "/Staat_und_Politik/");
  AddHostPathConstr(HostNm, "/Staedte_und_Laender/");
  AddHostPathConstr(HostNm, "/Unterhaltung/");
}

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Se
TWebTravelYahooSe::TWebTravelYahooSe(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("Yahoo")),
  StartUrlStr("http://www.yahoo.se"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(100, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelYahooSe::PutConstrs(){
  ClrConstr();
  AddSizeConstr(1000000);
  PutSingleFetchConstr(false);
  TStr HostNm="www.yahoo.se";

  AddHostPathConstr(HostNm, "/Datorer_och_Internet/");
  AddHostPathConstr(HostNm, "/Haelsa_och_sjukvaard/");
  AddHostPathConstr(HostNm, "/Konst_och_kultur/");
  AddHostPathConstr(HostNm, "/Naturvetenskap_och_teknik/");
  AddHostPathConstr(HostNm, "/Nyheter_och_media/");
  AddHostPathConstr(HostNm, "/Naeringsliv_och_ekonomi/");
  AddHostPathConstr(HostNm, "/Noeje/");
  AddHostPathConstr(HostNm, "/Politik_och_foervaltning/");
  AddHostPathConstr(HostNm, "/Referensverk/");
  AddHostPathConstr(HostNm, "/Regionalt/");
  AddHostPathConstr(HostNm, "/Samhaelle_och_kulturer/");
  AddHostPathConstr(HostNm, "/Samhaellsvetenskap_och_humaniora/");
  AddHostPathConstr(HostNm, "/Sport_och_fritid/");
  AddHostPathConstr(HostNm, "/Utbildning/");
}

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Fr
TWebTravelYahooFr::TWebTravelYahooFr(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("Yahoo")),
  StartUrlStr("http://www.yahoo.fr"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(100, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelYahooFr::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddSizeConstr(1000000);
  TStr HostNm="www.yahoo.fr";

  AddHostPathConstr(HostNm, "/Actualites_et_presse/");
  AddHostPathConstr(HostNm, "/Art_et_culture/");
  AddHostPathConstr(HostNm, "/Commerce_et_economie/");
  AddHostPathConstr(HostNm, "/Divertissement/");
  AddHostPathConstr(HostNm, "/Enseignement_et_formation/");
  AddHostPathConstr(HostNm, "/Exploration_geographique/");
  AddHostPathConstr(HostNm, "/Informatique_et_multimedia/");
  AddHostPathConstr(HostNm, "/Institutions_et_services_publics/");
  AddHostPathConstr(HostNm, "/References_et_annuaires/");
  AddHostPathConstr(HostNm, "/Sante/");
  AddHostPathConstr(HostNm, "/Sciences_et_technologies/");
  AddHostPathConstr(HostNm, "/Sciences_humaines_et_sociales/");
  AddHostPathConstr(HostNm, "/Sports_et_loisirs/");
  AddHostPathConstr(HostNm, "/Sujets_de_Societe/");
}

/////////////////////////////////////////////////
// Web-Travel-Yahoo-Dk
TWebTravelYahooDk::TWebTravelYahooDk(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("Yahoo")),
  StartUrlStr("http://www.yahoo.dk"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(100, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelYahooDk::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddSizeConstr(1000000);
  TStr HostNm="www.yahoo.dk";

  AddHostPathConstr(HostNm, "/EDB_og_internet/");
  AddHostPathConstr(HostNm, "/Erhvervsliv_og_oekonomi/");
  AddHostPathConstr(HostNm, "/Kunst_og_kultur/");
  AddHostPathConstr(HostNm, "/Naturvidenskab_og_teknologi/");
  AddHostPathConstr(HostNm, "/Nyheder_og_media/");
  AddHostPathConstr(HostNm, "/Politik_og_forvaltning/");
  AddHostPathConstr(HostNm, "/Reference/");
  AddHostPathConstr(HostNm, "/Regionalt/");
  AddHostPathConstr(HostNm, "/Samfund/");
  AddHostPathConstr(HostNm, "/Samfundsvidenskab_og_humaniora/");
  AddHostPathConstr(HostNm, "/Sport_og_fritid/");
  AddHostPathConstr(HostNm, "/Sundhed/");
  AddHostPathConstr(HostNm, "/Underholdning/");
  AddHostPathConstr(HostNm, "/Uddannelse/");
}

/////////////////////////////////////////////////
// Web-Travel-ARTFL-Roget
TWebTravelRogetARTFL::TWebTravelRogetARTFL(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("RogetARTFL")),
  StartUrlStr("http://humanities.uchicago.edu/biblio/roget_headwords.html"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(3, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelRogetARTFL::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddHostPathConstr("estragon.uchicago.edu", "/cgi-bin/RG.sh");
}

/////////////////////////////////////////////////
// Web-Travel-Ijs
TWebTravelIjs::TWebTravelIjs(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("IJS")),
  StartUrlStr("http://www-ai.ijs.si/"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(10, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelIjs::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddSizeConstr(500000);
  AddHostPathConstr("www-ai.ijs.si", "/");
}

/////////////////////////////////////////////////
// Web-Travel-Cia
//const TStr TWebTravelCia::DDfFPath=TStr::GetNrFPath("Cia"); // BCB3.0 Bug
//const TStr TWebTravelCia::SStartUrlStr=
// "http://www.odci.gov/cia/publications/nsolo/factbook/global.htm";

TWebTravelCia::TWebTravelCia(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("Cia")),
  StartUrlStr("http://www.odci.gov/cia/publications/nsolo/factbook/global.htm"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(10, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelCia::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddHostPathConstr("www.odci.gov", "/cia/publications/nsolo/factbook/");
}

/////////////////////////////////////////////////
// Web-Travel-HomeNet
TWebTravelHomeNet::TWebTravelHomeNet(
 const TXWebTravelMode& Mode, const PXWebTravelEvent& Event,
 const int& _TgUserId, const TStr& _InFNm, const TStr& _OutFPath):
  TXWebTravel(),
  TgUserId(_TgUserId),
  InFNm(_InFNm),
  OutFPath(TStr::GetNrFPath(_OutFPath)){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(100, OutFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(OutFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(OutFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelHomeNet::PutConstrs(){
  ClrConstr();
}

void TWebTravelHomeNet::StartTravel(){
  PutConstrs();
  TStrV UrlStrV(300000, 0);
  TIntIntH UserIdToDocsH(1000);
  PSIn SIn=PSIn(new TFIn(InFNm));
  TILx Lx(SIn, TFSet()|iloRetEoln);
  TChA UrlStr;
  Lx.GetSym(syInt, syEof);
  while ((Lx.Sym!=syEof)&&(Lx.SymLnN<200000)){
//  while (Lx.Sym!=syEof){
    int UserId=Lx.Int; Lx.GetSym(syComma);
    Lx.GetInt(); Lx.GetSym(syComma);
    Lx.GetInt(); Lx.GetSym(syComma);
    Lx.GetInt(); Lx.GetSym(syComma);
    Lx.GetInt(); Lx.GetSym(syComma);
    TStr Method=Lx.GetIdStr(); Lx.GetSym(syComma);  // GET, POST
    UrlStr.Clr(); UrlStr+=Lx.GetIdStr(); Lx.GetSym(syComma); // http, ftp
    UrlStr+="://";
    UrlStr+=Lx.GetStrToCh(','); Lx.GetSym(syComma); // domain name
    UrlStr+=Lx.GetStrToEoln(); Lx.GetEoln(); // path
    if ((UserId==TgUserId)&&IsUrlOk(UrlStr)&&(Method=="GET")){
      UserIdToDocsH.AddDat(UserId)++;
      UrlStrV.Add(UrlStr);
    }
    Lx.GetSym(syInt, syEof);
    if (Lx.SymLnN%100000==0){OnNotify(TInt::GetStr(Lx.SymLnN)+ " docs");}
  }
  int UserIdToDocsP=UserIdToDocsH.FFirstKeyId();
  while (UserIdToDocsH.FNextKeyId(UserIdToDocsP)){
    int UserId=UserIdToDocsH.GetKey(UserIdToDocsP);
    int Docs=UserIdToDocsH[UserIdToDocsP];
    TStr MsgStr=TStr("User ")+TInt::GetStr(UserId)+": "+
     TInt::GetStr(Docs)+" Docs.";
    OnNotify(MsgStr);
  }
  UrlStrV.Shuffle(TRnd());
  for (int UrlStrN=0; UrlStrN<UrlStrV.Len(); UrlStrN++){
    Go(UrlStrV[UrlStrN]);
  }
}

/////////////////////////////////////////////////
// Web-Travel-CmuPww
TWebTravelCmuPww::TWebTravelCmuPww(
 const TXWebTravelMode& Mode, const PXWebTravelEvent& Event,
 const TStr& _InFNm, const TStr& _OutFPath):
  TXWebTravel(),
  InFNm(_InFNm),
  OutFPath(TStr::GetNrFPath(_OutFPath)){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(10, OutFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(OutFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(OutFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelCmuPww::PutConstrs(){
  ClrConstr();
}

void TWebTravelCmuPww::StartTravel(){
  PutConstrs();
  TStrV UrlStrV(1000, 0);
  PSIn SIn=PSIn(new TFIn(InFNm));
  TILx Lx(SIn, TFSet()|iloRetEoln);
  Lx.GetSym(syQStr, syEof);
  while (Lx.Sym!=syEof){
    TStr UrlStr=Lx.Str;
    if (IsUrlOk(UrlStr)){
      UrlStrV.Add(UrlStr);}
    Lx.GetStrToEoln();
    Lx.GetEoln();
    Lx.GetSym(syQStr, syEof);
  }
  for (int UrlStrN=0; UrlStrN<UrlStrV.Len(); UrlStrN++){
    Go(UrlStrV[UrlStrN]);
  }
}

/////////////////////////////////////////////////
// Web-Travel-Test
TWebTravelTest::TWebTravelTest(const TXWebTravelMode& Mode,
 const PXWebTravelEvent& Event):
  TXWebTravel(),
  DfFPath(TStr::GetNrFPath("../test")),
  StartUrlStr("http://www.mf.uni-lj.si/"){
  PXWebBs WebBs;
  switch (Mode){
    case wtmStart: WebBs=PXWebBs(new TWebMemBs(10, DfFPath, false)); break;
    case wtmCont: WebBs=PXWebBs(new TWebMemBs(DfFPath, false)); break;
    case wtmCheck: WebBs=PXWebBs(new TWebMemBs(DfFPath, true)); break;
    default: Fail;
  }
  PutWebBs(WebBs);
  PutEvent(Event);
}

void TWebTravelTest::PutConstrs(){
  ClrConstr();
  PutSingleFetchConstr(false);
  AddSizeConstr(50000);
  AddHostPathConstr("mf.uni-lj.si", "/");
}

