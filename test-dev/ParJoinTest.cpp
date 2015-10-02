#include "Snap.h"
#include <cstring>
#include <sys/time.h>

int main(int argc, char** argv){
  TTableContext Context;

  if (argc < 3){
    printf("Usage: %s <srcfile1> <srcfile2>\n", argv[0]);
  }

  char* srcfile1 = argv[1];
  char* srcfile2 = argv[2];
  
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  TFIn SIn(srcfile1);
  PTable T1 = TTable::Load(SIn, Context);
  
  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Load1 time: %f\n", t2-t1);

  // create schema
  Schema S2;
  S2.Add(TPair<TStr,TAttrType>("Index", atInt));
  TIntV RelevantCols2;
  PTable T2 = TTable::LoadSS("2", S2, srcfile2, Context, RelevantCols2);

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Load2 time: %f\n", t3-t2);

  PTable P = T1->Join("Src", T2, "Index");
  gettimeofday(&tim, NULL);
  double t4=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Join time: %f\n", t4-t3);
  return 0;
}
