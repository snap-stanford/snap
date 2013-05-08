#include "Table.h"
#include <iostream>
using namespace std;

int main(){
  // create scheme
  TTable::Schema AnimalS;
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Animal", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Size", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Location", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Number", TTable::INT));
  // create table
  //PTable T = TTable::LoadSS("Animals", AnimalS, "../../testfiles/animals.txt");
  PTable T = TTable::LoadSS("Animals", AnimalS, "animals.txt");
  T->Unique("Animal");

  // create predicate tree: find all animals that are big and african or medium and Australian
  /*
  TPredicate::TAtomicPredicate A1(TPredicate::STR, true, TPredicate::EQ, "Location", "", 0, 0, "Africa");  
  TPredicate::TPredicateNode N1(A1);  // Location == "Africa"
  TPredicate::TAtomicPredicate A2(TPredicate::STR, true, TPredicate::EQ, "Size", "", 0, 0, "big");  
  TPredicate::TPredicateNode N2(A2);  // Size == "big"
  TPredicate::TPredicateNode N3(TPredicate::AND);
  N3.AddLeftChild(&N1);
  N3.AddRightChild(&N2);
  TPredicate::TAtomicPredicate A4(TPredicate::STR, true, TPredicate::EQ, "Location", "", 0, 0, "Australia");  
  TPredicate::TPredicateNode N4(A4);  
  TPredicate::TAtomicPredicate A5(TPredicate::STR, true, TPredicate::EQ, "Size", "", 0, 0, "medium");  
  TPredicate::TPredicateNode N5(A5); 
  TPredicate::TPredicateNode N6(TPredicate::AND);
  N6.AddLeftChild(&N4);
  N6.AddRightChild(&N5);
  TPredicate::TPredicateNode N7(TPredicate::OR);
  N7.AddLeftChild(&N3);
  N7.AddRightChild(&N6);
  TPredicate Pred(&N7);
  T->Select(Pred);
  */
  TStrV GroupBy;
  GroupBy.Add("Location");
  T->Group("LocationGroup", GroupBy);
  GroupBy.Add("Size");
  T->Group("LocationSizeGroup", GroupBy);
  T->Count("LocationCount", "Location");
  //print table
  //T->SaveSS("../../testfiles/animals_out.txt");
   T->SaveSS("../../testfiles/animals_out.txt");
  return 0;
}