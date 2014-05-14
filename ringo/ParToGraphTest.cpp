#include "Snap.h"
#include <cstring>
#include <sys/time.h>

int main(int argc, char** argv){
  TTableContext Context;

  if (argc < 2){
    printf("Usage: %s <srcfile>\n", argv[0]);
  }

  TIntV Key;
  TIntV Val;
  Key.Add(5);
  Key.Add(3);
  Key.Add(2);
  Key.Add(2);
  Key.Add(1);
  Val.Add(3);
  Val.Add(3);
  Val.Add(3);
  Val.Add(2);
  Val.Add(3);

  TTable::ISortKeyVal(Key, Val, 0, 4);
  printf("Result:\n");
  for (int i = 0; i < 5; i++) printf("%d %d, ", Key[i].Val, Val[i].Val);
  printf("\n");

  char* srcfile = argv[1];

  // TInt Res = TTable::CompareKeyVal(18, 2602679, 87, 1879175) ? 1 : 0;
  // printf("Res: %d\n", Res.Val);
  // return 0;
  
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  TFIn FIn(srcfile);
  PTable T1 = TTable::Load(FIn, Context);

  // Schema LJS;
  // LJS.Add(TPair<TStr,TAttrType>("Src", atInt));
  // LJS.Add(TPair<TStr,TAttrType>("Dst", atInt));
  // TIntV RelevantCols; RelevantCols.Add(0); RelevantCols.Add(1);
  // PTable T1 = TTable::LoadSS("1", LJS, srcfile, Context, RelevantCols);

  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Load time: %f\n", t2-t1);

  TVec<TPair<TStr, TAttrType> > S = T1->GetSchema();
  PNGraphMP Graph = TSnap::ToGraphMP2<PNGraphMP>(T1, S[0].GetVal1(), S[1].GetVal1());

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);

  printf("ToGraphMP2 time: %f\n", t3-t2);
  return 0;
}
