/////////////////////////////////////////////////
// Includes
#include "medline.h"

/////////////////////////////////////////////////
// Medline-Web-Fetch
void TMedlineWebFetch::OnFetch(const int&, const PWebPg& WebPg){
  TStr FNm=GetFNm(FPath, MnDocId, MxDocId);
  // save to file
  printf("[%s] ", FNm.GetFBase().CStr());
  TFOut FOut(FNm);
  FOut.PutStr(WebPg->GetHttpBodyAsStr());
  // wait & proceed
  TSysProc::Sleep(WaitSecs*1000);
  TSysMsg::Quit();
}

void TMedlineWebFetch::OnError(const int&, const TStr& MsgStr){
  // output error message
  if (MnDocId==MxDocId){
    printf("[%d: %s] ", MnDocId, MsgStr.CStr());
  } else {
    printf("[%d-%d: %s] ", MnDocId, MxDocId, MsgStr.CStr());
  }
  // wait & proceed
  TSysProc::Sleep(WaitSecs*1000);
  TSysMsg::Quit();
}

TStr TMedlineWebFetch::GetFNm(
 const TStr& FPath, const int& MnDocId, const int& MxDocId){
  TStr FBase;
  if (MnDocId==MxDocId){
    FBase=TInt::GetStr(MnDocId)+".Txt";
  } else {
    FBase=TInt::GetStr(MnDocId, "%08d")+"-"+TInt::GetStr(MxDocId, "%08d")+".Txt";
  }
  // create complete file-name
  TStr FNm=TStr::GetNrFPath(FPath)+FBase;
  // return file-name
  return FNm;
}

int TMedlineWebFetch::GetNextStartDocId(const TStr& FPath){
  // collect maximal numbers from file-names
  TIntV DocIdV;
  TFFile FFile(TStr::GetNrFPath(FPath)+"*.Txt"); TStr FNm;
  while (FFile.Next(FNm)){
    TStr DocIdStr=FNm.GetWcMatch("*-*.Txt", 0);
    if (DocIdStr.IsInt()){
      int DocId=DocIdStr.GetInt();
      DocIdV.Add(DocId);
    }
  }
  DocIdV.Sort();
  // get next start-doc-id
  int NextStartDocId=1;
  if (DocIdV.Len()>0){
    int MxDocId=DocIdV.Last();
    return MxDocId+1;
  }
  // return
  return NextStartDocId;
}

void TMedlineWebFetch::Go(
 const int& StartDocId, const int& Docs, const int& DocSetLen,
 const int& WaitSecs, const TStr& FPath){
  TStr BaseUrlStr=
   "http://www.ncbi.nlm.nih.gov/entrez/utils/pmfetch.fcgi"
   "?db=PubMed&id=ID&report=medline&mode=text";

  for (int DocId=StartDocId; DocId<StartDocId+Docs-1; DocId=DocId+DocSetLen){
    // get minimal & maximal document ids
    int MnDocId=DocId; int MxDocId=DocId+DocSetLen-1;
    // fetch if not exists
    TStr FNm=GetFNm(FPath, MnDocId, MxDocId);
    if (!TFile::Exists(FNm)){
      // prepare document-set string
      TChA DocIdSetChA;
      for (int SetDocId=MnDocId; SetDocId<=MxDocId; SetDocId++){
        if (SetDocId!=MnDocId){DocIdSetChA+=',';}
        DocIdSetChA+=TInt::GetStr(SetDocId);
      }
      // prepare url
      TStr UrlStr=BaseUrlStr;
      UrlStr.ChangeStr("ID", DocIdSetChA);
      // fetch documents
      TMedlineWebFetch WebFetch(MnDocId, MxDocId, WaitSecs, FPath);
      WebFetch.FetchUrl(UrlStr);
      TSysMsg::Loop();
    } else {
      printf(".");
      //printf("[Exists: %s] ", FNm.GetFBase().CStr());
    }
  }
}

