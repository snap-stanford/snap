#include "Snap.h"

// Print network statistics
void PrintNStats(const char s[], TPt <TNodeNet<TInt> > Net) {
  printf("network %s, nodes %d, edges %d, empty %s\n",
      s, Net->GetNodes(), Net->GetEdges(),
      Net->Empty() ? "yes" : "no");
}


// Test the default constructor
void DefaultConstructor() {
  TPt <TNodeNet<TInt> > Net;

  Net = TNodeNet<TInt>::New();
  PrintNStats("DefaultConstructor:Net", Net);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "demo.net.dat";

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int ECount1;
  int ECount2;
  int x,y;
  bool t;

  Net = TNodeNet<TInt>::New();
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NCount++;
  }

  // get all the edges for all the nodes
  ECount1 = 0;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ECount1++;
    }
  }

  // get all the edges directly
  ECount2 = 0;
  for (TNodeNet<TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    ECount2++;
  }
  printf("network ManipulateNodesEdges:Net, nodes %d, edges1 %d, edges2 %d\n",
      NCount, ECount1, ECount2);

  // assignment
  Net1 = TNodeNet<TInt>::New();
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
    Net2 = TNodeNet<TInt>::Load(FIn);
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

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int NodeId;
  int NodeDat;
  int Value;
  bool ok;

  Net = TNodeNet<TInt>::New();
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

  // add data to nodes, square of node ID
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeId = NI.GetId();
    NodeDat = NI.GetId()*NI.GetId();
    Net->SetNDat(NodeId, NodeDat);
  }

  // read and test node data
  ok = true;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool t;
  int NodeDat;
  int Value;
  bool ok;

  Net = TNodeNet<TInt>::New();
  t = Net->Empty();

  // create the nodes with node data
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = NI.GetId()+5;
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network UpdateNodeData:Net, status %s\n", (ok == true) ? "ok" : "ERROR");

  // update node data, node ID + 10
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()+10);
  }

  // read and test node data
  ok = true;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NodeDat = Net->GetNDat(NI.GetId());
    Value = NI.GetId()+10;
    if (NodeDat != Value) {
      ok = false;
    }
  }
  printf("network UpdateNodeData:Net, status %s\n", (ok == true) ? "ok" : "ERROR");
}


// Test data sorting
void SortNodeData() {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
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

  Net = TNodeNet<TInt>::New();
  t = Net->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    NodeId = (i*13) % NNodes;
    Net->AddNode((i*13) % NNodes, (NodeId*NodeId) % NNodes);
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

  // test node data
  ok = true;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = NI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  printf("network SortNodeData:Net, status6 %s\n", (Sorted == false) ? "ok" : "ERROR");

  // test node data
  ok = true;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  SortNodeData();
}

