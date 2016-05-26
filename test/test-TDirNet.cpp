#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TDirNet, DefaultConstructor) {
  PDirNet Graph;

  Graph = TDirNet::New();

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());
  EXPECT_EQ(1,Graph->HasFlag(gfDirected));
}

// Test node, edge creation
TEST(TDirNet, ManipulateNodesEdges) {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "test.graph.dat";

  PDirNet Graph;
  PDirNet Graph1;
  PDirNet Graph2;
  int i;
  int n;
  int NCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Graph = TDirNet::New();
  EXPECT_EQ(1,Graph->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(NNodes,Graph->GetNodes());

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Graph->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Graph->IsEdge(x,y)) {
      n = Graph->AddEdge(x, y);
      NCount--;
    }
  }

  EXPECT_EQ(NEdges,Graph->GetEdges());

  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(1,Graph->IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Graph->IsNode(i));
  }

  EXPECT_EQ(0,Graph->IsNode(NNodes));
  EXPECT_EQ(0,Graph->IsNode(NNodes+1));
  EXPECT_EQ(0,Graph->IsNode(2*NNodes));

  // nodes iterator
  NCount = 0;
  for (TDirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator
  NCount = 0;
  for (TDirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges,NCount);

  // edges iterator
  NCount = 0;
  for (TDirNet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TDirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg+OutDeg);
  }

  // assignment
  Graph1 = TDirNet::New();
  *Graph1 = *Graph;

  EXPECT_EQ(NNodes,Graph1->GetNodes());
  EXPECT_EQ(NEdges,Graph1->GetEdges());
  EXPECT_EQ(0,Graph1->Empty());
  EXPECT_EQ(1,Graph1->IsOk());

  // saving and loading
  {
    TFOut FOut(FName);
    Graph->Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    Graph2 = TDirNet::Load(FIn);
  }

  EXPECT_EQ(NNodes,Graph2->GetNodes());
  EXPECT_EQ(NEdges,Graph2->GetEdges());
  EXPECT_EQ(0,Graph2->Empty());
  EXPECT_EQ(1,Graph2->IsOk());

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Graph->GetRndNId();
    Graph->DelNode(n);
  }

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());

  Graph1->Clr();

  EXPECT_EQ(0,Graph1->GetNodes());
  EXPECT_EQ(0,Graph1->GetEdges());

  EXPECT_EQ(1,Graph1->IsOk());
  EXPECT_EQ(1,Graph1->Empty());
}

// Test small graph
TEST(TDirNet, GetSmallGraph) {
  PDirNet Graph;

  Graph = TDirNet::GetSmallGraph();

  EXPECT_EQ(5,Graph->GetNodes());
  EXPECT_EQ(6,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(1,Graph->HasFlag(gfDirected));
}

TEST(TDirNet, AddSAttrN) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TInt AttrId;
  int status = Graph->AddSAttrN("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph->AddSAttrN("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph->AddSAttrN("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  //status = Graph->AddSAttrN("TestAny", atAny, AttrId);
  //EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrIdN) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TInt AttrId;
  Graph->AddSAttrN("TestInt", atInt, AttrId);
  Graph->AddSAttrN("TestFlt", atFlt, AttrId);
  Graph->AddSAttrN("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int status = Graph->GetSAttrIdN(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph->GetSAttrIdN(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph->GetSAttrIdN(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Graph->GetSAttrIdN(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrNameN) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TInt AttrId;
  Graph->AddSAttrN("TestInt", atInt, AttrId);
  Graph->AddSAttrN("TestFlt", atFlt, AttrId);
  Graph->AddSAttrN("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int status = Graph->GetSAttrNameN(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Graph->GetSAttrNameN(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Graph->GetSAttrNameN(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Graph->GetSAttrNameN(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, AddSAttrDatN_int) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TInt Val(5);
  TInt Id(0);
  int status = Graph->AddSAttrDatN(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestInt");
  Graph->AddSAttrN(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Graph->AddSAttrDatN(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph->AddSAttrDatN(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph->AddSAttrDatN(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Graph->AddSAttrDatN(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph->AddSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, AddSAttrDatN_flt) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TFlt Val(5.0);
  TInt Id(0);
  int status = Graph->AddSAttrDatN(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph->AddSAttrN(AttrName, atFlt, AttrId);
  TInt ErrorVal(1);
  status = Graph->AddSAttrDatN(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph->AddSAttrDatN(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph->AddSAttrDatN(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Graph->AddSAttrDatN(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph->AddSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, AddSAttrDatN_str) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TStr Val("5");
  TInt Id(0);
  int status = Graph->AddSAttrDatN(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph->AddSAttrN(AttrName, atStr, AttrId);
  TInt ErrorVal(1);
  status = Graph->AddSAttrDatN(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph->AddSAttrDatN(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph->AddSAttrDatN(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Graph->AddSAttrDatN(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph->AddSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrDatN_int) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TInt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Graph->GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph->GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph->AddSAttrN(AttrName, atInt, AttrId);
  TInt TestVal(5);
  Graph->AddSAttrDatN(NId, AttrId, TestVal);
  status = Graph->GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph->GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  TInt ErrorId(1);
  status = Graph->GetSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrDatN_flt) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TFlt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Graph->GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph->GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph->AddSAttrN(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Graph->AddSAttrDatN(NId, AttrId, TestVal);
  status = Graph->GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph->GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  TInt ErrorId(1);
  status = Graph->GetSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrDatN_str) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TStr Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Graph->GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph->GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph->AddSAttrN(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Graph->AddSAttrDatN(NId, AttrId, TestVal);
  status = Graph->GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Graph->GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  TInt ErrorId(1);
  status = Graph->GetSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, DelSAttrDatN) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrN(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrN(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrN(StrAttr, atStr, StrId);
  TInt Id(0);
  int status = Graph->DelSAttrDatN(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Graph->DelSAttrDatN(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt IntVal(5);
  Graph->AddSAttrDatN(Id, IntId, IntVal);
  status = Graph->DelSAttrDatN(Id, IntAttr);
  EXPECT_EQ(0, status);
  Graph->AddSAttrDatN(Id, IntId, IntVal);
  status = Graph->DelSAttrDatN(Id, IntId);
  EXPECT_EQ(0, status);
  status = Graph->DelSAttrDatN(Id, IntId);
  EXPECT_EQ(-1, status);
  TInt ErrorId(1);
  status = Graph->DelSAttrDatN(ErrorId, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Graph->AddSAttrDatN(Id, FltId, FltVal);
  status = Graph->DelSAttrDatN(Id, FltAttr);
  EXPECT_EQ(0, status);
  Graph->AddSAttrDatN(Id, FltId, FltVal);
  status = Graph->DelSAttrDatN(Id, FltId);
  EXPECT_EQ(0, status);
  status = Graph->DelSAttrDatN(Id, FltId);
  EXPECT_EQ(-1, status);
  status = Graph->DelSAttrDatN(ErrorId, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Graph->AddSAttrDatN(Id, StrId, StrVal);
  status = Graph->DelSAttrDatN(Id, StrAttr);
  EXPECT_EQ(0, status);
  Graph->AddSAttrDatN(Id, StrId, StrVal);
  status = Graph->DelSAttrDatN(Id, StrId);
  EXPECT_EQ(0, status);
  status = Graph->DelSAttrDatN(Id, StrId);
  EXPECT_EQ(-1, status);
  status = Graph->DelSAttrDatN(ErrorId, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrVN) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrN(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrN(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrN(StrAttr, atStr, StrId);

  TInt Id(0);
  TInt IntVal(5);
  Graph->AddSAttrDatN(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Graph->AddSAttrDatN(Id, FltId, FltVal);
  TStr StrVal("5");
  Graph->AddSAttrDatN(Id, StrId, StrVal);

  TAttrPrV AttrV;
  int status = Graph->GetSAttrVN(Id, atInt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph->GetSAttrVN(Id, atFlt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph->GetSAttrVN(Id, atStr, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  //status = Graph->GetSAttrVN(Id, atAny, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(3, AttrV.Len());
  //status = Graph->GetSAttrVN(Id, atUndef, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(0, AttrV.Len());
  //TInt ErrorId(1);
  //status = Graph->GetSAttrVN(ErrorId, atUndef, AttrV);
  //EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetIdVSAttrN) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrN(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrN(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrN(StrAttr, atStr, StrId);

  TInt IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  for (int i = 0; i < 10; i++) {
    Graph->AddNode(i);
    TInt Id(i);
    Graph->AddSAttrDatN(Id, IntId, IntVal);
    if (i%2 == 0) {
      Graph->AddSAttrDatN(Id, FltId, FltVal);
    }
  }
  Graph->AddSAttrDatN(0, StrId, StrVal);

  TIntV IdV;
  Graph->GetIdVSAttrN(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Graph->GetIdVSAttrN(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Graph->GetIdVSAttrN(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Graph->GetIdVSAttrN(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Graph->GetIdVSAttrN(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Graph->GetIdVSAttrN(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}

TEST(TDirNet, AddSAttrE) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TInt AttrId;
  int status = Graph->AddSAttrE("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph->AddSAttrE("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph->AddSAttrE("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  //status = Graph->AddSAttrE("TestAny", atAny, AttrId);
  //EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrIdE) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TInt AttrId;
  Graph->AddSAttrE("TestInt", atInt, AttrId);
  Graph->AddSAttrE("TestFlt", atFlt, AttrId);
  Graph->AddSAttrE("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int status = Graph->GetSAttrIdE(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph->GetSAttrIdE(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph->GetSAttrIdE(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Graph->GetSAttrIdE(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrNameE) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TInt AttrId;
  Graph->AddSAttrE("TestInt", atInt, AttrId);
  Graph->AddSAttrE("TestFlt", atFlt, AttrId);
  Graph->AddSAttrE("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int status = Graph->GetSAttrNameE(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Graph->GetSAttrNameE(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Graph->GetSAttrNameE(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Graph->GetSAttrNameE(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, AddSAttrDatE_int) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TInt Val(5);
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->AddSAttrDatE(SrcId, DstId, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestInt");
  Graph->AddSAttrE(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Graph->AddSAttrDatE(SrcId, DstId, NewName, Val);
  EXPECT_EQ(0, status);
  int ErrorId = 5;
  status = Graph->AddSAttrDatE(SrcId, ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, AddSAttrDatE_flt) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TFlt Val(5.0);
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->AddSAttrDatE(SrcId, DstId, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph->AddSAttrE(AttrName, atFlt, AttrId);
  TInt ErrorVal(1);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Graph->AddSAttrDatE(SrcId, DstId, NewName, Val);
  EXPECT_EQ(0, status);
  int ErrorId = 5;
  status = Graph->AddSAttrDatE(SrcId, ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, AddSAttrDatE_str) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TStr Val("5");
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->AddSAttrDatE(SrcId, DstId, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph->AddSAttrE(AttrName, atStr, AttrId);
  TInt ErrorVal(1);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph->AddSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Graph->AddSAttrDatE(SrcId, DstId, NewName, Val);
  EXPECT_EQ(0, status);
  int ErrorId = 5;
  status = Graph->AddSAttrDatE(SrcId, ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrDatE_int) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TInt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->GetSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph->AddSAttrE(AttrName, atInt, AttrId);
  TInt TestVal(5);
  Graph->AddSAttrDatE(SrcId, DstId, AttrId, TestVal);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  int ErrorId = 5;
  status = Graph->GetSAttrDatE(SrcId, ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrDatE_flt) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TFlt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->GetSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph->AddSAttrE(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Graph->AddSAttrDatE(SrcId, DstId, AttrId, TestVal);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  int ErrorId = 5;
  status = Graph->GetSAttrDatE(SrcId, ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrDatE_str) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TStr Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->GetSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph->AddSAttrE(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Graph->AddSAttrDatE(SrcId, DstId, AttrId, TestVal);
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Graph->GetSAttrDatE(SrcId, DstId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  int ErrorId = 5;
  status = Graph->GetSAttrDatE(SrcId, ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, DelSAttrDatE) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrE(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrE(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrE(StrAttr, atStr, StrId);
  int SrcId = 0;
  int DstId = 1;
  int status = Graph->DelSAttrDatE(SrcId, DstId, IntAttr);
  EXPECT_EQ(-1, status);
  status = Graph->DelSAttrDatE(SrcId, DstId, IntId);
  EXPECT_EQ(-1, status);

  TInt IntVal(5);
  Graph->AddSAttrDatE(SrcId, DstId, IntId, IntVal);
  status = Graph->DelSAttrDatE(SrcId, DstId, IntAttr);
  EXPECT_EQ(0, status);
  Graph->AddSAttrDatE(SrcId, DstId, IntId, IntVal);
  status = Graph->DelSAttrDatE(SrcId, DstId, IntId);
  EXPECT_EQ(0, status);
  status = Graph->DelSAttrDatE(SrcId, DstId, IntId);
  EXPECT_EQ(-1, status);
  int ErrorId = 5;
  status = Graph->DelSAttrDatE(SrcId, ErrorId, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Graph->AddSAttrDatE(SrcId, DstId, FltId, FltVal);
  status = Graph->DelSAttrDatE(SrcId, DstId, FltAttr);
  EXPECT_EQ(0, status);
  Graph->AddSAttrDatE(SrcId, DstId, FltId, FltVal);
  status = Graph->DelSAttrDatE(SrcId, DstId, FltId);
  EXPECT_EQ(0, status);
  status = Graph->DelSAttrDatE(SrcId, DstId, FltId);
  EXPECT_EQ(-1, status);
  status = Graph->DelSAttrDatE(SrcId, ErrorId, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Graph->AddSAttrDatE(SrcId, DstId, StrId, StrVal);
  status = Graph->DelSAttrDatE(SrcId, DstId, StrAttr);
  EXPECT_EQ(0, status);
  Graph->AddSAttrDatE(SrcId, DstId, StrId, StrVal);
  status = Graph->DelSAttrDatE(SrcId, DstId, StrId);
  EXPECT_EQ(0, status);
  status = Graph->DelSAttrDatE(SrcId, DstId, StrId);
  EXPECT_EQ(-1, status);
  status = Graph->DelSAttrDatE(SrcId, ErrorId, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetSAttrVE) {
  PDirNet Graph;
  Graph = TDirNet::New();
  Graph->AddNode(0);
  Graph->AddNode(1);
  Graph->AddEdge(0, 1);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrE(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrE(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrE(StrAttr, atStr, StrId);

  int SrcId = 0;
  int DstId = 1;
  TInt IntVal(5);
  Graph->AddSAttrDatE(SrcId, DstId, IntId, IntVal);
  TFlt FltVal(5.0);
  Graph->AddSAttrDatE(SrcId, DstId, FltId, FltVal);
  TStr StrVal("5");
  Graph->AddSAttrDatE(SrcId, DstId, StrId, StrVal);

  TAttrPrV AttrV;
  int status = Graph->GetSAttrVE(SrcId, DstId, atInt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph->GetSAttrVE(SrcId, DstId, atFlt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph->GetSAttrVE(SrcId, DstId, atStr, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  //status = Graph->GetSAttrVE(SrcId, DstId, atAny, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(3, AttrV.Len());
  //status = Graph->GetSAttrVE(SrcId, DstId, atUndef, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(0, AttrV.Len());
  //int ErrorId = 5;
  //status = Graph->GetSAttrVE(SrcId, ErrorId, atUndef, AttrV);
  //EXPECT_EQ(-1, status);
}

TEST(TDirNet, GetIdVSAttrE) {
  PDirNet Graph;
  Graph = TDirNet::New();
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph->AddSAttrE(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph->AddSAttrE(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph->AddSAttrE(StrAttr, atStr, StrId);

  TInt IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  Graph->AddNode(0);
  for (int i = 0; i < 10; i++) {
    Graph->AddNode(i+1);
    Graph->AddEdge(i, i+1);
    Graph->AddSAttrDatE(i, i+1, IntId, IntVal);
    if (i%2 == 0) {
      Graph->AddSAttrDatE(i, i+1, FltId, FltVal);
    }
  }
  Graph->AddSAttrDatE(0, 1, StrId, StrVal);

  TIntPrV IdV;
  Graph->GetIdVSAttrE(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Graph->GetIdVSAttrE(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Graph->GetIdVSAttrE(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Graph->GetIdVSAttrE(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Graph->GetIdVSAttrE(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Graph->GetIdVSAttrE(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}
