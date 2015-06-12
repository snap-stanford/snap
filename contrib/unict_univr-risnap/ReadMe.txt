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

To compile the source code and build an executable for RI Snap and RI-DS Snap
as well use "make all". To clean the files execute "make clean".


////////////////////////////////////////
Usage:

risnap [iso ind mono] reference query

risnapds [ind ind mono] reference query


===============================================================================
    Contacts
===============================================================================

For software sources, databases, helps and bugs please send an email to
Rosalba Giugno giugno@dmi.unict.it, Vincenzo Bonnici vincenzo.bonnici@univr.it
and Davide Stefano Sardina dsardina@dmi.unict.it
