#include <gtest/gtest.h>

#include "Snap.h"

// Tests the default constructor.
TEST(TTable, DefaultConstructor) {
  TTable Table;

  EXPECT_EQ(0, Table.GetNumRows().Val);
  EXPECT_EQ(0, Table.GetNumValidRows().Val);
}

// Tests load and save from text file.
TEST(TTable, LoadSave) {
  TTableContext Context;
  // Create schema.
  Schema GradeS;
  GradeS.Add(TPair<TStr,TAttrType>("Class", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Area", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Quarter", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2011", atInt));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2012", atInt));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2013", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);
  RelevantCols.Add(3); RelevantCols.Add(4); RelevantCols.Add(5);

  PTable P = TTable::LoadSS(GradeS, "table/grades.txt", &Context, RelevantCols);

  EXPECT_EQ(5, P->GetNumRows().Val);
  EXPECT_EQ(5, P->GetNumValidRows().Val); 

  EXPECT_EQ(7, P->GetIntVal("Grade 2011", 0).Val);
  EXPECT_EQ(9, P->GetIntVal("Grade 2013", 4).Val);
  EXPECT_STREQ("Compilers", P->GetStrVal("Class", 3).CStr());

  P->SaveSS("table/p1.txt");

  // Test SaveSS by loading the saved table and testing values again.
  GradeS.Add(TPair<TStr,TAttrType>("_id", atInt));
  P = TTable::LoadSS(GradeS, "table/p1.txt", &Context, RelevantCols);

  EXPECT_EQ(5, P->GetNumRows().Val);
  EXPECT_EQ(5, P->GetNumValidRows().Val); 

  EXPECT_EQ(7, P->GetIntVal("Grade 2011", 0).Val);
  EXPECT_EQ(9, P->GetIntVal("Grade 2013", 4).Val);
  EXPECT_STREQ("Compilers", P->GetStrVal("Class", 3).CStr());
}

// Tests parallel select function.
TEST(TTable, ParallelSelect) {
  TTableContext Context;

  // TODO: Change this to point to a local copy of the LiveJournal table binary.
  // char srcfile[100] = "/dfs/ilfs2/0/ringo/benchmarks/soc-LiveJournal1.table";

  Schema LJS;
  LJS.Add(TPair<TStr,TAttrType>("Src", atInt));
  LJS.Add(TPair<TStr,TAttrType>("Dst", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);

  PTable T1 = TTable::LoadSS(LJS, "table/soc-LiveJournal1_small.txt", &Context, RelevantCols);

  EXPECT_EQ(499, T1->GetNumRows().Val);
  EXPECT_EQ(499, T1->GetNumValidRows().Val); 

  PTable T2 = TTable::New(T1->GetSchema(), &Context);
  T1->SelectAtomicIntConst("Src", 88, LT, T2);

  EXPECT_EQ(196, T2->GetNumRows().Val);
  EXPECT_EQ(196, T2->GetNumValidRows().Val); 
}

// Tests parallel select in-place function.
TEST(TTable, ParallelSelectInPlace) {
  TTableContext Context;

  Schema LJS;
  LJS.Add(TPair<TStr,TAttrType>("Src", atInt));
  LJS.Add(TPair<TStr,TAttrType>("Dst", atInt));
  TIntV RelevantCols; RelevantCols.Add(0); RelevantCols.Add(1);

  PTable T1 = TTable::LoadSS(LJS, "table/soc-LiveJournal1_small.txt", &Context, RelevantCols);

  EXPECT_EQ(499, T1->GetNumRows().Val);
  EXPECT_EQ(499, T1->GetNumValidRows().Val); 

  T1->SelectAtomicIntConst("Src", 87, GT);

  EXPECT_EQ(499, T1->GetNumRows().Val);
  EXPECT_EQ(303, T1->GetNumValidRows().Val); 
}

// Tests parallel join function.
TEST(TTable, ParallelJoin) {
  TTableContext Context;

  Schema LJS;
  LJS.Add(TPair<TStr,TAttrType>("Src", atInt));
  LJS.Add(TPair<TStr,TAttrType>("Dst", atInt));
  TIntV RelevantCols; RelevantCols.Add(0); RelevantCols.Add(1);

  PTable T1 = TTable::LoadSS(LJS, "table/soc-LiveJournal1_small.txt", &Context, RelevantCols);

  EXPECT_EQ(499, T1->GetNumRows().Val);
  EXPECT_EQ(499, T1->GetNumValidRows().Val); 
  
  PTable T2 = TTable::LoadSS(LJS, "table/soc-LiveJournal1_small.txt", &Context, RelevantCols);

  EXPECT_EQ(499, T2->GetNumRows().Val);
  EXPECT_EQ(499, T2->GetNumValidRows().Val); 

  PTable P = T1->Join("Src", T2, "Dst");

  EXPECT_EQ(24, P->GetNumRows().Val);
  EXPECT_EQ(24, P->GetNumValidRows().Val); 
}

// Tests sequential table to graph function.
TEST(TTable, ToGraph) {
  TTableContext Context;

  Schema LJS;
  LJS.Add(TPair<TStr,TAttrType>("Src", atInt));
  LJS.Add(TPair<TStr,TAttrType>("Dst", atInt));
  TIntV RelevantCols; RelevantCols.Add(0); RelevantCols.Add(1);

  PTable T1 = TTable::LoadSS(LJS, "table/soc-LiveJournal1_small.txt", &Context, RelevantCols);

  EXPECT_EQ(499, T1->GetNumRows().Val);
  EXPECT_EQ(499, T1->GetNumValidRows().Val); 

  TVec<TPair<TStr, TAttrType> > S = T1->GetSchema();
  PNGraph Graph = TSnap::ToGraph<PNGraph>(T1, S[0].GetVal1(), S[1].GetVal1(), aaFirst);

  EXPECT_EQ(689,Graph->GetNodes());
  EXPECT_EQ(499,Graph->GetEdges());
  EXPECT_EQ(1,Graph->IsOk());
}

#ifdef GCC_ATOMIC
// Tests parallel table to graph function.
TEST(TTable, ToGraphMP) {
  TTableContext Context;

  Schema LJS;
  LJS.Add(TPair<TStr,TAttrType>("Src", atInt));
  LJS.Add(TPair<TStr,TAttrType>("Dst", atInt));
  TIntV RelevantCols; RelevantCols.Add(0); RelevantCols.Add(1);

  PTable T1 = TTable::LoadSS(LJS, "table/soc-LiveJournal1_small.txt", &Context, RelevantCols);

  EXPECT_EQ(499, T1->GetNumRows().Val);
  EXPECT_EQ(499, T1->GetNumValidRows().Val); 

  TVec<TPair<TStr, TAttrType> > S = T1->GetSchema();
  PNGraphMP Graph = TSnap::ToGraphMP<PNGraphMP>(T1, S[0].GetVal1(), S[1].GetVal1());

  EXPECT_EQ(689,Graph->GetNodes());
  EXPECT_EQ(499,Graph->GetEdges());
  EXPECT_EQ(1,Graph->IsOk());
}
#endif // GCC_ATOMIC
