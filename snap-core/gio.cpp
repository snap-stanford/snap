namespace TSnap {

PNEANet LoadEdgeListNet(const TStr& InFNm, const char& Separator) {
  TSsParser Ss(InFNm, Separator, true, false, false);
  TStrIntH IntAttrVals;
  TStrIntH FltAttrVals;
  TStrIntH StrAttrVals;
  int SrcColId = -1;
  int DstColId = -1;
  TStr SchemaStart("#SCHEMA");
  TStr SrcIdName("SrcNId");
  TStr DstIdName("DstNId");
  /* Read in attribute positions */
  while (Ss.Next() && (Ss.GetFlds() > 0 && Ss.GetFld(0)[0] == '#')) {
    if (SchemaStart != Ss.GetFld(0)) continue;
    for (int i = 1; i < Ss.GetFlds(); i++) {
      if (SrcIdName == Ss.GetFld(i)) {
        SrcColId = i-1;
        continue;
      }
      if (DstIdName == Ss.GetFld(i)) {
        DstColId = i-1;
        continue;
      }
      TStr Attr(Ss.GetFld(i));
      TStr AttrType;
      TStr AttrName;
      Attr.SplitOnCh(AttrType, ":", AttrName);
      if (AttrType == "Int") {
        IntAttrVals[AttrName] = i-1;
      }
      if (AttrType == "Flt") {
        FltAttrVals[AttrName] = i-1;
      }
      if (AttrType == "Str") {
        StrAttrVals[AttrName] = i-1;
      }
    }
  }
  if (SrcColId == -1 || DstColId == -1) {
    //Do some error handling.
  }
  return LoadEdgeListNet(InFNm, SrcColId, DstColId, IntAttrVals, FltAttrVals, StrAttrVals, Separator);
}

PNEANet LoadEdgeListNet(const TStr& InFNm, const int SrcColId, const DstColId, const TStrIntH& IntAttrVals,
  const TStrIntH& FltAttrVals, const TStrIntH& StrAttrVals, const char& Separator) {
  TSsParser Ss(InFNm, Separator);
  PNEANet Graph = PNEANet::New();
  int SrcNId, DstNId;
  while (Ss.Next()) {
    if (! Ss.GetInt(SrcColId, SrcNId) || ! Ss.GetInt(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    int EId = Graph->AddEdge(SrcNId, DstNId);
    float FltAttrVal;
    for (TStrIntH::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      Ss.GetFlt(it.GetDat(), FltAttrVal);
      Graph->AddFltAttrDatN(EId, FltAttrVal, it.GetKey());
    }
    int IntAttrVal;
    for (TStrIntH::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      Ss.GetInt(it.GetDat(), IntAttrVal);
      Graph->AddIntAttrDatN(EId, IntAttrVal, it.GetKey());
    }
    char* StrAttrVal;
    for (TStrIntH::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      StrAttrVal = Ss.GetFld(it.GetDat());
      Graph->AddIntAttrDatN(EId, TStr(StrAttrVal), it.GetKey());
    }
  }
}

/*void SaveEdgeListNet(const PNEANet& Graph, const TStr& OutFNm, const TStr& Desc) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  TStr SchemaStart("#SCHEMA");
  TStr SrcIdName("SrcNId");
  TStr DstIdName("DstNId");
  if (HasGraphFlag(PNEANet, gfDirected)) { fprintf(F, "# Directed network: %s \n", OutFNm.CStr()); } 
  else { fprintf(F, "# Undirected network (each unordered pair of nodes is saved once): %s\n", OutFNm.CStr()); }
  if (! Desc.Empty()) { fprintf(F, "# %s\n", Desc.CStr()); }
  fprintf(F, "# Nodes: %d Edges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  fprintf(F, "%s\t%s\t%s", SchemaStart.CStr(), SrcIdName().CStr(), DstIdName.CStr());
  for()

  for (typename PGraph::TObj::TEdgeI ei = Graph->BegEI(); ei < Graph->EndEI(); ei++) {
    fprintf(F, "%d\t%d\n", ei.GetSrcNId(), ei.GetDstNId());
  }
  fclose(F);
}*/


/// For more info see ORA Network Analysis Data (http://www.casos.cs.cmu.edu/computational_tools/data2.php)
PNGraph LoadDyNet(const TStr& FNm) {
  TXmlLx XmlLx(TZipIn::IsZipFNm(FNm)?TZipIn::New(FNm):TFIn::New(FNm), xspTruncate);
  THashSet<TStr> NIdStr;
  while (XmlLx.GetSym()!=xsyEof) {
    if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="network") {
      PNGraph G = TNGraph::New();
      XmlLx.GetSym();
      while (XmlLx.TagNm=="link") {
        TStr Str1, Val1, Str2, Val2;
        XmlLx.GetArg(0, Str1, Val1);  XmlLx.GetArg(1, Str2, Val2);
        IAssert(Str1=="source" && Str2=="target");
        NIdStr.AddKey(Val1); NIdStr.AddKey(Val2);
        const int src=NIdStr.GetKeyId(Val1);
        const int dst=NIdStr.GetKeyId(Val2);
        if (! G->IsNode(src)) { G->AddNode(src); }
        if (! G->IsNode(dst)) { G->AddNode(dst); }
        G->AddEdge(src, dst);
        XmlLx.GetSym();
      }
      return G;
    }
  }
  return PNGraph();
}

/// For more info see ORA Network Analysis Data (http://www.casos.cs.cmu.edu/computational_tools/data2.php)
TVec<PNGraph> LoadDyNetGraphV(const TStr& FNm) {
  TXmlLx XmlLx(TZipIn::IsZipFNm(FNm)?TZipIn::New(FNm):TFIn::New(FNm), xspTruncate);
  TVec<PNGraph> GraphV;
  THashSet<TStr> NIdStr;
  while (XmlLx.GetSym()!=xsyEof) {
    if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="network") {
      PNGraph G = TNGraph::New();
      GraphV.Add(G);
      XmlLx.GetSym();
      while (XmlLx.TagNm=="link") {
        TStr Str1, Val1, Str2, Val2;
        XmlLx.GetArg(0, Str1, Val1);  XmlLx.GetArg(1, Str2, Val2);
        IAssert(Str1=="source" && Str2=="target");
        NIdStr.AddKey(Val1); NIdStr.AddKey(Val2);
        const int src=NIdStr.GetKeyId(Val1);
        const int dst=NIdStr.GetKeyId(Val2);
        if (! G->IsNode(src)) { G->AddNode(src); }
        if (! G->IsNode(dst)) { G->AddNode(dst); }
        G->AddEdge(src, dst);
        XmlLx.GetSym();
      }
    }
  }
  return GraphV;
}

}; // namespace TSnap
