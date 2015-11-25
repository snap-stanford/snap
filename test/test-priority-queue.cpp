#include <gtest/gtest.h>


#include "Snap.h"

class PriorityQueueTest { };  // For gtest highlighting

TEST(PriorityQueueTest, IsCorrect) {
  TMaxPriorityQueue<TInt> q;
  
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
