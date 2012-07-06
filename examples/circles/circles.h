#pragma once

#include "stdafx.h"

class TGraphAttributes {
public:
  /**
   * @param G the graph on which to run community detection
   * @param nodeFeaturePath the path of the file containing node attributes
   */
  TGraphAttributes(PUNGraph G, const char* nodeFeaturePath);
  ~TGraphAttributes() {
  }

  PUNGraph G;
  TInt nFeatures;

  THash<TInt, TIntIntH> nodeFeatures;
  THash<TIntPr, TIntIntH> edgeFeatures;
  TVec<TInt> nodeIds;
  TCRef CRef;
};

typedef TPt<TGraphAttributes> PGraphAttributes;

class TCluster {
public:
  /**
   * @param gd attributed graph object with attributes
   * @param K number of communities to detect
   * @param lambda regularization parameter
   */
  TCluster(PGraphAttributes gd, TInt K, TFlt lambda) :
    gd(gd), K(K), lambda(lambda) {
    theta = new TFlt[K * gd->nFeatures];
    dldt = new TFlt[K * gd->nFeatures];
    for (int k = 0; k < K; k++) {
      for (int f = 0; f < gd->nFeatures; f++) {
        theta[k * gd->nFeatures + f] = 0;
        dldt[k * gd->nFeatures + f] = 0;
      }

      // Clusters are initially empty.
      chat.Add(TIntSet());
    }
  }
  ~TCluster() {
    delete[] theta;
    delete[] dldt;
  }

  /**
   * @param outerReps number of coordinate ascent iterations
   * @param gradientReps number of iterations of gradient ascent
   * @param mcmcReps number of iterations of MCMC
   */
  void train(TInt outerReps, TInt gradientReps, TInt mcmcReps);

  /**
   * @return the predicted circles
   */
  TVec<TIntSet> getCircles(void) {
    return chat;
  }
  TCRef CRef;
private:
  TFlt* theta; // Community parameters
  TFlt* dldt; // Partial derivatives

  TVec<TIntSet> chat; // Latent community memberships
  PGraphAttributes gd; // Graph with attributes

  TInt K;
  TFlt lambda;

  /**
   * @return the log-likelihood of the current model
   */
  TFlt loglikelihood();

  /**
   * @param k community index on which to run MCMC
   * @param mcmcReps number of iterations of MCMC
   * @return node ids for the updated community
   */
  TIntSet mcmc(TInt k, TInt mcmcReps);
  void dl();
};

typedef TPt<TCluster> PCluster;
