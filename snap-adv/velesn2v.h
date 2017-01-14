#ifndef N2VVEL_H
#define N2VVEL_H

#include "stdafx.h"

#include "Snap.h"
#include "velesrandomwalk.h"

///Calculates node2vec feature representation for nodes and writes them into EmbeddinsHV
void Veles(PWNet& InNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, bool& Verbose, TIntFltVH& EmbeddingsHV); 

//Max x for e^x. Value taken from original word2vec code.
const int MaxExp = 6;

//Size of e^x precomputed table.
const int ExpTablePrecision = 10000;
const int TableSize = MaxExp*ExpTablePrecision*2;

//Number of negative samples. Value taken from original word2vec code.
const int NegSamN = 5;

//Learning rate for SGD. Value taken from original word2vec code.
const double StartAlpha = 0.025;

#endif //N2VVEL_H
