#ifndef snap_mag_h
#define snap_mag_h

#include "Snap.h"
#include "kronecker.h"

/////////////////////////////////////////////////
// MAG affinity matrix

class TMAGAffMtx {
  static const double NInf;
private:
  TInt MtxDim;
  TFltV SeedMtx;
public:
  TMAGAffMtx() : MtxDim(-1), SeedMtx() { }
  TMAGAffMtx(const int& Dim) : MtxDim(Dim), SeedMtx(Dim*Dim) { }
  TMAGAffMtx(const TFltV& SeedMatrix);
  TMAGAffMtx(const TMAGAffMtx& Kronecker) : MtxDim(Kronecker.MtxDim), SeedMtx(Kronecker.SeedMtx) { }
  TMAGAffMtx& operator = (const TMAGAffMtx& Kronecker);
  bool operator == (const TMAGAffMtx& Kronecker) const { return SeedMtx==Kronecker.SeedMtx; }
  int GetPrimHashCd() const { return SeedMtx.GetPrimHashCd(); }
  int GetSecHashCd() const { return SeedMtx.GetSecHashCd(); }

  // seed matrix
  int GetDim() const { return MtxDim; }
  int Len() const { return SeedMtx.Len(); }
  bool Empty() const { return SeedMtx.Empty(); }
  bool IsProbMtx() const; // probability (not log-lihelihood) matrix

  TFltV& GetMtx() { return SeedMtx; }
  const TFltV& GetMtx() const { return SeedMtx; }
  void SetMtx(const TFltV& ParamV) { SeedMtx = ParamV; }
  void SetRndMtx(TRnd& Rnd, const int& PrmMtxDim = 2, const double& MinProb = 0.0);
  void PutAllMtx(const double& Val) { SeedMtx.PutAll(Val); }
  void GenMtx(const int& Dim) { MtxDim=Dim;  SeedMtx.Gen(Dim*Dim); }
  void SetEpsMtx(const double& Eps1, const double& Eps0, const int& Eps1Val=1, const int& Eps0Val=0);
  void AddRndNoise(TRnd& Rnd, const double& SDev);
  TStr GetMtxStr() const;

  const double& At(const int& Row, const int& Col) const { return SeedMtx[MtxDim*Row+Col].Val; }
  double& At(const int& Row, const int& Col) { return SeedMtx[MtxDim*Row+Col].Val; }
  const double& At(const int& ValN) const { return SeedMtx[ValN].Val; }
  double& At(const int& ValN) { return SeedMtx[ValN].Val; }

  double GetMtxSum() const;
  double GetRowSum(const int& RowId) const;
  double GetColSum(const int& ColId) const;
  double Normalize();

  void GetLLMtx(TMAGAffMtx& LLMtx);
  void GetProbMtx(TMAGAffMtx& ProbMtx);
  void Swap(TMAGAffMtx& Mtx);

  void Dump(const TStr& MtxNm = TStr(), const bool& Sort = false) const;
  static double GetAvgAbsErr(const TMAGAffMtx& Mtx1, const TMAGAffMtx& Mtx2); // avg L1 on (sorted) parameters
  static double GetAvgFroErr(const TMAGAffMtx& Mtx1, const TMAGAffMtx& Mtx2); // avg L2 on (sorted) parameters
  static TMAGAffMtx GetMtx(TStr MatlabMtxStr);
  static TMAGAffMtx GetRndMtx(TRnd& Rnd, const int& Dim = 2, const double& MinProb = 0.0);
};

typedef TVec<TMAGAffMtx> TMAGAffMtxV;

/////////////////////////////////////////////////
// Simple MAG node attributes (Same Bernoulli for every attribute)

class TMAGNodeSimple {
public:
	static TRnd Rnd;
private:
	TFlt Mu;
	TInt Dim;
public:
	TMAGNodeSimple() : Mu(0.5), Dim(-1) {}
	TMAGNodeSimple(const int& _Dim, const double& _Mu = 0.5) : Dim(_Dim) {  IAssert(_Mu >= 0.0 && _Mu <= 1.0);  Mu = _Mu;  }
	TMAGNodeSimple(const TMAGNodeSimple& Simple) : Dim(Simple.Dim), Mu(Simple.Mu) {}

	TMAGNodeSimple& operator=(const TMAGNodeSimple& Simple) {  Mu = Simple.Mu;  Dim = Simple.Dim;  return (*this);  }
	
	void LoadTxt(const TStr& InFNm);
	void SaveTxt(TStrV& OutStrV) const;
	void AttrGen(TIntVV& AttrVV, const int& NNodes);
};

/////////////////////////////////////////////////
// MAG node attributes with (different) Bernoulli 

class TMAGNodeBern {
public:
	static TRnd Rnd;
private:
	TFltV MuV;
	TInt Dim;
public:
	TMAGNodeBern() : MuV(), Dim(-1) {}
	TMAGNodeBern(const int& _Dim, const double& _Mu = 0.5) : Dim(_Dim), MuV(_Dim) {  IAssert(_Mu >= 0.0 && _Mu <= 1.0);  MuV.PutAll(_Mu);  }
	TMAGNodeBern(const TFltV& _MuV) : Dim(_MuV.Len()), MuV(_MuV) {}
	TMAGNodeBern(const TMAGNodeBern& Dist) : Dim(Dist.Dim), MuV(Dist.MuV) {}

	TMAGNodeBern& operator=(const TMAGNodeBern& Dist);

	void SetMuV(const TFltV& _MuV) {  MuV = _MuV;  }
	const TFltV& GetMuV() const {  return MuV;  }
	void SetMu(const int& Attr, const double& Prob) {  IAssert(Prob >= 0.0 && Prob <= 1.0);  MuV[Attr] = Prob;  }
	double GetMu(const int& Attr) const {  return MuV[Attr];  }

	void LoadTxt(const TStr& InFNm);
	void SaveTxt(TStrV& OutStrV) const;
	void AttrGen(TIntVV& AttrVV, const int& NNodes);
};

/////////////////////////////////////////////////
// MAG node attributes with Beta + Bernoulli family

class TMAGNodeBeta {
public:
	static TRnd Rnd;
private:
	TFltV AlphaV;
	TFltV BetaV;
	TFltV MuV;
	TInt Dim;
	TBool Dirty;
public:
	TMAGNodeBeta() : AlphaV(), BetaV(), MuV(), Dim(-1), Dirty(true) {}
	TMAGNodeBeta(const int& _Dim, const double& Alpha = 1, const double& Beta = 1) : AlphaV(_Dim), BetaV(_Dim), MuV(_Dim), Dim(_Dim), Dirty(true) {  AlphaV.PutAll(Alpha);  BetaV.PutAll(Beta);  }
	TMAGNodeBeta(const TFltV& _AlphaV, const TFltV& _BetaV) : AlphaV(_AlphaV), BetaV(_BetaV), Dirty(true) {  IAssert(_AlphaV.Len() == _BetaV.Len());  Dim = _AlphaV.Len();  MuV.Gen(Dim);  }

	TMAGNodeBeta& operator=(const TMAGNodeBeta& Dist);

	void SetBeta(const int& Attr, const double& Alpha, const double& Beta);
	void GetBeta(const int& Attr, double& Alpha, double& Beta) const {  IAssert(Attr < Dim);  Alpha = AlphaV[Attr];  Beta = BetaV[Attr];  }
	void SetBetaV(const TFltV& _AlphaV, const TFltV& _BetaV);
	void GetBetaV(TFltV& _AlphaV, TFltV& _BetaV) const {  _AlphaV = AlphaV;  _BetaV = BetaV;  }
	bool GetMuV(TFltV& _MuV) const {  if(!Dirty)  {  _MuV = MuV;  }  return Dirty; }
	
	void LoadTxt(const TStr& InFNm);
	void SaveTxt(TStrV& OutStrV) const;
	void AttrGen(TIntVV& AttrVV, const int& NNodes);
};

/////////////////////////////////////////////////
// MAG model param. (node attribtes + affinity matrices)

template <class TNodeAttr>
class TMAGParam {
public:
	static TRnd Rnd;
private:
	TInt NNodes;
	TInt NAttrs;
	TNodeAttr NodeAttr;
	TMAGAffMtxV AffMtxV;

public:
	TMAGParam() : NNodes(-1), NAttrs(-1), NodeAttr(), AffMtxV() {}
	TMAGParam(const int& NumNodes, const int& NumAttrs) : NNodes(NumNodes), NAttrs(NumAttrs), NodeAttr(), AffMtxV(NumAttrs) {}
	TMAGParam(const int& NumNodes, const TMAGAffMtxV& MtxV) : NNodes(NumNodes), NAttrs(MtxV.Len()), NodeAttr(), AffMtxV(MtxV) {}
	TMAGParam(const int& NumNodes, const TStr& ConfFNm) : NNodes(NumNodes) { LoadTxt(ConfFNm); }
	TMAGParam(const TMAGParam<TNodeAttr>& MAGParam) : NNodes(MAGParam.NNodes), NAttrs(MAGParam.NAttrs), NodeAttr(MAGParam.NodeAttr), AffMtxV(MAGParam.AffMtxV) {}
	TMAGParam(const int& NumNodes, const int& NumAttrs, const TNodeAttr& Dist) : NNodes(NumNodes), NAttrs(NumAttrs), NodeAttr(Dist), AffMtxV(NumAttrs) {}

	TMAGParam<TNodeAttr>& operator=(const TMAGParam<TNodeAttr>& MAGParam);

	void Clr() {  AffMtxV.Clr();  }

	void LoadTxt(const TStr& InFNm);
	void SaveTxt(const TStr& OutFNm) const;

	void SetNodes(const int& Nodes) {  NNodes = Nodes;  }
	const int GetNodes() const {  return NNodes;  }

	void SetAttrs(const int& Attrs);
	const int GetAttrs() const {  return NAttrs;  }

	void SetNodeAttr(const TNodeAttr& Dist) {  NodeAttr = Dist;  }
	const TNodeAttr& GetNodeAttr() const {  return NodeAttr;  }

	const int GetDim(const int& Attr) const {  return AffMtxV[Attr].GetDim();  }
	void GetDimV(TIntV& DimV) const;

	void SetMtxVal(const int& Attr, const int& Row, const int& Col, const double& Val) {  AffMtxV[Attr].At(Row, Col) = Val;  }
	void SetMtx(const int& Attr, const TMAGAffMtx& Mtx) {  AffMtxV[Attr] = Mtx;  }
	void SetMtxV(const TMAGAffMtxV& MtxV) {  NAttrs = AffMtxV.Len();  AffMtxV = MtxV;  }
	void SetMtxAll(const TMAGAffMtx& Mtx) {  AffMtxV.PutAll(Mtx);  }
	const double GetMtxVal(const int& Attr, const int& Row, const int& Col) const {  return AffMtxV[Attr].At(Row, Col);  }
	const TMAGAffMtx& GetMtx(const int& Attr) const {  return AffMtxV[Attr];  }
	void GetMtxV(TMAGAffMtxV& MtxV) const {  MtxV = AffMtxV;  }

	PNGraph GenMAG(TIntVV& AttrVV, const bool& IsDir = false, const int& Seed = 1);
	PNGraph GenAttrMAG(const TIntVV& AttrVV, const bool& IsDir = false, const int& Seed = 1);
};

template <class TNodeAttr>
TMAGParam<TNodeAttr>& TMAGParam<TNodeAttr>::operator=(const TMAGParam<TNodeAttr>& MAGParam) {
	NNodes = MAGParam.NNodes;
	NAttrs = MAGParam.NAttrs;
	AffMtxV = MAGParam.AffMtxV;
	NodeAttr = MAGParam.NodeAttr;

	return (*this);
};

template <class TNodeAttr>
void TMAGParam<TNodeAttr>::LoadTxt(const TStr& InFNm) {
	NodeAttr.LoadTxt(InFNm);

	char buf[1024];
	FILE *fp = fopen(InFNm.CStr(), "r");
	NAttrs = 0;
	AffMtxV.Gen(10, 0);
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		strtok(buf, "&");
		char *token = strtok(NULL, "\r\n");
		NAttrs++;
		AffMtxV.Add(TMAGAffMtx::GetMtx(TStr(token)));
	}
	fclose(fp);
};

template <class TNodeAttr>
void TMAGParam<TNodeAttr>::SaveTxt(const TStr& OutFNm) const {
	TStrV OutStrV;
	NodeAttr.SaveTxt(OutStrV);

	FILE *fp = fopen(OutFNm.CStr(), "w");
	fprintf(fp, "# %d nodes with %d attributes\n", NNodes(), NAttrs());
	for(int i = 0; i < NAttrs; i++) {
		TStr MtxStr = AffMtxV[i].GetMtxStr();
		fprintf(fp, "%s & %s\n", OutStrV[i].CStr(), MtxStr.GetSubStr(1, MtxStr.Len()-2).CStr());
	}
	fclose(fp);
};

template <class TNodeAttr>
void TMAGParam<TNodeAttr>::SetAttrs(const int& Attrs) {
	NAttrs = Attrs;
	AffMtxV.Gen(Attrs);
};

template <class TNodeAttr>
void TMAGParam<TNodeAttr>::GetDimV(TIntV& DimV) const {
	DimV.Gen(NAttrs);
	for(int i = 0; i < NAttrs; i++) {
		DimV[i] = AffMtxV[i].GetDim();
	}
};

template <class TNodeAttr>
PNGraph TMAGParam<TNodeAttr>::GenAttrMAG(const TIntVV& AttrVV, const bool& IsDir, const int& Seed) {
	PNGraph Graph = TNGraph::New(NNodes, -1);
	for(int i = 0; i < NNodes; i++) {
		Graph->AddNode(i);
	}

	if(Seed > 0) {
		TNodeAttr::Rnd.PutSeed(Seed);
	}

	IAssert(AttrVV.GetYDim() == NAttrs);
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			IAssert(AttrVV.At(i, l) < AffMtxV[l].GetDim());
		}
	}

	for(int i = 0; i < NNodes; i++) {
		for(int j = i; j < NNodes; j++) {
			double Prob = 1.0;
			for(int l = 0; l < NAttrs; l++) {
				Prob *= AffMtxV[l].At(AttrVV.At(i, l), AttrVV.At(j, l));
			}
			
			if(TNodeAttr::Rnd.GetUniDev() < Prob) {
				Graph->AddEdge(i, j);
				if(! IsDir && i != j) {  Graph->AddEdge(j, i);  }
			}

			if(IsDir && i != j) {
				Prob = 1.0;
				for(int l = 0; l < NAttrs; l++) {
					Prob *= AffMtxV[l].At(AttrVV.At(j, l), AttrVV.At(i, l));
				}
				if(TNodeAttr::Rnd.GetUniDev() < Prob) {
					Graph->AddEdge(j, i);
				}
			}
		}
	}

	return Graph;
};

template <class TNodeAttr>
PNGraph TMAGParam<TNodeAttr>::GenMAG(TIntVV& AttrVV, const bool& IsDir, const int& Seed) {
	TNodeAttr::Rnd.PutSeed(Seed);
	NodeAttr.AttrGen(AttrVV, NNodes);

	IAssert(AttrVV.GetYDim() == NAttrs);
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			IAssert(AttrVV.At(i, l) < AffMtxV[l].GetDim());
		}
	}

	PNGraph Graph = GenAttrMAG(AttrVV, IsDir, 0);
	return Graph;

/*
	for(int i = 0; i < NNodes; i++) {
		for(int j = i; j < NNodes; j++) {
			double Prob = 1.0;
			for(int l = 0; l < NAttrs; l++) {
				Prob *= AffMtxV[l].At(AttrVV.At(i, l), AttrVV.At(j, l));
			}
			if(TNodeAttr::Rnd.GetUniDev() < Prob) {
				Graph->AddEdge(i, j);
				if(! IsDir && i != j) {  Graph->AddEdge(j, i);  }
			}

			if(IsDir && i != j) {
				Prob = 1.0;
				for(int l = 0; l < NAttrs; l++) {
					Prob *= AffMtxV[l].At(AttrVV.At(j, l), AttrVV.At(i, l));
				}
				if(TNodeAttr::Rnd.GetUniDev() < Prob) {
					Graph->AddEdge(j, i);
				}
			}
		}
	}

	return Graph;
*/
};


///////////////////////////////////////////////////////////////
// MAGFit with Bernoulli node attributes

class TMAGFitBern {
private:
	TFltVV PhiVV;
	TBoolVV KnownVV;
	PNGraph Graph;
	TMAGParam<TMAGNodeBern> Param;
	bool ESpeedUp, MSpeedUp, Debug;
	TFltV AvgPhiV;
	TFltVV AvgPhiPairVV;
	TFlt NormConst;

	TVec<TFltV> MuHisV;
	TVec<TMAGAffMtxV> MtxHisV;
	TFltV LLHisV;
	
public:
	TMAGFitBern() : PhiVV(), KnownVV(), Param(), Graph(), ESpeedUp(true), MSpeedUp(true), AvgPhiV(), AvgPhiPairVV(), Debug(false), NormConst(1.0)  { }
	TMAGFitBern(const PNGraph& G, const int& NAttrs) : Graph(G), PhiVV(G->GetNodes(), NAttrs), KnownVV(G->GetNodes(), NAttrs), Param(G->GetNodes(), NAttrs), ESpeedUp(true), MSpeedUp(true), AvgPhiV(NAttrs), AvgPhiPairVV(NAttrs, NAttrs), Debug(false), NormConst(1.0) { }
	
	TMAGFitBern(const PNGraph& G, const TStr& InitFNm) : Param(G->GetNodes(), InitFNm), ESpeedUp(true), MSpeedUp(true), Debug(false), NormConst(1.0) {
		const int NNodes = G->GetNodes();
		const int NAttrs = Param.GetAttrs();

		printf("NAttrs = %d\n", NAttrs);
		SetGraph(G);
		PhiVV.Gen(NNodes, NAttrs);
		KnownVV.Gen(NNodes, NAttrs);
		AvgPhiV.Gen(NAttrs);
		AvgPhiPairVV.Gen(NAttrs, NAttrs);
	}

	void Clr() {  PhiVV.Clr();  KnownVV.Clr();  }
	void SaveTxt(const TStr& FNm);

	const int GetNodes() const { return Param.GetNodes(); }
	const int GetAttrs() const { return Param.GetAttrs(); }
	const TMAGParam<TMAGNodeBern>& GetParams() const { return Param; }
	const TMAGNodeBern& GetNodeAttr() const {  return Param.GetNodeAttr();  }
	const TFltV& GetMuV() const {  const TMAGNodeBern& Dist = Param.GetNodeAttr();  return Dist.GetMuV();  }
	void SetMuV(const TFltV& MuV) {  TMAGNodeBern Dist = Param.GetNodeAttr();  Dist.SetMuV(MuV);  Param.SetNodeAttr(Dist);  }
	void GetMtxV(TMAGAffMtxV& MtxV) const {  Param.GetMtxV(MtxV);  }
	void SetMtxV(const TMAGAffMtxV& MtxV) {  Param.SetMtxV(MtxV);  }
	PNGraph GetGraph() {  return Graph;  }
	void SetGraph(const PNGraph& GraphPt);

	void SetDebug(const bool _Debug) {  Debug = _Debug;  }
	void SetAlgConf(const bool EStep = true, const bool MStep = true)  {  ESpeedUp = EStep;  MSpeedUp = MStep;  }

	void Init(const TFltV& MuV, const TMAGAffMtxV& AffMtxV);
//	void PerturbInit(const TFltV& MuV, const TMAGAffMtxV& AffMtxV, const double& PerturbRate);
	void RandomInit(const TFltV& MuV, const TMAGAffMtxV& AffMtxV, const int& Seed);
	const TFltVV& GetPhiVV() const { return PhiVV; }
	const TFltVV& SetPhiVV(const TIntVV& AttrVV, const int KnownIds = 0);
	const TFltVV& SetPhiVV(const TFltVV& AttrVV, const int KnownIds = 0) {  PhiVV = AttrVV;  KnownVV.Gen(PhiVV.GetXDim(), PhiVV.GetYDim());  KnownVV.PutAll(false);  for(int i = 0; i < KnownIds; i++) {  KnownVV.PutY(i, true); } }

	const double GetInCoeff(const int& i, const int& j, const int& l, const int& A, const TMAGAffMtx& Theta) const;
	const double GetAvgInCoeff(const int& i, const int& AId, const int& A, const TMAGAffMtx& Theta) const;
	const double GetOutCoeff(const int& i, const int& j, const int& l, const int& A, const TMAGAffMtx& Theta) const;
	const double GetAvgOutCoeff(const int& i, const int& AId, const int& A, const TMAGAffMtx& Theta) const;
	const double GetProbPhi(const int& NId1, const int& NId2, const int& AId, const int& Attr1, const int& Attr2) const;
	const double GetProbMu(const int& NId1, const int& NId2, const int& AId, const int& Attr1, const int& Attr2, const bool Left = false, const bool Right = false) const;
	const double GetThetaLL(const int& NId1, const int& NId2, const int& AId) const;
	const double GetAvgThetaLL(const int& NId1, const int& NId2, const int& AId, const bool Left = false, const bool Right = false) const;
	const double GetSqThetaLL(const int& NId1, const int& NId2, const int& AId) const;
	const double GetAvgSqThetaLL(const int& NId1, const int& NId2, const int& AId, const bool Left = false, const bool Right = false) const;
	const double GetProdLinWeight(const int& NId1, const int& NId2) const;
	const double GetAvgProdLinWeight(const int& NId1, const int& NId2, const bool Left = false, const bool Right = false) const;
	const double GetProdSqWeight(const int& NId1, const int& NId2) const;
	const double GetAvgProdSqWeight(const int& NId1, const int& NId2, const bool Left = false, const bool Right = false) const;
	
	const double GetEstNoEdgeLL(const int& NId, const int& AId) const;
	const double GradPhiMI(const double& x, const int& NId, const int& AId, const double& Lambda, const double& DeltaQ, const TFltVV& CntVV);
	const double ObjPhiMI(const double& x, const int& NId, const int& AId, const double& Lambda, const double& Q0, const double& Q1, const TFltVV& CntVV);
	const double UpdatePhiMI(const double& Lambda, const int& NId, const int& AId, double& Phi);
	const double UpdateApxPhiMI(const double& Lambda, const int& NId, const int& AId, double& Phi, TFltVV& ProdVV);
	const double UpdatePhi(const int& NId, const int& AId, double& Phi);
	const double UpdateMu(const int& AId);
	const void PrepareUpdateAffMtx(TFltVV& ProdVV, TFltVV& SqVV);
	const void PrepareUpdateApxAffMtx(TFltVV& ProdVV, TFltVV& SqVV);
	
	const double UpdateAffMtx(const int& AId, const double& LrnRate, const double& MaxGrad, const double& Lambda, TFltVV& ProdVV, TFltVV& SqVV, TMAGAffMtx& NewMtx);
	const double UpdateAffMtxV(const int& GradIter, const double& LrnRate, const double& MaxGrad, const double& Lambda, const int& NReal = 0);
	const void GradAffMtx(const int& AId, const TFltVV& ProdVV, const TFltVV& SqVV, const TMAGAffMtx& CurMtx, TFltV& GradV);
	const void GradApxAffMtx(const int& AId, const TFltVV& ProdVV, const TFltVV& SqVV, const TMAGAffMtx& CurMtx, TFltV& GradV);

	/* EM algorithm */
	double DoEStepOneIter(const TFltV& TrueMuV, TFltVV& NewPhi, const double& Lambda);
	double DoEStepApxOneIter(const TFltV& TrueMuV, TFltVV& NewPhi, const double& Lambda);
	double DoEStep(const TFltV& TrueMuV, const int& NIter, double& LL, const double& Lambda);
	void DoMStep(const int& GradIter, const double& LrnRate, const double& MaxGrad, const double& Lambda, const int& NReal = 0);
	void DoEMAlg(const int& NStep, const int& NEstep, const int& NMstep, const double& LrnRate, const double& MaxGrad, const double& Lambda, const double& ReInit, const int& NReal = 0);

	void CountAttr(TFltV& EstMuV) const;
	void SortAttrOrdering(const TFltV& TrueMuV, TIntV& IndexV) const;

	const double ComputeJointOneLL(const TIntVV& AttrVV) const;
	const double ComputeJointLL(int NSample) const;
	const double ComputeJointAdjLL(const TIntVV& AttrVV) const;
	const double ComputeApxLL() const;
	const double ComputeApxAdjLL() const;

	void MakeCCDF(const TFltPrV& RawV, TFltPrV& CcdfV);
	void PlotProperties(const TStr& FNm);

	static const double ComputeMI(const TIntVV& AttrV, const int AId1, const int AId2);
	static const double ComputeMI(const TFltVV& AttrV, const int AId1, const int AId2);
	static const double ComputeMI(const TIntVV& AttrV);
	static const double ComputeMI(const TFltVV& AttrV);
	void NormalizeAffMtxV(TMAGAffMtxV& MtxV, const bool UseMu = false);
	void UnNormalizeAffMtxV(TMAGAffMtxV& MtxV, const bool UseMu = false);
private:
	const bool NextPermutation(TIntV& IndexV) const;
};

const double LogSumExp(const double LogVal1, const double LogVal2);
const double LogSumExp(const TFltV& LogValV);
const double LogSumExp(const double *LogValArray, const int Len);

#endif	//	snap_mag_h
