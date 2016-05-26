#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
This test measures the performance of loading the StackOverflow tags dataset and joining on tags column.
To be compared with performance of the same set of operations by a table version that doesn't 
use a sting pool (ExplicitStringTableTest4).
*/

int main(int argc, char** argv){
  TBool debug = false;
  TStr TagsFnm = "/lfs/madmax4/0/yonathan/tags_200000";
  if(debug){ TagsFnm = "/lfs/madmax4/0/yonathan/tags_small";}
  Schema TagS; 
  TagS.Add(TPair<TStr,TAttrType>("UserId", atInt));
  TTableContext Context;
  TTable::SetMP(false);
	
  float ft_max;
  float mu_max;
	
  timeval timer0;
  gettimeofday(&timer0, NULL);
  double t1 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  PTable Tags = TTable::LoadSS(TagS, TagsFnm + ".tsv", Context);
  gettimeofday(&timer0, NULL);
  double t2 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  printf("Time to load tags table: %f\n", t2 - t1);
  printf("Table Size:\n");
  Tags->PrintSize();
  Tags->PrintContextSize();
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer1;
  gettimeofday(&timer1, NULL);
  t1 = timer1.tv_sec + (timer1.tv_usec/1000000.0);
  PTable TagsJoinTag = Tags->SelfJoin("Tag");
  gettimeofday(&timer1, NULL);
  t2 = timer1.tv_sec + (timer1.tv_usec/1000000.0);
  printf("Time to join on tags column: %f\n", t2 - t1);
  printf("Table Size:\n");
  TagsJoinTag->PrintSize();
  if(debug){ TagsJoinTag->SaveSS(TagsFnm + "_join_tag.tsv");}
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  return 0;
}