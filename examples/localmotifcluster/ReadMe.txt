========================================================================
    LocalMotifCluster: local motif-based clustering using MAPPR
========================================================================

The example implements the MAPPR local clustering algorithm for finding sets
of nodes in a directed/undirected graph around a targeted seed node with
low motif conductance.

For directed graph, users can specify one of the following motif types:
-"M1", "M2", ..., "M7" for the seven directed connected motifs on three nodes
  defined by Benson et al. Science 2016;
-"edge", "cycle", "FFLoop" specified in Yin et al. KDD 2017.

For undirected graph, users can choose motif to be any clique of size 2 - 9:
-"UEdge", "clique3", "clique4", ..., "clique9".

The methodology of the algorithm is explained in the following paper:
Local Higher-Order Graph Clustering.  Hao Yin, Austin R. Benson, Jure Leskovec, 
and David F. Gleich, KDD, 2017.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -d:indicating whether input graph is directed (default:'N' for not directed)
   -i:Input edgelist file name (default:'C-elegans-frontal.txt')
   -m:Motif type  (default:'clique3')
   -s:Seed node id (default:'1')
   -a:alpha (default:'0.98')
   -e:epsilon (default:'0.0001')
/////////////////////////////////////////////////////////////////////////////
Usage:

Detect a local feed-forward loop based cluster from the C-elegans-frontal network.

./localmotifclustermain -d:Y -i:C-elegans-frontal.txt -m:FFLoop -s:1

