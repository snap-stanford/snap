#include "stdafx.h"
/**
 * Used for benchmarking the baseling algorithm in 
 * file baseline.cpp. Takes as input W and starting 
 * point of a top cascade and outputs time taken for 
 * casacade detection. 
 * Input : W, Source, Dest, Start, Duration 
 * Output : Prints the time for cascade detection
 */
int main(int argc,char* argv[]) {
  //Read input from file and store in table.
  TTableContext Context;
  //Input File Format Source,Dest,Start_Time,Duration
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  // Dataset path
  PTable P1 = TTable::LoadSS(TimeS,"./../../../../datasets/temporal/yemen_call_201001.txt",&Context,' ');
  TInt SIdx = P1->GetColIdx("Source");
  TInt DIdx = P1->GetColIdx("Dest");
  TInt StIdx = P1->GetColIdx("Start");
  TInt DuIdx = P1->GetColIdx("Duration");
  int W = atoi(argv[1]);
  int TSource = atoi(argv[2]);
  int TDest = atoi(argv[3]);
  int TStart = atoi(argv[4]);
  int TDur = atoi(argv[5]);
  TSnapQueue<TInt> EventQ;
  //After completion VisitedH will contain all the connected events
  THashSet<TInt> VisitedH;
  TInt RIdx;
  for (TRowIterator RI = P1->BegRI(); RI < P1->EndRI(); RI++) {
    RIdx = RI.GetRowIdx();
    int RSource = P1->GetIntValAtRowIdx(SIdx,RIdx).Val;
    int RDest = P1->GetIntValAtRowIdx(DIdx,RIdx).Val;
    int RStart = P1->GetIntValAtRowIdx(StIdx,RIdx).Val;
    int RDur = P1->GetIntValAtRowIdx(DuIdx,RIdx).Val;
    if (TSource == RSource && TDest == RDest && TStart == RStart && TDur == RDur) {
      EventQ.Push(RIdx);
      VisitedH.AddKey(RIdx);
    }
  } 
  //Start the BFS logic
  clock_t st,et;
  st = clock();
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
  et = clock();
  float diff = ((float) et - (float) st)/CLOCKS_PER_SEC;
  printf("size %d,Time %f\n",VisitedH.Len(),diff);
  return 0;
}
