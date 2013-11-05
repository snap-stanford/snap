#include "Snap.h"

int main(){
  TTableContext Context;
  // create scheme
  TTable::Schema AnimalS;
  AnimalS.Add(TPair<TStr,TAttrType>("Animal", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Size", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Location", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Number", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);
  RelevantCols.Add(2);
  RelevantCols.Add(3);

  PTable P = TTable::LoadSS("Animals", AnimalS, "tests/animals.txt", Context, RelevantCols);

  P->SaveSS("tests/p1.txt");

  TStrV cols;
  cols.Add("Size");
  cols.Add("Number");

  P->Group(cols, "AnimalGroup", true);

  P->Unique(cols, true);

  //TStrV group1;
  //group1.Add("Location");
  //P->Group(group1, "LocationGroup");
  //group1.Add("Size");
  //P->Group(group1, "LocationSizeGroup");

  P->SaveSS("tests/p2.txt");
  return 0;
}
