========================================================================
    CESNA: Communities from Edge Structure and Node Attributes.
========================================================================

The example implements a large scale overlapping community detection method for networks with node attributes
based on Communities from Edge Structure and Node Attributes (CESNA).
This program formulates a generative model between community affiliations, network, and node attributes.
Then the program discovers community membership factors of nodes by maximum likelihood estimation.
In addition to community memberships, the program learns the logistic weight from each community to each node attribute.

User can specify how many communities she would detect, or let the program determine
the number of communities in the network from the topology of the network.
User can also specify the weights for the likelihood terms from the network and the node attributes, and l-1 regularization intensity for learning the logistic weights.

Fitting procedure and the community-Affiliation Graph Model are described in the following paper:
J. Yang, J. McAuley, and J. Leskovec, Community Detection in Networks with Node Attributes, ICDM '13.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -o:Output file prefix (default:'')
   -i:Input edgelist file name (default:'./1912.edges')
   -l:Input file name for node names (Node ID, Node label)  (default:'')
   -a:Input node attribute file name (default:'./1912.nodefeat')
   -n:Input file name for the names of attributes (default:'./1912.nodefeatnames')
   -c:The number of communities to detect (-1: detect automatically) (default:10)
   The following three parameters are for finding the number of communities to detect.
   The program tries nc numbers from mc to xc: by default, it tries 10 values from 3 to 20.
   -mc:Minimum number of communities to try (default:3) 
   -xc:Maximum number of communities to try (default:20)
   -nc:How many trials for the number of communities (default:5)
   -nt:Number of threads for parallelization (default:1) -nt:1 means no parallelization.
   -aw:Weight between the network and attributes. We maximize (1 - aw) P(Network) + aw * P(Attributes)
   -lw:l-1 regularization parameter for learning the logistic weights. 
   
   The following two parameters are for backracking line search described in Convex Optimization, Boyd and Vandenberghe, 2004.
   Refer to the book for the backtracking line search algorithm.
   -sa:Alpha for backtracking line search (default:0.05)
   -sb:Beta for backtracking line search (default:0.3)

/////////////////////////////////////////////////////////////////////////////
Usage:

Detect 200 communities from an a facebook ego network.

cesna -c:10

//////////////////////////////////////////////////////////////////////////////
Output:

<Output prefix>cmtyvv.txt: Community memberships. Each row shows the IDs of nodes that belong to each community. 
<Output prefix>weights.txt: Logistic weight. k-th number for i-th row shows the logistic weight factor between community i and attribute k. If the names of attributes are given, the names are written in the first row.
