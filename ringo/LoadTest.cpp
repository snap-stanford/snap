// Uses files on madmax5

#include "Snap.h"
#include <cstring>
#include <ctime>

int main(int argc, char** argv){
  //char filename[500] = "/lfs/local/0/arijitb/benchmarks/soc-LiveJournal1.txt";
  //char filename[500] = "/lfs/local/0/arijitb/benchmarks/soc-LiveJournal3.txt";
  char filename[500] = "/lfs/local/0/arijitb/benchmarks/twitter_rv.txt";

  TTableContext Context;
  Schema S; S.Add(TPair<TStr,TAttrType>("src", atInt)); S.Add(TPair<TStr,TAttrType>("dest", atInt));
  TIntV RelevantCols; RelevantCols.Add(0); RelevantCols.Add(1);

  struct timeval start, end;
  float delta;
  TTmProfiler Profiler;
  int TimerId = Profiler.AddTimer("Profiler");

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TFIn FIn(filename);
  PTable Q = TTable::LoadSS("Table", S, filename, Context, RelevantCols);
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  return 0;
}
