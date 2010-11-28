#include "Snap.h"
#include "stdafx.h"
#include "ncp.cpp"
#include "taobaonet.h"
#include "graphing.h"
#include <stdio.h>
#include <math.h>

// moved this to graphing.h // add this to svn later
/* binning from CommunityStats */
/*
void binning(TFltPrV& data, char *graphName, float initialWidth, float
             binWidthFactor, bool logScale, char *title,
             char *xAxis, char *yAxis, TFltPrV& output) {
  // represent x axis point as a single node
  // y axis value will be average of y's in the bin

  int MAXBINS = 100000;
  float bin[MAXBINS]; // bin[i] stores position where bin begins
  float width[MAXBINS]; // width of each bin (for convenience)
  int binCnt[MAXBINS]; // count of num points in bin
  float binTotal[MAXBINS]; // sum of y-values in bin

  for (int i=0; i<MAXBINS; i++) {
    binCnt[i] = 0;
    binTotal[i] = 0.0;
  }

  int numBins = 1;
  bin[0] = 0.0;
  width[0] = initialWidth;

  char buf[1024];

  float x;
  float y;
  for (int i=0; i<data.Len(); i++){
    //sscanf(buf, "%f %d %d", &price, &yes, &no);
    x = data[i].Val1;
    y = data[i].Val2;
    int pos=0;
    while (pos<numBins && (float)(bin[pos]+width[pos])<=x)
      pos++;
    if (pos == numBins) { // need to create more bins
      while (true) {
        bin[pos] = bin[pos-1]+width[pos-1];
        width[pos] = binWidthFactor*width[pos-1];
        numBins++;
        if ((float)(bin[pos]+width[pos])>x)
          break;
        pos++;
      }
    }

    binCnt[pos]++;
    binTotal[pos] += y;
  }

  //TFltPrV tempV;
  //tempV.Gen(numBins, 0);
  for (int i = 0; i < numBins; i++) {
    //printf("%d %f\n", bin[i], (float)binYes[i]/(float)(binNo[i]+binYes[i]));

    if (binCnt[i]==0) {
      if (i==0) // NEW JAN - don't show 0's due to data sparsity
        output.Add(TFltPr((float)bin[i], 0.0));
    }
    else
      output.Add(TFltPr((float)bin[i], (float)binTotal[i]/(float)(binCnt[i])));
  }

  printf("numBins %d\n", numBins);
  for (int i=0; i<20 && i<numBins; i++)
    printf("%d %f %d %f %f\n", i, bin[i], binCnt[i], (float) binTotal[i], (float)binTotal[i]/(float)(binCnt[i]));

  TGnuPlot ScGnuPlot(graphName, title);
  ScGnuPlot.AddPlot(output, gpwLinesPoints, "", "pt 6");
  ScGnuPlot.SetXYLabel(xAxis, yAxis);
  if (logScale)
    ScGnuPlot.SetScale(gpsLog10XY);
  else
    ScGnuPlot.SetScale(gpsAuto);
  ScGnuPlot.SavePng();

}
*/

bool matchDate(TInt& msgDate, TIntH& TXDates, int windowSize, bool tradeBeforeMsg){
  int gap = windowSize*24*60*60;
  for(int i=0;i<TXDates.Len();i++){
    TInt txDate = TXDates.GetKey(i);
    if (msgDate == txDate)
      return true;
    else if (!tradeBeforeMsg && msgDate < txDate && msgDate >= txDate-gap)
      return true;
    else if (tradeBeforeMsg && msgDate > txDate && msgDate <= txDate+gap)
      return true;
  }
  return false;
}

void timeWindow(PTaobaoNet t){
  PTaobaoShopNet shopNet = t->shopNet;
  PTaobaoShopNet msgNet = t->msgNet;

  int numOfTotalDates = 58;
  for (int windowSize=0;windowSize<numOfTotalDates;windowSize++){
    int tradeBefore = 0;
    int noTradeBefore = 0;
    int tradeAfter = 0;
    int noTradeAfter = 0;

    for (TTaobaoShopNet::TEdgeI EI = msgNet->BegEI(); EI < msgNet->EndEI(); EI++) {
      TIntH dates;
      TTaobaoShopEdge& edge = EI();
      TInt timeCount = edge.CntTX();
      for(int i=0;i<timeCount;i++){
        TSecTm dateTSecTm = edge.GetTime(i);//.GetAbsSecs();
        TInt date = dateTSecTm.GetAbsSecs();
        //printf("%d\n", (int)date);

        if (!dates.IsKey(date)) 
          dates.AddDat(date);
      }
      TInt SrcNId = EI.GetSrcNId();
      TInt DstNId = EI.GetDstNId();
      if (!shopNet->IsEdge(SrcNId, DstNId)){
        noTradeBefore += dates.Len();
        noTradeAfter += dates.Len();
      }
      else{
        TIntH TXDates;
        TTaobaoShopEdge& shopEdge = shopNet->GetEDat(SrcNId, DstNId);
        int TXCnt = shopEdge.CntTX();
        for(int i=0;i<TXCnt;i++){
          TInt date = shopEdge.GetTime(i).GetAbsSecs();
          if (!TXDates.IsKey(date)) 
            TXDates.AddDat(date);
        }
        for(int i=0;i<dates.Len();i++){
          TInt msgDate = dates.GetKey(i);
          if(matchDate(msgDate, TXDates, windowSize, true))
            tradeBefore++;
          else
            noTradeBefore++;
          if(matchDate(msgDate, TXDates, windowSize, false))
            tradeAfter++;
          else
            noTradeAfter++;
        }
      }
    }
    float likelihoodBefore = tradeBefore/ (tradeBefore+0.0+noTradeBefore);
    float likelihoodAfter = tradeAfter/ (tradeAfter+0.0+noTradeAfter);
    printf("%.12f\tfor all node pairs and dates that they have exchanged msgs, likelihood of trades happening at most %d days before the msg\n", likelihoodBefore, windowSize);
    printf("%.12f\tfor all node pairs and dates that they have exchanged msgs, likelihood of trades happening at most %d days after the msg\n", likelihoodAfter, windowSize);
  }
}


int binarySearch(TIntV& sortedArray, TInt key) {
   // function:
   //   Searches sortedArray[first]..sortedArray[last] for key.  
   // returns: index of the matching element if it finds key, 
   //         otherwise  -(index where it could be inserted)-1.
   // parameters:
   //   sortedArray in  array of sorted (ascending) values.
   //   first, last in  lower and upper subscript bounds
   //   key         in  value to search for.
   // returns:
   //   index of key, or -insertion_position -1 if key is not 
   //                 in the array. This value can easily be
   //                 transformed into the position to insert it.
   
   int first = 0;
   int last = sortedArray.Len()-1;
   while (first <= last) {
       int mid = (first + last) / 2;  // compute mid point.
       if (key > sortedArray[mid]) 
           first = mid + 1;  // repeat search in top half.
       else if (key < sortedArray[mid]) 
           last = mid - 1; // repeat search in bottom half.
       else
           return mid;     // found it. return position /////
   }
   return -(first + 1);    // failed to find key
}

void earliestMsgDate(PTaobaoNet t) {
  // for each buy edge
  // for each talk edge coming out of the buyer
  // check if there is a third shop edge that closes the triangle
  int numOfTotalDates = 58;

  PTaobaoShopNet shopNet = t->shopNet;
  PTaobaoShopNet msgNet = t->msgNet;

  int shopNetNodeCnt = shopNet->GetNodes();
  int shopNetEdgeCnt = shopNet->GetEdges();

  int uniqueDateTradeCount = 0;
  TIntH BeforeCnt;
  TIntH AfterCnt;

  for (TTaobaoShopNet::TEdgeI EI = shopNet->BegEI(); EI < shopNet->EndEI(); EI++) {
    TTaobaoShopEdge& shopEdge = EI();
    int TXCnt = shopEdge.CntTX();

    TInt SrcNId = EI.GetSrcNId();
    TInt DstNId = EI.GetDstNId();
    //printf("for edge %d %d\n", (int)SrcNId, (int)DstNId);

    if (!msgNet->IsEdge(SrcNId, DstNId)){
      //printf("is not an edge in msgNet\n");
      BeforeCnt.AddDat(-1)+=TXCnt; 
      AfterCnt.AddDat(-1)+=TXCnt; 
    }else{
      //printf("is an edge in msgNet\n");
      TTaobaoShopEdge& msgEdge = msgNet->GetEDat(SrcNId, DstNId);
      TInt timeCount = msgEdge.CntTX();
      TIntH msgDatesH; 
      TIntV msgDates;

      //printf("find unique dates\n");
      for(int i=0;i<timeCount;i++){
        TSecTm dateTSecTm = msgEdge.GetTime(i);//.GetAbsSecs();
        TInt date = dateTSecTm.GetAbsSecs();
        if(!msgDatesH.IsKey(date))
          msgDatesH.AddDat(date);
      }
      for(int i=0;i<msgDatesH.Len();i++)
        msgDates.Add(msgDatesH.GetKey(i));

      //printf("sorting\n");
      msgDates.Sort();  

      //printf("look for dates\n");
      for(int i=0;i<TXCnt;i++){
        TInt date = shopEdge.GetTime(i).GetAbsSecs();
        int first_index = binarySearch(msgDates, date);
        //printf("first_index:%d, msgDates.Len()=%d\n", first_index, msgDates.Len());
        int index = first_index;
        if (index < 0)
          index = -1*first_index-1;

        if (index == 0){
          BeforeCnt.AddDat(-1)++;
        }else{
          TInt mDate = msgDates[index-1];
          int dateDiff = (date-mDate)/60/60/24;
          //printf("%d %d dateDiff:%d\n", (int)date, (int)mDate, dateDiff);
          BeforeCnt.AddDat(dateDiff)++;
        }  
        
        if(index >= msgDates.Len()-1){
          AfterCnt.AddDat(-1)+=1;
        }else{
          TInt mDate = msgDates[index+1];
          int dateDiff = (mDate-date)/60/60/24;
          //printf("%d %d dateDiff:%d\n", (int)mDate, (int)date, dateDiff);
          AfterCnt.AddDat(dateDiff)++;
        }
      }
    }
  }

  printf("outputing");

  TIntPrV BeforeCntV;
  BeforeCnt.SortByKey(true);
  printf("Before:\n");
  for (int i = 0; i < BeforeCnt.Len(); i++) {
    if (BeforeCnt.GetKey(i) < 0 || BeforeCnt.GetKey(i) > 58)
      continue;
    printf("%d\t%d\n", (int)BeforeCnt.GetKey(i), (int)BeforeCnt[i]);
    BeforeCntV.Add(TIntPr( BeforeCnt.GetKey(i), BeforeCnt[i]));
  }

  TGnuPlot GnuPlot("Before_Time_Plot", TStr::Fmt("%sEarliest time of a msg before a trade",""));
  GnuPlot.AddPlot(BeforeCntV, gpwLinesPoints, "", "pt 6");
  GnuPlot.SetXYLabel("Time (no. of days)", "No. of trades");
  GnuPlot.SetScale(gpsLog10XY);
  GnuPlot.SavePng();  

  printf("\n\n\nAfter:\n");
  TIntPrV AfterCntV;
  AfterCnt.SortByKey(true);
  for (int i = 0; i < AfterCnt.Len(); i++) {
    if (AfterCnt.GetKey(i) < 0 || AfterCnt.GetKey(i) > 58)
      continue;
    printf("%d\t%d\n", (int)AfterCnt.GetKey(i), (int)AfterCnt[i]);
    AfterCntV.Add(TIntPr( AfterCnt.GetKey(i), AfterCnt[i]));
  }

  TGnuPlot AfterGnuPlot("After_Time_Plot.log", TStr::Fmt("%sEarliest time of a msg before a trade",""));
  AfterGnuPlot.AddPlot(AfterCntV, gpwLinesPoints, "", "pt 6");
  AfterGnuPlot.SetXYLabel("Time (no. of days)", "No. of trades");
  AfterGnuPlot.SetScale(gpsLog10XY);  
  AfterGnuPlot.SavePng();  

}

void measureTradeLikelihood(PTaobaoNet t) {
  // for each buy edge
  // for each talk edge coming out of the buyer
  // check if there is a third shop edge that closes the triangle
  int numOfTotalDates = 58;

  PTaobaoShopNet shopNet = t->shopNet;
  PTaobaoShopNet msgNet = t->msgNet;

  int shopNetNodeCnt = shopNet->GetNodes();
  int shopNetEdgeCnt = shopNet->GetEdges();

  int uniqueDateTradeCount = 0;
  for (TTaobaoShopNet::TEdgeI EI = shopNet->BegEI(); EI < shopNet->EndEI(); EI++) {
    TTaobaoShopEdge& shopEdge = EI();
    TIntH TXDates;
    int TXCnt = shopEdge.CntTX();
    for(int i=0;i<TXCnt;i++){
      TInt date = shopEdge.GetTime(i).GetAbsSecs();
      if (!TXDates.IsKey(date)) 
        TXDates.AddDat(date);
    }
    uniqueDateTradeCount += TXDates.Len();
  }

  printf("%d\n", uniqueDateTradeCount);

  double baselineLikelihood = uniqueDateTradeCount/((shopNetNodeCnt*(shopNetNodeCnt-1.0))*numOfTotalDates);

  printf("BaseLine trade likelihood:%.12f\n", baselineLikelihood);

  int repetitiveCnt = 0;
  for (TTaobaoShopNet::TEdgeI EI = shopNet->BegEI(); EI < shopNet->EndEI(); EI++) {
    TTaobaoShopEdge& edge = EI();
    if (edge.CntTX() > 1)
      repetitiveCnt++;
  }
  float repetitiveLikelihood = (repetitiveCnt+0.0) / shopNetEdgeCnt;
  printf("For node pairs that have traded, likelihood of trading again:%f\n", repetitiveLikelihood);

  timeWindow(t);
}

void plotMutualMsg() {
  FILE *in = fopen("mutualMsg.out", "r");

  int grandTotal=0;
  int K, observed, total;
  double prob;
  
  TFltTrV probV;

  while(fscanf(in, "%d %d %d %lf", &K, &observed, &total, &prob) == 4) {
    if (K<9) // OCT 10: limit this 
      probV.Add(TFltTr((float)K,(float)observed,(float)total));
    //printf("%d %d %d %lf\n", K, observed, total, prob);
  }
  fclose(in);

  TFltPrV outputV;
  // changed to binned mutualmsg2 for testing
  //binProb(probV, "binned_mutualMsg", 1.0, 2.0, true,"Binned Mutual Purchases vs Message Likelihood ", "Mutual Purchases", "Message Likelihood", outputV);
  binProb(probV, "binned_mutualMsg2", 1.0, 1.0, false,"", "Mutual Purchases", "Message Likelihood", outputV);
  
}

void plotMutual() {
  FILE *in = fopen("mutual.out", "r");

  int grandTotal=0;
  int K, observed, total;
  double prob;

  TIntPrV obsV;
  TIntPrV totV;

  while(fscanf(in, "%d %d %d %lf", &K, &observed, &total, &prob) == 4) {
    obsV.Add(TIntPr(K, observed));
    totV.Add(TIntPr(K, total));
    grandTotal += total;
    //printf("%d %d %d %lf\n", K, observed, total, prob);
  }
  fclose(in);

  FILE *in2 = fopen("mutualREQ.out", "r");
  
  TIntPrV obsReqV;
  TIntPrV totReqV;
  int grandTotalReq=0;

  while(fscanf(in2, "%d %d %d %lf", &K, &observed, &total, &prob) == 4) {
    obsReqV.Add(TIntPr(K, observed));
    totReqV.Add(TIntPr(K, total));
    grandTotalReq += total;
  }
  fclose(in2);

  /*
  int cumulative = 0;
  int sz;
  for (int i=0; i<obsV.Len(); i++) {
    cumulative += totV[i].Val2;
    if (cumulative > 0.99*grandTotal) {
      sz = i;
      break;
    }
  }
  */
 
  //float xLimit = 65.0; // default 60 .. or 50
  float xLimit = 200.0;

  int sz = 0; // ignore above, nearly all cases are mutual = 0 or 1
  while((float)obsV[sz].Val1 < xLimit)
    sz++;
  sz--;

  int szReq = 0;
  while((float)obsReqV[szReq].Val1 < xLimit)
    szReq++;
  szReq--;

  TFltPrV finalV;
  for (int i=0; i<sz; i++) {
    float prob = (float)obsV[i].Val2/(float)totV[i].Val2;
    finalV.Add(TFltPr((float)obsV[i].Val1, prob));
  }

  TFltPrV finalReqV;
  for (int i=0; i<szReq; i++) {
    float prob = (float)obsReqV[i].Val2/(float)totReqV[i].Val2;
    finalReqV.Add(TFltPr((float)obsReqV[i].Val1, prob));
  }

  // default 5.0, 1.0
  TFltPrV outputV;
  binning(finalV, "binned_mutualTrade2", 1.0, 2.0, false, "Binned Num Mutual Contacts vs Trade Likelihood", "Num Mutual Contacts", "Trade Likelihood", outputV);

  TFltPrV outputReqV;
  binning(finalReqV, "binned_mutualTradeREQ2", 1.0, 2.0, false, "Binned Num Mutual Contacts vs Trade Likelihood", "Num Mutual Contacts", "Trade Likelihood", outputReqV);

  // temp: changed both binnings to "true" for log scale

  // OCT 2010, changing graph format. changed mutOverlay2 to mutOverlay3
  TGnuPlot mutOverPlot("mutOverlay3", "", false);
  mutOverPlot.AddCmd("set key top left");
  mutOverPlot.AddPlot(outputReqV, gpwLinesPoints, "Msg Req", "lt 1 pt 7 ps 3");
  mutOverPlot.AddPlot(outputV, gpwLinesPoints, "Standard", "lt 2 pt 5 ps 3");
  //mutOverPlot.AddPlot(outputV, gpwPoints, "Standard", "lt 1 pt 6");
  //mutOverPlot.AddPlot(outputReqV, gpwPoints, "Msg Req", "lt 2 pt 6");
  mutOverPlot.SetXYLabel("Mutual Contacts", "Trade Likelihood");
  mutOverPlot.SetScale(gpsLog10X); // originally gpsAuto
  //mutOverPlot.SavePng();
  mutOverPlot.SaveEps("mutOverlay3.eps", 30, false);
}

void MutualBuyers(PTaobaoNet t) {
  /*
    measure likelihood of a msg given number of mutual purchases
   */

  TIntH mutualCnt;
  TIntH totalCnt;

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI< s->EndNI(); NI++) {
    int srcID = NI.GetId();
    TIntH mutuals;

    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopNet::TNodeI NJ = s->GetNI(sellerID);
      for (int j=0; j<NJ.GetInDeg(); j++) {
        int sinkID = NJ.GetInNId(j);
        if (sinkID!= srcID) {
          if (mutuals.IsKey(sinkID)) {
            int existCnt = mutuals.GetDat(sinkID);
            mutuals.AddDat(sinkID, existCnt+1);
          }
          else
            mutuals.AddDat(sinkID, 1);
        }
      }
    }

    // mutuals contains all nodes/cnts that are mutual with src
    for (int i=0; i<mutuals.Len(); i++) {
      int mutualK = (int)mutuals[i];
      int node = (int)mutuals.GetKey(i);
      if (srcID < node) { // no double counting of mutual pairs
        
        if (totalCnt.IsKey(mutualK)) {
          int existCnt = totalCnt.GetDat(mutualK);
          totalCnt.AddDat(mutualK, existCnt+1);
        }
        else {
          totalCnt.AddDat(mutualK, 1);
          mutualCnt.AddDat(mutualK, 0);
        }
        
        if (m->IsEdge(srcID, node) || m->IsEdge(node, srcID)) {
          if (mutualCnt.IsKey(mutualK)) {
            int existCnt = mutualCnt.GetDat(mutualK);
            mutualCnt.AddDat(mutualK, existCnt+1);
          }
          else { // shouldn't happen
            printf("error\n");
            mutualCnt.AddDat(mutualK, 1);
          }
        }
        
      }
    }
  }

  mutualCnt.SortByKey(true);
  totalCnt.SortByKey(true);

  if (mutualCnt.Len() != totalCnt.Len())
    printf("error\n");

  TFltPrV mutualV;
  FILE *out = fopen("mutualMsg.out", "w");
  for (int i=0; i<mutualCnt.Len(); i++) {
    int mutualK = (int) mutualCnt.GetKey(i);
    if ((int)mutualCnt.GetKey(i) != (int) totalCnt.GetKey(i))
      printf("error key\n");

    float prob = (float)mutualCnt[i]/(float)totalCnt[i];
    fprintf(out, "%d %d %d %f\n", mutualK, (int)mutualCnt[i], (int)totalCnt[i],
    prob);
    mutualV.Add(TFltPr((float)mutualK, prob));
  }
  fclose(out);

  TGnuPlot mGnuPlot("mutualMsgNew", "Num Mutual Purchases vs Msg Likelihood");
  mGnuPlot.AddPlot(mutualV, gpwLinesPoints, "", "pt 6 smooth bezier");
  mGnuPlot.SetXYLabel("Mutual Purchases", "Msg Likelihood");
  mGnuPlot.SetScale(gpsAuto);
  mGnuPlot.SavePng();

}

// note currently has extra edge check condition
void MutualContacts(PTaobaoNet t) {
  /*
    measure likelihood of a trade given number of mutual contacts
    for every node X, BFS 2 steps away to get all nodes Y with mutual contacts
    then just check for presence of a trade edge, and mark yes or no

    will need to compute 0 mutual contact case separately 
    can do this by (total trades-total mutual trades)/(total mutual edges)
   */

  // mutualCnt for trade success
  TIntH mutualCnt; // for mutual key k, how many observed successes
  TIntH mutualMsgCnt;
  TIntH totalCnt; // for mutual key k, how many possible

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;
  for (TTaobaoShopNet::TNodeI NI=c->BegNI(); NI<c->EndNI(); NI++) {
    int srcID = NI.GetId();
    TIntH mutuals;
    
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int nextID = NI.GetOutNId(i);
      TTaobaoShopNet::TNodeI NJ = c->GetNI(nextID);
      for (int j=0; j<NJ.GetOutDeg(); j++) {
        int sinkID = NJ.GetOutNId(j);
        if (sinkID != srcID) { //&& m->IsEdge(srcID, sinkID)) { 
          // latter condition src->sink is a test, REMOVE AS SOON AS DONE
          
          if (mutuals.IsKey(sinkID)) { 
            int existCnt = mutuals.GetDat(sinkID);
            mutuals.AddDat(sinkID, existCnt+1);
          }
          else
            mutuals.AddDat(sinkID, 1);
        }
      }
    }

    // mutuals contains all nodes/cnts that are mutual with src
    for (int i=0; i<mutuals.Len(); i++) {
      int mutualK = (int)mutuals[i];
      int node = (int)mutuals.GetKey(i);
      if (srcID < node) { // no double counting of mutual pairs

        if (totalCnt.IsKey(mutualK)) {
          int existCnt = totalCnt.GetDat(mutualK);
          totalCnt.AddDat(mutualK, existCnt+1);
        }
        else {
          totalCnt.AddDat(mutualK, 1);
          mutualCnt.AddDat(mutualK, 0);
          mutualMsgCnt.AddDat(mutualK, 0);
        }

        if (s->IsEdge(srcID, node) || s->IsEdge(node, srcID)) {
          if (mutualCnt.IsKey(mutualK)) {
            int existCnt = mutualCnt.GetDat(mutualK);
            mutualCnt.AddDat(mutualK, existCnt+1);
          }
          else { // shouldn't happen
            printf("error\n");
            mutualCnt.AddDat(mutualK, 1);
          }
        }
        
        if (m->IsEdge(srcID, node) || m->IsEdge(node, srcID)) {
          if (mutualMsgCnt.IsKey(mutualK)) {
            int existCnt = mutualMsgCnt.GetDat(mutualK);
            mutualMsgCnt.AddDat(mutualK, existCnt+1);
          }
          else { // shouldn't happen
            printf("error\n");
            mutualMsgCnt.AddDat(mutualK, 1);
          }
        }

      }
    }

    /*
    // compute 0 mutual case with the extra req of contact between src-sink
    // comment this out and assume 0 for the normal case
    if (s->IsNode(srcID)) {
      TTaobaoShopNet::TNodeI NK = s->GetNI(srcID);
      for (int i=0; i<NK.GetOutDeg(); j++) {      
      }
    }
    */

  }

  mutualCnt.SortByKey(true);
  mutualMsgCnt.SortByKey(true);
  totalCnt.SortByKey(true);

  if (mutualCnt.Len() != totalCnt.Len())
    printf("error\n");

  //TFltPrV mutualV;
  FILE *out = fopen("mutual.outOCT", "w");
  for (int i=0; i<mutualCnt.Len(); i++) {
    int mutualK = (int) mutualCnt.GetKey(i);
    if ((int)mutualCnt.GetKey(i) != (int) totalCnt.GetKey(i))
      printf("error key\n");

    float prob = (float)mutualCnt[i]/(float)totalCnt[i];
    //fprintf(out, "%d %d %d %f\n", mutualK, (int)mutualCnt[i], (int)totalCnt[i], prob);
    fprintf(out, "%d %d %d %d\n", mutualK, (int)mutualCnt[i], (int)mutualMsgCnt[i], (int)totalCnt[i]);
    //mutualV.Add(TFltPr((float)mutualK, prob));
  }
  fclose(out);

  /*
  TGnuPlot mGnuPlot("mutualTradeREQ", "Num Mutual Contacts vs Trade Likelihood");
  mGnuPlot.AddPlot(mutualV, gpwPoints, "", "pt 6");
  mGnuPlot.SetXYLabel("Mutual Contacts", "Trade Likelihood");
  mGnuPlot.SetScale(gpsAuto);
  mGnuPlot.SavePng();
  */
}

void EventSequences(PTaobaoNet t) {
  /*
    for each seller, for each pair of buyers, check if a message was exchanged
  before, between, and after the two purchases. Baseline: count the numbers of days in
  which these two exchanged messages, let that be M.

  The two initial purchases divide the timeline into the 3 regions: A,B,C.
  The probability of no msg during A is ((2m-A)/2m)^M. 
  eg we get probabilities for at least 1 msg in each of the regions.
  Sum up these probabilities for all pairs of buyers
  Compare these numbers to the actual counts observed

  To get probabilities, just divide these counts by the total number of buyers
  pairs we looked at
   */

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  // find date of first, last purchase in the data
  int totalDays = 58;
  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  uint beginSec = intervalBegin.GetAbsSecs();
  uint endSec = intervalEnd.GetAbsSecs();
  uint daySec = 24*60*60;
  /*
 for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TTaobaoShopEdge& edge = EI();
    TSecTm f = edge.GetTime(0);
    TSecTm l = edge.GetTime(edge.CntTX()-1);
    if (f < intervalBegin)
      intervalBegin = f;
    if (l > intervalEnd)
      intervalEnd = l;
  }
  printf("first %s last %s\n", intervalBegin.GetStr().CStr(), intervalEnd.GetStr().CStr());
  */

  int total = 0;
  //int beforeCnt = 0; // sum of booleans incidating msg before first purchase
  //int betweenCnt = 0;
  //int afterCnt = 0;
  //double beforeRand = 0.0;
  //double betweenRand = 0.0;
  //double afterRand = 0.0;
  double netBeforeDays=0.0;
  double netBetweenDays=0.0;
  double netAfterDays=0.0;
  double netBeforeMsg=0.0;
  double netBetweenMsg=0.0;
  double netAfterMsg=0.0;
  double beforeProp = 0.0;
  double betweenProp = 0.0;
  double afterProp = 0.0;
  double totalMsg = 0.0;
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int sellerID = NI.GetId();
    if (NI.GetInDeg() >= 2) { 
      for (int i=0; i<NI.GetInDeg(); i++) {
        TTaobaoShopEdge& shopEdge = NI.GetInEDat(i);
        TSecTm t1 = shopEdge.GetTime(0);
   
        for (int j=i+1; j<NI.GetInDeg(); j++) {
          //total++; // only consider at least 1 msg case
          TTaobaoShopEdge& shopEdge2 = NI.GetInEDat(j);          
          TSecTm t2 = shopEdge2.GetTime(0);
         
          int n1 = NI.GetInNId(i);
          int n2 = NI.GetInNId(j);

          if (t1 > t2) {
            TSecTm c = t1;
            t1 = t2;
            t2 = c;
            n1 = NI.GetInNId(j);
            n2 = NI.GetInNId(i);
          }

          // assume that each region is [begin, end), except for the last one
     
          int beforeDays = (t1.GetAbsSecs()-beginSec)/daySec; // was +1
          if (beforeDays > totalDays) {
            printf("ERROR t1 %s dif %d\n", t1.GetStr().CStr(), beforeDays);
            exit(0);
          }
          int betweenDays = (t2.GetAbsSecs()-t1.GetAbsSecs())/daySec; // was +1
          if (betweenDays > totalDays) {
            printf("ERROR t1 %s t2 %s dif %d\n", t1.GetStr().CStr(), t2.GetStr().CStr(), betweenDays);
            exit(0);
          }
          int afterDays = (endSec-t2.GetAbsSecs())/daySec+1; // was +1
          if (afterDays > totalDays) {
            printf("ERROR t2 %s dif %d\n", t2.GetStr().CStr(), afterDays); 
            exit(0);
          }

          if (beforeDays+betweenDays+afterDays != totalDays) {
            printf("days %d %d %d\n", beforeDays, betweenDays, afterDays);
            printf("%s %s\n", t1.GetStr().CStr(), t2.GetStr().CStr());
            exit(0);
          }

          // commented out blocks are for at least one msg in an interval
          // trying out expectations
          int foundBefore = 0;
          int foundBetween = 0;
          int foundAfter = 0;
          if (m->IsNode(n1) && m->IsNode(n2)) {
            if (m->IsEdge(n1, n2)) { //&& c->IsEdge(n1,n2)) { // chat req new testing
              //total++; //not exactly, check if before day1
              TTaobaoShopEdge& msgEdge = m->GetEDat(n1, n2);
              for (int k=0; k<msgEdge.CntTX(); k++) {
                TSecTm mT = msgEdge.GetTime(k);
                if (mT >=intervalBegin && mT < t1) { // feb new: added >= req
                  // for con
                  //foundBefore = 1;
                  //foundBefore++; // change to add strength
                  foundBefore += (int)msgEdge.GetStrength(k);
                  totalMsg += (double)msgEdge.GetStrength(k);
                }
                else if (mT>=t1 && mT<t2) { // changed >= to >
                  //foundBetween = 1;
                  //foundBetween++;
                  foundBetween += (int)msgEdge.GetStrength(k);
                  totalMsg += (double)msgEdge.GetStrength(k);
                }
                else { //if (mT>=t2 && mT <=intervalEnd) { 
                  //foundAfter = 1;
                  //foundAfter++;
                  foundAfter += (int)msgEdge.GetStrength(k);
                  totalMsg += (double)msgEdge.GetStrength(k);
                }
              }   

              // if to handle case where only have invalid contacts, eg no edge
              if (foundBefore+foundBetween+foundAfter != 0) { 
                total++;

                //beforeRand = beforeRand + 1.0-pow((double)(totalDays-beforeDays)/(double)totalDays, (double)msgEdge.CntTX());
                //beforeRand = beforeRand + ((double)beforeDays/(double)totalDays)*(double)msgEdge.CntTX();
                //betweenRand = betweenRand + 1.0-pow((double)(totalDays-betweenDays)/(double)totalDays, (double)msgEdge.CntTX());
                //betweenRand = betweenRand + ((double)betweenDays/(double)totalDays)*(double)msgEdge.CntTX();
                //afterRand = afterRand + 1.0-pow((double)(totalDays-afterDays)/(double)totalDays, (double)msgEdge.CntTX());
                //afterRand = afterRand + ((double)afterDays/(double)totalDays)*(double)msgEdge.CntTX();
                //totalMsg += (double)msgEdge.CntTX();
                
                // if (foundBefore+foundBetween+foundAfter != msgEdge.CntTX())
                //  printf("error\n");
                
                netBeforeDays += (double) beforeDays;
                netBetweenDays += (double) betweenDays;
                netAfterDays += (double) afterDays;
                
                netBeforeMsg += (double) foundBefore;
                netBetweenMsg += (double) foundBetween;
                netAfterMsg += (double) foundAfter;
              }

            }
               
            //beforeProp += (double)foundBefore/(double)beforeDays;
            //betweenProp += (double)foundBetween/(double)betweenDays;
            //afterProp += (double)foundAfter/(double)afterDays;

            // msg count
            //beforeCnt += foundBefore;
            //betweenCnt += foundBetween;
            //afterCnt += foundAfter;

            // if at least message
            //if (foundBefore == 1)
            //  beforeCnt++;
            //if (foundBetween == 1)
            //  betweenCnt++;
            //if (foundAfter == 1)
            //  afterCnt++;
          }
        }
      }

    }
  }

  //beforeRand = beforeRand / (double)total;
  //betweenRand = betweenRand / (double)total;
  //afterRand = afterRand / (double)total;

  //beforeProp = beforeProp / (double)total;
  //betweenProp = betweenProp / (double)total;
  //afterProp = afterProp / (double)total;
  double avgMsg = (double) totalMsg/((double)total*(double)totalDays);

  //printf("OBSERVED before:%lf between:%lf after:%lf\n", (double)beforeCnt/(double)total, double)betweenCnt/(double)total, (double)afterCnt/(double)total);
  
  printf("OBSERVED before:%.10lf between:%.10lf after:%.10lf\n",
         (double)netBeforeMsg/(double)netBeforeDays,
         (double)netBetweenMsg/(double)netBetweenDays, (double)netAfterMsg/(double)netAfterDays);
  //printf("OBSERVED before:%.10lf between:%.10lf after:%.10lf\n", beforeProp, betweenProp, afterProp);
  printf("RANDOM: %.10lf\n", avgMsg);

  printf("totalDays:%lf %lf %lf %lf totalMsg:%lf %lf %lf %lf\n",
         (double)total*(double)totalDays, netBeforeDays, netBetweenDays,
         netAfterDays, totalMsg, netBeforeMsg, netBetweenMsg, netAfterMsg);

  //printf("RANDOM before:%lf between:%lf after:%lf\n", beforeRand, betweenRand, afterRand);
  //printf("before:%d between:%d after:%d total:%d\n", beforeCnt, betweenCnt, afterCnt, total);
}


/* 
My def of IM-induced purchase triangle:
Buyer 1 first purchase at time t1
Buyer 2 first purchase at time t2
Buyer 1 talks to Buyer 2 within a time window W before t2
t1<t2

*currently i don't cnt multiple successful chats within window
fastest way to find triangles is to look at pair of edges going into seller
for total opporunities, use this approx: 
for nodes which haven't bought, sum of num communications after t1 
for nodes which have bought, sum of num communiatins between t1 and t2
* this is slower to calculate than num triangles
*/

void plotTriangle() {
  TFltPrV msgProbV;
  TFltPrV msgProbV2;
  TFltPrV msgProbV3;
  float xLimit = 5000.0;

  int msgStrength;
  float tradeProb;

  FILE *in = fopen("triMsgVolProbMSG.out", "r");
  while (fscanf(in, "%d %f", &msgStrength, &tradeProb) == 2) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbV.Add(TFltPr((float)msgStrength, tradeProb));
  }
  fclose(in);
  
  FILE *in2 = fopen("triMsgVolProbMSGTRADE.out", "r");
  while (fscanf(in2, "%d %f", &msgStrength, &tradeProb) == 2) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbV2.Add(TFltPr((float)msgStrength, tradeProb));
  }
  fclose(in2);

  FILE *in3 = fopen("triMsgVolProbMSGCON.out", "r");
  while (fscanf(in3, "%d %f", &msgStrength, &tradeProb) == 2) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbV3.Add(TFltPr((float)msgStrength, tradeProb));
  }
  fclose(in3);

  /*
  TGnuPlot tPGnuPlot("triMsgVolTradeConProb", "Triangle Message Strength vs Trade Triangle Close Probability");
  tPGnuPlot.AddPlot(msgProbV, gpwPoints, "", "pt 6");
  tPGnuPlot.SetXYLabel("Message Strength", "Trade Probability");
  tPGnuPlot.SetScale(gpsAuto);
  tPGnuPlot.SavePng();
  */

  TFltPrV outputV;
  binning(msgProbV, "binned_triMsgVolMsgProb", 1.0, 2.0, false, "Binned Message Strength vs Message Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV);

  TFltPrV outputV2;
  binning(msgProbV2, "binned_triMsgVolMsgTradeProb", 1.0, 2.0, false, "Binned Message Strength vs Trade Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV2);

  TFltPrV outputV3;
  binning(msgProbV3, "binned_triMsgVolMsgConProb", 1.0, 2.0, false, "Binned Message Strength vs Contact Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV3);

  TGnuPlot tZGnuPlot("binned_triMsgVolMsgOverlay", "Binned Message Strength vs Triangle Probability Given Initial Message");
  tZGnuPlot.AddPlot(outputV, gpwLinesPoints, "Message Close", "pt 6");
  tZGnuPlot.AddPlot(outputV2, gpwLinesPoints, "Trade Close", "pt 6");
  tZGnuPlot.AddPlot(outputV3, gpwLinesPoints, "Contact Close", "pt 6");
  tZGnuPlot.SetXYLabel("Message Strength", "Triangle Close Probability");
  tZGnuPlot.SetScale(gpsAuto);
  tZGnuPlot.SavePng();

  /*
  float randLevel = 0.0003215947; // baseline from triangle closing (update)
  TFltPrV randomV;
  for (int i=0; i<outputV2.Len(); i++) 
    randomV.Add(TFltPr((float)outputV2[i].Val1, randLevel));

  // new: add line indicating random
  TGnuPlot tZGnuPlot("binned_triMsgVolTradeMsgProb", "Binned Message Strength vs Message Close Prob Given Initial Trade");
  tZGnuPlot.AddPlot(outputV2, gpwLinesPoints, "Actual", "pt 6");
  tZGnuPlot.AddPlot(randomV, gpwLinesPoints, "Random", "pt 6");
  tZGnuPlot.SetXYLabel("Message Strength", "Triangle Probability");
  tZGnuPlot.SetScale(gpsAuto);
  tZGnuPlot.SavePng();
  */
}


/*
new testing
trade1, msg (all before trade1), trade2
 */
void triangleOrderTest(PTaobaoNet t) {
  //PTaobaoShopNet s = t->contactNet; // try running this replacing s with msg
  //or contact
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet x = t->shopNet; // the third leg of the triangle is now a
                                    // variable

  TIntH totMsg; // count of num msg strengths
  TIntH sucMsg; // count of num msg strengths leading to trades

  double success = 0.0;
  double total = 0.0;
  double baseline = 0.0;
  double numNodes = (double) s->GetNodes();

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      TSecTm t = shopEdge.GetTime(0);

      if (m->IsNode(buyerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
        int candMsg = 0;
        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);

          //int k=0; 
          //while (k<msgEdge.CntTX() && msgEdge.GetTime(k)<t)
          //  k++;
          
          if (msgEdge.GetTime(msgEdge.CntTX()-1) < t) { // get last message time
            TSecTm chatTime = msgEdge.GetTime(msgEdge.CntTX()-1);
            int msgCnt = msgEdge.CntTXStrength(); 

            if (totMsg.IsKey(msgCnt)) {
              int existCnt = totMsg.GetDat(msgCnt);
              totMsg.AddDat(msgCnt, existCnt+1);
            }
            else {
              totMsg.AddDat(msgCnt, 1);
              sucMsg.AddDat(msgCnt, 0);
            }

            candMsg++;
            total += 1.0;
            if (x->IsEdge(msgID, sellerID)) {
              TTaobaoShopEdge& closeEdge = x->GetEDat(msgID, sellerID);
              TSecTm t2 = closeEdge.GetTime(closeEdge.CntTX()-1);
              if (t2 >= t) {
                success += 1.0;
                int existCnt = sucMsg.GetDat(msgCnt);
                sucMsg.AddDat(msgCnt, existCnt+1);
              }
            }
          }
        }

        // insert random baseline simulation here
        // choose candMsg random nodes in the msg graph, trade?
        // to save time, approx upper bound expectation as candMsg *
        // (srcIndeg/totalNodes)
        // feb: changed to 3rd network in-degree
        if (x->IsNode(sellerID)) {
          TTaobaoShopNet::TNodeI NK = x->GetNI(sellerID);
          baseline += (double) ((double)NK.GetInDeg()/numNodes * (double) candMsg);
        }
        
      }
      
    }
  }
  
  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);
  
  TFltPrV msgProb;

  FILE *out = fopen("triMsgVolProb.out", "w");
  
  int tot = 0;
  for (int i=0; i<totMsg.Len(); i++)
    tot += totMsg[i];
  
  int trimLen = 0;
  int cumul =0;
  while (cumul < tot*0.99) {
    cumul += totMsg[trimLen];
    trimLen++;
  }
  printf("99 TRIMLEN %d\n", trimLen);
  
  //trimLen = 0;
  //while (totMsg.GetKey(trimLen) < 5000)
  //  trimLen++;
  
  for (int i=0; i<totMsg.Len(); i++) { // was < trimLen
    if((int)totMsg.GetKey(i)!=sucMsg.GetKey(i))
      printf("key error\n");
    float tradeProb = (float)sucMsg[i] / (float)totMsg[i];
    msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
    fprintf(out, "%d %f\n", (int)totMsg.GetKey(i), tradeProb);
  }
  
  fclose(out);
  
  // use plotTriangle for repeated plotting
    /*
      TGnuPlot tPGnuPlot("triMsgVolMsgProb", "Triangle Message Strength vs Message
      Triangle Close Probability");
      tPGnuPlot.AddPlot(msgProb, gpwPoints, "", "pt 6");
      tPGnuPlot.SetXYLabel("Message Strength", "Message Probability");
  tPGnuPlot.SetScale(gpsAuto);
  tPGnuPlot.SavePng();
  
  
  TFltPrV outputV;
  binning(msgProb, "binned_triMsgVolTradeProb", 1.0, 2.0, false, "Binned
  Triangle Message Strength vs Message Triangle Close Probability", "Message
  Strength", "Trade Prob", outputV);
    */
  
    printf("observed %.10lf %lf %lf baseline %.10lf\n", success/total, success,
           total, baseline/total);
}


// NEW, brute force triangle closing
// can add binary search later
// feb: added msg str graph, similar to edgeAgain
void triangleClosing(PTaobaoNet t) {
  //PTaobaoShopNet s = t->contactNet; // try running this replacing s with msg or contact
  PTaobaoShopNet s = t->msgNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet x = t->contactNet; // the third leg of the triangle is now a variable

  TIntH totMsg; // count of num msg strengths
  TIntH sucMsg; // count of num msg strengths leading to trades

  double success = 0.0;
  double total = 0.0;
  double baseline = 0.0;
  double numNodes = (double) s->GetNodes();

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      TSecTm t = shopEdge.GetTime(0);

      if (m->IsNode(buyerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
        int candMsg = 0;
        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);

          int k=0; 
          while (k<msgEdge.CntTX() && msgEdge.GetTime(k)<t)
            k++;
          
          if (k<msgEdge.CntTX()) { // use first message time
            TSecTm chatTime = msgEdge.GetTime(k);
            int msgCnt = msgEdge.CntTXStrength(); 

            if (totMsg.IsKey(msgCnt)) {
              int existCnt = totMsg.GetDat(msgCnt);
              totMsg.AddDat(msgCnt, existCnt+1);
            }
            else {
              totMsg.AddDat(msgCnt, 1);
              sucMsg.AddDat(msgCnt, 0);
            }

            candMsg++;
            total += 1.0;
            if (x->IsEdge(msgID, sellerID)) {
              TTaobaoShopEdge& closeEdge = x->GetEDat(msgID, sellerID);
              TSecTm t2 = closeEdge.GetTime(closeEdge.CntTX()-1);
              if (t2 >= chatTime) {
                success += 1.0;
                int existCnt = sucMsg.GetDat(msgCnt);
                sucMsg.AddDat(msgCnt, existCnt+1);
              }
            }
          }
        }

        // insert random baseline simulation here
        // choose candMsg random nodes in the msg graph, trade?
        // to save time, approx upper bound expectation as candMsg * (srcIndeg/totalNodes)
        // feb: changed to 3rd network in-degree
        if (x->IsNode(sellerID)) {
          TTaobaoShopNet::TNodeI NK = x->GetNI(sellerID);
          baseline += (double) ((double)NK.GetInDeg()/numNodes * (double) candMsg);
        }
        
      }
      
    }
  }
  
  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);

  TFltPrV msgProb;

  FILE *out = fopen("triMsgVolProb.out", "w");

  int tot = 0;
  for (int i=0; i<totMsg.Len(); i++)
    tot += totMsg[i];

  int trimLen = 0;
  int cumul =0;
  while (cumul < tot*0.99) {
    cumul += totMsg[trimLen];
    trimLen++;
  }
  printf("99 TRIMLEN %d\n", trimLen);

  //trimLen = 0;
  //while (totMsg.GetKey(trimLen) < 5000)
  //  trimLen++;

  for (int i=0; i<totMsg.Len(); i++) { // was < trimLen
    if((int)totMsg.GetKey(i)!=sucMsg.GetKey(i))
      printf("key error\n");
    float tradeProb = (float)sucMsg[i] / (float)totMsg[i];
    msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
    fprintf(out, "%d %f\n", (int)totMsg.GetKey(i), tradeProb);
  }

  fclose(out);

  // use plotTriangle for repeated plotting
  /*
  TGnuPlot tPGnuPlot("triMsgVolMsgProb", "Triangle Message Strength vs Message Triangle Close Probability");
  tPGnuPlot.AddPlot(msgProb, gpwPoints, "", "pt 6");
  tPGnuPlot.SetXYLabel("Message Strength", "Message Probability");
  tPGnuPlot.SetScale(gpsAuto);
  tPGnuPlot.SavePng();

  
  TFltPrV outputV;
  binning(msgProb, "binned_triMsgVolTradeProb", 1.0, 2.0, false, "Binned Triangle Message Strength vs Message Triangle Close Probability", "Message Strength", "Trade Prob", outputV);
  */

  printf("observed %.10lf %lf %lf baseline %.10lf\n", success/total, success, total, baseline/total);
}

// this function is old, ADAPT it
// we want to find prob of chat-induced second purchase
// randomized baseline ie just 
/*
void FindTriangles(PTaobaoNet t) {
  // for each buy edge
  // for each talk edge coming out of the buyer
  // check if there is a third shop edge that closes the triangle

  PTaobaoShopNet shopNet = t->shopNet;
  PTaobaoShopNet msgNet = t->msgNet;

  int edgeCnt=0;
  int softTriCnt=0;
  int triCnt=0;
  for (TTaobaoShopNet::TNodeI NI = shopNet->BegNI(); NI < shopNet->EndNI(); NI++) {
    uint window = 2*24*60*60;

    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    int inDeg = NI.GetInDeg();
    int sellNode = NI.GetId();
    
    for(int i=0; i<inDeg; i++) {
      TTaobaoShopEdge& shopEdge1 = NI.GetInEDat(i);
      int buyNode1 = NI.GetInNId(i);
      for (int j=i+1; j<inDeg; j++) {
        TTaobaoShopEdge& shopEdge2 = NI.GetInEDat(j);
        int buyNode2 = NI.GetInNId(j);

        // choose purchase timestamps to maximize msg window
        TSecTm t1 = shopEdge1.GetTX(0).Timestamp;
        TSecTm t2 = shopEdge2.GetTX(shopEdge2.CntTX()-1).Timestamp;
        if (t1 < t2) {
          if (msgNet->IsEdge(buyNode1, buyNode2)) {
            TTaobaoShopEdge& chatEdge = msgNet->GetEDat(buyNode1, buyNode2);
            int infoPassed=0;
            printf("%d %d %d\n", sellNode, buyNode1, buyNode2); 
            softTriCnt++;
            for (int k=0; k<chatEdge.CntTX(); k++) {
              TSecTm c = chatEdge.GetTime(k);
              //printf("%s %s %s\n", t1.GetStr().CStr(), t2.GetStr().CStr(), c.GetStr().CStr());
              if (c>t1 && c<t2 && c.GetAbsSecs()+window>t2.GetAbsSecs()) {
                infoPassed = 1;
                break;
              }
            }
            if (infoPassed)
              triCnt++;
          }
        }
        else {
          t1 = shopEdge1.GetTX(ShopEdge1.CntTX()-1).Timestamp;
          t2 = ShopEdge2.GetTX(0).Timestamp;
          if (msgNet->IsEdge(buyNode2, buyNode1)) {
            TTaobaoShopEdge& chatEdge = msgNet->GetEDat(buyNode2, buyNode1);
            int infoPassed=0;
            softTriCnt++;
            printf("%d %d %d\n", sellNode, buyNode2, buyNode1);
            for (int k=0; k<chatEdge.CntTX(); k++) {
              TSecTm c = chatEdge.GetTime(k);
              //printf("%s %s %s\n", t2.GetStr().CStr(), t1.GetStr().CStr(), c.GetStr().CStr());
              if (c>t2&& c<t1 && c.GetAbsSecs()+window>t1.GetAbsSecs()) {
                infoPassed = 1;
                break;
              }
            }
            if (infoPassed)
              triCnt++;
          }
        }
        
      }
      //printf("%d %d\n", nodeA, nodeB);
      edgeCnt++;
    }
  }

  printf("buybuychat triangles: %d\n", softTriCnt); /// triangle w/o time req
  printf("timestamp triangles: %d\n", triCnt); // triangle with time requirement
}
*/

/* get wealth/#chats */
void SellerScatter(PTaobaoNet t, TFltPrV& SellerWealthConvoV) {
  PTaobaoShopNet shopNet = t->shopNet;
  PTaobaoShopNet msgNet = t->msgNet;
  
  for (TTaobaoShopNet::TNodeI NI = shopNet->BegNI(); NI < shopNet->EndNI(); NI++) {
    TTaobaoUsr t = (TTaobaoUsr) NI.GetDat();
    if (t.IsSeller()) {
      float numShops = 0.0;
      float wealth = t.TotalSales;
      int sellNode = NI.GetId(); 
     
      if (msgNet->IsNode(sellNode)) {
        // find same node in the chat network
        TTaobaoShopNet::TNodeI NJ = msgNet->GetNI(sellNode);
        int outDeg = NJ.GetOutDeg();
        for (int i=0; i<outDeg; i++) {
          TTaobaoShopEdge& chatEdge = NJ.GetOutEDat(i);
          numShops = numShops + (float) chatEdge.CntTX();
        }
      }
      
      SellerWealthConvoV.Add(TFltPr(wealth, numShops));
    }
  }
}

// plot overlaid graphs from the networkStructure method
void plotNetOverlay() {

  TFltPrV transMsgV;
  TFltPrV transConV;
  TFltPrV wealthMsgV;
  TFltPrV wealthConV;

  float a, b;

  FILE *transMsg = fopen("invOutTransMsg.out" , "r");
  while(fscanf(transMsg, "%f %f", &a, &b) == 2) 
    transMsgV.Add(TFltPr(a, b));
  fclose(transMsg);

  FILE *transCon = fopen("invOutTransCon.out" , "r");
  while(fscanf(transCon, "%f %f", &a, &b) == 2)
    transConV.Add(TFltPr(a, b));
  fclose(transCon);

  FILE *wealthMsg = fopen("invOutWealthMsg.out" , "r");
  while(fscanf(wealthMsg, "%f %f", &a, &b) == 2)
    wealthMsgV.Add(TFltPr(a, b));
  fclose(wealthMsg);

  FILE *wealthCon = fopen("invOutWealthCon.out" , "r");
  while(fscanf(wealthCon, "%f %f", &a, &b) == 2)
    wealthConV.Add(TFltPr(a, b));
  fclose(wealthCon);

  TGnuPlot transMsgConGnuPlot("transMsgCon","Clustering Coefficient vs Transaction Count");
  transMsgConGnuPlot.AddPlot(transMsgV, gpwLinesPoints, "Message Clustering Coefficient", "pt 6");
  transMsgConGnuPlot.AddPlot(transConV, gpwLinesPoints, "Contact Clustering Coefficient", "pt 6");
  transMsgConGnuPlot.SetXYLabel("Clustering Coefficient", "Transaction Count");
  transMsgConGnuPlot.SetScale(gpsAuto);
  transMsgConGnuPlot.SavePng();

  TGnuPlot wealthMsgConGnuPlot("wealthMsgCon","Clustering Coefficient vs Wealth");
  wealthMsgConGnuPlot.AddPlot(wealthMsgV, gpwLinesPoints, "Message Clustering Coefficient", "pt 6");
  wealthMsgConGnuPlot.AddPlot(wealthConV, gpwLinesPoints, "Contact Clustering Coefficient", "pt 6");
  wealthMsgConGnuPlot.SetXYLabel("Clustering Coefficient", "Wealth");
  wealthMsgConGnuPlot.SetScale(gpsAuto);
  wealthMsgConGnuPlot.SavePng();

}

// load file from transTemp.out, for fast graphing of clustering data
void plotStructure() {
 
  FILE *in = fopen("transTemp.out", "r");

  TFltPrV invTransCntClusterV;
  TFltPrV invWealthClusterV;

  int ID, deg;
  float trans, wealth, cluster;
  while (fscanf(in, "id:%d deg:%d trans:%f wealth:%f cluster:%f\n", &ID, &deg,
                &trans, &wealth, &cluster) == 5) {
    // can insert degree restrictions later
    //if (deg == 10) {
      invTransCntClusterV.Add(TFltPr(cluster, trans));
      invWealthClusterV.Add(TFltPr(cluster, wealth));
      // }
  }
  fclose(in);

  invTransCntClusterV.Sort(true);
  invWealthClusterV.Sort(true);

  float xLimit = 0.5;

  printf("hi %d\n", invTransCntClusterV.Len());

  TFltPrV invTrimTrans;
  //int invTrimSize = (int)(invTransCntClusterV.Len()*0.99);
  int invTrimSize = 0;
  while ( invTrimSize<invTransCntClusterV.Len() && (float)invTransCntClusterV[invTrimSize].Val1 < xLimit)
    invTrimSize++;
  invTrimSize--;

  printf("%d\n", invTrimSize);

  for (int i=0; i<invTrimSize; i++)
    //if ((float)invTransCntClusterV[i].Val2 < yTransLimit) // feb new: cut off
    invTrimTrans.Add(TFltPr((float)invTransCntClusterV[i].Val1, (float)invTransCntClusterV[i].Val2));

  TFltPrV invTrimWealth;
  //invTrimSize = (int)(invWealthClusterV.Len()*0.99);
  for (int i=0; i<invTrimSize; i++)
    //if ((float)invWealthClusterV[i].Val2 < yWealthLimit) // feb new
    invTrimWealth.Add(TFltPr((float)invWealthClusterV[i].Val1, (float)invWealthClusterV[i].Val2));

  TFltPrV invOut1;
  TFltPrV invOut2;

  printf("hi\n");

  // 0.001 for exp bins

  //0.04, 1 looks good for wealth
  binning(invTrimTrans, "binned_invTransCntCluster", 0.001, 2.0, true,"Binned Clustering Coefficient vs Transaction Count", "Clustering Coefficient", "Transaction Count", invOut1);
  binning(invTrimWealth, "binned_invWealthCluster", 0.001, 2.0, true, "Binned Clustering Coefficient vs Wealth", "Clustering Coefficient", "Wealth", invOut2);

}

// graph seller num trans / wealth vs ego network (clustering coefficient)
void networkStructure(PTaobaoNet t, TStr jobType) {
  PTaobaoShopNet s = t->shopNet;
  //PTaobaoShopNet m = t->msgNet; 
  PTaobaoShopNet m;
  
  if (jobType == "msg")
    m = t->msgNet;
  else if (jobType == "contact")
    m = t->contactNet;
  else
    return;
  
  TFltPrV transCntClusterV;
  TFltPrV wealthClusterV;
  TFltPrV invTransCntClusterV;
  TFltPrV invWealthClusterV;

  FILE *out = fopen("transTemp.out", "w");
  
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int sellerID = NI.GetId();
    TTaobaoUsr t = (TTaobaoUsr) s->GetNDat(sellerID);
    float wealth = (float) t.TotalSales; // feb: changed from int to float

    // can try increasing this number higher to test...
    if (NI.GetInDeg() >= 2) { // need at least 2 buyers to have a possible triangle
      // seller properties: total sales, total quantity, total wealth
      float transCnt = 0;
      TIntV buyers;
      for (int i=0; i<NI.GetInDeg(); i++) {
        TTaobaoShopEdge& shopEdge = NI.GetInEDat(i);
        transCnt = transCnt + (float) shopEdge.CntTX();
        buyers.Add(TInt(NI.GetInNId(i)));
      }
      
      // look at each pair of buyers, check if there's a msg edge
      int comLink = 0;
      for (int i=0; i<NI.GetInDeg(); i++) {     
        if (m->IsNode(buyers[i])) {
          for (int j=i+1; j<NI.GetInDeg(); j++) {
            // check if there's a msg link from i to j or j to i
   
            if (m->IsNode(buyers[j])) {
              if (m->IsEdge(buyers[i], buyers[j]) || m->IsEdge(buyers[j], buyers[i]))
                comLink++;
            }
          }
        }
      }
      
      int numPairs = (NI.GetInDeg()*(NI.GetInDeg()-1))/2;
      float localCluster = (float)comLink/(float)numPairs;

      // feb: normalize y values
      transCnt = transCnt / (float) NI.GetInDeg();
      wealth = wealth / (float) NI.GetInDeg();

      transCntClusterV.Add(TFltPr((float)transCnt, localCluster));
      wealthClusterV.Add(TFltPr((float)wealth, localCluster));        
      invTransCntClusterV.Add(TFltPr(localCluster, (float)transCnt));
      invWealthClusterV.Add(TFltPr(localCluster, (float)wealth));

      // load and graph directly from this output file
      fprintf(out, "id:%d deg:%d trans:%f wealth:%f cluster:%f\n", sellerID, NI.GetInDeg(), transCnt, wealth, localCluster);
      //fprintf(out, "id:%d trans:%f wealth:%f %d %d cluster:%f\n", sellerID, transCnt, wealth, comLink, numPairs, localCluster);
    }
  }
  
  fclose(out);

  transCntClusterV.Sort(true); // assume that this sorts by key
  wealthClusterV.Sort(true); // lazy inverse
  invTransCntClusterV.Sort(true);
  invWealthClusterV.Sort(true);

  TFltPrV trimTrans;
  int trimSize = (int)(transCntClusterV.Len()*0.99);
  for (int i=0; i<trimSize; i++)
    trimTrans.Add(TFltPr((float)transCntClusterV[i].Val1, (float)transCntClusterV[i].Val2));

  float yTransLimit = (float)transCntClusterV[trimSize-1].Val1;

  TFltPrV trimWealth;
  trimSize = (int)(wealthClusterV.Len()*0.99);
  for (int i=0; i<trimSize; i++)
    trimWealth.Add(TFltPr((float)wealthClusterV[i].Val1, (float)wealthClusterV[i].Val2));

  float yWealthLimit = (float)wealthClusterV[trimSize-1].Val1;

  printf("99 y limits %f %f\n", yTransLimit, yWealthLimit);
  // feb: filter out y's for the inverse graph?

  // hardlimit of 0.14 to plot overlayed graphs
  float xLimit = 0.15;

  TFltPrV invTrimTrans;
  //int invTrimSize = (int)(invTransCntClusterV.Len()*0.99);
  int invTrimSize = 0;
  while ((float)invTransCntClusterV[invTrimSize].Val1 < xLimit)
    invTrimSize++;
  invTrimSize--;
  for (int i=0; i<invTrimSize; i++)
    //if ((float)invTransCntClusterV[i].Val2 < yTransLimit) // feb new: cut off y
      invTrimTrans.Add(TFltPr((float)invTransCntClusterV[i].Val1, (float)invTransCntClusterV[i].Val2));

  TFltPrV invTrimWealth;
  //invTrimSize = (int)(invWealthClusterV.Len()*0.99);
  for (int i=0; i<invTrimSize; i++)
    //if ((float)invWealthClusterV[i].Val2 < yWealthLimit) // feb new
      invTrimWealth.Add(TFltPr((float)invWealthClusterV[i].Val1, (float)invWealthClusterV[i].Val2));

  TGnuPlot tcGnuPlot("transCntCluster", "Transaction Count vs Clustering Coefficient");
  tcGnuPlot.AddPlot(trimTrans, gpwPoints, "", "pt 6");
  tcGnuPlot.SetXYLabel("Transaction Count", "Clustering Coefficient");
  tcGnuPlot.SetScale(gpsAuto);
  tcGnuPlot.SavePng();

  TGnuPlot wcGnuPlot("wealthCluster", "Wealth vs Clustering Coefficient");
  wcGnuPlot.AddPlot(trimWealth, gpwPoints, "", "pt 6");
  wcGnuPlot.SetXYLabel("Wealth", "Clustering Coefficient");
  wcGnuPlot.SetScale(gpsAuto);
  wcGnuPlot.SavePng();

  // spline smooth?

  TGnuPlot tc2GnuPlot("invTransCntCluster", "Clustering Coefficient vs Transactin Count");
  tc2GnuPlot.AddPlot(invTrimTrans, gpwPoints, "", "pt 6");
  tc2GnuPlot.SetXYLabel("Clustering Coefficient", "Transaction Count");
  tc2GnuPlot.SetScale(gpsAuto);
  tc2GnuPlot.SavePng();

  TGnuPlot wc2GnuPlot("invWealthCluster", "Clustering Coefficient vs Wealth");
  wc2GnuPlot.AddPlot(invTrimWealth, gpwPoints, "", "pt 6");
  wc2GnuPlot.SetXYLabel("Clustering Coefficient", "Wealth");
  wc2GnuPlot.SetScale(gpsAuto);
  wc2GnuPlot.SavePng();

  TFltPrV outputV;
  // was 100 bin for absolute trans
  binning(trimTrans, "binned_transCntCluster", 0.5, 1.0, false, "Binned Transaction Count vs Clustering Coefficient", "Transaction Count", "Clustering Coefficient", outputV);
  TFltPrV output2V;
  // was 10000 bin for absolute wealth
  binning(trimWealth, "binned_wealthCluster", 500.0, 1.0, false, "Binned Wealth vs Clustering Coefficient", "Wealth", "Clustering Coefficient", output2V);

  TFltPrV invOut1;
  TFltPrV invOut2;
  /*  
  binning(invTrimTrans, "binned_invTransCntCluster", 0.01, 1.0, false, "Binned Clustering Coefficient vs Transaction Count", "Clustering Coefficient", "Transaction Count", invOut1);
  binning(invTrimWealth, "binned_invWealthCluster", 0.01, 1.0, false, "Binned Clustering Coefficient vs Wealth", "Clustering Coefficient", "Wealth", invOut2);
  */
  
  // test exp binning on x... guessing it wont work
  
  binning(invTrimTrans, "binned_invTransCntCluster", 0.001, 2.0, true,"Binned Clustering Coefficient vs Transaction Count", "Clustering Coefficient", "Transaction Count", invOut1);
  binning(invTrimWealth, "binned_invWealthCluster", 0.001, 2.0, true, "Binned Clustering Coefficient vs Wealth", "Clustering Coefficient", "Wealth", invOut2);
  

  // write output files for comparison between msg and contact later
  FILE *out1 = fopen("invOutTrans.out", "w");
  for (int i=0; i<invOut1.Len(); i++)
    fprintf(out1, "%lf %lf\n", (float)invOut1[i].Val1, (float)invOut1[i].Val2);
  fclose(out1);

  FILE *out2 = fopen("invOutWealth.out", "w");
  for (int i=0; i<invOut2.Len(); i++)
    fprintf(out2, "%lf %lf\n", (float)invOut2[i].Val1, (float)invOut2[i].Val2);
  fclose(out2);

}

// msg vol vs prob of purchase, msg vol vs time to buy after last msg (if bought)
// also create random network (fix messages, randomize trades) and call this function
void edgeAgain(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;

  TIntH totMsg; // count of num msg strengths
  TIntH sucMsg; // count of num msg strengths leading to trades
  TFltPrV msgProb; // msg strength vs trade prob
  TFltPrV msgLag; // msg strength vs trade time lag
 
  double tot = 0.0;
  double success = 0.0;

  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++)
  {
    TIntPrV msgDates; 
    TIntV tradeDates;

    TTaobaoShopEdge& msgEdge = EI();
    int msgCnt = msgEdge.CntTXStrength();

    for (int i=0; i<msgEdge.CntTX(); i++) {
      TSecTm msgTm = msgEdge.GetTime(i);
      msgDates.Add(TIntPr(msgTm.GetAbsSecs(), (int)msgEdge.GetStrength(i)));
    }

    TInt SrcID = EI.GetSrcNId();
    TInt DstID = EI.GetDstNId();
    int reverseFnd = 0;
    if (m->IsEdge(DstID, SrcID)) {
      TTaobaoShopEdge& msgEdge2 = m->GetEDat(DstID, SrcID);
      msgCnt += msgEdge2.CntTXStrength();
      reverseFnd = 1;
    
      for (int i=0; i<msgEdge2.CntTX(); i++) {
        TSecTm msgTm = msgEdge2.GetTime(i);
        msgDates.Add(TIntPr(msgTm.GetAbsSecs(), (int)msgEdge2.GetStrength(i)));
      }
    }

    msgDates.Sort();

    if (reverseFnd == 0 || (reverseFnd==1 && SrcID < DstID)) {

      if (totMsg.IsKey(msgCnt)) {
        int existCnt = totMsg.GetDat(msgCnt);
        totMsg.AddDat(msgCnt, existCnt+1);
      }
      else {
        totMsg.AddDat(msgCnt, 1);
        sucMsg.AddDat(msgCnt, 0);
      }

      tot= tot + 1.0;

      if (s->IsEdge(SrcID, DstID) || s->IsEdge(DstID, SrcID)) {

        if (s->IsEdge(SrcID, DstID)) {
          TTaobaoShopEdge& tradeEdge = s->GetEDat(SrcID, DstID);
          for (int i=0; i<tradeEdge.CntTX(); i++) {
            TSecTm msgTm = tradeEdge.GetTime(i);
            tradeDates.Add(msgTm.GetAbsSecs());
          }
        }

        if (s->IsEdge(DstID, SrcID)) {
          TTaobaoShopEdge& tradeEdge2 = s->GetEDat(DstID, SrcID);
          for (int i=0; i<tradeEdge2.CntTX(); i++) {
            TSecTm msgTm = tradeEdge2.GetTime(i);
            tradeDates.Add(msgTm.GetAbsSecs());
          }
        }

        tradeDates.Sort();

        // at least one trade occurs after a message
        if (tradeDates[tradeDates.Len()-1] > msgDates[0].Val1) {
          int existCnt = sucMsg.GetDat(msgCnt);
          sucMsg.AddDat(msgCnt, existCnt+1);

          success = success + 1.0;

          int minDif = 60*24*60*60;
          int daySec = 24*60*60;
          // try to find small gap between msg and trade
          /*
          int m = 0;
          int t = 0;
          while (true) {
            while (t<tradeDates.Len() && msgDates[m]>tradeDates[t])
              t++;
            
            if (t == tradeDates.Len())
              break;

            while (m<msgDates.Len() && msgDates[m]<=tradeDates[t]) {
              if (tradeDates[t]-msgDates[m] < minDif)
                minDif = tradeDates[t]-msgDates[m];
              m++;
            }
            
            if (m == msgDates.Len())
              break;

          }
          */

          // actually, just find dif between first msg and first trade after

          int t = 0;
          while (tradeDates[t] < msgDates[0].Val1)
            t++;
          //minDif = tradeDates[t]-msgDates[0]; // time since first message

          // strength of msg will be # messages before first valid trade
          int m = 0;
          int cumul = 0;
          while (m<msgDates.Len() && msgDates[m].Val1 <= tradeDates[t]) {
            cumul += msgDates[m].Val2; // sum up msg strengths
            m++;
          }

          minDif = tradeDates[t]-msgDates[m-1].Val1; // time since previous message

          int dayDif = minDif / daySec;
          if (dayDif < 60) // there seem to be a few large errors
            msgLag.Add(TFltPr((float)cumul, (float)dayDif));
        }
      }
      
    }
  }
  
  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);
  msgLag.Sort();

  printf("%f %f %f\n", success, tot, success/tot);

  int total = 0;
  for (int i=0; i<totMsg.Len(); i++)
    total += totMsg[i];
 
  int trimLen = 0;
  int cumul =0;
  while (cumul < total*0.99) {
    cumul += totMsg[trimLen];
    trimLen++;
  }
  printf("old TRIMLEN %d\n", trimLen);
  
  trimLen = 0; 
  while ((int) totMsg.GetKey(trimLen) < 2000)
    trimLen++;
  trimLen--;

  printf("new TRIMLEN %d\n", trimLen);

  for (int i=0; i<trimLen; i++) {
    if((int)totMsg.GetKey(i)!=sucMsg.GetKey(i))
      printf("key error\n");
    float tradeProb = (float)sucMsg[i] / (float)totMsg[i];
    msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
  }
  
  // note that msg strength def differs between the two plots
  // first plot has total strength, second plot has strength before trade
  
  TGnuPlot tPGnuPlot("msgVolTradeProb", "Message Strength vs Trade Probability");
  tPGnuPlot.AddPlot(msgProb, gpwLinesPoints, "", "pt 6 smooth bezier");
  tPGnuPlot.SetXYLabel("Message Strength", "Trade Probability");
  tPGnuPlot.SetScale(gpsAuto);
  tPGnuPlot.SavePng();

  TFltPrV randomV;
  
  TFltPrV outputV2;
  
  // orig 50 buckets
  binning(msgProb, "binned_msgVolTradeProb", 2.0, 2.0, false, "Binned Message Strength vs Trade Prob", "Message Strength", "Trade Prob", outputV2);

  for (int i=0; i<outputV2.Len(); i++)
    randomV.Add(TFltPr((float)outputV2[i].Val1, (float)success/tot));

  TGnuPlot tZGnuPlot("binned_msgVolTradeProb", "Binned Message Strength vs Trade Probability");
  tZGnuPlot.AddPlot(outputV2, gpwLinesPoints, "actual", "pt 6");
  tZGnuPlot.AddPlot(randomV, gpwLinesPoints, "random", "pt 6");
  tZGnuPlot.SetXYLabel("Message Strength", "Trade Probability");
  tZGnuPlot.SetScale(gpsAuto);
  tZGnuPlot.SavePng();

  TFltPrV msgLagTrim;
  for (int i=0; i<msgLag.Len(); i++) {
    if (msgLag[i].Val1 > 2000)
      break;
    msgLagTrim.Add(TFltPr((float)msgLag[i].Val1, (float)msgLag[i].Val2));
  }

  TGnuPlot tLGnuPlot("msgVolTradeLag", "Message Strength vs Trade Lag");
  tLGnuPlot.AddPlot(msgLagTrim, gpwPoints, "", "pt 6");
  tLGnuPlot.SetXYLabel("Message Strength", "Trade Lag");
  tLGnuPlot.SetScale(gpsAuto);
  tLGnuPlot.SavePng();

  TFltPrV outputV;

  binning(msgLagTrim, "binned_msgVolTradeLag", 2.0, 2.0, false, "Binned Message Strength vs Trade Lag", "Message Strength", "Trade Lag", outputV);

}

void edgeTest(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;

  // first graph edge embeddedness and strength. these stats should be done
  // undirected. lets try edge structural graphs first
  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    TInt srcID = EI.GetSrcNId();
    TInt dstID = EI.GetDstNId();
    
    if ((m->IsEdge(dstID, srcID) && srcID<dstID) || (!m->IsEdge(dstID, srcID))) {
      TTaobaoShopNet::TNodeI NI = m->GetNI(srcID);
     
      TIntH triNode;
      // go through outdeg of src, then go through indeg of src
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int msgID = NI.GetOutNId(i);
        if (m->IsEdge(srcID, msgID) || m->IsEdge(msgID, srcID)) 
          triNode.AddDat(msgID, 1);
      }
      
      for (int i=0; i<NI.GetInDeg(); i++) {
        int msgID = NI.GetInNId(i);
        if (m->IsEdge(srcID, msgID) || m->IsEdge(msgID, srcID))
          if (!triNode.IsKey(msgID))
            triNode.AddDat(msgID, 1);
      }

      // go through all nodes in triNode, add them 
    }
  }
}

// graph edge trade/msg distribution, edge num msgs vs num trades (and inverse)
void edgeStudy(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  
  TIntH msgH;
  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int sourceID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int msgVol = 0;
      int tradeVol = 0;
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      msgVol = msgVol + shopEdge.CntTXStrength();
      int destID = NI.GetOutNId(i);

      int found = 0;
      TTaobaoShopNet::TNodeI NJ = m->GetNI(destID);
      for (int j=0; j<NJ.GetOutDeg(); j++)
        if (NJ.GetOutNId(j) == sourceID) {
          TTaobaoShopEdge& reverseEdge = NJ.GetOutEDat(j);
          msgVol = msgVol + reverseEdge.CntTXStrength();
          found = 1;
          break;
        }
   
      // do not add edge if reverse pair already added
      if (!(found==1 && sourceID > destID)) {
        
        if (msgH.IsKey(msgVol)) {
          int existCnt = msgH.GetDat(msgVol);
          msgH.AddDat(msgVol, existCnt+1);
        }
        else 
          msgH.AddDat(msgVol, 1);
      }    

    }
  }
  
  int totMsg=0;
  TFltPrV msgV;
  FILE *msgout = fopen("msgTemp.out", "w");
  msgH.SortByKey(true);
  for (int i=0; i<msgH.Len(); i++) {
    totMsg += msgH[i];
    fprintf(msgout, "%d %d\n", (int)msgH.GetKey(i), (int)msgH[i]);
    msgV.Add(TFltPr((float) msgH.GetKey(i), (float) msgH[i]));
  }
  fclose(msgout);
  printf("TOTAL MESSAGES %d\n", totMsg);

  TGnuPlot EdMsGnuPlot("msg_vol_distribution", "Edge Message Volume Distribution");
  EdMsGnuPlot.AddPlot(msgV, gpwLinesPoints, "", "pt 6");
  EdMsGnuPlot.SetXYLabel("Message Volume", "Message Cnt");
  EdMsGnuPlot.SetScale(gpsAuto);
  EdMsGnuPlot.SavePng();

  TIntH tradeH;
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int sourceID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int tradeVol = 0;
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      tradeVol = tradeVol + shopEdge.CntTX();
      int destID = NI.GetOutNId(i);

      int found = 0;
      TTaobaoShopNet::TNodeI NJ = s->GetNI(destID);
      for (int j=0; j<NJ.GetOutDeg(); j++)
        if (NJ.GetOutNId(j) == sourceID) {
          TTaobaoShopEdge& reverseEdge = NJ.GetOutEDat(j);
          tradeVol = tradeVol + reverseEdge.CntTX();
          found = 1;
          break;
        }
      
      if (!(found==1 && sourceID > destID)) {
        if (tradeH.IsKey(tradeVol)) {
          int existCnt = tradeH.GetDat(tradeVol);
          tradeH.AddDat(tradeVol, existCnt+1);
        }
        else
          tradeH.AddDat(tradeVol, 1);             
      }

    }
  }
  
  int totTrade=0;
  TFltPrV tradeV;
  FILE *tradeout = fopen("tradeTemp.out", "w");
  tradeH.SortByKey(true);
  for (int i=0; i<tradeH.Len(); i++) {
    totTrade += (int)tradeH[i];
    fprintf(tradeout, "%d %d\n", (int)tradeH.GetKey(i), (int)tradeH[i]);
    tradeV.Add(TFltPr((float) tradeH.GetKey(i), (float) tradeH[i]));
  }
  fclose(tradeout);
  printf("TOTAL TRADES %d\n", totTrade);

  TGnuPlot EdTrGnuPlot("trade_vol_distribution", "Edge Trade Volume Distribution");
  EdTrGnuPlot.AddPlot(tradeV, gpwLinesPoints, "", "pt 6");
  EdTrGnuPlot.SetXYLabel("Trade Volume", "Trade Cnt");
  EdTrGnuPlot.SetScale(gpsAuto);
  EdTrGnuPlot.SavePng();

  TFltPrV outputV;
  binning(msgV, "binned_msg_distribution", 1.0, 2.0, true, "Binned Message Volume Distribution", "Message Volume", "Message Cnt", outputV);
  TFltPrV output2V;
  binning(tradeV, "binned_trade_distribution", 1.0, 2.0, true, "Binned Trade Volume Distribution", "Trade Volume", "Trade Cnt", output2V);

  TFltPrV msgTradeV;
  TFltPrV pureMsgTradeV; // without chat-only edges
  TFltPrV tradeMsgV;
  TFltPrV pureTradeMsgV; // wihtout trade-only edges
  // yeah, my variables are not named well.. opposite in fact

  int zeroTradeMsgCnt=0;
  int nonZeroTradeMsgCnt=0;
  float zeroTradeMsgVol=0.0;
  float nonZeroTradeMsgVol=0.0;

  TIntH zeroTradeMsgH; // look at msg distribution for pure msg edges
  // not going to bother doing this for pure trade edges

  // this is inefficient, can change later
  // for all message edges, find (msg vol, trade vol) pairs
  int tCnt=0;
  FILE *out2 = fopen("temp2.out", "w");
  FILE *outT = fopen("largeTemp.out", "w");
  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int sourceID = NI.GetId(); 

    for (int i=0; i<NI.GetOutDeg(); i++) {
      int msgVol = 0;
      int tradeVol = 0;
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      msgVol = msgVol + shopEdge.CntTXStrength();
      int destID = NI.GetOutNId(i);

      int found = 0;
      TTaobaoShopNet::TNodeI NJ = m->GetNI(destID);
      for (int j=0; j<NJ.GetOutDeg(); j++)
        if (NJ.GetOutNId(j) == sourceID) {
          TTaobaoShopEdge& reverseEdge = NJ.GetOutEDat(j);
          msgVol = msgVol + reverseEdge.CntTXStrength();
          found = 1;
          break; // get undirected msg volume
        }
      
      tCnt++;

      if (!(found==1 && sourceID > destID)) { 
        if (s->IsNode(sourceID) && s->IsNode(destID)) { 
          TTaobaoShopNet::TNodeI NK = s->GetNI(sourceID);
          for (int k=0; k<NK.GetOutDeg(); k++)
            if (NK.GetOutNId(k) == destID) {
              TTaobaoShopEdge& tempEdge = NK.GetOutEDat(k);
              tradeVol += tempEdge.CntTX();
              break;
            }
          
          NK = s->GetNI(destID);
          for (int k=0; k<NK.GetOutDeg(); k++)
            if (NK.GetOutNId(k) == sourceID) {
              TTaobaoShopEdge& tempEdge = NK.GetOutEDat(k);
              tradeVol += tempEdge.CntTX();
              break;
            }
         
          msgTradeV.Add(TFltPr((float)msgVol, (float)tradeVol));
          fprintf(out2, "%d %d %d %d\n", sourceID, destID, msgVol, tradeVol);
          if (msgVol > 6000.0) // debug
            fprintf(outT, "%d %d\n", msgVol, tradeVol);
          if (tradeVol > 0) { // FEB 2010 test - only consider nonzero trades
            pureMsgTradeV.Add(TFltPr((float)msgVol, (float)tradeVol));
            //tradeMsgV.Add(TFltPr(tradeVol, msgVol));
            nonZeroTradeMsgCnt++;
            nonZeroTradeMsgVol += msgVol;
          }
          else {
            zeroTradeMsgCnt++;
            zeroTradeMsgVol += msgVol;
            if (zeroTradeMsgH.IsKey(msgVol)) {
              int existCnt = zeroTradeMsgH.GetDat(msgVol);
              zeroTradeMsgH.AddDat(msgVol, existCnt+1);
            }
            else
              zeroTradeMsgH.AddDat(msgVol, 1);
            
          }

        }
      }

    }
  }
  fclose(out2);
  fclose(outT);

  // FEB new, plot pure msg distribution ( 0 trade )
  TFltPrV zeroTradeMsgV;
 
  zeroTradeMsgH.SortByKey(true);
  for (int i=0; i<zeroTradeMsgH.Len(); i++) {
    zeroTradeMsgV.Add(TFltPr((float) zeroTradeMsgH.GetKey(i), (float) zeroTradeMsgH[i]));
  }

  TGnuPlot ZEdMsGnuPlot("zero_trade_msg_vol_distribution", "Pure Edge Message Volume Distribution");
  ZEdMsGnuPlot.AddPlot(zeroTradeMsgV, gpwPoints, "", "pt 6");
  ZEdMsGnuPlot.SetXYLabel("Message Volume", "Message Cnt");
  ZEdMsGnuPlot.SetScale(gpsAuto);
  ZEdMsGnuPlot.SavePng();

  TFltPrV output3V;
  binning(zeroTradeMsgV, "binned_zero_trade_msg_distribution", 1.0, 2.0, true, "Binned Pure Message Volume Distribution", "Message Volume", "Message Cnt", output3V);

  // new DUPLICATE block for the inverse case
  // not really needed, but doing this to be safe...
  // for all trade edges, find all (trade vol, msg vol) pairs
  FILE *out3 = fopen("temp3.out", "w");
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int sourceID = NI.GetId();

    for (int i=0; i<NI.GetOutDeg(); i++) {
      int msgVol = 0;
      int tradeVol = 0;
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      tradeVol = tradeVol + shopEdge.CntTX();
      int destID = NI.GetOutNId(i);

      int found = 0;
      TTaobaoShopNet::TNodeI NJ = s->GetNI(destID);
      for (int j=0; j<NJ.GetOutDeg(); j++)
        if (NJ.GetOutNId(j) == sourceID) {
          TTaobaoShopEdge& reverseEdge = NJ.GetOutEDat(j);
          tradeVol = tradeVol + reverseEdge.CntTX();
          found = 1;
          break; // get undirected msg volume
        }

      if (!(found==1 && sourceID > destID)) {
        if (m->IsNode(sourceID) && m->IsNode(destID)) {
          TTaobaoShopNet::TNodeI NK = m->GetNI(sourceID);
          for (int k=0; k<NK.GetOutDeg(); k++)
            if (NK.GetOutNId(k) == destID) {
              TTaobaoShopEdge& tempEdge = NK.GetOutEDat(k);
              msgVol += tempEdge.CntTXStrength();
              break;
            }

          NK = m->GetNI(destID);
          for (int k=0; k<NK.GetOutDeg(); k++)
            if (NK.GetOutNId(k) == sourceID) {
              TTaobaoShopEdge& tempEdge = NK.GetOutEDat(k);
              msgVol += tempEdge.CntTXStrength();
              break;
            }

            fprintf(out3, "%d %d %d %d\n", sourceID, destID, msgVol, tradeVol);
            tradeMsgV.Add(TFltPr((float)tradeVol, (float)msgVol));
            if (msgVol > 0) {
              pureTradeMsgV.Add(TFltPr((float)tradeVol, (float)msgVol));
            }
        }
      }

    }
  }
  fclose(out3);

  msgTradeV.Sort(true); // assume that this sorts by key
  tradeMsgV.Sort(true);
  pureMsgTradeV.Sort(true);
  pureTradeMsgV.Sort(true);

  printf("TCNT %d\n", tCnt); // this num should be equal to the "edges" in the paper

  float xLimit = 8000.0; // defaults 5k, 8k

  TFltPrV trim99;
  int trimSize = (int)(msgTradeV.Len()*0.999);
  printf("MAX MSGVOL %f\n", (float)msgTradeV[msgTradeV.Len()-1].Val1);
  trimSize = 0; // new hard thresh
  while ((float)msgTradeV[trimSize].Val1 < xLimit)
    trimSize++;
  trimSize--;
  printf("TRIM SIZE %d\n", trimSize);
  for (int i=0; i<trimSize; i++)
    trim99.Add(TFltPr((float)msgTradeV[i].Val1, (float)msgTradeV[i].Val2));

  TFltPrV trimPure;
  int trimSizeP = 0;
  while((float)pureMsgTradeV[trimSizeP].Val1 < xLimit)
    trimSizeP++;
  trimSizeP--;
  for (int i=0; i<trimSizeP; i++)
    trimPure.Add(TFltPr((float)pureMsgTradeV[i].Val1, (float)pureMsgTradeV[i].Val2));

  float xLimit2 = 65.0; // was 200 for full

  TFltPrV inv99;
  int invSize = (int)(tradeMsgV.Len()*0.999);
  printf("orig inv size %d\n", invSize);
  invSize = 0;
  while ((float)tradeMsgV[invSize].Val1 < xLimit2) 
    invSize++;
  invSize--;
  printf("INV SIZE %d\n", invSize);
  printf("MAX TRADEVOL %f\n", (float)tradeMsgV[tradeMsgV.Len()-1].Val1);
  for (int i=0; i<invSize; i++)
    inv99.Add(TFltPr((float)tradeMsgV[i].Val1, (float)tradeMsgV[i].Val2));

  TFltPrV invPure;
  int invSizeP = 0;
  while ((float)pureTradeMsgV[invSizeP].Val1 < xLimit2)
    invSizeP++;
  invSizeP--;
  for (int i=0; i<invSizeP; i++)
    invPure.Add(TFltPr((float)pureTradeMsgV[i].Val1, (float)pureTradeMsgV[i].Val2));


  TFltPrV outMTV1;
  //TFltPrV outMTV2;
  binning(trim99, "binned_msg_vol_vs_trade_vol", 500.0, 1.0, false, "Binned Message Volume vs Trade Volume", "Message Volume", "Trade Volume", outMTV1);
  //binning(trim99, "binned_msg_vol_vs_trade_vol2", 250.0, 1.0, false, "Binned Message Volume vs Trade Volume", "Message Volume", "Trade Volume", outMTV2);

  TFltPrV outPMTV; // by pure, i mean no chat-only edges
  binning(trimPure, "binned_pure_msg_vol_vs_trade_vol", 500.0, 1.0, false,
  "Binned Message Volume vs Trade Volume (ignore chat only) ", "Message Volume", "Trade Volume", outPMTV);

  // feb temp: added bezier
  TGnuPlot overMsgTradePlot("binned_over_msg_trade","Message Volume vs Trade Volume");
  overMsgTradePlot.AddPlot(outMTV1, gpwLinesPoints, "Normal", "pt 6 smooth bezier");
  overMsgTradePlot.AddPlot(outPMTV, gpwLinesPoints, "Ignore Chat-Only Edges", "pt 6 smooth bezier");
  overMsgTradePlot.SetXYLabel("Message Volume", "Trade Volume");
  overMsgTradePlot.SetScale(gpsAuto);
  overMsgTradePlot.SavePng();


  TFltPrV outTVM1;
  //TFltPrV outTVM2;
  binning(inv99, "binned_trade_vol_vs_msg_vol", 5.0, 1.0, false, "Binned Trade Volume vs Message Volume", "Trade Volume", "Message Volume", outTVM1);
  //binning(inv99, "binned_trade_vol_vs_msg_vol2", 10.0, 1.0, false, "Binned Trade Volume vs Message Volume", "Trade Volume", "Message Volume", outTVM2);

  TFltPrV outPTVM;
  binning(invPure, "binned_pure_trade_vol_vs_msg_vol", 5.0, 1.0, false,"Binned Trade Volume vs Message Volume (ignore trade only)", "Trade Volume", "Message Volume", outPTVM);

  // feb temp added bezier
  TGnuPlot overTradeMsgPlot("binned_over_trade_msg", "Trade Volume vs Message Volume");
  overTradeMsgPlot.AddPlot(outTVM1, gpwLinesPoints, "Normal", "pt 6 smooth bezier");
  overTradeMsgPlot.AddPlot(outPTVM, gpwLinesPoints, "Ignore Trade-Only Edges", "pt 6 smooth bezier");
  overTradeMsgPlot.SetXYLabel("Trade Volume", "Message Volume");
  overTradeMsgPlot.SetScale(gpsAuto);
  overTradeMsgPlot.SavePng();

  printf("binning done\n");

  TGnuPlot tcGnuPlot("msg_vol_vs_trade_vol", "Edge Message Volume vs Trade Volume");
  tcGnuPlot.AddPlot(msgTradeV, gpwPoints, "", "pt 6");
  tcGnuPlot.SetXYLabel("Message Volume", "Trade Volume");
  tcGnuPlot.SetScale(gpsAuto);
  tcGnuPlot.SavePng();

  TGnuPlot ScGnuPlot("trade_vol_vs_msg_vol", "Edge Trade Volume vs Message Volume");
  ScGnuPlot.AddPlot(tradeMsgV, gpwPoints, "", "pt 6");
  ScGnuPlot.SetXYLabel("Trade Volume", "Message Volume");
  ScGnuPlot.SetScale(gpsAuto);
  ScGnuPlot.SavePng();

  // msg stats with respect to trade (zero or not)
  printf("zeroCnt %d nonZeroCnt %d zeroVol %f nonZeroVol %f\n", zeroTradeMsgCnt, nonZeroTradeMsgCnt, zeroTradeMsgVol, nonZeroTradeMsgVol);
}

// OCT NEW: replot edge overlays
void plotOverlayMsgTrade() {
  int sourceID, destID, msgVol, tradeVol;
  TFltPrV msgTradeV;
  TFltPrV pureMsgTradeV; // without chat-only edges

  char line[2024];

  FILE *in = fopen("temp2.out", "r");

  //fprintf(out2, "%d %d %d %d\n", sourceID, destID, msgVol, tradeVol);
  //fscanf(in, "%d %d %d %d", &sourceID, &destID, &msgVol, tradeVol);
  
  while (true) {
    memset(line, 0, 2024);
    fgets(line, 2024, in);
    if (strlen(line) == 0)
      break;
    sscanf(line, "%d %d %d %d", &sourceID, &destID, &msgVol, &tradeVol);
    
      msgTradeV.Add(TFltPr((float)msgVol, (float)tradeVol));
      if (tradeVol > 0) 
        pureMsgTradeV.Add(TFltPr((float)msgVol, (float)tradeVol));
    
  }

  fclose(in);

  msgTradeV.Sort(true); // assume that this sorts by key
  pureMsgTradeV.Sort(true);

  TFltPrV outMTV1;
  binning(msgTradeV, "binned_mtVol", 1.0, 2.0, true, "", "Message Volume", "Trade Volume", outMTV1);
 
  TFltPrV outPMTV; // by pure, i mean no chat-only edges
  binning(pureMsgTradeV, "binned_pmtvVol", 1.0, 2.0, true, "", "Message Volume", "Trade Volume", outPMTV);

  TGnuPlot overMsgTradePlot("binned_overMT","", false);
  overMsgTradePlot.AddCmd("set key top left");
  overMsgTradePlot.AddPlot(outPMTV, gpwLinesPoints, "Message+Trade", "lt 1 pt 5 ps 3");
  overMsgTradePlot.AddPlot(outMTV1, gpwLinesPoints, "Message", "lt 2 pt 7 ps 3");
    overMsgTradePlot.SetXYLabel("Message Volume", "Trade Volume");
  overMsgTradePlot.SetScale(gpsLog10X);
  //overMsgTradePlot.SavePng();
  //overMsgTradePlot.SetXRange(1, 5000);
  overMsgTradePlot.SaveEps("binned_overMT.eps", 30, false);

}

/* graph contact/msg vs trade per node statistics 
IN PROGRESS
*/
void compareNetworks(PTaobaoNet t, TStr jobType) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m;
  if (jobType == "msg")
    m = t->msgNet;
  else if (jobType == "contact")
    m = t->contactNet;
  else
    return;

  TFltPrV msgBuyV;
  TFltPrV msgSellV;
  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int msgVol = 0;

    for (int i=0; i<NI.GetOutDeg(); i++) {
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      msgVol = msgVol + shopEdge.CntTX();
    }

    for (int i=0; i<NI.GetInDeg(); i++) {
      TTaobaoShopEdge& shopEdge = NI.GetInEDat(i);
      msgVol = msgVol + shopEdge.CntTX();
    }

    if (jobType == "contact")
      msgVol = msgVol / 2; // for contact network, due to bidirectionality
    int nodeID = NI.GetId();
    
    // buyer -> seller
    int buyVol = 0, sellVol = 0;
    if (s->IsNode(nodeID)) {
      // update buyVol, sellVol
      TTaobaoShopNet::TNodeI NJ = s->GetNI(nodeID);
      int outDeg = NJ.GetOutDeg();
      for (int i=0; i<outDeg; i++) {
        TTaobaoShopEdge& shopEdge = NJ.GetOutEDat(i);
        buyVol = buyVol + shopEdge.CntTX();
      }

      int inDeg = NJ.GetInDeg();
      for (int i=0; i<inDeg; i++) {
        TTaobaoShopEdge& shopEdge = NJ.GetInEDat(i);
        sellVol = sellVol + shopEdge.CntTX();
      }
    }
    
    msgBuyV.Add(TFltPr(msgVol, buyVol));
    msgSellV.Add(TFltPr(msgVol, sellVol));
  }

  TFltPrV outputV;
  binning(msgBuyV, "binned_msg_vol_vs_buy_vol", 100.0, 1.0, false, "Binned Message Volume vs Buy Volume", "Message Volume", "Buy Volume", outputV);
  TFltPrV output2V;
  binning(msgSellV, "binned_msg_vol_vs_sell_vol", 100.0, 1.0, false, "Binned Message Volume vs Sell Volume", "Message Volume", "Sell Volume", output2V);

  TGnuPlot ScGnuPlot("msg_vol_vs_buy_vol", "Message Volume vs Buy Volume");
  ScGnuPlot.AddPlot(msgBuyV, gpwPoints, "", "pt 6");
  ScGnuPlot.SetXYLabel("Message Volume", "Buy Volume");
  ScGnuPlot.SetScale(gpsAuto);
  ScGnuPlot.SavePng();

  TGnuPlot ScGnuPlot2("msg_vol_vs_sell_vol", "Message Volume vs Sell Volume");
  ScGnuPlot2.AddPlot(msgSellV, gpwPoints, "", "pt 6");
  ScGnuPlot2.SetXYLabel("Message Volume", "Sell Volume");
  ScGnuPlot2.SetScale(gpsAuto);
  ScGnuPlot2.SavePng();
}



int main(int argc, char* argv[]) {

  PTaobaoNet t = TTaobaoNet::New();
  TStr file1;
  TStr file2;
  TStr file3;
  TStr job1;
  TStr job2;
  TStr job3;
  if (argc != 7) {
    fprintf(stderr, "Usage:compareGraphs file1 jobType file2 jobType file3 jobType\n");
    exit(-1);
  } 
  file1 = TStr(argv[1]);
  job1 = TStr(argv[2]);
  file2 = TStr(argv[3]);
  job2 = TStr(argv[4]);
  file3 = TStr(argv[5]);
  job3 = TStr(argv[6]);
  
  //plotMutual();
  //plotNetOverlay();
  //plotStructure();
  //plotTriangle();
  //plotMutualMsg();
  plotOverlayMsgTrade();
  return 0;

  if (job1 == "fwd")
    t->LoadShopNet(file1);
  else if (job1 == "msg")
    t->LoadChatNet(file1, true);
  else if (job1 == "contact")
    t->LoadChatNet(file1, false);
 
  if (job2 == "fwd")
    t->LoadShopNet(file2);
  else if (job2 == "msg")
    t->LoadChatNet(file2, true);
  else if (job2 == "contact")
    t->LoadChatNet(file2, false);

  // loading of third datset should be optional
  
  if (job3 == "fwd")
    t->LoadShopNet(file3);
  else if (job3 == "msg")
    t->LoadChatNet(file3, true);
  else if (job3 == "contact")
    t->LoadChatNet(file3, false);
  

  printf("Loading complete\n");

  //PTaobaoShopNet s = t->shopNet;
  //PTaobaoShopNet c = t->msgNet;
  
  //printf("Nodes %d Edges %d\n", s->GetNodes(), s->GetEdges());
  //printf("Nodes %d Edges %d\n", c->GetNodes(), c->GetEdges());

  //triangleOrderTest(t);
  //triangleClosing(t);
  //MutualBuyers(t);
  MutualContacts(t);
  //EventSequences(t);
  //networkStructure(t, jobType);
  //edgeStudy(t);
  //edgeAgain(t);
  //compareNetworks(t, jobType);

  //earliestMsgDate(t);
  
  //measureTradeLikelihood(t);

  //t->ConvoTrades();
  //FindTriangles(t);

  //TFltPrV SellerWealthConvoV;
  //SellerScatter(t, SellerWealthConvoV);

  //TGnuPlot ScGnuPlot("sellerScatter", "Seller Wealth and Shopting");
  //ScGnuPlot.AddPlot(SellerWealthConvoV, gpwPoints, "", "pt 6");
  //ScGnuPlot.SetXYLabel("wealth", "chat volume");
  //ScGnuPlot.SetScale(gpsLog10XY);
  //ScGnuPlot.SavePng();

}
