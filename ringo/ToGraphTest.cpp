#include "Snap.h"
#include <cstring>
#include <ctime>

int main(int argc, char** argv){
  TTableContext Context;

  char filename[500] = "/dfs/ilfs2/0/ringo/benchmarks/soc-LiveJournal1.table";
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
  Q->SetSrcCol(Schema[0].GetVal1());
  Q->SetDstCol(Schema[1].GetVal1());

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PUNGraph G1 = Q->ToGraphUndirected(aaFirst);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("ToGraphUndirected time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PNGraph G2 = Q->ToGraphDirected(aaFirst);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("ToGraphDirected time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PNEANet G3 = Q->ToGraph(aaFirst);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("ToGraph time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  return 0;
}
