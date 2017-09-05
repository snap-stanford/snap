// localmotifclustermain.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "localmotifcluster.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Local motif clustering. build: %s, %s. Time: %s",
       __TIME__, __DATE__, TExeTm::GetCurTm()));  
  TExeTm ExeTm;  
  Try

  const bool IsDirected = 
    Env.GetIfArgPrefixBool("-d:", false, "Directed graph?");

  ProcessedGraph graph_p;
  if (IsDirected) {
    const TStr graph_filename =
      Env.GetIfArgPrefixStr("-i:", "C-elegans-frontal.txt", "Input graph file");
    const TStr motif =
      Env.GetIfArgPrefixStr("-m:", "triad", "Motif type");
    MotifType mt = ParseMotifType(motif, IsDirected);
    PNGraph graph;
    if (graph_filename.GetFExt().GetLc() == ".ngraph") {
      TFIn FIn(graph_filename);
      graph = TNGraph::Load(FIn);
    } else if (graph_filename.GetFExt().GetLc() == ".ungraph") {
      TExcept::Throw("Warning: input graph is an undirected graph!!");
    } else {
      graph = TSnap::LoadEdgeList<PNGraph>(graph_filename, 0, 1);
    }
    TSnap::DelSelfEdges(graph);
    graph_p = ProcessedGraph(graph, mt);

  } else {

    const TStr graph_filename =
      Env.GetIfArgPrefixStr("-i:", "C-elegans-frontal.txt", "Input graph file");
    const TStr motif =
      Env.GetIfArgPrefixStr("-m:", "clique3", "Motif type");
    MotifType mt = ParseMotifType(motif, IsDirected);
    PUNGraph graph;
    if (graph_filename.GetFExt().GetLc() == ".ungraph") {
      TFIn FIn(graph_filename);
      graph = TUNGraph::Load(FIn);
    } else if (graph_filename.GetFExt().GetLc() == ".ngraph") {
      TExcept::Throw("Warning: input graph is a directed graph!!");
    } else {
      graph = TSnap::LoadEdgeList<PUNGraph>(graph_filename, 0, 1);
    }
    TSnap::DelSelfEdges(graph);
    graph_p = ProcessedGraph(graph, mt);
  }

  const TInt seed =
    Env.GetIfArgPrefixInt("-s:", 1, "Seed");
  const TFlt alpha =
    Env.GetIfArgPrefixFlt("-a:", 0.98, "alpha");
  const TFlt eps =
    Env.GetIfArgPrefixFlt("-e:", 0.0001, "eps");


  MAPPR mappr;
  mappr.computeAPPR(graph_p, seed, alpha, eps / graph_p.getTotalVolume() * graph_p.getTransformedGraph()->GetNodes());
  mappr.sweepAPPR(-1);
  mappr.printProfile();
  printf("Size of Cluster: %d.\n", mappr.getCluster().Len());



  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(),
   TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
