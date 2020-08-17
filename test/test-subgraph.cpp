#include <gtest/gtest.h>

#include "Snap.h"

PUNGraph GetTestTUNGraph();
PNGraph GetTestTNGraph();
PNEGraph GetTestTNEGraph();
PNEANet GetTestTNEANet();
TPt <TNodeEDatNet<TInt, TInt> > GetTestTNodeEDatNet();
TPt <TNodeEdgeNet<TInt, TInt> > GetTestTNodeEdgeNet();

// Test subgraphs
TEST(subgraph, TestSubTUNGraphs) {
  PUNGraph Graph;
  PUNGraph Graph1;
  PUNGraph Graph2;
  PUNGraph Graph3;
  PUNGraph Graph4;
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

  Graph4 = TSnap::GetSubGraphRenumber(Graph, NIdV1);
  EXPECT_EQ(10,Graph4->GetNodes());
  EXPECT_EQ(10,Graph4->GetEdges());
}

// Test subgraphs
TEST(subgraph, TestSubTNGraphs) {
  PNGraph Graph;
  PNGraph Graph1;
  PNGraph Graph2;
  PNGraph Graph3;
  PNGraph Graph4;
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

  Graph4 = TSnap::GetSubGraphRenumber(Graph, NIdV1);
  EXPECT_EQ(10,Graph4->GetNodes());
  EXPECT_EQ(10,Graph4->GetEdges());
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

// Test TNGraph GetEgoNetHop
TEST(subgraph, TestEgoNetsTNGraph)
{
  PNGraph Graph;
  PNGraph Graph0;
  PNGraph Graph1;
  PNGraph Graph2;
  PNGraph Graph3;
  PNGraph Graph4;
  PNGraph Graph5;
  PNGraph Graph6;
  PNGraph Graph7;

  // Test on TNSmall Graph
  PNGraph Small = TNGraph::GetSmallGraph();
  PNGraph ego1 = TSnap::GetEgonetHop(Small, 4, 1);
  EXPECT_EQ(2, ego1->GetNodes());
  EXPECT_EQ(1, ego1->GetEdges());
  PNGraph ego2 = TSnap::GetEgonetHop(Small, 4, 2);
  EXPECT_EQ(4, ego2->GetNodes());
  EXPECT_EQ(4, ego2->GetEdges());
  PNGraph ego3 = TSnap::GetEgonetHop(Small, 4, 3);
  EXPECT_EQ(5, ego1->GetNodes());
  EXPECT_EQ(6, ego1->GetEdges());

  Graph = GetTestTNGraph();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetEgonetHop(Graph, 0, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetEgonetHop(Graph, 3, 1);
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_EQ(3, Graph1->GetEdges());

  Graph2 = TSnap::GetEgonetHop(Graph, 6, 2);
  EXPECT_EQ(7, Graph2->GetNodes());
  EXPECT_EQ(12, Graph2->GetEdges());

  Graph3 = TSnap::GetEgonetHop(Graph, 9, 3);
  EXPECT_EQ(10, Graph3->GetNodes());
  EXPECT_EQ(21, Graph3->GetEdges());

  Graph4 = TSnap::GetEgonetHop(Graph, 12, 4);
  EXPECT_EQ(13, Graph4->GetNodes());
  EXPECT_EQ(30, Graph4->GetEdges());

  Graph5 = TSnap::GetEgonetHop(Graph, 15, 5);
  EXPECT_EQ(16, Graph5->GetNodes());
  EXPECT_EQ(39, Graph5->GetEdges());

  Graph6 = TSnap::GetEgonetHop(Graph, 18, 6);
  EXPECT_EQ(19, Graph6->GetNodes());
  EXPECT_EQ(48, Graph6->GetEdges());

  Graph7 = TSnap::GetEgonetHop(Graph, 1, 7);
  EXPECT_EQ(20, Graph7->GetNodes());
  EXPECT_EQ(57, Graph7->GetEdges());
}

// Test TUNGraph GetEgoNetHop
TEST(subgraph, TestEgoNetsTUNGraph)
{
  PUNGraph Graph;
  PUNGraph Graph0;
  PUNGraph Graph1;
  PUNGraph Graph2;
  PUNGraph Graph3;
  PUNGraph Graph4;

  Graph = GetTestTUNGraph();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetEgonetHop(Graph, 3, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetEgonetHop(Graph, 0, 1);
  EXPECT_EQ(7, Graph1->GetNodes());
  EXPECT_EQ(15, Graph1->GetEdges());

  Graph2 = TSnap::GetEgonetHop(Graph, 7, 2);
  EXPECT_EQ(13, Graph2->GetNodes());
  EXPECT_EQ(33, Graph2->GetEdges());

  Graph3 = TSnap::GetEgonetHop(Graph, 19, 3);
  EXPECT_EQ(19, Graph3->GetNodes());
  EXPECT_EQ(54, Graph3->GetEdges());

  Graph4 = TSnap::GetEgonetHop(Graph, 8, 4);
  EXPECT_EQ(20, Graph4->GetNodes());
  EXPECT_EQ(60, Graph4->GetEdges());

}

// Test TNEANet GetEgoNetHop
TEST(subgraph, TestEgoNetsTNEANet)
{
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;
  PNEANet Graph3;
  PNEANet Graph4;
  PNEANet Graph5;
  PNEANet Graph6;
  PNEANet Graph7;
  TIntV NIds;
  const TStr s = "id";

  Graph = GetTestTNEANet();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetEgonetHop(Graph, 0, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());
  Graph0->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph0->GetIntAttrDatN(NIds[i], s));
  }

  Graph1 = TSnap::GetEgonetHop(Graph, 3, 1);
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_EQ(6, Graph1->GetEdges());
  NIds.Clr();
  Graph1->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph1->GetIntAttrDatN(NIds[i], s));
  }

  Graph2 = TSnap::GetEgonetHop(Graph, 6, 2);
  EXPECT_EQ(7, Graph2->GetNodes());
  EXPECT_EQ(15, Graph2->GetEdges());
  NIds.Clr();
  Graph2->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph2->GetIntAttrDatN(NIds[i], s));
  }

  Graph3 = TSnap::GetEgonetHop(Graph, 9, 3);
  EXPECT_EQ(10, Graph3->GetNodes());
  EXPECT_EQ(24, Graph3->GetEdges());
  NIds.Clr();
  Graph3->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph3->GetIntAttrDatN(NIds[i], s));
  }

  Graph4 = TSnap::GetEgonetHop(Graph, 12, 4);
  EXPECT_EQ(13, Graph4->GetNodes());
  EXPECT_EQ(33, Graph4->GetEdges());
  NIds.Clr();
  Graph4->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph4->GetIntAttrDatN(NIds[i], s));
  }

  Graph5 = TSnap::GetEgonetHop(Graph, 15, 5);
  EXPECT_EQ(16, Graph5->GetNodes());
  EXPECT_EQ(42, Graph5->GetEdges());
  NIds.Clr();
  Graph5->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph5->GetIntAttrDatN(NIds[i], s));
  }

  Graph6 = TSnap::GetEgonetHop(Graph, 18, 6);
  EXPECT_EQ(19, Graph6->GetNodes());
  EXPECT_EQ(54, Graph6->GetEdges());
  NIds.Clr();
  Graph6->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph6->GetIntAttrDatN(NIds[i], s));
  }

  Graph7 = TSnap::GetEgonetHop(Graph, 1, 7);
  EXPECT_EQ(20, Graph7->GetNodes());
  EXPECT_EQ(60, Graph7->GetEdges());
  NIds.Clr();
  Graph7->GetNIdV(NIds);
  for (int i = 0; i < NIds.Len(); i++)
  {
    EXPECT_EQ(NIds[i], Graph7->GetIntAttrDatN(NIds[i], s));
  }
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

// Generate TNEANet
PNEANet GetTestTNEANet()
{
  PNEANet Graph = PNEANet::New();
  TStr s = "id";

  for (int i = 0; i < 20; i++)
  {
    Graph->AddNode(i);
    Graph->AddIntAttrDatN(i, i, s);
  }

  for (int i = 0; i < 20; i++)
  {
    Graph->AddEdge(i, (i + 1) % 20);
    Graph->AddEdge(i, (i + 2) % 20);
    Graph->AddEdge(i, (i + 3) % 20);
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

