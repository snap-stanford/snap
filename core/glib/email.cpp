/////////////////////////////////////////////////
// Includes
#include "email.h"

/////////////////////////////////////////////////
// EMail-Alias-Base
PEMailAliasBs TEMailAliasBs::LoadSsTxt(const TStr& FNm){
  PSs Ss=TSs::LoadTxt(ssfTabSep, FNm);
  PEMailAliasBs EMailAliasBs=TEMailAliasBs::New();
  for (int Y=0; Y<Ss->GetYLen(); Y++){
    TStr PersonNm=Ss->At(0, Y).GetTrunc();
    TStr DepNm=Ss->At(1, Y).GetTrunc().GetLc();
    for (int X=2; X<Ss->GetXLen(Y); X++){
      TStr EAddr=Ss->At(X, Y).GetTrunc().GetLc();
      if (!PersonNm.Empty()&&!DepNm.Empty()&&!EAddr.Empty()){
        EMailAliasBs->EAddrToPersonNmDepNmPrH.AddDat(EAddr, TStrPr(PersonNm, DepNm));
      }
    }
  }
  return EMailAliasBs;
}

/////////////////////////////////////////////////
// EMail-Transaction-Base
bool TEMailTsactBs::IsEAddrOk(const TStr& EAddr){
  TChA EAddrChA=EAddr;
  EAddrChA.ToLc();
  for (int ChN=0; ChN<EAddrChA.Len(); ChN++){
    char Ch=EAddrChA[ChN];
    if ((('a'<=Ch)&&(Ch<='z'))||(('0'<=Ch)&&(Ch<='9'))||
     (Ch=='@')||(Ch=='.')||(Ch=='-')||(Ch=='_')){
    } else {
      //printf("%s\n", EAddr.CStr());
      return false;
    }
  }
  return true;
}

int TEMailTsactBs::AddTsact(const TStr& SrcEAddr, const TStrV& DstEAddrV, const TTm& Tm){
  // get source email-address-id
  int SrcEAddrId=EAddrStrToSrcFqDstFqPrH.AddKey(SrcEAddr);
  // get destionation email-address-id-vector
  TIntV DstEAddrIdV(DstEAddrV.Len(), 0);
  for (int DstEAddrN=0; DstEAddrN<DstEAddrV.Len(); DstEAddrN++){
    int DstEAddrId=EAddrStrToSrcFqDstFqPrH.AddKey(DstEAddrV[DstEAddrN]);
    DstEAddrIdV.Add(DstEAddrId);
  }
  // create transaction
  TEMailTsact Tsact(SrcEAddrId, DstEAddrIdV, Tm);
  // add transaction and return id
  return EMailTsactV.Add(Tsact);
}

/////////////////////////////////////////////////
// EMail-Message
TTm TEml::GetTmFromRfc2822DateTimeStr(const TStr& DateTimeStr){
  TChA DateTimeChA=DateTimeStr;
  int DateTimeChALen=DateTimeChA.Len();
  // whitespace
  TChA ChA; int ChN=0;
  while ((ChN<DateTimeChALen)&&isspace(DateTimeChA[ChN])){ChN++;}
  // day-of-week + comma + white space
  if ((ChN<DateTimeChALen)&&isalpha(DateTimeChA[ChN])){
    ChA.Clr();
    while ((ChN<DateTimeChALen)&&((DateTimeChA[ChN]!=',')&&(DateTimeChA[ChN]!=' '))){
      ChA+=DateTimeChA[ChN]; ChN++;}
    if ((ChN<DateTimeChALen)&&DateTimeChA[ChN]==','){ChN++;}
    while ((ChN<DateTimeChALen)&&isspace(DateTimeChA[ChN])){ChN++;}
  }
  // day
  ChA.Clr(); 
  while ((ChN<DateTimeChALen)&&(DateTimeChA[ChN]!=' ')){
    ChA+=DateTimeChA[ChN]; ChN++;}
  TStr DayStr=ChA;
  // month
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeChALen)&&(DateTimeChA[ChN]!=' ')){
    ChA+=DateTimeChA[ChN]; ChN++;}
  TStr MonthStr=ChA;
  // year
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeChALen)&&(DateTimeChA[ChN]!=' ')){
    ChA+=DateTimeChA[ChN]; ChN++;}
  TStr YearStr=ChA;
  // hour
  ChA.Clr(); ChN++;
  while ((ChN<DateTimeChALen)&&(isdigit(DateTimeChA[ChN]))){
    ChA+=DateTimeChA[ChN]; ChN++;}
  TStr HourStr=ChA;
  // minute
  TStr MinStr;
  if ((ChN<DateTimeChALen)&&(DateTimeStr[ChN]==':')){
    ChA.Clr(); ChN++;
    while ((ChN<DateTimeChALen)&&(isdigit(DateTimeChA[ChN]))){
      ChA+=DateTimeStr[ChN]; ChN++;}
  }
  MinStr=ChA;
  // second
  TStr SecStr;
  if ((ChN<DateTimeChALen)&&(DateTimeStr[ChN]==':')){
    ChA.Clr(); ChN++;
    while ((ChN<DateTimeChALen)&&isdigit(DateTimeStr[ChN])){
      ChA+=DateTimeStr[ChN]; ChN++;}
    SecStr=ChA;
  }
  // zone
  while ((ChN<DateTimeChALen)&&isspace(DateTimeChA[ChN])){ChN++;}
  TStr ZoneStr;
  if ((ChN<DateTimeChALen)&&((DateTimeStr[ChN]=='+')||(DateTimeStr[ChN]=='-'))){
    ChA.Clr(); 
    ChA+=DateTimeStr[ChN];
    ChN++;
    while ((ChN<DateTimeChALen)&&(isdigit(DateTimeStr[ChN]))){
      ChA+=DateTimeStr[ChN]; ChN++;}
    ZoneStr=ChA;
  } else
  if ((ChN<DateTimeChALen)&&isalpha(DateTimeStr[ChN])){
    ChA.Clr(); 
    while ((ChN<DateTimeChALen)&&(isalpha(DateTimeChA[ChN]))){
      ChA+=DateTimeStr[ChN]; ChN++;}
    ZoneStr=ChA;
  }
  // exceptions
  // day and month have changed order: " fri, jan 18 2002 08:13:47 -0800"
  if (TTmInfo::IsMonthNm(DayStr)){
    TStr MonthStr1=DayStr; DayStr=MonthStr; MonthStr=MonthStr1;}
  // transform to numbers
  int YearN=YearStr.GetInt(-1); if ((0<=YearN)&&(YearN<=100)){YearN+=1900;}
  int MonthN=TTmInfo::GetMonthN(MonthStr);
  int DayN=DayStr.GetInt(-1);
  int HourN=HourStr.GetInt(0);
  int MinN=MinStr.GetInt(0);
  int SecN=SecStr.GetInt(0);
  char ZoneSign=+1;
  int ZoneMins=0;
  if (!ZoneStr.Empty()&&!ZoneStr.IsInt()){
    ZoneStr=TTmInfo::GetTmZoneDiffStr(ZoneStr.GetUc());
  }
  if (!ZoneStr.Empty()&&ZoneStr.IsInt()){
    ZoneMins=ZoneStr.GetSubStr(1, 2).GetInt(-1)*60+ZoneStr.GetSubStr(3, 4).GetInt(-1);
    if (ZoneStr[0]=='-'){ZoneSign=-1;}
  }
  // construct time
  TTm Tm;
  if ((YearN!=-1)&&(MonthN!=-1)&&(DayN!=-1)){
    Tm=TTm(YearN, MonthN, DayN, -1, HourN, MinN, SecN, 0);
    TStr S1=Tm.GetWebLogDateTimeStr(true);
    if (ZoneSign>0){Tm.AddTime(0, ZoneMins, 0, 0);}
    else {Tm.SubTime(0, ZoneMins, 0, 0);}
    TStr S2=Tm.GetWebLogDateTimeStr(true);
  }
  // return time
  return Tm;
}

void TEml::ExtrMergeToAddrStrV(const TStr& FldVal, TStrV& AddrStrV){
  TStrV CandAddrStrV; FldVal.SplitOnAllAnyCh("\x09 ,;", CandAddrStrV);
  for (int CandN=0; CandN<CandAddrStrV.Len(); CandN++){
    TStr CandAddrStr=CandAddrStrV[CandN];
    if (!CandAddrStr.IsChIn('@')){continue;}
    char FirstCh=CandAddrStr[0]; char LastCh=CandAddrStr.LastCh();
    if ((FirstCh=='<')&&(LastCh=='>')){
      CandAddrStr=CandAddrStr.GetSubStr(1, CandAddrStr.Len()-2);
    }
    if (!isalnum(CandAddrStr[0])){continue;}
    AddrStrV.AddUnique(CandAddrStr);
  }
}

bool TEml::IsFieldNmCh(const uchar& Ch) const {
  // field-name  =  1*<any CHAR, excluding CTLs, SPACE, and ":">
  // The  field-name must be composed of printable ASCII characters
  // i.e., characters that  have  values  between  33.  and  126.,
  // decimal, except colon).
  return (32<=Ch)&&(Ch<=126)&&(Ch!=':');
}

void TEml::ToNrEncoding(const TStr& ContTransferEncVal, TChA& ChA){
  if (ContTransferEncVal=="base64"){
    ChA.Clr();
  } else
  if (ContTransferEncVal=="quoted-printable"){
    TChA NrChA(ChA.Len()); int ChN=0;
    while (ChN<ChA.Len()){
      if ((ChA[ChN]=='=')&&(ChN+2<ChA.Len())){
        ChN++; char HexCh1=ChA[ChN++]; char HexCh2=ChA[ChN++];
        if ((TCh::IsHex(HexCh1))&&(TCh::IsHex(HexCh2))){
          int ChVal=16*TCh::GetHex(HexCh1)+TCh::GetHex(HexCh2);
          NrChA.AddCh(ChVal);
        }
      } else {
        NrChA.AddCh(ChA[ChN]); ChN++;
      }
    }
    ChA=NrChA;
  }
}

void TEml::ExtrAddBoundaryStr(const TStr& ContTypeVal, TStrV& BoundaryStrV) const {
  TStr FldStr="boundary=";
  int FldChN=ContTypeVal.SearchStr(FldStr);
  TStr BoundaryStr;
  if (FldChN==-1){
    BoundaryStr="";
  } else {
    TChA ChA=ContTypeVal.GetSubStr(FldChN+FldStr.Len(), ContTypeVal.Len());
    if ((ChA.Len()>0)&&(ChA[0]=='\"')){ChA.Del(0);}
    int ChN=0;
    while ((ChN<ChA.Len())&&(ChA[ChN]!='\"')){ChN++;}
    ChA.Trunc(ChN);
    BoundaryStr=ChA;
  }
  if (!BoundaryStr.Empty()){
    BoundaryStrV.AddUnique(BoundaryStr);}
}

bool TEml::LoadField(const PSIn& SIn, TChA& FldNm, TChA& FldVal, TChA& LcFldVal){
  FldNm.Clr(); FldVal.Clr(); LcFldVal.Clr();
  // field name
  while (IsFieldNmCh(SIn->PeekCh())){
    FldNm+=tolower(SIn->GetCh());} 
  if (FldNm.Empty()){return false;}
  // colon
  if (SIn->PeekCh()!=':'){return false;} else {SIn->GetCh();}
  // field value
  forever{
    if (SIn->PeekCh()==13){
      SIn->GetCh(); 
      if (SIn->PeekCh()==10){
        SIn->GetCh();
        if ((SIn->PeekCh()==32)||(SIn->PeekCh()==9)){
        } else {
          break;
        }
      }
    }
    FldVal+=SIn->GetCh();
  }
  LcFldVal=FldVal; LcFldVal.ToLc();
  return true;
}

void TEml::LoadMessage(const PSIn& SIn){
  // parse header
  TChA FldNm; TChA FldVal; TChA LcFldVal;
  TStr ContTypeVal; TStr LcContTypeVal;
  TStr ContTransferEncVal; 
  TStrV BoundaryStrV;
  TChA SubjChA;
  forever {
    bool IsFld=LoadField(SIn, FldNm, FldVal, LcFldVal); 
    if (IsFld){
      if (FldNm=="from"){
        ExtrMergeToAddrStrV(LcFldVal, FromAddrStrV);}
      else if ((FldNm=="to")||(FldNm=="cc")||(FldNm=="bcc")){
        ExtrMergeToAddrStrV(LcFldVal, ToAddrStrV);}
      else if (FldNm=="date"){
        Tm=GetTmFromRfc2822DateTimeStr(LcFldVal);
      }
      else if (FldNm=="subject"){
        SubjChA=FldVal;}
      else if (FldNm=="content-type"){
        ContTypeVal=FldVal; LcContTypeVal=LcFldVal;}
      else if (FldNm=="content-transfer-encoding"){
        ContTransferEncVal=LcFldVal; ContTransferEncVal.ToTrunc();}
    } else {
      if (SIn->PeekCh()==13){
        SIn->GetCh(); if (SIn->PeekCh()==10){SIn->GetCh();} else {}
      }
      break;
    }
  }
  ToNrEncoding(ContTransferEncVal, SubjChA); SubjStr=SubjChA;
  // parse body
  TChA ContChA;
  // body (text)
  if (LcContTypeVal.Empty()||LcContTypeVal.IsStrIn("text/plain")||LcContTypeVal.IsStrIn("text/html")){
    TStrV LnStrV; TChA LnChA;
    while (SIn->PeekCh()!=0){
      LnChA+=SIn->GetCh();
      if ((LnChA.LastCh()==10)||(SIn->PeekCh()!=0)){
        ContChA+=LnChA; LnStrV.Add(LnChA.CStr()); LnChA.Clr();}
    }
  } else
  if (LcContTypeVal.IsStrIn("multipart/")){
    // get boundary
    ExtrAddBoundaryStr(ContTypeVal, BoundaryStrV);
    // collect lines
    TStrV LnStrV; TChA LnChA; bool InsLnP=false;
    while (SIn->PeekCh()!=0){
      LnChA+=SIn->GetCh();
      if ((LnChA.LastCh()==10)||(SIn->PeekCh()==0)){
        // add space to assure new-line being white-space
        if ((ContChA.Len()>0)&&(ContChA.LastCh()!=' ')){ContChA+=' ';}
        // check if the line is boundary
        bool IsBoundary=false;
        for (int BoundaryN=0; BoundaryN<BoundaryStrV.Len(); BoundaryN++){
          if (LnChA.IsStrIn(BoundaryStrV[BoundaryN])){
            IsBoundary=true; break;}
        }
        // add line if insertable and not boundary
        if (InsLnP&&!IsBoundary){
          ContChA+=LnChA; LnStrV.Add(LnChA.CStr());} 
        // start new part if boundary
        if (IsBoundary){
          // collect fields
          TStr PartContTypeVal; TStr PartLcContTypeVal; 
          TChA PartFldNm; TChA PartFldVal; TChA PartLcFldVal;
          forever {
            bool IsPartFld=LoadField(SIn, PartFldNm, PartFldVal, PartLcFldVal);
            if (IsPartFld){
              if (PartFldNm=="content-type"){
                PartContTypeVal=PartFldVal; PartLcContTypeVal=PartLcFldVal;
                ExtrAddBoundaryStr(PartContTypeVal, BoundaryStrV);
              } 
              else if (PartFldNm=="content-transfer-encoding"){
                if (ContTransferEncVal.Empty()){
                  ContTransferEncVal=PartLcFldVal; 
                  ContTransferEncVal.ToTrunc();
                }
              }
            } else {
              break;
            }
          }
          // if content-type is text/plain or text/html then enable line-insertion
          InsLnP=PartLcContTypeVal.IsStrIn("text/plain")||PartLcContTypeVal.IsStrIn("text/html");
        }
        // clear current line
        LnChA.Clr();
      }
    }
  }
  // normalise and save content
  ToNrEncoding(ContTransferEncVal, ContChA); 
  ContStr=ContChA; 
}

PEml TEml::LoadEml(const TStr& FNm, const TStr& OwnerNm){
  PSIn SIn=TFIn::New(FNm);
  PEml Eml=TEml::New();
  Eml->OwnerNm=OwnerNm;
  Eml->LoadMessage(SIn);
  return Eml;
}

/////////////////////////////////////////////////
// EMail-Message-Base
void TEmlBs::AddEml(const TStr& EmlNm, const PEml& Eml){
  NmToEmlH.AddDat(EmlNm, Eml);
  int EmlP=NmToEmlH.GetKeyId(EmlNm);
  for (int EAddrN=0; EAddrN<Eml->GetFromEAddrs(); EAddrN++){
    AddEAddr(Eml->GetFromEAddr(EAddrN), EmlP);}
  for (int EAddrN=0; EAddrN<Eml->GetToEAddrs(); EAddrN++){
    AddEAddr(Eml->GetToEAddr(EAddrN), EmlP);}
}

void TEmlBs::DelEml(const TStr& EmlNm){
  PEml Eml=NmToEmlH.GetDat(EmlNm);
  int EmlP=NmToEmlH.GetKeyId(EmlNm);
  for (int EAddrN=0; EAddrN<Eml->GetFromEAddrs(); EAddrN++){
    DelEAddr(Eml->GetFromEAddr(EAddrN), EmlP);}
  for (int EAddrN=0; EAddrN<Eml->GetToEAddrs(); EAddrN++){
    DelEAddr(Eml->GetToEAddr(EAddrN), EmlP);}
  NmToEmlH.DelKey(EmlNm);
}

void TEmlBs::GetTmEmlNmPrV(TTmStrPrV& TmEmlNmPrV) const {
  TStrV EmlIdV; GetEmlNmV(EmlIdV);
  TmEmlNmPrV.Gen(EmlIdV.Len(), 0);
  for (int EmlN=0; EmlN<EmlIdV.Len(); EmlN++){
    TStr EmlNm=EmlIdV[EmlN];
    PEml Eml=GetEml(EmlNm);
    TmEmlNmPrV.Add(TTmStrPr(Eml->GetTm(), EmlNm));
  }
  TmEmlNmPrV.Sort();
}

void TEmlBs::LoadWinMailAccount(const TStr& FPath, const TStr& OwnerNm, const int& MxEmls){
  printf("Processing WinMail-Account at '%s' ...\n", FPath.CStr());
  TStr NrFPath=TStr::GetNrFPath(FPath);
  PFFile FFile=TFFile::New(FPath, "eml", true); 
  TStr EmlFNm; int Emls=0; 
  while (FFile->Next(EmlFNm)){
    Emls++; if ((MxEmls!=-1)&&(Emls>=MxEmls)){break;}
    //if (!EmlFNm.IsStrIn("76F0371A-00022F57")){continue;}
    printf("%d\r", Emls);
    // get eml-id
    TStr EmlNm=EmlFNm;
    if (EmlNm.IsPrefix(NrFPath)){
      EmlNm=EmlNm.GetSubStr(NrFPath.Len(), EmlNm.Len());}
    // load eml-file
    PEml Eml=TEml::LoadEml(EmlFNm, OwnerNm);
    // add eml to base
    if (Eml->GetTm().IsDef()){
      AddEml(EmlNm, Eml);}
  }
  printf("Done.\n");
}

void TEmlBs::LoadEnronEMail(const TStr& FPath, const int& MxEmls){
  printf("Processing Enron-EMail at '%s' ...\n", FPath.CStr());
  TStr NrFPath=TStr::GetNrFPath(FPath);
  PFFile FFile=TFFile::New(FPath, "", true);
  TStr EmlFNm; int Emls=0; 
  while (FFile->Next(EmlFNm)){
    Emls++; if ((MxEmls!=-1)&&(Emls>=MxEmls)){break;}
    //if (!EmlFNm.IsStrIn("zufferli-j/sent_items/96")){continue;}
    printf("%d\r", Emls);
    // get eml-id
    TStr EmlNm=EmlFNm;
    if (EmlNm.IsPrefix(NrFPath)){
      EmlNm=EmlNm.GetSubStr(NrFPath.Len(), EmlNm.Len());}
    // get owner
    int OwnerEChN=EmlNm.SearchCh('/')-1;
    TStr OwnerNm=EmlNm.GetSubStr(0, OwnerEChN);
    // load eml-file
    PEml Eml=TEml::LoadEml(EmlFNm, OwnerNm);
    // add eml to base
    AddEml(EmlNm, Eml);
  }
  printf("\nDone.\n");
}

void TEmlBs::SaveTxt(const TStr& FNm){
  // get time sorted eml-ids
  TTmStrPrV TmEmlNmPrV; GetTmEmlNmPrV(TmEmlNmPrV);
  // output emails
  {TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int EmlN=0; EmlN<TmEmlNmPrV.Len(); EmlN++){
    TStr EmlNm=TmEmlNmPrV[EmlN].Val2;
    PEml Eml=GetEml(EmlNm);
    fprintf(fOut, "==================================\n");
    fprintf(fOut, "Id: %s\n", EmlNm.CStr());
    fprintf(fOut, "Owner: %s\n", Eml->GetOwnerNm().CStr());
    fprintf(fOut, "From:");
    for (int EAddrN=0; EAddrN<Eml->GetFromEAddrs(); EAddrN++){
      fprintf(fOut, " %s", Eml->GetFromEAddr(EAddrN).CStr());
    }
    fprintf(fOut, "\n");
    fprintf(fOut, "To:");
    for (int EAddrN=0; EAddrN<Eml->GetToEAddrs(); EAddrN++){
      fprintf(fOut, " %s", Eml->GetToEAddr(EAddrN).CStr());
    }
    fprintf(fOut, "\n"); 
    fprintf(fOut, "Time: %s\n", Eml->GetTm().GetWebLogDateTimeStr(true).CStr());
    fprintf(fOut, "Subject: %s\n", Eml->GetSubjStr().CStr());
    TChA ContChA=Eml->GetContStr(); 
    ContChA.ChangeCh('\r', ' '); ContChA.ChangeCh('\n', ' ');
    fprintf(fOut, "Content: %s\n", ContChA.CStr());
  }}
}

void TEmlBs::SaveTxtEAddr(const TStr& FNm){
  // collect address-frequency pairs
  TIntStrPrV FqAddrStrPrV; 
  int EAddrs=GetEAddrs();
  for (int EAddrN=0; EAddrN<EAddrs; EAddrN++){
    FqAddrStrPrV.Add(TIntStrPr(GetEAddrFq(EAddrN), GetEAddrStr(EAddrN)));
  }
  // output sorted pairs
  {FqAddrStrPrV.Sort(false);
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int AddrN=0; AddrN<FqAddrStrPrV.Len(); AddrN++){
    fprintf(fOut, "%d. #%d %s\n", 1+AddrN, FqAddrStrPrV[AddrN].Val1, FqAddrStrPrV[AddrN].Val2.CStr());
  }}
}
