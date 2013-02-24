#include "Snap.h"

// Print graph statistics
void PrintGStats(const char s[], PNEAGraph Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph->GetNodes(), Graph->GetEdges(),
      Graph->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  PNEAGraph Graph;

  Graph = TNEAGraph::New();
  PrintGStats("DefaultConstructor:Graph",Graph);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 1000;
  int NEdges = 100000;
  const char *FName = "demo.graph.dat";

  PNEAGraph Graph;
  PNEAGraph Graph1;
  PNEAGraph Graph2;
  int i;
  int n;
  int NCount;
  int ECount1;
  int ECount2;
  int x,y;
  bool t;

  Graph = TNEAGraph::New();
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
    n = Graph->AddEdge(x, y);
    NCount--;
  }
  PrintGStats("ManipulateNodesEdges:Graph",Graph);

  // get all the nodes
  NCount = 0;
  for (TNEAGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }

  // get all the edges for all the nodes
  ECount1 = 0;
  for (TNEAGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ECount1++;
    }
  }

  // get all the edges directly
  ECount2 = 0;
  for (TNEAGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ECount2++;
  }
  printf("graph ManipulateNodesEdges:Graph, nodes %d, edges1 %d, edges2 %d\n",
      NCount, ECount1, ECount2);

  // assignment
  Graph1 = TNEAGraph::New();
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
    Graph2 = TNEAGraph::Load(FIn);
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

// Test node attribute functionality
void ManipulateNodeAttributes() {
  int NNodes = 1000;

  PNEAGraph Graph;
  int i;
  int n;
  bool t;

  Graph = TNEAGraph::New();
  t = Graph->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  t = Graph->Empty();
  n = Graph->GetNodes();

  // create attributes and fill all nodes
  TStr attr1 = "str";
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr4 = "default";

  // Test vertical int iterator for node 3, 50, 700, 900
  Graph->AddIntAttrDat(3, 3*2, attr2);
  Graph->AddIntAttrDat(50, 50*2, attr2);
  Graph->AddIntAttrDat(700, 700*2, attr2);
  Graph->AddIntAttrDat(900, 900*2, attr2);
  int NodeId = 0;
  for (TNEAGraph::TNodeAIntI NI = Graph->BegNAIntI(attr2);
    NI < Graph->EndNAIntI(attr2); NI++) {
      printf("Attribute: %s, Node: %i, Val: %i\n", attr2(), NodeId, NI.GetDat()());
      NodeId++;
  } 

  // Test vertical flt iterator for node 3, 50, 700, 900
  Graph->AddFltAttrDat(5, 3.41, attr3);
  Graph->AddFltAttrDat(50, 2.718, attr3);
  Graph->AddFltAttrDat(300, 150.0, attr3);
  Graph->AddFltAttrDat(653, 653, attr3);
  NodeId = 0;
  for (TNEAGraph::TNodeAFltI NI = Graph->BegNAFltI(attr3);
    NI < Graph->EndNAFltI(attr3); NI++) {
      printf("Attribute: %s, Node: %i, Val: %f\n", attr3(), NodeId, NI.GetDat()());
      NodeId++;
  } 

  // Test vertical str iterator for node 3, 50, 700, 900
  Graph->AddStrAttrDat(10, "abc", attr1);
  Graph->AddStrAttrDat(20, "def", attr1);
  Graph->AddStrAttrDat(400, "ghi", attr1);
  // this does not show since ""=null
  Graph->AddStrAttrDat(455, "", attr1);
  NodeId = 0;
  
  for (TNEAGraph::TNodeAStrI NI = Graph->BegNAStrI(attr1);
    NI < Graph->EndNAStrI(attr1); NI++) {
      printf("Attribute: %s, Node: %i, Val: %s\n", attr1(), NodeId, NI.GetDat()());
      NodeId++;
  } 

  // Test vertical iterator over many types (must skip default/deleted attr) 
  int NId = 55;
  Graph->AddStrAttrDat(NId, "aaa", attr1);
  Graph->AddIntAttrDat(NId, 3*2, attr2);
  Graph->AddFltAttrDat(NId, 3.41, attr3);
  Graph->AddStrAttrDat(80, "dont appear", attr4); // should not show up
  for (TNEAGraph::TAttrNI NI = Graph->BegAttrNI(NId);
    NI < Graph->EndAttrNI(NId); NI++) {
    printf("Vertical Node: %i, Attr: %s\n", NId, NI.GetDat()());
  } 

  Graph->DelAttrDat(NId, attr2);
  for (TNEAGraph::TAttrNI NI = Graph->BegAttrNI(NId);
    NI < Graph->EndAttrNI(NId); NI++) {
    printf("Vertical Node (no int) : %i, Attr: %s\n", NId, NI.GetDat()());
  } 

  Graph->AddIntAttrDat(NId, 3*2, attr2);
  Graph->DelAttr(attr1);
  for (TNEAGraph::TAttrNI NI = Graph->BegAttrNI(NId);
    NI < Graph->EndAttrNI(NId); NI++) {
    printf("Vertical Node (no str) : %i, Attr: %s\n", NId, NI.GetDat()());
  } 

  for (i = 0; i <NNodes; i++) {
    Graph->AddIntAttrDat(i, 70, attr2);
  }

  int total = 0;
  for (TNEAGraph::TNodeAIntI NI = Graph->BegNAIntI(attr2);
    NI < Graph->EndNAIntI(attr2); NI++) {
    total += NI.GetDat();
  }

  printf("Average: %i (should be 70)\n", total/NNodes);

  Graph->Clr();
}


// Test small graph
void GetSmallGraph() {
  PNEAGraph Graph;

  return;

  //Graph = TNEAGraph::GetSmallGraph();
  PrintGStats("GetSmallGraph:Graph",Graph);
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateNodesEdges();
  ManipulateNodeAttributes();
  GetSmallGraph();
}

