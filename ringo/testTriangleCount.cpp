// Testing Triangle Count function

#include "../snap-core/Snap.h"

int main(int argc, char* argv[]) {
  TTableContext Context;

  char filename[500] = "/dfs/ilfs2/0/ringo/benchmarks/soc-LiveJournal1.graph";
  //char filename[500] = "/dfs/ilfs2/0/ringo/benchmarks/twitter_rv.graph";

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
  PNGraph Q = TNGraph::Load(FIn);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  
  PNGraph Graph = Q;

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  int64 cnt = TSnap::CountTriangles(Graph);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  printf("%s\n", TUInt64::GetStr(cnt).CStr());
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Count Triangles time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  return 0;
}
