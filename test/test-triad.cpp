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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TIntTrV NIdCOTriadV;
  
  TSnap::GetTriads(GraphTUN, NIdCOTriadV);
  TestOpenCloseVector(NIdCOTriadV);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();
  NIdCOTriadV.Clr();
  ASSERT_TRUE(NIdCOTriadV.Empty());

  TSnap::GetTriads(GraphTN, NIdCOTriadV);
  TestOpenCloseVector(NIdCOTriadV);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  NIdCOTriadV.Clr();
  ASSERT_TRUE(NIdCOTriadV.Empty());
  TestOpenCloseVector(NIdCOTriadV);

}

// Test GetTriads for Whole Graph (Open and Closed)
TEST(triad, TestGetTriadsOpenClosed) {
  const int ExpClosedTr = 3;  // Expected closed triads
  const int ExpOpenTr = 9;    // Expected open triads

  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  int64 ClosedTr = 0;
  int64 OpenTr = 0;

  TSnap::GetTriads(GraphTUN, ClosedTr, OpenTr);
  EXPECT_EQ(ExpClosedTr, ClosedTr);
  EXPECT_EQ(ExpOpenTr, OpenTr);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();
  ClosedTr = 0;
  OpenTr = 0;

  TSnap::GetTriads(GraphTN, ClosedTr, OpenTr);
  EXPECT_EQ(ExpClosedTr, ClosedTr);
  EXPECT_EQ(ExpOpenTr, OpenTr);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  ClosedTr = 0;
  OpenTr = 0;

  TSnap::GetTriads(GraphTNE, ClosedTr, OpenTr);
  EXPECT_EQ(ExpClosedTr, ClosedTr);
  EXPECT_EQ(ExpOpenTr, OpenTr);
}

// Test GetTriads for Whole Graph (Only Closed)
TEST(triad, TestGetTriadsClosed) {
  const int ExpClosedTr = 3; // Expected closed triads
  int64 ClosedTr = 0;

  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();

  ClosedTr = TSnap::GetTriads(GraphTUN);
  EXPECT_EQ(ExpClosedTr, ClosedTr);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();

  ClosedTr = TSnap::GetTriads(GraphTN);
  EXPECT_EQ(ExpClosedTr, ClosedTr);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph GraphTNE = TriadGetTestTNEGraph();

  ClosedTr = TSnap::GetTriads(GraphTNE);
  EXPECT_EQ(ExpClosedTr, ClosedTr);
}

// Test GetClustCf (Average Clustering Coefficient)
TEST(triad, TestGetClustCf) {
  const double ExpClustCf = 109.0/180.0;  // Expected ClustCf (0.60555..)

  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  double ClustCf = 0.0;

  ClustCf = TSnap::GetClustCf(GraphTUN);
  EXPECT_EQ(ExpClustCf, ClustCf);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();

  ClustCf = TSnap::GetClustCf(GraphTN);
  EXPECT_EQ(ExpClustCf, ClustCf);

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph GraphTNE = TriadGetTestTNEGraph();

  ClustCf = TSnap::GetClustCf(GraphTNE);
  EXPECT_EQ(ExpClustCf, ClustCf);
}

// Test GetClustCf (Distribution of Average Clustering Coefficients)
TEST(triad, TestGetClustCfDist) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;

  TSnap::GetClustCf(GraphTUN, DegToCCfV);
  TestDegToCCfVector(DegToCCfV);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();
  DegToCCfV.Clr();

  TSnap::GetClustCf(GraphTN, DegToCCfV);
  TestDegToCCfVector(DegToCCfV);

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  
  TSnap::GetClustCf(GraphTNE, DegToCCfV);
  for (TFltPr *Pair = DegToCCfV.BegI(); Pair < DegToCCfV.EndI(); Pair++) {
    double Diff = Pair->Val2 - 5.0/9.0;   // Used for case 4
    Diff = (Diff < 0) ? -1.0*Diff : Diff; // Used for case 4
    switch ((int) Pair->Val1) {
      case 2:
        EXPECT_EQ(1.0, Pair->Val2);
        break;
      case 4:
        EXPECT_GT(0.00001, Diff); // Due to floats being imprecise
        break;
      case 7:
        EXPECT_EQ(2.0/3.0, Pair->Val2);
        break;
      case 15:
        EXPECT_EQ(0.3, Pair->Val2);
        break;
      default:
        ASSERT_FALSE(true); // Shouldn't have degrees other than listed
        break;
    }
  }
}

// Test GetClustCf (Distribution and Closed and Open)
TEST(triad, TestGetClustCfDistCO) {
  const int ExpClosedTr = 3;  // Expected closed triads
  const int ExpOpenTr = 9;    // Expected open triads

  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TFltPrV DegToCCfV;
  int64 ClosedTr = 0;
  int64 OpenTr = 0;

  TSnap::GetClustCf(GraphTUN, DegToCCfV, ClosedTr, OpenTr);
  TestDegToCCfVector(DegToCCfV);
  EXPECT_EQ(ExpClosedTr, ClosedTr);
  EXPECT_EQ(ExpOpenTr, OpenTr);

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();
  DegToCCfV.Clr();
  ClosedTr = 0;
  OpenTr = 0;

  TSnap::GetClustCf(GraphTN, DegToCCfV, ClosedTr, OpenTr);
  TestDegToCCfVector(DegToCCfV);
  EXPECT_EQ(ExpClosedTr, ClosedTr);
  EXPECT_EQ(ExpOpenTr, OpenTr);

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  DegToCCfV.Clr();
  ClosedTr = 0;
  OpenTr = 0;

  TSnap::GetClustCf(GraphTNE, DegToCCfV, ClosedTr, OpenTr);
  for (TFltPr *Pair = DegToCCfV.BegI(); Pair < DegToCCfV.EndI(); Pair++) {
    double Diff = Pair->Val2 - 5.0/9.0;   // Used for case 4
    Diff = (Diff < 0) ? -1.0*Diff : Diff; // Used for case 4
    switch ((int) Pair->Val1) {
      case 2:
        EXPECT_EQ(1.0, Pair->Val2);
        break;
      case 4:
        EXPECT_GT(0.00001, Diff); // Due to floats being imprecise
        break;
      case 7:
        EXPECT_EQ(2.0/3.0, Pair->Val2);
        break;
      case 15:
        EXPECT_EQ(0.3, Pair->Val2);
        break;
      default:
        ASSERT_FALSE(true); // Shouldn't have degrees other than listed
        break;
    }
  }
  EXPECT_EQ(ExpClosedTr, ClosedTr);
  EXPECT_EQ(ExpOpenTr, OpenTr);
}

// Test GetNodeClustCf (Specific Node)
TEST(triad, TestGetNodeClustCfSpecific) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();

  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(GraphTUN, i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();

  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(GraphTN, i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph GraphTNE = TriadGetTestTNEGraph();

  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    double ClustCf = TSnap::GetNodeClustCf(GraphTNE, i);
    VerifyNodeClustCf(i, ClustCf);
  }
}

// Test GetNodeClustCf (Vector)
TEST(triad, TestGetNodeClustCfVector) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TIntFltH NIdCCfH;

  TSnap::GetNodeClustCf(GraphTUN, NIdCCfH);
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNGraph should be treated as TUNGraph for calculations
  PNGraph GraphTN = TriadGetTestTNGraph();
  NIdCCfH.Clr();

  TSnap::GetNodeClustCf(GraphTN, NIdCCfH);
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    VerifyNodeClustCf(i, ClustCf);
  }

  // TNEGraph should be treated as TUNGraph for calculations
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  NIdCCfH.Clr();

  TSnap::GetNodeClustCf(GraphTNE, NIdCCfH);
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    double ClustCf = NIdCCfH.GetDat(i);
    VerifyNodeClustCf(i, ClustCf);
  }
}

// Test GetTriadEdges
TEST(triad, TestGetTriadEdges) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  EXPECT_EQ(7, TSnap::GetTriadEdges(GraphTUN));

  // TNGraph is not treated the same! Each directed will be counted
  PNGraph GraphTN = TriadGetTestTNGraph();
  EXPECT_EQ(7, TSnap::GetTriadEdges(GraphTN));

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  EXPECT_EQ(14, TSnap::GetTriadEdges(GraphTNE));
}

// Test GetNodeTriads
TEST(triad, TestGetNodeTriads) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    VerifyClosedTriads(i, TSnap::GetNodeTriads(GraphTUN, i));
  }
  
  // TNGraph is not treated the same! Each directed will be counted
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    VerifyClosedTriads(i, TSnap::GetNodeTriads(GraphTN, i));
  }

  // TNEGraph is not treated the same! Be careful with multigraphs
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    VerifyClosedTriads(i, TSnap::GetNodeTriads(GraphTNE, i));
  }
}

// Test GetNodeTriads (Open and Closed)
TEST(triad, TestGetNodeCOTriads) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    int ClosedTr = -1, OpenTr = -1;
    TSnap::GetNodeTriads(GraphTUN, i, ClosedTr, OpenTr);
    VerifyClosedTriads(i, ClosedTr);
    VerifyOpenTriads(i, OpenTr);
  }
  
  // Test TNGraph which is treated same as undirected.
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    int ClosedTr = -1, OpenTr = -1;
    TSnap::GetNodeTriads(GraphTN, i, ClosedTr, OpenTr);
    VerifyClosedTriads(i, ClosedTr);
    VerifyOpenTriads(i, OpenTr);
  }

  // Test TNEGraph which is treated same as undirected.
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    int ClosedTr = -1, OpenTr = -1;
    TSnap::GetNodeTriads(GraphTNE, i, ClosedTr, OpenTr);
    VerifyClosedTriads(i, ClosedTr);
    VerifyOpenTriads(i, OpenTr);
  }
}

// Test GetNodeTriads (GroupSet, InGroupEdges, InOutGroupEdges, OutGroupEdges)
TEST(triad, TestGetNodeTriadsGroupSetAndOut) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(GraphTUN, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    VerifyInGroupEdges(i, InGroupEdges);
    VerifyInOutGroupEdges(i, InOutGroupEdges);
    VerifyOutGroupEdges(i, OutGroupEdges);
  }
  
  // Test TNGraph which is same as undirected.
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(GraphTN, i, GroupSet, 
                         InGroupEdges, InOutGroupEdges, OutGroupEdges);
    VerifyInGroupEdges(i, InGroupEdges);
    VerifyInOutGroupEdges(i, InOutGroupEdges);
    VerifyOutGroupEdges(i, OutGroupEdges);
  }
  
  // Test TNEGraph which is same as undirected.
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    TIntSet GroupSet;
    GetGroupSet(i, GroupSet);
    int InGroupEdges = -1, InOutGroupEdges = -1, OutGroupEdges = -1;
    TSnap::GetNodeTriads(GraphTNE, i, GroupSet, 
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
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  TIntPrV TriadCntV;
  TSnap::GetTriadParticip(GraphTUN, TriadCntV);
  VerifyGetTriadParticip(TriadCntV);

  // Test TNGraph which is same as undirected.
  PNGraph GraphTN = TriadGetTestTNGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(GraphTN, TriadCntV);
  VerifyGetTriadParticip(TriadCntV);
  
  // Test TNEGraph which is same as undirected.
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  TriadCntV.Clr();
  TSnap::GetTriadParticip(GraphTN, TriadCntV);
  VerifyGetTriadParticip(TriadCntV);
}

// Test GetCmnNbrs: the number of neighbors in common
TEST(triad, TestGetCmnNbrs) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTUN->GetNodes(); j++) {
      VerifyCmnNbrs(i, j, TSnap::GetCmnNbrs(GraphTUN, i, j));
    }
  }
  
  // Test TNGraph which is same as undirected.
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTN->GetNodes(); j++) {
      VerifyCmnNbrs(i, j, TSnap::GetCmnNbrs(GraphTN, i, j));
    }
  }
  
  // Test TNEGraph which is same as undirected.
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTNE->GetNodes(); j++) {
      VerifyCmnNbrs(i, j, TSnap::GetCmnNbrs(GraphTNE, i, j));
    }
  }
}

// Test GetLen2Paths: Number of path lengths 2 between pair of nodes
TEST(triad, TestGetLen2Paths) {
  // Test TUNGraph
  PUNGraph GraphTUN = TriadGetTestTUNGraph();
  for (int i = 0; i < GraphTUN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTUN->GetNodes(); j++) {
      VerifyLen2Paths(i, j, TSnap::GetLen2Paths(GraphTUN, i, j), 0);
    }
  }
  
  // Test TNGraph which is different from undirected due to out neighbors.
  PNGraph GraphTN = TriadGetTestTNGraph();
  for (int i = 0; i < GraphTN->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTN->GetNodes(); j++) {
      VerifyLen2Paths(i, j, TSnap::GetLen2Paths(GraphTN, i, j), 1);
    }
  }

  // Test TNEGraph which is different from undirected due to out neighbors.
  PNEGraph GraphTNE = TriadGetTestTNEGraph();
  for (int i = 0; i < GraphTNE->GetNodes(); i++) {
    for (int j = i + 1; j < GraphTNE->GetNodes(); j++) {
      VerifyLen2Paths(i, j, TSnap::GetLen2Paths(GraphTNE, i, j), 2);
    }
  }
}

// Helper: Testing Opened/Closed Triads for Specific Generated Graph
void TestOpenCloseVector(TIntTrV& NIdCOTriadV) {
  for (TIntTr *Vec = NIdCOTriadV.BegI(); Vec < NIdCOTriadV.EndI(); Vec++) {
    switch (Vec->Val1) {
      case 0:
        EXPECT_EQ(3, Vec->Val2);
        EXPECT_EQ(7, Vec->Val3);
        break;
      case 1:
        EXPECT_EQ(1, Vec->Val2);
        EXPECT_EQ(0, Vec->Val3);
        break;
      case 2:
        EXPECT_EQ(2, Vec->Val2);
        EXPECT_EQ(1, Vec->Val3);
        break;
      case 3:
        EXPECT_EQ(1, Vec->Val2);
        EXPECT_EQ(0, Vec->Val3);
        break;
      case 4:
        EXPECT_EQ(0, Vec->Val2);
        EXPECT_EQ(0, Vec->Val3);
        break;
      case 5:
        EXPECT_EQ(2, Vec->Val2);
        EXPECT_EQ(1, Vec->Val3);
        break;
      default:
        ASSERT_FALSE(true); // NId Outside Graph Construction FAIL
        break;
    }
  }
}

// Helper: Testing Degree to Clustering Coefficient Distribution
void TestDegToCCfVector(TFltPrV& DegToCCfV) {
  for (TFltPr *Pair = DegToCCfV.BegI(); Pair < DegToCCfV.EndI(); Pair++) {
    switch ((int) Pair->Val1) {
      case 1:
        EXPECT_EQ(0.0, Pair->Val2);
        break;
      case 2:
        EXPECT_EQ(1.0, Pair->Val2);
        break;
      case 3:
        EXPECT_EQ(2.0/3.0, Pair->Val2);
        break;
      case 5:
        EXPECT_EQ(0.3, Pair->Val2);
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
  for (TIntPr *Vec = TriadCntV.BegI(); Vec < TriadCntV.EndI(); Vec++) {
    switch (Vec->Val1) {
      case 0:
        EXPECT_EQ(1, Vec->Val2);
        break;
      case 1:
        EXPECT_EQ(2, Vec->Val2);
        break;
      case 2:
        EXPECT_EQ(2, Vec->Val2);
        break;
      case 3:
        EXPECT_EQ(1, Vec->Val2);
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
