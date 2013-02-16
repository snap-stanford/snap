========================================================================
    Community-Affiliation Graph Model 
========================================================================

The example implements the community-Affiliation Graph Model (AGM). 
AGM generates a realistic looking graph given a bipartite graph which
describes affiliations of nodes to communities.

The community-Affiliation Graph Model is described in the following 
two papers:
J. Yang and J. Leskovec, Structure and Overlaps of Communities in 
Networks, SNAKDD '12.
J. Yang and J. Leskovec, Community-Affiliation Graph Model for 
Overlapping Community Detection, ICDM '12.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

////////////////////////////////////////////////////////////////////////
Parameters:
   -i: Community affiliation data (one group per line). 
     Use 'DEMO' to consider a simple case where Nodes 0-24 belong to 
     first community, and nodes 15-39 belong to the second community.
     Also check 'community_affilications.txt' for an example.
   -o: Output filename prefix (The edges of the generated graph).
   -a: Power-law coefficient of the distribution of edge probability 
     inside each community.
   -c: Scaling constant for the edge probability.

////////////////////////////////////////////////////////////////////////
Usage:

Generate an AGM graph using 'DEMO' community affiliations:
agmgen -i:DEMO -a:0.6 -c:1.3 -o:agm_demo

Generate an AGM graph using 'community_affiliations.txt':
agmgen -i:community_affiliations.txt -a:0.6 -c:1.3 -o:agm_test
