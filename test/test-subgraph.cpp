#include <gtest/gtest.h>

#include "Snap.h"

PUNGraph GetTestTUNGraph();
PNGraph GetTestTNGraph();
PNEGraph GetTestTNEGraph();
PNEANet GetTestTNEANet();
PUNGraph GetUPetersen();
PNGraph GetDPetersen();
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

// Test GetEgonet
TEST(subgraph, TestGetEgonet) {

  PUNGraph UGraph = GetUPetersen();
  for (int i = 0; i < 10; i++) {
    int Edges;
    PUNGraph Ego = TSnap::GetEgonet(UGraph, i, Edges);
    EXPECT_EQ(4, Ego->GetNodes());
    EXPECT_EQ(3, Ego->GetEdges());
    EXPECT_EQ(6, Edges);
  }
  
  PNGraph Graph = GetDPetersen();
  for (int i = 0; i < 10; i++) {
    int EdgesIn, EdgesOut;
    PNGraph Ego = TSnap::GetEgonet(Graph, i, EdgesIn, EdgesOut);
    EXPECT_EQ(4, Ego->GetNodes());
    EXPECT_EQ(3, Ego->GetEdges());
    if (i < 5) {
      EXPECT_EQ(2, EdgesIn);
      EXPECT_EQ(4, EdgesOut);
    } else {
      EXPECT_EQ(4, EdgesIn);
      EXPECT_EQ(2, EdgesOut);
    }
  }
}

// Test TNGraph GetEgoNetHop
TEST(subgraph, TestGetInEgonetHopTNGraph) {
  // Test on TNSmall Graph
  PNGraph Small = TNGraph::GetSmallGraph();
  PNGraph ego1 = TSnap::GetInEgonetHop<PNGraph>(Small, 4, 1);
  EXPECT_EQ(2, ego1->GetNodes());
  EXPECT_EQ(1, ego1->GetEdges());
  PNGraph ego2 = TSnap::GetInEgonetHop<PNGraph>(Small, 4, 2);
  EXPECT_EQ(4, ego2->GetNodes());
  EXPECT_EQ(4, ego2->GetEdges());
  PNGraph ego3 = TSnap::GetInEgonetHop<PNGraph>(Small, 4, 3);
  EXPECT_EQ(5, ego3->GetNodes());
  EXPECT_EQ(6, ego3->GetEdges());

  // Middle Size TN graph
  PNGraph Middle = TNGraph::New();
  for (int i = 0; i < 6; i++) {
    Middle->AddNode(i);
  }
  for (int i = 0; i < 6; i++) {
    Middle->AddEdge(i, (i + 1) % 6);
    Middle->AddEdge(i, (i + 2) % 6);
  }
  EXPECT_EQ(12, Middle->GetEdges());
  PNGraph midego2 = TSnap::GetInEgonetHop<PNGraph>(Middle, 0, 2);
  EXPECT_EQ(5, midego2->GetNodes());
  EXPECT_EQ(8, midego2->GetEdges());
  PNGraph midego3 = TSnap::GetInEgonetHop<PNGraph>(Middle, 0, 3);
  EXPECT_EQ(6, midego3->GetNodes());
  EXPECT_EQ(12, midego3->GetEdges());

  // Larger TN Graph
  PNGraph Graph;
  PNGraph Graph0;
  PNGraph Graph1;
  PNGraph Graph2;
  PNGraph Graph3;
  PNGraph Graph4;
  PNGraph Graph5;
  PNGraph Graph6;
  PNGraph Graph7;

  Graph = GetTestTNGraph();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetHop<PNGraph>(Graph, 0, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetInEgonetHop<PNGraph>(Graph, 3, 1);
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_EQ(6, Graph1->GetEdges());

  Graph2 = TSnap::GetInEgonetHop<PNGraph>(Graph, 6, 2);
  EXPECT_EQ(7, Graph2->GetNodes());
  EXPECT_EQ(15, Graph2->GetEdges());

  Graph3 = TSnap::GetInEgonetHop<PNGraph>(Graph, 9, 3);
  EXPECT_EQ(10, Graph3->GetNodes());
  EXPECT_EQ(24, Graph3->GetEdges());

  Graph4 = TSnap::GetInEgonetHop<PNGraph>(Graph, 12, 4);
  EXPECT_EQ(13, Graph4->GetNodes());
  EXPECT_EQ(33, Graph4->GetEdges());

  Graph5 = TSnap::GetInEgonetHop<PNGraph>(Graph, 15, 5);
  EXPECT_EQ(16, Graph5->GetNodes());
  EXPECT_EQ(42, Graph5->GetEdges());

  Graph6 = TSnap::GetInEgonetHop<PNGraph>(Graph, 18, 6);
  EXPECT_EQ(19, Graph6->GetNodes());
  EXPECT_EQ(54, Graph6->GetEdges());

  Graph7 = TSnap::GetInEgonetHop<PNGraph>(Graph, 1, 7);
  EXPECT_EQ(20, Graph7->GetNodes());
  EXPECT_EQ(60, Graph7->GetEdges());
}

// Test TUNGraph GetInEgonetHop
TEST(subgraph, TestGetInEgonetHopTUNGraph) {
  PUNGraph Graph;
  PUNGraph Graph0;
  PUNGraph Graph1;
  PUNGraph Graph2;
  PUNGraph Graph3;
  PUNGraph Graph4;

  Graph = GetTestTUNGraph();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 3, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 0, 1);
  EXPECT_EQ(7, Graph1->GetNodes());
  EXPECT_EQ(15, Graph1->GetEdges());

  Graph2 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 7, 2);
  EXPECT_EQ(13, Graph2->GetNodes());
  EXPECT_EQ(33, Graph2->GetEdges());

  Graph3 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 19, 3);
  EXPECT_EQ(19, Graph3->GetNodes());
  EXPECT_EQ(54, Graph3->GetEdges());

  Graph4 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 8, 4);
  EXPECT_EQ(20, Graph4->GetNodes());
  EXPECT_EQ(60, Graph4->GetEdges());
}

// Test TNEANet GetInEgonetAttr
TEST(subgraph, TestGetInEgonetsAttr) {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;
  PNEANet Graph3;
  PNEANet Graph4;
  PNEANet Graph5;
  PNEANet Graph6;
  PNEANet Graph7;
  const TStr s = "id";

  Graph = GetTestTNEANet();
  //Graph->Dump();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetAttr(Graph, 0, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());
  for (TNEANet::TNodeI NI = Graph0->BegNI(); NI < Graph0->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph0->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph0->BegEI(); EI < Graph0->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph0->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph1 = TSnap::GetInEgonetAttr(Graph, 3, 1);
  //Graph1->Dump();
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_EQ(6, Graph1->GetEdges());
  for (TNEANet::TNodeI NI = Graph1->BegNI(); NI < Graph1->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph1->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph1->BegEI(); EI < Graph1->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph1->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph2 = TSnap::GetInEgonetAttr(Graph, 6, 2);
  EXPECT_EQ(7, Graph2->GetNodes());
  EXPECT_EQ(15, Graph2->GetEdges());
  for (TNEANet::TNodeI NI = Graph2->BegNI(); NI < Graph2->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph2->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph2->BegEI(); EI < Graph2->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph2->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph3 = TSnap::GetInEgonetAttr(Graph, 9, 3);
  EXPECT_EQ(10, Graph3->GetNodes());
  EXPECT_EQ(24, Graph3->GetEdges());
  for (TNEANet::TNodeI NI = Graph3->BegNI(); NI < Graph3->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph3->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph3->BegEI(); EI < Graph3->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph3->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph4 = TSnap::GetInEgonetAttr(Graph, 12, 4);
  EXPECT_EQ(13, Graph4->GetNodes());
  EXPECT_EQ(33, Graph4->GetEdges());
  for (TNEANet::TNodeI NI = Graph4->BegNI(); NI < Graph4->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph4->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph4->BegEI(); EI < Graph4->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph4->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph5 = TSnap::GetInEgonetAttr(Graph, 15, 5);
  EXPECT_EQ(16, Graph5->GetNodes());
  EXPECT_EQ(42, Graph5->GetEdges());
  for (TNEANet::TNodeI NI = Graph5->BegNI(); NI < Graph5->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph5->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph5->BegEI(); EI < Graph5->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph5->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph6 = TSnap::GetInEgonetAttr(Graph, 18, 6);
  EXPECT_EQ(19, Graph6->GetNodes());
  EXPECT_EQ(54, Graph6->GetEdges());
  for (TNEANet::TNodeI NI = Graph6->BegNI(); NI < Graph6->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph6->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph6->BegEI(); EI < Graph6->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph6->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph7 = TSnap::GetInEgonetAttr(Graph, 1, 7);
  EXPECT_EQ(20, Graph7->GetNodes());
  EXPECT_EQ(60, Graph7->GetEdges());
  for (TNEANet::TNodeI NI = Graph7->BegNI(); NI < Graph7->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph7->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph7->BegEI(); EI < Graph7->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph7->GetIntAttrDatE(EI.GetId(), s));
  }
}

// Test TUNGraph GetInEgonetSub
TEST(subgraph, TestGetInEgonetSubTUNGraph) {
  PUNGraph Graph;
  PUNGraph Graph0;
  PUNGraph Graph1;
  PUNGraph Graph2;

  Graph = GetTestTUNGraph();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetSub<PUNGraph>(Graph, 3, 0, 2, 1.0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetInEgonetSub<PUNGraph>(Graph, 0, 1, 3, -1.0);
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_LE(4, Graph1->GetEdges());
  EXPECT_GE(6, Graph1->GetEdges());

  Graph2 = TSnap::GetInEgonetSub<PUNGraph>(Graph, 0, 1, 0, 0.9);
  EXPECT_EQ(6, Graph2->GetNodes());
  EXPECT_LE(10, Graph2->GetEdges());
  EXPECT_GE(12, Graph2->GetEdges());
}

// Test TUNGraph GetInEgonetSub
TEST(subgraph, TestGetInEgonetSubTNGraph) {
  PNGraph Graph;
  PNGraph Graph0;
  PNGraph Graph1;
  PNGraph Graph2;

  Graph = GetTestTNGraph();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetSub<PNGraph>(Graph, 3, 0, 2, 1.0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetInEgonetSub<PNGraph>(Graph, 0, 1, 2, -1.0);
  EXPECT_EQ(3, Graph1->GetNodes());
  EXPECT_EQ(3, Graph1->GetEdges());

  Graph2 = TSnap::GetInEgonetSub<PNGraph>(Graph, 0, 1, 0, 0.9);
  EXPECT_EQ(3, Graph2->GetNodes());
  EXPECT_EQ(3, Graph2->GetEdges());
}

// Test TNEANet GetInEgonetSub
TEST(subgraph, TestGetInEgonetSubTNEANet) {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;

  Graph = GetTestTNEANet();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetSub<PNEANet>(Graph, 3, 0, 2, 1.0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());

  Graph1 = TSnap::GetInEgonetSub<PNEANet>(Graph, 0, 1, 2, -1.0);
  EXPECT_EQ(3, Graph1->GetNodes());
  EXPECT_EQ(3, Graph1->GetEdges());

  Graph2 = TSnap::GetInEgonetSub<PNEANet>(Graph, 0, 1, 0, 0.9);
  EXPECT_EQ(3, Graph2->GetNodes());
  EXPECT_EQ(3, Graph2->GetEdges());
}

// Test TNEANet GetEgonetAttr
TEST(subgraph, TestGetEgonetAttr) {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;
  const TStr s = "id";

  Graph = GetTestTNEANet();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetEgonetAttr(Graph, 3, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());
  for (TNEANet::TNodeI NI = Graph0->BegNI(); NI < Graph0->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph0->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph0->BegEI(); EI < Graph0->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph0->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph1 = TSnap::GetEgonetAttr(Graph, 4, 1);
  EXPECT_EQ(7, Graph1->GetNodes());
  EXPECT_EQ(15, Graph1->GetEdges());
  for (TNEANet::TNodeI NI = Graph1->BegNI(); NI < Graph1->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph1->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph1->BegEI(); EI < Graph1->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph1->GetIntAttrDatE(EI.GetId(), s));
  }

  Graph2 = TSnap::GetEgonetAttr(Graph, 4, 2);
  EXPECT_EQ(13, Graph2->GetNodes());
  EXPECT_EQ(33, Graph2->GetEdges());
  for (TNEANet::TNodeI NI = Graph2->BegNI(); NI < Graph2->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph2->GetIntAttrDatN(NI.GetId(), s));
  }
  for (TNEANet::TEdgeI EI = Graph2->BegEI(); EI < Graph2->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId() + EI.GetDstNId(), Graph2->GetIntAttrDatE(EI.GetId(), s));
  }
}

// Test TNEANet GetInEgonetAttr
TEST(subgraph, TestGetInEgonetAttr) {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;
  const TStr s = "id";

  Graph = GetTestTNEANet();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetAttr(Graph, 3, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());
  for (TNEANet::TNodeI NI = Graph0->BegNI(); NI < Graph0->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph0->GetIntAttrDatN(NI.GetId(), s));
  }

  Graph1 = TSnap::GetInEgonetAttr(Graph, 4, 1);
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_EQ(6, Graph1->GetEdges());
  for (TNEANet::TNodeI NI = Graph1->BegNI(); NI < Graph1->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph1->GetIntAttrDatN(NI.GetId(), s));
  }

  Graph2 = TSnap::GetInEgonetAttr(Graph, 4, 2);
  EXPECT_EQ(7, Graph2->GetNodes());
  EXPECT_EQ(15, Graph2->GetEdges());
  for (TNEANet::TNodeI NI = Graph2->BegNI(); NI < Graph2->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph2->GetIntAttrDatN(NI.GetId(), s));
  }
}

// Test TNEANet GetOutEgonetAttr
TEST(subgraph, TestGetOutEgonetAttr) {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;
  const TStr s = "id";

  Graph = GetTestTNEANet();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetOutEgonetAttr(Graph, 3, 0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());
  for (TNEANet::TNodeI NI = Graph0->BegNI(); NI < Graph0->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph0->GetIntAttrDatN(NI.GetId(), s));
  }

  Graph1 = TSnap::GetOutEgonetAttr(Graph, 4, 1);
  EXPECT_EQ(4, Graph1->GetNodes());
  EXPECT_EQ(6, Graph1->GetEdges());
  for (TNEANet::TNodeI NI = Graph1->BegNI(); NI < Graph1->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph1->GetIntAttrDatN(NI.GetId(), s));
  }

  Graph2 = TSnap::GetOutEgonetAttr(Graph, 4, 2);
  EXPECT_EQ(7, Graph2->GetNodes());
  EXPECT_EQ(15, Graph2->GetEdges());
  for (TNEANet::TNodeI NI = Graph2->BegNI(); NI < Graph2->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph2->GetIntAttrDatN(NI.GetId(), s));
  }
}

// Test TNEANet GetInEgonetSubAttr
TEST(subgraph, TestGetInEgonetSubAttr) {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;
  const TStr s = "id";

  Graph = GetTestTNEANet();
  EXPECT_EQ(20, Graph->GetNodes());
  EXPECT_EQ(60, Graph->GetEdges());

  Graph0 = TSnap::GetInEgonetSubAttr(Graph, 3, 0, 2, 1.0);
  EXPECT_EQ(1, Graph0->GetNodes());
  EXPECT_EQ(0, Graph0->GetEdges());
  for (TNEANet::TNodeI NI = Graph0->BegNI(); NI < Graph0->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph0->GetIntAttrDatN(NI.GetId(), s));
  }

  Graph1 = TSnap::GetInEgonetSubAttr(Graph, 0, 1, 2, -1.0);
  EXPECT_EQ(3, Graph1->GetNodes());
  EXPECT_EQ(3, Graph1->GetEdges());
  for (TNEANet::TNodeI NI = Graph1->BegNI(); NI < Graph1->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph1->GetIntAttrDatN(NI.GetId(), s));
  }

  Graph2 = TSnap::GetInEgonetSubAttr(Graph, 0, 1, 0, 0.9);
  EXPECT_EQ(3, Graph2->GetNodes());
  EXPECT_EQ(3, Graph2->GetEdges());
  for (TNEANet::TNodeI NI = Graph2->BegNI(); NI < Graph2->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId(), Graph2->GetIntAttrDatN(NI.GetId(), s));
  }
}

// Test PGraph GetGraphUnion
TEST(subgraph, TestGetGraphUnion) {
  //Undirected graph
  PUNGraph Graph = TUNGraph::New();
  PUNGraph Graph0 = TUNGraph::New();

  for (int i = 0; i < 5; i++) {
    Graph->AddNode(i);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i,(i+1) % 5);
    Graph->AddEdge(i,(i+2) % 5);
  }
  for (int i = 3; i < 8; i++) {
    Graph0->AddNode(i);
  }
  for (int i = 0; i < 5; i++) {
    Graph0->AddEdge(i + 3,((i+1) % 5) + 3);
  }
  TSnap::GetGraphUnion(Graph, Graph0);
  EXPECT_EQ(8, Graph->GetNodes());
  EXPECT_EQ(14, Graph->GetEdges());

  //Directed graph
  PNGraph Graph1 = TNGraph::New();
  PNGraph Graph2 = TNGraph::New();
  for (int i = 0; i < 4; i++) {
    Graph1->AddNode(i);
  }
  for (int i = 1; i < 5; i++) {
    Graph2->AddNode(i);
  }

  Graph1->AddEdge(0, 1);
  Graph1->AddEdge(1, 2);
  Graph2->AddEdge(1, 2);
  Graph2->AddEdge(2, 1);
  Graph1->AddEdge(2, 3);
  Graph2->AddEdge(2, 3);
  Graph1->AddEdge(3, 2);
  Graph2->AddEdge(3, 4);
  Graph2->AddEdge(1, 4);

  TSnap::GetGraphUnion(Graph1, Graph2);
  EXPECT_EQ(5, Graph1->GetNodes());
  EXPECT_EQ(7, Graph1->GetEdges());

  //Directed multigraph
  PNEANet Graph3 = TNEANet::New();
  PNEANet Graph4 = TNEANet::New();
  int EId = 0;
  for (int i = 0; i < 4; i++) {
    Graph3->AddNode(i);
  }
  for (int i = 1; i < 5; i++) {
    Graph4->AddNode(i);
  }

  Graph3->AddEdge(0, 1, EId++);
  Graph3->AddEdge(1, 2, EId++);
  Graph4->AddEdge(1, 2, EId++);
  Graph4->AddEdge(2, 1, EId++);
  Graph3->AddEdge(2, 3, EId);
  Graph4->AddEdge(2, 3, EId++);
  Graph3->AddEdge(3, 2, EId++);
  Graph4->AddEdge(3, 4, EId++);
  Graph4->AddEdge(1, 4, EId++);

  TSnap::GetGraphUnion(Graph3, Graph4);
  EXPECT_EQ(5, Graph3->GetNodes());
  EXPECT_EQ(7, Graph3->GetEdges());
}


// Test PGraph GetGraphUnionAttr
TEST(subgraph, TestGetGraphUnionAttr) {
  PNEANet Graph = PNEANet::New();
  PNEANet Graph0 = PNEANet::New();
  
  TStr s = "id";
  int EId;
  for (int i = 0; i < 6; i++) {
    Graph->AddNode(i);
    Graph->AddIntAttrDatN(i, i, s);
  }

  for (int i = 3; i < 9; i++) {
    Graph0->AddNode(i);
    Graph0->AddIntAttrDatN(i, i, s);
  }

  for (int i = 0; i < 6; i++) {
    EId = Graph->AddEdge(i, (i + 2) % 6);
    Graph->AddIntAttrDatE(EId, (i + 2) % 6, s);
    EId = Graph->AddEdge(i, (i + 5) % 6);
    Graph->AddIntAttrDatE(EId, (i + 5) % 6, s);
  }

  for (int i = 0; i < 6; i++) {
    EId = Graph0->AddEdge(i + 3, ((i + 3) % 6) + 3);
    Graph0->AddIntAttrDatE(EId, ((i + 3) % 6) + 3, s);
    EId = Graph0->AddEdge(i + 3, ((i + 4) % 6) + 3);
    Graph0->AddIntAttrDatE(EId, ((i + 4) % 6) + 3, s);
  }

  TSnap::GetGraphUnionAttr(Graph, Graph0);
  EXPECT_EQ(9, Graph->GetNodes());
  EXPECT_EQ(24, Graph->GetEdges());
}

// Test PGraph GetGraphIntersection
TEST(subgraph, TestGetGraphIntersection) {
  //Undirected graph
  PUNGraph Graph = TUNGraph::New();
  PUNGraph Graph0 = TUNGraph::New();

  for (int i = 0; i < 5; i++) {
    Graph->AddNode(i);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i,(i + 1) % 5);
    Graph->AddEdge(i,(i + 2) % 5);
  }

  for (int i = 2; i < 6; i++) {
    Graph0->AddNode(i);
  }
  for (int i = 0; i < 4; i++) {
    Graph0->AddEdge(i + 2, ((i + 1) % 4) + 2);
    Graph0->AddEdge(i + 2, ((i + 2) % 4) + 2);
  }

  PUNGraph IntersectionGraph = TSnap::GetGraphIntersection(Graph, Graph0);
  EXPECT_EQ(3, IntersectionGraph->GetNodes());
  EXPECT_EQ(3, IntersectionGraph->GetEdges());

  //Directed graph
  PNGraph Graph1 = TNGraph::New();
  PNGraph Graph2 = TNGraph::New();

  for (int i = 0; i < 8; i++) {
    Graph1->AddNode(i);
  }
  for (int i = 0; i < 8; i++) {
    Graph1->AddEdge(i, (i + 1) % 8);
    Graph1->AddEdge((i + 1) % 8, i);
  }

  for (int i = 2; i < 6; i++) {
    Graph2->AddNode(i);
  }
  for (int i = 0; i < 4; i++) {
    Graph2->AddEdge(i + 2, ((i + 1) % 4) + 2);
    Graph2->AddEdge(i + 2, ((i + 2) % 4) + 2);
  }

  PNGraph IntersectionGraph0 = TSnap::GetGraphIntersection(Graph1, Graph2);
  EXPECT_EQ(4, IntersectionGraph0->GetNodes());
  EXPECT_EQ(3, IntersectionGraph0->GetEdges());

  //Directed multigraph
  PNEANet Graph3 = TNEANet::New();
  PNEANet Graph4 = TNEANet::New();
  int EId3 = 0;
  int EId4 = 1;

  for (int i = 0; i < 4; i++) {
    Graph3->AddNode(i);
  }
  for (int i = 0; i < 3; i++) {
    Graph3->AddEdge(i, i + 1, EId3++);
  }
  Graph3->AddEdge(1, 0, EId3++);
  Graph3->AddEdge(1, 2, EId3++);
  Graph3->AddEdge(3, 2, EId3++);

  for (int i = 1; i < 5; i++) {
    Graph4->AddNode(i);
  }
  for (int i = 1; i < 4; i++) {
    Graph4->AddEdge(i + 1, i, EId4 + 3);
    Graph4->AddEdge(i, i + 1, EId4++);
  }

  PNEANet IntersectionGraph1 = TSnap::GetGraphIntersection(Graph3, Graph4);
  EXPECT_EQ(3, IntersectionGraph1->GetNodes());
  EXPECT_EQ(3, IntersectionGraph1->GetEdges());
}

// Test PNEANet GetGraphIntersectionAttr
TEST(subgraph, TestGetGraphIntersectionAttr) {
  PNEANet Graph = PNEANet::New();
  PNEANet Graph0 = PNEANet::New();

  TStr s = "id";
  int EId;
  for (int i = 0; i < 7; i++) {
    Graph->AddNode(i);
    Graph->AddIntAttrDatN(i, i, s);
  }

  for (int i = 2; i < 9; i++) {
    Graph0->AddNode(i);
    Graph0->AddIntAttrDatN(i, i, s);
  }

  for (int i = 0; i < 7; i++) {
    EId = Graph->AddEdge(i, (i + 2) % 7);
    Graph->AddIntAttrDatE(EId, (i + 2) % 7, s);
    EId = Graph->AddEdge(i, (i + 3) % 7);
    Graph->AddIntAttrDatE(EId, (i + 3) % 7, s);
  }

  for (int i = 0; i < 7; i++) {
    EId = Graph0->AddEdge(i + 2, ((i + 3) % 7) + 2);
    Graph0->AddIntAttrDatE(EId, ((i + 3) % 7) + 2, s);
  }

  PNEANet IntersectionGraph = TSnap::GetGraphIntersectionAttr(Graph, Graph0);
  EXPECT_EQ(5, IntersectionGraph->GetNodes());
  EXPECT_EQ(3, IntersectionGraph->GetEdges());
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
PNEANet GetTestTNEANet() {
  PNEANet Graph = PNEANet::New();
  TStr s = "id";
  int EId;

  for (int i = 0; i < 20; i++) {
    Graph->AddNode(i);
    Graph->AddIntAttrDatN(i, i, s);
  }

  for (int i = 0; i < 20; i++) {
    EId = Graph->AddEdge(i, (i + 1) % 20);
    Graph->AddIntAttrDatE(EId, i + (i + 1) % 20, s);

    EId = Graph->AddEdge(i, (i + 2) % 20);
    Graph->AddIntAttrDatE(EId, i + (i + 2) % 20, s);

    EId = Graph->AddEdge(i, (i + 3) % 20);
    Graph->AddIntAttrDatE(EId, i + (i + 3) % 20, s);
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

// Generate TUNGraph Petersen graph
PUNGraph GetUPetersen() {
  PUNGraph Graph = TUNGraph::New();

  for (int i = 0; i < 10; i++) {
    Graph->AddNode(i);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i,(i+1) % 5);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i + 5,(i+2) % 5 + 5);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i,i + 5);
  }

  return Graph;
}

// Generate TNGraph Petersen graph
PNGraph GetDPetersen() {
  PNGraph Graph = TNGraph::New();
  
  for (int i = 0; i < 10; i++) {
    Graph->AddNode(i);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i,(i+1) % 5);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i + 5,(i+2) % 5 + 5);
  }
  for (int i = 0; i < 5; i++) {
    Graph->AddEdge(i,i + 5);
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

