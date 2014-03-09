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
  PTable T1 = TTable::LoadSS("Animals", AnimalS, "tests/animals.txt", Context, '\t', false);
  PTable T2 = TTable::New(T1);
  //PTable T2 = TTable::LoadSS("Animals", AnimalS, "../../testfiles/animals.txt");

  // test Select
  TAtomicPredicate A1(atStr, true, EQ, "Size", "", 0, 0, "big");  
  TPredicateNode N1(A1);  // Size == "big"
	TPredicate Pred(&N1);
  //T1->Select(Pred);
  T1->SelectAtomicStrConst("Size", "big", SUPERSTR);
  T2->SelectAtomicStrConst("Size", "big", EQ);
  //PTable Tj = T1->Join("Location", *T2, "Location");
  PTable Tj = T1->Join("Location", *T1, "Location");
  Tj->SaveSS("tests/animals_out_Tj_raw.txt");
  TStrV GroupBy;
  GroupBy.Add("Animals_1.Animal");
  GroupBy.Add("Animals_2.Animal");
  Tj->Unique(GroupBy, false);
  //Tj->SetSrcCol("Animals_1.Animal");
  //Tj->SetDstCol("Animals_2.Animal");
  PNEANet G = TSnap::ToNetwork<PNEANet>(Tj, TStr("Animals_1.Animal"), TStr("Animals_2.Animal"), aaLast);
  //print table
  Tj->SaveSS("tests/animals_out_Tj_1.txt");
  G->Dump();

  // Join on Location to get animal pairs
  // select the animal pairs of animals of the same size
  // group by (Animal, Animal)
  // unique by group idx

  PTable T3 = TTable::LoadSS("Animals", AnimalS, "tests/animals.txt", Context, '\t', false);
  PTable To = T3->Join("Location", *T3, "Location");
  TAtomicPredicate A2(atStr, false, EQ, "Animals_1.Size", "Animals_2.Size");  
  TPredicateNode N2(A2);
  TPredicate Pred2(&N2);
  To->Select(Pred2);
  TStrV GroupBy1;
  GroupBy1.Add("Animals_1.Animal");
  GroupBy1.Add("Animals_2.Animal");
  To->Unique(GroupBy1, false);
  //print table
  To->SaveSS("tests/animals_out_To_1.txt");
  return 0;
}
