#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TNEGraph, DefaultConstructor) {
  PNEGraph Graph;

  Graph = TNEGraph::New();

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());
  EXPECT_EQ(1,Graph->HasFlag(gfDirected));
}

// Test node, edge creation
TEST(TNEGraph, ManipulateNodesEdges) {
  int NNodes = 1000;
  int NEdges = 100000;
  const char *FName = "test.graph.dat";

  PNEGraph Graph;
  PNEGraph Graph1;
  PNEGraph Graph2;
  int i;
  int n;
  int NCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Graph = TNEGraph::New();
  EXPECT_EQ(1,Graph->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(NNodes,Graph->GetNodes());

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    n = Graph->AddEdge(x, y);
    NCount--;
  }

  EXPECT_EQ(NEdges,Graph->GetEdges());

  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(1,Graph->IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Graph->IsNode(i));
  }

  EXPECT_EQ(0,Graph->IsNode(NNodes));
  EXPECT_EQ(0,Graph->IsNode(NNodes+1));
  EXPECT_EQ(0,Graph->IsNode(2*NNodes));

  // nodes iterator
  NCount = 0;
  for (TNEGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator
  NCount = 0;
  for (TNEGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges,NCount);

  // edges iterator
  NCount = 0;
  for (TNEGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TNEGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg+OutDeg);
  }

  // assignment
  Graph1 = TNEGraph::New();
  *Graph1 = *Graph;

  EXPECT_EQ(NNodes,Graph1->GetNodes());
  EXPECT_EQ(NEdges,Graph1->GetEdges());
  EXPECT_EQ(0,Graph1->Empty());
  EXPECT_EQ(1,Graph1->IsOk());

  // saving and loading
  {
    TFOut FOut(FName);
    Graph->Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    Graph2 = TNEGraph::Load(FIn);
  }

  EXPECT_EQ(NNodes,Graph2->GetNodes());
  EXPECT_EQ(NEdges,Graph2->GetEdges());
  EXPECT_EQ(0,Graph2->Empty());
  EXPECT_EQ(1,Graph2->IsOk());

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Graph->GetRndNId();
    Graph->DelNode(n);
  }

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());

  Graph1->Clr();

  EXPECT_EQ(0,Graph1->GetNodes());
  EXPECT_EQ(0,Graph1->GetEdges());

  EXPECT_EQ(1,Graph1->IsOk());
  EXPECT_EQ(1,Graph1->Empty());
}

// Test small graph
TEST(TNEGraph, GetSmallGraph) {
  PNEGraph Graph;

  return;

  // not implemented
  //Graph = TNEGraph::GetSmallGraph();

  EXPECT_EQ(5,Graph->GetNodes());
  EXPECT_EQ(6,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(1,Graph->HasFlag(gfDirected));
}

