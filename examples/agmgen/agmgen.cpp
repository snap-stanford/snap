#include "stdafx.h"
#include "agm.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("agmgen. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "DEMO", "Community affiliation data");
  const TStr OutFPrx = Env.GetIfArgPrefixStr("-o:", "agm", "out file name prefix");
  const int RndSeed = Env.GetIfArgPrefixInt("-rs:", 10, "Seed for random number generation");
  const double DensityCoef= Env.GetIfArgPrefixFlt("-a:", 0.6, "Power-law Coefficient a of density (density ~ N^(-a)");
  const double ScaleCoef= Env.GetIfArgPrefixFlt("-c:", 1.3, "Scaling Coefficient c of density (density ~ c");
  const bool Draw = Env.GetIfArgPrefixBool("-d:", false, "Use GraphViz to draw the generated graph.");

  TRnd Rnd(RndSeed);
  TVec<TIntV> CmtyVV;
  if(InFNm == "DEMO") {
    CmtyVV.Gen(2);
    TIntV NIdV;
    for(int i = 0; i < 25; i++) {
      TIntV& CmtyV = CmtyVV[0];
      CmtyV.Add(i + 1);
    }
    for(int i = 15; i < 40; i++) {
      TIntV& CmtyV = CmtyVV[1];
      CmtyV.Add(i+1);
    }
    printf("\nGenerate demo graph on 2 non-overlapping communities.\n");
  }
  else {
    int Membs = 0;
    TSsParser Ss(InFNm, ssfWhiteSep);
    while (Ss.Next()) {
      if(Ss.GetFlds() > 0) {
        TIntV CmtyV;
        for(int i = 0; i < Ss.GetFlds(); i++) {
          if (Ss.IsInt(i)) { CmtyV.Add(Ss.GetInt(i)); }
        }
        CmtyVV.Add(CmtyV);
        Membs += CmtyV.Len();
      }
    }
    printf("\nCommunity loading completed (%d communities, %d memberships)\n", CmtyVV.Len(), Membs);
  }
  PUNGraph AG = TAGM::GenAGM(CmtyVV, DensityCoef, ScaleCoef, Rnd);
  TSnap::SaveEdgeList(AG, OutFPrx + ".txt");
  if (Draw && AG->GetNodes() < 50) {
    TAGMUtil::GVizComGraph(AG, CmtyVV, OutFPrx + ".graph.gif");
  }
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
