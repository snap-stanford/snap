/////////////////////////////////////////////////
// One byte
class TB1Def{
public:
  typedef uchar TB1;
  static const int B1Bits;
  static const int MxP2Exp;
  static const TB1 MxB1;
  TB1* B1P2T;
  int* B1BitsT;
public:
  TB1Def();
  ~TB1Def(){delete[] B1P2T; delete[] B1BitsT;}

  TB1Def& operator=(const TB1Def&){Fail; return *this;}

  static int GetB1Bits(const TB1& B1);
  static uint GetP2(const int& P2Exp);
  static int GetL2(const uchar& Val);
  static bool GetBit(const int& BitN, const uchar& Val);

  static const TB1Def B1Def;
};

/////////////////////////////////////////////////
// Two bytes
class TB2Def{
public:
  typedef unsigned short int TB2;
  static const int B2Bits;
  static const int MxP2Exp;
  static const TB2 MxB2;
  TB2* B2P2T;
public:
  TB2Def();
  ~TB2Def(){delete[] B2P2T;}

  TB2Def& operator=(const TB2Def&){Fail; return *this;}

  static int GetB2Bits(const TB2& B2);
  static uint GetP2(const int& P2Exp);
  static int GetL2(const TB2& Val);

  static const TB2Def B2Def;
};

/////////////////////////////////////////////////
// Four bytes
class TB4Def{
public:
  typedef uint TB4;
  static const int B4Bits;
  static const int MxP2Exp;
  static const TB4 MxB4;
  TB4* B4P2T;
public:
  TB4Def();
  ~TB4Def(){delete[] B4P2T;}

  TB4Def& operator=(const TB4Def&){Fail; return *this;}

  static int GetB4Bits(const TB4& B4);
  static uint GetP2(const int& P2Exp);
  static int GetL2(const uint& Val);

  static const TB4Def B4Def;
};

/////////////////////////////////////////////////
// Flag-Set
/*class TFSet{
private:
  static const int B4s;
  static const int Bits;
  TB4Def::TB4 B4T[4];
public:
  TFSet(){
    B4T[0]=0; B4T[1]=0; B4T[2]=0; B4T[3]=0;}
  TFSet(const TFSet& FSet){
    B4T[0]=FSet.B4T[0]; B4T[1]=FSet.B4T[1];
    B4T[2]=FSet.B4T[2]; B4T[3]=FSet.B4T[3];}
  TFSet(const int& FlagN){
    B4T[0]=0; B4T[1]=0; B4T[2]=0; B4T[3]=0;
    Assert((0<=FlagN)&&(FlagN<Bits));
    B4T[FlagN/TB4Def::B4Def.B4Bits]=
     TB4Def::B4Def.B4P2T[FlagN%TB4Def::B4Def.B4Bits];}

  TFSet& operator=(const TFSet& FSet){
    if (this!=&FSet){
      B4T[0]=FSet.B4T[0]; B4T[1]=FSet.B4T[1];
      B4T[2]=FSet.B4T[2]; B4T[3]=FSet.B4T[3];}
    return *this;}
  bool operator==(const TFSet& FSet) const {
    return
     (B4T[0]==FSet.B4T[0])&&(B4T[1]==FSet.B4T[1])&&
     (B4T[2]==FSet.B4T[2])&&(B4T[3]==FSet.B4T[3]);}
  TFSet& operator|(const int& FlagN){
    Assert((0<=FlagN)&&(FlagN<Bits));
    B4T[FlagN/TB4Def::B4Def.B4Bits]|=
     TB4Def::B4Def.B4P2T[FlagN%TB4Def::B4Def.B4Bits];
    return *this;}
  TFSet& operator|(const TFSet& FSet){
    B4T[0]|=FSet.B4T[0]; B4T[1]|=FSet.B4T[1];
    B4T[2]|=FSet.B4T[2]; B4T[3]|=FSet.B4T[3];
    return *this;}

  bool Empty() const {
    return (B4T[0]==0)&&(B4T[1]==0)&&(B4T[2]==0)&&(B4T[3]==0);}
  bool In(const int& FlagN) const {
    Assert((0<=FlagN)&&(FlagN<Bits));
    return (B4T[FlagN/TB4Def::B4Def.B4Bits] &
     TB4Def::B4Def.B4P2T[FlagN%TB4Def::B4Def.B4Bits])!=0;}
};*/

/////////////////////////////////////////////////
// Flag-Set
class TFSet{
private:
  static const int B4s;
  static const int Bits;
  TUIntV B4V;
public:
  TFSet(): B4V(4, 4){}
  TFSet(const TFSet& FSet): B4V(FSet.B4V){}
  TFSet(
   const int& FlagN1, const int& FlagN2=-1, const int& FlagN3=-1,
   const int& FlagN4=-1, const int& FlagN5=-1, const int& FlagN6=-1,
   const int& FlagN7=-1, const int& FlagN8=-1, const int& FlagN9=-1);
  TFSet(const TFSet& FSet1, const TFSet& FSet2):
    B4V(4, 4){Incl(FSet1); Incl(FSet2);}
  ~TFSet(){}
  TFSet(TSIn& SIn): B4V(SIn){}
  void Save(TSOut& SOut) const {B4V.Save(SOut);}

  TFSet& operator=(const TFSet& FSet){
    if (this!=&FSet){B4V=FSet.B4V;} return *this;}
  bool operator==(const TFSet& FSet) const {return B4V==FSet.B4V;}
  TFSet& operator|(const int& FlagN){Incl(FlagN); return *this;}
  TFSet& operator|(const TFSet& FSet){Incl(FSet); return *this;}

  void Clr(){
    B4V[0]=0; B4V[1]=0; B4V[2]=0; B4V[3]=0;}
  bool Empty() const {
    return
     (uint(B4V[0])==0)&&(uint(B4V[1])==0)&&
     (uint(B4V[2])==0)&&(uint(B4V[3])==0);}
  void Incl(const int& FlagN){
    Assert((0<=FlagN)&&(FlagN<Bits));
    B4V[FlagN/TB4Def::B4Def.B4Bits]|=
     TB4Def::B4Def.B4P2T[FlagN%TB4Def::B4Def.B4Bits];}
  void Incl(const TFSet& FSet){
    B4V[0]|=FSet.B4V[0]; B4V[1]|=FSet.B4V[1];
    B4V[2]|=FSet.B4V[2]; B4V[3]|=FSet.B4V[3];}
  bool In(const int& FlagN) const {
    Assert((0<=FlagN)&&(FlagN<Bits));
    return (B4V[FlagN/TB4Def::B4Def.B4Bits] &
     TB4Def::B4Def.B4P2T[FlagN%TB4Def::B4Def.B4Bits])!=0;}

  static const TFSet EmptyFSet;
};

/////////////////////////////////////////////////
// Bit8-Set
class TB8Set{
private:
  static const int Bits;
  TB1Def::TB1 B1;
public:
  TB8Set(): B1(0){}
  TB8Set(const TB8Set& B8Set): B1(B8Set.B1){}
  TB8Set(const uchar& _B1): B1(_B1){}
  TB8Set(TSIn& SIn){SIn.LoadBf(&B1, sizeof(TB1Def::TB1));}
  void Save(TSOut& SOut) const {SOut.SaveBf(&B1, sizeof(TB1Def::TB1));}

  TB8Set& operator=(const TB8Set& BSet){B1=BSet.B1; return *this;}
  TB8Set& operator=(const uchar& _B1){B1=_B1; return *this;}
  bool operator==(const TB8Set& BSet) const {return B1==BSet.B1;}
  bool operator<(const TB8Set& BSet) const {return B1<BSet.B1;}

  bool Empty() const {return B1==0;}
  TB8Set& Clr(){B1=0; return *this;}
  TB8Set& Fill(){B1=TB1Def::B1Def.MxB1; return *this;}
  bool IsPrefix(const TB8Set& BSet, const int& MnBitN) const {
    Assert((0<=MnBitN)&&(MnBitN<Bits));
    return (B1>>MnBitN)==(BSet.B1>>MnBitN);}
  uchar GetUCh() const {return B1;}

  void Incl(const int& BitN){
    Assert((0<=BitN)&&(BitN<Bits));
    B1|=TB1Def::B1Def.B1P2T[BitN];}
  void Excl(const int& BitN){
    Assert((0<=BitN)&&(BitN<Bits));
    B1&=TB1Def::TB1(~(TB1Def::B1Def.B1P2T[BitN]));}
  bool In(const int& BitN) const {
    Assert((0<=BitN)&&(BitN<Bits));
    return (B1 & TB1Def::B1Def.B1P2T[BitN])!=0;}
  void SetBit(const int& BitN, const bool& Bool){
    if (Bool) Incl(BitN); else Excl(BitN);}
  bool GetBit(const int& BitN) const {
    Assert((0<=BitN)&&(BitN<Bits));
    return (B1 & TB1Def::B1Def.B1P2T[BitN])!=0;}
  int GetBits() const {return Bits;}
  int Get1s() const {return TB1Def::B1Def.GetB1Bits(B1);}
  int Get0s() const {return Bits-Get1s();}
  TStr GetStr() const;
  void Wr();

  void PutInt(const int& MnBitN, const int& MxBitN, const int& Val){
    Assert((0<=MnBitN)&&(MnBitN<=MxBitN)&&(MxBitN<Bits));
    B1 &= TB1Def::TB1((~(TB1Def::B1Def.B1P2T[MxBitN-MnBitN+1]-1)) << MnBitN);
    B1 |= TB1Def::TB1((Val & (TB1Def::B1Def.B1P2T[MxBitN-MnBitN+1]-1)) << MnBitN);}
  int GetInt(const int& MnBitN, const int& MxBitN) const {
    Assert((0<=MnBitN)&&(MnBitN<=MxBitN)&&(MxBitN<Bits));
    return (B1>>MnBitN) & (TB1Def::B1Def.B1P2T[MxBitN-MnBitN+1]-1);}

  TB8Set& operator~(){B1=TB1Def::TB1(~B1); return *this;}
  TB8Set& operator&=(const TB8Set& BSet){B1&=BSet.B1; return *this;}
  TB8Set& operator|=(const TB8Set& BSet){B1|=BSet.B1; return *this;}
  TB8Set& operator|=(const int& BitN){Incl(BitN); return *this;}
  TB8Set& operator^=(const TB8Set& BSet){B1^=BSet.B1; return *this;}
  TB8Set& operator>>=(const int& ShiftBits){B1>>=ShiftBits; return *this;}
  TB8Set& operator<<=(const int& ShiftBits){B1<<=ShiftBits; return *this;}

  friend TB8Set operator~(const TB8Set& BSet){
    return ~TB8Set(BSet);}
  friend TB8Set operator&(const TB8Set& LBSet, const TB8Set& RBSet){
    return TB8Set(LBSet)&=RBSet;}
  friend TB8Set operator|(const TB8Set& LBSet, const TB8Set& RBSet){
    return TB8Set(LBSet)|=RBSet;}
  friend TB8Set operator^(const TB8Set& LBSet, const TB8Set& RBSet){
    return TB8Set(LBSet)^=RBSet;}
};
typedef TVec<TB8Set> TB8SetV;

/////////////////////////////////////////////////
// Bit32-Set
class TB32Set{
private:
  static const int Bits;
  TB4Def::TB4 B4;
public:
  TB32Set(): B4(0){}
  TB32Set(const TB32Set& B32Set): B4(B32Set.B4){}
  TB32Set(const uint& _B4): B4(_B4){}
  TB32Set(TSIn& SIn){SIn.LoadBf(&B4, sizeof(TB4Def::TB4));}
  void Save(TSOut& SOut) const {SOut.SaveBf(&B4, sizeof(TB4Def::TB4));}

  TB32Set& operator=(const TB32Set& BSet){B4=BSet.B4; return *this;}
  bool operator==(const TB32Set& BSet) const {return B4==BSet.B4;}
  bool operator<(const TB32Set& BSet) const {return B4<BSet.B4;}

  bool Empty() const {return B4==0;}
  TB32Set& Clr(){B4=0; return *this;}
  TB32Set& Fill(){B4=TB4Def::B4Def.MxB4; return *this;}
  bool IsPrefix(const TB32Set& BSet, const int& MnBitN) const {
    Assert((0<=MnBitN)&&(MnBitN<Bits));
    return (B4>>MnBitN)==(BSet.B4>>MnBitN);}
  uint GetUInt() const {return B4;}

  void Incl(const int& BitN){
    Assert((0<=BitN)&&(BitN<Bits));
    B4|=TB4Def::B4Def.B4P2T[BitN];}
  void Excl(const int& BitN){
    Assert((0<=BitN)&&(BitN<Bits));
    B4&=~TB4Def::B4Def.B4P2T[BitN];}
  bool In(const int& BitN) const {
    Assert((0<=BitN)&&(BitN<Bits));
    return (B4 & TB4Def::B4Def.B4P2T[BitN])!=0;}
  void SetBit(const int& BitN, const bool& Bool){
    if (Bool) Incl(BitN); else Excl(BitN);}
  bool GetBit(const int& BitN) const {
    Assert((0<=BitN)&&(BitN<Bits));
    return (B4 & TB4Def::B4Def.B4P2T[BitN])!=0;}
  void SwitchBit(const int& BitN){
    SetBit(BitN, !GetBit(BitN));}
  int GetBits() const {return Bits;}
  int Get1s() const {return TB4Def::B4Def.GetB4Bits(B4);}
  int Get0s() const {return Bits-Get1s();}
  TStr GetStr() const;
  void Wr();

  void PutInt(const int& MnBitN, const int& MxBitN, const int& Val){
    Assert((0<=MnBitN)&&(MnBitN<=MxBitN)&&(MxBitN<Bits));
    B4 &= (~(TB4Def::B4Def.B4P2T[MxBitN-MnBitN+1]-1)) << MnBitN;
    B4 |= (Val & (TB4Def::B4Def.B4P2T[MxBitN-MnBitN+1]-1)) << MnBitN;}
  int GetInt(const int& MnBitN, const int& MxBitN) const {
    Assert((0<=MnBitN)&&(MnBitN<=MxBitN)&&(MxBitN<Bits));
    return (B4>>MnBitN) & (TB4Def::B4Def.B4P2T[MxBitN-MnBitN+1]-1);}

  TB32Set& operator~(){B4=~B4; return *this;}
  TB32Set& operator&=(const TB32Set& BSet){B4&=BSet.B4; return *this;}
  TB32Set& operator|=(const TB32Set& BSet){B4|=BSet.B4; return *this;}
  //TB32Set& operator|=(const int& BitN){Incl(BitN); return *this;}
  TB32Set& operator^=(const TB32Set& BSet){B4^=BSet.B4; return *this;}
  TB32Set& operator>>=(const int& ShiftBits){B4>>=ShiftBits; return *this;}
  TB32Set& operator<<=(const int& ShiftBits){B4<<=ShiftBits; return *this;}

  friend TB32Set operator~(const TB32Set& BSet){
    return ~TB32Set(BSet);}
  friend TB32Set operator&(const TB32Set& LBSet, const TB32Set& RBSet){
    return TB32Set(LBSet)&=RBSet;}
  friend TB32Set operator|(const TB32Set& LBSet, const TB32Set& RBSet){
    return TB32Set(LBSet)|=RBSet;}
  friend TB32Set operator^(const TB32Set& LBSet, const TB32Set& RBSet){
    return TB32Set(LBSet)^=RBSet;}
};
typedef TVec<TB32Set> TB32SetV;

/////////////////////////////////////////////////
// Bit-Set
ClassTPV(TBSet, PBSet, TBSetV)//{
private:
  int B4s, Bits;
  TB4Def::TB4 LastB4Mask;
  TB4Def::TB4* B4T;
  void SetLastB4(){B4T[B4s-1]&=LastB4Mask;}
public:
  TBSet(): B4s(0), Bits(0), LastB4Mask(0), B4T(NULL){}
  TBSet(const TBSet& BSet);
  PBSet Clone() const {return PBSet(new TBSet(*this));}
  TBSet(const int& _Bits):
    B4s(0), Bits(0), LastB4Mask(0), B4T(NULL){Gen(_Bits);}
  static PBSet New(const int& Bits){return PBSet(new TBSet(Bits));}
  ~TBSet(){delete[] B4T;}
  TBSet(TSIn& SIn){
    SIn.Load(B4s); SIn.Load(Bits);
    SIn.LoadBf(&LastB4Mask, sizeof(TB4Def::TB4));
    B4T=(TB4Def::TB4*)SIn.LoadNewBf(B4s*sizeof(TB4Def::TB4));}
  static PBSet Load(TSIn& SIn){return new TBSet(SIn);}
  void Save(TSOut& SOut) const {
    SOut.Save(B4s); SOut.Save(Bits);
    SOut.SaveBf(&LastB4Mask, sizeof(TB4Def::TB4));
    SOut.SaveBf(B4T, B4s*sizeof(TB4Def::TB4));}

  TBSet& operator=(const TBSet& BSet);
  bool operator==(const TBSet& BSet) const;

  void Gen(const int& _Bits);
  void Clr();
  void Fill();

  void Incl(const int& BitN){
    Assert((0<=BitN)&&(BitN<Bits));
    B4T[BitN/TB4Def::B4Def.B4Bits]|=
     TB4Def::B4Def.B4P2T[BitN%TB4Def::B4Def.B4Bits];}
  void Excl(const int& BitN){
    Assert((0<=BitN)&&(BitN<Bits));
    B4T[BitN/TB4Def::B4Def.B4Bits]&=
     ~TB4Def::B4Def.B4P2T[BitN%TB4Def::B4Def.B4Bits];}
  bool In(const int& BitN) const {
    Assert((0<=BitN)&&(BitN<Bits));
    return (B4T[BitN/TB4Def::B4Def.B4Bits] &
     TB4Def::B4Def.B4P2T[BitN%TB4Def::B4Def.B4Bits])!=0;}
  void SetBit(const int& BitN, const bool& Bool){
    if (Bool){Incl(BitN);} else {Excl(BitN);}}
  bool GetBit(const int& BitN) const {
    Assert((0<=BitN)&&(BitN<Bits));
    return (B4T[BitN/TB4Def::B4Def.B4Bits] &
     TB4Def::B4Def.B4P2T[BitN%TB4Def::B4Def.B4Bits])!=0;}
  void SwitchBit(const int& BitN){
    SetBit(BitN, !GetBit(BitN));}
  int GetBits() const {return Bits;}
  int Get1s();
  int Get0s(){return Bits-Get1s();}
  uint64 GetUInt64() const {
    Assert(Bits>=64); uint64 Val; memcpy(&Val, B4T, 8); return Val;}
  void Wr();

  TBSet& operator~(){
    for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]=~B4T[B4N];} return *this;}
  TBSet& operator&=(const TBSet& BSet){
    Assert(B4s==BSet.B4s);
    for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]&=BSet.B4T[B4N];} return *this;}
  TBSet& operator|=(const TBSet& BSet){
    Assert(B4s==BSet.B4s);
    for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]|=BSet.B4T[B4N];} return *this;}
  TBSet& operator|=(const int& BitN){
    Incl(BitN); return *this;}
  TBSet& operator^=(const TBSet& BSet){
    Assert(B4s==BSet.B4s);
    for (int B4N=0; B4N<B4s; B4N++){B4T[B4N]^=BSet.B4T[B4N];} return *this;}

  friend TBSet operator~(const TBSet& BSet){
    return ~TBSet(BSet);}
  friend TBSet operator&(const TBSet& LBSet, const TBSet& RBSet){
    return TBSet(LBSet)&=RBSet;}
  friend TBSet operator|(const TBSet& LBSet, const TBSet& RBSet){
    return TBSet(LBSet)|=RBSet;}
  friend TBSet operator^(const TBSet& LBSet, const TBSet& RBSet){
    return TBSet(LBSet)^=RBSet;}

  friend TBSet operator&(const TBSet& LBSet, const int& BitN){
    return TBSet(LBSet)&=BitN;}
  friend TBSet operator|(const TBSet& LBSet, const int& BitN){
    return TBSet(LBSet)|=BitN;}
  friend TBSet operator^(const TBSet& LBSet, const int& BitN){
    return TBSet(LBSet)^=BitN;}
};
