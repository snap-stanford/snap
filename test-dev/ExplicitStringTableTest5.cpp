#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
This test measures the performance of loading and performing join on user id column on text-rich StackOverflow posts dataset.
To be compared with performance of the same set of operations by a table version that doesn't use a sting pool (ExplicitStringTableTest6).
*/

int main(int argc, char** argv){
  TBool debug = false;
  TStr PostsFnm = "/lfs/madmax4/0/yonathan/posts_full";
  Schema S; 
  S.Add(TPair<TStr,TAttrType>("Id-1", atInt));
  S.Add(TPair<TStr,TAttrType>("PostTypeId-1", atInt));
  S.Add(TPair<TStr,TAttrType>("AcceptedAnswerId-1", atInt));
  S.Add(TPair<TStr,TAttrType>("OwnerUserId-1", atInt));
  S.Add(TPair<TStr,TAttrType>("Body-1", atStr));
  S.Add(TPair<TStr,TAttrType>("Tag-1", atStr));
  // Load a TTable object and benchmark how long it takes to iterate over all of its records
  TTableContext Context;
  TTable::SetMP(false);
	
  float ft_max;
  float mu_max;
	
  timeval timer0;
  gettimeofday(&timer0, NULL);
  double t1 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  printf("starting load\n");
  PTable Posts = TTable::LoadSS(S, PostsFnm + ".tsv", Context, '\t', true);
  gettimeofday(&timer0, NULL);
  double t2 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  printf("Time to load posts table: %f\n", t2 - t1);
  printf("Table Size:\n");
  Posts->PrintSize();
  Posts->PrintContextSize();
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer2;
  gettimeofday(&timer2, NULL);
  t1 = timer2.tv_sec + (timer2.tv_usec/1000000.0);
  printf("start join on user\n");
  PTable PostsJoinUser = Posts->Join("Id-1", Posts, "OwnerUserId-1");
  printf("finish join on user\n");
  gettimeofday(&timer2, NULL);
  t2 = timer2.tv_sec + (timer2.tv_usec/1000000.0);
  printf("Time to join on user id column: %f\n", t2 - t1);
  printf("Table Size:\n");
  PostsJoinUser->PrintSize();
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  return 0;
}