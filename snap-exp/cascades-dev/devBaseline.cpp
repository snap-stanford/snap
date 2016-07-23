#include "stdafx.h"
#include <time.h>
/**
 * Baseline algorithm for finding all cascades.
 * Input : Filename, W
 */
int main(int argc,char* argv[]) {
  TTableContext Context;
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  PTable P1 = TTable::LoadSS(TimeS,argv[1],&Context,' ');
  TInt SIdx = P1->GetColIdx("Source");
  TInt DIdx = P1->GetColIdx("Dest");
  TInt StIdx = P1->GetColIdx("Start");
  TInt DuIdx = P1->GetColIdx("Duration");
  int totalCas = 0;
  int numCas = 0;
  int W = atoi(argv[2]);
  int max = 0;
  clock_t st,et;
  st = clock();
  for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
    TInt RIdx = RI.GetRowIdx();
    THashSet<TInt> VisitedH;
    TSnapQueue<TInt> EventQ;
    EventQ.Push(RIdx);
    VisitedH.AddKey(RIdx);
    //Start the BFS logic
    while (!EventQ.Empty()) {
    TInt CIdx = EventQ.Top();
    EventQ.Pop();
    int CDest = P1->GetIntValAtRowIdx(DIdx,CIdx).Val;
    int CStart = P1->GetIntValAtRowIdx(StIdx,CIdx).Val;
    int CDur = P1->GetIntValAtRowIdx(DuIdx,CIdx).Val;
      for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
        TInt PId = RI.GetRowIdx();
        if (! VisitedH.IsKey(RI.GetRowIdx())) {
          int TSource = P1->GetIntValAtRowIdx(SIdx,PId).Val;
          int TStart = P1->GetIntValAtRowIdx(StIdx,PId).Val;
          if (TSource == CDest && TStart >= (CDur + CStart) && (TStart - (CDur + CStart) <= W)) {
            VisitedH.AddKey(RI.GetRowIdx());
            EventQ.Push(RI.GetRowIdx());
          }
        }
      } 
    }
    if (VisitedH.Len() > 1) {
      totalCas += VisitedH.Len();
      numCas += 1;
      if (VisitedH.Len() > max) {
        max = VisitedH.Len();
      }
    }
  }
  et = clock();
  float diff = ((float)et - (float)st)/CLOCKS_PER_SEC;
  printf("total: %d,numCas: %d,max: %d,time is : %f\n",totalCas,numCas,max,diff);
  return 0;
}
