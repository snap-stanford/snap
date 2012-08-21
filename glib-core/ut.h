/////////////////////////////////////////////////
// Type-Name
template <class Type>
class TTypeNm: public TStr{
public:
  static TStr GetNrTypeNm(const TStr& TypeNm){
    if (TypeNm.IsPrefix("class ")){
      return TypeNm.GetSubStr(6, TypeNm.Len()-1);}
    else {return TypeNm;}}
public:
  TTypeNm(): TStr(GetNrTypeNm((char*)(typeid(Type).name()))){}
};
template <class Type>
TStr GetTypeNm(const Type& Var){
  TStr TypeNm=TStr(typeid(Var).name());
  return TTypeNm<Type>::GetNrTypeNm(TypeNm);
}

/////////////////////////////////////////////////
// Notifications
inline void InfoNotify(const TStr& MsgStr){InfoNotify(MsgStr.CStr());}
inline void WarnNotify(const TStr& MsgStr){WarnNotify(MsgStr.CStr());}
inline void ErrNotify(const TStr& MsgStr){ErrNotify(MsgStr.CStr());}
inline void StatNotify(const TStr& MsgStr){StatNotify(MsgStr.CStr());}

typedef enum {ntInfo, ntWarn, ntErr, ntStat} TNotifyType;

ClassTP(TNotify, PNotify)//{
private:
  TNotify(const TNotify&);
  TNotify& operator=(const TNotify&);
public:
  TNotify(){}
  virtual ~TNotify(){}

  virtual void OnNotify(const TNotifyType& /*Type*/, const TStr& /*MsgStr*/){}
  virtual void OnStatus(const TStr& /*MsgStr*/){}
  virtual void OnLn(const TStr& /*MsgStr*/){}
  virtual void OnTxt(const TStr& /*MsgStr*/){}

  // shortcuts for easier formationg
  void OnNotifyFmt(const TNotifyType& Type, const char *FmtStr, ...);
  void OnStatusFmt(const char *FmtStr, ...);
  void OnLnFmt(const char *FmtStr, ...);
  void OnTxtFmt(const char *FmtStr, ...);

  static TStr GetTypeStr(
   const TNotifyType& Type, const bool& Brief=true);
  static void OnNotify(const PNotify& Notify,
   const TNotifyType& Type, const TStr& MsgStr){
    if (!Notify.Empty()){Notify->OnNotify(Type, MsgStr);}}
  static void OnStatus(const PNotify& Notify, const TStr& MsgStr){
    if (!Notify.Empty()){Notify->OnStatus(MsgStr);}}
  static void OnLn(const PNotify& Notify, const TStr& MsgStr){
    if (!Notify.Empty()){Notify->OnLn(MsgStr);}}
  static void OnTxt(const PNotify& Notify, const TStr& MsgStr){
    if (!Notify.Empty()){Notify->OnTxt(MsgStr);}}
  static void DfOnNotify(const TNotifyType& Type, const TStr& MsgStr);

  static const PNotify NullNotify;
  static const PNotify StdNotify;
  static const PNotify StdErrNotify;
};

/////////////////////////////////////////////////
// Null-Notifier
class TNullNotify: public TNotify{
public:
  TNullNotify(){}
  static PNotify New(){return PNotify(new TNullNotify());}

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr){}
  void OnStatus(const TStr& MsgStr){}
};

/////////////////////////////////////////////////
// Callback-Notifier
typedef void (__stdcall *TCallbackF)(const TNotifyType& Type, const TStr& MsgStr);
class TCallbackNotify : public TNotify
{
private:
  TCallbackF CallbackF;
public:
  TCallbackNotify(const TCallbackF& _CallbackF) : CallbackF(_CallbackF) {}
  static PNotify New(const TCallbackF& CallbackF) { return PNotify(new TCallbackNotify(CallbackF)); }

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr)
  {
    Assert(CallbackF != NULL);
    CallbackF(Type, MsgStr); 
  }
  void OnStatus(const TStr& MsgStr)
  {
    Assert(CallbackF != NULL);
    CallbackF(ntStat, MsgStr); 
  }
};

/////////////////////////////////////////////////
// Native-Callback-Notifier
typedef void (__stdcall *TNativeCallbackF)(int Type, const char* MsgStr);
class TNativeCallbackNotify : public TNotify
{
private:
  TNativeCallbackF CallbackF;
public:
  TNativeCallbackNotify(const TNativeCallbackF& _CallbackF) : CallbackF(_CallbackF) {}
  static PNotify New(const TNativeCallbackF& CallbackF) { return PNotify(new TNativeCallbackNotify(CallbackF)); }

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr)
  {
    Assert(CallbackF != NULL);
    CallbackF((int)Type, MsgStr.CStr()); 
  }
  void OnStatus(const TStr& MsgStr)
  {
    Assert(CallbackF != NULL);
    CallbackF((int)ntStat, MsgStr.CStr()); 
  }
};

/////////////////////////////////////////////////
// Standard-Notifier
class TStdNotify: public TNotify{
public:
  TStdNotify(){}
  static PNotify New(){return PNotify(new TStdNotify());}

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr);
  void OnStatus(const TStr& MsgStr);
};

/////////////////////////////////////////////////
// Standard-Error-Notifier
class TStdErrNotify: public TNotify{
public:
  TStdErrNotify(){}
  static PNotify New(){return PNotify(new TStdErrNotify());}

  void OnNotify(const TNotifyType& Type, const TStr& MsgStr);
  void OnStatus(const TStr& MsgStr);
};

//////////////////////////////////////
// Log-Notify
class TLogNotify : public TNotify {
private:
	PNotify Notify;

public:
	TLogNotify(const PNotify& _Notify): Notify(_Notify) { }
	static PNotify New(const PNotify& Notify) { return new TLogNotify(Notify); }

	void OnStatus(const TStr& MsgStr);
};

/////////////////////////////////////////////////
// Exception
ClassTP(TExcept, PExcept)//{
private:
  TStr MsgStr;
  TStr LocStr;
  UndefDefaultCopyAssign(TExcept);
public:
  TExcept(const TStr& _MsgStr): MsgStr(_MsgStr), LocStr(){}
  TExcept(const TStr& _MsgStr, const TStr& _LocStr): MsgStr(_MsgStr), LocStr(_LocStr){}
  virtual ~TExcept(){}

  TStr GetMsgStr() const {return MsgStr;}
  TStr GetLocStr() const {return LocStr;}
  TStr GetStr() const {
    if (LocStr.Empty()){return GetMsgStr();}
    else {return GetLocStr()+": "+GetMsgStr();}}

  // replacement exception handler
  typedef void (*TOnExceptF)(const TStr& MsgStr);
  static TOnExceptF OnExceptF;
  static bool IsOnExceptF(){return OnExceptF!=NULL;}
  static void PutOnExceptF(TOnExceptF _OnExceptF){OnExceptF=_OnExceptF;}
  static TOnExceptF GetOnExceptF(){return OnExceptF;}

  // throwing exception
  static void Throw(const TStr& MsgStr){
    if (IsOnExceptF()){(*OnExceptF)(MsgStr);}
    else {throw PExcept(new TExcept(MsgStr));}}
  static void Throw(const TStr& MsgStr, const TStr& ArgStr){
    TStr FullMsgStr=MsgStr+" ("+ArgStr+")";
    if (IsOnExceptF()){(*OnExceptF)(FullMsgStr);}
    else {throw PExcept(new TExcept(FullMsgStr));}}
  static void Throw(const TStr& MsgStr, const TStr& ArgStr1, const TStr& ArgStr2){
    TStr FullMsgStr=MsgStr+" ("+ArgStr1+", "+ArgStr2+")";
    if (IsOnExceptF()){(*OnExceptF)(FullMsgStr);}
    else {throw PExcept(new TExcept(FullMsgStr));}}
  static void ThrowFull(const TStr& MsgStr, const TStr& LocStr){
    if (IsOnExceptF()){(*OnExceptF)(MsgStr);}
    else {throw PExcept(new TExcept(MsgStr, LocStr));}}
};

#define Try try {
#define Catch } catch (PExcept Except){ErrNotify(Except->GetMsgStr());}
#define CatchFull } catch (PExcept Except){ErrNotify(Except->GetStr());}
#define CatchAll } catch (...){}
