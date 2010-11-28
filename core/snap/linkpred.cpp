#include "stdafx.h"
#include "linkpred.h"

/////////////////////////////////////////////////
// Link prediction examplevoid PrepareGraph();

// random walk with restars to node JumpNId
template <class PGraph>
void GetRndWalkRestart(const PGraph& Graph, const double& JumpProb, const int& JumpNId, THash<TInt, TFlt>& RwrNIdH) {
  const double DefVal = 1.0/Graph->GetNodes();
  RwrNIdH.Clr(false);
  TIntH NIdOutDegH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    RwrNIdH.AddDat(NI.GetId(), DefVal);
    NIdOutDegH.AddDat(NI.GetId(), NI.GetOutDeg());
  }
  THash<TInt, TFlt> RwrNIdH2(Graph->GetNodes());
  for (int iter = 0; iter < 10; iter++) {
    double Sum = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      double Val = 0;
      for (int i = 0; i < NI.GetInDeg(); i++) {
        const int InId = NI.GetInNId(i);
        Val += (1.0-JumpProb) * 1.0/(NIdOutDegH.GetDat(InId)) * RwrNIdH.GetDat(InId);
      }
      if (NI.GetId() == JumpNId) { Val+= JumpProb; }
      RwrNIdH.AddDat(NI.GetId(), Val);
      Sum += Val;
    }
    for (int i = 0; i < RwrNIdH.Len(); i++) {
      RwrNIdH[i] /= Sum; 
    }
  }
}

// Adamic-Adar measure: sum_cmn_nbrs 1/log(deg_cmn_nbr)
template <class PGraph>
double GetAdamicAdar(const PGraph& Graph, const int& SrcNId, const int& DstNId) {
  TIntV CmnV;
  TSnap::GetCmnNbhs(Graph, SrcNId, DstNId, CmnV);
  double Aa = 0;
  for (int c=0; c < CmnV.Len(); c++) {
    Aa += 1.0/log(Graph->GetNI(CmnV[c]).GetDeg());
  }
  return Aa;
}

  
// Adamic-Adar measure (defined for all hop-2 neighbors of SrcNId)
template <class PGraph>
void GetAdamicAdar(const PGraph& Graph, const int& SrcNId, THash<TInt, TFlt>& AaNIdH) {
  TIntV NIdV, CmnV;
  TSnap::GetNodesAtHop(Graph, SrcNId, 2, NIdV, false);
  AaNIdH.Clr(false);
  for (int i = 0; i < NIdV.Len(); i++) {
    const int DstNId = NIdV[i];
    TSnap::GetCmnNbhs(Graph, SrcNId, DstNId, CmnV);
    double Aa=0;
    for (int c=0; c < CmnV.Len(); c++) {
      Aa += 1.0/log((double)Graph->GetNI(CmnV[c]).GetDeg());
    }
    AaNIdH.AddDat(DstNId, Aa);
  }
}

// attributes (src_deg, dst_deg, cmn_nbrs, dst_prank, adamic_adar)
void TLpExample::GetNetAttrV(TVec<TFltV>& AttrV) const { // (dstnid, class, features)
  THash<TInt, TFlt> RwrNIdH, AdamicAdar;
  GetRndWalkRestart(PNet((TLpExample*)this), 0.15, SrcNId, RwrNIdH);
  GetAdamicAdar(PNet((TLpExample*)this), SrcNId, AdamicAdar);
  // get attr
  AttrV.Gen(DstNIdV.Len()+NolNIdV.Len(), 0);
  const TNodeI SrcNI = GetNI(SrcNId);
  for (int i = 0; i < DstNIdV.Len(); i++) {
    const int Dst = DstNIdV[i];
    const int Cmn = TSnap::GetCmnNbhs(PNet((TLpExample*)this), SrcNId, Dst);
    AttrV.Add();  TFltV& A = AttrV.Last();
    A.Add(DstNIdV[i]()); A.Add(1);
    A.Add(SrcNI.GetDeg());   A.Add(GetNI(Dst).GetDeg());
    A.Add(Cmn);  A.Add(RwrNIdH.GetDat(Dst));  A.Add(AdamicAdar.GetDat(Dst)); 
  }
  for (int i = 0; i < NolNIdV.Len(); i++) {
    const int Dst = NolNIdV[i];
    const int Cmn = TSnap::GetCmnNbhs(PNet((TLpExample*)this), SrcNId, Dst);
    AttrV.Add();  TFltV& A = AttrV.Last();
    A.Add(Dst); A.Add(0);
    A.Add(SrcNI.GetDeg());   A.Add(GetNI(Dst).GetDeg());
    A.Add(Cmn);  
    A.Add(RwrNIdH.GetDat(Dst)); 
    if (AdamicAdar.IsKey(Dst)) {
      A.Add(AdamicAdar.GetDat(Dst)); }
    else { A.Add(0); }
  }
}

void TLpExample::GetNodeAttrV(TVec<TFltV>& AttrV) const { // (dstnid, class, features)
  THash<TInt, TPair<TInt, TFltV> > NIdAttrH;
  for (TEdgeI EI=BegEI(); EI<EndEI(); EI++) {
    const TFltV& EA = EI().AttrV;
    if (! NIdAttrH.IsKey(EI.GetSrcNId())) {
      NIdAttrH.AddDat(EI.GetSrcNId(), TPair<TInt, TFltV>(1, EI().AttrV)); }
    else {
      TPair<TInt, TFltV>& A = NIdAttrH.GetDat(EI.GetSrcNId());
      for (int e = 0; e < EA.Len(); e++) { A.Val2[e]+=EA[e]; } A.Val1++;
    }
    /*if (! NIdAttrH.IsKey(EI.GetDstNId())) {
      NIdAttrH.AddDat(EI.GetDstNId(), TPair<TInt, TFltV>(1, EI().AttrV)); }
    else {
      TPair<TInt, TFltV>& A = NIdAttrH.GetDat(EI.GetDstNId());
      //for (int e = 0; e < EA.Len(); e++) { A.Val2[e]+=EA[e]; } A.Val1++;
    }*/
  }
  // average
  for (int i = 0; i <NIdAttrH.Len(); i++) {
    TPair<TInt, TFltV>& A = NIdAttrH[i];
    for (int i =0; i < A.Val2.Len(); i++) { 
      A.Val2[i] /= double(A.Val1()); }
  } //*/
  // get AttrV
  AttrV.Gen(DstNIdV.Len()+NolNIdV.Len(), 0);
  const TFltV& SrcAV = NIdAttrH.GetDat(SrcNId).Val2;
  for (int i = 0; i < DstNIdV.Len(); i++) {
    AttrV.Add();  TFltV& A = AttrV.Last();
    A.Add(DstNIdV[i]()); A.Add(1);
    A.AddV(SrcAV); A.AddV(NIdAttrH.GetDat(DstNIdV[i]).Val2);
  }
  for (int i = 0; i < NolNIdV.Len(); i++) {
    AttrV.Add();  TFltV& A = AttrV.Last();
    A.Add(NolNIdV[i]()); A.Add(0);
    A.AddV(SrcAV); A.AddV(NIdAttrH.GetDat(NolNIdV[i]).Val2);
  }
}

void TLpExample::GetLen2PathAttrV(TVec<TFltV>& AttrV) const { // (dstnid, class, features)
  TIntV CmnV;
  AttrV.Gen(DstNIdV.Len()+NolNIdV.Len(), 0);
  TIntV NIdV(DstNIdV);  NIdV.AddV(NolNIdV);
  for (int i = 0; i < NIdV.Len(); i++) {
    TFltV AttrV1(GetNAttr()), AttrV2(GetNAttr());
    int a1=0, a2=0;
    const int DstNId = NIdV[i];
    TSnap::GetCmnNbhs(PNet((TLpExample*)this), SrcNId, DstNId, CmnV);
    for (int c = 0; c < CmnV.Len(); c++) {
      const int C = CmnV[c];
      if (IsEdge(SrcNId, C)) {
        const TFltV& A = GetEDat(SrcNId, C).AttrV;
        for (int a=0;a<A.Len();a++) { AttrV1[a]+=A[a]; } a1++; }
      else {Fail; }
      /*if (IsEdge(C, SrcNId)) {
        const TFltV& A = GetEDat(C, SrcNId).AttrV;
        for (int a=0;a<A.Len();a++) { AttrV1[a]+=A[a]; } a1++; }
      // destination */
      if (IsEdge(DstNId, C)) {
        const TFltV& A = GetEDat(DstNId, C).AttrV;
        for (int a=0;a<A.Len();a++) { AttrV2[a]+=A[a]; } a2++; }
      else { Fail; }
      /*if (IsEdge(C, DstNId)) {
        const TFltV& A = GetEDat(C, DstNId).AttrV;
        for (int a=0;a<A.Len();a++) { AttrV2[a]+=A[a]; } a2++; }*/
    }
    // average
    for (int a=0;a<AttrV1.Len();a++) { AttrV1[a]/=double(a1); }
    for (int a=0;a<AttrV2.Len();a++) { AttrV2[a]/=double(a2); }
    AttrV.Add();
    AttrV.Last().Add(DstNId);
    if (i < DstNIdV.Len()) { AttrV.Last().Add(1); }
    else { AttrV.Last().Add(0); }
    AttrV.Last().AddV(AttrV1);
    AttrV.Last().AddV(AttrV2);
  }
}

void TLpExample::SaveAttrV(FILE *F, const bool& NetA, const bool& NodeA, const bool& PathA) const {
  TVec<TFltV> NetAV, NodeAV, PathAV;
  if (NetA) { GetNetAttrV(NetAV); }
  if (NodeA) { GetNodeAttrV(NodeAV); }
  if (PathA) { GetLen2PathAttrV(PathAV); }
  const TVec<TFltV>& NonE = NetAV.Len()>0?NetAV:(NodeAV.Len()>0?NodeAV:PathAV);
  for (int e = 0; e < NonE.Len(); e++) {
    fprintf(F, "%d\t%d\t%d", SrcNId.Val, (int)NonE[e][0](), e<DstNIdV.Len()?1:0); // class
    if (NodeAV.Len()) { for (int a=2; a<NodeAV[e].Len(); a++) { fprintf(F, "\t%f", NodeAV[e][a].Val); } }
    if (NetAV.Len())  { for (int a=2; a<NetAV[e].Len(); a++) { fprintf(F, "\t%f", NetAV[e][a].Val); } }
    if (PathAV.Len()) { for (int a=2; a<PathAV[e].Len(); a++) { fprintf(F, "\t%f", PathAV[e][a].Val); } }
    fprintf(F, "\n");
  }
}

void TLpExample::InitQGraph() {
  const int NAttr = GetNAttr();
  if (DummyNId == -1) {
    DummyNId = AddNode();
    const TEdgeGrad DummyEG(NAttr);
    for (TNodeI NI = BegNI(); NI < DummyNI(); NI++) { // edges to dummy
      AddEdge(NI.GetId(), DummyNId, DummyEG); }
    AddEdge(DummyNId, SrcNId, DummyEG); // dummy to source node
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    TEdgeGrad& EG = EI(); 
    EG.Val = 0;
    EG.GradV.PutAll(0);
  }
  // prepare pagerank 
  PRankH.Gen(GetNodes());
  PrGradH.Gen(GetNodes());
  TFltV GradV(NAttr);
  GradV.PutAll(0.0);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    PRankH.AddDat(NI.GetId(), 1.0/(double)GetNodes());
    PrGradH.AddDat(NI.GetId(), GradV);
  }
}

void TLpExample::UpdateQGraph(const TLpPredictor& Predictor) {
  TFltV SumGradV(GetNAttr());
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    double Z = 0.0;
    SumGradV.PutAll(0.0);
    // get edge weights and normalizing constant (sum FVal, sum Grad)
    const int I = NI.GetId();
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      const int J = NI.GetOutNId(i);
      TEdgeGrad& EG = NI.GetOutEDat(i);
      if (I == DummyNId) { 
        EG.Val = 1; 
      } else if (J == DummyNId) { 
        EG.Val = Predictor.Alpha; 
      } else { 
        EG.Val = Predictor.GetFVal(EG.AttrV);
        Predictor.GetFGrad(EG.AttrV, EG.GradV);
        Z += EG.Val;
        for (int k = 0; k < SumGradV.Len(); k++) { 
          SumGradV[k] += EG.GradV[k]; }
      }
    }
    // calc Q_ij and Q_ij'
    for (int j = 0; j < NI.GetOutDeg(); j++) {
      const int J = NI.GetOutNId(j);
      if (I != DummyNId && J != DummyNId) { 
        TEdgeGrad& EG = NI.GetOutEDat(j);
        for (int k = 0; k < SumGradV.Len(); k++) { // gradient
          EG.GradV[k] = (1.0-Predictor.Alpha) * (EG.GradV[k]*Z - EG.Val*SumGradV[k]) / TMath::Sqr(Z); }
        EG.Val = (1.0-Predictor.Alpha) * EG.Val / Z;
      }
    }
  }
}

void TLpExample::CalcPageRank() {
  const double InitVal = 1.0/double(GetNodes());
  const int NAttr = GetNAttr();
  for (int i = 0; i < PRankH.Len(); i++) {
    PRankH[i] = InitVal;
    PrGradH[i].PutAll(0);
  }
  THash<TInt, TFlt> NewPrH(PRankH);
  THash<TInt, TFltV> NewGrH(PrGradH);
  for (int i = 0; i < NewPrH.Len(); i++) { 
    NewPrH[i] = 0; 
    NewGrH[i].PutAll(0);
  }
  double s=0, sumDelta=1, sumDelta2=1;
  for (int T = 0; T < 100 && sumDelta>1e-2; T++) {
    // page rank
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      double sum = 0;
      const int I = NI.GetId();
      const double SrcPr = PRankH.GetDat(I);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int J = NI.GetOutNId(e);
        NewPrH.GetDat(J) += SrcPr * NI.GetOutEDat(e).Val; }
    }
    s=0; sumDelta=0;
    printf("\n***ITER %d\n", T+1);
    //Dump(true, true, true);
    //printf("PageRank:\n");
    for (int i = 0; i < NewPrH.Len(); i++) { 
      //printf("  %d\t%f\n", NewPrH.GetKey(i), NewPrH[i]);
      sumDelta += fabs(PRankH[i]-NewPrH[i]);
      s += NewPrH[i];
      PRankH[i] = NewPrH[i];  NewPrH[i]=0; 
    }
    printf("\t%f (Delta: %f)\n", s, sumDelta);
    // gradient
    for (TNodeI NI = BegNI(); NI < DummyNI(); NI++) {
      const int I = NI.GetId();
      const double SrcPR = PRankH.GetDat(I);
      const TFltV& SrcGR = PrGradH.GetDat(I);
      for (int e = 0; e < NI.GetOutDeg()-1; e++) {
        const int J = NI.GetOutNId(e);
        const TEdgeGrad& EG = NI.GetOutEDat(e);
        TFltV& NewDstGr = NewGrH.GetDat(J);
        for (int k = 0; k < NAttr; k++) {
          NewDstGr[k] +=  SrcPR * EG.GradV[k] + SrcGR[k] * EG.Val; }
      }
    }
    //printf("PageRank gradients\n");
    sumDelta2=0;
    for (int i = 0; i < NewGrH.Len(); i++) { 
      //printf("  %d\t", NewGrH.GetKey(i));
      const TFltV& GrV = NewGrH[i];
      const TFltV& OldV = PrGradH[i];
      for (int k = 0; k < NAttr; k++) {
        sumDelta2 += fabs(GrV[k]-OldV[k]);
        //printf("\t%f", GrV[k]);
      }
      //printf("\n");
      PrGradH[i].Swap(NewGrH[i]);  NewGrH[i].PutAll(0); 
    }
    printf("\t%Delta: %f\n", sumDelta2);
  }
}

void TLpExample::GenSmallExample1() {
  for (int i = 0; i < 3; i++) { AddNode(i); }
  AddEdge(0,1, TEdgeGrad(TFltV::GetV(0,1,1)));
  AddEdge(1,0, TEdgeGrad(TFltV::GetV(0,1,1)));
  AddEdge(0,2, TEdgeGrad(TFltV::GetV(1,0,1)));
  AddEdge(2,0, TEdgeGrad(TFltV::GetV(1,0,1)));
  SrcNId = 0;
  DstNIdV = TIntV::GetV(1);
  NolNIdV = TIntV::GetV(2);
}

void TLpExample::GenSmallExample2() {
  for (int i = 0; i < 7; i++) { AddNode(i); }
  AddEdge(0,1, TEdgeGrad(TFltV::GetV(1,1)));
  AddEdge(0,2, TEdgeGrad(TFltV::GetV(1,1)));
  AddEdge(2,3, TEdgeGrad(TFltV::GetV(1,1)));
  AddEdge(1,3, TEdgeGrad(TFltV::GetV(1,1)));
  AddEdge(0,4, TEdgeGrad(TFltV::GetV(0,0)));
  AddEdge(4,5, TEdgeGrad(TFltV::GetV(1,1)));
  AddEdge(4,6, TEdgeGrad(TFltV::GetV(0,1)));
  SrcNId = 0;
  DstNIdV = TIntV::GetV(6);
  NolNIdV = TIntV::GetV(3, 5);
}

void TLpExample::SaveTxt(FILE *F) const {
  fprintf(F, "%d\n", SrcNId.Val);
  fprintf(F, "%d", DstNIdV[0].Val);
  for (int i = 1 ; i < DstNIdV.Len(); i++) { fprintf(F, "\t%d", DstNIdV[i].Val); }
  fprintf(F, "\n%d", NolNIdV[0].Val);
  for (int i = 1 ; i < NolNIdV.Len(); i++) { fprintf(F, "\t%d", NolNIdV[i].Val); }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI.GetSrcNId()==DummyNId || EI.GetDstNId()==DummyNId) { continue; }
    fprintf(F, "\n%d\t%d", EI.GetSrcNId(), EI.GetDstNId());
    const TFltV& AttrV = EI().AttrV;
    for (int a = 0; a < AttrV.Len(); a++) {
      fprintf(F, "\t%f", AttrV[a].Val); }
  }
  fprintf(F, "\n\n");
}

void TLpExample::Dump(const bool& DumpPRank, const bool& DumpGrad, const bool& DumpNet) const {
  if (DumpPRank) {
    printf("PageRank:\n");
    for (int i = 0; i < PRankH.Len(); i++) {
      printf("  %d\t%f\n", PRankH.GetKey(i).Val, PRankH[i].Val); }
    if (DumpGrad) { printf("\n"); }
  }
  if (DumpGrad) {
    printf("PageRank Gradient:\n");
    for (int i = 0; i < PrGradH.Len(); i++) {
      printf("  %d", PrGradH.GetKey(i).Val);
      for (int j = 0; j < PrGradH[i].Len(); j++) {
        printf("\t%f", PrGradH[i][j].Val); }
      printf("\n");
    }
    if (DumpNet) { printf("\n"); }
  }
  if (DumpNet) {
    printf("Q-NET:\n");
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      printf("%d:", NI.GetId());
      for (int i = 0; i < NI.GetOutDeg(); i++) {
        printf("  %d:%f", NI.GetOutNId(i), NI.GetOutEDat(i).Val()); }
      printf("\n");
    }
    for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
      printf("%d -- %d  G:", EI.GetSrcNId(), EI.GetDstNId());
      for (int i = 0; i < EI().GradV.Len(); i++) {
        printf("\t%f", EI().GradV[i].Val); }
      /*printf("\n  A:");
      for (int i = 0; i < EI().AttrV.Len(); i++) {
        printf("\t%f", EI().AttrV[i]); }*/
      printf("\n");
    }
  }
}

// load a vector of examples
void TLpExample::LoadTxt(const TStr& FNm, TVec<TLpExample>& ExV) {
  TSsParser SS(FNm, ssfTabSep);
  printf("load:");
  while (SS.Next()) {
    ExV.Add();
    TLpExample& Ex = ExV.Last();
    Ex.SrcNId = SS.GetInt(0); IAssert(SS.Len()==1);
    SS.Next();
    for (int s = 0; s < SS.Len(); s++) {
      Ex.DstNIdV.Add(SS.GetInt(s)); }
    SS.Next();
    for (int s = 0; s < SS.Len(); s++) {
      Ex.NolNIdV.Add(SS.GetInt(s)); }
    SS.Next();
    do {
      TFltV AttrV;
      //printf("%s\n", SS.DumpStr());
      for (int i = 2; i < SS.Len(); i++) { AttrV.Add(SS.GetFlt(i)); }
      const int I=SS.GetInt(0), J=SS.GetInt(1);
      //printf("%d\t%d\n", SS.GetInt(0), SS.GetInt(1));
      if (! Ex.IsNode(I)) { Ex.AddNode(I); }
      if (! Ex.IsNode(J)) { Ex.AddNode(J); }
      Ex.AddEdge(I, J, AttrV);
      AttrV.Swap(0,1); Ex.AddEdge(J, I, AttrV); // add the return edge
      if (! SS.Next()) { break;}
    } while (SS.Len() > 1);
    printf("  %d", Ex.SrcNId.Val);
  }
  printf("\nitems: %d\n", ExV.Len());
}

/////////////////////////////////////////////////
// Link prediction Predictor
double TLpPredictor::GetFVal(const TFltV& AttrV) const {
  Assert(WgtV.Len() == AttrV.Len());
  double Val = 0;
  for (int i = 0; i < WgtV.Len(); i++) {
    Val += WgtV[i]*AttrV[i];
  }
  return exp(-Val);
}

void TLpPredictor::GetFGrad(const TFltV& AttrV, TFltV& GradV) const {
  const double Val = GetFVal(AttrV);
  if (GradV.Len() != WgtV.Len()) {
    GradV.Gen(WgtV.Len()); }
  for (int i = 0; i < AttrV.Len(); i++) {
    GradV[i] = - Val * AttrV[i];
  }
}

// Huber loss
double TLpPredictor::GetLoss(const double& Val) const {
  if (Val < 0) { return 0; }
  else if (Val < Z) { return Val*Val/(2.0*Z); }
  else { return Val-Z/2.0; }
}

double TLpPredictor::GetLossGrad(const double& Val) const {
  if (Val < 0) { return 0; }
  else if (Val < Z) { return Val/Z; }
  else { return 1; }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace __OLD__ {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Link prediction example
void TLpExample::GenSmallExample1() {
  Graph = TNGraph::New();
  for (int i = 0; i < 3; i++) { Graph->AddNode(i); }
  Graph->AddEdge(0,1);                          // edge
  Graph->AddEdge(1,0);
  Graph->AddEdge(0,2);
  Graph->AddEdge(2,0);
  EAttrV.AddDat(TIntPr(0,1), TFltV::GetV(0,1,1)); // edge attribute vector
  EAttrV.AddDat(TIntPr(1,0), TFltV::GetV(0,1,1));
  EAttrV.AddDat(TIntPr(0,2), TFltV::GetV(1,0,1));
  EAttrV.AddDat(TIntPr(2,0), TFltV::GetV(1,0,1));
  SrcNId = 0;
  DstNIdV = TIntV::GetV(1);
  NolNIdV = TIntV::GetV(2);
}

void TLpExample::GenSmallExample2() {
  Graph = TNGraph::New();
  for (int i = 0; i < 7; i++) { Graph->AddNode(i); }
  // add edges
  Graph->AddEdge(0,1);
  Graph->AddEdge(0,2);
  Graph->AddEdge(2,3);
  Graph->AddEdge(1,3);
  Graph->AddEdge(0,4);
  Graph->AddEdge(4,5);
  Graph->AddEdge(4,6);
  EAttrV.AddDat(TIntPr(0,1), TFltV::GetV(1,1));
  EAttrV.AddDat(TIntPr(0,2), TFltV::GetV(1,1));
  EAttrV.AddDat(TIntPr(2,3), TFltV::GetV(1,1));
  EAttrV.AddDat(TIntPr(1,3), TFltV::GetV(1,1));
  EAttrV.AddDat(TIntPr(0,4), TFltV::GetV(0,0));
  EAttrV.AddDat(TIntPr(4,5), TFltV::GetV(1,1));
  EAttrV.AddDat(TIntPr(4,6), TFltV::GetV(0, 1));
  SrcNId = 0;
  DstNIdV = TIntV::GetV(6);
  NolNIdV = TIntV::GetV(3, 5);
}

/////////////////////////////////////////////////
// Link prediction Predictor
double TLpPredictor::GetFVal(const TFltV& WgtV, const TFltV& AttrV) const {
  Assert(WgtV.Len() == AttrV.Len());
  double Val = 0;
  for (int i = 0; i < WgtV.Len(); i++) {
    Val += WgtV[i]*AttrV[i];
  }
  return exp(-Val);
}

void TLpPredictor::GetFGrad(const TFltV& WgtV, const TFltV& AttrV, TFltV& GradV) const {
  const double Val = GetFVal(WgtV, AttrV);
  if (GradV.Len() != WgtV.Len()) {
    GradV.Gen(WgtV.Len()); }
  for (int i = 0; i < AttrV.Len(); i++) {
    GradV[i] = - Val * AttrV[i];
  }
}

// Huber loss
double TLpPredictor::GetLoss(const double& Val) const {
  if (Val < 0) { return 0; }
  else if (Val < Z) { return Val*Val/(2.0*Z); }
  else { return Val-Z/2.0; }
}

double TLpPredictor::GetLossGrad(const double& Val) const {
  if (Val < 0) { return 0; }
  else if (Val < Z) { return Val/Z; }
  else { return 1; }
}

/////////////////////////////////////////////////
// Stochastic transition matrix: each directed edge has value and gradient
void TQGraph::UpdateNormConst() {
  // reset
  for (int i = 0; i < EdgeGradH.Len(); i++) {
    TEdgeGrad& E = NormConstH.AddDat(EdgeGradH.GetKey(i).Val1);
    E.Val1 = 0;
    if (! E.Val2.Empty()) { E.Val2.PutAll(0.0); }
    else { E.Val2.Gen(EdgeGradH[i].Val2.Len()); }
  }
  // calculate norm const
  for (int i = 0; i < EdgeGradH.Len(); i++) {
    TEdgeGrad& E = NormConstH.AddDat(EdgeGradH.GetKey(i).Val1);
    E.Val1 += EdgeGradH[i].Val1;
    for (int e = 0; e < E.Val2.Len(); e++) {
      E.Val2[e] += EdgeGradH[i].Val2[e];
    }
  }
}

void TQGraph::UpdateGrad(const double& Alpha, const TIntPr& Edge, TEdgeGrad& Grad) {
  IAssert(GetNDat(Edge.Val1)!=lntDummy && GetNDat(Edge.Val2)!=lntDummy);
  const TEdgeGrad& GradE = EdgeGradH.GetDat(Edge);
  const TEdgeGrad& NormConst = NormConstH.GetDat(Edge.Val1);
  Grad.Val1 = Alpha * GradE.Val1 / NormConst.Val1; // transition prob
  TFltV& GradV = Grad.Val2;
  printf("%d -- %d\n", Edge.Val1(), Edge.Val2());
  for (int i = 0; i < GradV.Len(); i++) {
    GradV[i] = Alpha * (GradE.Val2[i]*NormConst.Val1 - GradE.Val1*NormConst.Val2[i]) / TMath::Sqr(NormConst.Val1);
    //printf("%f = %f * (%f*%f - %f*%f) / %f\n", GradV[i], Alpha, GradE.Val2[i],NormConst.Val1,GradE.Val1,NormConst.Val2[i], TMath::Sqr(NormConst.Val1));
  }
}

void TQGraph::MakeGraph(const TLpExample& Example) {
  TEdgeGrad GradInit(0, TFltV(Example.Attrs()));
  // create graph
  Clr(false);
  PNGraph G = Example.Graph;
  for (TNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    AddNode(NI.GetId(), lntUndef);
  }
  DummyNId = AddNode(-1, lntDummy); // dummy node
  for (TNGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
    AddEdge(EI.GetSrcNId(), EI.GetDstNId(), GradInit);
  }
  for (TNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) { // edges to dummy
    AddEdge(NI.GetId(), DummyNId, GradInit);
  }
  AddEdge(DummyNId, Example.SrcNId, GradInit); // dummy to source node
  // set node types
  GetNDat(Example.SrcNId) = lntSrc;
  for (int i = 0; i < Example.DstNIdV.Len(); i++) {
    GetNDat(Example.DstNIdV[i]) = lntDst; }
  for (int i = 0; i < Example.NolNIdV.Len(); i++) {
    GetNDat(Example.NolNIdV[i]) = lntNoLink; }
}

void TQGraph::UpdateGraph(const TLpExample& Example, const TLpPredictor& Pred, const TFltV& WgtV) {
  const double Alpha = 0.15;
  if (Empty()) { MakeGraph(Example); }
  // update edge values and derivatives
  PNGraph G = Example.Graph;
  TFltV GradV;
  for (TNGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) { // pre-calculate edge gradients
    const TIntPr Edge(EI.GetSrcNId(), EI.GetDstNId());
    const double FVal = Pred.GetFVal(WgtV, Example.GetEAttrV(Edge));
    Pred.GetFGrad(WgtV, Example.GetEAttrV(Edge), GradV);
    EdgeGradH.AddDat(Edge, TEdgeGrad(FVal, GradV));
  }
  UpdateNormConst();
  // update QMatrix
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) { EI().Val1=-1; } // reset data
  for (TNodeI NI = BegNI(); NI < GetNI(DummyNId); NI++) {
    // edges of the network
    for (int i = 0; i < NI.GetOutDeg()-1; i++) {
      UpdateGrad(Alpha, TIntPr(NI.GetId(), NI.GetOutNId(i)), NI.GetOutEDat(i)); }
    // edge to dummy
    NI.GetOutEDat(NI.GetOutDeg()-1).Val1 = (1.0-Alpha);
    NI.GetOutEDat(NI.GetOutDeg()-1).Val2.PutAll(0.0);
  }
  // dummy to source
  GetEDat(DummyNId, Example.SrcNId).Val1 = 1.0;
  GetEDat(DummyNId, Example.SrcNId).Val2.PutAll(0.0);
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) { IAssert(EI().Val1!=-1.0); } // check data
}

void TQGraph::CalcPRankGrad() {
  const int NAttr = NormConstH[0].Val2.Len();
  if (PRankH.Empty()) { // init
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      PRankH.AddDat(NI.GetId(), 0);
      PrGradH.AddDat(NI.GetId(), TFltV(NAttr));
    }
  }
  const double InitVal = 1.0/double(GetNodes());
  for (int i = 0; i < PRankH.Len(); i++) {
    PRankH[i] = InitVal;
    PrGradH[i].PutAll(0);
  }
  THash<TInt, TFlt> NewPrH(PRankH);
  THash<TInt, TFltV> NewGrH(PrGradH);
  for (int T = 0; T < 2; T++) {
    // page rank
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      double sum = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        sum += PRankH.GetDat(NI.GetInNId(e)) * NI.GetInEDat(e).Val1; }
      NewPrH.AddDat(NI.GetId(), sum);
    }
    // page rank gradient
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      TFltV& RankV = NewGrH.GetDat(NI.GetId());
      RankV.PutAll(0);
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const double Qne = NI.GetInEDat(e).Val1;
        const TFltV& QGne = NI.GetInEDat(e).Val2;
        const double PRe = PRankH.GetDat(NI.GetInNId(e));
        const TFltV& GRe = PrGradH.GetDat(NI.GetInNId(e));
        for (int i = 0; i < NAttr; i++) {
          RankV[i] +=  Qne * GRe[i] + PRe * QGne[i];
        }
      }
    }
    PRankH = NewPrH;
    PrGradH = NewGrH;
    printf("\nITER %d\n", T+1);  Dump();
  }
}

double TQGraph::GetGradient(const TLpExample& Example, const TLpPredictor& Prd, TFltV& GradV) const {
  const double B = 0.1; // margin
  const int NAttr = NormConstH[0].Val2.Len();
  GradV.PutAll(0);
  double Loss=0;
  for (int l = 0; l < Example.NolNIdV.Len(); l++) {
    const TFltV& LGrad = PrGradH.GetDat(Example.NolNIdV[l]);
    const double LRank = PRankH.GetDat(Example.NolNIdV[l]);
    for (int d = 0; d < Example.DstNIdV.Len(); d++) {
      const TFltV& DGrad = PrGradH.GetDat(Example.DstNIdV[d]);
      const double DRank = PRankH.GetDat(Example.DstNIdV[d]);
      for (int i = 0; i < NAttr; i++) {
        GradV[i] += Prd.GetLossGrad(LGrad[i]+B-DGrad[i])*(LGrad[i]-DGrad[i]);
      }
      Loss += Prd.GetLoss(LRank+B-DRank);
    }
  }
  return Loss;
}

void TQGraph::Dump(const bool& OnlyPRank) const {
  printf("PageRank:\n");
  double S = 0;
  for (int i = 0; i < PRankH.Len(); i++) {
    TStr Str = "";
    switch(TLpNodeTy(GetNDat(PRankH.GetKey(i)).Val)) {
      //case lntSrc : Str="src"; break;
      case lntDst : Str="dst"; break;
      case lntNoLink : Str="nol"; break;
      //case lntDummy : Str="dmy"; break;
    }
    printf("%d\t%f\t%s\n", PRankH.GetKey(i).Val, PRankH[i](), Str.CStr());
    S+=PRankH[i];
  }
  printf("\t%f\n", S);
  if (! OnlyPRank) {
    printf("Gradient:\n");
    for (int i = 0; i < PrGradH.Len(); i++) {
      printf("%d", PrGradH.GetKey(i).Val);
      for (int j = 0; j < PrGradH[i].Len(); j++) {
        printf("\t%f", PrGradH[i][j].Val); }
      printf("\n");
    }
  }
  printf("Q-NET:\n");
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    printf("%d:", NI.GetId());
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      printf("  %d:%f", NI.GetOutNId(i), NI.GetOutEDat(i).Val1());
    }
    printf("\n");
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    printf("%d -- %d  G:", EI.GetSrcNId(), EI.GetDstNId());
    for (int i = 0; i < EI().Val2.Len(); i++) {
      printf("\t%f", EI().Val2[i].Val); }
    printf("\n");
  }
}

/////////////////////////////////////////////////
// Link prediction Base
void TLpBase::TestGrad() {
  const double Lambda = 0.001;
  LpData.GenSmallExample1();
  //LpData.GenSmallExample2();
  WgtV.Gen(LpData.Attrs());
  WgtV.PutAll(0);
  TFltV GradV(LpData.Attrs());
  for (int i = 0; i < 1; i++) {
    printf("\n----STEP %d\n", i+1);
    QGraph.UpdateGraph(LpData, LpPred, WgtV);
    QGraph.CalcPRankGrad();
    //QGraph.Dump(false);
    return;
    double Loss = QGraph.GetGradient(LpData, LpPred, GradV);
    for (int i = 0; i < WgtV.Len(); i++) {
      GradV[i] += Lambda*2*WgtV[i];
      Loss += Lambda*TMath::Sqr(WgtV[i]);
    }
    printf("LOSS: %f\n", Loss);
    printf("Grad:");
    for (int i = 0; i < GradV.Len(); i++) { printf("\t%f", GradV[i].Val); } printf("\n");
    printf("Wgt:");
    for (int i = 0; i < GradV.Len(); i++) {
      WgtV[i] += -2 * GradV[i];
      printf("\t%f", WgtV[i].Val); } printf("\n");
  }
}

}; // __OLD__
