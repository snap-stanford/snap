#include "Snap.h"
#include "stdafx.h"
#include "ncp.cpp"
#include "graphing.h"
#include "taobaonet.h"
#include <stdio.h>
#include <string.h>

// split raw files by category 
// similar to select.cpp in svmRank/

/*
                          priceDif, 1
                          ratingDif, 
                          isContact, 3 INT
                          fwdMsgVol, 
                          bwdMsgVol,
                          tradeVol, 
                          mutualCon, 
                          mutualMsg, 
                          buyerDeg, 
                          buyerVol, 10
                          sellerDeg, 
                          sellerVol, 
                          invPriceRank, 
                          invRatingRank,
                          invMsgRank, 
                          weekMsgVol,
                          weekTradeVol, 
                          bNumClusters, 18 INT
                          sNumClusters,19 INT
                          bCategories, 20 INT
                          sCategories, 21 INT
                          bRelativePrice,
                          sRelativePrice, 
                          edgeTime1, 
                          edgeTime2, 
                          edgeTime3,
                          dayST1, 
                          dayST2, 
                          dayST3, 
                          daySM1, 30
                          daySM2, 
                          daySM3,
                          fracPriceRank, 
                          fracRatingRank, 
                          fracMsgRank,
                          propPriceDif, 
                          propRatingDif, 
                          sDealRecord, 38 INT
                          sProtection, 
                          sPic, 40
                          invCreditRank, 
                          fracCreditRank,
                          creditDif, 
                          logCredit, 
                          sDesc, 
                          sServ, 
                          sSpeed, 
                          descDif,
                          servDif, 
                          speedDif, 50 
                          zConDeg, 
                          zMsgDeg, 
                          zConCCF,
                          zMsgCCF,
                          sConDeg, 
                          sMsgDeg, 
                          sConCCF, 
                          sMsgCCF, 
                          mutualConProp,
                          mutualConDensity, 60 
                          mutualMsgProp, 
                          mutualMsgDensity,
                          zFwdPR, 
                          zMsgPR, 
                          zConPR, 
                          sFwdPR, 
                          sMsgPR, 
                          sConPR,
                          cShortPath, THESE 2 WRONG, NEED TO RERUN AND PARALLELIZE
                          mShortPath); 70

 */

void plotStuff() {
  // precision @ 1, reciprocal rank, rank, reciprocal fractional rank, fractional rank

  TFltPrV origP, origRR, origR, origRFR, origFR;
  TFltPrV randP, randRR, randR, randRFR, randFR;
  TFltPrV priceP, priceRR, priceR, priceRFR, priceFR;
  TFltPrV msgP, msgRR, msgR, msgRFR, msgFR;
  TFltPrV noGraphP, noGraphRR, noGraphR, noGraphRFR, noGraphFR;
  TFltPrV noMetaP, noMetaRR, noMetaR, noMetaRFR, noMetaFR;
  TFltPrV noMsgP, noMsgRR, noMsgR, noMsgRFR, noMsgFR;
  TFltPrV noConP, noConRR, noConR, noConRFR, noConFR;
  TFltPrV noTradeP, noTradeRR, noTradeR, noTradeRFR, noTradeFR;
  TFltPrV noDirectP, noDirectRR, noDirectR, noDirectRFR, noDirectFR;
  TFltPrV noMutualP, noMutualRR, noMutualR, noMutualRFR, noMutualFR;
  TFltPrV aggP, aggRR, aggR, aggRFR, aggFR; // new, combined svms
  TFltPrV msg2P, msg2RR, msg2R, msg2RFR, msg2FR;
  TFltPrV conP, conRR, conR, conRFR, conFR;
  TFltPrV tradeP, tradeRR, tradeR, tradeRFR, tradeFR;

  // msg = msgbaseline
  // msg2 = msg edge test

  FILE *in1 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutB", "r");
  FILE *in2 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutRAND", "r");
  FILE *in3 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutCHOOSEPRICE", "r");
  FILE *in4 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutCHOOSEMSG", "r");
  FILE *in5 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNOGRAPH", "r");
  FILE *in6 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNOMETA", "r");
  FILE *in7 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNOMSG", "r");
  FILE *in8 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNOCON", "r");
  FILE *in9 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNOTRADE", "r");
  // use new lines
  FILE *in10 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNODIRECT4","r"); 
  FILE *in11 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBNOMUTUAL4", "r");
  // 64 categories trained by cat-specific models, 2 trained by orig model
  FILE *in12 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/cats/agg.out", "r");
  FILE *in13 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBMSG", "r");
  FILE *in14 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBCON", "r");
  FILE *in15 = fopen("/lfs/1/tmp/mengqiu/svmRank/tests/breakoutBTRADE", "r");

  int index;
  float p1, rRank, Rank, RFRank, FRank;
  for (int i=2; i<11; i++) {
    fscanf(in1, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    origP.Add(TFltPr((float)index, p1));
    origRR.Add(TFltPr((float)index, rRank));
    origR.Add(TFltPr((float)index, Rank));
    origRFR.Add(TFltPr((float)index, RFRank));
    origFR.Add(TFltPr((float)index, FRank));
    fscanf(in2, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    randP.Add(TFltPr((float)index, p1));
    randRR.Add(TFltPr((float)index, rRank));
    randR.Add(TFltPr((float)index, Rank));
    randRFR.Add(TFltPr((float)index, RFRank));
    randFR.Add(TFltPr((float)index, FRank));
    fscanf(in3, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    priceP.Add(TFltPr((float)index, p1));
    priceRR.Add(TFltPr((float)index, rRank));
    priceR.Add(TFltPr((float)index, Rank));
    priceRFR.Add(TFltPr((float)index, RFRank));
    priceFR.Add(TFltPr((float)index, FRank));
    fscanf(in4, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    msgP.Add(TFltPr((float)index, p1));
    msgRR.Add(TFltPr((float)index, rRank));
    msgR.Add(TFltPr((float)index, Rank));
    msgRFR.Add(TFltPr((float)index, RFRank));
    msgFR.Add(TFltPr((float)index, FRank));
    fscanf(in5, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noGraphP.Add(TFltPr((float)index, p1));
    noGraphRR.Add(TFltPr((float)index, rRank));
    noGraphR.Add(TFltPr((float)index, Rank));
    noGraphRFR.Add(TFltPr((float)index, RFRank));
    noGraphFR.Add(TFltPr((float)index, FRank));
    fscanf(in6, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noMetaP.Add(TFltPr((float)index, p1));
    noMetaRR.Add(TFltPr((float)index, rRank));
    noMetaR.Add(TFltPr((float)index, Rank));
    noMetaRFR.Add(TFltPr((float)index, RFRank));
    noMetaFR.Add(TFltPr((float)index, FRank));
    fscanf(in7, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noMsgP.Add(TFltPr((float)index, p1));
    noMsgRR.Add(TFltPr((float)index, rRank));
    noMsgR.Add(TFltPr((float)index, Rank));
    noMsgRFR.Add(TFltPr((float)index, RFRank));
    noMsgFR.Add(TFltPr((float)index, FRank));
    fscanf(in8, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noConP.Add(TFltPr((float)index, p1));
    noConRR.Add(TFltPr((float)index, rRank));
    noConR.Add(TFltPr((float)index, Rank));
    noConRFR.Add(TFltPr((float)index, RFRank));
    noConFR.Add(TFltPr((float)index, FRank));
    fscanf(in9, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noTradeP.Add(TFltPr((float)index, p1));
    noTradeRR.Add(TFltPr((float)index, rRank));
    noTradeR.Add(TFltPr((float)index, Rank));
    noTradeRFR.Add(TFltPr((float)index, RFRank));
    noTradeFR.Add(TFltPr((float)index, FRank));
    fscanf(in10, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noDirectP.Add(TFltPr((float)index, p1));
    noDirectRR.Add(TFltPr((float)index, rRank));
    noDirectR.Add(TFltPr((float)index, Rank));
    noDirectRFR.Add(TFltPr((float)index, RFRank));
    noDirectFR.Add(TFltPr((float)index, FRank));
    fscanf(in11, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    noMutualP.Add(TFltPr((float)index, p1));
    noMutualRR.Add(TFltPr((float)index, rRank));
    noMutualR.Add(TFltPr((float)index, Rank));
    noMutualRFR.Add(TFltPr((float)index, RFRank));
    noMutualFR.Add(TFltPr((float)index, FRank));
    fscanf(in12, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    aggP.Add(TFltPr((float)index, p1));
    aggRR.Add(TFltPr((float)index, rRank));
    aggR.Add(TFltPr((float)index, Rank));
    aggRFR.Add(TFltPr((float)index, RFRank));
    aggFR.Add(TFltPr((float)index, FRank));
    fscanf(in13, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    msg2P.Add(TFltPr((float)index, p1));
    msg2RR.Add(TFltPr((float)index, rRank));
    msg2R.Add(TFltPr((float)index, Rank));
    msg2RFR.Add(TFltPr((float)index, RFRank));
    msg2FR.Add(TFltPr((float)index, FRank));
    fscanf(in14, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    conP.Add(TFltPr((float)index, p1));
    conRR.Add(TFltPr((float)index, rRank));
    conR.Add(TFltPr((float)index, Rank));
    conRFR.Add(TFltPr((float)index, RFRank));
    conFR.Add(TFltPr((float)index, FRank));
    fscanf(in15, "%d %f %f %f %f %f", &index, &p1, &rRank, &Rank, &RFRank, &FRank);
    tradeP.Add(TFltPr((float)index, p1));
    tradeRR.Add(TFltPr((float)index, rRank));
    tradeR.Add(TFltPr((float)index, Rank));
    tradeRFR.Add(TFltPr((float)index, RFRank));
    tradeFR.Add(TFltPr((float)index, FRank));
  }

  TStr origLabel = "SVM";
  TStr randLabel = "Random"; // 3 baselines
  TStr priceLabel = "MinPrice";
  TStr msgLabel = "MostMsg";
  TStr noGraphLabel = "Metadata"; // formerly no graph
  TStr noMetaLabel = "Graph"; // formerly no metada
  TStr noMsgLabel = "No Messages";
  TStr noConLabel = "No Contacts";
  TStr noTradeLabel = "No Trades";
  TStr noDirectLabel = "Indirect"; // no direct
  TStr noMutualLabel = "Direct"; // no mutuals/triads
  TStr aggLabel = "Aggregated";
  TStr msg2Label = "Messages";
  TStr conLabel = "Contacts";
  TStr tradeLabel = "Trades";

  // change all graphs to large font, no title, no gridlines

  int FSIZE = 25;
  TStr xLabel = "Sellers per Cluster";

  // IMPOSSIBLE TO SET LINE COLOR and TYPE SEPARATELY in GNUPLOT 4.0

  // 5 main lines
  TGnuPlot sm_pGnuPlot("predP1small", "", false); 
  sm_pGnuPlot.AddCmd("set key top right");
  sm_pGnuPlot.AddPlot(aggP, gpwLinesPoints, aggLabel.CStr(), "pt 5 ps 3");
  sm_pGnuPlot.AddPlot(origP, gpwLinesPoints, origLabel.CStr(), "pt 7 ps 3");
  sm_pGnuPlot.AddPlot(msgP, gpwLinesPoints, msgLabel.CStr(), "pt 9 ps 3");
  sm_pGnuPlot.AddPlot(randP, gpwLinesPoints, randLabel.CStr(), "pt 1 ps 3");
  sm_pGnuPlot.AddPlot(priceP, gpwLinesPoints, priceLabel.CStr(), "pt 2 ps 3");
  sm_pGnuPlot.SetXYLabel(xLabel.CStr(), "Precision at 1");
  sm_pGnuPlot.SetScale(gpsAuto);
  //sm_pGnuPlot.SavePng();
  sm_pGnuPlot.SetYRange(0,0.8);
  sm_pGnuPlot.SaveEps("predP1small.eps", FSIZE, ""); // new, can set font size

  // compare 3 networks
  // old ignore
  TGnuPlot tri_pGnuPlot("predP1tri", "", false);
  tri_pGnuPlot.AddPlot(noMsgP, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  tri_pGnuPlot.AddPlot(noConP, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  tri_pGnuPlot.AddPlot(noTradeP, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  tri_pGnuPlot.SetXYLabel(xLabel.CStr(), "Precision at 1");
  tri_pGnuPlot.SetScale(gpsAuto);
  //tri_pGnuPlot.SavePng();
  tri_pGnuPlot.SaveEps("predP1tri.eps", FSIZE, "");

  // 3 networks : new comparison
  TGnuPlot tri2_pGnuPlot("predP1tri2", "", false);
  tri2_pGnuPlot.AddCmd("set key top right");
  tri2_pGnuPlot.AddPlot(origP, gpwLinesPoints, origLabel.CStr(), "pt 5 ps 3");
  tri2_pGnuPlot.AddPlot(msg2P, gpwLinesPoints, msg2Label.CStr(), "pt 7 ps 3");
  tri2_pGnuPlot.AddPlot(tradeP, gpwLinesPoints, tradeLabel.CStr(), "pt 9 ps 3");
  tri2_pGnuPlot.AddPlot(conP, gpwLinesPoints, conLabel.CStr(), "pt 1 ps 3");
  tri2_pGnuPlot.SetXYLabel(xLabel.CStr(), "Precision at 1");
  tri2_pGnuPlot.SetScale(gpsAuto);
  //tri2_pGnuPlot.SavePng();
  tri2_pGnuPlot.SetYRange(0,0.8);
  tri2_pGnuPlot.SaveEps("predP1tri2.eps", FSIZE, "");

  // compare direct edge info vs mutual edge info
  TGnuPlot e_pGnuPlot("predP1edge", "", false);
  e_pGnuPlot.AddCmd("set key top right");
  e_pGnuPlot.AddPlot(origP, gpwLinesPoints, origLabel.CStr(), "pt 5 ps 3");
  e_pGnuPlot.AddPlot(noMutualP, gpwLinesPoints, noMutualLabel.CStr(), "pt 7 ps 3");
  e_pGnuPlot.AddPlot(noDirectP, gpwLinesPoints, noDirectLabel.CStr(), "pt 9 ps 3");
  e_pGnuPlot.SetXYLabel(xLabel.CStr(), "Precision at 1");
  e_pGnuPlot.SetScale(gpsAuto);
  //e_pGnuPlot.SavePng();
  e_pGnuPlot.SetYRange(0,0.8);
  e_pGnuPlot.SaveEps("predP1edge.eps", FSIZE, "");

  // compare graph info vs meta info
  TGnuPlot m_pGnuPlot("predP1meta", "", false);
  m_pGnuPlot.AddCmd("set key top right");
  m_pGnuPlot.AddPlot(origP, gpwLinesPoints, origLabel.CStr(), "pt 5 ps 3");
  m_pGnuPlot.AddPlot(noMetaP, gpwLinesPoints, noMetaLabel.CStr(), "pt 7 ps 3");
  m_pGnuPlot.AddPlot(noGraphP, gpwLinesPoints, noGraphLabel.CStr(), "pt 9 ps 3");
  m_pGnuPlot.SetXYLabel(xLabel.CStr(), "Precision at 1");
  m_pGnuPlot.SetScale(gpsAuto);
  //m_pGnuPlot.SavePng();
  m_pGnuPlot.SetYRange(0,0.8);
  m_pGnuPlot.SaveEps("predP1meta.eps", FSIZE, "");

  TGnuPlot pGnuPlot("predP1", "Precision at 1");
  pGnuPlot.AddPlot(origP, gpwLinesPoints, origLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(aggP, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(randP, gpwLinesPoints, randLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(priceP, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(msgP, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noGraphP, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noMetaP, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noMsgP, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noConP, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noTradeP, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noDirectP, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  pGnuPlot.AddPlot(noMutualP, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  pGnuPlot.SetXYLabel("Number of Sellers", "Precision at 1");
  pGnuPlot.SetScale(gpsAuto);
  pGnuPlot.SavePng();

  // 5 main lines
  TGnuPlot sm_rrGnuPlot("predRRsmall", "", false);
  sm_rrGnuPlot.AddCmd("set key top right");
  sm_rrGnuPlot.AddPlot(aggRR, gpwLinesPoints, aggLabel.CStr(), "pt 5 ps 3");
  sm_rrGnuPlot.AddPlot(origRR, gpwLinesPoints, origLabel.CStr(), "pt 7 ps 3");
  sm_rrGnuPlot.AddPlot(msgRR, gpwLinesPoints, msgLabel.CStr(), "pt 9 ps 3");
  sm_rrGnuPlot.AddPlot(randRR, gpwLinesPoints, randLabel.CStr(), "pt 1 ps 3");
  sm_rrGnuPlot.AddPlot(priceRR, gpwLinesPoints, priceLabel.CStr(), "pt 2 ps 3");
  sm_rrGnuPlot.SetXYLabel(xLabel.CStr(), "Mean Reciprocal Rank");
  sm_rrGnuPlot.SetScale(gpsAuto);
  //sm_rrGnuPlot.SavePng();
  sm_rrGnuPlot.SetYRange(0.2,0.9);
  sm_rrGnuPlot.SaveEps("predRRsmall.eps", FSIZE, "");

  // compare 3 networks
  // old, ignore
  TGnuPlot tri_rrGnuPlot("predRRtri", "", false);
  tri_rrGnuPlot.AddPlot(noMsgRR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  tri_rrGnuPlot.AddPlot(noConRR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  tri_rrGnuPlot.AddPlot(noTradeRR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  tri_rrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Rank");
  tri_rrGnuPlot.SetScale(gpsAuto);
  //tri_rrGnuPlot.SavePng();
    tri_rrGnuPlot.SaveEps("predRRtri.eps", FSIZE, "");

  // NEW: compare 3 networks
  TGnuPlot tri2_rrGnuPlot("predRRtri2", "", false);
  tri2_rrGnuPlot.AddCmd("set key top right");
  tri2_rrGnuPlot.AddPlot(origRR, gpwLinesPoints, origLabel.CStr(), "pt 5 ps 3");
  tri2_rrGnuPlot.AddPlot(msg2RR, gpwLinesPoints, msg2Label.CStr(), "pt 7 ps 3");
  tri2_rrGnuPlot.AddPlot(tradeRR, gpwLinesPoints, tradeLabel.CStr(), "pt 9 ps 3");
  tri2_rrGnuPlot.AddPlot(conRR, gpwLinesPoints, conLabel.CStr(), "pt 1 ps 3");
  tri2_rrGnuPlot.SetXYLabel(xLabel.CStr(), "Mean Reciprocal Rank");
  tri2_rrGnuPlot.SetScale(gpsAuto);
  tri2_rrGnuPlot.SetYRange(0.2,0.9);
  tri2_rrGnuPlot.SaveEps("predRRtri2.eps", FSIZE, "");

  // compare direct edge info vs mutual edge info
  TGnuPlot e_rrGnuPlot("predRRedge", "", false);
  e_rrGnuPlot.AddCmd("set key top right");
  e_rrGnuPlot.AddPlot(origRR, gpwLinesPoints, origLabel.CStr(), "pt 5 ps 3");
  e_rrGnuPlot.AddPlot(noMutualRR, gpwLinesPoints, noMutualLabel.CStr(), "pt 7 ps 3");
  e_rrGnuPlot.AddPlot(noDirectRR, gpwLinesPoints, noDirectLabel.CStr(), "pt 9 ps 3");
  e_rrGnuPlot.SetXYLabel(xLabel.CStr(), "Mean Reciprocal Rank");
  e_rrGnuPlot.SetScale(gpsAuto);
  //e_rrGnuPlot.SavePng();
  e_rrGnuPlot.SetYRange(0.2,0.9);
  e_rrGnuPlot.SaveEps("predRRedge.eps", FSIZE, "");

  // compare graph info vs meta info
  TGnuPlot m_rrGnuPlot("predRRmeta", "", false);
  m_rrGnuPlot.AddCmd("set key top right");
  m_rrGnuPlot.AddPlot(origRR, gpwLinesPoints, origLabel.CStr(), "pt 5 ps 3");
  m_rrGnuPlot.AddPlot(noMetaRR, gpwLinesPoints, noMetaLabel.CStr(), "pt 7 ps 3");
  m_rrGnuPlot.AddPlot(noGraphRR, gpwLinesPoints, noGraphLabel.CStr(), "pt 9 ps 3");
  m_rrGnuPlot.SetXYLabel(xLabel.CStr(), "Mean Reciprocal Rank");
  m_rrGnuPlot.SetScale(gpsAuto);
  //m_rrGnuPlot.SavePng();
  m_rrGnuPlot.SetYRange(0.2,0.9);
  m_rrGnuPlot.SaveEps("predRRmeta.eps", FSIZE, "");

  TGnuPlot rrGnuPlot("predRR", "Mean Reciprocal Rank");
  rrGnuPlot.AddPlot(origRR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(aggRR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(randRR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(priceRR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(msgRR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noGraphRR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noMetaRR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noMsgRR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noConRR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noTradeRR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noDirectRR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  rrGnuPlot.AddPlot(noMutualRR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  rrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Rank");
  rrGnuPlot.SetScale(gpsAuto);
  rrGnuPlot.SavePng();

  // 5 main lines
  TGnuPlot sm_rGnuPlot("predRsmall", "Mean Rank");
  sm_rGnuPlot.AddPlot(origR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  sm_rGnuPlot.AddPlot(aggR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  sm_rGnuPlot.AddPlot(randR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  sm_rGnuPlot.AddPlot(priceR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  sm_rGnuPlot.AddPlot(msgR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  sm_rGnuPlot.SetXYLabel("Number of Sellers", "Mean Rank");
  sm_rGnuPlot.SetScale(gpsAuto);
  sm_rGnuPlot.SavePng();

  // compare 3 networks
  TGnuPlot tri_rGnuPlot("predRtri", "Mean Rank");
  tri_rGnuPlot.AddPlot(noMsgR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  tri_rGnuPlot.AddPlot(noConR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  tri_rGnuPlot.AddPlot(noTradeR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  tri_rGnuPlot.SetXYLabel("Number of Sellers", "Mean Rank");
  tri_rGnuPlot.SetScale(gpsAuto);
  tri_rGnuPlot.SavePng();

  // compare direct edge info vs mutual edge info
  TGnuPlot e_rGnuPlot("predRedge", "Mean Rank");
  e_rGnuPlot.AddPlot(noDirectR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  e_rGnuPlot.AddPlot(noMutualR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  e_rGnuPlot.SetXYLabel("Number of Sellers", "Mean Rank");
  e_rGnuPlot.SetScale(gpsAuto);
  e_rGnuPlot.SavePng();

  // compare graph info vs meta info
  TGnuPlot m_rGnuPlot("predRmeta", "Mean Rank");
  m_rGnuPlot.AddPlot(noGraphR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  m_rGnuPlot.AddPlot(noMetaR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  m_rGnuPlot.SetXYLabel("Number of Sellers", "Mean Rank");
  m_rGnuPlot.SetScale(gpsAuto);
  m_rGnuPlot.SavePng();
  
  TGnuPlot rGnuPlot("predR", "Mean Rank");
  rGnuPlot.AddPlot(origR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(aggR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(randR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(priceR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(msgR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noGraphR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noMetaR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noMsgR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noConR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noTradeR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noDirectR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  rGnuPlot.AddPlot(noMutualR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  rGnuPlot.SetXYLabel("Number of Sellers", "Mean Rank");
  rGnuPlot.SetScale(gpsAuto);
  rGnuPlot.SavePng();

  // 5 main lines
  TGnuPlot sm_rfrGnuPlot("predRFRsmall", "Mean Reciprocal Fractional Rank");
  sm_rfrGnuPlot.AddPlot(origRFR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  sm_rfrGnuPlot.AddPlot(aggRFR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  sm_rfrGnuPlot.AddPlot(randRFR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  sm_rfrGnuPlot.AddPlot(priceRFR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  sm_rfrGnuPlot.AddPlot(msgRFR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  sm_rfrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Fractional Rank");
  sm_rfrGnuPlot.SetScale(gpsAuto);
  sm_rfrGnuPlot.SavePng();

  // compare 3 networks
  TGnuPlot tri_rfrGnuPlot("predRFRtri", "Mean Reciprocal Fractioal Rank");
  tri_rfrGnuPlot.AddPlot(noMsgRFR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  tri_rfrGnuPlot.AddPlot(noConRFR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  tri_rfrGnuPlot.AddPlot(noTradeRFR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  tri_rfrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Fractional Rank");
  tri_rfrGnuPlot.SetScale(gpsAuto);
  tri_rfrGnuPlot.SavePng();

  // compare direct edge info vs mutual edge info
  TGnuPlot e_rfrGnuPlot("predRFRedge", "Mean Reciprocal Fractional Rank");
  e_rfrGnuPlot.AddPlot(noDirectRFR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  e_rfrGnuPlot.AddPlot(noMutualRFR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  e_rfrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Fractional Rank");
  e_rfrGnuPlot.SetScale(gpsAuto);
  e_rfrGnuPlot.SavePng();

  // compare graph info vs meta info
  TGnuPlot m_rfrGnuPlot("predRFRmeta", "Mean Reciprocal Fractional Rank");
  m_rfrGnuPlot.AddPlot(noGraphRFR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  m_rfrGnuPlot.AddPlot(noMetaRFR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  m_rfrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Fractional Rank");
  m_rfrGnuPlot.SetScale(gpsAuto);
  m_rfrGnuPlot.SavePng();

  TGnuPlot rfrGnuPlot("predRFR", "Mean Reciprocal Fractional Rank");
  rfrGnuPlot.AddPlot(origRFR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(aggRFR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(randRFR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(priceRFR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(msgRFR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noGraphRFR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noMetaRFR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noMsgRFR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noConRFR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noTradeRFR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noDirectRFR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  rfrGnuPlot.AddPlot(noMutualRFR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  rfrGnuPlot.SetXYLabel("Number of Sellers", "Mean Reciprocal Fractional Rank");
  rfrGnuPlot.SetScale(gpsAuto);
  rfrGnuPlot.SavePng();

  // 5 main lines
  TGnuPlot sm_frGnuPlot("predFRsmall", "Mean Fractional Rank");
  sm_frGnuPlot.AddPlot(origFR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  sm_frGnuPlot.AddPlot(aggFR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  sm_frGnuPlot.AddPlot(randFR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  sm_frGnuPlot.AddPlot(priceFR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  sm_frGnuPlot.AddPlot(msgFR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  sm_frGnuPlot.SetXYLabel("Number of Sellers", "Mean Fractional Rank");
  sm_frGnuPlot.SetScale(gpsAuto);
  sm_frGnuPlot.SavePng();

  // compare 3 networks
  TGnuPlot tri_frGnuPlot("predFRtri", "Mean Fractional Rank");
  tri_frGnuPlot.AddPlot(noMsgFR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  tri_frGnuPlot.AddPlot(noConFR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  tri_frGnuPlot.AddPlot(noTradeFR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  tri_frGnuPlot.SetXYLabel("Number of Sellers", "Mean Fractional Rank");
  tri_frGnuPlot.SetScale(gpsAuto);
  tri_frGnuPlot.SavePng();

  // compare direct edge info vs mutual edge info
  TGnuPlot e_frGnuPlot("predFRedge", "Mean Fractional Rank");
  e_frGnuPlot.AddPlot(noDirectFR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  e_frGnuPlot.AddPlot(noMutualFR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  e_frGnuPlot.SetXYLabel("Number of Sellers", "Mean Fractional Rank");
  e_frGnuPlot.SetScale(gpsAuto);
  e_frGnuPlot.SavePng();

  // compare graph info vs meta info
  TGnuPlot m_frGnuPlot("predFRmeta", "Mean Fractional Rank");
  m_frGnuPlot.AddPlot(noGraphFR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  m_frGnuPlot.AddPlot(noMetaFR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  m_frGnuPlot.SetXYLabel("Number of Sellers", "Mean Fractional Rank");
  m_frGnuPlot.SetScale(gpsAuto);
  m_frGnuPlot.SavePng();

  TGnuPlot frGnuPlot("predFR", "Mean Fractional Rank");
  frGnuPlot.AddPlot(origFR, gpwLinesPoints, origLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(aggFR, gpwLinesPoints, aggLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(randFR, gpwLinesPoints, randLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(priceFR, gpwLinesPoints, priceLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(msgFR, gpwLinesPoints, msgLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noGraphFR, gpwLinesPoints, noGraphLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noMetaFR, gpwLinesPoints, noMetaLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noMsgFR, gpwLinesPoints, noMsgLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noConFR, gpwLinesPoints, noConLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noTradeFR, gpwLinesPoints, noTradeLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noDirectFR, gpwLinesPoints, noDirectLabel.CStr(), "pt 6");
  frGnuPlot.AddPlot(noMutualFR, gpwLinesPoints, noMutualLabel.CStr(), "pt 6");
  frGnuPlot.SetXYLabel("Number of Sellers", "Mean Fractional Rank");
  frGnuPlot.SetScale(gpsAuto);
  frGnuPlot.SavePng();
  
  fclose(in1);
  fclose(in2);
  fclose(in3);
  fclose(in4);
  fclose(in5);
  fclose(in6);
  fclose(in7);
  fclose(in8);
  fclose(in9);
  fclose(in10);
  fclose(in11);  
  fclose(in12);
  fclose(in13);
  fclose(in14);
  fclose(in15);
}

// run this code twice, once for train, once for test
// be sure to comment and uncomment file open/close
int main(int argc, char* argv[]) {
  
  plotStuff(); // temp
  return 0;
  
  FILE *in = fopen(argv[1], "r");
  FILE *out = fopen(argv[2], "w");
  // INPUT IS RAW FILE

  int prevCluster=0;
  int cluster;
  //int skip = 5; // SAMPLE 1/3 of DATA. 
  int clustCnt=0;

  int match;
  int catIndex;

  FILE *trainFiles[100];
  FILE *testFiles[100];

  // ints 3, 18, 19, 20, 21, 38
  int f3, f18, f19, f20, f21, f38;
  float f1, f2, f4, f5, f6, f7, f8, f9, f10;
  float f11, f12, f13, f14, f15, f16, f17;
  float f22, f23, f24, f25, f26, f27, f28, f29, f30;
  float f31, f32, f33, f34, f35, f36, f37, f39, f40;
  float f41, f42, f43, f44, f45, f46, f47, f48, f49, f50;
  float f51, f52, f53, f54, f55, f56, f57, f58, f59, f60;
  float f61, f62, f63, f64, f65, f66, f67, f68, f69, f70;

  char line[100000];
  // UNCOMMENT THIS BLOCK IF OUTPUTTING FOR CATS
  /*
  TStrV catFields;
  FILE *inCat = fopen("catProd.out", "r");
  //FILE *outCat =fopen("catOut.out", "w");
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

    //catMap.AddDat(catFields[0], catNum);
    //fprintf(outCat, "%s %d\n", catFields[0].CStr(), catNum);
    // NEW: move training/testing files to lfs
    TStr newTrainFile =
      TStr::Fmt("/lfs/1/tmp/mengqiu/svmRank/tests/cats/train%d.txt", catNum);
    //trainFiles[catNum] = fopen(newTrainFile.CStr(), "w");
    TStr newTestFile = TStr::Fmt("/lfs/1/tmp/mengqiu/svmRank/tests/cats/test%d.txt", catNum);
    //testFiles[catNum] = fopen(newTestFile.CStr(), "w");
    //printf("%s %d\t", catTemp, catNum);
    catNum++;
  }
  fclose(inCat);
  //fclose(outCat);
  //return; // debug
  */

  while(!feof(in)) {
    memset(line, 0, 2024);
    fgets(line, 2024, in);
    if (strlen(line) == 0)
      break;

    sscanf(line, "%d %d %d %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", 
           &match, &cluster, &catIndex, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8,
           &f9, &f10, &f11, &f12, &f13, &f14, &f15, &f16, &f17, &f18, &f19,
           &f20, &f21, &f22, &f23, &f24, &f25, &f26, &f27, &f28, &f29, &f30,
           &f31, &f32, &f33, &f34, &f35, &f36, &f37, &f38, &f39, &f40, &f41,
           &f42, &f43, &f44, &f45, &f46, &f47, &f48, &f49, &f50, &f51, &f52,
           &f53, &f54, &f55, &f56, &f57, &f58, &f59, &f60, &f61, &f62, &f63,
           &f64, &f65, &f66, &f67, &f68, &f69, &f70);
    // test set A
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%d 16:%f 17:%f 18:%f 19:%f 20:%f 21:%f 22:%f 23:%f 24:%f 25:%f 26:%f 27:%f 28:%f 29:%f 30:%f 31:%f 32:%f\n", 
            match, cluster, f3, f4, f6, f7, f8, f10, f12, f28, f31, f33, f34, f35, f36, f37, f38, f39, f42,
            f44, f48, f49, f50, f53, f54, f57, f58, f59, f60, f61, f62, f66, f67, f68);
    */

    // test set B
    /*
    fprintf(trainFiles[catIndex], "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%d 14:%f 15:%f 16:%f 17:%f 18:%f 19:%f 20:%f 21:%f 22:%f 23:%f\n",
            match, cluster, f3, f4, f6, f7, f8, f10, f12, f28, f31, f33, f34,
            f35, f38, f39, f42, f44, f57, f58, f60, f62, f66, f67, f68);
    */

    /*
    fprintf(testFiles[catIndex], "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%d 14:%f 15:%f 16:%f 17:%f 18:%f 19:%f 20:%f 21:%f 22:%f 23:%f\n",
            match, cluster, f3, f4, f6, f7, f8, f10, f12, f28, f31, f33, f34,
            f35, f38, f39, f42, f44, f57, f58, f60, f62, f66, f67, f68);
    */

    // test set B NOMETA 
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f\n",
            match, cluster, f3, f4, f6, f7, f8, f10, f12, f28, f31, f35, f57, f58, f60, f62, f66, f67, f68);
    */

    // test set B NOGRAPH
    /*
    fprintf(out, "%d qid:%d 1:%f 2:%f 3:%d 4:%f 5:%f 6:%f\n", match, cluster, f33, f34, f38, f39, f42, f44);
    */

    // OCT 10: redo nomutual (direct) and nodirect (mutual)
    // new direct
    //fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%d 12:%f 13:%f 14:%f\n",
    //        match, cluster, f3, f4, f6, f10, f12, f28, f31, f33, f34, f35, f38, f39, f42, f44);

    // test set B NOMUTUAL
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%d 12:%f 13:%f 14:%f\n",
            match, cluster, f3, f4, f6, f10, f12, f28, f31, f33, f34, f35, f38, f39, f42, f44);
    */
      
    // new mutual
    fprintf(out, "%d qid:%d 1:%f 2:%f 3:%f 4:%f 5:%d 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f\n", 
            match, cluster, f7, f8, f33, f34, f38, f39, f42, f44, f57, f58, f60, f62, f66, f67, f68);

    // test set B NODIRECT
    /*
    fprintf(out, "%d qid:%d 1:%f 2:%f 3:%f 4:%f 5:%d 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f\n",
            match, cluster, f7, f8, f33, f34, f38, f39, f42, f44, f57, f58, f60, f62, f66, f67, f68);
    */

    // test set B NOMSG
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%d 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f\n",
            match, cluster, f3, f6, f7, f10, f12, f28, f33, f34, f38, f39, f42, f44, f57, f60, f66, f68);
    */

    // NEW: test set B MSG
    // 4, 8, 31, 33, 34, 35, 38, 39, 42, 44, 58, 62, 67
    /*
    fprintf(out, "%d qid:%d 1:%f 2:%f 3:%f 4:%f 5:%f 6:%f 7:%d 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f\n",
            match, cluster, f4, f8, f31, f33, f34, f35, f38, f39, f42, f44, f58, f62, f67);
    */

    // test set B NOCON
    /*
    fprintf(out, "%d qid:%d 1:%f 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%d 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%f\n",
            match, cluster, f4, f6, f8, f10, f12, f28, f31, f33, f34, f35, f38, f39, f42, f44, f58, f62, f66, f67);
    */
    
    // NEW: test set B CON
    // 3, 7, 33, 34, 38, 39, 42, 44, 57, 60, 68
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%d 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f\n",
            match, cluster, f3, f7, f33, f34, f38, f39, f42, f44, f57, f60, f68);
    */

    // test set B NOTRADE
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%d 10:%f 11:%f 12:%f 13:%f 14:%f 15:%f 16:%f 17:%f 18:%f\n",
            match, cluster, f3, f4, f7, f8, f31, f33, f34, f35, f38, f39, f42, f44, f57, f58, f60, f62, f67, f68);
    */

    // NEW: test set B TRADE
    // 6, 10, 12, 28, 33, 34, 38, 39, 42, 44, 66
    
    // fprintf(out, "%d qid:%d 1:%f 2:%f 3:%f 4:%f 5:%f 6:%f 7:%d 8:%f 9:%f 10:%f 11:%f\n",
    //       match, cluster, f6, f10, f12, f28, f33, f34, f38, f39, f42, f44, f66);
    

    // old 14 feature set to experiment on
    /* 
                 fprintf(trainRankB, "1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f
    9:%f 10:%f 11:%f 12:%f 13:%f 14:%f\n", isContact 3, msgVol 4+5,
                 tradeVol 6,
                          mutualCon 7, mutualMsg 8, sellerDeg 11, sellerVol 12,
                  invPriceRank 13, invRatingRank 14, invMsgRank 15, dayST2 28,
    daySM2 31, priceDif 1, ratingDif 2);
     */
    /*
    fprintf(out, "%d qid:%d 1:%d 2:%f 3:%f 4:%f 5:%f 6:%f 7:%f 8:%f 9:%f 10:%f 11:%f 12:%f 13:%f 14:%f\n", 
            match, cluster, f3, f4+f5, f6, f7, f8, f11, f12, f13, f14, f15, f28, f31, f1, f2);
    */
            
            //sscanf(line, "%d qid:%d 1:%f 2:%d 3:%f 4:%f 5:%f 6:%f 7:%f", &match, &cluster, &f1, &f2, &f3, &f4, &f5, &f6, &f7); 
    if (cluster!=prevCluster)
      clustCnt++;
    prevCluster = cluster;
    
    //if(clustCnt%skip == 0)
    // fprintf(out, "%s", line); 
  }
 
  fclose(in);
  fclose(out);
}
