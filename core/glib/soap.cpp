/////////////////////////////////////////////////
// Soap-Table
void TSoapTb::Append(const PSoapTb& SoapTb){
  for (int RowN=0; RowN<SoapTb->GetRows(); RowN++){
    AddRow();
    for (int ColN=0; ColN<SoapTb->GetCols(); ColN++){
      TStr ColNm, ColVal;
      if (SoapTb->IsGetColNmVal(RowN, ColN, ColNm, ColVal)){
        AddVal(ColNm, ColVal);
      }
    }
  }
}

void TSoapTb::GetValV(const int& ColN, TStrV& ValStrV) const {
  // traverse rows
  ValStrV.Clr();
  int Rows=GetRows();
  for (int RowN=0; RowN<Rows; RowN++){
    ValStrV.Add(GetVal(RowN, ColN));
  }
}

void TSoapTb::GetValPrV(const int& ColN1, const int& ColN2, TStrPrV& ValStrPrV) const {
  // traverse rows
  ValStrPrV.Clr();
  int Rows=GetRows();
  for (int RowN=0; RowN<Rows; RowN++){
    ValStrPrV.Add(TStrPr(GetVal(RowN, ColN1), GetVal(RowN, ColN2)));
  }
}

int TSoapTb::GetRowN(const TStr& WhereColNm, const TStr& WhereValStr) const {
  // traverse rows
  int Rows=GetRows();
  for (int RowN=0; RowN<Rows; RowN++){
    // traverse row-values
    int RowVals=GetRowVals(RowN);
    TStr ColNm; TStr ValStr;
    for (int ValN=0; ValN<RowVals; ValN++){
      // get row-value
      GetColNmVal(RowN, ValN, ColNm, ValStr);
      // return row-number if where-condition satisfied
      if ((ColNm==WhereColNm)&&(ValStr==WhereValStr)){
        return RowN;
      }
    }
  }
  return -1;
}

void TSoapTb::SelectValStrV(const TStr& SelColNm, TStrV& SelValStrV,
 const TStr& WhereColNm, const TStr& WhereValStr) const {
  // prepare selected-values
  SelValStrV.Clr();
  // traverse rows
  int Rows=GetRows();
  for (int RowN=0; RowN<Rows; RowN++){
    // traverse row-values
    int RowVals=GetRowVals(RowN);
    TStr ColNm; TStr ValStr;
    TStr CurSelValStr; TStr CurWhereValStr;
    for (int ValN=0; ValN<RowVals; ValN++){
      // get row-value
      GetColNmVal(RowN, ValN, ColNm, ValStr);
      // retrieve value if select-column or where-column
      if (ColNm==SelColNm){CurSelValStr=ValStr;}
      if (ColNm==WhereColNm){CurWhereValStr=ValStr;}
    }
    // check & add value if where-condition satisfied
    if (CurWhereValStr==WhereValStr){
      SelValStrV.Add(CurSelValStr);
    }
  }
}

void TSoapTb::SelectValStrV(const TStr& SelColNm, TStrV& SelValStrV,
 const TStr& WhereColNm1, const TStr& WhereValStr1,
 const TStr& WhereColNm2, const TStr& WhereValStr2) const {
  // prepare selected-values
  SelValStrV.Clr();
  // traverse rows
  int Rows=GetRows();
  for (int RowN=0; RowN<Rows; RowN++){
    // traverse row-values
    int RowVals=GetRowVals(RowN);
    TStr ColNm; TStr ValStr;
    TStr CurSelValStr; TStr CurWhereValStr1; TStr CurWhereValStr2;
    for (int ValN=0; ValN<RowVals; ValN++){
      // get row-value
      GetColNmVal(RowN, ValN, ColNm, ValStr);
      // retrieve value if select-column or where-column
      if (ColNm==SelColNm){
        CurSelValStr=ValStr;}
      if (ColNm==WhereColNm1){
        CurWhereValStr1=ValStr;}
      if (ColNm==WhereColNm2){
        CurWhereValStr2=ValStr;}
    }
    // check & add value if where-condition satisfied
    if ((CurWhereValStr1==WhereValStr1)&&(CurWhereValStr2==WhereValStr2)){
      SelValStrV.Add(CurSelValStr);
    }
  }
}

void TSoapTb::SelectValStr(const TStr& SelColNm, TStr& SelValStr,
 const TStr& WhereColNm1, const TStr& WhereValStr1,
 const TStr& WhereColNm2, const TStr& WhereValStr2) const {
  TStrV SelValStrV;
  SelectValStrV(SelColNm, SelValStrV, WhereColNm1, WhereValStr1, WhereColNm2, WhereValStr2);
  if (SelValStrV.Empty()){SelValStr="";}
  else {SelValStr=SelValStrV[0];}
}

void TSoapTb::SelectValStr(const TStr& SelColNm, TStr& SelValStr,
 const TStr& WhereColNm, const TStr& WhereValStr) const {
  TStrV SelValStrV;
  SelectValStrV(SelColNm, SelValStrV, WhereColNm, WhereValStr);
  if (SelValStrV.Empty()){SelValStr="";}
  else {SelValStr=SelValStrV[0];}
}

void TSoapTb::Dump(const TStr& FNm) const {
  TStr D;
  for (int ColN=0; ColN<GetCols(); ColN++){
    D+="C"+TInt::GetStr(ColN)+":["+GetColNm(ColN)+"]";}
  D+="\n";
  for (int RowN=0; RowN<GetRows(); RowN++){
    D+="R"+TInt::GetStr(RowN)+":";
    int Vals=GetRowVals(RowN); TStr ColNm; TStr ValStr;
    for (int ValN=0; ValN<Vals; ValN++){
      GetColNmVal(RowN, ValN, ColNm, ValStr);
      D+=" ["+ColNm+":"+ValStr+"]";
    }
    D+="\n";
  }
  TFOut F(FNm); F.PutStr(D);
}

/////////////////////////////////////////////////
// Soap-Environment
TSoapEnv::TSoapEnv(const TStrPrV& FldNmValPrV):
  FldNmToValH(), FldNmToValVH(), FldNmToSoapTbH(){
  int Flds=FldNmValPrV.Len(); int FldN=0;
  while (FldN<Flds){
    // check type of value
    if (TSoap::IsVecFld(FldN, FldNmValPrV)){
      // vector-value
      TStr FldNm; TStrV FldValV;
      FldN=TSoap::GetVecFld(FldN, FldNm, FldValV, FldNmValPrV);
      FldNmToValVH.AddDat(FldNm, FldValV);
    } else
    if (TSoap::IsTbFld(FldN, FldNmValPrV)){
      // table-value
      TStr FldNm; PSoapTb SoapTb;
      FldN=TSoap::GetTbFld(FldN, FldNm, SoapTb, FldNmValPrV);
      FldNmToSoapTbH.AddDat(FldNm, SoapTb);
    } else {
      // simple-value
      TStr FldNm=FldNmValPrV[FldN].Val1;
      TStr FldVal=FldNmValPrV[FldN].Val2;
      FldNmToValH.AddDat(FldNm, FldVal);
      FldN++;
    }
  }
}

void TSoapEnv::CopySoapTbH(const TSoapEnv& SoapEnv){
  FldNmToSoapTbH.Clr();
  int KeyId=SoapEnv.FldNmToSoapTbH.FFirstKeyId();
  while (SoapEnv.FldNmToSoapTbH.FNextKeyId(KeyId)){
    TStr TbNm; PSoapTb SrcSoapTb;
    SoapEnv.FldNmToSoapTbH.GetKeyDat(KeyId, TbNm, SrcSoapTb);
    PSoapTb DestSoapTb=PSoapTb(new TSoapTb(*SrcSoapTb));
    FldNmToSoapTbH.AddDat(TbNm, DestSoapTb);
  }
}

void TSoapEnv::MergeSoapEnv(const PSoapEnv& SoapEnv, const bool& AppendSoapTbP){
  // simple-fields
  {int KeyId=SoapEnv->FldNmToValH.FFirstKeyId();
  while (SoapEnv->FldNmToValH.FNextKeyId(KeyId)){
    TStr FldNm=SoapEnv->FldNmToValH.GetKey(KeyId);
    TStr FldVal=SoapEnv->FldNmToValH[KeyId];
    FldNmToValH.AddDat(FldNm, FldVal);
  }}
  // vector-fields
  {int KeyId=SoapEnv->FldNmToValVH.FFirstKeyId();
  while (SoapEnv->FldNmToValVH.FNextKeyId(KeyId)){
    TStr FldNm=SoapEnv->FldNmToValVH.GetKey(KeyId);
    TStrV& FldValV=SoapEnv->FldNmToValVH[KeyId];
    FldNmToValVH.AddDat(FldNm, FldValV);
  }}
  // table-fields
  {int KeyId=SoapEnv->FldNmToSoapTbH.FFirstKeyId();
  while (SoapEnv->FldNmToSoapTbH.FNextKeyId(KeyId)){
    TStr FldNm=SoapEnv->FldNmToSoapTbH.GetKey(KeyId);
    PSoapTb SoapTb=SoapEnv->FldNmToSoapTbH[KeyId];
    if (AppendSoapTbP&&FldNmToSoapTbH.IsKey(FldNm)){
      FldNmToSoapTbH.GetDat(FldNm)->Append(SoapTb);
    } else {
      FldNmToSoapTbH.AddDat(FldNm, SoapTb);
    }
  }}
}

void TSoapEnv::AddFlattened(TStrPrV& FldNmValPrV) const {
  // simple-fields
  for (int FldN=0; FldN<FldNmToValH.Len(); FldN++){
    TStr FldNm=FldNmToValH.GetKey(FldN);
    TStr FldVal=FldNmToValH[FldN];
    FldNmValPrV.Add(TStrPr(FldNm, FldVal));
  }
  // vector-fields
  for (int FldN=0; FldN<FldNmToValVH.Len(); FldN++){
    TStr FldNm=FldNmToValVH.GetKey(FldN);
    const TStrV& FldValV=FldNmToValVH[FldN];
    TSoap::AddVecFld(FldNm, FldValV, FldNmValPrV);
  }
  // table-fields
  for (int FldN=0; FldN<FldNmToSoapTbH.Len(); FldN++){
    TStr FldNm=FldNmToSoapTbH.GetKey(FldN);
    PSoapTb SoapTb=FldNmToSoapTbH[FldN];
    TSoap::AddTbFld(FldNm, SoapTb, FldNmValPrV);
  }
}

/////////////////////////////////////////////////
// Soap-General

// general
const TStr TSoap::ResponseStr="Response";

// Fault Codes
const TStr TSoap::VersionMismatchCodeNm="env:VersionMismatch";
const TStr TSoap::MustUnderstandCodeNm="env:MustUnderstand";
const TStr TSoap::DataEncodingUnknownCodeNm="env:DataEncodingUnknown";
const TStr TSoap::SenderCodeNm="env:Sender";
const TStr TSoap::ReceiverCodeNm="env:Receiver";

void TSoap::GetFromXmlStr(
 const TStr& XmlStr, const bool& RespP,
 TBool& Ok, bool& FaultP,
 TStr& FuncNm, TStrPrV& FldNmValPrV,
 TStr& FaultCodeNm, TStr& FaultReasonStr){
  // prepare fields
  Ok=false; FaultP=true;
  FuncNm=""; FldNmValPrV.Clr();
  FaultCodeNm=""; FaultReasonStr="";

  // construct xml-document
  PSIn SIn=TStrIn::New(XmlStr);
  PXmlDoc XmlDoc=TXmlDoc::LoadTxt(SIn);

  // fill soap-response fields
  if (XmlDoc->IsOk()){
    PXmlTok EnvTok=XmlDoc->GetTok();
    if (EnvTok->IsTag("env:Envelope")){
      PXmlTok BodyTok;
      if (EnvTok->IsSubTag("env:Body", BodyTok)){
        PXmlTok FaultTok;
        if (RespP&&(BodyTok->IsSubTag("env:Fault", FaultTok))){
          // failure message
          FaultP=true;
          FaultCodeNm=FaultTok->GetTagTokStr("env:Code|env:Value");
          FaultReasonStr=FaultTok->GetTagTokStr("env:Reason|env:Text");
          Ok=(!FaultCodeNm.Empty());
        } else {
          // search for result function name
          PXmlTok FuncTok;
          for (int TokN=0; TokN<BodyTok->GetSubToks(); TokN++){
            PXmlTok Tok=BodyTok->GetSubTok(TokN);
            if (Tok->IsTag()){
              TStr TagNm=Tok->GetTagNm();
              if (RespP){
                if (TagNm.IsSuffix(TSoap::ResponseStr)){
                  Ok=true; FaultP=false;
                  FuncTok=Tok;
                  FuncNm=TagNm.GetSubStr(0,
                   TagNm.Len()-1-TStr(TSoap::ResponseStr).Len());
                  break;
                }
              } else {
                Ok=true; FaultP=false;
                FuncTok=Tok;
                FuncNm=TagNm;
              }
            }
          }
          // extract field name & value pairs
          if (!FuncTok.Empty()){
            for (int TokN=0; TokN<FuncTok->GetSubToks(); TokN++){
              PXmlTok FldTok=FuncTok->GetSubTok(TokN);
              if (FldTok->IsTag()){
                TStr FldNm=FldTok->GetTagNm();
                TStr FldVal=FldTok->GetTagTokStr("");
                FldNmValPrV.Add(TStrPr(FldNm, FldVal));
              }
            }
          }
        }
      }
    }
  }
}

TStr TSoap::GetUrlPathStr(
 const TStr& FuncNm, const bool& RespP,
 const TStrPrV& FldNmValPrV){
  // create character-buffer
  TChA ChA;
  // add function name
  TStr UrlFuncNm=TUrl::GetUrlSearchStr(FuncNm);
  ChA+=UrlFuncNm; if (RespP){ChA+="Response";} ChA+="?";
  // add fields
  for (int FldN=0; FldN<FldNmValPrV.Len(); FldN++){
    if (FldN>0){ChA+='&';}
    // prepare field name & value
    TStr UrlFldNm=TUrl::GetUrlSearchStr(FldNmValPrV[FldN].Val1);
    TStr UrlFldVal=TUrl::GetUrlSearchStr(FldNmValPrV[FldN].Val2);
    // add field name&value
    ChA+=UrlFldNm; ChA+="="; ChA+=UrlFldVal;
  }
  // return string
  return ChA;
}

TStr TSoap::GetUrlPathStr(
 const TStr& FuncNm, const bool& RespP,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2){
  TStrPrV FldNmValPrV(2, 0);
  if (!FldNm1.Empty()){
    FldNmValPrV.Add(TStrPr(FldNm1, FldVal1));}
  if (!FldNm2.Empty()){
    FldNmValPrV.Add(TStrPr(FldNm2, FldVal2));}
  return GetUrlPathStr(FuncNm, RespP, FldNmValPrV);
}

TStr TSoap::GetXmlStr(
 const TStr& FuncNm, const bool& RespP,
 const TStrPrV& FldNmValPrV){
  TStr FuncXmlNm=FuncNm.GetTrunc(); //IAssert(FuncXmlNm.IsWord());
  TChA ChA;
  ChA+="<?xml version='1.0' ?>\n";
  ChA+="<env:Envelope xmlns:env=\"http://www.w3.org/2003/05/soap-envelope\">\n";
  ChA+="  <env:Body>\n";
  ChA+="    <"; ChA+=FuncXmlNm; if (RespP){ChA+="Response";} ChA+=">\n";
  for (int FldN=0; FldN<FldNmValPrV.Len(); FldN++){
    // prepare field name & value
    TStr FldNm=FldNmValPrV[FldN].Val1.GetTrunc(); //IAssert(FldNm.IsWord());
    TStr FldVal=TXmlLx::GetXmlStrFromPlainStr(FldNmValPrV[FldN].Val2);
    // add field-tag
    ChA+="      <"; ChA+=FldNm; ChA+=">";
    ChA+=FldVal;
    ChA+="</"; ChA+=FldNm;ChA+=">\n";
  }
  ChA+="    </"; ChA+=FuncXmlNm; if (RespP){ChA+="Response";} ChA+=">\n";
  ChA+="  </env:Body>\n";
  ChA+="</env:Envelope>\n";
  return ChA;
}

TStr TSoap::GetXmlStr(
 const TStr& FuncNm, const bool& RespP){
  TStrPrV FldNmValPrV;
  return GetXmlStr(FuncNm, RespP, FldNmValPrV);
}

TStr TSoap::GetXmlStr(
 const TStr& FuncNm, const bool& RespP,
 const TStr& FldNm, const TStr& FldVal){
  TStrPrV FldNmValPrV;
  FldNmValPrV.Add(TStrPr(FldNm, FldVal));
  return GetXmlStr(FuncNm, RespP, FldNmValPrV);
}

TStr TSoap::GetXmlStr(
 const TStr& FuncNm, const bool& RespP,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2){
  TStrPrV FldNmValPrV;
  FldNmValPrV.Add(TStrPr(FldNm1, FldVal1));
  FldNmValPrV.Add(TStrPr(FldNm2, FldVal2));
  return GetXmlStr(FuncNm, RespP, FldNmValPrV);
}

TStr TSoap::GetFaultXmlStr(const TStr& CodeNm, const TStr& ReasonStr){
  TStr ReasonXmlStr=TXmlLx::GetXmlStrFromPlainStr(ReasonStr);
  TChA ChA;
  ChA+="<?xml version='1.0' ?>\n";
  ChA+="<env:Envelope\n";
  ChA+=" xmlns:env=\"http://www.w3.org/2003/05/soap-envelope\"\n";
  ChA+=" xmlns:rpc='http://www.w3.org/2003/05/soap-rpc'>\n";
  ChA+="  <env:Body>\n";
  ChA+="    <env:Fault>\n";
  ChA+="      <env:Code>\n";
  ChA+="        <env:Value>"; ChA+=CodeNm; ChA+="</env:Value>\n";
  ChA+="      </env:Code>\n";
  ChA+="      <env:Reason>\n";
  ChA+="        <env:Text>"; ChA+=ReasonXmlStr; ChA+="</env:Text>\n";
  ChA+="      </env:Reason>\n";
  ChA+="    </env:Fault>\n";
  ChA+="  </env:Body>\n";
  ChA+="</env:Envelope>";
  return ChA;
}

bool TSoap::IsVecFld(const int& FldN, const TStrPrV& FldNmValPrV){
  return FldNmValPrV[FldN].Val1.IsSuffix("_VecBeg");
}

bool TSoap::IsVecFld(const TStr& FldNm, const TStrPrV& FldNmValPrV){
  TStr VecBeg_FldNm=FldNm+"_VecBeg";
  int Flds=FldNmValPrV.Len();
  for (int FldN=0; FldN<Flds; FldN++){
    if (FldNmValPrV[FldN].Val1==VecBeg_FldNm){return true;}
  }
  return false;
}

void TSoap::AddVecFld(const TStr& FldNm, const TStrV& FldValV, TStrPrV& FldNmValPrV){
  FldNmValPrV.Add(TStrPr(FldNm+"_VecBeg", TInt::GetStr(FldValV.Len())));
  for (int ValN=0; ValN<FldValV.Len(); ValN++){
    FldNmValPrV.Add(TStrPr(FldNm+"_"+TInt::GetStr(ValN), FldValV[ValN]));
  }
  FldNmValPrV.Add(TStrPr(FldNm+"_VecEnd", TInt::GetStr(FldValV.Len())));
}

void TSoap::GetVecFld(const TStr& FldNm, TStrV& FldValV, const TStrPrV& FldNmValPrV){
  // search begin-of-field
  TStr VecBeg_FldNm=FldNm+"_VecBeg";
  int Flds=FldNmValPrV.Len(); int FldN=0;
  while (FldN<Flds){
    if (FldNmValPrV[FldN].Val1==VecBeg_FldNm){break;}
    FldN++;
  }
  // extract field-value
  if (FldN==Flds){
    FldValV.Clr();
  } else {
    TStr _FldNm; GetVecFld(FldN, _FldNm, FldValV, FldNmValPrV);
  }
}

int TSoap::GetVecFld(
 const int& _FldN, TStr& FldNm, TStrV& FldValV, const TStrPrV& FldNmValPrV){
  IAssert(IsVecFld(_FldN, FldNmValPrV));
  int Flds=FldNmValPrV.Len();
  int FldN=_FldN;
  // extract field-name
  FldNm=FldNmValPrV[FldN].Val1;
  FldNm=FldNm.GetSubStr(0, FldNm.Len()-TStr("_VecBeg").Len()-1);
  // extract field-value
  TStr VecEnd_FldNm=FldNm+"_VecEnd";
  FldValV.Clr(); FldN++;
  while (FldN<Flds){
    if (FldNmValPrV[FldN].Val1==VecEnd_FldNm){FldN++; break;}
    else {FldValV.Add(FldNmValPrV[FldN].Val2);}
    FldN++;
  }
  return FldN;
}

bool TSoap::IsTbFld(const int& FldN, const TStrPrV& FldNmValPrV){
  return FldNmValPrV[FldN].Val1.IsSuffix("_TbBeg");
}

bool TSoap::IsTbFld(const TStr& FldNm, const TStrPrV& FldNmValPrV){
  TStr BTbFldNm=FldNm+"_TbBeg";
  int Flds=FldNmValPrV.Len();
  for (int FldN=0; FldN<Flds; FldN++){
    if (FldNmValPrV[FldN].Val1==BTbFldNm){return true;}
  }
  return false;
}

void TSoap::AddTbFld(const TStr& TbFldNm, const PSoapTb& SoapTb, TStrPrV& FldNmValPrV){
  int Rows=SoapTb->GetRows();
  // open table
  FldNmValPrV.Add(TStrPr(TbFldNm+"_TbBeg", TInt::GetStr(SoapTb->GetRows())));
  for (int RowN=0; RowN<Rows; RowN++){
    // open row
    FldNmValPrV.Add(TStrPr(TbFldNm+"_TbRowBeg_"+TInt::GetStr(RowN), TStr()));
    // add coloumn-values
    int RowVals=SoapTb->GetRowVals(RowN); TStr ColNm; TStr ValStr;
    for (int RowValN=0; RowValN<RowVals; RowValN++){
      SoapTb->GetColNmVal(RowN, RowValN, ColNm, ValStr);
      FldNmValPrV.Add(TStrPr(TbFldNm+"_"+ColNm+"_"+TInt::GetStr(RowN), ValStr));
    }
    // close row
    FldNmValPrV.Add(TStrPr(TbFldNm+"_TbRowEnd_"+TInt::GetStr(RowN), TStr()));
  }
  // close table
  FldNmValPrV.Add(TStrPr(TbFldNm+"_TbEnd", TInt::GetStr(SoapTb->GetRows())));
}

void TSoap::GetTbFld(const TStr& TbFldNm, PSoapTb& SoapTb, const TStrPrV& FldNmValPrV){
  // search begin-of-field
  TStr TbBeg_FldNm=TbFldNm+"_TbBeg";
  int Flds=FldNmValPrV.Len(); int FldN=0;
  while (FldN<Flds){
    if (FldNmValPrV[FldN].Val1==TbBeg_FldNm){break;}
    FldN++;
  }
  // extract field-value
  if (FldN==Flds){
    SoapTb=TSoapTb::New();
  } else {
    TStr _FldNm; GetTbFld(FldN, _FldNm, SoapTb, FldNmValPrV);
  }
}

int TSoap::GetTbFld(
 const int& _FldN, TStr& TbFldNm, PSoapTb& SoapTb, const TStrPrV& FldNmValPrV){
  IAssert(IsTbFld(_FldN, FldNmValPrV));
  int Flds=FldNmValPrV.Len();
  int FldN=_FldN;
  // extract field-name
  TbFldNm=FldNmValPrV[FldN].Val1;
  TbFldNm=TbFldNm.GetSubStr(0, TbFldNm.Len()-TStr("_TbBeg").Len()-1);
  // extract field-value
  TStr TbEnd_FldNm=TbFldNm+"_TbEnd";
  TStr TbRowBeg_FldNm=TbFldNm+"_TbRowBeg_"; 
  TStr TbRowEnd_FldNm=TbFldNm+"_TbRowEnd_";
  SoapTb=TSoapTb::New(); FldN++;
  while (FldN<Flds){
    // get field-name & field-value
    TStr FldNm=FldNmValPrV[FldN].Val1;
    TStr FldVal=FldNmValPrV[FldN].Val2;
    // process field
    if (FldNm==TbEnd_FldNm){FldN++; break;}
    else if (FldNm.IsStrIn(TbRowBeg_FldNm)){SoapTb->AddRow();}
    else if (FldNm.IsStrIn(TbRowEnd_FldNm)){}
    else {
      TStr FNmPrefix=TbFldNm+"_";
      TStr FNmSuffix="_"+TInt::GetStr(SoapTb->GetRows()-1);
      if (FldNm.IsPrefix(FNmPrefix)&&FldNm.IsSuffix(FNmSuffix)){
        FldNm=FldNm.GetSubStr(FNmPrefix.Len(), FldNm.Len()-1-FNmSuffix.Len());
      } else {WarnNotify("Invalid Field-Name Prefix/Suffix in Soap Table "+TbFldNm);}
      SoapTb->AddVal(FldNm, FldVal);
    }
    FldN++;
  }
  return FldN;
}

/////////////////////////////////////////////////
// Soap-Request
PSoapRq TSoapRq::New(const TStr& FuncNm,
 const TStr& FldNm, const TStr& FldVal){
  PSoapRq SoapRq=TSoapRq::New(FuncNm);
  SoapRq->AddFldNmVal(FldNm, FldVal);
  return SoapRq;
}

PSoapRq TSoapRq::New(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2){
  PSoapRq SoapRq=TSoapRq::New(FuncNm);
  SoapRq->AddFldNmVal(FldNm1, FldVal1);
  SoapRq->AddFldNmVal(FldNm2, FldVal2);
  return SoapRq;
}

PSoapRq TSoapRq::New(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2,
 const TStr& FldNm3, const TStr& FldVal3){
  PSoapRq SoapRq=TSoapRq::New(FuncNm);
  SoapRq->AddFldNmVal(FldNm1, FldVal1);
  SoapRq->AddFldNmVal(FldNm2, FldVal2);
  SoapRq->AddFldNmVal(FldNm3, FldVal3);
  return SoapRq;
}

PSoapRq TSoapRq::New(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2,
 const TStr& FldNm3, const TStr& FldVal3,
 const TStr& FldNm4, const TStr& FldVal4){
  PSoapRq SoapRq=TSoapRq::New(FuncNm);
  SoapRq->AddFldNmVal(FldNm1, FldVal1);
  SoapRq->AddFldNmVal(FldNm2, FldVal2);
  SoapRq->AddFldNmVal(FldNm3, FldVal3);
  SoapRq->AddFldNmVal(FldNm4, FldVal4);
  return SoapRq;
}

PSoapRq TSoapRq::New(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2,
 const TStr& FldNm3, const TStr& FldVal3,
 const TStr& FldNm4, const TStr& FldVal4,
 const TStr& FldNm5, const TStr& FldVal5){
  PSoapRq SoapRq=TSoapRq::New(FuncNm);
  SoapRq->AddFldNmVal(FldNm1, FldVal1);
  SoapRq->AddFldNmVal(FldNm2, FldVal2);
  SoapRq->AddFldNmVal(FldNm3, FldVal3);
  SoapRq->AddFldNmVal(FldNm4, FldVal4);
  SoapRq->AddFldNmVal(FldNm5, FldVal5);
  return SoapRq;
}

void TSoapRq::AddFldNmVal(const TStr& FldNm, const TStr& FldVal){
  for (int FldN=0; FldN<FldNmValPrV.Len(); FldN++){
    if (FldNmValPrV[FldN].Val1==FldNm){
      FldNmValPrV[FldN].Val2=FldVal; return;
    }
  }
  FldNmValPrV.Add(TStrPr(FldNm, FldVal));
}

bool TSoapRq::IsFldNm(const TStr& FldNm) const {
  int Flds=GetFlds();
  TStr _FldNm; TStr FldVal;
  for (int FldN=0; FldN<Flds; FldN++){
    GetFldNmVal(FldN, _FldNm, FldVal);
    if (FldNm==_FldNm){return true;}
  }
  return false;
}

bool TSoapRq::IsFldNmVal(const TStr& FldNm, const TStr& FldVal) const {
  int Flds=GetFlds();
  TStr _FldNm; TStr _FldVal;
  for (int FldN=0; FldN<Flds; FldN++){
    GetFldNmVal(FldN, _FldNm, _FldVal);
    if ((FldNm==_FldNm)&&(FldVal==_FldVal)){return true;}
  }
  return false;
}

TStr TSoapRq::GetFldVal(const TStr& FldNm) const {
  int Flds=GetFlds();
  TStr _FldNm; TStr FldVal;
  for (int FldN=0; FldN<Flds; FldN++){
    GetFldNmVal(FldN, _FldNm, FldVal);
    if (FldNm==_FldNm){return FldVal;}
  }
  return "";
}

TSoapRq::TSoapRq(const PHttpRq& HttpRq):
  Ok(false){
  if (HttpRq->IsOk()){
    if (HttpRq->IsContType(THttp::AppSoapXmlFldVal)){
      TStr XmlStr=HttpRq->GetBodyAsStr();
      bool _FaultP; TStr _FaultCodeNm; TStr _FaultReasonStr;
      TSoap::GetFromXmlStr(
       XmlStr, false, Ok, _FaultP,
       FuncNm, FldNmValPrV, _FaultCodeNm, _FaultReasonStr);
    } else {
      PUrl Url=HttpRq->GetUrl();
      if (Url->GetPathSegs()>0){
        FuncNm=Url->GetPathSeg(Url->GetPathSegs()-1);}
      if (FuncNm.Empty()){
        // empty url means info
        FuncNm="Info";
      } else {
        // fill the fields from search part of url
        PUrlEnv UrlEnv=HttpRq->GetUrlEnv();
        for (int KeyN=0; KeyN<UrlEnv->GetKeys(); KeyN++){
          TStr FldNm=UrlEnv->GetKeyNm(KeyN);
          TStr FldVal=UrlEnv->GetVal(FldNm);
          FldNmValPrV.Add(TStrPr(FldNm, FldVal));
        }
      }
      Ok=true;
    }
  }
}

/////////////////////////////////////////////////
// Soap-Response
TSoapResp::TSoapResp(const PHttpResp& HttpResp):
  Ok(false){
  if (HttpResp->IsOk()){
    if (HttpResp->IsContType(THttp::AppSoapXmlFldVal)){
      TStr XmlStr=HttpResp->GetBodyAsStr();
      TSoap::GetFromXmlStr(
       XmlStr, true, Ok, FaultP,
       FuncNm, FldNmValPrV, FaultCodeNm, FaultReasonStr);
    }
  }
}

void TSoapResp::AddFldNmVal(const TStr& FldNm, const TStr& FldVal){
  for (int FldN=0; FldN<FldNmValPrV.Len(); FldN++){
    if (FldNmValPrV[FldN].Val1==FldNm){
      FldNmValPrV[FldN].Val2=FldVal; return;
    }
  }
  FldNmValPrV.Add(TStrPr(FldNm, FldVal));
}

void TSoapResp::GetFldNmValKdV(TStrKdV& FldNmValKdV) const {
  FldNmValKdV.Clr();
  for (int FldN=0; FldN<GetFlds(); FldN++){
    TStr FldNm; TStr FldVal; GetFldNmVal(FldN, FldNm, FldVal);
    FldNmValKdV.Add(TStrKd(FldNm, FldVal));
  }
}

bool TSoapResp::IsFldNm(const TStr& FldNm) const {
  int Flds=GetFlds();
  TStr _FldNm; TStr FldVal;
  for (int FldN=0; FldN<Flds; FldN++){
    GetFldNmVal(FldN, _FldNm, FldVal);
    if (FldNm==_FldNm){return true;}
  }
  return false;
}

TStr TSoapResp::GetFldVal(const TStr& FldNm) const {
  int Flds=GetFlds();
  TStr _FldNm; TStr FldVal;
  for (int FldN=0; FldN<Flds; FldN++){
    GetFldNmVal(FldN, _FldNm, FldVal);
    if (FldNm==_FldNm){return FldVal;}
  }
  return "";
}

TStr TSoapResp::GetRespStr() const {
  if (IsOk()){
    if (IsResult()){
      return GetRespStr(FuncNm, FldNmValPrV);
    } else {
      return GetFaultRespStr(FaultCodeNm, FaultReasonStr);
    }
  } else {
    return GetFaultRespStr(TSoap::DataEncodingUnknownCodeNm, "Unknown Error.");
  }
}

TStr TSoapResp::GetAsUrlPathStr() const {
  if (IsResult()){
    return TSoap::GetUrlPathStr(FuncNm, true, FldNmValPrV);
  } else
  if (IsFault()){
    return TSoap::GetUrlPathStr("Fault", true,
     "FaultCode", GetFaultCodeNm(),
     "FaultReason", GetFaultReasonStr());
  } else {
    return TSoap::GetUrlPathStr("Fault", true);
  }
}

PHttpResp TSoapResp::GetHttpResp(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2,
 const TStr& FldNm3, const TStr& FldVal3){
  PSoapResp SoapResp=TSoapResp::New(FuncNm);
  SoapResp->AddFldNmVal(FldNm1, FldVal1);
  SoapResp->AddFldNmVal(FldNm2, FldVal2);
  SoapResp->AddFldNmVal(FldNm3, FldVal3);
  return SoapResp->GetHttpResp();
}

PHttpResp TSoapResp::GetHttpResp(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2,
 const TStr& FldNm3, const TStr& FldVal3,
 const TStr& FldNm4, const TStr& FldVal4){
  PSoapResp SoapResp=TSoapResp::New(FuncNm);
  SoapResp->AddFldNmVal(FldNm1, FldVal1);
  SoapResp->AddFldNmVal(FldNm2, FldVal2);
  SoapResp->AddFldNmVal(FldNm3, FldVal3);
  SoapResp->AddFldNmVal(FldNm4, FldVal4);
  return SoapResp->GetHttpResp();
}

PHttpResp TSoapResp::GetHttpResp(const TStr& FuncNm,
 const TStr& FldNm1, const TStr& FldVal1,
 const TStr& FldNm2, const TStr& FldVal2,
 const TStr& FldNm3, const TStr& FldVal3,
 const TStr& FldNm4, const TStr& FldVal4,
 const TStr& FldNm5, const TStr& FldVal5){
  PSoapResp SoapResp=TSoapResp::New(FuncNm);
  SoapResp->AddFldNmVal(FldNm1, FldVal1);
  SoapResp->AddFldNmVal(FldNm2, FldVal2);
  SoapResp->AddFldNmVal(FldNm3, FldVal3);
  SoapResp->AddFldNmVal(FldNm4, FldVal4);
  SoapResp->AddFldNmVal(FldNm5, FldVal5);
  return SoapResp->GetHttpResp();
}

