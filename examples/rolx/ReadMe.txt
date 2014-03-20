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

///////////////////////////////////////////////////////////////////////////////
Parameters:
Threshold: used as the converge condition in the non-negative factorization. In
the factorization, it maximize f=sum_i(sum_u(V_iu*log((W*H)_iu-(W*H)_iu))). We
use "abs((new_f - old_f)/old_f) < Threshold" as the converge condition. This
threshold need to be carefully tuned according the to dataset size. We use 
Threshold = 1e-6 for the enron-email dataset. 

///////////////////////////////////////////////////////////////////////////////
Usage:
./prototype <dataset file> <min roles> <max roles>
<dataset file> is the graph file
<min roles> and <max roles> is the minimum number of roles and the maximum 
number of roles. The rolx will try on all the role numbers in the interval
[min roles, max roles] and find the best one.

///////////////////////////////////////////////////////////////////////////////
Design consideration:
1. Egonet features: 
In the paper "Egonet features include the number of within-egonet edges, as 
well as the number of edges entering and leaving the egonet. In our 
implementation, we choose to make the number of edges within the egonet to be 
one column of features, and the number of edges entering and leaving the 
egonet to be another column of features.

2. Recursive feature generation:
Once a feature is pruned, we do not use it to generate new features in the next
iteration. This is a time-efficient choice. A conservative method is to use all
features to generate the next round features. There exists cases where a pruned
feature can generate new recursive features which are not correlated with any
old features. In our time-efficient method, these useful features will never be
generate. So we might omit some useful features. However, using all features to
generate next round features will cause exponential increasing complexity. In a
large scale graph, this is unacceptable.

3. Pruning features:
We only use those remained features to build the s-friend graph. Another choice
is using all features to build the s-friend graph. If feature A is round-one, 
feature B is round-two and feature C is round-three. At round 2, A and B are
friend so B is pruned. At round 3, C is friend of B but not a friend of A. In 
our method, we do not use B in the round 3 s-friend graph, so C cannot link A
and will not be pruned. In the alternate method, B will be used in the round-3
s-friend graph, so A-B-C will be in on connected component, C will be pruned.
Our choice is based on the consideration of eliminate the s-friend chain, more
conservative on feature pruning (we are not conservative in feature generation,
so this part is a neutralization for that part) and more efficient (using less
features in the s-friend graph).