#include "Snap.h"

template<class PGraph> void GenerateNodes(const PGraph& Graph, int NumNodes);
template<class PGraph> PGraph GenerateLoopGraph(const PGraph& Graph);
PNGraph GetTNLoopGraph();


// Dump the graph
template <class PGraph>
void PrintGraph(const char s[], const PGraph& Graph) {
  printf("---- %s -----\n",s);
  Graph->Dump();
}

// Print int value
void PrintIntValue(const char Desc[],
                   const int Value) {
  printf("---- %s -----\nValue: %d\n", Desc, Value);
}

// Print TIntPrV values
void PrintTIntPrVValues(const char Desc[],
                        const TIntPrV& Value) {
  printf("---- %s -----\n", Desc);
  for (int i = 0; i < Value.Len(); i++) {
    printf("Key: %d, Value: %d\n", Value[i].Val1.Val, Value[i].Val2.Val);
  }
}

// Print TFltPrV values
void PrintTFltPrVValues(const char Desc[],
                        const TFltPrV& Value) {
  printf("---- %s -----\n", Desc);
  for (int i = 0; i < Value.Len(); i++) {
    printf("Key: %f, Value: %f\n", Value[i].Val1.Val, Value[i].Val2.Val);
  }
}

// Print TIntV values
void PrintTIntVValues(const char Desc[],
                      const TIntV& Value) {
  printf("---- %s -----\n", Desc);
  for (int i = 0; i < Value.Len(); i++) {
    printf("Value: %d\n", Value[i].Val);
  }
}

// Print break in between methods
void PrintMethodHeader(const char Desc[]) {
  printf("\n\n\n\n\n----- %s -----\n", Desc);
}

/////////////////////////////////////////////////
// Algorithms
/////////////////////////////////////////////////

////
// CntInDegNodes
////

void PrintCntInDegNodes() {
  PrintMethodHeader("CntInDegNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntInDegNodes on TNLoopGraph",
                TSnap::CntInDegNodes(Graph, 1));
}

////
// CntOutDegNodes
////


void PrintCntOutDegNodes() {
  PrintMethodHeader("CntOutDegNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntOutDegNodes on TNLoopGraph",
                TSnap::CntOutDegNodes(Graph, 1));
}

////
// CntDegNodes
////


void PrintCntDegNodes() {
  PrintMethodHeader("CntDegNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntDegNodes on TNLoopGraph",
                 TSnap::CntDegNodes(Graph, 2));
}

////
// CntNonZNodes
////


void PrintCntNonZNodes() {
  PrintMethodHeader("CntNonZNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntNonZNodes on TNLoopGraph",
                 TSnap::CntNonZNodes(Graph));
}

////
// CntEdgesToSet
////

void PrintCntEdgesToSet() {
  PrintMethodHeader("CntEdgesToSet");

  PNGraph Graph = GetTNLoopGraph();

  TIntSet NodeKeysV;
  NodeKeysV.AddKey(0);
  NodeKeysV.AddKey(1);
  NodeKeysV.AddKey(2);
  NodeKeysV.AddKey(3);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntEdgesToSet on TNLoopGraph",
                 TSnap::CntEdgesToSet(Graph, 4, NodeKeysV));
}

////
// GetMxDegNId
////

void PrintGetMxDegNId() {
  PrintMethodHeader("GetMxDegNId");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling GetMxDegNId on TNLoopGraph",
                TSnap::GetMxDegNId(Graph));
}

////
// GetMxInDegNId
////

void PrintGetMxInDegNId() {
  PrintMethodHeader("GetMxInDegNId");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling GetMxInDegNId on TNLoopGraph",
                TSnap::GetMxInDegNId(Graph));
}

////
// GetMxOutDegNId
////

void PrintGetMxOutDegNId() {
  PrintMethodHeader("GetMxOutDegNId");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling GetMxOutDegNId on TNLoopGraph",
                TSnap::GetMxOutDegNId(Graph));
}

////
// GetInDegCnt for Int
////

void PrintGetInDegCntInt() {
  PrintMethodHeader("GetInDegCnt for Int");

  PNGraph Graph = GetTNLoopGraph();

  TIntPrV TNInDegCntLoopGraph;
  TSnap::GetInDegCnt(Graph, TNInDegCntLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntPrVValues("Calling GetInDegCnt on TNLoopGraph",
                     TNInDegCntLoopGraph);
}


////
// GetInDegCnt for Flt
////

void PrintGetInDegCntFlt() {
  PrintMethodHeader("GetInDegCnt for Flt");

  PNGraph Graph = GetTNLoopGraph();

  TFltPrV TNInDegCntLoopGraph;
  TSnap::GetInDegCnt(Graph, TNInDegCntLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTFltPrVValues("Calling GetInDegCnt on TNLoopGraph",
                     TNInDegCntLoopGraph);
}

////
// GetOutDegCnt for Int
////

void PrintGetOutDegCntInt() {
  PrintMethodHeader("GetOutDegCnt for Int");

  PNGraph Graph = GetTNLoopGraph();

  TIntPrV TNOutDegCntLoopGraph;
  TSnap::GetOutDegCnt(Graph, TNOutDegCntLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntPrVValues("Calling GetOutDegCnt on TNLoopGraph",
                     TNOutDegCntLoopGraph);
}

////
// GetOutDegCnt for Flt
////

void PrintGetOutDegCntFlt() {
  PrintMethodHeader("GetOutDegCnt for Flt");

  PNGraph Graph = GetTNLoopGraph();

  TFltPrV TNOutDegCntLoopGraph;
  TSnap::GetOutDegCnt(Graph, TNOutDegCntLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTFltPrVValues("Calling GetOutDegCnt on TNLoopGraph",
                     TNOutDegCntLoopGraph);
}

////
// GetDegCnt for Int
////

void PrintGetDegCntInt() {
  PrintMethodHeader("GetDegCnt for Int");

  PNGraph Graph = GetTNLoopGraph();

  TIntPrV TNDegCntLoopGraph;
  TSnap::GetDegCnt(Graph, TNDegCntLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntPrVValues("Calling GetDegCnt on TNLoopGraph",
                     TNDegCntLoopGraph);
}

////
// GetDegCnt for Flt
////

void PrintGetDegCntFlt() {
  PrintMethodHeader("GetDegCnt for Flt");

  PNGraph Graph = GetTNLoopGraph();

  TFltPrV TNDegCntLoopGraph;
  TSnap::GetDegCnt(Graph, TNDegCntLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTFltPrVValues("Calling GetDegCnt on TNLoopGraph",
                     TNDegCntLoopGraph);
}

////
// GetDegSeqV
////

void PrintGetDegSeqV() {
  PrintMethodHeader("GetDegSeqV");

  PNGraph Graph = GetTNLoopGraph();

  TIntV TNDegSeqVLoopGraph;
  TSnap::GetDegSeqV(Graph, TNDegSeqVLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntVValues("Calling GetDegSeqV on TNLoopGraph",
                   TNDegSeqVLoopGraph);
}

////
// GetDegSeqV with InDegV and OutDegV
////

void PrintGetDegSeqVWithInOutDegV() {
  PrintMethodHeader("GetDegSeqV");

  PNGraph Graph = GetTNLoopGraph();

  TIntV TNInDegVLoopGraph;
  TIntV TNOutDegVLoopGraph;
  TSnap::GetDegSeqV(Graph, TNInDegVLoopGraph, TNOutDegVLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntVValues("InDegV from calling GetDegSeqV on TNLoopGraph",
                   TNInDegVLoopGraph);
  PrintTIntVValues("OutDegV from calling GetDegSeqV on TNLoopGraph",
                   TNOutDegVLoopGraph);
}

////
// GetNodeInDegV
////

void PrintGetNodeInDegV() {
  PrintMethodHeader("GetNodeInDegV");

  PNGraph Graph = GetTNLoopGraph();

  TIntPrV TNInDegVLoopGraph;
  TSnap::GetNodeInDegV(Graph, TNInDegVLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntPrVValues("Calling GetNodeInDegV on TNLoopGraph",
                     TNInDegVLoopGraph);
}

////
// GetNodeOutDegV
////

void PrintGetNodeOutDegV() {
  PrintMethodHeader("GetNodeOutDegV");

  PNGraph Graph = GetTNLoopGraph();

  TIntPrV TNOutDegVLoopGraph;
  TSnap::GetNodeOutDegV(Graph, TNOutDegVLoopGraph);

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintTIntPrVValues("Calling GetNodeOutDegV on TNLoopGraph",
                     TNOutDegVLoopGraph);
}

////
// CntUniqUndirEdges
////

void PrintCntUniqUndirEdges() {
  PrintMethodHeader("CntUniqUndirEdges");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntUniqUndirEdges on TNLoopGraph",
                TSnap::CntUniqUndirEdges(Graph));
}

////
// CntUniqDirEdges
////

void PrintCntUniqDirEdges() {
  PrintMethodHeader("CntUniqDirEdges");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntUniqDirEdges on TNLoopGraph",
                TSnap::CntUniqDirEdges(Graph));
}

////
// CntUniqBiDirEdges
////

void PrintCntUniqBiDirEdges() {
  PrintMethodHeader("CntUniqBiDirEdges");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntUniqBiDirEdges on TNLoopGraph",
                TSnap::CntUniqBiDirEdges(Graph));
}

////
// CntSelfEdges
////

void PrintCntSelfEdges() {
  PrintMethodHeader("CntSelfEdges");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling CntSelfEdges on TNLoopGraph",
                TSnap::CntSelfEdges(Graph));
}

////
// GetUnDir
////

void PrintGetUnDir() {
  PrintMethodHeader("GetUnDir");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintGraph("Calling GetUnDir on TNLoopGraph",
             TSnap::GetUnDir(Graph));
}

////
// MakeUnDir
////

void PrintMakeUnDir() {
  PrintMethodHeader("MakeUnDir");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);

  TSnap::MakeUnDir(Graph);
  PrintGraph("Calling MakeUnDir on TNLoopGraph", Graph);
}

////
// AddSelfEdges
////

void PrintAddSelfEdges() {
  PrintMethodHeader("AddSelfEdges");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);

  TSnap::AddSelfEdges(Graph);
  PrintGraph("Calling AddSelfEdges on TNLoopGraph", Graph);
}

////
// DelSelfEdges
////

void PrintDelSelfEdges() {
  PrintMethodHeader("DelSelfEdges");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);

  TSnap::DelSelfEdges(Graph);
  PrintGraph("Calling DelSelfEdges on TNLoopGraph", Graph);
}

////
// DelNodes
////

void PrintDelNodes() {
  PrintMethodHeader("DelNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);

  TIntV TNDelNodes;
  TNDelNodes.Add(0);

  TSnap::DelNodes(Graph, TNDelNodes);
  PrintGraph("Calling DelNodes on TNLoopGraph", Graph);
}

////
// DelZeroDegNodes
////

void PrintDelZeroDegNodes() {
  PrintMethodHeader("DelZeroDegNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);

  TSnap::DelZeroDegNodes(Graph);
  PrintGraph("Calling DelZeroDegNodes on TNLoopGraph", Graph);
}

////
// DelDegKNodes
////

void PrintDelDegKNodes() {
  PrintMethodHeader("DelDegKNodes");

  PNGraph Graph = GetTNLoopGraph();

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);

  TSnap::DelDegKNodes(Graph, 1, 1);
  PrintGraph("Calling DelDegKNodes on TNLoopGraph", Graph);
}

////
// IsTree
////

void PrintIsTree() {
  PrintMethodHeader("IsTree");

  PNGraph Graph = GetTNLoopGraph();
  int RootNId;

  PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  PrintIntValue("Calling IsTree on TNLoopGraph", TSnap::IsTree(Graph, RootNId));
}

////
// GetTreeRootNId
////

void PrintGetTreeRootNId() {
  PrintMethodHeader("GetTreeRootNId");

  PNGraph Graph = GetTNLoopGraph();

  // OP RS 2014/06/12 commented out, needs more investigation
  //PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  //PrintIntValue("Calling GetTreeRootNId on TNLoopGraph",
  //              TSnap::GetTreeRootNId(Graph));
}

////
// GetTreeSig
////

void PrintGetTreeSig() {
  PrintMethodHeader("GetTreeSig");

  PNGraph Graph = GetTNLoopGraph();

  // OP RS 2014/06/12 commented out, needs more investigation
  //TIntV Sig;
  //int RootNId;

  //TSnap::GetTreeSig(Graph, RootNId, Sig);

  //PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  //PrintTIntVValues("Calling GetTreeSig on TNLoopGraph",
  //                 Sig);
}

////
// GetTreeSig with NodeMap
////

void PrintGetTreeSigWithNodeMap() {
  PrintMethodHeader("GetTreeSig with NodeMap");

  PNGraph Graph = GetTNLoopGraph();

  // OP RS 2014/06/12 commented out, needs more investigation
  //TIntV Sig;
  //TIntPrV NodeMap;
  //int RootNId;
  //TSnap::GetTreeSig(Graph, RootNId, Sig, NodeMap);

  //PrintGraph("TNGraph of loop of nodes (with two edges from 4 to 0)", Graph);
  //PrintTIntVValues("Sig from calling GetTreeSig on TNLoopGraph",
  //                 Sig);
  //PrintTIntPrVValues("NodeMap from calling GetTreeSig on TNLoopGraph",
  //                   NodeMap);
}

/////////////////////////////////////////////////
// Helper Methods
/////////////////////////////////////////////////

void GenerateNodes(const PNGraph& Graph, int NumNodes = 5) {
  for (int i = 0; i < NumNodes; i++) {
    Graph->AddNode(i);
  }
}

// Returns the following graph:
// 0 -> 1 -> 2 -> 3 -> 4
//  \-----------------/ (two edges from 4 to 0 and one edge from 0 to 4)
PNGraph GenerateLoopGraph(const PNGraph& Graph) {
  GenerateNodes(Graph);
  Graph->AddEdge(0, 1);
  Graph->AddEdge(1, 2);
  Graph->AddEdge(2, 3);
  Graph->AddEdge(3, 4);
  Graph->AddEdge(4, 0);
  Graph->AddEdge(4, 0);
  Graph->AddEdge(0, 4);
  return Graph;
}

PNGraph GetTNLoopGraph() {
  return GenerateLoopGraph(TNGraph::New());
}

/////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////

int main(int argc, char* argv[]) {
  PrintCntInDegNodes();
  PrintCntOutDegNodes();
  PrintCntDegNodes();
  PrintCntNonZNodes();
  PrintCntEdgesToSet();

  PrintGetMxDegNId();
  PrintGetMxInDegNId();
  PrintGetMxOutDegNId();

  PrintGetInDegCntInt();
  PrintGetInDegCntFlt();
  PrintGetOutDegCntInt();
  PrintGetOutDegCntFlt();
  PrintGetDegCntInt();
  PrintGetDegCntFlt();
  PrintGetDegSeqV();
  PrintGetDegSeqVWithInOutDegV();

  PrintGetNodeInDegV();
  PrintGetNodeOutDegV();

  PrintCntUniqUndirEdges();
  PrintCntUniqDirEdges();
  PrintCntUniqBiDirEdges();
  PrintCntSelfEdges();

  PrintGetUnDir();
  PrintMakeUnDir();
  PrintAddSelfEdges();
  PrintDelSelfEdges();

  PrintDelNodes();
  PrintDelZeroDegNodes();
  PrintDelDegKNodes();

  PrintIsTree();
  PrintGetTreeRootNId();
  PrintGetTreeSig();
  PrintGetTreeSigWithNodeMap();
}
