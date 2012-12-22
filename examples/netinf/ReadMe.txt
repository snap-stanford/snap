========================================================================
    Network Inference (NETINF)
========================================================================

Tracking how information diffuses or how viruses spread over networks has been
a topic of considerable interest.  While observing how nodes change state
(e.g., become infected) is often possible,  observing the individual
transmissions (i.e., who infects whom) is typically very difficult.
Furthermore, in many scenarios the underlying network over which the diffusions
and propagations take place is actually unobserved.

We tackle these challenges by developing NETINF to infer the network over which diffusion 
and infections propagate. Given the times when nodes adopt pieces of information or become 
infected (i.e. cascades), we develop a scalable algorithm to infer the links in the underlying 
network over which the propagations take place.

For more information about the procedure see:
  Inferring Networks of Diffusion 
  Manuel Gomez-Rodriguez, Jure Leskovec, Andreas Krause. 
  http://www.stanford.edu/~manuelgr/netinf-www10.pdf
  
/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input cascades (one file) (default:'example-cascades.txt')
   -n:Input ground-truth network (one file) (default:'example-network.txt')
   -o:Output file name(s) prefix (default:'network')
   -e:Number of iterations (default:'5')
   -a:Alpha for exponential model (default:1)
   -s:How much additional files to create?
    1:info about each edge, 2:objective function value (+upper bound), 3:Precision-recall plot, 4:all-additional-files (default:1)
 (default:1)
    
Generally -s:1 is the fastest (computes the least additional stuff), while -s:3 
takes longest to run but calculates all the additional stuff.

/////////////////////////////////////////////////////////////////////////////
Usage:

Infer the network given a text file with cascades (nodes and timestamps):

netinf -i:demo_cascades.txt

/////////////////////////////////////////////////////////////////////////////
Format input cascades:

The cascades input file should have two blocks separated by a blank line. 
- A first block with a line per node. The format of every line is <id>,<name>
- A second block with a line per cascade. The format of every line is <id>,<timestamp>,<id>,<timestamp>,<id>,<timestamp>...

A demo input file can be found under the name example-cascades.txt, example-network.txt.
/////////////////////////////////////////////////////////////////////////////
Format gound truth:

The ground truth input file should have two blocks separated by a blank line
- A first block with a line per node. The format of every line is <id>,<name>
- A second block with a line per directed edge. The format of every line is <id1>,<id2>