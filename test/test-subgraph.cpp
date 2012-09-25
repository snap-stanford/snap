#include <gtest/gtest.h>

#include "Snap.h"

PUNGraph GetTestTUNGraph();
PNGraph GetTestTNGraph();
PNEGraph GetTestTNEGraph();
TPt <TNodeEDatNet<TInt, TInt> > GetTestTNodeEDatNet();
TPt <TNodeEdgeNet<TInt, TInt> > GetTestTNodeEdgeNet();

// Test subgraphs
TEST(subgraph, TestSubTUNGraphs) {
  PUNGraph Graph;
  PUNGraph Graph1;
  PUNGraph Graph2;
  PUNGraph Graph3;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Graph = GetTestTUNGraph();
  EXPECT_EQ(20,Graph->GetNodes());
  EXPECT_EQ(60,Graph->GetEdges());

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Graph1 = TSnap::GetSubGraph(Graph, NIdV);
  EXPECT_EQ(5,Graph1->GetNodes());
  EXPECT_EQ(9,Graph1->GetEdges());

  Graph2 = TSnap::GetSubGraph(Graph, NIdV, true);
  EXPECT_EQ(5,Graph2->GetNodes());
  EXPECT_EQ(9,Graph2->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Graph3 = TSnap::GetSubGraph(Graph, NIdV1, true);
  EXPECT_EQ(10,Graph3->GetNodes());
  EXPECT_EQ(10,Graph3->GetEdges());
}

// Test subgraphs
TEST(subgraph, TestSubTNGraphs) {
  PNGraph Graph;
  PNGraph Graph1;
  PNGraph Graph2;
  PNGraph Graph3;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Graph = GetTestTNGraph();
  EXPECT_EQ(20,Graph->GetNodes());
  EXPECT_EQ(60,Graph->GetEdges());

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Graph1 = TSnap::GetSubGraph(Graph, NIdV);
  EXPECT_EQ(5,Graph1->GetNodes());
  EXPECT_EQ(9,Graph1->GetEdges());

  Graph2 = TSnap::GetSubGraph(Graph, NIdV, true);
  EXPECT_EQ(5,Graph2->GetNodes());
  EXPECT_EQ(9,Graph2->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Graph3 = TSnap::GetSubGraph(Graph, NIdV1, true);
  EXPECT_EQ(10,Graph3->GetNodes());
  EXPECT_EQ(10,Graph3->GetEdges());
}

// Test subgraphs
TEST(subgraph, TestSubTNEGraphs) {
  PNEGraph Graph;
  PNEGraph Graph1;
  PNEGraph Graph2;
  PNEGraph Graph3;
  int i;
  TIntV NIdV;
  TIntV NIdV1;
  TIntV EIdV;

  Graph = GetTestTNEGraph();
  EXPECT_EQ(20,Graph->GetNodes());
  EXPECT_EQ(120,Graph->GetEdges());

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Graph1 = TSnap::GetSubGraph(Graph, NIdV);
  EXPECT_EQ(5,Graph1->GetNodes());
  EXPECT_EQ(18,Graph1->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Graph2 = TSnap::GetSubGraph(Graph, NIdV1);
  EXPECT_EQ(10,Graph2->GetNodes());
  EXPECT_EQ(20,Graph2->GetEdges());

  for (i = 0; i < 120; i += 2) {
    EIdV.Add(i);
  }

  Graph3 = TSnap::GetESubGraph(Graph, EIdV);
  EXPECT_EQ(20,Graph3->GetNodes());
  EXPECT_EQ(60,Graph3->GetEdges());
}

// Test node subgraphs
TEST(subgraph, TestEDatSubNets) {
  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  TPt <TNodeEDatNet<TInt, TInt> > Net3;
  TPt <TNodeEDatNet<TInt, TInt> > Net4;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Net = GetTestTNodeEDatNet();
  EXPECT_EQ(20,Net->GetNodes());
  EXPECT_EQ(60,Net->GetEdges());

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Net1 = TSnap::GetSubGraph(Net, NIdV);
  EXPECT_EQ(5,Net1->GetNodes());
  EXPECT_EQ(9,Net1->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Net2 = TSnap::GetSubGraph(Net, NIdV1);
  EXPECT_EQ(10,Net2->GetNodes());
  EXPECT_EQ(10,Net2->GetEdges());

  Net3 = TSnap::GetEDatSubGraph(Net, 1, 0);
  EXPECT_EQ(20,Net3->GetNodes());
  EXPECT_EQ(15,Net3->GetEdges());

  Net4 = TSnap::GetEDatSubGraph(Net, 2, -1);
  EXPECT_EQ(20,Net4->GetNodes());
  EXPECT_EQ(30,Net4->GetEdges());
}

// Test edge subgraphs
TEST(subgraph, TestEdgeSubNets) {
  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  TPt <TNodeEdgeNet<TInt, TInt> > Net3;
  TPt <TNodeEdgeNet<TInt, TInt> > Net4;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Net = GetTestTNodeEdgeNet();
  EXPECT_EQ(20,Net->GetNodes());
  EXPECT_EQ(120,Net->GetEdges());

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Net1 = TSnap::GetSubGraph(Net, NIdV);
  EXPECT_EQ(5,Net1->GetNodes());
  EXPECT_EQ(18,Net1->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Net2 = TSnap::GetSubGraph(Net, NIdV1);
  EXPECT_EQ(10,Net2->GetNodes());
  EXPECT_EQ(20,Net2->GetEdges());

  Net3 = TSnap::GetEDatSubGraph(Net, 1, 0);
  EXPECT_EQ(20,Net3->GetNodes());
  EXPECT_EQ(30,Net3->GetEdges());

  Net4 = TSnap::GetEDatSubGraph(Net, 2, -1);
  EXPECT_EQ(20,Net4->GetNodes());
  EXPECT_EQ(60,Net4->GetEdges());
}

// Test graph conversion
TEST(subgraph, TestConvertGraphs) {
  PNGraph NGraph;
  PUNGraph UNGraph;

  NGraph = GetTestTNGraph();
  EXPECT_EQ(20,NGraph->GetNodes());
  EXPECT_EQ(60,NGraph->GetEdges());

  UNGraph = TSnap::ConvertGraph<PUNGraph>(NGraph);
  EXPECT_EQ(20,UNGraph->GetNodes());
  EXPECT_EQ(60,UNGraph->GetEdges());

  NGraph = TSnap::ConvertGraph<PNGraph>(UNGraph);
  EXPECT_EQ(20,NGraph->GetNodes());
  EXPECT_EQ(120,NGraph->GetEdges());
}

// Test node subgraph conversion
TEST(subgraph, TestConvertSubGraphs) {
  PNGraph NGraph;
  PUNGraph UNGraph;
  TIntV NIdV;
  int i;

  NGraph = GetTestTNGraph();
  EXPECT_EQ(20,NGraph->GetNodes());
  EXPECT_EQ(60,NGraph->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV.Add(i);
  }

  // TODO: fix TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true), it fails
  // UNGraph = TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true);
  UNGraph = TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV);
  EXPECT_EQ(10,UNGraph->GetNodes());
  EXPECT_EQ(10,UNGraph->GetEdges());

  NGraph = TSnap::ConvertSubGraph<PNGraph>(UNGraph, NIdV);
  EXPECT_EQ(10,NGraph->GetNodes());
  EXPECT_EQ(20,NGraph->GetEdges());
}

// Test edge subgraph conversion
TEST(subgraph, TestConvertESubGraphs) {
  PNEGraph NEGraph;
  PNGraph NGraph;
  TIntV NIdV;
  TIntV EIdV;
  int i;

  NGraph = GetTestTNGraph();
  EXPECT_EQ(20,NGraph->GetNodes());
  EXPECT_EQ(60,NGraph->GetEdges());

  for (i = 0; i < 20; i += 2) {
    NIdV.Add(i);
  }

  // TODO: fix TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true), it fails
  // UNGraph = TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true);
  NEGraph = TSnap::ConvertGraph<PNEGraph>(NGraph);
  EXPECT_EQ(20,NEGraph->GetNodes());
  EXPECT_EQ(60,NEGraph->GetEdges());

  // select every second edge
  i = 0;
  for (TNEGraph::TEdgeI EI = NEGraph->BegEI(); EI < NEGraph->EndEI(); EI++) {
    if (i == 0) {
      EIdV.Add(EI.GetId());
    }
    i = (i + 1) % 2;
  }

  NGraph = TSnap::ConvertESubGraph<PNGraph>(NEGraph, EIdV);
  EXPECT_EQ(20,NGraph->GetNodes());
  EXPECT_EQ(30,NGraph->GetEdges());
}

// Test get random graphs
TEST(subgraph, TestGetRndGraphs) {
  PUNGraph UNGraph1, UNGraph2, UNGraph3;
  PNGraph NGraph1, NGraph2, NGraph3;
  PNEGraph NEGraph1, NEGraph2, NEGraph3;

  UNGraph1 = GetTestTUNGraph();
  EXPECT_EQ(20,UNGraph1->GetNodes());
  EXPECT_EQ(60,UNGraph1->GetEdges());

  UNGraph2 = TSnap::GetRndSubGraph(UNGraph1,10);
  EXPECT_EQ(10,UNGraph2->GetNodes());

  UNGraph3 = TSnap::GetRndESubGraph(UNGraph1,10);
  EXPECT_EQ(10,UNGraph3->GetEdges());

  NGraph1 = GetTestTNGraph();
  EXPECT_EQ(20,NGraph1->GetNodes());
  EXPECT_EQ(60,NGraph1->GetEdges());

  NGraph2 = TSnap::GetRndSubGraph(NGraph1,10);
  EXPECT_EQ(10,NGraph2->GetNodes());

  NGraph3 = TSnap::GetRndESubGraph(NGraph1,10);
  EXPECT_EQ(10,NGraph3->GetEdges());
}

// Generate TUNGraph
PUNGraph GetTestTUNGraph() {
  PUNGraph Graph = TUNGraph::New();

  for (int i = 0; i < 20; i++) {
    Graph->AddNode(i);
  }

  for (int i = 0; i < 20; i++) {
    Graph->AddEdge(i,(i+1) % 20);
    Graph->AddEdge(i,(i+2) % 20);
    Graph->AddEdge(i,(i+3) % 20);
  }

  return Graph;
}

// Generate TNGraph
PNGraph GetTestTNGraph() {
  PNGraph Graph = TNGraph::New();

  for (int i = 0; i < 20; i++) {
    Graph->AddNode(i);
  }

  for (int i = 0; i < 20; i++) {
    Graph->AddEdge(i,(i+1) % 20);
    Graph->AddEdge(i,(i+2) % 20);
    Graph->AddEdge(i,(i+3) % 20);
  }

  return Graph;
}

// Generate TNEGraph
PNEGraph GetTestTNEGraph() {
  PNEGraph Graph = TNEGraph::New();

  for (int i = 0; i < 20; i++) {
    Graph->AddNode(i);
  }

  for (int i = 0; i < 20; i++) {
    Graph->AddEdge(i,(i+1) % 20);
    Graph->AddEdge(i,(i+2) % 20);
    Graph->AddEdge(i,(i+3) % 20);
    Graph->AddEdge(i,(i+1) % 20);
    Graph->AddEdge(i,(i+2) % 20);
    Graph->AddEdge(i,(i+3) % 20);
  }

  return Graph;
}

// Generate TNodeEDatNet
TPt <TNodeEDatNet<TInt, TInt> > GetTestTNodeEDatNet() {
  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int n;

  Net = TNodeEDatNet<TInt, TInt>::New();

  for (int i = 0; i < 20; i++) {
    Net->AddNode(i);
  }

  for (int i = 0; i < 20; i++) {
    Net->AddEdge(i,(i+1) % 20);
    Net->AddEdge(i,(i+2) % 20);
    Net->AddEdge(i,(i+3) % 20);
  }

  n = 0;
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Net->SetEDat(EI.GetSrcNId(),EI.GetDstNId(),n);
    n = (n+1) % 4;
  }

  return Net;
}

// Generate TNodeEdgeNet
TPt <TNodeEdgeNet<TInt, TInt> > GetTestTNodeEdgeNet() {
  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int n;

  Net = TNodeEdgeNet<TInt, TInt>::New();

  for (int i = 0; i < 20; i++) {
    Net->AddNode(i);
  }

  for (int i = 0; i < 20; i++) {
    Net->AddEdge(i,(i+1) % 20);
    Net->AddEdge(i,(i+2) % 20);
    Net->AddEdge(i,(i+3) % 20);
    Net->AddEdge(i,(i+1) % 20);
    Net->AddEdge(i,(i+2) % 20);
    Net->AddEdge(i,(i+3) % 20);
  }

  n = 0;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Net->SetEDat(EI.GetId(),n);
    n = (n+1) % 4;
  }

  return Net;
}

