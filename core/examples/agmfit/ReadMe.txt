========================================================================
    AGMfit: Detection of overlapping communities by fitting the Affiliated Graph Model (AGM)
========================================================================

AGMfit is a fast and scalable algorithm to detect overlapping communities from a given graph
by fitting the Affilation Graph Model (AGM) to the graph. As AGM evaluates the likelihood of 
community memberships for a given graph, AGMfit performs a stochastic random search over the space
of all possible community memberships using the Metropolis-Hastings update rule. The AGM can reveal
various kinds of community structure depending on the structure of a graph; for example, communities 
can exist disjointly, or they can form a nested structure. The computation time is quadratic in the
number of nodes and linear in the number of edges.

If a user specifies the number of communities that the user want to detect, AGMfit finds the corresponding 
number of communities. If a user does not know how many communities would exist, which is more realistic, 
AGMfit automatically estimates the number of communities in the graph.
User also can control the probability of edges between the nodes that do not share any communities. 
If a user does not assume a certain probability, AGMfit uses (1 / N^2) where N is the number of nodes in the graph.

For more detail, refer to the following paper: (currently under submission)
Model-based approach to detecting densely overlapping communities in networks: J. Yang and J. Leskovec.

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input graph file (single undirected edge per line) (default:'./football.names.edgelist') 'DEMO': use a synthetic network by AGM with 2 communities.
   -o:Output file prefix (default:'')
   -s:Random seed (default: 10)
   -c:Number of communities to detect (default: 0) 0 means to estimate the number by AGM
   -e:Probability of an edges between the nodes that do not share any community (default: 0 ) If this input is 0, AGMfit sets it to be (1 / N^2).

/////////////////////////////////////////////////////////////////////////////
Usage:

Find 12 communities from the NCAA football network. Use 0.1 as an edge probability between the nodes having no community in common. 

agmfit -i:football.names.edgelist -c:12 -e:0.1

/////////////////////////////////////////////////////////////////////////////
