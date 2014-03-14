#include <gtest/gtest.h>

#include "Snap.h"

TEST(FlowTest, BasicTest) {
  PNEANet Net = TNEANet::Load("flow/small_sample");
  int PRFlow1 = TSnap::GetMaxFlowIntPR(Net, 53, 2);
  int EKFlow1 = TSnap::GetMaxFlowIntEK(Net, 53, 2);
  int PRFlow2 = TSnap::GetMaxFlowIntPR(Net, 86, 77);
  int EKFlow2 = TSnap::GetMaxFlowIntEK(Net, 86, 77);
  int PRFlow3 = TSnap::GetMaxFlowIntPR(Net, 62, 81);
  int EKFlow3 = TSnap::GetMaxFlowIntEK(Net, 62, 81);
  int PRFlow4 = TSnap::GetMaxFlowIntPR(Net, 92, 92);
  int EKFlow4 = TSnap::GetMaxFlowIntEK(Net, 92, 92);
  EXPECT_EQ (PRFlow1, EKFlow1);
  EXPECT_EQ (PRFlow2, EKFlow2);
  EXPECT_EQ (PRFlow3, EKFlow3);
  EXPECT_EQ (PRFlow4, EKFlow4);
  EXPECT_EQ (PRFlow1, 1735);
  EXPECT_EQ (PRFlow2, 3959);
  EXPECT_EQ (PRFlow3, 2074);
  EXPECT_EQ (PRFlow4, 0);
}
