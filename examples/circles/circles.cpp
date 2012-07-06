#include "stdafx.h"
#include "circles.h"

TGraphAttributes::TGraphAttributes(PUNGraph G, const char* nodeFeaturePath) :
  G(G) {
  // Read node features for the graph
  FILE* f = fopen(nodeFeaturePath, "r");
  int nNodes;
  int nF;
  fscanf(f, "%d %d", &nNodes, &nF);
  nFeatures = nF;

  for (int i = 0; i < nNodes; i++) {
    int nid;
    fscanf(f, "%d", &nid);
    if (not G->IsNode(nid)) {
      printf("Warning: %d is not a node in G.\n", nid);
    }
    TInt kv = nodeFeatures.AddKey(nid);
    for (int x = 0; x < nFeatures; x++) {
      int z = 0;
      fscanf(f, "%d", &z);
      if (z) {
        nodeFeatures[kv].AddDat(x) = z;
      }
    }
    if (G->IsNode(nid)) {
      nodeIds.Add(nid);
    }
  }
  fclose(f);

  // Construct the features encoding the difference between node attributes.
  for (int i = 0; i < nodeIds.Len(); i++) {
    TInt ni = nodeIds[i];
    for (int j = i + 1; j < nodeIds.Len(); j++) {
      TInt nj = nodeIds[j];
      TInt kv = edgeFeatures.AddKey(TIntPr(ni, nj));
      for (THashKeyDatI<TInt, TInt> it = nodeFeatures.GetDat(ni).BegI(); not it.IsEnd(); it++) {
        TInt k = it.GetKey();
        TInt diff = 0;
        if (nodeFeatures.GetDat(nj).IsKey(k)) {
          diff = abs(it.GetDat() - nodeFeatures.GetDat(nj).GetDat(k));
        } else {
          diff = abs(it.GetDat());
        }
        if (diff) {
          edgeFeatures[kv].AddDat(k) = diff;
        }
      }
      for (THashKeyDatI<TInt, TInt> it = nodeFeatures.GetDat(nj).BegI(); not it.IsEnd(); it++) {
        TInt k = it.GetKey();
        TInt diff = 0;
        if (nodeFeatures.GetDat(ni).IsKey(k)) {
          diff = abs(it.GetDat() - nodeFeatures.GetDat(ni).GetDat(k));
        } else {
          diff = abs(it.GetDat());
        }
        if (diff) {
          edgeFeatures[kv].AddDat(k) = diff;
        }
      }
    }
  }
}

/** Train the model to predict K clusters */
void TCluster::train(TInt outerReps, TInt gradientReps, TInt mcmcReps) {
  // Learning rate
  TFlt increment = 1.0 / (1.0 * gd->nodeIds.Len() * gd->nodeIds.Len());
  TRnd t;

  for (int outerRep = 0; outerRep < outerReps; outerRep++) {
    // If it's the first iteration or the solution is degenerate, randomly initialize the weights and clusters
    for (int k = 0; k < K; k++)
      if (outerRep == 0 or chat[k].Empty() or chat[k].Len()
          == gd->nodeIds.Len()) {
        chat[k].Clr();
        for (int i = 0; i < gd->nodeIds.Len(); i++)
          if (t.GetUniDevInt(2) == 0) {
            chat[k].AddKey(i);
          }
        for (int i = 0; i < gd->nFeatures; i++)
          theta[k * gd->nFeatures + i] = 0;
        // Just set a single feature to 1 as a random initialization.
        theta[k * gd->nFeatures + t.GetUniDevInt(gd->nFeatures)] = 1.0;
        theta[k * gd->nFeatures] = 1;
      }

    for (int k = 0; k < K; k++)
      chat[k] = mcmc(k, mcmcReps);
    TFlt ll_prev = loglikelihood();

    // Perform gradient ascent
    TFlt ll = 0;
    for (int gradientRep = 0; gradientRep < gradientReps; gradientRep++) {
      dl();
      for (int i = 0; i < K * gd->nFeatures; i++) {
        theta[i] += increment * dldt[i];
      }
      printf(".");
      fflush( stdout);
      ll = loglikelihood();

      // If we reduced the objective, undo the update and stop.
      if (ll < ll_prev) {
        for (int i = 0; i < K * gd->nFeatures; i++)
          theta[i] -= increment * dldt[i];
        ll = ll_prev;
        break;
      }
      ll_prev = ll;
    }
    float llf = ll;
    printf("\nIteration %d, ll = %f\n", outerRep + 1, llf);
  }
}

/** Inner product for sparse features */
TFlt inp(TIntIntH& feature, TFlt* parameter) {
  TFlt res = 0;
  for (THashKeyDatI<TInt, TInt> it = feature.BegI(); not it.IsEnd(); it++)
    res += it.GetDat() * parameter[it.GetKey()];
  return res;
}

/** Optimize the cluster assignments for the k'th cluster */
TIntSet TCluster::mcmc(TInt k, TInt mcmcReps) {
  TRnd t;

  THash<TInt, TFlt> mc0;
  THash<TInt, TFlt> mc1;

  TVec<TInt> newLabel;
  int csize = 0;
  for (int i = 0; i < gd->nodeIds.Len(); i++) {
    if (chat[k].IsKey(gd->nodeIds[i])) {
      newLabel.Add(0);
    } else {
      newLabel.Add(1);
    }
    if (chat[k].IsKey(gd->nodeIds[i])) {
      csize++;
    }
  }
  // Compute edge log-probabilities.
  for (THashKeyDatI<TIntPr, TIntIntH> it = gd->edgeFeatures.BegI(); not it.IsEnd(); it++) {
    TIntPr e = it.GetKey();
    TInt kv = gd->edgeFeatures.GetKeyId(e);
    TInt e1 = e.Val1;
    TInt e2 = e.Val2;
    TBool exists = gd->G->IsEdge(e1, e2);
    TFlt inp_ = inp(it.GetDat(), theta + k * gd->nFeatures);
    TFlt other_ = 0;
    for (int l = 0; l < K; l++) {
      if (l == k) {
        continue;
      }
      TFlt d = (chat[l].IsKey(e1) and chat[l].IsKey(e2)) ? 1 : -1;
      other_ += d * inp(it.GetDat(), theta + l * gd->nFeatures);
    }

    TFlt c0;
    TFlt c1;

    if (exists) {
      c0 = -other_ + inp_ + log(1 + exp(other_ - inp_));
      c1 = -other_ - inp_ + log(1 + exp(other_ + inp_));
    } else {
      c0 = log(1 + exp(other_ - inp_));
      c1 = log(1 + exp(other_ + inp_));
    }

    mc0.AddDat(kv) = -c0;
    mc1.AddDat(kv) = -c1;
  }

  // Run MCMC using precomputed probabilities
  TFlt T1 = 1.0; // Initial temperature
  for (int r = 2; r < mcmcReps + 2; r++) {
    TFlt T = T1 / log(r);
    for (int n = 0; n < gd->nodeIds.Len(); n++) {
      TFlt l0 = 0;
      TFlt l1 = 0;
      for (int np = 0; np < gd->nodeIds.Len(); np++) {
        if (n == np) {
          continue;
        }
        TIntPr ed(gd->nodeIds[n], gd->nodeIds[np]);
        if (ed.Val1 > ed.Val2) {
          ed = TIntPr(ed.Val2, ed.Val1);
        }
        TInt kv = gd->edgeFeatures.GetKeyId(ed);
        TFlt m0 = mc0.GetDat(kv);
        if (newLabel[np] == 0) {
          l0 += m0;
          l1 += m0;
        } else {
          l0 += m0;
          l1 += mc1.GetDat(kv);
        }
      }
      TFlt frac = exp(l1 - l0);
      TFlt prob = pow(frac, 1.0 / T);
      if (t.GetUniDev() < prob) {
        newLabel[n] = 1;
      } else {
        newLabel[n] = 0;
      }
    }
  }

  TIntSet res;
  for (int i = 0; i < gd->nodeIds.Len(); i++)
    if (newLabel[i]) {
      res.AddKey(gd->nodeIds[i]);
    }

  return res;
}

/** Update partial derivatives of log-likelihood */
void TCluster::dl(void) {
  for (int i = 0; i < K * gd->nFeatures; i++) {
    if (theta[i] > 0) {
      dldt[i] = -lambda * theta[i];
    } else {
      dldt[i] = lambda * theta[i];
    }
  }

  for (THashKeyDatI<TIntPr, TIntIntH> it = gd->edgeFeatures.BegI(); not it.IsEnd(); it++) {
    TFlt inp_ = 0;
    TIntPr e = it.GetKey();
    TInt e1 = e.Val1;
    TInt e2 = e.Val2;
    TBool exists = gd->G->IsEdge(e1, e2);
    for (int k = 0; k < K; k++) {
      TFlt d = chat[k].IsKey(e1) and chat[k].IsKey(e2) ? 1 : -1;
      inp_ += d * inp(it.GetDat(), theta + k * gd->nFeatures);
    }
    TFlt expinp = exp(inp_);
    TFlt q = expinp / (1 + expinp);
    if (q != q) {
      q = 1; // Test for nan in case of overflow.
    }

    for (int k = 0; k < K; k++) {
      TBool d_ = chat[k].IsKey(e1) and chat[k].IsKey(e2);
      TFlt d = d_ ? 1 : -1;
      for (THashKeyDatI<TInt, TInt> itf = it.GetDat().BegI(); not itf.IsEnd(); itf++) {
        TInt i = itf.GetKey();
        TInt f = itf.GetDat();
        if (exists) {
          dldt[k * gd->nFeatures + i] += d * f;
        }
        dldt[k * gd->nFeatures + i] += -d * f * q;
      }
    }
  }
}

/** Compute the log-likelihood of a parameter vector and cluster assignments */
TFlt TCluster::loglikelihood(void) {
  TFlt ll = 0;
  for (THashKeyDatI<TIntPr, TIntIntH> it = gd->edgeFeatures.BegI(); not it.IsEnd(); it++) {
    TFlt inp_ = 0;
    TIntPr e = it.GetKey();
    TInt e1 = e.Val1;
    TInt e2 = e.Val2;
    TBool exists = gd->G->IsEdge(e1, e2);

    for (int k = 0; k < K; k++) {
      TFlt d = chat[k].IsKey(e1) and chat[k].IsKey(e2) ? 1 : -1;
      inp_ += d * inp(it.GetDat(), theta + k * gd->nFeatures);
    }
    if (exists) {
      ll += inp_;
    }
    TFlt ll_ = log(1 + exp(inp_));
    ll += -ll_;
  }

  if (ll != ll) {
    printf("ll isnan\n");
    exit(1);
  }
  return ll;
}

int main(int argc, char** argv) {
  // Number of circles and regularization constant
  TInt K = 4;
  TFlt lambda = 1;
  
  // Load an example graph
  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>("fb1.edges");
  // Load attributes for that graph
  PGraphAttributes PGA = new TGraphAttributes(Graph, "fb1.features");
  // Predict K circles
  PCluster PC = new TCluster(PGA, K, lambda);
  // Train for 50 iterations of coordinate ascent, with 100 iterations of gradient ascent, and 100 iterations of MCMC
  PC->train(50, 100, 100);
  // Get the predicted circles
  TVec<TIntSet> circles = PC->getCircles();
  for (int k = 0; k < K; k ++) {
    printf("Circle %d:", k + 1);
    for (THashSetKeyI<TInt> it = circles[k].BegI(); it != circles[k].EndI(); it ++) {
      int c = it.GetKey();
      printf(" %d", c);
    }
    printf("\n");
  }

  return 0;
}
