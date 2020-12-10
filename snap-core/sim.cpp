
float JaccardSim(TNGraph::TNodeI NI1, TNGraph::TNodeI NI2) {
  int lenA = NI1.GetOutDeg();
  int lenB = NI2.GetOutDeg();
  int ct = 0;
  int j = 0;
  int i = 0;
  while (i < lenA  &&  j < lenB) {
    if (NI1.GetOutNId(i) == NI2.GetOutNId(j)) {
      ct++; i++; j++;
    } else if (NI1.GetOutNId(i) > NI2.GetOutNId(j)) {
      j++;
    } else {
      i++;
    }
  }
  return ct*1.0/(lenA+lenB-ct);

}

void MergeNbrs(TIntV* NeighbourV, TIntV* list1, TNGraph::TNodeI NI2) {
  int j = 0;
  int k = 0;
  int prev = -1;
  int lenA = list1->Len();

  int lenB = NI2.GetInDeg();
  if (lenA > 0  &&  lenB > 0) {
    int v1 = (*list1)[j];
    int v2 = NI2.GetInNId(k);
    while (1) {
      if (v1 <= v2) {
        if (prev != v1) {
          NeighbourV->Add(v1);
          prev = v1;
        }
        j += 1;
        if (j >= lenA) {
          break;
        }
        v1 = (*list1)[j];
      } else {
        if (prev != v2) {
          NeighbourV->Add(v2);
          prev = v2;
        }
        k += 1;
        if (k >= lenB) {
          break;
        }
        v2 = NI2.GetInNId(k);
      }
    }
  }
  while (j < lenA) {
    int v = (*list1)[j];
    if (prev != v) {
      NeighbourV->Add(v);
      prev = v;
    }
    j += 1;
  }
  while (k < lenB) {
    int v = NI2.GetInNId(k);
    if (prev != v) {
      NeighbourV->Add(v);
      prev = v;
    }
    k += 1;
  }
}

PNGraph GetBiGraph(PTable P, int index_col_1, int index_col_2) {
  TVec<TPair<TStr, TAttrType>, int > S = P->GetSchema();
  PNGraph Graph = TSnap::ToGraph<PNGraph>(P, S[index_col_1].GetVal1(), S[index_col_2].GetVal1(), aaFirst);
  return Graph;
}

#ifdef GCC_ATOMIC
PNEANet KNNJaccardParallel(PNGraph Graph,int K) {
  PNEANet KNN = TNEANet::New();
  TIntV NIdV;
  Graph->GetNIdV (NIdV);
  int size = NIdV.Len();
  for (int ind = 0; ind < size; ind++) {
    KNN->AddNode(NIdV[ind]);
  }
  KNN->AddFltAttrE("sim");
  TVec<TVec<TPair<TFlt, TInt>, int >, int > TopKList;
  TVec<TVec<TPair<TFlt, TInt>, int >, int > ThTopK; // for each thread
  TIntV NodeList;
  TIntV ThNodeList;// for each thread
  int NumThreads = omp_get_max_threads();
  omp_set_num_threads(NumThreads);
  #pragma omp parallel private(ThNodeList, ThTopK)
  {
    TIntV* Neighbors_old = new TIntV();
    TIntV* Neighbors = new TIntV();
    TIntV* temp;

    #pragma omp for schedule(dynamic,1000)
    for (int ind = 0; ind < size; ind++) {
      TNGraph::TNodeI NI = Graph->GetNI(NIdV[ind]);
      if (NI.GetInDeg() > 0) {
        continue;
      }
      if (NI.GetOutDeg() == 0) {
        continue;
      }

      TVec<TPair<TFlt, TInt>, int > TopK;
      for (int i = 0; i < K; i++) {
          TopK.Add(TPair<TFlt,TInt>(0.0, -1));
      }

      Neighbors->Clr(false);
      Neighbors_old->Clr(false);

      for (int i = 0; i < NI.GetOutDeg(); i++) {
        TNGraph::TNodeI Inst_NI = Graph->GetNI(NI.GetOutNId(i));
        MergeNbrs(Neighbors, Neighbors_old, Inst_NI);

        temp = Neighbors_old;
        temp->Clr(false);
        Neighbors_old = Neighbors;
        Neighbors = temp;
      }

      // Swap neighbors and Neighbors_old

      temp = Neighbors_old;
      Neighbors_old = Neighbors;
      Neighbors = temp;
      for(int j = 0; j< Neighbors->Len(); j++) {

        TNGraph::TNodeI Auth_NI = Graph->GetNI((*Neighbors)[j]);

        float similarity = JaccardSim(NI, Auth_NI);
        if (TopK[K-1].GetVal1() < similarity) {
          int index = 0;
          for (int i = K-2; i >= 0; i--)
            if (TopK[i].GetVal1() < similarity) {
              TopK.SetVal(i+1, TopK[i]);
            } else {
              index = i+1;
              break;
            }
          TopK.SetVal(index, TPair<TFlt, TInt>(similarity, (*Neighbors)[j]));
        }
      }

      ThTopK.Add(TopK);
      ThNodeList.Add(NIdV[ind]);

//    if (ct%10000 == 0)
//    	cout<<ct<<" avg neighbor degree = "<<sum_neighbors*1.0/ct<<" "<<currentDateTime()<<endl;

    }
    #pragma omp critical
    {
      for (int j = 0; j < ThTopK.Len(); j++) {
        TopKList.Add(ThTopK[j]);
        NodeList.Add(ThNodeList[j]);
      }
    }
	}

  int size2 = NodeList.Len();
  for (int i= 0; i < size2 ; i++) {

    for (int j = 0; j < K; j++) {
      if (TopKList[i][j].GetVal2() <= -1) {
        break;
      }
      int EId = KNN->AddEdge(NodeList[i], TopKList[i][j].GetVal2());
      KNN->AddFltAttrDatE(EId, TopKList[i][j].GetVal1(), "sim");
    }
  }
  return KNN;
}
#endif

PNEANet KNNJaccard(PNGraph Graph, int K) {
  PNEANet KNN = TNEANet::New();

  int sum_neighbors = 0;
  int ct = 0;
  int end;
  end = Graph->GetNodes();
  TIntV* Neighbors_old = new TIntV();
  TIntV* Neighbors = new TIntV();
  TIntV* temp;
  TIntV NIdV;
  Graph->GetNIdV (NIdV);
  int size = NIdV.Len();
  for (int ind = 0; ind < size; ind++) {
    KNN->AddNode(NIdV[ind]);
  }
  KNN->AddFltAttrE("sim");

  for (int ind = 0; ind < size; ind++) {
    TNGraph::TNodeI NI = Graph->GetNI(NIdV[ind]);
    if (NI.GetInDeg() > 0) {
      continue;
    }
    if (NI.GetOutDeg() == 0) {
      continue;
    }
    ct ++;

    TVec<TPair<TFlt, TInt> > TopK;
    for (int i = 0; i < K; i++) {
      TopK.Add(TPair<TFlt,TInt>(0.0, -1));
    }

    Neighbors->Clr(false);
    Neighbors_old->Clr(false);

    for (int i = 0; i < NI.GetOutDeg(); i++) {
      TNGraph::TNodeI Inst_NI = Graph->GetNI(NI.GetOutNId(i));
      MergeNbrs(Neighbors, Neighbors_old, Inst_NI);

      temp = Neighbors_old;
      temp->Clr(false);
      Neighbors_old = Neighbors;
      Neighbors = temp;
    }
    int num = Neighbors_old->Len();
    sum_neighbors += num;

    //Swap neighbors and Neighbors_old

    temp = Neighbors_old;
    Neighbors_old = Neighbors;
    Neighbors = temp;
    for (int j = 0; j< Neighbors->Len(); j++) {

      TNGraph::TNodeI Auth_NI = Graph->GetNI((*Neighbors)[j]);

      float similarity = JaccardSim(NI, Auth_NI);
      if (TopK[K-1].GetVal1() < similarity) {
        int index = 0;
        for (int i = K-2; i >= 0; i--)
          if (TopK[i].GetVal1() < similarity) {
            TopK.SetVal(i+1, TopK[i]);
          } else {
            index = i+1;
            break;
          }
        TopK.SetVal(index, TPair<TFlt, TInt>(similarity, (*Neighbors)[j]));
      }
    }

    for (int i = 0; i < K; i++) {
      int EId = KNN->AddEdge(NI.GetId(), TopK[i].GetVal2());
      KNN->AddFltAttrDatE(EId, TopK[i].GetVal1(), "sim");
    }

//    if (ct%10000 == 0)
//    	cout<<ct<<" avg neighbor degree = "<<sum_neighbors*1.0/ct<<" "<<currentDateTime()<<endl;

  }

  return KNN;
}

