========================================================================
    BIGCLAM: CLuster Affiliation graph Model for BIG networks.
========================================================================

The example implements a large scale overlapping community detection method 
based on Cluster Affiliation Graph Model for Big Networks (BIGCLAM).
This program formulates community detection problems into non-negative matrix factorization
and discovers community membership factors of nodes by maximum likelihood estimation.
User can specify how many communities she would detect, or let the program determine
the number of communities in the network from the topology of the network.

Fitting procedure and the community-Affiliation Graph Model are described in the following paper:
J. Yang and J. Leskovec, Overlapping Community Detection at Scale: A Nonnegative Matrix Factorization, WSDM '13.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -o:Output Graph data prefix (default:'')
   -i:Input edgelist file name (default:'../as20graph.txt')
   -l:Input file name for node names (Node ID, Node label)  (default:'')
   -c:The number of communities to detect (-1: detect automatically) (default:-1)
   The following three parameters are for finding the number of communities to detect.
   The program tries nc numbers from mc to xc: by default, it tries 10 values from 5 to 100.
   -mc:Minimum number of communities to try (default:5) 
   -xc:Maximum number of communities to try (default:100)
   -nc:How many trials for the number of communities (default:10)
   
   -nt:Number of threads for parallelization (default:1) -nt:1 means no parallelization.
   
   The following two parameters are for backracking line search described in Convex Optimization, Boyd and Vandenberghe, 2004.
   Refer to the book for the backtracking line search algorithm.
   -sa:Alpha for backtracking line search (default:0.3)
   -sb:Beta for backtracking line search (default:0.3)

/////////////////////////////////////////////////////////////////////////////
Usage:

Detect 200 communities from an autonomous systems network.

bigclam -c:200
