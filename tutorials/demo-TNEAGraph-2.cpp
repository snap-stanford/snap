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
void ManipulateNodeEdgeAttributes() {
  int NNodes = 1000;
  int NEdges = 1000;

  PNEAGraph Graph;
  int i;
  int x, y;
  bool t;

  Graph = TNEAGraph::New();
  t = Graph->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }

  // create the edges 
  for (i = 0; i < NEdges; i++) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    Graph->AddEdge(x, y, i);
  }

  // create attributes and fill all nodes
  TStr attr1 = "str";
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr4 = "default";

  // Test vertical int iterator for node 3, 50, 700, 900
  Graph->AddIntAttrDatN(3, 3*2, attr2);
  Graph->AddIntAttrDatN(50, 50*2, attr2);
  Graph->AddIntAttrDatN(700, 700*2, attr2);
  Graph->AddIntAttrDatN(900, 900*2, attr2);
  int NodeId = 0;
  for (TNEAGraph::TAIntI NI = Graph->BegNAIntI(attr2);
    NI < Graph->EndNAIntI(attr2); NI++) {
      printf("Attribute: %s, Node: %i, Val: %i\n", attr2(), NodeId, NI.GetDat()());
      NodeId++;
  } 

  // Test vertical flt iterator for node 3, 50, 700, 900
  Graph->AddFltAttrDatN(5, 3.41, attr3);
  Graph->AddFltAttrDatN(50, 2.718, attr3);
  Graph->AddFltAttrDatN(300, 150.0, attr3);
  Graph->AddFltAttrDatN(653, 653, attr3);
  NodeId = 0;
  for (TNEAGraph::TAFltI NI = Graph->BegNAFltI(attr3);
    NI < Graph->EndNAFltI(attr3); NI++) {
      printf("Attribute: %s, Node: %i, Val: %f\n", attr3(), NodeId, NI.GetDat()());
      NodeId++;
  } 

  // Test vertical str iterator for node 3, 50, 700, 900
  Graph->AddStrAttrDatN(10, "abc", attr1);
  Graph->AddStrAttrDatN(20, "def", attr1);
  Graph->AddStrAttrDatN(400, "ghi", attr1);
  // this does not show since ""=null
  Graph->AddStrAttrDatN(455, "", attr1);
  NodeId = 0;
  
  for (TNEAGraph::TAStrI NI = Graph->BegNAStrI(attr1);
    NI < Graph->EndNAStrI(attr1); NI++) {
      printf("Attribute: %s, Node: %i, Val: %s\n", attr1(), NodeId, NI.GetDat()());
      NodeId++;
  } 

  // Test vertical iterator over many types (must skip default/deleted attr) 
  int NId = 55;
  Graph->AddStrAttrDatN(NId, "aaa", attr1);
  Graph->AddIntAttrDatN(NId, 3*2, attr2);
  Graph->AddFltAttrDatN(NId, 3.41, attr3);
  Graph->AddStrAttrDatN(80, "dont appear", attr4); // should not show up
  for (TNEAGraph::TAttrNI NI = Graph->BegAttrNI(NId);
    NI < Graph->EndAttrNI(NId); NI++) {
    printf("Vertical Node: %i, Attr: %s\n", NId, NI.GetDat()());
  } 

  Graph->DelAttrDatN(NId, attr2);
  for (TNEAGraph::TAttrNI NI = Graph->BegAttrNI(NId);
    NI < Graph->EndAttrNI(NId); NI++) {
    printf("Vertical Node (no int) : %i, Attr: %s\n", NId, NI.GetDat()());
  } 

  Graph->AddIntAttrDatN(NId, 3*2, attr2);
  Graph->DelAttrN(attr1);
  for (TNEAGraph::TAttrNI NI = Graph->BegAttrNI(NId);
    NI < Graph->EndAttrNI(NId); NI++) {
    printf("Vertical Node (no str) : %i, Attr: %s\n", NId, NI.GetDat()());
  } 

  for (i = 0; i <NNodes; i++) {
    Graph->AddIntAttrDatN(i, 70, attr2);
  }

  int total = 0;
  for (TNEAGraph::TAIntI NI = Graph->BegNAIntI(attr2);
    NI < Graph->EndNAIntI(attr2); NI++) {
    total += NI.GetDat();
  }

  printf("Average: %i (should be 70)\n", total/NNodes);

  // Test vertical int iterator for edge
  Graph->AddIntAttrDatE(3, 3*2, attr2);
  Graph->AddIntAttrDatE(55, 55*2, attr2);
  Graph->AddIntAttrDatE(705, 705*2, attr2);
  Graph->AddIntAttrDatE(905, 905*2, attr2);
  int EdgeId = 0;
  for (TNEAGraph::TAIntI EI = Graph->BegEAIntI(attr2);
    EI < Graph->EndEAIntI(attr2); EI++) {
      printf("E Attribute: %s, Edge: %i, Val: %i\n", attr2(), EdgeId, EI.GetDat()());
      EdgeId++;
  } 

  // Test vertical flt iterator for edge
  Graph->AddFltAttrDatE(5, 4.41, attr3);
  Graph->AddFltAttrDatE(50, 3.718, attr3);
  Graph->AddFltAttrDatE(300, 151.0, attr3);
  Graph->AddFltAttrDatE(653, 654, attr3);
  EdgeId = 0;
  for (TNEAGraph::TAFltI EI = Graph->BegEAFltI(attr3);
    EI < Graph->EndEAFltI(attr3); EI++) {
      printf("E Attribute: %s, Edge: %i, Val: %f\n", attr3(), EdgeId, EI.GetDat()());
      EdgeId++;
  } 

  // Test vertical str iterator for edge
  Graph->AddStrAttrDatE(10, "abc", attr1);
  Graph->AddStrAttrDatE(20, "def", attr1);
  Graph->AddStrAttrDatE(400, "ghi", attr1);
  // this does not show since ""=null
  Graph->AddStrAttrDatE(455, "", attr1);
  EdgeId = 0;
  for (TNEAGraph::TAStrI EI = Graph->BegEAStrI(attr1);
    EI < Graph->EndEAStrI(attr1); EI++) {
      printf("E Attribute: %s, Edge: %i, Val: %s\n", attr1(), EdgeId, EI.GetDat()());
      EdgeId++;
  } 

  // Test vertical iterator over many types (must skip default/deleted attr) 
  int EId = 55;
  Graph->AddStrAttrDatE(EId, "aaa", attr1);
  Graph->AddIntAttrDatE(EId, 3*2, attr2);
  Graph->AddFltAttrDatE(EId, 3.41, attr3);
  Graph->AddStrAttrDatE(80, "dont appear", attr4); // should not show up
  for (TNEAGraph::TAttrEI EI = Graph->BegAttrEI(EId);
    EI < Graph->EndAttrEI(EId); EI++) {
    printf("Vertical Edge: %i, Attr: %s\n", EId, EI.GetDat()());
  } 

  Graph->DelAttrDatE(EId, attr2);
  for (TNEAGraph::TAttrEI EI = Graph->BegAttrEI(EId);
    EI < Graph->EndAttrEI(EId); EI++) {
    printf("Vertical Edge (no int) : %i, Attr: %s\n", EId, EI.GetDat()());
  } 

  Graph->AddIntAttrDatE(EId, 3*2, attr2);
  Graph->DelAttrE(attr1);
  for (TNEAGraph::TAttrEI EI = Graph->BegAttrEI(EId);
    EI < Graph->EndAttrEI(EId); EI++) {
    printf("Vertical Edge (no str) : %i, Attr: %s\n", EId, EI.GetDat()());
  } 

  for (i = 0; i <NEdges; i++) {
    Graph->AddIntAttrDatE(i, 70, attr2);
  }

  total = 0;
  for (TNEAGraph::TAIntI EI = Graph->BegNAIntI(attr2);
    EI < Graph->EndNAIntI(attr2); EI++) {
    total += EI.GetDat();
  }

  printf("Average: %i (should be 70)\n", total/NEdges);

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
  ManipulateNodeEdgeAttributes();
  GetSmallGraph();
}

