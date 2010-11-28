========================================================================
    Node centrality
========================================================================

Loads undirected graph and computes various node centrality measures:
  -- degree centrality
  -- closeness centrality
  -- betweenness centrality
  -- eigenvector centrality
For more details see http://en.wikipedia.org/wiki/Centrality
  
Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graph (tab separated list of edges) (default:'graph.txt')
   -o:Output file name (default:'node_centrality.txt')

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute centralities of the nodes in the AS graph:

centrality -i:../as20graph.txt
