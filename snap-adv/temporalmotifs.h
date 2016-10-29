#ifndef snap_temporalmotifs_h
#define snap_temporalmotifs_h

#include "Snap.h"

class Counter2D {
 public:
 // Base constructor
 Counter2D(int m, int n) : m_(m), n_(n) {
    if (m * n > 0) {
      data_ = TIntV(m * n);
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
 const TInt& operator()(int i, int j) const { return data_[i + j * m_]; }
 TInt& operator()(int i, int j) { return data_[i + j * m_]; }
  
 private:
 int m_;
 int n_;
 TIntV data_;
};

class Counter3D {
 public:
 // Base constructor
 Counter3D(int m, int n, int p) : m_(m), n_(n), p_(p) {
   if (m * n * p > 0) {
     data_ = TIntV(m * n * p);
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
 const TInt& operator()(int i, int j, int k) const { return data_[i + j * n_ + k * m_ * n_]; }
 TInt& operator()(int i, int j, int k) { return data_[i + j * m_ + k * m_ * n_]; }

 private:
 int m_;
 int n_;
 int p_;
 TIntV data_;
};

class TemporalMotifCounter {
 public:
  TemporalMotifCounter() {}
  void LoadData(const TStr& filename);

  void ThreeEventEdgeCounts(double delta, Counter3D& counts);
  void ThreeEventEdgeCounts(int u, int v, double delta, Counter3D& counts);
  void ThreeEventStarCounts(double delta, Counter3D& pre_counts,
			    Counter3D& pos_counts, Counter3D& mid_counts);
  void ThreeEventStarCountsNaive(double delta, Counter3D& pre_counts,
				 Counter3D& pos_counts, Counter3D& mid_counts);  
  void ThreeEventTriangleCountsNaive(double delta, Counter3D& counts);
  void AllCounts(double delta, Counter2D& counts, bool naive);
  
 private:
  void GetAllTriangles(TIntV& Us, TIntV& Vs, TIntV& Ws);
  PNGraph static_graph_;
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
  TIntV counts1_;
  Counter2D counts2_;
  Counter3D counts3_;
  int size_;  // alphabet size
};

template <typename EventType>
class ThreeEventCounter {
 public:
  ThreeEventCounter() {}

  void Count(const TVec<EventType>& events, const TIntV& timestamps, double delta) {
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
  
  int PreCount(int dir1, int dir2, int dir3) { return pre_counts_(dir1, dir2, dir3); }
  int PosCount(int dir1, int dir2, int dir3) { return pos_counts_(dir1, dir2, dir3); }
  int MidCount(int dir1, int dir2, int dir3) { return mid_counts_(dir1, dir2, dir3); }

 protected:
  Counter2D pre_sum_ = Counter2D(2, 2);
  Counter2D pos_sum_ = Counter2D(2, 2);
  Counter2D mid_sum_ = Counter2D(2, 2);

  Counter3D pre_counts_ = Counter3D(2, 2, 2);
  Counter3D pos_counts_ = Counter3D(2, 2, 2);
  Counter3D mid_counts_ = Counter3D(2, 2, 2);

  virtual void PopPre(EventType event) = 0;
  virtual void PopPos(EventType event) = 0;
  virtual void PushPre(EventType event) = 0;
  virtual void PushPos(EventType event) = 0;
  virtual void ProcessCurrent(EventType event) = 0;
};

class StarEvent {
 public:
  StarEvent() {}
  StarEvent(int _nbr, int _dir) : nbr(_nbr), dir(_dir) {}
  int nbr;  // Which neighbor of the center node
  int dir;  // Outgoing (0) or incoming (1) direction
};

class ThreeEventStarCounter : public ThreeEventCounter<StarEvent> {
 public:
 ThreeEventStarCounter(int num_nodes) : pre_nodes_(2, num_nodes),
    pos_nodes_(2, num_nodes) {}

 protected:
  void PopPre(StarEvent event);
  void PopPos(StarEvent event);
  void PushPre(StarEvent event);
  void PushPos(StarEvent event);
  void ProcessCurrent(StarEvent event);
  
 private:
  Counter2D pre_nodes_;
  Counter2D pos_nodes_;
};

class TriadEvent {
 public:
  TriadEvent() {}
 TriadEvent(int _nbr, int _dir, int _u_or_v) : nbr(_nbr), dir(_dir), u_or_v(_u_or_v) {}
  int nbr;  // Which neighbor of the center node
  int dir;  // Outgoing (0) or incoming (1) direction
  int u_or_v;  // Points to first end point (u) or second end point (v)
};

class ThreeEventTriadCounter : public ThreeEventCounter<TriadEvent> {
 public:
 ThreeEventTriadCounter(int num_nodes, int node_u, int node_v) :
  pre_nodes_(2, 2, num_nodes), pos_nodes_(2, 2, num_nodes),
    node_u_(node_u), node_v_(node_v) {}

 protected:
  void PopPre(TriadEvent event);
  void PopPos(TriadEvent event);
  void PushPre(TriadEvent event);
  void PushPos(TriadEvent event);
  void ProcessCurrent(TriadEvent event);
  bool IsEdgeNode(int nbr) { return nbr == node_u_ || nbr == node_v_; }

  // Two end points of the edge whose triangles this class counts.
  int node_u_;
  int node_v_;

  Counter3D pre_nodes_;
  Counter3D pos_nodes_;
};

#endif  // snap_temporalmotifs_h
