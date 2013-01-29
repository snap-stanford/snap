#include "Snap.h"

// Print network statistics
void PrintNStats(const char s[], TPt <TNodeEDatNet<TInt, TInt> > Net) {
  printf("network %s, nodes %d, edges %d, empty %s\n",
      s, Net->GetNodes(), Net->GetEdges(),
      Net->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  TPt <TNodeEDatNet<TInt, TInt> > Net;

  Net = TNodeEDatNet<TInt, TInt>::New();
  PrintNStats("DefaultConstructor:Net", Net);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "demo.net.dat";

  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int ECount1;
  int ECount2;
  int x,y;
  bool t;

  Net = TNodeEDatNet<TInt, TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  t = Net->Empty();
  n = Net->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Net->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Net->IsEdge(x,y)) {
      n = Net->AddEdge(x, y);
      NCount--;
    }
  }
  PrintNStats("ManipulateNodesEdges:Net", Net);

  // get all the nodes
  NCount = 0;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NCount++;
  }

  // get all the edges for all the nodes
  ECount1 = 0;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ECount1++;
    }
  }

  // get all the edges directly
  ECount2 = 0;
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    ECount2++;
  }
  printf("network ManipulateNodesEdges:Net, nodes %d, edges1 %d, edges2 %d\n",
      NCount, ECount1, ECount2);

  // assignment
  Net1 = TNodeEDatNet<TInt, TInt>::New();
  *Net1 = *Net;
  PrintNStats("ManipulateNodesEdges:Net1",Net1);

  // save the network
  {
    TFOut FOut(FName);
    Net->Save(FOut);
    FOut.Flush();
  }

  // load the network
  {
    TFIn FIn(FName);
    Net2 = TNodeEDatNet<TInt, TInt>::Load(FIn);
  }
  PrintNStats("ManipulateNodesEdges:Net2",Net2);

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Net->GetRndNId();
    Net->DelNode(n);
  }
  PrintNStats("ManipulateNodesEdges:Net",Net);

  Net1->Clr();
  PrintNStats("ManipulateNodesEdges:Net1",Net1);
}

// Test set node data
void SetNodeData() {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int NodeId;
  int NodeDat;
  int Value;
  bool ok;

  Net = TNodeEDatNet<TInt, TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  t = Net->Empty();
  n = Net->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Net->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Net->IsEdge(x,y)) {
      n = Net->AddEdge(x, y);
      NCount--;
    }
  }
  PrintNStats("SetNodeData:Net", Net);

  // set node data, square of node ID
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeId = NI.GetId();
    NodeDat = NI.GetId()*NI.GetId();
    Net->SetNDat(NodeId, NodeDat);
  }

  // read and test node data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = NI.GetId()*NI.GetId();
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network SetNodeData:Net, status %s\n", (ok == true) ? "ok" : "ERROR");
}

// Test update node data
void UpdateNodeData() {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int NodeDat;
  int Value;
  bool ok;

  Net = TNodeEDatNet<TInt, TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i,i+5);
  }
  t = Net->Empty();
  n = Net->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Net->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Net->IsEdge(x,y)) {
      n = Net->AddEdge(x, y);
      NCount--;
    }
  }
  PrintNStats("UpdateNodeData:Net", Net);

  // read and test node data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = NI.GetId()+5;
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network UpdateNodeData:Net, status1 %s\n", (ok == true) ? "ok" : "ERROR");

  // update node data, node ID + 10
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()+10);
  }

  // read and test node data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = NI.GetId()+10;
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network UpdateNodeData:Net, status2 %s\n", (ok == true) ? "ok" : "ERROR");
}

// Test set edge data
void SetEdgeData() {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int SrcNId;
  int DstNId;
  int EdgeDat;
  int Value;
  bool ok;

  Net = TNodeEDatNet<TInt, TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  t = Net->Empty();
  n = Net->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Net->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Net->IsEdge(x,y)) {
      n = Net->AddEdge(x, y);
      NCount--;
    }
  }
  PrintNStats("SetEdgeData:Net", Net);

  // set edge data, source node ID * dest node ID
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Net->SetEDat(EI.GetSrcNId(),EI.GetDstNId(),EI.GetSrcNId()*EI.GetDstNId());
  }

  // verify edge data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    SrcNId = EI.GetSrcNId();
    DstNId = EI.GetDstNId();
    EdgeDat = Net->GetEDat(SrcNId, DstNId);
    Value = SrcNId*DstNId;
    if (EdgeDat != Value) {
      ok = false;
    }
  }
  printf("network SetEdgeData:Net, status1 %s\n", (ok == true) ? "ok" : "ERROR");

  // set edge data to 42.
  Net->SetAllEDat(42);

  // verify edge data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    SrcNId = EI.GetSrcNId();
    DstNId = EI.GetDstNId();
    EdgeDat = Net->GetEDat(SrcNId, DstNId);
    if (EdgeDat != 42) {
      ok = false;
    }
  }
  printf("network SetEdgeData:Net, status2 %s\n", (ok == true) ? "ok" : "ERROR");
}

// Test update edge data
void UpdateEdgeData() {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int SrcNId;
  int DstNId;
  int EdgeDat;
  int Value;
  bool ok;

  Net = TNodeEDatNet<TInt, TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  t = Net->Empty();
  n = Net->GetNodes();

  // create random edges and edge data x+y+10
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Net->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Net->IsEdge(x,y)) {
      n = Net->AddEdge(x, y, x+y+10);
      NCount--;
    }
  }
  PrintNStats("UpdateEdgeData:Net", Net);

  // verify edge data, x+y+10
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    SrcNId = EI.GetSrcNId();
    DstNId = EI.GetDstNId();
    EdgeDat = Net->GetEDat(SrcNId, DstNId);
    Value = SrcNId+DstNId+10;
    if (EdgeDat != Value) {
      ok = false;
    }
  }
  printf("network UpdateEdgeData:Net, status1 %s\n", (ok == true) ? "ok" : "ERROR");

  // update edge data, x+y+5
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Net->SetEDat(EI.GetSrcNId(),EI.GetDstNId(),EI.GetSrcNId()+EI.GetDstNId()+5);
  }

  // verify edge data, x+y+5
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    SrcNId = EI.GetSrcNId();
    DstNId = EI.GetDstNId();
    EdgeDat = Net->GetEDat(SrcNId, DstNId);
    Value = SrcNId+DstNId+5;
    if (EdgeDat != Value) {
      ok = false;
    }
  }
  printf("network UpdateEdgeData:Net, status2 %s\n", (ok == true) ? "ok" : "ERROR");
}

// Test node data sorting
void SortNodeData() {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEDatNet<TInt, TInt> > Net;
  TPt <TNodeEDatNet<TInt, TInt> > Net1;
  TPt <TNodeEDatNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int NodeId;
  int NodeDat;
  bool ok;
  bool Sorted;
  int Min;
  int Value;

  Net = TNodeEDatNet<TInt, TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode((i*13) % NNodes);
  }
  t = Net->Empty();
  n = Net->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Net->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Net->IsEdge(x,y)) {
      n = Net->AddEdge(x, y);
      NCount--;
    }
  }
  PrintNStats("SortNodeData:Net", Net);

  // add data to nodes, square of node ID % NNodes
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeId = NI.GetId();
    NodeDat = (NI.GetId()*NI.GetId()) % NNodes;
    Net->SetNDat(NodeId, NodeDat);
  }

  // test node data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = (NI.GetId()*NI.GetId()) % NNodes;
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network SortNodeData:Net, status1 %s\n", (ok == true) ? "ok" : "ERROR");

  // test sorting of node IDs (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = NI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  printf("network SortNodeData:Net, status2 %s\n", (Sorted == false) ? "ok" : "ERROR");

  // sort the nodes by node IDs (sorted)
  Net->SortNIdById();

  // test sorting of node IDs
  Min = -1;
  Sorted = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = NI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  printf("network SortNodeData:Net, status3 %s\n", (Sorted == true) ? "ok" : "ERROR");

  // test sorting of node data (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = Net->GetNDat(NI.GetId());
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  printf("network SortNodeData:Net, status4 %s\n", (Sorted == false) ? "ok" : "ERROR");

  // sort the nodes by node data
  Net->SortNIdByDat();

  // test sorting of node data (sorted)
  Min = -1;
  Sorted = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = Net->GetNDat(NI.GetId());
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  printf("network SortNodeData:Net, status5 %s\n", (Sorted == true) ? "ok" : "ERROR");

  // test sorting of node IDs (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = NI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  printf("network SortNodeData:Net, status6 %s\n", (Sorted == false) ? "ok" : "ERROR");

  // test node data
  ok = true;
  for (TNodeEDatNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = (NI.GetId()*NI.GetId()) % NNodes;
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network SortNodeData:Net, status7 %s\n", (ok == true) ? "ok" : "ERROR");
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateNodesEdges();
  SetNodeData();
  UpdateNodeData();
  SetEdgeData();
  UpdateEdgeData();
  SortNodeData();
}

