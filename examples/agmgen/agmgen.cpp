// agmgen.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "agm.h"

int main(int argc, char* argv[]) {
	Env = TEnv(argc, argv, TNotify::StdNotify);
	Env.PrepArgs(TStr::Fmt("agmgen. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
	TExeTm ExeTm;
	Try
	const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "DEMO", "Community affiliation data");
	const TStr OutFPrx = Env.GetIfArgPrefixStr("-o:", "agm", "out file name prefix");
	const int RndSeed = Env.GetIfArgPrefixInt("-rs:",10,"Rnd Seed");
	const double DensityCoef= Env.GetIfArgPrefixFlt("-a:",0.6,"Power-law Coefficient a of density (density ~ N^(-a)");
	const double ScaleCoef= Env.GetIfArgPrefixFlt("-c:",1.3,"Scaling Coefficient c of density (density ~ c");

	TRnd Rnd(RndSeed);
	TVec<TIntV> CmtyVV;
	if(InFNm=="DEMO") {
		CmtyVV.Gen(2);
		TIntV NIdV;
		for(int i=0;i<25;i++) {
			TIntV& CmtyV = CmtyVV[0];
			CmtyV.Add(i+1);
		}
		for(int i=15;i<40;i++) {
			TIntV& CmtyV = CmtyVV[1];
			CmtyV.Add(i+1);
		}
	}
	else {
		TVec<TIntV> CmtyVV;
	  TSsParser Ss(InFNm, ssfWhiteSep);
	  while (Ss.Next()) {
			if(Ss.GetFlds()>0) {
				TIntV CmtyV;
				for(int i=0;i<Ss.GetFlds();i++) {
					if(Ss.IsInt(i)){CmtyV.Add(Ss.GetInt(i));}
				}
				CmtyVV.Add(CmtyV);
			}
	  }
		printf("community loading completed (%d communities)\n",CmtyVV.Len());
	}
	PUNGraph AG = TAGM::GenAGM(CmtyVV,DensityCoef,ScaleCoef,Rnd);
	TSnap::SaveEdgeList(AG,OutFPrx + ".edgelist.txt");
	if(AG->GetNodes()<50) {
		TAGM::GVizComGraph(AG,CmtyVV,OutFPrx + ".graph.gif");
	}
	Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
