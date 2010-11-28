#ifndef shash_h
#define shash_h

/////////////////////////////////////////////////
// Hash-List-File
//J: saves and loads hash tables into files and allows fast
// iteration over the saved hash table file

template<class TKey, class TDat, class THashFunc = TDefaultHashFunc<TKey> >
class TKeyDatFl {
private:
  TInt ElemCnt;
  TFIn FIn;
  TKey Key;
  TDat Dat;
public:
  TKeyDatFl(const TStr& FNm) : FIn(FNm) { ElemCnt.Load(FIn); }
  int Len() const { return ElemCnt; }
  bool Next() { if (FIn.Eof()) { return false; }
    Key.Load(FIn);  Dat.Load(FIn); return true; }
  const TKey& GetKey() const { return Key; }
  TKey& GetKey() { return Key; }
  const TDat& GetDat() const { return Dat; }
  TDat& GetDat() { return Dat; }

  static void Save(const TStr& OutFNm, const THash<TKey, TDat, THashFunc>& Hash) {
    TFOut FOut(OutFNm);  Load(FOut, Hash); }
  static void Save(TSOut& SOut, const THash<TKey, TDat, THashFunc>& Hash) {
    SOut.Save(Hash.Len());
    for (int k = Hash.FFirstKeyId(); Hash.FNextKeyId(k); ) {
      Hash.GetKey(k).Save(SOut);  Hash[k].Save(SOut); }
  }
  static void LoadHash(const TStr& InFNm, THash<TKey, TDat, THashFunc>& Hash, const int& LoadN=-1) {
    TFIn FIn(InFNm);  Load(FIn, Hash, LoadN); }
  static void LoadHash(TSIn& SIn, THash<TKey, TDat, THashFunc>& Hash, int LoadN=-1) {
    TInt ElemCnt(SIn);  const int Start=clock();
    if (ElemCnt < LoadN || LoadN == -1) { LoadN = ElemCnt; }
    printf("Loading %s: %d elements ... ", SIn.GetSNm().CStr(), LoadN);  Hash.Gen(LoadN);
    for (int i = 0; i < LoadN; i++) { Hash.AddDat(TKey(SIn)).Load(SIn); }
    printf(" [%ds]\n", int((clock()-Start)/CLOCKS_PER_SEC));
  }
  static void LoadKeyV(TSIn& SIn, TVec<TKey>& KeyV, int LoadN=-1) {
    TInt ElemCnt(SIn);  const int Start=clock();
    if (ElemCnt < LoadN || LoadN == -1) { LoadN = ElemCnt; }
    printf("Loading %s: %d elements ... ", SIn.GetSNm().CStr(), LoadN);  KeyV.Gen(LoadN, 0);
    for (int i = 0; i < LoadN; i++) { KeyV.Add(TKey(SIn));  TDat D(SIn); }
    printf(" [%ds]\n", int((clock()-Start)/CLOCKS_PER_SEC));
  }
  static void LoadDatV(TSIn& SIn, TVec<TDat>& DatV, int LoadN=-1) {
    TInt ElemCnt(SIn);  const int Start=clock();
    if (ElemCnt < LoadN || LoadN == -1) { LoadN = ElemCnt; }
    printf("Loading %s: %d elements ... ", SIn.GetSNm().CStr(), LoadN);  DatV.Gen(LoadN, 0);
    for (int i = 0; i < LoadN; i++) { TKey(SIn);  DatV.Add(TDat(SIn)); }
    printf(" [%ds]\n", int((clock()-Start)/CLOCKS_PER_SEC));
  }
};

/////////////////////////////////////////////////
// Sparse Table Group
template <class TVal, uint16 GroupSize> // GroupSize=48; == 32*x+16
class TSparseGroup {
private:
  unsigned char BitSet [(GroupSize-1)/8 + 1];   // fancy math is so we round up
  uint16 Buckets;                               // limits GroupSize to 64K
  TVal *Group;
private:
  static int CharBit(const int&  ValN) { return ValN >> 3; }
  static int ModBit(const int&  ValN) { return 1 << (ValN&7); }
  bool BMTest(const int&  ValN) const { return (BitSet[CharBit(ValN)] & ModBit(ValN)) != 0; }
  void BMSet(const int&  ValN) { BitSet[CharBit(ValN)] |= ModBit(ValN); }
  void BMClear(const int&  ValN) { BitSet[CharBit(ValN)] &= ~ModBit(ValN); }
  static int PosToOffset(const unsigned char *BitSet, int Pos);
public:
  TSparseGroup() : Buckets(0), Group(NULL) { memset(BitSet, 0, sizeof(BitSet)); }
  TSparseGroup(TSIn& SIn) : Buckets(0), Group(NULL) { Load(SIn); }
  TSparseGroup(const TSparseGroup& SG);
  ~TSparseGroup() { if (Group != NULL) delete [] Group; }
  void Load(TSIn& SIn);
  void Save(TSOut& SOut) const;

  TSparseGroup& operator = (const TSparseGroup& SG);
  bool operator == (const TSparseGroup& SG) const;
  bool operator < (const TSparseGroup& SG) const;

  int Len() const { return Buckets; }
  int MxLen() const { return GroupSize; }
  int Reserved() const { return GroupSize; }
  bool Empty() const { return Buckets == 0; }
  void Clr(const bool& DoDel = true);
  int GetGroupSize() const { return GroupSize; }
  uint GetDiskSz() const { return sizeof(BitSet) + sizeof(uint16) + Buckets*sizeof(TVal); }

  bool IsEmpty(const int& ValN) const { return ! BMTest(ValN); }
  const TVal& Offset(const int& Pos) const { return Group[Pos]; }
  TVal& Offset(const int& Pos) { return Group[Pos]; }
  int OffsetToPos(int Offset) const;
  int PosToOffset(int Pos) const { return PosToOffset(BitSet, Pos); }

  const TVal& DefVal() const { static TVal DefValue = TVal();  return DefValue; }
  const TVal& Get(const int& ValN) const {
    if (BMTest(ValN)) return Group[PosToOffset(BitSet, ValN)]; else return DefVal(); }
  const TVal& operator [] (const int ValN) const { return Get(ValN); }

  TVal& Set(const int& ValN, const TVal& Val);
  TVal& Set(const int& ValN) {
    if (! BMTest(ValN)) Set(ValN, DefVal());
    return Group[PosToOffset(BitSet, ValN)];
  }
  void Del(const int& ValN);
};

template <class TVal, uint16 GroupSize>
int TSparseGroup<TVal, GroupSize>::PosToOffset(const unsigned char *BitSet, int Pos) {
  static const int bits_in [256] = {      // # of bits set in one char
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
  };
  // [Note: condition pos > 8 is an optimization; convince yourself we
  // give exactly the same result as if we had pos >= 8 here instead.]
  int Offset = 0;
  for ( ; Pos > 8; Pos -= 8 )                        // bm[0..pos/8-1]
    Offset += bits_in[*BitSet++];                    // chars we want *all* bits in
  return Offset + bits_in[*BitSet & ((1 << Pos)-1)]; // the char that includes pos
}

template <class TVal, uint16 GroupSize>
TSparseGroup<TVal, GroupSize>::TSparseGroup(const TSparseGroup& SG) : Buckets(SG.Buckets), Group(NULL) {
  memcpy(BitSet, SG.BitSet, sizeof(BitSet));
  if (Buckets > 0) {
    Group = new TVal [Buckets];
    for (int b = 0; b < Buckets; b++) { Group[b] = SG.Group[b]; }
  }
}

template <class TVal, uint16 GroupSize>
void TSparseGroup<TVal, GroupSize>::Load(TSIn& SIn) {
  SIn.LoadBf(BitSet, sizeof(BitSet));
  SIn.Load(Buckets);
  if (Group != NULL) delete [] Group;
  Group = new TVal [Buckets];
  for (int b = 0; b < Buckets; b++) { Group[b] = TVal(SIn); }
}

template <class TVal, uint16 GroupSize>
void TSparseGroup<TVal, GroupSize>::Save(TSOut& SOut) const {
  SOut.SaveBf(BitSet, sizeof(BitSet));
  SOut.Save(Buckets);
  for (int b = 0; b < Buckets; b++) { Group[b].Save(SOut); }
}

template <class TVal, uint16 GroupSize>
TSparseGroup<TVal, GroupSize>& TSparseGroup<TVal, GroupSize>::operator = (const TSparseGroup& SG) {
  if (this != &SG) {
    if (SG.Buckets == 0 && Group != NULL) {
      delete [] Group;
      Group = 0;
    } else {
      if (Buckets != SG.Buckets) {
        if (Group != NULL) delete [] Group;
        Group = new TVal [SG.Buckets];
      }
      for (int b = 0; b < SG.Buckets; b++) { Group[b] = SG.Group[b]; }
    }
    Buckets = SG.Buckets;
    memcpy(BitSet, SG.BitSet, sizeof(BitSet));
  }
  return *this;
}

template <class TVal, uint16 GroupSize>
bool TSparseGroup<TVal, GroupSize>::operator == (const TSparseGroup& SG) const {
  if (Buckets == SG.Buckets && memcmp(BitSet, SG.BitSet, sizeof(BitSet)) == 0) {
    for (int b = 0; b < Buckets; b++) {
      if (Group[b] != SG.Group[b]) return false;
    }
    return true;
  }
  return false;
}

template <class TVal, uint16 GroupSize>
bool TSparseGroup<TVal, GroupSize>::operator < (const TSparseGroup& SG) const {
  if (Buckets < SG.Buckets) return true;
  if (memcmp(BitSet, SG.BitSet, sizeof(BitSet)) == -1) return true;
  for (int b = 0; b < Buckets; b++) {
    if (Group[b] < SG.Group[b]) return true;
  }
  return false;
}

template <class TVal, uint16 GroupSize>
int TSparseGroup<TVal, GroupSize>::OffsetToPos(int Offset) const {
  Assert(Offset < Buckets);
  for (int i = 0; i < sizeof(BitSet); i++) {
    for (int b = 0; b < 8; b++) {
      if (TB1Def::GetBit(b, BitSet[i])) {
        if (Offset == 0) return i*8 + b;
        Offset--;
      }
    }
  }
  Fail;
  return -1;
}

template <class TVal, uint16 GroupSize>
void TSparseGroup<TVal, GroupSize>::Clr(const bool& DoDel) {
  if (DoDel && Group != NULL) {
    delete [] Group;
    Group = 0;
  }
  memset(BitSet, 0, sizeof(BitSet));
  Buckets = 0;
}

template <class TVal, uint16 GroupSize>
TVal& TSparseGroup<TVal, GroupSize>::Set(const int& ValN, const TVal& Val) {
  const int Offset = PosToOffset(BitSet, ValN);
  if (! BMTest(ValN)) {
    const TVal *OldGroup = Group;
    Group = new TVal [Buckets+1];
    for (int b = 0; b < Offset; b++) Group[b] = OldGroup[b];
    for (int b = Offset+1; b <= Buckets; b++) Group[b] = OldGroup[b-1];
    if (OldGroup != NULL) delete [] OldGroup;
    Buckets++;
    BMSet(ValN);
  }
  Group[Offset] = Val;
  return Group[Offset];
}

template <class TVal, uint16 GroupSize>
void TSparseGroup<TVal, GroupSize>::Del(const int& ValN) {
  if (BMTest(ValN)) {
    const int Offset = PosToOffset(BitSet, ValN);
    if (--Buckets == 0) {
      delete [] Group;
      Group = 0;
    } else {
      const TVal *OldGroup = Group;
      Group = new TVal [Buckets];
      for (int b = 0; b < Offset; b++) Group[b] = OldGroup[b];
      for (int b = Offset+1; b <= Buckets; b++) Group[b-1] = OldGroup[b];
      if (OldGroup != NULL) delete [] OldGroup;
    }
    BMClear(ValN);
  }
}

/////////////////////////////////////////////////
// Sparse Table Iterator
template <class TVal, uint16 GroupSize>
class TSparseTableI {
private:
  typedef TSparseGroup<TVal, GroupSize> TValGroup;
  typedef typename TVec<TValGroup>::TIter TGroupVI;
  int CurOff; // Offset in the current group
  TGroupVI BegI, GroupI, EndI;
public:
  TSparseTableI() : CurOff(0), GroupI(NULL), EndI(NULL) { }
  TSparseTableI(const TGroupVI& BegIter, const TGroupVI& CurIter, const TGroupVI& EndIter,
    const int& Offset = 0) : CurOff(Offset), BegI(BegIter), GroupI(CurIter), EndI(EndIter) { }
  TSparseTableI(const TSparseTableI& STI) :
    CurOff(STI.CurOff), BegI(STI.BegI), GroupI(STI.GroupI), EndI(STI.EndI) { }

  TSparseTableI& operator = (const TSparseTableI& STI) {
    CurOff=STI.CurOff;  BegI=STI.BegI;  GroupI=STI.GroupI;  EndI=STI.EndI;  return *this; }
  bool operator == (const TSparseTableI& STI) const {
    return GroupI == STI.GroupI && CurOff == STI.CurOff; }
  bool operator < (const TSparseTableI& STI) const {
    return GroupI < STI.GroupI || (GroupI == STI.GroupI && CurOff < STI.CurOff); }
  TSparseTableI& operator++ (int) {
    if (CurOff+1 == GroupI->Len()) { CurOff = 0;
      if (GroupI < EndI) { GroupI++;
        while (GroupI < EndI && GroupI->Empty()) { GroupI++; } }
    } else { CurOff++; }
    return *this;
  }
  TSparseTableI& operator-- (int) {
    if (CurOff == 0) {
      while (GroupI >= BegI && GroupI->Empty()) { GroupI--; }
      if (GroupI >= BegI) CurOff = GroupI->Len()-1;
    } else { CurOff--; }
    return *this;
  }
  int GetValN() const { return int(GroupI-BegI)*GroupSize + GroupI->OffsetToPos(CurOff); }
  bool IsEnd() const { return GroupI==EndI; }
  TVal& operator*() const { return GroupI->Offset(CurOff); }
  TVal& operator()() const { return GroupI->Offset(CurOff); }
  TVal* operator->() const { return &(operator*()); }
};

/////////////////////////////////////////////////
// Sparse Table
template <class TVal, uint16 GroupSize = 48> // == 32*x+16
class TSparseTable {
public:
  typedef TSparseGroup<TVal, GroupSize> TSGroup;
  typedef TSparseTableI<TVal, GroupSize> TIter;
private:
  TInt MxVals, Vals;
  TVec<TSGroup> GroupV;
private:
  static int GetGroups(const int& Vals) { return Vals == 0 ? 0 : ((Vals-1) / GroupSize) + 1; }
  int PosInGroup(const int& ValN) const { return ValN % GroupSize; }
  int GroupNum(const int& ValN) const { return ValN / GroupSize; }
  const TSGroup& GetGrp1(const int& ValN) const { return GroupV[GroupNum(ValN)]; }
  TSGroup& GetGrp1(const int& ValN) { return GroupV[GroupNum(ValN)]; }
public:
  TSparseTable(const int& MaxVals = 0) : MxVals(MaxVals),
    Vals(0), GroupV(GetGroups(MaxVals), GetGroups(MaxVals)) { }
  TSparseTable(const TSparseTable& ST) : MxVals(ST.MxVals), Vals(ST.Vals), GroupV(ST.GroupV) { }
  TSparseTable(TSIn& SIn) : MxVals(SIn), Vals(SIn), GroupV(SIn) { }
  void Load(TSIn& SIn) { MxVals.Load(SIn);  Vals.Load(SIn);  GroupV.Load(SIn); }
  void Save(TSOut& SOut) const { MxVals.Save(SOut);  Vals.Save(SOut);  GroupV.Save(SOut); }

  TSparseTable& operator = (const TSparseTable& ST);
  bool operator == (const TSparseTable& ST) const;
  bool operator < (const TSparseTable& ST) const;
  ::TSize GetMemUsed() const { return 2*sizeof(TInt)+Vals*sizeof(TVal)+GroupV.GetMemUsed(); }

  TIter BegI() const {
    if (Len() > 0) { int B = 0;
      while (B < Groups() && GroupV[B].Empty()) { B++; }
      return TIter(GroupV.BegI(), GroupV.BegI()+B, GroupV.EndI()); }
    return TIter(GroupV.BegI(), GroupV.EndI(), GroupV.EndI());
  }
  TIter EndI() const { return TIter(GroupV.BegI(), GroupV.EndI(), GroupV.EndI()); }
  TIter GetI(const int& ValN) const { Assert(! IsEmpty(ValN));
    typedef typename TVec<TSGroup>::TIter TVIter;
    const TVIter GI = GroupV.GetI(GroupNum(ValN));
    return TIter(GroupV.BegI(), GI, GroupV.EndI(), GI->PosToOffset(PosInGroup(ValN)));
  }

  int Len() const { return Vals; }
  int Reserved() const { return MxVals; }
  int Groups() const { return GroupV.Len(); }
  bool Empty() const { return Vals == 0; }
  uint GetDiskSz() const {
    return sizeof(TInt)*4 + ((GroupSize+16)/8)*Groups() + sizeof(TVal)*Vals; }

  void Clr(const bool& DoDel = true);
  void Reserve(const int NewVals) { Resize(NewVals); }
  void Resize(const int& NewVals);
  void Swap(TSparseTable& ST);

  bool IsEmpty(const int& ValN) const { return GroupV[GroupNum(ValN)].IsEmpty(PosInGroup(ValN)); }
  const TVal& Get(const int& ValN) const { return GroupV[GroupNum(ValN)].Get(PosInGroup(ValN)); }
  TVal& Set(const int& ValN, const TVal& Val);
  TVal& Set(const int& ValN);
  void Del(const int& ValN);

  TSGroup& GetGroup(const int& GroupN) { return GroupV[GroupN]; }
  const TSGroup& GetGroup(const int& GroupN) const { return GroupV[GroupN]; }
};

template <class TVal, uint16 GroupSize>
TSparseTable<TVal, GroupSize>& TSparseTable<TVal, GroupSize>::operator = (const TSparseTable& ST) {
  if (this != &ST) {
    MxVals = ST.MxVals;
    Vals = ST.Vals;
    GroupV = ST.GroupV;
  }
  return *this;
}

template <class TVal, uint16 GroupSize>
bool TSparseTable<TVal, GroupSize>::operator == (const TSparseTable& ST) const {
  return Vals == ST.Vals && MxVals == ST.MxVals && GroupV == ST.GroupV;
}

template <class TVal, uint16 GroupSize>
bool TSparseTable<TVal, GroupSize>::operator < (const TSparseTable& ST) const {
  return Vals < ST.Vals || (Vals == ST.Vals && GroupV < ST.GroupV);
}

template <class TVal, uint16 GroupSize>
void TSparseTable<TVal, GroupSize>::Clr(const bool& DoDel) {
  if (! DoDel) {
    for (int g = 0; g < GroupV.Len(); g++) GroupV[g].Clr(false);
  } else {
    MxVals = 0;
    GroupV.Clr(true);
  }
  Vals = 0;
}

template <class TVal, uint16 GroupSize>
void TSparseTable<TVal, GroupSize>::Resize(const int& NewVals) {
  // only allow to grow
  if (NewVals > MxVals) {
    const int Groups = GetGroups(NewVals);
    GroupV.Reserve(Groups, Groups);
    MxVals = NewVals;
  }
}

template <class TVal, uint16 GroupSize>
void TSparseTable<TVal, GroupSize>::Swap(TSparseTable& ST) {
  ::Swap(MxVals, ST.MxVals);
  ::Swap(Vals, ST.Vals);
  GroupV.Swap(ST.GroupV);
}

template <class TVal, uint16 GroupSize>
TVal& TSparseTable<TVal, GroupSize>::Set(const int& ValN, const TVal& Val) {
  Assert(ValN < MxVals);
  TSGroup& Group = GetGrp1(ValN);
  const int OldVals = Group.Len();
  TVal& ValRef = Group.Set(PosInGroup(ValN), Val);
  Vals += Group.Len() - OldVals;
  return ValRef;
}

template <class TVal, uint16 GroupSize>
TVal& TSparseTable<TVal, GroupSize>::Set(const int& ValN) {
  Assert(ValN < MxVals);
  TSGroup& Group = GetGrp1(ValN);
  const int OldVals = Group.Len();
  TVal& ValRef = Group.Set(PosInGroup(ValN));
  Vals += Group.Len() - OldVals;
  return ValRef;
}

template <class TVal, uint16 GroupSize>
void TSparseTable<TVal, GroupSize>::Del(const int& ValN) {
  Assert(ValN < MxVals);
  TSGroup& Group = GetGrp1(ValN);
  const int OldVals = Group.Len();
  Group.Del(PosInGroup(ValN));
  Vals += Group.Len() - OldVals;
}

/////////////////////////////////////////////////
// Sparse Hash Key Dat
#pragma pack(push, 1) // pack class size
template <class TKey, class TDat>
class TSHashKeyDat {
public:
  TKey Key;
  TDat Dat;
public:
  TSHashKeyDat() : Key(), Dat() { }
  TSHashKeyDat(const TKey& _Key) : Key(_Key), Dat() { }
  TSHashKeyDat(const TKey& _Key, const TDat& _Dat) : Key(_Key), Dat(_Dat) { }
  TSHashKeyDat(const TSHashKeyDat& HashKeyDat) : Key(HashKeyDat.Key), Dat(HashKeyDat.Dat) { }
  explicit TSHashKeyDat(TSIn& SIn) : Key(SIn), Dat(SIn) { }
  void Save(TSOut& SOut) const { Key.Save(SOut);  Dat.Save(SOut); }
  TSHashKeyDat& operator = (const TSHashKeyDat& HashKeyDat) { if (this != &HashKeyDat) {
    Key = HashKeyDat.Key;  Dat = HashKeyDat.Dat; }  return *this; }
  bool operator == (const TSHashKeyDat& HashKeyDat) const { return Key == HashKeyDat.Key; }
  bool operator < (const TSHashKeyDat& HashKeyDat) const { return Key < HashKeyDat.Key; }
  int Hash() const { return Key.GetPrimHashCd(); }
};
#pragma pack(pop)

/////////////////////////////////////////////////
// Sparse Hash Table
template <class TKey, class TDat, uint16 GroupSize=48> // GroupSize= 32*x+16, for some x
class TSparseHash {
public:
  typedef TSHashKeyDat<TKey, TDat> THashKeyDat;
  typedef typename TSparseTable<THashKeyDat, GroupSize>::TIter TIter;
  typedef typename TSparseTable<THashKeyDat, GroupSize>::TSGroup TSGroup;
public:
  static const float MxOccupancy; // = 0.7; //was 0.8
  static const float MnOccupancy; // = 0.4 * MxOccupancy;
  static const int MinBuckets;    // = 32 (must be power of 2)
private:
  void ResetThresh();
  int GetMinSize(const int& CurVals, const int& WantedVals) const;
  void CopyFrom(const TSparseHash& HT, const int& MnWanted);
  void MoveFrom(TSparseHash& HT, const int& MnWanted);
  void ResizeDelta(const int& ValsToAdd, const int& MnWanted = 0);
  void FindPos(const TKey& Key, int& Pos, int& PosToIns) const;
private:
  TInt ShrinkThresh, ExpandThresh;
  TSparseTable<THashKeyDat, GroupSize> Table;
public:
  TSparseHash(const int& WantedVals = 0) : Table(GetMinSize(0, WantedVals)) { ResetThresh(); }
  TSparseHash(TSIn& SIn) : ShrinkThresh(SIn), ExpandThresh(SIn), Table(SIn) { }
  void Load(TSIn& SIn) { ShrinkThresh.Load(SIn);  ExpandThresh.Load(SIn);  Table.Load(SIn); }
  void Save(TSOut& SOut) const { ShrinkThresh.Save(SOut); ExpandThresh.Save(SOut); Table.Save(SOut); }

  TSparseHash& operator = (const TSparseHash& SHT);
  bool operator == (const TSparseHash& SHT) const;
  bool operator < (const TSparseHash& SHT) const;
  ::TSize GetMemUsed() const { return 2*sizeof(TInt)+Table.GetMemUsed(); }

  TIter BegI() const { return Table.BegI(); }
  TIter EndI() const { return Table.EndI(); }
  TIter GetI(const TKey& Key) const { Assert(IsKey(Key));  return Table.GetI(GetKeyId(Key)); }

  bool Empty() const { return Len() == 0; }
  int Len() const { return Table.Len(); }
  int Reserved() const  { return Table.Reserved(); }
  uint GetDiskSz() const { return 2*sizeof(TInt) + Table.GetDiskSz(); }

  void Reserve(const int& MxVals) { if (MxVals > Len()) ResizeDelta(MxVals - Len(), 0); }
  void Clr(const bool& DoDel = true) { Table.Clr(DoDel);  ResetThresh(); }
  void Swap(TSparseHash& HT);

  int AddKey(const TKey& Key);
  TDat& AddDat(const TKey& Key);
  TDat& AddDat(const TKey& Key, const TDat& Dat);

  const TKey& GetKey(const int& KeyId) const { return Table.Get(KeyId).Key; }
  int GetKeyId(const TKey& Key) const {
    int Pos, PosToIns;  FindPos(Key, Pos, PosToIns);  return Pos; }
  bool IsKey(const TKey& Key) const { return GetKeyId(Key) != -1; }
  bool IsKey(const TKey& Key, int& KeyId) const {
    KeyId = GetKeyId(Key);  return KeyId != -1; }
  bool IsKeyId(const int& KeyId) const { return ! Table.IsEmpty(KeyId); }
  int GetRndKeyId(TRnd& Rnd = TInt::Rnd) const { Assert(Len()>0);
    int KeyId = Rnd.GetUniDevInt(Reserved());
    while (! IsKeyId(KeyId)) { KeyId = Rnd.GetUniDevInt(Reserved()); } return KeyId; }

  const TDat& GetDat(const TKey& Key) const;
  TDat& GetDat(const TKey& Key);
  const TDat& GetDatKeyId(const int& KeyId) const { return Table.Get(KeyId).Dat; }
  TDat& GetDatKeyId(const int& KeyId) { return Table.Set(KeyId).Dat; }
  void GetKeyDat(const int& KeyId, TKey& Key, TDat& Dat) const;
  bool IsKeyGetDat(const TKey& Key, TDat& Dat) const;

  void GetKeyV(TVec<TKey>& KeyV) const;
  void GetDatV(TVec<TDat>& DatV) const;
  void GetKeyDatPrV(TVec<TPair<TKey, TDat> >& KeyDatPrV) const;
  void GetDatKeyPrV(TVec<TPair<TDat, TKey> >& DatKeyPrV) const;
};

template <class TKey, class TDat, uint16 GroupSize>
const float TSparseHash<TKey, TDat, GroupSize>::MxOccupancy = 0.8f; //0.8f;

template <class TKey, class TDat, uint16 GroupSize>
const float TSparseHash<TKey, TDat, GroupSize>::MnOccupancy = 0.4f * 0.8f; //0.8f

template <class TKey, class TDat, uint16 GroupSize>
const int TSparseHash<TKey, TDat, GroupSize>::MinBuckets = 32;

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::ResetThresh() {
  ExpandThresh = int(Table.Reserved() * MxOccupancy);
  ShrinkThresh = int(Table.Reserved() * MnOccupancy);
}

template <class TKey, class TDat, uint16 GroupSize>
int TSparseHash<TKey, TDat, GroupSize>::GetMinSize(const int& CurVals, const int& WantedVals) const {
  int Size = MinBuckets;
  while (Size*MxOccupancy < WantedVals || CurVals >= Size * MxOccupancy) Size *= 2;
  return Size;
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::CopyFrom(const TSparseHash& HT, const int& MnWanted) {
  Clr(false);
  const int NewSize = GetMinSize(HT.Reserved(), MnWanted);
  if (NewSize > Reserved()) {
    Table.Resize(NewSize);
    ResetThresh();
  }
  const uint BuckM1 = Reserved() - 1;
  for (int g = 0; g < HT.Table.Groups(); g++) {
    const TSGroup& Group = HT.Table.GetGroup(g);
    for (int b = 0; b < Group.Len(); b++) {
      int Tries = 0; uint BuckNum;
      for (BuckNum = Group.Offset(b).Hash() & BuckM1;
       ! Table.IsEmpty(BuckNum); BuckNum = (BuckNum + Tries) & BuckM1) {
        Tries++;
        Assert(Tries < Reserved());
      }
      Table.Set(BuckNum, Group.Offset(b));
    }
  }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::MoveFrom(TSparseHash& HT, const int& MnWanted) {
  Clr(false);
  int NewSize;
  if (MnWanted == 0) NewSize = HT.Reserved();
  else NewSize = GetMinSize(HT.Reserved(), MnWanted);
  if (NewSize > Reserved()) {
    Table.Resize(NewSize);
    ResetThresh();
  }
  const uint BuckM1 = Reserved() - 1;
  for (int g = 0; g < HT.Table.Groups(); g++) {
    TSGroup& Group = HT.Table.GetGroup(g);
    for (int b = 0; b < Group.Len(); b++) {
      int Tries = 0; uint BuckNum;
      for (BuckNum = Group.Offset(b).Hash() & BuckM1;
       ! Table.IsEmpty(BuckNum); BuckNum = (BuckNum + Tries) & BuckM1) {
        Tries++;
        Assert(Tries < Reserved());
      }
      Assert(Table.IsEmpty(BuckNum));
      Table.Set(BuckNum, Group.Offset(b));
    }
    Group.Clr(true); // delete
  }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::ResizeDelta(const int& ValsToAdd, const int& MnWanted) {
  if (Reserved() > MnWanted && Len()+ValsToAdd < ExpandThresh) { return; }
  const int NewSize = GetMinSize(Table.Len()+ValsToAdd, MnWanted);
  if (NewSize > Reserved()) {
    printf("***Resize SparseHash:%d->%d\n", Reserved(), NewSize);
    TSparseHash TmpHt(ValsToAdd+Len());
    TmpHt.ResetThresh();
    TmpHt.MoveFrom(*this, Len());
    Swap(TmpHt);
  }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::FindPos(const TKey& Key, int& Pos, int& PosToIns) const {
  const uint BuckM1 = Reserved() - 1;
  uint BuckNum = Key.GetPrimHashCd() & BuckM1;
  int Tries = 0;
  while (true) {
    if (Table.IsEmpty(BuckNum)) {
      Pos = -1;  PosToIns = BuckNum;  return;
    }
    else if (Key == Table.Get(BuckNum).Key) {
      Pos = BuckNum;  PosToIns = -1;  return;
    }
    Tries++;
    BuckNum = (BuckNum + Tries) & BuckM1;
    Assert(Tries < Reserved());
  }
}

template <class TKey, class TDat, uint16 GroupSize>
TSparseHash<TKey, TDat, GroupSize>& TSparseHash<TKey, TDat, GroupSize>::operator = (const TSparseHash& SHT) {
  if (this != &SHT) {
    ShrinkThresh = SHT.ShrinkThresh;
    ExpandThresh = SHT.ExpandThresh;
    Table = SHT.Table;
  }
  return *this;
}

template <class TKey, class TDat, uint16 GroupSize>
bool TSparseHash<TKey, TDat, GroupSize>::operator == (const TSparseHash& SHT) const {
  return Table == SHT.Table;
}

template <class TKey, class TDat, uint16 GroupSize>
bool TSparseHash<TKey, TDat, GroupSize>::operator < (const TSparseHash& SHT) const {
  return Table < SHT.Table;
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::Swap(TSparseHash& HT) {
  ::Swap(ShrinkThresh, HT.ShrinkThresh);
  ::Swap(ExpandThresh, HT.ExpandThresh);
  Table.Swap(HT.Table);
}

template <class TKey, class TDat, uint16 GroupSize>
int TSparseHash<TKey, TDat, GroupSize>::AddKey(const TKey& Key) {
  ResizeDelta(1);
  int Pos, PosToIns;  FindPos(Key, Pos, PosToIns);
  if (Pos != -1) { return Pos; } // key exists
  else {
    Table.Set(PosToIns, THashKeyDat(Key));
    return PosToIns;
  }
}

template <class TKey, class TDat, uint16 GroupSize>
TDat& TSparseHash<TKey, TDat, GroupSize>::AddDat(const TKey& Key) {
  ResizeDelta(1);
  int Pos, PosToIns;  FindPos(Key, Pos, PosToIns);
  if (PosToIns != -1) {
    return Table.Set(PosToIns, THashKeyDat(Key)).Dat;
  } else { return Table.Set(Pos).Dat; }
}

template <class TKey, class TDat, uint16 GroupSize>
TDat& TSparseHash<TKey, TDat, GroupSize>::AddDat(const TKey& Key, const TDat& Dat) {
  ResizeDelta(1);
  int Pos, PosToIns;  FindPos(Key, Pos, PosToIns);
  if (PosToIns != -1) {
    return Table.Set(PosToIns, THashKeyDat(Key, Dat)).Dat;
  } else { return Table.Set(Pos).Dat = Dat; }
}

template <class TKey, class TDat, uint16 GroupSize>
const TDat& TSparseHash<TKey, TDat, GroupSize>::GetDat(const TKey& Key) const {
  int Pos, PosToIns;
  FindPos(Key, Pos, PosToIns);
  Assert(Pos != -1);
  return Table.Get(Pos).Dat;
}

template <class TKey, class TDat, uint16 GroupSize>
TDat& TSparseHash<TKey, TDat, GroupSize>::GetDat(const TKey& Key) {
  int Pos, PosToIns;
  FindPos(Key, Pos, PosToIns);
  Assert(Pos != -1);
  return Table.Set(Pos).Dat;
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::GetKeyDat(const int& KeyId, TKey& Key, TDat& Dat) const {
  Assert(IsKey(KeyId));
  const THashKeyDat& KeyDat = Table.Get(KeyId);
  Key = KeyDat.Key;
  Dat = KeyDat.Dat;
}

template <class TKey, class TDat, uint16 GroupSize>
bool TSparseHash<TKey, TDat, GroupSize>::IsKeyGetDat(const TKey& Key, TDat& Dat) const {
  int KeyId;
  if (IsKey(Key, KeyId)) {
    Dat=Table.Get(KeyId).Dat;
    return true;
  } else { return false; }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::GetKeyV(TVec<TKey>& KeyV) const {
  KeyV.Gen(Len(), 0);
  for (TIter i = BegI(); i < EndI(); i++) {
    KeyV.Add(i->Key);
  }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::GetDatV(TVec<TDat>& DatV) const {
  DatV.Gen(Len(), 0);
  for (TIter i = BegI(); i < EndI(); i++) {
    DatV.Add(i->Dat);
  }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::GetKeyDatPrV(TVec<TPair<TKey, TDat> >& KeyDatPrV) const {
  KeyDatPrV.Gen(Len(), 0);
  for (TIter i = BegI(); i < EndI(); i++) {
    KeyDatPrV.Add(TPair<TKey, TDat>(i->Key, i->Dat));
  }
}

template <class TKey, class TDat, uint16 GroupSize>
void TSparseHash<TKey, TDat, GroupSize>::GetDatKeyPrV(TVec<TPair<TDat, TKey> >& DatKeyPrV) const {
  DatKeyPrV.Gen(Len(), 0);
  for (TIter i = BegI(); i < EndI(); i++) {
    DatKeyPrV.Add(TPair<TDat, TKey>(i->Dat, i->Key));
  }
}

/////////////////////////////////////////////////
// Sparse Set
template <class TKey, uint16 GroupSize=48> // GroupSize= 32*x+16, for some x
class TSparseSet {
public:
  typedef typename TSparseTable<TKey, GroupSize>::TIter TIter;
  typedef typename TSparseTable<TKey, GroupSize>::TSGroup TSGroup;
public:
  static const float MxOccupancy; // = 0.7; //was 0.8
  static const float MnOccupancy; // = 0.4 * MxOccupancy;
  static const int MinBuckets;    // = 32 (must be power of 2)
private:
  void ResetThresh();
  int GetMinSize(const int& CurVals, const int& WantedVals) const;
  void CopyFrom(const TSparseSet& SSet, const int& MnWanted);
  void MoveFrom(TSparseSet& SSet, const int& MnWanted);
  void ResizeDelta(const int& ValsToAdd, const int& MnWanted = 0);
  void FindPos(const TKey& Key, int& Pos, int& PosToIns) const;
private:
  TInt ShrinkThresh, ExpandThresh;
  TSparseTable<TKey, GroupSize> Table;
public:
  TSparseSet(const int& WantedVals = 0) : Table(GetMinSize(0, WantedVals)) { ResetThresh(); }
  TSparseSet(TSIn& SIn) : ShrinkThresh(SIn), ExpandThresh(SIn), Table(SIn) { }
  void Load(TSIn& SIn) { ShrinkThresh.Load(SIn);  ExpandThresh.Load(SIn);  Table.Load(SIn); }
  void Save(TSOut& SOut) const { ShrinkThresh.Save(SOut); ExpandThresh.Save(SOut); Table.Save(SOut); }

  TSparseSet& operator = (const TSparseSet& SSet);
  bool operator == (const TSparseSet& SSet) const;
  bool operator < (const TSparseSet& SSet) const;
  ::TSize GetMemUsed() const { return 2*sizeof(TInt)+Table.GetMemUsed(); }

  TIter BegI() const { return Table.BegI(); }
  TIter EndI() const { return Table.EndI(); }
  TIter GetI(const int& KeyId) const { Assert(IsKeyId(KeyId));  return Table.GetI(KeyId); }

  bool Empty() const { return Len() == 0; }
  int Len() const { return Table.Len(); }
  int Reserved() const  { return Table.Reserved(); }
  uint GetDiskSz() const { return 2*sizeof(TInt) + Table.GetDiskSz(); }

  void Reserve(const int& MxVals) { if (MxVals > Len()) ResizeDelta(MxVals - Len(), 0); }
  void Clr(const bool& DoDel = true) { Table.Clr(DoDel);  ResetThresh(); }
  void Swap(TSparseSet& SSet);

  int AddKey(const TKey& Key);
  const TKey& GetKey(const int& KeyId) const { return Table.Get(KeyId); }
  int GetKeyId(const TKey& Key) const { int Pos, PosToIns;
    FindPos(Key, Pos, PosToIns);  return Pos; }
  bool IsKey(const TKey& Key) const { return GetKeyId(Key) != -1; }
  bool IsKey(const TKey& Key, int& KeyId) const {
    KeyId = GetKeyId(Key);  return KeyId != -1; }
  bool IsKeyId(const int& KeyId) const { return ! Table.IsEmpty(KeyId); }
  int GetRndKeyId(TRnd& Rnd = TInt::Rnd) const { Assert(Len()>0);
    int KeyId = Rnd.GetUniDevInt(Reserved());
    while (! IsKeyId(KeyId)) { KeyId = Rnd.GetUniDevInt(Reserved()); } return KeyId; }

  void GetKeyV(TVec<TKey>& KeyV) const;
};

template <class TKey, uint16 GroupSize>
const float TSparseSet<TKey, GroupSize>::MxOccupancy = 0.8f;

template <class TKey, uint16 GroupSize>
const float TSparseSet<TKey, GroupSize>::MnOccupancy = 0.4f * 0.8f;

template <class TKey, uint16 GroupSize>
const int TSparseSet<TKey, GroupSize>::MinBuckets = 32;

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::ResetThresh() {
  ExpandThresh = int(Table.Reserved() * MxOccupancy);
  ShrinkThresh = int(Table.Reserved() * MnOccupancy);
}

template <class TKey, uint16 GroupSize>
int TSparseSet<TKey, GroupSize>::GetMinSize(const int& CurVals, const int& WantedVals) const {
  int Size = MinBuckets;
  while (Size*MxOccupancy <= WantedVals || CurVals > Size * MxOccupancy) Size *= 2;
  return Size;
}

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::CopyFrom(const TSparseSet& SSet, const int& MnWanted) {
  Clr(false);
  const int NewSize = GetMinSize(SSet.Reserved(), MnWanted);
  if (NewSize > Reserved()) {
    Table.Resize(NewSize);
    ResetThresh();
  }
  const uint BuckM1 = Reserved() - 1;
  for (int g = 0; g < SSet.Table.Groups(); g++) {
    const TSGroup& Group = SSet.Table.GetGroup(g);
    for (int b = 0; b < Group.Len(); b++) {
      int Tries = 0; uint BuckNum;
      for (BuckNum = Group.Offset(b).GetPrimHashCd() & BuckM1;
       ! Table.IsEmpty(BuckNum); BuckNum = (BuckNum + Tries) & BuckM1) {
        Tries++;
        Assert(Tries < Reserved());
      }
      Table.Set(BuckNum, Group.Offset(b));
    }
  }
}

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::MoveFrom(TSparseSet& SSet, const int& MnWanted) {
  Clr(false);
  int NewSize;
  if (MnWanted == 0) NewSize = SSet.Reserved();
  else NewSize = GetMinSize(SSet.Reserved(), MnWanted);
  if (NewSize > Reserved()) {
    Table.Resize(NewSize);
    ResetThresh();
  }
  const uint BuckM1 = Reserved() - 1;
  for (int g = 0; g < SSet.Table.Groups(); g++) {
    TSGroup& Group = SSet.Table.GetGroup(g);
    for (int b = 0; b < Group.Len(); b++) {
      int Tries = 0; uint BuckNum;
      for (BuckNum = Group.Offset(b).GetPrimHashCd() & BuckM1;
       ! Table.IsEmpty(BuckNum); BuckNum = (BuckNum + Tries) & BuckM1) {
        Tries++;
        Assert(Tries < Reserved());
      }
      Assert(Table.IsEmpty(BuckNum));
      Table.Set(BuckNum, Group.Offset(b));
    }
    Group.Clr(true); // delete
  }
}

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::ResizeDelta(const int& ValsToAdd, const int& MnWanted) {
  if (Reserved() > MnWanted && Len()+ValsToAdd < ExpandThresh) { return; }
  const int NewSize = GetMinSize(Table.Len()+ValsToAdd, MnWanted);
  if (NewSize > Reserved()) {
    printf("***Resize SparseSet: %d->%d\n", Reserved(), NewSize);
    TSparseSet TmpSSet(Len()+ValsToAdd);
    TmpSSet.ResetThresh();
    TmpSSet.MoveFrom(*this, Len());
    Swap(TmpSSet);
  }
}

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::FindPos(const TKey& Key, int& Pos, int& PosToIns) const {
  const uint BuckM1 = Reserved() - 1;
  uint BuckNum = Key.GetPrimHashCd() & BuckM1;
  int Tries = 0;
  while (true) {
    if (Table.IsEmpty(BuckNum)) {
      Pos = -1;  PosToIns = BuckNum;  return;
    }
    else if (Key == Table.Get(BuckNum)) {
      Pos = BuckNum;  PosToIns = -1;  return;
    }
    Tries++;
    BuckNum = (BuckNum + Tries) & BuckM1;
    Assert(Tries < Reserved());
  }
}

template <class TKey, uint16 GroupSize>
TSparseSet<TKey, GroupSize>& TSparseSet<TKey, GroupSize>::operator = (const TSparseSet& SSet) {
  if (this != &SSet) {
    ShrinkThresh = SSet.ShrinkThresh;
    ExpandThresh = SSet.ExpandThresh;
    Table = SSet.Table;
  }
  return *this;
}

template <class TKey, uint16 GroupSize>
bool TSparseSet<TKey, GroupSize>::operator == (const TSparseSet& SSet) const {
  return Table == SSet.Table;
}

template <class TKey, uint16 GroupSize>
bool TSparseSet<TKey, GroupSize>::operator < (const TSparseSet& SSet) const {
  return Table < SSet.Table;
}

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::Swap(TSparseSet& SSet) {
  ::Swap(ShrinkThresh, SSet.ShrinkThresh);
  ::Swap(ExpandThresh, SSet.ExpandThresh);
  Table.Swap(SSet.Table);
}

template <class TKey, uint16 GroupSize>
int TSparseSet<TKey, GroupSize>::AddKey(const TKey& Key) {
  ResizeDelta(1);
  int Pos, PosToIns;  FindPos(Key, Pos, PosToIns);
  if (Pos != -1) { return Pos; } // key exists
  else {
    Table.Set(PosToIns, Key);
    return PosToIns;
  }
}

template <class TKey, uint16 GroupSize>
void TSparseSet<TKey, GroupSize>::GetKeyV(TVec<TKey>& KeyV) const {
  KeyV.Gen(Len(), 0);
  for (TIter I = BegI(); I < EndI(); I++) {
    KeyV.Add(I()); }
}

/////////////////////////////////////////////////
// Set-Hash-Key
#pragma pack(push, 1) // pack class size
template <class TKey>
class THashSetKey{
public:
  TInt Next;
  TInt HashCd;
  TKey Key;
public:
  THashSetKey():
    Next(-1), HashCd(-1), Key() {}
  THashSetKey(const int& _Next, const int& _HashCd, const TKey& _Key):
    Next(_Next), HashCd(_HashCd), Key(_Key) {}
  explicit THashSetKey(TSIn& SIn):
    Next(SIn), HashCd(SIn), Key(SIn) {}
  void Save(TSOut& SOut) const {
    Next.Save(SOut); HashCd.Save(SOut); Key.Save(SOut); }
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="") {
    XLoadHd(Nm); XLoad(Key); }
  void SaveXml(TSOut& SOut, const TStr& Nm) const {
    XSaveHd(Nm); XSave(Key); }

  THashSetKey& operator=(const THashSetKey& SetKey) {
    if (this!=&SetKey) { Next=SetKey.Next; HashCd=SetKey.HashCd; Key=SetKey.Key; }
    return *this; }
};
#pragma pack(pop)

/////////////////////////////////////////////////
// Set-Hash-Key-Iterator
template <class TKey>
class THashSetKeyI{
private:
  typedef THashSetKey<TKey> TSetKey;
  TSetKey* KeyI;
  TSetKey* EndI;
public:
  THashSetKeyI(): KeyI(NULL), EndI(NULL) { }
  THashSetKeyI(const THashSetKeyI& _SetKeyI):
    KeyI(_SetKeyI.KeyI), EndI(_SetKeyI.EndI) { }
  THashSetKeyI(const TSetKey* _KeyI, const TSetKey* _EndI):
    KeyI((TSetKey*)_KeyI), EndI((TSetKey*)_EndI) { }

  THashSetKeyI& operator=(const THashSetKeyI& SetKeyI) {
    KeyI=SetKeyI.KeyI; EndI=SetKeyI.EndI; return *this; }
  bool operator==(const THashSetKeyI& SetKeyI) const {
    return KeyI==SetKeyI.KeyI; }
  bool operator<(const THashSetKeyI& SetKeyI) const {
    return KeyI<SetKeyI.KeyI; }
  THashSetKeyI& operator++(int) { KeyI++; while (KeyI < EndI && KeyI->HashCd==-1) { KeyI++; } return *this; }
  THashSetKeyI& operator--(int) { do { KeyI--; } while (KeyI->HashCd==-1); return *this; }

  const TKey& operator*() const { return KeyI->Key; }
  const TKey& operator()() const { return KeyI->Key; }
  const TKey* operator->() const { return KeyI->Key; }

  const TKey& GetKey() const {Assert((KeyI!=NULL)&&(KeyI->HashCd!=-1)); return KeyI->Key; }
};

/////////////////////////////////////////////////
// Set-Table
template <class TKey, class THashFunc = TDefaultHashFunc<TKey> >
class THashSet{
public:
  typedef THashSetKeyI<TKey> TIter;
private:
  typedef THashSetKey<TKey> TSetKey;
  TIntV PortV;
  TVec<TSetKey> KeyV;
  TBool AutoSizeP;
  TInt FFreeKeyId, FreeKeys;
private:
  TSetKey& GetSetKey(const int& KeyId) {
    TSetKey& SetKey=KeyV[KeyId];
    Assert(SetKey.HashCd!=-1); return SetKey; }
  const TSetKey& GetSetKey(const int& KeyId) const {
    const TSetKey& SetKey=KeyV[KeyId];
    Assert(SetKey.HashCd!=-1); return SetKey; }
  uint GetNextPrime(const uint& Val) const;
  void Resize();
public:
  THashSet():
    PortV(), KeyV(),
    AutoSizeP(true), FFreeKeyId(-1), FreeKeys(0) { }
  THashSet(const THashSet& Set):
    PortV(Set.PortV), KeyV(Set.KeyV), AutoSizeP(Set.AutoSizeP),
    FFreeKeyId(Set.FFreeKeyId), FreeKeys(Set.FreeKeys) { }
  THashSet(const int& ExpectVals, const bool& _AutoSizeP=false);
  explicit THashSet(const TVec<TKey>& KeyV);
  explicit THashSet(TSIn& SIn):
    PortV(SIn), KeyV(SIn),
    AutoSizeP(SIn), FFreeKeyId(SIn), FreeKeys(SIn) {
    SIn.LoadCs(); }
  void Load(TSIn& SIn) {
    PortV.Load(SIn); KeyV.Load(SIn);
    AutoSizeP=TBool(SIn); FFreeKeyId=TInt(SIn); FreeKeys=TInt(SIn);
    SIn.LoadCs(); }
  void Save(TSOut& SOut) const {
    PortV.Save(SOut); KeyV.Save(SOut);
    AutoSizeP.Save(SOut); FFreeKeyId.Save(SOut); FreeKeys.Save(SOut);
    SOut.SaveCs(); }
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="") {
    XLoadHd(Nm); TVec<TSetKey> KeyV; XLoad(KeyV); XLoad(AutoSizeP);
    for (int KeyN=0; KeyN<KeyV.Len(); KeyN++) {
      AddKey(KeyV[KeyN].Key); }}
  void SaveXml(TSOut& SOut, const TStr& Nm) {
    Defrag(); XSaveHd(Nm); XSave(KeyV); XSave(AutoSizeP); }

  THashSet& operator=(const THashSet& Set) {
    if (this!=&Set) {
      PortV=Set.PortV; KeyV=Set.KeyV; AutoSizeP=Set.AutoSizeP;
      FFreeKeyId=Set.FFreeKeyId; FreeKeys=Set.FreeKeys; }
    return *this; }
  bool operator==(const THashSet& Set) const;
  const TKey& operator[](const int& KeyId) const {return GetSetKey(KeyId).Key; }
  TKey& operator[](const int& KeyId) {return GetSetKey(KeyId).Key; }
  //bool operator()(const TKey& Key) {return IsKey(Key); }
  ::TSize GetMemUsed() const {
    return PortV.GetMemUsed() + KeyV.GetMemUsed() + sizeof(bool) + 2*sizeof(int); }

  TIter BegI() const {
    if (Len()>0) {
      if (IsKeyIdEqKeyN()) { return TIter(KeyV.BegI(), KeyV.EndI()); }
      int FKeyId=-1;  FNextKeyId(FKeyId);
      return TIter(KeyV.BegI()+FKeyId, KeyV.EndI()); }
    return TIter(KeyV.EndI(), KeyV.EndI());
  }
  TIter EndI() const {return TIter(KeyV.EndI(), KeyV.EndI()); }
  TIter GetI(const TKey& Key) const {return TIter(&KeyV[GetKeyId(Key)], KeyV.EndI()); }

  void Gen(const int& ExpectVals) {
    PortV.Gen(GetNextPrime(ExpectVals/2)); KeyV.Gen(ExpectVals, 0);
    FFreeKeyId=-1; FreeKeys=0; PortV.PutAll(TInt(-1)); }

  void Clr(const bool& DoDel=true, const int& NoDelLim=-1);
  bool Empty() const {return Len()==0; }
  int Len() const {return KeyV.Len()-FreeKeys; }
  int GetPorts() const {return PortV.Len(); }
  bool IsAutoSize() const {return AutoSizeP; }
  int GetMxKeyIds() const {return KeyV.Len(); }
  int GetReservedKeyIds() const { return KeyV.Reserved(); }
  bool IsKeyIdEqKeyN() const {return FreeKeys==0; }

  int AddKey(const TKey& Key);
  void AddKeyV(const TVec<TKey>& KeyV);

  void DelKey(const TKey& Key);
  void DelIfKey(const TKey& Key) {
    int KeyId; if (IsKey(Key, KeyId)) {DelKeyId(KeyId); }}
  void DelKeyId(const int& KeyId) {DelKey(GetKey(KeyId)); }
  void DelKeyIdV(const TIntV& KeyIdV) {
    for (int KeyIdN=0; KeyIdN<KeyIdV.Len(); KeyIdN++) {DelKeyId(KeyIdV[KeyIdN]); }}

  void MarkDelKey(const TKey& Key);
  void MarkDelKeyId(const int& KeyId) {MarkDelKey(GetKey(KeyId)); }

  const TKey& GetKey(const int& KeyId) const {
    return GetSetKey(KeyId).Key; }
  int GetKeyId(const TKey& Key) const;
  int GetRndKeyId(TRnd& Rnd) const {
    IAssert(IsKeyIdEqKeyN());
    IAssert(Len()>0);
    return Rnd.GetUniDevInt(Len()); }
  bool IsKey(const TKey& Key) const {return GetKeyId(Key)!=-1; }
  bool IsKey(const TKey& Key, int& KeyId) const {
    KeyId=GetKeyId(Key); return KeyId!=-1; }
  bool IsKeyId(const int& KeyId) const {
    return (0<=KeyId)&&(KeyId<KeyV.Len())&&(KeyV[KeyId].HashCd!=-1); }

  int FFirstKeyId() const {return 0-1; }
  bool FNextKeyId(int& KeyId) const;
  void GetKeyV(TVec<TKey>& KeyV) const;
  void Swap(THashSet& Set);

  void Defrag();
  void Pack() {KeyV.Pack(); }
};

template <class TKey, class THashFunc>
uint THashSet<TKey, THashFunc>::GetNextPrime(const uint& Val) const {
  uint* f=(uint*)TIntH::HashPrimeT, *m, *l=(uint*)TIntH::HashPrimeT + (int)TIntH::HashPrimes;
  int h, len = (int)TIntH::HashPrimes;
  while (len > 0) {
    h = len >> 1;  m = f + h;
    if (*m < Val) { f = m;  f++;  len = len - h - 1; }
    else len = h;
  }
  return f == l ? *(l - 1) : *f;
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::Resize() {
  // resize & initialize port vector
  if (PortV.Len()==0) {PortV.Gen(17); }
  else if (AutoSizeP&&(KeyV.Len()>2*PortV.Len())) {
    PortV.Gen(GetNextPrime(PortV.Len()+1));
  } else {
    return;
  }
  PortV.PutAll(TInt(-1));
  // reSet keys
  for (int KeyId=0; KeyId<KeyV.Len(); KeyId++) {
    TSetKey& SetKey=KeyV[KeyId];
    if (SetKey.HashCd!=-1) {
      int PortN=abs(THashFunc::GetPrimHashCd(SetKey.Key)%PortV.Len());
      SetKey.Next=PortV[PortN];
      PortV[PortN]=KeyId;
    }
  }
}

template <class TKey, class THashFunc>
THashSet<TKey, THashFunc>::THashSet(const int& ExpectVals, const bool& _AutoSizeP):
  PortV(GetNextPrime(ExpectVals/2+1)), KeyV(ExpectVals, 0),
  AutoSizeP(_AutoSizeP), FFreeKeyId(-1), FreeKeys(0) {
  PortV.PutAll(TInt(-1));
}

template <class TKey, class THashFunc>
THashSet<TKey, THashFunc>::THashSet(const TVec<TKey>& _KeyV) :
 PortV(GetNextPrime(_KeyV.Len()/2+1)), KeyV(_KeyV.Len(), 0),
 AutoSizeP(false), FFreeKeyId(-1), FreeKeys(0) {
  PortV.PutAll(TInt(-1));
  for (int i = 0; i < _KeyV.Len(); i++) {
    AddKey(_KeyV[i]);
  }
}

template <class TKey, class THashFunc>
bool THashSet<TKey, THashFunc>::operator==(const THashSet& Set) const {
  if (Len() != Set.Len()) { return false; }
  for (int k = FFirstKeyId(); FNextKeyId(k); k++) {
    if (! Set.IsKey(GetKey(k))) { return false; }
  }
  return true;
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::Clr(const bool& DoDel, const int& NoDelLim) {
  if (DoDel) {
    PortV.Clr(); KeyV.Clr();
  } else {
    PortV.PutAll(TInt(-1));
    KeyV.Clr(DoDel, NoDelLim);
  }
  FFreeKeyId=TInt(-1); FreeKeys=TInt(0);
}

template <class TKey, class THashFunc>
int THashSet<TKey, THashFunc>::AddKey(const TKey& Key) {
  if ((KeyV.Len()>2*PortV.Len())||PortV.Empty()) {Resize(); }
  int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId=PortV[PortN];

  while ((KeyId!=-1) &&
   !((KeyV[KeyId].HashCd==HashCd) && (KeyV[KeyId].Key==Key))) {
    PrevKeyId=KeyId; KeyId=KeyV[KeyId].Next; }

  if (KeyId==-1) {
    if (FFreeKeyId==-1) {
      KeyId=KeyV.Add(TSetKey(-1, HashCd, Key));
    } else {
      KeyId=FFreeKeyId; FFreeKeyId=KeyV[FFreeKeyId].Next; FreeKeys--;
      KeyV[KeyId].Next = -1;
      KeyV[KeyId].HashCd = HashCd;
      KeyV[KeyId].Key = Key;
    }
    if (PrevKeyId==-1) {
      PortV[PortN]=KeyId;
    } else {
      KeyV[PrevKeyId].Next=KeyId;
    }
  }
  return KeyId;
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::AddKeyV(const TVec<TKey>& KeyV) {
  for (int i = 0; i < KeyV.Len(); i++) { AddKey(KeyV[i]); }
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::DelKey(const TKey& Key) {
  IAssert(!PortV.Empty());
  int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId=PortV[PortN];

  while ((KeyId!=-1) &&
   !((KeyV[KeyId].HashCd==HashCd) && (KeyV[KeyId].Key==Key))) {
    PrevKeyId=KeyId; KeyId=KeyV[KeyId].Next; }

  IAssertR(KeyId!=-1, Key.GetStr());
  if (PrevKeyId==-1) {PortV[PortN]=KeyV[KeyId].Next; }
  else {KeyV[PrevKeyId].Next=KeyV[KeyId].Next; }
  KeyV[KeyId].Next=FFreeKeyId; FFreeKeyId=KeyId; FreeKeys++;
  KeyV[KeyId].HashCd=TInt(-1);
  KeyV[KeyId].Key=TKey();
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::MarkDelKey(const TKey& Key) {
  IAssert(!PortV.Empty());
  int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId=PortV[PortN];

  while ((KeyId!=-1) &&
   !((KeyV[KeyId].HashCd==HashCd) && (KeyV[KeyId].Key==Key))) {
    PrevKeyId=KeyId; KeyId=KeyV[KeyId].Next; }

  IAssertR(KeyId!=-1, Key.GetStr());
  if (PrevKeyId==-1) {PortV[PortN]=KeyV[KeyId].Next; }
  else {KeyV[PrevKeyId].Next=KeyV[KeyId].Next; }
  KeyV[KeyId].Next=FFreeKeyId; FFreeKeyId=KeyId; FreeKeys++;
  KeyV[KeyId].HashCd=TInt(-1);
}

template <class TKey, class THashFunc>
int THashSet<TKey, THashFunc>::GetKeyId(const TKey& Key) const {
  if (PortV.Empty()) {return -1; }
  int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int KeyId=PortV[PortN];

  while ((KeyId!=-1) &&
   !((KeyV[KeyId].HashCd==HashCd) && (KeyV[KeyId].Key==Key))) {
    KeyId=KeyV[KeyId].Next; }
  return KeyId;
}

template <class TKey, class THashFunc>
bool THashSet<TKey, THashFunc>::FNextKeyId(int& KeyId) const {
  do {KeyId++; } while ((KeyId<KeyV.Len())&&(KeyV[KeyId].HashCd==-1));
  return KeyId<KeyV.Len();
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::GetKeyV(TVec<TKey>& KeyV) const {
  KeyV.Clr();
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)) {
    KeyV.Add(GetKey(KeyId)); }
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::Swap(THashSet& Set) {
  if (this!=&Set) {
    PortV.Swap(Set.PortV);
    KeyV.Swap(Set.KeyV);
    ::Swap(AutoSizeP, Set.AutoSizeP);
    ::Swap(FFreeKeyId, Set.FFreeKeyId);
    ::Swap(FreeKeys, Set.FreeKeys);
  }
}

template <class TKey, class THashFunc>
void THashSet<TKey, THashFunc>::Defrag() {
  if (!IsKeyIdEqKeyN()) {
    THashSet<TKey> Set(PortV.Len());
    int KeyId=FFirstKeyId(); TKey Key;
    while (FNextKeyId(KeyId)) {
      GetKey(KeyId, Key);
      Set.AddKey(Key);
    }
    Pack();
    operator=(Set);
    IAssert(IsKeyIdEqKeyN());
  }
}

/////////////////////////////////////////////////
// Common Hash Set Types
typedef THashSet<TInt> TIntSet;
typedef THashSet<TFlt> TFltSet;
typedef THashSet<TStr> TStrSet;
typedef THashSet<TIntPr> TIntPrSet;

/////////////////////////////////////////////////
// Packed Vec
template<class TVal>
class TPackVec {
public:
  typedef TVal* TIter;
private:
  int Vals;
  TVal* ValT;
  void ResizeDelta(const int& ValsToAdd=1);
public:
  TPackVec() : Vals(0), ValT(NULL) { }
  TPackVec(const TPackVec& Vec) : Vals(0), ValT(NULL) {
    Gen(Vec.Len());
    memcpy(ValT, Vec.ValT, sizeof(TVal)*Vals);
  }
  explicit TPackVec(const int& _Vals) : Vals(_Vals) {
    if (Vals==0) { ValT=NULL; } else { ValT = (TVal *) realloc(ValT, sizeof(TVal)*Vals); } }
  ~TPackVec() { if (ValT != NULL) { free(ValT); } }
  explicit TPackVec(TSIn& SIn): Vals(0), ValT(NULL) { Load(SIn); }
  void Load(TSIn& SIn);
  void Save(TSOut& SOut) const;

  const TVal& operator [] (const int& ValN) const { return ValT[ValN]; }
  TVal& operator [] (const int& ValN) { return ValT[ValN]; }
  TPackVec<TVal>& operator = (const TPackVec<TVal>& Vec) { Gen(Vec.Len());
    memcpy(ValT, Vec.ValT, sizeof(TVal)*Vals); return *this; }
  TVec<TVal>& operator = (const TVec<TVal>& Vec) { Gen(Vec.Len());
    memcpy(ValT, Vec.ValT, sizeof(TVal)*Vals); return *this; }

  void Gen(const int& _Vals) {
    if (ValT != NULL) { free(ValT); } Vals = _Vals;
    if (Vals==0) { ValT=NULL; } else { ValT = (TVal *) realloc(ValT, sizeof(TVal)*Vals); } }
  void Clr() { if (ValT != NULL) { free(ValT); ValT=NULL; } Vals = 0; }

  bool Empty() const {return Vals==0; }
  int Len() const {return Vals; }
  const TVal& Last() const { return ValT[Len()-1]; }
  TVal& Last() { return ValT[Len()-1]; }

  TIter BegI() const {return ValT; }
  TIter EndI() const {return ValT+Vals; }
  TIter GetI(const int& ValN) const { return ValT+ValN; }

  void Add(const TVal& Val) { ResizeDelta(1); ValT[Vals-1]=Val; }
  void AddV(const TPackVec<TVal>& ValV) { ResizeDelta(ValV.Len());
    memcpy(ValT+Vals-ValV.Len(), ValV.BegI(), sizeof(TVal)*ValV.Len()); }
  void AddV(const TVec<TVal>& ValV) { ResizeDelta(ValV.Len());
    memcpy(ValT+Vals-ValV.Len(), ValV.BegI(), sizeof(TVal)*ValV.Len()); }
  void AddV(TSIn& FIn) { int NVals;  FIn.Load(NVals);
    ResizeDelta(NVals);  FIn.LoadBf(ValT+Vals-NVals, sizeof(TVal)*NVals); }

  void Sort(const bool& Asc=true) {
    if (Asc) { TVec<TVal>::QSortCmp(BegI(), EndI(), TLss<TVal>()); }
    else { TVec<TVal>::QSortCmp(BegI(), EndI(), TGtr<TVal>()); }
  }
};

template<class TVal>
void TPackVec<TVal>::ResizeDelta(const int& ValsToAdd) {
  if (ValsToAdd == 0) return;
  Vals += ValsToAdd;
  ValT = (TVal *) realloc(ValT, sizeof(TVal)*Vals);
  EAssert(ValT != NULL);
}

template<class TVal>
void TPackVec<TVal>::Load(TSIn& SIn) {
  SIn.Load(Vals);
  if (Vals==0) {
    if (ValT != NULL) { free(ValT); }
    ValT = NULL; }
  else {
    ValT = (TVal *) realloc(ValT, sizeof(TVal)*Vals);
  }
  SIn.LoadBf(ValT, sizeof(TVal)*Vals);
}

template<class TVal>
void TPackVec<TVal>::Save(TSOut& SOut) const {
  SOut.Save(Vals);
  if (Vals != 0) {
    SOut.SaveBf(ValT, sizeof(TVal)*Vals); }
}

#endif
