#include <gtest/gtest.h>

#include "Snap.h"

TEST(multimodal, AddNbrType) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  TStrV M1Names;
  TStrV M2Names;
  M1.GetCrossNetNames(M1Names);
  EXPECT_EQ(M1Names.Len(), 1);
  EXPECT_EQ(M1Names[0], CrossName);
  M2.GetCrossNetNames(M2Names);
  EXPECT_EQ(M2Names.Len(), 1);
  EXPECT_EQ(M2Names[0], CrossName);

  EXPECT_EQ(Net->GetModeId(ModeName1), C1.GetMode1());
  EXPECT_EQ(Net->GetModeId(ModeName2), C1.GetMode2());
}

TEST(multimodal, CheckNeighborsDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);
}

TEST(multimodal, CheckNeighborsSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);
}

TEST(multimodal, CheckNeighborsSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);
}


TEST(multimodal, DelEdgeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.DelEdge(EId);
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelEdgeSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.DelEdge(EId);
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelEdgeSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.DelEdge(EId);
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode1Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.DelNode(0);

  OutNeighbors.Clr();
  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode1SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.DelNode(0);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode1SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.DelNode(0);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}


TEST(multimodal, DelNode2Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M2.DelNode(0);

  OutNeighbors.Clr();
  EXPECT_EQ(0, M2.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode2SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.DelNode(1);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode2SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.DelNode(1);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}


TEST(multimodal, DelCrossNetSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);

  TStrV InNeighbors;
  TStrV OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelCrossNetSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);

  TStrV InNeighbors;
  TStrV OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(2, InNeighbors.Len());
  EXPECT_EQ(2, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(2, InNeighbors.Len());
  EXPECT_EQ(2, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelCrossNetDifModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, false, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  
  TStrV InNeighbors;
  TStrV OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelCrossNetDifModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, true, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  
  TStrV InNeighbors;
  TStrV OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelMode) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());

  Net->DelMode(ModeName1);

  EXPECT_EQ(Net->BegModeNetI(), Net->EndModeNetI());
}

TEST(multimodal, DelModeWithCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, true, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  
  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_NE(Net->BegCrossNetI(), Net->EndCrossNetI());

  Net->DelMode(ModeName1);

  TStrV OutNeighbors;

  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());

  TMMNet::TModeNetI it = Net->BegModeNetI();
  it++;

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_EQ(it, Net->EndModeNetI());
  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());
}

TEST(multimodal, DelModeWithSameModeCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName1, true, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_NE(Net->BegCrossNetI(), Net->EndCrossNetI());

  Net->DelMode(ModeName1);

  EXPECT_EQ(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());
}








TEST(multimodal, ClrCrossDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.Clr();
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrCrossSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.Clr();
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrCrossSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.Clr();
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrMode1Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.Clr();

  OutNeighbors.Clr();
  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrMode1SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}

TEST(multimodal, ClrMode1SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}


TEST(multimodal, ClrMode2Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName1);
  Net->AddMode(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int EId = C1.AddEdge(0, 0);
  TIntV InNeighbors;
  TIntV OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M2.Clr();

  OutNeighbors.Clr();
  EXPECT_EQ(0, M2.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrMode2SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, true, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);


  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}

TEST(multimodal, ClrMode2SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, false, CrossName);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int EId = C1.AddEdge(0, 1);

  TIntV InNeighbors;
  TIntV OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}