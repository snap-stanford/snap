namespace TSnap {

// Max integer network flow algorithms. Currently utilizes the Edmonds-Karp algorithm.
// Upper bound runtime of O(VE^2)


// Max integer network flow for the PNEANet class. The network is expected to have the following attributes:
//   capacity: nonnegative integer valued capacity. An upper bound for the flow on each edge.
const TStr CapAttrName("capacity");

// Given a network and a source and sink NId, finds the maximum integer valued flow from source to sink
int GetMaxFlowIntEK (PNEANet &Net, const int &SrcNId, const int &SnkNId); // Uses Edmonds-Karp
int GetMaxFlowIntPR (PNEANet &Net, const int& SrcNId, const int& SnkNId); // Uses Push-Relabel

};
