#include "Snap.h"

void GetTimeStr(struct timeval *tval, char s[])
{
  int msec;
  char *s1;

  // printf("2. %d %d\n", tval->tv_sec, tval->tv_usec);
  s1 = strncpy(s, ctime(&tval->tv_sec)+11, 8);
  s1[8] = '\0';
  msec = (int) (tval->tv_usec / 1000);
  sprintf(s, "%s.%03d", s1, msec);
}

float GetCPUTime() {
  struct rusage rusage;
  float Result;

  getrusage(RUSAGE_SELF, &rusage);

  Result =
    ((float) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
    ((float) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
  return Result;
}

// random generation benchmark
void RandBench(const int& n) {
  float ft0, ft1;
  long Sum;
  int x;
  int i;

  // generate random numbers
  ft0 = GetCPUTime();
  Sum = 0;
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    Sum += x;
  }

  printf("rand:          sum %ld\n", Sum);

  ft1 = GetCPUTime();
  printf("rand: %7.3fs generating %d numbers\n",ft1-ft0,i);

}

// hash table benchmark with integer keys
void HashBench(const int& n) {
  TIntIntH TableInt;
  float ft0, ft1;
  int x;
  int i;
  int Found;
  int NotFound;
  int Id;

  // build the hash table
  ft0 = GetCPUTime();
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    TableInt.AddDat(x,0);
  }
  printf("hash:          size %d\n", TableInt.Len());

  ft1 = GetCPUTime();
  printf("hash: %7.3fs inserting  %d numbers\n",ft1-ft0,i);

  // search the hash table
  ft0 = GetCPUTime();
  Found = 0;
  NotFound = 0;
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    Id = TableInt.GetKeyId(x);
    if (Id < 0) {
      NotFound++;
    } else {
      Found++;
    }
  }
  printf("hash:          found %d, notfound %d\n", Found, NotFound);

  ft1 = GetCPUTime();
  printf("hash: %7.3fs searching %d numbers\n",ft1-ft0,i);
}

// vector benchmark with integer values
void VecBench(const int& n) {
  TIntV Vec;
  float ft0, ft1;
  int x;
  int i;
  int Found;
  int NotFound;
  int Id;

  // build the vector
  ft0 = GetCPUTime();
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    Vec.Add(x);
  }
  printf("vec :          size %d\n", Vec.Len());

  ft1 = GetCPUTime();
  printf("vec : %7.3fs inserting  %d numbers\n",ft1-ft0,i);

  // sort the vector
  ft0 = GetCPUTime();
  Vec.Sort();
  printf("vec :          size %d\n", Vec.Len());

  ft1 = GetCPUTime();
  printf("vec : %7.3fs sorting %d numbers\n",ft1-ft0,i);

  // search the vector
  ft0 = GetCPUTime();
  Found = 0;
  NotFound = 0;
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    Id = Vec.IsInBin(x);
    if (Id == 0) {
      NotFound++;
    } else {
      Found++;
    }
  }
  printf("vec :          found %d, notfound %d\n", Found, NotFound);

  ft1 = GetCPUTime();
  printf("vec : %7.3fs searching %d numbers\n",ft1-ft0,i);
}

// fixed vector benchmark with integer values
void FixedVecBench(const int& n) {
  TIntV Vec(100000000);
  float ft0, ft1;
  int x;
  int i;
  int Found;
  int NotFound;

  // set the elements to 0
  ft0 = GetCPUTime();
  Vec.PutAll(0);
  printf("fvec:          size %d\n", Vec.Len());

  ft1 = GetCPUTime();
  printf("fvec: %7.3fs zeroing    %d numbers\n",ft1-ft0,n);

  // build the vector
  ft0 = GetCPUTime();
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    Vec[x] = 1;
  }

  ft1 = GetCPUTime();
  printf("fvec: %7.3fs inserting  %d numbers\n",ft1-ft0,i);

  // search the vector
  ft0 = GetCPUTime();
  Found = 0;
  NotFound = 0;
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    if (Vec[x] <= 0) {
      NotFound++;
    } else {
      Found++;
    }
  }
  printf("fvec:          found %d, notfound %d\n", Found, NotFound);

  ft1 = GetCPUTime();
  printf("fvec: %7.3fs searching %d numbers\n",ft1-ft0,i);
}

// sorted vector benchmark with integer values
void SortedVecBench(const int& n) {
  TIntV Vec;
  float ft0, ft1;
  int x;
  int i;
  int Found;
  int NotFound;
  int Id;

  // build the vector
  ft0 = GetCPUTime();
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    //Vec.AddSorted(x);
    Vec.AddMerged(x);
  }
  printf("svec:          size %d\n", Vec.Len());

  ft1 = GetCPUTime();
  printf("svec: %7.3fs inserting  %d numbers\n",ft1-ft0,i);

  // search the vector
  ft0 = GetCPUTime();
  Found = 0;
  NotFound = 0;
  for (i = 0; i < n; i++) {
    x = (int) (drand48() * 100000000);
    Id = Vec.IsInBin(x);
    if (Id == 0) {
      NotFound++;
    } else {
      Found++;
    }
  }
  printf("svec:          found %d, notfound %d\n", Found, NotFound);

  ft1 = GetCPUTime();
  printf("svec: %7.3fs searching %d numbers\n",ft1-ft0,i);
}

int main(int argc, char* argv[]) {
  int i;
  float ft0;
  long tnow;
  struct timeval tval;
  char tstr[64];

  // initialize random
  ft0 = GetCPUTime();
  printf("      %7.3fs start\n",ft0);

  gettimeofday(&tval, 0);
  GetTimeStr(&tval, tstr);
  tnow = (long) tval.tv_sec*1000 + (long) tval.tv_usec/1000;
  srand48(tnow);

  for (i = 1000000; i <= 100000000; i *= 10) {
    RandBench(i);
    HashBench(i);
    FixedVecBench(i);
    VecBench(i);
    //SortedVecBench(n);
  }
}

