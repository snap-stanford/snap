// motifcluster.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "motifcluster.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Motifs. build: %s, %s. Time: %s",
			 __TIME__, __DATE__, TExeTm::GetCurTm()));  
  TExeTm ExeTm;  
  Try
  const TStr graph_filename =
    Env.GetIfArgPrefixStr("-i:", "../as20graph.txt",
			  "Input directed graph file");
  const TStr motif =
    Env.GetIfArgPrefixStr("-m:", "M4", "Motif type");
  
  MotifType mt = MotifCluster::ParseMotifType(motif);
  PNGraph graph;
  if (graph_filename.GetFExt().GetLc() == ".ungraph") {
    TFIn FIn(graph_filename);
    graph = TSnap::ConvertGraph<PNGraph>(TUNGraph::Load(FIn), true);
  } else if (graph_filename.GetFExt().GetLc() == ".ngraph") {
    TFIn FIn(graph_filename);
    graph = TNGraph::Load(FIn);
  } else {
    graph = TSnap::LoadEdgeList<PNGraph>(graph_filename, 0, 1);
  }
  
  TSweepCut sc;
  MotifCluster::GetMotifCluster(graph, mt, sc);

  printf("Largest CC size: %d\n", sc.component.Len());
  printf("Cluster size: %d\n", sc.cluster.Len());
  printf("Motif conductance in largest CC: %f\n", sc.cond);
  printf("Eigenvalue: %f\n", sc.eig);
  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(),
	 TSecTm::GetCurTm().GetTmStr().CStr());  
  return 0;
}
