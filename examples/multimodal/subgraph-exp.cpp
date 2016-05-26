#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>
#include "benchmark-util.cpp"
#include "multimodal.cpp"

PSOut StdOut = TStdOut::New();

template <class PGraph>
PGraph SubgraphExtractExp(const PGraph& Graph, const TIntV& NTypeIdV) {
  return Graph->GetSubGraph(NTypeIdV);
}

template <class PGraph>
PNEANet SubgraphExtractTNEANetExp(const PGraph& Graph, const TIntV& NTypeIdV) {
  return Graph->GetSubGraphTNEANet(NTypeIdV);
}

template <class PGraph>
PNEANetMP SubgraphExtractTNEANetMPExp(const PGraph& Graph, const TIntV& NTypeIdV) {
  return Graph->GetSubGraphTNEANetMP2(NTypeIdV);
}

int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");

  TStopwatch* Sw = TStopwatch::GetInstance();
  Sw->Start(TStopwatch::LoadTables);
  TTableContext Context;
  TVec<TPair<PTable,TStr> > NodeTblV = TVec<TPair<PTable,TStr> >();
  TVec<TQuad<PTable,TStr,TStr,TBool> > EdgeTblV = TVec<TQuad<PTable,TStr,TStr,TBool> >();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);
  Sw->Stop(TStopwatch::LoadTables);

  Sw->Start(TStopwatch::Preprocess);
  THash<TStr,TStrH> NStrH;
  TIntStrH NIdH;
  CreateIdHashes(NodeTblV, NStrH, NIdH);

  TIntV NTypeIdV;
  if (Env.GetArgs() > 2) {
    for (int i = 2; i < Env.GetArgs(); i++) {
      NTypeIdV.Add(Env.GetArg(i).GetInt());
    }
  } else {
    NTypeIdV.Add(0);
    NTypeIdV.Add(1);
    NTypeIdV.Add(2);
    NTypeIdV.Add(3);
  }
  Sw->Stop(TStopwatch::Preprocess);

  Sw->Start(TStopwatch::ConstructGraph);
  PCVNet Graph = LoadGraphMNet<PCVNet>(NodeTblV, EdgeTblV, NStrH, NIdH);
  Sw->Stop(TStopwatch::ConstructGraph);
  int nExps = 20;
  for (int i = 0; i < nExps; i++) {
    StdOut->PutStrFmtLn("i = %d", i);
    Sw->Start(TStopwatch::Compute);
    //PCVNet Subgraph = SubgraphExtractExp(Graph, NTypeIdV);
    //PNEANet Subgraph = SubgraphExtractTNEANetExp(Graph, NTypeIdV);
    PNEANetMP Subgraph = SubgraphExtractTNEANetMPExp(Graph, NTypeIdV);
    Sw->Stop(TStopwatch::Compute);
  }
  // Sanity test
  PNEANetMP Subgraph = SubgraphExtractTNEANetMPExp(Graph, NTypeIdV);
  StdOut->PutStrFmtLn("Graph Size %d-%d", Graph->GetNodes(), Graph->GetEdges());
  StdOut->PutStrFmtLn("Subgraph Size %d-%d", Subgraph->GetNodes(), Subgraph->GetEdges());
  int InDeg = 0;
  int OutDeg = 0;
  for (TNEANetMP::TNodeI NI = Subgraph->BegNI(); NI < Subgraph->EndNI(); NI++) {
    InDeg += NI.GetInDeg(); OutDeg += NI.GetOutDeg();
  }
  StdOut->PutStrFmtLn("InDeg = %d; OutDeg = %d", InDeg, OutDeg);

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("===== Subgraph Extraction - PCVNet =====");
  TimeOut->PutStrLn(Env.GetCmLn());
  TimeOut->PutStrFmtLn("Input Time = %f from %d", Sw->Avg(TStopwatch::LoadTables), Sw->Cnt(TStopwatch::LoadTables));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f from %d", Sw->Avg(TStopwatch::Preprocess), Sw->Cnt(TStopwatch::Preprocess));
  TimeOut->PutStrFmtLn("Graph Construction Time = %f from %d", Sw->Avg(TStopwatch::ConstructGraph), Sw->Cnt(TStopwatch::ConstructGraph));
  TimeOut->PutStrFmtLn("Subgraph = %f from %d", Sw->Avg(TStopwatch::Compute), Sw->Cnt(TStopwatch::Compute));
  TimeOut->PutStrFmtLn("ComputeETypes = %f from %d", Sw->Avg(TStopwatch::ComputeETypes), Sw->Cnt(TStopwatch::ComputeETypes));
  TimeOut->PutStrFmtLn("EstimateSizes = %f from %d", Sw->Avg(TStopwatch::EstimateSizes), Sw->Cnt(TStopwatch::EstimateSizes));
  TimeOut->PutStrFmtLn("InitGraph = %f from %d", Sw->Avg(TStopwatch::InitGraph), Sw->Cnt(TStopwatch::InitGraph));
  TimeOut->PutStrFmtLn("ExtractNbrETypes = %f from %d", Sw->Sum(TStopwatch::ExtractNbrETypes)/Sw->Cnt(TStopwatch::Compute), Sw->Cnt(TStopwatch::Compute));
  TimeOut->PutStrFmtLn("CopyNodes = %f from %d", Sw->Sum(TStopwatch::CopyNodes)/Sw->Cnt(TStopwatch::Compute), Sw->Cnt(TStopwatch::Compute));
  TimeOut->PutStrFmtLn("PopulateGraph = %f,%f,%f from %d", Sw->Sum(TStopwatch::PopulateGraph)/Sw->Cnt(TStopwatch::Compute),
                                          Sw->Max(TStopwatch::PopulateGraph), Sw->Min(TStopwatch::PopulateGraph), Sw->Cnt(TStopwatch::Compute));
  TimeOut->PutStrFmtLn("ExtractEdges = %f,%f,%f from %d", Sw->Sum(TStopwatch::ExtractEdges)/Sw->Cnt(TStopwatch::Compute),
                                          Sw->Max(TStopwatch::ExtractEdges), Sw->Min(TStopwatch::ExtractEdges), Sw->Cnt(TStopwatch::Compute));
  TimeOut->PutStrFmtLn("BuildSubgraph = %f,%f,%f from %d", Sw->Sum(TStopwatch::BuildSubgraph)/Sw->Cnt(TStopwatch::Compute),
                                          Sw->Max(TStopwatch::BuildSubgraph), Sw->Min(TStopwatch::BuildSubgraph), Sw->Cnt(TStopwatch::Compute));

  return 0;
}
