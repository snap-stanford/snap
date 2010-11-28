#include "stdafx.h"
#include "sir.h"

/////////////////////////////////////////////////
// Epidemiological modeling and model fitting
void TEpidemModel::RunModel(const TFltV& StartValV, const double& StartT, const double& StopT, const int& NSteps, TVec<TFltV>& OutValV) {
  TFltV ValV(StartValV), dydx(StartValV.Len()), ValV2(StartValV.Len());
  OutValV.Clr(false);
  for (int v = 0; v < StartValV.Len(); v++) {
    OutValV.Add();
    OutValV[v].Clr(false);
    OutValV[v].Add(StartValV[v]);
  }
  const double h = (StopT-StartT) / NSteps;
  double x = StartT;
  for (int k = 0; k < NSteps; k++) {
    GetDerivs(x, ValV, dydx);
    RungeKutta(ValV, dydx, x, h, ValV2);
    for (int v = 0; v < ValV2.Len(); v++) {
      double X = ValV2[v];
      if (X < 0 || _isnan(X) || !_finite(X)) { X = 0; }
      OutValV[v].Add(X);
    }
    ValV = ValV2;
    x += h;
  }
}

// run the model internally with 10 times greater resolution
void TEpidemModel::RunModel10(const TFltV& StartValV, const double& StartT, const double& StopT, const int& NSteps, TVec<TFltV>& OutValV) {
  TFltV ValV(StartValV), dydx(StartValV.Len()), ValV2(StartValV.Len());
  OutValV.Clr(false);
  for (int v = 0; v < StartValV.Len(); v++) {
    OutValV.Add();
    OutValV[v].Clr(false);
    OutValV[v].Add(StartValV[v]);
  }
  const double h = (StopT-StartT) / (10*NSteps);
  double x = StartT;
  for (int k = 0; k < 10*NSteps; k++) {
    GetDerivs(x, ValV, dydx);
    RungeKutta(ValV, dydx, x, h, ValV2);
    // take values at only every 10th step
    if (k % 10 == 0) {
      for (int v = 0; v < ValV2.Len(); v++) {
        OutValV[v].Add(ValV2[v]); }
    }
    ValV = ValV2;
    x += h;
  }
}

void TEpidemModel::RunEuler(const TFltV& StartValV, const double& StartT, const double& StopT, const int& NSteps, TVec<TFltV>& OutValV) {
  const double h = (StopT-StartT) / NSteps;
  TFltV ValV(StartValV), dydx(StartValV.Len()), ValV2(StartValV.Len());
  for (int v = 0; v < StartValV.Len(); v++) {
    OutValV.Add();
    OutValV[v].Clr(false);
    OutValV[v].Add(StartValV[v]);
  }
  OutValV.Add(); // x values
  OutValV.Last().Add(StartT);
  for (double x = StartT; x <= StopT; x += h) {
    GetDerivs(x, ValV, dydx);
    for (int v = 0; v < ValV.Len(); v++) {
      ValV[v] += h*dydx[v];
      OutValV[v].Add(ValV[v]);
    }
    OutValV.Last().Add(x+h);
  }
  for (int v = 1; v < OutValV.Len(); v++) {
    IAssert(OutValV[v].Len()==OutValV[v-1].Len());
  }
}

void TEpidemModel::RungeKutta(const TFltV& y, const TFltV& dydx, double x, double h, TFltV& SirOutV) {
  const int n = y.Len();
  IAssert(y.Len() == n && dydx.Len() == n);
  TFltV dym(n), dyt(n), yt(n);
  int i;
  double hh=h*0.5;
  double h6=h/6.0;
  double xh=x+hh;
  for (i=0; i < n; i++) {
    yt[i]=y[i]+hh*dydx[i];
  }
  GetDerivs(xh, yt, dyt);
  for (i=0; i<n; i++) {
    yt[i]=y[i]+hh*dyt[i];
  }
  GetDerivs(xh,yt,dym);
  for (i=0; i<n; i++) {
	  yt[i]=y[i]+h*dym[i];
	  dym[i] += dyt[i];
  }
  GetDerivs(x+h,yt,dyt);
  SirOutV.Clr(false);  
  for (i=0; i<n; i++) {
    SirOutV.Add(y[i]+h6 * (dydx[i]+dyt[i]+2.0*dym[i]));
  }
}

double TEpidemModel::GetErr(const TFltV& TrueV, const TFltV& SimV, const int& SimT0) { 
  if (SimV.Empty() || TrueV.Empty()) { return -1.0; }
  double Err = 0.0;
  double S=0;
  for (int t=0; t < TrueV.Len(); t++) {
    if (t-SimT0 >= 0) { S = SimV[t-SimT0]; } else { S = 0; }
    Err += TMath::Sqr(TrueV[t]-S);
  }
  if (Err <= 0) { Err = TFlt::Mx; }
  return Err;
}


void TEpidemModel::Plot(const TFltV& TrueV, const TStr& Label1, const TFltV& SimV, const TStr& Label2, const TStr& OutFNm, const TStr& Desc, const TStr& XLabel, const TStr& YLabel) {
  TGnuPlot GP(OutFNm, Desc);
  GP.AddPlot(TrueV, gpwLinesPoints, Label1);
  GP.AddPlot(SimV, gpwLinesPoints, Label2);
  GP.SetXYLabel(XLabel, YLabel);
  GP.SavePng();
  //GP.SaveEps(12);
}

void TEpidemModel::LoadTxt(const TStr& InFNm, const int& ColId, TFltV& ValV) {
  ValV.Clr();
  if (! TFile::Exists(InFNm)) { 
    printf("*** %s not found!\n", InFNm.CStr());
    return; 
  }
  TSsParser Ss(InFNm, ssfTabSep);
  while (Ss.Next()) {
    ValV.Add(Ss.GetFlt(ColId));
  }
}

/////////////////////////////////////////////////
// Basic SIR model
TSirModel::TSirModel(double NZero, TFltTr NZeroP, double IZero, TFltTr IZeroP, double _Beta, TFltTr BetaP, double _Gamma, TFltTr GammaP, int TZero, TFltTr TZeroP, TStr InFNm, int ColId) :
 N0(NZero), I0(IZero), Beta(_Beta), Gamma(_Gamma), T0(TZero) { 
  ParamMnMnRngV.Add(NZeroP);
  ParamMnMnRngV.Add(IZeroP);
  ParamMnMnRngV.Add(BetaP);
  ParamMnMnRngV.Add(GammaP);
  ParamMnMnRngV.Add(TZeroP);
  TEpidemModel::LoadTxt(InFNm, ColId, TrueInfectV); 
}

void TSirModel::GetParam(TFltV& ParamV) const { 
  ParamV.Clr(false); 
  ParamV.Add(N0);
  ParamV.Add(I0);
  ParamV.Add(Beta); 
  ParamV.Add(Gamma); 
  ParamV.Add(T0);
}

void TSirModel::SetParam(const TFltV& ParamV) { 
  N0 = ParamV[0];
  I0 = ParamV[1];
  Beta = ParamV[2]; 
  Gamma = ParamV[3]; 
  T0 = int(ParamV[4]); 
}

void TSirModel::SetPeakT0() {
  double Max =0;
  int Peak=0;
  //TMom PeakMom;
  for (int i = 0; i < TrueInfectV.Len(); i++) {
    if (TrueInfectV[i] > Max) { 
      Max=TrueInfectV[i]; 
      Peak=i;
    }
    //PeakMom.Add(i, TrueInfectV[i]);
  }
  //PeakMom.Def();
  //printf("Set peak T0 = %d \t %f\n", Peak, PeakMom.GetMedian());
  //T0 = (int) PeakMom.GetMedian();
  T0 = int(0.9*Peak);
  ParamMnMnRngV[4].Val1 = Peak - 0.5*abs(Peak); // min
  ParamMnMnRngV[4].Val2 = Peak + 0.5*abs(Peak); // max
  printf("Set peak T0 = %d (%g -- %g)\n", Peak, ParamMnMnRngV[4].Val1, ParamMnMnRngV[4].Val2);

}

void TSirModel::GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV) {
  //S = S - Beta*I*S;
  dxdyV[0] = -Beta*SirValV[0]*SirValV[1];
  //I = I + Beta*I*S - Gamma*I;
  dxdyV[1] = Beta*SirValV[0]*SirValV[1] - Gamma*SirValV[1];
}

double TSirModel::GetErr() const { 
  return TEpidemModel::GetErr(TrueInfectV, OutValV[1], T0);
}

double TSirModel::RunCalcErr(const TFltV& ParamV) {
  SetParam(ParamV);
  const int EndT = TrueInfectV.Len() - T0 + 2;
  RunModel(TFltV::GetV(N0-I0, I0), 0, EndT, EndT, OutValV);
  //RunModel10(TFltV::GetV(N0-I0, I0), 0, EndT, EndT, OutValV);
  return GetErr();
}

void TSirModel::GetParamRange(TFltTrV& ParamMnMxStepV) const {
  ParamMnMxStepV = ParamMnMnRngV; // (min, max, resolution)
}

void TSirModel::GetParamNmV(TStrV& ParamNmV) const {
  ParamNmV = TStrV::GetV("Beta", "Gamma");
}

void TSirModel::Plot(const TStr& OutFNm, TStr Desc) const {
  if (OutValV.Empty()) { printf("EMPTY()");  return; }
  TFltV V2;
  for (int t=0; t < TrueInfectV.Len(); t++) {
    if (t-T0 >= 0) { V2.Add(OutValV[1][t-T0]); } else { V2.Add(0); }
  }
  TEpidemModel::Plot(TrueInfectV, "TRUE", V2, "SIR model", OutFNm, Desc+" "+GetStr(), "Time", "Number of infected nodes");
}

void TSirModel::TestEurlerVsRk() {
  /*TSirModel Sir("", Mega(1), 1, 1e-6, TFltTr(), 0.2, TFltTr(), 0, TFltTr());
  TVec<TFltV> OutV, OutV2, OutV3, OutV4, OutV6, OutV5;
  Sir.RunModel(TFltV::GetV(1e6, 1), 0, 50, 50, OutV);  
  Sir.RunEuler(TFltV::GetV(1e6, 1), 0, 50, 50, OutV2);  
  Sir.RunModel(TFltV::GetV(1e6, 1), 0, 50, 500, OutV3);  
  Sir.RunEuler(TFltV::GetV(1e6, 1), 0, 50, 500, OutV4);  
  Sir.RunModel(TFltV::GetV(1e6, 1), 0, 50, 5000, OutV5);  
  Sir.RunEuler(TFltV::GetV(1e6, 1), 0, 50, 5000, OutV6);  
  TGnuPlot GP("SIR-rk-test", Sir.GetStr());
  GP.AddPlot(OutV.Last(), OutV[1], gpwLines, "RungeKutta, 50 steps");
  GP.AddPlot(OutV2.Last(), OutV2[1], gpwLines, "Euler method, 50 steps");
  GP.AddPlot(OutV3.Last(), OutV3[1], gpwLines, "RungeKutta, 500 steps");
  GP.AddPlot(OutV4.Last(), OutV4[1], gpwLines, "Euler method, 500 steps");
  GP.AddPlot(OutV5.Last(), OutV5[1], gpwLines, "RungeKutta, 5000 steps");
  GP.AddPlot(OutV6.Last(), OutV6[1], gpwLines, "Euler method, 5000 steps");
  GP.SavePng();*/
}



/////////////////////////////////////////////////
// 2 populations SIR model
TSir2Model::TSir2Model(double _N0M, TFltTr N0MP, double _I0M, TFltTr I0MP, double _N0B, TFltTr N0BP, double _I0B, TFltTr I0BP, 
  int _T0, TFltTr T0P, double _BetaM, TFltTr BetaMP, double _GammaM, TFltTr GammaMP, double _BetaB, TFltTr BetaBP, double _GammaB, TFltTr GammaBP, 
  double _BetaMB, TFltTr BetaMBP, double _BetaBM, TFltTr BetaBMP, TStr InFNm, int ColId1, int ColId2) : N0M(_N0M), I0M(_I0M), N0B(_N0B), I0B(_I0B), T0(_T0), BetaM(_BetaM), 
  GammaM(_GammaM), BetaB(_BetaB), GammaB(_GammaB), BetaMB(_BetaMB), BetaBM(_BetaBM) { 
  ParamMnMnRngV.Add(N0MP);
  ParamMnMnRngV.Add(I0MP);
  ParamMnMnRngV.Add(N0BP);
  ParamMnMnRngV.Add(I0BP);
  ParamMnMnRngV.Add(T0P);
  ParamMnMnRngV.Add(BetaMP);
  ParamMnMnRngV.Add(GammaMP);
  ParamMnMnRngV.Add(BetaBP);
  ParamMnMnRngV.Add(GammaBP);
  ParamMnMnRngV.Add(BetaMBP);
  ParamMnMnRngV.Add(BetaBMP);
  if (InFNm.Len()>0 && TFile::Exists(InFNm)) {
    TEpidemModel::LoadTxt(InFNm, ColId1, MediaV); 
    TEpidemModel::LoadTxt(InFNm, ColId2, BlogV); 
  }
}

void TSir2Model::GetParam(TFltV& ParamV) const { 
  ParamV.Clr(false); 
  ParamV.Add(N0M);
  ParamV.Add(I0M);
  ParamV.Add(N0B);
  ParamV.Add(I0B);
  ParamV.Add(T0);
  ParamV.Add(BetaM);
  ParamV.Add(GammaM);
  ParamV.Add(BetaB);
  ParamV.Add(GammaB);
  ParamV.Add(BetaMB);
  ParamV.Add(BetaBM);
}

void TSir2Model::SetParam(const TFltV& ParamV) { 
  N0M = ParamV[0];
  I0M = ParamV[1];
  N0B = ParamV[2];
  I0B = ParamV[3];
  T0 = (int) ParamV[4];
  BetaM = ParamV[5];
  GammaM = ParamV[6];
  BetaB = ParamV[7];
  GammaB = ParamV[8];
  BetaMB = ParamV[9];
  BetaBM = ParamV[10];
}

TStr TSir2Model::GetStr() const { 
  return TStr::Fmt("N0:%g %g  I0:%g %g  T0:%d  Beta:%g %g  Gamma:%g %g M-B:%g B-M:%g Err: %.2f", 
    N0M, N0B, I0M, I0B, T0, BetaM, BetaB, GammaM, GammaB, BetaMB, BetaBM, GetErr()); 
}

void TSir2Model::SetPeakT0() {
  double Max1=0, Max2=0;
  int Peak1=0, Peak2=0;
  for (int i = 0; i < MediaV.Len(); i++) {
    if (MediaV[i] > Max1) { 
      Max1 = MediaV[i]; Peak1 = i; }
  }
  for (int i = 0; i < BlogV.Len(); i++) {
    if (BlogV[i] > Max2) { 
      Max2 = BlogV[i]; Peak2 = i; }
  }
  int Peak = TMath::Mn(Peak1, Peak2);
  T0 = int(0.9*Peak);
  ParamMnMnRngV[4].Val1 = Peak - 0.5*Peak; // min
  ParamMnMnRngV[4].Val2 = TMath::Mx(Peak1, Peak2) + 0.5*TMath::Mx(Peak1, Peak2); // max
  printf("Set peak T0 = %d (%g -- %g) p1:%d p2:%d\n", 
    Peak, ParamMnMnRngV[4].Val1, ParamMnMnRngV[4].Val2, Peak1, Peak2);
}

void TSir2Model::SetMediaBlogV(const TFltPrV& _MediaV, const TFltPrV& _BlogV) {
  IAssert(_MediaV.Len() == _BlogV.Len());
  MediaV.Clr(false);  BlogV.Clr(false);
  for (int i = 0; i < _MediaV.Len(); i++) {
    MediaV.Add(_MediaV[i].Val2);
    BlogV.Add(_BlogV[i].Val2);
  }
}

void TSir2Model::GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV) {
  const TFlt& SM = SirValV[0];
  const TFlt& SB = SirValV[1];
  const TFlt& IM = SirValV[2];
  const TFlt& IB = SirValV[3];
  //SM = SM - BetaM*IM*SM - BetaBM*IB*SM;
  dxdyV[0] = -BetaM*IM*SM - BetaBM*IB*SM;
  //SB = SB - BetaB*IB*SB - BetaMB*IM*SB;
  dxdyV[1] = -BetaB*IB*SB - BetaMB*IM*SB;
  //IM = IM + BetaM*IM*SM + BetaBM*IB*SM - GammaM*IM;
  dxdyV[2] = BetaM*IM*SM + BetaBM*IB*SM - GammaM*IM;
  //IB = IB + BetaB*IB*SB + BetaMB*IM*SB - GammaB*IB;
  dxdyV[3] = BetaB*IB*SB + BetaMB*IM*SB - GammaB*IB;
}

double TSir2Model::GetErr() const { 
  return TEpidemModel::GetErr(MediaV, OutValV[2], T0) + TEpidemModel::GetErr(BlogV, OutValV[3], T0);
}

double TSir2Model::RunCalcErr(const TFltV& ParamV) {
  SetParam(ParamV);
  const int EndT = BlogV.Len() - T0 + 2;
  RunModel(TFltV::GetV(N0M-I0M, N0B-I0B, I0M, I0B), 0, EndT, EndT, OutValV);
  //RunModel10(TFltV::GetV(N0M-I0M, N0B-I0B, I0M, I0B), 0, EndT, EndT, OutValV);
  return GetErr();
}

void TSir2Model::GetParamRange(TFltTrV& ParamMnMxStepV) const {
  ParamMnMxStepV = ParamMnMnRngV; // (min, max, resolution)
}

void TSir2Model::GetParamNmV(TStrV& ParamNmV) const {
  TStr("N0_media,I0_media,N0_blogs,I0_blogs,T0,BetaM,GammaM,BetaB,GammaB,BetaMB,BetaBM").SplitOnAllCh(',', ParamNmV);
}

void TSir2Model::Plot(const TStr& OutFNm, TStr Desc) const {
  if (OutValV.Empty()) { printf("EMPTY()");  return; }
  TFltV MV, BV;
  const TFltV& SMediaV = OutValV[2];
  for (int t=0; t < MediaV.Len(); t++) {
    if (t-T0 >= 0) { MV.Add(SMediaV[t-T0]); } else { MV.Add(0); }
  }
  const TFltV& SBlogV = OutValV[3];
  for (int t=0; t < BlogV.Len(); t++) {
    if (t-T0 >= 0) { BV.Add(SBlogV[t-T0]); } else { BV.Add(0); }
  }
  TGnuPlot GP(OutFNm, Desc+" "+GetStr());
  GP.AddPlot(MediaV, gpwLines, "TRUE MEDIA");
  GP.AddPlot(MV, gpwLinesPoints, "SIM MEDIA");
  GP.AddPlot(BlogV, gpwLines, "TRUE BLOGS");
  GP.AddPlot(BV, gpwLinesPoints, "SIM BLOGS");
  GP.SetXYLabel("Time [hours]", "Number of infected nodes");
  GP.SavePng();
}

/////////////////////////////////////////////////
// 2 populations SIR model, where with rate Delta S --> R
//   (susceptible people also die with rate Delta)
TSirSR2Model::TSirSR2Model(double _N0M, TFltTr N0MP, double _I0M, TFltTr I0MP, double _N0B, TFltTr N0BP, double _I0B, TFltTr I0BP, 
 int _T0, TFltTr T0P, double _BetaM, TFltTr BetaMP, double _GammaM, TFltTr GammaMP, double _BetaB, TFltTr BetaBP, 
 double _GammaB, TFltTr GammaBP, double _BetaMB, TFltTr BetaMBP, double _BetaBM, TFltTr BetaBMP, 
 double _DeltaM, TFltTr DeltaMP, double _DeltaB, TFltTr DeltaBP, TStr InFNm, int ColId1, int ColId2) :  N0M(_N0M), I0M(_I0M), N0B(_N0B), 
 I0B(_I0B), T0(_T0), BetaM(_BetaM), GammaM(_GammaM), BetaB(_BetaB), GammaB(_GammaB), BetaMB(_BetaMB), BetaBM(_BetaBM),
 DeltaM(_DeltaM), DeltaB(DeltaB) { 
  ParamMnMnRngV.Add(N0MP);
  ParamMnMnRngV.Add(I0MP);
  ParamMnMnRngV.Add(N0BP);
  ParamMnMnRngV.Add(I0BP);
  ParamMnMnRngV.Add(T0P);
  ParamMnMnRngV.Add(BetaMP);
  ParamMnMnRngV.Add(GammaMP);
  ParamMnMnRngV.Add(BetaBP);
  ParamMnMnRngV.Add(GammaBP);
  ParamMnMnRngV.Add(BetaMBP);
  ParamMnMnRngV.Add(BetaBMP);
  ParamMnMnRngV.Add(DeltaMP);
  ParamMnMnRngV.Add(DeltaBP);
  if (InFNm.Len()>0 && TFile::Exists(InFNm)) {
    TEpidemModel::LoadTxt(InFNm, ColId1, MediaV); 
    TEpidemModel::LoadTxt(InFNm, ColId2, BlogV); 
  }
}

void TSirSR2Model::GetParam(TFltV& ParamV) const { 
  ParamV.Clr(false); 
  ParamV.Add(N0M);
  ParamV.Add(I0M);
  ParamV.Add(N0B);
  ParamV.Add(I0B);
  ParamV.Add(T0);
  ParamV.Add(BetaM);
  ParamV.Add(GammaM);
  ParamV.Add(BetaB);
  ParamV.Add(GammaB);
  ParamV.Add(BetaMB);
  ParamV.Add(BetaBM);
  ParamV.Add(DeltaM);
  ParamV.Add(DeltaB);
}

void TSirSR2Model::SetParam(const TFltV& ParamV) { 
  N0M = ParamV[0];
  I0M = ParamV[1];
  N0B = ParamV[2];
  I0B = ParamV[3];
  T0 = (int) ParamV[4];
  BetaM = ParamV[5];
  GammaM = ParamV[6];
  BetaB = ParamV[7];
  GammaB = ParamV[8];
  BetaMB = ParamV[9];
  BetaBM = ParamV[10];
  DeltaM = ParamV[11];
  DeltaB = ParamV[12];
}

TStr TSirSR2Model::GetStr() const { 
  return TStr::Fmt("N0:%g %g  I0:%g %g  T0:%d  Beta:%g %g  Gamma:%g %g M-B:%g B-M:%g  Delta:%g %g    ERR: %.2f", 
    N0M, N0B, I0M, I0B, T0, BetaM, BetaB, GammaM, GammaB, BetaMB, BetaBM, DeltaM, DeltaB, GetErr()); 
}

void TSirSR2Model::SetPeakT0() {
  double Max1=0, Max2=0;
  int Peak1=0, Peak2=0;
  for (int i = 0; i < MediaV.Len(); i++) {
    if (MediaV[i] > Max1) { 
      Max1 = MediaV[i]; Peak1 = i; }
  }
  for (int i = 0; i < BlogV.Len(); i++) {
    if (BlogV[i] > Max2) { 
      Max2 = BlogV[i]; Peak2 = i; }
  }
  int Peak = TMath::Mn(Peak1, Peak2);
  T0 = int(0.9*Peak);
  ParamMnMnRngV[4].Val1 = Peak - 0.5*Peak; // min
  ParamMnMnRngV[4].Val2 = TMath::Mx(Peak1, Peak2) + 0.5*TMath::Mx(Peak1, Peak2); // max
  printf("Set peak T0 = %d (%g -- %g) p1:%d p2:%d\n", 
    Peak, ParamMnMnRngV[4].Val1, ParamMnMnRngV[4].Val2, Peak1, Peak2);
}

void TSirSR2Model::SetMediaBlogV(const TFltPrV& _MediaV, const TFltPrV& _BlogV) {
  IAssert(_MediaV.Len() == _BlogV.Len());
  MediaV.Clr(false);  BlogV.Clr(false);
  for (int i = 0; i < _MediaV.Len(); i++) {
    MediaV.Add(_MediaV[i].Val2);
    BlogV.Add(_BlogV[i].Val2);
  }
}

void TSirSR2Model::GetDerivs(const double& T, const TFltV& SirValV, TFltV& dxdyV) {
  const TFlt& SM = SirValV[0];
  const TFlt& SB = SirValV[1];
  const TFlt& IM = SirValV[2];
  const TFlt& IB = SirValV[3];
  // susceptible people go away
  //SM = SM - BetaM*IM*SM - BetaBM*IB*SM;
  dxdyV[0] = -BetaM*IM*SM - BetaBM*IB*SM - DeltaM*SM;
  //SB = SB - BetaB*IB*SB - BetaMB*IM*SB;
  dxdyV[1] = -BetaB*IB*SB - BetaMB*IM*SB - DeltaB*SB;
  // people get infected
  //IM = IM + BetaM*IM*SM + BetaBM*IB*SM - GammaM*IM;
  dxdyV[2] = BetaM*IM*SM + BetaBM*IB*SM - GammaM*IM;
  //IB = IB + BetaB*IB*SB + BetaMB*IM*SB - GammaB*IB;
  dxdyV[3] = BetaB*IB*SB + BetaMB*IM*SB - GammaB*IB;
}

double TSirSR2Model::GetErr() const { 
  return TEpidemModel::GetErr(MediaV, OutValV[2], T0) + TEpidemModel::GetErr(BlogV, OutValV[3], T0);
}

double TSirSR2Model::RunCalcErr(const TFltV& ParamV) {
  SetParam(ParamV);
  const int EndT = BlogV.Len() - T0 + 2;
  RunModel(TFltV::GetV(N0M-I0M, N0B-I0B, I0M, I0B), 0, EndT, EndT, OutValV);
  //RunModel10(TFltV::GetV(N0M-I0M, N0B-I0B, I0M, I0B), 0, EndT, EndT, OutValV);
  return GetErr();
}

void TSirSR2Model::GetParamRange(TFltTrV& ParamMnMxStepV) const {
  ParamMnMxStepV = ParamMnMnRngV; // (min, max, resolution)
}

void TSirSR2Model::GetParamNmV(TStrV& ParamNmV) const {
  TStr("N0_media,I0_media,N0_blogs,I0_blogs,T0,BetaM,GammaM,BetaB,GammaB,BetaMB,BetaBM,DeltaM,DeltaB").SplitOnAllCh(',', ParamNmV);
}

void TSirSR2Model::Plot(const TStr& OutFNm, TStr Desc) const {
  if (OutValV.Empty()) { printf("EMPTY()");  return; }
  TFltV MV, BV;
  const TFltV& SMediaV = OutValV[2];
  for (int t=0; t < MediaV.Len(); t++) {
    if (t-T0 >= 0) { MV.Add(SMediaV[t-T0]); } else { MV.Add(0); }
  }
  const TFltV& SBlogV = OutValV[3];
  for (int t=0; t < BlogV.Len(); t++) {
    if (t-T0 >= 0) { BV.Add(SBlogV[t-T0]); } else { BV.Add(0); }
  }
  TGnuPlot GP(OutFNm, Desc+" "+GetStr());
  GP.AddPlot(MediaV, gpwLines, "TRUE MEDIA");
  GP.AddPlot(MV, gpwLinesPoints, "SIM MEDIA");
  GP.AddPlot(BlogV, gpwLines, "TRUE BLOGS");
  GP.AddPlot(BV, gpwLinesPoints, "SIM BLOGS");
  GP.SetXYLabel("Time [hours]", "Number of infected nodes");
  GP.SavePng();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class TSir2Pop {
public:
  double I0M, I0B;
  double BetaM, BetaMB, BetaBM, BetaB;
  double GammaM, GammaB;
  TFltV TrueB, TrueM, SimM, SimB;
public:
  TSir2Pop(TStr InFNm, double NM, double NB) {
    for (TSsParser Ss(InFNm, ssfTabSep); Ss.Next(); ) {
      TrueM.Add(Ss.GetInt(1)/NM);  TrueB.Add(Ss.GetInt(3)/NB);
    }
    SimM.Gen(TrueM.Len());
    SimB.Gen(TrueB.Len());
    IAssert(SimM.Len()==TrueM.Len() &&  SimB.Len()==TrueB.Len() && SimB.Len()==SimM.Len());
  }
  TSir2Pop& operator=(const TSir2Pop& S) {
    I0M=S.I0M; I0B=S.I0B; BetaM=S.BetaM; BetaMB=S.BetaMB; BetaBM=S.BetaBM; 
    BetaB=S.BetaB; GammaM=S.GammaM; GammaB=S.GammaB; TrueB=S.TrueB; 
    TrueM=S.TrueM; SimM=S.SimM; SimB=S.SimB;
    return *this;
  }
  void SetLag(const int& Lag) {
    TrueM.Del(0,Lag); TrueB.Del(0,Lag);
    SimM.Del(0,Lag);  SimB.Del(0,Lag);
  }
  void Run() { 
    const double StepSz = 1;
    double SM = 1.0-I0M, IM = I0M;
    double SB = 1.0-I0B, IB = I0B;
    SimM.Clr(false);  SimM.Add(IM);
    SimB.Clr(false);  SimB.Add(IB);
    for (int t = 0; t < TrueM.Len()-1; t++) {
      //I = I + Beta*I*S*StepSz - Gamma*I*StepSz;
      IM = IM + BetaM*IM*SM*StepSz + BetaBM*IB*SM*StepSz - GammaM*IM*StepSz;
      IB = IB + BetaB*IB*SB*StepSz + BetaMB*IM*SB*StepSz - GammaB*IB*StepSz;
      //S = S - Beta*I*S*StepSz;
      SM = SM - BetaM*IM*SM*StepSz - BetaBM*IB*SM*StepSz;
      SB = SB - BetaB*IB*SB*StepSz - BetaMB*IM*SB*StepSz;
      SimM.Add(IM);
      SimB.Add(IB);
    }
  }
  double GetMsErr() {
    Run();
    double Err = 0.0;
    //IAssert(SimM.Len()==TrueM.Len() &&  SimB.Len()==TrueB.Len());
    for (int i = 0; i < SimM.Len(); i++) {
      Err += TMath::Sqr(TrueM[i]-SimM[i]);
      Err += TMath::Sqr(TrueB[i]-SimB[i]);
    }
    return Err;
  }
  void Plot(const TStr& OutFNm, const TStr& Desc, double Err=-1) {
    Run();
    TGnuPlot GP(OutFNm, Desc+TStr::Fmt("BetaM=%g, BetaMB=%g, BetaBM=%g, BetaB=%g, GammaM=%g, GammaB=%g", 
      BetaM, BetaMB, BetaBM, BetaB, GammaM, GammaB));
    GP.AddPlot(TrueB, gpwLinesPoints, "True BLOGS");
    GP.AddPlot(TrueM, gpwLinesPoints, "True MEDIA");
    GP.AddPlot(SimB, gpwLinesPoints, "SIR BLOGS");
    GP.AddPlot(SimM, gpwLinesPoints, "SIR MEDIA");
    GP.SavePng();
  }
  double FitBest() {
    double BestErr=TInt::Mx;
    I0M = 1e-5;  I0B = 1e-5;
    double bestBetaM, bestBetaMB, bestBetaBM, bestBetaB, bestGammaM, bestGammaB;
    for (double pBetaM= 0.02; pBetaM<1;  pBetaM +=0.1) { BetaM=pBetaM;
    for (double pBetaMB=0.02; pBetaMB<1; pBetaMB+=0.1) { BetaMB=pBetaM;
    for (double pBetaBM=0.02; pBetaBM<1; pBetaBM+=0.1) { BetaBM=pBetaM;
    for (double pBetaB= 0.02; pBetaB<1;  pBetaB +=0.1) { BetaB=pBetaM;
    for (double pGammaM=0.02; pGammaM<1; pGammaM+=0.1) { GammaM=pBetaM;
    for (double pGammaB=0.02; pGammaB<1; pGammaB+=0.1) { GammaB=pBetaM;
      const double Err = GetMsErr();
      if (Err < BestErr) {
        BestErr=Err;
        bestBetaM=BetaM; bestBetaMB=BetaMB; bestBetaBM=BetaBM; 
        bestBetaB=BetaB; bestGammaM=GammaM; bestGammaB=GammaB;
      }
    } } } } } }
    BetaM=bestBetaM; BetaMB=bestBetaMB; BetaBM=bestBetaBM; BetaB=bestBetaB; GammaM=bestGammaM; GammaB=bestGammaB; 
    return BestErr;
  }
  static void FitAllParam() {
    TIntV NV = TIntV::GetV(Kilo(1), Kilo(5), Kilo(10), Kilo(20), Kilo(50), Kilo(100), Kilo(200), Kilo(500), Mega(1)); 
    double BestErr = TInt::Mx;
    int BestNM, BestNB, BestL; double BestT;
    TSir2Pop BestSir("SIR-model\\lag-clust1k1h.tab", 1, 1);
    for (int i = 0; i < NV.Len(); i++) {
    //for (int j = 0; j < NV.Len(); j++)
     {
    for (double TimeStepSz = 0.5; TimeStepSz < 20; TimeStepSz+=0.5) {
    for (int Lag = 1; Lag < 20; Lag+=2) { 
      const int NB = NV[i];
      const int NM = NV[i];
      printf("Media: NM: %d  NB: %d Lag=%d  TmStep=%g\n", NM/1000, NB/1000, Lag, TimeStepSz);
      TSir2Pop Sir("SIR-model\\lag-clust1k1h.tab", NM, NB);
      Sir.SetLag(Lag);
      const double Err = Sir.FitBest();
      if (Err < BestErr) {
        printf("plot\n");
        BestErr = Err;  BestSir = Sir;
        BestNM=NM;  BestNB=NB; BestL=Lag; BestT=TimeStepSz;
        BestSir.Plot(TStr::Fmt("sir3Pop2-T%.01f-lag%02d", TimeStepSz, Lag), 
          TStr::Fmt("2Pop: NM=%dk  NB=%dk  I0=1e-6  Lag=%d  TmStep=%.01f", BestNM/1000, BestNB/1000, BestL, BestT));
      }
    } } } 
     BestSir.Plot(TStr::Fmt("sir3Pop2-Best-%d", i), 
        TStr::Fmt("2Pop: NM=%dk  NB=%dk  I0=1e-6  Lag=%d  TmStep=%.01f", BestNM/1000, BestNB/1000, BestL, BestT));
    }
    BestSir.Plot(TStr::Fmt("sir3Pop2-Best"), 
      TStr::Fmt("2Pop: NM=%dk  NB=%dk  I0=1e-6  Lag=%d  TmStep=%.01f", BestNM/1000, BestNB/1000, BestL, BestT));
  }
};

/*double TimeStepSz=1.0;
class TSirModel {
public:
  double N, I0;
  double Beta, Gamma;
public:
  TSirModel() { }
  void Run(const int& Steps, TFltV& InfectV) const { 
    double StepSz = TimeStepSz;
    double S=N-I0, I = I0; // succeptible, infected
    InfectV.Gen(Steps, 0);
    InfectV.Add(I);
    for (int t = 0; t < Steps-1; t++) {
      I = I + Beta*I*S*StepSz - Gamma*I*StepSz;
      S = S - Beta*I*S*StepSz;
      InfectV.Add(I);
    }
  }
};

class TEstimateSirParams {
public:
  TFltV TrueInfV;
  TSirModel Sir;
public:
  TEstimateSirParams(const TSirModel& Model) : Sir(Model) { }
  void LoadTrueInf(const TStr& FNm, const int& N) { 
    for (TSsParser Ss(FNm, ssfTabSep); Ss.Next(); ) { 
      TrueInfV.Add(Ss.GetFlt(1)/double(N)); } // load media
      //TrueInfV.Add(Ss.GetFlt(3)/double(N)); } // load blogs
  }
  double GetMsErr() {
    TFltV SimInfV;
    Sir.Run(TrueInfV.Len(), SimInfV);
    double Err = 0.0;
    for (int i = 0; i < SimInfV.Len(); i++) {
      Err += TMath::Sqr(TrueInfV[i]-SimInfV[i]);
    }
    return Err;
  }
  void Sample() {
    TRnd Rnd(0);
    double PrevErr = TInt::Mx;
    double BestB, BestG, BestErr=GetMsErr();
    for (int t = 0; t < 1000000; t++) {
      const double OldB = Sir.Beta;
      const double OldG = Sir.Gamma;
      Sir.Beta += Rnd.GetNrmDev(0, 1e-4, -0.5, 0.5);
      Sir.Gamma +=  Rnd.GetNrmDev(0, 1e-4, -0.5, 0.5);
      if (Sir.Beta < 1e-6) { Sir.Beta = 1e-6; continue; }
      if (Sir.Beta > 1) { Sir.Beta = 0.99; continue; }
      if (Sir.Gamma < 1e-6) { Sir.Gamma = 1e-6; continue; }
      if (Sir.Gamma > 1) { Sir.Gamma = 0.99; continue; }
      const double NewErr = GetMsErr();
      if (NewErr < BestErr) {
        BestB=Sir.Beta;  BestG=Sir.Gamma; BestErr=NewErr;
        printf("e:%f\tb:%f\tg:%f\n", BestErr, BestB, BestG);
      }
      if (Rnd.GetUniDev() < exp(-NewErr+BestErr)) { // accept
        PrevErr = NewErr;
      } else { // reject
        Sir.Beta = OldB;
        Sir.Gamma = OldG;
      }
    }
    Sir.Beta=BestB;
    Sir.Gamma=BestG;
  }
  double Sample2() {
    double BestB, BestG, BestErr=GetMsErr();
    for (double B = 0.001; B<1; B+=0.001) {
      for (double G = 0.001; G<1; G+=0.001) {
        Sir.Beta=B; Sir.Gamma=G;
        const double NewErr = GetMsErr();
        if (NewErr < BestErr) { 
          BestB=Sir.Beta;  BestG=Sir.Gamma; BestErr=NewErr;
          //printf("e:%12f\tb:%f\tg:%f\n", BestErr, BestB, BestG);
        }
      }
    }
    Sir.Beta=BestB;
    Sir.Gamma=BestG;
    return BestErr;
  }
  void Plot(const TStr& OutFNm, const TStr& Desc) {
    TFltV SimInfV, SimInfV2;
    const double Err1 = GetMsErr();
    Sir.Run(TrueInfV.Len(), SimInfV);
    TGnuPlot GP(OutFNm, Desc);
    GP.AddPlot(TrueInfV, gpwLinesPoints, TStr::Fmt("TRUE (N=18k, TimeStep: %g)", TimeStepSz));
    GP.AddPlot(SimInfV, gpwLinesPoints,  TStr::Fmt("best: B=%f G=%f Err=%f", Sir.Beta, Sir.Gamma, Err1));
    GP.SavePng();
  }
  static void Run() {
    TIntV NV = TIntV::GetV(Kilo(1), Kilo(5), Kilo(10), Kilo(20), Kilo(50), Kilo(100), Kilo(200), Kilo(500), Mega(1)); 
    double BestErr = TInt::Mx;
    int BestN, BestL; double BestT;
    TSirModel Sir;
    TEstimateSirParams BestSir(Sir);
    for (int i = 0; i < NV.Len(); i++) {
      const int N = NV[i];
      for (double s = 0.5; s < 20; s+=0.5) {
        TimeStepSz = s;
        int Lag = 1;
        for (; Lag < 20; Lag+=2) { 
          printf("Media: Lag=%d  TmStep=%g\n", Lag, TimeStepSz);
          Sir.N = 1;  Sir.I0 = 1e-6;
          Sir.Beta = 0.5;  Sir.Gamma = 0.3;
          TEstimateSirParams EstSir(Sir);
          EstSir.LoadTrueInf("SIR-model\\lag-clust1k1h.tab", N);
          EstSir.TrueInfV.Del(0, Lag);
          const double Err = EstSir.Sample2();
          if (Err < BestErr) {
            BestErr = Err;  BestSir = EstSir;
            BestN=N; BestL=Lag; BestT=TimeStepSz;
            BestSir.Plot(TStr::Fmt("sirMedia-T%.01f-lag%02d", TimeStepSz, Lag), 
              TStr::Fmt("Media: N=%dk  I0=1e-6  Lag=%d  TmStep=%.01f", BestN/1000, BestL, BestT));
          }
        }
      }
      BestSir.Plot(TStr::Fmt("sirMedia-Best-%d", i), 
        TStr::Fmt("Media: N=%dk  I0=1e-6  Lag=%d  TmStep=%.01f", BestN/1000, BestL, BestT));
    }
  }
};
*/


