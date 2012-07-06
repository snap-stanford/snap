// ROK TODO, Jure included basic documentation, finalize reference doc

#ifndef snap_network_community_profile_h
#define snap_network_community_profile_h

#include "Snap.h"

class TLocClust;
class TLocClustStat;
class TNcpGraphsBase;

//////////////////////////////////////////////////
/// Local Spectral Clustering algorithm.
/// The code implements the PageRank Nibble local clustering algorithm of Andersen, Chung and Lang.
/// Given a single starting seed node, the algorithm will then find the clusters around that node.
/// This is achieved by the algorithm finding the approximate personalized PageRank score of every 
/// node with respect to the Seed node. Nodes are then ordered by the PageRank score and the idea
/// is then that by 'sweeping' the vector of PageRank scores one can find communities around the chosen seed node.
/// The idea is to try out K = 1...N/2 and then for a set of {node_1 ... node_K} test the value
/// of the conductance (Phi). If the conductance at certain value of K achieves a local minima,
/// then we found a good cut in the graph.
/// This method is also used for computing the Network Community Profile plots.
/// See: Local Graph Partitioning using PageRank Vectors by R. Andersen, F. Chung and K. Lang
/// URL: http://www.math.ucsd.edu/~fan/wp/localpartition.pdf 
class TLocClust {
public:
  static bool Verbose;
private:
  PUNGraph Graph;
  int Nodes, Edges2;       // Nodes, 2*edges in Graph
  TIntFltH ProbH, ResH;
  TIntQ NodeQ;
  double Alpha;            // PageRank jump probability (smaller Alpha diffuses the mass farther away)
  int SeedNId;             // Seed node
  // volume, cut size, node ids, conductances
  TIntV NIdV, VolV, CutV;  // Vol=2*edges_inside+cut (vol = sum of the degrees)
  TFltV PhiV;              // Conductance
  int BestCutIdx;          // Index K to vectors where the conductance of the bounding cut (PhiV[K]) achieves its minimum
private:
  /// Vector of node IDs sorted in the random walk order
  const TIntV& GetNIdV() const { return NIdV; }
  /// Volume (sum of the degrees) of first K-nodes in the node-id vector (GetNIdV()).
  const TIntV& GetVolV() const { return VolV; } 
  /// Edges cut to seperate the first K-nodes in the node-id vector (GetNIdV()) from the rest of the graph.
  const TIntV& GetCutV() const { return CutV; } 
  /// Conducatance of the cut separating first K-nodes in the node-id vector (GetNIdV()) from the rest of the graph.
  const TFltV& GetPhiV() const { return PhiV; } 
public:
  TLocClust(const PUNGraph& GraphPt, const double& AlphaVal) :
    Graph(GraphPt), Nodes(GraphPt->GetNodes()), Edges2(2*GraphPt->GetEdges()), Alpha(AlphaVal) { }
  /// Returns the support of the approximate random walk. That is the number of nodes with non-zero PageRank score.   
  int Len() const { return GetRndWalkSup(); }
  /// Returns the support of the approximate random walk. That is the number of nodes with non-zero PageRank score.
  int GetRndWalkSup() const { return VolV.Len(); }

  /// Returns the ID of the NodeN-th node in the sweep vector.
  int GetNId(const int& NodeN) const { return NIdV[NodeN]; }
  /// Returns the volume of the set of first NodeN nodes in the sweep vector. 
  /// Volume is defined as the sum of the degrees of the first Nodes nodes. Or in other words volume = 2* edges inside the ste + the edges pointing outside the set.
  int GetVol(const int& Nodes) const { return VolV[Nodes]; }
  /// Returns the size of the cut of the first Nodes nodes in the sweep vector.
  /// Size of the cut is the number of edges pointing betweent the first Nodes nodes and the remainder of the graph.
  int GetCut(const int& Nodes) const { return CutV[Nodes]; }
  /// Returns the conducance of the cut separating the first Nodes nodes in the sweep vector from the rest of the graph.
  /// Conducatance is the ration Cut/Volume. The lower the conductance the 'better' the cluster (higher volume, less edges cut).
  double GetPhi(const int& ValId) const { return PhiV[ValId]; }

  /// Index K of the cut of the minimum conductance around the seed node.
  /// This means that the set of GetNId(0)...GetNId(K) forms the best cut around the seed node.
  int BestCut() const { return BestCutIdx; }
  /// Number of nodes inside the 'best' (minimum conductance) cut.
  int BestCutNodes() const { return BestCutIdx+1; }
  /// Number of edges in the 'best' (minimum conductance) cut.
  int GetCutEdges() const { return GetCut(BestCut()); }
  /// Volume of the 'best' (minimum conductance) cut.
  int GetCutVol() const { return GetVol(BestCut()); }
  /// Conducatance of the 'best' (minimum conductance) cut.
  double GetCutPhi() const { return GetPhi(BestCut()); }

  /// Computes Approximate PageRank from the seed node SeedNId and with tolerance Eps.
  /// The algorithm basically sets the PageRank scores of nodes with score <Eps to zero. So the lower the value of Eps the longer the algorithm will run.
  int ApproxPageRank(const int& SeedNode, const double& Eps);
  /// After the function ApproxPageRank() has been run the SupportSweep() will compute the volume, cut size, node ids, conductance vectors. 
  void SupportSweep();
  /// Finds minimum conductance cut in the graph around the seed node.
  /// Function first computes the ApproxPageRank(), initializes the SupportSweep() and then find the minimum conductance cluster.
  /// Parameter ClustSz controls the expected cluster size and is used to determine the tolerance (Eps) of the approximate PageRank calculation.
  void FindBestCut(const int& SeedNode, const int& ClustSz, const double& MinSizeFrac=0.2);

  /// Plots the cluster volume vs. cluster size K (cluster is composes of nodes NIdV[1...K]).
  void PlotVolDistr(const TStr& OutFNm, TStr Desc=TStr()) const;
  /// Plots the cluster cut size vs. cluster size K (cluster is composes of nodes NIdV[1...K]).
  void PlotCutDistr(const TStr& OutFNm, TStr Desc=TStr()) const;
  /// Plots the cluster conductance vs. cluster size K (cluster is composes of nodes NIdV[1...K]).
  void PlotPhiDistr(const TStr& OutFNm, TStr Desc=TStr()) const;
  /// Saves the network in the Pajek format so it can be visualized. Red node represents the seed and color the cluster membership.
  void SavePajek(const TStr& OutFNm) const;

  /// Draws teh 'whiskers' of the graph. Whiskers are small sub-graphs that are attached to the rest of the graph via a single edge.
  static void DrawWhiskers(const PUNGraph& Graph, TStr FNmPref, const int& PlotN);
  /// For a given Graph and a set of nodes NIdV the function returns the Volume, CutSize and the Conductance of the cut.
  static void GetCutStat(const PUNGraph& Graph, const TIntV& NIdV, int& Vol, int& Cut, double& Phi, int GraphEdges=-1);
  /// For a given Graph and a set of nodes NIdV the function returns the Volume, CutSize and the Conductance of the cut.
  static void GetCutStat(const PUNGraph& Graph, const TIntSet& NIdSet, int& Vol, int& Cut, double& Phi, int GraphEdges=-1);
  /// Plots the Network Community Profile (NCP) of a given graph Graph.
  /// The NCP plot of a network captures the global community structure of the network. 
  /// Refer to 'Community Structure in Large Networks: Natural Cluster Sizes and the Absence of Large Well-Defined Clusters by J. Leskovec, K. Lang, A. Dasgupta, M. Mahoney. Internet Mathematics 6(1) 29--123, 2009'
  /// for the explanation of how to read these plots.
  /// URL: http://arxiv.org/abs/0810.1355
  static void PlotNCP(const PUNGraph& Graph, const TStr& FNm, const TStr Desc="", const bool& BagOfWhiskers=true, const bool& RewireNet=false,
    const int& KMin=10, const int& KMax=Mega(100), const int& Coverage=10, const bool& SaveTxtStat=false, const bool& PlotBoltzman=false);

  friend class TLocClustStat;
};

//////////////////////////////////////////////////
/// Local-Spectral-Clustering statistics of a given Graph.
class TLocClustStat {
public:
  static double BinFactor;
  static int TakeValAt;
public:
  class TNodeSweep {
  public:
    TInt SeedNId;
    TIntV SweepV;  // nodes inside the cut: cut of size k has node ids CutV[0...k-1]
    TFltV PhiV;    // conductance at the cut
  public:
    TNodeSweep() {}
    TNodeSweep(const int& SeedNode, const TIntV& SweepNIdV, const TFltV& PhiNIdV) :
      SeedNId(SeedNode), SweepV(SweepNIdV), PhiV(PhiNIdV) { IAssert(SweepV.Len()==PhiV.Len()); }
    TNodeSweep(const TNodeSweep& NS) : SeedNId(NS.SeedNId), SweepV(NS.SweepV), PhiV(NS.PhiV) { }
    int Len() const { return SweepV.Len(); }
    int GetSeed() const { return SeedNId; }
    int NId(const int i) const { return SweepV[i]; }
    double Phi(const int i) const { return PhiV[i]; }
    bool operator < (const TNodeSweep& CS) const { return Len() < CS.Len(); }
  };

  class TCutInfo {
  public:
    TInt Nodes, Edges, CutSz; // nodes inside, edges inside, edges cut
    TIntV CutNIdV;            // node ids inside the cluster
  public:
    TCutInfo() : Nodes(0), Edges(0), CutSz(0), CutNIdV() { }
    TCutInfo(const int& ClustNodes, const int& EdgesInside, const int& CutSize) : Nodes(ClustNodes), Edges(EdgesInside), CutSz(CutSize) { }
    TCutInfo(const int& ClustNodes, const int& EdgesInside, const int& CutSize, const TIntV& NIdV) :
      Nodes(ClustNodes), Edges(EdgesInside), CutSz(CutSize), CutNIdV(NIdV) { }
    TCutInfo(const PUNGraph& G, const TIntV& ClustNIdV, bool TakeNIdV=false) : Nodes(ClustNIdV.Len()) {
      TSnap::GetEdgesInOut(G, ClustNIdV, Edges.Val, CutSz.Val); if(TakeNIdV){CutNIdV=ClustNIdV;} }
    TCutInfo(const TCutInfo& CS) : Nodes(CS.Nodes), Edges(CS.Edges), CutSz(CS.CutSz), CutNIdV(CS.CutNIdV) { }
    int GetNodes() const { return Nodes; }
    int GetEdges() const { return Edges; }
    int GetVol() const { return 2*Edges+CutSz; }
    int GetCutSz() const { return CutSz; }
    // community score measures (lower is better)
    double GetPhi() const { return double(CutSz)/double(2*Edges+CutSz); }                                     // conductance
    double GetExpansion() const { return Nodes<2 ? 1.0 : double(CutSz)/double(Nodes); }                       // expansion
    double GetIntDens() const { return 1.0 - ((Nodes<2) ? 0 : 2.0*double(Edges)/double(Nodes*(Nodes-1))); }   // internal density
    double GetCutRatio(const int& GNodes) const { return double(CutSz)/double(Nodes*(GNodes-Nodes)); }        // cut ratio (external density)
    double GetNormCut(const int& GEdges) const { return GetPhi() + double(CutSz)/double(2*GEdges-GetVol()); } // normalized cut
    double GetFracDegOut(const PUNGraph& Graph, double& MxFrac, double& AvgFrac, double& MedianFrac, double& Pct9Frac, double& Flake) const; // fraction of node's edges pointing outside the cluster
    double GetModular(const int& GEdges) const { return (2.0*Edges - GetExpEdgesIn(GEdges)) / (2.0*GEdges); } // modularity
    double GetModRat(const int& GEdges) const { return (2.0*Edges) / GetExpEdgesIn(GEdges); }                 // modularity ratio
    double GetExpEdgesIn(const int& GEdges) const { return TMath::Sqr(2.0*Edges+CutSz)/(2.0*GEdges); }        // expected edges inside (sum of degrees on nodes inside)^2/(2*E)
    bool operator < (const TCutInfo& CS) const { return GetPhi() < CS.GetPhi(); }
  };

private:
  // parameters
  TFlt Alpha, SizeFrac, KFac;
  TInt KMin, KMax, Coverage;
  PUNGraph Graph; // set at ::Run()
//private:
public:
  TVec<TNodeSweep> SweepsV;       // node ids and conductances for each run of local clustering
  THash<TInt, TFltV> SizePhiH;    // all conductances at cluster size K
  THash<TInt, TCutInfo> BestCutH; // best cut (min conductance) at size K (edges inside, edges cut)
  TFltPrV BagOfWhiskerV;          // (size, conductance) for bag of whiskers
  TFltPr BestWhisk;               // best whisker (whisker with largest volume), (size, conductance)
  TCutInfo BestCut;               // best over-all cut
  TIntSet SizeBucketSet;          // for exponential bucketing (only indexes BestCutH at positions in BucketSet have CutNIdV filled)
public:
  TLocClustStat(const double& _Alpha, const int& _KMin, const int& _KMax, const double& _KFac, const int& _Coverage, const double& _SizeFrac);
  void Save(TSOut& SOut) const;
  void Clr();
  void SetGraph(const PUNGraph& GraphPt) { Graph=GraphPt; }
  void Run(const PUNGraph& Graph, const bool& SaveAllSweeps=false, const bool& SaveAllCond=false, const bool& SaveBestNodesAtK=false);
  void AddBagOfWhiskers();
  void AddCut(const TIntV& NIdV);
  void AddCut(const int& ClustSz, const double& Phi);
  void AddToBestCutH(const PUNGraph& Graph, const TIntV& NIdV, const bool& AddBestCond=true);

  double FindBestCut(const int& Nodes, const TIntSet& TabuNIdSet, int& BestCutId) const;
  const TCutInfo& FindBestCut(const int& Nodes=-1) const;
  double FindBestCut(const int& Nodes, TIntV& ClustNIdV) const;
  int FindBestCut(const int& Nodes, int& Vol, int& Cut, double& Phi) const;

  const TCutInfo& GetBestCut() const { return BestCut; } // overall best cut
  int GetCuts() const { return BestCutH.Len(); }
  const TCutInfo& GetKNodeCut(const int& Nodes) const { return BestCutH.GetDat(Nodes); }
  const TCutInfo& GetCutN(const int& N) const { return BestCutH[N]; }

  int BestWhiskNodes() const { return int(BestWhisk.Val1.Val); }
  int BestWhiskEdges() const { return (int)TMath::Round(1.0/BestWhisk.Val2.Val)/2-1; }
  double BestWhiskPhi() const { return BestWhisk.Val2; }
  TFltPr GetBestWhisk() const { return BestWhisk; }
  const TFltPrV& GetBagOfWhiskersV() const { return BagOfWhiskerV; }
  void GetCurveStat(TFltPrV& MeanV, TFltPrV& MedV, TFltPrV& Dec1V, TFltPrV& MinV, TFltPrV& MaxV) const;
  void GetBoltzmanCurveStat(const TFltV& TempV, TVec<TFltPrV>& NcpV) const;

  TStr ParamStr() const;
  void PlotNCP(const TStr& OutFNm, TStr Desc=TStr()) const; // lower-envelope of conductance
  void PlotNCPModul(const TStr& OutFNm, TStr Desc=TStr()) const; // NCP but with modularity on Y-axis
  void PlotNcpTop10(const TStr& OutFNm, TStr Desc, const int& TakeMinN) const;
  void PlotPhiInOut(const TStr& OutFNm, TStr Desc=TStr()) const; // conductance on the boundary / counductance inside
  void PlotBestClustDens(TStr OutFNm, TStr Desc=TStr()) const; // plot edges inside, cut size, conductance
  void PlotNCPScatter(const TStr& OutFNm, TStr Desc=TStr()) const; // all different conducances of all sizes (not just min)
  void PlotPhiDistr(const int& CmtySz, const TStr& OutFNm, TStr Desc=TStr()) const; // histogram of conductances for a fixed CmtySz
  void PlotBoltzmanCurve(const TStr& OutFNm, TStr Desc=TStr()) const;

  void ImposeNCP(const TLocClustStat& LcStat2, TStr OutFNm, TStr Desc, TStr Desc1, TStr Desc2) const;
  void ImposeNCP(const TLocClustStat& LcStat2, const TLocClustStat& LcStat3, TStr OutFNm, TStr Desc, TStr Desc1, TStr Desc2, TStr Desc3) const;
  void SaveTxtInfo(const TStr& OutFNmPref, const TStr& Desc, const bool& SetMaxAt1) const;

  static void BagOfWhiskers(const PUNGraph& Graph, TFltPrV& SizePhiV, TFltPr& BestWhisk);
  static void BagOfWhiskers2(const PUNGraph& Graph, TFltPrV& SizePhiV);
  static void MakeExpBins(const TFltPrV& ValV, TFltPrV& NewV);
  static void MakeExpBins(const TFltV& ValV, TFltV& NewV);
};

//////////////////////////////////////////////////
/// Local-Spectral-Clustering for a set of graphs (loads ncp-*.tab files)
class TNcpGraphsBase {
private:
  TStrV GNmV;
  TFltV ParamValV ;
  TIntPrV GSizeV;
  TFltPrV WhiskerV, RewWhiskerV; // (size, conductance)
  TVec<TFltPrV> NcpV;
  TVec<TFltPrV> RewNcpV;
  TVec<TFltPrV> WhiskNcpV;
public:
  TNcpGraphsBase(const TStr& FNmWc);
  TNcpGraphsBase(TSIn& SIn);
  void Save(TSOut& SOut) const;
  void Impose(const TStr& OutFNm, const int& TopN, const bool& Smooth);
  double GetXAtMinY(const TFltPrV& Ncp, const int& NNodes);
  TFltPr GetXYAtMinY(const TFltPrV& Ncp, const int& NNodes);
  void PlotNcpMin(const TStr& OutFNm, const bool& VsGraphN=false);
  void SaveTxtNcpMin(const TStr& OutFNm, const bool& VsGraphN=false);
  void PlotRewNcpMin(const TStr& OutFNm, const bool& VsGraphN=false);
  void PlotBestWhisker(const TStr& OutFNm, const bool& VsGraphN=false);
  void PlotRewBestWhisker(const TStr& OutFNm, const bool& VsGraphN=false);
  void PlotAvgNcp(const TStr& OutFNm, const TVec<TFltPrV>& NcpVec, const int& MinSz, const double& MaxMinY);
  void SaveTxt(const TStr& OutFNm);
  static void PlotDataset(const TStr& InFNmWc, const TStr& OutFNm, const bool& ImposeNcp=false, const bool& VsGraphN=false);
};

#endif
