#ifndef snap_taobaonet_h
#define snap_taobaonet_h
#include "Snap.h"

// this will store both ChatNet and ShopNet
class TTaobaoNet;
typedef TPt<TTaobaoNet> PTaobaoNet;
//class TTaobaoChatNet;
//typedef TPt<TTaobaoChatNet> PTaobaoChatNet;
class TTaobaoShopNet;
typedef TPt<TTaobaoShopNet> PTaobaoShopNet;
typedef THash<TStrPr, TStrPr> TStrPrStrPrH;
enum JobType {FWDJOB, MSGJOB, CONTACTJOB};

/**
 * TTaobaoUsr
 */ 
class TTaobaoUsr {
 public:
  TBool Buyer;
  TBool Seller;
  TFlt TotalSales; // needed because graph doesn't have indegree feature
  TFlt TotalPurchases; // extra, for convenience
  TInt Community; // community number, temp for community graphing
  TFlt Rating;
  // insert other info like reputation later
 public:
  TTaobaoUsr();
  TTaobaoUsr(TBool b, TBool s); // new constructor
  TTaobaoUsr(TSIn& SIn);

  void Save(TSOut& SOut) const;

  bool IsBuyer() const { return Buyer; }
  bool IsSeller() const { return Seller; }

  //friend class TTaobaoChatNet;
  friend class TTaobaoShopNet;
};

/**
 * TTaobaoTX 
 */ 
class TTaobaoTX {
 public: // for convenience 
  TSecTm Timestamp;
  TFlt SalePrice; // Unit Price 
  TFlt TransAmt; // Unit Price * Quantity + Delivery 
  TStr Product; // Product Sig
  TInt Quantity;
  TFlt Strength;

 public:
  TTaobaoTX();
  TTaobaoTX(TSIn& SIn);
  TTaobaoTX(TSecTm& ts, TStr& product, TFlt& transAmt, TFlt& salePrice,
                 TInt& quantity);
  TTaobaoTX(TSecTm& ts, TFlt& strength);
  bool operator < (const TTaobaoTX& tx) const { return Timestamp <
      tx.Timestamp; }
  bool operator > (const TTaobaoTX& tx) const { return Timestamp >
      tx.Timestamp; }
  bool operator == (const TTaobaoTX& tx) const { return Timestamp == 
      tx.Timestamp; }
  // TODO: create a constructor for transactions
  void Save(TSOut& SOut) const;
  TFlt GetTXAmt() const { return TransAmt; }
};


/**
 * TTaobaoShopEdge
 */
class TTaobaoShopEdge {
 private:
  TVec<TTaobaoTX> TXs;
  TFlt TotalSales;

 public:
  TTaobaoShopEdge();
  TTaobaoShopEdge(TSIn& SIn);
  bool operator < (const TTaobaoShopEdge& Edge) const { return CntTXAmt() < Edge.CntTXAmt() ; }

  void AddTX(TTaobaoTX& tx) { TXs.Add(tx); }

  TTaobaoTX& GetTX(int num) { return TXs[num]; }

  TInt CntTX() { return TXs.Len(); }

  void SortTX() {TXs.Sort(); }

  TFlt CntTXAmt() const; // total sales 

  TInt CntTXStrength(); // total chat volume

  void Save(TSOut& SOut) const;

  TSecTm& GetTime(int num) { return TXs[num].Timestamp; }

  // apr: just changed from tflt to tflt&, check everything okay
  TFlt& GetStrength(int num) { return TXs[num].Strength; }

  TInt GetTXCntBeforeDate(TSecTm& date);
  TInt GetTXCntAfterDate(TSecTm& date);

  TVec<TTaobaoTX> GetTXBeforeDate(TSecTm& date);
  TVec<TTaobaoTX> GetTXAfterDate(TSecTm& date);

  int GetNearstTXDate(TSecTm& date);

  bool HasTXOnDate(TSecTm& date);

  // new: hack, needed when creating combined network
  bool isCon;
  bool isMsg;
  bool isTrade;
  int edgeType; // apr
  int firstMsg; // oct: for quick time checks
  int firstTrade;
  friend class TTaobaoShopNet;
};


/**
 * TTaobaoShopNet
 */
class TTaobaoShopNet : public TNodeEDatNet<TTaobaoUsr, TTaobaoShopEdge> {
 private:
  //THash<TInt, TInt> UsrNIdH; // don't think this is needed
  TCRef CRef;

 public:
  TTaobaoShopNet() : TNet() { }
  
  static PTaobaoShopNet New() { return new TTaobaoShopNet; }
  //static PTaobaoShopNet LoadShopNet(const TStr& TXLog);

  //int GetUsrNId(const TInt& TaobaoId);
  //bool IsUsr(const TInt& TaobaoId);
  void GetNodeTXCnt(TIntPrV& NodeTXToCntV);
  void GetNodeTXAmtCnt(TFltPrV& NodeTXAmtToCntV, TFltPrV& NodeTXAmtVolToCntV);
  void GetEdgeTXCnt(TIntPrV& EdgeTXToCntV);
  void GetEdgeTXAmtCnt(TFltPrV& EdgeTXAmtToCntV, TFltPrV& EdgeTXAmtVolToCntV);
  void GetInNodeTXAmtCnt(TFltPrV& NodeTXAmtVolToCntV);
  void GetOutNodeTXAmtCnt(TFltPrV& NodeTXAmtVolToCntV);

  TInt GetTXCntBeforeDate(TTaobaoShopNet::TNodeI& NI, TSecTm& date, bool In);
  
  //find the communities with highest volume output

  friend class TPt<TTaobaoShopNet>;
};

// store global node info
//template <>
class TTaobaoNet {
 private:
  TCRef CRef;

 public:
  //struct HeapElem{
  //  TInt i;
  //  TInt j;
  //  TFlt val;
  //};
  //class CompareHeapElem
  //{
  //  public:
  //  // Compare two Foo structs.
  //  bool operator()(const HeapElem& x, const HeapElem& y) const
  //  {
  //    return x.val < y.val;
  //  }
  //};

  TTaobaoShopNet* msgNet;
  TTaobaoShopNet* contactNet;
  TTaobaoShopNet* shopNet;

  // mapping tables between products and categories
  // product strings are not unique, so we use TStrPr(product, price)
  TStrStrPrVH catProd;
  TStrPrStrH prodCat;
  TStrPrStrPrH prodSeller; // map prod to (buyer,seller)ppair
  // this will allow lookup of price, time, etc.

  // store global node info
  //THash<TInt, TTaobaoUsr> usrInfo;

  //TStrHash<TInt> StrIdH; // maps digital sigs to taobao ids

  TTaobaoNet(); // change to call other constructors
  //bool HeapCompare(HeapElem const& x, HeapElem const& y);

  void LoadShopNet(TStr const& TXLog);
  void LoadChatNet(TStr const& ChatLog, bool IsMsg);
  void LoadRating(TStr const& FileName);
  void LoadProduct(TStr const& FileName);
  void LoadCommunity(TStr const& CommunityFileName);
  //void CommunityMining(PTaobaoShopNet const& PG);

  void ConvoTrades();
  static PTaobaoNet New() { return new TTaobaoNet; }

  friend class TTaobaoShopNet;
  friend class TPt<TTaobaoNet>;
};



#endif
