#include "stdafx.h"
#include "./Snap.h"
#include "./agm.h"
#include "./logreg_light.h"
#include "./logreg_light.cpp"


//////////////////////////////////////////////////////////////////
//AGM
void TAGM::RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd){
	int CNodes = CmtyV.Len(), CEdges;
	if (CNodes < 20) {
		CEdges = (int) Rnd.GetBinomialDev(Prob, CNodes * (CNodes-1) / 2);
	} else {
		CEdges = (int) (Prob * CNodes * (CNodes-1) / 2);
	}
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
		Prob = ScaleCoef * pow(double(CmtyVV[i].Len()), - DensityCoef);
		if(Prob > 1) { Prob = 1;}
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

void TAGMUtil::DumpCmtyVV(const TStr OutFNm, TVec<TIntV>& CmtyVV, TIntStrH& NIDNmH) {
	FILE* F = fopen(OutFNm.CStr(), "wt");
	for (int c = 0; c < CmtyVV.Len(); c++) {
		for (int u = 0; u < CmtyVV[c].Len(); u++) {
			if (NIDNmH.IsKey(CmtyVV[c][u])){
				fprintf(F, "%s\t", NIDNmH.GetDat(CmtyVV[c][u]).CStr());
			}
			else {
				fprintf(F, "%d\t", (int) CmtyVV[c][u]);
			}
		}
		fprintf(F, "\n");
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

void TAGMUtil::SaveGephi(const TStr& OutFNm, const PUNGraph& G, const TVec<TIntV>& CmtyVVAtr, const double MaxSz, const double MinSz, const TIntStrH& NIDNameH, const THash<TInt, TIntTr>& NIDColorH ) {
	THash<TInt,TIntV> NIDComVHAtr;
	TAGMUtil::GetNodeMembership(NIDComVHAtr, CmtyVVAtr);

	FILE* F = fopen(OutFNm.CStr(), "wt");
	fprintf(F, "<?xml version='1.0' encoding='UTF-8'?>\n");
	fprintf(F, "<gexf xmlns='http://www.gexf.net/1.2draft' xmlns:viz='http://www.gexf.net/1.1draft/viz' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd' version='1.2'>\n");
	fprintf(F, "\t<graph mode='static' defaultedgetype='undirected'>\n");
	if (CmtyVVAtr.Len() > 0) {
		fprintf(F, "\t<attributes class='node'>\n");
		for (int c = 0; c < CmtyVVAtr.Len(); c++) {
			fprintf(F, "\t\t<attribute id='%d' title='c%d' type='boolean'>", c, c);
			fprintf(F, "\t\t<default>false</default>\n");
			fprintf(F, "\t\t</attribute>\n");
		}
		fprintf(F, "\t</attributes>\n");
	}
	fprintf(F, "\t\t<nodes>\n");
	for (TUNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
		int NID = NI.GetId();
		TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): "";
		TIntTr Color = NIDColorH.IsKey(NID)? NIDColorH.GetDat(NID) : TIntTr(120, 120, 120);

		double Size = MinSz;
		double SizeStep = (MaxSz - MinSz) / (double) CmtyVVAtr.Len();
		if (NIDComVHAtr.IsKey(NID)) {
			Size = MinSz +  SizeStep *  (double) NIDComVHAtr.GetDat(NID).Len();
		}
		double Alpha = 1.0;
		fprintf(F, "\t\t\t<node id='%d' label='%s'>\n", NID, Label.CStr());
		fprintf(F, "\t\t\t\t<viz:color r='%d' g='%d' b='%d' a='%.1f'/>\n", Color.Val1.Val, Color.Val2.Val, Color.Val3.Val, Alpha);
		fprintf(F, "\t\t\t\t<viz:size value='%.3f'/>\n", Size);
		//specify attributes
		if (NIDComVHAtr.IsKey(NID)) {
			fprintf(F, "\t\t\t\t<attvalues>\n");
			for (int c = 0; c < NIDComVHAtr.GetDat(NID).Len(); c++) {
				int CID = NIDComVHAtr.GetDat(NID)[c];
				fprintf(F, "\t\t\t\t\t<attvalue for='%d' value='true'/>\n", CID);
			}
			fprintf(F, "\t\t\t\t</attvalues>\n");
		}

		fprintf(F, "\t\t\t</node>\n");
	}
	fprintf(F, "\t\t</nodes>\n");
	//plot edges
	int EID = 0;
	fprintf(F, "\t\t<edges>\n");
	for (TUNGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
		fprintf(F, "\t\t\t<edge id='%d' source='%d' target='%d'/>\n", EID++, EI.GetSrcNId(), EI.GetDstNId());
	}
	fprintf(F, "\t\t</edges>\n");
	fprintf(F, "\t</graph>\n");
	fprintf(F, "</gexf>\n");
	fclose(F);
}


int TAGMUtil::FindComsByAGM(const PUNGraph& Graph, const int InitComs, const int MaxIter, const int RndSeed, const double RegGap, const double PNoCom, const TStr PltFPrx) {
	TRnd Rnd(RndSeed);
	int LambdaIter = 100;
	if (Graph->GetNodes() < 200) { LambdaIter = 10; } 
	if (Graph->GetNodes() > 200 && Graph->GetEdges() > 2000) { LambdaIter = 100; } 

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
		NumComs = (int) TMath::Round( (RightReg * RegComsV[IdxRegDrop - 1].Val2 + LeftReg * RegComsV[IdxRegDrop].Val2) / (LeftReg + RightReg));

	}
	printf("Number of communities that would exist in the graph:%d\n", NumComs);
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

/////////////////////////////////////////////////////////////////////
// AGM fitting
void TAGMFit::RandomInitCmtyVV(const int InitComs, const double ComSzAlpha, const double MemAlpha, const int MinComSz, const int MaxComSz, const int MinMem, const int MaxMem) {
	TVec<TIntV> InitCmtyVV(InitComs, 0);
	TAGMUtil::GenCmtyVVFromPL(InitCmtyVV, G, G->GetNodes(), InitComs, ComSzAlpha, MemAlpha, MinComSz, MaxComSz,
			MinMem, MaxMem, Rnd);
	SetCmtyVV(InitCmtyVV);
}

void TAGMFit::GetEdgeJointCom() {
	ComEdgesV.Gen(CIDNSetV.Len());
	EdgeComVH.Gen(G->GetEdges());
	for (TUNGraph::TNodeI SrcNI = G->BegNI(); SrcNI < G->EndNI(); SrcNI++) {
		int SrcNID = SrcNI.GetId();
		for(int v = 0; v < SrcNI.GetDeg(); v++) {
			int DstNID = SrcNI.GetNbhNId(v);
			if (SrcNID >= DstNID) { continue; }
			TIntSet JointCom;
			TAGMUtil::GetIntersection(NIDComVH.GetDat(SrcNID), NIDComVH.GetDat(DstNID), JointCom);
			EdgeComVH.AddDat(TIntPr(SrcNID,DstNID),JointCom);
			for(int k = 0; k < JointCom.Len(); k++) {
				ComEdgesV[JointCom[k]]++;
			}
		}
	}
	IAssert(EdgeComVH.Len() == G->GetEdges());
}

void TAGMFit::SetDefaultPNoCom() {
	PNoCom = 1.0 / (double) G->GetNodes() / (double) G->GetNodes();
}

double TAGMFit::Likelihood(const TFltV& NewLambdaV, double& LEdges, double& LNoEdges) {
	IAssert(CIDNSetV.Len() == NewLambdaV.Len());
	IAssert(ComEdgesV.Len() == CIDNSetV.Len());
	LEdges = 0.0; LNoEdges = 0.0;
	for(int e = 0; e < EdgeComVH.Len(); e++) {
		TIntSet& JointCom = EdgeComVH[e];
		double LambdaSum = SelectLambdaSum(NewLambdaV, JointCom);
		double Puv = 1 - exp(- LambdaSum);
		if (JointCom.Len() == 0) {	Puv = PNoCom;	}
		IAssert(! _isnan(log(Puv)));
		LEdges += log(Puv);
	}
	for(int k=0;k<NewLambdaV.Len();k++) {
		int MaxEk = CIDNSetV[k].Len() * (CIDNSetV[k].Len() - 1) / 2;
		int NotEdgesInCom = MaxEk - ComEdgesV[k];
		if(NotEdgesInCom > 0) {
			if (LNoEdges >= TFlt::Mn + double(NotEdgesInCom) * NewLambdaV[k]) { 
				LNoEdges -= double(NotEdgesInCom) * NewLambdaV[k];
			}
		}
	}
	double LReg = 0.0;
	if (RegCoef > 0.0) {
		LReg = - RegCoef * TLinAlg::SumVec(NewLambdaV);
	}
	return LEdges + LNoEdges + LReg;
}

double TAGMFit::Likelihood() { return Likelihood(LambdaV); }

double TAGMFit::GetStepSizeByLineSearchForLambda(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta) {
	double StepSize = 1.0;
	double InitLikelihood = Likelihood();
	IAssert(LambdaV.Len() == DeltaV.Len());
	TFltV NewLambdaV(LambdaV.Len());

	for(int iter = 0; ; iter++) {
		for (int i = 0; i < LambdaV.Len(); i++){
			NewLambdaV[i] = LambdaV[i] + StepSize * DeltaV[i];
			if (NewLambdaV[i] < MinLambda) { NewLambdaV[i] = MinLambda;	}
			if (NewLambdaV[i] > MaxLambda) { NewLambdaV[i] = MaxLambda; }
		}
		if (Likelihood(NewLambdaV) < InitLikelihood + Alpha * StepSize * TLinAlg::DotProduct(GradV, DeltaV)) {
			StepSize *= Beta;
		} else {
			break;
		}
	}
	return StepSize;
}

int TAGMFit::MLEGradAscentGivenCAG(const double& Thres, const int& MaxIter, const TStr PlotNm) {
	int Edges = G->GetEdges();
	TExeTm ExeTm;
	TFltV GradV(LambdaV.Len());
	int iter = 0;
	TIntFltPrV IterLV, IterGradNormV;
	double InitLearnRate = 0.01;
	double GradCutOff = 1000;
	for(iter = 0; iter < MaxIter; iter++) {
		GradLogLForLambda(GradV);		//if gradient is going out of the boundary, cut off
		for(int i = 0; i < LambdaV.Len(); i++) {
			if (GradV[i] < -GradCutOff) { GradV[i] = -GradCutOff; }
			if (GradV[i] > GradCutOff) { GradV[i] = GradCutOff; }
			if (LambdaV[i] <= MinLambda && GradV[i] < 0) { GradV[i] = 0.0; }
			if (LambdaV[i] >= MaxLambda && GradV[i] > 0) { GradV[i] = 0.0; }
		}
		double Alpha = 0.15, Beta = 0.2;
		if (Edges > Kilo(100)) { Alpha = 0.00015; Beta = 0.3;}
		double LearnRate = GetStepSizeByLineSearchForLambda(GradV, GradV, Alpha, Beta);
		if (TLinAlg::Norm(GradV) < Thres) { break; }
		for(int i = 0; i < LambdaV.Len(); i++) {
			double Change = LearnRate * GradV[i];
			LambdaV[i] += Change;
			if(LambdaV[i] < MinLambda) { LambdaV[i] = MinLambda;}
			if(LambdaV[i] > MaxLambda) { LambdaV[i] = MaxLambda;}
		}
		if (! PlotNm.Empty()) {
			double L = Likelihood();
			IterLV.Add(TIntFltPr(iter, L));
			IterGradNormV.Add(TIntFltPr(iter, TLinAlg::Norm(GradV)));
		}
		
	}
	if (! PlotNm.Empty()) {
		TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
		TGnuPlot::PlotValV(IterGradNormV, PlotNm + ".gradnorm_Q");
		printf("MLE for Lambda completed with %d iterations(%s)\n",iter,ExeTm.GetTmStr());
	}
	return iter;
}

void TAGMFit::RandomInit(const int& MaxK) {
	CIDNSetV.Clr();
	for (int c = 0; c < MaxK; c++) {
		CIDNSetV.Add();
		int NC = Rnd.GetUniDevInt(G -> GetNodes());
		TUNGraph::TNodeI NI = G -> GetRndNI();
		CIDNSetV.Last().AddKey(NI.GetId());
		for (int v = 0; v < NC; v++) {
			NI = G->GetNI(NI.GetNbhNId(Rnd.GetUniDevInt(NI.GetDeg())));
			CIDNSetV.Last().AddKey(NI.GetId());
		}
	}
	InitNodeData();
	SetDefaultPNoCom();
}

void TAGMFit::AddBaseCmty() {
	TVec<TIntV> CmtyVV;
	GetCmtyVV(CmtyVV);
	TIntV TmpV = CmtyVV[0];
	CmtyVV.Add(TmpV);
	G->GetNIdV(CmtyVV[0]);
	IAssert(CIDNSetV.Len() + 1 == CmtyVV.Len());
	SetCmtyVV(CmtyVV);
	InitNodeData();
	BaseCID = 0;
	PNoCom = 0.0;
}

void TAGMFit::InitNodeData(){
	TSnap::DelSelfEdges(G);
	NIDComVH.Gen(G->GetNodes());
	for (TUNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
		NIDComVH.AddDat(NI.GetId());
	}
	TAGMUtil::GetNodeMembership(NIDComVH, CIDNSetV);
	GetEdgeJointCom();
	LambdaV.Gen(CIDNSetV.Len());
	for (int c = 0; c < CIDNSetV.Len(); c++) {
		int MaxE = (CIDNSetV[c].Len()) * (CIDNSetV[c].Len()-1) / 2;
		if (MaxE < 2) {
			LambdaV[c] = MaxLambda;
		}
		else{
			LambdaV[c] = -log((double)(MaxE - ComEdgesV[c]) / MaxE);
		}
		if (LambdaV[c] > MaxLambda) {	LambdaV[c] = MaxLambda;	}
		if (LambdaV[c] < MinLambda) {	LambdaV[c] = MinLambda;	}
	}
	NIDCIDPrS.Gen(G->GetNodes() * 10);
	for (int c = 0; c < CIDNSetV.Len(); c++) {
		for (TIntSet::TIter SI = CIDNSetV[c].BegI(); SI < CIDNSetV[c].EndI(); SI++) {
			NIDCIDPrS.AddKey(TIntPr(SI.GetKey(), c));
		}
	}
}

void TAGMFit::LeaveCom(const int& NID, const int& CID) {
	TUNGraph::TNodeI NI = G->GetNI(NID);
	for (int e = 0; e < NI.GetDeg(); e++) {
		int VID = NI.GetNbhNId(e);
		if (NIDComVH.GetDat(VID).IsKey(CID)) {
			TIntPr SrcDstNIDPr = TIntPr(TMath::Mn(NID,VID), TMath::Mx(NID,VID));
			EdgeComVH.GetDat(SrcDstNIDPr).DelKey(CID);
			ComEdgesV[CID]--;
		}
	}
	CIDNSetV[CID].DelKey(NID);
	NIDComVH.GetDat(NID).DelKey(CID);
	NIDCIDPrS.DelKey(TIntPr(NID, CID));
}

void TAGMFit::JoinCom(const int& NID, const int& JoinCID){
	TUNGraph::TNodeI NI = G->GetNI(NID);
	for (int e = 0; e < NI.GetDeg(); e++){
		int VID = NI.GetNbhNId(e);
		if (NIDComVH.GetDat(VID).IsKey(JoinCID)){
			TIntPr SrcDstNIDPr = TIntPr(TMath::Mn(NID,VID), TMath::Mx(NID,VID));
			EdgeComVH.GetDat(SrcDstNIDPr).AddKey(JoinCID);
			ComEdgesV[JoinCID]++;
		}
	}
	CIDNSetV[JoinCID].AddKey(NID);
	NIDComVH.GetDat(NID).AddKey(JoinCID);
	NIDCIDPrS.AddKey(TIntPr(NID, JoinCID));
}

void TAGMFit::SampleTransition(int& NID, int& JoinCID, int& LeaveCID, double& DeltaL){
	int Option = Rnd.GetUniDevInt(3); //0:Join 1:Leave 2:Switch
	if (NIDCIDPrS.Len() <= 1) {		Option = 0;	} //if there is only one node membership, only join is possible.
	int TryCnt = 0;
	const int MaxTryCnt = G->GetNodes();
	DeltaL = TFlt::Mn;
	if (Option == 0) {
		do {
			JoinCID = Rnd.GetUniDevInt(CIDNSetV.Len());
			NID = G->GetRndNId();
		} while (TryCnt++ < MaxTryCnt && NIDCIDPrS.IsKey(TIntPr(NID, JoinCID)));
		if (TryCnt < MaxTryCnt) { //if successfully find a move
			DeltaL = SeekJoin(NID, JoinCID);
		}
	}
	else if (Option == 1) {
		do {
			TIntPr NIDCIDPr = NIDCIDPrS.GetKey(NIDCIDPrS.GetRndKeyId(Rnd));
			NID = NIDCIDPr.Val1;
			LeaveCID = NIDCIDPr.Val2;
		} while (TryCnt++ < MaxTryCnt && LeaveCID == BaseCID);
		if (TryCnt < MaxTryCnt) {//if successfully find a move
			DeltaL = SeekLeave(NID, LeaveCID);
		}
	}
	else{
		do {
			TIntPr NIDCIDPr = NIDCIDPrS.GetKey(NIDCIDPrS.GetRndKeyId(Rnd));
			NID = NIDCIDPr.Val1;
			LeaveCID = NIDCIDPr.Val2;
		} while (TryCnt++ < MaxTryCnt && (NIDComVH.GetDat(NID).Len() == CIDNSetV.Len() || LeaveCID == BaseCID));
		do {
			JoinCID = Rnd.GetUniDevInt(CIDNSetV.Len());
		} while (TryCnt++ < G->GetNodes() && NIDCIDPrS.IsKey(TIntPr(NID, JoinCID)));
		if (TryCnt < MaxTryCnt) {//if successfully find a move
			DeltaL = SeekSwitch(NID, LeaveCID, JoinCID);
		}
	}
}

void TAGMFit::RunMCMC(const int& MaxIter, const int& EvalLambdaIter, const TStr& PlotFPrx) {
	TExeTm IterTm, TotalTm;
	double PrevL = Likelihood(), DeltaL = 0;
	double BestL = PrevL;
	
	printf("initial likelihood = %f\n",PrevL);
	
	TIntFltPrV IterTrueLV, IterJoinV, IterLeaveV, IterAcceptV, IterSwitchV, IterLBV;
	TIntPrV IterTotMemV;
	TIntV IterV;
	TFltV BestLV;
	TVec<TIntSet> BestCmtySetV;
	int IterAfterBurn = 0;
	int SwitchCnt = 0, LeaveCnt = 0, JoinCnt = 0, AcceptCnt = 0, ProbBinSz;
	int Nodes = G->GetNodes(), Edges = G->GetEdges();
	TExeTm PlotTm;

	ProbBinSz = TMath::Mx(1000, G->GetNodes() / 10);//bin to compute probabilities
	IterLBV.Add(TIntFltPr(1, BestL));

	//int TempIter = G->GetNodes() * G->GetNodes();
	for (int iter = 0; iter < MaxIter; iter++) {
		IterTm.Tick();
		int NID = -1;
		int JoinCID = -1, LeaveCID = -1;
		SampleTransition(NID, JoinCID, LeaveCID, DeltaL); //sample a move
		double OptL = PrevL;
		if (DeltaL > 0 || Rnd.GetUniDev() < exp(DeltaL)) { //if it is accepted
			IterTm.Tick();
			if (LeaveCID > -1 && LeaveCID != BaseCID) { LeaveCom(NID, LeaveCID); }
			if (JoinCID > -1 && JoinCID != BaseCID) { JoinCom(NID, JoinCID); }
			if (LeaveCID > -1 && JoinCID > -1 && JoinCID != BaseCID && LeaveCID != BaseCID) { SwitchCnt++; }
			else if (LeaveCID > -1  && LeaveCID != BaseCID) { LeaveCnt++;}
			else if (JoinCID > -1 && JoinCID != BaseCID) { JoinCnt++;}
			AcceptCnt++;
			if ((iter + 1) % EvalLambdaIter == 0) {
				IterTm.Tick();
				MLEGradAscentGivenCAG(0.01, 3);
				OptL = Likelihood();
			}
			else{
				OptL = PrevL + DeltaL;
			}
			if (BestL <= OptL && CIDNSetV.Len() > 0) {
				BestCmtySetV = CIDNSetV;
				BestLV = LambdaV;
				BestL = OptL;
			}
		}
		if (iter > 0 && (iter % ProbBinSz == 0) && PlotFPrx.Len() > 0) { 
			IterLBV.Add(TIntFltPr(iter, OptL));
			IterSwitchV.Add(TIntFltPr(iter, (double) SwitchCnt / (double) AcceptCnt));
			IterLeaveV.Add(TIntFltPr(iter, (double) LeaveCnt / (double) AcceptCnt));
			IterJoinV.Add(TIntFltPr(iter, (double) JoinCnt / (double) AcceptCnt));
			IterAcceptV.Add(TIntFltPr(iter, (double) AcceptCnt / (double) ProbBinSz));
			SwitchCnt = JoinCnt = LeaveCnt = AcceptCnt = 0;
		}
		PrevL = OptL;

	}
	
	// plot the likelihood and acceptance probabilities if the plot file name is given
	if (PlotFPrx.Len() > 0) {
		TGnuPlot GP1;
		GP1.AddPlot(IterLBV, gpwLinesPoints, "likelihood");
		GP1.SetDataPlotFNm(PlotFPrx + ".likelihood.tab", PlotFPrx + ".likelihood.plt");
		TStr TitleStr = TStr::Fmt(" N:%d E:%d", Nodes, Edges);
		GP1.SetTitle(PlotFPrx + ".likelihood" + TitleStr);
		GP1.SavePng(PlotFPrx + ".likelihood.png");

		TGnuPlot GP2;
		GP2.AddPlot(IterSwitchV, gpwLinesPoints, "Switch");
		GP2.AddPlot(IterLeaveV, gpwLinesPoints, "Leave");
		GP2.AddPlot(IterJoinV, gpwLinesPoints, "Join");
		GP2.AddPlot(IterAcceptV, gpwLinesPoints, "Accept");
		GP2.SetTitle(PlotFPrx + ".transition");
		GP2.SetDataPlotFNm(PlotFPrx + "transition_prob.tab", PlotFPrx + "transition_prob.plt");
		GP2.SavePng(PlotFPrx + "transition_prob.png");
	}
	CIDNSetV = BestCmtySetV;
	LambdaV = BestLV;
	int DelCnt = RemoveEmptyCom();

	InitNodeData();
	MLEGradAscentGivenCAG(0.001, 100);
	printf("MCMC completed (best likelihood: %.2f) [%s]\n", BestL, TotalTm.GetTmStr());
}

void TAGMFit::GetQV(TFltV& OutV){
	OutV.Gen(LambdaV.Len());
	for (int i = 0; i < LambdaV.Len(); i++) {
		OutV[i] = exp(- LambdaV[i]);
	}
}

int TAGMFit::RemoveEmptyCom() {
	int DelCnt = 0;
	for (int c = 0; c < CIDNSetV.Len(); c++) {
		if (CIDNSetV[c].Len() == 0){
			CIDNSetV[c] = CIDNSetV.Last();
			CIDNSetV.DelLast();
			LambdaV[c] = LambdaV.Last();
			LambdaV.DelLast();
			DelCnt++;
			c--;
		}
	}
	return DelCnt;
}

double TAGMFit::SeekLeave(const int& UID, const int& CID) {
	IAssert(CIDNSetV[CID].IsKey(UID));
	IAssert(G->IsNode(UID));
	double Delta = 0.0;
	TUNGraph::TNodeI NI = G->GetNI(UID);
	int NbhsInC = 0;
	for (int e = 0; e < NI.GetDeg(); e++){
		const int VID = NI.GetNbhNId(e);
		if (! NIDComVH.GetDat(VID).IsKey(CID)) { continue; }
		TIntPr SrcDstNIDPr(TMath::Mn(UID,VID), TMath::Mx(UID,VID));
		TIntSet& JointCom = EdgeComVH.GetDat(SrcDstNIDPr);
		double CurPuv, NewPuv, LambdaSum = SelectLambdaSum(JointCom);
		CurPuv = 1 - exp(- LambdaSum);
		NewPuv = 1 - exp(- LambdaSum + LambdaV[CID]);
		IAssert(JointCom.Len() > 0);
		if (JointCom.Len() == 1) {
			NewPuv = PNoCom;
		}
		Delta += (log(NewPuv) - log(CurPuv));
		IAssert(!_isnan(Delta));
		NbhsInC++;
	}
	Delta += LambdaV[CID] * (CIDNSetV[CID].Len() - 1 - NbhsInC);
	return Delta;
}

double TAGMFit::SeekJoin(const int& UID, const int& CID) {
	IAssert(! CIDNSetV[CID].IsKey(UID));
	double Delta = 0.0;
	TUNGraph::TNodeI NI = G->GetNI(UID);
	int NbhsInC = 0;
	for (int e = 0; e < NI.GetDeg(); e++) {
		const int VID = NI.GetNbhNId(e);
		if (! NIDComVH.GetDat(VID).IsKey(CID)) { continue; }
		TIntPr SrcDstNIDPr(TMath::Mn(UID,VID), TMath::Mx(UID,VID));
		TIntSet& JointCom = EdgeComVH.GetDat(SrcDstNIDPr);
		double CurPuv, NewPuv, LambdaSum = SelectLambdaSum(JointCom);
		CurPuv = 1 - exp(- LambdaSum);
		if (JointCom.Len() == 0) { CurPuv = PNoCom; }
		NewPuv = 1 - exp(- LambdaSum - LambdaV[CID]);
		Delta += (log(NewPuv) - log(CurPuv));
		IAssert(!_isnan(Delta));
		NbhsInC++;
	}
	Delta -= LambdaV[CID] * (CIDNSetV[CID].Len() - NbhsInC);
	return Delta;
}

double TAGMFit::SeekSwitch(const int& UID, const int& CurCID, const int& NewCID){
	IAssert(! CIDNSetV[NewCID].IsKey(UID));
	IAssert(CIDNSetV[CurCID].IsKey(UID));
	double Delta = SeekJoin(UID, NewCID) + SeekLeave(UID, CurCID);
	//correct only for intersection between new com and current com
	TUNGraph::TNodeI NI = G->GetNI(UID);
	for (int e = 0; e < NI.GetDeg(); e++){
		const int VID = NI.GetNbhNId(e);
		if (! NIDComVH.GetDat(VID).IsKey(CurCID) || ! NIDComVH.GetDat(VID).IsKey(NewCID)) {continue;}
		TIntPr SrcDstNIDPr(TMath::Mn(UID,VID), TMath::Mx(UID,VID));
		TIntSet& JointCom = EdgeComVH.GetDat(SrcDstNIDPr);
		double CurPuv, NewPuvAfterJoin, NewPuvAfterLeave, NewPuvAfterSwitch, LambdaSum = SelectLambdaSum(JointCom);
		CurPuv = 1 - exp(- LambdaSum);
		NewPuvAfterLeave = 1 - exp(- LambdaSum + LambdaV[CurCID]);
		NewPuvAfterJoin = 1 - exp(- LambdaSum - LambdaV[NewCID]);
		NewPuvAfterSwitch = 1 - exp(- LambdaSum - LambdaV[NewCID] + LambdaV[CurCID]);
		if (JointCom.Len() == 1) {
			NewPuvAfterLeave = PNoCom;
		}
		Delta += (log(NewPuvAfterSwitch) + log(CurPuv) - log(NewPuvAfterLeave) - log(NewPuvAfterJoin));
		IAssert(!_isnan(Delta));
	}
	return Delta;
}

void TAGMFit::GetCmtyVV(TVec<TIntV>& CmtyVV, const double QMax){
	TFltV TmpQV;
	GetCmtyVV(CmtyVV, TmpQV, QMax);
}

void TAGMFit::GetCmtyVV(TVec<TIntV>& CmtyVV, TFltV& QV, const double QMax){
	CmtyVV.Gen(CIDNSetV.Len(), 0);
	QV.Gen(CIDNSetV.Len(), 0);
	TIntFltH CIDLambdaH(CIDNSetV.Len());
	for (int c = 0; c < CIDNSetV.Len(); c++) {
		CIDLambdaH.AddDat(c, LambdaV[c]);
	}
	CIDLambdaH.SortByDat(false);

	for (int c = 0; c < CIDNSetV.Len(); c++) {
		int CID = CIDLambdaH.GetKey(c);
		IAssert(LambdaV[CID] >= MinLambda);
		double Q = exp( - (double) LambdaV[CID]);
		if (Q > QMax) { continue; }
		TIntV CmtyV;
		CIDNSetV[CID].GetKeyV(CmtyV);
		if (CmtyV.Len() == 0) { continue; }
		if (CID == BaseCID) { //if the community is the base community(epsilon community), discard
			IAssert(CmtyV.Len() == G->GetNodes());
		} else {
			CmtyVV.Add(CmtyV);
			QV.Add(Q);
		}
	}
}

void TAGMFit::SetCmtyVV(const TVec<TIntV>& CmtyVV){
	CIDNSetV.Gen(CmtyVV.Len());
	for (int c = 0; c < CIDNSetV.Len(); c++) {
		CIDNSetV[c].AddKeyV(CmtyVV[c]);
		for (int j = 0; j < CmtyVV[c].Len(); j++) { IAssert(G->IsNode(CmtyVV[c][j])); } // check whether the member nodes exist in the graph
	}
	InitNodeData();
	SetDefaultPNoCom();
}

void TAGMFit::GradLogLForLambda(TFltV& GradV) {
	GradV.Gen(LambdaV.Len());
	TFltV SumEdgeProbsV(LambdaV.Len());
	for (int e = 0; e < EdgeComVH.Len(); e++) {
		TIntSet& JointCom = EdgeComVH[e];
		double LambdaSum = SelectLambdaSum(JointCom);
		double Puv = 1 - exp(- LambdaSum);
		if (JointCom.Len() == 0) {	Puv = PNoCom;	}
		for (TIntSet::TIter SI = JointCom.BegI(); SI < JointCom.EndI(); SI++) {
			SumEdgeProbsV[SI.GetKey()] += (1 - Puv) / Puv;
		}
	}
	for (int k = 0; k < LambdaV.Len(); k++) {
		int MaxEk = CIDNSetV[k].Len() * (CIDNSetV[k].Len() - 1) / 2;
		int NotEdgesInCom = MaxEk - ComEdgesV[k];
		GradV[k] = SumEdgeProbsV[k] - (double) NotEdgesInCom;
		if (LambdaV[k] > 0.0 && RegCoef > 0.0) { //if regularization exists
			GradV[k] -= RegCoef;
		}
	}
}

double TAGMFit::SelectLambdaSum(const TIntSet& ComK) { return SelectLambdaSum(LambdaV, ComK); }

double TAGMFit::SelectLambdaSum(const TFltV& NewLambdaV, const TIntSet& ComK) {
	double Result = 0.0;
	for (TIntSet::TIter SI = ComK.BegI(); SI < ComK.EndI(); SI++) {
		IAssert(NewLambdaV[SI.GetKey()] >= 0);
		Result += NewLambdaV[SI.GetKey()];
	}
	return Result;
}

void TAGMFit::PrintSummary() {
	int Coms = 0;
	for (int i = 0; i < LambdaV.Len(); i++) {
		if (LambdaV[i] <= 0.0001) { continue; }
		printf("P_c : %.3f Com Sz: %d, Total Edges inside: %d \n", 1.0 - exp(- LambdaV[i]), CIDNSetV[i].Len(), (int) ComEdgesV[i]);
		Coms++;
	}
	printf("%d Communities, Total Memberships = %d, Likelihood = %.2f, Epsilon = %f\n", Coms, NIDCIDPrS.Len(), Likelihood(), PNoCom.Val);
}
