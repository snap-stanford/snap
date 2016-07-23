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
  make all	# compiles all example applications

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

A description of examples is provided in snap/ReadMe.txt.

/////////////////////////////////////////////////////////////////////////////
SNAP documentation:
  http://snap.stanford.edu/snap/doc.html

