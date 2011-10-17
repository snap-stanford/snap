#include "stdafx.h"
#include "ning.h"

void TestTwitter();
void LoadNingNetBs();

int main(int argc, char* argv[]) {
  printf("NingApp. build: %s, %s. Start time: %s\n\n", __TIME__, __DATE__, TExeTm::GetCurTm());
  TExeTm ExeTm;  TInt::Rnd.PutSeed(0);  Try  TSysProc::SetLowPriority();
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs("NingApp");

  /*TestTwitter();

  //TJsonLoader J("/lfs/1/tmp/twitter-jan2011/*.log.rar");
  TJsonLoader J("W:\\xData\\Twitter\\tweets1k.txt");
  while (J.Next()) {
    printf("\n-------------------------------------------------------------------------------------------------------\n");
    J.Dump();
  }*/

  //PUNGraph G = TSnap::LoadEdgeList<PUNGraph>("W:\\xData\\Epinions\\epinions.txt", 0, 1);
  //G->Save(TFOut("epin.ungraph"));
  /*TStr GNm = "answers";
  PNGraph NG = TNGraph::Load(TZipIn("W:\\xData\\aBinData\\"+GNm+".ngraph.gz"));  PUNGraph G = TSnap::ConvertGraph<PUNGraph>(NG);
  //PUNGraph G = TUNGraph::Load(TFIn("aa_epin.ungraph")); GNm="epin";

  TSnap::PrintInfo(G, GNm);
  TSnap::PlotOutDegDistr(G, GNm);
  TIntPrV DegV;
  TSnap::GetNodeOutDegV(G, DegV);
  DegV.SortCmp(TCmpPairByVal2<TInt, TInt>(false));
  TFltPrV DegDiamV, DelDiamV, DelIdDiamV;
  double MaxDeg = DegV[0].Val2;
  double EffDiam, AvgDiam, Nodes=G->GetNodes();
  int FullDiam;
  TSnap::GetBfsEffDiam(G, 100, false, EffDiam, FullDiam, AvgDiam);
  FILE *F = fopen(TStr::Fmt("%s.tab", GNm.CStr()).CStr(), "wt");
  printf("%d] %.2g [%d]:\t%d\t%.2g\t%.2g\n", DelIdDiamV.Len(), 0/Nodes, DegV[0].Val2, FullDiam, EffDiam, AvgDiam);
  fprintf(F, "Id\tMaxDeg\tFracDel\tFullDiam\tEffDiam\tAvgDiam\n");
  fprintf(F, "%d\t%d\t%f\t%d\t%f\t%f\n", DelIdDiamV.Len(), DegV[0].Val2, 0/Nodes, FullDiam, EffDiam, AvgDiam);
  DegDiamV.Add(TFltPr(MaxDeg, EffDiam));
  DelDiamV.Add(TFltPr(1.0/Nodes, EffDiam));
  DelIdDiamV.Add(TFltPr(DelIdDiamV.Len(), EffDiam));
  for (int i = 0; i < DegV.Len()-1; ) {
    int previ = i;
    while (i < DegV.Len()-1 && DegV[i].Val2 >= MaxDeg) {
      G->DelNode(DegV[i].Val1); i++; }
    MaxDeg *= 0.8;
    if (i == previ) { continue; }
    TSnap::GetBfsEffDiam(G, 100, false, EffDiam, FullDiam, AvgDiam);
    printf("%d] %.2g [%d]:\t%d\t%.2g\t%.2g\n", DelIdDiamV.Len(), double(i), DegV[i].Val2, FullDiam, EffDiam, AvgDiam);
    fprintf(F, "%d\t%d\t%f\t%d\t%f\t%f\n", DelIdDiamV.Len(), DegV[i].Val2, i/Nodes, FullDiam, EffDiam, AvgDiam);
    DegDiamV.Add(TFltPr(DegV[i].Val2(), EffDiam));
    DelDiamV.Add(TFltPr(i/Nodes, EffDiam));
    DelIdDiamV.Add(TFltPr(DelIdDiamV.Len(), EffDiam));
    TGnuPlot::PlotValV(DegDiamV, "DegDiam-"+GNm, "Maximum node degre in the network vs. effective diameter", "Maximum Degree", "Effective Diameter");
    TGnuPlot::PlotValV(DelDiamV, "Del1Diam-"+GNm, "Fraction of deleted nodes vs. effective diameter", "Fraction deleted", "Effective Diameter", gpsLog10X);
    TGnuPlot::PlotValV(DelIdDiamV, "Del2Diam-"+GNm, "DeletionId vs. effective diameter", "Deletion Id", "Effective Diameter");
  }
  fclose(F);

  //PUNGraph G = TUNGraph::GetSmallGraph();
  /*PUNGraph G = TSnap::ConvertGraph<PUNGraph>(TNGraph::GetSmallGraph());
  G->AddEdge(0,3); G->AddEdge(2,4); G->AddEdge(1,4);
  TGraphViz::Plot(G, gvlNeato, "a.gif", "", true);
  int InGroupEdges=0,  InOutGroupEdges=0,  OutGroupEdges=0;
  TSnap::GetNodeTriads(G, 1, TIntSet(TIntV::GetV(0,1,2)), InGroupEdges, InOutGroupEdges, OutGroupEdges);
  printf("%d %d %d\n", InGroupEdges, InOutGroupEdges, OutGroupEdges);*/

  /*PNingNetBs NetBs = TNingNetBs::New(TZipIn("nets/NingNetBs-100n1y.bin.rar"));  printf("Loading NetBs done. %d nets [%s]\n", NetBs->Len(), ExeTm.GetStr());
  TNingGroupBs GroupBs(TZipIn("nets/NingGroupBs-10n05s60d.bin.rar"));
  //TNingUsrBs UsrBs(TZipIn("nets/NingUsrBs.bin.rar"));
  //TFOut FOut("NingNetGroup-N100n1y-G10n05s60d.bin");
  //TNingGroupEvol2 GroupEvol("Evol-AllNets");
  int netcnt=0, grpcnt=0, nskip=0;
  for (int n = 0; n < NetBs->Len(); n++) {
    const int AppId = NetBs->GetAppId(n);
    PNingNet Net = NetBs->GetNet(n);
    //if (Net->GetNodes() < 1000 || Net->GetNodes() > 1100) { continue; }
    if (! GroupBs.HasGroups(AppId)) { nskip++; continue; }
    const TNingGroupV& GV = GroupBs.GetGroupV(AppId);
    printf("\nNet %d on %d nodes and %d groups (%d total) %d skip, %d grps:", ++netcnt, Net->GetNodes(), GV.Len(), grpcnt, nskip, GroupBs.Len());
    GroupEvol.AddNet(Net, GV);  grpcnt+=GV.Len();
    //Net->Save(TFOut(TStr::Fmt("%d-net.bin", AppId)));  GV.Save(TFOut(TStr::Fmt("%d-groups.bin", AppId)));
    //Net->Save(FOut);  GV.Save(FOut);
    //if (cnt > 10) { break; }
  }
  //GroupEvol.PlotAll();
  //*/
  /*const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "Evol-at100-1000gt", "Output file name");
  const int MinNodes = Env.GetIfArgPrefixInt("-min:", 100, "Min nodes");
  int MaxNodes = Env.GetIfArgPrefixInt("-max:", -1, "Max nodes");
  if (MaxNodes == -1) { MaxNodes = Mega(100); }
  TNingGroupEvol3 GroupEvol(OutFNm);
  int netcnt=0, grpcnt=0, skipcnt=0;
  for (TFIn FIn("nets/NingNetGroup-N100n1y-G10n05s60d.bin"); ! FIn.Eof(); ) {
    PNingNet Net = TNingNet::Load(FIn);
    TNingGroupV GV(FIn);
    //if (Net->GetNodes() < MinNodes || Net->GetNodes() > MaxNodes) { skipcnt++; continue; } // final network size
    netcnt++;  grpcnt += GV.Len();
    printf("Net %d (skip %d) on %d nodes %d edges and %d groups (%d total)\n", netcnt, skipcnt, Net->GetNodes(), Net->GetEdges(), GV.Len(), grpcnt);
    //GroupEvol.AddNet(Net, GV, 100, 100, 200); // groups at 100 that at most doubled the size
    GroupEvol.AddNet(Net, GV, 100, 1000, Mega(1)); // groups at 100 that
  }
  GroupEvol.PlotAll(); //*/

  /*TNingGroupEvol2 GroupEvol("GroupEvol");
  TStr FNm;
  for (TFFile FFile("nets/*-groups.bin"); FFile.Next(FNm); ) {
    PNingNet Net = TNingNet::Load(TFIn(FNm.GetSubStr(0, FNm.SearchChBack('-'))+"net.bin"));
    TNingGroupV GV; GV.Load(TFIn(FNm));
    printf("%s. %d groups.\n", Net->GetTitle().CStr(), GV.Len());
    GroupEvol.AddNet(Net, GV);
    //GroupEvol.PlotAll();
  }
  GroupEvol.PlotAll("final"); //*/


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
  { TFOut FOut("NingNetBs.bin"); NetBs->Save(FOut); }
  { TFOut FOut("NingNetBs100n1y.bin"); NetBs->GetSubBs(100, 365, 0)->Save(FOut); }
  { TFOut FOut("NingUsrBs.bin"); UsrBs.Save(FOut); }
  TFOut FOut("NingNetBs.nets_bin");
  for (int i=0; i<NetBs->Len(); i++) {
    NetBs->GetNet(i)->Save(FOut); }
  //// Ning grops
  TNingGroupBs GroupBs;
  GroupBs.ParseGroups("W:\\xData\\Ning\\groupmembership\\*.gz", UsrBs);
  { TFOut FOut("NingGroupBs.bin"); GroupBs.Save(FOut); }
  { TFOut FOut("NingGroupBs-10n05s60d.bin"); GroupBs.GetSubBs(NetBs, 10, 0.5, 60)->Save(FOut); }
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
