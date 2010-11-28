#ifndef medline_h
#define medline_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "net.h"

/////////////////////////////////////////////////
// Medline-Web-Fetch
class TMedlineWebFetch: public TWebPgFetch{
private:
  int MnDocId, MxDocId;
  int WaitSecs;
  TStr FPath;
public:
  TMedlineWebFetch(
   const int& _MnDocId, const int& _MxDocId,
   const int& _WaitSecs, const TStr& _FPath):
    TWebPgFetch(), MnDocId(_MnDocId), MxDocId(_MxDocId),
    WaitSecs(_WaitSecs), FPath(_FPath){}

  void OnFetch(const int& FId, const PWebPg& WebPg);
  void OnError(const int& FId, const TStr& MsgStr);

  static int GetNextStartDocId(const TStr& FPath);
  static TStr GetFNm(const TStr& FPath, const int& MnDocId, const int& MxDocId);

  static void Go(
   const int& StartDocId=1, const int& Docs=12500000, const int& DocSetLen=100,
   const int& WaitSecs=1, const TStr& FPath=".");
};


#endif
