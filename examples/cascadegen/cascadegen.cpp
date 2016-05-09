#include "stdafx.h"
#include "Snap.h"
int main(int argc,char* argv[]) {
  //Read input from file and store in table.
  TTableContext Context;
  //Create schema
  //Input File Format Source,Dest,Start_Time,Duration
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Start",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Duration",atInt));
  PTable P = TTable::LoadSS(TimeS,argv[1],&Context,' ');
  TInt W = atoi(argv[2]);
  // Sort by Source
  PNGraph Graph = TSnap::CascGraph(P,"Source","Dest","Start","Duration",W);
  // Save the graph 
  TFOut FOut("cascade.graph");
  Graph->Save(FOut);
  // Sequential
  TVec<TIntV> TopCascVV;
  TSnap::CascFind(Graph,P,"Source","Dest","Start","Duration",TopCascVV,false);
  // Print statistics about top cascasdes
  int max = 0;
  int totalEvents = 0;
  for (int i = 0; i < TopCascVV.Len(); i++) {
    if (TopCascVV[i].Len() > max) { max = TopCascVV[i].Len();}
    totalEvents += TopCascVV[i].Len();
  } 
  printf("TotalCasc %d, TotalEvents %d, max %d\n",TopCascVV.Len(),totalEvents,max);
  return 0;
}
