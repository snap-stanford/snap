#include "Engine.h"
//#include "Table.h"
#include <time.h>

#define N 1
#define QA_FILES 7
#define COMMENT_FILES 7
#define QA_OPS 5
#define COMMENT_OPS 4

int main(){
  //TStr dir("/lfs/local/0/ringo/testfiles/");
  TStr outDir("out/");
  TStr dir("../../testfiles/");
  char* qa_file_names[QA_FILES] = {"posts_10.hashed.tsv"};//, "posts_30.hashed.tsv", "posts_100.hashed.tsv", "posts_300.hashed.tsv", "posts_1000.hashed.tsv", "posts_3000.hashed.tsv", "posts_10000.hashed.tsv"};
  TStrV qa_files(QA_FILES);
  for(int i = 0; i < QA_FILES; i++){
    qa_files[i] = dir + TStr(qa_file_names[i]);
  }
  double qa_results[QA_FILES][N][QA_OPS];
  for(int i = 0; i < QA_FILES; i++){
    for(int j = 0; j < N; j++){
      for(int k = 0; k < QA_OPS; k++){
        qa_results[i][j][k] = 0;
      }
    }
  }
  // build schema for Q&A
  TTable::Schema qa_S;
  qa_S.Add(TPair<TStr,TTable::TYPE>("Body", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("ViewCount", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("LastActivityDate", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("Title", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("LastEditorUserId", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("LastEditorDisplayName", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("LastEditDate", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("CommentCount", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("AnswerCount", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("AcceptedAnswerId", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("Score", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("PostTypeId", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("OwnerUserId", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("Tags", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("CreationDate", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("FavoriteCount", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("Id", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("ParentId", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("CommunityOwnedDate", TTable::INT));
  qa_S.Add(TPair<TStr,TTable::TYPE>("ClosedDate", TTable::INT));
  // relevant columns
  TIntV qa_RelevantCols;
  qa_RelevantCols.Add(11);
  qa_RelevantCols.Add(12);
  qa_RelevantCols.Add(16);
  qa_RelevantCols.Add(17);

  // build predicate "PostTypeId == 2"
  TPredicate::TAtomicPredicate ptiAtom(TPredicate::INT, true, TPredicate::EQ, "PostTypeId", "", 2, 0, "");
  TPredicate::TPredicateNode ptiNode(ptiAtom);
  TPredicate ptiPred(&ptiNode);

  // build GroupBy vector
  TStrV qaGroupBy;
  qaGroupBy.Add("Posts1.OwnerUserId");
  qaGroupBy.Add("Posts2.OwnerUserId");

  for(int f = 0; f < QA_FILES; f++){
    for(int i = 0; i < N; i++){
      TExeTm tl;
      PTable T1 = TTable::LoadSS("Posts1", qa_S, qa_files[f], qa_RelevantCols);
      qa_results[f][i][0] = tl.GetSecs();
      PTable T2 = TTable::LoadSS("Posts2", qa_S, qa_files[f], qa_RelevantCols);
      TExeTm ts;
      //T1->Select(ptiPred);
      T1->SelectAtomicIntConst("PostTypeId", 2, TPredicate::EQ);
      qa_results[f][i][1] = ts.GetSecs();
      TExeTm tj;
      PTable Tj = T1->Join("ParentId", *T2, "Id");
      qa_results[f][i][2] = tj.GetSecs();
      TExeTm te;
      Tj->Unique(qaGroupBy);
      qa_results[f][i][3] = te.GetSecs();
      Tj->SetSrcCol("Posts1.OwnerUserId");
      Tj->SetDstCol("Posts2.OwnerUserId");
      TExeTm tg;
      PNEANet G = Tj->ToGraph();
      qa_results[f][i][4] = tg.GetSecs();
      //TStr OutFNm = outDir + "out_" + qa_file_names[f];
      //if(i == N-1){Tj->SaveSS(OutFNm);}
      if(i == 0){printf("Number of rows in Q&A table: initial - %d, final %d\n", T2->GetNumValidRows().Val, Tj->GetNumValidRows().Val);}
    }

    printf("\nDone with %s. Stats:\n", qa_file_names[f]);
    printf("Load: ");
    double load_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][0]);
      load_avg += qa_results[f][i][0];
    }
    load_avg = load_avg/N;
    printf("\naverage load time: %f seconds\n", load_avg);

    printf("Select: ");
    double select_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][1]);
      select_avg += qa_results[f][i][1];
    }
    select_avg = select_avg/N;
    printf("\naverage select time: %f seconds\n", select_avg);

    printf("Join: ");
    double join_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][2]);
      join_avg += qa_results[f][i][2];
    }
    join_avg = join_avg/N;
    printf("\naverage join time: %f seconds\n", join_avg);

    printf("Unique: ");
    double unique_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][3]);
      unique_avg += qa_results[f][i][3];
    }
    unique_avg = unique_avg/N;
    printf("\naverage unique time: %f seconds\n", unique_avg);

    printf("Graph Creation: ");
    double graph_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][4]);
      graph_avg += qa_results[f][i][4];
    }
    graph_avg = graph_avg/N;
    printf("\naverage graph time: %f seconds\n", graph_avg);
  }
  
 char* comment_file_names[COMMENT_FILES] = {"comments_10.hashed.tsv", "comments_30.hashed.tsv", "comments_100.hashed.tsv", "comments_300.hashed.tsv", "comments_1000.hashed.tsv", "comments_3000.hashed.tsv", "comments_10000.hashed.tsv"};
  TStrV comment_files(COMMENT_FILES);
  for(int i = 0; i < COMMENT_FILES; i++){
    comment_files[i] = dir + TStr(comment_file_names[i]);
  }
  double comment_results[COMMENT_FILES][N][5];
  for(int i = 0; i < COMMENT_FILES; i++){
    for(int j = 0; j < N; j++){
      for(int k = 0; k < COMMENT_OPS; k++){
        comment_results[i][j][k] = 0;
      }
    }
  }

   // build schema for COMMENTS
  TTable::Schema comment_S;
  comment_S.Add(TPair<TStr,TTable::TYPE>("CreationDate", TTable::INT));
  comment_S.Add(TPair<TStr,TTable::TYPE>("Text", TTable::INT));
  comment_S.Add(TPair<TStr,TTable::TYPE>("UserId", TTable::INT));
  comment_S.Add(TPair<TStr,TTable::TYPE>("Score", TTable::INT));
  comment_S.Add(TPair<TStr,TTable::TYPE>("PostId", TTable::INT));
  comment_S.Add(TPair<TStr,TTable::TYPE>("Id", TTable::INT));
  // relevant columns
  TIntV comment_RelevantCols;
  comment_RelevantCols.Add(2);
  comment_RelevantCols.Add(4);
  // build predicate "UserId1 != UserId2"
  /*
  TPredicate::TAtomicPredicate uidAtom(TPredicate::INT, false, TPredicate::EQ, "UserId1", "UserId2");
  TPredicate::TPredicateNode uidEqNode(uidAtom);
  TPredicate::TPredicateNode uidNeqNode(TPredicate::NOT);
  uidNeqNode.AddLeftChild(&uidEqNode);
  */
  TPredicate::TAtomicPredicate uidAtom(TPredicate::INT, false, TPredicate::NEQ, "UserId1", "UserId2");
  TPredicate::TPredicateNode uidNeqNode(uidAtom);
  TPredicate uidPred(&uidNeqNode);
   // build GroupBy vector
  TStrV commentGroupBy;
  commentGroupBy.Add("UserId1");
  commentGroupBy.Add("UserId2");

  for(int f = 0; f < COMMENT_FILES; f++){
    for(int i = 0; i < N; i++){
      TExeTm tl;
      PTable T1 = TTable::LoadSS("Comments1", comment_S, comment_files[f], comment_RelevantCols);
      comment_results[f][i][0] = tl.GetSecs();
      PTable T2 = TTable::LoadSS("Comments2", comment_S, comment_files[f], comment_RelevantCols);
      TExeTm tj;
      PTable Tj = T1->Join("PostId", *T2, "PostId");
      comment_results[f][i][1] = tj.GetSecs();
      //debug
      //printf("Joint table size: %d\n", Tj->GetNumValidRows().Val);
      Tj->AddLabel("Comments1.UserId", "UserId1");
      Tj->AddLabel("Comments2.UserId", "UserId2");
      TExeTm ts;
      //Tj->Select(uidPred);
      Tj->SelectAtomic("UserId1", "UserId2", TPredicate::NEQ);
      comment_results[f][i][2] = ts.GetSecs();
      //debug
      //printf("Selected table size: %d\n", Tj->GetNumValidRows().Val);
      TExeTm te;
      Tj->Unique(commentGroupBy);
      comment_results[f][i][3] = te.GetSecs();
      TStr OutFNm = outDir + "out_" + comment_file_names[f];
      //if(i == N-1){Tj->SaveSS(OutFNm);}
      if(i == 0){printf("Number of rows in common comments table: initial - %d, final %d\n", T2->GetNumValidRows().Val, Tj->GetNumValidRows().Val);}
   }
    printf("\nDone with %s. Stats:\n", comment_file_names[f]);
    printf("Load: ");
    double load_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][0]);
      load_avg += comment_results[f][i][0];
    }
    load_avg = load_avg/N;
    printf("\naverage load time: %f seconds\n", load_avg);

    printf("Join: ");
    double join_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][1]);
      join_avg += comment_results[f][i][1];
    }
    join_avg = join_avg/N;
    printf("\naverage join time: %f seconds\n", join_avg);

    printf("Select: ");
    double select_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][2]);
      select_avg += comment_results[f][i][2];
    }
    select_avg = select_avg/N;
    printf("\naverage select time: %f seconds\n", select_avg);

     printf("Unique: ");
    double unique_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][3]);
      unique_avg += comment_results[f][i][3];
    }
    unique_avg = unique_avg/N;
    printf("\naverage unique time: %f seconds\n", unique_avg);
  }
   
  return 0;
}