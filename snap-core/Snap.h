#ifndef SNAP_H
#define SNAP_H

/// ##mainpage
#define _USE_MATH_DEFINES    // to use cmath's constants for VS

/////////////////////////////////////////////////
// SNAP library
#include "base.h"
#include "gnuplot.h"
#include "linalg.h"

#include "gbase.h"
#include "util.h"

// graph data structures
#include "graph.h"           // graphs
#include "mgraph.h"          // multimodal graphs
#include "network.h"         // networks
#include "networkmp.h"       // networks OMP
#include "bignet.h"          // large networks
#include "timenet.h"         // time evolving networks

// algorithms
#include "subgraph.h"        // subgraph manipulations
#include "anf.h"             // approximate diameter calculation
#include "bfsdfs.h"          // breadth and depth first search
#include "cncom.h"           // connected components
#include "kcore.h"           // k-core decomposition
#include "alg.h"             // misc graph algorithms
#include "triad.h"           // clustering coefficient and triads
#include "gsvd.h"            // SVD and eigenvector computations
#include "gstat.h"           // graph statistics
#include "centr.h"           // centrality measures
#include "cmty.h"            // community detection algorithms
#include "flow.h"            // network flow algorithms
#include "coreper.h"         // core-periphery algorithms

// graph generators
#include "ggen.h"            // graph generators (preferential attachment, small-world, ...)
#include "ff.h"              // forest fire graph generator

#include "gio.h"
#include "gviz.h"
#include "ghash.h"
#include "statplot.h"

// table data structures and algorithms
#include "predicate.h" 		 // predicate
#include "table.h"         // table
#include "conv.h" 				 // conversion functions - table to graph
#include "numpy.h" 				 // numpy conversion

// util
#include "stopwatch.h"

#endif // SNAP_H
