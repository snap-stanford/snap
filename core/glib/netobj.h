/////////////////////////////////////////////////
// Net-Object
ClassTP(TNetObj, PNetObj)//{
protected:
  typedef PNetObj (*TNetObjLoad)(TSIn& SIn);
  typedef TFunc<TNetObjLoad> TNetObjLoadF;
  static THash<TStr, TNetObjLoadF> TypeToLoadFH;
  static bool Reg(const TStr& TypeNm, const TNetObjLoadF& LoadF);
private:
  TInt SockId;
  UndefCopy(TNetObj);
public:
  TNetObj(): SockId(-1){}
  TNetObj(TSIn& SIn): SockId(SIn){}
  virtual ~TNetObj(){}
  static PNetObj Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){
    GetTypeNm(*this).Save(SOut); SockId.Save(SOut);}

  TNetObj& operator=(const TNetObj&){return *this;}
  bool operator==(const TNetObj&){return true;}

  void PutSockId(const int& _SockId){SockId=_SockId;}
  int GetSockId() const {return SockId;}
  virtual TStr GetStr() const {
    return GetTypeNm(*this)+"["+TInt::GetStr(SockId)+"]:";}

  static PSIn GetPckSIn(const PNetObj& NetObj);
  static PNetObj GetPckNetObj(TMOut& MOut, bool& Ok);
};

/////////////////////////////////////////////////
// Net-Acknowledge
class TNetAck: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){return TNetObj::Reg(TTypeNm<TNetAck>(), &Load);}
private:
public:
  TNetAck(): TNetObj(){}
  ~TNetAck(){}
  TNetAck(TSIn& SIn): TNetObj(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetAck(SIn));}
  void Save(TSOut& SOut){TNetObj::Save(SOut);}

  TStr GetStr() const {return TNetObj::GetStr();}
};

/////////////////////////////////////////////////
// Net-Object-Notify
class TNetNotify: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetNotify>(), TNetObjLoadF(&Load));}
private:
  TInt NotifyType;
  TStr NotifyStr;
public:
  TNetNotify(const TNotifyType& _NotifyType, const TStr& _NotifyStr):
    TNetObj(), NotifyType(_NotifyType), NotifyStr(_NotifyStr){}
  ~TNetNotify(){}
  TNetNotify(TSIn& SIn):
    TNetObj(SIn), NotifyType(SIn), NotifyStr(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetNotify(SIn));}
  void Save(TSOut& SOut){
    TNetObj::Save(SOut); NotifyType.Save(SOut); NotifyStr.Save(SOut);}

  TNotifyType GetNotifyType() const {return TNotifyType(int(NotifyType));}
  TStr GetNotifyStr() const {return NotifyStr;}
  TStr GetStr() const {
    TStr NotifyTypeStr=TNotify::GetTypeStr(GetNotifyType());
    return TNetObj::GetStr()+NotifyTypeStr+":"+NotifyStr;}
};

/////////////////////////////////////////////////
// Net-Object-Error
class TNetErr: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetErr>(), TNetObjLoadF(&Load));}
private:
  TInt ErrCd;
  TStr MsgStr;
public:
  TNetErr(const int& _ErrCd, const TStr& _MsgStr):
    TNetObj(), ErrCd(_ErrCd), MsgStr(_MsgStr){}
  ~TNetErr(){}
  TNetErr(TSIn& SIn): TNetObj(SIn), ErrCd(SIn), MsgStr(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetErr(SIn));}
  void Save(TSOut& SOut){
    TNetObj::Save(SOut); ErrCd.Save(SOut); MsgStr.Save(SOut);}

  int GetErrCd() const {return ErrCd;}
  TStr GetMsgStr() const {return MsgStr;}
  TStr GetStr() const {return TNetObj::GetStr()+MsgStr;}
};

/////////////////////////////////////////////////
// Net-Object-Accept
class TNetAccept: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetAccept>(), TNetObjLoadF(&Load));}
private:
  TInt SideSockId;
public:
  TNetAccept(const int& _SideSockId): TNetObj(), SideSockId(_SideSockId){}
  ~TNetAccept(){}
  TNetAccept(TSIn& SIn): TNetObj(SIn), SideSockId(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetAccept(SIn));}
  void Save(TSOut& SOut){TNetObj::Save(SOut); SideSockId.Save(SOut);}

  int GetSideSockId() const {return SideSockId;}
  TStr GetStr() const {return TNetObj::GetStr()+TInt::GetStr(SideSockId);}
};

/////////////////////////////////////////////////
// Net-Object-Connect
class TNetConn: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetConn>(), TNetObjLoadF(&Load));}
public:
  TNetConn(): TNetObj(){}
  ~TNetConn(){}
  TNetConn(TSIn& SIn): TNetObj(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetConn(SIn));}
  void Save(TSOut& SOut){TNetObj::Save(SOut);}
};

/////////////////////////////////////////////////
// Net-Object-Close
class TNetClose: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetClose>(), TNetObjLoadF(&Load));}
public:
  TNetClose(): TNetObj(){}
  ~TNetClose(){}
  TNetClose(TSIn& SIn): TNetObj(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetClose(SIn));}
  void Save(TSOut& SOut){TNetObj::Save(SOut);}
};

/////////////////////////////////////////////////
// Net-Object-TimeOut
class TNetTimeOut: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetTimeOut>(), TNetObjLoadF(&Load));}
private:
  TInt TimerId;
  TInt Ticks;
public:
  TNetTimeOut(const int& _TimerId, const int& _Ticks):
    TNetObj(), TimerId(_TimerId), Ticks(_Ticks){}
  ~TNetTimeOut(){}
  TNetTimeOut(TSIn& SIn): TNetObj(SIn), TimerId(SIn), Ticks(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetTimeOut(SIn));}
  void Save(TSOut& SOut){
    TNetObj::Save(SOut); TimerId.Save(SOut); Ticks.Save(SOut);}

  int GetTimerId() const {return TimerId;}
  int GetTicks() const {return Ticks;}
  TStr GetStr() const {
    return TNetObj::GetStr()+TInt::GetStr(TimerId)+" "+TInt::GetStr(Ticks);}
};

/////////////////////////////////////////////////
// Net-Object-Registration
typedef enum {nrtLogin, nrtConfirm, nrtRefuse, nrtLogout} TNetRegType;

class TNetReg: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetReg>(), TNetObjLoadF(&Load));}
private:
  TInt RegType;
  TStr UsrNm;
  TStr PwdStr;
  TStr InfoStr;
  TStr VerStr;
  TStr MsgStr;
public:
  TNetReg(const int& _RegType, const TStr& _UsrNm, const TStr& _PwdStr,
   const TStr& _InfoStr, const TStr& _VerStr):
    TNetObj(), RegType(_RegType), UsrNm(_UsrNm), PwdStr(_PwdStr),
    InfoStr(_InfoStr), VerStr(_VerStr), MsgStr(){
    IAssert(int(RegType)==nrtLogin);}
  TNetReg(const int& _RegType, const TStr& _MsgStr=TStr()):
    TNetObj(), RegType(_RegType), UsrNm(), PwdStr(),
    InfoStr(), VerStr(), MsgStr(_MsgStr){
    IAssert((int(RegType)==nrtConfirm)||(int(RegType)==nrtRefuse)||(int(RegType)==nrtLogout));}
  ~TNetReg(){}
  TNetReg(TSIn& SIn):
    TNetObj(SIn), RegType(SIn), UsrNm(SIn), PwdStr(SIn),
    InfoStr(SIn),
    VerStr(SIn),
    MsgStr(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetReg(SIn));}
  void Save(TSOut& SOut){
    TNetObj::Save(SOut); RegType.Save(SOut);
    UsrNm.Save(SOut); PwdStr.Save(SOut);
    InfoStr.Save(SOut); VerStr.Save(SOut); MsgStr.Save(SOut);}

  int GetRegType() const {return RegType;}
  TStr GetUsrNm() const {IAssert(int(RegType)==nrtLogin); return UsrNm;}
  TStr GetPwdStr() const {IAssert(int(RegType)==nrtLogin); return PwdStr;}
  TStr GetVerStr() const {return VerStr;}
  TStr GetInfoStr() const {return InfoStr;}
  TStr GetMsg() const {return MsgStr;}
  TStr GetStr() const {
    TChA Str=TNetObj::GetStr()+GetRegTypeStr(RegType)+":"+MsgStr;
    if (RegType==int(nrtLogin)){Str+=TStr(":'")+UsrNm+"'/'"+PwdStr+"'/'"+InfoStr+"'";}
    return Str;}

  static TStr GetRegTypeStr(const int& NetRegType);
};

/////////////////////////////////////////////////
// Net-Object-Chat-Message
class TNetChatMsg: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetChatMsg>(), TNetObjLoadF(&Load));}
private:
  TStr FromAddrStr;
  TStrV ToAddrStrV;
  TStr MsgStr;
public:
  TNetChatMsg(
   const TStr& _FromAddrStr, const TStrV& _ToAddrStrV, const TStr& _MsgStr):
    TNetObj(), FromAddrStr(_FromAddrStr), ToAddrStrV(_ToAddrStrV), MsgStr(_MsgStr){}
  ~TNetChatMsg(){}
  TNetChatMsg(TSIn& SIn):
    TNetObj(SIn), FromAddrStr(SIn), ToAddrStrV(SIn), MsgStr(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetChatMsg(SIn));}
  void Save(TSOut& SOut){
    TNetObj::Save(SOut);
    FromAddrStr.Save(SOut); ToAddrStrV.Save(SOut); MsgStr.Save(SOut);}

  TStr GetFromAddrStr() const {return FromAddrStr;}
  void GetToAddrStrV(TStrV& _ToAddrStrV) const {_ToAddrStrV=ToAddrStrV;}
  int GetToAddrStrs() const {return ToAddrStrV.Len();}
  TStr GetToAddrStr(const int& ToAddrStrN) const {return ToAddrStrV[ToAddrStrN];}
  TStr GetMsgStr() const {return MsgStr;}
  TStr GetStr() const {return TNetObj::GetStr();}

  TStr GetLogStr();
};

/////////////////////////////////////////////////
// Net-Object-Memory
class TNetMem: public TNetObj{
private:
  static bool IsReg;
  static bool MkReg(){
    return TNetObj::Reg(TTypeNm<TNetMem>(), TNetObjLoadF(&Load));}
private:
  TMem Mem;
public:
  TNetMem(const TMem& _Mem):
    TNetObj(), Mem(_Mem){}
  ~TNetMem(){}
  TNetMem(TSIn& SIn):
    TNetObj(SIn), Mem(SIn){}
  static PNetObj Load(TSIn& SIn){return PNetObj(new TNetMem(SIn));}
  void Save(TSOut& SOut){
    TNetObj::Save(SOut); Mem.Save(SOut);}

  const TMem& GetMem() const {return Mem;}
  TStr GetStr() const {return TNetObj::GetStr();}

  TStr GetLogStr();
};

/////////////////////////////////////////////////
// Net-Object-Packet-Header
class TNetObjPckHd{
private:
  TCh Ch;
  TInt PckLen;
public:
  TNetObjPckHd(const int& _PckLen): Ch('*'), PckLen(_PckLen){}
  TNetObjPckHd(TSIn& SIn, bool& Ok): Ch(SIn), PckLen(SIn){Ok=(Ch==TCh('*'));}
  void Save(TSOut& SOut){Ch.Save(SOut); PckLen.Save(SOut);}

  int GetPckLen() const {return PckLen;}

  static int GetSize(){return 5;}
};

/* ...to include after HIT bug resolved
/////////////////////////////////////////////////
// Net-Object-Packet-Header
class TNetObjPckHd{
private:
  TCh Ch;
  TInt PckLen;
  TMd5Sig Md5Sig;
public:
  TNetObjPckHd(const int& _PckLen, const TMd5Sig& _Md5Sig):
    Ch('*'), PckLen(_PckLen), Md5Sig(_Md5Sig){}
  TNetObjPckHd(TSIn& SIn, bool& Ok):
    Ch(SIn), PckLen(SIn), Md5Sig(SIn){
    Ok=(Ch==TCh('*'));}
  void Save(TSOut& SOut){
    Ch.Save(SOut); PckLen.Save(SOut); Md5Sig.Save(SOut);}

  int GetPckLen() const {return PckLen;}
  TMd5Sig GetMd5Sig() const {return Md5Sig;}

  static int GetSize(){return 5+16;}
};*/

/////////////////////////////////////////////////
// Net-Server-Socket-Event
class TNetSrv;

class TNetSrvSockEvent: public TSockEvent{
private:
  TNetSrv* NetSrv;
public:
  TNetSrvSockEvent(TNetSrv* _NetSrv): NetSrv(_NetSrv){}
  ~TNetSrvSockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int& SockId);
  void OnOob(const int&){Fail;}
  void OnAccept(const int& SockId, const PSock& Sock);
  void OnConnect(const int&){Fail;}
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const int& ErrCd, const TStr& MsgStr);
  void OnGetHost(const PSockHost&){Fail;}
};

/////////////////////////////////////////////////
// Net-Server
//ClassTP(TNetSrv, PNetSrv)//{
class TNetSrv{
private:
  int PortN;
  PSockEvent SrvSockEvent;
  PSock SrvSock;
  THash<TInt, PSock> CltSockIdToSockH;
  TMOut SockMOut;
  UndefCopyAssign(TNetSrv);
public:
  TNetSrv(){Fail;}
  TNetSrv(const int& _PortN, const bool& FixedPortNP=true);
  virtual ~TNetSrv();
  TNetSrv(TSIn&){Fail;}
  //static PNetSrv Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  bool IsConn() const {return !SrvSock.Empty();}
  virtual void Disconn();
  virtual void DisconnClt(const int& CltSockId);

  void SendNetObj(const int& CltSockId, const PNetObj& NetObj);
  void BroadcastNetObj(const PNetObj& NetObj);
  virtual void OnNetObj(const PNetObj& NetObj)=0;

  int GetPortN() const {return PortN;}
  int GetClts() const {return CltSockIdToSockH.Len();}
  bool IsCltSock(const int& CltSockId) const {
    return CltSockIdToSockH.IsKey(CltSockId);}
  PSock GetCltSock(const int& CltSockId) const {
    return CltSockIdToSockH.GetDat(CltSockId);}

  friend class TNetSrvSockEvent;
};

/////////////////////////////////////////////////
// Net-Client-Socket-Event
class TNetClt;
typedef enum {csmGetingHost, csmConnecting, csmConnected,
 csmAfterClose} TNetCltSockEventMode;

class TNetCltSockEvent: public TSockEvent{
private:
  static const int MxGetHostTrys;
  TNetCltSockEventMode Mode;
  int GetHostTrys;
  TNetClt* NetClt;
public:
  TNetCltSockEvent(TNetClt* _NetClt);
  ~TNetCltSockEvent(){}

  void OnRead(const int& SockId, const PSIn& SIn);
  void OnWrite(const int& SockId);
  void OnOob(const int&){Fail;}
  void OnAccept(const int&, const PSock&){Fail;}
  void OnConnect(const int& SockId);
  void OnClose(const int& SockId);
  void OnTimeOut(const int& SockId);
  void OnError(const int& SockId, const int& ErrCd, const TStr& MsgStr);
  void OnGetHost(const PSockHost& SockHost);
};

/////////////////////////////////////////////////
// Net-Client
//ClassTP(TNetClt, PNetClt)//{
class TNetClt{
private:
protected:
  TStr HostNm;
  int PortN;
  PSockEvent SockEvent;
  PSock Sock;
  TMOut SockMOut;
  TStr UsrNm;
  TStr PwdStr;
  UndefCopyAssign(TNetClt);
public:
  TNetClt(){Fail;}
  TNetClt(
   const TStr& _HostNm, const int& _PortN,
   const TStr& _UsrNm=TStr(), const TStr& _PwdStr=TStr());
  virtual ~TNetClt();
  TNetClt(TSIn&){Fail;}
  //static PNetClt Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  //** bug bool IsConn() const {return !Sock.Empty();}
  virtual void Disconn(){Sock=NULL;}
  int GetSockId() const {return Sock->GetSockId();}

  void SendNetObj(const PNetObj& NetObj);
  virtual void OnNetObj(const PNetObj& NetObj)=0;

  TStr GetHostNm() const {return HostNm;}
  int GetPortN() const {return PortN;}
  TStr GetHostPortStr() const {return TStr::Fmt("%s:%d", HostNm.CStr(), PortN);}
  static TStr GetHostPortStr(const TStr& HostNm, const int& PortN){
    return TStr::Fmt("%s:%d", HostNm.CStr(), PortN);}
  TStr GetUsrNm() const {return UsrNm;}
  TStr GetPwdStr() const {return PwdStr;}

  friend class TNetCltSockEvent;
};

