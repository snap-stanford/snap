#ifdef GLib_WIN
/////////////////////////////////////////////////
// Com
class TCom{
public:
  TCom(){
    /*EAssertR(::CoInitialize(NULL)==S_OK, "COM initialization failed.");*/}
  ~TCom(){/*CoUninitialize();*/}
};

/////////////////////////////////////////////////
// System-Processes
class TSysProc{
public:
  static void Sleep(const uint& MSecs);
  static TStr GetExeFNm();
  static void SetLowPriority();
  static bool ExeProc(const TStr& ExeFNm, TStr& ParamStr);
};

/////////////////////////////////////////////////
// System-Memory-Status
ClassTP(TSysMemStat, PSysMemStat)//{
private:
  MEMORYSTATUSEX MemStat;
public:
  TSysMemStat(){Refresh();}
  ~TSysMemStat(){}
  TSysMemStat(TSIn&){Fail;}
  static TPt<TSysMemStat> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSysMemStat& operator=(const TSysMemStat&){Fail; return *this;}

  void Refresh(){
    MemStat.dwLength=sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&MemStat);}

  uint64 GetLoad(){ // percent of memory in use
    return uint64(MemStat.dwMemoryLoad);}
  uint64 GetTotalPhys(){ // bytes of physical memory
    return uint64(MemStat.ullTotalPhys);}
  uint64 GetAvailPhys(){ // free physical memory bytes
    return uint64(MemStat.ullAvailPhys);}
  uint64 GetTotalPageFile(){ // bytes of paging file
    return uint64(MemStat.ullTotalPageFile);}
  uint64 GetAvailPageFile(){ // free bytes of paging file
    return uint64(MemStat.ullAvailPageFile);}
  uint64 GetTotalVirtual(){ // user bytes of address space
    return uint64(MemStat.ullTotalVirtual);}
  uint64 GetAvailVirtual(){ // free user bytes
    return uint64(MemStat.ullAvailVirtual);}

  TStr GetLoadStr();
  TStr GetUsageStr();
  TStr GetInfoStr();
  TStr GetStr();

  static bool Is32Bit(){return sizeof(char*)==4;}
  static bool Is64Bit(){return sizeof(char*)==8;}
};

/////////////////////////////////////////////////
// System-Console
ClassTP(TSysConsole, PSysConsole)//{
private:
  bool Ok;
  HANDLE hStdOut;
public:
  TSysConsole();
  static PSysConsole New(){return PSysConsole(new TSysConsole());}
  ~TSysConsole();
  TSysConsole(TSIn&){Fail;}
  static PSysConsole Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSysConsole& operator=(const TSysConsole&){Fail; return *this;}

  void Put(const TStr& Str);
  void PutLn(const TStr& Str){
    Put(Str); Put("\r\n");}

  static void OpenFile(const TStr& FNm){
    ShellExecute(0, "open", FNm.CStr(), "", "", SW_SHOWNORMAL);}
};

/////////////////////////////////////////////////
// System-Console-Notifier
class TSysConsoleNotify: public TNotify{
private:
  PSysConsole SysConsole;
public:
  TSysConsoleNotify(const PSysConsole& _SysConsole):
    SysConsole(_SysConsole){}
  static PNotify New(const PSysConsole& _SysConsole){
    return PNotify(new TSysConsoleNotify(_SysConsole));}

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr);
  void OnStatus(const TStr& MsgStr);
};

/////////////////////////////////////////////////
// System-Messages
class TSysMsg{
public:
  static void Loop();
  static void Quit();
};

/////////////////////////////////////////////////
// System-Time
class TSysTm{
public:
  static TTm GetCurUniTm();
  static TTm GetCurLocTm();
  static uint64 GetCurUniMSecs();
  static uint64 GetCurLocMSecs();
  static uint64 GetMSecsFromTm(const TTm& Tm);
  static TTm GetTmFromMSecs(const uint64& MSecs);
  static uint GetMSecsFromOsStart();

  static TTm GetLocTmFromUniTm(const TTm& Tm);
  static TTm GetUniTmFromLocTm(const TTm& Tm);

  static uint64 GetProcessMSecs();
  static uint64 GetThreadMSecs();

  static uint64 GetPerfTimerFq();
  static uint64 GetPerfTimerTicks();
};

/////////////////////////////////////////////////
// System-Strings
class TSysStr{
public:
  static TStr GetCmLn();
  static TStr GetMsgStr(const DWORD& MsgCd);
  static TStr GetLastMsgStr(){return GetMsgStr(GetLastError());}
  static char* GetLastMsgCStr();

  static BSTR NewBStr(const TStr& Str){
    return SysAllocStringByteLen(Str.CStr(), Str.Len());}
  static TStr DelBStr(BSTR& BStr){
    TStr Str=(char*)BStr; SysFreeString(BStr); BStr=NULL; return Str;}
};

/////////////////////////////////////////////////
// Registry-Key
ClassTP(TRegKey, PRegKey)//{
private:
  bool Ok;
  HKEY hKey;
  void UndefCopyAssgin(TRegKey);
private:
  TRegKey(): Ok(false), hKey(0){}
  TRegKey(const bool& _Ok, const HKEY& _hKey): Ok(_Ok), hKey(_hKey){}
  ~TRegKey(){if (Ok){RegCloseKey(hKey);}}

  HKEY GetHandle() const {return hKey;}
public:
  static PRegKey GetKey(const PRegKey& Key, const TStr& SubKeyNm);
  static TStr GetVal(const PRegKey& Key, const TStr& SubKeyNm, const TStr& ValNm);
  static PRegKey GetClassesRootKey(){return new TRegKey(true, HKEY_CLASSES_ROOT);}
  static PRegKey GetCurrentUserKey(){return new TRegKey(true, HKEY_CURRENT_USER);}
  static PRegKey GetLocalMachineKey(){return new TRegKey(true, HKEY_LOCAL_MACHINE);}
  static PRegKey GetUsersKey(){return new TRegKey(true, HKEY_USERS);}

  bool IsOk() const {return Ok;}
  void GetKeyNmV(TStrV& KeyNmV) const;
  void GetValV(TStrKdV& ValNmStrKdV) const;
};

/////////////////////////////////////////////////
// Program StdIn and StdOut redirection using pipes
class TStdIOPipe {
private:
  HANDLE ChildStdinRd, ChildStdinWrDup;
  HANDLE ChildStdoutWr, ChildStdoutRdDup;
  void CreateProc(const TStr& Cmd);
private:
  UndefDefaultCopyAssign(TStdIOPipe);
public:
  TStdIOPipe(const TStr& CmdToExe);
  ~TStdIOPipe();

  int Write(const char* Bf) { return Write(Bf, (int) strlen(Bf)); }
  int Write(const char* Bf, const int& BfLen);
  int Read(char *Bf, const int& BfMxLen);
};

#elif defined(GLib_UNIX)

/////////////////////////////////////////////////
// Compatibility functions

int GetModuleFileName(void *hModule, char *Bf, int MxBfL);
int GetCurrentDirectory(const int MxBfL, char *Bf);
int CreateDirectory(const char *FNm, void *useless);
int RemoveDirectory(const char *FNm);

/////////////////////////////////////////////////
// System-Processes
class TSysProc{
public:
  static void Sleep(const uint& MSecs);
  static TStr GetExeFNm();
  static void SetLowPriority();
  static bool ExeProc(const TStr& ExeFNm, TStr& ParamStr);
};

/////////////////////////////////////////////////
// System-Messages
class TSysMsg{
public:
  static void Loop();
  static void Quit();
};

/////////////////////////////////////////////////
// System-Time
class TSysTm{
public:
  static TTm GetCurUniTm();
  static TTm GetCurLocTm();
  static uint64 GetCurUniMSecs();
  static uint64 GetCurLocMSecs();
  static uint64 GetMSecsFromTm(const TTm& Tm);
  static TTm GetTmFromMSecs(const uint64& MSecs);
  static uint GetMSecsFromOsStart();

  static TTm GetLocTmFromUniTm(const TTm& Tm);
  static TTm GetUniTmFromLocTm(const TTm& Tm);

  static uint64 GetProcessMSecs();
  static uint64 GetThreadMSecs();

  static uint64 GetPerfTimerFq();
  static uint64 GetPerfTimerTicks();
};

/////////////////////////////////////////////////
// Program StdIn and StdOut redirection using pipes
// J: not yet ported to Linux
class TStdIOPipe {
private:
  int ChildStdinRd, ChildStdinWrDup;
  int ChildStdoutWr, ChildStdoutRdDup;
  void CreateProc(const TStr& Cmd);
private:
  UndefDefaultCopyAssign(TStdIOPipe);
public:
  TStdIOPipe(const TStr& CmdToExe);
  ~TStdIOPipe();

  int Write(const char* Bf) { return Write(Bf, (int) strlen(Bf)); }
  int Write(const char* Bf, const int& BfLen);
  int Read(char *Bf, const int& BfMxLen);
};
#endif
