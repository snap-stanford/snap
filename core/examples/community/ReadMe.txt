========================================================================
    Network community detection algoritms
========================================================================

Implements two community detection algoritms: 
  -- Girvan-Newman algorithm (Girvan M. and Newman M. E. J., Community 
  structure in social and biological networks, Proc. Natl. Acad. Sci. 
  USA 99, 7821–7826 (2002))
  -- fast modularity maximization algoritm by 'Finding Large community 
  in networks', A. Clauset, M.E.J. Newman, C. Moore, 2004
  
Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graph (tab separated list of edges) (default:'graph.txt')
   -o:Output file name (default:'communities.txt')
   -a:Algorithm: 1:Girvan-Newman, 2:Clauset-Newman-Moore (default:2)

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute communities in the AS graph:

community -i:../as20graph.txt -a:2
