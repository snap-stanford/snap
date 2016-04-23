#include "Snap.h"

// Print graph statistics
void PrintMMNetStats(const char s[], PMMNet Graph) {
  printf("graph %s, modes %d, crossnets %d\n",
      s, Graph->GetModeNets(), Graph->GetCrossNets());
}

// Print graph statistics
void PrintGStats(const char s[], TModeNet Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph.GetNodes(), Graph.GetEdges(),
      Graph.Empty() ? "yes" : "no");
}

// Print graph statistics
void PrintGStats(const char s[], TCrossNet Graph) {
  printf("graph %s, edges %d\n",
      s, Graph.GetEdges());
}

// Print graph statistics
void PrintGStats(const char s[], PNEANet Graph) {
  printf("graph %s, nodes %d, edges %d, empty %s\n",
      s, Graph->GetNodes(), Graph->GetEdges(),
      Graph->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  PMMNet Graph;

  Graph = PMMNet::New();
  PrintMMNetStats("DefaultConstructor:Graph",Graph);
}

// Create TMMNet, add modes, crossnets, create subgraphs, covert to TNEANet
void ManipulateMMNet() {
  int NNodes = 1000;
  int NEdges = 1000;
  // Create a multimodal network
  PMMNet Graph;
  Graph = PMMNet::New();
  PrintMMNetStats("Empty MMNet",Graph);

  // Add mode
  TStr TestMode1("TestMode1");
  Graph->AddModeNet(TestMode1);
  TInt TestModeId1 = Graph->GetModeId(TestMode1);

  // Add same-mode crossnet, directed
  TStr TestCross1("TestCross1");
  Graph->AddCrossNet(TestMode1, TestMode1, TestCross1, true);
  TInt TestCrossId1 = Graph->GetCrossId(TestCross1);

  // Add same-mode crossnet, undirected
  TStr TestCross2("TestCross2");
  Graph->AddCrossNet(TestModeId1, TestModeId1, TestCross2, false);
  TInt TestCrossId2 = Graph->GetCrossId(TestCross2);

  // Add mode
  TStr TestMode2("TestMode2");
  Graph->AddModeNet(TestMode2);
  TInt TestModeId2 = Graph->GetModeId(TestMode2);

  // Add crossnet, directed
  TStr TestCross3("TestCross3");
  Graph->AddCrossNet(TestMode1, TestMode2, TestCross3, true);
  TInt TestCrossId3 = Graph->GetCrossId(TestCross3);

  // Add crossnet, undirected
  TStr TestCross4("TestCross4");
  Graph->AddCrossNet(TestModeId1, TestModeId2, TestCross4, false);
  TInt TestCrossId4 = Graph->GetCrossId(TestCross4);

  PrintMMNetStats("MMNet with modes/crossnets",Graph);

  // Add Nodes
  TModeNet& ModeNet1 = Graph->GetModeNetByName(TestMode1);
  TModeNet& ModeNet2 = Graph->GetModeNetById(TestModeId2);
  for (int i=0; i < NNodes; i++) {
    ModeNet1.AddNode(i);
    ModeNet2.AddNode(i*2);
  }

  // Add edges
  TCrossNet& CrossNet1 = Graph->GetCrossNetByName(TestCross1);
  TCrossNet& CrossNet2 = Graph->GetCrossNetById(TestCrossId2);
  TCrossNet& CrossNet3 = Graph->GetCrossNetByName(TestCross3);
  TCrossNet& CrossNet4 = Graph->GetCrossNetById(TestCrossId4);
  for (int i=0; i < NEdges; i++) {
    CrossNet1.AddEdge(i, (i+1)%NNodes, i);
    CrossNet2.AddEdge((i+5)%NNodes, i, i);
    CrossNet3.AddEdge(i, (i%NNodes)*2, i);
    CrossNet4.AddEdge((i+5)%NNodes, (i%NNodes)*2, i);
  }

  //Iterate over modes
  for (TMMNet::TModeNetI MI = Graph->BegModeNetI(); MI < Graph->EndModeNetI(); MI++) {
    PrintGStats(MI.GetModeName().GetCStr(), MI.GetModeNet());
  }

  //Iterate over crossnets
  for (TMMNet::TCrossNetI CI = Graph->BegCrossNetI(); CI < Graph->EndCrossNetI(); CI++) {
    PrintGStats(CI.GetCrossName().GetCStr(), CI.GetCrossNet());
  }

  // Get subgraph
  TStrV CrossNets;
  CrossNets.Add(TestCross1);
  PMMNet Subgraph = Graph->GetSubgraphByCrossNet(CrossNets);
  PrintMMNetStats("Subgraph", Subgraph);
  TModeNet& M1 = Subgraph->GetModeNetByName(TestMode1);
  PrintGStats("M1", M1);
  // Get neighbor types
  TStrV M1Names;
  M1.GetCrossNetNames(M1Names);
  printf("Num neighbor types %d\n", M1Names.Len());
  // Get Neighbors for node 0
  TIntV Neighbors;
  M1.GetNeighborsByCrossNet(0, TestCross1, Neighbors);
  printf("Num Neighbors %d\n", Neighbors.Len());

  // Convert to TNEANet
  TIntV CrossNetIds;
  CrossNetIds.Add(TestCrossId1);
  CrossNetIds.Add(TestCrossId2);
  CrossNetIds.Add(TestCrossId3);
  CrossNetIds.Add(TestCrossId4);
  TVec<TTriple<TInt, TStr, TStr> > NodeAttrMapping; //Triples of (ModeId, OldAttrName, NewAttrName)
  TVec<TTriple<TInt, TStr, TStr> > EdgeAttrMapping; //Triples of (CrossId, OldAttrName, NewAttrName)
  PNEANet Net = Graph->ToNetwork(CrossNetIds, NodeAttrMapping, EdgeAttrMapping);
  PrintGStats("TNEANet", Net);
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateMMNet();
}

