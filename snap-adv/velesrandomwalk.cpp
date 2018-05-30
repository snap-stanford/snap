#include "stdafx.h"
#include "Snap.h"
#include "velesrandomwalk.h"

void Bfs(PWNet& InNet, TWNet::TNodeI Start, int& Dim){
  TQQueue<TWNet::TNodeI> ToVisit;
  ToVisit.Push(Start);
  Start.GetDat().GetRVec()[Dim] = 0;
  while (!ToVisit.Empty()) {
    TWNet::TNodeI NI = ToVisit.Top();
    ToVisit.Pop();
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      TWNet::TNodeI CurrNI = InNet->GetNI(NI.GetNbrNId(i));
      if (CurrNI.GetDat().GetRVec()[Dim] != -1){ continue; }
      CurrNI.GetDat().GetRVec()[Dim] = NI.GetDat().GetRVec()[Dim] + 1;
      ToVisit.Push(CurrNI);
    }
  }
}

void BuildRVec(PWNet& InNet) {
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    for (int i = 0; i < MDim; i++){ NI.GetDat().GetRVec()[i] = -1; }
  }
  TRnd Rnd(time(NULL));
  for (int i = 0; i < MDim; i++) {
    Bfs(InNet, InNet->GetRndNI(Rnd), i);
  }
}

//Get random element using alias sampling method
int64 AliasDrawInt(const TIntVFltVPr& NTTable, TRnd& Rnd) {
  int64 N = NTTable.GetVal1().Len();
  TInt X = static_cast<int64>(Rnd.GetUniDev()*N);
  double Y = Rnd.GetUniDev();
  return Y < NTTable.GetVal2()[X] ? X : NTTable.GetVal1()[X];
}

//Preprocess alias sampling method
void GetNodeAlias(TFltV& PTblV, TIntVFltVPr& NTTable) {
  int64 N = PTblV.Len();
  TIntV& KTbl = NTTable.Val1;
  TFltV& UTbl = NTTable.Val2;
  for (int64 i = 0; i < N; i++) {
    KTbl[i] = 0;
    UTbl[i] = 0;
  }
  TIntV UnderV;
  TIntV OverV;
  for (int64 i = 0; i < N; i++) {
    UTbl[i] = PTblV[i]*N;
    if (UTbl[i] < 1) {
      UnderV.Add(i);
    } else {
      OverV.Add(i);
    }
  }
  while (UnderV.Len() > 0 && OverV.Len() > 0) {
    int64 Small = UnderV.Last();
    int64 Large = OverV.Last();
    UnderV.DelLast();
    OverV.DelLast();
    KTbl[Small] = Large;
    UTbl[Large] = UTbl[Large] + UTbl[Small] - 1;
    if (UTbl[Large] < 1) {
      UnderV.Add(Large);
    } else {
      OverV.Add(Large);
    }
  }
}

void TKdTree::TTreeNode::Init(TIntV& IndicesV, TFltV& Weights, TVec<TIntV>& Embs,
 TIntV& SplitV, int Level, TKdTree& ParentTree) {
  Depth = Level;
  if (Depth+1 == MDim || IndicesV.Len() < 2) {
    Leaf = 1;
    PTable = TIntVFltVPr(TIntV(IndicesV.Len()),TFltV(IndicesV.Len()));
    double Sum = 0;
    for (int i = 0; i < Weights.Len(); i++) { Sum += Weights[i]; }
    for (int i = 0; i < Weights.Len(); i++) { Weights[i] /= Sum; }
    GetNodeAlias(Weights, PTable);
    Indices = IndicesV;
    return ;
  }
  int SplitD = SplitV[Depth];
  int NPos, NNeg;
  NPos = NNeg = 0;
  for (int i = 0; i < Embs.Len(); i++) {
    if ( Embs[i][SplitD] == -1 ) { NNeg++; }
    if ( Embs[i][SplitD] == 1 ) { NPos++; }
  }
  TIntV PosIndicesV, NegIndicesV;
  TFltV PosWeightsV, NegWeightsV;
  TVec<TIntV> PosEmbs, NegEmbs;
  int ZeroCnt = 0;
  for (int i = 0; i < Embs.Len(); i++) {
    if ( NNeg >= Embs.Len()/2 ) {
      if ( Embs[i][SplitD] > -1 || NegIndicesV.Len() > Embs.Len()/2 ) {
        PosIndicesV.Add(IndicesV[i]);
	PosWeightsV.Add(Weights[i]);
	PosEmbs.Add(Embs[i]);
      } else {
        NegIndicesV.Add(IndicesV[i]);
	NegWeightsV.Add(Weights[i]);
	NegEmbs.Add(Embs[i]);
      }
      continue;
    }
    if ( NPos >= Embs.Len()/2 ) {
      if ( Embs[i][SplitD] < 1 || PosIndicesV.Len() > Embs.Len()/2 ) {
        NegIndicesV.Add(IndicesV[i]);
	NegWeightsV.Add(Weights[i]);
	NegEmbs.Add(Embs[i]);
      } else {
        PosIndicesV.Add(IndicesV[i]);
	PosWeightsV.Add(Weights[i]);
	PosEmbs.Add(Embs[i]);
      }
      continue;
    }
    if ( Embs[i][SplitD] == 1 || (Embs[i][SplitD] == 0 && NPos + ZeroCnt++ <= Embs.Len()/2)) {
      PosIndicesV.Add(IndicesV[i]);
      PosWeightsV.Add(Weights[i]);
      PosEmbs.Add(Embs[i]);
    } else {
      NegIndicesV.Add(IndicesV[i]);
      NegWeightsV.Add(Weights[i]);
      NegEmbs.Add(Embs[i]);
    }
  }
  UnderSum = OverSum = 0;
  for (int i = 0; i < NegWeightsV.Len(); i++) { UnderSum += NegWeightsV[i]; }
  for (int i = 0; i < PosWeightsV.Len(); i++) { OverSum += PosWeightsV[i]; }
  UnderId = ParentTree.GetNodes().Len();
  TTreeNode NewNode;
  ParentTree.GetNodes().Add(NewNode);
  OverId = ParentTree.GetNodes().Len();
  int TmpOverId=OverId;
  ParentTree.GetNodes().Add(NewNode);
  ParentTree.GetNodes()[UnderId].Init(NegIndicesV, NegWeightsV, NegEmbs, SplitV,
   Level+1, ParentTree);
  //IMPORTANT: At this point 'this' might not point to the object due to resizes and reallocations of vector of nodes in ParentTree
  //this is why some variables (TmpOverId and Level) are used locally
  ParentTree.GetNodes()[TmpOverId].Init(PosIndicesV, PosWeightsV, PosEmbs, SplitV,
   Level+1, ParentTree);
}

void TKdTree::Init(TIntV& IndicesV, TFltV& Weights, TVec<TIntV>& Embs) {
  SplitV = TIntV(MDim);
  for (int i = 0; i < MDim; i++) { SplitV[i] = i; }
  TRnd Rnd(IndicesV[0]);//Quazi random seed
  IndicesV.Shuffle(Rnd);
  TTreeNode NewNode;
  NodeV.Add(NewNode);
  NodeV[0].Init(IndicesV, Weights, Embs, SplitV, 0, *this);
}

int TKdTree::GetRndEl (TIntV& Src, double Q, TRnd& Rnd) {
  TInt Curr = 0;
  for (int i = 0; i < MDim && !NodeV[Curr].IsLeaf(); i++) {
    double Neg = NodeV[Curr].GetUnderSum();
    double Pos = NodeV[Curr].GetOverSum();
    if (Src[SplitV[i]] == -1) { Neg /= Q; }
    if (Src[SplitV[i]] == 1) { Pos /= Q; }
    Neg /= (Neg+Pos);
    if (Rnd.GetUniDev() > Neg) { Curr = NodeV[Curr].GetOverId(); }
    else { Curr = NodeV[Curr].GetUnderId(); }
  }
  return NodeV[Curr].GetIndices()[AliasDrawInt(NodeV[Curr].GetPTable(),Rnd)];
}

void Diff(const TIntV& A, const TIntV& B, TIntV& Res) {
  for (int i = 0; i < A.Len(); i++) { Res[i] = A[i] - B[i]; }
}

void PreprocessNode(PWNet& InNet, TWNet::TNodeI& NI) {
  TIntV IndicesV;
  TFltV WeightsV;
  TVec<TIntV> Embs;
  for (int i = 0; i < NI.GetOutDeg(); i++) {
    TWNet::TNodeI CurrI = InNet->GetNI(NI.GetNbrNId(i));
    IndicesV.Add(CurrI.GetId());
    WeightsV.Add(InNet->GetEDat(NI.GetId(),CurrI.GetId()));
    TIntV CurrEmb(MDim);
    Diff(NI.GetDat().GetRVec(),CurrI.GetDat().GetRVec(),CurrEmb);
    Embs.Add(CurrEmb);
  }
  NI.GetDat().GetDecisionTree().Init(IndicesV,WeightsV,Embs);
}

void PreprocessTransitionProbs(PWNet& InNet, bool& Verbose) {
  //Add imaginary edges so graph is undirected
  for (TWNet::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
    if (!InNet->IsEdge(EI.GetDstNId(), EI.GetSrcNId())) {
      InNet->AddEdge(EI.GetDstNId(), EI.GetSrcNId(), 0);
    }
  }
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    InNet->SetNDat(NI.GetId(),TNodeDat());
  }
  BuildRVec(InNet);
  //Remove imaginary edges
  for (TWNet::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
    if (EI.GetDat() <1e-14) {
      InNet->DelEdge(EI.GetSrcNId(), EI.GetDstNId());
    }
  }
  InNet->Defrag();
  int NCnt=0;
  TIntV NodeList;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) NodeList.Add(NI.GetId());
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < NodeList.Len(); i++) {
    if (Verbose && NCnt%100 == 0) {
      printf("\rPreprocessing progress: %.2lf%% ",(double)NCnt*100/(double)(InNet->GetNodes()));fflush(stdout);
    }
    TWNet::TNodeI NI = InNet->GetNI(NodeList[i]);
    PreprocessNode(InNet, NI);
    NCnt++;
  }
  if (Verbose) { printf("\n"); fflush(stdout); }
}

void SimulateWalk(PWNet& InNet, int StartNId, int& WalkLen, double& P, double& Q,
 TRnd& Rnd, TVec<TInt, int64>& WalkV) {
  WalkV.Add((int)StartNId);
  if (WalkLen == 1) { return; }
  if (InNet->GetNI(StartNId).GetDeg() == 0) { return; }
  int64 NextNode = InNet->GetNI(StartNId).GetNbrNId(Rnd.GetUniDevInt(InNet->GetNI(StartNId).GetDeg()));
  WalkV.Add(NextNode);
  double LastW = InNet->GetEDat(StartNId, NextNode);
  while (WalkV.Len() < WalkLen) {
    int64 Dst = WalkV.Last();
    int64 Src = WalkV.LastLast();
    if (InNet->GetNI(Dst).GetDeg() == 0) {
      WalkV.Add(Src);
      continue;
    }
    double ReturnW = LastW / P;
    ReturnW /= (InNet->GetNDat(Dst).GetDecisionTree().GetNodes()[0].GetUnderSum() + InNet->GetNDat(Dst).GetDecisionTree().GetNodes()[0].GetOverSum());
    if ( Rnd.GetUniDev() < ReturnW) {
      WalkV.Add(Src);
      continue;
    }
    TIntV SrcV(MDim);
    Diff(InNet->GetNDat(Dst).GetRVec(),InNet->GetNDat(Src).GetRVec(),SrcV);
    for (int i = 0; i < 5; i++) {//5 tries to get a non-source node. Then we give up
      NextNode = InNet->GetNDat(Dst).GetDecisionTree().GetRndEl(SrcV, Q, Rnd);
      if(NextNode != Src || InNet->GetNI(Dst).GetDeg() < 2) { break; }
    }
    LastW = InNet->GetEDat(Dst,NextNode);
    WalkV.Add(NextNode);
  }
}
