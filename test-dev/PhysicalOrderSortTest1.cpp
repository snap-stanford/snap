#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
 This test measures the benefit of logical ordering of records - compare to PhysicalOrderSortTest2
*/


int main(int argc, char** argv){
 TBool debug = false;
  TStr infile = "/lfs/madmax2/0/yonathan/twitter_rv.txt";
  //TStr infile = "/lfs/madmax2/0/yonathan/soc-LiveJournal1-noheader.txt";
  if(debug){ infile = "/lfs/madmax2/0/yonathan/edges.tsv";}
  Schema S;
  S.Add(TPair<TStr,TAttrType>("src", atInt));
  S.Add(TPair<TStr,TAttrType>("dst", atInt));
  TTableContext Context;
  TTable::SetMP(true);
	
  float ft_max, mu_max;

  timeval timer0;
  gettimeofday(&timer0, NULL);
  double t1 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  PTable G = TTable::LoadSS(S, infile, Context);
  gettimeofday(&timer0, NULL);
  double t2 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  printf("Time to load table: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  timeval timer1;
  gettimeofday(&timer1, NULL);
  t1 = timer1.tv_sec + (timer1.tv_usec/1000000.0);
  TStrV SortBy;
  SortBy.Add("dst");
  G->Order(SortBy);
  printf("done sorting\n");
  gettimeofday(&timer1, NULL);
  t2 = timer1.tv_sec + (timer1.tv_usec/1000000.0);
  printf("Time to sort table: %f\n", t2 - t1);
  if(debug){ G->SaveSS("PhysicalOrderSortTest1Out.tsv");}
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
	
  return 0;
}