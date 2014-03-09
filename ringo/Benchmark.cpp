#include "Engine.h"
//#include "Table.h"
#include <time.h>

#define N 10
#define QA_FILES 7
#define COMMENT_FILES 7
#define QA_OPS 6
#define COMMENT_OPS 5

int main(){
  TStr dir("/lfs/local/0/ringo/testfiles/");
  TStr outDir("out/");
  //TStr dir("../../testfiles/");
  char* qa_file_names[QA_FILES] = {"posts_10.hashed.tsv", "posts_30.hashed.tsv", "posts_100.hashed.tsv", "posts_300.hashed.tsv", "posts_1000.hashed.tsv", "posts_3000.hashed.tsv", "posts_10000.hashed.tsv"};
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
  TTableContext qa_Context;
  // build schema for Q&A
  Schema qa_S;
  qa_S.Add(TPair<TStr,TAttrType>("Body", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("ViewCount", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("LastActivityDate", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("Title", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("LastEditorUserId", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("LastEditorDisplayName", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("LastEditDate", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("CommentCount", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("AnswerCount", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("AcceptedAnswerId", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("Score", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("PostTypeId", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("OwnerUserId", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("Tags", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("CreationDate", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("FavoriteCount", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("Id", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("ParentId", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("CommunityOwnedDate", atInt));
  qa_S.Add(TPair<TStr,TAttrType>("ClosedDate", atInt));
  // relevant columns
  TIntV qa_RelevantCols;
  qa_RelevantCols.Add(11);
  qa_RelevantCols.Add(12);
  qa_RelevantCols.Add(16);
  qa_RelevantCols.Add(17);

  // build predicate "PostTypeId == 2"
  TAtomicPredicate ptiAtom(atInt, true, EQ, "PostTypeId", "", 2, 0, "");
  TPredicateNode ptiNode(ptiAtom);
  TPredicate ptiPred(&ptiNode);

  // build GroupBy vector - group by user pairs
  TStrV qaGroupBy;
  qaGroupBy.Add("Posts1.OwnerUserId");
  qaGroupBy.Add("Posts2.OwnerUserId");

  for(int f = 0; f < QA_FILES; f++){
    for(int i = 0; i < N; i++){
      TExeTm tl;
      PTable T1 = TTable::LoadSS("Posts1", qa_S, qa_files[f], qa_Context, qa_RelevantCols, '\t', true);
      qa_results[f][i][0] = tl.GetSecs();
      TExeTm tcc;
      PTable T2 = TTable::New(T1, "Posts2");
      qa_results[f][i][1] = tcc.GetSecs();
      TExeTm ts;
      //T1->Select(ptiPred);
      T1->SelectAtomicIntConst("PostTypeId", 2, EQ);
      qa_results[f][i][2] = ts.GetSecs();
      TExeTm tj;
      PTable Tj = T1->Join("ParentId", *T2, "Id");
      qa_results[f][i][3] = tj.GetSecs();
      TExeTm te;
      Tj->Unique(qaGroupBy);
      qa_results[f][i][4] = te.GetSecs();
      TExeTm tg;

      PNEANet G = TSnap::ToNetwork<PNEANet>(Tj, "Posts1.OwnerUserId", "Posts2.OwnerUserId", aaLast);
      qa_results[f][i][5] = tg.GetSecs();
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

    printf("Copy Constructor: ");
    double cctor_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][1]);
      cctor_avg += qa_results[f][i][1];
    }
    cctor_avg = cctor_avg/N;
    printf("\naverage copy constructor time: %f seconds\n", cctor_avg);

    printf("Select: ");
    double select_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][2]);
      select_avg += qa_results[f][i][2];
    }
    select_avg = select_avg/N;
    printf("\naverage select time: %f seconds\n", select_avg);

    printf("Join: ");
    double join_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][3]);
      join_avg += qa_results[f][i][3];
    }
    join_avg = join_avg/N;
    printf("\naverage join time: %f seconds\n", join_avg);

    printf("Unique: ");
    double unique_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][4]);
      unique_avg += qa_results[f][i][4];
    }
    unique_avg = unique_avg/N;
    printf("\naverage unique time: %f seconds\n", unique_avg);

    printf("Graph Creation: ");
    double graph_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", qa_results[f][i][5]);
      graph_avg += qa_results[f][i][5];
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
  TTableContext comment_Context;
  Schema comment_S;
  comment_S.Add(TPair<TStr,TAttrType>("CreationDate", atInt));
  comment_S.Add(TPair<TStr,TAttrType>("Text", atInt));
  comment_S.Add(TPair<TStr,TAttrType>("UserId", atInt));
  comment_S.Add(TPair<TStr,TAttrType>("Score", atInt));
  comment_S.Add(TPair<TStr,TAttrType>("PostId", atInt));
  comment_S.Add(TPair<TStr,TAttrType>("Id", atInt));
  // relevant columns
  TIntV comment_RelevantCols;
  comment_RelevantCols.Add(2);
  comment_RelevantCols.Add(4);
  // build predicate "UserId1 != UserId2"
  /*
  TAtomicPredicate uidAtom(atInt, false, TPredicate::EQ, "UserId1", "UserId2");
  TPredicateNode uidEqNode(uidAtom);
  TPredicateNode uidNeqNode(TPredicate::NOT);
  uidNeqNode.AddLeftChild(&uidEqNode);
  */
  TAtomicPredicate uidAtom(atInt, false, NEQ, "UserId1", "UserId2");
  TPredicateNode uidNeqNode(uidAtom);
  TPredicate uidPred(&uidNeqNode);
   // build GroupBy vector
  TStrV commentGroupBy;
  commentGroupBy.Add("UserId1");
  commentGroupBy.Add("UserId2");

  for(int f = 0; f < COMMENT_FILES; f++){
    for(int i = 0; i < N; i++){
      TExeTm tl;
      PTable T1 = TTable::LoadSS("Comments1", comment_S, comment_files[f], comment_Context, comment_RelevantCols, '\t', true);
      comment_results[f][i][0] = tl.GetSecs();
      TExeTm tcc;
      PTable T2 = TTable::New(T1, "Comments2");
      comment_results[f][i][1] = tcc.GetSecs();
      TExeTm tj;
      PTable Tj = T1->Join("PostId", *T2, "PostId");
      comment_results[f][i][2] = tj.GetSecs();
      //debug
      //printf("Joint table size: %d\n", Tj->GetNumValidRows().Val);
      Tj->AddLabel("Comments1.UserId", "UserId1");
      Tj->AddLabel("Comments2.UserId", "UserId2");
      TExeTm ts;
      //Tj->Select(uidPred);
      Tj->SelectAtomic("UserId1", "UserId2", NEQ);
      comment_results[f][i][3] = ts.GetSecs();
      //debug
      //printf("Selected table size: %d\n", Tj->GetNumValidRows().Val);
      TExeTm te;
      Tj->Unique(commentGroupBy);
      comment_results[f][i][4] = te.GetSecs();
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

    printf("Copy Constructor: ");
    double cctor_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][1]);
      cctor_avg += comment_results[f][i][1];
    }
    cctor_avg = cctor_avg/N;
    printf("\naverage copy constructor time: %f seconds\n", cctor_avg);

    printf("Join: ");
    double join_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][2]);
      join_avg += comment_results[f][i][2];
    }
    join_avg = join_avg/N;
    printf("\naverage join time: %f seconds\n", join_avg);

    printf("Select: ");
    double select_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][3]);
      select_avg += comment_results[f][i][3];
    }
    select_avg = select_avg/N;
    printf("\naverage select time: %f seconds\n", select_avg);

     printf("Unique: ");
    double unique_avg = 0;
    for(int i = 0; i < N; i++){
      printf("%f ", comment_results[f][i][4]);
      unique_avg += comment_results[f][i][4];
    }
    unique_avg = unique_avg/N;
    printf("\naverage unique time: %f seconds\n", unique_avg);
  }
   
  return 0;
}
