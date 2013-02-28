//
//  main.cpp
//  XcodeTest
//
//  A sample SNAP program which uses Xcode.
//
//  For more detailed examples, see the snap/examples directory.

#include "Snap.h"

using namespace TSnap;

int main(int argc, char* argv[]) {
    
  // Demos Breadth First Search functions on a full graph.
  const int NNodes = 500;
  
  PNGraph G = GenFull<PNGraph>(NNodes);
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
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, true);
  printf("StartNId = %d, Nodes = %d, GetNodesAtHop NIdV.Len() = %d\n", StartNId, Nodes, NIdV.Len());
  
  TIntPrV HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, true);
  printf("StartNId = %d, Nodes = %d, GetNodesAtHops HopCntV.Len() = %d\n", StartNId, Nodes, HopCntV.Len());
  
  int Length, SrcNId, DstNId;
  SrcNId = 1;
  DstNId = NNodes-1;
  
  Length = GetShortPath(G, SrcNId, DstNId, true);
  printf("SPL Length = %d\n", Length);
  
  TIntH NIdToDistH;
  int MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, true, MaxDist);
  
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
    
    EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
    printf("For subgraph: EffDiam = %.3f, FullDiam = %d\n", EffDiam, FullDiam);
    
  }
  
  return 0;
}
