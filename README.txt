========================================================================
  SNAP : Stanford Network Analysis library
	http://snap.stanford.edu
========================================================================

SNAP is a general purpose graph mining and modeling library. The library
is written in C++ and it scales to massive graphs with billions of nodes,
and tens of billions of edges.

/////////////////////////////////////////////////////////////////////////////
Directory structure: http://snap.stanford.edu/snap/description.html

  snap-core: 
        the SNAP graph library (more info below)
  snap-adv: 
        advanced SNAP components, not in the core, but used by examples
  snap-exp:
        experimental SNAP components, still in development
  glib-core: 
        contains the library that provides basic data structures (vectors,
        strings, hash tables), infrastructure (serialization, xml and html
        parsing), interface to GnuPlot and linear algebra (SVD).
  examples:
        sample applications and examples of use. With every .cpp file there
        is a corresponding .txt readme file for application description.
  tutorials:
        simple programs, demonstrating use of various classes
  test:
        unit tests for various classes
  doxygen:
        SNAP reference manuals

Code compiles under Windows (Microsoft Visual Studio, CygWin with gcc) and
Linux and Mac (gcc). Use the SnapExamples.vcproj or the provided Makefile.

Some of applications expect that GnuPlot and GraphViz are installed and
accessible -- paths are in the system PATH variable or they reside in the
working directory.

/////////////////////////////////////////////////////////////////////////////
Example applications: http://snap.stanford.edu/snap/description.html

Execute:
  cd examples
  make all  : compiles all sample applications
  make clean: cleans up all the examples directories
  
Examples:
  agmgen :
        Implements the Affiliation Graph Model (AGM). AGM generates a
        realistic looking graph from the community affiliation of the nodes.
  cascades :
        Simulates a SI (susceptible-infected) model on a network and computes
        structural properties of cascades.
  centrality :
        Computes node centrality measures for a graph: closeness, eigen,
        degree, betweenness, page rank, hubs and authorities.
  cliques :
        Finds overlapping dense groups of nodes in networks, based on
        the Clique Percolation Method.
  community :
        Implements network community detection algorithms: Girvan-Newman and
        Clauset-Newman-Moore.
  concomp :
        Computes weakly, strongly and biconnected connected components,
        articulation points and bridge edges of a graph.
  forestfire : 
        Generates graphs using the Forest Fire model.
  graphhen : 
        Generates undirected graphs using one of the many SNAP graph generators.
  kcores :
        Plots the number of nodes in a k-core of a graph as a function of k.
  kronem : 
        Estimates Kronecker graph parameter matrix using EM algorithm.
  kronfit : 
        Estimates Kronecker graph parameter matrix
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
        Counts the number of occurence of every possible subgraph on K nodes 
        in the network.
  ncpplot : 
        Plots the Network Community Profile (NCP).
  netevol :
        Computes properties of an evolving network, like evolution of 
        diameter, densification power law, degree distribution, etc.
  netstat :
        Computes structural properties of a static network, like degree
        distribution, hop plot, clustering coefficient, distribution of sizes
        of connected components, spectral properties of graph adjacency
        matrix, etc.
  testgraph :
        Demonstrates some of the basic SNAP functionality.

/////////////////////////////////////////////////////////////////////////////
SNAP Introduction: http://snap.stanford.edu/snap/doc.html

The library defines Graphs (nodes and edges) and Networks (graph with data
associated with nodes and edges).

Graph types:
  TNGraph : 
  	directed graph (single directed edge between a pair of nodes)
  TNEGraph : 
  	directed multi-graph (any number of directed edges between a pair of
	nodes)
  TUNGraph : 
  	undirected graph (single undirected edge between a pair of nodes)

Network types:
  TNodeNet<TNodeData> : 
  	like TNGraph but with TNodeData object for each node
  TNodeEdgeNet<TNodeData, TEdgeData> : 
  	like TNEGraph but with TNodeData object for each node and TEdgeData
	object for each edge
  TBigNet<TNodeData> : 
  	memory efficient implementation of TNodeNet (avoids memory
	fragmentation)

/////////////////////////////////////////////////////////////////////////////
SNAP files: http://snap.stanford.edu/snap/description.html

snap-core:
  alg.h : basic algorithms for manipulating graphs
  anf.h : Approximate Neighborhood Function for measuring graph diameter. 
  	Avoids node sampling and scales to large graphs
  bfsdfs.h
  bignet.h : TBigNet -- memory efficient implementation of TNodeNet (avoids 
  	memory fragmentation)
  centr.h : node centrality (closeness, eigen, degree, betweenness, page rank)
  cmty.h
  cncom.h : extracting connected components
  ff.h : Forest Fire model (see our KDD '05 paper)
  gbase.h
  ggen.h : basic graph generation models
  ghash.h : hash table where key is a graph. Used for counting graphs.
  gio.h
  graph.h : graphs (TNGraph, TNEGraph)
  gstat.h : calculates various statistics of graphs
  gsvd.h : spectral analysis of graphs (singular value decomposition)
  gviz.h : interface to GraphViz for plotting small graphs
  kcore.h
  network.h
  Snap.h
  statplot.h
  subgraph.h
  timenet.h : time evolving networks
  triad.h
  util.h

snap-adv:
  agm.h
  cliques.h
  graphcounter.h
  kronecker.h : Kronecker graphs (see PKDD '05 and ICML '07)
  mag.h
  ncp.h
  subgraphenum.h

snap-exp:
  arxiv.h
  dblp.h
  imdbnet.h : IMDB network
  linkpred.h
  memenet.h
  memes.h
  mxdag.h
  signnet.h
  sir.h
  spinn3r.h
  trawling.h
  wgtnet.h
  wikinet.h

NOTFOUND:
  bigalg.h : some algorithms for TBigNet
  bigg.h : TBigGraph -- big disk based graphs that do not fit into memory
  blognet.h : blog network -- posts on blogs linking each other
  casc.h : cascade analysis and counting
  cga.h : Community Guided Attachment (see our KDD '05 paper)
  clust.h : graph clustering and community finding
  emailnet.h : email network
  GMine.h : main file 
  gnet.h : networks (TNodeNet, TNodeEdgeNet)
  gproj.h : graph projections (see our WWW '07 paper)
  layout.h : positions the nodes on the plane for drawing
  plots.h : plots graph properties (degree distributions, etc.)

