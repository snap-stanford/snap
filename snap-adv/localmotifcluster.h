#ifndef snap_localmotifcluster_h
#define snap_localmotifcluster_h

#include "Snap.h"

typedef TVec< THash<TInt, TFlt> > WeightVH;
typedef TVec< THash<TInt, TIntV> > CountVH;


/*
Section: Motif
*/
// Definition of motifs, on both directed graphs and undirected graphs
enum MotifType {
  // directed graph 2-node motifs
  UniDE,      // uni-directed edge, u --> v but not v --> u
  BiDE,       // bi-directed edge, u <--> v
  DE,         // directed edge, = UniDE + 2 * BiDE
  DE_any,     // any edge, counted as 1 if(u --> v or v --> u), = UniDE + BiDE

  // directed graph 3-node motifs, see [Austin et al. 2016 Science] for more information.
  M1,         // u  --> v, v  --> w, w  --> u
  M2,         // u <--> v, v  --> w, w  --> u
  M3,         // u <--> v, v <--> w, w  --> u
  M4,         // u <--> v, v <--> w, w <--> u
  M5,         // u  --> v, v  --> w, u  --> w
  M6,         // u <--> v, w  --> u, w  --> v
  M7,         // u <--> v, u  --> w, v  --> w
  triad,      // M1 + M2 + M3 + M4 + M5 + M6 + M7
  cycle,      // M1 + M2 + M3 + 2 * M4
  FFLoop,     // feed-forward loop, M5 + 2 * M6 + 2 * M7

  // undirected graph motifs
  UEdge,      // undirected edges
  clique3,    //       |
  clique4,    //       |
  clique5,    //       |
  clique6,    //       |
  clique7,    //       |
  clique8,    //       |
  clique9,    //       |
};

// Given a string representation of a motif name, parse it to a MotifType.
// Will throw an exception if (1) the graph is directed but the motif is for undirected graph, or (2) the other way.
MotifType ParseMotifType(const TStr& motif, const bool& IsDirected = false);




/*
Section: Preprocessing graphs, including:
1. counting motif on each edge
2. obtain a transformed weighted graph
*/
class ProcessedGraph {
 private :
  PUNGraph Graph_org;   // A pointer to the original input graph.
                        // If the input is a directed graph, we will create its undirected version.
                        // This is used in forming the weighted graph.

  PUNGraph Graph_trans; // The transformed graph which we will use in MAPPR (the first step of MAPPR)
                        // Note this is an unweighted graph, and the weights are stored in variable {WeightVH Weights}
                        // Note that is graph may be much more sparse than the original graph  

  CountVH Counts;       // To store the number of motif instances on each edge.
                        // vec = Counts[u].GetDat(v) are the numbers of instances of several motif types on edge (u, v)
                        // For undirected graph, vec[i] is the number of (i+3)-cliques on this edge, i.e., vec[0] is for triangle
                        // For directed graph motif, 
                        //    vec[0] is for uni-directed edges,
                        //    vec[1] is for bi-directed edges,
                        //    vec[i] is for M_{i-1} as defined in Austin et al. (Science 2016).

  WeightVH Weights;     // The weights of the weighted graph
                        // Weights[u].GetDat(v) is the weight of edge (u, v)
                        // Weights[u].GetDat(u) is the weighted degree of node u

  float TotalVol;       // The total volume of the whole graph, needed in computing the conductance


  // This function counts the undirected graph motif (clique) instances on each edge.
  // It uses recursive method for clique enumeration proposed by Chiba and Nishizeki (SIAM J. Comput. 1985).
  // Input {int KSize} denotes the size of the clique we are to enumerate in the current graph {PUNGraph& G}
  // Input {TIntV& PrevNodes} denotes a set of nodes that are directed connected to any node in the current graph G
  //    and {int level = PrevNodes.Len()} is the number of PreNodes. Therefore, any k-clique in G corresponds to 
  //    a (k+level)-clique after all nodes in PrevNodes are added in the current graph G.
  void countClique(PUNGraph& G, int KSize, TIntV& PrevNodes, int level);

  // This function counts the directed graph motif instances on each edge.
  // void countDirEdgeMotif(PNGraph graph);
  void countDirTriadMotif(PNGraph graph);


 public :
  // Initialing, which will run assignWeights* functions and obtain the weighted transformed graph.
  ProcessedGraph() {}
  // For undirected input graph.
  ProcessedGraph(PUNGraph graph, MotifType mt);
  // For directed input graph
  ProcessedGraph(PNGraph graph, MotifType mt);

  // Two functions, for undirected graph and directed graph respectively, that
  //  1) counts motifs on each pair of nodes
  //  2) assign weights
  //  3) obtain the transformed graph
  void assignWeights_undir(MotifType mt);
  void assignWeights_dir(MotifType mt);

  // Output and printing
  PUNGraph getOriginalGraph() const {return Graph_org; };
  PUNGraph getTransformedGraph() const {return Graph_trans; };
  CountVH getCounts() const { return Counts; };
  const WeightVH& getWeights() const { return Weights; };
  float getTotalVolume() const { return TotalVol; };
  void printCounts();
  void printWeights();
};




/*
Section: MAPPR main part, including:
1. compute the APPR vector on the weighted transformed graph
2. obtain the cluster by sweeping the NCP profile
*/
class MAPPR {
 private:
  THash<TInt, TFlt> appr_vec; // To store the APPR vector of the specified input 
  
  int NumPushs;               // Number of pushes in computing the APPR vector
  
  float appr_norm;            // L1-norm of the APPR vector

  TIntV NodeInOrder;          // Node Ids in the NCP in order

  TFltV MtfCondProfile;       // Conductance of the NCP in order
  
  int SizeGlobalMin;          // Size of the cluster given by the global minimum of NCP
                              // Value is initialized as 0 when this has not been computed.
  
  int SizeFirstLocalMin;      // Size of the cluster given by the first local minimum of NCP
                              // Value is initialized as -1 when this has not been computed.

  TIntV Cluster;              // To store the desired cluster

  // To compute the NCP of the graph with precomputed APPR vector
  // Results are stored in {this->NodeInOrder and MtfCondProfile}.
  // It will also compute the global min and first local min of the NCP.
  void computeProfile(const ProcessedGraph& graph_p);

  // To compute the size the global minimum in NCP
  void findGlobalMin();

  // To compute the size of the first local minimum in NCP
  void findFirstlocalMin();

  // To check whether the specified {idx} is a local min in NCP.
  // Local minimum is defined in [Yin et al 2017 KDD}.
  bool isLocalMin(int idx, double thresh=1.2);


 public:
  MAPPR();

  // Function to compute the APPR vector on the weighted transformed graph {ProcessedGraph graph_p} 
  //    with seed {int SeedNodeId} alpha = {float alpha}, and epsilon = {float eps}.
  // It will also compute the NCP as well as with {SizeGlobalMin} and {SizeFirstLocalMin}, using {computeProfile(*)}.
  // Results are stored in {this->appr_vec}, {this->NodeInOrder, MtfCondProfile}, and {this->SizeGlobalMin, SizeFirstLocalMin}.
  void computeAPPR(const ProcessedGraph& graph_p, const int SeedNodeId, float alpha, float eps);

  // Function to find the desired cluster based on the NCP.
  // Option > 0 will give the cluster of size option;
  // Option = 0 will give the cluster of global minimum conductance;
  // Option = -1 will give the cluster of first local minimum in the NCP.
  // Result is stored in {TIntV Cluster}.
  // Note that this function can only be run after finishing {computeAPPR(...)}
  void sweepAPPR(int option = -1);

  // Output and printing
  THash<TInt, TFlt> getAPPR() { return appr_vec; };
  TIntV getCluster() { return Cluster; };
  void printAPPR();
  void printProfile();
};




#endif  // snap_localmotifcluster_h