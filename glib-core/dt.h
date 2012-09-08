/////////////////////////////////////////////////
// Forward
class TILx;
class TOLx;
ClassHdTP(TXmlTok, PXmlTok);

/////////////////////////////////////////////////
// Random
class TRnd{
public:
  static const int RndSeed;
private:
  static const int a, m, q, r;
  int Seed;
  int GetNextSeed(){
    if ((Seed=a*(Seed%q)-r*(Seed/q))>0){return Seed;} else {return Seed+=m;}}
public:
  TRnd(const int& _Seed=1, const int& Steps=0){
    PutSeed(_Seed); Move(Steps);}
  explicit TRnd(TSIn& SIn){SIn.Load(Seed);}
  void Save(TSOut& SOut) const {SOut.Save(Seed);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TRnd& operator=(const TRnd& Rnd){Seed=Rnd.Seed; return *this;}
  bool operator==(const TRnd&) const {Fail; return false;}

  double GetUniDev(){return GetNextSeed()/double(m);}
  int GetUniDevInt(const int& Range=0);
  int GetUniDevInt(const int& MnVal, const int& MxVal){
    IAssert(MnVal<=MxVal); return MnVal+GetUniDevInt(MxVal-MnVal+1);}
  uint GetUniDevUInt(const uint& Range=0);
  double GetNrmDev();
  double GetNrmDev(
   const double& Mean, const double& SDev, const double& Mn, const double& Mx);
  double GetExpDev();
  double GetExpDev(const double& Lambda); // mean=1/lambda
  double GetGammaDev(const int& Order);
  double GetPoissonDev(const double& Mean);
  double GetBinomialDev(const double& Prb, const int& Trials);
  int GetGeoDev(const double& Prb){
    return 1+(int)floor(log(1.0-GetUniDev())/log(1.0-Prb));}
  double GetPowerDev(const double& AlphaSlope){ // power-law degree distribution (AlphaSlope>0)
    IAssert(AlphaSlope>1.0);
    return pow(1.0-GetUniDev(), -1.0/(AlphaSlope-1.0));}
  //void GetSphereDev(const int& Dim, TFltV& ValV);

  void PutSeed(const int& _Seed);
  int GetSeed() const {return Seed;}
  void Randomize(){PutSeed(RndSeed);}
  void Move(const int& Steps);
  bool Check();

  static double GetUniDevStep(const int& Seed, const int& Steps){
    TRnd Rnd(Seed); Rnd.Move(Steps); return Rnd.GetUniDev();}
  static double GetNrmDevStep(const int& Seed, const int& Steps){
    TRnd Rnd(Seed); Rnd.Move(Steps); return Rnd.GetNrmDev();}
  static double GetExpDevStep(const int& Seed, const int& Steps){
    TRnd Rnd(Seed); Rnd.Move(Steps); return Rnd.GetExpDev();}

  static TRnd LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;
};

/////////////////////////////////////////////////
// Memory
ClassTP(TMem, PMem)//{
private:
  int MxBfL, BfL;
  char* Bf;
  void Resize(const int& _MxBfL);
  bool DoFitLen(const int& LBfL) const {return BfL+LBfL<=MxBfL;}
public:
  TMem(const int& _MxBfL=0):
    MxBfL(_MxBfL), BfL(0), Bf(NULL){ IAssert(BfL>=0);
    if (MxBfL>0){Bf=new char[MxBfL]; IAssert(Bf!=NULL);}}
  static PMem New(const int& MxBfL=0){return new TMem(MxBfL);}
  TMem(const void* _Bf, const int& _BfL):
    MxBfL(_BfL), BfL(_BfL), Bf(NULL){ IAssert(BfL>=0);
    if (BfL>0){Bf=new char[BfL]; IAssert(Bf!=NULL); memcpy(Bf, _Bf, BfL);}}
  static PMem New(const void* Bf, const int& BfL){return new TMem(Bf, BfL);}
  TMem(const TMem& Mem):
    MxBfL(Mem.MxBfL), BfL(Mem.BfL), Bf(NULL){
    if (MxBfL>0){Bf=new char[MxBfL]; memcpy(Bf, Mem.Bf, BfL);}}
  static PMem New(const TMem& Mem){return new TMem(Mem);}
  static PMem New(const PMem& Mem){return new TMem(*Mem);}
  TMem(const TStr& Str);
  static PMem New(const TStr& Str){return new TMem(Str);}
  ~TMem(){if (Bf!=NULL){delete[] Bf;}}
  explicit TMem(TSIn& SIn){
    SIn.Load(MxBfL); SIn.Load(BfL);
    Bf=new char[MxBfL=BfL]; SIn.LoadBf(Bf, BfL);}
  void Save(TSOut& SOut) const {
    SOut.Save(MxBfL); SOut.Save(BfL); SOut.SaveBf(Bf, BfL);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TMem& operator=(const TMem& Mem){
    if (this!=&Mem){
      if (Bf!=NULL){delete[] Bf;}
      MxBfL=Mem.MxBfL; BfL=Mem.BfL; Bf=NULL;
      if (MxBfL>0){Bf=new char[MxBfL]; memcpy(Bf, Mem.Bf, BfL);}}
    return *this;}
  char* operator()() const {return Bf;}
  TMem& operator+=(const char& Ch);
  TMem& operator+=(const TMem& Mem);
  TMem& operator+=(const TStr& Str);
  TMem& operator+=(const PSIn& SIn);
  char& operator[](const int& ChN) const {
    Assert((0<=ChN)&&(ChN<BfL)); return Bf[ChN];}
  int GetMemUsed() const {return 2*sizeof(int)+sizeof(char*)+MxBfL;}

  void Gen(const int& _BfL){
    Clr(); Resize(_BfL); BfL=_BfL;}
  void GenZeros(const int& _BfL){
    Clr(false); Resize(_BfL); BfL=_BfL;
    if (BfL > 0) memset(Bf, 0, BfL);}
  void Reserve(const int& _MxBfL, const bool& DoClr = true){
	  if (DoClr){ Clr(); } Resize(_MxBfL);}
  void Del(const int& BChN, const int& EChN);
  void Clr(const bool& DoDel=true){
    if (DoDel){if (Bf!=NULL){delete[] Bf;} MxBfL=0; BfL=0; Bf=NULL;}
    else {BfL=0;}}
  int Len() const {return BfL;}
  bool Empty() const {return BfL==0;}
  void Trunc(const int& _BfL){
    if ((0<=_BfL)&&(_BfL<=BfL)){BfL=_BfL;}}
  void Push(const char& Ch){operator+=(Ch);}
  char Pop(){IAssert(BfL>0); BfL--; return Bf[BfL];}

  bool DoFitStr(const TStr& Str) const;
  //int AddStr(const TStr& Str);
  void AddBf(const void* Bf, const int& BfL);
  char* GetBf() const {return Bf;}
  TStr GetAsStr(const char& NewNullCh='\0') const;
  PSIn GetSIn() const {
    TMOut MOut(BfL); MOut.SaveBf(Bf, BfL); return MOut.GetSIn();}

  static void LoadMem(const PSIn& SIn, TMem& Mem){
    Mem.Clr(); Mem.Gen(SIn->Len()); SIn->GetBf(Mem.Bf, SIn->Len());}
  static void LoadMem(const PSIn& SIn, const PMem& Mem){
    Mem->Clr(); Mem->Gen(SIn->Len()); SIn->GetBf(Mem->Bf, SIn->Len());}
  void SaveMem(const PSOut& SOut) const {SOut->SaveBf(Bf, Len());}
};

/////////////////////////////////////////////////
// Input-Memory
class TMemIn: public TSIn{
private:
  PMem Mem;
  const char* Bf;
  int BfC, BfL;
public:
  TMemIn(const TMem& _Mem, const int& _BfC=0);
  static PSIn New(const TMem& Mem){
    return PSIn(new TMemIn(Mem));}
  static PSIn New(const PMem& Mem){
    TMemIn* MemIn=new TMemIn(*Mem); MemIn->Mem=Mem; return PSIn(MemIn);}
  ~TMemIn(){}

  bool Eof(){return BfC==BfL;}
  int Len() const {return BfL-BfC;}
  char GetCh(){Assert(BfC<BfL); return Bf[BfC++];}
  char PeekCh(){Assert(BfC<BfL); return Bf[BfC];}
  int GetBf(const void* LBf, const TSize& LBfL);
  void Reset(){Cs=TCs(); BfC=0;}
};

/////////////////////////////////////////////////
// Output-Memory
class TMemOut: public TSOut{
private:
  PMem Mem;
private:
  void FlushBf();
public:
  TMemOut(const PMem& _Mem);
  static PSOut New(const PMem& Mem){
    return new TMemOut(Mem);}
  ~TMemOut(){}

  int PutCh(const char& Ch){
    Mem->operator+=(Ch); return Ch;}
  int PutBf(const void* LBf, const TSize& LBfL);
  void Flush(){}
};

/////////////////////////////////////////////////
// Char-Array
class TChA{
private:
  int MxBfL, BfL;
  char* Bf;
  void Resize(const int& _MxBfL);
public:
  explicit TChA(const int& _MxBfL=256){
    Bf=new char[(MxBfL=_MxBfL)+1]; Bf[BfL=0]=0;}
  TChA(const char* CStr){
    Bf=new char[(MxBfL=BfL=int(strlen(CStr)))+1]; strcpy(Bf, CStr);}
  TChA(const char* CStr, const int& StrLen) : MxBfL(StrLen), BfL(StrLen) {
    Bf=new char[StrLen+1]; strncpy(Bf, CStr, StrLen); Bf[StrLen]=0;}
  TChA(const TChA& ChA){
    Bf=new char[(MxBfL=ChA.MxBfL)+1]; BfL=ChA.BfL; strcpy(Bf, ChA.CStr());}
  TChA(const TStr& Str);
  TChA(const TMem& Mem){
    Bf=new char[(MxBfL=BfL=Mem.Len())+1]; Bf[MxBfL]=0;
    memcpy(CStr(), Mem(), Mem.Len());}
  ~TChA(){delete[] Bf;}
  explicit TChA(TSIn& SIn){
    SIn.Load(MxBfL); SIn.Load(BfL); SIn.Load(Bf, MxBfL, BfL);}
  void Load(TSIn& SIn){ delete[] Bf;
    SIn.Load(MxBfL); SIn.Load(BfL); SIn.Load(Bf, MxBfL, BfL);}
  void Save(TSOut& SOut, const bool& SaveCompact=true) const { //J:
    SOut.Save(SaveCompact?BfL:MxBfL); SOut.Save(BfL); SOut.Save(Bf, BfL);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TChA& operator=(const TChA& ChA);
  TChA& operator=(const TStr& Str);
  TChA& operator=(const char* CStr);
  bool operator==(const TChA& ChA) const {return strcmp(CStr(), ChA.CStr())==0;}
  bool operator==(const char* _CStr) const {return strcmp(CStr(), _CStr)==0;}
  bool operator==(const char& Ch) const {return (BfL==1)&&(Bf[0]==Ch);}
  bool operator!=(const TChA& ChA) const {return strcmp(CStr(), ChA.CStr())!=0;}
  bool operator!=(const char* _CStr) const {return strcmp(CStr(), _CStr)!=0;}
  bool operator!=(const char& Ch) const {return !((BfL==1)&&(Bf[0]==Ch));}
  bool operator<(const TChA& ChA) const {return strcmp(CStr(), ChA.CStr())<0;}

  TChA& operator+=(const TMem& Mem);
  TChA& operator+=(const TChA& ChA);
  TChA& operator+=(const TStr& Str);
  TChA& operator+=(const char* CStr);
  TChA& operator+=(const char& Ch){
    if (BfL==MxBfL){Resize(BfL+1);}
    Bf[BfL]=Ch; BfL++; Bf[BfL]=0; return *this;}
  char operator[](const int& ChN) const {
    Assert((0<=ChN)&&(ChN<BfL)); return Bf[ChN];}
  char& operator[](const int& ChN){
    Assert((0<=ChN)&&(ChN<BfL)); return Bf[ChN];}
  int GetMemUsed() const {return 2*sizeof(int)+sizeof(char*)+sizeof(char)*MxBfL;}

  char* operator ()(){return Bf;}
  const char* operator ()() const {return Bf;}
  char* CStr() {return Bf;}
  const char* CStr() const {return Bf;}

  void Clr(){Bf[BfL=0]=0;}
  int Len() const {return BfL;}
  bool Empty() const {return BfL==0;}
  void Ins(const int& BChN, const char* CStr);
  void Del(const int& ChN);
  void DelLastCh(){Pop();}
  void Push(const char& Ch){operator+=(Ch);}
  char Pop(){IAssert(BfL>0); BfL--; char Ch=Bf[BfL]; Bf[BfL]=0; return Ch;}
  void Trunc();
  void Trunc(const int& _BfL){
    if ((0<=_BfL)&&(_BfL<=BfL)){Bf[BfL=_BfL]=0;}}
  void Reverse();

  void AddCh(const char& Ch, const int& MxLen=-1){
    if ((MxLen==-1)||(BfL<MxLen)){operator+=(Ch);}}
  void AddChTo(const char& Ch, const int& ToChN){
    while (Len()<ToChN){AddCh(Ch);}}
  void PutCh(const int& ChN, const char& Ch){
    Assert((0<=ChN)&&(ChN<BfL)); Bf[ChN]=Ch;}
  char GetCh(const int& ChN) const {return operator[](ChN);}
  char LastCh() const { Assert(1<=BfL); return Bf[BfL-1]; }
  char LastLastCh() const { Assert(2<=BfL); return Bf[BfL-2]; }

  TChA GetSubStr(const int& BChN, const int& EChN) const;

  int CountCh(const char& Ch, const int& BChN=0) const;
  int SearchCh(const char& Ch, const int& BChN=0) const;
  int SearchChBack(const char& Ch, int BChN=-1) const;
  int SearchStr(const TChA& Str, const int& BChN=0) const;
  int SearchStr(const TStr& Str, const int& BChN=0) const;
  int SearchStr(const char* CStr, const int& BChN=0) const;
  bool IsStrIn(const TStr& Str) const {return SearchStr(Str)!=-1;}
  bool IsPrefix(const char* CStr, const int& BChN=0) const;
  bool IsPrefix(const TStr& Str) const;
  bool IsPrefix(const TChA& Str) const;
  bool IsSuffix(const char* CStr) const;
  bool IsSuffix(const TStr& Str) const;
  bool IsSuffix(const TChA& Str) const;

  bool IsChIn(const char& Ch) const {return SearchCh(Ch)!=-1;}
  void ChangeCh(const char& SrcCh, const char& DstCh);
  TChA& ToUc();
  TChA& ToLc();
  TChA& ToTrunc();
  void CompressWs();
  void Swap(const int& ChN1, const int& ChN2);
  void Swap(TChA& ChA);

  int GetPrimHashCd() const;
  int GetSecHashCd() const;

  static void LoadTxt(const PSIn& SIn, TChA& ChA);
  void SaveTxt(const PSOut& SOut) const;
  
  //friend TChA operator+(const TChA& LStr, const TChA& RStr);
  //friend TChA operator+(const TChA& LStr, const TStr& RStr);
  //friend TChA operator+(const TChA& LStr, const char* RCStr);
};

/////////////////////////////////////////////////
// Input-Char-Array
class TChAIn: public TSIn{
private:
  const char* Bf;
  int BfC, BfL;
private:
  TChAIn();
  TChAIn(const TChAIn&);
  TChAIn& operator=(const TChAIn&);
public:
  TChAIn(const TChA& ChA, const int& _BfC=0);
  static PSIn New(const TChA& ChA){return PSIn(new TChAIn(ChA));}
  ~TChAIn(){}

  bool Eof(){return BfC==BfL;}
  int Len() const {return BfL-BfC;}
  char GetCh(){Assert(BfC<BfL); return Bf[BfC++];}
  char PeekCh(){Assert(BfC<BfL); return Bf[BfC];}
  int GetBf(const void* LBf, const TSize& LBfL);
  void Reset(){Cs=TCs(); BfC=0;}
};

/////////////////////////////////////////////////
// Ref-String
class TRStr{
public:
  char* Bf;
  int Refs;
public:
  TRStr(){Refs=1; Bf=new char[0+1]; Bf[0]=0;}
  TRStr(const int& Len){
    IAssert(Len>=0); Refs=0; Bf=new char[Len+1]; Bf[Len]=0;}
  TRStr(const char* CStr){
    Refs=0; Bf=new char[strlen(CStr)+1]; strcpy(Bf, CStr);}
  TRStr(const char* CStr, const int& MxLen){
    Refs=0; Bf=new char[MxLen+1]; strncpy(Bf, CStr, MxLen); Bf[MxLen]=0;}
  TRStr(const char* CStr1, const char* CStr2){
    Refs=0; int CStr1Len=int(strlen(CStr1)); Bf=new char[CStr1Len+int(strlen(CStr2))+1];
    strcpy(Bf, CStr1); strcpy(Bf+CStr1Len, CStr2);}
  TRStr(const char& Ch){
    Refs=0; Bf=new char[1+1]; Bf[0]=Ch; Bf[1]=0;}
  TRStr(const char& Ch1, const char& Ch2){
    Refs=0; Bf=new char[2+1]; Bf[0]=Ch1; Bf[1]=Ch2; Bf[2]=0;}
  ~TRStr(){
    Assert(((this!=GetNullRStr())&&(Refs==0))||((this==GetNullRStr())&&(Refs==1)));
    delete[] Bf;}
  explicit TRStr(TSIn& SIn, const bool& IsSmall){
    if (IsSmall){Refs=0; SIn.Load(Bf);}
    else {Refs=0; int BfL; SIn.Load(BfL); SIn.Load(Bf, BfL, BfL);}}
  void Save(TSOut& SOut, const bool& IsSmall) const {
    if (IsSmall){SOut.Save(Bf);}
    else {int BfL=int(strlen(Bf)); SOut.Save(BfL); SOut.Save(Bf, BfL);}}

  TRStr& operator=(const TRStr&){Fail; return *this;}
  int GetMemUsed() const {return int(sizeof(int))+int(strlen(Bf));}

  void MkRef(){Refs++;}
  void UnRef(){Assert(Refs>0); if (--Refs==0){delete this;}}

  const char* CStr() const {return Bf;}
  char* CStr() {return Bf;}
  bool Empty() const {return Bf[0]==0;}
  int Len() const {return int(strlen(Bf));}

  void PutCh(const int& ChN, const char& Ch){
    Assert((0<=ChN)&&(ChN<Len())); Bf[ChN]=Ch;}
  char GetCh(const int& ChN) const {
    Assert((0<=ChN)&&(ChN<Len())); return Bf[ChN];}

  bool IsUc() const;
  void ToUc();
  bool IsLc() const;
  void ToLc();
  void ToCap();
  void ConvUsFromYuAscii();
  static int CmpI(const char* CStr1, const char* CStr2);

  int GetPrimHashCd() const;
  int GetSecHashCd() const;

  static TRStr* GetNullRStr(){
    static TRStr NullRStr; Assert(NullRStr.Bf!=NULL); return &NullRStr;}
};

/////////////////////////////////////////////////
// String
class TStr;
template <class TVal> class TVec;
typedef TVec<TStr> TStrV;

class TStr{
private:
  TRStr* RStr;
private:
  TStr(const char& Ch, bool){
    RStr=new TRStr(Ch); RStr->MkRef();}
  TStr(const char& Ch1, const char& Ch2, bool){
    RStr=new TRStr(Ch1, Ch2); RStr->MkRef();}
  static TRStr* GetRStr(const char* CStr);
  void Optimize();
public:
  TStr(){RStr=TRStr::GetNullRStr(); RStr->MkRef();}
  TStr(const TStr& Str){RStr=Str.RStr; RStr->MkRef();}
  TStr(const TChA& ChA){RStr=GetRStr(ChA.CStr()); RStr->MkRef();}
  TStr(const TSStr& SStr){RStr=GetRStr(SStr.CStr()); RStr->MkRef();}
  TStr(const char* CStr){RStr=GetRStr(CStr); RStr->MkRef();}
  explicit TStr(const char& Ch){RStr=GetChStr(Ch).RStr; RStr->MkRef();}
  TStr(const TMem& Mem){
    RStr=new TRStr(Mem.Len()); RStr->MkRef();
    memcpy(CStr(), Mem(), Mem.Len()); Optimize();}
  explicit TStr(const PSIn& SIn){
    int SInLen=SIn->Len(); RStr=new TRStr(SInLen); RStr->MkRef();
    SIn->GetBf(CStr(), SInLen); Optimize();}
  ~TStr(){RStr->UnRef();}
  explicit TStr(TSIn& SIn, const bool& IsSmall=false):
    RStr(new TRStr(SIn, IsSmall)){RStr->MkRef(); Optimize();}
  void Load(TSIn& SIn, const bool& IsSmall=false){
    *this=TStr(SIn, IsSmall);}
  void Save(TSOut& SOut, const bool& IsSmall=false) const {
    RStr->Save(SOut, IsSmall);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TStr& operator=(const TStr& Str){
    if (this!=&Str){RStr->UnRef(); RStr=Str.RStr; RStr->MkRef();} return *this;}
  TStr& operator=(const TChA& ChA){
    RStr->UnRef(); RStr=GetRStr(ChA.CStr()); RStr->MkRef(); return *this;}
  TStr& operator=(const char* CStr){
    RStr->UnRef(); RStr=GetRStr(CStr); RStr->MkRef(); return *this;}
  TStr& operator=(const char& Ch){
    RStr->UnRef(); RStr=GetChStr(Ch).RStr; RStr->MkRef(); return *this;}
  TStr& operator+=(const TStr& Str){
    TRStr* NewRStr=new TRStr(RStr->CStr(), Str.RStr->CStr());
    RStr->UnRef(); RStr=NewRStr; RStr->MkRef();
    Optimize(); return *this;}
  TStr& operator+=(const char* CStr){
    TRStr* NewRStr=new TRStr(RStr->CStr(), CStr);
    RStr->UnRef(); RStr=NewRStr; RStr->MkRef();
    Optimize(); return *this;}
  bool operator==(const TStr& Str) const {
    return (RStr==Str.RStr)||(strcmp(RStr->CStr(), Str.RStr->CStr())==0);}
  bool operator==(const char* CStr) const {
    return strcmp(RStr->CStr(), CStr)==0;}
//  bool operator!=(const TStr& Str) const {
//    return strcmp(RStr->CStr(), Str.RStr->CStr())!=0;}
  bool operator!=(const char* CStr) const {
    return strcmp(RStr->CStr(), CStr)!=0;}
  bool operator<(const TStr& Str) const {
    return strcmp(RStr->CStr(), Str.RStr->CStr())<0;}
  char operator[](const int& ChN) const {return RStr->GetCh(ChN);}
  int GetMemUsed() const {return sizeof(TRStr*)+RStr->GetMemUsed();}

  char* operator()(){return RStr->CStr();}
  const char* operator()() const {return RStr->CStr();}
  char* CStr() {return RStr->CStr();}
  const char* CStr() const {return RStr->CStr();}

  void PutCh(const int& ChN, const char& Ch){
    TRStr* NewRStr=new TRStr(RStr->CStr());
    RStr->UnRef(); RStr=NewRStr; RStr->MkRef();
    RStr->PutCh(ChN, Ch); Optimize();}
  char GetCh(const int& ChN) const {return RStr->GetCh(ChN);}
  char LastCh() const {return GetCh(Len()-1);}

  void Clr(){RStr->UnRef(); RStr=TRStr::GetNullRStr(); RStr->MkRef();}
  int Len() const {return RStr->Len();}
  bool Empty() const {return RStr->Empty();}

  // upper-case
  bool IsUc() const {return RStr->IsUc();}
  TStr& ToUc();
  TStr GetUc() const {return TStr(*this).ToUc();}
  int CmpI(const TStr& Str) const {return TRStr::CmpI(CStr(), Str.CStr());}
  bool EqI(const TStr& Str) const {return TRStr::CmpI(CStr(), Str.CStr())==0;}
  // lower-case
  bool IsLc() const {return RStr->IsLc();}
  TStr& ToLc();
  TStr GetLc() const {return TStr(*this).ToLc();}
  // capitalize
  TStr& ToCap();
  TStr GetCap() const {return TStr(*this).ToCap();}

  // truncate
  TStr& ToTrunc();
  TStr GetTrunc() const {return TStr(*this).ToTrunc();}
  // Yu-Ascii to Us-Ascii
  TStr& ConvUsFromYuAscii();
  TStr GetUsFromYuAscii() const {return TStr(*this).ConvUsFromYuAscii();}
  // hex
  TStr& ToHex();
  TStr GetHex() const {return TStr(*this).ToHex();}
  TStr& FromHex();
  TStr GetFromHex() const {return TStr(*this).FromHex();}

  TStr GetSubStr(const int& BChN, const int& EChN) const;
  TStr GetSubStr(const int& BChN) const { return GetSubStr(BChN, Len()-1); }
  void InsStr(const int& BChN, const TStr& Str);
  void DelChAll(const char& Ch);
  void DelSubStr(const int& BChN, const int& EChN);
  bool DelStr(const TStr& Str);
  TStr LeftOf(const char& SplitCh) const;
  TStr LeftOfLast(const char& SplitCh) const;
  TStr RightOf(const char& SplitCh) const;
  TStr RightOfLast(const char& SplitCh) const;
  void SplitOnCh(TStr& LStr, const char& SplitCh, TStr& RStr) const;
  void SplitOnLastCh(TStr& LStr, const char& SplitCh, TStr& RStr) const;
  void SplitOnAllCh(
   const char& SplitCh, TStrV& StrV, const bool& SkipEmpty=true) const;
  void SplitOnAllAnyCh(
   const TStr& SplitChStr, TStrV& StrV, const bool& SkipEmpty=true) const;
  void SplitOnWs(TStrV& StrV) const;
  void SplitOnNonAlNum(TStrV& StrV) const;
  void SplitOnStr(const TStr& SplitStr, TStrV& StrV) const;
  void SplitOnStr(TStr& LeftStr, const TStr& MidStr, TStr& RightStr) const;

  //TStr Left(const int& Chs) const { return Chs>0 ? GetSubStr(0, Chs-1) : GetSubStr(0, Len()-Chs-1);}
  //TStr Right(const int& Chs) const {return GetSubStr(Len()-Chs, Len()-1);}
  TStr Mid(const int& BChN, const int& Chs) const { return GetSubStr(BChN, BChN+Chs-1); }
  TStr Mid(const int& BChN) const {return GetSubStr(BChN, Len()-1); }
  //TStr Slice(const int& BChN, const int& EChNP1) const {return GetSubStr(BChN, EChNP1-1);}
  //TStr operator()(const int& BChN, const int& EChNP1) const {return Slice(BChN, EChNP1);}
  //J: as in python or matlab: 1 is 1st character, -1 is last character
  // TODO ROK, ask Jure about this comment
  TStr Left(const int& EChN) const { return EChN>0 ? GetSubStr(0, EChN-1) : GetSubStr(0, Len()+EChN-1);}
  TStr Right(const int& BChN) const {return BChN>=0 ? GetSubStr(BChN, Len()-1) : GetSubStr(Len()+BChN, Len()-1);}
  TStr Slice(int BChN, int EChNP1) const { if(BChN<0){BChN=Len()+BChN;} if(EChNP1<=0){EChNP1=Len()+EChNP1;} return GetSubStr(BChN, EChNP1-1); }
  TStr operator()(const int& BChN, const int& EChNP1) const {return Slice(BChN, EChNP1);}

  int CountCh(const char& Ch, const int& BChN=0) const;
  int SearchCh(const char& Ch, const int& BChN=0) const;
  int SearchChBack(const char& Ch, int BChN=-1) const;
  int SearchStr(const TStr& Str, const int& BChN=0) const;
  bool IsChIn(const char& Ch) const {return SearchCh(Ch)!=-1;}
  bool IsStrIn(const TStr& Str) const {return SearchStr(Str)!=-1;}
  bool IsPrefix(const char *Str) const;
  bool IsPrefix(const TStr& Str) const {
    return IsPrefix(Str.CStr());}
  bool IsSuffix(const char *Str) const;
  bool IsSuffix(const TStr& Str) const {
    return IsSuffix(Str.CStr());}

  int ChangeCh(const char& SrcCh, const char& DstCh, const int& BChN=0);
  int ChangeChAll(const char& SrcCh, const char& DstCh);
  int ChangeStr(const TStr& SrcStr, const TStr& DstStr, const int& BChN=0);
  int ChangeStrAll(const TStr& SrcStr, const TStr& DstStr, const bool& FromStartP=false);
  TStr Reverse() const {
    TChA ChA(*this); ChA.Reverse(); return ChA;}

  int GetPrimHashCd() const {return RStr->GetPrimHashCd();}
  int GetSecHashCd() const {return RStr->GetSecHashCd();}

  bool IsBool(bool& Val) const;

  bool IsInt(
   const bool& Check, const int& MnVal, const int& MxVal, int& Val) const;
  bool IsInt(int& Val) const {return IsInt(false, 0, 0, Val);}
  bool IsInt() const {int Val; return IsInt(false, 0, 0, Val);}
  int GetInt() const {int Val; IAssertR(IsInt(false, 0, 0, Val), *this); return Val;}
  int GetInt(const int& DfVal) const {
    int Val; if (IsInt(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsUInt(
   const bool& Check, const uint& MnVal, const uint& MxVal, uint& Val) const;
  bool IsUInt(uint& Val) const {return IsUInt(false, 0, 0, Val);}
  bool IsUInt() const {uint Val; return IsUInt(false, 0, 0, Val);}
  uint GetUInt() const {uint Val; IAssert(IsUInt(false, 0, 0, Val)); return Val;}
  uint GetUInt(const uint& DfVal) const {
    uint Val; if (IsUInt(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsInt64(
   const bool& Check, const int64& MnVal, const int64& MxVal, int64& Val) const;
  bool IsInt64(int64& Val) const {return IsInt64(false, 0, 0, Val);}
  bool IsInt64() const {int64 Val; return IsInt64(false, 0, 0, Val);}
  int64 GetInt64() const {
    int64 Val; IAssert(IsInt64(false, 0, 0, Val)); return Val;}
  int64 GetInt64(const int64& DfVal) const {
    int64 Val; if (IsInt64(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsUInt64(
   const bool& Check, const uint64& MnVal, const uint64& MxVal, uint64& Val) const;
  bool IsUInt64(uint64& Val) const {return IsUInt64(false, 0, 0, Val);}
  bool IsUInt64() const {uint64 Val; return IsUInt64(false, 0, 0, Val);}
  uint64 GetUInt64() const {
    uint64 Val; IAssert(IsUInt64(false, 0, 0, Val)); return Val;}
  uint64 GetUInt64(const uint64& DfVal) const {
    uint64 Val; if (IsUInt64(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsHexInt(const bool& Check, const int& MnVal, const int& MxVal, int& Val) const;
  bool IsHexInt(int& Val) const {return IsHexInt(false, 0, 0, Val);}
  bool IsHexInt() const {int Val; return IsHexInt(false, 0, 0, Val);}
  int GetHexInt() const {
    int Val; IAssert(IsHexInt(false, 0, 0, Val)); return Val;}
  int GetHexInt(const int& DfVal) const {
    int Val; if (IsHexInt(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsHexInt64(const bool& Check, const int64& MnVal, const int64& MxVal, int64& Val) const;
  bool IsHexInt64(int64& Val) const {return IsHexInt64(false, 0, 0, Val);}
  bool IsHexInt64() const {int64 Val; return IsHexInt64(false, 0, 0, Val);}
  int64 GetHexInt64() const {
    int64 Val; IAssert(IsHexInt64(false, 0, 0, Val)); return Val;}
  int64 GetHexInt64(const int64& DfVal) const {
    int64 Val; if (IsHexInt64(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsFlt(const bool& Check, const double& MnVal, const double& MxVal,
   double& Val, const char& DecDelimCh='.') const;
  bool IsFlt(double& Val) const {return IsFlt(false, 0, 0, Val);}
  bool IsFlt() const {double Val; return IsFlt(false, 0, 0, Val);}
  double GetFlt() const {
    double Val; IAssert(IsFlt(false, 0, 0, Val)); return Val;}
  double GetFlt(const double& DfVal) const {
    double Val; if (IsFlt(false, 0, 0, Val)){return Val;} else {return DfVal;}}

  bool IsWord(const bool& WsPrefixP=true, const bool& FirstUcAllowedP=true) const;
  bool IsWs() const;

  bool IsWcMatch(
   const int& StrBChN, const TStr& WcStr, const int& WcStrBChN, TStrV& StarStrV,
   const char& StarCh='*', const char& QuestCh='?') const;
  bool IsWcMatch(
   const TStr& WcStr, TStrV& StarStrV,
   const char& StarCh='*', const char& QuestCh='?') const;
  bool IsWcMatch(const TStr& WcStr, const char& StarCh, const char& QuestCh) const;
  bool IsWcMatch(const TStr& WcStr, const int& StarStrN, TStr& StarStr) const;
  bool IsWcMatch(const TStr& WcStr) const;
  TStr GetWcMatch(const TStr& WcStr, const int& StarStrN=0) const;

  TStr GetFPath() const;
  TStr GetFBase() const;
  TStr GetFMid() const;
  TStr GetFExt() const;
  static TStr GetNrFPath(const TStr& FPath);
  static TStr GetNrFMid(const TStr& FMid);
  static TStr GetNrFExt(const TStr& FExt);
  static TStr GetNrNumFExt(const int& FExtN);
  static TStr GetNrFNm(const TStr& FNm);
  static TStr GetNrAbsFPath(const TStr& FPath, const TStr& BaseFPath=TStr());
  static bool IsAbsFPath(const TStr& FPath);
  static TStr PutFExt(const TStr& FNm, const TStr& FExt);
  static TStr PutFExtIfEmpty(const TStr& FNm, const TStr& FExt);
  static TStr PutFBase(const TStr& FNm, const TStr& FBase);
  static TStr PutFBaseIfEmpty(const TStr& FNm, const TStr& FBase);
  static TStr AddToFMid(const TStr& FNm, const TStr& ExtFMid);
  static TStr GetNumFNm(const TStr& FNm, const int& Num);
  static TStr GetFNmStr(const TStr& Str, const bool& AlNumOnlyP=true);

  static TStr LoadTxt(const PSIn& SIn){
    return TStr(SIn);}
  static TStr LoadTxt(const TStr& FNm){
    PSIn SIn=TFIn::New(FNm); return LoadTxt(SIn);}
  void SaveTxt(const PSOut& SOut) const {
    SOut->SaveBf(CStr(), Len());}
  void SaveTxt(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut);}

  static TStr& GetChStr(const char& Ch);
  static TStr& GetDChStr(const char& Ch1, const char& Ch2);

  TStr GetStr() const {return *this;}
  static TStr GetStr(const TStr& Str, const char* FmtStr);
  static TStr GetStr(const TStr& Str, const TStr& FmtStr){
    return GetStr(Str, FmtStr.CStr());}
  static TStr GetStr(const TStrV& StrV, const TStr& DelimiterStr);
  static TStr Fmt(const char *FmtStr, ...);
  static TStr GetSpaceStr(const int& Spaces);
  char* GetCStr() const {
    char* Bf=new char[Len()+1]; strcpy(Bf, CStr()); return Bf;}

  static TStr MkClone(const TStr& Str){return TStr(Str.CStr());}
  static TStr GetNullStr();

  friend TStr operator+(const TStr& LStr, const TStr& RStr);
  friend TStr operator+(const TStr& LStr, const char* RCStr);
};

/////////////////////////////////////////////////
// Input-String
class TStrIn: public TSIn{
private:
  TStr Str;
  char* Bf;
  int BfC, BfL;
private:
  TStrIn();
  TStrIn(const TStrIn&);
  TStrIn& operator = (const TStrIn&);
public:
  TStrIn(const TStr& _Str);
  static PSIn New(const TStr& Str){return PSIn(new TStrIn(Str));}
  ~TStrIn(){}

  bool Eof(){return BfC==BfL;}
  int Len() const {return BfL-BfC;}
  char GetCh(){Assert(BfC<BfL); return Bf[BfC++];}
  char PeekCh(){Assert(BfC<BfL); return Bf[BfC];}
  int GetBf(const void* LBf, const TSize& LBfL);
  void Reset(){Cs=TCs(); BfC=0;}
};

/////////////////////////////////////////////////
// Double-String
class TDbStr{
public:
  TStr Str1;
  TStr Str2;
public:
  TDbStr(): Str1(), Str2(){}
  TDbStr(const TDbStr& DbStr): Str1(DbStr.Str1), Str2(DbStr.Str2){}
  TDbStr(const TStr& _Str1): Str1(_Str1), Str2(){}
  TDbStr(const TStr& _Str1, const TStr& _Str2): Str1(_Str1), Str2(_Str2){}
  explicit TDbStr(TSIn& SIn): Str1(SIn), Str2(SIn){}
  void Save(TSOut& SOut) const {Str1.Save(SOut); Str2.Save(SOut);}

  TDbStr& operator=(const TDbStr& DbStr){
    if (this!=&DbStr){Str1=DbStr.Str1; Str2=DbStr.Str2;} return *this;}
  bool operator==(const TDbStr& DbStr) const {
    return (Str1==DbStr.Str1)&&(Str2==DbStr.Str2);}
  bool operator<(const TDbStr& DbStr) const {
    return (Str1<DbStr.Str1)||((Str1==DbStr.Str1)&&(Str2<DbStr.Str2));}

  TStr GetStr(const TStr& MidStr=TStr()) const {
    if (Filled()){return Str1+MidStr+Str2;} else {return Str1+Str2;}}
  int GetPrimHashCd() const {
    return Str1.GetPrimHashCd()+Str2.GetPrimHashCd();}
  int GetSecHashCd() const {
    return Str1.GetSecHashCd()+Str2.GetSecHashCd();}

  bool Empty() const {return (Str1.Empty())&&(Str2.Empty());}
  bool Filled() const {return (!Str2.Empty())&&(!Str1.Empty());}
};

/////////////////////////////////////////////////
// Simple-String-Pool
//ClassTP(TSStrPool, PSStrPool)//{
//private:
//  TMem Bf;
//public:
//  TSStrPool(const int& MxLen=0): Bf(MxLen){}
//  TSStrPool(TSStrPool& StrPool): Bf(StrPool.Bf){}
//  TSStrPool(TSIn& SIn): Bf(SIn){}
//  void Save(TSOut& SOut) const {Bf.Save(SOut);}
//
//  TSStrPool& operator=(const TSStrPool& StrPool){
//    Bf=StrPool.Bf; return *this;}
//
//  int Len() const {return Bf.Len();}
//  void Clr(){Bf.Clr();}
//  int AddStr(const TStr& Str){
//    if (Str.Empty()){return -1;}
//    else {int StrId=Bf.Len(); Bf+=Str; Bf+=char(0); return StrId;}}
//  TStr GetStr(const int& StrId) const {
//    if (StrId==-1){return "";}
//    else {return TStr(Bf()+StrId);}}
//};

/////////////////////////////////////////////////
// String-Pool
ClassTP(TStrPool, PStrPool)//{
private:
  uint MxBfL, BfL, GrowBy;
  char *Bf;
private:
  void Resize(uint _MxBfL);
public:
  TStrPool(uint MxBfLen = 0, uint _GrowBy = 16*1024*1024);
  TStrPool(TSIn& SIn, bool LoadCompact = true);
  TStrPool(const TStrPool& Pool) : MxBfL(Pool.MxBfL), BfL(Pool.BfL), GrowBy(Pool.GrowBy) {
    Bf = (char *) malloc(Pool.MxBfL); IAssertR(Bf, TStr::Fmt("Can not resize buffer to %u bytes. [Program failed to allocate more memory. Solution: Get a bigger machine.]", MxBfL).CStr()); memcpy(Bf, Pool.Bf, Pool.BfL); }
  ~TStrPool() { if (Bf) free(Bf); else IAssertR(MxBfL == 0, TStr::Fmt("size: %u, expected size: 0", MxBfL).CStr());  Bf = 0; MxBfL = 0; BfL = 0; }

  static PStrPool New(uint _MxBfLen = 0, uint _GrowBy = 16*1024*1024) { return PStrPool(new TStrPool(_MxBfLen, _GrowBy)); }
  static PStrPool New(TSIn& SIn) { return new TStrPool(SIn); }
  static PStrPool New(const TStr& fileName) { PSIn SIn = TFIn::New(fileName); return new TStrPool(*SIn); }
  static PStrPool Load(TSIn& SIn, bool LoadCompacted = true) { return PStrPool(new TStrPool(SIn, LoadCompacted)); }
  void Save(TSOut& SOut) const;
  void Save(const TStr& FNm){PSOut SOut=TFOut::New(FNm); Save(*SOut);}

  uint Len() const { return BfL; }
  uint Size() const { return MxBfL; }
  bool Empty() const { return ! Len(); }
  char* operator () () const { return Bf; }
  TStrPool& operator = (const TStrPool& Pool);

  uint AddStr(const char *Str, uint Len);
  uint AddStr(const char *Str) { return AddStr(Str, uint(strlen(Str)) + 1); }
  uint AddStr(const TStr& Str) { return AddStr(Str.CStr(), Str.Len() + 1); }

  TStr GetStr(uint Offset) const { Assert(Offset < BfL);
    if (Offset == 0) return TStr::GetNullStr(); else return TStr(Bf + Offset); }
  const char *GetCStr(uint Offset) const { Assert(Offset < BfL);
    if (Offset == 0) return TStr::GetNullStr().CStr(); else return Bf + Offset; }

  // Clr() removes the empty string at the start.
  // Call AddStr("") after Clr(), if you want to use the pool again.
  void Clr(bool DoDel = false) { BfL = 0; if (DoDel && Bf) { free(Bf); Bf = 0; MxBfL = 0; } }
  int Cmp(uint Offset, const char *Str) const { Assert(Offset < BfL);
    if (Offset != 0) return strcmp(Bf + Offset, Str); else return strcmp("", Str); }

  static int GetPrimHashCd(const char *CStr);
  static int GetSecHashCd(const char *CStr);
  int GetPrimHashCd(uint Offset) { Assert(Offset < BfL);
    if (Offset != 0) return GetPrimHashCd(Bf + Offset); else return GetPrimHashCd(""); }
  int GetSecHashCd(uint Offset) { Assert(Offset < BfL);
    if (Offset != 0) return GetSecHashCd(Bf + Offset); else return GetSecHashCd(""); }
};

/////////////////////////////////////////////////
// String-Pool-64bit
ClassTP(TStrPool64, PStrPool64)//{
private:
  ::TSize MxBfL, BfL, GrowBy;
  char *Bf;
private:
  void Resize(const ::TSize& _MxBfL);
public:
  TStrPool64(::TSize _MxBfL = 0, ::TSize _GrowBy = 16*1024*1024);
  TStrPool64(const TStrPool64& StrPool);
  TStrPool64(TSIn& SIn, bool LoadCompact = true);
  ~TStrPool64() { Clr(true); }
  void Save(TSOut& SOut) const;

  static PStrPool64 New(::TSize MxBfL = 0, ::TSize GrowBy = 16*1024*1024) { 
      return PStrPool64(new TStrPool64(MxBfL, GrowBy)); }
  static PStrPool64 Load(TSIn& SIn, bool LoadCompact = true) { 
      return PStrPool64(new TStrPool64(SIn, LoadCompact)); }

  TStrPool64& operator=(const TStrPool64& StrPool);

  uint64 GetMemUsed() const { return 3*sizeof(::TSize) + uint64(MxBfL); }

  bool Empty() const { return (BfL == 0); }
  uint64 Len() const {return BfL;}
  uint64 Reserved() const { return MxBfL; }
  void Clr(bool DoDel = false);
  int Cmp(uint64 Offset, const char *Str) const { Assert(Offset < BfL);
    if (Offset != 0) return strcmp(Bf + Offset, Str); else return strcmp("", Str); }

  uint64 AddStr(const TStr& Str);
  TStr GetStr(const uint64& StrId) const;
};

/////////////////////////////////////////////////
// Void
class TVoid{
public:
  TVoid(){}
  TVoid(TSIn&){}
  void Save(TSOut&) const {}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TVoid& operator=(const TVoid&){return *this;}
  bool operator==(const TVoid&) const {return true;}
  bool operator<(const TVoid&) const {Fail; return false;}
  int GetMemUsed() const {return sizeof(TVoid);}
};

/////////////////////////////////////////////////
// Boolean
class TBool{
public:
  bool Val;
public:
  static const bool Mn;
  static const bool Mx;
  static const int Vals;
  static TRnd Rnd;

  static const TStr FalseStr;
  static const TStr TrueStr;
  static const TStr NStr;
  static const TStr YStr;
  static const TStr NoStr;
  static const TStr YesStr;

  TBool(): Val(false){}
  TBool(const bool& _Val): Val(_Val){}
  operator bool() const {return Val;}
  explicit TBool(TSIn& SIn){SIn.Load(Val);}
  void Load(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TBool& operator=(const TBool& Bool){Val=Bool.Val; return *this;}
  bool operator==(const TBool& Bool) const {return Val==Bool.Val;}
  bool operator<(const TBool& Bool) const {//return Val<Bool.Val;
    return (Val==false)&&(Bool.Val==true);}
  bool operator()() const {return Val;}
  int GetMemUsed() const {return sizeof(TBool);}

  int GetPrimHashCd() const {return Val;}
  int GetSecHashCd() const {return Val;}

  static bool GetRnd(){return Rnd.GetUniDevInt(2)==1;}

  static TStr GetStr(const bool& Val){
    if (Val){return TrueStr;} else {return FalseStr;}}
  static TStr GetStr(const TBool& Bool){
    return GetStr(Bool.Val);}
  static TStr GetYNStr(const bool& Val){
    if (Val){return YStr;} else {return NStr;}}
  static TStr GetYesNoStr(const bool& Val){
    if (Val){return YesStr;} else {return NoStr;}}
  static TStr Get01Str(const bool& Val){
    if (Val){return "1";} else {return "0";}}
  static bool IsValStr(const TStr& Str);
  static bool GetValFromStr(const TStr& Str);
  static bool GetValFromStr(const TStr& Str, const bool& DfVal);
};

/////////////////////////////////////////////////
// Char
class TCh{
public:
  char Val;
public:
  static const char Mn;
  static const char Mx;
  static const int Vals;

  static const char NullCh;
  static const char TabCh;
  static const char LfCh;
  static const char CrCh;
  static const char EofCh;
  static const char HashCh;

  TCh(): Val(TCh::NullCh){}
  TCh(const char& _Val): Val(_Val){}
  operator char() const {return Val;}
  explicit TCh(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TCh& operator=(const TCh& Ch){Val=Ch.Val; return *this;}
  bool operator==(const TCh& Ch) const {return Val==Ch.Val;}
  bool operator<(const TCh& Ch) const {return Val<Ch.Val;}
  char operator()() const {return Val;}
  int GetMemUsed() const {return sizeof(TCh);}

  int GetPrimHashCd() const {return Val;}
  int GetSecHashCd() const {return Val;}

  static bool IsWs(const char& Ch){
    return (Ch==' ')||(Ch==TabCh)||(Ch==CrCh)||(Ch==LfCh);}
  static bool IsAlpha(const char& Ch){
    return (('A'<=Ch)&&(Ch<='Z'))||(('a'<=Ch)&&(Ch<='z'));}
  static bool IsNum(const char& Ch){return ('0'<=Ch)&&(Ch<='9');}
  static bool IsAlNum(const char& Ch){return IsAlpha(Ch)||IsNum(Ch);}
  static int GetNum(const char& Ch){Assert(IsNum(Ch)); return Ch-'0';}
  static bool IsHex(const char& Ch){return
    (('0'<=Ch)&&(Ch<='9'))||(('A'<=Ch)&&(Ch<='F'))||(('a'<=Ch)&&(Ch<='f'));}
  static int GetHex(const char& Ch){
    if (('0'<=Ch)&&(Ch<='9')){return Ch-'0';}
    else if (('A'<=Ch)&&(Ch<='F')){return Ch-'A'+10;}
    else if (('a'<=Ch)&&(Ch<='f')){return Ch-'a'+10;}
    else Fail; return 0;}
  static char GetHexCh(const int& Val){
    if ((0<=Val)&&(Val<=9)){return char('0'+char(Val));}
    else if ((10<=Val)&&(Val<=15)){return char('A'+char(Val-10));}
    else Fail; return 0;}
  static char IsUc(const char& Ch){
    return ('A'<=Ch)&&(Ch<='Z');}
  static char GetUc(const char& Ch){
    if (('a'<=Ch)&&(Ch<='z')){return Ch-'a'+'A';} else {return Ch;}}
  static char GetUsFromYuAscii(const char& Ch);

  static TStr GetStr(const TCh& Ch){
    return TStr(Ch.Val);}
};

/////////////////////////////////////////////////
// Unsigned-Char
class TUCh{
public:
  uchar Val;
public:
  static const uchar Mn;
  static const uchar Mx;
  static const int Vals;

  TUCh(): Val(TCh::NullCh){}
  TUCh(const uchar& _Val): Val(_Val){}
  operator uchar() const {return Val;}
  explicit TUCh(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TUCh& operator=(const TUCh& UCh){Val=UCh.Val; return *this;}
  bool operator==(const TUCh& UCh) const {return Val==UCh.Val;}
  bool operator<(const TUCh& UCh) const {return Val<UCh.Val;}
  uchar operator()() const {return Val;}
  int GetMemUsed() const {return sizeof(TUCh);}

  int GetPrimHashCd() const {return Val;}
  int GetSecHashCd() const {return Val;}
};

/////////////////////////////////////////////////
// Short-Integer
class TSInt{
public:
  int16 Val;
public:
  TSInt(): Val(0){}
  TSInt(const int16& _Val): Val(_Val){}
  operator int16() const {return Val;}
  explicit TSInt(TSIn& SIn){SIn.Load(Val);}
  void Load(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  int GetPrimHashCd() const {return Val;}
  int GetSecHashCd() const {return Val/0x10;}
};

/////////////////////////////////////////////////
// Integer
class TInt{
public:
  int Val;
public:
  static const int Mn;
  static const int Mx;
  static const int Kilo;
  static const int Mega;
  static const int Giga;
  static TRnd Rnd;

  TInt(): Val(0){}
  TInt(const int& _Val): Val(_Val){}
  operator int() const {return Val;}
  explicit TInt(TSIn& SIn){SIn.Load(Val);}
  void Load(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TInt& operator=(const TInt& Int){Val=Int.Val; return *this;}
  TInt& operator=(const int& Int){Val=Int; return *this;}
  bool operator==(const TInt& Int) const {return Val==Int.Val;}
  bool operator==(const int& Int) const {return Val==Int;}
  bool operator!=(const int& Int) const {return Val!=Int;}
  bool operator<(const TInt& Int) const {return Val<Int.Val;}
  bool operator<(const int& Int) const {return Val<Int;}
  int operator()() const {return Val;}
  TInt& operator+=(const int& Int){Val+=Int; return *this;}
  TInt& operator-=(const int& Int){Val-=Int; return *this;}
  TInt operator++(int){Val++; return *this;}
  TInt operator--(int){Val--; return *this;}
  int GetMemUsed() const {return sizeof(TInt);}

  int GetPrimHashCd() const {return Val;}
  int GetSecHashCd() const {return Val/0x10;}

  static int Abs(const int& Int){return Int<0?-Int:Int;}
  static int Sign(const int& Int){return Int<0?-1:(Int>0?1:0);}
  static void Swap(int& Int1, int& Int2){
    int SwapInt1=Int1; Int1=Int2; Int2=SwapInt1;}
  static int GetRnd(const int& Range=0){return Rnd.GetUniDevInt(Range);}

  static bool IsOdd(const int& Int){return ((Int%2)==1);}
  static bool IsEven(const int& Int){return ((Int%2)==0);}

  static int GetMn(const int& Int1, const int& Int2){
    return Int1<Int2?Int1:Int2;}
  static int GetMx(const int& Int1, const int& Int2){
    return Int1>Int2?Int1:Int2;}
  static int GetMn(const int& Int1, const int& Int2, const int& Int3){
    return GetMn(Int1, GetMn(Int2, Int3));}
  static int GetMn(const int& Int1, const int& Int2,
   const int& Int3, const int& Int4){
    return GetMn(GetMn(Int1, Int2), GetMn(Int3, Int4));}
  static int GetMx(const int& Int1, const int& Int2, const int& Int3){
    return GetMx(Int1, GetMx(Int2, Int3));}
  static int GetMx(const int& Int1, const int& Int2,
   const int& Int3, const int& Int4){
    return GetMx(GetMx(Int1, Int2), GetMx(Int3, Int4));}
  static int GetInRng(const int& Val, const int& Mn, const int& Mx){
    IAssert(Mn<=Mx); return Val<Mn?Mn:(Val>Mx?Mx:Val);}

  TStr GetStr() const {return TInt::GetStr(Val);}
  static TStr GetStr(const int& Val){
    char Bf[255]; sprintf(Bf, "%d", Val); return TStr(Bf);}
  static TStr GetStr(const TInt& Int){
    return GetStr(Int.Val);}
  static TStr GetStr(const int& Val, const char* FmtStr);
  static TStr GetStr(const int& Val, const TStr& FmtStr){
    return GetStr(Val, FmtStr.CStr());}

  static TStr GetHexStr(const int& Val){
    char Bf[255]; sprintf(Bf, "%X", Val); return TStr(Bf);}
  static TStr GetHexStr(const TInt& Int){
    return GetHexStr(Int.Val);}

  static TStr GetKiloStr(const int& Val){
    if (Val>=100*1000){return GetStr(Val/1000)+"K";}
    else if (Val>=1000){return GetStr(Val/1000)+"."+GetStr((Val%1000)/100)+"K";}
    else {return GetStr(Val);}}
  static TStr GetMegaStr(const int& Val){
    if (Val>=100*1000000){return GetStr(Val/1000000)+"M";}
    else if (Val>=1000000){
      return GetStr(Val/1000000)+"."+GetStr((Val%1000000)/100000)+"M";}
    else {return GetKiloStr(Val);}}

  // frugal
  static char* SaveFrugalInt(char *pDest, int i);
  static char* LoadFrugalInt(char *pSrc, int& i);
  static void TestFrugalInt();
  static void SaveFrugalIntV(TSOut& SOut, const TVec<TInt>& IntV);
  static void LoadFrugalIntV(TSIn& SIn, TVec<TInt>& IntV, bool ClrP=true);
};

/////////////////////////////////////////////////
// Unsigned-Integer
class TUInt{
public:
  uint Val;
public:
  static const uint Mn;
  static const uint Mx;
  static TRnd Rnd;

  TUInt(): Val(0){}
  TUInt(const uint& _Val): Val(_Val){}
  operator uint() const {return Val;}
  explicit TUInt(TSIn& SIn){SIn.Load(Val);}
  void Load(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TUInt& operator=(const TUInt& UInt){Val=UInt.Val; return *this;}
  TUInt& operator=(const uint& _Val){Val=_Val; return *this;}
  TUInt operator++(int){Val++; return *this;}
  TUInt operator--(int){Val--; return *this;}
  //bool operator==(const TUInt& UInt) const {return Val==UInt.Val;}
  //bool operator==(const uint& UInt) const {return Val==UInt;}
  //bool operator!=(const uint& UInt) const {return Val!=UInt;}
  //bool operator<(const TUInt& UInt) const {return Val<UInt.Val;}
  uint operator()() const {return Val;}
  uint& operator()() {return Val;}
  TUInt& operator~(){Val=~Val; return *this;}
  TUInt& operator&=(const TUInt& UInt){Val&=UInt.Val; return *this;}
  TUInt& operator|=(const TUInt& UInt){Val|=UInt.Val; return *this;}
  TUInt& operator^=(const TUInt& UInt){Val^=UInt.Val; return *this;}
  TUInt& operator>>=(const int& ShiftBits){Val>>=ShiftBits; return *this;}
  TUInt& operator<<=(const int& ShiftBits){Val<<=ShiftBits; return *this;}
  int GetMemUsed() const {return sizeof(TUInt);}

  int GetPrimHashCd() const {return int(Val);}
  int GetSecHashCd() const {return Val/0x10;}

  static uint GetRnd(const uint& Range=0){return Rnd.GetUniDevUInt(Range);}

  TStr GetStr() const {return TUInt::GetStr(Val);}
  static TStr GetStr(const uint& Val){
    char Bf[255]; sprintf(Bf, "%u", Val); return TStr(Bf);}
  static TStr GetStr(const TInt& UInt){
    return GetStr(UInt.Val);}
  static TStr GetStr(const uint& Val, const char* FmtStr);
  static TStr GetStr(const uint& Val, const TStr& FmtStr){
    return GetStr(Val, FmtStr.CStr());}

  static TStr GetKiloStr(const uint& Val){
    if (Val>100*1000){return GetStr(Val/1000)+"K";}
    else if (Val>1000){return GetStr(Val/1000)+"."+GetStr((Val%1000)/100)+"K";}
    else {return GetStr(Val);}}
  static TStr GetMegaStr(const uint& Val){
    if (Val>100*1000000){return GetStr(Val/1000000)+"M";}
    else if (Val>1000000){
      return GetStr(Val/1000000)+"."+GetStr((Val%1000000)/100000)+"M";}
    else {return GetKiloStr(Val);}}

  static uint JavaUIntToCppUInt(const uint& JavaUInt){
    uint B1=(JavaUInt & 0xFF000000) >> 24;
    uint B2=(JavaUInt & 0x00FF0000) >> 16;
    uint B3=(JavaUInt & 0x0000FF00) >> 8;
    uint B4=(JavaUInt & 0x000000FF) >> 0;
    uint CppUInt=(B4<<24)+(B3<<16)+(B2<<8)+(B1<<0);
    return CppUInt;}

  static bool IsIpStr(const TStr& IpStr, uint& Ip, const char& SplitCh = '.');
  static bool IsIpStr(const TStr& IpStr, const char& SplitCh = '.') { uint Ip; return IsIpStr(IpStr, Ip, SplitCh); }
  static uint GetUIntFromIpStr(const TStr& IpStr, const char& SplitCh = '.');
  static TStr GetStrFromIpUInt(const uint& Ip);
};

/////////////////////////////////////////////////
// Unsigned-Integer-64Bit
class TUInt64{
public:
  uint64 Val;
public:
  static const TUInt64 Mn;
  static const TUInt64 Mx;

  TUInt64(): Val(0){}
  TUInt64(const TUInt64& Int): Val(Int.Val){}
  TUInt64(const uint64& Int): Val(Int){}
  TUInt64(const uint& MsVal, const uint& LsVal): Val(0){
    Val=(((uint64)MsVal) << 32) | ((uint64)LsVal);}
  explicit TUInt64(void* Pt): Val(0){
     TConv_Pt64Ints32 Conv(Pt); Val=Conv.GetUInt64();}
  operator uint64() const {return Val;}
  explicit TUInt64(TSIn& SIn){SIn.Load(Val);}
  void Load(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TUInt64& operator=(const TUInt64& Int){Val=Int.Val; return *this;}
  TUInt64& operator+=(const TUInt64& Int){Val+=Int.Val; return *this;}
  TUInt64& operator-=(const TUInt64& Int){Val-=Int.Val; return *this;}
  TUInt64 operator++(int){Val++; return *this;}
  TUInt64 operator--(int){Val--; return *this;}
  int GetMemUsed() const {return sizeof(TUInt64);}

  int GetPrimHashCd() const { return (int)GetMsVal() + (int)GetLsVal(); } //TODO: to check
  int GetSecHashCd() const { return ((int)GetMsVal() + (int)GetLsVal()) / 0x10; } //TODO: to check

  uint GetMsVal() const {
    return (uint)(Val >> 32);}
  uint GetLsVal() const {
    return (uint)(Val & 0xffffffff);}

  //TStr GetStr() const {return TStr::Fmt("%Lu", Val);}
  //static TStr GetStr(const TUInt64& Int){return TStr::Fmt("%Lu", Int.Val);}
  //static TStr GetHexStr(const TUInt64& Int){return TStr::Fmt("%LX", Int.Val);}
  #ifdef GLib_WIN
  TStr GetStr() const {return TStr::Fmt("%I64u", Val);}
  static TStr GetStr(const TUInt64& Int){return TStr::Fmt("%I64u", Int.Val);}
  static TStr GetHexStr(const TUInt64& Int){return TStr::Fmt("%I64X", Int.Val);}
  #else
  TStr GetStr() const {return TStr::Fmt("%llu", Val);}
  static TStr GetStr(const TUInt64& Int){return TStr::Fmt("%llu", Int.Val);}
  static TStr GetHexStr(const TUInt64& Int){return TStr::Fmt("%llX", Int.Val);}
  #endif

  static TStr GetKiloStr(const uint64& Val){
    if (Val>100*1000){return GetStr(Val/1000)+"K";}
    else if (Val>1000){return GetStr(Val/1000)+"."+GetStr((Val%1000)/100)+"K";}
    else {return GetStr(Val);}}
  static TStr GetMegaStr(const uint64& Val){
    if (Val>100*1000000){return GetStr(Val/1000000)+"M";}
    else if (Val>1000000){
      return GetStr(Val/1000000)+"."+GetStr((Val%1000000)/100000)+"M";}
    else {return GetKiloStr(Val);}}
  /*static TStr GetGigaStr(const uint64& Val){
    if (Val>100*1000000000){return GetStr(Val/1000000000)+"G";}
    else if (Val>1000000000){
      return GetStr(Val/1000000000)+"."+GetStr((Val%1000000000)/100000000)+"G";}
    else {return GetMegaStr(Val);}}*/
};

/////////////////////////////////////////////////
// Float
class TFlt{
public:
  double Val;
public:
  static const double Mn;
  static const double Mx;
  static const double NInf;
  static const double PInf;
  static const double Eps;
  static const double EpsHalf;
  static TRnd Rnd;

  TFlt(): Val(0){}
  TFlt(const double& _Val): Val(_Val){}
  operator double() const {return Val;}
  explicit TFlt(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  explicit TFlt(TSIn& SIn, const bool& IsTxt){
    if (IsTxt){TStr Str(SIn, true); Val=Str.GetFlt(0);} else {SIn.Load(Val);}}
  void Load(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut, const bool& IsTxt) const {
    if (IsTxt){GetStr(Val).Save(SOut, true);} else {SOut.Save(Val);}}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TFlt& operator=(const TFlt& Flt){Val=Flt.Val; return *this;}
  TFlt& operator=(const double& Flt){Val=Flt; return *this;}
  bool operator==(const TFlt& Flt) const _CMPWARN {return Val==Flt.Val;}
  bool operator==(const double& Flt) const _CMPWARN {return Val==Flt;}
  bool operator!=(const double& Flt) const _CMPWARN {return Val!=Flt;}
  double operator()() const {return Val;}
  TFlt& operator+=(const double& Flt){Val+=Flt; return *this;}
  TFlt& operator-=(const double& Flt){Val-=Flt; return *this;}
  TFlt& operator*=(const double& Flt){Val*=Flt; return *this;}
  TFlt& operator/=(const double& Flt){Val/=Flt; return *this;}
  TFlt operator++(int){Val++; return *this;}
  TFlt operator--(int){Val--; return *this;}
  int GetMemUsed() const {return sizeof(TFlt);}

  int GetPrimHashCd() const {
    int Expn; return int((frexp(Val, &Expn)-0.5)*double(TInt::Mx));}
  int GetSecHashCd() const {
    int Expn; frexp(Val, &Expn); return Expn;}

  static double Abs(const double& Flt){return Flt<0?-Flt:Flt;}
  static int Sign(const double& Flt){return Flt<0?-1:(Flt>0?1:0);}
  static int Round(const double& Flt){return int(floor(Flt+0.5));}
  static double GetRnd(){return Rnd.GetUniDev();}
  static bool Eq6(const double& LFlt, const double& RFlt){
    return fabs(LFlt-RFlt)<0.000001;}

  static double GetMn(const double& Flt1, const double& Flt2){
    return Flt1<Flt2?Flt1:Flt2;}
  static double GetMn(const double& Flt1, const double& Flt2, const double& Flt3){
    return GetMn(GetMn(Flt1, Flt2), Flt3); }
  static double GetMn(const double& Flt1, const double& Flt2, const double& Flt3, const double& Flt4){
    return GetMn(GetMn(Flt1, Flt2), GetMn(Flt3, Flt4)); }

  static double GetMx(const double& Flt1, const double& Flt2){
    return Flt1>Flt2?Flt1:Flt2;}
  static double GetMx(const double& Flt1, const double& Flt2, const double Flt3){
    return GetMx(GetMx(Flt1, Flt2), Flt3); }
  static double GetMx(const double& Flt1, const double& Flt2, const double Flt3, const double& Flt4){
    return GetMx(GetMx(Flt1, Flt2), GetMx(Flt3, Flt4)); }

  static double GetInRng(const double& Val, const double& Mn, const double& Mx){
    IAssert(Mn<=Mx); return Val<Mn?Mn:(Val>Mx?Mx:Val);}
  static double IsNum(const double& Val){
    return (Mn<=Val)&&(Val<=Mx);}

  TStr GetStr() const {return TFlt::GetStr(Val);}
  static TStr GetStr(const double& Val, const int& Width=-1, const int& Prec=-1);
  static TStr GetStr(const TFlt& Flt, const int& Width=-1, const int& Prec=-1){
    return GetStr(Flt.Val, Width, Prec);}
  static TStr GetStr(const double& Val, const char* FmtStr);
  static TStr GetStr(const double& Val, const TStr& FmtStr){
    return GetStr(Val, FmtStr.CStr());}
  static TStr GetPrcStr(const double& RelVal, const double& FullVal){
    return GetStr(100*RelVal/FullVal, "%3.0f%%");}

  static TStr GetKiloStr(const double& Val){
    if (fabs(Val)>100*1000){return TStr::Fmt("%.0fK", Val/1000);}
    else if (fabs(Val)>1000){return TStr::Fmt("%.1fK", Val/1000);}
    else {return TStr::Fmt("%.0f", Val);}}
  static TStr GetMegaStr(const double& Val){
    if (fabs(Val)>100*1000000){return TStr::Fmt("%.0fM", Val/1000000);}
    else if (fabs(Val)>1000000){return TStr::Fmt("%.1fM", Val/1000000);}
    else {return GetKiloStr(Val);}}
  static TStr GetGigaStr(const double& Val){
    if (fabs(Val)>100*1000000000.0){return TStr::Fmt("%.0fG", Val/1000000000.0);}
    else if (fabs(Val)>1000000000.0){return TStr::Fmt("%.1fG", Val/1000000000.0);}
    else {return GetMegaStr(Val);}}
};

/////////////////////////////////////////////////
// Ascii-Float
class TAscFlt: public TFlt{
public:
  TAscFlt(): TFlt(){}
  TAscFlt(const double& Val): TFlt(Val){}
  explicit TAscFlt(TSIn& SIn): TFlt(SIn, true){}
  void Save(TSOut& SOut) const {TFlt::Save(SOut, true);}
};

/////////////////////////////////////////////////
// Short-Float
class TSFlt{
public:
  sdouble Val;
public:
  static const sdouble Mn;
  static const sdouble Mx;

  TSFlt(): Val(0){}
  TSFlt(const sdouble& _Val): Val(sdouble(_Val)){}
  //TSFlt(const double& _Val): Val(sdouble(_Val)){}
  operator sdouble() const {return Val;}
  //operator double() const {return Val;}
  explicit TSFlt(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TSFlt& operator=(const TSFlt& SFlt){Val=SFlt.Val; return *this;}
  bool operator==(const TSFlt& SFlt) const _CMPWARN {return Val==SFlt.Val;}
  bool operator==(const double& Flt) const _CMPWARN {return Val==Flt;}
  bool operator!=(const double& Flt) const _CMPWARN {return Val!=Flt;}
  bool operator<(const TSFlt& SFlt) const {return Val<SFlt.Val;}
  sdouble operator()() const {return Val;}
  TSFlt& operator+=(const double& SFlt){Val+=sdouble(SFlt); return *this;}
  TSFlt& operator-=(const double& SFlt){Val-=sdouble(SFlt); return *this;}
  TSFlt& operator*=(const double& SFlt){Val*=sdouble(SFlt); return *this;}
  TSFlt& operator/=(const double& SFlt){Val/=sdouble(SFlt); return *this;}
  TSFlt operator++(int){Val++; return *this;}
  TSFlt operator--(int){Val--; return *this;}
  int GetMemUsed() const {return sizeof(TSFlt);}

  int GetPrimHashCd() const {
    int Expn; return int((frexp(Val, &Expn)-0.5)*double(TInt::Mx));}
  int GetSecHashCd() const {
    int Expn; frexp(Val, &Expn); return Expn;}
};

/////////////////////////////////////////////////
// Long-Float
class TLFlt{
public:
  ldouble Val;
public:
  static const ldouble Mn;
  static const ldouble Mx;

  TLFlt(): Val(0){}
  TLFlt(const ldouble& _Val): Val(_Val){}
  operator ldouble() const {return Val;}
  explicit TLFlt(TSIn& SIn){SIn.Load(Val);}
  void Save(TSOut& SOut) const {SOut.Save(Val);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TLFlt& operator=(const TLFlt& LFlt){Val=LFlt.Val; return *this;}
  bool operator==(const TLFlt& LFlt) const _CMPWARN {return Val==LFlt.Val;}
  bool operator==(const ldouble& LFlt) const _CMPWARN {return Val==LFlt;}
  bool operator!=(const ldouble& LFlt) const _CMPWARN {return Val!=LFlt;}
  bool operator<(const TLFlt& LFlt) const {return Val<LFlt.Val;}
  ldouble operator()() const {return Val;}
  TLFlt& operator+=(const ldouble& LFlt){Val+=LFlt; return *this;}
  TLFlt& operator-=(const ldouble& LFlt){Val-=LFlt; return *this;}
  int GetMemUsed() const {return sizeof(TLFlt);}

  int GetPrimHashCd() const {Fail; return 0;}
  int GetSecHashCd() const {Fail; return 0;}

  static TStr GetStr(const ldouble& Val, const int& Width=-1, const int& Prec=-1);
  static TStr GetStr(const TLFlt& LFlt, const int& Width=-1, const int& Prec=-1){
    return GetStr(LFlt.Val, Width, Prec);}
  static TStr GetStr(const ldouble& Val, const char* FmtStr);
  static TStr GetStr(const ldouble& Val, const TStr& FmtStr){
    return GetStr(Val, FmtStr.CStr());}
};

/////////////////////////////////////////////////
// Float-Rectangle
class TFltRect{
public:
  TFlt MnX, MnY, MxX, MxY;
public:
  TFltRect():
    MnX(), MnY(), MxX(), MxY(){}
  TFltRect(const TFltRect& FltRect):
    MnX(FltRect.MnX), MnY(FltRect.MnY), MxX(FltRect.MxX), MxY(FltRect.MxY){}
  TFltRect(
   const double& _MnX, const double& _MnY,
   const double& _MxX, const double& _MxY):
    MnX(_MnX), MnY(_MnY), MxX(_MxX), MxY(_MxY){}
  TFltRect(TSIn& SIn):
    MnX(SIn), MnY(SIn), MxX(SIn), MxY(SIn){}
  void Save(TSOut& SOut) const {
    MnX.Save(SOut); MnY.Save(SOut); MxX.Save(SOut); MxY.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TFltRect& operator=(const TFltRect& FltRect){
    MnX=FltRect.MnX; MnY=FltRect.MnY; MxX=FltRect.MxX; MxY=FltRect.MxY;
    return *this;}

  // get coordinates
  double GetMnX() const {return MnX;}
  double GetMnY() const {return MnY;}
  double GetMxX() const {return MxX;}
  double GetMxY() const {return MxY;}

  // get lengths
  double GetXLen() const {return MxX-MnX;}
  double GetYLen() const {return MxY-MnY;}

  // get centers
  double GetXCenter() const {return MnX+(MxX-MnX)/2;}
  double GetYCenter() const {return MnY+(MxY-MnY)/2;}

  // tests
  bool IsXYIn(const double& X, const double& Y) const {
    return (MnX<=X)&&(X<=MxX)&&(MnY<=Y)&&(Y<=MxY);}
  static bool Intersection(const TFltRect& Rect1, const TFltRect& Rect2);

  // string
  TStr GetStr() const;
};

