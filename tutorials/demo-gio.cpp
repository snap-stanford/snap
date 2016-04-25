#include "Snap.h"

// Print graph statistics
template <class PGraph>
void PrintGStats(const char s[], PGraph Graph) {

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

using namespace TSnap;

// Save and load directed, undirected and multi-graphs as list of edges, where nodes are ids
void SaveLoadEdgeList() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName = "demo.pngraph.dat";
  const char *Desc = "Randomly generated graph for input/output.";
  
  PNGraph GOut, GIn;
  GOut = GenRndGnm<PNGraph>(NNodes, NEdges);
  
  // Output node IDs as numbers
  SaveEdgeList(GOut, FName, Desc);
  
  // Load edge list
  GIn = LoadEdgeList<PNGraph>(FName);
  
  // Verify all nodes exist in input and output graphs
  THashSet<TInt> OutNIdH, InNIdH;
  
  for (TNGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    // Nodes that do not have edges are left off during input
    if (NI.GetDeg() > 0) {
      OutNIdH.AddKey(NI.GetId());
    }
  }
  for (TNGraph::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    InNIdH.AddKey(NI.GetId());
  }
  
  PrintGStats<PNGraph>("EdgeList - Out", GOut);
  PrintGStats<PNGraph>("EdgeList - In", GIn);
  
}

// Save and load directed, undirected and multi-graphs, where node names are strings
void IOEdgeListStr() {
  
  const int NNodes = 1000;
  const int NEdges = 5000;
  
  const char *FName = "demo1.pnegraph.dat";
  
  PNEGraph GOut, GIn;      // Can also be PUNGraph or PNGraph
  GOut = GenRndGnm<PNEGraph>(NNodes, NEdges);
  
  // Output nodes as random strings
  TIntStrH OutNIdStrH;
  TStrHash<TInt> OutStrNIdH;
  
  // Generate unique random strings for graph
  TStr RandStr;
  for (TNEGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    do {
      RandStr.Clr();
      RandStr += "node-";
      TInt RandLen = TInt::Rnd.GetUniDevInt(3, 6);
      for (int i = 0; i < RandLen; i++) {
        //TStr RandChar(TInt::Rnd.GetUniDevInt(33, 126));
        TStr RandChar(TInt::Rnd.GetUniDevInt(48, 57));
        RandStr += RandChar;
      }
    }
    while (OutStrNIdH.IsKey(RandStr) || RandStr[0] == '#'); // Not unique or starts with comment
    OutNIdStrH.AddDat(NI.GetId(), RandStr);
    OutStrNIdH.AddDat(RandStr, NI.GetId());
  }
  
  // Create graph file
  FILE *F = fopen(FName, "w");
  for (TNEGraph::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    fprintf(F, "%s %s\n", OutNIdStrH[Src].CStr(), OutNIdStrH[Dst].CStr());
  }
  fclose(F);
  
  // Load edge list of strings
  TStrHash<TInt> InStrToNIdH;
  GIn = LoadEdgeListStr<PNEGraph>(FName, 0, 1, InStrToNIdH);

  PrintGStats<PNEGraph>("EdgeListStr - Out", GOut);
  PrintGStats<PNEGraph>("EdgeListStr - In", GIn);

  // Print out a mapping of node IDs to node name strings
  for (TNEGraph::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    int NId = NI.GetId();
    // Map a node ID to its string name
    TStr NameStr = InStrToNIdH.GetKey(NId);
    printf("node ID %d is string %s\n", NId, NameStr.CStr());
  }

  // Print out a mapping of name strings to node IDs
  for (TNEGraph::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    TStr NameStr = InStrToNIdH.GetKey(NI.GetId());
    // Map a node string name to its ID
    int NId = InStrToNIdH.GetKeyId(NameStr);
    printf("string %s has node ID %d\n", NameStr.CStr(), NId);
  }
}

// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 node and all its edges in a single line.
void IOConnList() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName = "demo2.pnegraph.dat";
  
  PNEGraph GOut, GIn;
  GOut = GenRndGnm<PNEGraph>(NNodes, NEdges);
  
  // Create graph file
  FILE *F = fopen(FName, "w");
  for (TNEGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    fprintf(F, "%d", NI.GetId());
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      fprintf(F, " %d", NI.GetOutNId(i));
    }
    fprintf(F, "\n");
  }
  fclose(F);
  
  // Load edge list
  GIn = LoadConnList<PNEGraph>(FName);
  
  PrintGStats("ConnList - Out", GOut);
  PrintGStats("ConnList - In", GIn);
}

// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 node and all its edges in a single line.
void IOConnListStr() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName = "demo.pungraph.dat";
  
  PUNGraph GOut, GIn;
  GOut = GenRndGnm<PUNGraph>(NNodes, NEdges);
  
  // Output nodes as random strings
  TIntStrH OutNIdStrH;
  TStrHash<TInt> OutStrNIdH;
  
  // Generate unique random strings for graph
  for (TUNGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    TStr RandStr = "";
    do {
      TInt RandLen = TInt::Rnd.GetUniDevInt(5, 10);
      for (int i = 0; i < RandLen; i++) {
        //        TStr RandChar(TInt::Rnd.GetUniDevInt(33, 126));
        TStr RandChar(TInt::Rnd.GetUniDevInt(97, 122));
        RandStr += RandChar;
      }
    }
    while (OutStrNIdH.IsKey(RandStr) || RandStr[0] == '#');
    OutNIdStrH.AddDat(NI.GetId(), RandStr);
    OutStrNIdH.AddDat(RandStr, NI.GetId());
  }
  
  // Create graph file
  FILE *F = fopen(FName, "w");
  for (TUNGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    fprintf(F, "%s", OutNIdStrH[NI.GetId()].CStr());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      fprintf(F, " %s", OutNIdStrH[NI.GetOutNId(e)].CStr());
    }
    fprintf(F, "\n");
  }
  fclose(F);
  
  TStrHash<TInt> InStrToNIdH;
  GIn = LoadConnListStr<PUNGraph>(FName, InStrToNIdH);
  
  PrintGStats("ConnListStr - Out", GOut);
  PrintGStats("ConnListStr - In", GIn);
  
}

// Loads a (directed, undirected or multi) graph from Pajek .PAJ format file.
// Function supports both the 1 edge per line (<source> <destination> <weight>)
// as well as the 1 node per line (<source> <destination1> <destination2> ...) formats.
void IOPajek() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName = "demo.paj.dat";
  
  PNEGraph GOut, GIn;
  GOut = GenRndGnm<PNEGraph>(NNodes, NEdges);
  
  // Output node IDs as numbers
  SavePajek(GOut, FName);
  
  // Load Pajek file
  GIn = LoadPajek<PNEGraph>(FName);
  
  // Verify all nodes exist in input and output graphs (and no more)
  TIntV OutNIdV, InNIdV;
  GOut->GetNIdV(OutNIdV);
  
  // Pajek graphs start with node IDs of 1, so add 1 to SNAP graph
  for (int i = 0; i < OutNIdV.Len(); i++) {
    OutNIdV[i]++;
  }
  GIn->GetNIdV(InNIdV);
  
  PrintGStats("Pajek - Out", GOut);
  PrintGStats("Pajek - In", GIn);

}

// Saves and loads a graph in a MATLAB sparse matrix format
void IOMatlabSparseMtx() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName = "demo.matlab.dat";
  
  PNGraph GOut, GIn;
  GOut = GenRndGnm<PNGraph>(NNodes, NEdges);
  
  SaveMatlabSparseMtx(GOut, FName);
  
  GIn = TNGraph::New();
  GIn->Reserve(NNodes, NEdges);
  
  // Read-in Matlab-file
  FILE *F = fopen(FName, "r");
  while (! feof(F)) {
    int Src, Dst, Edge;
    fscanf(F, "%d %d %d\n", &Src, &Dst, &Edge);
    Src--; Dst--;             // SNAP graphs start at Node Ids of 0
    if (not GIn->IsNode(Src)) {
      GIn->AddNode(Src);
    }
    if (not GIn->IsNode(Dst)) {
      GIn->AddNode(Dst);
    }
    GIn->AddEdge(Src, Dst);
    
  }
  fclose(F);
  
  // Verify all nodes exist in input and output graphs (and no more)
  THashSet<TInt> OutNIdH, InNIdH;
  
  for (TNGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    // Nodes that do not have edges are left off during input
    if (NI.GetDeg() > 0) {
      OutNIdH.AddKey(NI.GetId());
    }
  }
  for (TNGraph::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    InNIdH.AddKey(NI.GetId());
  }
  PrintGStats("Matlab - Out", GOut);
  PrintGStats("Matlab - In", GIn);
    
}

// Save directed, undirected and multi-graphs in GraphVizp .DOT format
void IOGViz() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName1 = "demo1.dot.dat", *FName2 = "demo2.dot.dat";
  const char *Desc = "Randomly generated GgraphVizp for input/output.";
  
  PNGraph GOut;     // Can be PNEGraph or PUNGraph
  GOut = GenRndGnm<PNGraph>(NNodes, NEdges);
  
  SaveGViz(GOut, FName1);
  
  // Output node IDs as numbers
  TIntStrH NIdLabelH;
  
  // Generate labels for random graph
  for (TNGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    NIdLabelH.AddDat(NI.GetId(), TStr::Fmt("Node%d", NI.GetId()));
    
  }
  SaveGViz(GOut, FName2, Desc, NIdLabelH);
  
  PrintGStats("IOGViz - In", GOut);
}

// Loads a directed network in the DyNetML format
void IODyNet() {
  
  const int NNodes = 500;
  const int NEdges = 2000;
  
  const char *FName = "demo.xml.dat";
  
  PNGraph GOut, GIn;
  GOut = GenRndGnm<PNGraph>(NNodes, NEdges);
  
  // Create XML graph file
  FILE *F = fopen(FName, "w");
  
  fprintf(F, "<network>\n");
  for (TNGraph::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    fprintf(F, "\t<link source=\"%d\" target=\"%d\"/>\n", Src.Val, Dst.Val);
  }
  fprintf(F, "</network>\n");
  fclose(F);
  
  GIn = LoadDyNet(FName);
  
  PrintGStats("DyNet - Out", GOut);
  PrintGStats("DyNet - In", GIn);
}

int main(int argc, char* argv[]) {
  SaveLoadEdgeList();
  IOEdgeListStr();
  IOConnList();
  IOConnListStr();
  IOPajek();
  IOMatlabSparseMtx();
  IOGViz();
  IODyNet();
}
