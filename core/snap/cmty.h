namespace TSnap {

/////////////////////////////////////////////////
// Modularity
template<typename PGraph> double GetModularity(const PGraph& G, const TIntV& NIdV, int GEdges=-1);
template<typename PGraph> double GetModularity(const PGraph& G, const TCnComV& CmtyV, int GEdges=-1);
template<typename PGraph> void GetEdgesInOut(const PGraph& Graph, const TIntV& NIdV, int& EdgesIn, int& EdgesOut);

/////////////////////////////////////////////////
// Community detection algorithms
void CmtyGirvanNewmanStep(PUNGraph& Graph, TIntV& Cmty1, TIntV& Cmty2);
double CommunityGirvanNewman(PUNGraph& Graph, TCnComV& CmtyV);
double CommunityCNM(const PUNGraph& Graph, TCnComV& CmtyV);

/////////////////////////////////////////////////
// Implementation
template<typename PGraph>
double GetModularity(const PGraph& Graph, const TIntV& NIdV, int GEdges) {
  if (GEdges == -1) { GEdges = Graph->GetEdges(); }
  double EdgesIn = 0.0, EEdgesIn = 0.0; // EdgesIn=2*number of edges inside the cluster, EEdgesIn=expected edges inside
  TIntSet NIdSet(NIdV.Len());
  for (int e = 0; e < NIdV.Len(); e++) { // edges inside
    NIdSet.AddKey(NIdV[e]); }
  for (int e1 = 0; e1 < NIdV.Len(); e1++) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NIdV[e1]);
    EEdgesIn += NI.GetOutDeg();
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (NIdSet.IsKey(NI.GetOutNId(i))) { EdgesIn += 1; }
    }
  }
  EEdgesIn = EEdgesIn*EEdgesIn/(2.0*GEdges);
  if ((EdgesIn - EEdgesIn) == 0) { return 0; }
  else { return (EdgesIn - EEdgesIn) / (2.0*GEdges); } // modularity
}

template<typename PGraph>
double GetModularity(const PGraph& G, const TCnComV& CmtyV, int GEdges) {
  if (GEdges == -1) { GEdges = G->GetEdges(); }
  double Modularity = 0;
  for (int c = 0; c < CmtyV.Len(); c++) {
    Modularity += GetModularity(G, CmtyV[c](), GEdges);
  }
  return Modularity;
}

template<typename PGraph>
void GetEdgesInOut(const PGraph& Graph, const TIntV& NIdV, int& EdgesIn, int& EdgesOut) {
  EdgesIn=0;
  EdgesOut=0;
  TIntSet NIdSet(NIdV.Len());
  for (int e = 0; e < NIdV.Len(); e++) {
    NIdSet.AddKey(NIdV[e]); }
  for (int e = 0; e < NIdV.Len(); e++) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NIdV[e]);
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (NIdSet.IsKey(NI.GetOutNId(i))) { EdgesIn += 1; }
      else { EdgesOut += 1; }
    }
  }
  EdgesIn /= 2;
}

}; // namespace TSnap

