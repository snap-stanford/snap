#include "subprocess.h"

const TStr& TSubProcess::GetCmd() const {
	return Cmd;
}

bool TSubProcess::IsRead(TSubProcessMode M) {
	return M == spmRead || M == spmReadWrite;
}
bool TSubProcess::IsWrite(TSubProcessMode M) {
	return M == spmWrite || M == spmReadWrite;
}

void TSubProcess::Stream(const PSIn& In, const PSOut& Out) {
	Stream(*In, *Out);
}
void TSubProcess::Stream(TSIn& In, TSOut& Out) {
	const int MxBfL = 4 * 1024;
	char Bf[4 * 1024];
	while (!In.Eof()) {
		int Len = In.Len();
		if (Len == 0 || Len > MxBfL) {
			Len = MxBfL;
		}
		In.GetBf(Bf, Len);
		Out.PutBf(Bf, Len);
	}
}

PSubProcess TSubProcess::New(const TStr& Cmd_, TSubProcessMode Mod_) {
	return new TSubProcess(Cmd_, Mod_);
}

#ifndef GLib_WIN

// 1 success, 0 fail
int TSubProcess::Open(char *cmd,            //duplicate this in argv[0]
                char *argv[],         //last argv[] must be NULL
                const char *type,     //"p" for execvp()
                FILE **pfp_read,      //read file handle returned
                FILE **pfp_write,     //write file handle returned
                int *ppid,            //process id returned
                int *fd_to_close_in_child,        //[in], array of fd
                int fd_to_close_in_child_count)   //number of valid fd
{
    int j, pfd_read[2], pfd_write[2];


    assert(pfp_read);
    assert(pfp_write);
    assert(cmd);
    assert(argv);
    assert(type);
    assert(ppid);
    assert(!fd_to_close_in_child_count || fd_to_close_in_child);
    if (!pfp_read || !pfp_write) return 0;
    *pfp_read=NULL;
    *pfp_write=NULL;
    if (!cmd || !argv || !type || !ppid) return 0;
    if (pipe(pfd_read)<0) return 0;
    if (pipe(pfd_write)<0) {
        close(pfd_read[0]);
        close(pfd_read[1]);
        return 0;
    }
    if ((*ppid=(int)fork())<0) {
        close(pfd_read[0]);
        close(pfd_read[1]);
        close(pfd_write[0]);
        close(pfd_write[1]);
        return 0;
    }
    if (!*ppid) {
        //child continues
        if (STDOUT_FILENO!=pfd_read[1]) {
            dup2(pfd_read[1],STDOUT_FILENO);
            close(pfd_read[1]);
        }
        close(pfd_read[0]);
        if (STDIN_FILENO!=pfd_write[0]) {
            dup2(pfd_write[0],STDIN_FILENO);
            close(pfd_write[0]);
        }
        close(pfd_write[1]);
        for (j=0; j<fd_to_close_in_child_count; j++)
            close(fd_to_close_in_child[j]);
        if (strstr(type,"p")) execvp(cmd, argv);
        else execv(cmd, argv);
        _exit(127); //execv() failed
    }
    //parent continues
    close(pfd_read[1]);
    close(pfd_write[0]);
    if (!(*pfp_read=fdopen(pfd_read[0],"r"))) {
        close(pfd_read[0]);
        close(pfd_write[1]);
        return 0;
    }
    if (!(*pfp_write=fdopen(pfd_write[1],"w"))) {
        fclose(*pfp_read);  //closing this also closes pfd_read[0]
        *pfp_read=NULL;
        close(pfd_write[1]);
        return 0;
    }
    return 1;
}    //pipe_open_2()

//Unix version
//returns 1 on success, 0 on failure
//
int TSubProcess::Close(FILE *fp_read,   //returned from pipe_open()
                 FILE *fp_write,  //returned from pipe_open()
                 int pid,         //returned from pipe_open()
                 int *result)     //can be NULL
{
    int res1, res2, status;


    if (result) *result=255;
    if (fp_read) res1=fclose(fp_read);
    else res1=0;
    if (fp_write) res2=fclose(fp_write);
    else res2=0;
    if (res1==EOF || res2==EOF) return 0;
    if (!pid) return 0;
    while (waitpid((pid_t)pid,&status,0)<0) if (EINTR!=errno) return 0;
    if (result && WIFEXITED(status)) *result=WEXITSTATUS(status);
    return 1;
}    //pipe_close_2()

#endif

TSubProcess::TSubProcess(const TStr& Cmd_, TSubProcessMode Mod_) : 
	/*Pip(Cmd_),*/ Cmd(Cmd_), Mode(Mod_), StdoutWr(NULL), StdinWr(NULL), StdinRd(NULL), StdoutRd(NULL) {
	Thr.SetSubProcess(this);
	#ifdef GLib_WIN

	
	// create pipes
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	// Create a pipe for the child process's STDOUT and STDIN
	const int PipeBufferSz = 1024*1024;
	if (IsRead(Mode)) {
		EAssertR(CreatePipe(&StdoutRd, &StdoutWr, &saAttr, PipeBufferSz), "Stdout pipe creation failed");
		SetHandleInformation(StdoutRd, HANDLE_FLAG_INHERIT, 0);
	}
	if (IsWrite(Mode)) {
		EAssertR(CreatePipe(&StdinRd, &StdinWr, &saAttr, 0), "Stdout pipe creation failed");
		SetHandleInformation(StdinWr, HANDLE_FLAG_INHERIT, 0);
	}

	// Create process params
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdOutput = StdoutWr;
	siStartInfo.hStdInput = StdinRd;

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
		
	Thr.Start();
	#else
	
	/*if (Mode == spmRead) {
		StdoutRd = popen(Cmd.CStr(), GetMode(Mode));
		EAssertR(StdoutRd != NULL,  TStr::Fmt("Can not execute '%s'", Cmd.CStr()).CStr());
	} else if (Mode == spmWrite) {
		StdinWr = popen(Cmd.CStr(), GetMode(Mode));
		EAssertR(StdinWr != NULL,  TStr::Fmt("Can not execute '%s'", Cmd.CStr()).CStr());
	} else {*/

		/*Open(char *cmd,            //duplicate this in argv[0]
                char *argv[],         //last argv[] must be NULL
                const char *type,     //"p" for execvp()
                FILE **pfp_read,      //read file handle returned
                FILE **pfp_write,     //write file handle returned
                int *ppid,            //process id returned
                int *fd_to_close_in_child,        //[in], array of fd
                int fd_to_close_in_child_count)   //number of valid fd*/
		TStrV Args;
		Cmd.SplitOnAllCh(' ', Args);
		const char **argv = new char *[Args.Len()];
		for (int i = 0; i < Args.Len(); i++) {
			argv[i] = Args[i].CStr();
		}
		const char *cmd = argv[0].CStr();
		int fd_to_close_in_child, fd_count;
		Open(cmd, argv, "p", &StdoutRd, &StdinWr, &Pid, NULL, 0);
	//}
	
	#endif
}

PSIn TSubProcess::OpenForReading() {
	Thr.Join();
	return TMemIn::New(Thr.Output);
}

PSOut TSubProcess::OpenForWriting() {
	EAssertR(IsWrite(Mode), "Process was not opened in write mode");
	return new TSubProcessOut(this);
}

PSIn TSubProcess::Pipe(const PSIn& In) {
	if (IsWrite(Mode)) {
		PSOut SOut = OpenForWriting();
		Stream(In, SOut);
		SOut->Flush();
		CloseForWriting();
	}
	if (IsRead(Mode)) {
		return OpenForReading();
	} else {
		return NULL;
	}
}

void TSubProcess::Execute(const PSIn& Send, PSOut& Receive) { 
	if (IsWrite(Mode)) {
		PSOut SOut = OpenForWriting();
		Stream(Send, SOut);
		SOut->Flush();
		CloseForWriting();
	}

	if (IsRead(Mode)) {
		PSIn SIn = OpenForReading();
		Stream(SIn, Receive);
		Receive->Flush();
		CloseForReading();
	}
}

void TSubProcess::CloseForWriting() {
	if (StdinRd != NULL) { 
		EAssertR(CloseHandle(StdinRd), "Closing read-end of pipe failed");
		StdinRd = NULL;
	}
	if (StdinWr != NULL) { 
		EAssertR(CloseHandle(StdinWr), "Closing write-end of pipe failed"); 
		StdinWr = NULL;
	}
}

void TSubProcess::CloseForReading() {
	if (StdoutRd != NULL) {
		EAssertR(CloseHandle(StdoutRd), "Closing read-end of pipe failed"); 
		StdoutRd = NULL;
	}
	if (StdoutWr != NULL) {
		EAssertR(CloseHandle(StdoutWr), "Closing write-end of pipe failed"); 
		StdoutWr = NULL;
	}
}

TSubProcess::~TSubProcess() {
	#ifdef GLib_WIN

	CloseForWriting();
	CloseForReading();	

	#else
	int Result = 0;

	/*if (StdoutRd != NULL) {
		EAssertR(pclose(StdoutRd) != -1, "Closing of the process failed"); 
	}
	if (StdinWr != NULL) { 
		EAssertR(pclose(StdinWr) != -1, "Closing of the process failed"); 
	}*/
	Close(StdoutRd, StdoutWr,  Pid, &Result);
	#endif
}


TSubProcessHandlerThread::TSubProcessHandlerThread() : Proc(NULL), Output(TMem::New()) {

}

void TSubProcessHandlerThread::Run() {
	PSOut SOut = TMemOut::New(Output);
	IAssert(Proc != NULL);
	PSIn SIn = new TSubProcessIn(Proc);
	TSubProcess::Stream(SIn, SOut);
}

void TSubProcessHandlerThread::SetSubProcess(TSubProcess *P) {
	Proc = P;
}

const int TSubProcessIn::MxBfL=4096; // on windows

void TSubProcessIn::FillBf(){
	EAssertR((BfC==BfL)/*&&((BfL==-1)||(BfL==MxBfL))*/, "Error reading file '"+GetSNm()+"'.");
#ifdef GLib_WIN
	// Read output from the child process
	//int BytesRead = Proc->Pip.Read(Bf, MxBfL);
	DWORD BytesRead = 0;
	if(ReadFile(Proc->StdoutRd, Bf, MxBfL, &BytesRead, NULL) == 0) {
		DWORD error = GetLastError();
		if (error != ERROR_HANDLE_EOF) {
			EFailR(TStr::Fmt("Error reading archive. %d", error));
		}
	}
  #else
  size_t BytesRead;
  if (feof(StdoutRd)) {
	  // if full length is unknown, check EOF flag
	  // useful in case the last read was final but stilled filled the buffer
	  FLen = CurFPos;
	  BfL = 0;
	  BfC = 0;
  } else {
	  BytesRead = fread(Bf, 1, MxBfL, StdoutRd);
	  EAssertR(BytesRead != 0, "Error reading archive.");
  }

  #endif

  if (BytesRead < MxBfL) {
		// if full length is unknown and this read did not fill the buffer,
		// consider this to be the final read and set the computed length
		FLen = CurFPos + BytesRead;
  }

  BfL = (int) BytesRead;
  CurFPos += BytesRead;
  EAssertR((BfC!=0)||(BfL!=0), "Error reading file '"+GetSNm()+"'.");
  BfC = 0;
}

TSubProcessIn::TSubProcessIn(const TStr& Cmd) : TSBase(Cmd.CStr()), TSIn(Cmd),
  FLen(0), CurFPos(0), Bf(NULL), BfC(0), BfL(0), IsEof(false) {
  EAssertR(! Cmd.Empty(), "Empty command.");

  Proc = new TSubProcess(Cmd, spmRead);
  Bf = new char[MxBfL]; BfC = BfL=-1;
  //FillBf();
}

TSubProcessIn::TSubProcessIn(PSubProcess _Proc) : TSBase(_Proc->GetCmd().CStr()), TSIn(_Proc->GetCmd()), Proc(_Proc),
  FLen(0), CurFPos(0), Bf(NULL), BfC(0), BfL(0), IsEof(false) {
	EAssertR((!Proc.Empty() && !Proc->GetCmd().Empty()), "Empty command.");
	Bf = new char[MxBfL]; BfC = BfL=-1;
	//FillBf();
}



PSIn TSubProcessIn::New(const TStr& FNm) {
  return PSIn(new TSubProcessIn(FNm));
}

PSIn TSubProcessIn::New(PSubProcess Proc) {
  return PSIn(new TSubProcessIn(Proc));
}

TSubProcessIn::~TSubProcessIn(){
  if (Bf != NULL) { delete[] Bf; }
}

int TSubProcessIn::GetBf(const void* LBf, const TSize& LBfL){

	#ifdef GLib_WIN
	// Read output from the child process
	//int BytesRead = Proc->Pip.Read(Bf, MxBfL);
	DWORD BytesRead = 0;
	if(ReadFile(Proc->StdoutRd, (char *)LBf, (DWORD) LBfL, &BytesRead, NULL) == 0) {
		DWORD error = GetLastError();
		if (error != ERROR_HANDLE_EOF) {
			EFailR(TStr::Fmt("Error reading archive. %d", error));
		} else {
			IsEof = true;
		}
	}
  #else
  size_t BytesRead;
  if (feof(StdoutRd)) {
	  // if full length is unknown, check EOF flag
	  // useful in case the last read was final but stilled filled the buffer
	  FLen = CurFPos;
	  BfL = 0;
	  BfC = 0;
  } else {
	  BytesRead = fread(LBf, 1, LBfL, StdoutRd);
	  EAssertR(BytesRead != 0, "Error reading archive.");
  }

  #endif

  if (BytesRead < MxBfL) {
		// if full length is unknown and this read did not fill the buffer,
		// consider this to be the final read and set the computed length
		FLen = CurFPos + BytesRead;
		IsEof = true;
  }

  BfL = (int) BytesRead;
  CurFPos += BytesRead;
  EAssertR((BfC!=0)||(BfL!=0), "Error reading file '"+GetSNm()+"'.");
  BfC = 0;



  /*int LBfS=0;
  if (TSize(BfC+LBfL)>TSize(BfL)){
    for (TSize LBfC=0; LBfC<LBfL; LBfC++){
      if (BfC==BfL){FillBf();}
      LBfS+=((char*)LBf)[LBfC]=Bf[BfC++];}
  } else {
    for (TSize LBfC=0; LBfC<LBfL; LBfC++){
      LBfS+=(((char*)LBf)[LBfC]=Bf[BfC++]);}
  }
  return LBfS;*/
  return 0;
}

/////////////////////////////////////////////////
// Output-File
TStrStrH TSubProcessOut::FExtToCmdH;
const TSize TSubProcessOut::MxBfL=4*1024;

void TSubProcessOut::FlushBf() {
  #ifdef GLib_WIN
  DWORD BytesOut;
	//BytesOut = Proc->Pip.Write(Bf, BfL);
  EAssertR(WriteFile(Proc->StdinWr, Bf, DWORD(BfL), &BytesOut, NULL)!=0, "Error writting to the file '"+GetSNm()+"'.");
  #else
  size_t BytesOut = fwrite(Bf, 1, BfL, Proc->StdinWr);
  #endif
  EAssert(BytesOut == BfL);
  BfL = 0;
}

void TSubProcessOut::Write(const char *Bf, int BfL) {
  #ifdef GLib_WIN
  DWORD BytesOut;
  //BytesOut = Proc->Pip.Write(Bf, BfL);
  EAssertR(WriteFile(Proc->StdinWr, Bf, DWORD(BfL), &BytesOut, NULL)!=0, "Error writting to pipe '"+GetSNm()+"'.");
  #else
  size_t BytesOut = fwrite(Bf, 1, BfL, Proc->StdinWr);
  #endif
  EAssert(BytesOut == BfL);
  BfL = 0;
}

TSubProcessOut::TSubProcessOut(const TStr& Cmd) : TSBase(Cmd.CStr()), TSOut(Cmd), Bf(NULL), BfL(0) {
  EAssertR(! Cmd.Empty(), "Empty file-name.");
  Proc = new TSubProcess(Cmd, spmWrite);
  Bf=new char[MxBfL];  BfL=0;
}

PSOut TSubProcessOut::New(const TStr& FNm){
  return PSOut(new TSubProcessOut(FNm));
}

TSubProcessOut::TSubProcessOut(PSubProcess _Proc) : TSBase(_Proc->GetCmd().CStr()), TSOut(_Proc->GetCmd()), Proc(_Proc),Bf(NULL), BfL(0) {
	EAssertR((!Proc.Empty() && !Proc->GetCmd().Empty()), "Empty command.");
	Bf = new char[MxBfL]; 
	BfL = 0;
}

PSOut TSubProcessOut::New(PSubProcess Proc){
	return PSOut(new TSubProcessOut(Proc));
}

TSubProcessOut::~TSubProcessOut() {
	if (BfL!=0) { FlushBf(); }
	if (Bf!=NULL) { delete[] Bf; }
}

int TSubProcessOut::PutCh(const char& Ch){
	/* unbuffered:
	Write(&Ch, sizeof(Ch));
	return Ch;*/
	if (BfL==MxBfL) {FlushBf();}
	return Bf[BfL++]=Ch;
}

int TSubProcessOut::PutBf(const void* LBf, const TSize& LBfL){
	int LBfS=0;
	if (BfL+LBfL>MxBfL){
		for (TSize LBfC=0; LBfC<LBfL; LBfC++){
			LBfS+=PutCh(((char*)LBf)[LBfC]);}
	} else {
		for (TSize LBfC=0; LBfC<LBfL; LBfC++){
		LBfS+=(Bf[BfL++]=((char*)LBf)[LBfC]);}
	}
  /* unbuffered:
  for (int i = 0; i < LBfL; i++) {
	  LBfS += ((char *) LBf)[i];
  }
  Write((const char *) LBf, LBfL);*/
  return LBfS;
}

void TSubProcessOut::Flush(){
  FlushBf();
  #ifdef GLib_WIN
  //EAssertR(FlushFileBuffers(Proc->StdinWr)!=0, "Can not flush file '"+GetSNm()+"'.");
  #else
  EAssertR(fflush(Proc->StdinWr)==0, "Can not flush file '"+GetSNm()+"'.");
  #endif
}
