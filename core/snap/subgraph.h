namespace TSnap {

/////////////////////////////////////////////////
// Convert between graph types and get subgraphs

// node subgraphs
template<class PGraph> PGraph GetSubGraph(const PGraph& Graph, const TIntV& NIdV);
PUNGraph GetSubGraph(const PUNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes);
PNGraph GetSubGraph(const PNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes);

// edge subgraphs
template<class PGraph> PGraph GetESubGraph(const PGraph& Graph, const TIntV& EIdV);
template<class PGraph> PGraph GetESubGraph(const PGraph& Graph, const TIntPrV& EdgeV);
template<class PGraph, class TEdgeDat> PGraph GetEDatSubGraph(const PGraph& Graph, const TEdgeDat& EDat, const int& Cmp);
template<class PGraph, class TEdgeDat> PGraph GetEDatSubGraph(const PGraph& Graph, const TIntV& NIdV, const TEdgeDat& EDat, const int& Cmp);

// convert between the graphs. Does NOT copy the data
template<class POutGraph, class PInGraph> POutGraph ConvertGraph(const PInGraph& InGraph, const bool& RenumberNodes=false);
template<class POutGraph, class PInGraph> POutGraph ConvertSubGraph(const PInGraph& InGraph, const TIntV& NIdV, const bool& RenumberNodes=false);
template<class POutGraph, class PInGraph> POutGraph ConvertESubGraph(const PInGraph& InGraph, const TIntV& EIdV, const bool& RenumberNodes=false);

// get random (induced) subgraphs 
template<class PGraph> PGraph GetRndSubGraph(const PGraph& Graph, const int& NNodes);
template<class PGraph> PGraph GetRndESubGraph(const PGraph& Graph, const int& NEdges);

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
      NewGraph.AddNode(Graph->GetNI(NIdV[n])); 
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
    for (int n = 0; n < NIdV.Len(); n++) {
      if (! HasGraphFlag(typename PGraph::TObj, gfNodeDat)) {
        NewGraph.AddNode(NIdV[n]); }
      else {
        NewGraph.AddNode(Graph->GetNI(NIdV[n])); }
    }
    for (int n = 0; n < NIdV.Len(); n++) {
      const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NIdV[n]);
      const int SrcNId = NI.GetId();
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
    const typename PGraph::TObj::TEdgeI EI = Graph->GetEI(SrcNId, DstNId);
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

// Get a subgraph on NIdV nodes, where edge data is Cmp (-1:less, 0:equal, 1:greater) than EDat
template<class PGraph, class TEdgeDat> 
PGraph GetEDatSubGraph(const PGraph& Graph, const TEdgeDat& EDat, const int& Cmp) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfEdgeDat));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    if ((Cmp==1 && EI()>EDat) || (Cmp==-1 && EI()<EDat) || (Cmp==0 && EI()==EDat)) {
      NewGraph.AddNode(Graph->GetNI(EI.GetSrcNId()));
      NewGraph.AddNode(Graph->GetNI(EI.GetDstNId()));
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
     (Cmp==1 && EI()>EDat) || (Cmp==-1 && EI()<EDat) || (Cmp==0 && EI()==EDat)) {
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

} // namespace TSnap
