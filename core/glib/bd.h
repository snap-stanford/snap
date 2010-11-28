/////////////////////////////////////////////////
// Basic-Macro-Definitions
#define forever for(;;)

/////////////////////////////////////////////////
// Basic-Type-Definitions
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;

typedef float sdouble;
typedef long double ldouble;
typedef FILE* TFileId;

typedef char int8;
typedef short int16;
typedef int int32;
#ifdef GLib_WIN32
typedef __int64 int64;
#elif defined(GLib_GLIBC)
typedef int64_t int64;
#else
typedef long long int64;
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#ifdef GLib_WIN32
typedef unsigned __int64 uint64;
#elif defined(GLib_GLIBC)
typedef u_int64_t uint64;
#else
typedef unsigned long long uint64;
#endif

#if (!defined(__ssize_t_defined) && !defined(GLib_MACOSX))
typedef ptrdiff_t ssize_t;
#endif

#if defined(GLib_UNIX)
#define _isnan(x) isnan(x)
#define _finite(x) finite(x)
#endif

#if defined(GLib_WIN32)
#define _vsnprintf vsnprintf
#endif

typedef size_t TSize;
#define TSizeMx SIZE_MAX

/////////////////////////////////////////////////
// Localization
typedef enum {lUndef, lUs, lSi} TLoc;

/////////////////////////////////////////////////
// Messages
void WrNotify(const char* CaptionCStr, const char* NotifyCStr);
void SaveToErrLog(const char* MsgCStr);

inline void InfoNotify(const char* NotifyCStr){
  WrNotify("Information", NotifyCStr);}
inline void WarnNotify(const char* NotifyCStr){
  WrNotify("Warning", NotifyCStr);}
inline void ErrNotify(const char* NotifyCStr){
  WrNotify("Error", NotifyCStr);}
inline void StatNotify(const char* NotifyCStr){
  WrNotify("Status", NotifyCStr);}

/////////////////////////////////////////////////
// Compiler-Options
#if defined (GLib_WIN)
  #define snprintf _snprintf
  #define vsnprintf  _vsnprintf
  // disable warning 'debug symbol greater than 255 chars'
  #pragma warning(disable: 4786)
  // disable warning 'return type for 'identifier::operator->'
  // is not a UDT or reference to a UDT.
  //#pragma warning(disable: 4284)
  // 'declaration' : function differs from 'declaration'
  // only by calling convention
  //#pragma warning(disable: 4666)
  #pragma warning(disable: 4996)
#endif

#if defined (GLib_GCC)
  #define _CONSOLE
#endif

// use console output if console already exists, otherwise
// define GLib_COUT to use console (class COut)
#if defined(_CONSOLE)||defined(__CONSOLE__)
  #define GLib_Console
#endif

// create console if want to output but there is no console allocated
#if defined(GLib_Console) && (! (defined(__CONSOLE__)||defined(_CONSOLE)))
  #define GLib_CreateConsole
#endif

/////////////////////////////////////////////////
// Class-Definition
#define ClassT(TNm) \
class TNm{

#define ClassTV(TNm, TNmV) \
class TNm; \
typedef TVec<TNm> TNmV; \
class TNm{

#define ClassTVQ(TNm, TNmV, TNmQ) \
class TNm; \
typedef TVec<TNm> TNmV; \
typedef TQQueue<TNm> TNmQ; \
class TNm{

#define ClassTP(TNm, PNm) \
class TNm; \
typedef TPt<TNm> PNm; \
class TNm{ \
private: \
  TCRef CRef; \
public: \
  friend class TPt<TNm>;

#define ClassHdTP(TNm, PNm) \
class TNm; \
typedef TPt<TNm> PNm;

#define ClassTPE(TNm, PNm, ENm) \
class TNm; \
typedef TPt<TNm> PNm; \
class TNm: public ENm{ \
private: \
  TCRef CRef; \
public: \
  friend class TPt<TNm>;

#define ClassTPEE(TNm, PNm, ENm1, ENm2) \
class TNm; \
typedef TPt<TNm> PNm; \
class TNm: public ENm1, public ENm2{ \
private: \
  TCRef CRef; \
public: \
  friend class TPt<TNm>;

#define ClassTE(TNm, ENm) \
class TNm; \
class TNm: public ENm{ \
public: \

#define ClassTPV(TNm, PNm, TNmV) \
class TNm; \
typedef TPt<TNm> PNm; \
typedef TVec<PNm> TNmV; \
class TNm{ \
private: \
  TCRef CRef; \
public: \
  friend class TPt<TNm>;

#define ClassHdTPV(TNm, PNm, TNmV) \
class TNm; \
typedef TPt<TNm> PNm; \
typedef TVec<PNm> TNmV;

#define ClassTPVL(TNm, PNm, TNmV, TNmL, TNmLN) \
class TNm; \
typedef TPt<TNm> PNm; \
typedef TVec<PNm> TNmV; \
typedef TLst<PNm> TNmL; \
typedef TLstNd<PNm>* TNmLN; \
class TNm{ \
private: \
  TCRef CRef; \
public: \
  friend class TPt<TNm>;

/////////////////////////////////////////////////
// Forward-Definitions
class TSIn;
class TSOut;
class TStr;
class TXmlObjSer;
class TXmlObjSerTagNm;
template <class TRec> class TPt;
ClassHdTP(TXmlTok, PXmlTok)
ClassHdTP(TExcept, PExcept)
ClassHdTP(TXmlDoc, PXmlDoc)

/////////////////////////////////////////////////
// DefaultConstructor/CopyConstructor/AssignmentOperator-UnDefinition
#define UndefDefaultCopyAssign(TNm) \
private: \
  TNm(){} \
  TNm(const TNm&); \
  TNm& operator=(const TNm&)

#define UndefCopyAssign(TNm) \
private: \
  TNm(const TNm&); \
  TNm& operator=(const TNm&)

#define UndefCopy(TNm) \
private: \
  TNm(const TNm&)

#define UndefAssign(TNm) \
private: \
  TNm& operator=(const TNm&)

/////////////////////////////////////////////////
// Assertions
class TOnExeStop{
private:
  typedef bool (*TOnExeStopF)(char* MsgCStr);
  static TOnExeStopF OnExeStopF;
public:
  static bool IsOnExeStopF(){return OnExeStopF!=NULL;}
  static void PutOnExeStopF(TOnExeStopF _OnExeStopF){OnExeStopF=_OnExeStopF;}
  static TOnExeStopF GetOnExeStopF(){return OnExeStopF;}
};

void ExeStop(
 const char* MsgStr, const char* ReasonStr,
 const char* CondStr, const char* FNm, const int& LnN);

#define Fail ExeStop(NULL, NULL, "Fail", __FILE__, __LINE__)

#define FailR(Reason) ExeStop((Reason), NULL, "Fail", __FILE__, __LINE__)
#define FailRA(Reason, ArgStr) \
  ExeStop((TStr(Reason)+" ("+ArgStr+")").CStr(), NULL, "Fail", __FILE__, __LINE__)

#define EFail TExcept::ThrowFull("", TStr("[")+ TStr(__FILE__) + " line " + TInt::GetStr(__LINE__) + "]")

#define EFailR(Reason) TExcept::ThrowFull(Reason, TStr("[")+TStr(__FILE__)+" line "+TInt::GetStr(__LINE__)+"]")

#ifdef NDEBUG
#define Assert(Cond)
#else
#define Assert(Cond) \
  ((Cond) ? static_cast<void>(0) : ExeStop(NULL, NULL, #Cond, __FILE__, __LINE__))
#endif

#ifdef NDEBUG
#define AssertR(Cond, Reason)
#else
#define AssertR(Cond, Reason) ((Cond) ? static_cast<void>(0) : \
  ExeStop(NULL, Reason.CStr(), #Cond, __FILE__, __LINE__))
#endif

#define IAssert(Cond) \
  ((Cond) ? static_cast<void>(0) : ExeStop(NULL, NULL, #Cond, __FILE__, __LINE__))

#define IAssertR(Cond, Reason) \
  ((Cond) ? static_cast<void>(0) : ExeStop(NULL, TStr(Reason).CStr(), #Cond, __FILE__, __LINE__))

#define WAssert(Cond, MsgCStr) \
  ((Cond) ? static_cast<void>(0) : WarnNotify(MsgCStr))

#define SAssert(Cond) \
  ((Cond) ? static_cast<void>(0) : ExeStop(TSysStr::GetLastMsgCStr(), NULL, #Cond, __FILE__, __LINE__))

#define FAssert(Cond, MsgCStr) \
  ((Cond) ? static_cast<void>(0) : ExeStop(TStr(MsgCStr).CStr(), NULL, NULL, __FILE__, __LINE__))

#define FSAssert(Cond) \
  ((Cond) ? static_cast<void>(0) : ExeStop(TSysStr::GetLastMsgCStr(), NULL, NULL, __FILE__, __LINE__))

#define EAssert(Cond) \
  ((Cond) ? static_cast<void>(0) : TExcept::ThrowFull(#Cond, TStr(__FILE__) + " line " + TInt::GetStr(__LINE__) +": "+ TStr(#Cond)))

#define EAssertR(Cond, MsgStr) \
  ((Cond) ? static_cast<void>(0) : TExcept::ThrowFull(MsgStr, TStr(__FILE__)+" line "+TInt::GetStr(__LINE__)+": "+TStr(#Cond)))

#define EAssertRA(Cond, MsgStr, ArgStr) \
  ((Cond) ? static_cast<void>(0) : TExcept::Throw(MsgStr, ArgStr))

#define EAssertRAA(Cond, MsgStr, ArgStr1, ArgStr2) \
  ((Cond) ? static_cast<void>(0) : TExcept::Throw(MsgStr, ArgStr1, ArgStr2))

#define ESAssert(Cond) \
  ((Cond) ? static_cast<void>(0) : TExcept::Throw(TSysStr::GetLastMsgCStr(), \
  TStr(__FILE__) + " line " + TInt::GetStr(__LINE__) +": "+ TStr(#Cond)))

// compile time assert 
// #define STATIC_ASSERT(x) { const char temp[ (((x) == 0) ? 0 : 1) ] = {'\0'}; }
template <bool BoolVal> struct TStaticAssert;
template <> struct TStaticAssert<true> { enum { value = 1 }; };
template<int IntVal> struct TStaticAssertTest{};

#define CAssert(Cond) \
  { typedef TStaticAssertTest<sizeof(TStaticAssert<(Cond)==0?false:true>)> TestStaticAssert; }

/////////////////////////////////////////////////
// Xml-Object-Serialization
template <class TRec>
bool IsXLoadFromFileOk(const TStr& FNm, const TStr& Nm, TRec& Rec, TStr& MsgStr);
template <class TRec>
void XLoadFromFile(const TStr& FNm, const TStr& Nm, TRec& Rec);

#define XLoadHd(Nm) \
  {TStr TypeNm=TXmlObjSer::GetTagNm(TStr(typeid(*this).name())); \
  TXmlObjSer::AssertXmlHd(XmlTok, Nm, TypeNm);}
#define XLoad(Nm) \
  Nm.LoadXml(XmlTok->GetTagTok(#Nm), #Nm);

#define XSaveHd(Nm) \
  TStr _TypeNm=TXmlObjSer::GetTagNm(TStr(typeid(*this).name())); \
  TXmlObjSerTagNm XmlObjSerTagNm(SOut, false, Nm, _TypeNm);
#define XSaveHdArg(Nm, ArgNm, ArgVal) \
  TStr _TypeNm=TXmlObjSer::GetTagNm(TStr(typeid(*this).name())); \
  TXmlObjSerTagNm XmlObjSerTagNm(SOut, false, Nm, _TypeNm, ArgNm, ArgVal);
#define XSaveBETag(Nm) \
  TStr _TypeNm=TXmlObjSer::GetTagNm(TStr(typeid(*this).name())); \
  TXmlObjSerTagNm XmlObjSerTagNm(SOut, true, Nm, _TypeNm);
#define XSaveBETagArg(Nm, ArgNm, ArgVal) \
  TStr _TypeNm=TXmlObjSer::GetTagNm(TStr(typeid(*this).name())); \
  TXmlObjSerTagNm XmlObjSerTagNm(SOut, true, Nm, _TypeNm, ArgNm, ArgVal);
#define XSaveBETagArg4(Nm, ArgNm1, ArgVal1, ArgNm2, ArgVal2, ArgNm3, ArgVal3, ArgNm4, ArgVal4) \
  TStr _TypeNm=TXmlObjSer::GetTagNm(TStr(typeid(*this).name())); \
  TXmlObjSerTagNm XmlObjSerTagNm(SOut, true, Nm, _TypeNm, ArgNm1, ArgVal1, ArgNm2, ArgVal2, ArgNm3, ArgVal3, ArgNm4, ArgVal4);
#define XSave(Nm) \
  Nm.SaveXml(SOut, #Nm)
#define XSaveToFile(Nm, FNm) \
  {TFOut SOut(FNm); Nm.SaveXml(SOut, #Nm);}

/////////////////////////////////////////////////
// Basic-Operators
typedef enum {loUndef, loNot, loAnd, loOr} TLogOp;
typedef enum {roUndef, roLs, roLEq, roEq, roNEq, roGEq, roGt} TRelOp;

/////////////////////////////////////////////////
// Comparation-Macros
#ifndef min
 #define min(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef max
 #define max(a,b) ((a)>(b)?(a):(b))
#endif

/////////////////////////////////////////////////
// Comparator-Definitions
template<class T>
class TEq{
public:
  bool operator()(const T& x, const T& y) const {return x==y;}
};

template<class T>
class TNEq{
public:
  bool operator()(const T& x, const T& y) const {return x!=y;}
};

template<class T>
class TLss{
public:
  bool operator()(const T& x, const T& y) const {return x<y;}
  int Cmp(const T& d1, const T& d2) const {
    if (d1<d2){return -1;}
    else if (d2<d1){return 1;}
    else {return 0;}
  }
};

template<class T>
class TLEq{
public:
  bool operator()(const T& x, const T& y) const {return x<=y;}
};

template<class T>
class TGtr{
public:
  bool operator() (const T& x, const T& y) const { return x>y; }
  int Cmp(const T& d1, const T& d2) const { //** vprasaj Janeza za uporabo
    if (d1<d2){return 1;}
    else if (d2<d1){return -1;}
    else {return 0;}
  }
};

template<class T>
class TGEq{
public:
  bool operator()(const T& x, const T& y) const {return x>=y;}
};

template<class T>
class TCmp{
public:
  int operator()(const T& x, const T& y) const {
    if (x < y){return -1;}
    else if (x > y){return 1;}
    else {return 0;}
  }
};

/////////////////////////////////////////////////
// Operator-Definitions
template <class TRec>
bool operator!=(const TRec& Rec1, const TRec& Rec2){return !(Rec1==Rec2);}

template <class TRec>
bool operator>(const TRec& Rec1, const TRec& Rec2){return Rec2<Rec1;}

template <class TRec>
bool operator<=(const TRec& Rec1, const TRec& Rec2){return !(Rec2<Rec1);}

template <class TRec>
bool operator>=(const TRec& Rec1, const TRec& Rec2){return !(Rec1<Rec2);}

template <class TRec>
bool Cmp(const int& RelOp, const TRec& Rec1, const TRec& Rec2){
  switch (RelOp){
    case roLs: return Rec1<Rec2;
    case roLEq: return Rec1<=Rec2;
    case roEq: return Rec1==Rec2;
    case roNEq: return Rec1!=Rec2;
    case roGEq: return Rec1>=Rec2;
    case roGt: return Rec1>Rec2;
    default: Fail; return false;
  }
}

/////////////////////////////////////////////////
// Reference-Count
class TCRef{
private:
  int Refs;
private:
  TCRef& operator=(const TCRef&);
  TCRef(const TCRef&);
public:
  TCRef(): Refs(0){}
  ~TCRef(){Assert(Refs==0);}

  void MkRef(){Refs++;}
  void UnRef(){Assert(Refs>0); Refs--;}
  bool NoRef() const {return Refs==0;}
  int GetRefs() const {return Refs;}
};

/////////////////////////////////////////////////
// Smart-Pointer-With-Reference-Count
template <class TRec>
class TPt{
public:
  typedef TRec TObj;
private:
  TRec* Addr;
  void MkRef() const {
    if (Addr!=NULL){
      Addr->CRef.MkRef();
    }
  }
  void UnRef() const {
    if (Addr!=NULL){
      Addr->CRef.UnRef();
      if (Addr->CRef.NoRef()){delete Addr;}
    }
  }
public:
  TPt(): Addr(NULL){}
  TPt(const TPt& Pt): Addr(Pt.Addr){MkRef();}
  TPt(TRec* _Addr): Addr(_Addr){MkRef();}
  static TPt New(){return TObj::New();}
  ~TPt(){UnRef();}
  explicit TPt(TSIn& SIn);
  explicit TPt(TSIn& SIn, void* ThisPt);
  void Save(TSOut& SOut) const;
  void LoadXml(const TPt<TXmlTok>& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TPt& operator=(const TPt& Pt){
    if (this!=&Pt){Pt.MkRef(); UnRef(); Addr=Pt.Addr;} return *this;}
  bool operator==(const TPt& Pt) const {return *Addr==*Pt.Addr;}
  bool operator!=(const TPt& Pt) const {return *Addr!=*Pt.Addr;}
  bool operator<(const TPt& Pt) const {return *Addr<*Pt.Addr;}

  TRec* operator->() const {Assert(Addr!=NULL); return Addr;}
  TRec& operator*() const {Assert(Addr!=NULL); return *Addr;}
  TRec& operator[](const int& RecN) const {
    Assert(Addr!=NULL); return Addr[RecN];}
  TRec* operator()() const {return Addr;}
  //const TRec* operator()() const {return Addr;}
  //TRec* operator()() {return Addr;}

  bool Empty() const {return Addr==NULL;}
  void Clr(){UnRef(); Addr=NULL;}
  int GetRefs() const {
    if (Addr==NULL){return -1;} else {return Addr->CRef.GetRefs();}}

  int GetPrimHashCd() const {return Addr->GetPrimHashCd();}
  int GetSecHashCd() const {return Addr->GetSecHashCd();}

  TPt<TRec> Clone(){return MkClone(*this);}
};

/////////////////////////////////////////////////
// Simple-String
class TSStr{
private:
  char* Bf;
public:
  TSStr(): Bf(NULL){
    Bf=new char[0+1]; Bf[0]=0;}
  TSStr(const TSStr& SStr): Bf(NULL){
    Bf=new char[strlen(SStr.Bf)+1]; strcpy(Bf, SStr.Bf);}
  TSStr(const char* _Bf): Bf(NULL){
    Bf=new char[strlen(_Bf)+1]; strcpy(Bf, _Bf);}
  ~TSStr(){delete[] Bf;}

  TSStr& operator=(const TSStr& SStr){
    if (this!=&SStr){
      delete[] Bf; Bf=new char[strlen(SStr.Bf)+1]; strcpy(Bf, SStr.Bf);}
    return *this;}

  char* CStr() {return Bf;}
  const char* CStr() const {return Bf;}
  bool Empty() const {return Bf[0]==0;}
  int Len() const {return int(strlen(Bf));}
};

/////////////////////////////////////////////////
// Conversion-Pointer64-To-Integers32
class TConv_Pt64Ints32{
private:
  union{
    void* Pt;
    uint64 UInt64;
    struct {uint Ls; uint Ms;} UInt32;
  } Val;
  UndefCopyAssign(TConv_Pt64Ints32);
public:
  TConv_Pt64Ints32(){
    Val.Pt=0; Val.UInt32.Ms=0; Val.UInt32.Ls=0;}
  TConv_Pt64Ints32(void* Pt){
    Val.UInt32.Ms=0; Val.UInt32.Ls=0; Val.Pt=Pt;}
  TConv_Pt64Ints32(const uint& Ms, const uint& Ls){
    Val.Pt=0; Val.UInt32.Ms=Ms; Val.UInt32.Ls=Ls;}

  void PutPt(void* Pt){Val.Pt=Pt;}
  void* GetPt() const {return Val.Pt;}
  void PutUInt64(const uint64& _UInt64){Val.UInt64=_UInt64;}
  uint64 GetUInt64() const {return Val.UInt64;}
  void PutMsUInt32(const uint& Ms){Val.UInt32.Ms=Ms;}
  uint GetMsUInt32() const {return Val.UInt32.Ms;}
  void PutLsUInt32(const uint& Ls){Val.UInt32.Ls=Ls;}
  uint GetLsUInt32() const {return Val.UInt32.Ls;}
};

/////////////////////////////////////////////////
// Swap
template <class TRec>
void Swap(TRec& Rec1, TRec& Rec2){
  TRec Rec=Rec1; Rec1=Rec2; Rec2=Rec;
}
