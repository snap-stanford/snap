#include "efficiency.hpp"
#include <vector>

namespace TSnap {
    
    /////////////////////////////////////////////////
    // Node efficiency measures
    /////////////////////////////////////////////////
    float getAvgEfficiency(const PUNGraph& Graph) {
        float efficiency = 0.0;
        float num = 1.0; 
        float num_nodes = Graph->GetNodes(); 
        vector<int> node_ids; 
        for (typename PUNGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
            node_ids.push_back(NI.GetId());
        }
        for (int i = 0; i < node_ids.size(); i++) {
            for (int j = 0; j < node_ids.size(); j++) {
                if (i != j) {
                    efficiency += (num / (float) GetShortPath(Graph, i, j, true));
                }
            }
        }
        float ans = num / (num_nodes * (num_nodes-1)) * efficiency;
        return ans; 
    }

    float getGlobalEfficiency(const PUNGraph& Graph) {
        float E_G = getAvgEfficiency(Graph);
        int num_nodes = Graph->GetNodes();
        PUNGraph G_ideal = GenFull<PUNGraph>(num_nodes)
        float E_G_ideal = getAvgEfficiency(G_ideal);
        return E_G / E_G_ideal;
    }

    
}; // namespace TSnap
