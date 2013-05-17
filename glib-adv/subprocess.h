#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_

#include <base.h>
#include <concurrent/thread.h>

class TSubProcessIn;
class TSubProcessOut;
class TSubProcess;

class TSubProcessHandlerThread : public TThread {
public:
	TSubProcess *Proc;
	PMem Output;
	TSubProcessHandlerThread();
	void SetSubProcess(TSubProcess *P);
	void Run();
};

enum TSubProcessMode {
	spmNone, spmRead, spmWrite, spmReadWrite
};

ClassTP(TSubProcess, PSubProcess) // {
protected:
	TStr Cmd;
	TSubProcessMode Mode;
	TSubProcessHandlerThread Thr;
	
  #ifdef GLib_WIN
	///TStdIOPipe Pip;
    HANDLE StdoutWr, StdinWr, StdinRd, StdoutRd;
  #else 
    FILE* StdoutWr, *StdinRd;
	int Pid;

	static int TSubProcess::Open(char *cmd,            //duplicate this in argv[0]
                char *argv[],         //last argv[] must be NULL
                const char *type,     //"p" for execvp()
                FILE **pfp_read,      //read file handle returned
                FILE **pfp_write,     //write file handle returned
                int *ppid,            //process id returned
                int *fd_to_close_in_child,        //[in], array of fd
                int fd_to_close_in_child_count);

	static int TSubProcess::Close(FILE *fp_read,   //returned from pipe_open()
                 FILE *fp_write,  //returned from pipe_open()
                 int pid,         //returned from pipe_open()
                 int *result);
  #endif
  
public:
	TSubProcess(const TStr& Cmd, TSubProcessMode Mod_);
	static PSubProcess New(const TStr& Cmd, TSubProcessMode Mod_);
	~TSubProcess();
	static bool IsRead(TSubProcessMode M);
	static bool IsWrite(TSubProcessMode M);
	static void Stream(const PSIn& In, const PSOut& Out);
	static void Stream(TSIn& In, TSOut& Out);

	const TStr& GetCmd() const;

	/** Get a stream for reading from stdout. */
	PSIn OpenForReading();
	/** Get a stream for writing into the stdin of the subprocess. */
	PSOut OpenForWriting();

	/** Wrap an input stream so that it flows through that subprocess and the 
		response is readable at the returned input stream. 
		
		Not recommended for huge streams, as it first writes everything and then reads 
		the response.*/
	PSIn Pipe(const PSIn& In);
	void Execute(const PSIn& Send, PSOut& Receive);
	
	void CloseForWriting();
	void CloseForReading();
	
	friend class TSubProcessIn;
	friend class TSubProcessOut;
};

class TSubProcessIn : public TSIn {
private:
  static const int MxBfL;
  PSubProcess Proc;
  uint64 FLen, CurFPos;
  char* Bf;
  int BfC, BfL;
  bool IsEof;
private:
  void FillBf();
private:
  TSubProcessIn();
  TSubProcessIn(const TSubProcessIn&);
  TSubProcessIn& operator=(const TSubProcessIn&);
public:
  TSubProcessIn(const TStr& FNm);
  TSubProcessIn(PSubProcess Proc);
  static PSIn New(const TStr& FNm);
  static PSIn New(PSubProcess Proc);
  ~TSubProcessIn();

  bool Eof() { /*return CurFPos==FLen && CurFPos > 0 && BfC==BfL;*/ return IsEof; }
  int Len() const { 
	  if (FLen == 0) {
		return 0;
	  } else {
		return int(FLen-CurFPos+BfL-BfC); 
	  }
  }
  char GetCh() { if (BfC==BfL){FillBf();} return Bf[BfC++]; }
  char PeekCh() { if (BfC==BfL){FillBf();} return Bf[BfC]; }
  int GetBf(const void* LBf, const TSize& LBfL);

  uint64 GetFLen() const { return FLen; }
  uint64 GetCurFPos() const { return CurFPos; }
};

class TSubProcessOut : public TSOut{
private:
	static const TSize MxBfL;
	static TStrStrH FExtToCmdH;
	PSubProcess Proc;

	void Write(const char *Bf, int BfL);
	void FlushBf();

	char* Bf;
	TSize BfL;

	TSubProcessOut();
	TSubProcessOut(const TSubProcessOut&);
	TSubProcessOut& operator=(const TSubProcessOut&);
public:

  TSubProcessOut(const TStr& Cmd);
  static PSOut New(const TStr& Cmd);

  TSubProcessOut(PSubProcess Proc);
  static PSOut New(PSubProcess Proc);

  ~TSubProcessOut();

  int PutCh(const char& Ch);
  int PutBf(const void* LBf, const TSize& LBfL);
  void Flush();
};

#endif