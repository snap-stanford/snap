========================================================================
    Clique Percolation Method for detecting overlapping communities
========================================================================

The example find overlapping dense groups of nodes in networks, based on the 
Clique Percolation Method (CPM). For example, see 
http://cfinder.org/wiki/?n=Main.ImageWords

The Clique Percolation Method is described in G. Palla, I. Derenyi, I. Farkas, 
T. Vicsek, Uncovering the overlapping community structure of complex networks 
in nature and society, Nature 435, 814-818 (2005).

The maximal clique enumeration procedure implements the method by E. Tomita, 
A. Tanaka, H. Takahashi. The worst-case time complexity for generating all 
maximal cliques and computational experiments. Theoretical Computer Science, 
Volume 363, Issue 1, 2006.

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input undirected graph file (single directed edge per line) (default:'../as20graph.txt')
   -k:Minimal clique overlap size (default:3)
   -o:Output file prefix (default:'')

/////////////////////////////////////////////////////////////////////////////
Usage:

Enumerate the communities in the AS graph:

cliques -i:../as20graph.txt -k:2 -o:as20
