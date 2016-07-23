#include "Snap.h"

int main(){
  TTableContext Context;
  // create scheme
  Schema CountryS;
  CountryS.Add(TPair<TStr,TAttrType>("Country", atStr));
  CountryS.Add(TPair<TStr,TAttrType>("Continent", atStr));
  CountryS.Add(TPair<TStr,TAttrType>("Size", atStr));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);
  RelevantCols.Add(2);

  Schema ContinentS;
  ContinentS.Add(TPair<TStr,TAttrType>("Continent", atStr));
  ContinentS.Add(TPair<TStr,TAttrType>("Size", atStr));
  RelevantCols.Clr();
  RelevantCols.Add(0);
  RelevantCols.Add(1);

  PTable P = TTable::LoadSS(CountryS, "tests/country.txt", Context, RelevantCols);
  P->SaveSS("tests/p.txt");

  P->Unique("Continent");
  P->SaveSS("tests/p1.txt");

  P->Defrag();

  P->SaveSS("tests/p2.txt");

  THash<TInt, TInt> Hash = P->GetRowIdMap();

  for (THash<TInt, TInt>::TIter it = Hash.BegI(); it != Hash.EndI(); it++) {
    TInt id = it.GetKey();
    TInt row = it.GetDat();
    printf("Id: %d, Row: %d\n", id.Val, row.Val);
  }

  PTable Q = TTable::LoadSS(ContinentS, "tests/continent.txt", Context, RelevantCols);
  Q->SaveSS("tests/q.txt");

  PTable R = P->Join("Continent", *Q, "Continent");
  R->SaveSS("tests/r.txt");

  return 0;
}
