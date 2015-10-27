namespace TSnap {

// Mandatory name of edge capacity attribute for flow networks.
const TStr CapAttrName = "capacity";

/// Returns the maximum integer valued flow in the network \c Net from source \c SrcNId to sink \c SnkNId. ##TSnap::GetMaxFlowIntEK
int GetMaxFlowIntEK (PNEANet &Net, const int &SrcNId, const int &SnkNId);
/// Returns the maximum integer valued flow in the network \c Net from source \c SrcNId to sink \c SnkNId. ##TSnap::GetMaxFlowIntEK
int GetMaxFlowIntPR (PNEANet &Net, const int &SrcNId, const int &SnkNId);

};
