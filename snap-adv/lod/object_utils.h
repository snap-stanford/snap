#ifndef SNAP_OBJECT_UTILS_H
#define SNAP_OBJECT_UTILS_H

#include "stdafx.h"
#include "typedefs.h"
#include "la_utils.h"

/**
 * Author: Klemen Simonic
 *
 * An object is a special type of node in the graph. Very often, objects are
 * resources or nodes with URLs that can be referenced. This module implements 
 * various functionalities regarding the objects in the graph.
 *
 * In this context, the property stands for data on the edge. Let us see some
 * examples to be crystal clear:
 * -  U.S. ---president---> BarackObama
 * -  California ---country--> U.S.
 * The first example shows that U.S. has an out-going property president and the 
 * associated property value is BarackObama, while in the second example, we can 
 * see that U.S. has an in-comming property country.
 */
class TObjectUtils {
public:
  /**
   * Returns the objects of the graph, which are determined via specified object 
   * functor. Object functor should return true, when the specified node id in 
   * the graph represents an object.
   */
  template<class TFunctor>
  static void GetObjects (const TGraph &G, TFunctor &ObjectFunctor, TIntV &Objects);
  /// Prints the specified objects.
  static void PrintObjects (const TIntV &Objects, const TStrSet &NodeStrs, TSOut &output);

  /// Returns the count (number of occurrences) of each in-comming property of a specified node.
  static void GetInPropertyCount (int NodeId, const TGraph &G, TIntH &InPropertyCount);
  /// Returns the count (number of occurrences) of each out-going property of a specified node.
  static void GetOutPropertyCount (int NodeId, const TGraph &G, TIntH &OutPropertyCount);
  /// Returns the count (number of occurrences) of each in-comming and out-going property of a node.
  static void GetPropertyCount (int NodeId, const TGraph &G, TIntH &PropertyCount);

  /// Returns the sparse vector of in-comming property counts of a specified node ordered by property id.
  static void GetInPropertyCount (int NodeId, const TGraph &G, TIntFltKdV &InPropertyCount);
  /// Returns the sparse vector of out-going property counts of a specified node ordered by property id.
  static void GetOutPropertyCount (int NodeId, const TGraph &G, TIntFltKdV &OutPropertyCount);
  /// Returns the sparse vector of in-comming and out-going property counts of a specified node ordered by property id.
  static void GetPropertyCount (int NodeId, const TGraph &G, TIntFltKdV &PropertyCount);

  /// Returns the sparse column-major matrix consisting of in-property counts vectors for the specified nodes.
  static void GetInPropertyCount (const TIntV &Nodes, const TGraph &G, TSparseColMatrix &InPropertyCountMatrix);
  /// Returns the sparse column-major matrix consisting of out-property counts vectors for the specified nodes.
  static void GetOutPropertyCount (const TIntV &Nodes, const TGraph &G, TSparseColMatrix &OutPropertyCountMatrix);
  /// Returns the sparse column-major matrix consisting of property counts vectors for the specified nodes.
  static void GetPropertyCount (const TIntV &Nodes, const TGraph &G, TSparseColMatrix &PropertyCountMatrix);

  /// Prints any property count vector (InPropertyCount, OutPropertyCount, PropertyCount).
  static void PrintPropertyVector (const TIntFltKdV &PropertyVector, const TStrSet &PropStrs, TSOut &Output);
  /// Prints any property count matrix (InPropertyCount, OutPropertyCount, PropertyCount).
	static void PrintPropertyMatrix (const TSparseColMatrix &PropertyMatrix, const TIntV &Objects, const TStrSet &NodeStrs, const TStrSet &PropStrs, TSOut &Output);

  /// Returns the count (number of occurrences) of each in-neighbouring object of a specified node.
	static void GetInNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntH &InNbhCount);
  /// Returns the count (number of occurrences) of each out-neighbouring object of a specified node.
	static void GetOutNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntH &OutNbhCount);
  /// Returns the count (number of occurrences) of each neighbouring object of a specified node.
	static void GetNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntH &NbhCount);

  /// Returns the sparse vector of neighbouring object counts of a specified node ordered by object id.
  static void GetNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntFltKdV &NbhCount);

  /// Returns the sparse column-major matrix consisting of neighbouring object counts vectors for the specified objects.
	static void GetNbhCount (const TIntV &Objects, const TGraph &G, TSparseColMatrix &NbhCountMatrix);
  
  /// Prints neighbour count vector
	static void PrintNbhVector (const TIntFltKdV &NbhVector, const TStrSet &NodeStrs, TSOut &Output);
  /// Prints neighbour count matrix.
	static void PrintNbhMatrix (const TSparseColMatrix &NbhMatrix, const TIntV &Objects, const TStrSet &NodeStrs, TSOut &Output);
};

template<class TFunctor>
void TObjectUtils::GetObjects (const TGraph &G, TFunctor &ObjectFunctor, TIntV &Objects)
{
  for (TGraph::TNodeI it = G.BegNI(); it < G.EndNI(); it++) {
    int NodeId = it.GetId();
    if (ObjectFunctor(NodeId)) {
      Objects.Add(NodeId);
    }
  }
}

void TObjectUtils::PrintObjects (const TIntV &Objects, const TStrSet &NodeStrs, TSOut &Output)
{
  for (int i = 0; i < Objects.Len(); i++) {
    Output.PutStr(NodeStrs[Objects[i]]);
    Output.PutStr("\n");
  }
}

void TObjectUtils::GetInPropertyCount (int NodeId, const TGraph &G, TIntH &InPropertyCount)
{
  TGraph::TNodeI NodeIt = G.GetNI(NodeId);
  for (int i = 0; i < NodeIt.GetInDeg(); i++) {
    int Property = NodeIt.GetInEDat(i);
    int KeyId = InPropertyCount.GetKeyId(Property);
    if (KeyId == -1) {
      InPropertyCount.AddDat(Property, 1);
    } else {
      InPropertyCount[KeyId]++;
    }
  }
}

void TObjectUtils::GetOutPropertyCount (int NodeId, const TGraph &G, TIntH &OutPropertyCount)
{
  TGraph::TNodeI NodeIt = G.GetNI(NodeId);
  for (int i = 0; i < NodeIt.GetOutDeg(); i++) {
    int Property = NodeIt.GetOutEDat(i);
    int KeyId = OutPropertyCount.GetKeyId(Property);
    if (KeyId == -1) {
      OutPropertyCount.AddDat(Property, 1);
    } else {
      OutPropertyCount[KeyId]++;
    }
  }
}

void TObjectUtils::GetPropertyCount (int NodeId, const TGraph &G, TIntH &PropertyCount)
{
  GetInPropertyCount(NodeId, G, PropertyCount);
  GetOutPropertyCount(NodeId, G, PropertyCount);
}

void TObjectUtils::GetInPropertyCount (int NodeId, const TGraph &G, TIntFltKdV &InPropertyCount)
{
  TIntH PropertyToCount;
  GetInPropertyCount(NodeId, G, PropertyToCount);

  TLAUtils::ToIntFltKdV(PropertyToCount, InPropertyCount);
}

void TObjectUtils::GetOutPropertyCount (int NodeId, const TGraph &G, TIntFltKdV &OutPropertyCount)
{
  TIntH PropertyToCount;
  GetOutPropertyCount(NodeId, G, PropertyToCount);

  TLAUtils::ToIntFltKdV(PropertyToCount, OutPropertyCount);
}

void TObjectUtils::GetPropertyCount (int NodeId, const TGraph &G, TIntFltKdV &PropertyCount)
{
  TIntH PropertyToCount;
  GetPropertyCount(NodeId, G, PropertyToCount);

  TLAUtils::ToIntFltKdV(PropertyToCount, PropertyCount);
}

void TObjectUtils::GetInPropertyCount (const TIntV &Nodes, const TGraph &G, TSparseColMatrix &M)
{
  int NumNodes = Nodes.Len();

  M.ColSpVV.Gen(NumNodes, NumNodes);

  for (int i = 0; i < NumNodes; i++) {
    TIntFltKdV &Col = M.ColSpVV[i];
    GetInPropertyCount(Nodes[i], G, Col);
    Col.Trunc();
  }

  M.ColN = NumNodes;
  M.RowN = TLAUtils::GetMaxRowIndex(M) + 1;
}

void TObjectUtils::GetOutPropertyCount (const TIntV &Nodes, const TGraph &G, TSparseColMatrix &M)
{
  int NumNodes = Nodes.Len();

  M.ColSpVV.Gen(NumNodes, NumNodes);

  for (int i = 0; i < NumNodes; i++) {
    TIntFltKdV &Col = M.ColSpVV[i];
    GetOutPropertyCount(Nodes[i], G, Col);
    Col.Trunc();
  }

  M.ColN = NumNodes;
  M.RowN = TLAUtils::GetMaxRowIndex(M) + 1;
}

void TObjectUtils::GetPropertyCount (const TIntV &Nodes, const TGraph &G, TSparseColMatrix &M)
{
  int NumNodes = Nodes.Len();

  M.ColSpVV.Gen(NumNodes, NumNodes);

  for (int i = 0; i < NumNodes; i++) {
    TIntFltKdV &Col = M.ColSpVV[i];
    GetPropertyCount(Nodes[i], G, Col);
    Col.Trunc();
  }

  M.ColN = NumNodes;
  M.RowN = TLAUtils::GetMaxRowIndex(M) + 1;
}

void TObjectUtils::GetInNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntH &NbhCount)
{
  TGraph::TNodeI NodeIt = G.GetNI(NodeId);
  for (int i = 0; i < NodeIt.GetInDeg(); i++) {
    int Nbh = NodeIt.GetInNId(i);

    if (!Objects.IsKey(Nbh)) {
      continue;
    }

    int KeyId = NbhCount.GetKeyId(Nbh);
    if (KeyId == -1) {
      NbhCount.AddDat(Nbh, 1);
    } else {
      NbhCount[KeyId]++;
    }
  }
}

void TObjectUtils::GetOutNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntH &NbhCount)
{
  TGraph::TNodeI NodeIt = G.GetNI(NodeId);
  for (int i = 0; i < NodeIt.GetOutDeg(); i++) {
    int Nbh = NodeIt.GetOutNId(i);

    if (!Objects.IsKey(Nbh)) {
      continue;
    }

    int KeyId = NbhCount.GetKeyId(Nbh);
    if (KeyId == -1) {
      NbhCount.AddDat(Nbh, 1);
    } else {
      NbhCount[KeyId]++;
    }
  }
}

void TObjectUtils::GetNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntH &NbhCount)
{
  GetInNbhCount(NodeId, G, Objects, NbhCount);
  GetOutNbhCount(NodeId, G, Objects, NbhCount);
}

void TObjectUtils::GetNbhCount (int NodeId, const TGraph &G, const TIntSet &Objects, TIntFltKdV &NbhCount)
{
  TIntH NbhToCount;
  GetNbhCount(NodeId, G, Objects, NbhToCount);

  TLAUtils::ToIntFltKdV(NbhToCount, NbhCount);
}

void TObjectUtils::GetNbhCount (const TIntV &Objects, const TGraph &G, TSparseColMatrix &M)
{
  int NumObjects = Objects.Len();

  M.ColSpVV.Gen(NumObjects, NumObjects);

  TIntSet ObjectsSet(Objects);
  for (int i = 0; i < NumObjects; i++) {
    TIntFltKdV &Col = M.ColSpVV[i];
    GetNbhCount(Objects[i], G, ObjectsSet, Col);
    Col.Trunc();
  }

  M.ColN = NumObjects;
  M.RowN = TLAUtils::GetMaxRowIndex(M) + 1;
}

void TObjectUtils::PrintPropertyVector (const TIntFltKdV &PropertyVector, const TStrSet &PropStrs, TSOut &Output)
{
  for (int i = 0; i < PropertyVector.Len(); i++) {
    const TIntFltKd &Pair = PropertyVector[i];
    Output.PutFlt(Pair.Dat);
    Output.PutStr("\t");
    Output.PutStr(PropStrs[Pair.Key]);
    Output.PutStr("\n");
  }
}

void TObjectUtils::PrintPropertyMatrix (const TSparseColMatrix &PropertyMatrix, const TIntV &Objects, const TStrSet &NodeStrs, const TStrSet &PropStrs, TSOut &Output)
{
  for (int i = 0; i < Objects.Len(); i++) {
    Output.PutStr("-->");
    Output.PutStr(NodeStrs[Objects[i]]);
    Output.PutStr("\n");

    PrintPropertyVector(PropertyMatrix.ColSpVV[i], PropStrs, Output);

    Output.PutStr("\n");
  }
}

void TObjectUtils::PrintNbhVector (const TIntFltKdV &NbhVector, const TStrSet &NodeStrs, TSOut &Output)
{
  for (int i = 0; i < NbhVector.Len(); i++) {
    const TIntFltKd &Pair = NbhVector[i];
    Output.PutFlt(Pair.Dat);
    Output.PutStr("\t");
    Output.PutStr(NodeStrs[Pair.Key]);
    Output.PutStr("\n");
  }
}

void TObjectUtils::PrintNbhMatrix (const TSparseColMatrix &NbhMatrix, const TIntV &Objects, const TStrSet &NodeStrs, TSOut &Output)
{
  for (int i = 0; i < Objects.Len(); i++) {
    Output.PutStr("-->");
    Output.PutStr(NodeStrs[Objects[i]]);
    Output.PutStr("\n");

    PrintNbhVector(NbhMatrix.ColSpVV[i], NodeStrs, Output);

    Output.PutStr("\n");
  }
}

#endif
