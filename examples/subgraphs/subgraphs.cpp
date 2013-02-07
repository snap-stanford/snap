#include "stdafx.h"
#include "enumerate_subgraphs.h"
#include "subgraph_isomorphism.h"
#include "subunit_counter.h"

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

int main (int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Motifs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try

  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input directed graph file (single directed edge per line)");
  const bool DrawMotifs = Env.GetIfArgPrefixBool("-d:", true, "Draw motif shapes (requires GraphViz)");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  if (OutFNm.Empty()) { OutFNm = InFNm.GetFMid(); }

  typedef PNGraph Graph;

  // load graph
  Graph G;
  if (InFNm.GetFExt().GetLc()==".ungraph") {
  TFIn FIn(InFNm);  G=TSnap::ConvertGraph<PNGraph>(TUNGraph::Load(FIn), true); }
  else if (InFNm.GetFExt().GetLc()==".ngraph") {
  TFIn FIn(InFNm);  G=TNGraph::Load(FIn); }
  else {
  G = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1); }

  //RemoveSelfLoops(G);

  const int SubgraphSize = 3;
  typedef TSubgraphIsomorphism<Graph, SubgraphSize> T3SubgraphIso;

  T3SubgraphIso SubgraphIso;
  SubgraphIso.Init(G);

  typedef TSubunitCounter<Graph, T3SubgraphIso> T3SubgraphCounter;

  T3SubgraphCounter SubgraphCounter;
  SubgraphCounter.Init(SubgraphIso);

  typedef TEnumerateSubgraphs<Graph> TEnumSubgraphs;

  TEnumSubgraphs EnumSubgraphs;
  EnumSubgraphs.Init(G);

  TFltV Probabilites(SubgraphSize);
  Probabilites[0] = 1.0;
  Probabilites[1] = 0.75;
  Probabilites[2] = 0.5;

  TRnd Rnd;

  EnumSubgraphs.Enumerate(SubgraphSize, SubgraphCounter);
  //EnumSubgraphs.Enumerate(SubgraphSize, SubgraphCounter, Probabilites, Rnd);

  SubgraphCounter.ComputeIsomorphism();

  uint64 sum = 0;
  for (T3SubgraphCounter::TSubunitIt it = SubgraphCounter.BegSubunitIt(); it < SubgraphCounter.EndSubunitIt(); it++) {
    sum += SubgraphCounter.Count(it);
  }

  FILE *F = fopen(TStr::Fmt("%s-counts.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "SubgraphId\tNodes\tEdges\tCount\tFrequency\n");

  int i = 0;
  for (T3SubgraphCounter::TSubunitIt it = SubgraphCounter.BegSubunitIt(); it < SubgraphCounter.EndSubunitIt(); it++) {
    T3SubgraphCounter::TId id = SubgraphCounter.SubunitId(it);
    Graph tmpG = SubgraphIso.GetGraph(id);
    uint64 count = SubgraphCounter.Count(it);

    if (DrawMotifs) {
      TSnap::DrawGViz(tmpG, gvlNeato, TStr::Fmt("%s-motif%03d.gif", OutFNm.CStr(), i),
                      TStr::Fmt("GId:%llu  Freq:%f", id.Val, count / (double)sum));
    }

    fprintf(F, "%llu\t%d\t%d\t%llu\t%f\n", id.Val, tmpG->GetNodes(), tmpG->GetEdges(), count, count / (double)sum);
    i++;
  }

  fclose(F);

  printf("done.");

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
