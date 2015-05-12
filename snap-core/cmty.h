namespace TSnap {

/////////////////////////////////////////////////
// Modularity
/// Computes Modularity score of a set of nodes NIdV in a graph G.
/// The function runs much faster if the number of edges in graph G is given (GEdges parameter).
template<typename PGraph> double GetModularity(const PGraph& G, const TIntV& NIdV, int GEdges=-1);
/// Computes Modularity score of a set of communities (each community is defined by its member nodes) in a graph G.
/// The function runs much faster if the number of edges in graph G is given (GEdges parameter).
template<typename PGraph> double GetModularity(const PGraph& G, const TCnComV& CmtyV, int GEdges=-1);
/// Returns the number of edges between the nodes NIdV and the edges pointing outside the set NIdV.
/// @param EdgesInX Number of edges between the nodes NIdV.
/// @param EdgesOutX Number of edges between the nodes in NIdV and the rest of the graph.
template<typename PGraph> void GetEdgesInOut(const PGraph& Graph, const TIntV& NIdV, int& EdgesInX, int& EdgesOutX);

/// Girvan-Newman community detection algorithm based on Betweenness centrality.
/// See: Girvan M. and Newman M. E. J., Community structure in social and biological networks, Proc. Natl. Acad. Sci. USA 99, 7821-7826 (2002)
double CommunityGirvanNewman(PUNGraph& Graph, TCnComV& CmtyV);

/// Clauset-Newman-Moore community detection method for large networks.
/// At every step of the algorithm two communities that contribute maximum positive value to global modularity are merged.
/// See: Finding community structure in very large networks, A. Clauset, M.E.J. Newman, C. Moore, 2004
double CommunityCNM(const PUNGraph& Graph, TCnComV& CmtyV);

/// Rosvall-Bergstrom community detection algorithm based on information theoretic approach.
/// See: Rosvall M., Bergstrom C. T., Maps of random walks on complex networks reveal community structure, Proc. Natl. Acad. Sci. USA 105, 1118-1123 (2008)
double Infomap(PUNGraph& Graph, TCnComV& CmtyV);

double InfomapOnline(PUNGraph& Graph, int n1, int n2, TIntFltH& PAlpha, double& SumPAlphaLogPAlpha, TIntFltH& Qi, TIntH& Module, int& Br, TCnComV& CmtyV);

void CmtyEvolutionFileBatch(TStr InFNm, TIntIntHH& sizesCont, TIntIntHH& cCont, TIntIntVH& edges, double alpha, double beta, int CmtyAlg);
void CmtyEvolutionFileBatchV(TStr InFNm, TIntIntVH& sizesContV, TIntIntVH& cContV, TIntIntVH& edges, double alpha, double beta, int CmtyAlg);
void CmtyEvolutionJson(TStr& Json, TIntIntVH& sizesContV, TIntIntVH& cContV, TIntIntVH& edges);
TStr CmtyTest(TStr t, int CmtyAlg);
void ReebSimplify(PNGraph& Graph, TIntH& t, int e, PNGraph& gFinal, TIntH& tFinal, bool collapse);
void ReebRefine(PNGraph& Graph, TIntH& t, int e, PNGraph& gFinal, TIntH& tFinal, bool collapse);

namespace TSnapDetail {
/// A single step of Girvan-Newman clustering procedure.
void CmtyGirvanNewmanStep(PUNGraph& Graph, TIntV& Cmty1, TIntV& Cmty2);
}

/////////////////////////////////////////////////
// Implementation
template<typename PGraph>
double GetModularity(const PGraph& Graph, const TIntV& NIdV, int GEdges) {
  if (GEdges == -1) { GEdges = Graph->GetEdges(); }
  double EdgesIn = 0.0, EEdgesIn = 0.0; // EdgesIn=2*number of edges inside the cluster, EEdgesIn=expected edges inside
  TIntSet NIdSet(NIdV.Len());
  for (int e = 0; e < NIdV.Len(); e++) { // edges inside
    NIdSet.AddKey(NIdV[e]);
  }
  for (int e1 = 0; e1 < NIdV.Len(); e1++) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NIdV[e1]);
    EEdgesIn += NI.GetOutDeg();
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (NIdSet.IsKey(NI.GetOutNId(i))) { EdgesIn += 1; }
    }
  }
  EEdgesIn = EEdgesIn*EEdgesIn / (2.0*GEdges);
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
  EdgesIn = 0;
  EdgesOut = 0;
  TIntSet NIdSet(NIdV.Len());
  for (int e = 0; e < NIdV.Len(); e++) {
    NIdSet.AddKey(NIdV[e]);
  }
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
