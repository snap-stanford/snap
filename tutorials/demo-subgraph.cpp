#include "Snap.h"

PUNGraph GetTestTUNGraph();
PNGraph GetTestTNGraph();
PNEGraph GetTestTNEGraph();
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
}

