#include "stdafx.h"
//#include <spinn3r.h>
//#include <memes.h>
#include "../../core/snap/memes.h"

void BigMain(int argc, char* argv[]);
void QuotesForDepression();
void DelSpam(PQuoteBs& QtBs, PClustNet& Net, TSecTm BeforeTm);
void SaveIfSubPhrase(char* Sent, const TVec<TChAV>& PhraseV, const TVec<FILE*>& FileV, const TChA& TmStr, const TChA& Url);

int main(int argc, char* argv[]) {
  printf("MemeWorks. build: %s, %s. Start time: %s\n\n", __TIME__, __DATE__, TExeTm::GetCurTm());
  //TSysProc::SetLowPriority();
  TExeTm ExeTm;  TInt::Rnd.PutSeed(0);  Try
  //BigMain(argc, argv); return 0;
  //char *ToDo [] = {"QuotesApp", "-do:mkdataset", "-i:_files.txt"};  BigMain(3, ToDo);  return 0;
  //char *ToDo [] = {"QuotesApp", "-do:memestoqtbs", "-i:f.txt"};  BigMain(3, ToDo);  return 0;
  //char *ToDo [] = {"QuotesApp", "-do:mksentences", "-i:f.txt"};  BigMain(3, ToDo);  return 0;

  THashSet<TMd5Sig> PostSet;
  //THash<TMd5Sig, TInt> DomCntH;
  TStrHash<TInt> DomCntH;
  int Posts=0, NSkip=0, NonFbTw=0;
  //for (TMemesDataLoader Memes("W:\\xData\\Spinn3r\\*.rar", true); Memes.LoadNext(); ) {
  TMemesDataLoader Memes(argv[1], false);
  //TMemesDataLoader Memes("W:\\xData\\Spinn3r\\*.txt", true);
  for (; Memes.LoadNext(); ) {
    //printf("%lld\t%s\n", Memes.LineCnt, Memes.PostUrlStr.CStr());
    TMd5Sig Url(Memes.PostUrlStr);
    if (PostSet.IsKey(Url)) { NSkip++; continue; }
    PostSet.AddKey(Url); Posts++;
    DomCntH.AddDat(TStrUtil::GetDomNm2(Memes.PostUrlStr)) += 1;
  }
  printf("Posts:  %d\nNSkip:  %d  (%f)\nNDoms:  %d\n", Posts, NSkip, NSkip/double(Posts+NSkip), DomCntH.Len());
  TIntPrV CntIdV(DomCntH.Len(), 0);
  for (int i = 0; i < DomCntH.Len(); i++) { CntIdV.Add(TIntPr(DomCntH[i], i)); }
  CntIdV.Sort(false);
  for (int i = 2; i < CntIdV.Len(); i++) { NonFbTw += CntIdV[i].Val1; }
  printf("NoFbTw: %d  (%f)\n\n", NonFbTw, NonFbTw/double(Posts));
  for (int i = 0; i < TMath::Mn(100, DomCntH.Len()); i++) { 
    printf("%d\t%s\n", DomCntH[CntIdV[i].Val2].Val, DomCntH.GetKey(CntIdV[i].Val2)); }

  //TVec<TChAV> PhraseV;  PhraseV.Add(TChAV::GetV("too", "", "to", "fail"));
  //TVec<FILE*> FileV;  FileV.Add(stdout);
  //SaveIfSubPhrase("too xxxx to fail", PhraseV, FileV, "TIME", "URL");

  /*TQuoteExtractor QE(TStr("fullSpinnr-2008-08-27d.txt.*"));
  TVec<char *> SentV;
  for (int item = 0; QE.Next(); item++) {
    TChA T = QE.PostText;
    printf("#%s#\n", T.CStr());
    TStrUtil::SplitSentences(T, SentV);
    for (int i = 0; i < SentV.Len(); i++) {
      printf("  --#%s#\n", SentV[i]);
    }
    printf("\n\n");
  }


  //////
  //TIntV MnWV = TIntV::GetV(5,6,7,8), MxWV = TIntV::GetV(10,20,30), MnFqV = TIntV::GetV(10,20,30);
  //for (int v1 = 0; v1 < MnWV.Len(); v1++) {
  //for (int v2 = 0; v2 < MxWV.Len(); v2++) {
  //for (int v3 = 0; v3 < MnFqV.Len(); v3++) {
  /*  PQuoteBs QtBs = TQuoteBs::Load(TZipIn("qt3-080901-090831-w4mfq10.QtBs.rar"));
    PClustNet Net = TClustNet::Load(TZipIn("qt3-080901-090831-w4fq10.ClustNet.rar"));
    TSnap::PrintInfo(Net);
    DelSpam(QtBs, Net, TSecTm(2008,9,5,0,0,0));
    TSnap::PrintInfo(Net);
    QtBs->Dump(false);
    Net->RecalcEdges(0.6);
    TSnap::PrintInfo(Net);
    printf("get subgraph\n");
    //const int MnW = MnWV[v1], MxW = MxWV[v2], MnFq = MnFqV[v3];
    int MnW =5, MxW=30, MnFq=10;
    TStr Pref = TStr::Fmt("qt3-8dPeak-w%d-%dfq%d", MnW, MxW, MnFq), Suff;
    Net = Net = Net->GetSubGraph(MnW, MxW, MnFq);  TSnap::PrintInfo(Net);
    printf("del self edges\n");
    TSnap::DelSelfEdges(Net);
    printf("find clusters\n");
    TIntPrV KeeepEdgeV;
    //Net->ClustGreedyTopDown(KeeepEdgeV); Suff="-TD";
    //Net->ClustKeepSingleEdge(1, KeeepEdgeV); Suff="-FQ";
    Net->ClustKeepSingleEdge(2, KeeepEdgeV);  Suff="-LG";
    //Net->ClustKeepSingleEdge(3, KeeepEdgeV); Suff="-SH";
    printf("make clusters\n");
    Net->MakeClusters(KeeepEdgeV);
    TVec<TIntV> ClustV, ClustV2;
    Net->DumpClustersByVol(TStr::Fmt("%s-w%d-%dfq%d%s", Pref.CStr(), MnW, MxW, MnFq, Suff.CStr()), 5, 100);
  //} } }
  return 0;
  Net->GetClusters(ClustV);
  printf("Skipping cluster of %d quotes\n", ClustV[0].Len());
  TIntV QtIdV;
  for (int i = 1; i < ClustV.Len(); i++) {
    if (ClustV[i].Len() >= 3) {
      ClustV2.Add(ClustV[i]);
      QtIdV.AddV(ClustV[i]);
    }
  }
  printf("Get small QtBs\n");
  PQuoteBs SmallQtBs = QtBs->GetQuoteBs(QtIdV);
  printf("Create clusters\n");
  SmallQtBs->CreateClusters(ClustV2); // set clusters
  printf("Save\n");
  //{ TFOut FOut(Pref+"-clust5.QtBs"); SmallQtBs->Save(FOut); }
  //PQuoteBs SmallQtBs = TQuoteBs::Load(TZipIn("qt3-w7-20fq20-clust3.QtBs.gz"));
  SmallQtBs->SaveClusters(Pref+Suff+"1", true);
  SmallQtBs->SaveClusters(Pref+Suff+"2", false); // */

  /*PQuoteBs QtBs;
  { TZipIn ZIn("qt3E08-w5-30fq10-clust5.QtBs.gz"); QtBs = TQuoteBs::Load(ZIn); }
  QtBs->SaveClusters("qt3E08-w5-30fq10-clust5a", true);
  QtBs->SaveClusters("qt3E08-w5-30fq10-clust5b", false);
  //*/

  CatchFull
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}

void SaveIfSubPhrase(char* Sent, const TVec<TChAV>& PhraseV, const TVec<FILE*>& FileV, const TChA& TmStr, const TChA& Url) {
  TVec<char *> WrdV;
  TChA SentTxt(Sent);
  SentTxt = TStrUtil::GetCleanWrdStr(SentTxt);
  TStrUtil::SplitWords(SentTxt, WrdV);
  for (int p = 0; p < PhraseV.Len(); p++) {
    const TChAV& Phrase = PhraseV[p];
    const int plen = Phrase.Len();
    for (int w = 0; w < WrdV.Len()-plen+1; w++) {
      int cnt = 0;
      for (int pw = 0; pw < plen; pw++) {
        if (strstr(WrdV[w+pw], Phrase[pw].CStr()) != NULL) { cnt++; }
        else { break; }
      }
      if (cnt == plen) {
        fprintf(FileV[p], "%s\t%s\t%s\n", TmStr.CStr(), Url.CStr(), Sent);
        break;
      }
    }
  }
}

void BigMain(int argc, char* argv[]) {
  TExeTm ExeTm;
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs("QuotesApp");
  const TStr ToDo = Env.GetIfArgPrefixStr("-do:", "", "To do").GetLc();
  if (Env.IsEndOfRun()) {
    printf("To do:\n");
    printf("    MkDataset         : Make memes dataset (extract quotes and save txt)\n");
    printf("    MkSentences       : Extract sentences with sequence of words\n");
    printf("    ExtractSubset     : Extract a subset of memes containing particular words\n");
    printf("    MemesToQtBs       : Load memes dataset and create quote base\n");
    printf("    MkClustNet        : Build cluster network from the quote base\n");
    printf("    LinkCascades      : Extract link cascades (for Andreas)\n");
    return;
  }
  // extract quotes and links and make them into a single file (Quotes dataset)
  if (ToDo == "mkdataset") {
    const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "files.txt", "Spinn3r input files (one file per line)");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "memes4_dataset.txt", "Output file");
    const int MinQtWrdLen = Env.GetIfArgPrefixInt("-w:", 3, "Minimum quote word length");
    const TStr UrlFNm = Env.GetIfArgPrefixStr("-u:", "", "Seen url set (THashSet<TMd5Sig>) file name");
    const bool UrlOnlyOnce = Env.GetIfArgPrefixBool("-q:", true, "Only keep unique Urls");
    //// parse directly from Spinn3r
    TStr Spinn3rFNm;
    THashSet<TMd5Sig> SeenUrlSet;
    if (UrlOnlyOnce && ! UrlFNm.Empty()) {  // keep track of already seen urls (so that there are no duplicate urls)
      TFIn FIn(UrlFNm);  SeenUrlSet.Load(FIn);
    }
    /*FILE *F = fopen(OutFNm.CStr(), "wt");
    TFIn FIn(InFNm);
    int Items=0;
    for (int f=0; FIn.GetNextLn(Spinn3rFNm); f++) {
      TQuoteExtractor QE(Spinn3rFNm.ToTrunc());
      printf("Processing %02d: %s [%s]\n", f+1, Spinn3rFNm.CStr(), TExeTm::GetCurTm());
      fflush(stdout);
      for (int item = 0; QE.Next(); item++) {
        const TMd5Sig PostMd5(QE.PostUrlStr);
        //if (QE.QuoteV.Empty() && QE.LinkV.Empty()) { continue; }   // no quotes, no links
        if (! TStrUtil::IsLatinStr(QE.PostText, 0.8)) { continue; }  // post not in latin alphabet
        if (UrlOnlyOnce) {
          if (SeenUrlSet.IsKey(PostMd5)) { continue; }
          SeenUrlSet.AddKey(PostMd5);
        }
        fprintf(F, "P\t%s\n", QE.PostUrlStr.CStr());
        //if (QE.PubTm > TSecTm(2008,8,30) || QE.PubTm < TSecTm(2008,7,25)) { printf("%s\n", QE.PubTm.GetStr().CStr()); }
        fprintf(F, "T\t%s\n", QE.PubTm.GetYmdTmStr().CStr());
        fprintf(F, "C\t%s\n", QE.PostText.CStr());
        for (int q = 0; q < QE.QuoteV.Len(); q++) {
          if (TStrUtil::CountWords(QE.QuoteV[q]) >= MinQtWrdLen) {
            fprintf(F, "Q\t%s\n", QE.QuoteV[q].CStr()); }
        }
        for (int l = 0; l < QE.LinkV.Len(); l++) {
          fprintf(F, "L\t%s\n", QE.LinkV[l].CStr()); }
        fprintf(F, "\n");
        if (item>0 && item % Kilo(100) == 0) {
          QE.DumpStat();  QE.ExeTm.Tick(); }
        Items++;
      }
      printf("file done. Total %d all posts, %d all items\n", SeenUrlSet.Len(), Items);
      fflush(stdout);
    }
    printf("all done. Saving %d post urls\n", SeenUrlSet.Len());  fflush(stdout);
    if (! SeenUrlSet.Empty()) {
      TFOut FOut(OutFNm.GetFMid()+".SeenUrlSet");
      SeenUrlSet.Save(FOut);
    }
    fclose(F);*/
  }
  // extract quotes and links and make them into a single file (Quotes dataset)
  if (ToDo == "mksentences") {
    const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "files.txt", "Spinn3r input files (one file per line)");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "phrases", "Output file");
    const bool UrlOnlyOnce = Env.GetIfArgPrefixBool("-q:", true, "Only keep unique Urls");
    // Sentences
    TVec<TChAV> PhraseV;
    // run 1
    //PhraseV.Add(TChAV::GetV("prehab"));
    //PhraseV.Add(TChAV::GetV("fundamentals", "of", "our"));
    //PhraseV.Add(TChAV::GetV("wise", "latina"));
    //PhraseV.Add(TChAV::GetV("pal", "around"));
    // run 2
    /*PhraseV.Add(TChAV::GetV("shovel", "ready"));
    PhraseV.Add(TChAV::GetV("climate", "change"));
    PhraseV.Add(TChAV::GetV("global", "warming"));
    PhraseV.Add(TChAV::GetV("financial", "tsunami"));
    PhraseV.Add(TChAV::GetV("too", "large", "to", "fail"));
    PhraseV.Add(TChAV::GetV("yes", "we", "can"));
    PhraseV.Add(TChAV::GetV("maverick"));
    PhraseV.Add(TChAV::GetV("fist", "bump"));*/
    // run 3
    PhraseV.Add(TChAV::GetV("too", "", "to", "fail"));
    PhraseV.Add(TChAV::GetV("lipstick", "on"));
    //// parse directly from Spinn3r
    TStr Spinn3rFNm;
    THashSet<TMd5Sig> SeenUrlSet;
    TVec<FILE *> FileV;
    for (int PhraseId = 0; PhraseId < PhraseV.Len(); PhraseId++) {
      FILE *F = fopen(TStr::Fmt("%s-p%d.txt", OutFNm.CStr(), PhraseId).CStr(), "wt");
      fprintf(F, "#");
      for (int p = 0; p < PhraseV[PhraseId].Len(); p++) {
        fprintf(F, " %s", PhraseV[PhraseId][p].CStr()); }
      fprintf(F, "\n"); fflush(F);
      FileV.Add(F);
    }
    TFIn FIn(InFNm);
    int Items=0;
    TVec<char *> SentV;
    for (int f=0; FIn.GetNextLn(Spinn3rFNm); f++) {
      /*TQuoteExtractor QE(Spinn3rFNm.ToTrunc());
      printf("Processing %02d: %s [%s]\n", f+1, Spinn3rFNm.CStr(), TExeTm::GetCurTm()); fflush(stdout);
      for (int item = 0; QE.Next(); item++) {
        const TMd5Sig PostMd5(QE.PostUrlStr);
        if (! TStrUtil::IsLatinStr(QE.PostText, 0.8)) { continue; }  // post not in latin alphabet
        if (UrlOnlyOnce) {
          if (SeenUrlSet.IsKey(PostMd5)) { continue; }
          SeenUrlSet.AddKey(PostMd5);
        }
        TStrUtil::SplitSentences(QE.PostText, SentV);
        const TChA TmStr = QE.PubTm.GetYmdTmStr();
        for (int s = 0; s < SentV.Len(); s++) {
          SaveIfSubPhrase(SentV[s], PhraseV, FileV, TmStr, QE.PostUrlStr); }
        if (item>0 && item % Kilo(100) == 0) {
          QE.DumpStat();  QE.ExeTm.Tick(); }
        Items++;
      }*/
      printf("file done. Total %d all posts, %d all items\n", SeenUrlSet.Len(), Items);
      fflush(stdout);
    }
    printf("all done. Saw %d urls\n", SeenUrlSet.Len());  fflush(stdout);
  }

  // save posts with memes containing particular words
  else if (ToDo == "extractsubset") {
    const TStr InFNmWc = Env.GetIfArgPrefixStr("-i:", "memes_*.rar", "Input file prefix");
    const bool IsInFNmWc = Env.GetIfArgPrefixBool("-w:", true, "Input is wildcard (else a file with list of input files)");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "memes-subset.txt", "Output memes file");
    const TStr WordsFNm = Env.GetIfArgPrefixStr("-p:", "phrases-in.txt", "Phrases that memes have to contain");

    TChAV CatchMemeV;// = TStr::GetV("great depression", "economic meltdown", "recession had bottomed out", "green shoots", "slow recovery", "gradual recovery");
    printf("Loading %s\n", WordsFNm.CStr());
    { TFIn FIn(WordsFNm);
    for (TStr Ln; FIn.GetNextLn(Ln); ) {
      printf("  %s\n", Ln.GetLc().CStr());
      CatchMemeV.Add(Ln.GetLc()); }
    }
    printf("%d strings loaded\n", CatchMemeV.Len());
    TFOut FOut(OutFNm);
    TMemesDataLoader Memes(InFNmWc, IsInFNmWc);
    for (int posts = 0, nsave=0; Memes.LoadNext(); posts++) {
      bool DoSave = false;
      for (int m = 0; m < Memes.MemeV.Len(); m++) {
        for (int i = 0; i < CatchMemeV.Len(); i++) {
          if (Memes.MemeV[m].SearchStr(CatchMemeV[i]) != -1) {
            DoSave=true; break; }
        }
        if (DoSave) { break; }
      }
      if (DoSave) { Memes.SaveTxt(FOut); nsave++; }
      if (posts % Mega(1) == 0) {
        printf("%dm posts, %d saved\n", posts/Mega(1), nsave);
        FOut.Flush();
      }
    }
  }
  // load memes dataset (MkDataset) and create quote base
  else if (ToDo == "memestoqtbs") {
    const TStr InFNmWc = Env.GetIfArgPrefixStr("-i:", "memes_200*.txt.rar", "Input Memes dataset files");
    const bool IsInFNmWc = Env.GetIfArgPrefixBool("-w:", true, "Input is wildcard (else a file with list of input files)");
    const TStr BlackListFNm = Env.GetIfArgPrefixStr("-b:", "quote_blacklist.txt", "Quote black list file");
    const TStr MediaUrlFNm = Env.GetIfArgPrefixStr("-u:", "news_media.txt", "Fule with news media urls");
    const TStr FNmPref = Env.GetIfArgPrefixStr("-o:", "qt08080902", "Output quote base");
    const int MinWrdLen = Env.GetIfArgPrefixInt("-l:", 4, "Min quote word length");
    const int MinMemeFq = Env.GetIfArgPrefixInt("-f:", 5, "Min meme frequency");
    THashSet<TChA> FqMemeSet, BlackList;
    TChA Ln;
    if (TFile::Exists(BlackListFNm)) {
      for (TFIn FIn(BlackListFNm); FIn.GetNextLn(Ln); ) {
        BlackList.AddKey(Ln); }
    }
    printf("Black list: %d\n", BlackList.Len());
    // count meme frequency, take each URL only once
    int NSkip=0;
    { TStrHash<TInt, TBigStrPool> MemeCntH(Mega(5));
    THashSet<TMd5Sig> SeenUrlSet;
    for (TMemesDataLoader Memes(InFNmWc, IsInFNmWc); Memes.LoadNext(); ) {
      TMd5Sig UrlSig(Memes.PostUrlStr);
      if (SeenUrlSet.IsKey(UrlSig)) { NSkip++; continue; }
      SeenUrlSet.AddKey(UrlSig);
      for (int m = 0; m < Memes.MemeV.Len(); m++) {
        MemeCntH.AddDat(Memes.MemeV[m].CStr()) += 1;
      }
    }
    printf("done [%s]\nAll quotes: %d\n", ExeTm.GetStr(), MemeCntH.Len());
    printf("  skip %d urls, keep %d\n", NSkip, SeenUrlSet.Len());
    // meme string to count hash table
    { TFOut FOut(FNmPref+"-MemeFq.strHash"); 
      MemeCntH.Save(FOut); }
    for (int i = 0; i < MemeCntH.Len(); i++) {
      if (MemeCntH[i] >= 5 && ! BlackList.IsKey(MemeCntH.GetKey(i))) { // take all the memes with frequency >= 5
        FqMemeSet.AddKey(MemeCntH.GetKey(i)); } // frequent memes
    }
    SeenUrlSet.Clr(true); }
    // load QuoteBs
    printf("Frequent quotes: %d\n", FqMemeSet.Len());
    PQuoteBs QtBs = TQuoteBs::New();
    { THashSet<TMd5Sig> SeenUrlSet2;
    NSkip = 0;
    for (TMemesDataLoader Memes(InFNmWc, IsInFNmWc); Memes.LoadNext(); ) {
      TMd5Sig UrlSig(Memes.PostUrlStr);
      if (SeenUrlSet2.IsKey(UrlSig)) { NSkip++; continue; }
      SeenUrlSet2.AddKey(UrlSig);
      for (int m = 0; m < Memes.MemeV.Len(); m++) { // delete non-frequent memes
        if (! FqMemeSet.IsKey(Memes.MemeV[m])) { // if meme is not frequent, forget it
          Memes.MemeV[m].Clr(); }
      }
      QtBs->AddQuote(Memes.MemeV, Memes.LinkV, Memes.PostUrlStr, Memes.PubTm, MinWrdLen);
    }
    printf("  skip %d urls, keep %d, add posts %d\n", NSkip, SeenUrlSet2.Len(), QtBs->Len()); }
    printf("SAVE 1: %d quotes\n", QtBs->Len());
    { TFOut FOut(TStr::Fmt("%s-w%dmfq%d.QtBs", FNmPref.CStr(), MinWrdLen, MinMemeFq)); QtBs->Save(FOut); }
    QtBs->SetUrlTy(MediaUrlFNm, utMedia);  // set url type
    printf("SAVE 2: %d quotes\n", QtBs->Len());
    { TFOut FOut(TStr::Fmt("%s-w%dmfq%d.QtBs", FNmPref.CStr(), MinWrdLen, MinMemeFq)); QtBs->Save(FOut); }
  }
  // make cluster network
  else if (ToDo == "mkclustnet") {
    const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "QtBase-qt0808-0902w3mfq5\\qt08080902w3mfq5-clust.QtBs.rar", "Input quote base");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "qt08080902w3mfq5", "Output network/updated QtBs filename");
    const int MinWrdLen = Env.GetIfArgPrefixInt("-l:", 4, "Min quote word length");
    const int MinMemeFq = Env.GetIfArgPrefixInt("-f:", 5, "Min meme frequency");
    const int ClustMeth = Env.GetIfArgPrefixInt("-c:", 1, "Clustering: keep edge to 1:most freq, 2: longest, 3:shortest, 4:random, 5:greedy top down");
    const TStr Pref = TStr::Fmt("%s-w%dfq%d", OutFNm.CStr(), MinWrdLen, MinMemeFq);
    PQuoteBs QtBs;
    if (TZipIn::IsZipFNm(InFNm)) { TZipIn ZipIn(InFNm);  QtBs = TQuoteBs::Load(ZipIn); }
    else { TFIn FIn(InFNm);  QtBs = TQuoteBs::Load(FIn); }
    PClustNet Net = TClustNet::GetFromQtBs(QtBs, MinMemeFq, MinWrdLen);
    TSnap::PrintInfo(Net);
    { printf("save clust net\n"); TFOut FOut(Pref+".ClustNet"); Net->Save(FOut); }
    // cluster
    TSnap::DelSelfEdges(Net);
    TIntPrV KeeepEdgeV;
    printf("cluster\n");
    if (ClustMeth == 5) { Net->ClustGreedyTopDown(KeeepEdgeV); }
    else { Net->ClustKeepSingleEdge(ClustMeth, KeeepEdgeV); }
    Net->MakeClusters(KeeepEdgeV);
    TVec<TIntV> ClustV;
    printf("get clusters\n");
    Net->GetClusters(ClustV);
    printf("set cluster\n");
    QtBs->CreateClusters(ClustV); // set clusters
    printf("save\n");
    { TFOut FOut(Pref+"-clust.QtBs"); QtBs->Save(FOut); }
    // save clusters
    TIntV QtIdV;
    QtBs->GetQtIdVByFq(QtIdV, MinWrdLen, 50, false); QtIdV.Del(100, QtIdV.Len()-1);
    QtBs->SaveQuotes(QtIdV, Pref+"-quotes.txt");
    QtBs->SaveClusters(Pref+"a", true);
    QtBs->SaveClusters(Pref+"b", false);
    const TUrlTy CntUrlTy = utUndef;
    TSecTm MinTm(2009,2,1), MaxTm(2009,7,31); //QtBs->GetMinMaxTm(MinTm, MaxTm);
    QtBs->GetQtIdVByFq(QtIdV, MinWrdLen, MinMemeFq, true, "", "", CntUrlTy);
    QtBs->SaveForFlash(QtIdV, Pref+"-top10", tmu4Hour, 10, MinTm, MaxTm, CntUrlTy);
    QtBs->SaveForFlash(QtIdV, Pref+"-top20", tmu4Hour, 20, MinTm, MaxTm, CntUrlTy);
    QtBs->SaveForFlash(QtIdV, Pref+"-top30", tmu4Hour, 30, MinTm, MaxTm, CntUrlTy);
    QtBs->SaveForFlash(QtIdV, Pref+"-top50", tmu4Hour, 50, MinTm, MaxTm, CntUrlTy);
    QtBs->SaveForFlash(QtIdV, Pref+"-top100", tmu4Hour, 100, MinTm, MaxTm, CntUrlTy);
  }
  // extract cascades (from MsDataset input file) and save to TXT format
  else if (ToDo == "linkcascades") {
    const TStr InFNmWc = Env.GetIfArgPrefixStr("-i:", "memes_*.rar", "Input file prefix");
    const bool IsInFNmWc = Env.GetIfArgPrefixBool("-w:", true, "Input is wildcard (else a file with list of input files)");
    const TStr StopList = Env.GetIfArgPrefixStr("-t:", "CELF-Cascades/websiteStopList.txt", "Domain name stoplist (delete domains from post net)");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "CELF-Cascades/Casc5-0808-0908", "Output file");
    const int MinCascSz = Env.GetIfArgPrefixInt("-c:", 5, "Minimum cascade size");
    const bool SavePostNet = Env.GetIfArgPrefixBool("-s:", true, "Save post network");
    const uint StartTm = TSecTm(2008, 1, 1, 0, 0, 0);
    //// build PostNetwork
    int NPosts=0, NSkipPost=0, NSkipLink=0, NLinks=0;
    TStrHash<TInt> BlogSet;     // blog url to blog id (key id)
    THash<TMd5Sig, TPair<TSecTm, TInt> > UrlTmH; // post url -> blog id and time
    PNGraph PostNet;
    //PostNet = TNGraph::New(Mega(1), Mega(100));
    // Load data directly
    { TZipIn FIn(OutFNm+".PostNet.rar"); PostNet = TNGraph::Load(FIn); BlogSet.Load(FIn); UrlTmH.Load(FIn); }
    /*for (TMemesDataLoader Memes(InFNmWc, IsInFNmWc); Memes.LoadNext(); NPosts++) {
      if (NPosts % Kilo(100) == 0) { printf("."); }
      if (NPosts % Mega(1) == 0) {
        printf("%dm[l:%.0f]", NPosts/Mega(1), float(Memes.LineCnt)); fflush(stdout); }
      const TMd5Sig SrcMd5(Memes.PostUrlStr);
      if (UrlTmH.IsKey(SrcMd5)) {
        TSecTm& Tm = UrlTmH.GetDat(SrcMd5).Val1;
        if (! Tm.IsDef()) { Tm = Memes.PubTm; }
        else { NSkipPost++; continue; }
      } else {
        const TChA BlogNm = TStrUtil::GetWebsiteNm(Memes.PostUrlStr);
        const int BlogId = BlogSet.AddKey(BlogNm);
        UrlTmH.AddDat(SrcMd5, TPair<TSecTm, TInt>(Memes.PubTm, BlogId));
      }
      const int SrcNId = UrlTmH.GetKeyId(SrcMd5);
      const int SrcBlog = UrlTmH.GetDat(SrcMd5).Val2;
      NLinks += Memes.LinkV.Len();
      for (int l = 0; l < Memes.LinkV.Len(); l++) {
        const TMd5Sig DstMd5(Memes.LinkV[l]);
        if (! UrlTmH.IsKey(DstMd5)) {
          const TChA BlogNm = TStrUtil::GetWebsiteNm(Memes.LinkV[l]);
          const int BlogId = BlogSet.AddKey(BlogNm);
          if (BlogId == SrcBlog) { NSkipLink++; continue; } // skip links inside same website
          UrlTmH.AddDat(DstMd5, TPair<TSecTm, TInt>(TSecTm(), BlogId));
          //if (BlogNm!=TStrUtil::GetDomNm2(Memes.LinkV[l])) printf("%s\n       %s\n", Memes.LinkV[l].CStr(), BlogNm.CStr());
        }
        const int DstNId = UrlTmH.GetKeyId(DstMd5);
        const int DstBlog = UrlTmH.GetDat(DstMd5).Val2;
        if (SrcBlog == DstBlog) { NSkipLink++; continue; } // skip links inside same website
        PostNet->AddNode(SrcNId);
        PostNet->AddNode(DstNId);
        PostNet->AddEdge(SrcNId, DstNId);
        IAssert(UrlTmH[SrcNId].Val2 != UrlTmH[DstNId].Val2);
      }
    }
    if (SavePostNet) {
      TFOut FOut(OutFNm+".PostNet");
      PostNet->Save(FOut);
      BlogSet.Save(FOut);
      UrlTmH.Save(FOut);
    }*/
    //// save websites
    { FILE *F = fopen(TStr::Fmt("%s-Websites.txt", OutFNm.CStr()).CStr(), "wt");
    TIntH PostCntH;
    for (int i = 0; i < UrlTmH.Len(); i++) {
      PostCntH.AddDat(UrlTmH[i].Val2) += 1; }
    fprintf(F, "#Id\tTotalPosts\tName\n");
    for (int w = 0; w < BlogSet.Len(); w++) {
      fprintf(F, "%d\t%d\t%s\n", w, PostCntH.GetDat(w)(), BlogSet.GetKey(w)); }
    } //*/
    printf("\nTotal websites: %d\n", BlogSet.Len());
    printf("Total posts:    %d\n", NPosts);
    printf("Skip posts:     %d\n", NSkipPost);
    printf("Total urls:     %d\n", UrlTmH.Len());
    printf("Total links:    %d\n", NLinks);
    printf("Skip links:     %d\n\n", NSkipLink);
    TSnap::PrintInfo(PostNet);
    //// delete posts from the stop-list blogs
    if (TFile::Exists(StopList)) {
      printf("build website stoplist\n");
      TIntSet StopSet;  TStr Ln;
      int lines=0;
      for (TFIn FIn(StopList); FIn.GetNextLn(Ln); lines++) {
        for (int w = 0; w < BlogSet.Len(); w++) {
          if (strstr(BlogSet.GetKey(w), Ln.CStr()) != 0) {
            StopSet.AddKey(w); }
        }
      }
      printf("Stoplist\n  %d lines --> %d websites\n", lines, StopSet.Len());
      TIntV DelNIdV;
      for (TNGraph::TNodeI NI = PostNet->BegNI(); NI < PostNet->EndNI(); NI++) {
        if (StopSet.IsKey(UrlTmH[NI.GetId()].Val2)) { DelNIdV.Add(NI.GetId()); }
      }
      printf("  deleting %d posts\n", DelNIdV.Len());
      TSnap::DelNodes(PostNet, DelNIdV);
    }
    //// blog-blog-citaton network
    { printf("Build blog network\n");
    TPt<TNodeEDatNet<TInt, TInt> > BlogNet = TNodeEDatNet<TInt, TInt>::New();
    for (TNGraph::TNodeI NI = PostNet->BegNI(); NI < PostNet->EndNI(); NI++) {
      IAssert(NI.GetId() < UrlTmH.Len());
      const int src = UrlTmH[NI.GetId()].Val2;
      if (NI.GetOutDeg() > 0) { BlogNet->AddNode(src); }
      for (int i = 0; i < NI.GetOutDeg(); i++) {
        IAssert(NI.GetOutNId(i) < UrlTmH.Len());
        const int dst = UrlTmH[NI.GetOutNId(i)].Val2;
        BlogNet->AddNode(dst);
        if (! BlogNet->IsEdge(src, dst)) {
          BlogNet->AddEdge(src, dst, 1); }
        else {
          BlogNet->GetEDat(src, dst) += 1; }
      }
    }
    TSnap::PrintInfo(BlogNet);
    FILE *F = fopen(TStr::Fmt("%s-BlogNet2.txt", OutFNm.CStr()).CStr(), "wt");
    fprintf(F, "# Directed blog citation network (edge (U,V) if some post at blog U links to some post at blog V)\n", OutFNm.CStr());
    fprintf(F, "# Nodes: %d Edges: %d\n", BlogNet->GetNodes(), BlogNet->GetEdges());
    fprintf(F, "# From\tTo\tLinks\n");
    for (TNodeEDatNet<TInt, TInt>::TEdgeI ei = BlogNet->BegEI(); ei < BlogNet->EndEI(); ei++) {
      fprintf(F, "%d\t%d\t%d\n", ei.GetSrcNId(), ei.GetDstNId(), ei().Val);
    }
    fclose(F);
    { TFOut FOut(OutFNm+".BlogNet2"); BlogNet->Save(FOut); } }

    /*//// create cascades
    printf("Create cascades\n");
    FILE *F = fopen(TStr::Fmt("%s-Cascades3.txt", OutFNm.CStr()).CStr(), "wt");
    int Cascade = 0;
    /* // old version -- saves CascadeId NodeId Time
    TIntH SzCntH;
    TBreathFS<PNGraph> BFS(PostNet);
    TIntPrV BIdTmV;
    for (TNGraph::TNodeI NI = PostNet->BegNI(); NI < PostNet->EndNI(); NI++) {
      if (! (NI.GetOutDeg()==0 && NI.GetInDeg()!=0)) { continue; }
      if (! UrlTmH[NI.GetId()].Val1.IsDef()) { continue; }
      TExeTm Tm;
      BFS.DoBfs(NI.GetId(), false, true);
      SzCntH.AddDat(BFS.GetNVisited()) += 1;
      if (BFS.GetNVisited() < MinCascSz) { continue; }
      printf("[%g]", Tm.GetSecs());
      BIdTmV.Clr(false);
      for (int i = BFS.NIdDistH.FFirstKeyId(); BFS.NIdDistH.FNextKeyId(i); ) {
        const int NId = BFS.NIdDistH.GetKey(i);
        if (! UrlTmH[NId].Val1.IsDef()) { printf("X"); continue; }
        const int Tm = int(UrlTmH[NId].Val1.GetAbsSecs());
        const int BId = UrlTmH[NId].Val2;
        BIdTmV.Add(TIntPr(Tm, BId));
      }
      BIdTmV.Sort();  Cascade++;
      printf("[%g]", Tm.GetSecs());
      for (int b = 0; b < BIdTmV.Len(); b++) {
        fprintf(F, "%d\t%d\t%d\n", Cascade, BIdTmV[b].Val2(), BIdTmV[b].Val1());
      }
      printf("[%g]c:%d,s:%d  ", Tm.GetSecs(), Cascade, BIdTmV.Len());
    }// old version*/
    /*fprintf(F, "# CascadeId\tSrcNId\tDstNId\tTimeDelta\n"); fflush(F);
    PNGraph Tree;
    TIntTrV TmSrcDstV;
    TExeTm Tm;
    TPt<TNodeNet<TSecTm> > CascNet;
    TFOut FOut(OutFNm+"-Cascade3.bin");
    for (TNGraph::TNodeI NI = PostNet->BegNI(); NI < PostNet->EndNI(); NI++) {
      if (! (NI.GetOutDeg()==0 && NI.GetInDeg()!=0)) { continue; }
      if (! UrlTmH[NI.GetId()].Val1.IsDef()) { continue; }
      Tree = TSnap::GetBfsTree(PostNet, NI.GetId(), false, true);
      if (Tree->GetNodes() < MinCascSz) { continue; }
      CascNet = TNodeNet<TSecTm>::New();
      TmSrcDstV.Clr(false);
      for (TNGraph::TEdgeI EI = Tree->BegEI(); EI < Tree->EndEI(); EI++) {
        const int SrcNId = EI.GetSrcNId();
        const int DstNId = EI.GetDstNId();
        if (! UrlTmH[SrcNId].Val1.IsDef()) { printf("XXX"); }
        if (! UrlTmH[DstNId].Val1.IsDef()) { printf("YYY"); }
        const TSecTm SrcTm = UrlTmH[SrcNId].Val1;
        const TSecTm DstTm = UrlTmH[DstNId].Val1;
        const int SrcBlog = UrlTmH[SrcNId].Val2;
        const int DstBlog = UrlTmH[DstNId].Val2;
        const int TmDiff = DstTm-SrcTm;
        TmSrcDstV.Add(TIntTr(TmDiff < 0 ? 0:TmDiff , SrcBlog, DstBlog));
        if (TmDiff  < 0) { printf("x"); }
        CascNet->AddNode(SrcBlog, SrcTm);
        CascNet->AddNode(DstBlog, DstTm);
        CascNet->AddEdge(SrcBlog, DstBlog);
      }
      printf("s");
      CascNet->Save(FOut);
      TmSrcDstV.Sort();
      for (int i = 0; i < TmSrcDstV.Len(); i++) {
        fprintf(F, "%d\t%d\t%d\t%d\n", Cascade, TmSrcDstV[i].Val2(), TmSrcDstV[i].Val3(), TmSrcDstV[i].Val1());
      }
      Cascade++;
      printf("[%d,%.1g]", Tree->GetNodes(), Tm.GetSecs());
      fflush(F); FOut.Flush();
    }
    fclose(F); // */
  }

}

void QuotesForDepression() {
  const TStr InFNm = "depression/FebJun09-w5mfq10.QtBs.rar";
  const int MinWrdLen = 7;
  const int MinMemeFq = 10;
  const int ClustMeth = 1;
  const TStr Pref = TStr::Fmt("Dep23e-w%dfq%d", MinWrdLen, MinMemeFq);
  PQuoteBs QtBs;
  if (TZipIn::IsZipFNm(InFNm)) { TZipIn ZipIn(InFNm);  QtBs = TQuoteBs::Load(ZipIn); }
  else { TFIn FIn(InFNm);  QtBs = TQuoteBs::Load(FIn); }
  PClustNet Net;
  { TFIn FIn("depression/FebJun09-w7fq10.ClustNet"); Net = TClustNet::Load(FIn); } //TClustNet::GetFromQtBs(QtBs, MinMemeFq, MinWrdLen);
  printf("\n***Build done\n");
  TSnap::PrintInfo(Net);
  //{ TFOut FOut(Pref+".ClustNet"); Net->Save(FOut); }
  // cluster
  printf("\n***CLUST\n");
  Net = Net->GetSubGraph(7, 100, 10);
  TSnap::DelSelfEdges(Net);
  TIntPrV KeeepEdgeV;
  if (ClustMeth == 5) { Net->ClustGreedyTopDown(KeeepEdgeV); }
  else { Net->ClustKeepSingleEdge(ClustMeth, KeeepEdgeV); }

  // delete
  IAssert(Net->IsNode(2547905)); Net->DelNode(2547905);
  IAssert(Net->IsNode(2547910)); Net->DelNode(2547910);
  IAssert(Net->IsNode(2564208)); Net->DelNode(2564208);
  IAssert(Net->IsNode(55769)); Net->DelNode(55769);
  QtBs->DelQtId(2547905);
  QtBs->DelQtId(2547910);
  QtBs->DelQtId(2564208);
  QtBs->DelQtId(55769);
  //*/


  Net->MakeClusters(KeeepEdgeV);
  TVec<TIntV> ClustV;
  Net->GetClusters(ClustV);
  QtBs->CreateClusters(ClustV); // set clusters
  //{ TFOut FOut(Pref+"-clust.QtBs"); QtBs->Save(FOut); }
  // save clusters
  TIntV QtIdV;
  QtBs->GetQtIdVByFq(QtIdV, MinWrdLen, 50, false); QtIdV.Del(100, QtIdV.Len()-1);
  QtBs->SaveQuotes(QtIdV, Pref+"-quotes.txt");
  //QtBs->SaveClusters(Pref+"a", true);
  //QtBs->SaveClusters(Pref+"b", false);
  const TUrlTy CntUrlTy = utUndef;
  TSecTm MinTm(2009,2,1), MaxTm(2009,7,31); //QtBs->GetMinMaxTm(MinTm, MaxTm);
  //QtBs->GetQtIdVByFq(QtIdV, MinWrdLen, MinMemeFq, true, "", "", CntUrlTy);
  TStrV StrQtIdV; TStr("789996,1413261,236762,599878,788250,1342267,1048831,1110510,1394318,1283009,1426713,1394397,12677,788690,957195,795390,788689,877586,404516,2111129,1406451,1734005").SplitOnAllCh(',', StrQtIdV);
  QtIdV.Clr(); for (int i = 0; i < StrQtIdV.Len(); i++)  { QtIdV.Add(StrQtIdV[i].GetInt()); }
  QtBs->SaveClusters(QtIdV, Pref+"c", false);
  QtBs->SaveForFlash(QtIdV, Pref+"-top10", tmu4Hour, 10, MinTm, MaxTm, CntUrlTy);
  QtBs->SaveForFlash(QtIdV, Pref+"-top20", tmu4Hour, 20, MinTm, MaxTm, CntUrlTy);
  QtBs->SaveForFlash(QtIdV, Pref+"-top30", tmu4Hour, 30, MinTm, MaxTm, CntUrlTy);
  //QtBs->SaveForFlash(QtIdV, Pref+"-top50", tmu4Hour, 50, MinTm, MaxTm, CntUrlTy);
  //QtBs->SaveForFlash(QtIdV, Pref+"-top100", tmu4Hour, 100, MinTm, MaxTm, CntUrlTy);
}

void DelSpam(PQuoteBs& QtBs, PClustNet& Net, TSecTm BeforeTm) {
  { TIntV DelV;
  for (TClustNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    if (NI().GetMinTm() < BeforeTm) { DelV.Add(NI.GetId()); }
  }
  printf("Deleting %d quotes that have first mention before %s\n", DelV.Len(), BeforeTm.GetStr().CStr());
  for (int d = 0; d < DelV.Len(); d++) {
    if (Net->IsNode(DelV[d])) { Net->DelNode(DelV[d]); }
    else { printf("N"); }
    if (QtBs->IsQtId(DelV[d])) { QtBs->DelQtId(DelV[d]); }
    else { printf("Q"); }
  }
  printf("\n");
  printf("Deleted %d quotes that have first mention before %s\n", DelV.Len(), BeforeTm.GetStr().CStr()); }
  // delete edges between quotes  that peak more than a week apart
  TIntPrV DelEdgeV;
  for (TClustNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
    //const double Peak1 = NI().GetPeakTm(tmu4Hour).GetAbsSecs();
    const double Peak1 = NI().GetMedianTm(tmu4Hour, utUndef, *QtBs).GetAbsSecs();
    for (int i = 0; i < NI.GetOutDeg(); NI++) {
      //const double Peak2 = NI.GetOutNDat(i)..GetPeakTm(tmu4Hour).GetAbsSecs();
      const double Peak2 = NI.GetOutNDat(i).GetMedianTm(tmu4Hour, utUndef, *QtBs).GetAbsSecs();
      if (fabs(Peak1-Peak2) > 4*24*3600) { DelEdgeV.Add(TIntPr(NI.GetId(), NI.GetOutNId(i))); }
    }
  }
  printf("Deleting %d edges where phrase peaks are too far apart\n", DelEdgeV.Len());
  for (int e = 0; e < DelEdgeV.Len(); e++) {
    Net->DelEdge(DelEdgeV[e].Val1, DelEdgeV[e].Val2);
  }
  printf("Done!\n");
}
