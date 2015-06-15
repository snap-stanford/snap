/*
Copyright (c) 2015 by Rosalba Giugno

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

RI is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef RIGRAPH_H
#define RIGRAPH_H

#include "Snap.h"

enum TMamaParent { mpParentTypeIn, mpParentTypeOut, mpParentTypeNull };
enum TMatch { mIso, mMono, mInd};

//#//////////////////////////////////////////////
/// Abstract class to manage candidates list. ##Abstract_candidates_List 
class TNodeList {
public:
  virtual ~TNodeList() {};
  virtual int GetNode(int Id) = 0;
};

//#//////////////////////////////////////////////
/// Adjacency list of in nodes. #In_adjacency_list
template<class TNodeData, class TEdgeData>
class TInAdjList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
public:
  TInAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet, int NNodeId):
    Net(NNet), NodeId(NNodeId) { }
  /// Return the Id-th In node
  int GetNode(int Id) {
    return Net->GetNI(NodeId).GetInNId(Id);
  }
};

//#//////////////////////////////////////////////
/// Adjacency list of out nodes. #Out_adjacency_list
template<class TNodeData, class TEdgeData>
class TOutAdjList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
public:
  TOutAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet, int NNodeId):
    Net(NNet), NodeId(NNodeId) {}
  /// Return the Id-th Out node
  int GetNode(int Id) {
    return Net->GetNI(NodeId).GetOutNId(Id);
  }
};

//#//////////////////////////////////////////////
/// List of all graph's nodes ##All_nodes_list
template<class TNodeData, class TEdgeData>
class TAllNodesList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
public:
  TAllNodesList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet): Net(NNet) {}
  /// Return node with Id 
  int GetNode(int Id) {
    return Net->GetNI(Id).GetId();
  }
};

/// Return the degree of node Id
template<class TNodeData, class TEdgeData>
int GetAdjSize(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int Id) {
  return Graph->GetNI(Id).GetDeg();
}

/// Return the in degree of node Id
template<class TNodeData, class TEdgeData>
int GetInAdjSize(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int Id) {
  return Graph->GetNI(Id).GetInDeg();
}

/// Return the out degree of node Id
template<class TNodeData, class TEdgeData>
int GetOutAdjSize(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int Id) {
  return Graph->GetNI(Id).GetOutDeg();
}

/// Return the N-th node in the adjacency list of node NId
template<class TNodeData, class TEdgeData>
int GetNodeInAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  typename TNodeEDatNet<TNodeData, TEdgeData>::TNodeI NI;
  NI = Graph->GetNI(NId);
  return NI.GetNbrNId(N);
};

/// Return the N-th node in the out adjacency list of node NId
template<class TNodeData, class TEdgeData>
int GetNthOutAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  return Graph->GetNI(NId).GetOutNId(N);
}

/// Return the N-th node in the in adjacency list of node NId
template<class TNodeData, class TEdgeData>
int GetNthInAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  return Graph->GetNI(NId).GetInNId(N);
}

//#//////////////////////////////////////////////
/// State machine used to build pre-candidates
template<class TNodeData, class TEdgeData>
class TMatchingMachine {
  
public:
  //#//////////////////////////////////////////////
  /// Edge class used by matching machine
  class TMaMaEdge {
  private:
    int Source;
    int Target;
    TEdgeData* Attr;
  public:
    int GetSource() const { return Source; }
    int GetTarget() const { return Target; }
    TEdgeData* GetAttr() const { return Attr; }
    
    void SetSource(const int ValSource){ Source = ValSource; }
    void SetTarget(const int ValTarget){ Target = ValTarget; }
    void SetAttr(TEdgeData* ValAttr) { Attr = ValAttr; }
    
    TMaMaEdge(int NSource, int NTarget, TEdgeData* NAttr) {
      Source = NSource;
      Target = NTarget;
      Attr = NAttr;
    }
    TMaMaEdge() {
      Source = -1;
      Target = -1;
      Attr = NULL;
    }
  };
  
protected:
  int NofSn;
  TNodeData** NodesAttrs;
  int* EdgesSizes;
  int* OutEdgesSizes;				//indexed by state_id
  int* InEdgesSizes;
  TMaMaEdge** Edges;
  int* MapNodeToState;
  int* MapStateToNode;
  int* ParentState;
  TMamaParent* ParentType;
  
private:
  // Compare the weights of the given nodes i,j
  int WCompare(int i, int j, int** Weights) {
    for (int w = 0; w < 3; w++) {
      if (Weights[i][w] != Weights[j][w]) {
        return Weights[j][w] - Weights[i][w];
      }
    }
    return i-j;
  }
  
  void Increase(int* NS, int* SIS, int i, int** Weights, int LeftLimit) {
    int temp;
    while (i > LeftLimit && (WCompare(NS[i], NS[i-1], Weights) < 0)) {
      temp = NS[i-1];
      NS[i-1] = NS[i];
      NS[i] = temp;
      temp = SIS[NS[i-1]];
      SIS[NS[i-1]] = SIS[NS[i]];
      SIS[NS[i]] = temp;
      i--;
    }
  }
  
public:
  TMatchingMachine(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Query) {
    NofSn = Query->GetNodes();
    NodesAttrs = new TNodeData*[NofSn];
    EdgesSizes = (int*) calloc(NofSn, sizeof(int));
    OutEdgesSizes = (int*) calloc(NofSn, sizeof(int));
	InEdgesSizes = (int*) calloc(NofSn, sizeof(int));
    Edges = new TMaMaEdge*[NofSn];
    MapNodeToState = (int*) calloc(NofSn, sizeof(int));
    MapStateToNode = (int*) calloc(NofSn, sizeof(int));
    ParentState = (int*) calloc(NofSn, sizeof(int));
    ParentType = new TMamaParent[NofSn];
  }
  
  virtual ~TMatchingMachine() {
    delete[] NodesAttrs;
    for (int i = 0; i < NofSn; i++) {
      delete[] Edges[i];
    }
    delete[] Edges;
    free(EdgesSizes);
    free(OutEdgesSizes);
    free(InEdgesSizes);
    free(MapNodeToState);
    free(MapStateToNode);
    free(ParentState);
    delete[] ParentType;
  }
  
  /// Preprocessing phase to determine candidate nodes
  void Build(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &SSG) {
    enum TNodeFlag { nsCore, nsCNeigh, nsUnv };
    TNodeFlag* NodeFlags = new TNodeFlag[NofSn];
    int** Weights = new int*[NofSn];
    int* ParentNode = (int*) calloc(NofSn, sizeof(int));
    TMamaParent* ParentTypes = new TMamaParent[NofSn];

    for (int i = 0; i < NofSn; i++) {
      NodeFlags[i] = nsUnv;
      Weights[i] = new int[3];
      Weights[i][0] = 0;
      Weights[i][1] = 0;
      Weights[i][2] = GetAdjSize(SSG, i);
      ParentNode[i] = -1;
      ParentTypes[i] = mpParentTypeNull;
    }

    int SId = 0;
    int n;
    int NIT; int NId;
    int NNIT; int NNId;
    int NQueueL = 0, NQueueR = 0;
    int MaxId, MaxV;
    int Tmp;

    while (SId < NofSn) {
      if (NQueueL == NQueueR) {
        //if queue is empty....
        MaxId = -1;
        MaxV = -1;
        NIT = 0;
        while (NIT < NofSn) {
          if (NodeFlags[NIT] == nsUnv && Weights[NIT][2] > MaxV) {
            MaxV = Weights[NIT][2];
            MaxId = NIT;
          }
          NIT++;
        }
        MapStateToNode[SId] = MaxId;
        MapNodeToState[MaxId] = SId;
        ParentTypes[MaxId] = mpParentTypeNull;
        ParentNode[MaxId] = -1;

        NQueueR++;
        n = MaxId;
        NIT = 0;
        
        while(NIT < GetOutAdjSize(SSG, n)){ 
          NId = GetNthOutAdjList(SSG, n, NIT);
          if (NId != n) {
            Weights[NId][1]++;
          }
          NIT++;
        }
        while(NIT < GetInAdjSize(SSG, n)){ 
          NId = GetNthInAdjList(SSG, n, NIT);
          if (NId != n) {
            Weights[NId][1]++;
          }
          NIT++;
        }
      }

      if (NQueueL != NQueueR-1) {
         MaxId = NQueueL;
         for (int mi = MaxId+1; mi < NQueueR; mi++) {
           if (WCompare(MapStateToNode[mi], MapStateToNode[MaxId], Weights) < 0) {
             MaxId = mi;
           }
         }
         Tmp = MapStateToNode[NQueueL];
         MapStateToNode[NQueueL] = MapStateToNode[MaxId];
         MapStateToNode[MaxId] = Tmp;
      }

      n = MapStateToNode[SId];
      MapNodeToState[n] = SId;
      //move queue left limit
      NQueueL++;
      //update nodes' flags & weights
      NodeFlags[n] = nsCore;
      NIT = 0;
      while (NIT < GetOutAdjSize(SSG, n)) {
        NId = GetNthOutAdjList(SSG, n, NIT);
        if (NId != n) {
          Weights[NId][0]++;
          Weights[NId][1]--;

          if (NodeFlags[NId] == nsUnv) {
            NodeFlags[NId] = nsCNeigh;
            ParentNode[NId] = n;
            if (NIT < GetOutAdjSize(SSG, n)) {
              ParentTypes[NId] = mpParentTypeOut;
            } else {
              ParentTypes[NId] = mpParentTypeIn;
            }
            //add to queue
            MapStateToNode[NQueueR] = NId;
            MapNodeToState[NId] = NQueueR;
            NQueueR++;

            NNIT = 0;
            while (NNIT < GetOutAdjSize(SSG, NId)) {
              NNId = GetNthOutAdjList(SSG, NId, NNIT);
              Weights[NNId][1]++;
              NNIT++;
            }
          }
        }
        NIT++;
      }
      
      NIT = 0;
      while (NIT < GetInAdjSize(SSG, n)) {
        NId = GetNthInAdjList(SSG, n, NIT);
        if (NId != n) {
          Weights[NId][0]++;
          Weights[NId][1]--;

          if (NodeFlags[NId] == nsUnv) {
            NodeFlags[NId] = nsCNeigh;
            ParentNode[NId] = n;
            if (NIT < GetOutAdjSize(SSG, n)) {
              ParentTypes[NId] = mpParentTypeOut;
            } else {
              ParentTypes[NId] = mpParentTypeIn;
            }
            //add to queue
            MapStateToNode[NQueueR] = NId;
            MapNodeToState[NId] = NQueueR;
            NQueueR++;

            NNIT = 0;
            while (NNIT < GetInAdjSize(SSG, NId)) {
              NNId = GetNthInAdjList(SSG, NId, NNIT);
              Weights[NNId][1]++;
              NNIT++;
            }
          }
        }
        NIT++;
      }
      SId++;
    }

    int ECount, OutECount, InECount; int i;
    for (SId = 0; SId < NofSn; SId++) {
      n = MapStateToNode[SId];

      //NodesAttrs[SId] = &SSG->GetNI(n).GetDat();
      if (ParentNode[n] != -1) {
        ParentState[SId] = MapNodeToState[ParentNode[n]];
      } else {
        ParentState[SId] = -1;
      }
      ParentType[SId] = ParentTypes[n];
      ECount = 0;
      OutECount = 0;

      for (i = 0; i < GetOutAdjSize(SSG, n); i++) {
        if (MapNodeToState[GetNthOutAdjList(SSG, n, i)] < SId) {
            ECount++;
            OutECount++;
        }
      }

      InECount = 0;
      for (i = 0; i < GetInAdjSize(SSG, n); i++) {
        if (MapNodeToState[GetNthInAdjList(SSG, n, i)] < SId) {
            ECount++;
            InECount++;
        }
      }
      
      EdgesSizes[SId] = ECount;
      OutEdgesSizes[SId] = OutECount;
      InEdgesSizes[SId] = InECount;
      Edges[SId] = new TMaMaEdge[ECount];
      ECount = 0;
      for (i = 0; i < GetOutAdjSize(SSG, n); i++) {
        int m = GetNthOutAdjList(SSG, n, i);
        if (MapNodeToState[m] < SId) {
          Edges[SId][ECount].SetSource(MapNodeToState[n]);
          Edges[SId][ECount].SetTarget(MapNodeToState[m]);
          Edges[SId][ECount].SetAttr(&SSG->GetEI(n, m).GetDat());
          ECount++;
        }
      }
      for (i = 0; i < GetInAdjSize(SSG, n);i++) {
        int m = GetNthInAdjList(SSG, n, i);
        if (MapNodeToState[m] < SId) {

          Edges[SId][ECount].SetTarget(MapNodeToState[n]);
          Edges[SId][ECount].SetSource(MapNodeToState[m]);
          Edges[SId][ECount].SetAttr(&SSG->GetEI(m, n).GetDat());
          ECount++;
        }
      }
    }

    delete[] NodeFlags;
    for (int i = 0; i < NofSn; i++) {
      delete[] Weights[i];
    }
    delete[] Weights;
    free(ParentNode);
    delete[] ParentTypes;

  }
  
  void print() {
    printf("| MatchingMachine:  nof states %d\n", NofSn);
    printf("| \tmap state_to_node(");
    
    for (int i = 0; i < NofSn; i++) {
      printf("[%d: %d]", i, MapStateToNode[i]);
    }
    printf(")\n");
    printf("| \tmap node_to_state(");
    for (int i = 0; i < NofSn; i++) {
      printf("[: %d]", i, MapNodeToState[i]);
    }
    printf(")\n");
    printf("| \tstates (node)(parent state, parent type)\n");
    for (int i = 0; i < NofSn; i++) {
      printf("| \t\t[%d] (%d) (%d, ", i, MapStateToNode[i], ParentState[i]);
      switch (ParentType[i]) {
        case mpParentTypeIn:
          printf("IN");
          break;
        case mpParentTypeOut:
          printf("OUT");
          break;
        case mpParentTypeNull:
          printf("NULL");
          break;
      }
      printf(")\n");
      printf("| \t\t\tchecking[%d] ", EdgesSizes[i]);
      for (int j = 0; j < EdgesSizes[i]; j++) {
        printf("{s(%d,%d):", Edges[i][j].GetSource(), Edges[i][j].GetTarget());
        printf("n(%d,%d)}", MapStateToNode[Edges[i][j].GetSource()],
          MapStateToNode[Edges[i][j].GetTarget()]);
      }
      printf("\n");
    }
  }
  
  TMaMaEdge** GetEdges() const { return Edges; }
  int* GetEdgesSizes() const { return EdgesSizes; }
  int* GetOutEdgesSizes() const { return OutEdgesSizes; }
  int* GetInEdgesSizes() const { return InEdgesSizes; }
  int* GetMapNodeToState() const { return MapNodeToState; }
  int* GetMapStateToNode() const { return MapStateToNode; }
  TNodeData** GetNodesAttrs() const { return NodesAttrs; }
  int GetNofSn() const { return NofSn; }
  int* GetParentState() const { return ParentState;}
  TMamaParent* GetParentType() const { return ParentType; }
  void SetEdges(TMaMaEdge** edges) { this->Edges = edges; }
  void SetEdgesSizes(int* edges_sizes) { this->EdgesSizes = edges_sizes; }
  void SetMapNodeToState(int* map_node_to_state) {
    this->MapNodeToState = map_node_to_state;
  }
  void SetMapStateToNode(int* map_state_to_node) {
    this->MapStateToNode = map_state_to_node;
  }
  void SetNodesAttrs(TNodeData** nodes_attrs) {
    this->NodesAttrs = nodes_attrs;
  }
  void SetNofSn(int nof_sn) { this->NofSn = nof_sn; }
  void SetParentState(int* parent_state) {
    this->ParentState = parent_state;
  }
  void SetParentType(TMamaParent* parent_type) {
    this->ParentType = parent_type;
  }
};

//#//////////////////////////////////////////////
/// Simple listener class
class TMatchListener {
protected:
  long MatchCount;
public:
  TMatchListener() { MatchCount = 0; }
  virtual ~TMatchListener() {};
  long GetMatchCount() const { return MatchCount; }
  void SetMatchCount(long NMatchCount) { MatchCount = NMatchCount; }
  virtual void Match(int n, int* QIds, int* RIds) = 0;
};

//#//////////////////////////////////////////////
/// Empty listener
class TEmptyMatchListener : public TMatchListener {
public:
  TEmptyMatchListener() : TMatchListener() {}
  virtual void Match(int n, int* QIds, int* RIds) {
    MatchCount++;
  };
};

//#//////////////////////////////////////////////
/// Console listener
class TConsoleMatchListener : public TMatchListener {
public:
  TConsoleMatchListener() : TMatchListener() {}
  
  virtual void Match(int n, int* QIds, int* RIds) {
    MatchCount++;
    printf("{");
    for (int i = 0; i < n; i++) {
      printf("(%d,%d)", QIds[i], RIds[i]);
    }
    printf("}\n");
  }
};

//#//////////////////////////////////////////////
/// Solver class includes matching algorithm
template<class TNodeData, class TEdgeData>
class TSolver {
protected:
  TMatchingMachine<TNodeData, TEdgeData> &MaMa;
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &RGraph;
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &QGraph;
  TMatchListener &MatchListener;
public:
  TSolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener) : MaMa(NMaMa),
      RGraph(NRGraph), QGraph(NQGraph), MatchListener(NMatchListener) {
  }
  virtual ~TSolver() {}
  
  //Matching algorithm
  void Solve() {
    int IId;
    int NofSn = MaMa.GetNofSn();
    int* MapStateToNode = MaMa.GetMapStateToNode();
    int* ParentState = MaMa.GetParentState();
    TMamaParent* ParentType = MaMa.GetParentType();
    TNodeList* ListAllRef = new TAllNodesList<TNodeData, TEdgeData>(RGraph);
    TNodeList** Candidates = new TNodeList*[NofSn];
    int* CandidatesIt = new int[NofSn];
    int* CandidatesSize = new int[NofSn];
    int* Solution = new int[NofSn];

    for (IId = 0; IId < NofSn; IId++) {
      Solution[IId] = -1;
    }

    bool* Matched = (bool*) calloc(RGraph->GetNodes(), sizeof(bool));
    Candidates[0] = ListAllRef;
    CandidatesSize[0] = RGraph->GetNodes();
    CandidatesIt[0] = -1;
    int PSId = -1;
    int SId = 0;
    int CId = -1;
    int Sip1;
    
    while (SId != -1) {
      if (PSId >= SId) { Matched[Solution[SId]] = false; }
      CId = -1;
      CandidatesIt[SId]++;
      while (CandidatesIt[SId] < CandidatesSize[SId]) {
        CId = Candidates[SId]->GetNode(CandidatesIt[SId]);
        Solution[SId] = CId;
        if ((!Matched[CId]) &&
             NodeCheck(SId,CId, MapStateToNode) &&
             EdgesCheck(SId, CId, Solution, Matched)) {
          break;
        } else {
          CId = -1;
        }
        CandidatesIt[SId]++;
      }
      
      if (CId == -1) {
        PSId = SId;
	delete Candidates[SId];
        SId--;
      } else {
        //MatchedCouples++;
        if (SId == NofSn -1) {
          MatchListener.Match(NofSn, MapStateToNode, Solution);
          PSId = SId;
        } else {
          Matched[Solution[SId]] = true;
          Sip1 = SId + 1;
          if (ParentType[Sip1] == mpParentTypeNull) {
            Candidates[Sip1] = ListAllRef;
            CandidatesSize[Sip1] = RGraph->GetNodes();
          } else {
            if (ParentType[Sip1] == mpParentTypeIn) {
              Candidates[Sip1] = new TInAdjList<TNodeData, TEdgeData>(RGraph,
                Solution[ParentState[Sip1]]);
              CandidatesSize[Sip1] = GetInAdjSize(RGraph,
                Solution[ParentState[Sip1]]);
            } else {//(parent_type[sip1] == MAMA_PARENTTYPE::PARENTTYPE_OUT)
              Candidates[Sip1] = new TOutAdjList<TNodeData, TEdgeData>(RGraph,
                Solution[ParentState[Sip1]]);
              CandidatesSize[Sip1] = GetOutAdjSize(RGraph,
                Solution[ParentState[Sip1]]);
            }
          }
          CandidatesIt[SId+1] = -1;
          PSId = SId;
          SId++;
        }
      }
    }
  }
  virtual bool NodeCheck(int SId, int CId, int* MapStateToNode) = 0;
  virtual bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched) = 0;
};

//#//////////////////////////////////////////////
/// Search for induced subgraph matching
template<class TNodeData, class TEdgeData>
class TInducedSubGISolver : public TSolver<TNodeData, TEdgeData> {
using TSolver<TNodeData, TEdgeData>::RGraph;
using TSolver<TNodeData, TEdgeData>::QGraph;
using TSolver<TNodeData, TEdgeData>::MaMa;

public:
  TInducedSubGISolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener) : TSolver<TNodeData, TEdgeData>(NMaMa,
      NRGraph, NQGraph, NMatchListener) {
  }
    
  /// Check if one node in the current comparison has a match
  bool NodeCheck(int SId, int CId, int* MapStateToNode) {
    if (GetOutAdjSize(RGraph, CId) >= GetOutAdjSize(QGraph, MapStateToNode[SId]) &&
         GetInAdjSize(RGraph, CId) >= GetInAdjSize(QGraph, MapStateToNode[SId])) {
      return (RGraph->GetNI(CId).GetDat() ==
               QGraph->GetNI(MapStateToNode[SId]).GetDat());
    }
    return false;
  }
  
  /// Check if the edge in the current comparison has a match
  bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched) {
    int Source, Target;
    int IId;
    
    for (int me = 0; me < MaMa.GetEdgesSizes()[SId]; me++) {
      Source = Solution[MaMa.GetEdges()[SId][me].GetSource()];
      Target = Solution[MaMa.GetEdges()[SId][me].GetTarget()];
      for (IId = 0; IId < GetOutAdjSize(RGraph, Source); IId++) {
        if (GetNthOutAdjList(RGraph, Source, IId) == Target) {
          if (RGraph->GetEI(Source, GetNthOutAdjList(RGraph, Source, IId)).GetDat() != 
               *MaMa.GetEdges()[SId][me].GetAttr()) {
            return false;
          } else {
            break;
          }
        }
      }
      if (IId >= GetOutAdjSize(RGraph, Source)) {
        return false;
      }
    }
    
    int Count = 0;
    for (IId = 0; IId < GetOutAdjSize(RGraph, CId); IId++) {
      if (Matched[GetNthOutAdjList(RGraph, CId, IId)]) {
        Count++;
        if (Count > MaMa.GetOutEdgesSizes()[SId])
          return false;
      }
    }
    Count = 0;
    for (IId = 0; IId < GetInAdjSize(RGraph, CId); IId++) {
      if (Matched[GetNthInAdjList(RGraph, CId, IId)]) {
        Count++;
        if (Count > MaMa.GetInEdgesSizes()[SId])
          return false;
      }
    }
    return true;
  }
};

//#//////////////////////////////////////////////
/// Search for graph isomorphism
template<class TNodeData, class TEdgeData>
class TIsoGISolver : public TSolver<TNodeData, TEdgeData> {
using TSolver<TNodeData, TEdgeData>::RGraph;
using TSolver<TNodeData, TEdgeData>::QGraph;
using TSolver<TNodeData, TEdgeData>::MaMa;
public:
  
  TIsoGISolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener) : TSolver<TNodeData, TEdgeData>(NMaMa,
      NRGraph, NQGraph, NMatchListener) {}
    
  /// Check if one node in the current comparison has a match
  bool NodeCheck(int SId, int CId, int* MapStateToNode) {
    if (GetOutAdjSize(RGraph, CId) == GetOutAdjSize(QGraph, MapStateToNode[SId]) &&
         GetInAdjSize(RGraph, CId) == GetInAdjSize(QGraph, MapStateToNode[SId])) {
      return (RGraph->GetNI(CId).GetDat() ==
              QGraph->GetNI(MapStateToNode[SId]).GetDat());
    }
    return false;
  }
  
  /// Check if the edge in the current comparison has a match
  bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched) {
    int Source, Target;
    int IId;
    for (int me = 0; me < MaMa.GetEdgesSizes()[SId]; me++) {
      Source = Solution[MaMa.GetEdges()[SId][me].GetSource()];
      Target = Solution[MaMa.GetEdges()[SId][me].GetTarget()];

      for (IId = 0; IId < GetOutAdjSize(RGraph, Source); IId++) {
        if (GetNthOutAdjList(RGraph, Source, IId) == Target) {
          if (RGraph->GetEI(Source, GetNthOutAdjList(RGraph, Source, IId)).GetDat() !=
               *MaMa.GetEdges()[SId][me].GetAttr()) {
            return false;
          } else {
            break;
          }
        }
      }
      if (IId >= GetOutAdjSize(RGraph, Source)) {
        return false;
      }
    }
    return true;
  }
};

//#//////////////////////////////////////////////
///
template <class TNodeData, class TEdgeData>
class TSubGISolver : public TSolver<TNodeData, TEdgeData> {
using TSolver<TNodeData, TEdgeData>::RGraph;
using TSolver<TNodeData, TEdgeData>::QGraph;
using TSolver<TNodeData, TEdgeData>::MaMa;
public:
  TSubGISolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener) : TSolver<TNodeData, TEdgeData>(NMaMa,
      NRGraph, NQGraph, NMatchListener) {
  }
  /// Check if one node in the current comparison has a match
  bool NodeCheck(int SId, int CId, int* MapStateToNode) {
    if (GetOutAdjSize(RGraph, CId) >= GetOutAdjSize(QGraph, MapStateToNode[SId]) &&
         GetInAdjSize(RGraph, CId) >= GetInAdjSize(QGraph, MapStateToNode[SId])) {
        return (RGraph->GetNI(CId).GetDat() ==
                QGraph->GetNI(MapStateToNode[SId]).GetDat());
    }
    return false;
  }
  
  /// Check if the edge in the current comparison has a match
  bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched) {
    int Source, Target;
    int IId;
    for (int me = 0; me < MaMa.GetEdgesSizes()[SId]; me++) {
      Source = Solution[MaMa.GetEdges()[SId][me].GetSource()];
      Target = Solution[MaMa.GetEdges()[SId][me].GetTarget()];
      
      for (IId = 0; IId < GetOutAdjSize(RGraph, Source); IId++) {
        if (GetNthOutAdjList(RGraph, Source, IId) == Target) {
          if (RGraph->GetEI(Source, GetNthOutAdjList(RGraph, Source, IId)).GetDat() !=
               *MaMa.GetEdges()[SId][me].GetAttr()) {
            return false;
          } else {
            break;
          }
        }
      }
      if (IId >= GetOutAdjSize(RGraph, Source)) {
        return false;
      }
    }
    return true;
  }
};

/// Create and call the correct solver for the current problem
template<class TNodeData, class TEdgeData>
void Match(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Reference,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Query,
  TMatchingMachine<TNodeData, TEdgeData> &MatchingMachine,
  TMatchListener &MatchListener,
  TMatch MatchType) {
    TSolver<TNodeData, TEdgeData>* Solver;
    switch (MatchType) {
      case mIso:
        Solver = new TIsoGISolver<TNodeData, TEdgeData>(MatchingMachine,
          Reference,
          Query,
          MatchListener);
        break;
      case mMono:
        Solver = new TSubGISolver<TNodeData, TEdgeData>(MatchingMachine,
          Reference,
          Query,
          MatchListener);
        break;
      case mInd:
        Solver = new TInducedSubGISolver<TNodeData, TEdgeData>(MatchingMachine,
          Reference,
          Query,
          MatchListener);
        break;
    }
    Solver->Solve();
};

/// Declaration of Match function
template <class TNodeData, class TEdgeData>
int Match(TMatch matchtype, TPt<TNodeEDatNet<TNodeData, TEdgeData> > &RefGraph,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &QueGraph);

/// Matches a reference and a query graph given a match type
template <class TNodeData, class TEdgeData> 
int Match(TMatch Matchtype, 
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &RefGraph,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &QueGraph) {
  
  TMatchingMachine<TNodeData, TEdgeData>* MaMa =
    new TMatchingMachine<TNodeData, TEdgeData>(QueGraph);
  MaMa->Build(QueGraph);

  long MatchCount = 0;

  TMatchListener* MatchListener = new TEmptyMatchListener();

  Match(RefGraph,
    QueGraph,
    *MaMa,
    *MatchListener,
    Matchtype);

 MatchCount = MatchListener->GetMatchCount();
 delete MatchListener;

#ifdef CSV_FORMAT
 
#else
 printf("number of found matches: %d\n", MatchCount);
#endif
  return 0;
};
#endif
