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
  
  TFIn SIn(srcfile);
  PTable T1 = TTable::Load(SIn, Context);

  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Load time: %f\n", t2-t1);

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);

  T1->SelectAtomicIntConst("Src", 10000, GT);
  gettimeofday(&tim, NULL);
  double t4=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Select time: %f\n", t4-t3);
  return 0;
}
