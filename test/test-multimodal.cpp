#include <gtest/gtest.h>

#include "Snap.h"

TEST(multimodal, AddNbrType) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  C1.AddEdge(0, 1);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  C1.AddEdge(0, 1);

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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName, false);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  C1.AddEdge(0, 0);
  
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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName, true);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  C1.AddEdge(0, 0);
  
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

TEST(multimodal, DelModeNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());

  Net->DelModeNet(ModeName1);

  EXPECT_EQ(Net->BegModeNetI(), Net->EndModeNetI());
}

TEST(multimodal, DelModeNetWithCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName, true);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  
  M1.AddNode(0);
  M2.AddNode(0);
  C1.AddEdge(0, 0);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_NE(Net->BegCrossNetI(), Net->EndCrossNetI());

  Net->DelModeNet(ModeName1);

  TStrV OutNeighbors;

  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());

  TMMNet::TModeNetI it = Net->BegModeNetI();
  it++;

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_EQ(it, Net->EndModeNetI());
  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());
}

TEST(multimodal, DelModeNetWithSameModeCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName1, CrossName, true);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  
  M1.AddNode(0);
  M1.AddNode(1);
  C1.AddEdge(0, 1);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_NE(Net->BegCrossNetI(), Net->EndCrossNetI());

  Net->DelModeNet(ModeName1);

  EXPECT_EQ(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());
}

TEST(multimodal, ClrCrossDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

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
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

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

TEST(multimodal, GetSubgraph) {
  int NNodes = 1000;
  int NEdges = 1000;

  PMMNet Graph;
  Graph = PMMNet::New();

  // Add mode2
  TStr TestMode1("TestMode1");
  Graph->AddModeNet(TestMode1);
  TInt TestModeId1 = Graph->GetModeId(TestMode1);
  TStr TestMode2("TestMode2");
  Graph->AddModeNet(TestMode2);
  TInt TestModeId2 = Graph->GetModeId(TestMode2);

  // Add crossnets
  TStr TestCross1("TestCross1");
  Graph->AddCrossNet(TestMode1, TestMode1, TestCross1, true);
  Graph->GetCrossId(TestCross1);
  TStr TestCross2("TestCross2");
  Graph->AddCrossNet(TestModeId1, TestModeId1, TestCross2, false);
  TInt TestCrossId2 = Graph->GetCrossId(TestCross2);
  TStr TestCross3("TestCross3");
  Graph->AddCrossNet(TestMode1, TestMode2, TestCross3, true);
  TStr TestCross4("TestCross4");
  Graph->AddCrossNet(TestModeId1, TestModeId2, TestCross4, false);
  TInt TestCrossId4 = Graph->GetCrossId(TestCross4);

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

  // Get subgraph
  TStrV CrossNets;
  CrossNets.Add(TestCross1);
  PMMNet Subgraph = Graph->GetSubgraphByCrossNet(CrossNets);
  EXPECT_EQ(1, Subgraph->GetModeNets());
  EXPECT_EQ(1, Subgraph->GetCrossNets());
  TModeNet& M1 = Subgraph->GetModeNetByName(TestMode1);
  
  // Get neighbor types
  TStrV M1Names;
  M1.GetCrossNetNames(M1Names);
  EXPECT_EQ(1, M1Names.Len());
  
  // Get Neighbors for node 0
  TIntV Neighbors;
  M1.GetNeighborsByCrossNet(0, TestCross1, Neighbors);
  EXPECT_EQ(1, Neighbors.Len());
}

TEST(multimodal, ToNetwork) {
    int NNodes = 1000;
  int NEdges = 1000;

  PMMNet Graph;
  Graph = PMMNet::New();

  // Add mode2
  TStr TestMode1("TestMode1");
  Graph->AddModeNet(TestMode1);
  TInt TestModeId1 = Graph->GetModeId(TestMode1);
  TStr TestMode2("TestMode2");
  Graph->AddModeNet(TestMode2);
  TInt TestModeId2 = Graph->GetModeId(TestMode2);

  // Add crossnets
  TStr TestCross1("TestCross1");
  Graph->AddCrossNet(TestMode1, TestMode1, TestCross1, true);
  TInt TestCrossId1 = Graph->GetCrossId(TestCross1);
  TStr TestCross2("TestCross2");
  Graph->AddCrossNet(TestModeId1, TestModeId1, TestCross2, false);
  TInt TestCrossId2 = Graph->GetCrossId(TestCross2);
  TStr TestCross3("TestCross3");
  Graph->AddCrossNet(TestMode1, TestMode2, TestCross3, true);
  TInt TestCrossId3 = Graph->GetCrossId(TestCross3);
  TStr TestCross4("TestCross4");
  Graph->AddCrossNet(TestModeId1, TestModeId2, TestCross4, false);
  TInt TestCrossId4 = Graph->GetCrossId(TestCross4);

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

  // Convert to TNEANet
  TIntV CrossNetIds;
  CrossNetIds.Add(TestCrossId1);
  CrossNetIds.Add(TestCrossId2);
  CrossNetIds.Add(TestCrossId3);
  CrossNetIds.Add(TestCrossId4);
  TVec<TTriple<TInt, TStr, TStr> > NodeAttrMapping; //Triples of (ModeId, OldAttrName, NewAttrName)
  TVec<TTriple<TInt, TStr, TStr> > EdgeAttrMapping; //Triples of (CrossId, OldAttrName, NewAttrName)
  PNEANet Net = Graph->ToNetwork(CrossNetIds, NodeAttrMapping, EdgeAttrMapping);
  EXPECT_EQ(NNodes*2, Net->GetNodes());
  EXPECT_EQ(NEdges*6, Net->GetEdges()); //undirected has 2*NEdges edges, one in each direction
}