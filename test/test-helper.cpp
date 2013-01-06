//
//  test-helper.cpp
//  snap-test
//
//  Created by Nicholas Shelly on 12/30/12.
//  Copyright (c) 2012 infolab. All rights reserved.
//

#include "Snap.h"

#include <sys/stat.h>
#include <stdio.h>
#include "test-helper.h"

using namespace TSnap;

bool fileExists(const std::string& filename)
{
  struct stat buf;
  if (stat(filename.c_str(), &buf) != -1)
  {
    return true;
  }
  return false;
}

bool compareFiles(const std::string& fname1, const std::string& fname2) {
  
  FILE *f1 = fopen(fname1.c_str(), "r"), *f2 = fopen(fname2.c_str(), "r");
  
  const int N = 10000;
  char buf1[N];
  char buf2[N];
  
  do {
    size_t r1 = fread(buf1, 1, N, f1);
    size_t r2 = fread(buf2, 1, N, f2);
    
    if (r1 != r2 ||
        memcmp(buf1, buf2, r1)) {
      return false;
    }
  } while (!feof(f1) || !feof(f2));
  
  return true;
}

void printv(TIntV IntV) {
  
  printf("{ ");
  for (int i = 0; i < IntV.Len(); i++){
    printf("%d ", IntV[i].Val);
  }
  printf("}\n");
}