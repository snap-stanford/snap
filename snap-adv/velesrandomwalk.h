#ifndef VEL_RAND_WALK_H
#define VEL_RAND_WALK_H

class TNodeDat;
class TKdTree;

typedef TNodeEDatNet<TNodeDat, TFlt> TWNet;
typedef TPt<TWNet> PWNet;
typedef TPair<TVec<TInt>, TVec<TFlt> > TIntVFltVPr;

const int MDim=5;

class TKdTree{// This k-d tree is designed specifically for this algorithm, don't reuse it.
public:
  class TTreeNode{
  private:
    TBool Leaf;
    TIntVFltVPr PTable;
    TIntV Indices;
    TInt Depth;
    TFlt UnderSum;
    TFlt OverSum;
    TInt UnderId;
    TInt OverId;
  public:
    TTreeNode() : Leaf(0), PTable(), Indices(), Depth(-1), UnderSum(0),
     OverSum(0), UnderId(-1), OverId(-1) {}
    TTreeNode(const TTreeNode& Node) : Leaf(Node.Leaf), PTable(Node.PTable),
     Indices(Node.Indices), Depth(Node.Depth), UnderSum(Node.UnderSum),
     OverSum(Node.OverSum), UnderId(Node.UnderId), OverId(Node.OverId) {}
    TTreeNode(TSIn& SIn) : Leaf(SIn), PTable(SIn), Indices(SIn),
     Depth(SIn), UnderSum(SIn), OverSum(SIn), UnderId(SIn), OverId(SIn) {}
    void Save(TSOut& SOut) const { Leaf.Save(SOut); PTable.Save(SOut);
      Indices.Save(SOut); Depth.Save(SOut); UnderSum.Save(SOut); OverSum.Save(SOut);
      UnderId.Save(SOut); OverId.Save(SOut); }
    TTreeNode& operator = (const TTreeNode& MCNode) { Leaf=MCNode.Leaf;
     PTable=MCNode.PTable; Indices=MCNode.Indices; Depth=MCNode.Depth;
     UnderSum=MCNode.UnderSum; OverSum=MCNode.OverSum; UnderId=MCNode.UnderId;
     OverId=MCNode.OverId; return *this; }
    void Init(TIntV& IndicesV, TFltV& Weights, TVec<TIntV>& Embs,
     TIntV& SplitV, int Level, TKdTree& ParentTree);
    TBool IsLeaf() { return Leaf; }
    TIntVFltVPr GetPTable() const { return PTable; }
    TInt GetDepth() const { return Depth; }
    TInt GetUnderId() const { return UnderId; }
    TInt GetOverId() const { return OverId; }
    double GetUnderSum() const { return UnderSum; }
    double GetOverSum() const { return OverSum; }
    const TIntV& GetIndices() const {return Indices;}
    TIntV& GetIndices() {return Indices;}
  };
private:
  TIntV SplitV;
  TVec<TTreeNode> NodeV;
public:
  TKdTree (): SplitV(), NodeV() {}
  TKdTree (const TKdTree& Tree) : SplitV(Tree.SplitV), NodeV(Tree.NodeV) {}
  TKdTree (TSIn& SIn) : SplitV(SIn), NodeV(SIn) {}
  void Save(TSOut& SOut) const { SplitV.Save(SOut); NodeV.Save(SOut); }
  TKdTree& operator = (const TKdTree& MCTree) { SplitV=MCTree.SplitV;
   NodeV=MCTree.NodeV; return *this; }
  void Init(TIntV& IndicesV, TFltV& Weights, TVec<TIntV>& Embs);
  const TVec<TTreeNode>& GetNodes() const { return NodeV; }
  TVec<TTreeNode>& GetNodes() { return NodeV; }
  const TIntV& GetSplitV() const { return SplitV; }
  TIntV& GetSplitV() { return SplitV; }
  int GetRndEl (TIntV& Src, double Q, TRnd& Rnd);
};

class TNodeDat {
private:
  TIntV RVec;
  TKdTree DecisionTree;
public:
  TNodeDat() : RVec(MDim), DecisionTree() {}
  TNodeDat(const TNodeDat& NodeDat) : RVec(NodeDat.RVec), DecisionTree(NodeDat.DecisionTree) {}
  TNodeDat(TSIn& SIn) : RVec(SIn), DecisionTree(SIn) {}
  void Save(TSOut& SOut) const { RVec.Save(SOut); DecisionTree.Save(SOut); }
  TNodeDat& operator = (const TNodeDat& MCTree) { RVec=MCTree.RVec;
   DecisionTree=MCTree.DecisionTree; return *this; }
  const TIntV& GetRVec() const { return RVec; }
  TIntV& GetRVec() { return RVec; }
  const TKdTree& GetDecisionTree() const {return DecisionTree; }
  TKdTree& GetDecisionTree() { return DecisionTree; }
};

void BuildRVec(PWNet& InNet);
///Preprocesses transition probabilities for random walks. Has to be calles once before SimulateWalk calls
void PreprocessTransitionProbs(PWNet& InNet, bool& verbose);
///Simulates one walk and writes it into Walk vector
void SimulateWalk(PWNet& InNet, int StartNId, int& WalkLen, double& P,
 double& Q, TRnd& Rnd, TVec<TInt, int64>& Walk);

#endif //VEL_RAND_WALK_H
