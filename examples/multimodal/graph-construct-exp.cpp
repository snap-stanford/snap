#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>
#include "benchmark-util.cpp"
#include "multimodal.cpp"

PSOut StdOut = TStdOut::New();

int main(int argc, char* argv[])
{
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");

  double ts1 = Tick();
  TTableContext Context;
  TVec<TPair<PTable,TStr> > NodeTblV = TVec<TPair<PTable,TStr> >();
  TVec<TQuad<PTable,TStr,TStr,TBool> > EdgeTblV = TVec<TQuad<PTable,TStr,TStr,TBool> >();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);

  double ts2 = Tick();

  THash<TStr,TStrH> NStrH;
  TIntStrH NIdH;
  Schema S;
  S.Add(TPair<TStr,TAttrType>("SrcId", atInt));
  S.Add(TPair<TStr,TAttrType>("DstId", atInt));
  PTable CombinedTable = TTable::New(S, Context);
  BuildCombinedEdgeTable(NodeTblV, EdgeTblV, NStrH, NIdH, CombinedTable);

  double ts3 = Tick();
  int nExps = 20;
  for (int i = 0; i < 20; i++) {
    PNEANetMP Graph = TSnap::ToTNEANetMP(CombinedTable, TStr("SrcId"), TStr("DstId"));
  }
//  StdOut->PutStrFmtLn("Graph %d-%d", Graph->GetNodes(), Graph->GetEdges());

  double ts4 = Tick();

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrLn("===== Graph Construction =====");
  TimeOut->PutStrLn(Env.GetCmLn());
  TimeOut->PutStrFmtLn("Input Time = %f", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Graph Construction Time = %f", GetCPUTimeUsage(ts3, ts4)/nExps);

	return 0;
}
