#include "stdafx.h"
#include "cascnetinf.h"

double TCascade::TransProb(const int& NId1, const int& NId2) const {
  if (!IsNode(NId1) || !IsNode(NId2)) { return Eps.Val; }
  if (GetTm(NId1) >= GetTm(NId2)) { return Eps.Val; }
  if (Model==0)
    return Alpha*exp(-Alpha*(GetTm(NId2)-GetTm(NId1))); // exponential
  else if (Model==1)
    return (Alpha-1)*pow((GetTm(NId2)-GetTm(NId1)), -Alpha); // power-law
  else
    return Alpha*(GetTm(NId2)-GetTm(NId1))*exp(-0.5*Alpha*pow(GetTm(NId2)-GetTm(NId1), 2)); // rayleigh

  return (-1);
}

double TCascade::GetProb(const PNGraph& G) {
    double P = 0;
    for (int n = 0; n < Len(); n++) {
      const int DstNId = GetNode(n);
      const double DstTm = GetTm(DstNId);
      TNGraph::TNodeI NI = G->GetNI(DstNId);
      double MxProb = log(Eps);
      int BestParent = -1;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int SrcNId = NI.GetInNId(e);
        if (IsNode(SrcNId) && GetTm(SrcNId) < DstTm) {
          const double Prob = log(TransProb(SrcNId, DstNId));
          if (MxProb < Prob) { MxProb = Prob;  BestParent = SrcNId; }
        }
      }
      NIdHitH.GetDat(DstNId).Parent = BestParent;
      P += MxProb;
    }

    return P;
}

// init prob of a cascade in an empty graph
void TCascade::InitProb() {
    CurProb = log(Eps) * Len();
    for (int i = 0; i < Len(); i++) {
      NIdHitH[i].Parent = -1; }
}

// update the cascade probability given a new edge (N1, N2) in the graph
double TCascade::UpdateProb(const int& N1, const int& N2, const bool& UpdateProb) {
    if (!IsNode(N1) || !IsNode(N2)) { return CurProb; }
    if (GetTm(N1) >= GetTm(N2)) { return CurProb; }
    const double P1 = log(TransProb(GetParent(N2), N2));
    const double P2 = log(TransProb(N1, N2)); // N1 influences N2
    if (P1 < P2) {
      if (UpdateProb) { // the edge is there, update the CurProb and best Parent
        CurProb = CurProb - P1 + P2;
        NIdHitH.GetDat(N2).Parent = N1;
      } else {
        return CurProb - P1 + P2; }
    }
    return CurProb;
}

void TNetInfBs::LoadCascadesTxt(TSIn& SIn, const int& Model, const double& alpha) {
  TStr Line;
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
      AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); 
  }
  printf("All nodes read!\n");
  while (!SIn.Eof()) { SIn.GetNextLn(Line); AddCasc(Line, Model, alpha); }
  printf("All cascades read!\n");
}

void TNetInfBs::LoadGroundTruthTxt(TSIn& SIn) {
  GroundTruth = TNGraph::New(); TStr Line;

  // add nodes
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    if (Line=="") { break; }
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    GroundTruth->AddNode(NIdV[0].GetInt());
  }

  // add edges
  while (!SIn.Eof()) {
    SIn.GetNextLn(Line);
    TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
    GroundTruth->AddEdge(NIdV[0].GetInt(), NIdV[1].GetInt());
    if (NIdV.Len()>2) { Alphas.AddDat(TIntPr(NIdV[0].GetInt(), NIdV[1].GetInt())) = NIdV[2].GetFlt(); }
    else { Alphas.AddDat(TIntPr(NIdV[0].GetInt(), NIdV[1].GetInt())) = 1.0; }
  }

  printf("groundtruth nodes:%d edges:%d\n", GroundTruth->GetNodes(), GroundTruth->GetEdges());
}

void TNetInfBs::AddCasc(const TStr& CascStr, const int& Model, const double& alpha) {
    TStrV NIdV; CascStr.SplitOnAllCh(',', NIdV);
    TCascade C(alpha, Model);
    for (int i = 0; i < NIdV.Len(); i+=2) {
      int NId;
      double Tm; 
      NId = NIdV[i].GetInt();
      Tm = NIdV[i+1].GetFlt();
      GetNodeInfo(NId).Vol = GetNodeInfo(NId).Vol + 1;
      C.Add(NId, Tm);
    }
    C.Sort();
    CascV.Add(C);
}

void TNetInfBs::GenCascade(TCascade& C, const int& TModel, const double &window, TIntPrIntH& EdgesUsed, const double& delta,
               const double& std_waiting_time, const double& std_beta) {
  TIntFltH InfectedNIdH; TIntH InfectedBy;
  double GlobalTime; int StartNId;
  double alpha, beta;

  if (GroundTruth->GetNodes() == 0)
    return;

  while (C.Len() < 2) {
    C.Clr();
    InfectedNIdH.Clr();
    InfectedBy.Clr();
    GlobalTime = 0;

    StartNId = GroundTruth->GetRndNId();
    InfectedNIdH.AddDat(StartNId) = GlobalTime;

    while (true) {
      // sort by time & get the oldest node that did not run infection
      InfectedNIdH.SortByDat(true);
      const int& NId = InfectedNIdH.BegI().GetKey();
      GlobalTime = InfectedNIdH.BegI().GetDat();

      // all the nodes has run infection
      if (GlobalTime >= window)
        break;

      // add current oldest node to the network and set its time
      C.Add(NId, GlobalTime);

      // run infection from the current oldest node
      const TNGraph::TNodeI NI = GroundTruth->GetNI(NId);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int DstNId = NI.GetOutNId(e);

        beta = Betas.GetDat(TIntPr(NId, DstNId));

        // flip biased coin (set by beta)
        if (TInt::Rnd.GetUniDev() > beta+std_beta*TFlt::Rnd.GetNrmDev())
          continue;

        alpha = Alphas.GetDat(TIntPr(NId, DstNId));

        // not infecting the parent
        if (InfectedBy.IsKey(NId) && InfectedBy.GetDat(NId).Val == DstNId)
          continue;

        double sigmaT;
        switch (TModel) {
        case 0:
          // exponential with alpha parameter
          sigmaT = TInt::Rnd.GetExpDev(alpha);
          break;
        case 1:
          // power-law with alpha parameter
          sigmaT = TInt::Rnd.GetPowerDev(alpha);
          while (sigmaT < delta) { sigmaT = TInt::Rnd.GetPowerDev(alpha); }
          break;
        case 2:
          // rayleigh with alpha parameter
          sigmaT = TInt::Rnd.GetRayleigh(1/sqrt(alpha));
          break;
        default:
          sigmaT = 1;
          break;
        }

        // avoid negative time diffs in case of noise
        if (std_waiting_time > 0)
          sigmaT = TFlt::GetMx(0.0, sigmaT + std_waiting_time*TFlt::Rnd.GetNrmDev());

        double t1 = GlobalTime + sigmaT;

        if (InfectedNIdH.IsKey(DstNId)) {
          double t2 = InfectedNIdH.GetDat(DstNId);
          if (t2 > t1 && t2 != window) {
            InfectedNIdH.GetDat(DstNId) = t1;
            InfectedBy.GetDat(DstNId) = NId;
          }
        } else {
          InfectedNIdH.AddDat(DstNId) = t1;
          InfectedBy.AddDat(DstNId) = NId;
        }
      }

      // we cannot delete key (otherwise, we cannot sort), so we assign a big time (window cut-off)
      InfectedNIdH.GetDat(NId) = window;
    }

  }

  C.Sort();

  for (TIntH::TIter EI = InfectedBy.BegI(); EI < InfectedBy.EndI(); EI++) {
    TIntPr Edge(EI.GetDat().Val, EI.GetKey().Val);

    if (!EdgesUsed.IsKey(Edge)) EdgesUsed.AddDat(Edge) = 0;

    EdgesUsed.GetDat(Edge) += 1;
  }
}

void TNetInfBs::Init() {
  THash<TInt, TIntV> CascPN;
    Graph = TNGraph::New();

    // reset vectors
    EdgeGainV.Clr();
    CascPerEdge.Clr();
    PrecisionRecall.Clr();

    for (int c = 0; c < CascV.Len(); c++) {
      for (int i = 0; i < CascV[c].Len(); i++) {
        if (!Graph->IsNode(CascV[c].GetNode(i))) Graph->AddNode(CascV[c].GetNode(i));
        if (!CascPN.IsKey(CascV[c].GetNode(i))) CascPN.AddDat(CascV[c].GetNode(i)) = TIntV();
        CascPN.GetDat(CascV[c].GetNode(i)).Add(c);
      }
      CascV[c].InitProb();
    }

    // only add edges that make sense (i.e., at least once coherent in time)
    for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      TIntV &Cascs = CascPN.GetDat(NI.GetId());
      for (int c = 0; c < Cascs.Len(); c++) {
        for (int i=0; i < CascV[Cascs[c]].Len(); i++) {
          if (CascV[Cascs[c]].GetNode(i)==NI.GetId())
            continue;

          if (CascV[Cascs[c]].GetTm(CascV[Cascs[c]].GetNode(i)) < CascV[Cascs[c]].GetTm(NI.GetId()) ) {
            if (!CascPerEdge.IsKey(TIntPr(CascV[Cascs[c]].GetNode(i), NI.GetId()))) {
              EdgeGainV.Add(TPair<TFlt, TIntPr>(TFlt::Mx, TIntPr(CascV[Cascs[c]].GetNode(i), NI.GetId())));
              CascPerEdge.AddDat(TIntPr(CascV[Cascs[c]].GetNode(i), NI.GetId())) = TIntV();
            }
            // Add cascade to hash of cascades per edge (to implement localized update)
            CascPerEdge.GetDat(TIntPr(CascV[Cascs[c]].GetNode(i), NI.GetId())).Add(Cascs[c]);
          }
        }
      }
    }
}

double TNetInfBs::GetAllCascProb(const int& EdgeN1, const int& EdgeN2) {
    double P = 0.0;

    if (EdgeN1==-1 && EdgeN2==-1) {
      for (int c = 0; c < CascV.Len(); c++) {
            P += CascV[c].UpdateProb(EdgeN1, EdgeN2, false); } // initial log-likelihood
      return P;
    }

    TIntV &CascsEdge = CascPerEdge.GetDat(TIntPr(EdgeN1, EdgeN2)); // only check cascades that contain the edge

    for (int c = 0; c < CascsEdge.Len(); c++) {
      P += (CascV[CascsEdge[c]].UpdateProb(EdgeN1, EdgeN2, false) - CascV[CascsEdge[c]].CurProb); } // marginal gain
    return P;
}

TIntPr TNetInfBs::GetBestEdge(double& CurProb, double& LastGain, bool& msort, int &attempts) {
  TIntPr BestE;
  TVec<TInt> KeysV;
  TVec<TPair<TFlt, TIntPr> > EdgeGainCopyToSortV;
  TIntV EdgeZero;
  double BestGain = TFlt::Mn;
  int BestGainIndex = -1;

    if (msort) {
      for (int i=0; i<TMath::Mn(attempts-1, EdgeGainV.Len()); i++)
          EdgeGainCopyToSortV.Add(EdgeGainV[i]);

      // printf("Sorting sublist of size %d of marginal gains!\n", EdgeGainCopyToSortV.Len());

      // sort this list
      EdgeGainCopyToSortV.Sort(false);

      // printf("Sublist sorted!\n");

      // clever way of resorting without need to copy (google interview question! :-))
      for (int i=0, ii=0, j=0; ii < EdgeGainCopyToSortV.Len(); j++) {
        if ( (i+EdgeGainCopyToSortV.Len() < EdgeGainV.Len()) && (EdgeGainCopyToSortV[ii].Val1 < EdgeGainV[i+EdgeGainCopyToSortV.Len()].Val1) ) {
          EdgeGainV[j] = EdgeGainV[i+EdgeGainCopyToSortV.Len()];
          i++;
        } else {
          EdgeGainV[j] = EdgeGainCopyToSortV[ii];
          ii++;
        }
      }
    }

    attempts = 0;
    
  for (int e = 0; e < EdgeGainV.Len(); e++) {
    const TIntPr& Edge = EdgeGainV[e].Val2;
    if (Graph->IsEdge(Edge.Val1, Edge.Val2)) { continue; } // if edge was already included in the graph

    const double EProb = GetAllCascProb(Edge.Val1, Edge.Val2);
    EdgeGainV[e].Val1 = EProb; // update marginal gain
    if (BestGain < EProb) {
    BestGain = EProb;
    BestGainIndex = e;
    BestE = Edge;
    }

    // if we only update one weight, we don't need to sort the list
    attempts++;

    // keep track of zero edges after sorting once the full list
    if (!Graph->IsEdge(Edge.Val1, Edge.Val2) && Graph->GetEdges() > 1) {
      if (EProb == 0)
        EdgeZero.Add(e);
    }

    // lazy evaluation
    if (e+1 == EdgeGainV.Len() || BestGain >= EdgeGainV[e+1].Val1) {
    CurProb += BestGain;

    if (BestGain == 0)
      return TIntPr(-1, -1);

    EdgeGainV.Del(BestGainIndex);

    // we know the edges in 0 will be in sorted order, so we start from the biggest
    for (int i=EdgeZero.Len()-1; i>=0; i--) {
      if (EdgeZero[i] > BestGainIndex)
        EdgeGainV.Del(EdgeZero[i]-1);
      else
        EdgeGainV.Del(EdgeZero[i]);
    }

    if (EdgeZero.Len() > 2) { attempts -= (EdgeZero.Len()-1); }

    msort = (attempts > 1);

    LastGain = BestGain;

    return BestE;
    }
  }

  printf("Edges exhausted!\n");
  return TIntPr(-1, -1);
}

double TNetInfBs::GetBound(const TIntPr& Edge, double& CurProb) {
  double Bound = 0;
  TFltV Bounds;

  // bound could be computed faster (using lazy evaluation, as in the optimization procedure)
  for (int e=0; e < EdgeGainV.Len(); e++) {
    const TIntPr& EE = EdgeGainV[e].Val2;
    if (EE != Edge && !Graph->IsEdge(EE.Val1, EE.Val2)) {
      const double EProb = GetAllCascProb(EE.Val1, EE.Val2);
      if (EProb > CurProb) Bounds.Add(EProb - CurProb); }
  }

  Bounds.Sort(false);
  for (int i=0; i<Graph->GetEdges() && i<Bounds.Len(); i++) Bound += Bounds[i];

  return Bound;
}

void TNetInfBs::GreedyOpt(const int& MxEdges) {
    double CurProb = GetAllCascProb(-1, -1);
    double LastGain = TFlt::Mx;
    int attempts = 0;
    bool msort = false;

    for (int k = 0; k < MxEdges && EdgeGainV.Len() > 0; k++) {
      const TIntPr BestE = GetBestEdge(CurProb, LastGain, msort, attempts);
      if (BestE == TIntPr(-1, -1)) // if we cannot add more edges, we stop
        break;

      if (CompareGroundTruth) {
        double precision = 0, recall = 0;
        if (PrecisionRecall.Len() > 1) {
          precision = PrecisionRecall[PrecisionRecall.Len()-1].Val2.Val;
          recall = PrecisionRecall[PrecisionRecall.Len()-1].Val1.Val;
        }
        if (GroundTruth->IsEdge(BestE.Val1, BestE.Val2)) {
          recall++;
        } else {
          precision++;
        }

        PrecisionRecall.Add(TPair<TFlt, TFlt>(recall, precision));
      }

      Graph->AddEdge(BestE.Val1, BestE.Val2); // add edge to network

      
      // localized update!
      TIntV &CascsEdge = CascPerEdge.GetDat(BestE); // only check cascades that contain the edge
      for (int c = 0; c < CascsEdge.Len(); c++) {
        CascV[CascsEdge[c]].UpdateProb(BestE.Val1, BestE.Val2, true); // update probabilities
      }

      // some extra info for the added edge
      TInt Vol; TFlt AverageTimeDiff; TFltV TimeDiffs;
      Vol = 0; AverageTimeDiff = 0;
      for (int i=0; i< CascV.Len(); i++) {
        if (CascV[i].IsNode(BestE.Val2) && CascV[i].GetParent(BestE.Val2) == BestE.Val1) {
          Vol += 1; TimeDiffs.Add(CascV[i].GetTm(BestE.Val2)-CascV[i].GetTm(BestE.Val1));
          AverageTimeDiff += TimeDiffs[TimeDiffs.Len()-1]; }
      }
      AverageTimeDiff /= Vol;
      if (TimeDiffs.Len() > 0)
        TimeDiffs.Sort();
      else
        TimeDiffs.Add(0);

      // compute bound only if explicitly required
      EdgeInfoH.AddDat(BestE) = TEdgeInfo(Vol,
                      LastGain,
                      0.0,
                      TimeDiffs[(int)(TimeDiffs.Len()/2)],
                      AverageTimeDiff);
    }

    if (CompareGroundTruth) {
      for (int i=0; i<PrecisionRecall.Len(); i++) {
        PrecisionRecall[i].Val2 = 1.0 - PrecisionRecall[i].Val2/(PrecisionRecall[i].Val2+PrecisionRecall[i].Val1);
        PrecisionRecall[i].Val1 /= (double)GroundTruth->GetEdges();
      }
    }
}

void TNetInfBs::SavePajek(const TStr& OutFNm) {
    TIntSet NIdSet;
    FILE *F = fopen(OutFNm.CStr(), "wt");
    fprintf(F, "*Vertices %d\r\n", NIdSet.Len());
    for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
      const TNodeInfo& I = NI.GetDat();
      fprintf(F, "%d \"%s\" ic Blue x_fact %f y_fact %f\r\n", NI.GetKey().Val,
        I.Name.CStr(), TMath::Mx<double>(log((double)I.Vol)-5,1), TMath::Mx<double>(log((double)I.Vol)-5,1));
    }
    fprintf(F, "*Arcs\r\n");
    for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
      fprintf(F, "%d %d 1\r\n", EI.GetSrcNId(), EI.GetDstNId());
    }
    fclose(F);
}

void TNetInfBs::SavePlaneTextNet(const TStr& OutFNm) {
  TIntSet NIdSet;
  FILE *F = fopen(OutFNm.CStr(), "wt");
  for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    fprintf(F, "%d,%d\r\n", NI.GetKey().Val, NI.GetKey().Val);
  }

  fprintf(F, "\r\n");

  for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    fprintf(F, "%d,%d\r\n", EI.GetSrcNId(), EI.GetDstNId());
  }
  fclose(F);
}

void TNetInfBs::SaveEdgeInfo(const TStr& OutFNm) {
  FILE *F = fopen(OutFNm.CStr(), "wt");

  fprintf(F, "src dst vol marginal_gain median_timediff average_timediff\n");
  for (THash<TIntPr, TEdgeInfo>::TIter EI = EdgeInfoH.BegI(); EI < EdgeInfoH.EndI(); EI++) {
    TEdgeInfo &EdgeInfo = EI.GetDat();
    fprintf(F, "%s/%s/%d/%f/%f/%f\n",
        NodeNmH.GetDat(EI.GetKey().Val1.Val).Name.CStr(), NodeNmH.GetDat(EI.GetKey().Val2.Val).Name.CStr(),
        EdgeInfo.Vol.Val, EdgeInfo.MarginalGain.Val,
        EdgeInfo.MedianTimeDiff.Val,
        EdgeInfo.AverageTimeDiff.Val);
  }
  fclose(F);
}

void TNetInfBs::SaveObjInfo(const TStr& OutFNm) {
  TGnuPlot GnuPlot(OutFNm);

  TFltV Objective;

  for (THash<TIntPr, TEdgeInfo>::TIter EI = EdgeInfoH.BegI(); EI < EdgeInfoH.EndI(); EI++) {
    if (Objective.Len()==0) { Objective.Add(EI.GetDat().MarginalGain); 
    } else {
      Objective.Add(Objective[Objective.Len()-1]+EI.GetDat().MarginalGain);
    }
  }

  GnuPlot.AddPlot(Objective, gpwLinesPoints);
  
  GnuPlot.SavePng();
}

void TNetInfBs::SaveGroundTruth(const TStr& OutFNm) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (TNGraph::TNodeI NI = GroundTruth->BegNI(); NI < GroundTruth->EndNI(); NI++) {
    FOut.PutStr(TStr::Fmt("%d,%d\r\n", NI.GetId(), NI.GetId())); // nodes
  }

  FOut.PutStr("\r\n");

  // write edges to file (not allowing self loops in the network)
  for (TNGraph::TEdgeI EI = GroundTruth->BegEI(); EI < GroundTruth->EndEI(); EI++) {
    // not allowing self loops in the Kronecker network
    if (EI.GetSrcNId() != EI.GetDstNId()) {
      if (Alphas.IsKey(TIntPr(EI.GetSrcNId(), EI.GetDstNId())))
        FOut.PutStr(TStr::Fmt("%d,%d,%f\r\n", EI.GetSrcNId(), EI.GetDstNId(), Alphas.GetDat(TIntPr(EI.GetSrcNId(), EI.GetDstNId())).Val));
      else
        FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
    }
  }
}

void TNetInfBs::SaveCascades(const TStr& OutFNm) {
  TFOut FOut(OutFNm);

  // write nodes to file
  for (TNGraph::TNodeI NI = GroundTruth->BegNI(); NI < GroundTruth->EndNI(); NI++) {
    FOut.PutStr(TStr::Fmt("%d,%d\r\n", NI.GetId(), NI.GetId())); // nodes
  }

  FOut.PutStr("\r\n");

  // write cascades to file
  for (int i=0; i<CascV.Len(); i++) {
    TCascade &C = CascV[i];
    int j = 0;
    for (THash<TInt, THitInfo>::TIter NI = C.NIdHitH.BegI(); NI < C.NIdHitH.EndI(); NI++, j++) {
      if (j > 0)
        FOut.PutStr(TStr::Fmt(",%d,%f", NI.GetDat().NId.Val, NI.GetDat().Tm.Val));
      else
        FOut.PutStr(TStr::Fmt("%d,%f", NI.GetDat().NId.Val, NI.GetDat().Tm.Val));
    }

    if (C.Len() >= 1)
      FOut.PutStr(TStr::Fmt("\r\n"));
  }
}
