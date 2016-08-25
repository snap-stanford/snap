#include "Snap.h"

int main(){
  TTableContext Context;
  // create scheme
  Schema GradeS;
  GradeS.Add(TPair<TStr,TAttrType>("Class", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Area", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Quarter", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2011", atFlt));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2012", atFlt));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2013", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);
  RelevantCols.Add(3); RelevantCols.Add(4); RelevantCols.Add(5);

  PTable P = TTable::LoadSS(GradeS, "tests/grades.txt", Context, RelevantCols);

  P->SaveSS("tests/p1.txt");

  TStrV group1;
  group1.Add("Area");
  group1.Add("Quarter");
  P->Group(group1, "Category");
  P->SaveSS("tests/p2.txt");

  //TStrV cols;
  //cols.Add("Area");
  //P->Unique(cols);
  //P->SaveSS("tests/p4.txt");

  TStrV G1; G1.Add("Category");
  TStrV G2; G2.Add("Quarter");

  //P->Count("Quarter", "QuarterCount");
  P->Aggregate(G1, aaMean, "Grade 2011", "Av11");
  P->Aggregate(G2, aaMean, "Grade 2012", "Av12");

  TStrV aggrcols;
  aggrcols.Add("Grade 2011");
  aggrcols.Add("Av12");
  P->AggregateCols(aggrcols, aaMax, "AggrMax");
  //aggrcols.Add("Grade 2013");
  //aggrcols.Add("Grade 2013");
  //P->AggregateCols(aggrcols, aaSum, "AggrSum");

  P->SaveSS("tests/p3.txt");
  return 0;
}
