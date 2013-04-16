#ifndef GIX_H
#define GIX_H

// this file depends only on base.h

/////////////////////////////////////////////////
// Forward-declarations
template <class TKey, class TItem> class TGix;

/////////////////////////////////////////////////
// General-Inverted-Index-Merger
template <class TKey, class TItem>
class TGixMerger {
protected:
    TCRef CRef;
    typedef TPt<TGixMerger<TKey, TItem> > PGixMerger;

public:
	virtual ~TGixMerger() { }

    virtual void Union(TVec<TItem>& MainV, const TVec<TItem>& JoinV) const = 0;
    virtual void Intrs(TVec<TItem>& MainV, const TVec<TItem>& JoinV) const = 0;
    virtual void Minus(const TVec<TItem>& MainV, 
		const TVec<TItem>& JoinV, TVec<TItem>& ResV) const = 0;
	virtual void Merge(TVec<TItem>& ItemV) const = 0;
    virtual void Def(const TKey& Key, TVec<TItem>& MainV) const = 0;

    friend class TPt<TGixMerger<TKey, TItem> >;
};

/////////////////////////////////////////////////
// General-Inverted-Index-Default-Merger
template <class TKey, class TItem>
class TGixDefMerger : public TGixMerger<TKey, TItem> {
private:
    typedef TPt<TGixMerger<TKey, TItem> > PGixMerger;

public:
	static PGixMerger New() { return new TGixDefMerger<TKey, TItem>(); }

    void Union(TVec<TItem>& MainV, const TVec<TItem>& JoinV) const { MainV.Union(JoinV); }
    void Intrs(TVec<TItem>& MainV, const TVec<TItem>& JoinV) const { MainV.Intrs(JoinV); }
    void Minus(const TVec<TItem>& MainV, const TVec<TItem>& JoinV, 
        TVec<TItem>& ResV) const { MainV.Diff(JoinV, ResV); }
	void Merge(TVec<TItem>& ItemV) const { ItemV.Merge(); }
    void Def(const TKey& Key, TVec<TItem>& MainV) const  { }
};

/////////////////////////////////////////////////
// General-Inverted-Index Key-To-String
template <class TKey>
class TGixKeyStr {
protected:
	TCRef CRef;
	typedef TPt<TGixKeyStr<TKey> > PGixKeyStr;

public:
	virtual ~TGixKeyStr() { }
	static PGixKeyStr New() { return new TGixKeyStr<TKey>; }

	// by default we cannot assume much about key, so just return empty string
	virtual TStr GetKeyNm(const TKey& Key) const { return TStr(); }

    friend class TPt<TGixKeyStr<TKey> >;
};

/////////////////////////////////////////////////
// General-Inverted-Index Item-Set
template <class TKey, class TItem>
class TGixItemSet {
private:
    TCRef CRef;
    typedef TPt<TGixItemSet<TKey, TItem> > PGixItemSet;
    typedef TPt<TGixMerger<TKey, TItem> > PGixMerger;
private:
    TKey ItemSetKey;
    TVec<TItem> ItemV;
    // for keeping the ItemV unique and sorted
    bool MergedP;
    PGixMerger Merger;

public:
    TGixItemSet(const TKey& _ItemSetKey, const PGixMerger& _Merger): 
      ItemSetKey(_ItemSetKey), MergedP(true), Merger(_Merger) { }
    static PGixItemSet New(const TKey& ItemSetKey, const PGixMerger& Merger) { 
        return new TGixItemSet(ItemSetKey, Merger); }

    TGixItemSet(TSIn& SIn, const PGixMerger& _Merger) { 
        ItemSetKey = TKey(SIn); ItemV.Load(SIn); Merger = _Merger; }
    static PGixItemSet Load(TSIn& SIn, const PGixMerger& Merger) { 
        return new TGixItemSet(SIn, Merger); }
    void Save(TSOut& SOut) const { ItemSetKey.Save(SOut); ItemV.Save(SOut); }

    // functions used by TCache
    int GetMemUsed() const {
        return ItemSetKey.GetMemUsed() + ItemV.GetMemUsed() + sizeof(TBool); }
    void OnDelFromCache(const TBlobPt& BlobPt, void* Gix);

    // key & items
    const TKey& GetKey() const { return ItemSetKey; }
	int AddItem(const TItem& NewItem);
	int AddItemV(const TVec<TItem>& NewItemV);
    int GetItems() const { return ItemV.Len(); }
    const TItem& GetItem(const int& ItemN) const { return ItemV[ItemN]; }
    const TVec<TItem>& GetItemV() const { return ItemV; }
    void GetItemV(TVec<TItem>& _ItemV);
	int DelItem(const TItem& Item);
    int Clr();
    void Def() { if (!MergedP) { Merger->Merge(ItemV); MergedP = true;} }

    friend class TPt<TGixItemSet>;
};

template <class TKey, class TItem>
void TGixItemSet<TKey, TItem>::OnDelFromCache(const TBlobPt& BlobPt, void* Gix) {
    if (!((TGix<TKey, TItem>*)Gix)->IsReadOnly()) {
        ((TGix<TKey, TItem>*)Gix)->StoreItemSet(BlobPt);
    } 
}

template <class TKey, class TItem>
int TGixItemSet<TKey, TItem>::AddItem(const TItem& NewItem) { 
    const int OldSize = ItemV.GetMemUsed();
    ItemV.Add(NewItem);
    MergedP = false;
    return ItemV.GetMemUsed() - OldSize;
}

template <class TKey, class TItem>
int TGixItemSet<TKey, TItem>::AddItemV(const TVec<TItem>& NewItemV) { 
    const int OldSize = ItemV.GetMemUsed();
    ItemV.AddV(NewItemV);
    MergedP = false;
    return ItemV.GetMemUsed() - OldSize;
}

template <class TKey, class TItem>
void TGixItemSet<TKey, TItem>::GetItemV(TVec<TItem>& _ItemV) { 
    _ItemV = ItemV; 
}

template <class TKey, class TItem>
int TGixItemSet<TKey, TItem>::DelItem(const TItem& Item) {
    const int OldSize = ItemV.GetMemUsed();
    ItemV.DelIfIn(Item);
    return ItemV.GetMemUsed() - OldSize;
}

template <class TKey, class TItem>
int TGixItemSet<TKey, TItem>::Clr() { 
    const int OldSize = ItemV.GetMemUsed();
    ItemV.Clr();
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
    typedef TPt<TGixMerger<TKey, TItem> > PGixMerger;
    typedef TGixDefMerger<TKey, TItem> _TGixDefMerger;
	typedef TPt<TGixKeyStr<TKey> > PGixKeyStr;
private:
    TFAccess Access;
    TStr GixFNm;
    TStr GixBlobFNm;
    THash<TKey, TBlobPt> KeyIdH; 
    mutable TCache<TBlobPt, PGixItemSet> ItemSetCache;
    PBlobBs ItemSetBlobBs;
	PGixMerger Merger;

    int64 CacheResetThreshold;
    int64 NewCacheSizeInc;
    bool CacheFullP;

    // returnes pointer to this object (used in cache call-backs)
    void* GetVoidThis() const { return (void*)this; }
    // asserts if we are allowed to change this index
    void AssertReadOnly() const {
        EAssertR(((Access==faCreate)||(Access==faUpdate)), 
            "Index opened in Read-Only mode!"); }
    // get keyid of a given key and create it if does not exist
    TBlobPt AddKeyId(const TKey& Key);
    TBlobPt GetKeyId(const TKey& Key) const;

public:
    TGix(const TStr& Nm, const TStr& FPath = TStr(), 
		const TFAccess& _Access = faRdOnly, const int64& CacheSize = 100000000, 
		const PGixMerger& _Merger = _TGixDefMerger::New() );
    static PGix New(const TStr& Nm, const TStr& FPath = TStr(), 
		const TFAccess& Access = faRdOnly, const int64& CacheSize = 100000000, 
		const PGixMerger& Merger = _TGixDefMerger::New()) {
            return new TGix(Nm, FPath, Access, CacheSize, Merger); }
	
	~TGix();

    // Gix properties
    bool IsReadOnly() const { return Access == faRdOnly; }
    TStr GetFPath() const { return GixFNm.GetFPath(); }
	int64 GetMxCacheSize() const { return ItemSetCache.GetMxMemUsed(); }

    // do we have Key in the index?
    bool IsKey(const TKey& Key) const { return KeyIdH.IsKey(Key); }
    // number of keys in the index
    int GetKeys() const { return KeyIdH.Len(); }
    // sort keys
    void SortKeys() { KeyIdH.SortByKey(true); }
    // get item set for given key
    PGixItemSet GetItemSet(const TKey& Key) const; 
    // adding new item to the inverted index
    void AddItem(const TKey& Key, const TItem& Item);
    // adding new items to the inverted index
    void AddItemV(const TKey& Key, const TVec<TItem>& ItemV);
    // delite one item
    void DelItem(const TKey& Key, const TItem& Item);
	// clears items
    void Clr(const TKey& Key);

    // traversing keys
    int FFirstKeyId() const { return KeyIdH.FFirstKeyId(); }
    bool FNextKeyId(int& KeyId) const { return KeyIdH.FNextKeyId(KeyId); }
    const TKey& GetKey(const int& KeyId) const { return KeyIdH.GetKey(KeyId); }

    // merges another index with same key and item vocabulary
    void MergeIndex(const PGix& TmpGix);

    // get amount of memory currently used
    int64 GetMemUsed() const { 
        return int64(sizeof(TFAccess) + GixFNm.GetMemUsed() + GixBlobFNm.GetMemUsed()) + 
            int64(KeyIdH.GetMemUsed()) + int64(ItemSetCache.GetMemUsed()); }
    int GetNewCacheSizeInc() const { return NewCacheSizeInc; }
    int GetCacheSize() const { return ItemSetCache.GetMemUsed(); }
    bool IsCacheFull() const { return CacheFullP; }
    void RefreshMemUsed();

    // for storing item sets from cache to blob
    void StoreItemSet(const TBlobPt& KeyId);

	// print statistics for index keys
	void SaveTxt(const TStr& FNm, const PGixKeyStr& KeyStr) const;

    friend class TPt<TGix>;
    friend class TGixItemSet<TKey, TItem>;
};

template <class TKey, class TItem>
TBlobPt TGix<TKey, TItem>::AddKeyId(const TKey& Key) { 
    if (IsKey(Key)) { return KeyIdH.GetDat(Key); }
    // we don't have this key, create an empty item set and return pointer to it
    AssertReadOnly(); // check if we are allowed to write
    PGixItemSet ItemSet = TGixItemSet<TKey, TItem>::New(Key, Merger);
    TMOut MOut; ItemSet->Save(MOut);
    TBlobPt KeyId = ItemSetBlobBs->PutBlob(MOut.GetSIn());
    KeyIdH.AddDat(Key, KeyId); // remember the new key and its Id
    return KeyId;
}

template <class TKey, class TItem>
TBlobPt TGix<TKey, TItem>::GetKeyId(const TKey& Key) const { 
    if (IsKey(Key)) { return KeyIdH.GetDat(Key); }
    // we don't have this key, return empty pointer
    return TBlobPt();
}

template <class TKey, class TItem>
TGix<TKey, TItem>::TGix(const TStr& Nm, const TStr& FPath, const TFAccess& _Access, 
  const int64& CacheSize, const TPt<TGixMerger<TKey, TItem> >& _Merger): Access(_Access),
  ItemSetCache(CacheSize, 1000000, GetVoidThis()), Merger(_Merger) {

    // filenames of the GIX datastore
    GixFNm = TStr::GetNrFPath(FPath) + Nm.GetFBase() + ".Gix";
    GixBlobFNm = TStr::GetNrFPath(FPath) + Nm.GetFBase() + ".GixDat";

    if (Access == faCreate) {
        // creating a new Gix
        ItemSetBlobBs = TMBlobBs::New(GixBlobFNm, faCreate);
    } else {
        // loading an old Gix and getting it ready for search and update
        EAssert((Access == faUpdate) || (Access == faRdOnly) || (Access == faRestore));
        // load Gix from GixFNm
        TFIn FIn(GixFNm); KeyIdH.Load(FIn);
        // load ItemSets from GixBlobFNm
        ItemSetBlobBs = TMBlobBs::New(GixBlobFNm, Access);
    }

    CacheResetThreshold = int64(0.1 * double(CacheSize));
    NewCacheSizeInc = 0; CacheFullP = false;
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
TPt<TGixItemSet<TKey, TItem> > TGix<TKey, TItem>::GetItemSet(const TKey& Key) const {
    PGixItemSet ItemSet;
    // load the item set, if possible from cache
    TBlobPt KeyId = GetKeyId(Key);
    if (KeyId.Empty()) { return NULL; }
    if (!ItemSetCache.Get(KeyId, ItemSet)) {
        // have to load it from the hard drive...
        PSIn ItemSetSIn = ItemSetBlobBs->GetBlob(KeyId);
        ItemSet = TGixItemSet<TKey, TItem>::Load(*ItemSetSIn, Merger);
    }
    // bring the itemset to the top of the cache
    ItemSetCache.Put(KeyId, ItemSet);
    return ItemSet;    
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::AddItem(const TKey& Key, const TItem& Item) {
    AssertReadOnly(); // check if we are allowed to write
    // get the key handle
    TBlobPt KeyId = AddKeyId(Key);
    // load the current item set
    PGixItemSet ItemSet = GetItemSet(Key);
    // add the new item to the set and update the size of new items
    NewCacheSizeInc += int64(ItemSet->AddItem(Item));
    // check if we have to drop anything from the cache
    RefreshMemUsed();
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::AddItemV(const TKey& Key, const TVec<TItem>& ItemV) {
    AssertReadOnly(); // check if we are allowed to write
    // get the key handle
    TBlobPt KeyId = AddKeyId(Key);
    // load the current item set
    PGixItemSet ItemSet = GetItemSet(Key);
    // add the new items to the set and update the size of new items
    NewCacheSizeInc += int64(ItemSet->AddItemV(ItemV));
    // check if we have to drop anything from the cache
    RefreshMemUsed();
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::DelItem(const TKey& Key, const TItem& Item) {
    AssertReadOnly(); // check if we are allowed to write
    if (IsKey(Key)) { // check if this key exists
        // load the current item set
        PGixItemSet ItemSet = GetItemSet(Key);
        // clear the items from the ItemSet
        ItemSet->DelItem(Item);
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
void TGix<TKey, TItem>::MergeIndex(const TPt<TGix<TKey, TItem> >& TmpGix) {
    // merge itemsets
    const int TmpKeys = TmpGix->GetKeys();
    int TmpKeyId = TmpGix->FFirstKeyId();
    while (TmpGix->FNextKeyId(TmpKeyId)) {
        const TKey& TmpKey = TmpGix->GetKey(TmpKeyId);
        PGixItemSet ItemSet = TmpGix->GetItemSet(TmpKey);
        AddItemV(ItemSet->GetKey(), ItemSet->GetItemV());
        if (TmpKeyId % 1000 == 0) { 
            printf("[%d/%d]\r", TmpKeyId, TmpKeys); }
    }
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::RefreshMemUsed() {
    // check if we have to drop anything from the cache
    if (NewCacheSizeInc > CacheResetThreshold) {
        printf("Cache clean-up [%s] ... ", 
            TUInt64::GetMegaStr(NewCacheSizeInc).CStr());
        // pack all the item sets
        TBlobPt BlobPt; PGixItemSet ItemSet;
        void* KeyDatP = ItemSetCache.FFirstKeyDat();
        while (ItemSetCache.FNextKeyDat(KeyDatP, BlobPt, ItemSet)) { ItemSet->Def(); }
        // clean-up cache
        CacheFullP = ItemSetCache.RefreshMemUsed(); 
        NewCacheSizeInc = 0; 
        const uint64 NewSize = ItemSetCache.GetMemUsed();
        printf("Done [%s]\n", TUInt64::GetMegaStr(NewSize).CStr());
    }
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::StoreItemSet(const TBlobPt& KeyId) {
    AssertReadOnly(); // check if we are allowed to write
    // get the pointer to the item set
    PGixItemSet ItemSet; EAssert(ItemSetCache.Get(KeyId, ItemSet));
    // make sure it's packed
    ItemSet->Def();
    // store the current version to the blob
    TMOut MOut; ItemSet->Save(MOut);
    TBlobPt NewKeyId = ItemSetBlobBs->PutBlob(KeyId, MOut.GetSIn());
    // and update the KeyId in the hash table
    KeyIdH.GetDat(ItemSet->GetKey()) = NewKeyId;
}

template <class TKey, class TItem>
void TGix<TKey, TItem>::SaveTxt(const TStr& FNm, const PGixKeyStr& KeyStr) const {
	TFOut FOut(FNm);
	// iterate over all the keys
	printf("Starting Gix SaveTxt\n");
	int KeyId = FFirstKeyId();
	int KeyN = 0; const int Keys = GetKeys();
	while (FNextKeyId(KeyId)) {
		if (KeyN % 1000 == 0) { printf("%d / %d\r", KeyN, Keys); } KeyN++;
		// get key and associated item set
		const TKey& Key = GetKey(KeyId);
		PGixItemSet ItemSet = GetItemSet(Key);
		// get statistics
		TStr KeyNm = KeyStr->GetKeyNm(Key);
		const int Items = ItemSet->GetItems();
		const int MemUsed = ItemSet->GetMemUsed();
		// output statistics
		FOut.PutStrFmtLn("%s\t%d\t%d", KeyNm .CStr(), Items, MemUsed);
	}
	printf("Done: %d / %d\n", Keys, Keys);
}

/////////////////////////////////////////////////
// General-Inverted-Index Expression-Item
typedef enum { getUndef, getEmpty, getOr, getAnd, getNot, getKey } TGixExpType;

template <class TKey, class TItem>
class TGixExpItem {
private:
    TCRef CRef;
    typedef TPt<TGixExpItem<TKey, TItem> > PGixExpItem;
    typedef TPt<TGixItemSet<TKey, TItem> > PGixItemSet;
    typedef TPt<TGix<TKey, TItem> > PGix;
    typedef TGixDefMerger<TKey, TItem> _TGixDefMerger;
private:
    TGixExpType ExpType;
    PGixExpItem LeftExpItem;
    PGixExpItem RightExpItem;
    TKey Key;

    TGixExpItem(const TGixExpType& _ExpType, const PGixExpItem& _LeftExpItem, 
      const PGixExpItem& _RightExpItem): ExpType(_ExpType), 
        LeftExpItem(_LeftExpItem), RightExpItem(_RightExpItem) { }
    TGixExpItem(const TKey& _Key): ExpType(getKey), Key(_Key) { }
    TGixExpItem(): ExpType(getEmpty) { }
	TGixExpItem(const TGixExpItem& ExpItem): ExpType(ExpItem.ExpType),
		LeftExpItem(ExpItem.LeftExpItem), RightExpItem(ExpItem.RightExpItem),
		Key(ExpItem.Key) { }

	void PutAnd(const PGixExpItem& _LeftExpItem, const PGixExpItem& _RightExpItem);
	void PutOr(const PGixExpItem& _LeftExpItem, const PGixExpItem& _RightExpItem);
public:
	// elementary operations
    static PGixExpItem NewOr(const PGixExpItem& LeftExpItem, const PGixExpItem& RightExpItem) { 
        return new TGixExpItem(getOr, LeftExpItem, RightExpItem); }
    static PGixExpItem NewAnd(const PGixExpItem& LeftExpItem, const PGixExpItem& RightExpItem) { 
        return new TGixExpItem(getAnd, LeftExpItem, RightExpItem); }
    static PGixExpItem NewNot(const PGixExpItem& RightExpItem) { 
        return new TGixExpItem(getNot, NULL, RightExpItem); }
    static PGixExpItem NewItem(const TKey& Key) { 
        return new TGixExpItem(Key); }
    static PGixExpItem NewEmpty() { 
        return new TGixExpItem(); }

	// some predifined structures
	static PGixExpItem NewAndV(const TVec<PGixExpItem>& ExpItemV);
	static PGixExpItem NewOrV(const TVec<PGixExpItem>& ExpItemV);
	static PGixExpItem NewAndV(const TVec<TKey>& KeyV);
	static PGixExpItem NewOrV(const TVec<TKey>& KeyV);

    bool IsEmpty() const { return (ExpType == getEmpty); }
    TGixExpType GetExpType() const { return ExpType; }
	TKey GetKey() const { return Key; }
	PGixExpItem Clone() const { return new TGixExpItem(*this); }
    bool Eval(const PGix& Gix, TVec<TItem>& ResItemV, 
		const TPt<TGixMerger<TKey, TItem> >& Merger = _TGixDefMerger::New());

    friend class TPt<TGixExpItem>;
};

template <class TKey, class TItem>
void TGixExpItem<TKey, TItem>::PutAnd(const TPt<TGixExpItem<TKey, TItem> >& _LeftExpItem, 
		const TPt<TGixExpItem<TKey, TItem> >& _RightExpItem) { 

	ExpType = getAnd;
	LeftExpItem = _LeftExpItem;
	RightExpItem = _RightExpItem;
}

template <class TKey, class TItem>
void TGixExpItem<TKey, TItem>::PutOr(const TPt<TGixExpItem<TKey, TItem> >& _LeftExpItem,
		const TPt<TGixExpItem<TKey, TItem> >& _RightExpItem) { 

	ExpType = getOr;
	LeftExpItem = _LeftExpItem;
	RightExpItem = _RightExpItem;
}

template <class TKey, class TItem>
TPt<TGixExpItem<TKey, TItem> > TGixExpItem<TKey, TItem>::NewAndV(
		const TVec<TPt<TGixExpItem<TKey, TItem> > >& ExpItemV) {

	// return empty item if no key is given
	if (ExpItemV.Empty()) { return TGixExpItem<TKey, TItem>::NewEmpty(); }
	// otherwise we start with the first key
	TPt<TGixExpItem<TKey, TItem> > TopExpItem = ExpItemV[0];
	// prepare a queue, which points to the next item (left) to be expanded to tree (and left right)
	TQQueue<TPt<TGixExpItem<TKey, TItem> > > NextExpItemQ;
	// we start with the top
	NextExpItemQ.Push(TopExpItem);
	// add the rest of the items to the expresion tree
	for (int ExpItemN = 1; ExpItemN < ExpItemV.Len(); ExpItemN++) {
		const TPt<TGixExpItem<TKey, TItem> >& RightExpItem = ExpItemV[ExpItemN];
		// which item should we expand
		TPt<TGixExpItem<TKey, TItem> > ExpItem = NextExpItemQ.Top(); NextExpItemQ.Pop();
		// clone the item to be expanded
		TPt<TGixExpItem<TKey, TItem> > LeftExpItem = ExpItem->Clone();
		// and make a new subtree
		ExpItem->PutAnd(LeftExpItem, RightExpItem);
		// update the queue
		NextExpItemQ.Push(ExpItem->LeftExpItem);
		NextExpItemQ.Push(ExpItem->RightExpItem);
	}
	return TopExpItem;
}

template <class TKey, class TItem>
TPt<TGixExpItem<TKey, TItem> > TGixExpItem<TKey, TItem>::NewOrV(
		const TVec<TPt<TGixExpItem<TKey, TItem> > >& ExpItemV) {

	// return empty item if no key is given
	if (ExpItemV.Empty()) { return TGixExpItem<TKey, TItem>::NewEmpty(); }
	// otherwise we start with the first key
	TPt<TGixExpItem<TKey, TItem> > TopExpItem = ExpItemV[0];
	// prepare a queue, which points to the next item (left) to be expanded to tree (and left right)
	TQQueue<TPt<TGixExpItem<TKey, TItem> > > NextExpItemQ;
	// we start with the top
	NextExpItemQ.Push(TopExpItem);
	// add the rest of the items to the expresion tree
	for (int ExpItemN = 1; ExpItemN < ExpItemV.Len(); ExpItemN++) {
		const TPt<TGixExpItem<TKey, TItem> >& RightExpItem = ExpItemV[ExpItemN];
		// which item should we expand
		TPt<TGixExpItem<TKey, TItem> > ExpItem = NextExpItemQ.Top(); NextExpItemQ.Pop();
		// clone the item to be expanded
		TPt<TGixExpItem<TKey, TItem> > LeftExpItem = ExpItem->Clone();
		// and make a new subtree
		ExpItem->PutOr(LeftExpItem, RightExpItem);
		// update the queue
		NextExpItemQ.Push(ExpItem->LeftExpItem);
		NextExpItemQ.Push(ExpItem->RightExpItem);
	}
	return TopExpItem;
}

template <class TKey, class TItem>
TPt<TGixExpItem<TKey, TItem> > TGixExpItem<TKey, TItem>::NewAndV(const TVec<TKey>& KeyV) {
	TVec<TPt<TGixExpItem<TKey, TItem> > > ExpItemV(KeyV.Len(), 0);
	for (int KeyN = 0; KeyN < KeyV.Len(); KeyN++) {
		ExpItemV.Add(TGixExpItem<TKey, TItem>::NewItem(KeyV[KeyN]));
	}
	return NewAndV(ExpItemV);
}

template <class TKey, class TItem>
TPt<TGixExpItem<TKey, TItem> > TGixExpItem<TKey, TItem>::NewOrV(const TVec<TKey>& KeyV) {
	TVec<TPt<TGixExpItem<TKey, TItem> > > ExpItemV(KeyV.Len(), 0);
	for (int KeyN = 0; KeyN < KeyV.Len(); KeyN++) {
		ExpItemV.Add(TGixExpItem<TKey, TItem>::NewItem(KeyV[KeyN]));
	}
	return NewOrV(ExpItemV);
}

template <class TKey, class TItem>
bool TGixExpItem<TKey, TItem>::Eval(const TPt<TGix<TKey, TItem> >& Gix, 
        TVec<TItem>& ResItemV, const TPt<TGixMerger<TKey, TItem> >& Merger) {

    // prepare place for result
    ResItemV.Clr();
    if (ExpType == getOr) {
        EAssert(!LeftExpItem.Empty() && !RightExpItem.Empty());
        TVec<TItem> RightItemV;
        const bool NotLeft = LeftExpItem->Eval(Gix, ResItemV, Merger);
        const bool NotRight = RightExpItem->Eval(Gix, RightItemV, Merger);
        if (NotLeft && NotRight) {
            Merger->Intrs(ResItemV, RightItemV);
        } else if (!NotLeft && !NotRight) { 
            Merger->Union(ResItemV, RightItemV);
        } else {
            TVec<TItem> MinusItemV;
            if (NotLeft) { Merger->Minus(ResItemV, RightItemV, MinusItemV); }
            else { Merger->Minus(RightItemV, ResItemV, MinusItemV); }
            ResItemV = MinusItemV;                       
        }
        return (NotLeft || NotRight);
    } else if (ExpType == getAnd) {
        EAssert(!LeftExpItem.Empty() && !RightExpItem.Empty());
        TVec<TItem> RightItemV;
        const bool NotLeft = LeftExpItem->Eval(Gix, ResItemV, Merger);
        const bool NotRight = RightExpItem->Eval(Gix, RightItemV, Merger);
        if (NotLeft && NotRight) { 
            Merger->Union(ResItemV, RightItemV);
        } else if (!NotLeft && !NotRight) {
            Merger->Intrs(ResItemV, RightItemV);
        } else {
            TVec<TItem> MinusItemV;
            if (NotLeft) { Merger->Minus(RightItemV, ResItemV, MinusItemV); }
            else { Merger->Minus(ResItemV, RightItemV, MinusItemV); }
            ResItemV = MinusItemV;
        }
        return (NotLeft && NotRight);
    } else if (ExpType == getKey) {
        PGixItemSet ItemSet = Gix->GetItemSet(Key);
        if (!ItemSet.Empty()) { 
            ItemSet->Def();
            ItemSet->GetItemV(ResItemV); 
            Merger->Def(ItemSet->GetKey(), ResItemV);
        }
        return false;
    } else if (ExpType == getNot) {
        return !RightExpItem->Eval(Gix, ResItemV, Merger);
    } else if (ExpType == getEmpty) {
        return false; // return nothing
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