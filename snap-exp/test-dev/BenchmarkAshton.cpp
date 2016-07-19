//#include "Engine.h"
//#include "Table.h"
#include "Snap.h"
#include <time.h>

#define N 1
#define OPS1 6 // load cctor select join graphprep tograph

int main(){
  TStr TestFile("../../testfiles/posts_1.tsv");
  //TStr TestFile("/lfs/local/0/ringo/testfiles/posts_full.tsv");
  double q1Results[N][OPS1];
  for(int i = 0; i < N; i++){
    for(int j = 0; j < OPS1; j++){
      q1Results[i][j] = 0;
    }
  }

  // Schema - change types..
  Schema S;
  /*
  S.Add(TPair<TStr,TAttrType>("Body", atInt));
  S.Add(TPair<TStr,TAttrType>("ViewCount", atInt));
  S.Add(TPair<TStr,TAttrType>("LastActivityDate", atInt));
  S.Add(TPair<TStr,TAttrType>("Title", atInt));
  S.Add(TPair<TStr,TAttrType>("LastEditorUserId", atInt));
  S.Add(TPair<TStr,TAttrType>("LastEditorDisplayName", atInt));
  S.Add(TPair<TStr,TAttrType>("LastEditDate", atInt));
  S.Add(TPair<TStr,TAttrType>("CommentCount", atInt));
  S.Add(TPair<TStr,TAttrType>("AnswerCount", atInt));
  S.Add(TPair<TStr,TAttrType>("AcceptedAnswerId", atInt));
  S.Add(TPair<TStr,TAttrType>("Score", atInt));
  S.Add(TPair<TStr,TAttrType>("PostTypeId", atInt));
  S.Add(TPair<TStr,TAttrType>("OwnerUserId", atInt));
  S.Add(TPair<TStr,TAttrType>("Tags", atInt));
  S.Add(TPair<TStr,TAttrType>("CreationDate", atInt));
  S.Add(TPair<TStr,TAttrType>("FavoriteCount", atInt));
  S.Add(TPair<TStr,TAttrType>("Id", atInt));
  S.Add(TPair<TStr,TAttrType>("ParentId", atInt));
  S.Add(TPair<TStr,TAttrType>("CommunityOwnedDate", atInt));
  S.Add(TPair<TStr,TAttrType>("ClosedDate", atInt));
  */
  
  S.Add(TPair<TStr,TAttrType>("Body", atStr));
  S.Add(TPair<TStr,TAttrType>("ViewCount", atInt));
  S.Add(TPair<TStr,TAttrType>("LastActivityDate", atStr));
  S.Add(TPair<TStr,TAttrType>("Title", atStr));
  S.Add(TPair<TStr,TAttrType>("LastEditorUserId", atInt));
  S.Add(TPair<TStr,TAttrType>("LastEditorDisplayName", atStr));
  S.Add(TPair<TStr,TAttrType>("LastEditDate", atStr));
  S.Add(TPair<TStr,TAttrType>("CommentCount", atInt));
  S.Add(TPair<TStr,TAttrType>("AnswerCount", atInt));
  S.Add(TPair<TStr,TAttrType>("AcceptedAnswerId", atInt));
  S.Add(TPair<TStr,TAttrType>("Score", atInt));
  S.Add(TPair<TStr,TAttrType>("PostTypeId", atInt));
  S.Add(TPair<TStr,TAttrType>("OwnerUserId", atInt));
  S.Add(TPair<TStr,TAttrType>("Tags", atStr));
  S.Add(TPair<TStr,TAttrType>("CreationDate", atStr));
  S.Add(TPair<TStr,TAttrType>("FavoriteCount", atInt));
  S.Add(TPair<TStr,TAttrType>("Id", atInt));
  S.Add(TPair<TStr,TAttrType>("ParentId", atInt));
  S.Add(TPair<TStr,TAttrType>("CommunityOwnedDate", atStr));
  S.Add(TPair<TStr,TAttrType>("ClosedDate", atStr));
  
  //Relevant Columns
  TIntV RelevantCols;
  RelevantCols.Add(9);
  RelevantCols.Add(11);
  RelevantCols.Add(12);
  RelevantCols.Add(13);
  RelevantCols.Add(14);
  RelevantCols.Add(16);

  TTableContext Context;

  for(int i = 0; i < N; i++){
    TExeTm tl;
    PTable T1 = TTable::LoadSS("posts1", S, TestFile, Context, RelevantCols, '\t', true);
    q1Results[i][0] = tl.GetSecs();
    TExeTm tcc;
    PTable T2 = TTable::New(T1, "posts2");
    q1Results[i][1] = tcc.GetSecs();
    TExeTm ts;
    T1->SelectAtomicIntConst("PostTypeId", 1, EQ);
    q1Results[i][2] = ts.GetSecs();
    TExeTm tj;
    PTable Tj = T1->Join("AcceptedAnswerId", *T2, "Id");
    q1Results[i][3] = tj.GetSecs();
    TExeTm tpg;
    Tj->AddEdgeAttr("posts2.CreationDate");
    q1Results[i][4] = tpg.GetSecs();
    TExeTm ttg;
    TSnap::ToNetwork<PNEANet>(Tj, "posts1.OwnerUserId", "posts2.OwnerUserId", aaLast);
    q1Results[i][5] = tpg.GetSecs();
  }

  printf("Load: ");
  double load_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][0]);
    load_avg += q1Results[i][0];
  }
  load_avg = load_avg/N;
  printf("\naverage load time: %f seconds\n", load_avg);

  printf("Copy Constructor: ");
  double cctor_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][1]);
    cctor_avg += q1Results[i][1];
  }
  cctor_avg = cctor_avg/N;
  printf("\naverage copy constructor time: %f seconds\n", cctor_avg);

  printf("Select: ");
  double select_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][2]);
    select_avg += q1Results[i][2];
  }
  select_avg = select_avg/N;
  printf("\naverage select time: %f seconds\n", select_avg);

  printf("Join: ");
  double join_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][3]);
    join_avg += q1Results[i][3];
  }
  join_avg = join_avg/N;
  printf("\naverage join time: %f seconds\n", join_avg);

  printf("Graph Prep: ");
  double gp_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][4]);
    gp_avg += q1Results[i][4];
  }
  gp_avg = gp_avg/N;
  printf("\naverage graph prep time: %f seconds\n", gp_avg);

  printf("Graph Creation: ");
  double graph_avg = 0;
  for(int i = 0; i < N; i++){
    printf("%f ", q1Results[i][5]);
    graph_avg += q1Results[i][5];
  }
  graph_avg = graph_avg/N;
  printf("\naverage graph time: %f seconds\n", graph_avg);
  return 0;
}
  
