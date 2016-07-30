#ifndef WORD_2_VEC_H
#define WORD_2_VEC_H

void LearnEmbeddings(TIntVV& WalksVV, int& Dimensions, int& WinSize, int& Iter,
 int& Workers, TIntFltVH& EmbeddingsHV);
void LearnVocab(TIntVV& WalksVV, TIntV& Vocab);
void InitNegEmb(TIntV& Vocab, int& Dimensions, TFltVV& SynNeg);
void InitPosEmb(TIntV& Vocab, int& Dimensions, TRnd& Rnd, TFltVV& SynPos);
void InitUnigramTable(TIntV& Vocab, TIntV& KTable, TFltV& UTable);
void TrainModel(TIntV& WalkV, TIntV& Vocab, int& Dimensions, int& Winsize,
 int& Iter, TRnd& Rnd, TIntV& KTable, TFltV& UTable, int& WordCntAll,
 TFltVV& SynNeg, TFltVV& SynPos);

#endif //WORD_2_VEC_H
