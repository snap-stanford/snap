/////////////////////////////////////////////////
// Forward-Definitions
class TMem;
class TChA;
class TStr;

/////////////////////////////////////////////////
// Check-Sum
class TCs{
private:
  static const int MxMask;
  int Val;
public:
  TCs(): Val(0){}
  TCs(const TCs& Cs): Val(Cs.Val&MxMask){}
  TCs(const int& Int): Val(Int&MxMask){}

  TCs& operator=(const TCs& Cs){Val=Cs.Val; return *this;}
  bool operator==(const TCs& Cs) const {return Val==Cs.Val;}
  TCs& operator+=(const TCs& Cs){Val=(Val+Cs.Val)&MxMask; return *this;}
  TCs& operator+=(const char& Ch){Val=(Val+Ch)&MxMask; return *this;}
  TCs& operator+=(const int& Int){Val=(Val+Int)&MxMask; return *this;}
  int Get() const {return Val;}

  static TCs GetCsFromBf(char* Bf, const int& BfL);
};

/////////////////////////////////////////////////
// Output-stream-manipulator
class TSOutMnp {
public:
  virtual TSOut& operator()(TSOut& SOut) const=0;
  virtual ~TSOutMnp();
};

/////////////////////////////////////////////////
// Stream-base
class TSBase{
protected:
  TCRef CRef;
  TSStr SNm;
  TCs Cs;
//protected:
//  TSBase();
//  TSBase(const TSBase&);
//  TSBase& operator=(const TSBase&);
public:
  TSBase(const TSStr& Nm): SNm(Nm){}
  virtual ~TSBase(){}

  virtual TStr GetSNm() const;
};

/////////////////////////////////////////////////
// Input-Stream
class TSIn: virtual public TSBase{
private:
  bool FastMode;
private:
  TSIn(const TSIn&);
  TSIn& operator=(const TSIn&);
public:
  TSIn(): TSBase("Input-Stream"), FastMode(false){}
  TSIn(const TStr& Str);
  virtual ~TSIn(){}

  virtual bool Eof()=0; // if end-of-file
  virtual int Len() const=0;  // get number of bytes till eof
  virtual char GetCh()=0;     // get one char and advance
  virtual char PeekCh()=0;    // get one char and do NOT advance
  virtual int GetBf(const void* Bf, const TSize& BfL)=0; // get BfL chars and advance
  virtual void Reset(){Fail;}

  bool IsFastMode() const {return FastMode;}
  void SetFastMode(const bool& _FastMode){FastMode=_FastMode;}

  void LoadCs();
  void LoadBf(const void* Bf, const TSize& BfL){Cs+=GetBf(Bf, BfL);}
  void* LoadNewBf(const int& BfL){
    void* Bf=(void*)new char[BfL]; Cs+=GetBf(Bf, BfL); return Bf;}
  void Load(bool& Bool){Cs+=GetBf(&Bool, sizeof(Bool));}
  void Load(uchar& UCh){Cs+=GetBf(&UCh, sizeof(UCh));}
  void Load(char& Ch){Cs+=GetBf(&Ch, sizeof(Ch));}
  void Load(short& Short){Cs+=GetBf(&Short, sizeof(Short));} //J:
  void Load(ushort& UShort){Cs+=GetBf(&UShort, sizeof(UShort));} //J:
  void Load(int& Int){Cs+=GetBf(&Int, sizeof(Int));}
  void Load(uint& UInt){Cs+=GetBf(&UInt, sizeof(UInt));}
  void Load(int64& Int){Cs+=GetBf(&Int, sizeof(Int));}
  void Load(uint64& UInt){Cs+=GetBf(&UInt, sizeof(UInt));}
  void Load(double& Flt){Cs+=GetBf(&Flt, sizeof(Flt));}
  void Load(sdouble& SFlt){Cs+=GetBf(&SFlt, sizeof(SFlt));}
  void Load(ldouble& LFlt){Cs+=GetBf(&LFlt, sizeof(LFlt));}
  void Load(char*& CStr, const int& MxCStrLen, const int& CStrLen){
    CStr=new char[MxCStrLen+1]; Cs+=GetBf(CStr, CStrLen+1);}
  void Load(char*& CStr);

  TSIn& operator>>(bool& Bool){Cs+=GetBf(&Bool, sizeof(Bool)); return *this;}
  TSIn& operator>>(uchar& UCh){Cs+=GetBf(&UCh, sizeof(UCh)); return *this;}
  TSIn& operator>>(char& Ch){Cs+=GetBf(&Ch, sizeof(Ch)); return *this;}
  TSIn& operator>>(short& Sh){Cs+=GetBf(&Sh, sizeof(Sh)); return *this;}
  TSIn& operator>>(ushort& USh){Cs+=GetBf(&USh, sizeof(USh)); return *this;}
  TSIn& operator>>(int& Int){Cs+=GetBf(&Int, sizeof(Int)); return *this;}
  TSIn& operator>>(uint& UInt){Cs+=GetBf(&UInt, sizeof(UInt)); return *this;}
  TSIn& operator>>(int64& Int){Cs+=GetBf(&Int, sizeof(Int)); return *this;}
  TSIn& operator>>(uint64& UInt){Cs+=GetBf(&UInt, sizeof(UInt)); return *this;}
  TSIn& operator>>(float& Flt){Cs+=GetBf(&Flt, sizeof(Flt)); return *this;}
  TSIn& operator>>(double& Double){Cs+=GetBf(&Double, sizeof(Double)); return *this;}
  TSIn& operator>>(long double& LDouble){Cs+=GetBf(&LDouble, sizeof(LDouble)); return *this;}

  bool GetNextLn(TStr& LnStr);
  bool GetNextLn(TChA& LnChA);

  static const TPt<TSIn> StdIn;
  friend class TPt<TSIn>;
};
typedef TPt<TSIn> PSIn;

template <class T>
TSIn& operator>>(TSIn& SIn, T& Val) {
  Val.Load(SIn); return SIn;
}

/////////////////////////////////////////////////
// Output-Stream
class TSOut: virtual public TSBase{
private:
  int MxLnLen, LnLen;
  int UpdateLnLen(const int& StrLen, const bool& ForceInLn=false);
private:
  TSOut(const TSIn&);
  TSOut& operator = (const TSOut&);
public:
  TSOut(): TSBase("Output-Stream"), MxLnLen(-1), LnLen(0){}
  TSOut(const TStr& Str);
  virtual ~TSOut(){}

  void EnableLnTrunc(const int& _MxLnLen){MxLnLen=_MxLnLen;}
  void DisableLnTrunc(){MxLnLen=-1;}

  virtual int PutCh(const char& Ch)=0;
  virtual int PutBf(const void* LBf, const TSize& LBfL)=0;
  virtual void Flush()=0;
  virtual TFileId GetFileId() const {return NULL;}

  int PutMem(const TMem& Mem);
  int PutCh(const char& Ch, const int& Chs);
  int PutBool(const bool& Bool);
  int PutInt(const int& Int);
  int PutInt(const int& Int, const char* FmtStr);
  int PutUInt(const uint& Int);
  int PutUInt(const uint& Int, const char* FmtStr);
  int PutFlt(const double& Flt);
  int PutFlt(const double& Flt, const char* FmtStr);
  int PutStr(const char* CStr);
  int PutStr(const TChA& ChA);
  int PutStr(const TStr& Str, const char* FmtStr);
  int PutStr(const TStr& Str, const bool& ForceInLn=false);
  int PutStrLn(const TStr& Str, const bool& ForceInLn=false){
    int Cs=PutStr(Str,ForceInLn); Cs+=PutLn(); return Cs;}
  int PutStrFmt(const char *FmtStr, ...); 
  int PutStrFmtLn(const char *FmtStr, ...); 
  int PutIndent(const int& IndentLev=1);
  int PutLn(const int& Lns=1);
  int PutDosLn(const int& Lns=1);
  int PutSep(const int& NextStrLen=0);
  int PutSepLn(const int& Lns=0);

  void SaveCs(){Cs+=PutBf(&Cs, sizeof(Cs));}
  void SaveBf(const void* Bf, const TSize& BfL){Cs+=PutBf(Bf, BfL);}
  void Save(const bool& Bool){Cs+=PutBf(&Bool, sizeof(Bool));}
  void Save(const char& Ch){Cs+=PutBf(&Ch, sizeof(Ch));}
  void Save(const uchar& UCh){Cs+=PutBf(&UCh, sizeof(UCh));}
  void Save(const short& Short){Cs+=PutBf(&Short, sizeof(Short));}
  void Save(const ushort& UShort){Cs+=PutBf(&UShort, sizeof(UShort));}
  void Save(const int& Int){Cs+=PutBf(&Int, sizeof(Int));}
  void Save(const uint& UInt){Cs+=PutBf(&UInt, sizeof(UInt));}
  void Save(const int64& Int){Cs+=PutBf(&Int, sizeof(Int));}
  void Save(const uint64& UInt){Cs+=PutBf(&UInt, sizeof(UInt));}
  void Save(const double& Flt){Cs+=PutBf(&Flt, sizeof(Flt));}
  void Save(const sdouble& SFlt){Cs+=PutBf(&SFlt, sizeof(SFlt));}
  void Save(const ldouble& LFlt){Cs+=PutBf(&LFlt, sizeof(LFlt));}
  void Save(const char* CStr, const TSize& CStrLen){Cs+=PutBf(CStr, CStrLen+1);}
  void Save(const char* CStr);
  void Save(TSIn& SIn, const TSize& BfL=-1);
  void Save(const PSIn& SIn, const TSize& BfL=-1){Save(*SIn, BfL);}
  void Save(const void* Bf, const TSize& BfL){Cs+=PutBf(Bf, BfL);}

  TSOut& operator<<(const bool& Bool){Cs+=PutBf(&Bool, sizeof(Bool)); return *this;}
  TSOut& operator<<(const uchar& UCh){Cs+=PutBf(&UCh, sizeof(UCh)); return *this;}
  TSOut& operator<<(const char& Ch){Cs+=PutBf(&Ch, sizeof(Ch)); return *this;}
  TSOut& operator<<(const short& Sh){Cs+=PutBf(&Sh, sizeof(Sh)); return *this;}
  TSOut& operator<<(const ushort& USh){Cs+=PutBf(&USh, sizeof(USh)); return *this;}
  TSOut& operator<<(const int& Int){Cs+=PutBf(&Int, sizeof(Int)); return *this;}
  TSOut& operator<<(const uint& Int){Cs+=PutBf(&Int, sizeof(Int)); return *this;}
  TSOut& operator<<(const int64& Int){Cs+=PutBf(&Int, sizeof(Int)); return *this;}
  TSOut& operator<<(const uint64& UInt){Cs+=PutBf(&UInt, sizeof(UInt)); return *this;}
  TSOut& operator<<(const float& Flt){Cs+=PutBf(&Flt, sizeof(Flt)); return *this;}
  TSOut& operator<<(const double& Double){Cs+=PutBf(&Double, sizeof(Double)); return *this;}
  TSOut& operator<<(const long double& LDouble){Cs+=PutBf(&LDouble, sizeof(LDouble)); return *this;}
  TSOut& operator<<(const TSOutMnp& Mnp){return Mnp(*this);}
  TSOut& operator<<(TSOut&(*FuncPt)(TSOut&)){return FuncPt(*this);}
  TSOut& operator<<(TSIn& SIn);
  TSOut& operator<<(PSIn& SIn){return operator<<(*SIn);}

  static const TPt<TSOut> StdOut;
  friend class TPt<TSOut>;
};
typedef TPt<TSOut> PSOut;

template <class T>
TSOut& operator<<(TSOut& SOut, const T& Val){
  Val.Save(SOut); return SOut;
}

/////////////////////////////////////////////////
// Input-Output-Stream-Base
class TSInOut: public TSIn, public TSOut{
private:
  TSInOut(const TSInOut&);
  TSInOut& operator=(const TSInOut&);
public:
  TSInOut(): TSBase("Input-Output-Stream"), TSIn(), TSOut() {}
  virtual ~TSInOut(){}

  virtual void SetPos(const int& Pos)=0;
  virtual void MovePos(const int& DPos)=0;
  virtual int GetPos() const=0;
  virtual int GetSize() const=0; // size of whole stream
  virtual void Clr()=0; // clear IO buffer

  friend class TPt<TSInOut>;
};
typedef TPt<TSInOut> PSInOut;

/////////////////////////////////////////////////
// Standard-Input
class TStdIn: public TSIn{
private:
  TStdIn(const TStdIn&);
  TStdIn& operator=(const TStdIn&);
public:
  TStdIn();
  static TPt<TSIn> New(){return new TStdIn();}

  bool Eof(){return feof(stdin)!=0;}
  int Len() const {return -1;}
  char GetCh(){return char(getchar());}
  char PeekCh(){
    int Ch=getchar(); ungetc(Ch, stdin); return char(Ch);}
  int GetBf(const void* LBf, const TSize& LBfL);
  void Reset(){Cs=TCs();}
};

/////////////////////////////////////////////////
// Standard-Output
class TStdOut: public TSOut{
private:
  TStdOut(const TStdOut&);
  TStdOut& operator=(const TStdOut&);
public:
  TStdOut();
  static TPt<TSOut> New(){return new TStdOut();}

  int PutCh(const char& Ch){putchar(Ch); return Ch;}
  int PutBf(const void *LBf, const TSize& LBfL);
  void Flush(){fflush(stdout);}
};

/////////////////////////////////////////////////
// Input-File
class TFIn: public TSIn{
private:
  static const int MxBfL;
  TFileId FileId;
  char* Bf;
  int BfC, BfL;
private:
  void SetFPos(const int& FPos) const;
  int GetFPos() const;
  int GetFLen() const;
  void FillBf();
private:
  TFIn();
  TFIn(const TFIn&);
  TFIn& operator=(const TFIn&);
public:
  TFIn(const TStr& FNm);
  TFIn(const TStr& FNm, bool& OpenedP);
  static PSIn New(const TStr& FNm);
  static PSIn New(const TStr& FNm, bool& OpenedP);
  ~TFIn();

  bool Eof(){
    if ((BfC==BfL)&&(BfL==MxBfL)){FillBf();}
    return (BfC==BfL)&&(BfL<MxBfL);}
  int Len() const {return GetFLen()-(GetFPos()-BfL+BfC);}
  char GetCh(){
    if (BfC==BfL){if (Eof()){return 0;} return Bf[BfC++];}
    else {return Bf[BfC++];}}
  char PeekCh(){
    if (BfC==BfL){if (Eof()){return 0;} return Bf[BfC];}
    else {return Bf[BfC];}}
  int GetBf(const void* LBf, const TSize& LBfL);
  void Reset(){rewind(FileId); Cs=TCs(); BfC=BfL=-1; FillBf();}

  //J:ne rabim
  //TFileId GetFileId() const {return FileId;} //J:
  //void SetFileId(const FileId& FlId) {FileId=FlId; BfC=BfL=-1; FillBf(); } //J: za grde low level manipulacije
};

/////////////////////////////////////////////////
// Output-File
class TFOut: public TSOut{
private:
  static const TSize MxBfL;
  TFileId FileId;
  char* Bf;
  TSize BfL;
private:
  void FlushBf();
private:
  TFOut();
  TFOut(const TFOut&);
  TFOut& operator=(const TFOut&);
public:
  TFOut(const TStr& _FNm, const bool& Append=false);
  TFOut(const TStr& _FNm, const bool& Append, bool& OpenedP);
  static PSOut New(const TStr& FNm, const bool& Append=false);
  static PSOut New(const TStr& FNm, const bool& Append, bool& OpenedP);
  ~TFOut();

  int PutCh(const char& Ch);
  int PutBf(const void* LBf, const TSize& LBfL);
  void Flush();

  TFileId GetFileId() const {return FileId;}
};

/////////////////////////////////////////////////
// Input-Output-File
typedef enum {faUndef, faCreate, faUpdate, faAppend, faRdOnly, faRestore} TFAccess;

class TFInOut : public TSInOut {
private:
  TFileId FileId;
private:
  TFInOut();
  TFInOut(const TFIn&);
  TFInOut& operator=(const TFIn&);
public:
  TFInOut(const TStr& FNm, const TFAccess& FAccess, const bool& CreateIfNo);
  static PSInOut New(const TStr& FNm, const TFAccess& FAccess, const bool& CreateIfNo);
  ~TFInOut() { if (FileId!=NULL) IAssert(fclose(FileId) == 0); }

  TStr GetFNm() const;
  TFileId GetFileId() const {return FileId;}

  bool Eof(){ return feof(FileId) != 0; }
  int Len() const { return GetSize() - GetPos(); } // bytes till eof
  char GetCh() { return char(fgetc(FileId)); }
  char PeekCh() { const char Ch = GetCh();  MovePos(-1);  return Ch; }
  int GetBf(const void* LBf, const TSize& LBfL);

  void SetPos(const int& Pos) { IAssert(fseek(FileId, Pos, SEEK_SET)==0); }
  void MovePos(const int& DPos) { IAssert(fseek(FileId, DPos, SEEK_CUR)==0); }
  int GetPos() const { return ftell(FileId); }
  int GetSize() const;
  void Clr() { Fail; }

  int PutCh(const char& Ch) { return PutBf(&Ch, sizeof(Ch)); }
  int PutBf(const void* LBf, const TSize& LBfL);
  void Flush() { IAssert(fflush(FileId) == 0); }
};

/////////////////////////////////////////////////
// Input-Memory
class TMIn: public TSIn{
private:
  char* Bf;
  int BfC, BfL;
private:
  TMIn();
  TMIn(const TMIn&);
  TMIn& operator=(const TMIn&);
public:
  TMIn(const void* _Bf, const int& _BfL, const bool& TakeBf=false);
  TMIn(TSIn& SIn);
  TMIn(const char* CStr);
  TMIn(const TStr& Str);
  TMIn(const TChA& ChA);
  static PSIn New(const char* CStr);
  static PSIn New(const TStr& Str);
  static PSIn New(const TChA& ChA);
  ~TMIn(){if (Bf!=NULL){delete[] Bf;}}

  bool Eof(){return BfC==BfL;}
  int Len() const {return BfL-BfC;}
  char GetCh();
  char PeekCh();
  int GetBf(const void* LBf, const TSize& LBfL);
  void Reset(){Cs=TCs(); BfC=0;}

  char* GetBfAddr(){return Bf;}
};

/////////////////////////////////////////////////
// Output-Memory
class TMOut: public TSOut{
private:
  char* Bf;
  int BfL, MxBfL;
  bool OwnBf;
  void Resize();
private:
  TMOut(const TMOut&);
  TMOut& operator=(const TMOut&);
public:
  TMOut(const int& _MxBfL=1024);
  static PSOut New(const int& MxBfL=1024){
    return PSOut(new TMOut(MxBfL));}
  TMOut(char* _Bf, const int& _MxBfL);
  ~TMOut(){if (OwnBf&&(Bf!=NULL)){delete[] Bf;}}

  int PutCh(const char& Ch){if (BfL==MxBfL){
    Resize();} return Bf[BfL++]=Ch;}
  int PutBf(const void* LBf, const TSize& LBfL);
  void Flush(){}

  int Len() const {return BfL;}
  void Clr(){BfL=0;}
  char GetCh(const int& ChN) const {
    IAssert((0<=ChN)&&(ChN<BfL)); return Bf[ChN];}
  TStr GetAsStr() const;
  void CutBf(const int& CutBfL);
  PSIn GetSIn(const bool& IsCut=true, const int& CutBfL=-1);
  char* GetBfAddr() const {return Bf;}

  bool IsCrLfLn() const;
  TStr GetCrLfLn();
  bool IsEolnLn() const;
  TStr GetEolnLn(const bool& DoAddEoln, const bool& DoCutBf);
  void MkEolnLn();
};

/////////////////////////////////////////////////
// Character-Returner
class TChRet{
private:
  PSIn SIn;
  char EofCh;
  char Ch;
private:
  TChRet();
  TChRet(const TChRet&);
  TChRet& operator=(const TChRet&);
public:
  TChRet(const PSIn& _SIn, const char& _EofCh=0):
    SIn(_SIn), EofCh(_EofCh), Ch(_EofCh){}

  bool Eof() const {return Ch==EofCh;}
  char GetCh(){
    if (SIn->Eof()){return Ch=EofCh;} else {return Ch=SIn->GetCh();}}
  char operator()(){return Ch;}
};

/////////////////////////////////////////////////
// Line-Returner
// J: after talking to BlazF -- can be removed from GLib
class TLnRet{
private:
  PSIn SIn;
  UndefDefaultCopyAssign(TLnRet);
public:
  TLnRet(const PSIn& _SIn): SIn(_SIn) {}

  bool NextLn(TStr& LnStr);
};

/////////////////////////////////////////////////
// Random-Access-File
ClassTP(TFRnd, PFRnd)//{
private:
  TFileId FileId;
  TSStr FNm;
  bool RecAct;
  int HdLen, RecLen;
private:
  void RefreshFPos();
private:
  TFRnd(const TFRnd&);
  TFRnd& operator=(const TFRnd&);
public:
  TFRnd(const TStr& _FNm, const TFAccess& FAccess,
   const bool& CreateIfNo=true, const int& _HdLen=-1, const int& _RecLen=-1);
  static PFRnd New(const TStr& FNm,
   const TFAccess& FAccess, const bool& CreateIfNo=true,
   const int& HdLen=-1, const int& RecLen=-1){
    return new TFRnd(FNm, FAccess, CreateIfNo, HdLen, RecLen);}
  ~TFRnd();

  TStr GetFNm() const;
  void SetHdRecLen(const int& _HdLen, const int& _RecLen){
    HdLen=_HdLen; RecLen=_RecLen; RecAct=(HdLen>=0)&&(RecLen>0);}

  void SetFPos(const int& FPos);
  void MoveFPos(const int& DFPos);
  int GetFPos();
  int GetFLen();
  bool Empty(){return GetFLen()==0;}
  bool Eof(){return GetFPos()==GetFLen();}

  void SetRecN(const int& RecN);
  int GetRecN();
  int GetRecs();

  void GetBf(void* Bf, const TSize& BfL);
  void PutBf(const void* Bf, const TSize& BfL);
  void Flush();

  void GetHd(void* Hd){IAssert(RecAct);
    int FPos=GetFPos(); SetFPos(0); GetBf(Hd, HdLen); SetFPos(FPos);}
  void PutHd(const void* Hd){IAssert(RecAct);
    int FPos=GetFPos(); SetFPos(0); PutBf(Hd, HdLen); SetFPos(FPos);}
  void GetRec(void* Rec, const int& RecN=-1){
    IAssert(RecAct); if (RecN!=-1){SetRecN(RecN);} GetBf(Rec, RecLen);}
  void PutRec(const void* Rec, const int& RecN=-1){
    IAssert(RecAct); if (RecN!=-1){SetRecN(RecN);} PutBf(Rec, RecLen);}

  void PutCs(const TCs& Cs){PutBf(&Cs, sizeof(Cs));}
  TCs GetCs(){TCs Cs; GetBf(&Cs, sizeof(Cs)); return Cs;}
  void PutCh(const char& Ch){PutBf(&Ch, sizeof(Ch));}
  void PutCh(const char& Ch, const int& Chs);
  char GetCh(){char Ch; GetBf(&Ch, sizeof(Ch)); return Ch;}
  void PutUCh(const uchar& UCh){PutBf(&UCh, sizeof(UCh));}
  uchar GetUCh(){uchar UCh; GetBf(&UCh, sizeof(UCh)); return UCh;}
  void PutInt(const int& Int){PutBf(&Int, sizeof(Int));}
  int GetInt(){int Int; GetBf(&Int, sizeof(Int)); return Int;}
  void PutUInt(const uint& UInt){PutBf(&UInt, sizeof(UInt));}
  uint GetUInt(){uint UInt; GetBf(&UInt, sizeof(UInt)); return UInt;}
  void PutStr(const TStr& Str);
  TStr GetStr(const int& StrLen);
  TStr GetStr(const int& MxStrLen, bool& IsOk);
  void PutSIn(const PSIn& SIn, TCs& Cs);
  PSIn GetSIn(const int& SInLen, TCs& Cs);

  static TStr GetStrFromFAccess(const TFAccess& FAccess);
  static TFAccess GetFAccessFromStr(const TStr& Str);
};

/////////////////////////////////////////////////
// Files
class TFile{
public:
  static const TStr TxtFExt;
  static const TStr HtmlFExt;
  static const TStr HtmFExt;
  static const TStr GifFExt;
  static const TStr JarFExt;
public:
  static bool Exists(const TStr& FNm);
  static void Copy(const TStr& SrcFNm, const TStr& DstFNm, 
    const bool& ThrowExceptP=true, const bool& FailIfExistsP=false);
  static void Del(const TStr& FNm, const bool& ThrowExceptP=true);
  static void DelWc(const TStr& WcStr, const bool& RecurseDirP=false);
  static void Rename(const TStr& SrcFNm, const TStr& DstFNm);
  static TStr GetUniqueFNm(const TStr& FNm);
  static uint64 GetSize(const TStr& FNm);
  static uint64 GetCreateTm(const TStr& FNm);
  static uint64 GetLastAccessTm(const TStr& FNm);
  static uint64 GetLastWriteTm(const TStr& FNm);
};

