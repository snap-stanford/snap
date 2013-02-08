#include <gtest/gtest.h>

#include "Snap.h"

/// returns a graph with a single node
PNGraph GetSingleNode() {
  PNGraph G = TNGraph::New();
  G->AddNode(0);
  return G;
}

/// returns a perfect binary tree
PNGraph GetTree() {
  PNGraph G = TNGraph::New();
  for (int i = 0; i < 15; i++) {
    G->AddNode(i);
  }
  for (int i = 1; i < 15; i++) {
    G->AddEdge(i,i/2);
  }
  return G;
}

/// returns a perfect binary tree minus one edge
PNGraph GetForest() {
  PNGraph G = GetTree();
  G->DelEdge(4,2);
  return G;
}

/// returns a perfect binary tree plus one edge
PNGraph GetCircle() {
  PNGraph G = GetTree();
  G->AddEdge(7,2);
  return G;
}

// TSnap::IsTree
TEST(alg, IsTree) {
  PNGraph Graph;
  int RootNId;

  Graph = GetSingleNode();
  EXPECT_EQ(1,TSnap::IsTree(Graph,RootNId));

  Graph = GetTree();
  EXPECT_EQ(1,TSnap::IsTree(Graph,RootNId));

  Graph = GetForest();
  EXPECT_EQ(0,TSnap::IsTree(Graph,RootNId));

  Graph = GetCircle();
  EXPECT_EQ(0,TSnap::IsTree(Graph,RootNId));
}

