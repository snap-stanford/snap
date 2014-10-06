========================================================================
  Contributions to SNAP : Stanford Network Analysis Platform
	http://snap.stanford.edu
========================================================================

This directory contains contributions to SNAP from other institutions.
Each contributed project or a set of related projects is in its own
directory, named <institution>-<project>.

To contribute to SNAP, perform the following steps:
- fork the SNAP repository on github.com
- create a new directory in contrib, named <institution>-<project>
- add your code and sample inputs to the <institution>-<project> directory
- create Makefile
- test your project
- create a ReadMe.txt file
- send us a pull request on github.com

We recommend that your project contains the source code and data that is
needed to execute it, a Makefile to build the project(s), and a ReadMe.txt
file with basic information about the project and instructions on how to
execute the programs. If data files are large, over 100KB, then place them
for download on the Web and put a link in the ReadMe.txt file.

Split the source code into a main program and a set of libraries that
implement your algorithms. A separate library file makes it easier to
include your algorithms into multiple programs. The main program
demonstrates how the library is being used.

