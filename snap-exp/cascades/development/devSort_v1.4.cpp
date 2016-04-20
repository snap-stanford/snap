#include "stdafx.h"

/*
 * Version 1.4 : Time based sorting of events.
 * This version only finds the top cascades. Has in 
 * line binary search. 
 * Input : filename, W
 */

int main(int argc,char* argv[]) {
  TTableContext Context;
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  PTable P1 = TTable::LoadSS(TimeS,argv[1],&Context,' ');
  TIntV TopCascadeV;
  TIntV MapV;
  TStrV SortBy;
  SortBy.Add("Start");
  P1->Order(SortBy);
  TIntV Start; // Sorted vec of start time
  P1->ReadIntCol("Start",Start);
  for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
    TopCascadeV.Add(1);
    MapV.Add(RI.GetRowIdx());
  }
  // Attribute to Int mapping
  TInt SIdx = P1->GetColIdx("Source");
  TInt DIdx = P1->GetColIdx("Dest");
  TInt StIdx = P1->GetColIdx("Start");
  TInt DuIdx = P1->GetColIdx("Duration");
  int totalCas = 0;
  int numCas = 0;
  int W = atoi(argv[2]);
  int max = 0;
  clock_t st,et;
  //Start the BFS logic
  st = clock();
  for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
    TInt RIdx = RI.GetRowIdx();
    if (TopCascadeV.GetVal(RIdx) == 0) { continue;}
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
      if (val >= Start.GetVal(hi)) { val = Start.GetVal(hi);}
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
            TopCascadeV.SetVal(PId,0);
            EventQ.Push(PId);
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
  printf("Total: %d,numCas: %d,max: %d,time is: %f\n",totalCas,numCas,max,diff);
  return 0;
}
