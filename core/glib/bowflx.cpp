/////////////////////////////////////////////////
// BagOfWords-Files
void TBowFl::LoadHtmlTxt(
 PBowDocBs BowDocBs, const TStr& FPath, TIntV& NewDIdV,
 const bool& RecurseDirP, const int& MxDocs,
 const bool& SaveDocP, const PNotify& Notify) {
  // prepare file-directory traversal
  TStr LcNrFPath=TStr::GetNrFPath(FPath).GetLc();
  Notify->OnStatus("Creating Bow from file-path " + FPath + " ...");
  TFFile FFile(FPath, "", RecurseDirP);
  // traverse files
  TStr FNm; int Docs=0; NewDIdV.Clr();
  while (FFile.Next(FNm)){
    Docs++; if ((MxDocs!=-1)&&(Docs>MxDocs)){break;}
    Notify->OnStatus(TStr::Fmt("%d\r", Docs));
    // prepare document-name
    if (TFile::Exists(FNm)) { //B:
        TStr DocNm=FNm.GetLc();
        if (DocNm.IsPrefix(LcNrFPath)){
          DocNm=DocNm.GetSubStr(LcNrFPath.Len(), DocNm.Len()-1);}
        // categories
        TStrV CatNmV; TStr CatNm;
        if (DocNm.IsChIn('/')){
          TStr Str; DocNm.SplitOnCh(CatNm, '/', Str);
        } else if (DocNm.IsChIn('\\')){
          TStr Str; DocNm.SplitOnCh(CatNm, '\\', Str);
        }
        if (!CatNm.Empty()){
          CatNmV.Add(CatNm);}
        // load document-content
        TStr DocStr=TStr::LoadTxt(FNm);
        // add document to bow
        NewDIdV.Add(BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocStr, SaveDocP));
    }
  }
  Notify->OnStatus(TStr::Fmt("%d", Docs));
  // return results
  Notify->OnStatus("Done.");
  BowDocBs->AssertOk();
}


PBowDocBs TBowFl::LoadHtmlTxt(
 const TStr& FPath, const bool& RecurseDirP, const int& MxDocs,
 const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
 const int& MxNGramLen, const int& MnNGramFq,
 const bool& SaveDocP, const PNotify& Notify){
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(SwSetTypeNm);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(StemmerTypeNm);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    NGramBs=TNGramBs::GetNGramBsFromHtmlFPathV(
     FPath, RecurseDirP, MxDocs,
     MxNGramLen, MnNGramFq, SwSet, Stemmer, Notify);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  TIntV NewDIdV; LoadHtmlTxt(BowDocBs, FPath, NewDIdV,
      RecurseDirP, MxDocs, SaveDocP, Notify);
  return BowDocBs;
}

PBowDocBs TBowFl::LoadFromSimMtx(const PBowSimMtx& BowSimMtx){
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New();
  // create documents
  int Docs=BowSimMtx->GetDocs();
  BowDocBs->DocSpVV.Gen(Docs, 0);
  for (int MtxDIdN=0; MtxDIdN<Docs; MtxDIdN++){
    int MtxDId=BowSimMtx->GetMtxDId(MtxDIdN);
    int DId=BowDocBs->DocNmToDescStrH.AddKey(TInt::GetStr(MtxDId));
    IAssert(MtxDIdN==DId);
    //printf("[%d:%d]", DId, MtxDId);
    BowDocBs->DocSpVV.Add(TBowSpV::New(DId));
    IAssert(DId==BowDocBs->DocSpVV.Len()-1);
  }
  // return results
  BowDocBs->AssertOk();
  return BowDocBs;
}

PBowDocBs TBowFl::LoadTabTxt(const TStr& FNm, const int& /*MxDocs*/){
  // prepare document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  // load domain
  PDm Dm=TDm::LoadFile(FNm);
  // define attributes as words
  for (int AttrN=0; AttrN<Dm->GetAttrs(); AttrN++){
    // get & check variable
    PTbVar Var=Dm->GetAttr(AttrN);
    TStr VarNm=Var->GetNm();
    PTbVarType VarType=Var->GetVarType();
    EAssertRA(VarType->IsFlt(), "Attribute must be continuous", VarNm);
    // add variable as word
    BowDocBs->WordStrToDescH.AddKey(VarNm);
  }

  // define examples as documents
  for (int ExN=0; ExN<Dm->GetExs(); ExN++){
    // create new document
    TStr DocNm=TInt::GetStr(ExN);
    int DId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
    BowDocBs->DocSpVV.Add(TBowSpV::New(DId)); IAssert(DId==BowDocBs->DocSpVV.Len()-1);
    BowDocBs->DocCIdVV.Add(); IAssert(DId==BowDocBs->DocCIdVV.Len()-1);
    // convert class values to cat-ids
    TIntV& DocCIdV=BowDocBs->DocCIdVV[DId];
    DocCIdV.Gen(Dm->GetClasses(), 0);
    for (int ClassN=0; ClassN<Dm->GetClasses(); ClassN++){
      TStr ValStr=Dm->GetClassValStr(ExN, ClassN);
      int CId=BowDocBs->CatNmToFqH.AddKey(ValStr);
      BowDocBs->CatNmToFqH[CId]++; DocCIdV.Add(CId);
    }
    DocCIdV.Sort();
    // convert attribute values to word-ids/weights
    PBowSpV DocSpV=BowDocBs->DocSpVV[DId];
    DocSpV->GenMx(Dm->GetAttrs());
    for (int AttrN=0; AttrN<Dm->GetAttrs(); AttrN++){
      // get attribute-name as word-id
      TStr WordStr=Dm->GetAttr(AttrN)->GetNm();
      int WId=BowDocBs->WordStrToDescH.GetKeyId(WordStr);
      // get attribute-value as word-frequency
      TTbVal Val=Dm->GetAttrVal(ExN, AttrN);
      double Wgt=Val.GetFlt();
      // update value limits
      TBowWordDesc& WordDesc=BowDocBs->WordStrToDescH[WId];
      WordDesc.MnVal=TFlt::GetMn(WordDesc.MnVal, Wgt);
      WordDesc.MxVal=TFlt::GetMx(WordDesc.MxVal, Wgt);
      // add (word-id, frequency) pair
      DocSpV->AddWIdWgt(WId, Wgt);
    }
    DocSpV->Sort();
  }
  // return results
  BowDocBs->AssertOk();
  return BowDocBs;
}

PBowDocBs TBowFl::LoadTsactTxt(const TStr& FNm, const int& MxDocs){
  // prepare document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  // open file
  PSIn SIn=TFIn::New(FNm);
  printf("Loading %s ...\n", FNm.CStr());
  if (!SIn->Eof()){
    // current document
    int CurDId=-1;
    TIntH CurDocWIdToFqH(100);
    // read first character
    char Ch=SIn->GetCh();
    // skip to the first digit or eof
    while ((!SIn->Eof())&&(!(('0'<=Ch)&&(Ch<='9')))){
      Ch=SIn->GetCh();}
    while (!SIn->Eof()){
      // notify
      if (BowDocBs->DocSpVV.Len()%1000==0){
        printf("%d transactions read\r", BowDocBs->DocSpVV.Len());}
      // read transaction number
      IAssert(('0'<=Ch)&&(Ch<='9'));
      int TsactN=Ch-'0'; Ch=SIn->GetCh();
      while (('0'<=Ch)&&(Ch<='9')){
        TsactN=TsactN*10+Ch-'0'; Ch=SIn->GetCh();}
      // skip space
      while (!(('0'<=Ch)&&(Ch<='9'))){
        IAssert((Ch==' ')||(Ch=='\t')); Ch=SIn->GetCh();}
      // read item number
      IAssert(('0'<=Ch)&&(Ch<='9'));
      int ItemN=Ch-'0'; Ch=SIn->GetCh();
      while (('0'<=Ch)&&(Ch<='9')){
        ItemN=ItemN*10+Ch-'0'; Ch=SIn->GetCh();}
      // skip to the next digit or eof
      while ((!SIn->Eof())&&(!(('0'<=Ch)&&(Ch<='9')))){
        Ch=SIn->GetCh();}
      // get document-id from transaction-id
      TStr DocNm=TInt::GetStr(TsactN);
      int DId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
      // get word-id from item-id
      TStr WordStr=TInt::GetStr(ItemN);
      int WId=BowDocBs->WordStrToDescH.AddKey(WordStr);
      BowDocBs->WordStrToDescH[WId].Fq++;
      // add word to document
      if (CurDId!=DId){
        if (CurDId!=-1){
          if ((MxDocs!=-1)&&(BowDocBs->GetDocs()>=MxDocs-1)){break;}
          PBowSpV NewSpV=TBowSpV::New(CurDId, CurDocWIdToFqH.Len());
          int NewDId=BowDocBs->DocSpVV.Add(NewSpV);
          IAssert(NewDId==CurDId);
          for (int DocWIdN=0; DocWIdN<CurDocWIdToFqH.Len(); DocWIdN++){
            int WId=CurDocWIdToFqH.GetKey(DocWIdN);
            int WordFq=CurDocWIdToFqH[DocWIdN];
            NewSpV->AddWIdWgt(WId, WordFq);
          }
          NewSpV->Sort();
        }
        CurDId=DId; CurDocWIdToFqH.Clr(false);
      }
      CurDocWIdToFqH.AddDat(WId)++;
    }
    // save last document
    if (CurDId!=-1){
      PBowSpV NewSpV=TBowSpV::New(CurDId, CurDocWIdToFqH.Len());
      BowDocBs->DocSpVV.Add(NewSpV);
      for (int DocWIdN=0; DocWIdN<CurDocWIdToFqH.Len(); DocWIdN++){
        int WId=CurDocWIdToFqH.GetKey(DocWIdN);
        int WordFq=CurDocWIdToFqH[DocWIdN];
        NewSpV->AddWIdWgt(WId, WordFq);
      }
      NewSpV->Sort();
    }
    printf("%d transactions read\n", BowDocBs->DocSpVV.Len());
  }
  printf("... Done.\n");
  // return results
  BowDocBs->AssertOk();
  return BowDocBs;
}

PBowDocBs TBowFl::LoadSparseTxt(
 const TStr& DocDefFNm, const TStr& WordDefFNm,
 const TStr& TrainDataFNm,
 const int& MxDocs){
  // prepare document set
  PBowDocBs BowDocBs=TBowDocBs::New();

  // document definition
  if (!DocDefFNm.Empty()){
    PSIn SIn=TFIn::New(DocDefFNm);
    TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloExcept);
    Lx.GetSym(syInt, syEof);
    while (Lx.Sym==syInt){
      int DId=Lx.Int;
      Lx.GetSym(syColon);
      Lx.GetSym(syQStr); TStr DocNm=Lx.Str;
      Lx.GetSym(syEoln);
      Lx.GetSym(syInt, syEof);
      int NewDId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
      EAssertR(DId==NewDId, "Document-Ids don't match.");
    }
  }
  // word definition
  if (!WordDefFNm.Empty()){
    PSIn SIn=TFIn::New(WordDefFNm);
    TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloExcept);
    Lx.GetSym(syInt, syEof);
    while (Lx.Sym==syInt){
      int WId=Lx.Int;
      Lx.GetSym(syColon);
      Lx.GetSym(syQStr); TStr WordStr=Lx.Str;
      Lx.GetSym(syEoln);
      Lx.GetSym(syInt, syEof);
      int NewWId=BowDocBs->WordStrToDescH.AddKey(WordStr);
      BowDocBs->WordStrToDescH[NewWId].Fq=1;
      EAssertR(WId==NewWId, "Word-Ids don't match.");
    }
  }
  // train data
  if (!TrainDataFNm.Empty()){
    PSIn SIn=TFIn::New(TrainDataFNm);
    TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloExcept);
    Lx.GetSym(syInt, syEof);
    while (Lx.Sym==syInt){
      if (BowDocBs->GetDocs()>=MxDocs){break;}
      // document-id
      int DId=Lx.Int;
      // words (attributes)
      PBowSpV NewSpV=TBowSpV::New(DId);
      int NewDId=BowDocBs->DocSpVV.Add(NewSpV);
      EAssertR(DId==NewDId, "Document-Ids don't match.");
      Lx.GetSym(syInt, syEoln);
      while (Lx.Sym==syInt){
        int WId=Lx.Int;
        Lx.GetSym(syColon, syInt, syEoln);
        double WordFq=1;
        if (Lx.Sym==syColon){
          WordFq=Lx.GetFlt();
          Lx.GetSym(syInt, syEoln);
        }
        NewSpV->AddWIdWgt(WId, WordFq);
      }
      NewSpV->Trunc();
      Lx.GetSym(syInt, syEof);
    }
  }
  // check correctness
  BowDocBs->AssertOk();
  // return result
  return BowDocBs;
}

PBowDocBs TBowFl::LoadSvmLightTxt(
 const TStr& DocDefFNm, const TStr& WordDefFNm,
 const TStr& TrainDataFNm, const TStr& TestDataFNm,
 const int& MxDocs){
  // prepare document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  int MOneCId=BowDocBs->CatNmToFqH.AddKey("-1");
  int POneCId=BowDocBs->CatNmToFqH.AddKey("+1");

  // document definition
  bool DocDefP=false;
  if (!DocDefFNm.Empty()&&(TFile::Exists(DocDefFNm))){
    // (DId "DoxNm"<eoln>)*
    PSIn SIn=TFIn::New(DocDefFNm);
    TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloExcept);
    Lx.GetSym(syInt, syEof);
    while (Lx.Sym==syInt){
      int DId=Lx.Int;
      Lx.GetSym(syColon);
      Lx.GetSym(syQStr); TStr DocNm=Lx.Str;
      Lx.GetSym(syEoln);
      Lx.GetSym(syInt, syEof);
      int NewDId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
      EAssertR(DId==NewDId, "Document-Ids don't match.");
    }
    DocDefP=true;
  }
  // word definition
  if (!WordDefFNm.Empty()&&(TFile::Exists(WordDefFNm))){
    BowDocBs->WordStrToDescH.AddDat("Undef").Fq=0; // ... to have WId==0
    PSIn SIn=TFIn::New(WordDefFNm);
    TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloExcept);
    Lx.GetSym(syQStr, syEof);
    while (Lx.Sym==syQStr){
      TStr WordStr=Lx.Str;
      Lx.GetSym(syInt); int WId=Lx.Int;
      Lx.GetSym(syInt); int WordFq=Lx.Int;
      Lx.GetSym(syEoln);
      Lx.GetSym(syQStr, syEof);
      int NewWId=BowDocBs->WordStrToDescH.AddKey(WordStr);
      EAssertR(WId==NewWId, "Word-Ids don't match.");
      BowDocBs->WordStrToDescH[WId].Fq=WordFq;
    }
  }
  // train & test data
  int MxWId=-1; TIntIntH WIdToFqH;
  // train data
  if (!TrainDataFNm.Empty()){
    PSIn SIn=TFIn::New(TrainDataFNm);
    TILx Lx(SIn, TFSet()|iloCmtAlw|iloRetEoln|iloSigNum|iloExcept);
    // skip comment lines
    while (Lx.GetSym(syInt, syEoln, syEof)==syEoln){}
    // parse data lines
    while (Lx.Sym==syInt){
      // document
      TStr DocNm=TInt::GetStr(BowDocBs->GetDocs());
      int DId;
      if (DocDefP){
        DId=BowDocBs->DocNmToDescStrH.GetKeyId(DocNm);
      } else {
        DId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
      }
      BowDocBs->TrainDIdV.Add(DId);
      // category (class value)
      int CId=(Lx.Int==-1) ? MOneCId : POneCId;
      BowDocBs->DocCIdVV.Add(); IAssert(DId==BowDocBs->DocCIdVV.Len()-1);
      BowDocBs->DocCIdVV.Last().Gen(1, 0);
      BowDocBs->DocCIdVV.Last().Add(CId);
      // words (attributes)
      PBowSpV SpV=TBowSpV::New(DId);
      BowDocBs->DocSpVV.Add(SpV); IAssert(DId==BowDocBs->DocSpVV.Len()-1);
      Lx.GetSym(syInt, syEoln);
      while (Lx.Sym==syInt){
        int WId=Lx.Int;
        Lx.GetSym(syColon);
        Lx.GetSym(syFlt); double WordFq=Lx.Flt;
        Lx.GetSym(syInt, syEoln);
        SpV->AddWIdWgt(WId, WordFq);
        if (MxWId==-1){MxWId=WId;} else {MxWId=TInt::GetMx(MxWId, WId);}
        WIdToFqH.AddDat(WId)++;
      }
      if (!Lx.CmtStr.Empty()){
        // change document name to 'N' if comment 'docDesc=N'
        TStr CmtStr=Lx.CmtStr;
        static TStr DocNmPrefixStr="docDesc=";
        if (CmtStr.IsPrefix(DocNmPrefixStr)){
          TStr NewDocNm=
           TStr("D")+CmtStr.GetSubStr(DocNmPrefixStr.Len(), CmtStr.Len()-1);
          BowDocBs->DocNmToDescStrH.DelKey(DocNm);
          int NewDId=BowDocBs->DocNmToDescStrH.AddKey(NewDocNm);
          IAssert(DId==NewDId);
        }
      }
      SpV->Trunc();
      while (Lx.GetSym(syInt, syEoln, syEof)==syEoln){}
    }
  }
  // test data
  if (!TestDataFNm.Empty()){
    PSIn SIn=TFIn::New(TestDataFNm);
    TILx Lx(SIn, TFSet()|iloCmtAlw|iloRetEoln|iloSigNum|iloExcept);
    while (Lx.GetSym(syInt, syEoln, syEof)==syEoln){}
    while (Lx.Sym==syInt){
      // document
      TStr DocNm=TInt::GetStr(BowDocBs->GetDocs());
      int DId;
      if (DocDefP){
        DId=BowDocBs->DocNmToDescStrH.GetKeyId(DocNm);
      } else {
        DId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
      }
      BowDocBs->TestDIdV.Add(DId);
      // category (class value)
      int CId=(Lx.Int==-1) ? MOneCId : POneCId;
      BowDocBs->DocCIdVV.Add(); IAssert(DId==BowDocBs->DocCIdVV.Len()-1);
      BowDocBs->DocCIdVV.Last().Gen(1, 0);
      BowDocBs->DocCIdVV.Last().Add(CId);
      // words (attributes)
      PBowSpV SpV=TBowSpV::New(DId);
      BowDocBs->DocSpVV.Add(SpV); IAssert(DId==BowDocBs->DocSpVV.Len()-1);
      Lx.GetSym(syInt, syEoln);
      while (Lx.Sym==syInt){
        int WId=Lx.Int;
        Lx.GetSym(syColon);
        Lx.GetSym(syFlt); double WordFq=Lx.Flt;
        Lx.GetSym(syInt, syEoln);
        SpV->AddWIdWgt(WId, WordFq);
        if (MxWId==-1){MxWId=WId;} else {MxWId=TInt::GetMx(MxWId, WId);}
        WIdToFqH.AddDat(WId)++;
      }
      if (!Lx.CmtStr.Empty()){
        // change document name to 'N' if comment 'docDesc=N'
        TStr CmtStr=Lx.CmtStr;
        static TStr DocNmPrefixStr="docDesc=";
        if (CmtStr.IsPrefix(DocNmPrefixStr)){
          TStr NewDocNm=
           TStr("D")+CmtStr.GetSubStr(DocNmPrefixStr.Len(), CmtStr.Len()-1);
          BowDocBs->DocNmToDescStrH.DelKey(DocNm);
          int NewDId=BowDocBs->DocNmToDescStrH.AddKey(NewDocNm);
          IAssert(DId==NewDId);
        }
      }
      SpV->Trunc();
      while (Lx.GetSym(syInt, syEoln, syEof)==syEoln){}
    }
  }
  // add missing words
  for (int WId=0; WId<=MxWId; WId++){
    if (!BowDocBs->IsWId(WId)){
      TStr WordStr=TInt::GetStr(WId, "W%d");
      int _WId=BowDocBs->AddWordStr(WordStr);
      IAssert(WId==_WId);
      TInt Fq;
      if (WIdToFqH.IsKeyGetDat(WId, Fq)){
        BowDocBs->PutWordFq(WId, Fq);
      }
    }
  }

  BowDocBs->AssertOk();
  return BowDocBs;
}

PBowDocBs TBowFl::LoadCpdTxt(
 const TStr& CpdFNm, const int& MxDocs,
 const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
 const int& MxNGramLen, const int& MnNGramFq,
 const bool& SaveDocP, const PNotify& Notify){
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(SwSetTypeNm);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(StemmerTypeNm);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    NGramBs=TNGramBs::GetNGramBsFromCpd(
     CpdFNm, MxDocs, MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  // traverse documents
  TIntH DocWIdToFqH(100);
  PSIn CpdSIn=TCpDoc::GetFirstCpd(CpdFNm); PCpDoc CpDoc; int Docs=0;
  while (TCpDoc::GetNextCpd(CpdSIn, CpDoc)){
    Docs++; if (Docs%100==0){printf("%d\r", Docs);}
    if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
    // get document-name
    TStr DocNm=CpDoc->GetDocId();
    // get document-categories
    TStrV CatNmV;
    for (int CatN=0; CatN<CpDoc->GetCats(); CatN++){
      CatNmV.Add(CpDoc->GetCatNm(CatN));
    }
    // get document-contents
    TStr DocStr=CpDoc->GetTxtStr();
    TStr DateStr=CpDoc->GetDateStr();
    // add document to bow
    int DId=BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocStr, SaveDocP);
    BowDocBs->PutDateStr(DId, DateStr);
  }
  // return results
  BowDocBs->AssertOk();
  return BowDocBs;
}

void TBowFl::SaveCpdToLnDocTxt(const TStr& InCpdFNm, const TStr& OutLnDocFNm){
  TFOut FOut(OutLnDocFNm); FILE* fOut=FOut.GetFileId();
  PSIn CpdSIn=TCpDoc::GetFirstCpd(InCpdFNm); PCpDoc CpDoc; int Docs=0;
  printf("Saving '%s' to '%s' ...\n", InCpdFNm.CStr(), OutLnDocFNm.CStr());
  while (TCpDoc::GetNextCpd(CpdSIn, CpDoc)){
    Docs++; if (Docs%100==0){printf("%d Docs\r", Docs);}
    // get document-name
    TStr DocNm=CpDoc->GetDocId();
    DocNm=TStr::GetFNmStr(DocNm);
    DocNm.ChangeChAll(' ', '_');
    // get document-categories
    TStrV CatNmV;
    for (int CatN=0; CatN<CpDoc->GetCats(); CatN++){
      CatNmV.Add(CpDoc->GetCatNm(CatN));}
    // get document-contents
    TChA DocChA=CpDoc->GetTxtStr();
    DocChA.ChangeCh('\r', ' ');
    DocChA.ChangeCh('\n', ' ');
    // save document
    fprintf(fOut, "%s", DocNm.CStr());
    for (int CatN=0; CatN<CatNmV.Len(); CatN++){
      fprintf(fOut, " !%s", CatNmV[CatN].CStr());}
    fprintf(fOut, " %s\n", DocChA.CStr());
  }
  printf("%d Docs\nDone.\n", Docs);
}

PBowDocBs TBowFl::LoadTBsTxt(
 const TStr& TBsFNm, const int& MxDocs,
 const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
 const int& MxNGramLen, const int& MnNGramFq){
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(SwSetTypeNm);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(StemmerTypeNm);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    NGramBs=TNGramBs::GetNGramBsFromTBs(
     TBsFNm, MxDocs,
     MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  // open input text-base
  TStr TxtBsNm=TBsFNm.GetFBase();
  TStr TxtBsFPath=TBsFNm.GetFPath();
  PTxtBs TxtBs=TTxtBs::New(TxtBsNm, TxtBsFPath, faRdOnly);
  // traverse documents
  TBlobPt TxtBsTrvBlobPt=TxtBs->FFirstDocId(); TBlobPt TxtBsDocId; int Docs=0;
  while (TxtBs->FNextDocId(TxtBsTrvBlobPt, TxtBsDocId)){
    Docs++; if (Docs%100==0){printf("%d\r", Docs);}
    if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
    // get document data
    TStr DocNm; TStr DocStr;
    TxtBs->GetDocNmStr(TxtBsDocId, DocNm, DocStr);
    // add document to bow
    BowDocBs->AddHtmlDoc(DocNm, TStrV(), DocStr, false);
  }
  // return results
  BowDocBs->AssertOk();
  return BowDocBs;
}

void TBowFl::LoadLnDocTxt(PBowDocBs BowDocBs, const TStr& LnDocFNm,
 TIntV& NewDIdV, const bool& NamedP, const int& MxDocs, const bool& SaveDocP,
 const PNotify& Notify) {
  // open line-doc file
  NewDIdV.Clr(); TFIn FIn(LnDocFNm); char Ch=' '; int Docs=0;
  while (!FIn.Eof()){
    Docs++; if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
    printf("%d\r", Docs);
    // document name
    TChA DocNm;
    if (NamedP){
      Ch=FIn.GetCh();
      while ((!FIn.Eof())&&(Ch!='\r')&&(Ch!='\n')&&(Ch!=' ')){
        DocNm+=Ch; Ch=FIn.GetCh();}
      DocNm.Trunc();
      if (DocNm.Empty()){Docs--; continue;}
    }
    // categories
    TStrV CatNmV;
    forever {
      while ((!FIn.Eof())&&(Ch==' ')){Ch=FIn.GetCh();}
      if (Ch=='!'){
        if (!FIn.Eof()){Ch=FIn.GetCh();}
        TChA CatNm;
        while ((!FIn.Eof())&&(Ch!='\r')&&(Ch!='\n')&&(Ch!=' ')){
          CatNm+=Ch; Ch=FIn.GetCh();}
        if (!CatNm.Empty()){CatNmV.Add(CatNm);}
      } else {
        break;
      }
    }
    // document text
    TChA DocChA;
    while ((!FIn.Eof())&&(Ch!='\r')&&(Ch!='\n')){
      DocChA+=Ch; Ch=FIn.GetCh();}
    // skip empty documents (empty lines)
    if (DocNm.Empty()&&DocChA.Empty()){
      continue;}
    // add document to document-base
    NewDIdV.Add(BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocChA, SaveDocP));
  }
  // return document-base
  BowDocBs->AssertOk();
  printf("\n");
}

PBowDocBs TBowFl::LoadLnDocTxt(
 const TStr& LnDocFNm, const bool& NamedP, const int& MxDocs,
 const PSwSet& SwSet, const PStemmer& Stemmer,
 const int& MxNGramLen, const int& MnNGramFq, const bool& SaveDocP,
 const PNotify& Notify){
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    NGramBs=TNGramBs::GetNGramBsFromLnDoc(
     LnDocFNm, NamedP, MxDocs,
     MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  TIntV NewDIdV; LoadLnDocTxt(BowDocBs, LnDocFNm, NewDIdV, NamedP, -1, SaveDocP, Notify);
  return BowDocBs;
}

PBowDocBs TBowFl::LoadLnDocTxt(
 const TStr& LnDocFNm, const bool& NamedP, const int& MxDocs,
 const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
 const int& MxNGramLen, const int& MnNGramFq, const bool& SaveDocP,
   const PNotify& Notify){
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(SwSetTypeNm);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(StemmerTypeNm);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    NGramBs=TNGramBs::GetNGramBsFromLnDoc(
     LnDocFNm, NamedP, MxDocs,
     MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  TIntV NewDIdV; LoadLnDocTxt(BowDocBs, LnDocFNm, NewDIdV, NamedP, -1, SaveDocP, Notify);
  return BowDocBs;
}

void TBowFl::SaveLnDocTxt(const PBowDocBs& BowDocBs, const TStr& FNm, const bool& UseDocStrP){
  TFOut SOut(FNm);
  int Docs=BowDocBs->GetDocs();
  for (int DId=0; DId<Docs; DId++){
    printf("%d/%d\r", DId+1, Docs);
    // output document-name
    TStr DocNm=TStr::GetFNmStr(BowDocBs->GetDocNm(DId));
    SOut.PutStr(DocNm);
    // output categories
    for (int CIdN=0; CIdN<BowDocBs->GetDocCIds(DId); CIdN++){
      int CId=BowDocBs->GetDocCId(DId, CIdN);
      TStr CatNm=TStr::GetFNmStr(BowDocBs->GetCatNm(CId));
      SOut.PutCh(' '); SOut.PutCh('!'); SOut.PutStr(CatNm);
    }
    // output words
    if (UseDocStrP){
      TStr DocStr=BowDocBs->GetDocStr(DId);
      DocStr.DelChAll('\n'); DocStr.DelChAll('\r');
      SOut.PutCh(' '); SOut.PutStr(DocStr);
    } else {
        int DocWIds=BowDocBs->GetDocWIds(DId);
        int WId; double WordFq;
        for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
          BowDocBs->GetDocWIdFq(DId, DocWIdN, WId, WordFq);
          TStr WordStr=BowDocBs->GetWordStr(WId);
          for (int WordFqN=0; WordFqN<WordFq; WordFqN++){
            SOut.PutCh(' '); SOut.PutStr(WordStr);
          }
        }
    }
    SOut.PutLn();
  }
  printf("\n");
}

PBowDocBs TBowFl::LoadReuters21578Txt(
 const TStr& FPath, const int& MxDocs,
 const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
 const int& MxNGramLen, const int& MnNGramFq, const bool& SaveDocP,
 const PNotify& Notify){
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(SwSetTypeNm);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(StemmerTypeNm);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    NGramBs=TNGramBs::GetNGramBsFromReuters21578(
     FPath, MxDocs,
     MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  // traverse directory
  TFFile FFile(FPath, ".SGM", false); TStr FNm; int Docs=0;
  while (FFile.Next(FNm)){
    printf("Processing file '%s'\n", FNm.CStr());
    TIntH DocWIdToFqH(100);
    TXmlDocV LDocV; TXmlDoc::LoadTxt(FNm, LDocV);
    for (int LDocN=0; LDocN<LDocV.Len(); LDocN++){
      Docs++; if (Docs%100==0){printf("%d\r", Docs);}
      if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
      // get document-name
      PXmlDoc Doc=LDocV[LDocN];
      PXmlTok DocTok=Doc->GetTok();
      TStr DocNm=DocTok->GetArgVal("NEWID");
      // get document-categories
      TStrV CatNmV;
      TXmlTokV TopicsTokV; Doc->GetTagTokV("REUTERS|TOPICS|D", TopicsTokV);
      for (int TopicsTokN=0; TopicsTokN<TopicsTokV.Len(); TopicsTokN++){
        TStr CatNm=TopicsTokV[TopicsTokN]->GetTokStr(false);
        CatNmV.Add(CatNm);
      }
      // get document-contents
      PXmlTok DocStrTok=Doc->GetTagTok("REUTERS|TEXT");
      TStr DocStr=DocStrTok->GetTokStr(false);
      // add document to bow
      int DId=BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocStr, SaveDocP);
      // train & test data
      if ((DocTok->GetArgVal("LEWISSPLIT")=="TRAIN")&&(DocTok->GetArgVal("TOPICS")=="YES")){
        BowDocBs->AddTrainDId(DId);}
      if ((DocTok->GetArgVal("LEWISSPLIT")=="TEST")&&(DocTok->GetArgVal("TOPICS")=="YES")){
        BowDocBs->AddTestDId(DId);}
    }
    if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
  }
  // return results
  BowDocBs->AssertOk();
  return BowDocBs;
}

PBowDocBs TBowFl::LoadCiaWFBTxt(
 const TStr& FPath, const int& MxDocs,
 const TStr& SwSetTypeNm, const TStr& StemmerTypeNm,
 const int& MxNGramLen, const int& MnNGramFq){
  // load CiaWFB documents into base
  PCiaWFBBs CiaWFBBs=TCiaWFBBs::LoadHtml(FPath);
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(SwSetTypeNm);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(StemmerTypeNm);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    TStrV HtmlStrV;
    for (int CountryN=0; CountryN<CiaWFBBs->GetCountries(); CountryN++){
      PCiaWFBCountry CiaWFBCountry=CiaWFBBs->GetCountry(CountryN);
      HtmlStrV.Add(CiaWFBCountry->GetDescStr());
    }
    NGramBs=TNGramBs::GetNGramBsFromHtmlStrV(
     HtmlStrV, MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create document-base
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  for (int CountryN=0; CountryN<CiaWFBBs->GetCountries(); CountryN++){
    PCiaWFBCountry CiaWFBCountry=CiaWFBBs->GetCountry(CountryN);
    TStr CountryNm=CiaWFBCountry->GetCountryNm();
    TStr DescStr=CiaWFBCountry->GetDescStr();
    TStr CatNm=CiaWFBCountry->GetFldVal("Map references").GetTrunc();
    TStrV CatNmV; CatNmV.Add(CatNm);
    BowDocBs->AddHtmlDoc(CountryNm, CatNmV, DescStr, true);
  }
  BowDocBs->AssertOk();
  return BowDocBs;
}

void TBowFl::SaveSparseMatlabTxt(const PBowDocBs& BowDocBs,
    const PBowDocWgtBs& BowDocWgtBs, const TStr& FNm,
    const TStr& CatFNm, const TIntV& _DIdV) {

  TIntV DIdV;
  if (_DIdV.Empty()) {
      BowDocBs->GetAllDIdV(DIdV);
  } else {
      DIdV = _DIdV;
  }
  // generate map of row-ids to words
  TFOut WdMapSOut(TStr::PutFExt(FNm, ".row-to-word-map.dat"));
  for (int WId = 0; WId < BowDocWgtBs->GetWords(); WId++) {
    TStr WdStr = BowDocBs->GetWordStr(WId);
    WdMapSOut.PutStrLn(TStr::Fmt("%d %s", WId+1,  WdStr.CStr()));
  }
  WdMapSOut.Flush();
  // generate map of col-ids to document names
  TFOut DocMapSOut(TStr::PutFExt(FNm, ".col-to-docName-map.dat"));
  for (int DocN = 0; DocN < DIdV.Len(); DocN++) {
    const int DId = DIdV[DocN];
    TStr DocNm = BowDocBs->GetDocNm(DId);
    DocMapSOut.PutStrLn(TStr::Fmt("%d %d %s", DocN, DId,  DocNm.CStr()));
  }
  DocMapSOut.Flush();
  // save documents' sparse vectors
  TFOut SOut(FNm);
  for (int DocN = 0; DocN < DIdV.Len(); DocN++){
    const int DId = DIdV[DocN];
    PBowSpV DocSpV = BowDocWgtBs->GetSpV(DId);
    const int DocWIds = DocSpV->GetWIds();
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      const int WId = DocSpV->GetWId(DocWIdN);
      const double WordWgt = DocSpV->GetWgt(DocWIdN);
      SOut.PutStrLn(TStr::Fmt("%d %d %.16f", WId+1, DocN+1, WordWgt));
    }
  }
  SOut.Flush();
  // save documents' category sparse vectors
  if (!CatFNm.Empty()) {
    TFOut CatSOut(CatFNm);
    for (int DocN = 0; DocN < DIdV.Len(); DocN++){
      const int DId = DIdV[DocN];
      const int DocCIds = BowDocBs->GetDocCIds(DId);
      for (int DocCIdN=0; DocCIdN<DocCIds; DocCIdN++){
        const int CId = BowDocBs->GetDocCId(DId, DocCIdN);
        const double CatWgt = 1.0;
        CatSOut.PutStrLn(TStr::Fmt("%d %d %.16f", CId+1, DocN+1, CatWgt));
      }
    }
    CatSOut.Flush();
  }
}
