/////////////////////////////////////////////////
// Forward
ClassHdTP(TWix, PWix);

/////////////////////////////////////////////////
// Word-Inverted-Index-Character-Definition
ClassTP(TWixChDef, PWixChDef)//{
private:
  TStr ChSetStr;
  TStrV Bit4CdToStrV;
  TIntV ChToBit4CdV;
  TChV Bit6CdToChV;
  TIntV ChToBit6CdV;
  TChV UcChV;
  TCh MnCh;
  TCh MxCh;
  int* ChToBit4CdT;
  char* Bit6CdToChT;
  int* ChToBit6CdT;
  char* UcChT;
  void AddBit4CdStr(const TStr& Str);
  void AddBit6CdCh(const char& Ch);
  void AddChSet(const TStr& Str);
  void SetUcCh(const TStr& Str);
  void SetMnMxCh();
  void ConvVecToTb();
public:
  TWixChDef();
  ~TWixChDef();
  TWixChDef(TSIn& SIn):
    ChSetStr(SIn),
    Bit4CdToStrV(SIn), ChToBit4CdV(SIn),
    Bit6CdToChV(SIn), ChToBit6CdV(SIn),
    UcChV(SIn), MnCh(SIn), MxCh(SIn){ConvVecToTb();}
  static TPt<TWixChDef> Load(TSIn& SIn){return new TWixChDef(SIn);}
  void Save(TSOut& SOut){
    ChSetStr.Save(SOut);
    Bit4CdToStrV.Save(SOut); ChToBit4CdV.Save(SOut);
    Bit6CdToChV.Save(SOut); ChToBit6CdV.Save(SOut);
    UcChV.Save(SOut); MnCh.Save(SOut); MxCh.Save(SOut);}

  TWixChDef& operator=(const TWixChDef&){Fail; return *this;}

  TStr GetChSet() const {return ChSetStr;}
  TStr GetBit6Str(const int& Cd) const {return Bit4CdToStrV[Cd];}
  int GetBit4VCd(const char& Ch) const {return ChToBit4CdV[Ch-TCh::Mn];}
  char GetBit6VCh(const int& Cd) const {return Bit6CdToChV[Cd];}
  int GetBit6VCd(const char& Ch) const {return ChToBit6CdV[Ch-TCh::Mn];}
  char GetUcVCh(const char& Ch) const {return UcChV[Ch-TCh::Mn];}
  char GetMnCh() const {return MnCh;}
  char GetMxCh() const {return MxCh;}

  int GetBit4TCd(const char& Ch) const {return ChToBit4CdT[Ch-TCh::Mn];}
  char GetBit6TCh(const int& Cd) const {return Bit6CdToChT[Cd];}
  int GetBit6TCd(const char& Ch) const {return ChToBit6CdT[Ch-TCh::Mn];}
  char GetUcTCh(const char& Ch) const {return UcChT[Ch-TCh::Mn];}

  void GetNrWordChA(const TChA& WordChA, bool& IsWordOk, TChA& NrWordChA) const;
  bool IsNrWordChA(const TChA& NrWordChA) const;

  void SaveTxt(TOLx& Lx) const;
};

/////////////////////////////////////////////////
// Word-Inverted-Index-Coded-Array
class TWixCdA{
private:
  TB32Set BSet1, BSet2, BSet3;
public:
  TWixCdA(){}
  TWixCdA(const TChA& NrWordChA, const TWixChDef& ChDef);
  ~TWixCdA(){}
  TWixCdA(TSIn& SIn):
    BSet1(SIn), BSet2(SIn), BSet3(SIn){}
  void Save(TSOut& SOut){
    BSet1.Save(SOut); BSet2.Save(SOut); BSet3.Save(SOut);}

  TWixCdA& operator=(const TWixCdA& Word){
    if (this!=&Word){BSet1=Word.BSet1; BSet2=Word.BSet2; BSet3=Word.BSet3;}
    return *this;}
  bool operator==(const TWixCdA& Word) const {
    return (BSet1==Word.BSet1)&&(BSet2==Word.BSet2)&&(BSet3==Word.BSet3);}
  bool operator<(const TWixCdA& Word) const {
    return (BSet1<Word.BSet1)||((BSet1==Word.BSet1)&&(BSet2<Word.BSet2))||
     ((BSet1==Word.BSet1)&&(BSet2==Word.BSet2)&&(BSet3<Word.BSet3));}

  int GetBit6Cd(const int& ChN) const;
  bool IsPrefix(const int& Chs, const TWixCdA& CdA) const;
  void GetNrWordChA(const TWixChDef& ChDef, TChA& NrWordChA) const;

  int GetPrimHashCd() const;
  int GetSecHashCd() const;
};
typedef THash<TWixCdA, TVoid> TWixCdAH;

/////////////////////////////////////////////////
// Word-Inverted-Index-Document-Id-Set
ClassTPV(TWixDocIdSet, PWixDocIdSet, TWixDocIdSetV)//{
private:
  bool Modified;
private:
  TWix* Wix;
  TInt WordId;
  TBlobPtV DocIdV;
public:
  TWixDocIdSet(TWix* _Wix, const int & _WordId, const int& DocIds):
    Modified(false), Wix(_Wix), WordId(_WordId), DocIdV(DocIds, 0){}
  ~TWixDocIdSet(){}
  TWixDocIdSet(TSIn& SIn, TWix* _Wix, const int & _WordId):
    Modified(false), Wix(_Wix), WordId(_WordId), DocIdV(SIn){}
  static TPt<TWixDocIdSet> Load(TSIn&){Fail; return NULL;}
  void Save(TSOut& SOut){
    DocIdV.Save(SOut);}

  TWixDocIdSet& operator=(const TWixDocIdSet&){Fail; return *this;}
  int GetMemUsed(){
    return sizeof(TWix*)+sizeof(int)+DocIdV.GetMemUsed();}

  void PutModified(const bool& _Modified){Modified=_Modified;}
  int GetWordId(){return WordId;}

  int GetDocIds();
  void AddDocId(const TBlobPt& DocId, const bool& DoIncDocIds=true);

  void GetDocIdV(TBlobPtV& _DocIdV){_DocIdV=DocIdV;}
  void AndDocIdV(TBlobPtV& AndDocIdV){AndDocIdV.Intrs(DocIdV);}
  void OrDocIdV(TBlobPtV& OrDocIdV){OrDocIdV.Union(DocIdV);}

  void OnDelFromCache(const TBlobPt& BlobPt, void*);

  void SaveTxt(TOLx& Lx, const TWixChDef& ChDef);
};
typedef THash<TBlobPt, PWixDocIdSet> TBPtDocIdSetH;

/////////////////////////////////////////////////
// Word-Inverted-Index-Word
class TWixWord{
private:
  TWixCdA CdA;
  TInt DocIds;
  TInt Next;
  TBlobPt BPt1, BPt2, BPt3, BPt4;
public:
  TWixWord(){}
  TWixWord(const TWixCdA& _CdA):
    CdA(_CdA), DocIds(0), Next(-1),
    BPt1(), BPt2(), BPt3(), BPt4(){}
  TWixWord(const TStr& WordStr, const TWixChDef& ChDef):
    CdA(WordStr, ChDef), DocIds(0), Next(-1),
    BPt1(), BPt2(), BPt3(), BPt4(){}
  ~TWixWord(){}
  TWixWord(TSIn& SIn):
    CdA(SIn), DocIds(SIn), Next(SIn),
    BPt1(SIn), BPt2(SIn), BPt3(SIn), BPt4(SIn){}
  void Save(TSOut& SOut){
    CdA.Save(SOut); DocIds.Save(SOut); Next.Save(SOut);
    BPt1.Save(SOut); BPt2.Save(SOut); BPt3.Save(SOut); BPt4.Save(SOut);}

  TWixWord& operator=(const TWixWord& Word){
    if (this!=&Word){
      CdA=Word.CdA; DocIds=Word.DocIds; Next=Word.Next;
      BPt1=Word.BPt1; BPt2=Word.BPt2; BPt3=Word.BPt3; BPt4=Word.BPt4;}
    return *this;}
  bool operator==(const TWixWord& Word) const {return CdA==Word.CdA;}
  bool operator<(const TWixWord& Word) const {return CdA<Word.CdA;}

  TStr GetNrWordStr(const TWixChDef& ChDef){
    TChA NrWordChA; CdA.GetNrWordChA(ChDef, NrWordChA); return NrWordChA;}
  void GetNrWordChA(const TWixChDef& ChDef, TChA& NrWordChA){
    CdA.GetNrWordChA(ChDef, NrWordChA);}
  bool IsPrefixChA(const TChA& NrWordChA, const TWixCdA& NrWordCdA){
    return CdA.IsPrefix(NrWordChA.Len(), NrWordCdA);}

  int GetDocIds(){return DocIds;}
  void IncDocIds(){DocIds++;}
  void AddDocId(const TBlobPt& DocId);

  int GetNext(){return Next;}
  void PutNext(const int& _Next){Next=_Next;}

  PWixDocIdSet GetDocIdSet(TWix* Wix, const int& WordId);
  void PutDocIdSetBPt(const TBlobPt& DocIdSetBPt);
  TBlobPt GetDocIdSetBPt();
};
typedef TVec<TWixWord> TWixWordV;

/////////////////////////////////////////////////
// Word-Inverted-Index
ClassTP(TWix, PWix)//{
public:
  // word port vector constants
  static const int& WordPortVIxChs;
  static const int& WordPortVBitsPerCh;
  static const int& WordPortVLen;
  // word representation constants
  static const int& MxChsPerWord;
  static const int& MxDocIdsInWord;
  // file name constants
  static const TStr WixFExt;
  static const TStr WixDocIdFExt;
  // blob-pointer flag-number constants
  static const int InTitle_BPtFlagN;
  static const int InA_BPtFlagN;
  static const int WordFqMsb_BPtFlagN;
  static const int WordFqLsb_BPtFlagN;
private:
  TStr WixFNm;
  TStr WixDocIdFNm;
  TFAccess Access;
  PWixChDef ChDef;
  TWixCdAH SwCdAH;
  TIntV WordPortV;
  TWixWordV WordV;
  PBlobBs DocIdBBs;
  TCache<TBlobPt, PWixDocIdSet> DocIdSetCache;

  // how much data must we read before we recheck the cache and calculate new size
  int64 CacheResetThreshold; 
  // current additions to the cache before last clean-up
  int64 NewCacheSizeInc;
    
  void* GetVoidThis() const {return (void*)this;}  
  int GetWordPortN(const TChA& WordChA, const bool& DoMaximize);
  int GetWordPortN(const TWixCdA& WordCdA);
  int GetWordId(const TWixCdA& WordCdA, const bool& CreateIfNo);
  int AddDocId(const int& WordId, const TBlobPt& DocId);
private:
  typedef TPair<TInt, TBlobPt> TFqDocIdPr;
  THash<TWixCdA, TFqDocIdPr> WordCdA_FqDocIdPrH;
public:
  TWix(const TStr& Nm, const TStr& FPath=TStr(),
   const TFAccess& _Access=faRdOnly, const int64& CacheSize=100000000);
  static PWix New(const TStr& Nm, const TStr& FPath=TStr(),
    const TFAccess& Access=faRdOnly, const int64& CacheSize=100000000) {
     return PWix(new TWix(Nm, FPath, Access, CacheSize)); }
  ~TWix();
  TWix(TSIn&){Fail;}
  static PWix Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TWix& operator=(const TWix&){Fail; return *this;}
  int64 GetMemUsed(){
    return int64(WordPortV.GetMemUsed())+int64(WordV.GetMemUsed())+DocIdSetCache.GetMemUsed();}
  TStr GetMemUsedStr();
  void CacheFlushAndClr(){DocIdSetCache.FlushAndClr();}

  TWixChDef& GetChDef(){return *ChDef;}
  PBlobBs GetDocIdBBs(){return DocIdBBs;}

  void AddHtmlDoc(const TBlobPt& DocId, const PSIn& SIn);

  PWixDocIdSet GetDocIdSet(const int& WordId);
  PWixDocIdSet GetDocIdSet(const TWixCdA& WordCdA);

  void GetDocIdV(const TChA& WordChA, const bool& IsExactMatch,
   const int& MxWcWords, TBlobPtV& DocIdV);
  TWixWord& GetWord(const int& WordId){return WordV[WordId];}

  int GetWords() const {return WordV.Len();}
  TStr GetWordStr(const int& WordId){
    return GetWord(WordId).GetNrWordStr(*ChDef);}
  int GetWordFq(const int& WordId){
    return GetWord(WordId).GetDocIds();}
  void GetFqWordV(TIntStrPrV& FqWordV);

  void SaveTxt(const TStr& FNm){SaveTxt(PSOut(new TFOut(FNm)));}
  void SaveTxt(const PSOut& SOut);
  void SaveTxtWordFq(const TStr& FNm){SaveTxtWordFq(PSOut(new TFOut(FNm)));}
  void SaveTxtWordFq(const PSOut& SOut);

  static void GetFNms(
   const TStr& Nm, const TStr& FPath,
   TStr& WixFNm, TStr& WixDocIdFNm);
  static bool Exists(const TStr& Nm, const TStr& FPath=TStr());
  static void Del(const TStr& Nm, const TStr& FPath=TStr());

  friend class TWixWordSet;
};

