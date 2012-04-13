#include "stdafx.h"
#include "./Snap.h"
#include "./agm.h"
#include "./agmfit.h"
#include "./logreg_light.h"
#include "./agmfit.cpp"
#include "./logreg_light.cpp"


//////////////////////////////////////////////////////////////////
//AGM
void TAGM::RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd){
	int CNodes = CmtyV.Len();
	int CEdges = (int) Rnd.GetBinomialDev(Prob,CNodes*(CNodes-1)/2);
	THashSet<TIntPr> NewEdgeSet(CEdges);
	for (int edge = 0; edge < CEdges; ) {
		int SrcNId = CmtyV[Rnd.GetUniDevInt(CNodes)];
		int DstNId = CmtyV[Rnd.GetUniDevInt(CNodes)];
		if (SrcNId > DstNId) { Swap(SrcNId,DstNId); }
		if (SrcNId != DstNId && !NewEdgeSet.IsKey(TIntPr(SrcNId,DstNId))) { // is new edge
			NewEdgeSet.AddKey(TIntPr(SrcNId,DstNId));
			Graph->AddEdge(SrcNId,DstNId);
			edge++; 
		} 
	}
}


PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const int TargetEdges, TRnd& Rnd){
	PUNGraph TryG = TAGM::GenAGM(CmtyVV, DensityCoef, 1.0, Rnd);
	const double ScaleCoef = (double) TargetEdges / (double) TryG->GetEdges();
	return TAGM::GenAGM(CmtyVV, DensityCoef, ScaleCoef, Rnd);
}

PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd){
	TFltV CProbV;
	double Prob;
	for(int i=0;i<CmtyVV.Len();i++) {
		Prob = ScaleCoef*pow(double(CmtyVV[i].Len()),-DensityCoef);
		if(Prob>1){Prob = 1;}
		CProbV.Add(Prob);
	}
	return TAGM::GenAGM(CmtyVV, CProbV, Rnd);
}

PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const TFltV& CProbV, TRnd& Rnd, const double PNoCom){
	PUNGraph G = TUNGraph::New(100 * CmtyVV.Len(), -1);
	printf("AGM begins\n");
	for(int i=0;i<CmtyVV.Len();i++) {
		TIntV& CmtyV = CmtyVV[i];
		for(int u=0;u<CmtyV.Len();u++) {
			G->AddNode(CmtyV[u]);
		}
		double Prob = CProbV[i];
		//printf("\r%d(%d)/%d",i,CmtyVV[i].Len(),CmtyVV.Len());
		RndConnectInsideCommunity(G,CmtyV,Prob,Rnd);
	}
	if (PNoCom > 0.0) { //if we want to connect nodes that do not share any community
		TIntSet NIDS;
		for (int c = 0; c < CmtyVV.Len(); c++) {
			for (int u = 0; u < CmtyVV[c].Len(); u++) {
				NIDS.AddKey(CmtyVV[c][u]);
			}
		}
		TIntV NIDV;
		NIDS.GetKeyV(NIDV);
		RndConnectInsideCommunity(G,NIDV,PNoCom,Rnd);
		
	}
	printf("AGM completed (%d nodes %d edges)\n",G->GetNodes(),G->GetEdges());
	G->Defrag();
	return G;
}

////////////////////////////////////////////////////////////////////////////////////
// AGMUtil:: Utilities for AGM

void TAGMUtil::GenPLSeq(TIntV& SzSeq,const int& SeqLen, const double& Alpha, TRnd& Rnd, const int& Min, const int& Max){
	SzSeq.Gen(SeqLen, 0);
	while(SzSeq.Len() < SeqLen) {
		int Sz = (int) TMath::Round(Rnd.GetPowerDev(Alpha));
		if(Sz>=Min && Sz<=Max) {
			SzSeq.Add(Sz);
		}
	}
}

void TAGMUtil::GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd){
	TIntV NIDV(Nodes, 0);
	for (int i = 0; i < Nodes; i++) {
		NIDV.Add(i);
	}
	GenCmtyVVFromPL(CmtyVV, NIDV, Nodes, Coms, ComSzAlpha, MemAlpha, MinSz, MaxSz, MinK, MaxK, Rnd);
}
void TAGMUtil::GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const PUNGraph& Graph, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd){
	if (Coms == 0 || Nodes == 0) {
		CmtyVV.Clr();
		return;
	}
	TIntV NIDV;
	Graph->GetNIdV(NIDV);
	GenCmtyVVFromPL(CmtyVV, NIDV, Nodes, Coms, ComSzAlpha, MemAlpha, MinSz, MaxSz, MinK, MaxK, Rnd);
}
void TAGMUtil::GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const TIntV& NIDV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd){
	if (Coms == 0 || Nodes == 0) {
		CmtyVV.Clr();
		return;
	}
	TIntV ComSzSeq, MemSeq;
	TAGMUtil::GenPLSeq(ComSzSeq,Coms,ComSzAlpha,Rnd,MinSz,MaxSz);
	TAGMUtil::GenPLSeq(MemSeq,Nodes,MemAlpha,Rnd,MinK,MaxK);
	TIntPrV CIDSzPrV, NIDMemPrV;
	for (int i = 0; i < ComSzSeq.Len(); i++) {
		CIDSzPrV.Add(TIntPr(i, ComSzSeq[i]));
	}
	for (int i = 0; i < MemSeq.Len(); i++) {
		NIDMemPrV.Add(TIntPr(NIDV[i], MemSeq[i]));
	}
	TAGMUtil::ConnectCmtyVV(CmtyVV, CIDSzPrV, NIDMemPrV, Rnd);
}

void TAGMUtil::ConnectCmtyVV(TVec<TIntV>& CmtyVV, const TIntPrV& CIDSzPrV, const TIntPrV& NIDMemPrV, TRnd& Rnd) {
	const int Nodes = NIDMemPrV.Len(), Coms = CIDSzPrV.Len();
	TIntV NDegV,CDegV;
	TIntPrSet CNIDSet;
	TIntSet HitNodes(Nodes);
	THash<TInt,TIntV> CmtyVH;
	for(int i = 0;i < CIDSzPrV.Len(); i++) {
		for(int j = 0; j < CIDSzPrV[i].Val2; j++) {
			CDegV.Add(CIDSzPrV[i].Val1);
		}
	}
	for(int i = 0; i < NIDMemPrV.Len(); i++) {
		for(int j=0;j<NIDMemPrV[i].Val2;j++) {
			NDegV.Add(NIDMemPrV[i].Val1);
		}
	}

	while(CDegV.Len() < 2*Nodes) {
		CDegV.Add(CIDSzPrV[Rnd.GetUniDevInt(Coms)].Val1);
	}
	while(NDegV.Len()<CDegV.Len()) {
		NDegV.Add(NIDMemPrV[Rnd.GetUniDevInt(Nodes)].Val1);
	}
	printf("Total Mem: %d, Total Sz: %d\n",NDegV.Len(), CDegV.Len());

	int TotalComSz = CDegV.Len();
	int c=0;
	while(c++<15 && CDegV.Len() >1){
		for(int i=0;i<CDegV.Len();i++) {
			int u = Rnd.GetUniDevInt(CDegV.Len());
			int v = Rnd.GetUniDevInt(NDegV.Len());
			if(CNIDSet.IsKey(TIntPr(CDegV[u],NDegV[v]))) {continue;		}
			CNIDSet.AddKey(TIntPr(CDegV[u],NDegV[v]));
			HitNodes.AddKey(NDegV[v]);
			if(u==CDegV.Len()-1){CDegV.DelLast();}
			else{CDegV[u] = CDegV.Last();CDegV.DelLast();}
			if(v==NDegV.Len()-1){NDegV.DelLast();}
			else{NDegV[v] = NDegV.Last();NDegV.DelLast();}
		}
	}
	//make sure that every node belongs to at least one community
	for (int i = 0; i < Nodes; i++) {
		int NID = NIDMemPrV[i].Val1;
		if (!HitNodes.IsKey(NID)) {
			CNIDSet.AddKey(TIntPr(CIDSzPrV[Rnd.GetUniDevInt(Coms)].Val1, NID));
			HitNodes.AddKey(NID);
		}
	}
	IAssert(HitNodes.Len() == Nodes);
	for (int i = 0; i < CNIDSet.Len(); i++) {
		TIntPr CNIDPr = CNIDSet[i];
		CmtyVH.AddDat(CNIDPr.Val1);
		CmtyVH.GetDat(CNIDPr.Val1).Add(CNIDPr.Val2);
	}
	CmtyVH.GetDatV(CmtyVV);
}

void TAGMUtil::LoadCmtyVV(const TStr& InFNm, TVec<TIntV>& CmtyVV) {
	TSsParser Ss(InFNm, ssfWhiteSep);
	while (Ss.Next()) {
		if(Ss.GetFlds()>0) {
			TIntV CmtyV;
			for(int i=0;i<Ss.GetFlds();i++) {
				if(Ss.IsInt(i)) {
					CmtyV.Add(Ss.GetInt(i));
				}
			}
			CmtyVV.Add(CmtyV);
		}
	}
	printf("community loading completed (%d communities)\n",CmtyVV.Len());

}

void TAGMUtil::DumpCmtyVV(const TStr& OutFNm, const TVec<TIntV>& CmtyVV) {
	FILE* F = fopen(OutFNm.CStr(),"wt");
	for(int i=0;i<CmtyVV.Len();i++) {
		for(int j=0;j<CmtyVV[i].Len();j++) {
			fprintf(F,"%d\t", (int) CmtyVV[i][j]);
		}
		fprintf(F,"\n");
	}
	fclose(F);
}

int TAGMUtil::TotalMemberships(const TVec<TIntV>& CmtyVV){
	int M = 0;
	for(int i=0;i<CmtyVV.Len();i++) {
		M += CmtyVV[i].Len();
	}
	return M;
}


void TAGMUtil::GetNodeMembership(TIntH& NIDComVH, const THash<TInt,TIntV >& CmtyVH) {
	for(THash<TInt,TIntV>::TIter HI = CmtyVH.BegI(); HI < CmtyVH.EndI(); HI++){
		int CID = HI.GetKey();
		for(int j = 0;j < HI.GetDat().Len(); j++) {
			int NID = HI.GetDat()[j];
			NIDComVH.AddDat(NID)++;
		}
	}
}
void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntV>& CmtyVV) {
	for(int i=0;i<CmtyVV.Len();i++){
		int CID =i;
		for(int j=0;j<CmtyVV[i].Len();j++) {
			int NID = CmtyVV[i][j];
			NIDComVH.AddDat(NID).AddKey(CID);
		}
	}
}
void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntSet>& CmtyVV) {
	for(int i=0;i<CmtyVV.Len();i++){
		int CID =i;
		for(TIntSet::TIter SI = CmtyVV[i].BegI(); SI < CmtyVV[i].EndI(); SI++) {
			int NID = SI.GetKey();
			NIDComVH.AddDat(NID).AddKey(CID);
		}
	}
}
void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const THash<TInt,TIntV>& CmtyVH) {
	for(THash<TInt,TIntV>::TIter HI = CmtyVH.BegI(); HI < CmtyVH.EndI(); HI++){
		int CID = HI.GetKey();
		for(int j = 0;j < HI.GetDat().Len(); j++) {
			int NID = HI.GetDat()[j];
			NIDComVH.AddDat(NID).AddKey(CID);
		}
	}
}

void TAGMUtil::GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const THash<TInt,TIntV>& CmtyVH) {
	for(int i=0;i<CmtyVH.Len();i++){
		int CID = CmtyVH.GetKey(i);
		for(int j=0;j<CmtyVH[i].Len();j++) {
			int NID = CmtyVH[i][j];
			NIDComVH.AddDat(NID).Add(CID);
		}
	}
}

void TAGMUtil::GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const TVec<TIntV>& CmtyVV) {
	THash<TInt,TIntV> CmtyVH;
	for(int i=0;i<CmtyVV.Len();i++) {
		CmtyVH.AddDat(i,CmtyVV[i]);
	}
	GetNodeMembership(NIDComVH,CmtyVH);
}

int TAGMUtil::Intersection(const TIntV& C1, const TIntV& C2) {
	TIntSet S1(C1), S2(C2);
	return TAGMUtil::Intersection(S1, S2);
}

void TAGMUtil::GetIntersection(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& C) {
	C.Gen(A.Len());
	if (A.Len() < B.Len()) {
		for (THashSetKeyI<TInt> it=A.BegI(); it<A.EndI(); it++) 
			if (B.IsKey(it.GetKey())) C.AddKey(it.GetKey());
	} else {
		for (THashSetKeyI<TInt> it=B.BegI(); it<B.EndI(); it++) 
			if (A.IsKey(it.GetKey())) C.AddKey(it.GetKey());
	}
}

int TAGMUtil::Intersection(const THashSet<TInt>& A, const THashSet<TInt>& B) {
	int n = 0;
	if (A.Len() < B.Len()) {
		for (THashSetKeyI<TInt> it=A.BegI(); it<A.EndI(); it++) 
			if (B.IsKey(it.GetKey())) n++;
	} else {
		for (THashSetKeyI<TInt> it=B.BegI(); it<B.EndI(); it++) 
			if (A.IsKey(it.GetKey())) n++;
	}
	return n;
}


int TAGMUtil::FindComsByAGM(const PUNGraph& Graph, const int InitComs, const int MaxIter, const int RndSeed, const double RegGap, const double PNoCom, const TStr PltFPrx) {
	TRnd Rnd(RndSeed);
	int LambdaIter = 100;
	if (Graph->GetNodes() < 200) { LambdaIter = 1; } 
	if (Graph->GetNodes() < 200 && Graph->GetEdges() > 2000) { LambdaIter = 100; } 

	//Find coms with large C
	TAGMFit AGMFitM(Graph, InitComs, RndSeed);
	if (PNoCom > 0.0) { AGMFitM.SetPNoCom(PNoCom); }
	AGMFitM.RunMCMC(MaxIter, LambdaIter, "");

	int TE = Graph->GetEdges();
	TFltV RegV; 
	RegV.Add(0.3 * TE);
	for (int r = 0; r < 25; r++) {
		RegV.Add(RegV.Last() * RegGap);
	}
	TFltPrV RegComsV, RegLV, RegBICV;
	TFltV LV, BICV;
	//record likelihood and number of communities with nonzero P_c
	for (int r = 0; r < RegV.Len(); r++) {
		double RegCoef = RegV[r];
		AGMFitM.RegCoef = RegCoef;
		AGMFitM.MLEGradAscentGivenCAG(0.01, 1000);
		double RegL = AGMFitM.Likelihood();
		AGMFitM.RegCoef = 0.0;
		double WORegL = AGMFitM.Likelihood();
				
		TVec<TIntV> EstCmtyVV;
		AGMFitM.GetCmtyVV(EstCmtyVV, 0.99);
		int NumLowQ = EstCmtyVV.Len();
		RegComsV.Add(TFltPr(RegCoef, (double) NumLowQ));

		if (EstCmtyVV.Len() > 0) {
			TAGMFit AFTemp(Graph, EstCmtyVV, Rnd);
			AFTemp.MLEGradAscentGivenCAG(0.001, 1000);
			double CurL = AFTemp.Likelihood();
			LV.Add(CurL);
			BICV.Add(-2.0 * CurL + (double) EstCmtyVV.Len() * log((double) Graph->GetNodes() * (Graph->GetNodes() - 1) / 2.0));
		}
		else {
			break;
		}
	}

	//normalize likelihood and BIC to 0~100
	int MaxL = 100;
	{
		TFltV& ValueV = LV;
		TFltPrV& RegValueV = RegLV;
		double MinValue = TFlt::Mx, MaxValue = TFlt::Mn;
		for (int l = 0; l < ValueV.Len(); l++) {
			if (ValueV[l] < MinValue) { MinValue = ValueV[l]; }
			if (ValueV[l] > MaxValue) { MaxValue = ValueV[l]; }
		}
		while (ValueV.Len() < RegV.Len()) { ValueV.Add(MinValue); }
		double RangeVal = MaxValue - MinValue;
		for (int l = 0; l < ValueV.Len(); l++) {
			RegValueV.Add(TFltPr(RegV[l], double(MaxL) * (ValueV[l] - MinValue) / RangeVal));
		}
		
	}
	{
		TFltV& ValueV = BICV;
		TFltPrV& RegValueV = RegBICV;
		double MinValue = TFlt::Mx, MaxValue = TFlt::Mn;
		for (int l = 0; l < ValueV.Len(); l++) {
			if (ValueV[l] < MinValue) { MinValue = ValueV[l]; }
			if (ValueV[l] > MaxValue) { MaxValue = ValueV[l]; }
		}
		while (ValueV.Len() < RegV.Len()) { ValueV.Add(MaxValue); }
		double RangeVal = MaxValue - MinValue;
		for (int l = 0; l < ValueV.Len(); l++) {
			RegValueV.Add(TFltPr(RegV[l], double(MaxL) * (ValueV[l] - MinValue) / RangeVal));
		}
	}

	//fit logistic regression to normalized likelihood.
	TVec<TFltV> XV(RegLV.Len());
	TFltV YV (RegLV.Len());
	for (int l = 0; l < RegLV.Len(); l++) {
		XV[l] = TFltV::GetV(log(RegLV[l].Val1));
		YV[l] = RegLV[l].Val2 / (double) MaxL;
	}
	TFltPrV LRVScaled, LRV;
	TLogRegFit LRFit;
	PLogRegPredict LRMd = LRFit.CalcLogRegNewton(XV, YV, PltFPrx);
	for (int l = 0; l < RegLV.Len(); l++) {
		LRV.Add(TFltPr(RegV[l], LRMd->GetCfy(XV[l])));
		LRVScaled.Add(TFltPr(RegV[l], double(MaxL) * LRV.Last().Val2));
	}

	//estimate # communities from fitted logistic regression
	int NumComs = 0, IdxRegDrop = 0;
	double LRThres = 1.1, RegDrop; // 1 / (1 + exp(1.1)) = 0.25
	double LeftReg = 0.0, RightReg = 0.0;
	TFltV Theta;
	LRMd->GetTheta(Theta);
	RegDrop = (- Theta[1] - LRThres) / Theta[0];
	if (RegDrop <= XV[0][0]) { NumComs = (int) RegComsV[0].Val2; }
	else if (RegDrop >= XV.Last()[0]) { NumComs = (int) RegComsV.Last().Val2; }
	else {	//interpolate for RegDrop
		for (int i = 0; i < XV.Len(); i++) {
			if (XV[i][0] > RegDrop) { IdxRegDrop = i; break; }
		}
		IAssert(IdxRegDrop > 0);
		LeftReg = RegDrop - XV[IdxRegDrop - 1][0];
		RightReg = XV[IdxRegDrop][0] - RegDrop;
		NumComs = TMath::Round( (RightReg * RegComsV[IdxRegDrop - 1].Val2 + LeftReg * RegComsV[IdxRegDrop].Val2) / (LeftReg + RightReg));

	}
	printf("Num Coms:%d\n", NumComs);
	if (NumComs < 2) { NumComs = 2; }

	if (PltFPrx.Len() > 0) {
		TStr PlotTitle = TStr::Fmt("N:%d, E:%d ", Graph->GetNodes(), TE);
		TGnuPlot GPC(PltFPrx + ".l");
		GPC.AddPlot(RegComsV, gpwLinesPoints, "C");
		GPC.AddPlot(RegLV, gpwLinesPoints, "likelihood");
		GPC.AddPlot(RegBICV, gpwLinesPoints, "BIC");
		GPC.AddPlot(LRVScaled, gpwLinesPoints, "Sigmoid (scaled)");
		GPC.SetScale(gpsLog10X);
		GPC.SetTitle(PlotTitle);
		GPC.SavePng(PltFPrx + ".l.png");
	}
	
	return NumComs;
}
