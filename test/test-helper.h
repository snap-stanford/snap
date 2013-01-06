//
//  test-helper.h
//  snap-test
//
//  Utility functions for tests.
//  Created by Nicholas Shelly on 12/30/12.
//

#ifndef __snap_test__test_helper__
#define __snap_test__test_helper__

#include "Snap.h"

//#include <stdio.h>
#include <string>

bool fileExists(const std::string& filename);
bool compareFiles(const std::string& f1, const std::string& f2);
void printv(TIntV IntV);

#endif /* defined(__snap_test__test_helper__) */
