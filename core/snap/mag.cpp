#include "stdafx.h"
#include "mag.h"

TRnd TMAGNodeSimple::Rnd = TRnd(0);
TRnd TMAGNodeBern::Rnd = TRnd(0);
TRnd TMAGNodeBeta::Rnd = TRnd(0);


/////////////////////////////////////////////////
// MAG affinity matrix

const double TMAGAffMtx::NInf = -DBL_MAX;

TMAGAffMtx::TMAGAffMtx(const TFltV& SeedMatrix) : SeedMtx(SeedMatrix) {
  MtxDim = (int) sqrt((double)SeedMatrix.Len());
  IAssert(MtxDim*MtxDim == SeedMtx.Len());
}

TMAGAffMtx& TMAGAffMtx::operator = (const TMAGAffMtx& Kronecker) {
  if (this != &Kronecker){
    MtxDim=Kronecker.MtxDim;
    SeedMtx=Kronecker.SeedMtx;
  }
  return *this;
}

bool TMAGAffMtx::IsProbMtx() const {
  for (int i = 0; i < Len(); i++) {
    if (At(i) < 0.0 || At(i) > 1.0) return false;
  }
  return true;
}

void TMAGAffMtx::SetRndMtx(TRnd& Rnd, const int& PrmMtxDim, const double& MinProb) {
  MtxDim = PrmMtxDim;
  SeedMtx.Gen(MtxDim*MtxDim);
  for (int p = 0; p < SeedMtx.Len(); p++) {
    do {
      SeedMtx[p] = Rnd.GetUniDev();
    } while (SeedMtx[p] < MinProb);
  }
}

void TMAGAffMtx::SetEpsMtx(const double& Eps1, const double& Eps0, const int& Eps1Val, const int& Eps0Val) {
  for (int i = 0; i < Len(); i++) {
    double& Val = At(i);
    if (Val == Eps1Val) Val = double(Eps1);
    else if (Val == Eps0Val) Val = double(Eps0);
  }
}

void TMAGAffMtx::AddRndNoise(TRnd& Rnd, const double& SDev) {
  Dump("before");
  double NewVal;
  int c =0;
  for (int i = 0; i < Len(); i++) {
    for(c = 0; ((NewVal = At(i)*Rnd.GetNrmDev(1, SDev, 0.8, 1.2)) < 0.01 || NewVal>0.99) && c <1000; c++) { }
    if (c < 999) { At(i) = NewVal; } else { printf("XXXXX\n"); }
  }
  Dump("after");
}

TStr TMAGAffMtx::GetMtxStr() const {
  TChA ChA("[");
  for (int i = 0; i < Len(); i++) {
    ChA += TStr::Fmt("%g", At(i));
    if ((i+1)%GetDim()==0 && (i+1<Len())) { ChA += "; "; }
    else if (i+1<Len()) { ChA += " "; }
  }
  ChA += "]";
  return TStr(ChA);
}

void TMAGAffMtx::GetLLMtx(TMAGAffMtx& LLMtx) {
  LLMtx.GenMtx(MtxDim);
  for (int i = 0; i < Len(); i++) {
    if (At(i) != 0.0) { LLMtx.At(i) = log(At(i)); }
    else { LLMtx.At(i) = NInf; }
  }
}

void TMAGAffMtx::GetProbMtx(TMAGAffMtx& ProbMtx) {
  ProbMtx.GenMtx(MtxDim);
  for (int i = 0; i < Len(); i++) {
    if (At(i) != NInf) { ProbMtx.At(i) = exp(At(i)); }
    else { ProbMtx.At(i) = 0.0; }
  }
}

void TMAGAffMtx::Swap(TMAGAffMtx& Mtx) {
  ::Swap(MtxDim, Mtx.MtxDim);
  SeedMtx.Swap(Mtx.SeedMtx);
}

double TMAGAffMtx::GetMtxSum() const {
  double Sum = 0;
  for (int i = 0; i < Len(); i++) {
    Sum += At(i); }
  return Sum;
}

double TMAGAffMtx::GetRowSum(const int& RowId) const {
  double Sum = 0;
  for (int c = 0; c < GetDim(); c++) {
    Sum += At(RowId, c); }
  return Sum;
}

double TMAGAffMtx::GetColSum(const int& ColId) const {
  double Sum = 0;
  for (int r = 0; r < GetDim(); r++) {
    Sum += At(r, ColId); }
  return Sum;
}

double TMAGAffMtx::Normalize() {
	double Sum = GetMtxSum();
	if(Sum == 0) {
		return 0;
	}

	for(int i = 0; i < Len(); i++) {
		At(i) = At(i) / Sum;
	}
	return Sum;
}

void TMAGAffMtx::Dump(const TStr& MtxNm, const bool& Sort) const {
  /*printf("%s: %d x %d\n", MtxNm.Empty()?"Mtx":MtxNm.CStr(), GetDim(), GetDim());
  for (int r = 0; r < GetDim(); r++) {
    for (int c = 0; c < GetDim(); c++) { printf("  %8.2g", At(r, c)); }
    printf("\n");
  }*/
  if (! MtxNm.Empty()) printf("%s\n", MtxNm.CStr());
  double Sum=0.0;
  TFltV ValV = SeedMtx;
  if (Sort) { ValV.Sort(false); }
  for (int i = 0; i < ValV.Len(); i++) {
    printf("  %10.4g", ValV[i]());
    Sum += ValV[i];
    if ((i+1) % GetDim() == 0) { printf("\n"); }
  }
  printf(" (sum:%.4f)\n", Sum);
}

// average difference in the parameters
double TMAGAffMtx::GetAvgAbsErr(const TMAGAffMtx& Mtx1, const TMAGAffMtx& Mtx2) {
  TFltV P1 = Mtx1.GetMtx();
  TFltV P2 = Mtx2.GetMtx();
  IAssert(P1.Len() == P2.Len());
  P1.Sort();  P2.Sort();
  double delta = 0.0;
  for (int i = 0; i < P1.Len(); i++) {
    delta += fabs(P1[i] - P2[i]);
  }
  return delta/P1.Len();
}

// average L2 difference in the parameters
double TMAGAffMtx::GetAvgFroErr(const TMAGAffMtx& Mtx1, const TMAGAffMtx& Mtx2) {
  TFltV P1 = Mtx1.GetMtx();
  TFltV P2 = Mtx2.GetMtx();
  IAssert(P1.Len() == P2.Len());
  P1.Sort();  P2.Sort();
  double delta = 0.0;
  for (int i = 0; i < P1.Len(); i++) {
    delta += pow(P1[i] - P2[i], 2);
  }
  return sqrt(delta/P1.Len());
}

// get matrix from matlab matrix notation
TMAGAffMtx TMAGAffMtx::GetMtx(TStr MatlabMtxStr) {
  TStrV RowStrV, ColStrV;
  MatlabMtxStr.ChangeChAll(',', ' ');
  MatlabMtxStr.SplitOnAllCh(';', RowStrV);  IAssert(! RowStrV.Empty());
  RowStrV[0].SplitOnWs(ColStrV);    IAssert(! ColStrV.Empty());
  const int Rows = RowStrV.Len();
  const int Cols = ColStrV.Len();
  IAssert(Rows == Cols);
  TMAGAffMtx Mtx(Rows);
  for (int r = 0; r < Rows; r++) {
    RowStrV[r].SplitOnWs(ColStrV);
    IAssert(ColStrV.Len() == Cols);
    for (int c = 0; c < Cols; c++) {
      Mtx.At(r, c) = (double) ColStrV[c].GetFlt(); }
  }
  return Mtx;
}

TMAGAffMtx TMAGAffMtx::GetRndMtx(TRnd& Rnd, const int& Dim, const double& MinProb) {
  TMAGAffMtx Mtx;
  Mtx.SetRndMtx(Rnd, Dim, MinProb);
  return Mtx;
}


/////////////////////////////////////////////////
// Simple MAG node attributes (Same Bernoulli for every attribute)

void TMAGNodeSimple::AttrGen(TIntVV& AttrVV, const int& NNodes) {
	IAssert(Dim > 0);
	AttrVV.Gen(NNodes, Dim);
	AttrVV.PutAll(0);
	
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < Dim; l++) {
			if((TMAGNodeSimple::Rnd).GetUniDev() > Mu) {
				AttrVV.At(i, l) = 1;
			}
		}
	}
}

void TMAGNodeSimple::LoadTxt(const TStr& InFNm) {
	FILE *fp = fopen(InFNm.CStr(), "r");
	IAssertR(fp != NULL, "File does not exist: " + InFNm);

	char buf[128];
	char *token;
	TStr TokenStr;
	TFlt Val;

	token = strtok(buf, "&");
	token = strtok(token, " \t");
	TokenStr = TStr(token);
	Mu = TokenStr.GetFlt(Val);

	fclose(fp);
}

void TMAGNodeSimple::SaveTxt(TStrV& OutStrV) const {
	OutStrV.Gen(Dim, 0);

	for(int i = 0; i < Dim; i++) {
		OutStrV.Add(TStr::Fmt("%f", double(Mu)));
	}
}


/////////////////////////////////////////////////
// MAG node attributes with (different) Bernoulli 

TMAGNodeBern& TMAGNodeBern::operator=(const TMAGNodeBern& Dist) {
	MuV = Dist.MuV;
	Dim = Dist.Dim;
	return (*this);
}

void TMAGNodeBern::AttrGen(TIntVV& AttrVV, const int& NNodes) {
	IAssert(Dim > 0);
	AttrVV.Gen(NNodes, Dim);
	AttrVV.PutAll(0);
	
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < Dim; l++) {
			if((TMAGNodeBern::Rnd).GetUniDev() > MuV[l]) {
				AttrVV.At(i, l) = 1;
			}
		}
	}
}

void TMAGNodeBern::LoadTxt(const TStr& InFNm) {
	FILE *fp = fopen(InFNm.CStr(), "r");
	IAssertR(fp != NULL, "File does not exist: " + InFNm);

	Dim = 0;
	MuV.Gen(10, 0);

	char buf[128];
	char *token;
	TStr TokenStr;
	TFlt Val;

	while(fgets(buf, sizeof(buf), fp) != NULL) {
		token = strtok(buf, "&");
		token = strtok(token, " \t");
		TokenStr = TStr(token);
		MuV.Add(TokenStr.GetFlt(Val));
	}
	Dim = MuV.Len();

	fclose(fp);
}

void TMAGNodeBern::SaveTxt(TStrV& OutStrV) const {
	OutStrV.Gen(Dim, 0);

	for(int i = 0; i < Dim; i++) {
		OutStrV.Add(TStr::Fmt("%f", double(MuV[i])));
	}
}


/////////////////////////////////////////////////
// MAG node attributes with Beta + Bernoulli family

TMAGNodeBeta& TMAGNodeBeta::operator=(const TMAGNodeBeta& Dist) {
	AlphaV = Dist.AlphaV;
	BetaV = Dist.BetaV;
	Dim = Dist.Dim;
	MuV = Dist.MuV;
	Dirty = Dist.Dirty;
	return (*this);
}

void TMAGNodeBeta::SetBeta(const int& Attr, const double& Alpha, const double& Beta) {
	IAssert(Attr < Dim);
	AlphaV[Attr] = Alpha;
	BetaV[Attr] = Beta;
	Dirty = true;
}
	
void TMAGNodeBeta::SetBetaV(const TFltV& _AlphaV, const TFltV& _BetaV) {
	IAssert(_AlphaV.Len() == _BetaV.Len());
	AlphaV = _AlphaV;
	BetaV = _BetaV;
	Dim = _AlphaV.Len();
	Dirty = true;
}

void TMAGNodeBeta::AttrGen(TIntVV& AttrVV, const int& NNodes) {
	IAssert(Dim > 0);
	AttrVV.Gen(NNodes, Dim);
	AttrVV.PutAll(0);

//	printf("AlphaV = %d, BetaV = %d, MuV = %d\n", AlphaV.Len(), BetaV.Len(), MuV.Len());
	
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < Dim; l++) {
			double x = TMAGNodeBeta::Rnd.GetGammaDev(AlphaV[l]);
			double y = TMAGNodeBeta::Rnd.GetGammaDev(BetaV[l]);
			MuV[l] = x / (x + y);
			if((TMAGNodeBeta::Rnd).GetUniDev() > MuV[l]) {
				AttrVV.At(i, l) = 1;
			}
		}
	}
	Dirty = false;
}

void TMAGNodeBeta::LoadTxt(const TStr& InFNm) {
	FILE *fp = fopen(InFNm.CStr(), "r");
	IAssertR(fp != NULL, "File does not exist: " + InFNm);

	Dim = 0;
	AlphaV.Gen(10, 0);
	BetaV.Gen(10, 0);

	char buf[128];
	char *token;
	TStr TokenStr;
	TFlt Val;

	while(fgets(buf, sizeof(buf), fp) != NULL) {
		token = strtok(buf, "&");
		
		token = strtok(token, " \t");
		TokenStr = TStr(token);
		AlphaV.Add(TokenStr.GetFlt(Val));

		token = strtok(NULL, " \t");
		TokenStr = TStr(token);
		BetaV.Add(TokenStr.GetFlt(Val));

		Dim++;
	}

	fclose(fp);
}

void TMAGNodeBeta::SaveTxt(TStrV& OutStrV) const {
	OutStrV.Gen(Dim, 0);

	for(int i = 0; i < Dim; i++) {
		OutStrV.Add(TStr::Fmt("%f %f", double(AlphaV[i]), double(BetaV[i])));
	}
}


///////////////////////////////////////////////////////////////
// MAGFit with Bernoulli node attributes

void TMAGFitBern::SetGraph(const PNGraph& GraphPt) {
	Graph = GraphPt;
	bool NodesOk = true;
	// check that nodes IDs are {0,1,..,Nodes-1}
	for (int nid = 0; nid < Graph->GetNodes(); nid++) {
	if (! Graph->IsNode(nid)) { NodesOk=false; break; } }
	if (! NodesOk) {
	TIntV NIdV;  GraphPt->GetNIdV(NIdV);
	Graph = TSnap::GetSubGraph(GraphPt, NIdV, true);
	for (int nid = 0; nid < Graph->GetNodes(); nid++) {
	  IAssert(Graph->IsNode(nid)); }
	}
}

const TFltVV& TMAGFitBern::SetPhiVV(const TIntVV& AttrVV, const int KnownIds) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();

	PhiVV.Gen(NNodes, NAttrs);
	KnownVV.Gen(NNodes, NAttrs);
	
	for(int l = 0; l < NAttrs; l++) {
		for(int i = 0; i < NNodes; i++) {
			if(int(AttrVV(i, l)) == 0) {
				PhiVV(i, l) = 0.9999;
			} else {
				PhiVV(i, l) = 0.0001;
			}
		}

		if(l < KnownIds) {
			KnownVV.PutY(l, true);
		} else {
			KnownVV.PutY(l, false);
		}
	}
}

void TMAGFitBern::SaveTxt(const TStr& FNm) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	const TFltV MuV = GetMuV();
	TMAGAffMtxV MtxV;
	Param.GetMtxV(MtxV);

	FILE *fp = fopen(FNm.GetCStr(), "w");
	for(int l = 0; l < NAttrs; l++) {
		fprintf(fp, "%.4f\t", double(MuV[l]));
		for(int row = 0; row < 2; row++) {
			for(int col = 0; col < 2; col++) {
				fprintf(fp, " %.4f", double(MtxV[l].At(row, col)));
			}
			fprintf(fp, (row == 0) ? ";" : "\n");
		}
	}
	fclose(fp);

	fp = fopen((FNm + "f").CStr(), "w");
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			fprintf(fp, "%f ", double(PhiVV(i, l)));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}
	
void TMAGFitBern::Init(const TFltV& MuV, const TMAGAffMtxV& AffMtxV) {
	TMAGNodeBern DistParam(MuV);
	Param.SetNodeAttr(DistParam);
	Param.SetMtxV(AffMtxV);

	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();

	PhiVV.Gen(NNodes, NAttrs);
	KnownVV.Gen(NNodes, NAttrs);
	KnownVV.PutAll(false);
}

#if 0
void TMAGFitBern::PerturbInit(const TFltV& MuV, const TMAGAffMtxV& AffMtxV, const double& PerturbRate) {
	IAssert(PerturbRate < 1.0);

	TFltV InitMuV = MuV;	//InitMuV.PutAll(0.5);
	TMAGNodeBern DistParam(InitMuV);
	Param.SetMtxV(AffMtxV);
	TRnd& Rnd = TMAGNodeBern::Rnd;
	TMAGAffMtxV PerturbMtxV = AffMtxV;

	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	
	for(int l = 0; l < NAttrs; l++) {
		double Mu = MuV[l] + PerturbRate * (Rnd.GetUniDev() - 0.5) * 2;
//		double Mu = 0.5;
		if(Mu < 0.01) {  Mu = 0.01;  }
		if(Mu > 0.99) {  Mu = 0.99;  }
		DistParam.SetMu(l, Mu);
//		PhiVV.PutY(l, MuV[l] + Perturb);
		TMAGAffMtx AffMtx(AffMtxV[l]);
		for(int p = 0; p < 4; p++) {
			AffMtx.At(p) += PerturbRate * (Rnd.GetUniDev() - 0.5) * 2;
			if(AffMtx.At(p) < 0.05) {  AffMtx.At(p) = 0.05;  }
			if(AffMtx.At(p) > 0.95) {  AffMtx.At(p) = 0.95;  }
		}
		AffMtx.At(0, 1) = AffMtx.At(1, 0);
		PerturbMtxV[l] = AffMtx;
	}
//	NormalizeAffMtxV(PerturbMtxV);

	printf("\n");
	for(int l = 0; l < NAttrs; l++) {
		printf("Mu = %.3f  ", DistParam.GetMu(l));
		printf("AffMtx = %s\n", PerturbMtxV[l].GetMtxStr().GetCStr());
	}
	Param.SetMtxV(PerturbMtxV);
	Param.SetNodeAttr(DistParam);
	
	PhiVV.Gen(NNodes, NAttrs);
	KnownVV.Gen(NNodes, NAttrs);
	KnownVV.PutAll(false);
}
#endif	//	0

void TMAGFitBern::RandomInit(const TFltV& MuV, const TMAGAffMtxV& AffMtxV, const int& Seed) {
	TRnd& Rnd = TMAGNodeBern::Rnd;
	Rnd.PutSeed(Seed);

	TFltV InitMuV = MuV;	InitMuV.PutAll(0.5);
	TMAGNodeBern DistParam(InitMuV);
	Param.SetMtxV(AffMtxV);
	
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	
	PhiVV.Gen(NNodes, NAttrs);
	KnownVV.Gen(NNodes, NAttrs);
	KnownVV.PutAll(false);

	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			PhiVV.At(i, l) = Rnd.GetUniDev();
//			PhiVV.At(i, l) = 0.5;
		}
	}
	
	TMAGAffMtxV RndMtxV = AffMtxV;
	for(int l = 0; l < NAttrs; l++) {
		for(int p = 0; p < 4; p++) {
			RndMtxV[l].At(p) = TMAGNodeBern::Rnd.GetUniDev();
			if(RndMtxV[l].At(p) < 0.1) {  RndMtxV[l].At(p) = 0.1;  }
			if(RndMtxV[l].At(p) > 0.9) {  RndMtxV[l].At(p) = 0.9;  }
		}
		RndMtxV[l].At(0, 1) = RndMtxV[l].At(1, 0);
	}
	
	printf("\n");
	for(int l = 0; l < NAttrs; l++) {
		printf("AffMtx = %s\n", RndMtxV[l].GetMtxStr().GetCStr());
	}
	Param.SetMtxV(RndMtxV);
	Param.SetNodeAttr(DistParam);
}

const double TMAGFitBern::GetInCoeff(const int& i, const int& j, const int& l, const int& A, const TMAGAffMtx& Theta) const {
	return (PhiVV.At(i, l) * Theta.At(0, A) + (1.0 - PhiVV.At(i, l)) * Theta.At(1, A));
}

const double TMAGFitBern::GetOutCoeff(const int& i, const int& j, const int& l, const int& A, const TMAGAffMtx& Theta) const {
	return (PhiVV.At(j, l) * Theta.At(A, 0) + (1.0 - PhiVV.At(j, l)) * Theta.At(A, 1));
}

const double TMAGFitBern::GetAvgInCoeff(const int& i, const int& AId, const int& A, const TMAGAffMtx& Theta) const {
	const int NNodes = Param.GetNodes();
	const double Mu_l = AvgPhiV[AId] / double(NNodes);
	return (Mu_l * Theta.At(0, A) + (1.0 - Mu_l) * Theta.At(1, A));
}

const double TMAGFitBern::GetAvgOutCoeff(const int& i, const int& AId, const int& A, const TMAGAffMtx& Theta) const {
	const int NNodes = Param.GetNodes();
	const double Mu_l = AvgPhiV[AId] / double(NNodes);
	return (Mu_l * Theta.At(A, 0) + (1.0 - Mu_l) * Theta.At(A, 1));
}

const double TMAGFitBern::GetProbPhi(const int& NId1, const int& NId2, const int& AId, const int& Attr1, const int& Attr2) const {
	double Prob1 = (Attr1 == 0) ? double(PhiVV.At(NId1, AId)) : (1.0 - PhiVV.At(NId1, AId));
	double Prob2 = (Attr2 == 0) ? double(PhiVV.At(NId2, AId)) : (1.0 - PhiVV.At(NId2, AId));
	return (Prob1 * Prob2);
}

const double TMAGFitBern::GetProbMu(const int& NId1, const int& NId2, const int& AId, const int& Attr1, const int& Attr2, const bool Left, const bool Right) const {
	TMAGNodeBern DistParam = Param.GetNodeAttr();
//	double Mu = DistParam.GetMu(AId);
	double Mu = AvgPhiV[AId] / double(Param.GetNodes());
	double Prob1 = (Left) ? double(PhiVV.At(NId1, AId)) : double(Mu);
	double Prob2 = (Right)? double(PhiVV.At(NId2, AId)) : double(Mu);
	Prob1 = (Attr1 == 0) ? Prob1 : 1.0 - Prob1;
	Prob2 = (Attr2 == 0) ? Prob2 : 1.0 - Prob2;
	return (Prob1 * Prob2);
}

const double TMAGFitBern::GetThetaLL(const int& NId1, const int& NId2, const int& AId) const {
	double LL = 0.0;
	const TMAGAffMtx& Mtx = Param.GetMtx(AId);
	for(int A1 = 0; A1 < 2; A1++) {
		for(int A2 = 0; A2 < 2; A2++) {
			LL += GetProbPhi(NId1, NId2, AId, A1, A2) * Mtx.At(A1, A2);
		}
	}
	return log(LL);
}

const double TMAGFitBern::GetAvgThetaLL(const int& NId1, const int& NId2, const int& AId, const bool Left, const bool Right) const {
	double LL = 0.0;
	const TMAGAffMtx& Mtx = Param.GetMtx(AId);
	for(int A1 = 0; A1 < 2; A1++) {
		for(int A2 = 0; A2 < 2; A2++) {
			LL += GetProbMu(NId1, NId2, AId, A1, A2, Left, Right) * Mtx.At(A1, A2);
		}
	}
	return log(LL);
}

const double TMAGFitBern::GetSqThetaLL(const int& NId1, const int& NId2, const int& AId) const {
	double LL = 0.0;
	const TMAGAffMtx& Mtx = Param.GetMtx(AId);
	for(int A1 = 0; A1 < 2; A1++) {
		for(int A2 = 0; A2 < 2; A2++) {
			LL += GetProbPhi(NId1, NId2, AId, A1, A2) * Mtx.At(A1, A2) * Mtx.At(A1, A2);
		}
	}
	return log(LL);
}

const double TMAGFitBern::GetAvgSqThetaLL(const int& NId1, const int& NId2, const int& AId, const bool Left, const bool Right) const {
	double LL = 0.0;
	const TMAGAffMtx& Mtx = Param.GetMtx(AId);
	for(int A1 = 0; A1 < 2; A1++) {
		for(int A2 = 0; A2 < 2; A2++) {
			LL += GetProbMu(NId1, NId2, AId, A1, A2, Left, Right) * Mtx.At(A1, A2) * Mtx.At(A1, A2);
		}
	}
	return log(LL);
}

const double TMAGFitBern::GetProdLinWeight(const int& NId1, const int& NId2) const {
	const int NAttrs = Param.GetAttrs();
	double LL = 0.0;

	for(int l = 0; l < NAttrs; l++) {
		LL += GetThetaLL(NId1, NId2, l);
	}
//	return LL;
	return LL + log(NormConst);
}

const double TMAGFitBern::GetAvgProdLinWeight(const int& NId1, const int& NId2, const bool Left, const bool Right) const {
	const int NAttrs = Param.GetAttrs();
	double LL = 0.0;

	for(int l = 0; l < NAttrs; l++) {
		LL += GetAvgThetaLL(NId1, NId2, l, Left, Right);
	}
//	return LL;
	return LL + log(NormConst);
}

const double TMAGFitBern::GetProdSqWeight(const int& NId1, const int& NId2) const {
	const int NAttrs = Param.GetAttrs();
	double LL = 0.0;

	for(int l = 0; l < NAttrs; l++) {
		LL += GetSqThetaLL(NId1, NId2, l);
	}
//	return LL;
	return LL + 2 * log(NormConst);
}

const double TMAGFitBern::GetAvgProdSqWeight(const int& NId1, const int& NId2, const bool Left, const bool Right) const {
	const int NAttrs = Param.GetAttrs();
	double LL = 0.0;

	for(int l = 0; l < NAttrs; l++) {
		LL += GetAvgSqThetaLL(NId1, NId2, l, Left, Right);
	}
//	return LL;
	return LL + 2 * log(NormConst);
}

const double LogSumExp(const double LogVal1, const double LogVal2) {
	double MaxExp = (LogVal1 > LogVal2) ? LogVal1 : LogVal2;
	double Sum = exp(LogVal1 - MaxExp) + exp(LogVal2 - MaxExp);
	return (log(Sum) + MaxExp);
}

const double LogSumExp(const TFltV& LogValV) {
	const int Len = LogValV.Len();
	double MaxExp = -DBL_MAX;
	
	for(int i = 0; i < Len; i++) {
		if(MaxExp < LogValV[i]) {  MaxExp = LogValV[i];  }
	}
	
	double Sum = 0.0;
	for(int i = 0; i < Len; i++) {
		Sum += exp(LogValV[i] - MaxExp);
	}

	return (log(Sum) + MaxExp);
}

const double LogSumExp(const double *LogValArray, const int Len) {
	TFltV TmpV(Len);
	for(int i = 0; i < Len; i++) {  TmpV[i] = LogValArray[i];  }
	return LogSumExp(TmpV);
}

const double TMAGFitBern::GradPhiMI(const double& x, const int& NId, const int& AId, const double& Lambda, const double& DeltaQ, const TFltVV& CntVV) {
	const int NAttrs = CntVV.GetYDim();
	double Grad = DeltaQ - log(x) + log(1.0-x);

	for(int l = 0; l < NAttrs; l++) {
		if(l == AId) {  continue;  }
		const double C0 = PhiVV(NId, l);
		const double C1 = 1.0 - C0;
		Grad -= Lambda * C0 * log(CntVV(0, l) + C0 * x);
		Grad -= Lambda * C1 * log(CntVV(1, l) + C1 * x);
		Grad += Lambda * C0 * log(CntVV(2, l) + C0 * (1-x));
		Grad += Lambda * C1 * log(CntVV(3, l) + C1 * (1-x));
		Grad -= Lambda * log(CntVV(0, l) + CntVV(1, l) + x);
		Grad += Lambda * log(CntVV(2, l) + CntVV(3, l) + (1-x));
	}

	return Grad;
}

const double TMAGFitBern::ObjPhiMI(const double& x, const int& NId, const int& AId, const double& Lambda, const double& Q0, const double& Q1, const TFltVV& CntVV) {
	const int NAttrs = CntVV.GetYDim();
	double Val = x*(Q0 - log(x)) + (1-x)*(Q1 - log(1.0-x));

	for(int l = 0; l < NAttrs; l++) {
		if(l == AId) {  continue;  }
		const double C0 = PhiVV(NId, l);
		const double C1 = 1.0 - C0;
		Val -= Lambda * (CntVV(0, l) + C0 * x) * log(CntVV(0, l) + C0 * x);
		Val -= Lambda * (CntVV(1, l) + C1 * x) * log(CntVV(1, l) + C1 * x);
		Val -= Lambda * (CntVV(2, l) + C0 * (1-x)) * log(CntVV(2, l) + C0 * (1-x));
		Val -= Lambda * (CntVV(3, l) + C1 * (1-x)) * log(CntVV(3, l) + C1 * (1-x));
		Val += Lambda * (CntVV(0, l) + CntVV(1, l) + x) * log(CntVV(0, l) + CntVV(1, l) + x);
		Val += Lambda * (CntVV(2, l) + CntVV(3, l) + 1 - x) * log(CntVV(2, l) + CntVV(3, l) + (1-x));

		if(!(CntVV(0, l) > 0))	printf("CntVV(0, %d) = %.2f\n", l, double(CntVV(0, l)));
		if(!(CntVV(1, l) > 0))	printf("CntVV(1, %d) = %.2f\n", l, double(CntVV(1, l)));
		if(!(CntVV(2, l) > 0))	printf("CntVV(2, %d) = %.2f\n", l, double(CntVV(2, l)));
		if(!(CntVV(3, l) > 0))	printf("CntVV(3, %d) = %.2f\n", l, double(CntVV(3, l)));
	}

	return Val;
}

const double TMAGFitBern::GetEstNoEdgeLL(const int& NId, const int& AId) const {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	
	TMAGNodeBern DistParam = Param.GetNodeAttr();
	double LL = 0.0;

	return LL;
}

const double TMAGFitBern::UpdatePhi(const int& NId, const int& AId, double& Phi) {
	TMAGAffMtx LLTheta, Theta = Param.GetMtx(AId); 
	TMAGAffMtx SqTheta(Theta);
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	Theta.GetLLMtx(LLTheta);
	TMAGNodeBern DistParam = Param.GetNodeAttr();
	const double Mu = DistParam.GetMu(AId);

	for(int i = 0; i < Theta.Len(); i++) {
		SqTheta.At(i) = SqTheta.At(i) * SqTheta.At(i);
	}

	//	Using log-sum-exp trick
	double EdgeQ[2], NonEdgeQ[2], MaxExp[2];
	TFltV NonEdgeLLV[2];
	for(int i = 0; i < 2; i++) {
		EdgeQ[i] = 0.0;
		NonEdgeQ[i] = 0.0;
		MaxExp[i] = -DBL_MAX;
		NonEdgeLLV[i].Gen(4 * NNodes, 0);
	}
	
	for(int j = 0; j < NNodes; j++) {
		if(j == NId) {	continue;	}

		if(Graph->IsEdge(NId, j)) {
			EdgeQ[0] += GetOutCoeff(NId, j, AId, 0, LLTheta);
			EdgeQ[1] += GetOutCoeff(NId, j, AId, 1, LLTheta);
		} else {
			double LinW = GetProdLinWeight(NId, j) - GetThetaLL(NId, j, AId);
			double SqW = GetProdSqWeight(NId, j) - GetSqThetaLL(NId, j, AId);

			for(int i = 0; i < 2; i++) {
				NonEdgeLLV[i].Add(LinW + log(GetOutCoeff(NId, j, AId, i, Theta)));
				NonEdgeLLV[i].Add(SqW + log(GetOutCoeff(NId, j, AId, i, SqTheta)) + log(0.5));
			}
		}

		if(Graph->IsEdge(j, NId)) {
			EdgeQ[0] += GetInCoeff(j, NId, AId, 0, LLTheta);
			EdgeQ[1] += GetInCoeff(j, NId, AId, 1, LLTheta);
		} else {
			double LinW = GetProdLinWeight(j, NId) - GetThetaLL(j, NId, AId);
			double SqW = GetProdSqWeight(j, NId) - GetSqThetaLL(j, NId, AId);

			for(int i = 0; i < 2; i++) {
				NonEdgeLLV[i].Add(LinW + log(GetInCoeff(j, NId, AId, i, Theta)));
				NonEdgeLLV[i].Add(SqW + log(GetInCoeff(j, NId, AId, i, SqTheta)) + log(0.5));
			}
		}
	}

	NonEdgeQ[0] = LogSumExp(NonEdgeLLV[0]);
	NonEdgeQ[1] = LogSumExp(NonEdgeLLV[1]);
	
	double Q[2];
	Q[0] = log(Mu) + EdgeQ[0] - exp(NonEdgeQ[0]);
	Q[1] = log(1.0 - Mu) + EdgeQ[1] - exp(NonEdgeQ[1]);
//	double Q = Q1 - Q0;
//	printf("  [Phi_{%d}{%d}]  :: Q0 = %f, Q1 = %f\n", NId, AId, Q0, Q1);

//	Phi = 1.0 / (1.0 + exp(Q));
	Phi = Q[0] - LogSumExp(Q, 2);
	Phi = exp(Phi);

	return Phi - PhiVV.At(NId, AId);
}


const double TMAGFitBern::UpdatePhiMI(const double& Lambda, const int& NId, const int& AId, double& Phi) {
	TMAGAffMtx LLTheta, Theta = Param.GetMtx(AId); 
	TMAGAffMtx SqTheta(Theta);
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	Theta.GetLLMtx(LLTheta);
	TMAGNodeBern DistParam = Param.GetNodeAttr();
	const double Mu = DistParam.GetMu(AId);

	for(int i = 0; i < Theta.Len(); i++) {
		SqTheta.At(i) = SqTheta.At(i) * SqTheta.At(i);
	}

	//	Using log-sum-exp trick
	double EdgeQ[2], NonEdgeQ[2], MaxExp[2];
	TFltV NonEdgeLLV[2];
	TFltVV CntVV(4, NAttrs);		CntVV.PutAll(0.0);
	for(int i = 0; i < 2; i++) {
		EdgeQ[i] = 0.0;
		NonEdgeQ[i] = 0.0;
		MaxExp[i] = -DBL_MAX;
		NonEdgeLLV[i].Gen(4 * NNodes, 0);
	}
	
	for(int j = 0; j < NNodes; j++) {
		if(j == NId) {	continue;	}

		for(int l = 0; l < NAttrs; l++) {
			if(l == AId) {  continue;  }
			CntVV(0, l) = CntVV(0, l) + PhiVV(j, AId) * PhiVV(j, l);
			CntVV(1, l) = CntVV(1, l) + PhiVV(j, AId) * (1.0-PhiVV(j, l));
			CntVV(2, l) = CntVV(2, l) + (1.0-PhiVV(j, AId)) * PhiVV(j, l);
			CntVV(3, l) = CntVV(3, l) + (1.0-PhiVV(j, AId)) * (1.0-PhiVV(j, l));
		}

		if(Graph->IsEdge(NId, j)) {
			EdgeQ[0] += GetOutCoeff(NId, j, AId, 0, LLTheta);
			EdgeQ[1] += GetOutCoeff(NId, j, AId, 1, LLTheta);
		} else {
			double LinW = GetProdLinWeight(NId, j) - GetThetaLL(NId, j, AId);
			double SqW = GetProdSqWeight(NId, j) - GetSqThetaLL(NId, j, AId);

			for(int i = 0; i < 2; i++) {
				NonEdgeLLV[i].Add(LinW + log(GetOutCoeff(NId, j, AId, i, Theta)));
				NonEdgeLLV[i].Add(SqW + log(GetOutCoeff(NId, j, AId, i, SqTheta)) + log(0.5));
			}
		}

		if(Graph->IsEdge(j, NId)) {
			EdgeQ[0] += GetInCoeff(j, NId, AId, 0, LLTheta);
			EdgeQ[1] += GetInCoeff(j, NId, AId, 1, LLTheta);
		} else {
			double LinW = GetProdLinWeight(j, NId) - GetThetaLL(j, NId, AId);
			double SqW = GetProdSqWeight(j, NId) - GetSqThetaLL(j, NId, AId);

			for(int i = 0; i < 2; i++) {
				NonEdgeLLV[i].Add(LinW + log(GetInCoeff(j, NId, AId, i, Theta)));
				NonEdgeLLV[i].Add(SqW + log(GetInCoeff(j, NId, AId, i, SqTheta)) + log(0.5));
			}
		}
	}
	
	NonEdgeQ[0] = LogSumExp(NonEdgeLLV[0]);
	NonEdgeQ[1] = LogSumExp(NonEdgeLLV[1]);
	
	double Q[2];
	Q[0] = log(Mu) + EdgeQ[0] - exp(NonEdgeQ[0]);
	Q[1] = log(1.0 - Mu) + EdgeQ[1] - exp(NonEdgeQ[1]);
	double DeltaQ = Q[0] - Q[1];

//	double x[] = {0.1, 0.3, 0.5, 0.7, 0.9};
	double x[] = {PhiVV(NId, AId)};
//	for(int n = 0; n < 5; n++) {
	for(int n = 0; n < 1; n++) {
//		double LrnRate = 0.0002;
		double LrnRate = 0.001;
		for(int step = 0; step < 200; step++) {
			double Grad = GradPhiMI(x[n], NId, AId, Lambda, DeltaQ, CntVV);
			if(Grad > 0.0) {  x[n] += LrnRate;  }
			else {  x[n] -= LrnRate;  }
			if(x[n] > 0.9999) {  x[n] = 0.9999;  }
			if(x[n] < 0.0001) {  x[n] = 0.0001;  }
			LrnRate *= 0.995;
		}
	}

	double MaxVal = -DBL_MAX;
	int MaxX = -1;
//	for(int n = 0; n < 5; n++) {
	for(int n = 0; n < 1; n++) {
		double Val = ObjPhiMI(x[n], NId, AId, Lambda, Q[0], Q[1], CntVV);
		if(Val > MaxVal) {
			MaxVal = Val;
			MaxX = n;
		}
	}
	IAssert(MaxX >= 0);

	Phi = x[MaxX];

	return Phi - PhiVV.At(NId, AId);
}


const double TMAGFitBern::UpdateApxPhiMI(const double& Lambda, const int& NId, const int& AId, double& Phi, TFltVV& ProdVV) {
	TMAGAffMtx LLTheta, Theta = Param.GetMtx(AId); 
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	Theta.GetLLMtx(LLTheta);
	TMAGNodeBern DistParam = Param.GetNodeAttr();
	const double Mu = DistParam.GetMu(AId);

	TMAGAffMtx SqTheta(Theta);
	for(int i = 0; i < Theta.Len(); i++) {
		SqTheta.At(i) = SqTheta.At(i) * SqTheta.At(i);
	}

	TFltV ProdV;	ProdVV.GetRow(NId, ProdV);
	ProdV[0] -= GetAvgThetaLL(NId, NId, AId, true, false);
	ProdV[1] -= GetAvgThetaLL(NId, NId, AId, false, true);
	ProdV[2] -= log(2) + GetAvgSqThetaLL(NId, NId, AId, true, false);
	ProdV[3] -= log(2) + GetAvgSqThetaLL(NId, NId, AId, false, true);

	//	Using log-sum-exp trick
	double EdgeQ[2], MaxExp[2];
	TFltV NonEdgeLLV[2];
	TFltVV CntVV(4, NAttrs);		CntVV.PutAll(0.0);
	for(int i = 0; i < 2; i++) {
		EdgeQ[i] = 0.0;
		MaxExp[i] = -DBL_MAX;
		NonEdgeLLV[i].Gen(4 * NNodes, 0);
	}

	for(int F = 0; F < 2; F++) {
		NonEdgeLLV[F].Add(ProdV[0] + log(GetAvgOutCoeff(NId, AId, F, Theta)));
		NonEdgeLLV[F].Add(ProdV[1] + log(GetAvgInCoeff(NId, AId, F, Theta)));
		NonEdgeLLV[F].Add(ProdV[2] + log(GetAvgOutCoeff(NId, AId, F, SqTheta)));
		NonEdgeLLV[F].Add(ProdV[3] + log(GetAvgInCoeff(NId, AId, F, SqTheta)));
	}
	EdgeQ[0] = -(NNodes - 1) * exp(LogSumExp(NonEdgeLLV[0]));
	EdgeQ[1] = -(NNodes - 1) * exp(LogSumExp(NonEdgeLLV[1]));

	
	for(int l = 0; l < NAttrs; l++) {
		if(l == AId) {  continue;  }
		int BgId = (AId > l) ? AId : l;
		int SmId = (AId + l) - BgId;
		int SmL = (l < AId) ? 1 : 0;
		BgId *= 4;
		CntVV(0, l) = AvgPhiPairVV(SmId, BgId) - PhiVV(NId, AId) * PhiVV(NId, l);
		CntVV(1+SmL, l) = AvgPhiPairVV(SmId, BgId+1+SmL) - PhiVV(NId, AId) * (1.0-PhiVV(NId, l));
		CntVV(2-SmL, l) = AvgPhiPairVV(SmId, BgId+2-SmL) - (1.0-PhiVV(NId, AId)) * PhiVV(NId, l);
		CntVV(3, l) = AvgPhiPairVV(SmId, BgId+3) - (1.0-PhiVV(NId, AId)) * (1.0-PhiVV(NId, l));
	}

	TNGraph::TNodeI NI = Graph->GetNI(NId);
	for(int d = 0; d < NI.GetOutDeg(); d++) {
		int Out = NI.GetOutNId(d);
		if(NId == Out) {  continue;  }
		double LinW = GetProdLinWeight(NId, Out) - GetThetaLL(NId, Out, AId);
		double SqW = GetProdSqWeight(NId, Out) - GetSqThetaLL(NId, Out, AId);

		for(int F = 0; F < 2; F++) {
			EdgeQ[F] += GetOutCoeff(NId, Out, AId, F, LLTheta);
			EdgeQ[F] += exp(LinW + log(GetOutCoeff(NId, Out, AId, F, Theta)));
			EdgeQ[F] += 0.5 * exp(SqW + log(GetOutCoeff(NId, Out, AId, F, SqTheta)));
		}
	}
	for(int d = 0; d < NI.GetInDeg(); d++) {
		int In = NI.GetInNId(d);
		if(NId == In) {  continue;  }
		double LinW = GetProdLinWeight(In, NId) - GetThetaLL(In, NId, AId);
		double SqW = GetProdSqWeight(In, NId) - GetSqThetaLL(In, NId, AId);

		for(int F = 0; F < 2; F++) {
			EdgeQ[F] += GetInCoeff(In, NId, AId, F, LLTheta);
			EdgeQ[F] += exp(LinW + log(GetInCoeff(In, NId, AId, F, Theta)));
			EdgeQ[F] += 0.5 * exp(SqW + log(GetInCoeff(In, NId, AId, F, SqTheta)));
		}
	}

	EdgeQ[0] += log(Mu);
	EdgeQ[1] += log(1.0 - Mu);
	double DeltaQ = EdgeQ[0] - EdgeQ[1];
//	printf("(%d, %d) :: Q[0] = %f, Q[1] = %f\n", NId, AId, EdgeQ[0], EdgeQ[1]);

//	double x[] = {0.1, 0.3, 0.5, 0.7, 0.9};
	double x[] = {PhiVV(NId, AId)};
	TFltV ObjValV;	ObjValV.Gen(60, 0);
//	for(int n = 0; n < 5; n++) {
	for(int n = 0; n < 1; n++) {
//		double LrnRate = 0.0002;
		double LrnRate = 0.001;
		for(int step = 0; step < 50; step++) {
//		for(int step = 0; step < 10; step++) {
			double Grad = GradPhiMI(x[n], NId, AId, Lambda, DeltaQ, CntVV);
			if(Grad > 0.0) {  x[n] += LrnRate;  }
			else {  x[n] -= LrnRate;  }
			if(x[n] > 0.9999) {  x[n] = 0.9999;  }
			if(x[n] < 0.0001) {  x[n] = 0.0001;  }
			if(x[n] == 0.9999 || x[n] == 0.0001) {
				break;
			}
			LrnRate *= 0.995;
		}
		ObjValV.Add(x[n]);
//		ObjValV.Add(PhiVV(NId, AId));
	}

	double MaxVal = -DBL_MAX;
	int MaxX = -1;
//	for(int n = 0; n < 5; n++) {
	for(int n = 0; n < ObjValV.Len(); n++) {
		double Val = ObjPhiMI(ObjValV[n], NId, AId, Lambda, EdgeQ[0], EdgeQ[1], CntVV);
		if(Val > MaxVal) {
			MaxVal = Val;
			MaxX = n;
		} else if(MaxX < 0) {
			printf("(%d, %d) : %f  Q[0] = %f  Q[1] = %f  Val = %f\n", NId, AId, double(x[n]), double(EdgeQ[0]), double(EdgeQ[1]), Val);
		}
	}
	IAssert(MaxX >= 0);

	Phi = ObjValV[MaxX];

	return Phi - PhiVV.At(NId, AId);
}



double TMAGFitBern::DoEStepOneIter(const TFltV& TrueMuV, TFltVV& NewPhiVV, const double& Lambda) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	double MaxDelta = 0, L1 = 0;
	double Val;
	TFltIntIntTrV NewVal;
	int RndCount = 0;
	double OldMI = 0.0, NewMI = 0.0;
	TFltV MuV(NAttrs);	MuV.PutAll(0.0);
	TIntV NIndV(NNodes), AIndV(NAttrs);

	//	Update Phi
	/*
	for(int i = 0; i < NNodes; i++) {  NIndV[i] = i;  }
	for(int l = 0; l < NAttrs; l++) {  AIndV[l] = l;  }
	if(Randomized) {
		NIndV.Shuffle(TMAGNodeBern::Rnd);
		AIndV.Shuffle(TMAGNodeBern::Rnd);
	}
	*/

	NewVal.Gen(NAttrs * 2);
	for(int i = 0; i < NNodes; i++) {
//		const int NId = NIndV[i]%NNodes;
		for(int l = 0; l < NAttrs * 2; l++) {
			const int NId = TMAGNodeBern::Rnd.GetUniDevInt(NNodes);
			const int AId = TMAGNodeBern::Rnd.GetUniDevInt(NAttrs);
//			const int AId = AIndV[l]%NAttrs;
//			double Delta = UpdatePhi(NId, AId, Val);
			double Delta = 0.0;
			if(KnownVV(NId, AId)) {
				Val = PhiVV.At(NId, AId);
			} else {
				Delta = UpdatePhiMI(Lambda, NId, AId, Val);
			}

//			PhiVV.At(NId, AId) = Val;
			NewVal[l] = TFltIntIntTr(Val, NId, AId);
			
//			MuV[AId] = MuV[AId] + Val;
			if(fabs(Delta) > MaxDelta) {
				MaxDelta = fabs(Delta);
			}
			if(Val > 0.3 && Val < 0.7) {	RndCount++;	}
		}

		for(int l = 0; l < NAttrs * 2; l++) {
			const int NId = NewVal[l].Val2;
			const int AId = NewVal[l].Val3;
			PhiVV.At(NId, AId) = NewVal[l].Val1;
		}
	}
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			MuV[l] = MuV[l] + PhiVV.At(i, l);
		}
	}
	for(int l = 0; l < NAttrs; l++) {
		MuV[l] = MuV[l] / double(NNodes);
	}

	TFltV SortMuV = MuV;
	double Avg = 0.0;
	SortMuV.Sort(false);
	for(int l = 0; l < NAttrs; l++) {
		printf("  F[%d] = %.3f", l, double(MuV[l]));
		Avg += SortMuV[l];
		L1 += fabs(TrueMuV[l] - SortMuV[l]);
	}
	printf("\n");
	printf("  Rnd = %d(%.3f)", RndCount, double(RndCount) / double(NNodes * NAttrs));
	printf("  Avg = %.3f\n", Avg / double(NAttrs));
	L1 /= double(NAttrs);

	return L1;
}


double TMAGFitBern::DoEStepApxOneIter(const TFltV& TrueMuV, TFltVV& NewPhiVV, const double& Lambda) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	double MaxDelta = 0, L1 = 0;
	double Val;
	TFltIntIntTrV NewVal;
	int RndCount = 0;
	double OldMI = 0.0, NewMI = 0.0;
	TFltV MuV(NAttrs);	MuV.PutAll(0.0);
	TFltVV ProdVV(NNodes, 4);	ProdVV.PutAll(0.0);
	TIntV NIndV(NNodes), AIndV(NAttrs);

	//	Update Phi
	/*
	for(int i = 0; i < NNodes; i++) {  NIndV[i] = i;  }
	for(int l = 0; l < NAttrs; l++) {  AIndV[l] = l;  }
	if(Randomized) {
		NIndV.Shuffle(TMAGNodeBern::Rnd);
		AIndV.Shuffle(TMAGNodeBern::Rnd);
	}
	*/

	AvgPhiV.Gen(NAttrs);	AvgPhiV.PutAll(0.0);
	AvgPhiPairVV.Gen(NAttrs, 4*NAttrs);		AvgPhiPairVV.PutAll(0.0);
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			for(int p = l+1; p < NAttrs; p++) {
				int index = 4 * p;
				AvgPhiPairVV(l, index) += PhiVV(i, l) * PhiVV(i, p);
				AvgPhiPairVV(l, index+1) += PhiVV(i, l) * (1.0-PhiVV(i, p));
				AvgPhiPairVV(l, index+2) += (1.0-PhiVV(i, l)) * PhiVV(i, p);
				AvgPhiPairVV(l, index+3) += (1.0-PhiVV(i, l)) * (1.0-PhiVV(i, p));
			}
			AvgPhiV[l] += PhiVV(i, l);
		}
	}
	for(int i = 0; i < NNodes; i++) {
		ProdVV(i, 0) = GetAvgProdLinWeight(i, i, true, false);
		ProdVV(i, 1) = GetAvgProdLinWeight(i, i, false, true);
		ProdVV(i, 2) = GetAvgProdSqWeight(i, i, true, false);
		ProdVV(i, 3) = GetAvgProdSqWeight(i, i, false, true);
	}

	const int Iter = 3;
	int NId;

	NewVal.Gen(NAttrs * Iter);
	for(int i = 0; i < NNodes * Iter; i++) {
		for(int l = 0; l < NAttrs; l++) {
			const int AId = TMAGNodeBern::Rnd.GetUniDevInt(NAttrs);
			double Delta = 0.0;
			if(KnownVV(NId, AId)) {
				Val = PhiVV.At(NId, AId);
			} else {
				Delta = UpdateApxPhiMI(Lambda, NId, AId, Val, ProdVV);
			}

//			PhiVV.At(NId, AId) = Val;
			NewVal[l] = TFltIntIntTr(Val, NId, AId);
			
//			MuV[AId] = MuV[AId] + Val;
			if(fabs(Delta) > MaxDelta) {
				MaxDelta = fabs(Delta);
			}
			if(Val > 0.3 && Val < 0.7) {	RndCount++;	}
		}

		for(int l = 0; l < NAttrs; l++) {
			const int NId = NewVal[l].Val2;
			const int AId = NewVal[l].Val3;

			ProdVV(NId, 0) -= GetAvgThetaLL(NId, NId, AId, true, false);
			ProdVV(NId, 1) -= GetAvgThetaLL(NId, NId, AId, false, true);
			ProdVV(NId, 2) -= GetAvgSqThetaLL(NId, NId, AId, true, false);
			ProdVV(NId, 3) -= GetAvgSqThetaLL(NId, NId, AId, false, true);
			for(int p = 0; p < NAttrs; p++) {
				if(p > AId) {
					int index = 4 * p;
					AvgPhiPairVV(AId, index) -= PhiVV(NId, AId) * PhiVV(NId, p);
					AvgPhiPairVV(AId, index+1) -= PhiVV(NId, AId) * (1.0-PhiVV(NId, p));
					AvgPhiPairVV(AId, index+2) -= (1.0-PhiVV(NId, AId)) * PhiVV(NId, p);
					AvgPhiPairVV(AId, index+3) -= (1.0-PhiVV(NId, AId)) * (1.0-PhiVV(NId, p));
				} else if (p < AId) {
					int index = 4 * AId;
					AvgPhiPairVV(p, index) -= PhiVV(NId, p) * PhiVV(NId, AId);
					AvgPhiPairVV(p, index+1) -= PhiVV(NId, p) * (1.0-PhiVV(NId, AId));
					AvgPhiPairVV(p, index+2) -= (1.0-PhiVV(NId, p)) * PhiVV(NId, AId);
					AvgPhiPairVV(p, index+3) -= (1.0-PhiVV(NId, p)) * (1.0-PhiVV(NId, AId));
				}
			}
			AvgPhiV[AId] -= PhiVV(NId, AId);

			PhiVV.At(NId, AId) = NewVal[l].Val1;
			
			ProdVV(NId, 0) += GetAvgThetaLL(NId, NId, AId, true, false);
			ProdVV(NId, 1) += GetAvgThetaLL(NId, NId, AId, false, true);
			ProdVV(NId, 2) += GetAvgSqThetaLL(NId, NId, AId, true, false);
			ProdVV(NId, 3) += GetAvgSqThetaLL(NId, NId, AId, false, true);
			for(int p = 0; p < NAttrs; p++) {
				if(p > AId) {
					int index = 4 * p;
					AvgPhiPairVV(AId, index) += PhiVV(NId, AId) * PhiVV(NId, p);
					AvgPhiPairVV(AId, index+1) += PhiVV(NId, AId) * (1.0-PhiVV(NId, p));
					AvgPhiPairVV(AId, index+2) += (1.0-PhiVV(NId, AId)) * PhiVV(NId, p);
					AvgPhiPairVV(AId, index+3) += (1.0-PhiVV(NId, AId)) * (1.0-PhiVV(NId, p));
				} else if (p < AId) {
					int index = 4 * AId;
					AvgPhiPairVV(p, index) += PhiVV(NId, p) * PhiVV(NId, AId);
					AvgPhiPairVV(p, index+1) += PhiVV(NId, p) * (1.0-PhiVV(NId, AId));
					AvgPhiPairVV(p, index+2) += (1.0-PhiVV(NId, p)) * PhiVV(NId, AId);
					AvgPhiPairVV(p, index+3) += (1.0-PhiVV(NId, p)) * (1.0-PhiVV(NId, AId));
				}
			}
			AvgPhiV[AId] += PhiVV(NId, AId);
		}
	}

	for(int l = 0; l < NAttrs; l++) {
		MuV[l] = AvgPhiV[l] / double(NNodes);
	}

	TFltV SortMuV = MuV;
	double Avg = 0.0;
//	SortMuV.Sort(false);
	for(int l = 0; l < NAttrs; l++) {
		printf("  F[%d] = %.3f", l, double(MuV[l]));
		Avg += SortMuV[l];
//		L1 += fabs(TrueMuV[l] - SortMuV[l]);
	}
	printf("\n");
	printf("  Rnd = %d(%.3f)", RndCount, double(RndCount) / double(NNodes * NAttrs));
	printf("  Avg = %.3f\n", Avg / double(NAttrs));
//	printf("  Linf = %f\n", MaxDelta);
//	L1 /= double(NAttrs);

	return L1;
}

double TMAGFitBern::DoEStep(const TFltV& TrueMuV, const int& NIter, double& LL, const double& Lambda) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TFltVV NewPhiVV(NNodes, NAttrs);
	double MI;

	TFltV Delta(NIter);

	for(int i = 0; i < NIter; i++) {
		TExeTm IterTm;
		printf("EStep iteration : %d\n", (i+1));
		if(ESpeedUp) {
			Delta[i] = DoEStepApxOneIter(TrueMuV, NewPhiVV, Lambda);
		} else {
			Delta[i] = DoEStepOneIter(TrueMuV, NewPhiVV, Lambda);
		}
//		PhiVV = NewPhiVV;

		printf("  (Time = %s)\n", IterTm.GetTmStr());
	}
	printf("\n");

	NewPhiVV.Clr();

	return Delta.Last();
}

const double TMAGFitBern::UpdateMu(const int& AId) {
	const int NNodes = Param.GetNodes();
	TMAGNodeBern DistParam = Param.GetNodeAttr();
	const double OldMu = DistParam.GetMu(AId);
	double NewMu = 0.0;

	for(int i = 0; i < NNodes; i++) {
		NewMu += PhiVV.At(i, AId);
	}
	AvgPhiV[AId] = NewMu;
	NewMu /= double(NNodes);

	printf("      [Posterior Mu] = %.4f\n", NewMu);

	double Delta = fabs(NewMu - OldMu);
	DistParam.SetMu(AId, NewMu);
	Param.SetNodeAttr(DistParam);

	return Delta;
}

const void TMAGFitBern::GradAffMtx(const int& AId, const TFltVV& ProdVV, const TFltVV& SqVV, const TMAGAffMtx& CurMtx, TFltV& GradV) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	GradV.PutAll(0.0);
	
	for(int i = 0; i < NNodes; i++) {
		for(int j = 0; j < NNodes; j++) {
			double Prod = ProdVV(i, j) - GetThetaLL(i, j, AId);
			double Sq = SqVV(i, j) - GetSqThetaLL(i, j, AId);

			for(int p = 0; p < 4; p++) {
				int Ai = p / 2;
				int Aj = p % 2;
				double Prob = GetProbPhi(i, j, AId, Ai, Aj);
				if(Graph->IsEdge(i, j)) {
					GradV[p] += Prob / CurMtx.At(p);
				} else {
					GradV[p] -= Prob * exp(Prod);
					GradV[p] -= Prob * exp(Sq) * CurMtx.At(p);
				}
			}
		}
	}
}


const void TMAGFitBern::GradApxAffMtx(const int& AId, const TFltVV& ProdVV, const TFltVV& SqVV, const TMAGAffMtx& CurMtx, TFltV& GradV) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	const int NSq = NNodes * (NNodes - 1);
	GradV.PutAll(0.0);

	TFltV LogSumV;
	for(int p = 0; p < 4; p++) {
		int Ai = p / 2;
		int Aj = p % 2;
		LogSumV.Gen(NNodes * 4, 0);

		for(int i = 0; i < NNodes; i++) {
			const double LProd = ProdVV(i, 0) - GetAvgThetaLL(i, i, AId, true, false);
			const double LSq = SqVV(i, 0) - GetAvgSqThetaLL(i, i, AId, true, false);
			const double RProd = ProdVV(i, 1) - GetAvgThetaLL(i, i, AId, false, true);
			const double RSq = SqVV(i, 1) - GetAvgSqThetaLL(i, i, AId, false, true);

			LogSumV.Add(LProd + log(GetProbMu(i, i, AId, Ai, Aj, true, false)));
			LogSumV.Add(LSq + log(GetProbMu(i, i, AId, Ai, Aj, true, false)) + log(CurMtx.At(p)));
			LogSumV.Add(RProd + log(GetProbMu(i, i, AId, Ai, Aj, false, true)));
			LogSumV.Add(RSq + log(GetProbMu(i, i, AId, Ai, Aj, false, true)) + log(CurMtx.At(p)));
		}
		double LogSum = LogSumExp(LogSumV);
		GradV[p] -= (NNodes - 1) * 0.5 * exp(LogSum);
	}
	
	for(TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
		const int NId1 = EI.GetSrcNId();
		const int NId2 = EI.GetDstNId();
		const double ProdOne = GetProdLinWeight(NId1, NId2) - GetThetaLL(NId1, NId2, AId);
		const double SqOne = GetProdSqWeight(NId1, NId2) - GetSqThetaLL(NId1, NId2, AId);

		for(int p = 0; p < 4; p++) {
			int Ai = p / 2;
			int Aj = p % 2;
			double Prob = GetProbPhi(NId1, NId2, AId, Ai, Aj);
			GradV[p] += Prob / CurMtx.At(p);
			GradV[p] += Prob * exp(ProdOne);
			GradV[p] += Prob * exp(SqOne) * CurMtx.At(p);
		}
	}

#if 0
	const double Prod = ProdVV(0, 0) - GetAvgThetaLL(0, 0, AId, false, false);
	const double Sq = SqVV(0, 0) - GetAvgSqThetaLL(0, 0, AId, false, false);
	for(int p = 0; p < 4; p++) {
		int Ai = p / 2;
		int Aj = p % 2;
		GradV[p] -= NSq * exp(Prod) * GetProbMu(0, 0, AId, Ai, Aj, false, false);
		GradV[p] -= NSq * exp(Sq) * GetProbMu(0, 0, AId, Ai, Aj, false, false) * CurMtx.At(p);
	}

	for(TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
		const int NId1 = EI.GetSrcNId();
		const int NId2 = EI.GetDstNId();
		const double ProdOne = GetProdLinWeight(NId1, NId2) - GetThetaLL(NId1, NId2, AId);
		const double SqOne = GetProdSqWeight(NId1, NId2) - GetSqThetaLL(NId1, NId2, AId);

		for(int p = 0; p < 4; p++) {
			int Ai = p / 2;
			int Aj = p % 2;
			double Prob = GetProbPhi(NId1, NId2, AId, Ai, Aj);
//			GradV[p] += Prob / CurMtx.At(p);
//			GradV[p] += Prob * exp(ProdOne);
//			GradV[p] += Prob * exp(SqOne) * CurMtx.At(p);
		}
	}
#endif
}


const double TMAGFitBern::UpdateAffMtx(const int& AId, const double& LrnRate, const double& MaxGrad, const double& Lambda, TFltVV& ProdVV, TFltVV& SqVV, TMAGAffMtx& NewMtx) {
	double Delta = 0.0;
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TMAGAffMtx AffMtx = Param.GetMtx(AId);

	TFltV GradV(4);
	TFltV HessV(4);
	if(MSpeedUp) {
		GradApxAffMtx(AId, ProdVV, SqVV, NewMtx, GradV);
	} else {
		GradAffMtx(AId, ProdVV, SqVV, NewMtx, GradV);
	}

	double Ratio = 1.0;
	for(int p = 0; p < 4; p++) {
		if(fabs(Ratio * LrnRate * GradV[p]) > MaxGrad) {
			Ratio = MaxGrad / fabs(LrnRate * GradV[p]);
		}
	}

	for(int p = 0; p < 4; p++) {
		GradV[p] *= (Ratio * LrnRate);
		NewMtx.At(p) = AffMtx.At(p) + GradV[p];
//		if(NewMtx.At(p) > 0.9999) {  NewMtx.At(p) = 0.9999;  }
		if(NewMtx.At(p) < 0.0001) {  NewMtx.At(p) = 0.0001;  }
	}

	printf("      [Attr = %d]\n", AId);
    printf("        %s  + [%f, %f; %f %f]  ----->  %s\n", (AffMtx.GetMtxStr()).GetCStr(), double(GradV[0]), double(GradV[1]), double(GradV[2]), double(GradV[3]), (NewMtx.GetMtxStr()).GetCStr());
	
//	Param.SetMtx(AId, NewMtx);
	return Delta;
}


void TMAGFitBern::NormalizeAffMtxV(TMAGAffMtxV& MtxV, const bool UseMu) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = MtxV.Len();
	TFltV MuV = GetMuV();
	double Product = 1.0, ExpEdge = NNodes * (NNodes - 1);
	
	TFltV SumV(NAttrs), EdgeSumV(NAttrs);
	SumV.PutAll(0.0);	EdgeSumV.PutAll(0.0);
	for(int l = 0; l < NAttrs; l++) {
		double Mu = (UseMu) ? double(MuV[l]) : (AvgPhiV[l] / double(NNodes));
		EdgeSumV[l] += Mu * Mu * MtxV[l].At(0, 0);
		EdgeSumV[l] += Mu * (1.0-Mu) * MtxV[l].At(0, 1);
		EdgeSumV[l] += Mu * (1.0-Mu) * MtxV[l].At(1, 0);
		EdgeSumV[l] += (1.0-Mu) * (1.0-Mu) * MtxV[l].At(1, 1);
		SumV[l] = SumV[l] + MtxV[l].At(0, 0);
		SumV[l] = SumV[l] + MtxV[l].At(0, 1);
		SumV[l] = SumV[l] + MtxV[l].At(1, 0);
		SumV[l] = SumV[l] + MtxV[l].At(1, 1);
		Product *= SumV[l];
		ExpEdge *= EdgeSumV[l];
	}
	ExpEdge = Graph->GetEdges() / ExpEdge;
	NormConst *= Product;
//	NormConst = ExpEdge;
	Product = 1.0;
//	Product = pow(Product * ExpEdge, 1.0 / double(NAttrs));
	
	for(int l = 0; l < NAttrs; l++) {
		for(int p = 0; p < 4; p++) {
			MtxV[l].At(p) = MtxV[l].At(p) * Product / SumV[l];
//			MtxV[l].At(p) = MtxV[l].At(p) * Product / MtxV[l].At(0, 0);
//			MtxV[l].At(p) = MtxV[l].At(p) * Product;
//			if(MtxV[l].At(p) > 0.9999) {  MtxV[l].At(p) = 0.9999;  }
//			if(MtxV[l].At(p) < 0.0001) {  MtxV[l].At(p) = 0.0001;  }
		}
	}
}

void TMAGFitBern::UnNormalizeAffMtxV(TMAGAffMtxV& MtxV, const bool UseMu) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = MtxV.Len();
	TFltIntPrV MaxEntV(NAttrs);
	TFltV MuV = GetMuV();
	NormalizeAffMtxV(MtxV, UseMu);
	
	double ExpEdge = NNodes * (NNodes - 1);
	for(int l = 0; l < NAttrs; l++) {
		double Mu = MuV[l];
		double EdgeSum = Mu * Mu * MtxV[l].At(0, 0);
		EdgeSum += Mu * (1.0-Mu) * MtxV[l].At(0, 1);
		EdgeSum += Mu * (1.0-Mu) * MtxV[l].At(1, 0);
		EdgeSum += (1.0-Mu) * (1.0-Mu) * MtxV[l].At(1, 1);
		ExpEdge *= EdgeSum;
	}
	NormConst = double(Graph->GetEdges()) / ExpEdge;
//	NormConst *= ExpEdge;
	
	for(int l = 0; l < NAttrs; l++) {
		MaxEntV[l] = TFltIntPr(-1, l);
		for(int p = 0; p < 4; p++) {
			if(MaxEntV[l].Val1 < MtxV[l].At(p)) {  MaxEntV[l].Val1 = MtxV[l].At(p);  }
		}
	}
	MaxEntV.Sort(false);

	for(int l = 0; l < NAttrs; l++) {
		int CurId = MaxEntV[l].Val2;
		double Factor = pow(NormConst, 1.0 / double(NAttrs - l));
		double MaxFactor = 0.9999 / MaxEntV[l].Val1;
		Factor = (Factor > MaxFactor) ? MaxFactor : Factor;
		NormConst = NormConst / Factor;

		for(int p = 0; p < 4; p++) {
			MtxV[CurId].At(p) = MtxV[CurId].At(p) * Factor;
		}
	}
}

const void TMAGFitBern::PrepareUpdateAffMtx(TFltVV& ProdVV, TFltVV& SqVV) {
	const int NNodes = Param.GetNodes();
	ProdVV.Gen(NNodes, NNodes);
	SqVV.Gen(NNodes, NNodes);

	for(int i = 0; i < NNodes; i++) {
		for(int j = 0; j < NNodes; j++) {
			ProdVV(i, j) = GetProdLinWeight(i, j);
			SqVV(i, j) = GetProdSqWeight(i, j);
		}
	}
}

const void TMAGFitBern::PrepareUpdateApxAffMtx(TFltVV& ProdVV, TFltVV& SqVV) {
	const int NNodes = Param.GetNodes();
	ProdVV.Gen(NNodes, 2);
	SqVV.Gen(NNodes, 2);

	for(int i = 0; i < NNodes; i++) {
		ProdVV(i, 0) = GetAvgProdLinWeight(i, i, true, false);
		ProdVV(i, 1) = GetAvgProdLinWeight(i, i, false, true);
		SqVV(i, 0) = GetAvgProdSqWeight(i, i, true, false);
		SqVV(i, 1) = GetAvgProdSqWeight(i, i, false, true);
	}
}
	
const double TMAGFitBern::UpdateAffMtxV(const int& GradIter, const double& LrnRate, const double& MaxGrad, const double& Lambda, const int& NReal) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	const TMAGNodeBern DistParam = Param.GetNodeAttr();
	const TFltV MuV = DistParam.GetMuV();
	double Delta = 0.0;
	double DecLrnRate = LrnRate, DecMaxGrad = MaxGrad;
	
	TFltVV ProdVV(NNodes, NNodes), SqVV(NNodes, NNodes);
	TMAGAffMtxV NewMtxV, OldMtxV;
	Param.GetMtxV(OldMtxV);
	Param.GetMtxV(NewMtxV);

	for(int g = 0; g < GradIter; g++) {
		if(MSpeedUp) {
			PrepareUpdateApxAffMtx(ProdVV, SqVV);
		} else {
			PrepareUpdateAffMtx(ProdVV, SqVV);
		}

		printf("    [Grad step = %d]\n", (g+1));
//		for(int l = 0; l < NAttrs; l++) {
		for(int l = NReal; l < NAttrs; l++) {
			UpdateAffMtx(l, DecLrnRate, DecMaxGrad, Lambda, ProdVV, SqVV, NewMtxV[l]);
			Param.SetMtxV(NewMtxV);
		}
		DecLrnRate *= 0.97;
		DecMaxGrad *= 0.97;

		printf("\n");
		NormalizeAffMtxV(NewMtxV, true);
		Param.SetMtxV(NewMtxV);
	}
	NormalizeAffMtxV(NewMtxV, true);
	
	printf( "\nFinal\n");
	for(int l = 0; l < NAttrs; l++) {
		printf("    [");
		for(int p = 0; p < 4; p++) {
//			NewMtxV[l].At(p) = NewMtxV[l].At(p) * Product / SumV[l];
			Delta += fabs(OldMtxV[l].At(p) - NewMtxV[l].At(p));
			printf(" %.4f ", double(NewMtxV[l].At(p)));
		}
		printf("]\n");
	}
	Param.SetMtxV(NewMtxV);
	ProdVV.Clr();		SqVV.Clr();
	return Delta;
}

void TMAGFitBern::DoMStep(const int& GradIter, const double& LrnRate, const double& MaxGrad, const double& Lambda, const int& NReal) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	double MuDelta = 0.0, AffMtxDelta = 0.0;

	TExeTm ExeTm;
	
	printf("\n");
	AvgPhiV.Gen(NAttrs);	AvgPhiV.PutAll(0.0);
	for(int l = 0; l < NAttrs; l++) {
//		printf("    [Attr = %d]\n", l);
		MuDelta += UpdateMu(l);
	}
	printf("\n");

	printf("  == Update Theta\n");
	AffMtxDelta += UpdateAffMtxV(GradIter, LrnRate, MaxGrad, Lambda, NReal);
	printf("\n");
	printf("Elpased time = %s\n", ExeTm.GetTmStr());
	printf("\n");
}

void TMAGFitBern::DoEMAlg(const int& NStep, const int& NEstep, const int& NMstep, const double& LrnRate, const double& MaxGrad, const double& Lambda, const double& ReInit, const int& NReal) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TIntV IndexV;
	double LL, MuDist, MtxDist;

	MuHisV.Gen(NStep + 1, 0);
	MtxHisV.Gen(NStep + 1, 0);
	LLHisV.Gen(NStep + 1, 0);

	printf("--------------------------------------------\n");
	printf("Before EM Iteration\n");
	printf("--------------------------------------------\n");

	TMAGAffMtxV InitMtxV;
	TMAGNodeBern NodeAttr = Param.GetNodeAttr();
	Param.GetMtxV(InitMtxV);
	TFltV InitMuV = NodeAttr.GetMuV();
	
	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			if(! KnownVV(i, l)) {
				PhiVV.At(i, l) = TMAGNodeBern::Rnd.GetUniDev();
			}
		}
	}
	
	if(Debug) {
		double LL = ComputeApxLL();
		MuHisV.Add(InitMuV);
		MtxHisV.Add(InitMtxV);
		LLHisV.Add(LL);
	}

	NormalizeAffMtxV(InitMtxV, true);
	Param.SetMtxV(InitMtxV);

	for(int n = 0; n < NStep; n++) {
		printf("--------------------------------------------\n");
		printf("EM Iteration : %d\n", (n+1));
		printf("--------------------------------------------\n");
		
		NodeAttr = Param.GetNodeAttr();
		for(int i = 0; i < NNodes; i++) {
			for(int l = 0; l < NAttrs; l++) {
				if(!KnownVV(i, l) && TMAGNodeBern::Rnd.GetUniDev() < ReInit) {
					PhiVV.At(i, l) = TMAGNodeBern::Rnd.GetUniDev();
				}
			}
		}
		DoEStep(InitMuV, NEstep, LL, Lambda);
		Param.GetMtxV(InitMtxV);
//		NormalizeAffMtxV(InitMtxV);
		Param.SetMtxV(InitMtxV);
		DoMStep(NMstep, LrnRate, MaxGrad, Lambda, NReal);

		printf("\n");

		if(Debug) {
			double LL = ComputeApxLL();
			MuHisV.Add(InitMuV);
			MtxHisV.Add(InitMtxV);
			LLHisV.Add(LL);
			printf("    ApxLL = %.2f (Const = %f)\n", LL, double(NormConst));
		}

	}
	Param.GetMtxV(InitMtxV);
	UnNormalizeAffMtxV(InitMtxV, true);
	Param.SetMtxV(InitMtxV);
}

void TMAGFitBern::MakeCCDF(const TFltPrV& RawV, TFltPrV& CcdfV) {
	double Total = 0.0;
	CcdfV.Gen(RawV.Len(), 0);

	for(int i = 0; i < RawV.Len(); i++) {
		if(RawV[i].Val2 <= 0) {  continue;  }
		Total += RawV[i].Val2;
		CcdfV.Add(RawV[i]);
		IAssert(RawV[i].Val2 > 0);
	}
	for(int i = 1; i < CcdfV.Len(); i++) {
		CcdfV[i].Val2 += CcdfV[i-1].Val2;
	}

	for(int i = CcdfV.Len() - 1; i > 0; i--) {
		CcdfV[i].Val2 = (Total - CcdfV[i-1].Val2) ;
		if(CcdfV[i].Val2 <= 0) {  printf("CCDF = %f\n", double(CcdfV[i].Val2));}
		IAssert(CcdfV[i].Val2 > 0);
	}
	CcdfV[0].Val2 = Total;
//	CcdfV[0].Val2 = 1.0;
}


void TMAGFitBern::PlotProperties(const TStr& FNm) {
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TMAGParam<TMAGNodeBern> MAGGen(NNodes, NAttrs);
	TMAGNodeBern MAGNode = Param.GetNodeAttr();
	MAGGen.SetNodeAttr(MAGNode);
	TMAGAffMtxV MtxV;	Param.GetMtxV(MtxV);
	MAGGen.SetMtxV(MtxV);
	
	PNGraph TrG = new TNGraph;
	*TrG = *Graph;

	TIntVV AttrVV(NNodes, NAttrs);
	for(int i = 0; i < NNodes; i++) {
		for(int j = 0; j < NAttrs; j++) {
			if(PhiVV(i, j) > TMAGNodeBern::Rnd.GetUniDev())	AttrVV(i, j) = 0;
			else AttrVV(i, j) = 1;
		}
	}
	PNGraph MAG = MAGGen.GenMAG(AttrVV, true, 10000);
//	PNGraph MAG = MAGGen.GenAttrMAG(AttrVV, true, 10000);
	printf("%d edges created for MAG...\n", MAG->GetEdges());
	
	TSnap::DelZeroDegNodes(TrG);
	TSnap::DelZeroDegNodes(MAG);

	TGStatVec GS(tmuNodes, TFSet() | gsdInDeg | gsdOutDeg | gsdWcc | gsdHops | gsdClustCf | gsdSngVec | gsdSngVal | gsdTriadPart);
	
    TGnuPlot InDegP(FNm + "-InDeg"), OutDegP(FNm + "-OutDeg"), SvalP(FNm + "-Sval"), SvecP(FNm + "-Svec"), WccP(FNm + "-Wcc"), HopP(FNm + "-Hop"), TriadP(FNm + "-Triad"), CcfP(FNm + "-Ccf");;

    InDegP.SetXYLabel("Degree", "# of nodes");
    OutDegP.SetXYLabel("Degree", "# of nodes");
    SvalP.SetXYLabel("Rank", "Singular value");
    SvecP.SetXYLabel("Rank", "Primary SngVec component");
    WccP.SetXYLabel("Size of component", "# of components");
    CcfP.SetXYLabel("Degree", "Clustering coefficient");
    HopP.SetXYLabel("Hops", "# of node pairs");
    TriadP.SetXYLabel("# of triads", "# of participating nodes");

    InDegP.SetScale(gpsLog10XY);    InDegP.AddCmd("set key top right");
    OutDegP.SetScale(gpsLog10XY);   OutDegP.AddCmd("set key top right");
    SvalP.SetScale(gpsLog10XY);     SvalP.AddCmd("set key top right");
    SvecP.SetScale(gpsLog10XY);     SvecP.AddCmd("set key top right");
    CcfP.SetScale(gpsLog10XY);      CcfP.AddCmd("set key top right");
    HopP.SetScale(gpsLog10XY);      HopP.AddCmd("set key top right");
    TriadP.SetScale(gpsLog10XY);    TriadP.AddCmd("set key top right");
	InDegP.ShowGrid(false);
	OutDegP.ShowGrid(false);
	SvalP.ShowGrid(false);
	SvecP.ShowGrid(false);
	CcfP.ShowGrid(false);
	HopP.ShowGrid(false);
	TriadP.ShowGrid(false);
	
	const TStr Style[2] = {"lt 1 lw 3 lc rgb 'black'", "lt 2 lw 3 lc rgb 'red'"};
	const TStr Name[2] = {"Real", "MAG"};
	GS.Add(Graph, TSecTm(1), "Real Graph");
	GS.Add(MAG, TSecTm(2), "MAG");

	TFltPrV InDegV, OutDegV, SvalV, SvecV, HopV, WccV, CcfV, TriadV;
	for(int i = 0; i < GS.Len(); i++) {
		MakeCCDF(GS.At(i)->GetDistr(gsdInDeg), InDegV);
		MakeCCDF(GS.At(i)->GetDistr(gsdOutDeg), OutDegV);
		SvalV = GS.At(i)->GetDistr(gsdSngVal);
		SvecV = GS.At(i)->GetDistr(gsdSngVec);
		MakeCCDF(GS.At(i)->GetDistr(gsdClustCf), CcfV);
		HopV = GS.At(i)->GetDistr(gsdHops);
		MakeCCDF(GS.At(i)->GetDistr(gsdTriadPart), TriadV);

		InDegP.AddPlot(InDegV, gpwLines, Name[i], Style[i]);
		OutDegP.AddPlot(OutDegV, gpwLines, Name[i], Style[i]);
		SvalP.AddPlot(SvalV, gpwLines, Name[i], Style[i]);
		SvecP.AddPlot(SvecV, gpwLines, Name[i], Style[i]);
		CcfP.AddPlot(CcfV, gpwLines, Name[i], Style[i]);
		HopP.AddPlot(HopV, gpwLines, Name[i], Style[i]);
		TriadP.AddPlot(TriadV, gpwLines, Name[i], Style[i]);
	}

	InDegP.SaveEps(30);
	OutDegP.SaveEps(30);
	SvalP.SaveEps(30);
	SvecP.SaveEps(30);
	CcfP.SaveEps(30);
	HopP.SaveEps(30);
	TriadP.SaveEps(30);
}

void TMAGFitBern::CountAttr(TFltV& EstMuV) const {
	const int NNodes = PhiVV.GetXDim();
	const int NAttrs = PhiVV.GetYDim();
	EstMuV.Gen(NAttrs);
	EstMuV.PutAll(0.0);

	for(int l = 0; l < NAttrs; l++) {
		for(int i = 0; i < NNodes; i++) {
			EstMuV[l] = EstMuV[l] + PhiVV(i, l);
		}
		EstMuV[l] = EstMuV[l] / double(NNodes);
	}
}

void TMAGFitBern::SortAttrOrdering(const TFltV& TrueMuV, TIntV& IndexV) const {
	const int NAttrs = TrueMuV.Len();
	const int NNodes = PhiVV.GetXDim();
	TFltV EstMuV, SortedTrueMuV, SortedEstMuV, TrueIdxV, EstIdxV;
	IndexV.Gen(NAttrs);
	TrueIdxV.Gen(NAttrs);
	EstIdxV.Gen(NAttrs);

	for(int l = 0; l < NAttrs; l++) {
		TrueIdxV[l] = l;
		EstIdxV[l] = l;
	}
	
	CountAttr(EstMuV);
	SortedTrueMuV = TrueMuV;
	SortedEstMuV = EstMuV;
	for(int i = 0; i < NAttrs; i++) {
		if(SortedTrueMuV[i] > 0.5) {  SortedTrueMuV[i] = 1.0 - SortedTrueMuV[i];  }
		if(SortedEstMuV[i] > 0.5) {  SortedEstMuV[i] = 1.0 - SortedEstMuV[i];  }
	}

	for(int i = 0; i < NAttrs; i++) {
		for(int j = i+1; j < NAttrs; j++) {
			if(SortedTrueMuV[i] < SortedTrueMuV[j]) {
				SortedTrueMuV.Swap(i, j);
				TrueIdxV.Swap(i, j);
			}
			if(SortedEstMuV[i] < SortedEstMuV[j]) {
				EstIdxV.Swap(SortedEstMuV[i], SortedEstMuV[j]);
				SortedEstMuV.Swap(i, j);
			}
		}
	}

	for(int l = 0; l < NAttrs; l++) {
		IndexV[l] = TrueIdxV[EstIdxV[l]];
	}
}

const bool TMAGFitBern::NextPermutation(TIntV& IndexV) const {
	const int NAttrs = IndexV.Len();
	int Pos = NAttrs - 1;
	while(Pos > 0) {
		if(IndexV[Pos-1] < IndexV[Pos]) {
			break;
		}
		Pos--;
	}
	if(Pos == 0) {
		return false;
	}

	int Val = NAttrs, NewPos = -1;
	for(int i = Pos; i < NAttrs; i++) {
		if(IndexV[i] > IndexV[Pos - 1] && IndexV[i] < Val) {
			NewPos = i;
			Val = IndexV[i];
		}
	}
	IndexV[NewPos] = IndexV[Pos - 1];
	IndexV[Pos - 1] = Val;

	TIntV SubIndexV;
    IndexV.GetSubValV(Pos, NAttrs - 1, SubIndexV);
	SubIndexV.Sort(true);
	for(int i = Pos; i < NAttrs; i++) {
		IndexV[i] = SubIndexV[i - Pos];
	}

	return true;
}

const double TMAGFitBern::ComputeJointOneLL(const TIntVV& AttrVV) const {
	double LL = 0.0;
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TMAGAffMtxV MtxV(NAttrs);	Param.GetMtxV(MtxV);
	const TMAGNodeBern NodeAttr = Param.GetNodeAttr();
	const TFltV MuV = NodeAttr.GetMuV();

	for(int l = 0; l < NAttrs; l++) {
		for(int i = 0; i < MtxV[l].Len(); i++) {
			MtxV[l].At(i) = log(MtxV[l].At(i));
		}
	}

	for(int i = 0; i < NNodes; i++) {
		for(int l = 0; l < NAttrs; l++) {
			if(AttrVV.At(i, l) == 0) {
				LL += log(MuV[l]);
			} else {
				LL += log(1.0 - MuV[l]);
			}
		}

		for(int j = 0; j < NNodes; j++) {
			if(i == j) {  continue;  }

			double ProbLL = 0.0;
			for(int l = 0; l < NAttrs; l++) {
				ProbLL += MtxV[l].At(AttrVV.At(i, l), AttrVV.At(j, l));
			}

			if(Graph->IsEdge(i, j)) {
				LL += ProbLL;
			} else {
				LL += log(1-exp(ProbLL));
			}
		}
	}

	return LL;
}

const double TMAGFitBern::ComputeJointAdjLL(const TIntVV& AttrVV) const {
	double LL = 0.0;
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TMAGAffMtxV MtxV(NAttrs);	Param.GetMtxV(MtxV);
	const TMAGNodeBern NodeAttr = Param.GetNodeAttr();
	const TFltV MuV = NodeAttr.GetMuV();

	for(int l = 0; l < NAttrs; l++) {
		for(int i = 0; i < MtxV[l].Len(); i++) {
			MtxV[l].At(i) = log(MtxV[l].At(i));
		}
	}

	for(int i = 0; i < NNodes; i++) {
		for(int j = 0; j < NNodes; j++) {
			if(i == j) {  continue;  }

			double ProbLL = 0.0;
			for(int l = 0; l < NAttrs; l++) {
				ProbLL += MtxV[l].At(AttrVV.At(i, l), AttrVV.At(j, l));
			}

			if(Graph->IsEdge(i, j)) {
				LL += ProbLL;
			} else {
				LL += log(1-exp(ProbLL));
			}
		}
	}

	return LL;
}

const double TMAGFitBern::ComputeJointLL(int NSample) const {
	double LL = 0.0;
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();

	TRnd Rnd(2000);
	TIntVV AttrVV(NNodes, NAttrs);
	int count = 0;
	for(int s = 0; s < NSample; s++) {
		for(int i = 0; i < NNodes; i++) {
			for(int l = 0; l < NAttrs; l++) {
			
				if(Rnd.GetUniDev() <= PhiVV(i, l)) {
					AttrVV.At(i, l) = 0;
				} else {
					AttrVV.At(i, l) = 1;
				}

				if(PhiVV(i, l) > 0.05 && PhiVV(i, l) < 0.95) count++;
			}
		}

		LL += ComputeJointOneLL(AttrVV);
	}
	AttrVV.Clr();

	return LL / double(NSample);
}

const double TMAGFitBern::ComputeApxLL() const {
	double LL = 0.0, LLSelf = 0.0;
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TMAGNodeBern NodeAttr = Param.GetNodeAttr();
	TFltV MuV = NodeAttr.GetMuV();
	TMAGAffMtxV LLMtxV(NAttrs);

	for(int l = 0; l < NAttrs; l++) {
		for(int i = 0; i < NNodes; i++) {
			LL += PhiVV(i, l) * log(MuV[l]);
			LL += (1.0 - PhiVV(i, l)) * log(1.0 - MuV[l]);
			LL -= PhiVV(i, l) * log(PhiVV(i, l));
			LL -= (1.0 - PhiVV(i, l)) * log(1.0 - PhiVV(i, l));
		}
		TMAGAffMtx Theta = Param.GetMtx(l);
		Theta.GetLLMtx(LLMtxV[l]);
	}

	for(int i = 0; i < NNodes; i++) {
		for(int j = 0; j < NNodes; j++) {
			if(i == j) {  continue;  }

			if(Graph->IsEdge(i, j)) {
				for(int l = 0; l < NAttrs; l++) {
					LL += GetProbPhi(i, j, l, 0, 0) * LLMtxV[l].At(0, 0);
					LL += GetProbPhi(i, j, l, 0, 1) * LLMtxV[l].At(0, 1);
					LL += GetProbPhi(i, j, l, 1, 0) * LLMtxV[l].At(1, 0);
					LL += GetProbPhi(i, j, l, 1, 1) * LLMtxV[l].At(1, 1);
				}
				LL += log(NormConst);
			} else {
				LL += log(1-exp(GetProdLinWeight(i, j)));
			}
		}
	}

	return LL;
}

const double TMAGFitBern::ComputeApxAdjLL() const {
	double LL = 0.0, LLSelf = 0.0;
	const int NNodes = Param.GetNodes();
	const int NAttrs = Param.GetAttrs();
	TMAGNodeBern NodeAttr = Param.GetNodeAttr();
	TFltV MuV = NodeAttr.GetMuV();
	MuV.PutAll(0.0);
	TMAGAffMtxV LLMtxV(NAttrs);
	double TotalEdge = 0.0;
	for(int l = 0; l < NAttrs; l++) {
		TMAGAffMtx Theta = Param.GetMtx(l);
		Theta.GetLLMtx(LLMtxV[l]);
	}

	for(int i = 0; i < NNodes; i++) {
		for(int j = 0; j < NNodes; j++) {
			if(i == j) {  continue;  }

			if(Graph->IsEdge(i, j)) {
				for(int l = 0; l < NAttrs; l++) {
					LL += GetProbPhi(i, j, l, 0, 0) * LLMtxV[l].At(0, 0);
					LL += GetProbPhi(i, j, l, 0, 1) * LLMtxV[l].At(0, 1);
					LL += GetProbPhi(i, j, l, 1, 0) * LLMtxV[l].At(1, 0);
					LL += GetProbPhi(i, j, l, 1, 1) * LLMtxV[l].At(1, 1);
				}
			} else {
				LL += log(1-exp(GetProdLinWeight(i, j)));
			}

			double TempLL = 1.0;
			for(int l = 0; l < NAttrs; l++) {
				int Ai = (double(PhiVV(i, l)) > 0.5) ? 0 : 1;
				int Aj = (double(PhiVV(j, l)) > 0.5) ? 0 : 1;
				TempLL *= Param.GetMtx(l).At(Ai, Aj);
			}
			if(TMAGNodeBern::Rnd.GetUniDev() < TempLL) {
				TotalEdge += 1.0;
			}
		}
	}

	return LL;
}

const double TMAGFitBern::ComputeMI(const TIntVV& AttrV, const int AId1, const int AId2) {
	const int NNodes = AttrV.GetXDim();
	double MI = 0.0;
	double Cor = 0.0;

	TFltVV Pxy(2,2);
	TFltV Px(2), Py(2);
	Pxy.PutAll(0.0);
	Px.PutAll(0.0);
	Py.PutAll(0.0);

	for(int i = 0; i < NNodes; i++) {
		int X = AttrV(i, AId1);
		int Y = AttrV(i, AId2);
		Pxy(X, Y) = Pxy(X, Y) + 1;
		Px[X] = Px[X] + 1;
		Py[Y] = Py[Y] + 1;
		Cor += double(X * Y);
	}

	for(int x = 0; x < 2; x++) {
		for(int y = 0; y < 2; y++) {
			MI += Pxy(x, y) / double(NNodes) * (log(Pxy(x, y)) - log(Px[x]) - log(Py[y]) + log(NNodes));
		}
	}
	
	return MI;
}

const double TMAGFitBern::ComputeMI(const TFltVV& AttrV, const int AId1, const int AId2) {
	const int NNodes = AttrV.GetXDim();
	double MI = 0.0;
	double Cor = 0.0;

	TFltVV Pxy(2,2);
	TFltV Px(2), Py(2);
	Pxy.PutAll(0.0);
	Px.PutAll(0.0);
	Py.PutAll(0.0);
	
	for(int i = 0; i < NNodes; i++) {
		double X = AttrV(i, AId1);
		double Y = AttrV(i, AId2);
		Pxy(0, 0) = Pxy(0, 0) + X * Y;
		Pxy(0, 1) = Pxy(0, 1) + X * (1 - Y);
		Pxy(1, 0) = Pxy(1, 0) + (1 - X) * Y;
		Pxy(1, 1) = (i+1) - Pxy(0, 0) - Pxy(0, 1) - Pxy(1, 0);
		Px[0] = Px[0] + X;
		Py[0] = Py[0] + Y;
		Cor += double((1-X) * (1-Y));
	}
	Px[1] = NNodes - Px[0];
	Py[1] = NNodes - Py[0];
	
	for(int x = 0; x < 2; x++) {
		for(int y = 0; y < 2; y++) {
			MI += Pxy(x, y) / double(NNodes) * (log(Pxy(x, y)) - log(Px[x]) - log(Py[y]) + log(NNodes));
		}
	}
	
	return MI;
}

const double TMAGFitBern::ComputeMI(const TIntVV& AttrV) {
	const int NNodes = AttrV.GetXDim();
	const int NAttrs = AttrV.GetYDim();
	double MI = 0.0;

	for(int l = 0; l < NAttrs; l++) {
		for(int k = l+1; k < NAttrs; k++) {
			MI += ComputeMI(AttrV, l, k);
		}
	}

	return MI;
}

const double TMAGFitBern::ComputeMI(const TFltVV& AttrV) {
	const int NNodes = AttrV.GetXDim();
	const int NAttrs = AttrV.GetYDim();
	double MI = 0.0;

	for(int l = 0; l < NAttrs; l++) {
		for(int k = l+1; k < NAttrs; k++) {
			MI += ComputeMI(AttrV, l, k);
		}
	}

	return MI;
}
