// TODO ROK, Jure included basic documentation, finalize reference doc

/////////////////////////////////////////////////
// Graph Generators
namespace TSnap {

/////////////////////////////////////////////////
// Deterministic graphs
/// Generates a 2D-grid graph of Rows rows and Cols columns.
template <class PGraph> PGraph GenGrid(const int& Rows, const int& Cols, const bool& IsDir=true);
/// Generates a graph with star topology. Node id 0 is in the center and then links to all other nodes.
template <class PGraph> PGraph GenStar(const int& Nodes, const bool& IsDir=true);
/// Generates a circle graph where every node creates out-links to NodeOutDeg forward nodes.
template <class PGraph> PGraph GenCircle(const int& Nodes, const int& NodeOutDeg=1, const bool& IsDir=true);
/// Generates a complete graph on Nodes nodes. Graph has no self-loops.
template <class PGraph> PGraph GenFull(const int& Nodes);
/// Generates a tree graph of Levels levels with every parent having Fanout children.
template <class PGraph> PGraph GenTree(const int& Fanout, const int& Levels, const bool& IsDir=true, const bool& ChildPointsToParent=true);
/// Generates a Ravasz-Barabasi deterministic scale-free graph.
template <class PGraph> PGraph GenBaraHierar(const int& Levels, const bool& IsDir=true);

/////////////////////////////////////////////////
// Random graphs

/// Generates an Erdos-Renyi random graph.
template <class PGraph> PGraph GenRndGnm(const int& Nodes, const int& Edges, const bool& IsDir=true, TRnd& Rnd=TInt::Rnd);
/// Generates a random bipartite graph.
PBPGraph GenRndBipart(const int& LeftNodes, const int& RightNodes, const int& Edges, TRnd& Rnd=TInt::Rnd);
/// Generates a random graph where each node has degree exactly NodeDeg.
PUNGraph GenRndDegK(const int& Nodes, const int& NodeDeg, const int& NSwitch=100, TRnd& Rnd=TInt::Rnd);
PUNGraph GenRndPowerLaw(const int& Nodes, const double& PowerExp, const bool& ConfModel=true, TRnd& Rnd=TInt::Rnd);
PUNGraph GenDegSeq(const TIntV& DegSeqV, TRnd& Rnd=TInt::Rnd);
PUNGraph GenPrefAttach(const int& Nodes, const int& NodeOutDeg, TRnd& Rnd=TInt::Rnd);
PUNGraph GenGeoPrefAttach(const int& Nodes, const int& OutDeg, const double& Beta, TRnd& Rnd=TInt::Rnd);
PUNGraph GenSmallWorld(const int& Nodes, const int& NodeOutDeg, const double& RewireProb, TRnd& Rnd=TInt::Rnd);

PUNGraph GenConfModel(const TIntV& DegSeqV, TRnd& Rnd=TInt::Rnd);
PUNGraph GenRewire(const PUNGraph& Graph, const int& NSwitch=100, TRnd& Rnd=TInt::Rnd);
PNGraph  GenRewire(const PNGraph& Graph, const int& NSwitch=100, TRnd& Rnd=TInt::Rnd);
PBPGraph GenRewire(const PBPGraph& Graph, const int& NSwitch=100, TRnd& Rnd=TInt::Rnd);

PNGraph GenForestFire(const int& Nodes, const double& FwdProb, const double& BckProb);
PNGraph GenCopyModel(const int& Nodes, const double& Beta, TRnd& Rnd=TInt::Rnd);
PNGraph GenRMat(const int& Nodes, const int& Edges, const double& A, const double& B, const double& C, TRnd& Rnd=TInt::Rnd);
PNGraph GenRMatEpinions();

/////////////////////////////////////////////////
// Implementation
template <class PGraph>
PGraph GenGrid(const int& Rows, const int& Cols, const bool& IsDir) {
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Rows*Cols, 4*Rows*Cols);
  int node, r, c;
  for (node = 0; node < Rows * Cols; node++) {
    Graph.AddNode(node); }
  for (r = 0; r < Rows; r++) {
    for (c = 0; c < Cols; c++) {
      const int nodeId = Cols*r + c;
      if (r < Rows-1) { // bottom node
        Graph.AddEdge(nodeId, nodeId+Cols); 
        if (Graph.HasFlag(gfDirected) && ! IsDir) { 
          Graph.AddEdge(nodeId+Cols-1, nodeId); }
      }
      if (c < Cols-1) { // right node
        Graph.AddEdge(nodeId, nodeId+1); 
        if (Graph.HasFlag(gfDirected) && ! IsDir) { 
          Graph.AddEdge(nodeId+1, nodeId); }
      }
    }
  }
  return GraphPt;
}

template <class PGraph>
PGraph GenStar(const int& Nodes, const bool& IsDir) {
  PGraph Graph = PGraph::TObj::New();
  Graph->Reserve(Nodes, Nodes);
  Graph->AddNode(0);
  for (int n = 1; n < Nodes; n++) {
    Graph->AddNode(n);
    Graph->AddEdge(0, n);
    if (Graph->HasFlag(gfDirected) && ! IsDir) { Graph->AddEdge(n, 0); }
  }
  return Graph;
}

template <class PGraph>
PGraph GenCircle(const int& Nodes, const int& NodeOutDeg, const bool& IsDir) {
  PGraph Graph = PGraph::TObj::New();
  Graph->Reserve(Nodes, Nodes*NodeOutDeg);
  for (int n = 0; n < Nodes; n++) {
    Graph->AddNode(n); }
  for (int n = 0; n < Nodes; n++) {
    for (int x = 0; x < NodeOutDeg; x++) {
      Graph->AddEdge(n, (n+x) % Nodes);
      if (Graph->HasFlag(gfDirected) && ! IsDir) { Graph->AddEdge((n+x) % Nodes, n); }
    }
  }
  return Graph;
}

template <class PGraph>
PGraph GenFull(const int& Nodes) {
  PGraph Graph = PGraph::TObj::New();
  Graph->Reserve(Nodes, Nodes*Nodes);
  for (int n = 0; n < Nodes; n++) {
    Graph->AddNode(n); }
  for (int n1 = 0; n1 < Nodes; n1++) {
    for (int n2 = 0; n2 < Nodes; n2++) {
      if (n1 != n2) { Graph->AddEdge(n1, n2); }
    }
  }
  return Graph;
}

template <class PGraph>
PGraph GenTree(const int& Fanout, const int& Levels, const bool& IsDir, const bool& ChildPointsToParent) {
  const int Nodes = (int) (pow(double(Fanout), double(Levels+1)) - 1) / (Fanout - 1);
  const int Edges = Nodes - 1;
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, Edges);
  int node;
  for (node = 0; node < Nodes; node++) {
    Graph.AddNode(node); }
  // non-leaf nodes
  for (node = 0; node < (int) Nodes - (int) pow(double(Fanout), double(Levels)); node++) {
    for (int edge = 1; edge <= Fanout; edge++) {
      if (IsDir) {
        if (ChildPointsToParent) { Graph.AddEdge(Fanout*node+edge, node); }
        else { Graph.AddEdge(node, Fanout*node+edge); }
      } else {
        Graph.AddEdge(node, Fanout*node+edge); // link children
        Graph.AddEdge(Fanout*node+edge, node);
      }
    }
  }
  return GraphPt;
}

/// Corners of the graph are recursively expanded with miniature copies of the
/// base graph (below). The graph has power-law degree distribution with the
/// exponent 1+ln(5)/ln(4) and clustering coefficient with power-law decay 
/// exponent -1.
/// Base graph:

/// \verbatim
///   o---o
///   |\ /|
///   | o |
///   |/ \|
///   o---o
/// \endverbatim

/// See: Hierarchical organization in complex networks. Ravasz and Barabasi.
/// URL: http://arxiv.org/abs/cond-mat/0206130
template <class PGraph>
PGraph GenBaraHierar(const int& Levels, const bool& IsDir) {
  const int Nodes = (int) TMath::Round(TMath::Power(5, Levels));
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, -1);
  // base graph
  for (int i = 0; i < 5; i++) { Graph.AddNode(i); }
  Graph.AddEdge(1,2);  Graph.AddEdge(2,3);
  Graph.AddEdge(3,4);  Graph.AddEdge(4,1);
  Graph.AddEdge(1,0);  Graph.AddEdge(3,0);
  Graph.AddEdge(2,0);  Graph.AddEdge(4,0);
  // expansion
  const int CenterId = 0;
  for (int lev = 1; lev < Levels+1; lev++) {
    const int MxNId = Graph.GetNodes();
    // make 4 duplicate copies
    for (int d = 0; d < 4; d++) {
      for (int n = 0; n < MxNId; n++) { Graph.AddNode(); }
      for (int n = 0; n < MxNId; n++) {
        typename PGraph::TObj::TNodeI NI = Graph.GetNI(n);
        const int SrcId = n+MxNId*(d+1);
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          Graph.AddEdge(SrcId, NI.GetOutNId(e)+MxNId*(d+1));
        }
      }
    }
    // add edges to the center
    const int LevPow = (int)TMath::Round(TMath::Power(5,lev-1));
    for (int n = MxNId; n < Graph.GetNodes(); n++) {
      typename PGraph::TObj::TNodeI NI = Graph.GetNI(n);
      const int SrcId = n;
      int Pow = 1;  bool Skip = false;
      for (int p = 1; p <= lev; p++) {
        if (SrcId % (5*Pow) < Pow) { Skip=true; break; }
        Pow *= 5;
      }
      if (Skip) { continue; }
      Graph.AddEdge(SrcId, CenterId);
    }
  }
  return GraphPt;
}

template <class PGraph>
PGraph GenRndGnm(const int& Nodes, const int& Edges, const bool& IsDir, TRnd& Rnd) {
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, Edges);
  for (int node = 0; node < Nodes; node++) {
    IAssert(Graph.AddNode(node) == node);
  }
  for (int edge = 0; edge < Edges; ) {
    const int SrcNId = Rnd.GetUniDevInt(Nodes);
    const int DstNId = Rnd.GetUniDevInt(Nodes);
    if (SrcNId != DstNId && Graph.AddEdge(SrcNId, DstNId) != -2) { // is new edge
      if (! IsDir) { Graph.AddEdge(DstNId, SrcNId); }
      edge++;
    }
  }
  return GraphPt;
}

namespace TSnapDetail {
/// Returns a random edge in a graph Graph where the edge does not touch nodes NId1 and NId2.
template <class PGraph>
TIntPr GetRndEdgeNonAdjNode(const PGraph& Graph, int NId1, int NId2) {
  typename PGraph::TObj::TNodeI NI1, NI2;
  int OutDeg = -1;
  do {
    NI1 = Graph->GetRndNI();
    OutDeg = NI1.GetOutDeg();
  } while (OutDeg == 0);
  NI2 = Graph->GetNI(NI1.GetOutNId(TInt::Rnd.GetUniDevInt(OutDeg)));
  int runs = 0;
  while (NI1.IsNbrNId(NId1) || NI1.IsNbrNId(NId2) || NI2.IsNbrNId(NId1) || NI2.IsNbrNId(NId2) || NI1.GetId()==NI2.GetId()) {
    do {
      NI1 = Graph->GetRndNI();
      OutDeg = NI1.GetOutDeg();
    } while (OutDeg == 0);
    NI2 = Graph->GetNI(NI1.GetOutNId(TInt::Rnd.GetUniDevInt(OutDeg)));
    if (runs++ == 1000) { return TIntPr(-1, -1); }
  }
  return TIntPr(NI1.GetId(), NI2.GetId());
}

} // namespace TSnapDetail

}; // namespace TSnap
