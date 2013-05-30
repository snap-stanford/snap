#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TNEANet, DefaultConstructor) {
  PNEANet Graph;

  Graph = TNEANet::New();

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());
  EXPECT_EQ(1,Graph->HasFlag(gfDirected));
}

// Test node, edge creation
TEST(TNEANet, ManipulateNodesEdges) {
  int NNodes = 1000;
  int NEdges = 100000;
  const char *FName = "test.graph.dat";

  PNEANet Graph;
  PNEANet Graph1;
  PNEANet Graph2;
  int i;
  int n;
  int NCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Graph = TNEANet::New();
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
    n = Graph->AddEdge(x, y);
    NCount--;
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
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator
  NCount = 0;
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges,NCount);

  // edges iterator
  NCount = 0;
  for (TNEANet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg+OutDeg);
  }

  // assignment
  Graph1 = TNEANet::New();
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
    Graph2 = TNEANet::Load(FIn);
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
TEST(TNEANet, GetSmallGraph) {
  PNEANet Graph;

  return;

  // not implemented
  //Graph = TNEANet::GetSmallGraph();

  EXPECT_EQ(5,Graph->GetNodes());
  EXPECT_EQ(6,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(1,Graph->HasFlag(gfDirected));
}

// Test node, edge attribute functionality
TEST(TNEANet, ManipulateNodesEdgeAttributes) {
  int NNodes = 1000;
  int NEdges = 1000;
  const char *FName = "demo.graph.dat";

  PNEANet Graph;
  PNEANet Graph1;
  int i;
  int x, y;
  bool t;

  Graph = TNEANet::New();
  t = Graph->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }

  EXPECT_EQ(NNodes, Graph->GetNodes());

  // create the edges 
  for (i = 0; i < NEdges; i++) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    Graph->AddEdge(x, y, i);
  }

  // create attributes and fill all nodes
  TStr attr1 = "str";
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr4 = "default";

  // Test vertical int iterator for node 3, 50, 700, 900
  // Check if we can set defaults to 0 for Int data.
  Graph->AddIntAttrN(attr2, 0);
  Graph->AddIntAttrDatN(3, 3*2, attr2);
  Graph->AddIntAttrDatN(50, 50*2, attr2);
  Graph->AddIntAttrDatN(700, 700*2, attr2);
  Graph->AddIntAttrDatN(900, 900*2, attr2);
  int NodeId = 0;
  int DefNodes = 0;
  TVec<TInt> TAIntIV = TVec<TInt>();
  for (TNEANet::TAIntI NI = Graph->BegNAIntI(attr2);
    NI < Graph->EndNAIntI(attr2); NI++) {
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
  Graph->AddFltAttrDatN(5, 3.41, attr3);
  Graph->AddFltAttrDatN(50, 2.718, attr3);
  Graph->AddFltAttrDatN(300, 150.0, attr3);
  Graph->AddFltAttrDatN(653, 563, attr3);
  NodeId = 0;
  DefNodes = 0;
  TVec<TFlt> TAFltIV = TVec<TFlt>();

  for (TNEANet::TAFltI NI = Graph->BegNAFltI(attr3);
    NI < Graph->EndNAFltI(attr3); NI++) {
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
  Graph->AddStrAttrDatN(10, "abc", attr1);
  Graph->AddStrAttrDatN(20, "def", attr1);
  Graph->AddStrAttrDatN(400, "ghi", attr1);
  // this does not show since ""=null
  Graph->AddStrAttrDatN(455, "", attr1);
  NodeId = 0;
  DefNodes = 0;
  TVec<TStr> TAStrIV = TVec<TStr>();

  for (TNEANet::TAStrI NI = Graph->BegNAStrI(attr1);
    NI < Graph->EndNAStrI(attr1); NI++) {
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
  Graph->AddStrAttrDatN(NId, "aaa", attr1);
  Graph->AddIntAttrDatN(NId, 3*2, attr2);
  Graph->AddFltAttrDatN(NId, 3.41, attr3);
  Graph->AddStrAttrDatN(80, "dont appear", attr4); // should not show up
  TStrV NIdAttrName;
  Graph->AttrNameNI(NId, NIdAttrName);
  int AttrLen = NIdAttrName.Len();
  NodeId = 0;
  DefNodes = 0;
  EXPECT_EQ(3, AttrLen);
  
  Graph->DelAttrDatN(NId, attr2);
  Graph->AttrNameNI(NId, NIdAttrName);
  AttrLen = NIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("int") == NIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(1,2);
    }
  }
  EXPECT_EQ(2, AttrLen);

  Graph->AddIntAttrDatN(NId, 3*2, attr2);
  Graph->DelAttrN(attr1);
  Graph->AttrNameNI(NId, NIdAttrName);
  AttrLen = NIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("str") == NIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(1,2);
    }
  }
  EXPECT_EQ(2, AttrLen);
   
  TStrV NIdAttrValue;
  Graph->AttrValueNI(NId, NIdAttrValue);
  AttrLen = NIdAttrValue.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("str") == NIdAttrValue[i]()) {
      // FAIL
      EXPECT_EQ(1,2);
    }
  } 

  for (i = 0; i <NNodes; i++) {
    Graph->AddIntAttrDatN(i, 70, attr2);
  }

  {
    TFOut FOut(FName);
    Graph->Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    Graph1 = TNEANet::Load(FIn);
  }

  int total = 0;
  for (TNEANet::TAIntI NI = Graph1->BegNAIntI(attr2);
    NI < Graph1->EndNAIntI(attr2); NI++) {
    total += NI.GetDat();
  }

  ASSERT_EQ(70, total/NNodes);

  Graph1->Clr();

  // Test vertical int iterator for edge
  Graph->AddIntAttrDatE(3, 3*2, attr2);
  Graph->AddIntAttrDatE(55, 55*2, attr2);
  Graph->AddIntAttrDatE(705, 705*2, attr2);
  Graph->AddIntAttrDatE(905, 905*2, attr2);
  int EdgeId = 0;
  int DefEdges = 0;
  TAIntIV.Clr();
  for (TNEANet::TAIntI EI = Graph->BegEAIntI(attr2);
    EI < Graph->EndEAIntI(attr2); EI++) {
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
  Graph->AddFltAttrE(attr3, 0.00);
  Graph->AddFltAttrDatE(5, 4.41, attr3);
  Graph->AddFltAttrDatE(50, 3.718, attr3);
  Graph->AddFltAttrDatE(300, 151.0, attr3);
  Graph->AddFltAttrDatE(653, 654, attr3);
  EdgeId = 0;
  DefEdges = 0;
  TAFltIV.Clr();

  for (TNEANet::TAFltI EI = Graph->BegEAFltI(attr3);
    EI < Graph->EndEAFltI(attr3); EI++) {
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
  Graph->AddStrAttrDatE(10, "abc", attr1);
  Graph->AddStrAttrDatE(20, "def", attr1);
  Graph->AddStrAttrDatE(400, "ghi", attr1);
  // this does not show since ""=null
  Graph->AddStrAttrDatE(455, "", attr1);
  EdgeId = 0;
  DefEdges = 0;
  TAStrIV.Clr();

  for (TNEANet::TAStrI EI = Graph->BegEAStrI(attr1);
    EI < Graph->EndEAStrI(attr1); EI++) {
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
  Graph->AddStrAttrDatE(EId, "aaa", attr1);
  Graph->AddIntAttrDatE(EId, 3*2, attr2);
  Graph->AddFltAttrDatE(EId, 3.41, attr3);
  Graph->AddStrAttrDatE(80, "dont appear", attr4); // should not show up  

  TStrV EIdAttrName;
  Graph->AttrNameEI(EId, EIdAttrName);
  AttrLen = EIdAttrName.Len();
  EXPECT_EQ(3, AttrLen);
  
  Graph->DelAttrDatE(EId, attr2);
  Graph->AttrNameEI(EId, EIdAttrName);
  AttrLen = EIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("int") == EIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(2,3);
    }
  }

  Graph->AddIntAttrDatE(EId, 3*2, attr2);
  Graph->DelAttrE(attr1);
  Graph->AttrNameEI(EId, EIdAttrName);
  AttrLen = EIdAttrName.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("aaa") == EIdAttrName[i]()) {
      // FAIL
      EXPECT_EQ(2,3);
    }
  }

  TStrV EIdAttrValue;
  Graph->AttrValueEI(EId, EIdAttrValue);
  AttrLen = EIdAttrValue.Len();
  for (i = 0; i < AttrLen; i++) {
    if (TStr("str") == EIdAttrValue[i]()) {
      // FAIL
      EXPECT_EQ(2,3);
    }
  }

  for (i = 0; i <NEdges; i++) {
    Graph->AddIntAttrDatE(i, 70, attr2);
  }

  {
    TFOut FOut(FName);
    Graph->Save(FOut);
    FOut.Flush();
    Graph->Clr();
  }

  {
    TFIn FIn(FName);
    Graph1 = TNEANet::Load(FIn);
  }

  total = 0;
  for (TNEANet::TAIntI EI = Graph1->BegNAIntI(attr2);
    EI < Graph1->EndNAIntI(attr2); EI++) {
    total += EI.GetDat();
  }

  EXPECT_EQ(70, total/NEdges);

  //Graph1->Dump();
  Graph1->Clr();
}
