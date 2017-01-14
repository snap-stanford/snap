#include "stdafx.h"

#include "velesn2v.h"

#ifdef USE_OPENMP
#include <omp.h>
#endif

void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile,
 int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize,  bool& Verbose,
 double& ParamP, double& ParamQ, bool& Directed, bool& Weighted) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nAn algorithmic framework for representational learning on graphs."));
  InFile = Env.GetIfArgPrefixStr("-i:", "graph/karate.edgelist",
   "Input graph path");
  OutFile = Env.GetIfArgPrefixStr("-o:", "emb/karate.emb",
   "Output graph path");
  Dimensions = Env.GetIfArgPrefixInt("-d:", 128,
   "Number of dimensions. Default is 128");
  WalkLen = Env.GetIfArgPrefixInt("-l:", 80,
   "Length of walk per source. Default is 80");
  NumWalks = Env.GetIfArgPrefixInt("-r:", 10,
   "Number of walks per source. Default is 10");
  WinSize = Env.GetIfArgPrefixInt("-k:", 10,
   "Context size for optimization. Default is 10");
  ParamP = Env.GetIfArgPrefixFlt("-p:", 1,
   "Return hyperparameter. Default is 1.");
  ParamQ = Env.GetIfArgPrefixFlt("-q:", 1,
   "Inout hyperparameter. Default is 1.");
  Verbose = Env.IsArgStr("-v", "Verbose output.");
  Directed = Env.IsArgStr("-dr", "Graph is directed.");
  Weighted = Env.IsArgStr("-w", "Graph is weighted.");
}

void ReadGraph(TStr& InFile, bool& Directed, bool& Weighted, bool& Verbose, PWNet& InNet) {
  TFIn FIn(InFile);
  int64 LineCnt = 0;
  try {
    while (!FIn.Eof()) {
      TStr Ln;
      FIn.GetNextLn(Ln);
      TStrV Tokens;
      Ln.SplitOnWs(Tokens);
      int64 SrcNId = Tokens[0].GetInt();
      int64 DstNId = Tokens[1].GetInt();
      double Weight = 1.0;
      if (Weighted) { Weight = Tokens[2].GetFlt(); }
      if (!InNet->IsNode(SrcNId)){ InNet->AddNode(SrcNId); }
      if (!InNet->IsNode(DstNId)){ InNet->AddNode(DstNId); }
      InNet->AddEdge(SrcNId,DstNId,Weight);
      if (!Directed){ InNet->AddEdge(DstNId,SrcNId,Weight); }
      LineCnt++;
    }
    if (Verbose) { printf("Read %ld lines from %s\n", LineCnt, InFile.CStr()); }
  } catch (PExcept Except) {
    if (Verbose) {
      printf("Read %ld lines from %s, then %s\n", LineCnt, InFile.CStr(),
       Except->GetStr().CStr());
    }
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
    for (int64 j = 0; j < EmbeddingsHV[i].Len(); j++) {
      FOut.PutCh(' ');
      FOut.PutFlt(EmbeddingsHV[i][j]);
    }
    FOut.PutLn();
  }
}

int main(int argc, char* argv[]) {
  TStr InFile,OutFile;
  int Dimensions, WalkLen, NumWalks, WinSize;
  double ParamP, ParamQ;
  bool Directed, Weighted, Verbose;
  ParseArgs(argc, argv, InFile, OutFile, Dimensions, WalkLen, NumWalks, WinSize,
   Verbose, ParamP, ParamQ, Directed, Weighted);
  PWNet InNet = PWNet::New();
  TIntFltVH EmbeddingsHV;
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  Veles(InNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Verbose,
   EmbeddingsHV);
  //printf("Did the calculations\n");fflush(stdout);
  WriteOutput(OutFile, EmbeddingsHV);
  return 0;
}
