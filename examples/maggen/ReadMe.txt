========================================================================
    MAGGen: Multiplicative Attribute Graph (MAG) generator
========================================================================

MAG graphs is a generative network model which considers categorical node 
attributes and corresponding affinity matrices. 

Affinity matrix represents the network structure by refelcting the affinity 
of link formation between a pair of nodes depending on each node attribute. 
For a binary valued attribute example, the affinity matrix should be a 
2 by 2 matrix where each entry (i, j) indicates the affinity between a pair 
of nodes that respectively take value i and j for that attribute. Therefore,
for instance, a homophily structure (community structure) can be represented
when diagonal entries take larger values than off-diagnal entries.

The MAG model combines these affinities from node attributes via multiplication.
In the result, the MAG model gives rise to networks that have lots of network
properties observed in real-world networks. For example, the networks generated
by the MAG models follow the Densification Power Law, have a small diameter,
show either a power-law or a log-normal degree distribution, etc.

For more information about the procedure see:
  Multiplicative Attribute Graph Model of Real-World Networks
  Myunghwan Kim and Jure Leskovec
  http://arxiv.org/abs/1009.3499

Here we provide two versions of generators.

1. Simplified version
  Every node attribute is governed by one Bernoulli distribution and
  every affinity matrix is the same as a 2 by 2 matrix. Therefore, you need
  only 4 parameters : # of nodes, # of attributes, Bernoulli parameter, and
  an affinity matrx.

2. Bernoulli-distributed node attributes
  Each node attribute is governed by its own Bernoulli distribution. Moreover,
  every affinity matrix may be different. Thus, to generate this version of
  MAG graph, you need to specify both a Bernoulli parameter and an affinity matrix
  for every node attribute (in the configuration file).

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graph configuration file name (except for Simplified MAG) (default:'graph.config')
   -o:Output graph file name (default:'graph.txt')
   -s:Random seed (0 - time seed) (default:1)
   -n:The number of nodes (default:10240)
   -simple:Simplified MAG option (default:'T')
   -l:The number of attributes (only for Simplified MAG) (default:8)
   -mu:Equalized Bernoulli parameter for every node attribute (only for Simplified MAG) (default:0.45)
   -m:Equalized affinity matrix for every node attribute (only for Simplified MAG) (default:'0.8 0.35;0.35 0.15')

/////////////////////////////////////////////////////////////////////////////
Usage:

1. Simplified version
Generate a simplified MAG graph on 1024 (2^10) nodes of 5 attributes with the 
Bernoulli parameter (0.45) and the affinity matrix [0.9 0.6; 0.6 0.1]:

maggen -simple:T -o:mag_simple.txt -n:1024 -l:5 -mu:0.45 -m:"0.9 0.6; 0.6 0.1"

2. Bernoulli-distributed node attributes
Generate a MAG graph of Bernoulli-distributed node attributes on 4096 (4K) nodes
of configuration specified in mag.config file. Each line of configuration file has 
the form of "Bernoull-parameter & affinity-matrix". Therefore, the number of lines 
indicates the number of node attributes.

maggen -simple:F -i:mag.config -o:mag_bern.txt -n:4096

mag.config 
0.60 & 0.9999 0.0432; 0.0505 0.9999
0.35 & 0.9999 0.9999; 0.9999 0.1506
0.25 & 0.9999 0.9999; 0.9999 0.2803
0.15 & 0.9999 0.9999; 0.9999 0.2833
0.65 & 0.9999 0.0476; 0.0563 0.9999
0.10 & 0.9999 0.9999; 0.9999 0.1319
0.55 & 0.9999 0.1246; 0.1402 0.9999
0.55 & 0.9999 0.1186; 0.1364 0.9999
0.40 & 0.9999 0.1757; 0.1535 0.9999
