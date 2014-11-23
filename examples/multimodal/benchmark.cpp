#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

PSOut StdOut = TStdOut::New();

int main(int argc, char* argv[]) {
  TMVNet Graph = TMVNet();
  Graph.AddNType(TStr("Photo"));
  Graph.AddNType(TStr("User"));
  StdOut->PutStrFmtLn("TypeId %d - %d - %d", Graph.AddNType(TStr("User")), Graph.GetNTypeId(TStr("Photo")), Graph.GetNTypeId(TStr("User")));
  int NId1 = Graph.AddNode(1, 0);
  int NId2 = Graph.AddNode(1, 1);
  int NId3 = Graph.AddNode(0, 1);
  int NId4 = Graph.AddNode(1, 2);
  int NId5 = Graph.AddNode(0, 0);
  StdOut->PutStrFmtLn("Nodes %d %d %d", NId1, NId2, NId3);
  StdOut->PutStrFmtLn("Size = %d", Graph.GetNodes());
  for (TMVNet::TNodeI Iter = Graph.BegNI(); Iter < Graph.EndNI(); Iter++) {
    StdOut->PutStrFmtLn("Iter = %d, %d", Iter.GetTypeId(), Iter.GetId());
  }

  int EId1 = Graph.AddEdge(NId1, NId2, 0);
  StdOut->PutStrFmtLn("Edges %d", EId1);
  StdOut->PutStrFmtLn("IsEdge(%d) = %d", EId1, Graph.IsEdge(EId1));
  StdOut->PutStrFmtLn("IsEdge(%d) = %d", 1, Graph.IsEdge(1));
  int EId;
  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d", NId1, NId2, Graph.IsEdge(NId1, NId2, true));
  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d - %d", NId1, NId2, Graph.IsEdge(NId1, NId2, EId, true), EId);
  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d", NId1, NId3, Graph.IsEdge(NId1, NId3, true));
  StdOut->PutStrFmtLn("IsEdge(%d, %d) = %d - %d", NId1, NId3, Graph.IsEdge(NId1, NId3, EId, true), EId);

  int EId2 = Graph.AddEdge(NId1, NId2, 0);
  int EId3 = Graph.AddEdge(NId1, NId3, 1);
  int EId4 = Graph.AddEdge(NId1, NId4, 1, 10);
  int EId5 = Graph.AddEdge(NId1, NId5, 0);
  for (TMVNet::TEdgeI Iter = Graph.BegEI(); Iter < Graph.EndEI(); Iter++) {
    StdOut->PutStrFmtLn("Edge Iter = %d, %d", Iter.GetTypeId(), Iter.GetId());
  }
  StdOut->PutStrFmtLn("Edge Size = %d", Graph.GetEdges());
}
