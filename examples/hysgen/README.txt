========================================================================
    Hypergraph Simultaneous Generators (HySGen)
========================================================================

This repository contains three main components:

1) HySGen: An efficient probabilistic generative model for discovering node 
      clusters/communities in hypergraphs. For the details of the model and 
      the community inference algorithm, please see our paper*.
2) HGraph: A fast, reliable, and comprehensive C++ data structure for 
      undirected, unweighted hypergraphs.
3) Three hypergraphs extracted from real-world data, uploaded in the "Data" 
      directory. Please see our paper* for more information.

Please cite our paper upon using any of those components:
* B. Pedrood, C. Domeniconi, and K. Laskey. "Hypergraph Simultaneous Generators." AISTATS 2022.

/////////////////////////////////////////////////////////////////////////////

The code in this project is developed on top of the SNAP [(c) 2007-2019, 
Jure Leskovec] open-source graph analysis library. To facilitate the usage 
for SNAP users, I maintained the structure and code standarads as recommended 
in SNAP. The directory structure of this project is as follows below:

snap: 
        An intact copy of the original SNAP library's source code, which 
        modules are used in this project.
local_snap: 
        We developed our classes in this directory. The subdirectories 
        and file structures are chosen this way for maximum consistency 
        with SNAP.
local_snap/snap-adv:
        HySGen's implemented classes and functions for community inference.
local_snap/snap-core:
        HGraph data structure is implemented in this directory. To see 
        the details of the function and classes, see the files with 
        "loc_graph" and "loc_subgraph" names.

Like other SNAP projects, this code works under Windows with Cygwin with GCC, 
Mac OS X, Linux and other Unix variants with GCC. To use with Visual Studio, 
you have to create a new project for this project. Make sure that a C++ compiler 
is installed on the system. Makefiles are provided, so you can complie the code 
in the command line with the following command: 
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
   -rw: Weight for l-1 regularization on learning the model parameters (Default: 0.0)
   -sz: Initial step size for backtracking line search (Default: 0.5).
   -sa: Control parameter for backtracking line search (Default: 0.5).
   -sr: Step-size reduction ratio for backtracking line search (Default: 0.5).

/////////////////////////////////////////////////////////////////////////////

Usage:

Discover 309 communities from the NSF collaboration hypergraph.

./hysgen_main -i:./Data/NSF/hypergraph.hyperedges -o:./out_communities -c:309 -mc:3 -ic:0.1 -in:0.001 -rw:0.0001 -sa:0.95 -sz:0.01
