#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
This test measures the performance of loading and performing join and select of the StackOverflow tags dataset when
using explicit string storage (i.e. no string pool). To be compared with performance of the same set of operations 
by TTable (ExplicitStringTableTest1).
*/


int main(int argc, char** argv){
  TBool debug = false;
  TStr TagsFnm = "/lfs/madmax4/0/yonathan/tags";
  if(debug){ TagsFnm = "/lfs/madmax4/0/yonathan/tags_small";}
  Schema TagS; 
  TagS.Add(TPair<TStr,TAttrType>("UserId", atInt));
  TagS.Add(TPair<TStr,TAttrType>("Tag", atStr));
	
  float ft_max;
  float mu_max;

  timeval timer4;
  gettimeofday(&timer4, NULL);
  double t1 = timer4.tv_sec + (timer4.tv_usec/1000000.0);
  PExplicitStringTable ES_Tags = ExplicitStringTable::LoadSS(TagS, TagsFnm + ".tsv");
  gettimeofday(&timer4, NULL);
  double t2 = timer4.tv_sec + (timer4.tv_usec/1000000.0);
  printf("Time to load tags table: %f\n", t2 - t1);
  printf("Table Size:\n");
  ES_Tags->PrintSize();
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer6;
  gettimeofday(&timer6, NULL);
  t1 = timer6.tv_sec + (timer6.tv_usec/1000000.0);
  PExplicitStringTable ES_TagsJoinUser = ES_Tags->SelfJoin("UserId");
  gettimeofday(&timer6, NULL);
  t2 = timer6.tv_sec + (timer6.tv_usec/1000000.0);
  printf("Time to join on user id column: %f\n", t2 - t1);
  printf("Table Size:\n");
  ES_TagsJoinUser->PrintSize();
  if(debug){ ES_TagsJoinUser->SaveSS(TagsFnm + "_join_user_es.tsv");}
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer7;
  gettimeofday(&timer7, NULL);
  t1 = timer7.tv_sec + (timer7.tv_usec/1000000.0);
  PExplicitStringTable ES_JavaTags = ExplicitStringTable::New(TagS);
  TIntV SelectedRows1;
  if(debug){
  	ES_Tags->SelectAtomicConst(TStr("Tag"), TStr("c#"), EQ, SelectedRows1, ES_JavaTags, false, true);
  } else{
  	ES_Tags->SelectAtomicConst(TStr("Tag"), TStr("java"), EQ, SelectedRows1, ES_JavaTags, false, true);
  }
  gettimeofday(&timer7, NULL);
  t2 = timer7.tv_sec + (timer7.tv_usec/1000000.0);
  printf("Time to select java users: %f\n", t2 - t1);
  printf("Table Size:\n");
  ES_JavaTags->PrintSize();
  if(debug){ ES_JavaTags->SaveSS(TagsFnm + "_select_es.tsv");}
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  return 0;
}