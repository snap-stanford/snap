#include "Table.h"

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
  TPredicate::TAtomicPredicate A1(TPredicate::STR, true, TPredicate::EQ, "Size", "", 0, 0, "big");  
  TPredicate::TPredicateNode N1(A1);  // Size == "big"
  TPredicate Pred(&N1);
  T1->Select(Pred);
  T1->SaveSS("../../testfiles/animals_out_T1.txt");
  PTable Tj = T1->Join("Location", *T2, "Location");
  TStrV GroupBy;
  GroupBy.Add("Animals1.Animal");
  GroupBy.Add("Animals2.Animal");
  Tj->Unique(GroupBy, false);
  //print table
  Tj->SaveSS("../../testfiles/animals_out_Tj_1.txt");

  // Join on Location to get animal pairs
  // select the animal pairs of animals of the same size
  // group by (Animal, Animal)
  // unique by group idx
  /*
  PTable T3 = TTable::LoadSS("Animals3", AnimalS, "../../testfiles/animals.txt");
  //PTable T4 = TTable::LoadSS("Animals4", AnimalS, "../../testfiles/animals.txt");
  TTable T4 = *T3;
  T4.Name = "Animals4";
  PTable To = T3->Join("Location", T4, "Location");
  TPredicate::TAtomicPredicate A2(TPredicate::STR, false, TPredicate::EQ, "Animals3.Size", "Animals4.Size");  
  TPredicate::TPredicateNode N2(A2);
  TPredicate Pred2(&N2);
  To->Select(Pred2);
  TStrV GroupBy1;
  GroupBy1.Add("Animals3.Animal");
  GroupBy1.Add("Animals4.Animal");
  To->Group("AnimalPair", GroupBy1);
  To->Unique("AnimalPair");
  //print table
  To->SaveSS("../../testfiles/animals_out_To_1.txt");
  return 0;
  */

  PTable T3 = TTable::LoadSS("Animals3", AnimalS, "../../testfiles/animals.txt");
  PTable T4 = TTable::LoadSS("Animals4", AnimalS, "../../testfiles/animals.txt");
  PTable To = T3->Join("Location", *T4, "Location");
  TPredicate::TAtomicPredicate A2(TPredicate::STR, false, TPredicate::EQ, "Animals3.Size", "Animals4.Size");  
  TPredicate::TPredicateNode N2(A2);
  TPredicate Pred2(&N2);
  To->Select(Pred2);
  TStrV GroupBy1;
  GroupBy1.Add("Animals3.Animal");
  GroupBy1.Add("Animals4.Animal");
  To->Unique(GroupBy1, false);
  //print table
  To->SaveSS("../../testfiles/animals_out_To_1.txt");
  return 0;
}
