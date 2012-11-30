#ifndef snap_cascnetinf_h
#define snap_cascnetinf_h

#include "Snap.h"

// Hit info (timestamp, candidate parent) about a node in a cascade
class THitInfo {
public:
  TInt NId, Parent;
  TFlt Tm;
public:
  THitInfo(const int& NodeId=-1, const double& HitTime=0) : NId(NodeId), Parent(-1), Tm(HitTime) { }
  THitInfo(TSIn& SIn) : NId(SIn), Parent(SIn), Tm(SIn) { }
  void Save(TSOut& SOut) const { NId.Save(SOut); Parent.Save(SOut); Tm.Save(SOut); }
  bool operator < (const THitInfo& Hit) const {
    return Tm < Hit.Tm; }
};

// Cascade
class TCascade {
public:
  THash<TInt, THitInfo> NIdHitH;
  TFlt CurProb, Alpha, Eps;
  TInt Model;
public:
  TCascade() : NIdHitH(), CurProb(0), Alpha(1.0), Eps(1e-64), Model(0) { }
  TCascade(const double &alpha) : NIdHitH(), CurProb(0), Alpha(alpha), Eps(1e-64), Model(0) { }
  TCascade(const double &alpha, const int &model) : NIdHitH(), CurProb(0), Alpha(alpha), Eps(1e-64), Model(model) { }
  TCascade(const double &alpha, const double &eps) : NIdHitH(), CurProb(0), Alpha(alpha), Eps(eps), Model(0) { }
  TCascade(const double &alpha, const int &model, const double &eps) : NIdHitH(), CurProb(0), Alpha(alpha), Eps(eps), Model(model) { }
  TCascade(TSIn& SIn) : NIdHitH(SIn), CurProb(SIn), Alpha(SIn) { }
  void Save(TSOut& SOut) const  { NIdHitH.Save(SOut); CurProb.Save(SOut); Alpha.Save(SOut); }
  void Clr() { NIdHitH.Clr(); CurProb = 0; Alpha = 1.0; }
  int Len() const { return NIdHitH.Len(); }
  int GetNode(const int& i) const { return NIdHitH.GetKey(i); }
  int GetParent(const int NId) const { return NIdHitH.GetDat(NId).Parent; }
  double GetAlpha() const { return Alpha; }
  double GetTm(const int& NId) const { return NIdHitH.GetDat(NId).Tm; }
  void Add(const int& NId, const double& HitTm) { NIdHitH.AddDat(NId, THitInfo(NId, HitTm)); }
  void Del(const int& NId) { NIdHitH.DelKey(NId); }
  bool IsNode(const int& NId) const { return NIdHitH.IsKey(NId); }
  void Sort() { NIdHitH.SortByDat(true); }
  double TransProb(const int& NId1, const int& NId2) const;
  double GetProb(const PNGraph& G);
  void InitProb();
  double UpdateProb(const int& N1, const int& N2, const bool& UpdateProb=false);
};

// Node info (name and number of cascades)
class TNodeInfo {
public:
  TStr Name;
  TInt Vol;
public:
  TNodeInfo() { }
  TNodeInfo(const TStr& NodeNm, const int& Volume) : Name(NodeNm), Vol(Volume) { }
  TNodeInfo(TSIn& SIn) : Name(SIn), Vol(SIn) { }
  void Save(TSOut& SOut) const { Name.Save(SOut); Vol.Save(SOut); }
};

// Edge info (name and number of cascades)
class TEdgeInfo {
public:
  TInt Vol;
  TFlt MarginalGain, MarginalBound, MedianTimeDiff, AverageTimeDiff; // we can skip MarginalBound for efficiency if not explicitly required
public:
  TEdgeInfo() { }
  TEdgeInfo(const int& v,
        const double& mg,
        const double& mb,
        const double& mt,
      const double& at) : Vol(v), MarginalGain(mg), MarginalBound(mb), MedianTimeDiff(mt), AverageTimeDiff(at) { }
  TEdgeInfo(const int& v,
        const double& mg,
        const double& mt,
      const double& at) : Vol(v), MarginalGain(mg), MarginalBound(0), MedianTimeDiff(mt), AverageTimeDiff(at) { }
  TEdgeInfo(TSIn& SIn) : Vol(SIn), MarginalGain(SIn), MarginalBound(SIn), MedianTimeDiff(SIn), AverageTimeDiff(SIn) { }
  void Save(TSOut& SOut) const { Vol.Save(SOut); SOut.Save(MarginalGain); SOut.Save(MarginalBound); SOut.Save(MedianTimeDiff); SOut.Save(AverageTimeDiff); } //
};

// NETINF algorithm class
class TNetInfBs {
public:
  TVec<TCascade> CascV;
  THash<TInt, TNodeInfo> NodeNmH;
  THash<TIntPr, TEdgeInfo> EdgeInfoH;
  TVec<TPair<TFlt, TIntPr> > EdgeGainV;

  THash<TIntPr, TIntV> CascPerEdge; // To implement localized update
  PNGraph Graph, GroundTruth;
  bool BoundOn, CompareGroundTruth;
  TFltPrV PrecisionRecall;

  TIntPrFltH Alphas, Betas;

public:
  TNetInfBs( ) { BoundOn = false; CompareGroundTruth=false; }
  TNetInfBs(bool bo, bool cgt) { BoundOn=bo; CompareGroundTruth=cgt; }
  TNetInfBs(TSIn& SIn) : CascV(SIn), NodeNmH(SIn) { }
  void Save(TSOut& SOut) const { CascV.Save(SOut); NodeNmH.Save(SOut); }

  void LoadCascadesTxt(TSIn& SIn, const int& Model, const double& alpha);
  void LoadGroundTruthTxt(TSIn& SIn);

  void AddGroundTruth(PNGraph& gt) { GroundTruth = gt; }
  
  void AddCasc(const TStr& CascStr, const int& Model=0, const double& alpha=1.0);
  void AddCasc(const TCascade& Cascade) { CascV.Add(Cascade); }
  void GenCascade(TCascade& C, const int& TModel, const double &window, TIntPrIntH& EdgesUsed, const double& delta,
                 const double& std_waiting_time=0, const double& std_beta=0);
  TCascade & GetCasc(int c) { return CascV[c]; }
  int GetCascs() { return CascV.Len(); }

  int GetNodes() { return Graph->GetNodes(); }
  void AddNodeNm(const int& NId, const TNodeInfo& Info) { NodeNmH.AddDat(NId, Info); }
  TStr GetNodeNm(const int& NId) const { return NodeNmH.GetDat(NId).Name; }
  TNodeInfo GetNodeInfo(const int& NId) const { return NodeNmH.GetDat(NId); }
  bool IsNodeNm(const int& NId) const { return NodeNmH.IsKey(NId); }

  void Init();
  double GetAllCascProb(const int& EdgeN1, const int& EdgeN2);
  TIntPr GetBestEdge(double& CurProb, double& LastGain, bool& msort, int &attempts);
  double GetBound(const TIntPr& Edge, double& CurProb);
  void GreedyOpt(const int& MxEdges);

  void SavePajek(const TStr& OutFNm);
  void SavePlaneTextNet(const TStr& OutFNm);
  void SaveEdgeInfo(const TStr& OutFNm);
  void SaveObjInfo(const TStr& OutFNm);

  void SaveGroundTruth(const TStr& OutFNm);
  void SaveCascades(const TStr& OutFNm);
};

#endif
