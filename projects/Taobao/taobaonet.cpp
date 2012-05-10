#include "stdafx.h"
#include "taobaonet.h"
#include "trawling.h"
#include <set> 

/** TTaobaoUsr */
TTaobaoUsr::TTaobaoUsr(TSIn& SIn) : Buyer(SIn), Seller(SIn), TotalSales(SIn),
                                    TotalPurchases(SIn), Community(SIn), Rating(SIn) {
}

TTaobaoUsr::TTaobaoUsr() : Buyer(false), Seller(false), TotalSales(0),
                           TotalPurchases(0), Community(-1), Rating(-1.0) {
}

TTaobaoUsr::TTaobaoUsr(TBool b, TBool s) : Buyer(b), Seller(s), TotalSales(0),
                                           TotalPurchases(0), Community(-1), Rating(-1.0) {
}

void TTaobaoUsr::Save(TSOut& SOut) const {
  Buyer.Save(SOut);
  Seller.Save(SOut);
  TotalSales.Save(SOut);
  TotalPurchases.Save(SOut);
}

/** TTaobaoTX */
TTaobaoTX::TTaobaoTX() : Timestamp(), SalePrice() {
}

TTaobaoTX::TTaobaoTX(TSIn& SIn) : Timestamp(SIn), SalePrice(SIn), Strength(SIn) {
}

TTaobaoTX::TTaobaoTX(TSecTm& ts, TFlt& strength) : Timestamp(ts),
                                                   Strength(strength) {
}

TTaobaoTX::TTaobaoTX(TSecTm& ts, TStr& product, TFlt& transAmt, TFlt& salePrice,
                               TInt& quantity) : Timestamp(ts),
                                                 Product(product),
                                                 TransAmt(transAmt),
                                                 SalePrice(salePrice),
                                                 Quantity(quantity) {}

void TTaobaoTX::Save(TSOut& SOut) const {
  Timestamp.Save(SOut);
  SalePrice.Save(SOut);
  Strength.Save(SOut);
}

/** TTaobaoShopEdge */
TTaobaoShopEdge::TTaobaoShopEdge() : TotalSales(), TXs() {
}

TTaobaoShopEdge::TTaobaoShopEdge(TSIn& SIn) : TXs(SIn) {
}

int TTaobaoShopEdge::GetNearstTXDate(TSecTm& date){
  //printf("GetNearestTXDate\n");
  int insertIndex = -1;
  TFlt zero = TFlt(0.0);
  int index = TXs.SearchBin(TTaobaoTX(date,zero), insertIndex);
  if (index >= 0)
    return 0;
  else{
    if (insertIndex == 0)
      return -1;
    else{
      TSecTm nearestDate = TXs[insertIndex-1].Timestamp;
      return (date.GetAbsSecs()-nearestDate.GetAbsSecs())/(24*60*60);
    }
  }
  return -1;
}

TInt TTaobaoShopEdge::GetTXCntBeforeDate(TSecTm& date){
  //printf("GetTXCntBeforeDate\n");
  TInt TXCnt = 0;
  TFlt zero = TFlt(0.0);
  if (date > TXs[TXs.Len()-1].Timestamp)
    TXCnt = TXs.Len();
  else if (date < TXs[0].Timestamp)
    TXCnt = 0;
  else{
    int insertIndex = -1;
    int index = TXs.SearchBin(TTaobaoTX(date,zero), insertIndex);
    if(index >= 0)
      if (index-1>0)
        TXCnt = index;
      else
        TXCnt = 0;
    else{
      if (insertIndex > TXs.Len()-1)
        TXCnt = TXs.Len();
      else if(insertIndex == 0)
        TXCnt = 0;
      else
        if (insertIndex>0)
          TXCnt = insertIndex+1;
        else
          TXCnt = 0;
    }
  }
  return TXCnt;

}

TInt TTaobaoShopEdge::GetTXCntAfterDate(TSecTm& date){
  //printf("GetTXCntAfterDate\n");
  TInt TXCnt = 0;
  TFlt zero = TFlt(0.0);
  if (date > TXs[TXs.Len()-1].Timestamp)
    TXCnt = 0;
  else if (date < TXs[0].Timestamp)
    TXCnt = TXs.Len();
  else{
    int insertIndex = -1;
    int index = TXs.SearchBin(TTaobaoTX(date,zero), insertIndex);
    if(index >= 0)
      if (index+1<TXs.Len()-1)
        TXCnt = TXs.Len()-(index+1);
      else
        TXCnt = 0;
    else{
      if (insertIndex > TXs.Len()-1)
        TXCnt = 0;
      else if(insertIndex == 0)
        TXCnt = TXs.Len();
      else
        if (insertIndex<TXs.Len())
          TXCnt = TXs.Len()-insertIndex;
        else
          TXCnt = 0;
    }
  }
  return TXCnt;
}

TVec<TTaobaoTX> TTaobaoShopEdge::GetTXBeforeDate(TSecTm& date){
  //printf("GetTXBeforeDate\n");
  TVec<TTaobaoTX> returnTXs;
  TFlt zero = TFlt(0.0);
  if (date > TXs[TXs.Len()-1].Timestamp)
    returnTXs = TXs;
  else if (date < TXs[0].Timestamp)
    returnTXs = TVec<TTaobaoTX>();
  else{
    int insertIndex = -1;
    int index = TXs.SearchBin(TTaobaoTX(date,zero), insertIndex);
    if(index >= 0)
      TXs.GetSubValV(0, index-1, returnTXs);
    else{
      if (insertIndex > TXs.Len()-1)
        returnTXs = TXs;
      else if(insertIndex == 0)
        returnTXs = TVec<TTaobaoTX>();
      else
        TXs.GetSubValV(0, insertIndex, returnTXs);
    }
  }
  return returnTXs;
}

TVec<TTaobaoTX> TTaobaoShopEdge::GetTXAfterDate(TSecTm& date){
  //printf("GetTXAfterDate\n");
  TVec<TTaobaoTX> returnTXs;
  TFlt zero = TFlt(0.0);
  if (date > TXs[TXs.Len()-1].Timestamp)
    returnTXs = TVec<TTaobaoTX>();
  else if (date < TXs[0].Timestamp)
    returnTXs = TXs;
  else{
    int insertIndex = -1;
    int index = TXs.SearchBin(TTaobaoTX(date,zero), insertIndex);
    if(index >= 0)
      TXs.GetSubValV(index+1, TXs.Len()-1, returnTXs);
    else{
      if (insertIndex > TXs.Len()-1)
        returnTXs = TVec<TTaobaoTX>();
      else if(insertIndex == 0)
        returnTXs = TXs;
      else
        TXs.GetSubValV(insertIndex, TXs.Len()-1, returnTXs);
    }
  }
  return returnTXs;
}

bool TTaobaoShopEdge::HasTXOnDate(TSecTm& date){
  //printf("HasTXOnDate\n");
  TFlt zero = TFlt(0.0);
  return TXs.IsInBin(TTaobaoTX(date,zero));
}

TFlt TTaobaoShopEdge::CntTXAmt() const {
    TFlt AmtSum = 0;
    for (int i=0;i<TXs.Len();i++)
      AmtSum += TXs[i].GetTXAmt();
    return AmtSum ;
}   

TInt TTaobaoShopEdge::CntTXStrength() {
  TInt Total = 0;
  for (int i=0; i<TXs.Len(); i++)
    // apr: added (int) cast
    Total += (int) TXs[i].Strength;
  return Total;
}

void TTaobaoShopEdge::Save(TSOut& SOut) const {
  TXs.Save(SOut);
}


TTaobaoNet::TTaobaoNet() {
}


/* determine percentage of trades (B,S) preceded by msgs (S,B) 
STEPHEN NEW: this is not right, assumes chats are in increasing timestamp order
this is not guaranteed by the data. change later to slow version
*/
void TTaobaoNet::ConvoTrades() {
  uint window = 2*24*60*60;
  int precede=0;
  int total=0;
  for (TTaobaoShopNet::TNodeI NI = shopNet->BegNI(); NI < shopNet->EndNI(); NI++) {

    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    int inDeg = NI.GetInDeg();
    int sellNode = NI.GetId();

    for(int i=0; i<inDeg; i++) {
      TTaobaoShopEdge& shopEdge = NI.GetInEDat(i);
      int numTrades = shopEdge.CntTX();
      int buyNode = NI.GetInNId(i);
      // check if there was a message from sellNode to buyNode prior to trade
      if (msgNet->IsEdge(sellNode, buyNode)) {
        TTaobaoShopEdge& chatEdge = msgNet->GetEDat(sellNode, buyNode);
        // check how many trades are preceded by msgs, linear traversal throughlists
        int tP=0;
        int mP=0;
        int numTimes = chatEdge.CntTX();
        while (tP<numTrades && mP<numTimes) {
          TSecTm shopTime = shopEdge.GetTX(tP).Timestamp;
          TSecTm chatTime = chatEdge.GetTime(mP);
          if (shopTime < chatTime) 
            tP++;
          else { // chatTime <= shopTime
            if (chatTime.GetAbsSecs()+window<shopTime.GetAbsSecs())
              mP++;
            else { // match!
              precede++;
              //mP++;
              // allow one msg to match multiple trades
              tP++;
            }
          }
        }
      }
      
      total += numTrades;  
    }
  }
 
  printf("Precede:%d Total:%d\n", precede, total);
}

void TTaobaoNet::LoadChatNet(const TStr& ChatLog, bool IsMsg) {
  TStrV fields;
  char line[2024];
  FILE *file = fopen(ChatLog.CStr(), "r");

  int lineCount = 0;
  
  // assume that shopNet not created yet, can move to constructor later
  TTaobaoShopNet* aChatNet = new TTaobaoShopNet();

  while (!feof(file)) {
    memset(line, 0, 2024);
    fgets(line, 2024, file);
    if (strlen(line) == 0)
      break;

    TStr(line).GetTrunc().SplitOnAllCh('\t', fields, false);

    // fields now contains userid1, userid2, timestamp, product id, transaction
    // amount, list price, number of items sold

    TInt FromID = TInt(atoi(fields[0].GetTrunc().CStr())); // from id
    TInt ToID = TInt(atoi(fields[1].GetTrunc().CStr())); // id id
    TStr tStr = TStr(fields[2].GetTrunc());

    //TFlt Strength = -1;
    TFlt Strength = 1.0; // feb: use unit strength for contacts 
    if (IsMsg)
      Strength = (float)(atoi(fields[3].GetTrunc().CStr()));

    TSecTm TimeStamp;
    if (tStr.Len() == 10){
      TStrV tmFields;
      if (tStr[4] == '-') // inconsistency in taobao data!!
        tStr.SplitOnAllCh('-', tmFields, false);
      else
        tStr.SplitOnAllCh('/', tmFields, false);
      int yr = atoi(tmFields[0].CStr());
      int mon = atoi(tmFields[1].CStr());
      int day = atoi(tmFields[2].CStr());
      if (mon == 10){
        if (day == 28)
          day = 27;
        else if(day == 31)
          day = 28;
      }
      TimeStamp  = TSecTm::GetDtTm(yr, mon, day);
    }else{
      TimeStamp = TSecTm::GetZeroTm();
    }

    TTaobaoUsr FromUsr, ToUsr;
    
    if (!aChatNet->IsNode(FromID)){
      FromUsr = TTaobaoUsr( false, false); // just use false false for chatnodes
      aChatNet->AddNode(FromID, FromUsr);
    }

    if (!aChatNet->IsNode(ToID)){
      ToUsr = TTaobaoUsr(false, false);
      aChatNet->AddNode(ToID, ToUsr);
    }
    

    if (aChatNet->IsEdge(FromID, ToID)) {
      TTaobaoShopEdge& EdgeDat = aChatNet->GetEDat(FromID, ToID);
      TTaobaoTX tx = TTaobaoTX(TimeStamp, Strength);
      EdgeDat.AddTX(tx);
      // update total if seller
      //etc
    }
    else {
      //TTaobaoTX tx = TTaobaoTX(TimeStamp, Product, TransAmt, ProductPrice, Quantity);
      TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
      TTaobaoTX tx = TTaobaoTX(TimeStamp, Strength);
      EdgeDat.AddTX(tx);
      aChatNet->AddEdge(FromID, ToID, EdgeDat);
      if (!IsMsg)
        aChatNet->AddEdge(ToID, FromID, EdgeDat);
    }
    // other code to add to network
    lineCount++;
    if (lineCount % 10000 == 0) {
      fprintf(stderr, "%d lines loaded\n" , lineCount);
      //break; //debug
    }
  }
  for (TTaobaoShopNet::TEdgeI EI = aChatNet->BegEI(); EI < aChatNet->EndEI(); EI++) {
    TTaobaoShopEdge& shopEdge = EI();
    shopEdge.SortTX();
  }

  if (IsMsg)
    msgNet = aChatNet;
  else
    contactNet = aChatNet;

}

/** TTaobaoShopNet */

TInt TTaobaoShopNet::GetTXCntBeforeDate(TTaobaoShopNet::TNodeI& NI, TSecTm& date, bool In){
  TInt TXCnt = 0;
  int deg = 0; 
  if (In)
    deg = NI.GetInDeg();
  else
    deg = NI.GetOutDeg();
  for(int NodeN=0; NodeN < deg; NodeN++) {
    TTaobaoShopEdge EdgeDat;
    if (In)
      EdgeDat = NI.GetInEDat(NodeN);
    else
      EdgeDat = NI.GetOutEDat(NodeN);
    TXCnt += EdgeDat.GetTXCntBeforeDate(date);
  }
  return TXCnt;
}

//int TTaobaoShopNet::GetUsrNId(const TInt& TaobaoId) {
//  const int KeyId = UsrNIdH.GetKeyId(TaobaoId);
//  if (KeyId==-1) { return -1; }
//  else { return UsrNIdH[KeyId]; }
//}

/**
* Count all transactions going out of a node, for all nodes
*/
void TTaobaoShopNet::GetNodeTXCnt(TIntPrV& NodeTXToCntV) {
  TIntH TXToCntH;
  for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    int OutDeg = NI.GetOutDeg();
    TInt TXCnt = 0;
    for(int NodeN=0; NodeN < OutDeg; NodeN++) {
      TTaobaoShopEdge& EdgeDat = NI.GetOutEDat(NodeN);
      TXCnt += EdgeDat.CntTX();
    }
    TXToCntH.AddDat(TXCnt)++; 
  }
  NodeTXToCntV.Gen(TXToCntH.Len(), 0);
  for (int i = 0; i < TXToCntH.Len(); i++) {
    NodeTXToCntV.Add(TIntPr(TXToCntH.GetKey(i), TXToCntH[i])); }
  NodeTXToCntV.Sort();
}

/* Calculate wealth of each seller (node in-volume) 
   This quantity is bucketed
*/
void TTaobaoShopNet::GetInNodeTXAmtCnt(TFltPrV& NodeTXAmtVolToCntV) {
  //TFltFltH TXVolToCntH; // hash of volume to cnt
  TFlt maxWealth = 0;

  for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    if (t.TotalSales > maxWealth)
      maxWealth = t.TotalSales;
  }

  int bin[100]; // bin[i] stores position where bin begins
  int binCnt[100]; 
  for (int i=0; i<100; i++)
    binCnt[i] = 0;
  int numBins = 1;
  int cumSum = 0;
  int width = 1;
  bin[0] = 0;
  while (true) {
    bin[numBins] = bin[numBins-1]+width;

    if ((float) bin[numBins] > maxWealth)
      break;
    numBins++;
    width *= 2;
  }
  // binCnt is last bin
  // bin width i = bin[i+1]-bin[i]

  for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    int b=0;
    while ((float)bin[b] <= t.TotalSales)
      b++;

    binCnt[b-1]++;
  }

  //for (int i=0; i<numBins; i++)
  //  printf("%d %d\n", bin[i], binCnt[i]);
  
  //for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
  //  TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
  //  TXVolToCntH.AddDat(t.TotalSales) += 1; // add 1 to cnt for hash value t.TotalSales
  // }
  //NodeTXAmtVolToCntV.Gen(TXVolToCntH.Len(), 0);
  NodeTXAmtVolToCntV.Gen(numBins, 0);
  for (int i=0; i<numBins; i++) {
    TFlt yValue = (float)binCnt[i]/((float)(bin[i+1]-bin[i]));
    // width normalized bin count
    NodeTXAmtVolToCntV.Add(TFltPr(bin[i], yValue));
  }
  NodeTXAmtVolToCntV.Sort();
}

// binned spending
void TTaobaoShopNet::GetOutNodeTXAmtCnt(TFltPrV& NodeTXAmtVolToCntV) {
  //TFltFltH TXVolToCntH; // hash of volume to cnt
  TFlt maxSpending = 0;

  for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    if (t.TotalPurchases > maxSpending)
      maxSpending = t.TotalPurchases;
  }

  int bin[100]; // bin[i] stores position where bin begins
  int binCnt[100];
  for (int i=0; i<100; i++)
    binCnt[i] = 0;
  int numBins = 1;
  int cumSum = 0;
  int width = 1;
  bin[0] = 0;
  while (true) {
    bin[numBins] = bin[numBins-1]+width;

    if ((float) bin[numBins] > maxSpending)
      break;
    numBins++;
    width *= 2;
  }
  // binCnt is last bin
  // bin width i = bin[i+1]-bin[i]

  for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    int b=0;
    while ((float)bin[b] <= t.TotalPurchases)
      b++;

    binCnt[b-1]++;
  }

  //for (int i=0; i<numBins; i++)
  //  printf("%d %d\n", bin[i], binCnt[i]);
  //for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
  //  TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
  //  TXVolToCntH.AddDat(t.TotalSales) += 1; // add 1 to cnt for hash value
  
      // }
      //NodeTXAmtVolToCntV.Gen(TXVolToCntH.Len(), 0);
      NodeTXAmtVolToCntV.Gen(numBins, 0);
    for (int i=0; i<numBins; i++) {
      TFlt yValue = (float)binCnt[i]/((float)(bin[i+1]-bin[i]));
      // width normalized bin count
      NodeTXAmtVolToCntV.Add(TFltPr(bin[i], yValue));
    }
    NodeTXAmtVolToCntV.Sort();
}


/**
* Count sum of transactions volume (unit * price) of a node, for all nodes
*/
void TTaobaoShopNet::GetNodeTXAmtCnt(TFltPrV& NodeTXAmtToCntV, TFltPrV& NodeTXAmtVolToCntV) {
  TFltFltH TXToCntH;
  TFltFltH TXVolToCntH;
  for (TTaobaoShopNet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    int OutDeg = NI.GetOutDeg();
    TFlt TXCnt = 0;
    for(int NodeN=0; NodeN < OutDeg; NodeN++) {
      TTaobaoShopEdge& EdgeDat = NI.GetOutEDat(NodeN);
      TXCnt += EdgeDat.CntTXAmt();
    }
    TXToCntH.AddDat(TXCnt)++;
    TXVolToCntH.AddDat(TXCnt) += TXCnt;
  }
  NodeTXAmtToCntV.Gen(TXToCntH.Len(), 0);
  NodeTXAmtVolToCntV.Gen(TXToCntH.Len(), 0);

  TFlt CntTotal = 0;
  TFlt VolTotal = 0;

  for (int i = 0; i < TXToCntH.Len(); i++) {
    NodeTXAmtToCntV.Add(TFltPr(TXToCntH.GetKey(i), TXToCntH[i]));
    NodeTXAmtVolToCntV.Add(TFltPr(TXVolToCntH.GetKey(i), TXVolToCntH[i]));
    CntTotal += TXToCntH[i];
    VolTotal += TXVolToCntH[i];
  }
  NodeTXAmtToCntV.Sort();
  NodeTXAmtVolToCntV.Sort();
  TFlt sum = 0;
  for (int i = 0; i < NodeTXAmtToCntV.Len(); i++) {
    sum += NodeTXAmtToCntV[i].Val2;
    NodeTXAmtToCntV[i].Val2 = sum / CntTotal;
  }
  sum = 0;
  for (int i = 0; i < NodeTXAmtVolToCntV.Len(); i++) {
    sum += NodeTXAmtVolToCntV[i].Val2;
    NodeTXAmtVolToCntV[i].Val2 = sum / VolTotal;
  }
}


/**
* Count edge transaction for all edges
*/
void TTaobaoShopNet::GetEdgeTXCnt(TIntPrV& EdgeTXToCntV) {
  TIntH TXToCntH;
  for (TTaobaoShopNet::TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    TTaobaoShopEdge& EdgeDat = EI();
    TXToCntH.AddDat(EdgeDat.CntTX())++; 
  }
  EdgeTXToCntV.Gen(TXToCntH.Len(), 0);
  for (int i = 0; i < TXToCntH.Len(); i++) {
    EdgeTXToCntV.Add(TIntPr(TXToCntH.GetKey(i), TXToCntH[i])); }
  EdgeTXToCntV.Sort();
}


/**
* Count the sum of transaction volume (unit * price) for all edges
*/
void TTaobaoShopNet::GetEdgeTXAmtCnt(TFltPrV& EdgeTXAmtToCntV, TFltPrV& EdgeTXAmtVolToCntV){
  TFltFltH TXToCntH;
  TFltFltH TXVolToCntH;

  for (TTaobaoShopNet::TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    TTaobaoShopEdge& EdgeDat = EI();
    TXToCntH.AddDat(EdgeDat.CntTXAmt())++;
    TXVolToCntH.AddDat(EdgeDat.CntTXAmt()) += EdgeDat.CntTXAmt();
  }
  EdgeTXAmtToCntV.Gen(TXToCntH.Len(), 0);
  EdgeTXAmtVolToCntV.Gen(TXVolToCntH.Len(), 0);

  TFlt CntTotal = 0;
  TFlt VolTotal = 0;

  for (int i = 0; i < TXToCntH.Len(); i++) {
    EdgeTXAmtToCntV.Add(TFltPr(TXToCntH.GetKey(i), TXToCntH[i]));
    EdgeTXAmtVolToCntV.Add(TFltPr(TXVolToCntH.GetKey(i), TXVolToCntH[i]));
    CntTotal += TXToCntH[i];
    VolTotal += TXVolToCntH[i];
  }
  EdgeTXAmtToCntV.Sort();
  EdgeTXAmtVolToCntV.Sort();
  TFlt sum = 0;
  for (int i = 0; i < EdgeTXAmtToCntV.Len(); i++) {
    sum += EdgeTXAmtToCntV[i].Val2;
    EdgeTXAmtToCntV[i].Val2 = sum / CntTotal;
  }
  sum = 0;
  for (int i = 0; i < EdgeTXAmtVolToCntV.Len(); i++) {
    sum += EdgeTXAmtVolToCntV[i].Val2;
    EdgeTXAmtVolToCntV[i].Val2 = sum / VolTotal;
  }
}


void TTaobaoNet::LoadShopNet(const TStr& TXLog) {
  TStrV fields;
  char line[2024];
  FILE *file = fopen(TXLog.CStr(), "r"); 

  // assume that shopNet not created yet, can move to constructor later
  shopNet = new TTaobaoShopNet();

  int lineCount = 0;

  while (!feof(file)) {
    memset(line, 0, 2024); 
    fgets(line, 2024, file);
    if (strlen(line) == 0)
      break;

    TStr(line).SplitOnAllCh('\t', fields, false);
    
    // fields now contains userid1, userid2, timestamp, product id, transaction
    // amount, list price, number of items sold

    TInt BuyerID = TInt(atoi(fields[0].GetTrunc().CStr())); // buyer id
    TInt SellerID = TInt(atoi(fields[1].GetTrunc().CStr())); // seller id
 
    TSecTm TimeStamp;
    TStr tStr = fields[2].GetTrunc();


    if (tStr.Len() != 19) // check needed due to data errors
      TimeStamp = TSecTm::GetZeroTm();
    else {
      tStr = tStr.GetSubStr(0,9);
      TStrV tmFields;
      if (tStr[4] == '-') // inconsistency in taobao data!!
        tStr.SplitOnAllCh('-', tmFields, false);
      else
        tStr.SplitOnAllCh('/', tmFields, false);
      int yr = atoi(tmFields[0].CStr());
      int mon = atoi(tmFields[1].CStr());
      int day = atoi(tmFields[2].CStr());
      if (mon == 10){
        if (day == 28)
          day = 27;
        else if(day == 31)
          day = 28;
      }
      TimeStamp  = TSecTm::GetDtTm(yr, mon, day);
    }
    //printf("%s\n", TimeStamp.GetStr().CStr());

    TStr Product = TStr(fields[3].GetTrunc().CStr()); // product code
    TFlt TransAmt = TFlt(atof(fields[4].GetTrunc().CStr())); // total
    TFlt ProductPrice = TFlt(atof(fields[5].GetTrunc().CStr())); // unitprice
    TInt Quantity = TInt(atoi(fields[6].GetTrunc().CStr())); // quantity

    TTaobaoUsr BuyerUsr, SellerUsr;

    // is buyer does not exist, create NodeData and add to graph
    if (!shopNet->IsNode(BuyerID)){ 
      BuyerUsr = TTaobaoUsr( true, false);
      shopNet->AddNode(BuyerID, BuyerUsr);
    }

    // do the same for the seller
    if (!shopNet->IsNode(SellerID)){
      SellerUsr = TTaobaoUsr(false, true);
      shopNet->AddNode(SellerID, SellerUsr);
    }  

    // update seller node info\=
    TTaobaoUsr t = (TTaobaoUsr) shopNet->GetNDat(SellerID);
    t.TotalSales += TransAmt;
    t.Seller = true;
    shopNet->AddNode(SellerID, t); // updates existing node
    // update buyer node info
    TTaobaoUsr u = (TTaobaoUsr) shopNet->GetNDat(BuyerID);
    u.TotalPurchases += TransAmt;
    u.Buyer = true;
    shopNet->AddNode(BuyerID, u); // updates existing node

    if (shopNet->IsEdge(BuyerID, SellerID)) {
      TTaobaoTX tx = TTaobaoTX(TimeStamp, Product, TransAmt, ProductPrice, Quantity);
      TTaobaoShopEdge& EdgeDat = shopNet->GetEDat(BuyerID, SellerID);
      EdgeDat.AddTX(tx);
      // update total if seller
      //etc
    }
    else {
      TTaobaoTX tx = TTaobaoTX(TimeStamp, Product, TransAmt, ProductPrice, Quantity);
      TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
      EdgeDat.AddTX(tx);
      shopNet->AddEdge(BuyerID, SellerID, EdgeDat);
    }
    // other code to add to network
    lineCount++;
    if (lineCount % 10000 == 0)
      fprintf(stderr, "%d lines loaded\n" , lineCount);
  }
  //keep transactions time sorted
  for (TTaobaoShopNet::TEdgeI EI = shopNet->BegEI(); EI < shopNet->EndEI(); EI++) {
    TTaobaoShopEdge& shopEdge = EI();
    shopEdge.SortTX();
  }

}

void TTaobaoNet::LoadProduct(const TStr& filename) {
  // prod_id, prod_price, median_price_of_cat, cat_brand_seq hash
  // load file, count number of unique categories
  // number of "product choices"
  // how many egregious seqs 

  TStrV fields;
  char line[2024];
  FILE *file = fopen(filename.CStr(), "r");

  //printf("%s", filename.CStr());

  int lineCount = 0;
  int repeatCnt=0;
  int difCnt=0;

  while (!feof(file)) {
    memset(line, 0, 2024);
    fgets(line, 2024, file);
    if (strlen(line) == 0)
      break;

    TStr(line).SplitOnAllCh('\t', fields, false);
    
    TStr ProdID = TStr(fields[0].GetTrunc().CStr());
    TFlt ProdPrice = TFlt(atof(fields[1].GetTrunc().CStr()));
    TFlt CatMed = TFlt(atof(fields[2].GetTrunc().CStr()));
    TStr CatID = TStr(fields[3].GetTrunc().CStr());

    //printf("%s %f %f %s\n", ProdID.CStr(), (float)ProdPrice, (float)CatMed, CatID.CStr());
    
    TStrPr ProdKey = TStrPr(ProdID, fields[1].GetTrunc());

    if (prodCat.IsKey(ProdKey)) {
      TStr oldCat = (TStr) prodCat.GetDat(ProdKey);
      if (oldCat != CatID) {
        difCnt++; // can product be in 2 cats
        printf("%s %s %s %s\n", ProdKey.Val1.CStr(), ProdKey.Val2.CStr(), oldCat.CStr(), CatID.CStr());
      }
      repeatCnt++;
    }
    else {
      prodCat.AddDat(ProdKey, CatID);
    }

    lineCount++;
  }

  printf("%d %d\n", repeatCnt, difCnt);

}

// May: load seller ratings
void TTaobaoNet::LoadRating(const TStr& filename) {
  TStrV fields;
  char line[2024];
  FILE *file = fopen(filename.CStr(), "r");

  printf("%s", filename.CStr());

  int lineCount = 0;

  while (!feof(file)) {
    memset(line, 0, 2024);
    fgets(line, 2024, file);
    if (strlen(line) == 0)
      break;

    TStr(line).SplitOnAllCh('\t', fields, false);

    TInt SellerID = TInt(atoi(fields[0].GetTrunc().CStr())); // seller id
    TFlt Rating = TFlt(atof(fields[1].GetTrunc().CStr()));
    
    printf("%d %f\n", (int)SellerID, (float)Rating);

    if (shopNet->IsNode(SellerID)) {
      TTaobaoUsr t = (TTaobaoUsr) shopNet->GetNDat(SellerID);
      t.Rating = Rating;
      shopNet->AddNode(SellerID, t); // updates existing node
    }
    
    lineCount++;
  }

  printf("%d ratings loaded\n", lineCount);

  int buyerCnt=0;
  int sellerCnt=0;
  int ratingDeg=0;
  int ratingCnt=0;
  // get some seller rating stats
  for (TTaobaoShopNet::TNodeI NI = shopNet->BegNI(); NI < shopNet->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr t = (TTaobaoUsr) shopNet->GetNDat(ID);
   
    // avg deg of sellers with ratings
    if (t.Rating != -1.0) {
      ratingDeg += NI.GetInDeg();
      ratingCnt++;
    }
    
    if (NI.GetInDeg() > NI.GetOutDeg())
      sellerCnt++;
    else
      buyerCnt++;
  }

  printf("Num Buyers %d Num Sellers %d\n", buyerCnt, sellerCnt);
  printf("Num Rated Sellers %d Avg In-Degree of Rated Seller: %f\n", ratingCnt,(float)ratingDeg/(float)ratingCnt);
 
  fclose(file);
}

/*
load tradenet
load community information
draw large communities in pajek
vary node size
vary node color (based upon buyer/seller proportion??)
edges between community 
 */

void TTaobaoNet::LoadCommunity(const TStr& filename){
  int MAXCOM = 1000000;
  int MINCOMSIZE = 0; 
  int MINTRADESFOREDGE = -1;
  
  int ignore[MAXCOM];
  int comSize[MAXCOM];
  int **community; // alloc int * to store ids for each community
  int **trades; // store trade strength betwen each community
  int NUMCOM = 0;
  
  int inputLineLength = 100000000;
  char* buffer = new char[inputLineLength];
  char* tokBuf = new char[inputLineLength];

  community  = (int **) malloc(MAXCOM * sizeof(int*));

  //char buffer[100000], tokBuf[100000];
  FILE *in = fopen(filename.CStr(), "r");
  // ignore initial part of file
  while (fgets(buffer, inputLineLength, in)) {
    if (buffer[0] == '*')
      break;
  }

  while (fgets(buffer, inputLineLength, in)) {
    if (buffer[0] == '*')
      break;

    // first pass get size of community
    strcpy(tokBuf, buffer);
    int numCnt = 0;
    char * pch;
    pch = strtok (tokBuf," \n");
    while (pch != NULL)
    {
        comSize[NUMCOM]++;
        //printf ("%s",pch);
        pch = strtok (NULL, " \n");
    }

    //printf("%s", buffer);

    if (comSize[NUMCOM] > MINCOMSIZE) {
    
      community[NUMCOM] = (int *) malloc(comSize[NUMCOM]*sizeof(int));
      
      // second pass: store nodes of each community
      strcpy(tokBuf, buffer);
      pch = strtok (tokBuf," \n");
      int t=0;
      while (pch != NULL)
        {
          community[NUMCOM][t] = atoi(pch);
          if (shopNet->IsNode(community[NUMCOM][t])){
            TTaobaoUsr usr = (TTaobaoUsr) shopNet->GetNDat(community[NUMCOM][t]);
            usr.Community = NUMCOM;
            shopNet->AddNode(community[NUMCOM][t], usr);
            t++;
          }
          //printf ("%s",pch);
          pch = strtok (NULL, " \n");
        }
      
      NUMCOM++;
    }
    else // ignore this line and continue, community too small
      comSize[NUMCOM] = 0;
  }
  fclose(in);
}

