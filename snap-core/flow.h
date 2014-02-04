namespace TSnap {

// Max integer network flow algorithms. Currently utilizes the Edmonds-Karp algorithm.
// Upper bound runtime of O(VE^2)


// Max integer network flow for the PNEANet class. The network is expected to have two attributes defined for each edge
//   flow: integer valued flow. After running the algorithm, this attribute will contain the flow over each edge
//   capacity: nonnegative integer valued capacity. An upper bound for the flow on each edge.
const TStr FlowAttrName("flow");
const TStr CapAttrName("capacity");

// Given a network and a source and sink NId, finds the maximum integer valued flow from source to sink
int GetMaxFlowInt (PNEANet &Net, const int &SrcNId, const int &SnkNId);


// Max integer network flow for the PNGraph class. Two hashes accompany the graph to provide the flow and capacity atributes.
// Each hash maps from a NId pair (an edge) to the flow/capacity on that edge.
int GetMaxFlowInt (const PNGraph &Graph, const TIntPrIntH &CapAttrH, TIntPrIntH &FlowAttrH, const int &SrcNId, const int &SnkNId);

};
