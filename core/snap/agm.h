#ifndef snaplib_agm_h
#define snaplib_agm_h
#include "./Snap.h"


class TAGM{
public:
	static void RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd);
	static PUNGraph GenAGM(const TIntV& NIdV , THash<TInt,TIntV >& CmtyVH, const TStr& AGMNm, const double& PiCoef, const double& ProbBase, TRnd& Rnd=TInt::Rnd);
	static PUNGraph GenAGM(TVec<TIntV >& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd=TInt::Rnd);
	static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const int TargetEdges, TRnd& Rnd);
	static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const TFltV& CProbV, TRnd& Rnd, const double PNoCom = -1.0);

};

class TAGMUtil{
public:
	static void GenPLSeq(TIntV& SzSeq,const int& SeqLen, const double& Alpha, TRnd& Rnd, const int& Min, const int& Max);
	static void ConnectCmtyVV(TVec<TIntV>& CmtyVV, const TIntPrV& CIDSzPrV, const TIntPrV& NIDMemPrV, TRnd& Rnd);
	static void GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const PUNGraph& Graph, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd);
	static void GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const TIntV& NIDV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd);
	static void GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd);
	static void RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd);
	static PUNGraph GenAGM(TVec<TIntV >& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd=TInt::Rnd);
	static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const int TargetEdges, TRnd& Rnd);
	static PUNGraph GenAGM(TVec<TIntV>& CmtyVV, const TFltV& CProbV, TRnd& Rnd, const double PNoCom = -1.0);
	static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntV>& CmtyVV);
	static void GetNodeMembership(TIntH& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
	static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntSet>& CmtyVV);
	static void GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
	static void GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
	static void GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const TVec<TIntV >& CmtyVV);
	static void GetNodeMembership(TIntH& NIDComVH, const TVec<TIntV >& CmtyVV);
	static void LoadCmtyVV(const TStr& InFNm, TVec<TIntV>& CmtyVV);
	static void DumpCmtyVV(const TStr& OutFNm, const TVec<TIntV>& CmtyVV);
	static void DumpCmtyVV(const TStr OutFNm, TVec<TIntV>& CmtyVV, TIntStrH& NIDNmH);
	static int TotalMemberships(const TVec<TIntV>& CmtyVV);
	static void RewireCmtyVV(const TVec<TIntV>& CmtyVVIn, TVec<TIntV>& CmtyVVOut, const TIntV& NIdV, TRnd& Rnd);
	static int Intersection(const TIntV& C1, const TIntV& C2);
	static void GetIntersection(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& C);
	static int Intersection(const THashSet<TInt>& A, const THashSet<TInt>& B);
	static int FindComsByAGM(const PUNGraph& Graph, const int InitComs, const int MaxIter, const int RndSeed, const double RegGap, const double PNoCom = 0.0, const TStr PltFPrx = TStr());
	static void SaveGephi(const TStr& OutFNm, const PUNGraph& G, const TVec<TIntV>& CmtyVVAtr, const double MaxSz, const double MinSz, const TIntStrH& NIDNameH = TIntStrH(), const THash<TInt, TIntTr >& NIDColorH = THash<TInt, TIntTr > ());
	// Separator separated file. One edge per line
	// SrcColId and DstColId are column indexes of source/destination INTEGER node id
	template <class PGraph>
	static PGraph LoadEdgeListStr(const TStr& InFNm, TIntStrH& NIDNameH, const int& SrcColId = 0, const int& DstColId = 1, const TSsFmt SsFmt = ssfTabSep) {
		TSsParser Ss(InFNm, SsFmt);
		PGraph Graph = PGraph::TObj::New();
		TStrHash<TInt> StrSet(Mega(1), true);
		while (Ss.Next()) {
			const int SrcNId = StrSet.AddKey(Ss[SrcColId]);
			const int DstNId = StrSet.AddKey(Ss[DstColId]);
			if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
			if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
			Graph->AddEdge(SrcNId, DstNId);
		}
		NIDNameH.Gen(StrSet.Len());
		for (int s = 0; s < StrSet.Len(); s++) { NIDNameH.AddDat(s, StrSet.GetKey(s)); }
		IAssert(NIDNameH.Len() == Graph->GetNodes());

		Graph->Defrag();
		return Graph;
	}

	template<class PGraph>
	static void GVizComGraph(const PGraph& Graph,const TVec<TIntV >& CmtyVV, const TStr& OutFNm, const TStr& Desc = TStr()){
		TStrV Colors = TStrV::GetV("red","blue","green","pink","cyan");
		TStrV Shapes = TStrV::GetV("ellipse","triangle","square","pentagon","hexagon");
		THash<TInt,TIntV> NIDComVH;
		GetNodeMembership(NIDComVH,CmtyVV);

	  const TStr Ext = OutFNm.GetFExt();
	  const TStr GraphFNm = OutFNm.GetSubStr(0, OutFNm.Len() - Ext.Len()) + "dot";
		const bool IsDir = HasGraphFlag(typename PGraph::TObj, gfDirected);
		FILE *F = fopen(GraphFNm.CStr(), "wt");
		if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
		if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
		fprintf(F, "  graph [splines=false overlap=false]\n"); //size=\"12,10\" ratio=fill
		fprintf(F, "  node  [width=0.3, height=0.3]\n");
		//nodes
		for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
			int NID = NI.GetId();
			TIntV& CIDV = NIDComVH.GetDat(NID);
			IAssert(CIDV.Len()>0);
			TStr ShapeNm = Shapes[(CIDV.Len()-1) % Shapes.Len()];
			TStr ColorNm = Colors[CIDV[0] % Colors.Len()];
			TStr NodeComLabel = TStr::Fmt("%d(",NID);
			for(int i=0;i<CIDV.Len();i++) {
				TStr TmpStr = TStr::Fmt("%d",int(CIDV[i]));
				NodeComLabel += TmpStr;
				if(i<CIDV.Len()-1){NodeComLabel+=",";}
			}
			NodeComLabel += ")";
			fprintf(F, "  %d [style=filled, shape=\"%s\" fillcolor=\"%s\" label=\"%s\"];\n", NI.GetId(), ShapeNm.CStr(),ColorNm.CStr(), NodeComLabel.CStr()); 
		}
	
		// edges
		for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
			if (NI.GetOutDeg()==0 && NI.GetInDeg()==0  ) { 
				fprintf(F, "%d;\n", NI.GetId()); }
			else {
				for (int e = 0; e < NI.GetOutDeg(); e++) {
					if (! IsDir && NI.GetId() > NI.GetOutNId(e)) { continue; }
					fprintf(F, "  %d %s %d;\n", NI.GetId(), IsDir?"->":"--", NI.GetOutNId(e)); 
				}
			}
		}
		if (! Desc.Empty()) {
			fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
			fprintf(F, "  fontsize=24;\n");
		}
		fprintf(F, "}\n");
		fclose(F);
		TGraphViz::DoLayout(GraphFNm, OutFNm, gvlNeato);
	}
};


/////////////////////////////////////////////////////////////////////////////////////
//Fitting AGM
class TAGMFit{
private:
	PUNGraph G; //graph to fit
	TVec<TIntSet> CIDNSetV; //Community ID -> Member Node ID Sets
	THash<TIntPr,TIntSet> EdgeComVH; // Edge -> Shared Community ID Set
	THash<TInt, TIntSet> NIDComVH; // Node ID -> Communitie IDs the node belongs to
	TIntV ComEdgesV; // The number of edges in each community
	TFlt PNoCom; //probability of edge when two nodes share no community (epsilon in the paper)
	TFltV LambdaV; // Parametrization of P_c (edge probability in community c), P_c = 1 - exp(-lambda)
	TRnd Rnd;
	THash<TIntPr,TFlt> NIDCIDPrH; //
	THash<TIntPr,TInt> NIDCIDPrS;

public:
	TFlt MinLambda; // Minimum value of lambda (default = 1e-5)
	TFlt MaxLambda; // Maximum value of lambda (default = 10)
	TFlt RegCoef; //Regularization coefficient when we fit for P_c (for finding # communities)
	TInt BaseCID; // ID of the epsilon community (in case we fit P_c of the epsilon community). We do not fit for the epsilon community in general.
	TAGMFit(){}
	~TAGMFit(){}
	TAGMFit(const PUNGraph& GraphPt, const TVec<TIntV>& CmtyVVPt, const int RndSeed = 0): Rnd(RndSeed), PNoCom(0.0), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), G(GraphPt), BaseCID(-1) { SetCmtyVV(CmtyVVPt);  }
	TAGMFit(const PUNGraph& GraphPt, const int InitComs, const int RndSeed = 0): Rnd(RndSeed), PNoCom(0.0), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), G(GraphPt), BaseCID(-1) { RandomInitCmtyVV(InitComs);  }
	TAGMFit(const PUNGraph& GraphPt, const TVec<TIntV>& CmtyVVPt, const TRnd& RndPt): Rnd(RndPt), PNoCom(0.0), MinLambda(0.00001), MaxLambda(10.0), RegCoef(0), G(GraphPt), BaseCID(-1) { SetCmtyVV(CmtyVVPt); }
	void RandomInitCmtyVV(const int InitComs, const double ComSzAlpha = 1.3, const double MemAlpha = 1.8, const int MinComSz = 8, const int MaxComSz = 200, const int MinMem = 1, const int MaxMem = 10);
	void AddBaseCmty(); // Include the epsilon community to fitting
	double Likelihood();
	double Likelihood(const TFltV& NewLambdaV) { double Tmp1, Tmp2; return Likelihood(NewLambdaV, Tmp1, Tmp2); }
	double Likelihood(const TFltV& NewLambdaV, double& LEdges, double& LNoEdges);
	void GetEdgeJointCom();
	void GradLogLForLambda(TFltV& GradV);
	void PrintSummary();
	int MLEGradAscentGivenCAG(const double& Thres=0.001, const int& MaxIter=10000, const TStr PlotNm = TStr());
	void SetDefaultPNoCom();
	void SetPNoCom(const double& Input) { if (BaseCID == -1) { PNoCom = Input; } }// only if we do not use eps-community
	double GetPNoCom() { return PNoCom; }
	void GetNewtonStep(TFltVV& HVV, TFltV& GradV, TFltV& DeltaLV);
	double SelectLambdaSum(const TIntSet& ComK);
	double SelectLambdaSum(const TFltV& NewLambdaV, const TIntSet& ComK);
	void RandomInit(const int& MaxK);
	void RunMCMC(const int& MaxIter, const int& EvalLambdaIter, const TStr& PlotFPrx = TStr());
	void SampleTransition(int& NID, int& JoinCID, int& LeaveCID, double& DeltaL);
	void InitNodeData();
	void LeaveCom(const int& NID, const int& CID);
	void JoinCom(const int& NID, const int& JoinCID);
	int RemoveEmptyCom();
	double GetStepSizeByLineSearchForLambda(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta);
	void SetLambdaV(const TFltV& LambdaPt) {LambdaV = LambdaPt;}
	void GetLambdaV(TFltV& OutV) {OutV = LambdaV;}
	void GetQV(TFltV& OutV);
	void GetCmtyVV(TVec<TIntV>& CmtyVV, const double QMax = 2.0);
	void GetCmtyVV(TVec<TIntV>& CmtyVV, TFltV& QV, const double QMax = 2.0);
	void SetCmtyVV(const TVec<TIntV>& CmtyVV);
	double SeekLeave(const int& UID, const int& CID);
	double SeekJoin(const int& UID, const int& CID);
	double SeekSwitch(const int& UID, const int& CurCID, const int& NewCID);
	void Save(TSOut& SOut) {
		G->Save(SOut);
		CIDNSetV.Save(SOut);
		EdgeComVH.Save(SOut);
		NIDComVH.Save(SOut);
		ComEdgesV.Save(SOut);
		PNoCom.Save(SOut);
		LambdaV.Save(SOut);
		NIDCIDPrH.Save(SOut);
		NIDCIDPrS.Save(SOut);
		MinLambda.Save(SOut);
		MaxLambda.Save(SOut);
		RegCoef.Save(SOut);
		BaseCID.Save(SOut);
	}
	void Load(TSIn& SIn, const int& RndSeed = 0) {
		G->Load(SIn);
		CIDNSetV.Load(SIn);
		EdgeComVH.Load(SIn);
		NIDComVH.Load(SIn);
		ComEdgesV.Load(SIn);
		PNoCom.Load(SIn);
		LambdaV.Load(SIn);
		NIDCIDPrH.Load(SIn);
		NIDCIDPrS.Load(SIn);
		MinLambda.Load(SIn);
		MaxLambda.Load(SIn);
		RegCoef.Load(SIn);
		BaseCID.Load(SIn);
		Rnd.PutSeed(RndSeed);
	}
};


#endif
