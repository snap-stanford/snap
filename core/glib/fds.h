/////////////////////////////////////////////////
// Forward
template <class TKey, class TFDat, class TVDat>
class TFHash;

typedef enum {fhbtUndef, fhbtKey, fhbtVDat} TFHashBlobType;

/////////////////////////////////////////////////
// File-Hash-Table-Key-Data
template <class TKey, class TFDat, class TVDat>
class TFHashKey{
private:
  TCRef CRef;
  bool Modified;
public:
  typedef TPt<TFHashKey<TKey, TFDat, TVDat> > PFHashKey;
  typedef TFHash<TKey, TFDat, TVDat> THash;
  TBlobPt Next;
  TKey Key;
  TFDat FDat;
  TBlobPt VDatBPt;
public:
  TFHashKey():
    Modified(false), Next(), Key(), FDat(), VDatBPt(){}
  TFHashKey(const TBlobPt& _Next,
   const TKey& _Key, const TFDat& _FDat, const TBlobPt& _VDatBPt=TBlobPt()):
    Modified(false), Next(_Next), Key(_Key), FDat(_FDat), VDatBPt(_VDatBPt){}
  ~TFHashKey(){}
  TFHashKey(TSIn& SIn):
    Modified(false), Next(SIn), Key(SIn), FDat(SIn), VDatBPt(SIn){}
  static PFHashKey Load(TSIn& SIn){return new TFHashKey(SIn);}
  void Save(TSOut& SOut){
    Next.Save(SOut); Key.Save(SOut); FDat.Save(SOut); VDatBPt.Save(SOut);}

  TFHashKey& operator=(const TFHashKey& FHashKey){
    if (this!=&FHashKey){
      Modified=true; Next=FHashKey.Next;
      Key=FHashKey.Key; FDat=FHashKey.FDat; VDatBPt=FHashKey.VDatBPt;}
    return *this;}
  int GetMemUsed() const {
    return sizeof(THash*)+Next.GetMemUsed()+
     Key.GetMemUsed()+FDat.GetMemUsed()+VDatBPt.GetMemUsed();}

  void PutModified(const bool& _Modified){Modified=_Modified;}

  void OnDelFromCache(const TBlobPt& BlobPt, void* RefToBs);

  friend class TPt<TFHashKey<TKey, TFDat, TVDat> >;
};

template <class TKey, class TFDat, class TVDat>
void TFHashKey<TKey, TFDat, TVDat>::OnDelFromCache(
 const TBlobPt& BlobPt, void* RefToBs){
  if (Modified){
    // prepare hash table object
    THash* FHash=(THash*)RefToBs;
    // save the key
    TMOut MOut; TInt(int(fhbtKey)).Save(MOut); Save(MOut);
    TBlobPt NewBlobPt=FHash->GetHashBBs()->PutBlob(BlobPt, MOut.GetSIn());
    // blob-pointer for key should not change
    IAssert(NewBlobPt==BlobPt);
  }
}

/////////////////////////////////////////////////
// File-Hash-Table

template <class TKey, class TFDat, class TVDat>
class TFHash{
private:
  TCRef CRef;
private:
  typedef TPt<TFHash<TKey, TFDat, TVDat> > PFHash;
  typedef TFHashKey<TKey, TFDat, TVDat> THashKey;
  typedef TPt<THashKey> PHashKey;
  TFAccess Access;
  PBlobBs HashBBs;
  TBlobPtV PortV;
  TInt Keys;
  TCache<TBlobPt, PHashKey> FHashKeyCache;
private:
  void* GetVoidThis() const {return (void*)this;}
  PBlobBs GetHashBBs(){return HashBBs;}
  PHashKey GetFHashKey(const TBlobPt& KeyId){
    PHashKey FHashKey;
    if (!FHashKeyCache.Get(KeyId, FHashKey)){ // if the key is in cache
      // read the key from blob-base
      PSIn SIn=HashBBs->GetBlob(KeyId);
      TFHashBlobType Type=TFHashBlobType(int(TInt(*SIn))); IAssert(Type==fhbtKey);
      FHashKey=PHashKey(new THashKey(*SIn));
    }
    FHashKeyCache.Put(KeyId, FHashKey); // refresh/put key in cache
    return FHashKey;
  }
  void GetKeyInfo(const TKey& Key,
   int& PortN, TBlobPt& PrevKeyId, TBlobPt& KeyId, PHashKey& FHashKey);
  void GetKeyInfo(const TKey& Key, TBlobPt& KeyId, PHashKey& FHashKey){
    int PortN=-1; TBlobPt PrevKeyId;
    GetKeyInfo(Key, PortN, PrevKeyId, KeyId, FHashKey);}
private:
  TBlobPt AddKey(const TKey& Key,
   const bool& ChangeFDat, const TFDat& FDat,
   const bool& ChangeVDatBPt, const TBlobPt& VDatBPt);
  TBlobPt AddDat(
   const TKey& Key,
   const bool& ChangeFDat, const TFDat& FDat,
   const bool& ChangeVDat, const TVDat& VDat);
public:
  TFHash(const TStr& HashFNm, const TFAccess& _Access,
   const int& Ports, const int& MxMemUsed);
  ~TFHash();
  TFHash(TSIn&){Fail;}
  static PFHash Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TFHash& operator=(const TFHash&){Fail; return *this;}
  int GetMemUsed(){
    return PortV.GetMemUsed()+(int)FHashKeyCache.GetMemUsed();} //TODO:64bit
  void CacheFlushAndClr(){FHashKeyCache.FlushAndClr();}

  bool Empty() const {return Keys==0;}
  int Len() const {return Keys;}

  TBlobPt AddFDat(const TKey& Key, const TFDat& FDat){
    return AddKey(Key, true, FDat, false, TBlobPt());}
  TBlobPt AddVDat(const TKey& Key, const TVDat& VDat){
    return AddDat(Key, false, TFDat(), true, VDat);}
  TBlobPt AddFVDat(const TKey& Key, const TFDat& FDat, const TVDat& VDat){
    return AddDat(Key, true, FDat, true, VDat);}

  void DelKey(const TKey& Key);
  void DelKeyId(const TBlobPt& KeyId){
    TKey Key; GetKey(KeyId, Key); DelKey(Key);}

  void GetKey(const TBlobPt& KeyId, TKey& Key){
    PHashKey FHashKey=GetFHashKey(KeyId); Key=FHashKey->Key;}
  TBlobPt GetKeyId(const TKey& Key){
    TBlobPt KeyId; PHashKey FHashKey; GetKeyInfo(Key, KeyId, FHashKey);
    return KeyId;}
  bool IsKey(const TKey& Key){
    return !GetKeyId(Key).Empty();}
  bool IsKey(const TKey& Key, TBlobPt& KeyId){
    KeyId=GetKeyId(Key); return !KeyId.Empty();}

  TBlobPt GetFDat(const TKey& Key, TFDat& FDat);
  TBlobPt GetVDat(const TKey& Key, TVDat& VDat);
  TBlobPt GetFVDat(const TKey& Key, TFDat& FDat, TVDat& VDat);
  void GetKeyFDat(const TBlobPt& KeyId, TKey& Key, TFDat& FDat);
  void GetKeyFVDat(const TBlobPt& KeyId, TKey& Key, TFDat& FDat, TVDat& VDat);

  TBlobPt FFirstKeyId();
  bool FNextKeyId(TBlobPt& TrvBlobPt, TBlobPt& KeyId);

  friend class TFHashKey<TKey, TFDat, TVDat>;
  friend class TPt<TFHash<TKey, TFDat, TVDat> >;
};


template <class TKey, class TFDat, class TVDat>
void TFHash<TKey, TFDat, TVDat>::GetKeyInfo(
 const TKey& Key,
 int& PortN, TBlobPt& PrevKeyId, TBlobPt& KeyId, PHashKey& FHashKey){
  // prepare key data
  PortN=abs(Key.GetPrimHashCd())%PortV.Len();
  PrevKeyId.Clr();
  KeyId=PortV[PortN];

  // test if the key exists
  if (!KeyId.Empty()){
    FHashKey=GetFHashKey(KeyId);
    while ((!KeyId.Empty())&&(FHashKey->Key!=Key)){
      PrevKeyId=KeyId;
      KeyId=FHashKey->Next;
      if (!KeyId.Empty()){FHashKey=GetFHashKey(KeyId);}
    }
  }
}

template <class TKey, class TFDat, class TVDat>
TFHash<TKey, TFDat, TVDat>::TFHash(
 const TStr& HashFNm, const TFAccess& _Access,
 const int& Ports, const int& MxMemUsed):
  Access(_Access), HashBBs(), PortV(), Keys(0),
  FHashKeyCache(MxMemUsed, 100003, GetVoidThis()){
  if (Access==faCreate){
    IAssert(Ports>0);
    // create blob-base
    HashBBs=PBlobBs(new TGBlobBs(HashFNm, faCreate));
    // save initial no. of keys and port-vector
    PortV.Gen(Ports);
    TMOut HdSOut; Keys.Save(HdSOut); PortV.Save(HdSOut);
    HashBBs->PutBlob(HdSOut.GetSIn());
  } else {
    IAssert((Access==faUpdate)||(Access==faRdOnly));
    IAssert(Ports==-1);
    // open blob-base
    HashBBs=PBlobBs(new TGBlobBs(HashFNm, Access));
    // load initial no. of keys and port-vector
    TBlobPt HdBPt=HashBBs->GetFirstBlobPt();
    PSIn HdSIn=HashBBs->GetBlob(HdBPt);
    Keys=TInt(*HdSIn);
    PortV=TBlobPtV(*HdSIn);
  }
}

template <class TKey, class TFDat, class TVDat>
TFHash<TKey, TFDat, TVDat>::~TFHash(){
  if ((Access==faCreate)||(Access==faUpdate)){
    // flush hash-key cache
    FHashKeyCache.Flush();
    // save port-vector
    TBlobPt HdBPt=HashBBs->GetFirstBlobPt();
    TMOut HdSOut; Keys.Save(HdSOut); PortV.Save(HdSOut);
    HashBBs->PutBlob(HdBPt, HdSOut.GetSIn());
  }
}

template <class TKey, class TFDat, class TVDat>
TBlobPt TFHash<TKey, TFDat, TVDat>::AddKey(
 const TKey& Key,
 const bool& ChangeFDat, const TFDat& FDat,
 const bool& ChangeVDatBPt, const TBlobPt& VDatBPt){
  // prepare key info
  int PortN=-1; TBlobPt PrevKeyId; TBlobPt KeyId; PHashKey FHashKey;
  GetKeyInfo(Key, PortN, PrevKeyId, KeyId, FHashKey);

  if (KeyId.Empty()){
    // generate key
    FHashKey=PHashKey(new THashKey(TBlobPt(), Key, FDat, VDatBPt));
    // save key to blob-base
    TMOut FHashKeyMOut;
    TInt(int(fhbtKey)).Save(FHashKeyMOut); FHashKey->Save(FHashKeyMOut);
    TBlobPt FHashKeyBPt=HashBBs->PutBlob(FHashKeyMOut.GetSIn());
    // save key to key-cache
    FHashKeyCache.Put(FHashKeyBPt, FHashKey);
    FHashKey->PutModified(false);
    // connect key to the structure
    KeyId=FHashKeyBPt;
    Keys++;
    if (PrevKeyId.Empty()){
      PortV[PortN]=KeyId;
    } else {
      PHashKey PrevFHashKey=GetFHashKey(PrevKeyId);
      PrevFHashKey->Next=KeyId;
      PrevFHashKey->PutModified(true);
    }
  } else {
    // update the data
    if (ChangeFDat){FHashKey->FDat=FDat;}
    if (ChangeVDatBPt){FHashKey->VDatBPt=VDatBPt;}
    if (ChangeFDat||ChangeVDatBPt){
      FHashKey->PutModified(true);}
  }
  return KeyId;
}

template <class TKey, class TFDat, class TVDat>
TBlobPt TFHash<TKey, TFDat, TVDat>::AddDat(
 const TKey& Key,
 const bool& ChangeFDat, const TFDat& FDat,
 const bool& ChangeVDat, const TVDat& VDat){
  // prepare key info
  TBlobPt KeyId; PHashKey FHashKey;
  GetKeyInfo(Key, KeyId, FHashKey);

  // prepare new variable-data blob-pointer
  TBlobPt VDatBPt;
  if (ChangeVDat){
    // save variable-data
    TMOut VDatMOut;
    TInt(int(fhbtVDat)).Save(VDatMOut); VDat.Save(VDatMOut);
    if (KeyId.Empty()){
      VDatBPt=HashBBs->PutBlob(VDatMOut.GetSIn());
    } else {
      VDatBPt=HashBBs->PutBlob(FHashKey->VDatBPt, VDatMOut.GetSIn());
    }
  }

  // save the data
  KeyId=AddKey(Key, ChangeFDat, FDat, ChangeVDat, VDatBPt);
  return KeyId;
}

template <class TKey, class TFDat, class TVDat>
void TFHash<TKey, TFDat, TVDat>::DelKey(const TKey& Key){
  // prepare key info
  int PortN=-1; TBlobPt PrevKeyId; TBlobPt KeyId; PHashKey FHashKey;
  GetKeyInfo(Key, PortN, PrevKeyId, KeyId, FHashKey);

  // disconnect key
  IAssert(!KeyId.Empty());
  if (PrevKeyId.Empty()){
    PortV[PortN]=FHashKey->Next;
  } else {
    PHashKey PrevFHashKey=GetFHashKey(PrevKeyId);
    PrevFHashKey->Next=FHashKey->Next;
    PrevFHashKey->PutModified(true);
  }
  // delete variable data
  if (!FHashKey->VDatBPt.Empty()){
    HashBBs->DelBlob(FHashKey->VDatBPt);}
  // delete key/fixed data
  HashBBs->DelBlob(KeyId);
  FHashKeyCache.Del(KeyId, false);
}

template <class TKey, class TFDat, class TVDat>
TBlobPt TFHash<TKey, TFDat, TVDat>::GetFDat(
 const TKey& Key, TFDat& FDat){
  // prepare key info
  TBlobPt KeyId; PHashKey FHashKey;
  GetKeyInfo(Key, KeyId, FHashKey);
  // get fixed data
  FDat=FHashKey->FDat;
  return KeyId;
}

template <class TKey, class TFDat, class TVDat>
TBlobPt TFHash<TKey, TFDat, TVDat>::GetVDat(const TKey& Key, TVDat& VDat){
  // prepare key info
  TBlobPt KeyId; PHashKey FHashKey;
  GetKeyInfo(Key, KeyId, FHashKey);
  // get variable data
  PSIn SIn=HashBBs->GetBlob(FHashKey->VDatBPt);
  TFHashBlobType Type=TFHashBlobType(int(TInt(*SIn))); IAssert(Type==fhbtVDat);
  VDat=TVDat(*SIn);
  return KeyId;
}

template <class TKey, class TFDat, class TVDat>
TBlobPt TFHash<TKey, TFDat, TVDat>::GetFVDat(
 const TKey& Key, TFDat& FDat, TVDat& VDat){
  // prepare key info
  TBlobPt KeyId; PHashKey FHashKey;
  GetKeyInfo(Key, KeyId, FHashKey);
  // get fixed data
  FDat=FHashKey->FDat;
  // get variable data
  PSIn SIn=HashBBs->GetBlob(FHashKey->VDatBPt);
  TFHashBlobType Type=TFHashBlobType(int(TInt(*SIn))); IAssert(Type==fhbtVDat);
  VDat=TVDat(*SIn);
  return KeyId;
}

template <class TKey, class TFDat, class TVDat>
void TFHash<TKey, TFDat, TVDat>::GetKeyFDat(
 const TBlobPt& KeyId, TKey& Key, TFDat& FDat){
  // prepare key info
  PHashKey FHashKey=GetFHashKey(KeyId);
  // get key
  Key=FHashKey->Key;
  // get fixed data
  FDat=FHashKey->FDat;
}

template <class TKey, class TFDat, class TVDat>
void TFHash<TKey, TFDat, TVDat>::GetKeyFVDat(
 const TBlobPt& KeyId, TKey& Key, TFDat& FDat, TVDat& VDat){
  // prepare key info
  PHashKey FHashKey=GetFHashKey(KeyId);
  // get key
  Key=FHashKey->Key;
  // get fixed data
  FDat=FHashKey->FDat;
  // get variable data
  PSIn SIn=HashBBs->GetBlob(FHashKey->VDatBPt);
  TFHashBlobType Type=TFHashBlobType(int(TInt(*SIn))); IAssert(Type==fhbtVDat);
  VDat=TVDat(*SIn);
}

template <class TKey, class TFDat, class TVDat>
TBlobPt TFHash<TKey, TFDat, TVDat>::FFirstKeyId(){
  return HashBBs->FFirstBlobPt();
}

template <class TKey, class TFDat, class TVDat>
bool TFHash<TKey, TFDat, TVDat>::FNextKeyId(
 TBlobPt& TrvBlobPt, TBlobPt& KeyId){
  PSIn SIn;
  while (HashBBs->FNextBlobPt(TrvBlobPt, KeyId, SIn)){
    TFHashBlobType Type=TFHashBlobType(int(TInt(*SIn)));
    if (Type==fhbtKey){return true;}
  }
  return false;
}

