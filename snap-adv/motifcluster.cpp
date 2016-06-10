#include "stdafx.h"
#include "motifcluster.h"

// Increments weight on (i, j) by one
static void IncrementWeight(int i, int j, TVec< THash<TInt, TInt> >& weights) {
  int minval = MIN(i, j);
  int maxval = MAX(i, j);
  THash<TInt, TInt>& edge_weights = weights[minval];
  edge_weights(maxval) += 1;
}


/////////////////////////////////////////////////
// Subgraph type checking
bool MotifCluster::IsNoEdge(PNGraph graph, int u, int v) {
  return !graph->IsEdge(u, v) && !graph->IsEdge(v, u);
}

bool MotifCluster::IsUnidirEdge(PNGraph graph, int u, int v) {
  return graph->IsEdge(u, v) && !graph->IsEdge(v, u);
}

bool MotifCluster::IsBidirEdge(PNGraph graph, int u, int v) {
  return graph->IsEdge(u, v) && graph->IsEdge(v, u);
}

bool MotifCluster::IsMotifM1(PNGraph graph, int u, int v, int w) {
  return ((IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, v, w) && IsUnidirEdge(graph, w, u)) ||
          (IsUnidirEdge(graph, u, w) && IsUnidirEdge(graph, w, v) && IsUnidirEdge(graph, v, u)));
}

bool MotifCluster::IsMotifM2(PNGraph graph, int u, int v, int w) {
  return ((IsBidirEdge(graph, u, v) && IsUnidirEdge(graph, u, w) && IsUnidirEdge(graph, w, v)) ||
          (IsBidirEdge(graph, u, v) && IsUnidirEdge(graph, w, u) && IsUnidirEdge(graph, v, w)) ||
          (IsBidirEdge(graph, u, w) && IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, v, w)) ||
          (IsBidirEdge(graph, u, w) && IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, w, v)) ||
          (IsBidirEdge(graph, v, w) && IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, u, w)) ||
          (IsBidirEdge(graph, v, w) && IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, w, u)));
}

bool MotifCluster::IsMotifM3(PNGraph graph, int u, int v, int w) {
  if ((IsBidirEdge( graph, u, v) && IsBidirEdge( graph, v, w)) &&
      (IsUnidirEdge(graph, u, w) || IsUnidirEdge(graph, w, u))) { return true; }
  if ((IsBidirEdge( graph, u, w) && IsBidirEdge( graph, w, v)) &&
      (IsUnidirEdge(graph, u, v) || IsUnidirEdge(graph, v, u))) { return true; }
  if ((IsBidirEdge( graph, w, u) && IsBidirEdge( graph, u, v)) &&
      (IsUnidirEdge(graph, w, v) || IsUnidirEdge(graph, v, w))) { return true; }
  return false;
}

bool MotifCluster::IsMotifM4(PNGraph graph, int u, int v, int w) {
  return IsBidirEdge(graph, u, v) && IsBidirEdge(graph, u, w) && IsBidirEdge(graph, v, w);
}

bool MotifCluster::IsMotifM5(PNGraph graph, int u, int v, int w) {
  if ((IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, u, w)) && 
      (IsUnidirEdge(graph, v, w) || IsUnidirEdge(graph, w, v))) { return true; }
  if ((IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, v, w)) && 
      (IsUnidirEdge(graph, u, w) || IsUnidirEdge(graph, w, u))) { return true; }
  if ((IsUnidirEdge(graph, w, v) && IsUnidirEdge(graph, w, u)) && 
      (IsUnidirEdge(graph, v, u) || IsUnidirEdge(graph, u, v))) { return true; }
  return false;
}

bool MotifCluster::IsMotifM6(PNGraph graph, int u, int v, int w) {
  return ((IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, u, w) && IsBidirEdge(graph, v, w)) ||
          (IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, v, w) && IsBidirEdge(graph, u, w)) ||
          (IsUnidirEdge(graph, w, u) && IsUnidirEdge(graph, w, v) && IsBidirEdge(graph, u, v)));
}

bool MotifCluster::IsMotifM7(PNGraph graph, int u, int v, int w) {
  return ((IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, w, u) && IsBidirEdge(graph, v, w)) ||
          (IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, w, v) && IsBidirEdge(graph, u, w)) ||
          (IsUnidirEdge(graph, u, w) && IsUnidirEdge(graph, v, w) && IsBidirEdge(graph, u, v)));
}

bool MotifCluster::IsMotifM8(PNGraph graph, int center, int v, int w) {
  return IsNoEdge(graph, v, w) && IsUnidirEdge(graph, center, v) && IsUnidirEdge(graph, center, w);
}

bool MotifCluster::IsMotifM9(PNGraph graph, int center, int v, int w) {
  return IsNoEdge(graph, v, w) && ((IsUnidirEdge(graph, center, v) && IsUnidirEdge(graph, w, center)) ||
                                   (IsUnidirEdge(graph, center, w) && IsUnidirEdge(graph, center, v)));
}

bool MotifCluster::IsMotifM10(PNGraph graph, int center, int v, int w) {
  return IsNoEdge(graph, v, w) && IsUnidirEdge(graph, v, center) && IsUnidirEdge(graph, w, center);
}

bool MotifCluster::IsMotifM11(PNGraph graph, int center, int v, int w) {
  return IsNoEdge(graph, v, w) && ((IsBidirEdge(graph, center, v) && IsUnidirEdge(graph, center, w)) ||
                                   (IsBidirEdge(graph, center, w) && IsUnidirEdge(graph, center, v)));
}

bool MotifCluster::IsMotifM12(PNGraph graph, int center, int v, int w) {
  return IsNoEdge(graph, v, w) && ((IsBidirEdge(graph, center, v) && IsUnidirEdge(graph, w, center)) ||
                                   (IsBidirEdge(graph, center, w) && IsUnidirEdge(graph, v, center)));
}

bool MotifCluster::IsMotifM13(PNGraph graph, int center, int v, int w) {
  return IsNoEdge(graph, v, w) && IsBidirEdge(graph, center, v) && IsBidirEdge(graph, center, w);
}

/////////////////////////////////////////////////
// Triangle weighting
void MotifCluster::DegreeOrdering(PNGraph graph, TIntV& order) {
  // Note: this works the best when the nodes are numbered 0, 1, ..., num_nodes - 1.
  int max_nodes = graph->GetMxNId() + 1;
  TVec< TKeyDat<TInt, TInt> > degrees(max_nodes);
  degrees.PutAll(TKeyDat<TInt, TInt>(0, 0));
  // Set the degree of a node to be the number of nodes adjacent to the node in
  // the undirected graph.
  for (TNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {
    int src = NI.GetId();
    int num_nbrs = NI.GetOutDeg();
    // For each incoming edge that is not an outgoing edge, include it.
    for (int i = 0; i < NI.GetInDeg(); ++i) {
      int dst = NI.GetInNId(i);
      if (!NI.IsOutNId(dst)) {
        ++num_nbrs;
      }
    }
    degrees[src] = TKeyDat<TInt, TInt>(num_nbrs, src);
  }

  degrees.Sort();
  order = TIntV(max_nodes);
  for (int i = 0; i < order.Len(); ++i) {
    order[degrees[i].Dat] = i;
  }
}

void MotifCluster::TriangleMotifAdjacency(PNGraph graph, MotifType motif,
                                          TVec< THash<TInt, TInt> >& weights) {
  TIntV order;
  DegreeOrdering(graph, order);
  for (TNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {
    int src = NI.GetId();
    int src_pos = order[src];
    
    // Get all neighbors who come later in the ordering
    TIntV neighbors_higher;
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      int nbr = NI.GetOutNId(i);
      if (order[nbr] > src_pos) {
        neighbors_higher.Add(nbr);
      }
    }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      int nbr = NI.GetInNId(i);
      if (!NI.IsOutNId(nbr) && order[nbr] > src_pos) {
        neighbors_higher.Add(nbr);
      }
    }

    for (int ind1 = 0; ind1 < neighbors_higher.Len(); ind1++) {
      for (int ind2 = ind1 + 1; ind2 < neighbors_higher.Len(); ind2++) {
        int dst1 = neighbors_higher[ind1];
        int dst2 = neighbors_higher[ind2];
        // Check for triangle formation
        if (graph->IsEdge(dst1, dst2) || graph->IsEdge(dst2, dst1)) {
          bool motif_occurs = false;
          switch (motif) {
          case M1:
            motif_occurs = IsMotifM1(graph, src, dst1, dst2);
            break;
          case M2:
            motif_occurs = IsMotifM2(graph, src, dst1, dst2);
            break;
          case M3:
            motif_occurs = IsMotifM3(graph, src, dst1, dst2);
            break;
          case M4:
            motif_occurs = IsMotifM4(graph, src, dst1, dst2);
            break;
          case M5:
            motif_occurs = IsMotifM5(graph, src, dst1, dst2);
            break;
          case M6:
            motif_occurs = IsMotifM6(graph, src, dst1, dst2);
            break;
          case M7:
            motif_occurs = IsMotifM7(graph, src, dst1, dst2);
            break;
          default:
            TExcept::Throw("Unknown directed triangle motif");
          }
          // Increment weights of the triad (src, dst1, dst2) if it occurs.
          if (motif_occurs) {
            IncrementWeight(src,  dst1, weights);
            IncrementWeight(src,  dst2, weights);
            IncrementWeight(dst1, dst2, weights);
          }
        }
      }
    }
  }
}

/////////////////////////////////////////////////
// Wedge weighting
void MotifCluster::WedgeMotifAdjacency(PNGraph graph, MotifType motif,
                                       TVec< THash<TInt, TInt> >& weights) {
  for (TNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {
    int center = NI.GetId();
    
    // Get all neighbors
    TIntV neighbors;
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      int nbr = NI.GetOutNId(i);
      neighbors.Add(nbr);
    }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      int nbr = NI.GetInNId(i);
      if (!NI.IsOutNId(nbr)) {
        neighbors.Add(nbr);
      }
    }

    for (int ind1 = 0; ind1 < neighbors.Len(); ind1++) {
      for (int ind2 = ind1 + 1; ind2 < neighbors.Len(); ind2++) {
        int dst1 = neighbors[ind1];
        int dst2 = neighbors[ind2];
        bool motif_occurs = false;
        switch (motif) {
        case M8:
          motif_occurs = IsMotifM8(graph, center, dst1, dst2);
          break;
        case M9:
          motif_occurs = IsMotifM9(graph, center, dst1, dst2);
          break;
        case M10:
          motif_occurs = IsMotifM10(graph, center, dst1, dst2);
          break;
        case M11:
          motif_occurs = IsMotifM11(graph, center, dst1, dst2);
          break;
        case M12:
          motif_occurs = IsMotifM12(graph, center, dst1, dst2);
          break;
        case M13:
          motif_occurs = IsMotifM13(graph, center, dst1, dst2);
          break;
        default:
          TExcept::Throw("Unknown directed wedge motif");
        }
        // Increment weights of (center, dst1, dst2) if it occurs.
        if (motif_occurs) {
          IncrementWeight(center, dst1, weights);
          IncrementWeight(center, dst2, weights);
          IncrementWeight(dst1,   dst2, weights);
        }
      }
    }
  }
}


/////////////////////////////////////////////////
// Bifan weighting
void MotifCluster::BifanMotifAdjacency(PNGraph graph,
                                       TVec< THash<TInt, TInt> >& weights) {
  // Find all pairs of nodes that are not adjacent
  // Note: does not scale to large sparse networks but will work for smaller
  // networks such as common neuronal connectivity datasets.
  TIntV node_ids;
  for (TNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {
    node_ids.Add(NI.GetId());
  }
  for (int i = 0; i < node_ids.Len(); i++) {
    for (int j = i + 1; j < node_ids.Len(); j++) {
      int src1 = node_ids[i];
      int src2 = node_ids[j];
      if (IsNoEdge(graph, src1, src2)) {
        // All unidirectional out-neighbors of src1
        THash<TInt, TInt> nbr_counts;
        auto NI1 = graph->GetNI(src1);
        for (int k = 0; k < NI1.GetOutDeg(); k++) {
          int nbr = NI1.GetOutNId(k);
          if (IsUnidirEdge(graph, src1, nbr)) {
            nbr_counts(nbr) += 1;
          }
        }

        // All unidirectional out-neighbors of src2
        auto NI2 = graph->GetNI(src2);
        for (int k = 0; k < NI2.GetOutDeg(); k++) {
          int nbr = NI2.GetOutNId(k);
          if (IsUnidirEdge(graph, src2, nbr)) {
            nbr_counts(nbr) += 1;
          }
        }

        // Get all common outgoing neighbors
        TIntV common;
        for (auto it = nbr_counts.BegI(); it < nbr_counts.EndI(); it++) {
          if (it->Dat == 2) {
            common.Add(it->Key);
          }
        }

        // Update weights with all common neighbors
        for (int ind1 = 0; ind1 < common.Len(); ind1++) {
          for (int ind2 = (ind1 + 1); ind2 < common.Len(); ind2++) {
            int dst1 = common[ind1];
            int dst2 = common[ind2];
            if (IsNoEdge(graph, dst1, dst2)) {
              IncrementWeight(src1, src2, weights);
              IncrementWeight(src1, dst1, weights);
              IncrementWeight(src1, dst2, weights);
              IncrementWeight(src2, dst1, weights);
              IncrementWeight(src2, dst2, weights);
              IncrementWeight(dst1, dst2, weights);
            }
          }
        }
      }
    }
  }
}

void MotifCluster::SemicliqueMotifAdjacency(PUNGraph graph,
                                            TVec< THash<TInt, TInt> >& weights) {
  for (TUNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {  
    int src = NI.GetId();
    for (int j = 0; j < NI.GetDeg(); j++) {
      int dst = NI.GetNbrNId(j);
      if (dst <= src) { continue; }

      // Common neighbors of dst that are neighbors of src
      TIntV common;
      auto dst_NI = graph->GetNI(dst);
      for (int k = 0; k < dst_NI.GetOutDeg(); k++) {
        int nbr = dst_NI.GetNbrNId(k);
        if (nbr != src && nbr != dst && NI.IsNbrNId(nbr)) {
          common.Add(nbr);
        }
      }
      
      for (int k = 0; k < common.Len(); k++) {
        for (int l = k + 1; l < common.Len(); l++) {
          int nbr1 = common[k];
          int nbr2 = common[l];
          if (!graph->IsEdge(nbr1, nbr2)) {
            IncrementWeight(nbr1, nbr2, weights);
          }
        }
      }
    }
  }
}


/////////////////////////////////////////////////
// Motif adjacency formation
void MotifCluster::MotifAdjacency(PNGraph graph, MotifType motif,
                                  TVec< THash<TInt, TInt> >& weights) {
  weights = TVec< THash<TInt, TInt> >(graph->GetMxNId() + 1);
  switch (motif) {
  case M1:
  case M2:
  case M3:
  case M4:
  case M5:
  case M6:
  case M7:
    TriangleMotifAdjacency(graph, motif, weights);
    break;
  case M8:
  case M9:
  case M10:
  case M11:
  case M12:
  case M13:
    WedgeMotifAdjacency(graph, motif, weights);
    break;    
  case bifan:
    BifanMotifAdjacency(graph, weights);
    break;
  default:
    TExcept::Throw("Unknown directed motif type");
  }
}

void MotifCluster::CliqueMotifAdjacency(PUNGraph graph, int clique_size,
                                        TVec< THash<TInt, TInt> >& weights) {
  ChibaNishizekiWeighter cnw(graph);
  cnw.Run(clique_size);
  weights = cnw.weights();
}

void MotifCluster::MotifAdjacency(PUNGraph graph, MotifType motif,
                                  TVec< THash<TInt, TInt> >& weights) {
  weights = TVec< THash<TInt, TInt> >(graph->GetMxNId() + 1);
  switch (motif) {
  case triangle:
  case clique3:
    CliqueMotifAdjacency(graph, 3, weights);
    break;
  case clique4:
    CliqueMotifAdjacency(graph, 4, weights);
    break;
  case clique5:
    CliqueMotifAdjacency(graph, 5, weights);
    break;    
  case clique6:
    CliqueMotifAdjacency(graph, 6, weights);
    break;    
  case clique7:
    CliqueMotifAdjacency(graph, 7, weights);
    break;    
  case clique8:
    CliqueMotifAdjacency(graph, 8, weights);
    break;    
  case clique9:
    CliqueMotifAdjacency(graph, 9, weights);
    break;
  case semiclique:
    SemicliqueMotifAdjacency(graph, weights);
    break;
  default:
    TExcept::Throw("Unknown undirected motif type");
  }
}


/////////////////////////////////////////////////
// Spectral stuff

// Scale a vector by another vector, entrywise: y = y .* x
void EntrywiseScale(const TFltV& x, TFltV& y) {
  if (x.Len() != y.Len()) {
    TExcept::Throw("Vectors not the same length.");
  }
  for (int i = 0; i < y.Len(); i++) {
    y[i] *= x[i];
  }
}

double MotifCluster::FiedlerVector(const TSparseColMatrix& W, TFltV& fvec,
                                   double tol, int maxiter) {
  if (W.GetRows() != W.GetCols()) {
    TExcept::Throw("Matrix must be square.");
  }

  int N = W.GetCols();
  
  // all ones vector
  TFltV e(N);
  e.PutAll(1.0);
  // degree vector
  TFltV d(N);
  d.PutAll(0.0);
  W.Multiply(e, d);

  // Unit first eigenvector and d^{-1/2}
  TFltV first(N);
  TFltV dnorm(N);
  for (int i = 0; i < d.Len(); i++) {
    if (d[i] <= 0.0) {
      TExcept::Throw("Node with zero degree.");
    }
    first[i] = TMath::Sqrt(d[i]);
    dnorm[i] = 1.0 / TMath::Sqrt(d[i]);
  }
  TLinAlg::Normalize(first);

  TFltV qk(N);
  TRnd Rnd;
  TFltV zk(N);
  for (int i = 0; i < N; i++) {
    zk[i] = Rnd.GetNrmDev(0, 1, -10, 10);
  }
  TLinAlg::Normalize(zk);

  double eig = -1;

  // Power method for the fiedler vector
  for (int iter = 0; iter < maxiter; iter++) {
    // Orthogonalize and normalize
    TLinAlg::AddVec(-TLinAlg::DotProduct(first, zk), first, zk, qk);
    TLinAlg::Normalize(qk);

    // Apply operator zk = (2I + D^{-1/2} * W * D^{-1/2}) * qk
    TFltV tmp = qk;
    EntrywiseScale(dnorm, tmp);
    W.Multiply(tmp, zk);
    EntrywiseScale(dnorm, zk);
    TLinAlg::AddVec(2, qk, zk, zk);

    // Eigenvalue estimation
    eig = TLinAlg::DotProduct(qk, zk);
    
    // Stopping criterion ||zk - eig * qk||_2 < epsilon
    TFltV residual(N);
    TLinAlg::AddVec(-eig, qk, zk, residual);
    if (TLinAlg::Norm(residual) < tol) {
      break;
    }
  }

  fvec = qk;
  return -eig + 3;  // eigenvalue for normalized Laplacian
}

// Given a vector of hashmaps representing weights in a graph, construct the
// undirected, unweighted. graph with the same network structure.
PUNGraph UnweightedGraphRepresentation(const TVec< THash<TInt, TInt> >& weights) {
  int num_edges = 0;
  for (int i = 0; i < weights.Len(); i++) {
    num_edges += weights[i].Len();
  }
  PUNGraph graph = TUNGraph::New(weights.Len(), num_edges);
  for (int i = 0; i < weights.Len(); i++) {
    graph->AddNode(i);
  }
  for (int i = 0; i < weights.Len(); i++) {
    const THash<TInt, TInt>& edge_list = weights[i];
    for (auto it = edge_list.BegI(); it < edge_list.EndI(); it++) {    
      graph->AddEdge(i, it->Key);
    }
  }

  return graph;
}


// Maps a vector of unique ids to 0, 1, ..., num_ids - 1.  Sets up data
// structures id_map and rev_id_map so that
//     id_map[original_id] = new_id
//     rev_id_map[new_id]  = original_id
void MapIdsToFirstN(const TIntV& ids, THash<TInt, TInt>& id_map, TIntV& rev_id_map) {
  id_map = THash<TInt, TInt>();
  rev_id_map = TIntV(ids.Len());
  for (int i = 0; i < ids.Len(); i++) {
    int id = ids[i];
    if (id_map.IsKey(id)) {
      TExcept::Throw("List of ids is not unique.");
    }
    id_map(id) = i;
    rev_id_map[i] = id;
  }
}


// Run a sweep cut on the network represented by W and Fiedler vector fvec,
// storing the conductances from the sweep in conds and the order of the nodes
// in order.
void Sweep(const TSparseColMatrix& W, const TFltV& fvec, TFltV& conds,
           TIntV& order) {
  // Get ordering of nodes
  TVec< TKeyDat<TFlt, TInt> > fvec_inds(fvec.Len());
  for (int i = 0; i < fvec.Len(); i++) {
    fvec_inds[i] = TKeyDat<TFlt, TInt>(fvec[i], i);
  }
  fvec_inds.Sort();
  order = TIntV(fvec.Len());
  TIntV rank(fvec.Len());
  for (int i = 0; i < fvec.Len(); i++) {
    order[i] = fvec_inds[i].Dat;
    rank[order[i]] = i;
  }

  // Sweep by adjusting cut and volume
  conds = TFltV(order.Len() - 1);
  double cut = 0;
  double vol = 0;
  double total_vol = 0;
  for (int ind = 0; ind < order.Len(); ind++) {
    const TIntFltKdV& nbr_weights = W.ColSpVV[ind];
    for (auto it = nbr_weights.BegI(); it < nbr_weights.EndI(); it++) {
      total_vol += it->Dat;
    }
  }
  double vol_comp = total_vol;
  
  for (int ind = 0; ind < order.Len() - 1; ind++) {
    int node = order[ind];
    const TIntFltKdV& nbr_weights = W.ColSpVV[node];
    for (auto it = nbr_weights.BegI(); it < nbr_weights.EndI(); it++) {
      int nbr = it->Key;
      if (node == nbr) { continue; }
      double val = it->Dat;
      // Adjust the cut amount
      if (rank[nbr] > ind) {
        // nbr is on the other side: add to the cut
        cut += val;
      } else {
        // now on the same side as nbr: subtract from the cut
        cut -= val;
      }
      vol += val;
      vol_comp -= val;
    }

    double mvol = MIN(vol, vol_comp);
    if (mvol <= 0.0) {
      TExcept::Throw("Nonpositive set volume.");
    }
    conds[ind] = cut / mvol;
  }
}

void MotifCluster::SpectralCut(const TVec< THash<TInt, TInt> >& weights, TSweepCut& sweepcut,
                               double tol, int maxiter) {
  // Form graph and get maximum component
  PUNGraph graph = UnweightedGraphRepresentation(weights);
  TCnComV components;
  TSnap::GetWccs(graph, components);
  int max_wcc_size = 0;
  int max_wcc_ind = -1;
  for (int i = 0; i < components.Len(); i++) {
    int size = components[i].Len();
    if (size > max_wcc_size) {
      max_wcc_size = size;
      max_wcc_ind = i;
    }
  }
  TCnCom comp = components[max_wcc_ind];
  sweepcut.component = comp;

  // Map largest connected component to a matrix, keeping track of ids.
  THash<TInt, TInt> id_map;
  TIntV rev_id_map;
  MapIdsToFirstN(comp.NIdV, id_map, rev_id_map);

  TVec<TIntFltKdV> matrix_entries(comp.Len());
  for (int ind1 = 0; ind1 < comp.Len(); ++ind1) {
    int c_ind = comp[ind1];
    const THash<TInt, TInt>& edge_list = weights[c_ind];
    int i_ind = id_map(c_ind);
    TIntFltKdV& col = matrix_entries[i_ind];
    for (auto it = edge_list.BegI(); it < edge_list.EndI(); it++) {
      int ind2 = it->Key;
      int val = it->Dat;
      if (comp.IsNIdIn(ind2)) {
        int j_ind = id_map(ind2);
        col.Add(TIntFltKd(j_ind, val));
        // Add symmetric part
        matrix_entries[j_ind].Add(TIntFltKd(i_ind, val));
      }
    }
  }

  // Get Fiedler vector and run the sweep
  TSparseColMatrix W(matrix_entries, comp.Len(), comp.Len());
  TFltV fvec;
  sweepcut.eig = FiedlerVector(W, fvec, tol, maxiter);

  TFltV conds;
  TIntV order;
  Sweep(W, fvec, conds, order);
  sweepcut.sweep_profile = conds;

  // Extract the cluster
  double min_cond = 2.0;
  int min_ind = -1;
  for (int i = 0; i < conds.Len(); i++) {
    double cond = conds[i];
    if (cond < min_cond) {
      min_cond = cond;
      min_ind = i;
    }
  }
  sweepcut.cond = min_cond;
  TIntV cluster;
  int start = 0;
  int end = min_ind + 1;
  if (end >= conds.Len() / 2) {
    start = min_ind + 1;
    end = conds.Len() + 1;
  }
  for (int i = start; i < end; i++) {
    cluster.Add(rev_id_map[order[i]]);    
  }
  sweepcut.cluster = cluster;
}


/////////////////////////////////////////////////
// Clique weighting
void ChibaNishizekiWeighter::Initialize(int k) {
  k_ = k;
  C_.Clr();

  // Process the k - 1 core
  PUNGraph kcore = TSnap::GetKCore(orig_graph_, k - 1);
  TSnap::DelSelfEdges(kcore);
  int max_nodes = kcore->GetMxNId();
  for (int i = 0; i <= max_nodes; i++) {
    if (!kcore->IsNode(i)) {
      kcore->AddNode(i);
    }
  }

  int N = kcore->GetNodes();
  weights_ = TVec< THash<TInt, TInt> >(N);
  graph_ = TVec < TVec<TIntV> >(k + 2);
  for (int i = 0; i < k + 2; ++i) {
    graph_[i] = TVec<TIntV>(N);
  }
  labels_ = TIntV(N);
  labels_.PutAll(k);
  
  auto& graph_k = graph_[k];
  for (int src = 0; src < N; src++) {
    auto src_it = kcore->GetNI(src);
    int deg = src_it.GetDeg();
    graph_k[src] = TIntV(deg);
    for (int edge = 0; edge < deg; edge++) {
      int dst = src_it.GetNbrNId(edge);
      graph_k[src][edge] = dst;
    }
  }
}

void ChibaNishizekiWeighter::Run(int k) {
  Initialize(k);
  TIntV U(graph_[k].Len());
  for (int i = 0; i < U.Len(); i++) {
    U[i] = i;
  }
  CliqueEnum(k, U);
}

void ChibaNishizekiWeighter::SubgraphDegreeOrder(int k, const TIntV& U,
						 TIntV& order) {
  TVec< TKeyDat<TInt, TInt> > degs(U.Len());
  int end_size = 0;
  for (int i = 0; i < U.Len(); i++) {
    int node = U[i];
    int size = graph_[k][node].Len();
    if (size > 0) {
      degs[end_size] = TKeyDat<TInt, TInt>(-size, node);
      ++end_size;
    }
  }
  // Only keep nodes with degree > 0  
  degs.Trunc(end_size);
  // Sort by increasing degree
  degs.Sort();

  order = TIntV(degs.Len());
  for (int i = 0; i < degs.Len(); i++) {
    order[i] = degs[i].Dat;
  }
}

void ChibaNishizekiWeighter::UpdateWeights(const TIntV& clique) {
  for (int i = 0; i < clique.Len(); ++i) {
    for (int j = i + 1; j < clique.Len(); ++j) {
      IncrementWeight(clique[i], clique[j], weights_);
    }
  }
}

void ChibaNishizekiWeighter::FlushCliques(const TIntV& U) {
  for (int i = 0; i < U.Len(); i++) {
    int node = U[i];
    TIntV& nbrs = graph_[2][node];
    for (int j = 0; j < nbrs.Len(); j++) {
      int nbr = nbrs[j];
      // Only count each edge once
      if (node < nbr) {
        TIntV clique(C_.Len() + 2);
        clique[0] = node;
        clique[1] = nbr;
        for (int k = 0; k < C_.Len(); k++) {
          clique[k + 2] = C_[k];
        }
        UpdateWeights(clique);
      }
    }
  }
}

void ChibaNishizekiWeighter::AdjustLabels(int kcurr, int klast, const TIntV& Up) {
  for (int i = 0; i < Up.Len(); i++) {
    int node = Up[i];
    TIntV& nbrs_kcurr = graph_[kcurr][node];
    TIntV& nbrs_klast = graph_[klast][node];
    TIntV nbrs_klast_new;
    for (int j = 0; j < nbrs_klast.Len(); j++) {
      int nbr = nbrs_klast[j];
      if (labels_[nbr] == kcurr) {
        nbrs_kcurr.Add(nbr);
      } else {
        nbrs_klast_new.Add(nbr);
      }
    }
    graph_[klast][node] = nbrs_klast_new;
  }
}

void ChibaNishizekiWeighter::CliqueEnum(int k, const TIntV& U) {
  // Handle base case
  if (k == 2) {
    FlushCliques(U);
    return;
  }

  // Get the degrees of nodes in U
  TIntV order;
  SubgraphDegreeOrder(k, U, order);
  for (int i = 0; i < order.Len(); i++) {
    int vi = order[i];
    // Get neighbors of current node in the subgraph
    TIntV& U_prime = graph_[k][vi];

    // Re-label
    for (int j = 0; j < U_prime.Len(); j++) {
      labels_[U_prime[j]] = k - 1;
    }
    AdjustLabels(k - 1, k, U_prime);

    // Recurse
    C_.Add(vi);
    CliqueEnum(k - 1, U_prime);
    C_.DelLast();

    // Re-label
    for (int j = 0; j < U_prime.Len(); j++) {
      labels_[U_prime[j]] = k;
    }
    AdjustLabels(k, k - 1, U_prime);
    labels_[vi] = k + 1;
    AdjustLabels(k + 1, k, U_prime);
  }
}
