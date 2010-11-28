========================================================================
    Motifs: connected subgraph enumeration
========================================================================

Motifs is a fast and scalable algorithm for counting frequency of 
connected induced subgraphs in a network. The program counts the number 
of occurences of a every possible connected subgraph on K nodes in a 
given graph. Frequency of such network motifs can be used to compare and
characterize networks.

The algorithm is described in Efficient Detection of Network Motifs by 
Sebastian Wernicke. IEEE/ACM Transactions on Computational Biology and 
Bioinformatics, 2006.

For information about network motifs refer to Network motifs: Simple 
building blocks of complex networks by R. Milo, S. Shen-Orr, S. Itzkovitz, 
N. Kashtan, D. Chklovskii and U. Alon. Science, October 2002.

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input directed graph file (single directed edge per line) (default:'../as20graph.txt')
   -m:Motif size (has to be 3 or 4) (default:3)
   -d:Draw motif shapes (requires GraphViz) (default:'T')
   -o:Output file prefix (default:'')

Nodes of the graph have to be numbered 0...N-1   

/////////////////////////////////////////////////////////////////////////////
Usage:

Count the 3-motifs in the AS graph:

motifs -i:../as20graph.txt -m:3 -d:T -o:as-3motifs
