#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

PSOut StdOut = TStdOut::New();

int main(int argc, char* argv[]) {
//  PSVNet PGraph = PSVNet::New();
//  PCVNet PGraph = PCVNet::New();
  PMVNet PGraph = PMVNet::New();
  int NTypeId1 = PGraph->AddNType(TStr("Photo"));
  int NTypeId2 = PGraph->AddNType(TStr("User"));
  int NTypeId3 = PGraph->AddNType(TStr("Tag"));
  int ETypeId1 = PGraph->AddEType(TStr("EType1"), TStr("Photo"), TStr("User"));
  int ETypeId2 = PGraph->AddEType(TStr("EType2"), TStr("User"), TStr("Photo"));
  int ETypeId3 = PGraph->AddEType(TStr("EType3"), TStr("User"), TStr("Photo"));
  int ETypeId4 = PGraph->AddEType(TStr("EType4"), TStr("Photo"), TStr("Tag"));
  StdOut->PutStrFmtLn("EdgeTypeId %d - %d - %d", ETypeId1, ETypeId2, ETypeId4);
  StdOut->PutStrFmtLn("TypeId %d - %d - %d", PGraph->AddNType(TStr("User")), PGraph->GetNTypeId(TStr("Photo")), PGraph->GetNTypeId(TStr("User")));
  int NId1 = PGraph->AddNode(1, 0);
  int NId2 = PGraph->AddNode(1, 1);
  int NId3 = PGraph->AddNode(0, 1);
  int NId4 = PGraph->AddNode(1, 2);
  int NId5 = PGraph->AddNode(0, 0);
  int NId6 = PGraph->AddNode(2, 1);
  StdOut->PutStrFmtLn("Nodes %d %d %d %d %d %d", NId1, NId2, NId3, NId4, NId5, NId6);
  StdOut->PutStrFmtLn("Size = %d", PGraph->GetNodes());
  for (TMVNet::TNodeI Iter = PGraph->BegNI(); Iter < PGraph->EndNI(); Iter++) {
    StdOut->PutStrFmtLn("Iter = %d, %d", Iter.GetTypeId(), Iter.GetId());
  }


//  StdOut->PutStrFmtLn("Edges %d", EId1);
//  StdOut->PutStrFmtLn("IsEdge(%d) = %d", EId1, PGraph->IsEdge(EId1));
//  StdOut->PutStrFmtLn("IsEdge(%d) = %d", 1, PGraph->IsEdge(1));
//  int EId;
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d", NId1, NId2, PGraph->IsEdge(NId1, NId2, true));
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d - %d", NId1, NId2, PGraph->IsEdge(NId1, NId2, EId, true), EId);
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d", NId1, NId3, PGraph->IsEdge(NId1, NId3, true));
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d - %d", NId1, NId3, PGraph->IsEdge(NId1, NId3, EId, true), EId);

  int EId1 = PGraph->AddEdge(NId1, NId3, ETypeId2);
  int EId2 = PGraph->AddEdge(NId3, NId2, ETypeId1);
  int EId3 = PGraph->AddEdge(NId5, NId2, ETypeId1);
  int EId4 = PGraph->AddEdge(NId1, NId5, ETypeId2, 10);
  int EId5 = PGraph->AddEdge(NId3, NId1, ETypeId1);
  int EId6 = PGraph->AddEdge(NId3, NId6, ETypeId4);
  int EId7 = PGraph->AddEdge(NId3, NId6, ETypeId4);
  for (TMVNet::TEdgeI Iter = PGraph->BegEI(); Iter < PGraph->EndEI(); Iter++) {
    StdOut->PutStrFmtLn("Edge = %d:%d", Iter.GetTypeId(), Iter.GetId());
  }
  StdOut->PutStrFmtLn("Edge Size = %d", PGraph->GetEdges());
  StdOut->PutStrFmtLn("Degree %d-%d", PGraph->GetNI(NId3).GetInDeg(), PGraph->GetNI(NId3).GetOutDeg());
  for (int i = 0; i < PGraph->GetNI(NId3).GetOutDeg(); i++) {
    StdOut->PutStrFmtLn("Edge %d", PGraph->GetNI(NId3).GetOutEId(i));
  }

  TStrV NTypeNameV;
  NTypeNameV.Add(TStr("Photo"));
  NTypeNameV.Add(TStr("User"));
  PMVNet PSubGraph = PGraph->GetSubGraph(NTypeNameV);
  StdOut->PutStrFmtLn("Size = %d", PSubGraph->GetNodes());
  StdOut->PutStrFmtLn("Edge Size = %d", PSubGraph->GetEdges());
  for (TMVNet::TEdgeI Iter = PSubGraph->BegEI(); Iter < PSubGraph->EndEI(); Iter++) {
    StdOut->PutStrFmtLn("Edge = %d:%d", Iter.GetTypeId(), Iter.GetId());
  }

  PGraph->DelEdge(EId5);
  PGraph->DelEdge(EId3);
  StdOut->PutStrFmtLn("Edge Size = %d", PGraph->GetEdges());
  PGraph->DelEdge(NId3, NId6, true);
  for (TSVNet::TEdgeI Iter = PGraph->BegEI(); Iter < PGraph->EndEI(); Iter++) {
    StdOut->PutStrFmtLn("Edge = %d:%d", Iter.GetTypeId(), Iter.GetId());
  }
  StdOut->PutStrFmtLn("Edge Size = %d", PGraph->GetEdges());

  PGraph->DelNode(NId2);
  StdOut->PutStrFmtLn("Size = %d", PGraph->GetNodes());
  for (TSVNet::TNodeI Iter = PGraph->BegNI(); Iter < PGraph->EndNI(); Iter++) {
    StdOut->PutStrFmtLn("Node = %d:%d", Iter.GetTypeId(), Iter.GetId());
  }
  for (TSVNet::TEdgeI Iter = PGraph->BegEI(); Iter < PGraph->EndEI(); Iter++) {
    StdOut->PutStrFmtLn("Edge = %d:%d", Iter.GetTypeId(), Iter.GetId());
  }
  StdOut->PutStrFmtLn("Edge Size = %d", PGraph->GetEdges());

  TIntV ShortestDists;
  StdOut->PutStrFmtLn("Depth = %d", TSnap::GetShortestDistances(PGraph, NId1, false, false, ShortestDists));
  for (TSVNet::TNodeI Iter = PGraph->BegNI(); Iter < PGraph->EndNI(); Iter++) {
    int NId = Iter.GetId();
    StdOut->PutStrFmtLn("Shortest Dist = %d:%d", NId, ShortestDists[NId]);
  }

  TIntFltH PageRankH;
  TSnap::GetPageRankMNetMP(PGraph, PageRankH, 0.849999999999998, 0.0001, 10);
}
