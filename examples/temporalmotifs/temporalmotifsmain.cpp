// temporalmotifsmain.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "temporalmotifs.h"
#ifdef USE_OPENMP
#include <omp.h>
#endif

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
			  "Output file in which to write counts");
  const TFlt delta =
    Env.GetIfArgPrefixFlt("-delta:", 4096, "Time window delta");
  const int num_threads =
    Env.GetIfArgPrefixInt("-nt:", 4, "Number of threads for parallelization");

#ifdef USE_OPENMP
  omp_set_num_threads(num_threads);
#endif

  // Count all 2-node and 3-node temporal motifs with 3 temporal edges
  TempMotifCounter tmc(temporal_graph_filename);
  Counter2D counts;
  tmc.Count3TEdge23Node(delta, counts);
  FILE* output_file = fopen(output.CStr(), "wt");
  for (int i = 0; i < counts.m(); i++) {
    for (int j = 0; j < counts.n(); j++) {
      int count = counts(i, j);
      fprintf(output_file, "%d", count);
      if (j < counts.n() - 1) { fprintf(output_file, " "); }
    }
    fprintf(output_file, "\n");
  }
  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(),
	 TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
