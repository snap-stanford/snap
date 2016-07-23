#ifndef hashmp_h
#define hashmp_h

#ifdef GCC_ATOMIC

#include "bd.h"

/////////////////////////////////////////////////
// PHash-Table-Key-Data
#pragma pack(push, 1) // pack class size
template <class TKey, class TDat>
class THashMPKeyDat {
public:
  TInt HashCd;
  TKey Key;
  TDat Dat;
public:
  THashMPKeyDat():
    HashCd(-1), Key(), Dat(){}
  THashMPKeyDat(const int& _HashCd, const TKey& _Key):
    HashCd(_HashCd), Key(_Key), Dat(){}
  explicit THashMPKeyDat(TSIn& SIn):
    HashCd(SIn), Key(SIn), Dat(SIn){}
  void Save(TSOut& SOut) const {
    HashCd.Save(SOut); Key.Save(SOut); Dat.Save(SOut);}

  bool operator==(const THashMPKeyDat& HashKeyDat) const {
    if (this==&HashKeyDat || (HashCd==HashKeyDat.HashCd
      && Key==HashKeyDat.Key && Dat==HashKeyDat.Dat)){return true;}
    return false;}
  THashMPKeyDat& operator=(const THashMPKeyDat& HashKeyDat){
    if (this!=&HashKeyDat){
      HashCd=HashKeyDat.HashCd; Key=HashKeyDat.Key;
      Dat=HashKeyDat.Dat;}
    return *this;}
};
#pragma pack(pop)

/////////////////////////////////////////////////
// PHash-Table-Key-Data-Iterator
template<class TKey, class TDat>
class THashMPKeyDatI{
public:
  typedef THashMPKeyDat<TKey, TDat> TPHKeyDat;
private:
  TPHKeyDat* KeyDatI;
  TPHKeyDat* EndI;
public:
  THashMPKeyDatI(): KeyDatI(NULL), EndI(NULL){}
  THashMPKeyDatI(const THashMPKeyDatI& _HashKeyDatI):
    KeyDatI(_HashKeyDatI.KeyDatI), EndI(_HashKeyDatI.EndI){}
  THashMPKeyDatI(const TPHKeyDat* _KeyDatI, const TPHKeyDat* _EndI):
    KeyDatI((TPHKeyDat*)_KeyDatI), EndI((TPHKeyDat*)_EndI){}

  THashMPKeyDatI& operator=(const THashMPKeyDatI& HashKeyDatI){
    KeyDatI=HashKeyDatI.KeyDatI; EndI=HashKeyDatI.EndI; return *this;}
  bool operator==(const THashMPKeyDatI& HashKeyDatI) const {
    return KeyDatI==HashKeyDatI.KeyDatI;}
  bool operator<(const THashMPKeyDatI& HashKeyDatI) const {
    return KeyDatI<HashKeyDatI.KeyDatI;}
  THashMPKeyDatI& operator++(int){ KeyDatI++; while (KeyDatI < EndI && KeyDatI->HashCd==-1) { KeyDatI++; } return *this; }
  THashMPKeyDatI& operator--(int){ do { KeyDatI--; } while (KeyDatI->HashCd==-1); return *this;}
  TPHKeyDat& operator*() const { return *KeyDatI; }
  TPHKeyDat& operator()() const { return *KeyDatI; }
  TPHKeyDat* operator->() const { return KeyDatI; }
  THashMPKeyDatI& Next(){ operator++(1); return *this; }

  /// Tests whether the iterator has been initialized.
  bool IsEmpty() const { return KeyDatI == NULL; }
  /// Tests whether the iterator is pointing to the past-end element.
  bool IsEnd() const { return EndI == KeyDatI; }
  
  const TKey& GetKey() const {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Key;}
  const TDat& GetDat() const {/*Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1));*/ return KeyDatI->Dat;}
  TDat& GetDat() {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Dat;}
};

//#//////////////////////////////////////////////
/// Hash-Table with multiprocessing support.
template<class TKey, class TDat, class THashFunc = TDefaultHashFunc<TKey> >
class THashMP {
public:
  enum {HashPrimes=32};
  static const unsigned int HashPrimeT[HashPrimes];
public:
  typedef THashMPKeyDatI<TKey, TDat> TIter;
private:
  typedef THashMPKeyDat<TKey, TDat> TPHKeyDat;
  typedef TPair<TKey, TDat> TKeyDatP;
  TVec<TPHKeyDat> Table;
  TInt NumVals;
private:
  class THashMPKeyDatCmp {
  public:
    const THashMP<TKey, TDat, THashFunc>& Hash;
    bool CmpKey, Asc;
    THashMPKeyDatCmp(THashMP<TKey, TDat, THashFunc>& _Hash, const bool& _CmpKey, const bool& _Asc) :
      Hash(_Hash), CmpKey(_CmpKey), Asc(_Asc) { }
    bool operator () (const int& KeyId1, const int& KeyId2) const {
      if (CmpKey) {
        if (Asc) { return Hash.GetKey(KeyId1) < Hash.GetKey(KeyId2); }
        else { return Hash.GetKey(KeyId2) < Hash.GetKey(KeyId1); } }
      else {
        if (Asc) { return Hash[KeyId1] < Hash[KeyId2]; }
        else { return Hash[KeyId2] < Hash[KeyId1]; } } }
  };
private:
  TPHKeyDat& GetPHashKeyDat(const int& KeyId){
    TPHKeyDat& KeyDat=Table[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  const TPHKeyDat& GetPHashKeyDat(const int& KeyId) const {
    const TPHKeyDat& KeyDat=Table[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  uint GetNextPrime(const uint& Val) const;
//  void Resize();
public:
  THashMP():
    Table(), NumVals(0){}
  THashMP(const THashMP& PHash):
    Table(PHash.Table), NumVals(PHash.NumVals){}
  explicit THashMP(const int& ExpectVals) {
    Gen(ExpectVals);}
  explicit THashMP(TSIn& SIn):
    Table(SIn), NumVals(SIn){
    SIn.LoadCs();}
  void Load(TSIn& SIn){
    Table.Load(SIn); NumVals.Load(SIn);
    SIn.LoadCs();}
  void Save(TSOut& SOut) const {
    Table.Save(SOut); NumVals.Save(SOut);
    SOut.SaveCs();}

  THashMP& operator=(const THashMP& Hash){
    if (this!=&Hash){
      Table=Hash.Table; NumVals=Hash.NumVals;}
    return *this;}
  bool operator==(const THashMP& Hash) const; //J: zdaj tak kot je treba
//bool operator < (const THash& Hash) const { Fail; return true; }
  const TDat& operator[](const int& KeyId) const {return GetPHashKeyDat(KeyId).Dat;}
  TDat& operator[](const int& KeyId){return GetPHashKeyDat(KeyId).Dat;}
  TDat& operator()(const TKey& Key){return AddDat(Key);}
  ::TSize GetMemUsed() const {
    // return PortV.GetMemUsed()+KeyDatV.GetMemUsed()+sizeof(bool)+2*sizeof(int);}
      int64 MemUsed = sizeof(int);
      for (int TableN = 0; TableN < Table.Len(); TableN++) {
          MemUsed += int64(sizeof(TInt));
          MemUsed += int64(Table[TableN].Key.GetMemUsed());
          MemUsed += int64(Table[TableN].Dat.GetMemUsed());
      }
      return ::TSize(MemUsed);
  }

  TIter BegI() const {
    if (Len() == 0){return TIter(Table.EndI(), Table.EndI());}
    return TIter(Table.BegI(), Table.EndI());}
  TIter EndI() const {return TIter(Table.EndI(), Table.EndI());}
  //TIter GetI(const int& KeyId) const {return TIter(&KeyDatV[KeyId], KeyDatV.EndI());}
  TIter GetI(const TKey& Key) const {return TIter(&Table[GetKeyId(Key)], Table.EndI());}

  void Gen(const int& ExpectVals){
    Table.Gen(GetNextPrime(2 * ExpectVals));}

  void Clr(const bool& DoDel=true);
  bool Empty() const {return Len()==0;}
  int Len() const {return NumVals;}
  void SetLen(const int& Length) {NumVals=Length;}
  int GetMxKeyIds() const {return Table.Len();}
//bool IsKeyIdEqKeyN() const {return FreeKeys==0;}
  int GetReservedKeyIds() const {return Table.Reserved();}

  // TODO: Non-unique keys
  int AddKey(const TKey& Key);
  int AddKey11(const int& Idx, const TKey& Key, bool& Found);
  int AddKey12(const int& Idx, const TKey& Key, bool& Found);
  int AddKey13(const int& Idx, const TKey& Key);
  int AddKey1(const TKey& Key, bool& Found);
  int AddKey2(const int& Idx, const TKey& Key, bool& Found);
  TDat& AddDatId(const TKey& Key){
    int KeyId=AddKey(Key); return Table[KeyId].Dat=KeyId;}
  // TODO: Make Dat updatable
  TDat& AddDat(const TKey& Key){return Table[AddKey(Key)].Dat;}
  TDat& AddDat(const TKey& Key, const TDat& Dat){
    return Table[AddKey(Key)].Dat=Dat;}

  const TKey& GetKey(const int& KeyId) const { return GetPHashKeyDat(KeyId).Key;}
  int GetKeyId(const TKey& Key) const;
  /// Get an index of a random element. If the hash table has many deleted keys, this may take a long time.
  int GetRndKeyId(TRnd& Rnd) const;
  /// Get an index of a random element. If the hash table has many deleted keys, defrag the hash table first (that's why the function is non-const).
  int GetRndKeyId(TRnd& Rnd, const double& EmptyFrac);
  bool IsKey(const TKey& Key) const {return GetKeyId(Key)!=-1;}
  bool IsKey(const TKey& Key, int& KeyId) const { KeyId=GetKeyId(Key); return KeyId!=-1;}
  bool IsKeyId(const int& KeyId) const {
    return (0<=KeyId)&&(KeyId<Table.Len())&&(Table[KeyId].HashCd!=-1);}
  const TDat& GetDat(const TKey& Key) const {return Table[GetKeyId(Key)].Dat;}
  TDat& GetDat(const TKey& Key){return Table[GetKeyId(Key)].Dat;}
//  TKeyDatP GetKeyDat(const int& KeyId) const {
//    TPHKeyDat& KeyDat=GetPHashKeyDat(KeyId);
//    return TKeyDatP(KeyDat.Key, KeyDat.Dat);}
  void GetKeyDat(const int& KeyId, TKey& Key, TDat& Dat) const {
    const TPHKeyDat& KeyDat=GetPHashKeyDat(KeyId);
    Key=KeyDat.Key; Dat=KeyDat.Dat;}
  bool IsKeyGetDat(const TKey& Key, TDat& Dat) const {int KeyId;
    if (IsKey(Key, KeyId)){Dat=GetPHashKeyDat(KeyId).Dat; return true;}
    else {return false;}}

  int FFirstKeyId() const {return 0-1;}
  bool FNextKeyId(int& KeyId) const;
  void GetKeyV(TVec<TKey>& KeyV) const;
  void GetDatV(TVec<TDat>& DatV) const;
  void GetKeyDatPrV(TVec<TPair<TKey, TDat> >& KeyDatPrV) const;
  void GetDatKeyPrV(TVec<TPair<TDat, TKey> >& DatKeyPrV) const;
  void GetKeyDatKdV(TVec<TKeyDat<TKey, TDat> >& KeyDatKdV) const;
  void GetDatKeyKdV(TVec<TKeyDat<TDat, TKey> >& DatKeyKdV) const;

  void Swap(THashMP& Hash);
//void Defrag();
  void Pack(){Table.Pack();}
/*void Sort(const bool& CmpKey, const bool& Asc);
  void SortByKey(const bool& Asc=true) { Sort(true, Asc); }
  void SortByDat(const bool& Asc=true) { Sort(false, Asc); }*/
};

template<class TKey, class TDat, class THashFunc>
const unsigned int THashMP<TKey, TDat, THashFunc>::HashPrimeT[HashPrimes]={
  3ul, 5ul, 11ul, 23ul,
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

template<class TKey, class TDat, class THashFunc>
uint THashMP<TKey, TDat, THashFunc>::GetNextPrime(const uint& Val) const {
  const uint* f=(const uint*)HashPrimeT, *m, *l=(const uint*)HashPrimeT + (int)HashPrimes;
  int h, len = (int)HashPrimes;
  while (len > 0) {
    h = len >> 1;  m = f + h;
    if (*m < Val) { f = m;  f++;  len = len - h - 1; }
    else len = h;
  }
  return f == l ? *(l - 1) : *f;
}

template<class TKey, class TDat, class THashFunc>
bool THashMP<TKey, TDat, THashFunc>::operator==(const THashMP& Hash) const {
  if (Len() != Hash.Len()) { return false; }
  for (int i = FFirstKeyId(); FNextKeyId(i); ) {
    const TKey& Key = GetKey(i);
    if (! Hash.IsKey(Key)) { return false; }
    if (GetDat(Key) != Hash.GetDat(Key)) { return false; }
  }
  return true;
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::AddKey(const TKey& Key) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const int BegTableN=abs(Key.GetPrimHashCd()%Table.Len());
  const int HashCd=abs(Key.GetSecHashCd());

  int TableN = BegTableN;
  while (Table[TableN].HashCd != -1 || 
    (!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd))) {
    TableN = (TableN + 1) % Table.Len();    
  }
  Table[TableN].Key = Key;
  __sync_fetch_and_add(&NumVals.Val, 1);
  return TableN;
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::AddKey11(const int& BegTableN, const TKey& Key, bool& Found) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const int Length = Table.Len();
  const int HashCd=abs(Key.GetSecHashCd());

  int TableN = BegTableN;
  Found = false;
  do {
    if (Table[TableN].HashCd.Val != -1) {
      if (Table[TableN].Key == Key) {
        Found = true;
        return TableN;
      }
    } else if (__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd)) {
      break;
    }
    
    TableN++;
    if (TableN >= Length) {
      TableN = 0;
    }
  } while (1);

  Table[TableN].Key = Key;
  return TableN;
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::AddKey12(const int& BegTableN, const TKey& Key, bool& Found) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const int Length = Table.Len();
  //const int HashCd=abs(Key.GetSecHashCd());

  // HashCd values:
  //  -1: empty slot
  //   1: occupied slot, invalid key
  //   2: occupied slot, valid key

  int TableN = BegTableN;
  do {
    int HashCd = Table[TableN].HashCd.Val;
    if (HashCd == -1) {
      // an empty slot
      if (__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, 1)) {
        // an empty slot has been claimed, key is invalid
        break;
      }
    } else {
      // slot is occupied
      if (HashCd != 2) {
        // key is not yet valid, wait for a valid key
        while (!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, 2, 2)) {
          usleep(20);
        }
      }
      if (Table[TableN].Key == Key) {
        // the key matches
        Found = true;
        return TableN;
      }
      // move to the next slot
      TableN++;
      if (TableN >= Length) {
        TableN = 0;
      }
    }
  } while (1);

  // write the key, indicate a valid key
  Table[TableN].Key = Key;
  Table[TableN].HashCd.Val = 2;

  Found = false;
  return TableN;
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::AddKey13(const int& BegTableN, const TKey& Key) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const int Length = Table.Len();
  //const int HashCd=abs(Key.GetSecHashCd());

  // HashCd values:
  //  -1: empty slot
  //   1: occupied slot, invalid key
  //   2: occupied slot, valid key

  int TableN = BegTableN;
  do {
    int HashCd = Table[TableN].HashCd.Val;
    if (HashCd == -1) {
      // an empty slot
      if (__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, 1)) {
        // an empty slot has been claimed, key is invalid
        break;
      }
    } else {
      // slot is occupied, move to the next slot
      TableN++;
      if (TableN >= Length) {
        TableN = 0;
      }
    }
  } while (1);

  // write the key, indicate a valid key
  Table[TableN].Key = Key;
  Table[TableN].HashCd.Val = 2;

  return TableN;
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::AddKey1(const TKey& Key, bool& Found) {
  const int Length = Table.Len();
  //const int Length = 12582917;
  const int BegTableN = abs(Key.GetPrimHashCd() % Length);
  const int HashCd = abs(Key.GetSecHashCd());

  //printf("AddKey1 %5d %5d\n", Length, BegTableN);

  int TableN = BegTableN;
  while (Table[TableN].HashCd.Val != -1) {
    if (Table[TableN].Key == Key) {
      Found = true;
      return TableN;
    }
    TableN++;
    if (TableN >= Length) {
      TableN = 0;
    }
    //(!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd))) {
    //TableN = (TableN + 1) % Table.Len();    
  }

  NumVals.Val++;
  Found = false;
  Table[TableN].HashCd.Val = HashCd;
  Table[TableN].Key = Key;
  return TableN;

  // TODO:RS, need to set the length at the end
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::AddKey2(const int& BegTableN, const TKey& Key, bool& Found) {
  //const int Length = 12582917;
  const int Length = Table.Len();
  const int HashCd = abs(Key.GetSecHashCd());

  //printf("AddKey2 %5d %5d\n", Length, BegTableN);

  int TableN = BegTableN;
  while (Table[TableN].HashCd.Val != -1) {
    if (Table[TableN].Key == Key) {
      Found = true;
      return TableN;
    }
    TableN++;
    if (TableN >= Length) {
      TableN = 0;
    }
    //(!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd))) {
    //TableN = (TableN + 1) % Table.Len();    
  }

  //NumVals.Val++;
  Found = false;
  Table[TableN].HashCd.Val = HashCd;
  Table[TableN].Key = Key;
  return TableN;

  // TODO:RS, need to set the length at the end
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::GetKeyId(const TKey& Key) const {
  const int BegTableN=abs(Key.GetPrimHashCd()%Table.Len());
  //const int HashCd=abs(Key.GetSecHashCd());

  int TableN = BegTableN;
  while (Table[TableN].HashCd != -1) {
    if (Table[TableN].Key == Key) { return TableN; }
    TableN = (TableN + 1) % Table.Len();
    if (TableN == BegTableN) { return -1; }
  }

  return -1;
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::Clr(const bool& DoDel){
  if (DoDel){
    Table.Clr();
  } else {
    Table.PutAll(TPHKeyDat());
  }
  NumVals = TInt(0);
}

template<class TKey, class TDat, class THashFunc>
bool THashMP<TKey, TDat, THashFunc>::FNextKeyId(int& KeyId) const {
  do {KeyId++;} while ((KeyId<Table.Len())&&(Table[KeyId].HashCd==-1));
  return KeyId<Table.Len();
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::GetKeyV(TVec<TKey>& KeyV) const {
  KeyV.Gen(Len(), 0);
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    KeyV.Add(GetKey(KeyId));}
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::GetDatV(TVec<TDat>& DatV) const {
  DatV.Gen(Len(), 0);
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    DatV.Add(GetPHashKeyDat(KeyId).Dat);}
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::GetKeyDatPrV(TVec<TPair<TKey, TDat> >& KeyDatPrV) const {
  KeyDatPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatPrV.Add(TPair<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::GetDatKeyPrV(TVec<TPair<TDat, TKey> >& DatKeyPrV) const {
  DatKeyPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyPrV.Add(TPair<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::GetKeyDatKdV(TVec<TKeyDat<TKey, TDat> >& KeyDatKdV) const {
  KeyDatKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatKdV.Add(TKeyDat<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::GetDatKeyKdV(TVec<TKeyDat<TDat, TKey> >& DatKeyKdV) const {
  DatKeyKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyKdV.Add(TKeyDat<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashMP<TKey, TDat, THashFunc>::Swap(THashMP& Hash) {
  if (this!=&Hash){
    Table.Swap(Hash.Table);
    ::Swap(NumVals, Hash.NumVals);
  }
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::GetRndKeyId(TRnd& Rnd) const  {
  printf("*** ERROR *** THashMP<TKey, TDat, THashFunc>::GetRndKeyId called\n");
  return 0;
}

template<class TKey, class TDat, class THashFunc>
int THashMP<TKey, TDat, THashFunc>::GetRndKeyId(TRnd& Rnd, const double& EmptyFrac) {
  printf("*** ERROR *** THashMP<TKey, TDat, THashFunc>::GetRndKeyId called\n");
  return 0;
}

#endif // GCC_ATOMIC

#endif // hashmp_h
