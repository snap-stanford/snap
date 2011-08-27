#include "stdafx.h"
#include "wordnet.h"

/////////////////////////////////////////////////
// WordNet Synset Pointer
//bits: 0..23 Id, 24..28 PtrTy, 29..31 SynCat
int TSynPtr::Code(const int& SynId, const TWnPtrTy& PtrTy, const TWnSynCat& SynCat) {
  IAssert(uint(SynId) < TMath::Pow2(24));
  IAssert(uint(PtrTy) < TMath::Pow2(5));
  IAssert(uint(SynCat) < TMath::Pow2(3));
  return SynId | PtrTy << 24 | SynCat << 29;
}

void TSynPtr::Decode(const int& Code, int& SynId, TWnPtrTy& PtrTy, TWnSynCat& SynCat) {
  SynId = Code & 0x00ffffff;
  PtrTy = TWnPtrTy((Code >> 24) & 0x0000001f);
  SynCat = TWnSynCat((Code >> 29) & 0x00000007);
  IAssert(SynId >= 0);
  IAssert(wptUndef < PtrTy && PtrTy < wptMx);
  IAssert(wscUndef < SynCat && SynCat < wscMx);
}

/////////////////////////////////////////////////
// WordNet

int TWordNet::TSynsetStorage::AddSyn(const int& LexFNm, const int& GlossStrId, const TWnSynCat& SynCat,
 const TIntV& WrdV, const TVec<TSynPtr> SynPtrV) {
  const int SynOff = Storage.Len();
  Storage.Add(LexFNm);  Storage.Add(GlossStrId);  Storage.Add(int(SynCat));
  Storage.Add(WrdV.Len());
  Storage.AddV(WrdV);
  Storage.Add(SynPtrV.Len());
  for (int i = 0; i < SynPtrV.Len(); i++) Storage.Add(SynPtrV[i].Code());
  return SynOff;
}

int TWordNet::AddSenseV(const int& WId, const TIntV& WrdSenseV) {
  const int SenseOff = SenseV.Len();
  SenseV.Add(WrdSenseV.Len());
  IAssert(WrdSenseV.IsSortedCmp(TLss<TInt>()));
  SenseV.AddV(WrdSenseV);
  WrdToIdH[WId] = SenseOff;
  return SenseOff;
}

bool TWordNet::IsSynset(const TStr& Wrd) const {
  int WId = -1;
  if (! WrdToIdH.IsKey(Wrd.GetLc().CStr(), WId)) return false;
  if (WrdToIdH[WId] != -1) return true;
  return false;
}

void TWordNet::GetSynsetV(const int& WId, TIntV& SynIdV) const {
  const int SenseOff = WrdToIdH[WId];
  SynIdV.Clr();
  if (SenseOff == -1) return;
  for (int i = 0; i < SenseV[SenseOff]; i++) SynIdV.Add(SenseV[SenseOff + 1 + i]);
}

void TWordNet::GetSynsetV(const TStr& Wrd, TIntV& SynIdV) const {
  const int WId = GetWId(Wrd);
  if (WId == -1) { SynIdV.Clr(); return; }
  GetSynsetV(WId, SynIdV);
}

PWordNet TWordNet::LoadWordNetTxt(const TStr& FPath) {
  PWordNet WnPt = TWordNet::New();
  // maps old synset Ids: KeyId is synsetId, Dat is SynOff in TWordNet::SynsetS
  THash<TIntPr, TInt> MapIdH; //old ID is composed of synset byte offset and syntactic category

  FILE *Fl = 0;
  char line [20000];
  int NewSynId, LexFNm, GlossStrId, id, f = 0, NSynSet = 0;
  TIntPr OldSynId;
  TStrV FieldV;
  TIntV SynWrdV;
  TVec<TSynPtr> SynPtrV;
  TChA GlossStr;

  TStrV WordNetDatFlV;
  WordNetDatFlV.Add("noun.dat");
  WordNetDatFlV.Add("verb.dat");
  WordNetDatFlV.Add("adj.dat");
  WordNetDatFlV.Add("adv.dat");  

  // see WNDB.html for *.dat file data format
  for (int wfl = 0; wfl < WordNetDatFlV.Len(); wfl++) {
    Fl = fopen((TStr::GetNrFPath(FPath) + WordNetDatFlV[wfl]).CStr(), "rt");
    //parsing: 00106612 04 n 02 smack 1 smooch 0 002 @ 00106288 n 0000 ~ 00106711 n 0000 | an enthusiastic kiss
    while (! feof(Fl)) {
      line[0] = 0;
      fgets(line, 20000, Fl);
      if (line[0] == 0 || (line[0] == ' ' && line[1] == ' ')) continue;
      if (++NSynSet % 1000 == 0) printf("%d\r", NSynSet);

      IAssert(strlen(line) < 19990);
      TStr(line).SplitOnWs(FieldV);
      IAssert(FieldV[2].Len() == 1);

      const TWnSynCat SynCat = GetSynCat(FieldV[2][0]);
      OldSynId = TIntPr(FieldV[0].GetInt(), int(SynCat));
      NewSynId = MapIdH.GetKeyId(OldSynId);
      if (NewSynId == -1) { NewSynId = MapIdH.AddKey(OldSynId);  MapIdH[NewSynId] = -1; }
      else IAssert(MapIdH[NewSynId] == -1);

      // add words from the synset
      SynWrdV.Clr();
      for (f = 4;  f < 4 + 2 * FieldV[3].GetHexInt(); f += 2) {
        if (FieldV[f].IsChIn('(')) FieldV[f] = FieldV[f].GetSubStr(0, FieldV[f].SearchCh('(') - 1);
        FieldV[f].ChangeChAll('_', ' ');
        FieldV[f].ToLc();
        SynWrdV.Add(WnPt->WrdToIdH.AddKey(FieldV[f]));
      }
      // parse pointers
      SynPtrV.Clr();
      const int LastPtr = f + 4 * FieldV[f].GetInt();  f++;
      while (f < LastPtr) {
        IAssert(FieldV[f+2].Len() == 1);
        OldSynId = TIntPr(FieldV[f+1].GetInt(), int(GetSynCat(FieldV[f+2][0])));
        id = MapIdH.GetKeyId(OldSynId);
        if (id == -1) { id = MapIdH.AddKey(OldSynId);  MapIdH[id] = -1; }
        SynPtrV.Add(TSynPtr(id, GetPtrTy(FieldV[f]), GetSynCat(FieldV[f+2][0])));
        f += 4;
      }
      while (FieldV[f] != "|") f++; // skip verb sentence frames
      GlossStr.Clr();
      GlossStr = FieldV[++f];  f++;
      while (f < FieldV.Len()) { GlossStr.AddCh(' ');  GlossStr += FieldV[f++]; }
      GlossStrId = WnPt->WrdToIdH.AddKey(GlossStr);
      LexFNm = FieldV[1].GetInt();
      IAssert(MapIdH[NewSynId] == -1); //synset has not yet been parsed
      MapIdH[NewSynId] = WnPt->SynsetS.AddSyn(LexFNm, GlossStrId, GetSynCat(FieldV[2][0]), SynWrdV, SynPtrV);
    }
    printf("%s: %d synsets parsed, %d detected (from pointers).\n", WordNetDatFlV[wfl].CStr(), NSynSet, MapIdH.Len());
    fclose(Fl);
  }
  IAssert(NSynSet == MapIdH.Len()); // all pointers found have to point to parsed sysets
  WnPt->SynIdToOffV.Gen(MapIdH.Len());
  for (f = 0; f < MapIdH.Len(); f++) {
    WnPt->SynIdToOffV[f] = MapIdH[f];
  }
  // build SenseV
  TVec<TIntV> WIdToSenseV(WnPt->GetWords());
  for (int s = 0; s < WnPt->GetSynsets(); s++) {
    IAssert(WnPt->GetWords(s) > 0);
    for (int w = 0; w < WnPt->GetWords(s); w++)
      WIdToSenseV[WnPt->GetWId(s, w)].Add(s);
  }
  printf("\n");
  for (int wid = 0; wid < WnPt->GetWords(); wid++) {
    if (WIdToSenseV[wid].Empty()) WnPt->WrdToIdH[wid] = -1;
    else {
      WIdToSenseV[wid].Sort();
      WnPt->AddSenseV(wid, WIdToSenseV[wid]); // note: also some glosses have a sense
    }
  }
  WnPt->PrintStats(stdout);
  return WnPt;
}

void TWordNet::PrintSyn(const int& SynId, bool Gloss, int MxWrds, FILE *OutF) const {
  MxWrds = TMath::Mn(MxWrds, GetWords(SynId));
  fprintf(OutF, "[%06d]%c (W%02d P%03d): %s", SynId, GetSynCatCh(GetSynCat(SynId)), GetWords(SynId), GetPtrs(SynId), GetWord(SynId, 0).CStr());
  for (int i = 1; i < MxWrds; i++) fprintf(OutF, ", %s", GetWord(SynId, i).CStr());
  if (Gloss) fprintf(OutF, " | %s\n", GetGloss(SynId).CStr()); else fprintf(OutF, "\n");
}

void TWordNet::PrintStats(FILE *OutF) const {
  fprintf(OutF, "WordNet statistics:\n");
  fprintf(OutF, "  Unique Words: %d (glosses are also words)\n", GetWords());
  int NounSyns=0, VerbSyns=0, AdjSyns=0, AdvSyns=0;
  int NounPtrs=0, VerbPtrs=0, AdjPtrs=0, AdvPtrs=0;
  for (int synid = 0; synid < GetSynsets(); synid++) {
    switch (GetSynCat(synid)) {
      case wscNoun : NounPtrs += GetPtrs(synid);  NounSyns++; break;
      case wscVerb : VerbPtrs += GetPtrs(synid);  VerbSyns++; break;
      case wscAdj : AdjPtrs += GetPtrs(synid);  AdjSyns++; break;
      case wscAdv : AdvPtrs += GetPtrs(synid);  AdvSyns++; break;
      default: Fail;
    }
  }
  IAssert(NounSyns+VerbSyns+AdvSyns+AdjSyns == GetSynsets());
  fprintf(OutF, "  Synsets: %d.  Pointers: %d\n", GetSynsets(),  NounPtrs+VerbPtrs+AdvPtrs+AdjPtrs);
  fprintf(OutF, "    Noun synsets: %d.  Pointers: %d\n", NounSyns, NounPtrs);
  fprintf(OutF, "    Verb synsets: %d.  Pointers: %d\n", VerbSyns, VerbPtrs);
  fprintf(OutF, "    Adjective synsets: %d.  Pointers: %d\n", AdjSyns, AdjPtrs);
  fprintf(OutF, "    Adverb synsets: %d.  Pointers: %d\n", AdvSyns, AdvPtrs);
  int WrdWithSense = 0;
  for (int w = 0; w < WrdToIdH.Len(); w++) {
    if (WrdToIdH[w] != -1) WrdWithSense++;
  }
  fprintf(OutF, "  Words with a sense: %d (words occuring in synsets)\n", WrdWithSense);
  fprintf(OutF, "  Words with no sense: %d (primarily glosses)\n", WrdToIdH.Len() - WrdWithSense);
  fprintf(OutF, "\n  Data structure sizes:\n");
  fprintf(OutF, "    Word-to-Id hash: len: %d\n", WrdToIdH.Len());
  fprintf(OutF, "                   ports: %d ints\n", WrdToIdH.GetPorts());
  fprintf(OutF, "        string pool size: %d bytes\n", WrdToIdH.GetPool()->Len());
  fprintf(OutF, "    SynId-to-SynsetOffset: len: %d\n", SynIdToOffV.Len());
  fprintf(OutF, "    SynsetStorage len: %d ints\n", SynsetS.Len());
  fprintf(OutF, "    Sense vector: len: %d ints\n", SenseV.Len());
}

TWnPtrTy TWordNet::GetPtrTy(const TStr& PtrStr) {
  static THash<TStr, TInt> PtrStrToTyH(19);
  if (PtrStrToTyH.Empty()) {
    IAssert(PtrStrDef[wptMx] == 0);
    for (int i = 0; i < wptMx; i++)
      if (strlen(PtrStrDef[i])) PtrStrToTyH.AddDat(PtrStrDef[i], TWnPtrTy(i));
  }
  TInt PtrTy ;
  if (PtrStrToTyH.IsKeyGetDat(PtrStr, PtrTy)) {
    IAssert(wptUndef < PtrTy && PtrTy < wptMx);
    return TWnPtrTy(PtrTy.Val);
  }
  Fail;
  return wptUndef;
}

TWnSynCat TWordNet::GetSynCat(char SynCatCh) {
  switch (SynCatCh) {
    case ('n') : return wscNoun;
    case ('v') : return wscVerb;
    case ('a') : return wscAdj;
    case ('s') : return wscAdj; // adjective satellite
    case ('r') : return wscAdv;
    default: TExcept::Throw("TWordNet::Unknown syntactic category " + SynCatCh);
  };
  return wscUndef;
}

void TWordNet::WordNetSample(const TStr& SearchWrd, const TStr& WnFNm) {
  PWordNet WnPt = TWordNet::Load(TFIn(WnFNm));
  TIntV SynIdV;
  int i, level = 0;

  WnPt->GetSynsetV(SearchWrd, SynIdV);
  if (SynIdV.Empty()) { printf("Word '%s' is can not be found in WordNet.\n", SearchWrd.CStr()); return; }

  printf("Search word: '%s' has %d senses:\n", SearchWrd.CStr(), SynIdV.Len());
  for (i = 0; i < SynIdV.Len(); i++) {
    printf("  "); WnPt->PrintSyn(SynIdV[i], false); }

  printf("\nRandom sense of %s:\n  ", SearchWrd.CStr());
  TRnd Rnd(0);
  const int TopSynId = SynIdV[Rnd.GetUniDevInt(SynIdV.Len())];
  WnPt->PrintSyn(TopSynId, true, 3);
  
  printf("\nDisplay all hypernyms:\n");
  int SynId = TopSynId;
  while (true) {
    for (i = 0; i < 2 * level; i++) printf(" ");
    WnPt->PrintSyn(SynId, false, 3);
    for (i = 0; i < WnPt->GetPtrs(SynId); i++)
      if (WnPt->GetPtr(SynId, i).PtrTy == wptHyper) {
        SynId = WnPt->GetPtr(SynId, i).SynId;
        break;
      }
    if (i == WnPt->GetPtrs(SynId)) break;
    level++;
  }
  
  printf("\nDisplay all hyponyms:\n");
  SynId = TopSynId;
  while (true) {
    for (i = 0; i < 2 * level; i++) printf(" ");
    WnPt->PrintSyn(SynId, false, 3);
    for (i = 0; i < WnPt->GetPtrs(SynId); i++)
      if (WnPt->GetPtr(SynId, i).PtrTy == wptHypo) {
        SynId = WnPt->GetPtr(SynId, i).SynId;
        break;
      }
    if (i == WnPt->GetPtrs(SynId)) break;
    level++;
  }
  
  printf("\nAll pointers from synset:\n");
  for (i = 0; i < WnPt->GetPtrs(SynId); i++) {
    printf("  %s  ", TWordNet::GetPtrStr(WnPt->GetPtr(SynId, i).PtrTy).CStr());
    WnPt->PrintSyn(WnPt->GetPtr(SynId, i).SynId, false, 3);
  }
  
  printf("\nRandom walk:\n");
  SynId = TopSynId;
  WnPt->PrintSyn(SynId, false, 3);
  for (i = 0; i < 10; i++) {
    if (WnPt->GetPtrs(SynId) == 0) break;
    int PtrN = TInt::GetRnd(WnPt->GetPtrs(SynId));
    const TSynPtr Ptr = WnPt->GetPtr(SynId, PtrN);
    SynId = Ptr.SynId;
    printf("  using %s: ", TWordNet::GetPtrStr(Ptr.PtrTy).CStr());   WnPt->PrintSyn(SynId, false, 3);
  }
}

char *(TWordNet::SynCatStr []) = { "Undef", "Noun", "Verb", "Adj", "Adv"};

char TWordNet::SynCatCh [] = {" nvar"};

char *(TWordNet::PtrTyStr []) = {
  "Undef", "Ant", "Hyper", "Hypo", "Entail", "Sim",
  "IsMemb", "IsStuff", "IsPart", "HasMemb", "HasStuff", "HasPart",
  "Mero", "Holo", "Cause", "Particip", "AlsoSee", "Pert", "Attr", "VrbGrp", 
  "Nominal", "ClsCategory", "ClsRegion", "ClsUsage",
  "MemCategory", "MemRegion", "MemUsage", 0
};

char *(TWordNet::PtrTyDesc []) = {
  "Undef", "Antonym", "Hypernym", "Hyponym", "Entailment", "Similarity",
  "Member Holonym", "Substance Holonym", "Part Holonym",
  "Member Meronym", "Substance Meronym", "Part Meronym",
  "Meronym", "Holonym", "Cause To", "Participle", "Also See", "Pertainym",
  "Attribute", "Verb Group", "Nominalization", "Classification Category", "Classification Region", "Classification Usage",
  "Member Category", "Member Region", "Member Usage", 0
};

// Pointer characters and searches
char *(TWordNet::PtrStrDef []) = {
    "",       // 0 not used
    "!",      // 1 ANTPTR
    "@",      // 2 HYPERPTR
    "~",      // 3 HYPOPTR
    "*",      // 4 ENTAILPTR
    "&",      // 5 SIMPTR
    "#m",     // 6 ISMEMBERPTR
    "#s",     // 7 ISSTUFFPTR
    "#p",     // 8 ISPARTPTR
    "%m",     // 9 HASMEMBERPTR
    "%s",     // 10 HASSTUFFPTR
    "%p",     // 11 HASPARTPTR
    "%",      // 12 MERONYM
    "#",      // 13 HOLONYM
    ">",      // 14 CAUSETO
    "<",      // 15 PPLPTR
    "^",      // 16 SEEALSO
    "\\",     // 17 PERTPTR
    "=",      // 18 ATTRIBUTE
    "$",      // 19 VERBGROUP
    "+",      // 20 NOMINALIZATIONS
    ";c",     // ;c CLASSIFICATION   Domain of synset - CATEGORY 
    ";r",     // ;r CLASSIFICATION   Domain of synset - REGION 
    ";u",     // ;u CLASSIFICATION   Domain of synset - USAGE 
    "-c",     // -c CLASS            Member of this domain - CATEGORY 
    "-r",     // -r CLASS            Member of this domain - REGION 
    "-u",     // -u CLASS            Member of this domain - USAGE 
    0
};

char *(TWordNet::LexFNms []) = {
    "adj.all",            // 0
    "adj.pert",           // 1
    "adv.all",            // 2
    "noun.Tops",          // 3
    "noun.act",           // 4
    "noun.animal",        // 5
    "noun.artifact",      // 6
    "noun.attribute",     // 7
    "noun.body",          // 8
    "noun.cognition",     // 9
    "noun.communication", // 10
    "noun.event",         // 11
    "noun.feeling",       // 12
    "noun.food",          // 13
    "noun.group",         // 14
    "noun.location",      // 15
    "noun.motive",        // 16
    "noun.object",        // 17
    "noun.person",        // 18
    "noun.phenomenon",    // 19
    "noun.plant",         // 20
    "noun.possession",    // 21
    "noun.process",       // 22
    "noun.quantity",      // 23
    "noun.relation",      // 24
    "noun.shape",         // 25
    "noun.state",         // 26
    "noun.substance",     // 27
    "noun.time",          // 28
    "verb.body",          // 29
    "verb.change",        // 30
    "verb.cognition",     // 31
    "verb.communication", // 32
    "verb.competition",   // 33
    "verb.consumption",   // 34
    "verb.contact",       // 35
    "verb.creation",      // 36
    "verb.emotion",       // 37
    "verb.motion",        // 38
    "verb.perception",    // 39
    "verb.possession",    // 40
    "verb.social",        // 41
    "verb.stative",       // 42
    "verb.weather",       // 43
    "adj.ppl",            // 44
    0
};