========================================================================
    Affiliation Graph Model 
========================================================================

The example implements the Affiliation Graph Model (AGM). 
AGM generates a realistic looking graph from the community affiliation
of the nodes.

The Affiliation Graph Model is described in J. Yang and J. Leskovec,
Real Communities in Real Networks: Structure and a Model.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Group affiliation data (one group per line). 
   ('DEMO' uses a simple case where Node 0~24 belongs to one community, and Node 15~39 belongs to the other community)
   -o:Output file prefix (the edges of the generated graph). (default:'agm')
   -a:Power-law coefficient of the edge probability inside each community
   -c:Scaling constant for the edge probability

/////////////////////////////////////////////////////////////////////////////
Usage:

Generate an AGM graph from the 'DEMO' case

agmgen -a:0.6 -c:1.3 -o:agm_demo
