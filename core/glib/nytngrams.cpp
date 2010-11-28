/////////////////////////////////////////////////
// Includes
#include "nytngrams.h"

/////////////////////////////////////////////////
// NYTimes-NGram-Base

bool TNytNGramBs::IsNGram(const TStrV& TermStrV, int& NGramId) const {
  NGramId=TermStrVH.GetKeyId(TermStrV);
  return NGramId!=-1;
}

TStr TNytNGramBs::GetNGramStr(const int& NGramId) const {
  const TStrV& TermStrV=TermStrVH.GetKey(NGramId);
  TChA NGramChA;
  for (int TermStrN=0; TermStrN<TermStrV.Len(); TermStrN++){
    if (TermStrN>0){NGramChA+=' ';}
    NGramChA+=TermStrV[TermStrN];
  }
  return NGramChA;
}

void TNytNGramBs::GetNGramStrV(const TStr& HtmlStr, TStrV& NGramStrV){
  NGramStrV.Clr();
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);
  // process text
  TStrV StrV;
  while (HtmlLx.GetSym()!=hsyEof){
    TStr Str=HtmlLx.ChA;
    Str.ToLc();
    switch (HtmlLx.Sym){
      case hsyUndef:
      case hsyUrl:
      case hsyMTag:
      case hsySSym:
        StrV.Clr();
        break;
      case hsyStr:
      case hsyNum:
        NGramStrV.Add(Str);
        StrV.Add(Str);
        for (int NGramLen=2; NGramLen<=4; NGramLen++){
          if (StrV.Len()<NGramLen){break;}
          TStrV TermStrV(NGramLen, 0);
          for (int StrN=StrV.Len()-NGramLen; StrN<StrV.Len(); StrN++){
            TermStrV.Add(StrV[StrN]);
          }
          int NGramId;
          if (IsNGram(TermStrV, NGramId)){
            TStr NGramStr=GetNGramStr(NGramId);
            NGramStrV.Add(NGramStr);
          }
        } 
        break;
      case hsyBTag:
      case hsyETag:
        StrV.Clr();
        break;
      case hsyEof: break;
      default: Fail;
    }
  }
}

PNytNGramBs TNytNGramBs::LoadNytNGramBs(const TStr& InNGramsFPath, const int& ExpectedNGrams){
  // prepare filenames
  TStr InNGramsNrFPath=TStr::GetNrFPath(InNGramsFPath);
  TStr LexiconFNm=InNGramsNrFPath+"Lexicon.Csv";
  TStr Coloc2FNm=InNGramsNrFPath+"significant_colocations_2.bin";
  TStr Coloc3FNm=InNGramsNrFPath+"significant_colocations_3.bin";
  TStr Coloc4FNm=InNGramsNrFPath+"significant_colocations_4.bin";
  
  // load lexicon
  printf("Loading Lexicon from %s ...\n", LexiconFNm.CStr());
  PSs LexiconSs=TSs::LoadTxt(ssfCommaSep, LexiconFNm, TNotify::StdNotify, false);
  printf("Done.");

  // create ngrams-base  
  PNytNGramBs NytNGramBs=TNytNGramBs::New(ExpectedNGrams);

  // add lexicon terms
  int Terms=LexiconSs->GetYLen();
  TIntStrH TermIdToStrH(Terms*2);
  for (int TermN=0; TermN<Terms; TermN++){
    int TermId=LexiconSs->At(0, TermN).GetInt();
    TStr TermStr=LexiconSs->At(1, TermN);
    //printf("[%d:%s] ", TermId, TermStr.CStr());
    TermIdToStrH.AddDat(TermId, TermStr);
    TStrV TermStrV(1, 0); TermStrV.Add(TermStr); 
    NytNGramBs->TermStrVH.AddDat(TermStrV);
  }
  
  // collocations od lenght 2  
  {printf("Load Collocations from '%s' ...\n", Coloc2FNm.CStr());
  FILE* fColoc=fopen(Coloc2FNm.CStr(), "rb");
  int Colocs=0; int BadColocs=0;
  while (!feof(fColoc)){
    Colocs++; if (Colocs%1000==0){printf("%d\r", Colocs);}
    int TermId1=-1; int TermId2=-1; 
    int CorpusFq=-1; int DocFq=-1; double Sig=-1;
    fread(&TermId1, 4, 1, fColoc); TermId1=TUInt::JavaUIntToCppUInt(TermId1);
    fread(&TermId2, 4, 1, fColoc); TermId2=TUInt::JavaUIntToCppUInt(TermId2);
    fread(&CorpusFq, 4, 1, fColoc);
    fread(&DocFq, 4, 1, fColoc);
    fread(&Sig, 8, 1, fColoc);
    TStr TermStr1; TStr TermStr2; 
    if (TermIdToStrH.IsKeyGetDat(TermId1, TermStr1)){
      if (TermIdToStrH.IsKeyGetDat(TermId2, TermStr2)){
        TStrV TermStrV(2, 0); TermStrV.Add(TermStr1); TermStrV.Add(TermStr2);
        NytNGramBs->TermStrVH.AddDat(TermStrV);
        //printf("[%s %s] ", TermStr1.CStr(), TermStr2.CStr());
      } else {BadColocs++;}
    } else {BadColocs++;}
  }
  printf("Colocations:%d Bad Collocations:%d\nDone.\n", Colocs, BadColocs);}
  // collocations od lenght 3  
  {printf("Load Collocations from '%s' ...\n", Coloc3FNm.CStr());
  FILE* fColoc=fopen(Coloc3FNm.CStr(), "rb");
  int Colocs=0; int BadColocs=0;
  while (!feof(fColoc)){
    Colocs++; if (Colocs%1000==0){printf("%d\r", Colocs);}
    int TermId1=-1; int TermId2=-1; int TermId3=-1;
    int CorpusFq=-1; int DocFq=-1; double Sig=-1;
    fread(&TermId1, 4, 1, fColoc); TermId1=TUInt::JavaUIntToCppUInt(TermId1);
    fread(&TermId2, 4, 1, fColoc); TermId2=TUInt::JavaUIntToCppUInt(TermId2);
    fread(&TermId3, 4, 1, fColoc); TermId3=TUInt::JavaUIntToCppUInt(TermId3);
    fread(&CorpusFq, 4, 1, fColoc);
    fread(&DocFq, 4, 1, fColoc);
    fread(&Sig, 8, 1, fColoc);
    TStr TermStr1; TStr TermStr2; TStr TermStr3; 
    if (TermIdToStrH.IsKeyGetDat(TermId1, TermStr1)){
      if (TermIdToStrH.IsKeyGetDat(TermId2, TermStr2)){
        if (TermIdToStrH.IsKeyGetDat(TermId3, TermStr3)){
          TStrV TermStrV(3, 0); 
          TermStrV.Add(TermStr1); TermStrV.Add(TermStr2); TermStrV.Add(TermStr3);
          NytNGramBs->TermStrVH.AddDat(TermStrV);
          //printf("[%s %s %s] ", TermStr1.CStr(), TermStr2.CStr(), TermStr3.CStr());
        } else {BadColocs++;}
      } else {BadColocs++;}
    } else {BadColocs++;}
  }
  printf("Colocations:%d Bad Collocations:%d\nDone.\n", Colocs, BadColocs);}
  // collocations od lenght 4  
  {printf("Load Collocations from '%s' ...\n", Coloc4FNm.CStr());
  FILE* fColoc=fopen(Coloc4FNm.CStr(), "rb");
  int Colocs=0; int BadColocs=0;
  while (!feof(fColoc)){
    Colocs++; if (Colocs%1000==0){printf("%d\r", Colocs);}
    int TermId1=-1; int TermId2=-1; int TermId3=-1; int TermId4=-1; 
    int CorpusFq=-1; int DocFq=-1; double Sig=-1;
    fread(&TermId1, 4, 1, fColoc); TermId1=TUInt::JavaUIntToCppUInt(TermId1);
    fread(&TermId2, 4, 1, fColoc); TermId2=TUInt::JavaUIntToCppUInt(TermId2);
    fread(&TermId3, 4, 1, fColoc); TermId3=TUInt::JavaUIntToCppUInt(TermId3);
    fread(&TermId4, 4, 1, fColoc); TermId4=TUInt::JavaUIntToCppUInt(TermId4);
    fread(&CorpusFq, 4, 1, fColoc);
    fread(&DocFq, 4, 1, fColoc);
    fread(&Sig, 8, 1, fColoc);
    TStr TermStr1; TStr TermStr2; TStr TermStr3; TStr TermStr4; 
    if (TermIdToStrH.IsKeyGetDat(TermId1, TermStr1)){
      if (TermIdToStrH.IsKeyGetDat(TermId2, TermStr2)){
        if (TermIdToStrH.IsKeyGetDat(TermId3, TermStr3)){
          if (TermIdToStrH.IsKeyGetDat(TermId4, TermStr4)){
            TStrV TermStrV(4, 0); 
            TermStrV.Add(TermStr1); TermStrV.Add(TermStr2); 
            TermStrV.Add(TermStr3); TermStrV.Add(TermStr4);
            NytNGramBs->TermStrVH.AddDat(TermStrV);
            //printf("[%s %s %s %s] ", TermStr1.CStr(), TermStr2.CStr(), TermStr3.CStr(), TermStr4.CStr());
          } else {BadColocs++;}
        } else {BadColocs++;}
      } else {BadColocs++;}
    } else {BadColocs++;}
  }
  printf("Colocations:%d Bad Collocations:%d\nDone.\n", Colocs, BadColocs);}
  printf("All Collocations:%d\n", NytNGramBs->TermStrVH.Len());

  // return ngrams
  return NytNGramBs;
}

void TNytNGramBs::SaveTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  int NGrams=TermStrVH.Len();
  for (int NGramId=0; NGramId<NGrams; NGramId++){
    TStr NGramStr=GetNGramStr(NGramId);
    fprintf(fOut, "%d\t%s\n", NGramId, NGramStr.CStr());
  }
}
