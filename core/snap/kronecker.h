#ifndef snap_kronecker_h
#define snap_kronecker_h

#include "Snap.h"

/////////////////////////////////////////////////
// Dense Kronecker Matrix
class TKroneckerLL;
typedef TPt<TKroneckerLL> PKroneckerLL;

class TKronMtx {
public:
  static const double NInf;
  static TRnd Rnd;
private:
  TInt MtxDim;
  TFltV SeedMtx;
public:
  TKronMtx() : MtxDim(-1), SeedMtx() { }
  TKronMtx(const int& Dim) : MtxDim(Dim), SeedMtx(Dim*Dim) { }
  TKronMtx(const TFltV& SeedMatrix);
  TKronMtx(const TKronMtx& Kronecker) : MtxDim(Kronecker.MtxDim), SeedMtx(Kronecker.SeedMtx) { }
  void SaveTxt(const TStr& OutFNm) const;
  TKronMtx& operator = (const TKronMtx& Kronecker);
  bool operator == (const TKronMtx& Kronecker) const { return SeedMtx==Kronecker.SeedMtx; }
  int GetPrimHashCd() const { return SeedMtx.GetPrimHashCd(); }
  int GetSecHashCd() const { return SeedMtx.GetSecHashCd(); }

  // seed matrix
  int GetDim() const { return MtxDim; }
  int Len() const { return SeedMtx.Len(); }
  bool Empty() const { return SeedMtx.Empty(); }
  bool IsProbMtx() const; // probability (not log-lihelihood) matrix

  TFltV& GetMtx() { return SeedMtx; }
  const TFltV& GetMtx() const { return SeedMtx; }
  void SetMtx(const TFltV& ParamV) { SeedMtx = ParamV; }
  void SetRndMtx(const int& MtxDim, const double& MinProb = 0.0);
  void PutAllMtx(const double& Val) { SeedMtx.PutAll(Val); }
  void GenMtx(const int& Dim) { MtxDim=Dim;  SeedMtx.Gen(Dim*Dim); }
  void SetEpsMtx(const double& Eps1, const double& Eps0, const int& Eps1Val=1, const int& Eps0Val=0);
  void SetForEdges(const int& Nodes, const int& Edges); // scales the values to allow E edges
  void AddRndNoise(const double& SDev);
  TStr GetMtxStr() const;

  const double& At(const int& Row, const int& Col) const { return SeedMtx[MtxDim*Row+Col].Val; }
  double& At(const int& Row, const int& Col) { return SeedMtx[MtxDim*Row+Col].Val; }
  const double& At(const int& ValN) const { return SeedMtx[ValN].Val; }
  double& At(const int& ValN) { return SeedMtx[ValN].Val; }

  int GetNodes(const int& NIter) const;
  int GetEdges(const int& NIter) const;
  int GetKronIter(const int& Nodes) const;
  int GetNZeroK(const PNGraph& Graph) const; // n0^k so that > Graph->GetNodes
  double GetEZero(const int& Edges, const int& KronIter) const;
  double GetMtxSum() const;
  double GetRowSum(const int& RowId) const;
  double GetColSum(const int& ColId) const;

  void ToOneMinusMtx();
  void GetLLMtx(TKronMtx& LLMtx);
  void GetProbMtx(TKronMtx& ProbMtx);
  void Swap(TKronMtx& KronMtx);

  // edge probability and log-likelihood
  double GetEdgeProb(int NId1, int NId2, const int& NKronIters) const; // given ProbMtx
  double GetNoEdgeProb(int NId1, int NId2, const int& NKronIters) const; // given ProbMtx
  double GetEdgeLL(int NId1, int NId2, const int& NKronIters) const; // given LLMtx
  double GetNoEdgeLL(int NId1, int NId2, const int& NKronIters) const; // given LLMtx
  double GetApxNoEdgeLL(int NId1, int NId2, const int& NKronIters) const; // given LLMtx
  bool IsEdgePlace(int NId1, int NId2, const int& NKronIters, const double& ProbTresh) const; // place an edge
  // derivative of edge log-likelihood
  double GetEdgeDLL(const int& ParamId, int NId1, int NId2, const int& NKronIters) const; // given LLMtx
  double GetNoEdgeDLL(const int& ParamId, int NId1, int NId2, const int& NKronIters) const; // given LLMtx
  double GetApxNoEdgeDLL(const int& ParamId, int NId1, int NId2, const int& NKronIters) const; // given LLMtx

  // edge prob from node signature
  static uint GetNodeSig(const double& OneProb = 0.5);
  double GetEdgeProb(const uint& NId1Sig, const uint& NId2Sig, const int& NIter) const;

  // from the current (probabilistic) adjacency matrix
  PNGraph GenThreshGraph(const double& Thresh) const;
  PNGraph GenRndGraph(const double& RndFact=1.0) const;

  static int GetKronIter(const int& GNodes, const int& SeedMtxSz);
  // from the seed matrix
  static PNGraph GenKronecker(const TKronMtx& SeedMtx, const int& NIter, const bool& IsDir, const int& Seed=0);
  static PNGraph GenFastKronecker(const TKronMtx& SeedMtx, const int& NIter, const bool& IsDir, const int& Seed=0);
  static PNGraph GenFastKronecker(const TKronMtx& SeedMtx, const int& NIter, const int& Edges, const bool& IsDir, const int& Seed=0);
  static PNGraph GenDetKronecker(const TKronMtx& SeedMtx, const int& NIter, const bool& IsDir);
  static void PlotCmpGraphs(const TKronMtx& SeedMtx, const PNGraph& Graph, const TStr& OutFNm, const TStr& Desc);
  static void PlotCmpGraphs(const TKronMtx& SeedMtx1, const TKronMtx& SeedMtx2, const PNGraph& Graph, const TStr& OutFNm, const TStr& Desc);
  static void PlotCmpGraphs(const TVec<TKronMtx>& SeedMtxV, const PNGraph& Graph, const TStr& FNmPref, const TStr& Desc);

  static void KronMul(const TKronMtx& LeftPt, const TKronMtx& RightPt, TKronMtx& OutMtx);
  static void KronSum(const TKronMtx& LeftPt, const TKronMtx& RightPt, TKronMtx& OutMtx); // log powering
  static void KronPwr(const TKronMtx& KronPt, const int& NIter, TKronMtx& OutMtx);

  void Dump(const TStr& MtxNm = TStr(), const bool& Sort = false) const;
  static double GetAvgAbsErr(const TKronMtx& Kron1, const TKronMtx& Kron2); // avg L1 on (sorted) parameters
  static TKronMtx GetMtx(TStr MatlabMtxStr);
  static TKronMtx GetRndMtx(const int& Dim, const double& MinProb);
  static TKronMtx GetInitMtx(const int& Dim, const int& Nodes, const int& Edges);
  static TKronMtx GetInitMtx(const TStr& MtxStr, const int& Dim, const int& Nodes, const int& Edges);
  static TKronMtx GetMtxFromNm(const TStr& MtxNm);
  static TKronMtx LoadTxt(const TStr& MtxFNm);
  static void PutRndSeed(const int& Seed) { TKronMtx::Rnd.PutSeed(Seed); }
};

/////////////////////////////////////////////////
// Kronecker Log Likelihood
class TKroneckerLL {
public:
private:
  TCRef CRef;
  PNGraph Graph;         // graph to fit
  TInt Nodes, KronIters;

  TFlt PermSwapNodeProb; // permutation proposal distribution (swap edge endpoins vs. swap random nodes)
  TIntPrV GEdgeV;        // edge vector (for swap edge permutation proposal)
  TIntV NodePerm;        // current permutation

  TKronMtx ProbMtx, LLMtx; // Prob and LL matrices (parameters)
  TFlt LogLike; // LL at ProbMtx
  TFltV GradV;  // DLL at ProbMtx (gradient)
public:
  TKroneckerLL() : Nodes(-1), KronIters(-1), PermSwapNodeProb(0.2), LogLike(TKronMtx::NInf) { }
  TKroneckerLL(const PNGraph& GraphPt, const TFltV& ParamV, const double& PermPSwapNd=0.2);
  TKroneckerLL(const PNGraph& GraphPt, const TKronMtx& ParamMtx, const double& PermPSwapNd=0.2);
  TKroneckerLL(const PNGraph& GraphPt, const TKronMtx& ParamMtx, const TIntV& NodeIdPermV, const double& PermPSwapNd=0.2);
  static PKroneckerLL New() { return new TKroneckerLL(); }
  static PKroneckerLL New(const PNGraph& GraphPt, const TKronMtx& ParamMtx, const double& PermPSwapNd=0.1);
  static PKroneckerLL New(const PNGraph& GraphPt, const TKronMtx& ParamMtx, const TIntV& NodeIdPermV, const double& PermPSwapNd=0.2);

  int GetNodes() const { return Nodes; }
  int GetKronIters() const { return KronIters; }
  PNGraph GetGraph() const { return Graph; }
  void SetGraph(const PNGraph& GraphPt);
  const TKronMtx& GetProbMtx() const { return ProbMtx; }
  const TKronMtx& GetLLMtx() const { return LLMtx; }
  int GetParams() const { return ProbMtx.Len(); }
  int GetDim() const { return ProbMtx.GetDim(); }

  // node permutation
  void SetPerm(const char& PermId);
  void SetOrderPerm(); // identity
  void SetRndPerm();   // random
  void SetDegPerm();   // node degrees
  void SetPerm(const TIntV& NodePermV) { NodePerm = NodePermV; }
  const TIntV& GetPermV() const { return NodePerm; }

  // full graph LL
  double GetFullGraphLL() const;
  double GetFullRowLL(int RowId) const;
  double GetFullColLL(int ColId) const;
  // empty graph LL
  double GetEmptyGraphLL() const;
  double GetApxEmptyGraphLL() const;
  // graph LL
  void InitLL(const TFltV& ParamV);
  void InitLL(const TKronMtx& ParamMtx);
  void InitLL(const PNGraph& GraphPt, const TKronMtx& ParamMtx);
  double CalcGraphLL();
  double CalcApxGraphLL();
  double GetLL() const { return LogLike; }
  double GetAbsErr() const { return fabs(pow((double)Graph->GetEdges(), 1.0/double(KronIters)) - ProbMtx.GetMtxSum()); }
  double NodeLLDelta(const int& NId) const;
  double SwapNodesLL(const int& NId1, const int& NId2);
  bool SampleNextPerm(int& NId1, int& NId2); // sampling from P(perm|graph)

  // derivative of the log-likelihood
  double GetEmptyGraphDLL(const int& ParamId) const;
  double GetApxEmptyGraphDLL(const int& ParamId) const;
  const TFltV& CalcGraphDLL();
  const TFltV& CalcFullApxGraphDLL();
  const TFltV& CalcApxGraphDLL();
  double NodeDLLDelta(const int ParamId, const int& NId) const;
  void UpdateGraphDLL(const int& SwapNId1, const int& SwapNId2);
  const TFltV& GetDLL() const { return GradV; }
  double GetDLL(const int& ParamId) const { return GradV[ParamId]; }

  // gradient
  void SampleGradient(const int& WarmUp, const int& NSamples, double& AvgLL, TFltV& GradV);
  double GradDescent(const int& NIter, const double& LrnRate, double MnStep, double MxStep, const int& WarmUp, const int& NSamples);
  double GradDescent2(const int& NIter, const double& LrnRate, double MnStep, double MxStep, const int& WarmUp, const int& NSamples);

  TFltV TestSamplePerm(const TStr& OutFNm, const int& WarmUp, const int& NSamples, const TKronMtx& TrueMtx, const bool& DoPlot=true);
  static double CalcChainR2(const TVec<TFltV>& ChainLLV);
  static void ChainGelmapRubinPlot(const TVec<TFltV>& ChainLLV, const TStr& OutFNm, const TStr& Desc);
  TFltQu TestKronDescent(const bool& DoExact, const bool& TruePerm, double LearnRate, const int& WarmUp, const int& NSamples, const TKronMtx& TrueParam);
  void GradDescentConvergence(const TStr& OutFNm, const TStr& Desc1, const bool& SamplePerm, const int& NIters,
    double LearnRate, const int& WarmUp, const int& NSamples, const int& AvgKGraphs, const TKronMtx& TrueParam);
  static void TestBicCriterion(const TStr& OutFNm, const TStr& Desc1, const PNGraph& G, const int& GradIters,
    double LearnRate, const int& WarmUp, const int& NSamples, const int& TrueN0);
  static void TestGradDescent(const int& KronIters, const int& KiloSamples, const TStr& Permutation);
  friend class TPt<TKroneckerLL>;
};

/////////////////////////////////////////////////
// Kronecker Log Likelihood Maximization
class TKronMaxLL {
public:
  class TFEval {
  public:
    TFlt LogLike;
    TFltV GradV;
  public:
    TFEval() : LogLike(0), GradV() { }
    TFEval(const TFlt& LL, const TFltV& DLL) : LogLike(LL), GradV(DLL) { }
    TFEval(const TFEval& FEval) : LogLike(FEval.LogLike), GradV(FEval.GradV) { }
    TFEval& operator = (const TFEval& FEval) { if (this!=&FEval) {
      LogLike=FEval.LogLike; GradV=FEval.GradV; } return *this; }
  };
private:
  //TInt WarmUp, NSamples;
  THash<TKronMtx, TFEval> FEvalH; // cached gradients
  TKroneckerLL KronLL;
public:
  TKronMaxLL(const PNGraph& GraphPt, const TKronMtx& StartParam) : KronLL(GraphPt, StartParam) { }
  void SetPerm(const char& PermId);

  void GradDescent(const int& NIter, const double& LrnRate, const double& MnStep, const double& MxStep,
    const double& WarmUp, const double& NSamples);

  /*void EvalNewEdgeP(const TKronMtx& ProbMtx);
  double GetLL(const TFltV& ThetaV);
  void GetDLL(const TFltV& ThetaV, TFltV& DerivV);
  double GetDLL(const TFltV& ThetaV, const int& ParamId);
  //void MaximizeLL(const int& NWarmUp, const int& Samples);//*/

  static void RoundTheta(const TFltV& ThetaV, TFltV& NewThetaV);
  static void RoundTheta(const TFltV& ThetaV, TKronMtx& Kronecker);

  static void Test();
};

/////////////////////////////////////////////////
// Kronecker Fitting using Metrod of Moments (by Art Owen)
class TKronMomentsFit {
public:
  double Edges, Hairpins, Tripins, Triads;
public:
  TKronMomentsFit(const PUNGraph& G) {
    Edges=0; Hairpins=0; Tripins=0; Triads=0;
    for (TUNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      const int d = NI.GetOutDeg();
      Edges += d;
      Hairpins += d*(d-1.0);
      Tripins += d*(d-1.0)*(d-2.0);
    }
    Edges /= 2.0;
    Hairpins /= 2.0;
    Tripins /= 6.0;
    int ot,ct;
    Triads = TSnap::GetTriads(G, ot, ct)/3.0;
    printf("E:%g\tH:%g\tT:%g\tD:%g\n", Edges, Hairpins, Tripins, Triads);
  }

  TFltQu EstABC(const int& R) {
    const double Step = 0.01;
    double MinScore=TFlt::Mx;
    double A=0, B=0, C=0;
    //Edges=log(Edges);  Hairpins=log(Hairpins);  Tripins=log(Tripins);  Triads=log(Triads);
    for (double a = 1.0; a > Step; a-=Step) {
      for (double b = Step; b <= 1.0; b+=Step) {
        for (double c = Step; c <= a; c+=Step) {
          double EE = ( pow(a+2*b+c, R) - pow(a+c, R) ) / 2.0;
          double EH = ( pow(pow(a+b,2) + pow(b+c,2), R)
                             -2*pow(a*(a+b)+c*(c+b), R)
                             -pow(a*a + 2*b*b + c*c, R)
                             +2*pow(a*a + c*c, R) ) / 2.0;
          double ET = ( pow(pow(a+b,3)+pow(b+c,3), R)
                             -3*pow(a*pow(a+b,2)+c*pow(b+c,2), R)
                             -3*pow(a*a*a + c*c*c + b*(a*a+c*c) + b*b*(a+c) + 2*b*b*b ,R)
                             +2*pow(a*a*a + 2*b*b*b + c*c*c, R)
                             +5*pow(a*a*a + c*c*c + b*b*(a+c), R)
                             +4*pow(a*a*a + c*c*c + b*(a*a+c*c), R)
                             -6*pow(a*a*a + c*c*c, R) ) / 6.0;
          double ED = ( pow(a*a*a + 3*b*b*(a+c) + c*c*c, R)
                             -3*pow(a*(a*a+b*b) + c*(b*b+c*c), R)
                             +2*pow(a*a*a+c*c*c, R) ) / 6.0;
          if (EE < 0) { EE = 1; }
          if (EH < 0) { EH = 1; }
          if (ET < 0) { ET = 1; }
          if (ED < 0) { ED = 1; }
          //EE=log(EE); EH=log(EH); ET=log(ET); ED=log(ED);
          double Score = pow(Edges-EE,2)/EE + pow(Hairpins-EH ,2)/EH + pow(Tripins-ET, 2)/ET + pow(Triads-ED, 2)/ED;
          //double Score = fabs(Edges-EE)/EE + fabs(Hairpins-EH)/EH + fabs(Tripins-ET)/ET + fabs(Triads-ED)/ED;
          //double Score = log(pow(Edges-EE,2)/EE) + log(pow(Hairpins-EH,2)/EH) + log(pow(Tripins-ET, 2)/ET) + log(pow(Triads-ED, 2)/ED);
          if (MinScore > Score || (a==0.9 && b==0.6 && c==0.2) || (TMath::IsInEps(a-0.99,1e-6) && TMath::IsInEps(b-0.57,1e-6) && TMath::IsInEps(c-0.05,1e-6)))
          {
            printf("%.03f %.03f %0.03f %10.4f  %10.10g\t%10.10g\t%10.10g\t%10.10g\n", a,b,c, log10(Score), EE, EH, ET, ED);
            //printf("%.03f %.03f %0.03f %g\n", a,b,c, log(Score));
            A=a; B=b; C=c; MinScore=Score;
          }
        }
      }
    }
    printf("\t\t\t      %10.10g\t%10.10g\t%10.10g\t%10.10g\n", Edges, Hairpins, Tripins, Triads);
    return TFltQu(A,B,C,MinScore);
  }

  static void Test() {
    TFIn FIn("as20.ngraph");
    PUNGraph G = TSnap::ConvertGraph<PUNGraph>(TNGraph::Load(FIn));
    //PUNGraph G = TKronMtx::GenFastKronecker(TKronMtx::GetMtx("0.9, 0.6; 0.6, 0.2"), 14, false, 0)->GetUNGraph();
    //PUNGraph G = TUNGraph::GetSmallGraph();
    TSnap::PrintInfo(G);
    TSnap::DelSelfEdges(G);
    TSnap::PrintInfo(G);
    TKronMomentsFit Fit(G);
    printf("iter %d\n", TKronMtx::GetKronIter(G->GetNodes(), 2));
    Fit.EstABC(TKronMtx::GetKronIter(G->GetNodes(), 2)); //*/
  }
};


/////////////////////////////////////////////////
// Kronecker Phase Plot
/*class TKronPhasePlot {
public:
  class TPhasePoint {
  public:
    TFlt Alpha, Beta;
    TGrowthStat GrowthStat;
  public:
    TPhasePoint() { }
    TPhasePoint(const double& A, const double& B, const TGrowthStat& GS) : Alpha(A), Beta(B), GrowthStat(GS) { }
    TPhasePoint(TSIn& SIn) : Alpha(SIn), Beta(SIn), GrowthStat(SIn) { }
    void Save(TSOut& SOut) const { Alpha.Save(SOut);  Beta.Save(SOut);  GrowthStat.Save(SOut); }
  };
  typedef TVec<TPhasePoint> TPhasePointV;
public:
  TPhasePointV PhaseV;
public:
  TKronPhasePlot() { }
  TKronPhasePlot(const TKronPhasePlot& Phase) : PhaseV(Phase.PhaseV) { }
  TKronPhasePlot(TSIn& SIn) : PhaseV(SIn) { }
  void Save(TSOut& SOut) const { PhaseV.Save(SOut);  }
  void SaveMatlab(const TStr& OutFNm) const;

  static void KroneckerPhase(const TStr& MtxId, const int& MxIter,
    const double& MnAlpha, const double& MxAlpha, const double& AlphaStep,
    const double& MnBeta, const double& MxBeta, const double& BetaStep,
    const TStr& FNmPref);
};*/

/*// for conjugate gradient
  class TFunc {
  private:
    TKronMaxLL* CallBack;
  public:
    TFunc(TKronMaxLL* CallBackPt) : CallBack(CallBackPt) { }
    TFunc(const TFunc& Func) : CallBack(Func.CallBack) { }
    TFunc& operator = (const TFunc& Func) { CallBack=Func.CallBack; return *this; }
    double FVal(const TFltV& Point) { return -CallBack->GetLL(Point); }
    void FDeriv(const TFltV& Point, TFltV& DerivV);
  };
public:
  // log barier
  class TLogBarFunc {
  private:
    TKronMaxLL* CallBack;
    double T;
  public:
    TLogBarFunc(TKronMaxLL* CallBackPt, const double& t=0.5) : CallBack(CallBackPt), T(t) { }
    TLogBarFunc(const TLogBarFunc& Func) : CallBack(Func.CallBack), T(Func.T) { }
    TLogBarFunc& operator = (const TLogBarFunc& Func) { CallBack=Func.CallBack; T=Func.T; return *this; }
    double FVal(const TFltV& Point);
    void FDeriv(const TFltV& Point, TFltV& DerivV);
  };*/

#endif
