#ifndef WORD_2_VEC_H
#define WORD_2_VEC_H

///Learns embeddings using SGD, Skip-gram with negative sampling.
void LearnEmbeddings(TIntVV& WalksVV, int& Dimensions, int& WinSize, int& Iter,
 bool& Verbose, TIntFltVH& EmbeddingsHV);

const int MaxExp = 6;
const int ExpTablePrecision = 10000;
const int TableSize = MaxExp*ExpTablePrecision*2;
const int NegSamN = 5;
const double StartAlpha = 0.025;

#endif //WORD_2_VEC_H
