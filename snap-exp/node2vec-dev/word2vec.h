#ifndef WORD_2_VEC_H
#define WORD_2_VEC_H

///Learns embeddings using SGD, Skip-gram with negative sampling.
void LearnEmbeddings(TIntVV& WalksVV, int& Dimensions, int& WinSize, int& Iter,
 int& Workers, TIntFltVH& EmbeddingsHV);

#endif //WORD_2_VEC_H
