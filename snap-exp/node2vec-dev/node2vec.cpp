#include "stdafx.h"

void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile,
 int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter,
 int& Workers, double& ParamP, double& ParamQ, bool& Directed, bool& Weighted) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nAn algorithmic framework for representational learning on graphs."));
  InFile = Env.GetIfArgPrefixStr("--input:", "graph/karate.edgelist",
   "Input graph path");
  OutFile = Env.GetIfArgPrefixStr("--output:", "emb/karate.emb",
   "Output graph path");
  Dimensions = Env.GetIfArgPrefixInt("--dimensions:",128,
   "Number of dimensions. Default is 128");
  WalkLen = Env.GetIfArgPrefixInt("--walk-length:",10,
   "Length of walk per source. Default is 10");
  NumWalks = Env.GetIfArgPrefixInt("--num-walks:",40,
   "Number of walks per source. Default is 40");
  WinSize = Env.GetIfArgPrefixInt("--window-size:",10,
   "Context size for optimization. Default is 10");
  Iter = Env.GetIfArgPrefixInt("--iter:",1,
   "Number of epochs in SGD. Default is 1");
  Workers = Env.GetIfArgPrefixInt("--workers:",1,
   "Number of parallel workers. Default is 1");
  ParamP = Env.GetIfArgPrefixFlt("--p:",1,
   "Return hyperparameter. Default is 1");
  ParamQ = Env.GetIfArgPrefixFlt("--q:",1,
   "Inout hyperparameter. Default is 1");
}

void ReadGraph(TStr& InFile, bool& Directed, bool& Weighted, PWNet& InNet) {
  TFIn FIn(InFile);
  int LineCnt = 0;
  try {
    while (!FIn.Eof()) {
      TStr Ln;
      FIn.GetNextLn(Ln);
      TStrV Tokens;
      Ln.SplitOnStr(" ",Tokens);
      int SrcNId = Tokens[0].GetInt();
      int DstNId = Tokens[1].GetInt();
      double Weight = 1.0;
      if (Weighted) { Weight = Tokens[2].GetFlt(); }
      if (!InNet->IsNode(SrcNId)){ InNet->AddNode(SrcNId); }
      if (!InNet->IsNode(DstNId)){ InNet->AddNode(DstNId); }
      InNet->AddEdge(SrcNId,DstNId,Weight);
      if (!Directed){ InNet->AddEdge(DstNId,SrcNId,Weight); }
      LineCnt++;
    }
    printf("Read %d lines from %s\n", LineCnt, InFile.CStr());
  } catch (PExcept Except) {
    printf("Read %d lines from %s, then %s\n", LineCnt, InFile.CStr(),
     Except->GetStr().CStr());
  }
}

void WriteOutput(TStr& OutFile, TIntFltVH& EmbeddingsHV) {
  TFOut FOut(OutFile);
  bool First = 1;
  for (int i = EmbeddingsHV.FFirstKeyId(); EmbeddingsHV.FNextKeyId(i);) {
    if (First) {
      FOut.PutInt(EmbeddingsHV.Len());
      FOut.PutCh(' ');
      FOut.PutInt(EmbeddingsHV[i].Len());
      FOut.PutLn();
      First = 0;
    }
    FOut.PutInt(EmbeddingsHV.GetKey(i));
    for (int j = 0; j < EmbeddingsHV[i].Len(); j++) {
      FOut.PutCh(' ');
      FOut.PutFlt(EmbeddingsHV[i][j]);
    }
    FOut.PutLn();
  }
}

struct TWalkDat {//For multithreading purposes
  PWNet& InNet;
  TIntV& FirstNIdsV;
  int& WalkLen;
  TRnd& Rnd;
  int WalkId;
  int& NWalks;
  int& WalksDone;
  TIntVV& WalksVV;
  TWalkDat (PWNet& InNet, TIntV& FirstNIdsV, int& WalkLen, TRnd& Rnd, int WalkId,
   int& NWalks, int& WalksDone, TIntVV& WalksVV) : InNet(InNet), FirstNIdsV(FirstNIdsV),
   WalkLen(WalkLen), Rnd(Rnd), WalkId(WalkId), NWalks(NWalks), WalksDone(WalksDone),
   WalksVV(WalksVV) {}
};

void* WalkGen (void* ThrDat) {
  TWalkDat* Dat = (TWalkDat*) ThrDat;
  for (int i = 0; i < Dat->FirstNIdsV.Len(); i++) {
    if ( Dat->WalksDone%10000 == 0 ) {
      printf("%cWalking Progress: %.2lf%%",13,(double)Dat->WalksDone*100/(double)Dat->NWalks);fflush(stdout);
    }
    TIntV WalkV;
    SimulateWalk(Dat->InNet, Dat->FirstNIdsV[i], Dat->WalkLen, Dat->Rnd, WalkV);
    for (int k = 0; k < WalkV.Len(); k++) { 
      Dat->WalksVV.PutXY(Dat->WalkId+i, k, WalkV[k]);
    }
    Dat->WalksDone++;
  }
  pthread_exit(NULL);
} 

int main(int argc, char* argv[]) {
  TStr InFile,OutFile;
  int Dimensions, WalkLen, NumWalks, WinSize, Iter, Workers;
  double ParamP, ParamQ;
  bool Directed, Weighted;
  ParseArgs(argc, argv, InFile, OutFile, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, Workers, ParamP, ParamQ, Directed, Weighted);
  PWNet InNet = PWNet::New();
  ReadGraph(InFile, Directed, Weighted, InNet);
  //Preprocess transition probabilities
  PreprocessTransitionProbs(InNet, ParamP, ParamQ, Workers);
  TIntV NIdsV;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    NIdsV.Add(NI.GetId());
  }
  //Generate random walks
  int AllWalks = NumWalks*NIdsV.Len();
  TIntVV WalksVV(AllWalks,WalkLen);
  TRnd Rnd(time(NULL));
  pthread_t Threads[Workers];
  TWalkDat* WalkDat[Workers];
  int CurrWalk = -1, WalksDone = 0;
  for (int i = 0; i < NumWalks; i++) {
    NIdsV.Shuffle(Rnd);
    for (int j = 0; j < NIdsV.Len(); j++){
      if ((CurrWalk+1)*AllWalks/Workers <= i*NIdsV.Len()+j) {
        CurrWalk++;
        WalkDat[CurrWalk] = new TWalkDat(InNet, *(new TIntV), WalkLen, Rnd, i*NIdsV.Len()+j,
         AllWalks, WalksDone, WalksVV);
      }
      WalkDat[CurrWalk]->FirstNIdsV.Add(NIdsV[j]);
    }
  }
  for (int i = 0; i < Workers; i++) {
    pthread_create(&Threads[i], NULL, WalkGen, (void *)WalkDat[i]);
  }
  for (int i = 0; i < Workers; i++) { pthread_join(Threads[i], NULL); }
  printf("\n");fflush(stdout);
  //Learning embeddings
  TIntFltVH EmbeddingsHV;
  LearnEmbeddings(WalksVV, Dimensions, WinSize, Iter, Workers, EmbeddingsHV);
  WriteOutput(OutFile, EmbeddingsHV);
  return 0;
}
