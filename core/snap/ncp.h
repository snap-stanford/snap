#ifndef snap_network_community_profile_h
#define snap_network_community_profile_h

#include "Snap.h"

//////////////////////////////////////////////////
// Local Spectral Clustering
//   PageRank Nibble Local Clustering
//   see: Local Graph Partitioning using PageRank Vectors
//        by Reid Andersen, Fan Chung and Kevin Lang
class TLocClust {
public:
  static bool Verbose;
private:
  PUNGraph Graph;
  int Nodes, Edges2;       // nodes, 2*edges in Graph
  TIntFltH ProbH, ResH;
  TIntQ NodeQ;
  double Alpha;            // PageRank jump probability (smaller Alpha diffuses the mass farther away)
  int SeedNId;             // current seed node
  // volume, cut size, node ids, conductances
  TIntV NIdV, VolV, CutV;  // vol=2*edges_inside+cut (vol = sum of the degrees)
  TFltV PhiV;              // conductance
  int BestCutIdx;          // best cut: index to vectors where PhiV is minimum
public:
  TLocClust(const PUNGraph& GraphPt, const double& AlphaVal) :
    Graph(GraphPt), Nodes(GraphPt->GetNodes()), Edges2(2*GraphPt->GetEdges()), Alpha(AlphaVal) { }

  int Len() const { return GetRndWalkSup(); }
  int GetRndWalkSup() const { return VolV.Len(); }

  // all cuts around node SeedNId
  int GetNId(const int& ValId) const { return NIdV[ValId]; }
  int GetVol(const int& ValId) const { return VolV[ValId]; }
  int GetCut(const int& ValId) const { return CutV[ValId]; }
  double GetPhi(const int& ValId) const { return PhiV[ValId]; }
  const TIntV& GetNIdV() const { return NIdV; } // node ids in the random walk order
  const TIntV& GetVolV() const { return VolV; } // volume of first VolV nodes in NIdV
  const TIntV& GetCutV() const { return CutV; } // edges cut to seperate first VolV nodes
  const TFltV& GetPhiV() const { return PhiV; } // conducatance of the bounding cut of first VolV nodes in NIdV

  // best cut around node SeedNId
  int BestCut() const { return BestCutIdx; }
  int BestCutNodes() const { return BestCutIdx+1; }     // nodes inside the cut
  int GetCutEdges() const { return GetCut(BestCut()); } // edges cut
  int GetCutVol() const { return GetVol(BestCut()); }   // 2*edges inside+edges cut
  double GetCutPhi() const { return GetPhi(BestCut()); }

  // find the cut
  int ApproxPageRank(const int& StartNId, const double& Eps);
  void DivByDeg();
  void SupportSweep();
  void FindBestCut(const int& SeedNode, const int& ClustSz, const double& MinSizeFrac=0.2);

  void PlotVolDistr(const TStr& OutFNm, TStr Desc=TStr()) const;
  void PlotCutDistr(const TStr& OutFNm, TStr Desc=TStr()) const;
  void PlotPhiDistr(const TStr& OutFNm, TStr Desc=TStr()) const;
  void SavePajek(const TStr& OutFNm) const;

  static void DrawWhiskers(const PUNGraph& Graph, TStr FNmPref, const int& PlotN);
  static void GetCutStat(const PUNGraph& Graph, const TIntV& NIdV, int& Vol, int& Cut, double& Phi, int GraphEdges=-1);
  static void GetCutStat(const PUNGraph& Graph, const TIntSet& NIdSet, int& Vol, int& Cut, double& Phi, int GraphEdges=-1);
  static void PlotNCP(const PUNGraph& Graph, const TStr& FNm, const TStr Desc="", const bool& BagOfWhiskers=true, const bool& RewireNet=false,
    const int& KMin=10, const int& KMax=Mega(100), const int& Coverage=10, const bool& SaveTxtStat=false, const bool& PlotBoltzman=false);
};

//////////////////////////////////////////////////
// Local Clustering Statistics
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
// Local clustering for a set of graphs (loads ncp-*.tab files)
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
