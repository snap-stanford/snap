#ifndef SNAP_RDF_GRAPH_H
#define SNAP_RDF_GRAPH_H

#include "stdafx.h"
#include "typedefs.h"
#include "rdf_parser.h"

namespace TSnap {

/**
 * Author: Klemen Simonic
 *
 * Creates a RDF graph (network) from the specified parser, which only needs to 
 * implement the following function:
 * -  int GetNextTriple(TStr &Subject, TStr &Predicate, TStr &Object)
 * A simple example of a RDF parser is TRDFParser.
 *
 * The function also maps the node and edge string data to integers, which are 
 * used in graph data structured as node and edge data identifiers, while the 
 * original node and edge strings are stored in the corresponding hash sets (NodeStrs,
 * EdgeStrs) and can be accessed via node and edge data identifiers as key ids
 * into corresponding hash set.
 */
template<class TParser>
bool GetGraphFromRDFParser (TParser &Input, TGraph &G, TStrSet &NodeStrs, TStrSet &EdgeStrs)
{
  int Status = 0;
  int LineNum = 0;

  TStr SubjectStr;
  TStr PredicateStr;
  TStr ObjectStr;

  while ( (Status = Input.GetNextTriple(SubjectStr, PredicateStr, ObjectStr)) == 1) {
    LineNum++;

    int SubjectId = NodeStrs.GetKeyId(SubjectStr);
    if (SubjectId == -1) {
      SubjectId = NodeStrs.AddKey(SubjectStr);
      G.AddNode(SubjectId);
    }

    int PredicateId = EdgeStrs.GetKeyId(PredicateStr);
    if (PredicateId == -1) {
      PredicateId = EdgeStrs.AddKey(PredicateStr);
    }

    int ObjectId = NodeStrs.GetKeyId(ObjectStr);
    if (ObjectId == -1) {
      ObjectId = NodeStrs.AddKey(ObjectStr);
      G.AddNode(ObjectId);
    }

    int EdgeId = G.AddEdge(SubjectId, ObjectId);
    G.GetEDat(EdgeId) = PredicateId;
  }

  if (Status == -1) {
      fprintf(stderr, "GetGraphFromRDFParser(): The input is not well formatted. Error at line: %d\n", LineNum + 1);
  }

  return (Status == 0);
}

} //namespace TSnap

#endif
