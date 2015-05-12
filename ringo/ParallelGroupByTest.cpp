#include "Snap.h"
/*
A simple test for parallel GroupBy
*/

int main(int argc, char** argv){
  TStr infile = "/lfs/madmax2/0/yonathan/soc-LiveJournal1-noheader.txt";
  Schema S;
  S.Add(TPair<TStr,TAttrType>("src", atInt));
  S.Add(TPair<TStr,TAttrType>("dst", atInt));
  TTableContext Context;
  TTable::SetMP(true);
	
  PTable Edges = TTable::LoadSS(S, infile, Context);
	
  THashMP<TInt, TIntV> Grouping(5000000);
  Edges->GroupByIntColMP("src", Grouping, true);
  return 0;
}
	