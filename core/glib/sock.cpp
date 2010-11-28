/////////////////////////////////////////////////
// Socket-System
class TSockSys{
public:
  // socket-system initialized
  static bool Active;
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
  // sockets
  static uint64 SockBytesRead;
  static uint64 SockBytesWritten;
  static THash<TInt, TUInt64> SockIdToHndH;
  static THash<TUInt64, TInt> SockHndToIdH;
  static THash<TUInt64, TInt> SockHndToEventIdH;
  static TIntH SockTimerIdH;
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
  // main message handler
  static LRESULT CALLBACK MainWndProc(
   HWND WndHnd, UINT Msg, WPARAM wParam, LPARAM lParam);
public:
  TSockSys();
  ~TSockSys();

  TSockSys& operator=(const TSockSys&){Fail; return *this;}

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
  static void AddSockTimer(const int& SockId, const int& MSecs){
    UINT ErrCd=(UINT)SetTimer(GetSockWndHnd(), SockId, uint(MSecs), NULL);
    ESAssert(ErrCd!=0);
    SockTimerIdH.AddKey(SockId);}
  static void DelIfSockTimer(const int& SockId){
    KillTimer(GetSockWndHnd(), SockId);
    SockTimerIdH.DelIfKey(SockId);}
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
// sockets
uint64 TSockSys::SockBytesRead=0;
uint64 TSockSys::SockBytesWritten=0;
THash<TInt, TUInt64> TSockSys::SockIdToHndH;
THash<TUInt64, TInt> TSockSys::SockHndToIdH;
THash<TUInt64, TInt> TSockSys::SockHndToEventIdH;
TIntH TSockSys::SockTimerIdH;
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
      DelSockHost(SockHostHnd);
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

void TSockSys::AddSock(
 const int& SockId, const TSockHnd& SockHnd, const int& SockEventId){
  IAssert(Active);
  SockIdToHndH.AddDat(SockId, SockHnd);
  SockHndToIdH.AddDat(SockHnd, SockId);
  SockHndToEventIdH.AddDat(SockHnd, SockEventId);
}

void TSockSys::DelSock(const int& SockId){
  IAssert(Active);
  TSockHnd SockHnd=TSockHnd(SockIdToHndH.GetDat(SockId));
  IAssert(!IsSockActive(SockHnd));
  // delete socket entries
  SockIdToHndH.DelKey(SockId);
  SockHndToIdH.DelKey(SockHnd);
  SockHndToEventIdH.DelKey(SockHnd);
  // kill associated timer if exists
  DelIfSockTimer(SockId);
}

/*void TSockSys::OnRead(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  char* Bf=new char[MxSockBfL]; int BfL;
  TMOut MOut(MxSockBfL);
  do {
    BfL=recv(SockHnd, Bf, MxSockBfL, 0);
    if (BfL!=SOCKET_ERROR){
      MOut.PutBf(Bf, BfL); SockBytesRead+=BfL;}
  } while ((BfL>0)&&(BfL!=SOCKET_ERROR));
  if (!SockEvent.Empty()){
    SockEvent->OnRead(int(GetSockId(SockHnd)), MOut.GetSIn());}
  delete[] Bf;
}*/

void TSockSys::OnRead(const TSockHnd& SockHnd, const PSockEvent& SockEvent){
  TMem Mem(MxSockBfL);
  char* Bf=new char[MxSockBfL]; int BfL;
  do {
    BfL=recv(SockHnd, Bf, MxSockBfL, 0);
    if (BfL!=SOCKET_ERROR){
      Mem.AddBf(Bf, BfL); SockBytesRead+=BfL;}
  } while ((BfL>0)&&(BfL!=SOCKET_ERROR));
  delete[] Bf;
  if (!SockEvent.Empty()){
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
  SockHnd=socket(AF_INET, SOCK_STREAM, 0);
  EAssertR(SockHnd!=INVALID_SOCKET, TSockSys::GetErrStr(WSAGetLastError()));
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
  closesocket(SockHnd);
  //EAssertR(closesocket(SockHnd)==0, TSockSys::GetErrStr(WSAGetLastError()));
}

PSockEvent TSock::GetSockEvent() const {
  return TSockSys::GetSockEvent(SockEventId);
}

void TSock::Listen(const int& PortN){
  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
  SockAddr.sin_family=AF_INET;
  SockAddr.sin_addr.s_addr=INADDR_ANY;
  SockAddr.sin_port=htons(u_short(PortN));
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
}

int TSock::GetPortAndListen(const int& MnPortN){
  int PortN=MnPortN-1;
  int ErrCd=0;
  forever {
    PortN++;
    sockaddr_in SockAddr;
    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr=INADDR_ANY;
    SockAddr.sin_port=htons(u_short(PortN));
    int OkCd=bind(SockHnd, (sockaddr*)&SockAddr, sizeof(SockAddr));
    if (OkCd==SOCKET_ERROR){
      ErrCd=WSAGetLastError();
      if (ErrCd!=WSAEADDRINUSE){break;}
    } else {
      ErrCd=0; break;
    }
  }
  EAssertR(ErrCd==0, TSockSys::GetErrStr(ErrCd));
  EAssertR(
   WSAAsyncSelect(SockHnd, TSockSys::GetSockWndHnd(),
   TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  EAssertR(
   listen(SockHnd, SOMAXCONN)==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  return PortN;
}

void TSock::Connect(const PSockHost& SockHost, const int& PortN){
  IAssert(SockHost->IsOk());
  uint HostIpNum=inet_addr(SockHost->GetIpNum().CStr());
  IAssert(HostIpNum!=INADDR_NONE);

  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
  memcpy(&(SockAddr.sin_addr), &HostIpNum, sizeof(HostIpNum));
  SockAddr.sin_family=AF_INET;
  SockAddr.sin_port=htons(u_short(PortN));

  EAssertR(
   WSAAsyncSelect(SockHnd, TSockSys::GetSockWndHnd(),
    TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));

  int ErrCd=connect(SockHnd, (sockaddr*)&SockAddr, sizeof(SockAddr));
  EAssertR(
   (ErrCd==SOCKET_ERROR)&&(WSAGetLastError()==WSAEWOULDBLOCK),
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
    if (LSentChs==SOCKET_ERROR){
      ErrCd=WSAGetLastError();
      Ok=(ErrCd==WSAEWOULDBLOCK);
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
  if (!Ok){
   ESAssert(PostMessage(
    TSockSys::GetSockWndHnd(), TSockSys::SockErrMsgHnd, SockHnd, ErrCd));
  }
}

void TSock::SendSafe(const PSIn& SIn){
  bool Ok; int ErrCd; Send(SIn, Ok, ErrCd);
}

TStr TSock::GetPeerIpNum() const {
  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
  int NmLen=sizeof(sockaddr_in);

  EAssertR(
   getpeername(SockHnd, (sockaddr*)&SockAddr, &NmLen)==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  TStr IpNum=
   TInt::GetStr(SockAddr.sin_addr.s_net)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_host)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_lh)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_impno);
  return IpNum;
}

TStr TSock::GetLocalIpNum() const {
  sockaddr_in SockAddr;
  memset(&SockAddr, 0, sizeof(SockAddr));
  int NmLen=sizeof(sockaddr_in);

  EAssertR(
   getsockname(SockHnd, (sockaddr*)&SockAddr, &NmLen)==0,
   TSockSys::GetErrStr(WSAGetLastError()));
  TStr IpNum=
   TInt::GetStr(SockAddr.sin_addr.s_net)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_host)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_lh)+"."+
   TInt::GetStr(SockAddr.sin_addr.s_impno);
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
  int SockAddrLen=sizeof(SockAddr);
  memset(&SockAddr, 0, sizeof(SockAddr));

  TSockHnd AccSockHnd=accept(SockHnd, (sockaddr*)&SockAddr, &SockAddrLen);
  EAssertR(
   AccSockHnd!=INVALID_SOCKET,
   TSockSys::GetErrStr(WSAGetLastError()));
  PSock AccSock=PSock(new TSock(AccSockHnd, SockEvent));

  EAssertR(
   WSAAsyncSelect(AccSock->GetSockHnd(), TSockSys::GetSockWndHnd(),
    TSockSys::GetSockMsgHnd(), TSockSys::GetAllSockEventCdSet())==0,
   TSockSys::GetErrStr(WSAGetLastError()));

  return AccSock;
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
  TSockSys::AddReportEvent(this);
  ESAssert(PostMessage(
   TSockSys::GetReportWndHnd(), TSockSys::GetReportMsgHnd(), 0, ReportEventId));
}

/////////////////////////////////////////////////
// Timer
int TTTimer::LastTimerId=0;

TTTimer::TTTimer(const int& _TimeOut):
  TimerId(++LastTimerId), TimerHnd(0), TimeOut(_TimeOut),
  Ticks(0), StartTm(TSecTm::GetCurTm()){
  IAssert(TimeOut>=0);
  StartTimer(TimeOut);
}

TTTimer::~TTTimer(){
  StopTimer();
}

void TTTimer::StartTimer(const int& _TimeOut){
  IAssert((_TimeOut==-1)||(_TimeOut>=0));
  // if _TimeOut==-1 use previous TimeOut
  if (_TimeOut!=-1){
    TimeOut=_TimeOut;}
  // stop current-timer
  StopTimer();
  if (TimeOut>0){
    TimerHnd=uint(SetTimer(
     TSockSys::GetTimerWndHnd(), UINT(TimerId), UINT(TimeOut), NULL));
    ESAssert(TimerHnd!=0);
    TSockSys::AddTimer(this);
  }
}

void TTTimer::StopTimer(){
  if (TimerHnd!=0){
    ESAssert(KillTimer(TSockSys::GetTimerWndHnd(), TimerId));
    TSockSys::DelTimer(TimerId);
    TimerHnd=0;
  }
}

