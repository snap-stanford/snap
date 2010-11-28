/////////////////////////////////////////////////
// NGram-Base
ClassTP(TNGramBs, PNGramBs)//{
private:
  TInt MxNGramLen;
  TInt MnNGramFq;
  PSwSet SwSet;
  PStemmer Stemmer;
  TStrIntH WordStrToFqH;
  TIntVIntH WIdVToFqH;
private:
  // work variables
  TInt PassN;
  TIntQ CandWIdQ;
  TIntPrIntH CandWIdPrToFqH;
  TIntVIntH CandWIdVToFqH;
public:
  TNGramBs(const int& _MxNGramLen, const int& _MnNGramFq,
   const PSwSet& _SwSet, const PStemmer& _Stemmer):
    MxNGramLen(_MxNGramLen), MnNGramFq(_MnNGramFq),
    SwSet(_SwSet), Stemmer(_Stemmer),
    WordStrToFqH(), WIdVToFqH(),
    PassN(1), CandWIdQ(), CandWIdPrToFqH(), CandWIdVToFqH(){
    IAssert(MxNGramLen>=1); IAssert(MnNGramFq>=1);}
  static PNGramBs New(const int& MxNGramLen, const int& MnNGramFq,
   const PSwSet& SwSet, const PStemmer& Stemmer){
    return PNGramBs(new TNGramBs(MxNGramLen, MnNGramFq, SwSet, Stemmer));}
  TNGramBs(const TNGramBs&){Fail;}
  TNGramBs(TSIn& SIn):
    MxNGramLen(SIn), MnNGramFq(SIn),
    SwSet(SIn), Stemmer(SIn),
    WordStrToFqH(SIn), WIdVToFqH(SIn),
    PassN(-1), CandWIdQ(), CandWIdPrToFqH(), CandWIdVToFqH(){}
  static PNGramBs Load(TSIn& SIn){return new TNGramBs(SIn);}
  void Save(TSOut& SOut){
    MxNGramLen.Save(SOut); MnNGramFq.Save(SOut);
    SwSet.Save(SOut); Stemmer.Save(SOut);
    WordStrToFqH.Save(SOut); WIdVToFqH.Save(SOut);}

  TNGramBs& operator=(const TNGramBs&){Fail; return *this;}

  // comonents retrieval
  int GetMxNGramLen() const {return MxNGramLen;}
  int GetMnNGramFq() const {return MnNGramFq;}
  PSwSet GetSwSet() const {return SwSet;}
  PStemmer GetStemmer() const {return Stemmer;}
  int GetPassN() const {return PassN;}

  // words
  int GetWords() const {return WordStrToFqH.Len();}
  bool IsWord(const TStr& WordStr, int& WId) const {
    return WordStrToFqH.IsKey(WordStr, WId);}
  int GetWId(const TStr& WordStr) const {
    return WordStrToFqH.GetKeyId(WordStr);}
  TStr GetWordStr(const int& WId) const {
    return WordStrToFqH.GetKey(WId);}
  int GetWordFq(const int& WId) const {
    return WordStrToFqH[WId];}
  bool IsSkipWord(const int& WId) const {
    return WordStrToFqH[WId]==-1;}

  // word-sequences
  int GetWIdVs() const {return WIdVToFqH.Len();}
  int GetWIdVFq(const TIntV& WIdV) const {
    return WIdVToFqH.GetDat(WIdV);}
  int GetWIdVFq(const int& WIdVId) const {
    return WIdVToFqH[WIdVId];}
  TStr GetWIdVStr(const TIntV& WIdV) const;

  // ngrams
  int GetNGrams() const {return WordStrToFqH.Len()+WIdVToFqH.Len();}
  TStr GetNGramStr(const int& NGramId) const;
  int GetNGramFq(const int& NGramId) const;
  void GetNGramStrFq(const int& NGramId, TStr& NGramStr, int& NGramFq) const;
  void GetNGramIdV(const TStr& HtmlStr, TIntV& NGramIdV, TIntPrV& NGramBEChXPrV) const;
  void GetNGramStrV(const TStr& HtmlStr, TStrV& NGramStrV, TIntPrV& NGramBEChXPrV) const;
  void GetNGramStrV(const TStr& HtmlStr, TStrV& NGramStrV) const;

  // generation of ngrams
  void AddWordToCand(const int& WId);
  int AddWord(const TStr& WordStr);
  void BreakNGram(){if (!CandWIdQ.Empty()){CandWIdQ.Clr();}}
  void ConcPass();
  bool IsFinished() const {return PassN>MxNGramLen;}

  // files
  void SaveTxt(const TStr& FNm="CON", const bool& SortP=false) const;

  // n-gram-base creation
  static void _UpdateNGramBsFromHtmlStr(
   const PNGramBs& NGramBs, const TStr& HtmlStr,
   const PSwSet& SwSet, const PStemmer& Stemmer);
  static PNGramBs GetNGramBsFromHtmlStrV(
   const TStrV& HtmlStrV,
   const int& MxNGramLen, const int& MnNGramFq,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
  static PNGramBs GetNGramBsFromHtmlFPathV(
   const TStr& FPath, const bool& RecurseDirP, const int& MxDocs,
   const int& MxNGramLen, const int& MnNGramFq,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
  static PNGramBs GetNGramBsFromCpd(
   const PSIn& CpdSIn, const int& MxDocs=-1,
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
  static PNGramBs GetNGramBsFromTBs(
   const TStr& TBsFNm, const int& MxDocs=-1,
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
  static PNGramBs GetNGramBsFromLnDoc(
   const TStr& LnDocFNm, const bool& NamedP, const int& MxDocs=-1,
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
  static PNGramBs GetNGramBsFromReuters21578(
   const TStr& FPath, const int& MxDocs=-1,
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
  static PNGramBs GetNGramBsFromCiaWFB(
   const TStr& FPath, const int& MxDocs=-1,
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL);
};
