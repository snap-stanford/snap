#include "Snap.h"
//#include "Table.h"

// select only big animals
// self join on location
// group by (Animal, Animal)
// unique by group idx

int main(){
  TTableContext Context;
  // create scheme
  Schema AnimalS;
  AnimalS.Add(TPair<TStr,TAttrType>("Animal", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Size", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Location", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Number", atInt));
  // create table
  PTable T1 = TTable::LoadSS("Animals", AnimalS, "../../testfiles/animals.txt", Context);
  PTable T2 = TTable::New(T1);
  //PTable T2 = TTable::LoadSS("Animals", AnimalS, "../../testfiles/animals.txt");
  // vector to be used as dummy parameter to the select function calls
  TIntV SelectedRows;
  // test Select
  TPredicate::TAtomicPredicate A1(atStr, true, EQ, "Size", "", 0, 0, "big");  
  TPredicate::TPredicateNode N1(A1);  // Size == "big"
  TPredicate Pred(&N1);
  //T1->Select(Pred);
  T1->SelectAtomicStrConst("Size", "ig", SUPERSTR, SelectedRows);
  T2->SelectAtomicStrConst("Size", "big", EQ, SelectedRows);
  //PTable Tj = T1->Join("Location", *T2, "Location");
  PTable Tj = T1->Join("Location", *T1, "Location");
  Tj->SaveSS("../../testfiles/animals_out_Tj_raw.txt");
  TStrV GroupBy;
  GroupBy.Add("Animals_1.Animal");
  GroupBy.Add("Animals_2.Animal");
  Tj->Unique(GroupBy, false);
  Tj->SetSrcCol("Animals_1.Animal");
  Tj->SetDstCol("Animals_2.Animal");
  PNEANet G = Tj->ToGraph(aaLast);
  //print table
  Tj->SaveSS("../../testfiles/animals_out_Tj_1.txt");
  G->Dump();

  // Join on Location to get animal pairs
  // select the animal pairs of animals of the same size
  // group by (Animal, Animal)
  // unique by group idx

  PTable T3 = TTable::LoadSS("Animals", AnimalS, "../../testfiles/animals.txt", Context);
  PTable To = T3->Join("Location", *T3, "Location");
  TPredicate::TAtomicPredicate A2(atStr, false, EQ, "Animals_1.Size", "Animals_2.Size");  
  TPredicate::TPredicateNode N2(A2);
  TPredicate Pred2(&N2);
  To->Select(Pred2, SelectedRows);
  TStrV GroupBy1;
  GroupBy1.Add("Animals_1.Animal");
  GroupBy1.Add("Animals_2.Animal");
  To->Unique(GroupBy1, false);
  //print table
  To->SaveSS("../../testfiles/animals_out_To_1.txt");
  return 0;
}
