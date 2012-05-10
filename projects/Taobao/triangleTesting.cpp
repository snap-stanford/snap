#include "Snap.h"
#include "stdafx.h"
#include "ncp.cpp"
#include "taobaonet.h"
#include "graphing.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void basicStats(PTaobaoNet t);
void nodePairCounting(PTaobaoNet t);
void triangleCounting(PTaobaoNet t);
void triangleClosing(PTaobaoNet t);
//void triangleClosingAll(PTaobaoNet t);
void triangleClosed(PTaobaoNet t);
void msgEdgeStudy(PTaobaoNet t);
void EventSequenceProb(PTaobaoNet t);
// oct: new counting methods
void triangleCountingNew(PTaobaoNet t); 
void dirCounting(PTaobaoNet t);

template <class PGraph> double GetCCF(const PGraph& Graph)
{
  TIntTrV NIdCOTriadV;
  TSnap::GetTriads(Graph, NIdCOTriadV, -1);
  if (NIdCOTriadV.Empty()) { return 0.0; }
  double SumCcf = 0.0;
  int weirdCnt=0;
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    int denom = NIdCOTriadV[i].Val2()+NIdCOTriadV[i].Val3();
    if (denom != 0) // this is triggering!!
      SumCcf += (double)NIdCOTriadV[i].Val2() / (double) denom;
    else
      weirdCnt++;
  }
  printf("%lf %d weird %d\n", SumCcf, NIdCOTriadV.Len(), weirdCnt);

  return SumCcf / double(NIdCOTriadV.Len());
}

// get num edges, avg in/out deg for directed graphs
void reallyBasic(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message

  printf("%d %d\n", s->GetEdges(), m->GetEdges());
  
  float sInSum=0.0;
  float sOutSum=0.0;
  float mInSum=0.0;
  float mOutSum=0.0;
  float sCnt=0.0;
  float mCnt=0.0;
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    sInSum += (float)NI.GetInDeg();
    sOutSum += (float)NI.GetOutDeg();
    sCnt += 1.0;
  }
  printf("%f %f\n", sInSum/sCnt, sOutSum/sCnt);

  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    mInSum += (float)NI.GetInDeg();
    mOutSum += (float)NI.GetOutDeg();
    mCnt += 1.0;
  }
  printf("%f %f\n", mInSum/mCnt, mOutSum/mCnt);

}

void countSellers(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  int buyerCnt=0;
  int sellerCnt=0;
  
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    if (NI.GetInDeg() > NI.GetOutDeg()) // was >=
      sellerCnt++;
    else
      buyerCnt++;
  }
  printf("%d %d\n", buyerCnt, sellerCnt);
}

// new: for triangle counting and basicStats
// permute all 3 networks in undirected fashion
void permuteEdgesUndir(PTaobaoNet t) {

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet c = t->contactNet; 

  PTaobaoNet t2 = new TTaobaoNet(); 
  t2->shopNet = new TTaobaoShopNet();
  t2->msgNet = new TTaobaoShopNet();
  t2->contactNet = new TTaobaoShopNet();


  int numSpokes, randCnt, srcDup, edgeDup;
  TIntV spokes;

  // trade network
  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (!t2->shopNet->IsNode(SrcId)) {
      TTaobaoUsr newUsr = TTaobaoUsr(false, false);
      t2->shopNet->AddNode(SrcId, newUsr);
    }
    if (!t2->shopNet->IsNode(DstId)) {
      TTaobaoUsr newUsr = TTaobaoUsr(false, false);
      t2->shopNet->AddNode(DstId, newUsr);
    }

    // add only one direction of reciprocated pairs
    if (!(s->IsEdge(DstId, SrcId)&&DstId<SrcId)) {
      spokes.Add(SrcId);
      spokes.Add(DstId);
    }
  }

  spokes.Shuffle(TInt::Rnd);
  numSpokes = spokes.Len();
  randCnt=0;
  srcDup=0;
  edgeDup=0;
  for (int i=0; i<numSpokes; i+=2) { // +=2, connect each pair
    int src = spokes[i];
    int dst = spokes[(i+1)%numSpokes];
    if (src == dst)
      srcDup++;
    else if (t2->shopNet->IsEdge(src,dst))
      edgeDup++;
    else {   
      TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
      t2->shopNet->AddEdge(src, dst, EdgeDat);
      EdgeDat = TTaobaoShopEdge();
      t2->shopNet->AddEdge(dst, src, EdgeDat);
      randCnt++;
    }
  }

  spokes.Clr();
  printf("trade connected %d actual %d\n", randCnt, numSpokes/2);
  printf("%d %d\n", srcDup, edgeDup);
 

  // msg network
  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (!t2->msgNet->IsNode(SrcId)) {
      TTaobaoUsr newUsr = TTaobaoUsr(false, false);
      t2->msgNet->AddNode(SrcId, newUsr);
    }
    if (!t2->msgNet->IsNode(DstId)) {
      TTaobaoUsr newUsr = TTaobaoUsr(false, false);
      t2->msgNet->AddNode(DstId, newUsr);
    }

  
    if (!(m->IsEdge(DstId, SrcId)&&DstId<SrcId)) {
      spokes.Add(SrcId);
      spokes.Add(DstId);
    }
  }

  spokes.Shuffle(TInt::Rnd);
  numSpokes = spokes.Len();
  randCnt=0;
  srcDup=0;
  edgeDup=0;
  for (int i=0; i<numSpokes; i+=2) { // +=2, connect each pair
    int src = spokes[i];
    int dst = spokes[(i+1)%numSpokes];
    if (src == dst)
      srcDup++;
    else if (t2->msgNet->IsEdge(src,dst))
      edgeDup++;
    else {
      TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
      t2->msgNet->AddEdge(src, dst, EdgeDat);
      EdgeDat = TTaobaoShopEdge();
      t2->msgNet->AddEdge(dst, src, EdgeDat);
      randCnt++;
    }
  }

  spokes.Clr();
  printf("message connected %d actual %d\n", randCnt, numSpokes/2);
  printf("%d %d\n", srcDup, edgeDup);
 
  // permute oontact network
  for (TTaobaoShopNet::TNodeI NI = c->BegNI(); NI < c->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( true, false);
    t2->contactNet->AddNode(ID, newUsr);
    
    // contact network has two copies of each edge
    for (int i=0; i<NI.GetInDeg(); i++)
      spokes.Add(ID);
  }
    
  spokes.Shuffle(TInt::Rnd);
  numSpokes = spokes.Len();
  randCnt=0;
  srcDup=0;
  edgeDup=0;
  for (int i=0; i<numSpokes; i+=2) { // +=2, connect each pair
    int src = spokes[i];
    int dst = spokes[(i+1)%numSpokes];
    if (src == dst)
      srcDup++;
    else if (t2->contactNet->IsEdge(src,dst))
      edgeDup++;
    else {
      // skip adding timestamps for contacts for now
      // not sure how to do this properly

      TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
      t2->contactNet->AddEdge(src, dst, EdgeDat);
      EdgeDat = TTaobaoShopEdge();
      t2->contactNet->AddEdge(dst, src, EdgeDat);
      randCnt++;
    }
  }
  
  printf("contact connected %d actual %d\n", randCnt, numSpokes/2);
  printf("%d %d\n", srcDup, edgeDup);  
  printf("%d %d %d\n", t2->shopNet->GetNodes(), t2->msgNet->GetNodes(), t2->contactNet->GetNodes());
  printf("%d %d %d\n", t2->shopNet->GetEdges(), t2->msgNet->GetEdges(), t2->contactNet->GetEdges());

  basicStats(t2);
  //triangleCounting(t2);
}


// create 3 randomized networks with the same degrees 
// there is a problem with x->AddEdge, it erases other network
void permuteEdges(PTaobaoNet t) {

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet c = t->contactNet; 

  PTaobaoNet t2 = new TTaobaoNet(); 
  t2->shopNet = new TTaobaoShopNet();
  t2->msgNet = new TTaobaoShopNet();
  t2->contactNet = new TTaobaoShopNet();

  TIntV inSpokes; 
  TIntV outSpokes;

  TIntH flagH; // used for edge TX tracking
  // flag is a counter for outdegree from a node

  // trade network  
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( true, false);

    flagH.AddDat(ID, 0);
    t2->shopNet->AddNode(ID, newUsr);
    
    for (int i=0; i<NI.GetInDeg(); i++) 
      inSpokes.Add(ID);
    
    for (int i=0; i<NI.GetOutDeg(); i++) 
      outSpokes.Add(ID);
  }
  
  inSpokes.Shuffle(TInt::Rnd);
  outSpokes.Shuffle(TInt::Rnd);
  
  int numSpokes = outSpokes.Len();
  int randCnt=0;
  for (int i=0; i<numSpokes; i++) {
    int src = outSpokes[i];
    int dst = inSpokes[(i+1)%numSpokes];
    if (src != dst && !t2->shopNet->IsEdge(src, dst)) {
      TTaobaoShopEdge newEdge = TTaobaoShopEdge();
      TTaobaoShopNet::TNodeI NI = s->GetNI(src);
      int eIndex = (int)flagH.GetDat(src);
      TTaobaoShopEdge& origEdge = NI.GetOutEDat(eIndex);
      for (int j=0; j<origEdge.CntTX(); j++) {
        TTaobaoTX tx = TTaobaoTX(origEdge.GetTime(j), origEdge.GetStrength(j));
        newEdge.AddTX(tx);
      }
      
      eIndex++; // increment counter
      flagH.AddDat(src, eIndex); 

      t2->shopNet->AddEdge(src, dst, newEdge);
      randCnt++;
    }
  }
  
  printf("purchase connected %d actual %d\n", randCnt, numSpokes);
  inSpokes.Clr();
  outSpokes.Clr();
  flagH.Clr();
  
  printf("%d %d %d\n", t2->shopNet->GetNodes(), t2->msgNet->GetNodes(), t2->contactNet->GetNodes());
  printf("%d %d %d\n", t2->shopNet->GetEdges(), t2->msgNet->GetEdges(), t2->contactNet->GetEdges());
  
  // message network
  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( false, false);
    flagH.AddDat(ID, 0);

    t2->msgNet->AddNode(ID, newUsr);
    
    for (int i=0; i<NI.GetInDeg(); i++) 
      inSpokes.Add(ID);
    
    for (int i=0; i<NI.GetOutDeg(); i++) 
      outSpokes.Add(ID);
  }
  
  inSpokes.Shuffle(TInt::Rnd);
  outSpokes.Shuffle(TInt::Rnd);
  
  numSpokes = outSpokes.Len();
  randCnt=0;
  for (int i=0; i<numSpokes; i++) {
    int src = outSpokes[i];
    int dst = inSpokes[(i+1)%numSpokes];
    if (src != dst && !t2->msgNet->IsEdge(src, dst)) {

      TTaobaoShopEdge newEdge = TTaobaoShopEdge();
      TTaobaoShopNet::TNodeI NI = m->GetNI(src);
      int eIndex = (int)flagH.GetDat(src);
      TTaobaoShopEdge& origEdge = NI.GetOutEDat(eIndex);
      for (int j=0; j<origEdge.CntTX(); j++) {
        TTaobaoTX tx = TTaobaoTX(origEdge.GetTime(j), origEdge.GetStrength(j));
        newEdge.AddTX(tx);
      }

      eIndex++; // increment counter
      flagH.AddDat(src, eIndex);

      t2->msgNet->AddEdge(src, dst, newEdge);
      randCnt++;
    }
  }
  
  printf("msg connected %d actual %d\n", randCnt, numSpokes);
  inSpokes.Clr();
  outSpokes.Clr();
  flagH.Clr();

  printf("%d %d %d\n", t2->shopNet->GetNodes(), t2->msgNet->GetNodes(), t2->contactNet->GetNodes());
  printf("%d %d %d\n", t2->shopNet->GetEdges(), t2->msgNet->GetEdges(), t2->contactNet->GetEdges());
  
  // permute oontact network
  TIntV spokes;
  for (TTaobaoShopNet::TNodeI NI = c->BegNI(); NI < c->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( true, false);
    t2->contactNet->AddNode(ID, newUsr);
    
    // contact network has two copies of each edge
    for (int i=0; i<NI.GetInDeg(); i++)
        spokes.Add(ID);
  }
    
  spokes.Shuffle(TInt::Rnd);
  numSpokes = spokes.Len();
  randCnt=0;
  int srcDup=0;
  int edgeDup=0;
  for (int i=0; i<numSpokes; i+=2) { // +=2, connect each pair
    int src = spokes[i];
    int dst = spokes[(i+1)%numSpokes];
    if (src == dst)
      srcDup++;
    else if (t2->contactNet->IsEdge(src,dst))
      edgeDup++;
    else {
      // skip adding timestamps for contacts for now
      // not sure how to do this properly

      TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
      t2->contactNet->AddEdge(src, dst, EdgeDat);
      EdgeDat = TTaobaoShopEdge();
      t2->contactNet->AddEdge(dst, src, EdgeDat);
      randCnt++;
    }
  }
  
  printf("contact connected %d actual %d\n", randCnt, numSpokes/2);
  printf("%d %d\n", srcDup, edgeDup);
  
  printf("%d %d %d\n", t2->shopNet->GetNodes(), t2->msgNet->GetNodes(), t2->contactNet->GetNodes());
  printf("%d %d %d\n", t2->shopNet->GetEdges(), t2->msgNet->GetEdges(), t2->contactNet->GetEdges());

  //triangleClosingAll(t2);
  //triangleClosed(t2);
  //msgEdgeStudy(t2);
  //EventSequenceProb(t2);
  basicStats(t2);
  //triangleClosing(t2);
  //triangleCounting(t2);
  //nodePairCounting(t2);
  
  printf("%d %d %d\n", t2->shopNet->GetNodes(), t2->msgNet->GetNodes(), t2->contactNet->GetNodes());
  printf("%d %d %d\n", t2->shopNet->GetEdges(), t2->msgNet->GetEdges(), t2->contactNet->GetEdges());
  
  //basicStats(t2);
}

// OCT: before/between/after plotting
void plotEvent() {
  TFltPrV plotV;
  TFltPrV plotV2;
  TFltPrV plotV3;

  float a, b, c;
  FILE *in = fopen("eventOCT.out", "r");
  int i = 1;
  while (fscanf(in, "%f %f %f", &a, &b, &c) == 3) {
    plotV.Add(TFltPr((float)i,(float)a));
    plotV2.Add(TFltPr((float)i,(float)b));
    plotV3.Add(TFltPr((float)i,(float)c));
    i++;
  }
  fclose(in);

  TGnuPlot plot1("eventSequence", "", false);
  plot1.AddCmd("set key top right");
  plot1.AddPlot(plotV2, gpwLinesPoints, "Between", "lt 1 pt 5 ps 2");
  plot1.AddPlot(plotV, gpwLinesPoints, "Before", "lt 2 pt 7 ps 2");
  plot1.AddPlot(plotV3, gpwLinesPoints, "After", "lt 3 pt 9 ps 2");
  plot1.SetXYLabel("Days", "Avg Msgs Per Day");
  plot1.SetScale(gpsAuto);
  plot1.SaveEps("eventSequence.eps", 30, false);
  
}

// plot mengqiu's trust experiments
void plotTrust() {
  TFltPrV plotV;
  TFltPrV plotV2;
  TFltPrV plotV3;

  // > ~/mengqiu/paper2/taobao-shopping/repeated_purchase.txt
  // > ~/mengqiu/paper2/taobao-shopping/seller_vs_price.txt
  // > ~/mengqiu/paper2/taobao-shopping/item_vs_price.txt

  int a;
  float b;
  FILE *in = fopen("../../../mengqiu/paper2/taobao-shopping/repeated_purchase.txt", "r");
  while (fscanf(in, "%d %f", &a, &b) == 2) {
    if (a <= 20)
      plotV.Add(TFltPr((float)a,(float)b));
  }
  fclose(in);

  TFltPrV outputV;
  //binning(plotV, "binned_rp", 10.0, 1.0, false, "", "", "", outputV);
  TGnuPlot plot1("repeated_purchase", "", false);
  plot1.AddPlot(plotV, gpwLinesPoints, "", "lt 1 pt 5 ps 3");
  plot1.SetXYLabel("", "");
  plot1.SetScale(gpsAuto);
  plot1.SaveEps("repeated_purchase.eps", 30, false);

  float c, d;
  FILE *in2 = fopen("sellerPrice2.out", "r");
  while (fscanf(in2, "%f %f", &c, &d) == 2) {
    plotV2.Add(TFltPr((float)c,(float)d));
  }
  fclose(in2);

  TFltPrV outputV2;
  //binning(plotV2, "binned_sp", 0.01, 1.0, false, "", "", "", outputV2);
  TGnuPlot plot2("seller_vs_price", "", false);
  plot2.AddCmd("set key left top");
  plot2.AddPlot(plotV2, gpwPoints, "", "lt 2 pt 5 ps 1");
  //plot2.AddPlot(plotV2, gpwPoints, "", "lt 1 pt 5 ps 1");
  plot2.AddFunc("1.6651*x**15.749-1", gpwLines, "Power: 1.665*x^{15.749}-1", "lt 1 lw 3");
  plot2.AddFunc("0", gpwLines, "", "lt 3 lw 3");
  plot2.SetXYLabel("Seller Rating", "Avg Price Dev (%)");
  plot2.SetScale(gpsAuto);
  plot2.SaveEps("seller_vs_price.eps", 30, false);

  float e, f;
  FILE *in3 = fopen("itemPrice2.out", "r");
  while (fscanf(in3, "%f %f", &e, &f) == 2) {
    plotV3.Add(TFltPr((float)e,(float)f));
  }
  fclose(in3);

  //temp add 1.0 for power

  TFltPrV outputV3;
  //binning(plotV, "binned_rp", 10.0, 1.0, false, "", "", "", outputV);
  TGnuPlot plot3("item_vs_price", "", false);
  //plot3.AddCmd("plot [0.944:1] 1.6651*x**15.749-1");
  //GP.AddFunc("2*x**-2+4", gpwLines, "2*x^-2+4");
  plot3.AddCmd("set key left top");
  plot3.AddPlot(plotV3, gpwPoints, "", "lt 2 pt 5 ps 1");
  plot3.AddFunc("1.7466*x**18.504-1", gpwLines, "Power: 1.747*x^{18.504}-1", "lt 1 lw 3"); 
  plot3.AddFunc("0", gpwLines, "", "lt 3 lw 3");  
//plot3.AddPwrFit(plotID, gpwLines); 
  //plot3.AddPlot(plotV3, gpwPoints, "", "lt 1 pt 5 ps 1");
  plot3.SetXYLabel("Seller Rating", "Avg Price Dev (%)");
  plot3.SetScale(gpsAuto);
  plot3.SaveEps("item_vs_price.eps", 30, false);

}

void plotTriangle() {
  TFltPrV msgProbV;
  TFltPrV msgProbV2;
  TFltPrV msgProbV3;
  TFltTrV msgProbTR;
  TFltTrV msgProbTR2;
  TFltTrV msgProbTR3;
  TFltTrV msgProbTR4;
  float xLimit = 1000000.0;
  int msgStrength;
  float tradeProb;
  int ySuc, yTot;
  
  
  //FILE *in = fopen("triMsgVolProbNew3D.out", "r");
  FILE *in = fopen("triMsgVolProbAllNew60D.out", "r");
  while (fscanf(in, "%d %d %d", &msgStrength, &ySuc, &yTot) == 3) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbTR.Add(TFltTr((float)msgStrength, (float)ySuc, (float)yTot));
  }
  fclose(in);

  //FILE *in2 = fopen("triMsgVolProbFirst3D.out", "r");
  FILE *in2 = fopen("triMsgVolProbAllFirst60D.out", "r");
  while (fscanf(in2, "%d %d %d", &msgStrength, &ySuc, &yTot) == 3) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbTR2.Add(TFltTr((float)msgStrength, (float)ySuc, (float)yTot));
  }
  fclose(in2);
  
  // xvalue is not msgstrenght, its day
  // OCT
  /*
  FILE *in3 = fopen("triCloseDayNew3D.out", "r");
  while (fscanf(in3, "%d %d %d", &msgStrength, &ySuc, &yTot) == 3) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbTR3.Add(TFltTr((float)msgStrength, (float)ySuc, (float)yTot));
  }
  fclose(in3);
  
  FILE *in4 = fopen("triCloseDayFirst3D.out", "r");
  while (fscanf(in4, "%d %d %d", &msgStrength, &ySuc, &yTot) == 3) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbTR4.Add(TFltTr((float)msgStrength, (float)ySuc, (float)yTot));
  }
  fclose(in4);
  */

  /*
  FILE *in = fopen("triMsgVolProbMSG.out", "r");
  while (fscanf(in, "%d %f", &msgStrength, &tradeProb) == 2) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbV.Add(TFltPr((float)msgStrength, tradeProb));
  }
  fclose(in);
  */

 //FILE *in2 = fopen("triMsgVolProbMSGTRADE.out", "r");
  /*
  FILE *in2 = fopen("triMsgVolProb.out", "r");
  while (fscanf(in2, "%d %f", &msgStrength, &tradeProb) == 2) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbV2.Add(TFltPr((float)msgStrength, tradeProb));
  }
  fclose(in2);
  */

  /*
  FILE *in3 = fopen("triMsgVolProbMSGCON.out", "r");
  while (fscanf(in3, "%d %f", &msgStrength, &tradeProb) == 2) {
    if (msgStrength >= xLimit) // arbitrary limit
      break;
    msgProbV3.Add(TFltPr((float)msgStrength, tradeProb));
  }
  fclose(in3);
  */

  /*
  TGnuPlot tPGnuPlot("triMsgVolTradeConProb", "Triangle Message Strength vs
  Trade Triangle Close Probability");
  tPGnuPlot.AddPlot(msgProbV, gpwPoints, "", "pt 6");
  tPGnuPlot.SetXYLabel("Message Strength", "Trade Probability");
  tPGnuPlot.SetScale(gpsAuto);
  tPGnuPlot.SavePng();
  */

  /*
  TFltPrV outputV;
  binning(msgProbV, "binned_triMsgVolMsgProb", 1.0, 2.0, false, "Binned Message
  Strength vs Message Close Prob Given Initial Message", "Message Strength",
          "Triangle Probability", outputV);
  */

  //TFltPrV outputV2;
  //binning(msgProbV2, "binned_triMsgVolMsgTradeAll", 1000.0, 1.0, false, "Binned Message Strength vs Trade Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV2);
  
  
  TFltPrV outputV;
  binProb(msgProbTR, "binned_triMsgVolProbNew3D", 1.0, 2.0, true,"Binned Message Strength vs Trade Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV);

  TFltPrV outputV2;
  binProb(msgProbTR2, "binned_triMsgVolProbFirst3D", 1.0, 2.0, true,"Binned Message Strength vs Trade Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV2);
  

  /*
  TFltPrV outputV3;
  binProb(msgProbTR3, "binned_triMsgVolProbNewPEDGE", 1.0, 2.0, true,"Binned Message Strength vs Trade Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV3);

  TFltPrV outputV4;
  binProb(msgProbTR4, "binned_triMsgVolProbFirstPEDGE", 1.0, 2.0, true,"Binned Message Strength vs Trade Close Prob Given Initial Message", "Message Strength", "Triangle Probability", outputV4);
  */

  /*
  TFltPrV outputV;
  binProb(msgProbTR3, "binned_triCloseDayNew", 5.0, 1.0, false,"Binned Time Difference vs Trade Close Prob Given Initial Message", "Time Difference", "Triangle Probability", outputV);

  TFltPrV outputV2;
  binProb(msgProbTR4, "binned_triCloseDayFirst", 5.0, 1.0, false,"Binned Time Difference vs Trade Close Prob Given Initial Message", "Time Difference", "Triangle Probability", outputV2);

  //TFltPrV outputV3;
  //binProb(msgProbTR3, "binned_triCloseDayNewPEDGE", 5.0, 1.0, false,"Binned Time Difference vs Trade Close Prob Given Initial Message", "Time Difference", "Triangle Probability", outputV3);

  //TFltPrV outputV4;
  //binProb(msgProbTR4, "binned_triCloseDayFirstPEDGE", 5.0, 1.0, false,"Binned Time Difference vs Trade Close Prob Given Initial Message", "Time Difference", "Triangle Probability", outputV4);


  TGnuPlot ScGnuPlot("binned_triCloseDayOverlayOCT", "", false);
  ScGnuPlot.AddCmd("set key top right");
  ScGnuPlot.AddPlot(outputV, gpwLinesPoints, "Standard", "lt 1 pt 5 ps 3");
  //ScGnuPlot.AddPlot(outputV, gpwPoints, "Any Purchase", "pt 6");
  ScGnuPlot.AddPlot(outputV2, gpwLinesPoints, "FirstBuy Req", "lt 2 pt 7 ps 3");
  //ScGnuPlot.AddPlot(outputV2, gpwPoints, "First Purchase Only", "pt 6");
  //ScGnuPlot.AddPlot(outputV3, gpwLinesPoints, "Baseline: Any Purchase", "pt 6 smooth bezier");
  //ScGnuPlot.AddPlot(outputV3, gpwPoints, "Baseline: Any Purchase", "pt 6");
  //ScGnuPlot.AddPlot(outputV4, gpwLinesPoints, "Baseline: First Purchase Only", "pt 6 smooth bezier");
  //ScGnuPlot.AddPlot(outputV4, gpwPoints, "Baseline: First Purchase Only", "pt 6");
  ScGnuPlot.SetXYLabel("Time Difference", "Triangle Probability");
  ScGnuPlot.SetScale(gpsAuto);
  //ScGnuPlot.SavePng();
  ScGnuPlot.SaveEps("binned_triCloseDayOverlayOCT.eps", 30, false);
  */

  // triangleClosing() overlay of observed vs permutedEdges random
  
  // OCT 10: remove title, grid, same color, etc..
  
  TFltPrV randV; // random baseline
  for (int i=0; i<outputV.Len(); i++)
    randV.Add(TFltPr((float)outputV[i].Val1,0.00003));

  TGnuPlot ScGnuPlot("binned_triMsgVolProbAllOverlay60Dnew", "", false);
  ScGnuPlot.AddCmd("set key top left");
  ScGnuPlot.AddPlot(outputV, gpwLinesPoints, "Standard", "lt 1 pt 5 ps 3");
  //ScGnuPlot.AddPlot(outputV, gpwPoints, "Any Purchase", "lt 1 pt 6");
  ScGnuPlot.AddPlot(outputV2, gpwLinesPoints, "FirstBuy Req", "lt 2 pt 7 ps 3");
  //ScGnuPlot.AddPlot(randV, gpwLinesPoints, "Random", "lt 3 pt 9 ps 3");
  // ScGnuPlot.AddPlot(outputV2, gpwPoints, "First Purchase Only", "lt 2 pt 6");
  //ScGnuPlot.AddPlot(outputV3, gpwLinesPoints, "Baseline: Any Purchase", "pt 6 smooth bezier");
  //ScGnuPlot.AddPlot(outputV3, gpwPoints, "Baseline: Any Purchase", "pt 6");
  //ScGnuPlot.AddPlot(outputV4, gpwLinesPoints, "Baseline: First Purchase Only", "pt 6 smooth bezier");
  //ScGnuPlot.AddPlot(outputV4, gpwPoints, "Baseline: First Purchase Only", "pt 6");

  // default y label is triangle probability
  ScGnuPlot.SetXYLabel("Number of Exchanged Messages", "Purchase Probability"); 
  //ScGnuPlot.SetXRange(1,1025); // for binned_triMsgVolProbOverlay3dNew.eps
  ScGnuPlot.SetScale(gpsLog10X);
  //ScGnuPlot.SavePng();  
  ScGnuPlot.SaveEps("binned_triMsgVolProbAllOverlay60DNew.eps", 30, false);

  /*
  TFltPrV outputV3;
  binning(msgProbV3, "binned_triMsgVolMsgConProb", 1.0, 2.0, false, "Binned
  Message Strength vs Contact Close Prob Given Initial Message", "Message
  Strength", "Triangle Probability", outputV3);

  TGnuPlot tZGnuPlot("binned_triMsgVolMsgOverlay", "Binned Message Strength vs
  Triangle Probability Given Initial Message");
  tZGnuPlot.AddPlot(outputV, gpwLinesPoints, "Message Close", "pt 6");
  tZGnuPlot.AddPlot(outputV2, gpwLinesPoints, "Trade Close", "pt 6");
  tZGnuPlot.AddPlot(outputV3, gpwLinesPoints, "Contact Close", "pt 6");
  tZGnuPlot.SetXYLabel("Message Strength", "Triangle Close Probability");
  tZGnuPlot.SetScale(gpsAuto);
  tZGnuPlot.SavePng();
  */

  /*
  float randLevel = 0.0003215947; // baseline from triangle closing (update)
  TFltPrV randomV;
  for (int i=0; i<outputV2.Len(); i++) 
    randomV.Add(TFltPr((float)outputV2[i].Val1, randLevel));

  // new: add line indicating random
  TGnuPlot tZGnuPlot("binned_triMsgVolTradeMsgProb", "Binned Message Strength
  vs Message Close Prob Given Initial Trade");
  tZGnuPlot.AddPlot(outputV2, gpwLinesPoints, "Actual", "pt 6");
  tZGnuPlot.AddPlot(randomV, gpwLinesPoints, "Random", "pt 6");
  tZGnuPlot.SetXYLabel("Message Strength", "Triangle Probability");
  tZGnuPlot.SetScale(gpsAuto);
  tZGnuPlot.SavePng();
  */
}

void compareTriFiles() {
  int NUM = 57; // ignore last day of data
  int rand[NUM];
  int obsSuc[NUM];
  int obsTot[NUM];
  int a;
  
  FILE *in = fopen("triTemp.out", "r");
  for (int i=0; i<NUM; i++)
    fscanf(in, "%d %d", &a, &rand[i]);
  // bernoulli prob per day is rand / 1m
  fclose(in);

  FILE *in2 = fopen("triObserved.out", "r");
  for (int i=0; i<NUM; i++) {
    fscanf(in2, "%d %d %d", &a, &obsSuc[i], &obsTot[i]);
  }
  fclose(in2);

  double totSuc=0.0;
  double total=0.0;
  for (int i=0; i<NUM; i++) {
    totSuc += (double)obsSuc[i];
    total += (double)obsTot[i];
  }
  
  double randSuc=0.0;
  for (int i=0; i<NUM; i++)
    randSuc += (double)obsTot[i]*(double)rand[i]/1000000.0;

  printf("rand %f observed %f\n", randSuc/total, totSuc/total);
}

/* new: for the general msg case 
commented: moved this into the triangleClosing() function
*/

/*
void triangleClosingAll(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet x = t->shopNet; // the third leg of the triangle

  TIntH totMsg; // count of num msg strengths
  TIntH sucMsg; // count of num msg strengths leading to trades

  double success = 0.0;
  double total = 0.0;
  TIntH successByDay; // new: observed successes per window
  TIntH totalByDay;
  
  double baseline = 0.0;
  double numNodes = (double) s->GetNodes();

  int windowSize = 2;
  int daySec = 24*60*60;
  int window = windowSize*daySec;
 
  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();
  
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      TSecTm t = shopEdge.GetTime(0);
      
      if (m->IsNode(buyerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
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
            
            total += 1.0;
            int CT = chatTime.GetAbsSecs();
            if (totalByDay.IsKey(CT)) {
              int existCnt = totalByDay.GetDat(CT);
              totalByDay.AddDat(CT, existCnt+1);
            }
            else {
              totalByDay.AddDat(CT, 1);
              successByDay.AddDat(CT, 0);
            }
            
            // NEW check if theres a third edge within a window
            if (x->IsNode(msgID)) {
              TTaobaoShopNet::TNodeI NK = x->GetNI(msgID);
              int fnd = 0;
              for (int l=0; l<NK.GetOutDeg() && fnd==0; l++) {
                TTaobaoShopEdge& e = NK.GetOutEDat(l);
                for (int q=0; q<e.CntTX(); q++) {
                  TSecTm t2 = e.GetTime(q);
                  if (t2 >= chatTime) {
                    if (t2.GetAbsSecs() <= chatTime.GetAbsSecs()+window) {
                      fnd = 1; // once we find one, we quit the 2 for loops
                      // tracking by day
                      int existCnt = successByDay.GetDat(CT);
                      successByDay.AddDat(CT, existCnt+1);
                      // tracking by msg strength
                      existCnt = sucMsg.GetDat(msgCnt);
                      sucMsg.AddDat(msgCnt, existCnt+1);
                    }
                    break;
                  }
                }
              }
            }
            
          }
        
        }
      }
      
    }
      
  }
  
  FILE *out3 = fopen("triObserved.out", "w");
  for (int a=beginSec; a<=endSec; a+=daySec) {
    if (successByDay.IsKey(a)) {
      fprintf(out3, "%d ", (a-beginSec)/daySec);
      int y = (int) successByDay.GetDat(a);
      int z = (int) totalByDay.GetDat(a);
      fprintf(out3, "%d %d\n", y, z);
    }
  }
  fclose(out3);

  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);
  
  //TFltPrV msgProb;
  
  FILE *out = fopen("triMsgVolProbAll.outPEDGE", "w");
  
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
    //float tradeProb = (float)sucMsg[i] / (float)totMsg[i];
    //msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
    //fprintf(out, "%d %f\n", (int)totMsg.GetKey(i), tradeProb);
    fprintf(out, "%d %d %d\n", (int)totMsg.GetKey(i), (int)sucMsg[i], (int)totMsg[i]);
  }
  
  fclose(out);
    
  //printf("observed %.10lf %lf %lf baseline %.10lf\n", success/total, success, total, baseline/total);
}
*/

void plotTP() {
  TFltTrV prob;
  TFltPrV msgCnt, pplCnt;
  FILE *in = fopen("tradePass.out", "r");
  int price, success, total;
  
  while (fscanf(in, "%d %d %d\n", &price, &success, &total) == 3) {
    if (price < 1000)
      prob.Add(TFltTr((float)price, (float)success, (float)total));
  }
  fclose(in);

  FILE *in2 = fopen("tradePassMsg.out", "r");
  float pr, msg;
  while (fscanf(in2, "%f %f\n", &pr, &msg) == 2) {
    if (pr < 10000.0)
      msgCnt.Add(TFltPr(pr, msg));
  }
  fclose(in2);

  FILE *in3 = fopen("tradePassPpl.out", "r");
  float pr2, ppl;
  while (fscanf(in3, "%f %f\n", &pr2, &ppl) == 2) {
    if (pr2 < 10000.0)
      pplCnt.Add(TFltPr(pr2, ppl));
  }
  fclose(in3);


  TFltPrV outputV;
  binProb(prob, "binned_priceProb2", 1.0, 2.0, true, "", "Price", "Triangle Probability", outputV);

  /*
  TFltPrV outputV2;
  binning(msgCnt, "binned_priceMsg2", 1.0, 2.0, true, "Binned Price vs Msgs Sent", "Price", "Messages Sent", outputV2);

  TFltPrV outputV3;
  binning(pplCnt, "binned_pricePpl2", 1.0, 2.0, true, "Binned Price vs People Messaged", "Price", "People Messaged", outputV3);
  */

}

/* triangleClosing, graph price of 1st trade vs prob of 2nd trade 
   graphs price vs num msgs sent out
*/
void tradePassing(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message

  // just round prices, floating point hash not accurate

  // for third edge prob
  TIntH success;
  TIntH total;

  // for avg #msgs
  TIntH msgTot;
  TIntH msgCnt;

  // for avg #people msged
  TIntH pplTot;
  TIntH pplCnt;

  int windowSize = 2;
  int daySec = 24*60*60;
  int window = windowSize*daySec;
 
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    if (m->IsNode(buyerID)) {
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int sellerID = NI.GetOutNId(i);
        TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
        TTaobaoTX tx = shopEdge.GetTX(0);
        int salePrice = (int) tx.SalePrice;
        TSecTm tm = shopEdge.GetTime(0);
        int buyTime = tm.GetAbsSecs();

        if (pplCnt.IsKey(salePrice)) {
          int existCnt = pplCnt.GetDat(salePrice);
          pplCnt.AddDat(salePrice, existCnt+1);
        }
        else {
          pplCnt.AddDat(salePrice, 1);
          pplTot.AddDat(salePrice, 0);
        }

        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
        // cnt msged ppl, only cnt first-timers for simplicity
        int gdCnt=0;
        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);
          int msgTime = msgEdge.GetTime(0).GetAbsSecs();
          if (msgTime>=buyTime && msgTime<=buyTime+window)
            gdCnt++;
        }
        if (gdCnt > 0) {
          int existCnt = pplTot.GetDat(salePrice);
          pplTot.AddDat(salePrice, existCnt+gdCnt);
        }

        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);


          for (int k=0; k<msgEdge.CntTX(); k++) { // can also restrict to first msg
            int msgTime = msgEdge.GetTime(k).GetAbsSecs();

            if (k==0 && msgTime>=buyTime && msgTime<=buyTime+window) {
              if (msgTot.IsKey(salePrice)) {
                int existCnt = msgTot.GetDat(salePrice);
                msgTot.AddDat(salePrice, existCnt+(int)msgEdge.GetStrength(k));
                existCnt = msgCnt.GetDat(salePrice);
                msgCnt.AddDat(salePrice, existCnt+1);
              }
              else {
                msgTot.AddDat(salePrice, (int)msgEdge.GetStrength(k));
                msgCnt.AddDat(salePrice, 1);
              }
            }

            if (msgTime >= buyTime) { // once we enter this loop, we break
              if (msgTime<=buyTime+window) {
               
                if (total.IsKey(salePrice)) { 
                  int existCnt = total.GetDat(salePrice);
                  total.AddDat(salePrice, existCnt+1);
                  //printf("add %d %d\n", salePrice, existCnt+1);
                }
                else {
                  //printf("new %d\n", salePrice);
                  success.AddDat(salePrice, 0);
                  total.AddDat(salePrice, 1);
                }

                if (s->IsNode(msgID) && s->IsEdge(msgID, sellerID)) {
                  TTaobaoShopEdge& shopEdge2 = s->GetEDat(msgID, sellerID);
                  TTaobaoTX tx2 = shopEdge2.GetTX(0);
                  int salePrice2 = (int) tx2.SalePrice;
                  if (salePrice<=1.05*salePrice2 && salePrice2<=1.05*salePrice) {
                    // only consider first purchase
                    int buy2Time = shopEdge2.GetTime(0).GetAbsSecs();
                    if (buy2Time >= msgTime) {
                      // successful

                      if (success.IsKey(salePrice)) {
                        //printf("success %d\n", salePrice);
                        int existCnt = success.GetDat(salePrice);
                        success.AddDat(salePrice, existCnt+1);
                      }
                      else {
                        printf("error\n");
                      }
                                           
                      break;
                    }
                  }
                  
                }
             
              }
              break;
            }

          } // end for loop
          
        }
        
      }
    }
  }

  printf("%d %d\n", success.Len(), total.Len());
  
  success.SortByKey(true);
  total.SortByKey(true);
  msgTot.SortByKey(true);
  msgCnt.SortByKey(true);
  pplTot.SortByKey(true);
  pplCnt.SortByKey(true);

  if (success.Len() != total.Len())
    printf(" length error \n");

  TFltTrV prob;
  FILE *out = fopen("tradePass.out", "w");
  for (int i=0; i<success.Len(); i++) {
    fprintf(out, "%d %d %d\n", (int)success.GetKey(i), (int)success[i], (int)total[i]);
    prob.Add(TFltTr((float)success.GetKey(i), (float)success[i], (float)total[i]));
  }
  fclose(out);

  TFltPrV avgMsg;
  FILE *out2 = fopen("tradePassMsg.out", "w");
  for (int i=0; i<msgCnt.Len(); i++) {
    fprintf(out2, "%f %f\n", (float)msgCnt.GetKey(i), (float)msgTot[i]/(float)msgCnt[i]);
    avgMsg.Add(TFltPr((float)msgCnt.GetKey(i), (float)msgTot[i]/(float)msgCnt[i]));
  }
  fclose(out2);

  TFltPrV avgPpl;
  FILE *out3 = fopen("tradePassPpl.out", "w");
  for (int i=0; i<pplCnt.Len(); i++) {
    fprintf(out3, "%f %f\n", (float)pplCnt.GetKey(i), (float)pplTot[i]/(float)pplCnt[i]);
    avgPpl.Add(TFltPr((float)pplCnt.GetKey(i), (float)pplTot[i]/(float)pplCnt[i]));
  }
  fclose(out3);


  TFltPrV outputV;
  binProb(prob, "binned_priceProb", 1.0, 2.0, false, "Binned Price vs Trade Close Prob", "Price", "Triangle Probability", outputV);

  TFltPrV outputV2;
  binning(avgMsg, "binned_priceMsg", 1.0, 2.0, false, "Binned Price vs Messages Sent", "Price", "Messages Sent", outputV2);

  TFltPrV outputV3;
  binning(avgPpl, "binned_pricePpl", 1.0, 2.0, false, "Binned Price vs People Messaged", "Price", "People Messaged", outputV3);

}

/*
  default triangle function
  buy, msg/contact, buy?
*/

void triangleClosing(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet x = t->shopNet; // the third leg of the triangle

  TIntH totMsg; // count of num msg strengths
  TIntH sucMsg; // count of num msg strengths leading to trades
  TIntH sucFirstMsg; // with more stringent requirement of first trade
  int totDay[60]; // total/success by dif between first 2 edges
  int sucDay[60];
  int sucFirstDay[60]; // with more stringent req that 3rd edge is first time trading

  double success = 0.0;
  double successFirst = 0.0; // more stringent
  double total = 0.0;
  double baseline = 0.0;
  double baselineFirst = 0.0;
  double numNodes = (double) s->GetNodes();

  printf("%d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges());

  int windowSize = 2;
  int daySec = 24*60*60;
  int window = windowSize*daySec;

  // count sum msg strengths in [-w/2, w/2] around the first trade 
  int msgWindowSize=35; // half msg window size
  int msgWindow = msgWindowSize*daySec;

  //TIntH nodesByWin[60];
  //TIntIntHH nodesByWin; // nodes pointed to in window beginning on this day (x network)
  // use linear pass to fill in this struct,
  // for each edge time, check up to W back 

  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();

  for (int i=0; i<60; i++) {
    totDay[i] = 0;
    sucDay[i] = 0;
    sucFirstDay[i] = 0;
  }

  // this section computes RAND
  /*

  for (int a=beginSec; a<=endSec; a+=daySec) {
    TIntH y; // hopefully this copy persists
    //x = new TIntH();
    nodesByWin.AddDat(a, y);
  }

  int cnt=0;
  // fill in struct with third network msg by day
  for (TTaobaoShopNet::TNodeI NI = x->BegNI(); NI < x->EndNI(); NI++) {
    for (int i=0; i<NI.GetOutDeg(); i++) {
      TTaobaoShopEdge& edge = NI.GetOutEDat(i);
      int sinkID = NI.GetOutNId(i);

      for (int j=0; j<edge.CntTX(); j++) {
        int xSec = edge.GetTime(j).GetAbsSecs();
        if (cnt%100000 == 0)
          printf("%d %d %d\n", xSec, cnt, nodesByWin.Len());
        // mark off window+1 days
        for (int a=xSec; a>=xSec-window && a>=beginSec; a-=daySec) { 
          TIntH y = (TIntH) nodesByWin.GetDat(a);
          if (!y.IsKey(sinkID)) {
            y.AddDat(sinkID);
            nodesByWin.AddDat(a, y);
          }
        }
        cnt++;
      }
      
    }
  }

  FILE *out3 = fopen("triTemp.out", "w");
  for (int a=beginSec; a<=endSec; a+=daySec) {
    if (nodesByWin.IsKey(a)) {
      printf("%d ", (a-beginSec)/daySec);
      fprintf(out3, "%d ", (a-beginSec)/daySec);
      TIntH y = (TIntH) nodesByWin.GetDat(a);
      printf("%d\n", y.Len());
      fprintf(out3, "%d\n", y.Len());
    }
  }
  fclose(out3);

  printf("%lf\n", numNodes);
  return; // compute generalized function
  */

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      TSecTm tm = shopEdge.GetTime(0);
      int tmSec = tm.GetAbsSecs();

      if (m->IsNode(buyerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
        int candMsg = 0;
        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);

          int msgCnt=0;
          int k=0; 
          while (k < msgEdge.CntTX()) {
            int tDif = tmSec-msgEdge.GetTime(k).GetAbsSecs();
            if (tDif <= 0)
              break;
            if (tDif <= msgWindow) // add all <= w/2 before the trade
              msgCnt += (int)msgEdge.GetStrength(k);
            k++; // increment when past trade
          }
          
          int q = k; // add all <= w/2 after the trade
          while (q < msgEdge.CntTX()) {
            int tDif = msgEdge.GetTime(q).GetAbsSecs()-tmSec;
            if (tDif <= msgWindow)
              msgCnt += (int)msgEdge.GetStrength(q);
            else
              break;
            q++;
          }

          if (k<msgEdge.CntTX()) { // use first message time
            TSecTm chatTime = msgEdge.GetTime(k);
            //msgCnt = msgEdge.CntTXStrength(); // TEMP comment out later
  
            // APR: changed msgCnt to sum of strengths for all messages within a
            // time window

            int dayDif = (chatTime.GetAbsSecs()-tm.GetAbsSecs())/daySec;

            if (totMsg.IsKey(msgCnt)) {
              int existCnt = totMsg.GetDat(msgCnt);
              totMsg.AddDat(msgCnt, existCnt+1);
            }
            else {
              totMsg.AddDat(msgCnt, 1);
              sucMsg.AddDat(msgCnt, 0);
              sucFirstMsg.AddDat(msgCnt, 0);
            }

            totDay[dayDif]++;

            candMsg++;
            total += 1.0;

            // default triangleClosing(), uncommented by default
            // check if at least one in the window
            
            if (x->IsEdge(msgID, sellerID)) {
              TTaobaoShopEdge& closeEdge = x->GetEDat(msgID, sellerID);
              for (int l=0; l<closeEdge.CntTX(); l++) {
                TSecTm tm2 = closeEdge.GetTime(l);
                if (tm2 >= chatTime) {
                  if (chatTime.GetAbsSecs()+window >= tm2.GetAbsSecs()) {
                    success += 1.0;
                    int existCnt = sucMsg.GetDat(msgCnt);
                    sucMsg.AddDat(msgCnt, existCnt+1);
                    sucDay[dayDif]++;
                    if (l==0) {
                      successFirst += 1.0;
                      existCnt = sucFirstMsg.GetDat(msgCnt);
                      sucFirstMsg.AddDat(msgCnt, existCnt+1);
                      sucFirstDay[dayDif]++;
                    }
                  }
                  break;
                }
              }
            }
            

            // final block from triangleClosingAll()
            // check if messaged buys from anybody
            // comment out this block if using triangleClosing() above
            /*
            if (x->IsNode(msgID)) {
              TTaobaoShopNet::TNodeI NK = x->GetNI(msgID);
              int fnd = 0;
              int fndFirst=0;
       
              // quick check with the extra first buyer requirement
              for (int l=0; l<NK.GetOutDeg() && fndFirst==0; l++) {
                TTaobaoShopEdge& e = NK.GetOutEDat(l);
                for (int r=0; r<1; r++) { // only check first
                  TSecTm tm2 = e.GetTime(r);
                  if (tm2 >= chatTime) {
                    if (tm2.GetAbsSecs() <= chatTime.GetAbsSecs()+window) {
                      fndFirst = 1;
                      successFirst += 1.0;
                      int existCnt = sucFirstMsg.GetDat(msgCnt);
                      sucFirstMsg.AddDat(msgCnt, existCnt+1);
                      sucFirstDay[dayDif]++;
                    }
                    break;
                  }
                }
              }

              if (fndFirst == 1) { // since its a special case
                success += 1.0;
                int existCnt = sucMsg.GetDat(msgCnt);
                sucMsg.AddDat(msgCnt, existCnt+1);
                sucDay[dayDif]++;
              }
              else { // have to go through and check
                for (int l=0; l<NK.GetOutDeg() && fnd==0; l++) {
                  TTaobaoShopEdge& e = NK.GetOutEDat(l);
                  for (int r=0; r<e.CntTX(); r++) {
                    TSecTm tm2 = e.GetTime(r);
                    if (tm2 >= chatTime) {
                      if (tm2.GetAbsSecs() <= chatTime.GetAbsSecs()+window) {
                        fnd = 1; // once we find one, we quit the 2 for loops
                        success += 1.0;
                        int existCnt = sucMsg.GetDat(msgCnt);
                        sucMsg.AddDat(msgCnt, existCnt+1);
                        sucDay[dayDif]++;
                      }
                      break;
                    }
                  }
                }
              }
            }
            // END triangleClosingAll() block
            */

            // update baseline (applies only to triangleClosing())
            TTaobaoShopNet::TNodeI NK = x->GetNI(sellerID);
            int num=0;
            int numFirst=0; // more stringent
            for (int l=0; l<NK.GetInDeg(); l++) {
              TTaobaoShopEdge& edge = NK.GetInEDat(l);              
              int fnd=0;
              int fndFirst=0;
              for (int a=0; a<edge.CntTX(); a++) {
                TSecTm tm3 = edge.GetTime(a);
                if (tm3 >= chatTime) {
                  if (chatTime.GetAbsSecs()+window >= tm3.GetAbsSecs()) {
                    fnd=1;
                    if (a==0)
                      fndFirst=1;
                  }
                  break;
                }
              }

              if (fnd == 1)
                num++; 
              if (fndFirst == 1)
                numFirst++;
            }
            baseline += (double) ((double)num/(double)numNodes);
            baselineFirst += (double) ((double)numFirst/(double)numNodes);
          }
        }

        // old baseline calculation (without window)
        /*
          if (x->IsNode(sellerID)) {
          TTaobaoShopNet::TNodeI NK = x->GetNI(sellerID);
          baseline += (double) ((double)NK.GetInDeg()/numNodes * (double) candMsg);
          // mar: for window, this is wrong. need to check all timing of each indeg
        }
        */
        
      }
      
    }
  }
  
  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);
  sucFirstMsg.SortByKey(true);

  //TFltPrV msgProb;
  
  FILE *out = fopen("triMsgVolProbNew35D.out", "w");
  FILE *out3 = fopen("triMsgVolProbFirst35D.out", "w");

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
    //float tradeProb = (float)sucMsg[i] / (float)totMsg[i];

    //msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
    fprintf(out, "%d %d %d\n", (int)totMsg.GetKey(i), (int)sucMsg[i], (int)totMsg[i]);
    fprintf(out3, "%d %d %d\n", (int)totMsg.GetKey(i), (int)sucFirstMsg[i], (int)totMsg[i]);
    //fprintf(out, "%d %f\n", (int)totMsg.GetKey(i), tradeProb);
  }
  
  fclose(out);
  fclose(out3);
  
  // only 58 days, should only be from 0 to 57, but more show up

  FILE *out2 = fopen("triCloseDayNew35D.out", "w");
  for (int i=0; i<60; i++)
    fprintf(out2, "%d %d %d\n", i, sucDay[i], totDay[i]);
  fclose(out2);
  
  FILE *out4 = fopen("triCloseDayFirst35D.out", "w");
  for (int i=0; i<60; i++)
    fprintf(out4, "%d %d %d\n", i, sucFirstDay[i], totDay[i]);
  fclose(out4);


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

  // mar: compare nums to top of function, make sure x doesn't screw up graph
  printf("%d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges());

  
  printf("observed %.10lf %lf %lf baseline %.10lf\n", success/total, success, total, baseline/total);
  printf("stringent %.10lf %lf %lf baseline %.10lf\n", successFirst/total,successFirst, total, baselineFirst/total);
}

/* IGNORE NOT DOING THIS FOR NOW: going to track messages to sellers */
void EventSequencesDetailed(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; 
  PTaobaoShopNet c = t->contactNet;
  
  int windowSize = 0; // use same day req for now
  int daySec = 24*60*60;
  int window = windowSize*daySec;
  TIntH nodesByWin[60];
  // array of (buyer, msgcnttoanyseller) by day
  
  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();

  int cnt=0;
 
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI<s->EndNI(); NI++)
    if (NI.GetInDeg() > NI.GetOutDeg()) { // we'll say this is a "seller"
      int sellerID = NI.GetId();
      if (m->IsNode(sellerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(sellerID);
        for (int i=0; i<NJ.GetInDeg(); i++) {
          TTaobaoShopEdge& edge = NJ.GetInEDat(i);
          int buyerID = NJ.GetInNId(i);
          
          for (int j=0; j<edge.CntTX(); j++) {
            int xSec = edge.GetTime(j).GetAbsSecs();
            if (cnt%100000 == 0)
              printf("%d %d %d\n", xSec, cnt, nodesByWin[(xSec-beginSec)/daySec].Len());
            // mark off window+1 days
            int aMax = (xSec-beginSec)/daySec;
            int aMin;
            if (xSec-window>=beginSec)
              aMin = (xSec-window-beginSec)/daySec;
            else
              aMin = 0;
            for (int a=aMax; a>=aMin; a--) {
              if (!nodesByWin[a].IsKey(buyerID)) {
                //printf("adding %d ", buyerID);
                nodesByWin[a].AddDat(buyerID, 1); 
              }
              else {
                int existCnt = nodesByWin[a].GetDat(buyerID);
                nodesByWin[a].AddDat(buyerID, existCnt+1);
              }
            }
            cnt++;
          }
        }
      }
    }
  
  // output daily arrays into a temp file
  // load it for later use in other functions

  FILE *out = fopen("eventDetailed.out", "w");
  for (int a=beginSec; a<=endSec; a+=daySec) {
    int ind = (a-beginSec)/daySec;
    printf("%d %d\n", ind, nodesByWin[ind].Len());
    fprintf(out, "%d %d\n", ind, nodesByWin[ind].Len());
  }

  for (int a=beginSec; a<=endSec; a+=daySec) {
    int ind = (a-beginSec)/daySec;
    for (int i=0; i<nodesByWin[ind].Len(); i++)
      fprintf(out, "%d %d ", (int)(nodesByWin[ind]).GetKey(i), (int)(nodesByWin[ind])[i]);
    fprintf(out, "\n");
  }
  fclose(out);

}

/* given two purchases, graph price vs time between first msg between and
   second purchase */
void EventTimePrice(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  int daySec = 24*60*60;
  TFltPrV timePrice;
  TFltPrV priceTime;

  FILE *out = fopen("timePrice.out", "w");
  
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
          TTaobaoTX tx1, tx2;

          if (t1 > t2) {
            TSecTm ct = t1;
            t1 = t2;
            t2 = ct;
            n1 = NI.GetInNId(j);
            n2 = NI.GetInNId(i);
            tx1 = shopEdge2.GetTX(0);
            tx2 = shopEdge.GetTX(0);
          }
          else {
            tx1 = shopEdge.GetTX(0);
            tx2 = shopEdge2.GetTX(0);
          }

          if (tx1.SalePrice > tx2.SalePrice*1.05 || tx2.SalePrice>tx1.SalePrice*1.05)
            continue;

          float avgPrice = (tx1.SalePrice+tx2.SalePrice)/2.0;

          // dont have buy and contact restrictions
          if (m->IsNode(n1) && m->IsNode(n2) && m->IsEdge(n1, n2)) {
            TTaobaoShopEdge& msgEdge = m->GetEDat(n1, n2);
            for (int k=0; k<msgEdge.CntTX(); k++) {
              TSecTm mT = msgEdge.GetTime(k);
              if (mT > t2)
                break;
              if (mT>=t1 && mT<=t2) {
                int daySpan = (t2.GetAbsSecs()-mT.GetAbsSecs())/daySec+1;
                timePrice.Add(TFltPr((float)daySpan, avgPrice));
                priceTime.Add(TFltPr(avgPrice,(float)daySpan));
                fprintf(out, "%d %f\n", daySpan, avgPrice);
                break;
              }
            }
          }
        }
      }
    }
  }
  fclose(out);

  printf("total %d\n", timePrice.Len());
  
  TFltPrV outputV;
  binning(timePrice, "timePrice", 1.0, 1.0, false, "Response Time vs Price", "Response Time", "Price", outputV);
  binning(priceTime, "binned_priceTime", 0.1, 2.0, false, "Price vs Response Time", "Price", "Response Time", outputV);
}

/* May: added buyer->seller counting */
void EventSequences(PTaobaoNet t) {
  /*
    for each seller, for each pair of buyers, check if a message was exchanged
  before, between, and after the two purchases. Baseline: count the numbers of
  days in
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
  // DEFAULT 58, 2009 10 28
  int totalDays = 57;
  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 27);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();
  int daySec = 24*60*60;

  int total = 0;

  // general count
  double netBeforeDays=0.0;
  double netBetweenDays=0.0;
  double netAfterDays=0.0;
  // may10: count of days in which they message 
  double netBeforeMsgDays=0.0;
  double netBetweenMsgDays=0.0;
  double netAfterMsgDays=0.0;

  // may10: consider total for the first msg day after a trade
  double firstBetweenDays=0.0;
  double firstBetweenMsg=0.0;

  double netBeforeMsg=0.0;
  double netBetweenMsg=0.0;
  double netAfterMsg=0.0;

  double totalMsg = 0.0;

  // may: equal window size counting
  int maxWin = 19;
  // buyer1 -> buyer2
  double winMsg[maxWin+1][3]; // before, between, after
  double winCnt[maxWin+1];
  // buyer1 -> seller
  double win1Msg[maxWin+1][3];
  double win1Cnt[maxWin+1];
  // buyer2 -> seller
  double win2Msg[maxWin+1][3];
  double win2Cnt[maxWin+1];

  for (int i=1; i<=maxWin; i++) {
    winMsg[i][0] = 0.0;
    winMsg[i][1] = 0.0;
    winMsg[i][2] = 0.0;
    winCnt[i] = 0.0;
    win1Msg[i][0] = 0.0;
    win1Msg[i][1] = 0.0;
    win1Msg[i][2] = 0.0;
    win1Cnt[i] = 0.0;
    win2Msg[i][0] = 0.0;
    win2Msg[i][1] = 0.0;
    win2Msg[i][2] = 0.0;
    win2Cnt[i] = 0.0;
  }

  // may10: variables from n1 to seller
  double net1BeforeDays=0.0;
  double net1BetweenDays=0.0;
  double net1AfterDays=0.0;
  double net1BeforeMsgDays=0.0;
  double net1BetweenMsgDays=0.0;
  double net1AfterMsgDays=0.0;

  double net1BeforeMsg=0.0;
  double net1BetweenMsg=0.0;
  double net1AfterMsg=0.0;

  double net1TradeDayMsg=0.0;
  double net1TradeDayCnt=0.0;
  
  // variables from n2 to seller

  double net2BeforeDays=0.0;
  double net2BetweenDays=0.0;
  double net2AfterDays=0.0;
  double net2BeforeMsgDays=0.0;
  double net2BetweenMsgDays=0.0;
  double net2AfterMsgDays=0.0;

  double net2BeforeMsg=0.0;
  double net2BetweenMsg=0.0;
  double net2AfterMsg=0.0;

  double net2TradeDayMsg=0.0;
  double net2TradeDayCnt=0.0;

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
          TTaobaoTX tx1, tx2;

          if (t1 > t2) {
            TSecTm c = t1;
            t1 = t2;
            t2 = c;
            n1 = NI.GetInNId(j);
            n2 = NI.GetInNId(i);
            tx1 = shopEdge2.GetTX(0);
            tx2 = shopEdge.GetTX(0);
          }
          else {
            tx1 = shopEdge.GetTX(0);
            tx2 = shopEdge2.GetTX(0);
          }

          // artificial: check what happens for expensive stuff
          
          // only focus on similar priced things
          if (tx1.SalePrice > tx2.SalePrice*1.05 || tx2.SalePrice>tx1.SalePrice*1.05)
            continue;

          float avgPrice = (tx1.SalePrice+tx2.SalePrice)/2.0;
          if (avgPrice > 1.0)
            continue;

          // assume that each region is [begin, end), except for the last one
          
          int beforeDays = (t1.GetAbsSecs()-beginSec)/daySec; 
          if (beforeDays > totalDays) {
            //printf("ERROR t1 %s dif %d\n", t1.GetStr().CStr(), beforeDays);
            //exit(0);
            continue;
          }
          int betweenDays = (t2.GetAbsSecs()-t1.GetAbsSecs())/daySec+1; //may +1 
          if (betweenDays > totalDays) {
            //printf("ERROR t1 %s t2 %s dif %d\n", t1.GetStr().CStr(), t2.GetStr().CStr(), betweenDays);
            //exit(0);
            continue;
          }
          
          if (t2.GetAbsSecs()>endSec) // may: new, allow for decreasing last day
            break;
          int afterDays = (endSec-t2.GetAbsSecs())/daySec; // rem +1
          if (afterDays > totalDays) {
            //printf("ERROR t2 %s dif %d\n", t2.GetStr().CStr(), afterDays); 
            //exit(0);
            continue;
          }

          if (beforeDays+betweenDays+afterDays != totalDays) {
            printf("days %d %d %d\n", beforeDays, betweenDays, afterDays);
            printf("%s %s\n", t1.GetStr().CStr(), t2.GetStr().CStr());
            exit(0);
          }

          int winBool = 0;
          int tradeSpan = (t2.GetAbsSecs()-t1.GetAbsSecs())/daySec+1;
          int beginWin = t1.GetAbsSecs()-tradeSpan*daySec;
          int endWin = t2.GetAbsSecs()+tradeSpan*daySec;
          if (tradeSpan<=maxWin && beforeDays>=tradeSpan && afterDays>=tradeSpan)
            winBool = 1;          

          // commented out blocks are for at least one msg in an interval
          // trying out expectations
          int foundBefore = 0;
          int foundBetween = 0;
          int foundAfter = 0;
         
          // may10: count exact days in which they message, ignore no-message days
          int beforeMsgDays = 0;
          int betweenMsgDays = 0;
          int afterMsgDays = 0;

          if (m->IsNode(n1) && m->IsNode(n2)) {
            // default c->IsEdge. change back soon
            // MAY temp: try edge from n2 to n1 
            if (m->IsEdge(n1, n2) && c->IsEdge(n1,n2) && !s->IsEdge(n1,n2) && !s->IsEdge(n2,n1)) { 

              if (winBool == 1)
                winCnt[tradeSpan] += 1.0;

              // mar: additional chat req, also they cannot buy from each other
              //total++; //not exactly, check if before day1
              TTaobaoShopEdge& msgEdge = m->GetEDat(n1, n2);
              for (int k=0; k<msgEdge.CntTX(); k++) {
                TSecTm mT = msgEdge.GetTime(k);
                if (mT >=intervalBegin && mT < t1) { // feb new: added >= req
                  // for con
                  //foundBefore = 1;
                  //foundBefore++; // change to add strength
                  beforeMsgDays++;
                  foundBefore += (int)msgEdge.GetStrength(k);
                  totalMsg += (double)msgEdge.GetStrength(k);

                  if (winBool==1 && mT.GetAbsSecs()>=beginWin)
                    winMsg[tradeSpan][0] += (double)msgEdge.GetStrength(k);
                }
                else if (mT>=t1 && mT<=t2) { // changed >= to >
                  //foundBetween = 1;
                  //foundBetween++;

                  if (betweenMsgDays == 0) {
                    firstBetweenDays += 1.0;
                    firstBetweenMsg += (double)msgEdge.GetStrength(k);
                  }

                  betweenMsgDays++;
                  foundBetween += (int)msgEdge.GetStrength(k);
                  totalMsg += (double)msgEdge.GetStrength(k);

                  if (winBool==1)
                    winMsg[tradeSpan][1] += (double)msgEdge.GetStrength(k);
                }
                else if (mT>t2 && mT <=intervalEnd) { // allow var end
                  //foundAfter = 1;
                  //foundAfter++;
                  afterMsgDays++;
                  foundAfter += (int)msgEdge.GetStrength(k);
                  totalMsg += (double)msgEdge.GetStrength(k);

                  if (winBool==1 && mT.GetAbsSecs()<=endWin)
                    winMsg[tradeSpan][2] += (double)msgEdge.GetStrength(k);
                }
              }   
              
              // if to handle case where only have invalid contacts, eg no edge
              if (foundBefore+foundBetween+foundAfter != 0) { 
                total++;
                
                netBeforeDays += (double) beforeDays;
                netBetweenDays += (double) betweenDays;
                netAfterDays += (double) afterDays;
                
                netBeforeMsgDays += (double) beforeMsgDays;
                netBetweenMsgDays += (double) betweenMsgDays;
                netAfterMsgDays += (double) afterMsgDays;

                netBeforeMsg += (double) foundBefore;
                netBetweenMsg += (double) foundBetween;
                netAfterMsg += (double) foundAfter;
              }

              // may10: for each of these cases, also look at distributions of
              // msgs to seller
              
              if (m->IsEdge(n1, sellerID)) {
                int n1foundBefore = 0;
                int n1foundBetween = 0;
                int n1foundAfter = 0;

                int n1beforeMsgDays = 0;
                int n1betweenMsgDays = 0;
                int n1afterMsgDays = 0;

                if (winBool == 1)
                  win1Cnt[tradeSpan] += 1.0;

                TTaobaoShopEdge& msgEdge = m->GetEDat(n1, sellerID);
                for (int k=0; k<msgEdge.CntTX(); k++) {
                  TSecTm mT = msgEdge.GetTime(k);
                  if (mT >=intervalBegin && mT < t1) { // feb new: added >= req
                    // for con
                    //foundBefore = 1;
                    //foundBefore++; // change to add strength
                    n1beforeMsgDays++;
                    n1foundBefore += (int)msgEdge.GetStrength(k);
                    //totalMsg += (double)msgEdge.GetStrength(k);

                    if (winBool==1 && mT.GetAbsSecs()>=beginWin)
                      win1Msg[tradeSpan][0] += (double)msgEdge.GetStrength(k);
                  }
                  else if (mT>=t1 && mT<=t2) { // changed >= to >
                    //foundBetween = 1;
                    //foundBetween++;
                  
                    //if (mT == t1) { // temp: special case day of trades
                    // net1TradeDayCnt += 1.0;
                    // net1TradeDayMsg += msgEdge.GetStrength(k);
                    //}
                    //else {
                      n1betweenMsgDays++;
                      n1foundBetween += (int)msgEdge.GetStrength(k);
                      //}
                    //totalMsg += (double)msgEdge.GetStrength(k);

                      if (winBool==1)
                        win1Msg[tradeSpan][1] += (double)msgEdge.GetStrength(k);
                  }
                  else if (mT>t2 && mT <=intervalEnd) { // allow var end
                    //foundAfter = 1;
                    //foundAfter++;
                    n1afterMsgDays++;
                    n1foundAfter += (int)msgEdge.GetStrength(k);
                    //totalMsg += (double)msgEdge.GetStrength(k);
                  
                    if (winBool==1 && mT.GetAbsSecs()<=endWin)
                      win1Msg[tradeSpan][2] += (double)msgEdge.GetStrength(k);
                  } 
                }
                
                if (n1foundBefore+n1foundBetween+n1foundAfter > 0) {
                  // this block is the same for all cases
                  net1BeforeDays += (double) beforeDays;
                  net1BetweenDays += (double) betweenDays;
                  net1AfterDays += (double) afterDays;
                  
                  net1BeforeMsgDays += (double) n1beforeMsgDays;
                  net1BetweenMsgDays += (double) n1betweenMsgDays;
                  net1AfterMsgDays += (double) n1afterMsgDays;
                  
                  net1BeforeMsg += (double) n1foundBefore;
                  net1BetweenMsg += (double) n1foundBetween;
                  net1AfterMsg += (double) n1foundAfter;
                }
              }

              if (m->IsEdge(n2, sellerID)) {
                int n2foundBefore = 0;
                int n2foundBetween = 0;
                int n2foundAfter = 0;

                int n2beforeMsgDays = 0;
                int n2betweenMsgDays = 0;
                int n2afterMsgDays = 0;

                if (winBool == 1)
                  win2Cnt[tradeSpan] += 1.0;

                TTaobaoShopEdge& msgEdge = m->GetEDat(n2, sellerID);
                for (int k=0; k<msgEdge.CntTX(); k++) {
                  TSecTm mT = msgEdge.GetTime(k);
                  if (mT >=intervalBegin && mT < t1) { // feb new: added >= req
                    // for con
                    //foundBefore = 1;
                    //foundBefore++; // change to add strength
                    n2beforeMsgDays++;
                    n2foundBefore += (int)msgEdge.GetStrength(k);
                    //totalMsg += (double)msgEdge.GetStrength(k);

                    if (winBool==1 && mT.GetAbsSecs()>=beginWin)
                      win2Msg[tradeSpan][0] += (double)msgEdge.GetStrength(k);
                  }
                  else if (mT>=t1 && mT<=t2) { // changed >= to >
                    //foundBetween = 1;
                    //foundBetween++;
                    n2betweenMsgDays++;
                    n2foundBetween += (int)msgEdge.GetStrength(k);
                    //totalMsg += (double)msgEdge.GetStrength(k);
                 
                    if (winBool==1)
                      win2Msg[tradeSpan][1] += (double)msgEdge.GetStrength(k);
                  }
                  else if (mT>t2 && mT <=intervalEnd) { // allow var end
                    //foundAfter = 1;
                    //foundAfter++;
                    //if (mT == t2) { // temp: separate day of trades
                    //  net2TradeDayCnt += 1.0;
                    //  net2TradeDayMsg += msgEdge.GetStrength(k);
                    //}
                    //else {
                      n2afterMsgDays++;
                      n2foundAfter += (int)msgEdge.GetStrength(k);
                      //}
                    //totalMsg += (double)msgEdge.GetStrength(k);
                  
                      if (winBool==1 && mT.GetAbsSecs()<=endWin)
                        win2Msg[tradeSpan][2] += (double)msgEdge.GetStrength(k);
                  } 
                }
               
                if (n2foundBefore+n2foundBetween+n2foundAfter > 0) {
                  // this blcok is the same for all cases
                  net2BeforeDays += (double) beforeDays;
                  net2BetweenDays += (double) betweenDays;
                  net2AfterDays += (double) afterDays;
                  
                  net2BeforeMsgDays += (double) n2beforeMsgDays;
                  net2BetweenMsgDays += (double) n2betweenMsgDays;
                  net2AfterMsgDays += (double) n2afterMsgDays;
                  
                  net2BeforeMsg += (double) n2foundBefore;
                  net2BetweenMsg += (double) n2foundBetween;
                  net2AfterMsg += (double) n2foundAfter;
                }
              }

            }
            
          }
        }
      }

    }
  }

   double avgMsg = (double) totalMsg/((double)total*(double)totalDays);

  printf("b1-b2 StdAvg before:%.10lf between:%.10lf after:%.10lf\n",
         (double)netBeforeMsg/(double)netBeforeDays,
         (double)netBetweenMsg/(double)netBetweenDays,
         (double)netAfterMsg/(double)netAfterDays);
  // averaging only over nonzero msg days
  printf("b1-b2 NonZeroAvg before:%.10lf between:%.10lf after:%.10lf\n",
         (double)netBeforeMsg/(double)netBeforeMsgDays,
         (double)netBetweenMsg/(double)netBetweenMsgDays,
         (double)netAfterMsg/(double)netAfterMsgDays);
  printf("first between:%.10lf msgs:%lf days:%lf\n", firstBetweenMsg/firstBetweenDays, firstBetweenMsg, firstBetweenDays);
  printf("RANDOM: %.10lf\n", avgMsg);
  printf("totalDays:%lf %lf %lf %lf totalMsg:%lf %lf %lf %lf\n",
         (double)total*(double)totalDays, netBeforeDays, netBetweenDays,
         netAfterDays, totalMsg, netBeforeMsg, netBetweenMsg, netAfterMsg);

  // N1 to seller
  printf("b1-s StdAvg before:%.10lf between:%.10lf after:%.10lf\n",
         (double)net1BeforeMsg/(double)net1BeforeDays,
         (double)net1BetweenMsg/(double)net1BetweenDays,
         (double)net1AfterMsg/(double)net1AfterDays);
  // averaging only over nonzero msg days
  printf("b1-s NonZeroAvg before:%.10lf between:%.10lf after:%.10lf\n",
         (double)net1BeforeMsg/(double)net1BeforeMsgDays,
         (double)net1BetweenMsg/(double)net1BetweenMsgDays,
         (double)net1AfterMsg/(double)net1AfterMsgDays);
  //printf("day of:%.10lf msgs:%lf days:%lf\n", net1TradeDayMsg/net1TradeDayCnt, net1TradeDayMsg, net1TradeDayCnt);

  // N2 to seller
  printf("b2-s StdAvg before:%.10lf between:%.10lf after:%.10lf\n",
         (double)net2BeforeMsg/(double)net2BeforeDays,
         (double)net2BetweenMsg/(double)net2BetweenDays,
         (double)net2AfterMsg/(double)net2AfterDays);
  // averaging only over nonzero msg days
  printf("b2-s NonZeroAvg before:%.10lf between:%.10lf after:%.10lf\n",
         (double)net2BeforeMsg/(double)net2BeforeMsgDays,
         (double)net2BetweenMsg/(double)net2BetweenMsgDays,
         (double)net2AfterMsg/(double)net2AfterMsgDays);
  //printf("day of:%.10lf msgs:%lf days:%lf\n", net2TradeDayMsg/net2TradeDayCnt, net2TradeDayMsg, net2TradeDayCnt);

  FILE *out = fopen("eSeq.out", "w");

  // may13: new method
  // n1->n2 depending upon time between t1 and t2
  fprintf(out, "buyer1 to buyer2\n");
  fprintf(out, "winSize instances before between after\n");
  for (int i=1; i<=maxWin; i++)
    fprintf(out, "%d %lf %lf %lf %lf\n", i, winCnt[i], winMsg[i][0]/winCnt[i],
           winMsg[i][1]/winCnt[i], winMsg[i][2]/winCnt[i]);

  // for all instances where there is at least one msg from n1 -> n2
  fprintf(out, "buyer1 to seller\n");
  fprintf(out, "winSize instances before between after\n");
  for (int i=1; i<=maxWin; i++)
    fprintf(out, "%d %lf %lf %lf %lf\n", i, win1Cnt[i], win1Msg[i][0]/win1Cnt[i],
           win1Msg[i][1]/win1Cnt[i], win1Msg[i][2]/win1Cnt[i]);

  fprintf(out, "buyer2 to seller\n");
  fprintf(out, "winSize instances before between after\n");
  for (int i=1; i<=maxWin; i++)
    fprintf(out, "%d %lf %lf %lf %lf\n", i, win2Cnt[i], win2Msg[i][0]/win2Cnt[i],
           win2Msg[i][1]/win2Cnt[i], win2Msg[i][2]/win2Cnt[i]);

  fclose(out);
}

// given two people that buy from the same seller, whats the chance they talk between?
void EventSequenceProb(PTaobaoNet t) {

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  //PTaobaoShopNet c = t->contactNet;
  double numNodes = (double) m->GetNodes();

  // find date of first, last purchase in the data
  int totalDays = 58;
  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  uint beginSec = intervalBegin.GetAbsSecs();
  uint endSec = intervalEnd.GetAbsSecs();
  uint daySec = 24*60*60;

  double success = 0.0;
  double total = 0.0;
  double baseline = 0.0;
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

          // test for existence of one msg between trades

          int foundBetween = 0;
          if (m->IsNode(n1) && m->IsNode(n2)) {
            total += 1.0; // ASSUME that we only consider situation where edges are possible
            if (m->IsEdge(n1, n2)) { 
              TTaobaoShopEdge& msgEdge = m->GetEDat(n1, n2);
              for (int k=0; k<msgEdge.CntTX(); k++) {
                TSecTm mT = msgEdge.GetTime(k);              
                if (mT>=t1 && mT<=t2) { // changed < to <=
                  foundBetween = 1;
                  //foundBetween++;
                  //foundBetween += (int)msgEdge.GetStrength(k);
                  //totalMsg += (double)msgEdge.GetStrength(k);
                  break;
                }
              }
              
              if (foundBetween==1)
                success += 1.0;

            }

            // baseline should be inserted here
            // count number of nodes pointing to n2 in time period, divide by totalnodes
            int bNum=0;
            TTaobaoShopNet::TNodeI NK = m->GetNI(n2);
            for (int l=0; l<NK.GetInDeg(); l++) {
              TTaobaoShopEdge& e = NK.GetInEDat(l);
              for (int p=0; p<e.CntTX(); p++) {
                TSecTm mT = e.GetTime(p);
                if (mT>=t1 && mT<=t2) {
                  bNum++;
                  break;
                }
              }
            }
            
            baseline = baseline + (double)bNum / numNodes;  
          }
          


        }
      }

    }
  }

  printf("Success %lf Total %lf Observed %.10lf Random %.10lf\n", success, total, success/total, baseline/total);
}

// count types of node pairs, compare to expected
void nodePairCounting(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  FILE *out = fopen("nodePair.outPTYPERERUN", "w");
  
  printf("%d %d %d\n", s->GetNodes(), m->GetNodes(), c->GetNodes());
  printf("%d %d %d\n", s->GetEdges(), m->GetEdges(), c->GetEdges());
  fprintf(out, "%d %d %d\n", s->GetNodes(), m->GetNodes(), c->GetNodes());
  fprintf(out, "%d %d %d\n", s->GetEdges(), m->GetEdges(), c->GetEdges());

  double tradeMsg = 0.0;
  double tradeCon = 0.0;
  double tradeMsgCon = 0.0;
  double tradeOnly = 0.0;
  double msgCon = 0.0;
  double msgOnly = 0.0;

  double conOnly = 0.0;
  double totCon = 0.0;
  double totMsg = 0.0;
  double totTrade = 0.0;

  // make sure to count each pair only once
  // later when counting undirected total edges, remove dup

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int srcID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sinkID = NI.GetOutNId(i);
      if (!(s->IsEdge(sinkID, srcID) && sinkID>srcID)) {
        bool isMsg = false, isCon = false;
        if (m->IsEdge(srcID, sinkID) || m->IsEdge(sinkID, srcID))
          isMsg = true;

        if (c->IsEdge(srcID, sinkID))
          isCon = true;
        
        if (isMsg && isCon)
          tradeMsgCon += 1.0;
        else if (isMsg)
          tradeMsg += 1.0;
        else if (isCon)
          tradeCon += 1.0;
        else
          tradeOnly += 1.0;

        totTrade += 1.0;
      }
    }
  }

  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int srcID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sinkID = NI.GetOutNId(i);
      if (!(m->IsEdge(sinkID, srcID) && sinkID>srcID)) {
        if (!(s->IsEdge(srcID, sinkID)|| s->IsEdge(sinkID, srcID))) { 
          // only consider pairs with no trade edge (should have been covered above)
          if (c->IsEdge(srcID, sinkID)) // contacts paired
            msgCon += 1.0;
          else
            msgOnly += 1.0;
        }
        totMsg += 1.0;
      }
    }
  }

  totCon = (double) c->GetEdges() / 2.0;
  conOnly = totCon-tradeMsgCon-msgCon-tradeCon;
  
  // debug, check cononly
  int c2=0;
  for (TTaobaoShopNet::TNodeI NI = c->BegNI(); NI < c->EndNI(); NI++) {
    int srcID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sinkID = NI.GetOutNId(i);
      if (!(c->IsEdge(sinkID, srcID) && sinkID>srcID)) {
        if (!(s->IsEdge(srcID, sinkID)|| s->IsEdge(sinkID, srcID))) {
          if (!(m->IsEdge(srcID, sinkID)|| m->IsEdge(sinkID, srcID))) {
            c2++;
          }
        }
        
      }
    }
  }
  
  // these two should be equal, but sometimes slightly different?
  printf("%lf %d\n", conOnly, c2);
  fprintf(out, "%lf %d\n", conOnly, c2);

  
  // random assumes that edge probs are distributed evenly 
  // tot/n^2 prob on each edge
  // randTotMsg = totTrade*totMsg / maxEdges. maxEdges ~ numNodes^2/2

  double num = (double) s->GetNodes(); // orig graph: 1million
  printf("totTrade %lf totMsg %lf totCon %lf\n", totTrade, totMsg, totCon);
  printf("tradeMsgCon %lf tradeMsg %lf tradeCon %lf msgCon %lf tradeOnly %lf msgOnly %lf conOnly %lf\n", tradeMsgCon, tradeMsg, tradeCon, msgCon, tradeOnly, msgOnly, conOnly);

  fprintf(out, "totTrade %lf totMsg %lf totCon %lf\n", totTrade, totMsg, totCon);
  fprintf(out, "tradeMsgCon %lf tradeMsg %lf tradeCon %lf msgCon %lf tradeOnly %lf msgOnly %lf conOnly %lf\n", tradeMsgCon, tradeMsg, tradeCon, msgCon, tradeOnly, msgOnly, conOnly);

  fclose(out);

  basicStats(t);
  // random calc is stupid, near zero always
  //printf("randTM %lf randTC %lf randMC %lf\n",
  //0.5*(totTrade/num)*(totMsg/num), 0.5*(totTrade/num)*(totCon/num), 0.5*(totMsg/num)*(totCon/num));
}

// for each msg edge, do they buy from the same seller?
// by msg edge, i mean pair of nodes
void msgEdgeStudy(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;

  TIntH totMsg;
  TIntH sucMsg; 
  
  double success = 0.0;
  double total = 0.0;
  
  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int srcID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sinkID = NI.GetOutNId(i);
      if (!(m->IsEdge(sinkID, srcID) && sinkID>srcID)) {
        
        TTaobaoShopEdge& msgEdge = m->GetEDat(srcID, sinkID);
        int msgCnt = msgEdge.CntTXStrength();
        if (m->IsEdge(sinkID, srcID)) {
          msgEdge = m->GetEDat(sinkID, srcID);
          msgCnt += msgEdge.CntTXStrength();
        }

        if (totMsg.IsKey(msgCnt)) {
          int existCnt = totMsg.GetDat(msgCnt);
          totMsg.AddDat(msgCnt, existCnt+1);
        }
        else {
          totMsg.AddDat(msgCnt, 1);
          sucMsg.AddDat(msgCnt, 0);
        }

        TTaobaoShopNet::TNodeI NJ = s->GetNI(srcID);
        for (int j=0; j<NJ.GetOutDeg(); j++) {
            int buyerID = NJ.GetOutNId(j);
            if (s->IsEdge(sinkID, buyerID)) {
              success += 1.0;
              int existCnt = sucMsg.GetDat(msgCnt);
              sucMsg.AddDat(msgCnt, existCnt+1);
              break;
            }
        }

        total += 1.0;
      }
      
    }
  }

  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);

  //TFltPrV msgProb;

  FILE *out = fopen("msgEdgeStudy.outPEDGE", "w");

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
    //float tradeProb = (float)sucMsg[i] / (float)totMsg[i];
    //msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
    fprintf(out, "%d %d %d\n", (int)totMsg.GetKey(i), (int)sucMsg[i], (int)totMsg[i]);
    //fprintf(out, "%d %f\n", (int)totMsg.GetKey(i), tradeProb);
  }

  fclose(out);


  // baseline: 2 random nodes buy from the same seller
  TIntPrIntH nodePairs; // hash to cnt num unique node pairs 

  double baseline = 0.0;
  double numNodes = (double) s->GetNodes();
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    double deg = (double) NI.GetInDeg();
    if (deg >=2) {
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int n1 = NI.GetOutNId(i);
        for (int j=i+1; j<NI.GetOutDeg(); j++) {
          int n2 = NI.GetOutNId(j);
          if (n1 < n2) {
            if (!nodePairs.IsKey(TIntPr(n1,n2)))
              nodePairs.AddDat(TIntPr(n1,n2), 1);
          }
          else {
            if (!nodePairs.IsKey(TIntPr(n2,n1)))
              nodePairs.AddDat(TIntPr(n2,n1), 1);
          }
           
        }
      }
      //baseline += (deg*(deg-1.0)*0.5);
    }
  }

  baseline = (double) nodePairs.Len();
  printf("node pairs %lf\n", baseline);

  // baseline is upper bound, overcount when a pair buys from multple sellers
  // overcount should be negligible
  baseline = baseline / ((numNodes)*(numNodes-1.0)/2.0);
  printf("%lf %lf observed %.10lf baseline %.10lf\n", success, total, success/total, baseline);

}

// new, for directed counting
// store msg and trade in same edge, also explicitly store first times
void dirAddEdge(PTaobaoShopNet t, int SrcId, int DstId, int edgeType, int firstTime) {
  if (t->IsEdge(SrcId, DstId)) {
    TTaobaoShopEdge &EdgeDat = t->GetEDat(SrcId, DstId);
    if (edgeType==0) {
      EdgeDat.isTrade = true;
      EdgeDat.firstTrade = firstTime;
    }
    else if (edgeType==1) {
      EdgeDat.isMsg = true;
      EdgeDat.firstMsg = firstTime;
    }
    //else if (edgeType==2)
    //  EdgeDat.isCon = true;
  }
  else {
    TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
    //EdgeDat.isCon = false;
    EdgeDat.isMsg = false;
    EdgeDat.isTrade = false;
    if (edgeType==0) {
      EdgeDat.isTrade = true;
      EdgeDat.firstTrade = firstTime;
    }
    else if (edgeType==1) {
      EdgeDat.isMsg = true;
      EdgeDat.firstMsg = firstTime;
    }
    //else if (edgeType==2)
    //  EdgeDat.isCon = true;
    t->AddEdge(SrcId, DstId, EdgeDat);
  }

}

// utility function for triangleCounting graph construction
void undirAddEdge(PTaobaoShopNet t, int SrcId, int DstId, int edgeType) {
  if (t->IsEdge(SrcId, DstId)) {
    TTaobaoShopEdge &EdgeDat = t->GetEDat(SrcId, DstId);
    if (edgeType==0)
      EdgeDat.isTrade = true;
    else if (edgeType==1)
      EdgeDat.isMsg = true;
    else if (edgeType==2)
      EdgeDat.isCon = true;
  }
  else {
    TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
    EdgeDat.isCon = false;
    EdgeDat.isMsg = false;
    EdgeDat.isTrade = false;
    if (edgeType==0)
      EdgeDat.isTrade = true;
    else if (edgeType==1)
      EdgeDat.isMsg = true;
    else if (edgeType==2)
      EdgeDat.isCon = true;    
    t->AddEdge(SrcId, DstId, EdgeDat);
  }

  if (t->IsEdge(DstId, SrcId)) {
    TTaobaoShopEdge &EdgeDat = t->GetEDat(DstId, SrcId);
    if (edgeType==0)
      EdgeDat.isTrade = true;
    else if (edgeType==1)
      EdgeDat.isMsg = true;
    else if (edgeType==2)
      EdgeDat.isCon = true;
  }
  else {
    TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
    EdgeDat.isCon = false;
    EdgeDat.isMsg = false;
    EdgeDat.isTrade = false;
    if (edgeType==0)
      EdgeDat.isTrade = true;
    else if (edgeType==1)
      EdgeDat.isMsg = true;
    else if (edgeType==2)
      EdgeDat.isCon = true;
    t->AddEdge(DstId, SrcId, EdgeDat);
  }
}

// new directed rule counting
/*
  central node X, second node U, third node V.
  first step go through all nodes, and count how many out msgs, how many out
  trades they have. store proportion out msgs

  we will compute generative surprise coming out of both the 2nd and 3rd nodes
  
*/
void dirCounting(PTaobaoNet t) {
  // count all wrt vertex
  // ignore contacts, no timestamps
  // assume t1 < t2 < t3
  // edge 1/2 can be in/out msg/trade
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  PTaobaoShopNet t2 = new TTaobaoShopNet();

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( false, false);
    t2->AddNode(ID, newUsr);
  }

  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
    }
  }

  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (SrcId == DstId)
      printf("error same trade\n");
    else {
      TTaobaoShopEdge& edge = EI();
      dirAddEdge(t2, SrcId, DstId, 0, edge.GetTime(0).GetAbsSecs());
    }
  }

  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (SrcId == DstId) 
      printf("error same msg\n");
    else {
      TTaobaoShopEdge& edge = EI();
      dirAddEdge(t2, SrcId, DstId, 1, edge.GetTime(0).GetAbsSecs());
    }
  }
 
  TIntFltH outPropH; // proportion of out edges that are messages
  TIntFltH inPropH; 
  for (TTaobaoShopNet::TNodeI NI = t2->BegNI(); NI < t2->EndNI(); NI++) {
    int n = NI.GetId();
    float outM = 0.0;
    float outT = 0.0;
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int n2 = NI.GetOutNId(i);
      TTaobaoShopEdge e1 = t2->GetEDat(n, n2);
      if (e1.isTrade)
        outT += 1.0;
      if (e1.isMsg)
        outM += 1.0;
    }
    float propOut = outM/(outM+outT);
    outPropH.AddDat(n, propOut);
    
    float inM = 0.0;
    float inT = 0.0;
    for (int i=0; i<NI.GetInDeg(); i++) {
      int n2 = NI.GetInNId(i);
      TTaobaoShopEdge e2 = t2->GetEDat(n2, n);
      if (e2.isTrade)
        inT += 1.0;
      if (e2.isMsg)
        inM += 1.0;
    }
    float propIn = inM/(inM+inT);
    inPropH.AddDat(n, propIn);
  }
  // use 1-propH for out trades

  double triCnt[4][4][4]; // for counting triads / proportions
  double triSuc[4][4]; // for counting prob of a config set succeeding
  double triFail[4][4];
  float genNum[4][4]; // generative baseline numerator
  float genDenom[4][4]; 
  
  // oct25 new: computing directed message surprises as well
  float genNum2[4][4];
  float genDenom2[4][4];
  float genNum3[4][4];
  float genDenom3[4][4];

  float recNum[4][4]; // receptive baseline denominator
  float recDenom[4][4];
  // generative surprises for directed trades
  float tradeNum2[4][4]; // direction from 2nd to 3rd node
  float tradeDenom2[4][4];
  float tradeNum3[4][4]; // direction from 3rd to 2nd node
  float tradeDenom3[4][4];

  // have to use floats for hash, compare double in end
  /*
    propNum is summation of pout msg for msgs
    use summation 1-propNum for trades
    denom is SQRT(summation propNum*(1-propNum))
    each instance of a node in a closed triangle is a bernoulli random var with
    some pi success
    variance of sum of bernoullis is sum of variances

    // for each completed triangle we can mark off four sums:
    generative num, generative denom, receptive num, receptive denom

    in each of the 64 cases, need to look up out node, update generative
    look up in node, update receptive

    type of 3rd edge is irrelevant for computation
    we just need to count and note direction of third edge
    eg 0, 2 update with node 2
    1, 3 update with node 3
   */

  // 0 out trade 1 in trade 2 out msg 3 in msg
  // 3rd dimension is wrt node 2. eg 2->3 is out
  // 5th dimension is fail count

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      triSuc[i][j] = 0.0; // if any of 4 possible 3rd edges satisfy, we have success
      triFail[i][j] = 0.0; // else it's a failure
      genNum[i][j] = 0.0;
      genDenom[i][j] = 0.0;
      genNum2[i][j] = 0.0;
      genDenom2[i][j] = 0.0;
      genNum3[i][j] = 0.0;
      genDenom3[i][j] = 0.0;

      recNum[i][j] = 0.0;
      recDenom[i][j] = 0.0;
      tradeNum2[i][j] = 0.0;
      tradeDenom2[i][j] = 0.0;
      tradeNum3[i][j] = 0.0;
      tradeDenom3[i][j] = 0.0;
      for (int k=0; k<4; k++)
        triCnt[i][j][k] = 0.0;
    }
  
  int cnt = 0;
  for (TTaobaoShopNet::TNodeI NI = t2->BegNI(); NI < t2->EndNI(); NI++) {
    cnt++;
    if (cnt%10000==0) // tracking
      printf("processing %d\n", cnt);
    int n1 = NI.GetId(); // n1 is central vertex of interest

    // 50% of cases: edge1 OUT
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int n2 = NI.GetOutNId(i);
      TTaobaoShopEdge e1 = t2->GetEDat(n1, n2);
      float n2mOutProp = outPropH.GetDat(n2);
      float n2tOutProp = 1.0-n2mOutProp;
      float n2mInProp = inPropH.GetDat(n2);

      // 25% of cases: edge1 OUT, edge2 OUT
      for (int j=0; j<NI.GetOutDeg(); j++) {
        int n3 = NI.GetOutNId(j);
        float n3mOutProp = outPropH.GetDat(n3);
        float n3tOutProp = 1.0-n3mOutProp;
        float n3mInProp = inPropH.GetDat(n3);

        if (n3 != n1) {
          TTaobaoShopEdge e2 = t2->GetEDat(n1, n3);

          // booleans indicating if first two edges satisfy time
          // change to value 2 if closed by a third edge
          int tm = 0;
          int tt = 0;
          int mt = 0;
          int mm = 0;
          if (e1.isTrade) {
            if (e2.isTrade && e2.firstTrade > e1.firstTrade)
              tt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstTrade)
              tm = 1;
          }
          if (e1.isMsg) {
            if (e2.isTrade && e2.firstTrade > e1.firstMsg)
              mt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstMsg)
              mm = 1;
          }
          
          // go through possible third edges
          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][0][0] += 1.0;
                  genNum[0][0] += n2mOutProp;
                  genDenom[0][0] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[0][0] += n2mOutProp;
                  genDenom2[0][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][0] += n2tOutProp;
                  tradeDenom2[0][0] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][0][0] += 1.0;
                  genNum[2][0] += n2mOutProp;
                  genDenom[2][0] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][0] += n2mOutProp;
                  genDenom2[2][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][0] += n2tOutProp;
                  tradeDenom2[2][0] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][2][0] += 1.0;
                  genNum[0][2] += n2mOutProp;
                  genDenom[0][2] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[0][2] += n2mOutProp;
                  genDenom2[0][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][2] += n2tOutProp;
                  tradeDenom2[0][2] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][2][0] += 1.0;
                  genNum[2][2] += n2mOutProp;
                  genDenom[2][2] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][2] += n2mOutProp;
                  genDenom2[2][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][2] += n2tOutProp;
                  tradeDenom2[2][2] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][0][2] += 1.0;
                  genNum[0][0] += n2mOutProp;
                  genDenom[0][0] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[0][0] += n2mOutProp;
                  genDenom2[0][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][0] += n2tOutProp;
                  tradeDenom2[0][0] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][0][2] += 1.0;
                  genNum[2][0] += n2mOutProp;
                  genDenom[2][0] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][0] += n2mOutProp;
                  genDenom2[2][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][0] += n2tOutProp;
                  tradeDenom2[2][0] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][2][2] += 1.0;
                  genNum[0][2] += n2mOutProp;
                  genDenom[0][2] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[0][2] += n2mOutProp;
                  genDenom2[0][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][2] += n2tOutProp;
                  tradeDenom2[0][2] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][2][2] += 1.0;
                  genNum[2][2] += n2mOutProp;
                  genDenom[2][2] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][2] += n2mOutProp;
                  genDenom2[2][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][2] += n2tOutProp;
                  tradeDenom2[2][2] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }
          }

          if (t2->IsEdge(n3, n2)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n3, n2);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][0][1] += 1.0;
                  genNum[0][0] += n3mOutProp;
                  genDenom[0][0] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][0] += n3mOutProp;
                  genDenom3[0][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][0] += n3tOutProp;
                  tradeDenom3[0][0] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][0][1] += 1.0;
                  genNum[2][0] += n3mOutProp;
                  genDenom[2][0] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][0] += n3mOutProp;
                  genDenom3[2][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][0] += n3tOutProp;
                  tradeDenom3[2][0] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][2][1] += 1.0;
                  genNum[0][2] += n3mOutProp;
                  genDenom[0][2] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][2] += n3mOutProp;
                  genDenom3[0][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][2] += n3tOutProp;
                  tradeDenom3[0][2] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][2][1] += 1.0;
                  genNum[2][2] += n3mOutProp;
                  genDenom[2][2] += (n3mOutProp*(1.0-n3mOutProp));
                
                  genNum3[2][2] += n3mOutProp;
                  genDenom3[2][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][2] += n3tOutProp;
                  tradeDenom3[2][2] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][0][3] += 1.0;
                  genNum[0][0] += n3mOutProp;
                  genDenom[0][0] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][0] += n3mOutProp;
                  genDenom3[0][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][0] += n3tOutProp;
                  tradeDenom3[0][0] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][0][3] += 1.0;
                  genNum[2][0] += n3mOutProp;
                  genDenom[2][0] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][0] += n3mOutProp;
                  genDenom3[2][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][0] += n3tOutProp;
                  tradeDenom3[2][0] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][2][3] += 1.0;
                  genNum[0][2] += n3mOutProp;
                  genDenom[0][2] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][2] += n3mOutProp;
                  genDenom3[0][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][2] += n3tOutProp;
                  tradeDenom3[0][2] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][2][3] += 1.0;
                  genNum[2][2] += n3mOutProp;
                  genDenom[2][2] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][2] += n3mOutProp;
                  genDenom3[2][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][2] += n3tOutProp;
                  tradeDenom3[2][2] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

          }

          // check 4 vars for 1's, mark off failures
          if (tt == 1)
            triFail[0][0] += 1.0;
          else if (tt > 1) 
            triSuc[0][0] += 1.0;
          if (tm == 1)
            triFail[0][2] += 1.0;
          else if (tm > 1)
            triSuc[0][2] += 1.0;
          if (mt == 1)
            triFail[2][0] += 1.0;
          else if (mt > 1)
            triSuc[2][0] += 1.0;
          if (mm == 1)
            triFail[2][2] += 1.0;
          else if (mm > 1)
            triSuc[2][2] += 1.0;

        }
      }
      
      // edge1 OUT, edge2 IN
      for (int j=0; j<NI.GetInDeg(); j++) {
        int n3 = NI.GetInNId(j);
        float n3mOutProp = outPropH.GetDat(n3);
        float n3tOutProp = 1.0-n3mOutProp;
        float n3mInProp = inPropH.GetDat(n3);
        
        // CHECK IF THIS DIRECTION IS CORRECT

        if (n3 != n1) {
          TTaobaoShopEdge e2 = t2->GetEDat(n3, n1);
          // booleans indicating if first two edges satisfy time
          int tm = 0;
          int tt = 0;
          int mt = 0;
          int mm = 0;
          if (e1.isTrade) {
            if (e2.isTrade && e2.firstTrade > e1.firstTrade)
              tt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstTrade)
              tm = 1;
          }
          if (e1.isMsg) {
            if (e2.isTrade && e2.firstTrade > e1.firstMsg)
              mt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstMsg)
              mm = 1;
          }

          // go through possible third edges
          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][1][0] += 1.0;
                  genNum[0][1] += n2mOutProp;
                  genDenom[0][1] += (n2mOutProp*(1.0-n2mOutProp));

                  genNum2[0][1] += n2mOutProp;
                  genDenom2[0][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][1] += n2tOutProp;
                  tradeDenom2[0][1] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][1][0] += 1.0;
                  genNum[2][1] += n2mOutProp;
                  genDenom[2][1] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[2][1] += n2mOutProp;
                  genDenom2[2][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][1] += n2tOutProp;
                  tradeDenom2[2][1] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][3][0] += 1.0;
                  genNum[0][3] += n2mOutProp;
                  genDenom[0][3] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[0][3] += n2mOutProp;
                  genDenom2[0][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][3] += n2tOutProp;
                  tradeDenom2[0][3] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][3][0] += 1.0;
                  genNum[2][3] += n2mOutProp;
                  genDenom[2][3] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][3] += n2mOutProp;
                  genDenom2[2][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][3] += n2tOutProp;
                  tradeDenom2[2][3] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][1][2] += 1.0;
                  genNum[0][1] += n2mOutProp;
                  genDenom[0][1] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[0][1] += n2mOutProp;
                  genDenom2[0][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][1] += n2tOutProp;
                  tradeDenom2[0][1] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][1][2] += 1.0;
                  genNum[2][1] += n2mOutProp;
                  genDenom[2][1] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][1] += n2mOutProp;
                  genDenom2[2][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][1] += n2tOutProp;
                  tradeDenom2[2][1] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][3][2] += 1.0;
                  genNum[0][3] += n2mOutProp;
                  genDenom[0][3] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[0][3] += n2mOutProp;
                  genDenom2[0][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[0][3] += n2tOutProp;
                  tradeDenom2[0][3] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][3][2] += 1.0;
                  genNum[2][3] += n2mOutProp;
                  genDenom[2][3] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[2][3] += n2mOutProp;
                  genDenom2[2][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[2][3] += n2tOutProp;
                  tradeDenom2[2][3] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }
          }

          if (t2->IsEdge(n3, n2)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n3, n2);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][1][1] += 1.0;
                  genNum[0][1] += n3mOutProp;
                  genDenom[0][1] += (n3mOutProp*(1.0-n3mOutProp));
                 
                  genNum3[0][1] += n3mOutProp;
                  genDenom3[0][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][1] += n3tOutProp;
                  tradeDenom3[0][1] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][1][1] += 1.0;
                  genNum[2][1] += n3mOutProp;
                  genDenom[2][1] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][1] += n3mOutProp;
                  genDenom3[2][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][1] += n3tOutProp;
                  tradeDenom3[2][1] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][3][1] += 1.0;
                  genNum[0][3] += n3mOutProp;
                  genDenom[0][3] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][3] += n3mOutProp;
                  genDenom3[0][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][3] += n3tOutProp;
                  tradeDenom3[0][3] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][3][1] += 1.0;
                  genNum[2][3] += n3mOutProp;
                  genDenom[2][3] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][3] += n3mOutProp;
                  genDenom3[2][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][3] += n3tOutProp;
                  tradeDenom3[2][3] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[0][1][3] += 1.0;
                  genNum[0][1] += n3mOutProp;
                  genDenom[0][1] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][1] += n3mOutProp;
                  genDenom3[0][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][1] += n3tOutProp;
                  tradeDenom3[0][1] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[2][1][3] += 1.0;
                  genNum[2][1] += n3mOutProp;
                  genDenom[2][1] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][1] += n3mOutProp;
                  genDenom3[2][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][1] += n3tOutProp;
                  tradeDenom3[2][1] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[0][3][3] += 1.0;
                  genNum[0][3] += n3mOutProp;
                  genDenom[0][3] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[0][3] += n3mOutProp;
                  genDenom3[0][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[0][3] += n3tOutProp;
                  tradeDenom3[0][3] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[2][3][3] += 1.0;
                  genNum[2][3] += n3mOutProp;
                  genDenom[2][3] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[2][3] += n3mOutProp;
                  genDenom3[2][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[2][3] += n3tOutProp;
                  tradeDenom3[2][3] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

          }

          // check 4 vars for 1's, mark off failures
          if (tt == 1)
            triFail[0][1] += 1.0;
          else if (tt > 1)
            triSuc[0][1] += 1.0;
          if (tm == 1)
            triFail[0][3] += 1.0;
          else if (tm > 1)
            triSuc[0][3] += 1.0;
          if (mt == 1)
            triFail[2][1] += 1.0;
          else if (mt > 1)
            triSuc[2][1] += 1.0;
          if (mm == 1)
            triFail[2][3] += 1.0;
          else if (mm > 1)
            triSuc[2][3] += 1.0;

        }
      }
    }

    // seoond 50% of cases
    for (int i=0; i<NI.GetInDeg(); i++) {
      int n2 = NI.GetInNId(i);
      TTaobaoShopEdge e1 = t2->GetEDat(n2, n1);
      float n2mOutProp = outPropH.GetDat(n2);
      float n2tOutProp = 1.0-n2mOutProp;
      float n2mInProp = inPropH.GetDat(n2);
      
      // 25% of cases: edge1 IN, edge2 OUT
      for (int j=0; j<NI.GetOutDeg(); j++) {
        int n3 = NI.GetOutNId(j);
        float n3mOutProp = outPropH.GetDat(n3);
        float n3tOutProp = 1.0-n3mOutProp;
        float n3mInProp = inPropH.GetDat(n3);

        if (n3 != n1) {
          TTaobaoShopEdge e2 = t2->GetEDat(n1, n3);

          // booleans indicating if first two edges satisfy time
          // change to value 2 if closed by a third edge
          int tm = 0;
          int tt = 0;
          int mt = 0;
          int mm = 0;
          if (e1.isTrade) {
            if (e2.isTrade && e2.firstTrade > e1.firstTrade)
              tt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstTrade)
              tm = 1;
          }
          if (e1.isMsg) {
            if (e2.isTrade && e2.firstTrade > e1.firstMsg)
              mt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstMsg)
              mm = 1;
          }
          
          // go through possible third edges
          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][0][0] += 1.0;
                  genNum[1][0] += n2mOutProp;
                  genDenom[1][0] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[1][0] += n2mOutProp;
                  genDenom2[1][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][0] += n2tOutProp;
                  tradeDenom2[1][0] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][0][0] += 1.0;
                  genNum[3][0] += n2mOutProp;
                  genDenom[3][0] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[3][0] += n2mOutProp;
                  genDenom2[3][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][0] += n2tOutProp;
                  tradeDenom2[3][0] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][2][0] += 1.0;
                  genNum[1][2] += n2mOutProp;
                  genDenom[1][2] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[1][2] += n2mOutProp;
                  genDenom2[1][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][2] += n2tOutProp;
                  tradeDenom2[1][2] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][2][0] += 1.0;
                  genNum[3][2] += n2mOutProp;
                  genDenom[3][2] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[3][2] += n2mOutProp;
                  genDenom2[3][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][2] += n2tOutProp;
                  tradeDenom2[3][2] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][0][2] += 1.0;
                  genNum[1][0] += n2mOutProp;
                  genDenom[1][0] += (n2mOutProp*(1.0-n2mOutProp));

                  genNum2[1][0] += n2mOutProp;
                  genDenom2[1][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][0] += n2tOutProp;
                  tradeDenom2[1][0] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][0][2] += 1.0;
                  genNum[3][0] += n2mOutProp;
                  genDenom[3][0] += (n2mOutProp*(1.0-n2mOutProp));
            
                  genNum2[3][0] += n2mOutProp;
                  genDenom2[3][0] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][0] += n2tOutProp;
                  tradeDenom2[3][0] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][2][2] += 1.0;
                  genNum[1][2] += n2mOutProp;
                  genDenom[1][2] += (n2mOutProp*(1.0-n2mOutProp));
                
                  genNum2[1][2] += n2mOutProp;
                  genDenom2[1][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][2] += n2tOutProp;
                  tradeDenom2[1][2] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][2][2] += 1.0;
                  genNum[3][2] += n2mOutProp;
                  genDenom[3][2] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[3][2] += n2mOutProp;
                  genDenom2[3][2] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][2] += n2tOutProp;
                  tradeDenom2[3][2] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }
          }

          if (t2->IsEdge(n3, n2)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n3, n2);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][0][1] += 1.0;
                  genNum[1][0] += n3mOutProp;
                  genDenom[1][0] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[1][0] += n3mOutProp;
                  genDenom3[1][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][0] += n3tOutProp;
                  tradeDenom3[1][0] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][0][1] += 1.0;
                  genNum[3][0] += n3mOutProp;
                  genDenom[3][0] += (n3mOutProp*(1.0-n3mOutProp));
                
                  genNum3[3][0] += n3mOutProp;
                  genDenom3[3][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][0] += n3tOutProp;
                  tradeDenom3[3][0] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][2][1] += 1.0;
                  genNum[1][2] += n3mOutProp;
                  genDenom[1][2] += (n3mOutProp*(1.0-n3mOutProp));
                
                  genNum3[1][2] += n3mOutProp;
                  genDenom3[1][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][2] += n3tOutProp;
                  tradeDenom3[1][2] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][2][1] += 1.0;
                  genNum[3][2] += n3mOutProp;
                  genDenom[3][2] += (n3mOutProp*(1.0-n3mOutProp));
                 
                  genNum3[3][2] += n3mOutProp;
                  genDenom3[3][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][2] += n3tOutProp;
                  tradeDenom3[3][2] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][0][3] += 1.0;
                  genNum[1][0] += n3mOutProp;
                  genDenom[1][0] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[1][0] += n3mOutProp;
                  genDenom3[1][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][0] += n3tOutProp;
                  tradeDenom3[1][0] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][0][3] += 1.0;
                  genNum[3][0] += n3mOutProp;
                  genDenom[3][0] += (n3mOutProp*(1.0-n3mOutProp));
                 
                  genNum3[3][0] += n3mOutProp;
                  genDenom3[3][0] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][0] += n3tOutProp;
                  tradeDenom3[3][0] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][2][3] += 1.0;
                  genNum[1][2] += n3mOutProp;
                  genDenom[1][2] += (n3mOutProp*(1.0-n3mOutProp));
                
                  genNum3[1][2] += n3mOutProp;
                  genDenom3[1][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][2] += n3tOutProp;
                  tradeDenom3[1][2] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][2][3] += 1.0;
                  genNum[3][2] += n3mOutProp;
                  genDenom[3][2] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[3][2] += n3mOutProp;
                  genDenom3[3][2] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][2] += n3tOutProp;
                  tradeDenom3[3][2] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

          }

          // check 4 vars for 1's, mark off failures
          if (tt == 1)
            triFail[1][0] += 1.0;
          else if (tt > 1) 
            triSuc[1][0] += 1.0;
          if (tm == 1)
            triFail[1][2] += 1.0;
          else if (tm > 1)
            triSuc[1][2] += 1.0;
          if (mt == 1)
            triFail[3][0] += 1.0;
          else if (mt > 1)
            triSuc[3][0] += 1.0;
          if (mm == 1)
            triFail[3][2] += 1.0;
          else if (mm > 1)
            triSuc[3][2] += 1.0;

        }
      }
      
      // edge1 IN, edge2 IN
      for (int j=0; j<NI.GetInDeg(); j++) {
        int n3 = NI.GetInNId(j);
        float n3mOutProp = outPropH.GetDat(n3);
        float n3tOutProp = 1.0-n3mOutProp;
        float n3mInProp = inPropH.GetDat(n3);

        if (n3 != n1) {
          TTaobaoShopEdge e2 = t2->GetEDat(n3, n1);
          // booleans indicating if first two edges satisfy time
          int tm = 0;
          int tt = 0;
          int mt = 0;
          int mm = 0;
          if (e1.isTrade) {
            if (e2.isTrade && e2.firstTrade > e1.firstTrade)
              tt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstTrade)
              tm = 1;
          }
          if (e1.isMsg) {
            if (e2.isTrade && e2.firstTrade > e1.firstMsg)
              mt = 1;
            if (e2.isMsg && e2.firstMsg > e1.firstMsg)
              mm = 1;
          }

          // go through possible third edges
          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][1][0] += 1.0;
                  genNum[1][1] += n2mOutProp;
                  genDenom[1][1] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[1][1] += n2mOutProp;
                  genDenom2[1][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][1] += n2tOutProp;
                  tradeDenom2[1][1] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][1][0] += 1.0;
                  genNum[3][1] += n2mOutProp;
                  genDenom[3][1] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[3][1] += n2mOutProp;
                  genDenom2[3][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][1] += n2tOutProp;
                  tradeDenom2[3][1] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][3][0] += 1.0;
                  genNum[1][3] += n2mOutProp;
                  genDenom[1][3] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[1][3] += n2mOutProp;
                  genDenom2[1][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][3] += n2tOutProp;
                  tradeDenom2[1][3] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][3][0] += 1.0;
                  genNum[3][3] += n2mOutProp;
                  genDenom[3][3] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[3][3] += n2mOutProp;
                  genDenom2[3][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][3] += n2tOutProp;
                  tradeDenom2[3][3] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][1][2] += 1.0;
                  genNum[1][1] += n2mOutProp;
                  genDenom[1][1] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[1][1] += n2mOutProp;
                  genDenom2[1][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][1] += n2tOutProp;
                  tradeDenom2[1][1] += (n2tOutProp*(1.0-n2tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][1][2] += 1.0;
                  genNum[3][1] += n2mOutProp;
                  genDenom[3][1] += (n2mOutProp*(1.0-n2mOutProp));
                  
                  genNum2[3][1] += n2mOutProp;
                  genDenom2[3][1] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][1] += n2tOutProp;
                  tradeDenom2[3][1] += (n2tOutProp*(1.0-n2tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][3][2] += 1.0;
                  genNum[1][3] += n2mOutProp;
                  genDenom[1][3] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[1][3] += n2mOutProp;
                  genDenom2[1][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[1][3] += n2tOutProp;
                  tradeDenom2[1][3] += (n2tOutProp*(1.0-n2tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][3][2] += 1.0;
                  genNum[3][3] += n2mOutProp;
                  genDenom[3][3] += (n2mOutProp*(1.0-n2mOutProp));
                 
                  genNum2[3][3] += n2mOutProp;
                  genDenom2[3][3] += (n2mOutProp*(1.0-n2mOutProp));
                  tradeNum2[3][3] += n2tOutProp;
                  tradeDenom2[3][3] += (n2tOutProp*(1.0-n2tOutProp));
                  mm++;
                }
              }
            }
          }

          if (t2->IsEdge(n3, n2)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n3, n2);
            if (e3.isTrade) {
              if (e3.firstTrade > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][1][1] += 1.0;
                  genNum[1][1] += n3mOutProp;
                  genDenom[1][1] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[1][1] += n3mOutProp;
                  genDenom3[1][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][1] += n3tOutProp;
                  tradeDenom3[1][1] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][1][1] += 1.0;
                  genNum[3][1] += n3mOutProp;
                  genDenom[3][1] += (n3mOutProp*(1.0-n3mOutProp));
                 
                  genNum3[3][1] += n3mOutProp;
                  genDenom3[3][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][1] += n3tOutProp;
                  tradeDenom3[3][1] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstTrade > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][3][1] += 1.0;
                  genNum[1][3] += n3mOutProp;
                  genDenom[1][3] += (n3mOutProp*(1.0-n3mOutProp));
                 
                  genNum3[1][3] += n3mOutProp;
                  genDenom3[1][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][3] += n3tOutProp;
                  tradeDenom3[1][3] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][3][1] += 1.0;
                  genNum[3][3] += n3mOutProp;
                  genDenom[3][3] += (n3mOutProp*(1.0-n3mOutProp));
                
                  genNum3[3][3] += n3mOutProp;
                  genDenom3[3][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][3] += n3tOutProp;
                  tradeDenom3[3][3] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

            if (e3.isMsg) {
              if (e3.firstMsg > e2.firstTrade) {
                if (tt != 0) {
                  triCnt[1][1][3] += 1.0;
                  genNum[1][1] += n3mOutProp;
                  genDenom[1][1] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[1][1] += n3mOutProp;
                  genDenom3[1][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][1] += n3tOutProp;
                  tradeDenom3[1][1] += (n3tOutProp*(1.0-n3tOutProp));
                  tt++;
                }
                if (mt != 0) {
                  triCnt[3][1][3] += 1.0;
                  genNum[3][1] += n3mOutProp;
                  genDenom[3][1] += (n3mOutProp*(1.0-n3mOutProp));
               
                  genNum3[3][1] += n3mOutProp;
                  genDenom3[3][1] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][1] += n3tOutProp;
                  tradeDenom3[3][1] += (n3tOutProp*(1.0-n3tOutProp));
                  mt++;
                }
              }
              if (e3.firstMsg > e2.firstMsg) {
                if (tm != 0) {
                  triCnt[1][3][3] += 1.0;
                  genNum[1][3] += n3mOutProp;
                  genDenom[1][3] += (n3mOutProp*(1.0-n3mOutProp));
                  
                  genNum3[1][3] += n3mOutProp;
                  genDenom3[1][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[1][3] += n3tOutProp;
                  tradeDenom3[1][3] += (n3tOutProp*(1.0-n3tOutProp));
                  tm++;
                }
                if (mm != 0) {
                  triCnt[3][3][3] += 1.0;
                  genNum[3][3] += n3mOutProp;
                  genDenom[3][3] += (n3mOutProp*(1.0-n3mOutProp));
                 
                  genNum3[3][3] += n3mOutProp;
                  genDenom3[3][3] += (n3mOutProp*(1.0-n3mOutProp));
                  tradeNum3[3][3] += n3tOutProp;
                  tradeDenom3[3][3] += (n3tOutProp*(1.0-n3tOutProp));
                  mm++;
                }
              }
            }

          }

          // check 4 vars for 1's, mark off failures
          if (tt == 1)
            triFail[1][1] += 1.0;
          else if (tt > 1)
            triSuc[1][1] += 1.0;
          if (tm == 1)
            triFail[1][3] += 1.0;
          else if (tm > 1)
            triSuc[1][3] += 1.0;
          if (mt == 1)
            triFail[3][1] += 1.0;
          else if (mt > 1)
            triSuc[3][1] += 1.0;
          if (mm == 1)
            triFail[3][3] += 1.0;
          else if (mm > 1)
            triSuc[3][3] += 1.0;

        }
      }
    }
  }

  //double triCnt[4][4][4]; // for counting triads / proportions
  //double triSuc[4][4]; // for counting prob of a ocnfig set succeeding
  //double triFail[4][4];

  // output all numbers to text files
  FILE *out = fopen("dirTri.outOCT2", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      fprintf(out, "%d %d %lf %lf %lf %lf %lf %lf\n", i, j, triSuc[i][j],
              triFail[i][j], triCnt[i][j][0], triCnt[i][j][1], triCnt[i][j][2], triCnt[i][j][3]);
    }
  fclose(out);

  FILE *out2 = fopen("dirTriRawParam.outOCT2", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      fprintf(out2, "%d %d %f %f\n", i, j, genNum[i][j], genDenom[i][j]);
  fclose(out2);

  // directed trade info
  FILE *out4 = fopen("dirTriRawParam2.outOCT2", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      fprintf(out4, "%d %d %f %f %f %f\n", i, j, tradeNum2[i][j],
              tradeDenom2[i][j], tradeNum3[i][j], tradeDenom3[i][j]);
  fclose(out4);

  // directed message info
  FILE *out6 = fopen("dirTriRawParam3.outOCT2", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      fprintf(out6, "%d %d %f %f %f %f\n", i, j, genNum2[i][j],
              genDenom2[i][j], genNum3[i][j], genDenom3[i][j]);
  fclose(out6);

  
  // convert variance to std dev
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      genDenom[i][j] = sqrt(genDenom[i][j]);
      tradeDenom2[i][j] = sqrt(tradeDenom2[i][j]);
      tradeDenom3[i][j] = sqrt(tradeDenom3[i][j]);
      genDenom2[i][j] = sqrt(genDenom2[i][j]);
      genDenom3[i][j] = sqrt(genDenom3[i][j]);
    }

  // undirected message surprises
  FILE *out3 = fopen("dirTriSurprise.outOCT2", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      float observedMsg = (float)triCnt[i][j][2]+(float)triCnt[i][j][3];
      float surprise = (observedMsg-(float)genNum[i][j])/genDenom[i][j];
      fprintf(out3, "%d %d %f\n", i, j, surprise);
    }
  fclose(out3);
  
  // directed trade surprises
  FILE *out5 = fopen("dirTriSurprise2.outOCT2", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      float observedTrade1 = (float)triCnt[i][j][0];
      float surprise1 = (observedTrade1-(float)tradeNum2[i][j])/tradeDenom2[i][j];
      float observedTrade2 = (float)triCnt[i][j][1];
      float surprise2 = (observedTrade2-(float)tradeNum3[i][j])/tradeDenom3[i][j];
      float observedMsg1 = (float)triCnt[i][j][2];
      float surprise3 = (observedMsg1-(float)genNum2[i][j])/genDenom2[i][j];
      float observedMsg2 = (float)triCnt[i][j][3];
      float surprise4 = (observedMsg2-(float)genNum3[i][j])/genDenom3[i][j];

      fprintf(out5, "%d %d %f %f %f %f\n", i, j, surprise1, surprise2, surprise3, surprise4);
    }
  fclose(out5);

}

// compute surprises, output to file
// screwed up first time
void surprise() {
  double triCnt[4][4][4]; // for counting triads / proportions
  double triSuc[4][4]; // for counting prob of a config set succeeding
  double triFail[4][4];
  float genNum[4][4]; // generative baseline numerator
  float genDenom[4][4];

  FILE *in = fopen("dirTri.outNEW", "r");
  int a, b;
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      fscanf(in, "%d %d %lf %lf %lf %lf %lf %lf", &a, &b,
             &triSuc[i][j], &triFail[i][j], &triCnt[i][j][0], &triCnt[i][j][1],
             &triCnt[i][j][2], &triCnt[i][j][3]);
    }
  fclose(in);
  
  FILE *in2 = fopen("dirTriRawParam.out", "r");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      fscanf(in2, "%d %d %f %f\n", &a, &b, &genNum[i][j], &genDenom[i][j]);
    }
  fclose(in2);

  // convert variance to std dev
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      genDenom[i][j] = sqrt(genDenom[i][j]);

  FILE *out3 = fopen("dirTriSurprise.outREDO", "w");
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      float observedMsg = (float)triCnt[i][j][2]+(float)triCnt[i][j][3];
      float surprise = (observedMsg-(float)genNum[i][j])/genDenom[i][j];
      fprintf(out3, "%d %d %f\n", i, j, surprise);
    }
  fclose(out3);

}

// OCT 10: only absolute triangle counts and config sets
// todo: directed triangle forming (ignore contacts)
void triangleCountingNew(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  PTaobaoShopNet t2 = new TTaobaoShopNet();

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( false, false);
    t2->AddNode(ID, newUsr);
  }

  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
    }
  }

  for (TTaobaoShopNet::TNodeI NI = c->BegNI(); NI < c->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
    }
  }

  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (SrcId == DstId)
      printf("error same trade\n");
    else
      undirAddEdge(t2, SrcId, DstId, 0);
  }

  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (SrcId == DstId) // one of these in the data
      printf("error same msg\n");
    else
      undirAddEdge(t2, SrcId, DstId, 1);
  }
  
  for (TTaobaoShopNet::TEdgeI EI = c->BegEI(); EI < c->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (SrcId == DstId) // ignore doesnt hurt
      printf("error same con\n");
    if (SrcId < DstId) { // already two copies 
      undirAddEdge(t2, SrcId, DstId, 2);
    }
  }

  // trade 0 message 1 contact 2 

  // successful (A,B) - > C. Use to compute 3rd edge proportion, + triangles
  double triCnt[3][3][3]; 
  double triSuc[3][3];
  double triFail[3][3];
 
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++) {
      triSuc[i][j] = 0.0;
      triFail[i][j] = 0.0;
      for (int k=0; k<3; k++)
        triCnt[i][j][k] = 0.0;
    }
  // we do counting based on configuration sets wrt a vertex
  
  // to count absolute number of triangles. we need to add 3 numbers
  // then divide by 3. since each vertex of triangle will be counted once

  // note: will not keep first 2 indices ordered like in trianglecounting
  // will need to count [0][1][0], [1][0][0], [0][0][1], etc..
  /*
  for (TTaobaoShopNet::TNodeI NI = t2->BegNI(); NI < t2->EndNI(); NI++) {
    if (NI.GetOutDeg() != NI.GetInDeg())
      printf("deg error\n");
    int n1 = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int n2 = NI.GetOutNId(i);
      TTaobaoShopEdge e1 = t2->GetEDat(n1, n2);
      if (!t2->IsEdge(n2, n1))
        printf("edge error\n");
      TTaobaoShopEdge e2 = t2->GetEDat(n2, n1); // sjhould be identical
      if (e1.isTrade != e2.isTrade)
        printf("edge trade error\n");
      if (e1.isMsg != e2.isMsg)
        printf("edge msg error\n");
      if (e1.isCon != e2.isCon)
        printf("edge contact error\n");
    }
      
  }
  */

  // debug triple message cases
  //FILE *outM = fopen("triOut3M.outPTYPE", "w");

  int cnt=0;
  for (TTaobaoShopNet::TNodeI NI = t2->BegNI(); NI < t2->EndNI(); NI++) 
    if (NI.GetOutDeg() > 1) {
      cnt++;
      if (cnt%10000==0) // tracking
        printf("processing %d\n", cnt);

      int n1 = NI.GetId(); // n1 is vertex
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int n2 = NI.GetOutNId(i);
        TTaobaoShopEdge e1 = t2->GetEDat(n1, n2);
                
        for (int j=i+1; j<NI.GetOutDeg(); j++) { // no 2x counting
          int n3 = NI.GetOutNId(j);
          TTaobaoShopEdge e2 = t2->GetEDat(n1, n3);
        
          int thirdEdge = 0;
          int i1, i2;
          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            // if 3rd edge exists, mark off sucesses
            if (e1.isTrade) {
              i1 = 0;
              if (e2.isTrade) {
                i2 = 0;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
              if (e2.isMsg) {
                i2 = 1;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
              if (e2.isCon) {
                i2 = 2;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
            }

            if (e1.isMsg) {
              i1 = 1;
              if (e2.isTrade) {
                i2 = 0;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
              if (e2.isMsg) {
                i2 = 1;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg) {
                  triCnt[i1][i2][1] += 1.0;
                  //fprintf(outM, "%d %d %d\n", n1, n2, n3);
                }
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
              if (e2.isCon) {
                i2 = 2;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }

            }

            if (e1.isCon) {
              i1 = 2;
              if (e2.isTrade) {
                i2 = 0;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
              if (e2.isMsg) {
                i2 = 1;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
              if (e2.isCon) {
                i2 = 2;
                if (e3.isTrade)
                  triCnt[i1][i2][0] += 1.0;
                if (e3.isMsg)
                  triCnt[i1][i2][1] += 1.0;
                if (e3.isCon)
                  triCnt[i1][i2][2] += 1.0;
                triSuc[i1][i2] += 1.0;
              }
            }

          }
          else {
            if (e1.isTrade) {
              i1 = 0;
              if (e2.isTrade)
                triFail[i1][0] += 1.0;
              if (e2.isMsg)
                triFail[i1][1] += 1.0;
              if (e2.isCon)
                triFail[i1][2] += 1.0;
            }
            
            if (e1.isMsg) {
              i1 = 1;
              if (e2.isTrade)
                triFail[i1][0] += 1.0;
              if (e2.isMsg)
                triFail[i1][1] += 1.0;
              if (e2.isCon)
                triFail[i1][2] += 1.0;
            }
            
            if (e1.isCon) {
              i1 = 2;
              if (e2.isTrade)
                triFail[i1][0] += 1.0;
              if (e2.isMsg)
                triFail[i1][1] += 1.0;
              if (e2.isCon)
                triFail[i1][2] += 1.0;
            }
          }


        } // close j
      } // close i
    }
  
  //fclose(outM);

  FILE *out = fopen("triCntSimple.outNEW2PTYPE", "w");
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++) {
      fprintf(out, "%d %d %f %f %f %f %f %f\n", i, j, triSuc[i][j], triFail[i][j], triCnt[i][j][0], triCnt[i][j][1], triCnt[i][j][2]);
    }
  fclose(out);
  
  // always divide by 3 because each triangle is counted 3 times
  // wtf: mmm is not divisible
  FILE *out2 = fopen("triangleCountNew.outNEW2PTYPE", "w");
  fprintf(out2, "ttt %lf\n", triCnt[0][0][0]/3.0);
  fprintf(out2, "mmm %lf\n", triCnt[1][1][1]/3.0);
  fprintf(out2, "ccc %lf\n", triCnt[2][2][2]/3.0);
  fprintf(out2, "ttm %lf\n", (triCnt[0][0][1]+triCnt[0][1][0]+triCnt[1][0][0])/3.0);
  fprintf(out2, "ttc %lf\n", (triCnt[0][0][2]+triCnt[0][2][0]+triCnt[2][0][0])/3.0);
  fprintf(out2, "mmt %lf\n", (triCnt[1][1][0]+triCnt[1][0][1]+triCnt[0][1][1])/3.0);
  fprintf(out2, "mmc %lf\n", (triCnt[1][1][2]+triCnt[1][2][1]+triCnt[2][1][1])/3.0);
  fprintf(out2, "cct %lf\n", (triCnt[2][2][0]+triCnt[2][0][2]+triCnt[0][2][2])/3.0);
  fprintf(out2, "ccm %lf\n", (triCnt[2][2][1]+triCnt[2][1][2]+triCnt[1][2][2])/3.0);
  fprintf(out2, "tmc %lf\n", (triCnt[0][1][2]+triCnt[0][2][1]+triCnt[1][0][2]+triCnt[1][2][0]+triCnt[2][0][1]+triCnt[2][1][0])/3.0);
  fclose(out2);

  // to calculate config sets, first 2 edge orderings are arbitrary
  double propSum, goodTot, badTot;
  FILE *out3 = fopen("triangleConfigSet.outNEW2PTYPE", "w");
  // 2 trade
  propSum = triCnt[0][0][0]+triCnt[0][0][1]+triCnt[0][0][2];
  fprintf(out3, "tt %lf %lf %.10lf %lf %lf %lf\n", triSuc[0][0], triFail[0][0],
          triSuc[0][0]/(triSuc[0][0]+triFail[0][0]), triCnt[0][0][0]/propSum,
          triCnt[0][0][1]/propSum, triCnt[0][0][2]/propSum);
  // 2 msg
  propSum = triCnt[1][1][0]+triCnt[1][1][1]+triCnt[1][1][2];
  fprintf(out3, "mm %lf %lf %.10lf %lf %lf %lf\n", triSuc[1][1], triFail[1][1],
          triSuc[1][1]/(triSuc[1][1]+triFail[1][1]), triCnt[1][1][0]/propSum,
          triCnt[1][1][1]/propSum, triCnt[1][1][2]/propSum);

  // 2 con
  propSum = triCnt[2][2][0]+triCnt[2][2][1]+triCnt[2][2][2];
  fprintf(out3, "cc %lf %lf %.10lf %lf %lf %lf\n", triSuc[2][2], triFail[2][2],
          triSuc[2][2]/(triSuc[2][2]+triFail[2][2]), triCnt[2][2][0]/propSum,
          triCnt[2][2][1]/propSum, triCnt[2][2][2]/propSum);
  // 1 trade, 1 msg
  propSum = triCnt[0][1][0]+triCnt[1][0][0]+triCnt[0][1][1]+triCnt[1][0][1]+triCnt[0][1][2]+triCnt[1][0][2];
  goodTot = triSuc[0][1]+triSuc[1][0];
  badTot = triFail[0][1]+triFail[1][0];
  fprintf(out3, "tm %lf %lf %.10lf %lf %lf %lf\n", goodTot, badTot, goodTot/(goodTot+badTot),(triCnt[0][1][0]+triCnt[1][0][0])/propSum, 
          (triCnt[0][1][1]+triCnt[1][0][1])/propSum, (triCnt[0][1][2]+triCnt[1][0][2])/propSum); 
  // 1 trade, 1 con
  propSum = triCnt[0][2][0]+triCnt[2][0][0]+triCnt[0][2][1]+triCnt[2][0][1]+triCnt[0][2][2]+triCnt[2][0][2];
  goodTot = triSuc[0][2]+triSuc[2][0];
  badTot = triFail[0][2]+triFail[2][0];
  fprintf(out3, "tc %lf %lf %.10lf %lf %lf %lf\n", goodTot, badTot, goodTot/(goodTot+badTot),(triCnt[0][2][0]+triCnt[2][0][0])/propSum, (triCnt[0][2][1]+triCnt[2][0][1])/propSum, (triCnt[0][2][2]+triCnt[2][0][2])/propSum);
  // 1 msg, 1 con
  propSum = triCnt[1][2][0]+triCnt[2][1][0]+triCnt[1][2][1]+triCnt[2][1][1]+triCnt[1][2][2]+triCnt[2][1][2];
  goodTot = triSuc[1][2]+triSuc[2][1];
  badTot = triFail[1][2]+triFail[2][1];
  fprintf(out3, "mc %lf %lf %.10lf %lf %lf %lf\n", goodTot, badTot, goodTot/(goodTot+badTot),(triCnt[1][2][0]+triCnt[2][1][0])/propSum, 
          (triCnt[1][2][1]+triCnt[2][1][1])/propSum, (triCnt[1][2][2]+triCnt[2][1][2])/propSum);
  fclose(out3);

}

// third edge: doing one way handling 
// contact edges are 2 way, so not exact
void triangleCounting(PTaobaoNet t) {

  /*
    MAR: new, created a combined (directed) graph
    edges have booleans indicating presence of each type of edge
    problem: graph undirect eliminates edge info, don't use it
    eg we create new unidrected or directed graph manually
    for undirected, insert double edges
    when counting for undirected, just use this graph
  
    0 - nothing
    1 - trade
    2 - msg
    3 - trade+msg
    4 - contact
    5 - trade+contact
    6 - msg+contact
    7 - trade+msg+contact
  */

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet; 

  // aggregate network
  PTaobaoShopNet t2 = new TTaobaoShopNet(); 

  FILE *out2 = fopen("triDebug.out", "w");

  // add all nodes first
  // for permuteTypes, we need to go through 3 networks when adding nodes
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( false, false);
    t2->AddNode(ID, newUsr);
  }

  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
    }
  }

  for (TTaobaoShopNet::TNodeI NI = c->BegNI(); NI < c->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
    }
  }

  // go through all trade edges 
  // for pedgesundir, we can speed up the undiraddedge with another src<dst
  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    undirAddEdge(t2, SrcId, DstId, 0);
  }

  printf("%d %d\n", t2->GetNodes(), t2->GetEdges());
  fprintf(out2, "%d %d\n", t2->GetNodes(), t2->GetEdges());

  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    undirAddEdge(t2, SrcId, DstId, 1);
  }

  printf("hi\n");

  int cCall=0;
  for (TTaobaoShopNet::TEdgeI EI = c->BegEI(); EI < c->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (SrcId < DstId) {
      undirAddEdge(t2, SrcId, DstId, 2);
      cCall++;
    }
  }

  // debug, should be half
  printf("%d %d\n", cCall, c->GetEdges()); 
  fprintf(out2, "%d %d\n", cCall, c->GetEdges());

  // last dimension is for 3 modes of counting (0 normal 1 aggregated 2 simple)
  double triCnt[8][8][8][3];
  for (int i=0; i<8; i++)
    for (int j=0; j<8; j++)
      for (int k=0; k<8; k++) 
        for (int l=0; l<3; l++)
          triCnt[i][j][k][l] = 0.0;
    
  // descendants for aggregation
  int numSub[8];
  numSub[1] = 1, numSub[2] = 1, numSub[3] = 3, numSub[4] = 1, numSub[5] = 3, numSub[6] = 3, numSub[7] = 7;
  int subTypes[8][7];
  subTypes[1][0] = 1, subTypes[2][0] = 2, subTypes[3][0] = 1, subTypes[3][1] = 2, subTypes[3][2] = 3, subTypes[4][0] = 4, subTypes[5][0] = 1, subTypes[5][1] = 4, subTypes[5][2] = 5, subTypes[6][0] = 2, subTypes[6][1] = 4, subTypes[6][2] = 6;
  for (int i=0; i<7; i++)
    subTypes[7][i] = i+1;

  printf("3networks %d %d %d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges(), c->GetNodes(), c->GetEdges());
  printf("combined %d %d\n", t2->GetNodes(), t2->GetEdges());
  fprintf(out2, "3networks %d %d %d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges(), c->GetNodes(), c->GetEdges());
  fprintf(out2, "combined %d %d\n", t2->GetNodes(), t2->GetEdges());

  for (TTaobaoShopNet::TNodeI NI = t2->BegNI(); NI < t2->EndNI(); NI++) 
    if (NI.GetOutDeg() > 1) {
      int n1 = NI.GetId();
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int n2 = NI.GetOutNId(i);
        TTaobaoShopEdge e1 = t2->GetEDat(n1, n2);
        int index1 = 0;
        if (e1.isTrade)
          index1 += 1;
        if (e1.isMsg)
          index1 += 2;
        if (e1.isCon)
          index1 += 4;
        
        for (int j=i+1; j<NI.GetOutDeg(); j++) { // no 2x counting
          int n3 = NI.GetOutNId(j);
          TTaobaoShopEdge e2 = t2->GetEDat(n1, n3);
          int index2 = 0;
          if (e2.isTrade)
            index2 += 1;
          if (e2.isMsg)
            index2 += 2;
          if (e2.isCon)
            index2 += 4;
          
          int low, high;
          if (index1 <= index2) {
            low = index1;
            high = index2;
          }
          else {
            low = index2;
            high = index1;
          }

          int thirdEdge = 0;
          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            int index3 = 0;
            if (e3.isTrade)
              index3 += 1;
            if (e3.isMsg)
              index3 += 2;
            if (e3.isCon)
              index3 += 4;
           
            triCnt[low][high][index3][0] += 1.0;
            thirdEdge = index3;
          }
          else {
            triCnt[low][high][0][0] += 1.0;
          }

          if (e1.isTrade) {
            if (e2.isTrade)
              triCnt[0][0][thirdEdge][2] += 1.0;
            if (e2.isMsg)
              triCnt[0][1][thirdEdge][2] += 1.0;
            if (e2.isCon)
              triCnt[0][2][thirdEdge][2] += 1.0;
          }

          if (e1.isMsg) {
            if (e2.isTrade)
              triCnt[0][1][thirdEdge][2] += 1.0; // 1 0
            if (e2.isMsg)
              triCnt[1][1][thirdEdge][2] += 1.0;
            if (e2.isCon)
              triCnt[1][2][thirdEdge][2] += 1.0;
          }

          if (e1.isCon) {
            if (e2.isTrade)
              triCnt[0][2][thirdEdge][2] += 1.0; // 2 0
            if (e2.isMsg)
              triCnt[1][2][thirdEdge][2] += 1.0; // 2 1
            if (e2.isCon)
              triCnt[2][2][thirdEdge][2] += 1.0;
          }

          // aggregated counting
          for (int k=0; k<numSub[index1]; k++)
            for (int l=0; l<numSub[index2]; l++) {
              int d1, d2;
              if (subTypes[index1][k] < subTypes[index2][l]) {
                d1 = subTypes[index1][k];
                d2 = subTypes[index2][l];
              }
              else {
                d2 = subTypes[index1][k];
                d1 = subTypes[index2][l];
              }
              triCnt[d1][d2][thirdEdge][1] += 1.0;
            }
              
        }
      }
    }

  // should be able to do loop on 4th dimension, replace 0 with l
  FILE *out = fopen("triCntPEdges.outUNDIR", "w");
  
  for (int l=0; l<3; l++) { // for normal and aggregated
    double tempSum[8][8][3];
    for (int i=0; i<8; i++)
      for (int j=0; j<8; j++)
        for (int k=0; k<3; k++)
          tempSum[i][j][k] = 0.0;
    
    int maxS;
    if (l<2) 
      maxS = 8;
    else
      maxS = 3;
    
    // last 3 nums are # of 3rd edges containing each type
    fprintf(out, "e1 e2 total fail success 1 2 3 4 5 6 7 trade message contact\n");
    for (int i=0; i<maxS; i++)
      for (int j=0; j<maxS; j++) {
        double tot = 0.0;
        for (int k=0; k<8; k++)
          tot += triCnt[i][j][k][l];
        
        if (tot > 0.0) { // equiv to j >= i
          fprintf(out, "%d %d %.0lf %.01f %.01f ", i, j, tot, triCnt[i][j][0][l], tot-triCnt[i][j][0][l]);
          for (int k=1; k<8; k++)
            fprintf(out, "%.0lf ", triCnt[i][j][k][l]);
          tempSum[i][j][0] = triCnt[i][j][1][l]+triCnt[i][j][3][l]+triCnt[i][j][5][l]+triCnt[i][j][7][l];
          tempSum[i][j][1] = triCnt[i][j][2][l]+triCnt[i][j][3][l]+triCnt[i][j][6][l]+triCnt[i][j][7][l];
          tempSum[i][j][2] = triCnt[i][j][4][l]+triCnt[i][j][5][l]+triCnt[i][j][6][l]+triCnt[i][j][7][l];
          fprintf(out, "%.0lf %.0lf %.0lf\n", tempSum[i][j][0], tempSum[i][j][1], tempSum[i][j][2]);
        }
      }
    fprintf(out, "\n");

    // last 3 nums are % of 3rd edge containing each type
    fprintf(out, "e1 e2 probSuccess max 1 2 3 4 5 6 7 trade message contact\n");
    for (int i=0; i<maxS; i++)
      for (int j=0; j<maxS; j++) {
        double tot = 0.0;
        int maxK=1;
        for (int k=0; k<8; k++) {
          tot += triCnt[i][j][k][l];
          if (k>0 && triCnt[i][j][k][l] > triCnt[i][j][maxK][l])
            maxK = k;
        }
        
        if (tot > 0.0) { // equiv to j >= i
          double nonZero = tot-triCnt[i][j][0][l];
          fprintf(out, "%d %d %lf %d ", i, j, nonZero/tot, maxK);
          for (int k=1; k<8; k++)
            fprintf(out, "%lf ", triCnt[i][j][k][l]/nonZero);
          fprintf(out, "%lf %lf %lf\n", tempSum[i][j][0]/nonZero, tempSum[i][j][1]/nonZero, tempSum[i][j][2]/nonZero);
        }
      }
    fprintf(out, "\n");
    
    // 1st way to do first block
    fprintf(out, "e3 max1 max2 e1 e2 percentage\n");
    for (int i=0; i<8; i++) {
      double tot=0;
      int maxJ=1;
      int maxK=1;
      for (int j=1; j<maxS; j++)
        for (int k=j; k<maxS; k++) {
          tot += triCnt[j][k][i][l];
          if (triCnt[j][k][i][l] > triCnt[maxJ][maxK][i][l]) {
            maxJ = j;
            maxK = k;
          }
        }
      
      fprintf(out, "%d %d %d ", i, maxJ, maxK);
      for (int j=1; j<maxS; j++)
        for (int k=j; k<maxS; k++)
          fprintf(out, "%d %d %lf ", j, k, triCnt[j][k][i][l]/tot);
      fprintf(out, "\n");
    }
    fprintf(out, "\n");
    
    // second way to do 3rd block
    fprintf(out, "e3 max1 max2 e1 e2 percentage\n");
    for (int i=0; i<3; i++) {
      double tot=0;
      int maxJ=1;
      int maxK=1;
      for (int j=1; j<maxS; j++)
        for (int k=j; k<maxS; k++) {
          tot += tempSum[j][k][i];
          if (tempSum[j][k][i] > tempSum[maxJ][maxK][i]) {
            maxJ = j;
            maxK = k;
          }
        }
      
      if (i == 0)
        fprintf(out, "trade ");
      else if (i==1)
        fprintf(out, "message ");
      else
        fprintf(out, "contact ");
      fprintf(out, "%d %d ", maxJ, maxK);
      for (int j=1; j<maxS; j++)
        for (int k=j; k<maxS; k++)
          fprintf(out, "%d %d %lf ", j, k, tempSum[j][k][i]/tot);
      fprintf(out, "\n");
    }
    fprintf(out, "\n");
    
  } // close l

  fclose(out);
  fclose(out2);

  //nodePairCounting(t);
  
  // T = trade, M = message, C = contact, N = nothing
  // order: C M T N

  /*

  printf("converting...\n");

  //printf("%d %d\n", t->shopNet->GetNodes(), t->shopNet->GetEdges());

  // undirect the 3 graphs
  PUNGraph s = TSnap::ConvertGraph<PUNGraph>((PTaobaoShopNet) t->shopNet, false);
  PUNGraph m = TSnap::ConvertGraph<PUNGraph>((PTaobaoShopNet) t->msgNet, false);
  PUNGraph c = TSnap::ConvertGraph<PUNGraph>((PTaobaoShopNet) t->contactNet, false);

  printf("after %d %d %d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges(), c->GetNodes(), c->GetEdges());

  double triCnt[3][3][4];
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      for (int k=0; k<4; k++)                  
        triCnt[i][j][k] = 0.0;
 
  PUNGraph x;
  //PTaobaoShopNet x;
  for (int k=0; k<3; k++) {
    if (k == 0)
      x = c;
    else if (k == 1)
      x = m;
    else if (k == 2)
      x = s;
    
    // the loops below are wrong, we should be looking at two edges coming out
    // of node1. the /2 optimization is incorrect

    // formerly TTaobaoShopNet::TNodeI NI
    for (TUNGraph::TNodeI NI = x->BegNI(); NI < x->EndNI(); NI++) {
      int node1 = NI.GetId();
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int node2 = NI.GetOutNId(i);
        
        if (k==0 && c->IsNode(node2)) {
          TUNGraph::TNodeI NJ = c->GetNI(node2);
          int begJ;
          if (k==0) // no 2x counting for con-con
            begJ=i+1;
          else
            begJ=0;
          for (int j=begJ; j<NJ.GetOutDeg(); j++) {
            int node3 = NJ.GetOutNId(j);
            if (node3 != node1) {
              int fnd=0;
              if (c->IsNode(node3) && c->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][0][0] += 1.0;
              }
              if (m->IsNode(node3) && m->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][0][1] += 1.0;
              }
              if (s->IsNode(node3) && s->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][0][2] += 1.0;
              }
              if (fnd == 0)
                triCnt[k][0][3] += 1.0;
            }
          }
        }
        
        if (k<2 && m->IsNode(node2)) {
          TUNGraph::TNodeI NJ = m->GetNI(node2);
          int begJ;
          if (k==1)
            begJ=i+1;
          else
            begJ=0;
          for (int j=begJ; j<NJ.GetOutDeg(); j++) {
            int node3 = NJ.GetOutNId(j);
            if (node3 != node1) {
              int fnd=0;
              if (c->IsNode(node3) && c->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][1][0] += 1.0;
              }
              if (m->IsNode(node3) && m->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][1][1] += 1.0;
              }
              if (s->IsNode(node3) && s->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][1][2] += 1.0;
              }
              if (fnd == 0)
                triCnt[k][1][3] += 1.0;
            }
          }
        }
      
        if (s->IsNode(node2)) {
          TUNGraph::TNodeI NJ = s->GetNI(node2);
          int begJ;
          if (k==2)
            begJ=i+1;
          else
            begJ=0;
          for (int j=begJ; j<NJ.GetOutDeg(); j++) {
            int node3 = NJ.GetOutNId(j);
            if (node3 != node1) {
              int fnd=0;
              if (c->IsNode(node3) && c->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][2][0] += 1.0;
              }
              if (m->IsNode(node3) && m->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][2][1] += 1.0;
              }
              if (s->IsNode(node3) && s->IsEdge(node3, node1)) {
                fnd = 1;
                triCnt[k][2][2] += 1.0;
              }
              if (fnd == 0)
                triCnt[k][2][3] += 1.0;
            }
          }
        }
        

      }
    }

    for (int i=0; i<3; i++) 
      for (int j=0; j<4; j++)
        printf("%lf ", triCnt[k][i][j]);
    printf("\n");
   
    // fill in the blanks
    for (int i=0; i<k; i++)
      for (int j=0; j<4; j++)
        triCnt[k][i][j] = triCnt[i][k][j];
 
    for (int i=0; i<3; i++) {
      double tot =
      triCnt[k][i][0]+triCnt[k][i][1]+triCnt[k][i][2]+triCnt[k][i][3];
      double triTot = tot-triCnt[k][i][3];
      printf("%d %d probClose %lf break %lf %lf %lf\n", k, i, triTot/tot,
             triCnt[k][i][0]/triTot, triCnt[k][i][1]/triTot, triCnt[k][i][2]/triTot);
    }

  }
  
  */
  
}

// new: look at proportion of third buy edges that close the triangle
void triangleClosed(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet x = t->shopNet; // the third leg of the triangle

  TIntH totMsg; // count of num msg strengths
  TIntH sucMsg; // count of num msg strengths leading to trades  
  int totDay[60]; // total/success by dif between first 2 edges
  int sucDay[60];

  for (int i=0; i<60; i++) {
    sucDay[i] = 0;
    totDay[i] = 0;
  }

  double probSuc = 0.0;
  double total = 0.0;
  double baseline = 0.0;
  double numNodes = (double) x->GetNodes();
 
  double restrProbSuc = 0.0;
  double restrTotal = 0.0;
  double restrBaseline = 0.0;

  double triSuc = 0.0; // sucesssful/possible triangles
  double triTot = 0.0; 

  int windowSize = 2;
  int daySec = 24*60*60;
  int window = windowSize*daySec;
  
  TIntH nodesByWin[60];
  //TIntIntHH nodesByWin; // nodes pointed to in window beginning on this day (x network)
  // use linear pass to fill in this struct,
  // for each edge time, check up to W back 

  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();
  
  int cnt=0;
  // fill in struct with third network msg by day
  for (TTaobaoShopNet::TNodeI NI = x->BegNI(); NI < x->EndNI(); NI++) {
    for (int i=0; i<NI.GetOutDeg(); i++) {
      TTaobaoShopEdge& edge = NI.GetOutEDat(i);
      int sinkID = NI.GetOutNId(i);

      for (int j=0; j<edge.CntTX(); j++) {
        int xSec = edge.GetTime(j).GetAbsSecs();
        if (cnt%100000 == 0)
          printf("%d %d %d\n", xSec, cnt, nodesByWin[(xSec-beginSec)/daySec].Len());
        // mark off window+1 days
        int aMax = (xSec-beginSec)/daySec;
        int aMin;
        if (xSec-window>=beginSec)
          aMin = (xSec-window-beginSec)/daySec;
        else
          aMin = 0;
        for (int a=aMax; a>=aMin; a--) {
          if (!nodesByWin[a].IsKey(sinkID)) {
            nodesByWin[a].AddDat(sinkID);
          }
        }
        cnt++;
      }
      
    }
  }

  for (int a=beginSec; a<=endSec; a+=daySec) {
    int ind = (a-beginSec)/daySec;
    printf("%d %d\n", ind, nodesByWin[ind].Len());
  }

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      TSecTm tm = shopEdge.GetTime(0);
      
      if (m->IsNode(buyerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
        int candMsg = 0;
        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);
        
          int k=0; 
          while (k<msgEdge.CntTX() && msgEdge.GetTime(k)<tm)
            k++;
          
          if (k<msgEdge.CntTX()) { // use first message time
            TSecTm chatTime = msgEdge.GetTime(k);
            int msgCnt = msgEdge.CntTXStrength(); 
            int dayDif = (chatTime.GetAbsSecs()-tm.GetAbsSecs())/daySec;
                          
            if (x->IsNode(msgID)) {
              int candCnt=0;
              int fnd=0; // either 0 or 1
              int tIndex = (chatTime.GetAbsSecs()-beginSec)/daySec;
              TTaobaoShopNet::TNodeI NK = x->GetNI(msgID);
              for (int l=0; l<NK.GetOutDeg(); l++) { 
                int nextID = NK.GetOutNId(l);
                // nodesbywin is a filter to only look at nodes in the time window
                if (nodesByWin[tIndex].IsKey(nextID)) {
                  // forgot one check 
                  // need to make sure nk's timestamp is in window
                  
                  TTaobaoShopEdge& edge = NK.GetOutEDat(l);
                  int okay=0;
                  for (int a=0; a<edge.CntTX(); a++) {
                    // a<1 temp, additional restriction for first time buyer
                    // eg we ignore if they have bought before
                    TSecTm tm3 = edge.GetTime(a);
                    if (tm3 >= chatTime) {
                      if (chatTime.GetAbsSecs()+window >= tm3.GetAbsSecs())
                        okay=1;
                      break;
                    }
                  }
                  
                  if (okay == 1) {
                    candCnt++;
                    if (nextID == sellerID) {
                      fnd = 1;
                      triSuc += 1.0;
                    }
                  }
                }
              }
              
              if (candCnt > 0) {

                if (totMsg.IsKey(msgCnt)) {
                  int existCnt = totMsg.GetDat(msgCnt);
                  totMsg.AddDat(msgCnt, existCnt+candCnt);
                }
                else {
                  totMsg.AddDat(msgCnt, candCnt);
                  sucMsg.AddDat(msgCnt, 0);
                }
                totDay[dayDif] += candCnt;

                if (fnd == 1) {
                  int existCnt = sucMsg.GetDat(msgCnt);
                  sucMsg.AddDat(msgCnt, existCnt+1);
                  sucDay[dayDif] += 1;
                }

                triTot += (double) candCnt;
                double prob = (double)fnd / (double)candCnt;
                probSuc += prob;
                baseline += (double)candCnt/(double)numNodes; // approx
                total += 1.0;
                
                if (fnd==1) {
                  restrProbSuc += (double)1.0/(double)candCnt;
                  restrBaseline += (double)candCnt/(double)numNodes;
                  restrTotal += 1.0;
                }
              }
            }   
          }
        } 
      }   
    }
  }
  
  baseline = baseline / total;
  probSuc = probSuc / total;
  restrBaseline = restrBaseline / restrTotal;
  restrProbSuc = restrProbSuc / restrTotal;
  printf("orig prob %.10lf random %.10lf total %lf\n", probSuc, baseline, total);
  printf("restr prob %.10lf random %.10lf total %lf\n", restrProbSuc, restrBaseline, restrTotal);
  printf("abs successful %lf total %lf prob %.10lf\n", triSuc, triTot, triSuc/triTot);

  totMsg.SortByKey(true);
  sucMsg.SortByKey(true);
  
  //TFltPrV msgProb;
  
  FILE *out = fopen("triangleClosed.outPEDGE", "w");
  
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
    
  for (int i=0; i<totMsg.Len(); i++) { // was < trimLen
    if((int)totMsg.GetKey(i)!=sucMsg.GetKey(i))
      printf("key error\n");
    //float tradeProb = (float)sucMsg[i] / (float)totMsg[i];
    //msgProb.Add(TFltPr((float)totMsg.GetKey(i), tradeProb));
    fprintf(out, "%d %d %d\n", (int)totMsg.GetKey(i), (int)sucMsg[i], (int)totMsg[i]);
  }
  fclose(out);
  
  FILE *out2 = fopen("triangleClosedDay.outPEDGE", "w");
  int maxDays = (endSec-beginSec)/daySec+1;
  for (int i=0; i<maxDays; i++)
    fprintf(out2, "%d %d %d\n", i, sucDay[i], totDay[i]);
  fclose(out2);
}

// redone again... assumes undirected
// just added trianglecounting directly into this
void permuteTypesNew(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  PTaobaoShopNet t2 = new TTaobaoShopNet();

  srand(time(NULL));
  FILE *out2 = fopen("triDebug.out", "w");

  TIntPrV nodePairsV;
  TIntV edgeTypesV; // 7 edge types

  // go through 3 networks
  // add all node pairs into two arrays
  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    if (!(s->IsEdge(DstId, SrcId)&& DstId>SrcId)) {
      int index=1;
      bool isM = m->IsEdge(SrcId, DstId) || m->IsEdge(DstId, SrcId);
      bool isC = c->IsEdge(SrcId, DstId);
      if (isM)
        index += 2;
      if (isC)
        index += 4;
      // add arbitrary direction
      nodePairsV.Add(TIntPr(SrcId, DstId));
      edgeTypesV.Add(index);
    }
  }

  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    if (!(m->IsEdge(DstId, SrcId)&& DstId>SrcId)) {
      int index=2;
      bool isS = s->IsEdge(SrcId, DstId) || s->IsEdge(DstId, SrcId);
      if (!isS) { // no trade edge
        bool isC = c->IsEdge(SrcId, DstId);
        if (isC)
          index += 4;
        // add arbitrary direction
        nodePairsV.Add(TIntPr(SrcId, DstId));
        edgeTypesV.Add(index);
      }
    }
  }

  for (TTaobaoShopNet::TEdgeI EI = c->BegEI(); EI < c->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    if (SrcId < DstId) {
      int index=4;
      bool isS = s->IsEdge(SrcId, DstId) || s->IsEdge(DstId, SrcId);
      bool isM = m->IsEdge(SrcId, DstId) || m->IsEdge(DstId, SrcId);
      if (!isS && !isM) { // contact-only
        // add arbitrary direction
        nodePairsV.Add(TIntPr(SrcId, DstId));
        edgeTypesV.Add(index);
      }
    }
  }
  
  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int ID = NI.GetId();
    TTaobaoUsr newUsr = TTaobaoUsr( false, false);
    t2->AddNode(ID, newUsr);
  }

  int cnt1=0;
  for (TTaobaoShopNet::TNodeI NI = m->BegNI(); NI < m->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
      cnt1++;
    }
  }

  int cnt2=0;
  for (TTaobaoShopNet::TNodeI NI = c->BegNI(); NI < c->EndNI(); NI++) {
    int ID = NI.GetId();
    if (!t2->IsNode(ID)) {
      TTaobaoUsr newUsr = TTaobaoUsr( false, false);
      t2->AddNode(ID, newUsr);
      cnt2++;
    }
  }

  // not sure why cnt1, cnt2 can be nonzero
  // make sure nothing wrong with permuteEdges
  printf("len %d cnt1 %d cnt2 %d\n", nodePairsV.Len(), cnt1, cnt2);
  edgeTypesV.Shuffle(TInt::Rnd);
  for (int i=0; i<edgeTypesV.Len(); i++) {
    TTaobaoShopEdge e = TTaobaoShopEdge();
    e.edgeType = (int) edgeTypesV[i];
    t2->AddEdge((int)nodePairsV[i].Val1, (int)nodePairsV[i].Val2, e);
    e = TTaobaoShopEdge();
    e.edgeType = (int) edgeTypesV[i];
    t2->AddEdge((int)nodePairsV[i].Val2, (int)nodePairsV[i].Val1, e);
  }

  // APR: create another network and pass to triangleCounting()

  PTaobaoNet t3 = TTaobaoNet::New();
  TTaobaoShopNet *s3 = new TTaobaoShopNet();
  TTaobaoShopNet *m3 = new TTaobaoShopNet();
  TTaobaoShopNet *c3 = new TTaobaoShopNet();
  
  t3->shopNet = s3;
  t3->msgNet = m3;
  t3->contactNet = c3;

  // go through edges of t2, add to t3
  for (TTaobaoShopNet::TEdgeI EI = t2->BegEI(); EI < t2->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    TTaobaoShopEdge& EdgeDat = EI();
    int edgeType = EdgeDat.edgeType;
    
    if (edgeType==1 || edgeType==3 || edgeType==5 || edgeType==7) {
      if (!s3->IsNode(SrcId)){
        TTaobaoUsr NewUsr = TTaobaoUsr(true, false);
        s3->AddNode(SrcId, NewUsr);
      }

      if (!s3->IsNode(DstId)){
        TTaobaoUsr NewUsr = TTaobaoUsr(false, true);
        s3->AddNode(DstId, NewUsr);
      }

      if (!s3->IsEdge(SrcId, DstId)) {
        TTaobaoShopEdge NewEdge = TTaobaoShopEdge();
        s3->AddEdge(SrcId, DstId, NewEdge);
      }

      if (!s3->IsEdge(DstId, SrcId)) {
        TTaobaoShopEdge NewEdge = TTaobaoShopEdge();
        s3->AddEdge(DstId, SrcId, NewEdge);
      }
    }

    if (edgeType==2 || edgeType==3 || edgeType==6 || edgeType==7) {
      if (!m3->IsNode(SrcId)){
        TTaobaoUsr NewUsr = TTaobaoUsr(false, false);
        m3->AddNode(SrcId, NewUsr);
      }

      if (!m3->IsNode(DstId)){
        TTaobaoUsr NewUsr = TTaobaoUsr(false, false);
        m3->AddNode(DstId, NewUsr);
      }

      if (!m3->IsEdge(SrcId, DstId)) {
        TTaobaoShopEdge NewEdge = TTaobaoShopEdge();
        m3->AddEdge(SrcId, DstId, NewEdge);
      }

      if (!m3->IsEdge(DstId, SrcId)) {
        TTaobaoShopEdge NewEdge = TTaobaoShopEdge();
        m3->AddEdge(DstId, SrcId, NewEdge);
      }
    }

    if (edgeType==4 || edgeType==5 || edgeType==6 || edgeType==7) {
      if (!c3->IsNode(SrcId)){
        TTaobaoUsr NewUsr = TTaobaoUsr(false, false);
        c3->AddNode(SrcId, NewUsr);
      }

      if (!c3->IsNode(DstId)){
        TTaobaoUsr NewUsr = TTaobaoUsr(false, false);
        c3->AddNode(DstId, NewUsr);
      }

      if (!c3->IsEdge(SrcId, DstId)) {
        TTaobaoShopEdge NewEdge = TTaobaoShopEdge();
        c3->AddEdge(SrcId, DstId, NewEdge);
      }

      if (!c3->IsEdge(DstId, SrcId)) {
        TTaobaoShopEdge NewEdge = TTaobaoShopEdge();
        c3->AddEdge(DstId, SrcId, NewEdge);
      }
    }
  }

  //basicStats(t3);

  triangleCountingNew(t3);

  //triangleCounting(t3);

  /*
  double triCnt[8][8][8];
  for (int i=0; i<8; i++)
    for (int j=0; j<8; j++)
      for (int k=0; k<8; k++)
        triCnt[i][j][k] = 0.0;

  printf("3networks %d %d %d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges(), c->GetNodes(), c->GetEdges());
  printf("combined %d %d\n", t2->GetNodes(), t2->GetEdges());
  fprintf(out2, "3networks %d %d %d %d %d %d\n", s->GetNodes(), s->GetEdges(), m->GetNodes(), m->GetEdges(), c->GetNodes(), c->GetEdges());
  fprintf(out2, "combined %d %d\n", t2->GetNodes(), t2->GetEdges());

  for (TTaobaoShopNet::TNodeI NI = t2->BegNI(); NI < t2->EndNI(); NI++) 
    if (NI.GetOutDeg() > 1) {
      int n1 = NI.GetId();
      for (int i=0; i<NI.GetOutDeg(); i++) {
        int n2 = NI.GetOutNId(i);
        TTaobaoShopEdge e1 = t2->GetEDat(n1, n2);
        int index1 = e1.edgeType;
        
        for (int j=i+1; j<NI.GetOutDeg(); j++) { // no 2x counting
          int n3 = NI.GetOutNId(j);
          TTaobaoShopEdge e2 = t2->GetEDat(n1, n3);
          int index2 = e2.edgeType;
                  
          int low, high;
          if (index1 <= index2) {
            low = index1;
            high = index2;
          }
          else {
            low = index2;
            high = index1;
          }

          if (t2->IsEdge(n2, n3)) {
            TTaobaoShopEdge e3 = t2->GetEDat(n2, n3);
            int index3 = e3.edgeType;
                       
            triCnt[low][high][index3] += 1.0;
          }
          else {
            triCnt[low][high][0] += 1.0;
          }

        }
      }
    }

  // not up to date, see triangleCounting for new changes

  FILE *out = fopen("triCntPTypes.outAPR", "w");

  for (int i=0; i<8; i++)
    for (int j=0; j<8; j++) {
      double tot = 0.0;
      for (int k=0; k<8; k++)
        tot += triCnt[i][j][k];
      
      if (tot > 0.0) {
        fprintf(out, "%d %d %.0lf ", i, j, tot);
        for (int k=0; k<8; k++)
          fprintf(out, "%.0lf ", triCnt[i][j][k]);
        fprintf(out, "\n");
      }
    }
  
  fprintf(out, "\n");

  for (int i=0; i<8; i++)
    for (int j=0; j<8; j++) {
      double tot = 0.0;
      int maxK=1;
      for (int k=0; k<8; k++) {
        tot += triCnt[i][j][k];
        if (k>0 && triCnt[i][j][k] > triCnt[i][j][maxK])
          maxK = k;
      }

      if (tot > 0.0) {
        double nonZero = tot-triCnt[i][j][0];
        fprintf(out, "%d %d probClose %lf max %d ", i, j, nonZero/tot, maxK);
        for (int k=1; k<8; k++)
          fprintf(out, "%lf ", triCnt[i][j][k]/nonZero);
        fprintf(out, "\n");
      }
    }

  fprintf(out, "\n");

  for (int i=0; i<8; i++) {
    double tot=0;
    int maxJ=1;
    int maxK=1;
    for (int j=1; j<8; j++)
      for (int k=j; k<8; k++) {
        tot += triCnt[j][k][i];
        if (triCnt[j][k][i] > triCnt[maxJ][maxK][i]) {
          maxJ = j;
          maxK = k;
        }
      }

    fprintf(out, "closing %d max %d %d raw ", i, maxJ, maxK);
    for (int j=1; j<8; j++)
      for (int k=j; k<8; k++)
        fprintf(out, "%d %d %lf ", j, k, triCnt[j][k][i]/tot);
    fprintf(out, "\n");
  }
    
  fclose(out);
  fclose(out2);
  */
}

// old graph = t, new graph = t2
void permuteTypes(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet; 

  PTaobaoNet t2 = TTaobaoNet::New();
  TTaobaoShopNet *s2 = new TTaobaoShopNet();
  TTaobaoShopNet *m2 = new TTaobaoShopNet();
  TTaobaoShopNet *c2 = new TTaobaoShopNet();
  
  t2->shopNet = s2;
  t2->msgNet = m2;
  t2->contactNet = c2;

  srand(time(NULL));

  // create array of (x,y) pairs, contact edges add only in one direction
  // forward or reverse pair
  // create second array of edge types, permute edge types
  // if contact is assigned to a directed edge, add reverse contact edge as well

  FILE *out = fopen("pDebug.out", "w");
  
  int autoAddCon=0;
  int autoAddMsg=0;
  int autoAddTrade=0;
  int conCnt=0;
  int how1=0, how2=0; // debug 
  TIntPrV nodePairsV;
  TIntV edgeTypesV; // 0=trade, 1=msg, 2=contact
  // go through trade graph, get tmc, tm, tc, t only
  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    bool isM = m->IsEdge(SrcId, DstId);
    bool isC = c->IsEdge(SrcId, DstId);
    if (isM && isC) { // add all 3 edges into t2, since no variation possible
      TTaobaoUsr NewUsr;
      TTaobaoShopEdge EdgeDat;
      if (!s2->IsNode(SrcId)){
        NewUsr = TTaobaoUsr(true, false); 
        s2->AddNode(SrcId, NewUsr);
      }
      if (!s2->IsNode(DstId)){
        NewUsr = TTaobaoUsr(false, true);
        s2->AddNode(DstId, NewUsr);
      }
      if (!s2->IsEdge(SrcId, DstId)) {
        EdgeDat = TTaobaoShopEdge();
        s2->AddEdge(SrcId, DstId, EdgeDat);
        autoAddTrade++;
      }
      else
        how1++;

      if (!m2->IsNode(SrcId)){
        NewUsr = TTaobaoUsr(false, false);
        m2->AddNode(SrcId, NewUsr);
      }
      if (!m2->IsNode(DstId)){
        NewUsr = TTaobaoUsr(false, false);
        m2->AddNode(DstId, NewUsr);
      }
      if (!m2->IsEdge(SrcId, DstId)) {
        EdgeDat = TTaobaoShopEdge();
        m2->AddEdge(SrcId, DstId, EdgeDat);
        autoAddMsg++;
      }
      else
        how2++;

      if (!c2->IsNode(SrcId)){
        NewUsr = TTaobaoUsr(false, false);
        c2->AddNode(SrcId, NewUsr);
      }
      if (!c2->IsNode(DstId)){
        NewUsr = TTaobaoUsr(false, false);
        c2->AddNode(DstId, NewUsr);
      }
      if (!c2->IsEdge(SrcId, DstId)) {
        EdgeDat = TTaobaoShopEdge();
        c2->AddEdge(SrcId, DstId, EdgeDat);
        //EdgeDat = TTaobaoShopEdge();

        if (SrcId == DstId) { // shouldnt happen
          printf("same src snk %d\n", SrcId);
          exit(0);
        }

        if (c2->IsEdge(DstId, SrcId)) { // this shouldn't happen
          printf("c2 error %d %d\n", SrcId, DstId);
          exit(0);
        }

        c2->AddEdge(DstId, SrcId, EdgeDat);
        autoAddCon++;

      }
    }
    else {
      // add trade
      nodePairsV.Add(TIntPr(SrcId, DstId));
      edgeTypesV.Add(0);

      if (isM) { // if message
        nodePairsV.Add(TIntPr(SrcId, DstId));
        edgeTypesV.Add(1);
      }
      else if (isC) { // if contact
        if (!c2->IsEdge(SrcId, DstId) && SrcId < DstId) { // only consider one dir
          if (rand()%2==0) // 50-50 direction if replaced with another type
            nodePairsV.Add(TIntPr(SrcId, DstId));
          else
            nodePairsV.Add(TIntPr(DstId, SrcId));
       
          if (SrcId<0 || DstId <0) {
            printf("wtf %d %d\n", SrcId, DstId);
            exit(0);
          }

          edgeTypesV.Add(2);
          conCnt++;
        }
      }
    } 
  }

  printf("how %d %d\n", how1, how2); // should be 0's

  // go through msg graph, get mc, m only
  for (TTaobaoShopNet::TEdgeI EI = m->BegEI(); EI < m->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    bool isS = s->IsEdge(SrcId, DstId);
    bool isC = c->IsEdge(SrcId, DstId);
    if (!isS) { 
      // add msg
      nodePairsV.Add(TIntPr(SrcId, DstId));
      edgeTypesV.Add(1);

      if (isC) {
        if (!c2->IsEdge(SrcId, DstId) && SrcId < DstId) {
          if (rand()%2==0)
            nodePairsV.Add(TIntPr(SrcId, DstId));
          else
            nodePairsV.Add(TIntPr(DstId, SrcId));
          
          if (SrcId<0 || DstId <0) {
            printf("wtf2 %d %d\n", SrcId, DstId);
            exit(0);
          }

          edgeTypesV.Add(2);
          conCnt++;
        }
      }
    }
  }

  // go through con graph, get contact only pairs
  for (TTaobaoShopNet::TEdgeI EI = c->BegEI(); EI < c->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    if (!c2->IsEdge(SrcId, DstId) && SrcId < DstId) { // only need to consider these
      bool isS = s->IsEdge(SrcId, DstId);
      bool isM = m->IsEdge(SrcId, DstId);
      if (!isS && !isM) {
        if (rand()%2==0)
          nodePairsV.Add(TIntPr(SrcId, DstId));
        else
          nodePairsV.Add(TIntPr(DstId, SrcId));
        
        if (SrcId<0 || DstId <0) {
          printf("wtf3 %d %d\n", SrcId, DstId);
          exit(0);
        }

        edgeTypesV.Add(2);
        conCnt++;
      }
    }
  }

  // debug, half of a contact pair in the graph
  FILE *out3 = fopen("errors.out", "w");
  for (TTaobaoShopNet::TEdgeI EI = c2->BegEI(); EI<c2->EndEI(); EI++) {
    int SrcId = EI.GetSrcNId();
    int DstId = EI.GetDstNId();
    if (!c2->IsEdge(DstId, SrcId)) 
      fprintf(out3, "%d %d\n", SrcId, DstId);
  }
  fclose(out3);
  
  int dupS, dupM, dupC;
  TIntV flagV; // 0 means retain for next iteration
  
  // comment out new block for pure random
  // we add random first in this case

  
  // begin new block
  int conMatch=0;
  for (int i=0; i<nodePairsV.Len(); i++)
    flagV.Add(0);

  printf("lens %d %d\n", edgeTypesV.Len(), nodePairsV.Len());
  fprintf(out, "lens %d %d\n", edgeTypesV.Len(), nodePairsV.Len());

  int cError=0;
  for (int i=0; i<conCnt; i++) {
    int iter=0;
    while (iter<100000) { // give up after 100k tries
      int rIndex = (rand())%((int)nodePairsV.Len());
      if (flagV[rIndex]==0) {
        int SrcId = (int) nodePairsV[rIndex].Val1;
        int SnkId = (int) nodePairsV[rIndex].Val2;
        if (SrcId<0 || SnkId <0) {
          printf("wtf4 %d %d %d\n", rIndex, SrcId, SnkId);
          exit(0);
        }
        if (!c2->IsEdge(SrcId, SnkId)) { // should only need to check 1 dir
          TTaobaoUsr NewUsr;
          TTaobaoShopEdge EdgeDat;
          if (!c2->IsNode(SrcId)){
            NewUsr = TTaobaoUsr(false, false);
            c2->AddNode(SrcId, NewUsr);
          }
          if (!c2->IsNode(SnkId)){
            NewUsr = TTaobaoUsr(false, false);
            c2->AddNode(SnkId, NewUsr);
          }

          EdgeDat = TTaobaoShopEdge();
          c2->AddEdge(SrcId, SnkId, EdgeDat);
          //EdgeDat = TTaobaoShopEdge();
          if (c2->IsEdge(SnkId, SrcId)) { 
            // this shouldn't be triggering, but its happening
            fprintf(out, "c error %d %d\n", SrcId, SnkId);
            cError++;
          }
          else {
            c2->AddEdge(SnkId, SrcId, EdgeDat);
          }

          flagV[rIndex] = 1; 
          conMatch++;
          break;
        }
        else {
          flagV[rIndex] = 2; // unusable, for speedup
        }
      }
      iter++;
    }
  }

  // cerror should be zero, but sometimes triggering?
  printf("cerror %d\n", cError);
  printf("conmatch %d conCnt %d\n", conMatch, conCnt);
  printf("%d %d\n", c2->GetNodes(), c2->GetEdges());
  printf("auto %d our edgePairs %d orig edges %d\n", autoAddCon, conCnt+autoAddCon, c->GetEdges());
  fprintf(out, "cerror %d\n", cError);
  fprintf(out, "conmatch %d conCnt %d\n", conMatch, conCnt);
  fprintf(out, "%d %d\n", c2->GetNodes(), c2->GetEdges());
  fprintf(out, "auto %d our edgePairs %d orig edges %d\n", autoAddCon, conCnt+autoAddCon, c->GetEdges());

  TIntPrV nodePairsVT;
  TIntV edgeTypesVT;
  for (int i=0; i<flagV.Len(); i++)
    if (flagV[i] != 1) 
      nodePairsVT.Add(TIntPr((int)nodePairsV[i].Val1, (int)nodePairsV[i].Val2));

  for (int i=0; i<flagV.Len(); i++)
    if (edgeTypesV[i] != 2) // add all non-contacts
      edgeTypesVT.Add(edgeTypesV[i]);


  for (int i=0; i<conCnt-conMatch; i++)
    edgeTypesVT.Add(2); // add unplaced contacts

  if (nodePairsVT.Len() != edgeTypesVT.Len()) {
    printf("error\n");
    exit(0);
  }

  nodePairsV.Clr();
  edgeTypesV.Clr();
  
  for (int i=0; i<nodePairsVT.Len(); i++) {
    nodePairsV.Add(TIntPr((int)nodePairsVT[i].Val1, (int)nodePairsVT[i].Val2));
    edgeTypesV.Add(edgeTypesVT[i]);
  }
  nodePairsVT.Clr();
  edgeTypesVT.Clr();
  // end new block
  

  // begin old block
 
  int toAssignTrade=0, toAssignMsg=0;
  for (int i=0; i<edgeTypesV.Len(); i++) {
    if (edgeTypesV[i] == 0)
      toAssignTrade++;
    if (edgeTypesV[i]==1)
      toAssignMsg++;
  }

  // these should be equal
  printf("auto %d %d toAssign+Auto %d %d original %d %d\n", autoAddTrade,
  autoAddMsg, autoAddTrade+toAssignTrade, autoAddMsg+toAssignMsg, s->GetEdges(), m->GetEdges());

  fprintf(out, "auto %d %d toAssign+Auto %d %d original %d %d\n", autoAddTrade,
         autoAddMsg, autoAddTrade+toAssignTrade, autoAddMsg+toAssignMsg, s->GetEdges(), m->GetEdges());

  FILE *out2 = fopen("iterDebug.out", "w");
  int iteration=0;
  while (true) {
    fprintf(out2, "edge types to assign: %d\n", edgeTypesV.Len());
  edgeTypesV.Shuffle(TInt::Rnd);

  dupS=0;
  dupM=0;
  dupC=0;
  flagV.Clr();
  // create new graph from permuted array
  for (int i=0; i<edgeTypesV.Len(); i++) {
    int SrcId = (int)nodePairsV[i].Val1;
    int DstId = (int)nodePairsV[i].Val2;
    TTaobaoUsr NewUsr;
    TTaobaoShopEdge EdgeDat;
    if (edgeTypesV[i] == 0) { // trade
      if (!s2->IsEdge(SrcId, DstId)) {
        if (!s2->IsNode(SrcId)){
          NewUsr = TTaobaoUsr(true, false);
          s2->AddNode(SrcId, NewUsr);
        }
        if (!s2->IsNode(DstId)){
          NewUsr = TTaobaoUsr(false, true);
          s2->AddNode(DstId, NewUsr);
        }
        EdgeDat = TTaobaoShopEdge();
        s2->AddEdge(SrcId, DstId, EdgeDat);
        flagV.Add(1);
        //repeated deletions is too slow
        //edgeTypesV.Del(i); // new
        //nodePairsV.Del(i);
      }
      else {
        flagV.Add(0);
        dupS++;
      }
    }
    else if (edgeTypesV[i] == 1) { // msg
      if (!m2->IsEdge(SrcId, DstId)) {
        if (!m2->IsNode(SrcId)){
          NewUsr = TTaobaoUsr(false, false);
          m2->AddNode(SrcId, NewUsr);
        }
        if (!m2->IsNode(DstId)){
          NewUsr = TTaobaoUsr(false, false);
          m2->AddNode(DstId, NewUsr);
        }
        EdgeDat = TTaobaoShopEdge();
        m2->AddEdge(SrcId, DstId, EdgeDat);
        flagV.Add(1);
      }
      else {
        dupM++;
        flagV.Add(0);
      }
    }
    else { // contact
      if (!c2->IsEdge(SrcId, DstId)) {
        if (!c2->IsNode(SrcId)){
          NewUsr = TTaobaoUsr(false, false);
          c2->AddNode(SrcId, NewUsr);
        }
        if (!c2->IsNode(DstId)){
          NewUsr = TTaobaoUsr(false, false);
          c2->AddNode(DstId, NewUsr);
        }
        EdgeDat = TTaobaoShopEdge();
        c2->AddEdge(SrcId, DstId, EdgeDat);
        EdgeDat = TTaobaoShopEdge();
        c2->AddEdge(DstId, SrcId, EdgeDat);
        flagV.Add(1);
      }
      else {
        flagV.Add(0);
        dupC++;
      }
    }
  }

  // duplications occur when same edge type mapped twice to a pair of nodes, or contacts

  fprintf(out2, "original %d %d %d %d %d %d\n", s->GetNodes(), s->GetEdges(),
         m->GetNodes(), m->GetEdges(), c->GetNodes(), c->GetEdges());
  fprintf(out2, "flag %d duplications %d %d %d\n", flagV.Len(), dupS, dupM, dupC);
  fprintf(out2, "new %d %d %d %d %d %d\n", s2->GetNodes(), s2->GetEdges(),
         m2->GetNodes(), m2->GetEdges(), c2->GetNodes(), c2->GetEdges());

  int totDup = dupS+dupM+dupC;
  if (totDup ==0 || iteration==10)
    break;
  
  TIntPrV nodePairsVTemp;
  TIntV edgeTypesVTemp; 
  for (int i=0; i<flagV.Len(); i++)
    if (flagV[i]==0) {
      nodePairsVTemp.Add(TIntPr((int)nodePairsV[i].Val1, (int)nodePairsV[i].Val2));
      edgeTypesVTemp.Add(edgeTypesV[i]);
    }
  nodePairsV.Clr();
  edgeTypesV.Clr();
  for (int i=0; i<nodePairsVTemp.Len(); i++) {
    nodePairsV.Add(TIntPr((int)nodePairsVTemp[i].Val1, (int)nodePairsVTemp[i].Val2));
    edgeTypesV.Add(edgeTypesVTemp[i]);
  }
  nodePairsVTemp.Clr();
  edgeTypesVTemp.Clr();
  iteration++;
  } // end while true
  fclose(out2);
  fclose(out);

  //printf("%d %d\n", t2->shopNet->GetNodes(), t2->shopNet->GetEdges());

  //nodePairCounting(t2);
  //basicStats(t2);
  triangleCounting(t2);

  // same edge types onto the same edge
  // eg in original graph, if 3 types of edges on an edge, it must stay the
  // after permutation, if you have 2 of the same type between a pair of nodes,
  // just ignore the latter

  // 2nd way of doing this, preserve bidirectionality of msg/trade as well, use
  // 3rd array. this will preserve reciprocity
}

void basicStats(PTaobaoNet t) {

  //printf("%d %d %d\n", t->shopNet->GetNodes(), t->msgNet->GetNodes(), t->contactNet->GetNodes());
  //printf("%d %d %d\n", t->shopNet->GetEdges(), t->msgNet->GetEdges(), t->contactNet->GetEdges());

  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  /*
  printf("Directed\n");

  TSnap::PrintInfo(s, "Graph info");
  PTaobaoShopNet MxWccS = TSnap::GetMxWcc(s);
  TSnap::PrintInfo(MxWccS, "Largest Weakly connected component");
  //float ClstCfS = TSnap::GetClustCf(t->shopNet);
  //printf("Clustering coefficient: %.5f\n", ClstCfS);

  TSnap::PrintInfo(m, "Graph info");
  PTaobaoShopNet MxWccM = TSnap::GetMxWcc(m);
  TSnap::PrintInfo(MxWccM, "Largest Weakly connected component");
  //float ClstCfM = TSnap::GetClustCf(t->msgNet);
  //printf("Clustering coefficient: %.5f\n", ClstCfM);

  TSnap::PrintInfo(c, "Graph info");
  PTaobaoShopNet MxWccC = TSnap::GetMxWcc(c);
  TSnap::PrintInfo(MxWccC, "Largest Weakly connected component");
  //float ClstCfC = TSnap::GetClustCf(t->contactNet);
  //printf("Clustering coefficient: %.5f\n", ClstCfC);
  */

  // undirected stats for 7 possible networks
  // default tm = t union m
  //todo add 4 more cases for intersection
  // array has max size 10 in gLib unfortunately
  TStrV ClrV; // = TStrV::GetV("t", "m", "tm", "c", "tc", "mc", "tmc", "tmi", "tci", "mci");
  ClrV.Add("t"); ClrV.Add("m"); ClrV.Add("tm"); ClrV.Add("c"); ClrV.Add("tc");
  ClrV.Add("mc"); ClrV.Add("tmc"); ClrV.Add("tmi"); ClrV.Add("tci");
  ClrV.Add("mci"); ClrV.Add("tmci");
    //TStrV[] ClrV = TStrV::GetV("t", "m", "tm", "c", "tc", "mc", "tmc", "tmi", "tci", "mci", "tmci");

  for (int i=10; i>=0; i--) { // default 0 to 11
    PTaobaoShopNet x = new TTaobaoShopNet();

    // when creating our new network x, don't worry about adding both
    // everything will be undirected after

    // add trade nodes/edges
    if (i==0 || i==2 || i==4 || i==6) {
      for (TTaobaoShopNet::TEdgeI EI=s->BegEI(); EI<s->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if (!x->IsNode(SrcId)) {
          TTaobaoUsr newUsr = TTaobaoUsr( false, false);
          x->AddNode(SrcId, newUsr);
        }
        if (!x->IsNode(DstId)) {
          TTaobaoUsr newUsr = TTaobaoUsr( false, false);
          x->AddNode(DstId, newUsr);
        }
        if (!x->IsEdge(SrcId, DstId)) {
          TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
          x->AddEdge(SrcId, DstId, EdgeDat);
        }
      }
    }

    // add msg nodes/edges
    if (i==1 || i==2 || i==5 || i==6) {
      for (TTaobaoShopNet::TEdgeI EI=m->BegEI(); EI<m->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if (!x->IsNode(SrcId)) {
          TTaobaoUsr newUsr = TTaobaoUsr( false, false);
          x->AddNode(SrcId, newUsr);
        }
        if (!x->IsNode(DstId)) {
          TTaobaoUsr newUsr = TTaobaoUsr( false, false);
          x->AddNode(DstId, newUsr);
        }
        if (!x->IsEdge(SrcId, DstId)) {
          TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
          x->AddEdge(SrcId, DstId, EdgeDat);
        }
      }
    }

    // add contact nodes/edges
    if (i==3 || i==4 || i==5 || i==6) {
      for (TTaobaoShopNet::TEdgeI EI=c->BegEI(); EI<c->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if (!x->IsNode(SrcId)) {
          TTaobaoUsr newUsr = TTaobaoUsr( false, false);
          x->AddNode(SrcId, newUsr);
        }
        if (!x->IsNode(DstId)) {
          TTaobaoUsr newUsr = TTaobaoUsr( false, false);
          x->AddNode(DstId, newUsr);
        }
        if (!x->IsEdge(SrcId, DstId)) {
          TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
          x->AddEdge(SrcId, DstId, EdgeDat);
        }
      }
    }

    if (i == 7) { // tm intersection
      for (TTaobaoShopNet::TEdgeI EI=s->BegEI(); EI<s->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if (m->IsEdge(SrcId, DstId)|| m->IsEdge(DstId, SrcId)) {
          if (!x->IsNode(SrcId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(SrcId, newUsr);
          }
          if (!x->IsNode(DstId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(DstId, newUsr);
          }
          if (!x->IsEdge(SrcId, DstId)) {
            TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
            x->AddEdge(SrcId, DstId, EdgeDat);
          }
        }
      }

    }
    else if (i == 8) { // tc intersection

      for (TTaobaoShopNet::TEdgeI EI=s->BegEI(); EI<s->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if (c->IsEdge(SrcId, DstId)|| c->IsEdge(DstId, SrcId)) {
          if (!x->IsNode(SrcId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(SrcId, newUsr);
          }
          if (!x->IsNode(DstId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(DstId, newUsr);
          }
          if (!x->IsEdge(SrcId, DstId)) {
            TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
            x->AddEdge(SrcId, DstId, EdgeDat);
          }
        }
      }


    }
    else if (i == 9) { // mc intersection
      for (TTaobaoShopNet::TEdgeI EI=m->BegEI(); EI<m->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if (c->IsEdge(SrcId, DstId) || c->IsEdge(DstId, SrcId)) {
          if (!x->IsNode(SrcId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(SrcId, newUsr);
          }
          if (!x->IsNode(DstId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(DstId, newUsr);
          }
          if (!x->IsEdge(SrcId, DstId)) {
            TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
            x->AddEdge(SrcId, DstId, EdgeDat);
          }
        }
      }

    }
    else if (i == 10) { // tmc intersection
      for (TTaobaoShopNet::TEdgeI EI=s->BegEI(); EI<s->EndEI(); EI++) {
        int SrcId = EI.GetSrcNId();
        int DstId = EI.GetDstNId();
        if ((m->IsEdge(SrcId, DstId)|| m->IsEdge(DstId, SrcId))&&(c->IsEdge(SrcId, DstId)||c->IsEdge(DstId, SrcId))) {
          if (!x->IsNode(SrcId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(SrcId, newUsr);
          }
          if (!x->IsNode(DstId)) {
            TTaobaoUsr newUsr = TTaobaoUsr( false, false);
            x->AddNode(DstId, newUsr);
          }
          if (!x->IsEdge(SrcId, DstId)) {
            TTaobaoShopEdge EdgeDat = TTaobaoShopEdge();
            x->AddEdge(SrcId, DstId, EdgeDat);
          }
        }
      }

    }

    TStr infoStr = TStr("pedge2/infoStats_");
    TStr infoStr2 = infoStr + TStr(ClrV[i]);
    FILE *out = fopen(infoStr2.CStr(), "w");
    
    PUNGraph UGraphS = TSnap::ConvertGraph<PUNGraph>(x, false);
    //TSnap::PrintInfo(UGraphS, "Graph info", "graphInfoT.out");
    PUNGraph MxWccSU = TSnap::GetMxWcc(UGraphS);
    //TSnap::PrintInfo(MxWccSU, "Largest Weakly connected component", "wccInfoT.out");
    fprintf(out, "Original nodes: %d edges: %d\n", UGraphS->GetNodes(), UGraphS->GetEdges());
    fprintf(out, "Largest WCC nodes: %d edges: %d\n", MxWccSU->GetNodes(), MxWccSU->GetEdges());
    fprintf(out, "Ratio nodes: %f edges: %f\n", (float)MxWccSU->GetNodes()/(float)UGraphS->GetNodes(), (float)MxWccSU->GetEdges()/(float)UGraphS->GetEdges());
    
    double wccDiam = TSnap::GetAnfEffDiam(MxWccSU);
    fprintf(out, "Effective largest WCC diameter: %.3lf\n", wccDiam); 
    double ClstCfS = GetCCF(UGraphS);
    double ClstCfW = GetCCF(MxWccSU);
    fprintf(out, "Clustering coefficient original: %.8lf WCC: %.8lf\n", ClstCfS, ClstCfW);
    TFltPrV degCntV, outputV;
    TSnap::GetDegCnt(UGraphS, degCntV);

    float avgDeg = 2.0*(float)UGraphS->GetEdges()/(float)UGraphS->GetNodes();
    TStr degStr = TStr("pedge2/degDist_");
    TStr degStr2 = degStr + TStr(ClrV[i]);
    FILE *out2 = fopen(degStr2.CStr(), "w");
    int nodeCnt=0;
    int nodeCnt2=0;
    for (int j=0; j<degCntV.Len(); j++) {
      fprintf(out2, "%.0f %.0f\n", (float)degCntV[j].Val1, (float)degCntV[j].Val2);
      if ((float)degCntV[j].Val1 > avgDeg)
        nodeCnt += (int)degCntV[j].Val2;
      if ((float)degCntV[j].Val1 > 2.0*avgDeg)
        nodeCnt2 += (int)degCntV[j].Val2;
    }
    fclose(out2);
    fprintf(out, "Avg Deg: %f #Nodes w Deg > Avg: %d 2*Avg: %d\n", avgDeg, nodeCnt, nodeCnt2);

    // these are outside of the folder
    TStr binDegStr = TStr("binDegDist_");
    TStr binDegStr2 = binDegStr + TStr(ClrV[i]);
    binning(degCntV, binDegStr2.CStr(), 1.0, 2.0, true, "Binned Degree Distribution", "Node Degree", "Node Cnt", outputV);
    
    TSnap::PlotHops(UGraphS, ClrV[i].CStr(), ClrV[i].CStr()); 
    
    TIntPrV WccSzCnt;
    TFltPrV WccSzCntF, outputV2;
    TSnap::GetWccSzCnt(UGraphS, WccSzCnt);
    
    TStr wccStr = TStr("pedge2/wcc_");
    TStr wccStr2 = wccStr + TStr(ClrV[i]);
    FILE *out3 = fopen(wccStr2.CStr(), "w");
    for (int j=0; j<WccSzCnt.Len(); j++) {
      fprintf(out3, "%d %d\n", (int)WccSzCnt[j].Val1, (int)WccSzCnt[j].Val2);
      WccSzCntF.Add(TFltPr((float)WccSzCnt[j].Val1, (float)WccSzCnt[j].Val2));
    }
    fclose(out3);

    fclose(out);
    x->Clr();    
  }

  // need to graph this later with initial bin set at 2
  //binning(WccSzCntF, "binned_wcc_dist", 1.0, 2.0, true, "Binned Component Size Distribution", "Component Size", "Component Cnt", outputV2);

  /*
  PUNGraph UGraphM = TSnap::ConvertGraph<PUNGraph>(m, false);
  TSnap::PrintInfo(UGraphM, "Graph info");
  PUNGraph MxWccMU = TSnap::GetMxWcc(UGraphM);
  TSnap::PrintInfo(MxWccMU, "Largest Weakly connected component");
  double ClstCfM = GetCCF(UGraphM);
  printf("Clustering coefficient: %.8lf\n", ClstCfM);


  PUNGraph UGraphC = TSnap::ConvertGraph<PUNGraph>(c, false);
  TSnap::PrintInfo(UGraphC, "Graph info");
  PUNGraph MxWccCU = TSnap::GetMxWcc(UGraphC);
  TSnap::PrintInfo(MxWccCU, "Largest Weakly connected component");
  double ClstCfC = GetCCF(UGraphC);
  printf("Clustering coefficient: %.8lf\n", ClstCfC);
  */
}

void degCompare(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  PUNGraph uS = TSnap::ConvertGraph<PUNGraph>(s, false);
  PUNGraph uM = TSnap::ConvertGraph<PUNGraph>(m, false);
  PUNGraph uC = TSnap::ConvertGraph<PUNGraph>(c, false);
  
  //TFltTrV degs; // trade msg contact

  FILE *out = fopen("nodeDeg.out", "w");
  int nodeCnt = 0;
  for (TUNGraph::TNodeI NI=uS->BegNI(); NI<uS->EndNI(); NI++) {
    int ID = NI.GetId();
    int sDeg = NI.GetDeg();
    int mDeg = 0;
    int cDeg = 0;
    
    if (uM->IsNode(ID)) {
      TUNGraph::TNodeI NJ = uM->GetNI(ID);
      mDeg = NJ.GetDeg();
    }
    
    if (uC->IsNode(ID)) {
      TUNGraph::TNodeI NJ = uC->GetNI(ID);
      cDeg = NJ.GetDeg();
    }
    
    fprintf(out, "%d %d %d\n", sDeg, mDeg, cDeg);
    nodeCnt++;
  }
  
  fclose(out);
  printf("%d\n", nodeCnt);
}

void plotDeg() {
  FILE *in = fopen("nodeDeg.out", "r");
  int tradeDeg, msgDeg, conDeg;
  TFltTrV degs;
  TFltPrV tradeMsgDeg, tradeConDeg;
  TFltPrV msgTradeDeg, msgConDeg;
  TFltPrV conTradeDeg, conMsgDeg;

  while (fscanf(in, "%d %d %d", &tradeDeg, &msgDeg, &conDeg) == 3) {
    degs.Add(TFltTr((float)tradeDeg, (float)msgDeg, (float)conDeg));
  }

  for (int i=0; i<degs.Len(); i++) {
    tradeMsgDeg.Add(TFltPr((float)degs[i].Val1,(float)degs[i].Val2));
    tradeConDeg.Add(TFltPr((float)degs[i].Val1,(float)degs[i].Val3));
    msgTradeDeg.Add(TFltPr((float)degs[i].Val2,(float)degs[i].Val1));
    msgConDeg.Add(TFltPr((float)degs[i].Val2,(float)degs[i].Val3));
    conTradeDeg.Add(TFltPr((float)degs[i].Val3,(float)degs[i].Val1));
    conMsgDeg.Add(TFltPr((float)degs[i].Val3,(float)degs[i].Val2));
  }

  TFltPrV tradeMsgDegOut, tradeConDegOut;
  TFltPrV msgTradeDegOut, msgConDegOut;
  TFltPrV conTradeDegOut, conMsgDegOut;

  binning(tradeMsgDeg, "binned_tradeMsgDeg", 1.0, 2.0, true,"Binned Trade Degree vs Message Degree", "Trade Degree", "Message Degree", tradeMsgDegOut);
  binning(tradeConDeg, "binned_tradeConDeg", 1.0, 2.0, true,"Binned Trade Degree vs Contact Degree", "Trade Degree", "Contact Degree", tradeConDegOut);
  binning(msgTradeDeg, "binned_msgTradeDeg", 1.0, 2.0, true,"Binned Message Degree vs Trade Degree", "Message Degree", "Trade Degree", msgTradeDegOut);
  binning(msgConDeg, "binned_msgConDeg", 1.0, 2.0, true,"Binned Message Degree vs Contact Degree", "Message Degree", "Contact Degree", msgConDegOut);
  binning(conTradeDeg, "binned_conTradeDeg", 1.0, 2.0, true,"Binned Contact Degree vs Trade Degree", "Contact Degree", "Trade Degree", conTradeDegOut);
  binning(conMsgDeg, "binned_conMsgDeg", 1.0, 2.0, true,"Binned Contact Degree vs Message Degree", "Contact Degree", "Message Degree", conMsgDegOut);

  fclose(in);
}

void plotMT() {
  TFltPrV data;
  
  int day;
  float prob;

  // OCT 10: +/- 5, log the y axis

  FILE *in = fopen("msgRelativeTrade.out", "r");
  for (int i=0; i<21; i++) {
    fscanf(in, "%d %f", &day, &prob);
    if (day>=-6 && day<=6)
      data.Add(TFltPr((float)day, log(prob)));
  }
  fclose(in);

  TGnuPlot tPGnuPlot("msgRelativeTrade2", "", false);
  tPGnuPlot.AddCmd("set arrow from 0,-2.5 to 0,2.5 nohead lt 2 lw 3");
  tPGnuPlot.AddPlot(data, gpwLinesPoints, "", "lt 1 pt 7 ps 3");
  //tPGnuPlot.AddFunc("0", gpwLines, "", "lt 2 lw 3");
  tPGnuPlot.SetXYLabel("Days Relative to Trade", "Log Messages");
  tPGnuPlot.SetScale(gpsAuto);
  //tPGnuPlot.SavePng();
  tPGnuPlot.SaveEps("msgRelativeTrade2.eps", 30, "");
}

// avg num msgs relative to a trade
void msgTrade(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  int winSize = 10;
  int daySec = 24*60*60;
  int window = winSize*daySec;

  TSecTm intervalBegin = TSecTm::GetDtTm(2009, 9, 1);
  TSecTm intervalEnd = TSecTm::GetDtTm(2009, 10, 28);
  int beginSec = intervalBegin.GetAbsSecs();
  int endSec = intervalEnd.GetAbsSecs();

  int afterTrade[winSize+1]; // 0 to +5
  int beforeTrade[winSize+1]; // -1 to -5
  int cnt = 0;

  for (int i=0; i<=winSize; i++) {
    afterTrade[i] = 0;
    beforeTrade[i] = 0;
  }

  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    TTaobaoShopEdge shopEdge = s->GetEDat(SrcId, DstId);
    TTaobaoTX tx = shopEdge.GetTX(0);
    float salePrice = (float) tx.SalePrice;
    int txTime =  shopEdge.GetTime(0).GetAbsSecs();
    // ignore border trades
    if ((txTime-beginSec)/daySec<=winSize || (endSec-txTime)/daySec<=winSize)
      continue;

    cnt++; // cand trade
    if (m->IsEdge(SrcId, DstId)) {
      TTaobaoShopEdge e = m->GetEDat(SrcId, DstId);
      for (int i=0; i<e.CntTX(); i++) {
        int msgTime = e.GetTime(i).GetAbsSecs();
        if (msgTime>txTime+window)
          break;
        if (msgTime>=txTime) {
          int dayDif = (msgTime-txTime)/daySec;
          afterTrade[dayDif] += (int)e.GetStrength(i);
        }
        else if (msgTime>=txTime-window) {
          int dayDif = (txTime-msgTime)/daySec;
          beforeTrade[dayDif] += (int)e.GetStrength(i);
        }
      }
    }
  }

  FILE *out = fopen("msgRelativeTrade.out", "w");
  for (int i=10; i>0; i--)
    fprintf(out, "%d %f\n", i*-1, (float)beforeTrade[i]/(float)cnt);
  for (int i=0; i<=10; i++)
    fprintf(out, "%d %f\n", i, (float)afterTrade[i]/(float)cnt);
  fclose(out);

}

// only consider people that msg buyer day of the trade
void msgPrice(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  int windowSize = 0;
  int daySec = 24*60*60;
  int window = windowSize*daySec;
 
  FILE *out = fopen("mP.out", "w");
  
  TFltPrV mP;

  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    TInt SrcId = EI.GetSrcNId();
    TInt DstId = EI.GetDstNId();
    if (m->IsEdge(SrcId, DstId)) {
      TTaobaoShopEdge e = EI();
      TTaobaoTX tx = e.GetTX(0);
      float salePrice = (float) tx.SalePrice;
      int txTime =  e.GetTime(0).GetAbsSecs();
      
      int msgStrength = 0;
      TTaobaoShopEdge msgEdge = m->GetEDat(SrcId, DstId);
      for (int i=0; i<msgEdge.CntTX(); i++) {
        int msgTime = msgEdge.GetTime(i).GetAbsSecs();
        if (msgTime > txTime+window)
          break;
        if (msgTime >= txTime-window) {
          msgStrength += (int) msgEdge.GetStrength(i);
        }
      }

      if (msgStrength > 0) {
        mP.Add(TFltPr(salePrice, (float)msgStrength));
        fprintf(out, "%f %d\n", salePrice, msgStrength);
      }
    }
  }

  fclose(out);

  TFltPrV outputV;
  binning(mP, "binned_msgPrice", 1.0, 2.0, false, "Binned Price vs Message Strength", "Price", "Message Strength", outputV);

}

// for plotting msg v strength, also msg vs response time
void plotMsgPrice() {
  TFltPrV priceTime, outputV, priceStrength;
  float time, price, strength;
 
  FILE *in = fopen("mP.out", "r");
  while (fscanf(in, "%f %f", &price, &strength) == 2) {
    //if (price < 500.0 && time<=10.0)
      priceStrength.Add(TFltPr((float)price, (float)strength));
  }
  fclose(in);

  //binning(priceStrength, "binned_msgPrice2", 1.0, 2.0, true, "Price vs Message Strength", "Price", "Message Strength", outputV);
  // OCT10 new
  binning(priceStrength, "binned_msgPrice3", 1.0, 2.0, true, "", "Price", "Messages on Trade Day", outputV);
 
  // try only looking at things within a reasonable time frame
  /*
  FILE *in = fopen("timePrice.out", "r");
  while (fscanf(in, "%f %f", &time, &price) == 2) {
    if (price < 500.0 && time<=10.0)
      priceTime.Add(TFltPr((float)price, (float)time));
  }
  fclose(in);
    
  binning(priceTime, "binned_priceTimeTrim3", 1.0, 2.0, true, "Price vs Response Time", "Price", "Response Time", outputV);
  */
}

/* 
find repeat buyers
hope we have prod information
find out how repeats affect prices
*/

void repeatBuyer(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;

  TStrV fields;
  TStrH prodBuyer;
  TStrH prodSeller;
  //TIntPrH buyerSeller; // store counts of buyer seller purchases
  char line[2024];

  int repeatCnt[10000];
  for (int i=0; i<10000; i++)
    repeatCnt[i] = 0;

  FILE *out2 = fopen("repeat2.out", "w");

  int largest = 0;
  for (TTaobaoShopNet::TEdgeI EI = s->BegEI(); EI < s->EndEI(); EI++) {
    int SrcID = EI.GetSrcNId();
    int DstID = EI.GetDstNId();
    TTaobaoShopEdge se = EI();
    int numTX = se.CntTX();
    if (numTX > largest)
      largest = numTX;
    repeatCnt[numTX]++;

    if (numTX > 1) {
      fprintf(out2, "%d %d %d\n", numTX, SrcID, DstID);
      for (int i=0; i<numTX; i++) {
        TTaobaoTX tx = se.GetTX(i);
        fprintf(out2, "%f %s\n", (float)tx.SalePrice, tx.Product.CStr());
      }
    }
  }

  fclose(out2);

  //FILE *out = fopen("repeat.out", "w");
  //for (int i=0; i<=largest; i++)
  //  fprintf(out, "%d %d\n", i, repeatCnt[i]);
  //fclose(out);

}

// Does most talkative person get the cheapest price?
// If true, compute avg rank correlation coefficient among all clusters
void clusterTest(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;
  
  TStrV fields;
  TStrH prodBuyer;  
  TStrH prodSeller;
  char line[2024];

  FILE *in = fopen("/lfs/1/tmp/mengqiu/fwd.1000000.txt", "r");
  while (!feof(in)) {
    memset(line, 0, 2024);
    fgets(line, 2024, in);
    if (strlen(line) == 0)
      break;

    TStr(line).SplitOnAllCh('\t', fields, false);
  
    TInt BuyerID = TInt(atoi(fields[0].GetTrunc().CStr())); // buyer id                                                                                    
    TInt SellerID = TInt(atoi(fields[1].GetTrunc().CStr())); // seller id                                                                                  
    TStr ProductID = fields[3].GetTrunc();
    TFlt ProductPrice = TFlt(atof(fields[5].GetTrunc().CStr()));
  
    if (prodSeller.IsKey(ProductID)) {
    }
    else {
      prodSeller.AddDat(ProductID, SellerID);      
      if (prodBuyer.IsKey(ProductID)) 
        printf("no way\n");
      else 
        prodBuyer.AddDat(ProductID, BuyerID);
    }
    
  }
  fclose(in);

  FILE *in2 = fopen("/lfs/1/tmp/mengqiu/catStephen.txt", "r");
  
  int maxLines = 1023; // IGNORE all clusters with more than this
  char lines[maxLines+1][2048]; // max clust size is ~ 36000 
  TStr prevCluster = "";
  int clustSize=0;
  int lineNum=0;

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
      clustSize=1;

      if (lineNum>1 && lineNum!=maxLines) { // added latter check
      
        for (int i=0; i<lineNum-1; i++) {
         
          TStr(lines[i]).SplitOnAllCh('\t', fields, false);
          TFlt ProductPrice = TFlt(atof(fields[1].GetTrunc().CStr()));
          TFlt rating = TFlt(atof(fields[7].GetTrunc().CStr()));

          TInt buyerID = prodBuyer.GetDat(fields[0]);
          TInt sellerID = prodSeller.GetDat(fields[0]);

          // insert buyer, seller into hashes
        } // close for int i
         
        // consider cases with 2-10 sellers. eg throw out 10-15% of the data 
        /*
        if (clustBuyer.Len()>0 && (clustSeller.Len()>1 &&
                                   clustSeller.Len()<11)) {

        }
        */
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
    
  fclose(in2);
}

int qSZ = 100000;
int queue[100000]; // for BFS

// generate features for prediction/other stuff
void updatePred(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet;
  PTaobaoShopNet c = t->contactNet;
  
  srand ( time(NULL) );
  
  // NOTE fwd file given a prodID, there will be multiple buyers, just use
  // first one, dont think it matters whcih one we assume 
  
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
    
  FILE *infPR = fopen("global_pagerank/fwd.pagerank", "r");
  while(!feof(infPR)) {
    fscanf(infPR, "%d %le", &nID, &pr);
    pr = pr * 1000.0;
    if (!fwdPR.IsKey(nID))
      fwdPR.AddDat(nID, (float)pr);
  }
  fclose(infPR);

  FILE *inmPR = fopen("global_pagerank/msg.pagerank", "r");
  while(!feof(inmPR)) {
    fscanf(inmPR, "%d %le", &nID, &pr);
    pr = pr * 1000.0;
    if (!msgPR.IsKey(nID))
      msgPR.AddDat(nID, (float)pr);
  }
  fclose(inmPR);

  FILE *incPR = fopen("global_pagerank/contact.pagerank", "r");
  while(!feof(incPR)) {
    fscanf(incPR, "%d %le", &nID, &pr);
    pr = pr * 1000.0;
    if (!conPR.IsKey(nID))
      conPR.AddDat(nID, (float)pr);
  }
  fclose(incPR);
  
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

  // tehse two need to be updated, ignore for now

  FILE *inBRP = fopen("buyerRelativePrice.out", "r");
  while (!feof(inBRP)) {
    fscanf(inBRP, "%d %f", &lA, &lC);
    loadedBRP.AddDat(lA, lC);
  }
  fclose(inBRP);

  FILE *inSRP = fopen("sellerRelativePrice.out", "r");
  while (!feof(inSRP)) {
    fscanf(inSRP, "%d %f", &lA, &lC);
    loadedSRP.AddDat(lA, lC);
  }
  fclose(inSRP);

  FILE *trainFiles[100];
  FILE *testFiles[100];
  int trainSize[100]; // number of qid's per cat file
  int testSize[100];
  // may have to open crapload of files simultaneously
  
  // new: load category-catnum mapping
  // use this to fopen all cat files simultanously and separate all examples
  char line[2024];
  TStrH catMap;
  TStrV catFields;
  FILE *inCat = fopen("catProd.out", "r");
  FILE *outCat =fopen("catOut.out", "w");
  int catNum=0;
  while(!feof(inCat)) {
    float someNum;
    memset(line, 0 ,2024);
    fgets(line, 2024, inCat);
    if (strlen(line)==0)
      break;
    line[strlen(line)-1] = 0; // remove newline
 
    // careful: special case where category name is ""
    // should be handled correctly
    TStr(line).SplitOnAllCh(' ', catFields, false);
    
    catMap.AddDat(catFields[0], catNum);
    fprintf(outCat, "%s %d\n", catFields[0].CStr(), catNum);
    // NEW: move training/testing files to lfs
    TStr newTrainFile = TStr::Fmt("/lfs/1/tmp/mengqiu/svmRank/cats/train%d.txt", catNum);
    trainFiles[catNum] = fopen(newTrainFile.CStr(), "w");
    TStr newTestFile = TStr::Fmt("/lfs/1/tmp/mengqiu/svmRank/cats/test%d.txt", catNum);
    testFiles[catNum] = fopen(newTestFile.CStr(), "w");
    //printf("%s %d\t", catTemp, catNum);
    catNum++;
  }
  fclose(inCat);
  fclose(outCat);
  //return; // debug 
  
  for (int i=0; i<catNum; i++) {
    trainSize[i] = 0;
    testSize[i] = 0;
  }

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
  }
  
  int conPos=0, conNeg=0;
  int msgPos=0, msgNeg=0;
  
  //FILE *outs = fopen("clustSize.out", "w"); 
 
  //FILE *trainMap = fopen("svmRank/trainMap.out", "w");
  //FILE *testMap = fopen("svmRank/testMap.out", "w");
 
  // TODO: write script in prediction.cpp to create different feature sets from this
  // formerly 32 FTR, testing 50FTR. will increase after more graph features 
  // start writing to lfs. local may be too slow
  FILE *trainRank = fopen("/lfs/1/tmp/mengqiu/svmRank/trainRank70FTR.outRERUN", "w");
  FILE *testRank = fopen("/lfs/1/tmp/mengqiu/svmRank/testRank70FTR.outRERUN", "w");
  // 50 ftr fails, trying smaller sets
  // CHANGE FOR TESTING
  FILE *trainRankSmall = fopen("/lfs/1/tmp/mengqiu/svmRank/trainRankSmall.outRERUN", "w");
  FILE *testRankSmall = fopen("/lfs/1/tmp/mengqiu/svmRank/testRankSmall.outRERUN", "w");

  FILE *rawTrain = fopen("/lfs/1/tmp/mengqiu/svmRank/rawTrain.outRERUN", "w");
  FILE *rawTest = fopen("/lfs/1/tmp/mengqiu/svmRank/rawTest.outRERUN", "w");
  
  // testing feature subsets 
  // CHANGE FOR TESTING
  FILE *trainRankA = fopen("svmRank/tests/trainRankA.out", "w");
  FILE *testRankA = fopen("svmRank/tests/testRankA.out", "w");
  FILE *trainRankB = fopen("svmRank/tests/trainRankB.out", "w");
  FILE *testRankB = fopen("svmRank/tests/testRankB.out", "w");
  FILE *trainRankC = fopen("svmRank/tests/trainRankC.out", "w");
  FILE *testRankC = fopen("svmRank/tests/testRankC.out", "w");
  FILE *trainRankD = fopen("svmRank/tests/trainRankD.out", "w");
  FILE *testRankD = fopen("svmRank/tests/testRankD.out", "w");
  FILE *trainRankE = fopen("svmRank/tests/trainRankE.out", "w");
  FILE *testRankE = fopen("svmRank/tests/testRankE.out", "w");
  FILE *trainRankF = fopen("svmRank/tests/trainRankF.out", "w");
  FILE *testRankF = fopen("svmRank/tests/testRankF.out", "w");
  FILE *trainRankG = fopen("svmRank/tests/trainRankG.out", "w");
  FILE *testRankG = fopen("svmRank/tests/testRankG.out", "w");

  // new: output features for all positive examples by category
  //FILE *posCat = fopen("svmRank/posCat.out", "w");
  // output positive feature values for correlation matrix, PCA analysis
  // only positive examples for R
  //FILE *posRBig = fopen("svmRank/posRBig.out", "w"); // all features
  //FILE *posRSmall = fopen("svmRank/posRSmall.out", "w"); // basic set
  // weka format, class attribute must be last
  FILE *wekaBig = fopen("/lfs/1/tmp/mengqiu/svmRank/wekaBig.outRERUN", "w");
  FILE *wekaSmall = fopen("/lfs/1/tmp/mengqiu/svmRank/wekaSmall.outRERUN", "w");

  // use csv files for both R and weka
  /*
  fprintf(posRBig,
  "priceDif,ratingDif,isContact,fwdMsgVol,bwdMsgVol,tradeVol,mutualCon,mutualMsg,buyerDeg,buyerVol,sellerDeg,sellerVol,invPriceRank,invRatingRank,invMsgRank,weekMsgVol,weekTradeVol,bNumClusters,sNumClusters,bCategories,sCategories,bRelativePrice,sRelativePrice,edgeTime1,edgeTime2,edgeTime3,dayST1,dayST2,dayST3,daySM1,daySM2,daySM3\n");
  fprintf(posRSmall,"isContact,msgVol,tradeVol,mutualCon,mutualMsg,sellerDeg,sellerVol,invPriceRank,invRatingRank,invMsgRank,dayST2,daySM2,priceDif,ratingDif\n");
  */
  /*
  fprintf(wekaBig,
  "priceDif,ratingDif,isContact,fwdMsgVol,bwdMsgVol,tradeVol,mutualCon,mutualMsg,buyerDeg,buyerVol,sellerDeg,sellerVol,invPriceRank,invRatingRank,invMsgRank,weekMsgVol,weekTradeVol,bNumClusters,sNumClusters,bCategories,sCategories,bRelativePrice,sRelativePrice,edgeTime1,edgeTime2,edgeTime3,dayST1,dayST2,dayST3,daySM1,daySM2,daySM3,CLASS\n");
  */

  fprintf(wekaBig, "priceDif,ratingDif,isContact,fwdMsgVol,bwdMsgVol,tradeVol,mutualCon,mutualMsg,buyerDeg,buyerVol,sellerDeg,sellerVol,invPriceRank,invRatingRank,invMsgRank,weekMsgVol,weekTradeVol,bNumClusters,sNumClusters,bCategories,sCategories,bRelativePrice,sRelativePrice,edgeTime1,edgeTime2,edgeTime3,dayST1,dayST2,dayST3,daySM1,daySM2,daySM3,fracPriceRank,fracRatingRank,fracMsgRank,propPriceDif,propRatingDif,sDealRecord,sProtection,sPic,invCreditRank,fracCreditRank,creditDif,logCredit,sDesc,sServ,sSpeed,descDif,servDif,speedDif,zConDeg,zMsgDeg,zConCCF,zMsgCCF,sConDeg,sMsgDeg,sConCCF,sMsgCCF,mutualConProp,mutualConDensity,mutualMsgProp,mutualMsgDensity,zFwdPR,zMsgPR,zConPR,sFwdPR,sMsgPR,sConPR,cShortPath,mShortPath,CLASS\n");

  fprintf(wekaSmall,"isContact,msgVol,tradeVol,mutualCon,mutualMsg,sellerDeg,sellerVol,invPriceRank,invRatingRank,invMsgRank,dayST2,daySM2,priceDif,ratingDif,CLASS\n");

  FILE *subgraphs = fopen("subgraph.out", "w");
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
        
          // just moved this check up
          /*
          TFlt transPrice = prodPrice.GetDat(fields[0]);
          TFlt transShip = prodShip.GetDat(fields[0]);
          if (ProductPrice > transPrice*2.0 || transPrice>ProductPrice*2.0) {
            skipPrice++; // skip this line
            continue;
          }
          */
          // price sanity check, ignore if off too much. mapping/ambiguity problem 
          
          if (!sellerRating.IsKey(sellerID))
            sellerRating.AddDat(sellerID, rating);

          // new vars
          if (!sellerDR.IsKey(sellerID))
            sellerDR.AddDat(sellerID, dealRecord);
          if (!sellerProtection.IsKey(sellerID))
            sellerProtection.AddDat(sellerID, protection);
          if (!sellerPic.IsKey(sellerID))
            sellerPic.AddDat(sellerID, hasPic);
          if (!sellerCredit.IsKey(sellerID))
            sellerCredit.AddDat(sellerID, credit);
          if (!sellerDesc.IsKey(sellerID))
            sellerDesc.AddDat(sellerID, desc);
          if (!sellerServ.IsKey(sellerID))
            sellerServ.AddDat(sellerID, serv);
          if (!sellerSpeed.IsKey(sellerID))
            sellerSpeed.AddDat(sellerID, speed);

           // compute median  product price for each seller
          
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
        int catIndex;
        if (catMap.IsKey(category))
          catIndex = catMap.GetDat(category);
        else {
          printf("missing category START%sEND\n", category.CStr());
          return;
        }
                
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
          
          // new vars
          sellerCredit.SortByDat(false);
          float medCredit;
          if (sellerCredit.Len()%2==1)
            medCredit = (float)sellerCredit[sellerCredit.Len()/2];
          else
            medCredit = (float)
          (sellerCredit[sellerCredit.Len()/2]+sellerCredit[sellerCredit.Len()/2-1])/2.0;
          
          sellerRating.SortByDat(false); // descending rating                                                                                              

          sellerDesc.SortByDat(false);
          float medDesc;
          if (sellerDesc.Len()%2==1)
            medDesc = sellerDesc[sellerDesc.Len()/2];
          else
            medDesc =
          (sellerDesc[sellerDesc.Len()/2]+sellerDesc[sellerDesc.Len()/2-1])/2.0;
          if (medDesc < 0.0)
            medDesc = 0.0;

          sellerServ.SortByDat(false);
          float medServ;
          if (sellerServ.Len()%2==1)
            medServ = sellerServ[sellerServ.Len()/2];
          else  
            medServ =
              (sellerServ[sellerServ.Len()/2]+sellerServ[sellerServ.Len()/2-1])/2.0;
          if (medServ < 0.0)
            medServ = 0.0;

          sellerSpeed.SortByDat(false);
          float medSpeed;
          if (sellerSpeed.Len()%2==1)
            medSpeed = sellerSpeed[sellerSpeed.Len()/2];
          else
            medSpeed =
              (sellerSpeed[sellerSpeed.Len()/2]+sellerSpeed[sellerSpeed.Len()/2-1])/2.0;
          if (medSpeed < 0.0)
            medSpeed = 0.0;

          // indicates a new cluster 
          fprintf(subgraphs, "cluster %d buyers %d sellers %d\n", cnt, clustBuyer.Len(), clustSeller.Len());
          for (int i=0; i<clustBuyer.Len(); i++)
            fprintf(subgraphs, "%d ", (int)clustBuyer.GetKey(i));
          fprintf(subgraphs, "\n");
          for (int i=0; i<clustSeller.Len(); i++)
            fprintf(subgraphs, "%d ", (int)clustSeller.GetKey(i));
          fprintf(subgraphs, "\n");
          // may want to print true mappings 

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
            
            fprintf(subgraphs, "refNode %d\n", bID);
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
              
              if (fwdPR.IsKey(zID))
                zFwdPR = fwdPR.GetDat(zID);
              if (msgPR.IsKey(zID))
                zMsgPR = msgPR.GetDat(zID);
              if (conPR.IsKey(zID))
                zConPR = conPR.GetDat(zID);

              // compute buyer volume and degree prior to trade time
              float buyerVol = 0.0;
              float buyerDeg = 0.0;
              if (s->IsNode(zID)) { 
                TTaobaoShopNet::TNodeI BI = s->GetNI(zID);
                for (int k=0; k<BI.GetOutDeg(); k++) {
                  int toNode = BI.GetOutNId(k);
                  TTaobaoShopEdge& shopEdge = s->GetEDat(zID, toNode);
                  int fnd=0;
                  for (int l=0; l<shopEdge.CntTX(); l++) {
                    int shopTime = shopEdge.GetTime(l).GetAbsSecs();
                    if (shopTime < refTime) {
                      buyerVol += 1.0;
                      fnd = 1;
                    }
                    else
                      break;
                  }
                  if (fnd == 1)
                    buyerDeg += 1.0;
                }
                buyerVol = log(1+buyerVol);
                buyerDeg = log(1+buyerDeg);
              }

              if (c->IsNode(zID)) {
                TTaobaoShopNet::TNodeI BI = c->GetNI(zID);
                for (int k=0; k<BI.GetOutDeg(); k++) {
                  int toNode = BI.GetOutNId(k);
                  TTaobaoShopEdge& edge = c->GetEDat(zID, toNode);
                  if (edge.GetTime(0).GetAbsSecs() < refTime) {
                    if (!zConNeighbors.IsKey(toNode))
                      zConNeighbors.AddDat(toNode, 1);
                  }
                }
                
                zConDeg = log(1.0+(float)zConNeighbors.Len());
              
                if (zConNeighbors.Len()>=2) {
                  float connect = 0.0;
                  for (int k=0; k<zConNeighbors.Len()-1; k++) {
                    int node1 = zConNeighbors.GetKey(k);
                    for (int l=k+1; l<zConNeighbors.Len(); l++) {
                      int node2 = zConNeighbors.GetKey(l);
                      if (c->IsEdge(node1, node2)) {
                        TTaobaoShopEdge& edge = c->GetEDat(node1, node2);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                    }
                  }

                  float mostEdges = (float) (zConNeighbors.Len() * (zConNeighbors.Len()-1))/2;
                  zConCCF = connect / mostEdges;
                }

              }
              
              if (m->IsNode(zID)) {
                TTaobaoShopNet::TNodeI BI = m->GetNI(zID);
                for (int k=0; k<BI.GetOutDeg(); k++) {
                  int toNode = BI.GetOutNId(k);
                  TTaobaoShopEdge& edge = m->GetEDat(zID, toNode);
                  if (edge.GetTime(0).GetAbsSecs() < refTime) {
                    if (!zMsgNeighbors.IsKey(toNode))
                      zMsgNeighbors.AddDat(toNode, 1);
                  }
                }
                
                for (int k=0; k<BI.GetInDeg(); k++) {
                  int fromNode = BI.GetInNId(k);
                  TTaobaoShopEdge& edge = m->GetEDat(fromNode, zID);
                  if (edge.GetTime(0).GetAbsSecs() < refTime) {
                    if (!zMsgNeighbors.IsKey(fromNode))
                      zMsgNeighbors.AddDat(fromNode, 1);
                  }
                }

                zMsgDeg = log(1.0+(float)zMsgNeighbors.Len());
              
                if (zMsgNeighbors.Len()>=2) {
                  float connect = 0.0;
                  for (int k=0; k<zMsgNeighbors.Len()-1; k++) {
                    int node1 = zMsgNeighbors.GetKey(k);
                    for (int l=k+1; l<zMsgNeighbors.Len(); l++) {
                      int node2 = zMsgNeighbors.GetKey(l);
                      if (m->IsEdge(node1, node2)) {
                        TTaobaoShopEdge& edge = m->GetEDat(node1, node2);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                      if (m->IsEdge(node2, node1)) {
                        TTaobaoShopEdge& edge = m->GetEDat(node2, node1);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                    }
                  }

                  float mostEdges = (float) (zMsgNeighbors.Len() * (zMsgNeighbors.Len()-1));
                  zMsgCCF = connect / mostEdges;
                }

              }

              // NEW: do a limited diameter BFS from z
              // compute shortest path, use max dist 3
              int maxDist = 3;
              TIntH cDist;
              if (c->IsNode(zID)) {
                int qHead = 0;
                int qTail = 0;
                int sqCnt=0;
                queue[0] = zID;
                cDist.AddDat(zID, 0);
                while (qHead <= qTail) {
                  int curID = queue[qHead];
                  int curDist = cDist.GetDat(curID);
                  if (curDist > maxDist)
                    break;
                  qHead++;                 
                  TTaobaoShopNet::TNodeI NI = c->GetNI(curID);
                  for (int k=0; k<NI.GetOutDeg(); k++) {
                    int toNode = NI.GetOutNId(k);
                    if (!cDist.IsKey(toNode)) {
                      TTaobaoShopEdge& edge = c->GetEDat(curID, toNode);
                      if (edge.GetTime(0).GetAbsSecs() < refTime) {
                        // add to queue
                        if (qTail<qSZ-1) {
                          qTail++;
                          queue[qTail] = toNode;
                          cDist.AddDat(toNode, curDist+1);
                          if (clustSeller.IsKey(toNode)) {
                            sqCnt++;
                            if (sqCnt == clustSeller.Len())
                              break; // found all sellers
                          }
                        }
                      }
                    }
                  }
                }
              }

              TIntH mDist;
              if (m->IsNode(zID)) {
                int qHead = 0;
                int qTail = 0;
                int sqCnt = 0;
                queue[0] = zID;
                mDist.AddDat(zID, 0);
                while (qHead <= qTail) {
                  int curID = queue[qHead];
                  int curDist = mDist.GetDat(curID);
                  if (curDist > maxDist)
                    break;
                  qHead++;
                  TTaobaoShopNet::TNodeI NI = m->GetNI(curID);
                  for (int k=0; k<NI.GetOutDeg(); k++) {
                    int toNode = NI.GetOutNId(k);
                    if (!mDist.IsKey(toNode)) {
                      TTaobaoShopEdge& edge = m->GetEDat(curID, toNode);
                      if (edge.GetTime(0).GetAbsSecs() < refTime) {
                        if (qTail < qSZ-1) {
                          // add to queue
                          qTail++;
                          queue[qTail] = toNode;
                          mDist.AddDat(toNode, curDist+1);
                          if (clustSeller.IsKey(toNode)) {
                            sqCnt++;
                            if (sqCnt == clustSeller.Len())
                              break; // found all sellers
                          }
                        }
                      }
                    }
                  }
                  for (int k=0; k<NI.GetInDeg(); k++) {
                    int fromNode = NI.GetInNId(k);
                    if (!mDist.IsKey(fromNode)) {
                      TTaobaoShopEdge& edge = m->GetEDat(fromNode, curID);
                      if (edge.GetTime(0).GetAbsSecs() < refTime) {
                        if (qTail < qSZ-1) {
                          // add to queue
                          qTail++;
                          queue[qTail] = fromNode;
                          mDist.AddDat(fromNode, curDist+1);
                          if (clustSeller.IsKey(fromNode)) {
                            sqCnt++;
                            if (sqCnt == clustSeller.Len())
                              break; // found all sellers
                          }
                        }
                      }
                    }
                  }
                }
              }


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
              
              float invPriceRank = 0.0;
              float invRatingRank = 0.0;
              float invMsgRank = 0.0;
              // NEw: fractional rank
              float fracPriceRank = 1.0;
              float fracRatingRank = 1.0;
              float fracMsgRank = 1.0; 
              
              for (int l=0; l<sellerMed.Len(); l++)
                if (sellerMed.GetKey(l) == sID) {
                  invPriceRank = 1.0/(1.0+(float)l);
                  fracPriceRank = (1.0+(float)l)/(float)clustSeller.Len();
                  break;
                }

              for (int l=0; l<sellerRating.Len(); l++)
                if (sellerRating.GetKey(l) == sID) {
                  invRatingRank = 1.0/(1.0+(float)l);
                  fracRatingRank = (1.0+(float)l)/(float)clustSeller.Len();
                  break;
                }

              for (int l=0; l<sellMsg.Len(); l++)
                if (sellMsg.GetKey(l) == sID) {
                  invMsgRank = 1.0/(1.0+(float)l);
                  fracMsgRank = (1.0+(float)l)/(float)clustSeller.Len();
                  break;
                }
              
              float priceDif = purchasePrice-clustMed; 
              float ratingDif = sellerRating.GetDat(sID)-medRating; 
              // NEW: assume average if rating info is missing
              // ratings are missing for 20% of the data?
              if (sellerRating.GetDat(sID) < 0.0) {
                ratingDif = 0.0;
                invRatingRank = 1.0/(1.0+(float)sellerRating.Len()/2.0);
                fracRatingRank = 0.5;
              }

              // TOUSE
              float propPriceDif = 0.0;
              if (clustMed != 0)
                propPriceDif = priceDif/clustMed;
              float propRatingDif = 0.0;
              if (medRating != 0)
                propRatingDif = ratingDif/medRating;
        

              int isContact = 0;
              float msgVol = 0.0;  // no limit bidir
              float dayMsgVol = 0.0; // 1 day bidir
              float medMsgVol = 0.0; // 3 day bidir
              float weekMsgVol = 0.0; // 7 day bidir
              float fwdMsgVol = 0.0; // no limit fwd. use for ppr
              float bwdMsgVol = 0.0; // no limit bwd
              int daySinceMsg = 100; // default max = 100
              float mutualCon = 0.0;
              float mutualMsg = 0.0;
              float tradeVol = 0.0; // between buyer and seller
              int daySinceTrade = 100;
              float medTradeVol = 0.0;
              float weekTradeVol = 0.0;
              float sellerVol = 0.0;
              float sellerDeg = 0.0;
              // loaded vars
              int bNumClusters = 0;
              int sNumClusters = 0;
              int bCategories = 0;
              int sCategories = 0;
              float bRelativePrice = 0.0;
              float sRelativePrice = 0.0;
              // more time decay
              float dayST1 = 0.0; // day since trade
              float dayST2 = 0.0;
              float dayST3 = 0.0;
              float daySM1 = 0.0;
              float daySM2 = 0.0;
              float daySM3 = 0.0;
              // new seller profile info
              int sDealRecord = 0; // use abs value
              float sProtection = 0.0;
              float sPic = 0.0;
              float sCredit = 0.0; // measure 4 ways
              float invCreditRank;
              float fracCreditRank;
              float sDesc = 0.0;
              float sServ = 0.0;
              float sSpeed = 0.0;

              float sFwdPR = 0.0;
              float sMsgPR = 0.0;
              float sConPR = 0.0;

              if (fwdPR.IsKey(sID))
                sFwdPR = fwdPR.GetDat(sID);
              if (msgPR.IsKey(sID))
                sMsgPR = msgPR.GetDat(sID);
              if (conPR.IsKey(sID))
                sConPR = conPR.GetDat(sID);

              if (sellerDR.IsKey(sID))
                sDealRecord = sellerDR.GetDat(sID);
              if (sellerProtection.IsKey(sID))
                sProtection = sellerProtection.GetDat(sID);
              if (sellerPic.IsKey(sID))
                sPic = sellerPic.GetDat(sID);
              if (sellerCredit.IsKey(sID))
                sCredit = (float)sellerCredit.GetDat(sID);
              if (sellerDesc.IsKey(sID))
                sDesc = sellerDesc.GetDat(sID);
              if (sellerServ.IsKey(sID))
                sServ = sellerServ.GetDat(sID);
              if (sellerSpeed.IsKey(sID))
                sSpeed = sellerSpeed.GetDat(sID);

              for (int l=0; l<sellerCredit.Len(); l++)
                if (sellerCredit.GetKey(l) == sID) {
                  invCreditRank = 1.0/(1.0+(float)l);
                  fracCreditRank = (1.0+(float)l)/(float)clustSeller.Len();
                  break;
                }
              float creditDif = 0.0;
              if (sCredit >= 0.0)
                creditDif = (float)(sCredit - medCredit);
              float logCredit = log(1.0+(float)sCredit);

              // these ratings are scaled, so just get dif
              float descDif= 0.0;
              if (sDesc >= 0.0)
                descDif = sDesc - medDesc;
              float servDif= 0.0;
              if (sServ >= 0.0)
                servDif = sServ - medServ;
              float speedDif= 0.0;
              if (sSpeed >= 0.0)
                speedDif = sSpeed - medSpeed;

              // isKey checks shouldn't be necessary
              if (loadedBNC.IsKey(zID))
                bNumClusters = loadedBNC.GetDat(zID);
              if (loadedSNC.IsKey(sID))
                sNumClusters = loadedSNC.GetDat(sID);
              if (loadedBC.IsKey(zID))
                bCategories = loadedBC.GetDat(zID);
              if (loadedSC.IsKey(sID))
                sCategories = loadedSC.GetDat(sID);
              if (loadedBRP.IsKey(zID))
                bRelativePrice = loadedBRP.GetDat(zID);
              if (loadedSRP.IsKey(sID))
                sRelativePrice = loadedSRP.GetDat(sID);
              
              // old trade volume between buyer and seller
              if (s->IsNode(zID) && s->IsNode(sID) && s->IsEdge(zID, sID)) {
                TTaobaoShopEdge& shopEdge = s->GetEDat(zID, sID);
                for (int l=0; l<shopEdge.CntTX(); l++) {
                  int shopTime = shopEdge.GetTime(l).GetAbsSecs();
                  if (shopTime < refTime) {
                    tradeVol += 1.0;
                    int tDif = refTime-shopTime;
                    if (tDif <= weekWindow)
                      weekTradeVol += 1.0;
                    int tday = tDif/daySec;
                    if (tday < daySinceTrade)
                      daySinceTrade = tday;
                  }
                  else
                    break;
                }
              }
              tradeVol = log(1+tradeVol); 
              weekTradeVol = log(1+weekTradeVol);
              if (daySinceTrade != 100) {
                dayST1 = pow((float)daySinceTrade, td1);
                dayST2 = pow((float)daySinceTrade, td2);
                dayST3 = pow((float)daySinceTrade, td3);
              }

              // compute seller volume and degree prior to reftime
              if (s->IsNode(sID)) { // for small file testing
                TTaobaoShopNet::TNodeI SI = s->GetNI(sID);
                for (int k=0; k<SI.GetInDeg(); k++) {
                  int fromNode = SI.GetInNId(k);
                  TTaobaoShopEdge& shopEdge = s->GetEDat(fromNode, sID);
                  int fnd=0; 
                  for (int l=0; l<shopEdge.CntTX(); l++) {
                    int shopTime = shopEdge.GetTime(l).GetAbsSecs();
                    if (shopTime < refTime) {
                      sellerVol += 1.0;
                      fnd = 1;
                    }
                    else
                      break;
                  }
                  if (fnd == 1)
                    sellerDeg += 1.0;
                }
                sellerVol = log(1+sellerVol);
                sellerDeg = log(1+sellerDeg);
              }
              
              // new block
              TIntH sConNeighbors;
              TIntH sMsgNeighbors;
              float sConDeg = 0.0;
              float sMsgDeg = 0.0;
              float sConCCF = 0.0; // undirected local CCF
              float sMsgCCF = 0.0; // directed local CCF
              float mutualConProp = 0.0; // mutual / union neighborhoods
              float mutualConDensity = 0.0; // density of edges between mutuals
              float mutualMsgProp = 0.0; // mutual / union neighborhoods
              float mutualMsgDensity = 0.0; // density of edges between mutuals
              float cShortPath = 10.0; // 10 means no path
              float mShortPath = 10.0;

              if (cDist.IsKey(sID))
                cShortPath = (float) cDist.GetDat(sID);
              if (mDist.IsKey(sID))
                mShortPath = (float) mDist.GetDat(sID);
              
              if (c->IsNode(sID)) {
                TTaobaoShopNet::TNodeI SI = c->GetNI(sID);
                for (int k=0; k<SI.GetOutDeg(); k++) {
                  int toNode = SI.GetOutNId(k);
                  TTaobaoShopEdge& edge = c->GetEDat(sID, toNode);
                  if (edge.GetTime(0).GetAbsSecs() < refTime) {
                    if (!sConNeighbors.IsKey(toNode))
                      sConNeighbors.AddDat(toNode, 1);
                  }
                }
                sConDeg = log(1.0+(float)sConNeighbors.Len());

                if (sConNeighbors.Len()>=2) {
                  float connect = 0.0;
                  for (int k=0; k<sConNeighbors.Len()-1; k++) {
                    int node1 = sConNeighbors.GetKey(k);
                    for (int l=k+1; l<sConNeighbors.Len(); l++) {
                      int node2 = sConNeighbors.GetKey(l);
                      if (c->IsEdge(node1, node2)) {
                        TTaobaoShopEdge& edge = c->GetEDat(node1, node2);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                    }
                  }

                  float mostEdges = (float) (sConNeighbors.Len() * (sConNeighbors.Len()-1))/2;
                  sConCCF = connect / mostEdges;
                }
              }

              if (m->IsNode(sID)) {
                TTaobaoShopNet::TNodeI SI = m->GetNI(sID);
                for (int k=0; k<SI.GetOutDeg(); k++) {
                  int toNode = SI.GetOutNId(k);
                  TTaobaoShopEdge& edge = m->GetEDat(sID, toNode);
                  if (edge.GetTime(0).GetAbsSecs() < refTime) {
                    if (!sMsgNeighbors.IsKey(toNode))
                      sMsgNeighbors.AddDat(toNode, 1);
                  }
                }

                for (int k=0; k<SI.GetInDeg(); k++) {
                  int fromNode = SI.GetInNId(k);
                  TTaobaoShopEdge& edge = m->GetEDat(fromNode, sID);
                  if (edge.GetTime(0).GetAbsSecs() < refTime) {
                    if (!sMsgNeighbors.IsKey(fromNode))
                      sMsgNeighbors.AddDat(fromNode, 1);
                  }
                }

                sMsgDeg = log(1.0+(float)sMsgNeighbors.Len());

                if (sMsgNeighbors.Len()>=2) {
                  float connect = 0.0;
                  for (int k=0; k<sMsgNeighbors.Len()-1; k++) {
                    int node1 = sMsgNeighbors.GetKey(k);
                    for (int l=k+1; l<sMsgNeighbors.Len(); l++) {
                      int node2 = sMsgNeighbors.GetKey(l);
                      if (m->IsEdge(node1, node2)) {
                        TTaobaoShopEdge& edge = m->GetEDat(node1, node2);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                      if (m->IsEdge(node2, node1)) {
                        TTaobaoShopEdge& edge = m->GetEDat(node2, node1);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                    }
                  }
                 
                  float mostEdges = (float) (sConNeighbors.Len() * (sConNeighbors.Len()-1));
                  sMsgCCF = connect / mostEdges;
                }
              }


              // ignore conPos, conNeg deprecated
              if (c->IsNode(zID) && c->IsNode(sID) && c->IsEdge(zID, sID)) {
                TTaobaoShopEdge& conEdge = c->GetEDat(zID, sID);
                int conTime = conEdge.GetTime(0).GetAbsSecs(); 
                if (conTime < refTime) { // was originally <= refTime
                  isContact = 1;
                  conPos++;
                }
                
                TIntH mutConNeigh;
                TTaobaoShopNet::TNodeI NI = c->GetNI(zID);
                for (int l=0; l<NI.GetOutDeg(); l++) {                                                                                                  
                  int toNode = NI.GetOutNId(l);
                  TTaobaoShopEdge& conEdge2 = c->GetEDat(zID, toNode);
                  int conTime2 = conEdge2.GetTime(0).GetAbsSecs();
                  if (conTime2 < refTime) {
                    if (c->IsEdge(toNode, sID)) {
                      TTaobaoShopEdge& conEdge3 = c->GetEDat(toNode, sID);
                      int conTime3 = conEdge3.GetTime(0).GetAbsSecs();
                      if(conTime3 < refTime) {
                        mutualCon += 1.0;
                        mutConNeigh.AddDat(toNode, 1);
                      }
                    }
                  }
                }

                // count number of edges between mutualConNeighbor
                if (mutConNeigh.Len()>=2) {
                  float connect = 0.0;
                  for (int k=0; k<mutConNeigh.Len()-1; k++) {
                    int node1 = mutConNeigh.GetKey(k);
                    for (int l=k+1; l<mutConNeigh.Len(); l++) {
                      int node2 = mutConNeigh.GetKey(l);
                      if (c->IsEdge(node1, node2)) {
                        TTaobaoShopEdge& edge = c->GetEDat(node1, node2);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                    }
                  }

                  float mostEdges = (float) (mutConNeigh.Len() * (mutConNeigh.Len()-1)/2);
                  mutualConDensity = connect / mostEdges;
                }

                // find size of union z neighbors + s neighbors
                float unionSize = zConNeighbors.Len();
                for (int k=0; k<sConNeighbors.Len(); k++)
                  if (!zConNeighbors.IsKey(sConNeighbors.GetKey(k)))
                    unionSize += 1.0;
                mutualConProp = mutualCon / unionSize;
                
                mutualCon = log(1.0+mutualCon);
              }
              else 
                conNeg++;
              
              if (m->IsNode(zID) && m->IsNode(sID)) {
                if (m->IsEdge(zID, sID)) {
                  TTaobaoShopEdge& edge = m->GetEDat(zID, sID);
                  int l=0;
                  while (l < edge.CntTX()) {
                    int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                    if (tDif <= 0) // was counting same day with <
                      break;
                    if (tDif <= msgWindow) {
                      msgVol += (float)edge.GetStrength(l);
                      fwdMsgVol += (float)edge.GetStrength(l);
                    }
                    if (tDif <= dayWindow)
                      dayMsgVol += (float)edge.GetStrength(l);
                    if (tDif <= medWindow)
                      medMsgVol += (float)edge.GetStrength(l);
                    if (tDif <= weekWindow)
                      weekMsgVol += (float)edge.GetStrength(l);
                    int tday = tDif/daySec;
                    if (tday < daySinceMsg)
                      daySinceMsg = tday;

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
                    if (tDif <= msgWindow) {
                      msgVol += (float)edge.GetStrength(l);
                      bwdMsgVol += (float)edge.GetStrength(l);
                    }
                    if (tDif <= dayWindow)
                      dayMsgVol += (float)edge.GetStrength(l);
                    if (tDif <= medWindow)
                      medMsgVol += (float)edge.GetStrength(l);
                    if (tDif <= weekWindow)
                      weekMsgVol += (float)edge.GetStrength(l);
                    int tday = tDif/daySec;
                    if (tday < daySinceMsg)
                      daySinceMsg = tday;

                    l++;
                  }
                }
                
                msgVol = log(1.0+msgVol);
                dayMsgVol = log(1.0+dayMsgVol);
                medMsgVol = log(1.0+medMsgVol);
                weekMsgVol = log(1.0+weekMsgVol);
                fwdMsgVol = log(1.0+fwdMsgVol);
                bwdMsgVol = log(1.0+bwdMsgVol);
                //sellMsg.AddDat(sID, msgVol);
                if (daySinceMsg != 100) {
                  daySM1 = pow((float)daySinceMsg, td1);
                  daySM2 = pow((float)daySinceMsg, td2);
                  daySM3 = pow((float)daySinceMsg, td3);
                }


                // count number of common msg nodes
                // eg not mutual volume
                TIntH candMutualMsg;
                TIntH mutMsgNeigh; // use above to construct this
                TTaobaoShopNet::TNodeI NI = m->GetNI(zID);                
                
                // go through indeg, outdeg, add all qualified nodes to a hashset  
                for (int k=0; k<NI.GetOutDeg(); k++) {
                  int toNode = NI.GetOutNId(k);
                  TTaobaoShopEdge& edge = m->GetEDat(zID, toNode);
                  int l=0;
                  while (l < edge.CntTX()) {
                    int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                    if (tDif <= 0) // changed to <=                                                                                                         
                      break;
                    if (tDif <= msgWindow) {
                      if (!candMutualMsg.IsKey(toNode))
                        candMutualMsg.AddDat(toNode, 1);
                      break; // okay
                    }
                    l++;
                  }
                }
                
                for (int k=0; k<NI.GetInDeg(); k++) {
                  int toNode = NI.GetInNId(k);
                  TTaobaoShopEdge& edge = m->GetEDat(toNode, zID);
                  int l=0;
                  while (l < edge.CntTX()) {
                    int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                    if (tDif <= 0) // changed to <=                                                                                                         
                      break;
                    if (tDif <= msgWindow) {
                      if (!candMutualMsg.IsKey(toNode))
                        candMutualMsg.AddDat(toNode, 1);
                      break; // okay                                                                                                                         
                    }                                   
                    l++;
                  }
                }

                // check each member of hash to seller
                for (int k=0; k<candMutualMsg.Len(); k++) {
                  int thirdNode = candMutualMsg.GetKey(k);
                  int valid=0; 
                  
                  if (m->IsEdge(thirdNode, sID)) {
                    TTaobaoShopEdge& edge = m->GetEDat(thirdNode, sID);
                    int l=0;
                    while (l < edge.CntTX()) {
                      int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                      if (tDif <= 0) // changed to <= 
                        break;
                      if (tDif <= msgWindow) {
                        valid = 1;
                        break; // okay
                      }
                      l++;
                    }
                  }

                  if (valid==0 && m->IsEdge(sID, thirdNode)) {
                    TTaobaoShopEdge& edge = m->GetEDat(sID, thirdNode);
                    int l=0;                    
                    while (l < edge.CntTX()) {
                      int tDif = refTime-edge.GetTime(l).GetAbsSecs();
                      if (tDif <= 0) // changed to <=
                        break;
                      if (tDif <= msgWindow) {
                        valid = 1;                        
                        break; // okay                                                                                                  
                      }
                      l++;
                    }
                  }
                  
                  if (valid == 1) {
                    mutualMsg += 1.0;
                    mutMsgNeigh.AddDat(thirdNode, 1);
                  }
                } // close k loop
                
                // count number of edges between mutMsgNeighbor
                if (mutMsgNeigh.Len()>=2) {
                  float connect = 0.0;
                  for (int k=0; k<mutMsgNeigh.Len()-1; k++) {
                    int node1 = mutMsgNeigh.GetKey(k);
                    for (int l=k+1; l<mutMsgNeigh.Len(); l++) {
                      int node2 = mutMsgNeigh.GetKey(l);
                      if (m->IsEdge(node1, node2)) {
                        TTaobaoShopEdge& edge = m->GetEDat(node1, node2);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }

                      if (m->IsEdge(node2, node1)) {
                        TTaobaoShopEdge& edge = m->GetEDat(node2, node1);
                        if (edge.GetTime(0).GetAbsSecs() < refTime)
                          connect += 1.0;
                      }
                    }
                  }

                  float mostEdges = (float)(mutMsgNeigh.Len() * (mutMsgNeigh.Len()-1));
                  mutualMsgDensity = connect / mostEdges;
                }

                // find size of union z neighbors + s neighbors
                float unionSize = (float)zMsgNeighbors.Len();
                for (int k=0; k<sMsgNeighbors.Len(); k++)
                  if (!zMsgNeighbors.IsKey(sMsgNeighbors.GetKey(k)))
                    unionSize += 1.0;
                mutualMsgProp = mutualMsg / unionSize;
              }
              
              if (msgVol>0.0) // not used, ignore
                msgPos++;
              else
                msgNeg++;
              
              mutualMsg = log(1.0+mutualMsg);
             
              // NOTE: msgVol split into fwd and bwd. Also tradeVol feature number changed   
              fprintf(subgraphs, "%d %d 1:%f 2:%f 3:%d 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f\n", zID, sID, priceDif, ratingDif, isContact, fwdMsgVol, bwdMsgVol, tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol, sellerDeg, sellerVol);

              if (zID == bID) {
                // output features for diagnostics
                /*
                fprintf(wekaBig,
              "%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,",
                        priceDif, ratingDif, isContact, fwdMsgVol,
                        bwdMsgVol,
                        tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                        sellerDeg, sellerVol, invPriceRank, invRatingRank,
                        invMsgRank, weekMsgVol,
                        weekTradeVol, bNumClusters, sNumClusters,
                        bCategories, sCategories, bRelativePrice,
                        sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                        dayST1, dayST2, dayST3, daySM1, daySM2, daySM3);
                */
                fprintf(wekaBig, "%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,",
                        priceDif, ratingDif, isContact, fwdMsgVol,
                bwdMsgVol,
                        tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                        sellerDeg, sellerVol, invPriceRank, invRatingRank,
                        invMsgRank, weekMsgVol,
                        weekTradeVol, bNumClusters, sNumClusters,
                        bCategories, sCategories, bRelativePrice,
                        sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                        dayST1, dayST2, dayST3, daySM1, daySM2, daySM3,
                        fracPriceRank, fracRatingRank, fracMsgRank,
                        propPriceDif, propRatingDif, sDealRecord,
                        sProtection, sPic, invCreditRank, fracCreditRank,
                        creditDif, logCredit, sDesc, sServ, sSpeed, descDif,
                        servDif, speedDif, zConDeg, zMsgDeg, zConCCF,
                        zMsgCCF,
                        sConDeg, sMsgDeg, sConCCF, sMsgCCF, mutualConProp,
                        mutualConDensity, mutualMsgProp, mutualMsgDensity,
                        zFwdPR, zMsgPR, zConPR, sFwdPR, sMsgPR, sConPR,
                        cShortPath, mShortPath);


                fprintf(wekaSmall,
                        "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,",
                        isContact, msgVol,
                        tradeVol, mutualCon, mutualMsg, sellerDeg, sellerVol,
                        invPriceRank, invRatingRank, invMsgRank, dayST2,
                        daySM2, priceDif, ratingDif);
                if (mtch==1) {
                  fprintf(wekaBig,"YES\n");
                  fprintf(wekaSmall,"YES\n");
                }
                else {
                  fprintf(wekaBig,"NO\n");
                  fprintf(wekaSmall,"NO\n");
                }

                if (mtch == 1) {
                  // first param is category number, for excel use
                  /*
                  fprintf(posCat, "%d %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f\n",
                          catIndex, priceDif, ratingDif, isContact, fwdMsgVol,
                  bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3);
                  */
                  // for R 
                  /*
                  fprintf(posRBig, "%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                          priceDif, ratingDif, isContact, fwdMsgVol,
                          bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3);
                  fprintf(posRSmall, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", isContact, msgVol,
                          tradeVol, mutualCon, mutualMsg, sellerDeg, sellerVol,
                          invPriceRank, invRatingRank, invMsgRank, dayST2,
                          daySM2, priceDif, ratingDif);
                  */
                }

                // 25% test set
                if (qID%4 != 0 ) {
                  if (mtch==0) {
                    fprintf(trainRank, "1 qid:%d ", qID);
                    fprintf(trainRankSmall, "1 qid:%d ", qID);
                    //fprintf(trainMap, "-1 qid:%d ", qID);
                    fprintf(rawTrain, "1 %d ", qID);
                    fprintf(trainRankA, "1 qid:%d ", qID);
                    fprintf(trainRankB, "1 qid:%d ", qID);
                    fprintf(trainRankC, "1 qid:%d ", qID);
                    fprintf(trainRankD, "1 qid:%d ", qID);
                    fprintf(trainRankE, "1 qid:%d ", qID);
                    fprintf(trainRankF, "1 qid:%d ", qID);
                    fprintf(trainRankG, "1 qid:%d ", qID);
                    fprintf(trainFiles[catIndex], "1 qid:%d ", qID);
                  }
                  else {
                    fprintf(trainRank, "2 qid:%d ", qID);
                    fprintf(trainRankSmall, "2 qid:%d ", qID);
                    //fprintf(trainMap, "1 qid:%d ", qID);
                    fprintf(rawTrain, "2 %d ", qID);
                    fprintf(trainRankA, "2 qid:%d ", qID);
                    fprintf(trainRankB, "2 qid:%d ", qID);
                    fprintf(trainRankC, "2 qid:%d ", qID);
                    fprintf(trainRankD, "2 qid:%d ", qID);
                    fprintf(trainRankE, "2 qid:%d ", qID);
                    fprintf(trainRankF, "2 qid:%d ", qID);
                    fprintf(trainRankG, "2 qid:%d ", qID);
                    fprintf(trainFiles[catIndex], "2 qid:%d ", qID);
                  }

                  // SHOULD I NORMALIZE FEATURE VECTORS TO BE LENGTH 1?
                  // which features should i pre-logspace
                                   
                  //fprintf(trainRank, "1:%f 2:%d 3:%f 4:%f 5:%f 6:%f 7:%f\n", priceDif, isContact, msgVol, tradeVol, mutualCon, mutualMsg, sellerVol);
                  //fprintf(trainMap, "1:%f 2:%d 3:%f 4:%f 5:%f 6:%f 7:%f\n", priceDif, isContact, msgVol, tradeVol, mutualCon, mutualMsg, sellerVol);
                  fprintf(trainRank, "1:%f 2:%f 3:%d 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%d 19:%d 20:%d 21:%d 22:%f 23:%f 24:%f 25:%f 26:%f 27:%f 28:%f 29:%f 30:%f 31:%f 32:%f 33:%f 34:%f 35:%f 36:%f 37:%f 38:%d 39:%f 40:%f 41:%f 42:%f 43:%f 44:%f 45:%f 46:%f 47:%f 48:%f 49:%f 50:%f 51:%f 52:%f 53:%f 54:%f 55:%f 56:%f 57:%f 58:%f 59:%f 60:%f 61:%f 62:%f 63:%f 64:%f 65:%f 66:%f 67:%f 68:%f 69:%f 70:%f\n", 
                          priceDif, ratingDif, isContact, fwdMsgVol, bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3,
                          fracPriceRank, fracRatingRank, fracMsgRank,
                          propPriceDif, propRatingDif, sDealRecord,
                          sProtection, sPic, invCreditRank, fracCreditRank,
                          creditDif, logCredit, sDesc, sServ, sSpeed, descDif,
                          servDif, speedDif, zConDeg, zMsgDeg, zConCCF, zMsgCCF, 
                          sConDeg, sMsgDeg, sConCCF, sMsgCCF, mutualConProp,
                          mutualConDensity, mutualMsgProp, mutualMsgDensity,
                          zFwdPR, zMsgPR, zConPR, sFwdPR, sMsgPR, sConPR, cShortPath, mShortPath);

                  fprintf(trainRankSmall, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%d 19:%d 20:%f 21:%d 22:%f 23:%f 24:%f 25:%f 26:%f\n",
                          isContact, fwdMsgVol, bwdMsgVol, tradeVol, mutualCon, mutualMsg, buyerVol, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, dayST2, daySM1, daySM2, fracPriceRank, fracRatingRank, fracMsgRank,
                          propPriceDif, propRatingDif, sDealRecord,
                          invCreditRank, fracCreditRank, logCredit, descDif, servDif, speedDif);

                  // select from 50 features later  
                  fprintf(trainFiles[catIndex], "1:%f 2:%f 3:%d 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%d 19:%d 20:%d 21:%d 22:%f 23:%f 24:%f 25:%f 26:%f 27:%f 28:%f 29:%f 30:%f 31:%f 32:%f\n", 
                          priceDif, ratingDif, isContact, fwdMsgVol, bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3);
                  
                  // first var is catIndex, for construction of catFiles
                  fprintf(rawTrain, "%d %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
                          catIndex, priceDif, ratingDif, isContact, fwdMsgVol, bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3,
                          fracPriceRank, fracRatingRank, fracMsgRank,
                          propPriceDif, propRatingDif, sDealRecord,
                          sProtection, sPic, invCreditRank, fracCreditRank,
                          creditDif, logCredit, sDesc, sServ, sSpeed, descDif,
                          servDif, speedDif, zConDeg, zMsgDeg, zConCCF,
                  zMsgCCF,
                          sConDeg, sMsgDeg, sConCCF, sMsgCCF, mutualConProp,
                          mutualConDensity, mutualMsgProp, mutualMsgDensity,
                          zFwdPR, zMsgPR, zConPR, sFwdPR, sMsgPR, sConPR,
                          cShortPath, mShortPath);

                  // basic setup, no relative price/rating
                  fprintf(trainRankA, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f\n", isContact, msgVol, tradeVol,
                          mutualCon, mutualMsg, sellerDeg, sellerVol, invPriceRank, invRatingRank, invMsgRank, dayST2, daySM2);
                  // note feature order changed, 2 tacked on at end
                  fprintf(trainRankB, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f\n", isContact, msgVol, tradeVol,
                          mutualCon, mutualMsg, sellerDeg, sellerVol,
                  invPriceRank, invRatingRank, invMsgRank, dayST2, daySM2, priceDif, ratingDif);
                  // no contact info
                  fprintf(trainRankC, "1:%f 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f\n", 
                          msgVol, tradeVol, mutualMsg, sellerDeg, sellerVol,
                          invPriceRank, invRatingRank, invMsgRank, dayST2, daySM2, priceDif, ratingDif);
                  // no msg info
                  fprintf(trainRankD, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f\n", 
                          isContact, tradeVol, mutualCon, sellerDeg, sellerVol,
                  invPriceRank, invRatingRank, dayST2, priceDif, ratingDif);
                  // no trade info
                  fprintf(trainRankE, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f\n", 
                          isContact, msgVol, mutualCon, mutualMsg, invPriceRank, invRatingRank, invMsgRank, daySM2, priceDif, ratingDif);
                  // network features only
                  fprintf(trainRankF, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f\n", isContact, msgVol, tradeVol,
                          mutualCon, mutualMsg, sellerDeg, sellerVol, dayST2, daySM2);
                  // non-network features only
                  fprintf(trainRankG, "1:%f 2:%f 3:%f 4:%f\n", invPriceRank, invRatingRank, priceDif, ratingDif);

                  trainSize[catIndex]++;
                }
                else { // qID%4==0
                  if (mtch==0) {
                     fprintf(testRank, "1 qid:%d ", qID);
                     fprintf(testRankSmall, "1 qid:%d ", qID);
                     //fprintf(testMap, "-1 qid:%d ", qID);
                    fprintf(rawTest, "1 %d ", qID);
                    fprintf(testRankA, "1 qid:%d ", qID);
                    fprintf(testRankB, "1 qid:%d ", qID);
                    fprintf(testRankC, "1 qid:%d ", qID);
                    fprintf(testRankD, "1 qid:%d ", qID);
                    fprintf(testRankE, "1 qid:%d ", qID);
                    fprintf(testRankF, "1 qid:%d ", qID);
                    fprintf(testRankG, "1 qid:%d ", qID);
                    fprintf(testFiles[catIndex], "1 qid:%d ", qID);
                  } 
                  else {
                     fprintf(testRank, "2 qid:%d ", qID);
                     fprintf(testRankSmall, "2 qid:%d ", qID);
                     //fprintf(testMap, "1 qid:%d ", qID);
                    fprintf(rawTest, "2 %d ", qID);
                    fprintf(testRankA, "2 qid:%d ", qID);
                    fprintf(testRankB, "2 qid:%d ", qID);
                    fprintf(testRankC, "2 qid:%d ", qID);
                    fprintf(testRankD, "2 qid:%d ", qID);
                    fprintf(testRankE, "2 qid:%d ", qID);
                    fprintf(testRankF, "2 qid:%d ", qID);
                    fprintf(testRankG, "2 qid:%d ", qID);
                    fprintf(testFiles[catIndex], "2 qid:%d ", qID);
                  }
                  
                  //fprintf(testRank, "1:%f 2:%d 3:%f 4:%f 5:%f 6:%f 7:%f\n", priceDif, isContact, msgVol, tradeVol, mutualCon, mutualMsg, sellerVol);
                  //fprintf(testMap, "1:%f 2:%d 3:%f 4:%f 5:%f 6:%f 7:%f\n", priceDif, isContact, msgVol, tradeVol, mutualCon, mutualMsg, sellerVol);
                  
                  fprintf(testRank, "1:%f 2:%f 3:%d 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%d 19:%d 20:%d 21:%d 22:%f 23:%f 24:%f 25:%f 26:%f 27:%f 28:%f 29:%f 30:%f 31:%f 32:%f 33:%f 34:%f 35:%f 36:%f 37:%f 38:%d 39:%f 40:%f 41:%f 42:%f 43:%f 44:%f 45:%f 46:%f 47:%f 4\
8:%f 49:%f 50:%f 51:%f 52:%f 53:%f 54:%f 55:%f 56:%f 57:%f 58:%f 59:%f 60:%f 61:%f 62:%f 63:%f 64:%f 65:%f 66:%f 67:%f 68:%f 69:%f 70:%f\n",
                          priceDif, ratingDif, isContact, fwdMsgVol, bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3,
                          fracPriceRank, fracRatingRank, fracMsgRank,
                          propPriceDif, propRatingDif, sDealRecord,
                          sProtection, sPic, invCreditRank, fracCreditRank,
                          creditDif, logCredit, sDesc, sServ, sSpeed, descDif,
                          servDif, speedDif, zConDeg, zMsgDeg, zConCCF, zMsgCCF,
                          sConDeg, sMsgDeg, sConCCF, sMsgCCF, mutualConProp,
                          mutualConDensity, mutualMsgProp, mutualMsgDensity,
                          zFwdPR, zMsgPR, zConPR, sFwdPR, sMsgPR, sConPR,
                          cShortPath, mShortPath);

                  fprintf(testRankSmall, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%d 19:%d 20:%f 21:%d 22:%f 23:%f 24:%f 25:%f 26:%f\n",
                          isContact, fwdMsgVol, bwdMsgVol, tradeVol, mutualCon,
                          mutualMsg, buyerVol, sellerVol, invPriceRank,
                  invRatingRank,
                          invMsgRank, dayST2, daySM1, daySM2, fracPriceRank,
                          fracRatingRank, fracMsgRank,
                          propPriceDif, propRatingDif, sDealRecord,
                          invCreditRank, fracCreditRank, logCredit, descDif,
                          servDif, speedDif);

                  fprintf(testFiles[catIndex], "1:%f 2:%f 3:%d 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%d 19:%d 20:%d 21:%d 22:%f 23:%f 24:%f 25:%f 26:%f 27:%f 28:%f 29:%f 30:%f 31:%f 32:%f\n", priceDif,
                          ratingDif, isContact, fwdMsgVol, bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3);

                  fprintf(rawTest, "%d %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %f\
 %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
                          catIndex, priceDif, ratingDif, isContact, fwdMsgVol,
                  bwdMsgVol,
                          tradeVol, mutualCon, mutualMsg, buyerDeg, buyerVol,
                          sellerDeg, sellerVol, invPriceRank, invRatingRank,
                          invMsgRank, weekMsgVol,
                          weekTradeVol, bNumClusters, sNumClusters,
                          bCategories, sCategories, bRelativePrice,
                          sRelativePrice, edgeTime1, edgeTime2, edgeTime3,
                          dayST1, dayST2, dayST3, daySM1, daySM2, daySM3,
                          fracPriceRank, fracRatingRank, fracMsgRank,
                          propPriceDif, propRatingDif, sDealRecord,
                          sProtection, sPic, invCreditRank, fracCreditRank,
                          creditDif, logCredit, sDesc, sServ, sSpeed, descDif,
                          servDif, speedDif, zConDeg, zMsgDeg, zConCCF,
                          zMsgCCF,
                          sConDeg, sMsgDeg, sConCCF, sMsgCCF, mutualConProp,
                          mutualConDensity, mutualMsgProp, mutualMsgDensity,
                          zFwdPR, zMsgPR, zConPR, sFwdPR, sMsgPR, sConPR,
                          cShortPath, mShortPath);

                  fprintf(testRankA, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f\n", isContact, msgVol,
                          tradeVol, mutualCon, mutualMsg, sellerDeg, sellerVol,
                          invPriceRank, invRatingRank, invMsgRank, dayST2, daySM2);
                  fprintf(testRankB, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f\n", isContact,
                          msgVol, tradeVol, mutualCon, mutualMsg, sellerDeg, sellerVol,
                          invPriceRank, invRatingRank, invMsgRank, dayST2, daySM2, priceDif, ratingDif);
                  fprintf(testRankC, "1:%f 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f\n",
                          msgVol, tradeVol, mutualMsg, sellerDeg, sellerVol,
                          invPriceRank, invRatingRank, invMsgRank, dayST2,
                          daySM2, priceDif, ratingDif);
                  fprintf(testRankD, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f\n",
                          isContact, tradeVol, mutualCon, sellerDeg, sellerVol,
                          invPriceRank, invRatingRank, dayST2, priceDif, ratingDif);
                  fprintf(testRankE, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f\n",
                          isContact, msgVol, mutualCon, mutualMsg,
                          invPriceRank, invRatingRank, invMsgRank, daySM2,
                          priceDif, ratingDif);
                  fprintf(testRankF, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f\n", isContact, msgVol, tradeVol,
                          mutualCon, mutualMsg, sellerDeg, sellerVol, dayST2, daySM2);
                  // non-network features only
                  fprintf(testRankG, "1:%f 2:%f 3:%f 4:%f\n", invPriceRank, invRatingRank, priceDif, ratingDif);
                  testSize[catIndex]++;
                }
              
              } // close zId == bId
              
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
                  }
                  else {
                    randF += 1.0;
                    PrandF[catIndex] += 1.0;
                  }

                  for (int l=0; l<randV.Len(); l++)
                    if (randV[l] == 2) {
                      randMRRCnt += 1.0;
                      PrandMRRCnt[catIndex] += 1.0;
                      randMRRTot += 1.0/(float)(1.0+l);
                      PrandMRRTot[catIndex] += 1.0/(float)(1.0+l);
                      randMRCnt += 1.0;
                      PrandMRCnt[catIndex] += 1.0;
                      randMRTot += (float)(1.0+l);
                      PrandMRTot[catIndex] += (float)(1.0+l);

                      float frac = (float)(1.0+l)/(float)clustSeller.Len();
                      randMRFRTot += 1.0/frac;
                      PrandMRFRTot[catIndex] += 1.0/frac;
                      randMFRTot += frac;
                      PrandMFRTot[catIndex] += frac;
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
                        choosePriceMRRTot += 1.0/(float)(1.0+l2);
                        PchoosePriceMRRTot[catIndex] += 1.0/(float)(1.0+l2);
                        choosePriceMRCnt += 1.0;
                        PchoosePriceMRCnt[catIndex] += 1.0;
                        choosePriceMRTot += (float)(1.0+l2);
                        PchoosePriceMRTot[catIndex] += (float)(1.0+l2);
                       
                        float frac = (float)(1.0+l2)/(float)clustSeller.Len();
                        choosePriceMRFRTot += 1.0/frac;
                        PchoosePriceMRFRTot[catIndex] += 1.0/frac;
                        choosePriceMFRTot += frac;
                        PchoosePriceMFRTot[catIndex] += frac;
                        
                        break;
                      }
                  }
                  if (good == 1) {
                    choosePriceTP += 1.0;
                    PchoosePriceTP[catIndex] += 1.0;
                  }
                  else {
                    choosePriceF += 1.0;
                    PchoosePriceF[catIndex] += 1.0;
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
                          chooseMsgMRRTot += 1.0/(float)(1.0+l2);
                          PchooseMsgMRRTot[catIndex] += 1.0/(float)(1.0+l2);
                          chooseMsgMRCnt += 1.0;
                          PchooseMsgMRCnt[catIndex] += 1.0;
                          chooseMsgMRTot += (float)(1.0+l2);
                          PchooseMsgMRTot[catIndex] += (float)(1.0+l2);

                          float frac = (float)(1.0+l2)/(float)clustSeller.Len();
                          chooseMsgMRFRTot += 1.0/frac;
                          PchooseMsgMRFRTot[catIndex] += 1.0/frac;
                          chooseMsgMFRTot += frac;
                          PchooseMsgMFRTot[catIndex] += frac;

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
                        chooseMsgMRRTot += 1.0/expectedRank;
                        PchooseMsgMRRTot[catIndex] += 1.0/expectedRank;
                        chooseMsgMRCnt += 1.0;
                        PchooseMsgMRCnt[catIndex] += 1.0;
                        chooseMsgMRTot += expectedRank;
                        PchooseMsgMRTot[catIndex] += expectedRank;
                      
                        float frac = expectedRank/(float)clustSeller.Len();
                        chooseMsgMRFRTot += 1.0/frac;
                        PchooseMsgMRFRTot[catIndex] += 1.0/frac;
                        chooseMsgMFRTot += frac;
                        PchooseMsgMFRTot[catIndex] += frac;
                        
                      }
                    }

                    //chooseMsgTotal += 1.0/(float)corSell.Len();
                    if (good == 1) {
                      chooseMsgTP += 1.0;
                      PchooseMsgTP[catIndex] += 1.0;
                    }
                    else {
                      chooseMsgF += 1.0;
                      PchooseMsgF[catIndex] += 1.0;
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
                    }
                    else {
                      chooseMsgF += 1.0;
                      PchooseMsgF[catIndex] += 1.0;
                    }

                    for (int l=0; l<randV.Len(); l++)
                      if (randV[l] == 2) {
                        chooseMsgMRRCnt += 1.0;
                        PchooseMsgMRRCnt[catIndex] += 1.0;
                        chooseMsgMRRTot += 1.0/(float)(1.0+l);
                        PchooseMsgMRRTot[catIndex] += 1.0/(float)(1.0+l);
                        chooseMsgMRCnt += 1.0;
                        PchooseMsgMRCnt[catIndex] += 1.0;
                        chooseMsgMRTot += (float)(1.0+l);
                        PchooseMsgMRTot[catIndex] += (float)(1.0+l);

                        float frac = (float)(1.0+l)/(float)clustSeller.Len();
                        chooseMsgMRFRTot += 1.0/frac;
                        PchooseMsgMRFRTot[catIndex] += 1.0/frac;
                        chooseMsgMFRTot += frac;
                        PchooseMsgMFRTot[catIndex] += frac;

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
  printf("random baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
         randTP/(randTP+randF), randMRRTot/randMRRCnt, randMRTot/randMRCnt,
  randMRFRTot/randMRRCnt, randMFRTot/randMRCnt);
  printf("chooseMinPrice baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
         choosePriceTP/(choosePriceTP+choosePriceF),
         choosePriceMRRTot/choosePriceMRRCnt,
         choosePriceMRTot/choosePriceMRCnt,
  choosePriceMRFRTot/choosePriceMRRCnt, choosePriceMFRTot/choosePriceMRCnt); 
  printf("chooseMostMsg baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
         chooseMsgTP/(chooseMsgTP+chooseMsgF), chooseMsgMRRTot/chooseMsgMRRCnt,
         chooseMsgMRTot/chooseMsgMRCnt, chooseMsgMRFRTot/chooseMsgMRRCnt, chooseMsgMFRTot/chooseMsgMRCnt);
  //printf("con posneg %d %d msg posneg %d %d\n", conPos, conNeg, msgPos, msgNeg);

  // per category baselines are just for reference
  // dont really need to combine these 
  FILE *catStats = fopen("catBaselines.out", "w");
  for (int i=0; i<catMap.Len(); i++) {
    fprintf(catStats, "category %d randInstances %f\n", i, PrandMRCnt[i]);
    fprintf(catStats, "random baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
            PrandTP[i]/(PrandTP[i]+PrandF[i]), PrandMRRTot[i]/PrandMRRCnt[i], PrandMRTot[i]/PrandMRCnt[i], PrandMRFRTot[i]/PrandMRRCnt[i], PrandMFRTot[i]/PrandMRCnt[i]);
    fprintf(catStats, "chooseMinPrice baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n",
            PchoosePriceTP[i]/(PchoosePriceTP[i]+PchoosePriceF[i]), PchoosePriceMRRTot[i]/PchoosePriceMRRCnt[i], PchoosePriceMRTot[i]/PchoosePriceMRCnt[i], PchoosePriceMRFRTot[i]/PchoosePriceMRRCnt[i], PchoosePriceMFRTot[i]/PchoosePriceMRCnt[i]);
    fprintf(catStats, "chooseMostMsg baseline precision@1 %f MRR %f MR %f MRFR %f MFR %f\n", PchooseMsgTP[i]/(PchooseMsgTP[i]+PchooseMsgF[i]), PchooseMsgMRRTot[i]/PchooseMsgMRRCnt[i], PchooseMsgMRTot[i]/PchooseMsgMRCnt[i], PchooseMsgMRFRTot[i]/PchooseMsgMRRCnt[i], PchooseMsgMFRTot[i]/PchooseMsgMRCnt[i]);

  }
  fclose(catStats);

    // one time output of statistics to file
  // these will be loaded for feature output
  /*
  if (subgraphOn == 0) {
  FILE *outBNC = fopen("buyerNumClusters.out", "w");
  for (int i=0; i<buyerNumClusters.Len(); i++)
    fprintf(outBNC, "%d %d\n", (int)buyerNumClusters.GetKey(i), (int)buyerNumClusters[i]);
  fclose(outBNC);

  FILE *outSNC = fopen("sellerNumClusters.out", "w");
  for (int i=0; i<sellerNumClusters.Len(); i++)
    fprintf(outSNC, "%d %d\n", (int)sellerNumClusters.GetKey(i), (int)sellerNumClusters[i]);
  fclose(outSNC);

  FILE *outBCats = fopen("buyerCats.out", "w");
  for (int i=0; i<buyerCats.Len(); i++) {
    TStrV cats = buyerCats[i];
    fprintf(outBCats, "%d %d\n", (int)buyerCats.GetKey(i), cats.Len());
  }
  fclose(outBCats);

  FILE *outSCats = fopen("sellerCats.out", "w");
  for (int i=0; i<sellerCats.Len(); i++) {
    TStrV cats = sellerCats[i];
    fprintf(outSCats, "%d %d\n", (int)sellerCats.GetKey(i), cats.Len());
  }
  fclose(outSCats);

  FILE *outBRP = fopen("buyerRelativePrice.out", "w");
  for (int i=0; i<buyerRelPriceTot.Len(); i++)
    fprintf(outBRP, "%d %f\n", (int)buyerRelPriceTot.GetKey(i), (float)buyerRelPriceTot[i]/(float)buyerRelPriceCnt[i]);
  fclose(outBRP);

  FILE *outSRP = fopen("sellerRelativePrice.out", "w");
  for (int i=0; i<sellerRelPriceTot.Len(); i++)
    fprintf(outSRP, "%d %f\n", (int)sellerRelPriceTot.GetKey(i), (float)sellerRelPriceTot[i]/(float)sellerRelPriceCnt[i]);
  fclose(outSRP);
  }
  */

  FILE *catSizeOut = fopen("catSizes.out", "w");
  for (int i=0; i<catMap.Len(); i++)
    fprintf(catSizeOut, "%d %d %d\n", i, trainSize[i], testSize[i]);
  fclose(catSizeOut);

  for (int i=0; i<catMap.Len(); i++) {
    fclose(trainFiles[i]);
    fclose(testFiles[i]);
  }

  fclose(in2);
  //fclose(outs);
  
  //fclose(posCat);
  //fclose(posRBig);
  //fclose(posRSmall);
  fclose(wekaBig);
  fclose(wekaSmall);

  //fclose(trainMap);
  //fclose(testMap);
  //fclose(allMap);
  fclose(trainRank);
  fclose(testRank);
  fclose(trainRankSmall);
  fclose(testRankSmall);

  //fclose(allRank);
  fclose(rawTrain);
  fclose(rawTest);
  fclose(trainRankA);
  fclose(testRankA);
  fclose(trainRankB);
  fclose(testRankB);
  fclose(trainRankC);
  fclose(testRankC);
  fclose(trainRankD);
  fclose(testRankD);
  fclose(trainRankE);
  fclose(testRankE);
  fclose(trainRankF);
  fclose(testRankF);
  fclose(trainRankG);
  fclose(testRankG);

  fclose(subgraphs);
}

/* load cat info, do diad and triad analysis */
void catAnalysis(PTaobaoNet t) {
  PTaobaoShopNet s = t->shopNet;
  PTaobaoShopNet m = t->msgNet; // middle leg is always message
  PTaobaoShopNet x = t->shopNet; // the third leg of the triangle
  
  TStrV fields;
  char info[22][2024]; // 22 lines per product
  FILE *file = fopen("/lfs/1/tmp/mengqiu/allRaw.txt", "r");

  TStrH catCnt;
  TStrH catName;

  TStrStrH prodCat; // can product be in multiple cats? NO

  int lineCount = 0;
  int tmpCnt=0;
  int entryCnt=0;

  int dupProd=0;

  int blankSeller=0;
  
  int windowSize = 2;
  int daySec = 24*60*60;
  int window = windowSize*daySec;

  int msgWindowSize=2; // use msg window 2 for now
  int msgWindow = msgWindowSize*daySec;
  
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
        printf("badCat %s %s %s\n", prodID.CStr(), oldCat.CStr(),
      catID.CStr()); 
    }
    else {
      prodCat.AddDat(prodID, catID);
    }

    entryCnt++;
    
  }
  fclose(file);

  // do differnet purchases induce more talking

  TStrH catProdCnt;
  TStrH catMsgCnt;
  // divide these two at end to get msg/prod sold

  TStrH catTriTot;
  TStrH catTriSuc;
  // virality of categories

  for (TTaobaoShopNet::TNodeI NI = s->BegNI(); NI < s->EndNI(); NI++) {
    int buyerID = NI.GetId();
    //printf("hi");
    for (int i=0; i<NI.GetOutDeg(); i++) {
      int sellerID = NI.GetOutNId(i);
      TTaobaoShopEdge& shopEdge = NI.GetOutEDat(i);
      TSecTm tm = shopEdge.GetTime(0);
      int tmSec = tm.GetAbsSecs();
      
      TTaobaoTX firstTX = shopEdge.GetTX(0);
      if (!prodCat.IsKey(firstTX.Product))
        continue; // ignore if not classified
      
      TStr firstCat = prodCat.GetDat(firstTX.Product); 

      
      if (catProdCnt.IsKey(firstCat)) {
        int existCnt = catProdCnt.GetDat(firstCat);
        catProdCnt.AddDat(firstCat, existCnt+1);
      }
      else {
        catProdCnt.AddDat(firstCat, 1);
      }

      if (m->IsNode(buyerID)) {
        TTaobaoShopNet::TNodeI NJ = m->GetNI(buyerID);
        int candMsg = 0;
        for (int j=0; j<NJ.GetOutDeg(); j++) {
          int msgID = NJ.GetOutNId(j);
          TTaobaoShopEdge& msgEdge = NJ.GetOutEDat(j);
          
          // count and add all messages surrounding a purchase
          int msgCnt=0;
          int k=0; 
          while (k < msgEdge.CntTX()) {
            int tDif = tmSec-msgEdge.GetTime(k).GetAbsSecs();
            if (tDif <= 0)
              break;
            if (tDif <= msgWindow) // add all <= w/2 before the trade
              msgCnt += (int)msgEdge.GetStrength(k);
            k++; // increment when past trade
          }
          
          int q = k; // add all <= w/2 after the trade
          while (q < msgEdge.CntTX()) {
            int tDif = msgEdge.GetTime(q).GetAbsSecs()-tmSec;
            if (tDif <= msgWindow)
              msgCnt += (int)msgEdge.GetStrength(q);
            else
              break;
            q++;
          }

          if (catMsgCnt.IsKey(firstCat)) {
            int existCnt = catMsgCnt.GetDat(firstCat);
            catMsgCnt.AddDat(firstCat, existCnt+msgCnt);
          }
          else {
            catMsgCnt.AddDat(firstCat, msgCnt);
          }

          
          if (k<msgEdge.CntTX()) { // use first message time
            TSecTm chatTime = msgEdge.GetTime(k);
            //if (msgCnt >= 5) { // consider if at least 5 msgs exchanged
              if (catTriTot.IsKey(firstCat)) {
                int existCnt = catTriTot.GetDat(firstCat);
                catTriTot.AddDat(firstCat, existCnt+1);
              }
              else {
                catTriTot.AddDat(firstCat, 1);
              }
              
              // hack: do time check later 
              if (x->IsNode(msgID) && x->IsEdge(msgID, sellerID)) {
                TTaobaoShopEdge& closeEdge = x->GetEDat(msgID, sellerID);
                for (int l=0; l<closeEdge.CntTX(); l++) {
                  TSecTm tm2 = closeEdge.GetTime(l);
                  if (tm2 >= chatTime) {
                    if (chatTime.GetAbsSecs()+window >= tm2.GetAbsSecs()) {
                      if (catTriSuc.IsKey(firstCat)) {
                        int existCnt = catTriSuc.GetDat(firstCat);
                        catTriSuc.AddDat(firstCat, existCnt+1);
                      }
                      else {
                        catTriSuc.AddDat(firstCat, 1);
                      }
                    }
                  }
                }
              }
              //} // close msgCnt >=5
          }
          
        }
      }
      
    }
  }
  
  FILE *catout = fopen("catProd.out", "w");
  for (int i=0; i<catMsgCnt.Len(); i++)
    fprintf(catout, "%s %f\n", catMsgCnt.GetKey(i).CStr(), (float)catMsgCnt[i]/(float)catProdCnt[i]);
  fclose(catout);

  FILE *cattriout = fopen("catProdTriStrict.out", "w");
  for (int i=0; i<catTriSuc.Len(); i++)
    fprintf(cattriout, "%s %f %d\n", catTriSuc.GetKey(i).CStr(),
            (float)catTriSuc[i]/(float)catTriTot[i], (int)catTriTot[i]);
  fclose(cattriout);
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
  
  //plotEvent();
  //plotTriangle();
  //compareTriFiles();
  //plotDeg();
  //plotMsgPrice();
  //plotTP();
  //plotMT();
  
  //updatePred(t); //new, here for now
  //catAnalysis(t);

  //surprise();

  //t->LoadProduct("//lfs//1//tmp//mengqiu//cat_brand_0.1.txt");
  //plotTrust();
  //return 0;

  if (job1 == "fwd")
    t->LoadShopNet(file1);
  else if (job1 == "msg")
    t->LoadChatNet(file1, true);
  else if (job1 == "contact")
    t->LoadChatNet(file1, false);
 
  repeatBuyer(t);
  //countSellers(t);
  return 0;

  if (job2 == "fwd")
    t->LoadShopNet(file2);
  else if (job2 == "msg")
    t->LoadChatNet(file2, true);
  else if (job2 == "contact")
    t->LoadChatNet(file2, false);

  //updatePred(t);
  //catAnalysis(t);
  //clusterTest(t);
  reallyBasic(t);
  return 0;

  // loading of third datset should be optional
  
  if (job3 == "fwd")
    t->LoadShopNet(file3);
  else if (job3 == "msg")
    t->LoadChatNet(file3, true);
  else if (job3 == "contact")
    t->LoadChatNet(file3, false);

  //t->LoadRating("//lfs//1//tmp//mengqiu//seller_rating.txt");
  
  printf("Loading complete\n");
  
  //dirCounting(t);
  //permuteTypesNew(t);
  //triangleCountingNew(t);
  //updatePred(t); // load all 3
  return 0;

  //nodePairCounting(t);
  //printf("%d %d %d\n", t->shopNet->GetNodes(), t->msgNet->GetNodes(),t->contactNet->GetNodes());
  //printf("%d %d %d\n", t->shopNet->GetEdges(), t->msgNet->GetEdges(),t->contactNet->GetEdges());
  // the above line segfaults after running nodepaircounting, something screwsup

  //degCompare(t);
  //basicStats(t);
 
  //permuteEdgesUndir(t);
  //permuteEdges(t);
  //permuteTypesNew(t);
  
  //msgTrade(t);
  //tradePassing(t);
  //msgPrice(t);
  //permuteTypes(t); // OLD IGNORE
  //triangleClosed(t);
  //triangleCounting(t);
  //msgEdgeStudy(t);
  //nodePairCounting(t);
  //EventSequenceProb(t);
  //EventSequences(t);
  //EventTimePrice(t);
  //EventSequencesDetailed(t);
  //triangleClosingAll(t);
  //triangleClosing(t);
  //triangleTest(t);
  return 0;
}
