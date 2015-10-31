#include "Snap.h"

// Print graph statistics
void PrintGStats(const char s[], PUNGraph Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph->GetNodes(), Graph->GetEdges(),
      Graph->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  PUNGraph Graph;

  Graph = TUNGraph::New();
  PrintGStats("DefaultConstructor:Graph",Graph);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "demo.graph.dat";

  PUNGraph Graph;
  PUNGraph Graph1;
  PUNGraph Graph2;
  int i;
  int n;
  int NCount;
  int ECount1;
  int ECount2;
  int x,y;
  bool t;

  Graph = TUNGraph::New();
  t = Graph->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  t = Graph->Empty();
  n = Graph->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Graph->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Graph->IsEdge(x,y)) {
      n = Graph->AddEdge(x, y);
      NCount--;
    }
  }
  PrintGStats("ManipulateNodesEdges:Graph",Graph);

  // get all the nodes
  NCount = 0;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }

  // get all the edges for all the nodes
  ECount1 = 0;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ECount1++;
    }
  }
  ECount1 /= 2;

  // get all the edges directly
  ECount2 = 0;
  for (TUNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ECount2++;
  }
  printf("graph ManipulateNodesEdges:Graph, nodes %d, edges1 %d, edges2 %d\n",
      NCount, ECount1, ECount2);

  // assignment
  Graph1 = TUNGraph::New();
  *Graph1 = *Graph;
  PrintGStats("ManipulateNodesEdges:Graph1",Graph1);

  // save the graph
  {
    TFOut FOut(FName);
    Graph->Save(FOut);
    FOut.Flush();
  }

  // load the graph
  {
    TFIn FIn(FName);
    Graph2 = TUNGraph::Load(FIn);
  }
  PrintGStats("ManipulateNodesEdges:Graph2",Graph2);

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Graph->GetRndNId();
    Graph->DelNode(n);
  }

  PrintGStats("ManipulateNodesEdges:Graph",Graph);

  Graph1->Clr();
  PrintGStats("ManipulateNodesEdges:Graph1",Graph1);
}

// Test small graph
void GetSmallGraph() {
  PUNGraph Graph;

  Graph = TUNGraph::GetSmallGraph();
  PrintGStats("GetSmallGraph:Graph",Graph);
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateNodesEdges();
  GetSmallGraph();
}


