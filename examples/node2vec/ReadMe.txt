========================================================================
    Node2vec
========================================================================

node2vec is an algorithmic framework for representational learning on graphs. Given any graph, it can learn continuous feature representations for the nodes, which can then be used for various downstream machine learning tasks. 

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////

Parameters:
Input graph path (-i:)
Output graph path (-o:)
Number of dimensions. Default is 128 (-d:)
Length of walk per source. Default is 80 (-l:)
Number of walks per source. Default is 10 (-r:)
Context size for optimization. Default is 10 (-k:)
Number of epochs in SGD. Default is 1 (-e:)
Return hyperparameter. Default is 1 (-p:)
Inout hyperparameter. Default is 1 (-q:)
Verbose output. (-v)
Graph is directed. (-dr)
Graph is weighted. (-w)
Output random walks instead of embeddings. (-ow)

/////////////////////////////////////////////////////////////////////////////

Usage:
./node2vec -i:graph/karate.edgelist -o:emb/karate.emb -l:3 -d:24 -p:0.3 -dr -v
