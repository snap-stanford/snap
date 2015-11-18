/////////////////////////////////////////////////
// Attribute Node Edge Network

const int TCVNode::DEF_WEIGHT = 4;
const int TCVNode::DEF_WEIGHT_COEFF = 2;
const int TCVNode::DEF_EXPAND_RATIO = 2;

/*
void TMNet::DelNode(const int& NId) {
  int i;

  const TNode& Node = GetNode(NId);
  for (int out = 0; out < Node.GetOutDeg(); out++) {
    const int EId = Node.GetOutEId(out);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetSrcNId() == NId);
    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
    // Delete from Edge Attributes
    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
      TVec<TInt>& IntVec = VecOfIntVecsE[i];
      IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
    }
    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
      TVec<TStr>& StrVec = VecOfStrVecsE[i];
      StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
    }
    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
      TVec<TFlt>& FltVec = VecOfFltVecsE[i];
      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
    }
    EdgeH.DelKey(EId);
  }
  for (int in = 0; in < Node.GetInDeg(); in++) {
    const int EId = Node.GetInEId(in);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetDstNId() == NId);
    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
    // Delete from Edge Attributes
    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
      TVec<TInt>& IntVec = VecOfIntVecsE[i];
      IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
    }
    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
      TVec<TStr>& StrVec = VecOfStrVecsE[i];
      StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
    }
    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
      TVec<TFlt>& FltVec = VecOfFltVecsE[i];
      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
    }
    EdgeH.DelKey(EId);
  }

  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsN[i];
    IntVec[NodeH.GetKeyId(NId)] = TInt::Mn;
  }
  for (i = 0; i < VecOfStrVecsN.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsN[i];
    StrVec[NodeH.GetKeyId(NId)] = TStr::GetNullStr();
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsN[i];
    FltVec[NodeH.GetKeyId(NId)] = TFlt::Mn;
  }
  NodeH.DelKey(NId);
}*/

/*
void TMNet::DelEdge(const int& EId) {
  int i;

  IAssert(IsEdge(EId));
  const int SrcNId = GetEdge(EId).GetSrcNId();
  const int DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  EdgeH.DelKey(EId);

  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    IntVec.Ins(EdgeH.GetKeyId(EId), TInt::Mn);
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec.Ins(EdgeH.GetKeyId(EId), TStr::GetNullStr());
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec.Ins(EdgeH.GetKeyId(EId), TFlt::Mn);
  }
}

// delete all edges between the two nodes
void TMNet::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  int EId = 0;
  bool Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  IAssert(Edge); // there is at least one edge
  while (Edge) {
    DelEdge(EId);
    Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  }
}

void TMNet::Defrag(const bool& OnlyNodeLinks) {
  for (int kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

bool TMNet::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int prevEId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetOutEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetOutEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
      const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetInEId(e), Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetInEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %d source node %d does not exist.", Edge.GetId(), Edge.GetSrcNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %d destination node %d does not exist.", Edge.GetId(), Edge.GetDstNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

void TMNet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  const int EdgePlaces = (int) ceil(log10((double) GetEdges()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Network: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*d]\n", NodePlaces, NodeI.GetId());
    // load node attributes
    TIntV IntAttrN;
    IntAttrValueNI(NodeI.GetId(), IntAttrN);
    fprintf(OutF, "    nai[%d]", IntAttrN.Len());
    for (int i = 0; i < IntAttrN.Len(); i++) {
      fprintf(OutF, " %*i", NodePlaces, IntAttrN[i]()); }
    TStrV StrAttrN;
    StrAttrValueNI(NodeI.GetId(), StrAttrN);
    fprintf(OutF, "    nas[%d]", StrAttrN.Len());
    for (int i = 0; i < StrAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, StrAttrN[i]()); }
    TFltV FltAttrN;
    FltAttrValueNI(NodeI.GetId(), FltAttrN);
    fprintf(OutF, "    naf[%d]", FltAttrN.Len());
    for (int i = 0; i < FltAttrN.Len(); i++) {
      fprintf(OutF, " %*f", NodePlaces, FltAttrN[i]()); }

    fprintf(OutF, "    in[%d]", NodeI.GetInDeg());
    for (int edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetInEId(edge)); }
    fprintf(OutF, "\n");
    fprintf(OutF, "    out[%d]", NodeI.GetOutDeg());
    for (int edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetOutEId(edge)); }
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*d]  %*d  ->  %*d", EdgePlaces, EdgeI.GetId(), NodePlaces, EdgeI.GetSrcNId(), NodePlaces, EdgeI.GetDstNId());

    // load edge attributes
    TIntV IntAttrE;
    IntAttrValueEI(EdgeI.GetId(), IntAttrE);
    fprintf(OutF, "    eai[%d]", IntAttrE.Len());
    for (int i = 0; i < IntAttrE.Len(); i++) {
      fprintf(OutF, " %*i", EdgePlaces, IntAttrE[i]());
    }
    TStrV StrAttrE;
    StrAttrValueEI(EdgeI.GetId(), StrAttrE);
    fprintf(OutF, "    eas[%d]", StrAttrE.Len());
    for (int i = 0; i < StrAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, StrAttrE[i]());
    }
    TFltV FltAttrE;
    FltAttrValueEI(EdgeI.GetId(), FltAttrE);
    fprintf(OutF, "    eaf[%d]", FltAttrE.Len());
    for (int i = 0; i < FltAttrE.Len(); i++) {
      fprintf(OutF, " %*f", EdgePlaces, FltAttrE[i]());
    }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

TFlt TMNet::GetWeightOutEdges(const TNodeI& NI, const TStr& attr) {
  TNode Node = GetNode(NI.GetId());
  TIntV OutEIdV = Node.OutEIdV;
  TFlt total = 0;
  int len = Node.OutEIdV.Len();
  for (int i = 0; i < len; i++) {
    total += GetFltAttrDatE(Node.OutEIdV[i], attr);
  }
  return total;
}

void TMNet::GetWeightOutEdgesV(TFltV& OutWeights, const TFltV& AttrVal) {
  for (TEdgeI it = BegEI(); it < EndEI(); it++) {
    int EId = it.GetId();
    int SrcId = it.GetSrcNId();
    OutWeights[SrcId] +=AttrVal[GetFltKeyIdE(EId)];
  }
}

bool TMNet::IsFltAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == FltType);
}

bool TMNet::IsIntAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == IntType);
}

bool TMNet::IsStrAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == StrType);
}
*/
