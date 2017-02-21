#ifndef SNAP_SUBNETWORK_ISOMORPHISM_H
#define SNAP_SUBNETWORK_ISOMORPHISM_H

#include "stdafx.h"
#undef min
#undef max
#include <algorithm>

template<class TPNetwork, int TSize>
struct TSubnetworkIsomorphismId;

/**
 * Author: Klemen Simonic
 *
 * This module implements basic (naive) subnetworks isomorphism functionality.
 * To be precise, it performs isomorphism on directed or undirected, induced,
 * connected, simple (no multiple edges) and possibly with self-loops subgraphs 
 * with labels (data) on nodes and edges of a given graph.
 *
 * Each subnetwork is represented by an instance of public TId type, which is 
 * a dense representation of the underlying subgraph and its labels. The id
 * consits of two main parts:
 *  - Nodes: stores the data for each node.
 *  - Edges: stores the data for each edge as an adjacency matrix.
 *
 * For more information about the usage, please check TSubgraphIsomorphism
 * documentation.
 */
template<class TPNetwork, int TSize>
class TSubnetworkIsomorphism {
public:
  typedef TSubnetworkIsomorphismId<TPNetwork, TSize> TId;

private:
  typedef TPNetwork PNetwork;
  static const int SIZE = TSize;

  typedef typename PNetwork::TObj TNetwork;
  typedef typename TNetwork::TNodeDat TNodeData;
  typedef typename TNetwork::TEdgeDat TEdgeData;
  typedef TTuple<TNodeData, SIZE> TArrayNodeData;
  typedef TTuple<TEdgeData, SIZE> TArrayEdgeData;
  typedef TInt TNode;
  typedef TVec<TNode> TVecNode;
  typedef TVec<TVecNode> TVecVecNode;

  PNetwork Network;
  int NumIsomorphicIds;
  TVecVecNode Permutations;
  TEdgeData NoEdgeData;

  void ComputePermutations();

public:
  ///Initializes the internal state.
  ///The user has to specify the NoEdgeData parameter, which is a specific
  ///edge data value that does not represent an actually edge data value.
  void Init(const PNetwork &Network, const TEdgeData &NoEdgeData);
  void Deinit();

  ///Computes the subnetwork id induced by the specified nodes.
  void GetId(const TVecNode &Nodes, TId &Id) const;
  ///Returns the k-th isomorphic subnetwork id based on the specified id.
  void GetIsomorphicId(const TId &BaseId, int K, TId &NextId) const;

  ///Returns the number of isomorphic subnetworks.
  int GetNumIsomorphicIds() const;

  ///Returns the subnetwork for the specified subgraph id.
  PNetwork GetNetwork(const TId &Id) const;
};

template<class TPNetwork, int TSize>
const int TSubnetworkIsomorphism<TPNetwork,TSize>::SIZE;

template<class TPNetwork, int TSize>
void TSubnetworkIsomorphism<TPNetwork, TSize>::Init(const PNetwork &Network, const TEdgeData &NoEdgeData) {
  this->Network = Network;
  this->NoEdgeData = NoEdgeData;

  NumIsomorphicIds = TMath::Fac(SIZE);

  ComputePermutations();
};

template<class TPNetwork, int TSize>
void TSubnetworkIsomorphism<TPNetwork, TSize>::Deinit () {
  Permutations.Clr(true);
}

template<class TPNetwork, int TSize>
void TSubnetworkIsomorphism<TPNetwork, TSize>::GetId(const TVecNode &Nodes, TId &Id) const {
  for (int i = 0; i < SIZE; i++) {
    TNode NodeId = Nodes[i];
    typename TNetwork::TNodeI NodeIt = Network->GetNI(NodeId);
    Id.Nodes[i] = NodeIt.GetDat();
  }

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      TNode u = Nodes[i];
      TNode v = Nodes[j];

      int index = i * SIZE + j;

      typename TNetwork::TEdgeI EdgeIt = Network->GetEI(u, v);
      if (EdgeIt == Network->EndEI()) {
        Id.Edges[index] = NoEdgeData;
      } else {
        Id.Edges[index] = EdgeIt.GetDat();
      }
    }
  }
}

template<class TPNetwork, int TSize>
int TSubnetworkIsomorphism<TPNetwork, TSize>::GetNumIsomorphicIds () const {
  return NumIsomorphicIds;
}

template<class TPNetwork, int TSize>
void TSubnetworkIsomorphism<TPNetwork, TSize>::GetIsomorphicId(const TId &BaseId, int K, TId &NextId) const {
  const TVecNode &Nodes = Permutations[K];

  for (int i = 0; i < SIZE; i++) {
    NextId.Nodes[i] = BaseId.Nodes[Nodes[i]];
  }

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      int u = Nodes[i];
      int v = Nodes[j];

      NextId.Edges[u * SIZE + v] = BaseId.Edges[i * SIZE + j];
    }
  }
}

template<class TPNetwork, int TSize>
TPNetwork TSubnetworkIsomorphism<TPNetwork, TSize>::GetNetwork(const TId &Id) const {
  PNetwork G = PNetwork::New();

  for (int i = 0; i < SIZE; i++) {
    G->AddNode(i, Id.Nodes[i]);
  }

  for (int i = 0; i < SIZE; i++) {
    for(int j = 0; j < SIZE; j++) {
      int n = i * SIZE + j;
      if (Id.Edges[n] != NoEdgeData) {
        G->AddEdge(i, j, Id.Edges[n]);
      }
    }
  }

  return G;
}

template<class TPNetwork, int TSize>
void TSubnetworkIsomorphism<TPNetwork, TSize>::ComputePermutations() {
  Permutations.Gen((int)NumIsomorphicIds, (int)NumIsomorphicIds);

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


template<class TPNetwork, int TSize>
struct TSubnetworkIsomorphismId {
  typedef TPNetwork PNetwork;
  static const int SIZE = TSize;

  typedef typename PNetwork::TObj TNetwork;
  typedef typename TNetwork::TNodeDat TNodeData;
  typedef typename TNetwork::TEdgeDat TEdgeData;
  typedef TTuple<TNodeData, SIZE> TArrayNodeData;
  typedef TTuple<TEdgeData, SIZE*SIZE> TArrayEdgeData;
  typedef TInt TNode;
  typedef TVec<TNode> TVecNode;
  typedef TVec<TVecNode> TVecVecNode;
  
  typedef TSubnetworkIsomorphismId TId;

  TArrayNodeData Nodes;
  TArrayEdgeData Edges;

  uint64 GetHashCode() const {
    uint64 hash = 5381;

    for (int i = 0; i < SIZE; i++) {
      hash = ((hash << 5) + hash) + (uint64)Nodes[i].GetPrimHashCd();
    }

    for (int i = 0; i < SIZE * SIZE; i++) {
      hash = ((hash << 5) + hash) + (uint64)Edges[i].GetPrimHashCd();
    }

    return hash;
  }

  int GetPrimHashCd() const { 
    return (int)GetHashCode();
  }

  int GetSecHashCd() const {
    return (int)GetHashCode(); 
  }

  bool operator==(const TId &Id) const {
    for (int i = 0; i < SIZE; i++) {
      if (Nodes[i] != Id.Nodes[i]) {
        return false;
      }
    }

    for (int i = 0; i < SIZE * SIZE; i++) {
      if (Edges[i] != Id.Edges[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const TId &Id) const {
    return !operator==(Id);
  }

  TId & operator=(const TId &Id) {
    Nodes = Id.Nodes;
    Edges = Id.Edges;

    return *this;
  }

  bool operator<(const TId &Id) const {
    for (int i = 0; i < SIZE; i++) {
      if (Nodes[i] >= Id.Nodes[i]) {
        return false;
      }
    }

    for (int i = 0; i < SIZE * SIZE; i++) {
      if (Edges[i] >= Id.Edges[i]) {
        return false;
      }
    }

    return true;
  }
};

#endif
