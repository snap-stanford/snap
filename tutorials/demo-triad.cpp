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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TIntTrV NIdCOTriadV;
  TSnap::GetTriads(GraphTUN, NIdCOTriadV);
  TestGetTriadsVectorHelper(NIdCOTriadV);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsVector - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  NIdCOTriadV.Clr();
  TSnap::GetTriads(GraphTN, NIdCOTriadV);
  TestGetTriadsVectorHelper(NIdCOTriadV);

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsVector - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  NIdCOTriadV.Clr();
  TSnap::GetTriads(GraphTNE, NIdCOTriadV);
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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  int64 ClosedTr = 0;
  int64 OpenTr = 0;

  TSnap::GetTriads(GraphTUN, ClosedTr, OpenTr);
  printf("\tGraph has %s closed triads and %s open triads.\n",
    TUInt64::GetStr(ClosedTr).CStr(), TUInt64::GetStr(OpenTr).CStr());

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsOpenClosed - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  ClosedTr = 0;
  OpenTr = 0;

  TSnap::GetTriads(GraphTN, ClosedTr, OpenTr);
  printf("\tGraph has %s closed triads and %s open triads.\n",
    TUInt64::GetStr(ClosedTr).CStr(), TUInt64::GetStr(OpenTr).CStr());

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsOpenClosed - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  ClosedTr = 0;
  OpenTr = 0;

  TSnap::GetTriads(GraphTNE, ClosedTr, OpenTr);
  printf("\tGraph has %s closed triads and %s open triads.\n",
    TUInt64::GetStr(ClosedTr).CStr(), TUInt64::GetStr(OpenTr).CStr());

  printf("\n");
}

// Test GetTriads for Whole Graph (Only Closed)
// Get the number of closed triads (similar to above but as return value)
void TestGetTriadsClosed() {

  // Test TUNGraph
  printf("---- TestGetTriadsClosed - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  int64 ClosedTr = 0;

  ClosedTr = TSnap::GetTriads(GraphTUN);
  printf("\tGraph has %s closed triads.\n", TUInt64::GetStr(ClosedTr).CStr());

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsClosed - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  ClosedTr = TSnap::GetTriads(GraphTN);
  printf("\tGraph has %s closed triads.\n", TUInt64::GetStr(ClosedTr).CStr());

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetTriadsClosed - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();

  ClosedTr = TSnap::GetTriads(GraphTNE);
  printf("\tGraph has %s closed triads.\n", TUInt64::GetStr(ClosedTr).CStr());

  printf("\n");
}

// Test GetClustCf (Average Clustering Coefficient)
void TestGetClustCf() {
  // Test TUNGraph
  printf("---- TestGetClustCf - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  double ClustCf = 0.0;
  ClustCf = TSnap::GetClustCf(GraphTUN);
  printf("\tGraph has a clustering coefficient of %f.\n", ClustCf);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCf - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  ClustCf = 0.0;
  ClustCf = TSnap::GetClustCf(GraphTN);
  printf("\tGraph has a clustering coefficient of %f.\n", ClustCf);

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCf - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  ClustCf = 0.0;
  ClustCf = TSnap::GetClustCf(GraphTNE);
  printf("\tGraph has a clustering coefficient of %f.\n", ClustCf);

  printf("\n");
}

// Test GetClustCf (Distribution of Average Clustering Coefficients)
void TestGetClustCfDist() {
  // Test TUNGraph
  printf("---- TestGetClustCfDist - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;
  TSnap::GetClustCf(GraphTUN, DegToCCfV);
  TestGetClustCfDistHelper(DegToCCfV);

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfDist - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  DegToCCfV.Clr();
  TSnap::GetClustCf(GraphTN, DegToCCfV);
  TestGetClustCfDistHelper(DegToCCfV);

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetClustCfDist - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  TSnap::GetClustCf(GraphTNE, DegToCCfV);
  TestGetClustCfDistHelper(DegToCCfV);

  printf("\n");
}

// Test GetClustCf (Distribution and Closed and Open)
// Gets the distribution as well as closed and opened triad numbers
void TestGetClustCfDistCO() {
  // Test TUNGraph
  printf("---- TestGetClustCfDistCO - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;
  int64 ClosedTr = 0;
  int64 OpenTr = 0;

  TSnap::GetClustCf(GraphTUN, DegToCCfV, ClosedTr, OpenTr);
  TestGetClustCfDistHelper(DegToCCfV);
  printf("\tClosed Triads: %s | Open Triads: %s\n",
    TUInt64::GetStr(ClosedTr).CStr(), TUInt64::GetStr(OpenTr).CStr());

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfDistCO - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  DegToCCfV.Clr();

  ClosedTr = 0;
  OpenTr = 0;
  TSnap::GetClustCf(GraphTN, DegToCCfV, ClosedTr, OpenTr);
  TestGetClustCfDistHelper(DegToCCfV);
  printf("\tClosed Triads: %s | Open Triads: %s\n",
    TUInt64::GetStr(ClosedTr).CStr(), TUInt64::GetStr(OpenTr).CStr());

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetClustCfDistCO - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  DegToCCfV.Clr();

  ClosedTr = 0;
  OpenTr = 0;
  TSnap::GetClustCf(GraphTNE, DegToCCfV, ClosedTr, OpenTr);
  TestGetClustCfDistHelper(DegToCCfV);
  printf("\tClosed Triads: %s | Open Triads: %s\n",
    TUInt64::GetStr(ClosedTr).CStr(), TUInt64::GetStr(OpenTr).CStr());

  printf("\n");
}

// Helper: Testing Degree to Clustering Coefficient Distribution
void TestGetClustCfDistHelper(TFltPrV& DegToCCfV) {
  printf("\tGraph has the following Clustering Coefficient Distribution:\n");
  for (TFltPr *Pair = DegToCCfV.BegI(); Pair < DegToCCfV.EndI(); Pair++) {
    printf("\tDegree: %f | Clustering Coefficient: %f\n", Pair->Val1.Val, 
           Pair->Val2.Val);
  }
}

// Test GetNodeClustCf (Specific Node)
void TestGetNodeClustCfSpecific() {
  // Test TUNGraph
  printf("---- TestGetClustCfSpecific - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(GraphTUN, i);
    printf("\tQuery Node: %d | Clustering Coefficient: %f\n", i, ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfSpecific - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(GraphTN, i);
    printf("\tQuery Node: %d | Clustering Coefficient: %f\n", i, ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfSpecific - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(GraphTNE, i);
    printf("\tQuery Node: %d | Clustering Coefficient: %f\n", i, ClustCf);
  }

  printf("\n");
}

// Test GetNodeClustCf (Vector)
// Gets the Clustering Coefficient at Queried Node but in Vector Format
void TestGetNodeClustCfVector() {
  // Test TUNGraph
  printf("---- TestGetClustCfVector - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TIntFltH NIdCCfH;
  TSnap::GetNodeClustCf(GraphTUN, NIdCCfH);
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    printf("\tQuery Node in Vector: %d | Clustering Coefficient: %f\n", i, 
           ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfVector - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  NIdCCfH.Clr();
  TSnap::GetNodeClustCf(GraphTN, NIdCCfH);
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    printf("\tQuery Node in Vector: %d | Clustering Coefficient: %f\n", i, 
           ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  printf("---- TestGetClustCfVector - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  NIdCCfH.Clr();

  TSnap::GetNodeClustCf(GraphTNE, NIdCCfH);
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  printf("\tNumber of Edges Participating in at least one triad: %d\n", 
         TSnap::GetTriadEdges(GraphTUN));

  // TNGraph is not treated the same! Each directed will be counted
  printf("---- TestGetTriadEdges - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  printf("\tNumber of Edges Participating in at least one triad: %d\n", 
         TSnap::GetTriadEdges(GraphTN));

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetTriadEdges - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  printf("\tNumber of Edges Participating in at least one triad: %d\n", 
         TSnap::GetTriadEdges(GraphTNE));

  printf("\n");
}

// Test GetNodeTriads
// Number of Closed Triads associated with Node NId
void TestGetNodeTriads() {
  // Test TUNGraph
  printf("---- TestGetNodeTriads - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    printf("\tNode: %d | Participating Closed Triads: %d\n", i,
           TSnap::GetNodeTriads(GraphTUN, i));
  }
  
  // TNGraph is not treated the same! Each directed will be counted
  printf("---- TestGetNodeTriads - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    printf("\tNode: %d | Participating Closed Triads: %d\n", i,
           TSnap::GetNodeTriads(GraphTN, i));
  }

  // TNEGraph is not treated the same! Be careful with multigraphs
  printf("---- TestGetNodeTriads - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    printf("\tNode: %d | Participating Closed Triads: %d\n", i,
           TSnap::GetNodeTriads(GraphTNE, i));
  }

  printf("\n");
}

// Test GetNodeTriads (Open and Closed)
void TestGetNodeCOTriads() {
  // Test TUNGraph
  printf("---- TestGetNodeCOTriads - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    int ClosedTr = -1, OpenTr = -1;
    TSnap::GetNodeTriads(GraphTUN, i, ClosedTr, OpenTr);
    printf("\tNode: %d | Closed Triads: %d | Open Triads: %d\n", i,
           ClosedTr, OpenTr);
  }
  
  // Test TNGraph which is treated same as undirected.
  printf("---- TestGetNodeCOTriads - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    int ClosedTr = -1, OpenTr = -1;
    TSnap::GetNodeTriads(GraphTN, i, ClosedTr, OpenTr);
    printf("\tNode: %d | Closed Triads: %d | Open Triads: %d\n", i,
           ClosedTr, OpenTr);
  }

  // Test TNEGraph which is treated same as undirected.
  printf("---- TestGetNodeCOTriads - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    int ClosedTr = -1, OpenTr = -1;
    TSnap::GetNodeTriads(GraphTNE, i, ClosedTr, OpenTr);
    printf("\tNode: %d | Closed Triads: %d | Open Triads: %d\n", i,
           ClosedTr, OpenTr);
  }

  printf("\n");
}

// Test GetNodeTriads (GroupSet, InGroupEdges, InOutGroupEdges, OutGroupEdges)
void TestGetNodeTriadsGroupSetAndOut() {
  // Test TUNGraph
  printf("---- TestGetNodeTriadsGroupSetAndOut - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(GraphTUN, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    printf("\tNode: %d | InGroupEdges: %d |", i, InGroupEdges);
    printf(" InOutGroupEdges: %d | OutGroupEdges: %d\n", InOutGroupEdges, 
           OutGroupEdges);
  }
  
  // Test TNGraph which is same as undirected.
  printf("---- TestGetNodeTriadsGroupSetAndOut - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(GraphTN, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    printf("\tNode: %d | InGroupEdges: %d |", i, InGroupEdges);
    printf(" InOutGroupEdges: %d | OutGroupEdges: %d\n", InOutGroupEdges, 
           OutGroupEdges);
  }
  
  // Test TNEGraph which is same as undirected.
  printf("---- TestGetNodeTriadsGroupSetAndOut - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(GraphTNE, i, GroupSet, 
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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TIntPrV TriadCntV;
  TSnap::GetTriadParticip(GraphTUN, TriadCntV);
  for (TIntPr *vec = TriadCntV.BegI(); vec < TriadCntV.EndI(); vec++) {
    printf("\tNode %d participates in %d triangles.\n", vec->Val1.Val,
           vec->Val2.Val);
  }

  // Test TNGraph which is same as undirected.
  printf("---- TestGetTriadParticip - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(GraphTN, TriadCntV);
  for (TIntPr *vec = TriadCntV.BegI(); vec < TriadCntV.EndI(); vec++) {
    printf("\tNode %d participates in %d triangles.\n", vec->Val1.Val,
           vec->Val2.Val);
  }
  
  // Test TNEGraph which is same as undirected.
  printf("---- TestGetTriadParticip - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(GraphTN, TriadCntV);
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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTUN->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d common neighbors.\n", i, j, 
             TSnap::GetCmnNbrs(GraphTUN, i, j));
    }
  }
  
  // Test TNGraph which is same as undirected.
  printf("---- TestGetCmnNbrs - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTN->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d common neighbors.\n", i, j, 
             TSnap::GetCmnNbrs(GraphTN, i, j));
    }
  }
  
  // Test TNEGraph which is same as undirected.
  printf("---- TestGetCmnNbrs - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTNE->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d common neighbors.\n", i, j, 
             TSnap::GetCmnNbrs(GraphTNE, i, j));
    }
  }

  printf("\n");
}

// Test GetLen2Paths: Number of path lengths 2 between pair of nodes
void TestGetLen2Paths() {
  // Test TUNGraph
  printf("---- TestGetLen2Paths - TUNGraph ----\n");
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTUN->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d paths with length 2 between them.\n",
             i, j, TSnap::GetLen2Paths(GraphTUN, i, j));
    }
    printf("\n");
  }
  
  // Test TNGraph which is different from undirected due to out neighbors.
  // Direction matters in pathing!
  printf("---- TestGetLen2Paths - TNGraph ----\n");
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    for (int j = 0; j < GraphTN->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d paths with length 2 between them.\n",
             i, j, TSnap::GetLen2Paths(GraphTN, i, j));
    }
    printf("\n");
  }

  // Test TNEGraph which is different from undirected due to out neighbors.
  // Direction matters in pathing!
  printf("---- TestGetLen2Paths - TNEGraph ----\n");
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    for (int j = 0; j < GraphTNE->GetNodes(); j++) {
      printf("\tNodes %d and %d share %d paths with length 2 between them.\n",
             i, j, TSnap::GetLen2Paths(GraphTNE, i, j));
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
