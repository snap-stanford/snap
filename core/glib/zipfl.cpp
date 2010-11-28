/////////////////////////////////////////////////
// Includes
//#include "zipfl.h"

/////////////////////////////////////////////////
// ZIP Input-File
TStrStrH TZipIn::FExtToCmdH;
const int TZipIn::MxBfL=32*1024;

void TZipIn::CreateZipProcess(const TStr& Cmd, const TStr& ZipFNm) {
  const TStr CmdLine = TStr::Fmt("%s %s", Cmd.CStr(), ZipFNm.CStr());
  #ifdef GLib_WIN
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdOutput = ZipStdoutWr;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  // Create the child process.
  const BOOL FuncRetn = CreateProcess(NULL,
    (LPSTR) CmdLine.CStr(),  // command line
    NULL,          // process security attributes
    NULL,          // primary thread security attributes
    TRUE,          // handles are inherited
    0,             // creation flags
    NULL,          // use parent's environment
    NULL,          // use parent's current directory
    &siStartInfo,  // STARTUPINFO pointer
    &piProcInfo);  // receives PROCESS_INFORMATION
  EAssertR(FuncRetn!=0, TStr::Fmt("Can not execute '%s'", CmdLine.CStr()).CStr());
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);
  #else
  ZipStdoutRd = popen(CmdLine.CStr(), "r");
  EAssertR(ZipStdoutRd != NULL,  TStr::Fmt("Can not execute '%s'", CmdLine.CStr()).CStr());
  #endif
}

void TZipIn::FillBf(){
  EAssertR(CurFPos < FLen, "End of file "+GetSNm()+" reached.");
  EAssertR((BfC==BfL)/*&&((BfL==-1)||(BfL==MxBfL))*/, "Error reading file '"+GetSNm()+"'.");
  #ifdef GLib_WIN
  // Read output from the child process
  DWORD BytesRead;
  EAssert(ReadFile(ZipStdoutRd, Bf, MxBfL, &BytesRead, NULL) != 0);
  #else
  size_t BytesRead = fread(Bf, 1, MxBfL, ZipStdoutRd);
  EAssert(BytesRead != 0);
  #endif
  BfL = (int) BytesRead;
  CurFPos += BytesRead;
  EAssertR((BfC!=0)||(BfL!=0), "Error reading file '"+GetSNm()+"'.");
  BfC = 0;
}

TZipIn::TZipIn(const TStr& FNm) : TSBase(FNm.CStr()), TSIn(FNm), ZipStdoutRd(NULL), ZipStdoutWr(NULL),
  FLen(0), CurFPos(0), Bf(NULL), BfC(0), BfL(0) {
  EAssertR(! FNm.Empty(), "Empty file-name.");
  EAssertR(TFile::Exists(FNm), TStr::Fmt("File %s does not exist", FNm.CStr()).CStr());
  FLen = TZipIn::GetFLen(FNm);
  if (FLen == 0) { return; } // empty file
  #ifdef GLib_WIN
  // create pipes
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
    // Create a pipe for the child process's STDOUT.
  const int PipeBufferSz = 32*1024;
  EAssertR(CreatePipe(&ZipStdoutRd, &ZipStdoutWr, &saAttr, PipeBufferSz), "Stdout pipe creation failed");
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  SetHandleInformation(ZipStdoutRd, HANDLE_FLAG_INHERIT, 0);
  #else
  // no implementation needed
  #endif
  CreateZipProcess(GetCmd(FNm), FNm);
  Bf = new char[MxBfL]; BfC = BfL=-1;
  FillBf();
}

TZipIn::TZipIn(const TStr& FNm, bool& OpenedP) : TSBase(FNm.CStr()), TSIn(FNm), ZipStdoutRd(NULL), ZipStdoutWr(NULL),
  FLen(0), CurFPos(0), Bf(NULL), BfC(0), BfL(0) {
  EAssertR(! FNm.Empty(), "Empty file-name.");
  FLen = TZipIn::GetFLen(FNm);
  OpenedP = TFile::Exists(FNm);
  if (OpenedP) {
    #ifdef GLib_WIN
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    // Create a pipe for the child process's STDOUT.
    EAssertR(CreatePipe(&ZipStdoutRd, &ZipStdoutWr, &saAttr, 0), "Stdout pipe creation failed");
    // Ensure the read handle to the pipe for STDOUT is not inherited.
    SetHandleInformation(ZipStdoutRd, HANDLE_FLAG_INHERIT, 0);
    #else
    // no implementation needed
    #endif
    CreateZipProcess(GetCmd(FNm.GetFExt()), FNm);
    Bf = new char[MxBfL]; BfC = BfL=-1;
    FillBf();
  }
}

PSIn TZipIn::New(const TStr& FNm) {
  return PSIn(new TZipIn(FNm));
}

PSIn TZipIn::New(const TStr& FNm, bool& OpenedP){
  return PSIn(new TZipIn(FNm, OpenedP));
}

TZipIn::~TZipIn(){
  #ifdef GLib_WIN
  if (ZipStdoutRd != NULL) {
    EAssertR(CloseHandle(ZipStdoutRd), "Closing read-end of pipe failed"); }
  if (ZipStdoutWr != NULL) {
    EAssertR(CloseHandle(ZipStdoutWr)!=0, "Closing write-end of pipe failed"); }
  #else
  if (ZipStdoutRd != NULL) {
    EAssertR(pclose(ZipStdoutRd) != -1, "Closing of the process failed"); }
  #endif
  if (Bf != NULL) { delete[] Bf; }
}

int TZipIn::GetBf(const void* LBf, const TSize& LBfL){
  int LBfS=0;
  if (TSize(BfC+LBfL)>TSize(BfL)){
    for (TSize LBfC=0; LBfC<LBfL; LBfC++){
      if (BfC==BfL){FillBf();}
      LBfS+=((char*)LBf)[LBfC]=Bf[BfC++];}
  } else {
    for (TSize LBfC=0; LBfC<LBfL; LBfC++){
      LBfS+=(((char*)LBf)[LBfC]=Bf[BfC++]);}
  }
  return LBfS;
}

void TZipIn::AddZipExtCmd(const TStr& ZipFNmExt, const TStr& ZipCmd) {
  if (FExtToCmdH.Empty()) FillFExtToCmdH();
  FExtToCmdH.AddDat(ZipFNmExt, ZipCmd);
}

bool TZipIn::IsZipExt(const TStr& FNmExt) {
  if (FExtToCmdH.Empty()) FillFExtToCmdH();
  return FExtToCmdH.IsKey(FNmExt);
}

void TZipIn::FillFExtToCmdH() {
  // 7za decompress: "e -y -bd -so";
  #ifdef GLib_WIN
  const char* ZipCmd = "7z.exe e -y -bd -so";
  #else
  const char* ZipCmd = "7za e -y -bd -so";
  #endif
  if (FExtToCmdH.Empty()) {
    FExtToCmdH.AddDat(".gz",  ZipCmd);
    FExtToCmdH.AddDat(".7z",  ZipCmd);
    FExtToCmdH.AddDat(".rar", ZipCmd);
    FExtToCmdH.AddDat(".zip", ZipCmd);
    FExtToCmdH.AddDat(".cab", ZipCmd);
    FExtToCmdH.AddDat(".arj", ZipCmd);
    FExtToCmdH.AddDat(".bzip2", ZipCmd);
    FExtToCmdH.AddDat(".bz2", ZipCmd);
  }
}

TStr TZipIn::GetCmd(const TStr& ZipFNm) {
  if (FExtToCmdH.Empty()) FillFExtToCmdH();
  const TStr Ext = ZipFNm.GetFExt().GetLc();
  EAssertR(FExtToCmdH.IsKey(Ext), TStr::Fmt("Unsupported file extension '%s'", Ext.CStr()));
  return FExtToCmdH.GetDat(Ext);
}

uint64 TZipIn::GetFLen(const TStr& ZipFNm) {
  #ifdef GLib_WIN
  HANDLE ZipStdoutRd, ZipStdoutWr;
  // create pipes
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
    // Create a pipe for the child process's STDOUT.
  const int PipeBufferSz = 32*1024;
  EAssertR(CreatePipe(&ZipStdoutRd, &ZipStdoutWr, &saAttr, PipeBufferSz), "Stdout pipe creation failed");
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  SetHandleInformation(ZipStdoutRd, HANDLE_FLAG_INHERIT, 0);
  //CreateZipProcess(GetCmd(FNm), FNm);
  { const TStr CmdLine = TStr::Fmt("7z.exe l %s", ZipFNm.CStr());
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdOutput = ZipStdoutWr;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  // Create the child process.
  const BOOL FuncRetn = CreateProcess(NULL, (LPSTR) CmdLine.CStr(),
    NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
  EAssertR(FuncRetn!=0, TStr::Fmt("Can not execute '%s'", CmdLine.CStr()).CStr());
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread); }
  #else
  const TStr CmdLine = TStr::Fmt("7za l %s", ZipFNm.CStr());
  FILE* ZipStdoutRd = popen(CmdLine.CStr(), "r");
  EAssertR(ZipStdoutRd != NULL, TStr::Fmt("Can not execute '%s'", CmdLine.CStr()).CStr());
  #endif
  // Read output from the child process
  const int BfSz = 32*1024;
  char* Bf = new char [BfSz];
  int BfC=0, BfL=0;
  memset(Bf, 0, BfSz);
  #ifdef GLib_WIN
  DWORD BytesRead;
  EAssert(ReadFile(ZipStdoutRd, Bf, MxBfL, &BytesRead, NULL) != 0);
  #else
  size_t BytesRead = fread(Bf, 1, MxBfL, ZipStdoutRd);
  EAssert(BytesRead != 0);
  EAssert(pclose(ZipStdoutRd) != -1);
  #endif
  BfL = (int) BytesRead;  IAssert((BfC!=0)||(BfL!=0));
  BfC = 0; Bf[BfL] = 0;
  // find file lenght
  TStr Str(Bf);  delete [] Bf;
  TStrV StrV; Str.SplitOnWs(StrV);
  int n = StrV.Len()-1;
  while (n > 0 && ! StrV[n].IsPrefix("-----")) { n--; }
  if (n-7 <= 0) {
    WrNotify(TStr::Fmt("Corrupt file %s: MESSAGE:\n", ZipFNm.CStr()).CStr(), Str.CStr());
    SaveToErrLog(TStr::Fmt("Corrupt file %s. Message:\n:%s\n", ZipFNm.CStr(), Str.CStr()).CStr());
    return 0;
  }
  return StrV[n-7].GetInt64();
}

/////////////////////////////////////////////////
// Output-File
TStrStrH TZipOut::FExtToCmdH;
const TSize TZipOut::MxBfL=4*1024;

void TZipOut::FlushBf() {
  #ifdef GLib_WIN
  DWORD BytesOut;
  EAssertR(WriteFile(ZipStdinWr, Bf, DWORD(BfL), &BytesOut, NULL)!=0, "Error writting to the file '"+GetSNm()+"'.");
  #else
  size_t BytesOut = fwrite(Bf, 1, BfL, ZipStdinWr);
  #endif
  EAssert(BytesOut == BfL);
  BfL = 0;
}

void TZipOut::CreateZipProcess(const TStr& Cmd, const TStr& ZipFNm) {
  const TStr CmdLine = TStr::Fmt("%s %s", Cmd.CStr(), ZipFNm.CStr());
  #ifdef GLib_WIN
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdInput = ZipStdinRd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  // Create the child process.
  const BOOL FuncRetn = CreateProcess(NULL,
    (LPSTR) CmdLine.CStr(),  // command line
    NULL,          // process security attributes
    NULL,          // primary thread security attributes
    TRUE,          // handles are inherited
    0,             // creation flags
    NULL,          // use parent's environment
    NULL,          // use parent's current directory
    &siStartInfo,  // STARTUPINFO pointer
    &piProcInfo);  // receives PROCESS_INFORMATION
  EAssertR(FuncRetn!=0, TStr::Fmt("Can not execute '%s'", CmdLine.CStr()).CStr());
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);
  #else
  ZipStdinWr = popen(CmdLine.CStr(),"w");
  EAssertR(ZipStdinWr != NULL,  TStr::Fmt("Can not execute '%s'", CmdLine.CStr()).CStr());
  #endif
}

TZipOut::TZipOut(const TStr& FNm) : TSBase(FNm.CStr()), TSOut(FNm), ZipStdinRd(NULL), ZipStdinWr(NULL), Bf(NULL), BfL(0){
  EAssertR(! FNm.Empty(), "Empty file-name.");
  #ifdef GLib_WIN
  // create pipes
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  // Create a pipe for the child process's STDOUT.
  EAssertR(CreatePipe(&ZipStdinRd, &ZipStdinWr, &saAttr, 0), "Stdout pipe creation failed");
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  SetHandleInformation(ZipStdinWr, HANDLE_FLAG_INHERIT, 0);
  #else
  // no implementation necessary
  #endif
  CreateZipProcess(GetCmd(FNm), FNm);
  Bf=new char[MxBfL];  BfL=0;
}

PSOut TZipOut::New(const TStr& FNm){
  return PSOut(new TZipOut(FNm));
}

TZipOut::~TZipOut() {
  if (BfL!=0) { FlushBf(); }
  #ifdef GLib_WIN
  if (ZipStdinWr != NULL) { EAssertR(CloseHandle(ZipStdinWr), "Closing write-end of pipe failed"); }
  if (ZipStdinRd != NULL) { EAssertR(CloseHandle(ZipStdinRd), "Closing read-end of pipe failed"); }
  #else
  if (ZipStdinWr != NULL) { EAssertR(pclose(ZipStdinWr) != -1, "Closing of the process failed"); }
  #endif
  if (Bf!=NULL) { delete[] Bf; }
}

int TZipOut::PutCh(const char& Ch){
  if (BfL==MxBfL) {FlushBf();}
  return Bf[BfL++]=Ch;
}

int TZipOut::PutBf(const void* LBf, const TSize& LBfL){
  int LBfS=0;
  if (BfL+LBfL>MxBfL){
    for (TSize LBfC=0; LBfC<LBfL; LBfC++){
      LBfS+=PutCh(((char*)LBf)[LBfC]);}
  } else {
    for (TSize LBfC=0; LBfC<LBfL; LBfC++){
      LBfS+=(Bf[BfL++]=((char*)LBf)[LBfC]);}
  }
  return LBfS;
}

void TZipOut::Flush(){
  FlushBf();
  #ifdef GLib_WIN
  EAssertR(FlushFileBuffers(ZipStdinWr)!=0, "Can not flush file '"+GetSNm()+"'.");
  #else
  EAssertR(fflush(ZipStdinWr)==0, "Can not flush file '"+GetSNm()+"'.");
  #endif
}

void TZipOut::AddZipExtCmd(const TStr& ZipFNmExt, const TStr& ZipCmd) {
  if (FExtToCmdH.Empty()) FillFExtToCmdH();
  FExtToCmdH.AddDat(ZipFNmExt, ZipCmd);
}

bool TZipOut::IsZipExt(const TStr& FNmExt) {
  if (FExtToCmdH.Empty()) FillFExtToCmdH();
  return FExtToCmdH.IsKey(FNmExt);
}

void TZipOut::FillFExtToCmdH() {
   // 7za compress: "a -y -bd -si{CompressedFNm}"
  #ifdef GLib_WIN
  const char* ZipCmd = "7z.exe a -y -bd -si";
  #else
  const char* ZipCmd = "7za a -y -bd -si";
  #endif
  if (FExtToCmdH.Empty()) {
    FExtToCmdH.AddDat(".gz",  ZipCmd);
    FExtToCmdH.AddDat(".7z",  ZipCmd);
    FExtToCmdH.AddDat(".rar", ZipCmd);
    FExtToCmdH.AddDat(".zip", ZipCmd);
    FExtToCmdH.AddDat(".cab", ZipCmd);
    FExtToCmdH.AddDat(".arj", ZipCmd);
    FExtToCmdH.AddDat(".bzip2", ZipCmd);
    FExtToCmdH.AddDat(".bz2", ZipCmd);
  }
}

TStr TZipOut::GetCmd(const TStr& ZipFNm) {
  if (FExtToCmdH.Empty()) FillFExtToCmdH();
  const TStr Ext = ZipFNm.GetFExt().GetLc();
  EAssertR(FExtToCmdH.IsKey(Ext), TStr::Fmt("Unsupported file extension '%s'", Ext.CStr()));
  return FExtToCmdH.GetDat(Ext)+ZipFNm.GetFMid();
}
