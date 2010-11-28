#ifndef SEARCH_H
#define SEARCH_H

/////////////////////////////////////////////////
// WdGix-Constants
class TGixConst {
public:
    static TStr WdGixFNm;
    static TStr WdGixDatFNm;
    static TStr WdGixMDSFNm;
    static TStr WdGixBsFNm;
    static TStr TrGixFNm;
    static TStr TrGixDatFNm;
    static TStr TrGixDocBsFNm;
    static TStr TrGixSentBsFNm;
    static TStr TrGixTrAttrBsFNm;
    static TStr MWdGixFNm;
    static TStr MWdGixDatFNm;
    static TStr MWdGixDocBsFNm;
    static TStr MWdGixBsFNm;
};

/////////////////////////////////////////////////
// Word-Inverted-Index-Item
#pragma pack(push, 1) // pack class size
ClassTV(TWdGixItem, TWdGixItemV)//{
private:
    static int TitleBit;
    static int NmObjBit;
    static int AnchorBit;
    static int EmphBit;

private:
    // TBlobPt fields
    uchar Seg;
    uint Addr;
    // bytes for extra info
    uchar Wgt; // external weight
    uchar WdPos; // position in document modulo 256
    TB8Set FSet; // flags for verious predefined stuff

public:
    TWdGixItem() { }
    TWdGixItem(const TBlobPt& BlobPt, const uchar& _Wgt, const uchar& _WdPos,
        const bool& TitleP, const bool& NmObjP, const bool& AnchorP, const bool& EmphP);
    TWdGixItem(const uchar& _Seg, const uint& _Addr, const uchar& _Wgt,
        const uchar& _WdPos, const bool& TitleP, const bool& NmObjP,
        const bool& AnchorP, const bool& EmphP);
    TWdGixItem(TSIn& SIn);
    void Save(TSOut& SOut) const;

    // for sorting and merging during search
    bool operator==(const TWdGixItem& Item) const;
    bool operator<(const TWdGixItem& Item) const;

    // pointer
    uchar GetSeg() const { return Seg; }
    uint GetAddr() const { return Addr; }
    TBlobPt GetBlobPt() const { return TBlobPt(Seg, Addr); }
    // weight assigned to the document
    uchar GetWgt() const { return Wgt; }
    // position in document
    uchar GetWdPos() const { return WdPos; }
    // word characteristics
    bool IsTitle() const { return FSet.GetBit(TitleBit); }
    bool IsNmObj() const { return FSet.GetBit(NmObjBit); }
    bool IsAnchor() const { return FSet.GetBit(AnchorBit); }
    bool IsEmph() const { return FSet.GetBit(EmphBit); }
};
#pragma pack(pop)

/////////////////////////////////////////////////
// Word-Inverted-Index
ClassTP(TWdGix, PWdGix)//{
private:
    typedef TInt TWdGixKey;
    typedef TGixItemSet<TWdGixKey, TWdGixItem> TWGixItemSet;
    typedef TPt<TWGixItemSet> PWGixItemSet;
    typedef TGix<TWdGixKey, TWdGixItem> TWGix;
    typedef TPt<TWGix> PWGix;
    typedef TGixExpItem<TWdGixKey, TWdGixItem> TWGixExpItem;
    typedef TPt<TWGixExpItem> PWGixExpItem;

private:
    TFAccess FAccess; // what is the current policy
    TStr FPath; // path opend under
    // word processing
    PStemmer Stemmer;
    PSwSet SwSet;
    // hash table holding words
    TStrHash<TInt> WordH;
    // inverted index points from word ids to documents
    PWGix WGix;

    // tags
    TStrH TitleTagH;
    TStrH NmObjTagH;
    TStrH EmphTagH;

    void LoadTags();

public:
    TWdGix(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize);
    static PWdGix New(const TStr& FPath, const TFAccess& FAccess,
        const int64& CacheSize) { return new TWdGix(FPath, FAccess, CacheSize); }

    ~TWdGix();

    // parses and addes an HTML string into the index
    void AddHtml(const TStr& DocStr, const TBlobPt& BlobPt, const uchar& Wgt = 0);
    // retrieves messages from the index that match the given query
    bool Search(const TStr& QueryStr, TWdGixItemV& ResItemV);

};

/////////////////////////////////////////////////
// WdGix-Meta-Data-Store
ClassTP(TWdGixMDS, PWdGixMDS)//{
private:
    typedef TPair<TUCh, TUInt> TAddrPr;

private:
    THash<TAddrPr, TUInt64> AddrPrToDateH;

public:
    TWdGixMDS() { }
    static PWdGixMDS New() { return new TWdGixMDS; }
    TWdGixMDS(TSIn& SIn) { AddrPrToDateH.Load(SIn); }
    static PWdGixMDS Load(TSIn& SIn) { return new TWdGixMDS(SIn); }
    void Save(TSOut& SOut) const { AddrPrToDateH.Save(SOut); }

    void AddDate(const TBlobPt& DocBlobPt, const TTm& DateTime);
    uint64 GetDateMSecs(const TBlobPt& DocBlobPt) const;
    TTm GetDateTTm(const TBlobPt& DocBlobPt) const;

    static PWdGixMDS LoadBin(const TStr& FNm) {
        TFIn FIn(FNm); return new TWdGixMDS(FIn); }
    void SaveBin(const TStr& FNm) const { TFOut FOut(FNm); Save(FOut); }
};

/////////////////////////////////////////////////
// WdGix-Result-Set
ClassTP(TWdGixRSet, PWdGixRSet)//{
private:
    TStr QueryStr;
    TInt AllDocs;
    TInt Offset;
    TStrV DocTitleV;
    TStrV DocStrV;
    TVec<TStrV> CatNmVV;
    TTmV DateTimeV;

public:
    TWdGixRSet(const TStr& _QueryStr, const int& _AllDocs, const int& _Offset):
      QueryStr(_QueryStr), AllDocs(_AllDocs), Offset(_Offset) { };
    static PWdGixRSet New(const TStr& QueryStr, const int& AllDocs,
      const int& Offset) { return new TWdGixRSet(QueryStr, AllDocs, Offset); }

    // adding documents to result set, assumes they are
    // added by rank starting with first
    void AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStrV& CatNmV, const TTm& DateTime = TTm());
    // query parameters
    TStr GetQueryStr() const { return QueryStr; }
    int GetAllDocs() const { return AllDocs; }
    int GetOffset() const { return Offset; }
    // access to documents
    int GetDocs() const { return DocStrV.Len(); }
    const TStr& GetDocTitle(const int DocN) const { return DocTitleV[DocN]; }
    const TStr& GetDocStr(const int DocN) const { return DocStrV[DocN]; }
    const TStrV& GetDocStrV() const { return DocStrV; }
    const TStrV& GetDocCatNmV(const int DocN) const { return CatNmVV[DocN]; }
    const TTm& GetDocDateTime(const int DocN) const { return DateTimeV[DocN]; }

    // manipulation
    void SortByDate(const bool& Asc = true);

    // result output
    void PrintRes(PNotify Notify = TStdNotify::New());
    PBowDocBs GenBowDocBs() const;
};

/////////////////////////////////////////////////
// WdGix-Ranking-Function
class TWdGixRankFun {
public:
    virtual ~TWdGixRankFun(){}
    virtual double operator()(const TTm& DateTime, const int& Wgt,
        const int& Count, const bool& TitleP, const bool& NmObjP,
        const bool& AnchorP, const bool& EmphP) = 0;
};

class TWdGixRankFunTime: public TWdGixRankFun {
public:
    TWdGixRankFunTime() { }

    double operator()(const TTm& DateTime, const int& Wgt,
        const int& Count, const bool& TitleP, const bool& NmObjP,
        const bool& AnchorP, const bool& EmphP) {

            return double(TTm::GetMSecsFromTm(DateTime));
    }
};

class TWdGixRankFunWgt: public TWdGixRankFun {
public:
    TWdGixRankFunWgt() { }

    double operator()(const TTm& DateTime, const int& Wgt,
        const int& Count, const bool& TitleP, const bool& NmObjP,
        const bool& AnchorP, const bool& EmphP) {

            return double(Wgt);
    }
};

class TWdGixRankFunCount: public TWdGixRankFun {
public:
    TWdGixRankFunCount() { }

    double operator()(const TTm& DateTime, const int& Wgt,
        const int& Count, const bool& TitleP, const bool& NmObjP,
        const bool& AnchorP, const bool& EmphP) {

            return double(Count);
    }
};

class TWdGixRankFunRnd: public TWdGixRankFun {
private:
    TRnd Rnd;
public:
    TWdGixRankFunRnd(const int& Seed = 1): Rnd(Seed) { }

    double operator()(const TTm& DateTime, const int& Wgt,
        const int& Count, const bool& TitleP, const bool& NmObjP,
        const bool& AnchorP, const bool& EmphP) {

            return Rnd.GetUniDev();
    }
};

/////////////////////////////////////////////////
// WdGix-Base
typedef enum { wgbgNone, wgbgName, wgbgDate, wgbgDateTime } TWdGixBsGrouping;

ClassTP(TWdGixBs, PWdGixBs)//{
private:
    typedef TKeyDat<TFlt, TWdGixItem> TWgtWdGixItemKd;
    typedef TVec<TWgtWdGixItemKd> TWgtWdGixItemKdV;
    //typedef THeap<TWgtWdGixItemKd> TWgtWdGixItemKdHp;

private:
    TFAccess FAccess; // what is the current policy
    TStr FPath; // path opend under
    PBlobBs DocBBs; // store of original texts
    PWdGixMDS WdGixMDS; // store of metadata
    PWdGix WdGix; // inverted index maping words to documents

    void Filter(const TWgtWdGixItemKdV& InItemV,
        const TWdGixBsGrouping& Grouping,
        TWgtWdGixItemKdV& OutItemV);

public:
    TWdGixBs(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize);
    static PWdGixBs New(const TStr& FPath, const TFAccess& FAccess,
        const int64& CacheSize) { return new TWdGixBs(FPath, FAccess, CacheSize); }

    ~TWdGixBs();

    // adds document do blob, gix and meta-data store
    void AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStrV& CatNmV = TStrV(), const TTm& DateTime = TTm(),
        const uchar& Wgt = 0);
    void AddDoc(const TStr& DocTitle, const TStr& DocStoreStr,
        const TStr& DocIndexStr, const TStrV& CatNmV = TStrV(),
        const TTm& DateTime = TTm(), const uchar& Wgt = 0);
    // get document from document base
    void GetDoc(const TBlobPt& BlobPt, TStr& DocTitle,
        TStr& DocStr, TStrV& CatNmV) const;
    TStr GetDocTitle(const TBlobPt& BlobPt) const;
    TStr GetDocStr(const TBlobPt& BlobPt) const;
    TStrV GetDocCatNmV(const TBlobPt& BlobPt) const;
    // search
    PWdGixRSet SearchDoc(
        const TStr& QueryStr, const TWdGixBsGrouping& Grouping /*= wgbgNone*/,
        TWdGixRankFun& RankFun /*= TWdGixRankFunTime()*/, const int& Docs = -1,
        const int& Offset = 0, const TTm& MnDate = TTm(), const TTm& MxDate = TTm());

    // special supported formats
    void AddReuters(const TStr& XmlFNm);
    void IndexReuters(const TStr& FPath);
    void AddNmEn(const TStr& XmlFNm);
    void IndexNmEnBs(const TStr& FNm);
    void IndexNyt(const TStr& XmlFNm);
};

/////////////////////////////////////////////////
// Search-Topics
ClassTP(TSearchTopics, PSearchTopics)//{
public:
    TStrV TopicV;
    TStrV FrameV;
    TFltVV TopicFrameFqVV;

public:
    TSearchTopics(PWdGixRSet RSet, const int& Topics);
    static PSearchTopics New(PWdGixRSet RSet, const int& Topics) {
        return new TSearchTopics(RSet, Topics); }

};

/////////////////////////////////////////////////
// Triplet-Inverted-Index-Item
ClassTV(TTrGixItem, TTrGixItemV)//{
private:
    typedef struct {
        unsigned int Subject:2;
        unsigned int Predicat:2;
        unsigned int Object:2;
    } TTrGixItemMerge;

    typedef struct {
        unsigned int Type:4;
        unsigned int Pos:4;
        unsigned int Full:1;
        unsigned int Stem:1;
        unsigned int Hyper:4;
    } TTrGixItemWdInfoBits;

    typedef union {
        TTrGixItemWdInfoBits Bits;
        short Short;
    } TTrGixItemWdInfo;

private:
    TTrGixItemWdInfo GetWdInfo() const {
        TTrGixItemWdInfo Info; Info.Short = WdInfo; return Info; }

    void ClrMergeInfo() { /*MergeInfo.Subject = MergeInfo.Predicat = MergeInfo.Object = 0;*/ }

private:
    // TBlobPt fields
    uchar Seg;
    uint Addr;
    // tripelt info
    int SubjectId;
    int PredicatId;
    int ObjectId;
    // indexed word info
    int WdId;
    short WdInfo;

    //// merging info
    //TTrGixItemMerge MergeInfo;

public:
    TTrGixItem() { ClrMergeInfo(); }
    TTrGixItem(const TBlobPt& BlobPt, const int& _SubjectId, const int& _PredicatId,
        const int& _ObjectId, const int& _WdId, const uchar& Type, const uchar& Pos,
        const bool& Full, const bool& Stem, const uchar& Hyper);
    TTrGixItem(TSIn& SIn);
    void Save(TSOut& SOut) const;

    // for sorting and merging during search
    bool operator==(const TTrGixItem& Item) const;
    bool operator<(const TTrGixItem& Item) const;

    // pointer
    uchar GetSeg() const { return Seg; }
    uint GetAddr() const { return Addr; }
    TBlobPt GetBlobPt() const { return TBlobPt(Seg, Addr); }
    // triplet
    int GetSubjectId() const { return SubjectId; }
    int GetPredicatId() const { return PredicatId; }
    int GetObjectId() const { return ObjectId; }
    TIntTr GetIdTr() const { return TIntTr(SubjectId, PredicatId, ObjectId); }
    // word
    void SetWordInfo(const uchar& Type, const uchar& Pos,
        const bool& Full, const bool& Stem, const uchar& Hyper);
    int GetWdId() const { return WdId; }
    uchar GetWdType() const { return GetWdInfo().Bits.Type; }
    uchar GetWdPos() const { return GetWdInfo().Bits.Pos; }
    bool GetWdFull() const { return (GetWdInfo().Bits.Full == 1); }
    bool GetWdStem() const { return (GetWdInfo().Bits.Stem == 1); }
    uchar GetWdHyper() const { return GetWdInfo().Bits.Hyper; }
};

/////////////////////////////////////////////////
// Triplet-Inverted-Index
ClassTP(TTrGix, PTrGix)//{
private:
    typedef TIntChPr TTrGixKey;
    typedef TGixItemSet<TTrGixKey, TTrGixItem> TTGixItemSet;
    typedef TPt<TTGixItemSet> PTGixItemSet;
    typedef TGix<TTrGixKey, TTrGixItem> TTGix;
    typedef TPt<TTGix> PTGix;
    typedef TGixExpItem<TTrGixKey, TTrGixItem> TTGixExpItem;
    typedef TPt<TTGixExpItem> PTGixExpItem;

    static char SubjectType;
    static char SubjectWdType;
    static char SubjectAttrWdType;
    static char SubjectStemType;
    static char SubjectAttrStemType;
    static char PredicatType;
    static char PredicatWdType;
    static char PredicatAttrWdType;
    static char PredicatStemType;
    static char PredicatAttrStemType;
    static char ObjectType;
    static char ObjectWdType;
    static char ObjectAttrWdType;
    static char ObjectStemType;
    static char ObjectAttrStemType;

private:
    TFAccess FAccess; // what is the current policy
    TStr FPath; // path opend under
    // word processing
    PStemmer Stemmer;
    // hash table holding words
    TStrHash<TInt> WordH;
    // inverted index points from word ids to documents
    PTGix Gix;

    void AddTrPart(const int& FullId, const char& Type, const int& SubjectId,
        const int& PredicatId, const int& ObjectId, const TBlobPt& BlobPt);
    void AddTrPart(const TIntPrV& IdPrV, const char& WdType, const char& StemType,
        const int& SubjectWId, const int& PredicatWId, const int& ObjectWId,
        const TBlobPt& BlobPt);

    // query for full string
    PTGixExpItem GetExactExp(const TStr& Str, const char& Type);
    // query for words or stems
    PTGixExpItem GetPartExp(const TStr& Str, const char& WdType, const char& StemType);

public:
    TTrGix(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize);
    static PTrGix New(const TStr& FPath, const TFAccess& FAccess,
        const int64& CacheSize) { return new TTrGix(FPath, FAccess, CacheSize); }

    ~TTrGix();

    // vocabulary
    int GetWordId(const TStr& WordStr, const bool& AddIfNotExistP);
    TStr GetWordStr(const int& WId) const;
    void GetWordIdV(const TStr& Str, TIntPrV& WordStemIdV,
        const bool& AddIfNotExistP);
    void GetWordIdV(const TStrV& WordStrV, TIntPrV& WordStemIdV,
        const bool& AddIfNotExistP);

    // adds triplet to the index
    void AddTr(const TStr& SubjectStr, const TStrV& SubjectAttrV,
        const TStr& PredicatStr, const TStrV& PredicatAttrV, const TStr& ObjectStr,
        const TStrV& ObjectAttrV, const TBlobPt& BlobPt);
    // retrieves triplets that exactly match the query from the index
    // empty strings indicates no constrain on that part of the triplet
    bool SearchExact(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, TTrGixItemV& ResItemV);
    // same as above, but sub-matches are possible as well
    //   example: Q="clinton" matches "president clinton"
    //            Q="bill clinton" does NOT match "president clinton"
    bool SearchPart(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, TTrGixItemV& ResItemV, const bool& IncExactP);
};

/////////////////////////////////////////////////
// TrGix-Result-Set
ClassTP(TTrGixRSet, PTrGixRSet)//{
private:
    // query
    TStr SubjectStr;
    TStr PredicatStr;
    TStr ObjectStr;
    // results
    TInt AllTrs;
    TInt Offset;
    TStrTrV TrStrV;
    TVec<TBlobPtV> TrAttrBlobPtVV;

public:
    TTrGixRSet(const TStr& _SubjectStr, const TStr& _PredicatStr,
        const TStr& _ObjectStr, const int& _AllTrs, const int& _Offset):
            SubjectStr(_SubjectStr), PredicatStr(_PredicatStr),
            ObjectStr(_ObjectStr), AllTrs(_AllTrs), Offset(_Offset) { };
    static PTrGixRSet New(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, const int& AllTrs, const int& Offset) {
            return new TTrGixRSet(SubjectStr, PredicatStr, ObjectStr, AllTrs, Offset); }

    // adding triplets to the result set, assumes they are added by decreasing rank order
    void AddTr(const TStrTr& TrStr, const TBlobPtV& TrAttrBlobPtV);
    // query parameters
    TStr GetSubjectStr() const { return SubjectStr; }
    TStr GetPredicatStr() const { return PredicatStr; }
    TStr GetObjectStr() const { return ObjectStr; }
    int GetAllTrs() const { return AllTrs; }
    int GetOffset() const { return Offset; }
    // access to triplets
    bool Empty() const { return TrStrV.Empty(); }
    int GetTrs() const { return TrStrV.Len(); }
    int GetTrCount(const int& TrN) const { return TrAttrBlobPtVV[TrN].Len(); }
    const TStrTr& GetTr(const int& TrN) const { return TrStrV[TrN]; }
    const TStr& GetTrSubjectStr(const int& TrN) const { return TrStrV[TrN].Val1; }
    const TStr& GetTrPredicatStr(const int& TrN) const { return TrStrV[TrN].Val2; }
    const TStr& GetTrObjectStr(const int& TrN) const { return TrStrV[TrN].Val3; }
    // attributes pointer
    int GetTrAttrBlobPts(const int& TrN) const { return TrAttrBlobPtVV[TrN].Len(); }
    TBlobPt GetBlobPt(const int& TrN, const int& AttrBlobPtN) const {
        return TrAttrBlobPtVV[TrN][AttrBlobPtN]; }

    // simple API :-)
    bool IsSubjectFree() const { return SubjectStr.Empty(); }
    void GetSubjectV(TStrIntKdV& SubjectStrWgtV);
    bool IsPredicatFree() const { return PredicatStr.Empty(); }
    void GetPredicatV(TStrIntKdV& PredicatStrWgtV);
    bool IsObjectFree() const { return ObjectStr.Empty(); }
    void GetObjectV(TStrIntKdV& ObjectStrWgtV);

    // result output
    void PrintRes(const bool& PrintSentsP = false, PNotify Notify = TStdNotify::New()) const;
};

/////////////////////////////////////////////////
// TrGix-Ranking-Function
class TTrGixRankFun {
public:
    virtual double operator()(const int& Count) = 0;
};

class TTrGixRankFunCount: public TTrGixRankFun {
public:
    TTrGixRankFunCount() { }
    double operator()(const int& Count) { return double(Count); }
};

/////////////////////////////////////////////////
// TrGix-Merger
//class TTrGixMerger {
//public:
//    TTrGixMerger() { }
//
//    void Union(TTrGixItemV& MainV, const TTrGixItemV& JoinV) const;
//    void Minus(const TTrGixItemV& MainV, const TTrGixItemV& JoinV,
//        TTrGixItemV& ResV) const { MainV.Minus(JoinV, ResV); }
//    void Def(const TIntChPr& Key, TTrGixItemV& ItemV) const;
//};

/////////////////////////////////////////////////
// TrGix-Base
ClassTP(TTrGixBs, PTrGixBs)//{
private:
    TFAccess FAccess; // what is the current policy
    TStr FPath; // path opend under
    PBlobBs DocBBs; // store of original documents
    PBlobBs SentBBs; // store of original senteces
    PBlobBs TrAttrBBs; // store of original senteces
    PTrGix TrGix; // inverted index mapping parts of triplet to triplets

    void GetAttrV(PXmlTok XmlTok, TStrV& AttrV);

public:
    TTrGixBs(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize);
    static PTrGixBs New(const TStr& FPath, const TFAccess& FAccess,
        const int64& CacheSize) { return new TTrGixBs(FPath, FAccess, CacheSize); }

    ~TTrGixBs();

    // adds document to the blob
    TBlobPt AddDoc(const TStr& DocTitle, const TStr& DocStr = TStr(),
        const TStrV& CatNmV = TStrV());
    // adds sentence to blob
    TBlobPt AddSent(const TStr& SentStr);
    // adds triplet attributes to blob
    TBlobPt AddTrAttr(const TStr& SubjectStr, const TStrV& SubjectAttrV,
        const TStr& PredicatStr, const TStrV& PredicatAttrV, const TStr& ObjectStr,
        const TStrV& ObjectAttrV, const TBlobPt& SentBlobPt, const TBlobPt& DocBlobPt);
    // adds triple to index
    void AddTr(const TStr& SubjectStr, const TStrV& SubjectAttrV,
        const TStr& PredicatStr, const TStrV& PredicatAttrV, const TStr& ObjectStr,
        const TStrV& ObjectAttrV, const TBlobPt& TrAttrBlobPt);
    // get document from document base
    void GetDoc(const TBlobPt& DocBlobPt, TStr& DocTitle,
        TStr& DocStr, TStrV& CatNmV) const;
    TStr GetDocTitle(const TBlobPt& DocBlobPt) const;
    TStr GetDocStr(const TBlobPt& DocBlobPt) const;
    TStrV GetDocCatNmV(const TBlobPt& DocBlobPt) const;
    // get sentence from sentence base
    TStr GetSentStr(const TBlobPt& SentBlobPt);
    // get attributes of a triplet
    void GetTrAttr(const TBlobPt& TrAttrBlobPt, TStr& SubjectStr, TStrV& SubjectAttrV,
        TStr& PredicatStr, TStrV& PredicatAttrV, TStr& ObjectStr, TStrV& ObjectAttrV,
        TBlobPt& SentBlobPt, TBlobPt& DocBlobPt);

    // search
    PTrGixRSet SearchTr(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, TTrGixRankFun& RankFun /*= TTrGixRankFunCount()*/,
        const int& Docs = -1, const int& Offset = 0, const bool& ExactP = false);

    // special supported formats
    void AddReuters(const TStr& XmlFNm, int& Trs, const PSOut& CsvOut);
    void IndexReuters(const TStr& XmlFPath, const TStr& CsvFNm = "", const int& MxTrs = -1);
};


/////////////////////////////////////////////////
// Forward-Declarations
ClassHdTP(TMWdGix, PMWdGix);

/////////////////////////////////////////////////
// Multilingual-Word-Inverted-Index-Item
ClassTV(TMWdGixItem, TMWdGixItemV)//{
private:
    // TBlobPt fields for the document
    uchar Seg;
    uint Addr;
    // bytes for extra info
    uchar WdFq; // word frequency
    ushort DocWds; // number of words in the document
    // to be used only during mergings
    TFlt Wgt;

public:
    TMWdGixItem() { }
    TMWdGixItem(const TMWdGixItem& Item1, const TMWdGixItem& Item2):
        Seg(Item1.Seg), Addr(Item1.Addr), WdFq(0), DocWds(0),
        Wgt(Item1.GetWgt() + Item2.GetWgt()) { }
    TMWdGixItem(const TBlobPt& BlobPt, const uchar& _WdFq, const uchar& _DocWds):
        Seg(BlobPt.GetSeg()), Addr(BlobPt.GetAddr()), WdFq(_WdFq), DocWds(_DocWds) { }
    TMWdGixItem(const uchar& _Seg, const uint& _Addr, const uchar& _WdFq,
        const uchar& _DocWds): Seg(_Seg), Addr(_Addr), WdFq(_WdFq), DocWds(_DocWds) { }
    TMWdGixItem(TSIn& SIn);
    void Save(TSOut& SOut) const;

    // for sorting and merging during search
    bool operator==(const TMWdGixItem& Item) const;
    bool operator<(const TMWdGixItem& Item) const;

    // pointer
    uchar GetSeg() const { return Seg; }
    uint GetAddr() const { return Addr; }
    TBlobPt GetBlobPt() const { return TBlobPt(Seg, Addr); }
    // frequency of the word in the document
    uchar GetWdFq() const { return WdFq; }
    // number of all words in the document
    ushort GetDocWds() const { return DocWds; }

    // merge time stuff
    void PutWgt(const double& _Wgt) { Wgt = _Wgt; }
    double GetWgt() const { return Wgt; }
};

/////////////////////////////////////////////////
// WdGix-Ranking-Function
ClassTP(TMWdGixRankFun, PMWdGixRankFun)//{
public:
    virtual ~TMWdGixRankFun(){}
    // returns ranking of one word from query matching a document
    // - WdFq -- word frequency in the document
    // - DocWds -- number of all words in the document
    // - WdDocFq -- number of documents in which the word appears
    // - Docs -- number of documents in the corpus
    // - AvgDocWds -- average number of words in the document
    virtual double WdRank(const int& WdFq, const int& DocWds,
        const int& WdDocFq, const int& Docs, const double& AvgDocWds) const = 0;
};

class TMWdGixRankFunOkapiBM25 : public TMWdGixRankFun {
private:
    // Okapi BM-25 parameters
    TFlt k, b;
public:
    TMWdGixRankFunOkapiBM25(const double& _k = 2.0, const double _b = 0.75): k(_k), b(_b) { }
    static PMWdGixRankFun New(const double& k = 2.0, const double b = 0.75) {
        return new TMWdGixRankFunOkapiBM25(k, b); }

    double WdRank(const int& WdFq, const int& DocWds, const int& WdDocFq,
            const int& Docs, const double& AvgDocWds) const {

        //const double IDF = log((double(Docs) - double(WdDocFq) + 0.5) / (double(WdDocFq) + 0.5));
        //const double top = double(WdFq) * (k + 1.0);
        //const double bottom = double(WdFq) + k * (1 - b + b * (double(Docs) / AvgDocWds));
        //return IDF * top / bottom;

        if (WdDocFq == 0 || DocWds == 0) { return 0.0; }
        const double IDF = log(double(Docs) / double(WdDocFq));
        const double DocLenNm = AvgDocWds / double(DocWds);
        return IDF * DocLenNm * double(WdFq);
    }
};

/////////////////////////////////////////////////
// General-Inverted-Index-Default-Merger
class TMWdGixDefMerger {
private:
    PMWdGix MWdGix;
    PMWdGixRankFun RankFun;

public:
    TMWdGixDefMerger(PMWdGix _MWdGix, const PMWdGixRankFun& _RankFun):
      MWdGix(_MWdGix), RankFun(_RankFun) { }

    void Union(TMWdGixItemV& MainV, const TMWdGixItemV& JoinV) const;
    void Minus(const TMWdGixItemV& MainV, const TMWdGixItemV& JoinV,
        TMWdGixItemV& ResV) const { MainV.Minus(JoinV, ResV); }
    void Def(const TInt& Key, TMWdGixItemV& ItemV) const;
};


/////////////////////////////////////////////////
// Multilingual-Word-Inverted-Index
ClassTP(TMWdGix, PMWdGix)//{
private:
    typedef TInt TMWdGixKey;
    typedef TGixItemSet<TMWdGixKey, TMWdGixItem> TMWGixItemSet;
    typedef TPt<TMWGixItemSet> PMWGixItemSet;
    typedef TGix<TMWdGixKey, TMWdGixItem> TMWGix;
    typedef TPt<TMWGix> PMWGix;
    typedef TGixExpItem<TMWdGixKey, TMWdGixItem, TMWdGixDefMerger> TMWGixExpItem;
    typedef TPt<TMWGixExpItem> PMWGixExpItem;

private:
    TFAccess FAccess; // what is the current policy
    TStr FPath; // path opend under
    // hash table holding words
    TStrHash<TInt> WordH;
    // number of all documents in the index
    TInt AllDocs;
    // number of all the words in the index (including repetitions)
    TInt AllWords;
    // inverted index points from word ids to documents
    PMWGix MWGix;

public:
    TMWdGix(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize);
    static PMWdGix New(const TStr& FPath, const TFAccess& FAccess,
        const int64& CacheSize) { return new TMWdGix(FPath, FAccess, CacheSize); }

    ~TMWdGix();

    // parameters
    int GetWdFq(const int& WId) const { return WordH[WId]; }
    int GetAllDocs() const { return AllDocs; }
    int GetAllWords() const { return AllWords; }
    double GetAvgDocWds() const { return double(AllWords) / double(AllDocs); }

    // parses and addes an HTML string into the index
    void AddHtml(const TStr& DocStr, const TBlobPt& BlobPt);
    // retrieves messages from the index that match the given query
    bool Search(const TStr& QueryStr, TMWdGixItemV& ResItemV,
        const TMWdGixDefMerger& Merger);
};

/////////////////////////////////////////////////
// MWdGix-Result-Set
ClassTP(TMWdGixRSet, PMWdGixRSet)//{
private:
    // query parameters
    TStr QueryStr;
    TStr QueryLang;
    TStrStrH LangQueryH;
    TInt AllDocs;
    TInt Offset;
    // results parameteres
    TStrV DocTitleV;
    TStrV DocStrV;
    TStrV DocLangV;
    TVec<TStrV> KeyWdVV;

    TStr GetMainPara(const TStr& QueryStr, const TStr& FullStr);

public:
    TMWdGixRSet(const TStr& _QueryStr, const TStr& _QueryLang,
        const TStrStrH& _LangQueryH, const int& _AllDocs,
        const int& _Offset): QueryStr(_QueryStr), QueryLang(_QueryLang),
            LangQueryH(_LangQueryH), AllDocs(_AllDocs), Offset(_Offset) { };
    static PMWdGixRSet New(const TStr& QueryStr, const TStr& QueryLang,
        const TStrStrH& LangQueryH, const int& AllDocs, const int& Offset) {
            return new TMWdGixRSet(QueryStr, QueryLang, LangQueryH, AllDocs, Offset); }

    // adding documents to result set, assumes they are
    // added by rank starting with first
    void AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStr& DocLang, const TStrV& KeyWdV);
    // query parameters
    TStr GetQueryStr() const { return QueryStr; }
    TStr GetQueryLang() const { return QueryLang; }
    int GetAllDocs() const { return AllDocs; }
    int GetOffset() const { return Offset; }
    // access to documents
    int GetDocs() const { return DocStrV.Len(); }
    const TStr& GetDocTitle(const int DocN) const { return DocTitleV[DocN]; }
    const TStr& GetDocStr(const int DocN) const { return DocStrV[DocN]; }
    const TStr& GetDocLang(const int DocN) const { return DocLangV[DocN]; }
    const TStrV& GetDocKeyWdV(const int DocN) const { return KeyWdVV[DocN]; }

    // result output
    void PrintRes(PNotify Notify = TStdNotify::New());

    // serialize to XML for web service
    TStr GetWsXml(const TStrPrStrH& EurovocH) const;
};

/////////////////////////////////////////////////
// MWdGix-Base
ClassTP(TMWdGixBs, PMWdGixBs)//{
private:
    typedef TPair<TInt, TMWdGixItem> TMWdGixIntItemPr;
    typedef TKeyDat<TFlt, TMWdGixIntItemPr> TWgtMWdGixIntItemKd;
    typedef TVec<TWgtMWdGixIntItemKd> TWgtMWdGixIntItemKdV;

private:
    TFAccess FAccess; // what is the current policy
    TStr FPath; // path opend under
    int64 CacheSize; // size of the cache for Gixs
    PBlobBs DocBBs; // store of original texts
    THash<TStr, PMWdGix> LangMWdGixH; // inverted index maping words to documents for each language
    PAlignPairBs AlignPairBs; // store if aligned documents for query translation
    TAlignPairMapCcar AlignPairMap; // query translation map

    void InitGixs(const TFAccess& GixFAccess) {
        // load inverted index for each language
        int LangId = AlignPairBs->FFirstLangId();
        while (AlignPairBs->FNextLangId(LangId)) {
            const TStr& Lang = AlignPairBs->GetLang(LangId);
            LangMWdGixH.AddDat(Lang) =
                TMWdGix::New(FPath + "/" + Lang, GixFAccess, CacheSize);
        }
    }

public:
    TMWdGixBs(const TStr& _FPath, const TFAccess& _FAccess, const int64& _CacheSize);
    static PMWdGixBs New(const TStr& FPath, const TFAccess& FAccess,
        const int64& CacheSize) { return new TMWdGixBs(FPath, FAccess, CacheSize); }

    ~TMWdGixBs();

    // adds document do blob, gix and meta-data store
    void AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStr& DocLang, const TStrV& KeyWdV);
    // get document from document base
    void GetDoc(const TBlobPt& BlobPt, TStr& DocTitle,
        TStr& DocStr, TStr& DocLang, TStrV& KeyWdV) const;
    // search
    PMWdGixRSet SearchDoc(const TStr& QueryStr, const TStr& QueryLang,
        const TStrV& TargetLangV, const int& Docs /*= 10*/, const int& Offset /*= 0*/,
        PMWdGixRankFun& RankFun /*= TMWdGixRankFunOkapiBM25::New()*/);

    // languages
    bool IsLang(const TStr& Lang) const { return AlignPairBs->IsLang(Lang); }

    // index documents
    void AddAcquis(const TStr& XmlFNm, const TStr& Lang);
    void IndexAcquis(const TStr& InFPath, PAlignPairBs _AlignPairBs,
        const int& MxDocs, const int64& IndexCacheSize);
};

#endif

