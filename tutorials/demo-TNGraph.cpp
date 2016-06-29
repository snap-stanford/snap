#include "Snap.h"

// Print graph statistics
void PrintGStats(const char s[], PNGraph Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph->GetNodes(), Graph->GetEdges(),
      Graph->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  PNGraph Graph;

  Graph = TNGraph::New();
  PrintGStats("DefaultConstructor:Graph",Graph);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "demo.graph.dat";

  PNGraph Graph;
  PNGraph Graph1;
  PNGraph Graph2;
  int i;
  int n;
  int NCount;
  int ECount1;
  int ECount2;
  int x,y;
  bool t;

  Graph = TNGraph::New();
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

  // traverse the nodes, count them
  NCount = 0;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
    // get the node ID and its in- and out-degree
    printf("node id %d has in-degree %d and out-degree %d\n",
          NI.GetId(), NI.GetInDeg(), NI.GetOutDeg());
  }

  // traverse all the in-edges of all the nodes, count them
  ECount1 = 0;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetInDeg(); e++) {
      ECount1++;
      // get the node ID and its in-neighbour
      printf("node id %d has in-neighbor %d\n", NI.GetId(), NI.GetInNId(e));
    }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      // get the node ID and its out-neighbour
      printf("node id %d has out-neighbor %d\n", NI.GetId(), NI.GetOutNId(e));
    }
  }

  // traverse all the edges directly, count them
  ECount2 = 0;
  for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ECount2++;
    // get the source and the destination nodes of the edge
    printf("edge from source node %d to destination node %d\n",
          EI.GetSrcNId(), EI.GetDstNId());
  }
  printf("ManipulateNodesEdges:Graph, nodes %d, edges1 %d, edges2 %d\n",
      NCount, ECount1, ECount2);

  // assignment
  Graph1 = TNGraph::New();
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
    Graph2 = TNGraph::Load(FIn);
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
  PNGraph Graph;

  Graph = TNGraph::GetSmallGraph();
  PrintGStats("GetSmallGraph:Graph",Graph);
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateNodesEdges();
  GetSmallGraph();
}

