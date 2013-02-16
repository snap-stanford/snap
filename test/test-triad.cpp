#include <gtest/gtest.h>

#include "Snap.h"

PUNGraph TriadGetTestTUNGraph();
PNGraph TriadGetTestTNGraph();
PNEGraph TriadGetTestTNEGraph();
void TestOpenCloseVector(TIntTrV& NIdCOTriadV);
void TestDegToCCfVector(TFltPrV& DegToCCfV);
void VerifyNodeClustCf(int NId, double ClustCf);
void VerifyClosedTriads(int NId, int ClosedTriads);
void VerifyOpenTriads(int NId, int OpenTriads);
void GetGroupSet(int NId, TIntSet& GroupSet);
void VerifyInGroupEdges(int NId, int InGroupEdges);
void VerifyInOutGroupEdges(int NId, int InOutGroupEdges);
void VerifyOutGroupEdges(int NId, int OutGroupEdges);
void VerifyGetTriadParticip(TIntPrV& TriadCntV);
void VerifyCmnNbrs(int NId1, int NId2, int CmnNbrs);
void VerifyLen2Paths(int NId1, int NId2, int Len2Paths, int type);

// Test GetTriads Close/Open Triad Vector for each NId
TEST(triad, TestGetTriadsVector) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TIntTrV NIdCOTriadV;
  
  TSnap::GetTriads(Graph_TUNGraph, NIdCOTriadV);
  TestOpenCloseVector(NIdCOTriadV);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  NIdCOTriadV.Clr();
  ASSERT_TRUE(NIdCOTriadV.Empty());

  TSnap::GetTriads(Graph_TNGraph, NIdCOTriadV);
  TestOpenCloseVector(NIdCOTriadV);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  NIdCOTriadV.Clr();
  ASSERT_TRUE(NIdCOTriadV.Empty());
  TestOpenCloseVector(NIdCOTriadV);

}

// Test GetTriads for Whole Graph (Open and Closed)
TEST(triad, TestGetTriadsOpenClosed) {
  const int expected_closed = 3;  // Expected closed triads
  const int expected_open = 9;    // Expected open triads

  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  int64 closed = 0, open = 0;

  TSnap::GetTriads(Graph_TUNGraph, closed, open);
  EXPECT_EQ(expected_closed, closed);
  EXPECT_EQ(expected_open, open);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  closed = 0, open = 0;

  TSnap::GetTriads(Graph_TNGraph, closed, open);
  EXPECT_EQ(expected_closed, closed);
  EXPECT_EQ(expected_open, open);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  closed = 0, open = 0;

  TSnap::GetTriads(Graph_TNEGraph, closed, open);
  EXPECT_EQ(expected_closed, closed);
  EXPECT_EQ(expected_open, open);
}

// Test GetTriads for Whole Graph (Only Closed)
TEST(triad, TestGetTriadsClosed) {
  const int expected_closed = 3; // Expected closed triads

  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  int closed = 0;

  closed = TSnap::GetTriads(Graph_TUNGraph);
  EXPECT_EQ(expected_closed, closed);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  closed = 0;

  closed = TSnap::GetTriads(Graph_TNGraph);
  EXPECT_EQ(expected_closed, closed);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  closed = 0;

  closed = TSnap::GetTriads(Graph_TNEGraph);
  EXPECT_EQ(expected_closed, closed);
}

// Test GetClustCf (Average Clustering Coefficient)
TEST(triad, TestGetClustCf) {
  const double expected_ClustCf = 109.0/180.0;  // Expected ClustCf (0.60555..)

  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  double ClustCf = 0.0;

  ClustCf = TSnap::GetClustCf(Graph_TUNGraph);
  EXPECT_EQ(expected_ClustCf, ClustCf);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  ClustCf = 0.0;

  ClustCf = TSnap::GetClustCf(Graph_TNGraph);
  EXPECT_EQ(expected_ClustCf, ClustCf);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  ClustCf = 0.0;

  ClustCf = TSnap::GetClustCf(Graph_TNEGraph);
  EXPECT_EQ(expected_ClustCf, ClustCf);
}

// Test GetClustCf (Distribution of Average Clustering Coefficients)
TEST(triad, TestGetClustCfDist) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;

  TSnap::GetClustCf(Graph_TUNGraph, DegToCCfV);
  TestDegToCCfVector(DegToCCfV);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  DegToCCfV.Clr();

  TSnap::GetClustCf(Graph_TNGraph, DegToCCfV);
  TestDegToCCfVector(DegToCCfV);

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  
  TSnap::GetClustCf(Graph_TNEGraph, DegToCCfV);
  for (TFltPr *pair = DegToCCfV.BegI(); pair < DegToCCfV.EndI(); pair++) {
    double diff = pair->Val2 - 5.0/9.0;   // Used for case 4
    diff = (diff < 0) ? -1.0*diff : diff; // Used for case 4
    switch ((int) pair->Val1) {
      case 2:
        EXPECT_EQ(1.0, pair->Val2);
        break;
      case 4:
        EXPECT_GT(0.00001, diff); // Due to floats being imprecise
        break;
      case 7:
        EXPECT_EQ(2.0/3.0, pair->Val2);
        break;
      case 15:
        EXPECT_EQ(0.3, pair->Val2);
        break;
      default:
        ASSERT_FALSE(true); // Shouldn't have degrees other than listed
        break;
    }
  }
}

// Test GetClustCf (Distribution and Closed and Open)
TEST(triad, TestGetClustCfDistCO) {
  const int expected_closed = 3;  // Expected closed triads
  const int expected_open = 9;    // Expected open triads

  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;
  int64 closed = 0, open = 0;

  TSnap::GetClustCf(Graph_TUNGraph, DegToCCfV, closed, open);
  TestDegToCCfVector(DegToCCfV);
  EXPECT_EQ(expected_closed, closed);
  EXPECT_EQ(expected_open, open);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  DegToCCfV.Clr();
  closed = 0, open = 0;

  TSnap::GetClustCf(Graph_TNGraph, DegToCCfV, closed, open);
  TestDegToCCfVector(DegToCCfV);
  EXPECT_EQ(expected_closed, closed);
  EXPECT_EQ(expected_open, open);

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  closed = 0, open = 0;

  TSnap::GetClustCf(Graph_TNEGraph, DegToCCfV, closed, open);
  for (TFltPr *pair = DegToCCfV.BegI(); pair < DegToCCfV.EndI(); pair++) {
    double diff = pair->Val2 - 5.0/9.0;   // Used for case 4
    diff = (diff < 0) ? -1.0*diff : diff; // Used for case 4
    switch ((int) pair->Val1) {
      case 2:
        EXPECT_EQ(1.0, pair->Val2);
        break;
      case 4:
        EXPECT_GT(0.00001, diff); // Due to floats being imprecise
        break;
      case 7:
        EXPECT_EQ(2.0/3.0, pair->Val2);
        break;
      case 15:
        EXPECT_EQ(0.3, pair->Val2);
        break;
      default:
        ASSERT_FALSE(true); // Shouldn't have degrees other than listed
        break;
    }
  }
  EXPECT_EQ(expected_closed, closed);
  EXPECT_EQ(expected_open, open);
}

// Test GetNodeClustCf (Specific Node)
TEST(triad, TestGetNodeClustCfSpecific) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();

  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(Graph_TUNGraph, i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();

  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(Graph_TNGraph, i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();

  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(Graph_TNEGraph, i);
    VerifyNodeClustCf(i, ClustCf);
  }
}

// Test GetNodeClustCf (Vector)
TEST(triad, TestGetNodeClustCfVector) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TIntFltH NIdCCfH;

  TSnap::GetNodeClustCf(Graph_TUNGraph, NIdCCfH);
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  NIdCCfH.Clr();

  TSnap::GetNodeClustCf(Graph_TNGraph, NIdCCfH);
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  NIdCCfH.Clr();

  TSnap::GetNodeClustCf(Graph_TNEGraph, NIdCCfH);
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    VerifyNodeClustCf(i, ClustCf);
  }
}

// Test GetTriadEdges
TEST(triad, TestGetTriadEdges) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  EXPECT_EQ(7, TSnap::GetTriadEdges(Graph_TUNGraph));

  // TNGraph is not treated the same! Each directed will be counted
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  EXPECT_EQ(7, TSnap::GetTriadEdges(Graph_TNGraph));

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  EXPECT_EQ(14, TSnap::GetTriadEdges(Graph_TNEGraph));
}

// Test GetNodeTriads
TEST(triad, TestGetNodeTriads) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    VerifyClosedTriads(i, TSnap::GetNodeTriads(Graph_TUNGraph, i));
  }
  
  // TNGraph is not treated the same! Each directed will be counted
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    VerifyClosedTriads(i, TSnap::GetNodeTriads(Graph_TNGraph, i));
  }

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    VerifyClosedTriads(i, TSnap::GetNodeTriads(Graph_TNEGraph, i));
  }
}

// Test GetNodeTriads (Open and Closed)
TEST(triad, TestGetNodeCOTriads) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    int ClosedTriads = -1, OpenTriads = -1;
    TSnap::GetNodeTriads(Graph_TUNGraph, i, ClosedTriads, OpenTriads);
    VerifyClosedTriads(i, ClosedTriads);
    VerifyOpenTriads(i, OpenTriads);
  }
  
  // Test TNGraph which is treated same as undirected.
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    int ClosedTriads = -1, OpenTriads = -1;
    TSnap::GetNodeTriads(Graph_TNGraph, i, ClosedTriads, OpenTriads);
    VerifyClosedTriads(i, ClosedTriads);
    VerifyOpenTriads(i, OpenTriads);
  }

  // Test TNEGraph which is treated same as undirected.
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    int ClosedTriads = -1, OpenTriads = -1;
    TSnap::GetNodeTriads(Graph_TNEGraph, i, ClosedTriads, OpenTriads);
    VerifyClosedTriads(i, ClosedTriads);
    VerifyOpenTriads(i, OpenTriads);
  }
}

// Test GetNodeTriads (GroupSet, InGroupEdges, InOutGroupEdges, OutGroupEdges)
TEST(triad, TestGetNodeTriadsGroupSetAndOut) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(Graph_TUNGraph, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    VerifyInGroupEdges(i, InGroupEdges);
    VerifyInOutGroupEdges(i, InOutGroupEdges);
    VerifyOutGroupEdges(i, OutGroupEdges);
  }
  
  // Test TNGraph which is same as undirected.
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(Graph_TNGraph, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    VerifyInGroupEdges(i, InGroupEdges);
    VerifyInOutGroupEdges(i, InOutGroupEdges);
    VerifyOutGroupEdges(i, OutGroupEdges);
  }
  
  // Test TNEGraph which is same as undirected.
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(Graph_TNEGraph, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    VerifyInGroupEdges(i, InGroupEdges);
    VerifyInOutGroupEdges(i, InOutGroupEdges);
    VerifyOutGroupEdges(i, OutGroupEdges);
  }
}

// Test GetTriadParticip
// Number of nodes with x number of triangles it participates in
TEST(triad, TestGetTriadParticip) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  TIntPrV TriadCntV;
  TSnap::GetTriadParticip(Graph_TUNGraph, TriadCntV);
  VerifyGetTriadParticip(TriadCntV);

  // Test TNGraph which is same as undirected.
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(Graph_TNGraph, TriadCntV);
  VerifyGetTriadParticip(TriadCntV);
  
  // Test TNEGraph which is same as undirected.
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(Graph_TNGraph, TriadCntV);
  VerifyGetTriadParticip(TriadCntV);
}

// Test GetCmnNbrs: the number of neighbors in common
TEST(triad, TestGetCmnNbrs) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TUNGraph->GetNodes(); j++) {
      VerifyCmnNbrs(i, j, TSnap::GetCmnNbrs(Graph_TUNGraph, i, j));
    }
  }
  
  // Test TNGraph which is same as undirected.
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TNGraph->GetNodes(); j++) {
      VerifyCmnNbrs(i, j, TSnap::GetCmnNbrs(Graph_TNGraph, i, j));
    }
  }
  
  // Test TNEGraph which is same as undirected.
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TNEGraph->GetNodes(); j++) {
      VerifyCmnNbrs(i, j, TSnap::GetCmnNbrs(Graph_TNEGraph, i, j));
    }
  }
}

// Test GetLen2Paths: Number of path lengths 2 between pair of nodes
TEST(triad, TestGetLen2Paths) {
  // Test TUNGraph
  PUNGraph Graph_TUNGraph = TriadGetTestTUNGraph();
  for (int i = 0; i < Graph_TUNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TUNGraph->GetNodes(); j++) {
      VerifyLen2Paths(i, j, TSnap::GetLen2Paths(Graph_TUNGraph, i, j), 0);
    }
  }
  
  // Test TNGraph which is different from undirected due to out neighbors.
  PNGraph Graph_TNGraph = TriadGetTestTNGraph();
  for (int i = 0; i < Graph_TNGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TNGraph->GetNodes(); j++) {
      VerifyLen2Paths(i, j, TSnap::GetLen2Paths(Graph_TNGraph, i, j), 1);
    }
  }

  // Test TNEGraph which is different from undirected due to out neighbors.
  PNEGraph Graph_TNEGraph = TriadGetTestTNEGraph();
  for (int i = 0; i < Graph_TNEGraph->GetNodes(); i++) {
    for (int j = i + 1; j < Graph_TNEGraph->GetNodes(); j++) {
      VerifyLen2Paths(i, j, TSnap::GetLen2Paths(Graph_TNEGraph, i, j), 2);
    }
  }
}

// Helper: Testing Opened/Closed Triads for Specific Generated Graph
void TestOpenCloseVector(TIntTrV& NIdCOTriadV) {
  for (TIntTr *vec = NIdCOTriadV.BegI(); vec < NIdCOTriadV.EndI(); vec++) {
    switch (vec->Val1) {
      case 0:
        EXPECT_EQ(3, vec->Val2);
        EXPECT_EQ(7, vec->Val3);
        break;
      case 1:
        EXPECT_EQ(1, vec->Val2);
        EXPECT_EQ(0, vec->Val3);
        break;
      case 2:
        EXPECT_EQ(2, vec->Val2);
        EXPECT_EQ(1, vec->Val3);
        break;
      case 3:
        EXPECT_EQ(1, vec->Val2);
        EXPECT_EQ(0, vec->Val3);
        break;
      case 4:
        EXPECT_EQ(0, vec->Val2);
        EXPECT_EQ(0, vec->Val3);
        break;
      case 5:
        EXPECT_EQ(2, vec->Val2);
        EXPECT_EQ(1, vec->Val3);
        break;
      default:
        ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
        break;
    }
  }
}

// Helper: Testing Degree to Clustering Coefficient Distribution
void TestDegToCCfVector(TFltPrV& DegToCCfV) {
  for (TFltPr *pair = DegToCCfV.BegI(); pair < DegToCCfV.EndI(); pair++) {
    switch ((int) pair->Val1) {
      case 1:
        EXPECT_EQ(0.0, pair->Val2);
        break;
      case 2:
        EXPECT_EQ(1.0, pair->Val2);
        break;
      case 3:
        EXPECT_EQ(2.0/3.0, pair->Val2);
        break;
      case 5:
        EXPECT_EQ(0.3, pair->Val2);
        break;
      default:
        ASSERT_FALSE(true); // Shouldn't have degrees other than listed
        break;
    }
  }
}

// Helper: Tests the Clustering Coefficient for a Node
void VerifyNodeClustCf(int NId, double ClustCf) {
  switch (NId) {
    case 0:
      EXPECT_EQ(0.3, ClustCf);
      break;
    case 1:
      EXPECT_EQ(1.0, ClustCf);
      break;
    case 2:
      EXPECT_EQ(2.0/3.0, ClustCf);
      break;
    case 3:
      EXPECT_EQ(1, ClustCf);
      break;
    case 4:
      EXPECT_EQ(0, ClustCf);
      break;
    case 5:
      EXPECT_EQ(2.0/3.0, ClustCf);
      break;
    default:
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
}

// Helper: Tests the Closed Triads a node participates in
void VerifyClosedTriads(int NId, int ClosedTriads) {
  switch (NId) {
    case 0:
      EXPECT_EQ(3, ClosedTriads);
      break;
    case 1:
      EXPECT_EQ(1, ClosedTriads);
      break;
    case 2:
      EXPECT_EQ(2, ClosedTriads);
      break;
    case 3:
      EXPECT_EQ(1, ClosedTriads);
      break;
    case 4:
      EXPECT_EQ(0, ClosedTriads);
      break;
    case 5:
      EXPECT_EQ(2, ClosedTriads);
      break;
    default:
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
}

// Helper: Tests the Open Triads a node participates in
void VerifyOpenTriads(int NId, int OpenTriads) {
  switch (NId) {
    case 0:
      EXPECT_EQ(7, OpenTriads);
      break;
    case 1:
      EXPECT_EQ(0, OpenTriads);
      break;
    case 2:
      EXPECT_EQ(1, OpenTriads);
      break;
    case 3:
      EXPECT_EQ(0, OpenTriads);
      break;
    case 4:
      EXPECT_EQ(0, OpenTriads);
      break;
    case 5:
      EXPECT_EQ(1, OpenTriads);
      break;
    default:
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
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
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
}

// Helper: Tests InGroupEdges for GetNodeTriads
void VerifyInGroupEdges(int NId, int InGroupEdges) {
  switch (NId) {
    case 0:
      EXPECT_EQ(1, InGroupEdges);
      break;
    case 1:
      EXPECT_EQ(0, InGroupEdges);
      break;
    case 2:
      EXPECT_EQ(2, InGroupEdges);
      break;
    case 3:
      EXPECT_EQ(0, InGroupEdges);
      break;
    case 4:
      EXPECT_EQ(0, InGroupEdges);
      break;
    case 5:
      EXPECT_EQ(1, InGroupEdges);
      break;
    default:
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
}

// Helper: Tests InOutGroupEdges for GetNodeTriads
void VerifyInOutGroupEdges(int NId, int InOutGroupEdges) {
  switch (NId) {
    case 0:
      EXPECT_EQ(2, InOutGroupEdges);
      break;
    case 1:
      EXPECT_EQ(0, InOutGroupEdges);
      break;
    case 2:
      EXPECT_EQ(0, InOutGroupEdges);
      break;
    case 3:
      EXPECT_EQ(1, InOutGroupEdges);
      break;
    case 4:
      EXPECT_EQ(0, InOutGroupEdges);
      break;
    case 5:
      EXPECT_EQ(1, InOutGroupEdges);
      break;
    default:
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
}

// Helper: Tests OutGroupEdges for GetNodeTriads
void VerifyOutGroupEdges(int NId, int OutGroupEdges) {
  switch (NId) {
    case 0:
      EXPECT_EQ(0, OutGroupEdges);
      break;
    case 1:
      EXPECT_EQ(1, OutGroupEdges);
      break;
    case 2:
      EXPECT_EQ(0, OutGroupEdges);
      break;
    case 3:
      EXPECT_EQ(0, OutGroupEdges);
      break;
    case 4:
      EXPECT_EQ(0, OutGroupEdges);
      break;
    case 5:
      EXPECT_EQ(0, OutGroupEdges);
      break;
    default:
      ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
      break;
  }
}

// Helper: Verify the distribution of how many triangles a node participates in
void VerifyGetTriadParticip(TIntPrV& TriadCntV) {
  for (TIntPr *vec = TriadCntV.BegI(); vec < TriadCntV.EndI(); vec++) {
    switch (vec->Val1) {
      case 0:
        EXPECT_EQ(1, vec->Val2);
        break;
      case 1:
        EXPECT_EQ(2, vec->Val2);
        break;
      case 2:
        EXPECT_EQ(2, vec->Val2);
        break;
      case 3:
        EXPECT_EQ(1, vec->Val2);
        break;
      default:
        EXPECT_FALSE(true); // Not suppose to have more than 3
        break;
    }
  }
}

// Helper: Verify the common neighbors between nodes
void VerifyCmnNbrs(int NId1, int NId2, int CmnNbrs) {
  switch (NId1) {
    case 0:
      switch (NId2) {
        case 1:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 2:
          EXPECT_EQ(2, CmnNbrs);
          break;
        case 3:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 4:
          EXPECT_EQ(0, CmnNbrs);
          break;
        case 5:
          EXPECT_EQ(2, CmnNbrs);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 1:
      switch (NId2) {
        case 2:
          EXPECT_EQ(2, CmnNbrs);
          break;
        case 3:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 4:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 5:
          EXPECT_EQ(1, CmnNbrs);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 2:
      switch (NId2) {
        case 3:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 4:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 5:
          EXPECT_EQ(1, CmnNbrs);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 3:
      switch (NId2) {
        case 4:
          EXPECT_EQ(1, CmnNbrs);
          break;
        case 5:
          EXPECT_EQ(2, CmnNbrs);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 4:
      switch (NId2) {
        case 5:
          EXPECT_EQ(1, CmnNbrs);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    default:
      EXPECT_FALSE(true); // Should not get to here
      break;
  }
}

// Helper: Verify the number of paths of length 2
// Type: 0 = Undirected | 1 = Directed | 2 = Multi-Directed
void VerifyLen2Paths(int NId1, int NId2, int Len2Paths, int type) {
  switch (NId1) {
    case 0:
      switch (NId2) {
        case 1:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 2:
          if (type == 0)
            EXPECT_EQ(2, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 3:
          if (type == 0 || type == 1)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 2)
            EXPECT_EQ(2, Len2Paths);
          break;
        case 4:
          EXPECT_EQ(0, Len2Paths);
          break;
        case 5:
          if (type == 0 || type == 1)
            EXPECT_EQ(2, Len2Paths);
          else if (type == 2)
            EXPECT_EQ(3, Len2Paths);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 1:
      switch (NId2) {
        case 2:
          if (type == 0)
            EXPECT_EQ(2, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 3:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 4:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 5:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 2:
      switch (NId2) {
        case 3:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 4:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 5:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 3:
      switch (NId2) {
        case 4:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        case 5:
          if (type == 0)
            EXPECT_EQ(2, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    case 4:
      switch (NId2) {
        case 5:
          if (type == 0)
            EXPECT_EQ(1, Len2Paths);
          else if (type == 1 || type == 2)
            EXPECT_EQ(0, Len2Paths);
          break;
        default:
          EXPECT_FALSE(true); // Should not get to here
          break;
      }
      break;
    default:
      EXPECT_FALSE(true); // Should not get to here
      break;
  }
}

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
