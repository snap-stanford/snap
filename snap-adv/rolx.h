#ifndef snap_rolx_h
#define snap_rolx_h
#include "Snap.h"

/// Feature of a node.
typedef TVec<TFlt> TFtr;
/// The node-feature mapping: <node ID, feature>.
typedef THash<TInt, TFtr> TIntFtrH;

/// Prints all nodes' feature.
void PrintFeatures(const TIntFtrH& Features);
/// Creates an empty node-feature mapping of all nodes in the given graph.
TIntFtrH CreateEmptyFeatures(const PUNGraph Graph);
/// Creates an empty node-feature mapping of all nodes from an existing mapping.
TIntFtrH CreateEmptyFeatures(const TIntFtrH& Features);
/// Gets number of features from the node-feature mapping.
int GetNumFeatures(const TIntFtrH& Features);
/// Gets the n-th feature of all nodes.
TFtr GetNthFeature(const TIntFtrH& Features, const int N);
/// Performs feature extraction, the first step of RolX.
TIntFtrH ExtractFeatures(const PUNGraph Graph);
/// Adds neighborhood features (local + egonet) to the node-feature mapping.
void AddNeighborhoodFeatures(const PUNGraph Graph, TIntFtrH& Features);
/// Adds recursive features to the node-feature mapping.
void AddRecursiveFeatures(const PUNGraph Graph, TIntFtrH& Features);
/// Adds local features to the node-feature mapping.
void AddLocalFeatures(const PUNGraph Graph, TIntFtrH& Features);
/// Adds egonet features to the node-feature mapping.
void AddEgonetFeatures(const PUNGraph Graph, TIntFtrH& Features);
/// Generates recursive features out of current features.
TIntFtrH GenerateRecursiveFeatures(const PUNGraph Graph,
    const TIntFtrH& CurrFeatures);
/// Prunes recursive features.
TIntFtrH PruneRecursiveFeatures(const PUNGraph Graph, const TIntFtrH& Features,
    const TIntFtrH& NewFeatures, const int SimilarityThreshold);
/// Appends all src features to dst features.
void AppendFeatures(TIntFtrH& DstFeatures, const TIntFtrH& SrcFeatures,
    const int ColIdx = -1);
/// Calculates vertical logarithmic binning features from the given features.
TIntFtrH CalcVerticalLogBinning(const TIntFtrH& Features,
    const float BinFraction);
/// Builds s-friend graph given similarity threshold.
PUNGraph BuildFeatureGraph(const TIntFtrH& LogFeatures,
    const int SimilarityThreshold);
/// Summarizes s-friend graph and return retained features.
TIntFtrH SummarizeConnectedComponents(const PUNGraph FeatureGraph,
    const TIntFtrH& Features, const TIntFtrH& NewFeatures);
/// Sorts the Idx-th feature, return the list of corresponding node ID.
TVec<TInt> GetNIdSorted(const TIntFtrH& Feature, const int Idx);
/// Assigns logarithmic binning value to features.
void AssignBinValue(const TVec<TInt>& SortedNId, const float BinFraction,
    TIntFtrH& LogBinFeatures);
/// Whether the two features are similar, given similarity threshold.
bool IsSimilarFeature(const TFtr& F1, const TFtr& F2,
    const int SimilarityThreshold);
/// Converts node-feature mapping to matrix. (i, j): i-th node, j-th feature.
TFltVV ConvertFeatureToMatrix(const TIntFtrH& Features,
    const TIntIntH& NodeIdMtxIdxH);
/// Prints feature matrix to stdout.
void PrintMatrix(const TFltVV& Matrix);
/// Creates a random matrix with specified dimension.
TFltVV CreateRandMatrix(const int XDim, const int YDim);
/// Whether the float is zero.
bool FltIsZero(const TFlt Number);
/// Performs non-negative factorization V = WH. 2nd dim of W == number of roles.
void CalcNonNegativeFactorization(const TFltVV& V, const int NumRoles,
    TFltVV& W, TFltVV& H, const double Threshold);
/// Calculates the description length L = M + E.
TFlt CalcDescriptionLength(const TFltVV& V, const TFltVV& G,
    const TFltVV& F);
/// Creates the mapping of <node ID, matrix index>.
TIntIntH CreateNodeIdMtxIdxHash(const TIntFtrH& Features);
/// Gets matrix index of the node ID.
int GetMtxIdx(const TInt NodeId, const TIntIntH& NodeIdMtxIdxH);
/// Gets matrix index of the node ID.
TIntIntH FindRoles(const TFltVV& G, const TIntIntH& NodeIdMtxIdxH);
/// Plots found roles on a picture (.png).
void PlotRoles(const PUNGraph Graph, const TIntIntH& Roles);
/// Prints found roles on stdout.
void PrintRoles(const TIntIntH& Roles);
/// Prints feature matrix to file.
void FPrintMatrix(const TFltVV& Matrix, const TStr& Path);
/// Prints found roles to file.
void FPrintRoles(const TIntIntH& Roles, const TStr& Path);

#endif
