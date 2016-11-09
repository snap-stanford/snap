#include "Snap.h"
#include "temporalmotifs.h"

typedef TKeyDat<TInt, TInt> TIntPair;

///////////////////////////////////////////////////////////////////////////////
// Initialization and helper methods for TempMotifCounter
TempMotifCounter::TempMotifCounter(const TStr& filename) {
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

void TempMotifCounter::GetAllNodes(TIntV& nodes) {
  nodes = TIntV();
  for (TNGraph::TNodeI it = static_graph_->BegNI();
       it < static_graph_->EndNI(); it++) {
    nodes.Add(it.GetId());
  }
}

void TempMotifCounter::GetAllNeighbors(int node, TIntV& nbrs) {
  nbrs = TIntV();
  TNGraph::TNodeI NI = static_graph_->GetNI(node);
  for (int i = 0; i < NI.GetOutDeg(); i++) { nbrs.Add(NI.GetOutNId(i)); }
  for (int i = 0; i < NI.GetInDeg(); i++) {
    int nbr = NI.GetInNId(i);
    if (!NI.IsOutNId(nbr)) { nbrs.Add(nbr); }
  }
}

void TempMotifCounter::Count3TEdge23Node(double delta, Counter2D& counts) {
  counts = Counter2D(6, 6);
  
  Counter2D edge_counts;
  Count3TEdge2Node(delta, edge_counts);
  counts(4, 0) = edge_counts(0, 0);
  counts(4, 1) = edge_counts(0, 1);
  counts(5, 0) = edge_counts(1, 0);
  counts(5, 1) = edge_counts(1, 1);

  Counter3D pre_counts, pos_counts, mid_counts;
  Count3TEdge3NodeStars(delta, pre_counts, pos_counts, mid_counts);
  counts(0, 0) = mid_counts(1, 1, 1);
  counts(0, 1) = mid_counts(1, 1, 0);
  counts(0, 4) = pos_counts(1, 1, 0);
  counts(0, 5) = pos_counts(1, 1, 1);
  counts(1, 0) = mid_counts(1, 0, 1);
  counts(1, 1) = mid_counts(1, 0, 0);
  counts(1, 4) = pos_counts(1, 0, 0);
  counts(1, 5) = pos_counts(1, 0, 1);
  counts(2, 0) = mid_counts(0, 0, 1);
  counts(2, 1) = mid_counts(0, 1, 1);
  counts(2, 2) = pos_counts(0, 1, 0);
  counts(2, 3) = pos_counts(0, 1, 1);
  counts(3, 0) = mid_counts(0, 0, 0);
  counts(3, 1) = mid_counts(0, 0, 1);
  counts(3, 2) = pos_counts(0, 0, 0);
  counts(3, 3) = pos_counts(0, 0, 1);
  counts(4, 2) = pre_counts(0, 1, 0);
  counts(4, 3) = pre_counts(0, 1, 1);
  counts(4, 4) = pre_counts(1, 0, 0);
  counts(4, 5) = pre_counts(1, 0, 1);
  counts(5, 2) = pre_counts(0, 0, 0);
  counts(5, 3) = pre_counts(0, 0, 1);
  counts(5, 4) = pre_counts(1, 1, 0);
  counts(5, 5) = pre_counts(1, 1, 1);  

  Counter3D triad_counts;
  Count3TEdgeTriads(delta, triad_counts);
  counts(0, 2) = triad_counts(0, 0, 0);
  counts(0, 3) = triad_counts(0, 0, 1);
  counts(1, 2) = triad_counts(0, 1, 0);
  counts(1, 3) = triad_counts(0, 1, 1);
  counts(2, 4) = triad_counts(1, 0, 0);
  counts(2, 5) = triad_counts(1, 0, 1);
  counts(3, 4) = triad_counts(1, 1, 0);
  counts(3, 5) = triad_counts(1, 1, 1);
}

void TempMotifCounter::GetAllStaticTriangles(TIntV& Us, TIntV& Vs, TIntV& Ws) {
  // Clear vectors
  Us = TIntV();
  Vs = TIntV();
  Vs = TIntV();

  // Get degree ordering of the graph
  int max_nodes = static_graph_->GetMxNId() + 1;
  TVec<TIntPair> degrees(max_nodes);
  degrees.PutAll(TIntPair(0, 0));
  // Set the degree of a node to be the number of nodes adjacent to the node in
  // the undirected graph.
  for (TNGraph::TNodeI NI = static_graph_->BegNI();
       NI < static_graph_->EndNI(); NI++) {  
    int src = NI.GetId();
    TIntV nbrs;
    GetAllNeighbors(src, nbrs);
    degrees[src] = TIntPair(nbrs.Len(), src);
  }
  degrees.Sort();
  TIntV order = TIntV(max_nodes);
  for (int i = 0; i < order.Len(); ++i) {
    order[degrees[i].Dat] = i;
  }

  // Get triangles centered at a given node where that node is the smallest in
  // the degree ordering.
  for (TNGraph::TNodeI NI = static_graph_->BegNI();
       NI < static_graph_->EndNI(); NI++) {
    int src = NI.GetId();
    int src_pos = order[src];
    
    // Get all neighbors who come later in the ordering
    TIntV neighbors_higher;
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      int nbr = NI.GetOutNId(i);
      if (order[nbr] > src_pos) { neighbors_higher.Add(nbr); }
    }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      int nbr = NI.GetInNId(i);
      if (!NI.IsOutNId(nbr) && order[nbr] > src_pos) { neighbors_higher.Add(nbr); }
    }

    for (int ind1 = 0; ind1 < neighbors_higher.Len(); ind1++) {
      for (int ind2 = ind1 + 1; ind2 < neighbors_higher.Len(); ind2++) {
        int dst1 = neighbors_higher[ind1];
        int dst2 = neighbors_higher[ind2];
        // Check for triangle formation
        if (static_graph_->IsEdge(dst1, dst2) || static_graph_->IsEdge(dst2, dst1)) {
          Us.Add(src);
          Vs.Add(dst1);
          Ws.Add(dst2);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Two-node (static edge) counting methods
void TempMotifCounter::Count3TEdge2Node(double delta, Counter2D& counts) {
  // Get a vector of undirected edges (so we can use openmp parallel for over it)
  TVec<TIntPair> undir_edges;
  for (TNGraph::TEdgeI it = static_graph_->BegEI(); it < static_graph_->EndEI(); it++) {
    int src = it.GetSrcNId();
    int dst = it.GetDstNId();
    // Only consider undirected edges
    if (src < dst || (dst < src && !static_graph_->IsEdge(dst, src))) {
      undir_edges.Add(TIntPair(src, dst));
    }
  }
  counts = Counter2D(2, 2);
  #pragma omp parallel for
  for (int i = 0; i < undir_edges.Len(); i++) {
    TIntPair edge = undir_edges[i];
    // Get the counts for that particular edge
    Counter3D local;
    int src = edge.Key;
    int dst = edge.Dat;
    Count3TEdge2Node(src, dst, delta, local);
    #pragma omp critical
    {
      counts(0, 0) += local(0, 1, 0) + local(1, 0, 1);  // M_{5,1} in paper
      counts(0, 1) += local(1, 0, 0) + local(0, 1, 1);  // M_{5,2}
      counts(1, 0) += local(0, 0, 0) + local(1, 1, 1);  // M_{6,1}
      counts(1, 1) += local(0, 0, 1) + local(1, 1, 0);  // M_{6,2}
    }
  }
}

void TempMotifCounter::Count3TEdge2Node(int u, int v, double delta,
					Counter3D& counts) {
  // Get u --> v and v --> u
  TIntV& ts_uv = temporal_data_[u](v);
  TIntV& ts_vu = temporal_data_[v](u);

  // Sort event list by time
  TVec<TIntPair> combined(ts_uv.Len() + ts_vu.Len());
  int ts_uv_size = ts_uv.Len();
  for (int k = 0; k < ts_uv_size; k++)  { combined[k] = TIntPair(ts_uv[k], 0); }
  for (int k = 0; k < ts_vu.Len(); k++) {
    combined[k + ts_uv_size] = TIntPair(ts_vu[k], 1);
  }
  combined.Sort();

  // Get the counts
  ThreeTEdgeMotifCounter counter(2);
  TIntV in_out(combined.Len());
  TIntV timestamps(combined.Len());
  for (int k = 0; k < combined.Len(); k++) {
    in_out[k] = combined[k].Dat;
    timestamps[k] = combined[k].Key;
  }
  counter.Count(in_out, timestamps, delta, counts);
}

///////////////////////////////////////////////////////////////////////////////
// Star counting methods

void AddStarData(TIntV& ts_vec, TVec<TIntPair>& ts_indices,TVec<StarEdgeData>& events,
		 int& index, int nbr, int key) {
  for (int j = 0; j < ts_vec.Len(); ++j) {
    ts_indices.Add(TIntPair(ts_vec[j], index));
    events.Add(StarEdgeData(nbr, key));
    index++;
  }
}
		 

void TempMotifCounter::Count3TEdge3NodeStars(double delta, Counter3D& pre_counts,
					     Counter3D& pos_counts, Counter3D& mid_counts) {
  // Get a vector of nodes (so we can use openmp parallel for over it)
  TIntV centers;
  GetAllNodes(centers);
  // Get counts for each node as the center
  pre_counts = Counter3D(2, 2, 2);
  pos_counts = Counter3D(2, 2, 2);
  mid_counts = Counter3D(2, 2, 2);
  #pragma omp parallel for
  for (int c = 0; c < centers.Len(); c++) {
    // Gather all adjacent events
    int center = centers[c];
    TVec<TIntPair> ts_indices;
    TVec<StarEdgeData> events;
    TNGraph::TNodeI NI = static_graph_->GetNI(center);
    int index = 0;
    TIntV nbrs;
    GetAllNeighbors(center, nbrs);
    for (int i = 0; i < nbrs.Len(); i++) {
      int nbr = nbrs[i];
      AddStarData(temporal_data_[center](nbr), ts_indices, events, index, nbr, 0);
      AddStarData(temporal_data_[nbr](center), ts_indices, events, index, nbr, 1);      
    }
    ts_indices.Sort();
    TIntV timestamps;
    TVec<StarEdgeData> ordered_events;
    for (int j = 0; j < ts_indices.Len(); j++) {
      timestamps.Add(ts_indices[j].Key);
      ordered_events.Add(events[ts_indices[j].Dat]);
    }
    
    ThreeTEdgeStarCounter tesc(static_graph_->GetMxNId());
    // dirs: outgoing --> 0, incoming --> 1
    tesc.Count(ordered_events, timestamps, delta);
    #pragma omp critical
    { // Update counts
      for (int dir1 = 0; dir1 < 2; ++dir1) {
        for (int dir2 = 0; dir2 < 2; ++dir2) {
          for (int dir3 = 0; dir3 < 2; ++dir3) {
            pre_counts(dir1, dir2, dir3) += tesc.PreCount(dir1, dir2, dir3);
            pos_counts(dir1, dir2, dir3) += tesc.PosCount(dir1, dir2, dir3);
            mid_counts(dir1, dir2, dir3) += tesc.MidCount(dir1, dir2, dir3);
          }
        }
      }
    }

    // Subtract off edge-wise counts
    for (int nbr_id = 0; nbr_id < nbrs.Len(); nbr_id++) {
      int nbr = nbrs[nbr_id];
      Counter3D edge_counts;
      Count3TEdge2Node(center, nbr, delta, edge_counts);
      #pragma omp critical
      {
        for (int dir1 = 0; dir1 < 2; ++dir1) {
          for (int dir2 = 0; dir2 < 2; ++dir2) {
            for (int dir3 = 0; dir3 < 2; ++dir3) {
              pre_counts(dir1, dir2, dir3) -= edge_counts(dir1, dir2, dir3);
              pos_counts(dir1, dir2, dir3) -= edge_counts(dir1, dir2, dir3);
              mid_counts(dir1, dir2, dir3) -= edge_counts(dir1, dir2, dir3);
            }
          }
        }
      }
    }
  }
}

void AddEvents(TVec<TIntPair>& combined, TIntV& timestamps, int value) {
  for (int i = 0; i < timestamps.Len(); i++) {
    combined.Add(TIntPair(timestamps[i], value));
  }
}

void TempMotifCounter::Count3TEdge3NodeStarsNaive(
        double delta, Counter3D& pre_counts, Counter3D& pos_counts,
        Counter3D& mid_counts) {
  // Get all nodes (so we can use openmp parallel for over it)
  TIntV centers;
  GetAllNodes(centers);
  // Get counts for each node as the center
  pre_counts = Counter3D(2, 2, 2);
  pos_counts = Counter3D(2, 2, 2);
  mid_counts = Counter3D(2, 2, 2);
  #pragma omp parallel for
  for (int c = 0; c < centers.Len(); c++) {
    // Gather all adjacent events
    int center = centers[c];
    TIntV nbrs;
    GetAllNeighbors(center, nbrs);
    for (int i = 0; i < nbrs.Len(); i++) {
      for (int j = i + 1; j < nbrs.Len(); j++) {
        int nbr1 = nbrs[i];
        int nbr2 = nbrs[j];
        TVec<TIntPair> combined;
	AddEvents(combined, temporal_data_[center](nbr1), 0);
        AddEvents(combined, temporal_data_[nbr1](center), 1);
	AddEvents(combined, temporal_data_[center](nbr2), 2);
	AddEvents(combined, temporal_data_[nbr2](center), 3);
        combined.Sort();
        ThreeTEdgeMotifCounter counter(4);
        TIntV edge_id(combined.Len());
        TIntV timestamps(combined.Len());
        for (int k = 0; k < combined.Len(); k++) {
          edge_id[k] = combined[k].Dat;
          timestamps[k] = combined[k].Key;
        }
        Counter3D local;
        counter.Count(edge_id, timestamps, delta, local);

        #pragma omp critical
        {  // Update with local counts
          for (int dir1 = 0; dir1 < 2; ++dir1) {
            for (int dir2 = 0; dir2 < 2; ++dir2) {
              for (int dir3 = 0; dir3 < 2; ++dir3) {
                pre_counts(dir1, dir2, dir3) +=
		  local(dir1, dir2, dir3 + 2) + local(dir1 + 2, dir2 + 2, dir3);
                pos_counts(dir1, dir2, dir3) +=
		  local(dir1, dir2 + 2, dir3 + 2) + local(dir1 + 2, dir2, dir3);
                mid_counts(dir1, dir2, dir3) +=
		  local(dir1, dir2 + 2, dir3) + local(dir1 + 2, dir2, dir3 + 2);
              }
            }
          }
        }
      }
    }
  }
}

void TempMotifCounter::Count3TEdgeTriadsNaive(double delta, Counter3D& counts) {
  TIntV Us, Vs, Ws;
  GetAllStaticTriangles(Us, Vs, Ws);

  counts = Counter3D(2, 2, 2);
  #pragma omp parallel for
  for (int i = 0; i < Us.Len(); i++) {
    int u = Us[i];
    int v = Vs[i];
    int w = Ws[i];

    // Gather all edges in triangle (u, v, w)
    int uv = 0;
    int vu = 1;
    int uw = 2;
    int wu = 3;
    int vw = 4;
    int wv = 5;
    TVec<TIntPair> combined;
    TIntV& ts_uv = temporal_data_[u](v);
    for (int k = 0; k < ts_uv.Len(); k++) { combined.Add(TIntPair(ts_uv[k], uv)); }
    TIntV& ts_vu = temporal_data_[v](u);
    for (int k = 0; k < ts_vu.Len(); k++) { combined.Add(TIntPair(ts_vu[k], vu)); }
    TIntV& ts_uw = temporal_data_[u](w);
    for (int k = 0; k < ts_uw.Len(); k++) { combined.Add(TIntPair(ts_uw[k], uw)); }
    TIntV& ts_wu = temporal_data_[w](u);
    for (int k = 0; k < ts_wu.Len(); k++) { combined.Add(TIntPair(ts_wu[k], wu)); }
    TIntV& ts_vw = temporal_data_[v](w);
    for (int k = 0; k < ts_vw.Len(); k++) { combined.Add(TIntPair(ts_vw[k], vw)); }
    TIntV& ts_wv = temporal_data_[w](v);
    for (int k = 0; k < ts_wv.Len(); k++) { combined.Add(TIntPair(ts_wv[k], wv)); }
    
    // Get the counts for this triangle
    combined.Sort();
    ThreeTEdgeMotifCounter counter(6);
    TIntV edge_id(combined.Len());
    TIntV timestamps(combined.Len());
    for (int k = 0; k < combined.Len(); k++) {
      edge_id[k] = combined[k].Dat;
      timestamps[k] = combined[k].Key;
    }
    Counter3D local;
    counter.Count(edge_id, timestamps, delta, local);

    // Update the global counter with the various symmetries
    #pragma omp critical
    {
      // i --> j, k --> j, i --> k
      counts(0, 0, 0) += local(uv, wv, uw) + local(vu, wu, vw) + local(uw, vw, uv)
        + local(wu, vu, wv) + local(vw, uw, vu) + local(wv, uv, wu);
      // i --> j, k --> j, k --> i
      counts(0, 0, 1) += local(uv, wv, wu) + local(vu, wu, wv) + local(uw, vw, vu)
        + local(wu, vu, vw) + local(vw, uw, uv) + local(wv, uv, uw);
      // i --> j, j --> k, i --> k
      counts(0, 1, 0) += local(uv, vw, uw) + local(vu, uw, vw) + local(uw, wv, uv)
        + local(wu, uv, wv) + local(vw, wu, vu) + local(wv, vu, wu);
      // i --> j, j --> k, k --> i
      counts(0, 1, 1) += local(uv, vw, wu) + local(vu, uw, wv) + local(uw, wv, vu)
        + local(wu, uv, vw) + local(vw, wu, uv) + local(wv, vu, uw);
      // i --> j, k --> i, j --> k
      counts(1, 0, 0) += local(uv, wu, vw) + local(vu, wv, uw) + local(uw, vu, wv)
        + local(wu, vw, uv) + local(vw, uv, wu) + local(wv, uw, vu);
      // i --> j, k --> i, k --> j
      counts(1, 0, 1) += local(uv, wu, wv) + local(vu, wv, wu) + local(uw, vu, vw)
        + local(wu, vw, vu) + local(vw, uv, uw) + local(wv, uw, uv);
      // i --> j, i --> k, j --> k
      counts(1, 1, 0) += local(uv, uw, vw) + local(vu, vw, uw) + local(uw, uv, wv)
        + local(wu, wv, uv) + local(vw, vu, wu) + local(wv, wu, vu);      
      // i --> j, i --> k, k --> j
      counts(1, 1, 1) += local(uv, uw, wv) + local(vu, vw, wu) + local(uw, uv, vw)
        + local(wu, wv, vu) + local(vw, vu, uw) + local(wv, wu, uv);
    }
  }
}

void AddTriadEdgeData(TVec<TriadEdgeData>& events, TVec<TIntPair>& ts_indices,
		    TIntV& timestamps, int& index, int nbr, int key1, int key2) {
  for (int i = 0; i < timestamps.Len(); i++) {
    ts_indices.Add(TIntPair(timestamps[i], index));
    events.Add(TriadEdgeData(nbr, key1, key2));
    ++index;
  }
}

void TempMotifCounter::Count3TEdgeTriads(double delta, Counter3D& counts) {
  counts = Counter3D(2, 2, 2);

  // Get the counts on each undirected edge
  TVec< THash<TInt, TInt> > edge_counts(temporal_data_.Len());
  for (TNGraph::TEdgeI it = static_graph_->BegEI();
       it < static_graph_->EndEI(); it++) {
    int src = it.GetSrcNId();
    int dst = it.GetDstNId();
    edge_counts[MIN(src, dst)](MAX(src, dst)) += temporal_data_[src](dst).Len();
  }
  
  // Assign triangles to the edge with the most events
  TVec< THash<TInt, TIntV> > assignments(temporal_data_.Len());
  TIntV Us, Vs, Ws;
  GetAllStaticTriangles(Us, Vs, Ws);
  for (int i = 0; i < Us.Len(); i++) {
    int u = Us[i];
    int v = Vs[i];
    int w = Ws[i];
    int counts_uv = edge_counts[MIN(u, v)](MAX(u, v));
    int counts_uw = edge_counts[MIN(u, w)](MAX(u, w));
    int counts_vw = edge_counts[MIN(v, w)](MAX(v, w));
    if        (counts_uv >= MAX(counts_uw, counts_vw)) {
      assignments[MIN(u, v)](MAX(u, v)).Add(w);
    } else if (counts_uw >= MAX(counts_uv, counts_vw)) {
      assignments[MIN(u, w)](MAX(u, w)).Add(v);
    } else if (counts_vw >= MAX(counts_uv, counts_uw)) {
      assignments[MIN(v, w)](MAX(v, w)).Add(u);
    }
  }

  TIntV all_nodes;
  GetAllNodes(all_nodes);
  // Count triangles on edges with the assigned neighbors
  for (int node_id = 0; node_id < all_nodes.Len(); node_id++) {
    TIntV nbrs;
    int u = all_nodes[node_id];
    GetAllNeighbors(u, nbrs);

    #pragma omp parallel for
    for (int nbr_id = 0; nbr_id < nbrs.Len(); nbr_id++) {
      int v = nbrs[nbr_id];
      TIntV& uv_assignment = assignments[u](v);
      // Skip if no triangles were assigned to this edge
      if (uv_assignment.Len() == 0) { continue; }
      // Get all events on (u, v)
      TVec<TriadEdgeData> events;
      TVec<TIntPair> ts_indices;
      int index = 0;
      AddTriadEdgeData(events, ts_indices, temporal_data_[u](v), index, u, 0, 1);
      AddTriadEdgeData(events, ts_indices, temporal_data_[v](u), index, v, 0, 0);
      // Get all events on triangles assigned to (u, v)
      for (int w_id = 0; w_id < uv_assignment.Len(); w_id++) {
        int w = uv_assignment[w_id];
	AddTriadEdgeData(events, ts_indices, temporal_data_[w](u), index, w, 0, 0);
	AddTriadEdgeData(events, ts_indices, temporal_data_[w](v), index, w, 0, 1);
	AddTriadEdgeData(events, ts_indices, temporal_data_[u](w), index, w, 1, 0);
	AddTriadEdgeData(events, ts_indices, temporal_data_[v](w), index, w, 1, 1);
      }

      // Put events in sorted order
      ts_indices.Sort();
      TIntV timestamps(ts_indices.Len());
      TVec<TriadEdgeData> sorted_events(ts_indices.Len());
      for (int i = 0; i < ts_indices.Len(); i++) {
        timestamps[i] = ts_indices[i].Key;
        sorted_events[i] = events[ts_indices[i].Dat];
      }

      // Get the counts and update the counter
      ThreeTEdgeTriadCounter tetc(static_graph_->GetMxNId(), u, v);
      tetc.Count(sorted_events, timestamps, delta);
      #pragma omp critical
      {
        for (int dir1 = 0; dir1 < 2; dir1++) {
          for (int dir2 = 0; dir2 < 2; dir2++) {
            for (int dir3 = 0; dir3 < 2; dir3++) {        
              counts(dir1, dir2, dir3) += tetc.Counts(dir1, dir2, dir3);
            }
          }
        }
      }
    }
  }
}

ThreeTEdgeMotifCounter::ThreeTEdgeMotifCounter(int size) {
  size_ = size;
}

void ThreeTEdgeMotifCounter::Count(const TIntV& event_string, const TIntV& timestamps,
				  double delta, Counter3D& counts) {
  // Initialize everything to empty
  counts1_ = Counter1D(size_);
  counts2_ = Counter2D(size_, size_);
  counts3_ = Counter3D(size_, size_, size_);
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
  counts = counts3_;
}

void ThreeTEdgeMotifCounter::IncrementCounts(int event) {
  for (int i = 0; i < size_; i++) {
    for (int j = 0; j < size_; j++) {
      counts3_(i, j, event) += counts2_(i, j);
    }
  }
  for (int i = 0; i < size_; i++) { counts2_(i, event) += counts1_(i); }
  counts1_(event) += 1;
}

void ThreeTEdgeMotifCounter::DecrementCounts(int event) {
  counts1_(event)--;
  for (int i = 0; i < size_; i++) { counts2_(event, i) -= counts1_(i); }
}

template <typename EdgeData>
void StarTriad3TEdgeCounter<EdgeData>::Count(const TVec<EdgeData>& events,
					     const TIntV& timestamps, double delta) {
  InitializeCounters();
  if (events.Len() != timestamps.Len()) {
    TExcept::Throw("Number of events must match number of timestamps.");
  }
  int start = 0;
  int end = 0;
  int L = timestamps.Len();
  for (int j = 0; j < L; j++) {
    double tj = double(timestamps[j]);
    // Adjust counts in pre-window [tj - delta, tj)
    while (start < L && double(timestamps[start]) < tj - delta) {
      PopPre(events[start]);
      start++;
    }
    // Adjust counts in post-window (tj, tj + delta]
    while (end < L && double(timestamps[end]) <= tj + delta) {
      PushPos(events[end]);
      end++;
    }
    // Move current event off post-window
    PopPos(events[j]);
    ProcessCurrent(events[j]);
    PushPre(events[j]);
  }
}

// Star counter
void ThreeTEdgeStarCounter::InitializeCounters() {
  pre_sum_ = Counter2D(2, 2);
  pos_sum_ = Counter2D(2, 2);
  mid_sum_ = Counter2D(2, 2);
  pre_counts_ = Counter3D(2, 2, 2);
  pos_counts_ = Counter3D(2, 2, 2);
  mid_counts_ = Counter3D(2, 2, 2);
  pre_nodes_ = Counter2D(2, max_nodes_);
  pos_nodes_ = Counter2D(2, max_nodes_);
}

void ThreeTEdgeStarCounter::PopPre(StarEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  pre_nodes_(dir, nbr) -= 1;
  for (int i = 0; i < 2; i++) { pre_sum_(dir, i) -= pre_nodes_(i, nbr); }
}

void ThreeTEdgeStarCounter::PopPos(StarEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;  
  pos_nodes_(dir, nbr) -= 1;
  for (int i = 0; i < 2; i++) { pos_sum_(dir, i) -= pos_nodes_(i, nbr); }
}

void ThreeTEdgeStarCounter::PushPre(StarEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;  
  for (int i = 0; i < 2; i++) { pre_sum_(i, dir) += pre_nodes_(i, nbr); }
  pre_nodes_(dir, nbr) += 1;
}

void ThreeTEdgeStarCounter::PushPos(StarEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;  
  for (int i = 0; i < 2; i++) { pos_sum_(i, dir) += pos_nodes_(i, nbr); }
  pos_nodes_(dir, nbr) += 1;
}

void ThreeTEdgeStarCounter::ProcessCurrent(StarEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  // Decrement middle sum
  for (int i = 0; i < 2; i++) { mid_sum_(i, dir) -= pre_nodes_(i, nbr); }
  // Update counts
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      pre_counts_(i, j, dir) += pre_sum_(i, j);
      pos_counts_(dir, i, j) += pos_sum_(i, j);
      mid_counts_(i, dir, j) += mid_sum_(i, j);
    }
  }
  // Increment middle sum
  for (int i = 0; i < 2; i++) { mid_sum_(dir, i) += pos_nodes_(i, nbr); }
}

///////////////////////////////////
// Triangle counters
void ThreeTEdgeTriadCounter::InitializeCounters() {
  pre_nodes_ = Counter3D(max_nodes_, 2, 2);
  pos_nodes_ = Counter3D(max_nodes_, 2, 2);
  pre_sum_ = Counter3D(2, 2, 2);
  pos_sum_ = Counter3D(2, 2, 2);
  mid_sum_ = Counter3D(2, 2, 2);
  triad_counts_ = Counter3D(2, 2, 2);
}

void ThreeTEdgeTriadCounter::PopPre(TriadEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  int u_or_v = event.u_or_v;
  if (!IsEdgeNode(nbr)) {
    pre_nodes_(nbr, u_or_v, dir) -= 1;
    for (int i = 0; i < 2; i++) {
      pre_sum_(u_or_v, dir, i) -= pre_nodes_(nbr, 1 - u_or_v, i);
    }
  }
}

void ThreeTEdgeTriadCounter::PopPos(TriadEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  int u_or_v = event.u_or_v;  
  if (!IsEdgeNode(nbr)) {  
    pos_nodes_(nbr, u_or_v, dir) -= 1;
    for (int i = 0; i < 2; i++) {
      pos_sum_(u_or_v, dir, i) -= pos_nodes_(nbr, 1 - u_or_v, i);
    }
  }
}

void ThreeTEdgeTriadCounter::PushPre(TriadEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  int u_or_v = event.u_or_v;  
  if (!IsEdgeNode(nbr)) {  
    for (int i = 0; i < 2; i++) {
      pre_sum_(1 - u_or_v, i, dir) += pre_nodes_(nbr, 1 - u_or_v, i);
    }
    pre_nodes_(nbr, u_or_v, dir) += 1;
  }
}

void ThreeTEdgeTriadCounter::PushPos(TriadEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  int u_or_v = event.u_or_v;
  if (!IsEdgeNode(nbr)) {
    for (int i = 0; i < 2; i++) {
      pos_sum_(1 - u_or_v, i, dir) += pos_nodes_(nbr, 1 - u_or_v, i);
    }
    pos_nodes_(nbr, u_or_v, dir) += 1;
  }
}

void ThreeTEdgeTriadCounter::ProcessCurrent(TriadEdgeData event) {
  int nbr = event.nbr;
  int dir = event.dir;
  int u_or_v = event.u_or_v;  
  // Adjust middle sums
  if (!IsEdgeNode(nbr)) {
    for (int i = 0; i < 2; i++) {
      mid_sum_(1 - u_or_v, i, dir) -= pre_nodes_(nbr, 1 - u_or_v, i);
    }
    for (int i = 0; i < 2; i++) {
      mid_sum_(u_or_v, dir, i) += pos_nodes_(nbr, 1 - u_or_v, i);
    }
  }
  // Update counts
  if (IsEdgeNode(nbr)) {
    // Determine if the event edge is u --> v or v --> u
    int u_to_v = 0;
    if (((nbr == node_u_) && dir == 0) || ((nbr == node_v_) && dir == 1)) {
      u_to_v = 1;
    }
    // i --> j, k --> j, i --> k    
    triad_counts_(0, 0, 0) += mid_sum_(u_to_v,     0, 0)
                           +  pos_sum_(u_to_v,     0, 1)
                           +  pre_sum_(1 - u_to_v, 1, 1);
    // i --> j, k --> j, k --> i
    triad_counts_(0, 0, 1) += mid_sum_(u_to_v,     0, 1)
                           +  pos_sum_(u_to_v,     0, 0)
                           +  pre_sum_(u_to_v,     1, 1);
    // i --> j, j --> k, i --> k
    triad_counts_(0, 1, 0) += mid_sum_(1 - u_to_v, 0, 0)
                           +  pos_sum_(u_to_v,     1, 1)
                           +  pre_sum_(1 - u_to_v, 1, 0);
    // i --> j, j --> k, k --> i
    triad_counts_(0, 1, 1) += mid_sum_(1 - u_to_v, 0, 1)
                           +  pos_sum_(u_to_v,     1, 0)
                           +  pre_sum_(u_to_v,     1, 0);
    // i --> j, k --> i, j --> k
    triad_counts_(1, 0, 0) += mid_sum_(u_to_v,     1, 0)
                           +  pos_sum_(1 - u_to_v, 0, 1)
                           +  pre_sum_(1 - u_to_v, 0, 1);
    // i --> j, k --> i, k --> j
    triad_counts_(1, 0, 1) += mid_sum_(u_to_v,     1, 1)
                           +  pos_sum_(1 - u_to_v, 0, 0)
                           +  pre_sum_(u_to_v,     0, 1);
    // i --> j, i --> k, j --> k
    triad_counts_(1, 1, 0) += mid_sum_(1 - u_to_v, 1, 0)
                           +  pos_sum_(1 - u_to_v, 1, 1)
                           +  pre_sum_(1 - u_to_v, 0, 0);
    // i --> j, i --> k, k --> j
    triad_counts_(1, 1, 1) += mid_sum_(1 - u_to_v, 1, 1)
                           +  pos_sum_(1 - u_to_v, 1, 0)
                           +  pre_sum_(u_to_v,     0, 0);
  }
}
