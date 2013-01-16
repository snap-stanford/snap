#include <gtest/gtest.h>

#include <iostream>
#include "Snap.h"

#define EPSILON 0.0001
#define DIRNAME "bfsdfs"

class BfsDfsTest { };  // For gtest highlighting

using namespace TSnap;

// Get connected components on full graph
template <class PGraph>
void TestFull() {
  
  const int NNodes = 500;
  
  PGraph G = GenFull<PGraph>(NNodes);
  PNGraph GOut;
  int TreeSz, TreeDepth;
  
  GOut = GetBfsTree(G, 1, false, false);
  GetSubTreeSz(G, 1, false, false, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == 1);
  EXPECT_TRUE(GOut->GetEdges() == 0);
  EXPECT_TRUE(TreeSz == 1);
  EXPECT_TRUE(TreeDepth == 0);
  
  GOut = GetBfsTree(G, NNodes-1, true, true);
  GetSubTreeSz(G, 1, true, true, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);

  GOut = GetBfsTree(G, NNodes/2, true, false);
  GetSubTreeSz(G, 1, true, false, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  
  GOut = GetBfsTree(G, 1, false, true);
  GetSubTreeSz(G, 1, false, true, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  
  TIntV NIdV;
  int StartNId, Hop, Nodes;

  StartNId = 1;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, HasGraphFlag(typename PGraph::TObj, gfDirected));
  printf("Nodes = %d, GetNodesAtHop NIdV.Len() = %d\n", Nodes, NIdV.Len());
  
  TIntPrV HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, HasGraphFlag(typename PGraph::TObj, gfDirected));
  printf("Nodes = %d, GetNodesAtHops HopCntV.Len() = %d\n", Nodes, HopCntV.Len());

  int Length, SrcNId, DstNId;
  SrcNId = 1;
  DstNId = NNodes-1;
  
  Length = GetShortPath(G, SrcNId, DstNId, HasGraphFlag(typename PGraph::TObj, gfDirected));
  printf("SPL Length = %d\n", Length);
  
  TIntH NIdToDistH;
  int MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, HasGraphFlag(typename PGraph::TObj, gfDirected), MaxDist);
  for (int i = 0; i < min(5,NIdToDistH.Len()); i++) {
    printf("NIdToDistH[%d] = %d\n", i, NIdToDistH[i].Val);
  }
  
  
  int FullDiam;
  double EffDiam, AvgDiam;
  int NTestNodes = 10;
  
  for (int IsDir = 0; IsDir < 2; IsDir++) {
    printf("IsDir = %d:\n", IsDir);
    
    FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
    printf("FullDiam = %d\n", FullDiam);
    
    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir);
    printf("EffDiam = %.3f\n", EffDiam);

    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir, EffDiam, FullDiam);
    printf("EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
    
    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir, EffDiam, FullDiam, AvgDiam);
    printf("EffDiam = %.3f, FullDiam = %d, AvgDiam = %.3f\n", EffDiam, FullDiam, AvgDiam);
    
    TIntV SubGraphNIdV;
    for (int i = 0; i < NTestNodes; i++) {
      SubGraphNIdV.Add(G->GetRndNId());
    }
    for (int i = 0; i < SubGraphNIdV.Len(); i++) {
      printf("SubGraphNIdV[%d] = %d\n", i, SubGraphNIdV[i].Val);
    }

    EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
    printf("For subgraph: EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
    
  }

//  // Tests whether the Graph is (weakly) connected
//  // Note: these are equivalent functions
//  EXPECT_TRUE(IsConnected(G));
//  EXPECT_TRUE(IsWeaklyConn(G));
//  
//  // Get all nodes in same component
//  TIntV CnCom;
//  GetNodeWcc(G, 1, CnCom);
//  EXPECT_TRUE(CnCom.Len() == G->GetNodes());
//  
//  // Get weakly connected component counts
//  TIntPrV WccSzCnt;
//  GetWccSzCnt(G, WccSzCnt);
//  EXPECT_TRUE(WccSzCnt[0] == TIntPr(NNodes, 1));
//  EXPECT_TRUE(WccSzCnt.Len() == 1);
//  
//  // Get weakly connected components
//  TCnComV WCnComV;
//  GetWccs(G, WCnComV);
//  EXPECT_TRUE(WCnComV[0].Len() == G->GetNodes());
//  EXPECT_TRUE(WCnComV.Len() == 1);
//  
//  double MxWccSz = GetMxWccSz(G);
//  EXPECT_TRUE(MxWccSz == 1.0);
//  
//  // Get graph with largest SCC
//  PGraph GMx = GetMxWcc(G);
//  EXPECT_TRUE(GMx->GetNodes() == G->GetNodes());
//  EXPECT_TRUE(GMx->GetEdges() == G->GetEdges());
//  
//  // Get strongly connected components
//  TCnComV SCnComV;
//  GetSccs(G, SCnComV);
//  EXPECT_TRUE(SCnComV[0].Len() == G->GetNodes());
//  EXPECT_TRUE(SCnComV.Len() == 1);
//  
//  // Get largest bi-connected component
//  PGraph GMxBi = GetMxBiCon(G);
//  EXPECT_TRUE(GMxBi->GetNodes() == G->GetNodes());
//  EXPECT_TRUE(GMxBi->GetEdges() == G->GetEdges());
  
}

//// Get connected components on undirected graph that is not fully connected
//TEST(BfsDfsTest, UndirectedDisconnected) {
//  
//  PUNGraph G;
//  
//  // As benchmark, this is done once and compared visually to confirm values are correct
//  //  const int NNodes = 30;
//  //  G = GenRndPowerLaw(NNodes, 2.5);
//  //  SaveEdgeList(G, "sample_cncom_power.txt");
//  
//  G = LoadEdgeList<PUNGraph>(TStr::Fmt("%s/sample_cncom_unpower.txt", DIRNAME));
//  TIntStrH NodeLabelH;
//  for (int i = 0; i < G->GetNodes(); i++) {
//    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
//  }
//  DrawGViz(G, gvlNeato, TStr::Fmt("%s/sample_cncom_unpower.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
//  
//  EXPECT_FALSE(IsConnected(G));
//  EXPECT_FALSE(IsWeaklyConn(G));
//  
//  // Get weakly connected component counts
//  TIntPrV WccSzCnt;
//  GetWccSzCnt(G, WccSzCnt);
//  EXPECT_TRUE(WccSzCnt[0] == TIntPr(2, 2));
//  EXPECT_TRUE(WccSzCnt[1] == TIntPr(26,1));
//  
//  // Get all nodes in same component
//  TIntV CnCom;
//  GetNodeWcc(G, 1, CnCom);
//  EXPECT_TRUE(CnCom.Len() == 26);
//  
//  TCnComV WCnComV;
//  GetWccs(G, WCnComV);
//  EXPECT_TRUE(WCnComV[0].Len() == 26);
//  EXPECT_TRUE(WCnComV[1].Len() == 2);
//  EXPECT_TRUE(WCnComV[2].Len() == 2);
//  
//  double MxWccSz = GetMxWccSz(G);
//  EXPECT_TRUE(MxWccSz > 0.86667 - EPSILON && MxWccSz < 0.86667 + EPSILON);
//  
//  // Get graph with largest SCC
//  PUNGraph GMx = GetMxWcc(G);
//  EXPECT_TRUE(GMx->GetNodes() == 26);
//  EXPECT_TRUE(GMx->GetEdges() == 27);
//  
//  // Get strongly connected components
//  TCnComV SCnComV;
//  GetSccs(G, SCnComV);
//  EXPECT_TRUE(SCnComV[0].Len() == 26);
//  EXPECT_TRUE(SCnComV[1].Len() == 2);
//  EXPECT_TRUE(SCnComV[2].Len() == 2);
//  
//  // Get largest bi-connected component
//  PUNGraph GMxBi = GetMxBiCon(G);
//  EXPECT_TRUE(GMxBi->GetNodes() == 6);
//  EXPECT_TRUE(GMxBi->GetEdges() == 6);
//  
//  // Get bi-connected size components counts
//  TIntPrV SzCntV;
//  GetBiConSzCnt(G, SzCntV);
//  EXPECT_TRUE(SzCntV[0] == TIntPr(2, 18));
//  EXPECT_TRUE(SzCntV[1] == TIntPr(5, 1));
//  EXPECT_TRUE(SzCntV[2] == TIntPr(6, 1));
//  
//  // Get bi-connected components
//  TCnComV BiCnComV;
//  GetBiCon(G, BiCnComV);
//  for (int i = 0; i < BiCnComV.Len(); i++) {
//    if (i != 6 && i != 13) {
//      EXPECT_TRUE(BiCnComV[i].Len() == 2);
//    }
//  }
//  EXPECT_TRUE(BiCnComV[6].Len() == 6);
//  EXPECT_TRUE(BiCnComV[13].Len() == 5);
//  
//  // Get articulation points of a graph
//  TIntV ArtNIdV;
//  GetArtPoints(G, ArtNIdV);
//  EXPECT_TRUE(ArtNIdV.Len() == 12);
//  EXPECT_TRUE(ArtNIdV[0] == 24);
//  EXPECT_TRUE(ArtNIdV[1] == 21);
//  EXPECT_TRUE(ArtNIdV[2] == 8);
//  EXPECT_TRUE(ArtNIdV[3] == 9);
//  EXPECT_TRUE(ArtNIdV[4] == 23);
//  EXPECT_TRUE(ArtNIdV[5] == 2);
//  
//  // Get edge bridges of a graph
//  TIntPrV EdgeV;
//  GetEdgeBridges(G, EdgeV);
//  EXPECT_TRUE(EdgeV.Len() == 18);
//  
//  EXPECT_TRUE(EdgeV[0] == TIntPr(6, 24));
//  EXPECT_TRUE(EdgeV[1] == TIntPr(14, 24));
//  EXPECT_TRUE(EdgeV[2] == TIntPr(21, 24));
//  EXPECT_TRUE(EdgeV[3] == TIntPr(8, 21));
//  EXPECT_TRUE(EdgeV[4] == TIntPr(8, 9));
//  EXPECT_TRUE(EdgeV[5] == TIntPr(1, 23));
//  
//  // Get 1-components counts
//  TIntPrV SzCnt1ComV;
//  Get1CnComSzCnt(G, SzCnt1ComV);
//  EXPECT_TRUE(SzCnt1ComV.Len() == 2);
//  EXPECT_TRUE(SzCnt1ComV[0] == TIntPr(2, 2));
//  EXPECT_TRUE(SzCnt1ComV[1] == TIntPr(20, 1));
//  
//  // Get 1-components
//  TCnComV Cn1ComV;
//  Get1CnCom(G, Cn1ComV);
//  EXPECT_TRUE(Cn1ComV.Len() == 3);
//  EXPECT_TRUE(Cn1ComV[0].Len() == 20);
//  EXPECT_TRUE(Cn1ComV[1].Len() == 2);
//  EXPECT_TRUE(Cn1ComV[2].Len() == 2);
//  
//}
//
//// Get connected components on directed graph that is not fully connected
//TEST(BfsDfsTest, DirectedDisconnected) {
//  
//  PNGraph G;
//  
//  // Create benchmark graph, initially visually to confirm values are correct
//  //  const int NNodes = 30;
//  //  G = GenRndGnm<PNGraph>(NNodes, NNodes);
//  //  // Add some more random edges
//  //  for (int i = 0; i < 10; i++) {
//  //    TInt Src, Dst;
//  //    do {
//  //      Src = G->GetRndNId();
//  //      Dst = G->GetRndNId();
//  //    }
//  //    while (Src == Dst || G->IsEdge(Src, Dst));
//  //    G->AddEdge(Src, Dst);
//  //  }
//  //
//  //  // Add isolated component
//  //  G->AddNode(NNodes);
//  //  G->AddNode(NNodes+1);
//  //  G->AddNode(NNodes+2);
//  //  G->AddEdge(NNodes, NNodes+1);
//  //  G->AddEdge(NNodes+1, NNodes+2);
//  //  G->AddEdge(NNodes+2, NNodes+1);//
//  //  SaveEdgeList(G, "sample_cncom_ngraph.txt");
//  
//  G = LoadEdgeList<PNGraph>(TStr::Fmt("%s/sample_cncom_ngraph.txt", DIRNAME));
//  TIntStrH NodeLabelH;
//  for (int i = 0; i < G->GetNodes(); i++) {
//    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
//  }
//  DrawGViz(G, gvlDot, TStr::Fmt("%s/sample_cncom_ngraph.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
//  
//  EXPECT_FALSE(IsConnected(G));
//  EXPECT_FALSE(IsWeaklyConn(G));
//  
//  // Get weakly connected component counts
//  TIntPrV WccSzCnt;
//  GetWccSzCnt(G, WccSzCnt);
//  EXPECT_TRUE(WccSzCnt[0] == TIntPr(3, 1));
//  EXPECT_TRUE(WccSzCnt[1] == TIntPr(30,1));
//  
//  // Get all nodes in same component
//  TIntV CnCom;
//  GetNodeWcc(G, 1, CnCom);
//  printf("CnCom.Len() = %d\n", CnCom.Len());
//  EXPECT_TRUE(CnCom.Len() == 30);
//  
//  TCnComV WCnComV;
//  GetWccs(G, WCnComV);
//  EXPECT_TRUE(WCnComV[0].Len() == 30);
//  EXPECT_TRUE(WCnComV[1].Len() == 3);
//  
//  double MxWccSz = GetMxWccSz(G);
//  EXPECT_TRUE(MxWccSz > 0.90909 - EPSILON && MxWccSz < 0.90909 + EPSILON);
//  
//  // Get graph with largest SCC
//  PNGraph GMx = GetMxWcc(G);
//  EXPECT_TRUE(GMx->GetNodes() == 30);
//  EXPECT_TRUE(GMx->GetEdges() == 40);
//  
//  // Get strongly connected components
//  TCnComV SCnComV;
//  GetSccs(G, SCnComV);
//  EXPECT_TRUE(SCnComV[3].Len() == 4);
//  EXPECT_TRUE(SCnComV[27].Len() == 2);
//  
//  // Get largest bi-connected component
//  PNGraph GMxBi = GetMxBiCon(G);
//  EXPECT_TRUE(GMxBi->GetNodes() == 25);
//  EXPECT_TRUE(GMxBi->GetEdges() == 35);
//  
//}

// Test connected components on full graphs of each type
TEST(BfsDfsTest, CompleteGraph) {
  
  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);
  
  printf("\nTesting undirected complete graph:\n");
  TestFull<PUNGraph>();

  printf("\nTesting directed complete graph:\n");
  TestFull<PNGraph>();

  printf("\nTesting undirected complete graph:\n");
  TestFull<PNEGraph>();
  
}
