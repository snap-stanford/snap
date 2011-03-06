/////////////////////////////////////////////////
// Loading and saving graphs
namespace TSnap {
// Load
template <class PGraph> PGraph LoadEdgeList(const TStr& InFNm, const int& SrcColId=0, const int& DstColId=1);
template <class PGraph> PGraph LoadEdgeList(const TStr& InFNm, const int& SrcColId, const int& DstColId, const char& Separator);
template <class PGraph> PGraph LoadEdgeListStr(const TStr& InFNm, const int& SrcColId=0, const int& DstColId=1);
template <class PGraph> PGraph LoadPajek(const TStr& InFNm);
template <class PGraph> PGraph LoadConnList(const TStr& InFNm);
//template <class PGraph> PGraph LoadAdjMtx(const TStr& FNm, const int Thresh);
//PNGraph TNGraph::LoadNextEdgeList(TSIn& SIn, const bool& IsDir);
//PNGraph TNGraph::LoadAdjMtx(const TStr& FNm, const int Thresh) {

// ORA Network Analysis Data (http://www.casos.cs.cmu.edu/computational_tools/data2.php)
// DyNetML format, loads only the first network in the file
PNGraph LoadDyNet(const TStr& FNm);
// DyNetML format, loads all the networks in the file
TVec<PNGraph> LoadDyNetGraphV(const TStr& FNm);

// Save
template <class PGraph> void SaveEdgeList(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc=TStr());
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm);
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStrH& NIdClrH);
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStrH& NIdClrH, const TIntStrH& NIdLabelH);
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStrH& NIdClrH, const TIntStrH& NIdLabelH, const TIntStrH& EIdClrH);
//template <class PGraph> SaveGml(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc);
template <class PGraph> void SaveMatlabSparseMtx(const PGraph& Graph, const TStr& OutFNm);

/////////////////////////////////////////////////
// Implementation

// Whitespace separated file. One edge per line
// SrcColId and DstColId are column indexes of source/destination INTEGER node id
template <class PGraph>
PGraph LoadEdgeList(const TStr& InFNm, const int& SrcColId, const int& DstColId) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  PGraph Graph = PGraph::TObj::New();
  int SrcNId, DstNId;
  while (Ss.Next()) {
    if (! Ss.GetInt(SrcColId, SrcNId) || ! Ss.GetInt(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}

// Separator separated file. One edge per line
// SrcColId and DstColId are column indexes of source/destination INTEGER node id
template <class PGraph>
PGraph LoadEdgeList(const TStr& InFNm, const int& SrcColId, const int& DstColId, const char& Separator) {
  TSsParser Ss(InFNm, Separator);
  PGraph Graph = PGraph::TObj::New();
  int SrcNId, DstNId;
  while (Ss.Next()) {
    if (! Ss.GetInt(SrcColId, SrcNId) || ! Ss.GetInt(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}


// Whitespace separated file. One edge per line
// SrcColId and DstColId are column indexes of source/destination STRING node id
template <class PGraph>
PGraph LoadEdgeListStr(const TStr& InFNm, const int& SrcColId, const int& DstColId) {
  TSsParser Ss(InFNm, ssfWhiteSep);
  PGraph Graph = PGraph::TObj::New();
  TStrHash<TInt> StrSet(Mega(1), true);
  while (Ss.Next()) {
    const int SrcNId = StrSet.AddKey(Ss[SrcColId]);
    const int DstNId = StrSet.AddKey(Ss[DstColId]);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}

template <class PGraph>
PGraph LoadConnList(const TStr& InFNm) {
  TSsParser Ss(InFNm, ssfWhiteSep);
  PGraph Graph = PGraph::TObj::New();
  while (Ss.Next()) {
    if (! Ss.IsInt(0)) { continue; }
    const int SrcNId = Ss.GetInt(0);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    for (int dst = 1; dst < Ss.Len(); dst++) {
      const int DstNId = Ss.GetInt(dst);
      if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
      Graph->AddEdge(SrcNId, DstNId);
    }
  }
  Graph->Defrag();
  return Graph;
}

template <class PGraph>
PGraph LoadPajek(const TStr& InFNm) {
  PGraph Graph = PGraph::TObj::New();
  TSsParser Ss(InFNm, ssfSpaceSep, true, true);
  Ss.Next();  Ss.ToLc();
  EAssert(strstr(Ss[0], "*vertices") != NULL);
  while (Ss.Next()) {
    Ss.ToLc();
    if (strstr(Ss[0], "*arcs")!=NULL || strstr(Ss[0],"*edges")!=NULL) { break; }
    Graph->AddNode(Ss.GetInt(0));
  }
  // edges
  while (Ss.Next()) {
    Graph->AddEdge(Ss.GetInt(0), Ss.GetInt(1));
  }
  return Graph;
}

template <class PGraph>
void SaveEdgeList(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) { fprintf(F, "# Directed graph: %s \n", OutFNm.CStr()); } 
  else { fprintf(F, "# Undirected graph (each unordered pair of nodes is saved once): %s\n", OutFNm.CStr()); }
  if (! Desc.Empty()) { fprintf(F, "# %s\n", Desc.CStr()); }
  fprintf(F, "# Nodes: %d Edges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) { fprintf(F, "# FromNodeId\tToNodeId\n"); }
  else { fprintf(F, "# NodeId\tNodeId\n"); }
  for (typename PGraph::TObj::TEdgeI ei = Graph->BegEI(); ei < Graph->EndEI(); ei++) {
    fprintf(F, "%d\t%d\n", ei.GetSrcNId(), ei.GetDstNId());
  }
  fclose(F);
}

template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm) {
  TIntH NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %d\n", Graph->GetNodes());
  int i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%d  \"%d\" ic Red fos 10\n", i+1, NI.GetId()); // ic: internal color, fos: font size
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %d\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %d\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%d %d %d c Black\n", SrcNId, DstNId, 1); // width=1
  }
  fclose(F);
}

template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStrH& NIdClrH) {
  TIntH NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %d\n", Graph->GetNodes());
  int i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%d  \"%d\" ic %s fos 10\n", i+1, NI.GetId(),
      NIdClrH.IsKey(NI.GetId()) ? NIdClrH.GetDat(NI.GetId()).CStr() : "Red");
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %d\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %d\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%d %d %d c Black\n", SrcNId, DstNId, 1);
  }
  fclose(F);
}

template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStrH& NIdClrH, const TIntStrH& NIdLabelH) {
  TIntH NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %d\n", Graph->GetNodes());
  int i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%d  \"%s\" ic %s fos 10\n", i+1,
      NIdLabelH.IsKey(NI.GetId()) ? NIdLabelH.GetDat(NI.GetId()).CStr() : TStr::Fmt("%d", NI.GetId()).CStr(),
      NIdClrH.IsKey(NI.GetId()) ? NIdClrH.GetDat(NI.GetId()).CStr() : "Red");
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %d\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %d\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%d %d %d c Black\n", SrcNId, DstNId, 1);
  }
  fclose(F);
}

template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStrH& NIdClrH, const TIntStrH& NIdLabelH, const TIntStrH& EIdClrH) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfMultiGraph)); // network needs to have edge ids
  TIntH NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %d\n", Graph->GetNodes());
  int i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%d  \"%s\" ic %s fos 10\n", i+1,
      NIdLabelH.IsKey(NI.GetId()) ? NIdLabelH.GetDat(NI.GetId()).CStr() : TStr::Fmt("%d", NI.GetId()).CStr(),
      NIdClrH.IsKey(NI.GetId()) ? NIdClrH.GetDat(NI.GetId()).CStr() : "Red");
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %d\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %d\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%d %d 1 c %s\n", SrcNId, DstNId, 1,
      EIdClrH.IsKey(EI.GetId()) ? EIdClrH.GetDat(EI.GetId()).CStr() : "Black");
  }
  fclose(F);
}

template <class PGraph>
void SaveMatlabSparseMtx(const PGraph& Graph, const TStr& OutFNm) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  TIntSet NIdSet(Graph->GetNodes()); // so that
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdSet.AddKey(NI.GetId());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int Src = NIdSet.GetKeyId(EI.GetSrcNId())+1;
    const int Dst = NIdSet.GetKeyId(EI.GetDstNId())+1;
    fprintf(F, "%d\t%d\t1\n", Src, Dst);
    if (! HasGraphFlag(typename PGraph::TObj, gfDirected) && Src!=Dst) {
      fprintf(F, "%d\t%d\t1\n", Dst, Src);
    }
  }
  fclose(F);
}

} // namespace TSnap
