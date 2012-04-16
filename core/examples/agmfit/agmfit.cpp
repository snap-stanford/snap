// agmfit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../snap/agm.h"

int main(int argc, char* argv[]) {
	Env = TEnv(argc, argv, TNotify::StdNotify);
	Env.PrepArgs(TStr::Fmt("cpm. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
	TExeTm ExeTm;
	Try
	const TStr OutFPrx  = Env.GetIfArgPrefixStr("-o:", "", "Output file name prefix");
	const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "football.names.edgelist", "Input edgelist file name. DEMO: AGM with 2 communities");
	const TInt RndSeed = Env.GetIfArgPrefixInt("-s:", 10, "Random seed for AGM");
	const TFlt Epsilon = Env.GetIfArgPrefixFlt("-e:", 0.0, "Edge probability between the nodes that do not share any community (default (0.0): set it to be 1 / N^2)");
	const TInt Coms = Env.GetIfArgPrefixInt("-c:", 0, "Number of communities (0: determine it by AGM)");

	PUNGraph G = TUNGraph::New();
	TVec<TIntV> CmtyVV;
	TIntStrH NIDNameH;

	if (InFNm == "DEMO") {
		TVec<TIntV> TrueCmtyVV;
		TRnd AGMRnd(RndSeed);

		//generate community bipartite affiliation
		const int ABegin = 0, AEnd = 70, BBegin = 30, BEnd = 100;
		TrueCmtyVV.Add(TIntV());
		TrueCmtyVV.Add(TIntV());
		for (int u = ABegin; u < AEnd; u++) {
			TrueCmtyVV[0].Add(u);
		}
		for (int u = BBegin; u < BEnd; u++) {
			TrueCmtyVV[1].Add(u);
		}
		G = TAGM::GenAGM(TrueCmtyVV, 0.0, 0.2, AGMRnd);
	}
	else {
		G = TAGMUtil::LoadEdgeListStr<PUNGraph>(InFNm, NIDNameH);
	}
	printf("Graph: %d Nodes %d Edges\n", G->GetNodes(), G->GetEdges());
	int NumComs = Coms;
	if (NumComs < 2) {
		int InitComs;
		if (G->GetNodes() > 1000) {
			InitComs = G->GetNodes() / 5;
			NumComs = TAGMUtil::FindComsByAGM(G, InitComs, 50 * G->GetNodes() * G->GetNodes(), RndSeed, 1.5, Epsilon, OutFPrx);
		} else {
			InitComs = G->GetNodes() / 5;
			NumComs = TAGMUtil::FindComsByAGM(G, InitComs, 50 * G->GetNodes() * G->GetNodes(), RndSeed, 1.2, Epsilon, OutFPrx);
		}
	}
	TAGMFit AGMFit(G, NumComs, RndSeed);
	if (Epsilon > 0) { AGMFit.SetPNoCom(Epsilon);	}
	AGMFit.RunMCMC(50 * G->GetNodes() * G->GetNodes(), 10);
	AGMFit.GetCmtyVV(CmtyVV, 0.9999);
	TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.txt", CmtyVV, NIDNameH);
	TAGMUtil::SaveGephi(OutFPrx + "graph.gexf", G, CmtyVV, 1.5, 1.5, NIDNameH);
	AGMFit.PrintSummary();

	Catch

  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}

