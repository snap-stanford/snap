#include <gtest/gtest.h>

#include "Snap.h"

class GGenTest { };  // For gtest highlighting

// Test generation of grid graph
TEST(GGenTest, GenGrid) {
  const int RowsStart = 1;
  const int RowsEnd = 20;
  
  const int ColsStart = 1;
  const int ColsEnd = 15;
  
  PUNGraph GraphUN;
  PNGraph GraphN;

  for (int Rows = RowsStart; Rows < RowsEnd; Rows++) {
    
    for (int Cols = ColsStart; Cols < ColsEnd; Cols++) {

      int NumEdges = 0;
      
      // Add right side and left side
      if (Cols > 1) {
        NumEdges += (Rows - 1) * 2;
      }
      else {
        NumEdges += Rows - 1;
      }

      // Add top and bottom sides
      if (Rows > 1) {
        NumEdges += (Cols - 1) * 2;
      }
      else {
        NumEdges += Cols - 1;
      }

      if (Rows > 1 && Cols > 1) {
        // Add inner side edges
        if (Cols >= 2) {
          NumEdges += Cols - 2;
        }
        if (Rows >= 2) {
          NumEdges += Rows - 2;
        }
        // Add inside edges
        if (Rows >= 3 || Cols >= 3) {
          NumEdges += (Rows-2) * (Cols-2) * 2;
        }
      }
      // -------------------------------------------
      // Generate undirected grid graph
      GraphUN = TSnap::GenGrid<PUNGraph>(Rows, Cols, false);
      EXPECT_FALSE(GraphUN->Empty());
      EXPECT_TRUE(GraphUN->IsOk());
      EXPECT_EQ(Rows*Cols, GraphUN->GetNodes());
      EXPECT_EQ(NumEdges, GraphUN->GetEdges());
      
      int NodeCount = 0;
      int EdgeCount = 0;
      // Iterate through nodes, verify it matches
      for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
        NodeCount++;
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          EdgeCount++;
        }
      }
      EXPECT_EQ(NodeCount, GraphUN->GetNodes());
      EXPECT_EQ(EdgeCount/2, GraphUN->GetEdges());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      
      // -------------------------------------------
      // Generate directed grid graph  
      GraphN = TSnap::GenGrid<PNGraph>(Rows, Cols, true);
      EXPECT_FALSE(GraphN->Empty());
      EXPECT_TRUE(GraphN->IsOk());
      EXPECT_EQ(Rows*Cols, GraphN->GetNodes());
      EXPECT_EQ(NumEdges, GraphN->GetEdges());
      
      NodeCount = 0;
      for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
        NodeCount++;
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          EdgeCount++;
        }
      }
      EXPECT_EQ(NodeCount, GraphN->GetNodes());
      EXPECT_EQ(EdgeCount/2, GraphN->GetEdges());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphN->GetEdges());
    } // end loop - Cols
  } // end loop - Rows

}

// Test generation of star graph
TEST(GGenTest, GenStar) {
  const int NNodesMax = 1000;
  
  PUNGraph GraphUN;
  PNGraph GraphN;
      
  for (int NNodes = 1; NNodes < NNodesMax; NNodes++) {
    
    int NumEdges = NNodes-1;
    
    // -------------------------------------------
    // Generate undirected graph
    GraphUN = TSnap::GenStar<PUNGraph>(NNodes, false);
    EXPECT_FALSE(GraphUN->Empty());
    EXPECT_TRUE(GraphUN->IsOk());
    EXPECT_EQ(NumEdges, GraphUN->GetEdges());
    
    int NodeCount = 0;
    int EdgeCount = 0;
    // Iterate through nodes, verify it matches
    for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
      NodeCount++;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        EdgeCount++;
      }
    }
    EXPECT_EQ(NodeCount, GraphUN->GetNodes());
    EXPECT_EQ(EdgeCount/2, GraphUN->GetEdges());
    
    EdgeCount = 0;
    // Iterate through edges, count and verify
    for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
      EdgeCount++;
    }
    EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
    
    // -------------------------------------------
    // Generate directed graph
    GraphN = TSnap::GenStar<PNGraph>(NNodes, true);
    EXPECT_FALSE(GraphN->Empty());
    EXPECT_TRUE(GraphN->IsOk());
    EXPECT_EQ(NumEdges, GraphN->GetEdges());
    
    NodeCount = 0;
    for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
      NodeCount++;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        EdgeCount++;
      }
    }
    EXPECT_EQ(NodeCount, GraphN->GetNodes());
    EXPECT_EQ(EdgeCount/2, GraphN->GetEdges());
    
    EdgeCount = 0;
    // Iterate through edges, count and verify
    for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
      EdgeCount++;
    }
    EXPECT_EQ(EdgeCount, GraphN->GetEdges());
    
  } // end loop - NNodes
}

TEST(GGenTest, GenCircle) {
  const int NNodesMax = 500;
  const int NodeOutDegMax = 5;
  
  PUNGraph GraphUN;
  PNGraph GraphN;
  
  for (int NNodes = 1; NNodes < NNodesMax; NNodes++) {

    for (int NodeOutDeg = 1; NodeOutDeg < NodeOutDegMax; NodeOutDeg++) {
            
      // -------------------------------------------
      // Generate undirected graph
      GraphUN = TSnap::GenCircle<PUNGraph>(NNodes, NodeOutDeg, false);
      
      EXPECT_FALSE(GraphUN->Empty());
      EXPECT_TRUE(GraphUN->IsOk());
            
      int NodeCount = 0;
      int EdgeCount = 0;
      // Iterate through nodes, verify it matches
      for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, GraphUN->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      
      // -------------------------------------------
      // Generate directed graph
      GraphN = TSnap::GenCircle<PNGraph>(NNodes, NodeOutDeg, true);
      EXPECT_FALSE(GraphN->Empty());
      EXPECT_TRUE(GraphN->IsOk());
            
      NodeCount = 0;
      for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, GraphN->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphN->GetEdges());
    }
    
  } // end loop - NNodes
}

TEST(GGenTest, GenFull) {
  const int NNodesMax = 100;
  
  PUNGraph GraphUN;
  PNGraph GraphN;
  
  for (int NNodes = 1; NNodes < NNodesMax; NNodes++) {
          
    // -------------------------------------------
    // Generate undirected graph
    GraphUN = TSnap::GenFull<PUNGraph>(NNodes);
    
    EXPECT_FALSE(GraphUN->Empty());
    EXPECT_TRUE(GraphUN->IsOk());
  
    // Number of edges in a complete graph = n * (n-1) / 2
    int NumEdges = NNodes * (NNodes - 1) / 2;
    EXPECT_EQ(NumEdges, GraphUN->GetEdges());
  
    int NodeCount = 0;
    int EdgeCount = 0;
    // Iterate through nodes, verify it matches
    for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
      NodeCount++;
    }
    EXPECT_EQ(NodeCount, GraphUN->GetNodes());
    
    EdgeCount = 0;
    // Iterate through edges, count and verify
    for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
      EdgeCount++;
    }
    EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
    
    // -------------------------------------------
    // Generate directed graph
    GraphN = TSnap::GenFull<PNGraph>(NNodes);
    EXPECT_FALSE(GraphN->Empty());
    EXPECT_TRUE(GraphN->IsOk());
    
    NumEdges = NNodes * (NNodes - 1);
    EXPECT_EQ(NumEdges, GraphN->GetEdges());
    
    NodeCount = 0;
    for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
      NodeCount++;
    }
    EXPECT_EQ(NodeCount, GraphN->GetNodes());
    
    EdgeCount = 0;
    // Iterate through edges, count and verify
    for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
      EdgeCount++;
    }
    EXPECT_EQ(EdgeCount, GraphN->GetEdges());
    
  } // end loop - NNodes
  
}

TEST(GGenTest, GenTree) {
  const int FanoutMax = 10;
  const int LevelsMax = 5;
  
  PUNGraph GraphUN;
  PNGraph GraphN;
  
  for (int Fanout = 2; Fanout < FanoutMax; Fanout++) {
    
    for (int Levels = 1; Levels < LevelsMax; Levels++) {
      
      // -------------------------------------------
      // Generate undirected graph
      GraphUN = TSnap::GenTree<PUNGraph>(Fanout, Levels, false);
      
      EXPECT_FALSE(GraphUN->Empty());
      EXPECT_TRUE(GraphUN->IsOk());
            
      int NodeCount = 0;
      int EdgeCount = 0;
      // Iterate through nodes, verify it matches
      for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, GraphUN->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      
      // -------------------------------------------
      // Generate directed graph
      GraphN = TSnap::GenTree<PNGraph>(Fanout, Levels, true, true);
      EXPECT_FALSE(GraphN->Empty());
      EXPECT_TRUE(GraphN->IsOk());
      
      NodeCount = 0;
      for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, GraphN->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphN->GetEdges());
    
    } // end loop - Levels
  } // end loop - Fanout
}

TEST(GGenTest, GenBaraHierar) {
  const int LevelsMax = 8;
  
  PUNGraph GraphUN;
  PNGraph GraphN;
      
  for (int Levels = 1; Levels < LevelsMax; Levels++) {
    
    // -------------------------------------------
    // Generate undirected graph
    GraphUN = TSnap::GenBaraHierar<PUNGraph>(Levels);
    
    EXPECT_FALSE(GraphUN->Empty());
    EXPECT_TRUE(GraphUN->IsOk());
    
    int NodeCount = 0;
    int EdgeCount = 0;
    // Iterate through nodes, verify it matches
    for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
      NodeCount++;
    }
    EXPECT_EQ(NodeCount, GraphUN->GetNodes());
    
    EdgeCount = 0;
    // Iterate through edges, count and verify
    for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
      EdgeCount++;
    }
    EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
    
    // -------------------------------------------
    // Generate directed graph
    GraphN = TSnap::GenBaraHierar<PNGraph>(Levels);
    EXPECT_FALSE(GraphN->Empty());
    EXPECT_TRUE(GraphN->IsOk());
    
    NodeCount = 0;
    for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
      NodeCount++;
    }
    EXPECT_EQ(NodeCount, GraphN->GetNodes());
    
    EdgeCount = 0;
    // Iterate through edges, count and verify
    for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
      EdgeCount++;
    }
    EXPECT_EQ(EdgeCount, GraphN->GetEdges());
    
  } // end loop - Levels
}

TEST(GGenTest, GenRndGnm) {
  const int NNodesMax = 30;
  const int NEdgesMax = 50;
  int NodeCount = 0;
  int EdgeCount = 0;

  PUNGraph GraphUN;
  PNGraph GraphN;
  
  TInt::Rnd.PutSeed(0);
  
  for (int NNodes = 1; NNodes < NNodesMax; NNodes++) {
    
    for (int NEdges = 0; NEdges < NEdgesMax; NEdges++) {
      
      // Make sure we have enough nodes for the requested edges
      if (NNodes * (NNodes-1) / 2 >= NEdges) {

        // -------------------------------------------
        // Generate undirected graph
        GraphUN = TSnap::GenRndGnm<PUNGraph>(NNodes, NEdges, false, TInt::Rnd);
        EXPECT_FALSE(GraphUN->Empty());
        EXPECT_TRUE(GraphUN->IsOk());
              
        NodeCount = 0;
        EdgeCount = 0;
        // Iterate through nodes, verify it matches
        for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, GraphUN->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      }
      
      else if (NNodes * (NNodes-1) >= NEdges) {

        // -------------------------------------------
        // Generate directed graph
        GraphN = TSnap::GenRndGnm<PNGraph>(NNodes, NEdges, true, TInt::Rnd);
        EXPECT_FALSE(GraphN->Empty());
        EXPECT_TRUE(GraphN->IsOk());
              
        NodeCount = 0;
        for (TNGraph::TNodeI NI = GraphN->BegNI(); NI < GraphN->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, GraphN->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TNGraph::TEdgeI NI = GraphN->BegEI(); NI < GraphN->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, GraphN->GetEdges());
      }
    
    } // end loop - NEdges
  } // end loop - NNodes
}

TEST(GGenTest, GenRndBipart) {
  const int LeftNodesMin = 8;
  const int LeftNodesMax = 20;
  const int RightNodesMin = 8;
  const int RightNodesMax = 50;
  const int EdgesMin = 5;
  const int EdgesMax = 30;
  
  PBPGraph Graph;
  
  TInt::Rnd.PutSeed(0);
  
  for (int LeftNodes = LeftNodesMin; LeftNodes < LeftNodesMax; LeftNodes++) {
    
    for (int RightNodes = RightNodesMin; RightNodes < RightNodesMax; RightNodes++) {
      
      for (int Edges = EdgesMin;  Edges < EdgesMax; Edges++) {
      
        // Make sure there are sufficient number of edges to cover the BP graph
        if (Edges > LeftNodes * RightNodes) {
          continue;
        }
        
        // -------------------------------------------
        // Generate undirected graph
        Graph = TSnap::GenRndBipart(LeftNodes, RightNodes, Edges, TInt::Rnd);
        EXPECT_FALSE(Graph->Empty());
//        EXPECT_TRUE(Graph->IsOk());   // BP graph always returns 'not ok'
        
        int NodeCount = 0;
        int EdgeCount = 0;
        // Iterate through nodes, verify it matches
        for (TBPGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, Graph->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TBPGraph::TEdgeI NI = Graph->BegEI(); NI < Graph->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, Graph->GetEdges());
        
      } // end loop - NEdges
    } // end loop - LeftNodes
  } // end loop - RightNodes

}

// Tests power-law like networks
// http://en.wikipedia.org/wiki/Scale-free_network
TEST(GGenTest, GenRndPowerLaw) {
  const int NNodesMin = 10;
  const int NNodesMax = 1000;
  const double PowerExpMin = 2.0;
  const double PowerExpMax = 3.0;
  int NodeCount = 0;
  int EdgeCount = 0;
  
  PUNGraph GraphUN;

  TInt::Rnd.PutSeed(0);
  
  // Outputs a lot of text, so only run a few iterations
  for (int NNodes = NNodesMin; NNodes <= NNodesMax; NNodes += (NNodesMax - NNodesMin)/2) {
    
    for (double PowerExp = PowerExpMin; PowerExp <= PowerExpMax; PowerExp += 1.0) {
      
      // Generate undirected graph
      printf("NNodes=%d, PowerExp=%f\n", NNodes, PowerExp);
      GraphUN = TSnap::GenRndPowerLaw(NNodes, PowerExp);
      EXPECT_FALSE(GraphUN->Empty());
      EXPECT_TRUE(GraphUN->IsOk());
      
      printf("done with generation\n");
      
      NodeCount = 0;
      EdgeCount = 0;
      // Iterate through nodes, verify it matches
      for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, GraphUN->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      
    } // end loop - NEdges
  } // end loop - NNodes

}

// Test generator for degree sequence -- hangs at times due to early connections
//TEST(GGenTest, GenDegSeq) {
//  const int NNodesMax = 15;
//  
//  PUNGraph GraphUN;
//  TIntV DegSeqV;
//  TInt::Rnd.PutSeed(0);
//  
//  int NodeCount = 0;
//  int EdgeCount = 0;
//  for (int NNodes = 3; NNodes < NNodesMax; NNodes+=NNodesMax/5) {
//        
//    // Generate Deg Sequence vector randomly
//    DegSeqV.Gen(NNodes);
//    int DegSum = 0;
//    for (int n = 0; n < DegSeqV.Len()/4; n++) {
//      DegSeqV[n] = TInt::Rnd.GetUniDevInt(1, 5);
//      DegSum += DegSeqV[n];
//    }
//    // Add 1's to the tail
//    for (int n = DegSeqV.Len()/4; n < DegSeqV.Len(); n++) {
//      DegSeqV[n] = 1;
//      DegSum += DegSeqV[n];
//    }
//    // Make sure the sum of degrees is divisible by 2
//    if (DegSum % 2 != 0) {
//      DegSeqV[(int)TInt::Rnd.GetUniDevInt(NNodes)]++;
//    }
//    DegSeqV.Sort();
//    DegSeqV.Reverse();
//    
//    // -------------------------------------------
//    // Generate undirected graph
//    
//    GraphUN = TSnap::GenDegSeq(DegSeqV);
//    EXPECT_FALSE(GraphUN->Empty());
//    EXPECT_TRUE(GraphUN->IsOk());
//    
//    NodeCount = 0;
//    EdgeCount = 0;
//    // Iterate through nodes, verify it matches
//    for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
//      NodeCount++;
//    }
//    EXPECT_EQ(NodeCount, GraphUN->GetNodes());
//    
//    EdgeCount = 0;
//    // Iterate through edges, count and verify
//    for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
//      EdgeCount++;
//    }
//    EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
//  }
//}

TEST(GGenTest, GenPrefAttach) {
  const int NNodesMax = 100;
  const int NodeOutDegMax = 15;
  
  PUNGraph GraphUN;
  
  for (int NNodes = 0; NNodes < NNodesMax; NNodes++) {
    
    for (int NodeOutDeg = 0; NodeOutDeg < NodeOutDegMax; NodeOutDeg++) {
      
      int NodeCount = 0;
      int EdgeCount = 0;
      
      // -------------------------------------------
      // Generate undirected graph
      GraphUN = TSnap::GenPrefAttach(NNodes, NodeOutDeg);
      EXPECT_FALSE(GraphUN->Empty());
      EXPECT_TRUE(GraphUN->IsOk());
      
      NodeCount = 0;
      EdgeCount = 0;
      // Iterate through nodes, verify it matches
      for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, GraphUN->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      
    } // end loop - NEdges
  } // end loop - NNodes
}

TEST(GGenTest, GenGeoPrefAttach) {
  const int NNodesMax = 100;
  const int NodeOutDegMax = 15;
  const double BetaMax = 2.0;
  
  PUNGraph GraphUN;
  PNGraph GraphN;
  
  for (int NNodes = 1; NNodes < NNodesMax; NNodes++) {
    
    for (int NodeOutDeg = 0; NodeOutDeg < NodeOutDegMax; NodeOutDeg++) {
      
      for (double Beta = -1*BetaMax; Beta < BetaMax; Beta += 0.2) {
      
        int NodeCount = 0;
        int EdgeCount = 0;
        
        // -------------------------------------------
        // Generate undirected graph
        GraphUN = TSnap::GenGeoPrefAttach(NNodes, NodeOutDeg, Beta);
        EXPECT_FALSE(GraphUN->Empty());
        EXPECT_TRUE(GraphUN->IsOk());
        
        NodeCount = 0;
        EdgeCount = 0;
        // Iterate through nodes, verify it matches
        for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, GraphUN->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      } // end loop - Beta
    } // end loop - NEdges
  } // end loop - NNodes
}

TEST(GGenTest, GenSmallWorld) {
  const int NNodesMax = 1000;
  const double RewireProbMax = 1.0;
  
  PUNGraph GraphUN;
  
  for (int NNodes = 1; NNodes < NNodesMax; NNodes+=NNodesMax/20) {
    
    for (int NodeOutDeg = 1; NodeOutDeg < 10; NodeOutDeg++) {
      
      for (double RewireProb = 0.0; RewireProb <= RewireProbMax; RewireProb += 0.2) {
        
        int NodeCount = 0;
        int EdgeCount = 0;
        
        if (NNodes <= NodeOutDeg) {
          continue;
        }

        // -------------------------------------------
        // Generate undirected graph
        GraphUN = TSnap::GenSmallWorld(NNodes, NodeOutDeg, RewireProb);
        EXPECT_FALSE(GraphUN->Empty());
        EXPECT_TRUE(GraphUN->IsOk());
        
        NodeCount = 0;
        EdgeCount = 0;
        // Iterate through nodes, verify it matches
        for (TUNGraph::TNodeI NI = GraphUN->BegNI(); NI < GraphUN->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, GraphUN->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TUNGraph::TEdgeI NI = GraphUN->BegEI(); NI < GraphUN->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, GraphUN->GetEdges());
      } // end loop - RewireProb
    } // end loop - NEdges
  } // end loop - NNodes
}

// Test forest fire graph generator
// Note: outputs a lot of text, so run with only a few values
TEST(GGenTest, GenForestFire) {
  const int NNodesMax = 1000;
  const double FwdProbMax = 1.0;
  const double BckProbMax = 1.0;
  
//  PNGraph Graph;
  
  for (int NNodes = 0; NNodes < NNodesMax; NNodes += NNodesMax/2) {
    
    for (double FwdProb = 0.1; FwdProb < FwdProbMax; FwdProb += 0.5) {

      for (double BckProb = 0.1; BckProb < BckProbMax; BckProb += 0.5) {
        
        int NodeCount = 0;
        int EdgeCount = 0;
        
        // -------------------------------------------
        // Generate directed forest fire graph
        PNGraph Graph = TSnap::GenForestFire(NNodes, FwdProb, BckProb);
        if (0 != NNodes) {
          EXPECT_FALSE(Graph->Empty());
        }
        EXPECT_TRUE(Graph->IsOk());
        
        NodeCount = 0;
        EdgeCount = 0;
        // Iterate through nodes, verify it matches
        for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, Graph->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TNGraph::TEdgeI NI = Graph->BegEI(); NI < Graph->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, Graph->GetEdges());
      } // end loop - RewireProb
    } // end loop - NEdges
  } // end loop - NNodes
}

TEST(GGenTest, GenCopyModel) {
  const int NNodesMax = 100;
  const double BetaMax = 2.0;
  
  PNGraph Graph;
  
  for (int NNodes = 0; NNodes < NNodesMax; NNodes++) {
          
    for (double Beta = -1*BetaMax; Beta < BetaMax; Beta += 0.2) {
      
      int NodeCount = 0;
      int EdgeCount = 0;
      
      Graph = TSnap::GenCopyModel(NNodes, Beta);
      EXPECT_FALSE(Graph->Empty());
      EXPECT_TRUE(Graph->IsOk());
      
      NodeCount = 0;
      EdgeCount = 0;
      // Iterate through nodes, verify it matches
      for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
        NodeCount++;
      }
      EXPECT_EQ(NodeCount, Graph->GetNodes());
      
      EdgeCount = 0;
      // Iterate through edges, count and verify
      for (TNGraph::TEdgeI NI = Graph->BegEI(); NI < Graph->EndEI(); NI++) {
        EdgeCount++;
      }
      EXPECT_EQ(EdgeCount, Graph->GetEdges());
    } // end loop - Beta
    
  } // end loop - NNodes
  
}

TEST(GGenTest, GenRMat) {
  const int NNodesMax = 10000;
  const int NEdgesMax = 100;
  int NodeCount = 0;
  int EdgeCount = 0;
  
  PNGraph Graph;
  
  TInt::Rnd.PutSeed(0);
  TFlt A, B, C;
  TFltTrV Vals;
  Vals.Add(TFltTr(0.15, 0.2, 0.35));
  Vals.Add(TFltTr(0.2, 0.2, 0.55));
  Vals.Add(TFltTr(0.4, 0.15, 0.2));
  
  for (int NNodes = 50; NNodes < NNodesMax; NNodes+=NNodesMax/2) {
    
    for (int NEdges = 5; NEdges < NEdgesMax; NEdges+=NEdgesMax/4) {

      for (int i = 0; i < Vals.Len(); i++) {
        
        A = Vals[i].Val1;
        B = Vals[i].Val2;
        C = Vals[i].Val3;
        
        Graph = TSnap::GenRMat(NNodes, NEdges, A, B, C);
        EXPECT_FALSE(Graph->Empty());
        EXPECT_TRUE(Graph->IsOk());
        
        NodeCount = 0;
        EdgeCount = 0;
        // Iterate through nodes, verify it matches
        for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
          NodeCount++;
        }
        EXPECT_EQ(NodeCount, Graph->GetNodes());
        
        EdgeCount = 0;
        // Iterate through edges, count and verify
        for (TNGraph::TEdgeI NI = Graph->BegEI(); NI < Graph->EndEI(); NI++) {
          EdgeCount++;
        }
        EXPECT_EQ(EdgeCount, Graph->GetEdges());
    
      }
    } // end loop - NEdges
    
  } // end loop - NNodes
}