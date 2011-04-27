#include "stdafx.h"
#include "ning.h"

void TestTwitter();
void LoadNingNetBs();

int main(int argc, char* argv[]) {
  printf("NingApp. build: %s, %s. Start time: %s\n\n", __TIME__, __DATE__, TExeTm::GetCurTm());
  TExeTm ExeTm;  TInt::Rnd.PutSeed(0);  Try  //TSysProc::SetLowPriority();
  // TestTwitter();

  //PUNGraph G = TUNGraph::GetSmallGraph();  TGraphViz::Plot(G, gvlNeato, "a.gif", "", true);
  //printf("%d\n", TSnap::GetTriadEdges(G));
  //TNingUsrBs UsrBs(TZipIn("nets/NingUsrBs.bin.rar"));
  TNingGroupBs GroupBs(TZipIn("nets/NingGroupBs.bin.rar"));
  GroupBs.PlotGroupStat("ningComment");

  //PNingNetBs NetBs = TNingNetBs::New(TZipIn("nets/NingNetBs.bin.rar")); 
  //printf("Loading NetBs done. %d nets [%s]\n", NetBs->Len(), ExeTm.GetStr());
  //NetBs->SaveTxtStat("ningComment2", 10, Mega(100));
  //NetBs->PlotDeadStat("ningComment");
  //NetBs->PlotSimNodesEvol(8994, 100, 90, 365, 10);
    
  
  /*int n = 0;
  for(TZipIn ZipIn("nets/NingNetBs.nets_bin.rar"); ! ZipIn.Eof(); n++) {
    PNingNet Net = TNingNet::Load(ZipIn);
    if (Net->GetAge(tmuDay) < 365) { continue; }
    if (Net->GetNodes() >= 1000 && Net->GetNodes() <= 1200) {
      Net->PlotOverTime(TStr::Fmt("net1k-%06d", n)); }
    //if (Net->GetNodes() >= 100 && Net->GetNodes() <= 110) {
   //   Net->PlotOverTime(TStr::Fmt("net100-%06d", n)); }
  } //*/

  //
  /*PNingNet Net = TNingNet::Load(TFIn("W:\\code\\projects\\NingApp\\AWorkDir\\NingNet-13267.bin"));
  printf("done. %gs\n", ExeTm.GetSecs()); ExeTm.Tick();
  printf("sort\n");
  Net->SortNIdByDat(true);
  printf("done. %gs\n", ExeTm.GetSecs()); ExeTm.Tick();
  Net->SortEIdByDat(true);
  printf("done. %gs\n", ExeTm.GetSecs()); ExeTm.Tick();
  /*TIntV V;
  for (TNingNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
    V.Add(EI().GetAbsSecs());
    //printf("%d\n", EI().GetAbsSecs());
  }
  V.Save(TFOut("v.bin"));*/
  /*TIntV V(TFIn("v.bin"));
  ExeTm.Tick();
  printf("sort2: %d\n", V.Len());
  V.SortCmp(TLss<TInt>());
  printf("done. %gs", ExeTm.GetSecs()); ExeTm.Tick();
  */
  /*TNingUsrBs UsrBs(TFIn("NingUsrBs.bin"));
  THashSet<TIntPr> AppUsrH(TFIn("AppUsrH.bin")); 
  THash<TInt,TInt> AppCntH(TFIn("AppCntH.bin")); 
  THash<TInt,TInt> UsrCntH(TFIn("UsrCntH.bin")); 
 
  printf("%d users in all apps\n", UsrCntH.Len());
  printf("%d apps\n", AppCntH.Len());
  printf("%d unique user-app pairs\n", AppUsrH.Len());
  { TIntH ValCntH;
  for (int i=0; i < AppCntH.Len(); i++) { ValCntH.AddDat(AppCntH[i])++; }
  TGnuPlot::PlotValCntH(ValCntH, "ning-appComments", "Ning App number of comments distribution", "Number of comments in the app", "Number of such apps", gpsLog); }
  { TIntH ValCntH;
  for (int i=0; i < UsrCntH.Len(); i++) { ValCntH.AddDat(UsrCntH[i])++; }
  TGnuPlot::PlotValCntH(ValCntH, "ning-usrComments", "Number of comments of a user (over all apps)", "Number of comments of a user", "Number of such users", gpsLog); }
  { TIntH UsrAppCntH, UsrH;
  for (int i =0; i < AppUsrH.Len(); i++) { UsrAppCntH.AddDat(AppUsrH[i].Val1)++; }
  for (int i =0; i < UsrAppCntH.Len(); i++) { UsrH.AddDat(UsrAppCntH[i])++; }
  TGnuPlot::PlotValCntH(UsrH, "ning-usrApps", "Number of apps of a user", "Number of apps a user is memeber of", "Number of such users", gpsLog); }
  { TIntH UsrAppCntH, UsrH;
  for (int i =0; i < AppUsrH.Len(); i++) { UsrAppCntH.AddDat(AppUsrH[i].Val2)++; }
  for (int i =0; i < UsrAppCntH.Len(); i++) { UsrH.AddDat(UsrAppCntH[i])++; }
  TGnuPlot::PlotValCntH(UsrH, "ning-appSize", "Number of apps of a user", "Number of users in the app", "Number of such apps", gpsLog); }
  

  //TNingUsrBs UsrBs(TZipIn("Ning-UsrBs.bin.rar"), false);  printf("load UsrBs done [%s].\n", ExeTm.GetStr());
  //
  //TNingNets Nets(TZipIn("data/Ning-FriendReq.NingNets.rar"));
  //TNingNets Nets(TZipIn("data/Ning-Comment.NingNets.rar"));
  //TNingNets Nets(TZipIn("data/Ning-EventAttend.NingNets.rar"));
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

void LoadNingNetBs() {
  //// Load Ning user ids
  //TNingUsrBs UsrBs;
  //UsrBs.ParseUsers("W:\\xData\\Ning\\comment\\comment-*.gz", "Comment");
  //UsrBs.Save(TFOut("NingUsrBs.bin"));
  //// Load Ning nets and fill-in UsrBs
  TNingUsrBs UsrBs;
  PNingNetBs NetBs = TNingNetBs::New();
  NetBs->ParseNetworks("W:\\xData\\Ning\\comment\\comment-*.gz", UsrBs, "comment");
  NetBs->Save(TFOut("NingNetBs.bin"));
  UsrBs.Save(TFOut("NingUsrBs.bin"));
  TFOut FOut("NingNetBs.nets_bin");
  for (int i=0; i<NetBs->Len(); i++) {
    NetBs->GetNet(i)->Save(FOut); }
  //// Ning grops
  TNingGroupBs GroupBs;
  GroupBs.ParseGroups("W:\\xData\\Ning\\groupmembership\\*.gz", UsrBs);
  GroupBs.Save(TFOut("NingGroupBs.bin"));
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
  Nets.Save(TFOut("Ning-FriendReq.NingNets")); }
  // event attendee
  { TNingNets Nets;
  Nets.ParseNetworks("eventattendee", UsrBs);
  Nets.Save(TFOut("Ning-EventAttend.NingNets")); }
  // comment network
  { TNingNets Nets;
  Nets.ParseNetworks("comment", UsrBs);
  Nets.Save(TFOut("Ning-Comment.NingNets")); } //*/

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

void TestTwitter() {
  const int ItemStepCnt = Kilo(1);
  int ItemCnt=0, SmallCnt=ItemStepCnt;
  //TJsonLoader J("W:\\code\\projects\\tweets1k.txt");
  TJsonLoader J("/lfs/1/tmp/twitter-jan2011/twitter_2011011*.log.rar");
  TExeTm Time1k;
  try {
    while (J.Next()) {
      ItemCnt++;  SmallCnt--;
      //printf("\n-----------------------------------------------------\n"); J.Dump();
      if (SmallCnt == 0) {
        printf("\rTotal %d items. Time to read %dk items: %g sec.   ", ItemCnt, ItemStepCnt/1000, Time1k.GetSecs());
        Time1k.Tick();  SmallCnt = ItemStepCnt;
      }
    }
  }
  catch (PExcept Except){
    printf("%s\n", Except->GetStr().CStr());
    J.Dump();
  }
}
