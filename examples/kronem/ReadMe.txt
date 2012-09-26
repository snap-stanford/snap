========================================================================
    KronEM: estimate Kronecker graphs initiator matrix using EM alg.
========================================================================

KronEM is a scalable algorithm for fitting the Kronecker graph model to 
large real networks when some nodes and edges are missing. In particular,
since a Kronecker graph needs to have n0^k nodes, we can regard most real 
networks that do not have n0^k nodes as this case.
(n0: dimension of Kronecker initiator matrx, k: any integer)

As KronFit does not take missing nodes and edges into account, it tends to
underestimate the Kronecker initiator matrix when the actual number of nodes
in the real network does not match n0^k.  In contrast, KronEM uses the EM 
algorithm considering the missing nodes and edges as latent variables.
Therefore, KronEM resolves the underestimation issue in KronFit.

Similarly to KronFit, using just four parameters we can accurately model several
aspects of global network structure.

For more information about the procedure see:
  The Network Completion Problem: Inferring Missing Nodes and Edges in Networks
  Myunghwan Kim and Jure Leskovec
  http://www-cs.stanford.edu/people/jure/pubs/kronEM-sdm11.pdf

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
usage: kronem
   -i:Input graph file (single directed edge per line) (default:'../as20graph.txt')
   -o:Output file prefix (default:'')
   -n0:Initiator matrix size (default:2)
   -m:Init Gradient Descent Matrix (R=random) (default:'0.9 0.7; 0.5 0.2')
   -gi:Gradient descent iterations for M-step (default:5)
   -ei:EM iterations (default:30)
   -l:Learning rate (default:1e-05)
   -mns:Minimum gradient step for M-step (default:0.001)
   -mxs:Maximum gradient step for M-step (default:0.008)
   -w:Samples for MCMC warm-up (default:5000)
   -s:Samples per gradient estimation (default:1500)
   -sim:Scale the initiator to match the number of edges (default:'F')
   -nsp:Probability of using NodeSwap (vs. EdgeSwap) MCMC proposal (default:0.6)
   -debug:Debug mode (default:'F')

/////////////////////////////////////////////////////////////////////////////
Usage:

Estimate the 2-by-2 Kronecker initiator matrix for the Autonomous Systems 
network using 50 EM iterations. We initialize the fitting with 
the [0.9 0.6; 0.6 0.1] initiator matrix.

kronem -i:../as20graph.txt -n0:2 -m:"0.9 0.6; 0.6 0.1" -ei:50
