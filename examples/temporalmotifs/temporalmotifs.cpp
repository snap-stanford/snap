// temporalmotifs.cpp : Defines the entry point for the console application.
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

  const TInt delta =
    Env.GetIfArgPrefixStr("-d:", 3600, "Time window delta");

  TempMotifCounter tmc(graph_filename);
  // Counts of all {2,3}-node, 3-edge motifs.
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
