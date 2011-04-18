========================================================================
    Node centrality
========================================================================

Loads a directed graph and computes the following node centrality measures.

Measures defined on a undirected graph (we drop edge directions):
  -- degree centrality
  -- closeness centrality
  -- betweenness centrality
  -- eigenvector centrality
Measures defined on (the original) directed graph
  -- page rank
  -- hubs and authorities

For more details on defitions of these measures see 
http://en.wikipedia.org/wiki/Centrality
  
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
