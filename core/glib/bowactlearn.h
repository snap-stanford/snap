/////////////////////////////////////////////////
// BagOfWords-Active-Learning
typedef enum {baltCat, baltQuery} TBowALType;

ClassTP(TBowAL, PBowAL)//{
private:
    TBowALType ALType;
    // general properties
    PBowDocBs BowDocBs;
    TFlt SvmC, SvmJ;
    TIntV LabeledDIdV, UnlabeledDIdV; // labeled/unlabeled pointers to DocSet!!!
    PBowDocWgtBs BowDocWgtBs;
    PSVMTrainSet DocSet;
    TFltIntPrV DistDIdPrV;
    // Cat properties
    TInt CId;
    // Query properties
    TStr QueryStr;
    PBowSpV QuerySpV;
    TBool InfoRetModeP;
    TInt MnPosDocs;
    TInt MnNegDocs;
    // stats
    TStr KeyWdStr;
    TInt PosDocN;
    TInt NegDocN;
public:
    // active learning based on documents lableled with categories
    TBowAL(const PBowDocBs& _BowDocBs, const int& _CId,
        const double& _SvmC, const double& _SvmJ, const TIntV& _DIdV);
    static PBowAL NewFromCat(const PBowDocBs& BowDocBs, const int& CId,
        const double& SvmC, const double& SvmJ, const TIntV& DIdV = TIntV()) {
            return PBowAL(new TBowAL(BowDocBs, CId, SvmC, SvmJ, DIdV));}
    // active learning based on query -- must also find initial
    // set of positive and negative documents
    TBowAL(const PBowDocBs& _BowDocBs, const TStr& _QueryStr,
        const int& _MnPosDocs, const int& _MnNegDocs, const double& _SvmC,
        const double& _SvmJ, const TIntV& _DIdV);
    static PBowAL NewFromQuery(const PBowDocBs& BowDocBs, const TStr& QueryStr,
        const int& MnPosDocs, const int& MnNegDocs, const double& SvmC,
        const double& SvmJ, const TIntV& DIdV = TIntV()) {
            return PBowAL(new TBowAL(BowDocBs, QueryStr,
                MnPosDocs, MnNegDocs, SvmC, SvmJ, DIdV));
    }

    TBowAL(TSIn& SIn){Fail;}
    static PBowAL Load(TSIn& SIn){return new TBowAL(SIn);}
    void Save(TSOut& SOut){Fail;}

    int GetCId() const {return CId;}
    // prepares a list of queries to ask the user
    bool GenQueryDIdV(const bool& GenStatP = false);
    // number of queries
    int GetQueryDIds() const { return DistDIdPrV.Len(); }
    // get the document id and it's distance of the N-th query
    void GetQueryDistDId(const int& QueryDIdN, double& Dist, int& DId){
        Dist=DistDIdPrV[QueryDIdN].Val1; DId=DistDIdPrV[QueryDIdN].Val2;}
    // adds the query feedback back into the datasets
    void MarkQueryDoc(const int& QueryDId, const bool& AssignToCatP);
    // marks the remaining of the unlabeled documents in the BOW using SVM
    void MarkUnlabeledPosDocs();
    // returns a list of DocIds of all positive documents
    // (both labeled and positivly classified unlabeled)
    void GetAllPosDocs(TIntV& PosDIdV);

    TBowALType GetType() const { return TBowALType(int(ALType)); }
    TStr GetQueryStr() const { return QueryStr; }
    bool GetInfoRetMode() const { return InfoRetModeP; }

    TStr GetKeyWdStr() const { return KeyWdStr; }
    int GetPosDocN() const { return PosDocN; }
    int GetNegDocN() const { return NegDocN; }
};

