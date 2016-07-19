#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>

int main(int argc, char** argv){

  if (argc < 2){
    printf("Usage: %s <memsize>\n", argv[0]);
  }

  char* memsize = argv[1];
  long long size = atoll(memsize);
  
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  char* ptr = new char[size];
  
  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Alloc time: %f\n", t2-t1);

  for (long long i = 0; i < size; i++) {
    ptr[i] = 'a';
  }

  gettimeofday(&tim, NULL);
  double t3=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Iter time: %f\n", t3-t2);

  return 0;
}
