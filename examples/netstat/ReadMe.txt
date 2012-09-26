========================================================================
    NetStat: calculate structural properties of a network
========================================================================

Computes the structural properties of a network:
	- cummulative degree distribution
	- degree distribution
	- hop plot (diameter)
	- distribution of weakly connected components
	- distribution of strongly connected components
	- clustering coefficient
	- singular values
	- left and right singular vector

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input graph (one edge per line, tab/space separated) (default:'graph.txt')
   -d:Directed graph (default:'T')
   -o:Output file prefix (default:'graph')
   -t:Title (description) (default:'')
   -p:What statistics to plot string:
        c: cummulative degree distribution
        d: degree distribution
        h: hop plot (diameter)
        w: distribution of weakly connected components
        s: distribution of strongly connected components
        C: clustering coefficient
        v: singular values
        V: left and right singular vector
    

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute degree distribution and clustering coefficient of a network

netstat -i:../as20graph.txt -p:dC
