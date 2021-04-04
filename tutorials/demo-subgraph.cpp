#include "Snap.h"

PUNGraph GetTestTUNGraph();
PNGraph GetTestTNGraph();
PNEGraph GetTestTNEGraph();
PNEANet GetTestTNEANet();
TPt <TNodeEDatNet<TInt, TInt> > GetTestTNodeEDatNet();
TPt <TNodeEdgeNet<TInt, TInt> > GetTestTNodeEdgeNet();

// Dump the graph 
template <class PGraph>
void PrintGraph(const char s[], const PGraph& Graph) {
  printf("---- %s -----\n",s);
  Graph->Dump();
}

// Print net stats
template <class TNet>
void PrintNet(const char s[], const TNet& Net) {
  printf("---- %s -----\n",s);
  printf("nodes: %d, edges: %d\n", Net->GetNodes(), Net->GetEdges());
  printf("\n");
}

// Test subgraphs
void TestSubTUNGraphs() {
  PUNGraph Graph;
  PUNGraph Graph1;
  PUNGraph Graph2;
  PUNGraph Graph3;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Graph = GetTestTUNGraph();
  PrintGraph("TUNGraph", Graph);

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Graph1 = TSnap::GetSubGraph(Graph, NIdV);
  PrintGraph("TUNGraph1", Graph1);

  Graph2 = TSnap::GetSubGraph(Graph, NIdV, true);
  PrintGraph("TUNGraph2", Graph2);

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Graph3 = TSnap::GetSubGraph(Graph, NIdV1, true);
  PrintGraph("TUNGraph3", Graph3);
}

// Test subgraphs
void TestSubTNGraphs() {
  PNGraph Graph;
  PNGraph Graph1;
  PNGraph Graph2;
  PNGraph Graph3;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Graph = GetTestTNGraph();
  PrintGraph("TNGraph", Graph);

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Graph1 = TSnap::GetSubGraph(Graph, NIdV);
  PrintGraph("TNGraph1", Graph1);

  Graph2 = TSnap::GetSubGraph(Graph, NIdV, true);
  PrintGraph("TNGraph2", Graph2);

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Graph3 = TSnap::GetSubGraph(Graph, NIdV1, true);
  PrintGraph("TNGraph3", Graph3);
}

// Test subgraphs
void TestSubTNEGraphs() {
  PNEGraph Graph;
  PNEGraph Graph1;
  PNEGraph Graph2;
  PNEGraph Graph3;
  int i;
  TIntV NIdV;
  TIntV NIdV1;
  TIntV EIdV;

  Graph = GetTestTNEGraph();
  PrintGraph("TNEGraph", Graph);

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Graph1 = TSnap::GetSubGraph(Graph, NIdV);
  PrintGraph("TNEGraph1", Graph1);

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Graph2 = TSnap::GetSubGraph(Graph, NIdV1);
  PrintGraph("TNEGraph2", Graph2);

  for (i = 0; i < 120; i += 2) {
    EIdV.Add(i);
  }

  Graph3 = TSnap::GetESubGraph(Graph, EIdV);
  PrintGraph("TNEGraph3", Graph3);
}

// Test subgraphs
void TestEDatSubNets() {
  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  TPt <TNodeEDatNet<TInt, TInt> > Net3;
  TPt <TNodeEDatNet<TInt, TInt> > Net4;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Net = GetTestTNodeEDatNet();
  PrintNet("TestEDatSubNets", Net);

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Net1 = TSnap::GetSubGraph(Net, NIdV);
  PrintNet("TestEDatSubNets1", Net1);

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Net2 = TSnap::GetSubGraph(Net, NIdV1);
  PrintNet("TestEDatSubNets2", Net2);

  Net3 = TSnap::GetEDatSubGraph(Net, 1, 0);
  PrintNet("TestEDatSubNets3", Net3);

  Net4 = TSnap::GetEDatSubGraph(Net, 2, -1);
  PrintNet("TestEDatSubNets4", Net4);
}

// Test subgraphs
void TestEdgeSubNets() {
  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  TPt <TNodeEdgeNet<TInt, TInt> > Net3;
  TPt <TNodeEdgeNet<TInt, TInt> > Net4;
  int i;
  TIntV NIdV;
  TIntV NIdV1;

  Net = GetTestTNodeEdgeNet();
  PrintNet("TestEdgeSubNets", Net);

  for (i = 10; i < 15; i++) {
    NIdV.Add(i);
  }

  Net1 = TSnap::GetSubGraph(Net, NIdV);
  PrintNet("TestEdgeSubNets1", Net1);

  for (i = 0; i < 20; i += 2) {
    NIdV1.Add(i);
  }

  Net2 = TSnap::GetSubGraph(Net, NIdV1);
  PrintNet("TestEdgeSubNets2", Net2);

  Net3 = TSnap::GetEDatSubGraph(Net, 1, 0);
  PrintNet("TestEdgeSubNets3", Net3);

  Net4 = TSnap::GetEDatSubGraph(Net, 2, -1);
  PrintNet("TestEdgeSubNets4", Net4);
}

// Test graph conversion
void TestConvertGraphs() {
  PNGraph NGraph;
  PUNGraph UNGraph;
  int N1, N2, N3;
  int E1, E2, E3;

  NGraph = GetTestTNGraph();
  N1 = NGraph->GetNodes();
  E1 = NGraph->GetEdges();

  UNGraph = TSnap::ConvertGraph<PUNGraph>(NGraph);
  N2 = UNGraph->GetNodes();
  E2 = UNGraph->GetEdges();

  NGraph = TSnap::ConvertGraph<PNGraph>(UNGraph);
  N3 = NGraph->GetNodes();
  E3 = NGraph->GetEdges();

  printf("---- TestConvertGraphs -----\n");
  printf("nodes: %d,%d,%d,  edges: %d,%d,%d\n", N1, N2, N3, E1, E2, E3);
  printf("\n");
}

// Test node subgraph conversion
void TestConvertSubGraphs() {
  PNGraph NGraph;
  PUNGraph UNGraph;
  int N1, N2, N3;
  int E1, E2, E3;
  TIntV NIdV;
  int i;

  NGraph = GetTestTNGraph();
  N1 = NGraph->GetNodes();
  E1 = NGraph->GetEdges();

  for (i = 0; i < 20; i += 2) {
    NIdV.Add(i);
  }

  // TODO: fix TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true), it fails
  // UNGraph = TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true);
  UNGraph = TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV);
  N2 = UNGraph->GetNodes();
  E2 = UNGraph->GetEdges();

  NGraph = TSnap::ConvertSubGraph<PNGraph>(UNGraph, NIdV);
  N3 = NGraph->GetNodes();
  E3 = NGraph->GetEdges();

  printf("---- TestConvertSubGraphs -----\n");
  printf("nodes: %d,%d,%d,  edges: %d,%d,%d\n", N1, N2, N3, E1, E2, E3);
  printf("\n");
}

// Test edge subgraph conversion
void TestConvertESubGraphs() {
  PNEGraph NEGraph;
  PNGraph NGraph;
  int N1, N2, N3;
  int E1, E2, E3;
  TIntV NIdV;
  TIntV EIdV;
  int i;

  NGraph = GetTestTNGraph();
  N1 = NGraph->GetNodes();
  E1 = NGraph->GetEdges();

  for (i = 0; i < 20; i += 2) {
    NIdV.Add(i);
  }

  // TODO: fix TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true), it fails
  // UNGraph = TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true);
  NEGraph = TSnap::ConvertGraph<PNEGraph>(NGraph);
  N2 = NEGraph->GetNodes();
  E2 = NEGraph->GetEdges();

  // select every second edge
  i = 0;
  for (TNEGraph::TEdgeI EI = NEGraph->BegEI(); EI < NEGraph->EndEI(); EI++) {
    if (i == 0) {
      EIdV.Add(EI.GetId());
    }
    i = (i + 1) % 2;
  }

  NGraph = TSnap::ConvertESubGraph<PNGraph>(NEGraph, EIdV);
  N3 = NGraph->GetNodes();
  E3 = NGraph->GetEdges();

  printf("---- TestConvertESubGraphs -----\n");
  printf("nodes: %d,%d,%d,  edges: %d,%d,%d\n", N1, N2, N3, E1, E2, E3);
  printf("\n");
}

// Test get random graphs
void TestGetRndGraphs() {
  PUNGraph UNGraph1, UNGraph2, UNGraph3;
  PNGraph NGraph1, NGraph2, NGraph3;
  PNEGraph NEGraph1, NEGraph2, NEGraph3;
  int N1, N2, N3;
  int E1, E2, E3;

  UNGraph1 = GetTestTUNGraph();
  N1 = UNGraph1->GetNodes();
  E1 = UNGraph1->GetEdges();

  UNGraph2 = TSnap::GetRndSubGraph(UNGraph1,10);
  N2 = UNGraph2->GetNodes();
  E2 = UNGraph2->GetEdges();

  UNGraph3 = TSnap::GetRndESubGraph(UNGraph1,10);
  N3 = UNGraph3->GetNodes();
  E3 = UNGraph3->GetEdges();

  printf("---- TestGetRndGraphs1 -----\n");
  printf("nodes: %d,%d,%d,  edges: %d,%d,%d\n", N1, N2, N3, E1, E2, E3);
  printf("\n");

  NGraph1 = GetTestTNGraph();
  N1 = NGraph1->GetNodes();
  E1 = NGraph1->GetEdges();

  NGraph2 = TSnap::GetRndSubGraph(NGraph1,10);
  N2 = NGraph2->GetNodes();
  E2 = NGraph2->GetEdges();

  NGraph3 = TSnap::GetRndESubGraph(NGraph1,10);
  N3 = NGraph3->GetNodes();
  E3 = NGraph3->GetEdges();

  printf("---- TestGetRndGraphs2 -----\n");
  printf("nodes: %d,%d,%d,  edges: %d,%d,%d\n", N1, N2, N3, E1, E2, E3);
  printf("\n");
}

// Test TNGraph GetEgoNetHop
void TestGetInEgonetHopTNGraph() {
  // Test on TNSmall Graph
  PNGraph Small = TNGraph::GetSmallGraph();
  PNGraph ego1 = TSnap::GetInEgonetHop<PNGraph>(Small, 4, 1);
  PrintGraph("small-ego1", ego1);
  PNGraph ego2 = TSnap::GetInEgonetHop<PNGraph>(Small, 4, 2);
  PrintGraph("small-ego2", ego2);
  PNGraph ego3 = TSnap::GetInEgonetHop<PNGraph>(Small, 4, 3);
  PrintGraph("small-ego3", ego3);

  // Middle Size TN graph
  PNGraph Middle = TNGraph::New();
  for (int i = 0; i < 6; i++) {
    Middle->AddNode(i);
  }
  for (int i = 0; i < 6; i++) {
    Middle->AddEdge(i, (i + 1) % 6);
    Middle->AddEdge(i, (i + 2) % 6);
  }
  PrintGraph("Mid-size TNGraph", Middle);
  PNGraph midego2 = TSnap::GetInEgonetHop<PNGraph>(Middle, 0, 2);
  PrintGraph("Mid-size Ego2", midego2);
  PNGraph midego3 = TSnap::GetInEgonetHop<PNGraph>(Middle, 0, 3);
  PrintGraph("Mid-size Ego3", midego3);

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
  PrintGraph("Large TNGraph", Graph);

  Graph0 = TSnap::GetInEgonetHop<PNGraph>(Graph, 0, 0);
  PrintGraph("Large TNGraph - Ego 0", Graph0);

  Graph1 = TSnap::GetInEgonetHop<PNGraph>(Graph, 3, 1);
  PrintGraph("Large TNGraph - Ego 1", Graph1);

  Graph2 = TSnap::GetInEgonetHop<PNGraph>(Graph, 6, 2);
  PrintGraph("Large TNGraph - Ego 2", Graph2);

  Graph3 = TSnap::GetInEgonetHop<PNGraph>(Graph, 9, 3);
  PrintGraph("Large TNGraph - Ego 3", Graph3);

  Graph4 = TSnap::GetInEgonetHop<PNGraph>(Graph, 12, 4);
  PrintGraph("Large TNGraph - Ego 4", Graph4);

  Graph5 = TSnap::GetInEgonetHop<PNGraph>(Graph, 15, 5);
  PrintGraph("Large TNGraph - Ego 5", Graph5);

  Graph6 = TSnap::GetInEgonetHop<PNGraph>(Graph, 18, 6);
  PrintGraph("Large TNGraph - Ego 6", Graph6);

  Graph7 = TSnap::GetInEgonetHop<PNGraph>(Graph, 1, 7);
  PrintGraph("Large TNGraph - Ego 7", Graph7);
}

// Test TUNGraph GetInEgonetHop
void TestGetInEgonetHopTUNGraph() {
  PUNGraph Graph;
  PUNGraph Graph0;
  PUNGraph Graph1;
  PUNGraph Graph2;
  PUNGraph Graph3;
  PUNGraph Graph4;

  Graph = GetTestTUNGraph();
  PrintGraph("Large TUNGraph", Graph);

  Graph0 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 3, 0);
  PrintGraph("Large TUNGraph - Ego 0", Graph0);

  Graph1 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 0, 1);
  PrintGraph("Large TUNGraph - Ego 1", Graph1);

  Graph2 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 7, 2);
  PrintGraph("Large TUNGraph - Ego 2", Graph1);

  Graph3 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 19, 3);
  PrintGraph("Large TUNGraph - Ego 3", Graph1);

  Graph4 = TSnap::GetInEgonetHop<PUNGraph>(Graph, 8, 4);
  PrintGraph("Large TUNGraph - Ego 4", Graph4);
}

// Test TNEANet GetInEgonetAttr
void TestGetInEgonetsAttr() {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;

  Graph = GetTestTNEANet();

  Graph0 = TSnap::GetInEgonetAttr(Graph, 0, 0);
  PrintGraph("TNEANet w/ Attr - Ego 0", Graph0);

  Graph1 = TSnap::GetInEgonetAttr(Graph, 3, 1);
  PrintGraph("TNEANet w/ Attr - Ego 1", Graph1);

  Graph2 = TSnap::GetInEgonetAttr(Graph, 6, 2);
  PrintGraph("TNEANet w/ Attr - Ego 2", Graph2);
}

// Test TUNGraph GetInEgonetSub
void TestGetInEgonetSubTUNGraph() {
  PUNGraph Graph;
  PUNGraph Graph1;
  PUNGraph Graph2;

  Graph = GetTestTUNGraph();

  Graph1 = TSnap::GetInEgonetSub<PUNGraph>(Graph, 0, 1, 3, -1.0);
  PrintGraph("TUNGraph - Ego 1 - Sub 3", Graph1);

  Graph2 = TSnap::GetInEgonetSub<PUNGraph>(Graph, 0, 1, 0, 0.9);
  PrintGraph("TUNGraph - Ego 1 - Sub 90%", Graph2);
}

// Test TUNGraph GetInEgonetSub
void TestGetInEgonetSubTNGraph() {
  PNGraph Graph;
  PNGraph Graph1;
  PNGraph Graph2;

  Graph = GetTestTNGraph();

  Graph1 = TSnap::GetInEgonetSub<PNGraph>(Graph, 0, 1, 2, -1.0);
  PrintGraph("TNGraph - Ego 1 - Sub 2", Graph1);

  Graph2 = TSnap::GetInEgonetSub<PNGraph>(Graph, 0, 1, 0, 0.9);
  PrintGraph("TNGraph - Ego 1 - Sub 90%", Graph2);
}

// Test TNEANet GetInEgonetSub
void TestGetInEgonetSubTNEANet() {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;

  Graph = GetTestTNEANet();

  Graph0 = TSnap::GetInEgonetSub<PNEANet>(Graph, 3, 0, 2, 1.0);

  Graph1 = TSnap::GetInEgonetSub<PNEANet>(Graph, 0, 1, 2, -1.0);

  Graph2 = TSnap::GetInEgonetSub<PNEANet>(Graph, 0, 1, 0, 0.9);
}

// Test TNEANet GetInEgonetSubAttr
void TestGetInEgonetSubAttr() {
  PNEANet Graph;
  PNEANet Graph0;
  PNEANet Graph1;
  PNEANet Graph2;

  Graph = GetTestTNEANet();

  Graph1 = TSnap::GetInEgonetSubAttr(Graph, 0, 1, 2, -1.0);
  PrintGraph("TNEANet w/ Attr - Ego 1 - Sub 2", Graph1);

  Graph2 = TSnap::GetInEgonetSubAttr(Graph, 0, 1, 0, 0.9);
  PrintGraph("TNEANet w/ Attr - Ego 1 - Sub 90%", Graph2);
}

// Test PGraph GetGraphUnion
void TestGetGraphUnion() {
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

  PrintGraph("TUNGraph DstGraph before union", Graph);
  PrintGraph("TUNGraph SrcGraph before union", Graph0);

  TSnap::GetGraphUnion(Graph, Graph0);
  PrintGraph("TUNGraph DstGraph after union", Graph);

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

  PrintGraph("TNGraph DstGraph before union", Graph1);
  PrintGraph("TNGraph SrcGraph before union", Graph2);

  TSnap::GetGraphUnion(Graph1, Graph2);
  PrintGraph("TNGraph DstGraph after union", Graph1);

  //Directed multi-graph
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

  PrintGraph("TNEANet DstGraph before union", Graph3);
  PrintGraph("TNEANet SrcGraph before union", Graph4);

  TSnap::GetGraphUnion(Graph3, Graph4);
  PrintGraph("TNEANet DstGraph after union", Graph3);
}


// Test PNEANet GetGraphUnionAttr
void TestGetGraphUnionAttr() {
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
  PrintGraph("PNEANet DstGraph before union", Graph);
  PrintGraph("PNEANet SrcGraph before union", Graph0);

  TSnap::GetGraphUnionAttr(Graph, Graph0);
  PrintGraph("PNEANet DstGraph after union", Graph);
}

// Test PGraph GetGraphIntersection
void TestGetGraphIntersection() {
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

  for (int i = 2; i < 6; i++) {
    Graph0->AddNode(i);
  }
  for (int i = 0; i < 4; i++) {
    Graph0->AddEdge(i + 2, ((i + 1) % 4) + 2);
    Graph0->AddEdge(i + 2, ((i + 2) % 4) + 2);
  }

  PrintGraph("PUNGraph Graph", Graph);
  PrintGraph("PUNGraph Graph0", Graph0);

  PUNGraph IntersectionGraph = TSnap::GetGraphIntersection(Graph, Graph0);
  PrintGraph("PUNGraph - Intersection of Graph and Graph0", IntersectionGraph);

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

  PrintGraph("PNGraph Graph1", Graph1);
  PrintGraph("PNGraph Graph2", Graph2);

  PNGraph IntersectionGraph0 = TSnap::GetGraphIntersection(Graph1, Graph2);
  PrintGraph("PNGraph - Intersection of Graph1 and Graph2", IntersectionGraph0);

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

  PrintGraph("PNEANet Graph3", Graph3);
  PrintGraph("PNEANet Graph4", Graph4);

  PNEANet IntersectionGraph1 = TSnap::GetGraphIntersection(Graph3, Graph4);
  PrintGraph("PNEANet - Intersection of Graph3 and Graph4 ", IntersectionGraph1);
}

// Test PNEANet GetGraphIntersectionAttr
void TestGetGraphIntersectionAttr() {
  PNEANet Graph = PNEANet::New();
  PNEANet Graph0 = PNEANet::New();
  TStr s = "id";
  int EId;

  for (int i = 0; i < 7; i++) {
    Graph->AddNode(i);
    Graph->AddIntAttrDatN(i, i, s);
  }
  for (int i = 0; i < 7; i++) {
    EId = Graph->AddEdge(i, (i + 2) % 7);
    Graph->AddIntAttrDatE(EId, (i + 2) % 7, s);
    EId = Graph->AddEdge(i, (i + 3) % 7);
    Graph->AddIntAttrDatE(EId, (i + 3) % 7, s);
  }

  for (int i = 2; i < 9; i++) {
    Graph0->AddNode(i);
    Graph0->AddIntAttrDatN(i, i, s);
  }
  for (int i = 0; i < 7; i++) {
    EId = Graph0->AddEdge(i + 2, ((i + 3) % 7) + 2);
    Graph0->AddIntAttrDatE(EId, ((i + 3) % 7) + 2, s);
  }

  PrintGraph("PNEANet Graph before intersection", Graph);
  PrintGraph("PNEANet Graph0 before intersection", Graph0);

  PNEANet IntersectionGraph = TSnap::GetGraphIntersectionAttr(Graph, Graph0);
  PrintGraph("PNEANet - Intersection of Graph and Graph0", IntersectionGraph);
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

  for (int i = 0; i < 20; i++) {
    Graph->AddNode(i);
    Graph->AddIntAttrDatN(i, i, s);
  }

  for (int i = 0; i < 20; i++) {
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

int main(int argc, char* argv[]) {
  TestSubTUNGraphs();
  TestSubTNGraphs();
  TestSubTNEGraphs();
  TestEDatSubNets();
  TestEdgeSubNets();
  TestConvertGraphs();
  TestConvertSubGraphs();
  TestConvertESubGraphs();
  TestGetRndGraphs();
  TestGetInEgonetHopTNGraph();
  TestGetInEgonetHopTUNGraph();
  TestGetInEgonetsAttr();
  TestGetInEgonetSubTUNGraph();
  TestGetInEgonetSubTNGraph();
  TestGetInEgonetSubTNEANet();
  TestGetInEgonetSubAttr();
  TestGetGraphUnion();
  TestGetGraphUnionAttr();
  TestGetGraphIntersection();
  TestGetGraphIntersectionAttr();
}

