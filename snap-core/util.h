//#//////////////////////////////////////////////
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

//#//////////////////////////////////////////////
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

//#//////////////////////////////////////////////
/// Routines to benchmark table operations

class TStopwatch {
public:
  /// List of all experiments
  typedef enum {LoadTables, Preprocess, ConstructGraph, Compute, Postprocess, StoreOutputs,
      // graph construction
      AllocateColumnCopies, CopyColumns, Sort, Group, MergeNeighborhoods, AddNeighborhoods, AddEdges, Sort2, ComputeOffset,
      // subgraph construction
      ComputeETypes, EstimateSizes, InitGraph, ExtractNbrETypes, CopyNodes, PopulateGraph, ExtractEdges, BuildSubgraph} TExperiment;
  /// The expected number of experiments (must be at least equal to the size of the above list)
  static const int NEXPS = 25;

  static TStopwatch* GetInstance() {
    static TStopwatch instance; // Guaranteed to be destroyed. Instantiated on first use.
    return &instance;
  }

private:
  TStopwatch() {
    for (int i = 0; i < NEXPS; i++) { Mins[i] = 100000000000.0; }
  }; // Constructor? (the {} brackets) are needed here.

  // Dont forget to declare these two. You want to make sure they
  // are unacceptable otherwise you may accidentally get copies of
  // your singleton appearing.
  TStopwatch(TStopwatch const&);              // Don't Implement
  void operator=(TStopwatch const&); // Don't implement

  double Starts[NEXPS];
  int Cnts[NEXPS];
  double Sums[NEXPS];
  double Maxs[NEXPS];
  double Mins[NEXPS];

private:
  double Tick();

public:
  /// Start a new experiment
  void Start(const TExperiment Exp);
  /// Stop the current experiment
  void Stop(const TExperiment Exp);
  /// Returns the number of experiments
  int Cnt(const TExperiment Exp) const;
  /// Returns the total time of all experiments
  double Sum(const TExperiment Exp) const;
  /// Returns the average time of all experiments
  double Avg(const TExperiment Exp) const;
  /// Returns the maximum time of all experiments
  double Max(const TExperiment Exp) const;
  /// Returns the minimum time of all experiments
  double Min(const TExperiment Exp) const;
};

//#//////////////////////////////////////////////
/// Snapworld supporting functions

#if defined(SW_WRITEN)

/// Writes \c nbytes bytes starting at \c ptr to a file/socket descriptor \c fd.
extern int WriteN(int fd, char *ptr, int nbytes);

/// Sends the vector contents \c V via a file/socket descriptor \c FileDesc. ##SendVec
template <class TVal, class TSizeTy>
int64 SendVec(const TVec<TVal, TSizeTy>& V, int FileDesc) {
  int64 l = 0;
  int n;
  int r;
  TSizeTy Vals = V.Len();
  int ChunkSize = 25600;

  r = WriteN(FileDesc, (char *) &Vals, (int) sizeof(TSizeTy));
  if (r < 0) {
    return r;
  }
  l += r;

  r = WriteN(FileDesc, (char *) &Vals, (int) sizeof(TSizeTy));
  if (r < 0) {
    return r;
  }
  l += r;

  for (TSizeTy ValN = 0; ValN < Vals; ValN += ChunkSize) {
    n = ChunkSize;
    if ((Vals - ValN) < ChunkSize) {
      n = Vals - ValN;
    }
    r = WriteN(FileDesc, (char *) &V[ValN], (int) (n*sizeof(TVal)));
    if (r < 0) {
      return r;
    }
    l += r;
  }
  return l;
}


/// Sends the segmented vector contents \c V via a file/socket descriptor \c FileDesc. ##SendVec64
template <class TVal, class TSizeTy>
int64 SendVec64(const TVec< TVec< TVal, TSizeTy > , TSizeTy >&Vec64, int FileDesc) {
  TSizeTy N =Vec64.Len();
  int64 l=0;
  int r;

  r = WriteN(FileDesc, (char *) &N, (int) sizeof(TSizeTy));
  if (r < 0) {
    return r;
  }
  l += r;

  r = WriteN(FileDesc, (char *) &N, (int) sizeof(TSizeTy));
  if (r < 0) {
    return r;
  }
  l += r;

  for (typename TVec< TVec< TVal, TSizeTy >, TSizeTy >::TIter it=Vec64.BegI(); it!=Vec64.EndI(); ++it) {
    r = SendVec(*it, FileDesc);
    if (r < 0) {
      return r;
    }
    l += r;
  }

  return l;
}
#endif
