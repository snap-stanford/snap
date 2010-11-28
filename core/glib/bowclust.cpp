/////////////////////////////////////////////////
// BagOfWords-Document-Partition-Cluster
void TBowDocPartClust::CalcEntropy(const PBowDocBs& BowDocBs){
  // prepare data
  int Docs=GetDocs();
  int Cats=BowDocBs->GetCats();
  // calculate category weights
  TFltV CatWgtV(Cats);
  double SumCatWgt=0;
  for (int DIdN=0; DIdN<Docs; DIdN++){
    int DId=DIdV[DIdN];
    int DocCats=BowDocBs->GetDocCIds(DId);
    for (int DocCIdN=0; DocCIdN<DocCats; DocCIdN++){
      int CId=BowDocBs->GetDocCId(DId, DocCIdN);
      double CatWgt=1/double(DocCats);
      CatWgtV[CId]+=CatWgt; SumCatWgt+=CatWgt;
    }
  }
  // normalize category weights to sum 1
  int NonZeroCatWgts=0;
  if (SumCatWgt>0){
    for (int CId=0; CId<Cats; CId++){
      CatWgtV[CId]/=SumCatWgt;
      if (double(CatWgtV[CId])!=0){NonZeroCatWgts++;}
    }
  }
  // calculate entropy
  CIdPrbKdV.Gen(NonZeroCatWgts, 0); Entropy=0;
  for (int CId=0; CId<Cats; CId++){
    double Prb=CatWgtV[CId];
    if (Prb>0){
      Entropy-=Prb*log(Prb);
      CIdPrbKdV.Add(TIntFltKd(CId, Prb));
    }
  }
}

void TBowDocPartClust::GetTopWordStrWgtPrV(
 const PBowDocBs& BowDocBs,
 const int& TopWords, const double& TopWordsWgtPrc,
 TStrFltPrV& WordStrWgtPrV) const {
  // get word-weight/string pair vector
  int WIds=GetConceptWords();
  TFltIntKdV WordWgtIdKdV(WIds); double WordWgtSum=0;
  for (int WIdN=0; WIdN<WIds; WIdN++){
    int WId; double WordWgt;
    GetConceptSpV()->GetWIdWgt(WIdN, WId, WordWgt);
    WordWgtSum+=WordWgt;
    WordWgtIdKdV[WIdN]=TFltIntKd(WordWgt, WId);
  }
  WordWgtIdKdV.Sort(false);
  double CutWordWgtSum=TopWordsWgtPrc*WordWgtSum;
  // extract top words
  WordStrWgtPrV.Clr();
  {for (int WIdN=0; WIdN<WIds; WIdN++){
    // get word
    double WordWgt=WordWgtIdKdV[WIdN].Key;
    int WId=WordWgtIdKdV[WIdN].Dat;
    TStr WordStr=BowDocBs->GetWordStr(WId);
    // add word-string/weight pair
    WordStrWgtPrV.Add(TStrFltPr(WordStr, WordWgt));
    // stopping criterion
    if ((TopWords!=-1)&&(1+WIdN>=TopWords)){break;}
    CutWordWgtSum-=WordWgt; if (CutWordWgtSum<0){break;}
  }}
}

double TBowDocPartClust::GetCatPrb(const int& CId) const {
  int CatN;
  if (CIdPrbKdV.IsIn(TIntFltKd(CId, 0), CatN)){
    return CIdPrbKdV[CatN].Dat;
  } else {
    return 0;
  }
}

int TBowDocPartClust::GetMxPrbCId() const {
  int MxCId=-1; double MxPrb=0;
  int CIdPrbs=GetCIdPrbs();
  for (int CIdPrbN=0; CIdPrbN<CIdPrbs; CIdPrbN++){
    int CId; double Prb; GetCIdPrb(CIdPrbN, CId, Prb);
    if ((CIdPrbN==0)||(Prb>MxPrb)){MxCId=CId; MxPrb=Prb;}
  }
  /*// random category
  TRnd Rnd(0);
  int CIdPrbN=Rnd.GetUniDevInt(CIdPrbs);
  int CId; double Prb; GetCIdPrb(CIdPrbN, CId, Prb);
  MxCId=CId;*/
  // return results
  return MxCId;
}

/////////////////////////////////////////////////
// BagOfWords-Document-Partition
void TBowDocPart::GetTopClustSimV(
 const double& ClustSimSumPrc, TFltIntIntTrV& ClustSimN1N2TrV) const {
  ClustSimN1N2TrV.Clr();
  // collect similarities
  int Clusts=GetClusts();
  double ClustSimSum=0;
  for (int ClustN1=0; ClustN1<Clusts; ClustN1++){
    for (int ClustN2=ClustN1+1; ClustN2<Clusts; ClustN2++){
      double ClustSim=GetClustSim(ClustN1, ClustN2);
      ClustSimN1N2TrV.Add(TFltIntIntTr(ClustSim, ClustN1, ClustN2));
      ClustSimSum+=ClustSim;
    }
  }
  // extract top similarities
  ClustSimN1N2TrV.Sort(false);
  double CutClustSimSum=(1-ClustSimSumPrc)*ClustSimSum;
  while (!ClustSimN1N2TrV.Empty()){
    CutClustSimSum-=ClustSimN1N2TrV.Last().Val1;
    if (CutClustSimSum<=0){
      break;
    } else {
      ClustSimN1N2TrV.DelLast();
    }
  }
}

int TBowDocPart::GetDocs() const {
  int Docs=0;
  int Clusts=GetClusts();
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    Docs+=GetClust(ClustN)->GetDocs();}
  return Docs;
}

void TBowDocPart::GetDIdV(TIntV& DIdV) const {
  DIdV.Gen(GetDocs(), 0);
  int Clusts=GetClusts();
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    TIntV DIdV; GetClust(ClustN)->GetDIdV(DIdV);
    DIdV.AddV(DIdV);
  }
}

int TBowDocPart::GetBestClust(const PBowSpV& SpV, const PBowSim& BowSim) const {
  // find best cluster
  int BestClustN=-1; double BestSim=0;
  int Clusts=GetClusts(); IAssert(Clusts>0);
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    PBowDocPartClust Clust=GetClust(ClustN);
    IAssert(Clust->IsConceptSpV());
    double Sim=BowSim->GetSim(SpV, Clust->GetConceptSpV());
    if ((BestClustN==-1)||(Sim>BestSim)){
      BestClustN=ClustN; BestSim=Sim;
    }
  }
  return BestClustN;
}

int TBowDocPart::GetBestCId(const PBowSpV& SpV, const PBowSim& BowSim) const {
  // get best cluster
  int BestClustN=GetBestClust(SpV, BowSim);
  PBowDocPartClust BestClust=GetClust(BestClustN);
  // get best category
  int BestCId=-1;
  if (BestClust->IsSubPart()){
    // get the best category from the best cluster subpartition
    BestCId=BestClust->GetSubPart()->GetBestCId(SpV, BowSim);
  } else {
    // get the category with the biggest probability
    BestCId=BestClust->GetMxPrbCId();
  }
  // return results
  return BestCId;
}

void TBowDocPart::GetClustSimV(
 const PBowSpV& SpV, const PBowSim& BowSim, TIntFltKdV& ClustSimV) const {
  // calculate similarity between given document and clusters
  int Clusts=GetClusts(); IAssert(Clusts>0);
  ClustSimV.Gen(Clusts, 0);
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    PBowDocPartClust Clust=GetClust(ClustN);
    IAssert(Clust->IsConceptSpV());
    double Sim=BowSim->GetSim(SpV, Clust->GetConceptSpV());
    ClustSimV.Add(TIntFltKd(ClustN, Sim));
  }
  IAssert(ClustSimV.Len() == Clusts);
}

void TBowDocPart::GetDefClustNmV(const TStr& ClustNmPfx, TStrV& ClustNmV) const {
  for (int ClustN=0; ClustN<GetClusts(); ClustN++){
    // create & add cluster name
    TStr ClustNm=ClustNmPfx+"_"+TInt::GetStr(ClustN);
    ClustNmV.Add(ClustNm);
    // get cluster and recurse
    PBowDocPartClust Clust=GetClust(ClustN);
    if (Clust->IsSubPart()){
      PBowDocPart SubPart=Clust->GetSubPart();
      SubPart->GetDefClustNmV(ClustNm, ClustNmV);
    }
  }
}

void TBowDocPart::GetDefClustNmV(TStrV& ClustNmV) const {
  ClustNmV.Clr();
  for (int ClustN=0; ClustN<GetClusts(); ClustN++){
    // create & add cluster name
    TStr ClustNm=TStr("C")+TInt::GetStr(ClustN);
    ClustNmV.Add(ClustNm);
    // get cluster and recurse
    PBowDocPartClust Clust=GetClust(ClustN);
    if (Clust->IsSubPart()){
      PBowDocPart SubPart=Clust->GetSubPart();
      SubPart->GetDefClustNmV(ClustNm, ClustNmV);
    }
  }
}

void TBowDocPart::GetCfClustNmV(
 const PBowSpV TestDocSpV, const PBowSim& BowSim, TStrV& ClustNmV) const {
  int ClustN=GetBestClust(TestDocSpV, BowSim);
  PBowDocPartClust Clust=GetClust(ClustN);
  TStr ClustNm=ClustNmV.Last()+"_"+TInt::GetStr(ClustN);
  ClustNmV.Add(ClustNm);
  if (Clust->IsSubPart()){
    PBowDocPart SubPart=Clust->GetSubPart();
    SubPart->GetCfClustNmV(TestDocSpV, BowSim, ClustNmV);
  }
}

void TBowDocPart::GetCfClustNmVV(
 const PBowDocBs& BowDocBs, const TBowWordWgtType& WordWgtType,
 const PBowSim& BowSim, const TIntV& TrainDIdV, const TIntV& TestDIdV,
 TVec<TStrV>& ClustNmVV, TVec<TBowDocPartClustV>& ClustVV) const {
  // create document-weights-bases
  PBowDocWgtBs TrainDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0, TrainDIdV);
  PBowDocWgtBs TestDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0, TestDIdV, TrainDIdV);
  int TestDocs=TestDocWgtBs->GetDocs();
  // classify test docs
  ClustNmVV.Clr(); ClustVV.Clr();
  for (int TestDIdN=0; TestDIdN<TestDocs; TestDIdN++){
    // get test document
    int TestDId=TestDIdV[TestDIdN];
    PBowSpV TestDocSpV=TestDocWgtBs->GetSpV(TestDId);
    // get best cluster
    TStrV ClustNmV;
    int ClustN=GetBestClust(TestDocSpV, BowSim);
    PBowDocPartClust Clust=GetClust(ClustN);
    TStr ClustNm=TStr("C")+TInt::GetStr(ClustN);
    ClustNmV.Add(ClustNm);
    // get best sub clusters
    if (Clust->IsSubPart()){
      PBowDocPart SubPart=Clust->GetSubPart();
      SubPart->GetCfClustNmV(TestDocSpV, BowSim, ClustNmV);
    }
    // add cluster names
    ClustNmVV.Add(ClustNmV); ClustVV.Add(ClustV);
  }
}

void TBowDocPart::SaveXmlCfClustNmVV(
 const PBowDocBs& BowDocBs, const TBowWordWgtType& WordWgtType,
 const PBowSim& BowSim, const TIntV& TrainDIdV, const TIntV& _TestDIdV,
 const TStr& FNm){
  // get testing doc-ids
  TIntV TestDIdV;
  if (_TestDIdV.Empty()){BowDocBs->GetAllDIdV(TestDIdV);}
  else {TestDIdV=_TestDIdV;}

  // get classified cluster names for test documents
  TVec<TStrV> CfClustNmVV; TVec<TBowDocPartClustV> CfClustVV;
  GetCfClustNmVV(
   BowDocBs, WordWgtType, BowSim, TrainDIdV, TestDIdV, CfClustNmVV, CfClustVV);

  // create xml file
  TFOut SOut(FNm); TFileId OutFId=SOut.GetFileId();
  // header
  fprintf(OutFId, "<DocClusterCf>\n");
  // get & save cluster name definitions
  fprintf(OutFId, "<ClusterDefs>\n");
  TStrV DefClustNmV; GetDefClustNmV(DefClustNmV);
  for (int DefClustNmN=0; DefClustNmN<DefClustNmV.Len(); DefClustNmN++){
    fprintf(OutFId, "<ClusterDef>%s</ClusterDef>\n", DefClustNmV[DefClustNmN].CStr());}
  fprintf(OutFId, "</ClusterDefs>\n");
  // save classification cluster names
  fprintf(OutFId, "<DocCfs>\n");
  int TestDocs=TestDIdV.Len();
  for (int TestDIdN=0; TestDIdN<TestDocs; TestDIdN++){
    int TestDId=TestDIdV[TestDIdN];
    TStr TestDocNm=BowDocBs->GetDocNm(TestDId);
    fprintf(OutFId, "<DocCf Nm=\"%s\">", TestDocNm.CStr());
    TStrV& CfClustNmV=CfClustNmVV[TestDIdN];
    for (int CfClustNmN=0; CfClustNmN<CfClustNmV.Len(); CfClustNmN++){
      fprintf(OutFId, "<ClusterCf>%s</ClusterCf>", CfClustNmV[CfClustNmN].CStr());
    }
    fprintf(OutFId, "</DocCf>\n");
  }
  fprintf(OutFId, "</DocCfs>\n");
  // footer
  fprintf(OutFId, "</DocClusterCf>\n");
}

void TBowDocPart::GetAreaPart(
 const TFltRect& Rect, TClustRectPrV& ClustRectPrV, const bool& ShuffleP) const {
  // shortcuts
  int Clusts=GetClusts(); int Docs=GetDocs();
  // do nothing if no clusters or no documents
  if ((Clusts==0)||(Docs==0)){return;}
  // previous X & Y
  double PrevMxX=Rect.MnX; double PrevMxY=Rect.MnY;
  // traverse clusters
  TIntV ClustNV(Clusts, 0);
  for (int ClustN=0; ClustN<Clusts; ClustN++){ClustNV.Add(ClustN);}
  if (ShuffleP){TRnd Rnd; ClustNV.Shuffle(Rnd);}
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    PBowDocPartClust Clust=GetClust(ClustNV[ClustN]);
    int ClustDocs=Clust->GetDocs();
    // calculate sub-rectangle
    TFltRect SubRect;
    if (Rect.GetXLen()>=Rect.GetYLen()){
      SubRect.MnX=PrevMxX;
      SubRect.MxX=PrevMxX=PrevMxX+(ClustDocs/double(Docs))*Rect.GetXLen();
      SubRect.MnY=Rect.MnY;
      SubRect.MxY=Rect.MxY;
    } else {
      SubRect.MnX=Rect.MnX;
      SubRect.MxX=Rect.MxX;
      SubRect.MnY=PrevMxY;
      SubRect.MxY=PrevMxY=PrevMxY+(ClustDocs/double(Docs))*Rect.GetYLen();
    }
    // add cluster/sub-rectangle pair
    ClustRectPrV.Add(TClustRectPr(Clust, SubRect));
    // recurse
    if (Clust->IsSubPart()){
      PBowDocPart SubPart=Clust->GetSubPart();
      SubPart->GetAreaPart(SubRect, ClustRectPrV);
    }
  }
}

void TBowDocPart::GetClustAtXY(const double& X, const double Y,
 const TClustRectPrV& ClustRectPrV, const bool& LeafPartP,
 TBowDocPartClustV& ClustV){
  ClustV.Clr();
  for (int ClustRectPrN=0; ClustRectPrN<ClustRectPrV.Len(); ClustRectPrN++){
    PBowDocPartClust Clust=ClustRectPrV[ClustRectPrN].Val1;
    TFltRect Rect=ClustRectPrV[ClustRectPrN].Val2;
    if (Rect.IsXYIn(X, Y)){
      if (((LeafPartP)&&(!Clust->IsSubPart()))||(!LeafPartP)){
        ClustV.Add(Clust);
      }
    }
  }
}

void TBowDocPart::SavePartEntropyTxt(
 const PSOut& SOut, const PBowDocBs& BowDocBs) const {
  // prepare data
  int Clusts=GetClusts();
  // output intercluster similarity
  SOut->PutStr("----------------------------\n");
  SOut->PutStr("Inter Cluster Similarity\n");
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    SOut->PutInt(ClustN, "\tC-%d");}
  SOut->PutLn();
  for (int ClustN1=0; ClustN1<Clusts; ClustN1++){
    SOut->PutInt(ClustN1, "C-%d");
    for (int ClustN2=0; ClustN2<Clusts; ClustN2++){
      if (ClustN1>=ClustN2){
        SOut->PutStr("\t.");
      } else {
        double Sim=GetClustSim(ClustN1, ClustN2);
        SOut->PutFlt(Sim, "\t%.3f");
      }
    }
    SOut->PutLn();
  }
  // output cluster entropy
  SOut->PutStr("----------------------------\n");
  SOut->PutStr("Cluster Summaries\n");
  {for (int ClustN=0; ClustN<Clusts; ClustN++){
    PBowDocPartClust Clust=GetClust(ClustN);
    SOut->PutInt(ClustN, "C-%d:");
    SOut->PutFlt(Clust->GetEntropy(), " [Entropy %0.3f]");
    //SOut->PutFlt(Clust->GetQual(), " [Quality %g]");
    SOut->PutFlt(Clust->GetMeanSim(), " [Mean Sim. %0.2f]");
    SOut->PutInt(Clust->GetDocs(), " [%d Docs.]");
    SOut->PutInt(Clust->GetConceptWords(), " [%d Words]\n");
  }}
  // output cluster/category probabilities
  SOut->PutStr("----------------------------\n");
  SOut->PutStr("Category/Cluster Probabilities\n");
  SOut->PutStr(" ", "%20s");
  {for (int ClustN=0; ClustN<Clusts; ClustN++){
    SOut->PutInt(ClustN, "\tC-%d");
  }}
  SOut->PutLn();
  for (int CId=0; CId<BowDocBs->GetCats(); CId++){
    SOut->PutStr(BowDocBs->GetCatNm(CId), "%20s");
    for (int ClustN=0; ClustN<Clusts; ClustN++){
      PBowDocPartClust Clust=GetClust(ClustN);
      SOut->PutStr("\t");
      double CatPrb=Clust->GetCatPrb(CId);
      if (CatPrb>0){
        int MxPrbCId=Clust->GetMxPrbCId();
        SOut->PutFlt(CatPrb, "%0.3f");
        if (MxPrbCId==CId){SOut->PutStr("!");}
      } else {SOut->PutStr(".");}
    }
    SOut->PutLn();
  }
}

void TBowDocPart::SaveTxt(
 const PSOut& SOut, const PBowDocBs& BowDocBs,
 const bool& SaveWordsP, const int& TopWords, const double& TopWordsWgtPrc,
 const bool& SaveDocsP, const TStr& PathStr) const {
  SOut->PutStr("***********************************\n");
  // save header
  SOut->PutStr(PathStr, "Path: %s\n");
  SOut->PutInt(GetClusts(), "Clusters: %d\n");
  SOut->PutInt(GetDocs(), "Documents: %d\n");
  SOut->PutFlt(GetQual(), "Quality: %g\n");
  SOut->PutFlt(GetMeanSim(), "Mean Similarity: %.3f\n");
  if (!GetNm().Empty()){
    SOut->PutStr(GetNm(), "Name: \"%s\"\n");}
  SOut->Flush();
  // save words for clusters
  for (int ClustN=0; ClustN<GetClusts(); ClustN++){
    PBowDocPartClust Clust=GetClust(ClustN);
    SOut->PutStr("===================================\n");
    SOut->PutInt(ClustN, "Cluster-%d: ");
    //SOut->PutFlt(Clust->GetQual(), " [Quality %g]");
    SOut->PutFlt(Clust->GetMeanSim(), " [Mean Sim. %0.3f]");
    SOut->PutInt(Clust->GetDocs(), " [%d Docs.]");
    SOut->PutInt(Clust->GetConceptWords(), " [%d Words]\n");
    if (!Clust->GetNm().Empty()){
      SOut->PutStr(Clust->GetNm(), "[Name: \"%s\"]\n");}
    if ((SaveWordsP)&&(!BowDocBs.Empty())){
      SOut->PutStr("---Words------------------------\n");
      int WIds=Clust->GetConceptWords();
      TFltIntKdV WordWgtIdKdV(WIds); double WordWgtSum=0;
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId; double WordWgt;
        Clust->GetConceptSpV()->GetWIdWgt(WIdN, WId, WordWgt);
        WordWgtSum+=WordWgt;
        WordWgtIdKdV[WIdN]=TFltIntKd(WordWgt, WId);
      }
      WordWgtIdKdV.Sort(false);
      double WordWgtSumSF=0;
      {for (int WIdN=0; WIdN<WIds; WIdN++){
        if (TopWords==WIdN){break;}
        if ((WordWgtSum>0)&&(WordWgtSumSF/WordWgtSum>TopWordsWgtPrc)){break;}
        double WordWgt=WordWgtIdKdV[WIdN].Key;
        WordWgtSumSF+=WordWgt;
        int WId=WordWgtIdKdV[WIdN].Dat;
        TStr WordStr=BowDocBs->GetWordStr(WId);
        SOut->PutStr(WordStr, "'%s':");
        SOut->PutFlt(WordWgt, "%.3f\n");
      }}
    }
    SOut->Flush();
    if (SaveDocsP){
      SOut->PutStr("---Documents--------------------\n");
      int Docs=Clust->GetDocs();
      if (Clust->IsDCSimV()){
        // create vector sorted according to similarity to centroid
        TFltIntPrV DCSimDIdPrV(Docs, 0);
        for (int DIdN=0; DIdN<Docs; DIdN++){
          DCSimDIdPrV.Add(TFltIntPr(Clust->GetDCSim(DIdN), Clust->GetDId(DIdN)));
        }
        DCSimDIdPrV.Sort(false);
        // output the vector
        {for (int DIdN=0; DIdN<Docs; DIdN++){
          double DCSim=DCSimDIdPrV[DIdN].Val1;
          int DId=DCSimDIdPrV[DIdN].Val2;
          SOut->PutStr(BowDocBs->GetDocNm(DId), "'%s'");
          SOut->PutFlt(DCSim, " : %g");
          if (!BowDocBs->GetDocDescStr(DId).Empty()){
            SOut->PutStr(BowDocBs->GetDocDescStr(DId), " [%s]");}
          SOut->PutStr("\n");
        }}
      } else {
        // output just document names
        for (int DIdN=0; DIdN<Docs; DIdN++){
          int DId=Clust->GetDId(DIdN);
          SOut->PutStr(BowDocBs->GetDocNm(DId), "'%s'");
          if (!BowDocBs->GetDocDescStr(DId).Empty()){
            SOut->PutStr(BowDocBs->GetDocDescStr(DId), " [%s]");}
          SOut->PutStr("\n");
        }
      }
      if (Clust->IsDDSimVV()){
        SOut->PutStr("---Intra-Document-Similarity----\n");
        for (int DIdN1=0; DIdN1<Docs; DIdN1++){
          int DId1=Clust->GetDId(DIdN1);
          SOut->PutStr(BowDocBs->GetDocNm(DId1), "'%s': ");
          for (int DIdN2=0; DIdN2<Docs; DIdN2++){
            double DDSim=Clust->GetDDSim(DIdN1, DIdN2);
            SOut->PutFlt(DDSim, " %.4f");
          }
          SOut->PutLn();
        }
      }
    }
  }
  // save entropy
  if (BowDocBs->GetCats()>0){
    SavePartEntropyTxt(SOut, BowDocBs);}
  // save subpartitions
  {for (int ClustN=0; ClustN<GetClusts(); ClustN++){
    PBowDocPart SubPart=GetClust(ClustN)->GetSubPart();
    if (!SubPart.Empty()){
      TStr NewPathStr=PathStr+"|"+TInt::GetStr(ClustN);
      SubPart->SaveTxt(SOut, BowDocBs,
       SaveWordsP, TopWords, TopWordsWgtPrc, SaveDocsP, NewPathStr);
    }
  }}
  SOut->Flush();
}

void TBowDocPart::SaveXml(
 const PSOut& SOut, const PBowDocBs& BowDocBs, const TStr& PathStr) const {
  // save header
  SOut->PutStr("<Partition");
  SOut->PutStr(PathStr, " Path=\"%s\"");
  SOut->PutInt(GetClusts(), " Clusts=\"%d\"");
  SOut->PutInt(GetDocs(), " Documents=\"%d\"");
  SOut->PutFlt(GetQual(), " Quality=\"%g\"");
  SOut->PutFlt(GetMeanSim(), " MeanSimilarity=\"%g\"");
  if (!GetNm().Empty()){
    SOut->PutStr(GetNm(), " Name=\"%s\"");}
  SOut->PutStr(">\n");
  // save words for clusters
  for (int ClustN=0; ClustN<GetClusts(); ClustN++){
    PBowDocPartClust Clust=GetClust(ClustN);
    SOut->PutStr("<Cluster");
    SOut->PutInt(ClustN, " Id=\"%d\"");
    SOut->PutFlt(Clust->GetQual(), " Quality=\"%g\"");
    SOut->PutFlt(Clust->GetMeanSim(), " MeanSimilarity=\"%g\"");
    SOut->PutFlt(Clust->GetEntropy(), " Entropy=\"%g\"");
    if (!Clust->GetNm().Empty()){
      SOut->PutStr(Clust->GetNm(), " Name=\"%s\"");}
    SOut->PutStr(">\n");
    // class distribution
    if (BowDocBs->GetCats()>0){
      int MxPrbCId=Clust->GetMxPrbCId();
      SOut->PutStr("<ClassDistr");
      SOut->PutInt(MxPrbCId, " BestId=\"%d\"");
      SOut->PutStr(">");
      for (int CId=0; CId<BowDocBs->GetCats(); CId++){
        SOut->PutStr("<Class");
        SOut->PutInt(CId, " Id=\"%d\"");
        SOut->PutStr(BowDocBs->GetCatNm(CId), " Name=\"%s\"");
        double CatPrb=Clust->GetCatPrb(CId);
        SOut->PutFlt(CatPrb, " Prob=\"%g\"");
        SOut->PutStr("/>");
      }
      SOut->PutStr("</ClassDistr>");
    }
    // documents
    int Docs=Clust->GetDocs();
    SOut->PutInt(Docs, "<Documents Count=\"%d\">\n");
    for (int DIdN=0; DIdN<Docs; DIdN++){
      int DId=Clust->GetDId(DIdN);
      SOut->PutStr(BowDocBs->GetDocNm(DId), "<D Nm=\"%s\"");
      SOut->PutInt(DId, " Id=\"%d\"");
      if (!BowDocBs->GetDocDescStr(DId).Empty()){
        SOut->PutStr(BowDocBs->GetDocDescStr(DId), " Desc=\"%s\"");}
      SOut->PutStr("/>");
    }
    SOut->PutStr("</Documents>\n");
    // words
    SOut->PutInt(Clust->GetConceptWords(), "<Words Count=\"%d\">\n");
    if (!BowDocBs.Empty()){
      int WIds=Clust->GetConceptWords();
      TFltIntKdV WordWgtIdKdV(WIds);
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId; double WordWgt;
        Clust->GetConceptSpV()->GetWIdWgt(WIdN, WId, WordWgt);
        WordWgtIdKdV[WIdN]=TFltIntKd(WordWgt, WId);
      }
      WordWgtIdKdV.Sort(false);
      {for (int WIdN=0; WIdN<WIds; WIdN++){
        double WordWgt=WordWgtIdKdV[WIdN].Key;
        int WId=WordWgtIdKdV[WIdN].Dat;
        SOut->PutStr(BowDocBs->GetWordStr(WId), "<W Nm=\"%s\" ");
        SOut->PutInt(WId, "Id=\"%d\" ");
        SOut->PutFlt(WordWgt, "Wgt=\"%g\"/>");
      }}
      SOut->PutLn();
    }
    SOut->PutStr("</Words>\n");
    SOut->PutStr("</Cluster>\n");
  }
  // save subpartitions
  {for (int ClustN=0; ClustN<GetClusts(); ClustN++){
    PBowDocPart SubPart=GetClust(ClustN)->GetSubPart();
    if (!SubPart.Empty()){
      TStr NewPathStr=PathStr+"|"+TInt::GetStr(ClustN);
      SubPart->SaveXml(SOut, BowDocBs, NewPathStr);
    }
  }}
  SOut->Flush();
  SOut->PutStr("</Partition>\n");
}

//B:
void TBowDocPart::SaveRdfClusts(const PSOut& SOut, const PBowDocBs& BowDocBs, 
        TIntIntVH& DIdClustIdVH, const int& FatherId, int& ClustId) const {

    for (int ClustN=0; ClustN<GetClusts(); ClustN++) {
        PBowDocPartClust Clust=GetClust(ClustN);
        TStr CptNm, KeyWdStr;
        
        if (!Clust->GetNm().Empty()){
            CptNm = Clust->GetNm(); }
        if (Clust->IsConceptSpV()) {
            KeyWdStr = Clust->GetConceptSpV()->GetStr(BowDocBs, 20); 
            if (CptNm.Empty()) {
                CptNm = Clust->GetConceptSpV()->GetStr(BowDocBs, 5, 1, ", ", false); }
        }
        if (CptNm.Empty()) {
            CptNm = TStr::Fmt("Clust%d", ClustId); }

        SOut->PutStrLn(TStr::Fmt("<ptop:Topic rdf:about=\"#TOP_%d\">", ClustId));
        SOut->PutStrLn(TStr::Fmt("  <psys:description>%s</psys:description>", CptNm.CStr()));
        if (FatherId != -1) {
            SOut->PutStrLn(
                TStr::Fmt("  <ptop:subTopicOf rdf:resource=\"#TOP_%d\" />", FatherId));
        }
        if (!KeyWdStr.Empty()) {
            SOut->PutStrLn(TStr::Fmt("  <jsikm:hasOntoGenClassProperties rdf:resource=\"#CLS_PROP_%d\" />", ClustId)); }
        SOut->PutStrLn("</ptop:Topic>");

        if (!KeyWdStr.Empty()) {
            SOut->PutStrLn(TStr::Fmt("<jsikm:OntoGenClassProperties rdf:about=\"#CLS_PROP_%d\">", ClustId));
            SOut->PutStrLn(TStr::Fmt("  <jsikm:hasCentroidKeywords>%s</jsikm:hasCentroidKeywords>", KeyWdStr.CStr()));
            SOut->PutStrLn("</jsikm:OntoGenClassProperties>");
        }
        SOut->PutStrLn("");

        int Docs=Clust->GetDocs();
        for (int DIdN=0; DIdN<Docs; DIdN++){
            int DId=Clust->GetDId(DIdN);
            if (DIdClustIdVH.IsKey(DId)) {
                DIdClustIdVH.GetDat(DId).Add(ClustId);
            } else {
                DIdClustIdVH.AddDat(DId, TIntV::GetV(ClustId));
            }
        }

        PBowDocPart SubPart=GetClust(ClustN)->GetSubPart();
        const int MyId = ClustId; ClustId++;
        if (!SubPart.Empty()){
            SubPart->SaveRdfClusts(SOut, BowDocBs, DIdClustIdVH, MyId, ClustId);
        }
    }
    SOut->Flush();
}

//B:
void TBowDocPart::SaveRdf(const TStr& FNm, const PBowDocBs& BowDocBs) const {
    PSOut SOut=TFOut::New(FNm);
    
    SOut->PutStrLn("<?xml version='1.0' encoding='UTF-8'?>");
    SOut->PutStrLn("");
    SOut->PutStrLn("<!DOCTYPE rdf:RDF [");
    SOut->PutStrLn("    <!ENTITY rdf  'http://www.w3.org/1999/02/22-rdf-syntax-ns#'>");
    SOut->PutStrLn("    <!ENTITY rdfs 'http://www.w3.org/TR/1999/PR-rdf-schema-19990303#'>");
    SOut->PutStrLn("    <!ENTITY owl  'http://www.w3.org/2002/07/owl#'>");
    SOut->PutStrLn("    <!ENTITY ptop 'http://proton.semanticweb.org/2005/04/protont#'>");
    SOut->PutStrLn("    <!ENTITY psys 'http://proton.semanticweb.org/2005/04/protons#'>");
    SOut->PutStrLn("    <!ENTITY pupp  'http://proton.semanticweb.org/2005/04/protonu#'>");
    SOut->PutStrLn("    <!ENTITY protonkm  'http://proton.semanticweb.org/2005/04/protonkm#'>");
    SOut->PutStrLn("    <!ENTITY jsikm  'http://kt.ijs.si/blazf/jsikm#'>");
    SOut->PutStrLn("]>");
    SOut->PutStrLn("");
    SOut->PutStrLn("<rdf:RDF");
    SOut->PutStrLn("    xmlns:rdf=\"&rdf;\"");
    SOut->PutStrLn("    xmlns:rdfs=\"&rdfs;\"");
    SOut->PutStrLn("    xmlns:owl=\"&owl;\"");
    SOut->PutStrLn("    xmlns:protonkm=\"&protonkm;\"");
    SOut->PutStrLn("    xmlns:psys=\"&psys;\"");
    SOut->PutStrLn("    xmlns:pupp=\"&pupp;\"");
    SOut->PutStrLn("    xmlns:jsikm=\"&jsikm;\"");
    SOut->PutStrLn("    xmlns=\"&jsikm;\"");
    SOut->PutStrLn("    xmlns:ptop=\"&ptop;\"");
    SOut->PutStrLn("    xml:base=\"&jsikm;\"");
    SOut->PutStrLn(">");
    SOut->PutStrLn("");
    SOut->PutStrLn("<owl:Ontology rdf:about=\"\">");
    SOut->PutStrLn("  <rdfs:comment>PROTON Topics (from Inspec Thesaurus) ordered by algorithm X</rdfs:comment>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protons\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protont\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protonu\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protonkm\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://kt.ijs.si/blazf/jsikm\"/>");
    SOut->PutStrLn("  <owl:versionInfo>\"0.1\"</owl:versionInfo>");
    SOut->PutStrLn("</owl:Ontology>");
    SOut->PutStrLn("");
    
    TIntIntVH DIdClustIdVH; int ClustId = 0;
    SaveRdfClusts(SOut, BowDocBs, DIdClustIdVH, -1, ClustId);  

    TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV);
    TStr BowFNm = TStr::PutFExt(FNm, "_docs.bow");
    TStr ShortBowFNm = BowFNm.GetFMid() + BowFNm.GetFExt();
    BowDocBs->SaveBin(BowFNm);
    for (int DocN = 0; DocN < AllDIdV.Len(); DocN++) {
        const int DId = AllDIdV[DocN];
        SOut->PutStrLn(TStr::Fmt("<ptop:Document rdf:about=\"#DOC_%d\">", DId));
        if (DIdClustIdVH.IsKey(DId)) {
            const TIntV& DocCptV = DIdClustIdVH.GetDat(DId);
            for (int CptN = 0; CptN < DocCptV.Len(); CptN++) {
                const int FthCptId = DocCptV[CptN];
                SOut->PutStrLn(TStr::Fmt("  <ptop:hasSubject rdf:resource=\"#TOP_%d\" />", FthCptId));
            }
        }
        SOut->PutStrLn(TStr::Fmt("  <jsikm:hasOntoGenInstanceProperties rdf:resource=\"#INST_PROP_%d\" />", DId));
        SOut->PutStrLn("</ptop:Topic>");

        SOut->PutStrLn(TStr::Fmt("<jsikm:hasOntoGenInstanceProperties rdf:about=\"#INST_PROP_%d\">", DId));
        SOut->PutStrLn(TStr::Fmt("  <jsikm:locationOfInstance>%s#%d</jsikm:locationOfInstance>", ShortBowFNm.CStr(), DId));
        SOut->PutStrLn("</jsikm:OntoGenClassProperties>");
        SOut->PutStrLn("");
    }        

    SOut->PutStrLn("</rdf:RDF>");
    SOut->Flush();
}

const TStr TBowDocPart::BowDocPartFExt=".BowPart";

/////////////////////////////////////////////////
// BagOfWords-Clustering
PBowSpV TBowClust::GetConceptSpV(
 const PBowDocWgtBs& BowDocWgtBs, const PBowSim& BowSim, const TIntV& DIdV,
 const double& CutWordWgtSumPrc, const TIntFltPrV& DocIdWgtPrV){
  // prepare concept vector
  PBowSpV ConceptSpV;
  // special cases
  if (DIdV.Empty()){
    ConceptSpV=TBowSpV::New();
  } else
  if ((!BowSim.Empty())&&(BowSim->GetSimType()==bstMtx)){
    int DIds=DIdV.Len(); int DIdGran=1;
    int BestDId=DIdV[0]; double BestAvgSim=0;
    for (int DIdN1=0; DIdN1<DIds; DIdN1+=DIdGran){
      int DId1=DIdV[DIdN1];
      double SumSim=0; int TestDIds=0;
      for (int DIdN2=0; DIdN2<DIds; DIdN2+=DIdGran){
        if (DIdN1==DIdN2){continue;}
        int DId2=DIdV[DIdN2];
        SumSim+=BowSim->GetSim(DId1, DId2); TestDIds++;
      }
      if (TestDIds>0){
        double AvgSim=SumSim/TestDIds;
        if (AvgSim>BestAvgSim){BestDId=DId1;}
      }
    }
    ConceptSpV=BowDocWgtBs->GetSpV(BestDId);
  } else {
    // prepare data
    int Words=BowDocWgtBs->GetWords();
    TFltV WordWgtSumV(Words); double WordWgtSum=0;
    int DIds=DIdV.Len();
    // prepare document weights
    TIntFltH DocIdToWgtH(DIds); 
    for (int DIdN=0; DIdN<DocIdWgtPrV.Len(); DIdN++){
      DocIdToWgtH.AddDat(DocIdWgtPrV[DIdN].Val1, DocIdWgtPrV[DIdN].Val2);
    }
    // collect word weights
    for (int DIdN=0; DIdN<DIds; DIdN++){
      int DId=DIdV[DIdN];
      // get doc-weight
      TFlt DocWgt=1.0;
      if (DocIdWgtPrV.Len()>0){
        DocIdToWgtH.IsKeyGetDat(DId, DocWgt);}
      PBowSpV SpV=BowDocWgtBs->GetSpV(DId);
      int WIds=SpV->GetWIds();
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId; double WordWgt; SpV->GetWIdWgt(WIdN, WId, WordWgt);
        WordWgtSumV[WId]+=DocWgt.Val*WordWgt; WordWgtSum+=DocWgt.Val*WordWgt;
      }
    }
    // extract non-zero word weights
    ConceptSpV=TBowSpV::New(-1, Words/10);
    for (int WId=0; WId<Words; WId++){
      if (double(WordWgtSumV[WId])>0){
        double WordWgtMean=WordWgtSumV[WId]/DIds;
        ConceptSpV->AddWIdWgt(WId, WordWgtMean);
      }
    }
    // cut low-weight words
    if (CutWordWgtSumPrc<1.0){
      ConceptSpV->CutLowWgtWords(CutWordWgtSumPrc);}
    // put unit-norm
    ConceptSpV->PutUnitNorm();
    // truncate vector
    ConceptSpV->Trunc();
  }
  // return concept-vector
  return ConceptSpV;
}

void TBowClust::GetConceptSpVV(const PBowSim& /*BowSim*/,
 const TBowSpVV& SpVV1, const TBowSpVV& SpVV2, TBowSpVV& ConceptSpVV){
  ConceptSpVV.Gen(SpVV1.Len()+SpVV2.Len(), 0);
  ConceptSpVV.AddV(SpVV1);
  ConceptSpVV.AddV(SpVV2);
}

double TBowClust::GetClustQual(
 const PBowDocWgtBs& BowDocWgtBs,
 const TIntV& DIdV, const PBowSpV& ConceptSpV, const PBowSim& BowSim){
  // prepare quality
  double Qual=0;
  // collect similarities relative to the concept vector
  int DIds=DIdV.Len();
  for (int DIdN=0; DIdN<DIds; DIdN++){
    int DId=DIdV[DIdN];
    Qual+=BowSim->GetSim(ConceptSpV, BowDocWgtBs->GetSpV(DId));
  }
  // return quality
  return Qual;
}

void TBowClust::GetPartQual(
 const PBowDocWgtBs& BowDocWgtBs,
 const TVec<TIntV>& DIdVV, const TBowSpVV& ConceptSpVV,
 const PBowSim& BowSim, double& PartQual, TFltV& ClustQualV){
  // prepare data
  int Clusts=DIdVV.Len();
  PartQual=0;
  ClustQualV.Gen(Clusts);
  // collect cluster qualities
  for (int ClustN=0; ClustN<Clusts; ClustN++){
    double ClustQual=
     GetClustQual(BowDocWgtBs, DIdVV[ClustN], ConceptSpVV[ClustN], BowSim);
    PartQual+=ClustQual;
    ClustQualV[ClustN]=ClustQual;
  }
}

PBowDocPart TBowClust::GetKMeansPartForDocWgtBs(
 const PNotify& Notify,
 const PBowDocWgtBs& BowDocWgtBs,
 const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
 const int& Clusts, const int& ClustTrials,
 const double& ConvergEps, const int& MnDocsPerClust,
 const TIntFltPrV& DocIdWgtPrV){
  // prepare documents-ids
  int Docs=BowDocWgtBs->GetDocs();
  int Words=BowDocWgtBs->GetWords();
  TNotify::OnNotify(Notify, ntInfo, "-----------------------");
  TNotify::OnNotify(Notify, ntInfo, TInt::GetStr(Docs, "Docs: %d"));
  TNotify::OnNotify(Notify, ntInfo, TInt::GetStr(Words, "Words: %d"));
  // create initial partition
  PBowDocPart BestPart=TBowDocPart::New();
  // clustering trials
  int ClustTrialN=0; int AddClustTrials=0;
  while (ClustTrialN<ClustTrials+AddClustTrials){
    ClustTrialN++;
    TNotify::OnNotify(Notify, ntInfo,
     TInt::GetStr(ClustTrialN, "Clustering Trial %d"));
    // create concept vectors
    TBowSpVV ConceptSpVV(Clusts);
    // select best set of random documents for concept vectors
    double BestStartConceptMeanSim=0;
    TBowSpVV BestStartConceptSpVV(Clusts);
    for (int CVTrialN=0; CVTrialN<3; CVTrialN++){
      // select random documents for initial concept vectors
      for (int ClustN=0; ClustN<Clusts; ClustN++){
        int DIdN=Rnd.GetUniDevInt(Docs);
        int DId=BowDocWgtBs->GetDId(DIdN);
        ConceptSpVV[ClustN]=BowDocWgtBs->GetSpV(DId);
      }
      // evaluate selected concept vectors
      PMom SimMom=TMom::New();
      for (int ClustN1=0; ClustN1<Clusts; ClustN1++){
        for (int ClustN2=ClustN1+1; ClustN2<Clusts; ClustN2++){
          double Sim=BowSim->GetSim(ConceptSpVV[ClustN1], ConceptSpVV[ClustN2]);
          SimMom->Add(Sim);
        }
      }
      SimMom->Def();
      // save first or better set of concept vectors
      double ConceptMeanSim=SimMom->GetMean();
      if ((CVTrialN==0)||(ConceptMeanSim<BestStartConceptMeanSim)){
        BestStartConceptMeanSim=ConceptMeanSim;
        BestStartConceptSpVV=ConceptSpVV;
      }
    }
    // assign best set of concept vectors
    ConceptSpVV=BestStartConceptSpVV;

    // clustering (k-means loop)
    double PrevPartQual=0; // previous partition quality;
    double PartQual=0; // current partition quality;
    TFltV ClustQualV; // cluster quality vector
    TVec<TIntV> DIdVV; // vector of cluster doc-id vectors
    int ClustIters=0; // number of clustering iterations
    forever {
      ClustIters++;
      bool EmptyClustP;
      do {
        // for each document find closest concept vector
        DIdVV.Gen(Clusts);
        for (int DIdN=0; DIdN<Docs; DIdN++){
          int DId=BowDocWgtBs->GetDId(DIdN);
          PBowSpV DocSpV=BowDocWgtBs->GetSpV(DId);
          TFltIntKdV SimClustNKdV(Clusts);
          for (int ClustN=0; ClustN<Clusts; ClustN++){
            double Sim=BowSim->GetSim(ConceptSpVV[ClustN], DocSpV);
            SimClustNKdV[ClustN]=TFltIntKd(Sim, ClustN);
          }
          // select best cluster
          SimClustNKdV.Sort(false); // sort similarity/cluster-id pairs
          int BestClustN=SimClustNKdV[0].Dat; // get best cluster-id
          // if there are more best clusters
          if ((SimClustNKdV.Len()>0)&&(SimClustNKdV[0].Key==SimClustNKdV[1].Key)){
            int BestClusts=1;
            for (int ClustN=1; ClustN<Clusts; ClustN++){
              if (SimClustNKdV[0].Key==SimClustNKdV[ClustN].Key){
                BestClusts++;
              } else {break;}
            }
            // assign random cluster among best ones
            BestClustN=Rnd.GetUniDevInt(BestClusts);
          }
          // assign current doc-id to the best cluster
          DIdVV[BestClustN].Add(DId);
        }
        // compute new concept vectors
        EmptyClustP=false;
        for (int ClustN=0; ClustN<Clusts; ClustN++){
          if ((MnDocsPerClust!=-1)&&(DIdVV[ClustN].Len()<MnDocsPerClust)){
            // cluster has less documents than allowed
            // assign random document to concept vector
            int DId=BowDocWgtBs->GetDId(Rnd.GetUniDevInt(Docs));
            ConceptSpVV[ClustN]=BowDocWgtBs->GetSpV(DId);
            PrevPartQual=0; PartQual=0; ClustIters=1;
            EmptyClustP=true; break;
          } else {
            // calculate new concept vector
            ConceptSpVV[ClustN]=
             TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, DIdVV[ClustN], 1, DocIdWgtPrV);
          }
        }
      } while (EmptyClustP);
      // stopping criterion
      PrevPartQual=PartQual;
      GetPartQual(BowDocWgtBs, DIdVV, ConceptSpVV, BowSim, PartQual, ClustQualV);
      TStr MsgStr=
       TInt::GetStr(ClustIters, "  Iteration %d:")+
       TFlt::GetStr(PartQual, " [Quality: %g]");
      TNotify::OnNotify(Notify, ntInfo, MsgStr);
      //if ((ClustIters<=2)&&(PrevPartQual==PartQual)){
      //  AddClustTrials++; break;}
      if (fabs(PartQual-PrevPartQual)<ConvergEps){break;}
    }
    // save first or better solution
    if ((ClustTrialN==1)||(PartQual>BestPart->GetQual())){
      BestPart=TBowDocPart::New();
      BestPart->PutQual(PartQual);
      for (int ClustN=0; ClustN<Clusts; ClustN++){
        // create cluster
        PBowDocPartClust Clust=TBowDocPartClust::New(BowDocBs,
         "", ClustQualV[ClustN], DIdVV[ClustN], ConceptSpVV[ClustN], NULL);
        BestPart->AddClust(Clust);
        // add document to centroid similarities
        {TIntV& DIdV=DIdVV[ClustN]; int Docs=DIdV.Len(); TFltV DCSimV(Docs, 0);
        for (int DIdN=0; DIdN<Docs; DIdN++){
          PBowSpV DocSpV=BowDocWgtBs->GetSpV(DIdV[DIdN]);
          double Sim=BowSim->GetSim(ConceptSpVV[ClustN], DocSpV);
          DCSimV.Add(Sim);
        }
        Clust->AddDCSimV(DCSimV);}
        // add document to document similarities
        {TIntV& DIdV=DIdVV[ClustN]; int Docs=DIdV.Len();
        if (Docs<=200){
          TFltVV DDSimVV(Docs, Docs);
          for (int DIdN1=0; DIdN1<Docs; DIdN1++){
            PBowSpV DocSpV1=BowDocWgtBs->GetSpV(DIdV[DIdN1]);
            for (int DIdN2=0; DIdN2<Docs; DIdN2++){
              PBowSpV DocSpV2=BowDocWgtBs->GetSpV(DIdV[DIdN2]);
              double Sim=BowSim->GetSim(DocSpV1, DocSpV2);
              DDSimVV.At(DIdN1, DIdN2)=Sim;
            }
          }
          Clust->AddDDSimVV(DDSimVV);
        }}
      }
      // calculate inter-cluster similarity
      TFltVV ClustSimVV(Clusts, Clusts);
      for (int ClustN1=0; ClustN1<Clusts; ClustN1++){
        ClustSimVV.At(ClustN1, ClustN1)=
         BowSim->GetSim(ConceptSpVV[ClustN1], ConceptSpVV[ClustN1]);
        for (int ClustN2=ClustN1+1; ClustN2<Clusts; ClustN2++){
          double Sim=BowSim->GetSim(ConceptSpVV[ClustN1], ConceptSpVV[ClustN2]);
          ClustSimVV.At(ClustN1, ClustN2)=Sim;
          ClustSimVV.At(ClustN2, ClustN1)=Sim;
        }
      }
      BestPart->PutClustSim(ClustSimVV);
    }
  }
  // return result
  TNotify::OnNotify(Notify, ntInfo,
   TFlt::GetStr(BestPart->GetQual(), "Final Quality: %g"));
  return BestPart;
}

PBowDocPart TBowClust::GetHPartForDocWgtBs(
 const PNotify& Notify,
 const PBowDocWgtBs& BowDocWgtBs,
 const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& /*Rnd*/){
  // prepare documents-ids
  int Docs=BowDocWgtBs->GetDocs();
  // prepare similarity matrix
  TFltVV SimVV(Docs, Docs); SimVV.PutAll(-1);
  TVec<TBowSpVV> ClustSpVVV(Docs, 0);
  TBowDocPartV PartV(Docs, 0);
  for (int DIdN1=0; DIdN1<Docs; DIdN1++){
    int DId1=BowDocWgtBs->GetDId(DIdN1);
    // add single doc as cluster
    TBowSpVV ClustSpVV(1); ClustSpVV[0]=BowDocWgtBs->GetSpV(DId1);
    ClustSpVVV.Add(ClustSpVV);
    // create one cluster partition
    TIntV ClustDIdV(1); ClustDIdV[0]=DId1;
    PBowDocPartClust Clust=
     TBowDocPartClust::New(BowDocBs, "", 0, ClustDIdV, NULL, NULL);
    PBowDocPart Part=TBowDocPart::New();
    Part->AddClust(Clust);
    PartV.Add(Part);
    // calculate similarities
    for (int DIdN2=DIdN1+1; DIdN2<Docs; DIdN2++){
      int DId2=BowDocWgtBs->GetDId(DIdN2);
      double Sim=
       BowSim->GetSim(BowDocWgtBs->GetSpV(DId1), BowDocWgtBs->GetSpV(DId2));
      SimVV.At(DIdN1, DIdN2)=Sim;
      SimVV.At(DIdN2, DIdN1)=Sim;
    }
  }
  // clustering
  int Clusts=Docs;
  while (Clusts>1){
    // find the most similar pair of documents
    int DIdN1; int DIdN2; SimVV.GetMxValXY(DIdN1, DIdN2);
    SimVV.PutX(DIdN1, -1); SimVV.PutY(DIdN1, -1);
    SimVV.PutX(DIdN2, -1); SimVV.PutY(DIdN2, -1);
    // get new concept vector
    TBowSpVV ConceptSpVV;
    GetConceptSpVV(BowSim, ClustSpVVV[DIdN1], ClustSpVVV[DIdN2], ConceptSpVV);
    ClustSpVVV[DIdN1]=ConceptSpVV;
    ClustSpVVV[DIdN2].Clr();
    // update similarity matrix
    for (int DIdN=0; DIdN<Docs; DIdN++){
      if ((ClustSpVVV[DIdN].Len()>0)&&(DIdN!=DIdN1)){
        double Sim=BowSim->GetSim(ConceptSpVV, ClustSpVVV[DIdN]);
        SimVV.At(DIdN1, DIdN)=Sim;
        SimVV.At(DIdN, DIdN1)=Sim;
      }
    }
    // update partition
    PBowDocPart Part1=PartV[DIdN1];
    PBowDocPart Part2=PartV[DIdN2];
    TIntV PartDIdV1; Part1->GetDIdV(PartDIdV1);
    TIntV PartDIdV2; Part2->GetDIdV(PartDIdV2);
    PBowDocPartClust Clust1=TBowDocPartClust::New(BowDocBs, "", 0, PartDIdV1, NULL, Part1);
    PBowDocPartClust Clust2=TBowDocPartClust::New(BowDocBs, "", 0, PartDIdV2, NULL, Part2);
    PBowDocPart Part=TBowDocPart::New();
    Part->AddClust(Clust1);
    Part->AddClust(Clust2);
    PartV[DIdN1]=Part;
    PartV[DIdN2]=NULL;
    // decrement number of clusters
    Clusts--;
    TNotify::OnStatus(Notify, TInt::GetStr(Clusts, "Clusts: %4d\r"));
  }
  // return result
  return PartV[0];
}

PBowDocPart TBowClust::GetKMeansPart(
 const PNotify& Notify,
 const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
 const int& Clusts, const int& ClustTrials,
 const double& ConvergEps, const int& MnDocsPerClust,
 const TBowWordWgtType& WordWgtType, const double& CutWordWgtSumPrc,
 const int& MnWordFq, const TIntV& _DIdV){
  // create document-id vector (DIdV)
  TIntV DIdV;
  if (_DIdV.Empty()){BowDocBs->GetAllDIdV(DIdV);} else {DIdV=_DIdV;}
  // create document-weights-base
  PBowDocWgtBs BowDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, CutWordWgtSumPrc, MnWordFq, DIdV);
  // create partitions
  PBowDocPart BowDocPart=TBowClust::GetKMeansPartForDocWgtBs(
   Notify, BowDocWgtBs, BowDocBs, BowSim,
   Rnd, Clusts, ClustTrials, ConvergEps, MnDocsPerClust);
  return BowDocPart;
}

PBowDocPart TBowClust::GetHKMeansPart(
 const PNotify& Notify,
 const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
 const int& MxDocsPerLeafClust, const int& ClustTrials,
 const double& ConvergEps, const int& MnDocsPerClust,
 const TBowWordWgtType& WordWgtType, const double& CutWordWgtSumPrc,
 const int& MnWordFq, const TIntV& _DIdV,
 const bool& PropBowDocWgtBsP, const PBowDocWgtBs& _BowDocWgtBs){
  // create document-id vector (DIdV)
  TIntV DIdV;
  if (_DIdV.Empty()){BowDocBs->GetAllDIdV(DIdV);} else {DIdV=_DIdV;}
  // create document-weights-base
  PBowDocWgtBs BowDocWgtBs;
  if ((PropBowDocWgtBsP)||(_BowDocWgtBs.Empty())){
   BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, WordWgtType, CutWordWgtSumPrc, MnWordFq, DIdV);
  } else {
    BowDocWgtBs=_BowDocWgtBs;
  }
  int BowDocWgtBsDocs=BowDocWgtBs->GetDocs();
  // create top document-partition
  PBowDocPart DocPart=TBowClust::GetKMeansPartForDocWgtBs(
   Notify, BowDocWgtBs, BowDocBs, BowSim,
   Rnd, 2, ClustTrials, ConvergEps, MnDocsPerClust);
  if (!PropBowDocWgtBsP){
    BowDocWgtBs=NULL;}
  // create sub document-partitions
  for (int ClustN=0; ClustN<DocPart->GetClusts(); ClustN++){
    PBowDocPartClust Clust=DocPart->GetClust(ClustN);
    int ClustDocs=Clust->GetDocs();
    if ((ClustDocs>MxDocsPerLeafClust)||(ClustDocs==BowDocWgtBsDocs)){
      TIntV ClustDIdV; Clust->GetDIdV(ClustDIdV);
      PBowDocPart SubDocPart=GetHKMeansPart(Notify,
       BowDocBs, BowSim, Rnd, MxDocsPerLeafClust, ClustTrials, ConvergEps,
       MnDocsPerClust, WordWgtType, CutWordWgtSumPrc, MnWordFq, ClustDIdV,
       PropBowDocWgtBsP, BowDocWgtBs);
      Clust->PutSubPart(SubDocPart);
    }
  }
  return DocPart;
}

PBowDocPart TBowClust::GetHPart(
 const PNotify& Notify,
 const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
 const TBowWordWgtType& WordWgtType, const double& CutWordWgtSumPrc,
 const int& MnWordFq, const TIntV& DIdV){
  // create document-weights-base
  PBowDocWgtBs BowDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, CutWordWgtSumPrc, MnWordFq, DIdV);
  // create sub document-partitions
  PBowDocPart BowDocPart=
   GetHPartForDocWgtBs(Notify, BowDocWgtBs, BowDocBs, BowSim, Rnd);
  return BowDocPart;
}

double TBowClust::GetNNbrCfAcc(
 const PNotify& Notify,
 const PBowDocBs& BowDocBs, const PBowSim& BowSim,
 const TBowWordWgtType& WordWgtType, const int& KNbrs,
 const TIntV& TrainDIdV, const TIntV& TestDIdV){
  // create document-weights-bases
  PBowDocWgtBs TrainDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0, TrainDIdV);
  PBowDocWgtBs TestDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0, TestDIdV, TrainDIdV);
  int TrainDocs=TrainDocWgtBs->GetDocs();
  int TestDocs=TestDocWgtBs->GetDocs();
  // classify docs
  int AllCfs=0; int CorrectCfs=0;
  for (int TestDIdN=0; TestDIdN<TestDocs; TestDIdN++){
    // traverse test documents
    int TestDId=TestDIdV[TestDIdN];
    PBowSpV TestDocSpV=TestDocWgtBs->GetSpV(TestDId);
    // similarity/doc-id vector
    TFltIntKdV SimDIdKdV(TrainDocs, 0);
    for (int TrainDIdN=0; TrainDIdN<TrainDocs; TrainDIdN++){
      // traverse train documents
      int TrainDId=TrainDIdV[TrainDIdN];
      PBowSpV TrainDocSpV=TrainDocWgtBs->GetSpV(TrainDId);
      // get similarity
      double Sim=BowSim->GetSim(TestDocSpV, TrainDocSpV);
      SimDIdKdV.Add(TFltIntKd(Sim, TrainDId));
    }
    // sort according to similarity
    SimDIdKdV.Sort(false);
    // calculate category weights for k-nbrs
    int Cats=BowDocBs->GetCats();
    TFltV CatWgtV(Cats);
    double SumCatWgt=0;
    int NrKNbrs=TInt::GetMn(KNbrs, SimDIdKdV.Len());
    for (int KNbrN=0; KNbrN<NrKNbrs; KNbrN++){
      int DId=SimDIdKdV[KNbrN].Dat;
      // traverse document categories
      int DocCats=BowDocBs->GetDocCIds(DId);
      for (int DocCIdN=0; DocCIdN<DocCats; DocCIdN++){
        int CId=BowDocBs->GetDocCId(DId, DocCIdN);
        double CatWgt=1/double(DocCats);
        CatWgtV[CId]+=CatWgt; SumCatWgt+=CatWgt;
      }
    }
    // normalize category weights to sum 1
    if (SumCatWgt>0){
      for (int CId=0; CId<Cats; CId++){
        CatWgtV[CId]=CatWgtV[CId]/SumCatWgt;
      }
    }
    // get best category
    int BestCId=CatWgtV.GetMxValN();
    // update accuracy info
    //if (IsCatInDoc(TestDId, BestCId)){CorrectCfs++;}
    if (BowDocBs->IsCatInDoc(TestDId, BestCId)){
      printf("+"); CorrectCfs++;} else {printf("-");}
    AllCfs++;
  }
  // calculate accuracy
  double CfAcc=double(CorrectCfs)/AllCfs;
  // output accuracy
  TStr MsgStr=TStr("NNbr:")+TInt::GetStr(CorrectCfs)+"/"+
   TInt::GetStr(AllCfs)+"="+TFlt::GetStr(CfAcc, "%.3f");
  TNotify::OnNotify(Notify, ntInfo, MsgStr);
  // return results
  return CfAcc;
}

double TBowClust::GetClustCfAcc(
 const PNotify& Notify,
 const PBowDocBs& BowDocBs, const PBowDocPart& Part, const PBowSim& BowSim,
 const TBowWordWgtType& WordWgtType,
 const TIntV& TrainDIdV, const TIntV& TestDIdV){
  // create document-weights-bases
  PBowDocWgtBs TrainDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0, TrainDIdV);
  PBowDocWgtBs TestDocWgtBs=
   TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0, TestDIdV, TrainDIdV);
  int TestDocs=TestDocWgtBs->GetDocs();
  // classify docs
  int AllCfs=0; int CorrectCfs=0;
  for (int DIdN=0; DIdN<TestDocs; DIdN++){
    int DId=TestDIdV[DIdN];
    PBowSpV DocSpV=TestDocWgtBs->GetSpV(DId);
    int BestCId=Part->GetBestCId(DocSpV, BowSim);
    // update accuracy info
    //if (IsCatInDoc(DId, BestCId)){CorrectCfs++;}
    if (BowDocBs->IsCatInDoc(DId, BestCId)){printf("+"); CorrectCfs++;}
    else {printf("-");}
    AllCfs++;
  }
  // calculate accuracy
  double CfAcc=double(CorrectCfs)/AllCfs;
  // output accuracy
  TStr MsgStr=TStr("Clust:")+TInt::GetStr(CorrectCfs)+"/"+
   TInt::GetStr(AllCfs)+"="+TFlt::GetStr(CfAcc, "%.3f");
  TNotify::OnNotify(Notify, ntInfo, MsgStr);
  // return results
  return CfAcc;
}

