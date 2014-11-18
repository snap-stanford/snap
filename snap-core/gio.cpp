namespace TSnap {

// Reads the schema from the file, and fills the SrcColId, DstColId, and the vectors with the index,
// within a given line, at which the source/destination nodes and attributes can be found in the file.
// The schema must have the format specified in WriteSchemaToFile.
void ReadSchemaFromFile(const TStr &InFNm, const char& Separator, int& SrcColId, int& DstColId, TStrIntH& IntAttrVals, TStrIntH& FltAttrVals, TStrIntH& StrAttrVals) {
  TSsParser Ss(InFNm, Separator, true, false, false);

  while (Ss.Next() && (Ss.GetFlds() > 0 && Ss.GetFld(0)[0] == '#')) {
    printf("%s", Ss.GetFld(0));
    if (SCHEMA_START != Ss.GetFld(0)) continue;
    for (int i = 1; i < Ss.GetFlds(); i++) {
      if (SRC_ID_NAME == Ss.GetFld(i)) {
        SrcColId = i-1;
        continue;
      }
      if (DST_ID_NAME == Ss.GetFld(i)) {
        DstColId = i-1;
        continue;
      }
      TStr Attr(Ss.GetFld(i));
      TStr AttrType;
      TStr AttrName;
      Attr.SplitOnCh(AttrType, ':', AttrName);
      if (AttrType == INT_TYPE_PREFIX) {
        IntAttrVals.AddDat(AttrName, i-1);
      }
      if (AttrType == FLT_TYPE_PREFIX) {
        FltAttrVals.AddDat(AttrName, i-1);
      }
      if (AttrType == STR_TYPE_PREFIX) {
        StrAttrVals.AddDat(AttrName, i-1);
      }
    }
  }
}

PNEANet LoadEdgeListNet(const TStr& InFNm, const char& Separator) {
  TStrIntH IntAttrVals;
  TStrIntH FltAttrVals;
  TStrIntH StrAttrVals;
  int SrcColId = -1;
  int DstColId = -1;
  ReadSchemaFromFile(InFNm, Separator, SrcColId, DstColId, IntAttrVals, FltAttrVals, StrAttrVals);
  return LoadEdgeListNet(InFNm, SrcColId, DstColId, IntAttrVals, FltAttrVals, StrAttrVals, Separator);
}

PNEANet LoadEdgeListNet(const TStr& InFNm, const int SrcColId, const int DstColId, const TStrIntH& IntAttrVals,
  const TStrIntH& FltAttrVals, const TStrIntH& StrAttrVals, const char& Separator) {
  TSsParser Ss(InFNm, Separator);
  PNEANet Graph = PNEANet::New();
  int SrcNId, DstNId;
  while (Ss.Next()) {
    if (! Ss.GetInt(SrcColId, SrcNId) || ! Ss.GetInt(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    int EId = Graph->AddEdge(SrcNId, DstNId);
    double FltAttrVal;
    for (TStrIntH::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      Ss.GetFlt(it.GetDat(), FltAttrVal);
      Graph->AddFltAttrDatE(EId, FltAttrVal, it.GetKey());
    }
    int IntAttrVal;
    for (TStrIntH::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      Ss.GetInt(it.GetDat(), IntAttrVal);
      Graph->AddIntAttrDatE(EId, IntAttrVal, it.GetKey());
    }
    char* StrAttrVal;
    for (TStrIntH::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      StrAttrVal = Ss.GetFld(it.GetDat());
      Graph->AddStrAttrDatE(EId, TStr(StrAttrVal), it.GetKey());
    }
  }
  return Graph;
}

// Writes the schema out to the file, which consists of the SrcNId, DstNId, and edge attributes, separated by tabs.
// Edge attributes are written in the format <Type>:<Name>, where Type is eihter Int, Flt, or Str.
void WriteSchemaToFile(FILE *F, const PNEANet& Graph, TStrV &IntAttrNames, TStrV &FltAttrNames, TStrV &StrAttrNames) {
  fprintf(F, "%s\t%s\t%s", SCHEMA_START.CStr(), SRC_ID_NAME.CStr(), DST_ID_NAME.CStr());
  for(int i = 0; i < IntAttrNames.Len(); i++) {
    fprintf(F, "\t%s:%s", INT_TYPE_PREFIX.CStr(), IntAttrNames[i].CStr());
  }
  for(int i = 0; i < FltAttrNames.Len(); i++) {
    fprintf(F, "\t%s:%s", FLT_TYPE_PREFIX.CStr(), FltAttrNames[i].CStr());
  }
  for(int i = 0; i < StrAttrNames.Len(); i++) {
    fprintf(F, "\t%s:%s", STR_TYPE_PREFIX.CStr(), StrAttrNames[i].CStr());
  }
  fprintf(F, "\n");
}

void SaveEdgeListNet(const PNEANet& Graph, const TStr& OutFNm, const TStr& Desc) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Directed network: %s \n", OutFNm.CStr());
  if (! Desc.Empty()) { fprintf(F, "# %s\n", Desc.CStr()); }
  fprintf(F, "# Nodes: %d Edges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  
  TStrV IntAttrNames;
  TStrV FltAttrNames;
  TStrV StrAttrNames;
  Graph->GetAttrENames(IntAttrNames, FltAttrNames, StrAttrNames);

  WriteSchemaToFile(F, Graph, IntAttrNames, FltAttrNames, StrAttrNames);

  // Write out node ids and attributes.
  for (TNEANet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    fprintf(F, "%d\t%d", EI.GetSrcNId(), EI.GetDstNId());
    for(int i = 0; i < IntAttrNames.Len(); i++) {
      int AttrIntVal = Graph->GetIntAttrDatE(EI.GetId(), IntAttrNames[i]);
      fprintf(F, "\t%d", AttrIntVal);
    }
    for(int i = 0; i < FltAttrNames.Len(); i++) {
      double AttrFltVal = Graph->GetFltAttrDatE(EI.GetId(), FltAttrNames[i]);
      fprintf(F, "\t%f", AttrFltVal);
    }
    for(int i = 0; i < StrAttrNames.Len(); i++) {
      char * AttrStrVal = Graph->GetStrAttrDatE(EI.GetId(), StrAttrNames[i]).CStr();
      fprintf(F, "\t%s", AttrStrVal);
    }
    fprintf(F, "\n");
  }
  fclose(F);
}


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
