/////////////////////////////////////////////////
// Forward-declarations
ClassHdTP(TTransCorpus, PTransCorpus);

/////////////////////////////////////////////////
// Translation-Evaluation-Scores
class TEvalScore {
private:
    THtmlLxChDef ChDef;
protected:
    TStrH WordH;
    TIntVIntH NGramH;

    void LoadLatin1();
    void LoadLatin2();

    TStr GetLc(const TStr& Str) { return ChDef.GetLcStr(Str); }
    bool IsChar(const char& Ch) { return ChDef.IsAlNum(Ch) || ChDef.IsSym(Ch); }
    void Tokenize(const TStr& Str, TStrV& TokenV);
    void Parse(const TStr& Str, TIntV& WIdV);
    void GetNGramH(const TIntV& WIdV, const int& MxNGramLen, TIntH& NGramIdH);
    int GetNGramLen(const int& NGramId) { return NGramH.GetKey(NGramId).Len(); }

public:
    virtual ~TEvalScore(){}
    virtual double Eval(const PTransCorpus& TransCorpus, const TIntV& SentIdV) = 0;
    PMom EvalSig(const PTransCorpus& TransCorpus,
        const int& Samples, const int& SampleSize, const int& Seed = 1);
};

/////////////////////////////////////////////////
// BLEU-score
class TEvalScoreBleu: public TEvalScore {
private:
    TInt MxNGramLen;
public:
    TEvalScoreBleu(const int& _MxNGramLen): MxNGramLen(_MxNGramLen) { }
    double Eval(const PTransCorpus& TransCorpus, const TIntV& _SentIdV = TIntV());
};

/////////////////////////////////////////////////
// Translation-Corpus
ClassTP(TTransCorpus, PTransCorpus) //{
private:
    // original sentences
    TIntStrH OrgStrH;
    // machine translation
    TIntStrH TransStrH;
    // reference trasnlations of reference sentences
    TIntStrVH RefTransStrVH;

    TStr GetOutFNm(const TStr& FBase, const int& LnN, const int& Lns, const TStr& FExt) {
        return (Lns == -1) ? FBase + FExt : FBase + TInt::GetStr(LnN / Lns, "%06d") + FExt; }

    void PutScriptHeader(const PSOut& SOut);
    static void CleanTmx(const TStr& InTmxFNm, const TStr& OutXmlFNm);
    static TStr CleanRtf(const TStr& InStr);
    static int CountWords(const TIntStrH& StrH);
    static int CountWords(const TIntStrVH& StrVH, const bool& AllSents);

public:
    TTransCorpus() { }
    static PTransCorpus New() { return new TTransCorpus(); }

    bool IsRefTrans() const { return (RefTransStrVH.Len()==TransStrH.Len()) || (RefTransStrVH.Len()==OrgStrH.Len()); }
    bool IsTrans() const { return (TransStrH.Len()==RefTransStrVH.Len()) || (TransStrH.Len()==OrgStrH.Len()); }
    bool IsOrg() const { return (!OrgStrH.Empty()) && (IsTrans()||IsRefTrans()); }

    void AddSentenceNoTrans(const int& SentId, const TStr& OrgStr, const TStr& RefTransStr);
    void AddSentenceNoTrans(const int& SentId, const TStr& OrgStr, const TStrV& RefTransStrV);
    void AddSentenceNoOrg(const int& SentId, const TStr& TransStr, const TStr& RefTransStr);
    void AddSentenceNoOrg(const int& SentId, const TStr& TransStr, const TStrV& RefTransStrV);
    void AddSentence(const int& SentId, const TStr& OrgStr,
        const TStr& TransStr, const TStr& RefTransStr);
    void AddSentence(const int& SentId, const TStr& OrgStr,
        const TStr& TransStr, const TStrV& RefTransStrV);
    int GetSentences() const { return OrgStrH.Len(); }
    int GetFirstSentKeyId() const { return OrgStrH.FFirstKeyId(); }
    bool GetNextSentKeyId(int& SentKeyId) const { return OrgStrH.FNextKeyId(SentKeyId); }
    int GetSentId(const int& SentKeyId) const { return OrgStrH.GetKey(SentKeyId); }
    void GetSentIdV(TIntV& SentIdV) const;
    const TStr& GetOrgStr(const int SentId) const { return OrgStrH.GetDat(SentId); }
    const TStr& GetTransStr(const int SentId) const { return TransStrH.GetDat(SentId); }
    const TStrV& GetRefTransStrV(const int SentId) const { return RefTransStrVH.GetDat(SentId); }

    int GetOrgWords() const { return CountWords(OrgStrH); }
    int GetTransWords() const { return CountWords(TransStrH); }
    int GetRefTransWords() const { return CountWords(RefTransStrVH, false); }

    // predefined corpora formats
    static PTransCorpus LoadAC(const TStr& InXmlFNm, const int& MxSents = -1);
    static PTransCorpus LoadEP(const TStr& InOrgFPath, const TStr& InTransFPath);
    static PTransCorpus LoadTMX(const TStr& InTmxFPath,
        const TStr& OrgLang, const TStr& RefTransLang);

    static PTransCorpus LoadTxt(const TStr& InOrgFNm,
        const TStr& InTransFNm, const TStr& InRefTransFNm);
    void SaveTxt(const TStr& OutFBase, const TStr& OutOrgFNm,
        const TStr& OutTransFNm, const TStr& OutRefTransFNm,
        TStrV& OrgFNmV, TStrV& TransFNmV, TStrV& RefTransFNmV,
        const int& LinesPerFile = 100);
    void SaveTransScript(const TStr& OutFBase, const TStr& OrgLang,
        const TStr& TransLang, const TStrV& OrgFNmV, const TStrV& RefTransFNmV);
};

/////////////////////////////////////////////////
// Aligned-Pair
ClassTPV(TAlignPair, PAlignPair, TAlignPairV)//{
private:
    // two letter language name
    TStr Lang1;
    TStr Lang2;
    // sparse vectors of aligned documents
    PBowDocBs BowDocBs1;
    PBowDocWgtBs BowDocWgtBs1;
    PBowDocBs BowDocBs2;
    PBowDocWgtBs BowDocWgtBs2;
    // sparse matrixes with pointers to sparse vectors
    // (columns are documents, rows are words)
    TBowMatrix Matrix1;
    TBowMatrix Matrix2;

public:
    TAlignPair(const TStr& _Lang1, const TStr& _Lang2);
    static PAlignPair New (const TStr& Lang1, const TStr& Lang2) {
        return new TAlignPair(Lang1, Lang2); }

    TAlignPair(TSIn& SIn);
    static PAlignPair Load(TSIn& SIn) { return new TAlignPair(SIn); }
    void Save(TSOut& SOut) const;
    virtual ~TAlignPair(){}

    // for filling up the corpus
    void AddSent(const TStr& Sent1, const TStr& Sent2);
    void Def();

    // access to content
    const TStr& GetLang1() const { return Lang1; }
    const TStr& GetLang2() const { return Lang2; }
    const TMatrix& GetMatrix(const TStr& Lang) const;
    void GetSpV(const TStr& Str, const TStr& Lang, TIntFltKdV& SpV) const;
    TStr GetSpVStr(const TIntFltKdV& SpV, const TStr& Lang,
        const int& MxWords, const double& MxWgtPrc) const;

    // loading
    static PAlignPair LoadAcXml(const TStr& FNm, const int& MxSents);
};

/////////////////////////////////////////////////
// Aligned-Based-Text-Mapper
class TAlignPairMap {
public:
    virtual void operator()(const TIntFltKdV& InSpV, const TMatrix& InMatrix,
        const TMatrix& OutMatrix, TIntFltKdV& OutSpV) const = 0;
};

/////////////////////////////////////////////////
// Aligned-Pair-Base
ClassTP(TAlignPairBs, PAlignPairBs)//{
private:
    // languages
    TStrH LangH;
    // storage of alignments
    TAlignPairV AlignPairV;
    // hash table with maps from languages to pairs
    THash<TInt, TAlignPairV> LangToAlignPairVH;
    THash<TIntPr, PAlignPair> LangPrToAlignPairH;

    void AddAlignPair(const PAlignPair& AlignPair);
    PAlignPair GetAlignPair(const int& Lang1, const int& Land2);

public:
    TAlignPairBs() { }
    static PAlignPairBs New() { return new TAlignPairBs; }

    TAlignPairBs(TSIn& SIn);
    static PAlignPairBs Load(TSIn& SIn) { return new TAlignPairBs(SIn); }
    void Save(TSOut& SOut) const;

    // lanugages
    bool IsLang(const TStr& Lang) const { return LangH.IsKey(Lang); }
    const TStr& GetLang(const int& LangId) const { return LangH.GetKey(LangId); }
    int GetLangId(const TStr& Lang) const { return LangH.GetKeyId(Lang); }
    int FFirstLangId() const { return LangH.FFirstKeyId(); }
    bool FNextLangId(int& LangId) const { return LangH.FNextKeyId(LangId); }

    // query translation
    TStr MapQuery(const TAlignPairMap& Map, const TStr& QueryStr,
        const int& QueryLangId, const int& TargetLangId,
        const int& TransQueryMtpy = 3, const double& MxWgtPrc = 0.5);

    // loading
    static PAlignPairBs LoadAcXml(const TStr& FPath, const int& MxSents);
    static PAlignPairBs LoadBin(const TStr& FNm) { TFIn FIn(FNm); return Load(FIn); }
    void SaveBin(const TStr& FNm) const { TFOut FOut(FNm); Save(FOut); }
};

