#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

double Tick() {
  //return clock() / ((double)CLOCKS_PER_SEC);
#ifdef USE_OPENMP
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
  TVec<TPair<PTable,TStr> >& NodeTblV, Schema& NodeSchema, TVec<TQuad<PTable,TStr,TStr,TBool> >& EdgeTblV, Schema& EdgeSchema) {
  Schema PhotoSchema;
  PhotoSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  PhotoSchema.Add(TPair<TStr,TAttrType>("UploadedDate", atInt));
  PhotoSchema.Add(TPair<TStr,TAttrType>("LastUpdatedDate", atInt));
  PhotoSchema.Add(TPair<TStr,TAttrType>("Views", atInt));
  PhotoSchema.Add(TPair<TStr,TAttrType>("Media", atStr));
  PhotoSchema.Add(TPair<TStr,TAttrType>("URL", atStr));
  PTable PPhotoTbl = Load(PhotoSchema, 1, PrefixPath + TStr("photos.tsv"), Context);
  NodeTblV.Add(TPair<PTable,TStr>(PPhotoTbl, TStr("Photos")));

  Schema UserSchema;
  UserSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  UserSchema.Add(TPair<TStr,TAttrType>("UserName", atStr));
  UserSchema.Add(TPair<TStr,TAttrType>("RealName", atStr));
  UserSchema.Add(TPair<TStr,TAttrType>("Location", atStr));
  PTable PUserTbl = Load(UserSchema, 1, PrefixPath + TStr("users.tsv"), Context);
  NodeTblV.Add(TPair<PTable,TStr>(PUserTbl, TStr("Users")));

  Schema TagSchema;
  TagSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  TagSchema.Add(TPair<TStr,TAttrType>("MachineTag", atStr));
  TagSchema.Add(TPair<TStr,TAttrType>("Text", atStr));
  TagSchema.Add(TPair<TStr,TAttrType>("DisplayedText", atStr));
  PTable PTagTbl = Load(TagSchema, 1, PrefixPath + TStr("tags.tsv"), Context);
  NodeTblV.Add(TPair<PTable,TStr>(PTagTbl, TStr("Tags")));

  Schema CommentSchema;
  CommentSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  CommentSchema.Add(TPair<TStr,TAttrType>("CreatedDate", atInt));
  CommentSchema.Add(TPair<TStr,TAttrType>("PermanentLink", atStr));
  CommentSchema.Add(TPair<TStr,TAttrType>("Text", atStr));
  PTable PCommentTbl = Load(CommentSchema, 1, PrefixPath + TStr("comments.tsv"), Context);
  NodeTblV.Add(TPair<PTable,TStr>(PCommentTbl, TStr("Comments")));

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
  NodeTblV.Add(TPair<PTable,TStr>(PLocationTbl, TStr("Locations")));

  NodeSchema.Add(TPair<TStr,TAttrType>("Id", atStr));

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

  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PPhotoOwnerTbl, "Photos", "Users", false));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PPhotoOwnerTbl, "Users", "Photos", true));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PPhotoCommentTbl, "Photos", "Comments", false));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PPhotoLocationTbl, "Photos", "Locations", false));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PPhotoLocationTbl, "Locations", "Photos", true));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PCommentUserTbl, "Comments", "Users", false));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PPhotoTagTbl, "Photos", "Tags", false));
  EdgeTblV.Add(TQuad<PTable,TStr,TStr,TBool>(PTaggerTagTbl, "Tags", "Users", true));

  EdgeSchema.Add(TPair<TStr,TAttrType>("SrcId", atStr));
  EdgeSchema.Add(TPair<TStr,TAttrType>("DstId", atStr));
  EdgeSchema.Add(TPair<TStr,TAttrType>("Weight", atFlt));
}

/// Load a mixed-type node list. The results are grouped by types.
void LoadMixedTypeNodeList(const TStr& PrefixPath, const TStr& RandNodeFileName, THash<TStr,TStrV>& NodeH) {
  Schema RandNodeListSchema;
  RandNodeListSchema.Add(TPair<TStr,TAttrType>("NTypeName", atStr));
  RandNodeListSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  TTableContext Context;
  PTable RandNTbl = Load(RandNodeListSchema, 2, PrefixPath+RandNodeFileName, Context);

  for (int i = 0; i < RandNTbl->GetNumRows(); i++) {
    TStr NTypeStr = RandNTbl->GetStrVal(RandNodeListSchema.GetVal(0).GetVal1(), i);
    TStr NStr = RandNTbl->GetStrVal(RandNodeListSchema.GetVal(1).GetVal1(), i);
    if (!NodeH.IsKey(NTypeStr)) {
      NodeH.AddDat(NTypeStr).Gen(RandNTbl->GetNumRows().Val, 0); // over-estimate
    }
    NodeH.GetDat(NTypeStr).Add(NStr);
  }
}

void LoadNodeList(const TStr& PrefixPath, const TStr& NodeFileName, TVec<TPair<TStr,TStr> >& RandNV) {
  Schema NodeListSchema;
  NodeListSchema.Add(TPair<TStr,TAttrType>("NTypeName", atStr));
  NodeListSchema.Add(TPair<TStr,TAttrType>("Id", atStr));
  TTableContext Context;
  PTable RandNTbl = Load(NodeListSchema, 2, PrefixPath+NodeFileName, Context);

  int n = RandNTbl->GetNumRows();
  RandNV.Gen(n, n);
  for (int i = 0; i < n; i++) {
    TStr NTypeStr = RandNTbl->GetStrVal(NodeListSchema.GetVal(0).GetVal1(), i);
    TStr NStr = RandNTbl->GetStrVal(NodeListSchema.GetVal(1).GetVal1(), i);
    TPair<TStr,TStr> Node(NTypeStr, NStr);
    RandNV[i] = Node;
  }
}

void CreateIdHashes(const TVec<TPair<PTable,TStr> >& NodeTblV, THash<TStr,TStrH>& NStrH, TIntStrH& NIdH) {
  int ExpectedSz = 0;
  for (TVec<TPair<PTable,TStr> >::TIter it = NodeTblV.BegI(); it < NodeTblV.EndI(); it++) {
    ExpectedSz += (*it).GetVal1()->GetNumRows();
  }
  NStrH.Gen(NodeTblV.Len());
  NIdH.Gen(ExpectedSz);
}

template <class PGraph>
PGraph LoadGraph(const TVec<PTable>& NodeTblV, const TVec<TQuad<PTable,TStr,TStr,TBool> >& EdgeTblV, TStrIntH& NStrH, TIntStrH& NIdH) {
  PGraph Graph = PGraph::TObj::New();
  TStr IdColName("Id");

  for (int i = 0; i < NodeTblV.Len(); i++) {
    PTable Table = NodeTblV[i];
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr NStr = Table->GetStrVal(IdColName, CurrRowIdx);
      int NId = Graph->AddNode();
      NStrH.AddDat(NStr, NId);
      NIdH.AddDat(NId, NStr);
    }
  }
  TStr IdColName1("SrcId");
  TStr IdColName2("DstId");
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    PTable Table;
    TStr SrcETypeName;
    TStr DstETypeName;
    TBool IsDirectionReverse;
    EdgeTblV[i].GetVal(Table, SrcETypeName, DstETypeName, IsDirectionReverse);

    TStr SrcIdColName, DstIdColName;
    if (!IsDirectionReverse) { SrcIdColName = IdColName1; DstIdColName = IdColName2; }
    else { SrcIdColName = IdColName2; DstIdColName = IdColName1; }

    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr SrcNStr = Table->GetStrVal(SrcIdColName, CurrRowIdx);
      IAssertR(NStrH.IsKey(SrcNStr), "SrcId of edges must be a node Id");
      TInt SrcNId = NStrH.GetDat(SrcNStr);
      TStr DstNStr = Table->GetStrVal(DstIdColName, CurrRowIdx);
      IAssertR(NStrH.IsKey(DstNStr), "DstId of edges must be a node Id");
      TInt DstNId = NStrH.GetDat(DstNStr);
      Graph->AddEdge(SrcNId, DstNId);
    }
  }
  return Graph;
}

template <class PGraph>
PGraph LoadGraphMNet(const TVec<TPair<PTable,TStr> >& NodeTblV, const TVec<TQuad<PTable,TStr,TStr,TBool> >& EdgeTblV, THash<TStr,TStrH>& NStrH, TIntStrH& NIdH) {
  PGraph Graph = PGraph::TObj::New();
  TStr IdColName("Id");

  // Add node types
  for (int i = 0; i < NodeTblV.Len(); i++) {
    Graph->AddNType(NodeTblV[i].GetVal2());
    NStrH.AddDat(NodeTblV[i].GetVal2());
  }

  // Add edge types
//  TStr SrcETypeNames[] = {TStr("Photos"), TStr("Photos"), TStr("Photos"), TStr("Comments"), TStr("Photos"), TStr("Users")};
//  TStr DstETypeNames[] = {TStr("Users"), TStr("Comments"), TStr("Locations"), TStr("Users"), TStr("Tags"), TStr("Tags")};
  THash<TPair<TStr,TStr>, TInt> ETypeIdH;
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    TStr SrcETypeName = EdgeTblV[i].GetVal2();
    TStr DstETypeName = EdgeTblV[i].GetVal3();
    int Id = Graph->AddEType(TStr("Edge") + SrcETypeName.CStr() + DstETypeName.CStr(), SrcETypeName, DstETypeName);
    TInt ETypeId(Id);
    TPair<TStr,TStr> ETypeNamePair(SrcETypeName,DstETypeName);
    ETypeIdH.AddDat(ETypeNamePair, ETypeId);
  }

  // Add nodes
  for (int i = 0; i < NodeTblV.Len(); i++) {
    PTable Table = NodeTblV[i].GetVal1();
    int NTypeId = Graph->GetNTypeId(NodeTblV[i].GetVal2());
    TStrH* PNStrH = &(NStrH.GetDat(NodeTblV[i].GetVal2()));
    PNStrH->Gen(Table->GetNumRows());
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr NStr = Table->GetStrVal(IdColName, CurrRowIdx);
      int NId = Graph->AddNode(NTypeId);
      PNStrH->AddDat(NStr, NId);
      NIdH.AddDat(NId, NStr);
    }
  }

  // Add edges
  TStr IdColName1("SrcId");
  TStr IdColName2("DstId");
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    PTable Table;
    TStr SrcETypeName;
    TStr DstETypeName;
    TBool IsDirectionReverse;
    EdgeTblV[i].GetVal(Table, SrcETypeName, DstETypeName, IsDirectionReverse);

    TPair<TStr,TStr> ETypeNamePair(SrcETypeName,DstETypeName);
    TInt ETypeId = ETypeIdH.GetDat(ETypeNamePair);

    TStr SrcIdColName, DstIdColName;
    if (!IsDirectionReverse) { SrcIdColName = IdColName1; DstIdColName = IdColName2; }
    else { SrcIdColName = IdColName2; DstIdColName = IdColName1; }

    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr SrcNStr = Table->GetStrVal(SrcIdColName, CurrRowIdx);
      TInt SrcNId = NStrH.GetDat(SrcETypeName).GetDat(SrcNStr);
      TStr DstNStr = Table->GetStrVal(DstIdColName, CurrRowIdx);
      TInt DstNId = NStrH.GetDat(DstETypeName).GetDat(DstNStr);
      //StdOut->PutStrFmtLn("Edge %d->%d : %d->%d", SrcNId, DstNId, Graph->GetNTypeId(SrcNId), Graph->GetNTypeId(DstNId));
      Graph->AddEdge(SrcNId, DstNId, ETypeId);
    }
  }
  return Graph;
}

void CreateNborList(const TVec<TPair<PTable,TStr> >& NodeTblV, const TVec<TQuad<PTable,TStr,TStr,TBool> >& EdgeTblV,
                      const TVec<TPair<TStr,TStr> >& NodeV, TVec<TVec<TTriple<TStr,TStr,TBool> > >& NbrVV) {
  PSOut StdOut = TStdOut::New();
  THash<TPair<TStr,TStr>, TInt> NOrderH;
  int order = 0;
  for (TVec<TPair<TStr,TStr> >::TIter iter = NodeV.BegI(); iter < NodeV.EndI(); iter++) {
    NOrderH.AddDat(*iter, order++);
  }
  NbrVV.Gen(order);
  for (int i = 0; i < order; i++) {
    TVec<TTriple<TStr,TStr,TBool> > NbrV;
    NbrVV[i] = NbrV;
  }

  // Add edges
  TStr IdColName1("SrcId");
  TStr IdColName2("DstId");
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    PTable Table;
    TStr SrcETypeName;
    TStr DstETypeName;
    TBool IsDirectionReverse;
    EdgeTblV[i].GetVal(Table, SrcETypeName, DstETypeName, IsDirectionReverse);

    TStr SrcIdColName, DstIdColName;
    if (!IsDirectionReverse) { SrcIdColName = IdColName1; DstIdColName = IdColName2; }
    else { SrcIdColName = IdColName2; DstIdColName = IdColName1; }

    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr SrcNStr = Table->GetStrVal(SrcIdColName, CurrRowIdx);
      TPair<TStr,TStr> Src(SrcETypeName, SrcNStr);
      TInt SrcOrder = NOrderH.GetDat(Src);

      TStr DstNStr = Table->GetStrVal(DstIdColName, CurrRowIdx);
      TPair<TStr,TStr> Dst(DstETypeName, DstNStr);
      TInt DstOrder = NOrderH.GetDat(Dst);

      if (SrcOrder < DstOrder) {
        TTriple<TStr,TStr,TBool> Nbr(SrcETypeName, SrcNStr, false);
        NbrVV[DstOrder.Val].Add(Nbr);
      } else {
        TTriple<TStr,TStr,TBool> Nbr(DstETypeName, DstNStr, true);
        NbrVV[SrcOrder.Val].Add(Nbr);
      }
    }
  }

}

template <class PGraph>
PGraph LoadGraphMNetRandom(const TVec<TPair<PTable,TStr> >& NodeTblV, const TVec<TQuad<PTable,TStr,TStr,TBool> >& EdgeTblV,
                            const TVec<TPair<TStr,TStr> >& RandNV, const TVec<TVec<TTriple<TStr,TStr,TBool> > >& RandNbrVV,
                            THash<TStr,TStrH>& NStrH, TIntStrH& NIdH) {
  PGraph Graph = PGraph::TObj::New();
  TStr IdColName("Id");

  // Add node types
  for (int i = 0; i < NodeTblV.Len(); i++) {
    Graph->AddNType(NodeTblV[i].GetVal2());
    TStrH StrH;
    NStrH.AddDat(NodeTblV[i].GetVal2(), StrH);
  }

  // Add edge types
  THash<TPair<TStr,TStr>, TInt> ETypeIdH;
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    TStr SrcETypeName = EdgeTblV[i].GetVal2();
    TStr DstETypeName = EdgeTblV[i].GetVal3();
    int Id = Graph->AddEType(TStr("Edge") + SrcETypeName.CStr() + DstETypeName.CStr(), SrcETypeName, DstETypeName);
    TInt ETypeId(Id);
    TPair<TStr,TStr> ETypeNamePair(SrcETypeName,DstETypeName);
    ETypeIdH.AddDat(ETypeNamePair, ETypeId);
  }

  // Add nodes
  int order = 0;
  for (TVec<TPair<TStr,TStr> >::TIter iter = RandNV.BegI(); iter < RandNV.EndI(); iter++) {
    int NId = Graph->AddNode((*iter).GetVal1());
    NStrH.GetDat((*iter).GetVal1()).AddDat((*iter).GetVal2(), NId);
    NIdH.AddDat(NId, (*iter).GetVal2());

    const TVec<TTriple<TStr,TStr,TBool> >* Nbrs = &(RandNbrVV[order++]);
    for (TVec<TTriple<TStr,TStr,TBool> >::TIter iter2 = Nbrs->BegI(); iter2 < Nbrs->EndI(); iter2++) {
      TTriple<TStr,TStr,TBool>* Nbr = iter2;
      if (Nbr->GetVal3()) {
        TPair<TStr,TStr> ETypeNamePair((*iter).GetVal1(), Nbr->GetVal1());
        TInt ETypeId = ETypeIdH.GetDat(ETypeNamePair);
        Graph->AddEdge(NId, NStrH.GetDat(Nbr->GetVal1()).GetDat(Nbr->GetVal2()), ETypeId);
      } else {
        TPair<TStr,TStr> ETypeNamePair(Nbr->GetVal1(), (*iter).GetVal1());
        TInt ETypeId = ETypeIdH.GetDat(ETypeNamePair);
        Graph->AddEdge(NStrH.GetDat(Nbr->GetVal1()).GetDat(Nbr->GetVal2()), NId, ETypeId);
      }
    }
  }
  return Graph;
}

void BuildCombinedEdgeTable(const TVec<TPair<PTable,TStr> >& NodeTblV, const TVec<TQuad<PTable,TStr,TStr,TBool> >& EdgeTblV, THash<TStr,TStrH>& NStrH, TIntStrH& NIdH, PTable CombinedEdgeTbl) {
  TStr IdColName("Id");

  TInt NId(0);

  // Find all nodes
  for (int i = 0; i < NodeTblV.Len(); i++) {
    PTable Table = NodeTblV[i].GetVal1();
    TStrH StrH(Table->GetNumRows());
    NStrH.AddDat(NodeTblV[i].GetVal2(), StrH);
    TStrH* PNStrH = &(NStrH.GetDat(NodeTblV[i].GetVal2()));
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr NStr = Table->GetStrVal(IdColName, CurrRowIdx);
      PNStrH->AddDat(NStr, NId);
      NIdH.AddDat(NId, NStr);
      NId += 1;
    }
  }

  // Add edges
  TStr IdColName1("SrcId");
  TStr IdColName2("DstId");

//  int Len = 0;
//  for (int i = 0; i < EdgeTblV.Len(); i++) {
//    PTable Table = EdgeTblV[i].GetVal1();
//    Len += Table->GetNumRows().Val;
//  }
  for (int i = 0; i < EdgeTblV.Len(); i++) {
    PTable Table;
    TStr SrcETypeName;
    TStr DstETypeName;
    TBool IsDirectionReverse;
    EdgeTblV[i].GetVal(Table, SrcETypeName, DstETypeName, IsDirectionReverse);

    TStr SrcIdColName, DstIdColName;
    if (!IsDirectionReverse) { SrcIdColName = IdColName1; DstIdColName = IdColName2; }
    else { SrcIdColName = IdColName2; DstIdColName = IdColName1; }
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr SrcNStr = Table->GetStrVal(SrcIdColName, CurrRowIdx);
      TInt SrcNId = NStrH.GetDat(SrcETypeName).GetDat(SrcNStr);
      TStr DstNStr = Table->GetStrVal(DstIdColName, CurrRowIdx);
      TInt DstNId = NStrH.GetDat(DstETypeName).GetDat(DstNStr);

      TTableRow Row;
      Row.AddInt(SrcNId);
      Row.AddInt(DstNId);
      CombinedEdgeTbl->AddRow(Row);

//      SrcIdV.Add(SrcNId);
//      DstIdV.Add(DstNId);
    }
  }
}

template <class PGraph>
void PageRankExp(const PGraph& Graph, const int nExps, TIntFltH& PageRankResults) {
  for (int i = 0; i < nExps; i++) {
#ifdef USE_OPENMP
//    TSnap::GetPageRankMP2(Graph, PageRankResults, 0.849999999999998, 0.0001, 10);
    TSnap::GetPageRankMNetMP(Graph, PageRankResults, 0.849999999999998, 0.0001, 10);
#else
    TSnap::GetPageRank(Graph, PageRankResults, 0.849999999999998, 0.0001, 10);
#endif
  }
}

template <class PGraph>
void BFSExp(const PGraph& Graph, const TStrV& RandNStrs, const TStrIntH& NStrH,
            const int nExps, TIntV& BFSResults) {
  for (int i = 0; i < nExps; i++) {
    TStr NStr = RandNStrs[i];
    TInt NId = NStrH.GetDat(NStr);
#ifdef USE_OPENMP
    TSnap::GetShortestDistancesMP2(Graph, NId.Val, true, false, BFSResults);
#else
    TSnap::GetShortestDistances(Graph, NId.Val, true, false, BFSResults);
#endif
  }
}
