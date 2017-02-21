#ifndef SNAP_SUBUNIT_COUNTER_H
#define SNAP_SUBUNIT_COUNTER_H

#include "stdafx.h"

/**
 * Author: Klemen Simonic
 *
 * This module acts as a functor for TEnumerateSubgraphs functions related with
 * enumerating size-k subgraphs of a give graph. It implements basic functionality 
 * for counting subunits (e.g. subgraphs, subnetworks), where subunit is defined
 * via template parameter TSubunitIsomorphism. It is an intermediate class between
 * a user defined subunit in graph and enumeration functions in TEnumerateSubgraphs.
 *
 * A subunit isomorphism class has to define:
 *   - Public type TId which is a representative for a subunit.
 *   - void GetId (const TVec<node_t> &Nodes, TId &Id) const, a function that 
 *     computes the id for a subunit based on the specified nodes.
 *   - void GetIsomorphicId (const TId &BaseId, int K, TId &NextId) const,
 *     a function that returns the K-th isomorphic id based on a specified base id.
 *   - int GetNumIsomorphicIds () const, a function that return the number of
 *     isomorphic ids.
 *
 * Examples of implementations are TSubgraphIsomorphism and TSubnetworkIsomorphism.
 *
 * This class enables us that we can define specialized subunits that we would like 
 * to count in the graph. 
 * For example: let us assume that the user wants to count the size-k multisubgraphs 
 * (size-k subgraphs with possible multitple edges between the nodes). In this case
 * the subunits has to store the number of edges between the nodes, while in a
 * scenario with simple size-k subgraphs (no multiple edges), the user only needs to
 * register the presence of the edges.
 */
template<class TPGraph, class TSubunitIsomorphism>
class TSubunitCounter {
public:
  typedef TPGraph PGraph;
  typedef typename TSubunitIsomorphism::TId TId;

private:
  typedef TInt TNode;
  typedef TVec<TNode> TVecNode;
  typedef THashSet<TNode> TSetNode;
  typedef uint64 TCount;
  typedef THash<TId, TCount> TIdToCount;
  typedef TVec<TId> TVecId;

  TSubunitIsomorphism *SubunitIsomorphism;
  TIdToCount IdToCount;
  TIdToCount MinIdToCount;

public:
  typedef typename TIdToCount::TIter TSubunitIt;

  void Init(TSubunitIsomorphism &SubunitIsomorphism);
  void Deinit();

  void operator()(const TVecNode &Nodes);

  ///Computes the isomorphism of the stored subunit ids.
  ///This function should be called after the enumeration process has finished.
  void ComputeIsomorphism();

  //Iteration functionality over the enumerated and non-isomorphic subunits.
  //These functions should be called after ComputeIsomorphism() function has
  //been called.
  TSubunitIt BegSubunitIt() const;
  TSubunitIt EndSubunitIt() const;
  uint64 Count(TSubunitIt It) const;
  const TId & SubunitId(TSubunitIt It) const;
};

template<class TPGraph, class TSubunitIsomorphism>
void TSubunitCounter<TPGraph,TSubunitIsomorphism>::Init(TSubunitIsomorphism &SubunitIsomorphism) {
  this->SubunitIsomorphism = &SubunitIsomorphism;
}

template<class TPGraph, class TSubunitIsomorphism>
void TSubunitCounter<TPGraph,TSubunitIsomorphism>::Deinit() {
  IdToCount.Clr(true);
  MinIdToCount.Clr(true);
}

template<class TPGraph, class TSubunitIsomorphism>
void TSubunitCounter<TPGraph,TSubunitIsomorphism>::operator()(const TVecNode &Nodes) {
  TId Id;
  SubunitIsomorphism->GetId(Nodes, Id);

  int KeyId = IdToCount.GetKeyId(Id);
  if (KeyId == -1) {
    IdToCount.AddDat(Id, 1);
  } else {
    IdToCount[KeyId]++;
  }
}

template<class TPGraph, class TSubunitIsomorphism>
void TSubunitCounter<TPGraph,TSubunitIsomorphism>::ComputeIsomorphism() {
  typedef typename TIdToCount::TIter TIter;

  TIter It;
  while ((It = IdToCount.BegI()) < IdToCount.EndI()) {
    TId BaseId = It.GetKey();

    TId NextId;
    TId MinId = BaseId;

    TCount SumCount = 0;

    int n = SubunitIsomorphism->GetNumIsomorphicIds();

    for (int k = 0; k < n; k++) {
      SubunitIsomorphism->GetIsomorphicId(BaseId, k, NextId);

      int KeyId = IdToCount.GetKeyId(NextId);
      if (KeyId != -1) {
        SumCount += IdToCount[KeyId];
        IdToCount.DelKeyId(KeyId);
      }

      if (NextId < MinId) {
        MinId = NextId;
      }
    }

    MinIdToCount.AddDat(MinId, SumCount);
  }
}

template<class TPGraph, class TSubunitIsomorphism>
typename TSubunitCounter<TPGraph,TSubunitIsomorphism>::TSubunitIt
TSubunitCounter<TPGraph,TSubunitIsomorphism>::BegSubunitIt() const {
  return MinIdToCount.BegI();
}

template<class TPGraph, class TSubunitIsomorphism>
typename TSubunitCounter<TPGraph,TSubunitIsomorphism>::TSubunitIt
TSubunitCounter<TPGraph,TSubunitIsomorphism>::EndSubunitIt() const {
  return MinIdToCount.EndI();
}

template<class TPGraph, class TSubunitIsomorphism>
const typename TSubunitIsomorphism::TId & 
TSubunitCounter<TPGraph,TSubunitIsomorphism>::SubunitId(TSubunitIt It) const {
  return It.GetKey();
}

template<class TPGraph, class TSubunitIsomorphism>
uint64 TSubunitCounter<TPGraph,TSubunitIsomorphism>::Count(TSubunitIt It) const {
  return It.GetDat();
}

#endif
