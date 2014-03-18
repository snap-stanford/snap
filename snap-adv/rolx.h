#ifndef snap_rolx_h
#define snap_rolx_h
#include "Snap.h"

// Feature of a node.
typedef TVec<TFlt> TFtr;
// The node-feature mapping: <node ID, feature>.
typedef THash<TInt, TFtr> TIntFtrH;

// Print all nodes' feature.
void PrintFeatures(const TIntFtrH& Features);
// Create an empty node-feature mapping of all nodes in the given graph.
TIntFtrH CreateEmptyFeatures(const PUNGraph Graph);
// Create an empty node-feature mapping of all nodes from an existing mapping.
TIntFtrH CreateEmptyFeatures(const TIntFtrH& Features);
// Get number of features from the node-feature mapping.
int GetNumFeatures(const TIntFtrH& Features);
// Get the n-th feature of all nodes.
TFtr GetNthFeature(const TIntFtrH& Features, const int N);
// Perform feature extraction, the first step of RolX.
TIntFtrH ExtractFeatures(const PUNGraph Graph);
// Add neighborhood features (local + egonet) to the node-feature mapping.
void AddNeighborhoodFeatures(const PUNGraph Graph, TIntFtrH& Features);
// Add recursive features to the node-feature mapping.
void AddRecursiveFeatures(const PUNGraph Graph, TIntFtrH& Features);
// Add local features to the node-feature mapping.
void AddLocalFeatures(const PUNGraph Graph, TIntFtrH& Features);
// Add egonet features to the node-feature mapping.
void AddEgonetFeatures(const PUNGraph Graph, TIntFtrH& Features);
// Generate recursive features out of current features.
TIntFtrH GenerateRecursiveFeatures(const PUNGraph Graph,
    const TIntFtrH& CurrFeatures);
// Prune recursive features.
TIntFtrH PruneRecursiveFeatures(const PUNGraph Graph, const TIntFtrH& Features,
    const TIntFtrH& NewFeatures, const int SimilarityThreshold);
// Append all src features to dst features.
void AppendFeatures(TIntFtrH& DstFeatures, const TIntFtrH& SrcFeatures,
    const int ColIdx = -1);
// Calculate vertical logarithmic binning features from the given features.
TIntFtrH CalcVerticalLogBinning(const TIntFtrH& Features,
    const float BinFraction);
// Build s-friend graph given similarity threshold.
PUNGraph BuildFeatureGraph(const TIntFtrH& LogFeatures,
    const int SimilarityThreshold);
// Summarize s-friend graph and return retained features.
TIntFtrH SummarizeConnectedComponents(const PUNGraph FeatureGraph,
    const TIntFtrH& Features, const TIntFtrH& NewFeatures);
// Sort the Idx-th feature, return the list of corresponding node ID.
TVec<TInt> GetNIdSorted(const TIntFtrH& Feature, const int Idx);
// Assign logarithmic binning value to features.
void AssignBinValue(const TVec<TInt>& SortedNId, const float BinFraction,
    TIntFtrH& LogBinFeatures);
// Whether the two features are similar, given similarity threshold.
bool IsSimilarFeature(const TFtr& F1, const TFtr& F2,
    const int SimilarityThreshold);
// Convert node-feature mapping to matrix form. (i, j): i-th node, j-th feature.
TFltVV ConvertFeatureToMatrix(const TIntFtrH& Features,
    const TIntIntH& NodeIdMtxIdxH);
// Print feature matrix to stdout.
void PrintMatrix(const TFltVV& Matrix);
// Create a random matrix with specified dimension.
TFltVV CreateRandMatrix(const int XDim, const int YDim);
// Whether the float is zero.
bool FltIsZero(const TFlt Number);
// Perform non-negative factorization: V = WH. 2nd dim of W == number of roles.
void CalcNonNegativeFactorization(const TFltVV& V, const int NumRoles,
    TFltVV& W, TFltVV& H);
// Calculate the description length L = M + E.
TFlt CalcDescriptionLength(const TFltVV& V, const TFltVV& G,
    const TFltVV& F);
// Create the mapping of <node ID, matrix index>.
TIntIntH CreateNodeIdMtxIdxHash(const TIntFtrH& Features);
// Get matrix index of the node ID.
int GetMtxIdx(const TInt NodeId, const TIntIntH& NodeIdMtxIdxH);
// Get matrix index of the node ID.
TIntIntH FindRoles(const TFltVV& G, const TIntIntH& NodeIdMtxIdxH);
// Plot found roles on a picture (.png).
void PlotRoles(const PUNGraph Graph, const TIntIntH& Roles);
// Print found roles on stdout.
void PrintRoles(const TIntIntH& Roles);
// Print feature matrix to file.
void FPrintMatrix(const TFltVV& Matrix, const TStr& Path);
// Print found roles to file.
void FPrintRoles(const TIntIntH& Roles, const TStr& Path);

#endif
