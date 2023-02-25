========================================================================
    Hypergraph Simultaneous Generators (HySGen)
========================================================================

This program fits a probabilistic generative model to undirected, unweighted 
hypergraphs to detect overlapping communities (node clusters) in hypergraphs. 
It takes a hyperedge list, the number of communities to be discovered, 
and several optional arguments as input, to produce a list of communities.
The details of the model and the community inference algorithm are described 
in the following paper:

B. Pedrood, C. Domeniconi, and K. Laskey. "Hypergraph Simultaneous Generators." AISTATS 2022.

This code works under Windows with Cygwin with GCC, Mac OS X, Linux and other 
Unix variants with GCC. To use with Visual Studio, you have to create a new 
project for this program. Make sure that a C++ compiler is installed on the 
system. Makefiles are provided, so you can complie the code in the command 
line with the following command: 
    make all

/////////////////////////////////////////////////////////////////////////////

Parameters:
   -i:  Input [hyper]edgelist file url.
   -o:  Output file url + name prefix for the discovered communities.
   -c:  The number of communities to detect.
   -op: Output file performance plot (Default: empty for no plot).
   -ci: Community initialization file url (Default: empty).
   -l:  Url for node names file (Default: empty).
   -mc: Minimum size of the communities(Default: 3).
   -rs: Random Seed.
   -xi: Maximum number of iterations (Default: 1000).
   -ic: Initial membership value for the seed communities (Default: 0.1).
   -in: The default membership value of each node to all the communities (Default: 0.03).
   -rp: Ratio of initial memberships to be randomly perturbed (Default: 0.0).
   -rw: Weight for l-1 regularization on learning the model parameters (Default: 0.005)
   -sz: Initial step size for backtracking line search (Default: 0.05).
   -sa: Control parameter for backtracking line search (Default: 0.1).
   -sr: Step-size reduction ratio for backtracking line search (Default: 0.5).
   -th: Cut-off threshold for the final community membership values (Default: the l-1 regularization value).

/////////////////////////////////////////////////////////////////////////////

Usage:

Discover 2 communities from the synthtic hypergraph (under synthetic_data/):

./hysgen_main -i:./synthetic_data/synthetic.hyperedges -o:./synthetic_res -c:2 -th:0.1 -rs:1


** For real-world hypergraph data please visit https://github.com/bpedrood/HySGen
