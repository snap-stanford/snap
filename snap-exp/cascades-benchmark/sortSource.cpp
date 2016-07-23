#include "stdafx.h"

/**
 * Used for benchmarking sorting by source algorithm.
 * Takes as input starting point of
 * a top cascade and outputs time taken for casacade detection. 
 * Input : Source, Dest, Start, Duration 
 * Output : Prints the time for cascade detection
 */
int main(int argc,char* argv[]) {
  TTableContext Context;
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  PTable P1 = TTable::LoadSS(TimeS,"./../../../../datasets/temporal/yemen_call_201001.txt",&Context,' ');
  TIntV MapV;
  TStrV SortBy;
  SortBy.Add("Source");
  P1->Order(SortBy);
  TIntV Source; // Sorted vec of start time
  P1->ReadIntCol("Source",Source);
  for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
    MapV.Add(RI.GetRowIdx());
  }
  // Attribute to Int mapping
  TInt SIdx = P1->GetColIdx("Source");
  TInt DIdx = P1->GetColIdx("Dest");
  TInt StIdx = P1->GetColIdx("Start");
  TInt DuIdx = P1->GetColIdx("Duration");
  int W = atoi(argv[1]);
  int len = 0;
  // Find the starting point
  int TSource = atoi(argv[2]);
  int TDest = atoi(argv[3]);
  int TStart = atoi(argv[4]);
  int TDur = atoi(argv[5]);
  TInt RIdx;
  for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
    RIdx = RI.GetRowIdx();
    int RSource = P1->GetIntValAtRowIdx(SIdx,RIdx).Val;
    int RDest = P1->GetIntValAtRowIdx(DIdx,RIdx).Val;
    int RStart = P1->GetIntValAtRowIdx(StIdx,RIdx).Val;
    int RDur = P1->GetIntValAtRowIdx(DuIdx,RIdx).Val;
    if (TSource == RSource && TDest == RDest && TStart == RStart && TDur == RDur) break;
  }
  // Start building the cascade from the start point
  clock_t st,et;
  st = clock();
  for (int i = 0; i < 1; i++) {
    THashSet<TInt> VisitedH;
    TSnapQueue<TInt> EventQ;
    EventQ.Push(RIdx);
    VisitedH.AddKey(RIdx);
    while (!EventQ.Empty()) {
      TInt CIdx = EventQ.Top();
      EventQ.Pop();
      int CDest = P1->GetIntValAtRowIdx(DIdx,CIdx).Val;
      int CStart = P1->GetIntValAtRowIdx(StIdx,CIdx).Val;
      int CDur = P1->GetIntValAtRowIdx(DuIdx,CIdx).Val;
      // In line binary search
      int val = CDest;
      int lo = 0;
      int hi = Source.Len() - 1;
      int index = -1;
      while (hi >= lo) {
        int mid = lo + (hi - lo)/2;
        if (Source.GetVal(mid) > val) { hi = mid - 1;}
        else if (Source.GetVal(mid) < val) { lo = mid + 1;}
        else { index = mid; hi = mid - 1;}
      } 
      // End of binary search
      int BIdx = index;
      for(int i = BIdx; i < Source.Len(); i++) {
        int PId = MapV.GetVal(i).Val;
        if (! VisitedH.IsKey(PId)) {
          int TSource = P1->GetIntValAtRowIdx(SIdx,PId).Val;
          int TStart = P1->GetIntValAtRowIdx(StIdx,PId).Val;
          if (TSource != CDest) {
            break;
          }
          if (TStart >= (CDur + CStart) && TStart - (CDur + CStart) <= W) {
            VisitedH.AddKey(PId);
            EventQ.Push(PId);
          }
        }
      }
    }
    len = VisitedH.Len();
  }
  et = clock();
  float diff = ((float) et - (float) st)/CLOCKS_PER_SEC;
  printf("Size %d,Time %f\n",len,diff);
  return 0;
}
