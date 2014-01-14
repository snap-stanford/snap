========================================================================
    Network community detection algoritms
========================================================================

Implements three community detection algoritms: 
  -- Girvan-Newman algorithm (Girvan M. and Newman M. E. J., Community 
  structure in social and biological networks, Proc. Natl. Acad. Sci. 
  USA 99, 7821-7826 (2002))
  -- fast modularity maximization algoritm by 'Finding Large community 
  in networks', A. Clauset, M.E.J. Newman, C. Moore, 2004
  -- Infomap algorithm (Rosvall M., Bergstrom C. T., Maps of random 
  walks on complex networks reveal community structure, Proc. Natl. 
  Acad. Sci. USA 105, 1118-1123 (2008))

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graph (tab separated list of edges) (default:'graph.txt')
   -o:Output file name (default:'communities.txt')
   -a:Algorithm: 1:Girvan-Newman, 2:Clauset-Newman-Moore, 3:Infomap (default:2)

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute communities in the AS graph:

community -i:../as20graph.txt -a:2
