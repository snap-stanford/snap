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
  TVec<TPair<PTable, int> > EdgeTblV = TVec<TPair<PTable, int> >();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);

  double ts2 = Tick();

  THash<TStr,TStrH> NStrH;
  TIntStrH NIdH;
  CreateIdHashes(NodeTblV, NStrH, NIdH);

  double ts3 = Tick();
  PSVNet Graph = LoadGraphMNet<PSVNet>(NodeTblV, EdgeTblV, NStrH, NIdH);

  double ts4 = Tick();

  int nExps = 10;
  int nTriads = 0;
  for (int i = 0; i < nExps; i++) {
    nTriads = TSnap::GetTriads(Graph);
  }
  double ts5 = Tick();
  StdOut->PutStrFmtLn("Triads %d", nTriads);

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("===== Triad Counting - PSVNet =====");
  TimeOut->PutStrLn(Env.GetCmLn());
  TimeOut->PutStrFmtLn("Input Time = %f", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Conversion Time = %f", GetCPUTimeUsage(ts3, ts4));
  TimeOut->PutStrFmtLn("Computing Time = %f", GetCPUTimeUsage(ts4, ts5)/nExps);

	return 0;
}
