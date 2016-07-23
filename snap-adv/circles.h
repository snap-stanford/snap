#pragma once

#include "stdafx.h"

class TGraphAttributes {
public:
  /**
   * @param G the graph on which to run community detection
   * @param nodeFeaturePath the path of the file containing node attributes
   */
  TGraphAttributes(PUNGraph G, const char* nodeFeaturePath, const char* groundtruthPath);
  ~TGraphAttributes() {
  }

  PUNGraph G;
  TInt NFeatures;

  THash<TInt, TIntIntH> NodeFeatures;
  THash<TIntPr, TIntIntH> EdgeFeatures;
  TVec<TInt> NodeIDs;
  TCRef CRef;

  TVec<TIntSet> GroundTruth; // Groundtruth communities
};

typedef TPt<TGraphAttributes> PGraphAttributes;

class TCluster {
public:
  /**
   * @param GraphAttributes attributed graph object with attributes
   * @param K number of communities to detect
   * @param Lambda regularization parameter
   */
  TCluster(PGraphAttributes GraphAttributes, TInt K, TFlt Lambda) :
    GraphAttributes(GraphAttributes), K(K), Lambda(Lambda) {
    Theta = new TFlt[K * GraphAttributes->NFeatures];
    Derivative = new TFlt[K * GraphAttributes->NFeatures];
    for (int k = 0; k < K; k++) {
      for (int f = 0; f < GraphAttributes->NFeatures; f++) {
        Theta[k * GraphAttributes->NFeatures + f] = 0;
        Derivative[k * GraphAttributes->NFeatures + f] = 0;
      }

      // Clusters are initially empty.
      CHat.Add(TIntSet());
    }
  }
  ~TCluster() {
    delete[] Theta;
    delete[] Derivative;
  }

  /**
   * @param OuterReps number of coordinate ascent iterations
   * @param GradientReps number of iterations of gradient ascent
   * @param MCMCReps number of iterations of MCMC
   */
  void Train(TInt OuterReps, TInt GradientReps, TInt MCMCReps);

  /**
   * @return the predicted circles
   */
  TVec<TIntSet> GetCircles(void) {
    return CHat;
  }
  TCRef CRef;

private:
  TFlt* Theta; // Community parameters
  TFlt* Derivative; // Partial derivatives

  TVec<TIntSet> CHat; // Latent community memberships
  PGraphAttributes GraphAttributes; // Graph with attributes

  TInt K;
  TFlt Lambda;

  /**
   * @return the log-likelihood of the current model
   */
  TFlt LogLikelihood();

  /**
   * @param k community index on which to run MCMC
   * @param MCMCReps number of iterations of MCMC
   * @return node ids for the updated community
   */
  TIntSet MCMC(TInt k, TInt MCMCReps);
  void Gradient();
};

typedef TPt<TCluster> PCluster;

enum lossType
{
  zeroOne = 0,
  balancedError = 1,
  fScore = 2
};

/// Compute the loss between a GroundTruth cluster l and a predicted cluster lHat
TFlt Loss(TIntSet& l, TIntSet lHat, int N, int Which)
{
  if (l.Len() == 0) {
    if (lHat.Len() == 0) {
      return 0;
    }
    return 1.0;
  }
  if (lHat.Len() == 0) {
    if (l.Len() == 0) {
      return 0;
    }
    return 1.0;
  }
  TInt TruePositives = 0;
  TInt FalsePositives = 0;
  TInt FalseNegatives = 0;

  TFlt LabelLoss = 0;
  for (THashSetKeyI<TInt> it = l.BegI(); it != l.EndI(); it ++) {
    int c = it.GetKey();
    if (!lHat.IsKey(c)) {
      // false negative
      FalseNegatives ++;
      if (Which == zeroOne) {
        LabelLoss += 1.0/N;
      }
      else if (Which == balancedError) {
        LabelLoss += 0.5/l.Len();
      }
    }
  }

  for (THashSetKeyI<TInt> it = lHat.BegI(); it != lHat.EndI(); it ++) {
    int c = it.GetKey();
    if (!l.IsKey(c)) {
      // false positive
      FalsePositives ++;
      if (Which == zeroOne) {
        LabelLoss += 1.0/N;
      }
      else if (Which == balancedError) {
        LabelLoss += 0.5/(N - l.Len());
      }
    }
    else {
      TruePositives ++;
    }
  }

  if ((lHat.Len() == 0 || TruePositives == 0) && Which == fScore) {
    return 1.0;
  }
  TFlt precision = (1.0*TruePositives)/lHat.Len();
  TFlt recall = (1.0*TruePositives)/l.Len();
  if (Which == fScore) {
    return 1 - 2 * (precision*recall) / (precision + recall);
  }

  return LabelLoss;
}

/*
/// Compute the optimal loss via linear assignment
// Requires code for the munkres algorithm, available at https://github.com/saebyn/munkres-cpp
TFlt TotalLoss(TVec<TIntSet>& Clusters, TVec<TIntSet>& CHat, int N, int Which)
{
  Matrix<double> matrix(Clusters.Len(), CHat.Len());

  for (int i = 0; i < (int) Clusters.Len(); i ++) {
    for (int j = 0; j < (int) CHat.Len(); j ++) {
      matrix(i,j) = Loss(Clusters[i], CHat[j], N, Which);
    }
  }

  Munkres m;
  m.solve(matrix);

  TFlt l = 0;
  for (int i = 0; i < (int) Clusters.Len(); i ++) {
    for (int j = 0; j < (int) CHat.Len(); j ++) {
      if (matrix(i,j) == 0) {
        l += Loss(Clusters[i], CHat[j], N, Which);
      }
    }
  }

  return l / (Clusters.Len() < CHat.Len() ? Clusters.Len() : CHat.Len());
}
*/

TGraphAttributes::TGraphAttributes(PUNGraph G, const char* NodeFeaturePath,
                                   const char* GroundTruthPath) :
  G(G) {
  // Read node Features for the graph
  FILE* f = fopen(NodeFeaturePath, "r");
  int NNodes;
  int nF;
  fscanf(f, "%d %d", &NNodes, &nF);
  NFeatures = nF;

  for (int i = 0; i < NNodes; i++) {
    int nid;
    fscanf(f, "%d", &nid);

    if (!G->IsNode(nid)) {
      printf("Warning: %d is not a node in G.\n", nid);
    }
    TInt kv = NodeFeatures.AddKey(nid);
    for (int x = 0; x < nF; x++) {
      int z = 0;
      fscanf(f, "%d", &z);
      if (z) {
        NodeFeatures[kv].AddDat(x) = z;
      }
    }
    if (G->IsNode(nid)) {
      NodeIDs.Add(nid);
    }
  }
  fclose(f);
  
  f = fopen(GroundTruthPath, "r");
  if (f == NULL) {
    printf("Groundtruth file %s not found.\n", GroundTruthPath);
  }
  else {
    char* CircleName = new char [1000];
    while (fscanf(f, "%s", CircleName) == 1)
    {
      TIntSet Circle;
      while (true) {
        int nid;
        fscanf(f, "%d", &nid);
        Circle.AddKey(nid);
        char c;
        while (true) {          
          c = fgetc(f);
          if (c == '\n') break;
          if (c >= '0' && c <= '9') {
            fseek(f, -1, SEEK_CUR);
            break;
          }
        }
        if (c == '\n') break;
      }
      GroundTruth.Add(Circle);
    }
    delete [] CircleName;
  }
  fclose(f);

  // Construct the Features encoding the difference between node attributes.
  for (int i = 0; i < NodeIDs.Len(); i++) {
    TInt ni = NodeIDs[i];
    for (int j = i + 1; j < NodeIDs.Len(); j++) {
      TInt nj = NodeIDs[j];
      TInt kv = EdgeFeatures.AddKey(TIntPr(ni, nj));
      for (THashKeyDatI<TInt, TInt> it = NodeFeatures.GetDat(ni).BegI();
           !it.IsEnd(); it++) {
        TInt k = it.GetKey();
        TInt diff = 0;
        if (NodeFeatures.GetDat(nj).IsKey(k)) {
          diff = abs(it.GetDat() - NodeFeatures.GetDat(nj).GetDat(k));
        } else {
          diff = abs(it.GetDat());
        }
        if (diff) {
          EdgeFeatures[kv].AddDat(k) = diff;
        }
      }
      for (THashKeyDatI<TInt, TInt> it = NodeFeatures.GetDat(nj).BegI();
           !it.IsEnd(); it++) {
        TInt k = it.GetKey();
        TInt diff = 0;
        if (NodeFeatures.GetDat(ni).IsKey(k)) {
          diff = abs(it.GetDat() - NodeFeatures.GetDat(ni).GetDat(k));
        } else {
          diff = abs(it.GetDat());
        }
        if (diff) {
          EdgeFeatures[kv].AddDat(k) = diff;
        }
      }
    }
  }
}

/// Train the model to predict K Clusters
void TCluster::Train(TInt OuterReps, TInt GradientReps, TInt MCMCReps) {
  // Learning rate
  TFlt Increment = 1.0 / (1.0 * GraphAttributes->NodeIDs.Len() * GraphAttributes->NodeIDs.Len());
  TRnd t;

  for (int OuterRep = 0; OuterRep < OuterReps; OuterRep++) {
    // If it's the first iteration or the solution is degenerate,
    // randomly initialize the weights and Clusters
    for (int k = 0; k < K; k++) {
      if (OuterRep == 0 || CHat[k].Empty() || CHat[k].Len()
          == GraphAttributes->NodeIDs.Len()) {
        CHat[k].Clr();
        for (int i = 0; i < GraphAttributes->NodeIDs.Len(); i++) {
          if (t.GetUniDevInt(2) == 0) {
            CHat[k].AddKey(GraphAttributes->NodeIDs[i]);
          }
        }
        for (int i = 0; i < GraphAttributes->NFeatures; i++) {
          Theta[k * GraphAttributes->NFeatures + i] = 0;
        }
        // Just set a single Feature to 1 as a random initialization.
        Theta[k * GraphAttributes->NFeatures + t.GetUniDevInt(GraphAttributes->NFeatures)] = 1.0;
        Theta[k * GraphAttributes->NFeatures] = 1;
      }
    }
    
    for (int k = 0; k < K; k++) {
      CHat[k] = MCMC(k, MCMCReps);
    }
    TFlt llPrevious = LogLikelihood();

    // Perform gradient ascent
    TFlt ll = 0;
    for (int gradientRep = 0; gradientRep < GradientReps; gradientRep++) {
      Gradient();
      for (int i = 0; i < K * GraphAttributes->NFeatures; i++) {
        Theta[i] += Increment * Derivative[i];
      }
      printf(".");
      fflush( stdout);
      ll = LogLikelihood();

      // If we reduced the objective, undo the update and stop.
      if (ll < llPrevious) {
        for (int i = 0; i < K * GraphAttributes->NFeatures; i++) {
          Theta[i] -= Increment * Derivative[i];
        }
        ll = llPrevious;
        break;
      }
      llPrevious = ll;
    }
    printf("\nIteration %d, ll = %f\n", OuterRep + 1, (double) ll);
  }
}

/// Inner product for sparse features
TFlt Inner(TIntIntH& Feature, TFlt* Parameter) {
  TFlt res = 0;
  for (THashKeyDatI<TInt, TInt> it = Feature.BegI(); !it.IsEnd(); it++) {
    res += it.GetDat() * Parameter[it.GetKey()];
  }
  return res;
}

/// Optimize the cluster assignments for the k'th cluster
TIntSet TCluster::MCMC(TInt k, TInt MCMCReps) {
  TRnd t;

  THash<TInt, TFlt> CostNotIncludeHash;
  THash<TInt, TFlt> CostIncludeHash;

  TVec<TInt> NewLabel;
  int csize = 0;
  for (int i = 0; i < GraphAttributes->NodeIDs.Len(); i++) {
    if (CHat[k].IsKey(GraphAttributes->NodeIDs[i])) {
      NewLabel.Add(0);
    } else {
      NewLabel.Add(1);
    }
    if (CHat[k].IsKey(GraphAttributes->NodeIDs[i])) {
      csize++;
    }
  }
  // Compute edge log-probabilities.
  for (THashKeyDatI<TIntPr, TIntIntH> it = GraphAttributes->EdgeFeatures.BegI();
       !it.IsEnd(); it++) {
    TIntPr e = it.GetKey();
    TInt kv = GraphAttributes->EdgeFeatures.GetKeyId(e);
    TInt Src = e.Val1;
    TInt Dst = e.Val2;

    TBool Exists = GraphAttributes->G->IsEdge(Src, Dst);
    TFlt InnerProduct = Inner(it.GetDat(), Theta + k * GraphAttributes->NFeatures);
    TFlt Other = 0;
    for (int l = 0; l < K; l++) {
      if (l == k) {
        continue;
      }
      TFlt d = (CHat[l].IsKey(Src) && CHat[l].IsKey(Dst)) ? 1 : -1;
      Other += d * Inner(it.GetDat(), Theta + l * GraphAttributes->NFeatures);
    }

    TFlt CostNotInclude;
    TFlt CostInclude;

    if (Exists) {
      CostNotInclude = -Other + InnerProduct + log(1 + exp(Other - InnerProduct));
      CostInclude = -Other - InnerProduct + log(1 + exp(Other + InnerProduct));
    } else {
      CostNotInclude = log(1 + exp(Other - InnerProduct));
      CostInclude = log(1 + exp(Other + InnerProduct));
    }

    CostNotIncludeHash.AddDat(kv) = -CostNotInclude;
    CostIncludeHash.AddDat(kv) = -CostInclude;
  }

  // Run MCMC using precomputed probabilities
  TFlt InitialTemperature = 1.0; // Initial temperature
  for (int r = 2; r < MCMCReps + 2; r++) {
    TFlt Temperature = InitialTemperature / log((double) r);
    for (int n = 0; n < GraphAttributes->NodeIDs.Len(); n++) {
      TFlt l0 = 0;
      TFlt l1 = 0;
      for (int np = 0; np < GraphAttributes->NodeIDs.Len(); np++) {
        if (n == np) {
          continue;
        }
        TIntPr ed(GraphAttributes->NodeIDs[n], GraphAttributes->NodeIDs[np]);
        if (ed.Val1 > ed.Val2) {
          ed = TIntPr(ed.Val2, ed.Val1);
        }
        TInt kv = GraphAttributes->EdgeFeatures.GetKeyId(ed);
        TFlt m0 = CostNotIncludeHash.GetDat(kv);
        if (NewLabel[np] == 0) {
          l0 += m0;
          l1 += m0;
        } else {
          l0 += m0;
          l1 += CostIncludeHash.GetDat(kv);
        }
      }
      TFlt LogLikelihoodDiff = exp(l1 - l0);
      TFlt AcceptProb = pow(LogLikelihoodDiff, 1.0 / Temperature);
      if (t.GetUniDev() < AcceptProb) {
        NewLabel[n] = 1;
      } else {
        NewLabel[n] = 0;
      }
    }
  }

  TIntSet Result;
  for (int i = 0; i < GraphAttributes->NodeIDs.Len(); i++) {
    if (NewLabel[i]) {
      Result.AddKey(GraphAttributes->NodeIDs[i]);
    }
  }

  return Result;
}

/// Update partial derivatives of log-likelihood
void TCluster::Gradient(void) {
  for (int i = 0; i < K * GraphAttributes->NFeatures; i++) {
    if (Theta[i] > 0) {
      Derivative[i] = -Lambda * Theta[i];
    } else {
      Derivative[i] = Lambda * Theta[i];
    }
  }

  for (THashKeyDatI<TIntPr, TIntIntH> it = GraphAttributes->EdgeFeatures.BegI();
       !it.IsEnd(); it++) {
    TFlt InnerProduct = 0;
    TIntPr Edge = it.GetKey();
    TInt Src = Edge.Val1;
    TInt Dst = Edge.Val2;
    TBool Exists = GraphAttributes->G->IsEdge(Src, Dst);
    for (int k = 0; k < K; k++) {
      TFlt d = CHat[k].IsKey(Src) && CHat[k].IsKey(Dst) ? 1 : -1;
      InnerProduct += d * Inner(it.GetDat(), Theta + k * GraphAttributes->NFeatures);
    }
    TFlt expinp = exp(InnerProduct);
    TFlt q = expinp / (1 + expinp);
    if (q != q) {
      q = 1; // Test for nan in case of overflow.
    }

    for (int k = 0; k < K; k++) {
      TBool d_ = CHat[k].IsKey(Src) && CHat[k].IsKey(Dst);
      TFlt d = d_ ? 1 : -1;
      for (THashKeyDatI<TInt, TInt> itf = it.GetDat().BegI();
           !itf.IsEnd(); itf++) {
        TInt i = itf.GetKey();
        TInt f = itf.GetDat();
        if (Exists) {
          Derivative[k * GraphAttributes->NFeatures + i] += d * f;
        }
        Derivative[k * GraphAttributes->NFeatures + i] += -d * f * q;
      }
    }
  }
}

/// Compute the log-likelihood of Parameters and cluster assignments
TFlt TCluster::LogLikelihood(void) {
  TFlt ll = 0;
  for (THashKeyDatI<TIntPr, TIntIntH> it = GraphAttributes->EdgeFeatures.BegI();
       !it.IsEnd(); it++) {
    TFlt InnerProduct = 0;
    TIntPr Edge = it.GetKey();
    TInt Src = Edge.Val1;
    TInt Dst = Edge.Val2;
    TBool Exists = GraphAttributes->G->IsEdge(Src, Dst);

    for (int k = 0; k < K; k++) {
      TFlt d = CHat[k].IsKey(Src) && CHat[k].IsKey(Dst) ? 1 : -1;
      InnerProduct += d * Inner(it.GetDat(), Theta + k * GraphAttributes->NFeatures);
    }
    if (Exists) {
      ll += InnerProduct;
    }
    TFlt ll_ = log(1 + exp(InnerProduct));
    ll += -ll_;
  }

  if (ll != ll) {
    printf("ll isnan\n");
    exit(1);
  }
  return ll;
}
