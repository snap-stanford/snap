#include "Snap.h"
#include "temporalmotifs.h"

// TODO(arbenson): remove these
#include <cassert>
#include <iostream>

const int kNumThreeEventEdgeMotifs = 4;
const int kNumThreeEventStars = 24;

typedef TKeyDat<TInt, TInt> TIntPair;

void TemporalMotifCounter::LoadData(const TStr& filename) {
  // First load the static graph
  static_graph_ = TSnap::LoadEdgeList<PNGraph>(filename, 0, 1);
  int max_nodes = static_graph_->GetMxNId();
  temporal_data_ = TVec< THash<TInt, TIntV> >(max_nodes);

  // Formulate input File Format:
  //   source_node destination_node timestamp
  TTableContext context;
  Schema temp_graph_schema;
  temp_graph_schema.Add(TPair<TStr,TAttrType>("source", atInt));
  temp_graph_schema.Add(TPair<TStr,TAttrType>("destination", atInt));
  temp_graph_schema.Add(TPair<TStr,TAttrType>("start", atInt));

  // Load the temporal graph
  PTable data_ptr = TTable::LoadSS(temp_graph_schema, filename, &context, ' ');
  TInt src_idx = data_ptr->GetColIdx("source");
  TInt dst_idx = data_ptr->GetColIdx("destination");
  TInt tim_idx = data_ptr->GetColIdx("start");
  for (TRowIterator RI = data_ptr->BegRI(); RI < data_ptr->EndRI(); RI++) {
    TInt row_idx = RI.GetRowIdx();
    int src = data_ptr->GetIntValAtRowIdx(src_idx, row_idx).Val;
    int dst = data_ptr->GetIntValAtRowIdx(dst_idx, row_idx).Val;
    int tim = data_ptr->GetIntValAtRowIdx(tim_idx, row_idx).Val;
    temporal_data_[src](dst).Add(tim);
  }
}

void TemporalMotifCounter::ThreeEventEdgeCounts(double delta, Counter3D& counts) {
  // Get a vector of undirected edges (so we can use openmp over it)
  TVec< TIntPair > undir_edges;
  for (TNGraph::TEdgeI it = static_graph_->BegEI();
       it < static_graph_->EndEI(); it++) {
    int src = it.GetSrcNId();
    int dst = it.GetDstNId();
    // Only consider undirected edges
    if (src < dst || (dst < src && !static_graph_->IsEdge(dst, src))) {
      undir_edges.Add(TIntPair(src, dst));
    }
  }
  counts = Counter3D(2, 2, 2);
  #pragma omp parallel for
  for (int i = 0; i < undir_edges.Len(); i++) {
    TIntPair edge = undir_edges[i];
    // Get the counts for that particular edge
    Counter3D local_counts;
    int src = edge.Key;
    int dst = edge.Dat;
    ThreeEventEdgeCounts(src, dst, delta, local_counts);
    #pragma omp critical
    {
      for (int dir1 = 0; dir1 < 2; ++dir1) {
	for (int dir2 = 0; dir2 < 2; ++dir2) {
	  for (int dir3 = 0; dir3 < 2; ++dir3) {
	    counts(dir1, dir2, dir3) += local_counts(dir1, dir2, dir3);
	  }
	}
      }
    }
  }
}

void TemporalMotifCounter::ThreeEventStarCounts(double delta, Counter3D& pre_counts,
						Counter3D& post_counts, Counter3D& mid_counts) {
  // Get a vector of nodes (so we can use openmp over it)
  TIntV centers;
  for (TNGraph::TNodeI it = static_graph_->BegNI();
       it < static_graph_->EndNI(); it++) {
    centers.Add(it.GetId());
  }

  // Get counts for each node as the center
  pre_counts = Counter3D(2, 2, 2);
  post_counts = Counter3D(2, 2, 2);
  mid_counts = Counter3D(2, 2, 2);  
  #pragma omp parallel for
  for (int c = 0; c < centers.Len(); c++) {
    // Gather all adjacent events
    int center = centers[c];
    TVec< TKeyDat<TInt, TIntPair> > ts_nbr_dir;
    TNGraph::TNodeI NI = static_graph_->GetNI(center);
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      int nbr = NI.GetOutNId(i);
      TIntV& ts_vec = temporal_data_[center](nbr);
      for (int j = 0; j < ts_vec.Len(); ++j) {
	TIntPair nbr_dir(nbr, 0);
	ts_nbr_dir.Add(TKeyDat<TInt, TIntPair>(ts_vec[j], nbr_dir));
      }
    }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      int nbr = NI.GetInNId(i);
      TIntV& ts_vec = temporal_data_[nbr](center);
      for (int j = 0; j < ts_vec.Len(); ++j) {
	TIntPair nbr_dir(nbr, 1);
	ts_nbr_dir.Add(TKeyDat<TInt, TIntPair>(ts_vec[j], nbr_dir));
      }      
    }
    ts_nbr_dir.Sort();
    TIntV timestamps;
    TIntV nbrs;
    TIntV dirs;
    for (int j = 0; j < ts_nbr_dir.Len(); j++) {
      timestamps.Add(ts_nbr_dir[j].Key);
      TIntPair nbr_dir = ts_nbr_dir[j].Dat;
      nbrs.Add(nbr_dir.Key);
      dirs.Add(nbr_dir.Dat);
    }
    
    ThreeEventStarCounter tesc(static_graph_->GetMxNId());
    tesc.Count(nbrs, dirs, timestamps, delta);

    #pragma omp critical
    {
      for (int dir1 = 0; dir1 < 2; ++dir1) {
	for (int dir2 = 0; dir2 < 2; ++dir2) {
	  for (int dir3 = 0; dir3 < 2; ++dir3) {
	    pre_counts(dir1, dir2, dir3) += tesc.PreCount(dir1, dir2, dir3);
	    post_counts(dir1, dir2, dir3) += tesc.PostCount(dir1, dir2, dir3);
	    mid_counts(dir1, dir2, dir3) += tesc.MidCount(dir1, dir2, dir3);
	  }
	}
      }
    }
  }

  // Subtract off edge-wise counts
  Counter3D edge_counts;
  ThreeEventEdgeCounts(delta, edge_counts);
  for (int dir1 = 0; dir1 < 2; ++dir1) {
    for (int dir2 = 0; dir2 < 2; ++dir2) {
      for (int dir3 = 0; dir3 < 2; ++dir3) {
	pre_counts(dir1, dir2, dir3)  -= edge_counts(dir1, dir2, dir3);
	post_counts(dir1, dir2, dir3) -= edge_counts(dir1, dir2, dir3);
	mid_counts(dir1, dir2, dir3)  -= edge_counts(dir1, dir2, dir3);
      }
    }
  }
}

void TemporalMotifCounter::ThreeEventEdgeCounts(int u, int v, double delta,
						Counter3D& counts) {
  // Get u --> v and v --> u
  TIntV& ts_uv = temporal_data_[u](v);
  TIntV& ts_vu = temporal_data_[v](u);

  // Sort event list by time
  TVec<TIntPair> combined(ts_uv.Len() + ts_vu.Len());
  int ts_uv_size = ts_uv.Len();
  for (int k = 0; k < ts_uv_size; k++) {
    combined[k] = TIntPair(ts_uv[k], 0);
  }
  for (int k = 0; k < ts_vu.Len(); k++) {
    combined[k + ts_uv_size] = TIntPair(ts_vu[k], 1);
  }
  combined.Sort();

  // Get the counts
  ThreeEventMotifCounter counter(2);
  TIntV in_out(combined.Len());
  TIntV timestamps(combined.Len());
  for (int k = 0; k < combined.Len(); k++) {
    in_out[k] = combined[k].Dat;
    timestamps[k] = combined[k].Key;
  }
  counter.Count(in_out, timestamps, delta);
  counts = counter.Counts();
}

ThreeEventMotifCounter::ThreeEventMotifCounter(int size) {
  size_ = size;
  // Initialize everything to empty
  counts1_ = TIntV(size_);
  counts1_.PutAll(0);
  counts2_ = Counter2D(size_, size_);
  counts3_ = Counter3D(size_, size_, size_);
}

void ThreeEventMotifCounter::Count(const TIntV& event_string,
                                   const TIntV& timestamps,
                                   double delta) {
  if (event_string.Len() != timestamps.Len()) {
    TExcept::Throw("Number of events must equal number of timestamps");
  }
  int start = 0;
  for (int end = 0; end < event_string.Len(); end++) {
    while (double(timestamps[start]) + delta < double(timestamps[end])) {
      DecrementCounts(event_string[start]);
      start++;
    }
    IncrementCounts(event_string[end]);
  }
}

void ThreeEventMotifCounter::IncrementCounts(int event) {
  // Update three-counts
  for (int i = 0; i < size_; i++) {
    for (int j = 0; j < size_; j++) {
      counts3_(i, j, event) += counts2_(i, j);
    }
  }
  // Update two-counts
  for (int i = 0; i < size_; i++) {
    counts2_(i, event) += counts1_[i];
  }
  // Update one-counts
  counts1_[event] += 1;
}

void ThreeEventMotifCounter::DecrementCounts(int event) {
  // Update one-counts
  counts1_[event]--;
  if (counts1_[event] < 0) {
    TExcept::Throw("Bad counts1 value in DecrementCounts()");
  }
  // Update two-counts
  for (int i = 0; i < size_; i++) {
    counts2_(event, i) -= counts1_[i];
    if (counts2_(event, i) < 0) {
      TExcept::Throw("Bad count in DecrementCounts()");
    }
  }
}

ThreeEventStarCounter::ThreeEventStarCounter(int num_nodes) {
  // Initialize node counters
  pre_nodes_ = TVec< TIntV >(2);
  post_nodes_ = TVec< TIntV >(2);
  for (int i = 0; i < 2; i++) {
    pre_nodes_[i] = TIntV(num_nodes);
    post_nodes_[i] = TIntV(num_nodes);
    pre_nodes_[i].PutAll(0);
    post_nodes_[i].PutAll(0);
  }
}

void ThreeEventStarCounter::Count(const TIntV& nbr, const TIntV& dir,
				  const TIntV& timestamps, double delta) {
  if (dir.Len() != timestamps.Len() || dir.Len() != nbr.Len()) {
    TExcept::Throw("nbr, dir, and timestamp vector must be the same size");
  }
  int start = 0;
  int end = 0;
  int L = timestamps.Len();

  for (int j = 0; j < L; j++) {
    double tj = double(timestamps[j]);
    // Adjust counts in pre-window [tj - delta, tj)
    while (start < L && double(timestamps[start]) + delta < tj) {
      PopPre(nbr[start], dir[start]);
      start++;
    }
    // Adjust counts in post-window (tj, tj + delta]
    while (end < L && double(timestamps[end]) < tj + delta) {
      PushPost(nbr[end], dir[end]);
      end++;
    }
    // Move current event off post-window
    PopPost(nbr[j], dir[j]);
    ProcessCurrent(nbr[j], dir[j]);
    PushPre(nbr[j], dir[j]);
  }
}

void ThreeEventStarCounter::PopPre(int nbr, int dir) {
  pre_nodes_[dir][nbr] -= 1;
  assert(pre_nodes_[dir][nbr] >= 0);
  for (int i = 0; i < 2; i++) {
    pre_sum_(dir, i) -= pre_nodes_[i][nbr];
    assert(pre_sum_(dir, i) >= 0);
    mid_sum_(dir, i) -= post_nodes_[i][nbr];
    assert(mid_sum_(dir, i) >= 0);
  }
}

void ThreeEventStarCounter::PopPost(int nbr, int dir) {
  post_nodes_[dir][nbr] -= 1;
  assert(post_nodes_[dir][nbr] >= 0);  
  for (int i = 0; i < 2; i++) {
    post_sum_(dir, i) -= post_nodes_[i][nbr];
    assert(post_sum_(dir, i) >= 0);
    mid_sum_(i, dir) -= pre_nodes_[i][nbr];
    assert(mid_sum_(i, dir) >= 0);
  }
}

void ThreeEventStarCounter::PushPre(int nbr, int dir) {
  for (int i = 0; i < 2; i++) {
    pre_sum_(i, dir) += pre_nodes_[i][nbr];
    mid_sum_(dir, i) += post_nodes_[i][nbr];
  }
  pre_nodes_[dir][nbr] += 1;
}

void ThreeEventStarCounter::PushPost(int nbr, int dir) {
  for (int i = 0; i < 2; i++) {
    post_sum_(i, dir) += post_nodes_[i][nbr];
    mid_sum_(i, dir) += pre_nodes_[i][nbr];
  }
  post_nodes_[dir][nbr] += 1;
}

void ThreeEventStarCounter::ProcessCurrent(int nbr, int dir) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      pre_counts_(i, j, dir)  += pre_sum_(i, j);
      post_counts_(dir, i, j) += post_sum_(i, j);
      mid_counts_(i, dir, j)  += mid_sum_(i, j);
    }
  }
}
