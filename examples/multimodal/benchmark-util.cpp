#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

double Tick() {
  //return clock() / ((double)CLOCKS_PER_SEC);
  #ifdef _OPENMP
  return omp_get_wtime();
  #else
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  
  float cputime =
  ((float) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
  ((float) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
  return cputime;
  #endif
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
