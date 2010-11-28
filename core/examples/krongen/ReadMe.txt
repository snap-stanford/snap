========================================================================
    KronGen: Kronecker graphs graph generator
========================================================================

Kronecker graphs is a generative network model which obeys all the main
static network patterns that have appeared in the literature. The model also
obeys recently discovered temporal evolution patterns like shrinking diameter
and densification power law. Kronecker graphs also lead to tractable analysis
and rigorous proofs. The model is based on a matrix operation, the Kronecker
product, and produces networks with heavy-tailed distributions for in-degree,
out-degree, eigenvalues, and eigenvectors.

Given an initiator matrix M the application generates a corresponding
Kronecker graph. If you want to estimate M for a given graph G use the
'kronfit' application.

For more information about the procedure see:
  Kronecker Graphs: an approach to modeling networks 
  Jure Leskovec, Deepayan Chakrabarti, Jon Kleinberg, Christos Faloutsos, 
  Zoubin Ghahramani. 
  http://arxiv.org/abs/0812.4905

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -o:Output graph file name (default:'graph.txt')
   -m:Matrix (in Maltab notation) (default:'0.9 0.5; 0.5 0.1')
   -i:Iterations of Kronecker product (default:5)
   -s:Random seed (0 - time seed) (default:0)

/////////////////////////////////////////////////////////////////////////////
Usage:

Generate a Stochastic Kronecker graph on 1024 (2^10) nodes with the 
initiator matrix [0.9 0.6; 0.6 0.1]:

krongen -o:kronecker_graph.txt -m:"0.9 0.6; 0.6 0.1" -i:10
