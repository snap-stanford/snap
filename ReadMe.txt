========================================================================
  SNAP : Stanford Network Analysis Platform
	http://snap.stanford.edu
========================================================================

Stanford Network Analysis Platform (SNAP) is a general purpose, high
performance system for analysis and manipulation of large networks.
SNAP is written in C++ and it scales to massive graphs with hundreds
of millions of nodes and billions of edges.

/////////////////////////////////////////////////////////////////////////////

Directory structure:
  http://snap.stanford.edu/snap/description.html

  snap-core: 
        the core SNAP graph library
  snap-adv: 
        advanced SNAP components, not in the core, but used by examples
  snap-exp:
        experimental SNAP components, still in development
  examples:
        small sample applications that demonstrate SNAP functionality
  tutorials:
        simple programs, demonstrating use of various classes
  glib-core: 
        STL-like library that implements basic data structures, like vectors
        (TVec), hash-tables (THash) and strings (TStr), provides
        serialization and so on
  test:
        unit tests for various classes
  doxygen:
        SNAP reference manuals

Code compiles under Windows (Microsoft Visual Studio, CygWin with gcc) and
Linux and Mac (gcc). Use the SnapExamples*.sln or provided makefiles.

Some of applications expect that GnuPlot and GraphViz are installed and
accessible -- paths are in the system PATH variable or they reside in the
working directory.

/////////////////////////////////////////////////////////////////////////////

Example applications for advanced SNAP functionality are available
in the examples directory and described at:
  http://snap.stanford.edu/snap/description.html.

To compile from the command line, execute:
  make all	# compiles SNAP and all sample applications

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
  cascadegen :
        Identifies cascades in a list of events.
  cascades :
  	Simulates a SI (susceptible-infected) model on a network and computes
	structural properties of cascades.
  centrality :
	Computes node centrality measures for a graph: closeness, eigen,
	degree, betweenness, page rank, hubs and authorities.
  cesna :
        Implements a large scale overlapping community detection method
        for networks with node attributes based on Communities from
        Edge Structure and Node Attributes (CESNA).
  circles :
	Implements a method for identifying users social circles.
  cliques :
	Finds overlapping dense groups of nodes in networks,
	based on the Clique Percolation Method.
  coda :
        Implements a large scale overlapping community detection method 
        based on Communities through Directed Affiliations (CoDA), which
        handles directed as well as undirected networks. The method is able
        to find 2-mode communities where the member nodes form a bipartite
        connectivity structure.
  community :
	Implements network community detection algorithms: Girvan-Newman,
	Clauset-Newman-Moore and Infomap.
  concomp :
	Computes weakly, strongly and biconnected connected components,
	articulation points and bridge edges of a graph.
  flows :
        Computes the maximum network flow in a network.
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
  localmotifcluster :
	Implements a local method for motif-based clustering using MAPPR.
  lshtest :
	Implements locality sensitive hashing.
  magfit :
	Estimates Multiplicative Attribute Graph (MAG) model parameter.
  maggen : 
	Generates Multiplicative Attribute Graphs (MAG).
  mkdatasets :
	Demonstrates how to load different kinds of networks in various
	network formats and how to compute various statistics of the network.
  motifcluster : 
  	Implements a spectral method for motif-based clustering.	
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
  randwalk :
        Computes Personalized PageRank between pairs of nodes.
  rolx :
        Implements the rolx algorithm for analysing the structural
        roles in the graph.
  testgraph :
	Demonstrates some of the basic SNAP functionality.
  temporalmotifs :
	Counts temporal motifs in temporal networks.
  zygote :
        Demonstrates how to use SNAP with the Zygote library, which
        significantly speeds up computations that need to process the
        same large graph many times.

/////////////////////////////////////////////////////////////////////////////

SNAP documentation:
  http://snap.stanford.edu/snap/doc.html

The library defines Graphs (nodes and edges) and Networks (graphs with data
associated with nodes and edges).

Graph types:
  TNGraph : 
  	directed graph (single directed edge between a pair of nodes)
  TUNGraph : 
  	undirected graph (single undirected edge between a pair of nodes)
  TNEGraph : 
  	directed multi-graph (multiple directed edges can exist between
        a pair of nodes)

Network types:
  TNodeNet<TNodeData> : 
  	like TNGraph, but with TNodeData object for each node
  TNodeEDatNet<TNodeData,TEdgeData> :
        like TNGraph, but with TNodeData object for each node and TEdgeData
        object for each edge
  TNodeEdgeNet<TNodeData, TEdgeData> : 
  	like TNEGraph but with TNodeData object for each node and TEdgeData
	object for each edge
  TNEANet :
        like TNEGraph, but with attributes on nodes and edges. The attributes
        are dynamic in that they can be defined at runtime
  TBigNet<TNodeData> : 
  	memory efficient implementation of TNodeNet (avoids memory
	fragmentation)

To generate reference manuals, install doxygen (www.doxygen.org), and execute:
  cd doxygen; make all    # generates user and developer reference manuals

/////////////////////////////////////////////////////////////////////////////

SNAP tutorials

Sample programs demonstrating the use of foundational SNAP classes and
functionality are available in the tutorials directory.

To compile all the tutorials, execute the following command line:
  cd tutorials; make all    # generates all the tutorials

/////////////////////////////////////////////////////////////////////////////

SNAP unit tests

Unit tests are available in the test directory.

To run unit tests, install googletest (code.google.com/p/googletest) and
execute:
  cd test; make run    # compiles and runs all the tests

/////////////////////////////////////////////////////////////////////////////

Description of SNAP files:
  http://snap.stanford.edu/snap/description.html

snap-core:
  alg.h : Simple algorithms like counting node degrees, simple graph
        manipulation (adding/deleting self edges, deleting isolated nodes)
        and testing whether graph is a tree or a star.
  anf.h : Approximate Neighborhood Function: linear time algorithm to
        approximately calculate the diameter of massive graphs.
  bfsdfs.h : Algorithms based on Breath First Search (BFS) and Depth First
        Search (DFS): shortest paths, spanning trees, graph diameter, and
        similar.
  bignet.h : Memory efficient implementation of a network with data on
        nodes. Use when working with very large networks.
  casc.h : Computes cascades from a list of events.
  centr.h : Node centrality measures: closeness, betweenness, PageRank, ...
  cmty.h : Algorithms for network community detection: Modularity,
        Girvan-Newman, Clauset-Newman-Moore.
  cncom.h : Connected components: weakly, strongly and biconnected
        components, articular nodes and bridge edges.
  ff.h : Forest Fire model for generating networks that densify and have
        shrinking diameters.
  flow.h: Maximum flow algorithms.
  gbase.h : Defines flags that are used to identify functionality of graphs.
  ggen.h : Various graph generators: random graphs, copying model,
        preferential attachment, RMAT, configuration model, Small world model.
  ghash.h : Hash table with directed graphs (<tt>TNGraph</tt>) as keys. Uses
        efficient adaptive approximate graph isomorphism testing to scale to
        large graphs. Useful when one wants to count frequencies of various
        small subgraphs or cascades.
  gio.h : Graph input output. Methods for loading and saving various textual
        and XML based graph formats: Pajek, ORA, DynNet, GraphML (GML), 
        Matlab.
  graph.h : Implements graph types TUNGraph, TNGraph and TNEGraph.
  gstat.h : Computes many structural properties of static and evolving networks.
  gsvd.h : Eigen and singular value decomposition of graph adjacency matrix.
  gviz.h : Interface to GraphViz for plotting small graphs.
  kcore.h : K-core decomposition of networks.
  network.h : Implements network types TNodeNet, TNodeEDatNet and TNodeEdgeNet.
  randwalk.h : Computing random walk scores and personalized PageRank
	between pairs of nodes
  Snap.h : Main include file of the library.
  statplot.h : Plots of various structural network properties: clustering,
        degrees, diameter, spectrum, connected components.
  subgraph.h : Extracting subgraphs and converting between different
        graph/network types.
  timenet.h : Temporally evolving networks.
  triad.h : Functions for counting triads (triples of connected nodes in the
        network) and computing clustering coefficient.
  util.h : Utilities to manipulate PDFs, CDFs and CCDFs. Quick and dirty
        string manipulation, URL and domain manipulation routines.

snap-adv:
  agm*.h : Implements the Affiliation Graph Model (AGM).
  cliques.h : Maximal clique detection and Clique Percolation method.
  graphcounter.h : Performs fast graph isomorphism testing to count the
        frequency of topologically distinct sub-graphs.
  kronecker.h : Kronecker Graph generator and KronFit algorithm for
        estimating parameters of Kronecker graphs.
  mag.h : Implements the Multiplicative Attribute Graph (MAG).
  motifcluster.h : Implements motif-based clustering algorithms.
  ncp.h : Network community profile plot. Implements local spectral graph
        partitioning method to efficiently find communities in networks.
  rolx.h : Node role detection.
  subgraphenum.h : Enumerates all connected induced sub-graphs of particular
        size.

snap-exp:
  arxiv.h : Functions for parsing Arxiv data and standardizing author names.
  dblp.h : Parser for XML dump of DBLP data.
  imdbnet.h : Actors-to-movies bipartite network of IMDB.
  mxdag.h  Finds the maximum directed-acyclic subgraph of a given
        directed graph.
  signnet.h : Networks with signed (+1, -1) edges that can denote
        trust/distrust between the nodes of the network.
  sir.h : SIR epidemic model and SIR parameter estimation.
  spinn3r.h : Past parser for loading blog post data from Spinn3r.
  trawling.h : Algorithm of extracting bipartite cliques from the network.
  wgtnet.h : Weighted networks.
  wikinet.h : Networks based on Wikipedia.



