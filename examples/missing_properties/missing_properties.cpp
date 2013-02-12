#include "stdafx.h"
#include "lod/typedefs.h"
#include "lod/rdf_parser.h"
#include "lod/rdf_graph.h"
#include "lod/la_utils.h"
#include "lod/object_utils.h"
#include "lod/cluster_utils.h"
#include "lod/similarity_utils.h"
#include "lod/property_utils.h"

struct TObjectFunctor {
  const TStrSet &NodeStrs;

  TObjectFunctor (const TStrSet &NodeStrsArg)
  : NodeStrs(NodeStrsArg) {}

  bool operator() (int NodeId) const
  {
    const TStr &NodeStr = NodeStrs[NodeId];
    return NodeStr.IsPrefix("http://dbpedia.org/resource/");
  }
};

void ComputeMissingProperties (const TStr &Dir, const TStr &TriplesFilename)
{
  // Parse the rdf file and create the graph.
  TFIn File(TriplesFilename);
  TRDFParser DBpediaDataset(File);

  printf("Creating graph from input file...\n");
  TGraph G;
  TStrSet NodeStrs;
  TStrSet PropStrs;
  bool Parsed = TSnap::GetGraphFromRDFParser(DBpediaDataset, G, NodeStrs, PropStrs);
  if (!Parsed) {
    return;
  }

  // Store the graph and associated data
  G.Save(*TFOut::New(Dir + "graph.bin"));
  NodeStrs.Save(*TFOut::New(Dir + "nodeStrs.bin"));
  PropStrs.Save(*TFOut::New(Dir + "propStrs.bin"));

  printf("Computing objects...\n");
  // Get the objects of the graph. 
  TIntV Objects;
  // We defined the objects to be the nodes with prefix http://dbpedia.org/resource/. 
  TObjectFunctor ObjectFunctor(NodeStrs);
  TObjectUtils::GetObjects(G, ObjectFunctor, Objects);
  // Store and print the objects.
  Objects.Save(*TFOut::New(Dir + "objects.bin"));
  TObjectUtils::PrintObjects(Objects, NodeStrs, *TFOut::New(Dir + "objects.txt"));

  printf("Computing object matrix...\n");
  // Here we choose the descriptors for the objects.
  // We chose property + nbh (value) descriptors for objects
  // We could also use more complicated descriptors such as subgraphs or subnetworks.
  TSparseColMatrix ObjectMatrix1;
  TSparseColMatrix ObjectMatrix2;
  TObjectUtils::GetPropertyCount(Objects, G, ObjectMatrix1);
  TObjectUtils::GetNbhCount(Objects, G, ObjectMatrix2);
  TLAUtils::NormalizeMatrix(ObjectMatrix1);
  TLAUtils::NormalizeMatrix(ObjectMatrix2);

  TSparseColMatrix ObjectMatrix;
  TLAUtils::ConcatenateMatricesRowWise(ObjectMatrix1, ObjectMatrix2, ObjectMatrix);
  TLAUtils::NormalizeMatrix(ObjectMatrix);
  ObjectMatrix.Save(*TFOut::New(Dir + "objectMatrix.bin"));

  printf("Clustering objects...\n");
  // Partition the objects into 64 partitions (clusters).
  int K = 64;
  int NumIterations = 20;
  TIntV Assigments;
  TVec<TIntV> Clusters;
  TClusterUtils::GetClusters(ObjectMatrix, K, NumIterations, Assigments, Clusters);
  // Store the clustering data.
  Assigments.Save(*TFOut::New(Dir + "assigments.bin"));
  Clusters.Save(*TFOut::New(Dir + "clusters.bin"));
  // Print some details about the clusters.
  TClusterUtils::PrintClusterSizes(Clusters, *TFOut::New(Dir + "clusterSizes.txt"));
  TClusterUtils::PrintClusters(Clusters, Objects, NodeStrs, *TFOut::New(Dir + "clusters.txt"));

  printf("Computing similarities...\n");
  // Compute the similarity betweeen the objects.
  const int MaxNumSimilarObjects = 100;
  const int NumThreads = 10;
  TVec<TIntFltKdV> Similarities;
  TSimilarityUtils::ComputeSimilarities(ObjectMatrix, Assigments, Clusters, MaxNumSimilarObjects, NumThreads, Similarities);
  // Store the object similarities.
  Similarities.Save(*TFOut::New(Dir + "objectSimilarities.bin"));
  // Print the object similarities.
  TSimilarityUtils::PrintSimilarities(Similarities, Objects, NodeStrs, 10, *TFOut::New(Dir + "objectSimilarities.txt"));

  printf("Computing existing property matrix...\n");
  // Our goal is to compute the missing out-going properties.
  // Therefore, we create the matrix of existing out-going properties of the objects.
  TSparseColMatrix OutPropertyCountMatrix;
  TObjectUtils::GetOutPropertyCount(Objects, G, OutPropertyCountMatrix);
  TObjectUtils::PrintPropertyMatrix(OutPropertyCountMatrix, Objects, NodeStrs, PropStrs, *TFOut::New(Dir + "outPropertyCountMatrix.txt"));
  OutPropertyCountMatrix.Save(*TFOut::New(Dir + "outPropertyCountMatrix.bin"));

  printf("Computing missing properties...\n");
  // And finally, compute the missing properties.
  int MaxNumMissingProperties = 100;
  TVec<TIntFltKdV> MissingProperties;
  TPropertyUtils::GetMissingProperties(Similarities, OutPropertyCountMatrix, MaxNumMissingProperties, NumThreads, MissingProperties);
  // Store the missing properties data.
  MissingProperties.Save(*TFOut::New(Dir + "missingProperties.bin"));
  // Print missing properties.
  TPropertyUtils::PrintMissingProperties(MissingProperties, Objects, NodeStrs, PropStrs, 10, *TFOut::New(Dir + "missingProperties.txt"));
}

int main (int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <Path to an existing directory for storing the binary data> <Path to a RDF file>\n", (argc > 0 ? argv[0] : ""));
    return 1;
  }

  ComputeMissingProperties(argv[1], argv[2]);

  printf("Finished.\n");
  return 0;
}