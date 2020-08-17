namespace TSnap {

/////////////////////////////////////////////////
// Graph Algorithms

// RenumberNodes ... Renumber node ids in the subgraph to 0...N-1
PUNGraph GetSubGraph(const PUNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes) {
  //if (! RenumberNodes) { return TSnap::GetSubGraph(Graph, NIdV); }
  PUNGraph NewGraphPt = TUNGraph::New();
  TUNGraph& NewGraph = *NewGraphPt;
  NewGraph.Reserve(NIdV.Len(), -1);
  TIntSet NIdSet(NIdV.Len());
  for (int n = 0; n < NIdV.Len(); n++) {
    if (Graph->IsNode(NIdV[n])) {
      NIdSet.AddKey(NIdV[n]);
      if (! RenumberNodes) { NewGraph.AddNode(NIdV[n]); }
      else { NewGraph.AddNode(NIdSet.GetKeyId(NIdV[n])); }
    }
  }
  if (! RenumberNodes) {
    for (int n = 0; n < NIdSet.Len(); n++) {
      const int SrcNId = NIdSet[n];
      const TUNGraph::TNodeI NI = Graph->GetNI(SrcNId);
      for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        if (NIdSet.IsKey(OutNId)) {
          NewGraph.AddEdge(SrcNId, OutNId); }
      }
    }
  } else {
    for (int n = 0; n < NIdSet.Len(); n++) {
      const int SrcNId = NIdSet[n];
      const TUNGraph::TNodeI NI = Graph->GetNI(SrcNId);
      for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        if (NIdSet.IsKey(OutNId)) {
          NewGraph.AddEdge(NIdSet.GetKeyId(SrcNId), NIdSet.GetKeyId(OutNId)); }
      }
    }
  }
  return NewGraphPt;
}

// RenumberNodes ... Renumber node ids in the subgraph to 0...N-1
PNGraph GetSubGraph(const PNGraph& Graph, const TIntV& NIdV, const bool& RenumberNodes) {
  //if (! RenumberNodes) { return TSnap::GetSubGraph(Graph, NIdV); }
  PNGraph NewGraphPt = TNGraph::New();
  TNGraph& NewGraph = *NewGraphPt;
  NewGraph.Reserve(NIdV.Len(), -1);
  TIntSet NIdSet(NIdV.Len());
  for (int n = 0; n < NIdV.Len(); n++) {
    if (Graph->IsNode(NIdV[n])) {
      NIdSet.AddKey(NIdV[n]);
      if (! RenumberNodes) { NewGraph.AddNode(NIdV[n]); }
      else { NewGraph.AddNode(NIdSet.GetKeyId(NIdV[n])); }
    }
  }
  if (! RenumberNodes) {
    for (int n = 0; n < NIdSet.Len(); n++) {
      const int SrcNId = NIdSet[n];
      const TNGraph::TNodeI NI = Graph->GetNI(SrcNId);
      for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        if (NIdSet.IsKey(OutNId)) {
          NewGraph.AddEdge(SrcNId, OutNId); }
      }
    }
  } else {
    for (int n = 0; n < NIdSet.Len(); n++) {
      const int SrcNId = NIdSet[n];
      const TNGraph::TNodeI NI = Graph->GetNI(SrcNId);
      for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int OutNId = NI.GetOutNId(edge);
        if (NIdSet.IsKey(OutNId)) {
          NewGraph.AddEdge(NIdSet.GetKeyId(SrcNId), NIdSet.GetKeyId(OutNId)); }
      }
    }
  }
  return NewGraphPt;
}

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
    if (!NewGraph.IsNode(CtrNode.GetNbrNId(i))) {
      NewGraph.AddNode(CtrNode.GetNbrNId(i));
    }
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
      if (NewGraph.IsNode(NbrNbrNId)) {
        NewGraph.AddEdge(NbrNId, NbrNbrNId);
      } else {
        OutEdges++;
      }
    }
  }
  return NewGraphPt;
}

PNGraph GetEgonetHop(const PNGraph &Graph, const int CtrNId, const int Radius)
{
  PNGraph NewGraphPt = TNGraph::New();
  TNGraph &NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempQueue;
  NewGraph.AddNode(CtrNId);
  Queue1.Clr(false);
  Queue1.Push(CtrNId);
  for (int r = 0; r < Radius; ++r)
  {
    while (!Queue1.Empty())
    {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const TNGraph::TNodeI &Node = Graph->GetNI(NId);
      for (int i = 0; i < Node.GetInDeg(); ++i)
      {
        const int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId))
        {
          NewGraph.AddNode(InNId);
          Queue2.Push(InNId);
        }
        if (!NewGraph.IsEdge(InNId, NId))
        {
          NewGraph.AddEdge(InNId, NId);
        }
      }
      for (int i = 0; i < Node.GetInDeg(); ++i)
      {
        int NbrNId = Node.GetInNId(i);
        const TNGraph::TNodeI &NbrNode = Graph->GetNI(NbrNId);
        for (int j = 0; j < NbrNode.GetOutDeg(); ++j)
        {
          int NbrOutNId = NbrNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId))
          {
            if (!NewGraph.IsEdge(NbrNId, NbrOutNId))
            {
              NewGraph.AddEdge(NbrNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempQueue;
  }
  return NewGraphPt;
}

PUNGraph GetEgonetHop(const PUNGraph &Graph, const int CtrNId, const int Radius) {
  PUNGraph NewGraphPt = TUNGraph::New();
  TUNGraph &NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempQueue;
  NewGraph.AddNode(CtrNId);
  Queue1.Clr(false); Queue1.Push(CtrNId);
  for (int r = 0; r < Radius; ++r) {
    while (!Queue1.Empty())
    {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const TUNGraph::TNodeI &Node = Graph->GetNI(NId);
      for (int i = 0; i < Node.GetInDeg(); ++i)
      {
        const int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId))
        {
          NewGraph.AddNode(InNId);
          Queue2.Push(InNId);
        }
        if (!NewGraph.IsEdge(InNId, NId))
        {
          NewGraph.AddEdge(InNId, NId);
        }
      }
      for (int i = 0; i < Node.GetInDeg(); ++i)
      {
        int NbrNId = Node.GetInNId(i);
        const TUNGraph::TNodeI &NbrNode = Graph->GetNI(NbrNId);
        for (int j = 0; j < NbrNode.GetOutDeg(); ++j)
        {
          int NbrOutNId = NbrNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId))
          {
            if (!NewGraph.IsEdge(NbrNId, NbrOutNId))
            {
              NewGraph.AddEdge(NbrNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempQueue;
  }
  return NewGraphPt;
}

void AddNodeWithAttributes(const PNEANet& Graph1, PNEANet& Graph2, const int NId) {
  Graph2->AddNode(NId);
  // Copy Int Attributes
  TStrV IntAttrNames;
  TIntV IntAttrValues;
  Graph1->IntAttrNameNI(NId, IntAttrNames);
  Graph1->IntAttrValueNI(NId, IntAttrValues);
  for (int i = 0; i < IntAttrNames.Len(); i++)
  {
    Graph2->AddIntAttrDatN(NId, IntAttrValues[i], IntAttrNames[i]);
  }
  // Copy Float Attributes
  TStrV FltAttrNames;
  TFltV FltAttrValues;
  Graph1->FltAttrNameNI(NId, FltAttrNames);
  Graph1->FltAttrValueNI(NId, FltAttrValues);
  for (int i = 0; i < FltAttrNames.Len(); i++)
  {
    Graph2->AddFltAttrDatN(NId, FltAttrValues[i], FltAttrNames[i]);
  }
  // Copy Str Attributes
  TStrV StrAttrNames;
  TStrV StrAttrValues;
  Graph1->StrAttrNameNI(NId, StrAttrNames);
  Graph1->StrAttrValueNI(NId, StrAttrValues);
  for (int i = 0; i < StrAttrNames.Len(); i++)
  {
    Graph2->AddStrAttrDatN(NId, StrAttrValues[i], StrAttrNames[i]);
  }
  // Copy IntV Attributes
  TStrV IntVAttrNames;
  TVec<TIntV> IntVAttrValues;
  Graph1->IntVAttrNameNI(NId, IntVAttrNames);
  Graph1->IntVAttrValueNI(NId, IntVAttrValues);
  for (int i = 0; i < IntVAttrNames.Len(); i++)
  {
    Graph2->AddIntVAttrDatN(NId, IntVAttrValues[i], IntVAttrNames[i]);
  }
}

void AddEdgeWithAttributes(const PNEANet &Graph1, PNEANet &Graph2, 
                           const int NId, const int NbrId)
{
  Graph2->AddEdge(NId, NbrId);
  const int EId = Graph2->GetEId(NId, NbrId);
  // Copy Int Attributes
  TStrV IntAttrNames;
  TIntV IntAttrValues;
  Graph1->IntAttrNameEI(EId, IntAttrNames);
  Graph1->IntAttrValueEI(EId, IntAttrValues);
  for (int i = 0; i < IntAttrNames.Len(); i++)
  {
    Graph2->AddIntAttrDatE(EId, IntAttrValues[i], IntAttrNames[i]);
  }
  // Copy Float Attributes
  TStrV FltAttrNames;
  TFltV FltAttrValues;
  Graph1->FltAttrNameEI(EId, FltAttrNames);
  Graph1->FltAttrValueEI(EId, FltAttrValues);
  for (int i = 0; i < FltAttrNames.Len(); i++)
  {
    Graph2->AddFltAttrDatE(EId, FltAttrValues[i], FltAttrNames[i]);
  }
  // Copy Str Attributes
  TStrV StrAttrNames;
  TStrV StrAttrValues;
  Graph1->StrAttrNameEI(EId, StrAttrNames);
  Graph1->StrAttrValueEI(EId, StrAttrValues);
  for (int i = 0; i < StrAttrNames.Len(); i++)
  {
    Graph2->AddStrAttrDatE(EId, StrAttrValues[i], StrAttrNames[i]);
  }
  // Copy IntV Attributes
  TStrV IntVAttrNames;
  TVec<TIntV> IntVAttrValues;
  Graph1->IntVAttrNameEI(EId, IntVAttrNames);
  Graph1->IntVAttrValueEI(EId, IntVAttrValues);
  for (int i = 0; i < IntVAttrNames.Len(); i++)
  {
    Graph2->AddIntVAttrDatE(EId, IntVAttrValues[i], IntVAttrNames[i]);
  }
}

PNEANet GetEgonetHop(const PNEANet &Graph, const int CtrNId, const int Radius)
{
  PNEANet NewGraphPt = PNEANet::New();
  TNEANet &NewGraph = *NewGraphPt;
  TSnapQueue<int> Queue1;
  TSnapQueue<int> Queue2;
  TSnapQueue<int> tempQueue;
  AddNodeWithAttributes(Graph, NewGraphPt, CtrNId);
  Queue1.Clr(false);
  Queue1.Push(CtrNId);
  for (int r = 0; r < Radius; ++r)
  {
    while (!Queue1.Empty())
    {
      const int NId = Queue1.Top();
      Queue1.Pop();
      const TNEANet::TNodeI &Node = Graph->GetNI(NId);
      for (int i = 0; i < Node.GetInDeg(); ++i)
      {
        const int InNId = Node.GetInNId(i);
        if (!NewGraph.IsNode(InNId))
        {
          AddNodeWithAttributes(Graph, NewGraphPt, InNId);
          Queue2.Push(InNId);
        }
        if (!NewGraph.IsEdge(InNId, NId))
        {
          AddEdgeWithAttributes(Graph, NewGraphPt, InNId, NId);
        }
      }
      for (int i = 0; i < Node.GetInDeg(); ++i)
      {
        int NbrNId = Node.GetInNId(i);
        const TNEANet::TNodeI &NbrNode = Graph->GetNI(NbrNId);
        for (int j = 0; j < NbrNode.GetOutDeg(); ++j)
        {
          int NbrOutNId = NbrNode.GetOutNId(j);
          if (NewGraph.IsNode(NbrOutNId))
          {
            if (!NewGraph.IsEdge(NbrNId, NbrOutNId))
            {
              AddEdgeWithAttributes(Graph, NewGraphPt, NbrNId, NbrOutNId);
            }
          }
        }
      }
    }
    tempQueue = Queue1;
    Queue1 = Queue2;
    Queue2 = tempQueue;
  }
  return NewGraphPt;
}

} // namespace TSnap
