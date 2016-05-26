#include <gtest/gtest.h>

#include "Snap.h"

// Test the default constructor
TEST(TUndirNet, DefaultConstructor) {
  PUndirNet Graph;

  Graph = TUndirNet::New();

  EXPECT_EQ(0,Graph->GetNodes());
  EXPECT_EQ(0,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(1,Graph->Empty());
  EXPECT_EQ(0,Graph->HasFlag(gfDirected));
}

// Test node, edge creation
TEST(TUndirNet, ManipulateNodesEdges) {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "test.graph.dat";

  PUndirNet Graph;
  PUndirNet Graph1;
  PUndirNet Graph2;
  int i;
  int n;
  int NCount;
  int LCount;
  int x,y;
  int Deg, InDeg, OutDeg;

  Graph = TUndirNet::New();
  EXPECT_EQ(1,Graph->Empty());

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(NNodes,Graph->GetNodes());
  
  // create random edges
  NCount = NEdges;
  LCount = 0;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    if (!Graph->IsEdge(x,y)) {
      n = Graph->AddEdge(x, y);
      NCount--;
      if (x == y) {
        LCount++;
      }
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
  for (TUndirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
  }
  EXPECT_EQ(NNodes,NCount);

  // edges per node iterator, each non-loop is visited twice, each loop once
  NCount = 0;
  for (TUndirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NCount++;
    }
  }
  EXPECT_EQ(NEdges*2-LCount,NCount);

  // edges iterator, each edge is visited once
  NCount = 0;
  for (TUndirNet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    NCount++;
  }
  EXPECT_EQ(NEdges,NCount);

  // node degree
  for (TUndirNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Deg = NI.GetDeg();
    InDeg = NI.GetInDeg();
    OutDeg = NI.GetOutDeg();

    EXPECT_EQ(Deg,InDeg);
    EXPECT_EQ(Deg,OutDeg);
  }

  // assignment
  Graph1 = TUndirNet::New();
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
    Graph2 = TUndirNet::Load(FIn);
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

// Test edge iterator, manipulate edges
TEST(TUndirNet, ManipulateEdges) {
  int Iterations = 100;
  
  int NNodes;
  int NNodesStart = 8;
  int NNodesEnd = 25;
  
  int NEdges;
  int NEdgesStart = 0;
  int NEdgesEnd = 50;
  
  PUndirNet Graph;
  PUndirNet Graph1;
  PUndirNet Graph2;
  int NCount, ECount;
  int x,y;
  TIntV NodeIds;
  THashSet<TIntPr> EdgeSet;
  TInt::Rnd.PutSeed(0);
  
  for (int i = 0; i < Iterations; i++) {
    
    for (NEdges = NEdgesStart; NEdges <= NEdgesEnd; NEdges++) {
      
      for (NNodes = NNodesStart; NNodes <= NNodesEnd; NNodes++) {
        
        // Skip if too many edges required per NNodes (n*(n+1)/2)
        if (NEdges > (NNodes * (NNodes+1)/2)) {
          continue;
        }
        
        Graph = TUndirNet::New();
        EXPECT_TRUE(Graph->Empty());
        
        // Generate NNodes
        NodeIds.Gen(NNodes);
        for (int n = 0; n < NNodes; n++) {
          NodeIds[n] = n;
        }
        // Add the nodes in random order
        NodeIds.Shuffle(TInt::Rnd);
        for (int n = 0; n < NodeIds.Len(); n++) {
          Graph->AddNode(NodeIds[n]);
        }
        EXPECT_FALSE(Graph->Empty());
        
        // Create random edges
        EdgeSet.Clr();
        NCount = NEdges;
        while (NCount > 0) {
          x = (long) (drand48() * NNodes);
          y = (long) (drand48() * NNodes);
          
          if (!Graph->IsEdge(x,y)) {
            Graph->AddEdge(x, y);
            EdgeSet.AddKey(TIntPr(x, y));
            EdgeSet.AddKey(TIntPr(y, x));
            NCount--;
          }
        }
        
        // Check edge iterator to make sure all edges are valid and no more (in hash set)
        TIntPrV DelEdgeV;
        for (TUndirNet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
          
          TIntPr Edge(EI.GetSrcNId(), EI.GetDstNId());
          TIntPr EdgeR(EI.GetDstNId(), EI.GetSrcNId());
          
          EXPECT_TRUE(EdgeSet.IsKey(Edge) || EdgeSet.IsKey(EdgeR));
          if (EdgeSet.IsKey(Edge)) {
              EdgeSet.DelKey(Edge);
          }
          if (EdgeSet.IsKey(EdgeR)) {
            EdgeSet.DelKey(EdgeR);
          }
          DelEdgeV.Add(Edge);
          
        }
        EXPECT_TRUE(EdgeSet.Empty());
        EXPECT_TRUE(DelEdgeV.Len() == NEdges);
        
        // Randomly delete node, check to make sure edges were deleted
        NodeIds.Shuffle(TInt::Rnd);
        for (int n = 0; n < NNodes; n++) {

          TIntPrV DelEdgeNodeV;
          int DelNodeId = NodeIds[n];
          
          // Get edges for node to be deleted (to verify all deleted)
          int EdgesBeforeDel;
          EdgesBeforeDel = Graph->GetEdges();
          for (TUndirNet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
            if (EI.GetSrcNId() == DelNodeId || EI.GetDstNId() == DelNodeId) {
              DelEdgeNodeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
            }
          }
          
          Graph->DelNode(DelNodeId);
          EXPECT_TRUE(EdgesBeforeDel == DelEdgeNodeV.Len() + Graph->GetEdges());
          EXPECT_FALSE(Graph->IsNode(DelNodeId));
          EXPECT_TRUE(Graph->IsOk());
          
          // Make sure all the edges to deleted node are gone
          for (int e = 0; e < DelEdgeNodeV.Len(); e++) {
            EXPECT_FALSE(Graph->IsEdge(DelEdgeNodeV[e].Val1, DelEdgeNodeV[e].Val2));
          }
          
          // Make sure no edge on graph is connected to deleted node
          ECount = 0;
          for (TUndirNet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
            EXPECT_FALSE(EI.GetSrcNId() == DelNodeId || EI.GetDstNId() == DelNodeId);
            ECount++;
          }
          // Make sure iterator returns all of the edges
          EXPECT_TRUE(ECount == Graph->GetEdges());
        }

        EXPECT_TRUE(0 == Graph->GetEdges());
        Graph->Clr();
        EXPECT_TRUE(Graph->Empty());
      }
    }
  }

}

// Test small graph
TEST(TUndirNet, GetSmallGraph) {
  PUndirNet Graph;

  Graph = TUndirNet::GetSmallGraph();

  EXPECT_EQ(5,Graph->GetNodes());
  EXPECT_EQ(5,Graph->GetEdges());

  EXPECT_EQ(1,Graph->IsOk());
  EXPECT_EQ(0,Graph->Empty());
  EXPECT_EQ(0,Graph->HasFlag(gfDirected));
}

TEST(TUndirNet, AddSAttrN) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrIdN) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrNameN) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrDatN_int) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrDatN_flt) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrDatN_str) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrDatN_int) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrDatN_flt) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrDatN_str) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, DelSAttrDatN) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrVN) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetIdVSAttrN) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrE) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrIdE) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrNameE) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrDatE_int) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrDatE_flt) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, AddSAttrDatE_str) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrDatE_int) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrDatE_flt) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrDatE_str) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, DelSAttrDatE) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetSAttrVE) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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

TEST(TUndirNet, GetIdVSAttrE) {
  PUndirNet Graph;
  Graph = TUndirNet::New();
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
