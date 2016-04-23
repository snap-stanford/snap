#include <vector>
#include <stdio.h>

struct ChildTypeCount {
  int claimedChildren;
  int failedChildren;
  int peers;
  int validParents;
};

//#//////////////////////////////////////////////
/// Breath-First-Search class.
/// The class is meant for executing many BFSs over a fixed graph. This means that the class can keep the hash tables and queues initialized between different calls of the DoBfs() function.
template<class PGraph>
class TBreathFS_Test {
public:
  PGraph Graph;
  TSnapQueue<int> Queue;
  TInt StartNId;
  TIntH NIdDistH;
  std::vector<ChildTypeCount> childCounts;
public:
  TBreathFS_Test(const PGraph& GraphPt, const bool& InitBigQ=true) :
    Graph(GraphPt), Queue(InitBigQ?Graph->GetNodes():1024), NIdDistH(InitBigQ?Graph->GetNodes():1024) { }
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs_test(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
};

template<class PGraph>
int TBreathFS_Test<PGraph>::DoBfs_test(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
//  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);
//  IAssertR(StartNodeI.GetOutDeg() > 0, TStr::Fmt("No neighbors from start node %d.", StartNode));
  NIdDistH.Clr(false);  NIdDistH.AddDat(StartNId, 0);
  Queue.Clr(false);  Queue.Push(StartNId);
  int v, MaxDist = 0;
  while (! Queue.Empty()) {
    const int NId = Queue.Top();  Queue.Pop();
    const int Dist = NIdDistH.GetDat(NId);
    if (Dist == MxDist) { break; } // max distance limit reached

    // for analyzing edges
    if (Dist >= (int)childCounts.size()) {
      struct ChildTypeCount c = {0, 0, 0, 0};
      childCounts.push_back(c);
      if (Dist != (int)childCounts.size() - 1) {
        printf("Error.....\n");
        exit(1); // sanity check
      }
    }

    const typename PGraph::TObj::TNodeI NodeI = Graph->GetNI(NId);
    if (FollowOut) { // out-links
      for (v = 0; v < NodeI.GetOutDeg(); v++) {  // out-links
        const int DstNId = NodeI.GetOutNId(v);
        if (! NIdDistH.IsKey(DstNId)) {
          NIdDistH.AddDat(DstNId, Dist+1);
          MaxDist = TMath::Mx(MaxDist, Dist+1);
          if (DstNId == TargetNId) { return MaxDist; }
          Queue.Push(DstNId);
          // Claimed child
          childCounts[Dist].claimedChildren += 1;
        } else {
          // Node visited before
          const int nextDist = NIdDistH.GetDat(DstNId);
          if (nextDist < Dist) {
            childCounts[Dist].validParents += 1;
          } else if (nextDist == Dist) {
            childCounts[Dist].peers += 1;
          } else {
            childCounts[Dist].failedChildren += 1;
          }
        }
      }
    }
    if (FollowIn) { // in-links
      for (v = 0; v < NodeI.GetInDeg(); v++) {
        const int DstNId = NodeI.GetInNId(v);
        if (! NIdDistH.IsKey(DstNId)) {
          NIdDistH.AddDat(DstNId, Dist+1);
          MaxDist = TMath::Mx(MaxDist, Dist+1);
          if (DstNId == TargetNId) { return MaxDist; }
          Queue.Push(DstNId);
        }
      }
    }
  }
  return MaxDist;
}
