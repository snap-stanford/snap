#ifndef SNAP_ENUMERATE_SUBGRAPHS_H
#define SNAP_ENUMERATE_SUBGRAPHS_H

#include "stdafx.h"

/**
 * Author: Klemen Simonic
 *
 * A modul for enumerating connected, induced, size-k subgraphs. The modul offers
 * several variations of enumeration:
 *  - Enumerating all connected, induced, size-k subgraphs of a given graph.
 *  - Uniform sampling of all connected, induced, size-k subgraphs of a given graph.
 *  - Enumerating all connected, induces, size-k subgraphs that contain a given node.
 *  - Uniform sampling of all connected, induced, size-k subgraphs that contain a given node.
 *
 * In all of the above scenarios, each size-k subgraph is enumerated exactly once.
 * 
 * The algorithm is based on paper Efficient Detection of Network Motifs by Sebastian 
 * Wernicke, IEEE/ACM Transactions on Computational Biology and Bioinformatics, 2006.
 *
 * The input to the algorithm is a SNAP graph-like data structure and a functor, which 
 * is called whenever a size-k subgraph is enumerated. The input to the functor is a 
 * set of vertices (stored as vector of node ids) that induce the enumerated size-k 
 * subgraph.
 * Sampling of size-k subgraphs requires two additional parameters:
 *  - A list of probabilities.
 *  - An instance of TRnd type (uniform sampling is used on the TRnd instance).
 * The i-th probability in the list determines the probability of taking the i-th node
 * in the incremental construction of size-k subgraph. For best use, we recommend the 
 * non-increasing sequence of probabilities of length k (e.g. {1.0, 0.75, 0.5} for k=3).
 *
 * The input graph does not need to be vertex-normalized (vertex id's are from 0 to n-1, 
 * where n is the number of vertices), although it is recommended for best memory and 
 * cache performance.
 *
 * Functor class has to implement the following method:
 *     void operator() (const TVec<int> &nodes)
 *
 * Usage:
 *     typedef PUNGraph Graph;
 *     typedef TEnumerateSubgraphs<Graph> TEnumSubgraphs;
 *
 *     //Let G be an instance of type Graph
 *     
 *     TEnumSubgraphs EnumSubgraphs;
 *     EnumSubgraphs.Init(G);
 *		
 *     Functor functor;
 *	   EnumSubgraphs.Enumerate(3, functor);
 *
 *	   EnumSubgraphs.Deinit();
 */   
template<class TPGraph>
class TEnumerateSubgraphs {
private:
  typedef TPGraph PGraph;
  typedef TInt TNode;
  typedef TVec<TNode> TVecNode;
  typedef THashSet<TNode> TSetNode;
  typedef THash<TNode, TVecNode> TNodeToVecNode;
  typedef TVec<uint8> TVecUInt8;
  typedef TVec<TVecNode> TVecVecNode;

  TNodeToVecNode NodeToNbhs;
  TNode MaxNodeId;

  template<class TCallback>
  void ExtendSubgraph (TVecNode &SubgraphVec, 
                        TVecUInt8 &SubgraphSet,
                        TVecNode &SubgraphNbhsVec, 
                        TVecUInt8 &SubgraphNbhsSet,
                        TVecVecNode &ExtensionVecVec,
                        TNode v, int SubgraphSize, TCallback &Callback,
                        const TFltV &Probabilities, TRnd &Rnd);
	
  template<class TCallback>
  void ExtendSubgraph (TVecNode &SubgraphVec,
                        TVecUInt8 &SubgraphSet,
                        TVecNode &SubgraphNbhsVec,
                        TVecUInt8 &SubgraphNbhsSet,
                        TVecVecNode &ExtensionVecVec,
                        int SubgraphSize, TCallback &Callback,
                        const TFltV &Probabilities, TRnd &Rnd);

  void AssertParameters(int SubgraphSize, const TFltV &Probabilities) const;

public:
  void Init(const PGraph &Graph);
  void Deinit();

  template<class TCallback>
  void Enumerate(int SubgraphSize, TCallback &Callback);
  template<class TCallback>
  void Enumerate(int SubgraphSize, TCallback &Callback, const TFltV &Probabilities, TRnd &Rnd);

  template<class TCallback>
  void Enumerate(TNode NodeId, int SubgraphSize, TCallback &Callback);
  template<class TCallback>
  void Enumerate(TNode NodeId, int SubgraphSize, TCallback &Callback, const TFltV &Probabilities, TRnd &Rnd);
};


template<class TPGraph>
void TEnumerateSubgraphs<TPGraph>::Init(const PGraph &Graph) {
  typedef typename PGraph::TObj::TNodeI TNodeIt;

  NodeToNbhs.Gen((int)(1.2 * Graph->GetNodes()));

  //For each node store all the neighbours' ids (unique and ordered)
  for (TNodeIt NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    TNode Node = NI.GetId();

    TSetNode SetNbhs;
    TVecNode VecNbhs;

    for (int i = 0; i < NI.GetDeg(); i++) {
      TNode Nbh = NI.GetNbrNId(i);
      SetNbhs.AddKey(Nbh);
    }

    SetNbhs.DelIfKey(Node);

    SetNbhs.GetKeyV(VecNbhs);

    VecNbhs.Sort();

    NodeToNbhs.AddDat(Node, VecNbhs);
  }

  MaxNodeId = 0;
  for (TNodeIt NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (MaxNodeId < NI.GetId()) {
      MaxNodeId = NI.GetId();
    }
  }

  IAssert(MaxNodeId >= 0);
}

template<class TPGraph>
void TEnumerateSubgraphs<TPGraph>::Deinit () {
  NodeToNbhs.Clr(true);
}

template<class TPGraph>
template<class TCallback>
void TEnumerateSubgraphs<TPGraph>::Enumerate(int SubgraphSize, TCallback &Callback) {
  IAssert(SubgraphSize > 1);

  TFltV Probabilities(SubgraphSize);
  for (int i = 0; i < Probabilities.Len(); i++) {
    Probabilities[i] = 1.0;
  }
        
  TRnd Rnd;

  Enumerate(SubgraphSize, Callback, Probabilities, Rnd);
}

template<class TPGraph>
template<class TCallback>
void TEnumerateSubgraphs<TPGraph>::Enumerate(int SubgraphSize, TCallback &Callback, const TFltV &Probabilities, TRnd &Rnd) {
  AssertParameters(SubgraphSize, Probabilities);

  TVecNode SubgraphVec;
  TVecUInt8 SubgraphSet(MaxNodeId + 1, MaxNodeId + 1);
  for (int i = 0; i < SubgraphSet.Len(); i++) {
    SubgraphSet[i] = 0;
  }

  TVecNode SubgraphNbhsVec;
  TVecUInt8 SubgraphNbhsSet(MaxNodeId + 1, MaxNodeId + 1);
  for (int i = 0; i < SubgraphSet.Len(); i++) {
    SubgraphNbhsSet[i] = 0;
  }

  const int OldNumNbhs = SubgraphNbhsVec.Len();

  TVecVecNode ExtensionVecVec(SubgraphSize);

  int h = SubgraphVec.Len();
  TVecNode &ExtensionVec = ExtensionVecVec[h];

  for (TNodeToVecNode::TIter NI = NodeToNbhs.BegI(); NI < NodeToNbhs.EndI(); NI++) {		
    if (Rnd.GetUniDev() > Probabilities[h]) {
      continue;
    }
        
    TNode v = NI.GetKey();
    const TVecNode &Nbhs = NI.GetDat();

    int index;
    Nbhs.SearchBin(v, index);

    for (int i = index; i < Nbhs.Len(); i++) {
      TNode Nbh = Nbhs[i];

      SubgraphNbhsSet[Nbh] = 1;
      SubgraphNbhsVec.Add(Nbh);

      ExtensionVec.Add(Nbh);
    }

    SubgraphSet[v] = 1;
    SubgraphVec.Add(v);

    ExtendSubgraph(SubgraphVec, SubgraphSet,
                    SubgraphNbhsVec, SubgraphNbhsSet,
                    ExtensionVecVec,
                    v, SubgraphSize, Callback,
                    Probabilities, Rnd);

    SubgraphSet[v] = 0;
    SubgraphVec.DelLast();

    for (int i = OldNumNbhs; i < SubgraphNbhsVec.Len(); i++) {
      TNode Nbh = SubgraphNbhsVec[i];
      SubgraphNbhsSet[Nbh] = 0;
    }

    if (OldNumNbhs < SubgraphNbhsVec.Len()) {
      SubgraphNbhsVec.Del(OldNumNbhs, SubgraphNbhsVec.Len() - 1);
    }
  }
}

template<class TPGraph>
template<class TCallback>
void TEnumerateSubgraphs<TPGraph>::ExtendSubgraph(TVecNode &SubgraphVec, 
                                                  TVecUInt8 &SubgraphSet,
                                                  TVecNode &SubgraphNbhsVec, 
                                                  TVecUInt8 &SubgraphNbhsSet,
                                                  TVecVecNode &ExtensionVecVec,
                                                  TNode v, int SubgraphSize, TCallback &Callback,
                                                  const TFltV &Probabilities, TRnd &Rnd) {
  int h = SubgraphVec.Len();
  TVecNode &ExtensionVec = ExtensionVecVec[h - 1];

  if (SubgraphVec.Len() == SubgraphSize - 1) {
    while (!ExtensionVec.Empty()) {
      TNode w = ExtensionVec.Last();
      ExtensionVec.DelLast();

      if (Rnd.GetUniDev() > Probabilities[h]) {
        continue;
      }

      SubgraphVec.Add(w);

      Callback(SubgraphVec);

      SubgraphVec.DelLast();
    }

    return;
  }

  TVecNode &WExtensionVec = ExtensionVecVec[h];

  const int OldNumNbhs = SubgraphNbhsVec.Len();

  while (!ExtensionVec.Empty()) {
    TNode w = ExtensionVec.Last();
    ExtensionVec.DelLast();

    if (Rnd.GetUniDev() > Probabilities[h]) {
      continue;
    }

    WExtensionVec.AddV(ExtensionVec);

    const TVecNode &WNbhs = NodeToNbhs.GetDat(w);

    int index;
    WNbhs.SearchBin(v, index);

    for (int i = index; i < WNbhs.Len(); i++) {
      TNode Nbh = WNbhs[i];

      if (SubgraphSet[Nbh] || SubgraphNbhsSet[Nbh]) {
        continue;
      }

      SubgraphNbhsSet[Nbh] = 1;
      SubgraphNbhsVec.Add(Nbh);

      WExtensionVec.Add(Nbh);
    }

    SubgraphSet[w] = 1;
    SubgraphVec.Add(w);

    ExtendSubgraph(SubgraphVec, SubgraphSet, 
                    SubgraphNbhsVec, SubgraphNbhsSet,
                    ExtensionVecVec, 
                    v, SubgraphSize, Callback,
                    Probabilities, Rnd);

    SubgraphSet[w] = 0;
    SubgraphVec.DelLast();

    for (int i = OldNumNbhs; i < SubgraphNbhsVec.Len(); i++) {
      TNode Nbh = SubgraphNbhsVec[i];
      SubgraphNbhsSet[Nbh] = 0;
    }

    if (OldNumNbhs < SubgraphNbhsVec.Len()) {
      SubgraphNbhsVec.Del(OldNumNbhs, SubgraphNbhsVec.Len() - 1);
    }
  }
}

template<class TPGraph>
template<class TCallback>
void TEnumerateSubgraphs<TPGraph>::Enumerate(TNode NodeId, int SubgraphSize, TCallback &Callback) {
  IAssert(SubgraphSize > 1);

  TFltV Probabilities(SubgraphSize);
  for (int i = 0; i < Probabilities.Len(); i++) {
    Probabilities[i] = 1.0;
  }
        
  TRnd Rnd;

  Enumerate(NodeId, SubgraphSize, Callback, Probabilities, Rnd);
}

template<class TPGraph>
template<class TCallback>
void TEnumerateSubgraphs<TPGraph>::Enumerate(TNode NodeId, int SubgraphSize, TCallback &Callback, const TFltV &Probabilities, TRnd &Rnd) {
  AssertParameters(SubgraphSize, Probabilities);

  TVecNode SubgraphVec;
  TVecUInt8 SubgraphSet(MaxNodeId + 1, MaxNodeId + 1);
  for (int i = 0; i < SubgraphSet.Len(); i++) {
    SubgraphSet[i] = 0;
  }

  TVecNode SubgraphNbhsVec;
  TVecUInt8 SubgraphNbhsSet(MaxNodeId + 1, MaxNodeId + 1);
  for (int i = 0; i < SubgraphSet.Len(); i++) {
    SubgraphNbhsSet[i] = 0;
  }

  const int OldNumNbhs = SubgraphNbhsVec.Len();

  TVecVecNode ExtensionVecVec(SubgraphSize);

  int h = SubgraphVec.Len();
  TVecNode &ExtensionVec = ExtensionVecVec[h];

  TNodeToVecNode::TIter NI = NodeToNbhs.GetI(NodeId);

  TNode v = NI.GetKey();
  const TVecNode &Nbhs = NI.GetDat();

  for (int i = 0; i < Nbhs.Len(); i++) {
    TNode Nbh = Nbhs[i];

    SubgraphNbhsSet[Nbh] = 1;
    SubgraphNbhsVec.Add(Nbh);

    ExtensionVec.Add(Nbh);
  }

  SubgraphSet[v] = 1;
  SubgraphVec.Add(v);

  ExtendSubgraph(SubgraphVec, SubgraphSet,
                  SubgraphNbhsVec, SubgraphNbhsSet,
                  ExtensionVecVec,
                  SubgraphSize, Callback,
                  Probabilities, Rnd);
}

template<class TPGraph>
template<class TCallback>
void TEnumerateSubgraphs<TPGraph>::ExtendSubgraph(TVecNode &SubgraphVec,
                                                  TVecUInt8 &SubgraphSet,
                                                  TVecNode &SubgraphNbhsVec, 
                                                  TVecUInt8 &SubgraphNbhsSet,
                                                  TVecVecNode &ExtensionVecVec,
                                                  int SubgraphSize, TCallback &Callback,
                                                  const TFltV &Probabilities, TRnd &Rnd) {
  int h = SubgraphVec.Len();
  TVecNode &ExtensionVec = ExtensionVecVec[h - 1];

  if (SubgraphVec.Len() == SubgraphSize - 1) {
    while (!ExtensionVec.Empty()) {
      TNode w = ExtensionVec.Last();
      ExtensionVec.DelLast();

      if (Rnd.GetUniDev() > Probabilities[h]) {
        continue;
      }

      SubgraphVec.Add(w);

      Callback(SubgraphVec);

      SubgraphVec.DelLast();
    }

    return;
  }

  TVecNode &WExtensionVec = ExtensionVecVec[h];

  const int OldNumNbhs = SubgraphNbhsVec.Len();

  while (!ExtensionVec.Empty()) {
    TNode w = ExtensionVec.Last();
    ExtensionVec.DelLast();

    if (Rnd.GetUniDev() > Probabilities[h]) {
      continue;
    }

    WExtensionVec.AddV(ExtensionVec);

    const TVecNode &WNbhs = NodeToNbhs.GetDat(w);

    for (int i = 0; i < WNbhs.Len(); i++) {
      TNode Nbh = WNbhs[i];

      if (SubgraphSet[Nbh] || SubgraphNbhsSet[Nbh]) {
        continue;
      }

      SubgraphNbhsSet[Nbh] = 1;
      SubgraphNbhsVec.Add(Nbh);

      WExtensionVec.Add(Nbh);
    }

    SubgraphSet[w] = 1;
    SubgraphVec.Add(w);

    ExtendSubgraph(SubgraphVec, SubgraphSet, 
                    SubgraphNbhsVec, SubgraphNbhsSet,
                    ExtensionVecVec, 
                    SubgraphSize, Callback,
                    Probabilities, Rnd);

    SubgraphSet[w] = 0;
    SubgraphVec.DelLast();

    for (int i = OldNumNbhs; i < SubgraphNbhsVec.Len(); i++) {
      TNode Nbh = SubgraphNbhsVec[i];
      SubgraphNbhsSet[Nbh] = 0;
    }

    if (OldNumNbhs < SubgraphNbhsVec.Len()) {
      SubgraphNbhsVec.Del(OldNumNbhs, SubgraphNbhsVec.Len() - 1);
    }
  }
}

template<class TPGraph>
void TEnumerateSubgraphs<TPGraph>::AssertParameters(int SubgraphSize, const TFltV &Probabilities) const {
  IAssert(SubgraphSize > 1);

  IAssert(Probabilities.Len() == SubgraphSize);

  for (int i = 0; i < Probabilities.Len(); i++) {
    IAssert(Probabilities[i] >= 0);
    IAssert(Probabilities[i] <= 1);
  }
}

#endif
