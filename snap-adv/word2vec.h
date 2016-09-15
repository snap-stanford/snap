#ifndef WORD_2_VEC_H
#define WORD_2_VEC_H

///Learns embeddings using SGD, Skip-gram with negative sampling.
void LearnEmbeddings(TVVec<TInt, int64>& WalksVV, int& Dimensions, int& WinSize,
 int& Iter, bool& Verbose, TIntFltVH& EmbeddingsHV);

//Max x for e^x. Value taken from original word2vec code.
const int MaxExp = 6;

//Size of e^x precomputed table.
const int ExpTablePrecision = 10000;
const int TableSize = MaxExp*ExpTablePrecision*2;

//Number of negative samples. Value taken from original word2vec code.
const int NegSamN = 5;

//Learning rate for SGD. Value taken from original word2vec code.
const double StartAlpha = 0.025;

#endif //WORD_2_VEC_H
