#ifndef SNAP_PROPERTY_UTILS_H
#define SNAP_PROPERTY_UTILS_H

#include "stdafx.h"
#include "typedefs.h"
#include "la_utils.h"

/**
 * Author: Klemen Simonic
 *
 * A property is a (special) type of edge in the graph. As such, each property p
 * has a set of source nodes (nodes having an out-going edge of type p) and a set 
 * of destination objects (objects having an in-comming edge of type p).
 * Typically, we are interested only in special source and destination nodes, 
 * called objects (see TObjectUtils).
 *
 * This module provides various functionalities regarding the properties in 
 * the graph.
 */
class TPropertyUtils {
private:
  static void ExcludeExistingProperties (const TFltV &AllProperties, const TIntFltKdV &ExistingProperties, TIntFltKdV &Properties);
	static void ToHashMap (const TIntV &Vector, TIntH &Map);

public:
  /**
   * Returns the properties of the graph, which are determined via specified property 
   * functor. Property functor should return true, when the specified edge data in 
   * the graph represents a property.
   */
	template<class TFunctor>
	static void GetProperties (const TGraph &G, TFunctor &PropertyFunctor, TIntV &Properties);
  /// Prints the properties.
	static void PrintProperties (const TIntV &Properties, const TStrSet &PropertyStrs, TSOut &Output);

  /// Returns the source objects for the specified properties.
	static void GetSrcObjects (const TIntV &Properties, const TIntV &Objects, const TGraph &G, TVec<TIntV> &SrcObjects);
  /// Returns the destination objects for the specified properties.
	static void GetDstObjects (const TIntV &Properties, const TIntV &Objects, const TGraph &G, TVec<TIntV> &DstObjects);

  /// Prints the specified objects.
  static void PrintObjects (const TIntV &PropertyObjects, const TIntV &Objects, const TStrSet &NodeStrs, TSOut &Output);
  /// Prints the properties and their associated objects.
	static void PrintPropertyObjects (const TVec<TIntV> &PropertyObjects, const TIntV &Objects, const TIntV &Properties, const TStrSet &NodeStrs, const TStrSet &PropStrs, TSOut &Output);

  /**
   * Computes the missing properties of a given object based on specified 
   * similarities to other objects.
   * The input parameters:
   * - ObjectIndex: index of a given object.
   * - Similarities: similarities to other objects.
   * - ExistingProperties: sparse column-major matrix of existing properties for
   *                       all the objects (e.g. OutPropertyCountMatrix).
   * - Sum: dense vector of size at least the number of rows in ExistingProperties 
   *        matrix. Used for intermediate result only.
   *
   * The output are ranked missing properties for the given object.
   */
  static void GetMissingProperties (int ObjectIndex, const TIntFltKdV &Similarities, const TSparseColMatrix &ExistingProperties, TFltV &Sum, TIntFltKdV &MissingProperties);

  /**
   * Computes the missing properties based on specified similarities between the 
   * objects. 
   * The input parameters:
   * - Similarities: similarities between the objects (for each object, there is 
   *                 a vector of similar objects and their similarities).
   * - ExistingProperties: sparse column-major matrix of existing properties for
   *                       all the objects (e.g. OutPropertyCountMatrix).
   * - N: the maximum number of missing properties stored for every object.
   * - NumThreads: number of parallel computations.
   *
   * The output are ranked missing properties for every objects.
   */
  static void GetMissingProperties (const TVec<TIntFltKdV> &Similarities, const TSparseColMatrix &ExistingProperties, int N, int NumThreads, TVec<TIntFltKdV> &MissingProperties);

  /// Prints at most N properties of the specified property vector.
  static void PrintProperties (const TIntFltKdV &Properties, const TStrSet &PropStrs, int N, TSOut &Output);

  /// Prints objects and theirs (at most) N missing properties.
  static void PrintMissingProperties (const TVec<TIntFltKdV> &MissingProperties, const TIntV &Objects, const TStrSet &NodeStrs, const TStrSet &PropStrs, int N, TSOut &Output);
};

template<class TFunctor>
void TPropertyUtils::GetProperties (const TGraph &G, TFunctor &PropertyFunctor, TIntV &Properties)
{
  TIntSet Set;

  for (TGraph::TEdgeI It  = G.BegEI(); It < G.EndEI(); It++) {
    int Property = It.GetDat();
    if (PropertyFunctor(Property)) {
      Set.AddKey(Property);
    }
  }

  Set.GetKeyV(Properties);
}

void TPropertyUtils::PrintProperties (const TIntV &Properties, const TStrSet &PropertyStrs, TSOut &Output)
{
  for (int i = 0; i < Properties.Len(); i++) {
    Output.PutStr(PropertyStrs[Properties[i]]);
    Output.PutStr("\n");
  }
}

void TPropertyUtils::ToHashMap (const TIntV &Vector, TIntH &Map)
{
  for (int i = 0; i < Vector.Len(); i++) {
    Map.AddDat(Vector[i], i);
  }
}

void TPropertyUtils::GetSrcObjects (const TIntV &Properties, const TIntV &Objects, const TGraph &G, TVec<TIntV> &SrcObjects)
{
  TIntH PropertiesMap;
  TIntH ObjectsMap;
  ToHashMap(Properties, PropertiesMap);
  ToHashMap(Objects, ObjectsMap);

  int NumProperties = Properties.Len();

  TVec<TIntSet> SrcObjectsSet(NumProperties, NumProperties);

  for (TGraph::TEdgeI It = G.BegEI(); It < G.EndEI(); It++) {
    int Property = It.GetDat();
    int PropertyKeyId = PropertiesMap.GetKeyId(Property);
    if (PropertyKeyId == -1) {
      continue;
    }

    int Src = It.GetSrcNId();
    int SrcKeyId = ObjectsMap.GetKeyId(Src);
    if (SrcKeyId == -1) {
      continue;
    }

    int PropertyIndex = PropertiesMap[PropertyKeyId];
    int SrcIndex = ObjectsMap[SrcKeyId];
    SrcObjectsSet[PropertyIndex].AddKey(SrcIndex);
  }

  SrcObjects.Gen(NumProperties, NumProperties);
  for (int i = 0; i < NumProperties; i++) {
    SrcObjectsSet[i].GetKeyV(SrcObjects[i]);
  }
}

void TPropertyUtils::GetDstObjects (const TIntV &Properties, const TIntV &Objects, const TGraph &G, TVec<TIntV> &DstObjects)
{
  TIntH PropertiesMap;
  TIntH ObjectsMap;
  ToHashMap(Properties, PropertiesMap);
  ToHashMap(Objects, ObjectsMap);

  int NumProperties = Properties.Len();

  TVec<TIntSet> DstObjectsSet(NumProperties, NumProperties);

  for (TGraph::TEdgeI It = G.BegEI(); It < G.EndEI(); It++) {
    int Property = It.GetDat();
    int PropertyKeyId = PropertiesMap.GetKeyId(Property);
    if (PropertyKeyId == -1) {
      continue;
    }

    int Src = It.GetDstNId();
    int SrcKeyId = ObjectsMap.GetKeyId(Src);
    if (SrcKeyId == -1) {
      continue;
    }

    int PropertyIndex = PropertiesMap[PropertyKeyId];
    int SrcIndex = ObjectsMap[SrcKeyId];
    DstObjectsSet[PropertyIndex].AddKey(SrcIndex);
  }

  DstObjects.Gen(NumProperties, NumProperties);
  for (int i = 0; i < NumProperties; i++) {
    DstObjectsSet[i].GetKeyV(DstObjects[i]);
  }
}

void TPropertyUtils::PrintObjects (const TIntV &PropertyObjects, const TIntV &Objects, const TStrSet &NodeStrs, TSOut &Output)
{
  for (int i = 0; i < PropertyObjects.Len(); i++) {
    int ObjectIndex = PropertyObjects[i];
    int ObjectId = Objects[ObjectIndex];
    Output.PutStr(NodeStrs[ObjectId]);
    Output.PutStr("\n");
  }
}

void TPropertyUtils::PrintPropertyObjects (const TVec<TIntV> &PropertyObjects, const TIntV &Objects, const TIntV &Properties, const TStrSet &NodeStrs, const TStrSet &PropStrs, TSOut &Output)
{
  for (int i = 0; i < PropertyObjects.Len(); i++) {
    int PropertyId = Properties[i];
    Output.PutStr("-->");
    Output.PutStr(PropStrs[PropertyId]);
    Output.PutStr("\n");

    PrintObjects(PropertyObjects[i], Objects, NodeStrs, Output);
			
    Output.PutStr("\n");
  }
}

void TPropertyUtils::ExcludeExistingProperties (const TFltV &AllProperties, const TIntFltKdV &ExistingProperties, TIntFltKdV &Properties)
{
  int i = 0;
  int j = 0;

  while (i < AllProperties.Len()) {
    if (j < ExistingProperties.Len() && i == ExistingProperties[j].Key) {
      j++;
    } else {
      if (AllProperties[i] > 0) {
        Properties.Add(TIntFltKd(i, AllProperties[i]));
      }
    }

    i++;
  }
}

void TPropertyUtils::GetMissingProperties (int ObjectIndex, const TIntFltKdV &Similarities, const TSparseColMatrix &ExistingProperties, TFltV &Sum, TIntFltKdV &MissingProperties)
{
  TLAMisc::FillZero(Sum);

  TLAUtils::GetWeightedSum(ExistingProperties, Similarities, Sum);

  ExcludeExistingProperties(Sum, ExistingProperties.ColSpVV[ObjectIndex], MissingProperties);

  MissingProperties.SortCmp(TIntFltKdComp());
}

void TPropertyUtils::GetMissingProperties (const TVec<TIntFltKdV> &Similarities, const TSparseColMatrix &ExistingProperties, int N, int NumThreads, TVec<TIntFltKdV> &MissingProperties)
{
  int NumObjects = Similarities.Len();
  int NumProperties = ExistingProperties.GetRows();

  MissingProperties.Gen(NumObjects, NumObjects);

  omp_set_num_threads(NumThreads);

  TVec<TFltV> Sums(NumThreads, NumThreads);
  for (int i = 0; i < NumThreads; i++) {
    Sums[i].Gen(NumProperties, NumProperties);
  }

  int Count = 0;

  #pragma omp parallel for
  for (int i = 0; i < NumObjects; i++) {
    #pragma omp atomic
    Count++;

    int ThreadNum = omp_get_thread_num();
    TFltV &Sum = Sums[ThreadNum];
    
    TIntFltKdV &MissingProps = MissingProperties[i];
    GetMissingProperties(i, Similarities[i], ExistingProperties, Sum, MissingProps);

    if (TLinAlg::NormL1(MissingProps) > 0) {
      TLinAlg::NormalizeL1(MissingProps);
    }

    int NewSize = MissingProps.Len() > N ? N : MissingProps.Len();
    MissingProps.Trunc(NewSize);

    printf("%f\r", Count / (double)NumObjects);
  }
}

void TPropertyUtils::PrintProperties (const TIntFltKdV &Properties, const TStrSet &PropStrs, int N, TSOut &Output)
{
  for (int i = 0; i < Properties.Len() && i < N; i++) {
    const TIntFltKd &Pair = Properties[i];

    Output.PutFlt(Pair.Dat);
    Output.PutStr("\t");
    Output.PutStr(PropStrs[Pair.Key]);
    Output.PutStr("\n");
  }
}

void TPropertyUtils::PrintMissingProperties (const TVec<TIntFltKdV> &MissingProperties, const TIntV &Objects, const TStrSet &NodeStrs, const TStrSet &PropStrs, int N, TSOut &Output)
{
  for (int i = 0; i < Objects.Len(); i++) {
    Output.PutStr("-->");
    Output.PutStr(NodeStrs[Objects[i]]);
    Output.PutStr("\n");

    PrintProperties(MissingProperties[i], PropStrs, N, Output);

    Output.PutStr("\n");
  }
}

#endif
