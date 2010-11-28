#ifndef Snap_SubGraphEnum
#define Snap_SubGraphEnum

#include "Snap.h"

/////////////////////////////////////////////////
// Subgraph enumeration 
//
// Enumerates all connected induced subgraph on SubGraphSz nodes
// The algorithm is described in Efficient Detection of Network 
// Motifs by Sebastian Wernicke, IEEE/ACM Transactions on 
// Computational Biology and Bioinformatics, 2006.

template<class TGraphCounter>
class TSubGraphEnum {
private:
	class TSSet {
	protected:
		int m_capacity;
		int m_size;
		bool *m_nodes;
	public:
		TSSet(int capacity) {
			m_nodes = (bool *)malloc(capacity); memset(m_nodes, 0, capacity);
			m_capacity = capacity; m_size = 0; }
		TSSet(const TSSet &set) {
			m_nodes = (bool *)malloc(set.m_capacity); memcpy(m_nodes, set.m_nodes, set.m_capacity);
			m_capacity = set.m_capacity; m_size = set.m_size; }
		~TSSet() { free(m_nodes); }
	public:
		inline void Add(int i) { if(!m_nodes[i]) m_size++; m_nodes[i]=true; }
		inline void Remove(int i) { m_nodes[i]=false; m_size--; }
		inline bool IsKey(int i) const { return m_nodes[i]; }
		inline int Capacity() const { return m_capacity; }
		inline int Size() const { return m_size; }
		inline bool operator[](int i) const { return m_nodes[i]; }
	};
	class TSVec {
	protected:
		int m_capacity;
		int m_size;
		int *m_arr;
		TIntV m_v;
	public:
		TSVec(int capacity) {
			m_v.Gen(capacity); m_arr = (int *) m_v.BegI();
      for(int i=0; i<capacity; i++) { m_arr[i]=-1; }
			m_capacity = capacity; m_size = 0; }
	public:
		inline bool Contains(int nodeId) const {
			for(int i=0; i<m_size; i++) { if(m_arr[i]==nodeId) return true; } return false; }
	public:
		inline void Push(int nodeId) { m_arr[m_size]=nodeId; m_size++; }
		inline void Pop() { m_size--; m_arr[m_size]=-1; }
		inline int Capacity() const { return m_capacity; }
		inline int Size() const { return m_size; }
		inline const TIntV &getVec() const { return m_v; }
		inline int operator[](int i) const { return m_arr[i]; }
	};
private:
	PNGraph m_graph;
	int m_nodes;
	int m_subGraphSz;
	TGraphCounter *m_functor;
private:
	void GetSubGraphs_recursive(TSVec &sg, const TSSet &sgNbhs, TSSet &ext, int vId);
	void GetSubGraphs_recursive(TSVec &sg, const TSSet &sgNbhs, TSSet &ext);
public: 
  TSubGraphEnum() { }
	//Graph must be normalized (vertex ids are 0,1,2,...)
	void GetSubGraphs(PNGraph &Graph, int SubGraphSz, TGraphCounter& Counter);
	void GetSubGraphs(PNGraph &Graph, int NId, int SubGraphSz, TGraphCounter& Counter);
};
// TGraphCounter must implement 
// void operator()(const PNGraph &G, const TIntV &SubGraphNIdV);
// which gets called whenever a new subgraph on nodes in SubGraphNIdV is identified

/////////////////////////////////////////////////
// TSubGraphEnum implementation
template <class TGraphCounter>
void TSubGraphEnum<TGraphCounter>::GetSubGraphs_recursive(TSVec &sg, const TSSet &sgNbhs, TSSet &ext, int vId) {
	if(sg.Size() == m_subGraphSz) { (*m_functor)(m_graph, sg.getVec()); return; }
	//
	for(int i=0; i<ext.Capacity(); i++) {
		while(ext[i] == false) {
			i++;
			if(i == ext.Capacity()) return;
		}
		//
		int wId = i;
		TNGraph::TNodeI wIt = m_graph->GetNI(wId);
		int wDeg = wIt.GetDeg();
		//
		ext.Remove(wId);
		//
		TSSet newExt = ext;
		TSSet newSgNbhs = sgNbhs;
		for(int j=0; j<wDeg; j++) {
			int nbhId = wIt.GetNbhNId(j);
			if(nbhId > vId && !sgNbhs.IsKey(nbhId) && !sg.Contains(nbhId)) {
				newExt.Add(nbhId);
				newSgNbhs.Add(nbhId);
			}
		}
		sg.Push(wId);
		GetSubGraphs_recursive(sg, newSgNbhs, newExt, vId);
		sg.Pop();
	}
}

template <class TGraphCounter>
void TSubGraphEnum<TGraphCounter>::GetSubGraphs(PNGraph &Graph, int SubGraphSz, TGraphCounter &Functor) {
	m_graph = Graph;
	m_nodes = m_graph->GetNodes();
	m_subGraphSz = SubGraphSz;
	m_functor = &Functor;
	//
	TExeTm extime;
	for(TNGraph::TNodeI it=m_graph->BegNI(); it<m_graph->EndNI(); it++) {
		int vId = it.GetId();
		int vDeg = it.GetDeg();
		//Subgraph
		TSVec sg(SubGraphSz);
		sg.Push(vId);
		//Subgraph extension
		TSSet ext(m_nodes);
		for(int i=0; i<vDeg; i++) {
			int nbhId = it.GetNbhNId(i);
			if(nbhId > vId) 
				ext.Add(nbhId);
		}
		//Subgraph neighbours
		TSSet sgNbhs = ext;
		GetSubGraphs_recursive(sg, sgNbhs, ext, vId);
	}
	//printf("secs: %llf\n", extime.GetSecs());
}

template <class TGraphCounter>
void TSubGraphEnum<TGraphCounter>::GetSubGraphs_recursive(TSVec &sg, const TSSet &sgNbhs, TSSet &ext) {
	if(sg.Size() == m_subGraphSz) { (*m_functor)(m_graph, sg.getVec()); return; }
	//
	for(int i=0; i<ext.Capacity(); i++) {
		while(ext[i] == false) {
			i++;
			if(i == ext.Capacity()) return;
		}
		//
		int wId = i;
		TNGraph::TNodeI wIt = m_graph->GetNI(wId);
		int wDeg = wIt.GetDeg();
		//
		ext.Remove(wId);
		//
		TSSet newExt = ext;
		TSSet newSgNbhs = sgNbhs;
		for(int j=0; j<wDeg; j++) {
			int nbhId = wIt.GetNbhNId(j);
			if(!sgNbhs.IsKey(nbhId) && !sg.Contains(nbhId)) {
				newExt.Add(nbhId);
				newSgNbhs.Add(nbhId);
			}
		}
		sg.Push(wId);
		GetSubGraphs_recursive(sg, newSgNbhs, newExt);
		sg.Pop();
	}
}

template <class TGraphCounter>
void TSubGraphEnum<TGraphCounter>::GetSubGraphs(PNGraph &Graph, int NId, int SubGraphSz, TGraphCounter &Functor) {
	m_graph = Graph;
	m_nodes = m_graph->GetNodes();
	m_subGraphSz = SubGraphSz;
	m_functor = &Functor;
	//
	TNGraph::TNodeI it=m_graph->GetNI(NId);
	int vId = NId;
	int vDeg = it.GetDeg();
	//Subgraph
	TSVec sg(SubGraphSz);
	sg.Push(vId);
	//Subgraph extension
	TSSet ext(m_nodes);
	for(int i=0; i<vDeg; i++) {
		int nbhId = it.GetNbhNId(i);
		ext.Add(nbhId);
	}
	//Subgraph neighbours
	TSSet sgNbhs = ext;
	//
	TExeTm extime;
	GetSubGraphs_recursive(sg, sgNbhs, ext);
	printf("secs: %llf\n", extime.GetSecs());
}


#endif
