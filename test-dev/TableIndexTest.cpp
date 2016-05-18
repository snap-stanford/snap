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

  TIntV rowidx1, rowidx2, rowidx3;
  double delta1, delta2, delta3;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetIntRowIdxByVal("src", 8229);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetIntRowIdxByVal("src", 8274);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx3 = wikitable->GetIntRowIdxByVal("src", 30);
  gettimeofday(&end, NULL);
  delta3 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\t Time 3: %f\n", delta1, delta2, delta3);
  printf("%d %d\n", rowidx1[0], rowidx2[0]);

  printf("Row Idx3:");
  for (int i = 0 ; i < rowidx3.Len() ; i++ ) {
    printf("%d ", rowidx3[i]);
  }
  printf("\n");

  gettimeofday(&start, NULL);
  wikitable->RequestIndexInt("src");
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;
  printf("Int: Time to index: %f\n", delta1);

  printf("Int: After indexing ...\n");
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetIntRowIdxByVal("src", 8229);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetIntRowIdxByVal("src", 8274);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx3 = wikitable->GetIntRowIdxByVal("src", 30);
  gettimeofday(&end, NULL);
  delta3 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\t Time 3: %f\n", delta1, delta2, delta3);
  printf("%d %d\n", rowidx1[0], rowidx2[0]);

  printf("Row Idx3:");
  for (int i = 0 ; i < rowidx3.Len() ; i++ ) {
    printf("%d ", rowidx3[i]);
  }
  printf("\n");


  printf("Time 1: %f\tTime 2: %f\n", delta1, delta2);
  printf("%d %d\n", rowidx1[0], rowidx2[0]);


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

  TIntV rowidx1, rowidx2, rowidx3;
  double delta1, delta2, delta3;
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

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx3 = wikitable->GetStrRowIdxByMap("src", 0);
  gettimeofday(&end, NULL);
  delta3 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;


  printf("Time 1: %f\tTime 2: %f\t Time 3: %f\n", delta1, delta2, delta3);
  printf("%d %d\n", rowidx1[0], rowidx2[0]);

  printf("Row Idx3: ");
  for (int i = 0 ; i < rowidx3.Len() ; i++ ) {
    printf("%d ", rowidx3[i]);
  }
  printf("\n");

  gettimeofday(&start, NULL);
  wikitable->RequestIndexStrMap("src");
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;
  printf("Str: Time to index: %f\n", delta1);


  printf("Str: After indexing ...\n");
  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx1 = wikitable->GetStrRowIdxByMap("src", 7096);
  gettimeofday(&end, NULL);
  delta1 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx2 = wikitable->GetStrRowIdxByMap("src", 7114);
  gettimeofday(&end, NULL);
  delta2 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  gettimeofday(&start, NULL);
  for (int i = 0 ; i < 100; i++) rowidx3 = wikitable->GetStrRowIdxByMap("src", 0);
  gettimeofday(&end, NULL);
  delta3 = ((end.tv_sec - start.tv_sec)* 1000000u + end.tv_usec - start.tv_usec)/1.e6;

  printf("Time 1: %f\tTime 2: %f\t Time 3: %f\n", delta1, delta2, delta3);
  printf("%d %d\n", rowidx1[0], rowidx2[0]);

  printf("Row Idx3: ");
  for (int i = 0 ; i < rowidx3.Len() ; i++ ) {
    printf("%d ", rowidx3[i]);
  }
  printf("\n");

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
