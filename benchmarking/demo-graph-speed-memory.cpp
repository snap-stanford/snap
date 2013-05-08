#include "Snap.h"
#include <mach/mach.h>

template<class PGraph> void GraphSpeedTest(const TStr& TypeName, const int& NNodes, const int& NEdges);

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("SnapTest. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  
  GraphSpeedTest<PNGraph>("NGRAPH", Kilo(1000), Mega(10));
  GraphSpeedTest<PNGraph>("NGRAPH", Kilo(2000), Mega(20));
  GraphSpeedTest<PNGraph>("NGRAPH", Kilo(4000), Mega(40));

  GraphSpeedTest<PUNGraph>("UNGRAPH", Kilo(1000), Mega(10));
  GraphSpeedTest<PUNGraph>("UNGRAPH", Kilo(2000), Mega(20));
  GraphSpeedTest<PUNGraph>("UNGRAPH", Kilo(4000), Mega(40));

  GraphSpeedTest<PNEGraph>("NEGRAPH", Kilo(1000), Mega(10));
  GraphSpeedTest<PNEGraph>("NEGRAPH", Kilo(2000), Mega(20));
  GraphSpeedTest<PNEGraph>("NEGRAPH", Kilo(4000), Mega(40));
   
  Catch
  printf("\ntotal run time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}

void PrintTm(TExeTm& ExeTm, TStr Desc=TStr()) {
  printf("** %s: %.2f sec\n", Desc.CStr(), ExeTm.GetSecs());
  ExeTm.Tick();
}

size_t GetMemoryUsage(void) {
    task_t targetTask = mach_task_self();
    struct task_basic_info ti;
    mach_msg_type_number_t count = TASK_BASIC_INFO_64_COUNT;
    kern_return_t kr = task_info(targetTask, TASK_BASIC_INFO_64, (task_info_t) &ti, &count);
    if (kr != KERN_SUCCESS) { printf("Kernel returned error during memory usage query"); return -1; }
    // On Mac OS X, the resident_size is in bytes, not pages!
    return ti.resident_size;
}

/////////////////////////////////////////////////////////////////////
// Graph speed test
template<class PGraph>
void GraphSpeedTest(const TStr& TypeName, const int& NNodes, const int& NEdges) {
  
  TRnd Rnd;
  TExeTm ExeTm;
  THash<TIntPr, TInt> EdgeH(NEdges, true);

  printf("\n %s: Generating %dk nodes, %dk edges:\n", TypeName.CStr(), NNodes/Kilo(1), NEdges/Kilo(1));
  while (EdgeH.Len() < NEdges) {
    int src = Rnd.GetUniDevInt(NNodes);
    int dst = Rnd.GetUniDevInt(NNodes);
    if (src >= dst) { continue; }
    EdgeH.AddKey(TIntPr(src, dst));
  }
  PrintTm(ExeTm, "time to generate edges");

  PGraph G = PGraph::TObj::New();
  for (int i = 0; i < NNodes; i++) {
    G->AddNode(i);
  }
  PrintTm(ExeTm, "time to add nodes");
  for (int i = 0; i < EdgeH.Len(); i++) {
    G->AddEdge(EdgeH.GetKey(i).Val1, EdgeH.GetKey(i).Val2);
  }
  PrintTm(ExeTm, "time to add edges");
  
  int tmp=0;
  for (int i = 0; i < 100; i++) {
    tmp += 3;
    for (typename PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      tmp *= NI.GetId();
    }
    tmp += 5;
  }
  PrintTm(ExeTm, "time to traverse nodes");
  for (int i = 0; i < 10; i++) {
    tmp *= 7;
    for (typename PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        tmp += NI.GetOutNId(e); }
    }
    tmp *= 9;
  }
  PrintTm(ExeTm, "time to traverse edges using TNodeI");
  for (int i = 0; i < 10; i++) {
    tmp *= 3;
    for (typename PGraph::TObj::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
      tmp += EI.GetSrcNId() * EI.GetDstNId();
    }
    tmp *= 7;
  }
  PrintTm(ExeTm, "time to traverse edges using TEdgeI");
  EdgeH.Clr(true); // remove edges hash table from memory
  printf("** graph memory usage: %.2f mb\n", double(GetMemoryUsage())/Mega(1));
  G->Defrag();
  printf("** graph memory usage: %.2f mb\n", double(GetMemoryUsage())/Mega(1));
  printf("-----------------------------------------------------------------------------%d\n", tmp);
}
