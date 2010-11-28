#include "stdafx.h"
#include "gnuplot.h"

/////////////////////////////////////////////////
// GNU-Plot-Chart
/*#ifdef GLib_WIN
  TStr TGnuPlot::GnuPlotPath = "\\Dev\\GLib\\gnuplot\\bin\\";
#else
  TStr TGnuPlot::GnuPlotPath = "/usr/bin/";
#endif*/

TStr TGnuPlot::DefPlotFNm = "GnuPlot.plt";
TStr TGnuPlot::DefDataFNm = "GnuPlot.tab";

TGnuPlot::TGpSeries::TGpSeries(const TGnuPlot::TGpSeries& Gps) :
  SeriesTy(Gps.SeriesTy), XYValV(Gps.XYValV), ZValV(Gps.ZValV),
  Label(Gps.Label), WithStyle(Gps.WithStyle), DataFNm(Gps.DataFNm),
  XCol(Gps.XCol), YCol(Gps.YCol), ZCol(Gps.ZCol) {
}

TGnuPlot::TGpSeries& TGnuPlot::TGpSeries::operator = (const TGnuPlot::TGpSeries& Gps) {
  if(this != &Gps) {
    SeriesTy = Gps.SeriesTy;
    XYValV = Gps.XYValV;  ZValV = Gps.ZValV;
    Label = Gps.Label;
    DataFNm = Gps.DataFNm;
    WithStyle = Gps.WithStyle;
    XCol = Gps.XCol;  YCol = Gps.YCol;  ZCol = Gps.ZCol;
  }
  return *this;
}

bool TGnuPlot::TGpSeries::operator < (const TGpSeries& Gps) const {
  return XYValV < Gps.XYValV || (XYValV == Gps.XYValV) && Label < Gps.Label;
}

TGnuPlot::TGnuPlot(const TStr& FileNm, const TStr& PlotTitle, const bool& Grid) :
 DataFNm(FileNm+".tab"), PlotFNm(FileNm+".plt"), Title(PlotTitle), LblX(), LblY(), ScaleTy(gpsAuto),
 YRange(0, 0), XRange(0, 0), SetGrid(Grid), SetPause(true),
 SeriesV(), MoreCmds() {
  IAssert(! FileNm.Empty());
}

TGnuPlot::TGnuPlot(const TStr& DataFileNm, const TStr& PlotFileNm, const TStr& PlotTitle, const bool& Grid) :
  DataFNm(DataFileNm.Empty() ? DefDataFNm : DataFileNm),
  PlotFNm(PlotFileNm.Empty() ? DefPlotFNm : PlotFileNm),
  Title(PlotTitle), LblX(), LblY(), ScaleTy(gpsAuto),
  YRange(0, 0), XRange(0, 0), SetGrid(Grid), SetPause(true), SeriesV(), MoreCmds() {
}

TGnuPlot::TGnuPlot(const TGnuPlot& GnuPlot) : DataFNm(GnuPlot.DataFNm), PlotFNm(GnuPlot.PlotFNm), 
  Title(GnuPlot.Title), LblX(GnuPlot.LblX), LblY(GnuPlot.LblY), ScaleTy(GnuPlot.ScaleTy), YRange(GnuPlot.YRange),
  XRange(GnuPlot.XRange), SetGrid(GnuPlot.SetGrid), SetPause(GnuPlot.SetPause), SeriesV(GnuPlot.SeriesV),
  MoreCmds(GnuPlot.MoreCmds) {
}

TGnuPlot& TGnuPlot::operator = (const TGnuPlot& GnuPlot) {
  if (this != &GnuPlot) {
    DataFNm = GnuPlot.DataFNm;
    PlotFNm = GnuPlot.PlotFNm;
    Title = GnuPlot.Title;
    LblX = GnuPlot.LblX;
    LblY = GnuPlot.LblY;
    ScaleTy = GnuPlot.ScaleTy;
    YRange = GnuPlot.YRange;
    XRange = GnuPlot.XRange;
    SetGrid = GnuPlot.SetGrid;
    SetPause = GnuPlot.SetPause;
    SeriesV = GnuPlot.SeriesV;
    MoreCmds = GnuPlot.MoreCmds;
  }
  return *this;
}

TStr TGnuPlot::GetSeriesPlotStr(const int& SeriesId) {
  TChA PlotStr;
  TGpSeries& Series = SeriesV[SeriesId];
  if (SeriesId != 0) PlotStr += ",\\\n\t";
  if (Series.XCol >= 0) {
    PlotStr += "\"" + Series.DataFNm + "\" using " + TInt::GetStr(Series.XCol);
    if (Series.YCol != 0) { PlotStr += ":" + TInt::GetStr(Series.YCol); }
    if (Series.ZCol != 0) { PlotStr += ":" + TInt::GetStr(Series.ZCol); }
    else if (Series.SeriesTy==gpwFilledCurves) { PlotStr += ":(0)"; } // filled curves requres 3rd column
  } else {
    // function
    //IAssertR(Series.DataFNm.SearchCh('=') != -1, TStr::Fmt("Expression %s is not a function", Series.DataFNm.CStr()));
    PlotStr += Series.DataFNm;
  }
  PlotStr += " title \"" + Series.Label + "\"";
  // hard coded line style
  if (Series.WithStyle.Empty()) {
    if (Series.SeriesTy == gpwLines) Series.WithStyle = "lw 1";
    //if (Series.SeriesTy == gpwPoints) Series.WithStyle = "pt 6 ps 1 lw 1"; // circles
    //if (Series.SeriesTy == gpwLinesPoints) Series.WithStyle = "pt 6 ps 1 lw 1"; // circles
    if (Series.SeriesTy == gpwBoxes) Series.WithStyle = "fill solid 0.3";
  }
  PlotStr += " with " + GetSeriesTyStr(Series.SeriesTy) + " " + Series.WithStyle;
  return PlotStr;
}

//GP.AddFunc("2*x**-2+4", gpwLines, "2*x^-2+4");
int TGnuPlot::AddFunc(const TStr& FuncStr, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  const int Id = SeriesV.Len();
  TGpSeries Plot;
  Plot.SeriesTy = SeriesTy;
  Plot.Label = Label;
  if (! FuncStr.Empty()) { Plot.DataFNm = TStr::Fmt("f%d(x)=%s, f%d(x)", Id, FuncStr.CStr(), Id); }
  else { Plot.DataFNm = TStr::Fmt("f%d(x)", Id); }
  Plot.XCol = -1;
  Plot.WithStyle = Style;
  SeriesV.Add(Plot);
  return Id;
}

int TGnuPlot::AddPlot(const TStr& DataFNm, const int& ColY,
                      const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  return AddPlot(DataFNm, 0, ColY, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TStr& DataFNm, const int& ColX, const int& ColY,
                      const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  IAssert(ColY > 0);  IAssert(ColX >= 0);
  TGpSeries Plot;
  Plot.SeriesTy = SeriesTy;
  Plot.Label = Label;
  Plot.DataFNm = DataFNm;  Plot.DataFNm.ChangeStrAll("\\", "\\\\");
  Plot.XCol = ColX;  Plot.YCol = ColY;  Plot.ZCol = 0;
  Plot.WithStyle = Style;
  SeriesV.Add(Plot);
  return SeriesV.Len() - 1;
}

int TGnuPlot::AddPlot(const TIntV& YValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYValV(YValV.Len(), 0);
  for (int i = 0; i < YValV.Len(); i++) {
    XYValV.Add(TFltKd(TFlt(i+1), TFlt(YValV[i])));
  }
  return AddPlot(XYValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TFltV& YValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYValV(YValV.Len(), 0);
  for (int i = 0; i < YValV.Len(); i++) {
    XYValV.Add(TFltKd(TFlt(i+1), TFlt(YValV[i])));
  }
  return AddPlot(XYValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TFltV& XValV, const TFltV& YValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  IAssert(XValV.Len() == YValV.Len());
  TFltKdV XYValV(XValV.Len(), 0);
  for (int i = 0; i < YValV.Len(); i++) {
    XYValV.Add(TFltKd(TFlt(XValV[i]), TFlt(YValV[i])));
  }
  return AddPlot(XYValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TIntPrV& XYValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(TFlt(XYValV[i].Val1), TFlt(XYValV[i].Val2)));
  }
  return AddPlot(XYFltValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TFltPrV& XYValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(XYValV[i].Val1, XYValV[i].Val2));
  }
  return AddPlot(XYFltValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TIntKdV& XYValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(TFlt(XYValV[i].Key), TFlt(XYValV[i].Dat)));
  }
  return AddPlot(XYFltValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TIntFltKdV& XYValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(TFlt(XYValV[i].Key), TFlt(XYValV[i].Dat)));
  }
  return AddPlot(XYFltValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TIntFltPrV& XYValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(TFlt(XYValV[i].Val1), TFlt(XYValV[i].Val2)));
  }
  return AddPlot(XYFltValV, SeriesTy, Label, Style);
}

int TGnuPlot::AddPlot(const TFltKdV& XYValV, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style) {
  if (XYValV.Empty()) {
    printf("***AddPlot: empty plot (%s) %s\n", DataFNm.CStr(), Title.CStr());
    return -1;
  }
  TGpSeries Plot;
  Plot.SeriesTy = SeriesTy;
  Plot.Label = Label;
  Plot.XYValV = XYValV;
  Plot.WithStyle = Style;
  SeriesV.Add(Plot);
  return SeriesV.Len() - 1;
}

int TGnuPlot::AddErrBar(const TFltTrV& XYDValV, const TStr& Label) {
  TFltKdV XYFltValV(XYDValV.Len(), 0);
  TFltV DeltaV(XYDValV.Len(), 0);
  for (int i = 0; i < XYDValV.Len(); i++) {
    XYFltValV.Add(TFltKd(XYDValV[i].Val1, XYDValV[i].Val2));
    DeltaV.Add(XYDValV[i].Val3);
  }
  return AddErrBar(XYFltValV, DeltaV, Label);
}

int TGnuPlot::AddErrBar(const TFltTrV& XYDValV, const TStr& DatLabel, const TStr& ErrLabel) {
  TFltKdV XYFltValV(XYDValV.Len(), 0);
  TFltV DeltaV(XYDValV.Len(), 0);
  for (int i = 0; i < XYDValV.Len(); i++) {
    XYFltValV.Add(TFltKd(XYDValV[i].Val1, XYDValV[i].Val2));
    DeltaV.Add(XYDValV[i].Val3);
  }
  const int PlotId = AddPlot(XYFltValV, gpwLinesPoints, DatLabel);
  AddErrBar(XYFltValV, DeltaV, ErrLabel);
  return PlotId;
}

int TGnuPlot::AddErrBar(const TFltV& YValV, const TFltV& DeltaYV, const TStr& Label) {
  IAssert(YValV.Len() == DeltaYV.Len());
  TFltKdV XYFltValV(YValV.Len(), 0);
  for (int i = 0; i < YValV.Len(); i++) {
    XYFltValV.Add(TFltKd(TFlt(i+1), YValV[i]));
  }
  return AddErrBar(XYFltValV, DeltaYV, Label);
}

int TGnuPlot::AddErrBar(const TFltV& XValV, const TFltV& YValV, const TFltV& DeltaYV, const TStr& Label) {
  IAssert(XValV.Len() == YValV.Len());
  IAssert(XValV.Len() == DeltaYV.Len());
  TFltKdV XYFltValV(XValV.Len(), 0);
  for (int i = 0; i < XValV.Len(); i++) {
    XYFltValV.Add(TFltKd(XValV[i], YValV[i]));
  }
  return AddErrBar(XYFltValV, DeltaYV, Label);
}

int TGnuPlot::AddErrBar(const TFltPrV& XYValV, const TFltV& DeltaYV, const TStr& Label) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(XYValV[i].Val1, XYValV[i].Val2));
  }
  return AddErrBar(XYFltValV, DeltaYV, Label);
}

int TGnuPlot::AddErrBar(const TFltPrV& XYValV, const TFltV& DeltaV, const TStr& DatLabel, const TStr& ErrLabel) {
  TFltKdV XYFltValV(XYValV.Len(), 0);
  for (int i = 0; i < XYValV.Len(); i++) {
    XYFltValV.Add(TFltKd(XYValV[i].Val1, XYValV[i].Val2));
  }
  const int PlotId = AddPlot(XYFltValV, gpwLinesPoints, DatLabel);
  AddErrBar(XYFltValV, DeltaV, ErrLabel);
  return PlotId;
}

int TGnuPlot::AddErrBar(const TFltKdV& XYValV, const TFltV& DeltaYV, const TStr& Label) {
  if (XYValV.Empty()) {
    printf("***AddErrBar: empty plot (%s) %s\n", DataFNm.CStr(), Title.CStr());
    return -1;
  }
  IAssert(XYValV.Len() == DeltaYV.Len());
  TGpSeries Plot;
  Plot.SeriesTy = gpwErrBars;
  Plot.Label = Label;
  Plot.XYValV = XYValV;
  Plot.ZValV = DeltaYV;
  SeriesV.Add(Plot);
  return SeriesV.Len() - 1;
}

int TGnuPlot::AddLinFit(const int& PlotId, const TGpSeriesTy& SeriesTy, const TStr& Style) {
  if (PlotId < 0 || PlotId >= SeriesV.Len()) return -1;
  const TGpSeries& Plot = SeriesV[PlotId];
  if(Plot.XYValV.Empty()) return -1;
  const TFltKdV& XY = Plot.XYValV;
  double A, B, R2, SigA, SigB, Chi2;
  // linear fit
  TFltPrV XYPr;
  int s;
  for (s = 0; s < XY.Len(); s++) {
    XYPr.Add(TFltPr(XY[s].Key, XY[s].Dat));
  }
  TSpecFunc::LinearFit(XYPr, A, B, SigA, SigB, Chi2, R2);
  TStr StyleStr=Style;
  if (StyleStr.Empty()) { StyleStr = "linewidth 3"; }
  const int FitId = AddFunc(TStr::Fmt("%f+%f*x", A, B),
    SeriesTy, TStr::Fmt("%.4g + %.4g x  R^2:%.2g", A, B, R2), StyleStr);
  return FitId;
  /*SeriesV.Add();
  TGpSeries& NewPlot = SeriesV.Last();
  TFltKdV& EstXY = NewPlot.XYValV;
  for (s = 0; s < XY.Len(); s++) {
    EstXY.Add(TFltKd(XY[s].Key, A + B*XYPr[s].Val1));
  }
  NewPlot.Label = TStr::Fmt("%.4g + %.4g x  R^2:%.2g", A, B, R2);
  NewPlot.SeriesTy = SeriesTy;
  if (Style.Empty()) { NewPlot.WithStyle = "linewidth 3"; }
  else { NewPlot.WithStyle = Style; }
  return SeriesV.Len() - 1;*/
}

int TGnuPlot::AddPwrFit(const int& PlotId, const TGpSeriesTy& SeriesTy, const TStr& Style) {
  const int PlotId1 = AddPwrFit3(PlotId, SeriesTy);
  AddPwrFit2(PlotId, SeriesTy, 5.0);
  return PlotId1;
}

// linear fit on log-log scales{%
int TGnuPlot::AddPwrFit1(const int& PlotId, const TGpSeriesTy& SeriesTy, const TStr& Style) {
  if (PlotId < 0 || PlotId >= SeriesV.Len()) return -1;
  const TGpSeries& Plot = SeriesV[PlotId];
  if(Plot.XYValV.Empty()) return -1;
  const TFltKdV& XY = Plot.XYValV;
  double A, B, R2, SigA, SigB, Chi2, MinY = TFlt::Mx, MinX = TFlt::Mx;
  // power fit
  TFltPrV XYPr;
  int s;
  for (s = 0; s < XY.Len(); s++) {
    if (XY[s].Key > 0) {
      XYPr.Add(TFltPr(XY[s].Key, XY[s].Dat)); //!!! skip zero values
      MinX = TMath::Mn(MinX, XY[s].Key());
      MinY = TMath::Mn(MinY, XY[s].Dat());
    }
  }
  MinY = TMath::Mn(1.0, MinY);
  TSpecFunc::PowerFit(XYPr, A, B, SigA, SigB, Chi2, R2);
  TStr StyleStr=Style;
  if (StyleStr.Empty()) { StyleStr = "linewidth 3"; }
  const int FitId = AddFunc(TStr::Fmt("%f*x**%f", A, B),
    SeriesTy, TStr::Fmt("%.1g * x^{%.4g}  R^2:%.2g", A, B, R2), StyleStr);
  return FitId;
  /*SeriesV.Add();
  TGpSeries& NewPlot = SeriesV.Last();
  const int FitId = SeriesV.Len() - 1;
  NewPlot.DataFNm = ;
  TFltKdV& EstXY = NewPlot.XYValV;
  for (s = 0; s < XYPr.Len(); s++) {
    const double YVal = A*pow(XYPr[s].Val1(), B);
    if (YVal < MinY) continue;
    EstXY.Add(TFltKd(XYPr[s].Val1, YVal));
  }
  NewPlot.Label = ;
  NewPlot.SeriesTy = SeriesTy;
  if (Style.Empty()) { NewPlot.WithStyle = "linewidth 3"; }
  else { NewPlot.WithStyle = Style; }
  //if (MinX < 5.0) MinX = 5.0;
  //AddPwrFit2(PlotId, SeriesTy, MinX);*/
}

// MLE power-coefficient
int TGnuPlot::AddPwrFit2(const int& PlotId, const TGpSeriesTy& SeriesTy, const double& MinX, const TStr& Style) {
  const TGpSeries& Plot = SeriesV[PlotId];
  if(Plot.XYValV.Empty()) return -1;
  const TFltKdV& XY = Plot.XYValV;
  // power fit
  TFltPrV XYPr;
  double MinY = TFlt::Mx;
  for (int s = 0; s < XY.Len(); s++) {
    if (XY[s].Key > 0.0) {
      XYPr.Add(TFltPr(XY[s].Key, XY[s].Dat));
      MinY = TMath::Mn(MinY, XY[s].Dat());
    }
  }
  if (XYPr.Empty()) return -1;
  MinY = TMath::Mn(1.0, MinY);
  // determine the sign of power coefficient
  double CoefSign = 0.0;
  { double A, B, R2, SigA, SigB, Chi2;
  TSpecFunc::PowerFit(XYPr, A, B, SigA, SigB, Chi2, R2);
  CoefSign = B > 0.0 ? +1.0 : -1.0; }
  const double PowerCf = CoefSign * TSpecFunc::GetPowerCoef(XYPr, MinX);
  int Mid = (int) exp(log((double)XYPr.Len())/2.0);
  if (Mid >= XYPr.Len()) { Mid = XYPr.Len()-1; }
  const double MidX = XYPr[Mid].Val1();
  const double MidY = XYPr[Mid].Val2();
  const double B = MidY / pow(MidX, PowerCf);
  TStr StyleStr=Style;
  if (StyleStr.Empty()) { StyleStr = "linewidth 3"; }
  const int FitId = AddFunc(TStr::Fmt("%f*x**%f", B, PowerCf),
    SeriesTy, TStr::Fmt("MLE = x^{%.4g}", PowerCf), StyleStr);
  return FitId;
  /*SeriesV.Add();
  TGpSeries& NewPlot = SeriesV.Last();
  TFltKdV& XYFit = NewPlot.XYValV;
  XYFit.Gen(XYPr.Len(), 0);
  for (int s = 0; s < XYPr.Len(); s++) {
    const double XVal = XYPr[s].Val1;
    const double YVal = B * pow(XYPr[s].Val1(), PowerCf);
    if (YVal < MinY || XVal < MinX) continue;
    XYFit.Add(TFltKd(XVal, YVal));
  }
  NewPlot.Label = TStr::Fmt("PowerFit: %g", PowerCf);
  NewPlot.SeriesTy = SeriesTy;
  if (Style.Empty()) { NewPlot.WithStyle = "linewidth 3"; }
  else { NewPlot.WithStyle = Style; }
  return SeriesV.Len() - 1;*/
}

int TGnuPlot::AddPwrFit3(const int& PlotId, const TGpSeriesTy& SeriesTy, const double& MinX, const TStr& Style) {
  double Intercept, Slope, R2;
  return AddPwrFit3(PlotId, SeriesTy, MinX, Style, Intercept, Slope, R2);
}

// some kind of least squares power-law fitting that cutts the tail until the fit is good
int TGnuPlot::AddPwrFit3(const int& PlotId, const TGpSeriesTy& SeriesTy, const double& MinX, const TStr& Style, double& Intercept, double& Slope, double& R2) {
  if (PlotId < 0 || PlotId >= SeriesV.Len()) return -1;
  const TGpSeries& Plot = SeriesV[PlotId];
  if(Plot.XYValV.Empty()) return -1;
  double A, B, SigA, SigB, Chi2, MinY=TFlt::Mx;
  const TFltKdV& XY = Plot.XYValV;
  //SeriesV.Add();
  //TGpSeries& NewPlot = SeriesV.Last();
  //TFltKdV& EstXY = NewPlot.XYValV;
  TFltPrV FitXY, NewFitXY;
  for (int s = 0; s < XY.Len(); s++) {
    if (XY[s].Key > 0 && XY[s].Key >= MinX) {
      FitXY.Add(TFltPr(XY[s].Key, XY[s].Dat)); //!!! skip zero values
      MinY = TMath::Mn(MinY, XY[s].Dat());
    }
  }
  MinY = TMath::Mn(1.0, MinY);
  // power fit (if tail is too fat, cut everything where
  // extrapolation sets the value < MinY
  while (true) {
    TSpecFunc::PowerFit(FitXY, A, B, SigA, SigB, Chi2, R2);
    NewFitXY.Clr(false);
    //EstXY.Clr(false);
    for (int s = 0; s < FitXY.Len(); s++) {
      const double YVal = A*pow(FitXY[s].Val1(), B);
      if (YVal < MinY) continue;
      //EstXY.Add(TFltKd(FitXY[s].Val1, YVal));
      NewFitXY.Add(TFltPr(FitXY[s].Val1, FitXY[s].Val2));
    }
    if (NewFitXY.Len() < 10 || FitXY.Last().Val1 < 1.2 * NewFitXY.Last().Val1) { break; }
    else { FitXY.Swap(NewFitXY); }
  }
  TStr StyleStr=Style;
  if (StyleStr.Empty()) { StyleStr = "linewidth 3"; }
  const int FitId = AddFunc(TStr::Fmt("%f*x**%f", A, B),
    SeriesTy, TStr::Fmt("%.1g * x^{%.4g}  R^2:%.2g", A, B, R2), StyleStr);
  return FitId;
  /*NewPlot.Label = TStr::Fmt("%.1g * x^{%.4g}  R^2:%.2g", A, B, R2);
  Intercept = A;
  Slope = B;
  NewPlot.SeriesTy = SeriesTy;
  if (Style.Empty()) { NewPlot.WithStyle = "linewidth 3"; }
  else { NewPlot.WithStyle = Style; }
  return SeriesV.Len() - 1;*/
}

int TGnuPlot::AddLogFit(const int& PlotId, const TGpSeriesTy& SeriesTy, const TStr& Style) {
  const TGpSeries& Plot = SeriesV[PlotId];
  if(Plot.XYValV.Empty()) return -1;
  const TFltKdV& XY = Plot.XYValV;
  double A, B, R2, SigA, SigB, Chi2;
  // power fit
  TFltPrV XYPr;
  int s;
  for (s = 0; s < XY.Len(); s++) {
    if (XY[s].Key > 0) {
      XYPr.Add(TFltPr(XY[s].Key, XY[s].Dat)); } //!!! skip zero values
  }
  TSpecFunc::LogFit(XYPr, A, B, SigA, SigB, Chi2, R2);
  TStr StyleStr=Style;
  if (StyleStr.Empty()) { StyleStr = "linewidth 3"; }
  const int FitId = AddFunc(TStr::Fmt("%f+%f*log(x)", A, B),
    SeriesTy, TStr::Fmt("%.4g + %.4g log(x)  R^2:%.2g", A, B, R2), StyleStr);
  return FitId;
  /*SeriesV.Add();
  TGpSeries& NewPlot = SeriesV.Last();
  TFltKdV& EstXY = NewPlot.XYValV;
  for (s = 0; s < XYPr.Len(); s++) {
    EstXY.Add(TFltKd(XYPr[s].Val1, A+B*log((double)XYPr[s].Val1)));
  }
  NewPlot.Label = TStr::Fmt("%.4g + %.4g log(x)  R^2:%.2g", A, B, R2);
  NewPlot.SeriesTy = SeriesTy;
  if (Style.Empty()) { NewPlot.WithStyle = "linewidth 3"; }
  else { NewPlot.WithStyle = Style; }
  return SeriesV.Len() - 1;*/
}

int TGnuPlot::AddExpFit(const int& PlotId, const TGpSeriesTy& SeriesTy, const double& FitXOffset, const TStr& Style) {
  const TGpSeries& Plot = SeriesV[PlotId];
  if(Plot.XYValV.Empty()) return -1;
  const TFltKdV& XY = Plot.XYValV;
  double A, B, R2, SigA, SigB, Chi2;
  // power fit
  TFltPrV XYPr;
  int s;
  for (s = 0; s < XY.Len(); s++) {
    if (XY[s].Key-FitXOffset > 0) {
      XYPr.Add(TFltPr(XY[s].Key-FitXOffset, XY[s].Dat)); } //!!! skip zero values
  }
  TSpecFunc::ExpFit(XYPr, A, B, SigA, SigB, Chi2, R2);
  TStr Label, StyleStr=Style;
  if (FitXOffset == 0) { Label = TStr::Fmt("%.4g exp(%.4g x)  R^2:%.2g", A, B, R2); }
  else { Label = TStr::Fmt("%.4g exp(%.4g x - %g)  R^2:%.2g", A, B, FitXOffset, R2); }
  if (StyleStr.Empty()) { StyleStr = "linewidth 3"; }
  const int FitId = AddFunc(TStr::Fmt("%f*exp(%f*x-%f)", A, B, FitXOffset),
    SeriesTy, Label, StyleStr);
  return FitId;
  /*SeriesV.Add();
  TGpSeries& NewPlot = SeriesV.Last();
  TFltKdV& EstXY = NewPlot.XYValV;
  for (s = 0; s < XYPr.Len(); s++) {
    EstXY.Add(TFltKd(XYPr[s].Val1+FitXOffset, A*exp(B*XYPr[s].Val1)));
  }
  NewPlot.SeriesTy = SeriesTy;
  if (Style.Empty()) { NewPlot.WithStyle = "linewidth 3"; }
  else { NewPlot.WithStyle = Style; }
  return SeriesV.Len() - 1;*/
}

void TGnuPlot::SavePng(const TStr& FNm, const int& SizeX, const int& SizeY, const TStr& Comment, const TStr& Terminal) {
  if (Terminal.Empty()) {
    //#ifdef GLib_WIN
    AddCmd(TStr::Fmt("set terminal png small size %d,%d", SizeX, SizeY));
    AddCmd(TStr::Fmt("set output '%s'", FNm.CStr()));
    /**
    #else // EPS
    AddCmd("set terminal postscript eps 10 enhanced color");
    AddCmd(TStr::Fmt("set output '%s.eps'", FNm.GetFMid().CStr()));
    #endif
    */
  } else {
    AddCmd(Terminal);
    AddCmd(TStr::Fmt("set output '%s'", FNm.CStr()));
  }
  Pause(false);
  CreatePlotFile(Comment.Empty()? Title : Comment);
  RunGnuPlot();
  MoreCmds.DelLast();
  MoreCmds.DelLast();
}

void TGnuPlot::SaveEps(const TStr& FNm, const int& FontSz, const TStr& Comment) {
  AddCmd(TStr::Fmt("set terminal postscript enhanced eps %d color", FontSz));
  AddCmd(TStr::Fmt("set output '%s'", FNm.CStr()));
  Pause(false);
  CreatePlotFile(Comment.Empty()? Title : Comment);
  RunGnuPlot();
  MoreCmds.DelLast();
  MoreCmds.DelLast();
}

void TGnuPlot::MakeExpBins(const TFltPrV& XYValV, TFltPrV& ExpXYValV, const double& BinFactor, const double& MinYVal) {
  TFltKdV KdV(XYValV.Len(), 0), OutV;
  for (int i = 0; i < XYValV.Len(); i++) {
    KdV.Add(TFltKd(XYValV[i].Val1, XYValV[i].Val2)); }
  KdV.Sort();
  TGnuPlot::MakeExpBins(KdV, OutV, BinFactor, MinYVal);
  ExpXYValV.Gen(OutV.Len(), 0);
  for (int i = 0; i < OutV.Len(); i++) {
    ExpXYValV.Add(TFltPr(OutV[i].Key, OutV[i].Dat)); }
}

void TGnuPlot::MakeExpBins(const TFltKdV& XYValV, TFltKdV& ExpXYValV, const double& BinFactor, const double& MinYVal) {
  if (XYValV.Empty()) { ExpXYValV.Clr(false); return; }
  IAssert(! XYValV.Empty());
  IAssert(XYValV.IsSorted());
  const TFlt MxX = XYValV.Last().Key;
  // find buckets
  TFltV BucketEndV; BucketEndV.Add(1);
  double PrevBPos = 1, BPos = 1;
  while (BPos <= MxX) {
    PrevBPos = (uint) floor(BPos);
    BPos *= BinFactor;
    if (floor(BPos) == PrevBPos) {
      BPos = PrevBPos + 1; }
    BucketEndV.Add(floor(BPos));
  }
  //printf("buckets:\n"); for (int i = 0; i < BucketEndV.Len(); i++) { printf("\t%g\n", BucketEndV[i]);}
  ExpXYValV.Gen(BucketEndV.Len(), 0);
  int CurB = 0;
  double AvgPos=0, Cnt=0, AvgVal=0;
  for (int v = 0; v < XYValV.Len(); v++) {
    if (XYValV[v].Key() == 0.0) { continue; }
    AvgPos += XYValV[v].Key ;//* XYValV[v].Dat;  // x
    AvgVal += XYValV[v].Dat;  // y
    Cnt++;
    if (v+1 == XYValV.Len() || XYValV[v+1].Key > BucketEndV[CurB]) {
      if (Cnt != 0) {
        //AvgPos /= AvgVal;
        //AvgVal /= (BucketEndV[CurB]-BucketEndV[CurB-1]);
        AvgPos /= (double) Cnt;
        AvgVal /= (double) Cnt;
        if (AvgVal < MinYVal) { AvgVal = MinYVal; }
        ExpXYValV.Add(TFltKd(AvgPos, AvgVal));
        //printf("b: %6.2f\t%6.2f\n", AvgPos, AvgVal);
        AvgPos = 0;  AvgVal = 0;  Cnt = 0;
      }
      CurB++;
    }
  }
}

void TGnuPlot::LoadTs(const TStr& FNm, TStrV& ColNmV, TVec<TFltKdV>& ColV) {
  PSs Ss = TSs::LoadTxt(ssfTabSep, FNm);
  int row = 0;
  ColNmV.Clr();
  while (Ss->At(0, row)[0] == '#') { row++; }
  for (int c = 1; c < Ss->GetXLen(row); c+=2) {
    ColNmV.Add(Ss->At(c, row));
  }
  row++;
  ColV.Gen(ColNmV.Len(), ColNmV.Len());
  for (; row < Ss->GetYLen(); row++) {
    for (int c = 0; c < Ss->GetXLen(row); c+=2) {
      if (Ss->At(c,row).Empty()) break;
      ColV[c/2].Add(TFltKd(Ss->At(c,row).GetFlt(), Ss->At(c+1,row).GetFlt()));
    }
  }
}

TStr TGnuPlot::GetScaleStr(const TGpScaleTy& ScaleTy) {
  switch(ScaleTy){
    case gpsNoAuto: return TStr("set noautoscale");
    case gpsAuto: return TStr("set autoscale");
    case gpsLog: return TStr("set logscale");
    case gpsLog2X: return TStr("set logscale x 2");
    case gpsLog2Y: return TStr("set logscale y 2");
    case gpsLog2XY: return TStr("set logscale xy 2");
    case gpsLog10X: return TStr("set logscale x 10");
    case gpsLog10Y: return TStr("set logscale y 10");
    case gpsLog10XY: return TStr("set logscale xy 10");
    default: Fail;
  }
  return TStr();
}

TStr TGnuPlot::GetSeriesTyStr(const TGpSeriesTy& SeriesTy) {
  switch(SeriesTy) {
    case gpwLines: return TStr("lines");
    case gpwPoints: return TStr("points");
    case gpwLinesPoints: return TStr("linespoints");
    case gpwImpulses: return TStr("impulses");
    case gpwDots: return TStr("dots");
    case gpwSteps: return TStr("steps");
    case gpwFSteps: return TStr("fsteps");
    case gpwHiSteps: return TStr("histeps");
    case gpwBoxes: return TStr("boxes");
    case gpwErrBars: return TStr("errorbars");
    case gpwFilledCurves: return TStr("filledcurves");
    default: Fail;
  }
  return TStr();
}

void TGnuPlot::SaveTs(const TIntKdV& KdV, const TStr& FNm, const TStr& HeadLn) {
  FILE *F = fopen(FNm.CStr(), "wt");
  EAssert(F);
  if (! HeadLn.Empty()) fprintf(F, "# %s\n", HeadLn.CStr());
  for (int i = 0; i < KdV.Len(); i++) {
    fprintf(F, "%d\t%d\n", KdV[i].Key(), KdV[i].Dat()); }
  fclose(F);
}


void TGnuPlot::SaveTs(const TIntFltKdV& KdV, const TStr& FNm, const TStr& HeadLn) {
  FILE *F = fopen(FNm.CStr(), "wt");
  EAssert(F);
  if (! HeadLn.Empty()) fprintf(F, "# %s\n", HeadLn.CStr());
  for (int i = 0; i < KdV.Len(); i++)
    fprintf(F, "%d\t%g\n", KdV[i].Key(), KdV[i].Dat());
  fclose(F);
}

void TGnuPlot::Test() {
  TFltV DeltaY;
  TFltPrV ValV1, ValV2, ValV3;
  for (int i = 1; i < 30; i++) {
    ValV1.Add(TFltPr(i, pow(double(i), 1.2)));
    DeltaY.Add(5*TInt::Rnd.GetUniDev());
    ValV2.Add(TFltPr(i, 5*i-1));
  }
  for (int i = -10; i < 20; i++) {
    ValV3.Add(TFltPr(i, 2*i + 2 + TInt::Rnd.GetUniDev()));
  }
  TGnuPlot GnuPlot("testDat", "TestPlot", true);
  GnuPlot.SetXYLabel("X", "Y");
  const int id2 = GnuPlot.AddPlot(ValV2, gpwPoints, "y=5*x-1");
  const int id3 = GnuPlot.AddPlot(ValV3, gpwPoints, "y=2*x+2");
  GnuPlot.AddErrBar(ValV1, DeltaY, "y=x^2", "Error bar");
  GnuPlot.AddLinFit(id2, gpwLines);
  GnuPlot.AddLinFit(id3, gpwLines);
  GnuPlot.Plot();
  GnuPlot.SavePng("testPlot.png");
}

int TGnuPlot::IsSameXCol(const int& CurId, const int& PrevId) const {
  //if (SerId < 1) { return -1; }
  if (SeriesV[CurId].XYValV.Len() != SeriesV[PrevId].XYValV.Len()) { return -1; }
  for (int x = 0; x < SeriesV[CurId].XYValV.Len(); x++) {
    if (SeriesV[CurId].XYValV[x] != SeriesV[PrevId].XYValV[x]) { return -1; }
  }
  IAssert(SeriesV[PrevId].XCol > 0);
  return SeriesV[PrevId].XCol;
}

void TGnuPlot::CreatePlotFile(const TStr& Comment) {
  time_t ltime;  time(&ltime);
  char* TimeStr = ctime(&ltime);  TimeStr[strlen(TimeStr) - 1] = 0;
  // rearrange columns so that longest are on the left
  //SeriesV.Sort(false);
  TIntV SerIdV(SeriesV.Len(), 0);
  for (int i = 0; i < SeriesV.Len(); i++) { SerIdV.Add(i); }
  SerIdV.SortCmp(TGpSeriesCmp(SeriesV));
  // set columns
  int ColCnt = 1;
  bool SaveData = false;
  for (int s = 0; s < SeriesV.Len(); s++) {
    TGpSeries& Plt = SeriesV[SerIdV[s]];
    if (Plt.XYValV.Empty()) { continue; }
    Plt.DataFNm = DataFNm;
    // plots use same X column
    const int PrevCol = s > 0 ? IsSameXCol(SerIdV[s], SerIdV[s-1]) : -1;
    if (PrevCol != -1) { Plt.XCol = PrevCol; }
    else { Plt.XCol = ColCnt;  ColCnt++; }
    Plt.YCol = ColCnt;  ColCnt++;
    if (! Plt.ZValV.Empty()) { Plt.ZCol = ColCnt;  ColCnt++; }
    if (! Plt.XYValV.Empty()) { SaveData=true; }
  }
  // save data file (skip duplicate X columns)
  if (SaveData) {
    FILE *F = fopen(DataFNm.CStr(), "wt");
    EAssertR(F != NULL, TStr("Can not open data file ")+DataFNm);
    fprintf(F, "#\n");
    fprintf(F, "# %s (%s)\n", Comment.CStr(), TimeStr);
    fprintf(F, "#\n");
    // column names
    for (int i = 0; i < SerIdV.Len(); i++) {
      const TGpSeries& Ser = SeriesV[SerIdV[i]];
      if (Ser.XYValV.Empty()) { continue; }
      if (i == 0) { fprintf(F, "# "); } else { fprintf(F, "\t"); }
      if (Ser.SaveXVals()) {
        if (! LblX.Empty()) { fprintf(F, "%s\t", LblX.CStr()); }
        else { fprintf(F, "XVals\t"); }
      }
      if (Ser.Label.Empty()) { fprintf(F, "%s", LblY.CStr()); }
      else { fprintf(F, "%s", SeriesV[SerIdV[i]].Label.CStr()); }
      if (Ser.ZCol > 0) fprintf(F, "\tDeltaY");
    }
    fprintf(F, "\n");
    // data
    for (int row = 0; row < SeriesV[SerIdV[0]].XYValV.Len(); row++) {
      for (int i = 0; i < SeriesV.Len(); i++) {
        const TGpSeries& Ser = SeriesV[SerIdV[i]];
        if (row < Ser.XYValV.Len()) {
          if (i > 0) { fprintf(F, "\t"); }
          if (Ser.SaveXVals()) { fprintf(F, "%g\t%g", Ser.XYValV[row].Key(), Ser.XYValV[row].Dat()); }
          else { fprintf(F, "%g", Ser.XYValV[row].Dat()); }
          if (! Ser.ZValV.Empty()) { fprintf(F, "\t%g", Ser.ZValV[row]()); }
        }
      }
      fprintf(F, "\n");
    }
    fclose(F);
  }
  // save plot file
  FILE *F = fopen(PlotFNm.CStr(), "wt");
  EAssertR(F != 0, TStr("Can not open plot file ")+PlotFNm);
  TStr CurDir = TDir::GetCurDir();
  CurDir.ChangeStrAll("\\", "\\\\");
  fprintf(F, "#\n");
  fprintf(F, "# %s (%s)\n", Comment.CStr(), TimeStr);
  fprintf(F, "#\n\n");
  if (! Title.Empty()) fprintf(F, "set title \"%s\"\n", Title.CStr());
  fprintf(F, "set key bottom right\n");
  fprintf(F, "%s\n", GetScaleStr(ScaleTy).CStr());
  if (ScaleTy==gpsLog || ScaleTy==gpsLog10X || ScaleTy==gpsLog10XY) { 
    fprintf(F, "set format x \"10^{%%L}\"\n"); 
    fprintf(F, "set mxtics 10\n"); }
  if (ScaleTy==gpsLog || ScaleTy==gpsLog10Y || ScaleTy==gpsLog10XY) { 
    fprintf(F, "set format y \"10^{%%L}\"\n"); 
    fprintf(F, "set mytics 10\n"); }
  if (ScaleTy==gpsLog2X || ScaleTy==gpsLog2XY) { fprintf(F, "set format x \"2^{%%L}\"\n"); }
  if (ScaleTy==gpsLog2Y || ScaleTy==gpsLog2XY) { fprintf(F, "set format y \"2^{%%L}\"\n"); }
  if (SetGrid) fprintf(F, "set grid\n");
  if (XRange.Val1 != XRange.Val2) fprintf(F, "set xrange [%g:%g]\n", XRange.Val1(), XRange.Val2());
  if (YRange.Val1 != YRange.Val2) fprintf(F, "set yrange [%g:%g]\n", YRange.Val1(), YRange.Val2());
  if (! LblX.Empty()) fprintf(F, "set xlabel \"%s\"\n", LblX.CStr());
  if (! LblY.Empty()) fprintf(F, "set ylabel \"%s\"\n", LblY.CStr());
  //fprintf(F, "set tics scale 2\n"); // J:new in version 4.2
  fprintf(F, "set ticscale 2 1\n"); // J:old (depreciated)
  // custom commands
  for (int i = 0; i < MoreCmds.Len(); i++) {
    fprintf(F, "%s\n", MoreCmds[i].CStr()); }
  // plot
  if (! SeriesV.Empty()) {
    fprintf(F, "plot \t");
    for (int i = 0; i < SeriesV.Len(); i++) {
      fprintf(F, "%s", GetSeriesPlotStr(i).CStr()); }
    fprintf(F, "\n");
  }
  if (SetPause) fprintf(F, "pause -1 \"Hit return to exit. %s\"\n", PlotFNm.CStr());
  fclose(F);
}

void TGnuPlot::RunGnuPlot() const {
  TStr GpFNm, GpPath;
  #if defined(GLib_WIN)
    GpFNm = "wgnuplot.exe";
    GpPath = "C:\\gnuplot\\";
  #elif defined(GLib_CYGWIN)
    GpFNm = "gnuplot.exe";
    GpPath = "/usr/bin/";
  #else
    GpFNm = "gnuplot";
    GpPath = "/usr/bin/";
  #endif
  if (system(TStr::Fmt("%s %s", GpFNm.CStr(), PlotFNm.CStr()).CStr())==0) { return; }
  #if defined(GLib_WIN)
  if (system(TStr::Fmt(".\\%s %s", GpFNm.CStr(), PlotFNm.CStr()).CStr())==0) { return; }
  #else
  if (system(TStr::Fmt("./%s %s", GpFNm.CStr(), PlotFNm.CStr()).CStr())==0) { return; }
  #endif
  if (system(TStr::Fmt("%s%s %s", GpPath.CStr(), GpFNm.CStr(), PlotFNm.CStr()).CStr())==0) { return; }
  //FailR(TStr::Fmt("Cat not find GnuPlot (%s) for plot %s. Set the PATH.", GpFNm.CStr(), PlotFNm.CStr()).CStr());
  //ErrNotify(TStr::Fmt("Cat not find GnuPlot (%s) for plot %s. Set the PATH.", GpFNm.CStr(), PlotFNm.CStr()).CStr());
  fprintf(stderr, "[%s:%d] Cat not find GnuPlot (%s) for plot %s. Set the PATH.\n", __FILE__, __LINE__, GpFNm.CStr(), PlotFNm.CStr());
}
