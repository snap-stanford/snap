#ifdef GLib_WIN

/////////////////////////////////////////////////
// System-Processes
void TSysProc::Sleep(const uint& MSecs){
  SleepEx(MSecs, false);
}

TStr TSysProc::GetExeFNm(){
  DWORD MxFNmLen=1024;
  LPTSTR FNmCStr=new char[MxFNmLen];
  DWORD FNmLen=GetModuleFileName(NULL, FNmCStr, MxFNmLen);
  TStr FNm;
  if (FNmLen!=0){
    FNm=FNmCStr;}
  delete[] FNmCStr;
  return FNm;
}

void TSysProc::SetLowPriority(){
  SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
}

bool TSysProc::ExeProc(const TStr& ExeFNm, TStr& ParamStr){
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb=sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Start the child process.
  BOOL Ok=CreateProcess(
   ExeFNm.CStr(),    // module name
   ParamStr.CStr(),  // patameters
   NULL,             // Process handle not inheritable.
   NULL,             // Thread handle not inheritable.
   FALSE,            // Set handle inheritance to FALSE.
   0,                // No creation flags.
   NULL,             // Use parent's environment block.
   NULL,             // Use parent's starting directory.
   &si,              // Pointer to STARTUPINFO structure.
   &pi);             // Pointer to PROCESS_INFORMATION structure.
  if (Ok){
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return true;
  } else {
    return false;
  }
}

/////////////////////////////////////////////////
// System-Messages
void TSysMsg::Quit(){PostQuitMessage(0);}

/////////////////////////////////////////////////
// Memory-Status
TStr TSysMemStat::GetLoadStr(){
  static TStr MemUsageStr="Mem Load: ";
  TChA ChA;
  ChA+=MemUsageStr;
  ChA+=TUInt64::GetStr(GetLoad());
  ChA+="%";
  return ChA;
}

TStr TSysMemStat::GetUsageStr(){
  static TStr MemUsageStr="Mem Usage: ";
  uint64 GlobalUsage=GetTotalPageFile()-GetAvailPageFile();
  TChA ChA;
  ChA+=MemUsageStr;
  ChA+=TUInt64::GetStr(GlobalUsage/1024);
  ChA+="K / ";
  ChA+=TUInt64::GetStr(GetTotalPageFile()/1024);
  ChA+="K";
  return ChA;
}

TStr TSysMemStat::GetInfoStr(){
  TChA ChA;
  ChA+="Memory Load:";
  ChA+=TUInt64::GetMegaStr(GetLoad()); ChA+="\r\n";
  ChA+="Total Physical:";
  ChA+=TUInt64::GetMegaStr(GetTotalPhys()); ChA+="\r\n";
  ChA+="Available Physical:";
  ChA+=TUInt64::GetMegaStr(GetAvailPhys()); ChA+="\r\n";
  ChA+="Total Page File:";
  ChA+=TUInt64::GetMegaStr(GetTotalPageFile()); ChA+="\r\n";
  ChA+="Available Page File:";
  ChA+=TUInt64::GetMegaStr(GetAvailPageFile()); ChA+="\r\n";
  ChA+="Total Virtual:";
  ChA+=TUInt64::GetMegaStr(GetTotalVirtual()); ChA+="\r\n";
  ChA+="Available Virtual:";
  ChA+=TUInt64::GetMegaStr(GetAvailVirtual()); ChA+="\r\n";
  return ChA;
}

TStr TSysMemStat::GetStr(){
  TChA ChA;
  ChA+=TUInt64::GetStr(GetLoad()); ChA+=' ';
  ChA+=TUInt64::GetStr(GetTotalPhys()); ChA+=' ';
  ChA+=TUInt64::GetStr(GetAvailPhys()); ChA+=' ';
  ChA+=TUInt64::GetStr(GetTotalPageFile()); ChA+=' ';
  ChA+=TUInt64::GetStr(GetAvailPageFile()); ChA+=' ';
  ChA+=TUInt64::GetStr(GetTotalVirtual()); ChA+=' ';
  ChA+=TUInt64::GetStr(GetAvailVirtual());
  return ChA;
}

/////////////////////////////////////////////////
// System-Console
TSysConsole::TSysConsole(){
  Ok=(AllocConsole()!=0);
  IAssert(Ok);
  hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
  IAssert(hStdOut!=INVALID_HANDLE_VALUE);
}

TSysConsole::~TSysConsole(){
  if (Ok){
    IAssert(FreeConsole());}
}

void TSysConsole::Put(const TStr& Str){
  DWORD ChsWritten;
  WriteConsole(hStdOut, Str.CStr(), Str.Len(), &ChsWritten, NULL);
  IAssert(ChsWritten==DWORD(Str.Len()));
}

/////////////////////////////////////////////////
// System-Console-Notifier
void TSysConsoleNotify::OnNotify(const TNotifyType& Type, const TStr& MsgStr){
  if (Type==ntInfo){
    SysConsole->PutLn(TStr::Fmt("%s", MsgStr.CStr()));
  } else {
    TStr TypeStr=TNotify::GetTypeStr(Type, false);
    SysConsole->PutLn(TStr::Fmt("%s: %s", TypeStr.CStr(), MsgStr.CStr()));
  }
}

void TSysConsoleNotify::OnStatus(const TStr& MsgStr){
  SysConsole->Put(MsgStr.CStr());
  // print '\n' if message not overlayed
  if ((!MsgStr.Empty())&&(MsgStr.LastCh()!='\r')){
    SysConsole->PutLn(""); }
}

/////////////////////////////////////////////////
// System-Messages
void TSysMsg::Loop(){
  MSG Msg;
  while (GetMessage(&Msg, NULL, 0, 0 )){
    TranslateMessage(&Msg); DispatchMessage(&Msg);}
}

/////////////////////////////////////////////////
// System-Time
TTm TSysTm::GetCurUniTm(){
  SYSTEMTIME SysTm;
  GetSystemTime(&SysTm);
  return TTm(SysTm.wYear, SysTm.wMonth, SysTm.wDay, SysTm.wDayOfWeek,
   SysTm.wHour, SysTm.wMinute, SysTm.wSecond, SysTm.wMilliseconds);
}

TTm TSysTm::GetCurLocTm(){
  SYSTEMTIME SysTm;
  GetLocalTime(&SysTm);
  return TTm(SysTm.wYear, SysTm.wMonth, SysTm.wDay, SysTm.wDayOfWeek,
   SysTm.wHour, SysTm.wMinute, SysTm.wSecond, SysTm.wMilliseconds);
}

uint64 TSysTm::GetCurUniMSecs(){
  SYSTEMTIME SysTm; FILETIME FileTm;
  GetSystemTime(&SysTm);
  IAssert(SystemTimeToFileTime(&SysTm, &FileTm));
  TUInt64 UInt64(uint(FileTm.dwHighDateTime), uint(FileTm.dwLowDateTime));
  return UInt64.Val/uint64(10000);
}

uint64 TSysTm::GetCurLocMSecs(){
  SYSTEMTIME SysTm; FILETIME FileTm;
  GetLocalTime(&SysTm);
  IAssert(SystemTimeToFileTime(&SysTm, &FileTm));
  TUInt64 UInt64(uint(FileTm.dwHighDateTime), uint(FileTm.dwLowDateTime));
  return UInt64.Val/uint64(10000);
}

uint64 TSysTm::GetMSecsFromTm(const TTm& Tm){
  SYSTEMTIME SysTm; FILETIME FileTm;
  SysTm.wYear=WORD(Tm.GetYear());
  SysTm.wMonth=WORD(Tm.GetMonth());
  SysTm.wDayOfWeek=WORD(Tm.GetDayOfWeek());
  SysTm.wDay=WORD(Tm.GetDay());
  SysTm.wHour=WORD(Tm.GetHour());
  SysTm.wMinute=WORD(Tm.GetMin());
  SysTm.wSecond=WORD(Tm.GetSec());
  SysTm.wMilliseconds=WORD(Tm.GetMSec());
  ESAssert(SystemTimeToFileTime(&SysTm, &FileTm));
  TUInt64 UInt64(uint(FileTm.dwHighDateTime), uint(FileTm.dwLowDateTime));
  return UInt64.Val/uint64(10000);
}

TTm TSysTm::GetTmFromMSecs(const uint64& MSecs){
  TUInt64 FileTmUnits(MSecs*uint64(10000));
  SYSTEMTIME SysTm; FILETIME FileTm;
  FileTm.dwHighDateTime=FileTmUnits.GetMsVal();
  FileTm.dwLowDateTime=FileTmUnits.GetLsVal();
  SAssert(FileTimeToSystemTime(&FileTm, &SysTm));
  return TTm(SysTm.wYear, SysTm.wMonth, SysTm.wDay, SysTm.wDayOfWeek,
   SysTm.wHour, SysTm.wMinute, SysTm.wSecond, SysTm.wMilliseconds);
}

uint TSysTm::GetMSecsFromOsStart(){
  return uint(GetTickCount());
}

TTm TSysTm::GetLocTmFromUniTm(const TTm& Tm){
  // get time-zone information
  TIME_ZONE_INFORMATION TzInf;
  GetTimeZoneInformation(&TzInf);
  // get system time
  SYSTEMTIME UniSysTm;
  UniSysTm.wYear=WORD(Tm.GetYear());
  UniSysTm.wMonth=WORD(Tm.GetMonth());
  UniSysTm.wDayOfWeek=WORD(Tm.GetDayOfWeek());
  UniSysTm.wDay=WORD(Tm.GetDay());
  UniSysTm.wHour=WORD(Tm.GetHour());
  UniSysTm.wMinute=WORD(Tm.GetMin());
  UniSysTm.wSecond=WORD(Tm.GetSec());
  UniSysTm.wMilliseconds=WORD(Tm.GetMSec());
  // convert system-time
  SYSTEMTIME LocSysTm;
  SystemTimeToTzSpecificLocalTime(&TzInf, &UniSysTm, &LocSysTm);
  // return local-time
  return TTm(LocSysTm.wYear, LocSysTm.wMonth, LocSysTm.wDay, LocSysTm.wDayOfWeek,
   LocSysTm.wHour, LocSysTm.wMinute, LocSysTm.wSecond, LocSysTm.wMilliseconds);
}

TTm TSysTm::GetUniTmFromLocTm(const TTm& Tm){
  // get time-zone information
  TIME_ZONE_INFORMATION TzInf;
  GetTimeZoneInformation(&TzInf);
  // get system time
  SYSTEMTIME LocSysTm;
  LocSysTm.wYear=WORD(Tm.GetYear());
  LocSysTm.wMonth=WORD(Tm.GetMonth());
  LocSysTm.wDayOfWeek=WORD(Tm.GetDayOfWeek());
  LocSysTm.wDay=WORD(Tm.GetDay());
  LocSysTm.wHour=WORD(Tm.GetHour());
  LocSysTm.wMinute=WORD(Tm.GetMin());
  LocSysTm.wSecond=WORD(Tm.GetSec());
  LocSysTm.wMilliseconds=WORD(Tm.GetMSec());
  // convert system-time
  SYSTEMTIME UniSysTm=LocSysTm;
  Fail; // BCB5.0 doesn't find TzSpecificLocalTimeToSystemTime
  //TzSpecificLocalTimeToSystemTime(&TzInf, &LocSysTm, &UniSysTm);
  // return system-time
  return TTm(UniSysTm.wYear, UniSysTm.wMonth, UniSysTm.wDay, UniSysTm.wDayOfWeek,
   UniSysTm.wHour, UniSysTm.wMinute, UniSysTm.wSecond, UniSysTm.wMilliseconds);
}

uint64 TSysTm::GetProcessMSecs(){
  FILETIME CreationTime, ExitTime, KernelTime, UserTime;
  IAssert(GetProcessTimes(GetCurrentProcess(),
   &CreationTime, &ExitTime, &KernelTime, &UserTime));
  TUInt64 KernelMSecs(uint(KernelTime.dwHighDateTime), uint(KernelTime.dwLowDateTime));
  TUInt64 UserMSecs(uint(UserTime.dwHighDateTime), uint(UserTime.dwLowDateTime));
  uint64 ProcessMSecs=KernelMSecs+UserMSecs;
  return ProcessMSecs;
}

uint64 TSysTm::GetThreadMSecs(){
  FILETIME CreationTime, ExitTime, KernelTime, UserTime;
  IAssert(GetProcessTimes(GetCurrentProcess(),
   &CreationTime, &ExitTime, &KernelTime, &UserTime));
  TUInt64 KernelMSecs(uint(KernelTime.dwHighDateTime), uint(KernelTime.dwLowDateTime));
  TUInt64 UserMSecs(uint(UserTime.dwHighDateTime), uint(UserTime.dwLowDateTime));
  uint64 ThreadMSecs=KernelMSecs+UserMSecs;
  return ThreadMSecs;
}

uint64 TSysTm::GetPerfTimerFq(){
  uint MsFq; uint LsFq;
  LARGE_INTEGER LargeInt;
  if (QueryPerformanceFrequency(&LargeInt)){
    MsFq=LargeInt.u.HighPart;
    LsFq=LargeInt.u.LowPart;
  } else {
    MsFq=0;
    LsFq=1;
  }
  TUInt64 UInt64(MsFq, LsFq);
  return UInt64.Val;
}

uint64 TSysTm::GetPerfTimerTicks(){
  uint MsVal; uint LsVal;
  LARGE_INTEGER LargeInt;
  if (QueryPerformanceCounter(&LargeInt)){
    MsVal=LargeInt.u.HighPart;
    LsVal=LargeInt.u.LowPart;
  } else {
    MsVal=0;
    LsVal=int(time(NULL));
  }
  TUInt64 UInt64(MsVal, LsVal);
  return UInt64.Val;
}

/////////////////////////////////////////////////
// System-Strings
TStr TSysStr::GetCmLn(){
  return TStr((char*)GetCommandLine());
}

TStr TSysStr::GetMsgStr(const DWORD& MsgCd){
  // retrieve message string
  LPVOID lpMsgBuf;
  FormatMessage(
   FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
   NULL,
   MsgCd,
   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
   (LPTSTR) &lpMsgBuf,
   0,
   NULL);
  // save string
  TStr MsgStr((char*)lpMsgBuf);
  // free the buffer.
  LocalFree(lpMsgBuf);
  return MsgStr;
}

char* TSysStr::GetLastMsgCStr(){
  TStr MsgStr=GetLastMsgStr();
  static char* MsgCStr=NULL;
  if (MsgCStr==NULL){MsgCStr=new char[1000];}
  strcpy(MsgCStr, MsgStr.CStr());
  return MsgCStr;
}

/////////////////////////////////////////////////
// Registry-Key
PRegKey TRegKey::GetKey(const PRegKey& BaseKey, const TStr& SubKeyNm){
  HKEY hKey;
  DWORD RetCd=RegOpenKeyEx(
   BaseKey->GetHandle(), SubKeyNm.CStr(), 0, KEY_ALL_ACCESS, &hKey);
  bool Ok=RetCd==ERROR_SUCCESS;
  return new TRegKey(Ok, hKey);
}

TStr TRegKey::GetVal(const PRegKey& Key, const TStr& SubKeyNm, const TStr& ValNm){
  PRegKey RegKey=TRegKey::GetKey(Key, SubKeyNm);
  if (RegKey->IsOk()){
    TStrKdV ValNmStrKdV; RegKey->GetValV(ValNmStrKdV);
    int ValN;
    if (ValNmStrKdV.IsIn(TStrKd(ValNm), ValN)){
      return ValNmStrKdV[ValN].Dat;
    } else {
      return "";
    }
  } else {
    return "";
  }
}

void TRegKey::GetKeyNmV(TStrV& KeyNmV) const {
  KeyNmV.Clr();
  if (!Ok){return;}
  // get subkey count
  DWORD SubKeys; // number of subkeys
  DWORD MxSubKeyNmLen; // longest subkey size
  DWORD RetCd=RegQueryInfoKey(
   hKey, // key handle
   NULL, // buffer for class name
   NULL, // length of class string
   NULL, // reserved
   &SubKeys, // number of subkeys
   &MxSubKeyNmLen, // longest subkey size
   NULL, // longest class string
   NULL, // number of values for this key
   NULL, // longest value name
   NULL, // longest value data
   NULL, // security descriptor
   NULL); // last write time
   if (RetCd!=ERROR_SUCCESS){return;}

  // retrieve subkey-names
  if (SubKeys>0){
    KeyNmV.Gen(SubKeys, 0);
    char* SubKeyNmCStr=new char[MxSubKeyNmLen+1];
    DWORD SubKeyN=0;
    forever{
      DWORD SubKeyNmCStrLen=MxSubKeyNmLen+1;
      DWORD RetCd=RegEnumKeyEx(
       hKey, // handle of key to enumerate
       SubKeyN, // index of subkey to enumerate
       SubKeyNmCStr, // address of buffer for subkey name
       &SubKeyNmCStrLen, // address for size of subkey buffer
       NULL, // reserved
       NULL, // address of buffer for class string
       NULL, // address for size of class buffer
       NULL); // address for time key last written to
      if (RetCd==ERROR_SUCCESS){
        TStr KeyNm(SubKeyNmCStr);
        KeyNmV.Add(KeyNm);
      } else {
        break;
      }
      SubKeyN++;
    }
    delete[] SubKeyNmCStr;
  }
}

void TRegKey::GetValV(TStrKdV& ValNmStrKdV) const {
  ValNmStrKdV.Clr();
  if (!Ok){return;}
  // get subkey count
  DWORD Vals; // number of values
  DWORD MxValNmLen; // longest value name
  DWORD MxValStrLen; // longest value data
  DWORD RetCd=RegQueryInfoKey(
   hKey, // key handle
   NULL, // buffer for class name
   NULL, // length of class string
   NULL, // reserved
   NULL, // number of subkeys
   NULL, // longest subkey size
   NULL, // longest class string
   &Vals, // number of values for this key
   &MxValNmLen, // longest value name
   &MxValStrLen, // longest value data
   NULL, // security descriptor
   NULL); // last write time
   if (RetCd!=ERROR_SUCCESS){return;}

  // retrieve subkey-names
  if (Vals>0){
    ValNmStrKdV.Gen(Vals, 0);
    char* ValNmCStr=new char[MxValNmLen+1];
    char* ValCStr=new char[MxValStrLen+1];
    DWORD ValN=0;
    forever{
      DWORD ValNmCStrLen=MxValNmLen+1;
      DWORD ValCStrLen=MxValStrLen+1;
      DWORD ValType;
      DWORD RetCd=RegEnumValue(
       hKey, // handle of key to query
       ValN, // index of value to query
       ValNmCStr, // address of buffer for value string
       &ValNmCStrLen, // address for size of value buffer
       NULL, // reserved
       &ValType, // address of buffer for type code
       (unsigned char*) ValCStr, // address of buffer for value data
       &ValCStrLen); // address for size of data buffer
      if (RetCd==ERROR_SUCCESS){
        if (ValType==REG_SZ){
          TStr ValNm(ValNmCStr);
          TStr ValStr(ValCStr);
          ValNmStrKdV.Add(TStrKd(ValNm, ValStr));
        }
      } else {
        break;
      }
      ValN++;
    }
    delete[] ValNmCStr;
    delete[] ValCStr;
  }
}

/////////////////////////////////////////////////
// Program StdIn and StdOut redirection using pipes
void TStdIOPipe::CreateProc(const TStr& Cmd) {
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdInput = ChildStdinRd;
  siStartInfo.hStdOutput = ChildStdoutWr;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  // Create the child process.
  const BOOL FuncRetn = CreateProcess(NULL,
    (LPSTR) Cmd.CStr(),  // command line
    NULL,          // process security attributes
    NULL,          // primary thread security attributes
    TRUE,          // handles are inherited
    0,             // creation flags
    NULL,          // use parent's environment
    NULL,          // use parent's current directory
    &siStartInfo,  // STARTUPINFO pointer
    &piProcInfo);  // receives PROCESS_INFORMATION
  EAssertR(FuncRetn!=0, TStr::Fmt("Can not execute '%s'", Cmd.CStr()).CStr());
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);
}

TStdIOPipe::TStdIOPipe(const TStr& CmdToExe) : ChildStdinRd(NULL), ChildStdinWrDup(NULL),
  ChildStdoutWr(NULL), ChildStdoutRdDup(NULL) {
  HANDLE ChildStdinWr, ChildStdoutRd;
  SECURITY_ATTRIBUTES saAttr;
  // Set the bInheritHandle flag so pipe handles are inherited.
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  // Create a pipe for the child process's STDOUT.
  EAssert(CreatePipe(&ChildStdoutRd, &ChildStdoutWr, &saAttr, 0));
  // Create noninheritable read handle and close the inheritable read handle.
  EAssert(DuplicateHandle(GetCurrentProcess(), ChildStdoutRd,
    GetCurrentProcess(), &ChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS));
  CloseHandle(ChildStdoutRd);
  // Create a pipe for the child process's STDIN.
  EAssert(CreatePipe(&ChildStdinRd, &ChildStdinWr, &saAttr, 0));
  // Duplicate the write handle to the pipe so it is not inherited.
  EAssert(DuplicateHandle(GetCurrentProcess(), ChildStdinWr,
    GetCurrentProcess(), &ChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS));
  CloseHandle(ChildStdinWr);
  // Now create the child process.
  CreateProc(CmdToExe);
}

TStdIOPipe::~TStdIOPipe() {
  if (ChildStdinRd != NULL) CloseHandle(ChildStdinRd);
  if (ChildStdinWrDup != NULL) CloseHandle(ChildStdinWrDup);
  if (ChildStdoutWr != NULL) CloseHandle(ChildStdoutWr);
  if (ChildStdoutRdDup != NULL) CloseHandle(ChildStdoutRdDup);
}

int TStdIOPipe::Write(const char* Bf, const int& BfLen) {
  DWORD Written;
  EAssert(WriteFile(ChildStdinWrDup, Bf, BfLen, &Written, NULL));
  return int(Written);
}

int TStdIOPipe::Read(char *Bf, const int& BfMxLen) {
  DWORD Read;
  EAssert(ReadFile(ChildStdoutRdDup, Bf, BfMxLen, &Read, NULL));
  return int(Read);
}

#elif defined(GLib_UNIX)
/////////////////////////////////////////////////
// Compatibility functions
int GetModuleFileName(void *hModule, char *Bf, int MxBfL) {
  int retlen = readlink("/proc/self/exe", Bf, MxBfL);
  if (retlen == -1) {
    if (MxBfL > 0) Bf[0] = '\0';
    return 0;
  }
  if (retlen == MxBfL) --retlen;
  Bf[retlen] = '\0';
  return retlen;
}

int GetCurrentDirectory(const int MxBfL, char *Bf) {
  getcwd(Bf, MxBfL);
  return strlen(Bf);
}

int CreateDirectory(const char *FNm, void *useless) {
  return mkdir(FNm, 0777)==0;
}

int RemoveDirectory(const char *FNm) {
  return unlink(FNm)==0;
}

/////////////////////////////////////////////////
// System-Time
TTm TSysTm::GetCurUniTm(){
  time_t t;
  struct tm tms;
  struct timeval tv;

  time(&t);
  int ErrCd = gettimeofday(&tv, NULL);
  Assert((ErrCd==0)&&(t!=-1));
  gmtime_r(&t, &tms);

  return TTm(1900+tms.tm_year, tms.tm_mon, tms.tm_mday, tms.tm_wday,
   tms.tm_hour, tms.tm_min, tms.tm_sec, tv.tv_usec/1000);
}

TTm TSysTm::GetCurLocTm(){
  time_t t;
  struct tm tms;
  struct timeval tv;

  time(&t);
  int ErrCd = gettimeofday(&tv, NULL);
  Assert((ErrCd==0)&&(t!=-1));
  localtime_r(&t, &tms);

  return TTm(1900+tms.tm_year, tms.tm_mon, tms.tm_mday, tms.tm_wday,
   tms.tm_hour, tms.tm_min, tms.tm_sec, tv.tv_usec/1000);
}

uint64 TSysTm::GetCurUniMSecs(){
  return TTm::GetMSecsFromTm(GetCurLocTm());
}

uint64 TSysTm::GetCurLocMSecs(){
  return TTm::GetMSecsFromTm(GetCurUniTm());
}

// !bn: timezone needs to be correctly loaded first...
uint64 TSysTm::GetMSecsFromTm(const TTm& Tm){
  time_t t;
  struct tm tms;
  tms.tm_year = Tm.GetYear() - 1900;
  tms.tm_mon = Tm.GetMonth();
  tms.tm_mday = Tm.GetDay();
  tms.tm_hour = Tm.GetHour();
  tms.tm_min = Tm.GetMin();
  tms.tm_sec = Tm.GetSec();
  t = mktime(&tms);

  t -= timezone;
  FailR("BUG!: miliseconds start Jan 1 1601 (and not Jan 1 1970)!!!"); //J:BUG

  return (1000*(uint64)t) + (uint64)Tm.GetMSec();
}

TTm TSysTm::GetTmFromMSecs(const uint64& TmNum){
  int MSec = TmNum % 1000;
  time_t Sec = (TmNum / 1000)-(1970-1601)*365*3600*24;

  FailR("BUG!: miliseconds start Jan 1 1601 (and not Jan 1 1970)!!!"); //J:BUG
  struct tm tms;
  gmtime_r(&Sec, &tms);

  return TTm(1900+tms.tm_year, tms.tm_mon, tms.tm_mday, tms.tm_wday,
   tms.tm_hour, tms.tm_min, tms.tm_sec, MSec);
}

// !bn: timezone needs to be correctly loaded first...
TTm TSysTm::GetLocTmFromUniTm(const TTm& Tm) {
  struct tm tms, tmr;

  tms.tm_year = Tm.GetYear() - 1900;
  tms.tm_mon = Tm.GetMonth();
  tms.tm_mday = Tm.GetDay();
  tms.tm_hour = Tm.GetHour();
  tms.tm_min = Tm.GetMin();
  tms.tm_sec = Tm.GetSec();
  int MSec = Tm.GetMSec();

  time_t Sec = mktime(&tms);
  Sec -= timezone;
  localtime_r(&Sec, &tmr);

  return TTm(1900+tmr.tm_year, tmr.tm_mon, tmr.tm_mday, tmr.tm_wday,
   tmr.tm_hour, tmr.tm_min, tmr.tm_sec, MSec);
}

TTm TSysTm::GetUniTmFromLocTm(const TTm& Tm) {
  struct tm tms, tmr;

  tms.tm_year = Tm.GetYear() - 1900;
  tms.tm_mon = Tm.GetMonth();
  tms.tm_mday = Tm.GetDay();
  tms.tm_hour = Tm.GetHour();
  tms.tm_min = Tm.GetMin();
  tms.tm_sec = Tm.GetSec();
  int MSec = Tm.GetMSec();

  time_t Sec = mktime(&tms);
  gmtime_r(&Sec, &tmr);

  return TTm(1900+tmr.tm_year, tmr.tm_mon, tmr.tm_mday, tmr.tm_wday,
   tmr.tm_hour, tmr.tm_min, tmr.tm_sec, MSec);
}

uint TSysTm::GetMSecsFromOsStart(){
#if defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK != -1)
  struct timespec ts;
  int ErrCd=clock_gettime(CLOCK_MONOTONIC, &ts);
  Assert(ErrCd==0);
  return (ts.tv_sec*1000) + (ts.tv_nsec/1000000);
#else
  FILE *f;
  uint sec, csec;
  f = fopen("/proc/uptime", "r");
  if (!f) return 0xffffffff;    // !bn: assert
  fscanf(f, "%u.%u", &sec, &csec);
  fclose(f);
  return (uint) (sec * 1000) + (csec * 10);
#endif
}

uint64 TSysTm::GetProcessMSecs() {
#if defined(_POSIX_CPUTIME) && (_POSIX_CPUTIME != -1)
  struct timespec ts;
  int ErrCd=clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
  Assert(ErrCd==0);
  return (ts.tv_sec*1000) + (ts.tv_nsec / 1000000);
#else
  //#warning "CLOCK_PROCESS_CPUTIME not available; using getrusage"
  struct rusage ru;
  int ErrCd = getrusage(RUSAGE_SELF, &ru);
  Assert(ErrCd == 0);
  return ((ru.ru_utime.tv_usec + ru.ru_stime.tv_usec) / 1000) +
         ((ru.ru_utime.tv_sec + ru.ru_stime.tv_sec) * 1000);
#endif
}

uint64 TSysTm::GetThreadMSecs() {
#if defined(_POSIX_THREAD_CPUTIME) && (_POSIX_THREAD_CPUTIME != -1)
  struct timespec ts;
  int ErrCd=clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
  Assert(ErrCd==0);
  return (ts.tv_sec*1000) + (ts.tv_nsec / 1000000);
#else
  //#warning "CLOCK_THREAD_CPUTIME not available; using GetProcessMSecs()"
  return GetProcessMSecs();
#endif
}

uint64 TSysTm::GetPerfTimerFq(){
#if defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK != -1)
  return 1000000000;
#else
  return 1000000;
#endif
}

uint64 TSysTm::GetPerfTimerTicks(){
#if defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK != -1)
  struct timespec ts;
  int ErrCd=clock_gettime(CLOCK_MONOTONIC, &ts);
  //Assert(ErrCd==0); //J: vcasih se prevede in ne dela
  if (ErrCd != 0) {
    return (uint64)ts.tv_sec*1000000000ll + (uint64)ts.tv_nsec; }
  else {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64)tv.tv_usec + ((uint64)tv.tv_sec)*1000000;
  }
#else
  //#warning "CLOCK_MONOTONIC not available; using gettimeofday()"
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64)tv.tv_usec + ((uint64)tv.tv_sec)*1000000;
#endif
}

/////////////////////////////////////////////////
// System-Processes
void TSysProc::Sleep(const uint& MSecs) {
  struct timespec tsp, trem;
  tsp.tv_sec = MSecs / 1000;
  tsp.tv_nsec = (MSecs % 1000) * 1000000;

  while (true) {
#if defined(GLib_POSIX_1j) && defined(_POSIX_MONOTONIC_CLOCK)
    //int ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &tsp, &trem); //J: troubles compiling
    //if ((ret == -1) && (errno == ENOTSUP)) {
    int ret = nanosleep(&tsp, &trem);
    //}
#else
    int ret = nanosleep(&tsp, &trem);
#endif
    if ((ret == -1) && (errno == EINTR)) {
      tsp = trem;
    } else break;
  }
}

TStr TSysProc::GetExeFNm() {
  char Bf[1024];
  GetModuleFileName(NULL, Bf, 1023);
  return TStr(Bf);
}

void TSysProc::SetLowPriority() {
  nice(19);
}

bool TSysProc::ExeProc(const TStr& ExeFNm, TStr& ParamStr) {
  TStrV SArgV;
  ParamStr.SplitOnWs(SArgV);

  int pid = fork();
  if (pid == -1) return false;
  if (pid > 0) return true;

  char **argv;
  argv = new char*[SArgV.Len()+2];
  argv[0] = strdup(ExeFNm.CStr());
  for (int i=0;i<SArgV.Len();i++) argv[i+1] = strdup(SArgV[i].CStr());
  argv[SArgV.Len()+1] = NULL;

  execvp(argv[0], argv);

  Fail;
  return false;
}

/////////////////////////////////////////////////
// System-Messages
void TSysMsg::Loop() {
    //bn!!! zdej mamo pa problem. kaksne msgje? samo za sockete?
    //!!! tle je treba klicat AsyncSys iz net::sock.cpp
    //#define TOTALNAZMEDA
    //#ifdef TOTALNAZMEDA
    //class TAsyncSys;
    //extern TAsyncSys AsyncSys;
    //AsyncSys::AsyncLoop();
    //#endif
  FailR("Not intended for use under Linux!");
}

void TSysMsg::Quit() {
  kill(getpid(), SIGINT);
}

/////////////////////////////////////////////////
// Program StdIn and StdOut redirection using pipes
// J: not yet ported to Linux
TStdIOPipe::TStdIOPipe(const TStr& CmdToExe) {
  FailR("Not intended for use under Linux!");
}

TStdIOPipe::~TStdIOPipe() {
  FailR("Not intended for use under Linux!");
}

int TStdIOPipe::Write(const char* Bf, const int& BfLen) {
  FailR("Not intended for use under Linux!");
  return -1;
}

int TStdIOPipe::Read(char *Bf, const int& BfMxLen) {
  FailR("Not intended for use under Linux!");
  return -1;
}

#endif
