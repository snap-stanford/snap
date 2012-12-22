#include "stdafx.h"
#include "cascdynetinf.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nStochastic network inference. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try

  bool verbose = true;

  const TStr InFNm  = Env.GetIfArgPrefixStr("-i:", "example-cascades.txt", "Input cascades");
  const TStr GroundTruthFNm = Env.GetIfArgPrefixStr("-n:", "example-network.txt", "Input ground-truth network");
  const TStr OutFNm  = Env.GetIfArgPrefixStr("-o:", "network", "Output file name(s) prefix");

  const TModel Model = (TModel)Env.GetIfArgPrefixInt("-m:", 0, "0:exponential, 1:power law, 2:rayleigh, 3:weibull");
  const double Delta = Env.GetIfArgPrefixFlt("-d:", 1.0, "Delta for power-law (default:1)\n"); // delta for power law
  const double k = Env.GetIfArgPrefixFlt("-k:", 1.0, "Shape parameter k for Weibull distribution for -m:3 (default:1)\n"); // k for weibull

  const TRunningMode RunningMode = (TRunningMode)Env.GetIfArgPrefixInt("-rm:", 0,"Running mode\n0:Time step, 1:Infections step, 2:Cascade step, 3:Single time point\n");
  const double TimeStep = Env.GetIfArgPrefixFlt("-ts:", 10.0, "Minimum time step size for -rm:0 (default:10.0)\n");
  const int NumberInfections = Env.GetIfArgPrefixInt("-is:", -1, "Number of infections for -rm:1  (default:-1)\n");

  const TStr MinTimeStr = Env.GetIfArgPrefixStr("-it:", "-1", "First time (default:-1)\n");
  const TStr MaxTimeStr = Env.GetIfArgPrefixStr("-tt:", "-1", "Last time (default:-1)\n");
  const int InputTimeFormat = Env.GetIfArgPrefixInt("-itf:",0, "First/last time format\n0:no time units,1:a date to be parsed\n");
  const int CascadesTimeFormat = Env.GetIfArgPrefixInt("-ctf:", 0, "Cascades time format\n0:no time units,1:second,2:minute,3:hour,4:6 hours,5:12 hours, 6:day\n");

  double Window = Env.GetIfArgPrefixFlt("-h:", -1, "Time window per cascade (if any), -1 means the window is set by  means last infection time (default:-1)\n");

  const TStr NodeIdx = Env.GetIfArgPrefixStr("-ni:", "-1", "Node indeces to estimate incoming tx rates (-1:all nodes, -X:random -X-node subset)");

  const TOptMethod TOpt = (TOptMethod)Env.GetIfArgPrefixInt("-t:", 0, "Optimization method\n0:SG, 1:Windowed SG, 2:Exp Decay SG, 3:Windowed Exp Decay SG, 4:Ray Decay SG, 5:Batch SG, 6:Windowed Batch SG, 7:Exp Decay Batch SG, 8:Ray Decay Batch, 9:FG");
  const int Iters  = Env.GetIfArgPrefixInt("-e:", 1000, "Number of iterations per time step");
  const int BatchLen = Env.GetIfArgPrefixInt("-bl:", 1000, "Number of cascades for each batch, -t:2 & -t:4 (default:1000)");
  const TStr ParamSampling = Env.GetIfArgPrefixStr("-sd:", "0.1", "Params for -t:1,2 & -t:4,5 (default:0.1)\n");

  const double Gamma = Env.GetIfArgPrefixFlt("-g:", 0.001, "Alpha for gradient descend (default:0.01)\n");
  const double Aging = Env.GetIfArgPrefixFlt("-a:", 1.0, "Aging factor for non-used edges (default:1.0)\n");
  const TRegularizer Regularizer = (TRegularizer)Env.GetIfArgPrefixInt("-r:", 0, "Regularizer\n0:no, 1:l2");
  const double Mu = Env.GetIfArgPrefixFlt("-mu:", 0.01, "Mu for regularizer (default:0.01)\n");

  const double Tol = Env.GetIfArgPrefixFlt("-tl:", 0.0005, "Tolerance (default:0.01)\n");
  const double MinAlpha = Env.GetIfArgPrefixFlt("-la:", 0.05, "Min alpha (default:0.05)\n");
  const double MaxAlpha = Env.GetIfArgPrefixFlt("-ua:", 100, "Maximum alpha (default:100)\n");
  const double InitAlpha = Env.GetIfArgPrefixFlt("-ia:", 0.01, "Initial alpha (default:0.01)\n");

  const int SaveOnlyEdges = Env.GetIfArgPrefixInt("-oe:", 0, "Save only edges, not nodes\n:0:edges and nodes, 1:only edges (default:0)\n");

  const int TakeAdditional = Env.GetIfArgPrefixInt("-s:", 1, "How much additional files to create?\n\
    0:no plots, 1:precision-recall plot, 2:accuracy plot, 3:mae plot, 4:mse plot, 5:all plots\n");

  bool PlotPrecisionRecall = false; 
  bool PlotAccuracy = false;
  bool PlotMAE = false;
  bool PlotMSE = false;
  
  bool PlotPerformance = false;

  switch (TakeAdditional) {
     case 0: break; // no plots
     case 1 : PlotPrecisionRecall = true; break;
     case 2 : PlotAccuracy = true; break;
     case 3 : PlotMAE = true; break;
     case 4 : PlotMSE = true; break;
     case 5 :
       PlotPrecisionRecall = true;
       PlotAccuracy = true;
       PlotMAE = true; 
       PlotMSE = true;
       break;
     default: 
         FailR("Bad -s: parameter.");
  }

  PlotPerformance = (PlotPrecisionRecall || PlotAccuracy || PlotMAE || PlotMSE);

  TNIBs NIBs;
  printf("\nLoading input cascades: %s\n", InFNm.CStr());

  NIBs.SetModel(Model);
  NIBs.SetDelta(Delta);
  NIBs.SetK(k);
  NIBs.SetGamma(Gamma);
  NIBs.SetTolerance(Tol);
  NIBs.SetMaxAlpha(MaxAlpha);
  NIBs.SetMinAlpha(MinAlpha);
  NIBs.SetInitAlpha(InitAlpha);
  NIBs.SetRegularizer(Regularizer);
  NIBs.SetMu(Mu);
  NIBs.SetWindow(Window);
  NIBs.SetAging(Aging);

  // load cascades from file
  TFIn FIn(InFNm);
  NIBs.LoadCascadesTxt(FIn);

  // load ground truth network
  if (PlotPrecisionRecall || PlotAccuracy || PlotMAE || PlotMSE) {
    TFIn FGIn(GroundTruthFNm);
    NIBs.LoadGroundTruthTxt(FGIn);
  } else {
    for (THash<TInt, TNodeInfo>::TIter NI = NIBs.NodeNmH.BegI(); NI<NIBs.NodeNmH.EndI(); NI++) { NIBs.Network.AddNode(NI.GetKey()); }
  }

  printf("cascades:%d\nRunning Stochastic Network Inference..\n", NIBs.GetCascs());

  double MaxTime = TFlt::Mn;
  double MinTime = TFlt::Mx;

  if (MaxTimeStr.EqI("-1")) {
    // find maximum time across cascades
    for (int i=0; i<NIBs.GetCascs(); i++) {
      if (NIBs.CascH[i].GetMaxTm() > MaxTime) {
        MaxTime = NIBs.CascH[i].GetMaxTm();
      }
    }
  } else {
    if (InputTimeFormat==0) { MaxTime = MaxTimeStr.GetFlt(); }
    else {
      MaxTime = (double)TSecTm::GetDtTmFromStr(MaxTimeStr).GetAbsSecs();
    }
  }

  if (MinTimeStr.EqI("-1")) {
    // find minimum time across cascades
    MinTime = TFlt::Mx;
    for (int i=0; i<NIBs.GetCascs(); i++) {
      if (NIBs.CascH[i].GetMinTm() < MinTime && NIBs.CascH[i].GetMinTm()!=0) {
        MinTime = NIBs.CascH[i].GetMinTm();
      }
    }
  } else {
    if (InputTimeFormat==0) { MinTime = MinTimeStr.GetFlt(); }
    else {
      MinTime = (double)TSecTm::GetDtTmFromStr(MinTimeStr).GetAbsSecs();
    }
  }

  if (MinTime<1.0) { MinTime = 0.0; }

  if (InputTimeFormat==1) {
    switch(CascadesTimeFormat) {
        case 0: case 1: break;
        case 2: MinTime /= 60.0; MaxTime /= 60.0; break;
        case 3: MinTime /= 3600.0; MaxTime /= 3600.0; break;
        case 4: MinTime /= (6.0*3600.0); MaxTime /= (6.0*3600.0); break;
        case 5: MinTime /= (12.0*3600.0); MaxTime /= (12.0*3600.0); break;
        case 6: MinTime /= (24.0*3600.0); MaxTime /= (24.0*3600.0); break;
        default: FailR("Bad -s: parameter.");
    }
  }

  TFltV Steps;
  int num_infections = 0;
  TFltV InfectionsV;

  // we always add 0.0 as starting point
  Steps.Add(MinTime);

  printf("Starting time: %f\n", MinTime);

  // compute number of time points to compute estimation for each running mode
  switch (RunningMode) {
      case TIME_STEP:
        for (float t = MinTime+TimeStep; t<=MaxTime; t += TimeStep) { Steps.Add(t); if (verbose) { printf("Time: %f\n", Steps[Steps.Len()-1].Val); } }
        break;
      case INFECTION_STEP:
        // copy infections
        if (verbose) { printf("Generating infections vector...\n"); }
        for (int i=0; i<NIBs.GetCascs(); i++) {
          for (int j=0; j<NIBs.CascH[i].Len() && NIBs.CascH[i].NIdHitH[j].Tm < MaxTime; j++) {
            InfectionsV.Add(NIBs.CascH[i].NIdHitH[j].Tm);
          }
        }

        // sort infections
        if (verbose) { printf("Infections vector generated (%d infections)!\nSorting infections...\n", InfectionsV.Len()); }
        InfectionsV.Sort(true);
        if (verbose) { printf("Infections sorted!\n"); }

        // generate time steps
        for (int i=0; i<InfectionsV.Len(); i++, num_infections++) {
          if (num_infections==NumberInfections) {
            if (InfectionsV[i]!=Steps[Steps.Len()-1]) { Steps.Add(InfectionsV[i]); }
            num_infections = 0;
            if (verbose) { printf("Time: %f\n", Steps[Steps.Len()-1].Val); }
          }
        }
        break;

      case CASCADE_STEP:
        for (int i=0; i<NIBs.GetCascs(); i++) {
          if (NIBs.CascH[i].GetMaxTm()<MinTime+MaxTime) { Steps.Add(NIBs.CascH[i].GetMaxTm()); }
        }

        Steps.Sort();

        if (verbose) { for (int i=0; i<Steps.Len(); i++) { printf("Time: %f\n", Steps[i].Val); } }
        break;

      case SINGLE_STEP:
        Steps.Add(MinTime+MaxTime);
        if (verbose) { printf("Time: %f\n", Steps[0].Val); }
        break;

      default:
      FailR("Bad -rm: parameter.");

  }

  // save time steps
  TFOut FOutTimeSteps(TStr::Fmt("%s-time-steps.txt", OutFNm.CStr()));
  for (int i=0; i<Steps.Len(); i++) { FOutTimeSteps.PutStr(TStr::Fmt("%f\n", Steps[i].Val)); }

  NIBs.Init(Steps);

  TIntV NIds; TStrV NIdsStr;
  if (NodeIdx.EqI("-1")) { NIBs.Network.GetNIdV(NIds); }
  else {
    NodeIdx.SplitOnAllCh('-', NIdsStr);
    for (int i=NIdsStr[0].GetInt(); i<TInt::GetMn(NIdsStr[1].GetInt(), NIBs.NodeNmH.Len()); i++) {
      NIds.Add(NIBs.NodeNmH.GetKey(i));
    }
  }

  for (int i=0; i<NIds.Len(); i++) {
    TStrFltFltHNEDNet::TNodeI NI = NIBs.Network.GetNI(NIds[i]);
    switch (TOpt) {
        // stochastic gradient
        case OSG:
          NIBs.SG(NI.GetId(), Iters, Steps, UNIF_SAMPLING, ParamSampling, PlotPerformance);
          break;
      // windowed stochastic gradient
      case OWSG:
          NIBs.SG(NI.GetId(), Iters, Steps, WIN_SAMPLING, ParamSampling, PlotPerformance);
          break;
      // exponential decay stochastic gradient
      case OESG:
        NIBs.SG(NI.GetId(), Iters, Steps, EXP_SAMPLING, ParamSampling, PlotPerformance);
          break;
          // exponential decay stochastic gradient
      case OWESG:
        NIBs.SG(NI.GetId(), Iters, Steps, WIN_EXP_SAMPLING, ParamSampling, PlotPerformance);
        break;
      // rayleigh decay stochastic gradient
      case ORSG:
        NIBs.SG(NI.GetId(), Iters, Steps, RAY_SAMPLING, ParamSampling, PlotPerformance);
        break;
          // no decay batch stochastic gradient
        case OBSG:
          NIBs.BSG(NI.GetId(), Iters, Steps, BatchLen, UNIF_SAMPLING, ParamSampling, PlotPerformance);
          break;
        // windowed stochastic gradient
      case OWBSG:
        NIBs.BSG(NI.GetId(), Iters, Steps, BatchLen, WIN_SAMPLING, ParamSampling, PlotPerformance);
        break;
        // exponential decay batch stochastic gradient
        case OEBSG:
          NIBs.BSG(NI.GetId(), Iters, Steps, BatchLen, EXP_SAMPLING, ParamSampling, PlotPerformance);
          break;
      // exponential decay batch stochastic gradient
      case ORBSG:
          NIBs.BSG(NI.GetId(), Iters, Steps, BatchLen, RAY_SAMPLING, ParamSampling, PlotPerformance);
          break;
        // full gradient
        case OFG:
          NIBs.FG(NI.GetId(), Iters, Steps);
          break;
        default :
          FailR("Bad -s: parameter.");
    }
  }

  // Save inferred network in a file
  if (SaveOnlyEdges) {
    if (NodeIdx.EqI("-1")) {
      NIBs.SaveInferredEdges(TStr::Fmt("%s.txt", OutFNm.CStr()));
    } else {
      NIBs.SaveInferredEdges(TStr::Fmt("%s-%s-%s.txt", OutFNm.CStr(), NIdsStr[0].CStr(), NIdsStr[1].CStr()));
    }
  } else {
    if (NodeIdx.EqI("-1")) {
      NIBs.SaveInferred(TStr::Fmt("%s.txt", OutFNm.CStr()));
    } else {
        NIBs.SaveInferred(TStr::Fmt("%s-%s-%s.txt", OutFNm.CStr(), NIdsStr[0].CStr(), NIdsStr[1].CStr()));
    }
  }
  
  // plot precision recall
  if (PlotPrecisionRecall) {
    TGnuPlot::PlotValV(NIBs.PrecisionRecall, TStr::Fmt("%s-precision-recall", OutFNm.CStr()), "Precision Recall", "Recall",
                 "Precision", gpsAuto, false, gpwLinesPoints);
  }

  // plot accuracy
  if (PlotAccuracy) {
      TGnuPlot::PlotValV(NIBs.Accuracy, TStr::Fmt("%s-accuracy", OutFNm.CStr()), "Accuracy", "Time", "Accuracy");
  }

  // Plot MAE
  if (PlotMAE) {
    TGnuPlot::PlotValV(NIBs.MAE, TStr::Fmt("%s-mae", OutFNm.CStr()), "Normalized MAE", "Time", "Normalized MAE");
  }

  // Plot MSE
  if (PlotMSE) {
    TGnuPlot::PlotValV(NIBs.MSE, TStr::Fmt("%s-mse", OutFNm.CStr()), "MSE", "Time", "MSE");
  }

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}