namespace TSnap {

// ORA Network Analysis Data (http://www.casos.cs.cmu.edu/computational_tools/data2.php)
// DyNetML format, loads only the first network in the file
PNGraph LoadDyNet(const TStr& FNm) {
  TXmlLx XmlLx(TFIn::New(FNm), xspTruncate);
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

// DyNetML format, loads all the networks in the file
TVec<PNGraph> LoadDyNetGraphV(const TStr& FNm) {
  TXmlLx XmlLx(TFIn::New(FNm), xspTruncate);
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
