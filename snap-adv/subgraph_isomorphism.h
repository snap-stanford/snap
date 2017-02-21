#ifndef SNAP_SUBGRAPH_ISOMORPHISM_H
#define SNAP_SUBGRAPH_ISOMORPHISM_H

#include "stdafx.h"
#undef min
#undef max
#include <algorithm>

/**
 * Author: Klemen Simonic
 *
 * This module implements basic (naive) subgraphs isomorphism functionality.
 * To be precise, it performs isomorphism on directed or undirected, induced,
 * connected, simple (no multiple edges) and possibly with self-loops subgraphs 
 * of a given graph. The subgraphs can contain at most 8 nodes.
 *
 * Each subgraph is represented by an instance of public TId type, which is an 
 * 8 byte unsigned integer. Isomorphic graphs can have different subgraph ids.
 * The underlying idea is that during heavy computation the user only stores the 
 * subgraph ids, and does not concern with the subgraph isomorphism. After heavy
 * computation step, the user computes the subgraph isomorphism via specified 
 * functionality.
 * The rational is that performing subgraph isomorphism each time when we are given 
 * two subgraphs is expensive. Therefore, it is often much faster to simply store 
 * the subgraphs as ids and manipulate with them as they would all be non-isomorphic 
 * subgraphs. And, when we are finnaly interested in which ids represent the same 
 * subgraph, we perform the subgraph isomorphism in one step for all the isomorphic 
 * subgraphs.
 * 
 * Usage:
 *   Let G1 and G2 be an isomorphic 3-subgraphs of a bigger graph G of type 
 *   PUNGraph and N1 and N2 the corresponding nodes (stored as a vector of node ids).
 * 
 *   typedef TSubgraphIsomorphism<PUNGraph, 3> TSubgraphIso;
 *   TSubgraphIso SubgraphIso;
 *   SubgraphIso.Init(G);
 * 
 *   //We perform some computation. For example:
 *   TSubgraphIso::TId Id1;
 *   TSubgraphIso::TId Id2;
 *   SubgraphIso.GetId(N1, Id1);
 *   SubgraphIso.GetId(N2, Id2);
 *
 *   //Check whether G1 and G2 are isomorphic subgraphs
 *	 TSubgraphIso::TId Id;
 *   for (int k = 0; k < Subgraph_iso.NumIsomorphicSubgraphs(); k++) {
 *       SubgraphIso.GetIsomorphicId(Id1, k, Id);
 *       if (Id2 == Id) { //Subgraphs G1 and G2 are isomorphic }
 *   }
 *   //Typically, we have more than two subgraphs, and the above code enable us to compute
 *   //all the isomorphic subgraphs to a given subgraph in ONE STEP!
 * 
 *   SubgraphIso.Deinit();
 *
 * For more details, please check how is this functionality used in the TSubunitCounter class.
 */
template<class TPGraph, int TSize>
class TSubgraphIsomorphism {
//  static_assert(TSize >= 0,
//                "Template argument TSize must be a nonnegative number.");
//  static_assert(TSize <= 8,
//                "Template argument TSize can be at most of size 8.");

public:
  typedef TUInt64 TId;

private:
  typedef TPGraph PGraph;
  typedef TVec<TId> TVecId;
  typedef TInt TNode;
  typedef TVec<TNode> TVecNode;
  typedef TVec<TVecNode> TVecVecNode;
    
  static const int SIZE = TSize;

  PGraph Graph;
  TVecVecNode Permutations;
  int NumIsomorphicSubgraphs;

  void ComputePermutations();

public:
  void Init(const PGraph &Graph);
  void Deinit();

  ///Computes the subgraph id induced by the specified nodes.
  void GetId(const TVecNode &Nodes, TId &Id) const;
  ///Returns the k-th isomorphic subgraph id based on the specified id.
  void GetIsomorphicId(const TId &BaseId, int K, TId &NextId) const;

  ///Returns the number of isomorphic subgraphs
  int GetNumIsomorphicIds() const;

  ///Returns the graph for the specified subgraph id.
  static PGraph GetGraph(const TId &Id);
};

template<class TPGraph, int TSize>
const int TSubgraphIsomorphism<TPGraph, TSize>::SIZE;

template<class TPGraph, int TSize>
void TSubgraphIsomorphism<TPGraph, TSize>::Init(const PGraph &G) {
  Graph = G;

  NumIsomorphicSubgraphs = TMath::Fac(TSize);

  ComputePermutations();
}

template<class TPGraph, int TSize>
void TSubgraphIsomorphism<TPGraph, TSize>::Deinit () {
  Permutations.Clr(true);
}

template<class TPGraph, int TSize>
void TSubgraphIsomorphism<TPGraph, TSize>::GetId(const TVecNode &Nodes, TId &Id) const {
  Id = 0;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      TNode u = Nodes[i];
      TNode v = Nodes[j];

      if (Graph->IsEdge(u, v)) {
        Id += TMath::Pow2(i * SIZE + j);
      }
    }
  }
}

template<class TPGraph, int TSize>
int TSubgraphIsomorphism<TPGraph, TSize>::GetNumIsomorphicIds() const {
  return NumIsomorphicSubgraphs;
}

template<class TPGraph, int TSize>
void TSubgraphIsomorphism<TPGraph, TSize>::GetIsomorphicId(const TId &BaseId, int K, TId &NextId) const {
  const TVecNode &Nodes = Permutations[K];
  NextId = 0;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      int u = Nodes[i];
      int v = Nodes[j];

      int n = u * SIZE + v;
      if (((BaseId >> n) & 1) == 1) {
        NextId += TMath::Pow2(i * SIZE + j);
      }
    }
  }
}

template<class TPGraph, int TSize>
TPGraph TSubgraphIsomorphism<TPGraph, TSize>::GetGraph(const TId &id) {
  PGraph G = PGraph::New();

  for (int i = 0; i < SIZE; i++) {
    G->AddNode(i);
  }

  for (int i = 0; i < SIZE; i++) {
    for(int j = 0; j < SIZE; j++) {
      int n = i * SIZE + j;
      if (((id >> n) & 1) == 1) {
        G->AddEdge(i, j);
      }
    }
  }

  return G;
}

template<class TPGraph, int TSize>
void TSubgraphIsomorphism<TPGraph, TSize>::ComputePermutations() {
  Permutations.Gen((int)NumIsomorphicSubgraphs, (int)NumIsomorphicSubgraphs);

  TVecNode Permutation(SIZE, SIZE);
  for (int i = 0; i < SIZE; i++) {
    Permutation[i] = i;
  }

  int i = 0;
  do {
    Permutations[i] = Permutation;
    i++;
  } while (std::next_permutation(Permutation.BegI(), Permutation.EndI()));
}

#endif
