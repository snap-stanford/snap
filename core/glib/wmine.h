#ifndef wmine_h
#define wmine_h

/////////////////////////////////////////////////
// Includes
#include "tmine.h"

/////////////////////////////////////////////////
// Web-Log
ClassTP(TWebLog, PWebLog)//{
private:
  TStrIntH UsrNmToClicksH;
  TStrIntH UrlNmToClicksH;
  TIntPrV UsrIdUrlIdPrV;
  TIntPrV UrlIdUsrIdPrV;
public:
  TWebLog():
    UsrNmToClicksH(), UrlNmToClicksH(),
    UsrIdUrlIdPrV(), UrlIdUsrIdPrV(){}
  static PWebLog New(){return new TWebLog();}
  TWebLog(const TWebLog&){Fail;}
  TWebLog(TSIn& SIn):
    UsrNmToClicksH(SIn), UrlNmToClicksH(SIn),
    UsrIdUrlIdPrV(SIn), UrlIdUsrIdPrV(SIn){}
  static PWebLog Load(TSIn& SIn){return new TWebLog(SIn);}
  void Save(TSOut& SOut){
    UsrNmToClicksH.Save(SOut); UrlNmToClicksH.Save(SOut);
    UsrIdUrlIdPrV.Save(SOut); UrlIdUsrIdPrV.Save(SOut);}

  TWebLog& operator=(const TWebLog&){Fail; return *this;}

  // user/url data
  int GetUsrs() const {return UsrNmToClicksH.Len();}
  int GetUrls() const {return UrlNmToClicksH.Len();}
  TStr GetUsrNm(const int& UsrId) const {return UsrNmToClicksH.GetKey(UsrId);}
  TStr GetUrlNm(const int& UrlId) const {return UrlNmToClicksH.GetKey(UrlId);}
  // click data
  int GetClicks() const {return UsrIdUrlIdPrV.Len();}
  void GetUsrIdUrlIdPr(const int& PrN, int& UsrId, int& UrlId){
    TIntPr& Pr=UsrIdUrlIdPrV[PrN]; UsrId=Pr.Val1; UrlId=Pr.Val2;}
  void GetUrlIdUsrIdPr(const int& PrN, int& UrlId, int& UsrId){
    TIntPr& Pr=UrlIdUsrIdPrV[PrN]; UrlId=Pr.Val1; UsrId=Pr.Val2;}

  // ranking
  static void GetUsrUrlRankStep(
   const double& TaxFact, const TIntPrV& LRIdPrV,
   TFltIntKdV& WgtLIdKdV, TFltIntKdV& WgtRIdKdV, double& WgtDiff);
  void GetUsrUrlRank(TFltIntKdV& WgtUsrIdKdV, TFltIntKdV& WgtUrlIdKdV) const;
  void SaveTxtUURank(const PSOut& SOut,
   const TFltIntKdV& WgtUsrIdKdV, const TFltIntKdV& WgtUrlIdKdV,
   const int& TopRecs=100) const;
  void SaveXmlUURank(const PSOut& SOut,
   const TFltIntKdV& WgtUsrIdKdV, const TFltIntKdV& WgtUrlIdKdV,
   const int& TopRecs=100) const;

  // convert to bag-of-words
  PBowDocBs GetBowDocBs(
   const TStrIntH& DocNmToFqH, const TIntPrV& DIdWIdPrV,
   const TStrIntH& WordStrToFqH, const int& MnFq) const;
  PBowDocBs GetUsrBowDocBsFromUrl(const int& MnFq) const;
  PBowDocBs GetUrlBowDocBsFromUsr(const int& MnFq) const;
  PBowDocBs GetUsrBowDocBsFromHtml(const int& MnFq, const TStr& WebRootFPath);

  // files
  // ... binary
  static PWebLog LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  // ... text
  static PWebLog LoadTxt(const TStr& FNm, const int& MxRecs=-1,
   const TSsFmt& SsFmt=ssfTabSep, const bool& HdLnP=false);
  static PWebLog LoadBinOrTxt(
   const TStr& BinFNm, const TStr& TxtFNm, const int& MxRecs=-1);
  // ... Yield-Broker
  static PWebLog LoadYbTxt(const TStr& FNm, const int& MxRecs=-1);
};

#endif
