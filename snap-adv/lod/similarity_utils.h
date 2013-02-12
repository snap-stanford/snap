#ifndef SNAP_SIMILARITY_UTILS_H
#define SNAP_SIMILARITY_UTILS_H

#include "stdafx.h"
#include "kmeans/kmpp.h"
#include "typedefs.h"
#include "la_utils.h"

/**
 * Author: Klemen Simonic
 *
 * A module providing computation of dot products or similarities
 * between a vector and a matrix.
 */
class TSimilarityUtils {
public:
  /**
   * Returns the dot products (similarities) between the specified 
   * column and the other specified columns.
   */
  static void GetDotProducts (const TFltV &Col, const TSparseColMatrix &M, const TIntV &OtherColIndices, TIntFltKdV &Similarities);
    
  /**
   * Returns descendingly ordered dot products (similarities) between the
   * specified column and the other specified columns.
   */
  static void GetOrderedDotProducts (const TFltV &Col, const TSparseColMatrix &M, const TIntV &OtherColIndices, TIntFltKdV &Similarities);

  /**
   * Returns the similarities between the columns in the specified 
   * sparse column-major matrix. The similarities are computed between
   * each pair of columns and ordered in decreasing ordered.
   *
   * The method has additional inputs:
   * - MaxNumSimilarities: the maximum number of similarities stored for each column.
   * - NumThreads: number of parallel computations.
   */
  static void ComputeSimilarities (const TSparseColMatrix &M, int MaxNumSimilarities, int NumThreads, TVec<TIntFltKdV> &Similarities);

  /**
   * Returns the similarities between the columns in the specified 
   * sparse column-major matrix. This method is a generalization of
   * the previous method. Instead of computing similarities between 
   * each pair of columns (which can be extremely expensive), it only
   * computes similarities between the columns that bellong to the same 
   * subset (cluster).
   *
   * The additional input parameters are:
   * - Assigments: specifies the subset or cluster for each column.
   * - Clusters: (disjoint) subsets of columns that are used to speed 
   *             up the similarity computation.
   *
   * This method can be effectively used with TClusterUtils module.
   */
  static void ComputeSimilarities (const TSparseColMatrix &Matrix, const TIntV &Assigments, const TVec<TIntV> &Clusters, int MaxNumSimilarities, int NumThreads, TVec<TIntFltKdV> &Similarities);

  /// Prints at most N nearest elements (e.g. objects, properties) of the specified similarity vector.
  static void PrintSimilarities (const TIntFltKdV &Similarities, const TIntV &Elements, const TStrSet &ElementStrs, int N, TSOut &Output);

  /// Prints at most N nearest elements (e.g. objects, properties) for each specified similarity vector.
  static void PrintSimilarities (const TVec<TIntFltKdV> &Similarities, const TIntV &Elements, const TStrSet &ElementStrs, int N, TSOut &Output);
};

void TSimilarityUtils::GetDotProducts (const TFltV &Col, const TSparseColMatrix &M, const TIntV &OtherColIndices, TIntFltKdV &Similarities)
{
  for (int i = 0; i < OtherColIndices.Len(); i++) {
    int ColIndex = OtherColIndices[i];

    Similarities[i].Key = ColIndex;
    Similarities[i].Dat = TLinAlg::DotProduct(Col, M.ColSpVV[ColIndex]);
  }
}

void TSimilarityUtils::GetOrderedDotProducts (const TFltV &Col, const TSparseColMatrix &M, const TIntV &OtherColIndices, TIntFltKdV &Similarities)
{
  GetDotProducts(Col, M, OtherColIndices, Similarities);

  Similarities.SortCmp(TIntFltKdComp());
}

void TSimilarityUtils::ComputeSimilarities (const TSparseColMatrix &M, int MaxNumSimilarities, int NumThreads, TVec<TIntFltKdV> &Similarities)
{
  int NumCols = M.GetCols();

  TIntV Assigments(NumCols, NumCols);
  for (int i = 0; i < NumCols; i++) {
    Assigments[i] = 0;
  }

  TVec<TIntV> Clusters(1, 1);
  Clusters[0].Gen(NumCols, NumCols);
  for (int i = 0; i < NumCols; i++) {
    Clusters[0][i] = i;
  }

  ComputeSimilarities(M, Assigments, Clusters, MaxNumSimilarities, NumThreads, Similarities);
}

void TSimilarityUtils::ComputeSimilarities (const TSparseColMatrix &M, const TIntV &Assigments, const TVec<TIntV> &Clusters, int MaxNumSimilarities, int NumThreads, TVec<TIntFltKdV> &Similarities)
{
  int NumCols = M.GetCols();
  int NumRows = M.GetRows();

  Similarities.Gen(NumCols, NumCols);

  omp_set_num_threads(NumThreads);

  TVec<TFltV> TmpDenseVectors(NumThreads, NumThreads);
  for (int i = 0; i < NumThreads; i++) {
    TmpDenseVectors[i].Gen(NumRows, NumRows);
  }

  int Count = 0;

  #pragma omp parallel for
  for (int i = 0; i < NumCols; i++) {
    int ThreadNum = omp_get_thread_num();

    TFltV &DenseVector = TmpDenseVectors[ThreadNum];
    TLAUtils::ToDenseVector(M.ColSpVV[i], DenseVector);

    int ClusterIndex = Assigments[i];
    const TIntV &Cluster = Clusters[ClusterIndex];
    int ClusterSize = Cluster.Len();

    TIntFltKdV &Sims = Similarities[i];
    Sims.Gen(ClusterSize, ClusterSize);

    GetOrderedDotProducts(DenseVector, M, Cluster, Sims);

    int NewSize = Sims.Len() > MaxNumSimilarities ? MaxNumSimilarities : Sims.Len();
    Sims.Trunc(NewSize);

    #pragma omp atomic
    Count++;

    printf("%f\r", Count / (double)NumCols);
  }
}

void TSimilarityUtils::PrintSimilarities (const TIntFltKdV &Similarities, const TIntV &Elements, const TStrSet &ElementStrs, int N, TSOut &Output)
{
  for (int i = 0; i < Similarities.Len() && i < N; i++) {
    const TIntFltKd &Sim = Similarities[i];

    Output.PutFlt(Sim.Dat);
    Output.PutStr("\t");
    Output.PutStr(ElementStrs[Elements[Sim.Key]]);
    Output.PutStr("\n");
  }
}

void TSimilarityUtils::PrintSimilarities (const TVec<TIntFltKdV> &Similarities, const TIntV &Elements, const TStrSet &ElementStrs, int N, TSOut &Output)
{
  for (int i = 0; i < Similarities.Len(); i++) {
    int Elem = Elements[i];
            
    Output.PutStr("-->");
    Output.PutStr(ElementStrs[Elem]);
    Output.PutStr("\n");

    PrintSimilarities(Similarities[i], Elements, ElementStrs, N, Output);

    Output.PutStr("\n");
  }
}

#endif
