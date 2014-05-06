#include "Snap.h"

#define DIRNAME "cncom"

class CnComTest { };  // For gtest highlighting

using namespace TSnap;

// Get connected components on full graph
template <class PGraph>
void GetCnComFull() {
  
  printf("\nFull Graph:\n");
  
  const int NNodes = 500;
  
  PGraph G = GenFull<PGraph>(NNodes);
  
  // Tests whether the Graph is (weakly) connected
  // Note: these are equivalent functions
  printf("IsConnected(G) = %d\n", IsConnected(G));
  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get all nodes in same component
  TIntV CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %d\n", CnCom.Len());
  
  // Get weakly connected component counts
  TIntPrV WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%d, %d)\n", i, WccSzCnt[i].Val1.Val, WccSzCnt[i].Val2.Val);
  }
  
  // Get weakly connected components
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %d\n", i, WCnComV[i].Len());
  }
  
  // Get strongly connected fraction -- should be 1.0
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest SCC
  PGraph GMx = GetMxWcc(G);
  printf("GMx->GetNodes() = %d, GMx->GetEdges() = %d\n", GMx->GetNodes(), GMx->GetEdges());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %d\n", i, SCnComV[i].Len());
  }
  
  // Get largest bi-connected component
  PGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %d, GMxBi->GetEdges() = %d\n", GMxBi->GetNodes(), GMxBi->GetEdges());
  
}

// Get connected components on undirected graph that is not fully connected
void GetCnComUndirectedDisconnected() {
  
  printf("\nDisconnected Undirected Graph:\n");

  PUNGraph G;
  
  // As benchmark, this is done once and compared visually to confirm values are correct
  const int NNodes = 30;
  G = GenRndPowerLaw(NNodes, 2.5);
  
  TIntStrH NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlNeato, TStr::Fmt("%s/sample_cncom_unpower.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
  
  printf("IsConnected(G) = %d\n", IsConnected(G));
  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get weakly connected component counts
  TIntPrV WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%d, %d)\n", i, WccSzCnt[i].Val1.Val, WccSzCnt[i].Val2.Val);
  }
  
  // Get all nodes in same component
  TIntV CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %d\n", CnCom.Len());
  
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %d\n", i, WCnComV[i].Len());
    for (int j = 0; j < WCnComV[i].Len(); j++) {
      printf("WCnComV[%d][%d] = %d\n", i, j, WCnComV[i][j].Val);
    }
  }
  
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest SCC
  PUNGraph GMx = GetMxWcc(G);
  printf("GMx->GetNodes() = %d, GMx->GetEdges() = %d\n", GMx->GetNodes(), GMx->GetEdges());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %d\n", i, SCnComV[i].Len());
  }
  
  // Get largest bi-connected component
  PUNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %d, GMxBi->GetEdges() = %d\n", GMxBi->GetNodes(), GMxBi->GetEdges());
  
  // Get bi-connected size components counts
  TIntPrV SzCntV;
  GetBiConSzCnt(G, SzCntV);
  for (int i = 0; i < SzCntV.Len(); i++) {
    printf("SzCntV[%d] = (%d, %d)\n", i, SzCntV[i].Val1.Val, SzCntV[i].Val2.Val);
  }
  
  // Get bi-connected components
  TCnComV BiCnComV;
  GetBiCon(G, BiCnComV);
  for (int i = 0; i < BiCnComV.Len(); i++) {
    printf("BiCnComV[%d].Len() = %d\n", i, BiCnComV[i].Len());
  }
  
  // Get articulation points of a graph
  TIntV ArtNIdV;
  GetArtPoints(G, ArtNIdV);
  printf("ArtNIdV.Len() = %d\n", ArtNIdV.Len());
  for (int i = 0; i < ArtNIdV.Len(); i++) {
    printf("ArtNIdV[%d] = %d\n", i, ArtNIdV[i].Val);
  }
  
  // Get edge bridges of a graph
  TIntPrV EdgeV;
  GetEdgeBridges(G, EdgeV);
  printf("EdgeV.Len() = %d\n", EdgeV.Len());
  for (int i = 0; i < EdgeV.Len(); i++) {
    printf("EdgeV[%d] = (%d, %d)\n", i, EdgeV[i].Val1.Val, EdgeV[i].Val2.Val);
  }
  
  // Get 1-components counts
  TIntPrV SzCnt1ComV;
  Get1CnComSzCnt(G, SzCnt1ComV);
  printf("SzCnt1ComV.Len() = %d\n", SzCnt1ComV.Len());
  for (int i = 0; i < SzCnt1ComV.Len(); i++) {
    printf("SzCnt1ComV[%d] = (%d, %d)\n", i, SzCnt1ComV[i].Val1.Val, SzCnt1ComV[i].Val2.Val);
  }
  
  // Get 1-components
  TCnComV Cn1ComV;
  Get1CnCom(G, Cn1ComV);
  for (int i = 0; i < Cn1ComV.Len(); i++) {
    printf("Cn1ComV[%d].Len() = %d\n", i, Cn1ComV[i].Len());
  }
  
}

// Get connected components on directed graph that is not fully connected
void GetCnComDirectedDisconnected() {
  
  printf("\nDisconnected Directed Graph:\n");
  PNGraph G;
  
// Create benchmark graph
  const int NNodes = 30;
  G = GenRndGnm<PNGraph>(NNodes, NNodes);

  // Add some more random edges
  for (int i = 0; i < 10; i++) {
    TInt Src, Dst;
    do {
      Src = G->GetRndNId();
      Dst = G->GetRndNId();
    }
    while (Src == Dst || G->IsEdge(Src, Dst));
    G->AddEdge(Src, Dst);
  }

  // Add isolated component
  G->AddNode(NNodes);
  G->AddNode(NNodes+1);
  G->AddNode(NNodes+2);
  G->AddEdge(NNodes, NNodes+1);
  G->AddEdge(NNodes+1, NNodes+2);
  G->AddEdge(NNodes+2, NNodes+1);

  printf("Nodes = %d, Edges = %d\n", G->GetNodes(), G->GetEdges());

  TIntStrH NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlDot, TStr::Fmt("%s/sample_cncom_ngraph.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
  
  printf("IsConnected(G) = %d\n", IsConnected(G));
  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get weakly connected component counts
  TIntPrV WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%d, %d)\n", i, WccSzCnt[i].Val1.Val, WccSzCnt[i].Val2.Val);
  }
  
  // Get all nodes in same component
  TIntV CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %d\n", CnCom.Len());
  
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %d\n", i, WCnComV[i].Len());
    for (int j = 0; j < WCnComV[i].Len(); j++) {
      printf("WCnComV[%d][%d] = %d\n", i, j, WCnComV[i][j].Val);
    }
  }
  
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest WCC
  PNGraph GWMx = GetMxWcc(G);
  printf("GWMx->GetNodes() = %d, GWMx->GetEdges() = %d\n", GWMx->GetNodes(), GWMx->GetEdges());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %d\n", i, SCnComV[i].Len());
  }
  
  double MxSccSz = GetMxSccSz(G);
  printf("MxSccSz = %.5f\n", MxSccSz);
  
  // Get graph with largest SCC
  PNGraph GSMx = GetMxScc(G);
  printf("GSMx->GetNodes() = %d, GSMx->GetEdges() = %d\n", GSMx->GetNodes(), GSMx->GetEdges());
  
  // Get largest bi-connected component
  PNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %d, GMxBi->GetEdges() = %d\n", GMxBi->GetNodes(), GMxBi->GetEdges());
  
}

// Get connected components on full graph
void GetCnComCompleteGraph() {
  
  GetCnComFull<PUNGraph>();
  
  GetCnComFull<PNGraph>();
  
  GetCnComFull<PNEGraph>();
  
  // Test the remaining functions (for undirected graphs only)
  const int NNodes = 500;
  
  PUNGraph G = GenFull<PUNGraph>(NNodes);
  
  // Get bi-connected size components counts
  TIntPrV SzCntV;
  GetBiConSzCnt(G, SzCntV);
  printf("SzCntV.Len() = %d\n", SzCntV.Len());
  printf("SzCntV[0] = (%d, %d)\n", SzCntV[0].Val1.Val, SzCntV[0].Val2.Val);
  
  // Get bi-connected components
  TCnComV BiCnComV;
  GetBiCon(G, BiCnComV);
  printf("BiCnComV[0].Len() = %d\n", BiCnComV[0].Len());
  printf("BiCnComV.Len() = %d\n", BiCnComV.Len());
  
  // Get articulation points of a graph
  TIntV ArtNIdV;
  GetArtPoints(G, ArtNIdV);
  printf("ArtNIdV.Len() = %d\n", ArtNIdV.Len());

  // Get edge bridges of a graph
  TIntPrV EdgeV;
  GetEdgeBridges(G, EdgeV);
  printf("EdgeV.Len() == %d\n", EdgeV.Len());
  
  // Get 1-components counts
  TIntPrV SzCnt1ComV;
  Get1CnComSzCnt(G, SzCnt1ComV);
  printf("SzCnt1ComV.Len() == %d\n", SzCnt1ComV.Len());
  
  // Get 1-components
  TCnComV Cn1ComV;
  Get1CnCom(G, Cn1ComV);
  printf("Cn1ComV.Len() == %d\n", Cn1ComV.Len());
  
  
}

int main(int argc, char* argv[]) {
  
  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);
  
  GetCnComCompleteGraph();
  GetCnComUndirectedDisconnected();
  GetCnComDirectedDisconnected();
}
