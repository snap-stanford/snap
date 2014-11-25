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
  THash<TInt, TStr> OriNIdH(ExpectedSz);

  //MergeNodeTables(NodeTblV, NodeSchema, Hash, OriNIdH);
  //PTable EdgeTable = MergeEdgeTables(EdgeTblV, EdgeSchema, Hash, Context);

  double ts3 = Tick();
  //PNGraphMP Graph = TSnap::ToGraphMP2<PNGraphMP>(EdgeTable, EdgeSchema.GetVal(0).GetVal1(), EdgeSchema.GetVal(1).GetVal1());
  PMVNet Graph = TMVNet::New();
  TStr IdColName("Id");
  TStr NTypeNames[] = {TStr("Photos"), TStr("Users"), TStr("Tags"), TStr("Comments"), TStr("Location")};
  for (int i = 0; i < NodeTblV.Len(); i++) {
    PTable Table = NodeTblV[i];
    int NTypeId = Graph->AddNType(NTypeNames[i]);
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr Val = Table->GetStrVal(IdColName, CurrRowIdx);
      int NId = Graph->AddNode(NTypeId);
      Hash.AddDat(Val, NId);
      OriNIdH.AddDat(NId, Val);
    }
  }
  TStr SrcIdColName("SrcId");
  TStr DstIdColName("DstId");
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    PTable Table = EdgeTblV[i];
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr OriginalSrcId = Table->GetStrVal(SrcIdColName, CurrRowIdx);
      IAssertR(Hash.IsKey(OriginalSrcId), "SrcId of edges must be a node Id");
      TInt UniversalSrcId = Hash.GetDat(OriginalSrcId);
      TStr OriginalDstId = Table->GetStrVal(DstIdColName, CurrRowIdx);
      IAssertR(Hash.IsKey(OriginalDstId), "DstId of edges must be a node Id");
      TInt UniversalDstId = Hash.GetDat(OriginalDstId);
      Graph->AddEdge(UniversalSrcId, UniversalDstId, 0);
    }
  }

  double ts4 = Tick();

  int nIters = 1;
  //int nIters = 40;
  TIntFltH PageRankResults;
  for (int i = 0; i < nIters; i++) {
//    #ifdef _OPENMP
//    TSnap::GetPageRankMP2(Graph, PageRankResults, 0.849999999999998, 0.0001, 10);
//    #else
    TSnap::GetPageRank(Graph, PageRankResults, 0.849999999999998, 0.0001, 10);
//    #endif
  }
  double ts5 = Tick();

  PSOut ResultOut = TFOut::New(PrefixPath + TStr("page-rank-results.tsv"));
  for (TIntFltH::TIter it = PageRankResults.BegI(); it < PageRankResults.EndI(); it++) {
    ResultOut->PutStrFmtLn("%s\t%f9", OriNIdH.GetDat(it.GetKey()).CStr(), it.GetDat().Val);
  }
  double ts6 = Tick();

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
  TimeOut->PutStrFmtLn("Input Time = %f", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Conversion Time = %f", GetCPUTimeUsage(ts3, ts4));
  TimeOut->PutStrFmtLn("Computing Time = %f", GetCPUTimeUsage(ts4, ts5)/nIters);
  TimeOut->PutStrFmtLn("Output Time = %f", GetCPUTimeUsage(ts5, ts6));

	return 0;
}
