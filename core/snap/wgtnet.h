#ifndef snap_wgtnet
#define snap_wgtnet

#include "Snap.h"

class TWgtNet;
typedef TPt<TWgtNet> PWgtNet;

/////////////////////////////////////////////////
// Weighted network
class TWgtNet : public TNodeEDatNet<TStr, TFlt> {
public:
  TWgtNet() : TNet() { }
  TWgtNet(TSIn& SIn) : TNet(SIn) { }
  void Save(TSOut& SOut) const { TNet::Save(SOut); }
  static PWgtNet New() { return new TWgtNet(); }
  static PWgtNet Load(TSIn& SIn) { return new TWgtNet(SIn); }

  double GetEdgeWgt() const;
  void MulEdgeWgt(const double& MulBy);
  void PermEdgeWgt();
  void PermOutEdgeWgt();
  void PutAllWgts();
  void PutRnd01Wgts();
  void AddBiDirEdges(const double& Wgt = -1);
  void DelMinWgtNodes(const double MinWgt);

  void ReinforceEdges(const int& NIters);

  static PWgtNet GetRngWgtNet(const int& Nodes, const int& Edges, const double& Wgt);
  static PWgtNet LoadCiteSeerCoAuth(const TStr& FNm);
  static PWgtNet LoadDblpCoAuth(const TStr& FNm);
  static PWgtNet LoadArxivCoAuth(const TStr& FNm);
  static PWgtNet LoadEveCommNet(const TStr& FNm);

  template <class PGraph>
  static PWgtNet MakeFromBipart(const PGraph& Graph, const bool& TakeLeft);
  friend class TPt<TWgtNet>;
};

// set flags
namespace TGBase {
template <> struct IsNodeGraph<TWgtNet> { enum { Val = 1 }; };
template <> struct IsDirected<TWgtNet> { enum { Val = 1 }; };
template <> struct IsNetwork<TWgtNet> { enum { Val = 1 }; };
template <> struct IsNodeDat<TWgtNet> { enum { Val = 1 }; };
template <> struct IsEdgeDat<TWgtNet> { enum { Val = 1 }; };
}

// For bipartite graph with edges A to B 
// TakeLeft=True: build a graph on A (where two nodes in A are linked 
// by edge weight of number of common nodes they link to in B)
template <class PGraph>
PWgtNet TWgtNet::MakeFromBipart(const PGraph& Graph, const bool& TakeLeft) {
  PWgtNet Net = TWgtNet::New();
  for (PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    IAssert((NI.GetOutDeg()==0 || NI.GetInDeg()==0) && (NI.GetOutDeg()>0 || NI.GetInDeg()>0)); // is bipartite
    if (TakeLeft) {
      if (NI.GetInDeg()==0) { continue; }
      for (int i = 0; i < NI.GetInDeg(); i++) {
        const int n1 = NI.GetInNId(i);
        for (int j = 0; j < NI.GetInDeg(); j++) {
          if (i == j) { continue; }
          const int n2 = NI.GetInNId(j);
          if (! Net->IsNode(n1)) { Net->AddNode(n1); }
          if (! Net->IsNode(n2)) { Net->AddNode(n2); }
          if (Net->IsEdge(n1, n2)) { Net->GetEDat(n1, n2) += 1; }
          else { Net->AddEdge(n1, n2, 1); }
        }
      }
    } else {
      if (NI.GetOutDeg()==0) { continue; }
      for (int i = 0; i < NI.GetOutDeg(); i++) {
        const int n1 = NI.GetOutNId(i);
        for (int j = 0; j < NI.GetOutDeg(); j++) {
          if (i == j) { continue; }
          const int n2 = NI.GetOutNId(j);
          if (! Net->IsNode(n1)) { Net->AddNode(n1); }
          if (! Net->IsNode(n2)) { Net->AddNode(n2); }
          if (Net->IsEdge(n1, n2)) { Net->GetEDat(n1, n2) += 1; }
          else { Net->AddEdge(n1, n2, 1); }
        }
      }
    }
  }
  TGBase::PrintInfo(Net);
  printf("  Edge weight: %f\n", Net->GetEdgeWgt());
  return Net;
}

/////////////////////////////////////////////////
// Top2 Friends network
class TTop2FriendNet {
private:
  PWgtNet Net;
  THash<TInt, TIntPr> Top2NIdH;
  TUnionFind Top1UF;
  void SetTop2();
public:
  TTop2FriendNet(const PWgtNet& NetPt) : Net(NetPt) { SetTop2(); }
  PNGraph GetTop1Net();
  int GetTop2Len() const { return Top2NIdH.Len(); }
  int GetTop2WccSz(const double ProbPick2nd) const;
  int GetRnd2WccSz(const double ProbPick2nd) const;

  static void GetAvgSDevV(const THash<TFlt, TMom>& MomH, TFltTrV& ValAvgSDevV);
  static void PlotPick2VsProb2nd(const PWgtNet& Net, const int& NRuns, const double& StepP, const TStr& OutFNm, 
    TStr Desc="", bool PlotTop2=true, bool PlotBtm2=true, bool PlotRnd2=true);
};

#endif
