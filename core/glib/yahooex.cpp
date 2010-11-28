/////////////////////////////////////////////////
// Includes
#include "yahooex.h"
#include "ss.h"

/////////////////////////////////////////////////
// Yahoo-Experiment
const TStr TYExpr::LogFPfx="Log";
const TStr TYExpr::DatFExt=".Dat";
const TStr TYExpr::TxtFExt=".Txt";
const TStr TYExpr::TmpFExt=".Tmp";
const TStr TYExpr::YWebBsFPfx="Y";
const TStr TYExpr::YTsWebBsFPfx="YTs";
const TStr TYExpr::YBsFPfx="YBs";
const TStr TYExpr::YWordFPfx="YWord";
const TStr TYExpr::YCtgFPfx="YCtg";
const TStr TYExpr::YMdFPfx="YMd";
const TStr TYExpr::YDsBsFPfx="YDsBs";
const TStr TYExpr::YFSelFPfx="YFSel";
const TStr TYExpr::YInvIxFPfx="YInvIx";

void TYExpr::GenYBs(
 const TStr& YRootFPath, const TStr& YDmNm, const PNotify& Notify){
  // file names
  TStr NrYRootFPath=TStr::GetNrFPath(YRootFPath);
  // load web-base file
  PXWebBs WebBs=PXWebBs(new TWebMemBs(NrYRootFPath+YWebBsFPfx+YDmNm));
  // generate yahoo-base
  PYBs YBs=PYBs(new TYBs(WebBs, 5, 4, swstEnglish523, TStrV(), Notify));
  // save yahoo-base
  YBs->Save(NrYRootFPath+YBsFPfx+YDmNm+DatFExt);
  YBs->SaveTxt(NrYRootFPath+YBsFPfx+YDmNm+TxtFExt);
  YBs->SaveTxtWords(NrYRootFPath+YWordFPfx+YDmNm+TxtFExt);
  YBs->SaveTxtCtgs(NrYRootFPath+YCtgFPfx+YDmNm+TxtFExt);
}

void TYExpr::GenYDsBs(
 const TStr& YRootFPath, const TStr& YDmNm, const PNotify& Notify){
  // file names
  TStr NrYRootFPath=TStr::GetNrFPath(YRootFPath);
  TStr YBsFNm=NrYRootFPath+YBsFPfx+YDmNm+DatFExt;
  TStr YDsBsFNm=NrYRootFPath+YDsBsFPfx+YDmNm+DatFExt;
  TStr YDsBsTxtFNm=NrYRootFPath+YDsBsFPfx+YDmNm+TxtFExt;
  // load yahoo-base file
  TFIn YBsFIn(YBsFNm);
  PYBs YBs=PYBs(new TYBs(YBsFIn));
  // generate yahoo-distribution-base
  PYDsBs YDsBs=PYDsBs(new TYDsBs(ydnLnSects, 1.00, 0.001, 0.0001, YBs, Notify));
  // save yahoo-distribution-base
  YDsBs->Save(YDsBsFNm);
  YDsBs->SaveTxt(TFOut::New(YDsBsTxtFNm), YBs);
}

void TYExpr::PutMomHd(TOLx& Lx, const TStr& VarNm){
  Lx.PutUQStr(VarNm+"Mean"); Lx.PutUQStr(VarNm+"SDev");
  Lx.PutUQStr(VarNm+"SErr"); Lx.PutUQStr(VarNm+"Med");
  Lx.PutUQStr(VarNm+"Q1"); Lx.PutUQStr(VarNm+"Q3");
}

void TYExpr::PutMomVal(TOLx& Lx, const PMom& Mom){
  if (Mom->IsUsable()){
    Lx.PutFlt(Mom->GetMean()); Lx.PutFlt(Mom->GetSDev());
    Lx.PutFlt(Mom->GetSErr()); Lx.PutFlt(Mom->GetMedian());
    Lx.PutFlt(Mom->GetQuart1()); Lx.PutFlt(Mom->GetQuart3());
  } else {
    for (int MomN=0; MomN<6; MomN++){Lx.PutFlt(-1);}
  }
}

void TYExpr::Go(
 const PSOut& SOut, const bool& PutHd, const bool& SaveTmp,
 const TStr& YDmNm, const TStr& YRootFPath, const TStr& InfoMsg,
 const PYBs& _YBs, const PYDsBs& _YDsBs,
 const PYFSelBs& _YFSelBs, const PYInvIx& _YInvIx,
 const int& SampleDocs,
 const int& TsRuns, const int& TsDocs,
 const int& MnTsWords, const double& MnTsWordPrb, const bool& ExclTs,
 const int& MxNGram, const int& MnWordFq, const TSwSetTy& SwSetTy,
 const TYDsBsNrType& YDsBsNrType, const double& YDsBsWordFqExp,
 const double& YDsBsMnDocWordPrb, const double& YDsBsSumAllWordPrb,
 const TYFSelType& YFSelType, const TFltV& YFSelsV,
 const bool& YFSelPosWords, const PAttrEstV& YFSelAttrEstV,
 const TYNegDsType& YNegDsType, const TYPriorType& YPriorType,
 const double& YInvIxEstExp, const TFltV& YInvIxSumEstPrbV,
 const TIntV& YInvIxMnDocFqV,
 const TFltV& PrRePrbTshV,
 const int& ShowTopHits, const PNotify& Notify){
  // prepare base file path
  TStr NrYRootFPath=TStr::GetNrFPath(YRootFPath);

  // check for correctness of input bases
  if (!_YInvIx.Empty()){
    IAssert((!_YBs.Empty())&&(!_YDsBs.Empty())&&(!_YFSelBs.Empty()));}
  if (!_YFSelBs.Empty()){
    IAssert((!_YBs.Empty())&&(!_YDsBs.Empty()));}
  if (!_YDsBs.Empty()){
    IAssert(!_YBs.Empty());}

  // prepare test example set resulting in TsWebBs and TsWebPgIdV
  PXWebBs TsWebBs=PXWebBs(new TWebMemBs(NrYRootFPath+YTsWebBsFPfx+YDmNm));
  TIntV TsWebPgIdV;
  {int TsWebPgP=TsWebBs->FFirstWebPg(); int TsWebPgId;
  while (TsWebBs->FNextWebPg(TsWebPgP, TsWebPgId)){
    TsWebPgIdV.Add(TsWebPgId);}}
  IAssert(TsRuns*TsDocs<=TsWebPgIdV.Len());
  TRnd Rnd; TsWebPgIdV.Shuffle(Rnd);

  // loop over test runs
  for (int TsRunN=0; TsRunN<TsRuns; TsRunN++){
//  for (int TsRunN=4; TsRunN<TsRuns; TsRunN++){
    int MnTsDocN=TsRunN*TsDocs;
    int MxTsDocN=MnTsDocN+TsDocs-1;

    // prepare Yahoo base resulting in YBs
    PYBs YBs=_YBs;
    if (YBs.Empty()){
      if (ExclTs){
        TStrV TsUrlStrV;
        for (int TsWebPgN=MnTsDocN; TsWebPgN<=MxTsDocN; TsWebPgN++){
          int TsWebPgId=TsWebPgIdV[TsWebPgN];
          TsUrlStrV.Add(TsWebBs->GetUrlStr(TsWebPgId));
        }
        PXWebBs WebBs=new TWebMemBs(NrYRootFPath+YWebBsFPfx+YDmNm);
        YBs=PYBs(new TYBs(
         WebBs, MxNGram, MnWordFq, SwSetTy, TsUrlStrV, Notify));
        if (SaveTmp){YBs->SaveTxt(PSOut(new TFOut(YBsFPfx+YDmNm+TxtFExt)));}
      } else {
        TFIn YBsFIn(NrYRootFPath+YBsFPfx+YDmNm+DatFExt);
        YBs=PYBs(new TYBs(YBsFIn));
        if (SaveTmp){YBs->SaveTxt(PSOut(new TFOut(YBsFPfx+YDmNm+TxtFExt)));}
      }
    }

    // prepare Yahoo distribution base resulting in YDsBs
    PYDsBs YDsBs=_YDsBs;
    if (YDsBs.Empty()){
      YDsBs=PYDsBs(new TYDsBs(YDsBsNrType, YDsBsWordFqExp,
       YDsBsMnDocWordPrb, YDsBsSumAllWordPrb, YBs, Notify));
      if (SaveTmp){YDsBs->SaveTxt(PSOut(new TFOut(YDsBsFPfx+YDmNm+TxtFExt)), YBs);}
    }

    // prepare general Yahoo statistics resulting in
    // DocDistMom (distance between Yahoo documents),
    // DocSectsMom (number of sections per Yahoo document),
    // DocWordsMom (number of words per Yahoo document)
    PMom DocDistMom=PMom(new TMom());
    PMom DocSectsMom=PMom(new TMom());
    PMom DocWordsMom=PMom(new TMom());
    for (int DocIdN=0; DocIdN<SampleDocs; DocIdN++){
      int DocId1=TInt::GetRnd(YBs->GetDocs());
      int DocId2=TInt::GetRnd(YBs->GetDocs());
      int DocDist=YBs->GetDocDist(DocId1, DocId2);
      int DocSects=YBs->GetDocSects(DocId1);
      int DocWords=YDsBs->GetWordDs(DocId1)->GetWordIds();
      if (DocDist!=-1){DocDistMom->Add(DocDist);}
      DocSectsMom->Add(DocSects);
      DocWordsMom->Add(DocWords);
    }
    DocDistMom->Def();
    DocSectsMom->Def();
    DocWordsMom->Def();

    // write result file header
    TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
    if (PutHd){
      Lx.PutUQStr("DmNm"); Lx.PutUQStr("Info");
      Lx.PutUQStr("NrType"); Lx.PutUQStr("WordFqExp");
      Lx.PutUQStr("MnDocWordPrb"); Lx.PutUQStr("SumAllWordPrb");
      Lx.PutUQStr("FSelType"); Lx.PutUQStr("FSelPosWords");
      Lx.PutUQStr("FSels"); Lx.PutUQStr("FSelAttrEst");
      Lx.PutUQStr("NegDsType"); Lx.PutUQStr("PriorType");
      Lx.PutUQStr("EstExp"); Lx.PutUQStr("SumEstPrb"); Lx.PutUQStr("MnDocFq");
      Lx.PutUQStr("TsRunN"); Lx.PutUQStr("MnTsDocN"); Lx.PutUQStr("MxTsDocN");
      Lx.PutUQStr("MnTsWords"); Lx.PutUQStr("MnTsWordPrb");
      Lx.PutUQStr("ExclTs");
      Lx.PutUQStr("PrRcPrbTsh");
      Lx.PutUQStr("TsDocs"); Lx.PutUQStr("BadTsDocsPrb");
      Lx.PutUQStr("AllCfs"); Lx.PutUQStr("UnknownCfPrb");
      PutMomHd(Lx, "Visits"); PutMomHd(Lx, "Plc"); PutMomHd(Lx, "Prb");
      PutMomHd(Lx, "KwF05"); PutMomHd(Lx, "KwF1"); PutMomHd(Lx, "KwF2");
      PutMomHd(Lx, "KwF4"); PutMomHd(Lx, "KwF8"); PutMomHd(Lx, "KwF16");
      PutMomHd(Lx, "KwPrecis"); PutMomHd(Lx, "KwRecall");
      PutMomHd(Lx, "DocF05"); PutMomHd(Lx, "DocF1"); PutMomHd(Lx, "DocF2");
      PutMomHd(Lx, "DocF4"); PutMomHd(Lx, "DocF8"); PutMomHd(Lx, "DocF16");
      PutMomHd(Lx, "DocPrecis"); PutMomHd(Lx, "DocRecall");
      PutMomHd(Lx, "Prb95Dist"); PutMomHd(Lx, "Prb95Docs");
      Lx.PutUQStr("Prb95DocsPrb"); Lx.PutUQStr("Prb95NoDocs");
      PutMomHd(Lx, "Dist"); PutMomHd(Lx, "Sects");
      Lx.PutUQStr("Sects"); Lx.PutUQStr("AbsSects");
      PutMomHd(Lx, "Words"); PutMomHd(Lx, "FSelWords");
      Lx.PutLn();
    }

    // open log file
    PSOut LogSOut=new TFOut(LogFPfx+YDmNm+TxtFExt);

    // loop over feature selections
    for (int YFSelsN=0; YFSelsN<YFSelsV.Len(); YFSelsN++){
      double YFSels=YFSelsV[YFSelsN];
      TNotify::OnNotify(Notify, ntInfo,
       TStr("  | YFSels: ")+TFlt::GetStr(YFSels)+"%");

      // loop over attribute estimators
      for (int AttrEstN=0; AttrEstN<YFSelAttrEstV.Len(); AttrEstN++){
        PAttrEst YFSelAttrEst=YFSelAttrEstV[AttrEstN];
        TStr YFSelAttrEstNm;
        if (YFSelAttrEst.Empty()){YFSelAttrEstNm="OddsRatio";} \
        else {YFSelAttrEstNm=YFSelAttrEst->GetNm();}
        TNotify::OnNotify(Notify, ntInfo,
          TStr("  |  | AttrEst: ")+YFSelAttrEstNm);

        // prepare Yahoo feature selection base resulting in YFSelBs
        PYFSelBs YFSelBs=_YFSelBs;
        if (YFSelBs.Empty()){
          YFSelBs=PYFSelBs(new TYFSelBs(
           YFSelType, YFSels, YFSelPosWords, YFSelAttrEst,
           YNegDsType, YPriorType, YBs, YDsBs, Notify));
          if (SaveTmp){
            PSOut SOut=TFOut::New(YFSelFPfx+YDmNm+TxtFExt);
            YFSelBs->SaveTxt(SOut, YBs, YDsBs);
          }
        }

        // statistics for documents after feature selection
        PMom DocFSelWordsMom=PMom(new TMom());
        for (int DocIdN=0; DocIdN<SampleDocs; DocIdN++){
          int DocId=TInt::GetRnd(YBs->GetDocs());
          int DocWords=YFSelBs->GetWords(DocId);
          DocFSelWordsMom->Add(DocWords);
        }
        DocFSelWordsMom->Def();

        // loop over summary-estimate-probabilities
        for (int SumEstPrbN=0; SumEstPrbN<YInvIxSumEstPrbV.Len(); SumEstPrbN++){
          double YInvIxSumEstPrb=YInvIxSumEstPrbV[SumEstPrbN];
          TNotify::OnNotify(Notify, ntInfo,
           TStr("  |  |  | SumEstPrb: ")+TFlt::GetStr(YInvIxSumEstPrb));

          // prepare Yahoo inverted index resulting in YInvIx
          PYInvIx YInvIx=_YInvIx;
          if (YInvIx.Empty()){
            YInvIx=PYInvIx(new TYInvIx(YInvIxEstExp, YInvIxSumEstPrb,
             YBs, YDsBs, YFSelBs, Notify));
            if (SaveTmp){
              PSOut SOut=PSOut(new TFOut(YInvIxFPfx+YDmNm+TxtFExt));
              YInvIx->SaveTxt(SOut, YBs);
            }
          }

          // learning model resulting in Md
          PMd Md=new TMdYBayes(
           YNegDsType, YPriorType, YBs, YDsBs, YFSelBs, YInvIx);

          // loop over
          for (int MnDocFqN=0; MnDocFqN<YInvIxMnDocFqV.Len(); MnDocFqN++){
            int MnDocFq=YInvIxMnDocFqV[MnDocFqN];
            TNotify::OnNotify(Notify, ntInfo,
             TStr("  |  |  |  | MnDocFq: ")+TInt::GetStr(MnDocFq));

            // loop over Precision/Recall tresholds
            for (int PrRePrbTshN=0; PrRePrbTshN<PrRePrbTshV.Len(); PrRePrbTshN++){
              double PrRePrbTsh=PrRePrbTshV[PrRePrbTshN];
              TNotify::OnNotify(Notify, ntInfo,
               TStr("  |  |  |  |  | PrRePrbTsh: ")+TFlt::GetStr(PrRePrbTsh));

              // prepare evaluation quantities
              int BadTsDocs=0;
              int AllCfs=0; int UnknownCfs=0;
              PMom VisitsMom=PMom(new TMom());
              PMom RefDocPlcMom=PMom(new TMom());
              PMom RefDocPrbMom=PMom(new TMom());
              PMom KwF05Mom=PMom(new TMom());
              PMom KwF1Mom=PMom(new TMom());
              PMom KwF2Mom=PMom(new TMom());
              PMom KwF4Mom=PMom(new TMom());
              PMom KwF8Mom=PMom(new TMom());
              PMom KwF16Mom=PMom(new TMom());
              PMom KwPrecisMom=PMom(new TMom());
              PMom KwRecallMom=PMom(new TMom());
              PMom DocF05Mom=PMom(new TMom());
              PMom DocF1Mom=PMom(new TMom());
              PMom DocF2Mom=PMom(new TMom());
              PMom DocF4Mom=PMom(new TMom());
              PMom DocF8Mom=PMom(new TMom());
              PMom DocF16Mom=PMom(new TMom());
              PMom DocPrecisMom=PMom(new TMom());
              PMom DocRecallMom=PMom(new TMom());
              PMom Prb95DistMom=PMom(new TMom());
              PMom Prb95DocsMom=PMom(new TMom());
              int Prb95NoDocs=0;

              // loop over testing documents
              for (int TsWebPgN=MnTsDocN; TsWebPgN<=MxTsDocN; TsWebPgN++){
                int TsWebPgId=TsWebPgIdV[TsWebPgN];
                PXWebPg TsWebPg=TsWebBs->GetWebPg(TsWebPgId);
                PYWordDs TsWordDs=YBs->GetWordDs(TsWebPg);
                if (SaveTmp){TsWordDs->SaveTxt(LogSOut, YBs());}

                TOLx LogLx(LogSOut, TFSet()|oloFrcEoln|oloSigNum);
                PValRet ValRet=new TYValRet(
                 MnTsWordPrb, Md->GetDmHd(), YBs, TsWordDs);
                TStr TsUrlStr=TsWebPg->GetUrlStr();
                TStr RefUrlStr=TsWebPg->GetRefUrlStrV()[0];
                TStr RefUrlStrPath=TUrl(RefUrlStr).GetPathStr();
                int RefDocId=YBs->GetDocId(RefUrlStr);

                if (SaveTmp){
                  LogLx.PutVarStr("UrlStr", TsWebPg->GetUrlStr());
                  LogLx.PutVarStr("FromCategory", RefUrlStrPath);
                }

                // bad document (having not enough words)
                if (TsWordDs->GetSumWordFq()<MnTsWords){
                  if (SaveTmp){
                    TChA Msg;
                    Msg+=" *** Bad Document";
                    Msg+=TStr(' ')+TFlt::GetStr(YFSels)+'%';
                    Msg+=TStr(' ')+TFlt::GetStr(MnDocFq);
                    Msg+=TStr(" [")+TFlt::GetStr(TsWordDs->GetSumWordFq());
                    Msg+=TStr('<')+TFlt::GetStr(MnTsWords)+']';
                    TNotify::OnNotify(Notify, ntInfo, Msg);
                    LogLx.PutVarStr("Classification", Msg);
                  }
                  BadTsDocs++; continue;
                };

                // classification of test documents resulting in PrbUrlStrKdV
                TIntV ClassNV; YInvIx->GetDocIdV(TsWordDs, MnDocFq, ClassNV);
                TFltStrKdV PrbUrlStrKdV(ClassNV.Len(), 0);
                for (int ClassNN=0; ClassNN<ClassNV.Len(); ClassNN++){
                  int ClassN=ClassNV[ClassNN];
                  TStr UrlStr=Md->GetDmHd()->GetClassNm(ClassN);
                  PTbValDs ValDs=Md->GetPostrValDs(ValRet, ClassN);
                  double ClassPrb=ValDs->GetPrb_RelFq(TTbVal(true));
                  PrbUrlStrKdV.Add(TFltStrKd(ClassPrb, UrlStr));
                }
                PrbUrlStrKdV.Sort(false);

                // evaluation for keyword precision/recall
                TIntV RefCtgIdV; YBs->GetCtgIdV(RefDocId, RefCtgIdV);
                TIntV BestCtgIdV;
                {int DocPlcN=0;
                while ((DocPlcN<PrbUrlStrKdV.Len())&&
                 (PrbUrlStrKdV[DocPlcN].Key>PrRePrbTsh)){
                  TStr UrlStr=PrbUrlStrKdV[DocPlcN].Dat;
                  int DocId=YBs->GetDocId(UrlStr);
                  TIntV CtgIdV; YBs->GetCtgIdV(DocId, CtgIdV);
                  BestCtgIdV.AddVMerged(CtgIdV);
                  DocPlcN++;
                }}
                TIntV IntsCtgIdV; RefCtgIdV.Intrs(BestCtgIdV, IntsCtgIdV);
                double KwPrecis=1; double KwRecall=1;
                double KwF05=0; double KwF1=0; double KwF2=0;
                double KwF4=0; double KwF8=0; double KwF16=0;
                if (BestCtgIdV.Len()>0){
                  KwPrecis=IntsCtgIdV.Len()/double(BestCtgIdV.Len());}
                if (RefCtgIdV.Len()>0){
                  KwRecall=IntsCtgIdV.Len()/double(RefCtgIdV.Len());}
                if (KwPrecis+KwRecall>0){
                  KwF05=(1.25*KwPrecis*KwRecall)/(0.25*KwPrecis+KwRecall);
                  KwF1=(2*KwPrecis*KwRecall)/(1*KwPrecis+KwRecall);
                  KwF2=(5*KwPrecis*KwRecall)/(4*KwPrecis+KwRecall);
                  KwF4=(17*KwPrecis*KwRecall)/(16*KwPrecis+KwRecall);
                  KwF8=(65*KwPrecis*KwRecall)/(64*KwPrecis+KwRecall);
                  KwF16=(257*KwPrecis*KwRecall)/(256*KwPrecis+KwRecall);
                }
                KwPrecisMom->Add(KwPrecis); KwRecallMom->Add(KwRecall);
                KwF05Mom->Add(KwF05); KwF1Mom->Add(KwF1); KwF2Mom->Add(KwF2);
                KwF4Mom->Add(KwF4); KwF8Mom->Add(KwF8); KwF16Mom->Add(KwF16);

                // evaluation for document precision/recall
                TIntV ParentDocIdV; YBs->GetParentDocIdV(RefDocId, ParentDocIdV);
                TIntV BestDocIdV;
                {int DocPlcN=0;
                while ((DocPlcN<PrbUrlStrKdV.Len())&&
                 (PrbUrlStrKdV[DocPlcN].Key>PrRePrbTsh)){
                  TStr UrlStr=PrbUrlStrKdV[DocPlcN].Dat;
                  int DocId=YBs->GetDocId(UrlStr);
                  BestDocIdV.AddSorted(DocId);
                  DocPlcN++;
                }}
                TIntV IntsDocIdV; ParentDocIdV.Intrs(BestDocIdV, IntsDocIdV);
                double DocPrecis=1; double DocRecall=1;
                double DocF05=0; double DocF1=0; double DocF2=0;
                double DocF4=0; double DocF8=0; double DocF16=0;
                if (BestDocIdV.Len()>0){
                  DocPrecis=IntsDocIdV.Len()/double(BestDocIdV.Len());}
                if (ParentDocIdV.Len()>0){
                  DocRecall=IntsDocIdV.Len()/double(ParentDocIdV.Len());}
                if (DocPrecis+DocRecall>0){
                  DocF05=(1.25*DocPrecis*DocRecall)/(0.25*DocPrecis+DocRecall);
                  DocF1=(2*DocPrecis*DocRecall)/(1*DocPrecis+DocRecall);
                  DocF2=(5*DocPrecis*DocRecall)/(4*DocPrecis+DocRecall);
                  DocF4=(17*DocPrecis*DocRecall)/(16*DocPrecis+DocRecall);
                  DocF8=(65*DocPrecis*DocRecall)/(64*DocPrecis+DocRecall);
                  DocF16=(257*DocPrecis*DocRecall)/(256*DocPrecis+DocRecall);
                }
                DocPrecisMom->Add(DocPrecis); DocRecallMom->Add(DocRecall);
                DocF05Mom->Add(DocF05); DocF1Mom->Add(DocF1); DocF2Mom->Add(DocF2);
                DocF4Mom->Add(DocF4); DocF8Mom->Add(DocF8); DocF16Mom->Add(DocF16);

                // evaluation for documents with probability over 0.95
                AllCfs++;
                {int DocPlcN=0; int Prb95Docs=0;
                while ((DocPlcN<PrbUrlStrKdV.Len())&&
                 ((DocPlcN<10)||(double(PrbUrlStrKdV[DocPlcN].Key))>0.95)){
                  double DocPrb=PrbUrlStrKdV[DocPlcN].Key;
                  TStr UrlStr=PrbUrlStrKdV[DocPlcN].Dat;
                  int DocDist=YBs->GetDocDist(RefDocId, YBs->GetDocId(UrlStr));
                  if (DocDist!=-1){
                    if (DocPrb>0.95){Prb95DistMom->Add(DocDist); Prb95Docs++;}
                  }
                  DocPlcN++;
                }
                Prb95DocsMom->Add(Prb95Docs);
                if (Prb95Docs==0){Prb95NoDocs++;}}

                int RefDocPlcN=0;
                while ((RefDocPlcN<PrbUrlStrKdV.Len())&&
                 (PrbUrlStrKdV[RefDocPlcN].Dat!=RefUrlStr)){RefDocPlcN++;}
                if (RefDocPlcN==PrbUrlStrKdV.Len()){
                  // correct document not found
                  UnknownCfs++;
                  VisitsMom->Add(PrbUrlStrKdV.Len());
                  RefDocPlcMom->Add(
                   PrbUrlStrKdV.Len()+(YBs->GetDocs()-PrbUrlStrKdV.Len())/2);
                  RefDocPrbMom->Add(0);
                  if (SaveTmp){
                    TStr Msg;
                    Msg+=TInt::GetStr(PrbUrlStrKdV.Len());
                    Msg+=TStr(" *** Not Found");
                    Msg+=TStr(' ')+TFlt::GetStr(YFSels)+'%';
                    Msg+=TStr(' ')+TFlt::GetStr(MnDocFq);
                    Msg+=TStr(" [")+TFlt::GetStr(TsWordDs->GetSumWordFq());
                    Msg+=TStr(">=")+TFlt::GetStr(MnTsWords)+']';
                    TNotify::OnNotify(Notify, ntInfo, Msg);
                    LogLx.PutVarStr("Classification", Msg);
                    LogLx.PutVarStr("RefCtgIdV", YBs->GetCtgIdVStr(RefCtgIdV));
                    LogLx.PutVarStr("BestCtgIdV", YBs->GetCtgIdVStr(BestCtgIdV));
                    LogLx.PutVarStr("IntsCtgIdV", YBs->GetCtgIdVStr(IntsCtgIdV));
                  }
                } else {
                  // correct document found
                  double RefDocPrb=PrbUrlStrKdV[RefDocPlcN].Key;
                  int MnRefDocPlcN=RefDocPlcN; int MxRefDocPlcN=RefDocPlcN;
                  while ((MnRefDocPlcN>0)&&
                   (fabs(RefDocPrb-PrbUrlStrKdV[MnRefDocPlcN-1].Key)<0.00001)){
                    MnRefDocPlcN--;}
                  while ((MxRefDocPlcN<PrbUrlStrKdV.Len()-1)&&
                   (fabs(RefDocPrb-PrbUrlStrKdV[MxRefDocPlcN+1].Key)<0.00001)){
                    MxRefDocPlcN++;}
                  int OrigRefDocPlcN=RefDocPlcN;
                  RefDocPlcN=(MxRefDocPlcN+MnRefDocPlcN)/2;
                  VisitsMom->Add(PrbUrlStrKdV.Len());
                  RefDocPlcMom->Add(RefDocPlcN+1); RefDocPrbMom->Add(RefDocPrb);

                  // wrting selected evaluations to log file
                  TChA Msg;
                  Msg+=TStr(" place:")+TInt::GetStr(RefDocPlcN+1);
                  Msg+=TStr(" [orig:")+TInt::GetStr(OrigRefDocPlcN+1)+']';
                  Msg+=TStr(" prob.:")+TFlt::GetStr(RefDocPrb);
                  Msg+=TStr(" [first:")+TFlt::GetStr(PrbUrlStrKdV[0].Key)+']';
                  Msg+=TStr(' ')+TInt::GetStr(PrbUrlStrKdV.Len());
                  Msg+=TStr(' ')+TFlt::GetStr(YFSels)+'%';
                  Msg+=TStr(' ')+TFlt::GetStr(MnDocFq);
                  if (SaveTmp){
                    TNotify::OnNotify(Notify, ntInfo, Msg);
                    LogLx.PutVarStr("Classification", Msg);
                    TChA KwPrRcStr; TChA DocPrRcStr;
                    KwPrRcStr+=TStr(" P:")+TFlt::GetStr(KwPrecis);
                    KwPrRcStr+=TStr(" R:")+TFlt::GetStr(KwRecall);
                    DocPrRcStr+=TStr(" P:")+TFlt::GetStr(DocPrecis);
                    DocPrRcStr+=TStr(" R:")+TFlt::GetStr(DocRecall);
                    LogLx.PutVarStr("KwPrRc", KwPrRcStr);
                    LogLx.PutVarStr("DocPrRc", DocPrRcStr);
                    LogLx.PutVarStr("RefCtgIdV", YBs->GetCtgIdVStr(RefCtgIdV));
                    LogLx.PutVarStr("BestCtgIdV", YBs->GetCtgIdVStr(BestCtgIdV));
                    LogLx.PutVarStr("IntsCtgIdV", YBs->GetCtgIdVStr(IntsCtgIdV));
                  }
                }

                // writing best hits to log file
                if (SaveTmp){
                  int DocPlcN=0;
                  while ((DocPlcN<PrbUrlStrKdV.Len())&&(DocPlcN<ShowTopHits)){
                    double DocPrb=PrbUrlStrKdV[DocPlcN].Key;
                    TStr UrlStr=PrbUrlStrKdV[DocPlcN].Dat;
                    int DocId=YBs->GetDocId(UrlStr);
                    int DocDist=YBs->GetDocDist(RefDocId, DocId);
                    TChA Msg;
                    Msg+=TStr("  ")+TInt::GetStr(DocPlcN+1)+'.';
                    Msg+=TStr(' ')+TFlt::GetStr(DocPrb);
                    Msg+=TStr(' ')+TInt::GetStr(DocDist);
                    Msg+=TStr(' ')+TUrl(UrlStr).GetPathStr();
                    LogLx.PutVarStr("Rank", Msg);
                    DocPlcN++;
                  }
                }
                if (SaveTmp){LogLx.PutLn();}
              } // TsWebPgN
              VisitsMom->Def(); RefDocPlcMom->Def(); RefDocPrbMom->Def();
              KwF05Mom->Def(); KwF1Mom->Def(); KwF2Mom->Def();
              KwF4Mom->Def(); KwF8Mom->Def(); KwF16Mom->Def();
              KwPrecisMom->Def(); KwRecallMom->Def();
              DocF05Mom->Def(); DocF1Mom->Def(); DocF2Mom->Def();
              DocF4Mom->Def(); DocF8Mom->Def(); DocF16Mom->Def();
              DocPrecisMom->Def(); DocRecallMom->Def();
              Prb95DistMom->Def(); Prb95DocsMom->Def();

              // writing to result file
              Lx.PutStr(YDmNm);
              if (InfoMsg.Empty()){Lx.PutStr("No");} else {Lx.PutStr(InfoMsg);}
              Lx.PutStr(TYDsBs::GetNrTypeStr(YDsBsNrType));
              Lx.PutFlt(YDsBsWordFqExp);
              Lx.PutFlt(YDsBsMnDocWordPrb);
              Lx.PutFlt(YDsBsSumAllWordPrb);
              Lx.PutStr(TYFSelBs::GetYFSelTypeStr(YFSelType));
              Lx.PutBool(YFSelPosWords);
              Lx.PutFlt(YFSels);
              Lx.PutStr(YFSelAttrEstNm);
              Lx.PutStr(TYDmDs::GetYNegDsTypeStr(YNegDsType));
              Lx.PutStr(TYDmDs::GetYPriorTypeStr(YPriorType));
              Lx.PutFlt(YInvIxEstExp);
              Lx.PutFlt(YInvIxSumEstPrb);
              Lx.PutInt(MnDocFq);
              Lx.PutInt(TsRunN); Lx.PutInt(MnTsDocN); Lx.PutInt(MxTsDocN);
              Lx.PutInt(MnTsWords); Lx.PutFlt(MnTsWordPrb);
              Lx.PutBool(ExclTs);
              Lx.PutFlt(PrRePrbTsh);
              Lx.PutFlt(TsDocs); Lx.PutFlt(BadTsDocs/double(TsDocs));
              Lx.PutFlt(AllCfs); Lx.PutFlt(UnknownCfs/double(AllCfs));
              PutMomVal(Lx, VisitsMom);
              PutMomVal(Lx, RefDocPlcMom); PutMomVal(Lx, RefDocPrbMom);
              PutMomVal(Lx, KwF05Mom); PutMomVal(Lx, KwF1Mom);
              PutMomVal(Lx, KwF2Mom); PutMomVal(Lx, KwF4Mom);
              PutMomVal(Lx, KwF8Mom); PutMomVal(Lx, KwF16Mom);
              PutMomVal(Lx, KwPrecisMom); PutMomVal(Lx, KwRecallMom);
              PutMomVal(Lx, DocF05Mom); PutMomVal(Lx, DocF1Mom);
              PutMomVal(Lx, DocF2Mom); PutMomVal(Lx, DocF4Mom);
              PutMomVal(Lx, DocF8Mom); PutMomVal(Lx, DocF16Mom);
              PutMomVal(Lx, DocPrecisMom); PutMomVal(Lx, DocRecallMom);
              PutMomVal(Lx, Prb95DistMom); PutMomVal(Lx, Prb95DocsMom);
              Lx.PutFlt((AllCfs-Prb95NoDocs)/double(AllCfs)); Lx.PutInt(Prb95NoDocs);
              PutMomVal(Lx, DocDistMom); PutMomVal(Lx, DocSectsMom);
              Lx.PutInt(TInt(YBs->GetSects())); Lx.PutInt(TInt(YBs->GetAbsSects()));
              PutMomVal(Lx, DocWordsMom); PutMomVal(Lx, DocFSelWordsMom);
              Lx.PutLn();
              SOut->Flush();
            } // PrRePrbTshN
          } // MnDocFqN
        } // SumEstPrbN
      } // AttrEstN
    }  // YFSelsN
  } // TsRunN
  TNotify::OnNotify(Notify, ntInfo, "Finished.");
}

void TYExpr::ExtrResMlj(const PNotify& Notify){
  TStrStrVH VarToMomVH(100);
  VarToMomVH.AddDat("FSels").Add("Mean");
  VarToMomVH.AddDat("MnDocFq").Add("Mean");
  VarToMomVH.AddDat("PrRcPrbTsh").Add("Mean");
  VarToMomVH.AddDat("PlcMed").Add("Mean");
  VarToMomVH.AddDat("PlcMed").Add("SDev");
  VarToMomVH.AddDat("PlcMed").Add("SErr");
  VarToMomVH.AddDat("PrbMed").Add("Mean");
  VarToMomVH.AddDat("PrbMed").Add("SDev");
  VarToMomVH.AddDat("PrbMed").Add("SErr");
  VarToMomVH.AddDat("KwPrecisMed").Add("Mean");
  VarToMomVH.AddDat("KwPrecisMed").Add("SDev");
  VarToMomVH.AddDat("KwPrecisMed").Add("SErr");
  VarToMomVH.AddDat("KwRecallMed").Add("Mean");
  VarToMomVH.AddDat("KwRecallMed").Add("SDev");
  VarToMomVH.AddDat("KwRecallMed").Add("SErr");
  VarToMomVH.AddDat("DocPrecisMed").Add("Mean");
  VarToMomVH.AddDat("DocPrecisMed").Add("SDev");
  VarToMomVH.AddDat("DocPrecisMed").Add("SErr");
  VarToMomVH.AddDat("DocRecallMed").Add("Mean");
  VarToMomVH.AddDat("DocRecallMed").Add("SDev");
  VarToMomVH.AddDat("DocRecallMed").Add("SErr");
  VarToMomVH.AddDat("KwF2").Add("Mean");
  VarToMomVH.AddDat("KwF2").Add("SDev");
  VarToMomVH.AddDat("KwF2").Add("SErr");
  VarToMomVH.AddDat("DocF2").Add("Mean");
  VarToMomVH.AddDat("DocF2").Add("SDev");
  VarToMomVH.AddDat("DocF2").Add("SErr");

  TStrV FPathV; FPathV.Add("../results");
  TStrV FExtV; FExtV.Add("st");
  TFFile FFile(FPathV, FExtV); TStr FNm;
  while (FFile.Next(FNm)){
//    if (FNm!="../results/CompOddsRatioHO.st"){continue;}
    PSIn SIn=new TFIn(FNm);
    TILx ILx(SIn, TFSet()|iloRetEoln|iloSigNum|iloUniStr);
    TNotify::OnNotify(Notify, ntInfo, TStr("Processing ")+FNm);
    TStrV VarNmV;
    TVec<TVec<TStrV> > VarValVVV;
    ILx.GetSym();
    while (ILx.Sym!=syEof){
      TStrV NmV;
      IAssert((ILx.Sym==syStr)&&(ILx.Str=="DmNm"));
      while (ILx.Sym!=syEoln){NmV.Add(ILx.Str); ILx.GetSym();}
      if (VarNmV.Empty()){VarNmV=NmV;} else {IAssert(VarNmV==NmV);}
      TVec<TStrV> ValVV;
      ILx.GetSym();
      while ((ILx.Sym!=syEof)&&((ILx.Sym!=syStr)||(ILx.Str!="DmNm"))){
        TStrV ValV(VarNmV.Len(), 0);
        while (ILx.Sym!=syEoln){ValV.Add(ILx.Str); ILx.GetSym();}
        IAssert(VarNmV.Len()==ValV.Len());
        ValVV.Add(ValV);
        ILx.GetSym();
      }
      VarValVVV.Add(ValVV);
      TNotify::OnNotify(Notify, ntInfo,
       TStr("... lines ")+TInt::GetStr(ValVV.Len()));
    }

    int InvIxMnDocFqVarN=VarNmV.SearchForw("MnDocFq");
    int PrRcPrbTshVarN=VarNmV.SearchForw("PrRcPrbTsh");
    THash<TStr, TVec<PMom> > VarNmToValMomVH(100);
    {int VarToMomVP=VarToMomVH.FFirstKeyId();
    while (VarToMomVH.FNextKeyId(VarToMomVP)){
      TStr VarNm=VarToMomVH.GetKey(VarToMomVP);
      int VarN=VarNmV.SearchForw(VarNm);
      int Vals=VarValVVV[0].Len();
      for (int ValN=0; ValN<Vals; ValN++){
        double InvIxMnDocFq=VarValVVV[0][ValN][InvIxMnDocFqVarN].GetFlt();
        double PrRcPrbTsh=VarValVVV[0][ValN][PrRcPrbTshVarN].GetFlt();
        if ((fabs(InvIxMnDocFq-3)>0.0001)||(fabs(PrRcPrbTsh-0.95)>0.0001)){
          VarNmToValMomVH.AddDat(VarNm).Add(NULL); continue;}
        PMom ValMom=PMom(new TMom());
        for (int ValSetN=0; ValSetN<VarValVVV.Len(); ValSetN++){
          TStr ValStr=VarValVVV[ValSetN][ValN][VarN];
          double Val=ValStr.GetFlt();
          ValMom->Add(Val);
        }
        ValMom->Def();
        VarNmToValMomVH.AddDat(VarNm).Add(ValMom);
      }
    }}

    PSOut SOut=PSOut(new TFOut(FNm+"t"));
    TOLx OLx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
    {int VarToMomVP=VarToMomVH.FFirstKeyId();
    while (VarToMomVH.FNextKeyId(VarToMomVP)){
      TStr VarNm=VarToMomVH.GetKey(VarToMomVP);
      TStrV& MomNmV=VarToMomVH[VarToMomVP];
      for (int MomNmN=0; MomNmN<MomNmV.Len(); MomNmN++){
        OLx.PutUQStr(VarNm+"_"+MomNmV[MomNmN]);
      }
    }}
    OLx.PutLn();
    int Vals=VarValVVV[0].Len();
    for (int ValN=0; ValN<Vals; ValN++){
      double InvIxMnDocFq=VarValVVV[0][ValN][InvIxMnDocFqVarN].GetFlt();
      double PrRcPrbTsh=VarValVVV[0][ValN][PrRcPrbTshVarN].GetFlt();
      if ((fabs(InvIxMnDocFq-3)>0.0001)||(fabs(PrRcPrbTsh-0.95)>0.0001)){continue;}
      {int VarToMomVP=VarToMomVH.FFirstKeyId();
      while (VarToMomVH.FNextKeyId(VarToMomVP)){
        TStr VarNm=VarToMomVH.GetKey(VarToMomVP);
        TStrV& MomNmV=VarToMomVH[VarToMomVP];
        for (int MomNmN=0; MomNmN<MomNmV.Len(); MomNmN++){
          PMom Mom=VarNmToValMomVH.GetDat(VarNm)[ValN];
          double Val=Mom->GetByNm(MomNmV[MomNmN]);
          OLx.PutFlt(Val);
        }
      }}
      OLx.PutLn();
    }
  }
  TNotify::OnNotify(Notify, ntInfo, "Finished.");
}

void TYExpr::ExtrResKdd(const PNotify& Notify){
  TStr FNm="../resultsKdd/resultAllA.st";
//  TStr FNm="../resultskdd/resultent.st";
  PSIn SIn=new TFIn(FNm);
  TILx ILx(SIn, TFSet()|iloRetEoln|iloSigNum|iloUniStr);
  TNotify::OnNotify(Notify, ntInfo, TStr("Processing ")+FNm);
  TStrV NmV;
  TVec<TFltV> ValVV;
  ILx.GetSym(syStr, syEoln);
  IAssert((ILx.Sym==syStr)&&(ILx.Str=="DmNm"));
  while (ILx.Sym!=syEoln){NmV.Add(ILx.Str); ILx.GetSym(syStr, syEoln);}
  ILx.GetSym(syStr, syFlt, syEof);
  while (ILx.Sym!=syEof){
    TFltV ValV(NmV.Len(), 0);
    while (ILx.Sym!=syEoln){
      if (ILx.Sym==syFlt){ValV.Add(ILx.Flt);}
      else if (ILx.Sym==syStr){ValV.Add(TStr(ILx.Str).GetPrimHashCd());}
      else {Fail;}
      ILx.GetSym(syStr, syFlt, syEoln);
    }
    IAssert(NmV.Len()==ValV.Len());
    ValVV.Add(ValV);
    ILx.GetSym(syStr, syFlt, syEof);
  }
  TNotify::OnNotify(Notify, ntInfo,
   TStr("... lines ")+TInt::GetStr(ValVV.Len()));

  TFltV FSelsV;
  FSelsV.Add(0.25); FSelsV.Add(0.50); FSelsV.Add(0.75); FSelsV.Add(1.0);
  FSelsV.Add(1.5); FSelsV.Add(2.0); FSelsV.Add(3.0); FSelsV.Add(5.0);
  FSelsV.Add(7.0); FSelsV.Add(9.0);
  TIntV MnDocFqV;
  MnDocFqV.Add(TInt(1)); MnDocFqV.Add(TInt(2)); MnDocFqV.Add(TInt(3)); 
  MnDocFqV.Add(TInt(4)); MnDocFqV.Add(TInt(5)); MnDocFqV.Add(TInt(6));
  MnDocFqV.Add(TInt(7)); MnDocFqV.Add(TInt(8)); MnDocFqV.Add(TInt(9));
  MnDocFqV.Add(TInt(10));
  TFltV SumEstPrbV;
  SumEstPrbV.Add(1.00); SumEstPrbV.Add(0.90); SumEstPrbV.Add(0.80);
  SumEstPrbV.Add(0.70); SumEstPrbV.Add(0.60); SumEstPrbV.Add(0.50);
  SumEstPrbV.Add(0.40);
  int DmNmVarN=NmV.SearchForw("DmNm");
  int FSelsVarN=NmV.SearchForw("FSels");
  int SumEstPrbVarN=NmV.SearchForw("SumEstPrb");
  int MnDocFqVarN=NmV.SearchForw("MnDocFq");
  int VisitsMeanVarN=NmV.SearchForw("VisitsMean");
  int VisitsSErrVarN=NmV.SearchForw("VisitsErr");
  int PlcMedVarN=NmV.SearchForw("PlcMed");
  int KwPrecisMeanVarN=NmV.SearchForw("KwPrecisMean");
  int KwRecallMeanVarN=NmV.SearchForw("KwRecallMean");
  int KwF2VarN=NmV.SearchForw("KwF2");

  for (int SumEstPrbN=0; SumEstPrbN<SumEstPrbV.Len(); SumEstPrbN++){
    TStr OutFNm="../resultskdd/tt/EntSMF1_"+TInt::GetStr(SumEstPrbN)+".st";
    TNotify::OnNotify(Notify, ntInfo, TStr("... writing ")+OutFNm);
    PSOut SOut=PSOut(new TFOut(OutFNm));
    TOLx OLx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
    OLx.PutUQStr("FSels");
    OLx.PutUQStr("SumEstPrb");
    OLx.PutUQStr("MnDocFq");
    OLx.PutUQStr("VisitsMean");
    OLx.PutUQStr("VisitsSErr");
    OLx.PutUQStr("PlcMed");
    OLx.PutUQStr("KwPrecisMean");
    OLx.PutUQStr("KwRecallMean");
    OLx.PutUQStr("KwF2");
    OLx.PutLn();
    double SumEstPrb=SumEstPrbV[SumEstPrbN];
    for (int MnDocFqN=0; MnDocFqN<MnDocFqV.Len(); MnDocFqN++){
      double MnDocFq=MnDocFqV[MnDocFqN];
      for (int FSelsN=0; FSelsN<FSelsV.Len(); FSelsN++){
        double FSels=FSelsV[FSelsN];
        if (!TFlt::Eq6(FSels, 1.0)){continue;}
        for (int ValN=0; ValN<ValVV.Len(); ValN++){
          int DmNmHashCd=int(ValVV[ValN][DmNmVarN]);
          if (DmNmHashCd!=TStr("Ent").GetPrimHashCd()){continue;}
          double FSelsVal=ValVV[ValN][FSelsVarN];
          double SumEstPrbVal=ValVV[ValN][SumEstPrbVarN];
          double MnDocFqVal=ValVV[ValN][MnDocFqVarN];
          double VisitsMeanVal=ValVV[ValN][VisitsMeanVarN];
          double VisitsSErrVal=ValVV[ValN][VisitsSErrVarN];
          double PlcMedVal=ValVV[ValN][PlcMedVarN];
          double KwPrecisMeanVal=ValVV[ValN][KwPrecisMeanVarN];
          double KwRecallMeanVal=ValVV[ValN][KwRecallMeanVarN];
          double KwF2Val=ValVV[ValN][KwF2VarN];
          if ((TFlt::Eq6(FSels, FSelsVal))&&
           (TFlt::Eq6(SumEstPrb, SumEstPrbVal))&&
           (TFlt::Eq6(MnDocFq, MnDocFqVal))){
            OLx.PutFlt(FSelsVal);
            OLx.PutFlt(SumEstPrbVal);
            OLx.PutFlt(MnDocFqVal);
            OLx.PutFlt(VisitsMeanVal);
            OLx.PutFlt(VisitsSErrVal);
            OLx.PutFlt(PlcMedVal);
            OLx.PutFlt(KwPrecisMeanVal);
            OLx.PutFlt(KwRecallMeanVal);
            OLx.PutFlt(KwF2Val);
            OLx.PutLn();
          }
        }
      }
    }
  }

  {for (int SumEstPrbN=0; SumEstPrbN<SumEstPrbV.Len(); SumEstPrbN++){
    TStr OutFNm="../resultskdd/tt/EntSFM9_"+TInt::GetStr(SumEstPrbN)+".st";
    TNotify::OnNotify(Notify, ntInfo, TStr("... writing ")+OutFNm);
    PSOut SOut=PSOut(new TFOut(OutFNm));
    TOLx OLx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
    OLx.PutUQStr("FSels");
    OLx.PutUQStr("SumEstPrb");
    OLx.PutUQStr("MnDocFq");
    OLx.PutUQStr("VisitsMean");
    OLx.PutUQStr("VisitsSErr");
    OLx.PutUQStr("PlcMed");
    OLx.PutUQStr("KwF2");
    OLx.PutUQStr("KwPrecisMean");
    OLx.PutUQStr("KwRecallMean");
    OLx.PutLn();
    double SumEstPrb=SumEstPrbV[SumEstPrbN];
    for (int MnDocFqN=0; MnDocFqN<MnDocFqV.Len(); MnDocFqN++){
      double MnDocFq=MnDocFqV[MnDocFqN];
      if (!TFlt::Eq6(MnDocFq, 9.0)){continue;}
      for (int FSelsN=0; FSelsN<FSelsV.Len(); FSelsN++){
        double FSels=FSelsV[FSelsN];
        for (int ValN=0; ValN<ValVV.Len(); ValN++){
          int DmNmHashCd=int(ValVV[ValN][DmNmVarN]);
          if (DmNmHashCd!=TStr("Ent").GetPrimHashCd()){continue;}
          double FSelsVal=ValVV[ValN][FSelsVarN];
          double SumEstPrbVal=ValVV[ValN][SumEstPrbVarN];
          double MnDocFqVal=ValVV[ValN][MnDocFqVarN];
          double VisitsMeanVal=ValVV[ValN][VisitsMeanVarN];
          double VisitsSErrVal=ValVV[ValN][VisitsSErrVarN];
          double PlcMedVal=ValVV[ValN][PlcMedVarN];
          double KwPrecisMeanVal=ValVV[ValN][KwPrecisMeanVarN];
          double KwRecallMeanVal=ValVV[ValN][KwRecallMeanVarN];
          double KwF2Val=ValVV[ValN][KwF2VarN];
          if ((TFlt::Eq6(FSels, FSelsVal))&&
           (TFlt::Eq6(SumEstPrb, SumEstPrbVal))&&
           (TFlt::Eq6(MnDocFq, MnDocFqVal))){
            OLx.PutFlt(FSelsVal);
            OLx.PutFlt(SumEstPrbVal);
            OLx.PutFlt(MnDocFqVal);
            OLx.PutFlt(VisitsMeanVal);
            OLx.PutFlt(VisitsSErrVal);
            OLx.PutFlt(PlcMedVal);
            OLx.PutFlt(KwPrecisMeanVal);
            OLx.PutFlt(KwRecallMeanVal);
            OLx.PutFlt(KwF2Val);
            OLx.PutLn();
          }
        }
      }
    }
  }}
  TNotify::OnNotify(Notify, ntInfo, "Finished.");
}

PMd TYExpr::GenMd(
 const TStr& YDmNm, const TStr& YRootFPath, const bool& DoSave,
 const int& MxNGram, const int& MnWordFq, const TSwSetTy& SwSetTy,
 const TYDsBsNrType& YDsBsNrType, const double& YDsBsWordFqExp,
 const double& YDsBsMnDocWordPrb, const double& YDsBsSumAllWordPrb,
 const TYFSelType& YFSelType, const double& YFSels,
 const bool& YFSelPosWords, const PAttrEst& YFSelAttrEst,
 const TYNegDsType& YNegDsType, const TYPriorType& YPriorType,
 const double& YInvIxEstExp, const double& YInvIxSumEstPrb,
 const PNotify& Notify){
  TStr NrYRootFPath=TStr::GetNrFPath(YRootFPath);
  // opening of web-base
  PXWebBs WebBs=PXWebBs(new TWebMemBs(NrYRootFPath+YWebBsFPfx+YDmNm));
  // feature construction
  PYBs YBs=PYBs(new TYBs(
   WebBs, MxNGram, MnWordFq, SwSetTy, TStrV(), Notify));
  // subproblem definition
  PYDsBs YDsBs=PYDsBs(new TYDsBs(
   YDsBsNrType, YDsBsWordFqExp, YDsBsMnDocWordPrb, YDsBsSumAllWordPrb,
   YBs, Notify));
  // feature selection
  PYFSelBs YFSelBs=PYFSelBs(new TYFSelBs(
   YFSelType, YFSels, YFSelPosWords, YFSelAttrEst,
   YNegDsType, YPriorType, YBs, YDsBs, Notify));
  // inverted index creation
  PYInvIx YInvIx=PYInvIx(new TYInvIx(
   YInvIxEstExp, YInvIxSumEstPrb,
   YBs, YDsBs, YFSelBs, Notify));
  // model generation
  PMd Md=PMd(new TMdYBayes(
   YNegDsType, YPriorType, YBs, YDsBs, YFSelBs, YInvIx));
  // saving files
  if (DoSave){
    // intermediate file
    YBs->Save(NrYRootFPath+YBsFPfx+YDmNm+DatFExt);
    YBs->SaveTxt(NrYRootFPath+YBsFPfx+YDmNm+TxtFExt);
    YBs->SaveTxtWords(NrYRootFPath+YWordFPfx+YDmNm+TxtFExt);
    YBs->SaveTxtCtgs(NrYRootFPath+YCtgFPfx+YDmNm+TxtFExt);
    // intermediate file
    TStr YDsBsFNm=NrYRootFPath+YDsBsFPfx+YDmNm+TxtFExt;
    TStr YFSelBsFNm=NrYRootFPath+YFSelFPfx+YDmNm+TxtFExt;
    TStr YInvIxFNm=NrYRootFPath+YInvIxFPfx+YDmNm+TxtFExt;
    YDsBs->SaveTxt(PSOut(new TFOut(YDsBsFNm)), YBs);
    YFSelBs->SaveTxt(PSOut(new TFOut(YFSelBsFNm)), YBs, YDsBs);
    YInvIx->SaveTxt(PSOut(new TFOut(YInvIxFNm)), YBs);
    // model file
    TStr MdFNm=NrYRootFPath+YMdFPfx+YDmNm+DatFExt;
    TStr CheckMdFNm=NrYRootFPath+YMdFPfx+YDmNm+TmpFExt;
    TNotify::OnNotify(Notify, ntInfo, TStr("Saving to ")+MdFNm);
    {TFOut FOut(MdFNm); Md->Save(FOut);}
//    PMd CheckMd;
//    {TFIn CheckMdFIn(MdFNm); CheckMd=TMdYBayes::Load(CheckMdFIn);}
//    {TFOut CheckMdFOut(CheckMdFNm); CheckMd->Save(CheckMdFOut);}
  }
  TNotify::OnNotify(Notify, ntInfo, "Finished.");
  return Md;
}

void TYExpr::UseMd(
 const TStr& YDmNm, const TStr& YRootFPath,
 const PSIn& SIn, const PSOut& SOut,
 const double& MnDocPrbTsh, const int& MxTopHits,
 const double& MnTsWordPrb, const int& MnDocFq,
 const PNotify&){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  TStr YMdFNm=
   TStr::GetNrFPath(YRootFPath)+TYExpr::YMdFPfx+YDmNm+TYExpr::DatFExt;
  TFIn YMdFIn(YMdFNm);
  PMd Md=TMdYBayes::Load(YMdFIn);
  TMdYBayes& YMd=*(TMdYBayes*)Md();
  PYBs YBs=YMd.GetYBs();
  PYInvIx YInvIx=YMd.GetYInvIx();
  PYWordDs TsWordDs=YBs->GetWordDs(SIn);
  Lx.PutUQStr("------------------------------------------"); Lx.PutLn();
  Lx.PutUQStr("Input Text Distribution"); Lx.PutLn();
  TsWordDs->SaveTxt(SOut, &(*YBs));

  PValRet ValRet=
   PValRet(new TYValRet(MnTsWordPrb, YMd.GetDmHd(), YBs, TsWordDs));

  TIntV ClassNV; YInvIx->GetDocIdV(TsWordDs, MnDocFq, ClassNV);
  TDocCfV DocCfV(ClassNV.Len(), 0);
  for (int ClassNN=0; ClassNN<ClassNV.Len(); ClassNN++){
    int ClassN=ClassNV[ClassNN];
    TStr UrlStr=Md->GetDmHd()->GetClassNm(ClassN);
    TFltIntKdV WordPrbIdV;
    PTbValDs ValDs=YMd.GetPostrValDs(ValRet, ClassN, WordPrbIdV);
    double ClassPrb=ValDs->GetPrb_RelFq(TTbVal(true));
    DocCfV.Add(TDocCf(UrlStr, ClassPrb, WordPrbIdV));
  }
  DocCfV.Sort(false);

  Lx.PutUQStr("------------------------------------------"); Lx.PutLn();
  Lx.PutLn(); Lx.PutUQStr("Best Categories"); Lx.PutLn();
  {int DocCfN=0; int TbDocCfN=0;
  while ((DocCfN<DocCfV.Len())&&(DocCfV[DocCfN].DocPrb>MnDocPrbTsh)&&
   (DocCfN<MxTopHits)){
    double DocPrb=DocCfV[DocCfN].DocPrb;
    TStr UrlStr=DocCfV[DocCfN].UrlStr;
    TStr RankStr=TInt::GetStr(TbDocCfN+1)+'.';
    TStr DocPrbStr=TFlt::GetStr(DocPrb, 4, 2);
    TStr DocPathStr=TUrl(UrlStr).GetPathStr();
    TStrV WordStrV;
    TFltIntKdV& WordPrbIdV=DocCfV[DocCfN].WordPrbIdV;
    for (int WordPrbIdN=0; WordPrbIdN<WordPrbIdV.Len(); WordPrbIdN++){
      double WordPrb=WordPrbIdV[WordPrbIdN].Key;
      int WordId=WordPrbIdV[WordPrbIdN].Dat;
      TStr WordStr=YBs->GetWordStr(WordId);
      TStr WordStrPrbStr=WordStr+" ["+TFlt::GetStr(WordPrb, 0, 4)+"]";
      WordStrV.Add(WordStrPrbStr);
    }
    int Slashes=0;
    for (int ChN=0; ChN<DocPathStr.Len(); ChN++){
      if (DocPathStr[ChN]=='/'){Slashes++;}
    }
    if (Slashes>2){
      Lx.PutVarStr("Category", RankStr+" "+DocPrbStr+" "+DocPathStr);
      Lx.PutVarStrV("Words", WordStrV);
      TbDocCfN++;
    }
    DocCfN++;
  }}

  {TIntV CtgIdV;
  TIntFltH CtgIdToWFqH(1000);
  int DocCfN=0;
  while ((DocCfN<DocCfV.Len())&&(DocCfV[DocCfN].DocPrb>MnDocPrbTsh)){
    double DocPrb=DocCfV[DocCfN].DocPrb;
    TStr UrlStr=DocCfV[DocCfN].UrlStr;
    int DocId=YBs->GetDocId(UrlStr);
    TIntV DocCtgIdV; YBs->GetCtgIdV(DocId, DocCtgIdV);
    CtgIdV.AddVMerged(DocCtgIdV);
    for (int DocCtgIdN=0; DocCtgIdN<DocCtgIdV.Len(); DocCtgIdN++){
      int CtgId=DocCtgIdV[DocCtgIdN];
      double WFq=DocPrb/log(1+YBs->GetCtgFq(CtgId));
      CtgIdToWFqH.AddDat(CtgId)+=WFq;
    }
    DocCfN++;
  }
  TStr CtgIdVStr=YBs->GetCtgIdVStr(CtgIdV);
  TStrV CtgStrV; YBs->GetCtgStrV(CtgIdV, CtgStrV);
  TStr CtgIdToWFqHStr=YBs->GetCtgIdToWFqHStr(CtgIdToWFqH, 0.9);
  Lx.PutUQStr("------------------------------------------"); Lx.PutLn();
  Lx.PutLn();Lx.PutUQStr("Best Keywords"); Lx.PutLn();
  Lx.PutVarStr("CtgIdV", CtgIdVStr);
//  Lx.PutVarStr("CtgIdToWFqHStr", CtgIdToWFqHStr);
  Lx.PutVarStrV("CtgStrV", CtgStrV);}
}

void TYExpr::Extr(const TStr& FNm, const PSOut& SOut){
  TStrV VarNmV;
  VarNmV.Add("DmNm"); VarNmV.Add("KNNbrs"); VarNmV.Add("RndSel");
  VarNmV.Add("HldPrecisMean"); VarNmV.Add("HldRecallMean");
  VarNmV.Add("HldF2Mean");
  VarNmV.Add("HlLenMed"); VarNmV.Add("DocLenMed");

  PSs Ss=TSs::LoadTxt(FNm);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
  int VarNmN=0;
  for (int Y=0; Y<Ss->GetYLen(); Y++){
    for (int X=0; X<Ss->GetXLen(Y); X++){
      if (Ss->GetVal(X, Y)==VarNmV[VarNmN]){
        TStr VarNm=VarNmV[VarNmN];
        TStr ValStr;
        if (Ss->GetVal(X, Y+1).IsFlt()){
          PMom Mom=PMom(new TMom()); int RelY=1;
          while (Ss->GetVal(X, Y+RelY).IsFlt()){
            double Flt=Ss->GetVal(X, Y+RelY).GetFlt();
            Mom->Add(Flt);
            RelY++;
          }
          Mom->Def();
          if (Mom->GetWgt()!=5){
            double Wgt=Mom->GetWgt();
            printf("%g", Wgt);
            //IAssert(Mom->GetWgt()==5);
          }
          ValStr=TFlt::GetStr(Mom->GetMean())+"/"+TFlt::GetStr(Mom->GetSErr());
        } else {
          ValStr=Ss->GetVal(X, Y+1);
        }
        Lx.PutUQStr(VarNm);
        Lx.PutUQStr(ValStr);
        VarNmN=(VarNmN+1)%VarNmV.Len();
        if (VarNmN==0){Lx.PutLn();}
      }
    }
  }
}
