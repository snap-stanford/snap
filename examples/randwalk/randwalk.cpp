#include "stdafx.h"

void BasicUsage(const PNGraph& Graph) {
  printf("\nRandom Walk with Restart (Personalized PageRank) from node 1 to node 3: %g\n",
         TSnap::GetRndWalkRestartBidirectional(Graph, 0.2, 1, 3));
  printf("Personalized PageRank from start uniformly sampled from [1, 3] to node 6: %g\n",
         TSnap::GetPersonalizedPageRankBidirectional(Graph, 0.2, TIntV::GetV(1, 3), 6));
}

void ShowForwardReverseTimeForTuning(const PNGraph& Graph) {
  int pairCount = 100;
  TRnd Rnd(0); // 0 means seed from clock
  TTmStopWatch w;
  w.Start();
  for (int i = 0; i < pairCount; i++) {
    int startId = Graph->GetRndNId();
    int targetId = Graph->GetRndNId();
    printf("PPR from node %d to node %d: %g\n", startId, targetId,
           TSnap::GetPersonalizedPageRankBidirectional(Graph, 0.2, TIntV::GetV(startId), targetId, 0.1f, 4.0f / Graph->GetNodes(), false, true));
  }
  printf("Total time for %d pairs: %g\n", pairCount, w.GetSec());
}

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("RandWalk. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input un/directed graph");
  PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);
  
  BasicUsage(Graph);
  ShowForwardReverseTimeForTuning(Graph);
  
  Catch
  return 0;
}
