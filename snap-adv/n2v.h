#ifndef N2V_H
#define N2V_H

#include "stdafx.h"

#include "Snap.h"
#include "biasedrandomwalk.h"
#include "word2vec.h"

/// Calculates node2vec feature representation for nodes and writes them into EmbeddinsHV, see http://arxiv.org/pdf/1607.00653v1.pdf
void node2vec(PWNet& InNet, const double& ParamP, const double& ParamQ,
  const int& Dimensions, const int& WalkLen, const int& NumWalks,
  const int& WinSize, const int& Iter, const bool& Verbose,
  const bool& OutputWalks, TVVec<TInt, int64>& WalksVV,
  TIntFltVH& EmbeddingsHV); 

/// Version without walk output flag. For backward compatibility.
void node2vec(PWNet& InNet, const double& ParamP, const double& ParamQ,
  const int& Dimensions, const int& WalkLen, const int& NumWalks,
  const int& WinSize, const int& Iter, const bool& Verbose,
  TIntFltVH& EmbeddingsHV); 

/// Version for unweighted graphs
void node2vec(const PNGraph& InNet, const double& ParamP, const double& ParamQ,
  const int& Dimensions, const int& WalkLen, const int& NumWalks,
  const int& WinSize, const int& Iter, const bool& Verbose,
  const bool& OutputWalks, TVVec<TInt, int64>& WalksVV,
  TIntFltVH& EmbeddingsHV); 

/// Version for unweighted graphs without walk output flag. For backward compatibility.
void node2vec(const PNGraph& InNet, const double& ParamP, const double& ParamQ,
  const int& Dimensions, const int& WalkLen, const int& NumWalks,
  const int& WinSize, const int& Iter, const bool& Verbose,
  TIntFltVH& EmbeddingsHV); 

/// Version for weighted graphs. Edges must have TFlt attribute "weight"
void node2vec(const PNEANet& InNet, const double& ParamP, const double& ParamQ,
  const int& Dimensions, const int& WalkLen, const int& NumWalks,
  const int& WinSize, const int& Iter, const bool& Verbose,
  const bool& OutputWalks, TVVec<TInt, int64>& WalksVV,
  TIntFltVH& EmbeddingsHV); 

/// Version for weighted graphs. Edges must have TFlt attribute "weight". No walk output flag. For backward compatibility.
void node2vec(const PNEANet& InNet, const double& ParamP, const double& ParamQ,
  const int& Dimensions, const int& WalkLen, const int& NumWalks,
  const int& WinSize, const int& Iter, const bool& Verbose,
 TIntFltVH& EmbeddingsHV);
#endif //N2V_H
