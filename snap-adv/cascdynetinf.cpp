#include "stdafx.h"
#include "cascdynetinf.h"

void TNIBs::LoadCascadesTxt(TSIn& SIn) {
  TStr Line;
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); 
  }
  printf("All nodes read!\n");
  while (!SIn.Eof()) { SIn.GetNextLn(Line); AddCasc(Line, Model); }

  printf("All cascades read!\n");
}

void TNIBs::LoadGroundTruthTxt(TSIn& SIn) {
  bool verbose = false;
  TStr Line;

  Network.Clr(); // clear network (if any)

  // add nodes
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    Network.AddNode(NIdV[0].GetInt(), NIdV[1]);
    if (!IsNodeNm(NIdV[0].GetInt())) {
      AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0));
      DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt();
    }
  }

  // add edges
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    TStrV FieldsV; Line.SplitOnAllCh(',', FieldsV);

    TFltFltH Alphas;
    if (FieldsV.Len() == 3) { 
    Alphas.AddDat(0.0) = FieldsV[2].GetFlt(); 
    } else {
      for (int i=2; i<FieldsV.Len()-1; i+=2) {
        Alphas.AddDat(FieldsV[i].GetFlt()) = FieldsV[i+1].GetFlt();
      }
    }

    Network.AddEdge(FieldsV[0].GetInt(), FieldsV[1].GetInt(), Alphas);

    if (verbose) {
      printf("Edge %d -> %d: ", FieldsV[0].GetInt(), FieldsV[1].GetInt());
      TFltFltH &AlphasE = Network.GetEDat(FieldsV[0].GetInt(), FieldsV[1].GetInt());
      for (int i=0; i<AlphasE.Len(); i+=2) { printf("(%f, %f)", AlphasE.GetKey(i).Val, AlphasE[i].Val); }
      printf("\n");
    }
  }

  printf("groundtruth nodes:%d edges:%d\n", Network.GetNodes(), Network.GetEdges());
}

void TNIBs::LoadGroundTruthNodesTxt(TSIn& SIn) {
  TStr Line;

  Network.Clr(); // clear network (if any)

  // add nodes
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    Network.AddNode(NIdV[0].GetInt(), NIdV[1]);
    if (!IsNodeNm(NIdV[0].GetInt())) {
      AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0));
      DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt();
    }
  }

  printf("groundtruth nodes:%d\n", Network.GetNodes());
}

void TNIBs::LoadInferredTxt(TSIn& SIn) {
  bool verbose = false;
  TStr Line;

  InferredNetwork.Clr(); // clear network (if any)

  // add nodes
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    if (DomainsIdH.IsKey(NIdV[1])) { IAssert(NIdV[0].GetInt()==DomainsIdH.GetDat(NIdV[1])); }
    InferredNetwork.AddNode(NIdV[0].GetInt(), NIdV[1]);
    if (!IsNodeNm(NIdV[0].GetInt())) { AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); }
    if (!DomainsIdH.IsKey(NIdV[1])) { DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt(); }
    if (verbose) { printf("Node:%s\n", NIdV[1].CStr()); }
  }

  // add edges
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    TStrV FieldsV; Line.SplitOnAllCh(',', FieldsV);

    TFltFltH Alphas;
    if (FieldsV.Len() == 3) { 
      Alphas.AddDat(0.0) = FieldsV[2].GetFlt(); 
    } else {
      for (int i=2; i<FieldsV.Len()-1; i+=2) {
        Alphas.AddDat(FieldsV[i].GetFlt()) = FieldsV[i+1].GetFlt();
      }
    }

    InferredNetwork.AddEdge(FieldsV[0].GetInt(), FieldsV[1].GetInt(), Alphas);

    if (verbose) {
      printf("Edge %d -> %d: ", FieldsV[0].GetInt(), FieldsV[1].GetInt());
      TFltFltH &AlphasE = InferredNetwork.GetEDat(FieldsV[0].GetInt(), FieldsV[1].GetInt());
      for (int i=0; i<AlphasE.Len(); i+=2) { printf("(%f, %f)", AlphasE.GetKey(i).Val, AlphasE[i].Val); }
      printf("\n");
    }
  }

  printf("inferred nodes:%d edges:%d\n", InferredNetwork.GetNodes(), InferredNetwork.GetEdges());
}

void TNIBs::LoadInferredNodesTxt(TSIn& SIn) {
  TStr Line;

  InferredNetwork.Clr(); // clear network (if any)

  // add nodes
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    if (DomainsIdH.IsKey(NIdV[1])) { IAssert(NIdV[0].GetInt()==DomainsIdH.GetDat(NIdV[1])); }
    InferredNetwork.AddNode(NIdV[0].GetInt(), NIdV[1]);
    if (!IsNodeNm(NIdV[0].GetInt())) { AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); }
    if (!DomainsIdH.IsKey(NIdV[1])) { DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt(); }
  }

  printf("Nodes:%d\n", InferredNetwork.GetNodes());
}

void TNIBs::AddCasc(const TStr& CascStr, const TModel& Model) {
  int CId = CascH.Len();

  // support cascade id if any
  TStrV FieldsV; CascStr.SplitOnAllCh(';', FieldsV);
  if (FieldsV.Len()==2) { CId = FieldsV[0].GetInt(); }

  // read nodes
    TStrV NIdV; FieldsV[FieldsV.Len()-1].SplitOnAllCh(',', NIdV);
    TCascade C(CId, Model);
    for (int i = 0; i < NIdV.Len(); i+=2) {
      int NId;
      double Tm; 
      NId = NIdV[i].GetInt();
      Tm = NIdV[i+1].GetFlt();
      GetNodeInfo(NId).Vol = GetNodeInfo(NId).Vol + 1;
      C.Add(NId, Tm);
    }
    C.Sort();

    AddCasc(C);
}

void TNIBs::AddCasc(const TIntFltH& Cascade, const int& CId, const TModel& Model) {
  TCascade C(CId, Model);
  for (TIntFltH::TIter NI = Cascade.BegI(); NI < Cascade.EndI(); NI++) {
    GetNodeInfo(NI.GetKey()).Vol = GetNodeInfo(NI.GetKey()).Vol + 1;
    C.Add(NI.GetKey(), NI.GetDat());
  }
  C.Sort();
  AddCasc(C);
}

void TNIBs::GenCascade(TCascade& C) {
  bool verbose = false;
  TIntFltH InfectedNIdH; TIntH InfectedBy;
  double GlobalTime, InitTime;
  double alpha;
  int StartNId;

  if (Network.GetNodes() == 0)
    return;

        // set random seed
        TInt::Rnd.Randomize();

  while (C.Len() < 2) {
    C.Clr();
    InfectedNIdH.Clr();
    InfectedBy.Clr();

    InitTime = TFlt::Rnd.GetUniDev() * TotalTime; // random starting point <TotalTime
    GlobalTime = InitTime;

    StartNId = Network.GetRndNId();
    InfectedNIdH.AddDat(StartNId) = GlobalTime;

    while (true) {
      // sort by time & get the oldest node that did not run infection
      InfectedNIdH.SortByDat(true);
      const int& NId = InfectedNIdH.BegI().GetKey();
      GlobalTime = InfectedNIdH.BegI().GetDat();

      // all the nodes has run infection
      if ( GlobalTime >= TFlt::GetMn(TotalTime, InitTime+Window) )
        break;

      // add current oldest node to the network and set its time
      C.Add(NId, GlobalTime);

      if (verbose) { printf("GlobalTime:%f, infected node:%d\n", GlobalTime, NId); }

      // run infection from the current oldest node
      TStrFltFltHNEDNet::TNodeI NI = Network.GetNI(NId);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int DstNId = NI.GetOutNId(e);

        // choose the current tx rate (we assume the most recent tx rate)
        TFltFltH& Alphas = Network.GetEDat(NId, DstNId);
        for (int j=0; j<Alphas.Len() && Alphas.GetKey(j)<GlobalTime; j++) { alpha = Alphas[j]; }
        if (verbose) { printf("GlobalTime:%f, nodes:%d->%d, alpha:%f\n", GlobalTime, NId, DstNId, alpha); }

        if (alpha<1e-9) { continue; }

        // not infecting the parent
        if (InfectedBy.IsKey(NId) && InfectedBy.GetDat(NId).Val == DstNId)
          continue;

        double sigmaT;
        switch (Model) {
        case EXP:
          // exponential with alpha parameter
          sigmaT = TInt::Rnd.GetExpDev(alpha);
          break;
        case POW:
          // power-law with alpha parameter
          sigmaT = TInt::Rnd.GetPowerDev(1+alpha);
          while (sigmaT < Delta) { sigmaT = Delta*TInt::Rnd.GetPowerDev(1+alpha); }
          break;
        case RAY:
          // rayleigh with alpha parameter
          sigmaT = TInt::Rnd.GetRayleigh(1/sqrt(alpha));
          break;
        default:
          sigmaT = 1;
          break;
        }

        IAssert(sigmaT >= 0);

        double t1 = TFlt::GetMn(GlobalTime + sigmaT, TFlt::GetMn(InitTime+Window, TotalTime));

        if (InfectedNIdH.IsKey(DstNId)) {
          double t2 = InfectedNIdH.GetDat(DstNId);
          if ( t2 > t1 && t2 < TFlt::GetMn(InitTime+Window, TotalTime)) {
            InfectedNIdH.GetDat(DstNId) = t1;
            InfectedBy.GetDat(DstNId) = NId;
          }
        } else {
          InfectedNIdH.AddDat(DstNId) = t1;
          InfectedBy.AddDat(DstNId) = NId;
        }
      }

      // we cannot delete key (otherwise, we cannot sort), so we assign a big time (InitTime + window cut-off)
      InfectedNIdH.GetDat(NId) = TFlt::GetMn(InitTime+Window, TotalTime);
    }
    }

  C.Sort();
}

void TNIBs::GetGroundTruthGraphAtT(const double& Step, PNGraph &GraphAtT) {
  GraphAtT = TNGraph::New();

  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) { GraphAtT->AddNode(NI.GetKey()); }

  for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }
    double Alpha = 0.0;
    if (EI().IsKey(Step)) { Alpha = EI().GetDat(Step); }

    if (Alpha > MinAlpha) {
      GraphAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
    }
  }
}

void TNIBs::GetGroundTruthNetworkAtT(const double& Step, PStrFltNEDNet& NetworkAtT) {
  NetworkAtT = TStrFltNEDNet::New();

  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) { NetworkAtT->AddNode(NI.GetKey(), NI.GetDat().Name); }

  for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }
    double Alpha = 0.0;
    if (EI().IsKey(Step)) { Alpha = EI().GetDat(Step); }

    if (Alpha > MinAlpha) {
      NetworkAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), Alpha);
    }
  }
}

void TNIBs::GetInferredGraphAtT(const double& Step, PNGraph &GraphAtT) {
  GraphAtT = TNGraph::New();

  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) { GraphAtT->AddNode(NI.GetKey()); }

  for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    double inferredAlpha = 0.0;
    if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

    if (inferredAlpha > MinAlpha) {
      GraphAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
    }
  }
}

void TNIBs::GetInferredNetworkAtT(const double& Step, PStrFltNEDNet& NetworkAtT) {
  NetworkAtT = TStrFltNEDNet::New();

  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    NetworkAtT->AddNode(NI.GetKey(), NI.GetDat().Name);
  }

  for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    double inferredAlpha = 0.0;
    if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

    if (inferredAlpha > MinAlpha) {
      NetworkAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), inferredAlpha);
    }
  }
}

void TNIBs::Init(const TFltV& Steps) {
  // inferred network
  InferredNetwork.Clr();

  // copy nodes from NodeNmH (it will be filled by loading cascades or loading groundtruth)
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    InferredNetwork.AddNode(NI.GetKey(), NI.GetDat().Name);
  }

  // performance measures
  PrecisionRecall.Clr();
  Accuracy.Clr();
  MAE.Clr();
  TotalCascadesAlpha.Clr();

  for (int i=0; i<Steps.Len()-1; i++) {
    MAE.Add(TFltPr(Steps[i], 0.0));
    MSE.Add(TFltPr(Steps[i], 0.0));
    Accuracy.Add(TFltPr(Steps[i], 0.0));
    PrecisionRecall.Add(TFltPr(0.0,0.0));
  }
}

void TNIBs::Reset() {
    // reset vectors
  for (int i=0; i<DiffAlphas.Len(); i++) {
    DiffAlphas[i].Clr();
  }
    DiffAlphas.Clr();
    AveDiffAlphas.Clr();
    SampledCascadesH.Clr();
    TotalCascadesAlpha.Clr();
}

void TNIBs::SG(const int& NId, const int& Iters, const TFltV& Steps, const TSampling& Sampling, const TStr& ParamSampling, const bool& PlotPerformance) {
  bool verbose = false;
  int currentCascade = -1;
  TIntIntH SampledCascades;
  TStrV ParamSamplingV; ParamSampling.SplitOnAllCh(';', ParamSamplingV);

  Reset();

  printf("Node %d\n", NId);

  // traverse through all times
  for (int t=1; t<Steps.Len(); t++) {
    // find cascades whose two first infections are earlier than Steps[t]
    TIntFltH CascadesIdx;
    int num_infections = 0;
    for (int i=0; i<CascH.Len(); i++) {
      if (CascH[i].LenBeforeT(Steps[t]) > 1 &&
        ( (Sampling!=WIN_SAMPLING && Sampling!=WIN_EXP_SAMPLING) ||
          (Sampling==WIN_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) ||
          (Sampling==WIN_EXP_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) )) {
        num_infections += CascH[i].LenBeforeT(Steps[t]);
        CascadesIdx.AddDat(i) = CascH[i].GetMinTm();
      }
    }

    // if there are not recorded cascades by Steps[t], continue
    if (CascadesIdx.Len()==0) {
      printf("WARNING: No cascades recorded by %f!\n", Steps[t].Val);
      if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
      continue;
    }

    // later cascades first
    CascadesIdx.SortByDat(false);

    printf("Solving step %f: %d cascades, %d infections\n", Steps[t].Val, CascadesIdx.Len(), num_infections);
    SampledCascades.Clr();

    // sampling cascades with no replacement
    for (int i=0; i < Iters; i++) {
      switch (Sampling) {
        case UNIF_SAMPLING:
          currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
          break;

        case WIN_SAMPLING:
          currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
          break;

        case EXP_SAMPLING:
          do {
            currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[0].GetFlt());
          } while (currentCascade > CascadesIdx.Len()-1);
          break;

        case WIN_EXP_SAMPLING:
          do {
            currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[1].GetFlt());
          } while (currentCascade > CascadesIdx.Len()-1);
          break;

        case RAY_SAMPLING:
          do {
            currentCascade = (int)TFlt::Rnd.GetRayleigh(ParamSamplingV[0].GetFlt());
          } while (currentCascade > CascadesIdx.Len()-1);
          break;
      }

      if (!SampledCascades.IsKey(currentCascade)) { SampledCascades.AddDat(currentCascade) = 0; }
      SampledCascades.GetDat(currentCascade)++;

      if (verbose) { printf("Cascade %d sampled!\n", currentCascade); }

      // sampled cascade
      TCascade &Cascade = CascH[CascadesIdx.GetKey(currentCascade)];

      // update gradient and alpha's
      TIntPrV AlphasToUpdate;
      UpdateDiff(OSG, NId, Cascade, AlphasToUpdate, Steps[t]);

      // update alpha's
      for (int j=0; j<AlphasToUpdate.Len(); j++) {
        if (InferredNetwork.IsEdge(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2) &&
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).IsKey(Steps[t])
          ) {
          InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) -=
              (Gamma * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1)
                  - (Regularizer==1? Mu*InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) : 0.0));

          // project into alpha >= 0
          if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) < Tol) {
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = Tol;
          }

          // project into alpha <= MaxAlpha
          if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) > MaxAlpha) {
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = MaxAlpha;
          }
        }
      }
      if (verbose) { printf("%d transmission rates updated!\n", AlphasToUpdate.Len()); }
    }

    printf("%d different cascades have been sampled for step %f!\n", SampledCascades.Len(), Steps[t].Val);

    // For alphas that did not get updated, copy last alpha value * aging factor
    int unchanged = 0;
    for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
      if (EI().IsKey(Steps[t]) || t == 0 || !EI().IsKey(Steps[t-1])) { continue; }

      EI().AddDat(Steps[t]) = Aging*EI().GetDat(Steps[t-1]);
      unchanged++;
    }
    if (verbose) { printf("%d transmission rates that did not changed were 'aged' by %f!\n", unchanged, Aging.Val); }

    // compute performance on-the-fly
    if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
  }
}

void TNIBs::BSG(const int& NId, const int& Iters, const TFltV& Steps, const int& BatchLen, const TSampling& Sampling, const TStr& ParamSampling, const bool& PlotPerformance) {
  bool verbose = false;
  int currentCascade = -1;
  TIntIntH SampledCascades;
  TStrV ParamSamplingV; ParamSampling.SplitOnAllCh(';', ParamSamplingV);

  Reset();

  printf("Node %d (|A|: %d)\n", NId, InferredNetwork.GetNodes());

  // traverse through all times (except 0.0; we use 0.0 to compute mse/mae since the inference is delayed)
  for (int t=1; t<Steps.Len(); t++) {
    // find cascades whose two first infections are earlier than Steps[t]
    TIntFltH CascadesIdx;
    int num_infections = 0;
    for (int i = 0; i < CascH.Len(); i++) {
      if (CascH[i].LenBeforeT(Steps[t]) > 1 &&
            ( (Sampling!=WIN_SAMPLING && Sampling!=WIN_EXP_SAMPLING) ||
          (Sampling==WIN_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) ||
          (Sampling==WIN_EXP_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) )) {
        num_infections += CascH[i].LenBeforeT(Steps[t]);
        CascadesIdx.AddDat(i) = CascH[i].GetMinTm();
      }
    }

    // if there are not recorded cascades by Steps[t], continue
    if (CascadesIdx.Len() == 0) {
      printf("WARNING: No cascades recorded by %f!\n", Steps[t].Val);
      if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
      continue;
    }

    printf("Solving step %f (%d cascades, %d infections)\n", Steps[t].Val,
        CascadesIdx.Len(), num_infections);

    // sort cascade from most recent to least recent
    CascadesIdx.SortByDat(false);

    // sampling cascades with no replacement
    for (int i=0; i < Iters; i++) {
      // alphas to update
      TIntPrV AlphasToUpdate;

      // delete average gradients
      AveDiffAlphas.Clr();

      // use all cascades for the average gradient
      for (int c=0; c<BatchLen; c++) {
        switch (Sampling) {
          case UNIF_SAMPLING:
            currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
            break;

          case WIN_SAMPLING:
            currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
            break;

          case EXP_SAMPLING:
            do {
              currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[0].GetFlt());
            } while (currentCascade > CascadesIdx.Len()-1);
            break;

          case WIN_EXP_SAMPLING:
            do {
              currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[1].GetFlt());
            } while (currentCascade > CascadesIdx.Len()-1);
            break;

          case RAY_SAMPLING:
            do {
              currentCascade = (int)TFlt::Rnd.GetRayleigh(ParamSamplingV[0].GetFlt());
            } while (currentCascade > CascadesIdx.Len()-1);
            break;
        }

        // sampled cascade
        TCascade &Cascade = CascH[CascadesIdx.GetKey(currentCascade)];

        if (!SampledCascades.IsKey(currentCascade)) { SampledCascades.AddDat(currentCascade) = 0; }
        SampledCascades.GetDat(currentCascade)++;

        // update gradient and alpha's
        UpdateDiff(OBSG, NId, Cascade, AlphasToUpdate, Steps[t]);
      }

      // update alpha's
      for (int j=0; j<AlphasToUpdate.Len(); j++) {
        if (InferredNetwork.IsEdge(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2) &&
              InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).IsKey(Steps[t])) {
          switch (Regularizer) {
            case 0:
              InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) -=
                Gamma * (1.0/(double)BatchLen) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
            case 1:
              InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) =
                InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t])*(1.0-Mu*Gamma/(double)BatchLen)
                - Gamma * (1.0/(double)BatchLen) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
          }

          // project into alpha >= 0
          if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) < Tol) {
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = Tol;
          }

          // project into alpha <= MaxAlpha
          if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) > MaxAlpha) {
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = MaxAlpha;
          }
        }
      }

      // for alphas that did not get updated, copy last alpha value
      int unchanged = 0;
      for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
        if (EI().IsKey(Steps[t]) || t == 0 || !EI().IsKey(Steps[t-1])) { continue; }

        EI().AddDat(Steps[t]) = EI().GetDat(Steps[t-1]);
        unchanged++;
      }
      if (verbose) { printf("%d unchanged transmission rates updated!\n", unchanged); }
    }

    printf("%d different cascades have been sampled for step %f!\n", SampledCascades.Len(), Steps[t].Val);

    // compute performance on-the-fly for each time step
    if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
  }
}

void TNIBs::FG(const int& NId, const int& Iters, const TFltV& Steps) {
  bool verbose = false;
  
  Reset();

  printf("Node %d (|A|: %d)\n", NId, InferredNetwork.GetNodes());

  // traverse through all times
  for (int t=1; t<Steps.Len(); t++) {
    // find cascades whose two first infections are earlier than Steps[t]
    TIntFltH CascadesIdx;
    int num_infections = 0;
    for (int i=0; i<CascH.Len(); i++) {
      if (CascH[i].LenBeforeT(Steps[t]) > 1) {
        num_infections += CascH[i].LenBeforeT(Steps[t]);
        CascadesIdx.AddDat(i) = CascH[i].GetMinTm();
      }
    }

    // if there are not recorded cascades by Steps[t], continue
    if (CascadesIdx.Len()==0) {
      printf("WARNING: No cascades recorded by %f!\n", Steps[t].Val);
//      ComputePerformance(NId, Tol, t, Steps);
      continue;
    }

    printf("Solving step %f (%d cascades, %d infections)\n", Steps[t].Val, CascadesIdx.Len(), num_infections);

    // sort cascade from most recent to least recent
    CascadesIdx.SortByDat(false);

    // sampling cascades with no replacement
    for (int i=0; i < Iters; i++) {
      // alphas to update
      TIntPrV AlphasToUpdate;

      // delete average gradients
      AveDiffAlphas.Clr();

      // use all cascades for the average gradient
      for (int c=0; c<CascadesIdx.Len(); c++) {
        // each cascade
        TCascade &Cascade = CascH[CascadesIdx.GetKey(c)];

        // update gradient and alpha's
        UpdateDiff(OBSG, NId, Cascade, AlphasToUpdate, Steps[t]);
      }

      // update alpha's
      for (int j=0; j<AlphasToUpdate.Len(); j++) {
        if (InferredNetwork.IsEdge(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2) &&
              InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).IsKey(Steps[t])) {
          switch (Regularizer) {
            case 0:
              InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) -=
                Gamma * (1.0/(double)CascadesIdx.Len()) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
            case 1:
              InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) =
                InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t])*(1.0-Mu*Gamma/(double)CascadesIdx.Len())
                - Gamma * (1.0/(double)CascadesIdx.Len()) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
          }

          // project into alpha >= 0
          if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) < Tol) {
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = Tol;
          }

          // project into alpha <= MaxAlpha
          if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) > MaxAlpha) {
            InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = MaxAlpha;
          }
        }
      }

      // for alphas that did not get updated, copy last alpha value
      int unchanged = 0;
      for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
        if (EI().IsKey(Steps[t]) || t == 0 || !EI().IsKey(Steps[t-1])) { continue; }

        EI().AddDat(Steps[t]) = EI().GetDat(Steps[t-1]);
        unchanged++;
      }
      if (verbose) { printf("%d unchanged transmission rates updated!\n", unchanged); }
    }

    // compute performance on-the-fly for each time step
    ComputePerformanceNId(NId, t, Steps);
  }
}

void TNIBs::UpdateDiff(const TOptMethod& OptMethod, const int& NId, TCascade& Cascade, TIntPrV& AlphasToUpdate, const double& CurrentTime) {
  IAssert(InferredNetwork.IsNode(NId));

  double sum = 0.0;

  // we assume cascade is sorted & iterator returns in sorted order
  if (Cascade.IsNode(NId) && Cascade.GetTm(NId) <= CurrentTime) {
    for (THash<TInt, THitInfo>::TIter NI = Cascade.BegI(); NI < Cascade.EndI(); NI++) {
      // consider only nodes that are earlier in time
      if ( (Cascade.GetTm(NId)<=NI.GetDat().Tm) ||
           (Cascade.GetTm(NId)-Delta<=NI.GetDat().Tm && Model==POW)
         ) { break; }

      TIntPr Pair(NI.GetKey(), NId);

      // if edge/alpha doesn't exist, create
      if (!InferredNetwork.IsEdge(Pair.Val1, Pair.Val2)) { InferredNetwork.AddEdge(Pair.Val1, Pair.Val2, TFltFltH()); }
      if (!InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).IsKey(CurrentTime)) {
        InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).AddDat(CurrentTime) = InitAlpha;
      }

      switch(Model) {
        case EXP: // exponential
          sum += InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).GetDat(CurrentTime).Val;
          break;
        case POW: // powerlaw
          sum += InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).GetDat(CurrentTime).Val/(Cascade.GetTm(NId)-NI.GetDat().Tm);
          break;
        case RAY: // rayleigh
          sum += InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).GetDat(CurrentTime).Val*(Cascade.GetTm(NId)-NI.GetDat().Tm);
          break;
        default:
          sum = 0.0;
      }
    }
  }

  // we assume cascade is sorted & iterator returns in sorted order
  for (THash<TInt, THitInfo>::TIter NI = Cascade.BegI(); NI < Cascade.EndI(); NI++) {
    // only consider nodes that are earlier in time if node belongs to the cascade
    if ( Cascade.IsNode(NId) && (Cascade.GetTm(NId)<=NI.GetDat().Tm ||
        (Cascade.GetTm(NId)-Delta<=NI.GetDat().Tm && Model==POW))
       ) { break; }

    // consider infected nodes up to CurrentTime
    if (NI.GetDat().Tm > CurrentTime) { break; }

    TIntPr Pair(NI.GetKey(), NId); // potential edge

    double val = 0.0;

    if (Cascade.IsNode(NId) && Cascade.GetTm(NId) <= CurrentTime) {
      IAssert((Cascade.GetTm(NId) - NI.GetDat().Tm) > 0.0);

      switch(Model) { // compute gradients for infected
        case EXP: // exponential
          val = (Cascade.GetTm(NId) - NI.GetDat().Tm) - 1.0/sum;
          break;
        case POW: // powerlaw
          val = log((Cascade.GetTm(NId) - NI.GetDat().Tm)/Delta) - 1.0/((Cascade.GetTm(NId)-NI.GetDat().Tm)*sum);
          break;
        case RAY: // rayleigh
          val = TMath::Power(Cascade.GetTm(NId)-NI.GetDat().Tm, 2.0)/2.0 - (Cascade.GetTm(NId)-NI.GetDat().Tm)/sum;
          break;
        default:
          val = 0.0;
      }
    } else { // compute gradients for non infected
      IAssert((CurrentTime - NI.GetDat().Tm) >= 0.0);

      switch(Model) {
        case EXP: // exponential
          val = (CurrentTime-NI.GetDat().Tm);
          // if every cascade was recorded up to a maximum Window cut-off
          if ( (Window > -1) && (CurrentTime-Cascade.GetMinTm() > Window) ) { val = (Cascade.GetMinTm()+Window-NI.GetDat().Tm); }
          break;
        case POW: // power-law
          val = TMath::Mx(log((CurrentTime-NI.GetDat().Tm)/Delta), 0.0);
          // if every cascade was recorded up to a maximum Window cut-off
          if ( (Window > -1) && (CurrentTime-Cascade.GetMinTm() > Window) ) { val = TMath::Mx(log((Cascade.GetMinTm()+Window-NI.GetDat().Tm)/Delta), 0.0); }
          break;
        case RAY: // rayleigh
          val = TMath::Power(CurrentTime-NI.GetDat().Tm,2.0)/2.0;
          // if every cascade was recorded up to a maximum Window cut-off
          if ( (Window > -1) && (CurrentTime-Cascade.GetMinTm() > Window) ) { val = TMath::Power(Cascade.GetMinTm()+Window-NI.GetDat().Tm,2.0)/2.0; }
          break;
        default:
          val = 0.0;
      }
    }

    if (!AveDiffAlphas.IsKey(Pair.Val1)) { AveDiffAlphas.AddDat(Pair.Val1) = 0.0; }

    switch (OptMethod) {
      case OBSG:
      case OEBSG:
      case OFG:
        // update stochastic average gradient (based on batch for OBSG and OEBSG and based on all cascades for FG)
        AveDiffAlphas.GetDat(Pair.Val1) += val;
        break;
      case OSG:
      case OESG:
        // update stochastic gradient (we use a single gradient due to current cascade)
        AveDiffAlphas.GetDat(Pair.Val1) = val;
      default:
        break;
    }

    AlphasToUpdate.Add(Pair);
  }

  return;
}

void TNIBs::find_C( int t, TFltV &x, TFltVV &C, const int& k, const double& s, const double& gamma, const double& T ){
  if ( t >= x.Len() ) return;
  if ( t == 0 ){
    C = TFltVV( x.Len(), k );
  }else{
    int n = x.Len() - 1;
    for (int j = 0; j < k; j++){
      double alpha = ( (x.Len() ) / T ) * pow( s, j );
      double term_1 = -log(alpha) + alpha * x[t];
      double term_2 = 0;
      if ( t == 1 ){
        term_2 = j * log((double) n) * gamma;
      }
      else{
        bool first = false;
        for (int l = 0; l < k; l++){
          double my_val = C(t-1, l);
          if ( j > l ) my_val += (j - l) * log((double) n) * gamma;
          if ( !first || my_val < term_2 ){
            term_2 = my_val;
            first = true;
          }
        }
      }
      C( t, j ) = term_1 + term_2;
    }
  }
  find_C( t + 1, x, C, k, s, gamma, T );
}

void TNIBs::find_min_state( TFltVV &C, TIntV &states, const int& k, const double& s, const double& gamma, const double& T ){
  states = TIntV( C.GetRows() );
  states[0] = 0;
  int n = C.GetRows() - 1;
  for (int t = C.GetRows() - 1; t > 0; t --){
    double best_val = 0;
    int best_state = -1;
    for (int j = 0; j < C.GetCols(); j++){
      double c_state = C( t, j );
      if ( t < C.GetRows() - 2 && states[t+1] > j ){
        c_state += ( states[t+1] - j ) * gamma * log((double) n);
      }
      if ( best_state == -1 || best_val > c_state ){
        best_state = j;
        best_val = c_state;
      }
    }
    states[t] = best_state;
  }
}

void TNIBs::LabelBurstAutomaton( const int& SrcId, const int& DstId, TIntV &state_labels, TFltV &state_times, const bool& inferred, const int& k, const double& s, const double& gamma, const TSecTm& MinTime, const TSecTm& MaxTime ){
  TVec<TSecTm> arrival_times;
  TFltFltH &LinksEdge = (inferred? InferredNetwork.GetEDat(SrcId, DstId) : Network.GetEDat(SrcId, DstId));

  for (int i=0; i<LinksEdge.Len(); i++) {
    if (LinksEdge[i]>MinAlpha) {
      TSecTm tsecs;
      tsecs = (uint)LinksEdge.GetKey(i); // link rates is in seconds
      if (tsecs > MaxTime || tsecs < MinTime) { continue; }
      arrival_times.Add(tsecs);
    }
  }

  if ( arrival_times.Len() < 2 ) return;

  TFltV x;
  x.Add( 0 );
  arrival_times.Sort(true);
  double T = ((double)arrival_times.Last().GetAbsSecs()) - ((double)arrival_times[0].GetAbsSecs());
  for (int i = 1; i < arrival_times.Len(); i++){
    x.Add( ((double)arrival_times[i].GetAbsSecs()) - ((double)arrival_times[i-1].GetAbsSecs()) );
  }
  TFltVV Cost_matrix;
  //printf("arrivals = %d\n", x.Len() );
  find_C( 0, x, Cost_matrix, k, s, gamma, T);

  find_min_state( Cost_matrix, state_labels, k, s, gamma, T );

  for (int i=0; i<state_labels.Len(); i++) { state_times.Add((double)arrival_times[i].GetAbsSecs()); }
}


void TNIBs::ComputePerformanceNId(const int& NId, const int& t, const TFltV& Steps) {
  double CurrentMAE = 0.0;
  double CurrentMSE = 0.0;
  TFltPr CurrentPrecisionRecall(0.0, 0.0);
  double CurrentAccD = 0.0;

  TStrFltFltHNEDNet::TNodeI NI = InferredNetwork.GetNI(NId);
  for (int i=0; i<NI.GetInDeg(); i++) {
    double inferredAlpha = InferredNetwork.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t]);
    // we get the true alpha in the previous step (the inferred network contains delayed estimates)
    double trueAlpha = 0.0;
    if (Network.IsEdge(NI.GetInNId(i), NId) && Network.GetEDat(NI.GetInNId(i), NId).IsKey(Steps[t-1])) { trueAlpha = Network.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t-1]); }

    // accuracy (denominator)
    CurrentAccD += (double) (inferredAlpha > MinAlpha);

    // precision
    CurrentPrecisionRecall.Val2 += (double) (inferredAlpha > MinAlpha && trueAlpha<MinAlpha);
  }

  NI = Network.GetNI(NId);
  int NumEdges = 0;
  for (int i=0; i<NI.GetInDeg(); i++) {
    TIntPr Pair(NI.GetInNId(i), NId);

    // we get the inferred Alpha if any
    double inferredAlpha = 0.0;
    if (InferredNetwork.IsEdge(NI.GetInNId(i), NId) && InferredNetwork.GetEDat(NI.GetInNId(i), NId).IsKey(Steps[t])) {
      inferredAlpha = InferredNetwork.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t]).Val;
    }

    // we get the true alpha in the previous step (the inferred network contains delayed estimates)
    double trueAlpha = 0.0;
    if (Network.GetEDat(NI.GetInNId(i), NId).IsKey(Steps[t-1])) { trueAlpha = Network.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t-1]); }

    // mae
    if (trueAlpha > MinAlpha) {
      NumEdges++;
      CurrentMAE += fabs(trueAlpha - TFlt::GetMn(inferredAlpha, MaxAlpha))/trueAlpha;
    }

    // mse
    CurrentMSE += pow(trueAlpha - TFlt::GetMn(inferredAlpha, MaxAlpha), 2.0);

    // recall
    CurrentPrecisionRecall.Val1 += (double) (inferredAlpha > MinAlpha && trueAlpha > MinAlpha);
  }

  if (NumEdges > 0) {
    MAE[t-1].Val2 += CurrentMAE / ((double)(NumEdges*Network.GetNodes()));
    MSE[t-1].Val2 += CurrentMSE / ((double)(NumEdges*Network.GetNodes()));
    PrecisionRecall[t-1].Val1 += CurrentPrecisionRecall.Val1/(double)(NumEdges*Network.GetNodes());
  }

  if (CurrentAccD > 0) {
    PrecisionRecall[t-1].Val2 += (1.0 - CurrentPrecisionRecall.Val2 / CurrentAccD)/(double)Network.GetNodes();
  } else {
    PrecisionRecall[t-1].Val2 += 1.0/(double)Network.GetNodes();
  }

  Accuracy[t-1].Val2 = 1.0 - (1.0-PrecisionRecall[t-1].Val2)/(PrecisionRecall[t-1].Val2 * (1.0/PrecisionRecall[t-1].Val2 + 1.0/PrecisionRecall[t-1].Val1)) - (1.0-PrecisionRecall[t-1].Val1)/(PrecisionRecall[t-1].Val1* (1.0/PrecisionRecall[t-1].Val2 + 1.0/PrecisionRecall[t-1].Val1));
}

void TNIBs::SaveInferredPajek(const TStr& OutFNm, const double& Step, const TIntV& NIdV) {
    TFOut FOut(OutFNm);
    FOut.PutStr(TStr::Fmt("*Vertices %d\r\n", NodeNmH.Len()));
    for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
      if (NIdV.Len() > 0 && !NIdV.IsIn(NI.GetKey())) { continue; }

      FOut.PutStr(TStr::Fmt("%d \"%s\" ic Blue\r\n", NI.GetKey().Val+1, NI.GetDat().Name.CStr()));
    }
    FOut.PutStr("*Arcs\r\n");
    for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
      if (NIdV.Len() > 0 && (!NIdV.IsIn(EI.GetSrcNId()) || !NIdV.IsIn(EI.GetDstNId()))) { continue; }
      if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

      double inferredAlpha = 0.0;
      if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

      if (inferredAlpha > MinAlpha) {
        FOut.PutStr(TStr::Fmt("%d %d %f\r\n", EI.GetSrcNId()+1, EI.GetDstNId()+1, (inferredAlpha > MaxAlpha? MaxAlpha.Val : inferredAlpha)));
      }
    }
}

void TNIBs::SaveInferred(const TStr& OutFNm, const TIntV& NIdV) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    if (NIdV.Len() > 0 && !NIdV.IsIn(NI.GetKey())) { continue; }

    FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
  }

  FOut.PutStr("\r\n");

  // write edges to file (not allowing self loops in the network)
  for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
    if (NIdV.Len() > 0 && (!NIdV.IsIn(EI.GetSrcNId()) || !NIdV.IsIn(EI.GetDstNId()))) { continue; }
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    // not allowing self loops in the Kronecker network
    if (EI.GetSrcNId() != EI.GetDstNId()) {
      if (EI().Len() > 0) {
        TStr Line; bool IsEdge = false;
        for (int i=0; i<EI().Len(); i++) {
          if (EI()[i]>MinAlpha) {
            Line += TStr::Fmt(",%f,%f", EI().GetKey(i).Val, (EI()[i] > MaxAlpha? MaxAlpha.Val : EI()[i].Val) );
            IsEdge = true;
          } else { // we write 0 explicitly
            Line += TStr::Fmt(",%f,0.0", EI().GetKey(i).Val);
          }
        }
        // if none of the alphas is bigger than 0, no edge is written
        if (IsEdge) {
          FOut.PutStr(TStr::Fmt("%d,%d", EI.GetSrcNId(), EI.GetDstNId()));
          FOut.PutStr(Line);
          FOut.PutStr("\r\n");
        }
      }
      else
        FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
    }
  }
}

void TNIBs::SaveInferred(const TStr& OutFNm, const double& Step, const TIntV& NIdV) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    if (NIdV.Len() > 0 && !NIdV.IsIn(NI.GetKey())) { continue; }

    FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
  }
  FOut.PutStr("\r\n");

  // write edges to file (not allowing self loops in the network)
  for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
    if (NIdV.Len() > 0 && (!NIdV.IsIn(EI.GetSrcNId()) || !NIdV.IsIn(EI.GetDstNId()))) { continue; }
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    // not allowing self loops in the Kronecker network
    if (EI.GetSrcNId() != EI.GetDstNId()) {
      double inferredAlpha = 0.0;
      if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

      if (inferredAlpha > MinAlpha) {
        FOut.PutStr(TStr::Fmt("%d,%d,%f\r\n", EI.GetSrcNId(), EI.GetDstNId(), (inferredAlpha > MaxAlpha? MaxAlpha.Val : inferredAlpha)));
      }
    }
  }
}

void TNIBs::SaveInferredEdges(const TStr& OutFNm) {
  TFOut FOut(OutFNm);

  // write edges to file (not allowing self loops in the network)
  for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    // not allowing self loops in the Kronecker network
    if (EI.GetSrcNId() != EI.GetDstNId()) {
      if (EI().Len() > 0) {
        TStr Line; bool IsEdge = false;
        for (int i=0; i<EI().Len(); i++) {
          if (EI()[i]>MinAlpha) {
            Line += TStr::Fmt(",%f,%f", EI().GetKey(i).Val, (EI()[i] > MaxAlpha? MaxAlpha.Val : EI()[i].Val) );
            IsEdge = true;
          } else { // we write 0 explicitly
            Line += TStr::Fmt(",%f,0.0", EI().GetKey(i).Val);
          }
        }
        // if none of the alphas is bigger than 0, no edge is written
        if (IsEdge) {
          FOut.PutStr(TStr::Fmt("%d,%d", EI.GetSrcNId(), EI.GetDstNId()));
          FOut.PutStr(Line);
          FOut.PutStr("\r\n");
        }
      }
      else
        FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
    }
  }
}

void TNIBs::SaveGroundTruth(const TStr& OutFNm) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
  }

  FOut.PutStr("\r\n");

  // write edges to file (not allowing self loops in the network)
  for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
    if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    // not allowing self loops in the Kronecker network
    if (EI.GetSrcNId() != EI.GetDstNId()) {
      if (EI().Len() > 0) {
        FOut.PutStr(TStr::Fmt("%d,%d,", EI.GetSrcNId(), EI.GetDstNId()));
        for (int i=0; i<EI().Len()-1; i++) { FOut.PutStr(TStr::Fmt("%f,%f,", EI().GetKey(i).Val, EI()[i].Val)); }
        FOut.PutStr(TStr::Fmt("%f,%f", EI().GetKey(EI().Len()-1).Val, EI()[EI().Len()-1].Val));
        FOut.PutStr("\r\n");
      }
      else
        FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
    }
  }
}

void TNIBs::SaveGroundTruthPajek(const TStr& OutFNm, const double& Step) {
    TFOut FOut(OutFNm);

    FOut.PutStr(TStr::Fmt("*Vertices %d\r\n", NodeNmH.Len()));
    for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
      FOut.PutStr(TStr::Fmt("%d \"%s\" ic Blue\r\n", NI.GetKey().Val+1, NI.GetDat().Name.CStr()));
    }
    FOut.PutStr("*Arcs\r\n");
    for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
      if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }
      double trueAlpha = 0.0;
      if (EI().IsKey(Step)) { trueAlpha = EI().GetDat(Step); }
      else { for (int j=0; j<EI().Len() && EI().GetKey(j)<=Step; j++) { trueAlpha = EI()[j]; } }

      if (trueAlpha > MinAlpha) {
        FOut.PutStr(TStr::Fmt("%d %d %f\r\n", EI.GetSrcNId()+1, EI.GetDstNId()+1, (trueAlpha > MaxAlpha? MaxAlpha.Val : trueAlpha)));
      }
    }
}

void TNIBs::SaveSites(const TStr& OutFNm, const TIntFltVH& CascadesPerNode) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    FOut.PutStr(TStr::Fmt("%d,%s", NI.GetKey().Val, NI.GetDat().Name.CStr()));
    if (CascadesPerNode.IsKey(NI.GetKey().Val)) {
      for (int i=0; i<CascadesPerNode.GetDat(NI.GetKey().Val).Len(); i++) {
        FOut.PutStr(TStr::Fmt(",%f", CascadesPerNode.GetDat(NI.GetKey().Val)[i].Val));
      }
    }
    FOut.PutStr("\r\n");
  }
}

void TNIBs::SaveCascades(const TStr& OutFNm) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
  }

  FOut.PutStr("\r\n");

  // write cascades to file
  for (THash<TInt, TCascade>::TIter CI = CascH.BegI(); CI < CascH.EndI(); CI++) {
    TCascade &C = CI.GetDat();
    int j = 0;
    for (THash<TInt, THitInfo>::TIter NI = C.NIdHitH.BegI(); NI < C.NIdHitH.EndI(); NI++) {
      if (!NodeNmH.IsKey(NI.GetDat().NId)) { continue; }
      if (j > 0) { FOut.PutStr(TStr::Fmt(",%d,%f", NI.GetDat().NId.Val, NI.GetDat().Tm.Val)); }
      else { FOut.PutStr(TStr::Fmt("%d;%d,%f", CI.GetKey().Val, NI.GetDat().NId.Val, NI.GetDat().Tm.Val)); }
      j++;
    }

    if (j >= 1)
      FOut.PutStr(TStr::Fmt("\r\n"));
  }
}
