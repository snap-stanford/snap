#ifndef yanglib_agmattr1_h
#define yanglib_agmattr1_h
#include "Snap.h"

class TCesnaUtil {
public:
  //static double GetConductance(const PUNGraph& Graph, const TIntSet& CmtyS, const int Edges);
  //static double GetConductance(const PNGraph& Graph, const TIntSet& CmtyS, const int Edges);
template<class PGraph>
static double GetConductance(const PGraph& Graph, const TIntSet& CmtyS, const int Edges) {
  const bool GraphType = HasGraphFlag(typename PGraph::TObj, gfDirected);
  int Edges2;
  if (GraphType) { Edges2 = Edges >= 0 ? Edges : Graph->GetEdges(); }
  else { Edges2 = Edges >= 0 ? 2 * Edges : Graph->GetEdges(); }
  int Vol = 0,  Cut = 0; 
  double Phi = 0.0;
  for (int i = 0; i < CmtyS.Len(); i++) {
    if (! Graph->IsNode(CmtyS[i])) { continue; }
    typename PGraph::TObj::TNodeI  NI = Graph->GetNI(CmtyS[i]);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (! CmtyS.IsKey(NI.GetOutNId(e))) { Cut += 1; }
    }
    Vol += NI.GetOutDeg();
  }
  // get conductance
  if (Vol != Edges2) {
    if (2 * Vol > Edges2) { Phi = Cut / double (Edges2 - Vol); }
    else if (Vol == 0) { Phi = 0.0; }
    else { Phi = Cut / double(Vol); }
  } else {
    if (Vol == Edges2) { Phi = 1.0; }
  }
  return Phi;
}


template<class PGraph>
  static void GenHoldOutPairs(const PGraph& G, TVec<TIntSet>& HoldOutSet, double HOFrac, TRnd& Rnd)  {
    TIntPrV EdgeV(G->GetEdges(), 0);
    for (typename PGraph::TObj::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
      EdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
    }
    EdgeV.Shuffle(Rnd);

    const bool GraphType = HasGraphFlag(typename PGraph::TObj, gfDirected);
    HoldOutSet.Gen(G->GetNodes());
    int HOTotal = int(HOFrac * G->GetNodes() * (G->GetNodes() - 1) / 2.0);
    if (GraphType) { HOTotal *= 2;}
    int HOCnt = 0;
    int HOEdges = (int) TMath::Round(HOFrac * G->GetEdges());
    printf("holding out %d edges...\n", HOEdges);
    for (int he = 0; he < (int) HOEdges; he++) {
      HoldOutSet[EdgeV[he].Val1].AddKey(EdgeV[he].Val2);
      if (! GraphType) { HoldOutSet[EdgeV[he].Val2].AddKey(EdgeV[he].Val1); }
      HOCnt++;
    }
    printf("%d Edges hold out\n", HOCnt);
    while(HOCnt++ < HOTotal) {
      int SrcNID = Rnd.GetUniDevInt(G->GetNodes());
      int DstNID = Rnd.GetUniDevInt(G->GetNodes());
      if (SrcNID == DstNID) { continue; }
      HoldOutSet[SrcNID].AddKey(DstNID);
      if (! GraphType) { HoldOutSet[DstNID].AddKey(SrcNID); }
    }
  }

template<class PGraph>
  static void GetNbhCom(const PGraph& Graph, const int NID, TIntSet& NBCmtyS) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NID);
    NBCmtyS.Gen(NI.GetDeg());
    NBCmtyS.AddKey(NID);
    for (int e = 0; e < NI.GetDeg(); e++) {
      NBCmtyS.AddKey(NI.GetNbrNId(e));
    }
  }
template<class PGraph>
  static void GetNIdPhiV(const PGraph& G, TFltIntPrV& NIdPhiV) {
    NIdPhiV.Gen(G->GetNodes(), 0);
    const int Edges = G->GetEdges();
    TExeTm RunTm;
    //compute conductance of neighborhood community
    for (typename PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      TIntSet NBCmty(NI.GetDeg() + 1);
      double Phi;
      if (NI.GetDeg() < 5) { //do not include nodes with too few degree
        Phi = 1.0; 
      } else {
        TCesnaUtil::GetNbhCom<PGraph>(G, NI.GetId(), NBCmty);
        //if (NBCmty.Len() != NI.GetDeg() + 1) { printf("NbCom:%d, Deg:%d\n", NBCmty.Len(), NI.GetDeg()); }
        //IAssert(NBCmty.Len() == NI.GetDeg() + 1);
        Phi = TCesnaUtil::GetConductance(G, NBCmty, Edges);
      }
      //NCPhiH.AddDat(u, Phi);
      NIdPhiV.Add(TFltIntPr(Phi, NI.GetId()));
    }
    printf("conductance computation completed [%s]\n", RunTm.GetTmStr());
    fflush(stdout);
  }

  static void LoadNIDAttrHFromNIDKH(const TIntV& NIDV, const TStr& InFNm, THash<TInt, TIntV>& NIDAttrH, const TStrHash<TInt>& NodeNameH, const TSsFmt Sep = ssfTabSep) {
    NIDAttrH.Clr();
    NIDAttrH.Gen(NIDV.Len());
    printf("nodes in the graph:%d\n", NIDV.Len());
    for (int u = 0; u < NIDV.Len(); u++) { NIDAttrH.AddDat(NIDV[u]).Gen(0, 0); }
    TSsParser Ss(InFNm, ssfTabSep);
    while (Ss.Next()) {
      TStr NodeName = Ss.GetFld(0);
      TInt NID = NodeName.GetInt();
      if (NodeNameH.Len() > 0 && ! NodeNameH.IsKey(NodeName)) { continue; }
      if (NodeNameH.Len() > 0) { 
        IAssertR(NodeNameH.IsKey(NodeName), TStr::Fmt("NodeName:%s", NodeName.CStr())); 
        NID = NodeNameH.GetKeyId(NodeName);
      }
      if (! NIDAttrH.IsKey(NID)) { 
        //printf("NodeName %s, NID %d does not exist\n", NodeName.CStr(), NID);
        continue; } //ignore nodes who are not in the graph
      IAssertR(! NIDAttrH.GetDat(NID).IsIn(Ss.GetInt(1)), TStr::Fmt("NIdx:%d NID:%s, K:%d", NID.Val, NodeName.CStr(), Ss.GetInt(1)));
      NIDAttrH.GetDat(NID).Add(Ss.GetInt(1));
    }
    printf("%s nodes, %s lines read \n",  TUInt64::GetStr(NIDAttrH.Len()).CStr(), TUInt64::GetStr(Ss.GetLineNo()).CStr());
    //printf("%d nodes, %d lines read \n",  NIDAttrH.Len(), Ss.GetLineNo());
  }
  static void LoadNIDAttrHFromNIDKH(const TIntV& NIDV, const TStr& InFNm, THash<TInt, TIntV>& NIDAttrH) {
    TStrHash<TInt> TmpH;
    LoadNIDAttrHFromNIDKH(NIDV, InFNm, NIDAttrH, TmpH);
  }
  static void DumpNIDAttrHToNIDK(const TStr& FNm, const THash<TInt, TIntSet>& NIDAttrH, const TStrHash<TInt>& FeatNameH, const TStrHash<TInt>& NodeNameH) {
    FILE* F = fopen(FNm.CStr(), "wt");
    for (int u = 0; u < NIDAttrH.Len(); u++) {
      int NID = NIDAttrH.GetKey(u);
      TStr NodeName = NodeNameH.IsKeyId(NID)? NodeNameH.GetKey(NID): TStr::Fmt("%d", NID);
      for (int k = 0; k < NIDAttrH[u].Len(); k++) {
        int KID = NIDAttrH[u][k];
        TStr FeatName = FeatNameH.IsKeyId(KID)? FeatNameH.GetKey(KID): TStr::Fmt("%d", KID);
        fprintf(F,"%s\t%s\n", NodeName.CStr(), FeatName.CStr());
      }
    }
    fclose(F);
  }
  static void DumpNIDAttrHToNIDK(const TStr& FNm, const THash<TInt, TIntSet>& NIDAttrH, const TStrHash<TInt>& FeatNameH) {
    TStrHash<TInt> TmpH;
    DumpNIDAttrHToNIDK(FNm, NIDAttrH, FeatNameH, TmpH);
  }
  static void DumpNIDAttrHToNIDK(const TStr& FNm, const THash<TInt, TIntSet>& NIDAttrH) {
    TStrHash<TInt> TmpH1, TmpH2;
    DumpNIDAttrHToNIDK(FNm, NIDAttrH, TmpH1, TmpH2);
  }
  static void DumpNIDAttrHToNIDK(const TStr& FNm, const THash<TInt, TIntV>& NIDAttrH, const TStrHash<TInt>& FeatNameH, const TStrHash<TInt>& NodeNameH) {
    FILE* F = fopen(FNm.CStr(), "wt");
    for (int u = 0; u < NIDAttrH.Len(); u++) {
      int NID = NIDAttrH.GetKey(u);
      TStr NodeName = NodeNameH.IsKeyId(NID)? NodeNameH.GetKey(NID): TStr::Fmt("%d", NID);
      for (int k = 0; k < NIDAttrH[u].Len(); k++) {
        int KID = NIDAttrH[u][k];
        TStr FeatName = FeatNameH.IsKeyId(KID)? FeatNameH.GetKey(KID): TStr::Fmt("%d", KID);
        fprintf(F,"%s\t%s\n", NodeName.CStr(), FeatName.CStr());
      }
    }
    fclose(F);
  }
  static void DumpNIDAttrHToNIDK(const TStr& FNm, const THash<TInt, TIntV>& NIDAttrH, const TStrHash<TInt>& FeatNameH) {
    TStrHash<TInt> TmpH;
    DumpNIDAttrHToNIDK(FNm, NIDAttrH, FeatNameH, TmpH);
  }
  static void DumpNIDAttrHToNIDK(const TStr& FNm, const THash<TInt, TIntV>& NIDAttrH) {
    TStrHash<TInt> TmpH1, TmpH2;
    DumpNIDAttrHToNIDK(FNm, NIDAttrH, TmpH1, TmpH2);
  }
  static int GetAttrs(const THash<TInt, TIntV>& NIDAttrH) {
    int Attrs = 0;
    for (int u = 0; u < NIDAttrH.Len(); u++) {
      for (int k = 0; k < NIDAttrH[u].Len(); k++) {
        if (NIDAttrH[u][k] >= Attrs) { Attrs = NIDAttrH[u][k] + 1; }
      }
    }
    return Attrs;
  }
  //Metis format (N + 1) line describes the attributes of N. ID start from 1
  static void DumpNIDAttrHToMetis(const TStr& FNm, const THash<TInt, TIntV>& NIDAttrH, const TIntV& NIDV) {
    int AttrCnt = 0;
    for (int u = 1; u < NIDV.Len(); u++) {
      if (! NIDAttrH.IsKey(NIDV[u])) { continue; }
      AttrCnt += NIDAttrH.GetDat(NIDV[u]).Len();
    }
    IAssert (NIDV[0] == -1);
    FILE* F = fopen(FNm.CStr(), "wt");
    fprintf(F, "%d %d\n", NIDV.Len() - 1, AttrCnt);
    int TmpCnt = 0;
    for (int u = 1; u < NIDV.Len(); u++) {
      if (NIDAttrH.IsKey(NIDV[u])) {  
        for (int k = 0; k < NIDAttrH.GetDat(NIDV[u]).Len(); k++) {
          if (k > 0) { fprintf(F, " "); }
          fprintf(F, "%d", NIDAttrH.GetDat(NIDV[u])[k].Val + 1);
          TmpCnt++;
        }
      }
      fprintf(F, "\n");
    }
    fclose(F);
    IAssert(AttrCnt == TmpCnt);

  }
  static void FilterLowEntropy(const THash<TInt, TIntV>& OldNIDAttrH, THash<TInt, TIntV>& NewNIDAttrH, const TIntStrH& OldNameH, TIntStrH& NewNameH, const double MinFrac = 0.00001, const double MaxFrac = 0.95, const int MinCnt = 3) {
    TIntH KIDCntH;
    for (int u = 0; u < OldNIDAttrH.Len(); u++) {
      for (int k = 0; k < OldNIDAttrH[u].Len(); k++) {
        KIDCntH.AddDat(OldNIDAttrH[u][k])++;
      }
    }
    KIDCntH.SortByDat(false);

    TIntSet SelectedK(KIDCntH.Len());
    for (int c = 0; c < KIDCntH.Len(); c++) {
      double Frac = (double) KIDCntH[c].Val / (double) OldNIDAttrH.Len();
      if (KIDCntH[c].Val < MinCnt) { continue; }
      if (Frac > MaxFrac || Frac < MinFrac) { continue; }
      SelectedK.AddKey(KIDCntH.GetKey(c));
    }
    printf("%d attributes selected from %d\n", SelectedK.Len(), KIDCntH.Len());
    NewNIDAttrH.Gen(OldNIDAttrH.Len());
    for (int u = 0; u < OldNIDAttrH.Len(); u++) {
      int NID = OldNIDAttrH.GetKey(u);
      TIntV& AttrV = NewNIDAttrH.AddDat(NID);
      for (int k = 0; k < OldNIDAttrH[u].Len(); k++) {
        if (! SelectedK.IsKey(OldNIDAttrH[u][k])) { continue; }
        AttrV.Add(SelectedK.GetKeyId(OldNIDAttrH[u][k]));
      }
    }

    if (! OldNameH.Empty()) {
      NewNameH.Gen(SelectedK.Len());
      for (int k = 0; k < SelectedK.Len(); k++) {
        int OldKID = SelectedK.GetKey(k);
        if (OldNameH.IsKey(OldKID)) {
          NewNameH.AddDat(k, OldNameH.GetDat(OldKID));
        }
      }
      printf("%d attributes names copied\n", NewNameH.Len());
    }
  }
  static void FilterLowEntropy(const THash<TInt, TIntV>& OldNIDAttrH, THash<TInt, TIntV>& NewNIDAttrH, const double MinFrac = 0.00001, const double MaxFrac = 0.95, const int MinCnt = 3) {
    TIntStrH TmpH1, TmpH2;
    FilterLowEntropy(OldNIDAttrH, NewNIDAttrH, TmpH1, TmpH2, MinFrac, MaxFrac, MinCnt);
  }
};
class TCesna { //CESNA: community detection in networks with node attributes
private:
  PUNGraph G; //graph to fit
  TVec<TIntSet> X; // X[u] = {k| X_uk = 1}
  TVec<TIntFltH> F; // membership for each user (Size: Nodes * Coms)
  TVec<TFltV> W; // weight vector for logistic regression. w_ck = W[k][c] (Column vector)
  TInt Attrs; // number of attributes
  TRnd Rnd; // random number generator
  TIntSet NIDToIdx; // original node ID vector NIDToIdx[i] = Node ID for index i, NIDToIdx.GetKey(NID) = index for NID
  TFlt RegCoef; //Regularization coefficient when we fit for P_c +: L1, -: L2
  TFltV SumFV; // sum_u F_uc for each community c. Needed for efficient calculation
  TInt NumComs; // number of communities
  TVec<TIntSet> HOVIDSV; //NID pairs to hold out for cross validation
  TVec<TIntSet> HOKIDSV; //set of attribute index (k) to hold out
public:
  TFlt MinVal; // minimum value of F (0)
  TFlt MaxVal; // maximum value of F (for numerical reason)
  TFlt MinValW; // minimum value of W (for numerical reason)
  TFlt MaxValW; // maximum value of W (for numerical reason)
  TFlt NegWgt; // weight of negative example (a pair of nodes without an edge)
  TFlt LassoCoef; // L1 regularization coefficient for W (MLE = argmax P(X|F, W) - LassoCoef * |W|)
  TFlt WeightAttr; // likelihood = log P(G|F) + WeightAttr * log P(X|F, W)
  TFlt PNoCom; // base probability \varepsilon (edge probability between a pair of nodes sharing no community
  TBool DoParallel; // whether to use parallelism for computation

  TCesna() { G = TUNGraph::New(10, -1); }
  TCesna(const PUNGraph& GraphPt, const THash<TInt, TIntV>& NIDAttrH, const int& InitComs, const int RndSeed = 0): Rnd(RndSeed), RegCoef(0), 
    MinVal(0.0), MaxVal(10.0), MinValW(-10.0), MaxValW(10.0), NegWgt(1.0), LassoCoef(1.0), WeightAttr(1.0) { SetGraph(GraphPt, NIDAttrH); NeighborComInit(InitComs); }
  void Save(TSOut& SOut) {
    G->Save(SOut);
    X.Save(SOut);
    F.Save(SOut);
    W.Save(SOut);
    Attrs.Save(SOut);
    NIDToIdx.Save(SOut);
    RegCoef.Save(SOut);
    LassoCoef.Save(SOut);
    SumFV.Save(SOut);
    NumComs.Save(SOut);
    HOVIDSV.Save(SOut);
    HOKIDSV.Save(SOut);
    MinVal.Save(SOut);
    MaxVal.Save(SOut);
    MinValW.Save(SOut);
    MaxValW.Save(SOut);
    NegWgt.Save(SOut);
    PNoCom.Save(SOut);
  }
  void Load(TSIn& SIn, const int& RndSeed = 0) {
    G->Load(SIn);
    X.Load(SIn);
    F.Load(SIn);
    W.Load(SIn);
    Attrs.Load(SIn);
    NIDToIdx.Load(SIn);
    RegCoef.Load(SIn);
    LassoCoef.Load(SIn);
    SumFV.Load(SIn);
    NumComs.Load(SIn);
    HOVIDSV.Load(SIn);
    HOKIDSV.Load(SIn);
    MinVal.Load(SIn);
    MaxVal.Load(SIn);
    MinValW.Load(SIn);
    MaxValW.Load(SIn);
    NegWgt.Load(SIn);
    PNoCom.Load(SIn);
  }

  void SetGraph(const PUNGraph& GraphPt, const THash<TInt, TIntV>& NIDAttrH);
  void SetRegCoef(const double _RegCoef) { RegCoef = _RegCoef; }
  double GetRegCoef() { return RegCoef; }
  void SetWeightAttr(const double _WeightAttr) { IAssert (_WeightAttr <= 1.0 && _WeightAttr >= 0.0); WeightAttr = _WeightAttr; }
  double GetWeightAttr() { return WeightAttr; }
  void SetLassoCoef(const double _LassoCoef) { LassoCoef = _LassoCoef; }
  int GetAttrs() { return Attrs; }
  double GetComFromNID(const int& NID, const int& CID) {
    int NIdx = NIDToIdx.GetKeyId(NID);
    if (F[NIdx].IsKey(CID)) {
      return F[NIdx].GetDat(CID);
    } else {
      return 0.0;
    }
  }
  double GetLassoCoef() { return LassoCoef; }
  void InitW() { // initialize W
    W.Gen(Attrs);
    for (int k = 0; k < Attrs; k++) {
      W[k].Gen(NumComs + 1);
    }
  }
  void SetAttrHoldOut(const int NID, const int KID) {
    int NIdx = NIDToIdx.GetKeyId(NID);
    HOKIDSV[NIdx].AddKey(KID);
  }
  void SetAttrHoldOutForOneNode(const int NID) {
    for (int k = 0; k < Attrs; k++) {
      SetAttrHoldOut(NID, k);
    }
  }
  void GetW(TVec<TFltV>& _W) { _W = W; }
  void SetW(TVec<TFltV>& _W) { W = _W; }
  void RandomInit(const int InitComs);
  void NeighborComInit(const int InitComs);
  void NeighborComInit(TFltIntPrV& NIdPhiV, const int InitComs);
  int GetNumComs() { return NumComs; }
  void SetCmtyVV(const TVec<TIntV>& CmtyVV);
  double Likelihood(const bool DoParallel = false);
  double LikelihoodForRow(const int UID);
  double LikelihoodForRow(const int UID, const TIntFltH& FU);
  double LikelihoodAttrKForRow(const int UID, const int K) { return LikelihoodAttrKForRow(UID, K, F[UID]); }
  double LikelihoodAttrKForRow(const int UID, const int K, const TIntFltH& FU) { return LikelihoodAttrKForRow(UID, K, FU, W[K]); }
  double LikelihoodAttrKForRow(const int UID, const int K, const TIntFltH& FU, const TFltV& WK);
  double LikelihoodForWK(const int K, const TFltV& WK) {
    double L = 0.0;
    for (int u = 0; u < F.Len(); u++) {
      if (HOKIDSV[u].IsKey(K)) { continue; }
      L += LikelihoodAttrKForRow(u, K, F[u], WK);
    }
    for (int c = 0; c < WK.Len() - 1; c++) {
      L -= LassoCoef * fabs(WK[c]);
    } 
    return L;
  }
  double LikelihoodForWK(const int K) { return LikelihoodForWK(K, W[K]); }
  double LikelihoodAttr() {
    double L = 0.0;
    for (int k = 0; k < Attrs; k++) {
      for (int u = 0; u < F.Len(); u++) {
        if (HOKIDSV[u].IsKey(k)) { continue; }
        L += LikelihoodAttrKForRow(u, k, F[u], W[k]);
      }
    }
    return L;
  }
  double LikelihoodGraph() {
    double L = Likelihood();
    //add regularization
    if (RegCoef > 0.0) { //L1
      for (int u = 0; u < F.Len(); u++) {
        L += RegCoef * Sum(F[u]);
      }
    }
    if (RegCoef < 0.0) { //L2
      for (int u = 0; u < F.Len(); u++) {
        L -= RegCoef * Norm2(F[u]);
      }
    }

    return L - WeightAttr * LikelihoodAttr();
  }
  void GenHoldOutAttr(const double HOFrac, TVec<TIntSet>& HOSetV) {
    HOSetV.Gen(F.Len());
    int HoldOutCnt = (int) ceil(HOFrac * G->GetNodes() * Attrs);
    TIntPrSet NIDKIDSet(HoldOutCnt);
    int Cnt = 0;
    for (int h = 0; h < 10 * HoldOutCnt; h++) {
      int UID = Rnd.GetUniDevInt(F.Len());
      int KID = Rnd.GetUniDevInt(Attrs);
      if (! NIDKIDSet.IsKey(TIntPr(UID, KID))) { 
        NIDKIDSet.AddKey(TIntPr(UID, KID)); 
        HOSetV[UID].AddKey(KID);
        Cnt++;
      }
      if (Cnt >= HoldOutCnt) { break; }
    }
    printf("%d hold out pairs generated for attributes\n", Cnt);
  }
  void SetHoldOut(const double HOFrac) { 
    TVec<TIntSet> HoldOut; 
    TCesnaUtil::GenHoldOutPairs(G, HoldOut, HOFrac, Rnd); 
    GenHoldOutAttr(HOFrac, HOKIDSV);
    HOVIDSV = HoldOut; 
  }
  void GradientForRow(const int UID, TIntFltH& GradU, const TIntSet& CIDSet);
  void GradientForWK(TFltV& GradV, const int K) {
    GradV.Gen(NumComs + 1);
    for (int u = 0; u < F.Len(); u++) {
      if (HOKIDSV[u].IsKey(K)) { continue; }
      double Pred = PredictAttrK(u, K);
      for (TIntFltH::TIter CI = F[u].BegI(); CI < F[u].EndI(); CI++) { 
        GradV[CI.GetKey()] += (GetAttr(u, K) - Pred) * GetCom(u, CI.GetKey());
      }
      GradV[NumComs] += (GetAttr(u, K) - Pred);
    }
    
    for (int c = 0; c < GradV.Len() - 1; c++) {
      GradV[c] -= LassoCoef * TMath::Sign(GetW(c, K));
    }
  }
  void GetCmtyVV(TVec<TIntV>& CmtyVV);
  void GetCmtyVV(TVec<TIntV>& CmtyVV, TVec<TFltV>& Wck, const double Thres, const int MinSz = 3);
  void GetCmtyVV(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz = 3) {
    TVec<TFltV> TmpV;
    GetCmtyVV(CmtyVV, TmpV, Thres, MinSz); 
  }
  void GetCmtyVV(TVec<TIntV>& CmtyVV, TVec<TFltV>& Wck) {
    GetCmtyVV(CmtyVV, Wck, sqrt(2.0 * (double) G->GetEdges() / G->GetNodes() / G->GetNodes()), 3);
  }

  void GetCmtyVVUnSorted(TVec<TIntV>& CmtyVV);
  void GetCmtyVVUnSorted(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz = 3);
  /* GetCmtyVVRelative: NOT working well (low accuracy)
  void GetCmtyVVRelative(TVec<TIntV>& CmtyVV, const int MinSz = 3) {
    CmtyVV.Clr();
    for (int c = 0; c < NumComs; c++) {
      TIntV CmtyV;
      double MaxVal = 0.0;
      for (int u = 0; u < G->GetNodes(); u++) {
        if (GetCom(u, c) > MaxVal) { MaxVal = GetCom(u, c); }
      }
      if (MaxVal == 0.0) { continue; }
      for (int u = 0; u < G->GetNodes(); u++) {
        if (GetCom(u, c) > 0.5 * MaxVal) { CmtyV.Add(NIDToIdx[u]); }
      }
      if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
    }
    if ( NumComs != CmtyVV.Len()) {
      printf("Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
    }
  }
  */
  int FindComs(TIntV& ComsV, const bool UseBIC = false, const double HOFrac = 0.2, const int NumThreads = 20, const TStr PlotLFNm = TStr(), const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int FindComs(const int NumThreads, const int MaxComs, const int MinComs, const int DivComs, const TStr OutFNm, const bool UseBIC = false, const double HOFrac = 0.1, const double StepAlpha = 0.3, const double StepBeta = 0.3);
  void DisplayAttrs(const int TopK, const TStrHash<TInt>& NodeNameH) {
    for (int u = 0; u < X.Len(); u++) {
      if (NodeNameH.Len() > 0) {
        printf("NID: %s\t Attrs: ", NodeNameH.GetKey(NIDToIdx[u]));
      } else {
        printf("NID: %d\t Attrs: ", NIDToIdx[u].Val);
      }
      for (int k = 0; k < X[u].Len(); k++) {
        printf("%d, ", X[u][k].Val);
      }
      printf("\n");
      if (u >= TopK) { break; }
    }
  }
  double LikelihoodHoldOut();
  double GetStepSizeByLineSearch(const int UID, const TIntFltH& DeltaV, const TIntFltH& GradV, const double& Alpha, const double& Beta, const int MaxIter = 10);
  double GetStepSizeByLineSearchForWK(const int K, const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta, const int MaxIter = 10) {
    double StepSize = 1.0;
    double InitLikelihood = LikelihoodForWK(K);
    TFltV NewVarV(DeltaV.Len());
    IAssert(DeltaV.Len() == NumComs + 1);
    for(int iter = 0; iter < MaxIter; iter++) {
      for (int c = 0; c < DeltaV.Len(); c++){
        double NewVal = W[K][c] + StepSize * DeltaV[c];
        if (NewVal < MinValW) { NewVal = MinValW; }
        if (NewVal > MaxValW) { NewVal = MaxValW; }
        NewVarV[c] = NewVal;
      }
      if (LikelihoodForWK(K, NewVarV) < InitLikelihood + Alpha * StepSize * TLinAlg::DotProduct(GradV, DeltaV)) {
        StepSize *= Beta;
      } else {
        break;
      }
      if (iter == MaxIter - 1) { 
        StepSize = 0.0;
        break;
      }
    }
    return StepSize;
  }
  int GetPositiveW() {
    int PosCnt = 0;
    for (int c = 0; c < NumComs; c++) {
      for (int k = 0; k < Attrs; k++) {
        if (GetW(c, k) > 0.0) { PosCnt++; }
      }
    }
    return PosCnt;
  }
  int MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const int ChunkSize, const TStr PlotNm, const double StepAlpha = 0.3, const double StepBeta = 0.1);
  int MLEGradAscentParallel(const double& Thres, const int& MaxIter, const int ChunkNum, const TStr PlotNm = TStr(), const double StepAlpha = 0.3, const double StepBeta = 0.1) {
    int ChunkSize = G->GetNodes() / 10 / ChunkNum;
    if (ChunkSize == 0) { ChunkSize = 1; }
    return MLEGradAscentParallel(Thres, MaxIter, ChunkNum, ChunkSize, PlotNm, StepAlpha, StepBeta);
  }
  //double FindOptimalThres(const TVec<TIntV>& TrueCmtyVV, TVec<TIntV>& CmtyVV);
  double inline GetCom(const int& NID, const int& CID) {
    if (F[NID].IsKey(CID)) {
      return F[NID].GetDat(CID);
    } else {
      return 0.0;
    }
  }
  double inline GetAttr(const int& NID, const int& K) {
    if (X[NID].IsKey(K)) {
      return 1.0;
    } else {
      return 0.0;
    }
  }
  void inline AddCom(const int& NID, const int& CID, const double& Val) {
    if (F[NID].IsKey(CID)) {
      SumFV[CID] -= F[NID].GetDat(CID);
    }
    F[NID].AddDat(CID) = Val;
    SumFV[CID] += Val;
  }

  void inline DelCom(const int& NID, const int& CID) {
    if (F[NID].IsKey(CID)) {
      SumFV[CID] -= F[NID].GetDat(CID);
      F[NID].DelKey(CID);
    }
  }
  /*
  double inline DotProduct(const TIntFltH& UV, const TFltV& VV) {
    double DP = 0;
    for (TIntFltH::TIter HI = UV.BegI(); HI < UV.EndI(); HI++) {
      DP += VV[HI.GetKey()] * HI.GetDat(); 
    }
    return DP;
  }
  */
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
  double inline DotProduct(const int& UID, const int& VID) {
    return DotProduct(F[UID], F[VID]);
  }
  double inline Prediction(const TIntFltH& FU, const TIntFltH& FV) {
    double DP = log (1.0 / (1.0 - PNoCom)) + DotProduct(FU, FV);
    IAssertR(DP > 0.0, TStr::Fmt("DP: %f", DP));
    return exp(- DP);
  }
  double inline PredictAttrK(const TIntFltH& FU, const TFltV& WK) {
    double DP = 0.0;
    for (TIntFltH::TIter FI = FU.BegI(); FI < FU.EndI(); FI++) {
      DP += FI.GetDat() * WK[FI.GetKey()];
    }
    DP += WK.Last();
    return Sigmoid(DP);
  }
  double inline PredictAttrK(const TIntFltH& FU, const int K) {
    return PredictAttrK(FU, W[K]);
  }
  double inline PredictAttrK(const int UID, const int K) {
    return PredictAttrK(F[UID], W[K]);
  }
  double inline GetW(const int CID, const int K) {
    return W[K][CID];
  }
  double inline Prediction(const int& UID, const int& VID) {
    return Prediction(F[UID], F[VID]);
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
  /*
  double inline Norm1(const TFltV& UV) {
    double N = 0.0;
    for (int i = 0; i < UV.Len(); i++) {
      N += fabs(UV[i]);
    }
    return N;
  }
  */
  double inline Sigmoid(const double X) {
    return 1.0 / ( 1.0 + exp(-X));
  }
};


#endif
