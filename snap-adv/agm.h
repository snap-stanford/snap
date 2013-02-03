#ifndef snap_agm_h
#define snap_agm_h
#include "Snap.h"

/////////////////////////////////////////////////
/// Affiliaiton Graph Model (AGM) graph generator.
class TAGM {
public:
  static void RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd);
  static PUNGraph GenAGM(const TIntV& NIdV , THash<TInt,TIntV >& CmtyVH, const TStr& AGMNm, const double& PiCoef, const double& ProbBase, TRnd& Rnd=TInt::Rnd);
  static PUNGraph GenAGM(TVec<TIntV >& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd=TInt::Rnd);
  static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const int TargetEdges, TRnd& Rnd);
  static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const TFltV& CProbV, TRnd& Rnd, const double PNoCom = -1.0);
};

/////////////////////////////////////////////////
/// Affiliaiton Graph Model (AGM) graph generator.
class TAGMUtil {
public:
  static void GenPLSeq(TIntV& SzSeq,const int& SeqLen, const double& Alpha, TRnd& Rnd, const int& Min, const int& Max);
  static void ConnectCmtyVV(TVec<TIntV>& CmtyVV, const TIntPrV& CIDSzPrV, const TIntPrV& NIDMemPrV, TRnd& Rnd);
  static void GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const PUNGraph& Graph, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd);
  static void GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const TIntV& NIDV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd);
  static void GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd);
  static void RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd);
  static PUNGraph GenAGM(TVec<TIntV >& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd=TInt::Rnd);
  static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const int TargetEdges, TRnd& Rnd);
  static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const TFltV& CProbV, TRnd& Rnd, const double PNoCom = -1.0);
  static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntV>& CmtyVV);
  static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntV>& CmtyVV, const TIntV& NIDV);
  static void GetNodeMembership(TIntH& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
  static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntSet>& CmtyVV);
  static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
  static void GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
  static void GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const TVec<TIntV >& CmtyVV);
  static void GetNodeMembership(TIntH& NIDComVH, const TVec<TIntV >& CmtyVV);
  static void LoadCmtyVV(const TStr& InFNm, TVec<TIntV>& CmtyVV);
  static void DumpCmtyVV(const TStr& OutFNm, const TVec<TIntV>& CmtyVV);
  static void DumpCmtyVV(const TStr OutFNm, TVec<TIntV>& CmtyVV, TIntStrH& NIDNmH);
  static int TotalMemberships(const TVec<TIntV>& CmtyVV);
  static void RewireCmtyVV(const TVec<TIntV>& CmtyVVIn, TVec<TIntV>& CmtyVVOut, TRnd& Rnd);
  static void RewireCmtyNID(THash<TInt,TIntV >& CmtyVH, TRnd& Rnd);
  static int Intersection(const TIntV& C1, const TIntV& C2);
  static void GetIntersection(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& C);
  static int Intersection(const THashSet<TInt>& A, const THashSet<TInt>& B);
  static double GetConductance(const PUNGraph& Graph, const TIntSet& CmtyS, const int Edges);
  static void GetNbhCom(const PUNGraph& Graph, const int NID, TIntSet& NBCmtyS);
  static void SaveGephi(const TStr& OutFNm, const PUNGraph& G, const TVec<TIntV >& CmtyVVAtr, const double MaxSz, const double MinSz) {
    THash<TInt, TStr> TmpH;
    SaveGephi(OutFNm, G, CmtyVVAtr, MaxSz, MinSz, TmpH);
  }
  static void SaveGephi(const TStr& OutFNm, const PUNGraph& G, const TVec<TIntV >& CmtyVVAtr, const double MaxSz, const double MinSz, const THash<TInt, TStr>& NIDNameH) { 
    THash<TInt, TIntTr> TmpH; 
    SaveGephi(OutFNm, G, CmtyVVAtr, MaxSz, MinSz, NIDNameH, TmpH);
  }
  static void SaveGephi(const TStr& OutFNm, const PUNGraph& G, const TVec<TIntV >& CmtyVVAtr, const double MaxSz, const double MinSz, const THash<TInt, TStr>& NIDNameH, const THash<TInt, TIntTr >& NIDColorH);
  static void SaveBipartiteGephi(const TStr& OutFNm, const TIntV& NIDV, const TVec<TIntV>& CmtyVV, const double MaxSz, const double MinSz, const TIntStrH& NIDNameH, const THash<TInt, TIntTr >& NIDColorH, const THash<TInt, TIntTr >& CIDColorH);
  static int FindComsByAGM(const PUNGraph& Graph, const int InitComs, const int MaxIter, const int RndSeed, const double RegGap, const double PNoCom = 0.0, const TStr PltFPrx = TStr());
  template <class PGraph>
  static PGraph LoadEdgeListStr(const TStr& InFNm, TIntStrH& NIDNameH, const int& SrcColId = 0, const int& DstColId = 1, const TSsFmt SsFmt = ssfTabSep) {
    TSsParser Ss(InFNm, SsFmt);
    PGraph Graph = PGraph::TObj::New();
    TStrHash<TInt> StrSet(Mega(1), true);
    while (Ss.Next()) {
      const int SrcNId = StrSet.AddKey(Ss[SrcColId]);
      const int DstNId = StrSet.AddKey(Ss[DstColId]);
      if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
      if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
      Graph->AddEdge(SrcNId, DstNId);
    }
    NIDNameH.Gen(StrSet.Len());
    for (int s = 0; s < StrSet.Len(); s++) { NIDNameH.AddDat(s, StrSet.GetKey(s)); }
    IAssert(NIDNameH.Len() == Graph->GetNodes());

    Graph->Defrag();
    return Graph;
  }

  template<class PGraph>
  static void GVizComGraph(const PGraph& Graph,const TVec<TIntV >& CmtyVV, const TStr& OutFNm, const TStr& Desc = TStr()){
    TStrV Colors = TStrV::GetV("red","blue","green","pink","cyan");
    TStrV Shapes = TStrV::GetV("ellipse","triangle","square","pentagon","hexagon");
    THash<TInt,TIntV> NIDComVH;
    GetNodeMembership(NIDComVH,CmtyVV);

    const TStr Ext = OutFNm.GetFExt();
    const TStr GraphFNm = OutFNm.GetSubStr(0, OutFNm.Len() - Ext.Len()) + "dot";
    const bool IsDir = HasGraphFlag(typename PGraph::TObj, gfDirected);
    FILE *F = fopen(GraphFNm.CStr(), "wt");
    if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
    if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
    fprintf(F, "  graph [splines=false overlap=false]\n"); //size=\"12,10\" ratio=fill
    fprintf(F, "  node  [width=0.3, height=0.3]\n");
    //nodes
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      int NID = NI.GetId();
      TIntV& CIDV = NIDComVH.GetDat(NID);
      IAssert(CIDV.Len()>0);
      TStr ShapeNm = Shapes[(CIDV.Len()-1) % Shapes.Len()];
      TStr ColorNm = Colors[CIDV[0] % Colors.Len()];
      TStr NodeComLabel = TStr::Fmt("%d(",NID);
      for(int i=0;i<CIDV.Len();i++) {
        TStr TmpStr = TStr::Fmt("%d",int(CIDV[i]));
        NodeComLabel += TmpStr;
        if(i<CIDV.Len()-1){NodeComLabel+=",";}
      }
      NodeComLabel += ")";
      fprintf(F, "  %d [style=filled, shape=\"%s\" fillcolor=\"%s\" label=\"%s\"];\n", NI.GetId(), ShapeNm.CStr(),ColorNm.CStr(), NodeComLabel.CStr()); 
    }
  
    // edges
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      if (NI.GetOutDeg()==0 && NI.GetInDeg()==0  ) { 
        fprintf(F, "%d;\n", NI.GetId()); }
      else {
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          if (! IsDir && NI.GetId() > NI.GetOutNId(e)) { continue; }
          fprintf(F, "  %d %s %d;\n", NI.GetId(), IsDir?"->":"--", NI.GetOutNId(e)); 
        }
      }
    }
    if (! Desc.Empty()) {
      fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
      fprintf(F, "  fontsize=24;\n");
    }
    fprintf(F, "}\n");
    fclose(F);
    TSnap::TSnapDetail::GVizDoLayout(GraphFNm, OutFNm, gvlNeato);
  }
};

//Light version of logistic regression (originally in Snap)-- Jaewon Yang, 04/07/12
// forward
class TLogRegPredict;
typedef TPt<TLogRegPredict> PLogRegPredict;

///////////////////////////////////////////////////////////////////////
// Logistic Regression using gradient descent
// X: N * M matrix where N = number of examples and M = number of features.
//J: JAEWON THIS HAS NOTHING TO DO WITH AGM. THIS NEEDS TO BE MOVED SOMEWHERE ELSE (glib/xmath.h)
//J: BUT I THINK THERE IS ALREADY A LOGISTIC REGRESSION CLASS THERE!!
class TLogRegFit {
private:
  TVec<TFltV> X;
  TFltV Y;
  TFltV Theta;
  int M; // number of features
public:
  TLogRegFit() {}
  ~TLogRegFit() {}
  // returns model for logistic regression
  PLogRegPredict CalcLogRegGradient(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm = TStr(), const double& ChangeEps = 0.01, const int& MaxStep = 200, const bool InterceptPt = false);
  PLogRegPredict CalcLogRegNewton(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm = TStr(), const double& ChangeEps = 0.01, const int& MaxStep = 200, const bool InterceptPt = false);
  int MLEGradient(const double& ChangeEps, const int& MaxStep, const TStr PlotNm);
  int MLENewton(const double& ChangeEps, const int& MaxStep, const TStr PlotNm);
  double GetStepSizeByLineSearch(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta);
  double Likelihood(const TFltV& NewTheta);
  double Likelihood() { return Likelihood(Theta); }
  void Gradient(TFltV& GradV);
  void Hessian(TFltVV& HVV);
  void GetNewtonStep(TFltVV& HVV, const TFltV& GradV, TFltV& DeltaLV);
};


///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model
//J: JAEWON THIS HAS NOTHING TO DO WITH AGM. THIS NEEDS TO BE MOVED SOMEWHERE ELSE (glib/xmath.h)
//J: BUT I THINK THERE IS ALREADY A LOGISTIC REGRESSION CLASS THERE!!
class TLogRegPredict {
private: 
    TCRef CRef;
private:
    TFltV Theta;
public:
    TLogRegPredict(const TFltV& _bb): Theta(_bb) { };
    //static PLogRegMd New(PSparseTrainSet Set) { return TLogReg::CalcLogReg(Set); }; //TODO

    TLogRegPredict(TSIn& SIn) { Theta.Load(SIn); };
    static PLogRegPredict Load(TSIn& SIn) { return new TLogRegPredict(SIn); };
    void Save(TSOut& SOut) const { Theta.Save(SOut); };

    UndefDefaultCopyAssign(TLogRegPredict);
public:    
    // classifies vector, returns probability that AttrV is positive
  static void GetCfy(const TVec<TFltV>& X, TFltV& OutV, const TFltV& NewTheta);
  static double GetCfy(const TFltV& AttrV, const TFltV& NewTheta);
  double GetCfy(const TFltV& AttrV) { return GetCfy(AttrV, Theta); }
  void GetTheta(TFltV& _Theta) { _Theta = Theta; }
  void GetCfy(const TVec<TFltV>& X, TFltV& OutV) { GetCfy(X, OutV, Theta); }
  void PrintTheta() { for (int t = 0; t < Theta.Len(); t++) { printf("Theta[%d] = %f\n", t, Theta[t].Val); } }
  friend class TPt<TLogRegPredict>;
};

#endif
