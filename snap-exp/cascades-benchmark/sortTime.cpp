#include "stdafx.h"
/**
 * Used for benchmarking the baseling algorithm in 
 * file devSort_v1.4.cpp. Takes as input starting point of
 * a top cascade and outputs time taken for casacade detection. 
 * Input : Source, Dest, Start, Duration 
 * Output : Prints the time for cascade detection
 */

int main(int argc,char* argv[]) {
  TTableContext Context;
  //Input File Format Source,Dest,Start_Time,Duration
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  PTable P1 = TTable::LoadSS(TimeS,"./../../../../datasets/temporal/yemen_call_201001.txt",&Context,' ');
  TIntV MapV;
  TStrV SortBy;
  SortBy.Add("Start");
  P1->Order(SortBy);
  TIntV Start; // Sorted vec of start time
  P1->ReadIntCol("Start",Start);
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
      int val = CStart + CDur;
      int lo = 0;
      int hi = Start.Len() - 1;
      int index = -1;
      if (val >= Start.GetVal(hi)) { index =  hi;}
      else {
        while (hi >= lo) {
          int mid = lo + (hi - lo)/2;
          if (Start.GetVal(mid) > val) {
            if (mid - 1 >= lo and Start.GetVal(mid - 1) < val) { index = mid - 1;break;}
            hi = mid - 1;
          }
          else if (Start.GetVal(mid) < val) {
            if (mid + 1 <= hi and Start.GetVal(mid + 1) > val) { index = mid;break;}
            lo = mid + 1;
          }
          else { index = mid; hi = mid - 1;}
        }
      }   
      // End of binary search
      int BIdx = index;
      for(int i = BIdx; i < Start.Len(); i++) {
        int PId = MapV.GetVal(i).Val;
        if (! VisitedH.IsKey(PId)) {
          int TSource = P1->GetIntValAtRowIdx(SIdx,PId).Val;
          int TStart = P1->GetIntValAtRowIdx(StIdx,PId).Val;
          if (TStart - (CDur + CStart) > W) {
            break;
          }
          if (TSource == CDest && TStart >= (CDur + CStart)) {
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
