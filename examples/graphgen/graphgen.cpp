#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Graph generators. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "output.txt", "Output graph filename");
  const TStr Plot = Env.GetIfArgPrefixStr("-g:", "e", "Which generator to use:"
    "\n\tf: Complete graph. Required parameters: n (number of nodes)"
    "\n\ts: Star graph. Required parameters: n (number of nodes)"
    "\n\t2: 2D Grid. Required parameters: n (number of rows), m (number of columns)"
    "\n\te: Erdos-Renyi (G_nm). Required parameters: n (number of nodes), m (number of edges)"
    "\n\tk: Random k-regular graph. Required parameters: n (number of nodes), k (degree of every node)"
    "\n\tb: Albert-Barabasi Preferential Attachment. Required parameters: n (number of nodes), k (edges created by each new node)"
    "\n\tp: Random Power-Law graph. Required parameters: n (number of nodes), p (power-law degree exponent)"
    "\n\tc: Copying model by Kleinberg et al. Required parameters: n (number of nodes), p (copying probability Beta)"
    "\n\tw: Small-world model. Required parameters: n (number of nodes), k (each node is connected to k nearest neighbors in ring topology), p (rewiring probability)\n"
    );
  const int N = Env.GetIfArgPrefixInt("-n:", 1000, "Number of nodes");
  const int M = Env.GetIfArgPrefixInt("-m:", 5000, "Number of edges");
  const double P = Env.GetIfArgPrefixFlt("-p:", 0.1, "Probability/Degree-exponent");
  const int K = Env.GetIfArgPrefixInt("-k:", 3, "Degree");

  if (Env.IsEndOfRun()) { return 0; }
  TExeTm ExeTm;
  TInt::Rnd.PutSeed(0); // initialize random seed
  printf("Generating...\n");
  PUNGraph G;
  TStr DescStr;
  if (Plot == "f") {
    G = TSnap::GenFull<PUNGraph>(N);
    DescStr = TStr::Fmt("Undirected complete graph.");
  } else
  if (Plot == "s") {
    G = TSnap::GenStar<PUNGraph>(N, false);
    DescStr = TStr::Fmt("Undirected star graph (1 center node connected to all other nodes).");
  } else
  if (Plot == "2") {
    G = TSnap::GenGrid<PUNGraph>(N, M, false);
    DescStr = TStr::Fmt("Undirected 2D grid of %d rows and %d columns.", N, M);
  } else
  if (Plot == "e") {
    G = TSnap::GenRndGnm<PUNGraph>(N, M, false);
    DescStr = TStr::Fmt("Undirected Erdos-Renyi random graph.");
  } else
  if (Plot == "k") {
    G = TSnap::GenRndDegK(N, K);
    DescStr = TStr::Fmt("Undirected k-regular random graph (every node has degree K).");
  } else
  if (Plot == "b") {
    G = TSnap::GenPrefAttach(N, K);
    DescStr = TStr::Fmt("Undirected Albert-Barabasi Preferential Attachment graph (each new node creades k preferentially attached edges).");
  } else
  if (Plot == "p") {
    G = TSnap::GenRndPowerLaw(N, P, true);
    DescStr = TStr::Fmt("Random Graph with Power-Law degree distribution with exponent P.");
  } else
  if (Plot == "c") {
    G = TSnap::ConvertGraph<PUNGraph>(TSnap::GenCopyModel(N, P));
    DescStr = TStr::Fmt("Copying model by Kleinberg et al. Node u comes, selects a random v, and with prob P it links to v, with 1-P links u links to neighbor of v. Power-law degree slope is 1/(1-P).");
  } else
  if (Plot == "w") {
    G = TSnap::GenSmallWorld(N, K, P);
    DescStr = TStr::Fmt("Watts-Strogatz Small-world model. Every node links to K other nodes.");
  }
  printf("done.\n");
  TSnap::SaveEdgeList(G, OutFNm, DescStr);

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
