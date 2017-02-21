#include "stdafx.h"
#include "enumerate_subgraphs.h"
#include "subnetwork_isomorphism.h"
#include "subunit_counter.h"

typedef PNGraph Graph;
typedef TNodeEDatNet<TInt, TInt> TNet;
typedef TPt<TNet> Net;

void RemoveSelfLoops (PNGraph &G) {
  TVec<TNGraph::TEdgeI> SelfEdges;
  for (TNGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
    if (EI.GetSrcNId() == EI.GetDstNId()) {
      SelfEdges.Add(EI);
    }
  }

  for (int i = 0; i < SelfEdges.Len(); i++) {
    TNGraph::TEdgeI e = SelfEdges[i];
    G->DelEdge(e.GetSrcNId(), e.GetDstNId());
  }
}

void AddLabels (const Graph &G, Net &N) {
  TRnd Rnd;

  for (TNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    //N->AddNode(NI.GetId(), Rnd.GetUniDevInt(1, 5));
    N->AddNode(NI.GetId(), 1);
  }

  for (TNGraph::TEdgeI NI = G->BegEI(); NI < G->EndEI(); NI++) {
    //N->AddEdge(NI.GetSrcNId(), NI.GetDstNId(), Rnd.GetUniDevInt(1, 5));
    N->AddEdge(NI.GetSrcNId(), NI.GetDstNId(), 1);
  }
}

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Motifs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input directed graph file (single directed edge per line)");
  const bool DrawMotifs = Env.GetIfArgPrefixBool("-d:", true, "Draw motif shapes (requires GraphViz)");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  if (OutFNm.Empty()) { OutFNm = InFNm.GetFMid(); }

  // load graph
  Graph G;
  if (InFNm.GetFExt().GetLc()==".ungraph") {
  TFIn FIn(InFNm);  G=TSnap::ConvertGraph<PNGraph>(TUNGraph::Load(FIn), true); }
  else if (InFNm.GetFExt().GetLc()==".ngraph") {
  TFIn FIn(InFNm);  G=TNGraph::Load(FIn); }
  else {
  G = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1); }

  //RemoveSelfLoops(G);

  Net network = Net::New();
  AddLabels(G, network);

  const int SubnetworkSize = 3;
  typedef TSubnetworkIsomorphism<Net, SubnetworkSize> T3SubnetworkIso;

  T3SubnetworkIso SubnetworkIso;
  SubnetworkIso.Init(network, 0);

  typedef TSubunitCounter<Graph, T3SubnetworkIso> T3SubnetworkCounter;

  T3SubnetworkCounter SubnetworkCounter;
  SubnetworkCounter.Init(SubnetworkIso);

  typedef TEnumerateSubgraphs<Graph> TEnumSubgraphs;

  TEnumSubgraphs EnumSubnetworks;
  EnumSubnetworks.Init(G);

  TFltV Probabilites(SubnetworkSize);
  Probabilites[0] = 1.0;
  Probabilites[1] = 0.75;
  Probabilites[2] = 0.5;

  TRnd Rnd;

  EnumSubnetworks.Enumerate(SubnetworkSize, SubnetworkCounter);
  //EnumSubnetworks.Enumerate(SubnetworkSize, SubnetworkCounter, Probabilites, Rnd);

  SubnetworkCounter.ComputeIsomorphism();

  uint64 sum = 0;
  for (T3SubnetworkCounter::TSubunitIt it = SubnetworkCounter.BegSubunitIt(); it < SubnetworkCounter.EndSubunitIt(); it++) {
    sum += SubnetworkCounter.Count(it);
  }

  FILE *F = fopen(TStr::Fmt("%s-counts.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "SubnetworkId\tNodes\tEdges\tCount\tFrequency\n");

  int i = 0;
  for (T3SubnetworkCounter::TSubunitIt it = SubnetworkCounter.BegSubunitIt(); it < SubnetworkCounter.EndSubunitIt(); it++) {
    const T3SubnetworkCounter::TId &id = SubnetworkCounter.SubunitId(it);
    Net tmpN = SubnetworkIso.GetNetwork(id);
    uint64 count = SubnetworkCounter.Count(it);

    if (DrawMotifs) {
      //Draws only the nodes and edges, without the data on nodes or edges.
      TSnap::DrawGViz(tmpN, gvlNeato, TStr::Fmt("%s-motif%03d.gif", OutFNm.CStr(), i), 
                      TStr::Fmt("GId:%d  Freq:%f", i, count / (double)sum));
    }

    fprintf(F, "%d\t%d\t%d\t%llu\t%f\n", i, tmpN->GetNodes(), tmpN->GetEdges(), count, count / (double)sum);
    i++;
  }

  fclose(F);

  printf("done.");

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
