#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
This test measures the performance of This test measures the performance of loading the StackOverflow tags dataset and joining
on tags column when using explicit string storage (i.e. no string pool). 
To be compared with performance of the same set of operations by TTable (ExplicitStringTableTest3).
*/

int main(int argc, char** argv){
  TBool debug = false;
  TStr TagsFnm = "/lfs/madmax4/0/yonathan/tags_200000";
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
	
  timeval timer5;
  gettimeofday(&timer5, NULL);
  t1 = timer5.tv_sec + (timer5.tv_usec/1000000.0);
  PExplicitStringTable ES_TagsJoinTag = ES_Tags->SelfJoin("Tag");
  gettimeofday(&timer5, NULL);
  t2 = timer5.tv_sec + (timer5.tv_usec/1000000.0);
  printf("Time to join on tags column: %f\n", t2 - t1);
  printf("Table Size:\n");
  ES_TagsJoinTag->PrintSize();
  if(debug){ ES_TagsJoinTag->SaveSS(TagsFnm + "_join_tag_es.tsv");}
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  return 0;
}