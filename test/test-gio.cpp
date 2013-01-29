#include <gtest/gtest.h>

#include "Snap.h"
#include "test-helper.h"

class GIOTest { };  // For gtest highlighting

using namespace TSnap;

// Function for testing saving / loading of directed, undirected and multi-graphs, where node names are ids
template <class PGraph>
void TestEdgeList() {
  
  const int NNodes = 10000;
  const int NEdges = 100000;

  const char *FName = "test.graph.dat";
  const char *Desc = "Randomly generated graph for input/output.";
  
  PGraph GOut, GIn;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  // Output node IDs as numbers
  SaveEdgeList(GOut, FName, Desc);
  
  // Test loading of edge list
  EXPECT_TRUE(fileExists(FName));
  EXPECT_FALSE(GOut->Empty());

  GIn = LoadEdgeList<PGraph>(FName);

  // Verify all nodes exist in input and output graphs (and no more)
  THashSet<TInt> OutNIdH, InNIdH;
  
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    // Nodes that do not have edges are left off during input
    if (NI.GetDeg() > 0) {
      OutNIdH.AddKey(NI.GetId());
    }
  }
  for (typename PGraph::TObj::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    InNIdH.AddKey(NI.GetId());
  }
  EXPECT_TRUE(OutNIdH == InNIdH);
  
  // Verify all edges in both input and output graphs
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GIn->IsEdge(Src, Dst));
//    printf("gout edge %d -> %d\n", Src.Val, Dst.Val);
  }
  for (typename PGraph::TObj::TEdgeI EI = GIn->BegEI(); EI < GIn->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GOut->IsEdge(Src, Dst));
//    printf("gin edge %d -> %d\n", Src.Val, Dst.Val);
  }
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());
    
}

// Tests saving and loading of undirected and directed graphs
TEST(GIOTest, SaveLoadEdgeList) {
  
  // Undirected graph - save and loading
  TestEdgeList<PUNGraph>();

  // Directed graph - save and loading
  TestEdgeList<PNGraph>();

  // Multi graph - save and loading
  TestEdgeList<PNEGraph>();
 
}

// Function for testing saving / loading of directed, undirected and multi-graphs, where node names are strings
template <class PGraph>
void TestEdgeListStr() {

  const int NNodes = 1000;
  const int NEdges = 10000;
  
  const char *FName = "test.graph.dat";
  
  PGraph GOut, GIn;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  // Output nodes as random strings
  TIntStrH OutNIdStrH;
  TStrHash<TInt> OutStrNIdH;
  
  // Generate unique random strings for graph
  TStr RandStr;
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    do {
      RandStr.Clr();
      TInt RandLen = TInt::Rnd.GetUniDevInt(5, 30);
      for (int i = 0; i < RandLen; i++) {
        TStr RandChar(TInt::Rnd.GetUniDevInt(33, 126));
        RandStr += RandChar;
      }
    }
    while (OutStrNIdH.IsKey(RandStr) || RandStr[0] == '#'); // Not unique or starts with comment
    OutNIdStrH.AddDat(NI.GetId(), RandStr);
    OutStrNIdH.AddDat(RandStr, NI.GetId());
  }
  
  // Create graph file
  FILE *F = fopen(FName, "w");
  //  printf("GOut->GetNodes() = %d\n", GOut->GetNodes());
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    fprintf(F, "%s %s\n", OutNIdStrH[Src].CStr(), OutNIdStrH[Dst].CStr());
  }
  fclose(F);
  
  // Test loading of edge list
  ASSERT_TRUE(fileExists(FName));
  EXPECT_FALSE(GOut->Empty());
  
  TStrHash<TInt> InStrToNIdH;
  GIn = LoadEdgeListStr<PGraph>(FName, 0, 1, InStrToNIdH);
//  printf("InStrToNIdH.Len() = %d, GIn->Edges=%d, GOut->GetEdges()=%d\n", InStrToNIdH.Len(), GIn->GetEdges(), GOut->GetEdges());
  ASSERT_TRUE(InStrToNIdH.Len() == GOut->GetNodes());
  
  // Verify all the edges read-in are in the original graph
  for (typename PGraph::TObj::TEdgeI EI = GIn->BegEI(); EI < GIn->EndEI(); EI++) {
    TStr SrcStr = InStrToNIdH.GetKey(EI.GetSrcNId());
    TStr DstStr = InStrToNIdH.GetKey(EI.GetDstNId());
    TInt OutSrc = OutStrNIdH.GetKeyId(SrcStr);
    TInt OutDst = OutStrNIdH.GetKeyId(DstStr);
    ASSERT_TRUE(GOut->IsEdge(OutSrc, OutDst));
  }
  
  // Verify all the edges in the original graph are read-in
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TStr SrcStr = OutStrNIdH.GetKey(EI.GetSrcNId());
    TStr DstStr = OutStrNIdH.GetKey(EI.GetDstNId());
    TInt InSrc = InStrToNIdH.GetKeyId(SrcStr);
    TInt InDst = InStrToNIdH.GetKeyId(DstStr);
    ASSERT_TRUE(GIn->IsEdge(InSrc, InDst));
  }
  
  EXPECT_TRUE(GIn->GetNodes() == GOut->GetNodes());
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());
  
}

// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 edge per line (whitespace separated columns, arbitrary string node ids).
TEST(GIOTest, SaveLoadEdgeListStr) {
  
  // Undirected graph - save and loading
  TestEdgeListStr<PUNGraph>();
  
  // Directed graph - save and loading
  TestEdgeListStr<PNGraph>();
  
  // Multi graph - save and loading
  TestEdgeListStr<PNEGraph>();

}

// Function for testing saving / loading of directed, undirected and multi-graphs, where node names are strings
template <class PGraph>
void TestConnList() {

  const int NNodes = 10000;
  const int NEdges = 100000;
  
  const char *FName = "test.graph.dat";
  
  PGraph GOut, GIn;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  // Create graph file
  FILE *F = fopen(FName, "w");
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    fprintf(F, "%d", NI.GetId());
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      fprintf(F, " %d", NI.GetOutNId(i));
    }
    fprintf(F, "\n");
  }
  fclose(F);
  
  // Test loading of edge list
  ASSERT_TRUE(fileExists(FName));
  EXPECT_FALSE(GOut->Empty());
  
  TStrHash<TInt> InStrToNIdH;
  GIn = LoadConnList<PGraph>(FName);
  
  // Verify all nodes exist in input and output graphs (and no more)
  THashSet<TInt> OutNIdH, InNIdH;
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    // Nodes that do not have edges are left off during input
    if (NI.GetDeg() > 0) {
      OutNIdH.AddKey(NI.GetId());
    }
  }
  for (typename PGraph::TObj::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    InNIdH.AddKey(NI.GetId());
  }
  EXPECT_TRUE(OutNIdH == InNIdH);
  EXPECT_TRUE(GIn->GetNodes() == GOut->GetNodes());
  
  // Verify all edges in both input and output graphs
  
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GIn->IsEdge(Src, Dst));
  }
  
  for (typename PGraph::TObj::TEdgeI EI = GIn->BegEI(); EI < GIn->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GOut->IsEdge(Src, Dst));
  }
  
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());
  
}

// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 node and all its edges in a single line.
TEST(GIOTest, LoadConnList) {
  
  // Undirected graph - save and loading
  TestConnList<PUNGraph>();
  
  // Directed graph - save and loading
  TestConnList<PNGraph>();
  
  // Multi graph - save and loading
  TestConnList<PNEGraph>();
}

// Function for testing saving / loading of directed, undirected and multi-graphs, where node names are strings
template <class PGraph>
void TestConnListStr() {

  const int NNodes = 10000;
  const int NEdges = 100000;
  
  const char *FName = "test.graph.dat";
  
  PGraph GOut, GIn;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  // Output nodes as random strings
  TIntStrH OutNIdStrH;
  TStrHash<TInt> OutStrNIdH;
  
  // Generate unique random strings for graph
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
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
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    fprintf(F, "%s", OutNIdStrH[NI.GetId()].CStr());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      fprintf(F, " %s", OutNIdStrH[NI.GetOutNId(e)].CStr());
    }
    fprintf(F, "\n");
  }
  fclose(F);
  
  // Test loading of edge list
  ASSERT_TRUE(fileExists(FName));
  EXPECT_FALSE(GOut->Empty());
  
  TStrHash<TInt> InStrToNIdH;
  GIn = LoadConnListStr<PGraph>(FName, InStrToNIdH);
//  printf("InStrToNIdH.Len() = %d, GIn->Edges=%d, GOut->GetEdges()=%d\n", InStrToNIdH.Len(), GIn->GetEdges(), GOut->GetEdges());
  ASSERT_TRUE(InStrToNIdH.Len() == GOut->GetNodes());
  
  // Verify all the edges read-in are in the original graph
  for (typename PGraph::TObj::TEdgeI EI = GIn->BegEI(); EI < GIn->EndEI(); EI++) {
    TStr SrcStr = InStrToNIdH.GetKey(EI.GetSrcNId());
    TStr DstStr = InStrToNIdH.GetKey(EI.GetDstNId());
    TInt OutSrc = OutStrNIdH.GetKeyId(SrcStr);
    TInt OutDst = OutStrNIdH.GetKeyId(DstStr);
//    printf("gin: %s (%d) -> %s (%d) \n", SrcStr.CStr(), OutSrc.Val, DstStr.CStr(), OutDst.Val);
    EXPECT_TRUE(GOut->IsEdge(OutSrc, OutDst));
  }
  
  // Verify all the edges in the original graph are read-in
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TStr SrcStr = OutStrNIdH.GetKey(EI.GetSrcNId());
    TStr DstStr = OutStrNIdH.GetKey(EI.GetDstNId());
    TInt InSrc = InStrToNIdH.GetKeyId(SrcStr);
    TInt InDst = InStrToNIdH.GetKeyId(DstStr);
//    printf("gout: %s (%d) -> %s (%d)\n", SrcStr.CStr(), InSrc.Val, DstStr.CStr(), InDst.Val);
    EXPECT_TRUE(GIn->IsEdge(InSrc, InDst));
  }
  
  EXPECT_TRUE(GIn->GetNodes() == GOut->GetNodes());
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());  
}

// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 node and all its edges in a single line.
TEST(GIOTest, LoadConnListStr) {
  
  //  // Undirected graph - save and loading
  TestConnListStr<PUNGraph>();
  
  // Directed graph - save and loading
  TestConnListStr<PNGraph>();
  
  // Multi graph - save and loading
  TestConnListStr<PNEGraph>();
  
}

// Function for testing saving / loading of directed, undirected and multi-graphs, where node names are strings
template <class PGraph>
void TestPajek() {
  
  const int NNodes = 10000;
  const int NEdges = 100000;
  
  const char *FName = "test.paj.dat";
  
  PGraph GOut, GIn;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  // Output node IDs as numbers
  SavePajek(GOut, FName);
  
  // Test loading of edge list
  EXPECT_TRUE(fileExists(FName));
  EXPECT_FALSE(GOut->Empty());
  
  GIn = LoadPajek<PGraph>(FName);
  
  // Verify all nodes exist in input and output graphs (and no more)
  TIntV OutNIdV, InNIdV;
  GOut->GetNIdV(OutNIdV);
  
  // Pajek graphs start with node IDs of 1, so add 1 to SNAP graph
  for (int i = 0; i < OutNIdV.Len(); i++) {
    OutNIdV[i]++;
  }
  GIn->GetNIdV(InNIdV);
  EXPECT_TRUE(OutNIdV == InNIdV);

  // Verify all edges are the same in both input and output graphs
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GIn->IsEdge(Src+1, Dst+1));
//    printf("gout edge %d -> %d\n", Src.Val+1, Dst.Val+1);
  }
  
  for (typename PGraph::TObj::TEdgeI EI = GIn->BegEI(); EI < GIn->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GOut->IsEdge(Src-1, Dst-1));
//    printf("gin edge %d -> %d\n", Src.Val, Dst.Val);
  }
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());
  
}

// Loads a (directed, undirected or multi) graph from Pajek .PAJ format file.
// Function supports both the 1 edge per line (<source> <destination> <weight>)
// as well as the 1 node per line (<source> <destination1> <destination2> ...) formats.
TEST(GIOTest, SaveLoadPajek) {
  
  // Undirected graph - save and loading
  TestPajek<PUNGraph>();
  
  // Directed graph - save and loading
  TestPajek<PNGraph>();
  
  // Multi graph - save and loading
  TestPajek<PNEGraph>();

}

// Function for testing saving / loading of directed, undirected and multi-graphs, where node names are strings
template <class PGraph>
void TestMatlabSparseMtx() {

  const int NNodes = 10000;
  const int NEdges = 100000;
  
  const char *FName = "test.graph.dat";
  
  PGraph GOut, GIn;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  SaveMatlabSparseMtx(GOut, FName);
    
  GIn = PGraph::TObj::New();
  GIn->Reserve(NNodes, NEdges);
  
  // Read-in Matlab-file
  FILE *F = fopen(FName, "r");
  while (! feof(F)) {
    int Src, Dst, Edge;
    EXPECT_TRUE(fscanf(F, "%d %d %d\n", &Src, &Dst, &Edge) == 3);
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
  
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    // Nodes that do not have edges are left off during input
    if (NI.GetDeg() > 0) {
      OutNIdH.AddKey(NI.GetId());
    }
  }
  for (typename PGraph::TObj::TNodeI NI = GIn->BegNI(); NI < GIn->EndNI(); NI++) {
    InNIdH.AddKey(NI.GetId());
  }
  
  EXPECT_TRUE(OutNIdH == InNIdH);
  
  // Verify all edges in both input and output graphs
  for (typename PGraph::TObj::TEdgeI EI = GOut->BegEI(); EI < GOut->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GIn->IsEdge(Src, Dst));
    //    printf("gout edge %d -> %d\n", Src.Val, Dst.Val);
  }
  for (typename PGraph::TObj::TEdgeI EI = GIn->BegEI(); EI < GIn->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(GOut->IsEdge(Src, Dst));
    //    printf("gin edge %d -> %d\n", Src.Val, Dst.Val);
  }
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());

}

// Saves a graph in a MATLAB sparse matrix format.
TEST(GIOTest, SaveMatlabSparseMtx) {
  
  // Undirected graph - save and loading
  TestMatlabSparseMtx<PUNGraph>();

  // Directed graph - save and loading
  TestMatlabSparseMtx<PNGraph>();
  
  // Multi graph - save and loading
  TestMatlabSparseMtx<PNEGraph>();
  
}

// Function for testing saving / loading directed, undirected and multi-graphs
// in GraphVizp .DOT format
template <class PGraph>
void TestSaveGViz() {

  const int NNodes = 5;
  const int NEdges = 8;

//  const int NNodes = 10000;
//  const int NEdges = 100000;
  
  const char *FName1 = "test1.dot.dat", *FName2 = "test2.dot.dat";
  const char *Desc = "Randomly generated GgraphVizp for input/output.";

  PGraph GOut;
  GOut = GenRndGnm<PGraph>(NNodes, NEdges);
  
  SaveGViz(GOut, FName1);
  EXPECT_TRUE(fileExists(FName1));

  // Output node IDs as numbers
  TIntStrH NIdLabelH;

  // Generate labels for random graph
  for (typename PGraph::TObj::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    NIdLabelH.AddDat(NI.GetId(), TStr::Fmt("Node%d", NI.GetId()));
    
  }

  SaveGViz(GOut, FName2, Desc, NIdLabelH);
  
  // Test loading of edge list
  EXPECT_TRUE(fileExists(FName2));
  EXPECT_FALSE(GOut->Empty());
  
}


// Save a graph in GraphVizp .DOT format.
// @param NIdLabelH Maps node ids to node string labels.
TEST(GIOTest, SaveGViz) {
  
  // Undirected graph - save and loading
  TestSaveGViz<PUNGraph>();
  
  // Directed graph - save and loading
  TestSaveGViz<PNGraph>();
  
  // Multi graph - save and loading
  TestSaveGViz<PNEGraph>();

}

// Tests loading directed networks in the DyNetML format. Loads all the networks in the file FNm.
TEST(GIOTest, LoadDyNet) {
  
  const int NNodes = 10000;
  const int NEdges = 100000;
  
  const char *FName = "test.xml.dat";
  
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
  
  // Verify all nodes exist in input and output graphs (and no more)
  EXPECT_TRUE(GIn->GetNodes() == GOut->GetNodes());
  
  // Verify all degree counts are the same in both input and output graphs
  TIntV GOutInDegV, GOutOutDegV, GInInDegV, GInOutDegV;
  GetDegSeqV(GOut, GOutInDegV, GOutOutDegV);
  GetDegSeqV(GOut, GInInDegV, GInOutDegV);
  
  EXPECT_TRUE(GOutInDegV == GInInDegV);
  EXPECT_TRUE(GOutOutDegV == GInOutDegV);
  
  EXPECT_TRUE(GIn->GetEdges() == GOut->GetEdges());
  
}

