//
//  priorityqueue.h
//  glib-core
//
//  Created by Peter Lofgren on 8/5/15.
//  Copyright (c) 2015 infolab. All rights reserved.
//

#ifndef snap_core_priorityqueue_h
#define snap_core_priorityqueue_h


//typedef TInt TVal;

// A max priority queue which supports changing the priority of items.
// Uses a binary heap, so operations run in O(log(n)) time, where n is
// the number of items in the priority queue.
template <class TVal>
class TMaxPriorityQueue {
public:
  TMaxPriorityQueue() {}
  
  void Insert(const TVal& X, float Priority) {
    ValToIndex.AddDat(X, IndexToVal.Len());
    IndexToVal.Add(X);
    // Priorities.Add(-INFINITY);
    Priorities.Add(INT_MIN);
    SetPriority(X, Priority);
  }
  
  void SetPriority(const TVal& X, float NewPriority) {
    if (!ValToIndex.IsKey(X)) {
      Insert(X, NewPriority);
    } else {
      int i = ValToIndex.GetDat(X);
      if (NewPriority >= GetPriority(X)) {
        Priorities[i] = NewPriority;
        while (i > 0 && Priorities[i] > Priorities[Parent(i)]) {
          Swap(i, Parent(i));
          i = Parent(i);
        }
      } else {
        Priorities[i] = NewPriority;
        MaxHeapify(i);
      }
    }
  }
  
  float GetPriority(const TVal& X) {
    if (ValToIndex.IsKey(X)) {
      return Priorities[ValToIndex.GetDat(X)];
    } else {
      return 0.0f; // Default value is 0.0
    }
  }
  
  float GetMaxPriority() {
    IAssertR(Size() > 0, "Attempt to query max priority of empty priority queue.");
    return Priorities[0];
  }
  
  TVal PopMax() {
    IAssertR(Size() > 0, "Attempt to query max priority of empty priority queue.");
    TVal maxVal = IndexToVal[0];
    Swap(0, Priorities.Len() - 1);
    Priorities.DelLast();
    IndexToVal.DelLast();
    ValToIndex.DelKey(maxVal);
    
    MaxHeapify(0);
    return maxVal;
  }
  
  bool IsEmpty() {
    return Size() == 0;
  }
  
  int Size() {
    return Priorities.Len();
  }
  
  // Copies the current priorities into the given hash table
  void GetPriorities(THash<TVal, TFlt>& Result ) {
    for (int i = 0; i < Priorities.Len(); i++) {
      Result.AddDat(IndexToVal[i], Priorities[i]);
    }
  }
  
private:
  TFltV Priorities;
  THash<TVal, int> ValToIndex;
  TVec<TVal> IndexToVal;
  
  int Parent(int i) { return (i + 1) / 2 - 1; }
  int Left(int i) { return i * 2 + 1; }
  int Right(int i) { return i * 2 + 2; }
  
  void Swap(int i, int j) {
    Priorities.Swap(i, j);
    IndexToVal.Swap(i, j);
    ValToIndex.GetDat(IndexToVal[i]) = i;
    ValToIndex.GetDat(IndexToVal[j]) = j;
  }
  
  // If the max-heap invariant is satisfied except for index i possibly being smaller than a child, restore the invariant.
  void MaxHeapify(int i) {
    int largest = i;
    if (Left(i) < Priorities.Len() && Priorities[Left(i)] > Priorities[largest]) {
      largest = Left(i);
    }
    if (Right(i) < Priorities.Len() && Priorities[Right(i)] > Priorities[largest]) {
      largest = Right(i);
    }
    if (largest != i) {
      Swap(i, largest);
      MaxHeapify(largest);
    }
  }
};
#endif

/*
 void Insert(const Tval& X, float Priority);
 void SetPriority(const TVal& X, float NewPriority);
 float GetPriority(const TVal& X);
 float GetMaxPriority();
 TVal PopMax();
 bool IsEmpty();
*/
