#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>
#include "multimodal.cpp"

PSOut StdOut = TStdOut::New(); 

double Tick() {
  //return clock() / ((double)CLOCKS_PER_SEC);
  //return omp_get_wtime();
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  
  float cputime =
  ((float) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
  ((float) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
  return cputime;
}

double GetCPUTimeUsage(double tick1, double tick2) {
  return tick2 - tick1;
}

PTable Load(const Schema& TblSchema, const int nCols, const TStr& TsvFileName, TTableContext& TableContext, const TFlt DefaultWeight = 0.0) {
  TIntV RelColV;
  for (int i = 0; i < min(nCols, TblSchema.Len()); i++) {
    RelColV.Add(i);
  }
  PTable PTbl = TTable::LoadSS(TblSchema, TsvFileName, TableContext, RelColV);
  if (DefaultWeight != 0.0) {
    TFltV WeightCol(PTbl->GetNumRows());
    for (int i = 0; i < PTbl->GetNumRows(); i++) {
      WeightCol[i] = DefaultWeight;
    }
    PTbl->StoreFltCol(TStr("Weight"), WeightCol);
  }
  return PTbl;
}

void LoadFlickrTables(const TStr& PrefixPath, TTableContext& Context, 
  TVec<PTable>& NodeTblV, Schema& NodeSchema, TVec<PTable>& EdgeTblV, Schema& EdgeSchema) {
  Schema PhotoSchema;
  PhotoSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  PhotoSchema.Add(TPair<TStr,TAttrType>("UploadedDate", atInt));
  PhotoSchema.Add(TPair<TStr,TAttrType>("LastUpdatedDate", atInt));
  PhotoSchema.Add(TPair<TStr,TAttrType>("Views", atInt));
  PhotoSchema.Add(TPair<TStr,TAttrType>("Media", atStr));
  PhotoSchema.Add(TPair<TStr,TAttrType>("URL", atStr));
  PTable PPhotoTbl = Load(PhotoSchema, 1, PrefixPath + TStr("photos.tsv"), Context);
  
  Schema UserSchema;
  UserSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  UserSchema.Add(TPair<TStr,TAttrType>("UserName", atStr));
  UserSchema.Add(TPair<TStr,TAttrType>("RealName", atStr));
  UserSchema.Add(TPair<TStr,TAttrType>("Location", atStr));
  PTable PUserTbl = Load(UserSchema, 1, PrefixPath + TStr("users.tsv"), Context);
  
  Schema TagSchema;
  TagSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  TagSchema.Add(TPair<TStr,TAttrType>("MachineTag", atStr));
  TagSchema.Add(TPair<TStr,TAttrType>("Text", atStr));
  TagSchema.Add(TPair<TStr,TAttrType>("DisplayedText", atStr));
  PTable PTagTbl = Load(TagSchema, 1, PrefixPath + TStr("tags.tsv"), Context);
  
  Schema CommentSchema;
  CommentSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  CommentSchema.Add(TPair<TStr,TAttrType>("CreatedDate", atInt));
  CommentSchema.Add(TPair<TStr,TAttrType>("PermanentLink", atStr));
  CommentSchema.Add(TPair<TStr,TAttrType>("Text", atStr));
  PTable PCommentTbl = Load(CommentSchema, 1, PrefixPath + TStr("comments.tsv"), Context);
  
  Schema LocationSchema;
  LocationSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("WoeId", atInt));
  LocationSchema.Add(TPair<TStr,TAttrType>("Accuracy", atInt));
  LocationSchema.Add(TPair<TStr,TAttrType>("Context", atInt));
  LocationSchema.Add(TPair<TStr,TAttrType>("Latitude", atFlt));
  LocationSchema.Add(TPair<TStr,TAttrType>("Longitude", atFlt));
  LocationSchema.Add(TPair<TStr,TAttrType>("NeighborhoodId", atStr));
  // TODO: The WoeId should be atInt. Use atStr to avoid missing data problem.
  LocationSchema.Add(TPair<TStr,TAttrType>("NeighborhoodWoeId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("NeighborhoodName", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("LocalityId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("LocalityWoeId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("LocalityName", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("CountyId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("CountyWoeId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("CountyName", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("RegionId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("RegionWoeId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("RegionName", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("CountryId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("CountryWoeId", atStr));
  LocationSchema.Add(TPair<TStr,TAttrType>("CountryName", atStr));
  PTable PLocationTbl = Load(LocationSchema, 1, PrefixPath + TStr("locations.tsv"), Context);

  NodeSchema.Add(TPair<TStr,TAttrType>("Id", atStr));

  NodeTblV.Add(PPhotoTbl);
  NodeTblV.Add(PUserTbl);
  NodeTblV.Add(PTagTbl);
  NodeTblV.Add(PCommentTbl);
  NodeTblV.Add(PLocationTbl);

  Schema StdEdgeSchema = Schema();
  StdEdgeSchema.Add(TPair<TStr,TAttrType>("SrcId", atStr));
  StdEdgeSchema.Add(TPair<TStr,TAttrType>("DstId", atStr));

  PTable PPhotoOwnerTbl = Load(StdEdgeSchema, 2, PrefixPath + "photo_owner_edges.tsv", Context, 1.0);
  
  PTable PPhotoCommentTbl = Load(StdEdgeSchema, 2, PrefixPath + "photo_comment_edges.tsv", Context, 0.2);

  PTable PPhotoLocationTbl = Load(StdEdgeSchema, 2, PrefixPath + "photo_location_edges.tsv", Context, 0.05);

  PTable PCommentUserTbl = Load(StdEdgeSchema, 2, PrefixPath + "comment_user_edges.tsv", Context, 0.2);

  Schema PhotoTagSchema;
  PhotoTagSchema.Add(TPair<TStr,TAttrType>("SrcId", atStr));
  PhotoTagSchema.Add(TPair<TStr,TAttrType>("DstId", atStr));
  PhotoTagSchema.Add(TPair<TStr,TAttrType>("UserId", atStr));
  PTable PPhotoTagTbl = Load(PhotoTagSchema, 2, PrefixPath + "photo_tag_edges.tsv", Context, 0.1);

  //PTable PPhotoTaggerTbl = Load(StdEdgeSchema, 2, PrefixPath + "photo_tagger_edges_dupremoved.tsv", Context);
  //PTable PTaggerTagTbl = Load(StdEdgeSchema, 2, PrefixPath + "tagger_tag_edges_dupremoved.tsv", Context);
  
  Schema TaggerTagSchema;
  TaggerTagSchema.Add(TPair<TStr,TAttrType>("SrcId", atStr));
  TaggerTagSchema.Add(TPair<TStr,TAttrType>("DstId", atStr));
  TaggerTagSchema.Add(TPair<TStr,TAttrType>("PhotoId", atStr));
  PTable PTaggerTagTbl = Load(TaggerTagSchema, 2, PrefixPath + "tagger_tag_edges.tsv", Context, 0.1);
  
  EdgeTblV.Add(PPhotoOwnerTbl);
  EdgeTblV.Add(PPhotoCommentTbl);
  EdgeTblV.Add(PPhotoLocationTbl);
  EdgeTblV.Add(PCommentUserTbl);
  EdgeTblV.Add(PPhotoTagTbl);
  //EdgeTblV.Add(PPhotoTaggerTbl);
  EdgeTblV.Add(PTaggerTagTbl);

  EdgeSchema.Add(TPair<TStr,TAttrType>("SrcId", atStr));
  EdgeSchema.Add(TPair<TStr,TAttrType>("DstId", atStr));
  EdgeSchema.Add(TPair<TStr,TAttrType>("Weight", atFlt));
}

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
  PNGraphMP Graph = TSnap::ToGraphMP2<PNGraphMP>(EdgeTable, EdgeSchema.GetVal(0).GetVal1(), EdgeSchema.GetVal(1).GetVal1());
  double ts4 = Tick();
  
  //int nIters = 1;
  int nIters = 40;
  TIntFltH PageRankResults;
  for (int i = 0; i < nIters; i++) {
    PageRankResults = TIntFltH(ExpectedSz);
    TSnap::GetPageRank(Graph, PageRankResults);
    //TSnap::GetPageRankMP2(Graph, PageRankResults, 0.849999999999998, 0.0001, 10);
  }
  double ts5 = Tick();

  PSOut ResultOut = TFOut::New(PrefixPath + TStr("page-rank-results.tsv"));
  for (TIntFltH::TIter it = PageRankResults.BegI(); it < PageRankResults.EndI(); it++) {
    ResultOut->PutStrFmtLn("%s\t%f9", OriNIdV[it.GetKey()].CStr(), it.GetDat().Val);
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
