//////////////////////////////////////////////////////////////////////////
// String-To-Words
ClassTP(TStrParser, PStrParser) //{
private:
    TStrIntH WordToIdH;  // word-strings to document frequency (DC) (word-id == key-id)
    TInt Type;           // type of parser (words, syllabels, characters)
    TInt DocsParsed;     // no. of documents parsed so far (used at IDF)

    UndefDefaultCopyAssign(TStrParser);
public:
    TStrParser(int tt): WordToIdH(), Type(tt) {}
    static PStrParser New(int tt) { return new TStrParser(tt); }

    // saving / loading from files
    TStrParser(TSIn& SIn) { WordToIdH.Load(SIn); Type = TInt(SIn); DocsParsed = TInt(SIn); }
    static PStrParser Load(TSIn& SIn) { return new TStrParser(SIn); }
    void Save(TSOut& SOut) { WordToIdH.Save(SOut); Type.Save(SOut); DocsParsed.Save(SOut); }

    // words
    int GetWords() const { return WordToIdH.Len(); }
    bool IsWordStr(const TStr& WordStr) const { return WordToIdH.IsKey(WordStr); }
    int GetWId(const TStr& WordStr) const { return WordToIdH.GetKeyId(WordStr); }
    TStr GetWordStr(const int& WId) const { return WordToIdH.GetKey(WId); }
    int GetWordFq(const int& WId) const { return WordToIdH[WId]; }

    void ProcessDocStr(const TStr& _DocStr, TIntV& DocVec) {
        if (Type == 0) DocStrToWIdV(_DocStr, DocVec, true);
        else if (Type == 1) DocStrToSylIdV(_DocStr, DocVec);
        else if (Type == 2) DocStrToChIdV(_DocStr, DocVec);
        else if (Type == 3) DocStrToWIdV(_DocStr, DocVec, false); }
    // document string to word-id vector
    void DocStrToWIdV(const TStr& _DocStr, TIntV& DocVec, const bool& Stemm);
    // document string to Syllables-id vector
    void DocStrToSylIdV(const TStr& _DocStr, TIntV& SyllablesIdV);
    // document string to letter-id vector
    void DocStrToChIdV(const TStr& _DocStr, TIntV& ChIdV);

    // prints parser info
    void WhoAmI(const TStr& intro) const;
    int GetAlphabetSize() const { return WordToIdH.Len(); }
    int GetParserType() const { return Type; }

    // calculates weights from alphabet statistics
    void GetIDFWeightV(TFltV& WeightV);
};

////////////////////////////////////////////////////////////////////////////
//// Similarity-Matrix
////   Calculates similarity between two words with a little help from WordNet
//ClassTP(TSimMatrix, PSimMatrix) //{
//private:
//    PWordNet WordNet;
//    TIntV PIdToWId;
//    THash<TIntPr, TInt> DistH;
//public:
//    TSimMatrix(const TStr& FName) { WordNet = TWordNet::Load(TFIn(FName)); }
//    static PSimMatrix Load(const TStr& FName) { return new TSimMatrix(FName); }
//
//    // set ids
//    void SetIdV(PStrParser Parser);
//
//    // gets similarity between words with TParser-Id WId1 and WId2
//    double GetSim(const int& WId1, const int& WId2);
//};

//////////////////////////////////////////////////////////////////////////
// String-Kernels
//   Different implementations and configurations of string kernels.
//   They all work on TIntV where values of elements corespond to
//   symbol of alphabet (characthers, syllabels, words, etc.)
//   So far this package includs:
//     - standard implementation of gap-weighted string kernels
//       using dynamci programming
//     - improvment of previos method with different weights for
//     - differnet symbols + linear combinations of kenrels
//     - two typs of implementation of trie based restricted
//       gap-weights kernels (nubmer of gaps is limited!)
ClassTP(TStringKernel, PStringKernel) //{
public:
    // string kernel for subsequances with length k and weight lb calculated using dynamic programming
    static double KDynamic(const TIntV& s, const TIntV& t, const TFltV& lc, const double& lb);
    //// string kernel for subsequances with length k and weight lb calculated using
    //// dynamic programming with soft matching of characters of alphabet
    //static double KDynamicSM(const TIntV& s, const TIntV& t, const TFltV& lc, const double& lb, PSimMatrix Sim);
    // calculates string kernels for subsequences with length 2 to lc.Len() and returns linear
    // combination of them with lc as scalars! wgt is a vector of weigths for each symbol of alphabet
    static double KDynamicW(const TIntV& s, const TIntV& t, const TFltV& lc, const TFltV& wgt);
    // trie based restricted gap-weighted kernels
    static double KTrie(const TIntV& s, const TIntV& t, const double& lb, const int& p, int m, const int& AlphN);
    // trie based restricted gap-weighted kernels (uses more memory, should be bit faster...)
    static double KTrie2(const TIntV& s, const TIntV& t, const double& lb, const int& p, int m, const int& AlphN);
private:
    static double *Buf1, *Buf2;
    static int BufN;
    static int CountEval;
    typedef TTriple<TIntV, TInt, TInt> TTrieNode;
    typedef TTriple<TIntPr, TInt, TInt> TTrieNodeP;
    static void KTrieR(const TIntV& s, const TIntV& t, const TVec<TTrieNodeP>& Ls, const TVec<TTrieNodeP>& Lt,
        TIntV& v, const int& depth, double& Kern, const TFltV& lbV, const int& p, const int& m, const int& AlphN);
    static void KTrieR2(const TIntV& s, const TIntV& t, const TVec<TTrieNodeP>& Ls, const TVec<TTrieNodeP>& Lt,
        TIntV& v, const int& depth, double& Kern, const TFltV& lbV, const int& p, const int& m, const int& AlphN);
private:
    TInt KernelType; // which algorithm to use for calculating string kernels
                     // 0 - KDynamic, 1 - KDynamicLCW, 2 - KTrie, 3 - KTrie2, //4 - KDynamicSM
    // next options are used with: KDynamic, KTrie, Ktrie2
    TFlt Lambda; // weight for feature's length
    // use by: KTrie, KTrie2
    TInt SeqLen; // sequence length
    TInt GapLen; // max alowed gap length
    TInt AlphN; // size of alphabet
    // used by: KDynamic, KDynamicLCW
    TFltV LinCombV;
    // used by: KDynamicLCW
    TFltV WeightV;
    // used by: KDynamicSM
    //PSimMatrix SimMatrix;

    UndefDefaultCopyAssign(TStringKernel);
public:
    // constructs KDynamic
    TStringKernel(const TFltV& _LinCombV, const double& _Lambda):
        KernelType(0), Lambda(_Lambda), LinCombV(_LinCombV) { }
    static PStringKernel New(const TFltV& _LinCombV, const double& _Lambda) {
        return new TStringKernel(_LinCombV, _Lambda); }
    //// constructs KDynamicSM
    //TStringKernel(const TFltV& _LinCombV, const double& _Lambda, PSimMatrix _SimMatrix):
    //    KernelType(4), LinCombV(_LinCombV), Lambda(_Lambda), SimMatrix(_SimMatrix) { }
    //static PStringKernel New(const TFltV& _LinCombV, const double& _Lambda, PSimMatrix _SimMatrix) {
    //    return new TStringKernel(_LinCombV, _Lambda, _SimMatrix); }
    // constructs KTire or KTire2
    TStringKernel(const int& _SeqLen, const double& _Lambda, const int& _GapLen, const bool& Fast):
        Lambda(_Lambda), SeqLen(_SeqLen), GapLen(_GapLen) { if (Fast) KernelType = 3; else KernelType = 2; }
    static PStringKernel New(const int& _SeqLen, const double& _Lambda, const int& _GapLen,
        const bool& Fast = false) { return new TStringKernel(_SeqLen, _Lambda, _GapLen, Fast); }
    // constructs KDynamicLCW
    TStringKernel(const TFltV& _LinCombV, const TFltV& _WeightV):
        KernelType(1), LinCombV(_LinCombV), WeightV(_WeightV) { }
    static PStringKernel New(const TFltV& _LinCombV, const TFltV& _WeightV) {
        return new TStringKernel(_LinCombV, _WeightV); }

    // saving / loading from files
    TStringKernel(TSIn& SIn);
    static PStringKernel Load(TSIn& SIn) { return new TStringKernel(SIn); }
    void Save(TSOut& SOut);

    int GetKernelType() { return KernelType; }
    // updates AlphN (needed for KTrie and KTrie2)
    void UpdateAlphN(const int& NewAlphN) { AlphN = NewAlphN; }
    // new weithgts vector
    void NewWeightV(const TFltV& NewWeightV) {
        if (KernelType == 0) KernelType = 1; WeightV = NewWeightV; }
    // prints kernel info
    void WhoAmI(const TStr& intro) const;
    // max subsequence lengths
    int GetSeqLen() const;
    // does usefull things with parser info
    void ProcessParser(PStrParser Parser) { /*if (KernelType == 4) SimMatrix->SetIdV(Parser); */ }

    // calculates kernel between two sequences
    double CalcKernel(const TIntV& Vec1, const TIntV& Vec2);
};

////////////////////////////////////////////////////////////////////////////
//// Ortogonal-System
////   Calculates basis for Dim-dimensional subspace of string-kernel
////   feature space. This can be used to approximate feature vectors.
//ClassTP(TOrtoSystem, POrtoSystem) //{
//private:
//    // basis
//    TVec<TIntV> Basis;
//    TFltV Norm2V;
//    PStringKernel StrKer;
//public:
//    // creates ortogonal system for Dim dimensional space with
//    // strings of lengths Len for basis
//    TOrtoSystem(PStringTrainSet BigSet, const int& Dim, int Len = -1);
//    static POrtoSystem New(PStringTrainSet BigSet, const int& Dim, const int& Len = -1) {
//        return new TOrtoSystem(BigSet, Dim, Len); }
//
//    TOrtoSystem(TSIn& SIn) { Basis.Load(SIn); Norm2V.Load(SIn); StrKer = TStringKernel::Load(SIn); }
//    static POrtoSystem Load(TSIn& SIn) { return new TOrtoSystem(SIn); }
//    void Save(TSOut& SOut) { Basis.Save(SOut); Norm2V.Save(SOut); StrKer->Save(SOut); }
//
//    // maps string x to feature space and from there it projects it to ortogonal
//    // system basis and returns projection as y, Norm2 is norm of x in feature space
//    void Project(const TIntV& x, const double& Norm2, TFltV& y);
//    // projects all whole BigSet to ortogonal system basis
//    PDenseTrainSet Project(PStringTrainSet BigSet);
//
//    TIntV& GetBasis(const int& VecId)  { return Basis[VecId]; }
//};
