
#ifndef SIM_H

#include <iostream>
#include <fstream>
#include <string>
#include "Snap.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
using namespace std;

PNGraph GetPNGraph(PTable P, int index_col_1, int index_col_2);
PNEANet KNNJaccard(PNGraph Graph,int K);
PNEANet KNNJaccardParallel(PNGraph Graph,int K);

#endif
