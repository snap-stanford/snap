/////////////////////////////////////////////////
// Text-Base-Results
TTxtBsRes::TTxtBsRes(
 const PTxtBs& _TxtBs, const TStr& _WixExpStr, const TBlobPtV& _DocIdV):
  Ok(true), ErrMsg("Ok"),
  TxtBs(_TxtBs), WixExpStr(_WixExpStr), DocIdV(){
  DocIdV.Gen(_DocIdV.Len());
  for (int DocIdN=0; DocIdN<_DocIdV.Len(); DocIdN++){
    DocIdV[DocIdN]=_DocIdV[DocIdN];}
  DocIdV.Sort(false);
}

void TTxtBsRes::GetDocInfo(
 const int& DocIdN, const int& MxDocTitleLen, const int& MxDocCtxLen,
 TStr& DocNm, TStr& DocTitleStr, TStr& DocStr, TStr& DocCtxStr) const {
  // get document as string
  TMem DocMem; TxtBs->GetDocNmMem(DocIdV[DocIdN], DocNm, DocMem);
  DocStr = TStr(DocMem);

  // convert document string to html
  PSIn HtmlSIn=TStrIn::New(DocStr);
  THtmlLx Lx(HtmlSIn);

  // construct title and document-context
  TChA DocTitleChA; bool IsDocTitleComplete=true;
  TChA DocCtxChA;
  TLxSym PrevLxSym=syUndef; TLxSym CurLxSym=syUndef;
  bool InTitleTag=false;
  while ((DocCtxChA.Len()<MxDocCtxLen)&&(Lx.GetSym()!=hsyEof)){
    if ((Lx.Sym==hsyStr)||(Lx.Sym==hsyNum)||(Lx.Sym==hsySSym)){
      PrevLxSym=CurLxSym;
      CurLxSym=THtmlDoc::GetLxSym(Lx.Sym, Lx.UcChA);
      if (InTitleTag){
        if (DocTitleChA.Len()<MxDocTitleLen){
          if (TLxSymStr::IsSep(PrevLxSym, CurLxSym)){DocTitleChA+=' ';}
          DocTitleChA+=Lx.ChA;
        } else {
          IsDocTitleComplete=false;
        }
      } else {
        if (TLxSymStr::IsSep(PrevLxSym, CurLxSym)){DocCtxChA+=' ';}
        DocCtxChA+=Lx.ChA;
      }
    } else
    if ((Lx.Sym==hsyBTag)||(Lx.Sym==hsyETag)){
      if (Lx.UcChA==THtmlTok::TitleTagNm){
        InTitleTag=(Lx.Sym==hsyBTag); PrevLxSym=syUndef;
      } else
      if (Lx.UcChA==THtmlTok::CardTagNm){
        DocTitleChA=Lx.GetArg(THtmlTok::TitleArgNm);
      }
    }
  }
  if (!IsDocTitleComplete){DocTitleChA+="...";}
  if (Lx.Sym!=hsyEof){DocCtxChA+="...";}
  DocTitleStr=THtmlLx::GetEscapedStr(DocTitleChA);
  DocCtxStr=THtmlLx::GetEscapedStr(DocCtxChA);
}

void TTxtBsRes::GetHitSetMnMxDocN(
 const int& HitSetN, const int& HitSetDocs, int& MnDocN, int& MxDocN) const {
  // general checks
  IAssert(IsOk());
  IAssert((HitSetN>0)&&(HitSetDocs>0));
  // min. & max. document numbers in current hit-set
  MnDocN=(HitSetN-1)*HitSetDocs;
  MxDocN=MnDocN+HitSetDocs-1;
  MxDocN=TInt::GetMn(MxDocN, GetDocs()-1);
}

void TTxtBsRes::GetHitSet(
 const int& HitSetN, const int& HitSetDocs, const int& TocHitSets,
 const TStr& HitSetUrlFldNm, const PUrlEnv& UrlEnv,
 TStr& PrevTocUrlStr, TStrPrV& TocNmUrlStrPrV, TStr& NextTocUrlStr) const {
  // prepare output values
  PrevTocUrlStr.Clr(); TocNmUrlStrPrV.Clr(); NextTocUrlStr.Clr();

  // general checks
  IAssert(IsOk());
  IAssert((HitSetN>0)&&(HitSetDocs>0)&&(TocHitSets>0));
  if (GetDocs()==0){return;}

  // min. & max. global hit-set numbers
  int MnHitSetN=1;
  int MxHitSetN=(GetDocs()-1)/HitSetDocs+1;
  if ((HitSetN<MnHitSetN)||(MnHitSetN>MxHitSetN)){return;}

  // min. & max. hit-set numbers in current string
  int MnTocHitSetN=((HitSetN-1)/TocHitSets)*TocHitSets+1;
  int MxTocHitSetN=MnTocHitSetN+TocHitSets-1;
  MxTocHitSetN=TInt::GetMn(MxTocHitSetN, MxHitSetN);
  PUrlEnv HitSetUrlEnv=TUrlEnv::MkClone(UrlEnv);

  // previous hit-set
  if (HitSetN!=MnHitSetN){
    HitSetUrlEnv->AddKeyVal(HitSetUrlFldNm, TInt::GetStr(HitSetN-1));
    PrevTocUrlStr=TStr("/")+HitSetUrlEnv->GetFullUrlStr();
  }

  // hit-set numbers
  for (int TocHitSetN=MnTocHitSetN; TocHitSetN<=MxTocHitSetN; TocHitSetN++){
    TStr Nm=TInt::GetStr(TocHitSetN);
    TStr UrlStr;
    if (TocHitSetN!=HitSetN){
      HitSetUrlEnv->AddKeyVal(HitSetUrlFldNm, TInt::GetStr(TocHitSetN));
      UrlStr=TStr("/")+HitSetUrlEnv->GetFullUrlStr();
    }
    TocNmUrlStrPrV.Add(TStrPr(Nm, UrlStr));
  }

  // next hit-set
  if (HitSetN!=MxHitSetN){
    HitSetUrlEnv->AddKeyVal(HitSetUrlFldNm, TInt::GetStr(HitSetN+1));
    NextTocUrlStr=TStr("/")+HitSetUrlEnv->GetFullUrlStr();
  }
}

/////////////////////////////////////////////////
// Text-Base
const TStr TTxtBs::TxtBsFExt=".tbs";
const TStr TTxtBs::TxtBsDocNmFExt=".tdn";
const TStr TTxtBs::TxtBsDocDatFExt=".tdd";

void TTxtBs::GenTxtBsMainFile(const TStr& Nm, const TStr& FPath){
  // get file-names
  TStr TxtBsFNm; TStr TxtBsDocNmFNm; TStr TxtBsDocDatFNm;
  GetFNms(Nm, FPath, TxtBsFNm, TxtBsDocNmFNm, TxtBsDocDatFNm);
  // open main file
  PSOut SOut=TFOut::New(TxtBsFNm);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);
  // write header info
  Lx.PutUQStr("Text Base Main File"); Lx.PutLn();
  Lx.PutVarStr("Original Name", Nm, true, false);
  Lx.PutVarStr("Original Path", FPath, true, false);
  Lx.PutVarStr("Text-Base-Main", TxtBsFNm, true, false);
  Lx.PutVarStr("Document-Names", TxtBsDocNmFNm, true, false);
  Lx.PutVarStr("Document-Contents", TxtBsDocDatFNm, true, false);
  Lx.PutVarStr("Creation Time", TSecTm::GetCurTm().GetStr(), true, false);
}

void TTxtBs::AddToTxtBsMainFile(const TStr& TxtBsFNm, const TStr& MsgStr){
  // open main file
  PSOut SOut=TFOut::New(TxtBsFNm, true);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);
  // write the message
  Lx.PutUQStr(MsgStr);
  Lx.PutUQStr(TStr(" [")+TSecTm::GetCurTm().GetStr()+TStr("]"));
  Lx.PutLn();
}

TTxtBs::TTxtBs(const TStr& Nm, const TStr& FPath,
 const TFAccess& _Access, const TFAccess& _WixAccess, 
 const int& CacheSize):
  NrNm(TStr::GetNrFMid(Nm)), NrFPath(TStr::GetNrFPath(FPath)),
  TxtBsFNm(), Access(_Access), WixAccess(_WixAccess),
  Wix(), DocNmH(), DocBBs(){
  // prepare file-names
  TStr TxtBsDocNmFNm; TStr TxtBsDocDatFNm;
  GetFNms(NrNm, NrFPath, TxtBsFNm, TxtBsDocNmFNm, TxtBsDocDatFNm);

  // prepare working set
  int DocNmHPorts;
  if (Access==faCreate){
    DocNmHPorts=1000000;
    GenTxtBsMainFile(NrNm, NrFPath);
  } else {
    IAssertR(Exists(NrNm, NrFPath), "[" + NrNm + ", " + NrFPath + "]");
    DocNmHPorts=-1;
    TStr OpenMsgStr=TStr("Open (")+TFRnd::GetStrFromFAccess(Access)+")";
    AddToTxtBsMainFile(TxtBsFNm, OpenMsgStr);
  }
  // index
  if (WixAccess==faUndef){WixAccess=Access;}
  Wix=PWix(new TWix(Nm, FPath, WixAccess, CacheSize));

  DocNmH=PDocNmH(new TDocNmH(TxtBsDocNmFNm, Access, DocNmHPorts, 100000000));
  DocBBs=PBlobBs(new TMBlobBs(TxtBsDocDatFNm, Access));
}

TTxtBs::~TTxtBs(){
  AddToTxtBsMainFile(TxtBsFNm, "Close");
}

TStr TTxtBs::GetMemUsedStr(){
  return TStr()+
   "Wix:["+Wix->GetMemUsedStr()+"]"+
   " DocNmH:"+TInt::GetMegaStr((int)DocNmH->GetMemUsed());
}

TBlobPt TTxtBs::AddDocMem(
 const TStr& UrlStr, const TMem& DocMem, const bool& IndexP){
  // save the document as name+contents
  TMOut NmDocSOut(UrlStr.Len()+DocMem.Len()+100);
  UrlStr.Save(NmDocSOut); DocMem.Save(NmDocSOut);
  PSIn NmDocSIn=NmDocSOut.GetSIn();
  TBlobPt DocId=DocBBs->PutBlob(NmDocSIn);

  // save the document name
  TTxtBsDocFDat FDat(TBlobPt(), TSecTm::GetCurTm(), DocMem.Len(), DocId);
  DocNmH->AddFDat(UrlStr, FDat);

  // index html contents
  if (IndexP){
    PSIn HtmlDocSIn=TStrIn::New(DocMem.GetAsStr());
    Wix->AddHtmlDoc(DocId, HtmlDocSIn);
  }

  return DocId;
}

TBlobPt TTxtBs::AddHtmlDoc(
 const TStr& UrlStr, const PSIn& SIn, const bool& IndexP){
  TMem DocMem; TMem::LoadMem(SIn, DocMem);
  return AddDocMem(UrlStr, DocMem, IndexP);
}

void TTxtBs::GetDocNmStr(const TBlobPt& BlobPt, TStr& DocNm, TStr& DocStr){
  PSIn NmDocSIn=DocBBs->GetBlob(BlobPt); // read name+contents blob
  DocNm=TStr(*NmDocSIn); // construct the name
  TMem DocMem=TMem(*NmDocSIn); DocStr=TStr(DocMem); // construct the contents
}

void TTxtBs::GetDocNmMem(const TBlobPt& BlobPt, TStr& DocNm, TMem& DocMem){
  PSIn NmDocSIn=DocBBs->GetBlob(BlobPt); // read name+contents blob
  DocNm=TStr(*NmDocSIn); // construct the name
  DocMem=TMem(*NmDocSIn); // construct the contents
}

TStr TTxtBs::GetDocNm(const TBlobPt& BlobPt){
  PSIn NmDocSIn=DocBBs->GetBlob(BlobPt); // read name+contents blob
  return TStr(*NmDocSIn); // construct & return the name
}

TStr TTxtBs::GetDocStr(const TStr& DocNm){
  TTxtBsDocFDat DocFDat;
  DocNmH->GetFDat(DocNm, DocFDat);
  TBlobPt DocBPt=DocFDat.DocId;
  TStr _DocNm; TMem DocMem; GetDocNmMem(DocFDat.DocId, _DocNm, DocMem);
  return TStr(DocMem);
}

void TTxtBs::GetDocMem(const TStr& DocNm, TMem& DocMem){
  TTxtBsDocFDat DocFDat;
  DocNmH->GetFDat(DocNm, DocFDat);
  TBlobPt DocBPt=DocFDat.DocId;
  TStr _DocNm; GetDocNmMem(DocFDat.DocId, _DocNm, DocMem);
}

TBlobPt TTxtBs::FFirstDocId(){
  return DocNmH->FFirstKeyId();
}

bool TTxtBs::FNextDocId(TBlobPt& TrvBlobPt, TBlobPt& DocId){
  TBlobPt KeyId;
  if (DocNmH->FNextKeyId(TrvBlobPt, KeyId)){
    TStr DocNm; TTxtBsDocFDat DocFDat;
    DocNmH->GetKeyFDat(KeyId, DocNm, DocFDat);
    DocId=DocFDat.DocId;
    return true;
  } else {
    return false;
  }
}

PTxtBsRes TTxtBs::Search(const TStr& WixExpStr){
  PWixExp WixExp=PWixExp(new TWixExp(WixExpStr));
  PTxtBsRes TxtBsRes;
  if (WixExp->IsOk()){
    PWixExpDocIdSet DocIdSet=WixExp->Eval(Wix);
    TxtBsRes=PTxtBsRes(new TTxtBsRes(this, WixExpStr, DocIdSet->GetDocIdV()));
  } else {
    TxtBsRes=PTxtBsRes(new TTxtBsRes(WixExp->GetErrMsg()));
  }
  return TxtBsRes;
}

void TTxtBs::GenIndex(const TStr& Nm, const TStr& FPath, const bool& MemStatP){
  // open text-base
  PTxtBs TxtBs=TTxtBs::New(Nm, FPath, faRdOnly, faCreate);
  // traverse documents
  TSecTm StartTm=TSecTm::GetCurTm();
  TBlobPt TrvBlobPt=TxtBs->FFirstDocId(); 
  TBlobPt DocId; int DocN=0; TStr DocNm; TMem DocMem;
  while (TxtBs->FNextDocId(TrvBlobPt, DocId)){
    // get document data
    //TxtBs->GetDocNmStr(DocId, DocNm, DocStr);
    TxtBs->GetDocNmMem(DocId, DocNm, DocMem);
    // index document
    //PSIn DocSIn=TStrIn::New(DocStr);
    PSIn DocSIn=TMemIn::New(DocMem);
    TxtBs->Wix->AddHtmlDoc(DocId, DocSIn);
    // statistics
    DocN++; 
    if (DocN%100==0){
      int SecsSf=TSecTm::GetDSecs(StartTm, TSecTm::GetCurTm());
      if (MemStatP){
        int64 MemUsed=TxtBs->GetMemUsed();
        printf("%d: MemUsed:%s [%s] / %d secs\n", DocN,
         TUInt64::GetMegaStr(MemUsed).CStr(), TxtBs->GetMemUsedStr().CStr(),
         SecsSf);
      } else {
        printf("%d docs / %d secs\r", DocN, SecsSf);
      }
    }
  }
}

void TTxtBs::SaveTxt(const PSOut& SOut){
  {TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);

  Lx.PutUQStr("---DocNmH---------------------"); Lx.PutLn();
  TBlobPt DocNmP=DocNmH->FFirstKeyId(); TBlobPt NmId;
  while (DocNmH->FNextKeyId(DocNmP, NmId)){
    TStr DocNm; TTxtBsDocFDat FDat;
    DocNmH->GetKeyFDat(NmId, DocNm, FDat);
    Lx.PutVarStr("DocNm", DocNm, false);
    Lx.PutVarStr("MainNmId", FDat.MainNmId.GetStr(), false);
    Lx.PutVarStr("Time", FDat.Tm.GetStr(), false);
    Lx.PutVarInt("Len", FDat.Len, false);
    Lx.PutVarStr("DocId", FDat.DocId.GetStr(), false);
    Lx.PutLn();
  }
  Lx.PutUQStr("------------------------------"); Lx.PutLn();}

  Wix->SaveTxt(SOut);
}

void TTxtBs::GetFNms(
 const TStr& Nm, const TStr& FPath,
 TStr& TxtBsFNm, TStr& TxtBsDocNmFNm, TStr& TxtBsDocDatFNm){
  TStr NrFPath=TStr::GetNrFPath(FPath);
  TStr NrNm=TStr::GetNrFMid(Nm);
  TxtBsFNm=NrFPath+NrNm+TxtBsFExt;
  TxtBsDocNmFNm=NrFPath+NrNm+TxtBsDocNmFExt;
  TxtBsDocDatFNm=NrFPath+NrNm+TxtBsDocDatFExt;
}

bool TTxtBs::Exists(const TStr& Nm, const TStr& FPath){
  TStr TxtBsFNm; TStr TxtBsDocNmFNm; TStr TxtBsDocDatFNm;
  GetFNms(Nm, FPath, TxtBsFNm, TxtBsDocNmFNm, TxtBsDocDatFNm);
  return
   TFile::Exists(TxtBsFNm)&&
   TFile::Exists(TxtBsDocNmFNm)&&
   TMBlobBs::Exists(TxtBsDocDatFNm)&&
   TWix::Exists(Nm, FPath);
}

void TTxtBs::Del(const TStr& Nm, const TStr& FPath){
  if (Exists(Nm, FPath)){
    TStr TxtBsFNm; TStr TxtBsDocNmFNm; TStr TxtBsDocDatFNm;
    GetFNms(Nm, FPath, TxtBsFNm, TxtBsDocNmFNm, TxtBsDocDatFNm);
    TFile::Del(TxtBsFNm);
    TFile::Del(TxtBsDocNmFNm);
    TFile::Del(TxtBsDocDatFNm);
    TWix::Del(Nm, FPath);
  }
}
