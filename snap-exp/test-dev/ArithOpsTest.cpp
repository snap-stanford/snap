//#include "Table.h"
#include "Snap.h"

int main(){
  TTableContext Context;
  Schema LocS;
  LocS.Add(TPair<TStr,TAttrType>("Name", atStr));
  LocS.Add(TPair<TStr,TAttrType>("X", atInt));
  LocS.Add(TPair<TStr,TAttrType>("Y", atFlt));
  LocS.Add(TPair<TStr,TAttrType>("Z", atInt));
  LocS.Add(TPair<TStr,TAttrType>("W", atFlt));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);
  RelevantCols.Add(2);
  RelevantCols.Add(3);
  RelevantCols.Add(4);

  PTable P = TTable::LoadSS(LocS, "tests/loc.txt", Context, RelevantCols, '\t', true);
  PTable Q = TTable::LoadSS(LocS, "tests/loc.txt", Context, RelevantCols, '\t', true);

  P->SaveSS("tests/res.txt");

  P->ColSub("W", *Q, "X", "foo");
  P->ColDiv("foo", "foo");

  P->ColMul("Y", *Q, "Z", "bar", false);
  P->ColDiv("Y", *Q, "W", "", false);

  P->ColMod("X", *Q, "X");

  Q->ColAdd("X", 1.5, "baz");
  Q->ColMul("baz", 2);
  Q->ColDiv("Y", 10.1);
  Q->ColMod("baz", 3);

  Q->ColMax("X", "baz", "max");
  Q->ColMin("Y", "W", "min");

  P->SaveSS("tests/res1.txt");
  Q->SaveSS("tests/res2.txt");
  return 0;
}
