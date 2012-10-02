/////////////////////////////////////////////////
// Includes
#include "cordisold.h"

/////////////////////////////////////////////////
// EuPartner
TStr TCordisEuPart::ExtrCountry(const TStr& AddrStr){
  TStr CountryStr;
  TStrV LnV; AddrStr.SplitOnStr("<br>", LnV);
  if (LnV.Len()>0){
    CountryStr=LnV.Last();
    if (CountryStr.Empty()&&(LnV.Len()>1)){
      CountryStr=LnV[LnV.Len()-2];}
    CountryStr.DelChAll('\r');
    CountryStr.DelChAll('\n');
  }
  if (CountryStr.Empty()){
    printf("Country Field Not Found!\n");}
  return CountryStr;
}

TStr TCordisEuPart::ExtrDeptNm(const TStr& EuPartStr){
  TStr DeptNm=EuPartStr.GetWcMatch("*Department:</span>*</td>*", 1).GetTrunc();
  if (!DeptNm.Empty()){
    DeptNm.ChangeStrAll("\n<BR>", " ");
    DeptNm=TXmlLx::GetPlainStrFromXmlStr(DeptNm);
  }
  return DeptNm;
}

void TCordisEuPart::ExtrEuPartV(const TStr& AllEuPartStr, TCordisEuPartV& EuPartV){
  TStrV EuPartStrV; AllEuPartStr.SplitOnStr("Organisation Type:", EuPartStrV);
  for (int EuPartN=1; EuPartN<EuPartStrV.Len(); EuPartN++){
    TStr EuPartStr=EuPartStrV[EuPartN];
    TStr WcStr=
     "</span>*</td>"
     "*Organisation:</span>*<br>"
     "*</td>*";
    TStrV StarStrV;
    if (EuPartStr.IsWcMatch(WcStr, StarStrV)){
      PCordisEuPart EuPart=TCordisEuPart::New();
      EuPart->DeptNm=ExtrDeptNm(EuPartStr);
      EuPart->OrgNm=TXmlLx::GetPlainStrFromXmlStr(StarStrV[2].GetTrunc());
      EuPart->OrgTypeNm=StarStrV[0].GetTrunc();
      EuPart->CountryNm=ExtrCountry(StarStrV[3].GetTrunc());
      EuPart->CoordP=(EuPartN==1);
      printf("   Partner: '%s'/'%s'/'%s'/'%s'/'%s'\n",
       EuPart->DeptNm.CStr(), EuPart->OrgNm.CStr(),
       EuPart->OrgTypeNm.CStr(), EuPart->CountryNm.CStr(),
       TBool::GetStr(EuPart->CoordP).CStr());
      EuPartV.Add(EuPart);
    } else {
      printf("Partner Fields Not Found!\n");
    }
  }
}

TStr TCordisEuPart::GetNrOrgNm(const TStr& Str){
  TStrV NrStrV; TChA ChA;
  PSwSet SwSet=TSwSet::GetSwSet(swstEnglish523);
  PSIn HtmlSIn=TStrIn::New(Str);
  THtmlLx Lx(HtmlSIn);
  while (Lx.Sym!=hsyEof){
    if ((Lx.Sym==hsyStr)||(Lx.Sym==hsyNum)){
      Lx.UcChA.ChangeCh('\xc2', 'A');
      Lx.UcChA.ChangeCh('\xc1', 'A');
      Lx.UcChA.ChangeCh('\xc4', 'A');
      Lx.UcChA.ChangeCh('\xc3', 'A');
      Lx.UcChA.ChangeCh('\xc0', 'A');
      Lx.UcChA.ChangeCh('\xc7', 'C');
      Lx.UcChA.ChangeCh('\xc9', 'E');
      Lx.UcChA.ChangeCh('\xc8', 'E');
      Lx.UcChA.ChangeCh('\xca', 'E');
      Lx.UcChA.ChangeCh('\xcd', 'I');
      Lx.UcChA.ChangeCh('\xd3', 'O');
      Lx.UcChA.ChangeCh('\xd6', 'O');
      Lx.UcChA.ChangeCh('\xd1', 'N');
      Lx.UcChA.ChangeCh('\xdc', 'U');
      Lx.UcChA.ChangeCh('\xda', 'U');
      TStr NrStr=THtmlLx::GetAsciiStr(Lx.UcChA);
      if ((NrStr.Len()==1)||(SwSet->IsIn(NrStr))||
       (NrStr=="AG")||(NrStr=="GMBH")||(NrStr=="LTD")||(NrStr=="SRL")||
       (NrStr=="SA")||(NrStr=="EV")||(NrStr=="AS")||(NrStr=="AB")||
       (NrStr=="NV")||(NrStr=="BV")||
       (NrStr=="AKTIENGESELLSCHAFT")||(NrStr=="LIMITED")||
       (NrStr=="FUER")||(NrStr=="FUR")){
        NrStr="";}
      if (!NrStr.Empty()){NrStrV.Add(NrStr);}
    }
    Lx.GetSym();
  }
  TChA NrChA;
  for (int NrStrN=0; NrStrN<NrStrV.Len(); NrStrN++){
    if (NrStrN>0){NrChA+=' ';}
    NrChA+=NrStrV[NrStrN];
  }
  if (NrChA.Empty()){
    NrChA=Str.GetUc();
  }
  return NrChA;
}

/////////////////////////////////////////////////
// EuProject
void TCordisEuProj::ExtrEuProjV(const TStr& EuProjHtmlBsFNm, TCordisEuProjV& EuProjV){
  PSIn EuProjSIn(TFIn::New(EuProjHtmlBsFNm)); int EuProjRecN=0;
  while (!EuProjSIn->Eof()){
    printf("[%d]\n", EuProjRecN); EuProjRecN++;
    //if (EuProjRecN>140){break;}
    // read data
    TStr UrlStr(*EuProjSIn);
    TStr HtmlStr(*EuProjSIn);
    //if (EuProjRecN-1!=26){continue;}
    //HtmlStr.SaveTxt(TFOut::New("t.html"));
    // prepare filter
    //TStr WcStr=
    // "*FP5 Project Record*<center><span class=h3>*.\n*</span></center>"
    // "*<span class=b>Project Acronym:</span> *</td>"
    // "*<span class=b>Project Description</span>*"
    // "<span class=b>Coordinator</span>*";
    // "Project Description</span></td>";
    TStr WcStr=
     "*FP5 Project Record*<center><span class=h3>*.\n*</span></center>"
     "*FP5 Programme Acronym:</span> <B><I>*</I></B></td>"
     "*Project Reference:</span>*</td>"
     "*Start Date:</span>*</td>"
     "*End Date:</span>*</td>"
     "*Duration:</span>*</td>"
     "*Project Status:</span>*</td>"
     "*Project Acronym:</span>*</td>"
     "*<span class=b>Project Description</span>*"
     "<span class=b>Coordinator</span>*";
    // extract data
    TStrV StarStrV;
    if (HtmlStr.IsWcMatch(WcStr, StarStrV)){
      PCordisEuProj EuProj=TCordisEuProj::New();
      EuProj->TitleStr=TXmlLx::GetPlainStrFromXmlStr(StarStrV[3].GetTrunc());
      EuProj->EuProgAcrStr=StarStrV[5].GetTrunc();
      EuProj->EuProjRefStr=StarStrV[7].GetTrunc();
      EuProj->StartDateStr=StarStrV[9].GetTrunc();
      EuProj->EndDateStr=StarStrV[11].GetTrunc();
      EuProj->DurationStr=StarStrV[13].GetTrunc();
      EuProj->EuProjStatusStr=StarStrV[15].GetTrunc();
      EuProj->EuProjAcrStr=StarStrV[17].GetTrunc();
      EuProj->EuProjDescHtmlStr=StarStrV[19].GetTrunc();
      TStr AllEuPartStr=StarStrV[20].GetTrunc();
      printf("Project: '%s'/'%s'/'%s'/'%s'/'%s'/'%s'/'%s'/'%s'\n",
       EuProj->TitleStr.CStr(), EuProj->EuProgAcrStr.CStr(),
       EuProj->EuProjRefStr.CStr(), EuProj->StartDateStr.CStr(),
       EuProj->EndDateStr.CStr(), EuProj->DurationStr.CStr(),
       EuProj->EuProjStatusStr.CStr(), EuProj->EuProjAcrStr.CStr());
      TCordisEuPart::ExtrEuPartV(AllEuPartStr, EuProj->EuPartV);

      //EuProj->TitleStr=TXmlLx::GetPlainStrFromXmlStr(StarStrV[3].GetTrunc());
      //EuProj->EuProjAcrStr=StarStrV[5].GetTrunc();
      //EuProj->EuProjDescHtmlStr=StarStrV[7].GetTrunc();
      //printf("Project: '%s'/'%s'\n",
      // EuProj->TitleStr.CStr(), EuProj->EuProjAcrStr.CStr());
      EuProjV.Add(EuProj);
    } else {
      printf("Project Fields Not Found!\n");
    }
  }
}

/////////////////////////////////////////////////
// EuProject-Base
PCordisEuProjBs TCordisEuProjBs::ExtrEuProjBs(const TStr& EuProjHtmlBsFNm){
  PCordisEuProjBs EuProjBs=TCordisEuProjBs::New();
  TCordisEuProj::ExtrEuProjV(EuProjHtmlBsFNm, EuProjBs->EuProjV);
  return EuProjBs;
}

PBowDocBs TCordisEuProjBs::GetBowDocBsFromEuProjDesc() const {
  printf("Generating Bag-Of-Words...\n");
  // create document vector
  TStrV HtmlStrV;
  int EuProjs=GetEuProjs();
  for (int EuProjN=0; EuProjN<EuProjs; EuProjN++){
    PCordisEuProj EuProj=GetEuProj(EuProjN);
    // get document & word ids
    TStr EuProjNm=EuProj->GetEuProjAcrStr();
    TStr EuProjHtmlStr=EuProj->GetTitleStr()+" "+EuProj->GetEuProjDescHtmlStr();
    HtmlStrV.Add(EuProjHtmlStr);
  }
  // create ngrams
  PSwSet SwSet=TSwSet::GetSwSet(swstEnglish523);
  PNGramBs NGramBs=TNGramBs::GetNGramBsFromHtmlStrV(HtmlStrV, 3, 3, SwSet);
  NGramBs->SaveTxt("NGram.Txt");
  // create bag-of-words
  printf("\n");
  PBowDocBs BowDocBs=TBowDocBs::New();
  BowDocBs->PutNGramBs(NGramBs);
  {for (int EuProjN=0; EuProjN<EuProjs; EuProjN++){
    if (EuProjN%100==0){printf("%d/%d\r", EuProjN, EuProjs);}
    PCordisEuProj EuProj=GetEuProj(EuProjN);
    TStr DocNm=EuProj->GetEuProjAcrStr();
    TStr HtmlStr=EuProj->GetTitleStr()+" "+EuProj->GetEuProjDescHtmlStr();
    BowDocBs->AddHtmlDoc(DocNm, TStrV(), HtmlStr);
  }}
  BowDocBs->AssertOk();
  // return bag-of-words
  printf("\nDone.\n");
  return BowDocBs;
}

/////////////////////////////////////////////////
// EuProjects-Web-Fetch
void TCordisEuProjWebFetch::OnFetch(const int&, const PWebPg& WebPg){
  // print url of downloaded page
  printf("%s\n", WebPg->GetUrlStr().CStr());
  printf("-----------------------\n");
  //printf("%s", WebPg->GetHttpHdStr().CStr());
  //printf("%s", WebPg->GetHttpBodyAsStr().CStr());
  // get current page url & set of outgoing urls with descriptions
  TStr WebPgUrlStr=WebPg->GetUrlStr();
  TStrKdV DescUrlStrKdV; WebPg->GetOutDescUrlStrKdV(DescUrlStrKdV);
  // check current page type and react accordingly
  if (WebPgUrlStr.IsPrefix("http://dbs.cordis.lu/fep-cgi/srchidadb?ACTION=D&")){
    // if the page represents project description (ACTION=D)
    // save the document
    TStr HtmlStr=WebPg->GetHttpBodyAsStr();
    WebPgUrlStr.Save(*EuProjSOut);
    HtmlStr.Save(*EuProjSOut);
    EuProjSOut->Flush();
    int DescUrlStrKdN=DescUrlStrKdV.SearchForw(TStrKd("NEXT RECORD"));
    if (DescUrlStrKdN!=-1){
      // fetch next document
      FetchDocN++;
      FetchUrlStr=
       TStr("http://dbs.cordis.lu/fep-cgi/srchidadb?ACTION=D&SESSION=")+
       FetchSesIdStr+"&DOC="+TInt::GetStr(FetchDocN);
      FetchRetries=0;
      FetchUrl(FetchUrlStr);
    } else {
      printf("*** No forward pointer.\n");
      TSysMsg::Quit();
    }
    // search for forward pointer (to the next project description)
    /*int DescUrlStrKdN=DescUrlStrKdV.SearchForw(TStrKd("NEXT RECORD"));
    if (DescUrlStrKdN!=-1){
      // fetch next project description (usual, most frequent case)
      FetchUrlStr=DescUrlStrKdV[DescUrlStrKdN].Dat; FetchRetries=0;
      FetchUrl(FetchUrlStr);
    } else {
      // last project description doesn't include forward pointer
      printf("*** No forward pointer.\n");
      TSysMsg::Quit();
    }*/
  } else
  if (WebPgUrlStr.IsPrefix("http://dbs.cordis.lu/fep-cgi/srchidadb?ACTION=R&")){
    // if the page represents project record-set (ACTION=R)
    // take session id
    FetchSesIdStr=WebPgUrlStr.GetWcMatch("*SESSION=*&*", 1);
    FetchDocN=1;
    FetchUrlStr=
     TStr("http://dbs.cordis.lu/fep-cgi/srchidadb?ACTION=D&SESSION=")+
     FetchSesIdStr+"&DOC="+TInt::GetStr(FetchDocN);
    FetchRetries=0;
    FetchUrl(FetchUrlStr);
    // move to the first project-description-url (first record-set only)
    /*int DescUrlStrKdN=0;
    while (DescUrlStrKdN<DescUrlStrKdV.Len()){
      TStr UrlStr=DescUrlStrKdV[DescUrlStrKdN].Dat; DescUrlStrKdN++;
      if (UrlStr.IsPrefix("http://dbs.cordis.lu/fep-cgi/srchidadb?ACTION=D&")){
        // fetch first project-description only
        FetchUrl(UrlStr);
      }
    }
    if (DescUrlStrKdN>=DescUrlStrKdV.Len()){
      // quit downloading if no project descriptions
      printf("*** No project descriptions.\n");
      TSysMsg::Quit();
    }*/
    // fetch next index page
    /*int DescUrlStrKdN=DescUrlStrKdV.SearchForw(TStrKd("NEXT 20 RECORDS"));
    if (DescUrlStrKdN!=-1){
      FetchUrlStr=DescUrlStrKdV[DescUrlStrKdN].Dat;
      FetchRetries=0;
      FetchUrl(FetchUrlStr);
    } else {
      printf("*** No next 20 records.\n");
    }*/
  } else {
    // get forward pointer to the first project record-set (start only)
    int DescUrlStrKdN=DescUrlStrKdV.SearchForw(TStrKd("NEXT 20 RECORDS"));
    if (DescUrlStrKdN!=-1){
      FetchUrl(DescUrlStrKdV[DescUrlStrKdN].Dat);}
  }
}

void TCordisEuProjWebFetch::OnError(const int&, const TStr& MsgStr){
  printf("*** Error: '%s'\n", MsgStr.CStr());
  if (FetchUrlStr.Empty()||(FetchRetries>10)){
    TSysMsg::Quit(); // send quit message
  } else {
    FetchRetries++;
    printf("*** Retry (#%d): '%s'\n", FetchRetries, FetchUrlStr.CStr());
    FetchUrl(FetchUrlStr);
  }
}

TStr TCordisEuProjWebFetch::GetEuProgUrlStr(const TEuProg& EuProg){
  switch (EuProg){
    case eupAll: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&USR_SORT=EP_PGA_A+CHAR+ASC"; // All
    case eupIST: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=IST&USR_SORT=EP_PGA_A+CHAR+ASC"; // IST
    case eupQualityOfLife: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=LIFE+QUALITY&USR_SORT=EP_PGA_A+CHAR+ASC"; // Quality-Of-Life
    case eupGrowth: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=GROWTH&USR_SORT=EP_PGA_A+CHAR+ASC"; // Growth
    case eupEEST: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=EESD&USR_SORT=EP_PGA_A+CHAR+ASC"; // EEST
    case eupINCO2: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=INCO+2&USR_SORT=EP_PGA_A+CHAR+ASC"; // INCO 2
    case eupInnovationSMEs: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=INNOVATION-SME&USR_SORT=EP_PGA_A+CHAR+ASC"; // Innovation-SMEs
    case eupImprovingHumanPotential: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=HUMAN+POTENTIAL&USR_SORT=EP_PGA_A+CHAR+ASC"; // Improving-Human-Potential
    case eupEurAtom: return "http://dbs.cordis.lu/fep-cgi/srchidadb?CALLER=PROJ_FP5&QM_EP_PGA_D=LIFE+QUALITY&QM_EP_PGA_D=EESD&QM_EP_PGA_D=GROWTH&QM_EP_PGA_D=FP5-EAECTP+C&QM_EP_PGA_D=HUMAN+POTENTIAL&QM_EP_PGA_D=INCO+2&QM_EP_PGA_D=INNOVATION-SME&QM_EP_PGA_D=IST&QM_EP_PGA_D=FRAMEWORK+5C&QZ_WEBSRCH=&QM_EP_PGA_A=FP5-EAECTP+C&USR_SORT=EP_PGA_A+CHAR+ASC"; // EurAtom
    default: Fail; return "";
  }
}

void TCordisEuProjWebFetch::FetchEuProg(
 const TEuProg& EuProg, const TStr& EuProjHtmlBsFNm){
  // create fetching object
  TCordisEuProjWebFetch EuProjWebFetch(EuProjHtmlBsFNm);
  // get eu-program url
  TStr EuProgUrlStr=GetEuProgUrlStr(EuProg);
  // initiate fetching
  EuProjWebFetch.FetchUrl(EuProgUrlStr);
  // message-loop waiting for quit message at the fetch-end
  TSysMsg::Loop();
}

