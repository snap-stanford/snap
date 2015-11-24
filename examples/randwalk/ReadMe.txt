========================================================================
    Random Walks
========================================================================

Loads a directed graph and computes personalized PageRank between a pair of nodes, and between a source sampled from a set of start nodes and a target node.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graph (tab separated list of edges) (default:'../as20graph.txt')

/////////////////////////////////////////////////////////////////////////////
Usage:

randwalk -i:../as20graph.txt
