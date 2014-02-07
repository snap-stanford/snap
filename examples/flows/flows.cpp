#include "stdafx.h"

void GetNetAndGraph(const TStr& InFNm, PNEANet &Net, PNGraph &Graph, TIntPrIntH &Cap, TIntPrIntH &Flow, TRnd &Random, const int& SrcColId = 0, const int& DstColId = 1, const int& CapColId = 2) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  Net.Clr();
  Net = TNEANet::New();
  Graph.Clr();
  Graph = TNGraph::New();
  Cap.Clr();
  Flow.Clr();
  TInt a;
  int NId = 1;
  int SrcNId, DstNId, CapVal, EId;
  long long OrigSrcNId, OrigDstNId;
  THash<TUInt64, TInt> NIdH;
  while (Ss.Next()) {
    if (! Ss.GetLongLong(SrcColId, OrigSrcNId) || ! Ss.GetLongLong(DstColId, OrigDstNId)) { continue; }
    CapVal = Random.GetUniDevInt(0, 10000);
    if (! NIdH.IsKey(OrigSrcNId)) {
      SrcNId = NIdH.AddDat(OrigSrcNId, NId++);
      Net->AddNode(SrcNId);
      Graph->AddNode(SrcNId);
    } else { SrcNId = NIdH.GetDat(OrigSrcNId); }
    if (! Net->IsNode(DstNId)) {
      DstNId = NIdH.AddDat(OrigDstNId, NId++);
      Net->AddNode(DstNId);
      Graph->AddNode(DstNId);
    } else { DstNId = NIdH.GetDat(OrigDstNId); }
    EId = Net->AddEdge(SrcNId, DstNId);
    Graph->AddEdge(SrcNId, DstNId);
    Net->AddIntAttrDatE(EId, 0, TSnap::FlowAttrName);
    Flow.AddDat(TIntPr(SrcNId, DstNId), 0);
    Net->AddIntAttrDatE(EId, CapVal, TSnap::CapAttrName);
    Cap.AddDat(TIntPr(SrcNId, DstNId), CapVal);
  }
  Net->Defrag();
  Graph->Defrag();
}

double getcputime() {
  struct rusage rusage;
  double result;

  getrusage(RUSAGE_SELF, &rusage);

  result =
    ((double) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
    ((double) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
  return result;
}

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nFlow. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  double BeginTime, EndTime, NetFlowRunTime, GraphFlowRunTime;
  int MaxEdgeCap = 0;
  double NetTimeSum = 0;
  double GraphTimeSum = 0;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "small.txt", "Input directed graph");
  const int Iters = Env.GetIfArgPrefixInt("-n:", 10, "Number of runs");
  printf("Integer Flow Test\n");
  printf("Filename: %s\n", InFNm.CStr());
  printf("Building Network...\n");
  PNEANet Net;
  PNGraph Graph;
  TIntPrIntH Cap;
  TIntPrIntH Flow;
  TRnd Random;
  GetNetAndGraph(InFNm, Net, Graph, Cap, Flow, Random);
  for (TIntPrIntH::TIter HI = Cap.BegI(); HI != Cap.EndI(); HI++) {
    MaxEdgeCap = (HI.GetDat().Val > MaxEdgeCap) ? HI.GetDat().Val : MaxEdgeCap;
  }
  printf("Nodes: %d, Edges: %d, Max Edge Capacity: %d \n\n\n", Net->GetNodes(), Net->GetEdges(), MaxEdgeCap);
  for (int i = 0; i < Iters; i++) {
    int SrcNId = Net->GetRndNId(Random);
    int SnkNId = Net->GetRndNId(Random);
    printf ("Source: %d, Sink %d\n", SrcNId, SnkNId);
    BeginTime = getcputime();
    int NetMaxFlow = TSnap::GetMaxFlowInt(Net, SrcNId, SnkNId);
    printf ("Max Flow PNEANet: %d\n", NetMaxFlow);
    EndTime = getcputime();
    NetFlowRunTime = EndTime - BeginTime;
    printf("run time: %f\n", NetFlowRunTime);
    BeginTime = getcputime();
    int GraphMaxFlow = TSnap::GetMaxFlowInt(Graph, Cap, Flow, SrcNId, SnkNId);
    printf ("Max Flow PNGraph: %d\n", GraphMaxFlow);
    EndTime = getcputime();
    GraphFlowRunTime = EndTime - BeginTime;
    printf("run time: %f\n", GraphFlowRunTime);
    NetTimeSum += NetFlowRunTime;
    GraphTimeSum += GraphFlowRunTime;
    printf ("\n");
  }
  printf ("\nAvg PNEANet Time: %f\n", NetTimeSum/Iters);
  printf ("Avg PNGraph Time: %f\n\n", GraphTimeSum/Iters);
  Catch
  return 0;
}

