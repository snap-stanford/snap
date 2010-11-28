#ifndef Snap_Cliques
#define Snap_Cliques

#include "Snap.h"

/////////////////////////////////////////////////
// Clique Percolation Method for Overlapping community detection
class TCliqueOverlap {
private: //Recursion variables only
	PUNGraph m_G;
	TIntV m_Q;
	TVec<TIntV> *m_maxCliques;
	int m_minMaxCliqueSize;
private:
	void GetNbhs(int NId, THashSet<TInt>& Nbhs) const;
	int GetNodeIdWithMaxDeg(const THashSet<TInt>& Set) const;
	int MaxNbhsInCANDNodeId(const THashSet<TInt>& SUBG, const THashSet<TInt>& CAND) const;
private:
	void Expand(const THashSet<TInt>& SUBG, THashSet<TInt>& CAND);
public:
	static void GetRelativeComplement(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& Complement);
	static void GetIntersection(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& C);
	static int Intersection(const THashSet<TInt>& A, const THashSet<TInt>& B);
  static void CalculateOverlapMtx(const TVec<TIntV>& MaxCliques, int MinNodeOverlap, TVec<TIntV>& OverlapMtx);
  static PUNGraph CalculateOverlapMtx(const TVec<TIntV>& MaxCliques, int MinNodeOverlap);
	static void GetOverlapCliques(const TVec<TIntV>& OverlapMtx, int MinNodeOverlap, TVec<TIntV>& CliqueIdVV);
	static void GetOverlapCliques(const TVec<TIntV>& OverlapMtx, const TVec<TIntV>& MaxCliques, double MinOverlapFrac, TVec<TIntV>& CliqueIdVV);
public:
  TCliqueOverlap() : m_G(), m_Q(), m_maxCliques(NULL), m_minMaxCliqueSize(3) { }
	void GetMaximalCliques(const PUNGraph& G, int MinMaxCliqueSize, TVec<TIntV>& MaxCliques);
  /// Enumerate maximal cliques of the network on more than MinMaxCliqueSize nodes
  static void GetMaxCliques(const PUNGraph& G, int MinMaxCliqueSize, TVec<TIntV>& MaxCliques);
  /// Clique Percolation method communities
  static void GetCPMCommunities(const PUNGraph& G, int MinMaxCliqueSize, TVec<TIntV>& Communities);
};

#endif
