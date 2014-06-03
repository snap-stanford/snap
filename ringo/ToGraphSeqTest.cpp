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

  Schema S;
  S.Add(TPair<TStr,TAttrType>("Id", atInt));
  S.Add(TPair<TStr,TAttrType>("OwnerUserId", atInt));
  S.Add(TPair<TStr,TAttrType>("AcceptedAnswerId", atInt));
  S.Add(TPair<TStr,TAttrType>("CreationDate", atInt));
  S.Add(TPair<TStr,TAttrType>("Score", atInt));
  PTable T1 = TTable::LoadSS(S, srcfile, Context);

  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Load time: %f\n", t2-t1);

  PTable T2 = T1->Join("AcceptedAnswerId", T1, "Id");

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Join time: %f\n", t3-t2);

  TInt WINDOW_SIZE = 2592000;
  T2->SetSrcCol("OwnerUserId-1");
  T2->SetDstCol("OwnerUserId-2");
  TVec<PNEANet> Graph = T2->ToGraphSequence("CreationDate", aaFirst, WINDOW_SIZE, WINDOW_SIZE);

  gettimeofday(&tim, NULL);
  double t4=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("ToGraphSequence time: %f\n", t4-t3);
  return 0;
}
