#ifndef snap_temporalmotifs_h
#define snap_temporalmotifs_h

#include "Snap.h"

// Simple two-dimensional counter class.
class Counter2D {
 public:
 // Base constructor
 Counter2D(int m, int n) : m_(m), n_(n) {
    if (m * n > 0) {
      data_ = TUInt64V(m * n);
      data_.PutAll(0);
    }
 }
 Counter2D() : Counter2D(0, 0) {}
 // Copy assignment
 Counter2D& operator=(const Counter2D& that) {
   if (this != &that) {
     m_ = that.m_;
     n_ = that.n_;
     data_ = that.data_;
   }
   return *this;
 }
 const TUInt64& operator()(int i, int j) const { return data_[i + j * m_]; }
 TUInt64& operator()(int i, int j) { return data_[i + j * m_]; }
  
 private:
 int m_;
 int n_;
 TUInt64V data_;
};

// Simple three-dimensional counter class.
class Counter3D {
 public:
 // Base constructor
 Counter3D(int m, int n, int p) : m_(m), n_(n), p_(p) {
   if (m * n * p > 0) {
     data_ = TUInt64V(m * n * p);
     data_.PutAll(0);
   }
 }
 Counter3D() : Counter3D(0, 0, 0) {}
 // Copy assignment
 Counter3D& operator=(const Counter3D& that) {
   if (this != &that) {
     m_ = that.m_;
     n_ = that.n_;
     p_ = that.p_;     
     data_ = that.data_;
   }
   return *this;
 }
 const TUInt64& operator()(int i, int j, int k) const { return data_[i + j * n_ + k * m_ * n_]; }
 TUInt64& operator()(int i, int j, int k) { return data_[i + j * m_ + k * m_ * n_]; }

 private:
 int m_;
 int n_;
 int p_;
 TUInt64V data_;
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

  // Directed graph from ignoring timestamps
  PNGraph static_graph_;  

  // Core data structure for storing temporal edges.  temporal_data_[u](v) is a
  // list of temporal edges along the static edge (u, v).
  TVec< THash<TInt, TIntV> > temporal_data_;
};

class ThreeEventMotifCounter {
 public:
  ThreeEventMotifCounter(int size);
  void Count(const TIntV& event_string, const TIntV& timestamps,
             double delta);
  int FinalCount(int i, int j, int k) { return counts3_(i, j, k); }
  Counter3D& Counts() { return counts3_; }

 private:
  void IncrementCounts(int event);
  void DecrementCounts(int event);
  TUInt64V counts1_;
  Counter2D counts2_;
  Counter3D counts3_;
  int size_;  // alphabet size
};

template <typename EdgeData>
class StarTriad3TEdgeCounter {
 public:
  StarTriad3TEdgeCounter() {}
  void Count(const TVec<EdgeData>& events, const TIntV& timestamps, double delta);
  
 protected:
  virtual void PopPre(EdgeData event) = 0;
  virtual void PopPos(EdgeData event) = 0;
  virtual void PushPre(EdgeData event) = 0;
  virtual void PushPos(EdgeData event) = 0;
  virtual void ProcessCurrent(EdgeData event) = 0;
};

class StarEdgeData {
 public:
  StarEdgeData() {}
  StarEdgeData(int _nbr, int _dir) : nbr(_nbr), dir(_dir) {}
  int nbr;  // Which neighbor of the center node
  int dir;  // Outgoing (0) or incoming (1) direction
};

class ThreeEventStarCounter : public StarTriad3TEdgeCounter<StarEdgeData> {
 public:
 ThreeEventStarCounter(int num_nodes) : pre_nodes_(2, num_nodes),
    pos_nodes_(2, num_nodes) {}

  int PreCount(int dir1, int dir2, int dir3) { return pre_counts_(dir1, dir2, dir3); }
  int PosCount(int dir1, int dir2, int dir3) { return pos_counts_(dir1, dir2, dir3); }
  int MidCount(int dir1, int dir2, int dir3) { return mid_counts_(dir1, dir2, dir3); }

 protected:
  void PopPre(StarEdgeData event);
  void PopPos(StarEdgeData event);
  void PushPre(StarEdgeData event);
  void PushPos(StarEdgeData event);
  void ProcessCurrent(StarEdgeData event);
  
 private:
  Counter2D pre_sum_ = Counter2D(2, 2);
  Counter2D pos_sum_ = Counter2D(2, 2);
  Counter2D mid_sum_ = Counter2D(2, 2);
  Counter3D pre_counts_ = Counter3D(2, 2, 2);
  Counter3D pos_counts_ = Counter3D(2, 2, 2);
  Counter3D mid_counts_ = Counter3D(2, 2, 2);
  Counter2D pre_nodes_;
  Counter2D pos_nodes_;
};

class TriadEdgeData {
 public:
  TriadEdgeData() {}
 TriadEdgeData(int _nbr, int _dir, int _u_or_v) : nbr(_nbr), dir(_dir), u_or_v(_u_or_v) {}
  int nbr;     // Which neighbor of the center node
  int dir;     // Outgoing (0) or incoming (1) direction
  int u_or_v;  // Points to first end point u (0) or second end point v (1)
};

class ThreeEventTriadCounter : public StarTriad3TEdgeCounter<TriadEdgeData> {
 public:
 ThreeEventTriadCounter(int num_nodes, int node_u, int node_v) :
  pre_nodes_(num_nodes, 2, 2), pos_nodes_(num_nodes, 2, 2),
    node_u_(node_u), node_v_(node_v) {}

  int Counts(int dir1, int dir2, int dir3) { return triad_counts_(dir1, dir2, dir3); }

 protected:
  void PopPre(TriadEdgeData event);
  void PopPos(TriadEdgeData event);
  void PushPre(TriadEdgeData event);
  void PushPos(TriadEdgeData event);
  void ProcessCurrent(TriadEdgeData event);
  bool IsEdgeNode(int nbr) { return nbr == node_u_ || nbr == node_v_; }

 private:
  Counter3D pre_sum_ = Counter3D(2, 2, 2);
  Counter3D pos_sum_ = Counter3D(2, 2, 2);
  Counter3D mid_sum_ = Counter3D(2, 2, 2);
  Counter3D triad_counts_ = Counter3D(2, 2, 2);
  Counter3D pre_nodes_;
  Counter3D pos_nodes_;
  // Two end points of the edge whose triangles this class counts.
  int node_u_;
  int node_v_;
};

#endif  // snap_temporalmotifs_h
