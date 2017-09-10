namespace TSnap {

/////////////////////////////////////////////////
// Node centrality measures (See: http://en.wikipedia.org/wiki/Centrality)

/// Returns Degree centrality of a given node NId.
/// Degree centrality if a node is defined as its degree/(N-1), where N is the number of nodes in the network.
double GetDegreeCentr(const PUNGraph& Graph, const int& NId);
/// Returns Group Degree centrality of a given group NId.
/// Degree centrality if a node is defined as its degree/(N-1), where N is the number of nodes in the network.
//double GetGroupDegreeCentr(const PUNGraph& Graph, const PUNGraph& Group);
double GetGroupDegreeCentr(const PUNGraph& Graph, const TIntH& GroupNodes);
/// Returns Group Degree centrality of a given group NId.
/// Degree centrality if a node is defined as its degree/(N-1), where N is the number of nodes in the network.
//double GetGroupDegreeCentr(const PUNGraph& Graph, const PUNGraph& Group);
double GetGroupClosenessCentr(const PUNGraph& Graph, const TIntH& GroupNodes);
/// Returns centrality Maximum k group.
TIntH MaxCPGreedyBetter(const PUNGraph& Graph, const int k);
/// Returns centrality Maximum k group.
TIntH MaxCPGreedyBetter1(const PUNGraph& Graph, const int k);
/// Returns centrality Maximum k group.
TIntH MaxCPGreedyBetter2(const PUNGraph& Graph, const int k);
/// Returns centrality Maximum k group.
TIntH MaxCPGreedyBetter3(const PUNGraph& Graph, const int k);
/// Event importance
TIntFltH EventImportance(const PNGraph& Graph, const int k);
/// Intersect
int Intersect(TUNGraph::TNodeI Node, TIntH NNodes);
/// Intersect
int Intersect(TUNGraph::TNodeI Node, TStr NNodes);
/// Intersect
int Intersect(TUNGraph::TNodeI Node, int *NNodes, int NNodes_br);
//Load nodes list
int Intersect1(TUNGraph::TNodeI Node, TStr NNodes);
//Load nodes list
TIntH LoadNodeList(TStr InFNmNodes);
/// Returns Farness centrality of a given node NId.
/// Farness centrality of a node is the average shortest path length to all other nodes that reside is the same connected component as the given node.
template <class PGraph> double GetFarnessCentr(const PGraph& Graph, const int& NId, const bool& Normalized=true, const bool& IsDir=false);

template <class PGraph> double GetFarnessCentrMP(const PGraph& Graph, const int& NId, const bool& Normalized=true, const bool& IsDir=false);

/// Returns weighted Farness centrality of a given node \c NId.
/// Farness centrality of a node is the average shortest path length to all other nodes that reside is the same connected component as the given node.
double GetWeightedFarnessCentr(const PNEANet Graph, const int& NId, const TFltV& Attr, const bool& Normalized=true, const bool& IsDir=false);

/// Returns Closeness centrality of a given node NId.
/// Closeness centrality of a node is defined as 1/FarnessCentrality.
template <class PGraph> double GetClosenessCentr(const PGraph& Graph, const int& NId, const bool& Normalized=true, const bool& IsDir=false);
template <class PGraph> double GetClosenessCentrMP(const PGraph& Graph, const int& NId, const bool& Normalized=true, const bool& IsDir=false);
/// Returns Closeness centrality of a given node \c NId. 
/// Closeness centrality of a node is defined as 1/FarnessCentrality.
double GetWeightedClosenessCentr(const PNEANet Graph, const int& NId, const TFltV& Attr, const bool& Normalized=true, const bool& IsDir=false);
/// Returns node Eccentricity, the largest shortest-path distance from the node NId to any other node in the Graph.
/// @param IsDir false: ignore edge directions and consider edges as undirected (in case they are directed).
template <class PGraph> int GetNodeEcc(const PGraph& Graph, const int& NId, const bool& IsDir=false);

/// Computes (approximate) Node Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param NIdBtwH hash table mapping node ids to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
template<class PGraph> void GetBetweennessCentr(const PGraph& Graph, TIntFltH& NIdBtwH, const double& NodeFrac=1.0, const bool& IsDir=false);
/// Computes (approximate) weighted Node Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param NIdBtwH hash table mapping node ids to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
void GetWeightedBetweennessCentr(const PNEANet Graph, TIntFltH& NIdBtwH, const TFltV& Attr, const double& NodeFrac=1.0, const bool& IsDir=false);
/// Computes (approximate) Edge Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param EdgeBtwH hash table mapping edges (pairs of node ids) to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
template<class PGraph> void GetBetweennessCentr(const PGraph& Graph, TIntPrFltH& EdgeBtwH, const double& NodeFrac=1.0, const bool& IsDir=false);
/// Computes (approximate) weighted Edge Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param EdgeBtwH hash table mapping edges (pairs of node ids) to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
void GetWeightedBetweennessCentr(const PNEANet Graph, TIntPrFltH& EdgeBtwH, const TFltV& Attr, const double& NodeFrac=1.0, const bool& IsDir=false);
/// Computes (approximate) Node and Edge Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param NIdBtwH hash table mapping node ids to their corresponding betweenness centrality values.
/// @param EdgeBtwH hash table mapping edges (pairs of node ids) to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
template<class PGraph> void GetBetweennessCentr(const PGraph& Graph, TIntFltH& NIdBtwH, TIntPrFltH& EdgeBtwH, const double& NodeFrac=1.0, const bool& IsDir=false);
/// Computes (approximate) weighted Node and Edge Beetweenness Centrality based on a sample of NodeFrac nodes.
/// @param NIdBtwH hash table mapping node ids to their corresponding betweenness centrality values.
/// @param EdgeBtwH hash table mapping edges (pairs of node ids) to their corresponding betweenness centrality values.
/// @param NodeFrac quality of approximation. NodeFrac=1.0 gives exact betweenness values.
void GetWeightedBetweennessCentr(const PNEANet Graph, TIntFltH& NIdBtwH, TIntPrFltH& EdgeBtwH, const TFltV& Attr, const double& NodeFrac=1.0, const bool& IsDir=false);
/// Computes (approximate) Beetweenness Centrality of all nodes and all edges of the network.
/// To obtain exact betweenness values one needs to solve single-source shortest-path problem for every node.
/// To speed up the algorithm we solve the shortest-path problem for the BtwNIdV subset of nodes. This gives centrality values that are about Graph->GetNodes()/BtwNIdV.Len() times lower than the exact betweenness centrality valus.
/// See "A Faster Algorithm for Beetweenness Centrality", Ulrik Brandes, Journal of Mathematical Sociology, 2001, and
/// "Centrality Estimation in Large Networks", Urlik Brandes and Christian Pich, 2006 for more details.
template<class PGraph> void GetBetweennessCentr(const PGraph& Graph, const TIntV& BtwNIdV, TIntFltH& NodeBtwH, const bool& DoNodeCent, TIntPrFltH& EdgeBtwH, const bool& DoEdgeCent, const bool& IsDir);
/// Computes (approximate) weighted Beetweenness Centrality of all nodes and all edges of the network.
void GetWeightedBetweennessCentr(const PNEANet Graph, const TIntV& BtwNIdV, TIntFltH& NodeBtwH, const bool& DoNodeCent, TIntPrFltH& EdgeBtwH, const bool& DoEdgeCent, const TFltV& Attr, const bool& IsDir);
/// Computes Eigenvector Centrality of all nodes in the network
/// Eigenvector Centrality of a node N is defined recursively as the average of centrality values of N's neighbors in the network.
void GetEigenVectorCentr(const PUNGraph& Graph, TIntFltH& NIdEigenH, const double& Eps=1e-4, const int& MaxIter=100);

/// PageRank
/// For more info see: http://en.wikipedia.org/wiki/PageRank
template<class PGraph> void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
template<class PGraph> void GetPageRank_v1(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
#ifdef USE_OPENMP
template<class PGraph> void GetPageRankMP(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
#endif

/// Weighted PageRank (TODO: Use template)
int GetWeightedPageRank(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
#ifdef USE_OPENMP
int GetWeightedPageRankMP(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
#endif

/// HITS: Hubs and Authorities
/// For more info see: http://en.wikipedia.org/wiki/HITS_algorithm)
template<class PGraph> void GetHits(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter=20);
#ifdef USE_OPENMP
template<class PGraph> void GetHitsMP(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter=20);
#endif

/// Dijkstra Algorithm
/// For more info see:  https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
int GetWeightedShortestPath(const PNEANet Graph, const int& SrcNId, TIntFltH& NIdDistH, const TFltV& Attr);
/////////////////////////////////////////////////
// Implementation
template <class PGraph>
double GetFarnessCentr(const PGraph& Graph, const int& NId, const bool& Normalized, const bool& IsDir) {
  TIntH NDistH(Graph->GetNodes());
  TSnap::GetShortPath<PGraph>(Graph, NId, NDistH, IsDir, TInt::Mx);  
  
  double sum = 0;
  for (TIntH::TIter I = NDistH.BegI(); I < NDistH.EndI(); I++) {
    sum += I->Dat();
  }
  if (NDistH.Len() > 1) { 
    double centr = sum/double(NDistH.Len()-1); 
    if (Normalized) {
      centr *= (Graph->GetNodes() - 1)/double(NDistH.Len()-1);
    }
    return centr;
  }
  else { return 0.0; }
}

template <class PGraph>
double GetFarnessCentrMP(const PGraph& Graph, const int& NId, const bool& Normalized, const bool& IsDir) {
  TIntH NDistH(Graph->GetNodes());
  TSnap::GetShortPath<PGraph>(Graph, NId, NDistH, IsDir, TInt::Mx);  
  
  double sum = 0;
  for (TIntH::TIter I = NDistH.BegI(); I < NDistH.EndI(); I++) {
    sum += I->Dat();
  }
  if (NDistH.Len() > 1) { 
    double centr = sum/double(NDistH.Len()-1); 
    if (Normalized) {
      centr *= (Graph->GetNodes() - 1)/double(NDistH.Len()-1);
    }
    return centr;
  }
  else { return 0.0; }
}

template <class PGraph>
double GetClosenessCentr(const PGraph& Graph, const int& NId, const bool& Normalized, const bool& IsDir) {
  const double Farness = GetFarnessCentr<PGraph> (Graph, NId, Normalized, IsDir);
  if (Farness != 0.0) { return 1.0/Farness; }
  else { return 0.0; }
  return 0.0;
}

template <class PGraph>
double GetClosenessCentrMP(const PGraph& Graph, const int& NId, const bool& Normalized, const bool& IsDir) {
  const double Farness = GetFarnessCentrMP<PGraph> (Graph, NId, Normalized, IsDir);
  if (Farness != 0.0) { return 1.0/Farness; }
  else { return 0.0; }
  return 0.0;
}

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
// This implementation is an unoptimized version, it accesses nodes via a hash table.
template<class PGraph>
void GetPageRank_v1(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
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

// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
// This implementation is an optimized version, it builds a vector and accesses nodes via the vector.
template<class PGraph>
void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TVec<typename PGraph::TObj::TNodeI> NV;
  PRankH.Gen(NNodes);
  int MxId = -1;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    int Id = NI.GetId();
    if (Id > MxId) {
      MxId = Id;
    }
  }

  TFltV PRankV(MxId+1);
  TIntV OutDegV(MxId+1);

  for (int j = 0; j < NNodes; j++) {
    typename PGraph::TObj::TNodeI NI = NV[j];
    int Id = NI.GetId();
    PRankV[Id] = 1.0/NNodes;
    OutDegV[Id] = NI.GetOutDeg();
  }

  TFltV TmpV(NNodes);

  for (int iter = 0; iter < MaxIter; iter++) {
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int OutDeg = OutDegV[InNId];
        if (OutDeg > 0) {
          Tmp += PRankV[InNId] / OutDeg;
        }
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
    }
    double sum = 0;
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);

    double diff = 0;
    for (int i = 0; i < NNodes; i++) {
      typename PGraph::TObj::TNodeI NI = NV[i];
      double NewVal = TmpV[i] + Leaked; // Berkhin
      int Id = NI.GetId();
      diff += fabs(NewVal-PRankV[Id]);
      PRankV[Id] = NewVal;
    }
    if (diff < Eps) { break; }
  }

  for (int i = 0; i < NNodes; i++) {
    typename PGraph::TObj::TNodeI NI = NV[i];
    PRankH[i] = PRankV[NI.GetId()];
  }
}

#ifdef USE_OPENMP
// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
// This is a parallel, optimized version.
template<class PGraph>
void GetPageRankMP(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TVec<typename PGraph::TObj::TNodeI> NV;
  PRankH.Gen(NNodes);

  int MxId = -1;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    int Id = NI.GetId();
    if (Id > MxId) {
      MxId = Id;
    }
  }

  TFltV PRankV(MxId+1);
  TIntV OutDegV(MxId+1);

  #pragma omp parallel for schedule(dynamic,10000)
  for (int j = 0; j < NNodes; j++) {
    typename PGraph::TObj::TNodeI NI = NV[j];
    int Id = NI.GetId();
    PRankV[Id] = 1.0/NNodes;
    OutDegV[Id] = NI.GetOutDeg();
  }

  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int OutDeg = OutDegV[InNId];
        if (OutDeg > 0) {
          Tmp += PRankV[InNId] / OutDeg;
        }
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
    }

    double sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);

    double diff = 0;
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < NNodes; i++) {
      double NewVal = TmpV[i] + Leaked; // Berkhin
      int Id = NV[i].GetId();
      diff += fabs(NewVal-PRankV[Id]);
      PRankV[Id] = NewVal;
    }
    if (diff < Eps) { break; }
  }

  #pragma omp parallel for schedule(dynamic,10000)
  for (int i = 0; i < NNodes; i++) {
    typename PGraph::TObj::TNodeI NI = NV[i];
    PRankH[i] = PRankV[NI.GetId()];
  }
}
#endif // USE_OPENMP

// Betweenness Centrality
template<class PGraph>
void GetBetweennessCentr(const PGraph& Graph, const TIntV& BtwNIdV, TIntFltH& NodeBtwH, const bool& DoNodeCent, TIntPrFltH& EdgeBtwH, const bool& DoEdgeCent, const bool& IsDir) {
  if (DoNodeCent) { NodeBtwH.Clr(); }
  if (DoEdgeCent) { EdgeBtwH.Clr(); }
  const int nodes = Graph->GetNodes();
  TIntS S(nodes);
  TIntQ Q(nodes);
  TIntIntVH P(nodes); // one vector for every node
  TIntFltH delta(nodes);
  TIntH sigma(nodes), d(nodes);
  // init
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (DoNodeCent) {
      NodeBtwH.AddDat(NI.GetId(), 0); }
    if (DoEdgeCent) {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        if (Graph->HasFlag(gfDirected) && IsDir) {
          // add all outgoing edges for directed graphs
          EdgeBtwH.AddDat(TIntPr(NI.GetId(), NI.GetOutNId(e)), 0); 
        } else {
          // add each edge only once in undirected graphs
          if (NI.GetId() < NI.GetOutNId(e)) {
            EdgeBtwH.AddDat(TIntPr(NI.GetId(), NI.GetOutNId(e)), 0); 
          }
        }
      }
      // add incoming edges in directed graphs that were not added yet
      if (Graph->HasFlag(gfDirected) && !IsDir) {
        for (int e = 0; e < NI.GetInDeg(); e++) {
          if (NI.GetId() < NI.GetInNId(e)  &&
              !Graph->IsEdge(NI.GetId(), NI.GetInNId(e))) {
            EdgeBtwH.AddDat(TIntPr(NI.GetId(), NI.GetInNId(e)), 0); 
          }
        }
      }
    }
    sigma.AddDat(NI.GetId(), 0);
    d.AddDat(NI.GetId(), -1);
    P.AddDat(NI.GetId(), TIntV());
    delta.AddDat(NI.GetId(), 0);
  }
  // calc betweeness
  for (int k=0; k < BtwNIdV.Len(); k++) {
    const typename PGraph::TObj::TNodeI NI = Graph->GetNI(BtwNIdV[k]);
    // reset
    for (int i = 0; i < sigma.Len(); i++) {
      sigma[i]=0;  d[i]=-1;  delta[i]=0;  P[i].Clr(false);
    }
    S.Clr(false);
    Q.Clr(false);
    sigma.AddDat(NI.GetId(), 1);
    d.AddDat(NI.GetId(), 0);
    Q.Push(NI.GetId());
    while (! Q.Empty()) {
      const int v = Q.Top();  Q.Pop();
      const typename PGraph::TObj::TNodeI NI2 = Graph->GetNI(v);
      S.Push(v);
      const int VDat = d.GetDat(v);
      // iterate over all outgoing edges
      for (int e = 0; e < NI2.GetOutDeg(); e++) {
        const int w = NI2.GetOutNId(e);
        if (d.GetDat(w) < 0) { // find w for the first time
          Q.Push(w);
          d.AddDat(w, VDat+1);
        }
        //shortest path to w via v ?
        if (d.GetDat(w) == VDat+1) {
          sigma.AddDat(w) += sigma.GetDat(v);
          P.GetDat(w).Add(v);
        }
      }
      // if ignoring direction in directed networks, iterate over incoming edges
      if (Graph->HasFlag(gfDirected) && !IsDir) {
        for (int e = 0; e < NI2.GetInDeg(); e++) {
          const int w = NI2.GetInNId(e);
          // skip neighbors that are also outgoing
          if (Graph->IsEdge(NI2.GetId(), w)) {
            continue;
          }
          if (d.GetDat(w) < 0) { // find w for the first time
            Q.Push(w);
            d.AddDat(w, VDat+1);
          }
          //shortest path to w via v ?
          if (d.GetDat(w) == VDat+1) {
            sigma.AddDat(w) += sigma.GetDat(v);
            P.GetDat(w).Add(v);
          }
        }
      }
    }
    while (! S.Empty()) {
      const int w = S.Top();
      const double SigmaW = sigma.GetDat(w);
      const double DeltaW = delta.GetDat(w);
      const TIntV NIdV = P.GetDat(w);
      S.Pop();
      for (int i = 0; i < NIdV.Len(); i++) {
        const int NId = NIdV[i];
        const double c = (sigma.GetDat(NId)*1.0/SigmaW) * (1+DeltaW);
        delta.AddDat(NId) += c;
        if (DoEdgeCent) {
          if (Graph->HasFlag(gfDirected) && IsDir) {
            EdgeBtwH.AddDat(TIntPr(NId, w)) += c;
          } else {
            EdgeBtwH.AddDat(TIntPr(TMath::Mn(NId, w), TMath::Mx(NId, w))) += c;
          }
        }
      }
      if (DoNodeCent && w != NI.GetId()) {
        NodeBtwH.AddDat(w) += delta.GetDat(w)/2.0; }
    }
  }
}

template<class PGraph>
void GetBetweennessCentr(const PGraph& Graph, TIntFltH& NodeBtwH, const double& NodeFrac, const bool& IsDir) {
  TIntPrFltH EdgeBtwH;
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  if (NodeFrac < 1.0) { // calculate beetweenness centrality for a subset of nodes
    NIdV.Shuffle(TInt::Rnd);
    for (int i = int((1.0-NodeFrac)*NIdV.Len()); i > 0; i--) {
      NIdV.DelLast(); }
  }
  GetBetweennessCentr<PGraph> (Graph, NIdV, NodeBtwH, true, EdgeBtwH, false, IsDir);
}

template<class PGraph>
void GetBetweennessCentr(const PGraph& Graph, TIntPrFltH& EdgeBtwH, const double& NodeFrac, const bool& IsDir) {
  TIntFltH NodeBtwH;
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  if (NodeFrac < 1.0) { // calculate beetweenness centrality for a subset of nodes
    NIdV.Shuffle(TInt::Rnd);
    for (int i = int((1.0-NodeFrac)*NIdV.Len()); i > 0; i--) {
      NIdV.DelLast(); }
  }
  GetBetweennessCentr<PGraph> (Graph, NIdV, NodeBtwH, false, EdgeBtwH, true, IsDir);
}

template<class PGraph>
void GetBetweennessCentr(const PGraph& Graph, TIntFltH& NodeBtwH, TIntPrFltH& EdgeBtwH, const double& NodeFrac, const bool& IsDir) {
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  if (NodeFrac < 1.0) { // calculate beetweenness centrality for a subset of nodes
    NIdV.Shuffle(TInt::Rnd);
    for (int i = int((1.0-NodeFrac)*NIdV.Len()); i > 0; i--) {
      NIdV.DelLast(); }
  }
  GetBetweennessCentr<PGraph> (Graph, NIdV, NodeBtwH, true, EdgeBtwH, true, IsDir);
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
  // make sure Hub and Authority scores normalize to L2 norm 1
  Norm = 0.0;
  for (int i = 0; i < NIdHubH.Len(); i++) { Norm += TMath::Sqr(NIdHubH[i]); }
  Norm = sqrt(Norm);
  for (int i = 0; i < NIdHubH.Len(); i++) { NIdHubH[i] /= Norm; }
  Norm = 0.0;
  for (int i = 0; i < NIdAuthH.Len(); i++) { Norm += TMath::Sqr(NIdAuthH[i]); }
  Norm = sqrt(Norm);
  for (int i = 0; i < NIdAuthH.Len(); i++) { NIdAuthH[i] /= Norm; }
}

#ifdef USE_OPENMP
template<class PGraph>
void GetHitsMP(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TIntV NV;
  NIdHubH.Gen(NNodes);
  NIdAuthH.Gen(NNodes);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI.GetId());
    NIdHubH.AddDat(NI.GetId(), 1.0);
    NIdAuthH.AddDat(NI.GetId(), 1.0);
  }
  double Norm=0;
  for (int iter = 0; iter < MaxIter; iter++) {
    // update authority scores
    Norm = 0;
    #pragma omp parallel for reduction(+:Norm) schedule(dynamic,1000)
    for (int i = 0; i < NNodes; i++) {
      typename PGraph::TObj::TNodeI NI = Graph->GetNI(NV[i]);
      double& Auth = NIdAuthH.GetDat(NI.GetId()).Val;
      Auth = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        Auth +=  NIdHubH.GetDat(NI.GetInNId(e)); }
      Norm = Norm + Auth*Auth;
    }
    Norm = sqrt(Norm);
    for (int i = 0; i < NIdAuthH.Len(); i++) { NIdAuthH[i] /= Norm; }
    // update hub scores
    #pragma omp parallel for reduction(+:Norm) schedule(dynamic,1000)
    for (int i = 0; i < NNodes; i++) {
      typename PGraph::TObj::TNodeI NI = Graph->GetNI(NV[i]);
      double& Hub = NIdHubH.GetDat(NI.GetId()).Val;
      Hub = 0;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        Hub += NIdAuthH.GetDat(NI.GetOutNId(e)); }
      Norm = Norm + Hub*Hub;
    }
    Norm = sqrt(Norm);
    for (int i = 0; i < NIdHubH.Len(); i++) { NIdHubH[i] /= Norm; }
  }
  // make sure Hub and Authority scores normalize to L2 norm 1
  Norm = 0.0;
  for (int i = 0; i < NIdHubH.Len(); i++) { Norm += TMath::Sqr(NIdHubH[i]); }
  Norm = sqrt(Norm);
  for (int i = 0; i < NIdHubH.Len(); i++) { NIdHubH[i] /= Norm; }
  Norm = 0.0;
  for (int i = 0; i < NIdAuthH.Len(); i++) { Norm += TMath::Sqr(NIdAuthH[i]); }
  Norm = sqrt(Norm);
  for (int i = 0; i < NIdAuthH.Len(); i++) { NIdAuthH[i] /= Norm; }
}
#endif

/// Gets sequence of PageRank tables from given \c GraphSeq into \c TableSeq.
template <class PGraph>
void MapPageRank(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq,
    TTableContext* Context,
    const double& C, const double& Eps, const int& MaxIter) {
  int NumGraphs = GraphSeq.Len();
  TableSeq.Reserve(NumGraphs, NumGraphs);
  // This loop is parallelizable.
  for (TInt i = 0; i < NumGraphs; i++) {
    TIntFltH PRankH;
    GetPageRank(GraphSeq[i], PRankH, C, Eps, MaxIter);
    TableSeq[i] = TTable::TableFromHashMap(PRankH, "NodeId", "PageRank", Context, false);
  }
}

/// Gets sequence of Hits tables from given \c GraphSeq into \c TableSeq.
template <class PGraph>
void MapHits(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq,
    TTableContext* Context,
    const int& MaxIter) {
  int NumGraphs = GraphSeq.Len();
  TableSeq.Reserve(NumGraphs, NumGraphs);
  // This loop is parallelizable.
  for (TInt i = 0; i < NumGraphs; i++) {
    TIntFltH HubH;
    TIntFltH AuthH;
    GetHits(GraphSeq[i], HubH, AuthH, MaxIter);
    PTable HubT =  TTable::TableFromHashMap(HubH, "NodeId", "Hub", Context, false);
    PTable AuthT =  TTable::TableFromHashMap(AuthH, "NodeId", "Authority", Context, false);
    PTable HitsT = HubT->Join("NodeId", AuthT, "NodeId");
    HitsT->Rename("1.NodeId", "NodeId");
    HitsT->Rename("1.Hub", "Hub");
    HitsT->Rename("2.Authority", "Authority");
    TStrV V = TStrV(3, 0);
    V.Add("NodeId");
    V.Add("Hub");
    V.Add("Authority");
    HitsT->ProjectInPlace(V);
    TableSeq[i] = HitsT;
  }
}

}; // namespace TSnap

