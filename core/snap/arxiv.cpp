#include "stdafx.h"
#include "arxiv.h"

/////////////////////////////////////////////////
// Arxiv List of Papers
/*
Paper: gr-qc/9207001
From: RCAPOVI%CINVESMX.BITNET@ricevm1.rice.edu
Date: Mon, 20 Jul 1992 16:50 CST   (7kb)

Title: NON-MINIMALLY COUPLED SCALAR FIELD AND ASHTEKAR VARIABLES Author:
Riccardo Capovilla # pages: 6
Journal-ref: Phys.Rev. D46 (1992) 1450-1452
*/
bool TArxivPaperList::TArxivPaper::SetData(const TStrV& LineV) {
  TStr Line = LineV[0];
  // paper id: "Paper: gr-qc/9207001"
  IAssert(Line.SearchStr("Paper") != -1);
  int pos = Line.Len()-1;
  while (pos >= 0 && Line[pos] != '/') { pos--; }
  Id = Line.GetSubStr(pos+1, Line.Len()).GetInt();
  // if paper is a cross listing, then modify the id
  // (since there would be duplicates)
  if (Line.SearchStr("(*cross-listing*)") != -1) { //prefix is 123
    Id = (int) (11.0 * pow(10.0, (int)ceil(log((double)Id)/log(10.0))) + Id);
  }
  // Time
  Line = LineV[2];  IAssert(Line.SearchStr("Date:") != -1);
  TStrV TimeV;
  Line.SplitOnAllCh(' ', TimeV);
  int DayN=0, MonthN=0, YearN=0, HourN=0, MinN=0, SecN=0;
  int DateStart = 0;
  while (DateStart < TimeV.Len() && ! TimeV[DateStart].IsInt()) DateStart++;
  DayN = TimeV[DateStart].GetInt();
  TChA MonthStr = TimeV[DateStart+1];
  if (MonthStr.Len() != 3) {
    //printf("\n%s\n", Line.CStr());
    MonthStr = TimeV[DateStart-1];
    if (! TimeV[DateStart+2].IsInt()) {
      Swap(TimeV[DateStart+2], TimeV[DateStart+3]); }
    Swap(TimeV[DateStart+1], TimeV[DateStart+3]);
  }
  IAssert(MonthStr.Len() == 3);
  MonthStr.ToLc(); MonthStr[0] = TCh::GetUc(MonthStr[0]);
  MonthN = TTmInfo::GetMonthN(MonthStr);
  IAssert(MonthN > 0);
  YearN = TimeV[DateStart+2].GetInt();
  if (YearN < 1900) YearN += 1900;  IAssert(YearN > 1980);
  TStr Tmp = TimeV[DateStart+3];
  Tmp.SplitOnAllCh(':', TimeV);
  HourN = TimeV[0].GetInt();  IAssert(HourN>=0 && HourN<=24);
  MinN = TimeV[1].GetInt();   IAssert(MinN>=0 && MinN<=60);
  if(TimeV.Len() > 2 && TimeV[2].IsInt()) { SecN = TimeV[2].GetInt(); }
  //printf("%02d-%02d-%02d %02d:%02d:%02d\n", DayN, MonthN, YearN, HourN, MinN, SecN);
  Time = TTm(YearN, MonthN, DayN, -1, HourN, MinN, SecN, 0);
  // authors
  TChA TitleAuth;
  int i;
  for (i = 3; i < LineV.Len(); i++) {
    if (LineV[i].SearchStr("Date (revised):")==-1) { TitleAuth += LineV[i]; TitleAuth+=' '; }
  }
  int AuthBeg=-1, AuthEnd=-1, TitBeg=-1, TitEnd=-1;
  if (TitleAuth.SearchStr("Author:")!=-1) { AuthBeg=TitleAuth.SearchStr("Author:")+(int)strlen("Author:"); TitEnd=AuthBeg-8; }
  if (AuthBeg == -1 && TitleAuth.SearchStr("Authors:")!=-1) { AuthBeg=TitleAuth.SearchStr("Authors:")+(int)strlen("Authors:"); TitEnd=AuthBeg-9; }
  if (AuthBeg == -1) { /*printf("*** %s\n", TitleAuth.CStr());*/  
    return false; }
  AuthEnd=TitleAuth.SearchStr("Comments:");
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Comment:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Comments:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Commments:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Coments:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Notes:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Journal-ref:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Report-no:"); }
  if (AuthEnd == -1) { AuthEnd=TitleAuth.SearchStr("Subj-class:"); }
  if (AuthEnd == -1) {
    if (TitleAuth.SearchCh(':', AuthBeg+1) <= AuthBeg){ AuthEnd=TitleAuth.Len()-1; }
  }
  if (AuthEnd == -1) { /*printf("=== %s\n", TitleAuth.CStr());*/  
    return false; }
  TitBeg = TitleAuth.SearchStr("Title: ");
  if (TitBeg == -1) { 
    printf("=== %s\n", TitleAuth.CStr());
    return false; }
  Title = TitleAuth.GetSubStr(int(TitBeg+strlen("Title: ")), TitEnd).ToTrunc();
  TStr Authors = TitleAuth.GetSubStr(AuthBeg, AuthEnd-1).ToTrunc();
  TStrUtil::GetStdNameV(Authors, AuthorV);
  //printf(" %d\n", AuthorV.Len());
  return true;
}

bool TArxivPaperList::Next() {
  TChA Line;
  while (! SIn->Eof()) {
    SIn->GetNextLn(Line);
    if (strstr(Line.CStr(), "Paper:")!=NULL || strstr(Line.CStr(), "Paper (*cross-listing*):")!=NULL) {
      TStrV LineV;
      while (! (strlen(Line.CStr())<5 && strstr(Line.CStr(), "\\\\")!=NULL)) {
        LineV.Add(Line);  LineV.Last().ToTrunc();
        if (LineV.Last().Empty()) { LineV.DelLast(); }
        SIn->GetNextLn(Line);
      }
      TArxivPaper Paper;
      if (Paper.SetData(LineV)) {
        Id = Paper.Id;
        Time = Paper.Time;
        Title = Paper.Title;
        AuthorV.Swap(Paper.AuthorV);
        return true;
      } else { 
        if (! SIn->Eof()) { return Next(); }      
      }
    }
  }
  return false;
}

void TArxivPaperList::MakeCoAuthNet(const TStr& FNm, const TStr& OutFNm) {
  TVec<TArxivPaper> PaperV;
  TArxivPaper Paper;
  printf("\n%s\n", OutFNm.CStr());
  FILE *F = fopen(FNm.CStr(), "rt");
  char line [2024];
  int NSkip=0, NPapers=0, CrossListed=0;
  while (! feof(F)) {
    memset(line, 0, 2024);
    fgets(line, 2024, F);
    if (strstr(line, "Paper:")!=NULL || strstr(line, "Paper (*cross-listing*):")!=NULL) {
      TStrV LineV;
      if (strstr(line, "Paper (*cross-listing*):") != NULL) { CrossListed++; }
      while (! (strlen(line)<5 && strstr(line, "\\\\")!=NULL)) {
        LineV.Add(line);  LineV.Last().ToTrunc();
        if (LineV.Last().Empty()) { LineV.DelLast(); }
        fgets(line, 2024, F);
      }
      if (Paper.SetData(LineV)) {
        PaperV.Add(Paper);
      } else { NSkip++; }
      NPapers++;
    } else {
      if (strstr(line, "\\\\") == 0 && strstr(line, "-----------------------------------------") == 0) {
        printf("%s", line);
      }
    }
    //if (cnt++ > 1000) break;
  }
  fclose(F);
  // save co-authorship data
  TStrHash<TInt> AuthToIdH;
  TStrHash<TTm> AuthToTmH; // time when published first paper
  int p, i, a, prevId;

  // author IDS
  { TStrV AuthorV;
  for (p = 0; p < PaperV.Len(); p++) {
    if (PaperV[p].AuthorV.Empty()) {
      PaperV.Del(p);  p--;  continue; }
    for (a = 0; a < PaperV[p].AuthorV.Len(); a++) {
      AuthorV.Add(PaperV[p].AuthorV[a]); }
  }
  AuthorV.Sort();
  for (a = 0; a < AuthorV.Len(); a++) {
    AuthToIdH.AddDat(AuthorV[a], a+1); }
  }
  // author time
  for (p = 0; p < PaperV.Len(); p++) {
    for (a = 0; a < PaperV[p].AuthorV.Len(); a++) {
      TStr Author = PaperV[p].AuthorV[a];
      if (AuthToTmH.IsKey(Author.CStr()) && PaperV[p].Time < AuthToTmH.GetDat(Author.CStr())) {
        AuthToTmH.GetDat(Author.CStr()) = PaperV[p].Time;
      } else {
        AuthToTmH.AddDat(Author, PaperV[p].Time); }
    }
  }
  F = fopen(TStr::Fmt("%s.dates", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#author_id\tfirst_apperance\n");
  for (i = 0; i < AuthToTmH.Len(); i++) {
    const TTm& Tm = AuthToTmH[i];
    fprintf(F, "%05d\t%4d-%02d-%02d\n", AuthToIdH[i].Val, Tm.GetYear(), Tm.GetMonth(), Tm.GetDay()); }
  fclose(F);

  F = fopen(TStr::Fmt("%s.auths", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#author_id\tauthor_name\n");
  for (i = 0; i < AuthToIdH.Len(); i++) {
    fprintf(F, "%05d\t%s\n", AuthToIdH[i].Val, AuthToIdH.GetKey(i)); }
  fclose(F);

  F = fopen(TStr::Fmt("%s.papers", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#bipartite graph (author ids [1...A], paper ids [100*A ...100*A+P])\n");
  fprintf(F, "#paper_id\tpaper_time\tauthor_ids\n");
  for (p = 0; p < PaperV.Len(); p++) {
    const int PaperId = 100*AuthToIdH.Len() + p;
    const TTm& Tm = PaperV[p].Time;
    if (PaperV[p].AuthorV.Empty()) continue; // skip papers with no authors
    fprintf(F, "%06d\t%4d-%02d-%02d", PaperId, Tm.GetYear(), Tm.GetMonth(), Tm.GetDay());
    TIntV AuthIdV;
    for (a = 0; a < PaperV[p].AuthorV.Len(); a++) { AuthIdV.Add(AuthToIdH.GetDat(PaperV[p].AuthorV[a].CStr())); }
    AuthIdV.Sort();  prevId = -1;
    for (int a = 0; a < AuthIdV.Len(); a++) {
      if (prevId != AuthIdV[a]) { prevId=AuthIdV[a];  fprintf(F, "\t%d", AuthIdV[a].Val); } }
    fprintf(F, "\n");
  }
  fclose(F);
  int all_links=0, unique_links=0;
  { THash<TIntPr, TInt> LinkH;
  F = fopen(TStr::Fmt("%s.coauth", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#undirected co-authorship graph (directed edges, x->y, y->x).\n");
  fprintf(F, "#there is a link if authors share at least 1 paper\n");
  fprintf(F, "#author_id\tauthor_id\n");
  for (p = 0; p < PaperV.Len(); p++) {
    for (int a1 = 0; a1 < PaperV[p].AuthorV.Len(); a1++) {
      for (int a2 = 0; a2 < PaperV[p].AuthorV.Len(); a2++) {
        if (a1 != a2) {
          const int Id1 = AuthToIdH.GetDat(PaperV[p].AuthorV[a1].CStr());
          const int Id2 = AuthToIdH.GetDat(PaperV[p].AuthorV[a2].CStr());
          if (! LinkH.IsKey(TIntPr(Id1, Id2))) {
            fprintf(F, "%05d\t%05d\n", Id1, Id2);
            LinkH.AddKey(TIntPr(Id1, Id2));
            unique_links++;
          }
        }
      }
    }
  }
  fclose(F); }
  /*{ THash<TIntPr, TInt> LinkH;
  F = fopen(TStr::Fmt("%s.links", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#undirected co-authorship graph (directed edges, x->y, y->x).\n");
  fprintf(F, "#links means that authors wrote a paper at that particular time.\n");
  fprintf(F, "#author_id\tauthor_id\ttime\n");
  int links=0;
  for (p = 0; p < PaperV.Len(); p++) {
    for (int a1 = 0; a1 < PaperV[p].AuthorV.Len(); a1++) {
      for (int a2 = 0; a2 < PaperV[p].AuthorV.Len(); a2++) {
        if (a1 != a2) {
          const TTm& Tm = PaperV[p].Time;
          const int Id1 = AuthToIdH.GetDat(PaperV[p].AuthorV[a1]);
          const int Id2 = AuthToIdH.GetDat(PaperV[p].AuthorV[a2]);
          fprintf(F, "%07d\t%07d\t%4d-%02d-%02d\n", Id1, Id2, Tm.GetYear(), Tm.GetMonth(), Tm.GetDay());
          all_links++;
        }
      }
    }
  }
  fclose(F); }*/
  printf("\tPapers parsed:  %d\n", NPapers);
  printf("\tPapers:         %d\n", PaperV.Len());
  printf("\tCross-listed:   %d\n", CrossListed);
  printf("\tSkipped papers: %d\n", NSkip);
  printf("\tAuthors:        %d\n", AuthToTmH.Len());
  printf("\tAll links:      %d\n", all_links/2);
  printf("\tUnique links:   %d\n", unique_links/2);
}

void TArxivPaperList::ParseFiles() {
  { TArxivPaperList P; P.MakeCoAuthNet("D:/Dev/GraphGarden/Data/Arxiv/Arxiv-CoAuth/gr-qc.lis", "gr-qc"); }
  { TArxivPaperList P; P.MakeCoAuthNet("D:/Dev/GraphGarden/Data/Arxiv/Arxiv-CoAuth/astro-ph.lis", "astro-ph"); }
  { TArxivPaperList P; P.MakeCoAuthNet("D:/Dev/GraphGarden/Data/Arxiv/Arxiv-CoAuth/cond-mat.lis", "cond-mat"); }
  { TArxivPaperList P; P.MakeCoAuthNet("D:/Dev/GraphGarden/Data/Arxiv/Arxiv-CoAuth/hep-th.lis", "hep-th"); }
  { TArxivPaperList P; P.MakeCoAuthNet("D:/Dev/GraphGarden/Data/Arxiv/Arxiv-CoAuth/hep-ph.lis", "hep-ph");
  /*FILE *F = fopen("hep-ph-slacdates", "wt");
  for (int p = 0; p < P.PaperV.Len(); p++) { const TTm& Tm = P.PaperV[p].Time;
    fprintf(F, "%07d\t%4d-%02d-%02d\n", P.PaperV[p].Id, Tm.GetYear(), Tm.GetMonth(), Tm.GetDay()); }
  fclose(F);//*/
  }
}

// Load 'hep-ph-citations.rndId', which contains random paper ids.
// Use 'hep-ph-mapping.map' to map from random paper ids to slac
// paper ids. Then use 'hep-ph.slacdates' to create a time network.
// File 'hep-ph.slacdates' was obtained from file 'hep-ph.lis'
void TArxivPaperList::MapHepPhRandDocIds() {
  THash<TInt, TInt> OldToNewId;
  // load mapping
  PSs Ss = TSs::LoadTxt(ssfSpaceSep, "/Dev/GraphGarden/Data/Arxiv/hep-ph-mapping.map");
  printf("cols: %d\n", Ss->GetYLen());
  IAssert(Ss->GetXLen() == 2);
  for (int y = 0; y < Ss->GetYLen(); y++) {
    if (Ss->At(0, y)[0] == '#') continue; // skip comments
    const int rndId = Ss->At(1, y).GetInt();
    const int slacId = Ss->At(0, y).GetInt();
    OldToNewId.AddDat(rndId, slacId);
  }
  // map paper ids
  Ss = TSs::LoadTxt(ssfSpaceSep, "/Dev/GraphGarden/Data/Arxiv/hep-ph-citations.rndId", NULL, false);
  printf("cols: %d\n", Ss->GetYLen());
  FILE *F = fopen("/Dev/GraphGarden/Data/Arxiv/hep-ph-citations", "wt");
  fprintf(F, "#citations (slac paper ids)\n");
  for (int i = 0; i < Ss->GetYLen(); i++) {
    if (Ss->At(0, i)[0] == '#') continue; // skip comments
    int srcId = Ss->At(0, i).GetInt();
    int dstId = Ss->At(1, i).GetInt();
    IAssert(OldToNewId.IsKey(srcId));
    IAssert(OldToNewId.IsKey(dstId));
    fprintf(F, "%07d %07d\n", OldToNewId.GetDat(srcId).Val, OldToNewId.GetDat(dstId).Val);
  }
  fclose(F);
}
