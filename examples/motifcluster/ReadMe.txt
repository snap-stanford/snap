========================================================================
    MotifCluster: motif-based clustering
========================================================================

The example implements the spectral motif-based clustering algorithm for
finding sets of nodes in a directed graph with low motif conductance.
Users can specify one of the following motif types:
-"M1", "M2", ..., "M13" for the thirteen directed connected motifs on three nodes
-"bifan" for the bifan motif on four nodes
-"edge" for standard spectral clustering on the undirected version of the graph

The methodology of the algorithm is explained in the following paper:
Austin R. Benson, David F. Gleich, and Jure Leskovec, Higher-order Organization of Complex Networks, 2016.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

NOTE: This code requires that ARPACK is installed on your machine. If you
are using homebrew, you can run "brew install arpack" to install this library.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input edgelist file name (default:'../as20graph.txt')
   -m:Motif type  (default:'M4')
/////////////////////////////////////////////////////////////////////////////
Usage:

Detect a M4-based cluster from an autonomous systems network.

motifclustermain -m:M4
