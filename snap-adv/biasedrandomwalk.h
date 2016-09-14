#ifndef RAND_WALK_H
#define RAND_WALK_H

typedef TNodeEDatNet<TIntIntVFltVPrH, TFlt> TWNet;
typedef TPt<TWNet> PWNet;

///Preprocesses transition probabilities for random walks. Has to be called once before SimulateWalk calls
void PreprocessTransitionProbs(PWNet& InNet, double& ParamP, double& ParamQ, bool& verbose);

///Simulates one walk and writes it into Walk vector
void SimulateWalk(PWNet& InNet, int64 StartNId, int& WalkLen, TRnd& Rnd, TIntV& Walk);

//Predicts approximate memory required for preprocessing the graph
int64 PredictMemoryRequirements(PWNet& InNet);

#endif //RAND_WALK_H
