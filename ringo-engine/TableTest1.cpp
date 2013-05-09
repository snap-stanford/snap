#include "Table.h"
#include <iostream>
using namespace std;
// select only big animals
// self join on location
// group by (Animal, Animal)
// unique by group idx

int main(){
  // create scheme
  TTable::Schema AnimalS;
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Animal", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Size", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Location", TTable::STR));
  AnimalS.Add(TPair<TStr,TTable::TYPE>("Number", TTable::INT));
  // create table
  PTable T1 = TTable::LoadSS("Animals1", AnimalS, "../../testfiles/animals.txt");
  PTable T2 = TTable::LoadSS("Animals2", AnimalS, "../../testfiles/animals.txt");
  // test Select
  // create predicate tree: find all animals that are big and african or medium and Australian
  TPredicate::TAtomicPredicate A1(TPredicate::STR, true, TPredicate::EQ, "Size", "", 0, 0, "big");  
  TPredicate::TPredicateNode N1(A1);  // Size == "big"
  TPredicate Pred(&N1);
  T1->Select(Pred);
  T1->SaveSS("../../testfiles/animals_out_T1.txt");
  PTable Tj = T1->Join("Location", *T2, "Location");
  TStrV GroupBy;
  GroupBy.Add("Animals1.Animal");
  GroupBy.Add("Animals2.Animal");
  Tj->Group("AnimalPair", GroupBy);
  Tj->Unique("AnimalPair");
  //print table
   Tj->SaveSS("../../testfiles/animals_out_Tj_1.txt");
  return 0;
}