#include "Predicate.h"
#include <iostream>
using namespace std;
/*
int main(){
  // T1 = (x > y)
  TPredicate::TAtomicPredicate A1(TPredicate::INT, false, TPredicate::GT, "x", "y");
  TPredicate::TPredicateNode N1(A1);
  TPredicate T1(&N1);
  T1.SetIntVal("x", 1);
  T1.SetIntVal("y", 2);
  TBool R1 = T1.Eval();
  cout << "1 > 2 " << R1 << endl;
  T1.SetIntVal("x", 2);
  T1.SetIntVal("y", 1);
  TBool R1_1 = T1.Eval();
  cout << "2 > 1 " << R1_1 << endl;
  // T2 = (x > y) && (z > 5)
  TPredicate::TAtomicPredicate A2(TPredicate::INT, true, TPredicate::GT, "z", "", 5, 0, "");
  TPredicate::TPredicateNode N2(A2);
  TPredicate::TPredicateNode N3(TPredicate::AND);
  N3.AddLeftChild(&N1);
  N3.AddRightChild(&N2);
  cout << &N3 << " " << N3.Left << endl;
  cout << &N1 << " " << N1.Parent << endl;
  TPredicate T2(&N3);
  T2.SetIntVal("x", 2);
  T2.SetIntVal("y", 1);
  T2.SetIntVal("z", 4);
  TBool R2 = T2.Eval();
  cout << "(2 > 1) && (4 > 5) " << R2 << endl;
  T2.SetIntVal("z", 6);
  TBool R2_1 = T2.Eval();
  cout << "(2 > 1) && (6 > 5) " << R2_1 << endl;
  // T3 = (x > y) || (z > 5)
  TPredicate::TPredicateNode N4(TPredicate::OR);
  N4.AddLeftChild(&N1);
  N4.AddRightChild(&N2);
  TPredicate T3(&N4);
  T3.SetIntVal("x", 2);
  T3.SetIntVal("y", 1);
  T3.SetIntVal("z", 4);
  TBool R3 = T3.Eval();
  cout << "(2 > 1) || (4 > 5) " << R3 << endl;
  // !((x == "bla") ^ (y > z)) || z <= 3
  TPredicate::TAtomicPredicate A3(atStr, true, TPredicate::EQ, "x", "", 0, 0, "bla");
  TPredicate::TAtomicPredicate A4(TPredicate::INT, false, TPredicate::GT, "y", "z", 0, 0, "");
  TPredicate::TAtomicPredicate A5(TPredicate::INT, true, TPredicate::LTE, "z", "", 3, 0, "");
  TPredicate::TPredicateNode N5(A3);
  TPredicate::TPredicateNode N6(A4);
  TPredicate::TPredicateNode N7(TPredicate::AND);
  N7.AddLeftChild(&N5);
  N7.AddRightChild(&N6);
  TPredicate::TPredicateNode N8(TPredicate::NOT);
  N8.AddLeftChild(&N7);
  TPredicate::TPredicateNode N9(A5);
  TPredicate::TPredicateNode N10(TPredicate::OR);
  N10.AddLeftChild(&N8);
  N10.AddRightChild(&N9);
  TPredicate T4(&N10);
  T4.SetStrVal("x", "bla");
  T4.SetIntVal("y", 2);
  T4.SetIntVal("z", 4);
  TBool R4 = T4.Eval();
  cout << "!((bla == bla) ^ (2 > 4)) || 4 <= 3 " << R4 << endl;
  T4.SetIntVal("y", 5);
  TBool R4_1 = T4.Eval();
  cout << "!((bla == bla) ^ (5 > 4)) || 4 <= 3 " << R4_1 << endl;
	return 0;
}
*/
