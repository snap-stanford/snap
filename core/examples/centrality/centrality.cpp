#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Node Centrality. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input undirected graph");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "node_centrality.tab", "Output file");
  printf("Loading %s...", InFNm.CStr());
  PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);
  //PNGraph Graph = TSnap::GenRndGnm<PNGraph>(10, 10);
  // TGraphViz::Plot(Graph, gvlDot, InFNm+".gif", InFNm, true);
  printf("nodes:%d  edges:%d\n", Graph->GetNodes(), Graph->GetEdges());
  PUNGraph UGraph = TSnap::ConvertGraph<PUNGraph>(Graph); // undirected version of the graph
  TIntFltH BtwH, EigH, PRankH, CcfH;
  printf("Computing...\n");
  printf(" eigenvector...");                  TSnap::GetEigenVectorCentr(UGraph, EigH);
  printf(" pagerank (directed graph!)...");   TSnap::GetPageRank(Graph, PRankH, 0.85);
    printf(" clustering...");                 TSnap::GetNodeClustCf(UGraph, CcfH);
  printf(" betweenness (SLOW!)...");          TSnap::GetBetweennessCentr(UGraph, BtwH, 1.0);
  printf(" constraint (SLOW!)...");           TNetConstraint<PUNGraph> NetC(UGraph, true);
  printf("saving...");
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F,"#Network: %s\n", InFNm.CStr());
  fprintf(F,"#Nodes: %d\tEdges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  fprintf(F,"#NodeId\tDegree\tCloseness\tBetweennes\tEigenVector\tPageRank\tNetworkConstraint\tClustering\n");
  for (TUNGraph::TNodeI NI = UGraph->BegNI(); NI < UGraph->EndNI(); NI++) {
    const int NId = NI.GetId();
    const double DegCentr = TSnap::GetDegreeCentr(UGraph, NId);
    const double CloCentr = TSnap::GetClosenessCentr(UGraph, NId);
    const double BtwCentr = BtwH.GetDat(NId);
    const double EigCentr = EigH.GetDat(NId);
    const double PgrCentr = PRankH.GetDat(NId);
    const double Constraint = NetC.GetNodeC(NId);
    const double ClustCf = CcfH.GetDat(NId);
    fprintf(F, "%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", NId, DegCentr, CloCentr, BtwCentr, EigCentr, PgrCentr, Constraint, ClustCf);
  }
  fclose(F);
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
