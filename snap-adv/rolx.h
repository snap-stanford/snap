#ifndef snap_rolx_h
#define snap_rolx_h
#include "Snap.h"

typedef TVec<TFlt> TFtr;
typedef THash<TInt, TFtr> TIntFtrH;

void PrintFeatures(const TIntFtrH& Features);
TIntFtrH CreateEmptyFeatures(const PUNGraph Graph);
TIntFtrH CreateEmptyFeatures(const TIntFtrH& Features);
int GetNumFeatures(const TIntFtrH& Features);
TFtr GetNthFeature(const TIntFtrH& Features, const int n);
TIntFtrH ExtractFeatures(const PUNGraph Graph);
void AddNeighborhoodFeatures(const PUNGraph Graph, TIntFtrH& Features);
void AddRecursiveFeatures(const PUNGraph Graph, TIntFtrH& Features);
void AddLocalFeatures(const PUNGraph Graph, TIntFtrH& Features);
void AddEgonetFeatures(const PUNGraph Graph, TIntFtrH& Features);
TIntFtrH GenerateRecursiveFeatures(const PUNGraph Graph,
    const TIntFtrH& CurrFeatures);
TIntFtrH PruneRecursiveFeatures(const PUNGraph Graph, const TIntFtrH& Features,
    const TIntFtrH& NewFeatures, const int SimilarityThreshold);
void AppendFeatures(TIntFtrH& DstFeatures, const TIntFtrH& SrcFeatures,
    const int ColIdx = -1);
TIntFtrH CalcVerticalLogBinning(const TIntFtrH& Features,
    const float BinFraction);
PUNGraph BuildFeatureGraph(const TIntFtrH& LogFeatures,
    const int SimilarityThreshold);
TIntFtrH SummarizeConnectedComponents(const PUNGraph FeatureGraph,
    const TIntFtrH& Features, const TIntFtrH& NewFeatures);
TVec<TInt> GetNIdSorted(const TIntFtrH& Feature, const int Idx);
void AssignBinValue(const TVec<TInt>& SortedNId, const float BinFraction,
    TIntFtrH& LogBinFeatures);
bool IsSimilarFeature(const TFtr& F1, const TFtr& F2,
    const int SimilarityThreshold);
TFltVV ConvertFeatureToMatrix(const TIntFtrH& Features,
    const TIntIntH& NodeIdMtxIdH);
void PrintMatrix(const TFltVV& Matrix);
TFltVV CreateRandMatrix(const int XDim, const int YDim);
bool FltIsZero(const TFlt f);
void CalcNonNegativeFactorization(const TFltVV& V, const int NumRoles,
    TFltVV& W, TFltVV& H);
TFlt ComputeDescriptionLength(const TFltVV& V, const TFltVV& G,
    const TFltVV& F);
TIntIntH CreateNodeIdMtxIdxHash(const TIntFtrH& Features);
int GetMtxIdx(const TInt NodeId, const TIntIntH& NodeIdMtxIdH);
TIntIntH FindRoles(const TFltVV& G, const TIntIntH& NodeIdMtxIdH);
void PlotRoles(const PUNGraph Graph, const TIntIntH& Roles);
void PrintRoles(const TIntIntH& Roles);
void FPrintMatrix(const TFltVV& Matrix, const std::string& path);
void FPrintRoles(const TIntIntH& Roles, const std::string& path);

#endif
