#include "Snap.h"

using namespace TSnap;

// Get connected components on directed graph that is not fully connected
void GetCnCom() {
  
  PNGraph G;
  
// Create benchmark graph
  const int NNodes = 10000;
  const int NEdges = 5000;
  G = GenRndGnm<PNGraph>(NNodes, NEdges);
  
  printf("IsConnected(G) = %s\n", (IsConnected(G) ? "True" : "False"));
  printf("IsWeaklyConnected(G) = %s\n", (IsWeaklyConn(G) ? "True" : "False"));
  
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
  PNGraph GMx = GetMxWcc(G);
  printf("GMx: GetNodes() = %d, GetEdges() = %d\n", GMx->GetNodes(), GMx->GetEdges());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %d\n", i, SCnComV[i].Len());
  }
  
  // Get largest bi-connected component
  PNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi: GetNodes() = %d, GetEdges() = %d\n", GMxBi->GetNodes(), GMxBi->GetEdges());
  
}

int main(int argc, char* argv[]) {
  GetCnCom();
}

