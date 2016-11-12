========================================================================
    TemporalMotifs: motifs for temporal graphs
========================================================================

The example counts all three-node, three-edge temporal motifs in a temporal
network.  The methodology is explained in the following paper:

Motifs in Temporal Networks.  Ashwin Paranjape, Austin R. Benson, and Jure
Leskovec.  In Proceedings of the International Conference on Web Search and Data
Mining (WSDM), 2017.

By default, the example emits the motif counts in a 6 x 6 table that matches the
presentation in the above paper.  Options are also available to just count one
of the following:
(1) two-node, three-event "edge motifs"
(2) three-node, three-event "star motifs"
(3) three-node, three-event "triangle motifs"

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
   -i:Input edgelist file name (default:'../as20graph.txt')
   -m:Motif type  (default:'M4')
/////////////////////////////////////////////////////////////////////////////
Usage:

Detect a M4-based cluster from an autonomous systems network.

motifclustermain -m:M4
