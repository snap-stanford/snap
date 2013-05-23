#include <gtest/gtest.h>

#include "Snap.h"

// Simple test for Defrag()
TEST(THashSet, Defrag) {
  TIntSet* TestSet = new TIntSet();

  // fragment the set (IsKeyIdEqKeyN() will be false)
  TestSet->AddKey(6);
  TestSet->AddKey(4);
  EXPECT_EQ(1,TestSet->IsKeyIdEqKeyN());

  TestSet->AddKey(2);
  EXPECT_EQ(1,TestSet->IsKeyIdEqKeyN());

  TestSet->DelKey(2);
  EXPECT_EQ(0,TestSet->IsKeyIdEqKeyN());

  TestSet->Defrag();
  EXPECT_EQ(1,TestSet->IsKeyIdEqKeyN());

  TestSet->DelKey(4);
  EXPECT_EQ(0,TestSet->IsKeyIdEqKeyN());

  TestSet->Defrag();
  EXPECT_EQ(1,TestSet->IsKeyIdEqKeyN());

  // this does not work with a fragmented set
  TestSet->DelKeyId(TestSet->GetRndKeyId(TInt::Rnd));
  EXPECT_EQ(0,TestSet->IsKeyIdEqKeyN());

  delete TestSet;
}
