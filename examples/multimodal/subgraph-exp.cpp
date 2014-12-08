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

int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");

  double ts1 = Tick();
  TTableContext Context;
  TVec<PTable> NodeTblV = TVec<PTable>();
  TVec<TPair<PTable, int> > EdgeTblV = TVec<TPair<PTable, int> >();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);

  double ts2 = Tick();

  TStrIntH NStrH;
  TIntStrH NIdH;
  CreateIdHashes(NodeTblV, NStrH, NIdH);

  double ts3 = Tick();

  double convertTime = 0;
  double subgraphTime = 0;
  int nExps = 10;

  TIntV NTypeIdV;
  NTypeIdV.Add(0);
  NTypeIdV.Add(1);
  NTypeIdV.Add(2);
  for (int i = 0; i < nExps; i++) {
    double t1 = Tick();
    PMVNet Graph = LoadGraphMNet<PMVNet>(NodeTblV, EdgeTblV, NStrH, NIdH);
    double t2 = Tick();
    PMVNet Subgraph = SubgraphExtractExp(Graph, NTypeIdV);
    double t3 = Tick();
    convertTime += (t2-t1);
    subgraphTime += (t3-t2);
    StdOut->PutStrFmtLn("Graph Size %d-%d", Graph->GetNodes(), Graph->GetEdges());
    StdOut->PutStrFmtLn("Subgraph Size %d-%d", Subgraph->GetNodes(), Subgraph->GetEdges());
  }

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("===== Subgraph Extraction - PMVNet =====");
  TimeOut->PutStrFmtLn("Loading Graph Tables = %f s", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing = %f s", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Conversion = %f s", convertTime/nExps);
  TimeOut->PutStrFmtLn("Node Deletion = %f s", subgraphTime/nExps);

  return 0;
}
