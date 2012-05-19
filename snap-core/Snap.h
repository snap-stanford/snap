#ifndef snap_main_h
#define snap_main_h

/////////////////////////////////////////////////
// SNAP library
#include "base.h"
#include "gnuplot.h"
#include "linalg.h"

#include "gbase.h"
#include "util.h"

// graph data structures
#include "graph.h"           // graphs
#include "network.h"         // networks
#include "bignet.h"          // large networks
#include "timenet.h"         // time evolving networks

// algorithms
#include "subgraph.h"        // subgraph manipulations
#include "anf.h"             // approximate diameter calculation
#include "bfsdfs.h"          // breath and depth first search
#include "cncom.h"           // connected components
#include "kcore.h"           // k-core decomposition
#include "alg.h"             // misc graph algorithms
#include "triad.h"           // clustering coefficient and triads
#include "gsvd.h"            // SVD and eigenvector computations
#include "gstat.h"           // graph statistics
#include "centr.h"           // centrality measures
#include "cmty.h"            // community detection algorithms

// graph generators
#include "ggen.h"            // graph generators (preferential attachment, small-world, ...)
#include "ff.h"              // forest fire graph generator

#include "gviz.h"
#include "ghash.h"
#include "statplot.h"
#include "gio.h"

#endif
