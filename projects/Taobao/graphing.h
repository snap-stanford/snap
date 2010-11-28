#ifndef snap_graphing_h
#define snap_graphing_h
#include "Snap.h"
#include "taobaonet.h"
#include <stdio.h>

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
  bin[0] = 0.0; // default 0
  if (logScale) // log(0) complains
    bin[0] = 1.0;
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
  // apr10: changed numBins to numBins-1, ignore last partial bin
  for (int i = 0; i < numBins-1; i++) { // default numBins-1
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
      printf("%d %f %d %f %f\n", i, bin[i], binCnt[i], (float) binTotal[i],
  (float)binTotal[i]/(float)(binCnt[i]));

  TGnuPlot ScGnuPlot(graphName, title, false); // added false, no grid
  ScGnuPlot.AddPlot(output, gpwLinesPoints, "", "lt 1 pt 5 ps 3"); // removed smooth bezier
  //ScGnuPlot.AddPlot(output, gpwPoints, "", "lt 1 pt 6"); // added lt1 to forcecolor
  // original apr10: below
  //ScGnuPlot.AddPlot(output, gpwLinesPoints, "", "pt 6");
  ScGnuPlot.SetXYLabel(xAxis, yAxis);
  if (logScale)
    ScGnuPlot.SetScale(gpsLog10X); // default gpsLog10XY
  else
    ScGnuPlot.SetScale(gpsAuto);
  //ScGnuPlot.SavePng(); // default
  //ScGnuPlot.SaveEps("binned_msgPrice3.eps", 30, ""); // force large font
}

// new, for aggregating data with probs on y axis 
// data is of form: x, success Y, total Y
void binProb(TFltTrV& data, char *graphName, float initialWidth, float
             binWidthFactor, bool logScale, char *title,
             char *xAxis, char *yAxis, TFltPrV& output) {
  // represent x axis point as a single node
  // y axis value will be average of y's in the bin

  int MAXBINS = 100000;
  float bin[MAXBINS]; // bin[i] stores position where bin begins
  float width[MAXBINS]; // width of each bin (for convenience)
  int binCnt[MAXBINS]; // count of num x values in the bin
  // binCnt is not really that useful in our case
  //float binTotal[MAXBINS]; // sum of y-values in bin
  float binYSuc[MAXBINS];
  float binYTot[MAXBINS];

  for (int i=0; i<MAXBINS; i++) {
    binCnt[i] = 0;
    binYSuc[i] = 0.0;
    binYTot[i] = 0.0;
    //binTotal[i] = 0.0;
  }

  int numBins = 1;
  bin[0] = 0.0;
  if (logScale) // log(0) complains
    bin[0] = 1.0;
  width[0] = initialWidth;

  char buf[1024];

  float x;
  float ySuc;
  float yTot;
  for (int i=0; i<data.Len(); i++){
    //sscanf(buf, "%f %d %d", &price, &yes, &no);
    x = data[i].Val1;
    ySuc = data[i].Val2;
    yTot = data[i].Val3;
    printf("%f %f %f\n", x, ySuc, yTot);
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
    binYSuc[pos] += ySuc;
    binYTot[pos] += yTot;
    //binTotal[pos] += y;
  }

  //TFltPrV tempV;
  //tempV.Gen(numBins, 0);
  for (int i = 0; i < numBins-1; i++) { // changed to numBins-1. ignore partial bin
    //printf("%d %f\n", bin[i], (float)binYes[i]/(float)(binNo[i]+binYes[i]));

    if (binCnt[i]==0) {
      if (i==0) // NEW JAN - don't show 0's due to data sparsity
        output.Add(TFltPr((float)bin[i], 0.0));
    }
    else
      output.Add(TFltPr((float)bin[i], binYSuc[i]/binYTot[i]));
  }

  printf("numBins %d\n", numBins);
  for (int i=0; i<20 && i<numBins; i++)
    printf("%d %f %d %f %f\n", i, bin[i], binCnt[i], binYTot[i], binYSuc[i]/binYTot[i]);

  // oct10: removed grid lines, forced same color
  TGnuPlot ScGnuPlot(graphName, title, false);
  // first line smooth bezier, 
  ScGnuPlot.AddPlot(output, gpwLinesPoints, "", "lt 1 pt 5 ps 3"); // ps 3 temp
  //ScGnuPlot.AddPlot(output, gpwPoints, "", "lt 1 pt 6");
  ScGnuPlot.SetXYLabel(xAxis, yAxis);
  if (logScale)
    ScGnuPlot.SetScale(gpsLog10X);
  else
    ScGnuPlot.SetScale(gpsAuto);
  ScGnuPlot.SavePng(); // default
  //ScGnuPlot.SetXRange(1, 513);
  //ScGnuPlot.SaveEps("binned_priceProb3.eps", 30, "");
}




#endif
