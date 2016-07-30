#include "stdafx.h"

//Code from https://github.com/nicholas-leonard/word2vec/blob/master/word2vec.c
//Rewritten for Snap and node2vec

void LearnVocab(TIntVV& WalksVV, TIntV& Vocab) {
  for( int i = 0; i < Vocab.Len(); i++) { Vocab[i] = 0; }
  for( int i = 0; i < WalksVV.GetXDim(); i++) {
    for( int j = 0; j < WalksVV.GetYDim(); j++) {
      Vocab[WalksVV(i,j)]++;
    }
  }
}

//Precompute unigram table using alias sampling method
void InitUnigramTable(TIntV& Vocab, TIntV& KTable, TFltV& UTable) {
  double TrainWordsPow = 0;
  double Pwr = 0.75;
  TFltV ProbV(Vocab.Len());
  for (int i = 0; i < Vocab.Len(); i++) {
    ProbV[i]=TMath::Power(Vocab[i],Pwr);
    TrainWordsPow += ProbV[i];
    KTable[i]=0;
    UTable[i]=0;
  }
  for (int i = 0; i < ProbV.Len(); i++) {
    ProbV[i] /= TrainWordsPow;
  }
  TIntV UnderV;
  TIntV OverV;
  for (int i = 0; i < ProbV.Len(); i++) {
    UTable[i] = ProbV[i] * ProbV.Len();
    if ( UTable[i] < 1 ) {
      UnderV.Add(i);
    } else {
      OverV.Add(i);
    }
  }
  while(UnderV.Len() > 0 && OverV.Len() > 0) {
    int Small = UnderV.Last();
    int Large = OverV.Last();
    UnderV.DelLast();
    OverV.DelLast();
    KTable[Small] = Large;
    UTable[Large] = UTable[Large] + UTable[Small] - 1;
    if (UTable[Large] < 1) {
      UnderV.Add(Large);
    } else {
      OverV.Add(Large);
    }
  } 
}

inline int RndUnigramInt(TIntV& KTable, TFltV& UTable, TRnd& Rnd) {
  TInt X = KTable[Rnd.GetUniDevInt(KTable.Len())];
  double Y = Rnd.GetUniDev();
  return Y < UTable[X] ? X : KTable[X];
}
//Initialize negative embeddings
void InitNegEmb(TIntV& Vocab, int& Dimensions, TFltVV& SynNeg) {
  SynNeg = TFltVV(Vocab.Len(),Dimensions);
  for (int i = 0; i < SynNeg.GetXDim(); i++) {
    for (int j = 0; j < SynNeg.GetYDim(); j++) {
      SynNeg(i,j) = 0;
    }
  }
}
//Initialize positive embeddings
void InitPosEmb(TIntV& Vocab, int& Dimensions, TRnd& Rnd, TFltVV& SynPos) {
  SynPos = TFltVV(Vocab.Len(),Dimensions);
  for (int i = 0; i < SynPos.GetXDim(); i++) {
    for (int j = 0; j < SynPos.GetYDim(); j++) {
      SynPos(i,j) = (Rnd.GetUniDev()-0.5)/Dimensions;
    }
  }
}

void TrainModel(TIntV& WalkV, TIntV& Vocab, int& Dimensions, int& WinSize,
 int& Iter, TRnd& Rnd, TIntV& KTable, TFltV& UTable, int& WordCntAll,
 TFltV& ExpTable, double& Alpha, double& StartAlpha, int AllWords,
 TFltVV& SynNeg, TFltVV& SynPos) {
  TFltV Neu1V(Dimensions);
  TFltV Neu1eV(Dimensions);
  int MAX_EXP = 6, EXP_TABLE_PRECISION = 1000;//There are 2 copies of this
  int TABLE_SIZE = MAX_EXP * EXP_TABLE_PRECISION * 2;
  int LocalIter = Iter, WordI = 0, NegSamN = 5;//15;
  while(1) {
    if ( WordCntAll%10000 == 0 ) {
      printf("%cAlpha: %lf  Progress: %.2lf%% ",13,Alpha,(double)WordCntAll*100/(double)(Iter*AllWords));fflush(stdout);
      Alpha = StartAlpha * (1 - WordCntAll / static_cast<double>(Iter * AllWords + 1));
      if ( Alpha < StartAlpha * 0.0001 ) { Alpha = StartAlpha * 0.0001; }
    }
    int Word = WalkV[WordI];
    for (int i = 0; i < Dimensions; i++) {
      Neu1V[i] = 0;
      Neu1eV[i] = 0;
    }
    int Offset = Rnd.GetUniDevInt() % WinSize;
    for (int a = Offset; a < WinSize * 2 + 1 - Offset; a++) {
      if (a == WinSize) { continue; }
      int CurrWordI = WordI - WinSize + a;
      if (CurrWordI < 0){ continue; }
      if (CurrWordI >= WalkV.Len()){ continue; }
      int CurrWord = WalkV[CurrWordI];
      for (int i = 0; i < Dimensions; i++) { Neu1eV[i] = 0; }
      //negative sampling
      for (int j = 0; j < NegSamN+1; j++) {
        int Target, Label;
        if (j == 0) {
          Target = Word;
          Label = 1;
        } else {
          Target = RndUnigramInt(KTable, UTable, Rnd);
          if (Target == Word) { continue; }
          Label = 0;
        }
        double Product = 0;
        for (int i = 0; i < Dimensions; i++) {
          Product += SynPos(CurrWord,i) * SynNeg(Target,i);
        }
        double Grad;//Gradient multiplied by learning rate
        if (Product > MAX_EXP) { Grad = (Label - 1) * Alpha; }
        else if (Product < -MAX_EXP) { Grad = Label * Alpha; }
        else { 
          double Exp = ExpTable[static_cast<int>(Product*EXP_TABLE_PRECISION)+TABLE_SIZE/2];
          Grad = (Label - 1 + 1 / (1 + Exp)) * Alpha;
        }
        for (int i = 0; i < Dimensions; i++) { 
          Neu1eV[i] += Grad * SynNeg(Target,i);
          SynNeg(Target,i) += Grad * SynPos(CurrWord,i);
        }
      }
      for (int i = 0; i < Dimensions; i++) {
        SynPos(CurrWord,i) += Neu1eV[i];
      }
    }
    WordCntAll++;
    WordI++;
    if (WordI >= WalkV.Len()) {
      LocalIter--;
      WordI = 0;
      if(LocalIter == 0) { break; }
    }
  }
}


void LearnEmbeddings(TIntVV& WalksVV, int& Dimensions, int& WinSize,
 int& Iter, int& Workers, TIntFltVH& EmbeddingsHV) {
  TIntIntH RnmH;
  TIntIntH RnmBackH;
  int NNodes = 0;
  //renaming nodes into consecutive numbers
  for (int i = 0; i < WalksVV.GetXDim(); i++) {
    for (int j = 0; j < WalksVV.GetYDim(); j++) {
      if ( RnmH.IsKey(WalksVV(i,j)) ) {
        WalksVV(i,j) = RnmH.GetDat(WalksVV(i,j));
      } else {
        RnmH.AddDat(WalksVV(i,j),NNodes);
        RnmBackH.AddDat(NNodes,WalksVV(i,j));
        WalksVV(i,j) = NNodes++;
      }
    }
  }
  TIntV Vocab(NNodes);
  LearnVocab(WalksVV,Vocab);
  TRnd Rnd(time(NULL));
  TIntV KTable(NNodes);
  TFltV UTable(NNodes);
  TFltVV SynNeg;
  TFltVV SynPos;
  InitPosEmb(Vocab, Dimensions, Rnd, SynPos);
  InitNegEmb(Vocab, Dimensions, SynNeg);
  InitUnigramTable(Vocab, KTable, UTable);
  int MAX_EXP = 6, EXP_TABLE_PRECISION = 1000;//There are 2 copies of this
  int TABLE_SIZE = MAX_EXP * EXP_TABLE_PRECISION * 2;
  TFltV ExpTable(TABLE_SIZE);
  double Alpha, StartAlpha;
  Alpha = StartAlpha = 0.025;//learning rate
  for (int i = 0; i < TABLE_SIZE; i++) {
    double Value = -MAX_EXP + static_cast<double>(i) / static_cast<double>(EXP_TABLE_PRECISION);
    ExpTable[i] = TMath::Power(TMath::E, Value);
  }
  int WordCntAll = 0;
  for (int i = 0; i < WalksVV.GetXDim(); i++) {
    TIntV CurrV(WalksVV.GetYDim());
    for (int j = 0; j < WalksVV.GetYDim(); j++) { CurrV[j] = WalksVV(i,j); }
    TrainModel(CurrV, Vocab, Dimensions, WinSize, Iter, Rnd, KTable, UTable,
     WordCntAll, ExpTable, Alpha, StartAlpha, WalksVV.GetXDim()*WalksVV.GetYDim(),
     SynNeg, SynPos);
  }
  for (int i = 0; i < SynPos.GetXDim(); i++) {
    TFltV CurrV(SynPos.GetYDim());
    for (int j = 0; j < SynPos.GetYDim(); j++) { CurrV[j] = SynPos(i, j); }
    EmbeddingsHV.AddDat(RnmBackH.GetDat(i),CurrV);
  }
}