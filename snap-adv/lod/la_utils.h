#ifndef SNAP_LA_UTILS_H
#define SNAP_LA_UTILS_H

#include "stdafx.h"
#include "typedefs.h"
#include "rdf_parser.h"

/**
 * Author: Klemen Simonic
 *
 * A module with various linear algebra utilities.
 */
class TLAUtils {
private:
  /// Creates a sample of size N of the specified vector.
  static void GetSample (const TIntV &Vector, int N, TRnd &Rnd, TIntV &Sample);

public:
  /// Normalizes the specified sparse column-major matrix.
  static void NormalizeMatrix (TSparseColMatrix &M);

  /// Returns the maximum row index of the specified sparse column-major matrix.
  static int GetMaxRowIndex (const TSparseColMatrix &M);

  /// Converts sparse vector to dense vector.
  static void ToDenseVector (const TIntFltKdV &SparseVector, TFltV &DenseVector);

  /// Converts dense vector to sparse vector.
  static void ToSparseVector (const TFltV &DenseVector, TIntFltKdV &SparseVector);

  /// Returns the weighted sum of columns.
  static void GetWeightedSum (const TSparseColMatrix &M, const TIntFltKdV &Weights, TFltV &Sum);

  /// Converts the hash map to sparse vector ordered by key.
  static void ToIntFltKdV (TIntH &Map, TIntFltKdV &Vector);

  /// Concatenates the matrices M1 and M2 into matrix M row wise.
	static void ConcatenateMatricesRowWise (const TSparseColMatrix &M1, const TSparseColMatrix &M2, TSparseColMatrix &M);
  
  /// Sums the specified vectors (columns).
	static void GetSumVectors (const TSparseColMatrix &M, const TIntV &ColIndices, TFltV &Sum);

  /**
   * Computes the sums of the specified vectors (columns) for each group.
   * When there is more than N columns in a group, the columns are sampled
   * to size N, and the sum is computed on the sampled columns.
   */
  static void GetSumVectors (const TSparseColMatrix &M, const TVec<TIntV> &VectorColIndices, int N, TSparseColMatrix &Sums);
};

void TLAUtils::NormalizeMatrix (TSparseColMatrix &M)
{
  for (int i = 0; i < M.ColN; i++) {
    TIntFltKdV &Col = M.ColSpVV[i];
    if (TLinAlg::Norm(Col) > 0) {
      TLinAlg::Normalize(Col);
    }
  }
}

int TLAUtils::GetMaxRowIndex (const TSparseColMatrix &M)
{
  int MaxIndex = 0;

  for (int i = 0; i < M.ColN; i++) {
    for (int j = 0; j < M.ColSpVV[i].Len(); j++) {
      if (MaxIndex < M.ColSpVV[i][j].Key) {
        MaxIndex = M.ColSpVV[i][j].Key;
      }
    }
  }

  return MaxIndex;
}

void TLAUtils::ToDenseVector (const TIntFltKdV &SparseVector, TFltV &DenseVector)
{
  for (int i = 0; i < DenseVector.Len(); i++) {
    DenseVector[i] = 0;
  }
        
  for (int i = 0; i < SparseVector.Len(); i++) {
    const TIntFltKd &Pair = SparseVector[i];
    DenseVector[Pair.Key] = Pair.Dat;
  }
}

void TLAUtils::ToSparseVector (const TFltV &DenseVector, TIntFltKdV &SparseVector)
{
  for (int i = 0; i < DenseVector.Len(); i++) {
    if (DenseVector[i].Val != 0) {
      SparseVector.Add(TIntFltKd(i, DenseVector[i]));
    }
  }
}

void TLAUtils::GetWeightedSum (const TSparseColMatrix &M, const TIntFltKdV &Weights, TFltV &Sum)
{
  for (int i = 0; i < Weights.Len(); i++) {
    const TIntFltKd &Pair = Weights[i];
    TLinAlg::AddVec(Pair.Dat, M.ColSpVV[Pair.Key], Sum);
  }
}

void TLAUtils::ToIntFltKdV (TIntH &Map, TIntFltKdV &Vector)
{
  Map.SortByKey();

  for (TIntH::TIter it = Map.BegI(); it < Map.EndI(); it++) {
    TInt Property = it.GetKey();
    TFlt Count = static_cast<double>(it.GetDat().Val);
    Vector.Add(TIntFltKd(Property, Count));
  }
}

void TLAUtils::ConcatenateMatricesRowWise (const TSparseColMatrix &M1, const TSparseColMatrix &M2, TSparseColMatrix &M)
{
  IAssert(M1.ColN == M2.ColN);

  int NumCols = M1.ColN;
  int NumRows = M1.RowN + M2.RowN;

  M.ColSpVV.Gen(NumCols, NumCols);

  for (int i = 0; i < NumCols; i++) {
    TIntFltKdV &Col = M.ColSpVV[i];

    Col.AddV(M1.ColSpVV[i]);

    for (int j = 0; j < M2.ColSpVV[i].Len(); j++) {
      const TIntFltKd &Pair = M2.ColSpVV[i][j];
      Col.Add(TIntFltKd(M1.RowN + Pair.Key, Pair.Dat));
    }

    Col.Trunc();
  }

  M.ColN = NumCols;
  M.RowN = NumRows;
}

void TLAUtils::GetSumVectors (const TSparseColMatrix &M, const TIntV &ColIndices, TFltV &Sum)
{
  for (int i = 0; i < ColIndices.Len(); i++) {
    TLinAlg::AddVec(1.0, M.ColSpVV[ColIndices[i]], Sum);
  }
}

void TLAUtils::GetSample (const TIntV &Vector, int N, TRnd &Rnd, TIntV &Sample)
{
  IAssert(N <= Vector.Len());

  TIntSet Set;
  while (Set.Len() < N) {
    int RndIndex = Rnd.GetUniDevInt(0, Vector.Len() - 1);
    Set.AddKey(Vector[RndIndex]);
  }

  Set.GetKeyV(Sample);
}

void TLAUtils::GetSumVectors (const TSparseColMatrix &M, const TVec<TIntV> &VectorColIndices, int N, TSparseColMatrix &Sums)
{
	int NumCols = VectorColIndices.Len();
	int NumRows = M.RowN;

	Sums.ColSpVV.Gen(NumCols, NumCols);

	TFltV Sum(NumRows, NumRows);

	TRnd Rnd;

	for (int i = 0; i < NumCols; i++) {
		const TIntV &ColIndices = VectorColIndices[i];

		TIntV SampledColIndices;
		if (ColIndices.Len() > N) {
			GetSample(ColIndices, N, Rnd, SampledColIndices);
		} else {
			SampledColIndices = ColIndices;
		}

    TLAMisc::FillZero(Sum);

		GetSumVectors(M, SampledColIndices, Sum);

		ToSparseVector(Sum, Sums.ColSpVV[i]);

		Sums.ColSpVV[i].Trunc();
	}

	Sums.ColN = NumCols;
	Sums.RowN = NumRows;
}

#endif
