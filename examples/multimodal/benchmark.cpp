#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

PSOut StdOut = TStdOut::New();

int main(int argc, char* argv[]) {
  TCVNet Graph = TCVNet();
  Graph.AddNType(TStr("Photo"));
  Graph.AddNType(TStr("User"));
  Graph.AddNType(TStr("Tag"));
  int ETypeId1 = Graph.AddEType(TStr("EType1"), TStr("Photo"), TStr("User"));
  int ETypeId2 = Graph.AddEType(TStr("EType2"), TStr("User"), TStr("Photo"));
  int ETypeId3 = Graph.AddEType(TStr("EType3"), TStr("User"), TStr("Photo"));
  int ETypeId4 = Graph.AddEType(TStr("EType4"), TStr("Photo"), TStr("Tag"));
  StdOut->PutStrFmtLn("EdgeTypeId %d - %d - %d", ETypeId1, ETypeId2, ETypeId4);
  StdOut->PutStrFmtLn("TypeId %d - %d - %d", Graph.AddNType(TStr("User")), Graph.GetNTypeId(TStr("Photo")), Graph.GetNTypeId(TStr("User")));
  int NId1 = Graph.AddNode(1, 0);
  int NId2 = Graph.AddNode(1, 1);
  int NId3 = Graph.AddNode(0, 1);
  int NId4 = Graph.AddNode(1, 2);
  int NId5 = Graph.AddNode(0, 0);
  int NId6 = Graph.AddNode(2, 1);
  StdOut->PutStrFmtLn("Nodes %d %d %d", NId1, NId2, NId3);
  StdOut->PutStrFmtLn("Size = %d", Graph.GetNodes());
  for (TCVNet::TNodeI Iter = Graph.BegNI(); Iter < Graph.EndNI(); Iter++) {
    StdOut->PutStrFmtLn("Iter = %d, %d", Iter.GetTypeId(), Iter.GetId());
  }


//  StdOut->PutStrFmtLn("Edges %d", EId1);
//  StdOut->PutStrFmtLn("IsEdge(%d) = %d", EId1, Graph.IsEdge(EId1));
//  StdOut->PutStrFmtLn("IsEdge(%d) = %d", 1, Graph.IsEdge(1));
//  int EId;
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d", NId1, NId2, Graph.IsEdge(NId1, NId2, true));
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d - %d", NId1, NId2, Graph.IsEdge(NId1, NId2, EId, true), EId);
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d", NId1, NId3, Graph.IsEdge(NId1, NId3, true));
//  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d - %d", NId1, NId3, Graph.IsEdge(NId1, NId3, EId, true), EId);

  int EId1 = Graph.AddEdge(NId1, NId3, ETypeId2);
  int EId2 = Graph.AddEdge(NId3, NId2, ETypeId1);
  int EId3 = Graph.AddEdge(NId5, NId2, ETypeId1);
  int EId4 = Graph.AddEdge(NId1, NId5, ETypeId2, 10);
  int EId5 = Graph.AddEdge(NId3, NId1, ETypeId1);
  int EId6 = Graph.AddEdge(NId3, NId6, ETypeId4);
  int EId7 = Graph.AddEdge(NId3, NId6, ETypeId4);
  for (TCVNet::TEdgeI Iter = Graph.BegEI(); Iter < Graph.EndEI(); Iter++) {
    StdOut->PutStrFmtLn("Edge Iter = %d, %d", Iter.GetTypeId(), Iter.GetId());
  }
  StdOut->PutStrFmtLn("Edge Size = %d", Graph.GetEdges());
  StdOut->PutStrFmtLn("Degree %d-%d", Graph.GetNI(NId3).GetInDeg(), Graph.GetNI(NId3).GetOutDeg());
  for (int i = 0; i < Graph.GetNI(NId3).GetOutDeg(); i++) {
    StdOut->PutStrFmtLn("Edge %d", Graph.GetNI(NId3).GetOutEId(i));
  }
}
