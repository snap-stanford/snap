========================================================================
    Structure and Dynamics of Information Pathways in On-line Media    
========================================================================

Diffusion of information, spread of rumors and infectious diseases are all
instances of stochastic processes that occur over the edges of an underlying
network. Many times networks over which contagions spread are unobserved and 
need to be inferred from the diffusion data. Moreover, such networks are often 
dynamic and change over time.

We have developed an on-line algorithm that relies on stochastic gradient
descent to efficiently infer dynamic networks based on information diffusion 
data. We assume there is an unobserved dynamic network that changes over time,
while we observe the results of a dynamic process spreading over the edges
of the network. The task then is to infer the edges and the dynamics of the
underlying network.

For more information about the procedure see:
  Structure and Dynamics of Information Pathways in On-line Media
  Manuel Gomez-Rodriguez, Jure Leskovec and Bernhard Schölkopf
  http://www.stanford.edu/~manuelgr/dynamic/
  
Usage:

Infer the network given a text file with cascades (nodes and timestamps):

./infopath -i:cascades.txt

All arguments are shown any time ./infopath is run.

/////////////////////////////////////////////////////////////////////////////
Format input cascades:

The cascades input file should have two blocks separated by a blank line. 
- A first block with a line per node. The format of every line is <id>,<name>
- A second block with a line per cascade. The format of every line is <cascade id>;<id>,<timestamp>,<id>,<timestamp>,<id>,<timestamp>...
