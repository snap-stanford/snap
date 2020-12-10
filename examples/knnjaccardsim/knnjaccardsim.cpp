#include "stdafx.h"
#include "Snap.h"
#include<iostream>
using namespace std;

const std::string currentDateTime() {
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
  // for more information about date/time format
  strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

  return buf;
}

int main(int argc,char* argv[]) {
  TTableContext Context;
  //Create schema
  //Input File Format Source,Dest,Start_Time,Duration
  Schema TimeS;
  TimeS.Add(TPair<TStr,TAttrType>("Source",atInt));
  TimeS.Add(TPair<TStr,TAttrType>("Dest",atInt));
  PTable P = TTable::LoadSS(TimeS,argv[1],&Context,' ');
  int K = atoi(argv[2]);
  cerr<<"Table Loaded "<<currentDateTime()<<endl;
  PNGraph G = GetBiGraph(P, 0, 1);
  cerr<<"Graph Generated "<<currentDateTime()<<endl;
  #ifdef GCC_ATOMIC
  PNEANet KNN =  KNNJaccardParallel(G, K);

  for (TNEANet::TEdgeI EI = KNN->BegEI(); EI < KNN->EndEI(); EI++ ){
    cout<<EI.GetSrcNId()<<" "<<EI.GetDstNId()<<" "<<KNN->GetFltAttrDatE(EI.GetId(), "sim")<<endl;
    }
  #endif
  return 0;
}

