#include <gtest/gtest.h>

#include <iostream>
#include "Snap.h"

#define EPSILON 0.0001
#define DIRNAME "cncom"

class CnComTest { };  // For gtest highlighting

using namespace TSnap;

template <class PGraph>
void TestFull() {
  
  const int NNodes = 500;
  
  PGraph G = GenFull<PGraph>(NNodes);
    
  // Tests whether the Graph is (weakly) connected
  // Note: these are equivalent functions
  EXPECT_TRUE(IsConnected(G));
  EXPECT_TRUE(IsWeaklyConn(G));
  
  // Get all nodes in same component
  TIntV CnCom;
  GetNodeWcc(G, 1, CnCom);
  EXPECT_TRUE(CnCom.Len() == G->GetNodes());
  
  // Get weakly connected component counts
  TIntPrV WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  EXPECT_TRUE(WccSzCnt[0] == TIntPr(NNodes, 1));
  EXPECT_TRUE(WccSzCnt.Len() == 1);
  
  // Get weakly connected components
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  EXPECT_TRUE(WCnComV[0].Len() == G->GetNodes());
  EXPECT_TRUE(WCnComV.Len() == 1);
  
  double MxWccSz = GetMxWccSz(G);
  EXPECT_TRUE(MxWccSz == 1.0);
  
  // Get graph with largest SCC
  PGraph GMx = GetMxWcc(G);
  EXPECT_TRUE(GMx->GetNodes() == G->GetNodes());
  EXPECT_TRUE(GMx->GetEdges() == G->GetEdges());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  EXPECT_TRUE(SCnComV[0].Len() == G->GetNodes());
  EXPECT_TRUE(SCnComV.Len() == 1);
  
  // Get largest bi-connected component
  PGraph GMxBi = GetMxBiCon(G);
  EXPECT_TRUE(GMxBi->GetNodes() == G->GetNodes());
  EXPECT_TRUE(GMxBi->GetEdges() == G->GetEdges());
  
}

// Test connected components on full graph
TEST(CnComTest, UndirectedDisconnected) {
  
  PUNGraph G;

  // As benchmark, this is done once and compared visually to confirm values are correct
//  const int NNodes = 30;
//  G = GenRndPowerLaw(NNodes, 2.5);
//  SaveEdgeList(G, "sample_cncom_power.txt");
  
  G = LoadEdgeList<PUNGraph>(TStr::Fmt("%s/sample_cncom_unpower.txt", DIRNAME));
  TIntStrH NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlNeato, TStr::Fmt("%s/sample_cncom_unpower.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);

  EXPECT_FALSE(IsConnected(G));
  EXPECT_FALSE(IsWeaklyConn(G));
//  printf("IsConnected(G) = %d\n", IsConnected(G));
//  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));

  // Get weakly connected component counts
  TIntPrV WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
//  for (int i = 0; i < WccSzCnt.Len(); i++) {
//    printf("WccSzCnt[%d] = (%d, %d)\n", i, WccSzCnt[i].Val1.Val, WccSzCnt[i].Val2.Val);
//  }
  EXPECT_TRUE(WccSzCnt[0] == TIntPr(2, 2));
  EXPECT_TRUE(WccSzCnt[1] == TIntPr(26,1));

  // Get all nodes in same component
  TIntV CnCom;
  GetNodeWcc(G, 1, CnCom);
//  printf("CnCom.Len() = %d\n", CnCom.Len());
  EXPECT_TRUE(CnCom.Len() == 26);

  TCnComV WCnComV;
  GetWccs(G, WCnComV);
//  for (int i = 0; i < WCnComV.Len(); i++) {
//    printf("WCnComV[%d].Len() = %d\n", i, WCnComV[i].Len());
//    for (int j = 0; j < WCnComV[i].Len(); j++) {
//      printf("WCnComV[%d][%d] = %d\n", i, j, WCnComV[i][j].Val);
//    }
//  }
  EXPECT_TRUE(WCnComV[0].Len() == 26);
  EXPECT_TRUE(WCnComV[1].Len() == 2);
  EXPECT_TRUE(WCnComV[2].Len() == 2);

  double MxWccSz = GetMxWccSz(G);
//  printf("MxWccSz = %.5f\n", MxWccSz);
  EXPECT_TRUE(MxWccSz > 0.86667 - EPSILON && MxWccSz < 0.86667 + EPSILON);

  // Get graph with largest SCC
  PUNGraph GMx = GetMxWcc(G);
//  printf("GMx->GetNodes() = %d, GMx->GetEdges() = %d\n", GMx->GetNodes(), GMx->GetEdges());
  EXPECT_TRUE(GMx->GetNodes() == 26);
  EXPECT_TRUE(GMx->GetEdges() == 27);

  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
//  for (int i = 0; i < SCnComV.Len(); i++) {
//    printf("SCnComV[%d].Len() = %d\n", i, SCnComV[i].Len());
//  }
  EXPECT_TRUE(SCnComV[0].Len() == 26);
  EXPECT_TRUE(SCnComV[1].Len() == 2);
  EXPECT_TRUE(SCnComV[2].Len() == 2);

  // Get largest bi-connected component
  PUNGraph GMxBi = GetMxBiCon(G);
//  printf("GMxBi->GetNodes() = %d, GMxBi->GetEdges() = %d\n", GMxBi->GetNodes(), GMxBi->GetEdges());
  EXPECT_TRUE(GMxBi->GetNodes() == 6);
  EXPECT_TRUE(GMxBi->GetEdges() == 6);

  // Get bi-connected size components counts
  TIntPrV SzCntV;
  GetBiConSzCnt(G, SzCntV);
//  for (int i = 0; i < SzCntV.Len(); i++) {
//    printf("SzCntV[%d] = (%d, %d)\n", i, SzCntV[i].Val1.Val, SzCntV[i].Val2.Val);
//  }
  EXPECT_TRUE(SzCntV[0] == TIntPr(2, 18));
  EXPECT_TRUE(SzCntV[1] == TIntPr(5, 1));
  EXPECT_TRUE(SzCntV[2] == TIntPr(6, 1));

  // Get bi-connected components
  TCnComV BiCnComV;
  GetBiCon(G, BiCnComV);
  for (int i = 0; i < BiCnComV.Len(); i++) {
//    printf("BiCnComV[%d].Len() = %d\n", i, BiCnComV[i].Len());
    if (i != 6 && i != 13) {
      EXPECT_TRUE(BiCnComV[i].Len() == 2);
    }
  }
  EXPECT_TRUE(BiCnComV[6].Len() == 6);
  EXPECT_TRUE(BiCnComV[13].Len() == 5);
  
  // Get articulation points of a graph
  TIntV ArtNIdV;
  GetArtPoints(G, ArtNIdV);
//  printf("ArtNIdV.Len() = %d\n", ArtNIdV.Len());
//  for (int i = 0; i < ArtNIdV.Len(); i++) {
//    printf("ArtNIdV[%d] = %d\n", i, ArtNIdV[i].Val);
//  }
  EXPECT_TRUE(ArtNIdV.Len() == 12);
  EXPECT_TRUE(ArtNIdV[0] == 24);
  EXPECT_TRUE(ArtNIdV[1] == 21);
  EXPECT_TRUE(ArtNIdV[2] == 8);
  EXPECT_TRUE(ArtNIdV[3] == 9);
  EXPECT_TRUE(ArtNIdV[4] == 23);
  EXPECT_TRUE(ArtNIdV[5] == 2);

  // Get edge bridges of a graph
  TIntPrV EdgeV;
  GetEdgeBridges(G, EdgeV);
//  printf("EdgeV.Len() = %d\n", EdgeV.Len());
//  for (int i = 0; i < EdgeV.Len(); i++) {
//    printf("EdgeV[%d] = (%d, %d)\n", i, EdgeV[i].Val1.Val, EdgeV[i].Val2.Val);
//  }
  EXPECT_TRUE(EdgeV.Len() == 18);
  
  EXPECT_TRUE(EdgeV[0] == TIntPr(6, 24));
  EXPECT_TRUE(EdgeV[1] == TIntPr(14, 24));
  EXPECT_TRUE(EdgeV[2] == TIntPr(21, 24));
  EXPECT_TRUE(EdgeV[3] == TIntPr(8, 21));
  EXPECT_TRUE(EdgeV[4] == TIntPr(8, 9));
  EXPECT_TRUE(EdgeV[5] == TIntPr(1, 23));

//  EXPECT_TRUE(EdgeV.Len() == 0);

  // Get 1-components counts
  TIntPrV SzCnt1ComV;
  Get1CnComSzCnt(G, SzCnt1ComV);
//  printf("SzCnt1ComV.Len() = %d\n", SzCnt1ComV.Len());
//  for (int i = 0; i < SzCnt1ComV.Len(); i++) {
//    printf("SzCnt1ComV[%d] = (%d, %d)\n", i, SzCnt1ComV[i].Val1.Val, SzCnt1ComV[i].Val2.Val);
//  }

  EXPECT_TRUE(SzCnt1ComV.Len() == 2);
  EXPECT_TRUE(SzCnt1ComV[0] == TIntPr(2, 2));
  
  EXPECT_TRUE(SzCnt1ComV[1] == TIntPr(20, 1));

  // Get 1-components
  TCnComV Cn1ComV;
  Get1CnCom(G, Cn1ComV);
  EXPECT_TRUE(Cn1ComV.Len() == 3);
  EXPECT_TRUE(Cn1ComV[0].Len() == 20);
  EXPECT_TRUE(Cn1ComV[1].Len() == 2);
  EXPECT_TRUE(Cn1ComV[2].Len() == 2);

}

TEST(CnComTest, DirectedDisconnected) {
  
  PNGraph G;
  
  // Create benchmark graph, initially visually to confirm values are correct
//  const int NNodes = 30;
//  G = GenRndGnm<PNGraph>(NNodes, NNodes);
//  
//  // Add some more random edges
//  for (int i = 0; i < 10; i++) {
//    TInt Src, Dst;
//    do {
//      Src = G->GetRndNId();
//      Dst = G->GetRndNId();
//    }
//    while (Src == Dst || G->IsEdge(Src, Dst));
//    G->AddEdge(Src, Dst);
//  }
//  
//  // Add isolated component
//  G->AddNode(NNodes);
//  G->AddNode(NNodes+1);
//  G->AddNode(NNodes+2);
//  G->AddEdge(NNodes, NNodes+1);
//  G->AddEdge(NNodes+1, NNodes+2);
//  G->AddEdge(NNodes+2, NNodes+1);
//  
//  SaveEdgeList(G, "sample_cncom_ngraph.txt");
  
  G = LoadEdgeList<PNGraph>(TStr::Fmt("%s/sample_cncom_ngraph.txt", DIRNAME));
  TIntStrH NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlDot, TStr::Fmt("%s/sample_cncom_ngraph.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
  
  EXPECT_FALSE(IsConnected(G));
  EXPECT_FALSE(IsWeaklyConn(G));
//  printf("IsConnected(G) = %d\n", IsConnected(G));
//  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get weakly connected component counts
  TIntPrV WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
//  for (int i = 0; i < WccSzCnt.Len(); i++) {
//    printf("WccSzCnt[%d] = (%d, %d)\n", i, WccSzCnt[i].Val1.Val, WccSzCnt[i].Val2.Val);
//  }
  EXPECT_TRUE(WccSzCnt[0] == TIntPr(3, 1));
  EXPECT_TRUE(WccSzCnt[1] == TIntPr(30,1));
  
  // Get all nodes in same component
  TIntV CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %d\n", CnCom.Len());
  EXPECT_TRUE(CnCom.Len() == 30);
  
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %d\n", i, WCnComV[i].Len());
    for (int j = 0; j < WCnComV[i].Len(); j++) {
      printf("WCnComV[%d][%d] = %d\n", i, j, WCnComV[i][j].Val);
    }
  }
  EXPECT_TRUE(WCnComV[0].Len() == 30);
  EXPECT_TRUE(WCnComV[1].Len() == 3);
  
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  EXPECT_TRUE(MxWccSz > 0.90909 - EPSILON && MxWccSz < 0.90909 + EPSILON);
  
  // Get graph with largest SCC
  PNGraph GMx = GetMxWcc(G);
  printf("GMx->GetNodes() = %d, GMx->GetEdges() = %d\n", GMx->GetNodes(), GMx->GetEdges());
  EXPECT_TRUE(GMx->GetNodes() == 30);
  EXPECT_TRUE(GMx->GetEdges() == 40);
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %d\n", i, SCnComV[i].Len());
    if (i != 3 && i != 27) {
      EXPECT_TRUE(SCnComV[i].Len() == 1);
    }
  }
  EXPECT_TRUE(SCnComV[3].Len() == 4);
  EXPECT_TRUE(SCnComV[27].Len() == 2);
  
  // Get largest bi-connected component
  PNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %d, GMxBi->GetEdges() = %d\n", GMxBi->GetNodes(), GMxBi->GetEdges());
  EXPECT_TRUE(GMxBi->GetNodes() == 25);
  EXPECT_TRUE(GMxBi->GetEdges() == 35);
  
}

// Test connected components on full graph
TEST(CnComTest, CompleteGraph) {
  
  TestFull<PUNGraph>();
  
  TestFull<PNGraph>();
  
  TestFull<PNEGraph>();

  // Test the remaining functions (for undirected graphs only)
  const int NNodes = 500;
  
  PUNGraph G = GenFull<PUNGraph>(NNodes);

  // Get bi-connected size components counts
  TIntPrV SzCntV;
  GetBiConSzCnt(G, SzCntV);
  EXPECT_TRUE(SzCntV[0] == TIntPr(NNodes, 1));
  EXPECT_TRUE(SzCntV.Len() == 1);
  
  // Get bi-connected components
  TCnComV BiCnComV;
  GetBiCon(G, BiCnComV);
  EXPECT_TRUE(BiCnComV[0].Len() == G->GetNodes());
  EXPECT_TRUE(BiCnComV.Len() == 1);
  
  // Get articulation points of a graph
  TIntV ArtNIdV;
  GetArtPoints(G, ArtNIdV);
  EXPECT_TRUE(ArtNIdV.Len() == 0);
  
  // Get edge bridges of a graph
  TIntPrV EdgeV;
  GetEdgeBridges(G, EdgeV);
  EXPECT_TRUE(EdgeV.Len() == 0);
  
  // Get 1-components counts
  TIntPrV SzCnt1ComV;
  Get1CnComSzCnt(G, SzCnt1ComV);
  EXPECT_TRUE(SzCnt1ComV.Len() == 0);
  
  // Get 1-components
  TCnComV Cn1ComV;
  Get1CnCom(G, Cn1ComV);
  EXPECT_TRUE(Cn1ComV.Len() == 0);

  
}
