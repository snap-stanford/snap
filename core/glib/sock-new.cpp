#include "sock.h"

#ifdef GLib_UNIX
class TSocketTimer : public TTTimer {
private:
  int SockId;
public:
  TSocketTimer(const int TimeOut, /*PSock &_Socket*/ const int _SockId) : TTTimer(TimeOut), SockId(_SockId) {}
  ~TSocketTimer() {}
  void OnTimeOut();
};
#endif

/////////////////////////////////////////////////
// Socket-System
class TSockSys{
public:
  // socket-system initialized
  static bool Active;
#ifdef GLib_WIN32
  // window handles
  static HWND SockWndHnd;
  static HWND DnsWndHnd;
  static HWND ReportWndHnd;
  static HWND TimerWndHnd;
  // message-handles
  static UINT SockMsgHnd;
  static UINT SockErrMsgHnd;
  static UINT DnsMsgHnd;
  static UINT ReportMsgHnd;
#endif
#ifdef GLib_UNIX
  static int TimerSignal;
  static int ResolverSignal;
  static int ET_epoll_fd, LT_epoll_fd;
#endif
  // sockets
  static uint64 SockBytesRead;
  static uint64 SockBytesWritten;
  static THash<TInt, TUInt64> SockIdToHndH;
  static THash<TUInt64, TInt> SockHndToIdH;
  static THash<TUInt64, TInt> SockHndToEventIdH;
#ifdef GLib_WIN32
  static TIntH SockTimerIdH;
#elif defined(GLib_UNIX)
  static THash<TInt, TInt> SockToTimerIdH;
  static THash<TInt, TInt> SockHndToStateH;
#endif
  static TUInt64H ActiveSockHndH;
  // socket-host
  static THash<TUInt64, PSockHost> HndToSockHostH;
  // socket-event
  static THash<TInt, PSockEvent> IdToSockEventH;
  static TIntH ActiveSockEventIdH;
  // report-event
  static THash<TInt, PReportEvent> IdToReportEventH;
  // timer
  static THash<TInt, ATimer> IdToTimerH;
public:
  // windows-sockets messages
  static TStr GetErrStr(const int ErrCd);
#ifdef GLib_WIN32
  // main message handler
  static LRESULT CALLBACK MainWndProc(
   HWND WndHnd, UINT Msg, WPARAM wParam, LPARAM lParam);
#endif
public:
  TSockSys();
  ~TSockSys();

  TSockSys& operator=(const TSockSys&){Fail; return *this;}

#ifdef GLib_WIN32
  // window handles
  static HWND GetSockWndHnd(){IAssert(Active); return SockWndHnd;}
  static HWND GetDnsWndHnd(){IAssert(Active); return DnsWndHnd;}
  static HWND GetReportWndHnd(){IAssert(Active); return ReportWndHnd;}
  static HWND GetTimerWndHnd(){IAssert(Active); return TimerWndHnd;}
  // message handles
  static UINT GetSockMsgHnd(){IAssert(Active); return SockMsgHnd;}
  static UINT GetSockErrMsgHnd(){IAssert(Active); return SockErrMsgHnd;}
  static UINT GetDnsMsgHnd(){IAssert(Active); return DnsMsgHnd;}
  static UINT GetReportMsgHnd(){IAssert(Active); return ReportMsgHnd;}
  // event set
  static int GetAllSockEventCdSet(){
    return (FD_READ|FD_WRITE|FD_OOB|FD_ACCEPT|FD_CONNECT|FD_CLOSE);}
#endif
#ifdef GLib_UNIX
  static int GetFreeRTSig(int Start);
  static void SetupFAsync(int fd);
  static void DoIOEvent(struct epoll_event *e);
  static void DoIO();
  static void DoTimer(siginfo_t *si);
  static void DoResolver(siginfo_t *si);
  static void AsyncLoop();
#endif
  // sockets (SockIdToHndH, SockHndToIdH, SockHndToEventH)
  static uint64 GetSockBytesRead(){return SockBytesRead;}
  static uint64 GetSockBytesWritten(){return SockBytesWritten;}
  static void AddSock(
   const int& SockId, const TSockHnd& SockHnd, const int& SockEventId);
  static void DelSock(const int& SockId);
  static bool IsSockId(const int& SockId){
    IAssert(Active); return SockIdToHndH.IsKey(SockId);}
  static bool IsSockHnd(const TSockHnd& SockHnd){
    IAssert(Active); return SockHndToIdH.IsKey(SockHnd);}
  static TSockHnd GetSockHnd(const int& SockId){
    IAssert(Active); return TSockHnd(SockIdToHndH.GetDat(SockId));}
  static TSockHnd GetSockId(const TSockHnd& SockHnd){
    IAssert(Active); return SockHndToIdH.GetDat(SockHnd);}
  static int GetSockEventId(const TSockHnd& SockHnd){
    IAssert(Active); return SockHndToEventIdH.GetDat(SockHnd);}
#ifdef GLib_WIN32
  static void AddSockTimer(const int& SockId, const int& MSecs){
    UINT ErrCd=(UINT)SetTimer(TSockSys::GetSockWndHnd(), SockId, uint(MSecs), NULL);
    ESAssert(ErrCd!=0);
    TSockSys::SockTimerIdH.AddKey(SockId);}
  static void DelIfSockTimer(const int& SockId){
    KillTimer(TSockSys::GetSockWndHnd(), SockId);
    TSockSys::SockTimerIdH.DelIfKey(SockId);}
#elif defined(GLib_UNIX)
  static void AddSockTimer(const int& SockId, const int& MSecs);
  static void DelIfSockTimer(const int& SockId);
#endif
  static bool IsSockActive(const TSockHnd& SockHnd){
    return ActiveSockHndH.IsKey(SockHnd);}
  static void SetSockActive(const TSockHnd& SockHnd, const bool& Active){
    IAssert(
     (Active&&!IsSockActive(SockHnd))||
     (!Active&&IsSockActive(SockHnd)));
    if (Active){ActiveSockHndH.AddKey(SockHnd);}
    else {ActiveSockHndH.DelKey(SockHnd);}}
  static const int MxSockBfL;

  // socket host (HndToSockHostH)
  static void AddSockHost(const TUInt64& SockHostHnd, const PSockHost& SockHost){
    HndToSockHostH.AddDat(SockHostHnd, SockHost);}
  static void DelSockHost(const TUInt64& SockHostHnd){
    HndToSockHostH.DelKey(SockHostHnd);}
  static bool IsSockHost(const TUInt64& SockHostHnd){
    return HndToSockHostH.IsKey(SockHostHnd);}
  static PSockHost GetSockHost(const TUInt64& SockHostHnd){
    return HndToSockHostH.GetDat(SockHostHnd);}

  // socket event (IdToSockEventH, ActiveSockEventIdH)
  static void AddSockEvent(const PSockEvent& SockEvent){
    IAssert(!IsSockEvent(SockEvent));
    IdToSockEventH.AddDat(SockEvent->GetSockEventId(), SockEvent);}
  static void DelSockEvent(const PSockEvent& SockEvent){
    IdToSockEventH.DelKey(SockEvent->GetSockEventId());}
  static bool IsSockEvent(const int& SockEventId){
    return IdToSockEventH.IsKey(TInt(SockEventId));}
  static bool IsSockEvent(const PSockEvent& SockEvent){
    return IdToSockEventH.IsKey(TInt(SockEvent->GetSockEventId()));}
  static PSockEvent GetSockEvent(const int& SockEventId){
    return IdToSockEventH.GetDat(SockEventId);}
  static bool IsSockEventActive(const int& SockEventId){
    return ActiveSockEventIdH.IsKey(SockEventId);}
  static void SetSockEventActive(const int& SockEventId, const bool& Active){
    IAssert(
     (Active&&!IsSockEventActive(SockEventId))||
     (!Active&&IsSockEventActive(SockEventId)));
    if (Active){ActiveSockEventIdH.AddKey(SockEventId);}
    else {ActiveSockEventIdH.DelKey(SockEventId);}}

  // report event (IdToReportEventH)
  static void AddReportEvent(const PReportEvent& ReportEvent){
    IAssert(!IsReportEvent(ReportEvent));
    IdToReportEventH.AddDat(TInt(ReportEvent->GetReportEventId()), ReportEvent);}
  static void DelReportEvent(const PReportEvent& ReportEvent){
    IdToReportEventH.DelKey(TInt(ReportEvent->GetReportEventId()));}
  static bool IsReportEvent(const PReportEvent& ReportEvent){
    return IdToReportEventH.IsKey(TInt(ReportEvent->GetReportEventId()));}
  static PReportEvent GetReportEvent(const int& ReportEventId){
    return IdToReportEventH.GetDat(ReportEventId);}

  // timer (IdToTimerH)
  static void AddTimer(const ATimer& Timer){
    IAssert(!IsTimer(Timer->GetTimerId()));
    IdToTimerH.AddDat(TInt(Timer->GetTimerId()), Timer);}
  static void DelTimer(const int& TimerId){
    IdToTimerH.DelKey(TimerId);}
  static bool IsTimer(const int& TimerId){
    return IdToTimerH.IsKey(TimerId);}
  static ATimer GetTimer(const int& TimerId){
    return IdToTimerH.GetDat(TimerId);}

  // socket & host events
  static void OnRead(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnWrite(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnOob(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnAccept(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnConnect(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnClose(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnTimeOut(const TSockHnd& SockHnd, const PSockEvent& SockEvent);
  static void OnError(
   const TSockHnd& SockHnd, const PSockEvent& SockEvent, const int& ErrCd);
  static void OnGetHost(const PSockHost& SockHost);

  // status
  static TStr GetStatusStr();
};

// socket system initialized
bool TSockSys::Active=false;
#ifdef GLib_WIN32
// window handles
HWND TSockSys::SockWndHnd=0;
HWND TSockSys::DnsWndHnd=0;
HWND TSockSys::ReportWndHnd=0;
HWND TSockSys::TimerWndHnd=0;
// message handles
UINT TSockSys::SockMsgHnd=0;
UINT TSockSys::SockErrMsgHnd=0;
UINT TSockSys::DnsMsgHnd=0;
UINT TSockSys::ReportMsgHnd=0;
#endif
#ifdef GLib_UNIX
int TSockSys::TimerSignal = -1;
int TSockSys::ResolverSignal = -1;
int TSockSys::ET_epoll_fd = -1;
int TSockSys::LT_epoll_fd = -1;
#endif
// sockets
uint64 TSockSys::SockBytesRead=0;
uint64 TSockSys::SockBytesWritten=0;
THash<TInt, TUInt64> TSockSys::SockIdToHndH;
THash<TUInt64, TInt> TSockSys::SockHndToIdH;
THash<TUInt64, TInt> TSockSys::SockHndToEventIdH;
#ifdef GLib_WIN32
TIntH TSockSys::SockTimerIdH;
#elif defined(GLib_UNIX)
THash<TInt, TInt> TSockSys::SockToTimerIdH;
THash<TInt, TInt> TSockSys::SockHndToStateH;
#endif
TUInt64H TSockSys::ActiveSockHndH;
const int TSockSys::MxSockBfL=100*1024;
// socket host
THash<TUInt64, PSockHost> TSockSys::HndToSockHostH;
// socket event
THash<TInt, PSockEvent> TSockSys::IdToSockEventH;
TIntH TSockSys::ActiveSockEventIdH;
// report event
THash<TInt, PReportEvent> TSockSys::IdToReportEventH;
// timer
THash<TInt, ATimer> TSockSys::IdToTimerH;

TSockSys SockSys; // the only instance of TSockSys

#ifdef GLib_WIN32
TStr TSockSys::GetErrStr(const int ErrCd){
  switch (ErrCd){
    // WSAStartup errors
    case WSASYSNOTREADY: return "Underlying network subsystem is not ready for network communication.";
    case WSAVERNOTSUPPORTED: return "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.";
    case WSAEPROCLIM: return "Limit on the number of tasks supported by the Windows Sockets implementation has been reached.";
    // WSACleanup error
    case WSANOTINITIALISED: return "Windows Sockets not initialized.";
    case WSAENETDOWN: return "The network subsystem has failed.";
    // general errors
    case WSAEWOULDBLOCK: return "Resource temporarily unavailable (op. would block).";
    case WSAEINPROGRESS: return "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
    case WSAEADDRINUSE: return "The specified address is already in use.";
    case WSAEADDRNOTAVAIL: return "The specified address is not available from the local machine.";
    case WSAEAFNOSUPPORT: return "Addresses in the specified family cannot be used with this socket.";
    case WSAECONNREFUSED: return "The attempt to connect was forcefully rejected.";
    case WSAENETUNREACH: return "The network cannot be reached from this host at this time.";
    case WSAEFAULT: return "Bad parameter.";
    case WSAEINVAL: return "The socket is already bound to an address.";
    case WSAEISCONN: return "The socket is already connected.";
    case WSAEMFILE: return "No more file descriptors are available.";
    case WSAENOBUFS: return "No buffer space is available. The socket cannot be connected.";
    case WSAENOTCONN: return "The socket is not connected.";
    case WSAETIMEDOUT: return "Attempt to connect timed out without establishing a connection.";
    case WSAECONNRESET: return "The connection was reset by the remote side.";
    case WSAECONNABORTED: return "The connection was terminated due to a time-out or other failure.";
    default: return TStr("Unknown socket error (code ")+TInt::GetStr(ErrCd)+TStr(").");
  }
}

LRESULT CALLBACK TSockSys::MainWndProc(
 HWND WndHnd, UINT MsgHnd, WPARAM wParam, LPARAM lParam){
  if (MsgHnd==TSockSys::SockMsgHnd){
    IAssert(WndHnd==GetSockWndHnd());
    TSockHnd SockHnd=wParam;
    if (IsSockHnd(SockHnd)){
      int SockEventId=GetSockEventId(SockHnd);
      PSockEvent SockEvent=GetSockEvent(SockEventId);
      //SetSockActive(SockHnd, true);
      SetSockEventActive(SockEventId, true);
      try {
        int ErrCd=WSAGETSELECTERROR(lParam);
        if (ErrCd==0){
          int EventCd=WSAGETSELECTEVENT(lParam);
          switch (EventCd){
            case FD_READ: OnRead(SockHnd, SockEvent); break;
            case FD_WRITE: OnWrite(SockHnd, SockEvent); break;
            case FD_OOB: OnOob(SockHnd, SockEvent); break;
            case FD_ACCEPT: OnAccept(SockHnd, SockEvent); break;
            case FD_CONNECT: OnConnect(SockHnd, SockEvent); break;
            case FD_CLOSE: OnClose(SockHnd, SockEvent); break;
            default: Fail;
          }
        } else {
          OnError(SockHnd, SockEvent, ErrCd);
        }
      } catch (...){
        SaveToErrLog("Exception from 'switch (EventCd)'");
      }
      SetSockEventActive(SockEventId, false);
      //SetSockActive(SockHnd, false);
    }
  } else
  if (MsgHnd==TSockSys::SockErrMsgHnd){
    IAssert(WndHnd==GetSockWndHnd());
    TSockHnd SockHnd=wParam;
    if (IsSockHnd(SockHnd)){
      int SockEventId=GetSockEventId(SockHnd);
      PSockEvent SockEvent=GetSockEvent(SockEventId);
      //SetSockActive(SockHnd, true);
      SetSockEventActive(SockEventId, true);
      try {
        int ErrCd=int(lParam);
        OnError(SockHnd, SockEvent, ErrCd);
      } catch (...){
        SaveToErrLog("Exception from 'OnError(SockHnd, SockEvent, ErrCd)'");
      }
      SetSockEventActive(SockEventId, false);
      //SetSockActive(SockHnd, false);
    }
  } else
  if (MsgHnd==WM_TIMER){
    if (WndHnd==GetSockWndHnd()){
      int SockId=int(wParam);
      DelIfSockTimer(SockId);
      if (IsSockId(SockId)){
        TSockHnd SockHnd=GetSockHnd(SockId);
        int SockEventId=GetSockEventId(SockHnd);
        PSockEvent SockEvent=GetSockEvent(SockEventId);
        //SetSockActive(SockHnd, true);
        SetSockEventActive(SockEventId, true);
        try {
          OnTimeOut(SockHnd, SockEvent);
        } catch (...){
          SaveToErrLog("Exception from OnTimeOut(SockHnd, SockEvent);");
        }
        SetSockEventActive(SockEventId, false);
        //SetSockActive(SockHnd, false);
      }
    } else
    if (WndHnd==GetTimerWndHnd()){
      int TimerId=int(wParam);
      if (TSockSys::IsTimer(TimerId)){
        PTimer Timer=TSockSys::GetTimer(TimerId)();
        Timer->IncTicks();
        try {
          Timer->OnTimeOut();
        } catch (...){
          SaveToErrLog("Exception from Timer->OnTimeOut();");
        }
      }
    } else {
      Fail;
    }
  } else
  if (MsgHnd==TSockSys::DnsMsgHnd){
    IAssert(WndHnd==GetDnsWndHnd());
    uint SockHostHnd=int(wParam);
    if (TSockSys::IsSockHost(SockHostHnd)){
      TSockHostStatus Status=TSockHost::GetStatus(WSAGETASYNCERROR(lParam));
      PSockHost SockHost=TSockSys::GetSockHost(SockHostHnd);
      SockHost->GetFromHostEnt(Status, (hostent*)SockHost->HostEntBf);
      DelSockHost(SockHostHnd);								// !!! !bn: kaj se zgodi ce unics TSockHost prezgodi? sej se ne deregistrira?
      try {
        OnGetHost(SockHost);
      } catch (...){
        SaveToErrLog("Exception from OnGetHost(SockHost);");
      }
    }
  } else
  if (MsgHnd==TSockSys::ReportMsgHnd){
    IAssert(WndHnd==GetReportWndHnd());
    int ReportEventId=int(lParam);
    PReportEvent ReportEvent=TSockSys::GetReportEvent(ReportEventId);
    try {
      ReportEvent->OnReport();
    } catch (...){
      SaveToErrLog("Exception from ReportEvent->OnReport()");
    }
    TSockSys::DelReportEvent(ReportEvent);
  } else {
    return DefWindowProc(WndHnd, MsgHnd, wParam, lParam);
  }
  return 0;
}

TSockSys::TSockSys(){
  IAssert(Active==false);

  WNDCLASS WndClass;
  WndClass.style=0;
  WndClass.lpfnWndProc=MainWndProc;
  WndClass.cbClsExtra=0;
  WndClass.cbWndExtra=0;
  FSAssert((WndClass.hInstance=GetModuleHandle(NULL))!=NULL);
  WndClass.hIcon=NULL;
  WndClass.hCursor=NULL;
  WndClass.hbrBackground=NULL;
  WndClass.lpszMenuName=NULL;
  WndClass.lpszClassName="SockWndClass";
  FSAssert(RegisterClass(&WndClass)!=0);

  TSockSys::SockWndHnd=CreateWindow(
   "SockWndClass", "Socket Window",
   WS_OVERLAPPEDWINDOW, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
   GetModuleHandle(NULL), NULL);
  FSAssert(TSockSys::SockWndHnd!=NULL);

  TSockSys::DnsWndHnd=CreateWindow(
   "SockWndClass", "Dns Window",
   WS_OVERLAPPEDWINDOW, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
   GetModuleHandle(NULL), NULL);
  FSAssert(TSockSys::DnsWndHnd!=NULL);

  TSockSys::ReportWndHnd=CreateWindow(
   "SockWndClass", "RepMsg Window",
   WS_OVERLAPPEDWINDOW, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
   GetModuleHandle(NULL), NULL);
  FSAssert(TSockSys::ReportWndHnd!=NULL);

  TSockSys::TimerWndHnd=CreateWindow(
   "SockWndClass", "Net Timer",
   WS_OVERLAPPEDWINDOW, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
   GetModuleHandle(NULL), NULL);
  FSAssert(TSockSys::TimerWndHnd!=NULL);

  SockMsgHnd=RegisterWindowMessage("SockSys.SockMsg"); FSAssert(SockMsgHnd!=0);
  SockErrMsgHnd=RegisterWindowMessage("SockSys.SockErrorMsg"); FSAssert(SockErrMsgHnd!=0);
  DnsMsgHnd=RegisterWindowMessage("SockSys.DnsMsg"); FSAssert(DnsMsgHnd!=0);
  ReportMsgHnd=RegisterWindowMessage("SockSys.RepMsg"); FSAssert(ReportMsgHnd!=0);

  //WORD Version=MAKEWORD((2),(0));
  WORD Version=((WORD) (((BYTE) (2)) | (((WORD) ((BYTE) (0))) << 8)));

  WSADATA WsaData;
  int WsaErrCd=WSAStartup(Version, &WsaData);
  FAssert(WsaErrCd==0, TSockSys::GetErrStr(WsaErrCd));
  FAssert(
   WsaData.wVersion==Version,
   "Can not find appropriate version of WinSock DLL.");
  Active=true;
}

TSockSys::~TSockSys(){
  if (Active){
    IAssert(ActiveSockHndH.Len()==0);
    IAssert(ActiveSockEventIdH.Len()==0);
    int WsaErrCd=WSACleanup();
    FAssert(WsaErrCd==0, TSockSys::GetErrStr(WsaErrCd));
    Active=false;
  }
}
#elif defined(GLib_UNIX)
TStr TSockSys::GetErrStr(const int ErrCd) {
  char b[1024];
/*	-- somewhat annoying - strerror_r as defined by SUSv3 doesn't want to work here.
  int ret = strerror_r(ErrCd, b, 1023);
  if (ret == -1) {
    if (errno == EINVAL) strcpy(b, "Invalid error number.");
    else if (errno == ERANGE) strcpy(b, "Insufficient storage space for error string.");
    else strcpy(b, "Error.");
  }
  return TStr(b);
*/
  return TStr(strerror_r(ErrCd, b, 1023));
}

void TSockSys::AddSockTimer(const int& SockId, const int& MSecs) {
  ATimer Tmr = new TSocketTimer(MSecs, SockId);
  AddTimer(Tmr);
  SockToTimerIdH.AddDat(SockId, Tmr->GetTimerId());
}

void TSockSys::DelIfSockTimer(const int& SockId) {
  // can be called from within TSocketTimer::OnTimeout() !
  // !!! !bn: error handling.
  ATimer Tmr = GetTimer(SockToTimerIdH[SockId]);
  DelTimer(SockToTimerIdH[SockId]);	// first take from timer list, then destroy object!
  delete Tmr();
}

int TSockSys::GetFreeRTSig(int Start) {
  sigset_t ss, cs;
  sigemptyset(&ss);
  sigprocmask(SIG_BLOCK, &ss, &cs);
  for (int i=Start;i<=SIGRTMAX;i++) {
    if (sigismember(&cs, i)) {
      // it's not blocked. if it does not have a signal handler, it's ours.
      struct sigaction sa;
      if (sigaction(i, NULL, &sa) != 0) {
        // !!! !bn: error handling. EINTR possible?
      }
      if ((sa.sa_flags & SA_SIGINFO) && (sa.sa_sigaction != NULL)) continue;
      if (!(sa.sa_flags & SA_SIGINFO) && (sa.sa_handler != NULL)) continue;
      return i;
    }
  }
  return -1;
}

TSockSys::TSockSys() {
  IAssert(Active==false);
  
  TimerSignal = GetFreeRTSig(SIGRTMIN);
  ResolverSignal = GetFreeRTSig(TimerSignal+1);
  
  sigset_t css;
  sigemptyset(&css);
  sigaddset(&css, SIGIO);
  sigaddset(&css, TimerSignal);
  sigaddset(&css, ResolverSignal);
  
  sigaddset(&css, SIGPIPE);		// these two could kill the process on socket errors / OOB.
  sigaddset(&css, SIGURG);		// we'll handle errors synchronously
  
  sigprocmask(SIG_BLOCK, &css, NULL);
	
  ET_epoll_fd = epoll_create(20);
  LT_epoll_fd = epoll_create(20);
	
  Active = true;
}

TSockSys::~TSockSys() {
  if (Active){
    IAssert(ActiveSockHndH.Len()==0);
    IAssert(ActiveSockEventIdH.Len()==0);
    
    close(ET_epoll_fd);
    close(LT_epoll_fd);
    
    sigset_t css;
    sigemptyset(&css);
    sigaddset(&css, SIGIO);
    sigaddset(&css, TimerSignal);
    sigaddset(&css, ResolverSignal);
    sigprocmask(SIG_UNBLOCK, &css, NULL);
    
    TimerSignal = ResolverSignal = -1;
    
    Active = false;
  }
}

void TSockSys::SetupFAsync(int fd) {
  IAssert(Active==true);
  
  fcntl(fd, F_SETOWN, (int)getpid());
  int flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK|O_ASYNC;
  fcntl(fd, F_SETFL, flags);

  struct epoll_event epe;
  epe.events = EPOLLIN;
  epe.data.fd = fd;
  if (epoll_ctl(LT_epoll_fd, EPOLL_CTL_ADD, fd, &epe) == -1) perror("epoll_ctl");		// !!!! TODO: kaksn assert nej bi dau tle?

  epe.events = EPOLLOUT | EPOLLET;
  if (epoll_ctl(ET_epoll_fd, EPOLL_CTL_ADD, fd, &epe) == -1) perror("epoll_ctl");
}

void TSockSys::DoIOEvent(struct epoll_event *e) {
  printf(" event = %d on fd = %d\n", e->events, e->data.fd);
  
    TSockHnd SockHnd=e->data.fd;
    int Events = e->events;
    
    if (IsSockHnd(SockHnd)){
      int SockEventId=GetSockEventId(SockHnd);
      PSockEvent SockEvent=GetSockEvent(SockEventId);
      //SetSockActive(SockHnd, true);
      SetSockEventActive(SockEventId, true);
      
      // what was the event?
      // note: close is handled in OnRead()
      // connect generates IN and OUT events, but only one is delivered [lt/et descriptors are separate, event aggregation not done]
      //  -> if socket not connected
      //      * OUT event ignored
      //      * IN event triggers OnConnect [if the first-delivered event triggered connect,
      //         then OnRead() could be called on freshly connected socket and nothing to read.]
      //  -> if it's a listening socket, do OnAccept()
      // err, oob, read|close, write
      
      try {
        int ErrCd=TSock::GetSockErr(SockHnd);
        
        if (ErrCd==0) {
          if (Events & EPOLLIN) {
            if (SockHndToStateH[SockHnd] == scsConnected) {
              OnRead(SockHnd, SockEvent);
            } else if (SockHndToStateH[SockHnd] == scsListening) {
              OnAccept(SockHnd, SockEvent);
            } else {
              SockHndToStateH[SockHnd] = scsConnected;
              OnConnect(SockHnd, SockEvent);
            }
          }
          if ((Events & EPOLLOUT)&&(SockHndToStateH[SockHnd] == scsConnected)) {
            OnWrite(SockHnd, SockEvent);
          }
          if (Events & EPOLLPRI) {
            OnOob(SockHnd, SockEvent); }
          if (Events & EPOLLERR) {
            OnError(SockHnd, SockEvent, TSock::GetSockErr(SockHnd)); }			// !bn: !!! to je blo not ze pred win32 SockErrReportHnd in processing kodo. a je prov?
          if (Events & EPOLLHUP) {
            // !!! a nas to sploh zanima? not really - dokler podatki so jih beremo, ko jih zmanjka jih je konc. al kako.
          }
        } else {
          OnError(SockHnd, SockEvent, ErrCd);
        }      
      } catch (...){
        SaveToErrLog("Exception from 'switch (EventCd)'");
      }
      SetSockEventActive(SockEventId, false);
      //SetSockActive(SockHnd, false);
    }
}

void TSockSys::DoIO() {
  printf(":: do_io\n");
  while (true) {  // process ALL I/O events, 64+64 at a time, interleaved. that's why DelayedIO.
    struct epoll_event evt[2][64];
    int ret = epoll_wait(LT_epoll_fd, evt[0], 64, 0);
    printf("lt epoll_wait.ret = %d\n", ret);
    
    int eret = epoll_wait(ET_epoll_fd, evt[1], 64, 0);
    printf("et epoll_wait.ret = %d\n", eret);

    if (ret == 0 && eret == 0) break;
    
    for (int i=0;i<ret;i++) DoIOEvent(&evt[0][i]);
    for (int i=0;i<eret;i++) DoIOEvent(&evt[1][i]);
  }
}

void TSockSys::DoTimer(siginfo_t *si) {
  int TimerId = si->si_int;
  if (TSockSys::IsTimer(TimerId)){
    PTimer Timer=TSockSys::GetTimer(TimerId)();
    Timer->IncTicks();
    try {
      Timer->OnTimeOut();
    } catch (...){
      SaveToErrLog("Exception from Timer->OnTimeOut();");
    }
  }
}

void TSockSys::DoResolver(siginfo_t *si) {
  int SockHostHnd = si->si_int;
  
  if (TSockSys::IsSockHost(SockHostHnd)) {
    PSockHost SockHost=TSockSys::GetSockHost(SockHostHnd);
    TSockHostStatus Status=TSockHost::GetStatus(&(SockHost->GAI));
    SockHost->GetFromHostEnt(Status, &(SockHost->GAI));
    DelSockHost(SockHostHnd);
    try {
      OnGetHost(SockHost);
    } catch (...){
      SaveToErrLog("Exception from OnGetHost(SockHost);");
    }
  }
}

void TSockSys::AsyncLoop() {
  bool DelayedIO = false;

  while (true) {
    int ret;
    
    sigset_t ss;
    siginfo_t si;
    sigemptyset(&ss);
    sigaddset(&ss, SIGIO);
    sigaddset(&ss, TimerSignal);
    sigaddset(&ss, ResolverSignal);
    
    if (DelayedIO) {
      struct timespec tsp;
      tsp.tv_sec = tsp.tv_nsec = 0;
      ret = sigtimedwait(&ss, &si, &tsp);
    } else {
      ret = sigwaitinfo(&ss, &si);
    }
    
    if (ret == -1) {
      if (errno == EINTR) continue;
      if (errno == EAGAIN) { DelayedIO = false; DoIO(); }
    } else {
      if (si.si_signo == SIGIO) {
        DelayedIO = true;
      } else if (si.si_signo == TimerSignal) {
        DoTimer(&si);
      } else if (si.si_signo == ResolverSignal) {
        DoResolver(&si);
      }
    }
  }

}

#endif

void TSockSys::AddSock(
 const int& SockId, const TSockHnd& SockHnd, const int& SockEventId){
  IAssert(Active);
  SockIdToHndH.AddDat(SockId, SockHnd);
  SockHndToIdH.AddDat(SockHnd, SockId);
  SockHndToEventIdH.AddDat(SockHnd, SockEventId);
#ifdef GLib_UNIX
  SockHndToStateH.AddDat(SockHnd, scsCreated);
  SetupFAsync(SockHnd);
#endif
}

void TSockSys::DelSock(const int& SockId){
  IAssert(Active);
  TSockHnd SockHnd=TSockHnd(SockIdToHndH.GetDat(SockId));
  IAssert(!IsSockActive(SockHnd));
  // delete socket entries
  SockIdToHndH.DelKey(SockId);
  SockHndToIdH.DelKey(SockHnd);
  SockHndToEventIdH.DelKey(SockHnd);
#ifdef GLib_UNIX
  SockHndToStateH.DelKey(SockHnd);
#endif
  // kill associated timer if exists
  DelIfSockTimer(SockId);
}

#ifdef GLib_UNIX
// !!! TODO: prever za vse kar si splitnu zarad SOCKET_ERROR - a je v vseh primerih -1 to ? kaj je to na windowsih?
//           v tem primeru bi blo upraviceno tole generalno nardit pa dat mir.
#define SOCKET_ERROR -1

// !bn: GLib_UNIX -> OnReadOrClose() :)
// "all subsequent reads will return -1" -> prvic ko recv() vrne 0, mormo klicat onclose()
//  - kje sm ze to brau?
// eniwejz. al tko, al pa dobimo ENOTCONN. druzga ze ne more bit.
#endif

void TSockSys::OnRead(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  TMem Mem(MxSockBfL);
  char* Bf=new char[MxSockBfL]; int BfL;
  do {
    BfL=recv(SockHnd, Bf, MxSockBfL, 0);
    if (BfL!=SOCKET_ERROR){
      Mem.AddBf(Bf, BfL); SockBytesRead+=BfL;}
  } while ((BfL>0)&&(BfL!=SOCKET_ERROR));
#ifdef GLib_UNIX
  bool Closed = (BfL == 0);
#endif
  delete[] Bf;
  if (Closed && !SockEvent.Empty()){
    PSIn SIn=Mem.GetSIn();
    SockEvent->OnRead(int(GetSockId(SockHnd)), SIn);
  }
}

void TSockSys::OnWrite(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  if (!SockEvent.Empty()){
    SockEvent->OnWrite(int(GetSockId(SockHnd)));}
}

void TSockSys::OnOob(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  if (!SockEvent.Empty()){
    SockEvent->OnOob(int(GetSockId(SockHnd)));}
}

void TSockSys::OnAccept(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  PSock AccSock=TSock::Accept(SockHnd, SockEvent);
  if (AccSock() == NULL) return;					// !bn: discard async networks errors; don't die on them.
  if (!SockEvent.Empty()){
    SockEvent->OnAccept(AccSock->GetSockId(), AccSock);}
}

void TSockSys::OnConnect(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  if (!SockEvent.Empty()){
    SockEvent->OnConnect(int(GetSockId(SockHnd)));}
}

void TSockSys::OnClose(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  if (!SockEvent.Empty()){
    SockEvent->OnClose(int(GetSockId(SockHnd)));}
}

void TSockSys::OnTimeOut(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  if (!SockEvent.Empty()){
    SockEvent->OnTimeOut(int(GetSockId(SockHnd)));}
}

void TSockSys::OnError(
 const TSockHnd& SockHnd, const PSockEvent& SockEvent, const int& ErrCd){
  if (!SockEvent.Empty()){
    SockEvent->OnError(int(GetSockId(SockHnd)), ErrCd, GetErrStr(ErrCd));}
}

void TSockSys::OnGetHost(const PSockHost& SockHost){
  if (IsSockEvent(SockHost->GetSockEventId())){
    PSockEvent SockEvent=SockHost->GetSockEvent();
    if (!SockEvent.Empty()){
      SockEvent->OnGetHost(SockHost);}
  }
}

TStr TSockSys::GetStatusStr(){
  TChA ChA;
  ChA+="Sockets: "; ChA+=TInt::GetStr(SockIdToHndH.Len()); ChA+="\r\n";
  ChA+="Host-Resolutions: "; ChA+=TInt::GetStr(HndToSockHostH.Len()); ChA+="\r\n";
  ChA+="Socket-Events: "; ChA+=TInt::GetStr(IdToSockEventH.Len()); ChA+="\r\n";
  ChA+="Report-Events: "; ChA+=TInt::GetStr(IdToReportEventH.Len()); ChA+="\r\n";
  ChA+="Timers: "; ChA+=TInt::GetStr(IdToTimerH.Len()); ChA+="\r\n";
  return ChA;
}

/////////////////////////////////////////////////
// Socket-Event
int TSockEvent::LastSockEventId=0;

TSockEvent::~TSockEvent(){
  IAssert(!TSockSys::IsSockEventActive(SockEventId));
}

bool TSockEvent::IsReg(const PSockEvent& SockEvent){
  return TSockSys::IsSockEvent(SockEvent);
}

void TSockEvent::Reg(const PSockEvent& SockEvent){
  IAssert(!TSockSys::IsSockEvent(SockEvent));
  TSockSys::AddSockEvent(SockEvent);
}

void TSockEvent::UnReg(const PSockEvent& SockEvent){
  IAssert(TSockSys::IsSockEvent(SockEvent));
  TSockSys::DelSockEvent(SockEvent);
}

/////////////////////////////////////////////////
// Socket-Host

int TSockHost::LastSockHostId = 0;

#ifdef GLib_WIN32
void TSockHost::GetFromHostEnt(
 const TSockHostStatus& _Status, const hostent* HostEnt){
  if ((Status=_Status)==shsOk){
    IAssert(HostEnt!=NULL);
    IAssert(HostEnt->h_addrtype==AF_INET);
    IAssert(HostEnt->h_length==4);
    HostNmV.Add(TStr(HostEnt->h_name).GetLc());
    int HostNmN=0;
    while (HostEnt->h_aliases[HostNmN]!=NULL){
      HostNmV.Add(TStr(HostEnt->h_aliases[HostNmN]).GetLc()); HostNmN++;}
    int IpNumN=0;
    while (HostEnt->h_addr_list[IpNumN]!=NULL){
      TStr IpNum=
       TInt::GetStr(uchar(HostEnt->h_addr_list[IpNumN][0]))+"."+
       TInt::GetStr(uchar(HostEnt->h_addr_list[IpNumN][1]))+"."+
       TInt::GetStr(uchar(HostEnt->h_addr_list[IpNumN][2]))+"."+
       TInt::GetStr(uchar(HostEnt->h_addr_list[IpNumN][3]));
      IpNumV.Add(IpNum); IpNumN++;
    }
  }
}
#elif defined(GLib_UNIX)
// !!! tole nalozi TSockHost iz addrinfo strukture (hostent na starih unixih in win32)
// !!! zato je TSockSys nas frend - da lahko zahteva da se nalozimo iz async odgovora.
// addrinfa ni treba posiljat, zato ker ga async dobimo direkt v nas gaicb
void TSockHost::GetFromHostEnt(
 const TSockHostStatus& _Status, gaicb *gcb) {
  if ((Status=_Status)==shsOk){
    // !!! TODO !bn: implement load from addrinfo.
    if ((Status=_Status)==shsOk) {
      IAssert(gcb!=NULL);
      addrinfo *ai = gcb->ar_result;
      
      // !!! kaj pa ce host nima imena? v vsakem primeru v name vtaknemo query. drgac bi biu lahko cist prazn.
      HostNmV.Add(gcb->ar_name);
      //HostNmV.Add(HNm);
      free((char *)gcb->ar_name);
      
      while (ai) {
        if (ai->ai_canonname) HostNmV.Add(ai->ai_canonname);
        if (ai->ai_family == PF_INET) {
          uchar *ia = (uchar*)&(((struct sockaddr_in*)ai->ai_addr)->sin_addr.s_addr);
          // !!! tole je pa narobe. sej je v network-order zapisan ip?
          TStr IpNum=
           TInt::GetStr(ia[0])+"." + TInt::GetStr(ia[1])+"."+
           TInt::GetStr(ia[2])+"." + TInt::GetStr(ia[3]);
          IpNumV.Add(IpNum);
          }
        addrinfo *oai = ai;  
        ai = ai->ai_next;
        freeaddrinfo(oai);
      }
    }
  }
}
#endif

PSockEvent TSockHost::GetSockEvent() const {
  return TSockSys::GetSockEvent(SockEventId);
}

bool TSockHost::IsIpNum(const TStr& HostNm){
  int HostNmLen=HostNm.Len();
  for (int ChN=0; ChN<HostNmLen; ChN++){
    if (TCh::IsAlpha(HostNm[ChN])){return false;}}
  return true;
}

TStr TSockHost::GetIpNum(const uint& IpNum){
  TChA IpNumChA;
  IpNumChA+=TUInt::GetStr(IpNum/0x1000000);
  IpNumChA+='.'; IpNumChA+=TUInt::GetStr((IpNum/0x10000)%0x100);
  IpNumChA+='.'; IpNumChA+=TUInt::GetStr((IpNum/0x100)%0x100);
  IpNumChA+='.'; IpNumChA+=TUInt::GetStr(IpNum%0x100);
  return IpNumChA;
}

#ifdef GLib_WIN32
PSockHost TSockHost::GetSyncSockHost(const TStr& HostNm){
  hostent* HostEnt; TSockHostStatus Status(shsUndef);
  if ((HostNm.Len()>0)&&(!IsIpNum(HostNm))){
    HostEnt=gethostbyname(HostNm.CStr());
    if (HostEnt==NULL){Status=GetStatus(WSAGetLastError());}
    else {Status=shsOk;}
  } else {
    uint HostIpNum=inet_addr(HostNm.CStr());
    if (HostIpNum==INADDR_NONE){
      Status=shsError; HostEnt=NULL;
    } else {
      HostEnt=gethostbyaddr((char*)&HostIpNum, 4, AF_INET);
      if (HostEnt==NULL){Status=GetStatus(WSAGetLastError());}
      else {Status=shsOk;}
    }
  }
  PSockHost SockHost=PSockHost(new TSockHost());
  SockHost->GetFromHostEnt(Status, HostEnt);
  return SockHost;
}

void TSockHost::GetAsyncSockHost(
 const TStr& HostNm, const PSockEvent& SockEvent){
  PSockHost SockHost=PSockHost(new TSockHost(SockEvent));
  HANDLE SockHostHnd=0;
//  if ((HostNm.Len()>0)&&(!IsIpNum(HostNm))){
  if ((HostNm.Len()>0)){
    SockHostHnd=WSAAsyncGetHostByName(TSockSys::GetDnsWndHnd(),
     TSockSys::GetDnsMsgHnd(), HostNm.CStr(),
     SockHost->HostEntBf, MAXGETHOSTSTRUCT);
  } else {
    uint HostIpNum=inet_addr(HostNm.CStr());
    if (HostIpNum==INADDR_NONE){
      SockHostHnd=0;
    } else {
      SockHostHnd=WSAAsyncGetHostByAddr(TSockSys::GetDnsWndHnd(),
       TSockSys::GetDnsMsgHnd(), (char*)&HostIpNum, 4, AF_INET,
       SockHost->HostEntBf, MAXGETHOSTSTRUCT);
    }
  }
  EAssertR(SockHostHnd!=0, TSockSys::GetErrStr(WSAGetLastError()));
  if (SockHostHnd!=0){
    TSockSys::AddSockHost(TUInt64(SockHostHnd), SockHost);
  }
}

TSockHostStatus TSockHost::GetStatus(const int& ErrCd){
  switch (ErrCd){
    case 0: return shsOk;
    case WSAHOST_NOT_FOUND: return shsHostNotFound;
    case WSATRY_AGAIN: return shsTryAgain;
    default: return shsError;
  }
}

TSockHost::~TSockHost(){}

#elif defined(GLib_UNIX)

TSockHostStatus TSockHost::GetStatus(gaicb *gcb){
  switch (gai_error(gcb)){
    case 0: return shsOk;
    case EAI_NONAME: return shsHostNotFound;
    case EAI_AGAIN: return shsTryAgain;
    case EAI_INPROGRESS: return shsInProgress;
    default: return shsError;
  }
}

TStr TSockHost::GetGaiErrStr(const int ErrCd) {
/*  switch (ErrCd) {
    case EAI_AGAIN: return "The name could not be resolved at this time.";
    case EAI_BADFLAGS: return "The flags had an invalid value.";
    case EAI_FAIL: return "A non-recoverable error occured.";
    case EAI_FAMILY: return "The address family was not recognized or the address length was invalid.";
    case EAI_MEMORY: return "Memory allocation failure.";
    case EAI_NONAME: return "The name does not resolve for the supplied parameters.";
    case EAI_SERVICE: return "The service passed was not recognized for the specified socket type.";
    case EAI_INPROGRESS: return "The asynchronous lookup has not yet finished.";
    case EAI_INTR: return "The operation was interrupted by a signal.";
    case EAI_CANCELED: return "The asynchronous request was canceled.";
    case EAI_NOTCANCELED: return "The asynchronous request was not canceled.";
    case EAI_ALLDONE: return "Nothing had to be done.";
    case EAI_SYSTEM: return TSockSys::GetErrStr(errno);
    default: return "Unknown error.";
  }
*/
  return gai_strerror(ErrCd);
}

TSockHostStatus TSockHost::SubmitQuery(const TStr &HostStr, gaicb *gcb, addrinfo *request, bool sync, uint32 EventId) {
  sigevent ev;
  if (!sync) {
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = TSockSys::ResolverSignal;
    ev.sigev_value.sival_int = EventId;
    ev.sigev_notify_attributes = NULL;
  }
  
  memset(gcb, 0, sizeof(*gcb));
  memset(request, 0, sizeof(*request));
  gcb->ar_name = strdup(HostStr.CStr());			// !!! TODO: pocistt za tem. oz pocistt sploh za vsemskup. like v GetFromHostEnt je treba tole sprostit in sprostit gcb result.
//  HNm = HostStr;

// !!! TODO !!! MEMORY LEAK !!!
// ne res. ar_name je const. submitquery je pa static. torej ne more spreminjat objektov.

// !!!!!!!!!!!!!!!!!!!!! nekak freeji result. :)

  //gcb->ar_name = HostStr.CStr();
  if (gcb->ar_name == NULL) return shsError;		//           v destruktorju pazi .. ce si naredu submit, si mogu tud pobrat rezultate. ce je pa outstanding, mors pa vsaj skenslat in
  gcb->ar_service = NULL;				//           deregistrirat, drugac bos v velki p***. vbistvu mors PREJ unregister nardit predn cekiras ce si spucu za sabo,
  gcb->ar_request = request;				//           drugac med pucanjem lahk pride rezultat not, pa mas leak.
  gcb->ar_result = NULL;
  /*if (IsIpNm(HostStr))*/
  request->ai_flags = AI_CANONNAME;
  request->ai_family = PF_INET;
//  gcb->ar_request->ai_flags = AI_CANONNAME;		//         !!!   ce se odlocas o completionu na podlagi Status polja, preglej ce pravilno povsod vpises to polje.
//  gcb->ar_request->ai_family = PF_INET;
  gcb->ar_request = request;

  gaicb *gca[1] = { gcb };
  int ret = getaddrinfo_a(sync?GAI_WAIT:GAI_NOWAIT, gca, 1, sync?NULL:&ev);
  if (ret == 0) { 
    return shsOk;
  } else {
    return GetStatus(gcb);
  }
}

PSockHost TSockHost::GetSyncSockHost(const TStr& HostStr){
  gaicb gcb; addrinfo ai; TSockHostStatus Status(shsUndef);
  Status = SubmitQuery(HostStr, &gcb, &ai);
  PSockHost SockHost=PSockHost(new TSockHost());
  SockHost->GetFromHostEnt(Status, &gcb);
  return SockHost;
}

void TSockHost::GetAsyncSockHost(
 const TStr& HostStr, const PSockEvent& SockEvent){
  PSockHost SockHost=PSockHost(new TSockHost(SockEvent));
  SockHost->Status = shsInProgress;
  TSockHostStatus Status = SubmitQuery(HostStr, &(SockHost->GAI), &(SockHost->request), false, SockHost->SockHostId);
  EAssertR(Status == shsOk, TSockHost::GetGaiErrStr(gai_error(&(SockHost->GAI))));
  if (Status == shsOk){
    TSockSys::AddSockHost(SockHost->SockHostId, SockHost);
  } else {
    SockHost->Status = Status;			// need this for the destructor.
  }
}

TSockHost::~TSockHost() {
  if (Status == shsInProgress) {
    TSockSys::DelSockHost(SockHostId);			// we don't want an async result to happen if we don't exist anymore. do we?
    int ret;
    do {
      ret = gai_cancel(&GAI);		// !!! problem. tole lahko vrne 'glih dugaja'. ce izmaknemo objekt, bo stala.
    } while (ret == EAI_NOTCANCELED);			// !!! ni lepo da se vrtimo v zanki, ampak objekta NE SMEMO BRISAT.
    addrinfo *ai = GAI.ar_result;
    while (ai) {
      addrinfo *oai = ai;
      ai = ai->ai_next;
      freeaddrinfo(oai);
    }
    free((char*)GAI.ar_name);				// it was allocated once and never cleaned up by AI->SH
  }							// !!! povozil smo const kvalifikator. upam da se svet ne bo podru. :)
}
#endif

PSockHost TSockHost::GetLocalSockHost(){
  PSockHost SockHost=TSockHost::GetSyncSockHost(LocalHostNm);
  if (SockHost->IsOk()){
    SockHost=TSockHost::GetSyncSockHost(SockHost->GetHostNm());}
  return SockHost;
}

const TStr TSockHost::LocalHostNm("localhost");

/////////////////////////////////////////////////
// Socket
int TSock::LastSockId=0;

TSock::TSock(const PSockEvent& SockEvent):
  SockId(++LastSockId), SockHnd(0),
  SockEventId(SockEvent->GetSockEventId()){
  SockHnd=socket(PF_INET, SOCK_STREAM, 0);		// !bn: changed. was: AF_INET. it is the same, for now. documentation says PF_INET.
#ifdef GLib_WIN32
  EAssertR(SockHnd!=INVALID_SOCKET, TSockSys::GetErrStr(WSAGetLastError()));
#elif defined(GLib_UNIX)
  EAssertR(SockHnd!=-1, TSockSys::GetErrStr(TSock::GetSockErr(SockHnd)));
#endif
  TSockSys::AddSock(SockId, SockHnd, SockEventId);
  IAssert(TSockEvent::IsReg(SockEvent));
}

TSock::TSock(const TSockHnd& _SockHnd, const PSockEvent& SockEvent):
  SockId(++LastSockId), SockHnd(_SockHnd),
  SockEventId(SockEvent->GetSockEventId()){
  TSockSys::AddSock(SockId, SockHnd, SockEventId);
  IAssert(TSockEvent::IsReg(SockEvent));
}

TSock::~TSock(){
  IAssert(!TSockSys::IsSockActive(SockHnd));
  TSockSys::DelSock(SockId);
#ifdef GLib_WIN32
  closesocket(SockHnd);
#elif defined(GLib_UNIX)
  close(SockHnd);
#endif
  //EAssertR(closesocket(SockHnd)==0, TSockSys::GetErrStr(WSAGetLastError()));
}

PSockEvent TSock::GetSockEvent() const {
  return TSockSys::GetSockEvent(SockEventId);
}

int TSock::GetSockErr(const TSockHnd s) {
  int err;
  socklen_t sz = sizeof(err);
  getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &sz);
  return err;
}

void TSock::Listen(const int& PortN){
  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
  SockAddr.sin_family=AF_INET;
#ifdef GLib_WIN32
  SockAddr.sin_addr.s_addr=INADDR_ANY;
#elif defined(GLib_UNIX)
  SockAddr.sin_addr.s_addr=htonl(INADDR_ANY);		// !bn: i think this is the 'correct' formulation. INADDR_ANY is 0 anyway, but perhaps might not remain so forever
#endif
  SockAddr.sin_port=htons(u_short(PortN));
#ifdef GLib_WIN32
  EAssertR(
   bind(SockHnd, (sockaddr*)&SockAddr, sizeof(SockAddr))==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  EAssertR(
   WSAAsyncSelect(SockHnd, TSockSys::GetSockWndHnd(),
    TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  EAssertR(
   listen(SockHnd, SOMAXCONN)==0,
   TSockSys::GetErrStr(WSAGetLastError()));
#elif defined(GLib_UNIX)
  // !!! copypaste :/ [almost]
  //     replace WSAGetLastError() with getsockopt() - returns per-socket error 
  //     instead per-process/thread error which might be already invalid.
  EAssertR(
   bind(SockHnd, (sockaddr*)&SockAddr, sizeof(SockAddr))==0,
   TSockSys::GetErrStr(GetSockErr(SockHnd)));
  // !!! why was there a select here?
  EAssertR(
   listen(SockHnd, SOMAXCONN)==0,
   TSockSys::GetErrStr(GetSockErr(SockHnd)));
  TSockSys::SockHndToStateH[SockHnd] = scsListening;
#endif
}

int TSock::GetPortAndListen(const int& MnPortN){
  int PortN=MnPortN-1;
  int ErrCd=0;
  forever {
    PortN++;
    sockaddr_in SockAddr;
    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family=AF_INET;
#ifdef GLib_WIN32
    SockAddr.sin_addr.s_addr=INADDR_ANY;
#elif defined(GLib_UNIX)
    SockAddr.sin_addr.s_addr=htonl(INADDR_ANY);
#endif
    SockAddr.sin_port=htons(u_short(PortN));
    int OkCd=bind(SockHnd, (sockaddr*)&SockAddr, sizeof(SockAddr));
#ifdef GLib_WIN32
    if (OkCd==SOCKET_ERROR){
      ErrCd=WSAGetLastError();
      if (ErrCd!=WSAEADDRINUSE){break;}
    } else {
      ErrCd=0; break;
    }
#elif defined(GLib_UNIX)
    if (OkCd==-1){
      ErrCd=GetSockErr(SockHnd);
      if (ErrCd!=EACCES){break;}	// !!! !bn: a res bind vrne EACCESS ce ne more bindat?
    } else {
      ErrCd=0; break;
    }
#endif
  }
  EAssertR(ErrCd==0, TSockSys::GetErrStr(ErrCd));
#ifdef GLib_WIN32
  EAssertR(
   WSAAsyncSelect(SockHnd, TSockSys::GetSockWndHnd(),
   TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  EAssertR(
   listen(SockHnd, SOMAXCONN)==0,
   TSockSys::GetErrStr(WSAGetLastError()));
#elif defined(GLib_UNIX)
  // !!! copypaste because of wsagetlasterror.
  EAssertR(
   listen(SockHnd, SOMAXCONN)==0,
   TSockSys::GetErrStr(GetSockErr(SockHnd)));
#endif
  return PortN;
}

void TSock::Connect(const PSockHost& SockHost, const int& PortN){
  IAssert(SockHost->IsOk());
#ifdef GLib_WIN32
  uint HostIpNum=inet_addr(SockHost->GetIpNum().CStr());
  IAssert(HostIpNum!=INADDR_NONE);
#elif defined(GLib_UNIX)
  uint HostIpNum;
  IAssert(inet_aton(SockHost->GetIpNum().CStr(), (in_addr*)&HostIpNum) == 0);	// !!! prov typecast?
#endif

  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
  memcpy(&(SockAddr.sin_addr), &HostIpNum, sizeof(HostIpNum));
  SockAddr.sin_family=AF_INET;
  SockAddr.sin_port=htons(u_short(PortN));

#ifdef GLib_WIN32
  EAssertR(
   WSAAsyncSelect(SockHnd, TSockSys::GetSockWndHnd(),
    TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));
#endif

  int ErrCd=connect(SockHnd, (sockaddr*)&SockAddr, sizeof(SockAddr));
  EAssertR(
#ifdef GLib_WIN32
   (ErrCd==SOCKET_ERROR)&&(WSAGetLastError()==WSAEWOULDBLOCK),
#elif defined(GLib_UNIX)
    // !!! !bn: tole je pa resno: na errno se ne gre zanasat ker ni mt-safe, ampak v errno-ju se najde EINPROGRESS. socket je pa lahko ze sconnectan do takrat.
    // a je tale pogoj - EINPROGRESS || 0 uredu?
   ((ErrCd==-1)&&(GetSockErr(SockHnd)==EINPROGRESS))||(GetSockErr(SockHnd) == 0),
#endif
   "Unsuccessful socket-connect.");
}

void TSock::Send(const PSIn& SIn, bool& Ok, int& ErrCd){
  if (!SIn.Empty()){UnsentBf+=SIn;}
  Ok=true; ErrCd=0;
  int SentChs=0;
  while (SentChs<UnsentBf.Len()){
    int SendBfL=UnsentBf.Len()-SentChs;
    if (SendBfL>TSockSys::MxSockBfL){SendBfL=TSockSys::MxSockBfL;}
    int LSentChs=send(SockHnd, &UnsentBf[SentChs], SendBfL, 0);
#ifdef GLib_WIN32
    if (LSentChs==SOCKET_ERROR){
      ErrCd=WSAGetLastError();
      Ok=(ErrCd==WSAEWOULDBLOCK);
#elif defined(GLib_UNIX)
    if (LSentChs==-1){
      ErrCd=GetSockErr(SockHnd);
      Ok=(ErrCd==EWOULDBLOCK);
#endif
      break;
    } else {
      SentChs+=LSentChs;
      TSockSys::SockBytesWritten+=LSentChs;
    }
  }
  UnsentBf.Del(0, SentChs-1);
}

void TSock::Send(const PSIn& SIn){
  bool Ok; int ErrCd; Send(SIn, Ok, ErrCd);
#ifdef GLib_WIN32
  if (!Ok){
   ESAssert(PostMessage(
    TSockSys::GetSockWndHnd(), TSockSys::SockErrMsgHnd, SockHnd, ErrCd));
  }
#elif defined(GLib_UNIX)
  if (!Ok){	// !!! !bn: why does win32 code post the error message instead of calling errorhandler?
                //      i don't want to do it by writing (sockhnd, errcd) in a pipe, because of a possible race condition (sockhnd is not unique)
    GetSockEvent()->OnError(SockId, ErrCd, TSockSys::GetErrStr(ErrCd));
  }
#endif
}

void TSock::SendSafe(const PSIn& SIn){
  bool Ok; int ErrCd; Send(SIn, Ok, ErrCd);
}

TStr TSock::GetPeerIpNum() const {
  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
#ifdef GLib_WIN32
  int NmLen=sizeof(sockaddr_in);
#elif defined(GLib_UNIX)
  socklen_t NmLen=sizeof(sockaddr_in);
#endif

  EAssertR(
   getpeername(SockHnd, (sockaddr*)&SockAddr, &NmLen)==0,
#ifdef GLib_WIN32
   TSockSys::GetErrStr(WSAGetLastError()));
  TStr IpNum=
   TInt::GetStr(SockAddr.sin_addr.s_net)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_host)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_lh)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_impno);
#elif defined(GLib_UNIX)
   TSockSys::GetErrStr(GetSockErr(SockHnd)));
   uchar *s_addr = (uchar*)&SockAddr.sin_addr.s_addr;
  TStr IpNum=
   TInt::GetStr(s_addr[4])+"."+		// !!! like .. check. a ma kakrsnkol smisu tole.
   TInt::GetStr(s_addr[3])+"."+
   TInt::GetStr(s_addr[2])+"."+
   TInt::GetStr(s_addr[1]);
#endif
  return IpNum;
}

TStr TSock::GetLocalIpNum() const {
  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
#ifdef GLib_WIN32
  int NmLen=sizeof(sockaddr_in);
#elif defined(GLib_UNIX)
  socklen_t NmLen=sizeof(sockaddr_in);
#endif

  EAssertR(
   getsockname(SockHnd, (sockaddr*)&SockAddr, &NmLen)==0,
#ifdef GLib_WIN32
   TSockSys::GetErrStr(WSAGetLastError()));
  TStr IpNum=
   TInt::GetStr(SockAddr.sin_addr.s_net)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_host)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_lh)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_impno);
#elif defined(GLib_UNIX)
   TSockSys::GetErrStr(GetSockErr(SockHnd)));
   uchar *s_addr = (uchar*)&SockAddr.sin_addr.s_addr;
  TStr IpNum=
   TInt::GetStr(s_addr[4])+"."+		// !!! like .. check. a ma kakrsnkol smisu tole.
   TInt::GetStr(s_addr[3])+"."+
   TInt::GetStr(s_addr[2])+"."+
   TInt::GetStr(s_addr[1]);
#endif
  return IpNum;
}


void TSock::PutTimeOut(const int& MSecs){
  TSockSys::AddSockTimer(SockId, MSecs);
}

void TSock::DelTimeOut(){
  TSockSys::DelIfSockTimer(SockId);
}

PSock TSock::Accept(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  sockaddr_in SockAddr;
#ifdef GLib_WIN32
  int SockAddrLen=sizeof(SockAddr);
#elif defined(GLib_UNIX)
  socklen_t SockAddrLen=sizeof(SockAddr);
#endif
  memset(&SockAddr, 0, sizeof(SockAddr));

  TSockHnd AccSockHnd=accept(SockHnd, (sockaddr*)&SockAddr, &SockAddrLen);
#ifdef GLib_WIN32
  EAssertR(
   AccSockHnd!=INVALID_SOCKET,
   TSockSys::GetErrStr(WSAGetLastError()));
  PSock AccSock=PSock(new TSock(AccSockHnd, SockEvent));

  EAssertR(
   WSAAsyncSelect(AccSock->GetSockHnd(), TSockSys::GetSockWndHnd(),
    TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  return AccSock;
#elif defined(GLib_UNIX)
  if (AccSockHnd == -1) {
    // !!! !bn: a bi rajs GetSockErr()?
    if ((errno == EWOULDBLOCK) || (errno == ECONNABORTED) || (errno == EINTR)) return NULL;	// !!! ECONNABORTED -> just return NULL.
    EAssertR(						// !!! !bn: overkill. ret==-1 se lahko velikrat zgodi, mogl bi sam preprost ignorirat take evente pa vrnt null.
     AccSockHnd!=-1,
     TSockSys::GetErrStr(GetSockErr(SockHnd)));
    return NULL;
  } else {
    PSock AccSock=PSock(new TSock(AccSockHnd, SockEvent));
    TSockSys::SockHndToStateH[AccSock->SockHnd] = scsConnected;
    return AccSock;
  }
#endif
}

TStr TSock::GetSockSysStatusStr(){
  return TSockSys::GetStatusStr();
}

uint64 TSock::GetSockSysBytesRead(){
  return TSockSys::SockBytesRead;
}

uint64 TSock::GetSockSysBytesWritten(){
  return TSockSys::SockBytesWritten;
}

bool TSock::IsSockId(const int& SockId){
  return TSockSys::IsSockId(SockId);
}

/////////////////////////////////////////////////
// Report-Event
int TReportEvent::LastReportEventId=0;

void TReportEvent::SendReport(){
#ifdef GLib_WIN32
  TSockSys::AddReportEvent(this);
  ESAssert(PostMessage(
   TSockSys::GetReportWndHnd(), TSockSys::GetReportMsgHnd(), 0, ReportEventId));
#elif defined(GLib_UNIX)
  Fail;		// !!! not implemented. [obsolete?]
#endif
}

/////////////////////////////////////////////////
// Timer
int TTTimer::LastTimerId=0;

TTTimer::TTTimer(const int& _TimeOut):
  TimerId(++LastTimerId),
#ifdef GLib_WIN32
  TimerHnd(0),
#endif
  TimeOut(_TimeOut),
  Ticks(0), StartTm(TSecTm::GetCurTm())
{
#ifdef GLib_UNIX
  sigevent evp;
  evp.sigev_notify = SIGEV_SIGNAL;
  evp.sigev_signo = TSockSys::TimerSignal;
  evp.sigev_value.sival_int = TimerId;
  evp.sigev_notify_attributes = NULL;

  IAssert(timer_create(CLOCK_MONOTONIC, &evp, &TimerHnd) == 0);
#endif
  IAssert(TimeOut>=0);
  StartTimer(TimeOut);
}

TTTimer::~TTTimer(){
  StopTimer();
#ifdef GLib_UNIX
  timer_delete(TimerHnd);
#endif
}

void TTTimer::StartTimer(const int& _TimeOut){
  IAssert((_TimeOut==-1)||(_TimeOut>=0));
  // if _TimeOut==-1 use previous TimeOut
  if (_TimeOut!=-1){
    TimeOut=_TimeOut;}
  // stop current-timer
  StopTimer();
  if (TimeOut>0){
#ifdef GLib_WIN32
    TimerHnd=uint(SetTimer(
     TSockSys::GetTimerWndHnd(), UINT(TimerId), UINT(TimeOut), NULL));
    ESAssert(TimerHnd!=0);
#elif defined(GLib_UNIX)
    itimerspec its;
    its.it_value.tv_sec = its.it_interval.tv_sec = TimeOut / 1000;
    its.it_value.tv_nsec = its.it_interval.tv_nsec = (TimeOut % 1000) * 1000000;
    EAssertR(timer_settime(TimerHnd, 0, &its, NULL) == 0, TSockSys::GetErrStr(errno));			// !!! TODO: check: ce je sigpending, pa ze pobrisemo timer pripadajoc. a prevermo ce smemo brskat?
#endif
    TSockSys::AddTimer(this);
  }
}

void TTTimer::StopTimer(){
  if (TimerHnd!=0){
#ifdef GLib_WIN32
    ESAssert(KillTimer(TSockSys::GetTimerWndHnd(), TimerId));
#elif defined(GLib_UNIX)
    itimerspec its;
    its.it_value.tv_sec = its.it_interval.tv_sec = 0;
    its.it_value.tv_nsec = its.it_interval.tv_nsec = 0;
    EAssertR(timer_settime(TimerHnd, 0, &its, NULL) == 0, TSockSys::GetErrStr(errno));
#endif
    TSockSys::DelTimer(TimerId);
    TimerHnd=0;
  }
}

#ifdef GLib_UNIX
void TSocketTimer::OnTimeOut() {
  TSockHnd SockHnd=TSockSys::GetSockHnd(SockId);
  int SockEventId=TSockSys::GetSockEventId(SockHnd);
  PSockEvent SockEvent=TSockSys::GetSockEvent(SockEventId);
  TSockSys::SetSockEventActive(SockEventId, true);
  try {
    if (!SockEvent.Empty()) SockEvent->OnTimeOut(int(SockId));
  } catch (...){
    SaveToErrLog("Exception from OnTimeOut(SockHnd, SockEvent);");
  }
  TSockSys::SetSockEventActive(SockEventId, false);
  
  TSockSys::DelIfSockTimer(SockId);
}
#endif
