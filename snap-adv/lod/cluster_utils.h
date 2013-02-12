#ifndef SNAP_CLUSTER_UTILS_H
#define SNAP_CLUSTER_UTILS_H

#include "stdafx.h"
#include "typedefs.h"
#include "kmeans/kmpp.h"

/**
 * Author: Klemen Simonic
 *
 * This module is a wrapper around K-means++ implementation.
 */
class TClusterUtils {
public:
  /**
   * Computes the clusters using K-means++ algorithm.
   * Input:
   * - M: sparse column-major matrix (data); the matrix columns should be normalized.
   * - K: mumber of clusters.
   * - NumIerations: number of iterations.
   * Output:
   * - Assigments: cluster assigment for each point (column in the input matrix).
   * - Clusters: points belonging to each cluster.
   */
	static void GetClusters (const TSparseColMatrix &M, int K, int NumIterations, TIntV &Assigments, TVec<TIntV> &Clusters);

  /// Prints the sizes of the specified clusters.
	static void PrintClusterSizes (const TVec<TIntV> &Clusters, TSOut &Output);
  /// Prints the objects for each cluster.
	static void PrintClusters (const TVec<TIntV> &Clusters, const TIntV &Objects, const TStrSet &NodeStrs, TSOut &output);
};


void TClusterUtils::GetClusters (const TSparseColMatrix &Matrix, int K, int NumIterations, TIntV &Assigments, TVec<TIntV> &Clusters) 
{
  PNotify Not = TNotify::StdNotify;
  TSparseKMeans SparseKMeans(&Matrix, K, NumIterations, Not);

  SparseKMeans.Init();

  SparseKMeans.Apply();

  Assigments = SparseKMeans.GetAssignment();

  Clusters.Gen(K, K);
  for (int i = 0; i < Assigments.Len(); i++) {
    int Ob = i;
    int Cluster = Assigments[i];

    Clusters[Cluster].Add(Ob);
  }
}

void TClusterUtils::PrintClusterSizes (const TVec<TIntV> &Clusters, TSOut &Output)
{
  Output.PutStr("ClusterId\tClusterSize\n");
  for (int i = 0; i < Clusters.Len(); i++) {
    Output.PutInt(i);
    Output.PutStr("\t");
    Output.PutInt(Clusters[i].Len());
    Output.PutStr("\n");
  }
}

void TClusterUtils::PrintClusters (const TVec<TIntV> &Clusters, const TIntV &Objects, const TStrSet &NodeStrs, TSOut &Output)
{
  for (int i = 0; i < Clusters.Len(); i++) {
    Output.PutStr("--> ClusterId: ");
    Output.PutInt(i);
    Output.PutStr("\n");

    for (int j = 0; j < Clusters[i].Len(); j++) {
      Output.PutStr(NodeStrs[Objects[Clusters[i][j]]]);
      Output.PutStr("\n");
    }

    Output.PutStr("\n");
  }
}

#endif
