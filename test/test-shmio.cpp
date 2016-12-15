#include <gtest/gtest.h>

#include "Snap.h"
#include "test-helper.h"

class SHMTest { };  // For gtest highlighting

using namespace TSnap;

template <class PGraph>
void checkGraphCorrect(PGraph saved, PGraph loaded) {
  EXPECT_EQ(saved->GetNodes(), loaded->GetNodes());
  EXPECT_EQ(saved->GetEdges(), loaded->GetEdges());
  for (typename PGraph::TObj::TEdgeI EI = saved->BegEI(); EI < saved->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(loaded->IsEdge(Src, Dst));
  }
}

template <class PGraph>
PGraph writeGraph(TStr filename) {
  int num_nodes = 100;
  PGraph g = PGraph::TObj::New();
  for (int i = 0; i < num_nodes; i++) {
    g->AddNode(i);
  }
  for (int i = 1; i < num_nodes - 1; i++) {
    g->AddEdge(i, i+1);
    g->AddEdge(i, i-1);
  }
  TFOut outstream(filename);
  g->Save(outstream);
  return g;
}

template <class PGraph>
PGraph readGraph(TStr filename) {
  TShMIn ShMIn(filename);
  PGraph g = PGraph::TObj::LoadShM(ShMIn);
  return g;
}

template <class PGraph>
void checkGraph() {
  TStr filename("test.graph");
  PGraph g = writeGraph<PGraph>(filename);
  PGraph g2 = readGraph<PGraph>(filename);
  checkGraphCorrect<PGraph>(g, g2);
}

// Tests saving and loading of undirected and directed graphs
TEST(SHMTest, LoadGraphs) {
  // Undirected graph
  checkGraph<PUNGraph>();
  // Directed graph
  checkGraph<PNGraph>();
}

PNEANet writeTNEANet(TStr filename) {
  int NNodes = 100;
  int NEdges = 100;
  PNEANet Graph = TNEANet::New();
  int i;
  for (i = NNodes - 1; i >= 0; i--) {
    Graph->AddNode(i);
  }
  // create the edges 
  int x, y;
  for (i = NEdges - 1; i >= 0; i--) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    Graph->AddEdge(x, y, i);
  }

  // create attributes and fill all nodes
  TStr attr1 = "str";
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr4 = "default";

  Graph->AddIntAttrDatN(3, 3*2, attr2);
  Graph->AddIntAttrDatN(50, 50*2, attr2);

  Graph->AddFltAttrDatN(5, 3.41, attr3);
  Graph->AddFltAttrDatN(50, 2.718, attr3);

  Graph->AddStrAttrDatN(10, "abc", attr1);
  Graph->AddStrAttrDatN(20, "def", attr1);

  TFOut outstream(filename);
  Graph->Save(outstream);
  return Graph;
}

void
checkTNEANetCorrect(PNEANet g, PNEANet g2) {
  EXPECT_EQ(g->GetNodes(), g2->GetNodes());
  EXPECT_EQ(g->GetEdges(), g2->GetEdges());
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr1 = "str";
  EXPECT_EQ(g->GetNAIntI(attr2, 3).GetDat(), g2->GetNAIntI(attr2, 3).GetDat());
  EXPECT_EQ(g->GetNAIntI(attr2, 50).GetDat(), g2->GetNAIntI(attr2, 50).GetDat());
  EXPECT_EQ(g->GetNAFltI(attr3, 5).GetDat(), g2->GetNAFltI(attr3, 5).GetDat());
  EXPECT_EQ(g->GetNAFltI(attr3, 50).GetDat(), g2->GetNAFltI(attr3, 50).GetDat());
  EXPECT_EQ(g->GetNAStrI(attr1, 10).GetDat().LastCh(), g2->GetNAStrI(attr1, 10).GetDat().LastCh());
  EXPECT_EQ(g->GetNAStrI(attr1, 20).GetDat().LastCh(), g2->GetNAStrI(attr1, 20).GetDat().LastCh());
}

// Tests saving and loading of undirected and directed graphs
TEST(SHMTest, LoadTNeanet) {
  TStr filename("test.graph");
  PNEANet g = writeTNEANet(filename);
  TShMIn ShMIn(filename);
  PNEANet g2 = TNEANet::LoadShM(ShMIn);
  checkTNEANetCorrect(g, g2);
}

template <class PNet>
void checkNetworkCorrect(PNet saved, PNet loaded) {
  EXPECT_EQ(saved->GetNodes(), loaded->GetNodes());
  EXPECT_EQ(saved->GetEdges(), loaded->GetEdges());
  TAttrType AttrTypeS;
  TAttrType AttrTypeL;
  TInt AttrIdS;
  TInt AttrIdL;

  saved->GetSAttrIdN(TStr("TestInt"), AttrIdS, AttrTypeS);
  loaded->GetSAttrIdN(TStr("TestInt"), AttrIdL, AttrTypeL);
  EXPECT_EQ(AttrTypeS, AttrTypeL);
  EXPECT_EQ(AttrIdS, AttrIdL);

  saved->GetSAttrIdN(TStr("TestFlt"), AttrIdS, AttrTypeS);
  loaded->GetSAttrIdN(TStr("TestFlt"), AttrIdL, AttrTypeL);
  EXPECT_EQ(AttrTypeS, AttrTypeL);
  EXPECT_EQ(AttrIdS, AttrIdL);

  saved->GetSAttrIdN(TStr("TestStr"), AttrIdS, AttrTypeS);
  loaded->GetSAttrIdN(TStr("TestStr"), AttrIdL, AttrTypeL);
  EXPECT_EQ(AttrTypeS, AttrTypeL);
  EXPECT_EQ(AttrIdS, AttrIdL);
}


template <class PNet>
PNet writeNetwork(TStr filename) {
  int NNodes = 1000;
  int NEdges = 1000;
  PNet Graph = PNet::TObj::New();
  int i;
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  int NCount = NEdges;
  int x,y;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    if (x != y  &&  !Graph->IsEdge(x,y)) {
      Graph->AddEdge(x, y);
      NCount--;
    }
  }
  TInt AttrId;
  Graph->AddSAttrN("TestInt", atInt, AttrId);
  Graph->AddSAttrN("TestFlt", atFlt, AttrId);
  Graph->AddSAttrN("TestStr", atStr, AttrId);
  TFOut outstream(filename);
  Graph->Save(outstream);
  return Graph;
}

template <class PNet>
PNet readNetwork(TStr filename) {
  TShMIn ShMIn(filename);
  PNet g = PNet::TObj::LoadShM(ShMIn);
  return g;
}

template <class PNet>
void checkNetwork() {
  TStr filename("test.graph");
  PNet g = writeNetwork<PNet>(filename);
  PNet g2 = readNetwork<PNet>(filename);
  checkNetworkCorrect<PNet>(g, g2);
}

// Tests saving and loading of undirected and directed graphs
TEST(SHMTest, LoadNetworks) {
  // Undirected network
  checkNetwork<PUndirNet>();
  // Directed network
  checkNetwork<PDirNet>();
  // only check graph functionality for the following networks
  checkGraph< TPt<TNodeNet<TInt> > >();
  checkGraph< TPt<TNodeEdgeNet<TInt, TInt> > >();
  checkGraph< TPt<TNodeEDatNet<TInt, TInt> > >();
}