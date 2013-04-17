#ifndef CACHE_H
#define CACHE_H

///////////////////////////////
// Value-Cache
// 
//   requirements for TVal:
//    - [int64 or int] GetMemUsed() const
//    - void Load(TSIn& SIn)
//    - void Save(TSOut& SOut) const
//
template <class TVal>
class TValCache {
private:
	ClassTP(TValDat, PValDat)//{
	public:
		TVal Val;

	public:
		TValDat(const TVal& _Val): Val(_Val) { }
		static PValDat New(const TVal& Val) { return new TValDat(Val); }

		// need to report size, for keeping up used-up space in cache
        int64 GetMemUsed() const { return (int64)Val.GetMemUsed(); }
        // no need to do anything on delete from cache as values are 
		// assumed stored on the disk already when crawled
        void OnDelFromCache(const TInt& ValId, void* ValCache) { }
	};

private:
    // remember access mode
    TStr FNmPrefix;
    TFAccess Access;
	// max memory to be used by cache
    int64 CacheResetThreshold;
    int64 NewCacheSizeInc;

	// value cache
	mutable TCache<TInt, PValDat> ValCache;
	// value disk store
    TBlobPtV ValBlobPtV;
	PBlobBs ValBlobBs;

private:
    // asserts if we are allowed to change stuff
    void AssertReadOnly() const {
        EAssertR(((Access==faCreate)||(Access==faUpdate)), 
            FNmPrefix + " opened in Read-Only mode!"); }
	// for callbacks from cache
    void* GetVoidThis() const { return (void*)this; }
	// undef default, copy and assign
	UndefDefaultCopyAssign(TValCache);
public:
    TValCache(const TStr& _FNmPrefix, const TFAccess& _Access, const int64& MxCacheMem);
	~TValCache();

	// properties
    bool IsReadOnly() const { return Access == faRdOnly; }

	// store new value
	int AddVal(const TVal& Val);
	// retrieve stored values
	int Len() const { return ValBlobPtV.Len(); }
	bool IsValId(const int& ValId) const { return (ValId >= 0) && (ValId < ValBlobPtV.Len()); }
	void GetVal(const int& ValId, TVal& Val) const;
};

template <class TVal>
TValCache<TVal>::TValCache(const TStr& _FNmPrefix, const TFAccess& _Access, 
	const int64& MxCacheMem): ValCache(MxCacheMem, 1000000, GetVoidThis()) {

	// initialize storage parameters
    FNmPrefix = _FNmPrefix;
    Access = _Access;
	// initialize cache parameters
    CacheResetThreshold = int64(0.1 * double(MxCacheMem));
	NewCacheSizeInc = 0;
	// initialize value disk store
    ValBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	// inititalize meta-data
    if (Access == faCreate) {
        // notning to initialize when starting from zero
	} else {
		// load ValId map
		TFIn FIn(FNmPrefix + ".Dat");
		ValBlobPtV.Load(FIn);
	}
}

template <class TVal>
TValCache<TVal>::~TValCache() {
    if ((Access == faCreate) || (Access == faUpdate)) {
        // save the rest to FNmPrefix + ".Dat"
        TFOut FOut(FNmPrefix + ".Dat"); ValBlobPtV.Save(FOut);
    }
}

template <class TVal>
int TValCache<TVal>::AddVal(const TVal& Val) {
	AssertReadOnly();
	// store value to the disk
    TMOut MOut; Val.Save(MOut);
	const TBlobPt& ValBlobPt = ValBlobBs->PutBlob(MOut.GetSIn());
	const int ValId = ValBlobPtV.Add(ValBlobPt);
	// push value to the top of the cache
	PValDat ValDat = TValDat::New(Val);
    NewCacheSizeInc += int64(sizeof(TInt) + ValDat->GetMemUsed());
	ValCache.Put(ValId, ValDat);
    // check if we have to drop anything from the cache
    if (NewCacheSizeInc > CacheResetThreshold) {
		// report on the size increase
        printf("Cache clean-up [%s] ... ", TUInt64::GetMegaStr(NewCacheSizeInc).CStr());
		// report on the current size
        //const uint64 OldSize = ValCache.GetMemUsed();
        //printf("[%s] ... ", TUInt64::GetMegaStr(OldSize).CStr());
		// refresh memory count
		ValCache.RefreshMemUsed();
		//// report on the current size
        //const uint64 NewSize = ValCache.GetMemUsed();
		//printf("[%s] ", TUInt64::GetMegaStr(NewSize).CStr());
		printf("Done\n");
		// reset increase counter
        NewCacheSizeInc = 0; 
	}
	// return id
	return ValId;
}

template <class TVal>
void TValCache<TVal>::GetVal(const int& _ValId, TVal& Val) const {
	const int ValId = (int)_ValId;
	// load from the cache
	PValDat ValDat;
	if (!ValCache.Get(ValId, ValDat)) {
		// if not in there, load from disk
		const TBlobPt& ValBlobPt = ValBlobPtV[ValId];
		PSIn SIn = ValBlobBs->GetBlob(ValBlobPt); 
		Val.Load(*SIn);
		// prepare it to be stored in cache
		ValDat = TValDat::New(Val);
	} else {
		// return cached value
		Val = ValDat->Val;
	}
	// bring to the top of cache
	ValCache.Put(ValId, ValDat);
}

///////////////////////////////
// Block-Cache
// 
//   requirements for TVal:
//    - TVal()
//    - [int64 or int] GetMemUsed() const
//    - TVal(TSIn& SIn)
//    - void Load(TSIn& SIn)
//    - void Save(TSOut& SOut) const
//
template <class TVal>
class TBlockCache {
private:
	typedef TVec<TVal> TValV;

	ClassTP(TBlockDat, PBlockDat)//{
	private:
		TBool ChangedP;
		TVec<TVal> ValV;

	public:
		TBlockDat(): ChangedP(true) { }
		static PBlockDat New() { return new TBlockDat; }
		// serialization
		TBlockDat(TSIn& SIn): ChangedP(false) { ValV.Load(SIn); }
		static PBlockDat Load(TSIn& SIn) { return new TBlockDat(SIn); }
		void Save(TSOut& SOut) const { ValV.Save(SOut); }

		// number of values in the block
		int GetVals() const { return ValV.Len(); }
		bool IsFull(const int& BlockSize) const { return !(ValV.Len() < BlockSize); }
		// add new value
		int AddVal(const TVal& Val) { ChangedP = true; return ValV.Add(Val); }
		// updated existing value
		void SetVal(const int& ValId, const TVal& Val) { ChangedP = true; ValV[ValId] = Val; }
		// retrieve value
		bool IsValId(const int& ValId) const { return (ValId >= 0) && (ValId < ValV.Len()); }
		const TVal& GetVal(const int& ValId) const { return ValV[ValId]; }

		// need to report size, for keeping up used-up space in cache
        int64 GetMemUsed() const {
			int64 MemUsed = (int64)sizeof(TVec<TVal>);
			for (int ValN = 0; ValN < ValV.Len(); ValN++) {
				MemUsed += (int64)ValV[ValN].GetMemUsed(); }
			return MemUsed;
		}
        // store only on delete-from cache (in case new values added)
        void OnDelFromCache(const TInt& BlockId, void* BlockCache) {
			if (ChangedP && !((TBlockCache*)BlockCache)->IsReadOnly()) {
				((TBlockCache*)BlockCache)->StoreBlock(BlockId); }
		}
	};

private:
    // remember access mode
    TStr FNmPrefix;
    TFAccess Access;
	// max memory to be used by cache
    int64 CacheResetThreshold;
    int64 NewCacheSizeInc;
	// number of items
	TInt Vals;
	// block-size
	TInt BlockSize;

	// value cache
	mutable TCache<TInt, PBlockDat> BlockCache;
	// value disk store
    TBlobPtV BlockBlobPtV;
	PBlobBs BlockBlobBs;

private:
    // asserts if we are allowed to change stuff
    void AssertReadOnly() const {
        EAssertR(((Access==faCreate)||(Access==faUpdate)), 
            FNmPrefix + " opened in Read-Only mode!"); }

	// for callbacks from cache, to store blocks before drop from cache
    void* GetVoidThis() const { return (void*)this; }
	void StoreBlock(const int& BlockId);

	// add new block to the end
	int AddBlock();
	// get block from cache or load from disc
	void GetBlock(const int& BlockId, PBlockDat& BlockDat) const;
	// returns last block in vector, creates a new one if full
	int GetLastBlock(PBlockDat& BlockDat);

	// value id transformations
	uint64 GetValId(const int& BlockId, const int& BlockValId) const {
		return (uint64)BlockId*(uint64)BlockSize + (uint64)BlockValId; }
	void GetBlockId(const uint64& ValId, int& BlockId, int& BlockValId) const {
		BlockId = (int)(ValId / (uint64)BlockSize);
		BlockValId = (int)(ValId % (uint64)BlockSize); 
	}

	// undef default, copy and assign
	UndefDefaultCopyAssign(TBlockCache);
public:
    TBlockCache(const TStr& _FNmPrefix, const int64& MxCacheMem, const int& _BlockSize);
    TBlockCache(const TStr& _FNmPrefix, const TFAccess& _Access, const int64& MxCacheMem);
	~TBlockCache();

	// properties
    bool IsReadOnly() const { return Access == faRdOnly; }

	// store new value
	uint64 AddVal(const TVal& Val);
	// update existing value
	void SetVal(const uint64& ValId, const TVal& Val);
	// retrieve stored values
	int Len() const { return Vals; }
	bool IsValId(const uint64& ValId) const;
	void GetVal(const uint64& ValId, TVal& Val) const;
};

template <class TVal>
void TBlockCache<TVal>::StoreBlock(const int& BlockId) {
	AssertReadOnly();
    PBlockDat BlockDat; EAssert(BlockCache.Get(BlockId, BlockDat));
	// store value to the disk
	TMOut MOut; BlockDat->Save(MOut);
	const TBlobPt& BlockBlobPt = BlockBlobPtV[BlockId];
	if (BlockBlobPt.Empty()) {
		// first time
		BlockBlobPtV[BlockId] = BlockBlobBs->PutBlob(MOut.GetSIn());
	} else {
		// overwrite existing
		BlockBlobPtV[BlockId] = BlockBlobBs->PutBlob(BlockBlobPt, MOut.GetSIn());
	}
}

template <class TVal>
void TBlockCache<TVal>::GetBlock(const int& BlockId, PBlockDat& BlockDat) const {
	// load from the cache
	if (!BlockCache.Get(BlockId, BlockDat)) {
		// if not in there, load from disk
		const TBlobPt& BlockBlobPt = BlockBlobPtV[(int)BlockId];
		PSIn SIn = BlockBlobBs->GetBlob(BlockBlobPt); 
		BlockDat = TBlockDat::Load(*SIn);
	}
	// bring to the top of cache
	BlockCache.Put(BlockId, BlockDat);
}

template <class TVal>
int TBlockCache<TVal>::AddBlock() {
	// creat new block
	PBlockDat BlockDat = TBlockDat::New();
	// mark existance of the block with empty disk-blob pointer
	const int BlockId = BlockBlobPtV.Add(TBlobPt());
	// add it to cache
	BlockCache.Put(BlockId, BlockDat);
	// update cache size increase with the size of cache key and empty block
	NewCacheSizeInc += sizeof(TInt) + BlockDat->GetMemUsed(); 
	// return id of the new block
	return BlockId;
}

template <class TVal>
int TBlockCache<TVal>::GetLastBlock(PBlockDat& BlockDat) {
	// ID of the last block in the queue
	const int LastBlockId = BlockBlobPtV.Len()-1;
	// retrieve block
	GetBlock(LastBlockId, BlockDat);
	// if full, make a new one
	if (BlockDat->IsFull(BlockSize)) {
		//creat new block
		const int NewBlockId = AddBlock();
		// return its reference
		GetBlock(NewBlockId, BlockDat);
		// and new block id
		return NewBlockId;
	}
	// just return existing last block id
	return LastBlockId;
}

template <class TVal>
TBlockCache<TVal>::TBlockCache(const TStr& _FNmPrefix, const int64& MxCacheMem,
		const int& _BlockSize): BlockSize(_BlockSize), BlockCache(MxCacheMem, 1000000, GetVoidThis()) {

	// initialize storage parameters
    FNmPrefix = _FNmPrefix;
    Access = faCreate;
	// initialize cache parameters
    CacheResetThreshold = int64(0.1 * double(MxCacheMem));
	NewCacheSizeInc = 0;
	// initialize value disk store
	try {
		BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	} catch (...) { 
		// try with restor
		TMBlobBs::New(FNmPrefix + "BlobBs", faRestore);
		// open it then in a normal fashion
		BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	}
	
    BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	// create first block
	EAssertR(AddBlock() == 0, "Error creating first cache block");
}

template <class TVal>
TBlockCache<TVal>::TBlockCache(const TStr& _FNmPrefix, const TFAccess& _Access,
		const int64& MxCacheMem): BlockCache(MxCacheMem, 1000000, GetVoidThis()) {

	// initialize storage parameters
    FNmPrefix = _FNmPrefix;
    Access = _Access;
	// initialize cache parameters
    CacheResetThreshold = int64(0.1 * double(MxCacheMem));
	NewCacheSizeInc = 0;
	// initialize value disk store
    BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	// make sure we are not trying to create
	EAssertR(Access != faCreate, "First call create constructor!");
	// load BlockId map and BlockSize
	TFIn FIn(FNmPrefix + ".Dat");
	Vals.Load(FIn);
	BlockSize.Load(FIn);
	BlockBlobPtV.Load(FIn);
}

template <class TVal>
TBlockCache<TVal>::~TBlockCache() {
    if ((Access == faCreate) || (Access == faUpdate)) {
        // flush all the latest changes in cache to the disk
        BlockCache.Flush();
        // save the rest to FNmPrefix + ".Dat"
        TFOut FOut(FNmPrefix + ".Dat");
		Vals.Save(FOut);
		BlockSize.Save(FOut);
		BlockBlobPtV.Save(FOut);
    }
}

template <class TVal>
uint64 TBlockCache<TVal>::AddVal(const TVal& Val) {
	// get last block, with some space left
	PBlockDat BlockDat; 
	const int BlockId = GetLastBlock(BlockDat);
	// add the value to the block
	const int BlockValId = BlockDat->AddVal(Val);
	// update cache increase count
    NewCacheSizeInc += BlockDat->GetVal(BlockValId).GetMemUsed();
    // check if we have to drop anything from the cache
    if (NewCacheSizeInc > CacheResetThreshold) {
		// report on the size increase
        printf("Cache clean-up [%s] ... ", TUInt64::GetMegaStr(NewCacheSizeInc).CStr());
		//// report on the current size
		//const uint64 OldSize = BlockCache.GetMemUsed();
		//printf("[%s] ... ", TUInt64::GetMegaStr(OldSize).CStr());
		// refresh memory count
		BlockCache.RefreshMemUsed();
		//// report on the current size
		//const uint64 NewSize = BlockCache.GetMemUsed();
		//printf("[%s] ", TUInt64::GetMegaStr(NewSize).CStr());
		printf("Done\n");
		// reset increase counter
        NewCacheSizeInc = 0; 
	}
	// increase count of all values
	Vals++;
	// compute the value ID as a combination of BlockId and BlockValId
	return GetValId(BlockId, BlockValId);
}

template <class TVal>
void TBlockCache<TVal>::SetVal(const uint64& ValId, const TVal& Val) {
	// transfor to block ids
	int BlockId = -1, BlockValId = -1;
	GetBlockId(ValId, BlockId, BlockValId);
	// get the block
	PBlockDat BlockDat; GetBlock(BlockId, BlockDat);
	// set the val
	BlockDat->SetVal(BlockValId, Val);
}

template <class TVal>
bool TBlockCache<TVal>::IsValId(const uint64& ValId) const { 
	// transfor to block ids
	int BlockId = -1, BlockValId = -1;
	GetBlockId(ValId, BlockId, BlockValId);
	// check all is fine
	if (BlockId < 0 || BlockId >= BlockBlobPtV.Len()) { return false; }
	PBlockDat BlockDat; GetBlock(BlockId, BlockDat);
	return BlockDat->IsValId(BlockValId);	
}

template <class TVal>
void TBlockCache<TVal>::GetVal(const uint64& ValId, TVal& Val) const {
	// transfor to block ids
	int BlockId = -1, BlockValId = -1;
	GetBlockId(ValId, BlockId, BlockValId);
	// get the block
	PBlockDat BlockDat; GetBlock(BlockId, BlockDat);
	// get the val
	Val = BlockDat->GetVal(BlockValId);
}

///////////////////////////////
// Windowed-Block-Cache
// 
//   requirements for TVal:
//    - TVal()
//    - [int64 or int] GetMemUsed() const
//    - TVal(TSIn& SIn)
//    - void Load(TSIn& SIn)
//    - void Save(TSOut& SOut) const
//
template <class TVal>
class TWndBlockCache {
private:
	typedef TVec<TVal> TValV;

	ClassTP(TBlockDat, PBlockDat)//{
	private:
		TBool ChangedP;
		TVec<TVal> ValV;

	public:

		TBlockDat(): ChangedP(true) { }
		static PBlockDat New() { return new TBlockDat; }
		// serialization
		TBlockDat(TSIn& SIn): ChangedP(false) { ValV.Load(SIn); }
		static PBlockDat Load(TSIn& SIn) { return new TBlockDat(SIn); }
		void Save(TSOut& SOut) const { ValV.Save(SOut); }

		// number of values in the block
		int GetVals() const { return ValV.Len(); }
		bool IsFull(const int& BlockSize) const { return !(ValV.Len() < BlockSize); }
		// add new value
		int AddVal(const TVal& Val) { ChangedP = true; return ValV.Add(Val); }
		// updated existing value
		void SetVal(const int& ValId, const TVal& Val) { ChangedP = true; ValV[ValId] = Val; }
		// retrieve value
		bool IsValId(const int& ValId) const { return (ValId >= 0) && (ValId < ValV.Len()); }
		const TVal& GetVal(const int& ValId) const { return ValV[ValId]; }

		// need to report size, for keeping up used-up space in cache
		int64 GetMemUsed() const {
			int64 MemUsed = (int64)sizeof(TVec<TVal>);
			for (int ValN = 0; ValN < ValV.Len(); ValN++) {
				MemUsed += (int64)ValV[ValN].GetMemUsed(); }
			return MemUsed;
		}
		// store only on delete-from cache (in case new values added)
		void OnDelFromCache(const TInt& BlockId, void* WndBlockCache) {
			if (ChangedP && !((TWndBlockCache*)WndBlockCache)->IsReadOnly()) {
				((TWndBlockCache*)WndBlockCache)->StoreBlock(BlockId); }
		}
	};

private:
	// remember access mode
	TStr FNmPrefix;
	TFAccess Access;
	// max memory to be used by cache
	int64 CacheResetThreshold;
	int64 NewCacheSizeInc;
	// number of items
	TInt Vals;
	// block-size
	TInt BlockSize;

	// value cache
	mutable TCache<TInt, PBlockDat> BlockCache;
	// value disk store
	TBlobPtV BlockBlobPtV;
	PBlobBs BlockBlobBs;

	// offset of the oldest block
	TInt FirstBlockOffset;
	// offset of the oldest record within the oldest block
	TInt FirstValOffset;

private:
	// asserts if we are allowed to change stuff
	void AssertReadOnly() const {
		EAssertR(((Access==faCreate)||(Access==faUpdate)), FNmPrefix + " opened in Read-Only mode!"); 
	}

	// for callbacks from cache, to store blocks before drop from cache
	void* GetVoidThis() const { return (void*)this; }
	void StoreBlock(const int& BlockId);

	// add new block to the end
	int AddBlock();
	// get block from cache or load from disc
	void GetBlock(const int& BlockId, PBlockDat& BlockDat) const;
	// returns last block in vector, creates a new one if full
	int GetLastBlock(PBlockDat& BlockDat);
	// delete oldest block
	void DelBlock();

	// value id transformations
	uint64 GetValId(const int& BlockId, const int& BlockValId) const {
		return (uint64)BlockId*(uint64)BlockSize + (uint64)BlockValId; 
	}
	void GetBlockId(const uint64& ValId, int& BlockId, int& BlockValId) const {
		BlockId = (int)(ValId / (uint64)BlockSize);
		BlockValId = (int)(ValId % (uint64)BlockSize); 
	}
	
public:
	TWndBlockCache(const TStr& _FNmPrefix, const int64& MxCacheMem, const int& _BlockSize);
	TWndBlockCache(const TStr& _FNmPrefix, const TFAccess& _Access, const int64& MxCacheMem);
	~TWndBlockCache();

	// properties
	bool IsReadOnly() const { return Access == faRdOnly; }
	// store new value 
	uint64 AddVal(const TVal& Val);
	// update existing value
	void SetVal(const uint64& ValId, const TVal& Val);
	// retrieve stored values
	bool Empty() const { return Vals == 0; }
	int Len() const { return Vals; }
	// oldest value id
	uint64 GetFirstValId() const;
	// latest value id
	uint64 GetLastValId() const;
	bool IsValId(const uint64& ValId) const;
	void GetVal(const uint64& ValId, TVal& Val) const;	
	uint64 GetFirstVal(TVal& Val) const;	
	// delete first value
	bool DelVal();
	// delete first N values
	int DelVals(const int& _Vals);
};

template <class TVal>
void TWndBlockCache<TVal>::StoreBlock(const int& BlockId) {
	AssertReadOnly();
	PBlockDat BlockDat; 
	EAssert(BlockCache.Get(BlockId, BlockDat));
	// store value to the disk
	TMOut MOut; 
	BlockDat->Save(MOut);
	int _BlockId = BlockId - FirstBlockOffset;
	const TBlobPt& BlockBlobPt = BlockBlobPtV[_BlockId];
	if (BlockBlobPt.Empty()) {
		// first time
		BlockBlobPtV[_BlockId] = BlockBlobBs->PutBlob(MOut.GetSIn());
	} else {
		// overwrite existing
		BlockBlobPtV[_BlockId] = BlockBlobBs->PutBlob(BlockBlobPt, MOut.GetSIn());
	}
}

template <class TVal>
void TWndBlockCache<TVal>::GetBlock(const int& BlockId, PBlockDat& BlockDat) const {
	// load from the cache
	if (!BlockCache.Get(BlockId, BlockDat)) {
		// if not in there, load from disk
		int _BlockId = BlockId - FirstBlockOffset;
		const TBlobPt& BlockBlobPt = BlockBlobPtV[_BlockId];
		PSIn SIn = BlockBlobBs->GetBlob(BlockBlobPt); 
		BlockDat = TBlockDat::Load(*SIn);
	}
	// bring to the top of cache
	BlockCache.Put(BlockId, BlockDat);
}

template <class TVal>
int TWndBlockCache<TVal>::AddBlock() {
	// create new block
	PBlockDat BlockDat = TBlockDat::New();
	// mark existance of the block with empty disk-blob pointer
	const int BlockId = BlockBlobPtV.Add(TBlobPt()) + FirstBlockOffset;
	// add it to cache
	BlockCache.Put(BlockId, BlockDat);
	// update cache size increase with the size of cache key and empty block
	NewCacheSizeInc += sizeof(TInt) + BlockDat->GetMemUsed(); 
	// return id of the new block
	return BlockId;
}

template <class TVal>
int TWndBlockCache<TVal>::GetLastBlock(PBlockDat& BlockDat) {
	// ID of the last block in the queue
	const int LastBlockId = BlockBlobPtV.Len() - 1 + FirstBlockOffset;
	// retrieve block
	GetBlock(LastBlockId, BlockDat);
	// if full, make a new one
	if (BlockDat->IsFull(BlockSize)) {
		//creat new block
		const int NewBlockId = AddBlock();
		// return its reference
		GetBlock(NewBlockId, BlockDat);
		// and new block id
		return NewBlockId;
	}
	// just return existing last block id
	return LastBlockId;
}

template <class TVal>
void TWndBlockCache<TVal>::DelBlock() { 
	// get first block id
	const int FirstBlockId = FirstBlockOffset;
	// delete from cache
	BlockCache.Del(FirstBlockId, false);
	// delete from blob
	if (!BlockBlobPtV[0].Empty()) { 
		BlockBlobBs->DelBlob(BlockBlobPtV[0]);
	}
	// forget blob pointer
	BlockBlobPtV.Del(0);
}

template <class TVal>
TWndBlockCache<TVal>::TWndBlockCache(const TStr& _FNmPrefix, const int64& MxCacheMem, 
		const int& _BlockSize): BlockSize(_BlockSize), BlockCache(MxCacheMem, 1000000, GetVoidThis()) {

	// initialize storage parameters
	FNmPrefix = _FNmPrefix;
	Access = faCreate;
	// initialize cache parameters
	CacheResetThreshold = int64(0.1 * double(MxCacheMem));
	NewCacheSizeInc = 0;
	// initialize value disk store
	try {
		BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	} catch (...) { 
		// try with restor
		TMBlobBs::New(FNmPrefix + "BlobBs", faRestore);
		// open it then in a normal fashion
		BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	}
	// create first block
	EAssertR(AddBlock() == 0, "Error creating first cache block");
}

template <class TVal>
TWndBlockCache<TVal>::TWndBlockCache(const TStr& _FNmPrefix, const TFAccess& _Access,
		const int64& MxCacheMem): BlockCache(MxCacheMem, 1000000, GetVoidThis()) {

	// initialize storage parameters
	FNmPrefix = _FNmPrefix;
	Access = _Access;
	// initialize cache parameters
	CacheResetThreshold = int64(0.1 * double(MxCacheMem));
	NewCacheSizeInc = 0;
	// initialize value disk store
	BlockBlobBs = TMBlobBs::New(FNmPrefix + "BlobBs", Access);
	// make sure we are not trying to create
	EAssertR(Access != faCreate, "First call create constructor!");
	// load BlockId map and BlockSize
	TFIn FIn(FNmPrefix + ".Dat");
	Vals.Load(FIn);
	BlockSize.Load(FIn);
	BlockBlobPtV.Load(FIn);		
	FirstBlockOffset.Load(FIn);
	FirstValOffset.Load(FIn);
}

template <class TVal>
TWndBlockCache<TVal>::~TWndBlockCache() {
	if ((Access == faCreate) || (Access == faUpdate)) {
		// flush all the latest changes in cache to the disk		
		BlockCache.Flush();
		// save the rest to FNmPrefix + ".Dat"
		TFOut FOut(FNmPrefix + ".Dat");		    
		Vals.Save(FOut);
		BlockSize.Save(FOut);
		BlockBlobPtV.Save(FOut);
		FirstBlockOffset.Save(FOut);
		FirstValOffset.Save(FOut);
	}
}

template <class TVal>
uint64 TWndBlockCache<TVal>::AddVal(const TVal& Val) {
	// get last block, with some space left
	PBlockDat BlockDat; 
	const int BlockId = GetLastBlock(BlockDat);
	// add the value to the block
	const int BlockValId = BlockDat->AddVal(Val);
	// update cache increase count
	NewCacheSizeInc += BlockDat->GetVal(BlockValId).GetMemUsed();
	// check if we have to drop anything from the cache
	if (NewCacheSizeInc > CacheResetThreshold) {
		// report on the size increase
		printf("Cache clean-up [%s] ... ", TUInt64::GetMegaStr(NewCacheSizeInc).CStr());
		BlockCache.RefreshMemUsed();
		printf("Done\n");
		// reset increase counter
		NewCacheSizeInc = 0; 
	}
	// increase count of all values
	Vals++;
	// return value id
	return GetValId(BlockId, BlockValId); 
}

template <class TVal>
void TWndBlockCache<TVal>::SetVal(const uint64& ValId, const TVal& Val) {
	// transform to block ids
	int BlockId = -1, BlockValId = -1;
	GetBlockId(ValId, BlockId, BlockValId);
	// get the block
	PBlockDat BlockDat;
	GetBlock(BlockId, BlockDat);
	BlockDat->SetVal(BlockValId, Val);
}

template <class TVal>
uint64 TWndBlockCache<TVal>::GetFirstValId() const { 
	return (uint64)BlockSize * (uint64)FirstBlockOffset + (uint64)FirstValOffset;
}

template <class TVal>
uint64 TWndBlockCache<TVal>::GetLastValId() const { 
	return GetFirstValId() + Len() - 1; 
}

template <class TVal>
bool TWndBlockCache<TVal>::IsValId(const uint64& ValId) const { 
	// transfor to block ids
	int BlockId = -1, BlockValId = -1;
	GetBlockId(ValId, BlockId, BlockValId);
	// check if block exists
	if (BlockId < FirstBlockOffset) { return false; }
	if (BlockId >= (BlockBlobPtV.Len() + FirstBlockOffset)) { return false; }
	// if in first block, check if not before offset
	if (BlockId == FirstBlockOffset && BlockValId < FirstValOffset) { return false; }
	// check if value exists
	PBlockDat BlockDat; GetBlock(BlockId, BlockDat);
	return BlockDat->IsValId(BlockValId);
}

template <class TVal>
uint64 TWndBlockCache<TVal>::GetFirstVal(TVal& Val) const {
	int BlockId = FirstBlockOffset, BlockValId = FirstValOffset;
	// get the block
	PBlockDat BlockDat;
	GetBlock(BlockId, BlockDat);
	// get the val
	Val = BlockDat->GetVal(BlockValId);
	return (uint64)FirstBlockOffset * (uint64)BlockSize + (uint64)FirstValOffset;
}

template <class TVal>
void TWndBlockCache<TVal>::GetVal(const uint64& ValId, TVal& Val) const {
	// transfor to block ids
	int BlockId = -1, BlockValId = -1;
	GetBlockId(ValId, BlockId, BlockValId);
	// get the block
	PBlockDat BlockDat;
	GetBlock(BlockId, BlockDat);
	// get the val
	Val = BlockDat->GetVal(BlockValId);
}

template <class TVal>
bool TWndBlockCache<TVal>::DelVal() {		
	// return if nothing to delete
	if (Empty()) { return false; }
	// move value offset to one up
	FirstValOffset++; Vals--;
	// if we reach end of block, increment the block offset
	if (FirstValOffset >= BlockSize) {
		// forget the oldest block
		DelBlock();
		FirstBlockOffset++;
		FirstValOffset = 0;
	}
	// success
	return true;
}

template <class TVal>
int TWndBlockCache<TVal>::DelVals(const int& _Vals) {
	int DeletedVals = 0;
	for (int ValN = 0; ValN < _Vals; ValN++) {
		// delete while we can
		if (!DelVal()) { break; }
		DeletedVals++;
	}
	// return number of deleted values
	return DeletedVals;
}

#endif
