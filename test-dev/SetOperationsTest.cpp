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

  PTable P = TTable::LoadSS(AnimalS, "tests/animals.txt", Context, RelevantCols);
  PTable Q = TTable::LoadSS(AnimalS, "tests/more_animals.txt", Context, RelevantCols);

  PTable P1 = P->UnionAll(*P);
  PTable P2 = P1->UnionAll(*Q);
  P2->SaveSS("tests/unionall.txt");

  PTable R = P->Union(*Q);
  R->SaveSS("tests/union.txt");

  PTable S = P->Intersection(*Q);
  S->SaveSS("tests/intersection.txt");

  PTable M = P->Minus(*Q);
  M->SaveSS("tests/minus.txt");

  TStrV Cols;
  Cols.Add("Animal");
  Cols.Add("Size");
  PTable T = P->Project(Cols);

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
