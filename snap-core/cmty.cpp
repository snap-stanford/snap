/////////////////////////////////////////////////
// Community detection algorithms
namespace TSnap {


namespace TSnapDetail {

// GIRVAN-NEWMAN algorithm
//	1. The betweenness of all existing edges in the network is calculated first.
//	2. The edge with the highest betweenness is removed.
//	3. The betweenness of all edges affected by the removal is recalculated.
//	4. Steps 2 and 3 are repeated until no edges remain.
//  Girvan M. and Newman M. E. J., Community structure in social and biological networks, Proc. Natl. Acad. Sci. USA 99, 7821-7826 (2002)
// Keep removing edges from Graph until one of the connected components of Graph splits into two.
void CmtyGirvanNewmanStep(PUNGraph& Graph, TIntV& Cmty1, TIntV& Cmty2) {
  TIntPrFltH BtwEH;
  TBreathFS<PUNGraph> BFS(Graph);
  Cmty1.Clr(false);  Cmty2.Clr(false);
  while (true) {
    TSnap::GetBetweennessCentr(Graph, BtwEH);
    BtwEH.SortByDat(false);
    if (BtwEH.Empty()) { return; }
    const int NId1 = BtwEH.GetKey(0).Val1;
    const int NId2 = BtwEH.GetKey(0).Val2;
    Graph->DelEdge(NId1, NId2);
    BFS.DoBfs(NId1, true, false, NId2, TInt::Mx);
    if (BFS.GetHops(NId1, NId2) == -1) { // two components
      TSnap::GetNodeWcc(Graph, NId1, Cmty1);
      TSnap::GetNodeWcc(Graph, NId2, Cmty2);
      return;
    }
  }
}

// Connected components of a graph define clusters
// OutDegH and OrigEdges stores node degrees and number of edges in the original graph
double _GirvanNewmanGetModularity(const PUNGraph& G, const TIntH& OutDegH, const int& OrigEdges, TCnComV& CnComV) {
  TSnap::GetWccs(G, CnComV); // get communities
  double Mod = 0;
  for (int c = 0; c < CnComV.Len(); c++) {
    const TIntV& NIdV = CnComV[c]();
    double EIn = 0, EEIn = 0;
    for (int i = 0; i < NIdV.Len(); i++) {
      TUNGraph::TNodeI NI = G->GetNI(NIdV[i]);
      EIn += NI.GetOutDeg();
      EEIn += OutDegH.GetDat(NIdV[i]);
    }
    Mod += (EIn-EEIn*EEIn / (2.0*OrigEdges));
  }
  if (Mod == 0) { return 0; }
  else { return Mod / (2.0*OrigEdges); }
}

void MapEquationNew2Modules(PUNGraph& Graph, TIntH& Module, TIntFltH& Qi, int a, int b) {
  float InModule = 0.0, OutModule = 0.0, Val;
  int Mds[2] = { a, b };
  for (int i = 0; i<2; i++) {
    InModule = 0.0, OutModule = 0.0;
    if (Qi.IsKey(Mds[i])) {
      int CentralModule = Mds[i];

      //printf("central module: %i\n ",CentralModule);

      TIntV newM;
      for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
        if (Module.GetDat(NI.GetId()) == CentralModule)
          newM.Add(NI.GetId());
        }

      for (int j = 0; j<newM.Len(); j++) {
        //int len1 = newM.Len();

        //int c = CentralModule;
        for (int k = 0; k<Graph->GetNI(newM[j]).GetDeg(); k++) {
          //int len = Graph->GetNI(newM[j]).GetDeg();

          int ids = Graph->GetNI(newM[j]).GetId();
          int idd = Graph->GetNI(newM[j]).GetNbrNId(k);
          int ms = Module.GetDat(ids);
          int md = Module.GetDat(idd);
          //int c = CentralModule;

          if (ms == md) {
            InModule += 1.0;
            //printf("IN: \t%i - %i; moduls: %i - %i\n", ids, idd, ms, md);
          } else {
            OutModule += 1.0;
            //printf("OUT: \t%i - %i; moduls: %i - %i\n", ids, idd, ms, md);
          }
        }
      }
      if (InModule >1) InModule = InModule / 2;

      //printf("\n");

      Val = 0.0;
      if (InModule + OutModule > 0) {
        Val = OutModule / (InModule + OutModule);
      }
      //int control = Mds[i];
      Qi.AddDat(Mds[i], Val);
    } else {
      //int control = Mds[i];
      Qi.AddDat(Mds[i], 0.0);
    }
  }
}

double Equation(TIntFltH& PAlpha, double& SumPAlphaLogPAlpha, TIntFltH& Qi){
  double SumPAlpha = 1.0, SumQi = 0.0, SumQiLogQi = 0.0;
  double SumQiSumPAlphaLogQiSumPAlpha = 0.0, logqi = 0.0, qi = 0.0;
  for (int i = 0; i<Qi.Len(); i++) {
    SumQi += Qi[i];
    qi = Qi[i];
    if (qi != 0) {
      logqi = log(qi);
    } else {
      logqi = 0;
    }
    SumQiLogQi += Qi[i] * logqi;
    SumQiSumPAlphaLogQiSumPAlpha += (Qi[i] + SumPAlpha)*log(Qi[i] + SumPAlpha);
  }
  return (SumQi*log(SumQi) - 2 * SumQiLogQi - SumPAlphaLogPAlpha +
          SumQiSumPAlphaLogQiSumPAlpha);
}

bool edgeIntersect(PNGraph& graph, TIntV& a, TIntV& b) {
  for (int i = 0; i<a.Len(); i++) {
    for (int j = 0; j<b.Len(); j++) {
      if (graph->IsEdge(a[i], b[j]))
        return true;
    }
  }

  return false;
}

int vectorIntersect(TIntV& a, TIntV& b) {
  int count = 0;
  for (int i = 0; i<a.Len(); i++) {
    for (int j = 0; j<b.Len(); j++) {
      if (a[i] == b[j])
        count++;
    }
  }

  return count;
}

bool inComp(PNGraph& g1, PNGraph& Graph, TIntH& inCompCount, int id, int neigh) {
  bool out = true;

  int inCompN = 0;
  int inComp = 0;

  if (g1->IsNode(id) && g1->IsNode(neigh)) {
    int deg = g1->GetNI(id).GetDeg();
    int neighDeg = g1->GetNI(neigh).GetDeg();


    if (inCompCount.IsKey(id)) {
      inComp = inCompCount.GetDat(id);
    }
    if (inCompCount.IsKey(neigh)) {
      inCompN = inCompCount.GetDat(neigh);
    }

    if (inCompN < neighDeg && inComp < deg && (!g1->IsNode(neigh) || Graph->GetNI(neigh).GetDeg() - neighDeg == 0)) {
      inCompCount.AddDat(neigh, ++inCompN);
      inCompCount.AddDat(id, ++inComp);
      out = true;
    } else {
      out = false;
    }
  }
  return out;
}

void transitiveTransform(TIntV& a, TIntV& b) {
  for (int i = 0; i < a.Len(); i++) {
    bool diff = false;
    for (int j = 0; j < b.Len(); j++) {
      if (a[i] == a[j]) {
        diff = true;
        break;
      }
    }
    if (!diff) {
      b.Add(a[i]);
      break;
    }
  }
}

bool chekIfCrossing(TIntV& a, TIntH& t, int f, int l, int TP) {
  bool after = false;
  bool before = false;
  for (int i = 0; i < a.Len(); i++) {
    if (t.GetDat(a[i]) < TP)
      before = true;
    if (t.GetDat(a[i]) > TP)
      after = true;
  }

  if (TP == f)
    before = true;

  if (TP == l)
    after = true;

  return (after && before);
}

double InfomapOnlineIncrement(PUNGraph& Graph, int n1, int n2, TIntFltH& PAlpha, double& SumPAlphaLogPAlpha, TIntFltH& Qi, TIntH& Module, int& Br) {
  // NOW NEW stuff add another additional iteration:

  bool n1new = false;
  bool n2new = false;

  // add edge
  if (!Graph->IsNode(n1)){
    Graph->AddNode(n1);
    n1new = true;
  }

  if (!Graph->IsNode(n2)) {
    Graph->AddNode(n2);
    n2new = true;
  }

  Graph->AddEdge(n1, n2);

  int e = Graph->GetEdges();

  // get previous alpha for 27 
  double oldAlphaN1 = 0.0;
  double oldAlphaN2 = 0.0;

  if (!n1new)
    oldAlphaN1 = PAlpha.GetDat(n1);

  if (!n2new)
    oldAlphaN2 = PAlpha.GetDat(n2);

  // update alpha for 27
  TUNGraph::TNodeI node = Graph->GetNI(n1);
  int nodeDeg = node.GetDeg();
  float d = ((float)nodeDeg / (float)(2 * e));
  PAlpha.AddDat(n1, d);

  //update alphasum
  SumPAlphaLogPAlpha = SumPAlphaLogPAlpha - oldAlphaN1 + d*log(d);

  if (n1new) {
    Module.AddDat(n1, Br);
    Qi.AddDat(Br, 1.0);
    Br++;
  }

  // update alpha for 28
  node = Graph->GetNI(n2);
  nodeDeg = node.GetDeg();
  d = ((float)nodeDeg / (float)(2 * e));
  PAlpha.AddDat(n2, d);

  //update alphasum
  SumPAlphaLogPAlpha = SumPAlphaLogPAlpha - oldAlphaN2 + d*log(d);

  //add module
  if (n2new) {
    Module.AddDat(n2, Br);
    Qi.AddDat(Br, 1.0);
    Br++;
  }

  // Start

  double MinCodeLength = TSnapDetail::Equation(PAlpha, SumPAlphaLogPAlpha, Qi);
  double PrevIterationCodeLength = 0.0;

  do {
    PrevIterationCodeLength = MinCodeLength;
    int id[2] = { n1, n2 };
    for (int k = 0; k<2; k++) {
      for (int i = 0; i<Graph->GetNI(id[k]).GetDeg(); i++) {

        int OldModule = Module.GetDat(id[k]);
        int NewModule = Module.GetDat(Graph->GetNI(id[k]).GetNbrNId(i));

        Module.AddDat(id[k], NewModule);

        TSnapDetail::MapEquationNew2Modules(Graph, Module, Qi, OldModule, NewModule);
        double NewCodeLength = TSnapDetail::Equation(PAlpha, SumPAlphaLogPAlpha, Qi);
        if (NewCodeLength<MinCodeLength) {
          MinCodeLength = NewCodeLength;
          OldModule = NewModule;
        }
        else {
          Module.AddDat(id[k], OldModule);
        }
      }
    }
  } while (MinCodeLength<PrevIterationCodeLength);

  return MinCodeLength;
}

} // namespace TSnapDetail

// Maximum modularity clustering by Girvan-Newman algorithm (slow)
//  Girvan M. and Newman M. E. J., Community structure in social and biological networks, Proc. Natl. Acad. Sci. USA 99, 7821-7826 (2002)
double CommunityGirvanNewman(PUNGraph& Graph, TCnComV& CmtyV) {
  TIntH OutDegH;
  const int NEdges = Graph->GetEdges();
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    OutDegH.AddDat(NI.GetId(), NI.GetOutDeg());
  }
  double BestQ = -1; // modularity
  TCnComV CurCmtyV;
  CmtyV.Clr();
  TIntV Cmty1, Cmty2;
  while (true) {
    TSnapDetail::CmtyGirvanNewmanStep(Graph, Cmty1, Cmty2);
    const double Q = TSnapDetail::_GirvanNewmanGetModularity(Graph, OutDegH, NEdges, CurCmtyV);
    //printf("current modularity: %f\n", Q);
    if (Q > BestQ) {
      BestQ = Q; 
      CmtyV.Swap(CurCmtyV);
    }
    if (Cmty1.Len() == 0 || Cmty2.Len() == 0) { break; }
  }
  return BestQ;
}

// Rosvall-Bergstrom community detection algorithm based on information theoretic approach.
// See: Rosvall M., Bergstrom C. T., Maps of random walks on complex networks reveal community structure, Proc. Natl. Acad. Sci. USA 105, 1118-1123 (2008)
double Infomap(PUNGraph& Graph, TCnComV& CmtyV){

  TIntFltH PAlpha; // probability of visiting node alpha
  TIntH Module; // module of each node
  TIntFltH Qi; // probability of leaving each module

  double SumPAlphaLogPAlpha = 0.0;
  int Br = 0;
  const int e = Graph->GetEdges();

  // initial values
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    int nodeId = NI.GetId();
    int nodeDeg = NI.GetDeg();
    float d = ((float)nodeDeg / (float)(2 * e));
    PAlpha.AddDat(nodeId, d);
    SumPAlphaLogPAlpha += d*log(d);
    Module.AddDat(nodeId, Br);
    Qi.AddDat(Br, 1.0);
    Br += 1;
  }

  double MinCodeLength = TSnapDetail::Equation(PAlpha, SumPAlphaLogPAlpha, Qi);
  double NewCodeLength, PrevIterationCodeLength = 0.0;
  int OldModule, NewModule;

  TIntV nodes;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++)
    nodes.Add(NI.GetId());

  do {
    PrevIterationCodeLength = MinCodeLength;
    TRnd rnd;
    rnd.Randomize();
    nodes.Shuffle(rnd);
    for (int ndcounter = 0; ndcounter<nodes.Len(); ndcounter++) {
      MinCodeLength = TSnapDetail::Equation(PAlpha, SumPAlphaLogPAlpha, Qi);
      int nodeId = nodes[ndcounter];
      TUNGraph::TNodeI NI = Graph->GetNI(nodeId);
      for (int i = 0; i<NI.GetDeg(); i++) {

        OldModule = Module.GetDat(nodeId);
        NewModule = Module.GetDat(NI.GetNbrNId(i));

        if (OldModule != NewModule){

          Module.AddDat(nodeId, NewModule);

          TSnapDetail::MapEquationNew2Modules(Graph, Module, Qi, OldModule, NewModule);
          NewCodeLength = TSnapDetail::Equation(PAlpha, SumPAlphaLogPAlpha, Qi);
          if (NewCodeLength<MinCodeLength) {
            MinCodeLength = NewCodeLength;
            OldModule = NewModule;
          }
          else {
            Module.AddDat(nodeId, OldModule);
          }
        }
      }
    }
  } while (MinCodeLength<PrevIterationCodeLength);

  Module.SortByDat(true);

  int Mod = -1;
  for (int i = 0; i<Module.Len(); i++) {
    if (Module[i]>Mod){
      Mod = Module[i];
      TCnCom t;
      for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
        if (Module.GetDat(NI.GetId()) == Mod)
          t.Add(NI.GetId());
      }
      CmtyV.Add(t);
    }
  }

  return MinCodeLength;
}

double InfomapOnline(PUNGraph& Graph, int n1, int n2, TIntFltH& PAlpha, double& SumPAlphaLogPAlpha, TIntFltH& Qi, TIntH& Module, int& Br, TCnComV& CmtyV) {

  double MinCodeLength = TSnapDetail::InfomapOnlineIncrement(Graph, n1, n2, PAlpha, SumPAlphaLogPAlpha, Qi, Module, Br);

  Module.SortByDat(true);

  int Mod = -1;
  for (int i = 0; i<Module.Len(); i++) {
    if (Module[i]>Mod){
      Mod = Module[i];
      TCnCom t;
      for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
        if (Module.GetDat(NI.GetId()) == Mod)
          t.Add(NI.GetId());
      }
      CmtyV.Add(t);
    }
  }

  return MinCodeLength;
}

void CmtyEvolutionFileBatchV(TStr InFNm, TIntIntVH& sizesContV, TIntIntVH& cContV, TIntIntVH& edges, double alpha, double beta, int CmtyAlg) {
  TIntIntHH sizesCont;
  TIntIntHH cCont;
  CmtyEvolutionFileBatch(InFNm, sizesCont, cCont, edges, alpha, beta, CmtyAlg);

  TIntV uniqueId;
  for (int i = 0; i < cCont.Len(); i++){
    for (THashKeyDatI<TInt, TInt> it = cCont[i].BegI(); !it.IsEnd(); it++){
      if (!uniqueId.IsIn(it.GetKey()))
        uniqueId.Add(it.GetKey());
    }
  }

  for (int j = 0; j<uniqueId.Len(); j++)
  {
    TIntV cV;
    for (int i = 0; i<cCont.Len(); i++)
    {
      if (cCont[i].IsKey(uniqueId[j]))
        cV.Add(cCont[i].GetDat(uniqueId[j]));
      else
        cV.Add(-1);
    }
    cContV.AddDat(uniqueId[j], cV);
  }

  TIntV uniqueC;
  for (int i = 0; i < sizesCont.Len(); i++){
    for (THashKeyDatI<TInt, TInt> it = sizesCont[i].BegI(); !it.IsEnd(); it++){
      if (!uniqueC.IsIn(it.GetKey()))
        uniqueC.Add(it.GetKey());
    }
  }

  for (int j = 0; j<uniqueC.Len(); j++)
  {
    TIntV cV;
    for (int i = 0; i<sizesCont.Len(); i++)
    {
      if (sizesCont[i].IsKey(uniqueC[j]))
        cV.Add(sizesCont[i].GetDat(uniqueC[j]));
      else
        cV.Add(0);
    }
    sizesContV.AddDat(uniqueC[j], cV);
  }

}

void CmtyEvolutionFileBatch(TStr InFNm, TIntIntHH& sizesCont, TIntIntHH& cCont, TIntIntVH& edges, double alpha, double beta, int CmtyAlg) {


  // reading folder with networks and calculating core/periphery
  int br = 0;
  TIntIntH prev;
  TIntH prev_sizes;

  TSsParser Ss(InFNm, ssfWhiteSep, true, false, true);
  Ss.Next();
  //int internal_year_counter = 0;
  // variable for delimiter between networks
  TStr Marker;
  // defining variables for node ids and starting year
  int SrcNId, DstNId; // , t = 1970;

  // temporal container for edges
  TIntIntVH edges_;

  while (!Ss.Eof()) {

    //printf("%i\n", t);
    Marker = Ss.GetLnStr();
    // get the year from the network seperator
    //t = Marker.GetSubStr(1, 4).GetInt();

    if (Marker.GetCh(0) == '#'){

      Ss.Next();
      PUNGraph Graph = PUNGraph::TObj::New();
      do{
        if (!Ss.GetInt(0, SrcNId) || !Ss.GetInt(1, DstNId)) {
          if (!Ss.Eof()){
            Ss.Next();
            if (!Ss.Eof())
              Marker = Ss.GetLnStr();
          }
          continue;
        }
        if (!Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
        if (!Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
        Graph->AddEdge(SrcNId, DstNId);
        Ss.Next();
        if (!Ss.Eof())
          Marker = Ss.GetLnStr();
      } while (Marker.GetCh(0) != '#' && !Ss.Eof());


      if (Graph->GetNodes()>0) {
        // WORK

        TSnap::DelSelfEdges(Graph);
        TCnComV CmtyV;
        //double Q = 0.0;
        TStr CmtyAlgStr;
        if (CmtyAlg == 1) {
          CmtyAlgStr = "Girvan-Newman";
          //Q = TSnap::CommunityGirvanNewman(Graph, CmtyV);
        }
        else if (CmtyAlg == 2) {
          CmtyAlgStr = "Clauset-Newman-Moore";
          //Q = TSnap::CommunityCNM(Graph, CmtyV);
        }
        else if (CmtyAlg == 3) {
          CmtyAlgStr = "Infomap";
          //Q = TSnap::Infomap(Graph, CmtyV);
        }
        else { Fail; }

        TIntIntHH distCont;

        if (br == 0) {
          prev.Clr();
          //int size = 0;
          for (int c = 0; c < CmtyV.Len(); c++) {
            for (int i = 0; i < CmtyV[c].Len(); i++){
              prev.AddDat(CmtyV[c][i].Val, c);
            }
            //int s = CmtyV[c].Len();
            prev_sizes.AddDat(c, CmtyV[c].Len());
          }
        }
        else {

          // containers for statistics

          //TIntFltHH stat1;
          //TIntIntHH stat2;
          TIntH dist;
          TIntH map;

          int first_new_c_id = -1;

          // getting first free id for a new community
          for (THashKeyDatI<TInt, TInt> it = prev_sizes.BegI(); !it.IsEnd(); it++)
            if (it.GetKey() > first_new_c_id)
              first_new_c_id = it.GetKey();
          if (CmtyV.Len() - 1>first_new_c_id)
            first_new_c_id = CmtyV.Len() - 1;
          first_new_c_id++;

          for (int c = 0; c < CmtyV.Len(); c++) {

            TIntV stat;
            TIntFltH statH1;
            TIntFltH statH2;

            // initialize distributions to 0
            for (THashKeyDatI<TInt, TInt> it = prev_sizes.BegI(); !it.IsEnd(); it++)
              dist.AddDat(it.GetKey(), 0);
            //for new nodes
            dist.AddDat(-1, 0);

            for (int i = 0; i < CmtyV[c].Len(); i++) {
              int id = CmtyV[c][i].Val;
              int prev_comm = -1;
              if (prev.IsKey(id))
                prev_comm = prev.GetDat(CmtyV[c][i].Val);
              stat.Add(prev_comm);
              int pre_val = dist.GetDat(prev_comm);
              dist.AddDat(prev_comm, pre_val + 1);
            }

            double sumstat2 = 0;
            for (THashKeyDatI<TInt, TInt> it = dist.BegI(); !it.IsEnd(); it++) {

              int k = it.GetKey();
              int d = it.GetDat();
              if (d > 0){
                if (prev_sizes.IsKey(it.GetKey())){

                  double stat1_ = (double)d / (double)prev_sizes.GetDat(k);
                  statH1.AddDat(k, stat1_);
                }
                double stat2_ = (double)d / (double)CmtyV[c].Len();
                statH2.AddDat(k, stat2_);
                sumstat2 += stat2_;

                TIntV edge;
                edge.Add(k);
                edge.Add(c);
                edge.Add(d);
                edge.Add(br - 1);
                edge.Add(br);
                edges_.AddDat(edges_.Len() + 1, edge);
              }

              // adding edges between two communities in two neighbouring time points;


              if (sumstat2 > 0.98) break;
            }

            int n_of_c_greater_than_half = 0;
            int id_of_c_greater_than_half = -1;
            TIntV ids_of_c_greater_than_half;

            for (THashKeyDatI<TInt, TFlt> it = statH1.BegI(); !it.IsEnd(); it++){
              if (it.GetDat()>alpha){
                id_of_c_greater_than_half = it.GetKey();
                ids_of_c_greater_than_half.Add(it.GetKey());
                n_of_c_greater_than_half++;
              }
            }

            // if this community is build of majority of one previous community and the other parts of the community are fractions of other communities smaller than half, the new community gets its label 
            if (n_of_c_greater_than_half == 1){
              map.AddDat(c, id_of_c_greater_than_half);
            }
            else{
              int h2part_id = -2;
              for (int i = 0; i<ids_of_c_greater_than_half.Len(); i++){
                double H2 = statH2.GetDat(ids_of_c_greater_than_half[i]);
                if (H2>beta){
                  h2part_id = ids_of_c_greater_than_half[i];
                }
              }
              if (h2part_id != -2)
                map.AddDat(c, h2part_id);
              else{
                map.AddDat(c, first_new_c_id);
                first_new_c_id++;
              }
            }

            distCont.AddDat(c, dist);

            //stat1.AddDat(c,statH1);
            //stat2.AddDat(c,statH2);

          }


          prev.Clr();
          prev_sizes.Clr();
          for (int c = 0; c < CmtyV.Len(); c++){
            for (int i = 0; i < CmtyV[c].Len(); i++){
              prev.AddDat(CmtyV[c][i].Val, map.GetDat(c));
            }
            //int s = CmtyV[c].Len();
            prev_sizes.AddDat(map.GetDat(c), CmtyV[c].Len());
          }

          // filing the edges container - the key thing is the map(c)
          for (THashKeyDatI<TInt, TIntV> it = edges_.BegI(); !it.IsEnd(); it++){
            TIntV edgesV;
            int a = it.GetDat()[0];
            int b = it.GetDat()[1];
            int v = it.GetDat()[2];
            int d = it.GetDat()[3];
            int e = it.GetDat()[4];
            edgesV.Add(map.GetDat(b));
            edgesV.Add(a);
            edgesV.Add(v);
            edgesV.Add(d);
            edgesV.Add(e);
            if (a != -1)
              edges.AddDat(edges.Len(), edgesV);
          }
          edges_.Clr();


        }

        sizesCont.AddDat(br, prev_sizes);
        cCont.AddDat(br, prev);
        br++;
        // WORK - END
      }
    }
    else Ss.Next();
  }

}

void CmtyEvolutionJson(TStr& Json, TIntIntVH& sizesContV, TIntIntVH& cContV, TIntIntVH& edges){
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // This function creates a JSON string with communities and edges for community evolution visualization using D3.js
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // writing json label for edges
  Json.InsStr(Json.Len(), "{\n\"edges\":[\n");

  TInt br = 0;
  // iterating hash of vector of edges and writing into string 
  for (THashKeyDatI<TInt, TIntV> it = edges.BegI(); !it.IsEnd(); it++)
  {
    // first node
    TInt n1 = it.GetDat()[1];
    // second node
    TInt n2 = it.GetDat()[0];
    // edge weight
    TInt w = it.GetDat()[2];
    // start time point
    TInt t0 = it.GetDat()[3];
    // end time point
    TInt t1 = it.GetDat()[4];

    if (br>0)
      Json.InsStr(Json.Len(), ",");

    // writing to string
    Json.InsStr(Json.Len(), "{\"n1\":"); Json.InsStr(Json.Len(), n1.GetStr());
    Json.InsStr(Json.Len(), ", \"n2\":"); Json.InsStr(Json.Len(), n2.GetStr());
    Json.InsStr(Json.Len(), ", \"w\":"); Json.InsStr(Json.Len(), w.GetStr());
    Json.InsStr(Json.Len(), ", \"t0\":"); Json.InsStr(Json.Len(), t0.GetStr());
    Json.InsStr(Json.Len(), ", \"t1\":"); Json.InsStr(Json.Len(), t1.GetStr());
    Json.InsStr(Json.Len(), " }\n");
    br++;
  }

  // json label for communities
  Json.InsStr(Json.Len(), "],\n\"communities\":[\n");

  br = 0;
  // printing communities into json file 
  for (int i = 0; i < sizesContV[0].Len(); i++)
  {
    for (THashKeyDatI<TInt, TIntV> it = sizesContV.BegI(); !it.IsEnd(); it++)
    {
      // id of community
      TInt id = it.GetKey();
      // community size
      TInt size = it.GetDat()[i];
      // time
      TInt j = i;

      // if the community has size greater than 0, output it to json string
      if (size > 0) {
        if (br>0)
          Json.InsStr(Json.Len(), ",");

        TInt size = it.GetDat()[i];
        Json.InsStr(Json.Len(), "{\"id\":"); Json.InsStr(Json.Len(), id.GetStr());
        Json.InsStr(Json.Len(), ", \"size\":"); Json.InsStr(Json.Len(), size.GetStr());
        Json.InsStr(Json.Len(), ", \"t\":"); Json.InsStr(Json.Len(), j.GetStr());
        Json.InsStr(Json.Len(), " }\n");

        br++;
      }
    }
  }

  // printing communities into json file - alternative ordering
  /*
  for (THashKeyDatI<TInt, TIntV> it = sizesContV.BegI();  !it.IsEnd(); it++)
  {
  TInt id = it.GetKey();
  int len = it.GetDat().Len();
  for (int i=0; i < it.GetDat().Len(); i++)
  {
  TInt size = it.GetDat()[i];
  TInt j = i;
  if (size > 0) {

  if(br>0)
  Json.InsStr(Json.Len(),",");

  TInt size = it.GetDat()[i];

  Json.InsStr(Json.Len(),"{\"id\":"); Json.InsStr(Json.Len(),id.GetStr());
  Json.InsStr(Json.Len(),", \"size\":"); Json.InsStr(Json.Len(),size.GetStr());
  Json.InsStr(Json.Len(),", \"t\":"); Json.InsStr(Json.Len(),j.GetStr());
  Json.InsStr(Json.Len()," }\n");

  br++;

  }

  }
  }
  */

  Json.InsStr(Json.Len(), "]\n}");

}

TStr CmtyTest(TStr InFNm, int CmtyAlg){

  TIntIntVH sizesContV;
  TIntIntVH cContV;
  TIntIntVH edges;
  double alpha = 0.5;
  double beta = 0.75;
  CmtyEvolutionFileBatchV(InFNm, sizesContV, cContV, edges, alpha, beta, CmtyAlg);
  TStr out;
  //int a = sizesContV.Len();
  //int b = cContV.Len();
  //int c = edges.Len();
  CmtyEvolutionJson(out, sizesContV, cContV, edges);

  return out;
}

void ReebSimplify(PNGraph& Graph, TIntH& t, int e, PNGraph& gFinal, TIntH& tFinal, bool collapse) {
  TIntIntVH components;
  TIntIntVH ct;

  int newId = 0; //get first new free id;

  // gett first and last t
  int first = 429496729;
  int last = -1;

  // smarter way of determining focus time points
  TIntV timePoints;

  // get first and last time point
  for (THashKeyDatI<TInt, TInt> it = t.BegI(); !it.IsEnd(); it++) {
    if (it.GetDat()<first)
      first = it.GetDat();
    if (it.GetDat()>last)
      last = it.GetDat();
  }

  // adding focus timepoints
  // this can be put in the previous (first, last time point detection) iteration if breaking borders is not an issue
  for (THashKeyDatI<TInt, TInt> it = t.BegI(); !it.IsEnd(); it++) {
    if (it.GetDat() - (e / 2) >= first)
      timePoints.Add(it.GetDat() - (e / 2) /*- 0.1*/);
    timePoints.Add(it.GetDat());
    if (it.GetDat() + (e / 2) <= last)
      timePoints.Add(it.GetDat() + (e / 2) /*+ 0.1*/);
  }
  

  //iterate each time point
  for (int i = 0; i<timePoints.Len(); i++) {

    int focusTimePoint = timePoints[i];

    TIntV fnodes; // all the nodes int the focus in that step

    // getting nodes in focus -- in epsilon
    for (THashKeyDatI<TInt, TInt> it = t.BegI(); !it.IsEnd(); it++) {
      if ((it.GetDat() <= focusTimePoint + (e / 2)) && (it.GetDat() >= focusTimePoint - (e / 2)))
        fnodes.Add(it.GetKey());
    }

    // create graph from nodes in focus
    PNGraph g1 = TNGraph::New();
    for (int i = 0; i<fnodes.Len(); i++) {
      if (!g1->IsNode(fnodes[i]))
        g1->AddNode(fnodes[i]);
      // lower star
      for (int j = 0; j<Graph->GetNI(fnodes[i]).GetInDeg(); j++) {
        int NeighId = Graph->GetNI(fnodes[i]).GetInNId(j);
        if (t.GetDat(NeighId)<focusTimePoint - (e / 2)) {

        }
        else {
          if (!g1->IsNode(NeighId))
            g1->AddNode(NeighId);
          g1->AddEdge(NeighId, fnodes[i]);
        }
      }
      // upper star
      for (int j = 0; j<Graph->GetNI(fnodes[i]).GetOutDeg(); j++) {
        int NeighId = Graph->GetNI(fnodes[i]).GetOutNId(j);
        if (t.GetDat(NeighId)>focusTimePoint + (e / 2)) {

        }
        else {
          if (!g1->IsNode(NeighId))
            g1->AddNode(NeighId);
          g1->AddEdge(fnodes[i], NeighId);
        }
      }
    }

    // getting results from commponents detection and recording elements of components and timestamps of components
    TCnComV CnComV;
    GetWccs(g1, CnComV);
    TIntV communitiesAtT;
    for (int cc = 0; cc < CnComV.Len(); cc++) {
      components.AddDat(newId, CnComV[cc].NIdV);
      communitiesAtT.Add(newId);
      newId++;
    }
    if (CnComV.Len() > 0)
      ct.AddDat(focusTimePoint, communitiesAtT);
  } // end iterate each node

  // connecting neighbouring components
  THashKeyDatI<TInt, TIntV> it = ct.BegI();
  THashKeyDatI<TInt, TIntV> prelast = ct.EndI()--;
  prelast--;
  while (it < prelast) {
    TIntV cms0;
    TIntV cms1;
    int focusTimePoint;
    int focusTimePoint1;
    focusTimePoint = it.GetKey();
    cms0 = it.GetDat();
    it++;
    focusTimePoint1 = it.GetKey();
    cms1 = it.GetDat();
    if (cms0.Len()>0 && cms1.Len() > 0) {
      for (int i = 0; i < cms0.Len(); i++) {
        for (int j = 0; j < cms1.Len(); j++) {
          TIntV ids0 = components.GetDat(cms0[i]);
          TIntV ids1 = components.GetDat(cms1[j]);
          if (ids0.IntrsLen(ids1) > 0 || TSnapDetail::edgeIntersect(Graph, ids0, ids1)) {
            if (!gFinal->IsNode(cms0[i])) {
              gFinal->AddNode(cms0[i]);
              tFinal.AddDat(cms0[i], focusTimePoint);
            }
            if (!gFinal->IsNode(cms1[j])) {
              gFinal->AddNode(cms1[j]);
              tFinal.AddDat(cms1[j], focusTimePoint1);
            }
            gFinal->AddEdge(cms0[i], cms1[j]);
          }
        }
      }
    }
  }// end connecting components 

  // collapsing chains
  if (collapse) {
    for (TNGraph::TNodeI NI = gFinal->BegNI(); NI < gFinal->EndNI(); NI++) {
      if (NI.GetInDeg() == 1 && NI.GetOutDeg() == 1)
        if (gFinal->GetNI(NI.GetInNId(0)).GetOutDeg() == 1 && gFinal->GetNI(NI.GetOutNId(0)).GetInDeg() == 1)
        {
        gFinal->AddEdge(NI.GetInNId(0), NI.GetOutNId(0));
        gFinal->DelEdge(NI.GetInNId(0), NI.GetId());
        tFinal.DelKey(NI.GetId());
        gFinal->DelNode(NI.GetId());
        }
    }
  }// end collapsing

}

void ReebRefine(PNGraph& Graph, TIntH& t, int e, PNGraph& gFinal, TIntH& tFinal, bool collapse) {
  TIntIntVH components;
  TIntIntVH ct;

  int newId = 0; //get first new free id;

  // gett first and last t
  int first = 429496729;
  int last = -1;

  // smarter way of determining focus time points
  TIntV timePoints;

  // get first and last time point
  for (THashKeyDatI<TInt, TInt> it = t.BegI(); !it.IsEnd(); it++) {
    if (it.GetDat() < first)
      first = it.GetDat();
    if (it.GetDat() > last)
      last = it.GetDat();
  }

  // adding focus timepoints
  // this can be put in the previous (first, last time point detection) iteration if breaking borders is not an issue
  for (THashKeyDatI<TInt, TInt> it = t.BegI(); !it.IsEnd(); it++) {
    if (it.GetDat() - (e / 2) >= first)
      timePoints.Add(it.GetDat() - (e / 2) /*- 0.1*/);
    timePoints.Add(it.GetDat());
    if (it.GetDat() + (e / 2) <= last)
      timePoints.Add(it.GetDat() + (e / 2) /*+ 0.1*/);
  }

  TIntV timePointsUnique;
  int prevtp = -1;
  //get unique time points
  for (int i = 0; i < timePoints.Len(); i++){
    if (timePoints[i] > prevtp)
      timePointsUnique.Add(timePoints[i]);
    prevtp = timePoints[i];
  }

  timePoints.Clr();
  timePoints = timePointsUnique;

  //iterate each time point
  for (int i = 0; i < timePoints.Len(); i++) {

    int focusTimePoint = timePoints[i];

    TIntV fnodes; // all the nodes int the focus in that step

    // getting nodes in focus -- in epsilon
    for (THashKeyDatI<TInt, TInt> it = t.BegI(); !it.IsEnd(); it++) {
      if ((it.GetDat() <= focusTimePoint + (e / 2)) && (it.GetDat() >= focusTimePoint - (e / 2)))
        fnodes.Add(it.GetKey());
    }

    // create graph from nodes in focus
    PNGraph g1 = TNGraph::New();
    for (int i = 0; i < fnodes.Len(); i++) {
      if (!g1->IsNode(fnodes[i]))
        g1->AddNode(fnodes[i]);
      // lower star
      for (int j = 0; j < Graph->GetNI(fnodes[i]).GetInDeg(); j++) {
        int NeighId = Graph->GetNI(fnodes[i]).GetInNId(j);
        if (t.GetDat(NeighId) < focusTimePoint - (e / 2)) {

        }
        else {
          if (!g1->IsNode(NeighId))
            g1->AddNode(NeighId);
          g1->AddEdge(NeighId, fnodes[i]);
        }
      }
      // upper star
      for (int j = 0; j < Graph->GetNI(fnodes[i]).GetOutDeg(); j++) {
        int NeighId = Graph->GetNI(fnodes[i]).GetOutNId(j);
        if (t.GetDat(NeighId) > focusTimePoint + (e / 2)) {

        }
        else {
          if (!g1->IsNode(NeighId))
            g1->AddNode(NeighId);
          g1->AddEdge(fnodes[i], NeighId);
        }
      }
    }

    // getting results from commponents detection and recording elements of components and timestamps of components
    TIntH inCompCount;
    TIntIntVH comps;
    int compBr = 0;
    TIntH nn_nodes;

    int FTP = focusTimePoint;
    TIntH TEdges;

    for (TNGraph::TNodeI NI = g1->BegNI(); NI < g1->EndNI(); NI++) {

      
      int FTPNode = NI.GetId();
      TNGraph::TNodeI GNI = Graph->GetNI(FTPNode);
      int FI, FO, RI, RO, I, O;

      RI = NI.GetInDeg();
      RO = NI.GetOutDeg();

      FI = Graph->GetNI(FTPNode).GetInDeg() - RI;
      FO = Graph->GetNI(FTPNode).GetOutDeg() - RO;

      if (focusTimePoint + (e / 2) == t.GetDat(NI.GetId())) { // if its on the right edge only in degree is observed
        RO = FO = 0;
      }
      if (focusTimePoint - (e / 2) == t.GetDat(NI.GetId())) { // if its on the left edge only out degree is observed
        RI = FI = 0;
      }

      I = RI + FI;
      O = RO + FO;

      // counting edges imidiately after time point
      int temp = 0;
      if (TEdges.IsKey(FTP))
        temp = TEdges.GetDat(FTP);
      TEdges.AddDat(FTP, O + temp);

      // FIND ELEMENTS

      // n - n,
      if (I > 1 && O > 1) {
        // number of nodes is in our out degree
        int nn = I;
        if (O > I)
          nn = O;

        TIntV nds;
        nds.Add(FTPNode);
        for (int i = 0; i < I; i++) {
          nds.Add(GNI.GetInNId(i));
        }

        for (int i = 0; i < O; i++) {
          nds.Add(GNI.GetOutNId(i));
        }

        for (int j = 0; j < nn; j++) {
          nn_nodes.AddDat(compBr);
          comps.AddDat(compBr, nds);
          compBr++;
        }
      }
      
      // 1 - n
      else if (I == 1 && O > 1) {
        for (int i = 0; i < O; i++) {
          TIntV nds;
          nds.Add(FTPNode);
          nds.Add(GNI.GetInNId(0));
          nds.Add(GNI.GetOutNId(i));
          comps.AddDat(compBr, nds);
          compBr++;
        }
      }

      // n - 1
      else if (I > 1 && O == 1) {
        for (int i = 0; i < I; i++) {
          TIntV nds;
          nds.Add(FTPNode);
          nds.Add(GNI.GetOutNId(0));
          nds.Add(GNI.GetInNId(i));
          comps.AddDat(compBr, nds);
          compBr++;
        }
      }

      // 0 - n
      else if (I == 0 && O > 1) {
        for (int i = 0; i < O; i++) {
          TIntV nds;
          nds.Add(FTPNode);
          nds.Add(GNI.GetOutNId(i));
          comps.AddDat(compBr, nds);
          compBr++;
        }
      }

      // n - 0
      else if (I > 1 && O == 0) {
        for (int i = 0; i < I; i++) {
          TIntV nds;
          nds.Add(FTPNode);
          nds.Add(GNI.GetInNId(i));
          comps.AddDat(compBr, nds);
          compBr++;
        }
      }

      // 1 - 1
      else if (I == 1 && O == 1) {
        TIntV nds;
        nds.Add(FTPNode);
        nds.Add(GNI.GetOutNId(0));
        nds.Add(GNI.GetInNId(0));
        comps.AddDat(compBr, nds);
        compBr++;
      }

      // 0 - 1
      else if (I == 0 && O == 1) {
        TIntV nds;
        nds.Add(FTPNode);
        nds.Add(GNI.GetOutNId(0));
        comps.AddDat(compBr, nds);
        compBr++;
      }

      // 1 - 0
      else if (I == 1 && O == 0) {
        TIntV nds;
        nds.Add(FTPNode);
        nds.Add(GNI.GetInNId(0));
        comps.AddDat(compBr, nds);
        compBr++;
      }

      

    } // end iterate each node

    // connecting inside of epsilon

    TIntIntVH elements;
    TIntH banned;
    for (int cc0 = 0; cc0 < comps.Len(); cc0++) {
      for (int cc1 = cc0; cc1 < comps.Len(); cc1++) {
        int smaller = comps[cc0].Len();
        int smaller_id = cc0;
        if (cc0 != cc1) {
          if (comps[cc1].Len() < smaller) {
            smaller = comps[cc1].Len();
            smaller_id = cc1;
          }
          int vi = TSnapDetail::vectorIntersect(comps[cc0], comps[cc1]);
          if (vi == smaller && !nn_nodes.IsKey(smaller_id)){
            banned.AddDat(smaller_id);
          }
          /*else if (smaller > 2 && vi == smaller - 1 && !nn_nodes.IsKey(smaller_id)) {
            TSnapDetail::transitiveTransform(comps[cc0], comps[cc1]);
            banned.AddDat(cc0);
          }*/
        }
      }
    }

    // add transitivity connection

    /*
    int max_out_tp = -1;
    int max_out = -1;
    for (THashKeyDatI<TInt, TInt> it = TEdges.BegI(); !it.IsEnd(); it++) {
      if (it.GetDat() > max_out) {
        max_out = it.GetDat();
        max_out_tp = it.GetKey();
      }
    }
    */
    for (int cc0 = 0; cc0 < comps.Len(); cc0++) {
      if (!banned.IsKey(cc0) /*&& TSnapDetail::chekIfCrossing(comps[cc0], t, first, last, max_out_tp)*/)
        elements.AddDat(cc0, comps[cc0]);
    }
    

    TIntV communitiesAtT;
    for (int cc = 0; cc < elements.Len(); cc++) {
      components.AddDat(newId, elements[cc]);
    communitiesAtT.Add(newId);
    newId++;
    }
    if (elements.Len() > 0)
      ct.AddDat(focusTimePoint, communitiesAtT);
    
  } // FOR

  // connecting neighbouring components
  THashKeyDatI<TInt, TIntV> it = ct.BegI();
  THashKeyDatI<TInt, TIntV> prelast = ct.EndI()--;
  prelast--;
  while (it < prelast) {
    TIntV cms0;
    TIntV cms1;
    int focusTimePoint;
    int focusTimePoint1;
    focusTimePoint = it.GetKey();
    cms0 = it.GetDat();
    it++;
    focusTimePoint1 = it.GetKey();
    cms1 = it.GetDat();
    if (cms0.Len() > 0 && cms1.Len() > 0) {
      for (int i = 0; i < cms0.Len(); i++) {
        for (int j = 0; j < cms1.Len(); j++) {
          TIntV ids0 = components.GetDat(cms0[i]);
          TIntV ids1 = components.GetDat(cms1[j]);
          int smaller = ids0.Len();
          if (ids1.Len() < smaller)
            smaller = ids1.Len();

          if (TSnapDetail::vectorIntersect(ids0, ids1) == smaller || (smaller > 2 && TSnapDetail::vectorIntersect(ids0, ids1) == (smaller -1 ))) {
            if (!gFinal->IsNode(cms0[i])) {
              gFinal->AddNode(cms0[i]);
              tFinal.AddDat(cms0[i], focusTimePoint);
            }
            if (!gFinal->IsNode(cms1[j])) {
              gFinal->AddNode(cms1[j]);
              tFinal.AddDat(cms1[j], focusTimePoint1);
            }
            gFinal->AddEdge(cms0[i], cms1[j]);
          }
        }
      }
    }
  }// end connecting components 

  // collapsing chains
  if (collapse) {
    for (TNGraph::TNodeI NI = gFinal->BegNI(); NI < gFinal->EndNI(); NI++) {
      if (NI.GetInDeg() == 1 && NI.GetOutDeg() == 1)
        if (gFinal->GetNI(NI.GetInNId(0)).GetOutDeg() == 1 && gFinal->GetNI(NI.GetOutNId(0)).GetInDeg() == 1)
        {
        gFinal->AddEdge(NI.GetInNId(0), NI.GetOutNId(0));
        gFinal->DelEdge(NI.GetInNId(0), NI.GetId());
        tFinal.DelKey(NI.GetId());
        gFinal->DelNode(NI.GetId());
        }
    }
  }// end collapsing

}

namespace TSnapDetail {
/// Clauset-Newman-Moore community detection method.
/// At every step two communities that contribute maximum positive value to global modularity are merged.
/// See: Finding community structure in very large networks, A. Clauset, M.E.J. Newman, C. Moore, 2004
class TCNMQMatrix {
private:
  struct TCmtyDat {
    double DegFrac;
    TIntFltH NIdQH;
    int MxQId;
    TCmtyDat() : MxQId(-1) { }
    TCmtyDat(const double& NodeDegFrac, const int& OutDeg) : 
      DegFrac(NodeDegFrac), NIdQH(OutDeg), MxQId(-1) { }
    void AddQ(const int& NId, const double& Q) {
      NIdQH.AddDat(NId, Q);
      if (MxQId == -1 || NIdQH[MxQId]<Q) { MxQId = NIdQH.GetKeyId(NId); }
    }
    void UpdateMaxQ() {
      MxQId = -1;
      for (int i = -1; NIdQH.FNextKeyId(i);) {
        if (MxQId == -1 || NIdQH[MxQId]< NIdQH[i]) { MxQId = i; }
      }
    }
    void DelLink(const int& K) {
      const int NId = GetMxQNId();
      NIdQH.DelKey(K); if (NId == K) { UpdateMaxQ(); }
    }
    int GetMxQNId() const { return NIdQH.GetKey(MxQId); }
    double GetMxQ() const { return NIdQH[MxQId]; }
  };
private:
  THash<TInt, TCmtyDat> CmtyQH;
  THeap<TFltIntIntTr> MxQHeap;
  TUnionFind CmtyIdUF;
  double Q;
public:
  TCNMQMatrix(const PUNGraph& Graph) : CmtyQH(Graph->GetNodes()), 
    MxQHeap(Graph->GetNodes()), CmtyIdUF(Graph->GetNodes()) {
    Init(Graph);
  }
  void Init(const PUNGraph& Graph) {
    const double M = 0.5 / Graph->GetEdges(); // 1/2m
    Q = 0.0;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      CmtyIdUF.Add(NI.GetId());
      const int OutDeg = NI.GetOutDeg();
      if (OutDeg == 0) { continue; }
      TCmtyDat& Dat = CmtyQH.AddDat(NI.GetId(), TCmtyDat(M * OutDeg, OutDeg));
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int DstNId = NI.GetOutNId(e);
        const double DstMod = 2 * M * (1.0 - OutDeg * Graph->GetNI(DstNId).GetOutDeg() * M);
        Dat.AddQ(DstNId, DstMod);
      }
      Q += -1.0*TMath::Sqr(OutDeg*M);
      if (NI.GetId() < Dat.GetMxQNId()) {
        MxQHeap.Add(TFltIntIntTr(Dat.GetMxQ(), NI.GetId(), Dat.GetMxQNId()));
      }
    }
    MxQHeap.MakeHeap();
  }
  TFltIntIntTr FindMxQEdge() {
    while (true) {
      if (MxQHeap.Empty()) { break; }
      const TFltIntIntTr TopQ = MxQHeap.PopHeap();
      if (!CmtyQH.IsKey(TopQ.Val2) || !CmtyQH.IsKey(TopQ.Val3)) { continue; }
      if (TopQ.Val1 != CmtyQH.GetDat(TopQ.Val2).GetMxQ() && TopQ.Val1 != CmtyQH.GetDat(TopQ.Val3).GetMxQ()) { continue; }
      return TopQ;
    }
    return TFltIntIntTr(-1, -1, -1);
  }

  bool MergeBestQ() {
    const TFltIntIntTr TopQ = FindMxQEdge();
    if (TopQ.Val1 <= 0.0) { return false; }
    // joint communities
    const int I = TopQ.Val3;
    const int J = TopQ.Val2;
    CmtyIdUF.Union(I, J); // join
    Q += TopQ.Val1;
    TCmtyDat& DatJ = CmtyQH.GetDat(J);
    { TCmtyDat& DatI = CmtyQH.GetDat(I);
    DatI.DelLink(J);  DatJ.DelLink(I);
    for (int i = -1; DatJ.NIdQH.FNextKeyId(i); ) {
      const int K = DatJ.NIdQH.GetKey(i);
      TCmtyDat& DatK = CmtyQH.GetDat(K);
      double NewQ = DatJ.NIdQH[i];
      if (DatI.NIdQH.IsKey(K)) { NewQ = NewQ + DatI.NIdQH.GetDat(K);  DatK.DelLink(I); }     // K connected to I and J
      else { NewQ = NewQ - 2 * DatI.DegFrac*DatK.DegFrac; }  // K connected to J not I
      DatJ.AddQ(K, NewQ);
      DatK.AddQ(J, NewQ);
      MxQHeap.PushHeap(TFltIntIntTr(NewQ, TMath::Mn(J, K), TMath::Mx(J, K)));
    }
    for (int i = -1; DatI.NIdQH.FNextKeyId(i); ) {
      const int K = DatI.NIdQH.GetKey(i);
      if (!DatJ.NIdQH.IsKey(K)) { // K connected to I not J
        TCmtyDat& DatK = CmtyQH.GetDat(K);
        const double NewQ = DatI.NIdQH[i] - 2 * DatJ.DegFrac*DatK.DegFrac; 
        DatJ.AddQ(K, NewQ);
        DatK.DelLink(I);
        DatK.AddQ(J, NewQ);
        MxQHeap.PushHeap(TFltIntIntTr(NewQ, TMath::Mn(J, K), TMath::Mx(J, K)));
      }
    }
    DatJ.DegFrac += DatI.DegFrac; }
    if (DatJ.NIdQH.Empty()) { CmtyQH.DelKey(J); } // isolated community (done)
    CmtyQH.DelKey(I);
    return true;
  }
  static double CmtyCMN(const PUNGraph& Graph, TCnComV& CmtyV) {
    TCNMQMatrix QMatrix(Graph);
    // maximize modularity
    while (QMatrix.MergeBestQ()) {}
    // reconstruct communities
    THash<TInt, TIntV> IdCmtyH;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      IdCmtyH.AddDat(QMatrix.CmtyIdUF.Find(NI.GetId())).Add(NI.GetId());
    }
    CmtyV.Gen(IdCmtyH.Len());
    for (int j = 0; j < IdCmtyH.Len(); j++) {
      CmtyV[j].NIdV.Swap(IdCmtyH[j]);
    }
    return QMatrix.Q;
  }
};

} // namespace TSnapDetail

double CommunityCNM(const PUNGraph& Graph, TCnComV& CmtyV) {
  return TSnapDetail::TCNMQMatrix::CmtyCMN(Graph, CmtyV);
}

}; //namespace TSnap
