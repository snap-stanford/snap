/////////////////////////////////////////////////
// MD5
void TMd5::Init(){
  DefP=false; // we just started!

  // Nothing counted, so count=0
  count[0]=0;
  count[1]=0;

  // Load magic initialization constants.
  state[0]=0x67452301;
  state[1]=0xefcdab89;
  state[2]=0x98badcfe;
  state[3]=0x10325476;
}

// MD5 basic transformation. Transforms state based on block.
void TMd5::Transform(uint1 block[64]){
  static const int S11=7;
  static const int S12=12;
  static const int S13=17;
  static const int S14=22;
  static const int S21=5;
  static const int S22=9;
  static const int S23=14;
  static const int S24=20;
  static const int S31=4;
  static const int S32=11;
  static const int S33=16;
  static const int S34=23;
  static const int S41=6;
  static const int S42=10;
  static const int S43=15;
  static const int S44=21;

  uint4 a=state[0];
  uint4 b=state[1];
  uint4 c=state[2];
  uint4 d=state[3];
  uint4 x[16];

  Decode(x, block, 64);

  IAssert(!DefP);  // not just a user error, since the method is private

  /* Round 1 */
  FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH(b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0]+=a;
  state[1]+=b;
  state[2]+=c;
  state[3]+=d;

  // Zeroize sensitive information.
  MemSet((uint1*)x, 0, sizeof(x));
}

// Encodes input (UINT4) into output (unsigned char). Assumes len is
// a multiple of 4.
void TMd5::Encode(uint1 *output, uint4 *input, uint4 len){
  for (uint4 i=0, j=0; j<len; i++, j+=4){
    output[j]=uint1(input[i] & 0xff);
    output[j+1]=uint1((input[i]>>8) & 0xff);
    output[j+2]=uint1((input[i]>>16) & 0xff);
    output[j+3]=uint1((input[i]>>24) & 0xff);
  }
}

// Decodes input (unsigned char) into output (UINT4).
// Assumes len is a multiple of 4.
void TMd5::Decode(uint4* output, uint1* input, uint4 len){
  for (uint4 i=0, j=0; j<len; i++, j+=4){
    output[i]=
     ((uint4)input[j]) | (((uint4)input[j+1]) << 8) |
     (((uint4)input[j+2]) << 16) | (((uint4)input[j+3]) << 24);
  }
}

void TMd5::Add(uchar* InBf, const int& InBfL){
  IAssert(!DefP);
  // compute number of bytes mod 64
  uint4 BfX=uint((count[0]>>3) & 0x3F);

  // update number of bits
  if ((count[0]+=((uint4)InBfL<<3))<((uint4)InBfL<<3)){
    count[1]++;}
  count[1]+=((uint4)InBfL>>29);

  uint4 BfSpace=64-BfX; // how much space is left in buffer

  // transform as many times as possible.
  uint4 InX;
  if (uint(InBfL)>=BfSpace) { // ie. we have enough to fill the buffer
    // fill the rest of the buffer and transform
    MemCpy(buffer+BfX, InBf, BfSpace);
    Transform(buffer);
    // now, transform each 64-byte piece of the InBf, bypassing the buffer
    for (InX=BfSpace; InX+63<uint(InBfL); InX+=64){Transform(InBf+InX);}
    BfX=0; // so we can buffer remaining
  } else {
    InX=0; // so we can buffer the whole InBf
  }

  // and here we do the buffering:
  MemCpy(buffer+BfX, InBf+InX, InBfL-InX);
}

void TMd5::Add(const PSIn& SIn){
  uchar Bf[1024];
  while (SIn->Len()>0){
    int BfL=1024;
    if (SIn->Len()<BfL){BfL=SIn->Len();}
    SIn->GetBf(Bf, BfL);
    Add(Bf, BfL);
  }
}

void TMd5::Def(){
  unsigned char bits[8];
  static uint1 PADDING[64]={
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  IAssert(!DefP);
  Encode(bits, count, 8); // save number of bits

  // Pad out to 56 mod 64.
  uint index=uint4((count[0] >> 3) & 0x3f);
  uint padLen=(index<56) ? (56-index) : (120-index);
  Add(PADDING, padLen);

  Add(bits, 8); // append length (before padding)
  Encode(Sig, state, 16); // store state in digest
  MemSet(buffer, 0, sizeof(*buffer)); // zeroize sensitive information
  DefP=true;
}

void TMd5::GetSigMem(TMem& Mem) const {
  IAssert(DefP);
  Mem.Gen(16);
  for (int CdN=0; CdN<16; CdN++){Mem+=Sig[CdN];}
}

TStr TMd5::GetSigStr() const {
  IAssert(DefP);
  TChA ChA(32);
  for (int CdN=0; CdN<16; CdN++){
    ChA+=TCh::GetHexCh(Sig[CdN]/16);
    ChA+=TCh::GetHexCh(Sig[CdN]%16);
  }
  return ChA;
}

bool TMd5::Check(){
  return
   (TMd5::GetMd5SigStr("")=="D41D8CD98F00B204E9800998ECF8427E")&&
   (TMd5::GetMd5SigStr("a")=="0CC175B9C0F1B6A831C399E269772661")&&
   (TMd5::GetMd5SigStr("abc")=="900150983CD24FB0D6963F7D28E17F72")&&
   (TMd5::GetMd5SigStr("message digest")=="F96B697D7CB7938D525A2F31AAF161D0")&&
   (TMd5::GetMd5SigStr("abcdefghijklmnopqrstuvwxyz")=="C3FCD3D76192E4007DFB496CCA67E13B")&&
   (TMd5::GetMd5SigStr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")==
    "D174AB98D277D9F5A5611C2C9F419D9F")&&
   (TMd5::GetMd5SigStr("12345678901234567890123456789012345678901234567890123456789012345678901234567890")==
    "57EDF4A22BE3C955AC49DA2E2107B67A");
}

/////////////////////////////////////////////////
// MD5-Signature
TMd5Sig::TMd5Sig(const PSIn& SIn){
  PMd5 Md5=TMd5::New(SIn);
  memcpy(CdT, Md5->Sig, 16);
}

TMd5Sig::TMd5Sig(const TStr& Str){
  PMd5 Md5=TMd5::New(TStrIn::New(Str));
  memcpy(CdT, Md5->Sig, 16);
}

TMd5Sig::TMd5Sig(const TChA& ChA) {
  TMd5 Md5;  Md5.Add((uchar *) ChA.CStr(), ChA.Len());  Md5.Def();
  memcpy(CdT, Md5.Sig, 16);
}

TMd5Sig::TMd5Sig(const char* CStr) {
  TMd5 Md5;  Md5.Add((uchar *) CStr, (int) strlen(CStr));  Md5.Def();
  memcpy(CdT, Md5.Sig, 16);
}

TMd5Sig::TMd5Sig(const TMem& Mem){
  PMd5 Md5=TMd5::New(TMemIn::New(Mem));
  memcpy(CdT, Md5->Sig, 16);
}

int TMd5Sig::GetPrimHashCd() const {
  int HashCd=0;
  memcpy(&HashCd, &CdT[0], 4);
  HashCd=abs(HashCd);
  return HashCd;
}

int TMd5Sig::GetSecHashCd() const {
  int HashCd=0;
  memcpy(&HashCd, &CdT[3], 4);
  HashCd=abs(HashCd);
  return HashCd;
}

TStr TMd5Sig::GetStr() const {
  TChA ChA(32);
  for (int CdN=0; CdN<16; CdN++){
    ChA+=TCh::GetHexCh(CdT[CdN]/16);
    ChA+=TCh::GetHexCh(CdT[CdN]%16);
  }
  return ChA;
}

