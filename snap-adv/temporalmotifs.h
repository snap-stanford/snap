#ifndef snap_temporalmotifs_h
#define snap_temporalmotifs_h

#include "Snap.h"

typedef TKeyDat<TInt, TInt> TIntPair;

// Simple one-dimensional, two-dimensional, and three-dimensional counter
// classes with default intialization.
class Counter1D {
 public:
  Counter1D(int m=0) : m_(m) {
    if (m > 0) {
      data_ = TUInt64V(m);
      data_.PutAll(0);
    }
  }
  const TUInt64& operator()(int i) const { return data_[i]; }
  TUInt64& operator()(int i) { return data_[i]; }
  int m() { return m_; }
  
 private:
  int m_;
  TUInt64V data_;
};

class Counter2D {
 public:
  Counter2D(int m=0, int n=0) : m_(m), n_(n) {
    if (m * n > 0) {
      data_ = TUInt64V(m * n);
      data_.PutAll(0);
    }
  }
  const TUInt64& operator()(int i, int j) const { return data_[i + j * m_]; }
  TUInt64& operator()(int i, int j) { return data_[i + j * m_]; }
  int m() { return m_; }
  int n() { return n_; }
  
 private:
  int m_;
  int n_;
  TUInt64V data_;
};

class Counter3D {
 public:
  Counter3D(int m=0, int n=0, int p=0) : m_(m), n_(n), p_(p) {
    if (m * n * p > 0) {
      data_ = TUInt64V(m * n * p);
      data_.PutAll(0);
    }
  }
  const TUInt64& operator()(int i, int j, int k) const {
    return data_[i + j * n_ + k * m_ * n_];
  }
  TUInt64& operator()(int i, int j, int k) {
    return data_[i + j * m_ + k * m_ * n_];
  }
  int m() { return m_; }
  int n() { return n_; }
  int p() { return p_; }  
  
 private:
  int m_;
  int n_;
  int p_;
  TUInt64V data_;
};

// Triad edge data consists of a neighbor, a direction, and an indicator of whether
// the edge connects with wich endpoint (u or v).
class TriadEdgeData {
 public:
  TriadEdgeData() {}
  TriadEdgeData(int _nbr, int _dir, int _u_or_v) :
      nbr(_nbr), dir(_dir), u_or_v(_u_or_v) {}
  int nbr;     // Which neighbor of the center node
  int dir;     // Outgoing (0) or incoming (1) direction
  int u_or_v;  // Points to first end point u (0) or second end point v (1)
};

// Star edge data consists of a neighbor and a direction.
class StarEdgeData {
 public:
  StarEdgeData() {}
  StarEdgeData(int _nbr, int _dir) : nbr(_nbr), dir(_dir) {}
  int nbr;  // Which neighbor of the center node
  int dir;  // Outgoing (0) or incoming (1) direction
};

// Main temporal motif counting class.  This implementation has support for
// counting motifs with three temporal edges on two or three nodes.  
class TempMotifCounter {
 public:
  // Reads directed temporal graph data from the specified file, which must have
  // the following format:
  //    source_node destination_node unix_timestamp
  TempMotifCounter(const TStr& filename);

  // Count all three temporal edge, two-node delta-temporal motifs and fills the
  // counter counts with the results.  The format is:
  //   counts(0, 0): u --> v, v --> u, u --> v  (M_{5,1})
  //   counts(0, 1): u --> v, v --> u, v --> u  (M_{5,2})
  //   counts(1, 0): u --> v, u --> v, u --> v  (M_{6,1})
  //   counts(1, 1): u --> v, u --> v, v --> u  (M_{6,2})
  void Count3TEdge2Node(double delta, Counter2D& counts);
  
  // Similar to Count3TEdge2Node() except only counts motif instances
  // for a given pair of nodes u and v and specifies the source and destination
  // node.  The counts format is:
  //   counts(0, 0, 0): u --> v, u --> v, u --> v
  //   counts(1, 1, 1): v --> u, v --> u, v --> u
  //   counts(0, 1, 1): u --> v, v --> u, v --> u
  //   counts(1, 0, 0): v --> u, u --> v, u --> v
  //   counts(0, 1, 0): u --> v, v --> u, u --> v
  //   counts(1, 0, 1): v --> u, u --> v, v --> u
  //   counts(0, 0, 1): u --> v, u --> v, v --> u
  //   counts(1, 1, 0): v --> u, v --> u, u --> v
  void Count3TEdge2Node(int u, int v, double delta, Counter3D& counts);

  // Counts 3-edge, 3-node star motifs and places the results in pre_counts,
  // pos_counts, and mid_counts.  Counts take the following structure (with
  // center node c):
  //
  //     pre: {c, u}, {c, u}, {c, v}
  //     pos: {c, u}, {c, v}, {c, v}
  //     mid: {c, u}, {c, v}, {c, u}
  //
  // The indicies in the counter correspond to the direction with the center
  // node as the reference (0 indexes outgoing and 1 indexes incoming edges to
  // the center).  For example,
  //
  //     pos_counts(0, 1, 0): c --> u, v --> c, c --> v
  void Count3TEdge3NodeStars(double delta, Counter3D& pre_counts,
                             Counter3D& pos_counts, Counter3D& mid_counts);
  
  // Counts the same information as Count3TEdge3NodeStars() but uses a naive
  // counting algorithm that iterates over all pairs of neighbors.
  void Count3TEdge3NodeStarsNaive(double delta, Counter3D& pre_counts,
                                  Counter3D& pos_counts, Counter3D& mid_counts);

  // Counts 3-edge triad events and places the result in counts:
  //
  //    counts(0, 0, 0): u --> v, w --> v, u --> w (M_{1,3})
  //    counts(0, 0, 1): u --> v, w --> v, w --> u (M_{1,4})
  //    counts(0, 1, 0): u --> v, v --> w, u --> w (M_{2,3})
  //    counts(0, 1, 1): u --> v, v --> w, w --> u (M_{2,4})
  //    counts(1, 0, 0): u --> v, w --> u, v --> w (M_{3,5})
  //    counts(1, 0, 1): u --> v, w --> u, w --> v (M_{3,6})
  //    counts(1, 1, 0): u --> v, u --> w, v --> w (M_{4,5})
  //    counts(1, 1, 1): u --> v, u --> w, w --> v (M_{4,6})
  void Count3TEdgeTriads(double delta, Counter3D& counts);
  
  // Counts the same information as Count3TEdgeTriads() but uses a naive
  // counting algorithm that enumerates over all triangles in the static graph.
  void Count3TEdgeTriadsNaive(double delta, Counter3D& counts);

  // Counts all 3-edge, {2,3}-node temporal motifs and places the result in
  // counts such that counts(i, j) corresponds to motif M_{i,j}.
  void Count3TEdge23Node(double delta, Counter2D& counts);  

 private:
  // Get all triangles in the static graph, (Us(i), Vs(i), Ws(i)) is the ith
  // triangle.
  void GetAllStaticTriangles(TIntV& Us, TIntV& Vs, TIntV& Ws);
  // Fills nbrs with all neighbors (ignoring direction) of the node in the
  // static graph.
  void GetAllNeighbors(int node, TIntV& nbrs);
  // Fills nodes with a vector of all nodes in the static graph.
  void GetAllNodes(TIntV& nodes);

  // Checks whether or not there is a temporal edge along the static edge (u, v)
  bool HasEdges(int u, int v);

  // A simple wrapper for adding triad edge data
  void AddTriadEdgeData(TVec<TriadEdgeData>& events, TVec<TIntPair>& ts_indices,
                        int& index, int u, int v, int nbr, int key1, int key2);
  // A simple wrapper for adding star edge data  
  void AddStarEdgeData(TVec<TIntPair>& ts_indices, TVec<StarEdgeData>& events,
		       int& index, int u, int v, int nbr, int key);
  // Another simple wrapper for adding star edge data
  void AddStarEdges(TVec<TIntPair>& combined, int u, int v, int key);

  // Directed graph from ignoring timestamps
  PNGraph static_graph_;  

  // Core data structure for storing temporal edges.  temporal_data_[u](v) is a
  // list of temporal edges along the static edge (u, v).
  TVec< THash<TInt, TIntV> > temporal_data_;
};

// This class exhaustively counts all size^3 three-edge temporal motifs in an
// alphabet of a given size.
class ThreeTEdgeMotifCounter {
 public:
  // Initialize counter with a given alphabet size
  ThreeTEdgeMotifCounter(int size) : size_(size) {}
  
  // Count all three-edge motifs with corresponding timestamps.  Each integer in
  // the event_string must belong to the set {0, 1, ..., size - 1}.  The function
  // stores the results in the counter, where counts(e, f, g) is the motif consisting
  // of the ordered edges e, f, g.
  void Count(const TIntV& event_string, const TIntV& timestamps,
             double delta, Counter3D& counts);

 private:
  void IncrementCounts(int event);
  void DecrementCounts(int event);
  Counter1D counts1_;
  Counter2D counts2_;
  Counter3D counts3_;
  int size_;  // alphabet size
};

// Base class for 3-edge, 3-node star and triangle counters.  The template type
// describes the data needed when processing an edge.
template <typename EdgeData>
class StarTriad3TEdgeCounter {
 public:
  StarTriad3TEdgeCounter() {}
  void Count(const TVec<EdgeData>& events, const TIntV& timestamps, double delta);
  
 protected:
  // These methods depend on the motif type (star or triad).
  virtual void InitializeCounters() = 0;
  virtual void PopPre(const EdgeData& event) = 0;
  virtual void PopPos(const EdgeData& event) = 0;
  virtual void PushPre(const EdgeData& event) = 0;
  virtual void PushPos(const EdgeData& event) = 0;
  virtual void ProcessCurrent(const EdgeData& event) = 0;
};

// Class for counting star motifs with a given center node.
class ThreeTEdgeStarCounter : public StarTriad3TEdgeCounter<StarEdgeData> {
 public:
  // Construct class with maximum number of neighbor nodes.  Each processed edge
  // consists of a neighbor and a direction where the neighbor is represented by
  // an int belong to the set {0, 1, ..., max_nodes - 1}.
  ThreeTEdgeStarCounter(int max_nodes) : max_nodes_(max_nodes) {}

  // Counting conventions follow TempMotifCounter::Count3TEdge3NodeStars().
  int PreCount(int dir1, int dir2, int dir3) { return pre_counts_(dir1, dir2, dir3); }
  int PosCount(int dir1, int dir2, int dir3) { return pos_counts_(dir1, dir2, dir3); }
  int MidCount(int dir1, int dir2, int dir3) { return mid_counts_(dir1, dir2, dir3); }

 protected:
  void InitializeCounters();
  void PopPre(const StarEdgeData& event);
  void PopPos(const StarEdgeData& event);
  void PushPre(const StarEdgeData& event);
  void PushPos(const StarEdgeData& event);
  void ProcessCurrent(const StarEdgeData& event);
  
 private:
  int max_nodes_;
  Counter2D pre_sum_;
  Counter2D pos_sum_;
  Counter2D mid_sum_;
  Counter3D pre_counts_;
  Counter3D pos_counts_;
  Counter3D mid_counts_;
  Counter2D pre_nodes_;
  Counter2D pos_nodes_;
};


// Class for counting triangle motifs that contain a specific undirected edge.
class ThreeTEdgeTriadCounter : public StarTriad3TEdgeCounter<TriadEdgeData> {
 public:
  // Construct class with maximum number of neighbor nodes.  Each processed edge
  // consists of a neighbor, a direction, and an indicator of which end point it
  // connects with.  Each neighbor is represented by an int belong to the set
  // {0, 1, ..., max_nodes - 1}.
  ThreeTEdgeTriadCounter(int max_nodes, int node_u, int node_v) :
      max_nodes_(max_nodes), node_u_(node_u), node_v_(node_v) {}

  // Counting conventions follow TempMotifCounter::Count3TEdgeTriads().
  int Counts(int dir1, int dir2, int dir3) { return triad_counts_(dir1, dir2, dir3); }

 protected:
  void InitializeCounters();
  void PopPre(const TriadEdgeData& event);
  void PopPos(const TriadEdgeData& event);
  void PushPre(const TriadEdgeData& event);
  void PushPos(const TriadEdgeData& event);
  void ProcessCurrent(const TriadEdgeData& event);
  bool IsEdgeNode(int nbr) { return nbr == node_u_ || nbr == node_v_; }

 private:
  int max_nodes_;
  Counter3D pre_sum_;
  Counter3D pos_sum_;
  Counter3D mid_sum_;
  Counter3D triad_counts_;
  Counter3D pre_nodes_;
  Counter3D pos_nodes_;
  // Two end points of the edge whose triangles this class counts.
  int node_u_;
  int node_v_;
};

#endif  // snap_temporalmotifs_h
