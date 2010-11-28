#ifndef webmb_h
#define webmb_h

/////////////////////////////////////////////////
// Includes
#include "webold.h"

/////////////////////////////////////////////////
// Web-Memory-Base
class TWebMemBs: public TXWebBs{
private:
  static const TStr WebMemBsFNm;
  static const int ExpectedWebPgs;
  TStrIntH UrlStrToWebPgIdH;
  TStrIntH CheckedUrlStrToWebPgIdH;
  TIntStrVH WebPgIdToUrlStrVH;
  TIntStrVH WebPgIdToRefUrlStrVH;
  bool Modified;
  void OnSave(const int& WebPgId, const PXWebPg& WebPg);
  void OnFetch(const int& WebPgId, const PXWebPg& WebPg);
public:
  TWebMemBs(const int& MxConns, const TStr& FPath, const bool& GenRef):
    TXWebBs(MxConns, FPath, GenRef),
    UrlStrToWebPgIdH(ExpectedWebPgs),
    CheckedUrlStrToWebPgIdH(),
    WebPgIdToUrlStrVH(ExpectedWebPgs),
    WebPgIdToRefUrlStrVH(ExpectedWebPgs),
    Modified(false){}
  TWebMemBs(const TStr& _FPath, const bool& _CheckMode=false);
  void SaveTxt();
  virtual ~TWebMemBs(){if (Modified){SaveTxt();}}

  TWebMemBs& operator=(const TWebMemBs&){Fail; return *this;}

  TStr GetWebPgFNm(const int& WebPgId){
    return GetFPath()+TInt::GetStr(WebPgId)+".http";}

  int GetWebPgs(){return WebPgIdToUrlStrVH.Len();}
  int GetCheckedUrls(){return CheckedUrlStrToWebPgIdH.Len();}
  int AddWebPg(const TStr& RelUrlStr, const TStr& BaseUrlStr=TStr());
  bool IsWebPg(const TStr& UrlStr);
  bool IsWebPg(const TStr& UrlStr, const bool& Check);
  PXWebPg GetWebPg(const int& WebPgId);
  PXWebPg GetWebPg(const TStr& UrlStr);
  void AddWebPgRef(const TStr& UrlStr, const TStr& RefUrlStr);
  bool DoStillFetchUrl(const TStr& UrlStr);
  bool DoFetchRedirUrl(const PUrl& RedirUrl, const TUrlV& UrlV);

  bool IsUrlStr(const TStr& UrlStr){
    return UrlStrToWebPgIdH.IsKey(UrlStr);}
  int GetUrlStrs(const int& WebPgId){
    return WebPgIdToUrlStrVH.GetDat(WebPgId).Len();}
  TStr GetUrlStr(const int& WebPgId, const int& UrlStrN=0){
    return WebPgIdToUrlStrVH.GetDat(WebPgId)[UrlStrN];}
  TStr GetUrlStr(const TStr& UrlStr){
    int UrlStrToWebPgIdP=UrlStrToWebPgIdH.GetKeyId(UrlStr);
    return UrlStrToWebPgIdH.GetKey(UrlStrToWebPgIdP);}
  int GetWebPgId(const TStr& UrlStr){
    return UrlStrToWebPgIdH.GetDat(UrlStr);}

  int FFirstWebPg();
  bool FNextWebPg(int& WebPgP, int& WebPgId);
};

#endif
