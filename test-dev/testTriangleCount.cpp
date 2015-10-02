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
  struct timeval startall, endall;
  float delta;
  TTmProfiler Profiler;
  int TimerId = Profiler.AddTimer("Profiler");
  int TimerAll = Profiler.AddTimer("ProfilerAll");

  Profiler.ResetTimer(TimerAll);
  Profiler.ResetTimer(TimerId);

  Profiler.StartTimer(TimerAll);
  Profiler.StartTimer(TimerId);

  gettimeofday(&startall, NULL);
  gettimeofday(&start, NULL);

  TFIn FIn(filename);
  PNGraph Graph = TNGraph::Load(FIn);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  
  for (int i = 0; i < 10; i++) {
    Profiler.ResetTimer(TimerId);
    Profiler.StartTimer(TimerId);
    gettimeofday(&start, NULL);
    int64 cnt = TSnap::CountTriangles(Graph);
    Profiler.StopTimer(TimerId);
    gettimeofday(&end, NULL);
    printf("%s\n", TUInt64::GetStr(cnt).CStr());
    delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
              end.tv_usec - start.tv_usec) / 1.e6;
    printf("CountTriangles time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  }

  gettimeofday(&endall, NULL);
  Profiler.StopTimer(TimerAll);
  delta = ((endall.tv_sec  - startall.tv_sec) * 1000000u +
            endall.tv_usec - startall.tv_usec) / 1.e6;
  printf("__all__  \ttime %7.3f\tcpu %8.3f\n", delta, Profiler.GetTimerSec(TimerAll));

  Profiler.ResetTimer(TimerAll);
  Profiler.StartTimer(TimerAll);

  for (int i = 0; i < 10; i++) {
    Profiler.ResetTimer(TimerId);
    Profiler.StartTimer(TimerId);
    gettimeofday(&start, NULL);
    int64 cnt = TSnap::GetTriangleCnt(Graph);
    Profiler.StopTimer(TimerId);
    gettimeofday(&end, NULL);
    printf("%s\n", TUInt64::GetStr(cnt).CStr());
    delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
              end.tv_usec - start.tv_usec) / 1.e6;
    printf("TSnap::GetTriangleCnt time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
  }

  gettimeofday(&endall, NULL);
  Profiler.StopTimer(TimerAll);
  delta = ((endall.tv_sec  - startall.tv_sec) * 1000000u +
            endall.tv_usec - startall.tv_usec) / 1.e6;
  printf("__all__  \ttime %7.3f\tcpu %8.3f\n", delta, Profiler.GetTimerSec(TimerAll));

  return 0;
}
