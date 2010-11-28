#include "Snap.h"

int main(int argc, char* argv[]) {
  // create a graph and save it
  { PNGraph Graph = TNGraph::New();
  for (int i = 0; i < 10; i++) {
    Graph->AddNode(i); }
  for (int i = 0; i < 10; i++) {
    Graph->AddEdge(i, TInt::Rnd.GetUniDevInt(10)); }
  TSnap::SaveEdgeList(Graph, "graph.txt", "Edge list format"); }
  // load a graph
  PNGraph Graph;
  Graph = TSnap::LoadEdgeList<PNGraph>("graph.txt", 0, 1);
  // traverse nodes
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", NI.GetId(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    for (int e = 0; e < NI.GetOutDeg(); e++) { printf("  %d", NI.GetOutNId(e)); }
    printf("\nInNodes: ");
    for (int e = 0; e < NI.GetInDeg(); e++) { printf("  %d", NI.GetInNId(e)); }
    printf("\n\n");
  }
  // graph statistic
  TSnap::PrintInfo(Graph, "Graph info");
  PNGraph MxWcc = TSnap::GetMxWcc(Graph);
  TSnap::PrintInfo(MxWcc, "Largest Weakly connected component");
  // random graph
  PNGraph RndGraph = TSnap::GenRndGnm<PNGraph>(100, 1000);
  TGStat GraphStat(RndGraph, TSecTm(1), TGStat::AllStat(), "Gnm graph");
  GraphStat.PlotAll("RndGraph", "Random graph on 1000 nodes");
  // Forest Fire graph
  { TFfGGen ForestFire(false, 1, 0.35, 0.30, 1.0, 0.0, 0.0);
  ForestFire.GenGraph(100);
  PNGraph FfGraph = ForestFire.GetGraph(); }
  // network
  TPt<TNodeEDatNet<TStr, TStr> > Net = TNodeEDatNet<TStr, TStr>::New();
  Net->AddNode(0, "zero");
  Net->AddNode(1, "one");
  Net->AddEdge(0, 1, "zero to one");
  return 0;
}
