#include "stdafx.h"
#include "taobaonet.h"
#include "graphing.h"
#include <stdio.h>
#include <string.h> 
#include <math.h>

// parallelize prediction generation, send all ouput into preds dir  

// for each product id, suggest K other most similar products
// should be in same category, cosine attributes

// for each cat file, find suggestions for each product (if possible)
// there seem to be duplicate prod listings with the same prodID, sellerID
// just dont store second listing, also ignore emtpy attr

typedef struct product {
  TStr prodID;
  TStr sellerID;
  TStr catID; //new 
  TFlt ratio;
  //TStr ratio;
  TStr title;
  TFlt price;
  TStrV atr;

  // new: oct3
  TInt dealRecord; // product inventory already sold?
  TFlt protection; // bool "TRUE"
  TFlt hasPic; // bool
  TInt credit; // seller history
  TFlt desc; // detailed rating
  TFlt serv;
  TFlt speed;

} PRODUCT;

PRODUCT products[2000000]; // nonDup prods with attributes
int numProducts;

// for cosine, use tfidf. tf should be 1
// split attr into field/value?
// set of all prods who share at least one attribute with query is toolarge

/*
1. construct attribute to list of product map
for speed, dont need to bother with attributes that appear in many products
this will require 2 pass 
for each query, go through each of its attributes.
compute running tfidf score for all wrt it
 */

void loadCat() {

  // block 0 -...
  // block 34-82
  // block 20-33

  for (int c=26; c<27; c++) { // 82 categories
    if (c==5||c==20||c==40||c==71)
      continue; // 4 cats are bad crawls
    TStr catFile = TStr::Fmt("/lfs/1/tmp/mengqiu/cats/%d.txt", c);
    TStr predFile = TStr::Fmt("/lfs/1/tmp/mengqiu/preds/%d.txt", c);
    FILE *file = fopen(catFile.CStr(), "r");
    FILE *fout = fopen(predFile.CStr(), "w");
    char info[22][2024]; // 22 lines per product

    numProducts=0; // reset for each cat
    TStrH prodH; // hash of prod.sell Key to product number
    TStrH atrH; // simple hashCnt over all attrs
    int repeat = 0;
    int noAtr=0;
    int shareCnt=0;
    // read in file, store all necessary prod info
    // insert into attribute to product hash
    //TStrStrVH atrProdH; // TStrStrVH, map attr to list of prods
    TStrV atrProdH[100000]; // cat 5 has 95k attributes
    TStrH atrMap; // map of attribute to index num for atrProdH
    int numAtr=0;

    /*
      this data struct is too slow
      can convert to
      array of tstrv
      external hash map of attr to array index
     */
   
    while (!feof(file)) {
      for (int i=0; i<22; i++) { // 21 = newline
        memset(info[i], 0, 2024);
        fgets(info[i], 2024, file);
        info[i][strlen(info[i])-1] = 0; // remove newline
      }

      // just prodID will still have repeats
      // originally had "." 
      TStr prodKey = TStr::Fmt("%s %s", TStr(info[0]).GetTrunc().CStr(), TStr(info[10]).GetTrunc().CStr());

      if (prodH.IsKey(prodKey)) {
        //printf("%s", info[0]);
        //return;
        repeat++;
      }
      else {
        products[numProducts].prodID = TStr(info[0]).GetTrunc();
        products[numProducts].sellerID = TStr(info[10]).GetTrunc();
        products[numProducts].ratio = TFlt(atof(TStr(info[12]).GetTrunc().CStr()));
        products[numProducts].title = TStr(info[3]).GetTrunc();
        products[numProducts].price = TFlt(atof(TStr(info[4]).GetTrunc().CStr()));

        //printf("%s", products[numProducts].title.CStr());
        //return;
         
        TStr atrLine = TStr(info[9]).GetTrunc();
        if (atrLine.Len() > 0) { // ignore empty attr?
          atrLine.SplitOnAllCh('\t', products[numProducts].atr,false);
          
          //if (products[numProducts].atr.Len()==1) {
          //  printf("attr: %d\n", products[numProducts].atr[0].Len());
          //  return;
          //}
          
          for (int i=0; i<products[numProducts].atr.Len(); i++) {
            if (atrH.IsKey(products[numProducts].atr[i])) {
              int existCnt = atrH.GetDat(products[numProducts].atr[i]);
              atrH.AddDat(products[numProducts].atr[i], existCnt+1);
              shareCnt++;

              int atrInd = atrMap.GetDat(products[numProducts].atr[i]);
              // changed prodID to prodKey
              atrProdH[atrInd].Add(prodKey);
              // assume each atr only adds a product once
            }
            else {
              atrH.AddDat(products[numProducts].atr[i], 1);
              atrMap.AddDat(products[numProducts].atr[i], numAtr);
              atrProdH[numAtr].Add(prodKey);
              numAtr++;
            }

            // update map of attributes to products
            // too slow, may need 2pass, and hard ignore common attr
            /*
            TStrV prodList;
            if (atrProdH.IsKey(products[numProducts].atr[i])) 
              prodList = atrProdH.GetDat(products[numProducts].atr[i]);
            prodList.Add(products[numProducts].atr[i]);
            atrProdH.AddDat(products[numProducts].atr[i], prodList);
            */
          }
          
          prodH.AddDat(prodKey, numProducts);
          numProducts++;
          //if (numProducts%100 == 0)
          //printf("%d\t", numProducts);
        }
        else
          noAtr++;
      }
    }
    
    fclose(file);
    // for sanity check, find most common atr.. something wrong
    //atrH.SortByDat(false); // sort attributes in descending order, not really needed 
    //for (int k=0; k<200; k++)
    //  printf("%s %d\n", atrH.GetKey(k).CStr(), (int) atrH[k]);
    
    //printf("%s ", atrH.GetKey(0).CStr());
    int atrInd = atrMap.GetDat(atrH.GetKey(0));
    //printf("%d\n", atrProdH[atrInd].Len());
    //for (int k=0; k<20; k++)
    //  printf("%s\t", (atrProdH[atrInd])[k].CStr());
    //printf("\n");
    fprintf(fout, "%d %d %d %d %d %d\n", c, numProducts, repeat, noAtr, shareCnt, atrH.Len());

    // for each product, return top k similar products 
    for (int i=0; i<numProducts; i++) {
      TStrH matchCnt; // track match cnts of top hits
      for (int j=0; j<products[i].atr.Len(); j++) {
        int atrInd = atrMap.GetDat(products[i].atr[j]);
        for (int k=0; k<atrProdH[atrInd].Len(); k++) {
          if (matchCnt.IsKey((atrProdH[atrInd])[k])) {
            int existCnt = matchCnt.GetDat((atrProdH[atrInd])[k]);
            matchCnt.AddDat((atrProdH[atrInd])[k], existCnt+1);
          }
          else {
            matchCnt.AddDat((atrProdH[atrInd])[k], 1);
          }
        }
      }

      // sort may be too slow 
      //matchCnt.SortByDat(false);
      // ignore self match  

      TStrFltH jaccard; // same as matchCnt, but store float 
      // use second float array, store jaccard
      // if we ignore idf,this is sufficient

      for (int l=0; l<matchCnt.Len(); l++) {
        //printf("%s\n", matchCnt.GetKey(l).CStr());
        
        int prodInd = prodH.GetDat(matchCnt.GetKey(l));
        float mc = (float)matchCnt.GetDat(matchCnt.GetKey(l));
        float dc = sqrt((float)products[i].atr.Len()*(float)products[prodInd].atr.Len());
        float jc = mc/dc;
        jaccard.AddDat(matchCnt.GetKey(l), jc);
        //printf("%s\n", jaccard.GetKey(l).CStr());
        //if (l==1)
        //  return;
      }

      //10/11, dont print other products from original seller       
      //perhaps also restrict output to 1 product per seller??
      // allow or disallow shorter results?

      //output format: score seller product price
      TStrH sellerFlag;

      int maxRetrieve = 10;
      // if (matchCnt.Len() < maxRetrieve)
      //  maxRetrieve = matchCnt.Len();
      sellerFlag.AddDat(products[i].sellerID, 1);

      // query product
      fprintf(fout, "%s %s %f %f\n", products[i].sellerID.CStr(),
              products[i].title.CStr(), (float)products[i].price, (float)products[i].ratio);
      //fprintf(fout, "%d %d %f\n", i, products[i].atr.Len(), (float)products[i].price);
      
      jaccard.SortByDat(false);

      int numRetrieved=0;
      for (int l=0; l<matchCnt.Len(); l++) {                                                                                                                  
        int prodInd = prodH.GetDat(jaccard.GetKey(l));                                                                                                      
        // changed \t temporarily to \n
       
        // note: this may kill cases where sellername is empty 
        if (!sellerFlag.IsKey(products[prodInd].sellerID)) {
          fprintf(fout, "%f %s %s %f %f\n",
                  (float)jaccard.GetDat(jaccard.GetKey(l)),
                  products[prodInd].sellerID.CStr(),
                  products[prodInd].title.CStr(),
                  (float)products[prodInd].price, (float)products[prodInd].ratio); 
          sellerFlag.AddDat(products[prodInd].sellerID, 1);
          numRetrieved++;
          if (numRetrieved == maxRetrieve)
            break;
        }
        
        /*
        fprintf(fout, "%s %f %s %f %s\n",  jaccard.GetKey(l).CStr(), (float)jaccard.GetDat(jaccard.GetKey(l)), 
        products[prodInd].prodID.CStr(),(float)products[prodInd].price,
                products[prodInd].title.CStr()); 
        */
      }
      
      fprintf(fout, "\n");

      /*
      for (int l=1; l<maxRetrieve; l++) {
        int prodInd = prodH.GetDat(matchCnt.GetKey(l));
        fprintf(fout, "%s %d %s %d %f\t",  matchCnt.GetKey(l).CStr(),
               products[prodInd].atr.Len(), products[prodInd].prodID.CStr(),
               (int)matchCnt.GetDat(matchCnt.GetKey(l)), (float)products[prodInd].price);
      }
      fprintf(fout, "\n");
      */
      jaccard.Clr();
      matchCnt.Clr();
      sellerFlag.Clr();
      //break; // for testing
    }
    
    fclose(fout);
  }
 
}

// original func
void loadRaw(const TStr& filename) {
  TStrV fields;
  char info[22][2024]; // 22 lines per product
  FILE *file = fopen(filename.CStr(), "r");

  TStrH catCnt;
  TStrH catName;

  TStrStrH prodCat; // can product be in multiple cats? NO

  int lineCount = 0;
  int tmpCnt=0;
  int entryCnt=0;

  TStrStrH prodSeller; // productID to sellerName mapping 
  int dupProd=0;

  int blankSeller=0;

  while (!feof(file)) {
    TStr catID;
    TStr prodID;
    for (int i=0; i<22; i++) { // 21 = newline
      memset(info[i], 0, 2024);
      fgets(info[i], 2024, file);
      info[i][strlen(info[i])-1] = 0; // remove newline
      
      if (i==0) 
        prodID = TStr(info[i]).GetTrunc();

      if (i==1) { // hash category id
        // some id not crawled, so need str 
        catID = TStr(info[i]).GetTrunc();

        if (catCnt.IsKey(catID)) {
          int existCnt = catCnt.GetDat(catID);
          catCnt.AddDat(catID, existCnt+1);
        }
        else {
          catCnt.AddDat(catID, 1);
        }

        // if (catID == "50032296")
        //  printf("X\n");
        //break;
      }

      if (i==2) {
        TStr catN = TStr(info[i]).GetTrunc(); 
        if (!catName.IsKey(catN)) {
          catName.AddDat(catN, 1);
        }
      }
     
     //if (strlen(line) == 0)
      //  break;
      lineCount++;
    }

    if (prodCat.IsKey(prodID)) {
      TStr oldCat = prodCat.GetDat(prodID);
      if (oldCat != catID)
        printf("badCat %s %s %s\n", prodID.CStr(), oldCat.CStr(), catID.CStr()); 
    }
    else {
      prodCat.AddDat(prodID, catID);
    }

    entryCnt++;

    //TStr prodID = TStr(info[0]).GetTrunc();
    TStr sellerName = TStr(info[10]).GetTrunc();
    if (sellerName != "") {
      if (!prodSeller.IsKey(prodID)) 
        prodSeller.AddDat(prodID, sellerName);
      else {
        TStr oldSeller = prodSeller.GetDat(prodID);
        if (sellerName != oldSeller) { // shouldn't occur
          printf("%s * %s\n", oldSeller.CStr(), sellerName.CStr());
          dupProd++;
        }
      }
    }
    else
      blankSeller++;
    

    if (catID == "50008907" && strlen(info[9])>0) {
      // only consider products with descriptions
      tmpCnt++;
      //printf("%s %s %s\n", info[4], info[10], info[9]);
    }

    //printf("%s\n", info[9]);
    //TStr(info[9]).GetTrunc().SplitOnAllCh('\t', fields, false);
    //for (int j=0; j<fields.Len(); j++)
    //  printf("%s*** ", fields[j].CStr());
   
  }
  fclose(file);

  printf("num cat %d\n", catCnt.Len());
  
  for (int i=0; i<catCnt.Len(); i++)
    printf("%d %s %s %d\n", i, catCnt.GetKey(i).CStr(), catName.GetKey(i).CStr(), (int)catCnt[i]);
    
  printf("%d %d\n", lineCount, dupProd);
  printf("%d %d\n", blankSeller, entryCnt);
  //return;

  // NEW code below to output a mapping file
  // remove when not needed

  TStrH prodNum; // map from productID to sellerNum
 
  FILE *out2 = fopen("/lfs/1/tmp/mengqiu/fwd.1000000.txt", "r");
  
  char line[2024];

  while (!feof(out2)) {
    memset(line, 0, 2024);
    fgets(line, 2024, file);
    if (strlen(line) == 0)
      break;

    TStr(line).SplitOnAllCh('\t', fields, false);
    TInt SellerID = TInt(atoi(fields[1].GetTrunc().CStr())); // seller id 
    TStr ProductID = fields[3].GetTrunc();
    if (prodNum.IsKey(ProductID)) {
      TInt oldSeller = prodNum.GetDat(ProductID);
      if (oldSeller != SellerID)
        printf("%d %d\n", (int)oldSeller, (int)SellerID);
    }
    else {
      prodNum.AddDat(ProductID, SellerID);
    }
  }
  
  fclose(out2);

   // output file should map sellerID to sellerName 
  FILE *out3 = fopen("/lfs/1/tmp/mengqiu/sellerMap.txt", "w");
  int missingProd=0;
  int okayProd=0;
  TIntH sellerFlag; // dont print a seller twice
  for (int i=0; i<prodNum.Len(); i++) {
    TStr ProductID = prodNum.GetKey(i);
    TInt sellerID = prodNum.GetDat(ProductID);
    if (!sellerFlag.IsKey(sellerID)) {
      if (prodSeller.IsKey(ProductID)) {
        TStr sellerName = prodSeller.GetDat(ProductID);
        fprintf(out3, "%d %s\n", (int)sellerID, sellerName.CStr());
        sellerFlag.AddDat(sellerID, 1);
        okayProd++;
      }
      else
        missingProd++;
    }
    else
      okayProd++;
  }

  printf("missing %d okay %d\n", missingProd, okayProd);

  fclose(out3);
  
  /*
  for (int n=0; n<catCnt.Len(); n++) {
    printf("rnd %d\n", n);
    file = fopen(filename.CStr(), "r");
    while (!feof(file)) {
      TStr catID;
      for (int i=0; i<22; i++) { // 21 = newline      
        memset(info[i], 0, 2024);
        fgets(info[i], 2024, file);
        info[i][strlen(info[i])-1] = 0; // remove newline
      }
      
    }
    fclose(file);
  }
  */
}

// THIS BLOCK WORKS WITH MENGQIU's PARSE
void tempProcess() {
  char line[2024];
  char info[22][2024];
  TStrV fields;
  TStrV fields2;
  // another temp block for processing mq's cat 0.1 file                                                                                                   
  // ignore, look below
  /*
  FILE *out4 = fopen("/lfs/1/tmp/mengqiu/cat_brand_0.1.txt", "r");
  FILE *fout = fopen("/lfs/1/tmp/mengqiu/catStephen.txt", "w");
 
  while (!feof(out4)) {
    memset(line, 0, 2024);
    fgets(line, 2024, out4);
    if (strlen(line) == 0)      break;

    TStr(line).SplitOnAllCh('\t', fields, false);
    TStr(fields[3]).SplitOnAllCh('_', fields2, false);
    //printf("%s\n", fields2[0].CStr());
    fprintf(fout, "%s %s %s %s %s\n", fields[0].GetTrunc().CStr(), fields[1].GetTrunc().CStr(),
           fields[2].GetTrunc().CStr(), fields[3].GetTrunc().CStr(), fields2[0].GetTrunc().CStr());
  }

  fclose(out4);
  fclose(fout);
  */


  numProducts=0; // reset for each cat                                                                                                                     
  TStrH prodH; // hash of prod.sell Key to product number 
  FILE *in = fopen("/lfs/1/tmp/mengqiu/allRaw.txt", "r");
  int dup=0;
 
  // diagnostic variables
  int priceMissing = 0;
  int dealMissing = 0;
  int dealZero = 0;
  int proTrue = 0;
  int proFalse = 0;
  int proMissing = 0;
  int picTrue = 0;
  int picFalse = 0;
  int picMissing = 0;
  int ratioMissing = 0;
  int creditMissing = 0;
  int descMissing = 0;
  int servMissing = 0;
  int speedMissing = 0;
  while(!feof(in)) {
    for (int i=0; i<22; i++) { // 21 = newline                                                                                                               
      memset(info[i], 0, 2024);
      fgets(info[i], 2024, in);
      info[i][strlen(info[i])-1] = 0; // remove newline  
    }

    TStr prodKey = TStr(info[0]).GetTrunc();

    if (prodH.IsKey(prodKey)) {
      dup++;
    }
    else {
      products[numProducts].prodID = TStr(info[0]).GetTrunc();
      products[numProducts].catID = TStr(info[1]).GetTrunc();
      products[numProducts].title = TStr(info[3]).GetTrunc();

      TStr priceString = TStr(info[4]).GetTrunc();
      if (priceString.Len() == 0) {
        continue; // new skip 
      }
      products[numProducts].price = TFlt(atof(priceString.CStr()));
      
      TStr dealString = TStr(info[5]).GetTrunc();
      if (dealString.Len() == 0) {
        dealMissing++;
        products[numProducts].dealRecord = 0;
      }
      else {
        products[numProducts].dealRecord = TInt(atoi(dealString.CStr()));
        int dNum = atoi(dealString.CStr());
        if (dNum==0)
          dealZero++;
      }
    
      TStr pString = TStr(info[7]).GetTrunc();
      if (pString == "True") {
        products[numProducts].protection = 1.0;
        proTrue++;
      }
      else if (pString == "False") {
        products[numProducts].protection = -1.0;
        proFalse++;
      }
      else { // unknown
        products[numProducts].protection = 0.0;
        proMissing++;
      }
      
      // looks like nearly all products have pics
      TStr picString = TStr(info[8]).GetTrunc();
      if (picString == "True") {
        products[numProducts].hasPic = 1.0;
        picTrue++;
      }
      else if (picString == "False") {
        products[numProducts].hasPic = -1.0;
        picFalse++;
      }
      else {
        products[numProducts].hasPic = 0.0;
        picMissing++;
      }

      products[numProducts].sellerID = TStr(info[10]).GetTrunc();

      TStr creditStr = TStr(info[11]).GetTrunc();
      if (creditStr.Len()==0) {
        products[numProducts].credit = 0;
        creditMissing++;
      }
      else
        products[numProducts].credit = TInt(atoi(creditStr.CStr()));
      
      TStr ratioStr = TStr(info[12]).GetTrunc();
      if (ratioStr.Len()==0) {
        ratioMissing++;
        products[numProducts].ratio = -1.0; // NEW: need to check for this later
        // if rating is zero, just use average
      }
      else
        products[numProducts].ratio = TFlt(atof(ratioStr.CStr()));
      
      TStr descStr = TStr(info[13]).GetTrunc();
      if (descStr.Len()==0) {
        descMissing++;
        products[numProducts].desc = -1.0;
      }
      else
        products[numProducts].desc = TFlt(atof(descStr.CStr()));

      TStr servStr = TStr(info[14]).GetTrunc();
      if (servStr.Len()==0) {
        servMissing++;
        products[numProducts].serv = -1.0;
      }
      else
        products[numProducts].serv = TFlt(atof(servStr.CStr()));

      TStr speedStr = TStr(info[15]).GetTrunc();
      if (speedStr.Len()==0) {
        speedMissing++;
        products[numProducts].speed = -1.0;
      }
      else
        products[numProducts].speed = TFlt(atof(speedStr.CStr()));
      
      // load data into product struct
      // insert hash relevance
      prodH.AddDat(prodKey, numProducts);
      numProducts++;
    }
  }
  // OCT NEW: dealRecord, protection, hasPic, credit, descRating, servRating, speedRating

  printf("%d loaded %d dup\n", numProducts, dup);
  printf("priceMissing %d dealMissing %d dealZero %d protection %d %d %d hasPic %d  %d %d ratioMissing %d creditMissing %d descMissing %d servMissing %d speedMissing %d\n", 
priceMissing, dealMissing,
  dealZero, proTrue, proFalse, proMissing, picTrue,
         picFalse, picMissing, ratioMissing, creditMissing, descMissing,
  servMissing, speedMissing);

  fclose(in);
  //return; // debug

  // load mq's cat 0.1 file, augment it with more information 
  // can verify validity by comparing prices

   
  FILE *out4 = fopen("/lfs/1/tmp/mengqiu/cat_brand_0.1.txt", "r");
  // todo load mor stuff
  FILE *fout = fopen("/lfs/1/tmp/mengqiu/catStephenNew.txt", "w");
  int missing=0;
  int difPrice=0;

  TStrH catFlag;
  TStr prevCatBrand = ""; //
  
  float sellPerBlock = 0;
  float numBlock = 0;
  int blocksWith5=0; // at least 5 unique sellers 
  int blocksLess3=0;
  TStrH sellerFlag; // used to track num unique sellers per block
  while (!feof(out4)) {
    memset(line, 0, 2024);
    fgets(line, 2024, out4);
    if (strlen(line) == 0)      break;

    TStr(line).SplitOnAllCh('\t', fields, false);
    TStr prodKey = TStr(fields[0]).GetTrunc();
    TFlt prodPrice = TFlt(atof(TStr(fields[1]).GetTrunc().CStr()));
    TStr catBrand = TStr(fields[3]).GetTrunc();

    if (catBrand != prevCatBrand) {
      // new block of data reset averages
      sellPerBlock += (float)sellerFlag.Len();
      numBlock += 1.0;
      if (sellerFlag.Len() >= 5.0)
        blocksWith5++;
      if (sellerFlag.Len() < 3.0)
        blocksLess3++;
      sellerFlag.Clr();
    }
    prevCatBrand = catBrand;

    if (prodH.IsKey(prodKey)) {
      int prodInd = prodH.GetDat(prodKey);
      /*
      fprintf(fout, "%s %s %s %s ", fields[0].GetTrunc().CStr(),
              fields[1].GetTrunc().CStr(), fields[2].GetTrunc().CStr(),
    fields[3].GetTrunc().CStr());
      fprintf(fout, "%s %s %f %f\n", products[prodInd].title.CStr(),
              products[prodInd].sellerID.CStr(),
    (float)products[prodInd].ratio, (float)products[prodInd].price);
      */
    
      if (!sellerFlag.IsKey(products[prodInd].sellerID))
        sellerFlag.AddDat(products[prodInd].sellerID, 1);
  
      if (!catFlag.IsKey(products[prodInd].catID))
        catFlag.AddDat(products[prodInd].catID, 1);

      if (prodPrice != products[prodInd].price) { // isthere mapping problem??
        // if one price > 2*otherprice, assume bad mapping 
        if (!((prodPrice <
               0.5*products[prodInd].price)||(products[prodInd].price<0.5*prodPrice))) {
        
          //printf("%f %f\n", (float)prodPrice, (float)products[prodInd].price);
       
          fprintf(fout, "%s\t%s\t%s\t%s\t", fields[0].GetTrunc().CStr(),
                  fields[1].GetTrunc().CStr(), fields[2].GetTrunc().CStr(),
                  fields[3].GetTrunc().CStr());
          fprintf(fout, "%s\t%s\t%s\t%f\t%f\t%d\t%f\t%f\t%d\t%f\t%f\t%f\n", products[prodInd].title.CStr(),
                  products[prodInd].sellerID.CStr(), products[prodInd].catID.CStr(),       
                  (float)products[prodInd].ratio,
                  (float)products[prodInd].price,
                  (int)products[prodInd].dealRecord,
                  (float)products[prodInd].protection,
                  (float)products[prodInd].hasPic,
                  (int)products[prodInd].credit,(float)products[prodInd].desc, (float)products[prodInd].serv,(float)products[prodInd].speed);
        }
        else
          difPrice++;
      }
      else { // same price, good mapping  
        fprintf(fout, "%s\t%s\t%s\t%s\t", fields[0].GetTrunc().CStr(),
                fields[1].GetTrunc().CStr(), fields[2].GetTrunc().CStr(),
                fields[3].GetTrunc().CStr());
        fprintf(fout, "%s\t%s\t%s\t%f\t%f\t%d\t%f\t%f\t%d\t%f\t%f\t%f\n",
                  products[prodInd].title.CStr(),
                products[prodInd].sellerID.CStr(),
                  products[prodInd].catID.CStr(),
                (float)products[prodInd].ratio,
                (float)products[prodInd].price,
                (int)products[prodInd].dealRecord,
                (float)products[prodInd].protection,
                (float)products[prodInd].hasPic,
                (int)products[prodInd].credit,(float)products[prodInd].desc,
                  (float)products[prodInd].serv,(float)products[prodInd].speed);
        // old output style
        /*
        fprintf(fout, "%s\t%s\t%s\t%f\t%f\n", products[prodInd].title.CStr(),
                products[prodInd].sellerID.CStr(), products[prodInd].catID.CStr(),
                (float)products[prodInd].ratio, (float)products[prodInd].price);
        */
      }
    }
    else { // shouldnt happen..
      missing++;
    }
  }

  fclose(out4);
  fclose(fout);
  printf("missing %d difPrice %d\n", missing, difPrice);
  printf("num cats %d avgSellPerBlock %f numBlocks %f blocksLess3 %d blocksWith5 %d\n", catFlag.Len(),
         (float)sellPerBlock/numBlock, numBlock, blocksLess3, blocksWith5);
}

void splitCat(const TStr& filename) {
  int numCat = 0;
  FILE *files[100];
  TStrH cats;
  char info[22][2024]; // 22 lines per product

  FILE *file = fopen(filename.CStr(), "r");

  int lineCount = 0;
  int tmpCnt=0;

  while (!feof(file)) {
    for (int i=0; i<22; i++) { // 21 = newline
      memset(info[i], 0, 2024);
      fgets(info[i], 2024, file);
      info[i][strlen(info[i])-1] = 0; // remove newline 
    }
    
    TStr catID = TStr(info[1]).GetTrunc();
    int catNum;

    char buf[5];

    if (cats.IsKey(catID)) {
      catNum = cats.GetDat(catID);
    }
    else {
      catNum = numCat;
      numCat++;
      cats.AddDat(catID, catNum);
      // open new file
      TStr newFile = TStr::Fmt("/lfs/1/tmp/mengqiu/cats/%d.txt", catNum);
      files[catNum] = fopen(newFile.CStr(), "w");
      printf("%s\n", newFile.CStr());
    }

    for (int i=0; i<22; i++) {
      fprintf(files[catNum], "%s\n", info[i]);
    }
    //fprintf(files[catNum], "\n");
  }

  for (int i=0; i<numCat; i++)
    fclose(files[i]);

  fclose(file);

}

// aggregation of results in /lfs/..svmRank/tests/cats by clustSize
// analog of this function is in selectSplit.cpp
void aggBreakout(const TStr& breakFile) {
  int cnt=0;
  int catNum, trainSize, testSize;
  float p1Cnt, p1Tot, rRankCnt, rRankTot, RankTot, RFRankTot, FRankTot;
  float Ap1Cnt[11], Ap1Tot[11], ArRankCnt[11], ArRankTot[11], ARankTot[11], ARFRankTot[11], AFRankTot[11];

  for (int i=0; i<11; i++) {
    Ap1Cnt[i] = 0.0;
    Ap1Tot[i] = 0.0;
    ArRankCnt[i] = 0.0;
    ArRankTot[i] = 0.0;
    ARankTot[i] = 0.0;
    ARFRankTot[i] = 0.0;
    AFRankTot[i] = 0.0;
  }

  for (int i=0; i<82; i++) {
    FILE *fp;
    //fscanf(in, "%d %d %d", &catNum, &trainSize, &testSize);
    //if (trainSize > 1000) { // new minsize condition, comment out if not
    
    TStr bString = TStr::Fmt("/lfs/1/tmp/mengqiu/svmRank/tests/cats/breakout%d", i);
    if ((fp = fopen(bString.CStr(), "r")) != NULL) {
      cnt++;

      //fprintf(bf, "%f %f %f %f %f %f %f\n", p1TotArray[i], p1CntArray[i],
      //        RRnkCntArray[i], RRnkTotArray[i], RnkTotArray[i],
      //        RFRnkTotArray[i], FRnkTotArray[i]);


      for (int j=2; j<11; j++) {
        fscanf(fp, "%f %f %f %f %f %f %f", &p1Tot, &p1Cnt, &rRankCnt,
                &rRankTot, &RankTot, &RFRankTot, &FRankTot);
        Ap1Tot[j] += p1Tot;
        Ap1Cnt[j] += p1Cnt;
        ArRankCnt[j] += rRankCnt;
        ArRankTot[j] += rRankTot;
        ARankTot[j] += RankTot;
        ARFRankTot[j] += RFRankTot;
        AFRankTot[j] += FRankTot;
      }
 
      fclose(fp);
    }
  //}
  }
  printf("%d\n", cnt);
  //fclose(in);

  FILE *out = fopen(breakFile.CStr(), "w");
  for (int i=2; i<11; i++)
    fprintf(out, "%d %f %f %f %f %f\n", i, Ap1Tot[i]/Ap1Cnt[i],
         ArRankTot[i]/ArRankCnt[i], ARankTot[i]/ArRankCnt[i],
          ARFRankTot[i]/ArRankCnt[i], AFRankTot[i]/ArRankCnt[i]);
  fclose(out);

}

// BEFORE RUNNING, CHANGE GRAPHING.H to output last bin
// ALSO REMOVE BEZIER
// run on predResults (many svm results)
void computeAgg(const TStr& aggFile) {
  FILE *tf = fopen(aggFile.CStr(), "r");
  float p1Tot[100];
  float p1Cnt[100];
  float RRnkCnt[100];
  float RRnkTot[100];
  float RnkTot[100];
  float RFRnkTot[100]; 
  float FRnkTot[100];

  // overall performance
  float globalp1Tot = 0.0;
  float globalp1Cnt = 0.0; 
  float globalRRnkCnt = 0.0; 
  float globalRRnkTot = 0.0;
  float globalRnkTot = 0.0; 
  float globalRFRnkTot = 0.0;
  float globalFRnkTot = 0.0;

  int cnt=0;

  //TGnuPlot ScGnuPlot("binned_triMsgVolProbAllOverlay60D", "Binned Message
  //Strength vs Trade Close Prob Given Initial Message");
  //ScGnuPlot.AddPlot(outputV, gpwLinesPoints, "Any Purchase", "pt 6 smooth bezier");

  TIntFltH catPos; // store list of cats and num positive examples
  // sort for ordering

  while (!feof(tf)) {
    fscanf(tf, "%f %f %f %f %f %f %f\n", &p1Tot[cnt], &p1Cnt[cnt], &RRnkCnt[cnt], &RRnkTot[cnt], &RnkTot[cnt], &RFRnkTot[cnt], &FRnkTot[cnt]);
    globalp1Tot += p1Tot[cnt];
    globalp1Cnt += p1Cnt[cnt];
    globalRRnkCnt += RRnkCnt[cnt];
    globalRRnkTot += RRnkTot[cnt];
    globalRnkTot += RnkTot[cnt];
    globalRFRnkTot += RFRnkTot[cnt];
    globalFRnkTot += FRnkTot[cnt];
    catPos.AddDat(cnt, p1Cnt[cnt]);
    cnt++;
    //printf("%f %f %f %f %f %f %f\n", p1Tot, p1Cnt, RRnkCnt, RRnkTot, RnkTot, RFRnkTot, FRnkTot);
  }

  catPos.SortByDat(false); // descending size

  //FILE *out = fopen("sortedCatResults.out", "w");
  //fclose(out);

  TFltPrV p1Array; // (numCats, net p1)
  TFltPrV RRnkArray;
  TFltPrV RnkArray;
  TFltPrV RFRnkArray;
  TFltPrV FRnkArray;
  for (int l=catPos.Len()-1; l--; l>-0) {
    // l is last cat to consider
    float p1NetTot=0.0;
    float p1NetCnt=0.0;
    float RRnkNetCnt = 0.0;
    float RRnkNetTot = 0.0;
    float RnkNetTot = 0.0;
    float RFRnkNetTot = 0.0;
    float FRnkNetTot = 0.0;

    for (int i=0; i<=l; i++) {
      p1NetTot += p1Tot[catPos.GetKey(i)];
      p1NetCnt += p1Cnt[catPos.GetKey(i)];
      RRnkNetCnt += RRnkCnt[catPos.GetKey(i)];
      RRnkNetTot += RRnkTot[catPos.GetKey(i)];
      RnkNetTot += RnkTot[catPos.GetKey(i)];
      RFRnkNetTot += RFRnkTot[catPos.GetKey(i)];
      FRnkNetTot += FRnkTot[catPos.GetKey(i)];
    }

    p1Array.Add(TFltPr((float)l+1, p1NetTot/p1NetCnt));
    RRnkArray.Add(TFltPr((float)l+1, RRnkNetTot/RRnkNetCnt));
    RnkArray.Add(TFltPr((float)l+1, RnkNetTot/RRnkNetCnt));
    RFRnkArray.Add(TFltPr((float)l+1, RFRnkNetTot/RRnkNetCnt));
    FRnkArray.Add(TFltPr((float)l+1, FRnkNetTot/RRnkNetCnt));
  }

  TFltPrV outputV;
  binning(p1Array, "binned_p1Agg", 5.0, 1.0, false,"", "Cumulative Categories", "Precision at 1", outputV);
  /*
  TFltPrV outputV2;
  binning(RRnkArray, "binned_rrAgg", 5.0, 1.0, false,"Binned Mean Reciprocal Rank", "Number of Categories", "Mean Reciprocal Rank", outputV2);
  TFltPrV outputV3;
  binning(RnkArray, "binned_rAgg", 5.0, 1.0, false,"Binned Mean Rank", "Number of Categories", "Mean Rank", outputV3);
  TFltPrV outputV4;
  binning(RFRnkArray, "binned_rfrAgg", 5.0, 1.0, false,"Binned Mean Reciprocal Fractional Rank", "Number of Categories", "Mean Reciprocal Fractional Rank", outputV4);  
  TFltPrV outputV5;
  binning(FRnkArray, "binned_frAgg", 5.0, 1.0, false,"Binned Mean Fractional Rank", "Number of Categories", "Mean Fractional Rank", outputV5);
  */

  /*
  TGnuPlot pGnuPlot("predAggP1", "Precision at 1");
  pGnuPlot.AddPlot(p1Array, gpwLinesPoints, "Aggregated SVMs", "pt 6 smooth bezier");
  pGnuPlot.AddPlot(p1Array, gpwPoints, "Aggregated SVMs", "pt 6");
  pGnuPlot.SetXYLabel("Number of Categories", "Precision at 1");
  pGnuPlot.SetScale(gpsAuto);
  pGnuPlot.SavePng();

  TGnuPlot rrGnuPlot("rrnkAggP1", "Mean Reciprocal Rank");
  rrGnuPlot.AddPlot(RRnkArray, gpwLinesPoints, "Aggregated SVMs", "pt 6 smooth bezier");
  rrGnuPlot.AddPlot(RRnkArray, gpwPoints, "Aggregated SVMs", "pt 6");
  rrGnuPlot.SetXYLabel("Number of Categories", "Mean Reciprocal Rank");
  rrGnuPlot.SetScale(gpsAuto);
  rrGnuPlot.SavePng();

  TGnuPlot rGnuPlot("rnkAggP1", "Mean Rank");
  rGnuPlot.AddPlot(RnkArray, gpwLinesPoints, "Aggregated SVMs", "pt 6 smooth bezier");
  rGnuPlot.AddPlot(RnkArray, gpwPoints, "Aggregated SVMs", "pt 6");
  rGnuPlot.SetXYLabel("Number of Categories", "Mean Rank");
  rGnuPlot.SetScale(gpsAuto);
  rGnuPlot.SavePng();

  TGnuPlot rfrGnuPlot("rfrnkAggP1", "Mean Reciprocal Fractional Rank");
  rfrGnuPlot.AddPlot(RFRnkArray, gpwLinesPoints, "Aggregated SVMs", "pt 6 smooth bezier");
  rfrGnuPlot.AddPlot(RFRnkArray, gpwPoints, "Aggregated SVMs", "pt 6");
  rfrGnuPlot.SetXYLabel("Number of Categories", "Mean Reciprocal Fractional Rank");
  rfrGnuPlot.SetScale(gpsAuto);
  rfrGnuPlot.SavePng();

  TGnuPlot frGnuPlot("frnkAggP1", "Mean Fractional Rank");
  frGnuPlot.AddPlot(FRnkArray, gpwLinesPoints, "Aggregated SVMs", "pt 6 smooth bezier");
  frGnuPlot.AddPlot(FRnkArray, gpwPoints, "Aggregated SVMs", "pt 6");
  frGnuPlot.SetXYLabel("Number of Categories", "Mean Fractional Rank");
  frGnuPlot.SetScale(gpsAuto);
  frGnuPlot.SavePng();
  */

  printf("%f %f %f %f %f %f %f\n", globalp1Tot, globalp1Cnt, globalRRnkCnt, globalRRnkTot, globalRnkTot, globalRFRnkTot, globalFRnkTot);
  printf("p@1: %f MRR: %f MR: %f MRFR: %f MFR: %f\n", globalp1Tot/globalp1Cnt, globalRRnkTot/globalRRnkCnt, globalRnkTot/globalRRnkCnt,
         globalRFRnkTot/globalRRnkCnt, globalFRnkTot/globalRRnkCnt);

  fclose(tf);
}

// 1. split allRaw into category files
// load each category, store products, create hash from attribute to product
// for each product, look at set of all products which share at least one
// compute cosines for this set, hopefully small



// NEW: third parameter to output breakouts 
void computeRank(const TStr& testFile, const TStr& predictions, const TStr& breakouts) {
  FILE *tf = fopen(testFile.CStr(), "r");
  FILE *pf = fopen(predictions.CStr(), "r");
  FILE *bf = fopen(breakouts.CStr(), "w");
  
  // these two files should be the same length
  char line[2024];
  TStrV fields;
  int prevClust=0;
  int clustNum;
  int clustSize = 0;
  TFltV values;
  float value;
  int match;
  TFltV corValues;
  TIntV flagV; // flag for corvalues
  float globalRRnkCnt=0.0;
  float globalRRnkTot=0.0;
  float globalRnkTot=0.0;
 
  // need to count sellers
  float globalRFRnkTot=0.0; // fractional rank totals
  float globalFRnkTot=0.0;
  
  float p1Cnt = 0.0; // compute precision@1 manually
  float p1Tot = 0.0;

  // compute statistics by cluster size
  // can I compute baselines again without running the whole damn thing?
  float p1CntArray[11];
  float p1TotArray[11];
  float RRnkCntArray[11];
  float RRnkTotArray[11];
  float RnkTotArray[11];
  float RFRnkTotArray[11];
  float FRnkTotArray[11];

  for (int i=0; i<11; i++) {
    p1CntArray[i] = 0;
    p1TotArray[i] = 0;
    RRnkCntArray[i] = 0;
    RRnkTotArray[i] = 0;
    RnkTotArray[i] = 0;
    RFRnkTotArray[i] = 0;
    FRnkTotArray[i] = 0;
  }

  int lineNum=0;
  while(!feof(tf)) {
    memset(line, 0, 2024);
    fgets(line, 2024, tf);
    line[strlen(line)-1] = 0; // remove newline

    if (strlen(line)==0)
      break;

    TStr(line).GetTrunc().SplitOnAllCh(' ', fields, false);
    
    char *fd = fields[1].CStr();
    sscanf(fd, "qid:%d", &clustNum);

    if (clustSize==0) // initialize only once
      prevClust = clustNum;

    char *mt = fields[0].CStr();
    sscanf(mt, "%d", &match);
    fscanf(pf, "%f", &value); // read in svm prediction

    lineNum++;
    if (clustNum!=prevClust) {
      //clustSize = 1;

      // use clustSize as numSellers for fractional rank
       
      prevClust = clustNum;
      
      float rrnkCnt=0.0;
      float rrnkTot=0.0;
      float rnkTot=0.0;
      float rfrnkTot=0.0; // fractionals
      float frnkTot=0.0;

      // process cluster
      if (values.Len()==0 || corValues.Len()==0)
        printf("error\n");

      values.Sort(false);
      for (int i=0; i<values.Len(); i++) {
        for (int j=0; j<corValues.Len(); j++) {
          if (fabs(values[i]-corValues[j]) <0.0001 && flagV[j]==0) {
            // use least possible rank per correct answer
            rrnkCnt += 1.0;
            RRnkCntArray[clustSize] += 1.0;

            rrnkTot += 1.0/(float)(i+1);
            RRnkTotArray[clustSize] += 1.0/(float)(i+1);

            rnkTot += (float)(i+1);
            RnkTotArray[clustSize] += (float)(i+1);

            float frac = (float)(i+1)/(float)(clustSize);
            rfrnkTot += 1.0/frac;
            RFRnkTotArray[clustSize] += 1.0/frac;

            frnkTot += frac;
            FRnkTotArray[clustSize] += frac;
            
            flagV[j] = 1;
          }
        }
      }
     
      // NEW: compute precision@1
      for (int i=0; i<corValues.Len(); i++)
        if (fabs(values[0]-corValues[i]) < 0.0001) {
          p1Tot += 1.0;
          p1TotArray[clustSize] += 1.0;
          break;
        }
      p1Cnt += 1.0;
      p1CntArray[clustSize] += 1.0;

      //printf("%f\t", rnkCnt);
      //printf("%f\t", rnkTot/rnkCnt);
      globalRRnkCnt += rrnkCnt;
      globalRRnkTot += rrnkTot;
      globalRnkTot += rnkTot;
      globalRFRnkTot += rfrnkTot;
      globalFRnkTot += frnkTot;

      values.Clr();
      corValues.Clr();
      flagV.Clr();
      clustSize = 1;
    }
    else
      clustSize++;

    values.Add(value);
    if (match == 2) {
      corValues.Add(value);
      flagV.Add(0);
    }
    //printf("%d\t", lineNum);
  }

  // comment this line out for batch processes!!
  
  printf("p@1: %f MRR: %f MR: %f MRFR: %f MFR: %f\n", p1Tot/p1Cnt,
         globalRRnkTot/globalRRnkCnt, globalRnkTot/globalRRnkCnt,
         globalRFRnkTot/globalRRnkCnt, globalFRnkTot/globalRRnkCnt);
  

  // note change in order of variables. use this output to aggregate results
  printf("%f %f %f %f %f %f %f\n", p1Tot, p1Cnt, globalRRnkCnt, globalRRnkTot,
         globalRnkTot, globalRFRnkTot, globalFRnkTot);

  // output probs to file, graph vs baselines
  
  //FILE *outCR = fopen("predStatsM.out", "w");  
  // default breakout results
  for (int i=2; i<11; i++)
    if (p1CntArray[i]>0.001 && RRnkCntArray[i]>0.001) // not zero
      fprintf(bf, "%d %f %f %f %f %f\n", i, p1TotArray[i]/p1CntArray[i],
             RRnkTotArray[i]/RRnkCntArray[i], RnkTotArray[i]/RRnkCntArray[i],
             RFRnkTotArray[i]/RRnkCntArray[i], FRnkTotArray[i]/RRnkCntArray[i]);
  //fclose(outCR);
  
  // output zeroes as well, aggregation function can ignore it
  // uncomment for aggregation
  /*
  for (int i=2; i<11; i++)
    fprintf(bf, "%f %f %f %f %f %f %f\n", p1TotArray[i], p1CntArray[i],
            RRnkCntArray[i], RRnkTotArray[i], RnkTotArray[i], RFRnkTotArray[i], FRnkTotArray[i]);
  */

  fclose(tf);
  fclose(pf);
  fclose(bf);
}

// create 4 batch files, run simultaneously
void generateBatch() {
  // load file with categories and sizes
  FILE *in = fopen("catSizes.out", "r");
  int SPLIT = 4;
  int MAXCATS=82;

  FILE *batchFiles[SPLIT];
  for (int i=0; i<SPLIT; i++) {
    TStr newTrainFile = TStr::Fmt("svmRank/batchRankNew%d.txt", i);
    batchFiles[i] = fopen(newTrainFile.CStr(), "w");
  }

  int catNum, trainSize, testSize;

  for (int i=0; i<MAXCATS; i++) {
    fscanf(in, "%d %d %d", &catNum, &trainSize, &testSize);
  //svm_rank_learn -c 20.0 train.dat model.dat
    if (testSize > 0) { // many zero size cats 
      float cValue = (float)trainSize/440.0; // we've been using 100 / 44000
      
      int splitNum = i/(MAXCATS/SPLIT + 1); // 82/4+1 = 21
      //float cValue = (float)trainSize/10.0; // old c value
      // may have to change 200 iterations later if needed
      TStr learnString = TStr::Fmt("./svm_rank_learn -c %f /lfs/1/tmp/mengqiu/svmRank/tests/cats/train%d.txt /lfs/1/tmp/mengqiu/svmRank/tests/cats/model%d.dat >> /lfs/1/tmp/mengqiu/svmRank/tests/cats/diag%d.out", cValue, catNum,
      catNum, splitNum);
     
      fprintf(batchFiles[splitNum], "%s\n", learnString.CStr());
             
      //svm_rank_classify test.dat model.dat predictions
      TStr classifyString = TStr::Fmt("./svm_rank_classify /lfs/1/tmp/mengqiu/svmRank/tests/cats/test%d.txt /lfs/1/tmp/mengqiu/svmRank/tests/cats/model%d.dat /lfs/1/tmp/mengqiu/svmRank/tests/cats/predictions%d",
                                      catNum, catNum, catNum);
      fprintf(batchFiles[splitNum], "%s\n", classifyString.CStr());
    }
  }
  fclose(in);
  for (int i=0; i<SPLIT; i++)
    fclose(batchFiles[i]);
}

// Try ignoring small categories??
// results are dumped into predResults*
void generatePredict(const TStr& batchFile, const TStr& outFile) {
  // generate all calls to predict where prediction file exists
  FILE *in = fopen("catSizes.out", "r");
  FILE *out = fopen(batchFile.CStr(), "w");
  
  int cnt=0;
  int catNum, trainSize, testSize;
  for (int i=0; i<82; i++) {
    FILE *fp;
    fscanf(in, "%d %d %d", &catNum, &trainSize, &testSize);
    //if (trainSize > 1000) { // new minsize condition, comment out if not needed
      TStr predictString = TStr::Fmt("/lfs/1/tmp/mengqiu/svmRank/tests/cats/predictions%d", i);
      if ((fp = fopen(predictString.CStr(), "r")) != NULL) {
        cnt++;
        // output previously sent to svmRank/cats/predResults.out
        TStr commandString = TStr::Fmt("./prediction /lfs/1/tmp/mengqiu/svmRank/tests/cats/test%d.txt /lfs/1/tmp/mengqiu/svmRank/tests/cats/predictions%d /lfs/1/tmp/mengqiu/svmRank/tests/cats/breakout%d >> %s", i, i, i, outFile.CStr());
        fprintf(out, "%s\n", commandString.CStr());
        
        fclose(fp);
      }
      //}
  }
  printf("%d\n", cnt);
  fclose(in);
  fclose(out);
}

// load global pageranks in updatepred
// implement personalized pagerank as well
void testFunc() {
  FILE *in = fopen("global_pagerank/contact.pagerank", "r");
  int node;
  int cnt=0;
  double pr;
  double min = 999999.0;
  double maxs = 0.0;
  while(!feof(in)) {
    fscanf(in, "%d %le", &node, &pr);
    pr = pr * 100000.0;
    if (pr > maxs)
      maxs = pr;
    if (pr < min)
      min = pr;
    //printf("%lf\n", pr*10000.0);
    cnt++;
    if (cnt == 1000000)
      break;
  }
  printf("%lf %lf\n", min, maxs);
  fclose(in);
}

int main(int argc, char* argv[]) {
  TStr file1 = TStr(argv[1]);
  TStr file2;
  TStr file3;
  if (argc >= 3)
    file2 = TStr(argv[2]);

  if (argc >= 4)
    file3 = TStr(argv[3]);

  //splitCat(file1);
  //loadRaw(file1); // new clustering method
  //loadCat();
  
  //tempProcess(); // mq clustering method

  //aggBreakout(file1);
  computeRank(file1, file2, file3);
  //computeAgg(file1);

  //generateBatch(); // generate batch file 
  //generatePredict(file1, file2); // batchPredict, predictOutput

  // load global pr * 10^6
  //testFunc();

  return 0;
}
