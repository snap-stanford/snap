#include <../snap/Snap.h>
#include "agmfit.h"
#include "gencom.h"
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
