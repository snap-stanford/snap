#ifndef snap_temporalmotifs_h
#define snap_temporalmotifs_h

#include "Snap.h"

class Counter2D {
 public:
 // Base constructor
 Counter2D(int n) : n_(n) {
    if (n > 0) {
      data_ = TIntV(n * n);
      data_.PutAll(0);
    }
 }
 Counter2D() : Counter2D(0) {}
 // Copy assignment
 Counter2D& operator=(const Counter2D& that) {
   if (this != &that) {
     n_ = that.n_;
     data_ = that.data_;
   }
   return *this;
 }
 const TInt& operator()(int i, int j) const { return data_[i + j * n_]; }
 TInt& operator()(int i, int j) { return data_[i + j * n_]; }
  
 private:
 int n_;
 TIntV data_;
};

class Counter3D {
 public:
 // Base constructor
 Counter3D(int n) : n_(n) {
   if (n > 0) {
     data_ = TIntV(n * n * n);
     data_.PutAll(0);
   }
 }
 Counter3D() : Counter3D(0) {}
 // Copy assignment
 Counter3D& operator=(const Counter3D& that) {
   if (this != &that) {
     n_ = that.n_;
     data_ = that.data_;
   }
   return *this;
 }
 const TInt& operator()(int i, int j, int k) const { return data_[i + j * n_ + k * n_ * n_]; }
 TInt& operator()(int i, int j, int k) { return data_[i + j * n_ + k * n_ * n_]; }

 private:
 int n_;
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

class ThreeEventStarCounter {
 public:
  ThreeEventStarCounter(int num_nodes);

  void Count(const TIntV& node, const TIntV& dir,
	     const TIntV& timestamps, double delta);

  int PreCount(int dir1, int dir2, int dir3) { return pre_counts_(dir1, dir2, dir3); }
  int PosCount(int dir1, int dir2, int dir3) { return pos_counts_(dir1, dir2, dir3); }
  int MidCount(int dir1, int dir2, int dir3) { return mid_counts_(dir1, dir2, dir3); }

 private:
  void PopPre(int nbr, int dir);
  void PopPos(int nbr, int dir);
  void PushPre(int nbr, int dir);
  void PushPos(int nbr, int dir);
  void DecMiddleSum(int nbr, int dir);
  void IncMiddleSum(int nbr, int dir);
  void ProcessCurrent(int nbr, int dir);  


  Counter2D pre_sum_ = Counter2D(2);
  Counter2D pos_sum_ = Counter2D(2);
  Counter2D mid_sum_ = Counter2D(2);

  Counter3D pre_counts_ = Counter3D(2);
  Counter3D pos_counts_ = Counter3D(2);
  Counter3D mid_counts_ = Counter3D(2);

  TVec< TIntV > pre_nodes_;
  TVec< TIntV > pos_nodes_;  
};

#endif  // snap_temporalmotifs_h
