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
  TVec<PTable> NodeTblV = TVec<PTable>();
  TVec<PTable> EdgeTblV = TVec<PTable>();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);

  double ts2 = Tick();

  int ExpectedSz = 0;
  for (TVec<PTable>::TIter it = NodeTblV.BegI(); it < NodeTblV.EndI(); it++) {
    PTable Table = *it;
    ExpectedSz += Table->GetNumRows();
  }

  THash<TStr, TInt> Hash(ExpectedSz);
  TStrV OriNIdV(ExpectedSz);

  MergeNodeTables(NodeTblV, NodeSchema, Hash, OriNIdV);
  PTable EdgeTable = MergeEdgeTables(EdgeTblV, EdgeSchema, Hash, Context);

  double ts3 = Tick();
  TStrV V;
  TStrV VE;
  VE.Add(EdgeSchema.GetVal(2).GetVal1());
  PNEANet Graph = TSnap::ToNetwork<PNEANet>(EdgeTable, EdgeSchema.GetVal(0).GetVal1(), EdgeSchema.GetVal(1).GetVal1(),
						V, V, VE, aaLast);
  double ts4 = Tick();

  //int nExps = 1;
  int nExps = 40;
  TIntFltH PageRankResults;
  for (int i = 0; i < nExps; i++) {
    PageRankResults = TIntFltH(ExpectedSz);
#ifdef USE_OPENMP
    TSnap::GetWeightedPageRankMP2(Graph, PageRankResults, EdgeSchema.GetVal(2).GetVal1(), 0.849999999999998, 0.0001, 10);
#else
    TSnap::GetWeightedPageRank(Graph, PageRankResults, EdgeSchema.GetVal(2).GetVal1(), 0.849999999999998, 0.0001, 10);
#endif
  }
  double ts5 = Tick();

  PSOut ResultOut = TFOut::New(PrefixPath + TStr("page-rank-results.tsv"));
  for (TIntFltH::TIter it = PageRankResults.BegI(); it < PageRankResults.EndI(); it++) {
    ResultOut->PutStrFmtLn("%s\t%f9", OriNIdV[it.GetKey()].CStr(), it.GetDat().Val);
  }
  double ts6 = Tick();

  bool isPar = false;
#ifdef USE_OPENMP
  isPar = true;
#endif

//  PSOut FeaturesOut = TFOut::New(PrefixPath + "features.txt");
//  FeaturesOut->PutStrFmtLn("Photo %d", PPhotoTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Users %d", PUserTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Tags %d", PTagTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Comments %d", PCommentTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Locations %d", PLocationTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Photo - Owner %d", PPhotoOwnerTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Photo - Comment %d", PPhotoCommentTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Photo - Location %d", PPhotoLocationTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Comment - User %d", PCommentUserTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Comment - User %d", PCommentUserTbl->GetNumRows().Val);
////  FeaturesOut->PutStrFmtLn("Photo - Tagger %d", PPhotoTaggerTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Tagger - Tag %d", PTaggerTagTbl->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Total number of nodes = %d", Graph->GetNodes());
//  FeaturesOut->PutStrFmtLn("Total number of edges = %d", Graph->GetEdges());

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("Experiment Weighted - %s - %s", PrefixPath.CStr(), (isPar ? "Parallel" : "Sequential"));
  TimeOut->PutStrFmtLn("Input Time = %f", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Conversion Time = %f", GetCPUTimeUsage(ts3, ts4));
  TimeOut->PutStrFmtLn("Computing Time = %f", GetCPUTimeUsage(ts4, ts5)/nExps);
  TimeOut->PutStrFmtLn("Output Time = %f", GetCPUTimeUsage(ts5, ts6));

  return 0;
}
