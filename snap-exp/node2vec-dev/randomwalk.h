#ifndef RAND_WALK_H
#define RAND_WALK_H

typedef THash<TInt,TPair<TIntV,TFltV> > TIntIntVFltVPrH;
typedef TNodeEDatNet<TIntIntVFltVPrH, TFlt> TWNet;
typedef TPt<TWNet> PWNet;
typedef TPair<TVec<TInt>, TVec<TFlt> > TIntVFltVPr;

void GetNodeAlias(TVec<TFlt>& PTbl, TIntVFltVPr& NTTable);
int AliasDrawInt(TIntVFltVPr& NTTable, TRnd& Rnd);
void PreprocessTransitionProbs(PWNet& InNet, double& ParamP, double& ParamQ);
void SimulateWalk(PWNet& InNet, int StartNId, int& WalkLen, TRnd& Rnd, TIntV& Walk);

#endif //RAND_WALK_H
