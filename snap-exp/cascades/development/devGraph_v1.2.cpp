#include "stdafx.h"
/*
 * Version 1.2 : Graph based algorithm for finding cascasdes.
 * This version only finds the top cascades. Uses an inline 
 * modified binary search.
*/

int main(int argc,char* argv[]) {
  TTableContext Context;
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  PTable P1 = TTable::LoadSS(TimeS,"./../../../../datasets/temporal/outsms-A.txt",&Context,' ');
  TIntV MapV;
  TStrV SortBy;
  SortBy.Add("Start");
  P1->Order(SortBy);
  TIntV Start; // Sorted vec of start time
  P1->ReadIntCol("Start",Start);
  // Attribute to Int mapping
  TInt SIdx = P1->GetColIdx("Source");
  TInt DIdx = P1->GetColIdx("Dest");
  TInt StIdx = P1->GetColIdx("Start");
  TInt DuIdx = P1->GetColIdx("Duration");
  int totalCas = 0;
  int numCas = 0;
  int W = atoi(argv[1]);
  int max = 0;
  clock_t st,et,gt;
  st = clock();
  // Generate a Directed Graph
  PNGraph Graph = TNGraph::New();
  // Create all the nodes in the graph and also create the Map
  // The iterator goes over the table in sorted order
  for (TRowIterator RI = P1->BegRI(); RI < P1-> EndRI(); RI++) {
    Graph->AddNode(RI.GetRowIdx().Val);
    MapV.Add(RI.GetRowIdx());
  }
  for (TRowIterator OI = P1->BegRI(); OI < P1->EndRI(); OI++) {
    int OIdx = OI.GetRowIdx().Val;
    int ODest = P1->GetIntValAtRowIdx(DIdx,OIdx).Val;
    int OStart = P1->GetIntValAtRowIdx(StIdx,OIdx).Val;
    int ODur = P1->GetIntValAtRowIdx(DuIdx,OIdx).Val;
    // Inline binary Search
    int val = OStart + ODur;
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
    // End of binary Search
    int BIdx = index;
    for(int i = BIdx; i < Start.Len(); i++) {
      int InIdx = MapV.GetVal(i).Val;
      if (InIdx == OIdx) {continue;}
      int InSource = P1->GetIntValAtRowIdx(SIdx,InIdx).Val;
      int InStart = P1->GetIntValAtRowIdx(StIdx,InIdx).Val;
      if (InStart - (ODur + OStart) > W) { break;}
      if (InSource == ODest && InStart >= (ODur + OStart)) {
        if (!Graph->IsEdge(OIdx,InIdx)) {
          Graph->AddEdge(OIdx,InIdx);
        }
      }      
    }
  }
  gt = clock();
  float diffG = ((float)gt - (float)st)/CLOCKS_PER_SEC; 
  printf("graph Gen: %f,Edges: %d \n",diffG,Graph->GetEdges());
  // Start Cascasde detection
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    // Check for top cascades
    if (NI.GetInDeg() != 0) { continue;}
    TSnapQueue<TInt> EventQ;
    THashSet<TInt> VisitedH;
    EventQ.Push(NI.GetId());
    VisitedH.AddKey(NI.GetId());
    while (! EventQ.Empty()) {
      TNGraph::TNodeI CNI = Graph->GetNI(EventQ.Top().Val); //Get Current Node
      EventQ.Pop();
      // Go over the outdegree nodes of the currernt node
      for (int e = 0; e < CNI.GetOutDeg(); e++) {
        if ( !VisitedH.IsKey(CNI.GetOutNId(e))) {
          EventQ.Push(CNI.GetOutNId(e));
          VisitedH.AddKey(CNI.GetOutNId(e));
        }
      }
    }
    if (VisitedH.Len() > 1) { numCas++; totalCas += VisitedH.Len();}
    if (VisitedH.Len() > max) { max = VisitedH.Len();}
  }
  et = clock();
  float diff = ((float)et - (float)st)/CLOCKS_PER_SEC;
  printf("Total: %d,numCas: %d,max: %d,graph Gen: %f,total time: %f,Edges: %d\n",totalCas,numCas,max,diffG,diff,Graph->GetEdges());
  return 0;
}
