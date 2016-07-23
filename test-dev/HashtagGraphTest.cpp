#include "Snap.h"
#include <cstring>

int main(int argc, char** argv){
  TTableContext Context;
  // create scheme
  Schema HashtagS;
  HashtagS.Add(TPair<TStr,TAttrType>("TweetId", atStr));
  HashtagS.Add(TPair<TStr,TAttrType>("Tag", atStr));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);

  char filename[100] = "/lfs/local/0/pararth/hashtags_dump.tsv";
  if (argc >= 2){
    strcpy(filename,argv[1]);
  }

  printf("hi1\n");
  PTable Q = TTable::LoadSS("Hashtag", HashtagS, filename, Context, RelevantCols);
  printf("hi2\n");
  PTable P = Q->SelfJoin("TweetId");
  printf("Unique\n");
  TStrV Cols;
  Cols.Add("Hashtag_1.Tag");
  Cols.Add("Hashtag_2.Tag");
  P->Unique(Cols, false);
  P->PrintSize();
  P->SaveSS("/lfs/local/0/pararth/hashtags_join.tsv");
  printf("hi3\n");

  P->SetSrcCol("Hashtag_1.Tag");
  P->SetDstCol("Hashtag_2.Tag");
  printf("hi4\n");

  PNEANet G = P->ToGraph(aaFirst);
  printf("graph size: %d %d %f\n", G->GetNodes(), G->GetEdges(), TSnap::GetMxWccSz(G));
  printf("diameter: %d\n", TSnap::GetBfsFullDiam(G, 10000));

  /*PTable NodeTable = TTable::GetNodeTable(N, "NodeTable", Context);
  PTable EdgeTable = TTable::GetEdgeTable(N, "EdgeTable", Context);

  NodeTable->SaveSS("tests/SO_node_table.txt");
  EdgeTable->SaveSS("tests/SO_edge_table.txt");*/
  return 0;
}
