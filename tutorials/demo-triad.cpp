#include <gtest/gtest.h>

#include "Snap.h"

PUNGraph TriadGetTestTUNGraph();
PNGraph TriadGetTestTNGraph();
PNEGraph TriadGetTestTNEGraph();

void TestGetTriadsVector(void);
void TestGetTriadsVectorHelper(TIntTrV& NIdCOTriadV);
void TestGetTriadsOpenClosed(void);
void TestGetTriadsClosed(void);
void TestGetClustCf(void);
void TestGetClustCfDist(void);
void TestGetClustCfDistCO(void);
void TestGetClustCfDistHelper(TFltPrV& DegToCCfV);
void TestGetNodeClustCfSpecific(void);
void TestGetNodeClustCfVector(void);
void TestGetTriadEdges(void);
void TestGetNodeTriads(void);
void TestGetNodeCOTriads(void);
void TestGetNodeTriadsGroupSetAndOut(void);
void GetGroupSet(int NId, TIntSet& GroupSet);
void TestGetTriadParticip(void);
void TestGetCmnNbrs(void);
void TestGetLen2Paths(void);


// Generate Test Graphs for Tutorial ///////////////////////////////////////////

// Generate TUNGraph
PUNGraph TriadGetTestTUNGraph() {
  PUNGraph Graph = TUNGraph::New();

  for (int i = 0; i < 6; i++) {
    Graph->AddNode(i);
  }
  
  for (int i = 1; i < 6; i++) {
    Graph->AddEdge(0, i);
  }
  
  Graph->AddEdge(2, 3);
  Graph->AddEdge(1, 5);
  Graph->AddEdge(2, 5);

  return Graph;
}

// Generate TNGraph
PNGraph TriadGetTestTNGraph() {
  PNGraph Graph = TNGraph::New();

  for (int i = 0; i < 6; i++) {
    Graph->AddNode(i);
  }
  
  for (int i = 1; i < 6; i++) {
    Graph->AddEdge(0, i);
  }
  
  Graph->AddEdge(2, 3);
  Graph->AddEdge(1, 5);
  Graph->AddEdge(2, 5);

  return Graph;
}

// Generate TNEGraph
PNEGraph TriadGetTestTNEGraph() {
  PNEGraph Graph = TNEGraph::New();

  for (int i = 0; i < 6; i++) {
    Graph->AddNode(i);
  }
  
  for (int i = 1; i < 6; i++) {
    for (int j = 0; j < i; j++) {
      Graph->AddEdge(0, i);
    }
  }
  
  Graph->AddEdge(2, 3);
  Graph->AddEdge(1, 5);
  Graph->AddEdge(2, 5);

  return Graph;
}

// Tests for Tutorial //////////////////////////////////////////////////////////

// Test GetTriads Close/Open Triad Vector for each NId
// Returns the triad vectors for entire graph (NId, closed, open);
void TestGetTriadsVector() {
  // Test TUNGraph
  printf("---- TestGetTriadsVector - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TIntTrV NIdCOTriadV;
  TSnap::GetTriads(Graph_TUNGraph, NIdCOTriadV);
  TestGetTriadsVectorHelper(NIdCOTriadV);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsVector - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  NIdCOTriadV.Clr();
  TSnap::GetTriads(Graph_TNGraph, NIdCOTriadV);
  TestGetTriadsVectorHelper(NIdCOTriadV);

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsVector - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  NIdCOTriadV.Clr();
  TSnap::GetTriads(Graph_TNEGraph, NIdCOTriadV);
  TestGetTriadsVectorHelper(NIdCOTriadV);

  printf("\n");
}


// Helper: Testing Opened/Closed Triads for Specific Generated Graph
void TestGetTriadsVectorHelper(TIntTrV& NIdCOTriadV) {
  printf("\tGraph has the following triads (With respect to NId):\n");
  for (TIntTr *vec = NIdCOTriadV.BegI(); vec < NIdCOTriadV.EndI(); vec++) {
    printf("\tNId: %d | Closed Triads: %d | Open Triads: %d\n", vec->Val1.Val, 
           vec->Val2.Val, vec->Val3.Val);
  }
}

// Test GetTriads for Whole Graph (Open and Closed)
// Returns the number of closed and open triads
void TestGetTriadsOpenClosed() {
  // Test TUNGraph
  printf("---- TestGetTriadsOpenClosed - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  int64 closed = 0, open = 0;

  TSnap::GetTriads(Graph_TUNGraph, closed, open);
  printf("\tGraph has %lld closed triads and %lld open triads.\n", closed, open);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsOpenClosed - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  closed = 0, open = 0;

  TSnap::GetTriads(Graph_TNGraph, closed, open);
  printf("\tGraph has %lld closed triads and %lld open triads.\n", closed, open);

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsOpenClosed - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  closed = 0, open = 0;

  TSnap::GetTriads(Graph_TNEGraph, closed, open);
  printf("\tGraph has %lld closed triads and %lld open triads.\n", closed, open);

  printf("\n");
}

// Test GetTriads for Whole Graph (Only Closed)
// Get the number of closed triads (similar to above but as return value)
void TestGetTriadsClosed() {

  // Test TUNGraph
  printf("---- TestGetTriadsClosed - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  int closed = 0;
  closed = TSnap::GetTriads(Graph_TUNGraph);
  printf("\tGraph has %d closed triads.\n", closed);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsClosed - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  closed = 0;
  closed = TSnap::GetTriads(Graph_TNGraph);
  printf("\tGraph has %d closed triads.\n", closed);

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsClosed - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  closed = 0;
  closed = TSnap::GetTriads(Graph_TNEGraph);
  printf("\tGraph has %d closed triads.\n", closed);

  printf("\n");
}

// Test GetClustCf (Average Clustering Coefficient)
void TestGetClustCf() {
  // Test TUNGraph
  printf("---- TestGetClustCf - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  double ClustCf = 0.0;
  ClustCf = TSnap::GetClustCf(Graph_TUNGraph);
  printf("\tGraph has a clustering coefficient of %f.\n", ClustCf);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCf - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  ClustCf = 0.0;
  ClustCf = TSnap::GetClustCf(Graph_TNGraph);
  printf("\tGraph has a clustering coefficient of %f.\n", ClustCf);

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCf - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  ClustCf = 0.0;
  ClustCf = TSnap::GetClustCf(Graph_TNEGraph);
  printf("\tGraph has a clustering coefficient of %f.\n", ClustCf);

  printf("\n");
}

// Test GetClustCf (Distribution of Average Clustering Coefficients)
void TestGetClustCfDist() {
  // Test TUNGraph
  printf("---- TestGetClustCfDist - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;
  TSnap::GetClustCf(Graph_TUNGraph, DegToCCfV);
  TestGetClustCfDistHelper(DegToCCfV);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfDist - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  DegToCCfV.Clr();
  TSnap::GetClustCf(Graph_TNGraph, DegToCCfV);
  TestGetClustCfDistHelper(DegToCCfV);

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetClustCfDist - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  TSnap::GetClustCf(Graph_TNEGraph, DegToCCfV);
  TestGetClustCfDistHelper(DegToCCfV);

  printf("\n");
}

// Test GetClustCf (Distribution and Closed and Open)
// Gets the distribution as well as closed and opened triad numbers
void TestGetClustCfDistCO() {
  // Test TUNGraph
  printf("---- TestGetClustCfDistCO - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;
  int64 closed = 0, open = 0;
  TSnap::GetClustCf(Graph_TUNGraph, DegToCCfV, closed, open);
  TestGetClustCfDistHelper(DegToCCfV);
  printf("\tClosed Triads: %lld | Open Triads: %lld\n", closed, open);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfDistCO - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  DegToCCfV.Clr();
  closed = 0, open = 0;
  TSnap::GetClustCf(Graph_TNGraph, DegToCCfV, closed, open);
  TestGetClustCfDistHelper(DegToCCfV);
  printf("\tClosed Triads: %lld | Open Triads: %lld\n", closed, open);

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetClustCfDistCO - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  closed = 0, open = 0;
  TSnap::GetClustCf(Graph_TNEGraph, DegToCCfV, closed, open);
  TestGetClustCfDistHelper(DegToCCfV);
  printf("\tClosed Triads: %lld | Open Triads: %lld\n", closed, open);

  printf("\n");
}

// Helper: Testing Degree to Clustering Coefficient Distribution
void TestGetClustCfDistHelper(TFltPrV& DegToCCfV) {
  printf("\tGraph has the following Clustering Coefficient Distribution:\n");
  for (TFltPr *pair = DegToCCfV.BegI(); pair < DegToCCfV.EndI(); pair++) {
    printf("\tDegree: %f | Clustering Coefficient: %f\n", pair->Val1.Val, 
           pair->Val2.Val);
  }
}

// Test GetNodeClustCf (Specific Node)
void TestGetNodeClustCfSpecific() {
  // Test TUNGraph
  printf("---- TestGetClustCfSpecific - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(Graph_TUNGraph, i);
    printf("\tQuery Node: %d | Clustering Coefficient: %f\n", i, ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfSpecific - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(Graph_TNGraph, i);
    printf("\tQuery Node: %d | Clustering Coefficient: %f\n", i, ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfSpecific - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(Graph_TNEGraph, i);
    printf("\tQuery Node: %d | Clustering Coefficient: %f\n", i, ClustCf);
  }

  printf("\n");
}

// Test GetNodeClustCf (Vector)
// Gets the Clustering Coefficient at Queried Node but in Vector Format
void TestGetNodeClustCfVector() {
  // Test TUNGraph
  printf("---- TestGetClustCfVector - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TIntFltH NIdCCfH;
  TSnap::GetNodeClustCf(Graph_TUNGraph, NIdCCfH);
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    printf("\tQuery Node in Vector: %d | Clustering Coefficient: %f\n", i, 
           ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfVector - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  NIdCCfH.Clr();
  TSnap::GetNodeClustCf(Graph_TNGraph, NIdCCfH);
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    printf("\tQuery Node in Vector: %d | Clustering Coefficient: %f\n", i, 
           ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfVector - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  NIdCCfH.Clr();

  TSnap::GetNodeClustCf(Graph_TNEGraph, NIdCCfH);
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    printf("\tQuery Node in Vector: %d | Clustering Coefficient: %f\n", i, 
           ClustCf);
  }

  printf("\n");
}

// Test GetTriadEdges
// Counts the number of edges that participate in at least one triad.
void TestGetTriadEdges() {
  // Test TUNGraph
  printf("---- TestGetTriadEdges - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  printf("\tNumber of Edges Participating in at least one triad: %d\n", 
         TSnap::GetTriadEdges(Graph_TUNGraph));

  // TNGraph is not treated the same! Each directed will be counted
  printf("---- TestGetTriadEdges - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  printf("\tNumber of Edges Participating in at least one triad: %d\n", 
         TSnap::GetTriadEdges(Graph_TNGraph));

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetTriadEdges - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  printf("\tNumber of Edges Participating in at least one triad: %d\n", 
         TSnap::GetTriadEdges(Graph_TNEGraph));

  printf("\n");
}

// Test GetNodeTriads
// Number of Closed Triads associated with Node NId
void TestGetNodeTriads() {
  // Test TUNGraph
  printf("---- TestGetNodeTriads - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    printf("\tNode: %d | Participating Closed Triads: %d\n", i,
           TSnap::GetNodeTriads(Graph_TUNGraph, i));
  }
  
  // TNGraph is not treated the same! Each directed will be counted
  printf("---- TestGetNodeTriads - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    printf("\tNode: %d | Participating Closed Triads: %d\n", i,
           TSnap::GetNodeTriads(Graph_TNGraph, i));
  }

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetNodeTriads - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    printf("\tNode: %d | Participating Closed Triads: %d\n", i,
           TSnap::GetNodeTriads(Graph_TNEGraph, i));
  }

  printf("\n");
}

// Test GetNodeTriads (Open and Closed)
void TestGetNodeCOTriads() {
  // Test TUNGraph
  printf("---- TestGetNodeCOTriads - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    int ClosedTriads = -1, OpenTriads = -1;
    TSnap::GetNodeTriads(Graph_TUNGraph, i, ClosedTriads, OpenTriads);
    printf("\tNode: %d | Closed Triads: %d | Open Triads: %d\n", i,
           ClosedTriads, OpenTriads);
  }
  
  // Test TNGraph which is treated same as undirected.
  printf("---- TestGetNodeCOTriads - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    int ClosedTriads = -1, OpenTriads = -1;
    TSnap::GetNodeTriads(Graph_TNGraph, i, ClosedTriads, OpenTriads);
    printf("\tNode: %d | Closed Triads: %d | Open Triads: %d\n", i,
           ClosedTriads, OpenTriads);
  }

  // Test TNEGraph which is treated same as undirected.
  printf("---- TestGetNodeCOTriads - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    int ClosedTriads = -1, OpenTriads = -1;
    TSnap::GetNodeTriads(Graph_TNEGraph, i, ClosedTriads, OpenTriads);
    printf("\tNode: %d | Closed Triads: %d | Open Triads: %d\n", i,
           ClosedTriads, OpenTriads);
  }

  printf("\n");
}

// Test GetNodeTriads (GroupSet, InGroupEdges, InOutGroupEdges, OutGroupEdges)
void TestGetNodeTriadsGroupSetAndOut() {
  // Test TUNGraph
  printf("---- TestGetNodeTriadsGroupSetAndOut - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(Graph_TUNGraph, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    printf("\tNode: %d | InGroupEdges: %d |", i, InGroupEdges);
    printf(" InOutGroupEdges: %d | OutGroupEdges: %d\n", InOutGroupEdges, 
           OutGroupEdges);
  }
  
  // Test TNGraph which is same as undirected.
  printf("---- TestGetNodeTriadsGroupSetAndOut - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(Graph_TNGraph, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    printf("\tNode: %d | InGroupEdges: %d |", i, InGroupEdges);
    printf(" InOutGroupEdges: %d | OutGroupEdges: %d\n", InOutGroupEdges, 
           OutGroupEdges);
  }
  
  // Test TNEGraph which is same as undirected.
  printf("---- TestGetNodeTriadsGroupSetAndOut - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(Graph_TNEGraph, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    printf("\tNode: %d | InGroupEdges: %d |", i, InGroupEdges);
    printf(" InOutGroupEdges: %d | OutGroupEdges: %d\n", InOutGroupEdges, 
           OutGroupEdges);
  }

  printf("\n");
}

// Helper: Return GroupSet based on NodeID
void GetGroupSet(int NId, TIntSet& GroupSet) {
  GroupSet.Clr();
  switch (NId) {
    case 0:
      GroupSet.AddKey(2);
      GroupSet.AddKey(4);
      GroupSet.AddKey(5);
      break;
    case 1:
      // Empty Set
      break;
    case 2:
      GroupSet.AddKey(0);
      GroupSet.AddKey(3);
      GroupSet.AddKey(5);
      break;
    case 3:
      GroupSet.AddKey(0);
      break;
    case 4:
      GroupSet.AddKey(0);
      break;
    case 5:
      GroupSet.AddKey(0);
      GroupSet.AddKey(1);
      break;
    default:
      break;
  }
}

// Test GetTriadParticip
// Number of nodes with x number of triangles it participates in
void TestGetTriadParticip() {
  // Test TUNGraph
  printf("---- TestGetTriadParticip - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TIntPrV TriadCntV;
  TSnap::GetTriadParticip(Graph_TUNGraph, TriadCntV);
  for (TIntPr *vec = TriadCntV.BegI(); vec < TriadCntV.EndI(); vec++) {
    printf("\tNode %d participates in %d triangles.\n", vec->Val1.Val,
           vec->Val2.Val);
  }

  // Test TNGraph which is same as undirected.
  printf("---- TestGetTriadParticip - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(Graph_TNGraph, TriadCntV);
  for (TIntPr *vec = TriadCntV.BegI(); vec < TriadCntV.EndI(); vec++) {
    printf("\tNode %d participates in %d triangles.\n", vec->Val1.Val,
           vec->Val2.Val);
  }
  
  // Test TNEGraph which is same as undirected.
  printf("---- TestGetTriadParticip - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(Graph_TNGraph, TriadCntV);
  for (TIntPr *vec = TriadCntV.BegI(); vec < TriadCntV.EndI(); vec++) {
    printf("\tNode %d participates in %d triangles.\n", vec->Val1.Val,
           vec->Val2.Val);
  }

  printf("\n");
}

// Test GetCmnNbrs: the number of neighbors in common
void TestGetCmnNbrs() {
  // Test TUNGraph
  printf("---- TestGetCmnNbrs - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TUNGraph->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d common neighbors.\n", i, j, 
             TSnap::GetCmnNbrs(Graph_TUNGraph, i, j));
    }
  }
  
  // Test TNGraph which is same as undirected.
  printf("---- TestGetCmnNbrs - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TNGraph->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d common neighbors.\n", i, j, 
             TSnap::GetCmnNbrs(Graph_TNGraph, i, j));
    }
  }
  
  // Test TNEGraph which is same as undirected.
  printf("---- TestGetCmnNbrs - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TNEGraph->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d common neighbors.\n", i, j, 
             TSnap::GetCmnNbrs(Graph_TNEGraph, i, j));
    }
  }

  printf("\n");
}

// Test GetLen2Paths: Number of path lengths 2 between pair of nodes
void TestGetLen2Paths() {
  // Test TUNGraph
  printf("---- TestGetLen2Paths - TUNGraph ----\n");
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TUNGraph->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d paths with length 2 between them.\n",
             i, j, TSnap::GetLen2Paths(Graph_TUNGraph, i, j));
    }
    printf("\n");
  }
  
  // Test TNGraph which is different from undirected due to out neighbors.
  // Direction matters in pathing!
  printf("---- TestGetLen2Paths - TNGraph ----\n");
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    for (int j = 0; j < Graph_TNGraph->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d paths with length 2 between them.\n",
             i, j, TSnap::GetLen2Paths(Graph_TNGraph, i, j));
    }
    printf("\n");
  }

  // Test TNEGraph which is different from undirected due to out neighbors.
  // Direction matters in pathing!
  printf("---- TestGetLen2Paths - TNEGraph ----\n");
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    for (int j = 0; j < Graph_TNEGraph->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d paths with length 2 between them.\n",
             i, j, TSnap::GetLen2Paths(Graph_TNEGraph, i, j));
    }
    printf("\n");
  }

  printf("\n");
}

// Run the tutorial
int main(int argc, char *argv[]) {
  printf("---- Tutorial for SNAP Triads ----\n");
  // Triads
  TestGetTriadsVector();
  TestGetTriadsOpenClosed();
  TestGetTriadsClosed();

  // Clustering Coefficients
  TestGetClustCf();
  TestGetClustCfDist();
  TestGetClustCfDistCO();
  TestGetNodeClustCfSpecific();
  TestGetNodeClustCfVector();

  // Edge Triads
  TestGetTriadEdges();

  // Triads With Respect to Nodes
  TestGetNodeTriads();
  TestGetNodeCOTriads();
  TestGetNodeTriadsGroupSetAndOut();

  // Participating Triangles
  TestGetTriadParticip();

  // Common Neighbors
  TestGetCmnNbrs();

  // Length 2 Paths
  TestGetLen2Paths();
}
