#include "Snap.h"
#include <cstring>
#include <ctime>

int main(int argc, char** argv){
  TTableContext Context;

  //char filename[500] = "/dfs/ilfs2/0/ringo/benchmarks/soc-LiveJournal1.table";
  char filename[500] = "/dfs/ilfs2/0/ringo/benchmarks/twitter_rv.table";

  if (argc >= 2){
    strcpy(filename,argv[1]);
  }
  struct timeval start, end;
  float delta;
  TTmProfiler Profiler;
  int TimerId = Profiler.AddTimer("Profiler");

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TFIn FIn(filename);
  PTable Q = TTable::Load(FIn, Context);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  
  TVec<TPair<TStr, TAttrType> > Schema = Q->GetSchema();

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PUNGraph G1 = TSnap::ToGraph<PUNGraph>(Q, Schema[0].GetVal1(), Schema[1].GetVal1(), aaFirst);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;

  printf("ToGraphUndirected time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PNGraph G2 = TSnap::ToGraph<PNGraph>(Q, Schema[0].GetVal1(), Schema[1].GetVal1(), aaFirst);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("ToGraphDirected time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PNEANet G3 = TSnap::ToNetwork<PNEANet>(Q, Schema[0].GetVal1(), Schema[1].GetVal1(), aaFirst);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("ToGraph time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PNGraphMP G4 = TSnap::ToGraphMP<PNGraphMP>(Q, Schema[0].GetVal1(), Schema[1].GetVal1());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;

  printf("ToPNGraphMP time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  return 0;
}
