#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Node Centrality. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input undirected graph");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "node_centrality.tab", "Output file");

  printf("Loading %s...", InFNm.CStr());
  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm);
  //PUNGraph Graph = TSnap::GenRndGnm<PUNGraph>(500, 2000);
  //PUNGraph Graph = TUNGraph::GetSmallGraph();
  printf("nodes:%d  edges:%d\n", Graph->GetNodes(), Graph->GetEdges());
  TIntFltH BtwH, EigH, PRankH, CcfH;
  printf("Computing...\n");
  printf("  eigenvector...");        TSnap::GetEigenVectorCentr(Graph, EigH);
  printf(" pagerank...");            TSnap::GetPageRank(Graph, PRankH, 0.15);
  printf(" clustering...");          TSnap::GetNodeClustCf(Graph, CcfH);
  printf(" betweenness(SLOW!)...");  TSnap::GetBetweennessCentr(Graph, BtwH, 1.0);
  printf(" constraint(SLOW!)...");   TNetConstraint<PUNGraph> NetC(Graph, true);
  printf("saving...");
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F,"#Network: %s\n", InFNm.CStr());
  fprintf(F,"#Nodes: %d\tEdges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  fprintf(F,"#NodeId\tDegree\tCloseness\tBetweennes\tEigenVector\tPageRank\tNetworkConstraint\tClustering\n");
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int NId = NI.GetId();
    const double DegCentr = TSnap::GetDegreeCentr(Graph, NId);
    const double CloCentr = TSnap::GetClosenessCentr(Graph, NId);
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
