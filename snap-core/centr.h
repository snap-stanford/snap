namespace TSnap {

/////////////////////////////////////////////////
// Node centrality measures (See: http://en.wikipedia.org/wiki/Centrality)

/// Returns Degree centrality of a given node NId.
/// Degree centrality if a node is defined as its degree/(N-1), where N is the number of nodes in the network.
double GetDegreeCentr(const PUNGraph& Graph, const int& NId);
/// Returns Farness centrality of a given node NId. 
/// Farness centrality of a node is the average shortest path length to all other nodes that reside is the same connected component as the given node.
double GetFarnessCentr(const PUNGraph& Graph, const int& NId);
/// Returns Closeness centrality of a given node NId.
/// Closeness centrality of a node is defined as 1/FarnessCentrality.
double GetClosenessCentr(const PUNGraph& Graph, const int& NId);
/// Returns node Eccentricity, the largest shortest-path distance from the node NId to any other node in the Graph.
/// @param IsDir false: ignore edge directions and consider edges as undirected (in case they are directed).
template <class PGraph> int GetNodeEcc(const PGraph& Graph, const int& NId, const bool& IsDir=false);

/// Computes (approximate) Node Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param NIdBtwH hash table mapping node ids to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
void GetBetweennessCentr(const PUNGraph& Graph, TIntFltH& NIdBtwH, const double& NodeFrac=1.0);
/// Computes (approximate) Edge Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param EdgeBtwH hash table mapping edges (pairs of node ids) to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
void GetBetweennessCentr(const PUNGraph& Graph, TIntPrFltH& EdgeBtwH, const double& NodeFrac=1.0);
/// Computes (approximate) Node and Edge Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param NIdBtwH hash table mapping node ids to their corresponding betweenness centrality values.
/// @param EdgeBtwH hash table mapping edges (pairs of node ids) to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
void GetBetweennessCentr(const PUNGraph& Graph, TIntFltH& NIdBtwH, TIntPrFltH& EdgeBtwH, const double& NodeFrac=1.0);
/// Computes (approximate) Beetweenness Centrality of all nodes and all edges of the network.
/// To obtain exact betweenness values one needs to solve single-source shortest-path problem for every node.
/// To speed up the algorithm we solve the shortest-path problem for the BtwNIdV subset of nodes. This gives centrality values that are about Graph->GetNodes()/BtwNIdV.Len() times lower than the exact betweenness centrality valus.
/// See "A Faster Algorithm for Beetweenness Centrality", Ulrik Brandes, Journal of Mathematical Sociology, 2001, and 
/// "Centrality Estimation in Large Networks", Urlik Brandes and Christian Pich, 2006 for more details.
void GetBetweennessCentr(const PUNGraph& Graph, const TIntV& BtwNIdV, TIntFltH& NodeBtwH, const bool& DoNodeCent, TIntPrFltH& EdgeBtwH, const bool& DoEdgeCent);

/// Computes Eigenvector Centrality of all nodes in the network
/// Eigenvector Centrality of a node N is defined recursively as the average of centrality values of N's neighbors in the network.
void GetEigenVectorCentr(const PUNGraph& Graph, TIntFltH& NIdEigenH, const double& Eps=1e-4, const int& MaxIter=100);

/// PageRank
/// For more info see: http://en.wikipedia.org/wiki/PageRank
template<class PGraph> void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
/// HITS: Hubs and Authorities
/// For more info see: http://en.wikipedia.org/wiki/HITS_algorithm)
template<class PGraph> void GetHits(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter=20);

/////////////////////////////////////////////////
// Implementation 
template <class PGraph>
int GetNodeEcc(const PGraph& Graph, const int& NId, const bool& IsDir) {
  int NodeEcc;
  int Dist;
  TBreathFS<PGraph> BFS(Graph);
  // get shortest paths to all the nodes
  BFS.DoBfs(NId, true, ! IsDir, -1, TInt::Mx);

  NodeEcc = 0;
  // find the largest value
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    Dist = BFS.NIdDistH[i];
    if (Dist > NodeEcc) {
      NodeEcc = Dist;
    }
  }
  return NodeEcc;
}

// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
template<class PGraph>
void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  //const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    //IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }
  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    int j = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int OutDeg = Graph->GetNI(InNId).GetOutDeg();
        if (OutDeg > 0) {
          TmpV[j] += PRankH.GetDat(InNId) / OutDeg; }
      }
      TmpV[j] =  C*TmpV[j]; // Berkhin (the correct way of doing it)
      //TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }
    double diff=0, sum=0, NewVal;
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);
    for (int i = 0; i < PRankH.Len(); i++) { // re-instert leaked PageRank
      NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      diff += fabs(NewVal-PRankH[i]);
      PRankH[i] = NewVal;
    }
    if (diff < Eps) { break; }
  }
}

template<class PGraph>
void GetHits(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  NIdHubH.Gen(NNodes);
  NIdAuthH.Gen(NNodes);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdHubH.AddDat(NI.GetId(), 1.0);
    NIdAuthH.AddDat(NI.GetId(), 1.0);
  }
  double Norm=0;
  for (int iter = 0; iter < MaxIter; iter++) {
    // update authority scores
    Norm = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      double& Auth = NIdAuthH.GetDat(NI.GetId()).Val;
      Auth = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        Auth +=  NIdHubH.GetDat(NI.GetInNId(e)); }
      Norm += Auth*Auth;
    }
    Norm = sqrt(Norm);
    for (int i = 0; i < NIdAuthH.Len(); i++) { NIdAuthH[i] /= Norm; }
    // update hub scores
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      double& Hub = NIdHubH.GetDat(NI.GetId()).Val;
      Hub = 0;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        Hub += NIdAuthH.GetDat(NI.GetOutNId(e)); }
      Norm += Hub*Hub;
    }
    Norm = sqrt(Norm);
    for (int i = 0; i < NIdHubH.Len(); i++) { NIdHubH[i] /= Norm; }
  }
}

}; // namespace TSnap
