========================================================================
    MAGFit: estimate MAG model parameter
========================================================================

MAGFit is a fast and scalable algorithm for fitting the Multiplicative 
Attribute Graph (MAG) model to large real networks. A naive approach to
fitting would take exponential time. In contrast, MAGFit takes O(n(log n)^2) 
time where n represents the number of nodes in the given network.

MAGFit assumes that each node attribute follows a Bernoulli distribution
so that each affinity matrix is 2 by 2. Despite of the binary node attribute
assumption, the MAG model can generate networks that mimic the properties of 
target networks. Therefore, MAGFit aims to find the Bernoulli parameter for
each node attribute and the affinity matrix associated wit it.

For more information about the procedure see:
  Modeling Social Networks with Node Attributes using the 
  Multiplicative Attribute Graph Model
  Myunghwan Kim and Jure Leskovec

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input graph file name (default:'../as20graph.txt')
   -pi:Initial parameter config file name (default:'init.config')
   -s:Random seed (0 - time seed) (default:1)
   -lrn:Learning rate for Theta parameter (default:0.0001)
   -lambda:Regularization coefficient (default:0.1)
   -max:Max gradient step (default:0.02)
   -ri:Reinitialization rate (default:0.1)
   -o:Output file name (default:'result')
   -ns:# of EM iterations (default:15)
   -es:# of iterations in E-step (default:5)
   -ms:# of iterations in M-step (default:5)
   -debug:Turns on the debugging option (default:'F')
   -fe:Turns on the fast E-step (default:'T')
   -fm:Turns on the fast M-step (default:'T')

/////////////////////////////////////////////////////////////////////////////
Usage:

Estimate the MAG model parapmeter for the Autonomous Systems network
using init.config as the initial configuration file with 40 EM iterations.
Each line of the configuration file has the form of "Bernoulli parameter & affinity matrix".
Therefore, the number of lines indicates the number of node attributes to fit.

magfit -i:../as20graph.txt -pi:init.config -ns:40

init.config
0.4 & 0.8 0.4;0.4 0.2
0.4 & 0.8 0.3;0.3 0.2
0.4 & 0.8 0.2;0.2 0.2
0.4 & 0.8 0.5;0.5 0.2
0.4 & 0.8 0.4;0.4 0.1
0.4 & 0.8 0.3;0.3 0.1
0.4 & 0.8 0.2;0.2 0.1
0.4 & 0.8 0.5;0.5 0.1
0.4 & 0.9 0.2;0.2 0.9
0.4 & 0.9 0.2;0.2 0.9
0.4 & 0.9 0.2;0.2 0.9
