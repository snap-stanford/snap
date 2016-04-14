#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

PSOut StdOut = TStdOut::New();

void test_ints() {

  TTableContext context;
  Schema schema;
  schema.Add(TPair<TStr,TAttrType>("src",atInt));
  schema.Add(TPair<TStr,TAttrType>("dst",atInt));
  TStr wikifilename = "/dfs/scratch0/viswa/wiki_Vote.txt";

  PTable wikitable = TTable::LoadSS(schema, wikifilename, &context, '\t', TBool(false));
  printf("Int: Before indexing ...\n");

  TInt rowidx1, rowidx2;
  double delta1, delta2;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetIntRowIdxByVal("src", 8229);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetIntRowIdxByVal("src", 8274);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\n", delta1, delta2);
  printf("%d %d\n", rowidx1, rowidx2);

  wikitable->RequestIndexInt("src");
  printf("Int: After indexing ...\n");
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetIntRowIdxByVal("src", 8229);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetIntRowIdxByVal("src", 8274);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\n", delta1, delta2);
  printf("%d %d\n", rowidx1, rowidx2);


  /*
  int i = 0;
  for (TRowIterator RowI = wikitable->BegRI(); RowI < wikitable->EndRI(); RowI++) {
    if (i > 100000)
    printf("%d %d %d\n", RowI.GetRowIdx(), RowI.GetIntAttr("src"), RowI.GetIntAttr("dst"));
    i++;
//   if (i > 10) break;
  }
  */

}

void test_strs() {

  TTableContext context;
  Schema schema;
  schema.Add(TPair<TStr,TAttrType>("src",atStr));
  schema.Add(TPair<TStr,TAttrType>("dst",atStr));
  TStr wikifilename = "/dfs/scratch0/viswa/wiki_Vote.txt";

  PTable wikitable = TTable::LoadSS(schema, wikifilename, &context, '\t', TBool(false));

  TInt rowidx1, rowidx2;
  double delta1, delta2;
  struct timeval start, end;
  printf("Str: Before indexing ...\n");
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetStrRowIdxByMap("src", 7096);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetStrRowIdxByMap("src", 7114);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\n", delta1, delta2);
  printf("%d %d\n", rowidx1, rowidx2);

  wikitable->RequestIndexStrMap("src");
  printf("Str: After indexing ...\n");
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetStrRowIdxByMap("src", 7096);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetStrRowIdxByMap("src", 7114);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\n", delta1, delta2);
  printf("%d %d\n", rowidx1, rowidx2);

  /*
  int i = 0;
  for (TRowIterator RowI = wikitable->BegRI(); RowI < wikitable->EndRI(); RowI++) {
    if (i > 100000)
    printf("%d %d %d\n", RowI.GetRowIdx(), RowI.GetStrMapByName("src"), RowI.GetStrMapByName("dst"));
    i++;
//    if (i > 10) break;
  }
  */
}


int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");
  test_ints();
  test_strs();
  return 0;
}
