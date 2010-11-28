#include "stdafx.h"
#include "imdbnet.h"

/////////////////////////////////////////////////
// IMDB Node
TImdbNode::TImdbNode(const int& MovieName, const int& MovieYear, const TMovieTy& MovieTy) :
  Name(MovieName), Year(MovieYear), Position(0), Type(MovieTy), IsMaleAct(false) { }

TImdbNode::TImdbNode(const int& ActorName, const int& ActorYear, const int& ActorPos, const bool& IsMaleActor) :
  Name(ActorName), Year(ActorYear), Position(ActorPos), Type(mtyActor), IsMaleAct(IsMaleActor) { }

TImdbNode::TImdbNode(TSIn& SIn) : Name(SIn), Year(SIn), Position(SIn), Type(SIn), 
  IsMaleAct(SIn), Cntry(SIn), Lang(SIn) { }

void TImdbNode::Save(TSOut& SOut) const {
  Name.Save(SOut);  Year.Save(SOut);
  Position.Save(SOut);  Type.Save(SOut);
  IsMaleAct.Save(SOut);
  Cntry.Save(SOut);  Lang.Save(SOut);
}

/////////////////////////////////////////////////
// IMDB Net
/*PTimeNet TImdbNet::GetTimeNet() const {
  PTimeNet TmNet = TTimeNet::New();
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TmNet->AddNode(NI.GetId(), TSecTm(NI().GetYear(), 1, 1));
  }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      TmNet->AddEdge(NI.GetId(), NI.GetOutNId(e)); }
  }
  return TmNet;
}*/

// actors collaboration graph
PUNGraph TImdbNet::GetActorGraph() const  { 
  TIntPrSet EdgeSet;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (NI().GetTy() == mtyActor) {
      const int NId1 = NI.GetId();
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        if (NI.GetOutNDat(e).GetTy() != mtyActor) {
          TNodeI NI2 = GetNI(NI.GetOutNId(e));
          for (int e2 = 0; e2 < NI2.GetInDeg(); e2++) {
            if (NI2.GetInNDat(e2).GetTy() == mtyActor) {
              const int NId2 = NI2.GetInNId(e2);
              EdgeSet.AddKey(TIntPr(TMath::Mn(NId1, NId2), TMath::Mx(NId1, NId2)));
            }
          }
        }
      }
    }
  }
  PUNGraph G = TUNGraph::New(); 
  for (int i = 0; i < EdgeSet.Len(); i++) {
    const int NId1 = EdgeSet[i].Val1;
    const int NId2 = EdgeSet[i].Val2;
    if (! G->IsNode(NId1)) { G->AddNode(NId1); }
    if (! G->IsNode(NId2)) { G->AddNode(NId2); }
    G->AddEdge(NId1, NId2);
  }
  return G;
}

// set Language and Country for movies that do not have the value set
// for every movie find the mojority language/country in 1-hop neighborhood and set it
void TImdbNet::SetLangCntryByMajority() {
  // set language
  while (true) {
    TIntPrV NIdToVal;
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      if (NI().GetLang() != 0) { continue; }
      int Nbhs=0; TIntH LangCntH;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        LangCntH.AddDat(NI.GetOutNDat(e).GetLang()) += 1;  Nbhs++; }
      for (int e = 0; e < NI.GetInDeg(); e++) {
        LangCntH.AddDat(NI.GetInNDat(e).GetLang()) += 1;  Nbhs++; }
      if (LangCntH.IsKey(0)) { Nbhs-=LangCntH.GetDat(0); LangCntH.GetDat(0)=0; }
      LangCntH.SortByDat(false);
      if (LangCntH.GetKey(0) == 0) { continue; }
      if (LangCntH[0]*2 >= Nbhs) { 
        NIdToVal.Add(TIntPr(NI.GetId(), LangCntH.GetKey(0))); }
    }
    if (NIdToVal.Empty()) { break; } // done
    for (int i = 0; i < NIdToVal.Len(); i++) {
      GetNDat(NIdToVal[i].Val1).Lang = NIdToVal[i].Val2; }
    printf("  language set: %d\n", NIdToVal.Len());
  }
  int cnt=0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { if (NI().GetLang()==0) cnt++; }
  printf(" NO language: %d\n\n", cnt);
  // set country
  while (true) {
    TIntPrV NIdToVal;
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      if (NI().GetCntry() != 0) { continue; }
      int Nbhs=0; TIntH CntryCntH;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        CntryCntH.AddDat(NI.GetOutNDat(e).GetCntry()) += 1;  Nbhs++; }
      for (int e = 0; e < NI.GetInDeg(); e++) {
        CntryCntH.AddDat(NI.GetInNDat(e).GetCntry()) += 1;  Nbhs++; }
      if (CntryCntH.IsKey(0)) { Nbhs-=CntryCntH.GetDat(0); CntryCntH.GetDat(0)=0; }
      CntryCntH.SortByDat(false);
      if (CntryCntH.GetKey(0) == 0) { continue; }
      if (CntryCntH[0]*2 >= Nbhs) { 
        NIdToVal.Add(TIntPr(NI.GetId(), CntryCntH.GetKey(0))); }
    }
    if (NIdToVal.Empty()) { break; } // done
    for (int i = 0; i < NIdToVal.Len(); i++) {
      GetNDat(NIdToVal[i].Val1).Cntry = NIdToVal[i].Val2; }
    printf("  country set: %d\n", NIdToVal.Len());
  }
  cnt=0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { if (NI().GetCntry()==0) cnt++; }
  printf(" NO country: %d\n\n", cnt);
}

// set actor's language and country
void TImdbNet::SetActorCntryLangByMajority() {
    // set language
  TIntPrV NIdToVal;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (! NI().IsActor()) { continue; }
    IAssert(NI().GetLang() == 0); // no language set
    IAssert(NI.GetInDeg() == 0);  // actors point to movies
    int Nbhs=0;  TIntH LangCntH;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      LangCntH.AddDat(NI.GetOutNDat(e).GetLang()) += 1;  Nbhs++; }
    if (LangCntH.IsKey(0)) { Nbhs-=LangCntH.GetDat(0); LangCntH.GetDat(0)=0; }
    LangCntH.SortByDat(false);
    if (LangCntH.GetKey(0) == 0) { continue; }
    if (LangCntH[0]*2 >= Nbhs) { 
      NIdToVal.Add(TIntPr(NI.GetId(), LangCntH.GetKey(0))); }
  }
  for (int i = 0; i < NIdToVal.Len(); i++) {
    GetNDat(NIdToVal[i].Val1).Lang = NIdToVal[i].Val2; }
  printf("  language set: %d\n", NIdToVal.Len());
  
  int cnt=0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { if (NI().IsActor() && NI().GetLang()==0) cnt++; }
  printf("  Actors NO language: %d\n\n", cnt);
  // set country
  NIdToVal.Clr(true);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (! NI().IsActor()) { continue; }
    IAssert(NI().GetCntry() == 0); // no country set
    IAssert(NI.GetInDeg() == 0);   // actors point to movies
    int Nbhs=0; TIntH CntryCntH;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      CntryCntH.AddDat(NI.GetOutNDat(e).GetCntry()) += 1;  Nbhs++; }
    if (CntryCntH.IsKey(0)) { Nbhs-=CntryCntH.GetDat(0); CntryCntH.GetDat(0)=0; }
    CntryCntH.SortByDat(false);
    if (CntryCntH.GetKey(0) == 0) { continue; }
    if (CntryCntH[0]*2 >= Nbhs) { 
      NIdToVal.Add(TIntPr(NI.GetId(), CntryCntH.GetKey(0))); }
  }
  for (int i = 0; i < NIdToVal.Len(); i++) {
    GetNDat(NIdToVal[i].Val1).Cntry = NIdToVal[i].Val2; }
  printf("  country set: %d\n", NIdToVal.Len());
  cnt=0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { if (NI().IsActor() && NI().GetCntry()==0) cnt++; }
  printf("  Actors NO country: %d\n\n", cnt);
}


// load from allactors.zip that was prepared by Brad Malin in 2005
PImdbNet TImdbNet::LoadTxt(const TStr& ActorFNm) {
  PImdbNet Net = TImdbNet::New();
  TStrV ColV;
  char line [2024];
  int NLines=0, DupEdge=0, Year, Position, ActorNId, MovieNId;
  TIntH ActorNIdH;
  THash<TIntPr, TInt> MovieNIdH;
  FILE *F = fopen(ActorFNm.CStr(), "rt");  fgets(line, 2024, F);
  while (! feof(F)) {
    memset(line, 0, 2024);
    fgets(line, 2024, F);
    if (strlen(line) == 0) break;
    TStr(line).SplitOnAllCh('|', ColV, false);  IAssert(ColV.Len() == 7);
    const int NameStrId = Net->AddStr(ColV[0].GetTrunc().GetLc()+" "+ColV[1].GetTrunc().GetLc());
    const int MovieStrId = Net->AddStr(ColV[2].GetTrunc().GetLc());
    TStr YearStr = ColV[3].GetTrunc();
    if (YearStr.Len() > 4) YearStr = YearStr.GetSubStr(0, 3);
    Year = 1;  YearStr.IsInt(Year);
    const TMovieTy MovieTy = TImdbNet::GetMovieTy(ColV[4]);
    Position = TInt::Mx;  ColV[5].GetTrunc().IsInt(Position);
    IAssert(ColV[6].GetTrunc()[0] == 'M' || ColV[6].GetTrunc()[0]=='F');
    const bool IsMale = ColV[6].GetTrunc()[0] == 'M';
    // create nodes  
    if (ActorNIdH.IsKey(NameStrId)) { 
      ActorNId = ActorNIdH.GetDat(NameStrId); }
    else { 
      ActorNId = Net->AddNode(-1, TImdbNode(NameStrId, Year, Position, IsMale));
      ActorNIdH.AddDat(NameStrId, ActorNId);
    }
    if (MovieNIdH.IsKey(TIntPr(MovieStrId, Year))) {
      MovieNId = MovieNIdH.GetDat(TIntPr(MovieStrId, Year)); }
    else {
      MovieNId = Net->AddNode(-1, TImdbNode(NameStrId, Year, MovieTy)); 
      MovieNIdH.AddDat(TIntPr(MovieStrId, Year), MovieNId); 
    }
    if (! Net->IsEdge(ActorNId, MovieNId)) { 
      Net->AddEdge(ActorNId, MovieNId); }
    else { DupEdge++; }
    if (++NLines % 100000 == 0) printf("\r  %dk  ", NLines/1000);
  }
  fclose(F);
  printf("duplicate edges: %d\n", DupEdge);
  printf("nodes:  %d\n", Net->GetNodes());
  printf("edges:  %d\n", Net->GetEdges());
  printf("actors: %d\n", ActorNIdH.Len());
  printf("movies: %d\n", MovieNIdH.Len());
  // set the actor year to the year of his first movie
  int NUpdates=0;
  for (TNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    if (NI().IsActor()) {
      int MinYear = NI().GetYear();
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const TImdbNode& NodeDat = Net->GetNDat(NI.GetOutNId(e));
        if (NodeDat.IsMovie()) MinYear = TMath::Mn(MinYear, NodeDat.GetYear());
      }
      if (NI().Year != MinYear) NUpdates++;
      NI().Year = MinYear;
    }
  }
  printf("updated actor times: %d\n", NUpdates);
  return Net;
}

// load IMDB from original files obtainable from ftp://ftp.fu-berlin.de/pub/misc/movies/database/
//  actors.list.gz (actor, movie),  
//  countries.list.gz, language.list.gz (movie, country and language)
//  movies.list.gz (movie, year)
//
//  "xxxxx"        = a television series
//  "xxxxx" (mini) = a television mini-series
//  [xxxxx]        = character name
//  <xx>           = number to indicate billing position in credits
//  (TV)           = TV movie, or made for cable movie
//  (V)            = made for video movie (this category does NOT include TV
//                   episodes repackaged for video, guest appearances in
//                   variety/comedy specials released on video, or
//                   self-help/physical fitness videos)

int GetYearFromTitle(char *title) {
  // find: (<year>) or (<year>/
  char *end = title + strlen(title) - 5;
  while (end > title) {
    if (*end=='(' && (*(end+5)==')' || *(end+5)=='/') && TCh::IsNum(*(end+1)) 
      && TCh::IsNum(*(end+2)) && TCh::IsNum(*(end+3)) && TCh::IsNum(*(end+4))) {
        char num [10];
        strncpy(num, end+1, 4); num[4]=0;
        return atoi(num);
    }
    end--;
  }
  return -1;
}

// Parse files downloaded from IMDB. Actors point to movies.
// Files: actors.list.gz, languages.list.gz, countries.list.gz
PImdbNet TImdbNet::LoadFromImdb(const TStr& DataDir) {
  PImdbNet Net = TImdbNet::New();
  Net->Reserve((int) Mega(2.5), -1);
  // ACTORS
  { TSsParser Ss(DataDir+"\\actors.list.gz", ssfTabSep);
  while (Ss.Next() && strcmp(Ss[0],"THE ACTORS LIST")!=0) { }
  Ss.Next();  Ss.Next();  Ss.Next();  
  int ActorId = -1, NAct=0;
  for (int i = 0; Ss.Next(); i++) {
    //printf("%s\n", Ss.DumpStr());
    int mPos = 0;
    if (Ss.Len() > 1) { // actor
      ActorId = Net->AddStr(Ss[0]);
      if (Net->IsNode(ActorId)) {
        printf("  actor '%s'(%d) is already a node %s\n", Ss[0], 
          ActorId, TImdbNet::GetMovieTyStr((TMovieTy) Net->GetNDat(ActorId).Type.Val).CStr());
        continue;
      } else { Net->AddNode(ActorId); }
      TImdbNode& Node = Net->GetNDat(ActorId);
      Node.Name = ActorId;  NAct++;
      Node.Type = mtyActor;
      mPos = 1;  while (strlen(Ss[mPos])==0) { mPos++; }
    } 
    // movie (delete everything last [)
    //  also parse the position <>, but is a property of an edge (actor, movie) pair
    char *DelFrom;
    char *C1 = strrchr(Ss[mPos], '<');
    char *C2 = strrchr(Ss[mPos], '[');
    if (C1==NULL) { DelFrom=C2; }
    else if (C2==NULL) { DelFrom=C1; }
    else { DelFrom = TMath::Mn(C1, C2); }
    if (DelFrom != NULL) {
      char *mov = DelFrom;  while (*mov) { *mov=0; mov++; }
      mov = (char *) DelFrom-1;  while (TCh::IsWs(*mov)) { *mov=0; mov--; }
    }
    const int MovNId = Net->AddStr(Ss[mPos]);
    if (! Net->IsNode(MovNId)) {
      Net->AddNode(MovNId);
      TImdbNode& Node = Net->GetNDat(MovNId);
      Node.Type = mtyMovie;  Node.Year = GetYearFromTitle(Ss[mPos]);
    }
    if (Net->IsEdge(ActorId, MovNId)) {
      printf("  already an edge %d %d\n", ActorId, MovNId); }
    else { Net->AddEdge(ActorId, MovNId); }
    if ((i+1) % Kilo(10) == 0) { 
      printf("\r   %d", (i+1)/1000); 
      if ((i+1) % Kilo(100) == 0) { 
        printf(" nodes: %d, edges: %d, actors: %d", 
          Net->GetNodes(), Net->GetEdges(), NAct); }
    }
  } 
  printf("\n=== nodes: %d, edges: %d, actors: %d", Net->GetNodes(), Net->GetEdges(), NAct); }
  // MOVIE LANGUAGE */
  { TSsParser Ss(DataDir+"\\language.list.gz", ssfTabSep);
  while (Ss.Next() && strcmp(Ss[0],"LANGUAGE LIST")!=0) { }
  Ss.Next();
  int LangCnt=0, i;
  for (i = 0; Ss.Next(); i++) {
    char *Mov = Ss[0];
    char *Lang = Ss[Ss.Len()-1];
    if (Net->IsStr(Mov)) {
      const int NId = Net->GetStrId(Mov);
      Net->GetNDat(NId).Lang = Net->AddStr(Lang);
      LangCnt++;
    } //else { printf("movie not found: '%s'\n", Mov); }
    if ((i+1) % Kilo(10) == 0) { 
      printf("\r   %d found %d ", (i+1), LangCnt); }
  } 
  printf("\n  LANG: total movies: %d,  found %d\n", (i+1), LangCnt); }
  // MOVIE COUNTRY
  { TSsParser Ss(DataDir+"\\countries.list.gz", ssfTabSep);
  while (Ss.Next() && strcmp(Ss[0],"COUNTRIES LIST")!=0) { }
  Ss.Next();
  int LangCnt=0, i;
  for (i = 0; Ss.Next(); i++) {
    char *Mov = Ss[0];
    char *Cntry = Ss[Ss.Len()-1];
    if (Net->IsStr(Mov)) {
      const int NId = Net->GetStrId(Mov);
      Net->GetNDat(NId).Cntry = Net->AddStr(Cntry);
      LangCnt++;
    } //else { printf("country not found: '%s'\n", Mov); }
    if ((i+1) % Kilo(10) == 0) { 
      printf("\n   %d found %d ", (i+1), LangCnt); }
  } 
  printf("\r  CNTRY: total movies: %d,  found %d\n", (i+1), LangCnt);  }
  return Net;
}

TMovieTy TImdbNet::GetMovieTy(const TStr& MovieTyStr) {
  TStr LcStr = MovieTyStr.GetTrunc().GetLc();
  if (LcStr == "m") return mtyMovie;
  else if (LcStr == "tv") return mtyTv;
  else if (LcStr == "v") return mtyV;
  else if (LcStr == "vg") return mtyVg;
  else if (LcStr == "a") return mtyA;
  else if (LcStr == "scene") return mtyScene;
  else if (LcStr == "mini") return mtyMini;
  else if (LcStr == "archiv") return mtyArch;
  else if (LcStr == "voice") return mtyVoice;
  else if (LcStr == "singin") return mtySing;
  else if (LcStr == "als") return mtyAls;
  else if (LcStr == "episod") return mtyEpisod;
  else if (LcStr == "unconfirmed") return mtyUndef;
  printf("Unknown type: '%s'\n", LcStr.CStr());
  return mtyUndef;
}

TStr TImdbNet::GetMovieTyStr(const TMovieTy& MovieTy) {
  switch (MovieTy) {
    case mtyUndef : return "Undef";
    case mtyActor : return "Actor";
    case mtyMovie : return "Movie";
    case mtyTv : return "Tv";
    case mtyV : return "V";
    case mtyVg : return "Vg";
    case mtyA : return "A";
    case mtyScene : return "Scene";
    case mtyMini : return "Mini";
    case mtyArch : return "Arch";
    case mtyVoice : return "Voice";
    case mtySing : return "Sing";
    case mtyAls : return "Als";
    case mtyEpisod : return "Episod";
    default: Fail; return TStr::GetNullStr();
  }
}