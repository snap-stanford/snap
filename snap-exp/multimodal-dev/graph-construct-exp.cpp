#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>
#include "benchmark-util.cpp"
#include "multimodal.cpp"

PSOut StdOut = TStdOut::New();
TStopwatch* Sw = TStopwatch::GetInstance();

int main(int argc, char* argv[])
{
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");

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
  Schema S;
  S.Add(TPair<TStr,TAttrType>("SrcId", atInt));
  S.Add(TPair<TStr,TAttrType>("DstId", atInt));
  PTable CombinedTable = TTable::New(S, Context);
  BuildCombinedEdgeTable(NodeTblV, EdgeTblV, NStrH, NIdH, CombinedTable);
  Sw->Stop(TStopwatch::Preprocess);

  int nExps = 40;
  for (int i = 0; i < nExps; i++) {
    Sw->Start(TStopwatch::Compute);
    PNEANetMP Graph = TSnap::ToTNEANetMP2(CombinedTable, TStr("SrcId"), TStr("DstId"));
    Sw->Stop(TStopwatch::Compute);
  }
//  StdOut->PutStrFmtLn("Graph %d-%d", Graph->GetNodes(), Graph->GetEdges());
  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrLn("===== Graph Construction =====");
  TimeOut->PutStrLn(Env.GetCmLn());
  TimeOut->PutStrFmtLn("Input Time = %f from %d", Sw->Avg(TStopwatch::LoadTables), Sw->Cnt(TStopwatch::LoadTables));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f from %d", Sw->Avg(TStopwatch::Preprocess), Sw->Cnt(TStopwatch::Preprocess));
  TimeOut->PutStrFmtLn("Graph Construction Time = %f from %d", Sw->Avg(TStopwatch::Compute), Sw->Cnt(TStopwatch::Compute));
  TimeOut->PutStrFmtLn("Allocate Copies = %f from %d", Sw->Avg(TStopwatch::AllocateColumnCopies), Sw->Cnt(TStopwatch::AllocateColumnCopies));
  TimeOut->PutStrFmtLn("Copy = %f from %d", Sw->Avg(TStopwatch::CopyColumns), Sw->Cnt(TStopwatch::CopyColumns));
  TimeOut->PutStrFmtLn("Sort = %f from %d", Sw->Avg(TStopwatch::Sort), Sw->Cnt(TStopwatch::Sort));
  TimeOut->PutStrFmtLn("Group = %f from %d", Sw->Avg(TStopwatch::Group), Sw->Cnt(TStopwatch::Group));
  TimeOut->PutStrFmtLn("Merge Neighborhood = %f from %d", Sw->Avg(TStopwatch::MergeNeighborhoods), Sw->Cnt(TStopwatch::MergeNeighborhoods));
  TimeOut->PutStrFmtLn("Add Neighborhood = %f from %d", Sw->Avg(TStopwatch::AddNeighborhoods), Sw->Cnt(TStopwatch::AddNeighborhoods));
  TimeOut->PutStrFmtLn("Add Edges = %f from %d", Sw->Avg(TStopwatch::AddEdges), Sw->Cnt(TStopwatch::AddEdges));

	return 0;
}
