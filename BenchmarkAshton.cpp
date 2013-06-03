//#include "Engine.h"
#include "Table.h"
#include <time.h>

#define N 1
#define OPS1 5 // load select join graphprep tograph

int main(){
  //TStr TestFile("../../testfiles/posts_10.hashed.tsv");
  TStr TestFile("/lfs/local/0/ringo/testfiles/posts_full.tsv");
  double q1Results[N][OPS1];
  for(int i = 0; i < N; i++){
    for(int j = 0; j < OPS1; j++){
      q1Results[i][j] = 0;
    }
  }

  // Schema - change types..
  TTable::Schema S;
  /*
  S.Add(TPair<TStr,TTable::TYPE>("Body", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("ViewCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("LastActivityDate", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Title", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("LastEditorUserId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("LastEditorDisplayName", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("LastEditDate", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("CommentCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("AnswerCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("AcceptedAnswerId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Score", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("PostTypeId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("OwnerUserId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Tags", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("CreationDate", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("FavoriteCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Id", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("ParentId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("CommunityOwnedDate", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("ClosedDate", TTable::INT));
  */
  
  S.Add(TPair<TStr,TTable::TYPE>("Body", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("ViewCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("LastActivityDate", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("Title", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("LastEditorUserId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("LastEditorDisplayName", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("LastEditDate", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("CommentCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("AnswerCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("AcceptedAnswerId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Score", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("PostTypeId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("OwnerUserId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Tags", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("CreationDate", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("FavoriteCount", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("Id", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("ParentId", TTable::INT));
  S.Add(TPair<TStr,TTable::TYPE>("CommunityOwnedDate", TTable::STR));
  S.Add(TPair<TStr,TTable::TYPE>("ClosedDate", TTable::STR));
  
  //Relevant Columns
  TIntV RelevantCols;
  RelevantCols.Add(9);
  RelevantCols.Add(11);
  RelevantCols.Add(12);
  RelevantCols.Add(13);
  RelevantCols.Add(14);
  RelevantCols.Add(16);

  for(int i = 0; i < N; i++){
    TExeTm tl;
    PTable T1 = TTable::LoadSS("posts1", S, TestFile, RelevantCols);
    q1Results[i][0] = tl.GetSecs();
    PTable T2 = TTable::LoadSS("posts2", S, TestFile, RelevantCols);
    TExeTm ts;
    T1->SelectAtomicIntConst("PostTypeId", 1, TPredicate::EQ);
    q1Results[i][1] = ts.GetSecs();
    TExeTm tj;
    PTable Tj = T1->Join("AcceptedAnswerId", *T2, "Id");
    q1Results[i][2] = tj.GetSecs();
    TExeTm tpg;
    Tj->SetSrcCol("posts1.OwnerUserId");
    Tj->SetDstCol("posts2.OwnerUserId");
    Tj->AddEdgeAttr("posts2.CreationDate");
    Tj->GraphPrep();
    q1Results[i][3] = tpg.GetSecs();
    TExeTm ttg;
    Tj->ToGraph();
    q1Results[i][4] = tpg.GetSecs();
  }

  printf("Load: ");
  double load_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][0]);
    load_avg += q1Results[i][0];
  }
  load_avg = load_avg/N;
  printf("\naverage load time: %f seconds\n", load_avg);

  printf("Select: ");
  double select_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][1]);
    select_avg += q1Results[i][1];
  }
  select_avg = select_avg/N;
  printf("\naverage select time: %f seconds\n", select_avg);

  printf("Join: ");
  double join_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][2]);
    join_avg += q1Results[i][2];
  }
  join_avg = join_avg/N;
  printf("\naverage join time: %f seconds\n", join_avg);

  printf("Graph Prep: ");
  double gp_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][3]);
    gp_avg += q1Results[i][3];
  }
  gp_avg = gp_avg/N;
  printf("\naverage graph prep time: %f seconds\n", gp_avg);

  printf("Graph Creation: ");
  double graph_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][4]);
    graph_avg += q1Results[i][4];
  }
  graph_avg = graph_avg/N;
  printf("\naverage graph time: %f seconds\n", graph_avg);
  return 0;
}
  
