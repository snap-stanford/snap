#include "stdafx.h"
#include "Snap.h"
#include "rolx.h"

void PrintFeatures(const TIntFtrH& Features) {
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    printf("%d: [", HI.GetKey()());
    const TFtr& Feature = HI.GetDat();
    for (int i = 0; i < Feature.Len(); ++i) {
      if (i > 0) {
        printf(",");
      }
      printf("%f", Feature[i]());
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

TFtr GetNthFeature(const TIntFtrH& Features, const int N) {
  TFtr NthFeature;
  IAssert(0 <= N && N < GetNumFeatures(Features));
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    NthFeature.Add(HI.GetDat()[N]);
  }
  return NthFeature;
}

TIntFtrH ExtractFeatures(const PUNGraph Graph) {
  TIntFtrH Features = CreateEmptyFeatures(Graph);
  AddNeighborhoodFeatures(Graph, Features);
  printf("finish neighborhood features\n");
  AddRecursiveFeatures(Graph, Features);
  printf("finish recursive features\n");
  return Features;
}

void AddNeighborhoodFeatures(const PUNGraph Graph, TIntFtrH& Features) {
  AddLocalFeatures(Graph, Features);
  printf("finish local features\n");
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
    const TFtr& Feature = HI.GetDat();
    if (ColIdx >= 0) {
      DstFeatures.GetDat(HI.GetKey()).Add(Feature[ColIdx]);
    } else {
      for (int i = 0; i < Feature.Len(); ++i) {
        DstFeatures.GetDat(HI.GetKey()).Add(Feature[i]);
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
    const TIntIntH& NodeIdMtxIdxH) {
  const int NumNodes = Features.Len();
  const int NumFeatures = GetNumFeatures(Features);
  TFltVV FeaturesMtx(NumNodes, NumFeatures);
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    int i = GetMtxIdx(HI.GetKey(), NodeIdMtxIdxH);
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
  int Seed = 13;
  TFltVV Matrix(XDim, YDim);
  for (int i = 0; i < XDim; ++i) {
    for (int j = 0; j < YDim; ++j) {
      Matrix(i, j) = (double)Seed / 10007;
      Seed = (Seed * 1871) % 10007;
    }
  }
  return Matrix;
}

bool FltIsZero(const TFlt Number) {
  return TFlt::Abs(Number) < TFlt::Eps;
}

void CalcNonNegativeFactorization(const TFltVV& V, const int NumRoles,
    TFltVV& W, TFltVV& H, const double Threshold) {
  double Cost = 100, NewCost = 0;
  int NumNodes = V.GetXDim();
  int NumFeatures = V.GetYDim();
  W = CreateRandMatrix(NumNodes, NumRoles);
  H = CreateRandMatrix(NumRoles, NumFeatures);
  TFltVV NewW(NumNodes, NumRoles);
  TFltVV NewH(NumRoles, NumFeatures);
  TFltVV Product(NumNodes, NumFeatures);
  TFltV Sum(NumRoles);
  TFltVV *PW = &W, *PH = &H, *PNewW = &NewW, *PNewH = &NewH, *Tmp;
  //int IterNum = 1;
  while (TFlt::Abs((NewCost - Cost)/Cost) > Threshold) {
    TLinAlg::Multiply(*PW, *PH, Product);
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
            SumU += V(i, u) / Product(i, u) * PH->At(a, u);
          }
        }
        PNewW->At(i, a) = PW->At(i, a) * SumU;
      }
    }
    for (int i = 0; i < NumRoles; i++) {
      Sum[i] = 0;
    }
    for (int i = 0; i < NumNodes; i++) {
      for (int j = 0; j < NumRoles; j++) {
        Sum[j] += PNewW->At(i, j);
      }
    }
    for (int i = 0; i < NumNodes; i++) {
      for (int j = 0; j < NumRoles; j++) {
        PNewW->At(i, j) /= Sum[j];
      }
    }
    // update H
    for (int a = 0; a < NumRoles; a++) {
      for (int u = 0; u < NumFeatures; u++) {
        double SumI = 0;
        for (int i = 0; i < NumNodes; ++i) {
          if (!FltIsZero(Product(i, u))) {
            SumI += PW->At(i, a) * V(i, u) / Product(i, u);
          }
        }
        PNewH->At(a, u) = PH->At(a, u) * SumI;
      }
    }
    //printf("iteration %d, cost is %f\n", IterNum++, NewCost);
    Tmp = PW; PW = PNewW; PNewW = Tmp;
    Tmp = PH; PH = PNewH; PNewH = Tmp;
  }
}

TFlt CalcDescriptionLength(const TFltVV& V, const TFltVV& G,
    const TFltVV& F) {
  int B = 64;
  int M = B * V.GetYDim() * (V.GetXDim() + F.GetYDim());
  TFlt E = 0;
  TFltVV GF(G.GetXDim(), F.GetYDim());
  TLinAlg::Multiply(G, F, GF);
  for (int i = 0; i < V.GetXDim(); ++i) {
    for (int j = 0; j < V.GetYDim(); ++j) {
      TFlt ValueV = V(i, j);
      TFlt ValueGF = GF(i, j);
      if (FltIsZero(ValueV)) {
        E += ValueGF;
      } else if (!FltIsZero(ValueGF)) {
        E += ValueV * TMath::Log(ValueV / ValueGF) - ValueV + ValueGF;
      }
    }
  }
  return M + E;
}

TIntIntH CreateNodeIdMtxIdxHash(const TIntFtrH& Features) {
  TIntIntH H;
  TInt Idx = 0;
  for (TIntFtrH::TIter HI = Features.BegI(); HI < Features.EndI(); HI++) {
    H.AddDat(HI.GetKey(), Idx);
    Idx++;
  }
  return H;
}

int GetMtxIdx(const TInt NodeId, const TIntIntH& NodeIdMtxIdxH) {
  return NodeIdMtxIdxH.GetDat(NodeId)();
}

int GetNodeId(const TInt MtxId, const TIntIntH& NodeIdMtxIdxH) {
  for (TIntIntH::TIter HI = NodeIdMtxIdxH.BegI();
      HI < NodeIdMtxIdxH.EndI();
      HI++) {
    if (HI.GetDat() == MtxId) {
      return HI.GetKey()();
    }
  }
  return -1;
}

TIntIntH FindRoles(const TFltVV& G, const TIntIntH& NodeIdMtxIdxH) {
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
    int NodeId = GetNodeId(i, NodeIdMtxIdxH);
    Roles.AddDat(NodeId, Role);
  }
  return Roles;
}

void PlotRoles(const PUNGraph Graph, const TIntIntH& Roles) {
  TStr RoleToColor[10] = { "white", "black", "red", "green", "blue",
      "yellow", "gold", "cyan", "magenta", "brown" };
  TIntStrH Color;
  for (TIntIntH::TIter HI = Roles.BegI(); HI < Roles.EndI(); HI++) {
    Color.AddDat(HI.GetKey(), RoleToColor[HI.GetDat()].CStr());
  }
  TSnap::DrawGViz(Graph, gvlDot, "gviz_plot.png", "Dot", 1, Color);
}

void PrintRoles(const TIntIntH& Roles) {
  printf("--roles (node ID: role ID)--\n");
  printf("{\n");
  for (TIntIntH::TIter HI = Roles.BegI(); HI < Roles.EndI(); HI++) {
    printf("(%d: %d)\n", HI.GetKey()(), HI.GetDat()());
  }
  printf("}\n");
}

void FPrintMatrix(const TFltVV& Matrix, const TStr& Path) {
  FILE *Fp;
  Fp = fopen(Path.CStr(), "w");
  int XDim = Matrix.GetXDim();
  int YDim = Matrix.GetYDim();
  for (int i = 0; i < XDim; ++i) {
    for (int j = 0; j < YDim; ++j) {
      if (j != 0) {
        fprintf(Fp, " ");
      }
      fprintf(Fp, "%f", Matrix(i, j)());
    }
    fprintf(Fp, "\n");
  }
  fclose(Fp);
}

void FPrintRoles(const TIntIntH& Roles, const TStr& Path) {
  FILE *Fp;
  Fp = fopen(Path.CStr(), "w");
  fprintf(Fp, "--roles (node ID role ID)--\n\n");
  for (TIntIntH::TIter HI = Roles.BegI(); HI < Roles.EndI(); HI++) {
    fprintf(Fp, "%d\t%d\n", HI.GetKey()(), HI.GetDat()());
  }
  fclose(Fp);
}
