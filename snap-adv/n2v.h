#ifndef N2V_H
#define N2V_H

#include "stdafx.h"

#include "Snap.h"
#include "biasedrandomwalk.h"
#include "word2vec.h"

/// Calculates node2vec feature representation for nodes and writes them into EmbeddinsHV, see http://arxiv.org/pdf/1607.00653v1.pdf
void node2vec(PWNet& InNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV); 
#endif //N2V_H
