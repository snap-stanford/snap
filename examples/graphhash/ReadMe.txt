========================================================================
    Graph hash
========================================================================

The code demonstrates the use of TGHash graph hash table. Implementation 
is in snap-core/ghash.h

The graph hash table is a hash table where keys are (small) directed
graphs. The class is useful for counting frequencies of small subgraphs
or information cascades. For small graphs with less than MxIsoCheck
nodes the class performs exact isomorphism checking. For graphs with
less than MxSvdGraph nodes the class performs approximate isomorphism
checking by comparing a numeric SVD-based signatures of two graphs. For
graphs with more than MxSvdGraph nodes thec class performs approximate
isorphism checking by comparing only the signature based on simple graph
statistics. For hashing trees (tree is encoded as a directed graph
where children point to the parent) the class always performs exact
isomorphism testing.

The example generates a number of small random directed graphs and
counts their frequencies.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
  There are no input parameters.

/////////////////////////////////////////////////////////////////////////////
Usage:

graphhash
