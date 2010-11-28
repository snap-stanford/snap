/////////////////////////////////////////////////
// MD5
ClassTP(TMd5, PMd5)//{
private:
  // first, some types:
  typedef TB4Def::TB4 uint4; // assumes 4 byte long
  typedef TB2Def::TB2 uint2; // assumes 2 byte long
  typedef TB1Def::TB1 uint1; // assumes 1 byte long

  // next, the private data:
  uint4 state[4];
  uint4 count[2]; // number of *bits*, mod 2^64
  uint1 buffer[64]; // input buffer
  uint1 Sig[16];
  bool DefP;

  // last, the private methods, mostly static:
  void Init(); // called by all constructors
  void Transform(uint1* buffer); // does the real update work.
  static void Encode(uint1* Dst, uint4* Src, uint4 Len);
  static void Decode(uint4* Dst, uint1* Src, uint4 Len);
  static void MemCpy(uint1* Dst, uint1* Src, uint4 Len){
    for (uint4 ChN=0; ChN<Len; ChN++){Dst[ChN]=Src[ChN];}}
  static void MemSet(uint1* Start, uint1 Val, uint4 Len){
    for (uint4 ChN=0; ChN<Len; ChN++){Start[ChN]=Val;}}

  // RotateLeft rotates x left n bits.
  static uint4 RotateLeft(uint4 x, uint4 n){return (x<<n)|(x>>(32-n));}
  // F, G, H and I are basic MD5 functions.
  static uint4 F(uint4 x, uint4 y, uint4 z){return (x&y)|(~x&z);}
  static uint4 G(uint4 x, uint4 y, uint4 z){return (x&z)|(y&~z);}
  static uint4 H(uint4 x, uint4 y, uint4 z){return x^y^z;}
  static uint4 I(uint4 x, uint4 y, uint4 z){return y^(x|~z);}
  // FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
  // Rotation is separate from addition to prevent recomputation.
  static void FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac){
    a+=F(b, c, d)+x+ac; a=RotateLeft(a, s)+b;}
  static void GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac){
    a+=G(b, c, d)+x+ac; a=RotateLeft(a, s)+b;}
  static void HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac){
    a+=H(b, c, d)+x+ac; a=RotateLeft(a, s)+b;}
  static void II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac){
    a+=I(b, c, d)+x+ac; a=RotateLeft(a, s)+b;}
  UndefCopyAssign(TMd5);
public:
  TMd5(){Init();}
  static PMd5 New(){return PMd5(new TMd5());}
  TMd5(const PSIn& SIn){Init(); Add(SIn); Def();}
  static PMd5 New(const PSIn& SIn){return PMd5(new TMd5(SIn));}
  TMd5(TSIn&){Fail;}
  static PMd5 Load(TSIn& SIn){return new TMd5(SIn);}
  void Save(TSOut&){Fail;}

  // adding data & defining digest
  void Add(uchar* InBf, const int& InBfL);
  void Add(const PSIn& SIn);
  void Def();

  // digest data retrieval
  void GetSigMem(TMem& Mem) const;
  TStr GetSigStr() const;

  // data package digest calculation
  static TStr GetMd5SigStr(const PSIn& SIn){
    PMd5 Md5=TMd5::New(SIn); return Md5->GetSigStr();}
  static TStr GetMd5SigStr(const TStr& Str){
    return GetMd5SigStr(TStrIn::New(Str));}
  static TStr GetMd5SigStr(const TMem& Mem){
    return GetMd5SigStr(TMemIn::New(Mem));}

  // testing correctnes
  static bool Check();

  friend class TMd5Sig;
};

/////////////////////////////////////////////////
// MD5-Signature
class TMd5Sig{
private:
  typedef TB1Def::TB1 uint1; // assumes 1 byte long
  uint1 CdT[16];
public:
  TMd5Sig(){memset(CdT, 0, 16);}
  TMd5Sig(const TMd5Sig& Md5Sig){memcpy(CdT, Md5Sig.CdT, 16);}
  TMd5Sig(const PSIn& SIn);
  TMd5Sig(const TStr& Str);
  TMd5Sig(const TChA& ChA);
  TMd5Sig(const char* CStr);
  TMd5Sig(const TMem& Mem);
  TMd5Sig(const TMd5& Md5){memcpy(CdT, Md5.Sig, 16);} 
  TMd5Sig(TSIn& SIn){SIn.LoadBf(CdT, 16);}
  void Save(TSOut& SOut) const {SOut.SaveBf(CdT, 16);}

  TMd5Sig& operator=(const TMd5Sig& Md5Sig){
    if (this!=&Md5Sig){memcpy(CdT, Md5Sig.CdT, 16);} return *this;}
  bool operator==(const TMd5Sig& Md5Sig) const {
    return memcmp(CdT, Md5Sig.CdT, 16)==0;}
  bool operator<(const TMd5Sig& Md5Sig) const { 
    return memcmp(CdT, Md5Sig.CdT, 16)==-1;}
  int operator[](const int& CdN) const {
    Assert((0<=CdN)&&(CdN<16)); return CdT[CdN];}

  // hash codes
  int GetPrimHashCd() const;
  int GetSecHashCd() const;

  // string representation
  TStr GetStr() const;
};
typedef TVec<TMd5Sig> TMd5SigV;

