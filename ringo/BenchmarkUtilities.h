#include <sys/time.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include <proc/readproc.h>  // need to install procps-devel-3.2.8-25.el6.i686
#include <sys/time.h>
#include <sys/resource.h>

/*
void getcpumem(float *scpu, float *smem) {
  struct rusage rusage;
  struct proc_t usage;

  getrusage(RUSAGE_SELF, &rusage);
  *scpu =
    ((float) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
    ((float) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));

  look_up_our_self(&usage);
  *smem = (float) usage.vsize / 1000000;
}
*/

void getmaxcpumem(float *scpu, float *smem) {
  struct rusage rusage;

  getrusage(RUSAGE_SELF, &rusage);
  *scpu =
    ((float) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
    ((float) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
  *smem = (float) (rusage.ru_maxrss) / 1000;
}