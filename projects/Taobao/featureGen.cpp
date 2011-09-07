#include "Snap.h"
#include "stdafx.h"
#include "ncp.cpp"
#include "taobaonet.h"
#include "graphing.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// this file is meant to be used as a hack to compute partial features/values
// mirror of updatePred in triangleTesting

void updatePred(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;
  
  srand ( time(NULL) );
  
  // NOTE fwd file given a prodID, there will be multiple buyers, just use
  // first one, dont think it matters whcih one we assume 

  char line[2024];
  TStrH catMap;
  
  TStrH prodBuyer;
  TStrH prodSeller;
  TStrH prodTime;
  TStrFltH prodPrice; // for sanity check, ignore if prices dont match
  TStrFltH prodShip;
  TIntPrStrH BSProd; //(buyer, seller) -> prod
  
  TIntH buyerNumClusters; // number of product clusters 
  TIntH sellerNumClusters;
  TIntStrVH buyerCats; // categories per buyer
  TIntStrVH sellerCats;  
  // use to output relative price per product
  TIntFltH buyerRelPriceTot;
  TIntH buyerRelPriceCnt;
  TIntFltH sellerRelPriceTot;
  TIntH sellerRelPriceCnt;
  
  // use to compute 
  int buyerSum=0;
  int sellerSum=0;

  // data loading
  TIntH loadedBNC;
  TIntH loadedSNC;
  TIntH loadedBC;
  TIntH loadedSC;
  TIntFltH loadedBRP;
  TIntFltH loadedSRP;

  // time decay exponents
  float td1 = -0.1;
  float td2 = -0.3;
  float td3 = -0.5;

  // pre-computed pageranks
  TIntFltH fwdPR;
  TIntFltH msgPR;
  TIntFltH conPR;
  int nID;
  double pr;
   
  int lA, lB;
  float lC;
  FILE *inBNC = fopen("buyerNumClusters.out", "r");
  while(!feof(inBNC)) {
    fscanf(inBNC, "%d %d", &lA, &lB);
    loadedBNC.AddDat(lA, lB);
  }
  fclose(inBNC);
  FILE *inSNC = fopen("sellerNumClusters.out", "r");
  while(!feof(inSNC)) {
    fscanf(inSNC, "%d %d", &lA, &lB);
    loadedSNC.AddDat(lA, lB);
  }
  fclose(inSNC);

  FILE *inBC = fopen("buyerCats.out", "r");
  while(!feof(inBC)) {
    fscanf(inBC, "%d %d", &lA, &lB);
    loadedBC.AddDat(lA, lB);
  }
  fclose(inBC);

  FILE *inSC = fopen("sellerCats.out", "r");
  while(!feof(inSC)) {
    fscanf(inSC, "%d %d", &lA, &lB);
    loadedSC.AddDat(lA, lB);
  }
  fclose(inSC);
 
  //  FILE *trainFiles[100];
  // FILE *testFiles[100];
  int trainSize[100]; // number of qid's per cat file
  int testSize[100];
  // may have to open crapload of files simultaneously

  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();
  int daySec = 24*60*60;
  
  // compute msgs and trades in 4 different time scales
  int msgWindowSize = 100; // consider all previous history                                                                             
  int dayWindowSize = 1;
  int medWindowSize = 3;
  int weekWindowSize = 7;

  int msgWindow = msgWindowSize*daySec;
  int dayWindow = dayWindowSize*daySec;
  int medWindow = medWindowSize*daySec;
  int weekWindow = weekWindowSize*daySec;

  int negShipping = 0;
  float maxShipping = 0.0;
  float minShipping = 99999.0;

  TStrV fields;
  //char line[2024];
  FILE *in = fopen("/lfs/1/tmp/mengqiu/fwd.1000000.txt", "r");
  while (!feof(in)) {
    memset(line, 0, 2024); 
    fgets(line, 2024, in);
    if (strlen(line) == 0)
      break;
    
    TStr(line).SplitOnAllCh('\t', fields, false);
    
    TInt BuyerID = TInt(atoi(fields[0].GetTrunc().CStr())); // buyer id
    TInt SellerID = TInt(atoi(fields[1].GetTrunc().CStr())); // seller id
    TIntPr BS = TIntPr(BuyerID, SellerID);
    TStr ProductID = fields[3].GetTrunc();
    TFlt TotalPrice = TFlt(atof(fields[4].GetTrunc().CStr()));
    TFlt ProductPrice = TFlt(atof(fields[5].GetTrunc().CStr()));
    TInt Quantity = TInt(atoi(fields[6].GetTrunc().CStr()));

    // store shippingCost for later
    // over 10% of shipping is neg price. WTF
    // not adding shipping to product cost for now
    TFlt ShippingCost = (TotalPrice-ProductPrice)/(float)Quantity;
    if (ShippingCost < minShipping)
      minShipping = ShippingCost;
    if (ShippingCost > maxShipping)
      maxShipping = ShippingCost;
    if (ShippingCost < 0.0) {
      //printf("shipping less than zero! %f\n", (float)ShippingCost);
      negShipping++;
      //printf("%d\n", (int)Quantity);
      //return;
    }

    TSecTm TimeStamp;
    TStrV tmFields;
    // date processing code from loadShopNet
    TStr dateStr = fields[2].GetTrunc().GetSubStr(0,9);
    if (dateStr[4] == '-') // inconsistency in taobao data!!                                                                                               
      dateStr.SplitOnAllCh('-', tmFields, false);
    else
      dateStr.SplitOnAllCh('/', tmFields, false);
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
    //printf("%s\n", TimeStamp.GetStr().CStr());
    
    if (prodSeller.IsKey(ProductID)) { 
    } 
    else { 
      prodSeller.AddDat(ProductID, SellerID);
      if (prodBuyer.IsKey(ProductID)) {
        printf("no way\n");
      }
      else {
        prodBuyer.AddDat(ProductID, BuyerID);
        prodPrice.AddDat(ProductID, ProductPrice);
        prodShip.AddDat(ProductID, ShippingCost);
        prodTime.AddDat(ProductID, TimeStamp.GetAbsSecs());
      }
    } 
   
    if (!BSProd.IsKey(BS)) {
      BSProd.AddDat(BS, ProductID);
    }
  }
  
  fclose(in);
  //printf("negShipping %d %f %f\n", negShipping, minShipping, maxShipping);
  //return;

  // OCT: new input file
  FILE *in2 = fopen("/lfs/1/tmp/mengqiu/catStephenNew.txt", "r");
  int maxLines = 1023; // IGNORE all clusters with more than this
  char lines[maxLines+1][2048]; // max clust size is ~ 36000 
  TStr prevCluster = "";
  int maxCluster=0;
  int clustSize=0;
  int lineNum=0;
  int cnt=0;
  int skipPrice=0;
  int dataLen=0; // training data size
  int qID=1;
  int multiBuy=0;
  
  // for random baseline, false positive = false negative
  // many vars below are redundant or deprecated, just ignore
  float randTP = 0.0; 
  float randF = 0.0;
  float randMRRCnt = 0.0; // mean reciprocal rank
  float randMRRTot = 0.0;
  float randMRCnt = 0.0; // mean rank
  float randMRTot = 0.0;
  float randMRFRCnt = 0.0;
  float randMRFRTot = 0.0;
  float randMFRCnt = 0.0; 
  float randMFRTot = 0.0;

  float choosePriceTP = 0.0;
  float choosePriceF = 0.0;
  float choosePriceMRRCnt = 0.0;
  float choosePriceMRRTot = 0.0;
  float choosePriceMRCnt = 0.0;
  float choosePriceMRTot = 0.0;
  float choosePriceMRFRCnt = 0.0;
  float choosePriceMRFRTot = 0.0;
  float choosePriceMFRCnt = 0.0;
  float choosePriceMFRTot = 0.0;

  float chooseMsgTP = 0.0;
  float chooseMsgF = 0.0;
  float chooseMsgMRRCnt = 0.0;
  float chooseMsgMRRTot = 0.0;
  float chooseMsgMRCnt = 0.0;
  float chooseMsgMRTot = 0.0;
  float chooseMsgMRFRCnt = 0.0;
  float chooseMsgMRFRTot = 0.0;
  float chooseMsgMFRCnt = 0.0;
  float chooseMsgMFRTot = 0.0;

  // arrays for computing baselines for all categories
  float PrandTP[100];
  float PrandF[100];
  float PrandMRRCnt[100]; // mean reciprocal rank
  float PrandMRRTot[100];
  float PrandMRCnt[100]; // mean rank
  float PrandMRTot[100];
  float PrandMRFRCnt[100]; // mean reciprocal fractional rank
  float PrandMRFRTot[100];
  float PrandMFRCnt[100]; // mean fractional rank
  float PrandMFRTot[100];

  float PchoosePriceTP[100];
  float PchoosePriceF[100];
  float PchoosePriceMRRCnt[100];
  float PchoosePriceMRRTot[100];
  float PchoosePriceMRCnt[100];
  float PchoosePriceMRTot[100];
  float PchoosePriceMRFRCnt[100];
  float PchoosePriceMRFRTot[100];
  float PchoosePriceMFRCnt[100];
  float PchoosePriceMFRTot[100];

  float PchooseMsgTP[100];
  float PchooseMsgF[100];
  float PchooseMsgMRRCnt[100];
  float PchooseMsgMRRTot[100];
  float PchooseMsgMRCnt[100];
  float PchooseMsgMRTot[100];
  float PchooseMsgMRFRCnt[100];
  float PchooseMsgMRFRTot[100];
  float PchooseMsgMFRCnt[100];
  float PchooseMsgMFRTot[100];

  // arrays for computing baselines by cluster size

  float ArandTP[100];
  float ArandF[100];
  float ArandMRRCnt[100]; // mean reciprocal rank
  float ArandMRRTot[100];
  float ArandMRCnt[100]; // mean rank
  float ArandMRTot[100];
  float ArandMRFRCnt[100]; // mean reciprocal fractional rank
  float ArandMRFRTot[100];
  float ArandMFRCnt[100]; // mean fractional rank
  float ArandMFRTot[100];

  float AchoosePriceTP[100];
  float AchoosePriceF[100];
  float AchoosePriceMRRCnt[100];
  float AchoosePriceMRRTot[100];
  float AchoosePriceMRCnt[100];
  float AchoosePriceMRTot[100];
  float AchoosePriceMRFRCnt[100];
  float AchoosePriceMRFRTot[100];
  float AchoosePriceMFRCnt[100];
  float AchoosePriceMFRTot[100];

  float AchooseMsgTP[100];
  float AchooseMsgF[100];
  float AchooseMsgMRRCnt[100];
  float AchooseMsgMRRTot[100];
  float AchooseMsgMRCnt[100];
  float AchooseMsgMRTot[100];
  float AchooseMsgMRFRCnt[100];
  float AchooseMsgMRFRTot[100];
  float AchooseMsgMFRCnt[100];
  float AchooseMsgMFRTot[100];


  for (int i=0; i<100; i++) {
    PrandTP[i] = 0.0;
    PrandF[i] = 0.0;
    PrandMRRCnt[i] = 0.0; 
    PrandMRRTot[i] = 0.0;
    PrandMRCnt[i] = 0.0;
    PrandMRTot[i] = 0.0;
    PrandMRFRCnt[i] = 0.0;
    PrandMRFRTot[i] = 0.0;
    PrandMFRCnt[i] = 0.0;
    PrandMFRTot[i] = 0.0;

    ArandTP[i] = 0.0;
    ArandF[i] = 0.0;
    ArandMRRCnt[i] = 0.0;
    ArandMRRTot[i] = 0.0;
    ArandMRCnt[i] = 0.0;
    ArandMRTot[i] = 0.0;
    ArandMRFRCnt[i] = 0.0;
    ArandMRFRTot[i] = 0.0;
    ArandMFRCnt[i] = 0.0;
    ArandMFRTot[i] = 0.0;

    PchoosePriceTP[i] = 0.0;
    PchoosePriceF[i] = 0.0;
    PchoosePriceMRRCnt[i] = 0.0;
    PchoosePriceMRRTot[i] = 0.0;
    PchoosePriceMRCnt[i] = 0.0;
    PchoosePriceMRTot[i] = 0.0;
    PchoosePriceMRFRCnt[i] = 0.0;
    PchoosePriceMRFRTot[i] = 0.0;
    PchoosePriceMFRCnt[i] = 0.0;
    PchoosePriceMFRTot[i] = 0.0;

    AchoosePriceTP[i] = 0.0;
    AchoosePriceF[i] = 0.0;
    AchoosePriceMRRCnt[i] = 0.0;
    AchoosePriceMRRTot[i] = 0.0;
    AchoosePriceMRCnt[i] = 0.0;
    AchoosePriceMRTot[i] = 0.0;
    AchoosePriceMRFRCnt[i] = 0.0;
    AchoosePriceMRFRTot[i] = 0.0;
    AchoosePriceMFRCnt[i] = 0.0;
    AchoosePriceMFRTot[i] = 0.0;

    PchooseMsgTP[i] = 0.0;
    PchooseMsgF[i] = 0.0;
    PchooseMsgMRRCnt[i] = 0.0;
    PchooseMsgMRRTot[i] = 0.0;
    PchooseMsgMRCnt[i] = 0.0;
    PchooseMsgMRTot[i] = 0.0;
    PchooseMsgMRFRCnt[i] = 0.0;
    PchooseMsgMRFRTot[i] = 0.0;
    PchooseMsgMFRCnt[i] = 0.0;
    PchooseMsgMFRTot[i] = 0.0;
    
    AchooseMsgTP[i] = 0.0;
    AchooseMsgF[i] = 0.0;
    AchooseMsgMRRCnt[i] = 0.0;
    AchooseMsgMRRTot[i] = 0.0;
    AchooseMsgMRCnt[i] = 0.0;
    AchooseMsgMRTot[i] = 0.0;
    AchooseMsgMRFRCnt[i] = 0.0;
    AchooseMsgMRFRTot[i] = 0.0;
    AchooseMsgMFRCnt[i] = 0.0;
    AchooseMsgMFRTot[i] = 0.0;

  }
  
  int conPos=0, conNeg=0;
  int msgPos=0, msgNeg=0;
 
  //FILE *subgraphs = fopen("subgraph.out", "w");
  // if a cluster has b sellers, generate b different versions of the subgraph 
  
  // toggle for speedup
  int subgraphOn = 0; // change to 1 to print all subgraphs

  while (!feof(in2)) {
    memset(lines[lineNum], 0, 2048);
    fgets(lines[lineNum], 2048, in2);
    if (strlen(lines[lineNum]) == 0)
      break; // todo process last cluster
    
    TStr(lines[lineNum]).SplitOnAllCh('\t', fields, false);
    if (lineNum!=maxLines)
      lineNum++;
    else {
      // already at limit, ignore this cluster
    }
    //if (lineNum==maxLines) // just ignore this cluster
    //  lineNum--;
    
    TStr cluster = fields[3].GetTrunc();
    
    if (cluster != prevCluster && prevCluster!="") {  
      if (lineNum!=maxLines) {
        if (clustSize>maxCluster)
          maxCluster = clustSize;
        //if (clustSize>100)
        //  printf("%d ", clustSize);
      }
      else {
        //printf("%d ", clustSize);
        //ignore cluster
      }
      clustSize=1;
      
      /*
         // cluster is from 0 to lineNum-1
         if (cnt==1) {
         printf("%d\n", lineNum);
         for (int i=0; i<lineNum-1; i++)
         printf("%s", lines[i]);
       }
      */
      
      if (lineNum>1 && lineNum != maxLines) { // added latter maxLines
        TIntH clustBuyer;
        TIntH clustSeller;
        TIntIntVH buyerSeller; // correct sellers for each buyer
        // how many cases are there a buyer buys from multiple sellers - 0.05%
        TIntFltVH sellerPrices; // store list of prices offered by each seller
        TIntFltH sellerMed; // computed from above
        TIntFltH sellerRating; 
        TIntFltVH buyerPrices; // store list of exact prices used by each buyer
        // new, these are product specific mostly
        // for now just use use one num per seller, only inv likely to vary
        TIntH sellerDR; // eh just use one inventory num
        TIntFltH sellerProtection; // just use one num
        TIntFltH sellerPic;
        TIntH sellerCredit; // seller history <- most important new var
        TIntFltH sellerDesc; // 3 detailed rating nums
        TIntFltH sellerServ;
        TIntFltH sellerSpeed; 
          

        TStr category;
        // do one-time cluster calculations (preprocessing, file output)
        for (int i=0; i<lineNum-1; i++) { // price/rating computation
          TStr(lines[i]).SplitOnAllCh('\t', fields, false);
          TFlt ProductPrice = TFlt(atof(fields[1].GetTrunc().CStr()));
          TFlt rating = TFlt(atof(fields[7].GetTrunc().CStr()));
          TInt dealRecord = TInt(atoi(fields[9].GetTrunc().CStr()));          
          TFlt protection = TFlt(atof(fields[10].GetTrunc().CStr()));
          TFlt hasPic = TFlt(atof(fields[11].GetTrunc().CStr()));
          TInt credit = TInt(atoi(fields[12].GetTrunc().CStr()));
          TFlt desc = TFlt(atof(fields[13].GetTrunc().CStr()));
          TFlt serv = TFlt(atof(fields[14].GetTrunc().CStr()));
          TFlt speed = TFlt(atof(fields[15].GetTrunc().CStr()));
          // note that rating = -1 means no rating info
          // same for desc, serv, speed

          TFlt transPrice = prodPrice.GetDat(fields[0]);
          TFlt transShip = prodShip.GetDat(fields[0]);
          if (ProductPrice > transPrice*2.0 || transPrice>ProductPrice*2.0) {
            skipPrice++; // skip this line
            continue;
          }
          
          // category is always the same in each cluster
          category = fields[6].GetTrunc().CStr(); 

          TInt buyerID = prodBuyer.GetDat(fields[0]);
          TInt sellerID = prodSeller.GetDat(fields[0]);
          // just added price check
          if (buyerPrices.IsKey(buyerID)) {
            TFltV prices = buyerPrices.GetDat(buyerID);
            prices.Add(ProductPrice+transShip);
            buyerPrices.AddDat(buyerID, prices);
          }
          else {
            TFltV prices;
            prices.Add(ProductPrice);
            buyerPrices.AddDat(buyerID, prices);
          }

          if (buyerSeller.IsKey(buyerID)) {
            TIntV sellers = buyerSeller.GetDat(buyerID);
            sellers.Add(sellerID); // no buy-sell dups?
            buyerSeller.AddDat(buyerID, sellers);
          }
          else {
            TIntV sellers;
            sellers.Add(sellerID);
            buyerSeller.AddDat(buyerID, sellers);
          }
        
          if (!sellerRating.IsKey(sellerID))
            sellerRating.AddDat(sellerID, rating);
          
          // OCT NEW: use prod price + shipping
          if (sellerPrices.IsKey(sellerID)) {
            TFltV prices = sellerPrices.GetDat(sellerID);
            prices.Add(ProductPrice);
            sellerPrices.AddDat(sellerID, prices);
          }
          else {
            TFltV prices;
            prices.Add(ProductPrice);
            sellerPrices.AddDat(sellerID, prices);
          }
          
          if (!clustBuyer.IsKey(buyerID))
            clustBuyer.AddDat(buyerID);
          if (!clustSeller.IsKey(sellerID))
            clustSeller.AddDat(sellerID);
        } // close for int i
        
        // check if catMap does not contain a category
        int catIndex = 0; // hack
        /*
        if (catMap.IsKey(category))
          catIndex = catMap.GetDat(category);
        
        else {
          printf("missing category START%sEND\n", category.CStr());
          return;
        }
        */
      
        // consider cases with 2-10 sellers. eg throw out 10-15% of the data 
        // too many sellers = cluster is most likely wrong
        if (clustBuyer.Len()>0 && (clustSeller.Len()>1 &&
                                   clustSeller.Len()<11)) {
                    
          dataLen += (clustBuyer.Len() * clustSeller.Len());
          //fprintf(outs, "%d %d\n", clustBuyer.Len(), clustSeller.Len());
          
          buyerSum += clustBuyer.Len();
          sellerSum += clustSeller.Len();

          // sort prices for each seller 
          for (int i=0; i<clustSeller.Len(); i++) {
            int sID = clustSeller.GetKey(i);
            TFltV prices = sellerPrices.GetDat(sID);
            prices.Sort();
            
            //printf("sellerRating %f\n", (float)sellerRating.GetDat(sID));
            
            float mid; // use median price per seller
            if (prices.Len()%2==1)
              mid = prices[prices.Len()/2];
            else 
              mid = (prices[prices.Len()/2]+prices[prices.Len()/2-1])/2.0;
            sellerMed.AddDat(sID, mid);
             //printf("sellerMed %d %f\n", sID, mid);
          }
          
          sellerMed.SortByDat(true); // ascending price
          float clustMed; 
          if (sellerMed.Len()%2==1)
            clustMed =  sellerMed[sellerMed.Len()/2];
          else
            clustMed = (sellerMed[sellerMed.Len()/2]+ sellerMed[sellerMed.Len()/2-1])/2.0;
          //printf("clustMed %f\n", clustMed);
          
          sellerRating.SortByDat(false); // descending rating
          float medRating;
          if (sellerRating.Len()%2==1)
            medRating = sellerRating[sellerRating.Len()/2];
          else
            medRating = (sellerRating[sellerRating.Len()/2]+sellerRating[sellerRating.Len()/2-1])/2.0; 
          if (medRating < 0.0)  
            medRating = 0.0;
          //printf("clustRating %f\n", medRating);
          
 
          // INSERT code: compare buyer actual and sellerMed to clusterMed
          // use exact seller prices, rather than median, for global over/under
          for (int i=0; i<clustSeller.Len(); i++) {
            int sID = clustSeller.GetKey(i);

            if (sellerCats.IsKey(sID)) {
              TStrV cats = sellerCats.GetDat(sID);
              int catFnd=0;
              for (int j=0; j<cats.Len(); j++)
                if (cats[j] == category) {
                  catFnd = 1;
                  break;
                }
              if (catFnd==0) {
                cats.Add(category);
                sellerCats.AddDat(sID, cats);
              }
            }
            else {
              TStrV cats;
              cats.Add(category);
              sellerCats.AddDat(sID, cats);
            }
            
            if (sellerNumClusters.IsKey(sID)) {
              int existCnt = sellerNumClusters.GetDat(sID);
              sellerNumClusters.AddDat(sID, existCnt+1);
            }
            else {
              sellerNumClusters.AddDat(sID, 1);
            }

            TFltV prices = sellerPrices.GetDat(sID);
            for (int j=0; j<prices.Len(); j++) {
              float sDif = prices[j]-clustMed;
              if (!sellerRelPriceTot.IsKey(sID)) {
                sellerRelPriceTot.AddDat(sID, sDif);
                sellerRelPriceCnt.AddDat(sID, 1);
              }
              else {
                float existTot = sellerRelPriceTot.GetDat(sID);
                sellerRelPriceTot.AddDat(sID, existTot+sDif);
                int existCnt = sellerRelPriceCnt.GetDat(sID);
                sellerRelPriceCnt.AddDat(sID, existCnt+1);
              }
            }
          }

          // buyer block symmetric with seller block
          for (int i=0; i<clustBuyer.Len(); i++) {
            int bID = clustBuyer.GetKey(i);

            if (buyerCats.IsKey(bID)) {
              TStrV cats = buyerCats.GetDat(bID);
              int catFnd=0;
              for (int j=0; j<cats.Len(); j++)
                if (cats[j] == category) {
                  catFnd = 1;
                  break;
                }
              if (catFnd==0) {
                cats.Add(category);
                buyerCats.AddDat(bID, cats);
              }
            }
            else {
              TStrV cats;
              cats.Add(category);
              buyerCats.AddDat(bID, cats);
            }


            if (buyerNumClusters.IsKey(bID)) {
              int existCnt = buyerNumClusters.GetDat(bID);
              buyerNumClusters.AddDat(bID, existCnt+1);
            }
            else {
              buyerNumClusters.AddDat(bID, 1);
            }

            TFltV prices = buyerPrices.GetDat(bID); // usually only length 1
            for (int j=0; j<prices.Len(); j++) {
              float bDif = prices[j]-clustMed;
              if (!buyerRelPriceTot.IsKey(bID)) {
                buyerRelPriceTot.AddDat(bID, bDif);
                buyerRelPriceCnt.AddDat(bID, 1);
              }
              else {
                float existTot = buyerRelPriceTot.GetDat(bID);
                buyerRelPriceTot.AddDat(bID, existTot+bDif);
                int existCnt = buyerRelPriceCnt.GetDat(bID);
                buyerRelPriceCnt.AddDat(bID, existCnt+1);
              }
            }
          }
          
          // BEGIN GIANT TRIPLE FOR LOOP
          for (int i=0; i<clustBuyer.Len(); i++) { // each buyer is a block
            //int isMatch=0;
            int bID = clustBuyer.GetKey(i);
            TIntV posSell = buyerSeller.GetDat(bID);
            // for each buyer, find transaction time for its purchase in the
            // dont worry about multiple purchases per buyer, almost never happens
            TIntPr pr = TIntPr(bID, posSell[0]);
            TStr refProd = BSProd.GetDat(pr); 
            int refTime; // we only consider prior to this date 
            if (prodTime.IsKey(refProd))
              refTime = prodTime.GetDat(refProd);
            else
              printf("missing prod\n");
            // new features: edge age/decay ala lars paper
            // older edge = harder to predict
            int edgeAge = (endSec-refTime)/daySec; 
            float edgeTime1 = pow((float)edgeAge, td1);
            float edgeTime2 = pow((float)edgeAge, td2);
            float edgeTime3 = pow((float)edgeAge, td3);
            
            //fprintf(subgraphs, "refNode %d\n", bID);
            // we will print the b x s graph once per buyer 

            // insert another giant for loop -> only fprintf if z = b
            for (int z=0; z<clustBuyer.Len(); z++) {
              // ALL REFERENCES to bID should be replaced with zID in this for loop 
              int zID = clustBuyer.GetKey(z);
              TIntH zConNeighbors; // new, neighboring nodes given timestamp
              TIntH zMsgNeighbors;
              float zConDeg = 0.0;
              float zMsgDeg = 0.0;
              float zConCCF = 0.0;
              float zMsgCCF = 0.0;
              
              if (subgraphOn==0 && zID!=bID) // skip most
                continue;
              
              float zFwdPR = 0.0;
              float zMsgPR = 0.0;
              float zConPR = 0.0;

              // we're just going to be dumb and calculate inverted msg rank
              // dont want to restructure code
              TIntFltH sellMsg; // msg vol from buyer to each seller 

              for (int j=0; j<clustSeller.Len(); j++) {
                int sID = clustSeller.GetKey(j);
                float tmsgVol = 0.0;

                if (m->IsNode(zID) && m->IsNode(sID)) {
                  if (m->IsEdge(zID, sID)) {
                    TTaobaoShopEdge& edge = m->GetEDat(zID, sID);
                    int l=0;
                    while (l < edge.CntTX()) {
                      int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                      if (tDif <= 0) // was counting same day with <
                        break;
                      if (tDif <= msgWindow)
                        tmsgVol += (float)edge.GetStrength(l);
                      l++;
                    }
                  }
                
                  if (m->IsEdge(sID, zID)) {
                    TTaobaoShopEdge& edge = m->GetEDat(sID, zID);
                    int l=0;
                    while (l < edge.CntTX()) {
                      int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                      if (tDif <= 0) // was counting same day with <
                        break;
                      if (tDif <= msgWindow)
                        tmsgVol += (float)edge.GetStrength(l);
                      l++;
                    }
                  }
                }

                sellMsg.AddDat(sID, tmsgVol);
              }
              sellMsg.SortByDat(false);
              // use sellMsg to output inverted msg rank
              // DNE = 0

              for (int j=0; j<clustSeller.Len(); j++) {
                // NOTE: we handle case where buyer buys from multiple sellers
                int sID = clustSeller.GetKey(j);
                int isMatch = 0;
                int mtch=0;
                int justMtch=0;
                // match only matters when zId = bId
                for (int k=0; k<posSell.Len(); k++)
                  if (posSell[k]==sID) {
                    mtch=1;
                    isMatch++;
                    justMtch=1;
                    break;
                  }
              
                float purchasePrice;
              //if (justMtch==1) { // FOR NOW, use same price dif for all buyers
                // per seller. can change later if nee 
              purchasePrice = sellerMed.GetDat(sID);
              //}
              //else { // use median seller price if buyer did not directly interact 
              //purchasePrice = sellerMed.GetDat(sID);
              // }
                            
              } // CLOSE SELLER ITERATION LOOP
            
              //if (isMatch==0)
              // printf("error no purchase\n");
              // else if (isMatch>1) { // multipurchase
              //printf("%d %d\n", qID, bID);
              //}
           
              if (zID == bID) {
                // HARD CODED BASELINE SIMULATION
                // TP, F is used to compute precision@1 metrics
                // also compute mean reciprocal rank
                
                if (qID%4 == 0) {  
                  // use corSell.Len() as number of correct sellers for this buyer
                  TIntV corSell = buyerSeller.GetDat(bID);
                  // harded coded baselines will always choose 1 seller per cluster
                  
                  int clustSz = clustSeller.Len();
                  
                  TIntV randV;
                  // new: do actual randomization to calculate p@1 and mrr
                  for (int l=0; l<corSell.Len(); l++)
                    randV.Add(2);
                  for (int l=0; l<clustSeller.Len()-corSell.Len(); l++)
                    randV.Add(1);
                  randV.Shuffle(TInt::Rnd);
                  
                  if (randV[0]==2) {
                    randTP += 1.0;
                    PrandTP[catIndex] += 1.0;
                    ArandTP[clustSz] += 1.0; 
                  }
                  else {
                    randF += 1.0;
                    PrandF[catIndex] += 1.0;
                    ArandF[clustSz] += 1.0;
                  }

                  for (int l=0; l<randV.Len(); l++)
                    if (randV[l] == 2) {
                      randMRRCnt += 1.0;
                      PrandMRRCnt[catIndex] += 1.0;
                      ArandMRRCnt[clustSz] += 1.0;

                      randMRRTot += 1.0/(float)(1.0+l);
                      PrandMRRTot[catIndex] += 1.0/(float)(1.0+l);
                      ArandMRRTot[clustSz] += 1.0/(float)(1.0+l);

                      randMRCnt += 1.0;
                      PrandMRCnt[catIndex] += 1.0;
                      ArandMRCnt[clustSz] += 1.0;

                      randMRTot += (float)(1.0+l);
                      PrandMRTot[catIndex] += (float)(1.0+l);
                      ArandMRTot[clustSz] += (float)(1.0+l);

                      float frac = (float)(1.0+l)/(float)clustSeller.Len();
                      randMRFRTot += 1.0/frac;
                      PrandMRFRTot[catIndex] += 1.0/frac;
                      ArandMRFRTot[clustSz] += 1.0/frac;

                      randMFRTot += frac;
                      PrandMFRTot[catIndex] += frac;
                      ArandMFRTot[clustSz] += frac;
                    }
                  randV.Clr();

                  /*
                  // random baseline: assume that method chooses 1 seller  
                  if (rand()%clustSeller.Len() < corSell.Len())
                    randTP += 1.0;
                  else { // wrong guess
                    randF += 1.0;
                  }
                  */
                      
                  // for simplicity, assume algorithm chooses one if tied
                  int good = 0;
                  for (int l=0; l<corSell.Len(); l++) {
                    if (corSell[l] == sellerMed.GetKey(0)) 
                      good=1; // p@1
                    for (int l2=0; l2<sellerMed.Len(); l2++)
                      if (corSell[l] == sellerMed.GetKey(l2)) {
                        choosePriceMRRCnt += 1.0;
                        PchoosePriceMRRCnt[catIndex] += 1.0;
                        AchoosePriceMRRCnt[clustSz] += 1.0;
                        
                        choosePriceMRRTot += 1.0/(float)(1.0+l2);
                        PchoosePriceMRRTot[catIndex] += 1.0/(float)(1.0+l2);
                        AchoosePriceMRRTot[clustSz] += 1.0/(float)(1.0+l2);

                        choosePriceMRCnt += 1.0;
                        PchoosePriceMRCnt[catIndex] += 1.0;
                        AchoosePriceMRCnt[clustSz] += 1.0;

                        choosePriceMRTot += (float)(1.0+l2);
                        PchoosePriceMRTot[catIndex] += (float)(1.0+l2);
                        AchoosePriceMRTot[clustSz] += (float)(1.0+l2);

                        float frac = (float)(1.0+l2)/(float)clustSeller.Len();
                        choosePriceMRFRTot += 1.0/frac;
                        PchoosePriceMRFRTot[catIndex] += 1.0/frac;
                        AchoosePriceMRFRTot[clustSz] += 1.0/frac;

                        choosePriceMFRTot += frac;
                        PchoosePriceMFRTot[catIndex] += frac;
                        AchoosePriceMFRTot[clustSz] += frac;

                        break;
                      }
                  }
                  if (good == 1) {
                    choosePriceTP += 1.0;
                    PchoosePriceTP[catIndex] += 1.0;
                    AchoosePriceTP[clustSz] += 1.0;
                  }
                  else {
                    choosePriceF += 1.0;
                    PchoosePriceF[catIndex] += 1.0;
                    AchoosePriceF[clustSz] += 1.0;
                  }

                  // choose min baseline: assume that method 
                  //chooses all sellers with lowest price
                  /*
                    float good=0.0;
                    for (int l=0; l<corSell.Len(); l++)
                    if (sellerMed.GetDat(corSell[l]) == clustPrices[0])
                    good += 1.0;
                    chooseMinTotal += good/(float)corSell.Len();
                    chooseMinCnt += 1.0;
                  */
                  
                  
                  if (sellMsg.Len() > 0) {
                    //sellMsg.SortByDat(false);
                    good = 0;
                    for (int l=0; l<corSell.Len(); l++) {
                      if (corSell[l] == sellMsg.GetKey(0))
                        good = 1;
                      int mrrFnd=0;
                      for (int l2=0; l2<sellMsg.Len(); l2++)
                        if (corSell[l] == sellMsg.GetKey(l2)) {
                          chooseMsgMRRCnt += 1.0;
                          PchooseMsgMRRCnt[catIndex] += 1.0;
                          AchooseMsgMRRCnt[clustSz] += 1.0;

                          chooseMsgMRRTot += 1.0/(float)(1.0+l2);
                          PchooseMsgMRRTot[catIndex] += 1.0/(float)(1.0+l2);
                          AchooseMsgMRRTot[clustSz] += 1.0/(float)(1.0+l2);

                          chooseMsgMRCnt += 1.0;
                          PchooseMsgMRCnt[catIndex] += 1.0;
                          AchooseMsgMRCnt[clustSz] += 1.0;

                          chooseMsgMRTot += (float)(1.0+l2);
                          PchooseMsgMRTot[catIndex] += (float)(1.0+l2);
                          AchooseMsgMRTot[clustSz] += (float)(1.0+l2);

                          float frac = (float)(1.0+l2)/(float)clustSeller.Len();
                          chooseMsgMRFRTot += 1.0/frac;
                          PchooseMsgMRFRTot[catIndex] += 1.0/frac;
                          AchooseMsgMRFRTot[clustSz] += 1.0/frac;
                          
                          chooseMsgMFRTot += frac;
                          PchooseMsgMFRTot[catIndex] += frac;
                          AchooseMsgMFRTot[clustSz] += frac;

                          mrrFnd = 1;
                          break;
                        }
                      if (mrrFnd == 0) {
                        // manually compute expected rank
                        // dont want to bother with random simulation
                        int extra = (clustSeller.Len()-sellMsg.Len());
                        int firstExtra = sellMsg.Len()+1;
                        int lastExtra = firstExtra+extra-1;
                        float expectedRank = ((float)firstExtra+(float)lastExtra)/2.0;
                        
                        chooseMsgMRRCnt += 1.0;
                        PchooseMsgMRRCnt[catIndex] += 1.0;
                        AchooseMsgMRRCnt[clustSz] += 1.0;
                        
                        chooseMsgMRRTot += 1.0/expectedRank;
                        PchooseMsgMRRTot[catIndex] += 1.0/expectedRank;
                        AchooseMsgMRRTot[clustSz] += 1.0/expectedRank;
                        
                        chooseMsgMRCnt += 1.0;
                        PchooseMsgMRCnt[catIndex] += 1.0;
                        AchooseMsgMRCnt[clustSz] += 1.0;

                        chooseMsgMRTot += expectedRank;
                        PchooseMsgMRTot[catIndex] += expectedRank;
                        AchooseMsgMRTot[clustSz] += expectedRank;

                        float frac = expectedRank/(float)clustSeller.Len();
                        chooseMsgMRFRTot += 1.0/frac;
                        PchooseMsgMRFRTot[catIndex] += 1.0/frac;
                        AchooseMsgMRFRTot[clustSz] += 1.0/frac;

                        chooseMsgMFRTot += frac;
                        PchooseMsgMFRTot[catIndex] += frac;
                        AchooseMsgMFRTot[clustSz] += frac;
                      }
                    }

                    //chooseMsgTotal += 1.0/(float)corSell.Len();
                    if (good == 1) {
                      chooseMsgTP += 1.0;
                      PchooseMsgTP[catIndex] += 1.0;
                      AchooseMsgTP[clustSz] += 1.0;
                    }
                    else {
                      chooseMsgF += 1.0;
                      PchooseMsgF[catIndex] += 1.0;
                      AchooseMsgF[clustSz] += 1.0;
                    }
                  }
                  else { // if no msg info, default to random guess
              
                    for (int l=0; l<corSell.Len(); l++)
                      randV.Add(2);
                    for (int l=0; l<clustSeller.Len()-corSell.Len(); l++)
                      randV.Add(1);
                    randV.Shuffle(TInt::Rnd);

                    if (randV[0]==2) {
                      chooseMsgTP += 1.0;
                      PchooseMsgTP[catIndex] += 1.0;
                      AchooseMsgTP[clustSz] += 1.0;
                    }
                    else {
                      chooseMsgF += 1.0;
                      PchooseMsgF[catIndex] += 1.0;
                      AchooseMsgF[clustSz] += 1.0;
                    }

                    for (int l=0; l<randV.Len(); l++)
                      if (randV[l] == 2) {
                        chooseMsgMRRCnt += 1.0;
                        PchooseMsgMRRCnt[catIndex] += 1.0;
                        AchooseMsgMRRCnt[clustSz] += 1.0;
                        
                        chooseMsgMRRTot += 1.0/(float)(1.0+l);
                        PchooseMsgMRRTot[catIndex] += 1.0/(float)(1.0+l);
                        AchooseMsgMRRTot[clustSz] += 1.0/(float)(1.0+l);

                        chooseMsgMRCnt += 1.0;
                        PchooseMsgMRCnt[catIndex] += 1.0;
                        AchooseMsgMRCnt[clustSz] += 1.0;

                        chooseMsgMRTot += (float)(1.0+l);
                        PchooseMsgMRTot[catIndex] += (float)(1.0+l);
                        AchooseMsgMRTot[clustSz] += (float)(1.0+l);
                        
                        float frac = (float)(1.0+l)/(float)clustSeller.Len();
                        chooseMsgMRFRTot += 1.0/frac;
                        PchooseMsgMRFRTot[catIndex] += 1.0/frac;
                        AchooseMsgMRFRTot[clustSz] += 1.0/frac;

                        chooseMsgMFRTot += frac;
                        PchooseMsgMFRTot[catIndex] += frac;
                        AchooseMsgMFRTot[clustSz] += frac;
                      }
                    randV.Clr();


                    // if (rand()%clustSeller.Len() < corSell.Len())
                    //  chooseMsgTP += 1.0;
                    //else 
                    //  chooseMsgF += 1.0;
                  }
                  //chooseMsgCnt += 1.0;
                }
                
                qID++;
              } // close zId == bId
              
            } // close giant z loop iteration
          } // CLOSE OUTER LOOP for buyer-seller feature output
          
          cnt++;
          //if (cnt==10)
          //  return; // DEBUG, first processed cluster 
        }
        
      }
      
      // copy last line to beginning
      strcpy(lines[0], lines[lineNum-1]);
      lineNum = 1;
      
    }
    else {
      clustSize++;
    }
     prevCluster = cluster;
     
  }  
  
  printf("maxClustSize %d\n", maxCluster);
  printf("priceskip %d\n", skipPrice);
  printf("training data len %d\n", dataLen);
  printf("clusters used %d\n", cnt);
  printf("perCluster avgBuyers %f avgSellers %f\n", (float)buyerSum/(float)cnt, (float)sellerSum/(float)cnt);
  //printf("multibuy %d\n", multiBuy);

  FILE *outRand = fopen("randBaseline.out", "w");
  printf("random baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
         randTP/(randTP+randF), randMRRTot/randMRRCnt, randMRTot/randMRCnt,
  randMRFRTot/randMRRCnt, randMFRTot/randMRCnt);
  for (int i=2; i<11; i++) 
    fprintf(outRand, "%d %f %f %f %f %f\n", 
            i, ArandTP[i]/(ArandTP[i]+ArandF[i]),
            ArandMRRTot[i]/ArandMRRCnt[i], ArandMRTot[i]/ArandMRCnt[i],
            ArandMRFRTot[i]/ArandMRRCnt[i], ArandMFRTot[i]/ArandMRCnt[i]);
  fclose(outRand);

  FILE *outPrice = fopen("choosePriceBaseline.out", "w");
  printf("chooseMinPrice baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
         choosePriceTP/(choosePriceTP+choosePriceF),
         choosePriceMRRTot/choosePriceMRRCnt,
         choosePriceMRTot/choosePriceMRCnt,
  choosePriceMRFRTot/choosePriceMRRCnt, choosePriceMFRTot/choosePriceMRCnt); 
  for (int i=2; i<11; i++)
    fprintf(outPrice, "%d %f %f %f %f %f\n", i,
            AchoosePriceTP[i]/(AchoosePriceTP[i]+AchoosePriceF[i]),
            AchoosePriceMRRTot[i]/AchoosePriceMRRCnt[i],
            AchoosePriceMRTot[i]/AchoosePriceMRCnt[i],
            AchoosePriceMRFRTot[i]/AchoosePriceMRRCnt[i], AchoosePriceMFRTot[i]/AchoosePriceMRCnt[i]);
  fclose(outPrice);

  FILE *outMsg = fopen("chooseMsgBaseline.out", "w");
  printf("chooseMostMsg baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
         chooseMsgTP/(chooseMsgTP+chooseMsgF), chooseMsgMRRTot/chooseMsgMRRCnt,
         chooseMsgMRTot/chooseMsgMRCnt, chooseMsgMRFRTot/chooseMsgMRRCnt, chooseMsgMFRTot/chooseMsgMRCnt);
  for (int i=2; i<11; i++)
    fprintf(outMsg, "%d %f %f %f %f %f\n", i,
            AchooseMsgTP[i]/(AchooseMsgTP[i]+AchooseMsgF[i]),
            AchooseMsgMRRTot[i]/AchooseMsgMRRCnt[i],
            AchooseMsgMRTot[i]/AchooseMsgMRCnt[i],
            AchooseMsgMRFRTot[i]/AchooseMsgMRRCnt[i],
            AchooseMsgMFRTot[i]/AchooseMsgMRCnt[i]);
  fclose(outMsg);

  fclose(in2);
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
    fprintf(stderr, "Usage:triangleTesting file1 jobType file2 jobType file3 jobType\n");
    exit(-1);
  } 
  file1 = TStr(argv[1]);
  job1 = TStr(argv[2]);
  file2 = TStr(argv[3]);
  job2 = TStr(argv[4]);
  file3 = TStr(argv[5]);
  job3 = TStr(argv[6]);
  
  //plotTriangle();
  //compareTriFiles();
  //plotDeg();
  //plotMsgPrice();
  //plotTP();
  //plotMT();
  
  //updatePred(t); //new, here for now
  //catAnalysis(t);

  //t->LoadProduct("//lfs//1//tmp//mengqiu//cat_brand_0.1.txt");
  //return 0;

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

  //updatePred(t);
  //catAnalysis(t);
  //clusterTest(t);
  //return 0;

  // loading of third datset should be optional
  
  if (job3 == "fwd")
    t->LoadShopNet(file3);
  else if (job3 == "msg")
    t->LoadChatNet(file3, true);
  else if (job3 == "contact")
    t->LoadChatNet(file3, false);

  //t->LoadRating("//lfs//1//tmp//mengqiu//seller_rating.txt");
  
  printf("Loading complete\n");

  updatePred(t); // load all 3
  return 0;

}
