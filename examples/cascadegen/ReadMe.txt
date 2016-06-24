========================================================================
    Cascadegen: Cascade Detection
========================================================================

The example detects cascades from a list of events. Parameter W specifies
W-adjacent events in a cascade. The program builds a graph of W-adjacent
events, each connected component in the graph is a cascade.

Event format in the input file name is as follows:
 <src> <dst> <start_time> <duration>
<src> is the event source node, <dst> is the event destination node,
<start_time> is event start, <duration> is event duration. Events A and B
are W-adjacent, if A_<dst> is equal to B_<src> and the time difference
between the end of A and the start of B is less than W.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:

/////////////////////////////////////////////////////////////////////////////
Usage: ./cascadegen <filename> <W>

/////////////////////////////////////////////////////////////////////////////
Output: A directed graph of W-adjacent events in cascades.txt

Example:
  cascadegen reality.txt 10000

