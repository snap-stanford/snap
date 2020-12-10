#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TCrossNet, DefaultConstructor) {
  TCrossNet Graph;

  Graph = TCrossNet();

  EXPECT_EQ(0,Graph.GetEdges());

  EXPECT_EQ(0,Graph.IsDirected());
}

// Test the default constructor
TEST(TCrossNet, DefaultDirectednessConstructors) {
  TCrossNet Graph;
  TInt MId1(0);
  TInt MId2(1);
  TInt CId(0);

  Graph = TCrossNet(MId1, MId2, CId);
  EXPECT_EQ(0,Graph.GetEdges());
  EXPECT_EQ(1,Graph.IsDirected());

  Graph = TCrossNet(MId1, MId2, false, CId);
  EXPECT_EQ(0,Graph.GetEdges());
  EXPECT_EQ(0,Graph.IsDirected());
}

// Test node, edge creation
TEST(TCrossNet, ManipulateNodesEdges) {
  int NNodes = 1000;
  int NEdges = 100000;
  const char *FName = "test.graph.dat";

  TCrossNet Graph;
  TCrossNet Graph1;
  TCrossNet Graph2;
  int n;
  int NCount;
  int x,y;

  Graph = TCrossNet();
  EXPECT_EQ(0,Graph.GetEdges());
  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    n = Graph.AddEdge(x, y);
    NCount--;
  }

  EXPECT_EQ(NEdges,Graph.GetEdges());

  // edges iterator
  NCount = 0;
  for (TCrossNet::TCrossEdgeI EI = Graph.BegEdgeI(); EI < Graph.EndEdgeI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // assignment
  Graph1 = Graph;

  EXPECT_EQ(NEdges,Graph1.GetEdges());

  // saving and loading
  {
    TFOut FOut(FName);
    Graph.Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    Graph2 = TCrossNet(FIn);
  }

  EXPECT_EQ(NEdges,Graph2.GetEdges());

  /*Graph1.Clr(); //TODO implement

  EXPECT_EQ(0,Graph1.GetNodes());
  EXPECT_EQ(0,Graph1.GetEdges());

  EXPECT_EQ(1,Graph1.IsOk());
  EXPECT_EQ(1,Graph1.Empty());*/
}
/*
// Test small graph
TEST(TModeNet, GetSmallGraph) {
  TModeNet Graph;

  return;

  // not implemented
  //Graph = TModeNet::GetSmallGraph();

  EXPECT_EQ(5,Graph.GetNodes());
  EXPECT_EQ(6,Graph.GetEdges());

  EXPECT_EQ(1,Graph.IsOk());
  EXPECT_EQ(0,Graph.Empty());
  EXPECT_EQ(1,Graph.HasFlag(gfDirected));
} */


// Test node, edge attribute functionality
TEST(TCrossNet, ManipulateNodesEdgeAttributes) {
  int NNodes = 1000;
  int NEdges = 1000;
//  const char *FName = "demo.graph.dat";

  TCrossNet Graph;
  TCrossNet Graph1;
  int i;
  int x, y;
  bool t;

  Graph = TCrossNet();
  t = Graph.GetEdges() == 0;

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

  // Test vertical int iterator for edge
  Graph.AddIntAttrDatE(3, 3*2, attr2);
  Graph.AddIntAttrDatE(55, 55*2, attr2);
  Graph.AddIntAttrDatE(705, 705*2, attr2);
  Graph.AddIntAttrDatE(905, 905*2, attr2);

  EXPECT_EQ(3*2, Graph.GetEAIntI(attr2, 3).GetDat());
  EXPECT_EQ(55*2, Graph.GetEAIntI(attr2, 55).GetDat());

  int EdgeId = 0;
  int DefEdges = 0;
  TVec<TInt> TAIntIV = TVec<TInt>();
  for (TCrossNet::TAIntI EI = Graph.BegEAIntI(attr2);
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
  TVec<TFlt> TAFltIV = TVec<TFlt>();

  for (TCrossNet::TAFltI EI = Graph.BegEAFltI(attr3);
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
  TVec<TStr> TAStrIV = TVec<TStr>();

  for (TCrossNet::TAStrI EI = Graph.BegEAStrI(attr1);
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
  TInt AttrLen = EIdAttrName.Len();
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

  int expectedTotal = 0;
  for (i = 0; i <NEdges; i++) {
    Graph.AddIntAttrDatE(i, NEdges+i, attr2);
    EXPECT_EQ(NEdges+i, Graph.GetIntAttrDatE(i, attr2));
    expectedTotal += NEdges+i;
  }

//  {
//    TFOut FOut(FName);
//    Graph.Save(FOut);
//    FOut.Flush();
//    Graph.Clr();
//  }
}


// Test node, edge attribute functionality
TEST(TCrossNet, AddEdgeAfterAttrAdded) {
  TCrossNet Graph;
  Graph = TCrossNet();

  TStr EIntAttr("weight");

  Graph.AddIntAttrE(EIntAttr);

  Graph.AddEdge(0, 1);
  Graph.AddEdge(1, 2);
  Graph.AddEdge(2, 0);

  for (TCrossNet::TCrossEdgeI EI = Graph.BegEdgeI(); EI < Graph.EndEdgeI(); EI++) {
    Graph.AddIntAttrDatE(EI.GetId(), EI.GetId()*3+1, EIntAttr);
  }

  for (TCrossNet::TCrossEdgeI EI = Graph.BegEdgeI(); EI < Graph.EndEdgeI(); EI++) {
    TInt AttrVal = Graph.GetIntAttrDatE(EI.GetId(), EIntAttr);
    ASSERT_EQ(EI.GetId()*3+1, AttrVal);
  }
}
