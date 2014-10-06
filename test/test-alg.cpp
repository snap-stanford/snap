#include <gtest/gtest.h>
#include "Snap.h"

void CheckVectors(const TIntPrV& Expected, const TIntPrV& Actual);
void CheckVectors(const TFltPrV& Expected, const TFltPrV& Actual);
void CheckVectors(const TIntSet& Expected, const TIntSet& Actual);
void CheckVectors(const TIntV& Expected, const TIntV& Actual);
void CheckGraphs(const TIntPrV& Expected, const PUNGraph& Actual);
void CheckGraphs(const TIntPrV& Expected, const PNGraph& Actual);
void CheckGraphs(const TIntPrV& Expected, const PNEGraph& Actual);
void CheckNotInSet(int Key, const TIntSet& Keys);
void AddValues(TIntV& Vector, int NumValues, int Value);
void AddConsecutiveValues(TIntV& Vector, int Start, int End);
void GetKeys(TIntV& Keys, const TIntPrV& Map);

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
  PUNGraph TUNComplicatedGraphWithLoop;
  PNGraph TNComplicatedGraphWithLoop;
  PNEGraph TNEComplicatedGraphWithLoop;

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
    TUNComplicatedGraphWithLoop =
        GenerateComplicatedGraphWithLoop(TUNGraph::New());
    TNComplicatedGraphWithLoop =
        GenerateComplicatedGraphWithLoop(TNGraph::New());
    TNEComplicatedGraphWithLoop =
        GenerateComplicatedGraphWithLoop(TNEGraph::New());
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
  // no node 3 and two edges from node 1 to node 4
  template<class PGraph>
  PGraph GenerateComplicatedGraph(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(0, 2);
    Graph->AddEdge(1, 2);
    Graph->AddEdge(2, 1);
    Graph->AddEdge(2, 4);
    Graph->AddEdge(1, 4);
    Graph->AddEdge(1, 4);
    return Graph;
  }

  // Returns a complicated graph with
  // no node 3, a self-loop on node 0, and two edges from node 1 to node 4
  template<class PGraph>
  PGraph GenerateComplicatedGraphWithLoop(const PGraph& Graph) {
    GenerateComplicatedGraph(Graph);
    Graph->AddEdge(0, 0);
    return Graph;
  }
};

class TreeTest : public ::testing::Test {
 protected:

  PNGraph SingleNode;
  PNGraph Tree;
  PNGraph Forest;
  PNGraph Circle;

  virtual void SetUp() {
    SingleNode = GetSingleNode();
    Tree = GetTree();
    Forest = GetForest();
    Circle = GetCircle();
  }

  // Returns a graph with a single node
  PNGraph GetSingleNode() {
    PNGraph G = TNGraph::New();
    G->AddNode(1);
    return G;
  }


  // Returns a binary tree of 14 nodes
  PNGraph GetTree() {
    PNGraph G = TNGraph::New();
    for (int i = 1; i < 15; i++) {
      G->AddNode(i);
    }
    for (int i = 2; i < 15; i++) {
      G->AddEdge(i, i/2);
    }
    return G;
  }

  // Returns a binary tree minus one edge
  PNGraph GetForest() {
    PNGraph G = GetTree();
    G->DelEdge(4,2);
    return G;
  }

  // Returns a binary tree plus one edge
  PNGraph GetCircle() {
    PNGraph G = GetTree();
    G->AddEdge(7,2);
    return G;
  }
};

/////
// Node Degrees
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

  // ComplicatedGraph

  EXPECT_EQ(1, TSnap::CntInDegNodes(TUNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TUNComplicatedGraph, 2));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TUNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntInDegNodes(TNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNComplicatedGraph, 1));
  EXPECT_EQ(3, TSnap::CntInDegNodes(TNComplicatedGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntInDegNodes(TNEComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TNEComplicatedGraph, 2));
  EXPECT_EQ(1, TSnap::CntInDegNodes(TNEComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEComplicatedGraph, 4));
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

  // ComplicatedGraph

  EXPECT_EQ(1, TSnap::CntOutDegNodes(TUNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TUNComplicatedGraph, 2));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TUNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNComplicatedGraph, 1));
  EXPECT_EQ(3, TSnap::CntOutDegNodes(TNComplicatedGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEComplicatedGraph, 2));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNEComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEComplicatedGraph, 4));
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

  // ComplicatedGraph

  EXPECT_EQ(2, TSnap::CntDegNodes(TUNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntDegNodes(TNComplicatedGraph, 3));
  EXPECT_EQ(1, TSnap::CntDegNodes(TNEComplicatedGraph, 3));

  EXPECT_EQ(0, TSnap::CntDegNodes(TUNComplicatedGraph, 4));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNComplicatedGraph, 4));
  EXPECT_EQ(1, TSnap::CntDegNodes(TNEComplicatedGraph, 4));
}

// Testing CntNonZNodes
TEST_F(GraphTest, CntNonZNodesTest) {
  EXPECT_EQ(5, TSnap::CntNonZNodes(TUNLoopGraph));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNLoopGraph));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNELoopGraph));

  EXPECT_EQ(5, TSnap::CntNonZNodes(TUNReverseTree));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNReverseTree));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNEReverseTree));

  EXPECT_EQ(4, TSnap::CntNonZNodes(TUNComplicatedGraph));
  EXPECT_EQ(4, TSnap::CntNonZNodes(TNComplicatedGraph));
  EXPECT_EQ(4, TSnap::CntNonZNodes(TNEComplicatedGraph));
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

  EXPECT_EQ(2, TSnap::CntEdgesToSet(TUNComplicatedGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNComplicatedGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNEComplicatedGraph, 4, NodeKeysV));
}

// Testing GetMxDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxDegNIdTest) {
  TIntSet IncorrectKeysLoopGraph;
  TIntSet IncorrectKeysReverseTree;
  TIntSet IncorrectKeysComplicatedGraph;

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

  // ComplicatedGraph

  IncorrectKeysComplicatedGraph.AddKey(3);
  IncorrectKeysComplicatedGraph.AddKey(4);

  CheckNotInSet(TSnap::GetMxDegNId(TUNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  CheckNotInSet(TSnap::GetMxDegNId(TNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  EXPECT_EQ(1, TSnap::GetMxDegNId(TNEComplicatedGraph));
}

// Testing GetMxInDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxInDegNIdTest) {
  TIntSet IncorrectKeysLoopGraph;
  TIntSet TUNIncorrectKeysReverseTree;
  TIntSet TNIncorrectKeysReverseTree;
  TIntSet IncorrectKeysComplicatedGraph;

  // OP RS 2014/06/12 commented out this test, needs more investigation
  return;

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

  // ComplicatedGraph

  IncorrectKeysComplicatedGraph.AddKey(3);
  IncorrectKeysComplicatedGraph.AddKey(4);

  CheckNotInSet(TSnap::GetMxInDegNId(TUNComplicatedGraph),
                IncorrectKeysComplicatedGraph);

  IncorrectKeysComplicatedGraph.AddKey(0);

  CheckNotInSet(TSnap::GetMxInDegNId(TNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  EXPECT_EQ(4, TSnap::GetMxInDegNId(TNEComplicatedGraph));
}

// Testing GetMxOutDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxOutDegNIdTest) {
  TIntSet IncorrectKeysLoopGraph;
  TIntSet IncorrectKeysReverseTree;
  TIntSet IncorrectKeysComplicatedGraph;

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

  // ComplicatedGraph

  IncorrectKeysComplicatedGraph.AddKey(3);
  IncorrectKeysComplicatedGraph.AddKey(4);

  CheckNotInSet(TSnap::GetMxOutDegNId(TUNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  // OP RS 2014/06/12, commented out, since several nodes can be returned
  //EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNComplicatedGraph));

  IncorrectKeysComplicatedGraph.AddKey(2);

  CheckNotInSet(TSnap::GetMxOutDegNId(TNEComplicatedGraph),
                IncorrectKeysComplicatedGraph);
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

  TIntPrV TUNExpectedComplicatedGraph;
  TIntPrV TUNInDegCntComplicatedGraph;
  TIntPrV TNExpectedComplicatedGraph;
  TIntPrV TNInDegCntComplicatedGraph;
  TIntPrV TNEExpectedComplicatedGraph;
  TIntPrV TNEInDegCntComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TIntPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TIntPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TIntPr(3, 2));
  TSnap::GetInDegCnt(TUNComplicatedGraph, TUNInDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNInDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TIntPr(0, 2));
  TNExpectedComplicatedGraph.Add(TIntPr(2, 3));
  TSnap::GetInDegCnt(TNComplicatedGraph, TNInDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNInDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TIntPr(0, 2));
  TNEExpectedComplicatedGraph.Add(TIntPr(2, 2));
  TNEExpectedComplicatedGraph.Add(TIntPr(3, 1));
  TSnap::GetInDegCnt(TNEComplicatedGraph, TNEInDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEInDegCntComplicatedGraph);
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

  TFltPrV TUNExpectedComplicatedGraph;
  TFltPrV TUNInDegCntComplicatedGraph;
  TFltPrV TNExpectedComplicatedGraph;
  TFltPrV TNInDegCntComplicatedGraph;
  TFltPrV TNEExpectedComplicatedGraph;
  TFltPrV TNEInDegCntComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TFltPr(3, 2));
  TSnap::GetInDegCnt(TUNComplicatedGraph, TUNInDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNInDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNExpectedComplicatedGraph.Add(TFltPr(2, 3));
  TSnap::GetInDegCnt(TNComplicatedGraph, TNInDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNInDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(3, 1));
  TSnap::GetInDegCnt(TNEComplicatedGraph, TNEInDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEInDegCntComplicatedGraph);
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

  TIntPrV TUNExpectedComplicatedGraph;
  TIntPrV TUNOutDegCntComplicatedGraph;
  TIntPrV TNExpectedComplicatedGraph;
  TIntPrV TNOutDegCntComplicatedGraph;
  TIntPrV TNEExpectedComplicatedGraph;
  TIntPrV TNEOutDegCntComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TIntPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TIntPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TIntPr(3, 2));
  TSnap::GetOutDegCnt(TUNComplicatedGraph, TUNOutDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNOutDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TIntPr(0, 2));
  TNExpectedComplicatedGraph.Add(TIntPr(2, 3));
  TSnap::GetOutDegCnt(TNComplicatedGraph, TNOutDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNOutDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TIntPr(0, 2));
  TNEExpectedComplicatedGraph.Add(TIntPr(2, 2));
  TNEExpectedComplicatedGraph.Add(TIntPr(3, 1));
  TSnap::GetOutDegCnt(TNEComplicatedGraph, TNEOutDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEOutDegCntComplicatedGraph);
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

  TFltPrV TUNExpectedComplicatedGraph;
  TFltPrV TUNOutDegCntComplicatedGraph;
  TFltPrV TNExpectedComplicatedGraph;
  TFltPrV TNOutDegCntComplicatedGraph;
  TFltPrV TNEExpectedComplicatedGraph;
  TFltPrV TNEOutDegCntComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TFltPr(3, 2));
  TSnap::GetOutDegCnt(TUNComplicatedGraph, TUNOutDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNOutDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNExpectedComplicatedGraph.Add(TFltPr(2, 3));
  TSnap::GetOutDegCnt(TNComplicatedGraph, TNOutDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNOutDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(3, 1));
  TSnap::GetOutDegCnt(TNEComplicatedGraph, TNEOutDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEOutDegCntComplicatedGraph);
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

  TIntPrV TUNExpectedComplicatedGraph;
  TIntPrV TUNDegCntComplicatedGraph;
  TIntPrV TNExpectedComplicatedGraph;
  TIntPrV TNDegCntComplicatedGraph;
  TIntPrV TNEExpectedComplicatedGraph;
  TIntPrV TNEDegCntComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TIntPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TIntPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TIntPr(3, 2));
  TSnap::GetDegCnt(TUNComplicatedGraph, TUNDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TIntPr(0, 1));
  TNExpectedComplicatedGraph.Add(TIntPr(2, 2));
  TNExpectedComplicatedGraph.Add(TIntPr(4, 2));
  TSnap::GetDegCnt(TNComplicatedGraph, TNDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TIntPr(0, 1));
  TNEExpectedComplicatedGraph.Add(TIntPr(2, 1));
  TNEExpectedComplicatedGraph.Add(TIntPr(3, 1));
  TNEExpectedComplicatedGraph.Add(TIntPr(4, 1));
  TNEExpectedComplicatedGraph.Add(TIntPr(5, 1));
  TSnap::GetDegCnt(TNEComplicatedGraph, TNEDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEDegCntComplicatedGraph);
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

  TFltPrV TUNExpectedComplicatedGraph;
  TFltPrV TUNDegCntComplicatedGraph;
  TFltPrV TNExpectedComplicatedGraph;
  TFltPrV TNDegCntComplicatedGraph;
  TFltPrV TNEExpectedComplicatedGraph;
  TFltPrV TNEDegCntComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TFltPr(3, 2));
  TSnap::GetDegCnt(TUNComplicatedGraph, TUNDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TNExpectedComplicatedGraph.Add(TFltPr(4, 2));
  TSnap::GetDegCnt(TNComplicatedGraph, TNDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(2, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(3, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(4, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(5, 1));
  TSnap::GetDegCnt(TNEComplicatedGraph, TNEDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEDegCntComplicatedGraph);
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

  TIntV TUNExpectedComplicatedGraph;
  TIntV TUNDegVComplicatedGraph;
  TIntV TNExpectedComplicatedGraph;
  TIntV TNDegVComplicatedGraph;
  TIntV TNEExpectedComplicatedGraph;
  TIntV TNEDegVComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(2);
  TUNExpectedComplicatedGraph.Add(3);
  TUNExpectedComplicatedGraph.Add(3);
  TUNExpectedComplicatedGraph.Add(0);
  TUNExpectedComplicatedGraph.Add(2);
  TSnap::GetDegSeqV(TUNComplicatedGraph, TUNDegVComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNDegVComplicatedGraph);

  TNExpectedComplicatedGraph.Add(2);
  TNExpectedComplicatedGraph.Add(4);
  TNExpectedComplicatedGraph.Add(4);
  TNExpectedComplicatedGraph.Add(0);
  TNExpectedComplicatedGraph.Add(2);
  TSnap::GetDegSeqV(TNComplicatedGraph, TNDegVComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNDegVComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(2);
  TNEExpectedComplicatedGraph.Add(5);
  TNEExpectedComplicatedGraph.Add(4);
  TNEExpectedComplicatedGraph.Add(0);
  TNEExpectedComplicatedGraph.Add(3);
  TSnap::GetDegSeqV(TNEComplicatedGraph, TNEDegVComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEDegVComplicatedGraph);
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

  TIntV TUNExpectedInDegVComplicatedGraph;
  TIntV TUNExpectedOutDegVComplicatedGraph;
  TIntV TUNInDegVComplicatedGraph;
  TIntV TUNOutDegVComplicatedGraph;
  TIntV TNExpectedInDegVComplicatedGraph;
  TIntV TNExpectedOutDegVComplicatedGraph;
  TIntV TNInDegVComplicatedGraph;
  TIntV TNOutDegVComplicatedGraph;
  TIntV TNEExpectedInDegVComplicatedGraph;
  TIntV TNEExpectedOutDegVComplicatedGraph;
  TIntV TNEInDegVComplicatedGraph;
  TIntV TNEOutDegVComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedInDegVComplicatedGraph.Add(2);
  TUNExpectedInDegVComplicatedGraph.Add(3);
  TUNExpectedInDegVComplicatedGraph.Add(3);
  TUNExpectedInDegVComplicatedGraph.Add(0);
  TUNExpectedInDegVComplicatedGraph.Add(2);
  TUNExpectedOutDegVComplicatedGraph.Add(2);
  TUNExpectedOutDegVComplicatedGraph.Add(3);
  TUNExpectedOutDegVComplicatedGraph.Add(3);
  TUNExpectedOutDegVComplicatedGraph.Add(0);
  TUNExpectedOutDegVComplicatedGraph.Add(2);
  TSnap::GetDegSeqV(
      TUNComplicatedGraph,
      TUNInDegVComplicatedGraph,
      TUNOutDegVComplicatedGraph);
  CheckVectors(TUNExpectedInDegVComplicatedGraph, TUNInDegVComplicatedGraph);
  CheckVectors(TUNExpectedOutDegVComplicatedGraph, TUNOutDegVComplicatedGraph);

  TNExpectedInDegVComplicatedGraph.Add(0);
  TNExpectedInDegVComplicatedGraph.Add(2);
  TNExpectedInDegVComplicatedGraph.Add(2);
  TNExpectedInDegVComplicatedGraph.Add(0);
  TNExpectedInDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(0);
  TNExpectedOutDegVComplicatedGraph.Add(0);
  TSnap::GetDegSeqV(
      TNComplicatedGraph,
      TNInDegVComplicatedGraph,
      TNOutDegVComplicatedGraph);
  CheckVectors(TNExpectedInDegVComplicatedGraph, TNInDegVComplicatedGraph);
  CheckVectors(TNExpectedOutDegVComplicatedGraph, TNOutDegVComplicatedGraph);

  TNEExpectedInDegVComplicatedGraph.Add(0);
  TNEExpectedInDegVComplicatedGraph.Add(2);
  TNEExpectedInDegVComplicatedGraph.Add(2);
  TNEExpectedInDegVComplicatedGraph.Add(0);
  TNEExpectedInDegVComplicatedGraph.Add(3);
  TNEExpectedOutDegVComplicatedGraph.Add(2);
  TNEExpectedOutDegVComplicatedGraph.Add(3);
  TNEExpectedOutDegVComplicatedGraph.Add(2);
  TNEExpectedOutDegVComplicatedGraph.Add(0);
  TNEExpectedOutDegVComplicatedGraph.Add(0);
  TSnap::GetDegSeqV(
      TNEComplicatedGraph,
      TNEInDegVComplicatedGraph,
      TNEOutDegVComplicatedGraph);
  CheckVectors(TNEExpectedInDegVComplicatedGraph, TNEInDegVComplicatedGraph);
  CheckVectors(TNEExpectedOutDegVComplicatedGraph, TNEOutDegVComplicatedGraph);
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

  TIntPrV TUNExpectedInDegVComplicatedGraph;
  TIntPrV TUNInDegVComplicatedGraph;
  TIntPrV TNExpectedInDegVComplicatedGraph;
  TIntPrV TNInDegVComplicatedGraph;
  TIntPrV TNEExpectedInDegVComplicatedGraph;
  TIntPrV TNEInDegVComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedInDegVComplicatedGraph.Add(TIntPr(0, 2));
  TUNExpectedInDegVComplicatedGraph.Add(TIntPr(1, 3));
  TUNExpectedInDegVComplicatedGraph.Add(TIntPr(2, 3));
  TUNExpectedInDegVComplicatedGraph.Add(TIntPr(3, 0));
  TUNExpectedInDegVComplicatedGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeInDegV(TUNComplicatedGraph, TUNInDegVComplicatedGraph);
  CheckVectors(TUNExpectedInDegVComplicatedGraph, TUNInDegVComplicatedGraph);

  TNExpectedInDegVComplicatedGraph.Add(TIntPr(0, 0));
  TNExpectedInDegVComplicatedGraph.Add(TIntPr(1, 2));
  TNExpectedInDegVComplicatedGraph.Add(TIntPr(2, 2));
  TNExpectedInDegVComplicatedGraph.Add(TIntPr(3, 0));
  TNExpectedInDegVComplicatedGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeInDegV(TNComplicatedGraph, TNInDegVComplicatedGraph);
  CheckVectors(TNExpectedInDegVComplicatedGraph, TNInDegVComplicatedGraph);

  TNEExpectedInDegVComplicatedGraph.Add(TIntPr(0, 0));
  TNEExpectedInDegVComplicatedGraph.Add(TIntPr(1, 2));
  TNEExpectedInDegVComplicatedGraph.Add(TIntPr(2, 2));
  TNEExpectedInDegVComplicatedGraph.Add(TIntPr(3, 0));
  TNEExpectedInDegVComplicatedGraph.Add(TIntPr(4, 3));
  TSnap::GetNodeInDegV(TNEComplicatedGraph, TNEInDegVComplicatedGraph);
  CheckVectors(TNEExpectedInDegVComplicatedGraph, TNEInDegVComplicatedGraph);
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

  TIntPrV TUNExpectedOutDegVComplicatedGraph;
  TIntPrV TUNOutDegVComplicatedGraph;
  TIntPrV TNExpectedOutDegVComplicatedGraph;
  TIntPrV TNOutDegVComplicatedGraph;
  TIntPrV TNEExpectedOutDegVComplicatedGraph;
  TIntPrV TNEOutDegVComplicatedGraph;

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

  // ComplicatedGraph

  TUNExpectedOutDegVComplicatedGraph.Add(TIntPr(0, 2));
  TUNExpectedOutDegVComplicatedGraph.Add(TIntPr(1, 3));
  TUNExpectedOutDegVComplicatedGraph.Add(TIntPr(2, 3));
  TUNExpectedOutDegVComplicatedGraph.Add(TIntPr(3, 0));
  TUNExpectedOutDegVComplicatedGraph.Add(TIntPr(4, 2));
  TSnap::GetNodeOutDegV(TUNComplicatedGraph, TUNOutDegVComplicatedGraph);
  CheckVectors(TUNExpectedOutDegVComplicatedGraph, TUNOutDegVComplicatedGraph);

  TNExpectedOutDegVComplicatedGraph.Add(TIntPr(0, 2));
  TNExpectedOutDegVComplicatedGraph.Add(TIntPr(1, 2));
  TNExpectedOutDegVComplicatedGraph.Add(TIntPr(2, 2));
  TNExpectedOutDegVComplicatedGraph.Add(TIntPr(3, 0));
  TNExpectedOutDegVComplicatedGraph.Add(TIntPr(4, 0));
  TSnap::GetNodeOutDegV(TNComplicatedGraph, TNOutDegVComplicatedGraph);
  CheckVectors(TNExpectedOutDegVComplicatedGraph, TNOutDegVComplicatedGraph);

  TNEExpectedOutDegVComplicatedGraph.Add(TIntPr(0, 2));
  TNEExpectedOutDegVComplicatedGraph.Add(TIntPr(1, 3));
  TNEExpectedOutDegVComplicatedGraph.Add(TIntPr(2, 2));
  TNEExpectedOutDegVComplicatedGraph.Add(TIntPr(3, 0));
  TNEExpectedOutDegVComplicatedGraph.Add(TIntPr(4, 0));
  TSnap::GetNodeOutDegV(TNEComplicatedGraph, TNEOutDegVComplicatedGraph);
  CheckVectors(TNEExpectedOutDegVComplicatedGraph, TNEOutDegVComplicatedGraph);
}

// Testing CntUniqUndirEdges
TEST_F(GraphTest, CntUniqUndirEdgesTest) {
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TUNLoopGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNLoopGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNELoopGraph));

  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TUNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TNEReverseTree));

  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TUNComplicatedGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNComplicatedGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNEComplicatedGraph));
}

// Testing CntUniqDirEdges
TEST_F(GraphTest, CntUniqDirEdgesTest) {
  EXPECT_EQ(10, TSnap::CntUniqDirEdges(TUNLoopGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNLoopGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNELoopGraph));

  EXPECT_EQ(8, TSnap::CntUniqDirEdges(TUNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqDirEdges(TNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqDirEdges(TNEReverseTree));

  EXPECT_EQ(10, TSnap::CntUniqDirEdges(TUNComplicatedGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNComplicatedGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNEComplicatedGraph));
}

// Testing CntUniqBiDirEdges
TEST_F(GraphTest, CntUniqBiDirEdgesTest) {
  EXPECT_EQ(5, TSnap::CntUniqBiDirEdges(TUNLoopGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNLoopGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNELoopGraph));

  EXPECT_EQ(4, TSnap::CntUniqBiDirEdges(TUNReverseTree));
  EXPECT_EQ(0, TSnap::CntUniqBiDirEdges(TNReverseTree));
  EXPECT_EQ(0, TSnap::CntUniqBiDirEdges(TNEReverseTree));

  EXPECT_EQ(5, TSnap::CntUniqBiDirEdges(TUNComplicatedGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNComplicatedGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNEComplicatedGraph));
}

// Testing CntSelfEdges
TEST_F(GraphTest, CntSelfEdgesTest) {
  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNLoopGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNLoopGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNELoopGraph));

  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNReverseTree));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNReverseTree));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNEReverseTree));

  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNComplicatedGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNComplicatedGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNEComplicatedGraph));

  EXPECT_EQ(1, TSnap::CntSelfEdges(TUNComplicatedGraphWithLoop));
  EXPECT_EQ(1, TSnap::CntSelfEdges(TNComplicatedGraphWithLoop));
  EXPECT_EQ(1, TSnap::CntSelfEdges(TNEComplicatedGraphWithLoop));
}

// Testing GetUnDir
TEST_F(GraphTest, GetUnDirTest) {
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;
  PUNGraph TUNGraphUnDir;
  PNGraph TNGraphUnDir;
  PNEGraph TNEGraphUnDir;

  // ComplicatedGraph

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(4, 2));
  TNExpectedDegCnt.Add(TIntPr(6, 2));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(4, 1));
  TNEExpectedDegCnt.Add(TIntPr(6, 2));
  TNEExpectedDegCnt.Add(TIntPr(8, 1));

  TNGraphUnDir = TSnap::GetUnDir(TNComplicatedGraph);
  TNEGraphUnDir = TSnap::GetUnDir(TNEComplicatedGraph);

  CheckGraphs(TNExpectedDegCnt, TNGraphUnDir);
  CheckGraphs(TNEExpectedDegCnt, TNEGraphUnDir);
}

// Testing MakeUnDir
TEST_F(GraphTest, MakeUnDirTest) {
  TIntPrV TNExpectedDegCnt;
  TIntPrV TNEExpectedDegCnt;

  // ComplicatedGraph

  TNExpectedDegCnt.Add(TIntPr(0, 1));
  TNExpectedDegCnt.Add(TIntPr(4, 2));
  TNExpectedDegCnt.Add(TIntPr(6, 2));

  TNEExpectedDegCnt.Add(TIntPr(0, 1));
  TNEExpectedDegCnt.Add(TIntPr(4, 1));
  TNEExpectedDegCnt.Add(TIntPr(6, 2));
  TNEExpectedDegCnt.Add(TIntPr(8, 1));

  TSnap::MakeUnDir(TNComplicatedGraph);
  TSnap::MakeUnDir(TNEComplicatedGraph);

  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
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

  // ComplicatedGraphWithLoop

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

  TSnap::DelSelfEdges(TUNComplicatedGraphWithLoop);
  TSnap::DelSelfEdges(TNComplicatedGraphWithLoop);
  TSnap::DelSelfEdges(TNEComplicatedGraphWithLoop);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraphWithLoop);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraphWithLoop);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraphWithLoop);
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

  TUNExpectedDegCnt.Add(TIntPr(2, 2));
  TUNExpectedDegCnt.Add(TIntPr(3, 2));

  TNExpectedDegCnt.Add(TIntPr(2, 2));
  TNExpectedDegCnt.Add(TIntPr(4, 2));

  TNEExpectedDegCnt.Add(TIntPr(2, 1));
  TNEExpectedDegCnt.Add(TIntPr(3, 1));
  TNEExpectedDegCnt.Add(TIntPr(4, 1));
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

  TUNExpectedDegCnt.Add(TIntPr(0, 3));

  TNExpectedDegCnt.Add(TIntPr(0, 1));

  TNEExpectedDegCnt.Add(TIntPr(2, 2));

  TSnap::DelDegKNodes(TUNComplicatedGraph, 3, 3);
  TSnap::DelDegKNodes(TNComplicatedGraph, 2, 0);
  TSnap::DelDegKNodes(TNEComplicatedGraph, 2, 0);

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
  EXPECT_EQ(1, RootNId);
  EXPECT_TRUE(TSnap::IsTree(Tree, RootNId));
  EXPECT_EQ(1, RootNId);
  EXPECT_FALSE(TSnap::IsTree(Forest, RootNId));
  EXPECT_EQ(-1, RootNId);
  EXPECT_FALSE(TSnap::IsTree(Circle, RootNId));
  EXPECT_EQ(-1, RootNId);
}

// Testing GetTreeRootNId
TEST_F(TreeTest, GetTreeRootNIdTest) {
  int RootNId = 1;

  EXPECT_EQ(RootNId, TSnap::GetTreeRootNId(SingleNode));
  EXPECT_EQ(RootNId, TSnap::GetTreeRootNId(Tree));
  //EXPECT_DEATH(TSnap::GetTreeRootNId(Forest), "");
  //EXPECT_DEATH(TSnap::GetTreeRootNId(Circle), "");
}

// Testing GetTreeSig
TEST_F(TreeTest, GetTreeSigTest) {
  int RootNId = 1;
  TIntV SingleNodeSig;
  TIntV TreeSig;
  TIntV ForestSig;
  TIntV CircleSig;
  TIntV ExpectedSingleNodeSig;
  TIntV ExpectedTreeSig;

  // SingleNode

  ExpectedSingleNodeSig.Add(0);
  TSnap::GetTreeSig(SingleNode, RootNId, SingleNodeSig);
  CheckVectors(ExpectedSingleNodeSig, SingleNodeSig);

  // Tree

  int NumNodesWithChildren = 6;
  int NumNodesWithChild = 1;
  int NumNodesWithNoChildren = 7;
  AddValues(ExpectedTreeSig, NumNodesWithChildren, 2);
  AddValues(ExpectedTreeSig, NumNodesWithChild, 1);
  AddValues(ExpectedTreeSig, NumNodesWithNoChildren, 0);

  // OP RS 2014/06/12, the code below commented out, needs more investigation
  //TSnap::GetTreeSig(Tree, RootNId, TreeSig);
  //CheckVectors(ExpectedTreeSig, TreeSig);

  // Forest

  //EXPECT_DEATH(TSnap::GetTreeSig(Forest, RootNId, ForestSig), "");

  // Circle

  //EXPECT_DEATH(TSnap::GetTreeSig(Circle, RootNId, CircleSig), "");
}

// Testing GetTreeSig with node map
TEST_F(TreeTest, GetTreeSigWithNodeMapTest) {
  int RootNId = 1;
  TIntV SingleNodeSig;
  TIntV TreeSig;
  TIntV ForestSig;
  TIntV CircleSig;
  TIntV ExpectedSingleNodeSig;
  TIntV ExpectedTreeSig;
  TIntPrV SingleNodeNodeMap;
  TIntPrV TreeNodeMap;
  TIntPrV ForestNodeMap;
  TIntPrV CircleNodeMap;
  TIntV TreeNodeMapKeys;
  TIntPrV ExpectedSingleNodeNodeMap;
  TIntV ExpectedTreeNodeMapKeys;

  // SingleNode

  ExpectedSingleNodeSig.Add(0);
  ExpectedSingleNodeNodeMap.Add(TIntPr(1, 0));
  TSnap::GetTreeSig(SingleNode, RootNId, SingleNodeSig, SingleNodeNodeMap);
  // OP RS 2014/06/12, the code below commented out, needs more investigation
  //CheckVectors(ExpectedSingleNodeSig, SingleNodeSig);
  //CheckVectors(ExpectedSingleNodeNodeMap, SingleNodeNodeMap);

  // Tree

  int NumNodesWithChildren = 6;
  int NumNodesWithChild = 1;
  int NumNodesWithNoChildren = 7;
  AddValues(ExpectedTreeSig, NumNodesWithChildren, 2);
  AddValues(ExpectedTreeSig, NumNodesWithChild, 1);
  AddValues(ExpectedTreeSig, NumNodesWithNoChildren, 0);
  AddConsecutiveValues(ExpectedTreeNodeMapKeys, RootNId, Tree->GetNodes());

  // OP RS 2014/06/12, the code below commented out, needs more investigation
  // Checking only node IDs (not positions) for node map

  //TSnap::GetTreeSig(Tree, RootNId, TreeSig, TreeNodeMap);
  //GetKeys(TreeNodeMapKeys, TreeNodeMap);
  //ExpectedTreeNodeMapKeys.Sort();
  //TreeNodeMapKeys.Sort();
  //CheckVectors(ExpectedTreeSig, TreeSig);
  //CheckVectors(ExpectedTreeNodeMapKeys, TreeNodeMapKeys);

  // Forest
  //EXPECT_DEATH(
  //    TSnap::GetTreeSig(Forest, RootNId, ForestSig, ForestNodeMap), "");

  // Circle
  //EXPECT_DEATH(
  //    TSnap::GetTreeSig(Circle, RootNId, CircleSig, CircleNodeMap), "");
}

///
// HELPER METHODS
///

void CheckVectors(const TIntPrV& Expected, const TIntPrV& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val1.Val, Actual[i].Val1.Val);
    EXPECT_EQ(Expected[i].Val2.Val, Actual[i].Val2.Val);
  }
}

void CheckVectors(const TFltPrV& Expected, const TFltPrV& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val1.Val, Actual[i].Val1.Val);
    EXPECT_EQ(Expected[i].Val2.Val, Actual[i].Val2.Val);
  }
}

void CheckVectors(const TIntV& Expected, const TIntV& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val, Actual[i].Val);
  }
}

void CheckVectors(const TIntSet& Expected, const TIntSet& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val, Actual[i].Val);
  }
}

void CheckGraphs(const TIntPrV& ExpectedDegCnt, const PUNGraph& ActualGraph) {
  TIntPrV ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckGraphs(const TIntPrV& ExpectedDegCnt, const PNGraph& ActualGraph) {
  TIntPrV ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckGraphs(const TIntPrV& ExpectedDegCnt, const PNEGraph& ActualGraph) {
  TIntPrV ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckNotInSet(int Key, const TIntSet& Keys) {
  for (int i = 0; i < Keys.Len(); i++) {
    EXPECT_NE(Keys[i], Key);
  }
}

void AddValues(TIntV& Vector, int NumValues, int Value) {
  for (int i = 0; i < NumValues; i++) {
    Vector.Add(Value);
  }
}

void AddConsecutiveValues(TIntV& Vector, int Start, int End) {
  for (int Id = Start; Id <= End; Id++) {
    Vector.Add(Id);
  }
}

void GetKeys(TIntV& Keys, const TIntPrV& Map) {
  for (int Id = 0; Id < Map.Len(); Id++) {
    Keys.Add(Map[Id].Val1.Val);
  }
}
