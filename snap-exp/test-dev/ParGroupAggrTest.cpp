#include "Snap.h"
#include <cstring>
#include <sys/time.h>

int main(int argc, char** argv){
  TTableContext Context;

  if (argc < 2){
    printf("Usage: %s <srcfile>\n", argv[0]);
  }

  char* srcfile = argv[1];
  
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  // create schema
  Schema S1;
  S1.Add(TPair<TStr,TAttrType>("Id", atInt));
  S1.Add(TPair<TStr,TAttrType>("OwnerUserId", atInt));
  S1.Add(TPair<TStr,TAttrType>("AcceptedAnswerId", atInt));
  S1.Add(TPair<TStr,TAttrType>("CreationDate", atStr));
  S1.Add(TPair<TStr,TAttrType>("Score", atInt));
  TIntV RelevantCols;
  PTable T1 = TTable::LoadSS("1", S1, srcfile, Context, RelevantCols);

  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Load time: %f\n", t2-t1);

  TStrV V;
  V.Add("OwnerUserId");
  T1->Aggregate(V, aaSum, "Score", "Sum");
  //T1->SelectAtomicIntConst("Score", 5, GT);

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);

  printf("Aggregate time: %f\n", t3-t2);
  return 0;
}
