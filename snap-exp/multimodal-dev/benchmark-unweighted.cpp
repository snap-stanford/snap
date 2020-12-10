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

  THash<TStr,TStrV> RandNStrV;
  LoadMixedTypeNodeList(PrefixPath, TStr("rand_node_ids_1000.tsv"), RandNStrV);

  double ts2 = Tick();

  THash<TStr,TStrH> NStrH;
  TIntStrH NIdH;
  CreateIdHashes(NodeTblV, NStrH, NIdH);



  double ts3 = Tick();
  PSVNet Graph = LoadGraphMNet<PSVNet>(NodeTblV, EdgeTblV, NStrH, NIdH);
//  PNEANet Graph = LoadGraph<PNEANet>(NodeTblV, EdgeTblV, NStrH, NIdH);

  double ts4 = Tick();

//  TIntFltH PageRankResults;
//  int nExps = 40;
//  PageRankExp(Graph, nExps, PageRankResults);

  TIntV BFSResults;
  int nExps = 10;
//  BFSExp(Graph, RandNStrV, NStrH, nExps, BFSResults);

  double ts5 = Tick();

//  PSOut ResultOut = TFOut::New(PrefixPath + TStr("page-rank-results.tsv"));
//  for (TIntFltH::TIter it = PageRankResults.BegI(); it < PageRankResults.EndI(); it++) {
//    ResultOut->PutStrFmtLn("%s\t%f9", NIdH.GetDat(it.GetKey()).CStr(), it.GetDat().Val);
//  }
  double ts6 = Tick();

  PSOut FeaturesOut = TFOut::New(PrefixPath + "features.txt");
  FeaturesOut->PutStrFmtLn("Photo %d", NodeTblV[0].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Users %d", NodeTblV[1].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Tags %d", NodeTblV[2].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Comments %d", NodeTblV[3].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Locations %d", NodeTblV[4].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Owner %d", EdgeTblV[0].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Comment %d", EdgeTblV[1].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Location %d", EdgeTblV[2].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Comment - User %d", EdgeTblV[3].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Tag %d", EdgeTblV[4].GetVal1()->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Photo - Tagger %d", PPhotoTaggerTbl->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Tagger - Tag %d", EdgeTblV[5].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Total number of nodes = %d", Graph->GetNodes());
  FeaturesOut->PutStrFmtLn("Total number of edges = %d", Graph->GetEdges());

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("Input Time = %f", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Conversion Time = %f", GetCPUTimeUsage(ts3, ts4));
  TimeOut->PutStrFmtLn("Computing Time = %f", GetCPUTimeUsage(ts4, ts5)/nExps);
  TimeOut->PutStrFmtLn("Output Time = %f", GetCPUTimeUsage(ts5, ts6));

	return 0;
}
