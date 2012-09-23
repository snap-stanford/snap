/////////////////////////////////////////////////
/// Graph Utilities
class TGUtil {
public:
  static void GetCdf(const TIntPrV& PdfV, TIntPrV& CdfV);
  static void GetCdf(const TFltPrV& PdfV, TFltPrV& CdfV);
  static void GetCdf(const TIntFltKdV& PdfV, TIntFltKdV& CdfV);
  static TIntPrV GetCdf(const TIntPrV& PdfV);
  static TFltPrV GetCdf(const TFltPrV& PdfV);

  static void GetCCdf(const TIntPrV& PdfV, TIntPrV& CCdfV);
  static void GetCCdf(const TFltPrV& PdfV, TFltPrV& CCdfV);
  static void GetCCdf(const TIntFltKdV& PdfV, TIntFltKdV& CCdfV);
  static TIntPrV GetCCdf(const TIntPrV& PdfV);
  static TFltPrV GetCCdf(const TFltPrV& PdfV);

  static void GetPdf(const TIntPrV& CdfV, TIntPrV& PdfV);
  static void GetPdf(const TFltPrV& CdfV, TFltPrV& PdfV);
  static void GetPdf(const TIntFltKdV& CdfV, TIntFltKdV& PdfV);

  static void Normalize(TFltPrV& PdfV);
  static void Normalize(TIntFltKdV& PdfV);

  static void MakeExpBins(const TFltPrV& XYValV, TFltPrV& ExpXYValV,
    const double& BinFactor = 2, const double& MinYVal = 1);
  static void MakeExpBins(const TFltKdV& XYValV, TFltKdV& ExpXYValV,
    const double& BinFactor = 2, const double& MinYVal = 1);
  static void MakeExpBins(const TFltV& YValV, TFltV& ExpYValV, const double& BinFactor = 1.01);
  static void MakeExpBins(const TIntV& YValV, TIntV& ExpYValV, const double& BinFactor = 1.01);
};

/////////////////////////////////////////////////
/// String helper functions and utilities. Quick and ditry!
class TStrUtil {
public:
  static TChA& GetXmlTagVal(TXmlLx& XmlLx, const TChA& TagNm);
  static void GetXmlTagNmVal(TXmlLx& XmlLx, TChA& TagNm, TChA& TagVal);
  static bool GetXmlTagNmVal2(TXmlLx& XmlLx, TChA& TagNm, TChA& TagVal, const bool& TakeTagNms);
  static TChA GetDomNm(const TChA& UrlChA);  // www.cs.cmu.edu
  static TChA GetDomNm2(const TChA& UrlChA); // also strip starting www.
  static TChA GetWebsiteNm(const TChA& UrlChA); // get website (GetDomNm2 or blog url)
  static bool GetNormalizedUrl(const TChA& UrlIn, const TChA& BaseUrl, TChA& UrlOut);
  static bool StripEnd(const TChA& Str, const TChA& SearchStr, TChA& NewStr);
  
  static TChA GetShorStr(const TChA& LongStr, const int MaxLen=50);
  static TChA GetCleanStr(const TChA& ChA);
  static TChA GetCleanWrdStr(const TChA& ChA);
  static int CountWords(const char* CStr);
  static int CountWords(const TChA& ChA);
  static int CountWords(const TChA& ChA, const TStrHash<TInt>& StopWordH);
  static int SplitWords(TChA& ChA, TVec<char *>& WrdV, const bool& SplitOnWs=true);
  static int SplitOnCh(TChA& ChA, TVec<char *>& WrdV, const char& Ch, const bool& SkipEmpty=false);
  static int SplitLines(TChA& ChA, TVec<char *>& LineV, const bool& SkipEmpty=false);
  static int SplitSentences(TChA& ChA, TVec<char *>& SentenceV);
  static void RemoveHtmlTags(const TChA& HtmlStr, TChA& TextStr);
  static bool IsLatinStr(const TChA& Str, const double& MinAlFrac);
  static void GetWIdV(const TStrHash<TInt>& StrH, const char *CStr, TIntV& WIdV);
  static void GetAddWIdV(TStrHash<TInt>& StrH, const char *CStr, TIntV& WIdV);
  /// Parses time in many different text formats. See source code for details.
  static bool GetTmFromStr(const char* TmStr, TSecTm& Tm);

  /// Puts person's name (fist middle last) in a standard form: <last_name>_<first name innitial>
  static TStr GetStdName(TStr AuthorName);
  /// Splits a list of people's names.
  static void GetStdNameV(TStr AuthorNames, TStrV& StdNameV);
};
