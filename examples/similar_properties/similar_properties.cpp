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

struct TPropertyFunctor {
  TIntSet SelectedProperties;

  TPropertyFunctor (const TGraph &G, int MinNumOccurrences)
  {
    TIntH Map;
    for (TGraph::TEdgeI It = G.BegEI(); It < G.EndEI(); It++) {
      int Property = It.GetDat();
      int KeyId = Map.GetKeyId(Property);
      if (KeyId == -1) {
        Map.AddDat(Property, 1);
      } else {
        Map[KeyId]++;
      }
    }

    for (TIntH::TIter It = Map.BegI(); It < Map.EndI(); It++) {
      int Property = It.GetKey();
      int Occurrences = It.GetDat();
      if (Occurrences >= MinNumOccurrences) {
        SelectedProperties.AddKey(Property);
      }
    }
  }

  bool operator() (int PropertyId) const
  {
    return SelectedProperties.IsKey(PropertyId);
  }
};

void GetAverageMatrixColWise (TSparseColMatrix &M, const TVec<TIntV> &Weights)
{
  for (int i = 0; i < M.ColN; i++) {
    int N = Weights[i].Len();
    if (N  > 0) {
      TLinAlg::MultiplyScalar(1 / (double)N, M.ColSpVV[i], M.ColSpVV[i]);
    }
  }
}

void ComputeSimilarProperties (const TStr &Dir, const TStr &TriplesFilename)
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

  printf("Computing properties...\n");
  TIntV Properties;
  int MinNumOccurences = 1000;
  TPropertyFunctor PropertyFunctor(G, MinNumOccurences);
  TPropertyUtils::GetProperties(G, PropertyFunctor, Properties);
  Properties.Save(*TFOut::New(Dir + "properties.bin"));
  TPropertyUtils::PrintProperties(Properties, PropStrs, *TFOut::New(Dir + "properties.txt"));

  printf("Computing source and destination objects for every property...\n");
  TVec<TIntV> SrcObjects;
  TVec<TIntV> DstObjects;
  // Get source objects for every property.
  TPropertyUtils::GetSrcObjects(Properties, Objects, G, SrcObjects);
  // Get destination objects for every property.
  TPropertyUtils::GetDstObjects(Properties, Objects, G, DstObjects);
  // Store the data.
  SrcObjects.Save(*TFOut::New(Dir + "SrcObjects.bin"));
  DstObjects.Save(*TFOut::New(Dir + "DstObjects.bin"));
  // Print the objects
  TPropertyUtils::PrintPropertyObjects(SrcObjects, Objects, Properties, NodeStrs, PropStrs, *TFOut::New(Dir + "SrcObjects.txt"));
  TPropertyUtils::PrintPropertyObjects(DstObjects, Objects, Properties, NodeStrs, PropStrs, *TFOut::New(Dir + "DstObjects.txt"));

  printf("Computing property descriptors...\n");
  // The descriptor for property consits of two parts:
  // - average of source objects descriptors
  // - average of destination objects descriptors
  
  // We first compute source and destination descriptors
  int MaxNumObjects = 1000;
  TSparseColMatrix PropertySrcMatrix;
  TSparseColMatrix PropertyDstMatrix;
  TLAUtils::GetSumVectors(ObjectMatrix, SrcObjects, MaxNumObjects, PropertySrcMatrix); 
  TLAUtils::GetSumVectors(ObjectMatrix, DstObjects, MaxNumObjects, PropertyDstMatrix); 
  GetAverageMatrixColWise(PropertySrcMatrix, SrcObjects);
  GetAverageMatrixColWise(PropertyDstMatrix, DstObjects);

  // Concatenate the source and destination matrices into one matrix.
  TSparseColMatrix PropertyMatrix;
  TLAUtils::ConcatenateMatricesRowWise(PropertySrcMatrix, PropertyDstMatrix, PropertyMatrix);
  TLAUtils::NormalizeMatrix(PropertyMatrix);
  PropertyMatrix.Save(*TFOut::New(Dir + "propertySrcMatrix.bin"));

  printf("Computing property similarities...\n");
  int NumThreads = 10;
  int MaxNumSimilarProperties = 100;
  TVec<TIntFltKdV> Similarities;
  TSimilarityUtils::ComputeSimilarities(PropertyMatrix, MaxNumSimilarProperties, NumThreads, Similarities);
  Similarities.Save(*TFOut::New(Dir + "property_similarities.bin"));
  TSimilarityUtils::PrintSimilarities(Similarities, Properties, PropStrs, 10, *TFOut::New(Dir + "property_similarities.txt"));
}

int main (int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <Path to an existing directory for storing the binary data> <Path to a RDF file>\n", (argc > 0 ? argv[0] : ""));
    return 1;
  }

  ComputeSimilarProperties(argv[1], argv[2]);

  printf("Finished.\n");
  return 0;
}