#include "Table.h"

int main(){
  TTableContext Context;
  // create scheme
  TTable::Schema AnimalS;
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Animal", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Size", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Location", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Number", TTable::INT));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);
  RelevantCols.Add(2);
  RelevantCols.Add(3);

  PTable P = TTable::LoadSS("Animals", AnimalS, "tests/animals.txt", Context, RelevantCols);
  PTable Q = TTable::LoadSS("MoreAnimals", AnimalS, "tests/more_animals.txt", Context, RelevantCols);

  PTable R = P->Union(*Q, "union");
  PTable S = P->Intersection(*Q, "intersection");
  PTable M = P->Minus(*Q, "minus");

  TStrV Cols;
  Cols.Add("Animal");
  Cols.Add("Size");
  PTable T = P->Project(Cols, "projection");

  P->ProjectInPlace(Cols);

  R->SaveSS("./union.txt");
  S->SaveSS("./intersection.txt");
  M->SaveSS("./minus.txt");
  T->SaveSS("./projection.txt");
  
  TFOut OutFile("loadsave_test.dat");
  P->Save(OutFile);
  OutFile.Flush();
  TFIn InFile("loadsave_test.dat");
  PTable U = TTable::Load(InFile, Context);
  U->SaveSS("./loadsave_test.txt");
  return 0;
}
