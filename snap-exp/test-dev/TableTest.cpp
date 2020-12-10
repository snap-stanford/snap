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
  // create table
  PTable T = TTable::LoadSS(AnimalS, "tests/animals.txt", Context, RelevantCols);
  //PTable T = TTable::LoadSS("Animals", AnimalS, "animals.txt");
  T->Unique("Animal");
  TTable Ts = *T;  // did we fix problem with copy-c'tor ?
  //PTable Ts = TTable::LoadSS("Animals_s", AnimalS, "../../testfiles/animals.txt", RelevantCols);
  //Ts->Unique(AnimalUnique);

  // test Select
  // create predicate tree: find all animals that are big and african or medium and Australian
  TAtomicPredicate A1(atStr, true, EQ, "Location", "", 0, 0, "Africa");  
  TPredicateNode N1(A1);  // Location == "Africa"
  TAtomicPredicate A2(atStr, true, EQ, "Size", "", 0, 0, "big");  
  TPredicateNode N2(A2);  // Size == "big"
  TPredicateNode N3(AND);
  N3.AddLeftChild(&N1);
  N3.AddRightChild(&N2);
  TAtomicPredicate A4(atStr, true, EQ, "Location", "", 0, 0, "Australia");  
  TPredicateNode N4(A4);  
  TAtomicPredicate A5(atStr, true, EQ, "Size", "", 0, 0, "medium");  
  TPredicateNode N5(A5); 
  TPredicateNode N6(AND);
  N6.AddLeftChild(&N4);
  N6.AddRightChild(&N5);
  TPredicateNode N7(OR);
  N7.AddLeftChild(&N3);
  N7.AddRightChild(&N6);
  TPredicate Pred(&N7);
  TIntV SelectedRows;
  Ts.Select(Pred, SelectedRows);

  TStrV GroupBy;
  GroupBy.Add("Location");
  T->Group(GroupBy, "LocationGroup");
  GroupBy.Add("Size");
  T->Group(GroupBy, "LocationSizeGroup");
  T->Count("LocationCount", "Location");
  PTable Tj = T->Join("Location", Ts, "Location");
  TStrV UniqueAnimals;
  UniqueAnimals.Add("Animals_1.Animal");
  UniqueAnimals.Add("Animals_2.Animal");
  Tj->Unique(UniqueAnimals, false);
  //print table
   T->SaveSS("tests/animals_out_T.txt");
   Ts.SaveSS("tests/animals_out_Ts.txt");
   Tj->SaveSS("tests/animals_out_Tj.txt");
  return 0;
}
