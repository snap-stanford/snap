#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TStrPool, DefaultConstructor) {
  PStrPool Pool;

  Pool = TStrPool::New();
  EXPECT_EQ(1,Pool->Len());
  EXPECT_EQ(0,Pool->Empty());

  // printf("Len %d\n", Pool->Len());
  // printf("Size %d\n", Pool->Size());
  // printf("Empty %d\n", Pool->Empty());

  Pool->Clr();
  EXPECT_EQ(0,Pool->Len());
  EXPECT_EQ(1,Pool->Empty());

  // printf("Len %d\n", Pool->Len());
  // printf("Size %d\n", Pool->Size());
  // printf("Empty %d\n", Pool->Empty());

  Pool->AddStr("");
  EXPECT_EQ(1,Pool->Len());
  EXPECT_EQ(0,Pool->Empty());

  // printf("Len %d\n", Pool->Len());
  // printf("Size %d\n", Pool->Size());
  // printf("Empty %d\n", Pool->Empty());
}

// Test the pool operations
TEST(TStrPool, PoolOps) {
  int NStr = 1000000;
  const char *FName = "test.pool.dat";
  int i;
  char Str[32];
  PStrPool Pool;
  PStrPool Pool1;
  PStrPool Pool2;
  TIntV PosV;
  uint Pos;

  Pool = TStrPool::New();
  
  // fill in the pool
  for (i = 0; i < NStr; i++) {
    sprintf(Str, "%d", i);
    Pos = Pool->AddStr(Str);
    PosV.Add(Pos);
    // printf("Pos %d\n", Pos);
    // printf("Len %d\n", Pool->Len());
    // printf("Size %d\n", Pool->Size());
    // printf("Empty %d\n", Pool->Empty());
  }

  // test the values
  for (i = 0; i < NStr; i++) {
    Pos = PosV[i];
    sprintf(Str, "%d", i);
    EXPECT_EQ(0,Pool->Cmp(Pos, Str));
  }

  // save the pool
  {
    TFOut FOut(FName);
    Pool->Save(FOut);
    FOut.Flush();
  }

  // load the pool
  {
    TFIn FIn(FName);
    Pool1 = TStrPool::Load(FIn);
  }

  // test the values
  for (i = 0; i < NStr; i++) {
    Pos = PosV[i];
    sprintf(Str, "%d", i);
    EXPECT_EQ(0,Pool1->Cmp(Pos, Str));
  }

  // test the values
  for (i = 0; i < NStr; i++) {
    Pos = PosV[i];
    EXPECT_EQ(0,Pool1->Cmp(Pos, Pool->GetStr(Pos).CStr()));
  }

  // copy the pool
  Pool2 = TStrPool::New();
  *Pool2 = *Pool;

  // test the values
  for (i = 0; i < NStr; i++) {
    Pos = PosV[i];
    sprintf(Str, "%d", i);
    EXPECT_EQ(0,Pool2->Cmp(Pos, Str));
  }

  // test the values
  for (i = 0; i < NStr; i++) {
    Pos = PosV[i];
    EXPECT_EQ(0,Pool2->Cmp(Pos, Pool->GetStr(Pos).CStr()));
  }

  Pool->Clr();
  EXPECT_EQ(0,Pool->Len());
  EXPECT_EQ(1,Pool->Empty());

  Pool->AddStr("");
  EXPECT_EQ(1,Pool->Len());
  EXPECT_EQ(0,Pool->Empty());
}

#if 0
void TestDestructor();

// Test the pool asserts
TEST(TStrPool, PoolAsserts) {
  int i;
  char Str[32];
  PStrPool Pool;
  TStrPool Tool;
  uint Pos;

  Pool = TStrPool::New();
  for (i = 0; i < 10000; i++) {
    sprintf(Str, "%d", i);
    Pos = Pool->AddStr(Str);
  }
  
#if 0
  // test the 4GB overflow
  for (i = 0; ; i++) {
    sprintf(Str, "%d", i);
    Pos = Pool->AddStr(Str);
    // printf("Pos %d\n", Pos);
    // printf("Len %d\n", Pool->Len());
    // printf("Size %d\n", Pool->Size());
    // printf("Empty %d\n", Pool->Empty());
  }
#endif

#if 0
  TestDestructor();
#endif

#if 0
  // test the copy constructor

  // TStrPool Tool1(Tool);
  PStrPool Pool7 = PStrPool(new TStrPool(*Pool));
#endif

#if 0
  // test memory allocation
  Pool = TStrPool::New(1000000);
#endif
}

#if 0
  // ignore this code, included for a future reference
  // Pool1 = TStrPool::New(Pool);
  // Pool1 = TStrPool(Pool);
  // PStrPool Pool1(Pool);
  // PStrPool Pool2(Pool());
  // PStrPool Pool3(TStrPool(Pool));
  // PStrPool Pool4(TStrPool(Pool()));
  // PStrPool Pool5(TStrPool(*Pool));
  // TPt <TStrPool> Pool6 = Pool;
  // Pool1 = Pool;
#endif

void TestDestructor() {
  int i;
  char Str[32];
  PStrPool Pool;
  uint Pos;

  Pool = TStrPool::New();

  // test the destructor
  for (i = 0; i < 10000; i++) {
    sprintf(Str, "%d", i);
    Pos = Pool->AddStr(Str);
    // printf("Pos %d\n", Pos);
    // printf("Len %d\n", Pool->Len());
    // printf("Size %d\n", Pool->Size());
    // printf("Empty %d\n", Pool->Empty());
  }
}

#endif

