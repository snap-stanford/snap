#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <proc/readproc.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <gtest/gtest.h>

#include <snap/Snap.h>

// Test the default constructor
TEST(TUNGraph, DefaultConstructor) {
  PUNGraph Graph;

  Graph = TUNGraph::New();

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());
  EXPECT_EQ(0,Graph->HasFlag(gfDirected));
}

// Test node, edge creation
TEST(TUNGraph, ManipulateNodesEdges) {
  int nnodes = 10000;
  int nedges = 100000;
  const char *fname = "test.graph";

  PUNGraph Graph;
  PUNGraph Graph1;
  PUNGraph Graph2;
  int i;
  int n;
  int ncount;
  int x,y;

  Graph = TUNGraph::New();
  EXPECT_EQ(1,Graph->Empty());

  // create the nodes
  for (i = 0; i < nnodes; i++) {
    Graph->AddNode(i);
  }
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(nnodes,Graph->GetNodes());

  // create random edges
  ncount = nedges;
  while (ncount > 0) {
    x = (long) (drand48() * nnodes);
    y = (long) (drand48() * nnodes);
    // Graph->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Graph->IsEdge(x,y)) {
      n = Graph->AddEdge(x, y);
      ncount--;
    }
  }

  EXPECT_EQ(nedges,Graph->GetEdges());

  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(1,Graph->IsOk());

  for (i = 0; i < nnodes; i++) {
    EXPECT_EQ(1,Graph->IsNode(i));
  }

  EXPECT_EQ(0,Graph->IsNode(nnodes));
  EXPECT_EQ(0,Graph->IsNode(nnodes+1));
  EXPECT_EQ(0,Graph->IsNode(2*nnodes));

  // iterators
  ncount = 0;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    ncount++;
  }
  EXPECT_EQ(nnodes,ncount);

  ncount = 0;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ncount++;
    }
  }
  EXPECT_EQ(nedges*2,ncount);

  ncount = 0;
  for (TUNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ncount++;
  }
  EXPECT_EQ(nedges,ncount);

  // assignment
  Graph1 = Graph;

  EXPECT_EQ(nnodes,Graph1->GetNodes());
  EXPECT_EQ(nedges,Graph1->GetEdges());
  EXPECT_EQ(0,Graph1->Empty());
  EXPECT_EQ(1,Graph1->IsOk());

  // saving and loading
  {
    TFOut FOut(fname);
    Graph->Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(fname);
    Graph2 = TUNGraph::Load(FIn);
  }

  EXPECT_EQ(nnodes,Graph2->GetNodes());
  EXPECT_EQ(nedges,Graph2->GetEdges());
  EXPECT_EQ(0,Graph2->Empty());
  EXPECT_EQ(1,Graph2->IsOk());

  // remove all the nodes and edges
  for (i = 0; i < nnodes; i++) {
    n = Graph->GetRndNId();
    Graph->DelNode(n);
  }

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());

  Graph1->Clr();

  EXPECT_EQ(0,Graph1->GetNodes());
  EXPECT_EQ(0,Graph1->GetEdges());

  EXPECT_EQ(1,Graph1->IsOk());
  EXPECT_EQ(1,Graph1->Empty());
}

// Test small graph
TEST(TUNGraph, GetSmallGraph) {
  PUNGraph Graph;

  Graph = TUNGraph::GetSmallGraph();

  EXPECT_EQ(5,Graph->GetNodes());
  EXPECT_EQ(5,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(0,Graph->HasFlag(gfDirected));
}

