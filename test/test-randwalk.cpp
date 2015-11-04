#include <gtest/gtest.h>


#include "Snap.h"

class PersonalizedPageRankTest { };  // For gtest highlighting

using namespace TSnap;

TEST(PPRTest, CorrectOnSmallGraph) {
  PNGraph graph = TSnap::LoadEdgeList<PNGraph>("randwalk_test_graph.txt", false);
  FILE* truePPRFile = fopen ("randwalk_test_pprs.txt","r");
  int s, t;
  float truePPR;
  while(fscanf(truePPRFile, "%d %d %f", &s, &t, &truePPR) == 3) {
    float estimate = TSnap::GetRndWalkRestartBidirectional(graph, 0.2, s, t, 0.03, 0.1, true);
    float relError = fabs(estimate - truePPR) / truePPR;
    EXPECT_TRUE(relError < 0.1);
    //printf("%d %d %f %f %f\n", s, t, estimate, truePPR, relError);
  }  
}

TEST(PPRTest, CorrectOnLineGraph) {
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

// Because the priority queue was implemented for PPR computation, test it here
TEST(PriorityQueueTest, IsCorrect) {
  MaxPriorityQueue q;
  
  // Test queue can sort elements
  q.Insert(2, 2.1f);
  q.Insert(1, 1.1f);
  q.Insert(-5, -5.1f);
  q.Insert(4, 4.1f);
  q.SetPriority(3, 3.1f);
  EXPECT_TRUE(q.GetPriority(1) == 1.1f);
  EXPECT_TRUE(q.GetPriority(-5) == -5.1f);
  EXPECT_TRUE(q.GetPriority(12345) == 0.0f);
  EXPECT_TRUE(q.GetMaxPriority() == 4.1f);
  EXPECT_TRUE(q.PopMax() == 4);
  EXPECT_TRUE(q.PopMax() == 3);
  EXPECT_TRUE(q.PopMax() == 2);
  EXPECT_TRUE(q.PopMax() == 1);
  EXPECT_TRUE(q.PopMax() == -5);
  EXPECT_TRUE(q.IsEmpty());
  
  // Test modifying priorities
  q.Insert(22, 0.5f);
  q.Insert(1, 1.1f);
  q.Insert(4, 4.4f);
  q.SetPriority(4, 0.4f);
  EXPECT_TRUE(q.GetPriority(4) == 0.4f);
  EXPECT_TRUE(q.GetMaxPriority() == 1.1f);
  q.SetPriority(22, 2.0f);
  q.SetPriority(4, 4.0f);
  EXPECT_TRUE(q.GetMaxPriority() == 4.0);
  EXPECT_TRUE(q.GetPriority(22) == 2.0f);
  q.SetPriority(3, 100.1f); // Should implicitly add
  q.SetPriority(3, 3.0f);
  EXPECT_TRUE(q.GetPriority(3) == 3.0f);
  EXPECT_TRUE(q.GetMaxPriority() == 4.0f);
  EXPECT_TRUE(q.PopMax() == 4);
  //printf("max priority %g\n", q.GetMaxPriority());
  //printf("Pop: %d\n", (int) q.PopMax());
  EXPECT_TRUE(q.PopMax() == 3);
  EXPECT_TRUE(q.PopMax() == 22);
  EXPECT_TRUE(q.PopMax() == 1);
  EXPECT_TRUE(q.IsEmpty());
}
