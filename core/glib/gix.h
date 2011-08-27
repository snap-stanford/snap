#ifndef GIX_H
#define GIX_H

// this file depends only on base.h

/////////////////////////////////////////////////
// Forward-declarations
template <class TKey, class TItem> class TGix;

/////////////////////////////////////////////////
// General-Inverted-Index Item-Set
template <class TKey, class TItem>
class TGixItemSet {
private:
    TCRef CRef;
    typedef TPt<TGixItemSet<TKey, TItem> > PGixItemSet;
private:
    TKey ItemSetKey;
    TVec<TItem> ItemV;
    TBool SortedP;
public:
    TGixItemSet(const TKey& _ItemSetKey): ItemSetKey(_ItemSetKey), SortedP(true) { }
    static PGixItemSet New(const TKey& ItemSetKey) { return new TGixItemSet(ItemSetKey); }

    TGixItemSet(TSIn& SIn) { ItemSetKey = TKey(SIn); ItemV.Load(SIn); Sort(); SortedP = true; }
    static PGixItemSet Load(TSIn& SIn) { return new TGixItemSet(SIn); }
    void Save(TSOut& SOut) const { ItemSetKey.Save(SOut); ItemV.Save(SOut); }

    // functiones used by TCache
    int GetMemUsed() const {
        return ItemSetKey.GetMemUsed() + ItemV.GetMemUsed() + sizeof(TBool); }
    void OnDelFromCache(const TBlobPt& BlobPt, void* Gix);

    // key & items
    const TKey& GetKey() const { return ItemSetKey; }
    int AddItem(const TItem& Item);
    int GetItems() const { return ItemV.Len(); }
    const TItem& GetItem(const int& ItemN) const { return ItemV[ItemN]; }
    void GetItemV(TVec<TItem>& _ItemV, const bool& SortP);
    const bool IsSorted() const { return SortedP; }
    void Sort() { ItemV.Sort(); SortedP = true;}
    int Clr();

    friend class TPt<TGixItemSet>;
};

template <class TKey, class TItem>
void TGixItemSet<TKey, TItem>::OnDelFromCache(const TBlobPt& BlobPt, void* Gix) {
    if (!((TGix<TKey, TItem>*)Gix)->IsReadOnly()) {
        ((TGix<TKey, TItem>*)Gix)->StoreItemSet(BlobPt);
    }
}

template <class TKey, class TItem>
int TGixItemSet<TKey, TItem>::AddItem(const TItem& Item) {
    const int OldSize = ItemV.GetMemUsed();
    ItemV.Add(Item); SortedP = false;
    return ItemV.GetMemUsed() - OldSize;
}

template <class TKey, class TItem>
void TGixItemSet<TKey, TItem>::GetItemV(TVec<TItem>& _ItemV, const bool& SortP) {
    if (SortP && !IsSorted()) {
        Sort();
    }
    _ItemV = ItemV;
}

template <class TKey, class TItem>
int TGixItemSet<TKey, TItem>::Clr() {
    const int OldSize = ItemV.GetMemUsed();
    ItemV.Clr(); SortedP = true;
    return ItemV.GetMemUsed() - OldSize;
}

/////////////////////////////////////////////////
// General-Inverted-Index
template <class TKey, class TItem>
class TGix {
private:
    TCRef CRef;
    typedef TPt<TGix<TKey, TItem> > PGix;
    typedef TPt<TGixItemSet<TKey, TItem> > PGixItemSet;
private:
    TFAccess Access;
    TStr GixFNm;
    TStr GixBlobFNm;
    THash<TKey, TBlobPt> KeyIdH;
    TCache<TBlobPt, PGixItemSet> ItemSetCache;
    PBlobBs ItemSetBlobBs;

    int64 CacheResetThreshold;
    int64 NewCacheSizeInc;

    // returnes pointer to this object (used in cache call-backs)
    void* GetVoidThis() const { return (void*)this; }
    // asserts if we are allowed to change this index
    void AssertReadOnly() const {
        EAssertR(((Access==faCreate)||(Access==faUpdate)),
            "Index opened in Read-Only mode!"); }
    // get keyid of a given key and create it if does not exist
    TBlobPt GetKeyId(const TKey& Key, const bool& CreateP = false);

public:
    TGix(const TStr& Nm, const TStr& FPath = TStr(),
        const TFAccess& _Access=faRdOnly, const int64& CacheSize=100000000);
    static PGix New(const TStr& Nm, const TStr& FPath = TStr(),
        const TFAccess& Access=faRdOnly, const int64& CacheSize=100000000) {
            return new TGix(Nm, FPath, Access, CacheSize); }
    ~TGix();

    // Gix properties
    bool IsReadOnly() const { return Access == faRdOnly; }

    // number of keys in the index
    int GetKeys() const { return KeyIdH.Len(); }
    // do we have Key in the index?
    bool IsKey(const TKey& Key) const { return KeyIdH.IsKey(Key); }
    // get item set for given key
    PGixItemSet GetItemSet(const TKey& Key);
    // adding new item to the inverted index
    void AddItem(const TKey& Key, const TItem& Item);
    // adding new items to the inverted index
    void AddItemV(const TKey& Key, const TVec<TItem>& ItemV);
    // clears items
    void Clr(const TKey& Key);

    // traversing keys
    int FFirstKeyId() const { return KeyIdH.FFirstKeyId(); }
    bool FNextKeyId(int& KeyId) const { return KeyIdH.FNextKeyId(KeyId); }
    const TKey& GetKey(const int& KeyId) { return KeyIdH.GetKey(KeyId); }

    // get amount of memory currently used
    int64 GetMemUsed() const {
        return int64(sizeof(TFAccess) + GixFNm.GetMemUsed() + GixBlobFNm.GetMemUsed()) +
            int64(KeyIdH.GetMemUsed()) + int64(ItemSetCache.GetMemUsed()); }
    int GetNewCacheSizeInc() const { return NewCacheSizeInc; }
    int GetCacheSize() const { return ItemSetCache.GetMemUsed(); }

    // for storing item sets from cache to blob
    void StoreItemSet(const TBlobPt& KeyId);

    friend class TPt<TGix>;
    friend class TGixItemSet<TKey, TItem>;
};

template <class TKey, class TItem>
TBlobPt TGix<TKey, TItem>::GetKeyId(const TKey& Key, const bool& CreateP) {
    if (IsKey(Key)) { return KeyIdH.GetDat(Key); }
    // we don't have this key, return empty pointer since we can't create it
    if (!CreateP) { return TBlobPt(); }
    // create an empty item set and return pointer to it
    AssertReadOnly(); // check if we are allowed to write
    PGixItemSet ItemSet = TGixItemSet<TKey, TItem>::New(Key);
    TMOut MOut; ItemSet->Save(MOut);
    TBlobPt KeyId = ItemSetBlobBs->PutBlob(MOut.GetSIn());
    KeyIdH.AddDat(Key, KeyId); // remember the new key and its Id
    return KeyId;
}

template <class TKey, class TItem>
TGix<TKey, TItem>::TGix(const TStr& Nm, const TStr& FPath, const TFAccess& _Access,
  const int64& CacheSize): Access(_Access), ItemSetCache(CacheSize, 1000000, GetVoidThis()) {

    // filenames of the GIX datastore
    GixFNm = TStr::GetNrFPath(FPath) + Nm.GetFBase() + ".Gix";
    GixBlobFNm = TStr::GetNrFPath(FPath) + Nm.GetFBase() + ".GixDat";

    if (Access == faCreate) {
        // creating a new Gix
        ItemSetBlobBs = TMBlobBs::New(GixBlobFNm, faCreate);
    } else {
        // loading an old Gix and getting it ready for search and update
        EAssert((Access == faUpdate) || (Access == faRdOnly));
        // load Gix from GixFNm
        TFIn FIn(GixFNm); KeyIdH.Load(FIn);
        // load ItemSets from GixBlobFNm
        ItemSetBlobBs = TMBlobBs::New(GixBlobFNm, Access);
    }

    CacheResetThreshold = int64(0.1 * double(CacheSize));
    NewCacheSizeInc = 0;
}

template <class TKey, class TItem>
TGix<TKey, TItem>::~TGix() {
    if ((Access == faCreate) || (Access == faUpdate)) {
        // flush all the latest changes in cache to the disk
        ItemSetCache.Flush();
        // save the rest to GixFNm
        TFOut FOut(GixFNm); KeyIdH.Save(FOut);
    }
}

template <class TKey, class TItem>
TPt<TGixItemSet<TKey, TItem> > TGix<TKey, TItem>::GetItemSet(const TKey& Key) {
    PGixItemSet ItemSet;
    // load the item set, if possible from cache
    TBlobPt KeyId = GetKeyId(Key, false);
    if (KeyId.Empty()) { return NULL; }
    if (!ItemSetCache.Get(KeyId, ItemSet)) {
        // have to load it from the hard drive...
        PSIn ItemSetSIn = ItemSetBlobBs->GetBlob(KeyId);
        ItemSet = TGixItemSet<TKey, TItem>::Load(*ItemSetSIn);
    }
    // bring the itemset to the top of the cache
    ItemSetCache.Put(KeyId, ItemSet);
    return ItemSet;
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::AddItem(const TKey& Key, const TItem& Item) {
    AssertReadOnly(); // check if we are allowed to write
    // get the key handle
    TBlobPt KeyId = GetKeyId(Key, true);
    // load the current item set
    PGixItemSet ItemSet = GetItemSet(Key);
    // add the new item to the set and update the size of new items
    NewCacheSizeInc += int64(ItemSet->AddItem(Item));
    // check if we have to drop anything from the cache
    if (NewCacheSizeInc > CacheResetThreshold) {
        ItemSetCache.RefreshMemUsed();
        NewCacheSizeInc = 0;
    }
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::AddItemV(const TKey& Key, const TVec<TItem>& ItemV) {
    AssertReadOnly(); // check if we are allowed to write
    // get the key handle
    TBlobPt KeyId = GetKeyId(Key, true);
    // load the current item set
    PGixItemSet ItemSet = GetItemSet(Key);
    // traverse new items
    for (int ItemN = 0; ItemN < ItemV.Len(); ItemN++) {
        //if (ItemN % 1000) { printf("%d/%d\r", ItemN, ItemV.Len()); }
        // add the new item to the set and update the size of new items
        NewCacheSizeInc += int64(ItemSet->AddItem(ItemV[ItemN]));
        // check if we have to drop anything from the cache
        if (NewCacheSizeInc > CacheResetThreshold) {
            ItemSetCache.RefreshMemUsed();
            NewCacheSizeInc = 0;
        }
    }
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::Clr(const TKey& Key) {
    AssertReadOnly(); // check if we are allowed to write
    if (IsKey(Key)) { // check if this key exists
        // load the current item set
        PGixItemSet ItemSet = GetItemSet(Key);
        // clear the items from the ItemSet
        ItemSet->Clr();
    }
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::StoreItemSet(const TBlobPt& KeyId) {
    AssertReadOnly(); // check if we are allowed to write
    // get the pointer to the item set
    PGixItemSet ItemSet; EAssert(ItemSetCache.Get(KeyId, ItemSet));
    // store the current version to the blob
    TMOut MOut; ItemSet->Save(MOut);
    TBlobPt NewKeyId = ItemSetBlobBs->PutBlob(KeyId, MOut.GetSIn());
    // and update the KeyId in the hash table
    KeyIdH.GetDat(ItemSet->GetKey()) = NewKeyId;
}

/////////////////////////////////////////////////
// General-Inverted-Index-Default-Merger
template <class TKey, class TItem>
class TGixDefMerger {
public:
    void Union(TVec<TItem>& MainV, const TVec<TItem>& JoinV) const {
        MainV.Union(JoinV); }
    void Minus(const TVec<TItem>& MainV, const TVec<TItem>& JoinV,
        TVec<TItem>& ResV) const { MainV.Minus(JoinV, ResV); }
    void Def(const TKey& Key, TVec<TItem>& MainV) const  { }
};

/////////////////////////////////////////////////
// General-Inverted-Index Expression-Item
typedef enum { getUndef, getEmpty, getOr, getAnd, getNot, getKey } TGixExpType;

template <class TKey, class TItem, class TMerger = TGixDefMerger<TKey, TItem> >
class TGixExpItem {
private:
    TCRef CRef;
    typedef TPt<TGixExpItem<TKey, TItem, TMerger> > PGixExpItem;
    typedef TPt<TGixItemSet<TKey, TItem> > PGixItemSet;
    typedef TPt<TGix<TKey, TItem> > PGix;
private:
    TGixExpType ExpType;
    PGixExpItem LeftExpItem;
    PGixExpItem RightExpItem;
    TKey Key;

    TGixExpItem(const TGixExpType& _ExpType, PGixExpItem _LeftExpItem,
      PGixExpItem _RightExpItem): ExpType(_ExpType),
        LeftExpItem(_LeftExpItem), RightExpItem(_RightExpItem) { }
    TGixExpItem(const TKey& _Key): ExpType(getKey), Key(_Key) { }
    TGixExpItem(): ExpType(getEmpty) { }
public:
    static PGixExpItem NewOr(PGixExpItem LeftExpItem, PGixExpItem RightExpItem) {
        return new TGixExpItem(getOr, LeftExpItem, RightExpItem); }
    static PGixExpItem NewAnd(PGixExpItem LeftExpItem, PGixExpItem RightExpItem) {
        return new TGixExpItem(getAnd, LeftExpItem, RightExpItem); }
    static PGixExpItem NewNot(PGixExpItem RightExpItem) {
        return new TGixExpItem(getNot, NULL, RightExpItem); }
    static PGixExpItem NewItem(const TKey& Key) {
        return new TGixExpItem(Key); }
    static PGixExpItem NewEmpty() {
        return new TGixExpItem(); }

    bool IsEmpty() const { return (ExpType == getEmpty); }
    TGixExpType GetExpType() const { return ExpType; }
    bool Eval(const PGix& Gix, TVec<TItem>& ResItemV,
        const TMerger& Merger = TMerger());

    friend class TPt<TGixExpItem>;
};

template <class TKey, class TItem, class TMerger>
bool TGixExpItem<TKey, TItem, TMerger>::Eval(const TPt<TGix<TKey, TItem> >& Gix,
        TVec<TItem>& ResItemV, const TMerger& Merger) {

    // prepare place for result
    ResItemV.Clr();
    if (ExpType == getOr) {
        EAssert(!LeftExpItem.Empty() && !RightExpItem.Empty());
        TVec<TItem> RightItemV;
        const bool NotLeft = LeftExpItem->Eval(Gix, ResItemV, Merger);
        const bool NotRight = RightExpItem->Eval(Gix, RightItemV, Merger);
        if ((NotLeft&&NotRight) || (!NotLeft&&!NotRight)) {
            Merger.Union(ResItemV, RightItemV);
        } else {
            TVec<TItem> MinusItemV;
            if (NotLeft) { Merger.Minus(ResItemV, RightItemV, MinusItemV); }
            else { Merger.Minus(RightItemV, ResItemV, MinusItemV); }
            ResItemV = MinusItemV;
        }
        return (NotLeft || NotRight);
    } else if (ExpType == getAnd) {
        EAssert(!LeftExpItem.Empty() && !RightExpItem.Empty());
        TVec<TItem> RightItemV;
        const bool NotLeft = LeftExpItem->Eval(Gix, ResItemV, Merger);
        const bool NotRight = RightExpItem->Eval(Gix, RightItemV, Merger);
        if (NotLeft&&NotRight) {
            Merger.Union(ResItemV, RightItemV);
        } else if (!NotLeft&&!NotRight) {
            Merger.Union(ResItemV, RightItemV);
        } else {
            TVec<TItem> MinusItemV;
            if (NotLeft) { Merger.Minus(RightItemV, ResItemV, MinusItemV); }
            else { Merger.Minus(ResItemV, RightItemV, MinusItemV); }
            ResItemV = MinusItemV;
        }
        return (NotLeft && NotRight);
    } else if (ExpType == getKey) {
        PGixItemSet ItemSet = Gix->GetItemSet(Key);
        if (!ItemSet.Empty()) {
            ItemSet->GetItemV(ResItemV, true);
            Merger.Def(ItemSet->GetKey(), ResItemV);
        }
        return false;
    } else if (ExpType == getNot) {
        RightExpItem->Eval(Gix, ResItemV, Merger);
        return true;
    } else if (ExpType == getEmpty) {
        return true; // we return everything
    }
    return true;
}

typedef TGixItemSet<TInt, TInt> TIntGixItemSet;
typedef TPt<TIntGixItemSet> PIntGixItemSet;
typedef TGix<TInt, TIntGixItemSet> TIntGix;
typedef TPt<TIntGix> PIntGix;
typedef TGixExpItem<TInt, TInt> TIntGixExpItem;
typedef TPt<TIntGixExpItem> PIntGixExpItem;

#endif
