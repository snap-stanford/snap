========================================================================
  SNAP : Stanford Network Analysis Platform
	http://snap.stanford.edu
========================================================================

Stanford Network Analysis Platform (SNAP) is a general purpose, high
performance system for analysis and manipulation of large networks.
SNAP is written in C++ and it scales to massive graphs with hundreds
of millions of nodes and billions of edges.

The examples work under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all". The SnapExamples project files compile all the examples.

Some of applications expect that GnuPlot and GraphViz are installed and
accessible -- paths are in the system PATH variable or they reside in the
working directory.

/////////////////////////////////////////////////////////////////////////////
Code for example applications: http://snap.stanford.edu/snap/download.html

To compile from the command line, execute:
  make all : compiles all sample applications

To compile on Mac OS X, using Xcode:
  1. From the Toolbar, select Scheme (e.g. 'bigclam').
  2. Product -> Build.  (or Cmd + B).
  3. Run executable via the command line; or
     Choose the scheme's executable (Product -> Edit Scheme -> Run -> Info)
     and run: Product -> Run (or Cmd + R). 
     Note: If using Gnuplot, add the PATH to the scheme's environment variables.
     or create symlink to /usr/bin:
     sudo ln -s <gnuplot_dir>/gnuplot /usr/bin/
  For code completion, the "docs" target has been created which includes all
  Snap-related files and example programs.

Description of examples:
  agmfit :
        Detects network communities from a given network by fitting
	AGM to the given network by maximum likelihood estimation.
  agmgen :
	Implements the Affiliation Graph Model (AGM). AGM generates
        a realistic looking graph from the community affiliation of the nodes.
  bigclam :
	Formulates community detection problems into non-negative matrix
	factorization and discovers community membership factors of nodes.
  cascades :
  	Simulates a SI (susceptible-infected) model on a network and computes
	structural properties of cascades.
  centrality :
	Computes node centrality measures for a graph: closeness, eigen,
	degree, betweenness, page rank, hubs and authorities.
  cliques :
	Finds overlapping dense groups of nodes in networks,
	based on the Clique Percolation Method.
  community :
	Implements network community detection algorithms: Girvan-Newman
	and Clauset-Newman-Moore.
  concomp :
	Computes weakly, strongly and biconnected connected components,
	articulation points and bridge edges of a graph.
  forestfire : 
	Generates graphs using the Forest Fire model.
  graphgen : 
	Generates undirected graphs using one of the many SNAP graph generators.
  graphhash : 
	Demonstrates the use of TGHash graph hash table, useful for
	counting frequencies of small subgraphs or information cascades.
  infopath :
        Implements stochastic algorithm for dynamic network inference from
        cascade data, see http://snap.stanford.edu/infopath/.
  kcores :
  	Computes the k-core decomposition of the network and plots
	the number of nodes in a k-core of a graph as a function of k.
  kronem : 
  	Estimates Kronecker graph parameter matrix using EM algorithm.
  kronfit : 
  	Estimates Kronecker graph parameter matrix.
  krongen : 
  	Generates Kronecker graphs.
  magfit :
	Estimates Multiplicative Attribute Graph (MAG) model parameter.
  maggen : 
	Generates Multiplicative Attribute Graphs (MAG).
  mkdatasets :
	Demonstrates how to load different kinds of networks in various
	network formats and how to compute various statistics of the network.
  motifs : 
  	Counts the number of occurrence of every possible subgraph on K nodes 
  	in the network.
  ncpplot : 
	Plots the Network Community Profile (NCP).
  netevol :
  	Computes properties of an evolving network, like evolution of 
  	diameter, densification power law, degree distribution, etc.
  netinf :
	Implements netinf algorithm for network inference from
	cascade data, see http://snap.stanford.edu/netinf.
  netstat :
  	Computes statistical properties of a static network, like degree
	distribution, hop plot, clustering coefficient, distribution of sizes
	of connected components, spectral properties of graph adjacency
	matrix, etc.
  testgraph :
	Demonstrates some of the basic SNAP functionality.

/////////////////////////////////////////////////////////////////////////////
SNAP documentation:
  http://snap.stanford.edu/snap/doc.html

