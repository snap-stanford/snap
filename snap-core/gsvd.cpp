/////////////////////////////////////////////////
// Directed Graph Matrix -- sparse {0,1} row matrix 
bool TNGraphMtx::CheckNodeIds() {
  for (int NId = 0; NId < Graph->GetNodes(); NId++) {
    if (! Graph->IsNode(NId)) { return false; }
  }
  return true;
}

TNGraphMtx::TNGraphMtx(const PNGraph& GraphPt) : Graph() { 
  Graph = GraphPt;
  if (! CheckNodeIds()) {
    printf("  Renumbering nodes.\n");
    Graph = TSnap::ConvertGraph<PNGraph>(GraphPt, true);
  }
}

// Result = A * B(:,ColId)
void TNGraphMtx::PMultiply(const TFltVV& B, int ColId, TFltV& Result) const {
  const int RowN = GetRows();
  Assert(B.GetRows() >= RowN && Result.Len() >= RowN);
  const THash<TInt, TNGraph::TNode>& NodeH = Graph->NodeH;
  for (int j = 0; j < RowN; j++) {
    const TIntV& RowV = NodeH[j].OutNIdV;
    Result[j] = 0.0;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[j] += B(RowV[i], ColId);
    }
  }
}

// Result = A * Vec
void TNGraphMtx::PMultiply(const TFltV& Vec, TFltV& Result) const {
  const int RowN = GetRows();
  Assert(Vec.Len() >= RowN && Result.Len() >= RowN);
  const THash<TInt, TNGraph::TNode>& NodeH = Graph->NodeH;
  for (int j = 0; j < RowN; j++) {
    const TIntV& RowV = NodeH[j].OutNIdV;
    Result[j] = 0.0;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[j] += Vec[RowV[i]];
    }
  }
}

// Result = A' * B(:,ColId)
void TNGraphMtx::PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const {
  const int ColN = GetCols();
  Assert(B.GetRows() >= ColN && Result.Len() >= ColN);
  const THash<TInt, TNGraph::TNode>& NodeH = Graph->NodeH;
  for (int i = 0; i < ColN; i++) Result[i] = 0.0;
  for (int j = 0; j < ColN; j++) {
    const TIntV& RowV = NodeH[j].OutNIdV;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[RowV[i]] += B(j, ColId);
    }
  }
}

// Result = A' * Vec
void TNGraphMtx::PMultiplyT(const TFltV& Vec, TFltV& Result) const {
  const int RowN = GetRows();
  Assert(Vec.Len() >= RowN && Result.Len() >= RowN);
  const THash<TInt, TNGraph::TNode>& NodeH = Graph->NodeH;
  for (int i = 0; i < RowN; i++) Result[i] = 0.0;
  for (int j = 0; j < RowN; j++) {
    const TIntV& RowV = NodeH[j].OutNIdV;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[RowV[i]] += Vec[j];
    }
  }
}

/////////////////////////////////////////////////
// Undirected Graph Matrix -- sparse {0,1} row matrix 
bool TUNGraphMtx::CheckNodeIds() {
  for (int NId = 0; NId < Graph->GetNodes(); NId++) {
    if (! Graph->IsNode(NId)) { return false; }
  }
  return true;
}

TUNGraphMtx::TUNGraphMtx(const PUNGraph& GraphPt) : Graph() { 
  Graph = GraphPt;
  if (! CheckNodeIds()) {
    printf("  Renumbering %d nodes....", GraphPt->GetNodes());
    TExeTm ExeTm;
    Graph = TSnap::ConvertGraph<PUNGraph>(GraphPt, true);
    /*TIntSet NIdSet;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      NIdSet.AddKey(NI.GetId());
    }
    Graph = TUNGraph::New();  *Graph = *GraphPt; */
    printf("done [%s]\n", ExeTm.GetStr());
  }
}

// Result = A * B(:,ColId)
void TUNGraphMtx::PMultiply(const TFltVV& B, int ColId, TFltV& Result) const {
  const int RowN = GetRows();
  Assert(B.GetRows() >= RowN && Result.Len() >= RowN);
  const THash<TInt, TUNGraph::TNode>& NodeH = Graph->NodeH;
  for (int j = 0; j < RowN; j++) {
    const TIntV& RowV = NodeH[j].NIdV;
    Result[j] = 0.0;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[j] += B(RowV[i], ColId);
    }
  }
}

// Result = A * Vec
void TUNGraphMtx::PMultiply(const TFltV& Vec, TFltV& Result) const {
  const int RowN = GetRows();
  Assert(Vec.Len() >= RowN && Result.Len() >= RowN);
  const THash<TInt, TUNGraph::TNode>& NodeH = Graph->NodeH;
  for (int j = 0; j < RowN; j++) {
    const TIntV& RowV = NodeH[j].NIdV;
    Result[j] = 0.0;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[j] += Vec[RowV[i]];
    }
  }
}

// Result = A' * B(:,ColId)
void TUNGraphMtx::PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const {
  const int ColN = GetCols();
  Assert(B.GetRows() >= ColN && Result.Len() >= ColN);
  const THash<TInt, TUNGraph::TNode>& NodeH = Graph->NodeH;
  for (int i = 0; i < ColN; i++) Result[i] = 0.0;
  for (int j = 0; j < ColN; j++) {
    const TIntV& RowV = NodeH[j].NIdV;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[RowV[i]] += B(j, ColId);
    }
  }
}

// Result = A' * Vec
void TUNGraphMtx::PMultiplyT(const TFltV& Vec, TFltV& Result) const {
  const int RowN = GetRows();
  Assert(Vec.Len() >= RowN && Result.Len() >= RowN);
  const THash<TInt, TUNGraph::TNode>& NodeH = Graph->NodeH;
  for (int i = 0; i < RowN; i++) Result[i] = 0.0;
  for (int j = 0; j < RowN; j++) {
    const TIntV& RowV = NodeH[j].NIdV;
    for (int i = 0; i < RowV.Len(); i++) {
      Result[RowV[i]] += Vec[j];
    }
  }
}

/////////////////////////////////////////////////
// Graphs Singular Value Decomposition
namespace TSnap {

void SetAllInvertSign(TFltV& ValV, const double& Val) {
  for (int i = 0; i < ValV.Len(); i++) { 
    ValV[i] = -ValV[i]; 
  }
}
bool IsAllValVNeg(TFltV& ValV, const bool& InvertSign) {
  bool IsAllNeg=true;
  for (int i = 0; i < ValV.Len(); i++) {
    if (ValV[i]>0.0) { IsAllNeg=false; break; }
  }
  if (IsAllNeg && InvertSign) {
    for (int i = 0; i < ValV.Len(); i++) { 
      ValV[i] = -ValV[i]; }
  }
  return IsAllNeg;
}

void GetSngVals(const PNGraph& Graph, const int& SngVals, TFltV& SngValV) {
  const int Nodes = Graph->GetNodes();
  IAssert(SngVals > 0);
  if (Nodes < 100) {
    // perform full SVD
    TFltVV AdjMtx(Nodes+1, Nodes+1);
    TFltVV LSingV, RSingV;
    TIntH NodeIdH;
    // create adjecency matrix
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      NodeIdH.AddKey(NodeI.GetId()); }
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      const int NodeId = NodeIdH.GetKeyId(NodeI.GetId()) + 1;
      for (int e = 0; e < NodeI.GetOutDeg(); e++) {
        const int DstNId = NodeIdH.GetKeyId(NodeI.GetOutNId(e)) + 1;  // no self edges
        if (NodeId != DstNId) AdjMtx.At(NodeId, DstNId) = 1;
      }
    }
    try { // can fail to converge but results seem to be good
      TSvd::Svd1Based(AdjMtx, LSingV, SngValV, RSingV); }
    catch(...) {
      printf("\n***No SVD convergence: G(%d, %d)\n", Nodes, Graph->GetEdges()); }
  } else {
    // Lanczos
    TNGraphMtx GraphMtx(Graph);
    int CalcVals = int(2*SngVals);
    //if (CalcVals > Nodes) { CalcVals = int(2*Nodes); }
    //if (CalcVals > Nodes) { CalcVals = Nodes; }
    //while (SngValV.Len() < SngVals && CalcVals < 10*SngVals) {
    try {
      if (SngVals > 4) { 
        TSparseSVD::SimpleLanczosSVD(GraphMtx, 2*SngVals, SngValV, false); }
      else { TFltVV LSingV, RSingV;  // this is much more precise, but also much slower
        TSparseSVD::LanczosSVD(GraphMtx, SngVals, 3*SngVals, ssotFull, SngValV, LSingV, RSingV); }
    }
    catch(...) {
      printf("\n  ***EXCEPTION:  TRIED %d GOT %d values** \n", 2*SngVals, SngValV.Len()); }
    if (SngValV.Len() < SngVals) {
      printf("  ***TRIED %d GOT %d values** \n", CalcVals, SngValV.Len()); }
    //  CalcVals += SngVals;
    //}
  }
  SngValV.Sort(false);
  //if (SngValV.Len() > SngVals) {
  //  SngValV.Del(SngVals, SngValV.Len()-1); }
  //else {
  //  while (SngValV.Len() < SngVals) SngValV.Add(1e-6); }
  //IAssert(SngValV.Len() == SngVals);
}

void GetSngVec(const PNGraph& Graph, TFltV& LeftSV, TFltV& RightSV) {
  const int Nodes = Graph->GetNodes();
  TFltVV LSingV, RSingV;
  TFltV SngValV;
  if (Nodes < 500) {
    // perform full SVD
    TFltVV AdjMtx(Nodes+1, Nodes+1);
    TIntH NodeIdH;
    // create adjecency matrix
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      NodeIdH.AddKey(NodeI.GetId()); }
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      const int NodeId = NodeIdH.GetKeyId(NodeI.GetId()) + 1;
      for (int e = 0; e < NodeI.GetOutDeg(); e++) {
        const int DstNId = NodeIdH.GetKeyId(NodeI.GetOutNId(e)) + 1;  // no self edges
        if (NodeId != DstNId) AdjMtx.At(NodeId, DstNId) = 1;
      }
    }
    try { // can fail to converge but results seem to be good
      TSvd::Svd1Based(AdjMtx, LSingV, SngValV, RSingV); }
    catch(...) {
      printf("\n***No SVD convergence: G(%d, %d)\n", Nodes, Graph->GetEdges()); }
  } else { // Lanczos
    TNGraphMtx GraphMtx(Graph);
    TSparseSVD::LanczosSVD(GraphMtx, 1, 8, ssotFull, SngValV, LSingV, RSingV);
  }
  TFlt MxSngVal = TFlt::Mn;
  int ValN = 0;
  for (int i = 0; i < SngValV.Len(); i++) {
    if (MxSngVal < SngValV[i]) { MxSngVal = SngValV[i]; ValN = i; } }
  LSingV.GetCol(ValN, LeftSV);
  RSingV.GetCol(ValN, RightSV);
  IsAllValVNeg(LeftSV, true);
  IsAllValVNeg(RightSV, true);
}

void GetSngVec(const PNGraph& Graph, const int& SngVecs, TFltV& SngValV, TVec<TFltV>& LeftSV, TVec<TFltV>& RightSV) {
  const int Nodes = Graph->GetNodes();
  SngValV.Clr();
  LeftSV.Clr();
  RightSV.Clr();
  TFltVV LSingV, RSingV;
  if (Nodes < 100) {
    // perform full SVD
    TFltVV AdjMtx(Nodes+1, Nodes+1);
    TIntH NodeIdH;
    // create adjecency matrix (1-based)
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      NodeIdH.AddKey(NodeI.GetId()); }
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      const int NodeId = NodeIdH.GetKeyId(NodeI.GetId())+1;
      for (int e = 0; e < NodeI.GetOutDeg(); e++) {
        const int DstNId = NodeIdH.GetKeyId(NodeI.GetOutNId(e))+1;  // no self edges
        if (NodeId != DstNId) AdjMtx.At(NodeId, DstNId) = 1;
      }
    }
    try { // can fail to converge but results seem to be good
      TSvd::Svd1Based(AdjMtx, LSingV, SngValV, RSingV);
    } catch(...) {
      printf("\n***No SVD convergence: G(%d, %d)\n", Nodes, Graph->GetEdges()); 
    }
  } else { // Lanczos
    TNGraphMtx GraphMtx(Graph);
    TSparseSVD::LanczosSVD(GraphMtx, SngVecs, 2*SngVecs, ssotFull, SngValV, LSingV, RSingV);
    //TGAlg::SaveFullMtx(Graph, "adj_mtx.txt");
    //TLAMisc::DumpTFltVVMjrSubMtrx(LSingV, LSingV.GetRows(), LSingV.GetCols(), "LSingV2.txt"); // save MTX
  }
  TFltIntPrV SngValIdV;
  for (int i = 0; i < SngValV.Len(); i++) {
    SngValIdV.Add(TFltIntPr(SngValV[i], i)); 
  }
  SngValIdV.Sort(false);
  SngValV.Sort(false);
  for (int v = 0; v < SngValIdV.Len(); v++) { 
    LeftSV.Add();
    LSingV.GetCol(SngValIdV[v].Val2, LeftSV.Last());
    RightSV.Add();
    RSingV.GetCol(SngValIdV[v].Val2, RightSV.Last());
  }
  IsAllValVNeg(LeftSV[0], true);
  IsAllValVNeg(RightSV[0], true);
}

void GetEigVals(const PUNGraph& Graph, const int& EigVals, TFltV& EigValV) {
  // Lanczos
  TUNGraphMtx GraphMtx(Graph);
  //const int Nodes = Graph->GetNodes();
  //int CalcVals = int(2*EigVals);
  //if (CalcVals > Nodes) { CalcVals = Nodes; }
  //while (EigValV.Len() < EigVals && CalcVals < 3*EigVals) {
  try {
    if (EigVals > 4) { 
      TSparseSVD::SimpleLanczos(GraphMtx, 2*EigVals, EigValV, false); }
    else { TFltVV EigVecVV; // this is much more precise, but also much slower
      TSparseSVD::Lanczos(GraphMtx, EigVals, 3*EigVals, ssotFull, EigValV, EigVecVV, false); }
  }
  catch(...) {
    printf("\n  ***EXCEPTION:  TRIED %d GOT %d values** \n", 2*EigVals, EigValV.Len()); }
  if (EigValV.Len() < EigVals) {
    printf("  ***TRIED %d GOT %d values** \n", 2*EigVals, EigValV.Len()); }
  //  CalcVals += EigVals;
  //}
  EigValV.Sort(false);
  /*if (EigValV.Len() > EigVals) {
    EigValV.Del(EigVals, EigValV.Len()-1); }
  else {
    while (EigValV.Len() < EigVals) EigValV.Add(1e-6); 
  }
  IAssert(EigValV.Len() == EigVals);*/
}

void GetEigVec(const PUNGraph& Graph, TFltV& EigVecV) {
  TUNGraphMtx GraphMtx(Graph);
  TFltV EigValV;
  TFltVV EigVecVV;
  TSparseSVD::Lanczos(GraphMtx, 1, 8, ssotFull, EigValV, EigVecVV, false);
  EigVecVV.GetCol(0, EigVecV); // vector components are not sorted!!!
  IsAllValVNeg(EigVecV, true);
}

// to get first few eigenvectors
void GetEigVec(const PUNGraph& Graph, const int& EigVecs, TFltV& EigValV, TVec<TFltV>& EigVecV) {
  const int Nodes = Graph->GetNodes();
  // Lanczos
  TUNGraphMtx GraphMtx(Graph);
  int CalcVals = int(2*EigVecs);
  if (CalcVals > Nodes) { CalcVals = Nodes; }
  TFltVV EigVecVV;
  //while (EigValV.Len() < EigVecs && CalcVals < 10*EigVecs) {
  try {
    TSparseSVD::Lanczos(GraphMtx, EigVecs, 2*EigVecs, ssotFull, EigValV, EigVecVV, false); }
  catch(...) {
    printf("\n  ***EXCEPTION:  TRIED %d GOT %d values** \n", CalcVals, EigValV.Len()); }
  if (EigValV.Len() < EigVecs) {
    printf("  ***TRIED %d GOT %d values** \n", CalcVals, EigValV.Len()); }
  //  CalcVals += EigVecs;
  //}
  TFltIntPrV EigValIdV;
  for (int i = 0; i < EigValV.Len(); i++) {
    EigValIdV.Add(TFltIntPr(EigValV[i], i)); 
  }
  EigValIdV.Sort(false);
  EigValV.Sort(false);
  for (int v = 0; v < EigValIdV.Len(); v++) { // vector components are not sorted!!!
    EigVecV.Add();
    EigVecVV.GetCol(EigValIdV[v].Val2, EigVecV.Last());
  }
  IsAllValVNeg(EigVecV[0], true);
}

// Inverse participation ratio: normalize EigVec to have L2=1 and then I=sum_k EigVec[i]^4
// see Spectra of "real-world" graphs: Beyond the semicircle law by Farkas, Derenyi, Barabasi and Vicsek
void GetInvParticipRat(const PUNGraph& Graph, int MaxEigVecs, int TimeLimit, TFltPrV& EigValIprV) {
  TUNGraphMtx GraphMtx(Graph);
  TFltVV EigVecVV;
  TFltV EigValV;
  TExeTm ExeTm;
  if (MaxEigVecs<=1) { MaxEigVecs=1000; }
  int EigVecs = TMath::Mn(Graph->GetNodes(), MaxEigVecs);
  printf("start %d vecs...", EigVecs);
  try {
    TSparseSVD::Lanczos2(GraphMtx, EigVecs, TimeLimit, ssotFull, EigValV, EigVecVV, false);
  } catch(...) {
    printf("\n  ***EXCEPTION:  TRIED %d GOT %d values** \n", EigVecs, EigValV.Len()); }
  printf("  ***TRIED %d GOT %d values in %s\n", EigVecs, EigValV.Len(), ExeTm.GetStr());
  TFltV EigVec;
  EigValIprV.Clr();
  if (EigValV.Empty()) { return; }
  for (int v = 0; v < EigVecVV.GetCols(); v++) {
    EigVecVV.GetCol(v, EigVec);
    EigValIprV.Add(TFltPr(EigValV[v], TSnapDetail::GetInvParticipRat(EigVec)));
  }
  EigValIprV.Sort();
}

namespace TSnapDetail {
double GetInvParticipRat(const TFltV& EigVec) {
  double Sum2=0.0, Sum4=0.0;
  for (int i = 0; i < EigVec.Len(); i++) {
    Sum2 += EigVec[i]*EigVec[i];
    Sum4 += pow(EigVec[i].Val, 4.0);
  }
  return Sum4/(Sum2*Sum2);
}
} // namespace TSnapDetail

}; // namespace TSnap
