#include "stdafx.h"
#include "Snap.h"
#include "rolx.h"

void PrintFeatures(const TIntFtrH& Features) {
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    printf("%d: [", HI.GetKey()());
    const TFtr& f = HI.GetDat();
    for (int i = 0; i < f.Len(); ++i) {
      if (i > 0) {
        printf(",");
      }
      printf("%f", f[i]());
    }
    printf("]\n");
  }
}

TIntFtrH CreateEmptyFeatures(const PUNGraph Graph) {
  TIntFtrH EmptyFeatures;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    EmptyFeatures.AddDat(TInt(NI.GetId()), TFtr());
  }
  return EmptyFeatures;
}

TIntFtrH CreateEmptyFeatures(const TIntFtrH& Features) {
  TIntFtrH EmptyFeatures;
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    EmptyFeatures.AddDat(HI.GetKey(), TFtr());
  }
  return EmptyFeatures;
}

int GetNumFeatures(const TIntFtrH& Features) {
  return Features.BegI().GetDat().Len();
}

TFtr GetNthFeature(const TIntFtrH& Features, const int n) {
  TFtr NthFeature;
  IAssert(0 <= n && n < GetNumFeatures(Features));
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    NthFeature.Add(HI.GetDat()[n]);
  }
  return NthFeature;
}

TIntFtrH ExtractFeatures(const PUNGraph Graph) {
  TIntFtrH Features = CreateEmptyFeatures(Graph);
  printf("finish createEmptyFeatures()\n");
  AddNeighborhoodFeatures(Graph, Features);
  printf("finish neighborhood features\n");
  AddRecursiveFeatures(Graph, Features);
  printf("finish recursive features\n");
  return Features;
}

void AddNeighborhoodFeatures(const PUNGraph Graph, TIntFtrH& Features) {
  AddLocalFeatures(Graph, Features);
  printf("finish recursive features\n");
  AddEgonetFeatures(Graph, Features);
  printf("finish egonet features\n");
}

void AddRecursiveFeatures(const PUNGraph Graph, TIntFtrH& Features) {
  int SimilarityThreshold = 0;
  TIntFtrH RetainedFeatures = Features;
  while (true) {
    TIntFtrH NewFeatures = GenerateRecursiveFeatures(Graph, RetainedFeatures);
    RetainedFeatures = PruneRecursiveFeatures(Graph, Features, NewFeatures,
        SimilarityThreshold);
    if (0 == GetNumFeatures(RetainedFeatures)) {
      break;
    }
    AppendFeatures(Features, RetainedFeatures);
    ++SimilarityThreshold;
    printf("recursion %d: ", SimilarityThreshold);
    printf("current feature number %d\n", GetNumFeatures(Features));
  }
}

void AddLocalFeatures(const PUNGraph Graph, TIntFtrH& Features) {
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Features.GetDat(TInt(NI.GetId())).Add(NI.GetInDeg());
  }
}

void AddEgonetFeatures(const PUNGraph Graph, TIntFtrH& Features) {
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    int NId = NI.GetId();
    int ArndEdges;
    PUNGraph Egonet = TSnap::GetEgonet(Graph, NId, ArndEdges);
    Features.GetDat(NId).Add(Egonet->GetEdges());
    Features.GetDat(NId).Add(ArndEdges);
  }
}

TIntFtrH GenerateRecursiveFeatures(const PUNGraph Graph,
    const TIntFtrH& CurrFeatures) {
  const int NumCurrFeatures = GetNumFeatures(CurrFeatures);
  if (0 == NumCurrFeatures) {
    return CurrFeatures;
  }
  TIntFtrH NewFeatures = CreateEmptyFeatures(CurrFeatures);
  for (int i = 0; i < NumCurrFeatures; ++i) {
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      float Sum = 0;
      for (int j = 0; j < NI.GetInDeg(); ++j) {
        int NbrNId = NI.GetInNId(j);
        Sum += CurrFeatures.GetDat(NbrNId)[i]();
      }
      NewFeatures.GetDat(NI.GetId()).Add(Sum);
      NewFeatures.GetDat(NI.GetId()).Add(0 == NI.GetInDeg()?
          0 : (float(Sum) / NI.GetInDeg()));
    }
  }
  return NewFeatures;
}

TIntFtrH PruneRecursiveFeatures(const PUNGraph Graph, const TIntFtrH& Features,
    const TIntFtrH& NewFeatures, const int SimilarityThreshold) {
  TIntFtrH AllFeatures = CreateEmptyFeatures(Features);
  AppendFeatures(AllFeatures, Features);
  AppendFeatures(AllFeatures, NewFeatures);
  const float BinFraction = 0.5;
  TIntFtrH LogBinFeatures = CalcVerticalLogBinning(AllFeatures, BinFraction);
  PUNGraph FeatureGraph = BuildFeatureGraph(LogBinFeatures,
      SimilarityThreshold);
  return SummarizeConnectedComponents(FeatureGraph, Features, NewFeatures);
}

void AppendFeatures(TIntFtrH& DstFeatures, const TIntFtrH& SrcFeatures,
    const int ColIdx) {
  for (TIntFtrH::TIter HI = SrcFeatures.BegI();
      HI < SrcFeatures.EndI();
      HI++) {
    const TFtr& f = HI.GetDat();
    if (ColIdx >= 0) {
      DstFeatures.GetDat(HI.GetKey()).Add(f[ColIdx]);
    } else {
      for (int i = 0; i < f.Len(); ++i) {
        DstFeatures.GetDat(HI.GetKey()).Add(f[i]);
      }
    }
  }
}

TIntFtrH CalcVerticalLogBinning(const TIntFtrH& Features,
    const float BinFraction) {
  const int NumFeatures = GetNumFeatures(Features);
  TIntFtrH LogBinFeatures = CreateEmptyFeatures(Features);
  for (int i = 0; i < NumFeatures; ++i) {
    TVec<TInt> SortedNId = GetNIdSorted(Features, i);
    AssignBinValue(SortedNId, BinFraction, LogBinFeatures);
  }
  return LogBinFeatures;
}

PUNGraph BuildFeatureGraph(const TIntFtrH& LogBinFeatures,
    const int SimilarityThreshold) {
  PUNGraph FeatureGraph = PUNGraph::New();
  const int NumFeatures = GetNumFeatures(LogBinFeatures);
  for (int i = 0; i < NumFeatures; ++i) {
    FeatureGraph->AddNode(i);
  }
  for (int i = 0; i < NumFeatures; ++i) {
    TFtr IthFeature = GetNthFeature(LogBinFeatures, i);
    for (int j = i + 1; j < NumFeatures; ++j) {
      TFtr JthFeature = GetNthFeature(LogBinFeatures, j);
      if (IsSimilarFeature(IthFeature, JthFeature, SimilarityThreshold) &&
          !FeatureGraph->IsEdge(i, j)) {
        FeatureGraph->AddEdge(i, j);
      }
    }
  }
  return FeatureGraph;
}

TIntFtrH SummarizeConnectedComponents(const PUNGraph FeatureGraph,
    const TIntFtrH& Features, const TIntFtrH& NewFeatures) {
  TCnComV Wcc;
  TSnap::GetWccs(FeatureGraph, Wcc);
  TVec<TInt> RetainedIdx;
  for (int i = 0; i < Wcc.Len(); ++i) {
    RetainedIdx.Add(Wcc[i][0]);
  }
  RetainedIdx.Sort();

  TIntFtrH RetainedFeatures = CreateEmptyFeatures(Features);
  const int StartIdxNewFeatures = GetNumFeatures(Features);
  for (int i = 0; i < RetainedIdx.Len(); ++i) {
    const int IdxNewFeatures = RetainedIdx[i] - StartIdxNewFeatures;
    if (IdxNewFeatures >= 0) {
      AppendFeatures(RetainedFeatures, NewFeatures, IdxNewFeatures);
    }
  }
  return RetainedFeatures;
}

TVec<TInt> GetNIdSorted(const TIntFtrH& Features, const int Idx) {
  THash<TInt, TFlt> F;
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    F.AddDat(HI.GetKey(), HI.GetDat()[Idx]);
  }
  F.SortByDat();
  TVec<TInt> SortedNId;
  for (THash<TInt, TFlt>::TIter HI = F.BegI(); HI < F.EndI(); HI++) {
    SortedNId.Add(HI.GetKey());
  }
  return SortedNId;
}

void AssignBinValue(const TVec<TInt>& SortedNId, const float BinFraction,
    TIntFtrH& LogBinFeatures) {
  int NumNodes = LogBinFeatures.Len();
  int NumAssigned = 0;
  int BinValue = 0;
  while (NumAssigned < NumNodes) {
    int NumToAssign = ceil(BinFraction * (NumNodes - NumAssigned));
    for (int i = NumAssigned; i < NumAssigned + NumToAssign; ++i) {
      int NId = SortedNId[i];
      LogBinFeatures.GetDat(NId).Add(BinValue);
    }
    NumAssigned += NumToAssign;
    ++BinValue;
  }
}

bool IsSimilarFeature(const TFtr& F1, const TFtr& F2,
    const int SimilarityThreshold) {
  IAssert(F1.Len() == F2.Len());
  for (int i = 0; i < F1.Len(); ++i) {
    if (TFlt::Abs(F1[i] - F2[i]) > SimilarityThreshold) {
      return false;
    }
  }
  return true;
}

TFltVV ConvertFeatureToMatrix(const TIntFtrH& Features,
    const TIntIntH& NodeIdMtxIdH) {
  const int NumNodes = Features.Len();
  const int NumFeatures = GetNumFeatures(Features);
  TFltVV FeaturesMtx(NumNodes, NumFeatures);
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    int i = GetMtxIdx(HI.GetKey(), NodeIdMtxIdH);
    for (int j = 0; j < NumFeatures; ++j) {
      FeaturesMtx(i, j) = HI.GetDat()[j];
    }
  }
  return FeaturesMtx;
}

void PrintMatrix(const TFltVV& Matrix) {
  int XDim = Matrix.GetXDim();
  int YDim = Matrix.GetYDim();
  printf("[");
  for (int i = 0; i < XDim; ++i) {
    printf("[");
    for (int j = 0; j < YDim; ++j) {
      if (j != 0) {
        printf(" ");
      }
      printf("%f", Matrix(i, j)());
    }
    printf("]\n");
  }
  printf("]\n");
}

TFltVV CreateRandMatrix(const int XDim, const int YDim) {
  //srand(time(NULL));
  //TFltVV Matrix(XDim, YDim);
  //for (int i = 0; i < XDim; ++i) {
  //  for (int j = 0; j < YDim; ++j) {
  //    Matrix(i, j) = (double) rand() / RAND_MAX;
  //  }
  //}
  //return Matrix;
  int seed = 13;
  TFltVV Matrix(XDim, YDim);
  for (int i = 0; i < XDim; ++i) {
    for (int j = 0; j < YDim; ++j) {
      Matrix(i, j) = (double)seed / 10007;
      seed = (seed * 1871) % 10007;
    }
  }
  return Matrix;
}

bool FltIsZero(const TFlt f) {
  return TFlt::Abs(f) < TFlt::Eps;
}

void CalcNonNegativeFactorization(const TFltVV& V, const int NumRoles,
    TFltVV& W, TFltVV& H) {
  double threshhold = 1e-6;
  double Cost = 100, NewCost = 0;
  int NumNodes = V.GetXDim();
  int NumFeatures = V.GetYDim();
  W = CreateRandMatrix(NumNodes, NumRoles);
  H = CreateRandMatrix(NumRoles, NumFeatures);
  //FPrintMatrix(W, "w.txt");
  //FPrintMatrix(H, "h.txt");
  TFltVV NewW(NumNodes, NumRoles);
  TFltVV NewH(NumRoles, NumFeatures);
  TFltVV Product(NumNodes, NumFeatures);
  TFltV Sum(NumRoles);
  TFltVV *w = &W, *h = &H, *newW = &NewW, *newH = &NewH, *tmp;
  int IterNum = 1;
  while (TFlt::Abs((NewCost - Cost)/Cost) > threshhold) {
    TLinAlg::Multiply(*w, *h, Product);
    //converge condition
    Cost = NewCost;
    NewCost = 0;
    for (int i = 0; i < NumNodes; i++) {
      for (int j = 0; j < NumFeatures; j++) {
        NewCost += V(i, j) * TMath::Log(Product(i, j)) - Product(i, j);
      }
    }
    // update W
    for (int i = 0; i < NumNodes; i++) {
      for (int a = 0; a < NumRoles; a++) {
        double SumU = 0;
        for (int u = 0; u < NumFeatures; ++u) {
          if (!FltIsZero(Product(i, u))) {
            SumU += V(i, u) / Product(i, u) * h->At(a, u);
          }
        }
        newW->At(i, a) = w->At(i, a) * SumU;
      }
    }
    //FPrintMatrix(NewW, "NewW.txt");
    for (int i = 0; i < NumRoles; i++) {
      Sum[i] = 0;
    }
    for (int i = 0; i < NumNodes; i++) {
      for (int j = 0; j < NumRoles; j++) {
        Sum[j] += newW->At(i, j);
      }
    }
    for (int i = 0; i < NumNodes; i++) {
      for (int j = 0; j < NumRoles; j++) {
        newW->At(i, j) /= Sum[j];
      }
    }
    //FPrintMatrix(NewW, "NewW.txt");
    // update H
    double diff = 0;
    for (int a = 0; a < NumRoles; a++) {
      for (int u = 0; u < NumFeatures; u++) {
        double SumI = 0;
        for (int i = 0; i < NumNodes; ++i) {
          if (!FltIsZero(Product(i, u))) {
            SumI += w->At(i, a) * V(i, u) / Product(i, u);
          }
        }
        newH->At(a, u) = h->At(a, u) * SumI;
        //diff += TFlt::Abs(SumI - 1);
      }
    }
    printf("iteration %d, cost is %f\n", IterNum++, NewCost);
    //if (diff / (NumRoles * NumFeatures) < threshhold) break;
    //FPrintMatrix(NewH, "NewH.txt");
    tmp = w; w = newW; newW = tmp;
    tmp = h; h = newH; newH = tmp;
  }
}

TFlt ComputeDescriptionLength(const TFltVV& V, const TFltVV& G,
    const TFltVV& F) {
  int b = 64;
  int m = b * V.GetYDim() * (V.GetXDim() + F.GetYDim());
  TFlt e = 0;
  TFltVV GF(G.GetXDim(), F.GetYDim());
  TLinAlg::Multiply(G, F, GF);
  for (int i = 0; i < V.GetXDim(); ++i) {
    for (int j = 0; j < V.GetYDim(); ++j) {
      TFlt ValueV = V(i, j);
      TFlt ValueGF = GF(i, j);
      if (FltIsZero(ValueV)) {
        e += ValueGF;
      } else if (!FltIsZero(ValueGF)) {
        e += ValueV * TMath::Log(ValueV / ValueGF) - ValueV + ValueGF;
      }
    }
  }
  return m + e;
}

TIntIntH CreateNodeIdMtxIdxHash(const TIntFtrH& Features) {
  TIntIntH h;
  TInt idx = 0;
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    h.AddDat(HI.GetKey(), idx);
    idx++;
  }
  return h;
}

int GetMtxIdx(const TInt NodeId, const TIntIntH& NodeIdMtxIdH) {
  return NodeIdMtxIdH.GetDat(NodeId)();
}

int GetNodeId(const TInt MtxId, const TIntIntH& NodeIdMtxIdH) {
  for (TIntIntH::TIter HI = NodeIdMtxIdH.BegI();
      HI < NodeIdMtxIdH.EndI();
      HI++) {
    if (HI.GetDat() == MtxId) {
      return HI.GetKey()();
    }
  }
  return -1;
}

TIntIntH FindRoles(const TFltVV& G, const TIntIntH& NodeIdMtxIdH) {
  TIntIntH Roles;
  for (int i = 0; i < G.GetXDim(); i++) {
    int Role = -1;
    TFlt Max = TFlt::Mn;
    for (int j = 0; j < G.GetYDim(); j ++) {
      if (G(i, j) > Max) {
        Max = G(i, j);
        Role = j;
      }
    }
    int NodeId = GetNodeId(i, NodeIdMtxIdH);
    Roles.AddDat(NodeId, Role);
  }
  return Roles;
}

void PlotRoles(const PUNGraph Graph, const TIntIntH& Roles) {
  std::string RoleToColor[10] = { "white", "black", "red", "green", "blue",
      "yellow", "gold", "cyan", "magenta", "brown" };
  TIntStrH Color;
  for (TIntIntH::TIter HI = Roles.BegI(); HI < Roles.EndI(); HI++) {
    Color.AddDat(HI.GetKey(), RoleToColor[HI.GetDat()].c_str());
  }
  TSnap::DrawGViz(Graph, gvlDot, "gviz_plot.png", "Dot", 1, Color);
  //TGraphViz::Plot<PNGraph>(Graph, gvlDot, "gviz_plot.png", "", Color);
}

void PrintRoles(const TIntIntH& Roles) {
  printf("--roles (node ID: role ID)--\n");
  printf("{\n");
  for (TIntIntH::TIter HI = Roles.BegI(); HI < Roles.EndI(); HI++) {
    printf("(%d: %d)\n", HI.GetKey()(), HI.GetDat()());
  }
  printf("}\n");
}

void FPrintMatrix(const TFltVV& Matrix, const std::string& path) {
  FILE *fp;
  fp = fopen(path.data(), "w");
  int XDim = Matrix.GetXDim();
  int YDim = Matrix.GetYDim();
  for (int i = 0; i < XDim; ++i) {
    for (int j = 0; j < YDim; ++j) {
      if (j != 0) {
        fprintf(fp, " ");
      }
      fprintf(fp, "%f", Matrix(i, j)());
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void FPrintRoles(const TIntIntH& Roles, const std::string& path) {
  FILE *fp;
  fp = fopen(path.data(), "w");
  fprintf(fp, "--roles (node ID role ID)--\n\n");
  for (TIntIntH::TIter HI = Roles.BegI(); HI < Roles.EndI(); HI++) {
    fprintf(fp, "%d\t%d\n", HI.GetKey()(), HI.GetDat()());
  }
  fclose(fp);
}
