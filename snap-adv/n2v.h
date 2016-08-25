#ifndef N2V_H
#define N2V_H

#include "stdafx.h"

///Does node2vec, see http://arxiv.org/pdf/1607.00653v1.pdf
void node2vec(PWNet& InNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV); 
#endif //N2V_H
