#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TModeNet, DefaultConstructor) {
  TModeNet Graph;

  Graph = TModeNet();

  EXPECT_EQ(0,Graph.GetNodes());
  EXPECT_EQ(0,Graph.GetEdges());

  EXPECT_EQ(1,Graph.IsOk());
  EXPECT_EQ(1,Graph.Empty());
  EXPECT_EQ(1,Graph.HasFlag(gfDirected));
  EXPECT_EQ(1,Graph.HasFlag(gfMultiGraph));
}

// Test node, edge creation
TEST(TModeNet, ManipulateNodesEdges) {
  int NNodes = 1000;
  int NEdges = 100000;
  const char *FName = "test.graph.dat";

  TModeNet Graph;
  TModeNet Graph1;
  TModeNet Graph2;
  int i;
  int n;
  int NCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Graph = TModeNet();
  EXPECT_EQ(1,Graph.Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph.AddNode(i);
  }
  EXPECT_EQ(0,Graph.Empty());
  EXPECT_EQ(NNodes,Graph.GetNodes());

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    n = Graph.AddEdge(x, y);
    NCount--;
  }

  EXPECT_EQ(NEdges,Graph.GetEdges());

  EXPECT_EQ(0,Graph.Empty());
  EXPECT_EQ(1,Graph.IsOk());

  for (i = 0; i < NNodes; i++) {
    EXPECT_EQ(1,Graph.IsNode(i));
  }

  EXPECT_EQ(0,Graph.IsNode(NNodes));
  EXPECT_EQ(0,Graph.IsNode(NNodes+1));
  EXPECT_EQ(0,Graph.IsNode(2*NNodes));

  // nodes iterator
  NCount = 0;
  for (TModeNet::TNodeI NI = Graph.BegMMNI(); NI < Graph.EndMMNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator
  NCount = 0;
  for (TModeNet::TNodeI NI = Graph.BegMMNI(); NI < Graph.EndMMNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges,NCount);

  // edges iterator
  NCount = 0;
  for (TNEANet::TEdgeI EI = Graph.BegEI(); EI < Graph.EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TModeNet::TNodeI NI = Graph.BegMMNI(); NI < Graph.EndMMNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg+OutDeg);
  }

  // assignment
  Graph1 = Graph;

  EXPECT_EQ(NNodes,Graph1.GetNodes());
  EXPECT_EQ(NEdges,Graph1.GetEdges());
  EXPECT_EQ(0,Graph1.Empty());
  EXPECT_EQ(1,Graph1.IsOk());

  // saving and loading
  {
    TFOut FOut(FName);
    Graph.Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    Graph2 = TModeNet(FIn);
  }

  EXPECT_EQ(NNodes,Graph2.GetNodes());
  EXPECT_EQ(NEdges,Graph2.GetEdges());
  EXPECT_EQ(0,Graph2.Empty());
  EXPECT_EQ(1,Graph2.IsOk());

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Graph.GetRndNId();
    Graph.DelNode(n);
  }

  EXPECT_EQ(0,Graph.GetNodes());
  EXPECT_EQ(0,Graph.GetEdges());

  EXPECT_EQ(1,Graph.IsOk());
  EXPECT_EQ(1,Graph.Empty());

  Graph1.Clr();

  EXPECT_EQ(0,Graph1.GetNodes());
  EXPECT_EQ(0,Graph1.GetEdges());

  EXPECT_EQ(1,Graph1.IsOk());
  EXPECT_EQ(1,Graph1.Empty());
}

TEST(TModeNet, IntVAttr) {
  TModeNet Graph;
  Graph = TModeNet();
  int i;
  TIntV test;
  int numNodes = 10;
  Graph.AddNode(0);
  for (i = 1; i < numNodes; i++) {
    Graph.AddNode(i);
    Graph.AddEdge(i-1, i);
  }
  TStr TestAttr("Test1");
  Graph.AddIntVAttrN(TestAttr);
  for (i = 0; i < numNodes; i++) {
    test = Graph.GetIntVAttrDatN(i, TestAttr);
    EXPECT_EQ(0, test.Len());
  }

  TIntV testVB;
  for (i = 0; i < numNodes; i++) {
    testVB.Add(i);
  }
  const TIntV testV = testVB;
  Graph.AddIntVAttrDatN(0, testV, TestAttr);
  test = Graph.GetIntVAttrDatN(0, TestAttr);
  EXPECT_EQ(numNodes, test.Len());

  for (i = 0; i < numNodes; i++) {
    EXPECT_EQ(test[i], i);
  }

  Graph.AppendIntVAttrDatN(0, numNodes, TestAttr);
  test = Graph.GetIntVAttrDatN(0, TestAttr);
  EXPECT_EQ(numNodes+1, test.Len());
  for (i = 0; i < numNodes+1; i++) {
    EXPECT_EQ(test[i], i);
  }

  Graph.DelAttrDatN(0, TestAttr);
  for (i = 0; i < numNodes; i++) {
    test = Graph.GetIntVAttrDatN(i, TestAttr);
    EXPECT_EQ(0, test.Len());
  }

}

// Test node, edge attribute functionality
TEST(TModeNet, ManipulateNodesEdgeAttributes) {
  int NNodes = 1000;
  int NEdges = 1000;
  const char *FName = "demo.graph.dat";

  TModeNet Graph;
  TModeNet Graph1;
  int i;
  int x, y;
  bool t;

  Graph = TModeNet();
  t = Graph.Empty();

  // create the nodes
  for (i = NNodes - 1; i >= 0; i--) {
    Graph.AddNode(i);
  }

  EXPECT_EQ(NNodes, Graph.GetNodes());

  // create the edges 
  for (i = NEdges - 1; i >= 0; i--) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    Graph.AddEdge(x, y, i);
  }

  // create attributes and fill all nodes
  TStr attr1 = "str";
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr4 = "default";

  // Test vertical int iterator for node 3, 50, 700, 900
  // Check if we can set defaults to 0 for Int data.
  Graph.AddIntAttrN(attr2, 0);
  Graph.AddIntAttrDatN(3, 3*2, attr2);
  Graph.AddIntAttrDatN(50, 50*2, attr2);
  Graph.AddIntAttrDatN(700, 700*2, attr2);
  Graph.AddIntAttrDatN(900, 900*2, attr2);

  EXPECT_EQ(3*2, Graph.GetNAIntI(attr2, 3).GetDat());
  EXPECT_EQ(50*2, Graph.GetNAIntI(attr2, 50).GetDat());

  int NodeId = 0;
  int DefNodes = 0;
  TVec<TInt> TAIntIV = TVec<TInt>();
  for (TModeNet::TAIntI NI = Graph.BegNAIntI(attr2);
    NI < Graph.EndNAIntI(attr2); NI++) {
    if (NI.GetDat()() != 0) {
      TAIntIV.Add(NI.GetDat());
      NodeId++;
    } else {
      DefNodes++;
    }
  }
  
  EXPECT_EQ(4, NodeId);
  EXPECT_EQ(NNodes - 4, DefNodes);
  TAIntIV.Sort();
  EXPECT_EQ(3*2, TAIntIV[0]);
  EXPECT_EQ(50*2, TAIntIV[1]);
  EXPECT_EQ(700*2, TAIntIV[2]);
  EXPECT_EQ(900*2, TAIntIV[3]);

  // Test vertical flt iterator for node 3, 50, 700, 900
  Graph.AddFltAttrDatN(5, 3.41, attr3);
  Graph.AddFltAttrDatN(50, 2.718, attr3);
  Graph.AddFltAttrDatN(300, 150.0, attr3);
  Graph.AddFltAttrDatN(653, 563, attr3);

  EXPECT_EQ(3.41, Graph.GetNAFltI(attr3, 5).GetDat());
  EXPECT_EQ(2.718, Graph.GetNAFltI(attr3, 50).GetDat());

  NodeId = 0;
  DefNodes = 0;
  TVec<TFlt> TAFltIV = TVec<TFlt>();

  for (TModeNet::TAFltI NI = Graph.BegNAFltI(attr3);
    NI < Graph.EndNAFltI(attr3); NI++) {
    if (NI.GetDat() != TFlt::Mn) {
      NodeId++;
      TAFltIV.Add(NI.GetDat());
    } else {
      DefNodes++;
    }
  }

  EXPECT_EQ(4, NodeId);
  EXPECT_EQ(NNodes - 4, DefNodes);
  TAFltIV.Sort();
  EXPECT_EQ(2.718, TAFltIV[0]);
  EXPECT_EQ(3.41, TAFltIV[1]);
  EXPECT_EQ(150.0, TAFltIV[2]);
  EXPECT_EQ(563.0, TAFltIV[3]);

  // Test vertical str iterator for node 3, 50, 700, 900
  Graph.AddStrAttrDatN(10, "abc", attr1);
  Graph.AddStrAttrDatN(20, "def", attr1);
  Graph.AddStrAttrDatN(400, "ghi", attr1);
  // this does not show since ""=null
  Graph.AddStrAttrDatN(455, "", attr1);

  EXPECT_EQ('c', Graph.GetNAStrI(attr1, 10).GetDat().LastCh());
  EXPECT_EQ('f', Graph.GetNAStrI(attr1, 20).GetDat().LastCh());

  NodeId = 0;
  DefNodes = 0;
  TVec<TStr> TAStrIV = TVec<TStr>();

  for (TModeNet::TAStrI NI = Graph.BegNAStrI(attr1);
    NI < Graph.EndNAStrI(attr1); NI++) {
    if (NI.GetDat() != TStr::GetNullStr()) {
      NodeId++;
      TAStrIV.Add(NI.GetDat());
    } else {
      DefNodes++;
    }
  }

  EXPECT_EQ(3, NodeId);
  EXPECT_EQ(NNodes - 3, DefNodes);
  TAStrIV.Sort();
  // TODO(nkhadke): Fix hack to compare strings properly. This works for now.
  EXPECT_EQ('c', TAStrIV[0].LastCh());
  EXPECT_EQ('f', TAStrIV[1].LastCh());
  EXPECT_EQ('i', TAStrIV[2].LastCh());
  
    
  // Test vertical iterator over many types (must skip default/deleted attr) 
  int NId = 55;
  Graph.AddStrAttrDatN(NId, "aaa", attr1);
  Graph.AddIntAttrDatN(NId, 3*2, attr2);
  Graph.AddFltAttrDatN(NId, 3.41, attr3);
  Graph.AddStrAttrDatN(80, "dont appear", attr4); // should not show up
  TStrV NIdAttrName;
  Graph.AttrNameNI(NId, NIdAttrName);
  int AttrLen = NIdAttrName.Len();
  NodeId = 0;
  DefNodes = 0;
  EXPECT_EQ(3, AttrLen);
  
  Graph.DelAttrDatN(NId, attr2);
  Graph.AttrNameNI(NId, NIdAttrName);
  AttrLen = NIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("int") == NIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(1,2);
    }
  }
  EXPECT_EQ(2, AttrLen);

  Graph.AddIntAttrDatN(NId, 3*2, attr2);
  Graph.DelAttrN(attr1);
  Graph.AttrNameNI(NId, NIdAttrName);
  AttrLen = NIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("str") == NIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(1,2);
    }
  }
  EXPECT_EQ(2, AttrLen);
   
  TStrV NIdAttrValue;
  Graph.AttrValueNI(NId, NIdAttrValue);
  AttrLen = NIdAttrValue.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("str") == NIdAttrValue[i]()) {
      // FAIL
      EXPECT_EQ(1,2);
    }
  } 

  int expectedTotal = 0;
  for (i = 0; i <NNodes; i++) {
    Graph.AddIntAttrDatN(i, NNodes+i, attr2);
    EXPECT_EQ(NNodes+i, Graph.GetIntAttrDatN(i, attr2));
    expectedTotal += NNodes+i;
  }

  {
    TFOut FOut(FName);
    Graph.Save(FOut);
    FOut.Flush();
  }

  /*{
    TFIn FIn(FName);
    Graph1 = TModeNet::Load(FIn);
  }

  int total = 0;
  for (TModeNet::TAIntI NI = Graph1.BegNAIntI(attr2);
    NI < Graph1.EndNAIntI(attr2); NI++) {
    total += NI.GetDat();
  }

  ASSERT_EQ(expectedTotal, total);

  Graph1.Clr();*/

  // Test vertical int iterator for edge
  Graph.AddIntAttrDatE(3, 3*2, attr2);
  Graph.AddIntAttrDatE(55, 55*2, attr2);
  Graph.AddIntAttrDatE(705, 705*2, attr2);
  Graph.AddIntAttrDatE(905, 905*2, attr2);

  EXPECT_EQ(3*2, Graph.GetEAIntI(attr2, 3).GetDat());
  EXPECT_EQ(55*2, Graph.GetEAIntI(attr2, 55).GetDat());

  int EdgeId = 0;
  int DefEdges = 0;
  TAIntIV.Clr();
  for (TModeNet::TAIntI EI = Graph.BegEAIntI(attr2);
    EI < Graph.EndEAIntI(attr2); EI++) {
    if (EI.GetDat() != TInt::Mn) {
      TAIntIV.Add(EI.GetDat());
      EdgeId++;
    } else {
      DefEdges++;
    }
  } 

  EXPECT_EQ(4, EdgeId);
  EXPECT_EQ(NEdges - 4, DefEdges);
  TAIntIV.Sort();
  EXPECT_EQ(3*2, TAIntIV[0]);
  EXPECT_EQ(55*2, TAIntIV[1]);
  EXPECT_EQ(705*2, TAIntIV[2]);
  EXPECT_EQ(905*2, TAIntIV[3]);
   
  // Test vertical flt iterator for edge
  Graph.AddFltAttrE(attr3, 0.00);
  Graph.AddFltAttrDatE(5, 4.41, attr3);
  Graph.AddFltAttrDatE(50, 3.718, attr3);
  Graph.AddFltAttrDatE(300, 151.0, attr3);
  Graph.AddFltAttrDatE(653, 654, attr3);

  EXPECT_EQ(4.41, Graph.GetEAFltI(attr3, 5).GetDat());
  EXPECT_EQ(3.718, Graph.GetEAFltI(attr3, 50).GetDat());

  EdgeId = 0;
  DefEdges = 0;
  TAFltIV.Clr();

  for (TModeNet::TAFltI EI = Graph.BegEAFltI(attr3);
    EI < Graph.EndEAFltI(attr3); EI++) {
    // Check if defaults are set to 0.
    if (EI.GetDat() != 0.00) {
      TAFltIV.Add(EI.GetDat());
      EdgeId++;
    } else {
      DefEdges++;
    }
  }

  EXPECT_EQ(4, EdgeId);
  EXPECT_EQ(NEdges - 4, DefEdges);
  TAFltIV.Sort();
  EXPECT_EQ(3.718, TAFltIV[0]);
  EXPECT_EQ(4.41, TAFltIV[1]);
  EXPECT_EQ(151.0, TAFltIV[2]);
  EXPECT_EQ(654.0, TAFltIV[3]);

  // Test vertical str iterator for edge
  Graph.AddStrAttrDatE(10, "abc", attr1);
  Graph.AddStrAttrDatE(20, "def", attr1);
  Graph.AddStrAttrDatE(400, "ghi", attr1);
  // this does not show since ""=null
  Graph.AddStrAttrDatE(455, "", attr1);

  EXPECT_EQ('c', Graph.GetEAStrI(attr1, 10).GetDat().LastCh());
  EXPECT_EQ('f', Graph.GetEAStrI(attr1, 20).GetDat().LastCh());

  EdgeId = 0;
  DefEdges = 0;
  TAStrIV.Clr();

  for (TModeNet::TAStrI EI = Graph.BegEAStrI(attr1);
    EI < Graph.EndEAStrI(attr1); EI++) {
    if (EI.GetDat() != TStr::GetNullStr()) {
      TAStrIV.Add(EI.GetDat());
      EdgeId++;
    } else {
      DefEdges++;
    }
  } 

  EXPECT_EQ(3, EdgeId);
  EXPECT_EQ(NEdges - 3, DefEdges);
  TAStrIV.Sort();
  // TODO(nkhadke): Fix hack to compare strings properly. This works for now.
  EXPECT_EQ('c', TAStrIV[0].LastCh());
  EXPECT_EQ('f', TAStrIV[1].LastCh());
  EXPECT_EQ('i', TAStrIV[2].LastCh());

  // Test vertical iterator over many types (must skip default/deleted attr) 
  int EId = 55;
  Graph.AddStrAttrDatE(EId, "aaa", attr1);
  Graph.AddIntAttrDatE(EId, 3*2, attr2);
  Graph.AddFltAttrDatE(EId, 3.41, attr3);
  Graph.AddStrAttrDatE(80, "dont appear", attr4); // should not show up  

  TStrV EIdAttrName;
  Graph.AttrNameEI(EId, EIdAttrName);
  AttrLen = EIdAttrName.Len();
  EXPECT_EQ(3, AttrLen);
  
  Graph.DelAttrDatE(EId, attr2);
  Graph.AttrNameEI(EId, EIdAttrName);
  AttrLen = EIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("int") == EIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(2,3);
    }
  }

  Graph.AddIntAttrDatE(EId, 3*2, attr2);
  Graph.DelAttrE(attr1);
  Graph.AttrNameEI(EId, EIdAttrName);
  AttrLen = EIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("aaa") == EIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(2,3);
    }
  }

  TStrV EIdAttrValue;
  Graph.AttrValueEI(EId, EIdAttrValue);
  AttrLen = EIdAttrValue.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("str") == EIdAttrValue[i]()) {
      // FAIL
      EXPECT_EQ(2,3);
    }
  }

  expectedTotal = 0;
  for (i = 0; i <NEdges; i++) {
    Graph.AddIntAttrDatE(i, NEdges+i, attr2);
    EXPECT_EQ(NEdges+i, Graph.GetIntAttrDatE(i, attr2));
    expectedTotal += NEdges+i;
  }

  {
    TFOut FOut(FName);
    Graph.Save(FOut);
    FOut.Flush();
    Graph.Clr();
  }

  /*{
    TFIn FIn(FName);
    Graph1 = TModeNet::Load(FIn);
  }

  total = 0;
  for (TModeNet::TAIntI EI = Graph1.BegNAIntI(attr2);
    EI < Graph1.EndNAIntI(attr2); EI++) {
    total += EI.GetDat();
  }

  EXPECT_EQ(expectedTotal, total);

  //Graph1.Dump();
  Graph1.Clr();*/
}

TEST(TModeNet, AddSAttrN) {
  TModeNet Graph;
  Graph = TModeNet();
  TInt AttrId;
  int status = Graph.AddSAttrN("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph.AddSAttrN("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph.AddSAttrN("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  //status = Graph.AddSAttrN("TestAny", atAny, AttrId);
  //EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrIdN) {
  TModeNet Graph;
  Graph = TModeNet();
  TInt AttrId;
  Graph.AddSAttrN("TestInt", atInt, AttrId);
  Graph.AddSAttrN("TestFlt", atFlt, AttrId);
  Graph.AddSAttrN("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int status = Graph.GetSAttrIdN(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph.GetSAttrIdN(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph.GetSAttrIdN(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Graph.GetSAttrIdN(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrNameN) {
  TModeNet Graph;
  Graph = TModeNet();
  TInt AttrId;
  Graph.AddSAttrN("TestInt", atInt, AttrId);
  Graph.AddSAttrN("TestFlt", atFlt, AttrId);
  Graph.AddSAttrN("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int status = Graph.GetSAttrNameN(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Graph.GetSAttrNameN(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Graph.GetSAttrNameN(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Graph.GetSAttrNameN(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, AddSAttrDatN_int) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TInt Val(5);
  TInt Id(0);
  int status = Graph.AddSAttrDatN(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestInt");
  Graph.AddSAttrN(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Graph.AddSAttrDatN(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph.AddSAttrDatN(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph.AddSAttrDatN(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Graph.AddSAttrDatN(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph.AddSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, AddSAttrDatN_flt) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TFlt Val(5.0);
  TInt Id(0);
  int status = Graph.AddSAttrDatN(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph.AddSAttrN(AttrName, atFlt, AttrId);
  TInt ErrorVal(1);
  status = Graph.AddSAttrDatN(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph.AddSAttrDatN(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph.AddSAttrDatN(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Graph.AddSAttrDatN(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph.AddSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, AddSAttrDatN_str) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TStr Val("5");
  TInt Id(0);
  int status = Graph.AddSAttrDatN(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph.AddSAttrN(AttrName, atStr, AttrId);
  TInt ErrorVal(1);
  status = Graph.AddSAttrDatN(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph.AddSAttrDatN(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph.AddSAttrDatN(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Graph.AddSAttrDatN(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph.AddSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrDatN_int) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TInt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Graph.GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph.GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph.AddSAttrN(AttrName, atInt, AttrId);
  TInt TestVal(5);
  Graph.AddSAttrDatN(NId, AttrId, TestVal);
  status = Graph.GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph.GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  TInt ErrorId(1);
  status = Graph.GetSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrDatN_flt) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TFlt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Graph.GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph.GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph.AddSAttrN(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Graph.AddSAttrDatN(NId, AttrId, TestVal);
  status = Graph.GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph.GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  TInt ErrorId(1);
  status = Graph.GetSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrDatN_str) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TStr Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Graph.GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph.GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph.AddSAttrN(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Graph.AddSAttrDatN(NId, AttrId, TestVal);
  status = Graph.GetSAttrDatN(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Graph.GetSAttrDatN(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  TInt ErrorId(1);
  status = Graph.GetSAttrDatN(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, DelSAttrDatN) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph.AddSAttrN(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph.AddSAttrN(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph.AddSAttrN(StrAttr, atStr, StrId);
  TInt Id(0);
  int status = Graph.DelSAttrDatN(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Graph.DelSAttrDatN(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt IntVal(5);
  Graph.AddSAttrDatN(Id, IntId, IntVal);
  status = Graph.DelSAttrDatN(Id, IntAttr);
  EXPECT_EQ(0, status);
  Graph.AddSAttrDatN(Id, IntId, IntVal);
  status = Graph.DelSAttrDatN(Id, IntId);
  EXPECT_EQ(0, status);
  status = Graph.DelSAttrDatN(Id, IntId);
  EXPECT_EQ(-1, status);
  TInt ErrorId(1);
  status = Graph.DelSAttrDatN(ErrorId, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Graph.AddSAttrDatN(Id, FltId, FltVal);
  status = Graph.DelSAttrDatN(Id, FltAttr);
  EXPECT_EQ(0, status);
  Graph.AddSAttrDatN(Id, FltId, FltVal);
  status = Graph.DelSAttrDatN(Id, FltId);
  EXPECT_EQ(0, status);
  status = Graph.DelSAttrDatN(Id, FltId);
  EXPECT_EQ(-1, status);
  status = Graph.DelSAttrDatN(ErrorId, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Graph.AddSAttrDatN(Id, StrId, StrVal);
  status = Graph.DelSAttrDatN(Id, StrAttr);
  EXPECT_EQ(0, status);
  Graph.AddSAttrDatN(Id, StrId, StrVal);
  status = Graph.DelSAttrDatN(Id, StrId);
  EXPECT_EQ(0, status);
  status = Graph.DelSAttrDatN(Id, StrId);
  EXPECT_EQ(-1, status);
  status = Graph.DelSAttrDatN(ErrorId, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrVN) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph.AddSAttrN(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph.AddSAttrN(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph.AddSAttrN(StrAttr, atStr, StrId);

  TInt Id(0);
  TInt IntVal(5);
  Graph.AddSAttrDatN(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Graph.AddSAttrDatN(Id, FltId, FltVal);
  TStr StrVal("5");
  Graph.AddSAttrDatN(Id, StrId, StrVal);

  TAttrPrV AttrV;
  int status = Graph.GetSAttrVN(Id, atInt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph.GetSAttrVN(Id, atFlt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph.GetSAttrVN(Id, atStr, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  //status = Graph.GetSAttrVN(Id, atAny, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(3, AttrV.Len());
  //status = Graph.GetSAttrVN(Id, atUndef, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(0, AttrV.Len());
  //TInt ErrorId(1);
  //status = Graph.GetSAttrVN(ErrorId, atUndef, AttrV);
  //EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetIdVSAttrN) {
  TModeNet Graph;
  Graph = TModeNet();
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph.AddSAttrN(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph.AddSAttrN(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph.AddSAttrN(StrAttr, atStr, StrId);

  TInt IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  for (int i = 0; i < 10; i++) {
    Graph.AddNode(i);
    TInt Id(i);
    Graph.AddSAttrDatN(Id, IntId, IntVal);
    if (i%2 == 0) {
      Graph.AddSAttrDatN(Id, FltId, FltVal);
    }
  }
  Graph.AddSAttrDatN(0, StrId, StrVal);

  TIntV IdV;
  Graph.GetIdVSAttrN(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Graph.GetIdVSAttrN(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Graph.GetIdVSAttrN(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Graph.GetIdVSAttrN(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Graph.GetIdVSAttrN(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Graph.GetIdVSAttrN(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}

TEST(TModeNet, AddSAttrE) {
  TModeNet Graph;
  Graph = TModeNet();
  TInt AttrId;
  int status = Graph.AddSAttrE("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph.AddSAttrE("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph.AddSAttrE("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  //status = Graph.AddSAttrE("TestAny", atAny, AttrId);
  //EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrIdE) {
  TModeNet Graph;
  Graph = TModeNet();
  TInt AttrId;
  Graph.AddSAttrE("TestInt", atInt, AttrId);
  Graph.AddSAttrE("TestFlt", atFlt, AttrId);
  Graph.AddSAttrE("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int status = Graph.GetSAttrIdE(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Graph.GetSAttrIdE(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Graph.GetSAttrIdE(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Graph.GetSAttrIdE(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrNameE) {
  TModeNet Graph;
  Graph = TModeNet();
  TInt AttrId;
  Graph.AddSAttrE("TestInt", atInt, AttrId);
  Graph.AddSAttrE("TestFlt", atFlt, AttrId);
  Graph.AddSAttrE("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int status = Graph.GetSAttrNameE(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Graph.GetSAttrNameE(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Graph.GetSAttrNameE(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Graph.GetSAttrNameE(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, AddSAttrDatE_int) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TInt Val(5);
  TInt Id(0);
  int status = Graph.AddSAttrDatE(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestInt");
  Graph.AddSAttrE(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Graph.AddSAttrDatE(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph.AddSAttrDatE(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph.AddSAttrDatE(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Graph.AddSAttrDatE(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph.AddSAttrDatE(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, AddSAttrDatE_flt) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TFlt Val(5.0);
  TInt Id(0);
  int status = Graph.AddSAttrDatE(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph.AddSAttrE(AttrName, atFlt, AttrId);
  TInt ErrorVal(1);
  status = Graph.AddSAttrDatE(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph.AddSAttrDatE(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph.AddSAttrDatE(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Graph.AddSAttrDatE(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph.AddSAttrDatE(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, AddSAttrDatE_str) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TStr Val("5");
  TInt Id(0);
  int status = Graph.AddSAttrDatE(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Graph.AddSAttrE(AttrName, atStr, AttrId);
  TInt ErrorVal(1);
  status = Graph.AddSAttrDatE(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Graph.AddSAttrDatE(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Graph.AddSAttrDatE(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Graph.AddSAttrDatE(Id, NewName, Val);
  EXPECT_EQ(0, status);
  TInt ErrorId(1);
  status = Graph.AddSAttrDatE(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrDatE_int) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TInt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt EId(0);
  int status = Graph.GetSAttrDatE(EId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph.GetSAttrDatE(EId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph.AddSAttrE(AttrName, atInt, AttrId);
  TInt TestVal(5);
  Graph.AddSAttrDatE(EId, AttrId, TestVal);
  status = Graph.GetSAttrDatE(EId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph.GetSAttrDatE(EId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  TInt ErrorId(1);
  status = Graph.GetSAttrDatE(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrDatE_flt) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TFlt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt EId(0);
  int status = Graph.GetSAttrDatE(EId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph.GetSAttrDatE(EId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph.AddSAttrE(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Graph.AddSAttrDatE(EId, AttrId, TestVal);
  status = Graph.GetSAttrDatE(EId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Graph.GetSAttrDatE(EId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  TInt ErrorId(1);
  status = Graph.GetSAttrDatE(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrDatE_str) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TStr Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt EId(0);
  int status = Graph.GetSAttrDatE(EId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Graph.GetSAttrDatE(EId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Graph.AddSAttrE(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Graph.AddSAttrDatE(EId, AttrId, TestVal);
  status = Graph.GetSAttrDatE(EId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Graph.GetSAttrDatE(EId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  TInt ErrorId(1);
  status = Graph.GetSAttrDatE(ErrorId, AttrId, Val);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, DelSAttrDatE) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph.AddSAttrE(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph.AddSAttrE(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph.AddSAttrE(StrAttr, atStr, StrId);
  TInt Id(0);
  int status = Graph.DelSAttrDatE(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Graph.DelSAttrDatE(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt IntVal(5);
  Graph.AddSAttrDatE(Id, IntId, IntVal);
  status = Graph.DelSAttrDatE(Id, IntAttr);
  EXPECT_EQ(0, status);
  Graph.AddSAttrDatE(Id, IntId, IntVal);
  status = Graph.DelSAttrDatE(Id, IntId);
  EXPECT_EQ(0, status);
  status = Graph.DelSAttrDatE(Id, IntId);
  EXPECT_EQ(-1, status);
  TInt ErrorId(1);
  status = Graph.DelSAttrDatE(ErrorId, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Graph.AddSAttrDatE(Id, FltId, FltVal);
  status = Graph.DelSAttrDatE(Id, FltAttr);
  EXPECT_EQ(0, status);
  Graph.AddSAttrDatE(Id, FltId, FltVal);
  status = Graph.DelSAttrDatE(Id, FltId);
  EXPECT_EQ(0, status);
  status = Graph.DelSAttrDatE(Id, FltId);
  EXPECT_EQ(-1, status);
  status = Graph.DelSAttrDatE(ErrorId, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Graph.AddSAttrDatE(Id, StrId, StrVal);
  status = Graph.DelSAttrDatE(Id, StrAttr);
  EXPECT_EQ(0, status);
  Graph.AddSAttrDatE(Id, StrId, StrVal);
  status = Graph.DelSAttrDatE(Id, StrId);
  EXPECT_EQ(0, status);
  status = Graph.DelSAttrDatE(Id, StrId);
  EXPECT_EQ(-1, status);
  status = Graph.DelSAttrDatE(ErrorId, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetSAttrVE) {
  TModeNet Graph;
  Graph = TModeNet();
  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddEdge(0, 1, 0);
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph.AddSAttrE(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph.AddSAttrE(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph.AddSAttrE(StrAttr, atStr, StrId);

  TInt Id(0);
  TInt IntVal(5);
  Graph.AddSAttrDatE(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Graph.AddSAttrDatE(Id, FltId, FltVal);
  TStr StrVal("5");
  Graph.AddSAttrDatE(Id, StrId, StrVal);

  TAttrPrV AttrV;
  int status = Graph.GetSAttrVE(Id, atInt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph.GetSAttrVE(Id, atFlt, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  status = Graph.GetSAttrVE(Id, atStr, AttrV);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrV.Len());
  //status = Graph.GetSAttrVE(Id, atAny, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(3, AttrV.Len());
  //status = Graph.GetSAttrVE(Id, atUndef, AttrV);
  //EXPECT_EQ(0, status);
  //EXPECT_EQ(0, AttrV.Len());
  //TInt ErrorId(1);
  //status = Graph.GetSAttrVE(ErrorId, atUndef, AttrV);
  //EXPECT_EQ(-1, status);
}

TEST(TModeNet, GetIdVSAttrE) {
  TModeNet Graph;
  Graph = TModeNet();
  TStr IntAttr("TestInt");
  TInt IntId;
  Graph.AddSAttrE(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Graph.AddSAttrE(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Graph.AddSAttrE(StrAttr, atStr, StrId);

  TInt IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  Graph.AddNode(0);
  for (int i = 0; i < 10; i++) {
    Graph.AddNode(i+1);
    Graph.AddEdge(i, i+1, i);
    TInt Id(i);
    Graph.AddSAttrDatE(Id, IntId, IntVal);
    if (i%2 == 0) {
      Graph.AddSAttrDatE(Id, FltId, FltVal);
    }
  }
  Graph.AddSAttrDatE(0, StrId, StrVal);

  TIntV IdV;
  Graph.GetIdVSAttrE(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Graph.GetIdVSAttrE(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Graph.GetIdVSAttrE(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Graph.GetIdVSAttrE(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Graph.GetIdVSAttrE(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Graph.GetIdVSAttrE(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}


// Test node, edge attribute functionality
TEST(TModeNet, AddEdgeNodeAfterAttrAdded) {
  TModeNet Graph;
  Graph = TModeNet();

  TStr StrAttr("name");
  TStr EIntAttr("weight");
  TStr NIntAttr("test");

  Graph.AddStrAttrN(StrAttr);
  Graph.AddIntAttrN(NIntAttr);
  Graph.AddIntAttrE(EIntAttr);

  Graph.AddNode(0);
  Graph.AddNode(1);
  Graph.AddNode(2);

  Graph.AddStrAttrDatN(0, "zero", StrAttr);
  Graph.AddStrAttrDatN(1, "one", StrAttr);
  Graph.AddStrAttrDatN(2, "two", StrAttr);

  Graph.AddEdge(0, 1);
  Graph.AddEdge(1, 2);
  Graph.AddEdge(2, 0);

  for (TNEANet::TEdgeI EI = Graph.BegEI(); EI < Graph.EndEI(); EI++) {
    Graph.AddIntAttrDatE(EI.GetId(), EI.GetId()*3+1, EIntAttr);
  }

  for (TNEANet::TEdgeI EI = Graph.BegEI(); EI < Graph.EndEI(); EI++) {
    TInt AttrVal = Graph.GetIntAttrDatE(EI.GetId(), EIntAttr);
    ASSERT_EQ(EI.GetId()*3+1, AttrVal);
  }
}

TEST(TModeNet, AddNodeAttributeError) {
  TModeNet Graph;
  Graph = TModeNet();

  TStr StrAttr("name");
  TStr FltAttr("weight");
  TStr IntAttr("test");

  Graph.AddStrAttrN(StrAttr);
  Graph.AddFltAttrN(FltAttr);
  Graph.AddIntAttrN(IntAttr);
  int NumNodes = 5;
  for (int i = 0; i < NumNodes; i++) {
    Graph.AddNode(i);
    Graph.AddIntAttrDatN(i, TInt(i), IntAttr);
    Graph.AddFltAttrDatN(i, TFlt(i), FltAttr);
    TInt Val(i);
    Graph.AddStrAttrDatN(i, Val.GetStr(), StrAttr);
  }
  Graph.DelNode(0);
  for (int j = 1; j < NumNodes; j++) {
    ASSERT_EQ(Graph.GetIntAttrDatN(j, IntAttr), j);
    ASSERT_EQ(Graph.GetFltAttrDatN(j, FltAttr), TFlt(j));
    TInt Val(j);
    ASSERT_EQ(Graph.GetStrAttrDatN(j, StrAttr), Val.GetStr());
  }
}

TEST(TModeNet, AddEdgeAttributeError) {
  TModeNet Graph;
  Graph = TModeNet();

  TStr StrAttr("name");
  TStr FltAttr("weight");
  TStr IntAttr("test");

  Graph.AddStrAttrE(StrAttr);
  Graph.AddFltAttrE(FltAttr);
  Graph.AddIntAttrE(IntAttr);
  int NumEdges = 5;
  for (int i = 0; i < NumEdges + 1; i++) {
    Graph.AddNode(i);
  }
  for (int i = 0; i < NumEdges; i++) {
    Graph.AddEdge(i, i+1, i);
    Graph.AddIntAttrDatE(i, TInt(i), IntAttr);
    Graph.AddFltAttrDatE(i, TFlt(i), FltAttr);
    TInt Val(i);
    Graph.AddStrAttrDatE(i, Val.GetStr(), StrAttr);
  }
  Graph.DelNode(0);
  for (int j = 1; j < NumEdges; j++) {
    ASSERT_EQ(Graph.GetIntAttrDatE(j, IntAttr), TInt(j));
    ASSERT_EQ(Graph.GetFltAttrDatE(j, FltAttr), TFlt(j));
    TInt Val(j);
    ASSERT_EQ(Graph.GetStrAttrDatE(j, StrAttr), Val.GetStr());
  }
}
