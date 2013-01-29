#ifndef snap_cascdynetinf_h
#define snap_cascdynetinf_h

#include "Snap.h"

// pairwise transmission models
typedef enum {
  EXP, // exponential
  POW, // powerlaw
  RAY, // rayleigh
  WEI // weibull
} TModel;

// tx rates trends over time for synthetic experiments
typedef enum {
  CONSTANT, // constant
  LINEAR, // linear trend up/down
  EXPONENTIAL, // exponential trend
  RAYLEIGH, // rayleigh trend
  SLAB, // slab
  SQUARE, // square
  CHAINSAW, // chainsaw
  RANDOM // random noise around alpha value
} TVarying;

// optimization methods
typedef enum {
  OSG,    // stochastic gradient
  OWSG,    // windowed stochastic gradient
  OESG,   // exponential decay stochastic gradient
  OWESG,  // windowed exponential decay stochastic gradient
  ORSG,  // rayleigh decay stochastic gradient
  OBSG,   // no decay batch stochastic gradient
  OWBSG,   // windowed batch stochastic gradient
  OEBSG,  // exponential decay batch stochastic gradient
  ORBSG,  // rayleigh decay batch stochastic gradient
  OFG
} TOptMethod;

typedef enum {
  UNIF_SAMPLING,
  WIN_SAMPLING,
  EXP_SAMPLING,
  WIN_EXP_SAMPLING,
  RAY_SAMPLING
} TSampling;

// l2 regularizer on/off
typedef enum {
  NONE, // no regularizer
  L2REG // L2 regularizer
} TRegularizer;

typedef enum {
  TIME_STEP, // run inference every time step
  INFECTION_STEP, // run inference every # number of infections
  CASCADE_STEP, // run inference every time a cascade "finishes"
  SINGLE_STEP
} TRunningMode;

typedef TNodeEDatNet<TStr, TFltFltH> TStrFltFltHNEDNet;
typedef TPt<TStrFltFltHNEDNet> PStrFltFltHNEDNet;

typedef TNodeEDatNet<TStr, TFlt> TStrFltNEDNet;
typedef TPt<TStrFltNEDNet> PStrFltNEDNet;

// Hit info (node id, timestamp) about a node in a cascade
class THitInfo {
public:
  TInt NId;
  TFlt Tm;
  TIntV Keywords;
public:
  THitInfo(const int& NodeId=-1, const double& HitTime=0) : NId(NodeId), Tm(HitTime) { }
  THitInfo(TSIn& SIn) : NId(SIn), Tm(SIn), Keywords(SIn) { }
  void AddKeyword(const int& KId) { Keywords.AddUnique(KId); }
  void DelKeywords() { Keywords.Clr(); }
  void Save(TSOut& SOut) const { NId.Save(SOut); Tm.Save(SOut); Keywords.Save(SOut); }
  bool operator < (const THitInfo& Hit) const {
    return Tm < Hit.Tm; }
};

// Cascade
class TCascade {
public:
  TInt CId; // cascade id
  THash<TInt, THitInfo> NIdHitH; // infected nodes
  TInt Model; // pairwise transmission model
public:
  TCascade() : CId(0), NIdHitH(), Model(0) { }
  TCascade(const int &model) : NIdHitH() { Model = model; }
  TCascade(const int &cid, const int& model) : NIdHitH() { CId = cid; Model = model; }
  TCascade(TSIn& SIn) : CId(SIn), NIdHitH(SIn), Model(SIn) { }
  void Save(TSOut& SOut) const  { CId.Save(SOut); NIdHitH.Save(SOut); Model.Save(SOut); }
  void Clr() { NIdHitH.Clr(); }
  int GetId() { return CId; }
  int Len() const { return NIdHitH.Len(); }
  int LenBeforeT(const double& T) { int len = 0; while (len < NIdHitH.Len() && NIdHitH[len].Tm <= T) { len++; } return len; }
  int LenAfterT(const double& T) { int len = 0; while (len < NIdHitH.Len() && NIdHitH[NIdHitH.Len()-1-len].Tm >= T) { len++; } return len; }
  int GetNode(const int& i) const { return NIdHitH.GetKey(i); }
  THash<TInt, THitInfo>::TIter BegI() const { return NIdHitH.BegI(); }
  THash<TInt, THitInfo>::TIter EndI() const { return NIdHitH.EndI(); }
  int GetModel() const { return Model; }
  double GetTm(const int& NId) const { return NIdHitH.GetDat(NId).Tm; }
  double GetMaxTm() const { return NIdHitH[NIdHitH.Len()-1].Tm; } // we assume the cascade is sorted
  double GetMinTm() const { return NIdHitH[0].Tm; } // we assume the cascade is sorted
  void Add(const int& NId, const double& HitTm) { NIdHitH.AddDat(NId, THitInfo(NId, HitTm)); }
  void Del(const int& NId) { NIdHitH.DelKey(NId); }
  bool IsNode(const int& NId) const { return NIdHitH.IsKey(NId); }
  void Sort() { NIdHitH.SortByDat(true); }
  bool operator < (const TCascade& Cascade) const {
      return Len() < Cascade.Len(); }
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
  bool operator < (const TNodeInfo& NodeInfo) const {
      return Vol < NodeInfo.Vol; }
};

// Stochastic gradient network inference class
class TNIBs {
public:
  THash<TInt, TCascade> CascH; // cascades, indexed by id
  THash<TInt, TNodeInfo> NodeNmH; // node info (name, volume), indexed by node id
  TStrIntH DomainsIdH; // domain, DomainId hash table
  TStrIntH CascadeIdH; // quote, CascadeId hash table, QuoteId is equivalent to cascadeId

  // cascades per edge
  THash<TIntPr, TIntV> CascPerEdge;

  // network
  TStrFltFltHNEDNet Network;

  // pairwise transmission model
  TModel Model;

  // time horizon per cascade (if it is fixed), and totaltime
  TFlt Window, TotalTime;

  // delta for power-law and k for weibull
  TFlt Delta, K;

  // step (gamma), regularizer (mu), tolerance, and min/max alpha for stochastic gradient descend
  TFlt Gamma, Mu, Aging;
  TRegularizer Regularizer;
  TFlt Tol, MaxAlpha, MinAlpha, InitAlpha;

  // inferred network
  TStrFltFltHNEDNet InferredNetwork;
  TIntFltH TotalCascadesAlpha;

  // gradients (per alpha & cascade)
  TIntFltH AveDiffAlphas;
  THash<TInt, TIntFltH> DiffAlphas;

  // sampled cascades
  TIntIntPrH SampledCascadesH;

  // performance measures
  TFltPrV PrecisionRecall;
  TFltPrV Accuracy, MAE, MSE;

public:
  TNIBs( ) { }
  TNIBs(TSIn& SIn) : CascH(SIn), NodeNmH(SIn), CascPerEdge(SIn), InferredNetwork(SIn) { Model = EXP; }
  void Save(TSOut& SOut) const { CascH.Save(SOut); NodeNmH.Save(SOut); CascPerEdge.Save(SOut); InferredNetwork.Save(SOut); }

  // functions to load text cascades & network files
  void LoadCascadesTxt(TSIn& SIn);
  void LoadGroundTruthTxt(TSIn& SIn);
  void LoadGroundTruthNodesTxt(TSIn& SIn);
  void LoadInferredTxt(TSIn& SIn);
  void LoadInferredNodesTxt(TSIn& SIn);

  // maximum time for synthetic generation, tx model & window per cascade (if any)
  void SetTotalTime(const float& tt) { TotalTime = tt; }
  void SetModel(const TModel& model) { Model = model; }
  void SetWindow(const double& window) { Window = window; }

  // delta for power law & k for weibull
  void SetDelta(const double& delta) { Delta = delta; }
  void SetK(const double& k) { K = k; }

  // optimization parameters
  void SetGamma(const double& gamma) { Gamma = gamma; }
  void SetAging(const double& aging) { Aging = aging; }
  void SetRegularizer(const TRegularizer& reg) { Regularizer = reg; }
  void SetMu(const double& mu) { Mu = mu; }
  void SetTolerance(const double& tol) { Tol = tol; }
  void SetMaxAlpha(const double& ma) { MaxAlpha = ma; }
  void SetMinAlpha(const double& ma) { MinAlpha = ma; }
  void SetInitAlpha(const double& ia) { InitAlpha = ia; }

  // processing cascades
  void AddCasc(const TStr& CascStr, const TModel& Model=EXP);
  void AddCasc(const TCascade& Cascade) { CascH.AddDat(Cascade.CId) = Cascade; }
  void AddCasc(const TIntFltH& Cascade, const int& CId=-1, const TModel& Model=EXP);
  void GenCascade(TCascade& C);
  bool IsCascade(int c) { return CascH.IsKey(c); }
  TCascade & GetCasc(int c) { return CascH.GetDat(c); }
  int GetCascs() { return CascH.Len(); }
  int GetCascadeId(const TStr& Cascade) { return CascadeIdH.GetDat(Cascade); }

  // node info
  int GetNodes() { return InferredNetwork.GetNodes(); }
  void AddNodeNm(const int& NId, const TNodeInfo& Info) { NodeNmH.AddDat(NId, Info); }
  TStr GetNodeNm(const int& NId) const { return NodeNmH.GetDat(NId).Name; }
  TNodeInfo GetNodeInfo(const int& NId) const { return NodeNmH.GetDat(NId); }
  bool IsNodeNm(const int& NId) const { return NodeNmH.IsKey(NId); }
  void SortNodeNmByVol(const bool& asc=false) { NodeNmH.SortByDat(asc); }

  // domains
  void AddDomainNm(const TStr& Domain, const int& DomainId=-1) { DomainsIdH.AddDat(Domain) = TInt(DomainId==-1? DomainsIdH.Len() : DomainId); }
  bool IsDomainNm(const TStr& Domain) const { return DomainsIdH.IsKey(Domain); }
  int GetDomainId(const TStr& Domain) { return DomainsIdH.GetDat(Domain); }

  // get network or graph at a given time
  void GetGroundTruthGraphAtT(const double& Step, PNGraph &GraphAtT);
  void GetGroundTruthNetworkAtT(const double& Step, PStrFltNEDNet& NetworkAtT);
  void GetInferredGraphAtT(const double& Step, PNGraph &GraphAtT);
  void GetInferredNetworkAtT(const double& Step, PStrFltNEDNet& NetworkAtT);

  // reset/init for optimization
  void Reset();
  void Init(const TFltV& Steps);

  // optimization methods
  void SG(const int& NId, const int& Iters, const TFltV& Steps, const TSampling& Sampling, const TStr& ParamSampling=TStr(""), const bool& PlotPerformance=false);
  void BSG(const int& NId, const int& Iters, const TFltV& Steps, const int& BatchLen, const TSampling& Sampling, const TStr& ParamSampling=TStr(""), const bool& PlotPerformance=false);
  void FG(const int& NId, const int& Iters, const TFltV& Steps);

  // auxiliary function for optimization
  void UpdateDiff(const TOptMethod& OptMethod, const int& NId, TCascade& Cascade, TIntPrV& AlphasToUpdate, const double& CurrentTime=TFlt::Mx);

  // functions to compute burstiness
  void find_C( int t, TFltV &x, TFltVV &C, const int& k, const double& s, const double& gamma, const double& T );
  void find_min_state( TFltVV &C, TIntV &states, const int& k, const double& s, const double& gamma, const double& T );
  void LabelBurstAutomaton(const int& SrcId, const int& DstId, TIntV &state_labels, TFltV &state_times, const bool& inferred=false, const int& k = 5, const double& s = 2.0, const double& gamma = 1.0, const TSecTm& MinTime=TSecTm(), const TSecTm& MaxTime=TSecTm() );

  // function to compute performance for a particular time step and node given groundtruth + inferred network
  void ComputePerformanceNId(const int& NId, const int& Step, const TFltV& Steps);

  // storing ground truth and inferred network in pajek and text format
  void SaveInferredPajek(const TStr& OutFNm, const double& Step, const TIntV& NIdV=TIntV());
  void SaveInferred(const TStr& OutFNm, const TIntV& NIdV=TIntV());
  void SaveInferred(const TStr& OutFNm, const double& Step, const TIntV& NIdV=TIntV());
  void SaveInferredEdges(const TStr& OutFNm);

  // store network
  void SaveGroundTruthPajek(const TStr& OutFNm, const double& Step);
  void SaveGroundTruth(const TStr& OutFNm);

  // storing NodeId, site name
  void SaveSites(const TStr& OutFNm, const TIntFltVH& CascadesPerNode=TIntFltVH());

  // storing cascades in text format
  void SaveCascades(const TStr& OutFNm);
};

#endif
