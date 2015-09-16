===============================================================================
    RI Snap, RI-DS Snap
===============================================================================

RI is a general purpose algorithm for one-to-one exact subgraph isomorphism
problem maintaining topological constraints. It is both a C++ library and a
standalone tool, providing developing API and a command line interface, with
no dependencies out of standard GNU C++ library. RI works on Unix and Mac OS X
systems with G++ installed, and it can be compiled under Windows using Gygwin.
Working graphs may be directed, undirected, multigraphs with optional
attributes both on nodes and edges. Customizable features allow user-defined
behaviors for attribute comparisons and the algorithm's flow.

RI aims to provide a better search strategy for the common used backtracking
approach to the subgraph isomorphism problem. It can be integrated with
additional preprocessing steps or it can be used for the verification of
candidate structures coming from data mining, data indexing or other filtering
techniques. RI is able to find graphs isomorphisms, subgraph isomorphisms and
induced subgraph isomorphisms. It is distributed in several versions divide
chiefly in two groups respectively for static or dynamically changing
attributes. All proposed versions are developed taking into account trade-offs
between time and memory requirements. Optional behaviors such as stop at first
encountered match, processing of result matches, type of isomorphism and
additional features may be enabled thanks to high modularity and library's API.

RI-DS is a domains based implementation of RI. Compatibility maps (domains) for
pattern nodes are built before the backtracking step. It avoids redundant
compatibility checks between nodes of pattern graph and target nodes. Checks
regard nodes attributes and neighborhood topology. Additional space to store
compatibility maps as bit vectors is required.


===============================================================================
    Compiling RI Snap
===============================================================================

In order to compile the source code and build the executables for RI Snap and 
RI-DS Snap test use "make all". In order to clean the files execute
"make clean". You will find "risnap" and "risnapds" executables within RiToSnap
and RI_dsToSnap directories, respectively.


  ////////////////////////////////////////
  Usage:

  risnap [iso ind mono] reference query

  risnapds [iso mono] reference query


The sources for command line tools above are in the files risnap.cpp and
risnapds.cpp. In order to use RI and RI-DS directly in your C++ code and call the
appropriate function, see examples in these files.


===============================================================================
    Execute RI Snap example
===============================================================================

In order to test RI Snap, we provide you two simple reference and query
networks. Execute the following command:

  ./risnap [iso ind mono] reference.snap query.snap

or

  ./risnapds [iso mono] reference.snap query.snap

where reference.snap and query.snap contains the following networks of type
TNodeEDatNet<TStr, TStr> (see risnap.cpp, risnapds.cpp file for source code):

	reference.snap		query.snap

	4 Nodes:		4 Nodes
	 A B C Br		 A B C Br
	5 Edges: 		3 Edges
	 (0 1)			 (0 1)
	 (2 1)			 (2 1)
	 (2 3)			 (2 3)
	 (0 3)		
	 (0 2)		


In order to load your own networks in our example use TNodeEDatNet<TStr, TStr>
network type from Snap library. Otherwise you can import directly the library
header from files rinetmatch.h or rids.h and call the Match function (see
risnap.cpp, risnapds.cpp for further references).
If you have to use other annotation types for nodes and edges, please make sure
they have already defined the '==' operator.

===============================================================================
    Acknowledgments
===============================================================================
This software is based on the algorithm presented in Bonnici et al.

We also thank Dott. Andrea Fornaia for all his work on this project.


If you have used any of the RI project software, please cite the following paper:

[1] Vincenzo Bonnici, Rosalba Giugno, Alfredo Pulvirenti, Dennis Shasha and Alfredo Ferro. A subgraph isomorphism algorithm and its application to biochemical data. BMC Bioinformatics 2013, 14(Suppl 7):S13 doi:10.1186/1471-2105-14-S7-S13.


===============================================================================
    Contacts
===============================================================================

For software sources, databases, helps and bugs please send an email to
Rosalba Giugno giugno@dmi.unict.it, Vincenzo Bonnici vincenzo.bonnici@univr.it
and Davide Stefano Sardina dsardina@dmi.unict.it
