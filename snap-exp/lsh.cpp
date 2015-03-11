#include "lsh.h"

TLSHash::JaccardHash::JaccardHash(TRnd &Gen, int Dim) {
  for (int i=0; i<Dim; i++) {
    Perm.Add(i);
  }
  Perm.Shuffle(Gen);
}

TLSHash::JaccardHash::JaccardHash(TIntV Perm) : Perm(Perm) { }

int TLSHash::JaccardHash::Hash(TFltV Datum) {
  for (int i=0; i<Perm.Len(); i++) {
    if (Datum[Perm[i]] > 0)
      return i;
  }
  return Perm.Len();
}

TLSHash::CosineHash::CosineHash(TRnd &Gen, int Dim) {
  for (int i=0; i<Dim; i++) {
    Hyperplane.Add(TBool(TBool::GetRnd()));
  }
}

TLSHash::CosineHash::CosineHash(TBoolV Hyperplane) : Hyperplane(Hyperplane) { }

int TLSHash::CosineHash::Hash(TFltV Datum) {
  int Side = 0;
  for (int i=0; i<Datum.Len(); i++) {
    if (Hyperplane[i])
      Side += Datum[i];
    else
      Side -= Datum[i];
  }
  if (Side > 0)
    return 1;
  else
    return -1;
}

const int TLSHash::EuclideanHash::Gap = 4;

TLSHash::EuclideanHash::EuclideanHash(TRnd &Gen, int Dim) {
  for (int j=0; j<Dim; j++) {
    Line.Add(Gen.GetNrmDev());
  }
  Line.Add(Gen.GetUniDevInt(Gap));
}

TLSHash::EuclideanHash::EuclideanHash(TFltV Line) : Line(Line) { }

int TLSHash::EuclideanHash::Hash(TFltV Datum) {
  double Proj = Line[Datum.Len()];
  for (int i=0; i<Datum.Len(); i++) {
    Proj += Datum[i] * Line[i];
  }
  Proj /= Gap;
  return static_cast<int>(Proj);
}

TInt TLSHash::ComputeSignature(TFltV Datum, int Band) {
  int Sig = 0;
  for (int r = 0; r<Rows; r++) {
    int ind = Band*Rows + r;
    int Hash = HashFuncV[ind]->Hash(Datum);
    Sig = TPairHashImpl::GetHashCd(Sig, Hash);
  }
  return Sig;
}

bool TLSHash::IsNear(TFltV Datum1, TFltV Datum2) {
  TFlt Dist2 = 0;
  for (int i=0; i<Datum1.Len(); i++) {
    TFlt Diff = Datum1[i] - Datum2[i];
    Dist2 += Diff*Diff;
  }
  return Dist2 <= NearDist * NearDist;
}

TLSHash::TLSHash() { }

TLSHash::TLSHash(int Bands, int Rows, int Dim, DistMeasure Type, int NearDist) :
  Bands(Bands), Rows(Rows), Dim(Dim), ExpectedSz(0),  Type(Type),
  NearDist(NearDist) { }

TLSHash::TLSHash(int Bands, int Rows, int Dim, int ExpectedSz,
  DistMeasure Type, int NearDist) : Bands(Bands), Rows(Rows), Dim(Dim),
  ExpectedSz(ExpectedSz), Type(Type), NearDist(NearDist) { }

TLSHash::~TLSHash() { }

void TLSHash::Init() {
  TRnd Gen;
  Gen.Randomize();

  for (int i=0; i<Bands*Rows; i++) {
    if (Type == JACCARD) {
      HashFuncV.Add(TPt<HashFunc>(new JaccardHash(Gen, Dim)));
    } else if (Type == COSINE) {
      HashFuncV.Add(TPt<HashFunc>(new CosineHash(Gen, Dim)));
    } else {
      HashFuncV.Add(TPt<HashFunc>(new EuclideanHash(Gen, Dim)));
    }
  }

  for (int i=0; i<Bands; i++) {
    SigBucketVHV.Add(THash<TInt, TIntV> (ExpectedSz, true));
  }
}

void TLSHash::Add(TFltV Datum) {
  int Id = DataV.Len();
  DataV.Add(Datum);

  for (int i=0; i<Bands; i++) {
    TInt Sig = ComputeSignature(Datum, i);
    THash<TInt, TIntV> &SigBucketVH = SigBucketVHV[i];

    int KeyId = SigBucketVH.AddKey(Sig);
    SigBucketVH[KeyId].Add(Id);
  }
}

void TLSHash::AddV(TVec<TFltV> NewV) {
  int StartId = DataV.Len();
  DataV.AddV(NewV);

  #pragma omp parallel for num_threads(4) schedule(dynamic)
  for (int i=0; i<Bands; i++) {
    THash<TInt, TIntV> &SigBucketVH = SigBucketVHV[i];
    for (int j=0; j<NewV.Len(); j++) {
      int KeyId = SigBucketVH.AddKey(ComputeSignature(NewV[j], i));
      SigBucketVH[KeyId].Add(StartId+j);
    }
  }
}

void TLSHash::Remove(TFltV Datum) {
  int Id = DataV.SearchForw(Datum);
  if (Id == -1) { return; }
  DataV[Id] = TFltV();
  for (int i=0; i<SigBucketVHV.Len(); i++) {
    THash<TInt, TIntV> &SigBucketVH = SigBucketVHV[i];
    TVec<TInt> SigV;
    SigBucketVH.GetKeyV(SigV);

    for (int j=0; j<SigV.Len(); j++) {
      TIntV &BucketV = SigBucketVH.GetDat(SigV[j]);
      int Ind = BucketV.SearchBin(Id);
      if (Ind != -1) {
        BucketV.Del(Ind);
      }
    }
  }
}

TVec<TFltV> TLSHash::GetCandidates(TFltV Datum) {
  THashSet<TInt> CandidateIds;
  for (int i=0; i<Bands; i++) {
    TInt Sig = ComputeSignature(Datum, i);
    THash<TInt, TIntV>& SigBucketVH = SigBucketVHV[i];

    if (!SigBucketVH.IsKey(Sig)) {
      continue;
    }
    CandidateIds.AddKeyV(SigBucketVH.GetDat(Sig));
  }

  TVec<TFltV> Candidates;
  int Ind = CandidateIds.FFirstKeyId();
  while(CandidateIds.FNextKeyId(Ind)) {
    int Id = CandidateIds[Ind];
    Candidates.Add(DataV[Id]);
  }
  return Candidates;
}

TVec<TPair<TFltV, TFltV> > TLSHash::GetAllCandidatePairs() {
  THashSet<TPair<TInt, TInt> > CandidateIdPairs;
  for (int i=0; i<Bands; i++) {
    TVec<TIntV> BucketVV;
    SigBucketVHV[i].GetDatV(BucketVV);
    for (int j=0; j<BucketVV.Len(); j++) {
      TIntV BucketV = BucketVV[j];

      for (int k=0; k<BucketV.Len(); k++) {
        for (int l=k+1; l<BucketV.Len(); l++) {
          int First = BucketV[k], Second = BucketV[l];
          if (First > Second) { 
            int Temp = First;
            First = Second;
            Second = Temp;
          }
          CandidateIdPairs.AddKey(TPair<TInt, TInt> (First, Second));
        }
      }
    }
  }

  TVec<TPair<TFltV, TFltV> > CandidatePairs;
  int Ind = CandidateIdPairs.FFirstKeyId();
  while (CandidateIdPairs.FNextKeyId(Ind)) {
    TPair<TInt, TInt> IdPair = CandidateIdPairs[Ind];
    TPair<TFltV, TFltV> Pair(DataV[IdPair.GetVal1()], DataV[IdPair.GetVal2()]);
    CandidatePairs.Add(Pair);
  }
  return CandidatePairs;
}

TVec<TFltV> TLSHash::GetNearPoints(TFltV Datum) {
  TVec<TFltV> Candidates = GetCandidates(Datum);
  TVec<TFltV> NearPoints;

  for (int i=0; i<Candidates.Len(); i++) {
    if (IsNear(Datum, Candidates[i]))
      NearPoints.Add(Candidates[i]);
  }
  return NearPoints;
}

TVec<TPair<TFltV, TFltV> > TLSHash::GetAllNearPairs() {
  TVec<TPair<TFltV, TFltV> > CandidatePairs = GetAllCandidatePairs();
  TVec<TPair<TFltV, TFltV> > NearPairs;

  for (int i=0; i<CandidatePairs.Len(); i++) {
    if (IsNear(CandidatePairs[i].GetVal1(), CandidatePairs[i].GetVal2())) {
      NearPairs.Add(CandidatePairs[i]);
    }
  }
  return NearPairs;
}
