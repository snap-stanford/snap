#include "stdafx.h"
#include "graphcounter.h"

//Number of directed connected 5-node subgraphs = 9364

/////////////////////////////////////////////////
// Directed 3 graph implementation
int TD3Graph::m_numOfGraphs = 13;
int TD3Graph::m_graphIds[] = {6,12,14,36,38,46,78,102,140,164,166,174,238};

int TD3Graph::getId(const PNGraph &G, const TIntV &sg) {
	int id=0;
	//Node 0
	if(TGraphEnumUtils::IsEdge(G, sg[0], sg[1])) id+=2;
	if(TGraphEnumUtils::IsEdge(G, sg[0], sg[2])) id+=4;
	//Node 1
	if(TGraphEnumUtils::IsEdge(G, sg[1], sg[0])) id+=8;
	if(TGraphEnumUtils::IsEdge(G, sg[1], sg[2])) id+=32;
	//Node 2
	if(TGraphEnumUtils::IsEdge(G, sg[2], sg[0])) id+=64;
	if(TGraphEnumUtils::IsEdge(G, sg[2], sg[1])) id+=128;
	//
	return id;
}
/////////////////////////////////////////////////
// Directed 4 graph implementation
int TD4Graph::m_numOfGraphs = 199;
int TD4Graph::m_graphIds[] = {14, 28, 30, 74, 76, 78, 90, 92, 94, 204, 206, 222, 280, 282, 286, 
								328, 330, 332, 334, 344, 346, 348, 350, 390, 392, 394, 396, 398, 
								404, 406, 408, 410, 412, 414, 454, 456, 458, 460, 462, 468, 470, 
								472, 474, 476, 478, 856, 858, 862, 904, 906, 908, 910, 922, 924, 
								926, 972, 974, 990, 2184, 2186, 2190, 2202, 2204, 2206, 2252, 2254, 
								2270, 2458, 2462, 2506, 2510, 2524, 2526, 3038, 4370, 4374, 4382, 
								4418, 4420, 4422, 4424, 4426, 4428, 4430, 4434, 4436, 4438, 4440, 
								4442, 4444, 4446, 4546, 4548, 4550, 4556, 4558, 4562, 4564, 4566, 
								4572, 4574, 4678, 4682, 4686, 4692, 4694, 4698, 4700, 4702, 4740, 
								4742, 4748, 4750, 4758, 4764, 4766, 4812, 4814, 4830, 4946, 4950, 
								4952, 4954, 4958, 4994, 4998, 5002, 5004, 5006, 5010, 5012, 5014, 
								5016, 5018, 5020, 5022, 5058, 5062, 5064, 5066, 5068, 5070, 5074, 
								5076, 5078, 5080, 5082, 5084, 5086, 6342, 6348, 6350, 6356, 6358, 
								6364, 6366, 6550, 6552, 6554, 6558, 6598, 6602, 6604, 6606, 6614, 
								6616, 6618, 6620, 6622, 6854, 6858, 6862, 6870, 6874, 6876, 6878, 
								7126, 7128, 7130, 7134, 13142, 13146, 13148, 13150, 13260, 13262, 
								13278, 14678, 14686, 14790, 14798, 14810, 14812, 14814, 15258, 
								15262, 15310, 15326, 31710 };

int TD4Graph::getId(const PNGraph &G, const TIntV &sg) {
	int id=0;
	//Node 0
	if(TGraphEnumUtils::IsEdge(G, sg[0], sg[1])) id+=2;
	if(TGraphEnumUtils::IsEdge(G, sg[0], sg[2])) id+=4;
	if(TGraphEnumUtils::IsEdge(G, sg[0], sg[3])) id+=8;
	//Node 1
	if(TGraphEnumUtils::IsEdge(G, sg[1], sg[0])) id+=16;
	if(TGraphEnumUtils::IsEdge(G, sg[1], sg[2])) id+=64;
	if(TGraphEnumUtils::IsEdge(G, sg[1], sg[3])) id+=128;
	//Node 2
	if(TGraphEnumUtils::IsEdge(G, sg[2], sg[0])) id+=256;
	if(TGraphEnumUtils::IsEdge(G, sg[2], sg[1])) id+=512;
	if(TGraphEnumUtils::IsEdge(G, sg[2], sg[3])) id+=2048;
	//Node 3
	if(TGraphEnumUtils::IsEdge(G, sg[3], sg[0])) id+=4096;
	if(TGraphEnumUtils::IsEdge(G, sg[3], sg[1])) id+=8192;
	if(TGraphEnumUtils::IsEdge(G, sg[3], sg[2])) id+=16384;
	//
	return id;
}

/////////////////////////////////////////////////
// Directed 3 or 4 graph counter implementation
TD34GraphCounter::TD34GraphCounter(int GraphSz) {
	IAssert(GraphSz==3 || GraphSz==4);
	//
	m_subGraphSize = GraphSz;
	//
	int numOfGraphs;
	if(GraphSz==3) numOfGraphs = TD3Graph::m_numOfGraphs;
	else if(GraphSz==4) numOfGraphs = TD4Graph::m_numOfGraphs;
	//
	for(int i=0; i<numOfGraphs; i++) {
		int graphId;
		if(GraphSz==3) graphId = TD3Graph::m_graphIds[i];
		else if(GraphSz==4) graphId = TD4Graph::m_graphIds[i];
		//
		TVec<PNGraph> isoG;
		TGraphEnumUtils::GetIsoGraphs(graphId, GraphSz, isoG);
		//
		TVec<uint64> graphIds(isoG.Len());
		uint64 minGraphId = TGraphEnumUtils::GetMinAndGraphIds(isoG, graphIds);
		for(int j=0; j<graphIds.Len(); j++)
			m_graphMaps.AddDat((int)graphIds[j], (int)minGraphId);
		//
		m_graphCounters.AddDat((int)minGraphId, 0);
	}
}
void TD34GraphCounter::operator()(const PNGraph &G, const TIntV &sg) {
	int graphId;
	if(m_subGraphSize==3) graphId = TD3Graph::getId(G, sg);
	else if(m_subGraphSize==4) graphId = TD4Graph::getId(G, sg);
	//
	if(!m_graphMaps.IsKey(graphId)) { printf("This graph does not exist: %d\n", graphId); getchar(); return; }
	int minGraphId = m_graphMaps.GetDat(graphId);
	//
	m_graphCounters.GetDat(minGraphId)++;
}

PNGraph TD34GraphCounter::GetGraph(const int& GraphId) const {
  PNGraph G = TNGraph::New();
  TGraphEnumUtils::GetGraph(GraphId, m_subGraphSize, G);
  return G;
}

/////////////////////////////////////////////////
// Directed graph counter implementation
void TDGraphCounter::operator()(const PNGraph &G, const TIntV &sg) {
	uint64 graphId = TGraphEnumUtils::GraphId(G, sg);
	//
	if(m_graphMaps.IsKey(graphId)) {
		TUInt64 minGraphId = m_graphMaps.GetDat(graphId);
		m_graphCounters.GetDat(minGraphId)++;
	}else{
		TVec<PNGraph> isoG;
		TGraphEnumUtils::GetIsoGraphs(graphId, sg.Len(), isoG);
		//
		TVec<uint64> graphIds(isoG.Len());
		uint64 minGraphId = TGraphEnumUtils::GetMinAndGraphIds(isoG, graphIds);
		for(int j=0; j<graphIds.Len(); j++)
			m_graphMaps.AddDat(graphIds[j], minGraphId);
		//
		m_graphCounters.AddDat(minGraphId, 1);
	}
}

/////////////////////////////////////////////////
// Directed ghash graph counter implementation
void TDGHashGraphCounter::operator()(const PNGraph &G, const TIntV &sg) {
	PNGraph indG = TNGraph::New();
	TGraphEnumUtils::GetIndGraph(G, sg, indG);
	//
	if(m_graphs.IsKey(indG))
		m_graphs.GetDat(indG)++;
	else m_graphs.AddDat(indG, 1);
}

/////////////////////////////////////////////////
// TGraphEnumUtils implementation
void TGraphEnumUtils::GetNormalizedMap(const PNGraph &G, THash<TInt,TInt> &map) {
	int nId=0;
	for(TNGraph::TNodeI it=G->BegNI(); it<G->EndNI(); it++) {
		//printf("%d -> %d\n", it.GetId(), nId);
		map.AddDat(it.GetId(), nId);
		nId++;
	}
}

void TGraphEnumUtils::GetPermutations(TIntV &v, int start, TVec<TIntV> &perms) {
	int n = v.Len();
	if (start == n-1) perms.Add(v);
	else {
		for (int i = start; i < n; i++) {
			int tmp = v[i];
			v[i] = v[start];
			v[start] = tmp;
			GetPermutations(v, start+1, perms);
			//
			v[start] = v[i];
			v[i] = tmp;
		}
	}
}

void TGraphEnumUtils::GetNormalizedGraph(const PNGraph &G, PNGraph &nG) {
	//Get bijective map from original node ids to normalized node ids(0,1,2,...)
	THash<TInt,TInt> map;
	GetNormalizedMap(G, map);
	//Add nodes
	for(int i=0; i<G->GetNodes(); i++) nG->AddNode(i);
	//Add edges
	for(TNGraph::TEdgeI eIt=G->BegEI(); eIt<G->EndEI(); eIt++) {
		int srcId = eIt.GetSrcNId();
		int dstId = eIt.GetDstNId();
		//
		int mSrcId = map.GetDat(srcId);
		int mDstId = map.GetDat(dstId);
		//
		nG->AddEdge(mSrcId, mDstId);
	}
}

void TGraphEnumUtils::GetEdges(uint64 graphId, int nodes, TVec<TPair<int,int> > &edges) {
	for(int row=0; row<nodes; row++) {
		for(int col=0; col<nodes; col++) {
			int n = row*nodes+col;
			//
			uint64 bits = graphId >> n;
			uint64 mask = 1;
			if((bits & mask)==1) edges.Add(TPair<int,int>(row, col));
		}
	}
}

void TGraphEnumUtils::GetIsoGraphs(uint64 graphId, int nodes, TVec<PNGraph> &isoG) {
	TIntV v(nodes); for(int i=0; i<nodes; i++) v[i]=i;
	TVec<TIntV> perms; GetPermutations(v, 0, perms);
	isoG.Gen(perms.Len());
	//
	TVec<TPair<int,int> > edges;
	GetEdges(graphId, nodes, edges);
	//
	for(int i=0; i<perms.Len(); i++) {
		isoG[i] = TNGraph::New();
		//Add nodes
		for(int j=0; j<nodes; j++) isoG[i]->AddNode(j);
		//Add edges
		for(int j=0; j<edges.Len(); j++) {
			int srcId = edges[j].Val1;
			int dstId = edges[j].Val2;
			//
			int pSrcId = perms[i][srcId];
			int pDstId = perms[i][dstId];
			//
			isoG[i]->AddEdge(pSrcId, pDstId);
		}
	}
}

void TGraphEnumUtils::GetIsoGraphs(const PNGraph &G, TVec<PNGraph> &isoG) {
	int nodes = G->GetNodes();
	//
	TIntV v(nodes); for(int i=0; i<nodes; i++) v[i]=i;
	TVec<TIntV> perms; GetPermutations(v, 0, perms);
	isoG.Gen(perms.Len());
	//
	for(int i=0; i<perms.Len(); i++) {
		isoG[i] = TNGraph::New();
		//Add nodes
		for(int j=0; j<nodes; j++) isoG[i]->AddNode(j);
		//Add edges
		for(TNGraph::TEdgeI eIt=G->BegEI(); eIt<G->EndEI(); eIt++) {
			int srcId = eIt.GetSrcNId();
			int dstId = eIt.GetDstNId();
			//
			int pSrcId = perms[i][srcId];
			int pDstId = perms[i][dstId];
			//
			isoG[i]->AddEdge(pSrcId, pDstId);
		}
	}
}

void TGraphEnumUtils::GetIndGraph(const PNGraph &G, const TIntV &sg, PNGraph &indG) {
	//Add nodes
	for(int i=0; i<sg.Len(); i++) indG->AddNode(sg[i]);
	//Add edges
	for(int i=0; i<sg.Len(); i++) {
		int nId = sg[i];
		TNGraph::TNodeI nIt = G->GetNI(nId);
		//
		int deg = nIt.GetOutDeg();
		for(int j=0; j<deg; j++) {
			int dstId = nIt.GetNbhNId(j);
			if(nId == dstId) continue;
			//
			if(indG->IsNode(dstId)) indG->AddEdge(nId, dstId);
		}
	}
}

void TGraphEnumUtils::GetGraph(uint64 graphId, int nodes, PNGraph &G) {
  G->Clr();
	//Add nodes;
	for(int i=0; i<nodes; i++) G->AddNode(i);
	//Add edges
	for(int row=0; row<nodes; row++) {
		for(int col=0; col<nodes; col++) {
			int n = row*nodes+col;
			//
			uint64 bits = graphId >> n;
			uint64 mask = 1;
			if((bits & mask)==1) G->AddEdge(row, col);
		}
	}
}

uint64 TGraphEnumUtils::GraphId(const PNGraph &G) {
	int nodes = G->GetNodes();
	uint64 id=0;
	for(TNGraph::TEdgeI it=G->BegEI(); it<G->EndEI(); it++) {
		int srcId = it.GetSrcNId();
		int dstId = it.GetDstNId();
		//
		id += TMath::Pow2(srcId*nodes + dstId);
	}
	//
	return id;
}

uint64 TGraphEnumUtils::GraphId(const PNGraph &G, const TIntV &sg) {
	int nodes = sg.Len();
	uint64 graphId = 0;
	for(int i=0; i<nodes; i++) {
		for(int j=0; j<nodes; j++) {
			if(i==j) continue;
			//
			if(TGraphEnumUtils::IsEdge(G, sg[i], sg[j])) graphId+=TMath::Pow2(i*nodes + j);
		}
	}
	return graphId;
}

uint64 TGraphEnumUtils::GetMinAndGraphIds(const TVec<PNGraph> &isoG, TVec<uint64> &graphIds) {
	IAssert(isoG.Len() > 0);
	//
	uint64 minGraphId = GraphId(isoG[0]);
	graphIds.Add(minGraphId);
	//
	for(int i=1; i<isoG.Len(); i++) {
		uint64 curGraphId = GraphId(isoG[i]);
		if(minGraphId > curGraphId) minGraphId=curGraphId;
		//
		graphIds.Add(curGraphId);
	}
	//
	return minGraphId;
}
