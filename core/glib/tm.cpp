/////////////////////////////////////////////////
// Time-Names
bool TTmInfo::InitP=false;
TStrV TTmInfo::UsMonthNmV;
TStrV TTmInfo::SiMonthNmV;
TStrV TTmInfo::UsDayOfWeekNmV;
TStrV TTmInfo::SiDayOfWeekNmV;

void TTmInfo::InitMonthNmV(){
  // english
  UsMonthNmV.Add("jan"); UsMonthNmV.Add("feb"); UsMonthNmV.Add("mar");
  UsMonthNmV.Add("apr"); UsMonthNmV.Add("may"); UsMonthNmV.Add("jun");
  UsMonthNmV.Add("jul"); UsMonthNmV.Add("aug"); UsMonthNmV.Add("sep");
  UsMonthNmV.Add("oct"); UsMonthNmV.Add("nov"); UsMonthNmV.Add("dec");
  IAssert(UsMonthNmV.Len()==12);
  // slovene
  SiMonthNmV.Add("jan"); SiMonthNmV.Add("feb"); SiMonthNmV.Add("mar");
  SiMonthNmV.Add("apr"); SiMonthNmV.Add("maj"); SiMonthNmV.Add("jun");
  SiMonthNmV.Add("jul"); SiMonthNmV.Add("aug"); SiMonthNmV.Add("sep");
  SiMonthNmV.Add("okt"); SiMonthNmV.Add("nov"); SiMonthNmV.Add("dec");
  IAssert(SiMonthNmV.Len()==12);
}

void TTmInfo::InitDayOfWeekNmV(){
  // english
  UsDayOfWeekNmV.Add("sun"); UsDayOfWeekNmV.Add("mon");
  UsDayOfWeekNmV.Add("tue"); UsDayOfWeekNmV.Add("wed");
  UsDayOfWeekNmV.Add("thu"); UsDayOfWeekNmV.Add("fri");
  UsDayOfWeekNmV.Add("sat");
  IAssert(UsDayOfWeekNmV.Len()==7);
  // slovene
  SiDayOfWeekNmV.Add("ned"); SiDayOfWeekNmV.Add("pon");
  SiDayOfWeekNmV.Add("tor"); SiDayOfWeekNmV.Add("sre");
  SiDayOfWeekNmV.Add("cet"); SiDayOfWeekNmV.Add("pet");
  SiDayOfWeekNmV.Add("sob");
  IAssert(SiDayOfWeekNmV.Len()==7);
}

int TTmInfo::GetMonthN(const TStr& MonthNm, const TLoc& Loc){
  EnsureInit();
  int MonthN=-1;
  switch (Loc){
    case lUs: MonthN=UsMonthNmV.SearchForw(MonthNm.GetLc()); break;
    case lSi: MonthN=SiMonthNmV.SearchForw(MonthNm.GetLc()); break;
    default: Fail;
  }
  if (MonthN==-1){return -1;} else {return MonthN+1;}
}

TStr TTmInfo::GetMonthNm(const int& MonthN, const TLoc& Loc){
  EnsureInit();
  IAssert((1<=MonthN)&&(MonthN<=12));
  switch (Loc){
    case lUs: return UsMonthNmV[MonthN-1];
    case lSi: return SiMonthNmV[MonthN-1];
    default: Fail; return TStr();
  }
}

int TTmInfo::GetDayOfWeekN(const TStr& DayOfWeekNm, const TLoc& Loc){
  EnsureInit();
  int DayOfWeekN=-1;
  switch (Loc){
    case lUs: DayOfWeekN=UsDayOfWeekNmV.SearchForw(DayOfWeekNm.GetLc()); break;
    case lSi: DayOfWeekN=SiDayOfWeekNmV.SearchForw(DayOfWeekNm.GetLc()); break;
    default: Fail;
  }
  if (DayOfWeekN==-1){return -1;} else {return DayOfWeekN+1;}
}

TStr TTmInfo::GetDayOfWeekNm(const int& DayOfWeekN, const TLoc& Loc){
  EnsureInit();
  IAssert((1<=DayOfWeekN)&&(DayOfWeekN<=7));
  switch (Loc){
    case lUs: return UsDayOfWeekNmV[DayOfWeekN-1];
    case lSi: return SiDayOfWeekNmV[DayOfWeekN-1];
    default: Fail; return TStr();
  }
}

TStr TTmInfo::GetHmFromMins(const int& Mins){
  return TInt::GetStr(Mins/60, "%02d")+":"+TInt::GetStr(Mins%60, "%02d");
}

int TTmInfo::GetTmUnitSecs(const TTmUnit& TmUnit) {
  switch(TmUnit) {
    case tmuYear : return 365*24*3600;
    case tmuMonth : return 31*24*3600;
    case tmuWeek : return 7*24*3600;
    case tmuDay : return 24*3600;
    case tmu12Hour : return 12*3600;
    case tmu6Hour : return 6*3600;
    case tmu4Hour : return 4*3600;
    case tmu2Hour : return 2*3600;
    case tmu1Hour : return 1*3600;
    case tmu30Min : return 30*60;
    case tmu15Min : return 15*60;
    case tmu10Min : return 10*60;
    case tmu1Min : return 60;
    case tmu1Sec : return 1;
    case tmuNodes : Fail;
    case tmuEdges : Fail;
    default: Fail;
  }
  return -1;
}

TStr TTmInfo::GetTmUnitStr(const TTmUnit& TmUnit) {
  switch(TmUnit) {
    case tmuYear : return "Year";
    case tmuMonth : return "Month";
    case tmuWeek : return "Week";
    case tmuDay : return "Day";
    case tmu12Hour : return "12 Hours";
    case tmu6Hour : return "6 Hours";
    case tmu4Hour : return "4 Hours";
    case tmu2Hour : return "2 Hours";
    case tmu1Hour : return "1 Hour";
    case tmu30Min : return "30 Minutes";
    case tmu15Min : return "15 Minutes";
    case tmu10Min : return "10 Minutes";
    case tmu1Min : return "Minute";
    case tmu1Sec : return "Second";
    case tmuNodes : return "Nodes";
    case tmuEdges : return "Edges";
    default: Fail;
  }
  return TStr::GetNullStr();
}

TStr TTmInfo::GetTmZoneDiffStr(const TStr& TmZoneStr){
  if (TmZoneStr=="A"){/* Alpha Time Zone Military*/ return "+1000";}
  if (TmZoneStr=="ACDT"){/* Australian Central Daylight Time	Australia */ return "+1030";}
  if (TmZoneStr=="ACST"){/* Australian Central Standard Time	Australia */ return "+0930";}
  if (TmZoneStr=="ADT"){/* Atlantic Daylight Time	North America */ return "-0300";}
  if (TmZoneStr=="AEDT"){/* Australian Eastern Daylight Time or Australian Eastern Summer Time	Australia */ return "+1100";}
  if (TmZoneStr=="AEST"){/* Australian Eastern Standard Time	Australia */ return "+1000";}
  if (TmZoneStr=="AKDT"){/* Alaska Daylight Time	North America */ return "-0800";}
  if (TmZoneStr=="AKST"){/* Alaska Standard Time	North America */ return "-0900";}
  if (TmZoneStr=="AST"){/* Atlantic Standard Time	North America */ return "-0400";}
  if (TmZoneStr=="AWDT"){/* Australian Western Daylight Time	Australia */ return "+0900";}
  if (TmZoneStr=="AWST"){/* Australian Western Standard Time	Australia */ return "+0800";}
  if (TmZoneStr=="B"){/* Bravo Time Zone	Military */ return "+0200";}
  if (TmZoneStr=="BST"){/* British Summer Time	Europe */ return "+0100";}
  if (TmZoneStr=="C"){/* Charlie Time Zone	Military */ return "+0300";}
  if (TmZoneStr=="CDT"){/* Central Daylight Time	North America */ return "-0500";}
  if (TmZoneStr=="CDT"){/* Central Daylight Time	Australia */ return "+1030";}
  if (TmZoneStr=="CEDT"){/* Central European Daylight Time	Europe */ return "+0200";}
  if (TmZoneStr=="CEST"){/* Central European Summer Time	Europe */ return "+0200";}
  if (TmZoneStr=="CET"){/* Central European Time	Europe */ return "+0100";}
  if (TmZoneStr=="CST"){/* Central Standard Time	North America */ return "-0600";}
  if (TmZoneStr=="CST"){/* Central Summer Time	Australia */ return "+1030";}
  if (TmZoneStr=="CST"){/* Central Standard Time	Australia */ return "+0930";}
  if (TmZoneStr=="CXT"){/* Christmas Island Time	Australia */ return "+0700";}
  if (TmZoneStr=="D"){/* Delta Time Zone	Military */ return "+0400";}
  if (TmZoneStr=="E"){/* Echo Time Zone	Military */ return "+0500";}
  if (TmZoneStr=="EDT"){/* Eastern Daylight Time	North America */ return "-0400";}
  if (TmZoneStr=="EDT"){/* Eastern Daylight Time	Australia */ return "+1100";}
  if (TmZoneStr=="EEDT"){/* Eastern European Daylight Time	Europe */ return "+0300";}
  if (TmZoneStr=="EEST"){/* Eastern European Summer Time	Europe */ return "+0300";}
  if (TmZoneStr=="EET"){/* Eastern European Time	Europe */ return "+0200";}
  if (TmZoneStr=="EST"){/* Eastern Standard Time	North America */ return "-0500";}
  if (TmZoneStr=="EST"){/* Eastern Summer Time	Australia */ return "+1100";}
  if (TmZoneStr=="EST"){/* Eastern Standard Time	Australia */ return "+1000";}
  if (TmZoneStr=="F"){/* Foxtrot Time Zone	Military */ return "+0600";}
  if (TmZoneStr=="G"){/* Golf Time Zone	Military */ return "+0700";}
  if (TmZoneStr=="GMT"){/* Greenwich Mean Time	Europe */ return "+0000";}
  if (TmZoneStr=="H"){/* Hotel Time Zone	Military */ return "+0800";}
  if (TmZoneStr=="HAA"){/* Heure Avancée de l'Atlantique	North America */ return "-0300";}
  if (TmZoneStr=="HAC"){/* Heure Avancée du Centre	North America */ return "-0500";}
  if (TmZoneStr=="HADT"){/* Hawaii-Aleutian Daylight Time	North America */ return "-0900";}
  if (TmZoneStr=="HAE"){/* Heure Avancée de l'Est	North America */ return "-0400";}
  if (TmZoneStr=="HAP"){/* Heure Avancée du Pacifique	North America */ return "-0700";}
  if (TmZoneStr=="HAR"){/* Heure Avancée des Rocheuses	North America */ return "-0600";}
  if (TmZoneStr=="HAST"){/* Hawaii-Aleutian Standard Time	North America */ return "-1000";}
  if (TmZoneStr=="HAT"){/* Heure Avancée de Terre-Neuve	North America */ return "-0230";}
  if (TmZoneStr=="HAY"){/* Heure Avancée du Yukon	North America */ return "-0800";}
  if (TmZoneStr=="HNA"){/* Heure Normale de l'Atlantique	North America */ return "-0400";}
  if (TmZoneStr=="HNC"){/* Heure Normale du Centre	North America */ return "-0600";}
  if (TmZoneStr=="HNE"){/* Heure Normale de l'Est	North America */ return "-0500";}
  if (TmZoneStr=="HNP"){/* Heure Normale du Pacifique	North America */ return "-0800";}
  if (TmZoneStr=="HNR"){/* Heure Normale des Rocheuses	North America */ return "-0700";}
  if (TmZoneStr=="HNT"){/* Heure Normale de Terre-Neuve	North America */ return "-0330";}
  if (TmZoneStr=="HNY"){/* Heure Normale du Yukon	North America */ return "-0900";}
  if (TmZoneStr=="I"){/* India Time Zone	Military */ return "+0900";}
  if (TmZoneStr=="IST"){/* Irish Summer Time	Europe */ return "+0100";}
  if (TmZoneStr=="K"){/* Kilo Time Zone	Military */ return "+1000";}
  if (TmZoneStr=="L"){/* Lima Time Zone	Military */ return "+1100";}
  if (TmZoneStr=="M"){/* Mike Time Zone	Military */ return "+1200";}
  if (TmZoneStr=="MDT"){/* Mountain Daylight Time	North America */ return "-0600";}
  if (TmZoneStr=="MESZ"){/* Mitteleuroäische Sommerzeit	Europe */ return "+0200";}
  if (TmZoneStr=="MEZ"){/* Mitteleuropäische Zeit	Europe */ return "+0100";}
  if (TmZoneStr=="MSD"){/* Moscow Daylight Time	Europe */ return "+0400";}
  if (TmZoneStr=="MSK"){/* Moscow Standard Time	Europe */ return "+0300";}
  if (TmZoneStr=="MST"){/* Mountain Standard Time	North America */ return "-0700";}
  if (TmZoneStr=="N"){/* November Time Zone	Military */ return "-0100";}
  if (TmZoneStr=="NDT"){/* Newfoundland Daylight Time	North America */ return "-0230";}
  if (TmZoneStr=="NFT"){/* Norfolk (Island) Time	Australia */ return "+ 11:30";}
  if (TmZoneStr=="NST"){/* Newfoundland Standard Time	North America */ return "-0330";}
  if (TmZoneStr=="O"){/* Oscar Time Zone	Military */ return "-0200";}
  if (TmZoneStr=="P"){/* Papa Time Zone	Military */ return "-0300";}
  if (TmZoneStr=="PDT"){/* Pacific Daylight Time	North America */ return "-0700";}
  if (TmZoneStr=="PST"){/* Pacific Standard Time	North America */ return "-0800";}
  if (TmZoneStr=="Q"){/* Quebec Time Zone	Military */ return "-0400";}
  if (TmZoneStr=="R"){/* Romeo Time Zone	Military */ return "-0500";}
  if (TmZoneStr=="S"){/* Sierra Time Zone	Military */ return "-0600";}
  if (TmZoneStr=="T"){/* Tango Time Zone	Military */ return "-0700";}
  if (TmZoneStr=="U"){/* Uniform Time Zone	Military */ return "-0800";}
  if (TmZoneStr=="UTC"){/* Coordinated Universal Time Europe */ return "+0000";}
  if (TmZoneStr=="V"){/* Victor Time Zone	Military */ return "-0900";}
  if (TmZoneStr=="W"){/* Whiskey Time Zone	Military */ return "-1000";}
  if (TmZoneStr=="WDT"){/* Western Daylight Time	Australia */ return "+0900";}
  if (TmZoneStr=="WEDT"){/* Western European Daylight Time	Europe */ return "+0100";}
  if (TmZoneStr=="WEST"){/* Western European Summer Time	Europe */ return "+0100";}
  if (TmZoneStr=="WET"){/* Western European Time	Europe */ return "+0000";}
  if (TmZoneStr=="WST"){/* Western Summer Time	Australia */ return "+0900";}
  if (TmZoneStr=="WST"){/* Western Standard Time	Australia */ return "+0800";}
  if (TmZoneStr=="X"){/* X-ray Time Zone	Military */ return "-1100";}
  if (TmZoneStr=="Y"){/* Yankee Time Zone	Military */ return "-1200";}
  if (TmZoneStr=="Z"){/* Zulu Time Zone	Military */ return "+0000";}
  return "-0000";
}

// day-of-week numbers
const int TTmInfo::SunN=1; const int TTmInfo::MonN=2;
const int TTmInfo::TueN=3; const int TTmInfo::WedN=4;
const int TTmInfo::ThuN=5; const int TTmInfo::FriN=6;
const int TTmInfo::SatN=7;

// month numbers
const int TTmInfo::JanN=1; const int TTmInfo::FebN=2;
const int TTmInfo::MarN=3; const int TTmInfo::AprN=4;
const int TTmInfo::MayN=5; const int TTmInfo::JunN=6;
const int TTmInfo::JulN=7; const int TTmInfo::AugN=8;
const int TTmInfo::SepN=9; const int TTmInfo::OctN=10;
const int TTmInfo::NovN=11; const int TTmInfo::DecN=12;

/////////////////////////////////////////////////
// Julian-Dates

/* public domain Julian Day Number functions
**
** Based on formulae originally posted by
**    Tom Van Flandern / Washington, DC / metares@well.sf.ca.us
**       in the UseNet newsgroup sci.astro.
**    Reposted 14 May 1991 in FidoNet C Echo conference by
**       Paul Schlyter (Stockholm)
** Minor corrections, added JDN to julian, and recast into C by
**    Raymond Gardner  Englewood, Colorado
**
** Synopsis:
**      long ymd_to_jdn(int year, int month, int day, int julian_flag)
**      void jdn_to_ymd(long jdn, int *year, int *month, int *day,
**                                                      int julian_flag)
**      year is negative if BC
**      if julian_flag is >  0, use Julian calendar
**      if julian_flag is == 0, use Gregorian calendar
**      if julian_flag is <  0, routines decide based on date
**
** These routines convert Gregorian and Julian calendar dates to and
** from Julian Day Numbers.  Julian Day Numbers (JDN) are used by
** astronomers as a date/time measure independent of calendars and
** convenient for computing the elapsed time between dates.  The JDN
** for any date/time is the number of days (including fractional
** days) elapsed since noon, 1 Jan 4713 BC.  Julian Day Numbers were
** originated by Joseph Scaliger in 1582 and named after his father
** Julius, not after Julius Caesar.  They are not related to the
** Julian calendar.
**
** For dates from 1 Jan 4713 BC thru 12 Dec Feb 32766 AD, ymd_to_jdn()
** will give the JDN for noon on that date.  jdn_to_ymd() will compute
** the year, month, and day from the JDN.  Years BC are given (and
** returned) as negative numbers.  Note that there is no year 0 BC;
** the day before 1 Jan 1 AD is 31 Dec 1 BC.  Note also that 1 BC,
** 5 BC, etc. are leap years.
**
** Pope Gregory XIII decreed that the Julian calendar would end on
** 4 Oct 1582 AD and that the next day would be 15 Oct 1582 in the
** Gregorian Calendar.  The only other change is that centesimal
** years (years ending in 00) would no longer be leap years
** unless divisible by 400.  Britain and its possessions and
** colonies continued to use the Julian calendar up until 2 Sep
** 1752, when the next day became 14 Sep 1752 in the Gregorian
** Calendar.  These routines can be compiled to use either
** convention.  By default, the British convention will be used.
** Simply #define PAPAL to use Pope Gregory's convention.
**
** Each routine takes, as its last argument, a flag to indicate
** whether to use the Julian or Gregorian calendar convention.  If
** this flag is negative, the routines decide based on the date
** itself, using the changeover date described in the preceding
** paragraph.  If the flag is zero, Gregorian conventions will be used,
** and if the flag is positive, Julian conventions will be used.
*/

// Pope Gregory XIII's decree
int TJulianDate::LastJulianDate=15821004; /* last day to use Julian calendar */
int TJulianDate::LastJulianDateN=2299160; /* jdn of same */
// British-American usage
//int TJulianDate::LastJulianDate=17520902; /* last day to use Julian calendar */
//int TJulianDate::LastJulianDateN=2361221; /* jdn of same */

int TJulianDate::GetJulianDateN(int d, int m, int y){
  IAssert(y != 0);
  int julian = -1;
  long jdn;

  if (julian < 0){ /* set Julian flag if auto set */
    julian = (((y * 100L) + m) * 100 + d  <=  LastJulianDate);}

  if (y < 0){ /* adjust BC year */
    y++;}

  if (julian){
    jdn = 367L * y - 7 * (y + 5001L + (m - 9) / 7) / 4
     + 275 * m / 9 + d + 1729777L;
  } else {
    jdn = (long)(d - 32076)
     + 1461L * (y + 4800L + (m - 14) / 12) / 4
     + 367 * (m - 2 - (m - 14) / 12 * 12) / 12
     - 3 * ((y + 4900L + (m - 14) / 12) / 100) / 4
     + 1;            /* correction by rdg */
  }
  return jdn;
}

void TJulianDate::GetCalendarDate(int jdn, int& dd, int& mm, int& yy){
  int julian = -1;

  long x, z, m, d, y;
  long daysPer400Years = 146097L;
  long fudgedDaysPer4000Years = 1460970L + 31;

  if (julian < 0){ /* set Julian flag if auto set */
    julian = (jdn <= LastJulianDateN);}

  x = jdn + 68569L;
  if (julian){
    x+=38;
    daysPer400Years = 146100L;
    fudgedDaysPer4000Years = 1461000L + 1;
  }
  z = 4 * x / daysPer400Years;
  x = x - (daysPer400Years * z + 3) / 4;
  y = 4000 * (x + 1) / fudgedDaysPer4000Years;
  x = x - 1461 * y / 4 + 31;
  m = 80 * x / 2447;
  d = x - 2447 * m / 80;
  x = m / 11;
  m = m + 2 - 12 * x;
  y = 100 * (z - 49) + y + x;

  yy = (int)y;
  mm = (int)m;
  dd = (int)d;

  if (yy <= 0){ /* adjust BC years */
   (yy)--;}
}

/////////////////////////////////////////////////
// Seconds-Time
bool TSecTm::GetTmSec(const int& YearN, const int& MonthN, const int& DayN, const int& HourN, const int& MinN, const int& SecN, uint& AbsSec) {
  AbsSec = 0;
  // tm_isdst:
  //  - Positive if daylight saving time is in effect;
  //  - 0 if daylight saving time is not in effect;
  //  - negative if status of daylight saving time is unknown.
  //  The C run-time library assumes the United States's rules for implementing
  //  the calculation of Daylight Saving Time (DST).
  struct tm Tm;
  Tm.tm_year=YearN-1900; Tm.tm_mon=MonthN-1; Tm.tm_mday=DayN;
  Tm.tm_hour=HourN; Tm.tm_min=MinN; Tm.tm_sec=SecN;
  Tm.tm_wday=1;  Tm.tm_yday=1;
  Tm.tm_isdst=-1;
  return TSecTm::GetTmSec(Tm, AbsSec);
}

// implementation of mkgmtime (taken from the web)
time_t TSecTm::MkGmTime(struct tm *t) {
  static const int m_to_d[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  short month, year;
  time_t result;
  month = t->tm_mon;
  year = t->tm_year + month / 12 + 1900;
  month %= 12;
  if (month < 0) {
    year -= 1;
    month += 12; }
  result = (year - 1970) * 365 + (year - 1969) / 4 + m_to_d[month];
  result = (year - 1970) * 365 + m_to_d[month];
  if (month <= 1) { year -= 1; }
  result += (year - 1968) / 4;
  result -= (year - 1900) / 100;
  result += (year - 1600) / 400;
  result += t->tm_mday;
  result -= 1;
  result *= 24;
  result += t->tm_hour;
  result *= 60;
  result += t->tm_min;
  result *= 60;
  result += t->tm_sec;
  return result;
}

bool TSecTm::GetTmSec(struct tm& Tm, uint& AbsSec) {
  const time_t GmtTime = MkGmTime(&Tm);
  EAssertR(uint(GmtTime) < TUInt::Mx,
    TStr::Fmt("Time out of range: %d/%d/%d %02d:%02d:%02d",
    Tm.tm_year, Tm.tm_mon, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec).CStr());
  AbsSec = uint(GmtTime);
  return GmtTime >= 0;
}

bool TSecTm::GetTmStruct(const uint& AbsSec, struct tm& Tm) {
  const time_t TimeT = time_t(AbsSec);
  #if defined(GLib_MSC)
  return _gmtime64_s(&Tm, &TimeT) == 0;
  #elif defined(GLib_BCB)
  Tm=*gmtime(&TimeT); return true;
  #else
  return gmtime_r(&TimeT, &Tm) != NULL;
  #endif
}

TSecTm::TSecTm(const int& YearN, const int& MonthN, const int& DayN,
 const int& HourN, const int& MinN, const int& SecN) : AbsSecs(TUInt::Mx){
  GetTmSec(YearN, MonthN, DayN, HourN, MinN, SecN, AbsSecs.Val);
}

TSecTm::TSecTm(const TTm& Tm): AbsSecs(
 TSecTm(Tm.GetYear(), Tm.GetMonth(), Tm.GetDay(), Tm.GetHour(),
   Tm.GetMin(), int(TMath::Round(Tm.GetSec()*1000+Tm.GetMSec()))).GetAbsSecs()){}

TSecTm::TSecTm(const PXmlTok& XmlTok) {
  const int Year = XmlTok->GetIntArgVal("Year");
  const int Month = XmlTok->GetIntArgVal("Month");
  const int Day = XmlTok->GetIntArgVal("Day");
  const int Hour = XmlTok->GetIntArgVal("Hour");
  const int Min = XmlTok->GetIntArgVal("Min");
  const int Sec = XmlTok->GetIntArgVal("Sec");
  AbsSecs = TSecTm(Year, Month, Day, Hour, Min, Sec).GetAbsSecs();
}

PXmlTok TSecTm::GetXmlTok() const {
  PXmlTok NodeTok = TXmlTok::New("NodeTime");
  NodeTok->AddArg("Year", GetYearN());
  NodeTok->AddArg("Month", GetMonthN());
  NodeTok->AddArg("Day", GetDayN());
  NodeTok->AddArg("Hour", GetHourN());
  NodeTok->AddArg("Min", GetMinN());
  NodeTok->AddArg("Sec", GetSecN());
  return NodeTok;
}

TStr TSecTm::GetStr(const TLoc& Loc) const {
  if (IsDef()) {
    struct tm Tm;
    IAssert(GetTmStruct(AbsSecs(), Tm));
    // Wed May 14 15:30:17 2003
    return TStr::Fmt("%s %s %d %02d:%02d:%02d %d",
      TTmInfo::GetDayOfWeekNm(Tm.tm_wday + 1, Loc).CStr(),
      TTmInfo::GetMonthNm(Tm.tm_mon + 1, Loc).CStr(),
      Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec, Tm.tm_year+1900);
  } else {
    return "Undef";
  }
}

TStr TSecTm::GetStr(const TTmUnit& TmUnit) const {
  if (TmUnit == tmuYear) {
    return TInt::GetStr(GetYearN()); }
  else if (TmUnit == tmuMonth) {
    return TStr::Fmt("%04d-%02d", GetYearN(), GetMonthN()); }
  else if (TmUnit == tmuDay) {
    return TStr::Fmt("%04d-%02d-%02d", GetYearN(), GetMonthN(), GetDayN()); }
  else {
    return TStr::Fmt("%04d-%02d-%02d %02d:%02d:%02d",
      GetYearN(), GetMonthN(), GetDayN(), GetHourN(), GetMinN(), GetSecN());
  }
}

TStr TSecTm::GetDtStr(const TLoc& Loc) const {
  if (IsDef()){
    struct tm Tm;
    IAssert(GetTmStruct(AbsSecs(), Tm));
    return TStr::Fmt("%s %s %d %d",
      TTmInfo::GetDayOfWeekNm(Tm.tm_wday + 1, Loc).CStr(),
      TTmInfo::GetMonthNm(Tm.tm_mon + 1, Loc).CStr(), Tm.tm_year+1900);
  } else {
    return "Undef";
  }
}

TStr TSecTm::GetDtMdyStr() const {
  struct tm Tm;
  IAssert(GetTmStruct(AbsSecs(), Tm));
  return TStr::Fmt("%02d/%02d%/%04d", Tm.tm_mon+1, Tm.tm_mday, Tm.tm_year+1900);
}

TStr TSecTm::GetDtYmdStr() const {
  struct tm Tm;
  IAssert(GetTmStruct(AbsSecs(), Tm));
  return TStr::Fmt("%04d-%02d-%02d", Tm.tm_year+1900, Tm.tm_mon+1, Tm.tm_mday);
}

TStr TSecTm::GetYmdTmStr() const {
  struct tm Tm;
  IAssert(GetTmStruct(AbsSecs(), Tm));
  return TStr::Fmt("%04d-%02d-%02d %02d:%02d:%02d", Tm.tm_year+1900, Tm.tm_mon+1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec);
}


TStr TSecTm::GetTmStr() const {
  if (IsDef()){
    struct tm Tm;
    IAssert(GetTmStruct(AbsSecs(), Tm));
    return TStr::Fmt("%02d:%02d:%02d", Tm.tm_hour, Tm.tm_min, Tm.tm_sec);
  } else {
    return "Undef";
  }
}

TStr TSecTm::GetTmMinStr() const {
  if (IsDef()){
    struct tm Tm;
    IAssert(GetTmStruct(AbsSecs(), Tm));
    return TStr::Fmt("%02d:%02d", Tm.tm_min, Tm.tm_sec);
  } else {
    return "Undef";
  }
}

TStr TSecTm::GetDtTmSortStr() const {
  return
    TInt::GetStr(GetYearN(), "%04d")+"/"+
    TInt::GetStr(GetMonthN(), "%02d")+"/"+
    TInt::GetStr(GetDayN(), "%02d")+" "+
    TInt::GetStr(GetHourN(), "%02d")+":"+
    TInt::GetStr(GetMinN(), "%02d")+":"+
    TInt::GetStr(GetSecN(), "%02d");
}

TStr TSecTm::GetDtTmSortFNmStr() const {
  return
    TInt::GetStr(GetYearN(), "%04d")+"-"+
    TInt::GetStr(GetMonthN(), "%02d")+"-"+
    TInt::GetStr(GetDayN(), "%02d")+"_"+
    TInt::GetStr(GetHourN(), "%02d")+"-"+
    TInt::GetStr(GetMinN(), "%02d")+"-"+
    TInt::GetStr(GetSecN(), "%02d");
}

int TSecTm::GetYearN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_year+1900;
}

int TSecTm::GetMonthN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_mon+1;
}

TStr TSecTm::GetMonthNm(const TLoc& Loc) const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return TTmInfo::GetMonthNm(Tm.tm_mon+1, Loc);
}

int TSecTm::GetDayN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_mday;
}

int TSecTm::GetDayOfWeekN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_wday + 1;
}

TStr TSecTm::GetDayOfWeekNm(const TLoc& Loc) const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return TTmInfo::GetDayOfWeekNm(Tm.tm_wday+1, Loc);
}

int TSecTm::GetHourN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_hour;
}

int TSecTm::GetMinN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_min;
}

int TSecTm::GetSecN() const {
  struct tm Tm;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Tm));
  return Tm.tm_sec;
}

TSecTm TSecTm::Round(const TTmUnit& TmUnit) const {
  if (TmUnit == tmu1Sec) { return *this; }
  struct tm Time;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Time));
  switch (TmUnit) {
    case tmu1Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, Time.tm_min, 0);
    case tmu10Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 10*(Time.tm_min/10), 0);
    case tmu15Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 15*(Time.tm_min/15), 0);
    case tmu30Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 30*(Time.tm_min/30), 0);
    case tmu1Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 0, 0);
    case tmu2Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 2*(Time.tm_hour/2), 0, 0);
    case tmu4Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 4*(Time.tm_hour/4), 0, 0);
    case tmu6Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 6*(Time.tm_hour/6), 0, 0);
    case tmu12Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 12*(Time.tm_hour/12), 0, 0);
    case tmuDay : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 0, 0, 0);
    case tmuMonth : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, 1, 0, 0, 0);
    case tmuYear : return TSecTm(Time.tm_year+1900, 1, 1, 0, 0, 0);
    case tmuWeek : { int dd=1, mm=1, yy=1;
      // week starts on Thursday, since 1.1.1970 is Thursday
      const int Day = TJulianDate::GetJulianDateN(Time.tm_mday, Time.tm_mon+1, 1900+Time.tm_year);
      TJulianDate::GetCalendarDate(3+7*(Day/7), dd, mm, yy);  return TSecTm(yy, mm, dd, 0, 0, 0); }
    default : Fail;
  }
  return TSecTm();
}
uint TSecTm::GetInUnits(const TTmUnit& TmUnit) const {
  static const int DayZero = TJulianDate::GetJulianDateN(1, 1, 1970);
  if (TmUnit == tmu1Sec) { return AbsSecs; }
  struct tm Time;
  IAssert(IsDef() && GetTmStruct(AbsSecs(), Time));
  switch (TmUnit) {
    case tmu1Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, Time.tm_min, 0).GetAbsSecs()/60;
    case tmu10Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 10*(Time.tm_min/10), 0).GetAbsSecs()/(10*60);
    case tmu15Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 15*(Time.tm_min/15), 0).GetAbsSecs()/(15*60);
    case tmu30Min : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 30*(Time.tm_min/30), 0).GetAbsSecs()/(30*60);
    case tmu1Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, Time.tm_hour, 0, 0).GetAbsSecs()/3600;
    case tmu2Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 2*(Time.tm_hour/2), 0, 0).GetAbsSecs()/(2*3600);
    case tmu4Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 4*(Time.tm_hour/4), 0, 0).GetAbsSecs()/(4*3600);
    case tmu6Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 6*(Time.tm_hour/6), 0, 0).GetAbsSecs()/(6*3600);
    case tmu12Hour : return TSecTm(Time.tm_year+1900, Time.tm_mon+1, Time.tm_mday, 12*(Time.tm_hour/12), 0, 0).GetAbsSecs()/(12*3600);
    case tmuDay : return TJulianDate::GetJulianDateN(Time.tm_mday, Time.tm_mon+1, 1900+Time.tm_year) - DayZero;
    case tmuWeek : return (TJulianDate::GetJulianDateN(Time.tm_mday, Time.tm_mon+1, 1900+Time.tm_year)-DayZero)/7;
    case tmuMonth : return 12*(Time.tm_year-70)+Time.tm_mon+1;
    case tmuYear : return Time.tm_year+1900;
    default : Fail;
  }
  return TUInt::Mx;
}

uint TSecTm::GetDSecs(const TSecTm& SecTm1, const TSecTm& SecTm2){
  IAssert(SecTm1.IsDef()&&SecTm2.IsDef());
  const time_t Time1= time_t(SecTm1.AbsSecs());
  const time_t Time2= time_t(SecTm2.AbsSecs());
  return uint(difftime(Time2, Time1));
}

TSecTm TSecTm::GetZeroWeekTm(){
  TSecTm ZeroWeekTm=GetZeroTm();
  while (ZeroWeekTm.GetDayOfWeekN()!=TTmInfo::MonN){
    ZeroWeekTm.AddDays(1);}
  return ZeroWeekTm;
}

TSecTm TSecTm::GetCurTm(){
  const time_t TmSec = time(NULL);
  struct tm LocTm;
  uint AbsSec = TUInt::Mx;
  #if defined(GLib_MSN)
  localtime_s(&LocTm, &TmSec);
  #elif defined(GLib_BCB)
  LocTm = *localtime(&TmSec);
  #else
  LocTm = *localtime(&TmSec);
  #endif
  IAssert(TSecTm::GetTmSec(LocTm, AbsSec));
  return TSecTm(AbsSec);
}

TSecTm TSecTm::GetDtTmFromHmsStr(const TStr& HmsStr){
  int HmsStrLen=HmsStr.Len();
  // hour
  TChA ChA; int ChN=0;
  while ((ChN<HmsStrLen)&&(HmsStr[ChN]!=':')){ChA+=HmsStr[ChN]; ChN++;}
  TStr HourStr=ChA;
  // minute
  ChA.Clr(); ChN++;
  while ((ChN<HmsStrLen)&&(HmsStr[ChN]!=':')){ChA+=HmsStr[ChN]; ChN++;}
  TStr MinStr=ChA;
  // second
  ChA.Clr(); ChN++;
  while (ChN<HmsStrLen){ChA+=HmsStr[ChN]; ChN++;}
  TStr SecStr=ChA;
  // transform to numbers
  int HourN=HourStr.GetInt();
  int MinN=MinStr.GetInt();
  int SecN=SecStr.GetInt();
  // construct the time
  TSecTm Tm=TSecTm::GetZeroTm();
  Tm.AddHours(HourN);
  Tm.AddMins(MinN);
  Tm.AddSecs(SecN);
  return Tm;
}

TSecTm TSecTm::GetDtTmFromMdyStr(const TStr& MdyStr){
  int MdyStrLen=MdyStr.Len();
  // month
  TChA ChA; int ChN=0;
  while ((ChN<MdyStrLen)&&(MdyStr[ChN]!='/')){
    ChA+=MdyStr[ChN]; ChN++;}
  TStr MonthStr=ChA;
  // day
  ChA.Clr(); ChN++;
  while ((ChN<MdyStrLen)&&(MdyStr[ChN]!='/')){
    ChA+=MdyStr[ChN]; ChN++;}
  TStr DayStr=ChA;
  // year
  ChA.Clr(); ChN++;
  while (ChN<MdyStrLen){
    ChA+=MdyStr[ChN]; ChN++;}
  TStr YearStr=ChA;
  // transform to numbers
  int MonthN=MonthStr.GetInt();
  int DayN=DayStr.GetInt();
  int YearN=YearStr.GetInt();
  if (YearN<1000){
    if (YearN<70){YearN+=2000;} else {YearN+=1900;}}
  // construct the date
  return GetDtTm(YearN, MonthN, DayN);
}

// parse 28/03/03 and 28-MAY-03 formats
TSecTm TSecTm::GetDtTmFromDmyStr(const TStr& DmyStr){
  int DmyStrLen=DmyStr.Len();
  // day
  TChA ChA; int ChN=0;
  while ((ChN<DmyStrLen)&&(DmyStr[ChN]!='/')&&(DmyStr[ChN]!='-')){
    ChA+=DmyStr[ChN]; ChN++;}
  TStr DayStr=ChA;
  // month
  ChA.Clr(); ChN++;
  while ((ChN<DmyStrLen)&&(DmyStr[ChN]!='/')&&(DmyStr[ChN]!='-')){
    ChA+=DmyStr[ChN]; ChN++;}
  TStr MonthStr=ChA;
  // year
  ChA.Clr(); ChN++;
  while (ChN<DmyStrLen){
    ChA+=DmyStr[ChN]; ChN++;}
  TStr YearStr=ChA;
  // transform to numbers
  int DayN=DayStr.GetInt(-1);
  int MonthN=MonthStr.GetInt(-1);
  int YearN=YearStr.GetInt(-1);
  if (MonthN == -1){
    MonthN = TTmInfo::GetMonthN(MonthStr.ToCap()); }
  if ((DayN==-1)||(MonthN==-1)||(YearN==-1)){
    return TSecTm();
  } else {
    if (YearN<1000){
      if (YearN<70){YearN+=2000;} else {YearN+=1900;}}
    // construct the date
    return GetDtTm(YearN, MonthN, DayN);
  }
  return TSecTm();
}

TSecTm TSecTm::GetDtTmFromMdyHmsPmStr(const TStr& MdyHmsPmStr,
 const char& DateSepCh, const char& TimeSepCh){
  int MdyHmsPmStrLen=MdyHmsPmStr.Len();
  // month
  TChA ChA; int ChN=0;
  while ((ChN<MdyHmsPmStrLen)&&(MdyHmsPmStr[ChN]!=DateSepCh)){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr MonthStr=ChA;
  // day
  ChA.Clr(); ChN++;
  while ((ChN<MdyHmsPmStrLen)&&(MdyHmsPmStr[ChN]!=DateSepCh)){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr DayStr=ChA;
  // year
  ChA.Clr(); ChN++;
  while ((ChN<MdyHmsPmStrLen)&&(MdyHmsPmStr[ChN]!=' ')){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr YearStr=ChA;
  // hour
  ChA.Clr(); ChN++;
  while ((ChN<MdyHmsPmStrLen)&&(MdyHmsPmStr[ChN]!=TimeSepCh)){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr HourStr=ChA;
  // minute
  ChA.Clr(); ChN++;
  while ((ChN<MdyHmsPmStrLen)&&(MdyHmsPmStr[ChN]!=TimeSepCh)){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr MinStr=ChA;
  // second
  ChA.Clr(); ChN++;
  while ((ChN<MdyHmsPmStrLen)&&(MdyHmsPmStr[ChN]!=' ')){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr SecStr=ChA;
  // AM/PM
  ChA.Clr(); ChN++;
  while (ChN<MdyHmsPmStrLen){
    ChA+=MdyHmsPmStr[ChN]; ChN++;}
  TStr AmPmStr=ChA;
  // transform to numbers
  int MonthN=MonthStr.GetInt();
  int DayN=DayStr.GetInt();
  int YearN=YearStr.GetInt();
  int HourN; int MinN; int SecN;
  if (HourStr.IsInt()){
    HourN=HourStr.GetInt();
    MinN=MinStr.GetInt();
    SecN=SecStr.GetInt();
    if (AmPmStr=="AM"){} else if (AmPmStr=="PM"){HourN+=12;} else {Fail;}
  } else {
    HourN=0; MinN=0; SecN=0;
  }
  // construct the time
  TSecTm Tm=TSecTm::GetDtTm(YearN, MonthN, DayN);
  Tm.AddHours(HourN);
  Tm.AddMins(MinN);
  Tm.AddSecs(SecN);
  return Tm;
}

TSecTm TSecTm::GetDtTmFromYmdHmsStr(const TStr& YmdHmsPmStr,
 const char& DateSepCh, const char& TimeSepCh){
  int YmdHmsPmStrLen=YmdHmsPmStr.Len();
  // year
  TChA ChA; int ChN=0;
  while ((ChN<YmdHmsPmStrLen)&&(YmdHmsPmStr[ChN]!=DateSepCh)){
    ChA+=YmdHmsPmStr[ChN]; ChN++;}
  TStr YearStr=ChA;
  // month
  ChA.Clr(); ChN++;
  while ((ChN<YmdHmsPmStrLen)&&(YmdHmsPmStr[ChN]!=DateSepCh)){
    ChA+=YmdHmsPmStr[ChN]; ChN++;}
  TStr MonthStr=ChA;
  // day
  ChA.Clr(); ChN++;
  while ((ChN<YmdHmsPmStrLen)&&(YmdHmsPmStr[ChN]!=' ')){
    ChA+=YmdHmsPmStr[ChN]; ChN++;}
  TStr DayStr=ChA;
  // hour
  ChA.Clr(); ChN++;
  while ((ChN<YmdHmsPmStrLen)&&(YmdHmsPmStr[ChN]!=TimeSepCh)){
    ChA+=YmdHmsPmStr[ChN]; ChN++;}
  TStr HourStr=ChA;
  // minute
  ChA.Clr(); ChN++;
  while ((ChN<YmdHmsPmStrLen)&&(YmdHmsPmStr[ChN]!=TimeSepCh)){
    ChA+=YmdHmsPmStr[ChN]; ChN++;}
  TStr MinStr=ChA;
  // second
  ChA.Clr(); ChN++;
  while (ChN<YmdHmsPmStrLen){
    ChA+=YmdHmsPmStr[ChN]; ChN++;}
  TStr SecStr=ChA;
  // transform to numbers
  int MonthN=MonthStr.GetInt();
  int DayN=DayStr.GetInt();
  int YearN=YearStr.GetInt();
  int HourN; int MinN; int SecN;
  if (HourStr.IsInt()){
    HourN=HourStr.GetInt();
    MinN=MinStr.GetInt();
    SecN=SecStr.GetInt();
  } else {
    HourN=0; MinN=0; SecN=0;
  }
  // construct the time
  TSecTm Tm=TSecTm::GetDtTm(YearN, MonthN, DayN);
  Tm.AddHours(HourN);
  Tm.AddMins(MinN);
  Tm.AddSecs(SecN);
  return Tm;
}

// Parse strings of the form 2006-08-28 14:11:16 or 14:11:16  08/28/2008
// Non-numeric characters act as separators (there can be many consecutive separating characters)
// Variables give indexes of the date fields
TSecTm TSecTm::GetDtTmFromStr(const TChA& YmdHmsPmStr, const int& YearId, const int& MonId,
 const int& DayId, const int& HourId, const int& MinId, const int& SecId) {
  TChA Tmp = YmdHmsPmStr;
  TVec<char *> FldV;
  // get the sequences of numbers
  for (char *c = (char *) Tmp.CStr(); *c; c++) {
    if (TCh::IsNum(*c)) {
      FldV.Add(c);
      while (TCh::IsNum(*c)) { c++; }
      c--;
    } else { *c = 0; }
  }
  const int Y = atoi(FldV[YearId]);
  const int M = atoi(FldV[MonId]);
  const int D = atoi(FldV[DayId]);
  const int H = atoi(FldV[HourId]);
  const int m = atoi(FldV[MinId]);
  const int S = atoi(FldV[SecId]);
  IAssert(Y>0 && M>0 && D>0 && M<13 && D<32);
  IAssert(H>=0 && H<24 && m>=0 && m<60 && S>=0 && S<60);
  return TSecTm(Y,M,D,H,m,S);
}

TSecTm TSecTm::GetDtTm(const int& YearN, const int& MonthN, const int& DayN){
  uint AbsSecs;
  TSecTm::GetTmSec(YearN, MonthN, DayN, 0, 0, 0, AbsSecs);
  return TSecTm(AbsSecs);
}

TSecTm TSecTm::GetDtTm(const TSecTm& Tm){
  int DaySecs=Tm.GetHourN()*3600+Tm.GetMinN()*60+Tm.GetSecN();
  TSecTm DtTm(Tm.AbsSecs-DaySecs);
  return DtTm;
}

TSecTm TSecTm::LoadTxt(TILx& Lx){
  return TSecTm(Lx.GetInt());
}

void TSecTm::SaveTxt(TOLx& Lx) const {
  IAssert(int(AbsSecs) < TInt::Mx);
  Lx.PutInt((int)AbsSecs);
}

/////////////////////////////////////////////////
// Date-Time
TStr TTm::GetStr(const bool& MSecP) const {
  TChA ChA;
  ChA+=TInt::GetStr(Year, "%04d"); ChA+='-';
//  ChA+=GetMonthNm(); ChA+='-';
  ChA+=TInt::GetStr(Month, "%02d"); ChA+='-';
  ChA+=TInt::GetStr(Day, "%02d"); ChA+=' ';
//  ChA+=GetDayOfWeekNm(); ChA+=' ';
  ChA+=TInt::GetStr(Hour, "%02d"); ChA+=':';
  ChA+=TInt::GetStr(Min, "%02d"); ChA+=':';
  ChA+=TInt::GetStr(Sec, "%02d");
  if (MSecP){ChA+='.'; ChA+=TInt::GetStr(MSec, "%04d");}
  return ChA;
}

TStr TTm::GetYMDDashStr() const {
  TChA ChA;
  ChA+=TInt::GetStr(Year, "%04d");
  ChA+='-'; ChA+=TInt::GetStr(Month, "%02d");
  ChA+='-'; ChA+=TInt::GetStr(Day, "%02d");
  return ChA;
}

TStr TTm::GetHMSTColonDotStr(const bool& FullP, const bool& MSecP) const {
  TChA ChA;
  ChA+=TInt::GetStr(Hour, "%02d");
  ChA+=':'; ChA+=TInt::GetStr(Min, "%02d");
  if (FullP||((Sec!=0)||(MSec!=0))){
    ChA+=':'; ChA+=TInt::GetStr(Sec, "%02d");
    if ((MSecP)&&(FullP||(MSec!=0))){
      ChA+='.'; ChA+=TInt::GetStr(MSec, "%d");
    }
  }
  return ChA;
}

TStr TTm::GetIdStr() const {
  TChA ChA;
  ChA+=TInt::GetStr(Year%100, "%02d");
  ChA+=TInt::GetStr(Month, "%02d");
  ChA+=TInt::GetStr(Day, "%02d");
  ChA+=TInt::GetStr(Hour, "%02d");
  ChA+=TInt::GetStr(Min, "%02d");
  ChA+=TInt::GetStr(Sec, "%02d");
  ChA+=TInt::GetStr(MSec, "%03d");
  return ChA;
}


void TTm::AddTime(const int& Hours,
 const int& Mins, const int& Secs, const int& MSecs){
  uint64 TmMSecs=TTm::GetMSecsFromTm(*this);
  TmMSecs+=(uint64(Hours)*uint64(3600)*uint64(1000));
  TmMSecs+=(uint64(Mins)*uint64(60)*uint64(1000));
  TmMSecs+=(uint64(Secs)*uint64(1000));
  TmMSecs+=uint64(MSecs);
  *this=GetTmFromMSecs(TmMSecs);
}

void TTm::SubTime(const int& Hours,
 const int& Mins, const int& Secs, const int& MSecs){
  uint64 TmMSecs=TTm::GetMSecsFromTm(*this);
  TmMSecs-=(uint64(Hours)*uint64(3600)*uint64(1000));
  TmMSecs-=(uint64(Mins)*uint64(60)*uint64(1000));
  TmMSecs-=(uint64(Secs)*uint64(1000));
  TmMSecs-=(uint64(MSecs));
  *this=GetTmFromMSecs(TmMSecs);
}

TTm TTm::GetCurUniTm(){
  return TSysTm::GetCurUniTm();
}

TTm TTm::GetUniqueCurUniTm(){
  static TTm LastUniqueTm=TSysTm::GetCurUniTm();
  TTm CurUniqueTm=TSysTm::GetCurUniTm();
  if (CurUniqueTm<LastUniqueTm){CurUniqueTm=LastUniqueTm;}
  if (CurUniqueTm==LastUniqueTm){CurUniqueTm.AddTime(0, 0, 0, 1);}
  LastUniqueTm=CurUniqueTm;
  return CurUniqueTm;
}

TTm TTm::GetUniqueCurUniTm(const int& UniqueSpaces, const int& UniqueSpaceN){
  static uint64 LastMUniqueTmMSecs=TSysTm::GetCurUniMSecs();
  // uniqueness-space-parameters range-check
  Assert(UniqueSpaces>=1&&UniqueSpaceN>=0&&UniqueSpaceN<UniqueSpaces);
  // get current time
  uint64 CurUniqueTmMSecs=TSysTm::GetCurUniMSecs();
  if (CurUniqueTmMSecs<LastMUniqueTmMSecs){CurUniqueTmMSecs=LastMUniqueTmMSecs;}
  // normalize to uniqueness-space-grid
  CurUniqueTmMSecs-=CurUniqueTmMSecs%UniqueSpaces; CurUniqueTmMSecs+=UniqueSpaceN;
  // get next free unique-time
  if (CurUniqueTmMSecs<=LastMUniqueTmMSecs){
    CurUniqueTmMSecs+=UniqueSpaces;
  }
  // update last-time
  LastMUniqueTmMSecs=CurUniqueTmMSecs;
  return GetTmFromMSecs(CurUniqueTmMSecs);
}

TTm TTm::GetCurLocTm(){
  return TSysTm::GetCurLocTm();
}

uint64 TTm::GetCurUniMSecs(){
  return TSysTm::GetCurUniMSecs();
}

uint64 TTm::GetCurLocMSecs(){
  return TSysTm::GetCurLocMSecs();
}

uint64 TTm::GetMSecsFromTm(const TTm& Tm){
  return TSysTm::GetMSecsFromTm(Tm);
}

TTm TTm::GetTmFromMSecs(const uint64& MSecs){
  return TSysTm::GetTmFromMSecs(MSecs);
}

uint TTm::GetMSecsFromOsStart(){
  return TSysTm::GetMSecsFromOsStart();
}

uint64 TTm::GetPerfTimerFq(){
  return TSysTm::GetPerfTimerFq();
}

uint64 TTm::GetPerfTimerTicks(){
  return TSysTm::GetPerfTimerTicks();
}

uint64 TTm::GetDiffMSecs(const TTm& Tm1, const TTm& Tm2){
  uint64 Tm1MSecs=GetMSecsFromTm(Tm1);
  uint64 Tm2MSecs=GetMSecsFromTm(Tm2);
  if (Tm1MSecs>Tm2MSecs){
    return Tm1MSecs-Tm2MSecs;
  } else {
    return Tm2MSecs-Tm1MSecs;
  }
}

TTm TTm::GetLocTmFromUniTm(const TTm& Tm){
  return TSysTm::GetLocTmFromUniTm(Tm);
}

TTm TTm::GetUniTmFromLocTm(const TTm& Tm){
  return TSysTm::GetUniTmFromLocTm(Tm);
}


/*
//!!peter
class TMonthParser{
private:
  TStrIntH MonH;
public:
  TMonthParser() {
    for (int i=1; i<=12; i++) {
      TStr Us=TTmInfo::GetMonthNm(i,lUs).GetUc();
      TStr Si=TTmInfo::GetMonthNm(i,lSi).GetUc();
      MonH.AddDat(Us,i); if (Us!=Si) MonH.AddDat(Si,i);
    }
  }
  int GetMonthN(const TStr &Month) {
    TInt MonN=-1;
    MonH.IsKeyGetDat(Month.GetUc(),MonN);
    return MonN;
  }
};
static TMonthParser MonthParser;*/

TTm TTm::GetTmFromWebLogDateTimeStr(const TStr& DateTimeStr,
 const char DateSepCh, const char TimeSepCh, const char MSecSepCh){
  int DateTimeStrLen=DateTimeStr.Len();
  // year
  TChA ChA; int ChN=0;
  while ((ChN<DateTimeStrLen)&&(DateTimeStr[ChN]!=DateSepCh)){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr YearStr=ChA;
  // month
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeStrLen)&&(DateTimeStr[ChN]!=DateSepCh)){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr MonthStr=ChA;
  // day
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeStrLen)&&(DateTimeStr[ChN]!=' ')){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr DayStr=ChA;
  // hour
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeStrLen)&&(DateTimeStr[ChN]!=TimeSepCh)){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr HourStr=ChA;
  // minute
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeStrLen)&&(DateTimeStr[ChN]!=TimeSepCh)){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr MinStr=ChA;
  // second
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeStrLen)&&(DateTimeStr[ChN]!=MSecSepCh)){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr SecStr=ChA;
  // mili-second
  ChA.Clr(); ChN++;
  while (ChN<DateTimeStrLen){
    ChA+=DateTimeStr[ChN]; ChN++;}
  TStr MSecStr=ChA;
  // transform to numbers
  int YearN=YearStr.GetInt(-1);
  int MonthN=MonthStr.GetInt(-1);
  int DayN=DayStr.GetInt(-1);
  int HourN=HourStr.GetInt(0);
  int MinN=MinStr.GetInt(0);
  int SecN=SecStr.GetInt(0);
  int MSecN=MSecStr.GetInt(0);
  // construct time

/*
  //!!peter: convert month name to number and flip date/day (oracle: 10-FEB-05)
  if ((MonthN==-1)&&(isalpha(MonthStr.CStr()[0]))){
    if ((MonthN=MonthParser.GetMonthN(MonthStr))!=-1){
      int Y=DayN; DayN=YearN; YearN=Y<100?Y+2000:Y;
    }
  }
*/

  TTm Tm;
  if ((YearN!=-1)&&(MonthN!=-1)&&(DayN!=-1)){
    Tm=TTm(YearN, MonthN, DayN, -1, HourN, MinN, SecN, MSecN);
  }
  // return time
  return Tm;
}

TTm TTm::GetTmFromIdStr(const TStr& IdStr){
  // normalize
  TChA IdChA=IdStr;
  if (IdChA.Len()==14){
    IdChA.Ins(0, "0");}
  // check
  IAssert(IdChA.Len()==15);
  for (int ChN=0; ChN<IdChA.Len(); ChN++){
    IAssert(TCh::IsNum(IdChA[ChN]));}
  // extract parts
  int YearN=2000+(TStr(IdChA[0])+TStr(IdChA[1])).GetInt();
  int MonthN=(TStr(IdChA[2])+TStr(IdChA[3])).GetInt();
  int DayN=(TStr(IdChA[4])+TStr(IdChA[5])).GetInt();
  int HourN=(TStr(IdChA[6])+TStr(IdChA[7])).GetInt();
  int MinN=(TStr(IdChA[8])+TStr(IdChA[9])).GetInt();
  int SecN=(TStr(IdChA[10])+TStr(IdChA[11])).GetInt();
  int MSecN=(TStr(IdChA[12])+TStr(IdChA[13])+TStr(IdChA[14])).GetInt();
  TTm Tm=TTm(YearN, MonthN, DayN, -1, HourN, MinN, SecN, MSecN);
  return Tm;
}

uint TTm::GetDateTimeInt(const int& Year, const int& Month,
      const int& Day, const int& Hour, const int& Min) {

    TTmDateTime DateTime; DateTime.Int = 0;
    DateTime.Bits.Year = Year;
    DateTime.Bits.Month = Month;
    DateTime.Bits.Day = Day;
    DateTime.Bits.Hour = Hour;
    DateTime.Bits.Min = Min;
    return DateTime.Int;
}

uint TTm::GetDateIntFromTm(const TTm& Tm) {
    return GetDateTimeInt(Tm.GetYear(), Tm.GetMonth(), Tm.GetDay());
}

uint TTm::GetMonthIntFromTm(const TTm& Tm) {
    return GetDateTimeInt(Tm.GetYear(), Tm.GetMonth());
}

uint TTm::GetYearIntFromTm(const TTm& Tm) {
    return GetDateTimeInt(Tm.GetYear());
}

uint TTm::GetDateTimeIntFromTm(const TTm& Tm) {
    return GetDateTimeInt(Tm.GetYear(), Tm.GetMonth(),
        Tm.GetDay(), Tm.GetHour(), Tm.GetMin());
}

TTm TTm::GetTmFromDateTimeInt(const uint& DateTimeInt) {
    TTmDateTime DateTime; DateTime.Int = DateTimeInt;
    return TTm(DateTime.Bits.Year, DateTime.Bits.Month, DateTime.Bits.Day,
        -1, DateTime.Bits.Hour, DateTime.Bits.Min, 0, 0);
}

uint TTm::KeepMonthInDateTimeInt(const uint& DateTimeInt) {
    TTmDateTime DateTime;
    DateTime.Int = DateTimeInt;
    DateTime.Bits.Year = 2000;
    DateTime.Bits.Day = 1;
    DateTime.Bits.Hour = 0;
    DateTime.Bits.Min = 0;
    return DateTime.Int;
}

uint TTm::KeepDayInDateTimeInt(const uint& DateTimeInt) {
    TTmDateTime DateTime;
    DateTime.Int = DateTimeInt;
    DateTime.Bits.Year = 2000;
    DateTime.Bits.Month = 1;
    DateTime.Bits.Hour = 0;
    DateTime.Bits.Min = 0;
    return DateTime.Int;
}

uint TTm::KeepHourInDateTimeInt(const uint& DateTimeInt) {
    TTmDateTime DateTime;
    DateTime.Int = DateTimeInt;
    DateTime.Bits.Year = 2000;
    DateTime.Bits.Month = 1;
    DateTime.Bits.Day = 1;
    DateTime.Bits.Min = 0;
    return DateTime.Int;
}
