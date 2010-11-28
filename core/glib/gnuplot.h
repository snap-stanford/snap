#ifndef gnuplot_h
#define gnuplot_h

/////////////////////////////////////////////////
// GNU-Plot-Chart
typedef enum {
  gpsNoAuto, gpsAuto, gpsLog, gpsLog2X, gpsLog2Y, gpsLog2XY, gpsLog10X,
  gpsLog10Y, gpsLog10XY
} TGpScaleTy;

typedef enum {
  gpwUndef, gpwLines, gpwPoints, gpwLinesPoints, gpwImpulses, gpwDots,
  gpwSteps, gpwFSteps, gpwHiSteps, gpwBoxes, gpwErrBars, gpwFilledCurves, gpwMax
} TGpSeriesTy;

class TGnuPlot {
public:
  static TStr DefPlotFNm;
  static TStr DefDataFNm;
private:
  class TGpSeries {
  public:
    TGpSeriesTy SeriesTy;
    TFltKdV XYValV;
    TFltV ZValV; // error bar and 3d plots
    TStr Label, WithStyle, DataFNm;
    int XCol, YCol, ZCol;
  public:
    TGpSeries() : SeriesTy(gpwLines), XYValV(), ZValV(), Label(), WithStyle(), DataFNm(), XCol(0), YCol(0), ZCol(0) { }
    TGpSeries(const TGpSeries& Gps);
    TGpSeries& operator = (const TGpSeries& Gps);
    bool operator < (const TGpSeries& Gps) const;
    bool SaveXVals() const { return (YCol-XCol)==1; }
  };
  class TGpSeriesCmp {
  private:
    const TVec<TGpSeries>& SeriesV;
  public:
    TGpSeriesCmp(const TVec<TGpSeries>& _SeriesV) : SeriesV(_SeriesV) { }
    bool operator () (const int& Left, const int& Right) const {
      return SeriesV[Left] > SeriesV[Right]; }
  };
private:
  TStr DataFNm, PlotFNm;
  TStr Title, LblX, LblY;
  TGpScaleTy ScaleTy;
  TFltPr YRange, XRange;
  bool SetGrid, SetPause;
  TVec<TGpSeries> SeriesV;
  TStrV MoreCmds;
public:
  TStr GetSeriesPlotStr(const int& PlotN);
  int IsSameXCol(const int& CurId, const int& PrevId) const;
  void CreatePlotFile(const TStr& Comment = TStr());
  void RunGnuPlot() const;
public:
  TGnuPlot(const TStr& FileNm="gplot", const TStr& PlotTitle=TStr(), const bool& Grid=true);
  TGnuPlot(const TStr& DataFileNm, const TStr& PlotFileNm, const TStr& PlotTitle, const bool& Grid);
  TGnuPlot(const TGnuPlot& GnuPlot);
  TGnuPlot& operator = (const TGnuPlot& GnuPlot);
  
  void SetTitle(const TStr& PlotTitle) { Title = PlotTitle; }
  void SetXLabel(const TStr& XLabel) { LblX = XLabel; }
  void SetYLabel(const TStr& YLabel) { LblY = YLabel; }
  void SetXYLabel(const TStr& XLabel, const TStr& YLabel) { LblX = XLabel;  LblY = YLabel; }
  void SetDataPlotFNm(const TStr& DatFNm, const TStr& PltFNm) { DataFNm = DatFNm;  PlotFNm = PltFNm; }
  
  void ShowGrid(const bool& Show) { SetGrid = Show; }
  void Pause(const bool& DoPause) { SetPause = DoPause; }
  void SetScale(const TGpScaleTy& GpScaleTy) { ScaleTy = GpScaleTy;}
  void SetXRange(const double& Min, const double& Max) { XRange = TFltPr(Min, Max); }
  void SetYRange(const double& Min, const double& Max) { YRange = TFltPr(Min, Max); }
  void AddCmd(const TStr& Cmd) { MoreCmds.Add(Cmd); }
  TStr GetLineStyle(const int& PlotId) const { return SeriesV[PlotId].WithStyle; }
  void SetLineStyle(const int& PlotId, const TStr& StyleStr) { SeriesV[PlotId].WithStyle = StyleStr; }

  int AddFunc(const TStr& FuncStr, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TIntV& YValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TFltV& YValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TFltV& XValV, const TFltV& YValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TIntPrV& XYValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TFltPrV& XYValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TIntKdV& XYValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TFltKdV& XYValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TIntFltKdV& XYValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TIntFltPrV& XYValV, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TStr& DataFNm, const int& ColY, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  int AddPlot(const TStr& DataFNm, const int& ColX, const int& ColY, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr());
  template<class TKey, class TDat, class THashFunc>
  int AddPlot(const THash<TKey, TDat, THashFunc>& XYValH, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr()) {
    TFltKdV XYFltValV(XYValH.Len(), 0);  for (int k = XYValH.FFirstKeyId();  XYValH.FNextKeyId(k); ) { 
      XYFltValV.Add(TFltKd(TFlt(XYValH.GetKey(k)), TFlt(XYValH[k]))); }  XYFltValV.Sort();
    return AddPlot(XYFltValV, SeriesTy, Label, Style); }
  template<class TKey, class THashFunc>
  int AddPlot(THash<TKey, TMom, THashFunc>& ValMomH, const TGpSeriesTy& SeriesTy=gpwLinesPoints, const TStr& Label=TStr(), const TStr& Style=TStr(),
    bool PlotAvg=true, bool PlotMed=true, bool PlotMin=false, bool PlotMax=false, bool PlotSDev=false, bool PlotStdErr=false);

  int AddErrBar(const TFltTrV& XYDValV, const TStr& Label=TStr());
  int AddErrBar(const TFltTrV& XYDValV, const TStr& DatLabel, const TStr& ErrLabel);
  int AddErrBar(const TFltV& YValV, const TFltV& DeltaYV, const TStr& Label=TStr());
  int AddErrBar(const TFltV& XValV, const TFltV& YValV, const TFltV& DeltaYV, const TStr& Label=TStr());
  int AddErrBar(const TFltPrV& XYValV, const TFltV& DeltaYV, const TStr& Label=TStr());
  int AddErrBar(const TFltKdV& XYValV, const TFltV& DeltaYV, const TStr& Label=TStr());
  int AddErrBar(const TFltPrV& XYValV, const TFltV& DeltaYV, const TStr& DatLabel, const TStr& ErrLabel);

  int AddLinFit(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const TStr& Style=TStr());
  int AddPwrFit(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const TStr& Style=TStr());
  int AddPwrFit1(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const TStr& Style=TStr());
  int AddPwrFit2(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const double& MinX=-1.0, const TStr& Style=TStr());
  int AddPwrFit3(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const double& MinX=-1.0, const TStr& Style=TStr());
  int AddPwrFit3(const int& PlotId, const TGpSeriesTy& SeriesTy, const double& MinX, const TStr& Style, double& Intercept, double& Slope, double& R2);
  int AddLogFit(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const TStr& Style=TStr());
  int AddExpFit(const int& PlotId, const TGpSeriesTy& SeriesTy=gpwLines, const double& FitXOffset=0.0, const TStr& Style=TStr());

  void SavePng(const int& SizeX=1000, const int& SizeY=800, const TStr& Comment=TStr()) {
    SavePng(PlotFNm.GetFMid()+".png", SizeX, SizeY, Comment); }
  void SavePng(const TStr& FNm, const int& SizeX=1000, const int& SizeY=800, const TStr& Comment=TStr(), const TStr& Terminal=TStr());
  void SaveEps(const int& FontSz=30, const TStr& Comment=TStr()) {
    SaveEps(PlotFNm.GetFMid()+".eps", FontSz, Comment); }
  void SaveEps(const TStr& FNm, const int& FontSz=30, const TStr& Comment=TStr());
  void Plot(const TStr& Comment=TStr()) { CreatePlotFile(Comment);  RunGnuPlot(); }

  static void MakeExpBins(const TFltPrV& XYValV, TFltPrV& ExpXYValV,
    const double& BinFactor = 2, const double& MinYVal = 1);
  static void MakeExpBins(const TFltKdV& XYValV, TFltKdV& ExpXYValV,
    const double& BinFactor = 2, const double& MinYVal = 1);
  static void LoadTs(const TStr& FNm, TStrV& ColNmV, TVec<TFltKdV>& ColV);

  static TStr GetScaleStr(const TGpScaleTy& ScaleTy);
  static TStr GetSeriesTyStr(const TGpSeriesTy& SeriesTy);

  // save tab separated
  static void SaveTs(const TIntKdV& KdV, const TStr& FNm, const TStr& HeadLn = TStr());
  static void SaveTs(const TIntFltKdV& KdV, const TStr& FNm, const TStr& HeadLn = TStr());
  template <class TVal1, class TVal2>
  static void SaveTs(const TVec<TPair<TVal1, TVal2> >& ValV, const TStr& FNm, const TStr& HeadLn = TStr());
  template <class TVal1, class TVal2, class TVal3>
  static void SaveTs(const TVec<TTriple<TVal1, TVal2, TVal3> >& ValV, const TStr& FNm, const TStr& HeadLn = TStr());
  template <class TVal, int Vals>
  static void SaveTs(const TVec<TTuple<TVal, Vals> >& ValV, const TStr& FNm, const TStr& HeadLn = TStr());
  static void Test();

  // plot value-count tables, and pair vectors
  template <class TVal1, class TVal2>
  static void PlotValV(const TVec<TPair<TVal1, TVal2> >& ValV, const TStr& OutFNmPref, const TStr& Desc="",
   const TStr& XLabel="", const TStr& YLabel="", const TGpScaleTy& ScaleTy=gpsAuto, const bool& PowerFit=false, 
   const TGpSeriesTy& SeriesTy=gpwLinesPoints);
  template <class TVal1, class TVal2>
  static void PlotValV(const TVec<TPair<TVal1, TVal2> >& ValV1, const TStr& Name1,
    const TVec<TPair<TVal1, TVal2> >& ValV2, const TStr& Name2, const TStr& OutFNmPref, const TStr& Desc="",
   const TStr& XLabel="", const TStr& YLabel="", const TGpScaleTy& ScaleTy=gpsAuto, const bool& PowerFit=false, 
   const TGpSeriesTy& SeriesTy=gpwLinesPoints);
  template <class TVal1>
  static void PlotValV(const TVec<TVal1>& ValV, const TStr& OutFNmPref, const TStr& Desc="",
   const TStr& XLabel="", const TStr& YLabel="", const TGpScaleTy& ScaleTy=gpsAuto, const bool& PowerFit=false, 
   const TGpSeriesTy& SeriesTy=gpwLinesPoints);
  template <class TKey, class TVal, class THashFunc>
  static void PlotValCntH(const THash<TKey, TVal, THashFunc>& ValCntH, const TStr& OutFNmPref, const TStr& Desc="",
   const TStr& XLabel="", const TStr& YLabel="", const TGpScaleTy& ScaleTy=gpsAuto, const bool& PowerFit=false, 
   const TGpSeriesTy& SeriesTy=gpwLinesPoints, const bool& PlotNCDF=false, const bool& ExpBucket=false);
  template <class TKey, class TVal, class THashFunc>
  static void PlotValCntH(const THash<TKey, TVal, THashFunc>& ValCntH1, const TStr& Label1, 
    const THash<TKey, TVal, THashFunc>& ValCntH2, const TStr& Label2, 
    const TStr& OutFNmPref, const TStr& Desc, const TStr& XLabel, const TStr& YLabel, 
    const TGpScaleTy& ScaleTy=gpsAuto, const TGpSeriesTy& SeriesTy=gpwLinesPoints);
  template <class TKey, class TVal, class THashFunc>
  static void PlotValCntH(const THash<TKey, TVal, THashFunc>& ValCntH1, const TStr& Label1, 
    const THash<TKey, TVal, THashFunc>& ValCntH2, const TStr& Label2, 
    const THash<TKey, TVal, THashFunc>& ValCntH3, const TStr& Label3, 
    const TStr& OutFNmPref, const TStr& Desc, const TStr& XLabel, const TStr& YLabel, 
    const TGpScaleTy& ScaleTy=gpsAuto, const TGpSeriesTy& SeriesTy=gpwLinesPoints);
  template <class TVal1>
  static void PlotValMomH(THash<TVal1, TMom>& ValMomH, const TStr& OutFNmPref, const TStr& Desc="",
   const TStr& XLabel="", const TStr& YLabel="", const TGpScaleTy& ScaleTy=gpsAuto, const TGpSeriesTy& SeriesTy=gpwLinesPoints,
   bool PlotAvg=true, bool PlotMed=true, bool PlotMin=false, bool PlotMax=false, bool PlotSDev=false, bool PlotStdErr=true);
  
};

//---------------------------------------------------------
// useful commands
// set terminal png small size 800,600
// set terminal postscript enhanced eps 22
// set output 'hopsasa.png'
// set size 1,0.5
// set pointsize 0.9
// set key right bottom Left
// set style fill solid 0.2
// set ticscale 3 1   # longer axis tics
// plot ... with points pointtype 6 pointsize 1
// 1   +           7  (f) circle
// 2  cross        8  (o) triangle
// 3  *            9  (f) triangle
// 4  (o) square  10  (o) inverse-triangle
// 5  (f) square  11  (f) inverse-triangle
// 6  (o) circle  12  (o) diamond
//                13  (f) diamond
// set label "Text" at 23,47000
// set arrow from 28,45000 to 28,31000 lw 3
// axes x1y2: http://t16web.lanl.gov/Kawano/gnuplot/plot1-e.html#5.2

template <class TVal1, class TVal2>
void TGnuPlot::SaveTs(const TVec<TPair<TVal1, TVal2> >& ValV, const TStr& FNm, const TStr& HeadLn) {
  FILE *F = fopen(FNm.CStr(), "wt");
  EAssert(F);
  if (! HeadLn.Empty()) { fprintf(F, "# %s\n", HeadLn.CStr()); }
  for (int i = 0; i < ValV.Len(); i++) {
    fprintf(F, "%g\t%g\n", double(ValV[i].Val1), double(ValV[i].Val2)); }
  fclose(F);
}

template <class TVal1, class TVal2, class TVal3>
void TGnuPlot::SaveTs(const TVec<TTriple<TVal1, TVal2, TVal3> >& ValV, const TStr& FNm, const TStr& HeadLn) {
  FILE *F = fopen(FNm.CStr(), "wt");
  EAssert(F);
  if (! HeadLn.Empty()) { fprintf(F, "# %s\n", HeadLn.CStr()); }
  for (int i = 0; i < ValV.Len(); i++) {
    fprintf(F, "%g\t%g\t%g\n", double(ValV[i].Val1), double(ValV[i].Val2), double(ValV[i].Val3)); }
  fclose(F);
}

template <class TVal, int Vals>
void TGnuPlot::SaveTs(const TVec<TTuple<TVal, Vals> >& ValV, const TStr& FNm, const TStr& HeadLn) {
  FILE *F = fopen(FNm.CStr(), "wt");
  EAssert(F);
  if (! HeadLn.Empty()) { fprintf(F, "# %s\n", HeadLn.CStr()); }
  for (int i = 0; i < ValV.Len(); i++) {
    fprintf(F, "%g", double(ValV[i][0]));
    for (int v = 1; v < Vals; v++) {
      fprintf(F, "\t%g", double(ValV[i][v])); }
    fprintf(F, "\n");
  }
  fclose(F);
}

template<class TKey, class THashFunc>
int TGnuPlot::AddPlot(THash<TKey, TMom, THashFunc>& ValMomH, const TGpSeriesTy& SeriesTy, const TStr& Label, const TStr& Style, bool PlotAvg, bool PlotMed, bool PlotMin, bool PlotMax, bool PlotSDev, bool PlotStdErr) {
  TFltTrV AvgV, StdErrV;
  TFltPrV AvgV2, MedV, MinV, MaxV;
  for (int i = ValMomH.FFirstKeyId(); ValMomH.FNextKeyId(i); ) {
    ValMomH[i].Def();
    if (PlotAvg) { 
      if (PlotSDev) { 
        AvgV.Add(TFltTr((double)ValMomH.GetKey(i), ValMomH[i].GetMean(), ValMomH[i].GetSDev())); } // std deviation
      else { 
        AvgV2.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMean())); 
      }
      if (PlotStdErr) {
        StdErrV.Add(TFltTr((double)ValMomH.GetKey(i), ValMomH[i].GetMean(), ValMomH[i].GetSDev()/sqrt((double)ValMomH[i].GetVals()))); 
      }
    }
    if (PlotMed) { MedV.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMedian())); }
    if (PlotMin) { MinV.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMn())); }
    if (PlotMax) { MaxV.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMx())); }
  }
  AvgV.Sort();  AvgV2.Sort();
  MedV.Sort();  MinV.Sort();  MaxV.Sort(); 
  int PlotId=0;
  if (! AvgV.Empty()) { PlotId = AddErrBar(AvgV, Label+" Average", "StdDev"); }
  if (! AvgV2.Empty()) { PlotId = AddPlot(AvgV2, SeriesTy, Label+" Average", Style); }
  if (! MedV.Empty()) { PlotId = AddPlot(MedV, SeriesTy, Label+" Median", Style); }
  if (! MinV.Empty()) { PlotId = AddPlot(MinV, SeriesTy, Label+" Min", Style); }
  if (! MaxV.Empty()) { PlotId = AddPlot(MaxV, SeriesTy, Label+" Max", Style); }
  if (! StdErrV.Empty()) { PlotId = AddErrBar(StdErrV, Label+" Standard error", Style); }
  return PlotId;
}

// plot value-count tables, and pair vectors
template <class TKey, class TVal, class THashFunc>
void TGnuPlot::PlotValCntH(const THash<TKey, TVal, THashFunc>& ValCntH, const TStr& OutFNmPref, const TStr& Desc,
 const TStr& XLabel, const TStr& YLabel, const TGpScaleTy& ScaleTy, const bool& PowerFit, const TGpSeriesTy& SeriesTy, 
 const bool& PlotNCDF, const bool& ExpBucket) {
  TFltPrV IdCntV(ValCntH.Len(), 0), BucketV;
  for (int i = ValCntH.FFirstKeyId(); ValCntH.FNextKeyId(i); ) {
    IdCntV.Add(TFltPr(double(ValCntH.GetKey(i)), double(ValCntH[i]))); }
  IdCntV.Sort();
  if (ExpBucket) { 
    TGnuPlot::MakeExpBins(IdCntV, BucketV);
    BucketV.Swap(IdCntV);
  }
  if (PlotNCDF) { 
    TFltPrV NCdfV = IdCntV;
    for (int i = NCdfV.Len()-2; i >= 0; i--) {
      NCdfV[i].Val2 = NCdfV[i].Val2 + NCdfV[i+1].Val2; 
    }
    PlotValV(NCdfV, OutFNmPref, Desc, "NCDF "+XLabel, "NCDF "+YLabel, ScaleTy, PowerFit, SeriesTy);
  } else {
    PlotValV(IdCntV, OutFNmPref, Desc, XLabel, YLabel, ScaleTy, PowerFit, SeriesTy); 
  }
}

template <class TKey, class TVal, class THashFunc>
void TGnuPlot::PlotValCntH(const THash<TKey, TVal, THashFunc>& ValCntH1, const TStr& Label1, 
                           const THash<TKey, TVal, THashFunc>& ValCntH2, const TStr& Label2, 
                           const TStr& OutFNmPref, const TStr& Desc, const TStr& XLabel, const TStr& YLabel, 
                           const TGpScaleTy& ScaleTy, const TGpSeriesTy& SeriesTy) {
  PlotValCntH(ValCntH1, Label1, ValCntH2, Label2, THash<TKey, TVal, THashFunc>(), "", OutFNmPref, Desc, XLabel, YLabel,
    ScaleTy, SeriesTy);
}

template <class TKey, class TVal, class THashFunc>
void TGnuPlot::PlotValCntH(const THash<TKey, TVal, THashFunc>& ValCntH1, const TStr& Label1, 
                           const THash<TKey, TVal, THashFunc>& ValCntH2, const TStr& Label2, 
                           const THash<TKey, TVal, THashFunc>& ValCntH3, const TStr& Label3, 
                           const TStr& OutFNmPref, const TStr& Desc, const TStr& XLabel, const TStr& YLabel, 
                           const TGpScaleTy& ScaleTy, const TGpSeriesTy& SeriesTy) {
  TFltPrV IdCntV1(ValCntH1.Len(), 0), IdCntV2(ValCntH2.Len(), 0), IdCntV3(ValCntH3.Len(), 0);
  for (int i = ValCntH1.FFirstKeyId(); ValCntH1.FNextKeyId(i); ) {
    IdCntV1.Add(TFltPr(double(ValCntH1.GetKey(i)), double(ValCntH1[i]))); }
  for (int i = ValCntH2.FFirstKeyId(); ValCntH2.FNextKeyId(i); ) {
    IdCntV2.Add(TFltPr(double(ValCntH2.GetKey(i)), double(ValCntH2[i]))); }
  for (int i = ValCntH3.FFirstKeyId(); ValCntH3.FNextKeyId(i); ) {
    IdCntV3.Add(TFltPr(double(ValCntH3.GetKey(i)), double(ValCntH3[i]))); }
  IdCntV1.Sort();
  IdCntV2.Sort();
  IdCntV3.Sort();
  TGnuPlot GP(OutFNmPref, Desc);
  GP.SetXYLabel(XLabel, YLabel);
  GP.SetScale(ScaleTy);
  if (! IdCntV1.Empty()) { GP.AddPlot(IdCntV1, SeriesTy, Label1); }
  if (! IdCntV2.Empty()) { GP.AddPlot(IdCntV2, SeriesTy, Label2); }
  if (! IdCntV3.Empty()) { GP.AddPlot(IdCntV3, SeriesTy, Label3); }
  GP.SavePng();
}

template <class TVal1, class TVal2>
void TGnuPlot::PlotValV(const TVec<TPair<TVal1, TVal2> >& ValV, const TStr& OutFNmPref, const TStr& Desc,
 const TStr& XLabel, const TStr& YLabel, const TGpScaleTy& ScaleTy, const bool& PowerFit, const TGpSeriesTy& SeriesTy) {
  TFltKdV IdCntV(ValV.Len(), 0);
  for (int i = 0; i < ValV.Len(); i++) {
    IdCntV.Add(TFltKd(double(ValV[i].Val1), double(ValV[i].Val2))); }
  if (IdCntV.Empty()) { printf("*** Empty plot %s\n", OutFNmPref.CStr());  return; }
  IdCntV.Sort();
  TGnuPlot GP(OutFNmPref, Desc);
  GP.SetXYLabel(XLabel, YLabel);
  GP.SetScale(ScaleTy);
  const int Id = GP.AddPlot(IdCntV, SeriesTy);
  if (PowerFit) { 
    GP.AddPwrFit3(Id);
    double MaxY = IdCntV.Last().Dat, MinY = IdCntV[0].Dat;
    if (MaxY < MinY) { Swap(MaxY, MinY); }
    //GP.SetYRange(MinY, pow(10.0, floor(log10(MaxY))+1.0));
    GP.AddCmd(TStr::Fmt("set yrange[%f:]", MinY));
  }
  GP.SavePng();
}

template <class TVal1, class TVal2>
void TGnuPlot::PlotValV(const TVec<TPair<TVal1, TVal2> >& ValV1, const TStr& Name1, 
 const TVec<TPair<TVal1, TVal2> >& ValV2, const TStr& Name2, const TStr& OutFNmPref, const TStr& Desc,
 const TStr& XLabel, const TStr& YLabel, const TGpScaleTy& ScaleTy, const bool& PowerFit, const TGpSeriesTy& SeriesTy) {
  TFltKdV IdCntV1(ValV1.Len(), 0), IdCntV2(ValV2.Len(), 0);
  for (int i = 0; i < ValV1.Len(); i++) {
    IdCntV1.Add(TFltKd(double(ValV1[i].Val1), double(ValV1[i].Val2))); }
  for (int i = 0; i < ValV2.Len(); i++) {
    IdCntV2.Add(TFltKd(double(ValV2[i].Val1), double(ValV2[i].Val2))); }
  if (IdCntV1.Empty() || IdCntV2.Empty()) { printf("*** Empty plot %s\n", OutFNmPref.CStr());  return; }
  IdCntV1.Sort();
  IdCntV2.Sort();
  TGnuPlot GP(OutFNmPref, Desc);
  GP.SetXYLabel(XLabel, YLabel);
  GP.SetScale(ScaleTy);
  { const int Id = GP.AddPlot(IdCntV1, SeriesTy, Name1);
  if (PowerFit) { 
    GP.AddPwrFit3(Id);
    double MaxY = IdCntV1.Last().Dat, MinY = IdCntV1[0].Dat;
    if (MaxY < MinY) { Swap(MaxY, MinY); }
    GP.AddCmd(TStr::Fmt("set yrange[%f:]", MinY));
  } }
  { const int Id = GP.AddPlot(IdCntV2, SeriesTy, Name2);
  if (PowerFit) { 
    GP.AddPwrFit3(Id);
    double MaxY = IdCntV2.Last().Dat, MinY = IdCntV2[0].Dat;
    if (MaxY < MinY) { Swap(MaxY, MinY); }
    GP.AddCmd(TStr::Fmt("set yrange[%f:]", MinY));
  } }
  GP.SavePng();
}

           

template <class TVal1>
void TGnuPlot::PlotValV(const TVec<TVal1>& ValV, const TStr& OutFNmPref, const TStr& Desc,
 const TStr& XLabel, const TStr& YLabel, const TGpScaleTy& ScaleTy, const bool& PowerFit, const TGpSeriesTy& SeriesTy) {
  TFltKdV IdCntV(ValV.Len(), 0);
  for (int i = 0; i < ValV.Len(); i++) {
    IdCntV.Add(TFltKd(double(i+1), double(ValV[i]))); }
  if (IdCntV.Empty()) { printf("*** Empty plot %s\n", OutFNmPref.CStr());  return; }
  IdCntV.Sort();
  TGnuPlot GP(OutFNmPref, Desc);
  GP.SetXYLabel(XLabel, YLabel);
  GP.SetScale(ScaleTy);
  const int Id = GP.AddPlot(IdCntV, SeriesTy);
  if (PowerFit) { 
    GP.AddPwrFit3(Id);
    double MaxY = IdCntV.Last().Dat, MinY = IdCntV[0].Dat;
    if (MaxY < MinY) { Swap(MaxY, MinY); }
    //GP.SetYRange(MinY, pow(10.0, floor(log10(MaxY))+1.0));
    GP.AddCmd(TStr::Fmt("set yrange[%f:]", MinY));
  }
  GP.SavePng();
}

template <class TVal1>
void TGnuPlot::PlotValMomH(THash<TVal1, TMom>& ValMomH, const TStr& OutFNmPref, const TStr& Desc,
 const TStr& XLabel, const TStr& YLabel, const TGpScaleTy& ScaleTy, const TGpSeriesTy& SeriesTy,
 bool PlotAvg, bool PlotMed, bool PlotMin, bool PlotMax, bool PlotSDev, bool PlotStdErr) {
  TFltTrV AvgV, StdErrV;
  TFltPrV AvgV2, MedV, MinV, MaxV;
  for (int i = ValMomH.FFirstKeyId(); ValMomH.FNextKeyId(i); ) {
    ValMomH[i].Def();
    if (PlotAvg) { 
      if (PlotSDev) { 
        AvgV.Add(TFltTr((double)ValMomH.GetKey(i), ValMomH[i].GetMean(), ValMomH[i].GetSDev())); } // std deviation
      else { 
        AvgV2.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMean())); 
      }
      if (PlotStdErr) {
        StdErrV.Add(TFltTr((double)ValMomH.GetKey(i), ValMomH[i].GetMean(), ValMomH[i].GetSDev()/sqrt((double)ValMomH[i].GetVals()))); 
      }
    }
    if (PlotMed) { MedV.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMedian())); }
    if (PlotMin) { MinV.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMn())); }
    if (PlotMax) { MaxV.Add(TFltPr((double)ValMomH.GetKey(i), ValMomH[i].GetMx())); }
  }
  AvgV.Sort();  AvgV2.Sort();
  MedV.Sort();  MinV.Sort();  MaxV.Sort();  StdErrV.Sort();
  TGnuPlot GP(OutFNmPref, Desc);
  GP.SetScale(ScaleTy);
  GP.SetXYLabel(XLabel, YLabel);
  if (! AvgV.Empty()) { GP.AddErrBar(AvgV, "Average", "StdDev"); }
  if (! AvgV2.Empty()) { GP.AddPlot(AvgV2, SeriesTy, "Average"); }
  if (! MedV.Empty()) { GP.AddPlot(MedV, SeriesTy, "Median"); }
  if (! MinV.Empty()) { GP.AddPlot(MinV, SeriesTy, "Min"); }
  if (! MaxV.Empty()) { GP.AddPlot(MaxV, SeriesTy, "Max"); }
  if (! StdErrV.Empty()) { GP.AddErrBar(StdErrV, "Standard error"); }
  GP.SavePng();
}

#endif

