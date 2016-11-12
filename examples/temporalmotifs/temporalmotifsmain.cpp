// temporalmotifsmain.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "temporalmotifs.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Temporalmotifs. build: %s, %s. Time: %s",
			 __TIME__, __DATE__, TExeTm::GetCurTm()));  
  TExeTm ExeTm;  
  Try
  const TStr temporal_graph_filename =
    Env.GetIfArgPrefixStr("-i:", "example-temporal-graph.txt",
			  "Input directed temporal graph file");
  const TStr output = 
    Env.GetIfArgPrefixStr("-o:", "temporal-motif-counts.txt",
			  "Output file to write data");
  const TDbl delta =
    Env.GetIfArgPrefixDbl("-delta:", 4096, "Time window delta");

  // Count all {2,3}-node temporal motifs with 3 temporal edges
  TempMotifCounter tmc(graph_filename);
  Counter2D counts;
  tmc.Count3TEdge23Node(delta, counts);
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      printf("%d", counts(i, j));
      if (j < 5) { printf(" "); }
    }
    printf("%d\n");
  }
  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(),
	 TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
