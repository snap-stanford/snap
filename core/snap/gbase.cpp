/////////////////////////////////////////////////
// Graph Base
namespace TSnap {

TStr GetFlagStr(const TGraphFlag& GraphFlag) {
  static THash<TInt, TStr> GraphFlagToStrH;
  if (GraphFlagToStrH.Empty()) {
    GraphFlagToStrH.AddDat((int) gfUndef, "Undef");
    GraphFlagToStrH.AddDat((int) gfDirected, "Directed");
    GraphFlagToStrH.AddDat((int) gfMultiGraph, "MultiGraph");
    GraphFlagToStrH.AddDat((int) gfNodeDat, "NodeDat");
    GraphFlagToStrH.AddDat((int) gfEdgeDat, "EdgeDat");
    GraphFlagToStrH.AddDat((int) gfSources, "Sources");
  }
  return GraphFlagToStrH.GetDat((int) GraphFlag);
}

};  // namespace TSnap

/////////////////////////////////////////////////
// Union Find
int TUnionFind::Find(const int& Key) {
  int SetId = Key, parent = Parent(Key);
  // find set id
  while (parent != -1) {
    SetId = parent;
    parent = Parent(parent);
  }
  // flatten
  parent = Key;
  while (parent != -1) {
    const int tmp = Parent(parent);
    if (tmp != -1) { Parent(parent) = SetId; }
    parent = tmp;
  }
  return SetId;
}

void TUnionFind::Union(const int& Key1, const int& Key2) {
  const int root1 = Find(Key1);
  const int root2 = Find(Key2);
  TInt& rank1 = Rank(root1);
  TInt& rank2 = Rank(root2);
  if (rank1 > rank2) { Parent(root2) = root1; }
  else if (rank1 < rank2) { Parent(root1) = root2; }
  else if (root1 != root2) {
    Parent(root2) = root1;
    Rank(root1)++;
  }
}

void TUnionFind::Dump() {
  printf("  key\tset\n");
  for (int i = 0; i < KIdSetH.Len(); i++) {
    printf("  %d\t%d\n", int(KIdSetH.GetKey(i)), Find(KIdSetH.GetKey(i)));
  }
  printf("\n");
}


