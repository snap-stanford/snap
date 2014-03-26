namespace TSnap {

void GetMergeSortedV(TIntV& NeighbourV, TNGraph::TNodeI NI) {
  int ind, j, k;
  ind = j = k = 0;
  while (j < NI.GetInDeg() && k < NI.GetOutDeg()) {
    int v1 = NI.GetInNId(j);
    int v2 = NI.GetOutNId(k);
    if (v1 <= v2) {
      if ((ind == 0) || (NeighbourV[ind-1] != v1)) {
        NeighbourV.Add(v1);
        ind += 1;
      }
      j += 1;
    }
    else {
      if ((ind == 0) || (NeighbourV[ind-1] != v2)) {
        NeighbourV.Add(v2);
        ind += 1;
      }
      k += 1;
    }
  }
  while (j < NI.GetInDeg()) {
    int v = NI.GetInNId(j);
    if ((ind == 0) || (NeighbourV[ind-1] != v)) {
        NeighbourV.Add(v);
        ind += 1;
    }
    j += 1;
  }
  while (k < NI.GetOutDeg()) {
    int v = NI.GetOutNId(k);
    if ((ind == 0) || (NeighbourV[ind-1] != v)) {
        NeighbourV.Add(v);
        ind += 1;
    }
    k += 1;
  }
}

int GetCommon(TIntV& A, TIntV& B) {
  int i = 0, j = 0;
  int ret = 0;
  while (i < A.Len()) {
    while (j < B.Len() && B[j] < A[i]) {
      j += 1;
    }
    if (j == B.Len()) {
      break;
    }
    if (B[j] == A[i]) ret += 1;
    i += 1;
  }
  return ret;
}

}
