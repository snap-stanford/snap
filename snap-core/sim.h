
#ifndef SIM_H

//#include <iostream>
//#include <fstream>
//#include <string>
//#include "Snap.h"
//#include <stdlib.h>
//#include <time.h>
//#include <unistd.h>
//using namespace std;

PNGraph GetBiGraph(PTable P, int index_col_1, int index_col_2);
PNEANet KNNJaccard(PNGraph Graph,int K);
#ifdef GCC_ATOMIC
PNEANet KNNJaccardParallel(PNGraph Graph,int K);
#endif

#endif
