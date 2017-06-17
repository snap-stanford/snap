========================================================================
    K-truss network decomposition
========================================================================

A k-truss is a cohesive subgraph such that every edge is in at least k triangles in the subgraph. This is a relaxation of a clique. 

The GetTruss function returns table of truss values indicating, for each edge, the maximal k-truss the edge occurs in. Note that if an edge is in a k-truss, it is in a (k-1)-truss, (k-2)-truss, and so on. 

Currently, only undirected graphs (TUNGraphs) are supported. Since these do not have edge IDs, for the purposes of the truss output, the edges are specified using source and destination nodes. Thus, the output is a hash table of type TIntPrInt, where the keys (IntPr) are the edges (source and destination node pairs) and the values (Int) are the maximal k-truss the edge occurs in. Please see the example where the output is written to file. 

References: 

Cohen, Jonathan. "Trusses: Cohesive subgraphs for social network analysis." National Security Agency Technical Report (2008): 16.

Wang, Jia, and James Cheng. "Truss decomposition in massive networks." Proceedings of the VLDB Endowment 5.9 (2012): 812-823.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input undirected graph file (single undirected edge per line) (default:'../as20graph.txt')

/////////////////////////////////////////////////////////////////////////////
Usage:

Enumerate the communities in the AS graph:

./truss -i:../as20graph.txt
