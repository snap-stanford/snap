#include "stdafx.h"
#include "velesn2v.h"

//Code from https://github.com/nicholas-leonard/word2vec/blob/master/word2vec.c
//Customized for SNAP and veles

void LearnVocab(PWNet& InNet, TIntIntH& RnmH, TIntV& Vocab) {
  for( int64 i = 0; i < Vocab.Len(); i++) { Vocab[i] = 0; }
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    Vocab[RnmH.GetDat(NI.GetId())]=NI.GetDeg();
  }
}

//Precompute unigram table using alias sampling method
void InitUnigramTable(TIntV& Vocab, TIntV& KTable, TFltV& UTable) {
  double TrainWordsPow = 0;
  double Pwr = 0.75;
  TFltV ProbV(Vocab.Len());
  for (int64 i = 0; i < Vocab.Len(); i++) {
    ProbV[i]=TMath::Power(Vocab[i],Pwr);
    TrainWordsPow += ProbV[i];
    KTable[i]=0;
    UTable[i]=0;
  }
  for (int64 i = 0; i < ProbV.Len(); i++) {
    ProbV[i] /= TrainWordsPow;
  }
  TIntV UnderV;
  TIntV OverV;
  for (int64 i = 0; i < ProbV.Len(); i++) {
    UTable[i] = ProbV[i] * ProbV.Len();
    if ( UTable[i] < 1 ) {
      UnderV.Add(i);
    } else {
      OverV.Add(i);
    }
  }
  while(UnderV.Len() > 0 && OverV.Len() > 0) {
    int64 Small = UnderV.Last();
    int64 Large = OverV.Last();
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

int64 RndUnigramInt(TIntV& KTable, TFltV& UTable, TRnd& Rnd) {
  TInt X = KTable[static_cast<int64>(Rnd.GetUniDev()*KTable.Len())];
  double Y = Rnd.GetUniDev();
  return Y < UTable[X] ? X : KTable[X];
}

//Initialize negative embeddings
void InitNegEmb(TIntV& Vocab, int& Dimensions, TVVec<TFlt, int64>& SynNeg) {
  SynNeg = TVVec<TFlt, int64>(Vocab.Len(), Dimensions);
  for (int64 i = 0; i < SynNeg.GetXDim(); i++) {
    for (int64 j = 0; j < SynNeg.GetYDim(); j++) {
      SynNeg(i,j) = 0;
    }
  }
}

//Initialize positive embeddings
void InitPosEmb(TIntV& Vocab, int& Dimensions, TRnd& Rnd, TVVec<TFlt, int64>& SynPos) {
  SynPos = TVVec<TFlt, int64>(Vocab.Len(),Dimensions);
  for (int64 i = 0; i < SynPos.GetXDim(); i++) {
    for (int j = 0; j < SynPos.GetYDim(); j++) {
      SynPos(i,j) =(Rnd.GetUniDev()-0.5)/Dimensions;
    }
  }
}

void TrainModel(TVec<TInt, int64>& WalkV, int& Dimensions, int& WinSize,
 bool& Verbose, TIntV& KTable, TFltV& UTable, TFltV& ExpTable, double& Alpha,
 TRnd& Rnd, TVVec<TFlt, int64>& SynNeg, TVVec<TFlt, int64>& SynPos) {
  TFltV Neu1V(Dimensions);
  TFltV Neu1eV(Dimensions);
  for (int64 WordI=0; WordI<WalkV.Len(); WordI++) {
    int64 Word = WalkV[WordI];
    for (int i = 0; i < Dimensions; i++) {
      Neu1V[i] = 0;
      Neu1eV[i] = 0;
    }
    int Offset = Rnd.GetUniDevInt() % WinSize;
    for (int a = Offset; a < WinSize * 2 + 1 - Offset; a++) {
      if (a == WinSize) { continue; }
      int64 CurrWordI = WordI - WinSize + a;
      if (CurrWordI < 0){ continue; }
      if (CurrWordI >= WalkV.Len()){ continue; }
      int64 CurrWord = WalkV[CurrWordI];
      for (int i = 0; i < Dimensions; i++) { Neu1eV[i] = 0; }
      //negative sampling
      for (int j = 0; j < NegSamN+1; j++) {
        int64 Target, Label;
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
        double Grad;                    //Gradient multiplied by learning rate
        if (Product > MaxExp) { Grad = (Label - 1) * Alpha; }
        else if (Product < -MaxExp) { Grad = Label * Alpha; }
        else {
          double Exp = ExpTable[static_cast<int>(Product*ExpTablePrecision)+TableSize/2];
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
  }
}

void Veles(PWNet& InNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, bool& Verbose, TIntFltVH& EmbeddingsHV) {
  //Preprocess transition probabilities
  PreprocessTransitionProbs(InNet, Verbose);
  TIntV NIdsV;
  TIntIntH RnmH;
  TIntIntH RnmBackH;
  int64 NNodes = 0;
  //renaming nodes into consecutive numbers
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    int CurrId=NI.GetId();
    NIdsV.Add(CurrId);
    RnmH.AddDat(CurrId,NNodes);
    RnmBackH.AddDat(NNodes++,CurrId);
  }
  //Generate random walks
  int64 AllWalks = (int64)NumWalks * NIdsV.Len();
  TRnd Rnd(time(NULL));
  int64 WalksDone = 0;
  TIntV Vocab(NNodes);
  LearnVocab(InNet, RnmH, Vocab);//Instead of number of occurences, use node degree
  TIntV KTable(NNodes);
  TFltV UTable(NNodes);
  TVVec<TFlt, int64> SynNeg;
  TVVec<TFlt, int64> SynPos;
  InitPosEmb(Vocab, Dimensions, Rnd, SynPos);
  InitNegEmb(Vocab, Dimensions, SynNeg);
  InitUnigramTable(Vocab, KTable, UTable);
  TFltV ExpTable(TableSize);
  double Alpha = StartAlpha;//learning rate
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < TableSize; i++ ) {
    double Value = -MaxExp + static_cast<double>(i) / static_cast<double>(ExpTablePrecision);
    ExpTable[i] = TMath::Power(TMath::E, Value);
  }
#pragma omp parallel for schedule(dynamic)
  for (int64 i = 0; i < NumWalks; i++) {
    NIdsV.Shuffle(Rnd);
    for (int64 j = 0; j < NIdsV.Len(); j++){
      if ( Verbose && WalksDone%100 == 0 ) {
        printf("\rWalking Progress: %.2lf%%",(double)WalksDone*100/(double)AllWalks);fflush(stdout);
      }
      TVec<TInt, int64>  WalkV;
      SimulateWalk(InNet, NIdsV[j](), WalkLen, ParamP, ParamQ, Rnd, WalkV);
      for (int64 k = 0; k < WalkLen; k++) { WalkV[k]=RnmH.GetDat(WalkV[k]); }
      TrainModel(WalkV, Dimensions, WinSize, Verbose, KTable, UTable, ExpTable,
       Alpha, Rnd, SynNeg, SynPos);
      WalksDone++;
    }
  }
  if (Verbose) {
    printf("\n");
    fflush(stdout);
  }
  for (int64 i = 0; i < SynPos.GetXDim(); i++) {
    TFltV CurrV(SynPos.GetYDim());
    for (int j = 0; j < SynPos.GetYDim(); j++) { CurrV[j] = SynPos(i, j); }
    EmbeddingsHV.AddDat(RnmBackH.GetDat(i),CurrV);
  }
}
