#include "Snap.h"

#include <zygote.h>

int main(int argc, char* argv[]) {
  TPt <TUNGraph> UNGraph;

  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Generating Preferential Attachment Graph: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;

  Try

  const TInt NNodes = Env.GetIfArgPrefixInt("-n:", 10000, "Number of nodes");
  const TInt NodeOutDeg = Env.GetIfArgPrefixFlt("-o:", 100, "Node degree");

  //
  // generate a random PA graph with NNodes and NodeOutDeg average degree
  //
  UNGraph = TSnap::GenPrefAttach(NNodes, NodeOutDeg);

  Catch

  printf("\nGraph generated, run time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  zygote((char *) "zygote", &UNGraph, NULL);

  return 0;
}

int run(int objc, void* objv[], int argc, char* argv[]) {
  TPt <TUNGraph> UNGraph = *(TPt <TUNGraph> *) objv[0];

  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Finding Shortest Path: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  const TInt SrcNId = Env.GetIfArgPrefixInt("-s:", 0, "Source node id");
  const TInt DstNId = Env.GetIfArgPrefixInt("-d:", 10, "Destination node id");

	printf("Nodes %d\n", UNGraph->GetNodes());
  printf("Edges %d\n", UNGraph->GetEdges());

  //
  // compute the shortest path between SrcNId and DstNId
  //
  int Length = TSnap::GetShortPath(UNGraph, SrcNId, DstNId);
  printf("Shortest Path (%d,%d) is %d\n", SrcNId.Val, DstNId.Val, Length);

	return 0;
}

