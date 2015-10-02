namespace TSnap {

// Returns the NId where the two directions of search meet up, or -1 if no augmenting path exists. ##TSnap::IntFlowBiDBFS
int IntFlowBiDBFS (const PNEANet &Net, const int& CapIndex, TIntV &Flow, TIntQ &FwdNodeQ, TIntH &PredEdgeH, TIntQ &BwdNodeQ, TIntH &SuccEdgeH, const int& SrcNId, const int& SnkNId) {
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
      if (!PredEdgeH.IsKey(NextNId) && Flow[NextEId] > 0) {
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
      if (!PredEdgeH.IsKey(NextNId) && Net->GetIntAttrIndDatE(NextEId, CapIndex) > Flow[NextEId]) {
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
      if (!SuccEdgeH.IsKey(PrevNId) && Flow[PrevEId] > 0) {
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
      if (!SuccEdgeH.IsKey(PrevNId) && Net->GetIntAttrIndDatE(PrevEId, CapIndex) > Flow[PrevEId]) {
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

/// Returns the amount the flow can be augmented over the paths, 0 if no path can be found. ##TSnap::FindAugV
int FindAugV (const PNEANet &Net, const int& CapIndex, TIntV &Flow, TIntQ &FwdNodeQ, TIntH &PredEdgeH, TIntQ &BwdNodeQ, TIntH &SuccEdgeH, TIntV &MidToSrcAugV, TIntV &MidToSnkAugV, const int& SrcNId, const int& SnkNId) {
  int MidPtNId = IntFlowBiDBFS(Net, CapIndex, Flow, FwdNodeQ, PredEdgeH, BwdNodeQ, SuccEdgeH, SrcNId, SnkNId);
  if (MidPtNId == -1) { return 0; }
  int MinAug = TInt::Mx, NId = MidPtNId, AugFlow = 0;
  // Build the path from the midpoint back to the source by tracing through the PredEdgeH
  for (int EId = PredEdgeH.GetDat(NId); NId != SrcNId; EId = PredEdgeH.GetDat(NId)) {
    MidToSrcAugV.Add(EId);
    const TNEANet::TEdgeI &EI = Net->GetEI(EId);
    if (EI.GetSrcNId() == NId) {
      NId = EI.GetDstNId();
      AugFlow = Flow[EId];
    } else {
      NId = EI.GetSrcNId();
      AugFlow = Net->GetIntAttrIndDatE(EId, CapIndex) - Flow[EId];
    }
    if (AugFlow < MinAug) { MinAug = AugFlow; }
  }
  NId = MidPtNId;
  // Build the path from the midpoint back to the sink by tracing through the SuccEdgeH
  for (int EId = SuccEdgeH.GetDat(NId); NId != SnkNId; EId = SuccEdgeH.GetDat(NId)) {
    MidToSnkAugV.Add(EId);
    const TNEANet::TEdgeI &EI = Net->GetEI(EId);
    if (EI.GetDstNId() == NId) {
      NId = EI.GetSrcNId();
      AugFlow = Flow[EId];
    } else {
      NId = EI.GetDstNId();
      AugFlow = Net->GetIntAttrIndDatE(EId, CapIndex) - Flow[EId];
    }
    if (AugFlow < MinAug) { MinAug = AugFlow; }
  }
  return MinAug;
}

int GetMaxFlowIntEK (PNEANet &Net, const int &SrcNId, const int &SnkNId) {
  IAssert(Net->IsNode(SrcNId));
  IAssert(Net->IsNode(SnkNId));
  if (SrcNId == SnkNId) { return 0; }
  int CapIndex = Net->GetIntAttrIndE(CapAttrName);
  TIntV Flow(Net->GetMxEId());
  // Initialize flow values to 0, and make sure capacities are nonnegative
  for (TNEANet::TEdgeI EI = Net->BegEI(); EI != Net->EndEI(); EI++) {
    IAssert(Net->GetIntAttrIndDatE(EI, CapIndex) >= 0);
    Flow[EI.GetId()] = 0;
  }
  // Return 0 if user attempts to flow from a node to itself.
  if (SrcNId == SnkNId) { return 0; }
  int MaxFlow = 0, MinAug, CurNId;
  while (true) {
    TIntV MidToSrcAugV; TIntV MidToSnkAugV;
    TIntQ FwdNodeQ; TIntQ BwdNodeQ;
    TIntH PredEdgeH; TIntH SuccEdgeH;
    MinAug = FindAugV(Net, CapIndex, Flow, FwdNodeQ, PredEdgeH, BwdNodeQ, SuccEdgeH, MidToSrcAugV, MidToSnkAugV, SrcNId, SnkNId);
    if (MinAug == 0) { break; }
    MaxFlow += MinAug;
    CurNId = SrcNId;
    for (int i = MidToSrcAugV.Len() - 1; i >= 0; i--) {
      int NextEId = MidToSrcAugV[i];
      const TNEANet::TEdgeI &EI = Net->GetEI(NextEId);
      if (EI.GetSrcNId() == CurNId) {
        Flow[NextEId] += MinAug;
        CurNId = EI.GetDstNId();
      } else {
        Flow[NextEId] -= MinAug;
        CurNId = EI.GetSrcNId();
      }
    }
    for (int i = 0; i < MidToSnkAugV.Len(); i++) {
      int NextEId = MidToSnkAugV[i];
      const TNEANet::TEdgeI &EI = Net->GetEI(NextEId);
      if (EI.GetSrcNId() == CurNId) {
        Flow[NextEId] += MinAug;
        CurNId = EI.GetDstNId();
      } else {
        Flow[NextEId] -= MinAug;
        CurNId = EI.GetSrcNId();
      }
    }
  }
  return MaxFlow;
}

//#///////////////////////////////////////////////
/// Push relabel attr manager. ##PR_Manager
class TPRManager {
public:
  TPRManager(PNEANet &Net) : Net(Net), CapIndex(0), FlowV(Net->GetMxEId()), ExcessV(Net->GetMxNId()), EdgeNumsV(Net->GetMxNId()), LabelsV(Net->GetMxNId()), LabelCounts(Net->GetNodes() + 1), LabelLimit(0), MaxLabel(Net->GetNodes()), ActiveNodeSet(Net->GetMxNId()), ActiveCount(0) {
    CapIndex = Net->GetIntAttrIndE(CapAttrName);
    for (int i = 0; i <= Net->GetNodes(); i++) { LabelCounts[i] = 0; }
    for (TNEANet::TEdgeI EI = Net->BegEI(); EI != Net->EndEI(); EI++) {
      int EId = EI.GetId();
      IAssert(Capacity(EId) >= 0);
      FlowV[EId] = 0;
    }
    for (TNEANet::TNodeI NI = Net->BegNI(); NI != Net->EndNI(); NI++) {
      int NId = NI.GetId();
      ExcessV[NId] = 0;
      EdgeNumsV[NId] = 0;
      ActiveNodeSet[NId] = 0;
    }
    LabelCounts[0] = Net->GetNodes();
  }

  int Capacity (int EId) {
    return Net->GetIntAttrIndDatE(EId, CapIndex);
  }

  int &Flow (int EId) {
    return FlowV[EId].Val;
  }

  int Label (int NId) {
    return LabelsV[NId];
  }

  int &Excess (int NId) {
    return ExcessV[NId].Val;
  }

  int &EdgeNum (int NId) {
    return EdgeNumsV[NId].Val;
  }

  void SetLabel (int NId, int Label) {
    if (Label > LabelLimit + 1) { Label = MaxLabel; }
    int OldLabel = LabelsV[NId];
    LabelsV[NId] = Label;
    LabelCounts[OldLabel]--;
    LabelCounts[Label]++;
    if (Label != MaxLabel) { LabelLimit = MAX(LabelLimit, Label); }
    if (LabelCounts[OldLabel] == 0) { CheckGap (OldLabel); }
  }

  /// Removes any gaps in node labels. ##TPRManager::CheckGap
  void CheckGap (int GapLabel) {
    for (TNEANet::TNodeI NI = Net->BegNI(); NI != Net->EndNI(); NI++) {
      int NId = NI.GetId();
      int OldLabel = LabelsV[NId];
      if (OldLabel > GapLabel && OldLabel <= LabelLimit) {
        LabelsV[NId] = MaxLabel;
        LabelCounts[OldLabel]--;
        LabelCounts[MaxLabel]++;
        if (IsActive(NId)) { RemoveActive(NId); }
      }
    }
    LabelLimit = GapLabel - 1;
  }

  bool HasActive() {
    return ActiveCount > 0;
  }

  int IsActive(int NId) {
    return ActiveNodeSet[NId];
  }

  int PopActive() {
    while (true) {
      IAssert (!ActiveNodeQ.Empty());
      int NId = ActiveNodeQ.Top();
      ActiveNodeQ.Pop();
      if (ActiveNodeSet[NId])  {
        ActiveNodeSet[NId] = 0;
        ActiveCount--;
        return NId;
      }
    }
    return -1;
  }

  void PushActive(int NId) {
    ActiveNodeSet[NId] = 1;
    ActiveNodeQ.Push(NId);
    ActiveCount++;
  }

  void RemoveActive(int NId) {
    ActiveNodeSet[NId] = 0;
    ActiveCount--;
  }

  int GetMaxLabel() { return MaxLabel; }

private:
  PNEANet &Net;
  int CapIndex;
  TIntV FlowV;

  TIntV ExcessV;
  TIntV EdgeNumsV;

  TIntV LabelsV;
  TIntV LabelCounts;
  int LabelLimit;
  int MaxLabel;

  TIntQ ActiveNodeQ;
  TIntV ActiveNodeSet;
  int ActiveCount;
};

/// Pushes flow from a node \c NId to a neighbor \c OutNId over edge \c EId.
void PushToOutNbr (TPRManager &PRM, const int &NId, const int &OutNId, const int &EId) {
  int MinPush = MIN(PRM.Capacity(EId) - PRM.Flow(EId), PRM.Excess(NId));
  PRM.Flow(EId) += MinPush;
  PRM.Excess(NId) -= MinPush;
  PRM.Excess(OutNId) += MinPush;
}

/// Returns flow from a node \c NId to a neighbor \c InNId over edge \c EId.
void PushToInNbr (TPRManager &PRM, const int &NId, const int &InNId, const int &EId) {
  int MinPush = MIN(PRM.Flow(EId), PRM.Excess(NId));
  PRM.Flow(EId) -= MinPush;
  PRM.Excess(NId) -= MinPush;
  PRM.Excess(InNId) += MinPush;
}

/// Increases the label of a node \c NId to allow valid pushes to some neighbor.
void Relabel (TPRManager &PRM, const int &NId, const TNEANet::TNodeI &NI) {
  int MaxLabel = PRM.GetMaxLabel();
  int MinLabel = MaxLabel;
  for (int EdgeN = 0; EdgeN < NI.GetInDeg(); EdgeN++) {
    if (PRM.Flow(NI.GetInEId(EdgeN)) > 0) {
      int InLabel = PRM.Label(NI.GetInNId(EdgeN));
      MinLabel = MIN(MinLabel, InLabel);
    }
  }
  for (int EdgeN = 0; EdgeN < NI.GetOutDeg(); EdgeN++) {
    if (PRM.Capacity(NI.GetOutEId(EdgeN)) > PRM.Flow(NI.GetOutEId(EdgeN))) {
      int OutLabel = PRM.Label(NI.GetOutNId(EdgeN));
      MinLabel = MIN(MinLabel, OutLabel);
    }
  }
  if (MinLabel == MaxLabel) {
    PRM.SetLabel(NId, MaxLabel);
  } else {
    PRM.SetLabel(NId, MinLabel + 1);
  }
}

/// Returns the ID of the neighbor that \c NId pushes to, -1 if no push was made.
int PushRelabel (TPRManager &PRM, const int &NId, const TNEANet::TNodeI &NI) {
  int EdgeN = PRM.EdgeNum(NId);
  int EId = -1, NbrNId = -1, ResFlow = 0;
  int Cutoff = NI.GetInDeg();
  if (EdgeN < Cutoff) {
    EId = NI.GetInEId(EdgeN);
    NbrNId = NI.GetInNId(EdgeN);
    ResFlow = PRM.Flow(EId);
  } else {
    EId = NI.GetOutEId(EdgeN - Cutoff);
    NbrNId = NI.GetOutNId(EdgeN - Cutoff);
    ResFlow = PRM.Capacity(EId) - PRM.Flow(EId);
  }
  if (ResFlow > 0 && PRM.Label(NId) - 1 == PRM.Label(NbrNId)) {
    if (EdgeN < Cutoff) {
      PushToInNbr(PRM, NId, NbrNId, EId);
    } else {
      PushToOutNbr(PRM, NId, NbrNId, EId);
    }
    return NbrNId;
  }
  if (EdgeN + 1 == NI.GetDeg()) {
    PRM.EdgeNum(NId) = 0;
    Relabel(PRM, NId, NI);
  } else {
    PRM.EdgeNum(NId)++;
  }
  return -1;
}

/// Implements the Global Relabeling heuristic. ##TSnap::GlobalRelabel
void GlobalRelabel (PNEANet &Net, TPRManager &PRM, const int& SrcNId, const int& SnkNId) {
  TIntQ NodeQ;
  int size = Net->GetMxNId();
  TIntV NodeV(size);
  for (int i = 0; i < size; i++) { NodeV[i] = 0; }
  NodeQ.Push(SnkNId);
  NodeV[SnkNId] = 1;
  int MaxLabel = PRM.GetMaxLabel();
  while (!NodeQ.Empty()) {
    // Backward search
    int NId = NodeQ.Top(); NodeQ.Pop();
    const TNEANet::TNodeI &NI = Net->GetNI(NId);
    // Check all edges that point out of the current node for those over which flow can be returned.
    for (int EdgeN = 0; EdgeN < NI.GetOutDeg(); EdgeN++) {
      int OutNId = NI.GetOutNId(EdgeN);
      int EId = NI.GetOutEId(EdgeN);
      if (!NodeV[OutNId] && PRM.Flow(EId) > 0) {
        NodeV[OutNId] = 1;
        NodeQ.Push(OutNId);
        PRM.SetLabel(OutNId, PRM.Label(NId) + 1);
      }
    }
    // Check all edges that point into the current node for those over which flow can be added.
    for (int EdgeN = 0; EdgeN < NI.GetInDeg(); EdgeN++) {
      int InNId = NI.GetInNId(EdgeN);
      int EId = NI.GetInEId(EdgeN);
      if (!NodeV[InNId] && PRM.Capacity(EId) > PRM.Flow(EId)) {
        NodeV[InNId] = 1;
        NodeQ.Push(InNId);
        PRM.SetLabel(InNId, PRM.Label(NId) + 1);
      }
    }
  }

  for (TNEANet::TNodeI NI = Net->BegNI(); NI != Net->EndNI(); NI++) {
    int NId = NI.GetId();
    if (NodeV[NId]) {
      if (PRM.Excess(NId) > 0 && PRM.Label(NId) < MaxLabel && NId != SnkNId) {
        if (!PRM.IsActive(NId)) { PRM.PushActive(NId); }
      }
    } else {
      if (PRM.IsActive(NId)) { PRM.RemoveActive(NId); }
      PRM.SetLabel(NId, MaxLabel);
    }
  }
}

int GetMaxFlowIntPR (PNEANet &Net, const int& SrcNId, const int& SnkNId) {
  IAssert(Net->IsNode(SrcNId));
  IAssert(Net->IsNode(SnkNId));
  if (SrcNId == SnkNId) { return 0; }

  TPRManager PRM(Net);
  int MaxLabel = PRM.GetMaxLabel();

  TNEANet::TNodeI SrcNI = Net->GetNI(SrcNId);
  for (int EdgeN = 0; EdgeN < SrcNI.GetOutDeg(); EdgeN++) {
    int EId = SrcNI.GetOutEId(EdgeN);
    int OutNId = SrcNI.GetOutNId(EdgeN);
    if (OutNId != SrcNId) {
      int Capacity = PRM.Capacity(EId);
      PRM.Flow(EId) = Capacity;
      PRM.Excess(OutNId) = Capacity;
    }
  }
  GlobalRelabel(Net, PRM, SrcNId, SnkNId);
  PRM.SetLabel(SrcNId, MaxLabel);
  int RelabelCount = 1;
  int GRRate = Net->GetNodes();
  while (PRM.HasActive()) {
    int NId = PRM.PopActive();
    const TNEANet::TNodeI &NI = Net->GetNI(NId);
    int PrevLabel = MaxLabel;
    while (PRM.Excess(NId) > 0 && PRM.Label(NId) <= PrevLabel) {
      PrevLabel = PRM.Label(NId);
      int NbrNId = PushRelabel(PRM, NId, NI);
      if (NbrNId != -1 && NbrNId != SnkNId && PRM.Excess(NbrNId) > 0 && !PRM.IsActive(NbrNId)) {
        PRM.PushActive(NbrNId);
      }
    }
    if (PRM.Excess(NId) > 0 && PRM.Label(NId) < MaxLabel) {
      PRM.PushActive(NId);
    }
    if (RelabelCount % GRRate == 0) { GlobalRelabel(Net, PRM, SrcNId, SnkNId); }
  }
  return PRM.Excess(SnkNId);
}


};
