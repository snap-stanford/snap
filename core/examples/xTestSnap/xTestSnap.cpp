#include "stdafx.h"

int main(int argc, char* argv[]) {
  PUNGraph G = TUNGraph::New();
  for (int i =0; i < 5; i++) {
    G->AddNode(i); }
  G->AddEdge(0,1);
  G->AddEdge(0,2);
  G->AddEdge(1,2);
  G->AddEdge(0,3);
  G->AddEdge(3,4);
  //G->AddEdge(4,3);
  G->AddEdge(4,4);
  TIntTrV NIdCOTriadV;
  TSnap::GetTriads(G, NIdCOTriadV);
  NIdCOTriadV.Sort();
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    printf("%d\t%d\t%d\n", NIdCOTriadV[i].Val1, NIdCOTriadV[i].Val2, NIdCOTriadV[i].Val3);
  }
  int Close, Open;
  TSnap::GetTriads(G, Close, Open);
  printf("%d close\n%d open\n", Close, Open);

  return 0;
}

