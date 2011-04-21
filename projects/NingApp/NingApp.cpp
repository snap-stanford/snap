#include "stdafx.h"
//#include "ning.h"

/////////////////////////////////////////////////
// Simple JSON parser (does not support nested objects and arrays)
class TJsonItem {
private:
  THash<TChA, TChA> KeyValH;               // key-value pairs
  THash<TChA, TJsonItem> KeyObjH;          // key-nested object
  THash<TChA, TVec<TJsonItem> > KeyArrayH; // key-array of objects
public:
  TJsonItem() { }
  TJsonItem(const TStr& JsonStr) { Parse(JsonStr.CStr()); }
  TJsonItem(const TChA& JsonStr) { Parse(JsonStr.CStr()); }
  TJsonItem(const char* JsonStr) { Parse(JsonStr); }
  const TChA& operator[](const int& KeyId) const { return KeyValH[KeyId]; }
  const TChA& operator[](const TChA& Key) const { return KeyValH.GetDat(Key); }
  const TChA& GetDat(const TChA& Key) const { return KeyValH.GetDat(Key); }
  bool IsKey(const TChA& Key) const { return KeyValH.IsKey(Key); }
  int Len() const { return KeyValH.Len(); }
  void Clr() { 
    KeyValH.Clr(false, 1000, false);
    KeyObjH.Clr(false, 1000, false);
    KeyArrayH.Clr(false, 1000, false);
  }
  const char* GetStr(const char* Beg, TChA& Str) const {
    Str.Clr();
    const char* c = Beg+1;  IAssert(*Beg=='"');
    while (*c && *c != '"') {
      if (*c!='\\') { Str.Push(*c); }
      else {  c++;
        switch (*c) {
          case '"' : Str.Push('"'); break;
          case '\\' : Str.Push('\\'); break;
          case '/' : Str.Push('/'); break;
          case 'b' : Str.Push('\b'); break;
          case 'f' : Str.Push('\f'); break;
          case 'n' : Str.Push('\n'); break;
          case 't' : Str.Push('\t'); break;
          case 'r' : Str.Push('\r'); break;
          case 'u' : break; //printf("Unicode not supported: '%s'", Beg); break;
          default : FailR(TStr::Fmt("Unknown escape sequence: '%s'", Beg).CStr());
        };
      }
      c++;
    }
    if (*c && *c=='"') { c++; }
    return c;
  }
  const char* ParseArrayVal(const char* JsonStr) {
    const char *c = JsonStr;
    bool Nested = false;
    TChA ValStr;
    Clr();
    while (*c && TCh::IsWs(*c)) { c++; }
    if (*c == '"') { c = GetStr(c, ValStr); } // string
    else if (TCh::IsNum(*c) || (*c=='-' &&  TCh::IsNum(*(c+1)))) {  // number
      while (*c && *c!=',' && *c!='}' && *c!=']' && ! TCh::IsWs(*c)) { ValStr.Push(*c); c++; } }
    else if (*c=='t' || *c=='f' || *c=='n') { // true, false, null
      while (*c && *c!=',' && *c!='}' && *c!=']') { ValStr.Push(*c); c++; } }
    else if (*c=='{') { // nested object
      TJsonItem& Obj = KeyObjH.AddDat("key");
      c = Obj.Parse(c) + 1;  Nested = true;
    }
    else if (*c=='[') { // array
      TVec<TJsonItem>& Array = KeyArrayH.AddDat("key");
        c++;
        while (*c && *c!=']') {
          while (*c && TCh::IsWs(*c)) { c++; }
          Array.Add();
          if (*c=='{') { c = Array.Last().Parse(c) + 1; } // nested object
          else { c = Array.Last().ParseArrayVal(c); }
          if (*c && *c==',') { c++; }
        }
        c++; Nested = true;
    }
    if (! Nested) {
      IAssert(! KeyValH.IsKey("key"));
      KeyValH.AddDat("key", ValStr); 
    }
    while (*c && TCh::IsWs(*c)) { c++; }
    return c;
  }
  const char* Parse(const char* JsonStr) {
    TChA KeyStr, ValStr;
    Clr();
    const char *c = JsonStr;
    bool Nested = false;
    while (*c && *c!='{') { c++; } // find first '{'
    while (*c && *c!='}') {
      while (*c && *c!='"') { c++; }
      c = GetStr(c, KeyStr); // key -- string
      while (*c && TCh::IsWs(*c)) { c++; }
      IAssert(*c==':'); c++;
      while (*c && TCh::IsWs(*c)) { c++; }
      // value
      ValStr.Clr();
      if (*c == '"') { c = GetStr(c, ValStr); } // string
      else if (TCh::IsNum(*c) || (*c=='-' &&  TCh::IsNum(*(c+1)))) {  // number
        while (*c && *c!=',' && *c!='}' && *c!=']' && ! TCh::IsWs(*c)) { ValStr.Push(*c); c++; } }
      else if (*c=='t' || *c=='f' || *c=='n') { // true, false, null
        while (*c && *c!=',' && *c!='}' && *c!=']') { ValStr.Push(*c); c++; } }
      else if (*c=='{') { // nested object
        TJsonItem& Obj = KeyObjH.AddDat(KeyStr);
        c = Obj.Parse(c) + 1;  Nested = true;
      }
      else if (*c=='[') { // array
        TVec<TJsonItem>& Array = KeyArrayH.AddDat(KeyStr);
        c++;
        while (*c && *c!=']') {
          while (*c && TCh::IsWs(*c)) { c++; }
          Array.Add();
          if (*c=='{') { c = Array.Last().Parse(c) + 1; } // nested object
          else { c = Array.Last().ParseArrayVal(c); }
          if (*c && *c==',') { c++; }
        }
        c++; Nested = true;
      }
      else { FailR(TStr::Fmt("Unknown character '%c' at position %d in %s", *c, int(c-JsonStr), JsonStr).CStr()); }
      if (! Nested) {
        KeyValH.AddDat(KeyStr, ValStr); }
      while (*c && TCh::IsWs(*c)) { c++; }
      if (*c && *c==',') { c++; }
      while (*c && TCh::IsWs(*c)) { c++; }
    }
    return c;
  }

  void Dump(const int& Indent=0) const {
    // short outputs
    if (KeyValH.Len()==0 && KeyArrayH.Len()==0 && KeyObjH.Len()==0) {
      for (int x=0;x<Indent;x++){printf(" ");} 
      printf("{ }\n", KeyValH.GetKey(0).CStr(), KeyValH[0].CStr());
      return;
    }
    if (KeyValH.Len()==1 && KeyArrayH.Len()==0 && KeyObjH.Len()==0) {
      for (int x=0;x<Indent;x++){printf(" ");} 
      printf("{ %s : %s }\n", KeyValH.GetKey(0).CStr(), KeyValH[0].CStr());
      return;
    }
    // long output
    for (int x=0;x<Indent;x++){printf(" ");} printf("{\n");
    for (int i = 0; i < KeyValH.Len(); i++) {
      for (int x=0;x<Indent;x++){printf(" ");}
      printf("%s : %s\n", KeyValH.GetKey(i).CStr(), KeyValH[i].CStr());
    }
    if (KeyArrayH.Len() > 0) {
      for (int i = 0; i < KeyArrayH.Len(); i++) {
        const TVec<TJsonItem>& Array = KeyArrayH[i];
        for (int x=0;x<Indent;x++){printf(" ");} printf("%s : [\n", KeyArrayH.GetKey(i).CStr());
        for (int a = 0; a < Array.Len(); a++) { Array[a].Dump(Indent+2); }
        for (int x=0;x<Indent;x++){printf(" ");} printf("]\n");
      }
    }
    if (KeyObjH.Len() > 0) {
      for (int i = 0; i < KeyObjH.Len(); i++) {
        for (int x=0;x<Indent;x++){printf(" ");} 
        printf("%s : \n", KeyObjH.GetKey(i).CStr());
        KeyObjH[i].Dump(Indent+2);
      }
    }
    for (int x=0;x<Indent;x++){printf(" ");} printf("}\n");
  }
};

/////////////////////////////////////////////////
// Simple JSON parser
class TJsonLoader {
private:
  PSIn SIn;
  TFFile FFile;
  TChA Line;
  TJsonItem Item;
  int ItemCnt;
  TExeTm ExeTm;
public:
  TJsonLoader(const TStr& FNmWc) : FFile(FNmWc), ItemCnt(0) { }
  const TChA& operator[](const int& KeyId) const { return Item[KeyId]; }
  const TChA& operator[](const TChA& Key) const { return Item[Key]; }
  const TChA& GetDat(const TChA& Key) const { return Item.GetDat(Key); }
  bool IsKey(const TChA& Key) const { return Item.IsKey(Key); }
  int Len() const { return Item.Len(); }
  void Dump() const { Item.Dump(0); }
  bool Next() {
    if (SIn.Empty() || SIn->Eof()) {
      TStr FNm;
      if (! SIn.Empty()) { printf("  %d items\t[%s]\n", ItemCnt, ExeTm.GetTmStr()); }
      if (! FFile.Next(FNm)) { return false; }
      printf("Parsing: %s\n", FNm.CStr()); ItemCnt=0;
      if (TZipIn::IsZipExt(FNm.GetFExt())) { SIn=TZipIn::New(FNm); }
      else { SIn=TFIn::New(FNm); }
      ExeTm.Tick();
    }
    SIn->GetNextLn(Line);
    Item.Parse(Line.CStr());  ItemCnt++;
    return true;
  }
};


int main(int argc, char* argv[]) {
  printf("NingApp. build: %s, %s. Start time: %s\n\n", __TIME__, __DATE__, TExeTm::GetCurTm());
  TExeTm ExeTm;  TInt::Rnd.PutSeed(0);  Try  //TSysProc::SetLowPriority();
  
  const int ItemStepCnt = Kilo(10);
  int ItemCnt=0, SmallCnt=ItemStepCnt;
  //TJsonLoader J("W:\\code\\projects\\tweets1k.txt");//"*.log.gz");
  TJsonLoader J("/lfs/1/tmp/twitter-jan2011/*.log.rar");
  TExeTm Time1k;
  while (J.Next()) {
    ItemCnt++; SmallCnt--;
    //printf("\n-----------------------------------------------------\n"); J.Dump();
    if (SmallCnt == 0) {
      printf("\r%dm items. Time to read %dk: %gs. ", ItemCnt/Mega(1), ItemStepCnt/1000, Time1k.GetSecs());
      fflush(stdout);
      Time1k.Tick();  SmallCnt = ItemStepCnt;
    }
  }
  

  //TNingUsrBs UsrBs(TZipIn("Ning-UsrBs.bin.rar"), false);  printf("load UsrBs done [%s].\n", ExeTm.GetStr());
  //
  //TNingNets Nets(TZipIn("data/Ning-FriendReq.ningTmNets.rar"));
  //TNingNets Nets(TZipIn("data/Ning-Comment.ningTmNets.rar"));
  //TNingNets Nets(TZipIn("data/Ning-EventAttend.ningTmNets.rar"));
  //Nets.SaveTxtStat("Comment-rnd1k_XXX");
  
  /*
  THash<TInt, TInt> H;
  TSsParser Ss("W:\\Data\\OrangePortugal\\result_CALL_DATE_20060402.txt.gz", ssfSpaceSep, true, true, true);
  while (Ss.Next()) {
    //printf("%s\n", Ss[0]);
    H.AddDat(Ss.GetInt(0)) ++;
  }
  //H.SortByDat(false);
  TGnuPlot::PlotValCntH(H, "test", "test", "custromer", "count", gpsLog, false, gpwPoints);
  for (int i = 0; i < H.Len(); i++) {
    printf("%d\t%d\n", H.GetKey(i), H[i]); // H.GetKeyId(key); H.GetDat(key)
  }
  for (THash<TInt,TInt>::TIter I = H.BegI(); I < H.EndI(); I++) {
    //;printf("%d\t%d\n", I.GetKey(), I.GetDat());
  }
  
  // gnuplot
  TFltPrV XY, XY2;
  for (int i =0; i < 100; i++) {
    XY.Add(TFltPr(i, i*i));
    XY2.Add(TFltPr(i, i*i*i));
  }
  TGnuPlot Gp("plot1", "Polo title");
  Gp.AddPlot(XY, gpwLinesPoints, "quadratic");
  int id = Gp.AddPlot(XY2, gpwLinesPoints, "cubic");
  Gp.AddPwrFit(id, gpwLines, "lw 5");
  Gp.SetXYLabel("x", "y");
  Gp.SavePng();
  { TFOut FOut("a.bin");
  XY.Save(FOut); }
  { TFIn FIn("a.bin");
  XY.Load(FIn); }

  // graph
  PUNGraph G = TUNGraph::New();
  //G = TSnap::GenCircle<PNGraph>(100, 2);
  G = TSnap::GenRndGnm<PUNGraph>(100, 200, true);
  { TFOut FOut("g.bin");
  G->Save(FOut); }
  //G->Load(FIn);
  for (TUNGraph::TNodeI I = G->BegNI(); I < G->EndNI(); I++) {
    printf("%d\t%d\n", I.GetId(), I.GetOutDeg());
  }
  TSnap::PlotOutDegDistr(G, "outDeg", "GNM");
  PUNGraph Wcc = TSnap::GetMxWcc(G);
  */
  
  CatchFull
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}

void BuildNetsAndStats() {
  //TGroupBs GroupBs;
  //GroupBs.ParseGroups(UsrBs);
  //GroupBs.Save(TFOut("Ning-GroupBs.bin"));
  
  // ning user ids and user network statistics
  //TNingUsrBs UsrBs;
  //UsrBs.CollectStat();
  //UsrBs.Save(TFOut("NingUsrBs.bin"));

/*  TNingUsrBs UsrBs(TZipIn("NingUsrBs.bin.rar"), false);  printf("load UsrBs done\n");
  // friend request
  { TNingNets Nets;
  Nets.ParseNetworks("friendrequestmessage", UsrBs);
  Nets.Save(TFOut("Ning-FriendReq.ningTmNets")); }
  // event attendee
  { TNingNets Nets;
  Nets.ParseNetworks("eventattendee", UsrBs);
  Nets.Save(TFOut("Ning-EventAttend.ningTmNets")); }
  // comment network
  { TNingNets Nets;
  Nets.ParseNetworks("comment", UsrBs);
  Nets.Save(TFOut("Ning-Comment.ningTmNets")); } //*/

  // simple network statistics
  /*TNingAppStat NingAppStat;
  NingAppStat.CollectStat();
  NingAppStat.Save(TFOut("NingAppStat2.bin"));
  NingAppStat.SaveTab("NingAppStat2.tab", 5);
  NingAppStat.PlotAll("NingAppStat2"); //*/
  /*TNingAppStatsDat NASD;
  NASD.Parse("data/app_stats.dat.gz");
  NASD.SaveTab("app_stats.dat.tab", 5);
  NASD.PlotAll("app_stats"); //*/
}

/*
void Test() {
  TJsonLoader J(NingPath+"comment\\*.gz");
  TStrHash<TInt> UH;
  int MinApp=TInt::Mx, MaxApp=0;
  while (J.Next()) {
    if (! J.IsKey("author") || ! J.IsKey("appId")) { continue; }
    const int keyid = UH.AddKey(J.GetDat("author"));
    const int AppId = atoi(J.GetDat("appId").CStr());
    if (J.GetDat("author") == "2b79zrwpy6mm1") {
      printf("key, app: %d\td\n", keyid, AppId); }
    if (UH[keyid] == 0) { 
      UH[keyid] = AppId; }
    else if (UH[keyid] != AppId) {
      printf("User %s is in two networks: ", J.GetDat("author").CStr());
      printf("%d and %s\n", UH[keyid], J.GetDat("appId").CStr());
    }
    MaxApp=TMath::Mx(MaxApp, AppId);
    MinApp=TMath::Mn(MinApp, AppId);
  }
  printf("%d users\n MinApp: %d\nMaxApp: %d\n", UH.Len(), MinApp, MaxApp);
}
*/
