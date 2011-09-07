/////////////////////////////////////////////////
// Macro-Processor
TMacro::TMacro(const TStr& _TxtStr, const char& _MacroCh, const char& _VarCh):
  Ok(true), MsgStr("Ok"), MacroCh(_MacroCh), VarCh(_VarCh),
  TxtStr(_TxtStr), SubstToValStrH(10), VarNmToValStrH(10){
  int TxtStrLen=TxtStr.Len(); int TxtStrChN=0;
  while ((Ok)&&(TxtStrChN<TxtStrLen)){
    if (TxtStr[TxtStrChN]==MacroCh){
      // extract substitution from text
      TChA SubstChA; TxtStrChN++;
      while ((TxtStrChN<TxtStrLen)&&(TxtStr[TxtStrChN]!=MacroCh)){
        SubstChA+=TxtStr[TxtStrChN]; TxtStrChN++;}
      Ok=(TxtStrChN<TxtStrLen);
      if (!Ok){MsgStr=SubstChA; break;}
      TxtStrChN++;
      SubstToValStrH.AddDat(SubstChA);
      // extract variable name from substitution
      int SubstChN=0;
      while ((Ok)&&(SubstChN<SubstChA.Len())){
        if (SubstChA[SubstChN]==VarCh){
          TChA VarNmChA; SubstChN++;
          while ((SubstChN<SubstChA.Len())&&(SubstChA[SubstChN]!=VarCh)){
            VarNmChA+=SubstChA[SubstChN]; SubstChN++;}
          Ok=(SubstChN<SubstChA.Len());
          if (!Ok){MsgStr=VarNmChA; break;}
          SubstChN++;
          VarNmToValStrH.AddDat(VarNmChA);
        } else {
          SubstChN++;
        }
      }
    } else {
      TxtStrChN++;
    }
  }
}

TStr TMacro::GetDstTxtStr() const {
  int TxtStrLen=TxtStr.Len(); int TxtStrChN=0;
  TChA DstTxtChA;
  while (TxtStrChN<TxtStrLen){
    if (TxtStr[TxtStrChN]==MacroCh){
      TChA SubstChA; TxtStrChN++;
      while ((TxtStrChN<TxtStrLen)&&(TxtStr[TxtStrChN]!=MacroCh)){
        SubstChA+=TxtStr[TxtStrChN]; TxtStrChN++;}
      TxtStrChN++;
      int SubstKeyId;
      if (SubstToValStrH.IsKey(SubstChA, SubstKeyId)){
        DstTxtChA+=SubstToValStrH[SubstKeyId];
      } else {
        DstTxtChA+=MacroCh; DstTxtChA+=SubstChA; DstTxtChA+=MacroCh;
      }
    } else {
      DstTxtChA+=TxtStr[TxtStrChN]; TxtStrChN++;
    }
  }
  return DstTxtChA;
}

void TMacro::GetSrcSubstStrV(TStrV& SubstStrV) const {
  SubstStrV.Gen(GetSubstStrs(), 0);
  for (int SubstStrN=0; SubstStrN<GetSubstStrs(); SubstStrN++){
    SubstStrV.Add(GetSrcSubstStr(SubstStrN));
  }
}

TStr TMacro::GetDstSubstStr(const int& SubstStrN) const {
  TStr SrcSubstStr=SubstToValStrH.GetKey(SubstStrN);
  int SrcSubstStrLen=SrcSubstStr.Len();
  int SrcSubstChN=0;
  TChA DstSubstChA;
  while (SrcSubstChN<SrcSubstStrLen){
    if (SrcSubstStr[SrcSubstChN]==VarCh){
      TChA VarNmChA; SrcSubstChN++;
      while ((SrcSubstChN<SrcSubstStrLen)&&(SrcSubstStr[SrcSubstChN]!=VarCh)){
        VarNmChA+=SrcSubstStr[SrcSubstChN]; SrcSubstChN++;}
      IAssert(SrcSubstChN<SrcSubstStrLen); SrcSubstChN++;
      TStr VarVal=GetVarVal(VarNmChA);
      DstSubstChA+=VarVal;
    } else {
      DstSubstChA+=SrcSubstStr[SrcSubstChN]; SrcSubstChN++;
    }
  }
  return DstSubstChA;
}

TStr TMacro::GetAllSubstValStr() const {
  TChA AllSubstValChA;
  for (int SubstStrN=0; SubstStrN<GetSubstStrs(); SubstStrN++){
    if (SubstStrN>0){AllSubstValChA+=", ";}
    AllSubstValChA+=GetSubstValStr(SubstStrN);
  }
  return AllSubstValChA;
}

void TMacro::GetVarNmV(TStrV& VarNmV) const {
  VarNmV.Gen(GetVars(), 0);
  for (int VarN=0; VarN<GetVars(); VarN++){
    VarNmV.Add(GetVarNm(VarN));
  }
}

void TMacro::SplitVarNm(
 const TStr& VarNm, TStr& CapStr,
 bool& IsComboBox, TStr& TbNm, TStr& ListFldNm, TStr& DataFldNm){
  if (VarNm.SearchCh(':')==-1){
    CapStr=VarNm; IsComboBox=false; TbNm=""; ListFldNm=""; DataFldNm="";
  } else {
    int FirstColonChN=VarNm.SearchCh(':');
    int SecondColonChN=VarNm.SearchCh(':', FirstColonChN+1);
    int ThirdColonChN=VarNm.SearchCh(':', SecondColonChN+1);
    IAssert((FirstColonChN!=-1)&&(SecondColonChN!=-1));
    if (ThirdColonChN==-1){ThirdColonChN=VarNm.Len();}
    CapStr=VarNm.GetSubStr(0, FirstColonChN-1);
    IsComboBox=true;
    TbNm=VarNm.GetSubStr(FirstColonChN+1, SecondColonChN-1);
    ListFldNm=VarNm.GetSubStr(SecondColonChN+1, ThirdColonChN-1);
    if (ThirdColonChN!=VarNm.Len()){
      DataFldNm=VarNm.GetSubStr(ThirdColonChN+1, VarNm.Len()-1);
    } else {
      DataFldNm=ListFldNm;
    }
  }
}

