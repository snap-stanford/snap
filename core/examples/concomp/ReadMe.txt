========================================================================
    Connected components
========================================================================

Loads a directed (or undirected) graph and computes:
  -- weakly connected components: for any pair of nodes there is an 
     undirected path between nodes
  -- strongly connected components: (directed graph) for any pair of nodes in
     the component there is a directed path between them
  -- biconnected components: (undirected graph) any pair of nodes is 
     connected by 2 disjoint paths (removal of any single edge does not 
     disconnect the component
  -- articulation points: (undirected graph) vertices that if removed 
     disconnect the graph
  -- bridge edges: (undirected graph) edges that if removed disconnect the
     graph
  
Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graphs (each file is a graph snapshot, or use "DEMO") (default:'graph*.txt')
   -o:Output file name prefix (default:'over-time')

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute the components of the AS graph:

cncom -i:../as20graph.txt
