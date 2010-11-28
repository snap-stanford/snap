/////////////////////////////////////////////////
// Mathematical-Utilities
double TMath::E=2.71828182845904523536;
double TMath::Pi=3.14159265358979323846;
double TMath::LogOf2=log(double(2));

/////////////////////////////////////////////////
// Special-Functions
void TSpecFunc::GammaPSeries/*gser*/(
 double& gamser, const double& a, const double& x, double& gln){
  static const int ITMAX=100;
  static const double EPS=3.0e-7;
  int n;
  double sum, del, ap;

  gln=LnGamma(a);
  if (x <= 0.0){
    IAssert(x>=0); /*if (x < 0.0) nrerror("x less than 0 in routine gser");*/
    gamser=0.0;
    return;
  } else {
    ap=a;
    del=sum=1.0/a;
    for (n=1; n<=ITMAX; n++){
      ++ap;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS){
        gamser=sum*exp(-x+a*log(x)-(gln));
        return;
      }
    }
    Fail; /*nrerror("a too large, ITMAX too small in routine gser");*/
    return;
  }
}

void TSpecFunc::GammaQContFrac/*gcf*/(
 double& gammcf, const double& a, const double& x, double& gln){
  static const int ITMAX=100;
  static const double EPS=3.0e-7;
  static const double  FPMIN=1.0e-30;
  int i;
  double an, b, c, d, del, h;

  gln=LnGamma(a);
  b=x+1.0-a;
  c=1.0/FPMIN;
  d=1.0/b;
  h=d;
  for (i=1;i<=ITMAX;i++){
    an = -i*(i-a);
    b += 2.0;
    d=an*d+b;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=b+an/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    del=d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
  IAssert(i<=ITMAX);
  /*if (i > ITMAX) nrerror("a too large, ITMAX too small in gcf");*/
  gammcf=exp(-x+a*log(x)-(gln))*h;
}

double TSpecFunc::GammaQ/*gammq*/(const double& a, const double& x){
  IAssert((x>=0)&&(a>0));
  double gamser, gammcf, gln;
  if (x<(a+1.0)){
    GammaPSeries(gamser,a,x,gln);
    return 1.0-gamser;
  } else {
    GammaQContFrac(gammcf,a,x,gln);
    return gammcf;
  }
}

double TSpecFunc::LnGamma/*gammln*/(const double& xx){
  double x, y, tmp, ser;
  static double cof[6]={76.18009172947146,-86.50532032941677,
          24.01409824083091,-1.231739572450155,
          0.1208650973866179e-2,-0.5395239384953e-5};
  int j;

  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}

double TSpecFunc::LnComb(const int& n, const int& k){
  return LnGamma(n+1)-LnGamma(k+1)-LnGamma(n-k+1);
}

double TSpecFunc::BetaCf(const double& a, const double& b, const double& x){
  static const double MAXIT=100;
  static const double EPS=3.0e-7;
  static const double FPMIN=1.0e-30;
  int m,m2;
  double aa,c,d,del,h,qab,qam,qap;

  qab=a+b;
  qap=a+1.0;
  qam=a-1.0;
  c=1.0;
  d=1.0-qab*x/qap;
  if (fabs(d) < FPMIN) d=FPMIN;
  d=1.0/d;
  h=d;
  for (m=1;m<=MAXIT;m++) {
    m2=2*m;
    aa=m*(b-m)*x/((qam+m2)*(a+m2));
    d=1.0+aa*d;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=1.0+aa/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    h *= d*c;
    aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
    d=1.0+aa*d;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=1.0+aa/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    del=d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
  if (m > MAXIT){Fail;}// a or b too big, or MAXIT too small in betacf
  return h;
}

double TSpecFunc::BetaI(const double& a, const double& b, const double& x){
  double bt;

  if (x < 0.0 || x > 1.0){Fail;} // Bad x in routine betai
  if (x == 0.0 || x == 1.0) bt=0.0;
  else
    bt=exp(LnGamma(a+b)-LnGamma(a)-LnGamma(b)+a*log(x)+b*log(1.0-x));
  if (x < (a+1.0)/(a+b+2.0))
    return bt*BetaCf(a,b,x)/a;
  else
    return 1.0-bt*BetaCf(b,a,1.0-x)/b;
}

void TSpecFunc::LinearFit(
 const TVec<TFltPr>& XY, double& A, double& B,
 double& SigA, double& SigB, double& Chi2, double& R2) {
  // y = a + bx :: SigA (SigB): A's uncertainty; Chi2: std dev on all points
  int i;
  double t, sxoss, sx = 0.0, sy = 0.0, st2 = 0.0, ss, sigdat;

  A = B = SigA = SigB = Chi2 = 0.0;
  for (i = 0; i < XY.Len(); i++) {
    sx += XY[i].Val1;
    sy += XY[i].Val2;
  }
  ss = XY.Len();
  sxoss = sx / ss;
  for (i = 0; i <XY.Len(); i++) {
    t = XY[i].Val1 - sxoss;
    st2 += t*t;
    B += t * XY[i].Val2;
  }
  B /= st2;
  A = (sy - sx * B) / ss;
  SigA = sqrt((1.0 + sx * sx / (ss * st2)) / ss);
  SigB = sqrt(1.0 / st2);
  for (i = 0; i < XY.Len(); i++)
    Chi2 += TMath::Sqr(XY[i].Val2 - A - B * XY[i].Val1);
  sigdat = sqrt(Chi2 / (XY.Len() - 2));
  SigA *= sigdat;
  SigB *= sigdat;

  // calculate R squared
  { double N = XY.Len(), sXY=0.0, sX=0.0, sY=0.0, sSqX=0.0, sSqY=0.0;
  for (int s =0; s < XY.Len(); s++) {
    sX += XY[s].Val1;  sY += XY[s].Val2;
    sXY += XY[s].Val1 * XY[s].Val2;
    sSqX += TMath::Sqr(XY[s].Val1);
    sSqY += TMath::Sqr(XY[s].Val2);
  }
  R2 = TMath::Sqr(N*sXY - sX*sY) / ((N*sSqX - sX*sX) * (N*sSqY - sY*sY)); }
  if (1.1 < R2 || -1.1 > R2) R2 = 0.0;
  if (_isnan(A) || ! _finite(A)) A = 0.0;
  if (_isnan(B) || ! _finite(B)) B = 0.0;
}

void TSpecFunc::PowerFit(const TVec<TFltPr>& XY, double& A, double& B,
 double& SigA, double& SigB, double& Chi2, double& R2) {
  // y = a x^b :: SigA (SigB): A's uncertainty; Chi2: std dev on all points
  // log fit
  double AA, BB;
  TFltPrV LogXY(XY.Len(), 0);
  for (int s = 0; s < XY.Len(); s++) {
    LogXY.Add(TFltPr(log((double)XY[s].Val1), log((double)XY[s].Val2)));
  }
  TSpecFunc::LinearFit(LogXY, AA, BB, SigA, SigB, Chi2, R2);
  A = exp(AA);  B = BB;
  if (_isnan(AA) || ! _finite(AA)) A = 0.0;
  if (_isnan(BB) || ! _finite(BB)) B = 0.0;
}

void TSpecFunc::LogFit(const TVec<TFltPr>& XY, double& A, double& B,
 double& SigA, double& SigB, double& Chi2, double& R2) {
  // Y = A + B*log(X)
  TFltPrV LogXY(XY.Len(), 0);
  for (int s = 0; s < XY.Len(); s++) {
    LogXY.Add(TFltPr(log((double)XY[s].Val1), XY[s].Val2));
  }
  TSpecFunc::LinearFit(LogXY, A, B, SigA, SigB, Chi2, R2);
}

void TSpecFunc::ExpFit(const TVec<TFltPr>& XY, double& A, double& B,
 double& SigA, double& SigB, double& Chi2, double& R2) {
  // Y = A * exp(B*X)
  TFltPrV XLogY(XY.Len(), 0);
  double AA, BB;
  for (int s = 0; s < XY.Len(); s++) {
    XLogY.Add(TFltPr(XY[s].Val1, log((double)XY[s].Val2)));
  }
  TSpecFunc::LinearFit(XLogY, AA, BB, SigA, SigB, Chi2, R2);
  A = exp(AA);
  B = BB;
}

double TSpecFunc::Entropy(const TIntV& ValV) {
  TFltV NewValV(ValV.Len());
  for (int i = 0; i < ValV.Len(); i++) { NewValV[i] = ValV[i]; }
  return Entropy(NewValV);
}

// Entropy of a distribution: ValV[i] contains the number of events i
double TSpecFunc::Entropy(const TFltV& ValV) {
  double Sum=0, Ent=0;
  for (int i = 0; i < ValV.Len(); i++) {
    const double& Val = ValV[i];
    if (Val > 0.0) { Ent -= Val * log(Val);  Sum += Val; }
  }
  if (Sum > 0.0) {
    Ent /= Sum;
    Ent += log(Sum);
    Ent /= TMath::LogOf2;
  } else return 1.0;
  return Ent;
}

void TSpecFunc::EntropyFracDim(const TIntV& ValV, TFltV& EntropyV) {
  TFltV NewValV(ValV.Len());
  for (int i = 0; i < ValV.Len(); i++) { 
    IAssert(ValV[i]==1 || ValV[i] == 0);
    NewValV[i] = ValV[i]; 
  }
  EntropyFracDim(NewValV, EntropyV);
}

// Entropy fractal dimension. Input is a vector {0,1}^n. 
// Where 0 means the event did not occur, and 1 means it occured.
// Works exactly as Mengzi Wang's code.
void TSpecFunc::EntropyFracDim(const TFltV& ValV, TFltV& EntropyV) {
  TFltV ValV1, ValV2;
  int Pow2 = 1;
  while (2*Pow2 <= ValV.Len()) { Pow2 *= 2; }
  ValV1.Gen(Pow2);
  for (int i = 0; i < Pow2; i++) { ValV1[i] = ValV[i]; 
    IAssert(ValV[i]==1.0 || ValV[i] == 0.0); }
  EntropyV.Clr();
  EntropyV.Add(Entropy(ValV1)); // 2^Pow2 windows
  while (ValV1.Len() > 2) {
    ValV2.Gen(ValV1.Len() / 2);
    for (int i = 0; i < ValV1.Len(); i++) {
      ValV2[i/2] += ValV1[i];
    }
    EntropyV.Add(Entropy(ValV2));
    ValV1.MoveFrom(ValV2);
  }
  EntropyV.Reverse();
}

// solves for p: B = p * log2(p) + (1-p) * log2(1-p)
double TSpecFunc::EntropyBias(const double& B){
  static TFltFltH BToP;
  if (BToP.Empty()) {
    for (double p = 0.5; p < 1.0; p +=0.0001) {
      double H = p * log(p) + (1.0-p) * log(1.0 - p);
      H = -H / log(2.0);
      BToP.AddDat(TMath::Round(H, 3), p);
    }
  }
  if (BToP.IsKey(TMath::Round(B, 3))) { return BToP.GetDat(TMath::Round(B, 3)); }
  else { return -1.0; }
}

// MLE of the power-law coefficient
double TSpecFunc::GetPowerCoef(const TFltV& XValV, double MinX) {
  for (int i = 0; MinX <= 0.0 && i < XValV.Len(); i++) { 
    MinX = XValV[i]; }
  IAssert(MinX > 0.0);
  double LnSum=0.0;
  for (int i = 0; i < XValV.Len(); i++) {
    if (XValV[i].Val < MinX) continue;
    LnSum += log(XValV[i] / MinX);
  }
  return 1.0 + double(XValV.Len()) / LnSum;
}

double TSpecFunc::GetPowerCoef(const TFltPrV& XValCntV, double MinX) {
  for (int i = 0; MinX <= 0.0 && i < XValCntV.Len(); i++) { 
    MinX = XValCntV[i].Val1; }
  IAssert(MinX > 0.0);
  double NSamples=0.0, LnSum=0.0;
  for (int i = 0; i < XValCntV.Len(); i++) {
    if (XValCntV[i].Val1() < MinX) continue;
    LnSum += XValCntV[i].Val2 * log(XValCntV[i].Val1 / MinX);
    NSamples += XValCntV[i].Val2;
  }
  return 1.0 + NSamples / LnSum;
}

/////////////////////////////////////////////////
// Statistical-Moments
TMom::TMom(const TFltV& _ValV):
  //WgtV(_ValV.Len(), 0), ValV(_ValV.Len(), 0),
  ValWgtV(_ValV.Len(), 0),
  SumW(), ValSumW(),
  UsableP(false), UnusableVal(-1),
  Mn(), Mx(),
  Mean(), Vari(), SDev(), SErr(),
  Median(), Quart1(), Quart3(),
  DecileV(), PercentileV(){
  for (int ValN=0; ValN<_ValV.Len(); ValN++){Add(_ValV[ValN], 1);}
  Def();
}

void TMom::Def(){
  IAssert(!DefP); DefP=true;
  UsableP=(SumW>0)&&(ValWgtV.Len()>0);
  if (UsableP){
    // Mn, Mx
    Mn=ValWgtV[0].Val1;
    Mx=ValWgtV[0].Val1;
    // Mean, Variance (Mn, Mx), Standard-Error
    Mean=ValSumW/SumW;
    Vari=0;
    if (ValWgtV.Len()>1){
      for (int ValN=0; ValN<ValWgtV.Len(); ValN++){
        const double Val=ValWgtV[ValN].Val1;
        Vari+=ValWgtV[ValN].Val2*TMath::Sqr(Val-Mean);
        if (Val<Mn){Mn=Val;}
        if (Val>Mx){Mx=Val;}
      }
      Vari=Vari/SumW;
      SErr=sqrt(Vari/(ValWgtV.Len()*(ValWgtV.Len()-1)));
    }
    // Standard-Deviation
    SDev=sqrt(double(Vari));
    // Median
    ValWgtV.Sort();
    double CurSumW = 0;
    for (int ValN=0; ValN<ValWgtV.Len(); ValN++){
      CurSumW += ValWgtV[ValN].Val2;
      if (CurSumW > 0.5*SumW) { 
        Median = ValWgtV[ValN].Val1; break; }
      else if (CurSumW == 0.5*SumW) {
        Median = 0.5 * (ValWgtV[ValN].Val1+ValWgtV[ValN+1].Val1); break; }
    }
    // Quartile-1 and Quartile-3
    Quart1=Quart3=TFlt::Mn;
    CurSumW = 0;
    for (int ValN=0; ValN<ValWgtV.Len(); ValN++){
      CurSumW += ValWgtV[ValN].Val2;
      if (Quart1==TFlt::Mn) {
        if (CurSumW > 0.25*SumW) {  Quart1 = ValWgtV[ValN].Val1; }
        //else if (CurSumW == 0.25*SumW) { Quart1 = 0.5 * (ValWgtV[ValN].Val1+ValWgtV[ValN+1].Val1); }
      } 
      if (Quart3==TFlt::Mn) {
        if (CurSumW > 0.75*SumW) { Quart3 = ValWgtV[ValN].Val1; }
        //else if (CurSumW == 0.75*SumW) { Quart3 = 0.5 * (ValWgtV[ValN].Val1+ValWgtV[ValN+1].Val1); }
      }
    }
    // Deciles & Percentiles
    CurSumW = 0;
    int DecileN = 1, PercentileN = 1;
    DecileV.Gen(11);  PercentileV.Gen(101);
    DecileV[0]=Mn; DecileV[10]=Mx;
    PercentileV[0]=Mn; PercentileV[100]=Mx;
    for (int ValN=0; ValN<ValWgtV.Len(); ValN++){
      CurSumW += ValWgtV[ValN].Val2;
      if (CurSumW > SumW*DecileN*0.1) { 
        DecileV[DecileN] = ValWgtV[ValN].Val1;  DecileN++; }
      if (CurSumW > SumW*PercentileN*0.01) {
        PercentileV[PercentileN] = ValWgtV[ValN].Val1;  PercentileN++; }
    }
  }
  ValWgtV.Clr();
}



double TMom::GetByNm(const TStr& MomNm) const {
  if (MomNm=="Mean"){return GetMean();}
  else if (MomNm=="Vari"){return GetVari();}
  else if (MomNm=="SDev"){return GetSDev();}
  else if (MomNm=="SErr"){return GetSErr();}
  else if (MomNm=="Median"){return GetMedian();}
  else if (MomNm=="Quart1"){return GetQuart1();}
  else if (MomNm=="Quart3"){return GetQuart3();}
  else if (MomNm=="Decile0"){return GetDecile(0);}
  else if (MomNm=="Decile1"){return GetDecile(1);}
  else if (MomNm=="Decile2"){return GetDecile(2);}
  else if (MomNm=="Decile3"){return GetDecile(3);}
  else if (MomNm=="Decile4"){return GetDecile(4);}
  else if (MomNm=="Decile5"){return GetDecile(5);}
  else if (MomNm=="Decile6"){return GetDecile(6);}
  else if (MomNm=="Decile7"){return GetDecile(7);}
  else if (MomNm=="Decile8"){return GetDecile(8);}
  else if (MomNm=="Decile9"){return GetDecile(9);}
  else if (MomNm=="Decile10"){return GetDecile(10);}
  else {Fail; return 0;}
}

TStr TMom::GetStrByNm(const TStr& MomNm, char* FmtStr) const {
  if (IsUsable()){
    if (FmtStr==NULL){
      return TFlt::GetStr(GetByNm(MomNm));
    } else {
      return TFlt::GetStr(GetByNm(MomNm), FmtStr);
    }
  } else {
    return "X";
  }
}

TStr TMom::GetStr(
 const char& SepCh, const char& DelimCh,
 const bool& DecileP, const bool& PercentileP, const TStr& FmtStr) const {
  TChA ChA;
  if (IsUsable()){
    ChA+="["; ChA+=SepCh;
    ChA+="Vals"; ChA+=DelimCh; ChA+=TInt::GetStr(GetVals()); ChA+=SepCh;
    ChA+="Min"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetMn(), FmtStr.CStr()); ChA+=SepCh;
    ChA+="Max"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetMx(), FmtStr.CStr()); ChA+=SepCh;
    ChA+="Mean"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetMean(), FmtStr.CStr()); ChA+=SepCh;
    //ChA+="Vari"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetVari(), FmtStr.CStr()); ChA+=SepCh;
    ChA+="SDev"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetSDev(), FmtStr.CStr()); ChA+=SepCh;
    //ChA+="SErr"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetSErr(), FmtStr.CStr()); ChA+=SepCh;
    ChA+="Quart1"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetQuart1(), FmtStr.CStr()); ChA+=SepCh;
    ChA+="Median"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetMedian(), FmtStr.CStr()); ChA+=SepCh;
    ChA+="Quart3"; ChA+=DelimCh; ChA+=TFlt::GetStr(GetQuart3(), FmtStr.CStr()); ChA+=SepCh;
    if (DecileP){
      for (int DecileN=0; DecileN<=10; DecileN++){
        ChA+="Dec"; ChA+=TInt::GetStr(DecileN);
        ChA+=DelimCh; ChA+=TFlt::GetStr(GetDecile(DecileN), FmtStr.CStr());
        ChA+=SepCh;
      }
    }
    if (PercentileP){
      for (int PercentileN=0; PercentileN<=100; PercentileN++){
        ChA+="Per"; ChA+=TInt::GetStr(PercentileN);
        ChA+=DelimCh; ChA+=TFlt::GetStr(GetPercentile(PercentileN), FmtStr.CStr());
        ChA+=SepCh;
      }
    }
    ChA+="]";
  } else {
    ChA="[Unusable]";
  }
  return ChA;
}

TStr TMom::GetNmVStr(const TStr& VarPfx,
 const char& SepCh, const bool& DecileP, const bool& PercentileP){
  TChA ChA;
  ChA+=VarPfx; ChA+="Vals"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="Min"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="Max"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="Mean"; ChA+=SepCh;
  //ChA+=VarPfx; ChA+="Vari"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="SDev"; ChA+=SepCh;
  //ChA+=VarPfx; ChA+="SErr"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="Quart1"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="Median"; ChA+=SepCh;
  ChA+=VarPfx; ChA+="Quart3";
  if (DecileP){
    ChA+=SepCh;
    for (int DecileN=0; DecileN<=10; DecileN++){
      ChA+=VarPfx; ChA+="Dec"; ChA+=TInt::GetStr(DecileN);
      if (DecileN<10){ChA+=SepCh;}
    }
  }
  if (PercentileP){
    ChA+=SepCh;
    for (int PercentileN=0; PercentileN<=100; PercentileN++){
      ChA+=VarPfx; ChA+="Per"; ChA+=TInt::GetStr(PercentileN);
      if (PercentileN<100){ChA+=SepCh;}
    }
  }
  return ChA;
}

TStr TMom::GetValVStr(
 const char& SepCh, const bool& DecileP, const bool& PercentileP) const {
  TChA ChA;
  if (IsUsable()){
    ChA+=TInt::GetStr(GetVals()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetMn()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetMx()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetMean()); ChA+=SepCh;
    //ChA+=TFlt::GetStr(GetVari()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetSDev()); ChA+=SepCh;
    //ChA+=TFlt::GetStr(GetSErr()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetQuart1()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetMedian()); ChA+=SepCh;
    ChA+=TFlt::GetStr(GetQuart3()); ChA+=SepCh;
    if (DecileP){
      for (int DecileN=0; DecileN<=10; DecileN++){
        ChA+=TFlt::GetStr(GetDecile(DecileN)); ChA+=SepCh;
      }
    }
    if (PercentileP){
      for (int PercentileN=0; PercentileN<=100; PercentileN++){
        ChA+=TFlt::GetStr(GetPercentile(PercentileN)); ChA+=SepCh;
      }
    }
  } else {
    int Vals=8;
    if (DecileP){Vals+=11;}
    if (PercentileP){Vals+=101;}
    for (int ValN=0; ValN<Vals; ValN++){
      ChA="[Unusable]";
      if (ValN<Vals-1){ChA+=SepCh;}
    }
  }
  return ChA;
}

/////////////////////////////////////////////////
// Correlation
TCorr::TCorr(const TFltV& ValV1, const TFltV& ValV2):
  ValVLen(ValV1.Len()), CorrCf(), CorrCfPrb(), FisherZ(){
  static const double TINY=1.0e-20;
  IAssert(ValV1.Len()==ValV2.Len());

  // calculate the means
  double MeanVal1=0; double MeanVal2=0;
  {for (int ValN=0; ValN<ValVLen; ValN++){
    MeanVal1+=ValV1[ValN]; MeanVal2+=ValV2[ValN];}}
  MeanVal1/=ValVLen; MeanVal2/=ValVLen;

  // calculate correlation coefficient
  double yt, xt;
  double syy=0.0; double sxy=0.0; double sxx=0.0;
  {for (int ValN=0; ValN<ValVLen; ValN++){
    xt=ValV1[ValN]-MeanVal1;
    yt=ValV2[ValN]-MeanVal2;
    sxx+=xt*xt;
    syy+=yt*yt;
    sxy+=xt*yt;
  }}
  if (sxx*syy==0){
    CorrCf=0; //** not in numerical recipes - check why (pojavi se, ko so same nicle)
  } else {
    CorrCf=sxy/sqrt(sxx*syy);
  }
  // calculate correlation coefficient significance level
  double df=ValVLen-2;
  double t=CorrCf*sqrt(df/((1.0-CorrCf+TINY)*(1.0+CorrCf+TINY)));
  CorrCfPrb=TSpecFunc::BetaI(0.5*df,0.5,df/(df+t*t));
  // calculate Fisher's Z transformation
  FisherZ=0.5*log((1.0+(CorrCf)+TINY)/(1.0-(CorrCf)+TINY));
}

/////////////////////////////////////////////////
// Statistical Tests
void TStatTest::AveVar(const TFltV& ValV, double& Ave, double& Var){
  Ave=0;
  for (int ValN=0; ValN<ValV.Len(); ValN++){
    Ave+=ValV[ValN];}
  Ave/=ValV.Len();
  Var=0;
  double ep=0;
  for (int ValN=0; ValN<ValV.Len(); ValN++){
    double s=ValV[ValN]-Ave;
    ep+=s;
    Var+=s*s;
  }
  Var=(Var-ep*ep/ValV.Len())/(ValV.Len()-1);
}

double TStatTest::KsProb(const double& Alam) {
  const double EPS1 = 0.001;
  const double EPS2 = 1.0e-8;
  double a2 = -2.0*Alam*Alam, fac = 2.0, sum = 0.0, term, termbf = 0.0;
  for (int j=1; j <= 100; j++) {
    term = fac*exp(a2*j*j);
    sum += term;
    if (fabs(term) <= EPS1*termbf || fabs(term) <= EPS2*sum)
      return sum;
    fac = -fac;
    termbf = fabs(term);
  }
  return 1.0;
}

void TStatTest::ChiSquareOne(
 const TFltV& ObservedBinV, const TFltV& ExpectedBinV,
 double& ChiSquareVal, double& SignificancePrb){
  IAssert(ObservedBinV.Len()==ExpectedBinV.Len());
  int Bins=ObservedBinV.Len();
  int Constraints=0;
  int DegreesOfFreedom=Bins-Constraints;
  ChiSquareVal=0.0;
  for (int BinN=0; BinN<Bins; BinN++){
    IAssert(ExpectedBinV[BinN]>0);
    double BinDiff=ObservedBinV[BinN]-ExpectedBinV[BinN];
    ChiSquareVal+=BinDiff*BinDiff/ExpectedBinV[BinN];
  }
  SignificancePrb=
   TSpecFunc::GammaQ(0.5*(DegreesOfFreedom), 0.5*(ChiSquareVal));
}

void TStatTest::ChiSquareTwo(
 const TFltV& ObservedBin1V, const TFltV& ObservedBin2V,
 double& ChiSquareVal, double& SignificancePrb){
  IAssert(ObservedBin1V.Len()==ObservedBin1V.Len());
  int Bins=ObservedBin1V.Len();
  int Constraints=0;
  int DegreesOfFreedom=Bins-Constraints;
  ChiSquareVal=0.0;
  for (int BinN=1; BinN<=Bins; BinN++){
    if ((ObservedBin1V[BinN]==0.0) && (ObservedBin2V[BinN]==0.0)){
      DegreesOfFreedom--;
    } else {
      double BinDiff=ObservedBin1V[BinN]-ObservedBin2V[BinN];
      ChiSquareVal+=BinDiff*BinDiff/(ObservedBin1V[BinN]+ObservedBin2V[BinN]);
    }
  }
  SignificancePrb=
   TSpecFunc::GammaQ(0.5*(DegreesOfFreedom),0.5*(ChiSquareVal));
}

void TStatTest::TTest(
 const TFltV& ValV1, const TFltV& ValV2, double& TTestVal, double& TTestPrb){
  /*double Ave1; double Var1;
  AveVar(ValV1, Ave1, Var1);
  double Ave2; double Var2;
  AveVar(ValV2, Ave2, Var2);*/

  PMom Val1Mom=TMom::New(ValV1);
  PMom Val2Mom=TMom::New(ValV2);
  double ave1=Val1Mom->GetMean();
  double ave2=Val2Mom->GetMean();
  double var1=Val1Mom->GetVari();
  double var2=Val2Mom->GetVari();
  int n1=ValV1.Len();
  int n2=ValV2.Len();

  TTestVal=(ave1-ave2)/sqrt(var1/n1+var2/n2);
  double df=TMath::Sqr(var1/n1+var2/n2)/(TMath::Sqr(var1/n1)/(n1-1)+TMath::Sqr(var2/n2)/(n2-1));
  TTestPrb=TSpecFunc::BetaI(0.5*df, 0.5, df/(df+TMath::Sqr(TTestVal)));
}

void TStatTest::KsTest(const TFltV& ValV1, const TFltV& ValV2, double& DStat, double& PVal) {
  IAssert(ValV1.IsSorted() && ValV2.IsSorted());
  int i1=0, i2=0;
  double CumSum1=0.0, CumSum2=0.0, Cdf1=0.0, Cdf2=0.0;
  const double N1 = ValV1.Len();
  const double N2 = ValV2.Len();
  if (! (N1 > 0.0 && N2 > 0.0)) { DStat = 1.0;  PVal = 0.0;  return; }
  DStat=0.0; PVal=0.0;
  while (i1 < ValV1.Len() && i2 < ValV2.Len()) {
    const double X1 = ValV1[i1];
    const double X2 = ValV2[i2];
    if (X1 <= X2) {
      CumSum1 += 1;
      Cdf1 = (CumSum1 / N1);
      i1++;
    }
    if (X2 <= X1) {
      CumSum2 += 1;
      Cdf2 = (CumSum2 / N2);
      i2++;
    }
    DStat = TMath::Mx(DStat, fabs(Cdf1 - Cdf2));
  }
  const double En = sqrt( N1*N2 / (N1+N2));
  PVal = TStatTest::KsProb((En+0.12+0.11/En)*DStat);
}

void TStatTest::KsTest(const TFltPrV& ValCntV1, const TFltPrV& ValCntV2, double& DStat, double& PVal) {
  IAssert(ValCntV1.IsSorted() && ValCntV2.IsSorted());
  int i1=0, i2=0;
  double N1=0.0, N2=0.0, CumSum1=0.0, CumSum2=0.0, Cdf1=0.0, Cdf2=0.0;
  DStat=0.0;  PVal=0.0;
  for (int i = 0; i < ValCntV1.Len(); i++) N1 += ValCntV1[i].Val2;
  for (int i = 0; i < ValCntV2.Len(); i++) N2 += ValCntV2[i].Val2;
  if (! (N1 > 0.0 && N2 > 0.0)) { DStat = 1.0;  PVal = 0.0;  return; }

  while (i1 < ValCntV1.Len() && i2 < ValCntV2.Len()) {
    const double X1 = ValCntV1[i1].Val1;
    const double X2 = ValCntV2[i2].Val1;
    if (X1 <= X2) {
      CumSum1 += ValCntV1[i1].Val2;
      Cdf1 = (CumSum1 / N1);
      i1++;
    }
    if (X2 <= X1) {
      CumSum2 += ValCntV2[i2].Val2;
      Cdf2 = (CumSum2 / N2);
      i2++;
    }
    DStat = TMath::Mx(DStat, fabs(Cdf1 - Cdf2));
  }
  const double En = sqrt( N1*N2 / (N1+N2));
  PVal = TStatTest::KsProb((En+0.12+0.11/En)*DStat);
}

/////////////////////////////////////////////////
// Combinations
bool TComb::GetNext(){
  if (ItemV.Len()==0){
    ItemV.Gen(Order, Order);
    for (int OrderN=0; OrderN<Order; OrderN++){
      ItemV[OrderN]=OrderN;}
    return true;
  } else {
    if (ItemV.Last()==Items-1){
      int OrderN=Order-1;
      while ((OrderN>=0)&&(ItemV[OrderN]==Items-(Order-OrderN-1)-1)){OrderN--;}
      if (OrderN<0){
        return false;
      } else {
        ItemV[OrderN]++;
        for (int SubOrderN=OrderN+1; SubOrderN<Order; SubOrderN++){
          ItemV[SubOrderN]=ItemV[SubOrderN-1]+1;}
        CombN++; return true;
      }
    } else {
      ItemV.Last()++; CombN++; return true;
    }
  }
}

int TComb::GetCombs() const {
  int LCombs=1; int HCombs=1;
  for (int OrderN=0; OrderN<Order; OrderN++){
    LCombs*=OrderN+1; HCombs*=Items-OrderN;}
  int Combs=HCombs/LCombs;
  return Combs;
}

void TComb::Wr(){
  printf("%d:[", GetCombN());
  for (int OrderN=0; OrderN<Order; OrderN++){
    if (OrderN>0){printf(" ");}
    printf("%d", ItemV[OrderN]());
  }
  printf("]\n");
}

/////////////////////////////////////////////////
// Linear-Regression
PLinReg TLinReg::New(const TFltVV& _XVV, const TFltV& _YV, const TFltV& _SigV){
  PLinReg LinReg=PLinReg(new TLinReg());
  LinReg->XVV=_XVV;
  LinReg->YV=_YV;
  if (_SigV.Empty()){
    LinReg->SigV.Gen(LinReg->YV.Len());
    LinReg->SigV.PutAll(1);
  } else {
    LinReg->SigV=_SigV;
  }
  LinReg->Recs=LinReg->XVV.GetXDim();
  LinReg->Vars=LinReg->XVV.GetYDim();
  IAssert(LinReg->Recs>0);
  IAssert(LinReg->Vars>0);
  IAssert(LinReg->YV.Len()==LinReg->Recs);
  IAssert(LinReg->SigV.Len()==LinReg->Recs);
  LinReg->CovarVV.Gen(LinReg->Vars+1, LinReg->Vars+1);
  LinReg->CfV.Gen(LinReg->Vars+1);
  LinReg->NR_lfit();
  return LinReg;
}

void TLinReg::NR_covsrt(
 TFltVV& CovarVV, const int& Vars, const TIntV& ia, const int& mfit){
  for (int i=mfit+1; i<=Vars; i++){
    for (int j=1; j<=i; j++){
      CovarVV.At(i, j)=0; CovarVV.At(j, i)=0.0;}
  }
  int k=mfit;
  for (int j=Vars; j>=1; j--){
    if (ia[j]!=0){
      for (int i=1; i<=Vars; i++){Swap(CovarVV.At(i, k), CovarVV.At(i, j));}
      {for (int i=1; i<=Vars; i++){Swap(CovarVV.At(k, i), CovarVV.At(j, i));}}
      k--;
    }
  }
}

void TLinReg::NR_gaussj(TFltVV& a, const int& n, TFltVV& b, const int& m){
  int i, icol, irow=0, j, k, l, ll;
  double big, dum, pivinv;

  TIntV indxc(n+1);
  TIntV indxr(n+1);
  TIntV ipiv(n+1);
  for (j=1; j<=n; j++){ipiv[j]=0;}
  for (i=1; i<=n; i++){
    big=0.0;
    for (j=1; j<=n; j++){
      if (ipiv[j]!=1){
        for (k=1; k<=n; k++){
          if (ipiv[k]==0){
            if (fabs(double(a.At(j, k))) >= big){
              big=fabs(double(a.At(j, k)));
              irow=j;
              icol=k;
            }
          } else
          if (ipiv[k]>1){
            TExcept::Throw("Singular Matrix(1) in Gauss");}
        }
      }
    }
    ipiv[icol]++;
    if (irow != icol){
      for (l=1; l<=n; l++){Swap(a.At(irow, l), a.At(icol, l));}
      for (l=1; l<=m; l++){Swap(b.At(irow, l), b.At(icol, l));}
    }
    indxr[i]=irow;
    indxc[i]=icol;
    if (a.At(icol, icol)==0.0){
      TExcept::Throw("Singular Matrix(1) in Gauss");}
    pivinv=1.0/a.At(icol, icol);
    a.At(icol, icol)=1.0;
    for (l=1; l<=n; l++){a.At(icol, l)=a.At(icol, l)*pivinv;}
    for (l=1; l<=m; l++){b.At(icol, l)=b.At(icol, l)*pivinv;}
    for (ll=1; ll<=n; ll++){
      if (ll != icol){
        dum=a.At(ll, icol);
        a.At(ll, icol)=0.0;
        for (l=1;l<=n;l++){a.At(ll, l)-=a.At(icol, l)*dum;}
        for (l=1;l<=m;l++){b.At(ll, l)-=b.At(icol, l)*dum;}
      }
    }
  }
  for (l=n; l>=1; l--){
    if (indxr[l]!=indxc[l]){
      for (k=1; k<=n; k++){
        Swap(a.At(k, indxr[l]), a.At(k, indxc[l]));}
    }
  }
}

void TLinReg::NR_lfit(){
  int i,j,k,l,m,mfit=0;
  double ym,wt,sum,sig2i;

  TIntV ia(Vars+1); for (i=1; i<=Vars; i++){ia[i]=1;}
  TFltVV beta(Vars+1, 1+1);
  TFltV afunc(Vars+1);
  for (j=1;j<=Vars;j++){
    if (ia[j]!=0){mfit++;}}
  if (mfit==0){TExcept::Throw("No parameters to be fitted in LFit");}
  for (j=1; j<=mfit; j++){
    for (k=1; k<=mfit; k++){CovarVV.At(j, k)=0.0;}
    beta.At(j, 1)=0.0;
  }
  for (i=1; i<=Recs; i++){
    GetXV(i, afunc); // funcs(XVV[i],afunc,Vars);
    ym=GetY(i);
    if (mfit<Vars){
      for (j=1;j<=Vars;j++){
        if (ia[j]==0){ym-=CfV[j]*afunc[j];}}
    }
    sig2i=1.0/TMath::Sqr(GetSig(i));
    for (j=0, l=1; l<=Vars; l++){
      if (ia[l]!=0){
        wt=afunc[l]*sig2i;
        for (j++, k=0, m=1; m<=l; m++){
          if (ia[m]!=0){CovarVV.At(j, ++k)+=wt*afunc[m];}
        }
        beta.At(j, 1)+=ym*wt;
      }
    }
  }
  for (j=2; j<=mfit; j++){
    for (k=1; k<j; k++){CovarVV.At(k, j)=CovarVV.At(j, k);}
  }
  NR_gaussj(CovarVV, mfit, beta, 1);
  for (j=0, l=1; l<=Vars; l++){
    if (ia[l]!=0){CfV[l]=beta.At(++j, 1);}
  }
  ChiSq=0.0;
  for (i=1; i<=Recs; i++){
    GetXV(i, afunc); // funcs(XVV[i],afunc,Vars);
    for (sum=0.0, j=1; j<=Vars; j++){sum+=CfV[j]*afunc[j];}
    ChiSq+=TMath::Sqr((GetY(i)-sum)/GetSig(i));
  }
  NR_covsrt(CovarVV, Vars, ia, mfit);
}

void TLinReg::Wr() const {
  printf("\n%11s %21s\n","parameter","uncertainty");
  for (int i=0; i<Vars;i++){
    printf("  a[%1d] = %8.6f %12.6f\n",
     i+1, GetCf(i), GetCfUncer(i));
  }
  printf("chi-squared = %12f\n", GetChiSq());

  printf("full covariance matrix\n");
  {for (int i=0;i<Vars;i++){
    for (int j=0;j<Vars;j++){
      printf("%12f", GetCovar(i, j));}
    printf("\n");
  }}
}

/////////////////////////////////////////////////
// Singular-Value-Decomposition
PSvd TSvd::New(const TFltVV& _XVV, const TFltV& _YV, const TFltV& _SigV){
  PSvd Svd=PSvd(new TSvd());
  Svd->XVV=_XVV;
  Svd->YV=_YV;
  if (_SigV.Empty()){
    Svd->SigV.Gen(Svd->YV.Len());
    Svd->SigV.PutAll(1);
  } else {
    Svd->SigV=_SigV;
  }
  Svd->Recs=Svd->XVV.GetXDim();
  Svd->Vars=Svd->XVV.GetYDim();
  IAssert(Svd->Recs>0);
  IAssert(Svd->Vars>0);
  IAssert(Svd->YV.Len()==Svd->Recs);
  IAssert(Svd->SigV.Len()==Svd->Recs);
  Svd->CovarVV.Gen(Svd->Vars+1, Svd->Vars+1);
  Svd->CfV.Gen(Svd->Vars+1);
  Svd->NR_svdfit();
  return Svd;
}

double TSvd::NR_pythag(double a, double b){
  double absa,absb;
  absa=fabs(a);
  absb=fabs(b);
  if (absa > absb){
    return absa*sqrt(1.0+TMath::Sqr(absb/absa));
  } else {
    return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+TMath::Sqr(absa/absb)));
  }
}

void TSvd::NR_svdcmp(TFltVV& a, int m, int n, TFltV& w, TFltVV& v){
  int flag,i,its,j,jj,k,l=0,nm;
  double anorm,c,f,g,h,s,scale,x,y,z;

  TFltV rv1(n+1);
  g=scale=anorm=0.0;
  for (i=1;i<=n;i++) {
    l=i+1;
    rv1[i]=scale*g;
    g=s=scale=0.0;
    if (i <= m) {
      for (k=i;k<=m;k++) scale += fabs(double(a.At(k,i)));
      if (scale) {
        for (k=i;k<=m;k++) {
          a.At(k,i) /= scale;
          s += a.At(k,i)*a.At(k,i);
        }
        f=a.At(i,i);
        g = -NR_SIGN(sqrt(s),f);
        h=f*g-s;
        a.At(i,i)=f-g;
        for (j=l;j<=n;j++) {
          for (s=0.0,k=i;k<=m;k++) s += a.At(k,i)*a(k,j);
          f=s/h;
          for (k=i;k<=m;k++) a.At(k,j) += f*a.At(k,i);
        }
        for (k=i;k<=m;k++) a.At(k,i) *= scale;
      }
    }
    w[i]=scale *g;
    g=s=scale=0.0;
    if (i <= m && i != n) {
      for (k=l;k<=n;k++) scale += fabs(double(a.At(i,k)));
      if (scale) {
        for (k=l;k<=n;k++) {
          a.At(i,k) /= scale;
          s += a.At(i,k)*a.At(i,k);
        }
        f=a.At(i,l);
        g = -NR_SIGN(sqrt(s),f);
        h=f*g-s;
        a.At(i,l)=f-g;
        for (k=l;k<=n;k++) rv1[k]=a.At(i,k)/h;
        for (j=l;j<=m;j++) {
          for (s=0.0,k=l;k<=n;k++) s += a.At(j,k)*a.At(i,k);
          for (k=l;k<=n;k++) a.At(j,k) += s*rv1[k];
        }
        for (k=l;k<=n;k++) a.At(i,k) *= scale;
      }
    }
    anorm=NR_FMAX(anorm,(fabs(double(w[i]))+fabs(double(rv1[i]))));
  }
  for (i=n;i>=1;i--) {
    if (i < n) {
      if (g) {
        for (j=l;j<=n;j++)
          v.At(j,i)=(a.At(i,j)/a.At(i,l))/g;
        for (j=l;j<=n;j++) {
          for (s=0.0,k=l;k<=n;k++) s += a.At(i,k)*v.At(k,j);
          for (k=l;k<=n;k++) v.At(k,j) += s*v.At(k,i);
        }
      }
      for (j=l;j<=n;j++) v.At(i,j)=v.At(j,i)=0.0;
    }
    v.At(i,i)=1.0;
    g=rv1[i];
    l=i;
  }
  for (i=NR_IMIN(m,n);i>=1;i--) {
    l=i+1;
    g=w[i];
    for (j=l;j<=n;j++) a.At(i,j)=0.0;
    if (g) {
      g=1.0/g;
      for (j=l;j<=n;j++) {
        for (s=0.0,k=l;k<=m;k++) s += a.At(k,i)*a.At(k,j);
        f=(s/a.At(i,i))*g;
        for (k=i;k<=m;k++) a.At(k,j) += f*a.At(k,i);
      }
      for (j=i;j<=m;j++) a.At(j,i) *= g;
    } else for (j=i;j<=m;j++) a.At(j,i)=0.0;
    a.At(i,i)++;
  }
  for (k=n;k>=1;k--) {
    for (its=1;its<=30;its++) {
      flag=1;
      for (l=k;l>=1;l--) {
        nm=l-1;
        if ((double)(fabs(double(rv1[l])+anorm)) == anorm) {
          flag=0;
          break;
        }
        if ((double)(fabs(double(w[nm]))+anorm) == anorm) break;
      }
      if (flag) {
        c=0.0;
        s=1.0;
        for (i=l;i<=k;i++) {
          f=s*rv1[i];
          rv1[i]=c*rv1[i];
          if ((double)(fabs(f)+anorm) == anorm) break;
          g=w[i];
          h=NR_pythag(f,g);
          w[i]=h;
          h=1.0/h;
          c=g*h;
          s = -f*h;
          for (j=1;j<=m;j++) {
            y=a.At(j,nm);
            z=a.At(j,i);
            a.At(j,nm)=y*c+z*s;
            a.At(j,i)=z*c-y*s;
          }
        }
      }
      z=w[k];
      if (l == k) {
        if (z < 0.0) {
          w[k] = -z;
          for (j=1;j<=n;j++) v.At(j,k) = -v.At(j,k);
        }
        break;
      }
      if (its==30){
        TExcept::Throw("no convergence in 30 svdcmp iterations");}
      x=w[l];
      nm=k-1;
      y=w[nm];
      g=rv1[nm];
      h=rv1[k];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g=NR_pythag(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+NR_SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=l;j<=nm;j++) {
        i=j+1;
        g=rv1[i];
        y=w[i];
        h=s*g;
        g=c*g;
        z=NR_pythag(f,h);
        rv1[j]=z;
        c=f/z;
        s=h/z;
        f=x*c+g*s;
        g = g*c-x*s;
        h=y*s;
        y *= c;
        for (jj=1;jj<=n;jj++) {
          x=v.At(jj,j);
          z=v.At(jj,i);
          v.At(jj,j)=x*c+z*s;
          v.At(jj,i)=z*c-x*s;
        }
        z=NR_pythag(f,h);
        w[j]=z;
        if (z) {
          z=1.0/z;
          c=f*z;
          s=h*z;
        }
        f=c*g+s*y;
        x=c*y-s*g;
        for (jj=1;jj<=m;jj++) {
          y=a.At(jj,j);
          z=a.At(jj,i);
          a.At(jj,j)=y*c+z*s;
          a.At(jj,i)=z*c-y*s;
        }
      }
      rv1[l]=0.0;
      rv1[k]=f;
      w[k]=x;
    }
  }
}

void TSvd::NR_svbksb(
 TFltVV& u, TFltV& w, TFltVV& v, int m, int n, TFltV& b, TFltV& x){
  int jj,j,i;
  double s;

  TFltV tmp(n+1);
  for (j=1;j<=n;j++) {
    s=0.0;
    if (w[j]) {
      for (i=1;i<=m;i++) s += u.At(i,j)*b[i];
      s /= w[j];
    }
    tmp[j]=s;
  }
  for (j=1;j<=n;j++) {
    s=0.0;
    for (jj=1;jj<=n;jj++) s += v.At(j,jj)*tmp[jj];
    x[j]=s;
  }
}

void TSvd::NR_svdvar(TFltVV& v, int ma, TFltV& w, TFltVV& cvm){
  int k,j,i;
  double sum;

  TFltV wti(ma+1);
  for (i=1;i<=ma;i++) {
    wti[i]=0.0;
    if (w[i]) wti[i]=1.0/(w[i]*w[i]);
  }
  for (i=1;i<=ma;i++) {
    for (j=1;j<=i;j++) {
      for (sum=0.0,k=1;k<=ma;k++) sum += v.At(i,k)*v.At(j,k)*wti[k];
      cvm.At(j,i)=cvm.At(i,j)=sum;
    }
  }
}

void TSvd::NR_svdfit(){
  int j,i;
  double wmax,tmp,thresh,sum;
  double TOL=1.0e-5;

  TFltVV u(Recs+1, Vars+1);
  TFltVV v(Vars+1, Vars+1);
  TFltV w(Vars+1);
  TFltV b(Recs+1);
  TFltV afunc(Vars+1);
  for (i=1;i<=Recs;i++) {
    GetXV(i, afunc); // (*funcs)(x[i],afunc,Vars);
    tmp=1.0/GetSig(i);
    for (j=1;j<=Vars;j++){u.At(i,j)=afunc[j]*tmp;}
    b[i]=GetY(i)*tmp;
  }
  NR_svdcmp(u,Recs,Vars,w,v);
  wmax=0.0;
  for (j=1;j<=Vars;j++){
    if (w[j] > wmax){wmax=w[j];}}
  thresh=TOL*wmax;
  for (j=1;j<=Vars;j++){
    if (double(w[j])<thresh){w[j]=0.0;}}
  NR_svbksb(u,w,v,Recs,Vars,b,CfV);
  ChiSq=0.0;
  for (i=1;i<=Recs;i++) {
    GetXV(i, afunc); // (*funcs)(x[i],afunc,Vars);
    for (sum=0.0,j=1;j<=Vars;j++){sum += CfV[j]*afunc[j];}
    ChiSq += (tmp=(GetY(i)-sum)/GetSig(i),tmp*tmp);
  }
  // covariance matrix calculation
  CovarVV.Gen(Vars+1, Vars+1);
  NR_svdvar(v, Vars, w, CovarVV);
}

void TSvd::GetCfV(TFltV& _CfV){
  _CfV=CfV; _CfV.Del(0);
}

void TSvd::GetCfUncerV(TFltV& CfUncerV){
  CfUncerV.Gen(Vars);
  for (int VarN=0; VarN<Vars; VarN++){
    CfUncerV[VarN]=GetCfUncer(VarN);
  }
}

// all vectors (matrices) start with 0 
void TSvd::Svd(const TFltVV& InMtx, TFltVV& LSingV, TFltV& SingValV, TFltVV& RSingV) {
  //LSingV = InMtx;
  LSingV.Gen(InMtx.GetYDim()+1, InMtx.GetYDim()+1);
  // create 1 based adjacency matrix
  for (int x = 0; x < InMtx.GetXDim(); x++) {
    for (int y = 0; y < InMtx.GetYDim(); y++) {
      LSingV.At(x+1, y+1) = InMtx.At(x, y);
    }
  }
  RSingV.Gen(InMtx.GetYDim()+1, InMtx.GetYDim()+1);
  SingValV.Gen(InMtx.GetYDim()+1);
  TSvd::NR_svdcmp(LSingV, InMtx.GetXDim(), InMtx.GetYDim(), SingValV, RSingV);
  // 0-th singular value/vector is full of zeros, delete it
  SingValV.Del(0);
  LSingV.DelX(0); LSingV.DelY(0);
  RSingV.DelX(0); RSingV.DelY(0);
}

// InMtx1 is 1-based (0-th row/column are full of zeros)
// returned singular vectors/values are 0 based
void TSvd::Svd1Based(const TFltVV& InMtx1, TFltVV& LSingV, TFltV& SingValV, TFltVV& RSingV) {
  LSingV = InMtx1;
  SingValV.Gen(InMtx1.GetYDim());
  RSingV.Gen(InMtx1.GetYDim(), InMtx1.GetYDim());
  TSvd::NR_svdcmp(LSingV, InMtx1.GetXDim()-1, InMtx1.GetYDim()-1, SingValV, RSingV);
  // 0-th singular value/vector is full of zeros, delete it
  SingValV.Del(0);
  LSingV.DelX(0); LSingV.DelY(0);
  RSingV.DelX(0); RSingV.DelY(0);
}

void TSvd::Wr() const {
  printf("\n%11s %21s\n","parameter","uncertainty");
  for (int i=0; i<Vars;i++){
    printf("  a[%1d] = %8.6f %12.6f\n",
     i+1, GetCf(i), GetCfUncer(i));
  }
  printf("chi-squared = %12f\n", GetChiSq());

  printf("full covariance matrix\n");
  {for (int i=0;i<Vars;i++){
    for (int j=0;j<Vars;j++){
      printf("%12f", GetCovar(i, j));}
    printf("\n");
  }}
}

