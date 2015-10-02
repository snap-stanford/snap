#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
This test measures the performance of loading and performing join and select of the StackOverflow tags dataset.
To be compared with performance of the same set of operations by a table version that doesn't use a sting pool (ExplicitStringTableTest2).
*/

int main(int argc, char** argv){
  TBool debug = false;
  TStr TagsFnm = "/lfs/madmax4/0/yonathan/tags";
  if(debug){ TagsFnm = "/lfs/madmax4/0/yonathan/tags_small";}
  Schema TagS; 
  TagS.Add(TPair<TStr,TAttrType>("UserId", atInt));
  TagS.Add(TPair<TStr,TAttrType>("Tag", atStr));
  // Load a TTable object and benchmark how long it takes to iterate over all of its records
  TTableContext Context;
  TTable::SetMP(false);
	
  //float ft0, ft1, ft2, ft3;
  //float mu0, mu1, mu2, mu3;
  float ft_max;
  float mu_max;
  //float tdiff1, tdiff2, dfiff3;
  //float mdiff1, mdiff2, mdiff3;
    
  //getcpumem(&ft0,&mu0);
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
  //getcpumem(&ft1,&mu1);
  //tdiff1 = ft1 - ft0;
  //mdiff1 = mu1 - mu0;
  //printf("time: %0.3f seconds, memory: %0.3f MB\n", tdiff1, mdiff1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer2;
  gettimeofday(&timer2, NULL);
  t1 = timer2.tv_sec + (timer2.tv_usec/1000000.0);
  printf("start join on user\n");
  PTable TagsJoinUser = Tags->SelfJoin("UserId");
  printf("finish join on user\n");
  gettimeofday(&timer2, NULL);
  t2 = timer2.tv_sec + (timer2.tv_usec/1000000.0);
  printf("Time to join on user id column: %f\n", t2 - t1);
  printf("Table Size:\n");
  TagsJoinUser->PrintSize();
  //getcpumem(&ft2,&mu2);
  if(debug){ TagsJoinUser->SaveSS(TagsFnm + "_join_user.tsv");}
  //tdiff2 = ft2 - ft1;
  //mdiff2 = mu2 - mu1;
  //printf("time: %0.3f seconds, memory: %0.3f MB\n", tdiff2, mdiff2);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer3;
  gettimeofday(&timer3, NULL);
  t1 = timer3.tv_sec + (timer3.tv_usec/1000000.0);
  PTable JavaTags = TTable::New(TagS, Context);
  TIntV SelectedRows;
  if(debug){
  	Tags->SelectAtomicConst(TStr("Tag"), TStr("c#"), EQ, SelectedRows, JavaTags, false, true);
  } else{
  	Tags->SelectAtomicConst(TStr("Tag"), TStr("java"), EQ, SelectedRows, JavaTags, false, true);
  }
  gettimeofday(&timer3, NULL);
  t2 = timer3.tv_sec + (timer3.tv_usec/1000000.0);
  printf("Time to select java users: %f\n", t2 - t1);
  printf("Table Size:\n");
  JavaTags->PrintSize();
  //getcpumem(&ft3,&mu3);
  if(debug){ JavaTags->SaveSS(TagsFnm + "_select.tsv");}
  //tdiff3 = ft3 - ft2;
  //mdiff3 = mu3 - mu2;
  //printf("time: %0.3f seconds, memory: %0.3f MB\n", tdiff3, mdiff3);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
  return 0;
}