/*! \file subgraph.h
    \brief Functions and templates to generate subgraphs.
*/

/// Main namespace for all the Snap global entities.
namespace TSnap {

/////////////////////////////////////////////////
// Convert between graph types and get subgraphs

// node subgraphs
/// Returns an induced subgraph of graph Graph with NIdV nodes. ##TSnap::GetSubGraph
template<class PGraph> PGraph GetSubGraph(const PGraph& Graph, const TIntV& NIdV);
/// Returns an induced subgraph of graph Graph with NIdV nodes with an node renumbering. ##TSnap::GetSubGraph
template<class PGraph> PGraph GetSubGraphRenumber(const PGraph& Graph, const TIntV& NIdV);
/// Returns an induced subgraph of an undirected graph Graph with NIdV nodes with an optional node renumbering. ##TSnap::GetSubGraph-1
PUNGraph GetSubGraph(const PUNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes=false);
// Returns an induced subgraph of a directed graph Graph with NIdV nodes with an optional node renumbering. ##TSnap::GetSubGraph-2
PNGraph GetSubGraph(const PNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes=false);
// TODO ROK 2012/08/15 PNGraph GetSubGraph is not documented by doxygen.
//  It is combined with PUNGraph GetSubGraph.

// edge subgraphs
/// Returns a subgraph of graph Graph with EIdV edges. ##TSnap::GetESubGraph
template<class PGraph> PGraph GetESubGraph(const PGraph& Graph, const TIntV& EIdV);
// Returns a subgraph of graph Graph with EdgeV edges. ##TSnap::GetESubGraph-1
template<class PGraph> PGraph GetESubGraph(const PGraph& Graph, const TIntPrV& EdgeV);
// TODO ROK 2012/08/15 PGraph GetESubGraph TIntPrV is not documented by doxygen.
//  It is combined with PGraph GetESubGraph TIntV.
/// Returns a subgraph of graph Graph with edges where edge data matches the parameters. ##TSnap::GetEDatSubGraph
template<class PGraph, class TEdgeDat> PGraph GetEDatSubGraph(const PGraph& Graph, const TEdgeDat& EDat, const int& Cmp);
/// Returns a subgraph of graph Graph with NIdV nodes and edges where edge data matches the parameters. ##TSnap::GetEDatSubGraph-1
template<class PGraph, class TEdgeDat> PGraph GetEDatSubGraph(const PGraph& Graph, const TIntV& NIdV, const TEdgeDat& EDat, const int& Cmp);

// convert between the graphs. Does NOT copy the data
/// Performs conversion of graph InGraph with an optional node renumbering. ##TSnap::ConvertGraph
template<class POutGraph, class PInGraph> POutGraph ConvertGraph(const PInGraph& InGraph, const bool& RenumberNodes=false);
/// Returns an induced subgraph of graph InGraph with NIdV nodes with an optional node renumbering. ##TSnap::ConvertSubGraph
template<class POutGraph, class PInGraph> POutGraph ConvertSubGraph(const PInGraph& InGraph, const TIntV& NIdV, const bool& RenumberNodes=false);
// TODO RS 2012/08/14 find out why TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true) aborts
/// Returns a subgraph of graph InGraph with EIdV edges with an optional node renumbering. ##TSnap::ConvertESubGraph
template<class POutGraph, class PInGraph> POutGraph ConvertESubGraph(const PInGraph& InGraph, const TIntV& EIdV, const bool& RenumberNodes=false);
// does not work on multigraphs

// get random (induced) subgraphs 
/// Returns an induced random subgraph of graph Graph with NNodes nodes. ##TSnap::GetRndSubGraph
template<class PGraph> PGraph GetRndSubGraph(const PGraph& Graph, const int& NNodes);
/// Returns a random subgraph of graph Graph with NEdges edges. ##TSnap::GetRndESubGraph
template<class PGraph> PGraph GetRndESubGraph(const PGraph& Graph, const int& NEdges);

// Get 1st degree egonet of a center node
/// Returns the egonet of node CtrNId as center in undirected graph Graph. And returns number of edges around the egonet.
PUNGraph GetEgonet(const PUNGraph& Graph, const int CtrNId, int& ArndEdgesX);
/// Returns the egonet of node CtrNId as center in directed graph Graph. And returns number of edges go in and out the egonet.
PNGraph GetEgonet(const PNGraph& Graph, const int CtrNId, int& InEgoEdgesX, int& OutEgoEdgesX);

// Get egonet for given radius
/// Returns the egonet of node CtrNId as center for a Graph for a given radius.
template<class PGraph> PGraph GetEgonetHop(const PGraph &Graph, const int CtrNId, const int Radius);
/// Returns the in-egonet of node CtrNId as center in directed graph Graph for a given radius.
template<class PGraph> PGraph GetInEgonetHop(const PGraph& Graph, const int CtrNId, const int Radius);
/// Returns the out-egonet of node CtrNId as center in directed graph Graph for a given radius.
template<class PGraph> PGraph GetOutEgonetHop(const PGraph &Graph, const int CtrNId, const int Radius);

/// Returns the complete egonet of at given radius and copies node and edge attributes
PNEANet GetEgonetAttr(const PNEANet &Graph, const int CtrNId, const int Radius);
/// Returns the in-egonet of at given radius and copies node and edge attributes
PNEANet GetInEgonetAttr(const PNEANet &Graph, const int CtrNId, const int Radius);
/// Returns the out-egonet of at given radius and copies node and edge attributes
PNEANet GetOutEgonetAttr(const PNEANet &Graph, const int CtrNId, const int Radius);

/// Returns the randomly sampled in-egonet with nodes sampled based on percentage, if percent != -1.0, or MaxNum nodes otherwise.
template<class PGraph> PGraph GetInEgonetSub(const PGraph &Graph, const int CtrNId, const int Radius, const int MaxNum = 2, const float percent = -1.0);
/// Returns the randomly sampled egonet with nodes sampled based on percentage or raw number, copying attributes
PNEANet GetInEgonetSubAttr(const PNEANet &Graph, const int CtrNId, const int Radius, const int MaxNum, const float percent);

/// Modifies DstGraph so that it is the union of SrcGraph and DstGraph and returns a copy of DstGraph.
template<class PGraph> PGraph GetGraphUnion(PGraph& DstGraph, const PGraph& SrcGraph);
/// Modifies DstGraph with attributes so that it is the union of SrcGraph and DstGraph and returns a copy of DstGraph.
PNEANet GetGraphUnionAttr(PNEANet &DstGraph, const PNEANet &SrcGraph);

/////////////////////////////////////////////////
// Implementation
namespace TSnapDetail {
// Slow for small sub-graphs as it traverses all the edges of Graph
template <class PGraph, bool IsMultiGraph>
struct TGetSubGraph {
  static PGraph Do(const PGraph& Graph, const TIntV& NIdV) {
    PGraph NewGraphPt = PGraph::TObj::New();
    typename PGraph::TObj& NewGraph = *NewGraphPt;
    NewGraph.Reserve(NIdV.Len(), -1);
    for (int n = 0; n < NIdV.Len(); n++) {
      if (Graph->IsNode(NIdV[n])) {
        NewGraph.AddNode(Graph->GetNI(NIdV[n])); }
    }
    for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
      if (NewGraph.IsNode(EI.GetSrcNId()) && NewGraph.IsNode(EI.GetDstNId())) {
        NewGraph.AddEdge(EI); }
    }
    NewGraph.Defrag();
    return NewGraphPt;
  }
};

template <class PGraph> 
struct TGetSubGraph<PGraph, false> { // not multigraph
  static PGraph Do(const PGraph& Graph, const TIntV& NIdV) {
    CAssert(! HasGraphFlag(typename PGraph::TObj, gfMultiGraph));
    PGraph NewGraphPt = PGraph::TObj::New();
    typename PGraph::TObj& NewGraph = *NewGraphPt;
    NewGraph.Reserve(NIdV.Len(), -1);
    TIntSet NodeSet;
    for (int n = 0; n < NIdV.Len(); n++) {
      if (! HasGraphFlag(typename PGraph::TObj, gfNodeDat)) {
        if (Graph->IsNode(NIdV[n])) { NewGraph.AddNode(NIdV[n]); NodeSet.AddKey(NIdV[n]); } }
      else {
        if (Graph->IsNode(NIdV[n])) { NewGraph.AddNode(Graph->GetNI(NIdV[n])); NodeSet.AddKey(NIdV[n]); } }
    }
    for (int n = 0; n < NodeSet.Len(); n++) {
      const int SrcNId = NodeSet[n];
      const typename PGraph::TObj::TNodeI NI = Graph->GetNI(SrcNId);
      for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        if (NewGraph.IsNode(OutNId)) {
          if (! HasGraphFlag(typename PGraph::TObj, gfEdgeDat)) { 
            NewGraph.AddEdge(SrcNId, OutNId); }
          else { 
            NewGraph.AddEdge(Graph->GetEI(SrcNId, OutNId)); } // also copy data
        }
      }
    }
    NewGraph.Defrag();
    return NewGraphPt;
  }
};
}; // TSnapDetail

template<class PGraph> 
PGraph GetSubGraph(const PGraph& Graph, const TIntV& NIdV) {
  return TSnapDetail::TGetSubGraph<PGraph, HasGraphFlag(typename PGraph::TObj, gfMultiGraph)>
    ::Do(Graph, NIdV);
}

template<class PGraph>
PGraph GetSubGraphRenumber(const PGraph& Graph, const TIntV& NIdV) {
  return GetSubGraph(Graph, NIdV, true);
}

template<class PGraph> 
PGraph GetESubGraph(const PGraph& Graph, const TIntV& EIdV) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfMultiGraph));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(-1, EIdV.Len());
  for (int edge = 0; edge < EIdV.Len(); edge++) {
    const int EId = EIdV[edge];
    IAssert(Graph->IsEdge(EId));
    const typename PGraph::TObj::TEdgeI EI = Graph->GetEI(EId);
    if (! NewGraph.IsNode(EI.GetSrcNId())) {
      NewGraph.AddNode(Graph->GetNI(EI.GetSrcNId())); 
    }
    if (! NewGraph.IsNode(EI.GetDstNId())) {
      NewGraph.AddNode(Graph->GetNI(EI.GetDstNId())); 
    }
    NewGraph.AddEdge(EI);
  }
  return NewGraphPt;
}

template<class PGraph> 
PGraph GetESubGraph(const PGraph& Graph, const TIntPrV& EdgeV) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(-1, EdgeV.Len());

  for (int edge = 0; edge < EdgeV.Len(); edge++) {
    const int SrcNId = EdgeV[edge].Val1;
    const int DstNId = EdgeV[edge].Val2;
    if (! NewGraph.IsNode(SrcNId)) {
      NewGraph.AddNode(Graph->GetNI(SrcNId)); 
    }
    if (! NewGraph.IsNode(DstNId)) {
      NewGraph.AddNode(Graph->GetNI(DstNId)); 
    }

    NewGraph.AddEdge(SrcNId, DstNId);
  }
  return NewGraphPt;
}

// Get a subgraph on NIdV nodes, where edge data is Cmp (-1:less, 0:equal, 1:greater) than EDat
template<class PGraph, class TEdgeDat> 
PGraph GetEDatSubGraph(const PGraph& Graph, const TEdgeDat& EDat, const int& Cmp) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfEdgeDat));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    if ((Cmp==1 && EI()>EDat) || (Cmp==-1 && EI()<EDat) || (Cmp==0 && EI()==EDat)) {
      if (! NewGraph.IsNode(EI.GetSrcNId())) {
        NewGraph.AddNode(Graph->GetNI(EI.GetSrcNId()));
      }
      if (! NewGraph.IsNode(EI.GetDstNId())) {
        NewGraph.AddNode(Graph->GetNI(EI.GetDstNId()));
      }
      NewGraph.AddEdge(EI); 
    }
  }
  return NewGraphPt;
}

// Get a subgraph on NIdV nodes, where edge data is Cmp (-1:less, 0:equal, 1:greater) than EDat
template<class PGraph, class TEdgeDat> 
PGraph GetEDatSubGraph(const PGraph& Graph, const TIntV& NIdV, const TEdgeDat& EDat, const int& Cmp) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfEdgeDat));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(NIdV.Len(), -1);
  for (int n = 0; n < NIdV.Len(); n++) {
    NewGraph.AddNode(Graph->GetNI(NIdV[n])); 
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    if (NewGraph.IsNode(EI.GetSrcNId()) && NewGraph.IsNode(EI.GetDstNId()) &&
     ((Cmp==1 && EI()>EDat)|| (Cmp==-1 && EI()<EDat) || (Cmp==0 && EI()==EDat))) {
      NewGraph.AddEdge(EI); }
  }
  NewGraph.Defrag();
  return NewGraphPt;
}

// Converts between different types of graphs/networks
// Node/edge data is not copied between the graphs.
template<class POutGraph, class PInGraph> 
POutGraph ConvertGraph(const PInGraph& InGraph, const bool& RenumberNodes) {
  POutGraph OutGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& OutGraph = *OutGraphPt;
  OutGraph.Reserve(InGraph->GetNodes(), InGraph->GetEdges());
  if (! RenumberNodes) {
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      OutGraph.AddNode(NI.GetId());
    }
    for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
      OutGraph.AddEdge(EI.GetSrcNId(), EI.GetDstNId());
      if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) { // add edge in the other direction
        OutGraph.AddEdge(EI.GetDstNId(), EI.GetSrcNId()); }
    }
  } else { // renumber nodes so that node ids are 0...N-1
    TIntSet NIdSet(InGraph->GetNodes());
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      const int nid = NIdSet.AddKey(NI.GetId());
      OutGraph.AddNode(nid);
    }
    for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
      const int SrcNId = NIdSet.GetKeyId(EI.GetSrcNId());
      const int DstNId = NIdSet.GetKeyId(EI.GetDstNId());
      OutGraph.AddEdge(SrcNId, DstNId);
      if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) {
        OutGraph.AddEdge(DstNId, SrcNId); }
    }
  }
  //OutGraph.Defrag();
  return OutGraphPt;
}

namespace TSnapDetail {
// Slow for small sub-graphs as it traverses all the edges of Graph
template <class POutGraph, class PInGraph, bool IsMultiGraph>
struct TConvertSubGraph {
  static POutGraph Do(const PInGraph& InGraph, const TIntV& NIdV, const bool& RenumberNodes) {
    POutGraph OutGraphPt = POutGraph::TObj::New();
    typename POutGraph::TObj& OutGraph = *OutGraphPt;
    if (! RenumberNodes) {
      for (int n = 0; n < NIdV.Len(); n++) {
        OutGraph.AddNode(NIdV[n]);
      }
      for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
        if (! OutGraph.IsNode(EI.GetSrcNId()) || ! OutGraph.IsNode(EI.GetDstNId())) { continue; }
        OutGraph.AddEdge(EI.GetSrcNId(), EI.GetDstNId());
        if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) {
          OutGraph.AddEdge(EI.GetDstNId(), EI.GetSrcNId());
        }
      }
    } else { // renumber nodes so that node ids are 0...N-1
      TIntSet NIdSet(InGraph->GetNodes());
      for (int n = 0; n < NIdV.Len(); n++) {
        const int NId = NIdSet.AddKey(NIdV[n]);
        OutGraph.AddNode(NId);
      }
      for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
        const int SrcNId = NIdSet.GetKeyId(EI.GetSrcNId());
        const int DstNId = NIdSet.GetKeyId(EI.GetDstNId());
        if (! OutGraph.IsNode(SrcNId) || ! OutGraph.IsNode(DstNId)) { continue; }
        OutGraph.AddEdge(SrcNId, DstNId);
        if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) {
          OutGraph.AddEdge(DstNId, SrcNId);
        }
      }
    }
    OutGraph.Defrag();
    return OutGraphPt;
  }
};

template <class POutGraph, class PInGraph>
struct TConvertSubGraph<POutGraph, PInGraph, false> { // InGraph is not multigraph
  static POutGraph Do(const PInGraph& InGraph, const TIntV& NIdV, const bool& RenumberNodes) {
    POutGraph OutGraphPt = POutGraph::TObj::New();
    typename POutGraph::TObj& OutGraph = *OutGraphPt;
    if (! RenumberNodes) {
      for (int n = 0; n < NIdV.Len(); n++) {
        OutGraph.AddNode(NIdV[n]); }
      for (int n = 0; n < NIdV.Len(); n++) {
        typename PInGraph::TObj::TNodeI NI = InGraph->GetNI(NIdV[n]);
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          const int dst = NI.GetOutNId(e);
          if (! OutGraph.IsNode(dst)) { continue; }
          OutGraph.AddEdge(NIdV[n], dst);
        }
      }
    } else { // renumber nodes so that node ids are 0...N-1
      TIntSet NIdSet(InGraph->GetNodes());
      for (int n = 0; n < NIdV.Len(); n++) {
        const int NId = NIdSet.AddKey(NIdV[n]);
        OutGraph.AddNode(NId);
      }
      for (int n = 0; n < NIdV.Len(); n++) {
        typename PInGraph::TObj::TNodeI NI = InGraph->GetNI(NIdV[n]);
        const int src = NIdSet.GetKey(NIdV[n]);
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          const int dst = NIdSet.GetKey(NI.GetOutNId(e));
          if (! OutGraph.IsNode(dst)) { continue; }
          OutGraph.AddEdge(src, dst);
        }
      }
    }
    OutGraph.Defrag();
    return OutGraphPt;
  }
};
} // TSnapDetail

// May be slow as it traverses all the edges of the in-graph to create the sub-graph
template<class POutGraph, class PInGraph> 
POutGraph ConvertSubGraph(const PInGraph& InGraph, const TIntV& NIdV, const bool& RenumberNodes) {
  return TSnapDetail::TConvertSubGraph<POutGraph, PInGraph, HasGraphFlag(typename PInGraph::TObj, gfMultiGraph)>::Do(InGraph, NIdV, RenumberNodes);
}

template<class POutGraph, class PInGraph> 
POutGraph ConvertESubGraph(const PInGraph& InGraph, const TIntV& EIdV, const bool& RenumberNodes) {
  CAssert(HasGraphFlag(typename PInGraph::TObj, gfMultiGraph)); // needs to have explicit edges
  POutGraph NewGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(-1, EIdV.Len());
  if (! RenumberNodes) {
    for (int edge = 0; edge < EIdV.Len(); edge++) {
      const int EId = EIdV[edge];
      IAssert(InGraph->IsEdge(EId));
      const typename PInGraph::TObj::TEdgeI EI = InGraph->GetEI(EId);
      const int SrcNId = EI.GetSrcNId();
      const int DstNId = EI.GetDstNId();
      if (! NewGraph.IsNode(SrcNId)) {
        NewGraph.AddNode(SrcNId); }
      if (! NewGraph.IsNode(DstNId)) {
        NewGraph.AddNode(DstNId); }
      NewGraph.AddEdge(SrcNId, DstNId);
    }
  } else {
    // renumber nodes so that node ids are 0...N-1
    TIntSet NIdSet(InGraph->GetNodes());
    for (int edge = 0; edge < EIdV.Len(); edge++) {
      const int EId = EIdV[edge];
      IAssert(InGraph->IsEdge(EId));
      const typename PInGraph::TObj::TEdgeI EI = InGraph->GetEI(EId);
      const int SrcNId = NIdSet.AddKey(EI.GetSrcNId()); // map node ids
      const int DstNId = NIdSet.AddKey(EI.GetDstNId());
      if (! NewGraph.IsNode(SrcNId)) {
        NewGraph.AddNode(SrcNId); }
      if (! NewGraph.IsNode(DstNId)) {
        NewGraph.AddNode(DstNId); }
      NewGraph.AddEdge(SrcNId, DstNId);
    }
  }
  return NewGraphPt;
}

template<class PGraph> 
PGraph GetRndSubGraph(const PGraph& Graph, const int& NNodes) {
  IAssert(NNodes <= Graph->GetNodes());
  TIntV NIdV;
  Graph->GetNIdV(NIdV);
  NIdV.Shuffle(TInt::Rnd);
  NIdV.Del(NNodes, NIdV.Len()-1);
  IAssert(NIdV.Len() == NNodes);
  return GetSubGraph(Graph, NIdV);
}

template<class PGraph> 
PGraph GetRndESubGraph(const PGraph& Graph, const int& NEdges) {
  CAssert(! HasGraphFlag(typename PGraph::TObj, gfMultiGraph));
  TIntPrV EdgeV(Graph->GetEdges(), 0);
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    EdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
  }
  EdgeV.Shuffle(TInt::Rnd);
  EdgeV.Del(NEdges, EdgeV.Len()-1);
  IAssert(EdgeV.Len() == NEdges);
  return GetESubGraph(Graph, EdgeV);
}


// Egonet templatized functions

template<class PGraph>
PGraph GetEgonetHop(const PGraph &Graph, const int CtrNId, const int Radius) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempSwapQueue;
  NewGraph.AddNode(CtrNId);
  Queue1.Clr(false);
  Queue1.Push(CtrNId);
  for (int r = 0; r < Radius; ++r) {
    while (!Queue1.Empty()) {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const typename PGraph::TObj::TNodeI &Node = Graph->GetNI(NId);
      for (int i = 0; i < Node.GetInDeg(); ++i) {
        const int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId)) {
          NewGraph.AddNode(InNId);
          Queue2.Push(InNId);
        }
        if (!NewGraph.IsEdge(InNId, NId)) {
          NewGraph.AddEdge(InNId, NId);
        }
      }
      for (int i = 0; i < Node.GetOutDeg(); ++i) {
        const int OutNId = Node.GetOutNId(i);
        if (!NewGraph.IsNode(OutNId)) {
          NewGraph.AddNode(OutNId);
          Queue2.Push(OutNId);
        }
        if (!NewGraph.IsEdge(NId, OutNId)) {
          NewGraph.AddEdge(NId, OutNId);
        }
      }
      for (int i = 0; i < Node.GetInDeg(); ++i) {
        int InNId = Node.GetInNId(i);
        const typename PGraph::TObj::TNodeI &InNode = Graph->GetNI(InNId);
        for (int j = 0; j < InNode.GetInDeg(); ++j) {
          int NbrInNId = InNode.GetInNId(j);
          if (NewGraph.IsNode(NbrInNId)) {
            if (!NewGraph.IsEdge(NbrInNId, InNId)) {
              NewGraph.AddEdge(NbrInNId, InNId);
            }
          }
        }
        for (int j = 0; j < InNode.GetOutDeg(); ++j) {
          int NbrOutNId = InNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId)) {
            if (!NewGraph.IsEdge(InNId, NbrOutNId)) {
              NewGraph.AddEdge(InNId, NbrOutNId);
            }
          }
        }
      }
      for (int i = 0; i < Node.GetOutDeg(); ++i) {
        int OutNId = Node.GetOutNId(i);
        const typename PGraph::TObj::TNodeI &OutNode = Graph->GetNI(OutNId);
        for (int j = 0; j < OutNode.GetInDeg(); ++j) {
          int NbrInNId = OutNode.GetInNId(j);
          if (NewGraph.IsNode(NbrInNId)) {
            if (!NewGraph.IsEdge(NbrInNId, OutNId)) {
              NewGraph.AddEdge(NbrInNId, OutNId);
            }
          }
        }
        for (int j = 0; j < OutNode.GetOutDeg(); ++j) {
          int NbrOutNId = OutNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId)) {
            if (!NewGraph.IsEdge(OutNId, NbrOutNId)) {
              NewGraph.AddEdge(OutNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempSwapQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempSwapQueue;
  }
  return NewGraphPt;
}

template<class PGraph>
PGraph GetInEgonetHop(const PGraph &Graph, const int CtrNId, const int Radius) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempSwapQueue;
  NewGraph.AddNode(CtrNId);
  Queue1.Clr(false);
  Queue1.Push(CtrNId);
  for (int r = 0; r < Radius; ++r) {
    while (!Queue1.Empty()) {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const typename PGraph::TObj::TNodeI &Node = Graph->GetNI(NId);
      for (int i = 0; i < Node.GetInDeg(); ++i) {
        const int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId)) {
          NewGraph.AddNode(InNId);
          Queue2.Push(InNId);
        }
        if (!NewGraph.IsEdge(InNId, NId)) {
          NewGraph.AddEdge(InNId, NId);
        }
      }
      for (int i = 0; i < Node.GetInDeg(); ++i) {
        int InNId = Node.GetInNId(i);
        const typename PGraph::TObj::TNodeI &InNode = Graph->GetNI(InNId);
        for (int j = 0; j < InNode.GetInDeg(); ++j) {
          int NbrInNId = InNode.GetInNId(j);
          if (NewGraph.IsNode(NbrInNId)) {
            if (!NewGraph.IsEdge(NbrInNId, InNId)) {
              NewGraph.AddEdge(NbrInNId, InNId);
            }
          }
        }
        for (int j = 0; j < InNode.GetOutDeg(); ++j) {
          int NbrOutNId = InNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId)) {
            if (!NewGraph.IsEdge(InNId, NbrOutNId)) {
              NewGraph.AddEdge(InNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempSwapQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempSwapQueue;
  }
  return NewGraphPt;
}

template<class PGraph>
PGraph GetOutEgonetHop(const PGraph &Graph, const int CtrNId, const int Radius) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempSwapQueue;
  NewGraph.AddNode(CtrNId);
  Queue1.Clr(false);
  Queue1.Push(CtrNId);
  for (int r = 0; r < Radius; ++r) {
    while (!Queue1.Empty()) {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const typename PGraph::TObj::TNodeI &Node = Graph->GetNI(NId);
      for (int i = 0; i < Node.GetOutDeg(); ++i) {
        const int OutNId = Node.GetOutNId(i);
        if (!NewGraph.IsNode(OutNId)) {
          NewGraph.AddNode(OutNId);
          Queue2.Push(OutNId);
        }
        if (!NewGraph.IsEdge(NId, OutNId)) {
          NewGraph.AddEdge(NId, OutNId);
        }
      }
      for (int i = 0; i < Node.GetOutDeg(); ++i) {
        int OutNId = Node.GetOutNId(i);
        const typename PGraph::TObj::TNodeI &OutNode = Graph->GetNI(OutNId);
        for (int j = 0; j < OutNode.GetInDeg(); ++j) {
          int NbrInNId = OutNode.GetInNId(j);
          if (NewGraph.IsNode(NbrInNId)) {
            if (!NewGraph.IsEdge(NbrInNId, OutNId)) {
              NewGraph.AddEdge(NbrInNId, OutNId);
            }
          }
        }
        for (int j = 0; j < OutNode.GetOutDeg(); ++j) {
          int NbrOutNId = OutNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId)) {
            if (!NewGraph.IsEdge(OutNId, NbrOutNId)) {
              NewGraph.AddEdge(OutNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempSwapQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempSwapQueue;
  }
  return NewGraphPt;
}

template<class PGraph>
PGraph GetInEgonetSub(const PGraph &Graph, const int CtrNId, const int Radius, const int MaxNum, const float percent) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempSwapQueue;
  TSnapQueue<int> sampleQueue;
  NewGraph.AddNode(CtrNId);
  Queue1.Clr(false);
  Queue1.Push(CtrNId);
  bool usePercent = (percent != -1.0);
  int numSamples = MaxNum;
  for (int r = 0; r < Radius; ++r) {
    while (!Queue1.Empty()) {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const typename PGraph::TObj::TNodeI &Node = Graph->GetNI(NId);
      sampleQueue.Clr(true);
      for (int i = 0; i < Node.GetInDeg(); ++i) {
        const int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId)) {
          sampleQueue.Push(InNId);
        }
      }
      if (usePercent) {
        numSamples = (int) (percent * sampleQueue.Len());
      }
      sampleQueue.Sample(numSamples);
      for (int i = 0; i < numSamples && !sampleQueue.Empty(); ++i) {
        const int InNId = sampleQueue.Top();
        sampleQueue.Pop();
        if (!NewGraph.IsNode(InNId)) {
          NewGraph.AddNode(InNId);
          Queue2.Push(InNId);
        }
        if (!NewGraph.IsEdge(InNId, NId)) {
          NewGraph.AddEdge(InNId, NId);
        }
      }
      for (int i = 0; i < Node.GetInDeg(); ++i) {
        int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId)) { continue; }
        const typename PGraph::TObj::TNodeI &InNode = Graph->GetNI(InNId);
        for (int j = 0; j < InNode.GetInDeg(); ++j) {
          int NbrInNId = InNode.GetInNId(j);
          if (NewGraph.IsNode(NbrInNId)) {
            if (!NewGraph.IsEdge(NbrInNId, InNId)) {
              NewGraph.AddEdge(NbrInNId, InNId);
            }
          }
        }
        for (int j = 0; j < InNode.GetOutDeg(); ++j) {
          int NbrOutNId = InNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId)) {
            if (!NewGraph.IsEdge(InNId, NbrOutNId)) {
              NewGraph.AddEdge(InNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempSwapQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempSwapQueue;
  }
  return NewGraphPt;
}

template<class PGraph> 
PGraph GetGraphUnion(PGraph& DstGraph, const PGraph& SrcGraph) {
  for (typename PGraph::TObj::TNodeI NI = SrcGraph->BegNI(); NI < SrcGraph->EndNI(); NI++) {
    if (! DstGraph->IsNode(NI.GetId())) {
      DstGraph->AddNode(NI.GetId());
    }
  }
  for (typename PGraph::TObj::TEdgeI EI = SrcGraph->BegEI(); EI < SrcGraph->EndEI(); EI++) {
    if (! DstGraph->IsEdge(EI.GetSrcNId(), EI.GetDstNId())) {
      DstGraph->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
    }
  }
  return DstGraph;
}

} // namespace TSnap
