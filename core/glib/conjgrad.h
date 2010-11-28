#ifndef conj_grad_h
#define conj_grad_h

/////////////////////////////////////////////////
// Function for Conjugate Gradient Minimization
//class TFunction {
//public:
//  double FVal(const TFltV& Point); // function evaluated at Point
//  void FDeriv(const TFltV& Point, TFltV& DerivV); // minus function derivative evaluated at Point
//};

/////////////////////////////////////////////////
// Conjugate Gradient Function Minimization
// (section 10.6 of Numerical Recipies)
template <class TFunction>
class TConjGrad {
private:
  TFunction Func;  // function value and derivative
  TFltV Point;     // starting/mimimum point
  TInt NIters;     // number of iteratons
  TFlt MinFVal;    // function value at minimum
  UndefDefaultCopyAssign(TConjGrad);
public:
  TConjGrad(const TFltV& StartPoint) : Point(StartPoint), NIters(0), MinFVal(TFlt::Mx) { }
  TConjGrad(const TFltV& StartPoint, const TFunction& Function) : Point(StartPoint), Func(Function) { }

  void SetStart(const TFltV& StartPoint) { Point=StartPoint; NIters=0; MinFVal=TFlt::Mx; }
  double At(const int& PointN) const { return Point[PointN]; }
  double GetMinVal() const { return MinFVal; }
  int GetIters() const { return NIters; }
  const TFltV& GetMinPoint() const { return Point; }

  TFunction& GetFunc() { return Func; }
  const TFunction& GetFunc() const { return Func; }
  void SetFunc(const TFunction& Function) { Func=Function; }

  void ConjGradMin(const double& Tolerance = 1e-6);
  // helper functions
  void dlinmin(TFltV& p, TFltV& xi);
  void mnbrak(double& ax, double& bx, double& cx, double& fa, double& fb, double& fc, const TFltV& pcom, const TFltV& xicom);
  double dbrent(double ax, double bx, double cx, double tol, double& xmin, const TFltV& pcom, const TFltV& xicom);
  double f1dim(double x, const TFltV& pcom, const TFltV& xicom);
  double df1dim(double x, const TFltV& pcom, const TFltV& xicom);
  static void Shift(double& a, double& b, double& c, const double& d) { a=b; b=c; c=d; }
  static double Sign(const double a, const double b) { return b >= 0.0 ? fabs(a) : -fabs(a); }
  static void Mov3(double& a,double& b,double& c, const double& d,const double& e,const double& f) { a=d; b=e; c=f; }
  static void Dump(const TFltV& ValV) { for (int i = 0; i < ValV.Len(); i++) { printf("  %d:%g", i, ValV[i]); } printf("\n"); }
};

// Fletcher-Reeves-Polak-Ribiere minimization (Numerical Recipies section 10.6)
template <class TFunction>
void TConjGrad<TFunction>::ConjGradMin(const double& Tolerance) {
  const double EPS = 1e-10;
  const double ITMAX = 200;
  const int N = Point.Len();
  double gg, gam, fp, dgg;
  TFltV g(N), h(N), xi(N);
  fp = Func.FVal(Point);
  Func.FDeriv(Point, xi);
  for (int j = 0; j < N; j++) {
    g[j] = -xi[j];
    xi[j]=h[j]=g[j];
  }
  for (int NIters=1; NIters <= ITMAX; NIters++) {
    Func.Dump();
    dlinmin(Point, xi);
    //if (2.0*fabs(MinFVal-fp) <= Tolerance*(fabs(MinFVal)+fabs(fp)+EPS)) {
    if (fabs(MinFVal-fp) <= Tolerance) { // less strict stopping criteria
      return;
    }
    fp = Func.FVal(Point);
    Func.FDeriv(Point, xi);
    dgg=gg=0.0;
    for (int j = 0; j < N; j++) {
      gg += g[j]*g[j];
      dgg += (xi[j]+g[j])*xi[j];
    }
    if (gg == 0.0) { return; }
    gam = dgg / gg;
    for (int j = 0; j < N; j++) {
      g[j] = -xi[j];
      xi[j]=h[j]=g[j]+gam*h[j];
    }
  }
  FailR("Too many iterations in TConjGrad::ConjGradMin");
}

//void dlinmin(double p[], double xi[], int n, double *fret, double (*func)(double []), void (*dfunc)(double [], double [])) {
template <class TFunction>
void TConjGrad<TFunction>::dlinmin(TFltV& p, TFltV& xi) {
  const double TOL = 2.0e-4;
  double xx=1.0, xmin, fx, fb, fa, bx, ax=0.0;
  /*// set xx so that we do not look over the 0...1 interval 
  // (since we work with probabilities
  for (int i = 0; i < xi.Len(); i++) {
    if (p[i]+xx*xi[i] > 0.95) { xx = (0.95-p[i])/xi[i]; }
    if (p[i]+xx*xi[i] < 0.05) { xx = (0.05-p[i])/xi[i]; }
  }
  if (xx == 0) { xx = 0.01; printf("!!!");}*/
  //if (xx != 1.0) { printf("***XX: %f\n"); }
  mnbrak(ax, xx, bx, fa, fx, fb, p, xi);
  MinFVal = dbrent(ax, xx, bx, TOL, xmin, p, xi);
  if (MinFVal == 0.0) {
    return;
  } else {
    // move the point
    for (int j = 0; j < xi.Len(); j++) {
      xi[j] *= xmin;
      p[j] += xi[j];
    }
  }
}

template <class TFunction>
void TConjGrad<TFunction>::mnbrak(double& ax, double& bx, double& cx, double& fa, double& fb, double& fc, const TFltV& pcom, const TFltV& xicom) {
  const double GOLD = 1.618034;
  const double GLIMIT = 100.0;
  const double TINY = 1.0e-20;
  double ulim,u,r,q,fu,dum;
  fa = f1dim(ax, pcom, xicom);
  fb = f1dim(bx, pcom, xicom);
  if (fb > fa) {
    Shift(dum, ax, bx, dum);
    Shift(dum, fb, fa, dum);
  }
  cx=(bx)+GOLD*(bx-ax);
  fc=f1dim(cx, pcom, xicom);
  while (fb > fc) {
    r=(bx-ax)*(fb-fc);
    q=(bx-cx)*(fb-fa);
    u=(bx)-((bx-cx)*q-(bx-ax)*r)/(2.0*Sign(TMath::Mx(fabs(q-r),TINY),q-r));
    ulim=(bx)+GLIMIT*(cx-bx);
    if ((bx-u)*(u-cx) > 0.0) {
      fu=f1dim(u, pcom, xicom);
      if (fu < fc) {
        ax=bx;
        bx=u;
        fa=fb;
        fb=fu;
        return;
      } else if (fu > fb) {
        cx=u;
        fc=fu;
        return;
      }
      u=(cx)+GOLD*(cx-bx);
      fu=f1dim(u, pcom, xicom);
    } else if ((cx-u)*(u-ulim) > 0.0) {
      fu=f1dim(u, pcom, xicom);
      if (fu < fc) {
        Shift(bx,cx,u,cx+GOLD*(cx-bx));
        Shift(fb,fc,fu,f1dim(u, pcom, xicom));
      }
    } else if ((u-ulim)*(ulim-cx) >= 0.0) {
      u=ulim;
      fu=f1dim(u, pcom, xicom);
    } else {
      u=cx+GOLD*(cx-bx);
      fu=f1dim(u, pcom, xicom);
    }
    Shift(ax,bx,cx,u);
    Shift(fa,fb,fc,fu);
  }
}

template <class TFunction>
double TConjGrad<TFunction>::dbrent(double ax, double bx, double cx, double tol, double& xmin, const TFltV& pcom, const TFltV& xicom) {
  const double ITMAX = 100;
  const double ZEPS = 1.0e-10;
  int iter,ok1,ok2;
  double a,b,d,d1,d2,du,dv,dw,dx,e=0.0;
  double fu,fv,fw,fx,olde,tol1,tol2,u,u1,u2,v,w,x,xm;

  a=(ax < cx ? ax : cx);
  b=(ax > cx ? ax : cx);
  x=w=v=bx;
  fw=fv=fx=f1dim(x, pcom, xicom);
  dw=dv=dx=df1dim(x, pcom, xicom);
  for (iter=1;iter<=ITMAX;iter++) {
    xm=0.5*(a+b);
    tol1=tol*fabs(x)+ZEPS;
    tol2=2.0*tol1;
    if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
      xmin=x;
      return fx;
    }
    if (fabs(e) > tol1) {
      d1=2.0*(b-a);
      d2=d1;
      if (dw != dx) d1=(w-x)*dx/(dx-dw);
      if (dv != dx) d2=(v-x)*dx/(dx-dv);
      u1=x+d1;
      u2=x+d2;
      ok1 = (a-u1)*(u1-b) > 0.0 && dx*d1 <= 0.0;
      ok2 = (a-u2)*(u2-b) > 0.0 && dx*d2 <= 0.0;
      olde=e;
      e=d;
      if (ok1 || ok2) {
        if (ok1 && ok2)
          d=(fabs(d1) < fabs(d2) ? d1 : d2);
        else if (ok1)
          d=d1;
        else
          d=d2;
        if (fabs(d) <= fabs(0.5*olde)) {
          u=x+d;
          if (u-a < tol2 || b-u < tol2)
            d=Sign(tol1,xm-x);
        } else {
          d=0.5*(e=(dx >= 0.0 ? a-x : b-x));
        }
      } else {
        d=0.5*(e=(dx >= 0.0 ? a-x : b-x));
      }
    } else {
      d=0.5*(e=(dx >= 0.0 ? a-x : b-x));
    }
    if (fabs(d) >= tol1) {
      u=x+d;
      fu=f1dim(u, pcom, xicom);
    } else {
      u=x+Sign(tol1,d);
      fu=f1dim(u, pcom, xicom);
      if (fu > fx) {
        xmin=x;
        return fx;
      }
    }
    du=df1dim(u, pcom, xicom);
    if (fu <= fx) {
      if (u >= x) a=x; else b=x;
      Mov3(v,fv,dv, w,fw,dw);
      Mov3(w,fw,dw, x,fx,dx);
      Mov3(x,fx,dx, u,fu,du);
    } else {
      if (u < x) a=u; else b=u;
      if (fu <= fw || w == x) {
        Mov3(v,fv,dv, w,fw,dw);
        Mov3(w,fw,dw, u,fu,du);
      } else if (fu < fv || v == x || v == w) {
        Mov3(v,fv,dv, u,fu,du);
      }
    }
  }
  xmin=-xm;
  printf("XMin: %f\n", xmin);
  printf("Point:");  Dump(pcom);
  printf("Deriv:");  Dump(xicom);
  //FailR("Too many iterations in routine TConjGrad::dbrent");
  return 0.0;
}

template <class TFunction>
double TConjGrad<TFunction>::f1dim(double x, const TFltV& pcom, const TFltV& xicom) {
  const int ncom = pcom.Len();
  TFltV xt(ncom);
  for (int j=0; j<ncom; j++) xt[j] = pcom[j] + x*xicom[j];
  return Func.FVal(xt);
}

template <class TFunction>
double TConjGrad<TFunction>::df1dim(double x, const TFltV& pcom, const TFltV& xicom) {
  const int ncom = pcom.Len();
  double df1=0.0;
  TFltV xt(ncom), df(ncom);
  for (int j=0; j<ncom; j++) xt[j]=pcom[j]+x*xicom[j];
  Func.FDeriv(xt, df);
  for (int j=0; j<ncom; j++) df1 += df[j]*xicom[j];
  return df1;
}

#endif