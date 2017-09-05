========================================================================
    knnjaccardsim: KNN Jaccard Similarity
========================================================================

The example finds K nearest neighbor for every source node in the graph based
on jaccard similarity and returns it as a PNEANet graph. 
Input: a table, src & dst node indexes in the table schema such that they
       form a bipartite graph, K.
Node format in the input file name is as follows:
  <src> <dst> 
<src> is the source node, <dst> is the destination node.
They form a bipartite graph.

/////////////////////////////////////////////////////////////////////////////
Parameters:

/////////////////////////////////////////////////////////////////////////////
Usage: ./knnjaccardsim <filename> <K>

/////////////////////////////////////////////////////////////////////////////
Output: three columns giving edges with the K nearest neighbors and the
  edge similarity score

Example:
  knnjaccardsim reality.txt 3

