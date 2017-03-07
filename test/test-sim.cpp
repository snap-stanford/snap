#include <gtest/gtest.h>

#include "Snap.h"

TEST(sim, sequential) {
  PNGraph G = new TNGraph();
  G->AddNode(1);
  G->AddNode(2);
  G->AddNode(3);
  G->AddNode(4);
  G->AddNode(10);
  G->AddNode(11);
  G->AddNode(12);
  G->AddEdge(1,10);
  G->AddEdge(2,10);
  G->AddEdge(1,11);
  G->AddEdge(2,12);
  G->AddEdge(3,11);
  G->AddEdge(3,12);
  G->AddEdge(2,11);
  G->AddEdge(4,10);

  PNEANet K = KNNJaccard(G,3);
  int s = 0;
  float sum = 0;
  for (TNEANet::TEdgeI EI = K->BegEI(); EI < K->EndEI(); EI++ ){
    s += EI.GetDstNId();
    sum += K->GetFltAttrDatE(EI.GetId(), "sim");
  }
  EXPECT_EQ(26, s);
  EXPECT_EQ(8, int(sum));

}
#ifdef GCC_ATOMIC
TEST(sim, parallel) {
  PNGraph G = new TNGraph();
  G->AddNode(1);
  G->AddNode(2);
  G->AddNode(3);
  G->AddNode(4);
  G->AddNode(10);
  G->AddNode(11);
  G->AddNode(12);
  G->AddEdge(1,10);
  G->AddEdge(2,10);
  G->AddEdge(1,11);
  G->AddEdge(2,12);
  G->AddEdge(3,11);
  G->AddEdge(3,12);
  G->AddEdge(2,11);
  G->AddEdge(4,10);

  PNEANet K = KNNJaccardParallel(G,3);
  int s = 0;
  float sum = 0;
  for (TNEANet::TEdgeI EI = K->BegEI(); EI < K->EndEI(); EI++ ){
    s += EI.GetDstNId();
    sum += K->GetFltAttrDatE(EI.GetId(), "sim");
  }
  EXPECT_EQ(26, s);
  EXPECT_EQ(8, int(sum));

}
#endif
