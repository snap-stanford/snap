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
  for (int i = EmbeddingsHV.FFirstKeyId(); EmbeddingsHV.FNextKeyId(i);) {
    FOut.PutInt(EmbeddingsHV.GetKey(i));
    FOut.PutCh(':');
    for (int j = 0; j < EmbeddingsHV[i].Len(); j++) {
      FOut.PutCh(' ');
      FOut.PutFlt(EmbeddingsHV[i][j]);
    }
    FOut.PutLn();
  }
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
  PreprocessTransitionProbs(InNet, ParamP, ParamQ);
  TIntV NIdsV;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    NIdsV.Add(NI.GetId());
  }
  //Generate random walks
  TIntVV WalksVV(NumWalks*NIdsV.Len(),WalkLen);
  TRnd Rnd(time(NULL));
  for (int i = 0; i < NumWalks; i++) {
    NIdsV.Shuffle(Rnd);
    for (int j = 0; j < NIdsV.Len(); j++){
      TIntV WalkV;
      SimulateWalk(InNet, NIdsV[j], WalkLen, Rnd, WalkV);
      for (int k = 0; k < WalkV.Len(); k++) { 
        WalksVV.PutXY(j+i*NIdsV.Len(),k,WalkV[k]);
      }
    }
  }
  printf("walks done\n");fflush(stdout);
  FILE* inter = fopen("tmp.txt","w+");
  for(int i=0;i<NumWalks*NIdsV.Len(); i++){
    for(int j=0; j<WalkLen;j++){
      fprintf(inter,"%d ",WalksVV(i,j));
    }
    fprintf(inter,"\n");
  }
  fclose(inter);
  //Learning embeddings
  TIntFltVH EmbeddingsHV;
  LearnEmbeddings(WalksVV, Dimensions, WinSize, Iter, Workers, EmbeddingsHV);
  WriteOutput(OutFile, EmbeddingsHV);
  return 0;
}