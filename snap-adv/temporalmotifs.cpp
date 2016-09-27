#include "Snap.h"
#include "temporalmotifs.h"

void TemporalGraph::LoadData(const TStr& filename) {
  // First load the static graph
  static_graph_ = TSnap::LoadEdgeList<PNGraph>(filename, 0, 1);
  max_nodes_ = static_graph_->GetMxNId();
  temporal_data_ = TVec< THash<TInt, TIntV> >(max_nodes_);

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
    temporal_data_[dst](src).Add(tim);
  }
}

ThreeEventMotifCounter::ThreeEventMotifCounter(int size) {
  size_ = size;
  // Initialize everything to empty
  counts1_ = TIntV(size_);
  counts1_.PutAll(0);
  counts2_ = TVec< TIntV >(size_);
  counts3_ = TVec< TVec< TIntV > >(size_);
  for (int i = 0; i < size_; i++) {
    counts2_[i] = TIntV(size_);
    counts2_[i].PutAll(0);
    counts3_[i] = TVec< TIntV >(size_);
    for (int j = 0; j < size_; j++) {
      counts3_[i][j] = TIntV(size_);
      counts3_[i][j].PutAll(0);
    }
  }
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
      counts3_[i][j][event] += counts2_[i][j];
    }
  }
  // Update two-counts
  for (int i = 0; i < size_; i++) {
    counts2_[i][event] += counts1_[i];
  }
  // Update one-counts
  counts1_[event] += 1;
}

void ThreeEventMotifCounter::DecrementCounts(int event) {
  // Update one-counts
  counts1_[event]--;
  if (!(counts1_[event] >= 0)) {
    TExcept::Throw("Bad counts1 value in DecrementCounts()");
  }
  // Update two-counts
  for (int i = 0; i < size_; i++) {
    counts2_[event][i] -= counts1_[i];
    if (!(counts2_[event][i] >= 0)) {
      TExcept::Throw("Bad count in DecrementCounts()");
    }
  }
}
