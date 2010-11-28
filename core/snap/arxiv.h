#ifndef snap_arxiv
#define snap_arxiv

#include "Snap.h"

/////////////////////////////////////////////////
// Arxiv List of Papers
class TArxivPaperList {
public:
  class TArxivPaper {
  public:
    TInt Id;
    TTm Time;
    TStr Title;
    TStrV AuthorV;
  public:
    TArxivPaper() { }
    bool SetData(const TStrV& LineV);
  };
private:
  PSIn SIn;
public:
  TInt Id;
  TSecTm Time;
  TStr Title;
  TStrV AuthorV;
public :
  TArxivPaperList() { }
  TArxivPaperList(const TStr& InFNm) : SIn(TZipIn::IsZipFNm(InFNm)?TZipIn::New(InFNm):TFIn::New(InFNm)) { }
  bool Next();
  
  static void MakeCoAuthNet(const TStr& FNm, const TStr& OutFNm);
  static void ParseFiles();
  static void MapHepPhRandDocIds();

  static void LoadAndSaveASGraphs();
};

#endif
