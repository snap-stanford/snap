void TBowLearn::TrainMajorityMd(const PBowDocBs& BowDocBs, TFltV& MdCatExWgtV){
// P(C) -> MdCatExWgtV
  // prepare shortcuts
  int AllCats=BowDocBs->GetCats();
  // training
  // collecting category example weights
  double SumCatExWgt=0;
  {for (int DIdN=0; DIdN<BowDocBs->GetTrainDocs(); DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      MdCatExWgtV[CId]++; SumCatExWgt++;
    }
  }}
  if (SumCatExWgt>0){
    for (int CId=0; CId<AllCats; CId++){
      MdCatExWgtV[CId]/=SumCatExWgt;
      TStr CatNm=BowDocBs->GetCatNm(CId);
//      if (MdCatExWgtV[CId]>0){printf("[%s]-%d:%.5f ", CatNm(), CId, MdCatExWgtV[CId]);} //**
    }
//    printf("\n");
  } else {printf("No training examples!\n");}
}

double TBowLearn::TestMajorityMd(const PBowDocBs& BowDocBs,
 TFltV& MdCatExWgtV, FILE* OutFile, int& TestExs){
// P(C|Doc)=argmx_C P(C)
  // prepare shortcuts
  int AllCats=BowDocBs->GetCats();
  // testing
  double CorrectExs=0; TestExs=0;
  int MajorCId=-1; TFlt MajorCatWgt=0;
  {for (int CId=0; CId<AllCats; CId++){
    if (MdCatExWgtV[CId]>MajorCatWgt){MajorCId=CId; MajorCatWgt=MdCatExWgtV[CId];}
  }}
  if (MajorCId>-1){
    TStr CatNm=BowDocBs->GetCatNm(MajorCId);
    printf("\nMajority category P(%d:%s)=%.5f\n", MajorCId, CatNm(), MajorCatWgt()); //**
    fprintf(OutFile, "\nMajority category P(%d:%s)=%.5f\n", MajorCId, CatNm(), MajorCatWgt());
  } else {
    printf("No category with probability > 0\n"); //**
    fprintf(OutFile, "No category with probability > 0\n");
    return CorrectExs;
  }
  for (int DIdN=0; DIdN<BowDocBs->GetTestDocs(); DIdN++){
    int DId=BowDocBs->GetTestDId(DIdN);
    int WIds=BowDocBs->GetDocWIds(DId);
    int CIds=BowDocBs->GetDocCIds(DId);
    // write best categories
    TestExs++;
//    printf("Document vector: "); //**
    fprintf(OutFile, "Document vector:");
    {for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      TStr WordStr=BowDocBs->GetWordStr(WId);
      for (int WordFqN=0; WordFqN<WordFq; WordFqN++){
//        printf("%s ", WordStr.CStr()); //**
        fprintf(OutFile, "%s ", WordStr.CStr()); //**
      }
    }}
//    printf("\n");
    fprintf(OutFile, "\n");
    {for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      TStr CatNm=BowDocBs->GetCatNm(CId);
//      printf("[%d:%s] ", CId, CatNm()); //**
      fprintf(OutFile, "[%d:%s] ", CId, CatNm());
      if (CId==MajorCId){CorrectExs+=1/double(CIds);}
    }}
//    printf("  --> "); //**
    fprintf(OutFile, "  --> ");
    int CId=MajorCId;
    TStr CatNm=BowDocBs->GetCatNm(CId);
//    printf("\n  [%d:%s]:%.5f ", CId, CatNm(), MajorCatWgt); //**
    fprintf(OutFile, "\n  [%d:%s]:%.5f ", CId, CatNm(), MajorCatWgt());
//      printf("\n    Model evidence: majority class ");
    fprintf(OutFile, "\n    Model evidence: majority class value");
//    printf("\n"); //**
    fprintf(OutFile, "\n");
  }
  printf("Testing documents:%d, Non-empty documents:%d ",
   BowDocBs->GetTestDocs(), TestExs);
  fprintf(OutFile, "Testing documents:%d, Non-empty documents:%d ",
   BowDocBs->GetTestDocs(), TestExs);
  if (TestExs>0){
    printf("CorrectExs:%.5f\n", CorrectExs);
    fprintf(OutFile, "CorrectExs:%.5f\n", CorrectExs);
  } else {
    printf("CorrectExs:%.5f\n", CorrectExs);
    fprintf(OutFile, "CorrectExs:%.5f\n", CorrectExs);
  }
  return CorrectExs;
}

void TBowLearn::TrainInvertedMd(const PBowDocBs& BowDocBs,
 TFltVV& MdWordCatWgtVV, TFltV& MdWordWgtV, TFltV& MdCatWgtV){
  // prepare shortcuts
  int AllWords=BowDocBs->GetWords();
  int AllCats=BowDocBs->GetCats();
  // collecting word-category weights
  for (int DIdN=0; DIdN<BowDocBs->GetTrainDocs(); DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int WIds=BowDocBs->GetDocWIds(DId);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      for (int CIdN=0; CIdN<CIds; CIdN++){
        int CId=BowDocBs->GetDocCId(DId, CIdN);
        double CatWgt=WordFq*(1/double(CIds));
        MdWordCatWgtVV.At(WId, CId)+=CatWgt;
      }
    }
  }
  // collecting word weights
  for (int WId=0; WId<AllWords; WId++){
    double SumWordCatWgt=0;
    for (int CId=0; CId<AllCats; CId++){
      SumWordCatWgt+=MdWordCatWgtVV.At(WId, CId);}
    if (SumWordCatWgt>0){
      for (int CId=0; CId<AllCats; CId++){
        MdWordCatWgtVV.At(WId, CId)/=SumWordCatWgt;}
    }
    double WordWgt=0;
    {for (int CId=0; CId<AllCats; CId++){
      double WordCatWgt=MdWordCatWgtVV.At(WId, CId);
      if (WordCatWgt>=0){
        WordWgt-=WordCatWgt*log(WordCatWgt);}
    }}
    TStr WordStr=BowDocBs->GetWordStr(WId);
    MdWordWgtV[WId]=1/(1+TMath::Sqr(WordWgt));
    //MdWordWgtV[WId]=1;
//    if (WordWgt>0){printf("MdWordWgtV[%d:%s]:%.3f ", WId, WordStr.CStr(), MdWordWgtV[WId]);}  //**
  }
  printf("\n"); //**
  // collecting category weights
  double SumCatExWgt=0;
  {for (int DIdN=0; DIdN<BowDocBs->GetTrainDocs(); DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
     MdCatWgtV[CId]++; SumCatExWgt++;
    }
  }}
  if (SumCatExWgt>0){
    for (int CId=0; CId<AllCats; CId++){
      TStr CatNm=BowDocBs->GetCatNm(CId); //**
//      if (MdCatWgtV[CId]>0){printf("MdCatWgtV[%d:%s]:%.3f", CId, CatNm(), MdCatWgtV[CId]);} //**
      MdCatWgtV[CId]=1/log(1+1+TMath::Sqr(MdCatWgtV[CId]));
      //MdCatWgtV[CId]=1;
    }
    printf("\n"); //**
  } else {printf("No training examples!\n");}
}

double TBowLearn::TestInvertedMd(const PBowDocBs& BowDocBs,
 TFltVV& MdWordCatWgtVV, TFltV& MdWordWgtV, TFltV& MdCatWgtV,
 FILE* OutFile, int& TestExs){
  // testing
  // prepare shortcuts
  int AllCats=BowDocBs->GetCats();
  // testing
  double CorrectExs=0; TestExs=0;
  int MajorCId=-1; TFlt MajorCatWgt=0;
  {for (int CId=0; CId<AllCats; CId++){
    if (MdCatWgtV[CId]>MajorCatWgt){MajorCId=CId; MajorCatWgt=MdCatWgtV[CId];}
  }}
  if (MajorCId>-1){
    TStr CatNm=BowDocBs->GetCatNm(MajorCId);
    printf("\nMajority category P(%d:%s])=%.5f\n", MajorCId, CatNm(), MajorCatWgt()); //**
    fprintf(OutFile, "\nMajority category P(%d:%s])=%.5f\n", MajorCId, CatNm(), MajorCatWgt());
  } else {
    printf("No category with probability > 0\n"); //**
    fprintf(OutFile, "No category with probability > 0\n");
  }
  for (int DIdN=0; DIdN<BowDocBs->GetTestDocs(); DIdN++){
    int DId=BowDocBs->GetTestDId(DIdN);
    int WIds=BowDocBs->GetDocWIds(DId);
    int CIds=BowDocBs->GetDocCIds(DId);
    if (WIds==0){printf("It is hard to classify an empty document!\n"); continue;}
    // create category vector
    TFltIntKdV CatWgtCIdKdV(AllCats);
    {for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Dat=CId; CatWgtCIdKdV[CId].Key=0;
    }}
    {for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      printf("MdWordWgtV[%d]:%.3f ", WId, MdWordWgtV[WId]());//**
      for (int CId=0; CId<AllCats; CId++){
        double CatWgt=
         WordFq*MdCatWgtV[CId]*MdWordWgtV[WId]*MdWordCatWgtVV.At(WId, CId);
        CatWgtCIdKdV[CId].Key+=CatWgt;
        if ((CId==5)||(CId==130)){printf("CatWgt[%d]=%.1f*%.3f*%.3f*%.3f\n",
            CId, WordFq, MdCatWgtV[CId](), MdWordWgtV[WId](), MdWordCatWgtVV.At(WId, CId)());} //**
        if ((CId==5)||(CId==130)){printf("CatWgt.Key[%d]=%.5f\n", CId, CatWgtCIdKdV[CId].Key());} //**
      }
    }}
    double SumCatWgt=0;
    {for (int CId=0; CId<AllCats; CId++){
      SumCatWgt+=CatWgtCIdKdV[CId].Key;
      if ((CId==5)||(CId==130)){ //**
        printf("\nMdCatWgtV[%d]=%.5f CatWgt[%d]=%.5f\n", CId, MdCatWgtV[CId](),
          CId, CatWgtCIdKdV[CId].Key());
      }
    }}
    printf("\n");
    if (SumCatWgt>0){
      for (int CId=0; CId<AllCats; CId++){CatWgtCIdKdV[CId].Key/=SumCatWgt;}
    }
    // choose best categories
    CatWgtCIdKdV.Sort(false);
    printf("BestCat:%d, prob:%.5f\n", CatWgtCIdKdV[0].Dat(), CatWgtCIdKdV[0].Key()); //**
    if (CatWgtCIdKdV[0].Key<=0.0001){ // check the most probable category
      printf("No category found for %d!\n", DId); //**
      fprintf(OutFile, "No category found for %d!\n", DId);
      continue;
    }
    double SumCatWgtSF=0; TIntV BestCIdV;
    for (int CIdN=0; CIdN<AllCats; CIdN++){
      double CatWgt=CatWgtCIdKdV[CIdN].Key;
      int CId=CatWgtCIdKdV[CIdN].Dat;
      SumCatWgtSF+=CatWgt; BestCIdV.Add(CId);
      if (SumCatWgtSF>0.25){break;}
    }
    // write best categories
    TestExs++;
    printf("Document vector: "); //**
    fprintf(OutFile, "Document vector:");
    {for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      TStr WordStr=BowDocBs->GetWordStr(WId);
      for (int WordFqN=0; WordFqN<WordFq; WordFqN++){
        printf("%s ", WordStr.CStr()); //**
        fprintf(OutFile, "%s ", WordStr.CStr()); //**
      }
    }}
    printf("\n"); //**
    fprintf(OutFile, "\n");
    {for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      TStr CatNm=BowDocBs->GetCatNm(CId);
      printf("[%d:%s] ", CId, CatNm()); //**
      fprintf(OutFile, "[%d:%s] ", CId, CatNm());
      if (BestCIdV.IsIn(CId)){CorrectExs+=1/double(CIds);}
    }}
    printf("  --> "); //**
    fprintf(OutFile, "  --> ");
    {for (int CIdN=0; CIdN<BestCIdV.Len(); CIdN++){
      int CId=BestCIdV[CIdN];
      TStr CatNm=BowDocBs->GetCatNm(CId);
      printf("\n  [%d:%s]:%.5f ", CId, CatNm(), CatWgtCIdKdV[CIdN].Key()); //**
      fprintf(OutFile, "\n  [%d:%s]:%.5f ", CId, CatNm(), CatWgtCIdKdV[CIdN].Key());
      printf("\n    Model evidence: ");  //**
      fprintf(OutFile, "\n    Model evidence: ");
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
        if (MdWordCatWgtVV.At(WId, CId)>0){
          TStr WordStr=BowDocBs->GetWordStr(WId);
          printf("TF(%s)=%.1f ", WordStr.CStr(), MdWordCatWgtVV.At(WId, CId)()); //**
          fprintf(OutFile, "TF(%s)=%.1f ", WordStr.CStr(), MdWordCatWgtVV.At(WId, CId)());
        }
      }
      printf("\n"); //**
      fprintf(OutFile, "\n");
    }}
    printf("\n"); //**
    fprintf(OutFile, "\n");
  }
  printf("Testing documents:%d, Non-empty documents:%d ",
   BowDocBs->GetTestDocs(), TestExs);
  fprintf(OutFile, "Testing documents:%d, Non-empty documents:%d ",
   BowDocBs->GetTestDocs(), TestExs);
  if (TestExs>0){
    printf("CorrectExs:%.5f\n", CorrectExs);
    fprintf(OutFile, "CorrectExs:%.5f\n", CorrectExs);
  } else {
    printf("CorrectExs:%.5f\n", CorrectExs);
    fprintf(OutFile, "CorrectExs:%.5f\n", CorrectExs);
  }
  return CorrectExs;
}

void TBowLearn::TrainSimMd(const PBowDocBs& BowDocBs,
 const TVec<TIntV>& DIdVV, TBowSpVV& ConceptSpVV){
// P(C|Doc) = argmx_C CosSim(TFIDF(Doc),TFIDF(C))
// TFIDF(DId) -> SpV=BowDocWgtBs->GetSpVFromDIdN(DId), Doc -> DId
// TF(CIdN) -> BowDocWgtBs->GetConceptSpV(BowSim, DIdVV[CIdN], ConceptSpVV[CIdN]), C -> CIdN
  // prepare shortcuts
  int AllCats=BowDocBs->GetCats();
  // get train doc-ids
  TIntV CtxTrainDIdV; BowDocBs->GetTrainDIdV(CtxTrainDIdV);
  // make TFIDF vectors for training documents
  PBowDocWgtBs BowDocWgtBs= //** Dunja: error, CtxTrainDIdV has 389 Ids, but their range is 0..3175
   TBowDocWgtBs::New(BowDocBs, bwwtNrmTFIDF, 0, 0, CtxTrainDIdV);
  // get representative concept vectors
  PBowSim BowSim=TBowSim::New(bstCos);
  for (int CIdN=0; CIdN<AllCats; CIdN++){
    ConceptSpVV[CIdN]=TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, DIdVV[CIdN]);
  }
}

double TBowLearn::TestSimMd(const PBowDocBs& BowDocBs,
 const TBowSpVV& ConceptSpVV,
 FILE* OutFile, int& TestExs){
// P(C|Doc) = argmx_C CosSim(TFIDF(Doc),TFIDF(C))
// TFIDF(DIdN) -> SpV=BowDocWgtBs->GetSpVFromDIdN(DIdN), Doc -> DIdN
// TF(CIdN) -> BowDocWgtBs->GetConceptSpV(BowSim, DIdVV[CIdN], ConceptSpVV[CIdN]), C -> CIdN
  // prepare shortcuts
  int AllCats=BowDocBs->GetCats();
  // make TFIDF vectors for testing documents
  TIntV CtxTestDIdNV; BowDocBs->GetTestDIdV(CtxTestDIdNV);
  PBowDocWgtBs BowDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, bwwtNrmTFIDF, 0, 0, CtxTestDIdNV);
  // testing
  double CorrectExs=0; TestExs=0;
  // for each document find closest concept vector
  for (int DIdN=0; DIdN<BowDocBs->GetTestDocs(); DIdN++){
    int DId=BowDocWgtBs->GetDId(DIdN);
    PBowSpV SpV=BowDocWgtBs->GetSpV(DId);
    int DIdx=BowDocBs->GetTestDId(DIdN);
    printf("DIdx:%d = DId:%d ", DIdx, DId); // are they the same?
    int WIdsx=BowDocBs->GetDocWIds(DId);
    int WIds=SpV->GetWIds();
    printf("WIdsx:%d = WIds:%d", WIdsx, WIds); // are they the same?
    int CIds=BowDocBs->GetDocCIds(DId);
    if (WIds==0){printf("It is hard to classify an empty document!\n"); continue;}
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId=SpV->GetWId(WIdN); double WordWgt=SpV->GetWgt(WIdN);
      printf("WId:%d  ", WId);
      TStr WordStr=BowDocBs->GetWordStr(WId);
      printf("%s:%.2f ", WordStr.CStr(), WordWgt); //**
    }
    printf("\n");
    // create category vector
    PBowSim BowSim=TBowSim::New(bstCos);
    TFltIntKdV CatWgtCIdKdV(AllCats);
    for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Dat=CId;
      CatWgtCIdKdV[CId].Key=BowSim->GetSim(ConceptSpVV[CId], SpV);
      printf("Sim[%d]=%.3f ", CId, CatWgtCIdKdV[CId].Key());
    }
    // choose best categories
    CatWgtCIdKdV.Sort(false);
    printf("BestCat:%d, prob:%.5f\n", CatWgtCIdKdV[0].Dat(), CatWgtCIdKdV[0].Key());
    if (CatWgtCIdKdV[0].Key<=0.0001){ // check the most probable category
      printf("No category found for %d!\n", DId); //**
      fprintf(OutFile, "No category found for %d!\n", DId);
      continue;
    }
    double SumCatWgtSF=0; TIntV BestCIdV;
    for (int CIdN=0; CIdN<AllCats; CIdN++){
      double CatWgt=CatWgtCIdKdV[CIdN].Key;
      int CId=CatWgtCIdKdV[CIdN].Dat;
      SumCatWgtSF+=CatWgt; BestCIdV.Add(CId);
      if (SumCatWgtSF>0.25){break;}
    }
    // write best categories
    TestExs++;
    {for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      TStr CatNm=BowDocBs->GetCatNm(CId);
      printf("[%s]-%d ", CatNm(), CId); //**
      fprintf(OutFile, "[%s]-%d ", CatNm(), CId);
      if (BestCIdV.IsIn(CId)){CorrectExs+=1/double(CIds);}
    }}
    printf("  Document: "); //**
    fprintf(OutFile, "  Document: ");
    {for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId=SpV->GetWId(WIdN); double WordWgt=SpV->GetWgt(WIdN);
      TStr WordStr=BowDocBs->GetWordStr(WId);
      printf("%s:%.2f ", WordStr.CStr(), WordWgt);
      fprintf(OutFile, "%s:%.2f ", WordStr.CStr(), WordWgt);
    }}
    printf("\n--> "); //**
    fprintf(OutFile, "\n--> ");
    {for (int CIdN=0; CIdN<BestCIdV.Len(); CIdN++){
      int CId=BestCIdV[CIdN];
      TStr CatNm=BowDocBs->GetCatNm(CId);
      printf("[%s]:%.5f ", CatNm(), CatWgtCIdKdV[CIdN].Key());
      fprintf(OutFile, "[%s]:%.5f ", CatNm(), CatWgtCIdKdV[CIdN].Key());
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId=SpV->GetWId(WIdN); double WordWgt=ConceptSpVV[CId]->GetWgt(WIdN);
        if (WordWgt>0){
          TStr WordStr=BowDocBs->GetWordStr(WId);
          printf("TF(%s,%d)=%.5f ", WordStr.CStr(), CId, WordWgt);
          fprintf(OutFile, "TF(%s,%d)=%.5f ", WordStr.CStr(), CId, WordWgt);
        }
      }
    }}
    printf("\n"); //**
    fprintf(OutFile, "\n");
  }
  return CorrectExs;
}

void TBowLearn::TrainNBayesMd(const PBowDocBs& BowDocBs,
 TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV,
 TFltV& MdCatExWgtV){
// log P(C|Doc)= argmx_C log P(C) + sum_i TF(W_i,Doc) * log P(W_i|C)
// P(W_i|C) = (TF(W_i,C)+1)/(sum_j P(W_j,C) + AllWords
// TF(W_i,C) = sum_k TF(W_i, D_k) * P(D_k,C) -> MdWordCatWgtVV
// sum_j TF(W_j|C) -> MdCatWgtV
// P(C) -> MdCatExWgtV
    // prepare shortcuts
  int AllWords=BowDocBs->GetWords();
  int AllCats=BowDocBs->GetCats();
  int TrainDocs=BowDocBs->GetTrainDocs();
  // training
  // collecting word-category weights
  for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int WIds=BowDocBs->GetDocWIds(DId);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      for (int CIdN=0; CIdN<CIds; CIdN++){
        int CId=BowDocBs->GetDocCId(DId, CIdN);
        double CatWgt=WordFq*(1/double(CIds));
        MdWordCatWgtVV.At(WId, CId)+=CatWgt;
      }
    }
  }
  // collecting category weights
  {for (int CId=0; CId<AllCats; CId++){
    double SumWordCatWgt=0;
    for (int WId=0; WId<AllWords; WId++){
      SumWordCatWgt+=MdWordCatWgtVV.At(WId, CId);
/*      if (MdWordCatWgtVV.At(WId, CId)>0){
        printf("TF(W%d,C%d):%.5f ", WId, CId, MdWordCatWgtVV.At(WId, CId));} */
    }
    MdCatWgtV[CId]=SumWordCatWgt;
  }}
  // collecting category example weights
  {for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      MdCatExWgtV[CId]++;
    }
  }}
  if (TrainDocs>0){
    for (int CId=0; CId<AllCats; CId++){
      MdCatExWgtV[CId]/=TrainDocs;
      TStr CatNm=BowDocBs->GetCatNm(CId);
//      if (MdCatExWgtV[CId]>0){printf("[%s]-%d:%.5f ", CatNm(), CId, MdCatExWgtV[CId]);} //**
    }
//    printf("\n");
  } else {printf("No training examples!\n");}
}

double TBowLearn::TestNBayesMd(const PBowDocBs& BowDocBs,
 TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV,
 TFltV& MdCatExWgtV, FILE* OutFile, FILE* ErrFile, int& TestExs){
// log P(C|Doc)= argmx_C sum_i [log P(W_i|C) * TF(W_i,Doc)] + log P(C)
// P(W_i|C) = (TF(W_i,C)+1)/(sum_j P(W_j,C) + AllWords)
// TF(W_i,C) -> MdWordCatWgtVV
// sum_j TF(W_j|C) -> MdCatWgtV
// P(C) -> MdCatExWgtV
//
// handles multilabeled examples
//
// skip empty documents and docs where all categories have low prob. (instead of giving majority category)
// set TF(W_i,Doc)=1
//
  //double MnProb=0.00000001;
  // prepare shortcuts
  int AllWords=BowDocBs->GetWords();
  int AllCats=BowDocBs->GetCats();
  // find majority category
  int MajorCId=-1; TFlt MajorCatWgt=0;
  {for (int CId=0; CId<AllCats; CId++){
    if (MdCatExWgtV[CId]>MajorCatWgt){MajorCId=CId; MajorCatWgt=MdCatExWgtV[CId];}
  }}
  if (MajorCId>-1){
    TStr CatNm=BowDocBs->GetCatNm(MajorCId);
//    printf("      Majority P(%d:%s])=%.5f\n", MajorCId, CatNm(), MajorCatWgt); //**
    fprintf(OutFile, "      Majority P(%d:%s])=%.5f\n", MajorCId, CatNm(), MajorCatWgt());
  } else {
//    printf("      No category with probability > 0\n"); //**
    fprintf(OutFile, "      No category with probability > 0\n");
  }
  // testing on testing examples
  double CorrectExs=0; TestExs=0;
  //int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<BowDocBs->GetTestDocs(); DIdN++){
    int DId=BowDocBs->GetTestDId(DIdN);
    int WIds=BowDocBs->GetDocWIds(DId);
    int CIds=BowDocBs->GetDocCIds(DId);
    if (WIds==0){printf("It is hard to classify an empty document!\n"); continue;}
    // create category vector
    TIntV ShareWordsCat(AllCats); // 1 if the category share any word with document
    TFltIntKdV CatWgtCIdKdV(AllCats);
    {for (int CId=0; CId<AllCats; CId++){
      ShareWordsCat[CId]=0;
      CatWgtCIdKdV[CId].Dat=CId; CatWgtCIdKdV[CId].Key=0;
    }}

    {for (int CId=0; CId<AllCats; CId++){
      if(MdCatExWgtV[CId]>0){ // prior probability of category > 0
        for (int WIdN=0; WIdN<WIds; WIdN++){
          int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
          // different contexts compose one documents, WordFq can be as high as 80
          WordFq=1;
          if(MdWordCatWgtVV.At(WId, CId)>0){
            ShareWordsCat[CId]=1;
//            printf("P(%d,%d)=%0.4f ", WId, CId, MdWordCatWgtVV.At(WId, CId));
          }
          double CatWgt=(MdWordCatWgtVV.At(WId, CId)+1)/(MdCatWgtV[CId]+double(AllWords));
          CatWgtCIdKdV[CId].Key+=(log(CatWgt)*WordFq);
/*          if(MdWordCatWgtVV.At(WId, CId)>0){
              if ((CId==8)||(CId==35)||(CId==20)){
                printf("DIdN=%d ", DIdN);
                printf("log CatWgt[%d]=(%.3f+1)/(%.3f+%d) = %.5f\n", CId,
                 MdWordCatWgtVV.At(WId, CId), MdCatWgtV[CId], AllWords, CatWgtCIdKdV[CId].Key);
               }
            } //*/
        }
        // add prior probabilty only to categories that have prior porbability > 0
/*        if ((CId==8)||(CId==35)||(CId==20)){
          printf("log(MdCatExWgtV[%d])=%.4f, CatWgtCIdKdV[%d])=%.4f ", CId, log(MdCatExWgtV[CId]), CId, CatWgtCIdKdV[CId].Key);
        } //*/
        CatWgtCIdKdV[CId].Key+=log(double(MdCatExWgtV[CId]));
/*        if ((CId==8)||(CId==35)||(CId==20)){
          printf("CatWgtCIdKdV.Key[%d]=%.20f\n", CId, CatWgtCIdKdV[CId].Key);
        } //*/
      } else {
        CatWgtCIdKdV[CId].Key=TFlt::NInf;
      }
    }}
    // sort categories according to log(probability)
    CatWgtCIdKdV.Sort(false);
//    printf("BestCat:%d, prob:%.5f\n", CatWgtCIdKdV[0].Dat, CatWgtCIdKdV[0].Key);
    if(ShareWordsCat[CatWgtCIdKdV[0].Dat]==0){ // category does not share a word with document
      continue; // skip the document instead of returning prior probability
    }
/*    if (CatWgtCIdKdV[0].Key<=log(MnProb)){ // check the most probable category
      printf("No category found for %d!\n", DId);
      fprintf(OutFile, "No category found for %d!\n", DId);
      continue; // skip the document instead of returning prior probability
    } */
    // back from log and calc prob. for each category
    double SumCatWgt=0;
    {for (int CIdN=0; CIdN<AllCats; CIdN++){
//      printf("\nMdCatExWgtV[%d]=%.5f log(CatWgt[%d])=%.5f ", CatWgtCIdKdV[CIdN].Dat,
//        MdCatExWgtV[CIdN], CatWgtCIdKdV[CIdN].Dat, CatWgtCIdKdV[CIdN].Key);
      // set prob. to 0 for categories that do not share a word with document
      if (ShareWordsCat[CatWgtCIdKdV[CIdN].Dat]==1){// (CatWgtCIdKdV[CId].Key>TFlt::NInf)
        CatWgtCIdKdV[CIdN].Key=exp(double(CatWgtCIdKdV[CIdN].Key));
//        printf("Shared: CatWgtCIdKdV.Key[%d]=%g\n", CatWgtCIdKdV[CIdN].Dat, CatWgtCIdKdV[CIdN].Key);
      } else {CatWgtCIdKdV[CIdN].Key=0;}
      SumCatWgt+=CatWgtCIdKdV[CIdN].Key;
/*      if ((CatWgtCIdKdV[CIdN].Dat==8)||(CatWgtCIdKdV[CIdN].Dat==35)||(CatWgtCIdKdV[CIdN].Dat==20)){
        printf("CatWgtCIdKdV.Key[%d]=%g\n", CatWgtCIdKdV[CIdN].Dat, CatWgtCIdKdV[CIdN].Key);
      } //*/
    }}
//    printf("\n");
    if (SumCatWgt>0){
//      printf("SumCatWgt=%g ", SumCatWgt);
      {for (int CIdN=0; CIdN<AllCats; CIdN++){
/*        if ((CatWgtCIdKdV[CIdN].Dat==8)||(CatWgtCIdKdV[CIdN].Dat==35)||(CatWgtCIdKdV[CIdN].Dat==20)){
          printf("CatWgtCIdKdV[CIdN].Key[%d]=%g ", CatWgtCIdKdV[CIdN].Dat, CatWgtCIdKdV[CIdN].Key);
        } //*/
        CatWgtCIdKdV[CIdN].Key/=SumCatWgt;
 /*       if ((CatWgtCIdKdV[CIdN].Dat==8)||(CatWgtCIdKdV[CIdN].Dat==35)||(CatWgtCIdKdV[CIdN].Dat==20)){
          printf("CatWgtCIdKdV[CIdN].Key[%d]=%g ", CatWgtCIdKdV[CIdN].Dat, CatWgtCIdKdV[CIdN].Key);
        } //*/
      }}
    }
    // collect the set of best categories having summary prob. weight 25% of the total weight
    double SumCatWgtSF=0; TIntV BestCIdV;
    {for (int CIdN=0; CIdN<AllCats; CIdN++){
      double CatWgt=CatWgtCIdKdV[CIdN].Key;
      int CId=CatWgtCIdKdV[CIdN].Dat;
      SumCatWgtSF+=CatWgt; BestCIdV.Add(CId);
      if (SumCatWgtSF>0.25){break;}
    }}
    // record wrongly classified examples
    bool ErrorInClass=true;
    {for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      if (BestCIdV.IsIn(CId)){ErrorInClass=false;}
    }}
    if (ErrorInClass){fprintf(ErrFile, "%s\n", BowDocBs->GetDocNm(DId).CStr());}

    // write best categories
    TestExs++;
//    printf("Document vector: "); //**
    fprintf(OutFile, "Document vector:");
    {for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      TStr WordStr=BowDocBs->GetWordStr(WId);
      for (int WordFqN=0; WordFqN<WordFq; WordFqN++){
//        printf("%s ", WordStr.CStr()); //**
        fprintf(OutFile, "%s ", WordStr.CStr()); //**
      }
    }}
//    printf("\n");
    fprintf(OutFile, "\n");
    {for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      TStr CatNm=BowDocBs->GetCatNm(CId);
//      printf("[%d:%s] ", CId, CatNm()); //**
      fprintf(OutFile, "[%d:%s] ", CId, CatNm());
      if (BestCIdV.IsIn(CId)){CorrectExs+=1/double(CIds);}
    }}
//    printf("  --> "); //**
    fprintf(OutFile, "  --> ");
    {for (int CIdN=0; CIdN<BestCIdV.Len(); CIdN++){
      int CId=BestCIdV[CIdN];
      TStr CatNm=BowDocBs->GetCatNm(CId);
//      printf("\n  [%d:%s]:%.5f ", CId, CatNm(), CatWgtCIdKdV[CIdN].Key); //**
      fprintf(OutFile, "\n  [%d:%s]:%.5f ", CId, CatNm(), CatWgtCIdKdV[CIdN].Key());
//      printf("\n    Model evidence: ");
      fprintf(OutFile, "\n    Model evidence: ");
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
        if (MdWordCatWgtVV.At(WId, CId)>0){
          TStr WordStr=BowDocBs->GetWordStr(WId);
//          printf("TF(%s)=%.1f ", WordStr.CStr(), MdWordCatWgtVV.At(WId, CId)); //**
          fprintf(OutFile, "TF(%s)=%.1f ", WordStr.CStr(), MdWordCatWgtVV.At(WId, CId)());
        }
      }
//      printf("\n"); //**
      fprintf(OutFile, "\n");
    }}
//    printf("\n"); //**
    fprintf(OutFile, "\n");
  }
//  printf("Testing documents:%d, Non-empty documents:%d ", BowDocBs->GetTestDocs(), TestExs);
  fprintf(OutFile, "Testing documents:%d, Non-empty documents:%d ", BowDocBs->GetTestDocs(), TestExs);
  if (TestExs>0){
//    printf("CorrectExs:%.5f\n", CorrectExs);
    fprintf(OutFile, "CorrectExs:%.5f\n", CorrectExs);
  } else {
 //   printf("CorrectExs:%.5f\n", CorrectExs);
    fprintf(OutFile, "CorrectExs:%.5f\n", CorrectExs);
  }
  return CorrectExs;
}

void TBowLearn::TrainBinNBayesMd(const PBowDocBs& BowDocBs,
 TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV,
 TFltV& MdCatExWgtV, const TFileId& OutFile){
// log P(C|Doc)= argmx_C log P(C) + sum_i TF(W_i,Doc) * log P(W_i|C)
// P(W_i|C) = (TF(W_i,C)+1)/(sum_j P(W_j,C) + AllWords
// TF(W_i,C) = sum_k TF(W_i, D_k) * P(D_k,C) -> MdWordCatWgtVV
// sum_j TF(W_j|C) -> MdCatWgtV
// P(C) -> MdCatExWgtV
    // prepare shortcuts
  int AllWords=BowDocBs->GetWords();
  int AllCats=BowDocBs->GetCats();
  int TrainDocs=BowDocBs->GetTrainDocs();
  // training
  // collecting word-category weights
  for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int WIds=BowDocBs->GetDocWIds(DId);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      for (int CIdN=0; CIdN<CIds; CIdN++){
        int CId=BowDocBs->GetDocCId(DId, CIdN);
        double CatWgt=WordFq*(1/double(CIds));
        MdWordCatWgtVV.At(WId, CId)+=CatWgt;
      }
    }
  }
  // collecting category weights
  {for (int CId=0; CId<AllCats; CId++){
    double SumWordCatWgt=0;
    for (int WId=0; WId<AllWords; WId++){
      SumWordCatWgt+=MdWordCatWgtVV.At(WId, CId);
/*      if (MdWordCatWgtVV.At(WId, CId)>0){
        printf("TF(W%d,C%d):%.5f ", WId, CId, MdWordCatWgtVV.At(WId, CId));} */
    }
    MdCatWgtV[CId]=SumWordCatWgt;
  }}
  // collecting category example weights
  {for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    int CIds=BowDocBs->GetDocCIds(DId);
    for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      MdCatExWgtV[CId]++;
    }
  }}
  if (TrainDocs>0){
    for (int CId=0; CId<AllCats; CId++){
      MdCatExWgtV[CId]/=TrainDocs;
      TStr CatNm=BowDocBs->GetCatNm(CId);
//      if (MdCatExWgtV[CId]>0){printf("[%s]-%d:%.5f ", CatNm(), CId, MdCatExWgtV[CId]);} //**
    }
//    printf("\n");
  } else {printf("No training examples!\n");}
  // write the model
  fprintf(OutFile, "\n# NBayes Model\n");
  fprintf(OutFile, "# Feature");
  for (int CId=0; CId<AllCats; CId++){
    TStr CatNm=BowDocBs->GetCatNm(CId);
    fprintf(OutFile, ", %s_Weight", CatNm.CStr());
  }
  fprintf(OutFile, "\n");
  {for (int WId=0; WId<AllWords; WId++){
    fprintf(OutFile, "%s", BowDocBs->GetWordStr(WId).CStr());
    for (int CId=0; CId<AllCats; CId++){
      fprintf(OutFile, ", %0.5f", MdWordCatWgtVV.At(WId, CId)());
    }
    fprintf(OutFile, "\n");
  }}
}

void TBowLearn::TestBinNBayesMd(const PBowDocBs& BowDocBs,
 const TStr& TargetCatNm,
 TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV, TFltV& MdCatExWgtV,
 const TFileId& TrainOutFile, const TFileId& TestOutFile, const TFileId& ErrFile){
// log P(C|Doc)= argmx_C sum_i [log P(W_i|C) * TF(W_i,Doc)] + log P(C)
// P(W_i|C) = (TF(W_i,C)+1)/(sum_j P(W_j,C) + AllWords)
// TF(W_i,C) = sum_k TF(W_i, D_k) * P(D_k,C) -> MdWordCatWgtVV
// sum_j TF(W_j|C) -> MdCatWgtV
// P(C) -> MdCatExWgtV
  double MnProb=0.00000001;
  // prepare shortcuts
  int AllWords=BowDocBs->GetWords();
  int AllCats=BowDocBs->GetCats();
  // prepare shortcuts
  int TargetCId=BowDocBs->GetCId(TargetCatNm);
  int TrainDocs=BowDocBs->GetTrainDocs();
  int TestDocs=BowDocBs->GetTestDocs();
  // find majority category
  int MajorCId=-1; TFlt MajorCatWgt=0;
  {for (int CId=0; CId<AllCats; CId++){
    if (MdCatExWgtV[CId]>MajorCatWgt){MajorCId=CId; MajorCatWgt=MdCatExWgtV[CId];}
  }}
  if (MajorCId>-1){
    TStr CatNm=BowDocBs->GetCatNm(MajorCId);
    printf("      Majority P([%d]:%s)=%.5f\n", MajorCId, CatNm(), MajorCatWgt()); //**
    fprintf(ErrFile, "      Majority P([%d]:%s)=%.5f\n", MajorCId, CatNm(), MajorCatWgt());
  } else {
    printf("      No category with probability > 0\n"); //**
    fprintf(ErrFile, "      No category with probability > 0\n");
  }
  // test on training examples
  {int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
    int DocWIds=BowDocBs->GetDocWIds(DId);
    if (DocWIds==0){
      printf("It is hard to classify an empty document %s!\n", BowDocBs->GetDocNm(DId).CStr());
    }
    // create category vector
    TFltIntKdV CatWgtCIdKdV(AllCats);
    {for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Dat=CId; CatWgtCIdKdV[CId].Key=0;
    }}
    {for (int WIdN=0; WIdN<DocWIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      WordFq=1; // use boolean weight insted of TF
      // use LaPlace estimate smoothing of probability
      for (int CId=0; CId<AllCats; CId++){
        double CatWgt=(MdWordCatWgtVV.At(WId, CId)+1)/(MdCatWgtV[CId]+double(AllWords));
        CatWgtCIdKdV[CId].Key+=(log(CatWgt)*WordFq);
      }
    }}
    // add prior probabilty to all categories
    {for (int CId=0; CId<AllCats; CId++){
      if(MdCatExWgtV[CId]>0){
        CatWgtCIdKdV[CId].Key+=log(double(MdCatExWgtV[CId]));
      } else {CatWgtCIdKdV[CId].Key=TFlt::NInf;}
    }}
    // find the predicted category
    CatWgtCIdKdV.Sort(false); // stil in log space
    // shift to have mx. value in 0
    {for (int CId=1; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Key-=CatWgtCIdKdV[0].Key;
    }}
    CatWgtCIdKdV[0].Key-=CatWgtCIdKdV[0].Key;
    // back from log and calc prob. for each category
    double SumCatWgt=0;
    for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Key=exp(double(CatWgtCIdKdV[CId].Key));
      SumCatWgt+=CatWgtCIdKdV[CId].Key;
    }
    if (SumCatWgt>0){
      for (int CId=0; CId<AllCats; CId++){CatWgtCIdKdV[CId].Key/=SumCatWgt;}
    }
    // prepare the results
    if (CatWgtCIdKdV[0].Key<=MnProb){ // all categories have very low probability
      CatWgtCIdKdV[0].Dat=MajorCId; CatWgtCIdKdV[0].Key=MajorCatWgt;
    }
    bool PredClassVal;
    PredClassVal=(CatWgtCIdKdV[0].Dat==TargetCId);
    if (PredClassVal==ClassVal){
      if (PredClassVal){TruePos++;} else {TrueNeg++;}
    } else {// Error In Classification
      if (PredClassVal){FalsePos++;} else {FalseNeg++;}
    }
    // write results in the same form as SVM
    int ClassValN=(ClassVal)?1:-1;
    int PredClassValN=(PredClassVal)?1:-1;
    fprintf(TrainOutFile, "%0.6f # true=%d # docDesc=%s\n",
     PredClassValN*CatWgtCIdKdV[0].Key, ClassValN, BowDocBs->GetDocNm(DId).CStr());
  }}
  // now the real test on testing examples
  {int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<TestDocs; DIdN++){
    int DId=BowDocBs->GetTestDId(DIdN);
    bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
    int DocWIds=BowDocBs->GetDocWIds(DId);
    if (DocWIds==0){
      printf("It is hard to classify an empty document %s!\n", BowDocBs->GetDocNm(DId).CStr());
    }
    // create category vector
    TFltIntKdV CatWgtCIdKdV(AllCats);
    {for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Dat=CId; CatWgtCIdKdV[CId].Key=0;
    }}
    {for (int WIdN=0; WIdN<DocWIds; WIdN++){
      int WId; double WordFq; BowDocBs->GetDocWIdFq(DId, WIdN, WId, WordFq);
      WordFq=1; // use boolean weight insted of TF
      // use LaPlace estimate smoothing of probability
      for (int CId=0; CId<AllCats; CId++){
        double CatWgt=(MdWordCatWgtVV.At(WId, CId)+1)/(MdCatWgtV[CId]+double(AllWords));
        CatWgtCIdKdV[CId].Key+=(log(CatWgt)*WordFq);
      }
    }}
    // add prior probabilty to all categories
    {for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Key+=log(double(MdCatExWgtV[CId]));
    }}
    // find the predicted category
    CatWgtCIdKdV.Sort(false); // stil in log space
    // shift to have mx. value in 0
    {for (int CId=1; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Key-=CatWgtCIdKdV[0].Key;
    }}
    CatWgtCIdKdV[0].Key-=CatWgtCIdKdV[0].Key;
    // back from log and calc prob. for each category
    double SumCatWgt=0;
    for (int CId=0; CId<AllCats; CId++){
      CatWgtCIdKdV[CId].Key=exp(double(CatWgtCIdKdV[CId].Key));
      SumCatWgt+=CatWgtCIdKdV[CId].Key;
    }
    if (SumCatWgt>0){
      for (int CId=0; CId<AllCats; CId++){CatWgtCIdKdV[CId].Key/=SumCatWgt;}
    }
    // prepare the results
    if (CatWgtCIdKdV[0].Key<=MnProb){ // all categories have very low probability
      CatWgtCIdKdV[0].Dat=MajorCId; CatWgtCIdKdV[0].Key=MajorCatWgt;
    }
    bool PredClassVal;
    PredClassVal=(CatWgtCIdKdV[0].Dat==TargetCId);
    if (PredClassVal==ClassVal){
      if (PredClassVal){TruePos++;} else {TrueNeg++;}
    } else {// Error In Classification
      fprintf(ErrFile, "%s\n", BowDocBs->GetDocNm(DId).CStr());
      if (PredClassVal){FalsePos++;} else {FalseNeg++;}
    }
    // write results in the same form as SVM
    int ClassValN=(ClassVal)?1:-1;
    int PredClassValN=(PredClassVal)?1:-1;
    fprintf(TestOutFile, "%0.6f # true=%d # docDesc=%s\n",
     PredClassValN*CatWgtCIdKdV[0].Key, ClassValN, BowDocBs->GetDocNm(DId).CStr());
  }
  double Acc=100*(TruePos+TrueNeg)/double(TestDocs);
  double Prec=100*(TruePos/double(TruePos+FalsePos));
  double Rec=100*(TruePos/double(TruePos+FalseNeg));
  printf("Precision %0.2f%%, Recall %0.2f%%, Accuracy %0.2f%%\n", Prec, Rec, Acc);
//  fprintf(ErrFile, "%0.2f, %0.2f, %0.2f, %0.2f\n", Beta, Prec, Rec, Acc);
  }
}

void TBowLearn::TrainWinnowMd(const PBowDocBs& BowDocBs,
 const TStr& TargetCatNm, TFltV& ExpWgtV,
 const TFileId& OutFile, const double& Beta){
  // prepare Winnow parameters
  double VoteTsh=0.5;
  // prepare shortcuts
  int TargetCId=BowDocBs->GetCId(TargetCatNm);
  int TrainDocs=BowDocBs->GetTrainDocs();
  // prepare mini-experts, two for each word
  ExpWgtV.PutAll(1);
  const double MnWgt=1e-15;
  double PrevAcc=0; double PrevPrec=0; double PrevRec=0; double PrevF1=0;
  const double MxDiff=-0.005; const int MxWorseIters=3; int WorseIters=0;
  const int MxIters=50; int IterN=0;
  while ((IterN<MxIters)&&(WorseIters<MxWorseIters)){
    IterN++;
    int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
    for (int DIdN=0; DIdN<TrainDocs; DIdN++){
      int DId=BowDocBs->GetTrainDId(DIdN);
      bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
      double PosWgt=0; double NegWgt=0;
      double OldSum=0; double NewSum=0;
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // change only experts of words that occur in the document
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
        int PosExpN=2*DocWId; int NegExpN=2*DocWId+1;
        OldSum+=ExpWgtV[PosExpN]+ExpWgtV[NegExpN];
        // penalize expert giving wrong class prediction
        int PenN=(ClassVal)?NegExpN:PosExpN;
        ExpWgtV[PenN]=ExpWgtV[PenN]*Beta;
        NewSum+=ExpWgtV[PosExpN]+ExpWgtV[NegExpN];
        PosWgt+=ExpWgtV[PosExpN]; NegWgt+=ExpWgtV[NegExpN];
      }
      // normalize all experts
      if (NewSum>MnWgt){
        for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
          int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
          ExpWgtV[2*DocWId]=ExpWgtV[2*DocWId]*OldSum/NewSum;
          ExpWgtV[2*DocWId+1]=ExpWgtV[2*DocWId+1]*OldSum/NewSum;
        }
      }
      bool PredClassVal;
      if (PosWgt+NegWgt==0){PredClassVal=TBool::GetRnd();}
      else {PredClassVal=(PosWgt/(PosWgt+NegWgt))>VoteTsh;}
      if (PredClassVal==ClassVal){
        if (PredClassVal){TruePos++;} else {TrueNeg++;}
      } else {
        if (PredClassVal){FalsePos++;} else {FalseNeg++;}
      }
      //printf(PredClassVal==ClassVal ? "." : (PredClassVal ? "+" : "-"));
    }
    // output temporary results
    double Acc=100*(TruePos+TrueNeg)/double(TrainDocs);
    double Prec=(TruePos/double(TruePos+FalsePos));
    double Rec=(TruePos/double(TruePos+FalseNeg));
    double F1=(2*Prec*Rec/(Prec+Rec));
    // check if the current iteration gave worse results then the previous
    if (((Acc-PrevAcc)<MxDiff)||((F1-PrevF1)<MxDiff)||(((Prec-PrevPrec)<MxDiff)&&
     ((Rec-PrevRec)<MxDiff))){WorseIters++;}
    else {WorseIters=0;}
    PrevAcc=Acc; PrevPrec=Prec; PrevRec=Rec; PrevF1=F1;
    // if we want to see contingency table info.
/*    printf("%d] %d True Pos, %d True Neg, %d False Pos, %d False Neg, Accuracy %0.2f%%\n",
     IterN, TruePos, TrueNeg, FalsePos, FalseNeg, Acc);
    fprintf(OutFile,
     "%d] %d True Pos, %d True Neg, %d False Pos, %d False Neg, Accuracy %0.2f%%\n",
     IterN, TruePos, TrueNeg, FalsePos, FalseNeg, Acc);
*/  if(IterN==1){fprintf(OutFile, "# IterN, Precision, Recall, F1, Accuracy\n");}
    printf("%d] Precision %0.2f, Recall %0.2f, F1 %0.2f, Accuracy %0.2f%%\n",
      IterN, Prec, Rec, F1, Acc);
    fprintf(OutFile,
      "%d %0.2f, %0.2f, %0.2f, %0.2f%%\n", IterN, Prec, Rec, F1, Acc);
  }
  // write the model
/*  fprintf(OutFile, "\n# Winnow Model\n");
  fprintf(OutFile, "# Feature, PosClass_Weight, NegClass_Weight\n");
  for (int WId=0; WId<BowDocBs->GetWords(); WId++){
    if((ExpWgtV[2*WId]>MnWgt)||(ExpWgtV[2*WId+1]>MnWgt)){
      fprintf(OutFile, "%s, %0.5f, %0.5f\n",
       BowDocBs->GetWordStr(WId).CStr(), ExpWgtV[2*WId], ExpWgtV[2*WId+1]);
    }
  }
*/
// used for testing paramatere range
//  fprintf(OutFile,
//   "%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", Beta, PrevPrec, PrevRec, PrevF1, PrevAcc);
}

void TBowLearn::TestWinnowMd(const PBowDocBs& BowDocBs,
 const TStr& TargetCatNm, const TFltV& ExpWgtV,
 const TFileId& TrainOutFile, const TFileId& TestOutFile, const TFileId& ErrFile,
 const double& /*Beta*/){
  // prepare Winnow threshold
  double VoteTsh=0.5;
  // prepare shortcuts
  int TargetCId=BowDocBs->GetCId(TargetCatNm);
  int TrainDocs=BowDocBs->GetTrainDocs();
  int TestDocs=BowDocBs->GetTestDocs();
  // run through training examples
  {int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
    double PosWgt=0; double NegWgt=0;
    int DocWIds=BowDocBs->GetDocWIds(DId);
    if (DocWIds==0){
      printf("It is hard to classify an empty document %s!\n", BowDocBs->GetDocNm(DId).CStr());
    }
    // check experts of words that occur in the document
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
      int PosExpN=2*DocWId; int NegExpN=2*DocWId+1;
      PosWgt+=ExpWgtV[PosExpN]; NegWgt+=ExpWgtV[NegExpN];
    }
    bool PredClassVal;
    if (PosWgt+NegWgt==0){PredClassVal=TBool::GetRnd();}
    else {PredClassVal=(PosWgt/(PosWgt+NegWgt))>VoteTsh;}
    if (PredClassVal==ClassVal){
      if (PredClassVal){TruePos++;} else {TrueNeg++;}
    } else {// Error In Classification
      if (PredClassVal){FalsePos++;} else {FalseNeg++;}
    }
    int ClassValN=(ClassVal)?1:-1;
    fprintf(TrainOutFile, "%0.6f # true=%d # docDesc=%s\n",
     (PosWgt/(PosWgt+NegWgt))-VoteTsh, ClassValN, BowDocBs->GetDocNm(DId).CStr());
  }}
  // run through testing examples
  {int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<TestDocs; DIdN++){
    int DId=BowDocBs->GetTestDId(DIdN);
    bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
    double PosWgt=0; double NegWgt=0;
    int DocWIds=BowDocBs->GetDocWIds(DId);
    if (DocWIds==0){
      printf("It is hard to classify an empty document %s!\n", BowDocBs->GetDocNm(DId).CStr());
    }
    // check experts of words that occur in the document
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
      int PosExpN=2*DocWId; int NegExpN=2*DocWId+1;
      PosWgt+=ExpWgtV[PosExpN]; NegWgt+=ExpWgtV[NegExpN];
    }
    bool PredClassVal; double PredVal;
    if (PosWgt+NegWgt==0){
      PredClassVal=TBool::GetRnd(); PredVal=double(PredClassVal);}
    else {
      PredClassVal=(PosWgt/(PosWgt+NegWgt))>VoteTsh;
      PredVal=double(PosWgt/(PosWgt+NegWgt))-VoteTsh;
    }
    if (PredClassVal==ClassVal){
//      printf("%d ", DIdN);
      if (PredClassVal){TruePos++;} else {TrueNeg++;}
    } else {// Error In Classification
      fprintf(ErrFile, "%s\n", BowDocBs->GetDocNm(DId).CStr());
      if (PredClassVal){FalsePos++;} else {FalseNeg++;}
    }
    int ClassValN=(ClassVal)?1:-1;
    // output classification results
 //   printf("%0.6f # true=%d # docDesc=%s\n", (PosWgt/(PosWgt+NegWgt))-VoteTsh, ClassValN, BowDocBs->GetDocNm(DId).CStr());
    fprintf(TestOutFile, "%0.6f # true=%d # docDesc=%s\n",
     PredVal, ClassValN, BowDocBs->GetDocNm(DId).CStr());
  }
  double Acc=100*(TruePos+TrueNeg)/double(TestDocs);
  double Prec=(TruePos/double(TruePos+FalsePos));
  double Rec=(TruePos/double(TruePos+FalseNeg));
  double F1=(2*Prec*Rec/(Prec+Rec));
  printf("Precision %0.2f%%, Recall %0.2f%%, F1 %0.2f, Accuracy %0.2f%%\n", Prec, Rec, F1, Acc);
// used for testing paramatere range
//  fprintf(ErrFile, "%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", Beta, Prec, Rec, F1, Acc);
  }
}

void TBowLearn::TrainPerceptronMd(const PBowDocBs& BowDocBs,
 const TStr& TargetCatNm, TFltV& ExpWgtV,
 const TFileId& OutFile, const double& Beta, const double& VoteTsh){
  // prepare shortcuts
  int TargetCId=BowDocBs->GetCId(TargetCatNm);
  int TrainDocs=BowDocBs->GetTrainDocs();
  // prepare weight for each word
  ExpWgtV.PutAll(0);
  //const double MnWgt= 1e-15;
  double PrevAcc=0; double PrevPrec=0; double PrevRec=0; double PrevF1=0;
  const double MxDiff=-0.005; const int MxWorseIters=3; int WorseIters=0;
  const int MxIters=30; int IterN=0;
  while ((IterN<MxIters)&&(WorseIters<MxWorseIters)){
    IterN++;
    int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
    for (int DIdN=0; DIdN<TrainDocs; DIdN++){
      int DId=BowDocBs->GetTrainDId(DIdN);
      bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
      double Wgt=0;
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // change only wight of words that occur in the document
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
        Wgt+=ExpWgtV[DocWId];
      }
      bool PredClassVal;
      if (Wgt==0){PredClassVal=TBool::GetRnd();}
      else {PredClassVal=(Wgt>VoteTsh);}
      if ((PredClassVal==ClassVal)&&(fabs(Wgt)>VoteTsh)){
//      if ((PredClassVal==ClassVal)){
        if (PredClassVal){TruePos++;} else {TrueNeg++;}
      } else { // penalize wrong class prediction
        if (PredClassVal){FalsePos++;} else {FalseNeg++;}
        for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
          int DocWId; double WordFq; BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, WordFq);
          // additive
          if (PredClassVal){ExpWgtV[DocWId]-=Beta*WordFq;} else {ExpWgtV[DocWId]+=Beta*WordFq;}
        }
      }
    }
    // output temporary results
    double Acc=100*(TruePos+TrueNeg)/double(TrainDocs);
    double Prec=(TruePos/double(TruePos+FalsePos));
    double Rec=(TruePos/double(TruePos+FalseNeg));
    double F1=(2*Prec*Rec/(Prec+Rec));
    // check if the current iteration gave worse results then the previous
    if (((Acc-PrevAcc)<MxDiff)||((F1-PrevF1)<MxDiff)||(((Prec-PrevPrec)<MxDiff)&&
     ((Rec-PrevRec)<MxDiff))){WorseIters++;}
    else {WorseIters=0;}
    PrevAcc=Acc; PrevPrec=Prec; PrevRec=Rec; PrevF1=F1;
    // if we want to see contingency table info.
//    printf("%d] %d True Pos, %d True Neg, %d False Pos, %d False Neg, Accuracy %0.2f%%\n",
//     IterN, TruePos, TrueNeg, FalsePos, FalseNeg, Acc);
//    fprintf(OutFile,
//     "%d] %d True Pos, %d True Neg, %d False Pos, %d False Neg, Accuracy %0.2f%%\n",
//     IterN, TruePos, TrueNeg, FalsePos, FalseNeg, Acc);
    if(IterN==1){fprintf(OutFile, "# IterN, Precision, Recall, F1, Accuracy\n");}
    printf("%d] Precision %0.2f, Recall %0.2f, F1 %0.4f, Accuracy %0.2f%%\n",
      IterN, Prec, Rec, F1, Acc);
    fprintf(OutFile,
      "%d %0.2f, %0.2f, %0.2f, %0.2f%%\n", IterN, Prec, Rec, F1, Acc);
  }
  // write the model
/*  fprintf(OutFile, "\n# Perceptron Model\n");
  fprintf(OutFile, "# Feature, PosClass_Weight\n");
  for (int WId=0; WId<BowDocBs->GetWords(); WId++){
    if(ExpWgtV[WId]>MnWgt){
      fprintf(OutFile, "%s, %0.5f\n", BowDocBs->GetWordStr(WId).CStr(), ExpWgtV[WId]);
    }
  }
*/
// used for testing paramatere range
//  fprintf(OutFile,
//   "%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", Beta, PrevPrec, PrevRec, PrevF1, PrevAcc);
//  fprintf(OutFile,
//   "%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", VoteTsh, PrevPrec, PrevRec, PrevF1, PrevAcc);
}

void TBowLearn::TestPerceptronMd(const PBowDocBs& BowDocBs,
 const TStr& TargetCatNm, const TFltV& ExpWgtV,
 const TFileId& TrainOutFile, const TFileId& TestOutFile, const TFileId& ErrFile,
 const double& /*Beta*/){
  // prepare threshold
  double VoteTsh=0.0;
  const double MxWgt=1e+15;
  // prepare shortcuts
  int TargetCId=BowDocBs->GetCId(TargetCatNm);
  int TrainDocs=BowDocBs->GetTrainDocs();
  int TestDocs=BowDocBs->GetTestDocs();
  // run through training examples
  {int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<TrainDocs; DIdN++){
    int DId=BowDocBs->GetTrainDId(DIdN);
    bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
    double Wgt=0;
    int DocWIds=BowDocBs->GetDocWIds(DId);
    if (DocWIds==0){
      printf("It is hard to classify an empty document %s!\n", BowDocBs->GetDocNm(DId).CStr());
    }
    // check experts of words that occur in the document
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
      Wgt+=ExpWgtV[DocWId];
    }
    bool PredClassVal;
    if (Wgt==0){PredClassVal=TBool::GetRnd();}
    else {PredClassVal=(Wgt>VoteTsh);}
    if (PredClassVal==ClassVal){
      if (PredClassVal){TruePos++;} else {TrueNeg++;}
    } else {// Error In Classification
      if (PredClassVal){FalsePos++;} else {FalseNeg++;}
    }
    int ClassValN=(ClassVal)?1:-1;
    fprintf(TrainOutFile, "%0.6f # true=%d # docDesc=%s\n",
     Wgt-VoteTsh, ClassValN, BowDocBs->GetDocNm(DId).CStr());
  }}
  // run through testing examples
  {int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
  for (int DIdN=0; DIdN<TestDocs; DIdN++){
    int DId=BowDocBs->GetTestDId(DIdN);
    bool ClassVal=BowDocBs->IsCatInDoc(DId, TargetCId);
    double Wgt=0;
    int DocWIds=BowDocBs->GetDocWIds(DId);
    if (DocWIds==0){
      printf("It is hard to classify an empty document %s!\n", BowDocBs->GetDocNm(DId).CStr());
    }
    // check experts of words that occur in the document
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      int DocWId=BowDocBs->GetDocWId(DId, DocWIdN);
      Wgt+=ExpWgtV[DocWId];
    }
    if(Wgt>MxWgt){Wgt=MxWgt;}
    bool PredClassVal=(Wgt>VoteTsh);
    if (PredClassVal==ClassVal){
      if (PredClassVal){TruePos++;} else {TrueNeg++;}
    } else {// Error In Classification
      fprintf(ErrFile, "%s\n", BowDocBs->GetDocNm(DId).CStr());
      if (PredClassVal){FalsePos++;} else {FalseNeg++;}
    }
    int ClassValN=(ClassVal)?1:-1;
    // output classification results
//    printf("%0.6f # true=%d # docDesc=%s\n", Wgt-VoteTsh, ClassValN, BowDocBs->GetDocNm(DId).CStr());
    fprintf(TestOutFile, "%0.6f # true=%d # docDesc=%s\n",
     Wgt-VoteTsh, ClassValN, BowDocBs->GetDocNm(DId).CStr());
  }
  double Acc=100*(TruePos+TrueNeg)/double(TestDocs);
  double Prec=(TruePos/double(TruePos+FalsePos));
  double Rec=(TruePos/double(TruePos+FalseNeg));
  double F1=(2*Prec*Rec/(Prec+Rec));
  printf("Precision %0.2f, Recall %0.2f, F1: %0.2f, Accuracy %0.2f%%\n", Prec, Rec, F1, Acc);
// used for testing paramatere range
//  fprintf(ErrFile, "%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", Beta, Prec, Rec, F1, Acc);
  }
}


