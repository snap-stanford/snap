#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TNodeNet, DefaultConstructor) {
  TPt <TNodeNet<TInt> > Net;

  Net = TNodeNet<TInt>::New();

  EXPECT_EQ(0,Net->GetNodes());
  EXPECT_EQ(0,Net->GetEdges());

  EXPECT_EQ(1,Net->IsOk());
  EXPECT_EQ(1,Net->Empty());
  EXPECT_EQ(1,Net->HasFlag(gfDirected));
  EXPECT_EQ(1,Net->HasFlag(gfNodeDat));
}

// Test node, edge creation
TEST(TNodeNet, ManipulateNodesEdges) {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "test.net.dat";

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Net = TNodeNet<TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(NNodes,Net->GetNodes());

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

  EXPECT_EQ(NEdges,Net->GetEdges());

  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(1,Net->IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Net->IsNode(i));
  }

  EXPECT_EQ(0,Net->IsNode(NNodes));
  EXPECT_EQ(0,Net->IsNode(NNodes+1));
  EXPECT_EQ(0,Net->IsNode(2*NNodes));

  // nodes iterator
  NCount = 0;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator
  NCount = 0;
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges,NCount);

  // edges iterator
  NCount = 0;
  for (TNodeNet<TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg+OutDeg);
  }

  // assignment
  Net1 = TNodeNet<TInt>::New();
  *Net1 = *Net;

  EXPECT_EQ(NNodes,Net1->GetNodes());
  EXPECT_EQ(NEdges,Net1->GetEdges());
  EXPECT_EQ(0,Net1->Empty());
  EXPECT_EQ(1,Net1->IsOk());

  // saving and loading
  {
    TFOut FOut(FName);
    Net->Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    Net2 = TNodeNet<TInt>::Load(FIn);
  }

  EXPECT_EQ(NNodes,Net2->GetNodes());
  EXPECT_EQ(NEdges,Net2->GetEdges());
  EXPECT_EQ(0,Net2->Empty());
  EXPECT_EQ(1,Net2->IsOk());

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Net->GetRndNId();
    Net->DelNode(n);
  }

  EXPECT_EQ(0,Net->GetNodes());
  EXPECT_EQ(0,Net->GetEdges());

  EXPECT_EQ(1,Net->IsOk());
  EXPECT_EQ(1,Net->Empty());

  Net1->Clr();

  EXPECT_EQ(0,Net1->GetNodes());
  EXPECT_EQ(0,Net1->GetEdges());

  EXPECT_EQ(1,Net1->IsOk());
  EXPECT_EQ(1,Net1->Empty());
}

// Test set node data
TEST(TNodeNet, SetNodeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;

  Net = TNodeNet<TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(NNodes,Net->GetNodes());

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

  EXPECT_EQ(NEdges,Net->GetEdges());

  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(1,Net->IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Net->IsNode(i));
  }

  EXPECT_EQ(0,Net->IsNode(NNodes));
  EXPECT_EQ(0,Net->IsNode(NNodes+1));
  EXPECT_EQ(0,Net->IsNode(2*NNodes));

  // set node data, node ID + 10
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()+10);
  }

  // test node data
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()+10, Net->GetNDat(NI.GetId()));
  }
}

// Test update node data
TEST(TNodeNet, UpdateNodeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;

  Net = TNodeNet<TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes with node data
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i,i+5);
  }
  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(NNodes,Net->GetNodes());

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

  EXPECT_EQ(NEdges,Net->GetEdges());

  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(1,Net->IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Net->IsNode(i));
  }

  EXPECT_EQ(0,Net->IsNode(NNodes));
  EXPECT_EQ(0,Net->IsNode(NNodes+1));
  EXPECT_EQ(0,Net->IsNode(2*NNodes));

  // test node data
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()+5, Net->GetNDat(NI.GetId()));
  }

  // update node data, node ID + 10
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()+10);
  }

  // test node data
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()+10, Net->GetNDat(NI.GetId()));
  }
}

// Test node data sorting
TEST(TNodeNet, SortNodeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeNet<TInt> > Net;
  TPt <TNodeNet<TInt> > Net1;
  TPt <TNodeNet<TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool Sorted;
  int Min;
  int Value;

  Net = TNodeNet<TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode((i*13) % NNodes);
  }
  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(NNodes,Net->GetNodes());

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

  EXPECT_EQ(NEdges,Net->GetEdges());

  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(1,Net->IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Net->IsNode(i));
  }

  EXPECT_EQ(0,Net->IsNode(NNodes));
  EXPECT_EQ(0,Net->IsNode(NNodes+1));
  EXPECT_EQ(0,Net->IsNode(2*NNodes));

  // add data to nodes, square of node ID % NNodes
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), (NI.GetId()*NI.GetId()) % NNodes);
  }

  // test node data
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ((NI.GetId()*NI.GetId()) % NNodes, Net->GetNDat(NI.GetId()));
  }

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
  EXPECT_EQ(false,Sorted);

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
  EXPECT_EQ(true,Sorted);

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
  EXPECT_EQ(false,Sorted);

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
  EXPECT_EQ(true,Sorted);

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
  EXPECT_EQ(false,Sorted);

  // test node data
  for (TNodeNet<TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ((NI.GetId()*NI.GetId()) % NNodes, Net->GetNDat(NI.GetId()));
  }
}

// Test small graph
TEST(TNodeNet, GetSmallGraph) {
  // not implemented
}

