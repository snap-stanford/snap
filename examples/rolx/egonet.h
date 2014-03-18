#ifndef __EGONET_H__
#define __EGONET_H__

#include "stdafx.h"

//class TUNEgonet;
//class TNEgonet;
//typedef TPt<TUNEgonet> PUNEgonet;
//typedef TPt<TNEgonet> PNEgonet;
//
//class TUNEgonet : public TUNGraph {
//private:
//  TInt CtrNId;
//  TInt WthnEdges, ArndEdges;
//public:
//  TUNEgonet(const PUNGraph& Graph, const int CtrNId);
//  int GetWthnEdges() { return WthnEdges; }
//  int GetArndEdges() { return ArndEdges; }
//  int GetCtrNId() { return CtrNId; }
//};
//
//class TNEgonet : public TNGraph {
//private:
//  TInt CtrNId;
//  TInt WthnEdges, InEdges, OutEdges;
//public:
//  TNEgonet(const PNGraph& Graph, const int CtrNId);
//  int GetWthEdges() { return WthnEdges; }
//  int GetInEdges() { return InEdges; }
//  int GetOutEdges() { return OutEdges; }
//  int GetCtrNId() { return CtrNId; }
//};
//

// get egonet of a center node
/// Returns the egonet of node CtrNId as center in undirected graph Graph. And returns number of edges around the egonet.  ##TSnap::GetEgonet-1
PUNGraph GetEgonet(const PUNGraph& Graph, const int CtrNId, int& ArndEdges);
/// Returns the egonet of node CtrNId as center in directed graph Graph. And returns number of edges go in and out the egonet.  ##TSnap::GetEgonet-2
PNGraph GetEgonet(const PNGraph& Graph, const int CtrNId, int& InEdges, int& OutEdges);

#endif