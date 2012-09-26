========================================================================
    K-core network decomposition
========================================================================

Plot the number of nodes in a k-core of a graph as a function of k.

A subgraph H = (C,E|C) induced by the set C subset of V is a k-core or a 
core of order k if and only if the degree of every node v in C induced in 
H is greater or equal than k, and H is the maximum subgraph with this 
property.

A k-core of G can be obtained by recursively removing all the vertices of 
degree less than k, until all vertices in the remaining graph have degree
at least k.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input undirected graph file (single directed edge per line) (default:'../as20graph.txt')
   -k:Minimal clique overlap size (default:3)
   -o:Output file prefix (default:'')

/////////////////////////////////////////////////////////////////////////////
Usage:

Enumerate the communities in the AS graph:

cliques -i:../as20graph.txt -k:2 -o:as20
