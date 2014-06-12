#ifndef yanglib_agmdirected_h
#define yanglib_agmdirected_h
#include "Snap.h"
#include "agm.h"
#include "agmfast.h"

class TCoda { //sparse AGM-fast with coordinate ascent for directed affiliation
private:
  PNGraph G; //graph to fit
  TVec<TIntFltH> F; // outdegree membership for each user (Size: Nodes * Coms)
  TVec<TIntFltH> H; // in-degree membership for each user (Size: Nodes * Coms) A ~ F * H'
  TRnd Rnd; // random number generator
  TIntV NIDV; // original node ID vector
  TFlt RegCoef; //Regularization coefficient when we fit for P_c +: L1, -: L2
  TFltV SumFV; // sum_u F_uc for each community c. Needed for efficient calculation
  TFltV SumHV; // sum_u H_uc for each community c. Needed for efficient calculation
  TBool NodesOk; // Node ID is from 0 ~ N-1
  TInt NumComs; // number of communities
  TVec<TIntSet> HOVIDSV; //NID pairs to hold out for cross validation
public:
  TFlt MinVal; // minimum value of F (0)
  TFlt MaxVal; // maximum value of F (for numerical reason)
  TFlt NegWgt; // weight of negative example (a pair of nodes without an edge)
  TFlt PNoCom; // base probability \varepsilon (edge probability between a pair of nodes sharing no community
  TBool DoParallel; // whether to use parallelism for computation

  TCoda(const PNGraph& GraphPt, const int& InitComs, const int RndSeed = 0): Rnd(RndSeed), RegCoef(0), 
    NodesOk(true), MinVal(0.0), MaxVal(1000.0), NegWgt(1.0) { SetGraph(GraphPt); RandomInit(InitComs); }
  TCoda() { G = TNGraph::New(); }
  void SetGraph(const PNGraph& GraphPt);
  PNGraph GetGraph() { return G; }
  PNGraph GetGraphRawNID();
  void SetRegCoef(const double _RegCoef) { RegCoef = _RegCoef; }
  double GetRegCoef() { return RegCoef; }
  void RandomInit(const int InitComs);
  int GetNumComs() { return NumComs.Val; }
  void NeighborComInit(const int InitComs);
  void NeighborComInit(TFltIntPrV& NIdPhiV, const int InitComs);
  void SetCmtyVV(const TVec<TIntV>& CmtyVVOut, const TVec<TIntV>& CmtyVVIn);
  double Likelihood(const bool DoParallel = false);
  double LikelihoodForNode(const bool IsRow, const int UID);
  double LikelihoodForNode(const bool IsRow, const int UID, const TIntFltH& FU);
  void GetNonEdgePairScores(TFltIntIntTrV& ScoreV);
  void GetNIDValH(TIntFltH& NIdValInOutH, TIntFltH& NIdValOutH, TIntFltH& NIdValInH, const int CID, const double Thres);
  void DumpMemberships(const TStr& OutFNm, const TStrHash<TInt>& NodeNameH) { DumpMemberships(OutFNm, NodeNameH, sqrt(PNoCom)); }
  void DumpMemberships(const TStr& OutFNm, const TStrHash<TInt>& NodeNameH, const double Thres);
  void GetCmtyS(TIntSet& CmtySOut, TIntSet& CmtySIn, const int CID, const double Thres);
  void DumpMemberships(const TStr& OutFNm, const double Thres);
  void DumpMemberships(const TStr& OutFNm) { DumpMemberships(OutFNm, sqrt(PNoCom)); }
  void GetCommunity(TIntV& CmtyVIn, TIntV& CmtyVOut, const int CID) { GetCommunity(CmtyVIn, CmtyVOut, CID, sqrt(PNoCom)); }
  void GetCommunity(TIntV& CmtyVIn, TIntV& CmtyVOut, const int CID, const double Thres);
  void GetTopCIDs(TIntV& CIdV, const int TopK, const int IsAverage = 1, const int MinSz = 1);
  void GradientForNode(const bool IsRow, const int UID, TIntFltH& GradU, const TIntSet& CIDSet);
  void SetHoldOut(const double HOFrac) { TVec<TIntSet> HoldOut; TAGMFastUtil::GenHoldOutPairs(G, HoldOut, HOFrac, Rnd); HOVIDSV = HoldOut; }
  //double LikelihoodForRow(const int UID);
  //double LikelihoodForRow(const int UID, const TIntFltH& FU);
  //double LikelihoodForCol(const int VID);
  //double LikelihoodForCol(const int VID, const TIntFltH& HV);
  //void GradientForRow(const int UID, TIntFltH& GradU, const TIntSet& CIDSet);
  void GetCmtyVV(TVec<TIntV>& CmtyVVOut, TVec<TIntV>& CmtyVVIn, const int MinSz = 3);
  void GetCmtyVV(TVec<TIntV>& CmtyVVOut, TVec<TIntV>& CmtyVVIn, const double ThresOut, const double ThresIn, const int MinSz = 3);
  void GetCmtyVV(const bool IsOut, TVec<TIntV>& CmtyVV);
  void GetCmtyVV(const bool IsOut, TVec<TIntV>& CmtyVV, const double Thres, const int MinSz = 3);
  void GetCmtyVVUnSorted(const bool IsOut, TVec<TIntV>& CmtyVV, const double Thres, const int MinSz = 3);
  void GetCmtyVVUnSorted(TVec<TIntV>& CmtyVVOut, TVec<TIntV>& CmtyVVIn);
  //void GetCmtyVVIn(TVec<TIntV>& CmtyVV);
  //void GetCmtyVVIn(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz = 3);
  int FindComsByCV(TIntV& ComsV, const double HOFrac = 0.2, const int NumThreads = 20, const TStr PlotLFNm = TStr(), const int EdgesForCV = 100, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int FindComsByCV(const int NumThreads, const int MaxComs, const int MinComs, const int DivComs, const TStr OutFNm, const int EdgesForCV = 100, const double StepAlpha = 0.3, const double StepBeta = 0.3);
  double LikelihoodHoldOut(const bool DoParallel = false);
  double GetStepSizeByLineSearch(const bool IsRow, const int UID, const TIntFltH& DeltaV, const TIntFltH& GradV, const double& Alpha, const double& Beta, const int MaxIter = 10);
  int MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const int ChunkSize, const TStr PlotNm, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const TStr PlotNm = TStr(), const double StepAlpha = 0.3, const double StepBeta = 0.1) {
    int ChunkSize = G->GetNodes() / 10 / ChunkNum;
    if (ChunkSize == 0) { ChunkSize = 1; }
    return MLEGradAscentParallel(Thres, MaxIter, ChunkNum, ChunkSize, PlotNm, StepAlpha, StepBeta);
  }
  //int MLENewton(const double& Thres, const int& MaxIter, const TStr PlotNm = TStr());
  //double GradientForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val);
  //double HessianForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val);
  //double LikelihoodForOneVar(const TFltV& AlphaKV, const int UID, const int CID, const double& Val);
  //double FindOptimalThres(const TVec<TIntV>& TrueCmtyVV, TVec<TIntV>& CmtyVV);
  void Save(TSOut& SOut);
  void Load(TSIn& SIn, const int& RndSeed = 0);
  TFlt& GetSumVal(const bool IsOut, const int CID) { 
    if (IsOut) {
      return SumFV[CID];
    } else {
      return SumHV[CID];
    }
  }
  double inline GetCom(const bool IsOut, const int& NID, const int& CID) {
    if (IsOut) {
      return GetComOut(NID, CID);
    } else {
      return GetComIn(NID, CID);
    }
  }
  double inline GetComOut(const int& NID, const int& CID) {
    if (F[NID].IsKey(CID)) {
      return F[NID].GetDat(CID);
    } else {
      return 0.0;
    }
  }
  double inline GetComIn(const int& NID, const int& CID) {
    if (H[NID].IsKey(CID)) {
      return H[NID].GetDat(CID);
    } else {
      return 0.0;
    }
  }
  void inline AddCom(const bool IsOut, const int& NID, const int& CID, const double& Val) {
    if (IsOut) {
      AddComOut(NID, CID, Val);
    } else {
      AddComIn(NID, CID, Val);
    }
  }
  void inline AddComOut(const int& NID, const int& CID, const double& Val) {
    if (F[NID].IsKey(CID)) {
      SumFV[CID] -= F[NID].GetDat(CID);
    }
    F[NID].AddDat(CID) = Val;
    SumFV[CID] += Val;
  }
  void inline AddComIn(const int& NID, const int& CID, const double& Val) {
    if (H[NID].IsKey(CID)) {
      SumHV[CID] -= H[NID].GetDat(CID);
    }
    H[NID].AddDat(CID) = Val;
    SumHV[CID] += Val;
  }
  void inline DelCom(const bool IsOut, const int& NID, const int& CID) {
    if (IsOut) {
      return DelComOut(NID, CID);
    } else {
      return DelComIn(NID, CID);
    }
  }
  void inline DelComOut(const int& NID, const int& CID) {
    if (F[NID].IsKey(CID)) {
      SumFV[CID] -= F[NID].GetDat(CID);
      F[NID].DelKey(CID);
    }
  }
  void inline DelComIn(const int& NID, const int& CID) {
    if (H[NID].IsKey(CID)) {
      SumHV[CID] -= H[NID].GetDat(CID);
      H[NID].DelKey(CID);
    }
  }
  double inline DotProduct(const TIntFltH& UV, const TIntFltH& VV) {
    double DP = 0;
    if (UV.Len() > VV.Len()) {
      for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
        if (VV.IsKey(HI.GetKey())) { 
          DP += VV.GetDat(HI.GetKey()) * HI.GetDat(); 
        }
      }
    } else {
      for (TIntFltH::TIter HI = VV.BegI(); HI < VV.EndI(); HI++) {
        if (UV.IsKey(HI.GetKey())) { 
          DP += UV.GetDat(HI.GetKey()) * HI.GetDat(); 
        }
      }
    }
    return DP;
  }
  double inline DotProductUtoV(const int& UID, const int& VID) {
    return DotProduct(F[UID], H[VID]);
  }
  double inline Prediction(const TIntFltH& FU, const TIntFltH& HV) {
    double DP = log (1.0 / (1.0 - PNoCom)) + DotProduct(FU, HV);
    IAssertR(DP > 0.0, TStr::Fmt("DP: %f", DP));
    return exp(- DP);
  }
  double inline Prediction(const int& UID, const int& VID) {
    return Prediction(F[UID], H[VID]);
  }
  double inline Sum(const TIntFltH& UV) {
    double N = 0.0;
    for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
      N += HI.GetDat();
    }
    return N;
  }
  double inline Norm2(const TIntFltH& UV) {
    double N = 0.0;
    for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
      N += HI.GetDat() * HI.GetDat();
    }
    return N;
  }
};

class TCodaAnalyzer {
public:
  PNGraph G;
  TVec<TIntFltH> InCmtyValHV;
  TVec<TIntFltH> OutCmtyValHV;
  TVec<TIntFltH> InOutCmtyValHV;
  TCodaAnalyzer() { G = TNGraph::New(); }
  TCodaAnalyzer(TCoda& Coda, const double MemThres = -1.0) {
    G = Coda.GetGraphRawNID();
    printf("graph copied (%d nodes %d edges)\n", G->GetNodes(), G->GetEdges());
    TIntV CIdV;
    Coda.GetTopCIDs(CIdV, Coda.GetNumComs());
    double Delta = MemThres == -1.0 ? sqrt(Coda.PNoCom): MemThres;
    for (int c = 0; c < CIdV.Len(); c++) {
      int CID = CIdV[c];
      TIntFltH InMemH, OutMemH, InOutMemH;
      Coda.GetNIDValH(InOutMemH, OutMemH, InMemH, CID, Delta);
      InCmtyValHV.Add(InMemH);
      OutCmtyValHV.Add(OutMemH);
      InOutCmtyValHV.Add(InOutMemH);
    }
    printf("Communities copied (%d communities)\n", InCmtyValHV.Len());
  }
  void GetAllCmtyVV(TVec<TIntV>& CmtyVV, const int MinSz) {
    for (int c = 0; c < InCmtyValHV.Len(); c++) {
      TIntV CmtyVIn, CmtyVOut, CmtyVInOut;
      if (InCmtyValHV[c].Len() < MinSz || OutCmtyValHV[c].Len() < MinSz) { continue; }
      InOutCmtyValHV[c].GetKeyV(CmtyVInOut);
      InCmtyValHV[c].GetKeyV(CmtyVIn);
      OutCmtyValHV[c].GetKeyV(CmtyVOut);
      CmtyVV.Add(CmtyVInOut);
      CmtyVV.Add(CmtyVOut);
      CmtyVV.Add(CmtyVIn);
    }
  }

  double GetFrac2Mode(const double Thres2Mode = 0.2, const int MinSzEach = 2) {
    int Cnt2Mode = 0;
    int CntAll = 0;
    for (int c = 0; c < InCmtyValHV.Len(); c++) {
      double Jacc = (double) InOutCmtyValHV[c].Len() / (double) (InCmtyValHV[c].Len() + OutCmtyValHV[c].Len() - InOutCmtyValHV[c].Len());
      if (InCmtyValHV[c].Len() < MinSzEach || OutCmtyValHV[c].Len() < MinSzEach) { continue; }
      if (Jacc <= Thres2Mode) { Cnt2Mode++; }
      CntAll++;
    }
    return (double) Cnt2Mode / (double) CntAll;
  }

  void Summary(const int TopK = 10, const double Thres2Mode = 0.2) {
    int Cnt2Mode = 0;
    double SumJacc = 0.0;
    for (int c = 0; c < InCmtyValHV.Len(); c++) {
      double Jacc = (double) InOutCmtyValHV[c].Len() / (double) (InCmtyValHV[c].Len() + OutCmtyValHV[c].Len() - InOutCmtyValHV[c].Len());
      if (Jacc <= Thres2Mode) { Cnt2Mode++; }
      SumJacc += Jacc;
      if (c < TopK) {
        printf("Cmty %d: InOut: %d, In:%d, Out:%d, Jacc;%.3f\n", c, InCmtyValHV[c].Len(), InCmtyValHV[c].Len(), OutCmtyValHV[c].Len(), Jacc);
      }
    }
    double AvgJacc = SumJacc / (double) InCmtyValHV.Len();
    printf("Average jaccard similarity = %.3f. (%d / %d communities are 2-mode)\n", AvgJacc, Cnt2Mode, InCmtyValHV.Len());
  }
  int GetNumComs() { return InCmtyValHV.Len(); }
/// save bipartite community affiliation into gexf file

  void GetCmtyVAll(TIntV& CmtyVAll, const int CID) {
    TIntV CmtyVIn, CmtyVOut;
    InCmtyValHV[CID].GetKeyV(CmtyVIn);
    OutCmtyValHV[CID].GetKeyV(CmtyVOut);
    CmtyVIn.Sort();
    CmtyVOut.Sort();
    CmtyVAll.Gen(CmtyVIn.Len() + CmtyVOut.Len(), 0);
    CmtyVIn.Union(CmtyVOut, CmtyVAll);
  }

  PNGraph Net2ModeCommunities(const double MaxJac, const double JacEdge, const bool GetWcc = true) {
    //if In(A) is similar to Out(B), create an edge A->B between 2 communities A, B
    int Coms = InCmtyValHV.Len();
    PNGraph ComG = TNGraph::New(Coms, -1);
    for (int c = 0; c < InCmtyValHV.Len(); c++) {
      double Jacc = (double) InOutCmtyValHV[c].Len() / (double) (InCmtyValHV[c].Len() + OutCmtyValHV[c].Len() - InOutCmtyValHV[c].Len());
      if (Jacc > MaxJac) { continue; }
      ComG->AddNode(c);
    }
    TVec<TIntSet> CmtySVIn, CmtySVOut;
    for (int c = 0; c < Coms; c++) {
      TIntV CmtyVIn, CmtyVOut;
      InCmtyValHV[c].GetKeyV(CmtyVIn);
      OutCmtyValHV[c].GetKeyV(CmtyVOut);
      TIntSet CmtySIn(CmtyVIn), CmtySOut(CmtyVOut);
      CmtySVIn.Add(CmtySIn);
      CmtySVOut.Add(CmtySOut);
    }
    for (int c1 = 0; c1 < Coms; c1++) {
      if (! ComG->IsNode(c1)) { continue; }
      for (int c2 = 0; c2 < Coms; c2++) {
        if (! ComG->IsNode(c2)) { continue; }
        int IntC1C2 = TAGMUtil::Intersection(CmtySVIn[c1], CmtySVOut[c2]);
        double Jac = (double) IntC1C2 / (CmtySVIn[c1].Len() + CmtySVOut[c2].Len() - IntC1C2);
        if (Jac >= JacEdge) {
          ComG->AddEdge(c1, c2);
        }
      }
    }
    //PNGraph Wcc = TSnap::GetMxWcc(ComG);
    TIntV NIDV;
    ComG->GetNIdV(NIDV);
    for (int u = 0; u < NIDV.Len(); u++) {
      int NID = NIDV[u];
      TNGraph::TNodeI NI = ComG->GetNI(NID);
      if (NI.GetDeg() == 0) { ComG->DelNode(NID); }
      if (NI.GetInDeg() == 1 && NI.GetOutDeg() == 1 && NI.GetOutNId(0) == NID) { ComG->DelNode(NID); }
    }
    printf("Community graph made (Jaccard similarity for edges: %f, %d nodes, %d edges)\n", JacEdge, ComG->GetNodes(), ComG->GetEdges());
    return ComG;
  }

  // RS:2014/03/11 default parameter values do not compile on OS X with g++-4.2
  //void Dump2ModeCommunities(const TStr& OutFNm, const double MaxJac, const TIntStrH& NIDNameH =  THash<TInt, TStr>()) {
  void Dump2ModeCommunities(const TStr& OutFNm, const double MaxJac, const TIntStrH& NIDNameH) {
    FILE* F = fopen(OutFNm.CStr(), "wt");
    for (int c = 0; c < InCmtyValHV.Len(); c++) {
      double Jacc = (double) InOutCmtyValHV[c].Len() / (double) (InCmtyValHV[c].Len() + OutCmtyValHV[c].Len() - InOutCmtyValHV[c].Len());
      if (Jacc > MaxJac) { continue; }
      TIntV CmtyVIn, CmtyVOut, CmtyVAll;
      InCmtyValHV[c].GetKeyV(CmtyVIn);
      OutCmtyValHV[c].GetKeyV(CmtyVOut);
      GetCmtyVAll(CmtyVAll, c);
      //adjust for the nodes who belong to both cmtyvin and cmtyvout
      for (int u = 0; u < InOutCmtyValHV[c].Len(); u++) {
        int UID = InOutCmtyValHV[c].GetKey(u);
        if (CmtyVIn.Len() >= CmtyVOut.Len()) {
          CmtyVIn.DelIfIn(UID); 
        } else {
          CmtyVOut.DelIfIn(UID); 
        }
      }
      if (CmtyVAll.Len() == 0) { continue; }
      fprintf(F, "Com %d\n", c);
      for (int u = 0; u < CmtyVOut.Len(); u++) {
        int NID = CmtyVOut[u];
        TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): TStr::Fmt("Concept %d", NID);
        fprintf(F, "%s:%f\n", Label.CStr(), OutCmtyValHV[c].GetDat(NID).Val);
      }
      fprintf(F, "||==>||\n");
      for (int u = 0; u < CmtyVIn.Len(); u++) {
        int NID = CmtyVIn[u];
        TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): TStr::Fmt("Concept %d", NID);
        fprintf(F, "%s:%f\n", Label.CStr(), InCmtyValHV[c].GetDat(NID).Val);
      }
      fprintf(F, "\n");
    }
    fclose(F);
  }

  // RS:2014/03/11 default parameter values do not compile on OS X with g++-4.2
  //void Draw2ModeCommunity(const int CID, const TStr& OutFNm, const TIntStrH& NIDNameH =  THash<TInt, TStr>(), const THash<TInt, TIntTr>& NIDColorH = THash<TInt, TIntTr>() ) {
  void Draw2ModeCommunity(const int CID, const TStr& OutFNm, const TIntStrH& NIDNameH, const THash<TInt, TIntTr>& NIDColorH) {
    TIntV CmtyVIn, CmtyVOut, CmtyVAll;
    InCmtyValHV[CID].GetKeyV(CmtyVIn);
    OutCmtyValHV[CID].GetKeyV(CmtyVOut);
    GetCmtyVAll(CmtyVAll, CID);

    //adjust for the nodes who belong to both cmtyvin and cmtyvout
    for (int u = 0; u < InOutCmtyValHV[CID].Len(); u++) {
      int UID = InOutCmtyValHV[CID].GetKey(u);
      if (CmtyVIn.Len() >= CmtyVOut.Len()) {
        CmtyVIn.DelIfIn(UID); 
      } else {
        CmtyVOut.DelIfIn(UID); 
      }
    }

    PNGraph SG = TSnap::GetSubGraph(G, CmtyVAll);
    /// Plot bipartite graph for the 2-mode community
    if (CmtyVAll.Len() == 0) { return; }
    double OXMin = 0.1, YMin = 0.1, OXMax = 2500.00, YMax = 1000.0, IXMin = 0.1, IXMax = 2500.00;
    double OStep = (OXMax - OXMin) / (double) CmtyVOut.Len(), IStep = (IXMax - IXMin) / (double) CmtyVIn.Len();

    FILE* F = fopen(OutFNm.CStr(), "wt");
    fprintf(F, "<?xml version='1.0' encoding='UTF-8'?>\n");
    fprintf(F, "<gexf xmlns='http://www.gexf.net/1.2draft' xmlns:viz='http://www.gexf.net/1.1draft/viz' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd' version='1.2'>\n");
    fprintf(F, "\t<graph mode='static' defaultedgetype='directed'>\n");
    fprintf(F, "\t\t<nodes>\n");
    for (int c = 0; c < CmtyVOut.Len(); c++) {
      int NID = CmtyVOut[c];
      double XPos = c * OStep + OXMin;
      TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): "";
      Label.ChangeChAll('<', ' ');
      Label.ChangeChAll('>', ' ');
      Label.ChangeChAll('&', ' ');
      Label.ChangeChAll('\'', ' ');
      TIntTr Color = NIDColorH.IsKey(NID)? NIDColorH.GetDat(NID) : TIntTr(120, 120, 120);
      fprintf(F, "\t\t\t<node id='%d' label='%s'>\n", NID, Label.CStr());
      fprintf(F, "\t\t\t\t<viz:color r='%d' g='%d' b='%d'/>\n", Color.Val1.Val, Color.Val2.Val, Color.Val3.Val);
      fprintf(F, "\t\t\t\t<viz:size value='4.0'/>\n");
      fprintf(F, "\t\t\t\t<viz:shape value='square'/>\n");
      fprintf(F, "\t\t\t\t<viz:position x='%f' y='%f' z='0.0'/>\n", XPos, YMax); 
      fprintf(F, "\t\t\t</node>\n");
    }

    for (int u = 0; u < CmtyVIn.Len(); u++) {
      int NID = CmtyVIn[u];
      TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): "";
      Label.ChangeChAll('<', ' ');
      Label.ChangeChAll('>', ' ');
      Label.ChangeChAll('&', ' ');
      Label.ChangeChAll('\'', ' ');
      double XPos = IXMin + u * IStep;
      TIntTr Color = NIDColorH.IsKey(NID)? NIDColorH.GetDat(NID) : TIntTr(120, 120, 120);
      double Alpha = 1.0;
      fprintf(F, "\t\t\t<node id='%d' label='%s'>\n", NID, Label.CStr());
      fprintf(F, "\t\t\t\t<viz:color r='%d' g='%d' b='%d' a='%.1f'/>\n", Color.Val1.Val, Color.Val2.Val, Color.Val3.Val, Alpha);
      fprintf(F, "\t\t\t\t<viz:size value='4.0'/>\n");
      fprintf(F, "\t\t\t\t<viz:shape value='square'/>\n");
      fprintf(F, "\t\t\t\t<viz:position x='%f' y='%f' z='0.0'/>\n", XPos, YMin); 
      fprintf(F, "\t\t\t</node>\n");
    }
    fprintf(F, "\t\t</nodes>\n");
    //plot edges
    int EID = 0;
    fprintf(F, "\t\t<edges>\n");
    for (TNGraph::TNodeI NI = SG->BegNI(); NI < SG->EndNI(); NI++) {
      if (NI.GetOutDeg() == 0 && NI.GetInDeg() == 0  ) { continue; }
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        fprintf(F, "\t\t\t<edge id='%d' source='%d' target='%d'/>\n", EID++, NI.GetId(), NI.GetOutNId(e));
      }
    }
    fprintf(F, "\t\t</edges>\n");
    fprintf(F, "\t</graph>\n");
    fprintf(F, "</gexf>\n");
    fclose(F);
  }

};

#endif
