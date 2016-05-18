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
  printf("Entering ToGraph!\n");
  PUNGraph pungraph = TSnap::ToGraph<PUNGraph>(refs,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  fprintf(stderr, "Made the graph.\n");
  printf("Made the TUNGraph of %d nodes and %d edges.\n",(*pungraph).GetNodes(),(*pungraph).GetEdges());
//  for (TUNGraph::TNodeI node_i = (*pungraph).BegNI(); node_i < (*pungraph).EndNI(); node_i++) {
//    printf("%d\t1\n", node_i.GetId());
 // }
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
  PNGraph pngraph = TSnap::ToGraph<PNGraph>(refs,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TNGraph of %d nodes and %d edges.\n",(*pngraph).GetNodes(),(*pngraph).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));

  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  PNGraphMP pngraphmp = TSnap::ToGraphMP<PNGraphMP>(refs,schema[0].GetVal1(),schema[1].GetVal1());
  printf("Made the TNGraphMP of %d nodes and %d edges.\n",(*pngraphmp).GetNodes(),(*pngraphmp).GetEdges());
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
  PNEANet pneanet= TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(), aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges with no attributes.\n", (*pneanet).GetNodes(),(*pneanet).GetEdges());
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
  TVec<TStr> eattrv_0;
//  eattrv.Add(schema[1].GetVal1());
  TVec<TStr> nattrv_0;
  nattrv_0.Add(nodeschema[1].GetVal1());
  PNEANet pneanet_nattr = TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(),eattrv_0, nodetable, nodeschema[0].GetVal1(), nattrv_0, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges with one node attribute.\n", (*pneanet_nattr).GetNodes(),(*pneanet_nattr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif


#if 1
  TStr newintcolname("newintcolname");
  refs->AddIntCol(newintcolname);
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> attrv;
  attrv.Add(newintcolname);
  PNEANet pneanet_attr = TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(), attrv, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges with one edge attribute.\n", (*pneanet_attr).GetNodes(),(*pneanet_attr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
//  TStr newintcolname("newintcolname");
//  refs->AddIntCol(newintcolname);
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> eattrv;
  eattrv.Add(newintcolname);
  TVec<TStr> nattrv;
  nattrv.Add(nodeschema[1].GetVal1());
  PNEANet pneanet_neattr = TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(),eattrv, nodetable, nodeschema[0].GetVal1(), nattrv, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges with one node and one edge attribute.\n", (*pneanet_neattr).GetNodes(),(*pneanet_neattr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
//  TStr newintcolname("newintcolname");
//  refs->AddIntCol(newintcolname);
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> eattrv_21;
  eattrv_21.Add(newintcolname);
  TStr newintcolnamen("newintcolnamen");
  nodetable->AddIntCol(newintcolnamen);
  TVec<TStr> nattrv_21;
  nattrv_21.Add(nodeschema[1].GetVal1());
  nattrv_21.Add(newintcolnamen);
  PNEANet pneanet_n2eattr = TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(),eattrv_21, nodetable, nodeschema[0].GetVal1(), nattrv_21, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges with two node and one edge attribute.\n", (*pneanet_n2eattr).GetNodes(),(*pneanet_n2eattr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
  TStr newintcolnamee2("newintcolnamee2");
  refs->AddIntCol(newintcolnamee2);
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> eattrv_12;
  eattrv_12.Add(newintcolname);
  eattrv_12.Add(newintcolnamee2);
//  TStr newintcolnamen("newintcolnamen");
//  nodetable->AddIntCol(newintcolnamen);
  TVec<TStr> nattrv_12;
  nattrv_12.Add(nodeschema[1].GetVal1());
//  nattrv.Add(newintcolnamen);
  PNEANet pneanet_ne2attr = TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(),eattrv_12, nodetable, nodeschema[0].GetVal1(), nattrv_12, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges one node and two edge attribute.\n", (*pneanet_ne2attr).GetNodes(),(*pneanet_ne2attr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

#if 1
//  TStr newintcolnamee2("newintcolnamee2");
//  refs->AddIntCol(newintcolnamee2);
  Profiler.ResetTimer(TimerId);
  Profiler.StartTimer(TimerId);
  gettimeofday(&start, NULL);
  TVec<TStr> eattrv_22;
  eattrv_22.Add(newintcolname);
  eattrv_22.Add(newintcolnamee2);
//  TStr newintcolnamen("newintcolnamen");
//  nodetable->AddIntCol(newintcolnamen);
  TVec<TStr> nattrv_22;
  nattrv_22.Add(nodeschema[1].GetVal1());
  nattrv_22.Add(newintcolnamen);
  PNEANet pneanet_n2e2attr = TSnap::ToNetwork<PNEANet>(refs,schema[0].GetVal1(),schema[1].GetVal1(),eattrv_22, nodetable, nodeschema[0].GetVal1(), nattrv_22, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges two node and two edge attribute.\n", (*pneanet_n2e2attr).GetNodes(),(*pneanet_n2e2attr).GetEdges());
  Profiler.StopTimer(TimerId);
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e6;
  printf("Load time (elapsed): %f, cpu: %f\n", delta, Profiler.GetTimerSec(TimerId));
#endif

  return 0;
}
