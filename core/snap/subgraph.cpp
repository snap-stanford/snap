namespace TSnap {

/////////////////////////////////////////////////
// Graph Algorithms

// renumber node ids to 0...N-1
PUNGraph GetSubGraph(const PUNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes) {
  if (! RenumberNodes) { return TSnap::GetSubGraph(Graph, NIdV); }
  PUNGraph NewGraphPt = TUNGraph::New();
  TUNGraph& NewGraph = *NewGraphPt;
  NewGraph.Reserve(NIdV.Len(), -1);
  TIntSet NIdSet(NIdV.Len());
  for (int n = 0; n < NIdV.Len(); n++) {
    NewGraph.AddNode(n);
    NIdSet.AddKey(NIdV[n]);
  }
  for (int n = 0; n < NIdV.Len(); n++) {
    const TUNGraph::TNodeI NI = Graph->GetNI(NIdV[n]);
    const int SrcNId = NIdSet.GetKeyId(NI.GetId());
    for (int edge = 0; edge < NI.GetDeg(); edge++) {
      const int OutNId = NIdSet.GetKeyId(NI.GetNbhNId(edge));
      if (NewGraph.IsNode(OutNId)) {
        NewGraph.AddEdge(SrcNId, OutNId); }
    }
  }
  return NewGraphPt;
}

// renumber node ids to 0...N-1
PNGraph GetSubGraph(const PNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes) {
  if (! RenumberNodes) { return TSnap::GetSubGraph(Graph, NIdV); }
  PNGraph NewGraphPt = TNGraph::New();
  TNGraph& NewGraph = *NewGraphPt;
  NewGraph.Reserve(NIdV.Len(), -1);
  TIntSet NIdSet(NIdV.Len());
  for (int n = 0; n < NIdV.Len(); n++) {
    NewGraph.AddNode(n);
    NIdSet.AddKey(NIdV[n]);
  }
  for (int n = 0; n < NIdV.Len(); n++) {
    const TNGraph::TNodeI NI = Graph->GetNI(NIdV[n]);
    const int SrcNId = NIdSet.GetKeyId(NI.GetId());
    for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
      const int OutNId = NIdSet.GetKeyId(NI.GetOutNId(edge));
      if (NewGraph.IsNode(OutNId)) {
        NewGraph.AddEdge(SrcNId, OutNId); }
    }
  }
  return NewGraphPt;
}


} // namespace TSnap
