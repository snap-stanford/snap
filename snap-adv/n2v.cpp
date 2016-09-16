#include "stdafx.h"
#include "n2v.h"

void node2vec(PWNet& InNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV) {
  //Preprocess transition probabilities
  PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);
  TIntV NIdsV;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    NIdsV.Add(NI.GetId());
  }
  //Generate random walks
  int64 AllWalks = (int64)NumWalks * NIdsV.Len();
  TVVec<TInt, int64> WalksVV(AllWalks,WalkLen);
  TRnd Rnd(time(NULL));
  int64 WalksDone = 0;
  for (int64 i = 0; i < NumWalks; i++) {
    NIdsV.Shuffle(Rnd);
#pragma omp parallel for schedule(dynamic)
    for (int64 j = 0; j < NIdsV.Len(); j++) {
      if ( Verbose && WalksDone%10000 == 0 ) {
        printf("\rWalking Progress: %.2lf%%",(double)WalksDone*100/(double)AllWalks);fflush(stdout);
      }
      TIntV WalkV;
      SimulateWalk(InNet, NIdsV[j], WalkLen, Rnd, WalkV);
      for (int64 k = 0; k < WalkV.Len(); k++) { 
        WalksVV.PutXY(i*NIdsV.Len()+j, k, WalkV[k]);
      }
      WalksDone++;
    }
  }
  if (Verbose) {
    printf("\n");
    fflush(stdout);
  }
  //Learning embeddings
  LearnEmbeddings(WalksVV, Dimensions, WinSize, Iter, Verbose, EmbeddingsHV);
}

void node2vec(PNGraph& InNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV) {
  PWNet NewNet = PWNet::New();
  for (TNGraph::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
    if (!NewNet->IsNode(EI.GetSrcNId())) { NewNet->AddNode(EI.GetSrcNId()); }
    if (!NewNet->IsNode(EI.GetDstNId())) { NewNet->AddNode(EI.GetDstNId()); }
    NewNet->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), 1.0);
  }
  node2vec(NewNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHV);
}

void node2vec(PNEANet& InNet, double& ParamP, double& ParamQ,
 int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV) {
  PWNet NewNet = PWNet::New();
  for (TNEANet::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
    if (!NewNet->IsNode(EI.GetSrcNId())) { NewNet->AddNode(EI.GetSrcNId()); }
    if (!NewNet->IsNode(EI.GetDstNId())) { NewNet->AddNode(EI.GetDstNId()); }
    NewNet->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), InNet->GetFltAttrDatE(EI,"weight"));
  }
  node2vec(NewNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHV);
}
