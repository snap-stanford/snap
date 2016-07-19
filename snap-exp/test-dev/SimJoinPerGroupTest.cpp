#include "Snap.h"

int main(){
  TTableContext Context;
  // create schema
  Schema SimJoinPerGroupS;
  SimJoinPerGroupS.Add(TPair<TStr,TAttrType>("UserId", atStr));
  SimJoinPerGroupS.Add(TPair<TStr,TAttrType>("ProductId", atStr));

  // create table
  PTable T = TTable::LoadSS("Products", SimJoinPerGroupS, "tests/products.txt", Context, '\t', true);

	TStrV GroupBy;
	GroupBy.Add("UserId");
	PTable JointTable = T->SelfSimJoinPerGroup(GroupBy, "ProductId", "Jaccard_Distance", Jaccard, 0.5);

	JointTable->SelectAtomic("Products_1.UserId", "Products_2.UserId", NEQ);
	JointTable->SaveSS("tests/products.out.txt");
	printf("Saved products.out.txt\n");
  return 0;
}
