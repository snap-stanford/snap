#include "stdafx.h"
#include "mxdag.h"

namespace TSnap {

// number of edge going from left to right
int EvalMxDagOrdering(const PNGraph& Graph, const TIntV& NIdV) {
  TIntH NIdPosH(NIdV.Len());
  for (int i = 0; i < NIdV.Len(); i++) {
	  NIdPosH.AddDat(NIdV[i], i);
  }
  return EvalMxDagOrdering(Graph, NIdPosH);
}

// number of edge going from left to right
int EvalMxDagOrdering(const PNGraph& Graph, const TIntH& NIdPosH) {
  int OkEdges=0;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int MyPos = NIdPosH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (MyPos < NIdPosH.GetDat(NI.GetOutNId(e))) { OkEdges++; }
    }
  }
  return OkEdges;
}

// change in objective function of nodes NId1 and NId2 change positions
int EvalMxDagNodeSwap(const PNGraph& Graph, const TIntH& NIdPosH, const int& NId1, const int& NId2) {
  int oldOkEdges=0, newOkEdges=0;
  const int newPos1 = NIdPosH.GetDat(NId2);
  const int newPos2 = NIdPosH.GetDat(NId1);
  { TNGraph::TNodeI NI = Graph->GetNI(NId1);
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    const int DstPos = NIdPosH.GetDat(NI.GetOutNId(e));
    if (newPos2 < DstPos) { oldOkEdges++; }
    if (newPos1 < DstPos) { newOkEdges++; } }
  for (int e = 0; e < NI.GetInDeg(); e++) {
    const int DstPos = NIdPosH.GetDat(NI.GetInNId(e));
    if (DstPos < newPos2) { oldOkEdges++; }
    if (DstPos < newPos1) { newOkEdges++; } }
  }
  { TNGraph::TNodeI NI = Graph->GetNI(NId2);
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    const int DstPos = NIdPosH.GetDat(NI.GetOutNId(e));
    if (newPos1 < DstPos) { oldOkEdges++; }
    if (newPos2 < DstPos) { newOkEdges++; } }
  for (int e = 0; e < NI.GetInDeg(); e++) {
    const int DstPos = NIdPosH.GetDat(NI.GetInNId(e));
    if (DstPos < newPos1) { oldOkEdges++; }
    if (DstPos < newPos2) { newOkEdges++; } }
  }
  if (Graph->IsEdge(NId1, NId2) || Graph->IsEdge(NId2, NId1)) { oldOkEdges--; }
  return newOkEdges - oldOkEdges;
}

int GetGreedyMxDag(const PNGraph& Graph, const int& NIters) {
  TIntV NIdOrdeV;
  return GetGreedyMxDag(Graph, NIters, NIdOrdeV);
}

// largest acyclic subgraph problem (randomized greedy algorithm)
int GetGreedyMxDag(const PNGraph& Graph, const int& NIters, TIntV& NIdOrderV) {
  const int Edges = Graph->GetEdges();
  TIntH NIdPosH;
  TIntV NIdV;
  Graph->GetNIdV(NIdV);  NIdV.Shuffle(TInt::Rnd);
  for (int i = 0; i < NIdV.Len(); i++) {
    NIdPosH.AddDat(NIdV[i], i);
  }
  int Score = EvalMxDagOrdering(Graph, NIdPosH), GoodMoves=0;
  for (int i = 1; i <= NIters; i++) {
    const int RndNId1 = NIdPosH.GetKey(TInt::Rnd.GetUniDevInt(NIdPosH.Len()));
    const int RndNId2 = NIdPosH.GetKey(TInt::Rnd.GetUniDevInt(NIdPosH.Len()));
    if (RndNId1 == RndNId2) { i--; continue; }
    const int Delta = EvalMxDagNodeSwap(Graph, NIdPosH, RndNId1, RndNId2);
    if (Delta > 0) {
      int tmp=NIdPosH.GetDat(RndNId2);
      NIdPosH.GetDat(RndNId2) = NIdPosH.GetDat(RndNId1);
      NIdPosH.GetDat(RndNId1) = tmp;
      Score += Delta;  GoodMoves++;
    }
    if (i==1 || i % Kilo(100) == 0) {
      //printf("  Score: %6d (%d) vs %6d = %.4f\tgood %d\n", Score, 
      //  EvalMxDagOrdering(Graph, NIdPosH), Edges-Score, Score/double(Edges), GoodMoves);
    }
  }
  NIdOrderV.Gen(NIdPosH.Len());
  for (int i = 0; i < NIdPosH.Len(); i++) {
    IAssert(NIdPosH[i] < NIdOrderV.Len());
    NIdOrderV[NIdPosH[i]] = NIdPosH.GetKey(i);
  }
  return EvalMxDagOrdering(Graph, NIdPosH);
}

} // namespace TSnap
