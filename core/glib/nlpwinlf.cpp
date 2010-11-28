/////////////////////////////////////////////////
// Includes
#include "nlpwinlf.h"

/////////////////////////////////////////////////
// Logical-Form-Substitutions
void TLfSubst::GetLfSubstV(TLfSubstV& LfSubstV){
  LfSubstV.Clr();
  { // ... he
  PLfSubst LfSubst=TLfSubst::New("He"); LfSubstV.Add(LfSubst);
  // targets
  LfSubst->AddTarget("he"); LfSubst->AddTarget("him");
  LfSubst->AddTarget("his"); LfSubst->AddTarget("himself");
  // bits
  LfSubst->AddSelBit("Masc");
  LfSubst->AddSelBit("Sing");
  //LfSubst->AddSelBit("PrprN");
  LfSubst->AddSelBit("Humn");
  //LfSubst->AddSelBit("Nme");
  }

  { // ... she
  PLfSubst LfSubst=TLfSubst::New("She"); LfSubstV.Add(LfSubst);
  // targets
  LfSubst->AddTarget("she"); LfSubst->AddTarget("her");
  LfSubst->AddTarget("hers"); LfSubst->AddTarget("herself");
  // bits
  LfSubst->AddSelBit("Fem");
  LfSubst->AddSelBit("Sing");
  //LfSubst->AddSelBit("PrprN");
  LfSubst->AddSelBit("Humn");
  //LfSubst->AddSelBit("Nme");
  }

  { // ... name
  PLfSubst LfSubst=TLfSubst::New("Name"); LfSubstV.Add(LfSubst);
  // bits
  LfSubst->AddSelBit("Sing");
  LfSubst->AddSelBit("PrprN");
  //LfSubst->AddSelBit("Humn");
  }

  { // ... they
  PLfSubst LfSubst=TLfSubst::New("They"); LfSubstV.Add(LfSubst);
  // targets
  LfSubst->AddTarget("they"); LfSubst->AddTarget("their");
  // bits
  LfSubst->AddSelBit("Plur");
  LfSubst->AddSelBit("Humn");
  }
}

void TLfSubst::Dump(const bool& DumpP, const TLfSubstV& LfSubstV){
  if (!DumpP){return;}
  printf("---Start-----Substitutions-------\n");
  for (int LfSubstN=0; LfSubstN<LfSubstV.Len(); LfSubstN++){
    // get substitution
    PLfSubst LfSubst=LfSubstV[LfSubstN];
    // print id & current value
    printf("%s : %s\n", LfSubst->SubstId.CStr(), LfSubst->SubstStr.CStr());
  }
  printf("---End-------Substitutions-------\n");
}

/////////////////////////////////////////////////
// Logical-Form-Alias-Base
bool TLfAliasBs::IsGenericWord(const TStr& WordStr) const {
  if (WordStr.IsChIn('.')){return true;}
  if (SwSet->IsIn(WordStr)){return true;}
  if (WordStr=="PRESIDENT"){return true;}
  if (WordStr=="JUSTICE"){return true;}
  if (WordStr=="JUDGE"){return true;}
  if (WordStr=="NIGHT"){return true;}
  if (WordStr=="U.S."){return true;}

  if (WordStr=="SOUTH"){return true;}
  if (WordStr=="NORTH"){return true;}
  if (WordStr=="WEST"){return true;}
  if (WordStr=="EAST"){return true;}
  return false;
}

void TLfAliasBs::PutAliasStr(const TStr& SrcStr, const TStr& SubstId, TStr& DstStr){
  // get word-vector from source-string
  TStr SrcUcStr=SrcStr.GetUc();
  TStrV SrcUcStrV; SrcUcStr.SplitOnWs(SrcUcStrV);
  for (int SrcUcStrN=0; SrcUcStrN<SrcUcStrV.Len(); SrcUcStrN++){
    if (IsGenericWord(SrcUcStrV[SrcUcStrN])){
      SrcUcStrV[SrcUcStrN]="";}
  }
  SrcUcStrV.DelAll("");
  SrcUcStrV.Sort();
  // get substitution
  if (WordStrVToAliasStrH.IsKey(SrcUcStrV)){
    DstStr=WordStrVToAliasStrH.GetDat(SrcUcStrV);
    WordStrVToAliasH.GetDat(SrcUcStrV).IncFq();
  } else {
    // collect non-zero intersections
    for (int WAStrP=0; WAStrP<WordStrVToAliasStrH.Len(); WAStrP++){
      TStrV& WordUcStrV=WordStrVToAliasStrH.GetKey(WAStrP);
      TStr AliasStr=WordStrVToAliasStrH[WAStrP];
      TStrV IntrsWordStrV; WordUcStrV.Intrs(SrcUcStrV, IntrsWordStrV);
      if (IntrsWordStrV.Len()>0){
        AddAlias(SrcUcStrV, SubstId, AliasStr);
        DstStr=AliasStr;
        return;
      }
    }
    AddAlias(SrcUcStrV, SubstId, SrcStr);
    DstStr=SrcStr;
  }
}

void TLfAliasBs::UpdateLfSubstV(const TStr& AliasStr, const TLfSubstV& LfSubstV) const {
  for (int WToAN=0; WToAN<WordStrVToAliasStrH.Len(); WToAN++){
    if (WordStrVToAliasStrH[WToAN]==AliasStr){
      TStrV& WordStrV=WordStrVToAliasStrH.GetKey(WToAN);
      TStr SubstId=WordStrVToAliasH.GetDat(WordStrV).GetSubstId();
      for (int LfSubstN=0; LfSubstN<LfSubstV.Len(); LfSubstN++){
        if (LfSubstV[LfSubstN]->SubstId==SubstId){
          LfSubstV[LfSubstN]->SubstStr=AliasStr;
        }
      }
    }
  }
}

void TLfAliasBs::AddAlias(
 const TStrV& WordStrV, const TStr& SubstId, const TStr& AliasStr){
  WordStrVToAliasStrH.AddDat(WordStrV, AliasStr);
  IAssert(!WordStrVToAliasH.IsKey(WordStrV));
  TLfAlias LfAlias(SubstId);
  LfAlias.IncFq();
  WordStrVToAliasH.AddDat(WordStrV, LfAlias);
}

void TLfAliasBs::MergeAliasBs(const PLfAliasBs& LfAliasBs){
  int MnAliasFq=2;
  int MnIntrsLen=2;
  for (int NewWordStrVN=0; NewWordStrVN<LfAliasBs->WordStrVToAliasH.Len(); NewWordStrVN++){
    TStrV& NewWordStrV=LfAliasBs->WordStrVToAliasH.GetKey(NewWordStrVN);
    TStr NewAliasStr=LfAliasBs->WordStrVToAliasStrH.GetDat(NewWordStrV);
    int NewAliasFq=LfAliasBs->WordStrVToAliasH[NewWordStrVN].GetFq();
    if (NewAliasFq>=MnAliasFq){
      if (WordStrVToAliasH.IsKey(NewWordStrV)){
        WordStrVToAliasH.GetDat(NewWordStrV).IncFq(NewAliasFq);
      } else {
        TIntKdV IntrsLen_OldWordStrVNV;
        for (int OldWordStrVN=0; OldWordStrVN<WordStrVToAliasH.Len(); OldWordStrVN++){
          TStrV& OldWordStrV=WordStrVToAliasH.GetKey(OldWordStrVN);
          TStr OldAliasStr=WordStrVToAliasStrH.GetDat(OldWordStrV);
          TStrV IntrsWordStrV; NewWordStrV.Intrs(OldWordStrV, IntrsWordStrV);
          if (IntrsWordStrV.Len()>0){
            IntrsLen_OldWordStrVNV.Add(TIntKd(IntrsWordStrV.Len(), OldWordStrVN));
          }
        }
        IntrsLen_OldWordStrVNV.Sort(false);
        if (IntrsLen_OldWordStrVNV.Len()==0){
          TLfAlias NewLfAlias(LfAliasBs->WordStrVToAliasH[NewWordStrVN]);
          WordStrVToAliasH.AddDat(NewWordStrV, NewLfAlias);
          WordStrVToAliasStrH.AddDat(NewWordStrV, NewAliasStr);
        } else
        if (IntrsLen_OldWordStrVNV.Len()>=MnIntrsLen){
          int OldWordStrVN=IntrsLen_OldWordStrVNV[0].Dat;
          TStrV& OldWordStrV=WordStrVToAliasH.GetKey(OldWordStrVN);
          TStr OldAliasStr=WordStrVToAliasStrH.GetDat(OldWordStrV);
          TLfAlias NewLfAlias(LfAliasBs->WordStrVToAliasH[NewWordStrVN]);
          WordStrVToAliasH.AddDat(NewWordStrV, NewLfAlias);
          WordStrVToAliasStrH.AddDat(NewWordStrV, OldAliasStr);
        }
      }
    }
  }
}

void TLfAliasBs::Dump(const bool& DumpP, const int& MnAliasFq) const {
  if (!DumpP){return;}
  /*printf("======================\n");
  printf("Substitution\n");
  for (int WordStrVN=0; WordStrVN<WordStrVToAliasH.Len(); WordStrVN++){
    TStrV& WordStrV=WordStrVToAliasH.GetKey(WordStrVN);
    TStr SubstId=WordStrVToAliasH[WordStrVN]->GetSubstId();
    int Fq=WordStrVToAliasH[WordStrVN]->GetFq();
    for (int WordN=0; WordN<WordStrV.Len(); WordN++){
      printf("%s ", WordStrV[WordN].CStr());}
    printf(": %s/%d\n", SubstId.CStr(), Fq);
  }
  printf("----------------------\n");*/
  printf("Alias-Base\n");
  TStrStrVPrV AliasStrWordStrVV;
  WordStrVToAliasStrH.GetDatKeyPrV(AliasStrWordStrVV);
  AliasStrWordStrVV.Sort();
  for (int AliasN=0; AliasN<AliasStrWordStrVV.Len(); AliasN++){
    TStr AliasStr=AliasStrWordStrVV[AliasN].Val1;
    TStrV& WordStrV=AliasStrWordStrVV[AliasN].Val2;
    TStr SubstId=WordStrVToAliasH.GetDat(WordStrV).GetSubstId();
    int Fq=WordStrVToAliasH.GetDat(WordStrV).GetFq();
    if ((MnAliasFq==-1)||(Fq>=MnAliasFq)){
      printf("%s :", AliasStr.CStr());
      for (int WordN=0; WordN<WordStrV.Len(); WordN++){
        printf(" %s", WordStrV[WordN].CStr());}
      printf(" [%s/%d]\n", SubstId.CStr(), Fq);
    }
  }
  printf("======================\n");
}

/////////////////////////////////////////////////
// Logical-Subject-Predicate-Object
TStrH TLfSpo::UnusableWordStrH;

bool TLfSpo::IsUsable() const {
  if (UnusableWordStrH.Empty()){
    UnusableWordStrH.AddKey(";");
    UnusableWordStrH.AddKey(",");
    UnusableWordStrH.AddKey("_");
    UnusableWordStrH.AddKey("BEEN");
    UnusableWordStrH.AddKey("ARE");
    UnusableWordStrH.AddKey("AREN'T");
    UnusableWordStrH.AddKey("IS");
    UnusableWordStrH.AddKey("DID");
    UnusableWordStrH.AddKey("WAS");
    UnusableWordStrH.AddKey("WERE");
    UnusableWordStrH.AddKey("IT");
    UnusableWordStrH.AddKey("WE");
    UnusableWordStrH.AddKey("I");
    UnusableWordStrH.AddKey("YOU");
    UnusableWordStrH.AddKey("AND");
    UnusableWordStrH.AddKey("OR");
    UnusableWordStrH.AddKey("HE");
    UnusableWordStrH.AddKey("SHE");
    UnusableWordStrH.AddKey("THIS");
  }
  TStr UcSubjStr=SubjStr.GetUc();
  TStr UcObjStr=ObjStr.GetUc();
  if ((UnusableWordStrH.IsKey(UcSubjStr))||(UnusableWordStrH.IsKey(UcObjStr))){
    return false;
  } else {
    return true;
  }
}

TStr TLfSpo::GetSent() const {
  TChA SentChA;
  SentChA+=SubjStr;
  SentChA+=' ';
  SentChA+=PredStr;
  SentChA+=' ';
  SentChA+=ObjStr;
  if (SentChA.Len()>0){SentChA.PutCh(0, toupper(SentChA[0]));}
  if ((SentChA.Len()>0)&&(SentChA.LastCh()!='.')){SentChA+='.';}
  return SentChA;
}

TStr TLfSpo::GetLSent() const {
  TChA SentChA;
  SentChA+=LSubjStr;
  SentChA+=' ';
  SentChA+=PredStr;
  SentChA+=' ';
  SentChA+=LObjStr;
  if (SentChA.Len()>0){
    SentChA.PutCh(0, toupper(SentChA[0]));}
  if (SentChA.Len()>0){
    if ((SentChA.LastCh()==',')||(SentChA.LastCh()==';')){
      SentChA.PutCh(SentChA.Len()-1, '.');
    } else
    if (SentChA.LastCh()!='.'){
      SentChA+='.';
    }
  }
  return SentChA;
}

/////////////////////////////////////////////////
// Logical-Form-Tree
bool TLfTree::IsSubLfTree(const TStr& SelLfTypeNm, const TStr& SelHeadTypeNm,
 TStr& SubHeadStr, TStr& SubSentStr, PLfTree& SubLfTree) const {
  // traverse sub-trees
  for (int ObjLfTreeN=0; ObjLfTreeN<SubLfTreeV.Len(); ObjLfTreeN++){
    // get subtree
    SubLfTree=SubLfTreeV[ObjLfTreeN];
    // test if logical-form-type fits
    if ((!SelLfTypeNm.Empty())&&(SubLfTree->LfTypeNm!=SelLfTypeNm)){continue;}
    // test if head-type fits
    if ((!SelHeadTypeNm.Empty())&&(SubLfTree->HeadTypeNm!=SelHeadTypeNm)){continue;}
    // get head & sentence
    SubHeadStr=SubLfTree->GetHeadOrSubstStr();
    SubSentStr=SubLfTree->GetArSentStr();
    // return 'found'
    return true;
  }
  // return 'not found'
  SubHeadStr=""; SubSentStr=""; SubLfTree=NULL;
  return false;
}

bool TLfTree::IsSubLfTree(const TStr& SelLfTypeNm, const TStr& SelHeadTypeNm,
 TStr& SubHeadStr, TStr& SubSentStr) const {
  PLfTree SubLfTree;
  return IsSubLfTree(SelLfTypeNm, SelHeadTypeNm, SubHeadStr, SubSentStr, SubLfTree);
}

void TLfTree::GetLfSpoV(
 TLfSpoV& LfSpoV, const int& MxLev,
 const TStr& UpSubjStr, const TStr& UpLSubjStr) const {
  // prune levels deeper than MxLev
  if ((MxLev!=-1)&&(Lev>MxLev)){return;}

  // get next subject & object
  TStr NextUpSubjStr; TStr NextUpLSubjStr;
  if (LfTypeNm=="Dsub"){
    NextUpSubjStr=GetHeadOrSubstStr(); NextUpLSubjStr=GetArSentStr();
  }
  if (HeadTypeNm!="VERB"){
    if (LfTypeNm=="Dobj"){/*NextUpObjStr=GetHeadOrSubstStr();*/}
    else if (LfTypeNm=="Dind"){/*NextUpObjStr=GetHeadOrSubstStr();*/}
  }

  if (HeadTypeNm=="VERB"){
    // get predicate
    TStr PredStr=GetHeadOrSubstStr();
    // get subject
    TStr SubjStr; TStr LSubjStr; PLfTree SubjLfTree;
    if (IsSubLfTree("Dsub", "", SubjStr, LSubjStr, SubjLfTree)){
      NextUpSubjStr=SubjStr; NextUpLSubjStr=LSubjStr;
    }
    // get object
    TStr ObjStr; TStr LObjStr; PLfTree ObjLfTree;
    if (IsSubLfTree("Dobj", "", ObjStr, LObjStr, ObjLfTree)){
      /*NextUpObjStr=ObjStr;*/
    } else
    if (IsSubLfTree("Dind", "", ObjStr, LObjStr, ObjLfTree)){
      /*NextUpObjStr=ObjStr;*/
    }
    // extend object if verb
    if ((!ObjLfTree.Empty())&&(ObjLfTree->HeadTypeNm=="VERB")){
      // add sub-subject if exists
      TStr SubSubjStr; TStr SubLSubjStr;
      if (ObjLfTree->IsSubLfTree("Dsub", "", SubSubjStr, SubLSubjStr)){
        ObjStr=SubSubjStr+" "+ObjStr;}
      // add sub-object if exists
      TStr SubObjStr; TStr SubLObjStr; PLfTree SubObjLfTree;
      if (ObjLfTree->IsSubLfTree("Dobj", "NOUN", SubObjStr, SubLObjStr, SubObjLfTree)){
        ObjStr=ObjStr+" "+SubObjStr; NextUpSubjStr=""; NextUpLSubjStr="";
      } else
      if (ObjLfTree->IsSubLfTree("Dind", "NOUN", SubObjStr, SubLObjStr, SubObjLfTree)){
        ObjStr=ObjStr+" "+SubObjStr; NextUpSubjStr=""; NextUpLSubjStr="";
      }
      ObjLfTree=SubObjLfTree;
      // update next-object
      if (!ObjLfTree.Empty()){
        /*NextUpObjStr=ObjStr;*/
      }
    }

    // get sub-subject/predicate/sub-object triple
    if ((!SubjLfTree.Empty())&&(!ObjLfTree.Empty())){
      // create & add triple
      LfSpoV.Add(TLfSpo::New(LSubjStr, SubjStr, PredStr, ObjStr, LObjStr));
    }
    // get upper-subject/predicate/sub-object triple
    if ((!UpSubjStr.Empty())&&(SubjLfTree.Empty())&&(!ObjLfTree.Empty())){
      // create & add triple
      LfSpoV.Add(TLfSpo::New(UpLSubjStr, UpSubjStr, PredStr, ObjStr, LObjStr));
    }
  }

  // recurse
  for (int SubLfTreeN=0; SubLfTreeN<SubLfTreeV.Len(); SubLfTreeN++){
    PLfTree SubLfTree=SubLfTreeV[SubLfTreeN];
    SubLfTree->GetLfSpoV(LfSpoV, MxLev, NextUpSubjStr, NextUpLSubjStr);
  }
}

void TLfTree::GetLfSpoV(TLfSpoV& LfSpoV, const int& MxLev) const {
  GetLfSpoV(LfSpoV, MxLev, "", "");
}

void TLfTree::PutSubst(
 const TLfSubstV& LfSubstV, const PLfAliasBs& LfAliasBs,
 TStrPrV& HeadStrSubstStrPrV, const bool& DumpP){
  // substitute if head appears in target-strings
  TStr HeadUcStr=HeadStr.GetUc();
  for (int LfSubstN=0; LfSubstN<LfSubstV.Len(); LfSubstN++){
    // get substitution
    PLfSubst LfSubst=LfSubstV[LfSubstN];
    // get new substitution if bits fit
    if ((!BitNmV.Empty())&&(!LfSubst->SelBitNmV.Empty())){
      // test if subst-bits fit current-node-bits
      bool BitMatchP=true;
      for (int SelBitN=0; SelBitN<LfSubst->SelBitNmV.Len(); SelBitN++){
        int BitN=0;
        while ((BitN<BitNmV.Len())&&(LfSubst->SelBitNmV[SelBitN]!=BitNmV[BitN])){BitN++;}
        if (BitN==BitNmV.Len()){BitMatchP=false; break;}
      }
      // if bits fit
      if (BitMatchP){
        if (!LfSubst->TargetStrV.IsIn(HeadUcStr)){
          LfAliasBs->PutAliasStr(HeadStr, LfSubst->SubstId, LfSubst->SubstStr);
          LfAliasBs->UpdateLfSubstV(LfSubst->SubstStr, LfSubstV);
          HeadSubstStr=LfSubst->SubstStr;
          HeadStrSubstStrPrV.Add(TStrPr(HeadStr, LfSubst->SubstStr));
          if (DumpP){
            printf("New substitution for '%s': '%s' (from '%s')\n",
             LfSubst->SubstId.CStr(), LfSubst->SubstStr.CStr(), HeadStr.CStr());
          }
        }
      }
    }
    // if head and target-list match
    if ((!LfSubst->SubstStr.Empty())&&(LfSubst->TargetStrV.IsIn(HeadUcStr))){
      HeadSubstStr=LfSubst->SubstStr;
      HeadStrSubstStrPrV.Add(TStrPr(HeadStr, LfSubst->SubstStr));
      if (DumpP){
        printf("Substitute head: '%s' --> '%s'\n",
         HeadStr.CStr(), LfSubst->SubstStr.CStr());
      }
    }
  }
  // recurse
  for (int SubLfTreeN=0; SubLfTreeN<SubLfTreeV.Len(); SubLfTreeN++){
    SubLfTreeV[SubLfTreeN]->PutSubst(
     LfSubstV, LfAliasBs, HeadStrSubstStrPrV, DumpP);
  }
  // update sentence
  ArSentStr=TLfDoc::GenArSentStr(SentStr, HeadStrSubstStrPrV, false);
}

void TLfTree::ClrSubst(){
  HeadSubstStr="";
  // recurse
  for (int SubLfTreeN=0; SubLfTreeN<SubLfTreeV.Len(); SubLfTreeN++){
    SubLfTreeV[SubLfTreeN]->ClrSubst();
  }
}

PLfTree TLfTree::GetLfTree(const PXmlTok& TreeTok, const int& Lev){
  //if ((!TreeTok.Empty())&&(TreeTok->IsArg("Head"))){
  if (!TreeTok.Empty()){
    // create log-form node
    PLfTree LfTree=TLfTree::New();
    // extract head info
    LfTree->SentStr=TreeTok->GetArgVal("String", "");
    LfTree->LfTypeNm=TreeTok->GetTagNm();
    LfTree->Lev=Lev;
    LfTree->HeadStr=TreeTok->GetArgVal("Head", "");
    LfTree->HeadTypeNm=TreeTok->GetArgVal("HeadSegType", "");
    // get bits
    PXmlTok BitsTok;
    if (TreeTok->IsTagTok("Bits", BitsTok)){
      int BitToks=BitsTok->GetSubToks();
      for (int BitTokN=0; BitTokN<BitToks; BitTokN++){
        PXmlTok BitTok=BitsTok->GetSubTok(BitTokN);
        if (BitTok->IsTag()){
          LfTree->BitNmV.Add(BitTok->GetTagNm());
        }
      }
    }
    // get children
    int SubTrees=TreeTok->GetSubToks();
    for (int SubTreeN=0; SubTreeN<SubTrees; SubTreeN++){
      PXmlTok SubTreeTok=TreeTok->GetSubTok(SubTreeN);
      PLfTree SubLfTree=GetLfTree(SubTreeTok, Lev+1);
      if (!SubLfTree.Empty()){
        LfTree->SubLfTreeV.Add(SubLfTree);}
    }
    return LfTree;
  } else {
    return NULL;
  }
}

void TLfTree::Dump(){
  if (LfTypeNm=="Bits"){return;}
  for (int LevN=0; LevN<Lev; LevN++){printf("  ");}
  if (HeadSubstStr.Empty()){
    printf("%s:%s/%s\n", HeadStr.CStr(), HeadTypeNm.CStr(), LfTypeNm.CStr());
  } else {
    printf("%s[%s]:%s/%s\n",
     HeadStr.CStr(), HeadSubstStr.CStr(), HeadTypeNm.CStr(), LfTypeNm.CStr());
  }
  for (int SubLfTreeN=0; SubLfTreeN<SubLfTreeV.Len(); SubLfTreeN++){
    SubLfTreeV[SubLfTreeN]->Dump();
  }
}

/////////////////////////////////////////////////
// Logical-Form-Document
TStr TLfDoc::GenArSentStr(
 const TStr& SentStr, const TStrPrV& HeadStrSubstStrPrV, const bool& PhraseP){
  if (HeadStrSubstStrPrV.Empty()){return SentStr;}
  TStr ArSentStr=SentStr;
  TStrV UsedSubstStrV;
  int SentSearchChN=0;
  for (int HSPrN=0; HSPrN<HeadStrSubstStrPrV.Len(); HSPrN++){
    // prepare head & substitution
    TStr HeadStr=HeadStrSubstStrPrV[HSPrN].Val1;
    TStr SubstStr=HeadStrSubstStrPrV[HSPrN].Val2;
    if (HeadStr==SubstStr){continue;}
    if (PhraseP){
      SubstStr.ChangeChAll(' ', '_');
      SubstStr.ChangeChAll('.', '_');
      SubstStr.ChangeChAll(',', '_');
    }
    // search the head
    int PrevSentSearchChN=SentSearchChN;
    forever{
      SentSearchChN=ArSentStr.SearchStr(HeadStr, SentSearchChN);
      // check word boundary
      if (SentSearchChN==-1){break;}
      char PrevCh=TCh::NullCh;
      char NextCh=TCh::NullCh;
      if (SentSearchChN>0){PrevCh=ArSentStr[SentSearchChN-1];}
      if (SentSearchChN+HeadStr.Len()<ArSentStr.Len()){
        NextCh=ArSentStr[SentSearchChN+HeadStr.Len()];}
      if ((!TCh::IsAlpha(PrevCh))&&(!TCh::IsAlpha(NextCh))){break;}
      else {SentSearchChN++;}
    }
    if (SentSearchChN==-1){
      SentSearchChN=PrevSentSearchChN;
    } else {
      // inserting head[subst]
      //ArSentStr.InsStr(SentSearchChN+HeadStr.Len(), TStr("[")+SubstStr+"]");
      //SentSearchChN+=HeadStr.Len()+1+SubstStr.Len()+1;
      // deleting head and inserting subst
      if (!UsedSubstStrV.IsIn(SubstStr)){
        ArSentStr.ChangeStr(HeadStr, SubstStr, SentSearchChN);
        SentSearchChN+=SubstStr.Len();
        UsedSubstStrV.Add(SubstStr);
      }
    }
  }
  // change multiword phrases
  {for (int HSPrN=0; HSPrN<HeadStrSubstStrPrV.Len(); HSPrN++){
    // prepare head & substitution
    TStr HeadStr=HeadStrSubstStrPrV[HSPrN].Val1;
    TStr SubstStr=HeadStrSubstStrPrV[HSPrN].Val2;
    if (HeadStr!=SubstStr){continue;}
    if (!HeadStr.IsChIn(' ')){continue;}
    if (PhraseP){
      SubstStr.ChangeChAll(' ', '_');
      SubstStr.ChangeChAll('.', '_');
      SubstStr.ChangeChAll(',', '_');
    }
    // search the head
    int SentSearchChN=0;
    forever{
      SentSearchChN=ArSentStr.SearchStr(HeadStr, SentSearchChN);
      // check word boundary
      if (SentSearchChN==-1){break;}
      char PrevCh=TCh::NullCh;
      char NextCh=TCh::NullCh;
      if (SentSearchChN>0){PrevCh=ArSentStr[SentSearchChN-1];}
      if (SentSearchChN+HeadStr.Len()<ArSentStr.Len()){
        NextCh=ArSentStr[SentSearchChN+HeadStr.Len()];}
      if ((!TCh::IsAlpha(PrevCh))&&(!TCh::IsAlpha(NextCh))){break;}
      else {SentSearchChN++;}
    }
    if (SentSearchChN!=-1){
      if (!UsedSubstStrV.IsIn(SubstStr)){
        // deleting head and inserting subst
        ArSentStr.ChangeStr(HeadStr, SubstStr, SentSearchChN);
        UsedSubstStrV.Add(SubstStr);
      }
    }
  }}
  return ArSentStr;
}

void TLfDoc::GenHeadsSubst(const PLfAliasBs& CtxLfAliasBs, const bool& DumpP){
  // get substitutions
  TLfSubstV LfSubstV; TLfSubst::GetLfSubstV(LfSubstV);
  // create alias
  PLfAliasBs LfAliasBs=CtxLfAliasBs;
  if (LfAliasBs.Empty()){
    LfAliasBs=TLfAliasBs::New();}
  // traverse sentences
  int Sents=GetSents();
  for (int SentN=0; SentN<Sents; SentN++){
    TStr SentStr=GetSentStr(SentN);
    if (DumpP){
      printf("=================================\n");
      printf("Sentence %d: %s\n", SentN, SentStr.CStr());
      printf("-----------------\n");
    }
    // get sentence logical-form tree
    PLfTree SentLfTree=GetSentLfTree(SentN);
    // head substitutions
    TStrPrV HeadStrSubstStrPrV;
    SentLfTree->PutSubst(LfSubstV, LfAliasBs, HeadStrSubstStrPrV, DumpP);
    // sentence substitutions
    TStr ArSentStr=GenArSentStr(SentStr, HeadStrSubstStrPrV, true);
    SentStrArSentStrPrV[SentN].Val2=ArSentStr;
    if (DumpP){
      printf("-----------------\n");
      printf("ArSentence %d: %s\n", SentN, ArSentStr.CStr());
    }
    // dump
    TLfSubst::Dump(DumpP, LfSubstV);
    LfAliasBs->Dump(DumpP);
  }
}

void TLfDoc::ClrHeadsSubst(){
  int Sents=GetSents();
  for (int SentN=0; SentN<Sents; SentN++){
    PLfTree SentLfTree=GetSentLfTree(SentN);
    SentLfTree->ClrSubst();
  }
}

void TLfDoc::Dump(const int& DumpSentN) const {
  printf("=========================\n");
  printf("Document: '%s'\n", DocId.CStr());
  for (int SentN=0; SentN<GetSents(); SentN++){
    if ((SentN!=-1)&&(SentN!=DumpSentN)){continue;}
    printf("-------------------------\n");
    printf("Sentence[%d]: %s\n", SentN, GetSentStr(SentN).CStr());
    printf("ArSenten[%d]: %s\n", SentN, GetArSentStr(SentN).CStr());
    printf("-------------------------\n");
    GetSentLfTree(SentN)->Dump();
  }
  printf("=========================\n");
}

/////////////////////////////////////////////////
// Logical-Form-Document-Base
PLfDoc TLfDocBs::GetLfDoc(const TStr& DocId) const {
  int LfDocs=GetLfDocs();
  for (int LfDocN=0; LfDocN<LfDocs; LfDocN++){
    if (GetLfDoc(LfDocN)->DocId==DocId){
      return GetLfDoc(LfDocN);
    }
  }
  return NULL;
}

void TLfDocBs::GenHeadsSubst(const bool& DumpP){
  int LfDocs=GetLfDocs();
  for (int LfDocN=0; LfDocN<LfDocs; LfDocN++){
    GetLfDoc(LfDocN)->GenHeadsSubst(NULL, DumpP);
  }
}

void TLfDocBs::GenHeadsSubst(const TStrV& DocIdV, const bool& DumpP){
  // traverse documents from doc-id list
  TLfAliasBsV LfAliasBsV;
  for (int DocIdN=0; DocIdN<DocIdV.Len(); DocIdN++){
    TStr DocId=DocIdV[DocIdN]; PLfDoc LfDoc;
    if (IsLfDoc(DocId, LfDoc)){
      PLfAliasBs LfAliasBs=TLfAliasBs::New();
      LfDoc->GenHeadsSubst(LfAliasBs, false);
      LfAliasBsV.Add(LfAliasBs);
      //LfAliasBs->Dump(DumpP, 2);
    }
  }
  // create alias-object
  PLfAliasBs TopLfAliasBs=TLfAliasBs::New();
  for (int LfAliasBsN=0; LfAliasBsN<LfAliasBsV.Len(); LfAliasBsN++){
    TopLfAliasBs->MergeAliasBs(LfAliasBsV[LfAliasBsN]);
    //TopLfAliasBs->Dump(DumpP);
  }
  TopLfAliasBs->Dump(DumpP);
  // clear substitution
  for (int DocIdN=0; DocIdN<DocIdV.Len(); DocIdN++){
    TStr DocId=DocIdV[DocIdN]; PLfDoc LfDoc;
    if (IsLfDoc(DocId, LfDoc)){
      LfDoc->ClrHeadsSubst();
      PLfAliasBs LfAliasBs=TLfAliasBs::New(TopLfAliasBs);
      LfDoc->GenHeadsSubst(LfAliasBs, false);
    }
  }
}

void TLfDocBs::GetCompClV(const PXmlTok& XmlTok, TStrV& CompClStrV) const {
  if (XmlTok->IsTag("NP")&&XmlTok->IsArg("name")){
    if (XmlTok->GetArgVal("name").IsPrefix("COMPCL")){
      TStr CompClStr=XmlTok->GetArgVal("String");
      CompClStrV.AddMerged(CompClStr);
      return;
    }
  }
  int SubToks=XmlTok->GetSubToks();
  for (int SubTokN=0; SubTokN<SubToks; SubTokN++){
    GetCompClV(XmlTok->GetSubTok(SubTokN), CompClStrV);
  }
}

PLfDocBs TLfDocBs::LoadXml(const TStr& FNm, const bool& DumpP){
  // create document-base
  PLfDocBs LfDocBs=TLfDocBs::New();
  // open xml file
  PSIn XmlSIn=TFIn::New(FNm);
  // skip top level tag
  TXmlDoc::SkipTopTag(XmlSIn);
  // traverse file
  PXmlDoc XmlDoc; int XmlDocs=0;
  forever{
    // load xml tree
    XmlDocs++; printf("%d\r", XmlDocs);
    XmlDoc=TXmlDoc::LoadTxt(XmlSIn);
    if (!XmlDoc->IsOk()){break;}
    // extract fields from xml-trees
    PXmlTok DocTok=XmlDoc->GetTok();
    // document level
    IAssert(DocTok->IsTag("DOCUMENT"));
    TStr DocId=DocTok->GetArgVal("NO");
    printf("%s: ", DocId.CStr());
    PLfDoc LfDoc=TLfDoc::New(DocId);
    LfDocBs->AddLfDoc(LfDoc);
    // traverse sentences
    int SentToks=DocTok->GetSubToks();
    for (int SentTokN=0; SentTokN<SentToks; SentTokN++){
      PXmlTok SentTok=DocTok->GetSubTok(SentTokN);
      if (!SentTok->IsTag("SENTENCE")){continue;}
      // sentence level
      TStr SentId=SentTok->GetArgVal("ID");
      printf("[%d]", SentId.GetInt());
      PXmlTok LfTok=SentTok->GetTagTok("NLP|LogicalForm");
      TStr SentStr; PLfTree SentLfTree;
      if (!LfTok.Empty()){
        // get sentence data
        SentStr=LfTok->GetArgVal("String");
        PXmlTok SentTreeTok=SentTok->GetTagTok("NLP|LogicalForm|Sentence");
        // create logical-form node
        SentLfTree=TLfTree::GetLfTree(SentTreeTok);
        if (SentLfTree.Empty()){SentLfTree=TLfTree::New();}
      } else {
        SentLfTree=TLfTree::New();
      }
      LfDoc->AddSent(SentStr, SentLfTree);
      IAssert(SentId.GetInt()==LfDoc->GetSents()-1);
    }
    printf("\n");
    // dump document if enabled
    if (DumpP){LfDoc->Dump();}
  }
  // return document-base
  return LfDocBs;
}

PLfDocBs TLfDocBs::LoadBinSet(const TStr& FPath, const TStr& FExt){
  printf("Loading Logical-Form-Binary-Set...\n");
  PLfDocBs LfDocBs=TLfDocBs::New();
  TFFile FFile(FPath, FExt, true); TStr FNm;
  while (FFile.Next(FNm)){
    printf("%s\n", FNm.CStr());
    PLfDocBs LocLfDocBs=TLfDocBs::LoadBin(FNm);
    //for (int LfDocN=0; LfDocN<LocLfDocBs->GetLfDocs(); LfDocN++){
    //  printf("%s: %d\n",
    //   LocLfDocBs->GetLfDoc(LfDocN)->GetDocId().CStr(),
    //   LocLfDocBs->GetLfDoc(LfDocN)->GetSents());
    //}
    LfDocBs->AddLfDocBs(LocLfDocBs);
  }
  printf("Done.\n");
  return LfDocBs;
}

void TLfDocBs::ConvXmlToLdb(const TStr& FNm, const bool& DumpP){
  printf("%s\n", FNm.CStr());
  PLfDocBs LfDocBs=TLfDocBs::LoadXml(FNm, DumpP);
  LfDocBs->SaveBin(TStr::PutFExt(FNm, ".Ldb"));
}

void TLfDocBs::ConvSetXmlToLdb(const TStr& FPath, const bool& DumpP){
  TFFile FFile(FPath, ".xml", false); TStr FNm;
  while (FFile.Next(FNm)){
    ConvXmlToLdb(FNm, DumpP);
  }
}

void TLfDocBs::Dump() const {
  for (int LfDocN=0; LfDocN<GetLfDocs(); LfDocN++){
    GetLfDoc(LfDocN)->Dump();
  }
}

