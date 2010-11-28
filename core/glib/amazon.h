#ifndef amazon_h
#define amazon_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "graph.h"

/////////////////////////////////////////////////
// Amazon-Item
ClassTP(TAmazonItem, PAmazonItem)
private:
  TStr ItemId;
  TStr TitleStr;
  TStrV AuthorNmV;
  TStrV NextItemIdV;
public:
  TAmazonItem():
    ItemId(), TitleStr(), AuthorNmV(), NextItemIdV(){}
  TAmazonItem(
   const TStr& _ItemId, const TStr& _TitleStr,
   const TStrV& _AuthorNmV, const TStrV& _NextItemIdV):
    ItemId(_ItemId), TitleStr(_TitleStr),
    AuthorNmV(_AuthorNmV), NextItemIdV(_NextItemIdV){}

  // retrieve components
  TStr GetItemId() const {return ItemId;}
  TStr GetTitleStr() const {return TitleStr;}
  int GetAuthorNms() const {return AuthorNmV.Len();}
  TStr GetAuthorNm(const int& AuthorNmN) const {return AuthorNmV[AuthorNmN];}
  int GetNextItemIds() const {return NextItemIdV.Len();}
  TStr GetNextItemId(const int& ItemIdN) const {return NextItemIdV[ItemIdN];}

  // string representation
  TStr GetStr() const;

  // xml
  static PAmazonItem New(const PXmlDoc& XmlDoc);
  PXmlDoc GetXmlDoc() const;

  // filters from html
  static PAmazonItem GetFromWebPg(const PWebPg& WebPg);
  static TStr GetItemId(const PUrl& Url){
    return Url->GetPathSeg(3);}
  static TStr GetUrlStr(const TStr& ItemId){
    return TStr("http://www.amazon.com/exec/obidos/ASIN/")+ItemId;}
};

/////////////////////////////////////////////////
// Amazon-Item-Base
ClassTP(TAmazonItemBs, PAmazonItemBs)
private:
  THash<TStr, PAmazonItem> IdToItemH;
public:
  TAmazonItemBs(): IdToItemH(100000){}
  static PAmazonItemBs New(){return PAmazonItemBs(new TAmazonItemBs());}

  bool IsItem(const TStr& ItemId) const {
    return IdToItemH.IsKey(ItemId);}
  bool IsItem(const PAmazonItem& AmazonItem) const {
    return IdToItemH.IsKey(AmazonItem->GetItemId());}
  int GetItems() const {return IdToItemH.Len();}
  TStr GetItemId(const int& ItemIdN) const {return IdToItemH.GetKey(ItemIdN);}
  int GetItemIdN(const TStr& ItemId) const {return IdToItemH.GetKeyId(ItemId);}
  PAmazonItem GetItem(const int& ItemIdN) const {return IdToItemH[ItemIdN];}
  PAmazonItem GetItem(const TStr& ItemId) const {return IdToItemH.GetDat(ItemId);}
  void AddItem(const PAmazonItem& AmazonItem){
    IdToItemH.AddDat(AmazonItem->GetItemId(), AmazonItem);}
  void AddItemBs(const PAmazonItemBs& AmazonItemBs);

  void GetVoidItemIdV(TStrV& VoidItemIdV) const;
  PGraph GetGraph() const;

  static PAmazonItemBs LoadXml(const TStr& FNm, const int& MxItems=-1);
  void SaveXml(const TStr& FNm);
};

/////////////////////////////////////////////////
// Amazon-Page-Fetch
ClassTPE(TAmazonFetch, PAmazonFetch, TWebPgFetch)
private:
  TStrH ItemItemIdH;
  PSOut XmlSOut;
public:
  TAmazonFetch();
  static PAmazonFetch New(){return PAmazonFetch(new TAmazonFetch());}
  static PAmazonFetch New(const TStr& FNm);
  static PAmazonFetch New(const PAmazonItemBs& AmazonItemBs);

  bool IsItem(const TStr& ItemId) const {return ItemItemIdH.IsKey(ItemId);}
  int GetItems() const {return ItemItemIdH.Len();}
  void AddItem(const TStr& ItemId){ItemItemIdH.AddDat(ItemId);}
  void FetchItemIdV(const TStrV& ItemIdV);

  void OnFetch(const int& FId, const PWebPg& WebPg);
  void OnError(const int& FId, const TStr& MsgStr);
};

#endif
