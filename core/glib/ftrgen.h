///////////////////////////////////////
// Abstract-Feature-Generator
ClassTP(TFtrGen, PFtrGen)//{
protected:
  typedef PFtrGen (*TFtrGenLoad)(TSIn& SIn);
  typedef TFunc<TFtrGenLoad> TFtrGenLoadF;
  static THash<TStr, TFtrGenLoadF> TypeToLoadFH;
  static bool Reg(const TStr& TypeNm, const TFtrGenLoadF& LoadF);

public:
    TFtrGen() { }
	virtual ~TFtrGen() { }
	
	TFtrGen(TSIn& SIn) { }
	static PFtrGen Load(TSIn& SIn);
	virtual void Save(TSOut& SOut) const { GetTypeNm(*this).Save(SOut);}

	// updates counts
    virtual void Update(const TStr& Val) = 0;
	// generates features
    virtual void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const = 0;
	
	// adds features as words to BowDocBs
	void AddWds(const TStr& Prefix, const PBowDocBs& BowDocBs, int& Offset) const;

	// number of values it takes in a feature vector
    virtual int GetVals() const = 0;
	// returns a word/value in a feature vector
    virtual TStr GetVal(const int& ValN, const bool& PrefixP = true) const = 0;
    // function which only work in nominal and multinomial (for category)
    virtual void GetValV(const TStr& Str, TStrV& ValV) const = 0;
};
    
///////////////////////////////////////
// Numeric-Feature-Generator
class TFtrGenNumeric : public TFtrGen {
private:
  static bool IsReg;
  static bool MkReg() { return TFtrGen::Reg(TTypeNm<TFtrGenNumeric>(), &Load); }

private:        
    TFlt MnVal;
    TFlt MxVal;

    double Trans(const double& Val) const;
    double GetFlt(const TStr& Str) const;

    TFtrGenNumeric(): MnVal(TFlt::Mx), MxVal(TFlt::Mn) { }
	TFtrGenNumeric(TSIn& SIn): TFtrGen(SIn), MnVal(SIn), MxVal(SIn) { }
public:
    static PFtrGen New() { return new TFtrGenNumeric; }

	static PFtrGen Load(TSIn& SIn) { return PFtrGen(new TFtrGenNumeric(SIn)); }
	void Save(TSOut& SOut) const{ TFtrGen::Save(SOut); MnVal.Save(SOut); MxVal.Save(SOut); }

	static TStr GetType() { return "NUM"; }

    void Update(const TStr& Val);
    void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const;

    int GetVals() const { return 1; }
    TStr GetVal(const int& ValN, const bool& PrefixP) const { return GetType(); }
    void GetValV(const TStr& Str, TStrV& ValV) const { Fail; }
};

///////////////////////////////////////
// Nominal-Feature-Generator
class TFtrGenNominal : public TFtrGen {
private:
  static bool IsReg;
  static bool MkReg() { return TFtrGen::Reg(TTypeNm<TFtrGenNominal>(), &Load); }

private:
    TStrH ValH;   

    TFtrGenNominal() { }
	TFtrGenNominal(TSIn& SIn): TFtrGen(SIn), ValH(SIn) { }
public:
    static PFtrGen New() { return new TFtrGenNominal; }

	static PFtrGen Load(TSIn& SIn) { return PFtrGen(new TFtrGenNominal(SIn)); }
	void Save(TSOut& SOut) const { TFtrGen::Save(SOut); ValH.Save(SOut);}

	static TStr GetType() { return "NOM"; }

    void Update(const TStr& Val);
    void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const;

    int GetVals() const { return ValH.Len(); } 
    TStr GetVal(const int& ValN, const bool& PrefixP) const { 
		return PrefixP ? (GetType() + ":" + ValH.GetKey(ValN)) : ValH.GetKey(ValN); }
    void GetValV(const TStr& Str, TStrV& ValV) const { ValV = TStrV::GetV(Str); }
};

///////////////////////////////////////
// Tokenizable-Feature-Generator
class TFtrGenToken : public TFtrGen {
private:
  static bool IsReg;
  static bool MkReg() { return TFtrGen::Reg(TTypeNm<TFtrGenToken>(), &Load); }

private:
    PSwSet SwSet;
    PStemmer Stemmer;
    TInt Docs;
    TStrH TokenH;

    void GetTokenV(const TStr& Str, TStrV& TokenStrV) const;
    TFtrGenToken(PSwSet _SwSet, PStemmer _Stemmer): SwSet(_SwSet), Stemmer(_Stemmer) { }
	TFtrGenToken(TSIn& SIn);
public:
    static PFtrGen New(PSwSet SwSet, PStemmer Stemmer) { return new TFtrGenToken(SwSet, Stemmer); }

	static PFtrGen Load(TSIn& SIn) { return PFtrGen(new TFtrGenToken(SIn)); }
	void Save(TSOut& SOut) const;

	static TStr GetType() { return "TOK"; }

    void Update(const TStr& Val);
    void Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const;

    int GetVals() const { return TokenH.Len(); } 
    TStr GetVal(const int& ValN, const bool& PrefixP) const { 
		return PrefixP ? GetType() + ":" + TokenH.GetKey(ValN) : TokenH.GetKey(ValN); }
    void GetValV(const TStr& Str, TStrV& ValV) const { Fail; }
};   

///////////////////////////////////////
// Sparse-Feature-Generator
class TFtrGenSparseNumeric : public TFtrGen {
private:
  static bool IsReg;
  static bool MkReg() { return TFtrGen::Reg(TTypeNm<TFtrGenSparseNumeric>(), &Load); }

private:
    TInt MxId;
	PFtrGen FtrGen;

    void Split(const TStr& Str, int& Id, TStr& Val) const;
	TFtrGenSparseNumeric(): FtrGen(TFtrGenNumeric::New()) { }
	TFtrGenSparseNumeric(TSIn& SIn): TFtrGen(SIn), MxId(SIn), FtrGen(SIn) { }
public:
    static PFtrGen New() { return new TFtrGenSparseNumeric; }

	static PFtrGen Load(TSIn& SIn) { return PFtrGen(new TFtrGenSparseNumeric(SIn)); }
	void Save(TSOut& SOut) const { TFtrGen::Save(SOut); MxId.Save(SOut); FtrGen.Save(SOut); }

	static TStr GetType() { return "SP-NUM"; }

    void Update(const TStr& Str);
    void Add(const TStr& Str, TIntFltKdV& SpV, int& Offset) const;

    int GetVals() const { return MxId + 1; } 
    TStr GetVal(const int& ValN, const bool& PrefixP) const;
    void GetValV(const TStr& Str, TStrV& ValV) const { Fail; }
};

///////////////////////////////////////
// MultiNomial-Feature-Generator
class TFtrGenMultiNom : public TFtrGen {
private:
  static bool IsReg;
  static bool MkReg() { return TFtrGen::Reg(TTypeNm<TFtrGenMultiNom>(), &Load); }

private:
	PFtrGen FtrGen;

	TFtrGenMultiNom(): FtrGen(TFtrGenNominal::New()) { }
	TFtrGenMultiNom(TSIn& SIn): TFtrGen(SIn), FtrGen(SIn) { }
public:
    static PFtrGen New() { return new TFtrGenMultiNom; }

	static PFtrGen Load(TSIn& SIn) { return PFtrGen(new TFtrGenMultiNom(SIn)); }
	void Save(TSOut& SOut) const { TFtrGen::Save(SOut); FtrGen.Save(SOut); }

	static TStr GetType() { return "MULTI-NOM"; }

    void Update(const TStr& Str);
    void Add(const TStr& Str, TIntFltKdV& SpV, int& Offset) const;

    int GetVals() const { return FtrGen->GetVals(); } 
    TStr GetVal(const int& ValN, const bool& PrefixP) const;
    void GetValV(const TStr& Str, TStrV& ValV) const;
};

///////////////////////////////////////
// Feature-Generator-Base
ClassTP(TFtrGenBs, PFtrGenBs)//{
private:
	typedef enum { fgsInit, fgsUpdate, fgsGen } TFtrGenState;

private:
	TFtrGenState State;
    TVec<PFtrGen> FtrGenV;
    PFtrGen ClsFtrGen;

public:
	TFtrGenBs(): State(fgsInit) { }
	static PFtrGenBs New() { return new TFtrGenBs; }

	TFtrGenBs(TSIn& SIn);
	static PFtrGenBs Load(TSIn& SIn) { return new TFtrGenBs(SIn); }
	void Save(TSOut& SOut) const;

	// feature generators
	int GetFtrGens() const { return FtrGenV.Len(); }
	int AddFtrGen(const PFtrGen& FtrGen);
	void PutClsFtrGen(const PFtrGen& FtrGen);
	void FinishInit() { State = fgsUpdate; }
	
	// counting features
	void Update(const TStrV& FtrValV);
	void UpdateCls(const TStr& ClsFtrVal);
	void FinishUpdate() { State = fgsGen; }
	// generate feature vector
	void GenFtrV(const TStrV& FtrValV, TIntFltKdV& FtrSpV) const;
	void GenClsV(const TStr& ClsFtrVal, TStrV& ClsV) const;
	// dealing with bow
	PBowDocBs MakeBowDocBs() const;
	void AddBowDoc(const PBowDocBs& BowDocBs, const TStr& DocNm, 
		const TStrV& FtrValV) const;
	void AddBowDoc(const PBowDocBs& BowDocBs, const TStr& DocNm, 
		const TStrV& FtrValV, const TStr& ClsFtrVal) const;
	// dealing with categories
	void GetAllCatNmV(TStrV& CatNmV) const;

	// loads CSV files into BowDocBs
    static PBowDocBs LoadCsv(TStr& FNm, const int& ClassId, 
        const TIntV& IgnoreIdV, const int& TrainLen);
};