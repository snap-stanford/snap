/////////////////////////////////////////////////
// One byte
const int TB1Def::B1Bits=8;
const int TB1Def::MxP2Exp=TB1Def::B1Bits-1;
const TB1Def::TB1 TB1Def::MxB1=0xFF;

TB1Def::TB1Def(){
  B1P2T=new TB1[B1Bits+1]; B1P2T[0]=1;
  for (int BitN=1; BitN<B1Bits; BitN++){B1P2T[BitN]=TB1(2*B1P2T[BitN-1]);}
  B1P2T[B1Bits]=0;

  B1BitsT=new int[MxB1+1];
  for (int B1N=0; B1N<MxB1+1; B1N++){
    TB1 B1=(TB1)B1N; B1BitsT[B1]=0;
    for (int BitN=0; BitN<B1Bits; BitN++){B1BitsT[B1N]+=B1%2; B1/=(TB1)2;}}
}

int TB1Def::GetB1Bits(const TB1& B1){
  return B1Def.B1BitsT[B1];
}

uint TB1Def::GetP2(const int& P2Exp){
  IAssert((0<=P2Exp)&&(P2Exp<=TB1Def::MxP2Exp));
  return B1Def.B1P2T[P2Exp];
}

int TB1Def::GetL2(const uchar& Val){
  int L2=0;
  while ((L2<TB1Def::MxP2Exp)&&(Val>=B1Def.B1P2T[L2])){L2++;}
  return L2-1;
}

bool TB1Def::GetBit(const int& BitN, const uchar& Val){
  IAssert((0<=BitN)&&(BitN<=TB1Def::MxP2Exp));
  return (Val & B1Def.B1P2T[BitN])!=0;
}

const TB1Def TB1Def::B1Def;

/////////////////////////////////////////////////
// Two bytes
const int TB2Def::B2Bits=16;
const int TB2Def::MxP2Exp=TB2Def::B2Bits-1;
const TB2Def::TB2 TB2Def::MxB2=0xFFFF;

TB2Def::TB2Def(){
  B2P2T=new TB2[B2Bits+1]; B2P2T[0]=1;
  for (int BitN=1; BitN<B2Bits; BitN++){B2P2T[BitN]=TB2(2*B2P2T[BitN-1]);}
  B2P2T[B2Bits]=0;
}

int TB2Def::GetB2Bits(const TB2& B2){
  return
   TB1Def::B1Def.B1BitsT[(B2>>(0*TB1Def::B1Def.B1Bits))&TB1Def::B1Def.MxB1]+
   TB1Def::B1Def.B1BitsT[(B2>>(1*TB1Def::B1Def.B1Bits))&TB1Def::B1Def.MxB1];
}

uint TB2Def::GetP2(const int& P2Exp){
  IAssert((0<=P2Exp)&&(P2Exp<=TB2Def::MxP2Exp));
  return B2Def.B2P2T[P2Exp];
}

int TB2Def::GetL2(const TB2& Val){
  int L2=0;
  while ((L2<TB2Def::MxP2Exp)&&(Val>=B2Def.B2P2T[L2])){L2++;}
  return L2-1;
}

const TB2Def TB2Def::B2Def;

/////////////////////////////////////////////////
// Four bytes
const int TB4Def::B4Bits=32;
const int TB4Def::MxP2Exp=TB4Def::B4Bits-1;
const TB4Def::TB4 TB4Def::MxB4=0xFFFFFFFF;

TB4Def::TB4Def(){
  B4P2T=new TB4[B4Bits+1]; B4P2T[0]=1;
  for (int BitN=1; BitN<B4Bits; BitN++){B4P2T[BitN]=TB4(2*B4P2T[BitN-1]);}
  B4P2T[B4Bits]=0;
}

int TB4Def::GetB4Bits(const TB4& B4){
  return
   TB1Def::B1Def.B1BitsT[(B4>>(0*TB1Def::B1Def.B1Bits))&TB1Def::B1Def.MxB1]+
   TB1Def::B1Def.B1BitsT[(B4>>(1*TB1Def::B1Def.B1Bits))&TB1Def::B1Def.MxB1]+
   TB1Def::B1Def.B1BitsT[(B4>>(2*TB1Def::B1Def.B1Bits))&TB1Def::B1Def.MxB1]+
   TB1Def::B1Def.B1BitsT[(B4>>(3*TB1Def::B1Def.B1Bits))&TB1Def::B1Def.MxB1];
}

uint TB4Def::GetP2(const int& P2Exp){
  IAssert((0<=P2Exp)&&(P2Exp<=TB4Def::MxP2Exp));
  return B4Def.B4P2T[P2Exp];
}

int TB4Def::GetL2(const uint& Val){
  int L2=0;
  while ((L2<TB4Def::MxP2Exp)&&(Val>=B4Def.B4P2T[L2])){L2++;}
  return L2-1;
}

const TB4Def TB4Def::B4Def;

/////////////////////////////////////////////////
// Flag-Set
const int TFSet::B4s=4;
const int TFSet::Bits=TFSet::B4s*TB4Def::B4Bits;

TFSet::TFSet(
 const int& FlagN1, const int& FlagN2, const int& FlagN3,
 const int& FlagN4, const int& FlagN5, const int& FlagN6,
 const int& FlagN7, const int& FlagN8, const int& FlagN9):
  B4V(4, 4){
  if (FlagN1!=-1){Incl(FlagN1);}
  if (FlagN2!=-1){Incl(FlagN2);}
  if (FlagN3!=-1){Incl(FlagN3);}
  if (FlagN4!=-1){Incl(FlagN4);}
  if (FlagN5!=-1){Incl(FlagN5);}
  if (FlagN6!=-1){Incl(FlagN6);}
  if (FlagN7!=-1){Incl(FlagN7);}
  if (FlagN8!=-1){Incl(FlagN8);}
  if (FlagN9!=-1){Incl(FlagN9);}
}

const TFSet TFSet::EmptyFSet;

/////////////////////////////////////////////////
// Bit8-Set
const int TB8Set::Bits=8;

TStr TB8Set::GetStr() const {
  TChA ChA;
  for (int BitN=0; BitN<Bits; BitN++){
    if (In(BitN)){ChA+='1';} else {ChA+='0';}}
  return ChA;
}

void TB8Set::Wr(){
  printf("[");
  for (int BitN=0; BitN<Bits; BitN++){printf("%d", GetBit(BitN));}
  printf("]\n");
}

/////////////////////////////////////////////////
// Bit32-Set
const int TB32Set::Bits=32;

TStr TB32Set::GetStr() const {
  TChA ChA;
  for (int BitN=0; BitN<Bits; BitN++){
    if (In(BitN)){ChA+='1';} else {ChA+='0';}}
  return ChA;
}

void TB32Set::Wr(){
  printf("[");
  for (int BitN=0; BitN<Bits; BitN++){printf("%d", GetBit(BitN));}
  printf("]\n");
}

/////////////////////////////////////////////////
// Bit-Set
TBSet::TBSet(const TBSet& BSet):
  B4s(0), Bits(0), LastB4Mask(0), B4T(NULL){
  Gen(BSet.Bits);
  for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]=BSet.B4T[B4N];}
}

TBSet& TBSet::operator=(const TBSet& BSet){
  if (this!=&BSet){
    if (Bits!=BSet.Bits){Gen(BSet.Bits);}
    for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]=BSet.B4T[B4N];}}
  return *this;
}

bool TBSet::operator==(const TBSet& BSet) const {
  if (Bits!=BSet.Bits){return false;}
  for (int B4N=0; B4N<B4s-1; B4N++){if (B4T[B4N]!=BSet.B4T[B4N]){return false;}}
  return (B4s>0)&&((B4T[B4s-1]&LastB4Mask)==(BSet.B4T[B4s-1]&BSet.LastB4Mask));
}

void TBSet::Gen(const int& _Bits){
  if (B4T!=NULL){delete[] B4T;}
  Bits=_Bits;
  B4T=new TB4Def::TB4[B4s=(Bits-1)/TB4Def::B4Def.B4Bits+1];
  LastB4Mask=TB4Def::B4Def.MxB4;
  for (int BitN=Bits; BitN<B4s*TB4Def::B4Def.B4Bits; BitN++){
    LastB4Mask&= ~TB4Def::B4Def.B4P2T[BitN%TB4Def::B4Def.B4Bits];}
  Clr();
}

void TBSet::Clr(){
  for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]=0;}}

void TBSet::Fill(){
  for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]=TB4Def::B4Def.MxB4;}}

int TBSet::Get1s(){
  int Ones=0; SetLastB4();
  for (int B4N=0; B4N<B4s; B4N++){Ones += TB4Def::B4Def.GetB4Bits(B4T[B4N]);}
  return Ones;
}

void TBSet::Wr(){
  for (int BitN=0; BitN<Bits; BitN++){printf("%d", GetBit(BitN));}
  //printf("\n");
}

