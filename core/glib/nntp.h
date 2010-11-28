/////////////////////////////////////////////////
// Forward
class TNntpClt;

/////////////////////////////////////////////////
// Nntp
class TNntp{
public:
  static const int DfNntpPortN;
};

/////////////////////////////////////////////////
// Nntp-Socket-Event
typedef enum {ncmUndef,
 ncmWaitConnAck, ncmWaitCmAck, ncmEnd} TNntpConnMode;

class TNntpSockEvent: public TSockEvent{
private:
  static const int MxGetHostTrys;
  TNntpClt* NntpClt;
  TStr NntpCmStr;
  TNntpConnMode NntpConnMode;
  int GetHostTrys;
  PSockHost SockHost;
  PSock Sock;
  TMOut SockMOut;
public:
  TNntpSockEvent(TNntpClt* _NntpClt, const TStr& _NntpCmStr):
    TSockEvent(),
    NntpClt(_NntpClt), NntpCmStr(_NntpCmStr),
    NntpConnMode(ncmUndef), GetHostTrys(0),
    SockHost(), Sock(), SockMOut(10000){}
  ~TNntpSockEvent(){}

  TSockEvent& operator=(const TSockEvent&){Fail; return *this;}

  void CloseConn(){SockHost=NULL; Sock=NULL;}

  int GetRespCd(const TStr& RespCrLfLn);
  void SendNntpRq(const TStr& RqStr, const TNntpConnMode& NewNntpConnMode);
  void OnNntpError(const TStr& ErrStr);

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int&){}
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int&);
  void OnClose(const int&);
  void OnTimeOut(const int&);
  void OnError(const int&, const int&, const TStr&);
  void OnGetHost(const PSockHost& _SockHost);
};

/////////////////////////////////////////////////
// Nntp-Client
ClassTP(TNntpClt, PNntpClt)//{
private:
  PNotify Notify;
  TStr SrvHostNm;
  int SrvPortN;
  PSockEvent NntpSockEvent;
  void OnClose();
  UndefDefaultCopyAssign(TNntpClt);
public:
  TNntpClt(const TStr& _SrvHostNm, const PNotify& _Notify=TNotify::StdNotify,
   const int& _SrvPortN=TNntp::DfNntpPortN):
    Notify(_Notify), SrvHostNm(_SrvHostNm), SrvPortN(_SrvPortN){}
  static PNntpClt New(const TStr& SrvHostNm,
   const PNotify& Notify=TNotify::StdNotify,
   const int& SrvPortN=TNntp::DfNntpPortN){
    return new TNntpClt(SrvHostNm, Notify, SrvPortN);}

  TStr GetSrvHostNm() const {return SrvHostNm;}
  int GetSrvPortN() const {return SrvPortN;}

  //
  void GetGroupNmV();

  friend class TNntpSockEvent;
};
