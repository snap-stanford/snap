========================================================================
    Network evolution
========================================================================

Computes how various statistical properties of a network change over time.
The program loads a sequence of snapshots and produces plots of densification
power law, shrining diameter, fraction of nodes in largest connected component
over time and similar.

Depending on the platform (Windows or Linux) you need to edit the Makefile.
Use 'make opt' to compile the optimized (fast) version of the code.

/////////////////////////////////////////////////////////////////////////////
Parameters:

   -i:Input graphs (each file is a graph snapshot, or use "DEMO") (default:'graph*.txt')
   -o:Output file name prefix (default:'over-time')
   -t:Description (default:'')
   -s:How much statistics to calculate?
    1:basic, 2:degrees, 3:no-diameter, 4:no-distributions, 5:no-svd, 6:all-statistics (default:6)
    
Generally -s:1 is the fastest (computes the least statistics), while -s:6 
takes longest to run but calculates all the statistics.

/////////////////////////////////////////////////////////////////////////////
Usage:

Compute the evolution of a ForestFire graph model:

netstat -i:DEMO
