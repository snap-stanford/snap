#include "stdafx.h"
#include "cliques.h"

/////////////////////////////////////////////////
// TCommunity implementation
void TCliqueOverlap::GetRelativeComplement(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& Complement) {
  for (THashSet<TInt>::TIter it=A.BegI(); it<A.EndI(); it++) {
		const int nId = it.GetKey();
		if (!B.IsKey(nId)) Complement.AddKey(nId);
	}
}

void TCliqueOverlap::GetIntersection(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& C) {
	if (A.Len() < B.Len()) {
		for (THashSetKeyI<TInt> it=A.BegI(); it<A.EndI(); it++) 
			if (B.IsKey(it.GetKey())) C.AddKey(it.GetKey());
	} else {
		for (THashSetKeyI<TInt> it=B.BegI(); it<B.EndI(); it++) 
			if (A.IsKey(it.GetKey())) C.AddKey(it.GetKey());
	}
}

int TCliqueOverlap::Intersection(const THashSet<TInt>& A, const THashSet<TInt>& B) {
	int n = 0;
	if (A.Len() < B.Len()) {
		for (THashSetKeyI<TInt> it=A.BegI(); it<A.EndI(); it++) 
			if (B.IsKey(it.GetKey())) n++;
	} else {
		for (THashSetKeyI<TInt> it=B.BegI(); it<B.EndI(); it++) 
			if (A.IsKey(it.GetKey())) n++;
	}
	return n;
}

void TCliqueOverlap::GetNbhs(int NId, THashSet<TInt>& Nbhs) const{
	TUNGraph::TNodeI node = m_G->GetNI(NId);
	int deg = node.GetDeg();
	for (int i=0; i<deg; i++) Nbhs.AddKey(node.GetNbhNId(i));
}

int TCliqueOverlap::GetNodeIdWithMaxDeg(const THashSet<TInt>& Set) const{
	int id = -1;
	int maxDeg = -1;
	//
	for (THashSetKeyI<TInt> it=Set.BegI(); it<Set.EndI(); it++) {
		int nId = it.GetKey();
		int deg = m_G->GetNI(nId).GetDeg();
		if (maxDeg < deg) { maxDeg=deg; id=nId; }
	}
	return id;
}

int TCliqueOverlap::MaxNbhsInCANDNodeId(const THashSet<TInt>& SUBG, const THashSet<TInt>& CAND) const{
	int id = -1;
	int maxIntersection = -1;
	//
	for (THashSetKeyI<TInt> it=SUBG.BegI(); it<SUBG.EndI(); it++) {
		int nId = it.GetKey();
		TUNGraph::TNodeI nIt = m_G->GetNI(nId);
		int deg = nIt.GetDeg();
		//
		int curIntersection = 0;
		for (int i=0; i<deg; i++) {
			int nbhId = nIt.GetNbhNId(i);
			if (CAND.IsKey(nbhId)) curIntersection++;
		}
		//
		if (maxIntersection < curIntersection) { maxIntersection=curIntersection; id=nId; }
	}
	return id;
}

void TCliqueOverlap::Expand(const THashSet<TInt>& SUBG, THashSet<TInt>& CAND) {
	if (SUBG.Len()==0) { if (m_Q.Len() >= m_minMaxCliqueSize) { m_Q.Pack(); m_maxCliques->Add(m_Q); } return; }
	if (CAND.Len()==0) return;
	//Get u that maximaze CAND intersection with neighbours of vertex u
	int u = MaxNbhsInCANDNodeId(SUBG, CAND);
	//Get neighbours of node u
	THashSet<TInt> nbhsU;
	GetNbhs(u, nbhsU);
	//Get relative complement of nbhsU in CAND
	THashSet<TInt> EXT;
	GetRelativeComplement(CAND, nbhsU, EXT);
	while(EXT.Len() != 0) {
		int q = GetNodeIdWithMaxDeg(EXT);
		//
		m_Q.Add(q);
		//
		THashSet<TInt> nbhsQ;
		GetNbhs(q, nbhsQ);
		//
		THashSet<TInt> SUBGq;
		GetIntersection(SUBG, nbhsQ, SUBGq);
		//
		THashSet<TInt> CANDq;
		GetIntersection(CAND, nbhsQ, CANDq);
		//
		Expand(SUBGq, CANDq);
		//
 		CAND.DelKey(q);
		m_Q.DelLast();
		//
		EXT.Clr();
		GetRelativeComplement(CAND, nbhsU, EXT);
	}
}

void TCliqueOverlap::GetMaximalCliques(const PUNGraph& G, int MinMaxCliqueSize, TVec<TIntV>& MaxCliques) {
	if (G->GetNodes() == 0) return;
	//
	m_G = G;
	m_minMaxCliqueSize = MinMaxCliqueSize;
	m_maxCliques =& MaxCliques;
	m_Q.Clr();
	//
	THashSet<TInt> SUBG;
	THashSet<TInt> CAND;
	for (TUNGraph::TNodeI NI=m_G->BegNI(); NI<m_G->EndNI(); NI++) {
		TInt nId = NI.GetId();
		SUBG.AddKey(nId);
		CAND.AddKey(nId);
	}
	//
	Expand(SUBG, CAND);
}

void TCliqueOverlap::CalculateOverlapMtx(const TVec<TIntV>& MaxCliques, int MinNodeOverlap, TVec<TIntV>& OverlapMtx) {
	OverlapMtx.Clr();
	//
	int n = MaxCliques.Len();
	//Convert max cliques to HashSets
		TVec<THashSet<TInt> > cliques;
	for (int i=0; i<n; i++) {
		const int len = MaxCliques[i].Len();
    cliques.Add();
    if (len < MinNodeOverlap) { continue; }
		THashSet<TInt>& set = cliques.Last();  set.Gen(len);
    for (int j=0; j<len; j++) { set.AddKey(MaxCliques[i][j]); }
	}
	//Init clique clique overlap matrix
	n = cliques.Len();
	OverlapMtx.Gen(n);
	for (int i=0; i<n; i++) OverlapMtx[i].Gen(n);
	//Calculate clique clique overlap matrix
  for (int i=0; i<n; i++) {
    OverlapMtx[i][i] = cliques[i].Len();
    for (int j=i+1; j<n; j++) {
      OverlapMtx[i][j] = Intersection(cliques[i], cliques[j]); }
  }
}

PUNGraph TCliqueOverlap::CalculateOverlapMtx(const TVec<TIntV>& MaxCliques, int MinNodeOverlap) {
	const int n = MaxCliques.Len();
  //Convert max cliques to HashSets
	TVec<THashSet<TInt> > cliques;
	for (int i=0; i<n; i++) {
		const int len = MaxCliques[i].Len();
    cliques.Add();
    if (len < MinNodeOverlap) { continue; }
		THashSet<TInt>& set = cliques.Last();  set.Gen(len);
    for (int j=0; j<len; j++) { set.AddKey(MaxCliques[i][j]); }
	}
	//Init clique clique overlap matrix
	PUNGraph OverlapMtx = TUNGraph::New();
  for (int i=0; i < n; i++) {
    OverlapMtx->AddNode(i); }
	//Calculate clique clique overlap matrix
  for (int i=0; i<n; i++) {
    for (int j=i+1; j<n; j++) {
      if (Intersection(cliques[i], cliques[j]) >= MinNodeOverlap) {
        OverlapMtx->AddEdge(i,j); }
    }
  }
  return OverlapMtx;
}

void TCliqueOverlap::GetOverlapCliques(const TVec<TIntV>& OverlapMtx, int MinNodeOverlap, TVec<TIntV>& CliqueIdVV) {
	int n = OverlapMtx.Len();
	for (int i=0; i<n; i++) {
		bool isCommunity = false;
		for (int j=i+1; j<n; j++) {
			if (OverlapMtx[i][j] >= MinNodeOverlap) {
				if (! isCommunity) {
					TIntV v; v.Add(i);
					CliqueIdVV.Add(v);
					isCommunity=true;
				}
				CliqueIdVV.Last().Add(j);
			}
		}
	}
}

void TCliqueOverlap::GetOverlapCliques(const TVec<TIntV>& OverlapMtx, const TVec<TIntV>& MaxCliques, double MinOverlapFrac, TVec<TIntV>& CliqueIdVV) {
	int n = OverlapMtx.Len();
	for(int i=0; i<n; i++){
		bool isCommunity = false;
		int size1 = MaxCliques[i].Len();
		for(int j=i+1; j<n; j++){
			int size2 = MaxCliques[j].Len();
			double ratio = OverlapMtx[i][j];
			if(size1 < size2) ratio /= size1;
			else ratio /= size2;
			if(ratio >= MinOverlapFrac){
				if(!isCommunity){
					TIntV v; v.Add(i);
					CliqueIdVV.Add(v);
					isCommunity=true;
				}
				CliqueIdVV.Last().Add(j);
			}
		}
	}
}

/// Enumerate maximal cliques of the network on more than MinMaxCliqueSize nodes
void TCliqueOverlap::GetMaxCliques(const PUNGraph& G, int MinMaxCliqueSize, TVec<TIntV>& MaxCliques) {
  TCliqueOverlap CO;
  MaxCliques.Clr(false);
  CO.GetMaximalCliques(G, MinMaxCliqueSize, MaxCliques);
}

/// Clique Percolation method communities
void TCliqueOverlap::GetCPMCommunities(const PUNGraph& G, int MinMaxCliqueSize, TVec<TIntV>& NIdCmtyVV) {
  printf("Clique Percolation Method\n");
  TExeTm ExeTm;
  TVec<TIntV> MaxCliques;
  TCliqueOverlap::GetMaxCliques(G, MinMaxCliqueSize, MaxCliques);
  printf("...%d cliques found\n");
  // get clique overlap matrix (graph)
  PUNGraph OverlapGraph = TCliqueOverlap::CalculateOverlapMtx(MaxCliques, MinMaxCliqueSize-1);
  printf("...overlap matrix (%d, %d)\n", G->GetNodes(), G->GetEdges());
  // connected components are communities
  TCnComV CnComV;
  TSnap::GetWccs(OverlapGraph, CnComV);
  NIdCmtyVV.Clr(false);
  TIntSet CmtySet;
  for (int c = 0; c < CnComV.Len(); c++) {
    CmtySet.Clr(false);
    for (int i = 0; i <CnComV[c].Len(); i++) {
      const TIntV& CliqueNIdV = MaxCliques[CnComV[c][i]];
      CmtySet.AddKeyV(CliqueNIdV);
    }
    NIdCmtyVV.Add();
    CmtySet.GetKeyV(NIdCmtyVV.Last());
    NIdCmtyVV.Last().Sort();
  }
  printf("done [%s].\n", ExeTm.GetStr());
}
