#include "stdafx.h"

PNEANet GetNEANet(const TStr& InFNm, const int& SrcColId = 0, const int& DstColId = 1, const int& CapColId = 2) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  PNEANet Net = TNEANet::New();
  int SrcNId, DstNId, Cap, EId;
  while (Ss.Next()) {
    if (! Ss.GetInt(SrcColId, SrcNId) || ! Ss.GetInt(DstColId, DstNId) || ! Ss.GetInt(CapColId, Cap)) { continue; }
    if (! Net->IsNode(SrcNId)) { Net->AddNode(SrcNId); }
    if (! Net->IsNode(DstNId)) { Net->AddNode(DstNId); }
    EId = Net->AddEdge(SrcNId, DstNId);
    Net->AddIntAttrDatE(EId, 0, TSnap::FlowAttrName);
    Net->AddIntAttrDatE(EId, Cap, TSnap::CapAttrName);
  }
  Net->Defrag();
  return Net;
}

PNGraph GetNGraph(const TStr& InFNm, TIntPrIntH &Cap, TIntPrIntH &Flow, const int& SrcColId = 0, const int& DstColId = 1, const int& CapColId = 2) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  PNGraph Graph = TNGraph::New();
  int SrcNId, DstNId, CapVal;
  while (Ss.Next()) {
    if (! Ss.GetInt(SrcColId, SrcNId) || ! Ss.GetInt(DstColId, DstNId) || ! Ss.GetInt(CapColId, CapVal)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
    Cap.AddDat(TIntPr(SrcNId, DstNId), CapVal);
    Flow.AddDat(TIntPr(SrcNId, DstNId), 0);
  }
  Graph->Defrag();
  return Graph;
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
  int Iters = 10;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "small.txt", "Input directed graph");
  printf("Integer Flow Test\n");
  printf("Filename: %s\n", InFNm.CStr());
  printf("Building Network...\n");
  PNEANet Net = GetNEANet(InFNm);
  TIntPrIntH Cap;
  TIntPrIntH Flow;
  PNGraph Graph = GetNGraph(InFNm, Cap, Flow);
  TRnd Random;
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

