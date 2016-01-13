#include <gtest/gtest.h>

#include "Snap.h"

TEST(TMMNet, DefaultConstructor) {
  PMMNet Net;
  Net = TMMNet::New();
}

TEST(TMMNet, AddMode) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
}

TEST(TMMNet, AddLinkType) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TStr LinkName("TestLink");
  Net->AddLinkType(ModeName1, ModeName2, LinkName);
}

TEST(TMMNet, GetModeId) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt ModeId1(0);
  TInt ModeId2(1);
  EXPECT_EQ(ModeId1, Net->GetModeId(ModeName1));
  EXPECT_EQ(ModeId2, Net->GetModeId(ModeName2));
}

TEST(TMMNet, GetModeName) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt ModeId1(0);
  TInt ModeId2(1);
  EXPECT_EQ(ModeName1, Net->GetModeName(ModeId1));
  EXPECT_EQ(ModeName2, Net->GetModeName(ModeId2));
}

TEST(TMMNet, GetLinkId) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt LinkId1(0);
  TInt LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddLinkType(ModeName1, ModeName2, LinkName1);
  TStr LinkName2("TestLink2");
  Net->AddLinkType(ModeName1, ModeName2, LinkName2);
  EXPECT_EQ(LinkId1, Net->GetLinkId(LinkName1));
  EXPECT_EQ(LinkId2, Net->GetLinkId(LinkName2));
}

TEST(TMMNet, GetLinkName) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt LinkId1(0);
  TInt LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddLinkType(ModeName1, ModeName2, LinkName1);
  TStr LinkName2("TestLink2");
  Net->AddLinkType(ModeName1, ModeName2, LinkName2);
  EXPECT_EQ(LinkName1, Net->GetLinkName(LinkId1));
  EXPECT_EQ(LinkName2, Net->GetLinkName(LinkId2));
}

TEST(TMMNet, GetModeNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TInt ModeId1(0);
  Net->AddMode(ModeName1);
  TModeNet& M1 = Net->GetModeNet(ModeName1);
  TModeNet& M2 = Net->GetModeNet(ModeId1);
  EXPECT_EQ(0, M2.GetNodes());
  M1.AddNode(0);
  EXPECT_EQ(1, M2.GetNodes());
}