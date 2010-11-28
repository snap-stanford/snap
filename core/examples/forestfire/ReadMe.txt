========================================================================
    ForestFire graph generator
========================================================================

Forest Fire graph generation model, is based on having new nodes attach to
the network by ``burning'' through existing edges in epidemic fashion. For
a range of parameter values the model exhibits realistic behavior in
densification, shrinking diameter, and degree distributions.

For more information about the model see:
  Graph Evolution: Densification and Shrinking Diameters 
  Jure Leskovec, Jon Kleinberg, Christos Faloutsos. 
  ACM Transactions on Knowledge Discovery from Data (ACM TKDD), 1(1), 2007. 
  http://arxiv.org/abs/physics/0603229
  
Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -o:Output graph file name (default:'graph.txt')
   -n:Number of nodes (size of the generated graph) (default:10000)
   -f:Forward burning probability (default:0.35)
   -b:Backward burning probability (default:0.32)
   -s:Start graph with S isolated nodes (default:1)
   -a:Probability of a new node choosing 2 ambassadors (default:0)
   -op:Probability of a new node being an orphan (node with zero out-degree) (default:0)

/////////////////////////////////////////////////////////////////////////////
Usage:

Generate a Forest Fire graph on 1000 nodes with forward burning probability
p_f=0.3 and backward burning probability p_b=0.25

forestfire -o:graph.txt -n:1000 -f:0.3 -b:0.25
