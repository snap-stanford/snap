#include "stdafx.h"
#include "lod/typedefs.h"
#include "lod/rdf_parser.h"
#include "lod/lod_utils.h"
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
  bool Parsed = TLODUtils::GetGraphFromRDFParser(DBpediaDataset, G, NodeStrs, PropStrs);
  if (!Parsed) {
    return;
  }

  // Store the graph and associated data to disk
  G.Save(TFOut(Dir + "graph.bin"));
  NodeStrs.Save(TFOut(Dir + "nodeStrs.bin"));
  PropStrs.Save(TFOut(Dir + "propStrs.bin"));
  //TGraph G(TFIn(Dir + "graph.bin"));
  //NodeStrs.Load(TFIn(Dir + "nodeStrs.bin"));
  //PropStrs.Load(TFIn(Dir + "propStrs.bin"));

  printf("Computing objects...\n");
  // Get the objects of the graph. 
  TIntV Objects;
  // We defined the objects to be the nodes with prefix http://dbpedia.org/resource/. 
  TObjectFunctor ObjectFunctor(NodeStrs);
  TObjectUtils::GetObjects(G, ObjectFunctor, Objects);
  // Print and store the objects.
  TObjectUtils::PrintObjects(Objects, NodeStrs, TFOut(Dir + "objects.txt"));
  Objects.Save(TFOut(Dir + "objects.bin"));
  //Objects.Load(TFIn(Dir + "/objects.bin"));

  printf("Computing object matrix...\n");
  // Here we choose the descriptors for the objects.
  // We choose property + nbh (value) descriptors for objects
  TSparseColMatrix ObjectMatrix;
  //TSparseColMatrix ObjectMatrix1;
  //TSparseColMatrix ObjectMatrix2;
  TObjectUtils::GetPropertyCount(Objects, G, ObjectMatrix);
  //TObjectUtils::GetNbhCount(Objects, G, ObjectMatrix2);
  //TLODUtils::ConcatenateMatricesRowWise(ObjectMatrix1, ObjectMatrix2, ObjectMatrix);
	
  TObjectUtils::PrintPropertyMatrix(ObjectMatrix, Objects, NodeStrs, PropStrs, TFOut(Dir + "propertyMatrix.txt"));
  // Normalize the object descriptors
  TLODUtils::NormalizeMatrix(ObjectMatrix);
  // Store the object matrix
  ObjectMatrix.Save(TFOut(Dir + "objectMatrix.bin"));
  //ObjectMatrix.Load(TFIn(Dir + "objectMatrix.bin"));

  printf("Clustering objects...\n");
  // Partition the objects into 64 partitions (clusters)
  int K = 64;
  int NumIterations = 20;
  TIntV Assigments;
  TVec<TIntV> Clusters;
  TClusterUtils::GetClusters(ObjectMatrix, K, NumIterations, Assigments, Clusters);
  // Store clustering data
  Assigments.Save(TFOut(Dir + "assigments.bin"));
  Clusters.Save(TFOut(Dir + "clusters.bin"));
  //Assigments.Load(TFIn(Dir + "assigments.bin"));
  //Clusters.Load(TFIn(Dir + "clusters.bin"));
  // Print some details about the clusters
  TClusterUtils::PrintClusterSizes(Clusters, TFOut(Dir + "clusterSizes.txt"));
  TClusterUtils::PrintClusters(Clusters, Objects, NodeStrs, TFOut(Dir + "clusters.txt"));

  printf("Computing similarities...\n");
  // Compute the similarity betweeen the objects
  const int MaxNumSimilarObjects = 100;
  const int NumThreads = 10;
  TVec<TIntFltKdV> Similarities;
  TSimilarityUtils::ComputeSimilarities(ObjectMatrix, Assigments, Clusters, MaxNumSimilarObjects, NumThreads, Similarities);
  // Store the object similarities
  Similarities.Save(TFOut(Dir + "objectSimilarities.bin"));
  //Similarities.Load(TFIn(Dir + "objectSimilarities.bin"));
  // Print the object similarities
  TSimilarityUtils::PrintSimilarities(Similarities, Objects, NodeStrs, 10, TFOut(Dir + "objectSimilarities.txt"));

  printf("Computing existing property matrix...\n");
  // Create the matrix of existing out-going properties of the objects
  TSparseColMatrix OutPropertyCountMatrix;
  TObjectUtils::GetOutPropertyCount(Objects, G, OutPropertyCountMatrix);
  TObjectUtils::PrintPropertyMatrix(OutPropertyCountMatrix, Objects, NodeStrs, PropStrs, TFOut(Dir + "outPropertyCountMatrix.txt"));
  OutPropertyCountMatrix.Save(TFOut(Dir + "outPropertyCountMatrix.bin"));
  //OutPropertyCountMatrix.Load(TFIn(Dir + "outPropertyCountMatrix.bin"));

  printf("Computing missing properties...\n");
  // And finally, compute the missing properties
  int MaxNumMissingProperties = 100;
  TVec<TIntFltKdV> MissingProperties;
  TPropertyUtils::GetMissingProperties(Similarities, OutPropertyCountMatrix, MaxNumMissingProperties, NumThreads, MissingProperties);
  // Store missing properties data
  MissingProperties.Save(TFOut(Dir + "missingProperties.bin"));
  // Print missing properties.
  TPropertyUtils::PrintMissingProperties(MissingProperties, Objects, NodeStrs, PropStrs, 10, TFOut(Dir + "missingProperties.txt"));
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