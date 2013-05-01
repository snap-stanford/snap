#include <gtest/gtest.h>
#include "Snap.h"

void CheckVectors(TIntPrV& Expected, TIntPrV& Actual);
void CheckVectors(TFltPrV& Expected, TFltPrV& Actual);
void CheckVectors(TIntSet& Expected, TIntSet& Actual);
void CheckVectors(TIntV& Expected, TIntV& Actual);
void CheckGraphs(TIntPrV& Expected, PUNGraph& Actual);
void CheckGraphs(TIntPrV& Expected, PNGraph& Actual);
void CheckGraphs(TIntPrV& Expected, PNEGraph& Actual);
void CheckNotInSet(int Key, const TIntSet& Keys);

/////////////////////////////////////////////////
// Test Algorithms
/////////////////////////////////////////////////

class GraphTest : public ::testing::Test {
 protected:

  PUNGraph TUNLoopGraph;
  PNGraph TNLoopGraph;
  PNEGraph TNELoopGraph;
  PUNGraph TUNReverseTree;
  PNGraph TNReverseTree;
  PNEGraph TNEReverseTree;
  PUNGraph TUNComplicatedGraph;
  PNGraph TNComplicatedGraph;
  PNEGraph TNEComplicatedGraph;

  virtual void SetUp() {
    TUNLoopGraph = GenerateLoopGraph(TUNGraph::New());
    TNLoopGraph = GenerateLoopGraph(TNGraph::New());
    TNELoopGraph = GenerateLoopGraph(TNEGraph::New());
    TUNReverseTree = GenerateReverseTree(TUNGraph::New());
    TNReverseTree = GenerateReverseTree(TNGraph::New());
    TNEReverseTree = GenerateReverseTree(TNEGraph::New());
    TUNComplicatedGraph = GenerateComplicatedGraph(TUNGraph::New());
    TNComplicatedGraph = GenerateComplicatedGraph(TNGraph::New());
    TNEComplicatedGraph = GenerateComplicatedGraph(TNEGraph::New());
  }

  template<class PGraph>
  void GenerateNodes(const PGraph& Graph, int NumNodes = 5) {
    for (int i = 0; i < NumNodes; i++) {
      Graph->AddNode(i);
    }
  }

  // Returns the following graph:
  // 0 -> 1 -> 2 -> 3 -> 4
  //  \-----------------/ (two edges from 4 to 0 and one edge from 0 to 4)
  template<class PGraph>
  PGraph GenerateLoopGraph(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(1, 2);
    Graph->AddEdge(2, 3);
    Graph->AddEdge(3, 4);
    Graph->AddEdge(4, 0);
    Graph->AddEdge(4, 0);
    Graph->AddEdge(0, 4);
    return Graph;
  }

  // Returns the following graph:
  // 0 -> 1 -> 3
  //   \> 2 -> 4
  template<class PGraph>
  PGraph GenerateReverseTree(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(0, 2);
    Graph->AddEdge(1, 3);
    Graph->AddEdge(2, 4);
    return Graph;
  }

  // Returns a complicated graph with
  // no node 3, a self edge on node 0, and two edges from node 1 to node 4
  template<class PGraph>
  PGraph GenerateComplicatedGraph(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 0);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(0, 2);
    Graph->AddEdge(1, 2);
    Graph->AddEdge(2, 1);
    Graph->AddEdge(2, 4);
    Graph->AddEdge(1, 4);
    Graph->AddEdge(1, 4);
    return Graph;
  }
};

class TreeTest : public ::testing::Test {
 protected:

  PNGraph SingleNode;
  PNGraph BalancedTree;
  PNGraph Forest;
  PNGraph Circle;

  virtual void SetUp() {
    SingleNode = GetSingleNode();
    BalancedTree = GetTree();
    Forest = GetForest();
    Circle = GetCircle();
  }

  // Returns a graph with a single node
  PNGraph GetSingleNode() {
    PNGraph G = TNGraph::New();
    G->AddNode(1);
    return G;
  }


  // Returns a perfect binary tree of 15 nodes
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

  // Returns a perfect binary tree minus one edge
  PNGraph GetForest() {
    PNGraph G = GetTree();
    G->DelEdge(4,2);
    return G;
  }

  // Returns a perfect binary tree plus one edge
  PNGraph GetCircle() {
    PNGraph G = GetTree();
    G->AddEdge(7,2);
    return G;
  }
};

/////
// Node Degrees
//
// Note: not testing node degrees of graphs with self loops since
// the current implementations of node degrees do not take into
// account self-loops.
//
////

// Testing CntInDegNodes
TEST_F(GraphTest, CntInDegNodesTest) {

  // LoopGraph
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 1));
  EXPECT_EQ(5, TSnap::CntInDegNodes(TUNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntInDegNodes(TNLoopGraph, 0));
  EXPECT_EQ(4, TSnap::CntInDegNodes(TNLoopGraph, 1));
  EXPECT_EQ(1, TSnap::CntInDegNodes(TNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntInDegNodes(TNELoopGraph, 0));
  EXPECT_EQ(3, TSnap::CntInDegNodes(TNELoopGraph, 1));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TNELoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNELoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNELoopGraph, 4));

  // ReverseTree

  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TUNReverseTree, 1));
  EXPECT_EQ(3, TSnap::CntInDegNodes(TUNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNReverseTree, 4));

  EXPECT_EQ(1, TSnap::CntInDegNodes(TNReverseTree, 0));
  EXPECT_EQ(4, TSnap::CntInDegNodes(TNReverseTree, 1));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNReverseTree, 4));

  EXPECT_EQ(1, TSnap::CntInDegNodes(TNEReverseTree, 0));
  EXPECT_EQ(4, TSnap::CntInDegNodes(TNEReverseTree, 1));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEReverseTree, 4));
}

// Testing CntOutDegNodes
TEST_F(GraphTest, CntOutDegNodesTest) {

  // LoopGraph

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 1));
  EXPECT_EQ(5, TSnap::CntOutDegNodes(TUNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNLoopGraph, 0));
  EXPECT_EQ(4, TSnap::CntOutDegNodes(TNLoopGraph, 1));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNELoopGraph, 0));
  EXPECT_EQ(3, TSnap::CntOutDegNodes(TNELoopGraph, 1));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNELoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNELoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNELoopGraph, 4));

  // ReverseTree

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TUNReverseTree, 1));
  EXPECT_EQ(3, TSnap::CntOutDegNodes(TUNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNReverseTree, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNReverseTree, 1));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNReverseTree, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEReverseTree, 1));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNEReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEReverseTree, 4));
}

// Testing CntDegNodes
TEST_F(GraphTest, CntDegNodesTest) {

  // LoopGraph

  EXPECT_EQ(5, TSnap::CntDegNodes(TUNLoopGraph, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNLoopGraph, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNELoopGraph, 2));

  EXPECT_EQ(0, TSnap::CntDegNodes(TUNLoopGraph, 3));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntDegNodes(TNELoopGraph, 3));

  // ReverseTree

  EXPECT_EQ(3, TSnap::CntDegNodes(TUNReverseTree, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNReverseTree, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNEReverseTree, 2));

  EXPECT_EQ(2, TSnap::CntDegNodes(TUNReverseTree, 1));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNReverseTree, 1));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNEReverseTree, 1));
}

// Testing CntNonZNodes
TEST_F(GraphTest, CntNonZNodesTest) {
  EXPECT_EQ(5, TSnap::CntNonZNodes(TUNLoopGraph));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNLoopGraph));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNELoopGraph));

  EXPECT_EQ(5, TSnap::CntNonZNodes(TUNReverseTree));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNReverseTree));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNEReverseTree));
}

// Testing CntEdgesToSet
TEST_F(GraphTest, CntEdgesToSetTest) {
  TIntSet NodeKeysV;
  NodeKeysV.AddKey(0);
  NodeKeysV.AddKey(1);
  NodeKeysV.AddKey(2);
  NodeKeysV.AddKey(3);
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TUNLoopGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNLoopGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNELoopGraph, 4, NodeKeysV));

  EXPECT_EQ(1, TSnap::CntEdgesToSet(TUNReverseTree, 4, NodeKeysV));
  EXPECT_EQ(1, TSnap::CntEdgesToSet(TNReverseTree, 4, NodeKeysV));
  EXPECT_EQ(1, TSnap::CntEdgesToSet(TNEReverseTree, 4, NodeKeysV));
}

// Testing GetMxDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxDegNIdTest) {
  TIntSet IncorrectKeysLoopGraph;
  TIntSet IncorrectKeysReverseTree;

  // LoopGraph

  CheckNotInSet(TSnap::GetMxDegNId(TUNLoopGraph), IncorrectKeysLoopGraph);

  IncorrectKeysLoopGraph.AddKey(1);
  IncorrectKeysLoopGraph.AddKey(2);
  IncorrectKeysLoopGraph.AddKey(3);

  CheckNotInSet(TSnap::GetMxDegNId(TNLoopGraph), IncorrectKeysLoopGraph);
  CheckNotInSet(TSnap::GetMxDegNId(TNELoopGraph), IncorrectKeysLoopGraph);

  // ReverseTree

  IncorrectKeysReverseTree.AddKey(3);
  IncorrectKeysReverseTree.AddKey(4);

  CheckNotInSet(TSnap::GetMxDegNId(TUNReverseTree), IncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxDegNId(TNReverseTree), IncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxDegNId(TNEReverseTree), IncorrectKeysReverseTree);
}

// Testing GetMxInDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxInDegNIdTest) {
  TIntSet IncorrectKeysLoopGraph;
  TIntSet TUNIncorrectKeysReverseTree;
  TIntSet TNIncorrectKeysReverseTree;

  // LoopGraph

  CheckNotInSet(TSnap::GetMxInDegNId(TUNLoopGraph), IncorrectKeysLoopGraph);
  EXPECT_EQ(4, TSnap::GetMxInDegNId(TNLoopGraph));

  IncorrectKeysLoopGraph.AddKey(1);
  IncorrectKeysLoopGraph.AddKey(2);
  IncorrectKeysLoopGraph.AddKey(3);

  CheckNotInSet(TSnap::GetMxInDegNId(TNELoopGraph), IncorrectKeysLoopGraph);

  // ReverseTree

  TUNIncorrectKeysReverseTree.AddKey(3);
  TUNIncorrectKeysReverseTree.AddKey(4);
  TNIncorrectKeysReverseTree.AddKey(0);

  CheckNotInSet(TSnap::GetMxInDegNId(TUNReverseTree),
                TUNIncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxInDegNId(TNReverseTree),
                TNIncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxInDegNId(TNEReverseTree),
                TNIncorrectKeysReverseTree);
}

// Testing GetMxOutDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxOutDegNIdTest) {
  TIntSet IncorrectKeysLoopGraph;
  TIntSet IncorrectKeysReverseTree;

  // LoopGraph

  CheckNotInSet(TSnap::GetMxOutDegNId(TUNLoopGraph), IncorrectKeysLoopGraph);
  EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNLoopGraph));

  IncorrectKeysLoopGraph.AddKey(1);
  IncorrectKeysLoopGraph.AddKey(2);
  IncorrectKeysLoopGraph.AddKey(3);

  CheckNotInSet(TSnap::GetMxOutDegNId(TNELoopGraph), IncorrectKeysLoopGraph);

  // ReverseTree

  IncorrectKeysReverseTree.AddKey(3);
  IncorrectKeysReverseTree.AddKey(4);

  CheckNotInSet(TSnap::GetMxOutDegNId(TUNReverseTree),
                IncorrectKeysReverseTree);
  EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNReverseTree));
  EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNEReverseTree));
}

// Testing GetInDegCnt for TIntPrV
TEST_F(GraphTest, GetInDegCntIntTest) {
  TIntPrV TUNExpectedLoopGraph;
  TIntPrV TUNInDegCntLoopGraph;
  TIntPrV TNExpectedLoopGraph;
  TIntPrV TNInDegCntLoopGraph;
  TIntPrV TNEExpectedLoopGraph;
  TIntPrV TNEInDegCntLoopGraph;

  TIntPrV TUNExpectedReverseTree;
  TIntPrV TUNInDegCntReverseTree;
  TIntPrV TNExpectedReverseTree;
  TIntPrV TNInDegCntReverseTree;
  TIntPrV TNEExpectedReverseTree;
  TIntPrV TNEInDegCntReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TIntPr(2, 5));
  TSnap::GetInDegCnt(TUNLoopGraph, TUNInDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNInDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TIntPr(1, 4));
  TNExpectedLoopGraph.Add(TIntPr(2, 1));
  TSnap::GetInDegCnt(TNLoopGraph, TNInDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNInDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TIntPr(1, 3));
  TNEExpectedLoopGraph.Add(TIntPr(2, 2));
  TSnap::GetInDegCnt(TNELoopGraph, TNEInDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEInDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TIntPr(1, 2));
  TUNExpectedReverseTree.Add(TIntPr(2, 3));
  TSnap::GetInDegCnt(TUNReverseTree, TUNInDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNInDegCntReverseTree);

  TNExpectedReverseTree.Add(TIntPr(0, 1));
  TNExpectedReverseTree.Add(TIntPr(1, 4));
  TSnap::GetInDegCnt(TNReverseTree, TNInDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNInDegCntReverseTree);

  TNEExpectedReverseTree.Add(TIntPr(0, 1));
  TNEExpectedReverseTree.Add(TIntPr(1, 4));
  TSnap::GetInDegCnt(TNEReverseTree, TNEInDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEInDegCntReverseTree);
}

// Testing GetInDegCnt for TFltPrV
TEST_F(GraphTest, GetInDegCntFltTest) {
  TFltPrV TUNExpectedLoopGraph;
  TFltPrV TUNInDegCntLoopGraph;
  TFltPrV TNExpectedLoopGraph;
  TFltPrV TNInDegCntLoopGraph;
  TFltPrV TNEExpectedLoopGraph;
  TFltPrV TNEInDegCntLoopGraph;

  TFltPrV TUNExpectedReverseTree;
  TFltPrV TUNInDegCntReverseTree;
  TFltPrV TNExpectedReverseTree;
  TFltPrV TNInDegCntReverseTree;
  TFltPrV TNEExpectedReverseTree;
  TFltPrV TNEInDegCntReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TFltPr(2, 5));
  TSnap::GetInDegCnt(TUNLoopGraph, TUNInDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNInDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TFltPr(1, 4));
  TNExpectedLoopGraph.Add(TFltPr(2, 1));
  TSnap::GetInDegCnt(TNLoopGraph, TNInDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNInDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TFltPr(1, 3));
  TNEExpectedLoopGraph.Add(TFltPr(2, 2));
  TSnap::GetInDegCnt(TNELoopGraph, TNEInDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEInDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TFltPr(1, 2));
  TUNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetInDegCnt(TUNReverseTree, TUNInDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNInDegCntReverseTree);

  TNExpectedReverseTree.Add(TFltPr(0, 1));
  TNExpectedReverseTree.Add(TFltPr(1, 4));
  TSnap::GetInDegCnt(TNReverseTree, TNInDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNInDegCntReverseTree);

  TNEExpectedReverseTree.Add(TFltPr(0, 1));
  TNEExpectedReverseTree.Add(TFltPr(1, 4));
  TSnap::GetInDegCnt(TNEReverseTree, TNEInDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEInDegCntReverseTree);
}

// Testing GetOutDegCnt for TIntPrV
TEST_F(GraphTest, GetOutDegCntIntTest) {
  TIntPrV TUNExpectedLoopGraph;
  TIntPrV TUNOutDegCntLoopGraph;
  TIntPrV TNExpectedLoopGraph;
  TIntPrV TNOutDegCntLoopGraph;
  TIntPrV TNEExpectedLoopGraph;
  TIntPrV TNEOutDegCntLoopGraph;

  TIntPrV TUNExpectedReverseTree;
  TIntPrV TUNOutDegCntReverseTree;
  TIntPrV TNExpectedReverseTree;
  TIntPrV TNOutDegCntReverseTree;
  TIntPrV TNEExpectedReverseTree;
  TIntPrV TNEOutDegCntReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TIntPr(2, 5));
  TSnap::GetOutDegCnt(TUNLoopGraph, TUNOutDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNOutDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TIntPr(1, 4));
  TNExpectedLoopGraph.Add(TIntPr(2, 1));
  TSnap::GetOutDegCnt(TNLoopGraph, TNOutDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNOutDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TIntPr(1, 3));
  TNEExpectedLoopGraph.Add(TIntPr(2, 2));
  TSnap::GetOutDegCnt(TNELoopGraph, TNEOutDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEOutDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TIntPr(1, 2));
  TUNExpectedReverseTree.Add(TIntPr(2, 3));
  TSnap::GetOutDegCnt(TUNReverseTree, TUNOutDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNOutDegCntReverseTree);

  TNExpectedReverseTree.Add(TIntPr(0, 2));
  TNExpectedReverseTree.Add(TIntPr(1, 2));
  TNExpectedReverseTree.Add(TIntPr(2, 1));
  TSnap::GetOutDegCnt(TNReverseTree, TNOutDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNOutDegCntReverseTree);

  TNEExpectedReverseTree.Add(TIntPr(0, 2));
  TNEExpectedReverseTree.Add(TIntPr(1, 2));
  TNEExpectedReverseTree.Add(TIntPr(2, 1));
  TSnap::GetOutDegCnt(TNEReverseTree, TNEOutDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEOutDegCntReverseTree);
}

// Testing GetOutDegCnt for TFltPrV
TEST_F(GraphTest, GetOutDegCntFltTest) {
  TFltPrV TUNExpectedLoopGraph;
  TFltPrV TUNOutDegCntLoopGraph;
  TFltPrV TNExpectedLoopGraph;
  TFltPrV TNOutDegCntLoopGraph;
  TFltPrV TNEExpectedLoopGraph;
  TFltPrV TNEOutDegCntLoopGraph;

  TFltPrV TUNExpectedReverseTree;
  TFltPrV TUNOutDegCntReverseTree;
  TFltPrV TNExpectedReverseTree;
  TFltPrV TNOutDegCntReverseTree;
  TFltPrV TNEExpectedReverseTree;
  TFltPrV TNEOutDegCntReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TFltPr(2, 5));
  TSnap::GetOutDegCnt(TUNLoopGraph, TUNOutDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNOutDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TFltPr(1, 4));
  TNExpectedLoopGraph.Add(TFltPr(2, 1));
  TSnap::GetOutDegCnt(TNLoopGraph, TNOutDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNOutDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TFltPr(1, 3));
  TNEExpectedLoopGraph.Add(TFltPr(2, 2));
  TSnap::GetOutDegCnt(TNELoopGraph, TNEOutDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEOutDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TFltPr(1, 2));
  TUNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetOutDegCnt(TUNReverseTree, TUNOutDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNOutDegCntReverseTree);

  TNExpectedReverseTree.Add(TFltPr(0, 2));
  TNExpectedReverseTree.Add(TFltPr(1, 2));
  TNExpectedReverseTree.Add(TFltPr(2, 1));
  TSnap::GetOutDegCnt(TNReverseTree, TNOutDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNOutDegCntReverseTree);

  TNEExpectedReverseTree.Add(TFltPr(0, 2));
  TNEExpectedReverseTree.Add(TFltPr(1, 2));
  TNEExpectedReverseTree.Add(TFltPr(2, 1));
  TSnap::GetOutDegCnt(TNEReverseTree, TNEOutDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEOutDegCntReverseTree);
}

// Testing GetDegCnt for TIntPrV
TEST_F(GraphTest, GetDegCntIntTest) {
  TIntPrV TUNExpectedLoopGraph;
  TIntPrV TUNDegCntLoopGraph;
  TIntPrV TNExpectedLoopGraph;
  TIntPrV TNDegCntLoopGraph;
  TIntPrV TNEExpectedLoopGraph;
  TIntPrV TNEDegCntLoopGraph;

  TIntPrV TUNExpectedReverseTree;
  TIntPrV TUNDegCntReverseTree;
  TIntPrV TNExpectedReverseTree;
  TIntPrV TNDegCntReverseTree;
  TIntPrV TNEExpectedReverseTree;
  TIntPrV TNEDegCntReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TIntPr(2, 5));
  TSnap::GetDegCnt(TUNLoopGraph, TUNDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TIntPr(2, 3));
  TNExpectedLoopGraph.Add(TIntPr(3, 2));
  TSnap::GetDegCnt(TNLoopGraph, TNDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TIntPr(2, 3));
  TNEExpectedLoopGraph.Add(TIntPr(4, 2));
  TSnap::GetDegCnt(TNELoopGraph, TNEDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TIntPr(1, 2));
  TUNExpectedReverseTree.Add(TIntPr(2, 3));
  TSnap::GetDegCnt(TUNReverseTree, TUNDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNDegCntReverseTree);

  TNExpectedReverseTree.Add(TIntPr(1, 2));
  TNExpectedReverseTree.Add(TIntPr(2, 3));
  TSnap::GetDegCnt(TNReverseTree, TNDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNDegCntReverseTree);

  TNEExpectedReverseTree.Add(TIntPr(1, 2));
  TNEExpectedReverseTree.Add(TIntPr(2, 3));
  TSnap::GetDegCnt(TNEReverseTree, TNEDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEDegCntReverseTree);
}

// Testing GetDegCnt for TFltPrV
TEST_F(GraphTest, GetDegCntFltTest) {
  TFltPrV TUNExpectedLoopGraph;
  TFltPrV TUNDegCntLoopGraph;
  TFltPrV TNExpectedLoopGraph;
  TFltPrV TNDegCntLoopGraph;
  TFltPrV TNEExpectedLoopGraph;
  TFltPrV TNEDegCntLoopGraph;

  TFltPrV TUNExpectedReverseTree;
  TFltPrV TUNDegCntReverseTree;
  TFltPrV TNExpectedReverseTree;
  TFltPrV TNDegCntReverseTree;
  TFltPrV TNEExpectedReverseTree;
  TFltPrV TNEDegCntReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TFltPr(2, 5));
  TSnap::GetDegCnt(TUNLoopGraph, TUNDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TFltPr(2, 3));
  TNExpectedLoopGraph.Add(TFltPr(3, 2));
  TSnap::GetDegCnt(TNLoopGraph, TNDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TFltPr(2, 3));
  TNEExpectedLoopGraph.Add(TFltPr(4, 2));
  TSnap::GetDegCnt(TNELoopGraph, TNEDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TFltPr(1, 2));
  TUNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetDegCnt(TUNReverseTree, TUNDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNDegCntReverseTree);

  TNExpectedReverseTree.Add(TFltPr(1, 2));
  TNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetDegCnt(TNReverseTree, TNDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNDegCntReverseTree);

  TNEExpectedReverseTree.Add(TFltPr(1, 2));
  TNEExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetDegCnt(TNEReverseTree, TNEDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEDegCntReverseTree);
}

// Testing GetDegSeqV
TEST_F(GraphTest, GetDegSeqVTest) {
  TIntV TUNExpectedLoopGraph;
  TIntV TUNDegVLoopGraph;
  TIntV TNExpectedLoopGraph;
  TIntV TNDegVLoopGraph;
  TIntV TNEExpectedLoopGraph;
  TIntV TNEDegVLoopGraph;

  TIntV TUNExpectedReverseTree;
  TIntV TUNDegVReverseTree;
  TIntV TNExpectedReverseTree;
  TIntV TNDegVReverseTree;
  TIntV TNEExpectedReverseTree;
  TIntV TNEDegVReverseTree;

  // LoopGraph

  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TSnap::GetDegSeqV(TUNLoopGraph, TUNDegVLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNDegVLoopGraph);

  TNExpectedLoopGraph.Add(3);
  TNExpectedLoopGraph.Add(2);
  TNExpectedLoopGraph.Add(2);
  TNExpectedLoopGraph.Add(2);
  TNExpectedLoopGraph.Add(3);
  TSnap::GetDegSeqV(TNLoopGraph, TNDegVLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNDegVLoopGraph);

  TNEExpectedLoopGraph.Add(4);
  TNEExpectedLoopGraph.Add(2);
  TNEExpectedLoopGraph.Add(2);
  TNEExpectedLoopGraph.Add(2);
  TNEExpectedLoopGraph.Add(4);
  TSnap::GetDegSeqV(TNELoopGraph, TNEDegVLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEDegVLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(2);
  TUNExpectedReverseTree.Add(2);
  TUNExpectedReverseTree.Add(2);
  TUNExpectedReverseTree.Add(1);
  TUNExpectedReverseTree.Add(1);
  TSnap::GetDegSeqV(TUNReverseTree, TUNDegVReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNDegVReverseTree);

  TNExpectedReverseTree.Add(2);
  TNExpectedReverseTree.Add(2);
  TNExpectedReverseTree.Add(2);
  TNExpectedReverseTree.Add(1);
  TNExpectedReverseTree.Add(1);
  TSnap::GetDegSeqV(TNReverseTree, TNDegVReverseTree);
  CheckVectors(TNExpectedReverseTree, TNDegVReverseTree);

  TNEExpectedReverseTree.Add(2);
  TNEExpectedReverseTree.Add(2);
  TNEExpectedReverseTree.Add(2);
  TNEExpectedReverseTree.Add(1);
  TNEExpectedReverseTree.Add(1);
  TSnap::GetDegSeqV(TNEReverseTree, TNEDegVReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEDegVReverseTree);
}

// Testing GetDegSeqV with InDegV and OutDegV
TEST_F(GraphTest, GetDegSeqVInOutDegVTest) {
  TIntV TUNExpectedInDegVLoopGraph;
  TIntV TUNExpectedOutDegVLoopGraph;
  TIntV TUNInDegVLoopGraph;
  TIntV TUNOutDegVLoopGraph;
  TIntV TNExpectedInDegVLoopGraph;
  TIntV TNExpectedOutDegVLoopGraph;
  TIntV TNInDegVLoopGraph;
  TIntV TNOutDegVLoopGraph;
  TIntV TNEExpectedInDegVLoopGraph;
  TIntV TNEExpectedOutDegVLoopGraph;
  TIntV TNEInDegVLoopGraph;
  TIntV TNEOutDegVLoopGraph;

  TIntV TUNExpectedInDegVReverseTree;
  TIntV TUNExpectedOutDegVReverseTree;
  TIntV TUNInDegVReverseTree;
  TIntV TUNOutDegVReverseTree;
  TIntV TNExpectedInDegVReverseTree;
  TIntV TNExpectedOutDegVReverseTree;
  TIntV TNInDegVReverseTree;
  TIntV TNOutDegVReverseTree;
  TIntV TNEExpectedInDegVReverseTree;
  TIntV TNEExpectedOutDegVReverseTree;
  TIntV TNEInDegVReverseTree;
  TIntV TNEOutDegVReverseTree;

  // LoopGraph

  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TSnap::GetDegSeqV(TUNLoopGraph, TUNInDegVLoopGraph, TUNOutDegVLoopGraph);
  CheckVectors(TUNExpectedInDegVLoopGraph, TUNInDegVLoopGraph);
  CheckVectors(TUNExpectedOutDegVLoopGraph, TUNOutDegVLoopGraph);

  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(2);
  TNExpectedOutDegVLoopGraph.Add(2);
  TNExpectedOutDegVLoopGraph.Add(1);
  TNExpectedOutDegVLoopGraph.Add(1);
  TNExpectedOutDegVLoopGraph.Add(1);
  TNExpectedOutDegVLoopGraph.Add(1);
  TSnap::GetDegSeqV(TNLoopGraph, TNInDegVLoopGraph, TNOutDegVLoopGraph);
  CheckVectors(TNExpectedInDegVLoopGraph, TNInDegVLoopGraph);
  CheckVectors(TNExpectedOutDegVLoopGraph, TNOutDegVLoopGraph);

  TNEExpectedInDegVLoopGraph.Add(2);
  TNEExpectedInDegVLoopGraph.Add(1);
  TNEExpectedInDegVLoopGraph.Add(1);
  TNEExpectedInDegVLoopGraph.Add(1);
  TNEExpectedInDegVLoopGraph.Add(2);
  TNEExpectedOutDegVLoopGraph.Add(2);
  TNEExpectedOutDegVLoopGraph.Add(1);
  TNEExpectedOutDegVLoopGraph.Add(1);
  TNEExpectedOutDegVLoopGraph.Add(1);
  TNEExpectedOutDegVLoopGraph.Add(2);
  TSnap::GetDegSeqV(TNELoopGraph, TNEInDegVLoopGraph, TNEOutDegVLoopGraph);
  CheckVectors(TNEExpectedInDegVLoopGraph, TNEInDegVLoopGraph);
  CheckVectors(TNEExpectedOutDegVLoopGraph, TNEOutDegVLoopGraph);

  // ReverseTree

  TUNExpectedInDegVReverseTree.Add(2);
  TUNExpectedInDegVReverseTree.Add(2);
  TUNExpectedInDegVReverseTree.Add(2);
  TUNExpectedInDegVReverseTree.Add(1);
  TUNExpectedInDegVReverseTree.Add(1);
  TUNExpectedOutDegVReverseTree.Add(2);
  TUNExpectedOutDegVReverseTree.Add(2);
  TUNExpectedOutDegVReverseTree.Add(2);
  TUNExpectedOutDegVReverseTree.Add(1);
  TUNExpectedOutDegVReverseTree.Add(1);
  TSnap::GetDegSeqV(
      TUNReverseTree,
      TUNInDegVReverseTree,
      TUNOutDegVReverseTree);
  CheckVectors(TUNExpectedInDegVReverseTree, TUNInDegVReverseTree);
  CheckVectors(TUNExpectedOutDegVReverseTree, TUNOutDegVReverseTree);

  TNExpectedInDegVReverseTree.Add(0);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedOutDegVReverseTree.Add(2);
  TNExpectedOutDegVReverseTree.Add(1);
  TNExpectedOutDegVReverseTree.Add(1);
  TNExpectedOutDegVReverseTree.Add(0);
  TNExpectedOutDegVReverseTree.Add(0);
  TSnap::GetDegSeqV(TNReverseTree, TNInDegVReverseTree, TNOutDegVReverseTree);
  CheckVectors(TNExpectedInDegVReverseTree, TNInDegVReverseTree);
  CheckVectors(TNExpectedOutDegVReverseTree, TNOutDegVReverseTree);

  TNEExpectedInDegVReverseTree.Add(0);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedOutDegVReverseTree.Add(2);
  TNEExpectedOutDegVReverseTree.Add(1);
  TNEExpectedOutDegVReverseTree.Add(1);
  TNEExpectedOutDegVReverseTree.Add(0);
  TNEExpectedOutDegVReverseTree.Add(0);
  TSnap::GetDegSeqV(
      TNEReverseTree,
      TNEInDegVReverseTree,
      TNEOutDegVReverseTree);
  CheckVectors(TNEExpectedInDegVReverseTree, TNEInDegVReverseTree);
  CheckVectors(TNEExpectedOutDegVReverseTree, TNEOutDegVReverseTree);
}

// Testing GetNodeInDegV
TEST_F(GraphTest, GetNodeInDegVTest) {
  TIntPrV TUNExpectedInDegVLoopGraph;
  TIntPrV TUNInDegVLoopGraph;
  TIntPrV TNExpectedInDegVLoopGraph;
  TIntPrV TNInDegVLoopGraph;
  TIntPrV TNEExpectedInDegVLoopGraph;
  TIntPrV TNEInDegVLoopGraph;

  TIntPrV TUNExpectedInDegVReverseTree;
  TIntPrV TUNInDegVReverseTree;
  TIntPrV TNExpectedInDegVReverseTree;
  TIntPrV TNInDegVReverseTree;
  TIntPrV TNEExpectedInDegVReverseTree;
  TIntPrV TNEInDegVReverseTree;

  // LoopGraph

  TUNExpectedInDegVLoopGraph.Add(TIntPr(0, 2));
  TUNExpectedInDegVLoopGraph.Add(TIntPr(1, 2));
  TUNExpectedInDegVLoopGraph.Add(TIntPr(2, 2));
  TUNExpectedInDegVLoopGraph.Add(TIntPr(3, 2));
  TUNExpectedInDegVLoopGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeInDegV(TUNLoopGraph, TUNInDegVLoopGraph);
  CheckVectors(TUNExpectedInDegVLoopGraph, TUNInDegVLoopGraph);

  TNExpectedInDegVLoopGraph.Add(TIntPr(0, 1));
  TNExpectedInDegVLoopGraph.Add(TIntPr(1, 1));
  TNExpectedInDegVLoopGraph.Add(TIntPr(2, 1));
  TNExpectedInDegVLoopGraph.Add(TIntPr(3, 1));
  TNExpectedInDegVLoopGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeInDegV(TNLoopGraph, TNInDegVLoopGraph);
  CheckVectors(TNExpectedInDegVLoopGraph, TNInDegVLoopGraph);

  TNEExpectedInDegVLoopGraph.Add(TIntPr(0, 2));
  TNEExpectedInDegVLoopGraph.Add(TIntPr(1, 1));
  TNEExpectedInDegVLoopGraph.Add(TIntPr(2, 1));
  TNEExpectedInDegVLoopGraph.Add(TIntPr(3, 1));
  TNEExpectedInDegVLoopGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeInDegV(TNELoopGraph, TNEInDegVLoopGraph);
  CheckVectors(TNEExpectedInDegVLoopGraph, TNEInDegVLoopGraph);

  // ReverseTree

  TUNExpectedInDegVReverseTree.Add(TIntPr(0, 2));
  TUNExpectedInDegVReverseTree.Add(TIntPr(1, 2));
  TUNExpectedInDegVReverseTree.Add(TIntPr(2, 2));
  TUNExpectedInDegVReverseTree.Add(TIntPr(3, 1));
  TUNExpectedInDegVReverseTree.Add(TIntPr(4, 1));
  TSnap::GetNodeInDegV(TUNReverseTree, TUNInDegVReverseTree);
  CheckVectors(TUNExpectedInDegVReverseTree, TUNInDegVReverseTree);

  TNExpectedInDegVReverseTree.Add(TIntPr(0, 0));
  TNExpectedInDegVReverseTree.Add(TIntPr(1, 1));
  TNExpectedInDegVReverseTree.Add(TIntPr(2, 1));
  TNExpectedInDegVReverseTree.Add(TIntPr(3, 1));
  TNExpectedInDegVReverseTree.Add(TIntPr(4, 1));
  TSnap::GetNodeInDegV(TNReverseTree, TNInDegVReverseTree);
  CheckVectors(TNExpectedInDegVReverseTree, TNInDegVReverseTree);

  TNEExpectedInDegVReverseTree.Add(TIntPr(0, 0));
  TNEExpectedInDegVReverseTree.Add(TIntPr(1, 1));
  TNEExpectedInDegVReverseTree.Add(TIntPr(2, 1));
  TNEExpectedInDegVReverseTree.Add(TIntPr(3, 1));
  TNEExpectedInDegVReverseTree.Add(TIntPr(4, 1));
  TSnap::GetNodeInDegV(TNEReverseTree, TNEInDegVReverseTree);
  CheckVectors(TNEExpectedInDegVReverseTree, TNEInDegVReverseTree);
}

// Testing GetNodeOutDegV
TEST_F(GraphTest, GetNodeOutDegVTest) {
  TIntPrV TUNExpectedOutDegVLoopGraph;
  TIntPrV TUNOutDegVLoopGraph;
  TIntPrV TNExpectedOutDegVLoopGraph;
  TIntPrV TNOutDegVLoopGraph;
  TIntPrV TNEExpectedOutDegVLoopGraph;
  TIntPrV TNEOutDegVLoopGraph;

  TIntPrV TUNExpectedOutDegVReverseTree;
  TIntPrV TUNOutDegVReverseTree;
  TIntPrV TNExpectedOutDegVReverseTree;
  TIntPrV TNOutDegVReverseTree;
  TIntPrV TNEExpectedOutDegVReverseTree;
  TIntPrV TNEOutDegVReverseTree;

  // LoopGraph

  TUNExpectedOutDegVLoopGraph.Add(TIntPr(0, 2));
  TUNExpectedOutDegVLoopGraph.Add(TIntPr(1, 2));
  TUNExpectedOutDegVLoopGraph.Add(TIntPr(2, 2));
  TUNExpectedOutDegVLoopGraph.Add(TIntPr(3, 2));
  TUNExpectedOutDegVLoopGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeOutDegV(TUNLoopGraph, TUNOutDegVLoopGraph);
  CheckVectors(TUNExpectedOutDegVLoopGraph, TUNOutDegVLoopGraph);

  TNExpectedOutDegVLoopGraph.Add(TIntPr(0, 2));
  TNExpectedOutDegVLoopGraph.Add(TIntPr(1, 1));
  TNExpectedOutDegVLoopGraph.Add(TIntPr(2, 1));
  TNExpectedOutDegVLoopGraph.Add(TIntPr(3, 1));
  TNExpectedOutDegVLoopGraph.Add(TIntPr(4, 1));
  TSnap::GetNodeOutDegV(TNLoopGraph, TNOutDegVLoopGraph);
  CheckVectors(TNExpectedOutDegVLoopGraph, TNOutDegVLoopGraph);

  TNEExpectedOutDegVLoopGraph.Add(TIntPr(0, 2));
  TNEExpectedOutDegVLoopGraph.Add(TIntPr(1, 1));
  TNEExpectedOutDegVLoopGraph.Add(TIntPr(2, 1));
  TNEExpectedOutDegVLoopGraph.Add(TIntPr(3, 1));
  TNEExpectedOutDegVLoopGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeOutDegV(TNELoopGraph, TNEOutDegVLoopGraph);
  CheckVectors(TNEExpectedOutDegVLoopGraph, TNEOutDegVLoopGraph);

  // ReverseTree

  TUNExpectedOutDegVReverseTree.Add(TIntPr(0, 2));
  TUNExpectedOutDegVReverseTree.Add(TIntPr(1, 2));
  TUNExpectedOutDegVReverseTree.Add(TIntPr(2, 2));
  TUNExpectedOutDegVReverseTree.Add(TIntPr(3, 1));
  TUNExpectedOutDegVReverseTree.Add(TIntPr(4, 1));
  TSnap::GetNodeOutDegV(TUNReverseTree, TUNOutDegVReverseTree);
  CheckVectors(TUNExpectedOutDegVReverseTree, TUNOutDegVReverseTree);

  TNExpectedOutDegVReverseTree.Add(TIntPr(0, 2));
  TNExpectedOutDegVReverseTree.Add(TIntPr(1, 1));
  TNExpectedOutDegVReverseTree.Add(TIntPr(2, 1));
  TNExpectedOutDegVReverseTree.Add(TIntPr(3, 0));
  TNExpectedOutDegVReverseTree.Add(TIntPr(4, 0));
  TSnap::GetNodeOutDegV(TNReverseTree, TNOutDegVReverseTree);
  CheckVectors(TNExpectedOutDegVReverseTree, TNOutDegVReverseTree);

  TNEExpectedOutDegVReverseTree.Add(TIntPr(0, 2));
  TNEExpectedOutDegVReverseTree.Add(TIntPr(1, 1));
  TNEExpectedOutDegVReverseTree.Add(TIntPr(2, 1));
  TNEExpectedOutDegVReverseTree.Add(TIntPr(3, 0));
  TNEExpectedOutDegVReverseTree.Add(TIntPr(4, 0));
  TSnap::GetNodeOutDegV(TNEReverseTree, TNEOutDegVReverseTree);
  CheckVectors(TNEExpectedOutDegVReverseTree, TNEOutDegVReverseTree);
}

// Testing CntUniqUndirEdges
TEST_F(GraphTest, CntUniqUndirEdgesTest) {
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TUNLoopGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNLoopGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNELoopGraph));

  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TUNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TNEReverseTree));
}

// Testing CntUniqDirEdges
TEST_F(GraphTest, CntUniqDirEdgesTest) {
  EXPECT_EQ(10, TSnap::CntUniqDirEdges(TUNLoopGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNLoopGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNELoopGraph));

  EXPECT_EQ(8, TSnap::CntUniqDirEdges(TUNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqDirEdges(TNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqDirEdges(TNEReverseTree));
}

// Testing CntUniqBiDirEdges
TEST_F(GraphTest, CntUniqBiDirEdgesTest) {
  EXPECT_EQ(5, TSnap::CntUniqBiDirEdges(TUNLoopGraph));
  EXPECT_EQ(2, TSnap::CntUniqBiDirEdges(TNLoopGraph));
  EXPECT_EQ(2, TSnap::CntUniqBiDirEdges(TNELoopGraph));

  EXPECT_EQ(4, TSnap::CntUniqBiDirEdges(TUNReverseTree));
  EXPECT_EQ(0, TSnap::CntUniqBiDirEdges(TNReverseTree));
  EXPECT_EQ(0, TSnap::CntUniqBiDirEdges(TNEReverseTree));
}

// Testing CntSelfEdges
TEST_F(GraphTest, CntSelfEdgesTest) {
  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNLoopGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNLoopGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNELoopGraph));

  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNReverseTree));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNReverseTree));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNEReverseTree));
}

// Testing GetUnDir
TEST_F(GraphTest, GetUnDirTest) {
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;
  PUNGraph TUNGraphUnDir;
  PNGraph TNGraphUnDir;
  PNEGraph TNEGraphUnDir;

  // LoopGraph

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(4, 1));
  TNExpectedDegCnt.Add(TIntPr(6, 3));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(6, 3));
  TNEExpectedDegCnt.Add(TIntPr(8, 1));

  EXPECT_DEATH(TSnap::GetUnDir(TUNLoopGraph), "");
  TNGraphUnDir = TSnap::GetUnDir(TNLoopGraph);
  TNEGraphUnDir = TSnap::GetUnDir(TNELoopGraph);

  CheckGraphs(TNExpectedDegCnt, TNGraphUnDir);
  CheckGraphs(TNEExpectedDegCnt, TNEGraphUnDir);
}

// Testing MakeUnDir
TEST_F(GraphTest, MakeUnDirTest) {
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // LoopGraph

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(4, 1));
  TNExpectedDegCnt.Add(TIntPr(6, 3));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(6, 3));
  TNEExpectedDegCnt.Add(TIntPr(8, 1));

  EXPECT_DEATH(TSnap::GetUnDir(TUNLoopGraph), "");
  TSnap::MakeUnDir(TNLoopGraph);
  TSnap::MakeUnDir(TNELoopGraph);

  CheckGraphs(TNExpectedDegCnt, TNLoopGraph);
  CheckGraphs(TNEExpectedDegCnt, TNELoopGraph);
}

// Testing AddSelfEdges
TEST_F(GraphTest, AddSelfEdgesTest) {
  TIntPrV TUNExpectedDegCnt;
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TIntPr(1, 1));
  TUNExpectedDegCnt.Add(TIntPr(3, 2));
  TUNExpectedDegCnt.Add(TIntPr(4, 2));

  TNExpectedDegCnt.Add(TIntPr(2, 1));
  TNExpectedDegCnt.Add(TIntPr(4, 2));
  TNExpectedDegCnt.Add(TIntPr(6, 2));

  TNEExpectedDegCnt.Add(TIntPr(2, 1));
  TNEExpectedDegCnt.Add(TIntPr(4, 1));
  TNEExpectedDegCnt.Add(TIntPr(5, 1));
  TNEExpectedDegCnt.Add(TIntPr(6, 1));
  TNEExpectedDegCnt.Add(TIntPr(7, 1));

  TSnap::AddSelfEdges(TUNComplicatedGraph);
  TSnap::AddSelfEdges(TNComplicatedGraph);
  TSnap::AddSelfEdges(TNEComplicatedGraph);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

// Testing DelSelfEdges
TEST_F(GraphTest, DelSelfEdgesTest) {
  TIntPrV TUNExpectedDegCnt;
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TIntPr(0, 1));
  TUNExpectedDegCnt.Add(TIntPr(2, 2));
  TUNExpectedDegCnt.Add(TIntPr(3, 2));

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(2, 2));
  TNExpectedDegCnt.Add(TIntPr(4, 2));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(2, 1));
  TNEExpectedDegCnt.Add(TIntPr(3, 1));
  TNEExpectedDegCnt.Add(TIntPr(4, 1));
  TNEExpectedDegCnt.Add(TIntPr(5, 1));

  TSnap::DelSelfEdges(TUNComplicatedGraph);
  TSnap::DelSelfEdges(TNComplicatedGraph);
  TSnap::DelSelfEdges(TNEComplicatedGraph);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

// Testing DelNodes
TEST_F(GraphTest, DelNodesTest) {
  TIntV TUNDelNodes;
  TIntV TNDelNodes;
  TIntV TNEDelNodes;
  TIntPrV TUNExpectedDegCnt;
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNDelNodes.Add(0);
  TNDelNodes.Add(0);
  TNEDelNodes.Add(0);

  TUNExpectedDegCnt.Add(TIntPr(0, 1));
  TUNExpectedDegCnt.Add(TIntPr(2, 3));

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(2, 1));
  TNExpectedDegCnt.Add(TIntPr(3, 2));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(3, 2));
  TNEExpectedDegCnt.Add(TIntPr(4, 1));

  TSnap::DelNodes(TUNComplicatedGraph, TUNDelNodes);
  TSnap::DelNodes(TNComplicatedGraph, TNDelNodes);
  TSnap::DelNodes(TNEComplicatedGraph, TNEDelNodes);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}


// Testing DelZeroDegNodes
TEST_F(GraphTest, DelZeroDegNodesTest) {
  TIntPrV TUNExpectedDegCnt;
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TIntPr(2, 1));
  TUNExpectedDegCnt.Add(TIntPr(3, 3));

  TNExpectedDegCnt.Add(TIntPr(2, 1));
  TNExpectedDegCnt.Add(TIntPr(4, 3));

  TNEExpectedDegCnt.Add(TIntPr(3, 1));
  TNEExpectedDegCnt.Add(TIntPr(4, 2));
  TNEExpectedDegCnt.Add(TIntPr(5, 1));

  TSnap::DelZeroDegNodes(TUNComplicatedGraph);
  TSnap::DelZeroDegNodes(TNComplicatedGraph);
  TSnap::DelZeroDegNodes(TNEComplicatedGraph);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

// Testing DelDegKNodes
TEST_F(GraphTest, DelDegKNodesTest) {
  TIntPrV TUNExpectedDegCnt;
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TIntPr(0, 2));

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(2, 1));
  TNExpectedDegCnt.Add(TIntPr(3, 2));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(1, 2));

  TSnap::DelDegKNodes(TUNComplicatedGraph, 3, 3);
  TSnap::DelDegKNodes(TNComplicatedGraph, 3, 1);
  TSnap::DelDegKNodes(TNEComplicatedGraph, 3, 1);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

/////
// Tree Routines
////

// Testing IsTree
TEST_F(TreeTest, IsTreeTest) {
  int RootNId;

  EXPECT_TRUE(TSnap::IsTree(SingleNode, RootNId));
  EXPECT_TRUE(TSnap::IsTree(BalancedTree, RootNId));
  EXPECT_FALSE(TSnap::IsTree(Forest, RootNId));
  EXPECT_FALSE(TSnap::IsTree(Circle, RootNId));
}

// Testing GetTreeRootNId
TEST_F(TreeTest, GetTreeRootNIdTest) {
  EXPECT_EQ(1, TSnap::GetTreeRootNId(SingleNode));
  EXPECT_EQ(0, TSnap::GetTreeRootNId(BalancedTree));
  EXPECT_DEATH(TSnap::GetTreeRootNId(Forest), "");
  EXPECT_DEATH(TSnap::GetTreeRootNId(Circle), "");
}

// Testing GetTreeSig
TEST_F(TreeTest, GetTreeSigTest) {
  int RootNId;
  TIntV SingleNodeSig;
  TIntV BalancedTreeSig;
  TIntV ForestSig;
  TIntV CircleSig;

  TSnap::GetTreeSig(SingleNode, RootNId, SingleNodeSig);
//   TSnap::GetTreeSig(BalancedTree, RootNId, BalancedTreeSig);
//   TSnap::GetTreeSig(Forest, RootNId, ForestSig);
//   TSnap::GetTreeSig(Circle, RootNId, CircleSig);
}

// Testing GetTreeSig with node map
TEST_F(TreeTest, GetTreeSigTestWithNodeMap) {
  int RootNId;
  TIntV SingleNodeSig;
  TIntV BalancedTreeSig;
  TIntV ForestSig;
  TIntV CircleSig;
  TIntPrV SingleNodeNodeMap;
  TIntPrV BalancedTreeNodeMap;
  TIntPrV ForestNodeMap;
  TIntPrV CircleNodeMap;

//   TSnap::GetTreeSig(SingleNode, RootNId, SingleNodeSig, SingleNodeNodeMap);
//   TSnap::GetTreeSig(BalancedTree, RootNId, BalancedTreeSig, BalancedTreeNodeMap);
//   TSnap::GetTreeSig(Forest, RootNId, ForestSig, ForestTreeNodeMap);
//   TSnap::GetTreeSig(Circle, RootNId, CircleSig, CircleTreeNodeMap);
}

///
// HELPER METHODS
///

void CheckVectors(TIntPrV& Expected, TIntPrV& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val1.Val, Actual[i].Val1.Val);
    EXPECT_EQ(Expected[i].Val2.Val, Actual[i].Val2.Val);
  }
}

void CheckVectors(TFltPrV& Expected, TFltPrV& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val1.Val, Actual[i].Val1.Val);
    EXPECT_EQ(Expected[i].Val2.Val, Actual[i].Val2.Val);
  }
}

void CheckVectors(TIntV& Expected, TIntV& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val, Actual[i].Val);
  }
}

void CheckVectors(TIntSet& Expected, TIntSet& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val, Actual[i].Val);
  }
}

void CheckGraphs(TIntPrV& ExpectedDegCnt, PUNGraph& ActualGraph) {
  TIntPrV ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckGraphs(TIntPrV& ExpectedDegCnt, PNGraph& ActualGraph) {
  TIntPrV ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckGraphs(TIntPrV& ExpectedDegCnt, PNEGraph& ActualGraph) {
  TIntPrV ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckNotInSet(int Key, const TIntSet& Keys) {
  for (int i = 0; i < Keys.Len(); i++) {
    EXPECT_NE(Keys[i], Key);
  }
}
