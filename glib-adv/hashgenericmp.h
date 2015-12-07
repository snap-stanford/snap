#include "bd.h"

#ifdef GLib_GLIBC
inline unsigned int __sync_fetch_and_add_2(volatile unsigned int* p, unsigned int incr)
{
    unsigned int result;
    asm volatile("lock; xadd %0, %1" :
            "=r"(result), "=m"(*p):
            "0"(incr), "m"(*p) :
            "memory");
    return result + 1;
}
#endif

/////////////////////////////////////////////////
// Hash-Table
template<class TKey, class TDat, class THashFunc = TDefaultHashFunc<TKey> >
class THashGenericMP{
public:
  //pthread_mutex_t lock;
  enum {HashPrimes=32};
  static const unsigned int HashPrimeT[HashPrimes];
public:
  typedef THashKeyDatI<TKey, TDat> TIter;
private:
  typedef THashKeyDat<TKey, TDat> THKeyDat;
  typedef TPair<TKey, TDat> TKeyDatP;
  TIntV PortV;
  TIntV PortLockV;
  TVec<THKeyDat> KeyDatV;
  TBool AutoSizeP;
  TInt FFreeKeyId, FreeKeys;
private:
  class THashKeyDatCmp {
  public:
    const THash<TKey, TDat, THashFunc>& Hash;
    bool CmpKey, Asc;
    THashKeyDatCmp(THash<TKey, TDat, THashFunc>& _Hash, const bool& _CmpKey, const bool& _Asc) :
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
  THKeyDat& GetHashKeyDat(const int& KeyId){
    THKeyDat& KeyDat=KeyDatV[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  const THKeyDat& GetHashKeyDat(const int& KeyId) const {
    const THKeyDat& KeyDat=KeyDatV[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  uint GetNextPrime(const uint& Val) const;
  void Resize();
public:
  THashGenericMP():
    PortV(), KeyDatV(),
    AutoSizeP(true), FFreeKeyId(-1), FreeKeys(0){
      //lock = PTHREAD_MUTEX_INITIALIZER;
    }
  THashGenericMP(const THashGenericMP& Hash):
    PortV(Hash.PortV), KeyDatV(Hash.KeyDatV), AutoSizeP(Hash.AutoSizeP),
    FFreeKeyId(Hash.FFreeKeyId), FreeKeys(Hash.FreeKeys) {
      //lock = PTHREAD_MUTEX_INITIALIZER;
    }
  explicit THashGenericMP(const int& ExpectVals, const bool& _AutoSizeP=false);
  explicit THashGenericMP(TSIn& SIn):
    PortV(SIn), KeyDatV(SIn),
    AutoSizeP(SIn), FFreeKeyId(SIn), FreeKeys(SIn){
    SIn.LoadCs();
    //lock = PTHREAD_MUTEX_INITIALIZER;
  }
  void Load(TSIn& SIn){
    PortV.Load(SIn); KeyDatV.Load(SIn);
    AutoSizeP=TBool(SIn); FFreeKeyId=TInt(SIn); FreeKeys=TInt(SIn);
    SIn.LoadCs();
    //lock = PTHREAD_MUTEX_INITIALIZER;
  }
  void Save(TSOut& SOut) const {
    PortV.Save(SOut); KeyDatV.Save(SOut);
    AutoSizeP.Save(SOut); FFreeKeyId.Save(SOut); FreeKeys.Save(SOut);
    SOut.SaveCs();
  }
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm);

  void ResizePar(int);

  THashGenericMP& operator=(const THashGenericMP& Hash){
    if (this!=&Hash){
      PortV=Hash.PortV; KeyDatV=Hash.KeyDatV; AutoSizeP=Hash.AutoSizeP;
      FFreeKeyId=Hash.FFreeKeyId; FreeKeys=Hash.FreeKeys;}
    return *this;}
  bool operator==(const THashGenericMP& Hash) const; //J: zdaj tak kot je treba
  bool operator < (const THashGenericMP& Hash) const { Fail; return true; }
  const TDat& operator[](const int& KeyId) const {return GetHashKeyDat(KeyId).Dat;}
  TDat& operator[](const int& KeyId){return GetHashKeyDat(KeyId).Dat;}
  TDat& operator()(const TKey& Key){return AddDat(Key);}
  ::TSize GetMemUsed() const {
    // return PortV.GetMemUsed()+KeyDatV.GetMemUsed()+sizeof(bool)+2*sizeof(int);}
      int64 MemUsed = sizeof(bool)+2*sizeof(int);
      MemUsed += int64(PortV.Reserved()) * int64(sizeof(TInt));
      for (int KeyDatN = 0; KeyDatN < KeyDatV.Len(); KeyDatN++) {
          MemUsed += int64(2 * sizeof(TInt));
          MemUsed += int64(KeyDatV[KeyDatN].Key.GetMemUsed());
          MemUsed += int64(KeyDatV[KeyDatN].Dat.GetMemUsed());
      }
      return ::TSize(MemUsed);
  }

  TIter BegI() const {
    if (Len() == 0){return TIter(KeyDatV.EndI(), KeyDatV.EndI());}
    if (IsKeyIdEqKeyN()) { return TIter(KeyDatV.BegI(), KeyDatV.EndI());}
    int FKeyId=-1;  FNextKeyId(FKeyId);
    return TIter(KeyDatV.BegI()+FKeyId, KeyDatV.EndI()); }
  TIter EndI() const {return TIter(KeyDatV.EndI(), KeyDatV.EndI());}
  //TIter GetI(const int& KeyId) const {return TIter(&KeyDatV[KeyId], KeyDatV.EndI());}
  TIter GetI(const TKey& Key) const {return TIter(&KeyDatV[GetKeyId(Key)], KeyDatV.EndI());}

  void Gen(const int& ExpectVals){
    PortV.Gen(GetNextPrime(ExpectVals/2)); KeyDatV.Gen(ExpectVals, 0);
    FFreeKeyId=-1; FreeKeys=0; PortV.PutAll(TInt(-1));}

  void Clr(const bool& DoDel=true, const int& NoDelLim=-1, const bool& ResetDat=true);
  bool Empty() const {return Len()==0;}
  int Len() const {return KeyDatV.Len()-FreeKeys;}
  int GetPorts() const {return PortV.Len();}
  bool IsAutoSize() const {return AutoSizeP;}
  int GetMxKeyIds() const {return KeyDatV.Len();}
  int GetReservedKeyIds() const {return KeyDatV.Reserved();}
  bool IsKeyIdEqKeyN() const {return FreeKeys==0;}

  int AddKey(const TKey& Key);
  int AddKeyPar(const TKey& Key);
  TDat& AddDatId(const TKey& Key){
    int KeyId=AddKey(Key); return KeyDatV[KeyId].Dat=KeyId;}
  TDat& AddDat(const TKey& Key){return KeyDatV[AddKey(Key)].Dat;}
  TDat& AddDat(const TKey& Key, const TDat& Dat){
    return KeyDatV[AddKey(Key)].Dat=Dat;}

  bool AddDatIfNotExist(const TKey& Key, const TDat& Dat);

  int AddDatPar(const TKey& Key, const TInt& Dat) {
    //if ((KeyDatV.Len()>2*PortV.Len())||PortV.Empty()){
    //  Resize();
    //}
    const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
    const int HashCd=abs(THashFunc::GetSecHashCd(Key));
    int PrevKeyId=-1;
    int KeyId;
    int Ret;

    bool done = false;
    while(!done) {
      bool port_lock = false;
      int old;
      int *ptr = &PortLockV[PortN].Val;

      old = PortLockV[PortN];
      if (old == -2) {
        port_lock = false;
      }
      else if (__sync_bool_compare_and_swap(ptr, old, -2)) {
        port_lock = true;
      }

      KeyId = PortV[PortN];
      while ((KeyId!=-1) &&
          !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
        PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

      if (KeyId==-1) {
        if (port_lock == false) continue;

        volatile unsigned int *p = (volatile unsigned int *)&FFreeKeyId.Val;
        KeyId = __sync_fetch_and_add(p, 1);

        //KeyId = __sync_fetch_and_add(&FFreeKeyId.Val, 1);

        KeyDatV[KeyId].Next=-1;
        KeyDatV[KeyId].HashCd=HashCd;
        KeyDatV[KeyId].Key=Key;

        int temp;
        int* pt = &KeyDatV[KeyId].Next.Val;
        while(true) {
          temp = KeyDatV[KeyId].Next;
          if (temp == -2) continue;
          if (__sync_bool_compare_and_swap(pt, temp, -2)) {
            KeyDatV[KeyId].Dat.Val1 = 0;
            KeyDatV[KeyId].Dat.Val2.Add(Dat);
            *pt = temp;
            done = true;
	          Ret = 0;
            break;
          }
        }
        if (PrevKeyId==-1){
          PortV[PortN] = KeyId;
        } else {
          KeyDatV[PrevKeyId].Next=KeyId;
        }
        *ptr = old;
      }
      else {
        int temp, temp1;
        int* pt = &KeyDatV[KeyId].Next.Val;
        while(true) {
          temp = KeyDatV[KeyId].Next;
          temp1 = __sync_val_compare_and_swap(pt, temp, -2);
          if (temp1 == temp && temp1 != -2) {
            KeyDatV[KeyId].Dat.Val2.Add(Dat);
            *pt = temp;
            if (port_lock) *ptr = old;
            done = true;
	          Ret = KeyDatV[KeyId].Dat.Val1;
            break;
          }
          else {
            usleep(20);
          }
        }
      }
    }
    return Ret;
  }

  void DelKey(const TKey& Key);
  bool DelIfKey(const TKey& Key){
    int KeyId; if (IsKey(Key, KeyId)){DelKeyId(KeyId); return true;} return false;}
  void DelKeyId(const int& KeyId){DelKey(GetKey(KeyId));}
  void DelKeyIdV(const TIntV& KeyIdV){
    for (int KeyIdN=0; KeyIdN<KeyIdV.Len(); KeyIdN++){DelKeyId(KeyIdV[KeyIdN]);}}

  void MarkDelKey(const TKey& Key); // marks the record as deleted - doesn't delete Dat (to avoid fragmentation)
  void MarkDelKeyId(const int& KeyId){MarkDelKey(GetKey(KeyId));}

  const TKey& GetKey(const int& KeyId) const { return GetHashKeyDat(KeyId).Key;}
  int GetKeyId(const TKey& Key) const;
  /// Get an index of a random element. If the hash table has many deleted keys, this may take a long time.
  int GetRndKeyId(TRnd& Rnd) const;
  /// Get an index of a random element. If the hash table has many deleted keys, defrag the hash table first (that's why the function is non-const).
  int GetRndKeyId(TRnd& Rnd, const double& EmptyFrac);
  bool IsKey(const TKey& Key) const {return GetKeyId(Key)!=-1;}
  bool IsKey(const TKey& Key, int& KeyId) const { KeyId=GetKeyId(Key); return KeyId!=-1;}
  bool IsKeyId(const int& KeyId) const {
    return (0<=KeyId)&&(KeyId<KeyDatV.Len())&&(KeyDatV[KeyId].HashCd!=-1);}
  const TDat& GetDat(const TKey& Key) const {return KeyDatV[GetKeyId(Key)].Dat;}
  TDat& GetDat(const TKey& Key){return KeyDatV[GetKeyId(Key)].Dat;}
//  TKeyDatP GetKeyDat(const int& KeyId) const {
//    TKeyDat& KeyDat=GetHashKeyDat(KeyId);
//    return TKeyDatP(KeyDat.Key, KeyDat.Dat);}
  void GetKeyDat(const int& KeyId, TKey& Key, TDat& Dat) const {
    const THKeyDat& KeyDat=GetHashKeyDat(KeyId);
    Key=KeyDat.Key; Dat=KeyDat.Dat;}
  bool IsKeyGetDat(const TKey& Key, TDat& Dat) const {int KeyId;
    if (IsKey(Key, KeyId)){Dat=GetHashKeyDat(KeyId).Dat; return true;}
    else {return false;}}

  int FFirstKeyId() const {return 0-1;}
  bool FNextKeyId(int& KeyId) const;
  void GetKeyV(TVec<TKey>& KeyV) const;
  void GetDatV(TVec<TDat>& DatV) const;
  void GetKeyDatPrV(TVec<TPair<TKey, TDat> >& KeyDatPrV) const;
  void GetDatKeyPrV(TVec<TPair<TDat, TKey> >& DatKeyPrV) const;
  void GetKeyDatKdV(TVec<TKeyDat<TKey, TDat> >& KeyDatKdV) const;
  void GetDatKeyKdV(TVec<TKeyDat<TDat, TKey> >& DatKeyKdV) const;

  void Swap(THashGenericMP& Hash);
  void Defrag();
  void Pack(){KeyDatV.Pack();}
  void Sort(const bool& CmpKey, const bool& Asc);
  void SortByKey(const bool& Asc=true) { Sort(true, Asc); }
  void SortByDat(const bool& Asc=true) { Sort(false, Asc); }
};

template<class TKey, class TDat, class THashFunc>
const unsigned int THashGenericMP<TKey, TDat, THashFunc>::HashPrimeT[HashPrimes]={
  3ul, 5ul, 11ul, 23ul,
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

template<class TKey, class TDat, class THashFunc>
uint THashGenericMP<TKey, TDat, THashFunc>::GetNextPrime(const uint& Val) const {
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
void THashGenericMP<TKey, TDat, THashFunc>::ResizePar(int sz){
  if (PortV.Len()==0){
    PortV.Gen(sz);
    KeyDatV.Gen(sz);
    PortLockV.Gen(sz);
  } else if (AutoSizeP&&(KeyDatV.Len()>2*PortV.Len())){
    PortV.Gen(GetNextPrime(PortV.Len()+1));
  } else {
    return;
  }
  PortV.PutAll(TInt(-1));
  // rehash keys
  for (int KeyId=0; KeyId<KeyDatV.Len(); KeyId++){
    THKeyDat& KeyDat=KeyDatV[KeyId];
    if (KeyDat.HashCd!=-1){
      const int PortN = abs(THashFunc::GetPrimHashCd(KeyDat.Key) % PortV.Len());
      KeyDat.Next=PortV[PortN];
      PortV[PortN]=KeyId;
    }
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::Resize(){
  // resize & initialize port vector
  //if (PortV.Len()==0){PortV.Gen(17);}
  //else {PortV.Gen(2*PortV.Len()+1);}
  if (PortV.Len()==0){
    PortV.Gen(17);
  } else if (AutoSizeP&&(KeyDatV.Len()>2*PortV.Len())){
    PortV.Gen(GetNextPrime(PortV.Len()+1));
  } else {
    return;
  }
  PortV.PutAll(TInt(-1));
  // rehash keys
  for (int KeyId=0; KeyId<KeyDatV.Len(); KeyId++){
    THKeyDat& KeyDat=KeyDatV[KeyId];
    if (KeyDat.HashCd!=-1){
      const int PortN = abs(THashFunc::GetPrimHashCd(KeyDat.Key) % PortV.Len());
      KeyDat.Next=PortV[PortN];
      PortV[PortN]=KeyId;
    }
  }
}

template<class TKey, class TDat, class THashFunc>
THashGenericMP<TKey, TDat, THashFunc>::THashGenericMP(const int& ExpectVals, const bool& _AutoSizeP):
  PortV(GetNextPrime(ExpectVals/2)), KeyDatV(ExpectVals, 0),
  AutoSizeP(_AutoSizeP), FFreeKeyId(-1), FreeKeys(0){
  PortV.PutAll(TInt(-1));
}

template<class TKey, class TDat, class THashFunc>
bool THashGenericMP<TKey, TDat, THashFunc>::operator==(const THashGenericMP& Hash) const {
  if (Len() != Hash.Len()) { return false; }
  for (int i = FFirstKeyId(); FNextKeyId(i); ) {
    const TKey& Key = GetKey(i);
    if (! Hash.IsKey(Key)) { return false; }
    if (GetDat(Key) != Hash.GetDat(Key)) { return false; }
  }
  return true;
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::Clr(const bool& DoDel, const int& NoDelLim, const bool& ResetDat){
  if (DoDel){
    PortV.Clr(); KeyDatV.Clr();
  } else {
    PortV.PutAll(TInt(-1));
    KeyDatV.Clr(DoDel, NoDelLim);
    if (ResetDat){KeyDatV.PutAll(THKeyDat());}
  }
  FFreeKeyId=TInt(-1); FreeKeys=TInt(0);
}

template<class TKey, class TDat, class THashFunc>
int THashGenericMP<TKey, TDat, THashFunc>::AddKey(const TKey& Key){
  //if ((KeyDatV.Len()>2*PortV.Len())||PortV.Empty()){Resize();}
  const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId=PortV[PortN];
  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

  if (KeyId==-1){
    if (FFreeKeyId==-1){
      KeyId=KeyDatV.Add(THKeyDat(-1, HashCd, Key));
    } else {
      KeyId=FFreeKeyId; FFreeKeyId=KeyDatV[FFreeKeyId].Next; FreeKeys--;
      //KeyDatV[KeyId]=TKeyDat(-1, HashCd, Key); // slow version
      KeyDatV[KeyId].Next=-1;
      KeyDatV[KeyId].HashCd=HashCd;
      KeyDatV[KeyId].Key=Key;
      //KeyDatV[KeyId].Dat=TDat(); // already empty
    }
    if (PrevKeyId==-1){
      PortV[PortN]=KeyId;
    } else {
      KeyDatV[PrevKeyId].Next=KeyId;
    }
  }
  return KeyId;
}

template<class TKey, class TDat, class THashFunc>
int THashGenericMP<TKey, TDat, THashFunc>::AddKeyPar(const TKey& Key){
    //if ((KeyDatV.Len()>2*PortV.Len())||PortV.Empty()){
    //  Resize();
    //}
    const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
    const int HashCd=abs(THashFunc::GetSecHashCd(Key));
    int PrevKeyId=-1;
    int KeyId;

    bool done = false;
    while(!done) {
      bool port_lock = false;
      int old;
      int *ptr = &PortLockV[PortN].Val;

      old = PortLockV[PortN];
      if (old == -2) {
        port_lock = false;
      }
      else if (__sync_bool_compare_and_swap(ptr, old, -2)) {
        port_lock = true;
      }

      KeyId = PortV[PortN];
      while ((KeyId!=-1) &&
          !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
        PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

      if (KeyId==-1) {
        if (port_lock == false) continue;

        volatile unsigned int *p = (volatile unsigned int *)&FFreeKeyId.Val;
        KeyId = __sync_fetch_and_add(p, 1);

        //KeyId = __sync_fetch_and_add(&FFreeKeyId.Val, 1);

        KeyDatV[KeyId].Next=-1;
        KeyDatV[KeyId].HashCd=HashCd;
        KeyDatV[KeyId].Key=Key;

        if (PrevKeyId==-1){
          PortV[PortN] = KeyId;
        } else {
          KeyDatV[PrevKeyId].Next=KeyId;
        }

        *ptr = old;
        done = true;
      }
      else {
        done = true;
      }
    }
    return KeyId;
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::DelKey(const TKey& Key){
  IAssert(!PortV.Empty());
  const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId=PortV[PortN];

  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

  //IAssertR(KeyId!=-1, Key.GetStr()); //J: some classes do not provide GetStr()?
  IAssert(KeyId!=-1); //J: some classes do not provide GetStr()?
  if (PrevKeyId==-1){PortV[PortN]=KeyDatV[KeyId].Next;}
  else {KeyDatV[PrevKeyId].Next=KeyDatV[KeyId].Next;}
  KeyDatV[KeyId].Next=FFreeKeyId; FFreeKeyId=KeyId; FreeKeys++;
  KeyDatV[KeyId].HashCd=TInt(-1);
  KeyDatV[KeyId].Key=TKey();
  KeyDatV[KeyId].Dat=TDat();
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::MarkDelKey(const TKey& Key){
  // MarkDelKey is same as Delkey except last two lines
  IAssert(!PortV.Empty());
  const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId=PortV[PortN];
  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}
  IAssertR(KeyId!=-1, Key.GetStr());
  if (PrevKeyId==-1){PortV[PortN]=KeyDatV[KeyId].Next;}
  else {KeyDatV[PrevKeyId].Next=KeyDatV[KeyId].Next;}
  KeyDatV[KeyId].Next=FFreeKeyId; FFreeKeyId=KeyId; FreeKeys++;
  KeyDatV[KeyId].HashCd=TInt(-1);
}

template<class TKey, class TDat, class THashFunc>
int THashGenericMP<TKey, TDat, THashFunc>::GetRndKeyId(TRnd& Rnd) const  {
  IAssert(! Empty());
  int KeyId = abs(Rnd.GetUniDevInt(KeyDatV.Len()));
  while (KeyDatV[KeyId].HashCd == -1) { // if the index is empty, just try again
    KeyId = abs(Rnd.GetUniDevInt(KeyDatV.Len())); }
  return KeyId; 
}

// return random KeyId even if the hash table contains deleted keys
// defrags the table if necessary
template<class TKey, class TDat, class THashFunc>
int THashGenericMP<TKey, TDat, THashFunc>::GetRndKeyId(TRnd& Rnd, const double& EmptyFrac) {
  IAssert(! Empty());
  if (FreeKeys/double(Len()+FreeKeys) > EmptyFrac) { Defrag(); }
  int KeyId = Rnd.GetUniDevInt(KeyDatV.Len());
  while (KeyDatV[KeyId].HashCd == -1) { // if the index is empty, just try again
    KeyId = Rnd.GetUniDevInt(KeyDatV.Len());
  }
  return KeyId;
}

template<class TKey, class TDat, class THashFunc>
int THashGenericMP<TKey, TDat, THashFunc>::GetKeyId(const TKey& Key) const {
  if (PortV.Empty()){return -1;}
  const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int KeyId=PortV[PortN];
  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    KeyId=KeyDatV[KeyId].Next;}
  return KeyId;
}

template<class TKey, class TDat, class THashFunc>
bool THashGenericMP<TKey, TDat, THashFunc>::FNextKeyId(int& KeyId) const {
  do {KeyId++;} while ((KeyId<KeyDatV.Len())&&(KeyDatV[KeyId].HashCd==-1));
  return KeyId<KeyDatV.Len();
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::GetKeyV(TVec<TKey>& KeyV) const {
  KeyV.Gen(Len(), 0);
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    KeyV.Add(GetKey(KeyId));}
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::GetDatV(TVec<TDat>& DatV) const {
  DatV.Gen(Len(), 0);
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    DatV.Add(GetHashKeyDat(KeyId).Dat);}
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::GetKeyDatPrV(TVec<TPair<TKey, TDat> >& KeyDatPrV) const {
  KeyDatPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatPrV.Add(TPair<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::GetDatKeyPrV(TVec<TPair<TDat, TKey> >& DatKeyPrV) const {
  DatKeyPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyPrV.Add(TPair<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::GetKeyDatKdV(TVec<TKeyDat<TKey, TDat> >& KeyDatKdV) const {
  KeyDatKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatKdV.Add(TKeyDat<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::GetDatKeyKdV(TVec<TKeyDat<TDat, TKey> >& DatKeyKdV) const {
  DatKeyKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  int KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyKdV.Add(TKeyDat<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::Swap(THashGenericMP& Hash) {
  if (this!=&Hash){
    PortV.Swap(Hash.PortV);
    KeyDatV.Swap(Hash.KeyDatV);
    ::Swap(AutoSizeP, Hash.AutoSizeP);
    ::Swap(FFreeKeyId, Hash.FFreeKeyId);
    ::Swap(FreeKeys, Hash.FreeKeys);
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::Defrag(){
  if (!IsKeyIdEqKeyN()){
    THashGenericMP<TKey, TDat, THashFunc> Hash(PortV.Len());
    int KeyId=FFirstKeyId(); TKey Key; TDat Dat;
    while (FNextKeyId(KeyId)){
      GetKeyDat(KeyId, Key, Dat);
      Hash.AddDat(Key, Dat);
    }
    Pack();
    operator=(Hash);
    IAssert(IsKeyIdEqKeyN());
  }
}

template<class TKey, class TDat, class THashFunc>
void THashGenericMP<TKey, TDat, THashFunc>::Sort(const bool& CmpKey, const bool& Asc) {
  IAssertR(IsKeyIdEqKeyN(), "THash::Sort only works when table has no deleted keys.");
  TIntV TargV(Len()), MapV(Len()), StateV(Len());
  for (int i = 0; i < TargV.Len(); i++) {
    TargV[i] = i; MapV[i] = i; StateV[i] = i;
  }
  // sort KeyIds
  THashKeyDatCmp HashCmp(*this, CmpKey, Asc);
  TargV.SortCmp(HashCmp);
  // now sort the update vector
  THashKeyDat<TKey, TDat> Tmp;
  for (int i = 0; i < TargV.Len()-1; i++) {
    const int SrcPos = MapV[TargV[i]];
    const int Loc = i;
    // swap data
    Tmp = KeyDatV[SrcPos];
    KeyDatV[SrcPos] = KeyDatV[Loc];
    KeyDatV[Loc] = Tmp;
    // swap keys
    MapV[StateV[i]] = SrcPos;
    StateV.Swap(Loc, SrcPos);
  }
  for (int i = 0; i < TargV.Len(); i++) {
    MapV[TargV[i]] = i; }
  for (int p = 0; p < PortV.Len(); p++) {
    if (PortV[p] != -1) {
      PortV[p] = MapV[PortV[p]]; } }
  for (int i = 0; i < KeyDatV.Len(); i++) {
    if (KeyDatV[i].Next != -1) {
      KeyDatV[i].Next = MapV[KeyDatV[i].Next]; }
  }
}

template<class TKey, class TDat, class THashFunc>
bool THashGenericMP<TKey, TDat, THashFunc>::AddDatIfNotExist(const TKey& Key, const TDat& Dat) {
  const int PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const int HashCd=abs(THashFunc::GetSecHashCd(Key));
  int PrevKeyId=-1;
  int KeyId;
  bool done = false;
  while(!done) {
    bool port_lock = false;
    int old;
    int *ptr = &PortLockV[PortN].Val;

    old = PortLockV[PortN];
    if (old == -2) {
      port_lock = false;
    }
    else if (__sync_bool_compare_and_swap(ptr, old, -2)) {
      port_lock = true;
    }

    KeyId = PortV[PortN];
    while ((KeyId!=-1) &&
        !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
      PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

    if (KeyId==-1) {
      if (port_lock == false) continue;

      volatile unsigned int *p = (volatile unsigned int *)&FFreeKeyId.Val;
      KeyId = __sync_fetch_and_add(p, 1);

      //KeyId = __sync_fetch_and_add(&FFreeKeyId.Val, 1);

      KeyDatV[KeyId].Next=-1;
      KeyDatV[KeyId].HashCd=HashCd;
      KeyDatV[KeyId].Key=Key;

      int temp;
      int* pt = &KeyDatV[KeyId].Next.Val;
      while(true) {
        temp = KeyDatV[KeyId].Next;
        if (temp == -2) continue;
        if (__sync_bool_compare_and_swap(pt, temp, -2)) {
          KeyDatV[KeyId].Dat = Dat;
          *pt = temp;
          done = true;
          break;
        }
      }
      if (PrevKeyId==-1){
        PortV[PortN] = KeyId;
      } else {
        KeyDatV[PrevKeyId].Next=KeyId;
      }
      *ptr = old;
    }
    else {
      if (port_lock) *ptr = old;
      break;
    }
  }
  return done;
}
