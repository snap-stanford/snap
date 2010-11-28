#ifndef snap_sir_h
#define snap_sir_h

#include "Snap.h"

/////////////////////////////////////////////////
// Epidemiological modeling and model fitting
class TEpidemModel {
public:
  TEpidemModel() { } // const double& NZero, const double& IZero) : N0(NZero), I0(IZero)  { }
  
  virtual void GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV) = 0;
  virtual TStr GetStr() const { return TStr(); }
  virtual double RunCalcErr(const TFltV& ParamV, const int& StepsPerTm) { Fail; return -1; }
  virtual void GetParam(TFltV& ParamV) const = 0;
  virtual void SetParam(const TFltV& ParamV) = 0;
  //virtual bool GetNextSample(const TFltV& ParamV, TFltV& NewParamV) = 0;*/

  void RunModel(const TFltV& StartValV, const double& StartT, const double& StopT, const int& NSteps, TVec<TFltV>& OutValV);
  void RunModel10(const TFltV& StartValV, const double& StartT, const double& StopT, const int& NSteps, TVec<TFltV>& OutValV);
  void RunEuler(const TFltV& StartValV, const double& StartT, const double& StopT, const int& NSteps, TVec<TFltV>& OutValV);
  void RungeKutta(const TFltV& y, const TFltV& dydx, double x, double h, TFltV& SirOutV);
  static double GetErr(const TFltV& TrueV, const TFltV& SimV, const int& SimT0);
  static void Plot(const TFltV& TrueV, const TStr& Label1, const TFltV& SimV, const TStr& Label2, 
    const TStr& OutFNm, const TStr& Desc, const TStr& XLabel, const TStr& YLabel);
  static void LoadTxt(const TStr& InFNm, const int& ColId, TFltV& ValV);
};

/////////////////////////////////////////////////
// Epidemiological model parameter estimation
template <class TEpiModel>
class TEstEpidemParams {
public:
  TRnd Rnd;
  TEpiModel Model;
  TFltTrV ParamMnMxStepV;
  TFltV ParamV, NewParamV, BestParamV;
  double CurErr, BestErr;
  // statistics
  TVec<THash<TFlt,TInt> > ParamH; // posterior distribution histograms
public:
  TEstEpidemParams(const TEpiModel& EpiModel) : Rnd(0), Model(EpiModel) { }
  bool GetNextSample();
  void EstParams(const int& NIter);
  double PlotBest(const TStr& OutFNm, TStr Desc="");
  void PlotParamHist(const TStr& OutFNm, TStr Desc="");
};

template <class TEpiModel>
bool TEstEpidemParams<TEpiModel>::GetNextSample() {
  for (int p = 0; p < NewParamV.Len(); p++) {
    if (ParamMnMxStepV[p].Val3 <= 0) { continue; } 
    //if (p == 10) { printf("%f  + %f           %f\n", ParamV[p],  ParamMnMxStepV[p].Val3*Rnd.GetNrmDev(), ParamMnMxStepV[p].Val3); }
    NewParamV[p] = ParamV[p] + ParamMnMxStepV[p].Val3*Rnd.GetNrmDev();
    if (NewParamV[p] < ParamMnMxStepV[p].Val1) { NewParamV[p]=ParamMnMxStepV[p].Val1; }  // min
    if (NewParamV[p] > ParamMnMxStepV[p].Val2) { NewParamV[p]=ParamMnMxStepV[p].Val2; }  // max
    //printf("\t%10g  -> %10g\n", ParamV[p], NewParamV[p]);
  }
  const double NewErr = Model.RunCalcErr(NewParamV);
  if (NewErr < BestErr || BestErr<1) {
    BestErr = NewErr;
    BestParamV = NewParamV;
  }
  const double E = exp((CurErr-NewErr)/(4*BestErr)); // 0.2 for Sir2Pop model
  if (Rnd.GetUniDev() < E || CurErr<1) { // accept
    CurErr = NewErr;
    ParamV = NewParamV;
    return true;
  } else { // reject
    return false;
  }
}

template <class TEpiModel>
void TEstEpidemParams<TEpiModel>::EstParams(const int& NIter) {
  Model.GetParam(ParamV);
  NewParamV = ParamV;
  BestParamV = ParamV;
  CurErr = -1;
  BestErr = -1;
  Model.GetParamRange(ParamMnMxStepV);
  ParamH.Gen(ParamV.Len());
  TExeTm ExeTm;
  int nAcc=0, plot=0;
  for (int iter = 0; iter < NIter; iter++) {
    const bool Accept = GetNextSample();
    if (Accept) { nAcc++; }
    for (int p=0; p<ParamV.Len(); p++) { 
      const double Res = (ParamMnMxStepV[p].Val2-ParamMnMxStepV[p].Val1)/50.0;
      ParamH[p].AddDat(TMath::Round(ParamV[p]/Res, 1)*Res) += 1;
    }
    if (iter % (NIter/100) == 0) {
      printf("%dk]", iter/1000); 
      for (int p=0; p<ParamV.Len(); p++) { printf(" %g", ParamV[p]); }
      printf("\tE:%g B:%g  A:%.3f [%s]\n", CurErr, BestErr, nAcc/double(iter+1), ExeTm.GetStr());
      if (iter % (NIter/10) == 0) {
       //Model.RunCalcErr(ParamV);  Model.Plot(TStr::Fmt("est-param-%02d", ++plot)); 
      }
    }
  }
  printf("\ndone. Accept %f\n\n", nAcc/double(NIter));
}

template <class TEpiModel>
double TEstEpidemParams<TEpiModel>::PlotBest(const TStr& OutFNm, TStr Desc) {
  const double E = Model.RunCalcErr(BestParamV);
  Model.Plot(OutFNm, Desc);
  return E;
}

template <class TEpiModel>
void TEstEpidemParams<TEpiModel>::PlotParamHist(const TStr& OutFNm, TStr Desc) {
  TStrV ParamNmV; Model.GetParamNmV(ParamNmV);
  for (int p = 0; p < ParamH.Len(); p++) {
    TGnuPlot::PlotValCntH(ParamH[p], "param-"+ParamNmV[p]+"-"+OutFNm, Desc+" "+Model.GetStr(), ParamNmV[p], "Count");
  }
}

/////////////////////////////////////////////////
// Basic SIR model (single population)
class TSirModel : public TEpidemModel {
public:
  double N0, I0;
  TFltTrV ParamMnMnRngV;
  double Beta, Gamma;
  int T0;
  TFltV TrueInfectV;
  TVec<TFltV> OutValV; // S, I
public:
  TSirModel(double NZero, TFltTr NZeroP, double IZero, TFltTr IZeroP, double _Beta, TFltTr BetaP, double _Gamma, TFltTr GammaP, 
    int TZero, TFltTr TZeroP, TStr InFNm, int ColId);
  TStr GetStr() const { return TStr::Fmt("N0:%g  I0:%g  T0:%d  Beta:%g  Gamma:%g. Err: %f", N0, I0, T0, Beta, Gamma, GetErr()); }
  void SetPeakT0();
  void GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV);
  double GetErr() const;
  double RunCalcErr(const TFltV& ParamV);
  void GetParam(TFltV& ParamV) const;// { ParamV.Clr(false); ParamV.Add(Beta); ParamV.Add(Gamma); ParamV.Add(T0); }
  void SetParam(const TFltV& ParamV);  // { Beta=ParamV[0]; Gamma=ParamV[1]; T0=int(ParamV[2]); }
  void GetParamRange(TFltTrV& ParamMnMxStepV) const;
  void GetParamNmV(TStrV& ParamNmV) const;
  void Plot(const TStr& OutFNm, TStr Desc="") const;
  static void TestEurlerVsRk();
};

/////////////////////////////////////////////////
// 2 populations SIR model (with cross infection probabilities)
class TSir2Model : public TEpidemModel {
public:
  double N0M, I0M, N0B, I0B;
  int T0;
  double BetaM, GammaM, BetaB, GammaB, BetaMB, BetaBM;
  TFltTrV ParamMnMnRngV;
  TFltV MediaV, BlogV;
  TVec<TFltV> OutValV;
public:
  TSir2Model() { }
  TSir2Model(double _N0M, TFltTr N0MP, double _I0M, TFltTr I0MP, double _N0B, TFltTr N0BP, double _I0B, TFltTr I0BP, 
    int _T0, TFltTr T0P, double _BetaM, TFltTr BetaMP, double _GammaM, TFltTr GammaMP, double _BetaB, TFltTr BetaBP, 
    double _GammaB, TFltTr GammaBP, double _BetaMB, TFltTr BetaMBP, double _BetaBM, TFltTr BetaBMP, TStr InFNm, int ColId1, int ColId2);
  TStr GetStr() const;
  void SetPeakT0();
  void SetMediaBlogV(const TFltPrV& _MediaV, const TFltPrV& _BlogV);
  void GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV);
  double GetErr() const;
  double RunCalcErr(const TFltV& ParamV);
  void GetParam(TFltV& ParamV) const;
  void SetParam(const TFltV& ParamV);
  void GetParamRange(TFltTrV& ParamMnMxStepV) const;
  void GetParamNmV(TStrV& ParamNmV) const;
  void Plot(const TStr& OutFNm, TStr Desc="") const;
};

/////////////////////////////////////////////////
// 2 populations SIR model, where with rate Delta S --> R 
//   (susceptible people also die with rate Delta)
class TSirSR2Model : public TEpidemModel {
public:
  double N0M, I0M, N0B, I0B;
  int T0;
  double BetaM, GammaM, BetaB, GammaB, BetaMB, BetaBM;
  double DeltaM, DeltaB;
  TFltTrV ParamMnMnRngV;
  TFltV MediaV, BlogV;
  TVec<TFltV> OutValV;
public:
  TSirSR2Model(double _N0M, TFltTr N0MP, double _I0M, TFltTr I0MP, double _N0B, TFltTr N0BP, double _I0B, TFltTr I0BP, 
    int _T0, TFltTr T0P, double _BetaM, TFltTr BetaMP, double _GammaM, TFltTr GammaMP, double _BetaB, TFltTr BetaBP, 
    double _GammaB, TFltTr GammaBP, double _BetaMB, TFltTr BetaMBP, double _BetaBM, TFltTr BetaBMP, 
    double _DeltaM, TFltTr DeltaMP, double _DeltaB, TFltTr DeltaBP, TStr InFNm, int ColId1, int ColId2);
  TStr GetStr() const;
  void SetPeakT0();
  void SetMediaBlogV(const TFltPrV& _MediaV, const TFltPrV& _BlogV);
  void GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV);
  double GetErr() const;
  double RunCalcErr(const TFltV& ParamV);
  void GetParam(TFltV& ParamV) const;
  void SetParam(const TFltV& ParamV);
  void GetParamRange(TFltTrV& ParamMnMxStepV) const;
  void GetParamNmV(TStrV& ParamNmV) const;
  void Plot(const TStr& OutFNm, TStr Desc="") const;
};


#endif
