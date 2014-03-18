#include "egonet.h"

//TUNEgonet::TUNEgonet(const PUNGraph& Graph, const int CtrNId) {
//  this->AddNode(CtrNId);
//  this->CtrNId = CtrNId;
//  const TUNGraph::TNodeI& CtrNode = Graph->GetNI(CtrNId);
//  for (int i = 0; i < CtrNode.GetInDeg(); ++i) {
//    this->AddNode(CtrNode.GetInNId(i));
//  }
//  this->ArndEdges = 0;
//  for (int i = 0; i < CtrNode.GetInDeg(); ++i) {
//    int NbrNId = CtrNode.GetInNId(i);
//    const TUNGraph::TNodeI& NbrNode = Graph->GetNI(NbrNId);
//    for (int j = 0; j < NbrNode.GetInDeg(); ++j) {
//      int NbrNbrNId = NbrNode.GetInNId(j);
//      if (this->IsNode(NbrNbrNId)) {
//        if (!this->IsEdge(NbrNId, NbrNbrNId)) {
//          this->AddEdge(NbrNId, NbrNbrNId);
//        }
//      } else {
//        this->ArndEdges++;
//      }
//    }
//  }
//  this->WthnEdges = this->GetEdges();
//}
//
//TNEgonet::TNEgonet(const PNGraph& Graph, const int CtrNId) {
//  this->AddNode(CtrNId);
//  this->CtrNId = CtrNId;
//  const TNGraph::TNodeI& CtrNode = Graph->GetNI(CtrNId);
//  for (int i = 0; i < CtrNode.GetDeg(); ++i) {
//    this->AddNode(CtrNode.GetNbrNId(i));
//  }
//  this->InEdges = 0;
//  this->OutEdges = 0;
//  for (int i = 0; i < CtrNode.GetDeg(); ++i) {
//    int NbrNId = CtrNode.GetNbrNId(i);
//    const TNGraph::TNodeI& NbrNode = Graph->GetNI(NbrNId);
//    for (int j = 0; j < NbrNode.GetInDeg(); ++j) {
//      int NbrNbrNId = NbrNode.GetInNId(j);
//      if (this->IsNode(NbrNbrNId)) {
//        this->AddEdge(NbrNbrNId, NbrNId);
//      } else {
//        this->InEdges++;
//      }
//    }
//    for (int j = 0; j < NbrNode.GetOutDeg(); ++j) {
//      int NbrNbrNId = NbrNode.GetOutNId(j);
//      if (!this->IsNode(NbrNbrNId)) {
//        this->OutEdges++;
//      }
//    }
//  }
//  this->WthnEdges = this->GetEdges();
//}

PUNGraph GetEgonet(const PUNGraph& Graph, const int CtrNId, int& ArndEdges) {
  PUNGraph NewGraphPt = TUNGraph::New();
  TUNGraph& NewGraph = *NewGraphPt;
  NewGraph.AddNode(CtrNId);
  const TUNGraph::TNodeI& CtrNode = Graph->GetNI(CtrNId);
  for (int i = 0; i < CtrNode.GetInDeg(); ++i) {
    NewGraph.AddNode(CtrNode.GetInNId(i));
  }
  ArndEdges = 0;
  for (int i = 0; i < CtrNode.GetInDeg(); ++i) {
    int NbrNId = CtrNode.GetInNId(i);
    const TUNGraph::TNodeI& NbrNode = Graph->GetNI(NbrNId);
    for (int j = 0; j < NbrNode.GetInDeg(); ++j) {
      int NbrNbrNId = NbrNode.GetInNId(j);
      if (NewGraph.IsNode(NbrNbrNId)) {
        if (!NewGraph.IsEdge(NbrNId, NbrNbrNId)) {
          NewGraph.AddEdge(NbrNId, NbrNbrNId);
        }
      } else {
        ArndEdges++;
      }
    }
  }
  return NewGraphPt;
}

PNGraph GetEgonet(const PNGraph& Graph, const int CtrNId, int& InEdges, int& OutEdges) {
  PNGraph NewGraphPt = TNGraph::New();
  TNGraph& NewGraph = *NewGraphPt;
  NewGraph.AddNode(CtrNId);
  const TNGraph::TNodeI& CtrNode = Graph->GetNI(CtrNId);
  for (int i = 0; i < CtrNode.GetDeg(); ++i) {
    NewGraph.AddNode(CtrNode.GetNbrNId(i));
  }
  InEdges = 0;
  OutEdges = 0;
  for (int i = 0; i < CtrNode.GetDeg(); ++i) {
    int NbrNId = CtrNode.GetNbrNId(i);
    const TNGraph::TNodeI& NbrNode = Graph->GetNI(NbrNId);
    for (int j = 0; j < NbrNode.GetInDeg(); ++j) {
      int NbrNbrNId = NbrNode.GetInNId(j);
      if (NewGraph.IsNode(NbrNbrNId)) {
        NewGraph.AddEdge(NbrNbrNId, NbrNId);
      } else {
        InEdges++;
      }
    }
    for (int j = 0; j < NbrNode.GetOutDeg(); ++j) {
      int NbrNbrNId = NbrNode.GetOutNId(j);
      if (!NewGraph.IsNode(NbrNbrNId)) {
        OutEdges++;
      }
    }
  }
  return NewGraphPt;
}
