#ifndef snap_temporalmotifs_h
#define snap_temporalmotifs_h
#include "Snap.h"
#include "motifcluster.h"

class TemporalGraph {
 public:
  TemporalGraph() {}
  void LoadData(const TStr& filename);

 private:
  PNGraph static_graph_;
  int max_nodes_ = -1;
  TIntV timestamps_;
  // temporal_data_[i][j] --> list of sorted timestamps on (i, j)
  // This data structure double stores to access (i, j) or (j, i)
  TVec< THash<TInt, TIntV> > temporal_data_;
};

class ThreeEventMotifCounter {
public:
  ThreeEventMotifCounter(int size);
  void Count(const TIntV& event_string,
	     const TIntV& timestamps,
             double delta);
private:
  void IncrementCounts(int event);
  void DecrementCounts(int event);
  TIntV counts1_;
  TVec< TIntV > counts2_;
  TVec< TVec< TIntV > > counts3_;
  int size_;  // alphabet size
};

#endif  // snap_temporalmotifs_h
