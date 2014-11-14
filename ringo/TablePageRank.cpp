#include "Snap.h"
#include <cstring>
#include "BenchmarkUtilities.h"

/*
This test benchmarks the different steps in an implementation of PageRank using TTable operations.
*/

int main(int argc, char** argv){
  int Iters = 10;
  TBool debug = false;
  //TStr infile = "/lfs/madmax2/0/yonathan/twitter_rv.txt";
  TStr infile = "/lfs/madmax2/0/yonathan/soc-LiveJournal1-noheader.txt";
  if(debug){ infile = "/lfs/madmax2/0/yonathan/edges.tsv";}
  Schema S;
  S.Add(TPair<TStr,TAttrType>("src", atInt));
  S.Add(TPair<TStr,TAttrType>("dst", atInt));
  TTableContext Context;
  TTable::SetMP(false);
	
  float ft_max, mu_max;
	
  timeval timer0;
  gettimeofday(&timer0, NULL);
  double t1 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  PTable Edges = TTable::LoadSS(S, infile, Context);
  gettimeofday(&timer0, NULL);
  double t2 = timer0.tv_sec + (timer0.tv_usec/1000000.0);
  printf("Time to load table: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  /// generate nodes table ///
  // Projections
  timeval timer1;
  gettimeofday(&timer1, NULL);
  t1 = timer1.tv_sec + (timer1.tv_usec/1000000.0);
	
  TStrV SrcProj;
  SrcProj.Add("src");
  PTable SrcT = Edges->Project(SrcProj);
  TStrV DstProj;
  DstProj.Add("dst");
  PTable DstT = Edges->Project(DstProj);
	
  gettimeofday(&timer1, NULL);
  t2 = timer1.tv_sec + (timer1.tv_usec/1000000.0);
  printf("Time to project node columns: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  // Unique Src Table
  timeval timer2;
  gettimeofday(&timer2, NULL);
  t1 = timer2.tv_sec + (timer2.tv_usec/1000000.0);
	
  SrcT->Unique("src");
  SrcT->Rename("src","NId");
  if(debug){SrcT->SaveSS("SrcNodes.tsv");}
	
  gettimeofday(&timer2, NULL);
  t2 = timer2.tv_sec + (timer2.tv_usec/1000000.0);
  printf("Time for src unique: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  // Unique Dst Table
  timeval timer3;
  gettimeofday(&timer3, NULL);
  t1 = timer3.tv_sec + (timer3.tv_usec/1000000.0);
	
  DstT->Unique("dst");
  DstT->Rename("dst","NId");
  if(debug){DstT->SaveSS("DstNodes.tsv");}
	
  gettimeofday(&timer3, NULL);
  t2 = timer3.tv_sec + (timer3.tv_usec/1000000.0);
  printf("Time for dst unique: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  // Union Src & Dst
  timeval timer4;
  gettimeofday(&timer4, NULL);
  t1 = timer4.tv_sec + (timer4.tv_usec/1000000.0);
	
  PTable Nodes = SrcT->Union(DstT);
  if(debug){Nodes->SaveSS("UnionNodes.tsv");}
	
  gettimeofday(&timer4, NULL);
  t2 = timer4.tv_sec + (timer4.tv_usec/1000000.0);
  printf("Time to union src & dst: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  // initialize node scores in Nodes table
  timeval timer5;
  gettimeofday(&timer5, NULL);
  t1 = timer5.tv_sec + (timer5.tv_usec/1000000.0);

  TInt NumNodes = Nodes->GetNumValidRows();
  printf("Num of nodes: %d\n", NumNodes.Val);
  TFltV Scores(Nodes->GetNumRows());
  for(int i = 0; i < Nodes->GetNumRows(); i++){
  	Scores[i] = 1.0/NumNodes;	
  }
  Nodes->StoreFltCol("score",Scores);
  if(debug){Nodes->SaveSS("InitNodes.tsv");}
	
  gettimeofday(&timer5, NULL);
  t2 = timer5.tv_sec + (timer5.tv_usec/1000000.0);
  printf("Time to initialize scores for node table: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  TTable::SetMP(true);
		
  //find out degrees of all nodes
  timeval timer6;
  gettimeofday(&timer6, NULL);
  t1 = timer6.tv_sec + (timer6.tv_usec/1000000.0);

  Edges->Count("src","out_degree");

  gettimeofday(&timer6, NULL);
  t2 = timer6.tv_sec + (timer6.tv_usec/1000000.0);
  printf("Time to compute out degrees: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  // initialize node scores in Edges table
  timeval timer7;
  gettimeofday(&timer7, NULL);
  t1 = timer7.tv_sec + (timer7.tv_usec/1000000.0);
	
  TFltV ScoresE(Edges->GetNumRows());
  for(int i = 0; i < ScoresE.Len(); i++){
  	ScoresE[i] = 1.0/NumNodes;	
  }
  Edges->StoreFltCol("score",ScoresE);
  if(debug){Edges->SaveSS("InitEdges.tsv");}
	
  gettimeofday(&timer7, NULL);
  t2 = timer7.tv_sec + (timer7.tv_usec/1000000.0);
  printf("Time to initialize node scores in Edges table: %f\n", t2 - t1);
  getmaxcpumem(&ft_max, &mu_max);
  printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
  printf("\n");
	
  for(int i = 0; i < Iters; i++){
		char buf[5];
		sprintf(buf,"%d",i);
		buf[4] = 0;
		TStr suffix = TStr(buf);
		printf("start iteration %d\n", i);
		if(debug){Edges->SaveSS(TStr("StartEdges") + suffix + TStr(".tsv"));}
		
		// column arithmetics for finding score contributions
		timeval timer8;
		gettimeofday(&timer8, NULL);
		t1 = timer8.tv_sec + (timer8.tv_usec/1000000.0);
		
		Edges->ColDiv("score","out_degree","contribution");
		Edges->ColMul("contribution",0.85);	
		if(debug){Edges->SaveSS(TStr("contributions") + suffix + TStr(".tsv"));}
		
		gettimeofday(&timer8, NULL);
		t2 = timer8.tv_sec + (timer8.tv_usec/1000000.0);
		printf("Time to compute contributions (column arithmetics): %f\n", t2 - t1);
		getmaxcpumem(&ft_max, &mu_max);
		printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
		printf("\n");
	
		// Aggregate contributions by dst
		timeval timer9;
		gettimeofday(&timer9, NULL);
		t1 = timer9.tv_sec + (timer9.tv_usec/1000000.0);

		TStrV GroupBy;
		GroupBy.Add("dst");
		Edges->Aggregate(GroupBy, aaSum, "contribution", "new_score");
		if(debug){Edges->SaveSS(TStr("aggregation") + suffix + TStr(".tsv"));}

		gettimeofday(&timer9, NULL);
		t2 = timer9.tv_sec + (timer9.tv_usec/1000000.0);
		printf("Time to aggregate contributions by dst: %f\n", t2 - t1);
		getmaxcpumem(&ft_max, &mu_max);
		printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
		printf("\n");
		
		// Update scores in nodes table
		timeval timer10;
		gettimeofday(&timer10, NULL);
		t1 = timer10.tv_sec + (timer10.tv_usec/1000000.0);
		
		Nodes->UpdateFltFromTable("NId","score", *Edges, "dst", "new_score", 0.0);
		if(debug){Nodes->SaveSS(TStr("updated_nodes_leak") + suffix + TStr(".tsv"));}
		
		gettimeofday(&timer10, NULL);
		t2 = timer10.tv_sec + (timer10.tv_usec/1000000.0);
		printf("Time to update score in nodes table: %f\n", t2 - t1);
		getmaxcpumem(&ft_max, &mu_max);
		printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
		printf("\n");
		
		// Put back leaked pagerank
		timeval timer11;
		gettimeofday(&timer11, NULL);
		t1 = timer11.tv_sec + (timer11.tv_usec/1000000.0);
		
		TFlt S = 0.0;
		for(TRowIterator RI = Nodes->BegRI(); RI < Nodes->EndRI(); RI++){
			S += RI.GetFltAttr("score");
		}	
		Nodes->ColAdd("score", (1-S)/NumNodes);
		if(debug){Nodes->SaveSS(TStr("updated_nodes") + suffix + TStr(".tsv"));}
		
		gettimeofday(&timer11, NULL);
		t2 = timer11.tv_sec + (timer11.tv_usec/1000000.0);
		printf("Time to put back leaked pagerank: %f\n", t2 - t1);
		getmaxcpumem(&ft_max, &mu_max);
		printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
		printf("\n");
		
		if(i < Iters-1){
			// join edges with node
			timeval timer12;
			gettimeofday(&timer12, NULL);
			t1 = timer12.tv_sec + (timer12.tv_usec/1000000.0);
			
			PTable T = Edges->Join("src", Nodes, "NId");
			if(debug && !TTable::GetMP()){T->SaveSS("NewEdgesFull_seq" + suffix + TStr(".tsv"));}
			if(debug && TTable::GetMP()){T->SaveSS("NewEdgesFull_par" + suffix + TStr(".tsv"));}
			
			gettimeofday(&timer12, NULL);
			t2 = timer12.tv_sec + (timer12.tv_usec/1000000.0);
			printf("Time to join edges with nodes: %f\n", t2 - t1);
			getmaxcpumem(&ft_max, &mu_max);
			printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
			printf("\n");
		
			Edges = T;
			
			// project in-place relevant column
			timeval timer13;
			gettimeofday(&timer13, NULL);
			t1 = timer13.tv_sec + (timer13.tv_usec/1000000.0);
			
			TStrV ProjectCols;
			ProjectCols.Add("src");
			ProjectCols.Add("dst");
			ProjectCols.Add("_id-1");
			ProjectCols.Add("out_degree");
			ProjectCols.Add("score-2");
			Edges->ProjectInPlace(ProjectCols);
			Edges->Rename("score-2", "score");
			Edges->Rename("_id-1", "_id");
			if(debug && !TTable::GetMP()){Edges->SaveSS("NewEdges_seq" + suffix + TStr(".tsv"));}
			if(debug && TTable::GetMP()){Edges->SaveSS("NewEdges_par" + suffix + TStr(".tsv"));}
			
			gettimeofday(&timer13, NULL);
			t2 = timer13.tv_sec + (timer13.tv_usec/1000000.0);
			printf("Time project in-place relevant columns: %f\n", t2 - t1);
			getmaxcpumem(&ft_max, &mu_max);
			printf("time: %0.3f seconds, memory: %0.3f MB\n", ft_max, mu_max);
			printf("\n");
		}
  }
  return 0;
}