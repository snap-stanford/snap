#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

/*
PSOut StdOut = TStdOut::New();


context = snap.TTableContext()

t = testutils.Timer()
r = testutils.Resource() 
# load tables and context
FIn = snap.TFIn(srcfile)

# load papers
print time.ctime(), "loading papers ..."
papers = snap.TTable.Load(FIn,context)
t.show("loadbin papers", papers)
r.show("__papers__")

# load references
print time.ctime(), "lo.ading references ..."
refs = snap.TTable.Load(FIn,context)
t.show("loadbin references", refs)
r.show("__references__")

# load context
print time.ctime(), "loading context ..."
context.Load(FIn)
t.show("loadbin context", refs)
r.show("__context__")

# load network
print time.ctime(), "loading network ..."
net = snap.TNEANet.Load(FIn)
t.show("loadbin network", net)
r.show("__network__")

print time.ctime(), "done"
*/
int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");
  TTableContext context;
  TTableContext nodecontext;
  Schema schema;
  Schema nodeschema;
  nodeschema.Add(TPair<TStr,TAttrType>("nid",atInt));
  nodeschema.Add(TPair<TStr,TAttrType>("attr",atInt));

  TFIn srcfile("/dfs/scratch0/viswa/fromlfs/mag-papers-refs-context-net.bin");

  struct timeval start, end;
  float delta;
  TTmProfiler Profiler;
  int TimerId = Profiler.AddTimer("Profiler");

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  printf("Loading node table...\n");
  TStr nodefilename = "/dfs/scratch0/viswa/fromlfs/snap-dev1/examples/convbenchmark/nodelistwithattr";
  PTable nodetable = TTable::LoadSS(nodeschema, nodefilename, &nodecontext, '\t', TBool(false));
  printf("Done!\n");
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));



  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  printf("Loading papers...\n");
  PTable papers = TTable::Load(srcfile, &context);
  printf("Done!\n");
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  printf("Loading refs...\n");
  PTable refs = TTable::Load(srcfile, &context);
  printf("Done!\n");
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  printf("Loading context...\n");
  context.Load(srcfile);
  printf("Done!\n");
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

//  TInt a(2);
//  refs->GetContextKey(a);

  schema = refs->GetSchema();


#if 1
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> eattrv;
  PNEANetMP pneanetmp = TSnap::ToNetworkMP<PNEANetMP>(refs, schema[0].GetVal1(), schema[1].GetVal1(), eattrv, aaFirst);
  printf("Made the TNEANetMP of %d nodes and %d edges with no attributes.\n",(*pneanetmp).GetNodes(),(*pneanetmp).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
  TStr newintcolname("newintcolname");
  refs->AddIntCol(newintcolname);
#endif
 
#if 1
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
 TVec<TStr> eattrv_1;
  eattrv_1.Add(newintcolname);
  PNEANetMP pneanetmp_eattr = TSnap::ToNetworkMP<PNEANetMP>(refs, schema[0].GetVal1(), schema[1].GetVal1(), eattrv_1, aaFirst);
  printf("Made the TNEANetMP of %d nodes and %d edges with one edge attribute.\n",(*pneanetmp_eattr).GetNodes(),(*pneanetmp_eattr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> nattrv2;
  TVec<TStr> eattrv2;
  nattrv2.Add(nodeschema[1].GetVal1());
  PNEANetMP pneanetmp_neattr = TSnap::ToNetworkMP<PNEANetMP>(refs, schema[0].GetVal1(), schema[1].GetVal1(), eattrv2, nodetable, nodeschema[0].GetVal1(), nattrv2, aaFirst);
  printf("Made the TNEANetMP of %d nodes and %d edges with one node attribute.\n",(*pneanetmp_neattr).GetNodes(),(*pneanetmp_neattr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> nattrv3;
  TVec<TStr> eattrv3;
  eattrv3.Add(newintcolname);
  nattrv3.Add(nodeschema[1].GetVal1());
  PNEANetMP pneanetmp_n2eattr = TSnap::ToNetworkMP<PNEANetMP>(refs, schema[0].GetVal1(), schema[1].GetVal1(), eattrv3, nodetable, nodeschema[0].GetVal1(), nattrv3, aaFirst);
  printf("Made the TNEANetMP of %d nodes and %d edges with one node and one edge attribute.\n",(*pneanetmp_n2eattr).GetNodes(),(*pneanetmp_n2eattr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif




  return 0;
}
