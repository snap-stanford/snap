========================================================================
    Flows: Single commodity maximal network flow
========================================================================

This example shows how to compute the maximum network flow on a network
using the functions provided in snap-core/flow.h. A text file for a network
with edge capacities must be supplied by the user. Each run of the example will
choose a random source and sink node and run both the Edmonds-Karp algorithm 
and Push-Relabel algorithm between the two nodes. Users may specify to run the
example many times, and with multiple threads at once.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input text file for network (default:'')
   -n:Number of runs per thread
   -t:Number of threads to run

/////////////////////////////////////////////////////////////////////////////
Usage:

Run the flow algorithms 200 times over 10 threads on a small sample network.

./flows -i:small_sample.txt -n:20 -t:10 
