#include "Snap.h"
#include <cstring>
#include <cstdlib>
#include <sys/time.h>

int main(int argc, char** argv){
  TTableContext Context;

  if (argc < 3){
    printf("Usage: %s <nodes> <degree>\n", argv[0]);
  }

  int NumNodes = atoi(argv[1]);
  int Degree = atoi(argv[2]);

  uint64 NumEdges = NumNodes;
  NumEdges *= (Degree/2);
  
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

  PUNGraph Graph = TUNGraph::New(NumNodes, 100);

  TVec<TIntV> NbrVV(NumNodes);

  int NumThreads = 2;
  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(static)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    NbrVV[m].Reserve(Degree, Degree);
    //double endTr = omp_get_wtime();
    //printf("Thread=%d, i=%d, t=%f\n", omp_get_thread_num(), m, endTr-startTr);
  }
  
  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Alloc time: %f\n", t2-t1);

  NumThreads = omp_get_max_threads();
  omp_set_num_threads(omp_get_max_threads());
  #pragma omp parallel for schedule(dynamic,1000)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    if (m+Degree < NumNodes) {
      for (int i = 0; i < Degree; i++) {
        NbrVV[m][i] = (m+i+1);
      }
    } else {
      int Overflow = m+Degree-NumNodes+1;
      for (int i = 0; i < Overflow; i++) {
        NbrVV[m][i] = i;
      }
      for (int i = 0; i < Degree-Overflow; i++) {
        NbrVV[m][i+Overflow] = (m+i+1);
      }
    }
    //double endTr = omp_get_wtime();
    //printf("Thread=%d, i=%d, t=%f\n", omp_get_thread_num(), m, endTr-startTr);
  }

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Populate time: %f\n", t3-t2);

  NumThreads = 1;
  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(dynamic)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    Graph->AddNodeWithEdges(m, NbrVV[m]);
    //double endTr = omp_get_wtime();
    //printf("Thread=%d, i=%d, t=%f\n", omp_get_thread_num(), m, endTr-startTr);
  }
  Graph->SetNodesEdges(NumNodes, NumEdges);

  gettimeofday(&tim, NULL);
  double t4=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Add time: %f\n", t4-t3);

  TUNGraph::TNodeI NI = Graph->BegNI();
  for (int i = 0; i < 10; i++) {
    int Id = NI.GetId();
    int Deg = NI.GetDeg();
    printf("%d %d: ", Id, Deg);
    for (int j = 0; j < Deg && j < 10; j++) {
      printf("\t%d", NI.GetNbrNId(j));
    }
    printf("\n");
    NI++;
  }

  printf("Nodes      = %d\n",Graph->GetNodes());
  unsigned long long ss = Graph->GetEdges64();
  printf("Edges      = %llu\n", ss);

  //printf("IsOk       = %d\n",Graph->IsOk());
  printf("IsEmpty    = %d\n",Graph->Empty());
  printf("IsDirected = %d\n",Graph->HasFlag(gfDirected));

  return 0;
}
