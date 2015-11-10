#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Node Centrality. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input un/directed graph");
  PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);
  printf("\nRandom Walk with Restart (Personalized PageRank) from node 1 to node 3: %g\n",
    TSnap::GetRndWalkRestartBidirectional(Graph, 0.85, 1, 3));
  printf("Personalized PageRank from start uniformly sampled from [1, 3] to node 6: %g\n",
	 TSnap::GetPersonalizedPageRankBidirectional(Graph, 0.85, TIntV::GetV(1, 3), 6));
  Catch
  return 0;
}
