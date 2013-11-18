#include "Snap.h"

int main(){
  TTableContext Context;
  // create scheme
  Schema AnimalS;
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
  PTable Q = TTable::LoadSS("MoreAnimals", AnimalS, "tests/more_animals.txt", Context, RelevantCols);

  PTable R = P->Union(*Q, "union");
  R->SaveSS("tests/union.txt");

  PTable S = P->Intersection(*Q, "intersection");
  S->SaveSS("tests/intersection.txt");

  PTable M = P->Minus(*Q, "minus");
  M->SaveSS("tests/minus.txt");

  TStrV Cols;
  Cols.Add("Animal");
  Cols.Add("Size");
  PTable T = P->Project(Cols, "projection");

  P->ProjectInPlace(Cols);

  T->SaveSS("tests/projection.txt");
  
  TFOut OutFile("tests/loadsave_test.dat");
  P->Save(OutFile);
  OutFile.Flush();
  TFIn InFile("tests/loadsave_test.dat");
  PTable U = TTable::Load(InFile, Context);
  U->SaveSS("tests/loadsave_test.txt");
  return 0;
}
