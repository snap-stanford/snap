#include "stdafx.h"
#include "agm.h"

PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd){
	TFltV CProbV;
	double Prob;
	for(int i=0;i<CmtyVV.Len();i++) {
		Prob = ScaleCoef*pow(double(CmtyVV[i].Len()),-DensityCoef);
		if(Prob>1){Prob = 1;}
		CProbV.Add(Prob);
	}
	PUNGraph G = TUNGraph::New();
	printf("AGM begins\n");
	for(int i=0;i<CmtyVV.Len();i++) {
		TIntV& CmtyV = CmtyVV[i];
		for(int u=0;u<CmtyV.Len();u++) {
			G->AddNode(CmtyV[u]);
		}
		Prob = CProbV[i];
		printf("\r%d(%d)/%d",i,CmtyVV[i].Len(),CmtyVV.Len());
		RndConnectInsideCommunity(G,CmtyV,Prob,Rnd);
	}
	printf("AGM completed (%d nodes %d edges)\n",G->GetNodes(),G->GetEdges());
	return G;
}
void TAGM::RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd){
	int CNodes = CmtyV.Len();
	int CEdges = Rnd.GetBinomialDev(Prob,CNodes*(CNodes-1)/2);
	THashSet<TIntPr> NewEdgeSet(CEdges);
	for (int edge = 0; edge < CEdges; ) {
		int SrcNId = CmtyV[Rnd.GetUniDevInt(CNodes)];
		int DstNId = CmtyV[Rnd.GetUniDevInt(CNodes)];
		if(SrcNId>DstNId){Swap(SrcNId,DstNId);}
		if (SrcNId != DstNId && !NewEdgeSet.IsKey(TIntPr(SrcNId,DstNId))) { // is new edge
			NewEdgeSet.AddKey(TIntPr(SrcNId,DstNId));
			Graph->AddEdge(SrcNId,DstNId);
			edge++; 
		} 
	}
}
void TAGM::GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const THash<TInt,TIntV>& CmtyVH) {
	for(int i=0;i<CmtyVH.Len();i++){
		int CID = CmtyVH.GetKey(i);
		for(int j=0;j<CmtyVH[i].Len();j++) {
			int NID = CmtyVH[i][j];
			NIDComVH.AddDat(NID).Add(CID);
		}
	}
}

void TAGM::GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const TVec<TIntV>& CmtyVV) {
	THash<TInt,TIntV> CmtyVH;
	for(int i=0;i<CmtyVV.Len();i++) {
		CmtyVH.AddDat(i,CmtyVV[i]);
	}
	GetNodeMembership(NIDComVH,CmtyVH);
}
