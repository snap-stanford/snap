#include "Snap.h"

// Print graph statistics
void PrintGStats(const char s[], PUndirNet Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph->GetNodes(), Graph->GetEdges(),
      Graph->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  PUndirNet Graph;

  Graph = TUndirNet::New();
  PrintGStats("DefaultConstructor:Graph",Graph);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "demo.graph.dat";

  PUndirNet Graph;
  PUndirNet Graph1;
  PUndirNet Graph2;
  int i;
  int n;
  int NCount;
  int ECount1;
  int ECount2;
  int x,y;
  bool t;

  Graph = TUndirNet::New();
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
  for (TUndirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }

  // get all the edges for all the nodes
  ECount1 = 0;
  for (TUndirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ECount1++;
    }
  }
  ECount1 /= 2;

  // get all the edges directly
  ECount2 = 0;
  for (TUndirNet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ECount2++;
  }
  printf("graph ManipulateNodesEdges:Graph, nodes %d, edges1 %d, edges2 %d\n",
      NCount, ECount1, ECount2);

  // assignment
  Graph1 = TUndirNet::New();
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
    Graph2 = TUndirNet::Load(FIn);
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
void ManipulateNodeSparseAttributes() {
  PUndirNet Graph;
  Graph = TUndirNet::New();
  Graph->AddNode(0);

  // Add mappings for three node attributes
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrN(IntAttr, atInt, IntId);
  printf("Added attribute %s with id %d\n", IntAttr.CStr(), IntId.Val);

  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrN(FltAttr, atFlt, FltId);
  printf("Added attribute %s with id %d\n", FltAttr.CStr(), FltId.Val);

  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrN(StrAttr, atStr, StrId);
  printf("Added attribute %s with id %d\n", StrAttr.CStr(), StrId.Val);

  // Add values for attributes to node with id NId.
  TInt NId(0);
  TInt IntVal(5);
  Graph->AddSAttrDatN(NId, IntId, IntVal);
  TFlt FltVal(5.0);
  Graph->AddSAttrDatN(NId, FltId, FltVal);
  TStr StrVal("5");
  Graph->AddSAttrDatN(NId, StrId, StrVal);


  // Get values for the attributes for node with id NId.
  TInt IntVal2;
  Graph->GetSAttrDatN(NId, IntId, IntVal2);
  printf("Node %d has attribute, with id %d, with value %d.\n", NId.Val, IntId.Val, IntVal2.Val);
  TFlt FltVal2;
  Graph->GetSAttrDatN(NId, FltAttr, FltVal2);
  printf("Node %d has attribute, with id %d, with value %f.\n", NId.Val, FltId.Val, FltVal2.Val);
  TStr StrVal2;
  Graph->GetSAttrDatN(NId, StrId, StrVal2);
  printf("Node %d has attribute, with id %d, with value %s.\n", NId.Val, StrId.Val, StrVal2.CStr());


  // Get list of attributes for node with id NId.
  TAttrPrV AttrV;
  Graph->GetSAttrVN(NId, atInt, AttrV);
  printf("Node with id %d has %d int attributes.\n", NId.Val, AttrV.Len());
  Graph->GetSAttrVN(NId, atFlt, AttrV);
  printf("Node with id %d has %d flt attributes.\n", NId.Val, AttrV.Len());
  Graph->GetSAttrVN(NId, atStr, AttrV);
  printf("Node with id %d has %d str attributes.\n", NId.Val, AttrV.Len());
  //Graph->GetSAttrVN(NId, atAny, AttrV);
  //printf("Node with id %d has %d attributes.\n", NId.Val, AttrV.Len());

  // Delete all attributes for node with id NId (use either name or id).
  Graph->DelSAttrDatN(NId, IntAttr);
  Graph->DelSAttrDatN(NId, FltId);
  Graph->DelSAttrDatN(NId, StrAttr);

  //Get all nodes with given attribute
  for (int i = 1; i <= 10; i++) {
    Graph->AddNode(i);
    TInt Id(i);
    Graph->AddSAttrDatN(Id, IntId, IntVal);
    if (i%2 == 0) {
      Graph->AddSAttrDatN(Id, FltId, FltVal);
    }
  }
  Graph->AddSAttrDatN(NId, StrId, StrVal);

  TIntV IdV;
  Graph->GetIdVSAttrN(IntAttr, IdV);
  printf("%d nodes have attribute with name %s\n", IdV.Len(), IntAttr.CStr());

  Graph->GetIdVSAttrN(FltAttr, IdV);
  printf("%d nodes have attribute with name %s\n", IdV.Len(), FltAttr.CStr());

  Graph->GetIdVSAttrN(StrAttr, IdV);
  printf("%d nodes have attribute with name %s\n", IdV.Len(), StrAttr.CStr());
}

// Test edge sparse attribute functionality
void ManipulateEdgeSparseAttributes() {
  PUndirNet Graph;
  Graph = TUndirNet::New();
  int SrcNId = 0;
  int DstNId = 1;
  Graph->AddNode(SrcNId);
  Graph->AddNode(DstNId);
  Graph->AddEdge(SrcNId, DstNId);

  // Add mappings for three edge attributes
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrE(IntAttr, atInt, IntId);
  printf("Added attribute %s with id %d\n", IntAttr.CStr(), IntId.Val);

  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrE(FltAttr, atFlt, FltId);
  printf("Added attribute %s with id %d\n", FltAttr.CStr(), FltId.Val);

  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrE(StrAttr, atStr, StrId);
  printf("Added attribute %s with id %d\n", StrAttr.CStr(), StrId.Val);

  // Add values for attributes to edge with id (SrcNId, DstNId).
  TInt IntVal(5);
  Graph->AddSAttrDatE(SrcNId, DstNId, IntId, IntVal);
  TFlt FltVal(5.0);
  Graph->AddSAttrDatE(SrcNId, DstNId, FltId, FltVal);
  TStr StrVal("5");
  Graph->AddSAttrDatE(SrcNId, DstNId, StrId, StrVal);


  // Get values for the attributes for edge with id (SrcNId, DstNId).
  TInt IntVal2;
  Graph->GetSAttrDatE(SrcNId, DstNId, IntId, IntVal2);
  printf("Edge %d->%d has attribute, with id %d, with value %d.\n", SrcNId, DstNId, IntId.Val, IntVal2.Val);
  TFlt FltVal2;
  Graph->GetSAttrDatE(SrcNId, DstNId, FltAttr, FltVal2);
  printf("Edge %d->%d has attribute, with id %d, with value %f.\n", SrcNId, DstNId, FltId.Val, FltVal2.Val);
  TStr StrVal2;
  Graph->GetSAttrDatE(SrcNId, DstNId, StrId, StrVal2);
  printf("Edge %d->%d has attribute, with id %d, with value %s.\n", SrcNId, DstNId, StrId.Val, StrVal2.CStr());


  // Get list of attributes for edge with id (SrcNId, DstNId).
  TAttrPrV AttrV;
  Graph->GetSAttrVE(SrcNId, DstNId, atInt, AttrV);
  printf("Edge %d->%d has %d int attributes.\n", SrcNId, DstNId, AttrV.Len());
  Graph->GetSAttrVE(SrcNId, DstNId, atFlt, AttrV);
  printf("Edge %d->%d has %d flt attributes.\n", SrcNId, DstNId, AttrV.Len());
  Graph->GetSAttrVE(SrcNId, DstNId, atStr, AttrV);
  printf("Edge %d->%d has %d str attributes.\n", SrcNId, DstNId, AttrV.Len());
  //Graph->GetSAttrVE(SrcNId, DstNId, atAny, AttrV);
  //printf("Edge %d->%d has %d attributes.\n", SrcNId, DstNId, AttrV.Len());

  // Delete all attributes for edge with id (SrcNId, DstNId) (use either name or id).
  Graph->DelSAttrDatE(SrcNId, DstNId, IntAttr);
  Graph->DelSAttrDatE(SrcNId, DstNId, FltId);
  Graph->DelSAttrDatE(SrcNId, DstNId, StrAttr);

  //Get all edges with given attribute
  for (int i = 2; i < 12; i++) {
    Graph->AddNode(i);
    Graph->AddEdge(i-1, i);
    Graph->AddSAttrDatE(i-1, i, IntId, IntVal);
    if (i%2 == 0) {
      Graph->AddSAttrDatE(i-1,i, FltId, FltVal);
    }
  }
  Graph->AddSAttrDatE(SrcNId, DstNId, StrId, StrVal);

  TIntPrV IdV;
  Graph->GetIdVSAttrE(IntAttr, IdV);
  printf("%d edges have attribute with name %s\n", IdV.Len(), IntAttr.CStr());

  Graph->GetIdVSAttrE(FltAttr, IdV);
  printf("%d edges have attribute with name %s\n", IdV.Len(), FltAttr.CStr());

  Graph->GetIdVSAttrE(StrAttr, IdV);
  printf("%d edges have attribute with name %s\n", IdV.Len(), StrAttr.CStr());
}

// Test small graph
void GetSmallGraph() {
  PUndirNet Graph;

  Graph = TUndirNet::GetSmallGraph();
  PrintGStats("GetSmallGraph:Graph",Graph);
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateNodesEdges();
  ManipulateNodeSparseAttributes();
  ManipulateEdgeSparseAttributes();
  GetSmallGraph();
}


