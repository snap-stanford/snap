========================================================================
    TemporalMotifs: motifs for temporal graphs
========================================================================

The example counts all three-node, three-edge temporal motifs in a temporal
network.  The methodology is explained in the following paper:

Motifs in Temporal Networks.  Ashwin Paranjape, Austin R. Benson, and Jure
Leskovec.  In Proceedings of the International Conference on Web Search and Data
Mining (WSDM), 2017.

The example formats the motif counts in a 6 x 6 table that matches the
presentation in the above paper.

Each line of the data file is assumed to take the following format:

src_id dst_id timestamp

which src_id and dst_id are nonnegative integers (as would be used for a SNAP
graph) and timestamp is an integer (as in a UNIX timestamp of seconds from the
epoch).

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input edgelist file name (default:'example-temporal-graph.txt')
   -delta:Time window delta (default:4096)
   -o:Output file (default:'temporal-motif-counts.txt')
   -nt:Number of threads (default:4)
/////////////////////////////////////////////////////////////////////////////
Usage:

Count all 2-node and 3-node temporal motifs with 3 temporal edges in
the graph stored in example-temporal-graph.txt using a time window
of 300.  Results are written to out.txt.

temporalmotifsmain -i:example-temporal-graph.txt -delta:300 -o:out.txt
