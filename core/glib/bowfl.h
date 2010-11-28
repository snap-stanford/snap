/////////////////////////////////////////////////
// BagOfWords-Files
class TBowFl{
public:
  // custom format files (Html)
  static void LoadHtmlTxt(
   PBowDocBs BowDocBs, const TStr& FPath, TIntV& NewDIdV,
   const bool& RecurseDirP=false, const int& MxDocs=-1,
   const bool& SaveDocP=false, const PNotify& Notify = TNotify::NullNotify);
  static PBowDocBs LoadHtmlTxt(
   const TStr& FPath, const bool& RecurseDirP=false, const int& MxDocs=-1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL,
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const bool& SaveDocP=false, const PNotify& Notify = TNotify::NullNotify);
  static PBowDocBs LoadHtmlTxt(
   const TStr& FPath, const bool& RecurseDirP=false, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1,
   const bool& SaveDocP=false, const PNotify& Notify = TNotify::NullNotify);

  // custom format file (Similarity-Matrix)
  static PBowDocBs LoadFromSimMtx(const PBowSimMtx& BowSimMtx);

  // custom format files (Tab-Separated-File)
  static PBowDocBs LoadTabTxt(const TStr& FNm, const int& MxDocs=-1);

  // custom format files (Transaction)
  static PBowDocBs LoadTsactTxt(const TStr& FNm, const int& MxDocs=-1);

  // custom format files (Sparse)
  static PBowDocBs LoadSparseTxt(
   const TStr& DocDefFNm=TStr(), const TStr& WordDefFNm=TStr(),
   const TStr& TrainDataFNm=TStr(),
   const int& MxDocs=-1);

  // custom format files (SvmLight)
  static PBowDocBs LoadSvmLightTxt(
   const TStr& DocDefFNm=TStr(), const TStr& WordDefFNm=TStr(),
   const TStr& TrainDataFNm=TStr(), const TStr& TestDataFNm=TStr(),
   const int& MxDocs=-1);

  // custom format files (Compact-Document-Base)
  static PBowDocBs LoadCpdTxt(
   const PSIn& CpdSIn, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1);
  static PBowDocBs LoadCpdTxt(
   const TStr& CpdFNm, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1);
  static void SaveCpdToLnDocTxt(const TStr& InCpdFNm, const TStr& OutLnDocFNm);

  // custom format files (Text-Base)
  static PBowDocBs LoadTBsTxt(
   const TStr& TBsFNm, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1);

  // custom format files (Line-Documents)
  static void LoadLnDocTxt(
   PBowDocBs BowDocBs, const TStr& LnDocFNm, TIntV& NewDIdV,
   const bool& NamedP=false, const int& MxDocs=-1, const bool& SaveDocP=false);
  static PBowDocBs LoadLnDocTxt(
   const TStr& LnDocFNm, const bool& NamedP=false, const int& MxDocs=-1,
   const PSwSet& SwSet=NULL, const PStemmer& Stemmer=NULL,
   const int& MxNGramLen=1, const int& MnNGramFq=1, const bool& SaveDocP=false);
  static PBowDocBs LoadLnDocTxt(
   const TStr& LnDocFNm, const bool& NamedP=false, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1, const bool& SaveDocP=false);
  static void SaveLnDocTxt(const PBowDocBs& BowDocBs, const TStr& FNm,
   const bool& UseDocStrP=false);

  // custom format files (Reuters-21578)
  static PBowDocBs LoadReuters21578Txt(
   const TStr& FPath, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1, const bool& SaveDocP=false);

  // custom format files (CIA-World-Fact-Book)
  static PBowDocBs LoadCiaWFBTxt(
   const TStr& FPath, const int& MxDocs=-1,
   const TStr& SwSetTypeNm="", const TStr& StemmerTypeNm="",
   const int& MxNGramLen=1, const int& MnNGramFq=1);

  // custom format files (Matlab-Sparse)
  static void SaveSparseMatlabTxt(const PBowDocBs& BowDocBs,
   const PBowDocWgtBs& BowDocWgtBs, const TStr& FNm,
   const TStr& CatFNm=TStr(), const TIntV& _DIdV=TIntV());

  // custom format files (tabular data)
  static PBowDocBs LoadTabTxt(
   const TStr& FNm, const TStr& SsFmtNm, const int& Recs,
   const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
   const int& MxNGramLen, const int& MnNGramFq,
   const int& IdFldN, const TStr& IdFldNm,
   const TIntV& CatFldNV, const TStrV& CatFldNmV,
   const TIntV& TxtFldNV, const TStrV& TxtFldNmV);
};

