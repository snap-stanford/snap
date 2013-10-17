#include <time.h>

namespace TSnap {

typedef TVec<TInt, int> TIntV;
typedef TVec<TIntV, int> TIntIntVV;
typedef THash<TInt, TVec< TInt, int> > TIntIntVH;
//typedef THash<TInt, TInt> TIntH;

void SeedRandom() {
  long int ITime;
  long int IPid;
  long int RSeed;

  ITime = (long int) time(NULL);
  IPid = (long int) getpid();

  RSeed = ITime * IPid;
  srand48(RSeed);
}

void Randomize(TIntV& Vec) {
  int Pos;
  int Last = Vec.Len() - 1;
  for (int ValN = Last; ValN > 0; ValN--) {
    Pos = (long) (drand48() * ValN);
    Vec.Swap(ValN, Pos);
  }
}

int StdDist(double Mean, double Dev) {
  int i;
  double x;

  x = -6.0;
  for (i = 0; i < 12; i++) {
    x += drand48();
  }

  x *= Dev;
  x += Mean;

  return int(x + 0.5);
}

#if 0
void GetDegrees(TIntV* Nodes, double Mean, double Dev) {
  int i;
  int d;
  int Len;
  printf("GetDegrees\n");
  printf("Nodes Len %d\n",Nodes->Len());

  Len = Nodes->Len();
  for (i = 0; i < Len; i++) {
    d = StdDist(Mean, Dev);
    printf("degree1 %d %d\n", i, d);
    (*Nodes)[i] = d;
  }

  for (i = 0; i < Len; i++) {
    printf("degree2 %d %d\n", i, Nodes->GetVal(i).Val);
  }
}
#endif

void GetDegrees(TIntV& Nodes, double Mean, double Dev) {
  int i;
  int d;
  int Len;
  printf("GetDegrees\n");
  printf("Nodes Len %d\n",Nodes.Len());

  // assign degree to each node
  Len = Nodes.Len();
  for (i = 0; i < Len; i++) {
    d = StdDist(Mean, Dev);
    printf("degree1 %d %d\n", i, d);
    Nodes[i] = d;
  }

  for (i = 0; i < Len; i++) {
    printf("degree2 %d %d\n", i, Nodes[i].Val);
  }
}

void IncVal(TIntV& Nodes, int disp) {
  int i;
  int Len;

  // increment value for each element
  Len = Nodes.Len();
  for (i = 0; i < Len; i++) {
    Nodes[i] += disp;
  }
}

void AssignRndTask(const TIntV& Nodes, TIntIntVV& Tasks) {
  int i;
  int j;
  int n;
  int t;
  int NumNodes;
  int NumTasks;
  printf("AssignRndTask\n");
  printf("Nodes Len %d\n",Nodes.Len());
  printf("Tasks Len %d\n",Tasks.Len());

  NumNodes = Nodes.Len();
  NumTasks = Tasks.Len();

  // distribute stubs randomly to tasks
  for (i = 0; i < NumNodes; i++) {
    n = Nodes[i].Val;
    printf("degree3 %d %d\n", i, n);
    for (j = 0; j < n; j++) {
      t = (long) (drand48() * NumTasks);
      Tasks[t].Add(i);
    }
  }
}

void AssignEdges(const TIntV& Pairs, TIntIntVV& Tasks, int tsize) {
  int i;
  int NumStubs;
  int NumTasks;
  int TaskId;
  int Node1;
  int Node2;

  printf("AssignEdges\n");
  printf("Pairs Len %d\n",Pairs.Len());
  printf("Tasks Len %d\n",Tasks.Len());

  NumStubs = Pairs.Len();
  NumTasks = Tasks.Len();

  // distribute edges to tasks
  for (i = 0; i < NumStubs-1; i += 2) {

    Node1 = Pairs.GetVal(i).Val;
    Node2 = Pairs.GetVal(i+1).Val;

    // add an edge twice, once for each end node
    TaskId = Node1 / tsize;
    Tasks[TaskId].Add(Node1);
    Tasks[TaskId].Add(Node2);

    TaskId = Node2 / tsize;
    Tasks[TaskId].Add(Node2);
    Tasks[TaskId].Add(Node1);
  }
}

void GetAdjLists(const TIntV& Edges, TIntIntVH& AdjLists) {
  int i;
  int NumStubs;
  int Node1;
  int Node2;

  printf("GetAdjLists\n");
  printf("Edges1 Len %d\n",Edges.Len());

  NumStubs = Edges.Len();

  // distribute node pairs to adjacency lists
  for (i = 0; i < NumStubs-1; i += 2) {
    Node1 = Edges.GetVal(i).Val;
    Node2 = Edges.GetVal(i+1).Val;

    AdjLists.AddKey(Node1);
    AdjLists(Node1).AddMerged(Node2);
  }
}

void GetNeighborhood(const TIntV& Nodes, const TIntIntVH& AdjLists, TIntV& Hood) {
  int i;
  int j;
  int Node;
  int NumNodes;
  int NumNeighbors;
  int Neighbor;
  TIntH HashHood;
  TIntV Neighbors;

  NumNodes = Nodes.Len();

  // create a union of all neighbors
  for (i = 0; i < NumNodes; i++) {
    Node = Nodes.GetVal(i).Val;
    Neighbors = AdjLists.GetDat(Node);
    NumNeighbors = Neighbors.Len();
    for (j = 0; j < NumNeighbors; j++) {
      Neighbor = Neighbors.GetVal(j).Val;
      HashHood.AddDat(Neighbor,0);
    }
  }

  // change a hash table to a vector
  HashHood.GetKeyV(Hood);
}
  

void Edge2Hash(const TIntV& Edges, TIntH& Hash) {
  int i;
  int Num;
  int Key;
  int Value;

  printf("Edges2 Len %d\n",Edges.Len());
  Num = Edges.Len();

  for (i = 0; i < Num-1; i += 2) {
    Key = Edges.GetVal(i).Val;
    Value = Edges.GetVal(i+1).Val;

    Hash.AddDat(Key, Value);
  }
}

}; // namespace TSnap

