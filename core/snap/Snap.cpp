/////////////////////////////////////////////////
// SNAP library
#include "stdafx.h"
#include "Snap.h"

#include "../glib/base.cpp"
#include "../glib/gnuplot.cpp"
#include "../glib/linalg.cpp"

#include "gbase.cpp"
#include "util.cpp"

// graph data structures
#include "graph.cpp"         // graphs
#include "timenet.cpp"       // time evolving networks

// algorithms
#include "subgraph.cpp"      // subgraph manipulations
#include "anf.cpp"           // approximate diameter calculation
#include "cncom.cpp"         // connected components
#include "alg.cpp"           // misc graph algorithms
#include "gsvd.cpp"          // SVD and eigenvector computations
#include "gstat.cpp"         // graph statistics
#include "centr.cpp"         // centrality measures
#include "cmty.cpp"          // community detection algorithms

// graph generators
#include "ggen.cpp"          // graph generators (preferential attachment, small-world, ...)
#include "ff.cpp"            // forest fire graph generator

#include "gviz.cpp"
#include "ghash.cpp"
//#include "casc.cpp"
//#include "gproj.cpp"
//#include "sampl.cpp"
//#include "layout.cpp"
//#include "plots.cpp"
#include "statplot.cpp"
#include "gio.cpp"
