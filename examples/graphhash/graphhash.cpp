#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("graphhash. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try

  const int NNodes = 5;
  const int MinEdges = 3;
  const int MaxEdges = NNodes*(NNodes-1)/2;

  TGHash<TInt> GHash(false); // graph hash table

  // generate 100 random graphs on 5 nodes and at least 3 edges and add them to the hash table
  TInt::Rnd.PutSeed(1);
  for (int gn = 0; gn < 100; gn++) {
    PNGraph G = TSnap::GenRndGnm<PNGraph>(NNodes, MinEdges+TInt::Rnd.GetUniDevInt(MaxEdges-MinEdges));
    GHash.AddDat(G)++;
  }
  printf("%d graphs in the hash table\n", GHash.Len());
  
  // re-generate those same 100 random graphs on 5 nodes and at least 3 edges and 
  // make sure that exist in the hash table
  TInt::Rnd.PutSeed(1);
  for (int gn = 0; gn < 100; gn++) {
    PNGraph G = TSnap::GenRndGnm<PNGraph>(NNodes, MinEdges+TInt::Rnd.GetUniDevInt(MaxEdges-MinEdges));
    IAssert(GHash.IsKey(G));
  }
  printf("%d graphs in the hash table\n", GHash.Len());
  
  // generate another 10k random graphs and add them to the hash table
  for (int gn = 0; gn < 10000; gn++) {
    PNGraph G = TSnap::GenRndGnm<PNGraph>(NNodes, MinEdges+TInt::Rnd.GetUniDevInt(MaxEdges-MinEdges));
    GHash.AddDat(G)++;
  }
  printf("%d graphs in the hash table\n", GHash.Len());

  // draw top 10 most frequent graphs and show statistics for top 100.
  int PlotCnt = 0;
  TIntV KeyIdV;
  GHash.GetKeyIdByDat(KeyIdV, false);
  printf("Rank\tKeyId\tNodes\tEdges\tCount\n");
  for (int i = 0; i < TMath::Mn(100, KeyIdV.Len()); i++) {
    const TGraphKey& Key = GHash.GetKey(KeyIdV[i]);
    printf("%d\t%d\t%d\t%d\t%s\n", i+1, KeyIdV[i](), Key.GetNodes(), Key.GetEdges(), GHash.GetDatId(KeyIdV[i]).GetStr().CStr());
    if (PlotCnt++ < 10) {  // draw the graph
      GHash.DrawGViz(KeyIdV[i], TStr::Fmt("ghash%02d", PlotCnt), "gif", TStr::Fmt("Count: %d", GHash.GetDatId(KeyIdV[i]).Val));
    }
  }
  // plot graph frequency distributions
  TIntH EdgesCntH, EdgesFreqH, FreqCntH;
  for (int keyid = GHash.FFirstKeyId(); GHash.FNextKeyId(keyid); ) {
    FreqCntH.AddDat(GHash[keyid])++;
    EdgesFreqH.AddDat(GHash.GetKey(keyid).GetEdges()) += GHash[keyid];
    EdgesCntH.AddDat(GHash.GetKey(keyid).GetEdges())++;
  }
  TGnuPlot::PlotValCntH(FreqCntH, "ghash-FreqCnt", "Frequency-count plot.", "Frequency of a graph in the hash table", "Number of graphs with such frequency", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(EdgesFreqH, "ghash-EdgesFreq", "Edges-Frequency plot.", "Number of edges in the graph", "Total frequency of graphs on X-edges", gpsAuto, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(EdgesCntH, "ghash-EdgesCnt", "Edges-Count plot.", "Number of edges in the graph", "Total number of such graphs", gpsAuto, false, gpwLinesPoints, false, false);

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
