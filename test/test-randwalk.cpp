#include <gtest/gtest.h>


#include "Snap.h"

class RandWalkTest { };  // For gtest highlighting

using namespace TSnap;

TEST(RandWalkTest, CorrectOnSmallGraph) {
  PNGraph graph = TSnap::LoadEdgeList<PNGraph>("randwalk/test_graph.txt", false);
  FILE* truePPRFile = fopen ("randwalk/test_pprs.txt","r");
  int s, t;
  float truePPR;
  while(fscanf(truePPRFile, "%d %d %f", &s, &t, &truePPR) == 3) {
    float estimate = TSnap::GetRndWalkRestartBidirectional(graph, 0.2, s, t, 0.03, 0.1, true);
    float relError = fabs(estimate - truePPR) / truePPR;
    EXPECT_TRUE(relError < 0.1);
    //printf("%d %d %f %f %f\n", s, t, estimate, truePPR, relError);
  }  
}

TEST(RandWalkTest, CorrectOnLineGraph) {
  PNGraph Graph = TNGraph::New();
  for (int i = 0; i < 100; i++) {
    Graph->AddNode(i);
  }
  for (int i = 0; i < 99; i++) {
    Graph->AddEdge(i, i + 1);
  }
  for (int i = 0; i < 10; i++) {
    double truePPR = 0.2 * pow(0.8, i);
    double estimate = TSnap::GetRndWalkRestartBidirectional(Graph, 0.2, 0, i, 0.01, 0.1, true);
    //printf("pi(0,%d): %f ~= %f\n", i, truePPR, estimate);
    double relError = fabs(estimate - truePPR) / truePPR;
    EXPECT_TRUE( relError < 0.2);
  }
}
