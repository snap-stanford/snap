#include "Snap.h"

#define EPSILON 0.0001
#define DIRNAME "bfsdfs"

using namespace TSnap;

// Demos BFS functions on full graph
template <class PGraph>
void DemoFullBfsDfs() {
  
  const int NNodes = 500;
  
  PGraph G = GenFull<PGraph>(NNodes);
  PNGraph GOut;
  int TreeSz, TreeDepth;
  
  // Get BFS tree from first node without following links (demos different options)
  GOut = GetBfsTree(G, 1, false, false);
  GetSubTreeSz(G, 1, false, false, TreeSz, TreeDepth);
  printf("FollowOut=false, FollowIn=false, GOut->GetNodes() == %d, GOut->GetEdges() = %d\n", 
        GOut->GetNodes(), G->GetEdges());
  printf("TreeSz == %d, TreeDepth = %d\n", TreeSz, TreeDepth);
  
  GOut = GetBfsTree(G, NNodes-1, true, true);
  GetSubTreeSz(G, 1, true, true, TreeSz, TreeDepth);
  printf("FollowOut=true, FollowIn=true, GOut->GetNodes() == %d, GOut->GetEdges() = %d\n", 
        GOut->GetNodes(), G->GetEdges());
  printf("TreeSz == %d, TreeDepth = %d\n", TreeSz, TreeDepth);
  
  GOut = GetBfsTree(G, NNodes/2, true, false);
  GetSubTreeSz(G, 1, true, false, TreeSz, TreeDepth);
  printf("FollowOut=true, FollowIn=false, GOut->GetNodes() == %d, GOut->GetEdges() = %d\n", 
        GOut->GetNodes(), G->GetEdges());
  printf("TreeSz == %d, TreeDepth = %d\n", TreeSz, TreeDepth);
  
  GOut = GetBfsTree(G, 1, false, true);
  GetSubTreeSz(G, 1, false, true, TreeSz, TreeDepth);
  printf("FollowOut=false, FollowIn=true, GOut->GetNodes() == %d, GOut->GetEdges() = %d\n", 
        GOut->GetNodes(), G->GetEdges());
  printf("TreeSz == %d, TreeDepth = %d\n", TreeSz, TreeDepth);
  
  TIntV NIdV;
  int StartNId, Hop, Nodes;
  
  StartNId = 1;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, HasGraphFlag(typename PGraph::TObj, gfDirected));
  printf("StartNId = %d, Nodes = %d, GetNodesAtHop NIdV.Len() = %d\n", StartNId, Nodes, NIdV.Len());
  
  TIntPrV HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, HasGraphFlag(typename PGraph::TObj, gfDirected));
  printf("StartNId = %d, Nodes = %d, GetNodesAtHops HopCntV.Len() = %d\n", StartNId, Nodes, HopCntV.Len());
  
  int Length, SrcNId, DstNId;
  SrcNId = 1;
  DstNId = NNodes-1;
  
  Length = GetShortPath(G, SrcNId, DstNId, HasGraphFlag(typename PGraph::TObj, gfDirected));
  printf("SPL Length = %d\n", Length);
  
  TIntH NIdToDistH;
  int MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, HasGraphFlag(typename PGraph::TObj, gfDirected), MaxDist);
//  for (int i = 0; i < min(5,NIdToDistH.Len()); i++) {
//    printf("NIdToDistH[%d] = %d\n", i, NIdToDistH[i].Val);
//  }
  
  int FullDiam;
  double EffDiam, AvgDiam;
  int NTestNodes = 10;
  
  for (int IsDir = 0; IsDir < 2; IsDir++) {
    printf("IsDir = %d:\n", IsDir);
    
    FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
    printf("FullDiam = %d\n", FullDiam);
    
    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir);
    printf("EffDiam = %.3f\n", EffDiam);
    
    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir, EffDiam, FullDiam);
    printf("EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
    
    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir, EffDiam, FullDiam, AvgDiam);
    printf("EffDiam = %.3f, FullDiam = %d, AvgDiam = %.3f\n", EffDiam, FullDiam, AvgDiam);
    
    TIntV SubGraphNIdV;
    for (int i = 0; i < NTestNodes; i++) {
      SubGraphNIdV.Add(G->GetRndNId());
    }
//    for (int i = 0; i < SubGraphNIdV.Len(); i++) {
//      printf("SubGraphNIdV[%d] = %d\n", i, SubGraphNIdV[i].Val);
//    }
    
    EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
    printf("For subgraph: EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
    
  }
  
}

// Demos BFS functions on undirected graph that is not fully connected
void DemoBFSUndirectedRandom() {
  
  PUNGraph G;
  
  TStr FName = TStr::Fmt("%s/sample_bfsdfs_unpower.txt", DIRNAME);
  
  const int NNodes = 50;
  G = GenRndPowerLaw(NNodes, 2.5);
  
  // Can save/here
//  SaveEdgeList(G, FName);
  
//  G = LoadEdgeList<PUNGraph>(FName);
  TIntStrH NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlNeato, TStr::Fmt("%s/sample_bfsdfs_unpower.png", DIRNAME), "Sample bfsdfs Graph", NodeLabelH);
  
  TIntV NIdV;
  int StartNId, Hop, Nodes;
  
  int IsDir = 0;
  printf("IsDir = %d:\n", IsDir);
  
  StartNId = 1;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, IsDir);
  printf("StartNId = %d, Nodes = %d, GetNodesAtHop NIdV.Len() = %d, NIdV[0] = %d\n", StartNId, Nodes, NIdV.Len(), NIdV[0].Val);
  
  TIntPrV HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, IsDir);
  printf("StartNId = %d, Nodes = %d, GetNodesAtHops HopCntV.Len() = %d\n", StartNId , Nodes, HopCntV.Len());
//  for (int N = 0; N < HopCntV.Len(); N++) {
//    printf("HopCntV[%d] = (%d, %d)\n", N, HopCntV[N].Val1.Val, HopCntV[N].Val2.Val);
//  }
  
  
  int Length, SrcNId, DstNId;
  SrcNId = 1;
  DstNId = G->GetNodes() - 1;
  
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  printf("%d -> %d: SPL Length = %d\n", SrcNId, DstNId, Length);
  
  SrcNId = 1;
  DstNId = 33;
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  printf("%d -> %d: SPL Length = %d\n", SrcNId, DstNId, Length);
  
  TIntH NIdToDistH;
  int MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, IsDir, MaxDist);
//  for (int i = 0; i < min(5,NIdToDistH.Len()); i++) {
//    printf("NIdToDistH[%d] = %d\n", i, NIdToDistH[i].Val);
//  }
  
  TInt::Rnd.PutSeed(0);
  
  int FullDiam;
  double EffDiam, AvgSPL;
  int NTestNodes = G->GetNodes() / 3 * 2;
  
  FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
  printf("FullDiam = %d\n", FullDiam);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir);
  printf("EffDiam = %.3f\n", EffDiam);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam);
  printf("EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam, AvgSPL);
  printf("EffDiam = %.3f, FullDiam = %d, AvgDiam = %.3f\n", EffDiam, FullDiam, AvgSPL);
  
  TIntV SubGraphNIdV;
  SubGraphNIdV.Add(0);
  SubGraphNIdV.Add(4);
  SubGraphNIdV.Add(31);
  SubGraphNIdV.Add(45);
  SubGraphNIdV.Add(18);
  SubGraphNIdV.Add(11);
  SubGraphNIdV.Add(11);
  SubGraphNIdV.Add(48);
  SubGraphNIdV.Add(34);
  SubGraphNIdV.Add(30);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
  printf("For subgraph: EffDiam = %.4f, FullDiam = %d\n", EffDiam, FullDiam);
 
}

// Demos BFS functions on directed graph that is not fully connected
void DemoBFSDirectedRandom() {

  
  PNGraph G = TNGraph::New();
  
  TStr FName = TStr::Fmt("%s/sample_bfsdfs_ngraph.txt", DIRNAME);
  
  // Create benchmark graph, initially visually to confirm values are correct
  const int NNodes = 30;
  G = GenRndGnm<PNGraph>(NNodes, NNodes*2);
  // Add some more random edges
  for (int i = 0; i < 10; i++) {
    TInt Src, Dst;
    do {
      Src = G->GetRndNId();
      Dst = G->GetRndNId();
    }
    while (Src == Dst || G->IsEdge(Src, Dst));
    G->AddEdge(Src, Dst);
  }
  // Add isolated component
  G->AddNode(NNodes);
  G->AddNode(NNodes+1);
  G->AddNode(NNodes+2);
  G->AddEdge(NNodes, NNodes+1);
  G->AddEdge(NNodes+1, NNodes+2);
  G->AddEdge(NNodes+2, NNodes+1);
  printf("G->GetNodes() = %d, G->GetEdges() = %d\n", G->GetNodes(), G->GetEdges());
  
  
  //  SaveEdgeList(G, FName);
  
  //  G = LoadEdgeList<PNGraph>(FName);
  TIntStrH NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlDot, TStr::Fmt("%s/sample_bfsdfs_ngraph.png", DIRNAME), "Sample BFS Graph", NodeLabelH);
  
  printf("G->GetNodes() = %d, G->GetEdges() = %d\n", G->GetNodes(), G->GetEdges());
  
  TIntV NIdV;
  int StartNId, Hop, Nodes;
  
  //  for (int IsDir = 0; IsDir < 2; IsDir++) {
  int IsDir = 1;
  printf("IsDir = %d:\n", IsDir);
  
  StartNId = 11;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, IsDir);
  printf("Nodes = %d, GetNodesAtHop NIdV.Len() = %d\n", Nodes, NIdV.Len());
  for (int i = 0; i < NIdV.Len(); i++) {
    printf("NIdV[%d] = %d\n", i, NIdV[i].Val);
  }
  printf("Nodes == 2");
  printf("NIdV.Len() == 2");
  
  TIntPrV HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, IsDir);
  printf("Nodes = %d, GetNodesAtHops HopCntV.Len() = %d\n", Nodes, HopCntV.Len());
  printf("Nodes == 10");
  printf("HopCntV.Len() == 10");
//  for (int N = 0; N < HopCntV.Len(); N++) {
//    printf("HopCntV[%d] = (%d, %d)\n", N, HopCntV[N].Val1.Val, HopCntV[N].Val2.Val);
//  }
  
  int Length, SrcNId, DstNId;
  SrcNId = 11;
  DstNId = G->GetNodes() - 1;
  
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  printf("%d -> %d: SPL Length = %d\n", SrcNId, DstNId, Length);
  
  SrcNId = 11;
  DstNId = 27;
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  printf("%d -> %d: SPL Length = %d\n", SrcNId, DstNId, Length);
  
  TIntH NIdToDistH;
  int MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, IsDir, MaxDist);
//  for (int i = 0; i < min(5,NIdToDistH.Len()); i++) {
//    printf("NIdToDistH[%d] = %d\n", i, NIdToDistH[i].Val);
//  }
  
  TInt::Rnd.PutSeed(0);
  
  int FullDiam;
  double EffDiam, AvgSPL;
  int NTestNodes = G->GetNodes() / 2;
  
  FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
  printf("FullDiam = %d\n", FullDiam);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir);
  printf("EffDiam = %.3f\n", EffDiam);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam);
  printf("EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam, AvgSPL);
  printf("EffDiam = %.3f, FullDiam = %d, AvgDiam = %.3f\n", EffDiam, FullDiam, AvgSPL);
  
  TIntV SubGraphNIdV;
  SubGraphNIdV.Add(8);
  SubGraphNIdV.Add(29);
  SubGraphNIdV.Add(16);
  SubGraphNIdV.Add(0);
  SubGraphNIdV.Add(19);
  SubGraphNIdV.Add(17);
  SubGraphNIdV.Add(26);
  SubGraphNIdV.Add(14);
  SubGraphNIdV.Add(10);
  SubGraphNIdV.Add(24);
  SubGraphNIdV.Add(27);
  SubGraphNIdV.Add(2);
  SubGraphNIdV.Add(18);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
  printf("For subgraph: EffDiam = %.4f, FullDiam = %d\n", EffDiam, FullDiam);
  
}

// Demos BFS functions on full graphs of each type
void DemoBFSCompleteGraph() {
  
  printf("Demoing undirected complete graph:\n");
  DemoFullBfsDfs<PUNGraph>();
  
  printf("Demoing directed complete graph:\n");
  DemoFullBfsDfs<PNGraph>();
  
  printf("Demoing undirected complete graph:\n");
  DemoFullBfsDfs<PNEGraph>();
  
}

int main(int argc, char* argv[]) {
  
  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);

  DemoBFSUndirectedRandom();
  DemoBFSDirectedRandom();
  DemoBFSCompleteGraph();
  
  return 0;
}
