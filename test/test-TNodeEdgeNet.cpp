#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TNodeEdgeNet, DefaultConstructor) {
  TPt <TNodeEdgeNet<TInt, TInt> > Net;

  Net = TNodeEdgeNet<TInt, TInt>::New();

  EXPECT_EQ(0,Net->GetNodes());
  EXPECT_EQ(0,Net->GetEdges());

  EXPECT_EQ(1,Net->IsOk());
  EXPECT_EQ(1,Net->Empty());
  EXPECT_EQ(1,Net->HasFlag(gfDirected));
  EXPECT_EQ(1,Net->HasFlag(gfNodeDat));
}

// Test node, edge creation
TEST(TNodeEdgeNet, ManipulateNodesEdges) {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "test.net.dat";

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Net = TNodeEdgeNet<TInt, TInt>::New();
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
    n = Net->AddEdge(x, y);
    NCount--;
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator
  NCount = 0;
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges,NCount);

  // edges iterator
  NCount = 0;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg+OutDeg);
  }

  // assignment
  Net1 = TNodeEdgeNet<TInt, TInt>::New();
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
    Net2 = TNodeEdgeNet<TInt, TInt>::Load(FIn);
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
TEST(TNodeEdgeNet, SetNodeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;

  Net = TNodeEdgeNet<TInt, TInt>::New();
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
    n = Net->AddEdge(x, y);
    NCount--;
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()+10);
  }

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()+10, Net->GetNDat(NI.GetId()));
  }
}

// Test update node data
TEST(TNodeEdgeNet, UpdateNodeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;

  Net = TNodeEdgeNet<TInt, TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes
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
    n = Net->AddEdge(x, y);
    NCount--;
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()+5, Net->GetNDat(NI.GetId()));
  }

  // update node data, node ID + 10
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()+10);
  }

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()+10, Net->GetNDat(NI.GetId()));
  }
}

// Test set edge data
TEST(TNodeEdgeNet, SetEdgeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;

  Net = TNodeEdgeNet<TInt, TInt>::New();
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
    n = Net->AddEdge(x, y);
    // printf("0a %d %d %d\n",x,y,n);
    NCount--;
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

  // add data to nodes, square of node ID
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()*NI.GetId());
  }

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()*NI.GetId(), Net->GetNDat(NI.GetId()));
  }

  // set edge data, source node ID * dest node ID
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Net->SetEDat(EI.GetId(),EI.GetSrcNId()*EI.GetDstNId());
  }

  // verify edge data
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId()*EI.GetDstNId(), Net->GetEDat(EI.GetId()));
  }

  // set edge data to 42.
  Net->SetAllEDat(42);

  // verify edge data
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    EXPECT_EQ(42, Net->GetEDat(EI.GetId()));
  }

  // test node data again
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()*NI.GetId(), Net->GetNDat(NI.GetId()));
  }
}

// Test update edge data
TEST(TNodeEdgeNet, UpdateEdgeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;

  Net = TNodeEdgeNet<TInt, TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Net->AddNode(i);
  }
  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(NNodes,Net->GetNodes());

  // create random edges and edge data x+y+10
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    n = Net->AddEdge(x, y, -1, x+y+10);
    // printf("0a %d %d %d\n",x,y,n);
    NCount--;
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

  // add data to nodes, square of node ID
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Net->SetNDat(NI.GetId(), NI.GetId()*NI.GetId());
  }

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()*NI.GetId(), Net->GetNDat(NI.GetId()));
  }

  // verify edge data, x+y+10
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId()+EI.GetDstNId()+10, Net->GetEDat(EI.GetId()));
  }

  // update edge data, x+y+5
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Net->SetEDat(EI.GetId(),EI.GetSrcNId()+EI.GetDstNId()+5);
  }

  // verify edge data, x+y+5
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    EXPECT_EQ(EI.GetSrcNId()+EI.GetDstNId()+5, Net->GetEDat(EI.GetId()));
  }

  // test node data again
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ(NI.GetId()*NI.GetId(), Net->GetNDat(NI.GetId()));
  }
}

// Test node data sorting
TEST(TNodeEdgeNet, SortNodeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int NCount;
  int x,y;
  bool Sorted;
  int Min;
  int Value;

  Net = TNodeEdgeNet<TInt, TInt>::New();
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
    n = Net->AddEdge(x, y);
    NCount--;
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    // Net->AddNode(NI.GetId(), (NI.GetId()*NI.GetId()) % NNodes);
    Net->SetNDat(NI.GetId(), NI.GetId()*NI.GetId() % NNodes);
  }

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ((NI.GetId()*NI.GetId()) % NNodes, Net->GetNDat(NI.GetId()));
  }

  // test sorting of node IDs (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    Value = NI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  EXPECT_EQ(false,Sorted);

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ((NI.GetId()*NI.GetId()) % NNodes, Net->GetNDat(NI.GetId()));
  }
}

// Test edge data sorting
TEST(TNodeEdgeNet, SortEdgeData) {
  int NNodes = 10000;
  int NEdges = 100000;

  TPt <TNodeEdgeNet<TInt, TInt> > Net;
  TPt <TNodeEdgeNet<TInt, TInt> > Net1;
  TPt <TNodeEdgeNet<TInt, TInt> > Net2;
  int i;
  int n;
  int x,y;
  bool Sorted;
  int Min;
  int Value;

  Net = TNodeEdgeNet<TInt, TInt>::New();
  EXPECT_EQ(1,Net->Empty());

  // create the nodes with node data x*x % NNodes
  for (i = 0; i < NNodes; i++) {
    x = (i*13) % NNodes;
    Net->AddNode(x, (x*x) % NNodes);
  }
  EXPECT_EQ(0,Net->Empty());
  EXPECT_EQ(NNodes,Net->GetNodes());

  // create random edges with edge data x*y % NEdges
  for (i = 0; i < NEdges; i++) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    n = Net->AddEdge(x, y, (i*37) % NEdges, (x*y) % NEdges);
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
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ((NI.GetId()*NI.GetId()) % NNodes, Net->GetNDat(NI.GetId()));
  }

  // test edge data
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    EXPECT_EQ((EI.GetSrcNId()*EI.GetDstNId()) % NEdges, Net->GetEDat(EI.GetId()));
  }

  // test sorting of edge IDs (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Value = EI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  EXPECT_EQ(false,Sorted);

  // sort the nodes by edge IDs (sorted)
  Net->SortEIdById();

  // test sorting of edge IDs
  Min = -1;
  Sorted = true;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Value = EI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  EXPECT_EQ(true,Sorted);

  // test sorting of edge data (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Value = Net->GetEDat(EI.GetId());
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  EXPECT_EQ(false,Sorted);

  // sort the nodes by edge data
  Net->SortEIdByDat();

  // test sorting of edge data (sorted)
  Min = -1;
  Sorted = true;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Value = Net->GetEDat(EI.GetId());
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  EXPECT_EQ(true,Sorted);

  // test sorting of edge IDs (unsorted)
  Min = -1;
  Sorted = true;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    Value = EI.GetId();
    if (Min > Value) {
      Sorted = false;
    }
    Min = Value;
  }
  EXPECT_EQ(false,Sorted);

  // test edge data
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    EXPECT_EQ((EI.GetSrcNId()*EI.GetDstNId()) % NEdges, Net->GetEDat(EI.GetId()));
  }

  // test node data
  for (TNodeEdgeNet<TInt, TInt>::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    EXPECT_EQ((NI.GetId()*NI.GetId()) % NNodes, Net->GetNDat(NI.GetId()));
  }
}

// Test small graph
TEST(TNodeEdgeNet, GetSmallGraph) {
  // not implemented
}

