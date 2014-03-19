========================================================================
    Rolx: A method to analyse structural roles in the graph
========================================================================

This is an implementation of the rolx algorithm for analysing the structural
roles in the graph. The method includes 3 main black: feature extraction,
implemented in the ExtractFeatures() method, non-negative factorization,
implemented in the CalcNonNegativeFactorization() method, and the model
selection, implemented in the prototype.cpp. More information please refer the
paper: RolX: Structural Role Extraction & Mining in Large Graphs.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
Threshold: used as the converge condition in the non-negative factorization. In
the factorization, it maximize f=sum_i(sum_u(V_iu*log((W*H)_iu-(W*H)_iu))). We
use "abs((new_f - old_f)/old_f) < Threshold" as the converge condition. This
threshold need to be carefully tuned according the to dataset size. We use 
Threshold = 1e-6 for the enron-email dataset. 

/////////////////////////////////////////////////////////////////////////////
Usage:
./prototype <dataset file> <min roles> <max roles>
<dataset file> is the graph file
<min roles> and <max roles> is the minimum number of roles and the maximum 
number of roles. The rolx will try on all the role numbers in the interval
[min roles, max roles] and find the best one.