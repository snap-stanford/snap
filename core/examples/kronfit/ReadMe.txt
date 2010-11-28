========================================================================
    KronFit: estimate Kronecker graphs initiator matrix
========================================================================

KronFit is a fast and scalable algorithm for fitting the Kronecker graph
generation model to large real networks. A naive approach to fitting would
take super-exponential time. In contrast, KronFit takes linear time. KronFit
finds accurate parameters that very well mimic the properties of target
networks. In fact, using just four parameters we can accurately model several
aspects of global network structure.

For more information about the procedure see:
  Kronecker Graphs: an approach to modeling networks 
  Jure Leskovec, Deepayan Chakrabarti, Jon Kleinberg, Christos Faloutsos, 
  Zoubin Ghahramani. 
  http://arxiv.org/abs/0812.4905

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input graph file (single directed edge per line) (default:'../as20graph.txt')
   -o:Output file prefix (default:'')
   -n0:Innitiator matrix size (default:2)
   -m:Init Gradient Descent Matrix (R=random) (default:'0.9 0.7; 0.5 0.2')
   -p:Initial node permutation: d:Degree, r:Random, o:Order (default:'d')
   -gi:Gradient descent iterations (default:50)
   -l:Learning rate (default:1e-05)
   -mns:Minimum gradient step (default:0.005)
   -mxs:Maximum gradient step (default:0.05)
   -w:Samples to warm up (default:10000)
   -s:Samples per gradient estimation (default:100000)
   -sim:Scale the initiator to match the number of edges (default:'T')
   -nsp:Probability of using NodeSwap (vs. EdgeSwap) MCMC proposal distribution (default:1)

/////////////////////////////////////////////////////////////////////////////
Usage:

Estimate the 2-by-2 Kronecker initiator matrix for the Autonomous Systems 
network using 100 gradient descent iterations. We initialize the fitting with 
the [0.9 0.6; 0.6 0.1] initiator matrix.

kronfit -i:../as20graph.txt -n0:2 -m:"0.9 0.6; 0.6 0.1" -gi:100
