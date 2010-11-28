/////////////////////////////////////////////////
// Includes
#include "wordnet.h"

/////////////////////////////////////////////////
// WordNet-SynSet
void TWnSynSet::GetDstSynSetPV(
 const TWnRelType& RelType, TIntV& DstSynSetPV) const {
  DstSynSetPV.Clr();
  for (int RelN=0; RelN<RelIntIntTrV.Len(); RelN++){
    TWnRelType CurRelType=TWnRelType(RelIntIntTrV[RelN].Val1.Val);
    if (RelType==CurRelType){
      int DstSynSetP=RelIntIntTrV[RelN].Val3;
      DstSynSetPV.Add(DstSynSetP);
    }
  }
}

/////////////////////////////////////////////////
// WordNet-Base
TWnSynSetType TWnBs::GetSynSetTypeFromStr(const TStr& SynSetTypeStr){
  if (SynSetTypeStr=="n"){return wsstNoun;}
  if (SynSetTypeStr=="v"){return wsstVerb;}
  if (SynSetTypeStr=="a"){return wsstAdjective;}
  if (SynSetTypeStr=="s"){return wsstAdjectiveSatellite;}
  if (SynSetTypeStr=="r"){return wsstAdverb;}
  Fail; return wsstUndef;
}

void TWnBs::AddRel(const TWnRelType& RelType, const TStr& SrcSynSetId, const TStr& DstSynSetId){
  IAssert(IsSynSetId(SrcSynSetId)); IAssert(IsSynSetId(DstSynSetId));
  int SrcSynSetP=GetSynSetP(SrcSynSetId);
  int DstSynSetP=GetSynSetP(DstSynSetId);
  PWnSynSet SrcSynSet=GetSynSet(SrcSynSetId);
  PWnSynSet DstSynSet=GetSynSet(DstSynSetId);
  SrcSynSet->AddRel(RelType, SrcSynSetP, DstSynSetP);
  DstSynSet->AddInvRel(RelType, SrcSynSetP, DstSynSetP);
}

TStr TWnBs::GetRelTypeNm(const TWnRelType& RelType){
  switch (RelType){
    case wrtUndef: return "Undef";
    case wrtSenseKey: return "SenseKey";
    case wrtGloss: return "Gloss";
    case wrtSyntax: return "Syntax";
    case wrtHypernym: return "Hypernym";
    case wrtInstance: return "Instance";
    case wrtEntailment: return "Entailment";
    case wrtSimilar: return "Similar";
    case wrtMemberMeronym: return "MemberMeronym";
    case wrtSubstanceMeronym: return "SubstanceMeronym";
    case wrtPartMeronym: return "PartMeronym";
    case wrtDerivation: return "Derivation";
    case wrtClass: return "Class";
    case wrtCause: return "Cause";
    case wrtVerbGroup: return "VerbGroup";
    case wrtAttribute: return "Attribute";
    case wrtAntonym: return "Antonym";
    case wrtAdditionalInfo: return "AdditionalInfo";
    case wrtParticiple: return "Participle";
    case wrtPosRel: return "PosRel";
    case wrtFrame: return "Frame";
    default: Fail; return "";
  }
}

void TWnBs::LoadFromPlBs(const PPlBs& PlBs){
  printf("Loading from Prolog-Base ...\n");
  // collect relation-ids
  int S6_RelId=PlBs->GetRelId("s", 6);
  int S4_RelId=PlBs->GetRelId("s", 4);

  // collect max. words per synset and max. synsets per extended-word
  printf("  ... collect counts ...\n");
  TStrIntSH SynSetIdToMxWordNSH;
  TStrIntSH WordStrToMxSynSetNSH;
  for (int RelId=0; RelId<PlBs->GetRels(); RelId++){
    if ((RelId!=S6_RelId)&&(RelId!=S4_RelId)){continue;}
    for (int TupN=0; TupN<PlBs->GetRelTups(RelId); TupN++){
      // get tuple
      int TupId=PlBs->GetRelTupId(RelId, TupN);
      PPlTup Tup=PlBs->GetTup(TupId);
      // get components
      TStr SynSetId=TInt::GetStr(Tup->GetVal(0).GetInt());
      int SynSetWordN=Tup->GetVal(1).GetInt();
      TStr WordStr=PlBs->GetAtomStr(Tup->GetVal(2).GetAtomId());
      TStr SynSetTypeStr=PlBs->GetAtomStr(Tup->GetVal(3).GetAtomId());
      int WordSynSetN=(Tup->GetVals()>4) ? Tup->GetVal(4).GetInt() : 1;
      TStr EWordStr=GetEWordStr(WordStr, SynSetTypeStr);
      // update numbers
      TInt& MxSynSetWordN=SynSetIdToMxWordNSH.AddDat(SynSetId);
      if (SynSetWordN>MxSynSetWordN){MxSynSetWordN=SynSetWordN;}
      TInt& MxWordSynSetN=WordStrToMxSynSetNSH.AddDat(EWordStr);
      if (WordSynSetN>MxWordSynSetN){MxWordSynSetN=WordSynSetN;}
    }
  }

  // create synsets
  printf("  ... create synsets ...\n");
  for (int RelId=0; RelId<PlBs->GetRels(); RelId++){
    if ((RelId!=S6_RelId)&&(RelId!=S4_RelId)){continue;}
    for (int TupN=0; TupN<PlBs->GetRelTups(RelId); TupN++){
      // get tuple
      int TupId=PlBs->GetRelTupId(RelId, TupN);
      PPlTup Tup=PlBs->GetTup(TupId);
      // get components
      TStr SynSetId=TInt::GetStr(Tup->GetVal(0).GetInt());
      int SynSetWordN=Tup->GetVal(1).GetInt();
      TStr WordStr=PlBs->GetAtomStr(Tup->GetVal(2).GetAtomId());
      TStr SynSetTypeStr=PlBs->GetAtomStr(Tup->GetVal(3).GetAtomId());
      TWnSynSetType SynSetType=GetSynSetTypeFromStr(SynSetTypeStr);
      int WordSynSetN=(Tup->GetVals()>4) ? Tup->GetVal(4).GetInt() : 1;
      TStr EWordStr=GetEWordStr(WordStr, SynSetTypeStr);
      // create synsetid to synset hash
      int SynSetP=-1;
      if (IdToWnSynSetSH.IsKey(SynSetId)){
        SynSetP=IdToWnSynSetSH.GetKeyId(SynSetId);
      } else {
        SynSetP=IdToWnSynSetSH.AddKey(SynSetId);
        IdToWnSynSetSH[SynSetP]=TWnSynSet::New();
        IdToWnSynSetSH[SynSetP]->SynSetType=SynSetType;
        int MxSynSetWordN=SynSetIdToMxWordNSH.GetDat(SynSetId);
        IdToWnSynSetSH[SynSetP]->WordStrPV.Gen(MxSynSetWordN);
        IdToWnSynSetSH[SynSetP]->WordStrPV.PutAll(-1);
      }
      // create word-string to synset hash
      int WordStrP=-1;
      if (EWordStrToSynSetPVSH.IsKey(EWordStr)){
        WordStrP=EWordStrToSynSetPVSH.GetKeyId(EWordStr);
      } else {
        WordStrP=EWordStrToSynSetPVSH.AddKey(EWordStr);
        int MxWordSynSetN=WordStrToMxSynSetNSH.GetDat(EWordStr);
        EWordStrToSynSetPVSH[WordStrP].Gen(MxWordSynSetN);
        EWordStrToSynSetPVSH[WordStrP].PutAll(-1);
      }
      // update synset index
      if (IdToWnSynSetSH[SynSetP]->WordStrPV[SynSetWordN-1]==-1){
        IdToWnSynSetSH[SynSetP]->WordStrPV[SynSetWordN-1]=WordStrP;
      } else {
        // bug in wordnet-prolog - some lines of s() relation are duplicated
        int TestWordP=IdToWnSynSetSH[SynSetP]->WordStrPV[SynSetWordN-1];
        TStr TestEWordStr=EWordStrToSynSetPVSH.GetKey(TestWordP);
        IAssert(EWordStr==TestEWordStr);
        //printf("{%s,%s} ", WordStr.CStr(), SynSetTypeStr.CStr());
      }
      // update word index
      if (EWordStrToSynSetPVSH[WordStrP][WordSynSetN-1]==-1){
        EWordStrToSynSetPVSH[WordStrP][WordSynSetN-1]=SynSetP;
      } else {
        // bug in wordnet-prolog - some lines of s() relation are duplicated
        int TestSynSetP=EWordStrToSynSetPVSH[WordStrP][WordSynSetN-1];
        TStr TestSynSetId=IdToWnSynSetSH.GetKey(TestSynSetP);
        IAssert(SynSetId==TestSynSetId);
        //printf("[%s,%s] ", WordStr.CStr(), SynSetTypeStr.CStr());
      }
    }
  }

  // relations
  printf("  ... create relations ...\n");
  int Sk_RelId=PlBs->GetRelId("sk", 3);
  int G_RelId=PlBs->GetRelId("g", 2);
  int Syntax_RelId=PlBs->GetRelId("syntax", 3);
  int Hyp_RelId=PlBs->GetRelId("hyp", 2);
  int Ins_RelId=PlBs->GetRelId("ins", 2);
  int Ent_RelId=PlBs->GetRelId("ent", 2);
  int Sim_RelId=PlBs->GetRelId("sim", 2);
  int Mm_RelId=PlBs->GetRelId("mm", 2);
  int Ms_RelId=PlBs->GetRelId("ms", 2);
  int Mp_RelId=PlBs->GetRelId("mp", 2);
  int Der_RelId=PlBs->GetRelId("der", 4);
  int Cls_RelId=PlBs->GetRelId("cls", 5);
  int Cs_RelId=PlBs->GetRelId("cs", 2);
  int Vgp_RelId=PlBs->GetRelId("vgp", 4);
  int At_RelId=PlBs->GetRelId("at", 2);
  int Ant_RelId=PlBs->GetRelId("ant", 4);
  int Sa_RelId=PlBs->GetRelId("sa", 4);
  int Ppl_RelId=PlBs->GetRelId("ppl", 4);
  int Per_RelId=PlBs->GetRelId("per", 4);
  int Fr_RelId=PlBs->GetRelId("fr", 3);

  for (int RelId=0; RelId<PlBs->GetRels(); RelId++){
    // get relation-type, source-synset-arg, destination-synset-arg
    TWnRelType RelType=wrtUndef; int SrcSynSetIdArgN=-1; int DstSynSetIdArgN=-1;
    if (RelId==S6_RelId){RelType=wrtUndef;}
    else if (RelId==S4_RelId){RelType=wrtUndef;}
    else if (RelId==Sk_RelId){RelType=wrtUndef;}
    else if (RelId==G_RelId){RelType=wrtUndef;}
    else if (RelId==Syntax_RelId){RelType=wrtUndef;}
    else if (RelId==Hyp_RelId){RelType=wrtHypernym; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Ins_RelId){RelType=wrtInstance; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Ent_RelId){RelType=wrtEntailment; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Sim_RelId){RelType=wrtSimilar; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Mm_RelId){RelType=wrtMemberMeronym; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Ms_RelId){RelType=wrtSubstanceMeronym; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Mp_RelId){RelType=wrtPartMeronym; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Der_RelId){RelType=wrtDerivation; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==Cls_RelId){RelType=wrtClass; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==Cs_RelId){RelType=wrtCause; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Vgp_RelId){RelType=wrtVerbGroup; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==At_RelId){RelType=wrtAttribute; SrcSynSetIdArgN=0; DstSynSetIdArgN=1;}
    else if (RelId==Ant_RelId){RelType=wrtAntonym; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==Sa_RelId){RelType=wrtAdditionalInfo; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==Ppl_RelId){RelType=wrtParticiple; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==Per_RelId){RelType=wrtPosRel; SrcSynSetIdArgN=0; DstSynSetIdArgN=2;}
    else if (RelId==Fr_RelId){RelType=wrtUndef;}
    else {TStr FuncArityStr=PlBs->GetFuncArityStr(RelId); Fail;}
    // traverse tuples
    if (RelType!=wrtUndef){
      for (int TupN=0; TupN<PlBs->GetRelTups(RelId); TupN++){
        // get tuple
        int TupId=PlBs->GetRelTupId(RelId, TupN);
        PPlTup Tup=PlBs->GetTup(TupId);
        // get components
        TStr SrcSynSetId=TInt::GetStr(Tup->GetVal(SrcSynSetIdArgN).GetInt());
        TStr DstSynSetId=TInt::GetStr(Tup->GetVal(DstSynSetIdArgN).GetInt());
        // add relation & inverse-relation
        if (IsSynSetId(SrcSynSetId)&&IsSynSetId(DstSynSetId)){
          AddRel(RelType, SrcSynSetId, DstSynSetId);
        }
      }
    }
  }

  printf("Done.\n");
}

void TWnBs::SaveTxtSynSet(const int& SynSetP, FILE* fOut, const int& LevelN){
  TStr SynSetId=GetSynSetId(SynSetP);
  PWnSynSet SynSet=GetSynSet(SynSetId);
  fprintf(fOut, "%s%s:", TStr::GetSpaceStr(LevelN*2).CStr(), SynSetId.CStr());
  for (int WordN=0; WordN<SynSet->GetWords(); WordN++){
    int EWordStrP=SynSet->GetWordStrP(WordN);
    TStr WordStr=GetEWordStrP(EWordStrP);
    fprintf(fOut, " '%s'", WordStr.CStr());
  }
  fprintf(fOut, "\n");
}

void TWnBs::SaveTxtRel(const TWnRelType& RelType, const int& SynSetP,
 const bool& Recurse, FILE* fOut, const int& LevelN, TIntS& SynSetPS){
  PWnSynSet SynSet=GetSynSetFromP(SynSetP);
  TIntV SubSynSetPV; SynSet->GetDstSynSetPV(RelType, SubSynSetPV);
  if ((LevelN==0)&&(!SubSynSetPV.Empty())){
    TStr RelTypeNm=GetRelTypeNm(RelType);
    fprintf(fOut, "  ---%s--------------------------\n", RelTypeNm.CStr());
  }
  for (int SubSynSetPN=0; SubSynSetPN<SubSynSetPV.Len(); SubSynSetPN++){
    int SubSynSetP=SubSynSetPV[SubSynSetPN];
    SaveTxtSynSet(SubSynSetP, fOut, LevelN+1);
    if (Recurse){
      if (!SynSetPS.IsIn(SubSynSetP)){
        SynSetPS.Push(SubSynSetP);
        SaveTxtRel(RelType, SubSynSetP, Recurse, fOut, LevelN+1, SynSetPS);
        SynSetPS.Pop();
      } else {
        fprintf(fOut, "  ***Cycling\n");
      }
    }
  }
}

void TWnBs::SaveTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int SynSetP=0; SynSetP<GetSynSets(); SynSetP++){
    fprintf(fOut, "=====================================\n");
    SaveTxtSynSet(SynSetP, fOut);
    SaveTxtRel(wrtHypernym, SynSetP, true, fOut);
    SaveTxtRel(wrtInstance, SynSetP, false, fOut);
    SaveTxtRel(wrtEntailment, SynSetP, false, fOut);
    SaveTxtRel(wrtSimilar, SynSetP, false, fOut);
    SaveTxtRel(wrtMemberMeronym, SynSetP, false, fOut);
    SaveTxtRel(wrtSubstanceMeronym, SynSetP, false, fOut);
    SaveTxtRel(wrtPartMeronym, SynSetP, false, fOut);
    SaveTxtRel(wrtDerivation, SynSetP, false, fOut);
    SaveTxtRel(wrtClass, SynSetP, false, fOut);
    SaveTxtRel(wrtCause, SynSetP, false, fOut);
    SaveTxtRel(wrtVerbGroup, SynSetP, false, fOut);
    SaveTxtRel(wrtAttribute, SynSetP, false, fOut);
    SaveTxtRel(wrtAntonym, SynSetP, false, fOut);
    SaveTxtRel(wrtAdditionalInfo, SynSetP, false, fOut);
    SaveTxtRel(wrtParticiple, SynSetP, false, fOut);
    SaveTxtRel(wrtPosRel, SynSetP, false, fOut);
    fprintf(fOut, "=====================================\n\n");
  }
}

