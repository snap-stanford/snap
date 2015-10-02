#ifndef snap_lsh_h
#define snap_lsh_h

#include "Snap.h"

class TLSHash {
public:
  enum DistMeasure {JACCARD, COSINE, EUCLIDEAN};
private:
  class HashFunc {
  private:
    TCRef CRef;
  public:
    virtual int Hash(TFltV Datum) = 0;
    friend class TPt<HashFunc>;
  };
  
  class JaccardHash : public HashFunc {
  private:
    TIntV Perm;
  public:
    JaccardHash(TRnd &Gen, int Dim);
    JaccardHash(TIntV Perm);
    int Hash(TFltV Datum);
  };

  class CosineHash : public HashFunc {
  private:
    TBoolV Hyperplane;
  public:
    CosineHash(TRnd &Gen, int Dim);
    CosineHash(TBoolV Hyperplane);
    int Hash(TFltV Datum);
  };
  
  class EuclideanHash : public HashFunc {
  private:
    static const int Gap;
    TFltV Line;
  public:
    EuclideanHash(TRnd &Gen, int Dim);
    EuclideanHash(TFltV Line);
    int Hash(TFltV Datum);
  };

  int Bands;
  int Rows;
  int Dim;
  int ExpectedSz;
  DistMeasure Type;
  int NearDist;

  TVec<TPt<HashFunc> > HashFuncV;
  TVec<TFltV> DataV;
  TVec<THash<TInt, TIntV> > SigBucketVHV;

  TInt ComputeSignature(TFltV Datum, int Band);
  bool IsNear(TFltV Datum1, TFltV Datum2);

public:
  TLSHash();
  TLSHash(int Bands, int Rows, int Dim, DistMeasure Type, int NearDist = 1);
  TLSHash(int Bands, int Rows, int Dim, int ExpectedSz, DistMeasure Type,
    int NearDist = 1);
  ~TLSHash();

  void Init();

  void Add(TFltV Datum);
  void AddV(TVec<TFltV> DataV);
  void Remove(TFltV Datum);

  TVec<TFltV> GetCandidates(TFltV Datum);
  TVec<TPair<TFltV, TFltV> >  GetAllCandidatePairs();

  TVec<TFltV> GetNearPoints(TFltV Datum);
  TVec<TPair<TFltV, TFltV> > GetAllNearPairs();
};
#endif // snap_lsh_h
