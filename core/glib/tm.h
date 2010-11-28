/////////////////////////////////////////////////
// Forward
class TILx;
class TOLx;
class TTm;

/////////////////////////////////////////////////
// Time-Units (round time to one of the buckets)
typedef enum {
  tmuUndef, tmu1Sec, tmu1Min, tmu10Min, tmu15Min, tmu30Min,
  tmu1Hour, tmu2Hour, tmu4Hour, tmu6Hour, tmu12Hour, tmuDay, tmuWeek, // Sunday is the start of the week
  tmuMonth, tmuYear, tmuNodes, tmuEdges
  // wrap around time
  // tmuWrapHour, tmuWrapDay, tmuWrapWeek, tmuWrapMonth, tmuWrapYear
} TTmUnit;

/////////////////////////////////////////////////
// Time-Info
class TTmInfo{
private:
  static bool InitP;
  static TStrV UsMonthNmV;
  static TStrV SiMonthNmV;
  static TStrV UsDayOfWeekNmV;
  static TStrV SiDayOfWeekNmV;
  static void InitMonthNmV();
  static void InitDayOfWeekNmV();
  static void EnsureInit(){
    if (!InitP){InitMonthNmV(); InitDayOfWeekNmV(); InitP=true;}}
public:
  static int GetMonthN(const TStr& MonthNm, const TLoc& Loc=lUs);
  static bool IsMonthNm(const TStr& MonthNm, const TLoc& Loc=lUs){
    return GetMonthN(MonthNm, Loc)!=-1;}
  static TStr GetMonthNm(const int& MonthN, const TLoc& Loc=lUs);
  static int GetDayOfWeekN(const TStr& DayOfWeekNm, const TLoc& Loc=lUs);
  static bool IsDayOfWeekNm(const TStr& DayOfWeekNm, const TLoc& Loc=lUs){
    return GetDayOfWeekN(DayOfWeekNm, Loc)!=-1;}
  static TStr GetDayOfWeekNm(const int& DayOfWeekN, const TLoc& Loc=lUs);
  static TStr GetHmFromMins(const int& Mins);
  static int GetTmUnitSecs(const TTmUnit& TmUnit);
  static TStr GetTmUnitStr(const TTmUnit& TmUnit);
  static TStr GetTmZoneDiffStr(const TStr& TmZoneStr);

  // day-of-week numbers
  static const int SunN; static const int MonN;
  static const int TueN; static const int WedN;
  static const int ThuN; static const int FriN;
  static const int SatN;

  // month numbers
  static const int JanN; static const int FebN;
  static const int MarN; static const int AprN;
  static const int MayN; static const int JunN;
  static const int JulN; static const int AugN;
  static const int SepN; static const int OctN;
  static const int NovN; static const int DecN;

  // time duration (msecs)
  static uint64 GetMinMSecs(){return 60*1000;}
  static uint64 GetHourMSecs(){return 60*60*1000;}
  static uint64 GetDayMSecs(){return 24*60*60*1000;}
  static uint64 GetWeekMSecs(){return 7*24*60*60*1000;}
};

/////////////////////////////////////////////////
// Julian-Dates
class TJulianDate{
public:
  static int LastJulianDate; /* last day to use Julian calendar */
  static int LastJulianDateN; /* jdn of same */
  static int GetJulianDateN(int d, int m, int y);
  static void GetCalendarDate(int jdn, int& dd, int& mm, int& yy);
};

/////////////////////////////////////////////////
// Seconds-Time
// NOTE: Time origin is relative to time zone. Regardless of computer time
// zone second 0 maps to "Jan 1 1970 00:00" (so time zone information is lost)
class TSecTm {
private:
  TUInt AbsSecs; // pretend that local time zone is UTC
private:
  // functions that interact with C time functions (time.h)
  static bool GetTmSec(const int& YearN, const int& MonthN, const int& DayN,
   const int& HourN, const int& MinN, const int& SecN, uint& AbsSec);
  static bool GetTmSec(struct tm& Tm, uint& AbsSec);
  static bool GetTmStruct(const uint& AbsSec, struct tm& Tm);
  static time_t MkGmTime(struct tm *t); // implementation of _mkgmtime
public:
  TSecTm(): AbsSecs(TUInt::Mx){}
  explicit TSecTm(const uint& _AbsSecs): AbsSecs(_AbsSecs){}
  operator uint() const {return AbsSecs.Val;}
  TSecTm(const TSecTm& SecTm): AbsSecs(SecTm.AbsSecs){}
  TSecTm(const int& YearN, const int& MonthN, const int& DayN,
   const int& HourN=0, const int& MinN=0, const int& SecN=0);
  TSecTm(const TTm& Tm);
  explicit TSecTm(const PXmlTok& XmlTok);
  PXmlTok GetXmlTok() const;
  TSecTm(TSIn& SIn): AbsSecs(SIn){}
  void Load(TSIn& SIn){AbsSecs.Load(SIn);}
  void Save(TSOut& SOut) const {AbsSecs.Save(SOut);}

  TSecTm& operator=(const TSecTm& SecTm){
    AbsSecs=SecTm.AbsSecs; return *this;}
  TSecTm& operator=(const uint& _AbsSecs){
    AbsSecs=_AbsSecs; return *this;}
  TSecTm& operator+=(const uint& Secs){
    IAssert(IsDef()); AbsSecs()+=Secs; return *this;}
  TSecTm& operator-=(const uint& Secs){
    IAssert(IsDef()); AbsSecs()-=Secs; return *this;}
  bool operator==(const TSecTm& SecTm) const {
    return AbsSecs==SecTm.AbsSecs;}
  bool operator<(const TSecTm& SecTm) const {
    IAssert(IsDef()&&SecTm.IsDef()); return AbsSecs<SecTm.AbsSecs;}
  int GetMemUsed() const {return AbsSecs.GetMemUsed();}

  int GetPrimHashCd() const {return AbsSecs.GetPrimHashCd();}
  int GetSecHashCd() const {return AbsSecs.GetSecHashCd();}

  // definition
  bool IsDef() const {return uint(AbsSecs)!=TUInt::Mx;}
  void Undef(){AbsSecs=TUInt::Mx;}

  // string retrieval
  TStr GetStr(const TLoc& Loc=lUs) const;
  TStr GetStr(const TTmUnit& TmUnit) const;
  TStr GetDtStr(const TLoc& Loc=lUs) const;
  TStr GetDtMdyStr() const;
  TStr GetDtYmdStr() const;
  TStr GetYmdTmStr() const; // returns "y-m-d h:m:s"
  TStr GetTmStr() const;
  TStr GetTmMinStr() const;
  TStr GetDtTmSortStr() const;
  TStr GetDtTmSortFNmStr() const;

  // component retrieval
  int GetYearN() const;
  int GetMonthN() const;
  TStr GetMonthNm(const TLoc& Loc=lUs) const;
  int GetDayN() const;
  int GetDayOfWeekN() const;
  TStr GetDayOfWeekNm(const TLoc& Loc=lUs) const;
  int GetHourN() const;
  int GetMinN() const;
  int GetSecN() const;
  uint GetAbsSecs() const {return AbsSecs();}
  TSecTm Round(const TTmUnit& TmUnit) const;
  uint GetInUnits(const TTmUnit& TmUnit) const;

  // additions/substractions
  TSecTm& AddSecs(const int& Secs){
    IAssert(IsDef()); AbsSecs.Val+=uint(Secs); return *this;}
  TSecTm& AddMins(const int& Mins){
    IAssert(IsDef()); AbsSecs.Val+=uint(Mins*60); return *this;}
  TSecTm& AddHours(const int& Hours){
    IAssert(IsDef()); AbsSecs.Val+=uint(Hours*3600); return *this;}
  TSecTm& AddDays(const int& Days){
    IAssert(IsDef()); AbsSecs.Val+=uint(Days*24*3600); return *this;}
  TSecTm& AddWeeks(const int& Weeks){
    IAssert(IsDef()); AbsSecs.Val+=uint(Weeks*7*24*3600); return *this;}
  static uint GetDSecs(const TSecTm& SecTm1, const TSecTm& SecTm2);
  /*friend TSecTm operator+(const TSecTm& SecTm, const uint& Secs){
    return TSecTm(SecTm)+=Secs;}
  friend TSecTm operator-(const TSecTm& SecTm, const uint& Secs){
    return TSecTm(SecTm)-=Secs;}
  friend TSecTm operator+(const TSecTm& SecTm1, const TSecTm& SecTm2){
    return TSecTm(SecTm1)+=SecTm2.AbsSecs;}
  friend TSecTm operator-(const TSecTm& SecTm1, const TSecTm& SecTm2){
    return TSecTm(SecTm1)-=SecTm2.AbsSecs;}*/

  // time construction
  static TSecTm GetZeroTm(){return TSecTm(0).AddHours(23);}
  static TSecTm GetZeroWeekTm();
  static TSecTm GetCurTm();
  static TSecTm GetCurDtTm(){return GetDtTm(GetCurTm());}
  static TSecTm GetDtTmFromHmsStr(const TStr& HmsStr);
  static TSecTm GetDtTmFromMdyStr(const TStr& MdyStr);
  static TSecTm GetDtTmFromDmyStr(const TStr& DmyStr);
  static TSecTm GetDtTmFromMdyHmsPmStr(const TStr& MdyHmsPmStr,
   const char& DateSepCh='/', const char& TimeSepCh=':');
  static TSecTm GetDtTmFromYmdHmsStr(const TStr& YmdHmsPmStr,
   const char& DateSepCh='-', const char& TimeSepCh=':');
  static TSecTm GetDtTmFromStr(const TChA& YmdHmsPmStr, const int& YearId=0, const int& MonId=1,
    const int& DayId=2, const int& HourId=3, const int& MinId=4, const int& SecId=5);
  static TSecTm GetDtTm(const int& YearN, const int& MonthN, const int& DayN);
  static TSecTm GetDtTm(const TSecTm& Tm);

  // text load/save
  static TSecTm LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;
};
typedef TVec<TSecTm> TSecTmV;
typedef TKeyDat<TSecTm, TStr> TSecTmStrKd;
typedef TVec<TSecTmStrKd> TSecTmStrKdV;

/////////////////////////////////////////////////
// Time
class TTm{
private:
  typedef struct { 
    unsigned int Min:6;
    unsigned int Hour:5;
    unsigned int Day:5;
    unsigned int Month:4;
    unsigned int Year:12;
  } TTmDateTimeBits;
  typedef union {
    TTmDateTimeBits Bits;
    int Int;
  } TTmDateTime;
private:
  TInt Year, Month, Day, DayOfWeek;
  TInt Hour, Min, Sec, MSec;
public:
  TTm():
    Year(-1), Month(-1), Day(-1), DayOfWeek(-1),
    Hour(-1), Min(-1), Sec(-1), MSec(-1){}
  TTm(const TTm& Tm):
    Year(Tm.Year), Month(Tm.Month), Day(Tm.Day), DayOfWeek(Tm.DayOfWeek),
    Hour(Tm.Hour), Min(Tm.Min), Sec(Tm.Sec), MSec(Tm.MSec){}
  TTm(
   const int& _Year, const int& _Month, const int& _Day, const int& _DayOfWeek=-1,
   const int& _Hour=0, const int& _Min=0, const int& _Sec=0, const int& _MSec=0):
    Year(_Year), Month(_Month), Day(_Day), DayOfWeek(_DayOfWeek),
    Hour(_Hour), Min(_Min), Sec(_Sec), MSec(_MSec){}
  TTm(const TSecTm& SecTm):
    Year(SecTm.GetYearN()), Month(SecTm.GetMonthN()), Day(SecTm.GetDayN()),
    DayOfWeek(SecTm.GetDayOfWeekN()), Hour(SecTm.GetHourN()),
    Min(SecTm.GetMinN()), Sec(SecTm.GetSecN()), MSec(0){}
  ~TTm(){}
  TTm(TSIn& SIn):
    Year(SIn), Month(SIn), Day(SIn), DayOfWeek(SIn),
    Hour(SIn), Min(SIn), Sec(SIn), MSec(SIn){}
  void Save(TSOut& SOut) const {
    Year.Save(SOut); Month.Save(SOut); Day.Save(SOut); DayOfWeek.Save(SOut);
    Hour.Save(SOut); Min.Save(SOut); Sec.Save(SOut); MSec.Save(SOut);}

  TTm& operator=(const TTm& Tm){
    Year=Tm.Year; Month=Tm.Month; Day=Tm.Day; DayOfWeek=Tm.DayOfWeek;
    Hour=Tm.Hour; Min=Tm.Min; Sec=Tm.Sec; MSec=Tm.MSec;
    return *this;}
  bool operator==(const TTm& Tm) const {
    return
     (Year==Tm.Year)&&(Month==Tm.Month)&&(Day==Tm.Day)&&
     (Hour==Tm.Hour)&&(Min==Tm.Min)&&(Sec==Tm.Sec)&&(MSec==Tm.MSec);}
  bool operator<(const TTm& Tm) const {
    return
     (Year<Tm.Year)||
     ((Year==Tm.Year)&&(Month<Tm.Month))||
     ((Year==Tm.Year)&&(Month==Tm.Month)&&(Day<Tm.Day))||
     (((Year==Tm.Year)&&(Month==Tm.Month)&&(Day==Tm.Day))&&(
      (Hour<Tm.Hour)||
      ((Hour==Tm.Hour)&&(Min<Tm.Min))||
      ((Hour==Tm.Hour)&&(Min==Tm.Min)&&(Sec<Tm.Sec))||
      ((Hour==Tm.Hour)&&(Min==Tm.Min)&&(Sec==Tm.Sec)&&(MSec<Tm.MSec))));} // saxo
  int GetMemUsed() const {return sizeof(TTm);}

  int GetPrimHashCd() const {return Year*Month*Day+Hour*Min*Sec*MSec;}
  int GetSecHashCd() const {return Year*Month*Day;}

  // defined
  bool IsDef() const {
   return (Year!=-1)&&(Month!=-1)&&(Day!=-1)&&
    (Hour!=-1)&&(Min!=-1)&&(Sec!=-1)&&(MSec!=-1);}
  void Undef(){
    Year=-1; Month=-1; Day=-1; DayOfWeek=-1;
    Hour=-1; Min=-1; Sec=-1; MSec=-1;}

  // get components
  int GetYear() const {return Year;}
  int GetMonth() const {return Month;}
  TStr GetMonthNm() const {return TTmInfo::GetMonthNm(Month);}
  int GetDay() const {return Day;}
  int GetDayOfWeek() const {return DayOfWeek;}
  TStr GetDayOfWeekNm() const {return TTmInfo::GetDayOfWeekNm(DayOfWeek);}
  int GetHour() const {return Hour;}
  int GetMin() const {return Min;}
  int GetSec() const {return Sec;}
  int GetMSec() const {return MSec;}

  // time string formats
  TStr GetStr(const bool& MSecP=true) const;
  TStr GetYMDDashStr() const;
  TStr GetHMSTColonDotStr(const bool& FullP=false, const bool& MSecP=true) const;
  TStr GetWebLogDateStr() const {return GetYMDDashStr();}
  TStr GetWebLogTimeStr() const {return GetHMSTColonDotStr(false);}
  TStr GetWebLogDateTimeStr(const bool& FullP=false) const {
    return GetYMDDashStr()+" "+GetHMSTColonDotStr(FullP);}
  TStr GetIdStr() const;
  TSecTm GetSecTm() const {
    return TSecTm(Year, Month, Day, Hour, Min, Sec);}

  // calculation
  void AddTime(const int& Hours, const int& Mins=0, const int& Secs=0, const int& MSecs=0);
  void AddDays(const int& Days){AddTime(Days*24);}
  void SubTime(const int& Hours, const int& Mins=0, const int& Secs=0, const int& MSecs=0);
  void SubDays(const int& Days){SubTime(Days*24);}

  // static functions
  static TTm GetCurUniTm();
  static TTm GetUniqueCurUniTm();
  static TTm GetUniqueCurUniTm(const int& UniqueSpaces, const int& UniqueSpaceN);
  static TTm GetCurLocTm();
  static uint64 GetCurUniMSecs();
  static uint64 GetCurLocMSecs();
  static uint64 GetMSecsFromTm(const TTm& Tm);
  static TTm GetTmFromMSecs(const uint64& MSecs);
  static uint GetMSecsFromOsStart();
  static uint64 GetPerfTimerFq();
  static uint64 GetPerfTimerTicks();
  static uint64 GetDiffMSecs(const TTm& Tm1, const TTm& Tm2);
  static uint64 GetDiffDays(const TTm& Tm1, const TTm& Tm2){
    return GetDiffMSecs(Tm1, Tm2)/uint64(1000*60*60*24);}
  static TTm GetLocTmFromUniTm(const TTm& Tm);
  static TTm GetUniTmFromLocTm(const TTm& Tm);
  static TTm GetTmFromWebLogDateTimeStr(const TStr& DateTimeStr,
   const char DateSepCh='-', const char TimeSepCh=':', const char MSecSepCh='.');
  static TTm GetTmFromIdStr(const TStr& IdStr);
  
  // unique sortable 32-bit integer from date and time (TTmDateTime)
  static uint GetDateTimeInt(const int& Year = 0, const int& Month = 1, 
    const int& Day = 1, const int& Hour = 0, const int& Min = 0);   
  static uint GetDateIntFromTm(const TTm& Tm);   
  static uint GetMonthIntFromTm(const TTm& Tm);
  static uint GetYearIntFromTm(const TTm& Tm);
  static uint GetDateTimeIntFromTm(const TTm& Tm);   
  static TTm GetTmFromDateTimeInt(const uint& DateTimeInt);
  static uint KeepMonthInDateTimeInt(const uint& DateTimeInt);
  static uint KeepDayInDateTimeInt(const uint& DateTimeInt);
  static uint KeepHourInDateTimeInt(const uint& DateTimeInt);
};
typedef TVec<TTm> TTmV;
typedef TPair<TTm, TStr> TTmStrPr;
typedef TVec<TTmStrPr> TTmStrPrV;

/////////////////////////////////////////////////
// Execution-Time
class TExeTm{
private:
  int LastTick;
public:
  TExeTm(): LastTick(0) { Tick(); }
  TExeTm(const TExeTm& Tm): LastTick(Tm.LastTick) { }
  TExeTm& operator=(const TExeTm& Tm){
    LastTick=Tm.LastTick; return *this;}

  void Tick(){LastTick=clock();}
  int GetTime(){return clock()-LastTick;}
  double GetSecs() const {return double(clock()-LastTick)/double(CLOCKS_PER_SEC);}
  const char* GetStr() const {return GetTmStr();}
  TStr GetStr2() const {return GetTmStr();}
  const char* GetTmStr() const { static char TmStr[32];
    if (GetSecs() < 60) { sprintf(TmStr, "%.2fs", GetSecs()); }
    else if (GetSecs() < 3600) { sprintf(TmStr, "%02dm%02ds", int(GetSecs())/60, int(GetSecs())%60); }
    else { sprintf(TmStr, "%02dh%02dm", int(GetSecs())/3600, (int(GetSecs())%3600)/60); }  return TmStr; }
  static char* GetCurTm(){ static TStr TmStr; TmStr=TSecTm::GetCurTm().GetTmStr(); return TmStr.CStr(); }
};

/////////////////////////////////////////////////
// Time-Stop-Watch
class TTmStopWatch {
private:
  int TmSoFar;
  bool RunningP;
  TExeTm ExeTm;
  UndefCopyAssign(TTmStopWatch);
public:
  TTmStopWatch(const bool& Start = false): TmSoFar(0), RunningP(Start) { }

  void Start() { if (!RunningP) { RunningP = true; ExeTm.Tick(); } }
  void Stop() { if (RunningP) { RunningP = false; TmSoFar += ExeTm.GetTime(); } }
  void Reset(const bool& Start) { TmSoFar = 0; RunningP = Start; ExeTm.Tick(); }

  int GetTime() { return TmSoFar + (RunningP ? ExeTm.GetTime() : 0); }
  double GetSec() { return double(GetTime()) / double(CLOCKS_PER_SEC); }
  int GetSecInt() { return TFlt::Round(GetSec()); }
};
