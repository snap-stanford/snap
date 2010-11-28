========================================================================
    NCP plot: creates the Network Community Profile Plot
========================================================================

Network Community Profile plot measures the score of ``best'' community as a
function of community size in a network. Formally, we define it as the
conductance value of the minimum conductance set of cardinality k in the
network, as a function of k. As defined, the NCP plot will be NP-hard to
compute exactly, so operationally we will use several natural approximation
algorithms for solving the Minimum Conductance Cut Problem in order to
compute different approximations to it.

The shape of the plot offers insights into the large scale community
structure of networks. Networks with nice and/or hierarchical community
structure have  a downward sloping NPC plot. Random graphs have flat NCP
plot. Large real networks tend to have V shaped (down and up) NCP plotm which
illustrates not only tight communities at very small scales, but also that at
larger and larger size scales the best possible communities gradually ``blend
in'' more and more with the rest of the network and thus gradually become
less and less community-like.

For more information about the procedure see:
  Community Structure in Large Networks: Natural Cluster Sizes and the
  Absence of Large Well-Defined Clusters 
  Jure Leskovec, Kevin Lang, Anirban Dasgupta, Michael Mahoney.
  http://arxiv.org/abs/0810.1355

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input undirected graph (one edge per line) (default:'as20graph.txt')
   -o:Output file name (default:'')
   -d:Description (default:'')
   -d:Draw largest D whiskers (default:-1)
   -k:Take core (strip away whiskers) (default:'F')
   -w:Do bag of whiskers (default:'F')
   -r:Do rewired network (default:'F')
   -s:Save info file (for each size store conductance, modulariy, ...) (default:'F')
   -kmin:minimum K (volume) (default:10)
   -kmax:maximum K (volume) (default:100000000)
   -c:coverage (so that every node is covered C times) (default:10)
   -v:Verbose (plot intermediate output) (default:'T')

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute the NCP plot of the Autonomous systems graphs (as20graph.txt)

ncpplot -i:as20graph.txt 
