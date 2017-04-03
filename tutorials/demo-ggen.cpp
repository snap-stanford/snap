#include "Snap.h"

// Print undirected graph statistics
void PrintGStats(const char s[], PUNGraph Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph->GetNodes(), Graph->GetEdges(),
      Graph->Empty() ? "yes" : "no");
}

// Print direct graph statistics
void PrintGStats(const char s[], PNGraph Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
         s, Graph->GetNodes(), Graph->GetEdges(),
         Graph->Empty() ? "yes" : "no");
}

// Print bipartite graph statistics
void PrintGStats(const char s[], PBPGraph Graph) {
  printf("graph %s, right nodes %d, left nodes %d, edges %d, empty %s\n",
         s, Graph->GetRNodes(), Graph->GetLNodes(),
         Graph->GetEdges(), Graph->Empty() ? "yes" : "no");
}

template <class PGraph>
void PrintGStats(const char s[], PGraph Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
         s, Graph->GetNodes(), Graph->GetEdges(),
         Graph->Empty() ? "yes" : "no");
}


// Generate Grid graph 
void GenerateGridGraph() {
  PNGraph NGraph;
  PUNGraph UNGraph;
  const int Rows = 300;
  const int Cols = 250;
  
  UNGraph = TSnap::GenGrid<PUNGraph>(Rows, Cols, false);
  PrintGStats("GenerateGridGraph:UNGraph", UNGraph);

  NGraph = TSnap::GenGrid<PNGraph>(Rows, Cols, true);
  PrintGStats("GenerateGridGraph:NGraph", NGraph);
}

// Generate Star graph 
void GenerateStarGraph() {
  const int NNodes = 1000;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenStar<PUNGraph>(NNodes, false);
  PrintGStats("GenerateStarGraph:UNGraph", UNGraph);

  // Generate directed graph
  NGraph = TSnap::GenStar<PNGraph>(NNodes, true);
  PrintGStats("GenerateStarGraph:NGraph", NGraph);
}

// Generate Circle graph 
void GenerateCircleGraph() {
  const int NNodes = 500;
  const int NodeOutDeg = 5;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenCircle<PUNGraph>(NNodes, NodeOutDeg, false);
  PrintGStats("GenerateCircleGraph:NGraph", UNGraph);
  
  // Generate directed graph
  NGraph = TSnap::GenCircle<PNGraph>(NNodes, NodeOutDeg, true);
  PrintGStats("GenerateCircleGraph:NGraph", NGraph);
}

// Generate Full graph 
void GenerateFullGraph() {
  const int NNodes = 100;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenFull<PUNGraph>(NNodes);
  PrintGStats("GenerateFullGraph:UNGraph", UNGraph);

  // Generate directed graph
  NGraph = TSnap::GenFull<PNGraph>(NNodes);
  PrintGStats("GenerateFullGraph:NNGraph", NGraph);
}

// Generate Tree graph 
void GenerateTreeGraph() {
  const int Fanout = 10;
  const int Levels = 5;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenTree<PUNGraph>(Fanout, Levels, false);
  PrintGStats("GenerateTreeGraph:UNGraph", UNGraph);

  // Generate directed graph
  NGraph = TSnap::GenTree<PNGraph>(Fanout, Levels, true, true);
  PrintGStats("GenerateTreeGraph:NGraph", NGraph);
}

// Generate BaraHierar graph 
void GenerateBaraHierarGraph() {
  const int Levels = 6;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenBaraHierar<PUNGraph>(Levels);
  PrintGStats("GenerateBaraHierarGraph:UNGraph", UNGraph);

  // Generate directed graph
  NGraph = TSnap::GenBaraHierar<PNGraph>(Levels);
  PrintGStats("GenerateBaraHierarGraph:UNGraph", NGraph);
}

// Generate RndGnm graph 
void GenerateRndGnmGraph() {
  const int NNodes = 30;
  const int NEdges = 50;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  TInt::Rnd.PutSeed(0);
  
  // Generate undirected graph
  UNGraph = TSnap::GenRndGnm<PUNGraph>(NNodes, NEdges, false, TInt::Rnd);
  PrintGStats("GenerateRndGnmGraph:UNGraph", UNGraph);

  // Generate directed graph
  NGraph = TSnap::GenRndGnm<PNGraph>(NNodes, NEdges, true, TInt::Rnd);
  PrintGStats("GenerateRndGnmGraph:NGraph", NGraph);
}

// Generate RndBipart graph 
void GenerateRndBipartGraph() {
  const int LeftNodes = 20;
  const int RightNodes = 50;
  const int Edges = 30;
  
  PBPGraph BPGraph;
  
  TInt::Rnd.PutSeed(0);

  // Generate directed bipartite graph
  BPGraph = TSnap::GenRndBipart(LeftNodes, RightNodes, Edges, TInt::Rnd);
  PrintGStats("GenerateRndBiPartGraph:BPGraph", BPGraph);
}

// Generates power-law like networks
// http://en.wikipedia.org/wiki/Scale-free_network
void GenerateRndPowerLawGraph() {
  const int NNodes = 1000;
  const double PowerExp = 3.0;
  
  PUNGraph UNGraph;
  
  TInt::Rnd.PutSeed(0);
  
  // Generate undirected graph
  UNGraph = TSnap::GenRndPowerLaw(NNodes, PowerExp);
  
  TStr FuncStr = TStr::Fmt("%s::UNgraph", __FUNCTION__);
  PrintGStats(FuncStr.CStr(), UNGraph);
}

// Generate graphs with specific degree sequences
void GenerateDegSeqGraph() {
  const int NNodes = 15;
  
  PUNGraph UNGraph;
  TIntV DegSeqV;
  TInt::Rnd.PutSeed(0);
      
  // Generate Deg Sequence vector randomly, populate long tail of 1's
  DegSeqV.Gen(NNodes);
  int DegSum = 0;
  for (int n = 0; n < DegSeqV.Len()/4; n++) {
    DegSeqV[n] = TInt::Rnd.GetUniDevInt(1, NNodes/2);
    DegSum += DegSeqV[n];
  }
  // Add 1's to the tail
  for (int n = DegSeqV.Len()/4; n < DegSeqV.Len(); n++) {
    DegSeqV[n] = 1;
    DegSum += DegSeqV[n];
  }
  // Make sure the sum of degrees is divisible by 2
  if (DegSum % 2 != 0) {
    DegSeqV[(int)TInt::Rnd.GetUniDevInt(NNodes)]++;
  }
  DegSeqV.Sort();
  DegSeqV.Reverse();
  
  UNGraph = TSnap::GenDegSeq(DegSeqV);
  
  printf("NNodes=%d, ", NNodes);
  printf("DegSeqV = { ");
  for (int i = 0; i < DegSeqV.Len(); i++) {
    printf("%d ", (int)DegSeqV[i]);
  }
  printf("}\n");
  
  TStr FuncStr = TStr::Fmt("%s:UNgraph", __func__);
  PrintGStats(FuncStr.CStr(), UNGraph);
}

// Generate PrefAttach graph 
void GeneratePrefAttachGraph() {
  const int NNodes = 100;
  const int NodeOutDeg = 15;
  
  PUNGraph UNGraph;

  // Generate undirected graph
  UNGraph = TSnap::GenPrefAttach(NNodes, NodeOutDeg);
  
  TStr FuncStr = TStr::Fmt("%s:UNgraph", __func__);
  PrintGStats(FuncStr.CStr(), UNGraph);
}

// Generate GeoPrefAttach graph 
void GenerateGeoPrefAttachGraph() {
  const int NNodes = 100;
  const int NodeOutDeg = 15;
  const double Beta = 2.0;
  
  PUNGraph UNGraph;
  PNGraph NGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenGeoPrefAttach(NNodes, NodeOutDeg, Beta);
  
  TStr FuncStr = TStr::Fmt("%s:UNgraph", __func__);
  PrintGStats(FuncStr.CStr(), UNGraph);
}

// Generate Small World graph
void GenerateSmallWorldGraph() {
  const int NNodes = 1000;
  const int NodeOutDeg = 30;
  const double RewireProb = 1.0;
  
  PUNGraph UNGraph;
  
  // Generate undirected graph
  UNGraph = TSnap::GenSmallWorld(NNodes, NodeOutDeg, RewireProb);
  
  TStr FuncStr = TStr::Fmt("%s:UNgraph", __func__);
  PrintGStats(FuncStr.CStr(), UNGraph);
}

// Generate forest fire graph
void GenerateForestFireGraph() {
  const int NNodes = 1000;
  const double FwdProb = 1.0;
  const double BckProb = 1.0;
  
  // Generate directed forest fire graph
  PNGraph NGraph = TSnap::GenForestFire(NNodes, FwdProb, BckProb);
  
  TStr FuncStr = TStr::Fmt("%s:Ngraph", __func__);
  PrintGStats(FuncStr.CStr(), NGraph);
}

// Generate copy model graph 
void GenerateCopyModelGraph() {
  const int NNodes = 100;
  const double Beta = 2.0;
  
  PNGraph NGraph;
  NGraph = TSnap::GenCopyModel(NNodes, Beta);
  
  TStr FuncStr = TStr::Fmt("%s:NGraph", __func__);
  PrintGStats(FuncStr.CStr(), NGraph);
}

// Generate RMat graph 
void GenerateRMatGraph() {
  const int NNodes = 100;
  const int NEdges = 50;
  
  PNGraph NGraph;
  
  TInt::Rnd.PutSeed(0);
  TFlt A, B, C;
  TFltTrV Vals;
  Vals.Add(TFltTr(0.15, 0.2, 0.35));
  Vals.Add(TFltTr(0.2, 0.2, 0.55));
  Vals.Add(TFltTr(0.4, 0.15, 0.2));
  
  for (int i = 0; i < Vals.Len(); i++) {
    
    A = Vals[i].Val1;
    B = Vals[i].Val2;
    C = Vals[i].Val3;
    
    NGraph = TSnap::GenRMat(NNodes, NEdges, A, B, C);
    
    TStr FuncStr = TStr::Fmt("%s:NGraph", __func__);
    PrintGStats(FuncStr.CStr(), NGraph);

  } // end loop - Vals
}

int main(int argc, char* argv[]) {
  GenerateGridGraph();
  GenerateStarGraph();
  GenerateCircleGraph();
  GenerateFullGraph();
  GenerateTreeGraph();
  GenerateBaraHierarGraph();
  GenerateRndGnmGraph();
  GenerateRndBipartGraph();
  GenerateRndPowerLawGraph();
  GenerateDegSeqGraph();
  GeneratePrefAttachGraph();
  GenerateGeoPrefAttachGraph();
  GenerateSmallWorldGraph();
  GenerateForestFireGraph();
  GenerateCopyModelGraph();
  GenerateRMatGraph();
}
