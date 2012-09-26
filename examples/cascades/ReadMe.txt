========================================================================
    Cascades
========================================================================

The application implements a simple SI (susceptible--infected) model and 
measures structural properties of cascades (propagation trees). The program
measure how the cascade properties (like, number of nodes, edge and depth)
change as a function of amount of missing data (number of random nodes 
removed from the cascade).

For more details and motivation what this code is trying to achive see 
"Correcting for Missing Data in Information Cascades" by E. Sadikov, M. 
Medina, J. Leskovec, H. Garcia-Molina. WSDM, 2011.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input graph (tab separated list of edges) (default:'demo')
   -o:Output file name (default:'demo')
   -b:Infection (i.e., cascade propagation) probability

/////////////////////////////////////////////////////////////////////////////
Usage:

cascades -i:demo -o:demo -b:0.1
