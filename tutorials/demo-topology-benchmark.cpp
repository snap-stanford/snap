#include "Snap.h"
#include <time.h>

// Benchmarking prototypes:
template <class PGraph> void BenchmarkGraphEdgeI(PGraph Graph, bool isDefrag);
template <class PGraph> void BenchmarkGraphNodeI(PGraph Graph, bool isDefrag);
template <class PGraph> void BenchmarkGraphDegTrav(PGraph Graph, bool isDefrag);
template <class PGraph> void Benchmark(PGraph G);

// Added to pass the assert condition, which overflows for large node values
template <class PGraph> PGraph GenRndGnm(const int& Nodes, const int& Edges, const bool& IsDir=true, TRnd& Rnd=TInt::Rnd);
template <class PGraph>
PGraph GenRndGnm(const int& Nodes, const int& Edges, const bool& IsDir, TRnd& Rnd) {
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, Edges);
  for (int node = 0; node < Nodes; node++) {
    IAssert(Graph.AddNode(node) == node);
  }
  for (int edge = 0; edge < Edges; ) {
    const int SrcNId = Rnd.GetUniDevInt(Nodes);
    const int DstNId = Rnd.GetUniDevInt(Nodes);
    if (SrcNId != DstNId && Graph.AddEdge(SrcNId, DstNId) != -2) {
      if (! IsDir) { Graph.AddEdge(DstNId, SrcNId); }
      edge++;
    }
  }
  return GraphPt;
}

template <class PGraph>
void BenchmarkGraphEdgeI(PGraph Graph, bool isDefrag) {
  int ECount = 0;
  clock_t start = clock();

  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ECount++;
  }
  if (ECount - Graph->GetEdges()) {
    printf("Error iterating over edges\n");
  }
  
  int msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
  printf("Nodes: %d Edges: %d Time: %d ms\n", Graph->GetNodes(), ECount, msec);
}

template <class PGraph>
void BenchmarkGraphDegTrav(PGraph Graph, bool isDefrag) {
  int ECount = 0;
  clock_t start = clock();

  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ECount++;
    }
  }
  if (ECount - Graph->GetEdges()) {
    printf("Error iterating over edges per node\n");
  }
  
  int msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
  printf("Nodes: %d Edges: %d Time: %d ms\n", Graph->GetNodes(), ECount, msec);
}

template <class PGraph>
void BenchmarkGraphNodeI(PGraph Graph, bool isDefrag) {
  int NCount = 0;
  clock_t start = clock();

  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }
  if (NCount - Graph->GetNodes()) {
    printf("Error iterating over nodes\n");
  }
  
  int msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
  printf("Nodes: %d Edges: %d Time: %d ms\n", NCount, Graph->GetEdges(), msec);
}

template <class PGraph>
void Benchmark(PGraph G) {
  printf("NodeI ");
  BenchmarkGraphNodeI(G, false);
  printf("EdgeI ");
  BenchmarkGraphEdgeI(G, false);
  printf("NodeE ");
  BenchmarkGraphDegTrav(G, false);
}

int main(int argc, char* argv[]) {
  // benchmark graph creation
  int OneK = 1000;
  int NodeArr[5];
  int EdgeArr[3];
  int i = 0;
  int j = 0;
  int k = 0;
  int TenP = 1;
  int NNodes = 0;
  int NEdges = 0;
  int msec = 0;
  PNGraph G1;
  PUNGraph G2;
  PNEGraph G3;
  PNEAGraph G4;

  for (i = 0; i < 5; i++) {
    TenP *= 10;
    NodeArr[i] = TenP * OneK;
  } 
  EdgeArr[0] = 10;
  EdgeArr[1] = 50;
  EdgeArr[2] = 100;
  
  for (k = 0; k < 4; k++) {
    printf("Starting Benchmarking for ");
    switch (k) {
    case 0:
      printf("TNGraph\n\n");
      break;
    case 1:
      printf("TUNGraph\n\n");
      break;
    case 2:
      printf("TNEGraph\n\n");
      break;
    default:
      printf("TNEAGraph\n\n");
      break;
    }

    for (i = 0; i < 5; i++) {
      for (j = 0; j < 3; j++) {
	clock_t start = clock();
	NNodes = NodeArr[i];
	NEdges = EdgeArr[j]*NodeArr[i];
	switch (k) {
      case 0:
	printf("\nGenerating Graph...\n");
	printf("GrGen ");
	G1 = GenRndGnm<PNGraph>(NNodes, NEdges, false);
	msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	printf("Nodes: %d Edges: %d Time: %d ms\n", NNodes, NEdges, msec);
	Benchmark(G1);
	printf("Defragmenting...\n");
	G1->Defrag();
	Benchmark(G1);
	break;
      case 1:
	printf("\nGenerating Graph...\n");
	printf("GrGen ");
	G2 = GenRndGnm<PUNGraph>(NNodes, NEdges, true);
	msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	printf("Nodes: %d Edges: %d Time: %d ms\n", NNodes, NEdges, msec);
	Benchmark(G2);    
	printf("Defragmenting...\n");
	G2->Defrag();
	Benchmark(G2);
	break;
      case 2:
	printf("\nGenerating Graph...\n");
	printf("GrGen ");
	G3 = GenRndGnm<PNEGraph>(NNodes, NEdges, true);
	msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	printf("Nodes: %d Edges: %d Time: %d ms\n", NNodes, NEdges, msec);
	Benchmark(G3);    
	printf("Defragmenting...\n");
	G3->Defrag();
	Benchmark(G3);
	break;
      default:
	printf("\nGenerating Graph...\n");
	printf("GrGen ");
	G4 = GenRndGnm<PNEAGraph>(NNodes, NEdges, true);
	msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	printf("Nodes: %d Edges: %d Time: %d ms\n", NNodes, NEdges, msec);
	Benchmark(G4);    
	printf("Defragmenting...\n");
	G4->Defrag();
	Benchmark(G4);
	break;
	}
      }
    }
  }
}

