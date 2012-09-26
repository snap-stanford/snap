========================================================================
    Graph Generators
========================================================================

Generates a undirected graph using one of many graph generators 
implemented in SNAP :
  - Complete graph. Required parameters: n (number of nodes)
  - Star graph. Required parameters: n (number of nodes)
  - 2D Grid. Required parameters: n (number of rows), m (number of columns)
  - Erdos-Renyi (G_nm). Required parameters: n (number of nodes), m (number of edges)
  - Random k-regular graph. Required parameters: n (number of nodes), k (degree of every node)
  - Albert-Barabasi Preferential Attachment. Required parameters: n (number of nodes), k (edge s created by each new node)
  - Random Power-Law graph. Required parameters: n (number of nodes), p (power-law degree exponent)
  - Copying model by Kleinberg et al. Required parameters: n (number of nodes), p (copying probability Beta)
  - Small-world model. Required parameters: n (number of nodes), k (each node is connected to k nearest neighbors in ring topology), p (rewiring probability)

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -o:Output graph filename (default:'')
   -g:Which generator to use:
        f: Complete graph. Required parameters: n (number of nodes)
        s: Star graph. Required parameters: n (number of nodes)
        2: 2D Grid. Required parameters: n (number of rows), m (number of columns)
        e: Erdos-Renyi (G_nm). Required parameters: n (number of nodes), m (number of edges)
        k: Random k-regular graph. Required parameters: n (number of nodes), k (degree of every node)
        b: Albert-Barabasi Preferential Attachment. Required parameters: n (number of nodes), k (edges created by each new node)
        p: Random Power-Law graph. Required parameters: n (number of nodes), p (power-law degree exponent)
        c: Copying model by Kleinberg et al. Required parameters: n (number of nodes), p (copying probability Beta)
        w: Small-world model. Required parameters: n (number of nodes), k (each node is connected to k nearest neighbors in ring topology), p (rewiring probability)
   -n:Number of nodes (default:1000)
   -m:Number of edges (default:5000)
   -p:Probability/Degree-exponent (default:0.1)
   -k:Degree (default:3)
/////////////////////////////////////////////////////////////////////////////
Usage:

Generate Erdos-Renyi graph on 1000 nodes and 5000 edges:

graphgen -g:e -n:1000 -m:5000 -o:graph.txt

