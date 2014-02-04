namespace TSnap {

// Bidirectional BFS for finding an augmenting path between the source and sink nodes
// Returns the NId where the two directions of search meet up, or -1 if no augmenting path exists.
// @param FwdNodeQ: Queue of NIds for the forward search from the source node
// @param PredEdgeH: Hash from a NId to EId in the forward search. Indicates which edge was traversed to reach the node,
//                   allowing for tracing the eventual path back to the source.
// @param BwdNodeQ: Queue of NIds for the backward search from the sink node
// @param SuccEdgeH: Hash from a NId to EId in the backward search. Indicates which edge was traversed to reach the node,
//                   allowing for tracing the eventual path back to the sink.
int IntFlowBiDBFS (const PNEANet &Net, TIntQ &FwdNodeQ, TIntH &PredEdgeH, TIntQ &BwdNodeQ, TIntH &SuccEdgeH, const int& SrcNId, const int& SnkNId) {
  FwdNodeQ.Push(SrcNId);
  PredEdgeH.AddDat(SrcNId, -1);
  BwdNodeQ.Push(SnkNId);
  SuccEdgeH.AddDat(SnkNId, -1);
  while (!FwdNodeQ.Empty() && !BwdNodeQ.Empty()) {
    // Forward search
    const TNEANet::TNodeI &FwdNI = Net->GetNI(FwdNodeQ.Top()); FwdNodeQ.Pop();
    // Check all edges that point into the current node for those over which flow can be returned.
    for (int EdgeN = 0; EdgeN < FwdNI.GetInDeg(); EdgeN++) {
      int NextNId = FwdNI.GetInNId(EdgeN);
      int NextEId = FwdNI.GetInEId(EdgeN);
      if (!PredEdgeH.IsKey(NextNId) && Net->GetIntAttrDatE(NextEId, FlowAttrName) > 0) {
        PredEdgeH.AddDat(NextNId, NextEId);
        if (SuccEdgeH.IsKey(NextNId)) {
          return NextNId;
        }
        FwdNodeQ.Push(NextNId);
      }
    }
    // Check all edges that point out of the current node for those over which flow can be added.
    for (int EdgeN = 0; EdgeN < FwdNI.GetOutDeg(); EdgeN++) {
      int NextNId = FwdNI.GetOutNId(EdgeN);
      int NextEId = FwdNI.GetOutEId(EdgeN);
      if (!PredEdgeH.IsKey(NextNId) && Net->GetIntAttrDatE(NextEId, CapAttrName) - Net->GetIntAttrDatE(NextEId, FlowAttrName) > 0) {
        PredEdgeH.AddDat(NextNId, NextEId);
        if (SuccEdgeH.IsKey(NextNId)) {
          return NextNId;
        }
        FwdNodeQ.Push(NextNId);
      }
    }
    // Backward search
    const TNEANet::TNodeI &BwdNI = Net->GetNI(BwdNodeQ.Top());  BwdNodeQ.Pop();
    // Check all edges that point out of the current node for those over which flow can be returned.
    for (int EdgeN = 0; EdgeN < BwdNI.GetOutDeg(); EdgeN++) {
      int PrevNId = BwdNI.GetOutNId(EdgeN);
      int PrevEId = BwdNI.GetOutEId(EdgeN);
      if (!SuccEdgeH.IsKey(PrevNId) && Net->GetIntAttrDatE(PrevEId, FlowAttrName) > 0) {
        SuccEdgeH.AddDat(PrevNId, PrevEId);
        if (PredEdgeH.IsKey(PrevNId)) {
          return PrevNId;
        }
        BwdNodeQ.Push(PrevNId);
      }
    }
    // Check all edges that point into the current node for those over which flow can be added.
    for (int EdgeN = 0; EdgeN < BwdNI.GetInDeg(); EdgeN++) {
      int PrevNId = BwdNI.GetInNId(EdgeN);
      int PrevEId = BwdNI.GetInEId(EdgeN);
      if (!SuccEdgeH.IsKey(PrevNId) && Net->GetIntAttrDatE(PrevEId, CapAttrName) - Net->GetIntAttrDatE(PrevEId, FlowAttrName) > 0) {
        SuccEdgeH.AddDat(PrevNId, PrevEId);
        if (PredEdgeH.IsKey(PrevNId)) {
          return PrevNId;
        }
        BwdNodeQ.Push(PrevNId);
      }
    }
  }
  return -1;
}

// Find the augmenting path. Calls bidirectional BFS to find the path, and then builds the two path vectors.
// Returns the amount the flow can be augmented over the paths, 0 if no path can be found.
// @param MidToSrcAugV: Contains the path vector from the midpoint node where the bi-d search met back to the source node
// @param MidToSnkAugV: Contains the path vector from the midpoint node where the bi-d search met back to the sink node
int FindAugV (const PNEANet &Net, TIntQ &FwdNodeQ, TIntH &PredEdgeH, TIntQ &BwdNodeQ, TIntH &SuccEdgeH, TIntV &MidToSrcAugV, TIntV &MidToSnkAugV, const int& SrcNId, const int& SnkNId) {
  int MidPtNId = IntFlowBiDBFS(Net, FwdNodeQ, PredEdgeH, BwdNodeQ, SuccEdgeH, SrcNId, SnkNId);
  if (MidPtNId == -1) { return 0; }
  int MinAug = TInt::Mx, NId = MidPtNId, AugFlow = 0;
  // Build the path from the midpoint back to the source by tracing through the PredEdgeH
  for (int EId = PredEdgeH.GetDat(NId); EId != -1; EId = PredEdgeH.GetDat(NId)) {
    MidToSrcAugV.Add(EId);
    const TNEANet::TEdgeI &EI = Net->GetEI(EId);
    if (EI.GetSrcNId() == NId) {
      NId = EI.GetDstNId();
      AugFlow = Net->GetIntAttrDatE(EId, FlowAttrName);
    } else {
      NId = EI.GetSrcNId();
      AugFlow = Net->GetIntAttrDatE(EId, CapAttrName) - Net->GetIntAttrDatE(EId, FlowAttrName);
    }
    if (AugFlow < MinAug) { MinAug = AugFlow; }
  }
  NId = MidPtNId;
  // Build the path from the midpoint back to the sink by tracing through the SuccEdgeH
  for (int EId = SuccEdgeH.GetDat(NId); EId != -1; EId = SuccEdgeH.GetDat(NId)) {
    MidToSnkAugV.Add(EId);
    const TNEANet::TEdgeI &EI = Net->GetEI(EId);
    if (EI.GetDstNId() == NId) {
      NId = EI.GetSrcNId();
      AugFlow = Net->GetIntAttrDatE(EId, FlowAttrName);
    } else {
      NId = EI.GetDstNId();
      AugFlow = Net->GetIntAttrDatE(EId, CapAttrName) - Net->GetIntAttrDatE(EId, FlowAttrName);
    }
    if (AugFlow < MinAug) { MinAug = AugFlow; }
  }
  return MinAug;
}

// Implementation of Edmonds-Karp Algorithm. Iterates, trying to find an augmenting path from source to sink.
// If a path is found, augment the flow along that path. Flow is maximal when no such path can be found.
int GetMaxFlowInt (PNEANet &Net, const int &SrcNId, const int &SnkNId) {
  IAssert(Net->IsNode(SrcNId));
  IAssert(Net->IsNode(SnkNId));
  // Initialize flow values to 0, and make sure capacities are nonnegative
  for (TNEANet::TEdgeI EI = Net->BegEI(); EI != Net->EndEI(); EI++) {
    IAssert(Net->GetIntAttrDatE(EI, CapAttrName) >= 0);
    Net->AddIntAttrDatE(EI, 0, FlowAttrName);
  }
  // Return 0 if user attempts to flow from a node to itself.
  if (SrcNId == SnkNId) { return 0; }
  int MaxFlow = 0, MinAug, CurNId;
  while (true) {
    TIntV MidToSrcAugV; TIntV MidToSnkAugV;
    TIntQ FwdNodeQ; TIntQ BwdNodeQ;
    TIntH PredEdgeH; TIntH SuccEdgeH;
    MinAug = FindAugV(Net, FwdNodeQ, PredEdgeH, BwdNodeQ, SuccEdgeH, MidToSrcAugV, MidToSnkAugV, SrcNId, SnkNId);
    if (MinAug == 0) { break; }
    MaxFlow += MinAug;
    CurNId = SrcNId;
    for (int i = MidToSrcAugV.Len() - 1; i >= 0; i--) {
      int NextEId = MidToSrcAugV[i];
      const TNEANet::TEdgeI &EI = Net->GetEI(NextEId);
      if (EI.GetSrcNId() == CurNId) {
        Net->AddIntAttrDatE(NextEId, Net->GetIntAttrDatE(NextEId, FlowAttrName) + MinAug, FlowAttrName);
        CurNId = EI.GetDstNId();
      } else {
        Net->AddIntAttrDatE(NextEId, Net->GetIntAttrDatE(NextEId, FlowAttrName) - MinAug, FlowAttrName);
        CurNId = EI.GetSrcNId();
      }
    }
    for (int i = 0; i < MidToSnkAugV.Len(); i++) {
      int NextEId = MidToSnkAugV[i];
      const TNEANet::TEdgeI &EI = Net->GetEI(NextEId);
      if (EI.GetSrcNId() == CurNId) {
        Net->AddIntAttrDatE(NextEId, Net->GetIntAttrDatE(NextEId, FlowAttrName) + MinAug, FlowAttrName);
        CurNId = EI.GetDstNId();
      } else {
        Net->AddIntAttrDatE(NextEId, Net->GetIntAttrDatE(NextEId, FlowAttrName) - MinAug, FlowAttrName);
        CurNId = EI.GetSrcNId();
      }
    }
  }
  return MaxFlow;
}

// Identical implementation of bidirectional BFS, adapted to work for the PNGraph instead.
int IntFlowBiDBFS (const PNGraph &Graph, const TIntPrIntH &CapAttrH, const TIntPrIntH &FlowAttrH, TIntQ &FwdNodeQ, TIntIntPrH &PredEdgeH, TIntQ &BwdNodeQ, TIntIntPrH &SuccEdgeH, const int& SrcNId, const int& SnkNId) {
  FwdNodeQ.Push(SrcNId);
  PredEdgeH.AddDat(SrcNId, TIntPr(-1,-1));
  BwdNodeQ.Push(SnkNId);
  SuccEdgeH.AddDat(SnkNId, TIntPr(-1,-1));
  int NId;
  while (!FwdNodeQ.Empty() && !BwdNodeQ.Empty()) {
    NId = FwdNodeQ.Top(); FwdNodeQ.Pop();
    const TNGraph::TNodeI &FwdNI = Graph->GetNI(NId);
    for (int EdgeN = 0; EdgeN < FwdNI.GetInDeg(); EdgeN++) {
      int NextNId = FwdNI.GetInNId(EdgeN);
      TIntPr NextE(NextNId, NId);
      if (! PredEdgeH.IsKey(NextNId) && FlowAttrH.GetDat(NextE) > 0) {
        PredEdgeH.AddDat(NextNId, NextE);
        if (SuccEdgeH.IsKey(NextNId)) {
          return NextNId;
        }
        FwdNodeQ.Push(NextNId);
      }
    }
    for (int EdgeN = 0; EdgeN < FwdNI.GetOutDeg(); EdgeN++) {
      int NextNId = FwdNI.GetOutNId(EdgeN);
      TIntPr NextE(NId, NextNId);
      if (! PredEdgeH.IsKey(NextNId) && CapAttrH.GetDat(NextE) - FlowAttrH.GetDat(NextE) > 0) {
        PredEdgeH.AddDat(NextNId, NextE);
        if (SuccEdgeH.IsKey(NextNId)) {
          return NextNId;
        }
        FwdNodeQ.Push(NextNId);
      }
    }
    NId = BwdNodeQ.Top(); BwdNodeQ.Pop();
    const TNGraph::TNodeI &BwdNI = Graph->GetNI(NId);
    for (int EdgeN = 0; EdgeN < BwdNI.GetOutDeg(); EdgeN++) {
      int PrevNId = BwdNI.GetOutNId(EdgeN);
      TIntPr PrevE(NId, PrevNId);
      if (! SuccEdgeH.IsKey(PrevNId) && FlowAttrH.GetDat(PrevE) > 0) {
        SuccEdgeH.AddDat(PrevNId, PrevE);
        if (PredEdgeH.IsKey(PrevNId)) {
          return PrevNId;
        }
        BwdNodeQ.Push(PrevNId);
      }
    }
    for (int EdgeN = 0; EdgeN < BwdNI.GetInDeg(); EdgeN++) {
      int PrevNId = BwdNI.GetInNId(EdgeN);
      TIntPr PrevE(PrevNId, NId);
      if (! SuccEdgeH.IsKey(PrevNId) && CapAttrH.GetDat(PrevE) - FlowAttrH.GetDat(PrevE) > 0) {
        SuccEdgeH.AddDat(PrevNId, PrevE);
        if (PredEdgeH.IsKey(PrevNId)) {
          return PrevNId;
        }
        BwdNodeQ.Push(PrevNId);
      }
    }
  }
  return -1;
}

// Identical implementation of FindAugV, adapted for the PNGraph class.
int FindAugV (const PNGraph &Graph, const TIntPrIntH &CapAttrH, const TIntPrIntH &FlowAttrH, TIntQ &FwdNodeQ, TIntIntPrH &PredEdgeH, TIntQ &BwdNodeQ, TIntIntPrH &SuccEdgeH, TIntPrV &MidToSrcAugV, TIntPrV &MidToSnkAugV, const int& SrcNId, const int& SnkNId) {
  int MidPtNId = IntFlowBiDBFS(Graph, CapAttrH, FlowAttrH, FwdNodeQ, PredEdgeH, BwdNodeQ, SuccEdgeH, SrcNId, SnkNId);
  if (MidPtNId == -1) { return 0; }
  int MinAug = TInt::Mx, AugFlow = 0, NId = MidPtNId;
  for (TIntPr &EdgePr = PredEdgeH.GetDat(NId); NId != SrcNId ; EdgePr = PredEdgeH.GetDat(NId)) {
    MidToSrcAugV.Add(EdgePr);
    if (EdgePr.Val1.Val == NId) {
      NId = EdgePr.Val2.Val;
      AugFlow = FlowAttrH.GetDat(EdgePr);
    } else {
      NId = EdgePr.Val1.Val;
      AugFlow = CapAttrH.GetDat(EdgePr) - FlowAttrH.GetDat(EdgePr);
    }
    if (AugFlow < MinAug) { MinAug = AugFlow; }
  }
  NId = MidPtNId;
  for (TIntPr &EdgePr = SuccEdgeH.GetDat(NId); NId != SnkNId; EdgePr = SuccEdgeH.GetDat(NId)) {
    MidToSnkAugV.Add(EdgePr);
    if (EdgePr.Val2.Val == NId) {
      NId = EdgePr.Val1.Val;
      AugFlow = FlowAttrH.GetDat(EdgePr);
    } else {
      NId = EdgePr.Val2.Val;
      AugFlow = CapAttrH.GetDat(EdgePr) - FlowAttrH.GetDat(EdgePr);
    }
    if (AugFlow < MinAug) { MinAug = AugFlow; }
  }
  return MinAug;
}

// Identical implementation of Edmonds-Karp, adapted for the PNGraph class.
int GetMaxFlowInt (const PNGraph &Graph, const TIntPrIntH &CapAttrH, TIntPrIntH &FlowAttrH, const int &SrcNId, const int &SnkNId) {
  IAssert(Graph->IsNode(SrcNId));
  IAssert(Graph->IsNode(SnkNId));
  for (TNGraph::TEdgeI EI = Graph->BegEI(); EI != Graph->EndEI(); EI++) {
    TIntPr EdgePr(EI.GetSrcNId(), EI.GetDstNId());
    IAssert(CapAttrH.GetDat(EdgePr) >= 0);
    FlowAttrH.GetDat(TIntPr(EI.GetSrcNId(), EI.GetDstNId())) = 0;
  }
  if (SrcNId == SnkNId) { return 0; }
  int MaxFlow = 0, MinAug, CurNId;
  while (true) {
    TIntPrV MidToSrcAugV; TIntPrV MidToSnkAugV;
    TIntQ FwdNodeQ; TIntQ BwdNodeQ;
    TIntIntPrH PredEdgeH; TIntIntPrH SuccEdgeH;
    MinAug = FindAugV(Graph, CapAttrH, FlowAttrH, FwdNodeQ, PredEdgeH, BwdNodeQ, SuccEdgeH, MidToSrcAugV, MidToSnkAugV, SrcNId, SnkNId);
    if (MinAug == 0) { break; }
    MaxFlow += MinAug;
    CurNId = SrcNId;
    for (int i = MidToSrcAugV.Len() - 1; i >= 0; i--) {
      TIntPr &EdgePr = MidToSrcAugV[i];
      if (EdgePr.Val1.Val == CurNId) {
        FlowAttrH.GetDat(EdgePr) += MinAug;
        CurNId = EdgePr.Val2.Val;
      } else {
        FlowAttrH.GetDat(EdgePr) -= MinAug;
        CurNId = EdgePr.Val1.Val;
      }
    }
    for (int i = 0; i < MidToSnkAugV.Len(); i++) {
      TIntPr &EdgePr = MidToSnkAugV[i];
      if (EdgePr.Val1.Val == CurNId) {
        FlowAttrH.GetDat(EdgePr) += MinAug;
        CurNId = EdgePr.Val2.Val;
      } else {
        FlowAttrH.GetDat(EdgePr) -= MinAug;
        CurNId = EdgePr.Val1.Val;
      }
    }
  }
  return MaxFlow;
}

};
