#include "stdafx.h"
#include "ning.h"

void TestTwitter();

int main(int argc, char* argv[]) {
  printf("NingApp. build: %s, %s. Start time: %s\n\n", __TIME__, __DATE__, TExeTm::GetCurTm());
  TExeTm ExeTm;  TInt::Rnd.PutSeed(0);  Try  //TSysProc::SetLowPriority();
  // TestTwitter();

  // ning user ids and user network statistics
  TNingUsrBs UsrBs;
  UsrBs.ParseUsers("W:\\xData\\Ning\\comment\\comment-*.gz ");
  UsrBs.Save(TFOut("NingUsrBs.bin"));


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
