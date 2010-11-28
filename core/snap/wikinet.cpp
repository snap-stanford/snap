#include "stdafx.h"
#include "wikinet.h"
#include "trawling.h"

/////////////////////////////////////////////////
// Wikipedia Talk Network
TWikiUsr::TWikiUsr() : Usr(), Admin(false), ElecTm(), BarnStars(), MnEdCnt(), MnEdWrds(), MnTkEdCnt(), MnTkEdWrds(),
  WkEdCnt(), WkEdWrds(), WkTkEdCnt(), WkTkEdWrds(), MnRevCnt(), MnRevWrds() {
}

TWikiUsr::TWikiUsr(const TChA& UsrStr) : Usr(UsrStr), Admin(false), ElecTm(), BarnStars(), MnEdCnt(), MnEdWrds(),
  MnTkEdCnt(), MnTkEdWrds(), WkEdCnt(), WkEdWrds(), WkTkEdCnt(), WkTkEdWrds(), MnRevCnt(), MnRevWrds() {
}

TWikiUsr::TWikiUsr(const TWikiUsr& WikiUsr) : Usr(WikiUsr.Usr), Admin(WikiUsr.Admin), ElecTm(WikiUsr.ElecTm),
  BarnStars(WikiUsr.BarnStars), MnEdCnt(WikiUsr.MnEdCnt), MnEdWrds(WikiUsr.MnEdWrds),
  MnTkEdCnt(WikiUsr.MnTkEdCnt), MnTkEdWrds(WikiUsr.MnTkEdWrds), WkEdCnt(WikiUsr.WkEdCnt),
  WkEdWrds(WikiUsr.WkEdWrds), WkTkEdCnt(WikiUsr.WkTkEdCnt), WkTkEdWrds(WikiUsr.WkTkEdWrds),
  MnRevCnt(WikiUsr.MnRevCnt), MnRevWrds(WikiUsr.MnRevCnt) {
}

TWikiUsr::TWikiUsr(TSIn& SIn) : Usr(SIn), Admin(SIn), ElecTm(SIn), BarnStars(SIn), MnEdCnt(SIn),
  MnEdWrds(SIn), MnTkEdCnt(SIn), MnTkEdWrds(SIn), WkEdCnt(SIn), WkEdWrds(SIn), WkTkEdCnt(SIn), WkTkEdWrds(SIn)
  , MnRevCnt(SIn), MnRevWrds(SIn)
{
}

void TWikiUsr::Save(TSOut& SOut) const {
  Usr.Save(SOut);  Admin.Save(SOut);  ElecTm.Save(SOut);
  BarnStars.Save(SOut);  MnEdCnt.Save(SOut);  MnEdWrds.Save(SOut);
  MnTkEdCnt.Save(SOut);  MnTkEdWrds.Save(SOut);  WkEdCnt.Save(SOut);  WkEdWrds.Save(SOut);
  WkTkEdCnt.Save(SOut);  WkTkEdWrds.Save(SOut);
  MnRevCnt.Save(SOut);   MnRevWrds.Save(SOut);
}

TWikiTalkEdge::TWikiTalkEdge() : TotTalks(), TotWords(), TalksBE(), WordsBE(), TalksAE(), WordsAE(), VoteSign(0), FirstTalk(), LastTalk(), VoteTm() {
}

TWikiTalkEdge::TWikiTalkEdge(const int& _VoteSign) : TotTalks(), TotWords(), TalksBE(), WordsBE(), TalksAE(), WordsAE(),
  VoteSign(_VoteSign), FirstTalk(), LastTalk(), VoteTm() {
}

TWikiTalkEdge::TWikiTalkEdge(const TSecTm& FTalk, const TSecTm& LTalk, const int& NTalks, const int& NWords) :
  TotTalks(NTalks), TotWords(NWords), TalksBE(), WordsBE(), TalksAE(), WordsAE(), VoteSign(0), FirstTalk(FTalk), LastTalk(LTalk), VoteTm() {
}

TWikiTalkEdge::TWikiTalkEdge(const TWikiTalkEdge& Talk) : TotTalks(Talk.TotTalks), TotWords(Talk.TotWords),
  TalksBE(Talk.TalksBE), WordsBE(Talk.WordsBE), TalksAE(Talk.TalksAE), WordsAE(Talk.WordsAE),
  VoteSign(Talk.VoteSign), FirstTalk(Talk.FirstTalk), LastTalk(Talk.LastTalk), VoteTm(Talk.VoteTm) {
}

TWikiTalkEdge::TWikiTalkEdge(TSIn& SIn) : TotTalks(SIn), TotWords(SIn), TalksBE(SIn), WordsBE(SIn),
  TalksAE(SIn), WordsAE(SIn), VoteSign(SIn), FirstTalk(SIn), LastTalk(SIn), VoteTm(SIn) {
}

void TWikiTalkEdge::Save(TSOut& SOut) const {
  TotTalks.Save(SOut);  TotWords.Save(SOut);
  TalksBE.Save(SOut);   WordsBE.Save(SOut);
  TalksAE.Save(SOut);   WordsAE.Save(SOut);
  VoteSign.Save(SOut);  FirstTalk.Save(SOut);
  LastTalk.Save(SOut);  VoteTm.Save(SOut);
}

int TWikiTalkNet::GetUsrNId(const TStr& UsrStr) const {
  const int KeyId = UsrNIdH.GetKeyId(UsrStr);
  if (KeyId==-1) { return -1; }
  else { return UsrNIdH[KeyId]; }
}

bool TWikiTalkNet::IsUsr(const TStr& UsrStr) const {
  return UsrNIdH.IsKey(UsrStr);
}

void TWikiTalkNet::PermuteAllVoteSigns(const bool& OnlyVotes) {
  printf("\nPermuting %s edge signs\n", OnlyVotes?"VOTE":"ALL");
  TVec<TWikiTalkEdge> EDatV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (OnlyVotes && EI().GetVote() != 0) { EDatV.Add(EI()); }
    else if (! OnlyVotes){ EDatV.Add(EI()); }
  }
  EDatV.Shuffle(TInt::Rnd);
  int cnt = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (OnlyVotes && EI().GetVote() != 0) { EI() = EDatV[cnt++]; }
    else if (! OnlyVotes){ EI() = EDatV[cnt++]; }
  }
}

void TWikiTalkNet::PermuteOutVoteSigns(const bool& OnlyVotes) {
  TIntV VoteSignV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (OnlyVotes && NI.GetOutEDat(e).GetVote() != 0) {
        VoteSignV.Add(NI.GetOutEDat(e).GetVote()); }
      else if (! OnlyVotes) {
        VoteSignV.Add(NI.GetOutEDat(e).GetVote()); }
    }
  }
  VoteSignV.Shuffle(TInt::Rnd);
  int cnt = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (OnlyVotes && NI.GetOutEDat(e).GetVote() != 0) {
        NI.GetOutEDat(e).VoteSign = VoteSignV[cnt++]; }
      else if (! OnlyVotes) {
        NI.GetOutEDat(e).VoteSign = VoteSignV[cnt++]; }
    }
  }
}

void TWikiTalkNet::CountStructBalance() const {
  TIntSet NbhIdSet;
  THash<TIntTr, TInt> TriadCntH;
  TIntH SignH;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    SignH.AddDat(EI().VoteSign()) += 1;
  }
  printf("Structural balance triads:\n  background sign distribution:\n");
  SignH.SortByKey(false);
  for (int i = 0; i < SignH.Len(); i++) {
    printf("\t%2d\t%d\n", SignH.GetKey(i), SignH[i]);
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const TNodeI SrcNI = GetNI(EI.GetSrcNId());
    const TNodeI DstNI = GetNI(EI.GetDstNId());
    const TInt E1Dat = EI().VoteSign();
    NbhIdSet.Clr(false);
    for (int es = 0; es < SrcNI.GetDeg(); es++) {
      NbhIdSet.AddKey(SrcNI.GetNbhNId(es)); }
    for (int ed = 0; ed < DstNI.GetDeg(); ed++) {
      const int nbh = DstNI.GetNbhNId(ed);
      if (! NbhIdSet.IsKey(nbh)) { continue; }
      const TInt E2Dat = SrcNI.GetNbhEDat(NbhIdSet.GetKeyId(nbh)).VoteSign();
      const TInt E3Dat = DstNI.GetNbhEDat(ed).VoteSign();
      TriadCntH.AddDat(TIntTr(TMath::Mx(E1Dat, E2Dat, E3Dat),
        TMath::Median(E1Dat, E2Dat, E3Dat), TMath::Mn(E1Dat, E2Dat, E3Dat))) += 1;
    }
  }
  TriadCntH.SortByKey(false);
  printf("triad counts (all counts times 3):\n");
  int SumTriad = 0, SignTriad=0;
  for (int i = 0; i < TriadCntH.Len(); i++) {
    SumTriad += TriadCntH[i];
    TIntTr SignTr = TriadCntH.GetKey(i);
    if (SignTr.Val1!=0 && SignTr.Val2!=0 && SignTr.Val3!=0) {
      SignTriad += TriadCntH[i];; }
  }
  for (int i = 0; i < TriadCntH.Len(); i++) {
    TIntTr SignTr = TriadCntH.GetKey(i);
    printf("\t%2d %2d %2d\t%8d\t%f", SignTr.Val1, SignTr.Val2, SignTr.Val3,
      TriadCntH[i], TriadCntH[i]/double(SumTriad));
    if (SignTr.Val1!=0 && SignTr.Val2!=0 && SignTr.Val3!=0) {
      printf("\t%f", TriadCntH[i]/double(SignTriad)); }
    printf("\n");
  }
}

void TWikiTalkNet::FindPartitions(const int& NPart, const bool& OnlyMinus) const {
  printf("\nFind %d partitions use %s edges\n", NPart, OnlyMinus?"NEGATIVE":"ALL");
  TIntV NIdV(GetNodes(), 0);
  TIntH NIdPartH;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NIdPartH.AddDat(NI.GetId(), TInt::Rnd.GetUniDevInt(NPart));
    NIdV.Add(NI.GetId());
  }
  int Flips = 0;
  TIntPrV CntPartV;
  for (int p = 0; p < NPart; p++) {
    CntPartV.Add(TIntPr(0, p)); }
  for (int iter = 0; iter < 100; iter++) {
    NIdV.Shuffle(TInt::Rnd);
    Flips = 0;
    for (int n = 0; n < NIdV.Len(); n++) {
      TNodeI NI = GetNI(NIdV[n]);
      for (int p = 0; p < NPart; p++) {
        CntPartV[p].Val1=0;  CntPartV[p].Val2=p; }
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int DstPart = NIdPartH.GetDat(NI.GetOutNId(e));
        const int Vote = NI.GetOutEDat(e).GetVote();
        if (OnlyMinus && Vote==-1) { CntPartV[DstPart].Val1 += -1; }
        else if (! OnlyMinus) { CntPartV[DstPart].Val1 += Vote; }
      }
      CntPartV.Sort(false); // take group with highest score (little - edges, lots + edges)
      const int NewPart = CntPartV[0].Val2;
      if (NIdPartH.GetDat(NI.GetId()) != NewPart) {
        NIdPartH.AddDat(NI.GetId(), NewPart);
        Flips++;
      }
    }
    //printf(" %d", Flips);
  }
  // calculate overall energy
  int OkMns=0, OkPls=0, AllMns=0, AllPls=0;
  TIntH OkPlsH, AllPlsH, OkMnsH, AllMnsH;
  for (int p = 0; p < NPart; p++) {
    OkPlsH.AddDat(p,0); AllPlsH.AddDat(p,0);
    OkMnsH.AddDat(p,0); AllMnsH.AddDat(p,0);
  }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int SrcPart = NIdPartH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int DstPart = NIdPartH.GetDat(NI.GetOutNId(e));
      const int Vote = NI.GetOutEDat(e).GetVote();
      if (Vote == -1) {
        if (DstPart != SrcPart) { OkMnsH.AddDat(SrcPart) += 1;  OkMns++; }
        AllMnsH.AddDat(SrcPart) += 1;  AllMns++; }
      if (Vote == +1) {
        if (DstPart == SrcPart) { OkPlsH.AddDat(SrcPart) += 1;  OkPls++; }
        AllPlsH.AddDat(SrcPart) += 1;  AllPls++;
      }
    }
  }
  printf("\nSatisfied edges: + : %5d / %5d  = %f\n", OkPls, AllPls, double(OkPls)/double(AllPls));
  printf(  "                 - : %5d / %5d  = %f\n", OkMns, AllMns, double(OkMns)/double(AllMns));
  TIntH PartCntH;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    PartCntH.AddDat(NIdPartH.GetDat(NI.GetId())) += 1; }
  PartCntH.SortByKey();
  for (int p = 0; p < PartCntH.Len(); p++) {
    printf("  part %2d : %5d (%.4f) nodes: +: %25s    -: %25s\n", p, PartCntH[p], PartCntH[p]/double(NIdPartH.Len()),
      TStr::Fmt("%5d/%d=%.4f", OkPlsH.GetDat(PartCntH.GetKey(p)), AllPlsH.GetDat(PartCntH.GetKey(p)), OkPlsH.GetDat(PartCntH.GetKey(p))/double(AllPlsH.GetDat(PartCntH.GetKey(p)))).CStr(),
      TStr::Fmt("%5d/%d=%.4f", OkMnsH.GetDat(PartCntH.GetKey(p)), AllMnsH.GetDat(PartCntH.GetKey(p)), OkMnsH.GetDat(PartCntH.GetKey(p))/double(AllMnsH.GetDat(PartCntH.GetKey(p)))).CStr() );
  }
}

void TWikiTalkNet::GetPartStat(const TVec<TIntV>& PartNIdV) const {
  THash<TIntPr, TIntPr> PartEdgeH;
  TIntH NIdPartH;
  for (int p = 0; p < PartNIdV.Len(); p++) {
    for (int n = 0; n < PartNIdV[p].Len(); n++) {
      NIdPartH.AddDat(PartNIdV[p][n], p);
    }
  }
  TInt DstPart;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (! NIdPartH.IsKey(NI.GetId())) { continue; }
    const int p = NIdPartH.GetDat(NI.GetId());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int Vote = NI.GetOutEDat(e).GetVote();
      TIntPr& IOCnt = PartEdgeH.AddDat(TIntPr(p, Vote));
      if (NIdPartH.IsKeyGetDat(NI.GetOutNId(e), DstPart) && DstPart==p) {
        if (Vote==1) { IOCnt.Val1++; } else { IOCnt.Val2++; } }
      else {
        if (Vote==-1) { IOCnt.Val1++; } else { IOCnt.Val2++; } }
    }
  }
  PartEdgeH.SortByKey();
  printf("Partition statistics (satisfied edges):\n");
  for (int p = 0; p < PartEdgeH.Len(); p++) {
    printf("  %2d  %2d : %6d : %6d  =  %f\n", PartEdgeH.GetKey(p).Val1, PartEdgeH.GetKey(p).Val2,
      PartEdgeH[p].Val1(), PartEdgeH[p].Val2(), PartEdgeH[p].Val1/double(PartEdgeH[p].Val1+PartEdgeH[p].Val2));
  }
}

// get subnetwork on votes
// VoteSign=11 : take +1 and -1 votes
PWikiTalkNet TWikiTalkNet::GetVoteSubNet(const int& VoteSign, const bool& VoteOnly, const bool& TalkOnly) const {
  PWikiTalkNet Net = TWikiTalkNet::New();
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (! (EI().GetVote()==VoteSign || (VoteSign==11 && EI().GetVote()!=0))) { continue; }
    if (VoteOnly && ! EI().IsVoteE()) { continue; }
    if (TalkOnly && ! EI().IsTalkE()) { continue; }
    if (! Net->IsNode(EI.GetSrcNId())) {
      Net->UsrNIdH.AddDat(EI.GetSrcNDat().Usr, EI.GetSrcNId());
      Net->AddNode(EI.GetSrcNId(), EI.GetSrcNDat());
    }
    if (! Net->IsNode(EI.GetDstNId())) {
      Net->UsrNIdH.AddDat(EI.GetDstNDat().Usr, EI.GetDstNId());
      Net->AddNode(EI.GetDstNId(), EI.GetDstNDat());
    }
    Net->AddEdge(EI);
  }
  TSnap::PrintInfo(Net, TStr::Fmt("Vote network: sign %d %s %s",
    VoteSign, VoteOnly?"OnlyVote":"", TalkOnly?"OnlyTalk":""));
  return Net;
}

PSignNet TWikiTalkNet::GetSignNet(const int& VoteSign, const bool& VoteOnly, const bool& TalkOnly) const {
  PSignNet Net = TSignNet::New();
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI.GetSrcNId() == EI.GetDstNId()) { continue; }
    if (! (EI().GetVote()==VoteSign || (VoteSign==11 && EI().GetVote()!=0))) { continue; }
    if (VoteOnly && ! EI().IsVoteE()) { continue; }
    if (TalkOnly && ! EI().IsTalkE()) { continue; }
    if (! Net->IsNode(EI.GetSrcNId())) {
      Net->AddNode(EI.GetSrcNId(), 0); }
    if (! Net->IsNode(EI.GetDstNId())) {
      Net->AddNode(EI.GetDstNId(), 0); }
    Net->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), EI().GetVote());
  }
  //TSnap::PrintInfo(Net, TStr::Fmt("SIGN-NET %d %s %s", VoteSign, VoteOnly?"OnlyVote":"", TalkOnly?"OnlyTalk":""));
  return Net;
}

void TWikiTalkNet::TestPartitions(const TStr& OutFNm) {
  { PSignNet MnsNet = GetSignNet(-1, true, false);
  PSignNet AllNet = GetSignNet(11, true, false);
  PSignNet CoreNet = TSnap::GetKCore(TSnap::GetMxWcc(MnsNet), 2);
  TSnap::PrintInfo(CoreNet, "MINUS K-CORE");
  for (int npart  = 2; npart < 5; npart++) {
    printf("\n**** %d-partitions\n", npart);
    THopfield Hopfield(CoreNet);
    Hopfield.FindStableSet(npart, 1000);
    TVec<TIntV> PartNIdV;
    Hopfield.GetStableSet(990, PartNIdV);
    Hopfield.PlotPartStab(TStr::Fmt("%s-c2-p%d", OutFNm.CStr(), npart));
    CoreNet->GetPartStat(PartNIdV, "NETWORK 2-CORE");
    MnsNet->GetPartStat(PartNIdV, "FULL MINUS NET");
    AllNet->GetPartStat(PartNIdV, "FULL PLUS-MINUS NET");
  } }
  PermuteAllVoteSigns(false);
  printf("\n*** PERMUTE EDGE SIGNS ************************************************************************************\n");
  { PSignNet MnsNet = GetSignNet(-1, true, false);
  PSignNet AllNet = GetSignNet(11, true, false);
  PSignNet CoreNet = TSnap::GetKCore(TSnap::GetMxWcc(MnsNet), 2);
  TSnap::PrintInfo(CoreNet, "MINUS K-CORE");
  for (int npart  = 2; npart < 6; npart++) {
    printf("\n**** %d-partitions\n", npart);
    THopfield Hopfield(CoreNet);
    Hopfield.FindStableSet(npart, 1000);
    TVec<TIntV> PartNIdV;
    Hopfield.GetStableSet(990, PartNIdV);
    Hopfield.PlotPartStab(TStr::Fmt("%s-c2-Rp%d", OutFNm.CStr(), npart));
    CoreNet->GetPartStat(PartNIdV, "NETWORK 2-CORE");
    MnsNet->GetPartStat(PartNIdV, "FULL MINUS NET");
    AllNet->GetPartStat(PartNIdV, "FULL PLUS-MINUS NET");
  } }
}

void TWikiTalkNet::PlotBarnStarDelta(const TStr& OutFNm) const {
  THash<TInt, TMom> DiffMomH;
  TIntH DiffCntH;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI().GetVote() == 0) { continue; }
    //if (! EI().IsVoteTalkE()) { continue; }
    DiffMomH.AddDat(EI.GetSrcNDat().GetStars()-EI.GetDstNDat().GetStars()).Add(EI().GetVote()==1?1:0);
    DiffCntH.AddDat(EI.GetSrcNDat().GetStars()-EI.GetDstNDat().GetStars()) += 1;
  }
  TGnuPlot::PlotValMomH(DiffMomH, "dBarnStars-"+OutFNm, "Number of BarnStars (over ALL VOTES): "+OutFNm, "Barnstars delta (source - destination)",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, true);
  TGnuPlot::PlotValCntH(DiffCntH, "dBarnStarts2-"+OutFNm, "Number of BarnSTars (over aLL VOES): "+OutFNm, "Barnstars delta (source - destination)",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
}

int log10Delta(const int& Delta) {
  //return 10*(Delta/10);
  if (Delta == 0) { return 0; }
  else if (Delta > 0) { return (int)log10((double)Delta)+1; }
  else if (Delta < 0) { return -(int)log10((double)-Delta)+1; }
  Fail; return -1;
}

// For every talk edge
void TWikiTalkNet::PlotFracPosVsWords(const TStr& OutFNm) const {
  THash<TInt, TMom> DiffMomH, DiffMomH2, WrdAE, WrdBE, TlkAE, TlkBE;
  TIntH DiffCntH, DiffCntH2, CWrdAE, CWrdBE, CTlkAE, CTlkBE;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI().GetVote() == 0) { continue; }
    if (! EI().IsVoteTalkE()) { continue; }
    if (EI().GetTalks() < 50) {
      DiffMomH.AddDat(EI().GetTalks()).Add(EI().GetVote()==1?1:0);
      DiffCntH.AddDat(EI().GetTalks()) += 1;
      TlkBE.AddDat((EI().GetTalksBE())).Add(EI().GetVote()==1?1:0);
      CTlkBE.AddDat((EI().GetTalksBE())) += 1;
      TlkAE.AddDat((EI().GetTalksAE())).Add(EI().GetVote()==1?1:0);
      CTlkAE.AddDat((EI().GetTalksAE())) += 1;
    }
    DiffMomH2.AddDat(log10Delta(EI().GetWords())).Add(EI().GetVote()==1?1:0);
    DiffCntH2.AddDat(log10Delta(EI().GetWords())) += 1;
    WrdBE.AddDat(log10Delta(EI().GetWordsBE())).Add(EI().GetVote()==1?1:0);
    CWrdBE.AddDat(log10Delta(EI().GetWordsBE())) += 1;
    WrdAE.AddDat(log10Delta(EI().GetWordsAE())).Add(EI().GetVote()==1?1:0);
    CWrdAE.AddDat(log10Delta(EI().GetWordsAE())) += 1;
  }
  TGnuPlot::PlotValMomH(DiffMomH, "talks-"+OutFNm, "Number of talks: "+OutFNm, "Number of times users talked",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValCntH(DiffCntH, "talks2-"+OutFNm, "Number of talks: "+OutFNm, "Number of times users talked",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValMomH(DiffMomH2, "words-"+OutFNm, "Number of words edited: "+OutFNm, "Number of words edited",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValCntH(DiffCntH2, "words2-"+OutFNm, "Number of words edited: "+OutFNm, "Number of words edited",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
  // words
  TGnuPlot::PlotValMomH(WrdAE, "wordsBE-"+OutFNm, "Number of words edited: "+OutFNm, "Number of words edited BEFORE the election (on talk page between a pair)",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValCntH(CWrdAE, "wordsBE2-"+OutFNm, "Number of words edited: "+OutFNm, "Number of words edited BEFORE the election (on talk page between a pair)",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValMomH(WrdBE, "wordsAE-"+OutFNm, "Number of words edited: "+OutFNm, "Number of words edited AFTER the election (on talk page between a pair)",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValCntH(CWrdBE, "wordsAE2-"+OutFNm, "Number of words edited: "+OutFNm, "Number of words edited AFTER the election (on talk page between a pair)",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
  // talks
  TGnuPlot::PlotValMomH(TlkAE, "talksBE-"+OutFNm, "Number of talks edited: "+OutFNm, "Number of talks BEFORE the election (on talk page between a pair)",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValCntH(CTlkAE, "talksBE2-"+OutFNm, "Number of talks edited: "+OutFNm, "Number of talks BEFORE the election (on talk page between a pair)",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValMomH(TlkBE, "talksAE-"+OutFNm, "Number of talks edited: "+OutFNm, "Number of talks AFTER the election (on talk page between a pair)",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValCntH(CTlkBE, "talksAE2-"+OutFNm, "Number of talks edited: "+OutFNm, "Number of talks AFTER the election (on talk page between a pair)",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
}

// votes based on the characteristic of the target
void TWikiTalkNet::PlotFracPosVsWords2(const TStr& OutFNm) const {
  THash<TInt, TMom> TlkAll, WrdAll, WrdAE, WrdBE, TlkAE, TlkBE, EdtAll, EdtAll2, EdtAll3;
  // for every node compute total words and talks before and after the election
  TIntH NIdTAE, NIdTBE, NIdTalks, NIdWAE, NIdWBE, NIdWords;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    NIdTAE.AddDat(EI.GetDstNId()) += EI().GetTalksAE();
    NIdTBE.AddDat(EI.GetDstNId()) += EI().GetTalksBE();
    NIdTalks.AddDat(EI.GetDstNId()) += EI().GetTalks();
    NIdWAE.AddDat(EI.GetDstNId()) += EI().GetWordsAE();
    NIdWBE.AddDat(EI.GetDstNId()) += EI().GetWordsBE();
    NIdWords.AddDat(EI.GetDstNId()) += EI().GetWords();
  }
  // for every vote get target total talk activity
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI().GetVote() == 0) { continue; }
    const int C = EI.GetDstNId(); // candidate
    if (NIdTAE.GetDat(C)<1000) TlkAE.AddDat(NIdTAE.GetDat(C)).Add(EI().GetVote()==1?1:0);
    if (NIdTBE.GetDat(C)<1000) TlkBE.AddDat(NIdTBE.GetDat(C)).Add(EI().GetVote()==1?1:0);
    if (NIdTalks.GetDat(C)<1000) TlkAll.AddDat(NIdTalks.GetDat(C)).Add(EI().GetVote()==1?1:0);
    WrdAE.AddDat(log10Delta(NIdWAE.GetDat(C))).Add(EI().GetVote()==1?1:0);
    WrdBE.AddDat(log10Delta(NIdWBE.GetDat(C))).Add(EI().GetVote()==1?1:0);
    WrdAll.AddDat(log10Delta(NIdWords.GetDat(C))).Add(EI().GetVote()==1?1:0);
    EdtAll.AddDat(log10Delta(EI.GetDstNDat().GetEdCnt())).Add(EI().GetVote()==1?1:0);
    EdtAll2.AddDat(log10Delta(EI.GetDstNDat().GetAllEdCnt())).Add(EI().GetVote()==1?1:0);
    EdtAll3.AddDat(log10Delta(EI.GetDstNDat().GetTkEdCnt())).Add(EI().GetVote()==1?1:0);

  }
  TGnuPlot::PlotValMomH(EdtAll, "editsTot-"+OutFNm, "Number of edits: "+OutFNm, "Total number of times candidate edited",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(EdtAll2, "editsTot2-"+OutFNm, "Number of edits: "+OutFNm, "Total number of times candidate edited",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(EdtAll3, "editsTot3-"+OutFNm, "Number of edits: "+OutFNm, "Total number of times candidate edited",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  // words
  TGnuPlot::PlotValMomH(TlkAll, "talksTot-"+OutFNm, "Number of talks: "+OutFNm, "Total number of times candidate talked",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WrdAE, "wordsBETot-"+OutFNm, "Number of words edited: "+OutFNm, "1+Log_10 Total number of words edited of candidate BEFORE the election",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(WrdBE, "wordsAETot-"+OutFNm, "Number of words edited: "+OutFNm, "1+Log_10 Total number of words edited of candidate AFTER the election",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  // talks
  TGnuPlot::PlotValMomH(WrdAll, "wordsTot-"+OutFNm, "Number of words edited: "+OutFNm, "1+Log_10 Total number of words candidate edited",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TlkAE, "talksBETot-"+OutFNm, "Number of talks edited: "+OutFNm, "Total number of talks of candidate BEFORE the election",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(TlkBE, "talksAETot-"+OutFNm, "Number of talks edited: "+OutFNm, "Total number of talks of candidate AFTER the election",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
}

void TWikiTalkNet::PlotNodeAttrDistr(const TStr& OutFNm) const {
  TIntH BarnStars, AllEdCnt, AllEdWrds, MnEdCnt, MnEdWrds, TkEdCnt, TkEdWrds;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const TWikiUsr& N = NI();
    BarnStars.AddDat(N.GetStars()) += 1;
    AllEdCnt.AddDat(N.GetAllEdCnt()) += 1;
    AllEdWrds.AddDat(1*(N.GetAllWrdCnt()/1)) += 1;
    MnEdCnt.AddDat(N.GetEdCnt()) += 1;
    MnEdWrds.AddDat(1*(N.GetWrdCnt()/1)) += 1;
    TkEdCnt.AddDat(N.GetTkEdCnt()) += 1;
    TkEdWrds.AddDat(1*(N.GetTkWrdCnt()/1)) += 1;
  }
  TGnuPlot::PlotValCntH(BarnStars, "barnStars-"+OutFNm, OutFNm, "Number of barn stars", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(AllEdCnt, "allEdCnt-"+OutFNm, OutFNm, "Number of edits", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(AllEdWrds, "allEdWrds-"+OutFNm, OutFNm, "Number of edited words", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(MnEdCnt, "MnEdCnt-"+OutFNm, OutFNm, "Number of edits on MAIN pages", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(MnEdWrds, "MnEdWrds-"+OutFNm, OutFNm, "Number of edited words onmain pages", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(TkEdCnt, "tkEdCnt-"+OutFNm, OutFNm, "Number of edits on TALK pages", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(TkEdWrds, "tkEdWrds-"+OutFNm, OutFNm, "Number of edited words onmain pages", "Number of users", gpsLog, false, gpwLinesPoints, false, false);
}

void TWikiTalkNet::PlotFracPosVsEdgeAttr(const TStr& OutFNm) const {
  THash<TInt, TMom> EdgeTalksH, EdgeTalksH2, EdgeWordsH;
  THash<TInt, TMom> dBarnStars, dAllEdCnt, dAllEdWrds;
  TIntH dBarnStarsCnt, dAllEdCntCnt, dAllEdWrdsCnt;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int Val = EI().GetVote() ==1 ? 1 : 0;
    if (EI().GetTalks() < 100) {
      EdgeTalksH.AddDat(int(EI().GetTalks())).Add(Val);
    }
    EdgeTalksH2.AddDat((int)log10((double)EI().GetTalks())).Add(Val);
    EdgeWordsH.AddDat((int)log10((double)EI().GetWords())).Add(Val);
    const TWikiUsr& S = EI.GetSrcNDat();
    const TWikiUsr& D = EI.GetDstNDat();
    if (abs(S.GetStars()-D.GetStars()) < 10) {
      dBarnStars.AddDat(S.GetStars()-D.GetStars()).Add(Val);
      dBarnStarsCnt.AddDat(S.GetStars()-D.GetStars())++;
    }
    dAllEdCnt.AddDat(log10Delta(S.GetAllEdCnt()-D.GetAllEdCnt())).Add(Val);
    dAllEdCntCnt.AddDat(log10Delta(S.GetAllEdCnt()-D.GetAllEdCnt()))++;
    dAllEdWrds.AddDat(log10Delta(S.GetAllWrdCnt()-D.GetAllWrdCnt())).Add(Val);
    dAllEdWrdsCnt.AddDat(log10Delta(S.GetAllWrdCnt()-D.GetAllWrdCnt()))++;
  }
  // plot
  const bool SDev = true;
  TGnuPlot::PlotValMomH(EdgeTalksH, "edgeTks"+OutFNm, OutFNm, "Number of talks over the edge", "Fraction of positive edges", gpsAuto, gpwLinesPoints, true, false, false, false, false, SDev);
  TGnuPlot::PlotValMomH(EdgeTalksH2, "edgeTks2"+OutFNm, OutFNm, "1+Log_{10} Number of talks over the edge", "Fraction of positive edges", gpsAuto, gpwLinesPoints, true, false, false, false, false, SDev);
  TGnuPlot::PlotValMomH(EdgeWordsH, "edgeWrds"+OutFNm, OutFNm, "1+Log_{10} Number of words over all talks over the edge", "Fraction of positive edges", gpsAuto, gpwLinesPoints, true, false, false, false, false, SDev);
  TGnuPlot::PlotValMomH(dBarnStars, "dBarnStars-"+OutFNm, OutFNm, "delta BarnStars", "Fraction of positive edges", gpsAuto, gpwLinesPoints, true, false, false, false, false, SDev);
  TGnuPlot::PlotValMomH(dAllEdCnt, "dAllEdCnt-"+OutFNm, OutFNm, "delta AllEdCnt", "Fraction of positive edges", gpsAuto, gpwLinesPoints, true, false, false, false, false, SDev);
  TGnuPlot::PlotValMomH(dAllEdWrds, "dAllEdWrds-"+OutFNm, OutFNm, "delta AllEdWrds", "Fraction of positive edges", gpsAuto, gpwLinesPoints, true, false, false, false, false, SDev);
  TGnuPlot::PlotValCntH(dBarnStarsCnt, "dBarnStarsCnt-"+OutFNm, OutFNm, "delta BarnStars", "Count", gpsAuto);
  TGnuPlot::PlotValCntH(dAllEdCntCnt, "dAllEdCntCnt-"+OutFNm, OutFNm, "delta AllEdCnt", "Count", gpsAuto);
  TGnuPlot::PlotValCntH(dAllEdWrdsCnt, "dAllEdWrdsCnt-"+OutFNm, OutFNm, "delta AllEdWrds", "Count", gpsAuto);

}

// plot the span of a particular edge:
//   number of common friends of the endpoints
//   path length between the endpoints
void TWikiTalkNet::PlotVoteSignCmnFriends(const TStr& OutFNm) const {
  TFltFltH SupRngH, OppRngH; // path length
  TFltFltH SupCmnH, OppCmnH; // common friends
  THash<TFlt, TMom> RngFracH, CmnFracH; // fraction of supporters
  PWikiTalkNet ThisPt = PWikiTalkNet((TWikiTalkNet*) this);
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int C = -1; Fail;//TGAlg::GetCmnNbhs(ThisPt, EI.GetSrcNId(), EI.GetDstNId(), false);
    const int R = -1; //TGAlg::GetRangeDist(ThisPt, EI.GetSrcNId(), EI.GetDstNId(), false);
    if (EI().GetVote() == 1) {
      SupRngH.AddDat(R)++;  RngFracH.AddDat(R).Add(1);
      SupCmnH.AddDat(C)++;  CmnFracH.AddDat(C).Add(1);
    } else if (EI().GetVote() == -1) {
      OppRngH.AddDat(R)++;  RngFracH.AddDat(R).Add(0);
      OppCmnH.AddDat(C)++;  CmnFracH.AddDat(C).Add(0);
    }
  }
  TGnuPlot::PlotValCntH(SupRngH, "rngSup-"+OutFNm, "range of support votes", "range (path length after edge is removed)", "count");
  TGnuPlot::PlotValCntH(OppRngH, "rngOpp-"+OutFNm, "range of opposing votes", "range (path length after edge is removed)", "count");
  TGnuPlot::PlotValCntH(SupCmnH, "cmnSup-"+OutFNm, "number of common friends of support votes", "number of common friends", "count", gpsLog);
  TGnuPlot::PlotValCntH(OppCmnH, "cmnOpp-"+OutFNm, "number of common friends of opposing votes", "number of common friends", "count", gpsLog);
  TGnuPlot::PlotValMomH(RngFracH, "fracRng-"+OutFNm, "fraction of support edges spanning range X", "range", "fraction of support edges");
  TGnuPlot::PlotValMomH(CmnFracH, "fracCmn-"+OutFNm, "fraction of support edges having X common neighbors", "number of common neighbors", "fraction of support edges", gpsLog);
}

void TWikiTalkNet::SaveAreaUTrailAttr(const TStr& OutFNm, const int& MinUsrVotes, const TWikiElecBs& ElecBs) const {
  TIntV UIdV;
  TFltV AreaV;
  TIntSet AdminSet, ElecUsrV;
  { TIntSet FqVoterSet; ElecBs.GetFqVoters(FqVoterSet, MinUsrVotes, 10, false); FqVoterSet.GetKeyV(UIdV);
  TIntV ElecUIdV; ElecBs.GetElecUsrV(ElecUIdV); ElecUsrV=TIntSet(ElecUIdV); }
  ElecBs.GetUsrAreaUTrail(UIdV, AreaV);
  ElecBs.GetAdminSet(AdminSet);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "Area\tUser\tIsAdmin\tIsElec\tTalkOutDeg\tInDeg\tTalkTriads\tBarnStars\tInAdmins\tOutAdmins\tAllEdCnt\tAllWrdCnt\tRevCnt\tRevWrds\n");
  printf("%d users", UIdV.Len());
  int skip=0;
  for (int u = 0; u < UIdV.Len(); u++) {
    TStr Usr = ElecBs.GetUsr(UIdV[u]);
    if (! IsUsr(Usr)) {  printf("x"); skip++; continue; } else { printf("."); }
    fprintf(F, "%f\t%s\t%d\t%d", AreaV[u], Usr.CStr(), AdminSet.IsKey(UIdV[u])?1:0, ElecUsrV.IsKey(UIdV[u])?1:0);
    TNodeI U =  GetNI(GetUsrNId(Usr));
    fprintf(F, "\t%d\t%d\t%d", U.GetOutDeg(), U.GetInDeg(), TSnap::GetNodeTriads<PWikiTalkNet>((TWikiTalkNet*)this, U.GetId()));
    int OutAdmins=0, InAdmins=0;
    for (int i = 0; i < U.GetOutDeg(); i++) {
      if (U.GetOutNDat(i).IsAdmin()) { OutAdmins++; } }
    for (int i = 0; i < U.GetInDeg(); i++) {
      if (U.GetInNDat(i).IsAdmin()) { InAdmins++; } }
    fprintf(F, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", U().GetStars(), OutAdmins, InAdmins,
      U().GetAllEdCnt(), U().GetAllWrdCnt(), U().GetRevCnt(), U().GetRevWrds());
    fflush(F);
  }
  fclose(F);
  printf("\n%d users, %d skip\n", UIdV.Len(), skip);
}

void TWikiTalkNet::DumpEdgeStat() const {
  TIntH VoteE, TalkE, AllE, VtTkE;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const TWikiTalkEdge& E = EI();
    const int V = E.GetVote();
    AllE.AddDat(V) += 1;
    if (E.IsTalkE()) { TalkE.AddDat(V)+=1; }
    if (E.IsVoteE()) { VoteE.AddDat(V)+=1; }
    if (E.IsVoteTalkE()) { VtTkE.AddDat(V)+=1; }
  }
  VoteE.SortByKey(); TalkE.SortByKey(); AllE.SortByKey(); VtTkE.SortByKey();
  printf("Edge\tAll\tTalk\tVote\tVoteTalk\n");
  for (int e = 0; e < AllE.Len(); e++) {
    const int v = AllE.GetKey(e);
    printf("%4d\t%7d\t%7d\t%7d\t%7d\n", v, AllE[e], TalkE.IsKey(v)?TalkE.GetDat(v):0,
      VoteE.IsKey(v)?VoteE.GetDat(v):0, VtTkE.IsKey(v)?VtTkE.GetDat(v):0);
  }
  printf("\n");
}

void TWikiTalkNet::ClearElecData() {
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    EI().VoteSign = -1;
    EI().VoteTm = TSecTm();
  }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NI().Admin = false;
    NI().ElecTm = TSecTm();
  }
}

void TWikiTalkNet::ImposeElecNet(const TWikiElecBs& ElecBs, const THash<TChA, TChA>& UsrChageH, const bool& AddVoteOnlyEdges) {
  int GoodEl=0, ESet=0, ESkip=0, GoodVotes=0, AllCmts=0, VoteEdge=0;
  int DstNf=0, SrcNf=0;
  for (int e = 0; e < ElecBs.Len(); e++) {
    const TWikiElec& E = ElecBs[e];
    TChA DstUsr = ElecBs.GetUsr(E.GetUId());  DstUsr.ToLc();
    if (UsrChageH.IsKey(DstUsr)) { DstUsr = UsrChageH.GetDat(DstUsr); }
    if (! UsrNIdH.IsKey(DstUsr)) { DstNf++; continue; }
    GoodEl++;
    TWikiElec Votes;
    E.GetOnlyVotes(Votes, false);
    AllCmts += E.Len();
    GoodVotes += Votes.Len();
    for (int v = 0; v < Votes.Len(); v++) {
      TChA SrcUsr = ElecBs.GetUsr(E.GetVote(v).GetUId());  SrcUsr.ToLc();
      if (UsrChageH.IsKey(SrcUsr)) { SrcUsr = UsrChageH.GetDat(SrcUsr); }
      if (! UsrNIdH.IsKey(SrcUsr)) { SrcNf++;  continue; }
      const int SrcId = UsrNIdH.GetDat(SrcUsr);
      const int DstId = UsrNIdH.GetDat(DstUsr);
      if (SrcId==DstId) { continue; }
      if (IsEdge(SrcId, DstId)) {
        //IAssert(! GetEDat(SrcId, DstId).VoteTm.IsDef());
        //IAssert(GetEDat(SrcId, DstId).GetTalks() > 0);
        GetEDat(SrcId, DstId).VoteSign = E.GetVote(v).GetVote();
        GetEDat(SrcId, DstId).VoteTm = E.GetVote(v).GetTm();
        ESet++;
      }
      else if (AddVoteOnlyEdges) {
        AddEdge(SrcId, DstId);
        TWikiTalkEdge& EdgeDat = GetEDat(SrcId, DstId);
        EdgeDat.VoteSign = E[v].GetVote();
        EdgeDat.VoteTm = E[v].GetTm();
        VoteEdge++;
      }
      else { ESkip++; }
    }
  }
  printf("admins that talk (good elections): %d / %d\n", GoodEl, ElecBs.Len());
  printf("actual votes vs. comments in elections: %d / %d : %f\n", GoodVotes, AllCmts, GoodVotes/double(AllCmts));
  printf("votes where both users talk %d (%f of votes where nodes exist)\n", ESet, ESet/double(ESet+ESkip));
  printf("fraction of all votes found in the graph: %d/%d : %f\n", ESet, GoodVotes, ESet/double(GoodVotes));
  printf("destination not found: %d\n", DstNf);
  printf("source not found: %d\n", SrcNf);
  printf("talk+vote: %d  vote: %d  skip: %d\n", ESet, VoteEdge, ESkip);
}

PWikiTalkNet TWikiTalkNet::LoadTalkNet(const TStr& ParsedWikiDir, const TWikiElecBs& ElecBs) {
  // data files
  const TStr WikiUsrTalk = "enwiki-20080103.user_talk.7z"; // talk network
  const TStr WikiMain = "enwiki-20080103.main.7z";
  const TStr WikiMainTalk = "enwiki-20080103.talk.7z";
  const TStr WikiWiki = "enwiki-20080103.wikipedia.7z";
  const TStr WikiWikiTalk = "enwiki-20080103.wikipedia_talk.7z";
  // misc files
  const TStr BarnStartList = "barnstars.history.unsorted";
  const TStr AdminList = "enwiki.admins2009.txt";
  const TStr BotList = "enwiki.botlist.2007-03";
  const TStr UsrChanges = "enwiki.important-username-changes.2007-08-06.txt";
  // load bot list (skip bots) and username change list
  THash<TChA, TChA> UsrMapH;
  THashSet<TChA> BotSet;
  THash<TChA, TInt> UsrElecId; // users that went up for election
  TChA Ln;
  TExeTm ExeTm;
  for (TFIn FIn(ParsedWikiDir+BotList); FIn.GetNextLn(Ln); ) {
    BotSet.AddKey(Ln.ToLc()); }
  printf("Bots: %d bots\n", BotSet.Len());
  for (TSsParser Ss(ParsedWikiDir+UsrChanges, ssfSpaceSep); Ss.Next(); ) {
    TChA U1=Ss[0], U2=Ss[1]; if (U1.ToLc()!=U2.ToLc()) { UsrMapH.AddDat(U1,U2); }
  }
  printf("User changes: %d chages\n", UsrMapH.Len());
  for (int e = 0; e < ElecBs.Len(); e++) {
    UsrElecId.AddDat(ElecBs.GetUsr(ElecBs[e].GetUId()), e); }
  printf("Elections: %d users up for election\n", UsrElecId.Len());
  // create network
  printf("Load WikiTalk network:");
  PWikiTalkNet Net = TWikiTalkNet::New();
  printf("  Load %s ", WikiUsrTalk.CStr());
  { TWikiMetaLoader WML(ParsedWikiDir+WikiUsrTalk);
  int src, dst, k, elecSet=0;
  for (int edges=0; WML.Next(); edges++) {
    if (! WML.Title.IsPrefix("User_talk:")) { printf("."); continue; }
    const int b = (int) strlen("User_talk:");
    int e2 = WML.Title.SearchCh('/', b)-1;
    if (e2<0) { e2=TInt::Mx; }
    TChA Dst = WML.Title.GetSubStr(b, e2).ToLc();
    TChA Src = WML.Usr.ToLc();
    if (BotSet.IsKey(Src) || BotSet.IsKey(Dst)) { printf(""); continue; }
    if (TWikiMetaLoader::IsIpAddr(Src) || TWikiMetaLoader::IsIpAddr(Dst)) { printf(""); continue; }
    if (Src != Dst) { // skip self edges
      k = UsrMapH.GetKeyId(Src);
      if (k != -1) { Src = UsrMapH[k]; }
      k = UsrMapH.GetKeyId(Dst);
      if (k != -1) { Dst = UsrMapH[k]; }
      src = Net->UsrNIdH.AddDatId(Src);
      dst = Net->UsrNIdH.AddDatId(Dst);
      IAssert(src != dst);
      if (! Net->IsNode(src)) {
        Net->AddNode(src, TWikiUsr(Src));
        if (UsrElecId.IsKey(Src)) { printf("T"); elecSet++; // set election time
          Net->GetNDat(src).ElecTm = ElecBs.GetElec(UsrElecId.GetDat(Src)).GetTm();  }
      }
      if (! Net->IsNode(dst)) {
        Net->AddNode(dst, TWikiUsr(Dst));
        if (UsrElecId.IsKey(Dst)) { printf("T"); elecSet++; // set election time
          Net->GetNDat(dst).ElecTm = ElecBs.GetElec(UsrElecId.GetDat(Dst)).GetTm(); }
      }
      if (! Net->IsEdge(src, dst)) {
        Net->AddEdge(src, dst, TWikiTalkEdge(WML.RevTm, WML.RevTm, 1, WML.RevWrds));
      }
      TWikiTalkEdge& TalkEdge = Net->GetEDat(src, dst);
      if (TalkEdge.FirstTalk > WML.RevTm) { TalkEdge.FirstTalk = WML.RevTm; }
      if (TalkEdge.LastTalk < WML.RevTm) { TalkEdge.LastTalk = WML.RevTm; }
      TalkEdge.TotWords += WML.RevWrds;
      TalkEdge.TotTalks += 1;
      const TSecTm DstElecTm = Net->GetNDat(dst).ElecTm;
      if (DstElecTm.IsDef()) { // talks before/after the election
        if (WML.RevTm < DstElecTm) { TalkEdge.TalksBE++; TalkEdge.WordsBE+=WML.RevWrds; }
        else { TalkEdge.TalksAE++; TalkEdge.WordsAE+=WML.RevWrds; }
      }
    }
  }
  printf("DONE NET[%s]\n", ExeTm.GetStr());  ExeTm.Tick();
  printf("node election time %d / %d (%f)\n", elecSet, UsrElecId.Len(), elecSet/double(UsrElecId.Len())); }
  TSnap::PrintInfo(Net, "WikiUserTalk network.");
  // load barnstarts
  printf(" [%s]\n  Loading %s\n", ExeTm.GetStr(), BarnStartList.CStr());  ExeTm.Tick();
  int cnt=0, cnt2=0;
  for (TSsParser Ss(ParsedWikiDir+BarnStartList, ssfSpaceSep); Ss.Next(); cnt2++) { TChA U1=Ss[3]; U1.ToLc();
    if (UsrMapH.IsKey(U1)) { U1 = UsrMapH.GetDat(U1); }
    if (Net->UsrNIdH.IsKey(U1)) { cnt++;
      Net->GetNDat(Net->UsrNIdH.GetDat(U1)).BarnStars += 1; }
  }
  printf("    %d / %d stars set [%s]\n  Loading %s\n", cnt, cnt2, ExeTm.GetStr(), AdminList.CStr());  ExeTm.Tick();
  // load admins
  cnt=0; cnt2=0;
  for (TFIn FIn(ParsedWikiDir+AdminList); FIn.GetNextLn(Ln); cnt2++) { Ln.ToLc();
    if (UsrMapH.IsKey(Ln)) { Ln = UsrMapH.GetDat(Ln); }
    if (Net->UsrNIdH.IsKey(Ln)) { cnt++;
      Net->GetNDat(Net->UsrNIdH.GetDat(Ln)).Admin = true; }
  }//*/
  printf("    %d / %d admins set [%s]\n", cnt, cnt2, ExeTm.GetStr());
  TIntSet AdminSet; ElecBs.GetAdminSet(AdminSet);
  for (int a = 0; a < AdminSet.Len(); a++, cnt2++) {
    TChA Ln = ElecBs.GetUsr(AdminSet[a]);
    if (Net->UsrNIdH.IsKey(Ln)) { cnt++;
      Net->GetNDat(Net->UsrNIdH.GetDat(Ln)).Admin = true; }
  }
  printf("    %d / %d admins set [%s]\n", cnt, cnt2, ExeTm.GetStr());
  // impose election net
  Net->ImposeElecNet(ElecBs, UsrMapH, true); // add also vote only edges
  TSnap::PrintInfo(Net);
  //Net->Save(TFOut("tmp.net"));
  printf("SAVE DONE.\n\nLoading user edit statistics:\n");

  //// load user stat
  //TInt MnEdCnt, MnEdWrds;      // number of edits, changed words (in main namespace)
  printf("  Loading %s ", WikiMain.CStr());
  { TWikiMetaLoader WML(ParsedWikiDir+WikiMain);
  for (int rec=0; WML.Next(); rec++) {
    const TChA& U = WML.Usr;
    const int keyId = Net->UsrNIdH.GetKeyId(U);
    if (keyId == -1) { continue; }
    TWikiUsr& WU = Net->GetNDat(Net->UsrNIdH[keyId]);
    WU.MnEdCnt += 1;  WU.MnEdWrds += WML.RevWrds;
    WML.CommentStr.ToLc();
    if (WML.CommentStr.IsPrefix("rv") || WML.CommentStr.SearchStr("revert")!=-1) {
      WU.MnRevCnt+=1; WU.MnRevWrds+=WML.RevWrds; }
  } }
  //TInt MnTkEdCnt, MnTkEdWrds;  // number of edits, changed words (in talk pages of main namespace)
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiMainTalk.CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(ParsedWikiDir+WikiMainTalk);
  for (int rec=0; WML.Next(); rec++) {
    const TChA& U = WML.Usr.ToLc();
    const int keyId = Net->UsrNIdH.GetKeyId(U);
    if (keyId == -1) { continue; }
    TWikiUsr& WU = Net->GetNDat(Net->UsrNIdH[keyId]);
    WU.MnTkEdCnt += 1;  WU.MnTkEdWrds += WML.RevWrds;
  } }
  //TInt WkEdCnt, WkEdWrds;      // number of edits, changed words (in wikipedia namespace)
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiWiki.CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(ParsedWikiDir+WikiWiki);
  for (int rec=0; WML.Next(); rec++) {
    const TChA& U = WML.Usr.ToLc();
    const int keyId = Net->UsrNIdH.GetKeyId(U);
    if (keyId == -1) { continue; }
    TWikiUsr& WU = Net->GetNDat(Net->UsrNIdH[keyId]);
    WU.WkEdCnt += 1;  WU.WkEdWrds += WML.RevWrds;
  } }
  //TInt WkTkEdCnt, WkTkEdWrds;  // number of edits, changed words (in talk pages of wikipedia namespace)
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiWikiTalk.CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(ParsedWikiDir+WikiWikiTalk);
  for (int rec=0; WML.Next(); rec++) {
    const TChA& U = WML.Usr.ToLc();
    const int keyId = Net->UsrNIdH.GetKeyId(U);
    if (keyId == -1) { continue; }
    TWikiUsr& WU = Net->GetNDat(Net->UsrNIdH[keyId]);
    WU.WkTkEdCnt += 1;  WU.WkTkEdWrds += WML.RevWrds;
  } }
  TSnap::PrintInfo(Net, "WikiUserTalk network.");
  printf("\n[%s]\nDONE.\n", ExeTm.GetStr());
  // done */
  return Net;
}

PWikiTalkNet TWikiTalkNet::LoadSlashdot(const TStr& InFNm) {
  THashSet<TChA> NIdSet;
  TChA LnStr;
  TVec<char *> WrdV;
  int Sign;
  PWikiTalkNet Net = TWikiTalkNet::New();
  for (TFIn FIn(InFNm); FIn.GetNextLn(LnStr); ) {
    if (LnStr.Empty() || LnStr[0]=='#') { continue; }
    LnStr.ToLc();
    TStrUtil::SplitOnCh(LnStr, WrdV, '\t', false);
    NIdSet.AddKey(WrdV[0]);
    if (strcmp(WrdV[1], "friends")==0) { Sign = 1; }
    else if (strcmp(WrdV[1], "fans")==0) { continue; } // skip
    else if (strcmp(WrdV[1], "foes")==0) { Sign = -1; }
    else { Fail; }
    const int SrcNId = NIdSet.AddKey(WrdV[0]);
    if (! Net->IsNode(SrcNId)) {
      Net->AddNode(SrcNId, TWikiUsr(WrdV[0]));
      Net->UsrNIdH.AddDat(WrdV[0], SrcNId);
    }
    for (int e = 2; e < WrdV.Len(); e++) {
      const int DstNId = NIdSet.AddKey(WrdV[e]);
      if (! Net->IsNode(DstNId)) {
        Net->AddNode(DstNId, TWikiUsr(WrdV[e]));
        Net->UsrNIdH.AddDat(WrdV[e], DstNId);
      }
      if (SrcNId != DstNId && ! Net->IsEdge(SrcNId, DstNId)) {
        Net->AddEdge(SrcNId, DstNId, TWikiTalkEdge(Sign));
      }
    }
  }
  TSnap::PrintInfo(Net, InFNm);
  return Net;
}

// Load: class TWikiVoteNet : public TNodeEDatNet<TStr, TInt>
PWikiTalkNet TWikiTalkNet::LoadOldNet(const TStr& InFNm) {
  typedef TNodeEDatNet<TStr, TInt> TOldNet;
  PWikiTalkNet Net = TWikiTalkNet::New();
  TPt<TOldNet> ON = TOldNet::Load(TZipIn(InFNm));
  TSnap::PrintInfo(ON);
  for(TOldNet::TEdgeI EI = ON->BegEI(); EI<ON->EndEI(); EI++) {
    if (! Net->IsNode(EI.GetSrcNId())) {
      Net->AddNode(EI.GetSrcNId(), TWikiUsr(EI.GetSrcNDat().GetLc()));
      Net->UsrNIdH.AddDat(EI.GetDstNDat().GetLc(), EI.GetSrcNId());
    }
    if (! Net->IsNode(EI.GetDstNId())) {
      Net->AddNode(EI.GetDstNId(), TWikiUsr(EI.GetDstNDat().GetLc()));
      Net->UsrNIdH.AddDat(EI.GetDstNDat().GetLc(), EI.GetDstNId());
    }
    Net->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), TWikiTalkEdge(EI()));
  }
  TSnap::PrintInfo(Net);
  return Net;
}


/////////////////////////////////////////////////
// Wikipedia Talk net over time
PNGraph TWikiTimeTalkNet::GetBeforeTimeG(const TSecTm& EdgeTm) const {
  PNGraph G = TNGraph::New();
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI().Tm <= EdgeTm) {
      if (! G->IsNode(EI.GetSrcNId())) { G->AddNode(EI.GetSrcNId()); }
      if (! G->IsNode(EI.GetDstNId())) { G->AddNode(EI.GetDstNId()); }
      G->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
    }
  }
  return G;
}

PSignNet TWikiTimeTalkNet::GetBeforeTimeNet(const TSecTm& EdgeTm) const {
  PSignNet Net = TSignNet::New();
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI().Tm <= EdgeTm) {
      if (! Net->IsNode(EI.GetSrcNId())) { Net->AddNode(EI.GetSrcNId()); }
      if (! Net->IsNode(EI.GetDstNId())) { Net->AddNode(EI.GetDstNId()); }
      Net->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
      Net->GetEDat(EI.GetSrcNId(), EI.GetDstNId()) += EI().Words;
    }
  }
  return Net;
}

void TWikiTimeTalkNet::SaveDataset(const TWikiElecBs& ElecBs, const TStr& OutFNm) const {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "SuccElec\tRfa\tNetNodes\tNetEdges\tNetConstraint\tBarnStars\tTalkOutDeg\tTalkInDeg\tTalkTriads\tInWords\tOutWords\tInAdmins\tOutAdmins\tAdminTriads\tInAdminWords\tOutAdminWords\n");
  // load admin list
  TStrSet AdminSet;
  for (TSsParser Ss("W:\\Data\\wiki20080103-parseByGuerogy\\enwiki.admins2009.txt", ssfTabSep); Ss.Next(); ) {
    TStr A=Ss[0]; AdminSet.AddKey(A.GetTrunc().GetLc());
  }
  TExeTm ExeTm;
  TBarnStars  BarnStars;
  TIntSet AdminNIdSet;
  printf("Admin list: %d\n", AdminSet.Len());
  for (int e = 0; e < ElecBs.Len(); e++) {
    const TWikiElec& E = ElecBs.GetElec(e);
    const TStr Usr = ElecBs.GetUsr(E.UsrId);
    printf("%s: %s   ", Usr.CStr(), E.RfaTitle.CStr());
    if (! UsrNIdH.IsKey(Usr)) {
      printf(" DOES NOT TALK\n", Usr.CStr()); continue;
    }
    PSignNet Net = GetBeforeTimeNet(E.ElecTm);
    const int NId = UsrNIdH.GetDat(Usr);
    if (! Net->IsNode(NId)) {
      printf(" TALKS AFTER ELECTION\n"); continue;
    }
    const TSignNet::TNodeI NI = Net->GetNI(NId);
    printf(" deg %d:%d ", NI.GetInDeg(), NI.GetOutDeg());
    int InWgt=0, OutWgt=0, InAdmins=0, OutAdmins=0, InAWgt=0, OutAWgt=0;
    AdminNIdSet.Clr(false);
    AdminNIdSet.AddKey(NI.GetId());
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      OutWgt += NI.GetOutEDat(i);
      if (AdminSet.IsKey(UsrNIdH.GetKey(NI.GetOutNId(i)))) {
        OutAWgt += NI.GetOutEDat(i);
        OutAdmins++;
        AdminNIdSet.AddKey(NI.GetOutNId(e));
      }
    }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      InWgt += NI.GetInEDat(i);
      if (AdminSet.IsKey(UsrNIdH.GetKey(NI.GetInNId(i)))) {
        InAWgt += NI.GetInEDat(i);
        InAdmins++;
        AdminNIdSet.AddKey(NI.GetInNId(e));
      }
    }
    TSnap::TNetConstraint<PSignNet> NetC(Net, false);
    NetC.CalcConstraints(NId);
    TIntV AdminV; AdminNIdSet.GetKeyV(AdminV);
    const double C = NetC.GetNodeC(NId);
    const int BS = BarnStars.GetBarnStars(Usr, E.GetTm());
    const int Triads = TSnap::GetNodeTriads(Net, NI.GetId());
    const int AdmintTriads = TSnap::GetNodeTriads(TSnap::GetSubGraph(Net, AdminV), NI.GetId());
    fprintf(F, "%d\t%s\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", E.IsSucc?1:0, E.RfaTitle.CStr(), Net->GetNodes(), Net->GetEdges(), C, BS,
      NI.GetOutDeg(), NI.GetInDeg(), Triads, InWgt, OutWgt, InAdmins, OutAdmins, AdmintTriads, InAWgt, OutAWgt);
    fflush(F);
    printf(" [%s]\n", ExeTm.GetStr());
  }
  fclose(F);
}

PWikiTimeTalkNet TWikiTimeTalkNet::LoadWikiTimeTalkNet() {
  const TStr WikiUsrTalk = "W:\\data\\wiki20080103-parseByGuerogy\\enwiki-20080103.user_talk.7z"; // talk network
  const TStr UsrChanges = "W:\\data\\wiki20080103-parseByGuerogy\\enwiki.important-username-changes.2007-08-06";
  const TStr BotList = "W:\\data\\wiki20080103-parseByGuerogy\\enwiki.botlist.2007-03";
  // load bot list (skip bots) and username change list
  THash<TChA, TChA> UsrMapH;
  THashSet<TChA> BotSet;
  TChA Ln;
  TExeTm ExeTm;
  for (TFIn FIn(BotList); FIn.GetNextLn(Ln); ) {
    BotSet.AddKey(Ln.ToLc()); }
  printf("Bots: %d bots\n", BotSet.Len());
  for (TSsParser Ss(UsrChanges, ssfSpaceSep); Ss.Next(); ) {
    TChA U1=Ss[3], U2=Ss[4]; if (U1.ToLc()!=U2.ToLc()) { UsrMapH.AddDat(U1,U2); } }
  printf("User changes: %d chages\n", UsrMapH.Len());
  // create network
  printf("Load Take network before the election:");
  PWikiTimeTalkNet Net = TWikiTimeTalkNet::New();
  printf("  Load %s ", WikiUsrTalk.CStr());
  TWikiMetaLoader WML(WikiUsrTalk);
  int src, dst, k;
  for (int edges=0; WML.Next(); edges++) {
    if (! WML.Title.IsPrefix("User_talk:")) { printf("."); continue; }
    const int b = (int) strlen("User_talk:");
    int e2 = WML.Title.SearchCh('/', b)-1;
    if (e2<0) { e2=TInt::Mx; }
    TChA Dst = WML.Title.GetSubStr(b, e2).ToLc();
    TChA Src = WML.Usr.ToLc();
    if (BotSet.IsKey(Src) || BotSet.IsKey(Dst)) { printf(""); continue; }
    if (TWikiMetaLoader::IsIpAddr(Src) || TWikiMetaLoader::IsIpAddr(Dst)) { printf(""); continue; }
    if (Src != Dst) { // skip self edges
      k = UsrMapH.GetKeyId(Src);
      if (k != -1) { Src = UsrMapH[k]; }
      k = UsrMapH.GetKeyId(Dst);
      if (k != -1) { Dst = UsrMapH[k]; }
      src = Net->UsrNIdH.AddDatId(Src);
      dst = Net->UsrNIdH.AddDatId(Dst);
      if (src != dst) {
        if (! Net->IsNode(src)) { Net->AddNode(src, Src); }
        if (! Net->IsNode(dst)) { Net->AddNode(dst, Dst); }
        Net->AddEdge(src, dst, -1, TWikiTalkEdge2(WML.RevTm, WML.RevWrds));
      }
    }
  }
  printf("DONE NET[%s]\n", ExeTm.GetStr());  ExeTm.Tick();
  return Net;
}

/////////////////////////////////////////////////
// Wikipedia Edit Counts

// load edit counts before the user went up for election
void TWikiEditCnt::LoadTxtBeforeElec(const TWikiElecBs& ElecBs) {
  const TStr WikiMain = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.main.7z";
  const TStr WikiMainTalk = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.talk.7z";
  const TStr WikiWiki = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.wikipedia.7z";
  const TStr WikiWikiTalk = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.wikipedia_talk.7z";
  const TStr UsrChanges = "W:\\data\\wiki20080103-parseByGuerogy\\enwiki.important-username-changes.2007-08-06";
  THash<TChA, TChA> UsrMapH;
  for (TSsParser Ss(UsrChanges, ssfSpaceSep); Ss.Next(); ) {
    TChA U1=Ss[3], U2=Ss[4]; if (U1.ToLc()!=U2.ToLc()) { UsrMapH.AddDat(U1,U2); } }
  // Load only statistics for users going up for election
  THash<TChA, TVec<TPair<TChA, TSecTm> > > UsrToRfaTmH;
  for (int e = 0; e < ElecBs.Len(); e++) {
    UsrToRfaTmH.AddDat(ElecBs.GetUsr(ElecBs[e].GetUId())).Add(TPair<TChA, TSecTm>(ElecBs[e].RfaTitle, ElecBs[e].GetTm()));
  }
  TExeTm ExeTm;
  //// load user stat
  printf("  Loading %s ", WikiMain.GetFMid().CStr());
  { TWikiMetaLoader WML(WikiMain);
  for (int rec=0; WML.Next(); rec++) {
    TChA U = WML.Usr;
    if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
    if (! UsrToRfaTmH.IsKey(U)) { continue; }
    const TVec<TPair<TChA, TSecTm> >& V = UsrToRfaTmH.GetDat(U);
    for (int t = 0; t < V.Len(); t++) {
      if (WML.RevTm <= V[t].Val2 ) { // edits only before the election
        TEditCnt& EC = RfaEdCntH.AddDat(V[t].Val1);
        EC.MainE+=1; EC.MainW+=WML.RevWrds;
        WML.CommentStr.ToLc();
        if (WML.CommentStr.IsPrefix("rv") || WML.CommentStr.SearchStr("revert")!=-1) {
          EC.RevE+=1; EC.RevW+=WML.RevWrds;
        }
      }
    }
  } }
  Save(TZipOut("wikiEditCounts.BeforeElec.rar"));
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiMainTalk.GetFMid().CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(WikiMainTalk);
  for (int rec=0; WML.Next(); rec++) {
    TChA U = WML.Usr;
    if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
    if (! UsrToRfaTmH.IsKey(U)) { continue; }
    const TVec<TPair<TChA, TSecTm> >& V = UsrToRfaTmH.GetDat(U);
    for (int t = 0; t < V.Len(); t++) {
      if (WML.RevTm <= V[t].Val2 ) {
        TEditCnt& EC = RfaEdCntH.AddDat(V[t].Val1);
        EC.MainTE+=1; EC.MainTW+=WML.RevWrds;
      }
    }
  } }
  Save(TZipOut("wikiEditCounts.BeforeElec.rar"));
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiWiki.GetFMid().CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(WikiWiki);
  for (int rec=0; WML.Next(); rec++) {
    TChA U = WML.Usr;
    if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
    if (! UsrToRfaTmH.IsKey(U)) { continue; }
    const TVec<TPair<TChA, TSecTm> >& V = UsrToRfaTmH.GetDat(U);
    for (int t = 0; t < V.Len(); t++) {
      if (WML.RevTm <= V[t].Val2 ) {
        TEditCnt& EC = RfaEdCntH.AddDat(V[t].Val1);
        EC.WikiE+=1; EC.WikiW+=WML.RevWrds;
      }
    }
  } }
  Save(TZipOut("wikiEditCounts.BeforeElec.rar"));
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiWikiTalk.GetFMid().CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(WikiWikiTalk);
  for (int rec=0; WML.Next(); rec++) {
    TChA U = WML.Usr;
    if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
    if (! UsrToRfaTmH.IsKey(U)) { continue; }
    const TVec<TPair<TChA, TSecTm> >& V = UsrToRfaTmH.GetDat(U);
    for (int t = 0; t < V.Len(); t++) {
      if (WML.RevTm <= V[t].Val2 ) {
        TEditCnt& EC = RfaEdCntH.AddDat(V[t].Val1);
        EC.WikiTE+=1; EC.WikiTW+=WML.RevWrds;
      }
    }
  } }
  Save(TZipOut("wikiEditCounts.BeforeElec.rar"));
  printf(" [%s]\n", ExeTm.GetStr());
}

// edit counts of all users that ever casted a vote
void TWikiEditCnt::LoadTxtAll(const TWikiElecBs& ElecBs) {
  const TStr WikiMain = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.main.7z";
  const TStr WikiMainTalk = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.talk.7z";
  const TStr WikiWiki = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.wikipedia.7z";
  const TStr WikiWikiTalk = "W:\\Data\\wiki20080103-parseByGuerogy\\enwiki-20080103.wikipedia_talk.7z";
  const TStr UsrChanges = "W:\\data\\wiki20080103-parseByGuerogy\\enwiki.important-username-changes.2007-08-06";
  TExeTm ExeTm;
  //// load user stat
  printf("  Loading %s ", WikiMain.GetFMid().CStr());
  { TWikiMetaLoader WML(WikiMain);
  for (int rec=0; WML.Next(); rec++) {
    if (! ElecBs.IsUsr(WML.Usr)) { continue; }
    TEditCnt& EC = RfaEdCntH.AddDat(WML.Usr);
    EC.MainE+=1; EC.MainW+=WML.RevWrds;
    WML.CommentStr.ToLc();
    if (WML.CommentStr.IsPrefix("rv") || WML.CommentStr.SearchStr("revert")!=-1) {
      EC.RevE+=1; EC.RevW+=WML.RevWrds;
    }
  } }
  Save(TZipOut("wikiEditCounts.All.rar"));
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiMainTalk.GetFMid().CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(WikiMainTalk);
  for (int rec=0; WML.Next(); rec++) {
    if (! ElecBs.IsUsr(WML.Usr)) { continue; }
    TEditCnt& EC = RfaEdCntH.AddDat(WML.Usr);
    EC.MainTE+=1; EC.MainTW+=WML.RevWrds;
  } }
  Save(TZipOut("wikiEditCounts.All.rar"));
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiWiki.GetFMid().CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(WikiWiki);
  for (int rec=0; WML.Next(); rec++) {
    TChA U = WML.Usr;
    if (! ElecBs.IsUsr(WML.Usr)) { continue; }
    TEditCnt& EC = RfaEdCntH.AddDat(WML.Usr);
    EC.WikiE+=1; EC.WikiW+=WML.RevWrds;
  } }
  Save(TZipOut("wikiEditCounts.All.rar"));
  printf(" [%s]\n  Loading %s", ExeTm.GetStr(), WikiWikiTalk.GetFMid().CStr());  ExeTm.Tick();
  { TWikiMetaLoader WML(WikiWikiTalk);
  for (int rec=0; WML.Next(); rec++) {
    if (! ElecBs.IsUsr(WML.Usr)) { continue; }
    TEditCnt& EC = RfaEdCntH.AddDat(WML.Usr);
    EC.WikiTE+=1; EC.WikiTW+=WML.RevWrds;
  } }
  Save(TZipOut("wikiEditCounts.All.rar"));
  printf(" [%s]\n", ExeTm.GetStr());
}

void TWikiEditCnt::SaveTxt(const TWikiElecBs& ElecBs, const TStr& OutFNm) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "Rfat\tMainE\tMainW\tMainTE\tMainTW\tWikiE\tWikiW\tWikiTE\tWikiTW\n");
  //for (int e = 0; e < ElecBs.Len(); e++) {
  for (TSsParser Ss("isSuccVote-TalkNet3.tab", ssfTabSep); Ss.Next(); ) {
    //TStr Rfa = ElecBs[e].RfaTitle;
    TStr Rfa = Ss[1];
    if (! RfaEdCntH.IsKey(Rfa)) {
      printf("%s\n", Rfa.CStr());
      fprintf(F, "%s\t0\t0\t0\t0\t0\t0\t0\t0\n", Rfa.CStr());
    } else {
      const TEditCnt& EC = RfaEdCntH.GetDat(Rfa);
      fprintf(F, "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", Rfa.CStr(),
        EC.MainE(), EC.MainW(), EC.MainTE(), EC.MainTW(), EC.WikiE(), EC.WikiW(), EC.WikiTE(), EC.WikiTW());
    }
  }
  fclose(F);
}

/////////////////////////////////////////////////
// Wikipedia Election
TWikiElec::TWikiElec(TSIn& SIn) : RfaTitle(SIn), UsrId(SIn), NomUId(SIn), BurUId(SIn), IsSucc(SIn), ElecTm(SIn), VoteV(SIn) {
}

void TWikiElec::Save(TSOut& SOut) const {
  RfaTitle.Save(SOut); UsrId.Save(SOut); NomUId.Save(SOut);
  BurUId.Save(SOut); IsSucc.Save(SOut); ElecTm.Save(SOut);
  VoteV.Save(SOut);
}

bool TWikiElec::operator < (const TWikiElec& WE) const {
  return ElecTm<WE.ElecTm || (ElecTm==WE.ElecTm && UsrId<WE.UsrId);
}

// determine whether an item is a comment or actual vote
void TWikiElec::SetIsVoteFlag() {
  THash<TInt, TIntPr> UsrVoteH(Len());
  for (int i = 0; i < VoteV.Len(); i++) {
    if (VoteV[i].GetIndent() != 0) { continue; } // only take vote at indent 0
    if (VoteV[i].GetUId() == UsrId) { continue; }  // skip self votes
    if (! UsrVoteH.IsKey(VoteV[i].GetUId())) {
      UsrVoteH.AddDat(VoteV[i].GetUId(), TIntPr(VoteV[i].GetTm().GetAbsSecs(), i));
    } else {
      TIntPr& Dat = UsrVoteH.GetDat(VoteV[i].GetUId());
      if (Dat.Val1 > (int)VoteV[i].GetTm().GetAbsSecs()) { // new vote accured sooner than the previous one
        Dat = TIntPr(VoteV[i].GetTm().GetAbsSecs(), i);
      }
    }
  }
  // set vote flag
  for (int v = 0; v < VoteV.Len(); v++) {
    VoteV[v].IsAVote = false;
  }
  for (int v = 0; v < UsrVoteH.Len(); v++) {
    VoteV[UsrVoteH[v].Val2].IsAVote = true;
  }
}

double TWikiElec::GetFracSup(const bool& OnlyVotes) const {
  const TIntTr Votes = GetVotes(OnlyVotes);
  return double(Votes.Val1)/double(Votes.Val1+Votes.Val3);
}

double TWikiElec::GetFracSup(int VoteId1, int VoteId2) const {
  double Sup=0;
  for (int v = VoteId1; v < TMath::Mn(VoteId2, Len()); v++) {
    if (VoteV[v].GetVote()==1) { Sup+=1; }
  }
  if (VoteId1==VoteId2) { return 0.0; }
  return Sup/double(VoteId2-VoteId1);
}

// fit least squares line between votes Vote1 and Vote2 and return the coefficient
double TWikiElec::GetTrend(int VoteId1, int VoteId2) const {
  TFltPrV XY;
  for (int v = VoteId1; v < VoteId2; v++) {
    XY.Add(TFltPr(v, (double)VoteV[v].GetVote()));
  }
  double A, B, SigA, SigB, Chi2, R2;
  TSpecFunc::LinearFit(XY, A, B, SigA, SigB, Chi2, R2);
  return B;
}

TIntTr TWikiElec::GetVotes(const bool& OnlyVotes) const {
  int Sup=0, Opp=0, Neu=0;
  for (int i = 0; i < VoteV.Len(); i++) {
    if (OnlyVotes && ! VoteV[i].IsVote()) { continue; }
    if (VoteV[i].GetVote()==1) { Sup++; }
    else if (VoteV[i].GetVote()==0) { Neu++; }
    else if (VoteV[i].GetVote()==-1) { Opp++; }
  }
  return TIntTr(Sup, Neu, Opp);
}

void TWikiElec::GetVotesOt(TWikiVoteV& WVoteV, const bool& OnlyVotes) const {
  // also counts opinions, changes of opinion, etc
  if (! OnlyVotes) {
    WVoteV = VoteV;
    WVoteV.Sort();
    return;
  }
  // take the earliest vote of a user
  TIntPrV TmIdV;
  for (int i = 0; i < VoteV.Len(); i++) {
    if (OnlyVotes && ! VoteV[i].IsVote()) { continue; }
    TmIdV.Add(TIntPr(VoteV[i].VoteTm.GetAbsSecs(), i));
  }
  TmIdV.Sort();
  WVoteV.Clr(false);
  for (int v = 0; v < TmIdV.Len(); v++) {
    WVoteV.Add(VoteV[TmIdV[v].Val2]);
  }
  WVoteV.Sort();
}

// get average vote (fraction of positive votes) over time
int TWikiElec::GetAvgVoteOt(TFltV& AvgVoteV, const bool& OnlyVotes) const {
  TWikiVoteV WVoteV;  GetVotesOt(WVoteV, OnlyVotes);
  double VoteSum = 0;
  AvgVoteV.Clr(false);
  for (int i = 0; i < WVoteV.Len(); i++) {
    VoteSum += WVoteV[i].GetVote() > 0 ? 1 : 0;
    AvgVoteV.Add(VoteSum/double(i+1));
  }
  return AvgVoteV.Len();
}

// get absolute change in the average vote (fraction of positive votes) over time
int TWikiElec::GetAvgVoteDevOt(TFltV& AvgVoteV, const bool& OnlyVotes) const {
  TWikiVoteV WVoteV;  GetVotesOt(WVoteV, OnlyVotes);
  double XBar = WVoteV[0].GetVote() > 0 ? 1 : 0;
  AvgVoteV.Clr(false);
  for (int i = 1; i < WVoteV.Len(); i++) {
    const double CurVote = WVoteV[i].GetVote() > 0 ? 1 : 0;
    AvgVoteV.Add(fabs(CurVote-XBar)/double(i+1));
    XBar = (i*XBar+CurVote)/double(i+1);
  }
  return AvgVoteV.Len();
}

// after the v-th vote was casted, how many people voted the same
int TWikiElec::GetRunLen(const int& VoteId) const {
  const int V = VoteV[VoteId].GetVote();
  int runL=0;
  for (int v = VoteId+1; v < Len(); v++) {
    if (VoteV[v].GetVote() == V) { runL++; }
    else { return runL; }
  }
  return runL;
}

// shuffle the order of user votes over time (users, times remain unaltered, but votes change)
void TWikiElec::PermuteVotes() {
  TIntV ValV(VoteV.Len(), 0);
  for (int i = 0; i < VoteV.Len(); i++) {
    ValV.Add(VoteV[i].UsrVote); }
  ValV.Shuffle(TInt::Rnd);
  for (int i = 0; i < ValV.Len(); i++) {
    VoteV[i].UsrVote=ValV[i]; }
}

void TWikiElec::KeepVotes(const TIntSet& UIdSet) {
  TWikiVoteV NewVoteV;
  for (int v = 0; v < VoteV.Len(); v++) {
    if (UIdSet.IsKey(VoteV[v].GetUId())) {
      NewVoteV.Add(VoteV[v]);
    }
  }
  VoteV=NewVoteV;
}

void TWikiElec::GetOnlyVotes(TWikiElec& NewElec, const bool& OnlySupOpp) const {
  NewElec.RfaTitle = RfaTitle;
  NewElec.UsrId = UsrId;
  NewElec.NomUId = NomUId;
  NewElec.BurUId = BurUId;
  NewElec.IsSucc = IsSucc;
  NewElec.ElecTm = ElecTm;
  NewElec.VoteV.Clr(false);
  for (int i = 0; i < VoteV.Len(); i++) {
    if (! VoteV[i].IsVote()) { continue; }
    if (OnlySupOpp && VoteV[i].GetVote()==0) { continue; }
    NewElec.VoteV.Add(VoteV[i]);
  }
}

void TWikiElec::RemoveSelfVotes() {
  TWikiVoteV VoteV2(Len(), 0);
  for (int v = 0; v < VoteV.Len(); v++) {
    if (VoteV[v].GetIndent()==0 && VoteV[v].GetUId()==UsrId) { continue; } // skip self votes
    VoteV2.Add(VoteV[v]);
  }
  VoteV.Swap(VoteV2);
}

void TWikiElec::Dump(const TStrHash<TInt>& UsrH) const {
  TIntTr V = GetVotes(true);
  if (UsrH.IsKeyId(UsrId)) {
    printf("\nELEC %s : %s %d vote at %s votes %d\n", RfaTitle.CStr(), UsrH.GetKey(UsrId), UsrId, ElecTm.GetYmdTmStr().CStr(), VoteV.Len()); }
  else {
     printf("\nELEC %s : %d  %d vote at %s votes %d\n", RfaTitle.CStr(), UsrId, ElecTm.GetYmdTmStr().CStr(), VoteV.Len()); }
  printf("    %d / %d / %d  =  %f\n", V.Val1, V.Val2, V.Val3, V.Val1/double(V.Val1+V.Val3));
  for (int v = 0; v < VoteV.Len(); v++) {
    if (UsrH.IsKeyId(VoteV[v].UsrId)) {
      printf("  %2d  %2d %s: %s (%d)  %s  t:%d  i: %d\n", v+1, VoteV[v].UsrVote, VoteV[v].IsVote()?"V":" ", UsrH.GetKey(VoteV[v].UsrId), VoteV[v].UsrId, VoteV[v].VoteTm.GetYmdTmStr().CStr(), VoteV[v].TxtLen, VoteV[v].UsrIndent);
    } else {
      printf("  %2d  %2d %s: %d  %s  t:%d  i: %d\n", v+1, VoteV[v].UsrVote, VoteV[v].IsVote()?"V":" ", VoteV[v].UsrId, VoteV[v].VoteTm.GetYmdTmStr().CStr(), VoteV[v].TxtLen, VoteV[v].UsrIndent);
    }
  }
}

/////////////////////////////////////////////////
// Wikipedia Election Base
void TWikiElecBs::GetEIdByVotes(TIntV& EIdV, const bool& AscNumVotes) const {
  TIntPrV V(Len(), 0);
  for (int u = 0; u < Len(); u++) {
    const TWikiElec& E = GetElec(u);
    V.Add(TIntPr(E.Len(), E.UsrId));
  }
  V.Sort(AscNumVotes);
  EIdV.Clr(false);
  for (int u = 0; u < V.Len(); u++) {
    EIdV.Add(V[u].Val2); }
}

void TWikiElecBs::GetEIdByVotes(TIntV& EIdV, const int& MinLen, const double& FracPos, const double AboveFrac, const bool& AscNumVotes) const {
  TFltIntPrV V(Len(), 0);
  for (int u = 0; u < Len(); u++) {
    const TWikiElec& E = GetElec(u);
    if (E.Len() < MinLen) { continue; }
    if ((AboveFrac && E.GetFracSup() > FracPos) || (!AboveFrac && E.GetFracSup() < FracPos)) {
      V.Add(TFltIntPr(E.Len(), E.UsrId)); }
  }
  V.Sort(AscNumVotes);
  EIdV.Clr(false);
  for (int u = 0; u < V.Len(); u++) {
    EIdV.Add(V[u].Val2); }
}

// get elections with final fraction and minimum number of votes
void TWikiElecBs::GetEIdByFrac(TIntV& EIdV, const int& MinLen, const double& MnFracSup, const double& MxFracSup) const {
  TFltIntPrV V(Len(), 0);
  for (int u = 0; u < Len(); u++) {
    const TWikiElec& E = GetElec(u);
    if (E.Len() < MinLen) { continue; }
    const double FracSup = E.GetFracSup();
    if (FracSup==1) { continue; }
    if (FracSup >= MnFracSup && FracSup <= MxFracSup) {
      V.Add(TFltIntPr(FracSup, E.UsrId)); }
  }
  V.Sort(true);
  EIdV.Clr(false);
  for (int u = 0; u < V.Len(); u++) {
    EIdV.Add(V[u].Val2); }
}

void TWikiElecBs::GetUsrV(TIntV& UIdV) const {
  TIntSet UIdSet;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    UIdSet.AddKey(E.GetUId());
    for (int v = 0; v < E.Len(); v++) {
      UIdSet.AddKey(E.GetVote(v).GetUId());
    }
  }
  UIdSet.GetKeyV(UIdV);
}

// users that went up for election
void TWikiElecBs::GetElecUsrV(TIntV& ElecUsrV) const {
  ElecUsrV.Clr(false);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    ElecUsrV.Add(E.UsrId);
  }
}

// users that got elected (promoted into adminship)
void TWikiElecBs::GetElecAdminUsrV(TIntV& ElecAdminUsrV) const {
  ElecAdminUsrV.Clr(false);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.IsSucc) {
      ElecAdminUsrV.Add(E.UsrId);
    }
  }
}

// users that did not get promoted
void TWikiElecBs::GetElecNonAdminUsrV(TIntV& ElecNonAdminUsrV) const {
  ElecNonAdminUsrV.Clr(false);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (! E.IsSucc) {
      ElecNonAdminUsrV.Add(E.UsrId);
    }
  }
}

// get users that are frequent votes
// MinVotes=25 gives 1k users
// MinVotes=50 gives 500 users
// MinVotes=100 gives 241 users
void TWikiElecBs::GetFqVoters(TIntSet& FqVoterSet, const int& MinVotes, const int& MinElecLen, const bool& OnlyAdmins) const {
  TIntH UsrCntH, CntH;
  printf("%d\n", Len());
  TIntSet AdminSet;
  if (OnlyAdmins) {
    GetAdminSet(AdminSet);
  }
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinElecLen) { continue; }
    for (int v = 0; v < E.Len(); v++) {
      if (OnlyAdmins && ! AdminSet.IsKey(E[v].GetUId())) {
        continue; } // skip non admins
      UsrCntH.AddDat(E[v].GetUId()) += 1;
    }
  }
  for (int i = 0; i < UsrCntH.Len(); i++) {
    CntH.AddDat(UsrCntH[i]) += 1; }
  //TGnuPlot::PlotValCntH(CntH, "votesPerUser-wiki", "", "votes per user (elections user participated in)", "count");
  FqVoterSet.Clr(false);
  UsrCntH.SortByDat(false);
  for (int i = 0; i < UsrCntH.Len(); i++) {
    if (UsrCntH[i] >= MinVotes) {
      FqVoterSet.AddKey(UsrCntH.GetKey(i));
    }
  }
}

void TWikiElecBs::GetUsrVotes(TIntPrV& VoteUIdV) const {
  TIntH UsrCntH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    for (int v = 0; v < E.Len(); v++) {
      UsrCntH.AddDat(E[v].GetUId()) += 1;
    }
  }
  VoteUIdV.Clr();
  for (int i = 0; i < UsrCntH.Len(); i++) {
    VoteUIdV.Add(TIntPr(UsrCntH[i], UsrCntH.GetKey(i)));
  }
  VoteUIdV.Sort(false);
}

void TWikiElecBs::GetAdminSet(TIntSet& AdminSet) const {
  AdminSet.Clr(false);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.IsSucc) {
      AdminSet.AddKey(E.UsrId);
    }
  }
  printf("  %d admins from elecs. ", AdminSet.Len());
  if (TFile::Exists("../../admin-list.txt")) {
    for (TSsParser Ss("../../admin-list.txt", ssfTabSep); Ss.Next(); ) {
      TStr U = Ss[0];
      if (IsUsr(U)) { AdminSet.AddKey(GetUId(U)); }
      if(IsUsr(U.GetLc())) { AdminSet.AddKey(GetUId(U.GetLc())); }
    }
  }
  printf("  %d admins after admin-list.txt\n", AdminSet.Len());
}

void TWikiElecBs::GetFqVoterSet(TIntSet& FqVoterSet) const {
  TIntPrV VoteUIdV;
  GetUsrVotes(VoteUIdV);
  int HalfVotes = GetVotes()/2, SoFar=0;
  FqVoterSet.Clr(false);
  for (int i = 0; i < VoteUIdV.Len() && SoFar < HalfVotes; i++) {
    FqVoterSet.AddKey(VoteUIdV[i].Val2);
    SoFar+=VoteUIdV[i].Val1;
  }
  printf("Users:%d (%d votes) FqVoters:%d (%d votes)\n", VoteUIdV.Len(), HalfVotes*2, FqVoterSet.Len(), SoFar);
}

void TWikiElecBs::GetAdminTmSet(THash<TInt, TSecTm>& AdminSet) const {
  AdminSet.Clr(false);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.IsSucc) {
      AdminSet.AddDat(E.UsrId, E.GetTm());
    }
  }
}

void TWikiElecBs::KeepFqVoters(const int& MinVotes, const int& MinElecLen, const bool& OnlyAdmins) {
  TIntSet Voters;
  GetFqVoters(Voters, MinVotes, MinElecLen, OnlyAdmins);
  printf("freq  voters %d\n", Voters.Len());
  int VotesB=0, VotesA=0;
  for (int e = 0; e < Len(); e++) {
    TWikiElec& E = GetElec(e);
    VotesB += E.Len();
    if (E.Len() < MinElecLen) { E.VoteV.Clr(); }
    //printf("  %d --> ", E.Len());
    E.KeepVotes(Voters);
    //printf("%d\n", E.Len());
    VotesA += E.Len();
  }
  printf("Votes: %d --> %d\n", VotesB, VotesA);
}

void TWikiElecBs::KeepVoters(const bool& KeepAdmins, const bool& KeepNonAdmins) {
  TIntSet KeepVoters, Admins;
  GetAdminSet(Admins);
  if (KeepAdmins) {
    KeepVoters = Admins; }
  if (KeepNonAdmins) {
    TIntV UIdV; GetUsrV(UIdV);
    for (int i = 0; i < UIdV.Len(); i++) {
      if (! Admins.IsKey(UIdV[i])) {
        KeepVoters.AddKey(UIdV[i]); } }
  }
  int VotesB=0, VotesA=0;
  printf("voters %d\n", KeepVoters.Len());
  for (int e = 0; e < Len(); e++) {
    TWikiElec& E = GetElec(e);
    VotesB += E.Len();
    E.KeepVotes(KeepVoters);
    VotesA += E.Len();
  }
  printf("  votes: %d --> %d\n", VotesB, VotesA);
}

void TWikiElecBs::KeepTopVoters(const int& Votes, const bool& KeepTop) {

}



void TWikiElecBs::PermuteVotes() {
  printf("Permute election entries...");
  for (int u = 0; u < Len(); u++) {
    //for (int e = 0; e < UsrElecH[u].Len(); e++) { UsrElecH[u][e].PermuteVotes(OnlyVotes);  }
    GetElec(u).PermuteVotes();
  }
  printf("done.\n");
}

void TWikiElecBs::SortVotesByTm() {
  for (int e = 0; e < Len(); e++) {
    GetElec(e).VoteV.Sort();
  }
}

// UIdV            : user ids of users we consider
// ProbSupTmV      : Probability of user voting support vs. time of user's vote (index in the election)
// FracSupTmV      : Fraction of support voters in the election so far vs. time of user's vote
// ProbSupFracSupV : Probability of user voting support vs. current fraction of support votes in election
// VotesTmV        : Number of votes/elections user casted vs. time of user's vote
int TWikiElecBs::GetVoteTrails(const int& MinUsrVotes, const bool& No01Prob, TIntV& UIdV, TVec<TFltPrV>& ProbSupTmV,
                                TVec<TFltPrV>& FracSupTmV, TVec<TFltPrV>& ProbSupFracSupV, TVec<TFltPrV>& VotesTmV) const {
  UIdV.Clr();  ProbSupTmV.Clr();  FracSupTmV.Clr();
  ProbSupFracSupV.Clr();  VotesTmV.Clr();
  THash<TInt, TIntPrV> UIdVotesH;
  TIntFltH FracSupH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10) { continue; } // skip if election has less than 10 votes
    for (int v = 0; v < E.Len(); v++) {
      UIdVotesH.AddDat(E[v].GetUId()).Add(TIntPr(e,v));
    }
    FracSupH.AddDat(e, E.GetFracSup());
  }
  int Votes = 0;
  UIdVotesH.SortByDat(false);
  TIntH AllCntH, AllCntHS, AllCntHF;
  for (int u = 0; u < UIdVotesH.Len(); u++) {
    THash<TInt, TMom> TProbH, TFracH, UFPH;
    THash<TFlt, TFlt> TVotesH;
    TIntPrV& EV = UIdVotesH[u];
    EV.Sort();
    const int UId = UIdVotesH.GetKey(u);
    if (EV.Len() < MinUsrVotes) { continue; }
    //// h2: delete first half of elections
    //EV.Del(0, EV.Len()/2);
    //// h1: delete last half of elections
    EV.Del(EV.Len()/2, EV.Len()-1);
    for (int ev = 0; ev < EV.Len(); ev++) { // for all user's elections
      const TWikiElec& E = GetElec(EV[ev].Val1);
      const int v = EV[ev].Val2;
      if (v < 10) { continue; } // skip first 10 votes
      const int Vote = E[v].GetVote()==1 ? 1:0;
      const double Frac = E.GetFracSup(0, v); //!!!
      if (v < 110) { // vote index
        TProbH.AddDat(10*(v/10)).Add(Vote);
        TFracH.AddDat(10*(v/10)).Add(Frac);
        TVotesH.AddDat(10*(v/10)) += 1;
      }
      //UFPH.AddDat(10*int(10*Frac)).Add(Vote);
      UFPH.AddDat(10*(int)TMath::Round(10*Frac)).Add(Vote);
      Votes++;
    }
    double M;
    TFltPrV TPV, TFV, TFPV, TVV;
    for (int i = 0; i < TProbH.Len(); i++) {
      TProbH[i].Def();   M = TProbH[i].GetMean();
      if (No01Prob && M!=0 && M!=1) { TPV.Add(TFltPr(TProbH.GetKey(i).Val, M)); }
      else if (! No01Prob) { TPV.Add(TFltPr(TProbH.GetKey(i).Val, M)); }
      //if(TProbH[i].GetVals()>2) { TPV.Add(TFltPr(TProbH.GetKey(i).Val, M)); }
    }
    for (int i = 0; i < TFracH.Len(); i++) {
      TFracH[i].Def();   M = TFracH[i].GetMean();
      if (No01Prob && M!=0 && M!=1) { TFV.Add(TFltPr(TFracH.GetKey(i).Val, M)); }
      else if (! No01Prob) { TFV.Add(TFltPr(TFracH.GetKey(i).Val, M)); }
      //if(TFracH[i].GetVals()>2) { TFV.Add(TFltPr(TFracH.GetKey(i).Val, M)); }

    }
    for (int i = 0; i < UFPH.Len(); i++) {
      UFPH[i].Def();   M = UFPH[i].GetMean();
      if (No01Prob && M!=0 && M!=1) { TFPV.Add(TFltPr(UFPH.GetKey(i).Val, M)); }
      else if (! No01Prob) { TFPV.Add(TFltPr(UFPH.GetKey(i).Val, M)); }
      //if(UFPH[i].GetVals()>2) { TFPV.Add(TFltPr(UFPH.GetKey(i).Val, M)); }
    }
    TVotesH.GetKeyDatPrV(TVV);  TVV.Sort();
    TFPV.Sort(); TPV.Sort(); TFV.Sort();
    if (TFPV[0].Val1!=0.0) { TFPV.Ins(0, TFltPr(0,0)); printf("Z");} // padd with zeros
    if (TFPV.Last().Val1!=100.0) { TFPV.Add(TFltPr(100,1)); printf("O");} // padd with ones
    // add data
    UIdV.Add(UId);
    ProbSupTmV.Add(TPV);
    FracSupTmV.Add(TFV);
    ProbSupFracSupV.Add(TFPV);
    VotesTmV.Add(TVV);
  }
  IAssert(UIdV.Len() == ProbSupTmV.Len());
  return Votes;
}

void TWikiElecBs::GetVoteTrails2(const int& MinUsrVotes, const bool& No01Prob, TIntV& UIdV, TVec<TFltPrV>& VoteIdxFracSupV, TVec<TFltPrV>& NVotesFracSupV) const {
  UIdV.Clr();  VoteIdxFracSupV.Clr();  NVotesFracSupV.Clr();
  THash<TInt, TIntPrV> UIdVotesH;
  TIntFltH FracSupH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10) { continue; } // skip if election has less than 10 votes
    for (int v = 0; v < E.Len(); v++) {
      UIdVotesH.AddDat(E[v].GetUId()).Add(TIntPr(e,v));
    }
    FracSupH.AddDat(e, E.GetFracSup());
  }
  UIdVotesH.SortByDat(false);
  TIntH AllCntH, AllCntHS, AllCntHF;
  for (int u = 0; u < UIdVotesH.Len(); u++) {
    THash<TInt, TMom> FSVIdxH;
    TFltFltH FSVotesH;
    const TIntPrV& EV = UIdVotesH[u];
    const int UId = UIdVotesH.GetKey(u);
    if (EV.Len() < MinUsrVotes) { continue; }
    for (int ev = 0; ev < EV.Len(); ev++) { // for all user's elections
      const TWikiElec& E = GetElec(EV[ev].Val1);
      const int v = EV[ev].Val2;
      //if (v < 10) { continue; }
      const int Vote = E[v].GetVote()==1 ? 1:0;
      const double Frac = E.GetFracSup(0, v); //!!!
      FSVotesH.AddDat(10*int(10*Frac)) += 1;
      FSVIdxH.AddDat(10*int(10*Frac)).Add(v);
    }
    // add data
    UIdV.Add(UId);
    double M = 0;
    VoteIdxFracSupV.Add();
    TFltPrV& VoteIdxV = VoteIdxFracSupV.Last();
    for (int i = 0; i < FSVIdxH.Len(); i++) {
      FSVIdxH[i].Def();   M = FSVIdxH[i].GetMean();
      if (No01Prob && M!=0 && M!=1) { VoteIdxV.Add(TFltPr(FSVIdxH.GetKey(i).Val, M)); }
      else if (! No01Prob) { VoteIdxV.Add(TFltPr(FSVIdxH.GetKey(i).Val, M)); }
    }
    VoteIdxV.Sort();
    NVotesFracSupV.Add();
    FSVotesH.GetKeyDatPrV(NVotesFracSupV.Last());
    NVotesFracSupV.Last().Sort();
  }
}

void TWikiElecBs::GetUsrVoteTrail(const TIntV& UIdV, TVec<TFltPrV>& ProbPosFracPosV) const {
  ProbPosFracPosV.Clr();
  THash<TInt, TIntPrV> UIdVotesH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10) { continue; } // skip if election has less than 10 votes
    for (int v = 0; v < E.Len(); v++) {
      UIdVotesH.AddDat(E[v].GetUId()).Add(TIntPr(e,v));
    }
  }
  for (int u = 0; u < UIdV.Len(); u++) {
    ProbPosFracPosV.Add();
    if (! UIdVotesH.IsKey(UIdV[u])) { continue; }
    const TIntPrV& EV = UIdVotesH.GetDat(UIdV[u]);
    THash<TInt, TMom> FracProbH;
    for (int ev = 0; ev < EV.Len(); ev++) { // for all user's elections
      const TWikiElec& E = GetElec(EV[ev].Val1);
      const int v = EV[ev].Val2;
      //if (v < 10) { continue; }
      const int Vote = E[v].GetVote()==1 ? 1:0;
      const double Frac = E.GetFracSup(0, v); //!!!
      FracProbH.AddDat(10*int(10*Frac)).Add(Vote);
    }
    FracProbH.SortByKey();
    TFltPrV& ProbFracV = ProbPosFracPosV.Last();
    for (int i = 0; i < FracProbH.Len(); i++) {
      FracProbH[i].Def();
      const double M = FracProbH[i].GetMean();
      ProbFracV.Add(TFltPr(FracProbH.GetKey(i).Val, M));
    }
    ProbFracV.Sort();
  }
}

void TWikiElecBs::GetUsrAreaUTrail(const TIntV& UIdV, TFltV& AreaV) const {
  TVec<TFltPrV> ProbPosFracPosV;
  GetUsrVoteTrail(UIdV, ProbPosFracPosV);
  AreaV.Clr();
  for (int u = 0; u < ProbPosFracPosV.Len(); u++) {
    double Area = 0;
    TFltPrV& V = ProbPosFracPosV[u];
    for (int f = 0; f < V.Len(); f++) {
      IAssert(V[f].Val1>= 0 && V[f].Val1<=100);
      Area += (V[f].Val2 - V[f].Val1/100.0);
    }
    AreaV.Add(Area);
  }
}

// plot vote statistics: lengths
void TWikiElecBs::PlotElecLenDistr(const TStr& OutFNm) const {
  TIntH SupCntHS, OppCntHS, VotesCntHS, // succ elecs
        SupCntHF, OppCntHF, VotesCntHF, // fail elecs
        SupCntHA, OppCntHA, VotesCntHA; // all elecs
  TIntH VotesPerUser;
  THashSet<TFltPr> SupOppHS, SupOppHF;
  THash<TInt, TMom> ElLenVsSucc;
  TIntH SupFracA, SupFracS, SupFracF;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    const int SupFrac = (int)TMath::Round(E.GetFracSup()*10)*10;
    TIntTr V = E.GetVotes();
    int len = (E.Len()/5)*5;
    V.Val1 = (V.Val1/5)*5;
    V.Val2 = (V.Val2/5)*5;
    V.Val3 = (V.Val3/5)*5;
    SupFracA.AddDat(SupFrac) +=1;
    if (E.IsSucc) {
      SupFracS.AddDat(SupFrac) += 1;
      VotesCntHS.AddDat(len) += 1;
      SupCntHS.AddDat(V.Val1) += 1;
      OppCntHS.AddDat(V.Val3) += 1;
      SupOppHS.AddKey(TFltPr(V.Val1+0.1+TInt::Rnd.GetNrmDev(), V.Val3+0.1+TInt::Rnd.GetNrmDev()));
      ElLenVsSucc.AddDat(len).Add(1);
    } else {
      SupFracF.AddDat(SupFrac) += 1;
      VotesCntHF.AddDat(len) += 1;
      SupCntHF.AddDat(V.Val1) += 1;
      OppCntHF.AddDat(V.Val3) += 1;
      SupOppHF.AddKey(TFltPr(V.Val1+0.1+TInt::Rnd.GetNrmDev(), V.Val3+0.1+TInt::Rnd.GetNrmDev()));
      ElLenVsSucc.AddDat(len).Add(0);
    }
    VotesCntHA.AddDat(len) += 1;
    SupCntHA.AddDat(V.Val1) += 1;
    OppCntHA.AddDat(V.Val3) += 1;

  }
  TIntPrV VotesUIdV; GetUsrVotes(VotesUIdV);
  for (int i = 0; i < VotesUIdV.Len(); i++) {
    VotesPerUser.AddDat(10*(VotesUIdV[i].Val1/10))+=1;
  }
  TGnuPlot::PlotValCntH(VotesPerUser, "userVotes-"+OutFNm, "", "Number of votes", "Number of such users");

  TGnuPlot::PlotValMomH(ElLenVsSucc, "elLenSucc-"+OutFNm, "", "Election length", "Probability of success");
  { TGnuPlot GP("suppOppScatter-"+OutFNm); GP.SetXYLabel("Support votes", "Oppose votes");
  TFltPrV SuccV, FailV;  SupOppHS.GetKeyV(SuccV);  SupOppHF.GetKeyV(FailV);
  GP.AddPlot(FailV, gpwPoints, "FAIL");  GP.AddPlot(SuccV, gpwPoints, "SUCC");
  GP.SavePng(); }
  { TGnuPlot GP("countSup-"+OutFNm); GP.SetXYLabel("Support votes", "Count");
  GP.AddPlot(SupCntHA, gpwLinesPoints, "ALL elections");
  GP.AddPlot(SupCntHS, gpwLinesPoints, "SUCC elections");
  GP.AddPlot(SupCntHF, gpwLinesPoints, "FAIL elections");
  GP.SavePng(); }
  { TGnuPlot GP("countOpp-"+OutFNm); GP.SetXYLabel("Oppose votes", "Count");
  GP.AddPlot(OppCntHA, gpwLinesPoints, "ALL elections");
  GP.AddPlot(OppCntHS, gpwLinesPoints, "SUCC elections");
  GP.AddPlot(OppCntHF, gpwLinesPoints, "FAIL elections");
  GP.SavePng(); }
  { TGnuPlot GP("countVot-"+OutFNm); GP.SetXYLabel("Votes", "Count");
  GP.AddPlot(VotesCntHA, gpwLinesPoints, "ALL elections");
  GP.AddPlot(VotesCntHS, gpwLinesPoints, "SUCC elections");
  GP.AddPlot(VotesCntHF, gpwLinesPoints, "FAIL elections");
  GP.SavePng(); }
  { TGnuPlot GP("supFrac-"+OutFNm); GP.SetXYLabel("Final Fraction of support votes", "Number of such elections");
  GP.AddPlot(SupFracA, gpwLinesPoints, "ALL elections");
  GP.AddPlot(SupFracS, gpwLinesPoints, "SUCC elections");
  GP.AddPlot(SupFracF, gpwLinesPoints, "FAIL elections");
  GP.SavePng(); }
}

// plot election support-oppose votes over time
void TWikiElecBs::PlotElecSupOppOt(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const {
  int ElecCnt=0;
  TGnuPlot GP("supOppOT-"+OutFNm);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinVotes || E.Len() > MaxVotes) { continue; }
    TFltPrV OppSupV; OppSupV.Add(TFltPr(0,0));
    for (int v = 0; v < E.Len(); v++) {
      const int S = E[v].GetVote();
      OppSupV.Add(TFltPr(OppSupV.Last().Val1+(S<0?1:0)+0.1*TInt::Rnd.GetNrmDev(),
                         OppSupV.Last().Val2+(S>0?1:0)+0.1*TInt::Rnd.GetNrmDev()));
    }
    //for (int i = 0; i < OppSupV.Len(); i++) {
    //  OppSupV[i].Val1 /= OppSupV.Last().Val1;
    //  OppSupV[i].Val2 /= OppSupV.Last().Val2; }
    GP.AddPlot(OppSupV, gpwLines, "", TStr::Fmt("lt %d", E.IsSucc?2:1));
    ElecCnt++;
  }
  //GP.SetXRange(0,1);  GP.SetYRange(0,1);
  GP.SetXYLabel("oppose votes", "support votes");
  GP.SetTitle(TStr::Fmt("elections with %d -- %d votes: %d elections", MinVotes, MaxVotes, ElecCnt));
  GP.SavePng();
}

// plot user vote run length statistics
void TWikiElecBs::PlotRunLenStat(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const {
  THash<TInt, TTriple<TMom, TMom, TMom> > UsrMomH;
  THash<TInt, TMom> TxtLenH;
  TPair<TMom, TMom> RunLStat;
  THash<TInt, TInt> SupH, OppH;
  int Cnt=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinVotes || E.Len() > MaxVotes) { continue; }
    for (int v = 0; v < E.Len(); v++) {
      const int V = E.GetVote(v).GetVote();
      const int RunL = E.GetRunLen(v);
      if (V == 1) {
        UsrMomH.AddDat(E.GetVote(v).GetUId()).Val1.Add(RunL);
        RunLStat.Val1.Add(RunL);
        SupH.AddDat(RunL)++;
      } else {
        UsrMomH.AddDat(E.GetVote(v).GetUId()).Val2.Add(RunL);
        RunLStat.Val2.Add(RunL);
        OppH.AddDat(RunL)++;
      }
      UsrMomH.AddDat(E.GetVote(v).GetUId()).Val3.Add(E[v].GetTxtLen());
      TxtLenH.AddDat(10*(E[v].GetTxtLen()/10)).Add(RunL);
    }
    Cnt++;
  }
  TGnuPlot::PlotValMomH(TxtLenH, "runLenTxtLen-"+OutFNm, "", "Length of the text supporting the vote", "Run length",
    gpsAuto, gpwLinesPoints, true, true, false, false, false);
  RunLStat.Val1.Def();
  RunLStat.Val2.Def();
  printf("SUP run len: A:%g  M:%g\n", RunLStat.Val1.GetMean(), RunLStat.Val1.GetMedian());
  printf("OPP run len: A:%g  M:%g\n", RunLStat.Val2.GetMean(), RunLStat.Val2.GetMedian());
  TGnuPlot GP("runLen-"+OutFNm, TStr::Fmt("Elecs %d--%d: %d. SUP run len: A:%g  M:%g  OPP: A:%g  M:%g", MinVotes, MaxVotes, Cnt,
    RunLStat.Val1.GetMean(), RunLStat.Val1.GetMedian(), RunLStat.Val2.GetMean(), RunLStat.Val2.GetMedian()));
  GP.AddPlot(OppH, gpwLinesPoints, "OPP");
  GP.AddPlot(SupH, gpwLinesPoints, "SUP");
  GP.SetXYLabel("run length", "count");
  GP.SavePng();
  // save user stat
  FILE *F = fopen(TStr::Fmt("runLenUsers-%s.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#UsrId\tUser\tVotes\tSup-Votes\tOpp-Votes\tSup-Avg\tSup-Med\tOpp-Avg\tOpp-Med\tTxtLen-Avg\tTxtLen-Med\n");
  for (int u = 0; u < UsrMomH.Len(); u++) {
    TMom& MS = UsrMomH[u].Val1;  MS.Def();
    TMom& MO = UsrMomH[u].Val2;  MO.Def();
    TMom& TL = UsrMomH[u].Val3;  TL.Def();
    fprintf(F, "%d\t%s\t%d\t%d\t%d\t%g\t%g\t%g\t%g\t%g\t%g\n", UsrMomH.GetKey(u), GetUsr(UsrMomH.GetKey(u)),
      MS.GetVals()+MO.GetVals(), MS.GetVals(), MO.GetVals(), MS.GetMean(), MS.GetMedian(), MO.GetMean(), MO.GetMedian(),
      TL.GetMean(), TL.GetMedian());
  }
  fclose(F);
}

// draw election tree
void TWikiElecBs::DrawElecTree(const TStr& OutFNm, const int& MinVotes) const {
  TIntV UIdV;  GetEIdByVotes(UIdV);
  TWikiVoteV VoteV;
  THash<TInt, TIntQu> TreeH;  // NId
  const int TakeVotes = 3;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinVotes) { continue; }
    TInt BegVote=1;
    const TIntTr SNO = E.GetVotes();
    { TIntQu& V = TreeH.AddDat(BegVote);
    V.Val1 += SNO.Val1; V.Val2 += SNO.Val3;  V.Val3 += 1;
    V.Val4 += E.IsSucc() ? 1:0; }
    for (int v = 0; v < TakeVotes; v++) {
      if (E[v].GetVote() == 1) { BegVote = 2*BegVote+1; } // sup
      else { BegVote = 2*BegVote; } // opp
      TIntQu& V = TreeH.AddDat(BegVote);
      V.Val1 += SNO.Val1;
      V.Val2 += SNO.Val3;
      V.Val3 += 1;
      V.Val4 += E.IsSucc() ? 1:0;
    }
  }
  PNGraph G = TNGraph::New();
  TIntStrH LabelH;
  TreeH.SortByKey();
  for (int i = 0; i < TreeH.Len(); i++) {
    printf("%d\n", TreeH.GetKey(i));
    const int id = TreeH.GetKey(i);
    const int nid = G->AddNode(i+1);
    //LabelH.AddDat(nid, TStr::Fmt("%s\\nElections:%d\\nS/O: %d/%d\\nSup: %.4f\\nSucc:%.4f", id==1?"":(id%2==1 ?"SUPPORT":"OPPOSE"),
    //  TreeH[i].Val3, TreeH[i].Val1, TreeH[i].Val2, TreeH[i].Val1/double(TreeH[i].Val1+TreeH[i].Val2), TreeH[i].Val4/double(TreeH[i].Val3)));
    LabelH.AddDat(nid, TStr::Fmt("%sElections: %d\\nSuccessful: %.3f", id==1?"":(id%2==1 ?"SUPPORT\\n":"OPPOSE\\n"), TreeH[i].Val3, TreeH[i].Val4/double(TreeH[i].Val3)));
    if (id > 1) {
      G->AddEdge(id/2, nid);
      printf("link  %d <-- %d\n", id/2, nid);
    }
  }
  TGraphViz::Plot(G, gvlDot, "elecTree-"+OutFNm+".gif", "", LabelH);
}

// plot how the statistics of the election as the election goes on
void TWikiElecBs::PlotVotesOt(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const {
  THash<TInt, TMom> FSupOtA, PSupOtA, DevOtA,
    FSupOtS, PSupOtS, DevOtS, FSupOtF, PSupOtF, DevOtF;
  int Cnt=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinVotes || E.Len() > MaxVotes) { continue; }
    int S = 0;
    double Dev = 0;
    for (int v = 0; v < E.Len(); v++) {
      const int V = E.GetVote(v).GetVote()==1? 1:0;
      if (v > 0) {
        Dev = fabs(V - S/double(v))/double(v+1); }
      S += V; // support votes so far
      if (E.IsSucc) {
        FSupOtS.AddDat(v+1).Add(S/double(v+1));
        PSupOtS.AddDat(v+1).Add(V);
        DevOtS.AddDat(v+1).Add(Dev);
      } else {
        FSupOtF.AddDat(v+1).Add(S/double(v+1));
        PSupOtF.AddDat(v+1).Add(V);
        DevOtF.AddDat(v+1).Add(Dev);
      }
      FSupOtA.AddDat(v+1).Add(S/double(v+1));
      PSupOtA.AddDat(v+1).Add(V);
      DevOtA.AddDat(v+1).Add(Dev);
    }
    Cnt++;
  }
  { TGnuPlot GP("votesOT-Frac-"+OutFNm, TStr::Fmt("Elections %d--%d: %d", MinVotes, MaxVotes, Cnt));
  GP.SetXYLabel("Time (vote index)", "Fraction of support votes so far");
  GP.AddPlot(FSupOtA, gpwLinesPoints, "ALL elections", "", true, false);
  GP.AddPlot(FSupOtS, gpwLinesPoints, "SUCC elections", "", true, false);
  GP.AddPlot(FSupOtF, gpwLinesPoints, "FAIL elections", "", true, false);
  if (MinVotes>10) { GP.SetXRange(0, MinVotes); }  GP.SavePng(); }
  { TGnuPlot GP("votesOT-Sup-"+OutFNm, TStr::Fmt("Elections %d--%d: %d", MinVotes, MaxVotes, Cnt));
  GP.SetXYLabel("Time (vote index)", "Probability i-th vote is Support");
  GP.AddPlot(PSupOtA, gpwLinesPoints, "ALL elections", "", true, false);
  GP.AddPlot(PSupOtS, gpwLinesPoints, "SUCC elections", "", true, false);
  GP.AddPlot(PSupOtF, gpwLinesPoints, "FAIL elections", "", true, false);
  if (MinVotes>10) { GP.SetXRange(0, MinVotes); }  GP.SavePng(); }
  { TGnuPlot GP("votesOT-Dev-"+OutFNm, TStr::Fmt("Elections %d--%d: %d", MinVotes, MaxVotes, Cnt));
  GP.SetXYLabel("Time (vote index)", "Vote deviation: avg |avg(V,t+1) - avg(V,t)|");
  GP.AddPlot(DevOtA, gpwLinesPoints, "ALL elections", "", true, false);
  GP.AddPlot(DevOtS, gpwLinesPoints, "SUCC elections", "", true, false);
  GP.AddPlot(DevOtF, gpwLinesPoints, "FAIL elections", "", true, false);
  if (MinVotes>10) { GP.SetXRange(0, MinVotes); }  GP.SavePng(); }
}

void TWikiElecBs::PlotCovotingUsers(const TStr& OutFNm, const TStr& MinSupStr, const int& TakeOnlyVotes) const {
  PNGraph G = TNGraph::New();
  TStrSet RfaSet;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    const int RfaId = Kilo(100)+RfaSet.AddKey(E.RfaTitle);
    if (G->IsNode(RfaId)) { printf("%s\n", E.RfaTitle.CStr()); continue; }
    IAssertR(! G->IsNode(RfaId), E.RfaTitle);
    G->AddNode(RfaId);
    for (int v = 0; v < E.Len(); v++) {
      if (TakeOnlyVotes==1 && E[v].GetVote() != 1) { continue; } // take only supoprt votes
      else if (TakeOnlyVotes==-1 && E[v].GetVote() != -1) { continue; } // only oppose votes
      const int usr = E[v].GetUId();
      if (! G->IsNode(usr)) { G->AddNode(usr); }
      IAssert(! G->IsEdge(usr, RfaId));
      G->AddEdge(usr, RfaId);
    }
  }
  TSnap::PrintInfo(G);
  printf("\n*** ALL VOTES\n");
  TStrV MinSupV;
  TStr(MinSupStr).SplitOnAllCh(',', MinSupV);
  TGnuPlot GP(TStr::Fmt("itemSet-%s", OutFNm.CStr()), "Number and size of frequent itemsets");
  for (int i = 0; i < MinSupV.Len(); i++) {
    printf("\n*** MinSup = %s\n", MinSupV[i].CStr());
    TTrawling Trawl(G, MinSupV[i].GetInt());
    const TIntPrV SzCntV = Trawl.PlotMinFqVsMaxSet(TStr::Fmt("%s-%02d", OutFNm.CStr(), MinSupV[i].GetInt()));
    GP.AddPlot(SzCntV, gpwLinesPoints, TStr::Fmt("MinSup = %d", MinSupV[i].GetInt()));
  }
  GP.SetXYLabel("Itemset size", "Number of itemsets");
  GP.SavePng();
}

// fraction of support votes before and after the user casted a vote
void TWikiElecBs::PlotFracBeforeAfterVote(const TStr& OutFNm) const {
  THash<TInt, TIntPrV> UIdVotesH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10) { continue; } // election has at least 10 votes
    for (int v = 0; v < E.Len(); v++) {
      UIdVotesH.AddDat(E[v].GetUId()).Add(TIntPr(e,v));
    }
  }
  THash<TFlt, TInt> DeltaCntHA, DeltaCntHS, DeltaCntHO, DeltaCntHA2, DeltaCntHS2, DeltaCntHO2;
  FILE *F = fopen(TStr::Fmt("deltabBAUsr-%s.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#Usr\tVotes\tAvgDSup\tMedDSup\tSupVotes\tAvgDOpp\tMedDOpp\tOppVotes\tAvgTrendSup\tMedTrendSup\tAvgTrendOpp\tMedTrendOpp\n");
  for (int u = 0; u < UIdVotesH.Len(); u++) {
    const TIntPrV& EV = UIdVotesH[u];
    if (EV.Len() < 10) { continue; } // user did at least 10 votes
    TMom MomS, MomO, MomS2, MomO2;
    for (int ev = 0; ev < EV.Len(); ev++) {
      const TWikiElec& E = GetElec(EV[ev].Val1);
      const int v = EV[ev].Val2;
      if (v < 5 || v+5 >= E.Len()) { continue; }
      const double Bef = E.GetFracSup(0, v); // v-5,v
      const double Aft = E.GetFracSup(v+1, E.Len()); // full, else v+1+5
      const double Bef2 = E.GetTrend(0, v);
      const double Aft2 = E.GetTrend(v+1, E.Len()); // full
      DeltaCntHA.AddDat(TMath::Round(Aft-Bef, 2)) += 1;
      DeltaCntHA2.AddDat(TMath::Round(Aft2-Bef2, 2)) += 1;
      if (E[v].GetVote() == 1) {
        DeltaCntHS.AddDat(TMath::Round(Aft-Bef, 2)) += 1;
        DeltaCntHS2.AddDat(TMath::Round(Aft2-Bef2, 2)) += 1;
        MomS.Add(Aft-Bef);
        MomS2.Add(Aft2-Bef2);
      } else {
        DeltaCntHO.AddDat(TMath::Round(Aft-Bef, 2)) += 1;
        DeltaCntHO2.AddDat(TMath::Round(Aft2-Bef2, 2)) += 1;
        MomO.Add(Aft-Bef);
        MomO2.Add(Aft2-Bef2);
      }
    }
    MomS.Def(); MomO.Def();  MomS2.Def(); MomO2.Def();
    if (! MomS.IsUsable() || ! MomO.IsUsable()) { continue; }
    fprintf(F, "%s\t%d\t%f\t%f\t%d\t%f\t%f\t%d\n", GetUsr(UIdVotesH.GetKey(u)), EV.Len(),
      MomS.GetMean(), MomS.GetMedian(), MomS.GetVals(), MomO.GetMean(), MomO.GetMedian(), MomO.GetVals(),
      MomS2.GetMean(), MomS2.GetMedian(), MomO2.GetMean(), MomO2.GetMedian());
  }
  fclose(F);
  TGnuPlot::PlotValCntH(DeltaCntHA, "deltaBA-"+OutFNm, "Fraction of support votes After-Before user casted a vote. (min 10 votes per user, min 10 vote elections, min 5 votes before/after",
    "After - Before fraction of support votes. Any vote.", "Count");
  TGnuPlot::PlotValCntH(DeltaCntHS, "deltaBASup-"+OutFNm, "Fraction of support votes After-Before user casted a vote. (min 10 votes per user, min 10 vote elections, min 5 votes before/after",
    "After - Before fraction of support votes. User voted +1.", "Count");
  TGnuPlot::PlotValCntH(DeltaCntHO, "deltaBAOpp-"+OutFNm, "Fraction of support votes After-Before user casted a vote. (min 10 votes per user, min 10 vote elections, min 5 votes before/after",
    "After - Before fraction of support votes. User voted -1.", "Count");

  TGnuPlot::PlotValCntH(DeltaCntHA2, "deltaTrBA-"+OutFNm, "Trend of fraction ofsupport votes After-Before user casted a vote. (min 10 votes per user, min 10 vote elections, min 5 votes before/after",
    "After - Before trend (linear fit coefficient) fraction of support votes. Any vote.", "Count");
  TGnuPlot::PlotValCntH(DeltaCntHS2, "deltaTrBASup-"+OutFNm, "Trend of fraction ofsupport votes After-Before user casted a vote. (min 10 votes per user, min 10 vote elections, min 5 votes before/after",
    "After - Before trend (linear fit coefficient) fraction of support votes. User voted +1.", "Count");
  TGnuPlot::PlotValCntH(DeltaCntHO2, "deltaTrBAOpp-"+OutFNm, "Trend of fraction of support votes After-Before user casted a vote. (min 10 votes per user, min 10 vote elections, min 5 votes before/after",
    "After - Before trend (linear fit coefficient) fraction of support votes. User voted -1.", "Count");
}

void TWikiElecBs::PlotDeltaFracSupOt(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const {
  THash<TInt, TMom> DeltaOtA, DeltaOtS, DeltaOtF;
  int Cnt = 0, CntS=0, CntF=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinVotes || E.Len() > MaxVotes) { continue; }
    TIntTr SNO = E.GetVotes();
    const double FinalF = SNO.Val1/double(SNO.Val1+SNO.Val3);
    int S = 0;
    for (int v = 0; v < E.Len(); v++) {
      const int V = E.GetVote(v).GetVote()==1? 1:0;
      S += V; // support votes so far
      if (E.IsSucc) { DeltaOtS.AddDat(v).Add(S/double(v+1)-FinalF); }
      else { DeltaOtF.AddDat(v).Add(S/double(v+1)-FinalF); }
      DeltaOtA.AddDat(v).Add(S/double(v+1)-FinalF);
    }
    if (E.IsSucc) { CntS++; }
    else { CntF++; }
    Cnt++;
  }
  TGnuPlot::PlotValMomH(DeltaOtA, "deltaFracSup-A-"+OutFNm, TStr::Fmt("ALL Elections %d--%d: %d", MinVotes, MaxVotes, Cnt),
    "Vote index", "Deviation from the final fraction of support votes");
  TGnuPlot::PlotValMomH(DeltaOtS, "deltaFracSup-S-"+OutFNm, TStr::Fmt("SUCC Elections %d--%d: %d", MinVotes, MaxVotes, CntS),
    "Vote index", "Deviation from the final fraction of support votes");
  TGnuPlot::PlotValMomH(DeltaOtF, "deltaFracSup-F-"+OutFNm, TStr::Fmt("FAIL Elections %d--%d: %d", MinVotes, MaxVotes, CntF),
    "Vote index", "Deviation from the final fraction of support votes");
}

void TWikiElecBs::PlotUsrVoteVsTime(const TStr& OutFNm, const int& MinUsrVotes, const TIntSet& UsrSplitSet) const {
  THash<TInt, TIntPrV> UIdVotesH;
  TIntFltH FracSupH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    //if (E.Len() < 10) { continue; } // election has at least 10 votes
    for (int v = 0; v < E.Len(); v++) {
      UIdVotesH.AddDat(E[v].GetUId()).Add(TIntPr(e,v));
    }
    FracSupH.AddDat(e, E.GetFracSup());
  }
  UIdVotesH.SortByDat(false);
  THash<TInt, TMom> AllMomH, AllMomHS, AllMomHF, AdmMomH, NAdmMomH;
  TIntH AllCntH, AllCntHS, AllCntHF, AdmCntH, NAdmCntH;
  //TIntSet AdminSet; GetAdminSet(AdminSet);
  int Cnt=0, UCnt=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    for (int v = 0; v < TMath::Mn(E.Len(),101); v++) {
      const int Vote = E.GetVote(v).GetVote()==1?1:0;
      AllMomH.AddDat(v).Add(Vote);
      AllCntH.AddDat(v) += 1;
      if (E.IsSucc) {
        AllMomHS.AddDat(v).Add(Vote);
        AllCntHS.AddDat(v) += 1; }
      else {
        AllMomHF.AddDat(v).Add(Vote);
        AllCntHF.AddDat(v) += 1;
      }
      // admins vs. non-admins
      //if (AdminSet.IsKey(E[v].GetUId())) {
      if (UsrSplitSet.IsKey(E[v].GetUId())) {
        AdmMomH.AddDat(v).Add(Vote);
        AdmCntH.AddDat(v) += 1;
      } else {
        NAdmMomH.AddDat(v).Add(Vote);
        NAdmCntH.AddDat(v) += 1;
      }
    }
  }
  /*
  for (int u = 0; u < UIdVotesH.Len(); u++) {
    THash<TInt, TMom> UsrMomH;
    const TIntPrV& EV = UIdVotesH[u];
    //if (EV.Len() < MinUsrVotes) { continue; }
    for (int ev = 0; ev < EV.Len(); ev++) {
      const TWikiElec& E = GetElec(EV[ev].Val1);
      const int v = EV[ev].Val2;
      const int Vote = E[v].GetVote()==1 ? 1:0;
      //const double Vote = E[v].GetVote()==1 ? FracSupH.GetDat(EV[ev].Val1):1-FracSupH.GetDat(EV[ev].Val1);
      if (v > 100) { continue; }
      UsrMomH.AddDat(v).Add(Vote);
      AllMomH.AddDat(v).Add(Vote);
      AllCntH.AddDat(v) += 1;
      if (E.IsSucc) {
        AllMomHS.AddDat(v).Add(Vote);
        AllCntHS.AddDat(v) += 1; }
      else {
        AllMomHF.AddDat(v).Add(Vote);
        AllCntHF.AddDat(v) += 1;
      }
      Cnt++;
    }
    //TGnuPlot::PlotValMomH(UsrMomH, TStr::Fmt("usrVoteVsTm-%s-%03d", OutFNm.CStr(), u),
    //  TStr::Fmt("%d votes of user %s", EV.Len(), GetUsr(UIdVotesH.GetKey(u))), "time when user voted", "fraction of positive votes",
    //  gpsAuto, gpwLinesPoints, true, false, false, false, false, false);
    UCnt++;
  } //*/
  TGnuPlot::PlotValMomH(AllMomH, TStr::Fmt("usrVoteVsTm-%s-all", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "fraction of positive votes",
    gpsAuto, gpwLinesPoints, true, false, false, false, false, false);
  TGnuPlot::PlotValCntH(AllCntH, TStr::Fmt("usrVoteVsTm-%s-CNT-all", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "number of votes");
  TGnuPlot::PlotValMomH(AdmMomH, TStr::Fmt("usrVoteVsTm-%s-admin", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "fraction of positive votes",
    gpsAuto, gpwLinesPoints, true, false, false, false, false, false);
  TGnuPlot::PlotValCntH(AdmCntH, TStr::Fmt("usrVoteVsTm-%s-CNT-admin", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "number of votes");
  TGnuPlot::PlotValMomH(NAdmMomH, TStr::Fmt("usrVoteVsTm-%s-nonadmin", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "fraction of positive votes",
    gpsAuto, gpwLinesPoints, true, false, false, false, false, false);
  TGnuPlot::PlotValCntH(NAdmCntH, TStr::Fmt("usrVoteVsTm-%s-CNT-nonadmin", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "number of votes");
  /*TGnuPlot::PlotValMomH(AllMomHS, TStr::Fmt("usrVoteVsTm-%s-succ", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. SUCC ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "fraction of positive votes",
    gpsAuto, gpwLinesPoints, true, false, false, false, false, false);
  TGnuPlot::PlotValMomH(AllMomHF, TStr::Fmt("usrVoteVsTm-%s-fail", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. FAIL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "fraction of positive votes",
    gpsAuto, gpwLinesPoints, true, false, false, false, false, false);*/
  /*TGnuPlot::PlotValCntH(AllCntHS, TStr::Fmt("usrVoteVsTm-%s-CNT-succ", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. SUCC ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "number of votes");
  TGnuPlot::PlotValCntH(AllCntHF, TStr::Fmt("usrVoteVsTm-%s-CNT-fail", OutFNm.CStr()),
    TStr::Fmt("%d votes of %d users with more than %d votes. FAIL ELEC.", Cnt, UCnt, MinUsrVotes),
    "time when user voted", "number of votes");*/
}

// plot as a function of outcome
void TWikiElecBs::PlotFirstOppOutcome(const TStr& OutFNm, const int& NVotes) const {
  THash<TInt, TMom> FOppH, FOpp2H;
  TIntH FOppCntH;
  int NElec=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10 || E.Len() < NVotes) { continue; } // election has at least 10 votes
    int FirstOpp=-1, VoteSum=0;
    for (int v = 0; v < NVotes; v++) {
      if (E[v].GetVote()==-1 && FirstOpp==-1) { FirstOpp = v+1; }
      VoteSum += E[v].GetVote();
    }
    if (VoteSum != NVotes-2) { continue; }
    FOppCntH.AddDat(FirstOpp) += 1;
    FOppH.AddDat(FirstOpp).Add(E.IsSucc?1:0);
    FOpp2H.AddDat(FirstOpp).Add(E.GetFracSup());
    NElec++;
  }
  TGnuPlot::PlotValMomH(FOppH, "firstOpp1-"+OutFNm, TStr::Fmt("Take first %d votes of an election, where %d Sup and 1 Opp. %d such elections", NVotes, NVotes-1, NElec),
    "Index of the oppose vote", "Fraction of successful elections", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(FOpp2H, "firstOpp2-"+OutFNm, TStr::Fmt("Take first %d votes of an election, where %d Sup and 1 Opp. %d such elections", NVotes, NVotes-1, NElec),
    "Index of the oppose vote", "Final fraction of support votes in the election", gpsAuto, gpwLinesPoints, true, false, false, false, true, false);
  TGnuPlot::PlotValCntH(FOppCntH, "firstOppCnt-"+OutFNm, TStr::Fmt("Take first %d votes of an election, where %d Sup and 1 Opp. %d such elections", NVotes, NVotes-1, NElec),
    "Index of the oppose vote", "Number of such elections");
}

void TWikiElecBs::PlotVoteTrails(const TStr& OutFNm, const int& MinUsrVotes, const bool& No01Prob) const {
  TIntSet AdminSet;
  TIntV UIdV;
  TVec<TFltPrV> ProbSupTmV, FracSupTmV, ProbSupFracSupV, VotesTmV, VoteIdxFracSupV, NVotesFracSupV;
  const int TotalVotes = GetVoteTrails(MinUsrVotes, No01Prob, UIdV, ProbSupTmV, FracSupTmV, ProbSupFracSupV, VotesTmV);
  GetVoteTrails2(MinUsrVotes, No01Prob, UIdV, VoteIdxFracSupV, NVotesFracSupV);
  /// admins
  GetAdminSet(AdminSet);
  /// frequent voters (take top half of the voters by votes)
  /*TIntPrV VotesV; GetUsrVotes(VotesV);
  TIntH VotesH;
  for (int i = 0; i < VotesV.Len(); i++) {
    if (UIdV.IsIn(VotesV[i].Val2)) {
    VotesH.AddDat(VotesV[i].Val2, VotesV[i].Val1); }
  }
  VotesH.SortByDat(false);
  for (int i = 0; i < VotesH.Len()/2;i++) {
    AdminSet.AddKey(VotesH.GetKey(i)); } //*/
  //// votes by the time of their first vote
  //

  TIntH UsrLnTy;
  for (int u = 0; u < UIdV.Len(); u++) { UsrLnTy.AddDat(UIdV[u], AdminSet.IsKey(UIdV[u])?2:1); }
  //for (int u = 0; u < UIdV.Len(); u++) { UsrLnTy.AddDat(UIdV[u], u+1); }
  /*//// color based on time of adminship (green early, red late)
  THash<TInt, TSecTm> AdminTmH;
  GetAdminTmSet(AdminTmH); AdminTmH.SortByDat(true);
  for (int u = 0; u < UIdV.Len(); u++) {
    if (! AdminTmH.IsKey(UIdV[u])) { UsrLnTy.AddDat(UIdV[u], 3); }
    else { UsrLnTy.AddDat(UIdV[u], (AdminTmH.GetKeyId(UIdV[u])>=AdminTmH.Len()/2)?2:1); }
  }//*/

  /*//// Save WikiTalk net on frequent voters
  PWikiTalkNet Net = TWikiTalkNet::Load(TZipIn("data/wikiTalkFull.net.rar"));
  TIntH UIdTalkH;
  TIntStrH ColorH;  TIntV SubNIdV;
  for (int u = 0; u < UIdV.Len(); u++) {
    const TStr U = GetUsr(UIdV[u]);
    if (! Net->IsUsr(U)) { UIdTalkH.AddDat(UIdV[u], 0); printf("x"); }
    else { UIdTalkH.AddDat(UIdV[u], Net->GetNI(Net->GetUsrNId(U)).GetOutDeg()); printf(".");
    //else { UIdTalkH.AddDat(UIdV[u], Net->GetNDat(Net->GetUsrNId(U)).GetTkEdCnt()); printf(".");
      SubNIdV.Add(Net->GetUsrNId(U)); ColorH.AddDat(SubNIdV.Last(), AdminSet.IsKey(UIdV[u])?"Blue":"Red");
    }
  }
  UIdTalkH.SortByDat();
  for (int i = 0; i < UIdTalkH.Len(); i++) {
    if (i < UIdTalkH.Len()/2) { UsrLnTy.AddDat(UIdTalkH.GetKey(i), 1); }
    else { UsrLnTy.AddDat(UIdTalkH.GetKey(i), 2); }
  }
  TSnap::SavePajek(TSnap::GetSubGraph(Net, SubNIdV), "200votes-talknet-all.net", ColorH);
  TSnap::SavePajek(TSnap::GetSubGraph(Net, SubNIdV, TWikiTalkEdge(TSecTm(), TSecTm(), 10, 10), true), "200votes-talknet-10talks.net", ColorH);
  //*/
  // plot
  TGnuPlot GPP("voteTrailP-"+OutFNm+"-r10"); //r10==10buckets
  TGnuPlot GPF("voteTrailF-"+OutFNm+"-r10");
  TGnuPlot GPPF("voteTrailPF-"+OutFNm+"-r10");
  TGnuPlot GPV("voteHist-"+OutFNm+"-r10"); // votes over time
  TGnuPlot GPFI("voteFIdx-"+OutFNm+"-r10");
  TGnuPlot GPFC("voteFCnt-"+OutFNm+"-r10");
  THash<TFlt, TMom> AvgAdm, AvgNAdm;
  bool FirstA=true, FirstNA=true;
  for (int u = 0; u < UIdV.Len(); u++) {
    const int UId = UIdV[u];
    TStr Tit;
    if (FirstA && UsrLnTy.GetDat(UId) == 2) { Tit="Admin (HIGH)"; FirstA=false; }
    if (FirstNA && UsrLnTy.GetDat(UId) == 1) { Tit="NonAdmin (LOW)"; FirstNA=false; }
    GPP.AddPlot(ProbSupTmV[u], gpwLines, Tit, TStr::Fmt("lt %d lw 1 smooth bezier", UsrLnTy.GetDat(UId)));
    GPF.AddPlot(FracSupTmV[u], gpwLines, Tit, TStr::Fmt("lt %d lw 1 smooth bezier", UsrLnTy.GetDat(UId)));
    GPV.AddPlot(VotesTmV[u], gpwLines, Tit, TStr::Fmt("lt %d lw 1 smooth bezier", UsrLnTy.GetDat(UId)));
    GPPF.AddPlot(ProbSupFracSupV[u], gpwLinesPoints, Tit, TStr::Fmt("lt %d lw 1 smooth bezier", UsrLnTy.GetDat(UId)));
    GPFI.AddPlot(VoteIdxFracSupV[u], gpwLines, Tit, TStr::Fmt("lt %d lw 1 smooth bezier", UsrLnTy.GetDat(UId)));
    GPFC.AddPlot(NVotesFracSupV[u], gpwLines, Tit, TStr::Fmt("lt %d lw 1 smooth bezier", UsrLnTy.GetDat(UId)));
    for (int i = 0; i < ProbSupFracSupV[u].Len(); i++) {
      if (AdminSet.IsKey(UId) || AdminSet.Empty()) { // admins
        AvgAdm.AddDat(ProbSupFracSupV[u][i].Val1).Add(ProbSupFracSupV[u][i].Val2); }
      else { // non-admins
        AvgNAdm.AddDat(ProbSupFracSupV[u][i].Val1).Add(ProbSupFracSupV[u][i].Val2); }
    }
  }
  TFltPrV AvgAdmV, AvgNAdmV;
  for (int i =0; i < AvgAdm.Len(); i++) { AvgAdm[i].Def();
    AvgAdmV.Add(TFltPr(AvgAdm.GetKey(i), AvgAdm[i].GetMean())); }
  for (int i =0; i < AvgNAdm.Len(); i++) { AvgNAdm[i].Def();
    AvgNAdmV.Add(TFltPr(AvgNAdm.GetKey(i), AvgNAdm[i].GetMean())); }
  AvgAdmV.Sort();  AvgNAdmV.Sort();
  GPPF.AddPlot(AvgAdmV, gpwLinesPoints, "AVG ADMIN (HIGH)", TStr::Fmt("lt %d lw 5 smooth bezier", 4));
  GPPF.AddPlot(AvgNAdmV, gpwLinesPoints, "AVG NON-ADMIN (LOW)", TStr::Fmt("lt %d lw 5 smooth bezier", 3));
  GPP.SetTitle(TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", TotalVotes, UIdV.Len(), MinUsrVotes));
  GPP.SetXYLabel("vote index", "probability of support vote");
  GPP.SetYRange(0,1.01);
  //GPP.SavePng();
  GPF.SetTitle(TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", TotalVotes, UIdV.Len(), MinUsrVotes));
  GPF.SetXYLabel("vote index", "fraction of support votes so far");
  GPF.SetYRange(0,1.01);
  //GPF.SavePng();
  GPPF.SetTitle(TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", TotalVotes, UIdV.Len(), MinUsrVotes));
  GPPF.SetXYLabel("Fraction of support votes at time of vote", "Probability of voting positively");
  GPPF.SetYRange(0,1.01);
  GPPF.SavePng();
  GPV.SetTitle(TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", TotalVotes, UIdV.Len(), MinUsrVotes));
  GPV.SetXYLabel("vote index", "number of votes");
  //GPV.SavePng();
  GPFI.SetTitle(TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", TotalVotes, UIdV.Len(), MinUsrVotes));
  GPFI.SetXYLabel("Fraction of support votes at time of vote", "Average vote index (time) of a user voting");
  //GPFI.SavePng();
  GPFC.SetTitle(TStr::Fmt("%d votes of %d users with more than %d votes. ALL ELEC.", TotalVotes, UIdV.Len(), MinUsrVotes));
  GPFC.SetXYLabel("Fraction of support votes at time of vote", "Number of times user voted at that fraction");
  //GPFC.SavePng();
}

void TWikiElecBs::PlotVoteTrailGlobal(const TStr& OutFNm) const {
  THash<TInt, TMom> SuppVoteH, Set1, Set2;
  TIntSet UIdSet;
  //GetFqVoterSet(UIdSet);
  GetAdminSet(UIdSet);
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    int nsup=0;
    if (E.Len()<10) { continue; }
    for (int v = 0; v < E.Len(); v++) {
      const int supFrac = v==0? 0 : int(TMath::Round(10.0*nsup/double(v)))*10;
      nsup += E.GetVote(v).GetVote()==1?1:0;
      //if (supFrac==0) { continue; }
      if (v<10) { continue; } // skip first 10 votes
      SuppVoteH.AddDat(supFrac).Add(E.GetVote(v).GetVote()==1?1:0);
      if (UIdSet.IsKey(E.GetVote(v).GetUId())) { // set 1
        Set1.AddDat(supFrac).Add(E.GetVote(v).GetVote()==1?1:0);
      } else { // set 2
        Set2.AddDat(supFrac).Add(E.GetVote(v).GetVote()==1?1:0);
      }
    }
  }
  TGnuPlot::PlotValMomH(SuppVoteH, "voteTrailAll-"+OutFNm, "Global vote trail", "Fraction of support votes at time of vote", "Probability of voting positively", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(Set1, "voteTrailAll1-"+OutFNm, "Global vote trail. Set1", "Fraction of support votes at time of vote", "Probability of voting positively", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
  TGnuPlot::PlotValMomH(Set2, "voteTrailAll2-"+OutFNm, "Global vote trail. Set2", "Fraction of support votes at time of vote", "Probability of voting positively", gpsAuto, gpwLinesPoints, true, false, false, false, false, true);
}

void TWikiElecBs::PlotFinalFracVoteCnt(const TStr& OutFNm, const int& MinUsrVotes, const TIntSet& UsrSplitSet) const {
  TIntPrV VoteUIdV; GetUsrVotes(VoteUIdV);
  THash<TInt, TIntH> UIdVoteCntH;
  TIntH FracVotesH, Set1, Set2;
  VoteUIdV.Sort(false);
  for (int i = 0; i < VoteUIdV.Len() && VoteUIdV[i].Val1>MinUsrVotes; i++) {
    UIdVoteCntH.AddKey(VoteUIdV[i].Val2); }
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    //if (E.Len() < 10) { continue; } // election has at least 10 votes
    const int SupFrac= (int(100*E.GetFracSup())/10)*10;
    for (int v = 0; v < E.Len(); v++) {
      const int V = E[v].GetVote()==1?1:0;
      const int U = E[v].GetUId();
      FracVotesH.AddDat(SupFrac) += 1;
      if (UsrSplitSet.IsKey(U)) { Set1.AddDat(SupFrac) += 1; }
      else { Set2.AddDat(SupFrac) += 1; }
      if (UIdVoteCntH.IsKey(U)) { UIdVoteCntH.AddDat(U).AddDat(SupFrac) += 1; }
    }
  }
  TGnuPlot::PlotValCntH(FracVotesH, "voteFrac-"+OutFNm+"-all", "All users", "Final fraction of support votes", "Number of votes");
  TGnuPlot::PlotValCntH(Set1, "voteFrac-"+OutFNm+"-set1", "UsrSplitSet users", "Final fraction of support votes", "Number of votes");
  TGnuPlot::PlotValCntH(Set2, "voteFrac-"+OutFNm+"-set2", "Users no in UsrSplitSet", "Final fraction of support votes", "Number of votes");
  TGnuPlot GP("voteFrac-"+OutFNm);
  for (int u = 0; u < UIdVoteCntH.Len(); u++) {
    TIntPrV ValV;
    UIdVoteCntH[u].GetKeyDatPrV(ValV);
    ValV.Sort();
    GP.AddPlot(ValV, gpwLinesPoints, "", TStr::Fmt("lw 1 smooth bezier"));
  }
  GP.SetXYLabel("Final fraction of support votes", "Number of votes");
  GP.SavePng();
}

void TWikiElecBs::PlotConfusionMatrix(const TStr& OutFNm, const int& MinUsrVotes) const {
  THash<TInt, TTuple<TFlt, 4> > TupH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    const int R = E.IsSucc?1:0;
    for (int v = 0; v < E.Len(); v++) {
      const int V = E[v].GetVote()==1?1:0;
      TupH.AddDat(E[v].GetUId())[2*R+V] += 1;
    }
  }
  FILE *F=fopen(TStr::Fmt("confusion-%s.tab", OutFNm.CStr()).CStr(), "wt");
  for (int u = 0; u < TupH.Len(); u++) {
    const double S = TupH[u][0]+TupH[u][1]+TupH[u][2]+TupH[u][3];
    if (S < MinUsrVotes) { continue; }
    //TupH[u][0]/=S;  TupH[u][1]/=S;  TupH[u][2]/=S;  TupH[u][3]/=S;
    fprintf(F, "%f\t%f\t%f\t%f\n", TupH[u][0], TupH[u][1], TupH[u][2], TupH[u][3]);
  }
  fclose(F);
}

void TWikiElecBs::PlotSlopeHist(const TStr& OutFNm, const int& MinElecLen) const {
  const double BPrec = 1000;
  TIntH R21A, R22A, K1A, K2A;
  TIntH R21S, R22S, K1S, K2S;
  TIntH R21F, R22F, K1F, K2F;
  int Cnt=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < MinElecLen) { continue; }
    TFltPrV XyV, Xy2;
    for (int v = 0; v < E.Len(); v++) {
      XyV.Add(TFltPr(v, E[v].GetVote()));
      Xy2.Add(TFltPr(v, E.GetFracSup(0, v+1)));
    }
    double A, B, SigA, SigB, Chi2, R2=0;
    TSpecFunc::LinearFit(XyV, A, B, SigA, SigB, Chi2, R2);
    if (R2 < -1 || _isnan(R2)) R2 = 0;
    K1A.AddDat(int(BPrec*B))+=1;
    R21A.AddDat(int(100*R2))+=1;
    if (E.IsSucc) {
      K1S.AddDat(int(BPrec*B))+=1;
      R21S.AddDat(int(100*R2))+=1;
    } else {
      K1F.AddDat(int(BPrec*B))+=1;
      R21F.AddDat(int(100*R2))+=1;
    }
    TSpecFunc::LinearFit(Xy2, A, B, SigA, SigB, Chi2, R2);
    if (R2 < -1 || _isnan(R2)) R2 = 0;
    K2A.AddDat(int(BPrec*B))+=1;
    R22A.AddDat(int(100*R2))+=1;
    if (E.IsSucc) {
      K2S.AddDat(int(BPrec*B))+=1;
      R22S.AddDat(int(100*R2))+=1;
    } else {
      K2F.AddDat(int(BPrec*B))+=1;
      R22F.AddDat(int(100*R2))+=1;
    }
    Cnt++;
  }
  TGnuPlot::PlotValCntH(K1A, "ALL ELEC", K1S, "SUCC ELEC", K1F, "FAIL ELEC", "slopeK1-"+OutFNm, TStr::Fmt("%d elections with >%d votes", Cnt, MinElecLen),  "slope (fit of Prob(+|t) vs t)", "count", gpsLog10Y);
  TGnuPlot::PlotValCntH(K2A, "ALL ELEC", K2S, "SUCC ELEC", K2F, "FAIL ELEC", "slopeK2-"+OutFNm, TStr::Fmt("%d elections with >%d votes", Cnt, MinElecLen),  "slope (fit of FracSup(1..t) vs t)", "count", gpsLog10Y);
  TGnuPlot::PlotValCntH(R21A, "ALL ELEC", R21S, "SUCC ELEC", R21F, "FAIL ELEC", "slopeR1-"+OutFNm, TStr::Fmt("%d elections with >%d votes", Cnt, MinElecLen),  "R2 (fit of Prob(+|t) vs t)", "count", gpsLog10Y);
  TGnuPlot::PlotValCntH(R22A, "ALL ELEC", R22S, "SUCC ELEC", R22F, "FAIL ELEC", "slopeR2-"+OutFNm, TStr::Fmt("%d elections with >%d votes", Cnt, MinElecLen),  "R2 (fit of FracSup(1..t) vs t)", "count", gpsLog10Y);
}

//// old plots
void TWikiElecBs::PlotBarnStarsDelta(const TStr& OutFNm) const {
  TBarnStars BarnStars;
  THash<TInt, TMom> DiffMomH;
  TIntH DiffCntH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    const TStr TargetUsr = GetUsr(E.GetUId());
    const int TB = BarnStars.GetBarnStars(TargetUsr, E.GetTm());
    for (int v = 0; v < E.Len(); v++) {
      if (! E[v].IsVote()) { continue; }
      const int DeltaStars = BarnStars.GetBarnStars(GetUsr(E[v].GetUId()), E[v].GetTm()) - TB;
      DiffMomH.AddDat(DeltaStars).Add(E[v].GetVote()==1?1:0);
      DiffCntH.AddDat(DeltaStars) += 1;
    }
  }
  TGnuPlot::PlotValMomH(DiffMomH, "dBarnStars-"+OutFNm, "Number of BarnStars (over ALL VOTES): "+OutFNm, "Barnstars delta (source - destination)",
    "Fraction of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false);
  TGnuPlot::PlotValCntH(DiffCntH, "dBarnStarts2-"+OutFNm, "Number of BarnSTars (over aLL VOES): "+OutFNm, "Barnstars delta (source - destination)",
    "Number of such users", gpsAuto, false, gpwLinesPoints, false, false);
}
// plot the distribution on the number of in-votes for people that went up for adminship
// and were/were not promoted
void TWikiElecBs::PlotAdminVotes(const TStr& OutFNm) const {
  TIntH APos, NAPos, ANeg, NANeg;
  TIntPrSet AVotes, NAVotes;
  for (int e = 0; e < Len(); e++) {
    const TIntTr V = GetElec(e).GetVotes(true);
    if (GetElec(e).IsSucc) {
      APos.AddDat(V.Val1) += 1;
      ANeg.AddDat(V.Val3) += 1;
      AVotes.AddKey(TIntPr(V.Val1, V.Val3));
    } else {
      NAPos.AddDat(V.Val1) += 1;
      NANeg.AddDat(V.Val3) += 1;
      NAVotes.AddKey(TIntPr(V.Val1, V.Val3));
    }
  }
  // election results for admins vs. non-admins
  { TGnuPlot GP("adminSup-"+OutFNm, OutFNm);
  GP.AddPlot(APos, gpwLinesPoints, "Support votes for ADMINS");
  GP.AddPlot(NAPos, gpwLinesPoints, "Support votes for NON-ADMINS");
  GP.SetXYLabel("Number of support votes", "Number of elections");
  GP.SavePng(); }
  { TGnuPlot GP("adminOpp-"+OutFNm, OutFNm);
  GP.AddPlot(ANeg, gpwLinesPoints, "Oppose votes for ADMINS");
  GP.AddPlot(NANeg, gpwLinesPoints, "Oppose votes for NON-ADMINS");
  GP.SetXYLabel("Number of oppose votes", "Number of elections");
  GP.SavePng(); }
  { TGnuPlot GP("adminSupOpp-"+OutFNm, OutFNm);
  TIntPrV V;  AVotes.GetKeyV(V);
  GP.AddPlot(V, gpwPoints, "Votes for ADMINS");
  NAVotes.GetKeyV(V);
  GP.AddPlot(V, gpwPoints, "Votes for NON-ADMINS");
  GP.SetXYLabel("Number of support votes", "Number of oppose votes");
  GP.SavePng(); }
}

void TWikiElecBs::PlotAvgVote(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const {
  TIntV ElecV;
  for (int e = 0; e < Len(); e++) {
    //if (GetElec(e).Len() >= MinVotes && GetElec(e).Len() <= MaxVotes) {
    if (! GetElec(e).IsSucc()) {
      ElecV.Add(e); }
  }
  PlotAvgVote(ElecV, OutFNm, TStr::Fmt("Elections with %d--%d votes, %d elections", MinVotes, MaxVotes, ElecV.Len()));
}

// running vote average over time
void TWikiElecBs::PlotAvgVote(const TIntV& ElecIdV, const TStr& OutFNm, const TStr& Desc) const {
  THash<TInt, TMom> MomH;
  THash<TInt, TMom> Mom2H;
  TFltV AvgVoteV;
  for (int i = 0; i < ElecIdV.Len(); i++) {
    GetElec(ElecIdV[i]).GetAvgVoteOt(AvgVoteV, true);
    for (int v = 0; v < AvgVoteV.Len(); v++) {
      MomH.AddDat(v+1).Add(AvgVoteV[v]); }
    const TWikiElec& E = GetElec(ElecIdV[i]);
    for (int v = 0; v < E.Len(); v++) {
      Mom2H.AddDat(v).Add(E.GetVote(v).GetVote()==1?1:0);
    }
  }
  TGnuPlot::PlotValMomH(MomH, "voteAvg-"+OutFNm, TStr::Fmt("%s. %d elections", Desc.CStr(), ElecIdV.Len()),
    "n (vote index, time)", "Running average of positive votes", gpsAuto, gpwLinesPoints, true, false, false, false, false);
  TGnuPlot::PlotValMomH(Mom2H, "voteAvg2-"+OutFNm, TStr::Fmt("%s. %d elections", Desc.CStr(), ElecIdV.Len()),
    "n (vote index, time)", "Probability of positive vote", gpsAuto, gpwLinesPoints, true, false, false, false, false);
}

// running vote deviation over time
void TWikiElecBs::PlotAvgVoteDev(const TIntV& ElecIdV, const TStr& OutFNm, const TStr& Desc) const {
  THash<TInt, TMom> MomH;
  TFltV AvgVoteV;
  for (int i = 0; i < ElecIdV.Len(); i++) {
    GetElec(ElecIdV[i]).GetAvgVoteDevOt(AvgVoteV, true);
    for (int v = 0; v < AvgVoteV.Len(); v++) {
      MomH.AddDat(v+1).Add(AvgVoteV[v]); }
  }
  TGnuPlot::PlotValMomH(MomH, "voteDev-"+OutFNm, TStr::Fmt("%s. %d elections", Desc.CStr(), ElecIdV.Len()),
    "n (vote index, time)", "Deviation of the running average", gpsAuto, gpwLinesPoints, true, false, false, false, false);
  MomH.SortByKey();
  for (int v = 0; v < MomH.Len(); v+=10) {
    printf("  %d\t%g\n", v, MomH[v].GetWgt());
  }
}

void TWikiElecBs::PlotAvgSupFrac(const TIntV& ElecIdV, const TStr& OutFNm, const TStr& Desc) const {
  THash<TInt, TMom> MomH;
  TFltV AvgVoteV;
  for (int i = 0; i < ElecIdV.Len(); i++) {
    TWikiVoteV VoteV;
    GetElec(ElecIdV[i]).GetVotesOt(VoteV);
    for (int v = 0; v < VoteV.Len(); v++) {
      MomH.AddDat(v+1).Add(VoteV[v].GetVote()==1?1:0); }
  }
  TGnuPlot::PlotValMomH(MomH, "voteFrac-"+OutFNm, TStr::Fmt("%s. %d elections", Desc.CStr(), ElecIdV.Len()),
    "T (vote index, time)", "Fraction of all votes casted at time T that were supporting", gpsAuto, gpwLinesPoints, true, false, false, false, false);
}

void TWikiElecBs::PlotOutcomes(const TStr& OutFNm) const {
  TWikiVoteV VoteV;
  TIntV UIdV;  GetEIdByVotes(UIdV);
  TGnuPlot GP("votes-otime3-0"), GP2("votes-otime4-0");
  for (int u = 0; u < 100; u++) {
    const TWikiElec& E = GetElec(UIdV[u]);
    E.GetVotesOt(VoteV, true);
    const int b = VoteV[0].GetTm().GetAbsSecs();
    TFltPrV FracV, FracV2;
    int pos=0, neg=0;
    for (int v = 0; v < VoteV.Len(); v++) {
      if (VoteV[v].GetVote()==1) { pos++; }
      else if (VoteV[v].GetVote()==-1) { neg++; } else { continue; }
      FracV.Add(TFltPr(v, pos/double(pos+neg)));
      //if (v < 10) { continue; }
      FracV2.Add(TFltPr(v/double(VoteV.Len()), pos/double(pos+neg)));
    }
    GP.AddPlot(FracV, gpwLines);
    GP2.AddPlot(FracV2, gpwLines);
    if ((u+1) % 10 == 0) {
      GP.SavePng();
      GP2.SavePng();
      GP = TGnuPlot(TStr::Fmt("votes-otime3-%d", u/10));
      GP2 = TGnuPlot(TStr::Fmt("votes-otime4-%d", u/10));
    }
  }
  GP.SavePng();
  GP2.SavePng();
}

void TWikiElecBs::PlotSupFracVsElecLen(const TStr& OutFNm) const {
  THash<TInt, TMom> ElecMom;
  THash<TInt, TMom> ElecMom5;
  THash<TInt, TMom> ElecMom10;
  for (int e = 0; e < Len(); e++) {
    ElecMom.AddDat(GetElec(e).Len()).Add(GetElec(e).GetFracSup());
    ElecMom5.AddDat(5*(GetElec(e).Len()/5)).Add(GetElec(e).GetFracSup());
    ElecMom10.AddDat(10*(GetElec(e).Len()/10)).Add(GetElec(e).GetFracSup());
  }
  TGnuPlot::PlotValMomH(ElecMom, "fracSupLen-"+OutFNm, "Bucket size 1", "Number of votes in the election", "Final fraction of support votes");
  TGnuPlot::PlotValMomH(ElecMom5, "fracSupLen-"+OutFNm+"5", "Bucket size 5", "Number of votes in the election", "Final fraction of support votes");
  TGnuPlot::PlotValMomH(ElecMom10, "fracSupLen-"+OutFNm+"10", "Bucket size 10", "Number of votes in the election", "Final fraction of support votes");
}

void TWikiElecBs::PlotSupOpp(const TStr& OutFNm) const {
  /*TWikiVoteV VoteV;
  TIntV UIdV;  GetEIdByVotes(UIdV);
  TGnuPlot GP("votes-posneg2-00");
  TVec<TMom> SupV, OppV;
  THash<TFlt, TMom> SupH;
  for (int u = 0; u < 2000; u++) {
    const TWikiElec& E = GetElec(UIdV[u]);
    E.GetVotesOt(VoteV, true);
    //printf("%d\t%d\t%d\n", PON.Val1, PON.Val2, PON.Val3);
    TFltPrV SupOppV;
    int pos=0, neg=0;
    for (int v = 0, cnt=0; v < VoteV.Len(); v++) {
      if (VoteV[v].GetVote()==1) { pos++; }
      else if (VoteV[v].GetVote()==-1) { neg++; } else { continue; }
      //SupOppV.Add(TFltPr(pos, neg));
      SupOppV.Add(TFltPr(pos/double(PON.Val1), neg/double(PON.Val2)));//2
      while (SupV.Len() <= cnt) { SupV.Add(); OppV.Add(); }
      SupV[cnt].Add(pos);
      OppV[cnt].Add(neg);
      if (PON.Val1!=0 && PON.Val2!=0) {
        SupH.AddDat(TMath::Round(pos/double(PON.Val1), 1)).Add(neg/double(PON.Val2)); }
      cnt++;
    }
    //GP.AddPlot(SupOppV, gpwLines);
    if ((u+1) % 10 == 0) {
      //GP.SetXRange(0,1);  GP.SetYRange(0,1);
      //GP.AddPlot(TIntPrV::GetV(TIntPr(0,0), TIntPr(1,1)), gpwLines, "", "lw 3");
      //GP.SavePng();
      //GP = TGnuPlot(TStr::Fmt("votes-posneg2-%02d", u/10));
    }
  }
  GP.SavePng();
  TFltPrV AvgV, MedV;
  for (int i = 0; i < SupV.Len(); i++) {
    SupV[i].Def();  OppV[i].Def();
    AvgV.Add(TFltPr(SupV[i].GetMean(), OppV[i].GetMean()));
    MedV.Add(TFltPr(SupV[i].GetMedian(), OppV[i].GetMedian()));
  }
  TGnuPlot::PlotValV(AvgV, "votes-posneg-avg",  "", "","", gpsAuto, false, gpwLines);
  TGnuPlot::PlotValV(MedV, "votes-posneg-median",  "", "","", gpsAuto, false, gpwLines);
  AvgV.Clr(); MedV.Clr();  SupH.SortByKey();
  for (int i = 0; i < SupH.Len(); i++) {
    SupH[i].Def();
    AvgV.Add(TFltPr(SupH.GetKey(i), SupH[i].GetMean()));
    MedV.Add(TFltPr(SupH.GetKey(i), SupH[i].GetMedian()));
  }
  TGnuPlot::PlotValV(AvgV, "votes-posneg2-avg",  "", "","", gpsAuto, false, gpwLines);
  TGnuPlot::PlotValV(MedV, "votes-posneg2-median",  "", "","", gpsAuto, false, gpwLines);
  // */
}

void TWikiElecBs::PlotVoteDistr(const TStr& OutFNm) const {
  TWikiVoteV VoteV;
  TIntV UIdV;  GetEIdByVotes(UIdV);
  TVec<TMom> SupV, OppV;
  THash<TInt, TIntH> PosH, NegH;
  for (int u = 0; u < 2000; u++) {
    const TWikiElec& E = GetElec(UIdV[u]);
    const TIntTr PON = E.GetVotes(true);
    if ((PON.Val1+2*PON.Val2) < 50)  {
      PosH.AddDat((PON.Val1+PON.Val2)/10).AddDat((PON.Val1))++;
      NegH.AddDat((PON.Val1+PON.Val2)/10).AddDat((PON.Val1-PON.Val2))++;
      printf("%d ", PON.Val1+PON.Val2);
    }
    //int pos=0, neg=0;
    /*for (int v = 0, cnt=0; v < VoteV.Len(); v++) {
      if (VoteV[v].GetVote()==1) { pos++; }
      else if (VoteV[v].GetVote()==-1) { neg++; } else { continue; }
      if ((pos+neg) % 5 == 0 && (pos+neg) < 100) {
        PosH.AddDat((pos+neg)/10).AddDat((pos-neg))++;
        NegH.AddDat((pos+neg)/10).AddDat((neg))++;
      }
    }*/
  }
  PosH.SortByKey(); NegH.SortByKey();
  for (int i = 0; i < PosH.Len(); i++) {
    TGnuPlot GP(TStr::Fmt("voteX-distr%02d", PosH.GetKey(i)));
    TFltPrV PrV1, PrV2;
    IAssert(PosH.GetKey(i) == NegH.GetKey(i));
    TIntH& CntH1 = PosH[i];  CntH1.SortByKey();
    TIntH& CntH2 = NegH[i];  CntH2.SortByKey();
    for (int j = 0; j < CntH1.Len(); j++) {
      PrV1.Add(TFltPr(CntH1.GetKey(j).Val, CntH1[j].Val)); }
    for (int j = 0; j < CntH2.Len(); j++) {
      PrV2.Add(TFltPr(CntH2.GetKey(j).Val, CntH2[j].Val)); }
    GP.AddPlot(PrV1, gpwLinesPoints, "Pos");
    //GP.AddPlot(PrV2, gpwLinesPoints, "Neg");
    GP.AddCmd("set yzeroaxis lt -1");
    GP.SavePng();
  }
}

// nodes are users that voted on, edges are who voted on whom
// take only users successfully promoted for adminship
PSignNet TWikiElecBs::GetAdminUsrVoteNet() const {
  TIntV UIdV;
  GetElecAdminUsrV(UIdV);
  return GetVoteNet(UIdV);
}

// nodes are users that voted on, edges are who voted on whom
// take only users who went up for promotion
PSignNet TWikiElecBs::GetElecUsrVoteNet() const {
  TIntV UIdV;
  GetElecUsrV(UIdV);
  return GetVoteNet(UIdV);
}

PSignNet TWikiElecBs::GetAllUsrVoteNet() const {
  TIntV UIdV;
  GetUsrV(UIdV);
  return GetVoteNet(UIdV);
}

// nodes are users that voted on, edges are who voted on whom
PSignNet TWikiElecBs::GetVoteNet(const TIntV& UsrIdV) const {
  TIntSet UsrSet;
  for (int u = 0; u < UsrIdV.Len(); u++) {
    UsrSet.AddKey(UsrIdV[u]);
  }
  PSignNet Net = TSignNet::New();
  THash<TIntPr, TStr> EdgeElecH;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& FullE = GetElec(e);
    const int Dst = FullE.GetUId();
    if (! UsrSet.IsKey(Dst)) { continue; }
    TWikiElec NewElec;
    FullE.GetOnlyVotes(NewElec, true);
    for (int v = 0; v < NewElec.Len(); v++) {
      const int Src = NewElec[v].GetUId();
      if (! UsrSet.IsKey(Src)) { continue; }
      if (Src == Dst) { continue; }
      if (! Net->IsNode(Dst)) {
        Net->AddNode(Dst); }
      if (! Net->IsNode(Src)) {
        Net->AddNode(Src); }
      if (Net->IsEdge(Src, Dst)) { // some edges will already exis (as people go for election multiple times)
        //TStr oldRfa = EdgeElecH.GetDat(TIntPr(Src, Dst));
        //printf("Edge exists: %s   %s  \t\t  %d --> %d \tvote %d -- %d\n", oldRfa.CStr(),
        //  FullE.RfaTitle.CStr(), Src, Dst, Net->GetEDat(Src, Dst), NewElec[v].GetVote());
        //FullE.Dump(UsrH);
      }
      //EdgeElecH.AddDat(TIntPr(Src, Dst), FullE.RfaTitle);
      Net->AddEdge(Src, Dst, NewElec[v].GetVote());
    }
  }
  return Net;
}

void TWikiElecBs::GetOnlyVoteElecBs(TWikiElecBs& NewElecBs, const bool& OnlySupOpp) const {
  NewElecBs.UsrH = UsrH;
  NewElecBs.ElecV = ElecV;
  for (int e = 0; e < ElecV.Len(); e++) {
    ElecV[e].GetOnlyVotes(NewElecBs.ElecV[e], OnlySupOpp);  // only keep only actual votes
  }
}

bool TWikiElecBs::AddElecRes(const TWikiMetaHist& WMH, const THash<TStr, TStr>& UsrMapH, const THash<TStr, TWikiElecBs::TElecSum>& ElecSumH) {
  if (! WMH.Title.IsPrefix("Wikipedia:Requests_for_adminship/")) { return false; }
  const int b = WMH.Title.SearchCh('/')+1;
  const int e = WMH.Title.SearchCh('/', b+1)-1;
  TChA RfaTitle = WMH.Title.GetSubStr(b, e>b?e:TInt::Mx);
  TWikiElec WikiElec(-1, WMH.RevTm); // time of revision
  ParseVotes(WMH, UsrMapH, WikiElec);
  WikiElec.SetIsVoteFlag();
  WikiElec.RfaTitle = RfaTitle;
  TIntTr V = WikiElec.GetVotes();
  // custom ifs to correct errors
  if (RfaTitle.IsSuffix("_Couriano")) { RfaTitle = "Couriano"; } // J+¬sk+¬_Couriano --> Couriano
  if (ElecSumH.IsKey(RfaTitle)) {
    const TElecSum& ElSum = ElecSumH.GetDat(RfaTitle);
    WikiElec.IsSucc = true;
    TChA U = ElSum.Usr;
    if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
    WikiElec.UsrId = AddUsr(ElSum.Usr.CStr()); // is lower case
    if (! ElSum.Bureaucrat.Empty()) {
      TChA U = ElSum.Bureaucrat;
      if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
      WikiElec.BurUId = AddUsr(U);
    }
    if (! ElSum.NominatedBy.Empty()) {
      TChA U = ElSum.NominatedBy;
      if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
      WikiElec.NomUId = AddUsr(U);
    }
    if (abs(V.Val1-ElSum.Sup) > 0) { printf("SUP VOTES: %d != %d\n", V.Val1, ElSum.Sup);  }
    if (abs(V.Val2-ElSum.Neu) > 0) { printf("NEU VOTES: %d != %d\n", V.Val2, ElSum.Neu);  }
    if (abs(V.Val3-ElSum.Opp) > 0) { printf("OPP VOTES: %d != %d\n", V.Val3, ElSum.Opp);  }
    //if ((abs(V.Val1-ElSum.Sup)>(2+V.Val1/10)) || (abs(V.Val2-ElSum.Neu)>(2+V.Val2/10)) || (abs(V.Val3-ElSum.Opp)>(2+V.Val3/10)) ) {
    //  WikiElec.Dump(UsrH);
    //  FILE* F=fopen("page.html","wt"); fprintf(F, "%s",WMH.Text.CStr()); fclose(F);
    //  return false;
    //}
  } else {
    TChA U = RfaTitle.ToLc(); // is lower case
    if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
    WikiElec.UsrId = AddUsr(U); // is lower case
  }
  if (V.Val1+V.Val2+V.Val3 == 0) {
    printf("no votes\n"); return false;
  }
  // good election, add to the set
  ElecV.Add(WikiElec);
  return true;
}

int TWikiElecBs::GetWikiTxtLen(char* LineStr) {
  int Len=0;
  for (char *ch=LineStr+1; *ch != NULL; ch++) {
    if (TCh::IsAlNum(*ch)) { Len++; }
    if (*ch=='[' && *(ch-1)=='[') {
      while (*ch && *ch!=']' && *(ch-1)!=']') { ch++; }
    }
  }
  return Len;
}

void TWikiElecBs::LoadElecSumTxt(const TStr& FNm, THash<TStr, TWikiElecBs::TElecSum>& ElecSumH) {
  ElecSumH.Clr();
  for (TSsParser SS(FNm, ssfTabSep); SS.Next(); ) {
    IAssert(SS.Len() == 9);
    TElecSum& S = ElecSumH.AddDat(SS[1]); // RfA title
    S.Usr = SS[0];          S.Usr.ToLc();
    S.RfA = SS[1];
    S.Sup = SS.GetInt(2);
    S.Opp = SS.GetInt(3);
    S.Neu = SS.GetInt(4);
    S.NominatedBy = SS[7];  S.NominatedBy.ToLc();
    S.Bureaucrat = SS[8];   S.Bureaucrat.ToLc();
  }
}

bool EndOfSection(char* Line, const TStr& WhatStr) {
  if (Line[0]=='#' || Line[0]==':') { return false; }
  TChA LnStr(Line);
  LnStr.ToTrunc();
  // support
  if (LnStr.SearchStr(TStr::Fmt("'''%s'''", WhatStr.CStr()))!=-1) { return true; }
  if (LnStr.SearchStr(TStr::Fmt("====%s====", WhatStr.CStr()))!=-1) { return true; }
  if (LnStr.SearchStr(TStr::Fmt("==== %s ====", WhatStr.CStr()))!=-1) { return true; }
  if (LnStr.SearchStr(TStr::Fmt("'''%s:'''", WhatStr.CStr()))!=-1) { return true; }
  if (LnStr.SearchStr(TStr::Fmt("====%s:====", WhatStr.CStr()))!=-1) { return true; }
  if (LnStr.IsPrefix(TStr::Fmt("; %s", WhatStr.CStr()))) { return true; }
  if (LnStr.IsPrefix(TStr::Fmt(";%s", WhatStr.CStr()))) { return true; }
  if (LnStr==TStr::Fmt("%s", WhatStr.CStr())) { return true; }
  if (LnStr==TStr::Fmt("%s:", WhatStr.CStr())) { return true; }
  return false;
}

void TWikiElecBs::ParseVotes(const TWikiMetaHist& WMH, const THash<TStr, TStr>& UsrMapH, TWikiElec& WikiElec) {
  TVec<char *> LineV;  TChA Tmp = WMH.Text;
  Tmp.ToLc();
  TStrUtil::SplitLines(Tmp, LineV);
  int l = 0, goodVote=0;
  int NSup=0, NOpp=0, NNeu=0;
  for (; l < LineV.Len(); l++) {
    if (EndOfSection(LineV[l], "support")) { l++; break; }
    // parse "(31/1/0) ending"
    char *vote=strstr(LineV[l], ") end");
    if (vote == NULL) { vote=strstr(LineV[l], ") end"); }
    if (vote != NULL && TCh::IsNum(*(vote-1)) && NSup==-1) {
      *vote=0;  vote--;
      while(TCh::IsNum(*vote)) { vote--; }
      if (*vote == '/') {
        NNeu = atoi(vote+1);  *vote=0;  vote--;
        while(TCh::IsNum(*vote)) { vote--; }
        if (*vote == '/') {
          NOpp = atoi(vote+1);  *vote=0;  vote--;
          while(TCh::IsNum(*vote)) { vote--; }
          if (*vote == '(') { NSup = atoi(vote+1); } }
    } }//*/
  }
  TChA Usr;  TSecTm Tm;  int Indent;
  // support votes
  for (; l < LineV.Len(); l++) {
    if(GetUsrTm(LineV[l], Usr, Tm, Indent)) {  goodVote++;
    if (UsrMapH.IsKey(Usr)) { Usr = UsrMapH.GetDat(Usr);  }
      WikiElec.VoteV.Add(TWikiVote(AddUsr(Usr), +1, Indent, GetWikiTxtLen(LineV[l]), Tm));
    }
    if (EndOfSection(LineV[l], "oppose")) { l++; break; }
  }
  // oppose votes
  for (; l < LineV.Len(); l++) {
    if(GetUsrTm(LineV[l], Usr, Tm, Indent)) {  goodVote++;
      if (UsrMapH.IsKey(Usr)) { Usr = UsrMapH.GetDat(Usr); }
      WikiElec.VoteV.Add(TWikiVote(AddUsr(Usr), -1, Indent, GetWikiTxtLen(LineV[l]), Tm));
    }
    if (EndOfSection(LineV[l], "neutral")) { l++; break; }
  }
  // neutral votes
  for (; l < LineV.Len(); l++) {
    if(GetUsrTm(LineV[l], Usr, Tm, Indent)) {  goodVote++;
      if (UsrMapH.IsKey(Usr)) { Usr = UsrMapH.GetDat(Usr); printf("u"); }
      WikiElec.VoteV.Add(TWikiVote(AddUsr(Usr), 0, Indent, GetWikiTxtLen(LineV[l]), Tm));
    }
    if (LineV[l][0]!='#' && (strstr(LineV[l], "'''comments")!=NULL || strstr(LineV[l], "'''no vote")!=NULL
      || strstr(LineV[l], "====")!=NULL || strstr(LineV[l], ";comments")!=NULL || strstr(LineV[l], "; comments")!=NULL ||
      strcmp(LineV[l],"comment")==0 || strcmp(LineV[l],"comments")==0)) { l++; break; }
  }
  // sort votes over time
  //WikiElec.VoteV.Sort(); // DON'T SORT so that the order is not lost
  // dump the post if difference > 500 votes
  const TIntTr SON = WikiElec.GetVotes(true);
  if (SON.Val1+SON.Val2+SON.Val3 > 100 && (SON.Val1==0 || SON.Val2==0)) {
    //printf("EXTRACT %s (text val==extracted vals): %d=%d  %d=%d  %d=%d\n", WMH.Title.CStr(), NSup, SON.Val1, NOpp, SON.Val2, NNeu, SON.Val3);
    //WMH.Dump(true);
  }
}

void TWikiElecBs::SetUserIdFromRfa() {
  THash<TChA, TChA> RfaUsrMapH;
  RfaUsrMapH.AddDat("White_Cat_(01)", "White_Cat");
  RfaUsrMapH.AddDat("White_Cat_(02)", "White_Cat");
  RfaUsrMapH.AddDat("White_Cat_(03)", "White_Cat");
  RfaUsrMapH.AddDat("White_Cat_(04)", "White_Cat");
  RfaUsrMapH.AddDat("Computerjoe_(4)", "Computerjoe");
  RfaUsrMapH.AddDat("Purplefeltangel2", "Purplefeltangel");
  RfaUsrMapH.AddDat("Haham_Hanuka_(3)", "Haham_Hanuka");
  RfaUsrMapH.AddDat("dbertman_(archive)", "dbertman");
  RfaUsrMapH.AddDat("Agentsoo_(archive)", "Agentsoo");
  RfaUsrMapH.AddDat("Guanaco3", "Guanaco");
  RfaUsrMapH.AddDat("Brendenhull2", "Brendenhull");
  RfaUsrMapH.AddDat("Xerocs2", "Xerocs");
  RfaUsrMapH.AddDat("Wikiwoohoo2", "Wikiwoohoo");
  RfaUsrMapH.AddDat("Folajimi2", "Folajimi");
  RfaUsrMapH.AddDat("FuriousFreddy_r1", "FuriousFreddy");
  RfaUsrMapH.AddDat("General_Eisenhower3", "General_Eisenhower");
  RfaUsrMapH.AddDat("HolyRomanEmperor2", "HolyRomanEmperor");
  RfaUsrMapH.AddDat("HolyRomanEmperor3", "HolyRomanEmperor");
  RfaUsrMapH.AddDat("HolyRomanEmperor3a", "HolyRomanEmperor");
  RfaUsrMapH.AddDat("Jet123_(2nd_nom)", "Jet123");
  RfaUsrMapH.AddDat("Natl1_(2nd_nom)", "Natl1");
  RfaUsrMapH.AddDat("NickCatal2", "NickCatal");
  RfaUsrMapH.AddDat("P.B._Pilhet_2nd_Nomination", "P.B._Pilhet");
  RfaUsrMapH.AddDat("Patchouli2", "Patchouli");
  RfaUsrMapH.AddDat("Patchouli3", "Patchouli");
  RfaUsrMapH.AddDat("The_Wookieepedian2", "The_Wookieepedian");
  RfaUsrMapH.AddDat("Son_of_a_Peach_II", "Son_of_a_Peach");
  RfaUsrMapH.AddDat("SVera1NY_(second_nomination)", "SVera1NY");
  RfaUsrMapH.AddDat("Tellyaddict2", "Tellyaddict");
  RfaUsrMapH.AddDat("Tenebrae2", "Tenebrae");
  RfaUsrMapH.AddDat("Poccil2", "Poccil");
  RfaUsrMapH.AddDat("Purplefeltangel2", "Purplefeltangel");
  RfaUsrMapH.AddDat("Ramsquire2", "Ramsquire");
  RfaUsrMapH.AddDat("ScienceApologist2", "ScienceApologist");
  RfaUsrMapH.AddDat("Jor2", "Jor");
  for (int e = 0; e < Len(); e++) {
    TWikiElec& E = GetElec(e);
    if (E.IsSucc()) { continue; }
    TChA Rfa = E.RfaTitle;
    const int l = Rfa.Len();
    TChA User;
    if (l>2 && Rfa[l-2]=='_' && TCh::IsNum(Rfa[l-1])) { User = Rfa.GetSubStr(0, l-3); } // _[num]
    else if (Rfa.IsSuffix(".08") || Rfa.IsSuffix(".09") || Rfa.IsSuffix(".10")) { User = Rfa.GetSubStr(0, l-4); }
    else if (Rfa.IsSuffix("_(renomination)")) { User = Rfa.GetSubStr(0, l-(int)strlen("_(renomination)")-1); }
    else if (Rfa.IsSuffix("_(2)")) { User = Rfa.GetSubStr(0, l-5); }
    else if (Rfa.IsSuffix("_(2nd)")) { User = Rfa.GetSubStr(0, l-7); }
    else if (Rfa.IsSuffix("_(2nd_nomination)")) { User = Rfa.GetSubStr(0, l-(int)strlen("_(2nd_nomination)")-1); }
    else if (RfaUsrMapH.IsKey(Rfa)) { User = RfaUsrMapH.GetDat(Rfa); }
    else { continue; }
    User.ToLc();
    printf("%20s\t->\t%s\n", Rfa.CStr(), User.CStr());
    E.UsrId = AddUsr(User.CStr());
  }
}

void TWikiElecBs::Dump() const {
  int v=0, Sup=0, Opp=0, Neu=0;
  int VSup=0, VOpp=0, VNeu=0;
  for (int e=0; e< Len(); e++) {
    v += GetElec(e).Len();
    TIntTr Votes = GetElec(e).GetVotes(false);
    Sup += Votes.Val1;  Neu += Votes.Val2;  Opp += Votes.Val3;
    Votes = GetElec(e).GetVotes(true);
    VSup += Votes.Val1;  VNeu += Votes.Val2;  VOpp += Votes.Val3;
  }
  printf("%5d users, %3d elections, %d all votes (%d/%d/%d)\n", UsrH.Len(), Len(), v, Sup, Opp, Neu);
  printf("                             %d all votes (%d/%d/%d)\n", VSup+VOpp+VNeu, VSup, VOpp, VNeu);
  // election outcome for admins vs. non-admins
  TIntSet AdminSet, NAdminSet;
  TMom SupA, OppA, NeuA, SupNA, OppNA, NeuNA, ASupFrac, NASupFrac;
  TFltIntPrV AElecV, NAElecV;
  int nsucc=0;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    const TIntTr V = E.GetVotes(true);
    double SupFrac = V.Val1+V.Val3>0 ? V.Val1/double(V.Val1+V.Val3) : 0;
    if (V.Val1+V.Val3 == 0) { continue; }
    if (E.IsSucc) {
      AdminSet.AddKey(E.UsrId);
      SupA.Add(V.Val1());  NeuA.Add(V.Val2());  OppA.Add(V.Val3());
      AElecV.Add(TFltIntPr(SupFrac, e));
      ASupFrac.Add(SupFrac);
      nsucc++;
    } else {
      NAdminSet.AddKey(E.UsrId());
      SupNA.Add(V.Val1());  NeuNA.Add(V.Val2());  OppNA.Add(V.Val3());
      NAElecV.Add(TFltIntPr(SupFrac, e));
      NASupFrac.Add(SupFrac);
    }
  }
  printf("succ elecs %d\n", nsucc);
  SupA.Def(); OppA.Def(); NeuA.Def(); SupNA.Def(); OppNA.Def(); NeuNA.Def();
  ASupFrac.Def();  NASupFrac.Def();
  printf("succ elecs vs. failed elecs: %d admins. Votes:\n", AdminSet.Len());
  printf("  sup: %.1f : %.1f\n", SupA.GetMean(), SupNA.GetMean());
  printf("  neu:  %.1f : %.1f\n", NeuA.GetMean(), NeuNA.GetMean());
  printf("  fail: %.1f : %.1f\n", OppA.GetMean(), OppNA.GetMean());
  printf("  support fraction: %f : %f\n", ASupFrac.GetMean(), NASupFrac.GetMean());
  // votes casted by admins/non-admins/othes
  TInt ASup, AOpp, NASup, NAOpp, OSup, OOpp, AllSup, AllOpp; // sup/opp votes by admins, non-admins, others
  TInt AEl, NAEl, OEl;
  TIntPrV TmV;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    TWikiElec NewElec;  E.GetOnlyVotes(NewElec, true);
    bool IsA=false, IsNA=false, IsO=false;
    TmV.Add(TIntPr(E.GetTm().GetAbsSecs(), e));
    for (int v = 0; v < NewElec.Len(); v++) {
      const int vote = NewElec[v].GetVote();
      const int uid = NewElec[v].GetUId();
      if (AdminSet.IsKey(uid)) {
        if (vote == 1) { ASup++; } else { AOpp++; }
        IsA=true; }
      else if (NAdminSet.IsKey(uid)) {
        if (vote == 1) { NASup++; } else { NAOpp++; }
        IsNA=true; }
      else {
        if (vote == 1) { OSup++; } else { OOpp++; }
        IsO=true; }
      if (vote == 1) { AllSup++; }
      if (vote == -1) { AllOpp++; }
    }
    if (IsA) { AEl++; }
    if (IsNA) { NAEl++; }
    if (IsO) { OEl++; }
  }
  TmV.Sort();
  printf("elections from:\t%s\n\t%s\n", TSecTm(TmV[0].Val1()).GetStr().CStr(), TSecTm(TmV.Last().Val1()).GetStr().CStr());
  for (int i = 0; i <100; i++) { printf("%d\n", TmV[i].Val2); }
  for (int i = TmV.Len()-100; i <TmV.Len(); i++) { printf("%d\n", TmV[i].Val2); }
  TIntV UsrV; GetUsrV(UsrV);
  printf("voters: %d\n", UsrV.Len());
  printf("votes by:\n");
  printf("  all:         %5d : %5d = %f    elecs: %d    users: %d\n", AllSup, AllOpp, AllSup/double(AllSup+AllOpp), Len(), UsrV.Len());
  printf("  admins:      %5d : %5d = %f    elecs: %d    users: %d\n", ASup, AOpp, ASup/double(ASup+AOpp), AEl, AdminSet.Len());
  printf("  non-admins:  %5d : %5d = %f    elecs: %d    users: %d\n", NASup, NAOpp, NASup/double(NASup+NAOpp), NAEl, NAdminSet.Len());
  printf("  others:      %5d : %5d = %f    elecs: %d    users: %d\n\n", OSup, OOpp, OSup/double(OSup+OOpp), OEl, UsrH.Len()-AdminSet.Len()-NAdminSet.Len());
  /*printf("\n10 admin elections with lowest support:\n");
  AElecV.Sort(true);
  for (int e =0; e < 100; e++) {
    GetElec(AElecV[e].Val2).Dump(UsrH); }
  printf("\n10 non-admin elections with highest support:\n");
  NAElecV.Sort(false);
  for (int e =0; e < 100; e++) {
    GetElec(NAElecV[e].Val2).Dump(UsrH); }
  // */
}

// save: eid isSucc\tUsers that votes
void TWikiElecBs::SaveElecUserVotes(const TStr& OutFNm) const {
  TIntSet USet;
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10) { continue; }
    for (int v = 0; v < E.Len(); v++) {
      USet.AddKey(E[v].GetUId()); }
  }
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "IsSucc");
  for (int u = 0; u < USet.Len(); u++) {
    fprintf(F, "\tu%d", USet[u]);
  }
  fprintf(F, "\n");
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    if (E.Len() < 10) { continue; }
    TIntSet Voters;
    for (int v = 0; v < E.Len(); v++) {
      Voters.AddKey(E[v].GetUId()); }
    fprintf(F, "%d", E.IsSucc?1:0);
    for (int u = 0; u < USet.Len(); u++) {
      if (Voters.IsKey(USet[u])) { fprintf(F, "\t1"); }
      else { fprintf(F, "\t0"); }
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

void TWikiElecBs::SaveTxt(const TStr& OutFNm) {
  SortVotesByTm();

  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Wikipedia elections (http://cs.stanford.edu/people/jure/pubs/triads-chi10.pdf). Data format:\n");
  fprintf(F, "#   E: is election succesful (1) or not (0)\n");
  fprintf(F, "#   T: time election was closed\n");
  fprintf(F, "#   U: user id (and username) of editor that is being considered for promotion\n");
  fprintf(F, "#   N: user id (and username) of the nominator\n");
  fprintf(F, "#   V: <vote(1:support, 0:neutral, -1:oppose)> <user_id> <time> <username>\n");
  for (int e = 0; e < Len(); e++) {
    const TWikiElec& E = GetElec(e);
    //if (E.Len() < 10) { continue; }
    fprintf(F, "E\t%d\n", E.IsSucc?1:0);
    fprintf(F, "T\t%s\n", E.ElecTm.GetYmdTmStr().CStr());
    fprintf(F, "U\t%d\t%s\n", E.UsrId, GetUsr(E.UsrId));
    fprintf(F, "N\t%d\t%s\n", E.NomUId, E.NomUId==-1?"UNKNOWN":GetUsr(E.NomUId));
    for (int v = 0; v < E.Len(); v++) {
      if (! E[v].IsVote()) { continue; }
      fprintf(F, "V\t%d\t%d\t%s\t%s\n", E[v].GetVote(), E[v].GetUId(), E[v].GetTm().GetYmdTmStr().CStr(), GetUsr(E[v].GetUId()));
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

void TWikiElecBs::SaveOnlyVotes() {
  TWikiElecBs NewElBs;
  GetOnlyVoteElecBs(NewElBs, true);
  NewElBs.SortVotesByTm();
  TStrSet RfaSet;
  for (int e = 0; e < NewElBs.Len(); e++) {
    TWikiElec& E = NewElBs.GetElec(e);
    if (RfaSet.IsKey(E.RfaTitle)) {
      // add _# to RFA until it is unique
      for (int i = 2; ; i++) {
        TStr NewRfa = TStr::Fmt("%s_%d", E.RfaTitle.CStr(), i);
        if (! RfaSet.IsKey(NewRfa)) {
          printf("  %s --> %s\n", E.RfaTitle.CStr(), NewRfa.CStr());
          E.RfaTitle= NewRfa;
          break;
        }
      }
    }
    RfaSet.AddKey(E.RfaTitle);
  }
  NewElBs.Save(TZipOut("wikiElec-Votes.ElecBs3.rar"));
}


/*void TWikiVotes::PlotAll() {
  TGnuPlot::PlotValCntH(SupCntH, "cnt-support");
  TGnuPlot::PlotValCntH(OppCntH, "cnt-oppose");
  TGnuPlot::PlotValCntH(NeuCntH, "cnt-neutral");
  THash<TIntPr, TInt> PosNegH;
  for (int i = 0; i < VoteH.Len(); i++) {
    const TVec<TWikiVote>& V = VoteH[i];
    int pos=0, neg=0;
    for (int v = 0; v <  V.Len(); v++) {
      if (V[v].GetVote() == 1) { pos++; }
      else if (V[v].GetVote() == -1) { neg++; }
    }
    PosNegH.AddKey(TIntPr(pos, neg));
  }
  TIntPrV PrV; PosNegH.GetKeyV(PrV);
  TGnuPlot::PlotValV(PrV, "pos-neg-votes", "", "positive", "negative", gpsAuto, false, gpwPoints);
}*/

// parse: #:: tralala [[User:Renesis13|Renesis]] ([[User talk:Renesis13|talk]]) 21:39, 29 November 2006 (UTC)
bool TWikiElecBs::GetUsrTm(char* LineStr, TChA& Usr, TSecTm& Tm, int& Indent) {
  TChA TmpLine=LineStr;
  TChA SearchStr = "[[user:";
  char *UsrId = NULL;
  // find last mention of a user in a line
  for (char *next=LineStr; next=strstr(next, SearchStr.CStr()); ) {
    UsrId=next; next++; }
  if (UsrId == NULL) {
    SearchStr = "[[user talk:";
    for (char *next=LineStr; next=strstr(next, SearchStr.CStr()); ) {
      UsrId=next; next++; }
  }
  if (UsrId == NULL) {
    SearchStr = "[[user_talk:";
    for (char *next=LineStr; next=strstr(next, SearchStr.CStr()); ) {
      UsrId=next; next++; }
  }
  if (UsrId == NULL) {
    return false;
  }
  UsrId += SearchStr.Len();
  while (*UsrId && TCh::IsWs(*UsrId)) { UsrId++; }
  char *c = UsrId;
  while (*c && *c!='|' && *c!=']' && *c!='/' && *c!='#' && *c!=' ' && *c!='&') {
    if (*c==' ') { *c='_'; }
    c++;
  }
  *c = 0; c++;
  Usr = UsrId;  Usr.ToLc();
  // find date "  (UTC)---- 19:22:32, 16 Nov 2004 (UTC)"
  char *utc = strstr(c, " (utc)");
  if (utc == NULL) { return false; }
  while (strstr(utc+1, "(utc)")!=NULL) { utc = strstr(utc+1, "(utc)"); }
  *utc = 0; utc--;
  // parse backward, take 5 (YMDHM) fields + until the first space
  int FldCnt=0;
  char *e = utc;
  while (e>c && (*e==' ' || *e=='-' || *e==',')) { e--; }
  while(e>c) {
    while (e>c && ! (*e==' ' || *e=='-' || *e==',')) { e--; }
    if (++FldCnt==4) { break; }
    while (e>c && (*e==' ' || *e=='-' || *e==',')) { e--; }
  }
  e += 1;
  if (! TStrUtil::GetTmFromStr(e, Tm)) {
    return false; }
  // extract indent
  Indent=0;
  for (char *ch=LineStr; *ch && ! TCh::IsAlNum(*ch); ch++) {
    if (*ch == ':' || *ch == '*') { Indent++; }
  }
  return true;
}

// nodes are people, signed directed edge A->B means that
// A votes on B's promotion and the edge sign tells the vote
/*PWikiVoteNet TWikiElecBs::GetVoteNet() {
  PWikiVoteNet Net = TWikiVoteNet::New();
  TWikiVoteV VoteV;
  TIntV ElecIdV;  GetEIdByVotes(ElecIdV, false);
  int conflict=0, same=0;
  int sup=0, opp=0, neu=0;
  for (int u = 0; u < ElecIdV.Len(); u++) {
    const TWikiElec& E = GetElec(ElecIdV[u]);
    E.GetVotesOt(VoteV, true);
    VoteV.Sort(false);
    const int DstUId = E.UsrId;
    if (! Net->IsNode(DstUId)) { Net->AddNode(DstUId); }
    for (int v1 = 0; v1 < VoteV.Len(); v1++) {
      const int SrcUId = VoteV[v1].GetUId();
      if (VoteV[v1].GetVote() == 0) { neu++;  continue; } // skip neutral vote
      if (SrcUId == DstUId) { continue; } // skip self votes (about 150)
      if (! Net->IsNode(SrcUId)) { Net->AddNode(SrcUId); }
      Net->AddEdge(SrcUId, DstUId, VoteV[v1].GetVote());
      //Net->AddEdge(SrcUId, DstUId, TIntPr(VoteV[v1].GetVote(), ElecIdV[u]));
      if(Net->IsEdge(DstUId, SrcUId)) {
        if (Net->GetEDat(SrcUId, DstUId) != Net->GetEDat(DstUId, SrcUId)) { conflict++; }
        else { same++; }
      }
      if (VoteV[v1].GetVote()==1) { sup++; } else { opp++; }
    }
  }
  printf("votes %d users, %d elections: %d/%d/%d\n", UsrH.Len(), UsrElecH.Len(), sup, opp, neu);
  printf("both way edges %d, %d edges have same sign, %d opposite sign\n", conflict+same, same, conflict);
  printf("net: %d nodes, %d edges\n", Net->GetNodes(), Net->GetEdges());
  return Net;
}

/*
// connect nodes that vote on the same matter
// + edges: nodes tend to agree (both vote yes or no), -: nodes tend to disagree
void TWikiElecBs::GetVoteNet(TNodeEdgeNet<TInt, TInt>& Net) {
  TWikiVoteV VoteV;
  TIntV UIdV;  GetUIdByVotes(UIdV);
  THash<TIntPr, TIntTr> CmnVoteH;
  int sup=0, opp=0, neu=0;
  for (int u = 0; u < UIdV.Len(); u++) {
    const TWikiElec& E = GetElec(UIdV[u]);
    const TIntTr PON = E.GetVotesOt(VoteV, true);
    for (int v1 = 0; v1 < VoteV.Len(); v1++) {
      for (int v2 = v1+1; v2 < VoteV.Len(); v2++) {
        const int uid1= TMath::Mn(VoteV[v1].UId(), VoteV[v2].UId());
        const int uid2= TMath::Mx(VoteV[v1].UId(), VoteV[v2].UId());
        TIntTr& Vt = CmnVoteH.AddDat(TIntPr(uid1, uid2));
        if (VoteV[v1].GetVote() != VoteV[v2].GetVote()) { Vt.Val3++; }
        else if (VoteV[v1].GetVote()==1) { Vt.Val1++; }
        else if (VoteV[v1].GetVote()==-1) { Vt.Val2++; }
      }
      if (VoteV[v1].GetVote()==1) { sup++; }
      else if (VoteV[v1].GetVote()==-1) { opp++; }
      else { neu++; }
    }
  }
  printf("votes %d users, %d elections: %d/%d/%d\n", UsrH.Len(), UsrElecH.Len(), sup, opp, neu);
  TIntH CmnSupH, CmnOppH, NotCmnH;
  int supedge=0, oppedge=0, potEdges=0;
  for (int i = 0; i < CmnVoteH.Len(); i++) {
    CmnSupH.AddDat(CmnVoteH[i].Val1)++;
    CmnOppH.AddDat(CmnVoteH[i].Val2)++;
    NotCmnH.AddDat(CmnVoteH[i].Val3)++;
    if (CmnVoteH[i].Val1+CmnVoteH[i].Val2+CmnVoteH[i].Val3 > 10) {
      if ((CmnVoteH[i].Val1+CmnVoteH[i].Val2) > 1.5*CmnVoteH[i].Val3) {
        //printf("++ %d\t%d\t%d\n", CmnVoteH[i].Val1, CmnVoteH[i].Val2, CmnVoteH[i].Val3);
        supedge++;
        if (! Net.IsNode(CmnVoteH.GetKey(i).Val1)) { Net.AddNode(CmnVoteH.GetKey(i).Val1); }
        if (! Net.IsNode(CmnVoteH.GetKey(i).Val2)) { Net.AddNode(CmnVoteH.GetKey(i).Val2); }
        Net.AddEdge(CmnVoteH.GetKey(i).Val1, CmnVoteH.GetKey(i).Val2, -1, TInt(+1));
        Net.AddEdge(CmnVoteH.GetKey(i).Val2, CmnVoteH.GetKey(i).Val1, -1, TInt(+1));
      }
      if ((CmnVoteH[i].Val1+CmnVoteH[i].Val2)*1.5 < CmnVoteH[i].Val3) {
        oppedge++;
        if (! Net.IsNode(CmnVoteH.GetKey(i).Val1)) { Net.AddNode(CmnVoteH.GetKey(i).Val1); }
        if (! Net.IsNode(CmnVoteH.GetKey(i).Val2)) { Net.AddNode(CmnVoteH.GetKey(i).Val2); }
        Net.AddEdge(CmnVoteH.GetKey(i).Val1, CmnVoteH.GetKey(i).Val2, -1, TInt(-1));
        Net.AddEdge(CmnVoteH.GetKey(i).Val2, CmnVoteH.GetKey(i).Val1, -1, TInt(-1));
      }
    }
    potEdges++;
  }
  printf("supEdge: %d, oppEdge: %d, potentialEdge: %d\n", supedge, oppedge, potEdges);
  printf("net: %d nodes, %d edges\n", Net.GetNodes(), Net.GetEdges());
  TGnuPlot::PlotValCntH(CmnSupH, "common-support", "", "common support votes", "count", gpsLog10XY);
  TGnuPlot::PlotValCntH(CmnOppH, "common-oppose", "", "common oppose votes", "count", gpsLog10XY);
  TGnuPlot::PlotValCntH(NotCmnH, "common-notagree", "", "votes where decision was different", "count", gpsLog10XY);
  // count triangles
  THash<TIntPr, TInt> EdgeH;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net.BegEI(); EI < Net.EndEI(); EI++) {
    if (EI() == -1) {
      TNodeEdgeNet<TInt, TInt>::TNodeI LN = Net.GetNI(EI.GetSrcNId());
      TNodeEdgeNet<TInt, TInt>::TNodeI RN = Net.GetNI(EI.GetDstNId());
      if (LN>=RN) { continue; }
      for (int n1 = 0; n1 < LN.GetOutDeg(); n1++) {
        for (int n2 = 0; n2 < RN.GetOutDeg(); n2++) {
          if (LN.GetOutNId(n1) == RN.GetOutNId(n2)) {
            const int e1 = Net.GetEDat(LN.GetOutEId(n1));
            const int e2 = Net.GetEDat(RN.GetOutEId(n2));
            EdgeH.AddDat(TIntPr(TMath::Mn(e1, e2), TMath::Mx(e1, e2)))++;
          }
        }
      }
    }
  }
  TGPlot::PlotDegDistr(Net.GetNEGraph(), "deg-distr", "");
  for (int e = 0; e < EdgeH.Len(); e++) {
    printf("-1 %2d %2d : %d\n", EdgeH.GetKey(e).Val1, EdgeH.GetKey(e).Val2, EdgeH[e]);
  }
}

// nodes are people, signed directed edge A->B means that
// A votes on B's promotion and the edge sign tells the vote
void TWikiElecBs::GetVoteNet2(TNodeEdgeNet<TInt, TInt>& Net) {
  TWikiVoteV VoteV;
  TIntV UIdV;  GetUIdByVotes(UIdV);
  THash<TIntPr, TIntTr> CmnVoteH;
  int sup=0, opp=0, neu=0;
  for (int u = 0; u < UIdV.Len(); u++) {
    const TWikiElec& E = GetElec(UIdV[u]);
    const TIntTr PON = E.GetVotesOt(VoteV, true);
    const int DstUId = E.UsrId;
    if (! Net.IsNode(DstUId)) { Net.AddNode(DstUId); }
    for (int v1 = 0; v1 < VoteV.Len(); v1++) {
      const int SrcUId = VoteV[v1].UId();
      if (VoteV[v1].GetVote() == 0) { continue; }
      if (! Net.IsNode(SrcUId)) { Net.AddNode(SrcUId); }
      //IAssert(Net.IsEdge(SrcUId, DstUId, false));
      Net.AddEdge(SrcUId, DstUId, -1, VoteV[v1].GetVote());
      Net.AddEdge(DstUId,SrcUId, -1, VoteV[v1].GetVote());
      if (VoteV[v1].GetVote()==1) { sup++; }
      else {opp++; }
    }
  }
  printf("votes %d users, %d elections: %d/%d/%d\n", UsrH.Len(), UsrElecH.Len(), sup, opp, neu);
  printf("net: %d nodes, %d edges\n", Net.GetNodes(), Net.GetEdges());
  // count triangles
  THash<TIntTr, TInt> EdgeH;
  for (TNodeEdgeNet<TInt, TInt>::TEdgeI EI = Net.BegEI(); EI < Net.EndEI(); EI++) {
    //if (EI() == -1) {
      TNodeEdgeNet<TInt, TInt>::TNodeI LN = Net.GetNI(EI.GetSrcNId());
      TNodeEdgeNet<TInt, TInt>::TNodeI RN = Net.GetNI(EI.GetDstNId());
      if (LN>=RN) { continue; }
      for (int n1 = 0; n1 < LN.GetOutDeg(); n1++) {
        for (int n2 = 0; n2 < RN.GetOutDeg(); n2++) {
          if (LN.GetOutNId(n1) == RN.GetOutNId(n2)) {
            const int e1 = Net.GetEDat(LN.GetOutEId(n1));
            const int e2 = Net.GetEDat(RN.GetOutEId(n2));
            EdgeH.AddDat(TIntTr(EI(), TMath::Mn(e1, e2), TMath::Mx(e1, e2)))++;
          }
        }
      }
    //}
  }
  printf("sup: %d\n opp: %d\n", sup, opp);
  EdgeH.SortByKey(true);
  for (int e = 0; e < EdgeH.Len(); e++) {
    printf("%2d %2d %2d : %d\n", EdgeH.GetKey(e).Val1, EdgeH.GetKey(e).Val2, EdgeH.GetKey(e).Val3, EdgeH[e]);
  }
}*/

/////////////////////////////////////////////////
// Wikipedia Meta parser
//   loads processed version of wikipedia prepared by Gueorgi Kossinets
//   for each revision the following fields are extracted
//     REVISION  article_id rev_id article_title timestamp [ip:]username user_id
//     CATEGORY  list of categories
//     IMAGE     list of images (each listed as many times as it occurs)
//     MAIN, TALK, USER, USER_TALK, OTHER  cross-references to pages in other namespaces
//     EXTERNAL  hyperlinks to pages outside Wikipedia
//     TEMPLATE  list of all templates (each listed as many times as it occurs)
//     COMMENT   contains the comments as entered by the author
//     MINOR     whether the edit was marked as minor by the author
//     TEXTDATA  word count of revision's plain text

TWikiMetaLoader::TWikiMetaLoader(const TStr& InFNm) {
  if (TZipIn::IsZipExt(InFNm.GetFExt())) { SInPt = TZipIn::New(InFNm); }
  else if (! InFNm.Empty()) { SInPt = TFIn::New(InFNm); }
  else { SInPt = TStdIn::New(); } // empty file name loads from stdin (useful for compressed files)
}

bool TWikiMetaLoader::IsIpAddrUsr() const {
  return IsIpAddr(Usr);
}

bool TWikiMetaLoader::Next() {
  TSIn& SIn = *SInPt;
  static TChA LnStr;
  for (int rec=0; SIn.GetNextLn(LnStr); rec++) {
    if (! LnStr.IsPrefix("REVISION ")) {
      //if (LnStr.Len() > 0) { printf("TWikiMetaLoader::Next[%d]: bad line '%s'\n", rec, LnStr.CStr()); }
      continue;
    }
    TChA Tmp = LnStr;
    TVec<char *> FldV; TStrUtil::SplitWords(Tmp, FldV);
    if (FldV.Len() != 7) {
      //printf("TWikiMetaLoader::Next[%d]: bad line2 '%s'\n", rec, LnStr.CStr());
      continue;
    }
    IAssert(FldV.Len() == 7);
    ArticleId = atoi(FldV[1]);
    RevisionId = atoi(FldV[2]);
    Title = FldV[3];
    RevTm = TSecTm::GetDtTmFromStr(FldV[4]);
    Usr = FldV[5];  Usr.ToLc();  Usr.ChangeCh(' ', '_');  // usernames are in lower case
    UsrId = atoi(FldV[6]);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("CATEGORY"));
    CatStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr)); IAssert(LnStr.IsPrefix("IMAGE"));
    ImgStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("MAIN"));
    MainLStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("TALK"));
    TalkLStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("USER"));
    UserLStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("USER_TALK"));
    UserTalkLStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("OTHER"));
    OtherLStr =  LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("EXTERNAL"));
    ExternalLStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("TEMPLATE"));
    TemplateStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("COMMENT"));
    CommentStr = LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx);
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("MINOR"));
    MonorEdit = LnStr[LnStr.SearchCh(' ')+1] == '1';
    IAssert(SIn.GetNextLn(LnStr));  IAssert(LnStr.IsPrefix("TEXTDATA"));
    RevWrds = atoi(LnStr.GetSubStr(LnStr.SearchCh(' ')+1, TInt::Mx).CStr());
    return true;
  }
  return false;
}

bool TWikiMetaLoader::IsIpAddr(const TChA& Usr) {
  if (Usr.IsPrefix("ip:")) { return true; }
  int i = 0, Len=Usr.Len();
  while (i<Len && TCh::IsNum(Usr[i])) { i++; }
  if (! (i<Len && Usr[i]=='.')) { return false; }
  i++;
  while (i<Len && TCh::IsNum(Usr[i])) { i++; }
  if (! (i<Len && Usr[i]=='.')) { return false; }
  i++;
  while (i<Len && TCh::IsNum(Usr[i])) { i++; }
  if (i==Len) { return true; }
  return false;
}

/////////////////////////////////////////////////
// Wikipedia pages-meta-history parser
TWikiMetaHist::TWikiMetaHist(const TStr& InFNm) : SInPt(TZipIn::IsZipFNm(InFNm) ? TZipIn::New(InFNm) : TFIn::New(InFNm)), XmlInPt(TXmlParser::New(SInPt)), PageCnt(0) {
  printf("Init from %s\n skip till <page>...", InFNm.CStr());
  XmlInPt->SkipTillTag("page");
  printf("done\n");
}

TWikiMetaHist::TWikiMetaHist(const PSIn& SIn) : SInPt(SIn), XmlInPt() {
}

void TWikiMetaHist::Save(TSOut& SOut) const {
  SOut.Save(PageId);
  SOut.Save(RevId);
  SOut.Save(UsrId);
  RevTm.Save(SOut);
  Usr.Save(SOut);
  Cmt.Save(SOut);
  Title.Save(SOut);
  Text.Save(SOut);
}

void TWikiMetaHist::Clr() {
  PageId = RevId = UsrId = -1;
  RevTm = TSecTm();
  Usr.Clr();  Cmt.Clr();
  Title.Clr();  Text.Clr();  Tmp.Clr();
}

bool TWikiMetaHist::LoadNextBin() {
  TSIn& SIn = *SInPt;
  if (SIn.Eof()) { Clr(); return false; }
  SIn.Load(PageId);
  SIn.Load(RevId);
  SIn.Load(UsrId);
  RevTm.Load(SIn);
  Usr.Load(SIn);
  Cmt.Load(SIn);
  Title.Load(SIn);
  Text.Load(SIn);
  for (int i = 0; i < Usr.Len(); i++) {
    if (Usr[i]==' ') { Usr[i]='_'; }
  }
  for (int i = 0; i < Title.Len(); i++) {
    if (Title[i]==' ') { Title[i]='_'; }
  }
  return true;
}

bool TWikiMetaHist::LoadNextTxt() {
  IAssert(! XmlInPt.Empty());
  //Clr();  //!!! DON'T clear so that the Title remains
  TXmlParser& XmlIn = *XmlInPt;
  // skip till <page> or <revision>
  while (! (XmlIn.Sym == xsySTag && (XmlIn.SymStr == "page" || XmlIn.SymStr == "revision"))) {
    XmlIn.GetSym();
    if (XmlIn.Sym == xsyEof) { return false; }
  }
  // page info
  if (XmlIn.SymStr == "page") {
    XmlIn.GetTagVal("title", Title);
    XmlIn.GetTagVal("id", Tmp);
    PageId = atoi(Tmp.CStr());
    XmlIn.GetSym();
    if (XmlIn.SymStr != "revision") {
      while (XmlIn.SymStr != "revision") { printf("NEW_TAG:  %s\n", XmlIn.SymStr.CStr()); XmlIn.GetSym(); }
    }
  }
  EAssertR(XmlIn.SymStr == "revision", XmlIn.SymStr); // revision
  XmlIn.GetTagVal("id", Tmp);
  //printf("%s\n", Tmp.CStr());
  RevId = atoi(Tmp.CStr());          // 012345678901234567890
  XmlIn.GetTagVal("timestamp", Tmp); // 2001-01-19T01:12:51Z
  Tmp[4]=0; Tmp[7]=0; Tmp[10]=0; Tmp[13]=0; Tmp[16]=0; Tmp[19]=0;
  RevTm = TSecTm(atoi(Tmp.CStr()), atoi(Tmp.CStr()+5), atoi(Tmp.CStr()+8),
    atoi(Tmp.CStr()+11), atoi(Tmp.CStr()+14), atoi(Tmp.CStr()+17));
  EAssert(XmlIn.GetTag("contributor") == xsySTag);
  XmlIn.GetSym();
  if (XmlIn.SymStr == "ip" && XmlIn.Sym==xsySTag) {
    XmlIn.GetSym();  Usr="ip:"; Usr+=XmlIn.SymStr;
    XmlIn.GetTag("ip");  IAssert(! XmlIn.SymStr.IsPrefix("ip:")); }
  else if (XmlIn.SymStr == "username") {
    if (XmlIn.Sym == xsySTag) { XmlIn.GetSym(); Usr=XmlIn.SymStr;  EAssert(XmlIn.GetTag("username") == xsyETag); } // not-empty username tag
    else { EAssert(XmlIn.Sym == xsyETag && XmlIn.SymStr == "username"); } // empty username tag
    XmlIn.GetTagVal("id", Tmp);  UsrId = atoi(Tmp.CStr());
  }
  // remove spaces in the username
  for (int i = 0; i < Usr.Len(); i++) {
    if (Usr[i]==' ') { Usr[i]='_'; }
  }
  EAssert(XmlIn.GetTag("contributor") == xsyETag);
  //XmlIn.GetTagVal("comment", Cmt);
  XmlIn.GetSym();
  if (XmlIn.Sym == xsyETag && XmlIn.SymStr == "minor") { XmlIn.GetSym(); }
  if (XmlIn.Sym == xsySTag && XmlIn.SymStr == "comment") {
    XmlIn.GetSym();
    if (XmlIn.Sym == xsyStr) { Cmt=XmlIn.SymStr;  EAssert(XmlIn.GetTag("comment") == xsyETag); } // not-empty comment tag
    else { EAssert(XmlIn.Sym == xsyETag && XmlIn.SymStr == "comment"); } // empty comment tag
    XmlIn.GetSym();
  }
  EAssertR(XmlIn.SymStr.IsPrefix("text"), XmlIn.SymStr);
  if (XmlIn.Sym == xsySTag) {
    XmlIn.GetSym(Text);
    if (XmlIn.Sym == xsyStr) { EAssert(XmlIn.GetTag("text") == xsyETag); } // non-empty text tag
    else { EAssert(XmlIn.Sym == xsyETag && XmlIn.SymStr == "text"); } // empty text tag <text>   </text>
  }
  EAssert(XmlIn.GetTag("revision") == xsyETag);
  if (++PageCnt % 10000 == 0) {
    printf("** %dk items: %s time %f\n", PageCnt/1000, ExeTm.GetStr(), ExeTm.GetSecs()); fflush(stdout); }
  return true;
}

void TWikiMetaHist::Dump(const bool& AlsoText) const {
  printf("page: %d %s\n", PageId, Title.CStr());
  printf("rev:  %d %s\n", RevId, RevTm.GetYmdTmStr().CStr());
  printf("user: %d %s\n", UsrId, Usr.CStr());
  printf("cmt:  %s\n", Cmt.CStr());
  if (AlsoText) { printf("text:\n%s\n", Text.CStr()); }
  printf("\n");
}

void TWikiMetaHist::DumpNextXmlTags(const int& DumpN) {
  TXmlParser& XmlIn = *XmlInPt;
  for (int i = 0; i < DumpN; i++) {
    printf("%s\n", XmlIn.SymStr.CStr());
    XmlIn.GetSym();
    if (XmlIn.Sym == xsyEof) { return; }
  }
}

/*
/////////////////////////////////////////////////
// Wikipedia Voting Network
void TWikiVoteNet::PermuteOutEdgeSigns(const bool& OnlySigns) {
  printf("\nPermute out-edge signs\n");
  TIntV SignV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    SignV.Clr(false);
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (OnlySigns) {
        if (NI.GetOutEDat(i)!=0) {
          SignV.Add(NI.GetOutEDat(i)); } }
      else { SignV.Add(NI.GetOutEDat(i)); }
    }
    SignV.Shuffle(TInt::Rnd);
    for (int i = 0, s=0; i < NI.GetOutDeg(); i++) {
      if (OnlySigns) {
        if (NI.GetOutEDat(i)!=0) {
          NI.GetOutEDat(i) = SignV[s++];  } }
      else { NI.GetOutEDat(i) = SignV[s++]; }
    }
  }
}

void TWikiVoteNet::PermuteAllEdgeSigns(const bool& OnlySigns) {
  TIntV SignV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (OnlySigns) {
        if (NI.GetOutEDat(i)!=0) {
          SignV.Add(NI.GetOutEDat(i)); } }
      else { SignV.Add(NI.GetOutEDat(i)); }
    }
  }
  SignV.Shuffle(TInt::Rnd);
  int s = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (OnlySigns) {
        if (NI.GetOutEDat(i)!=0) {
          NI.GetOutEDat(i) = SignV[s++];  } }
      else { NI.GetOutEDat(i) = SignV[s++]; }

    }
  }
}

void TWikiVoteNet::CountTriads() const {
  TIntSet NbhIdSet;
  THash<TIntTr, TInt> TriadCntH;
  TIntH SignH;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    SignH.AddDat(EI()) += 1;
  }
  printf("Count triads:\nbackground sign distribution:\n");
  SignH.SortByKey(false);
  for (int i = 0; i < SignH.Len(); i++) {
    printf("\t%2d\t%d\n", SignH.GetKey(i), SignH[i]);
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const TNodeI SrcNI = GetNI(EI.GetSrcNId());
    const TNodeI DstNI = GetNI(EI.GetDstNId());
    const TInt E1Dat = EI();
    NbhIdSet.Clr(false);
    for (int es = 0; es < SrcNI.GetDeg(); es++) {
      NbhIdSet.AddKey(SrcNI.GetNbhNId(es)); }
    for (int ed = 0; ed < DstNI.GetDeg(); ed++) {
      const int nbh = DstNI.GetNbhNId(ed);
      if (! NbhIdSet.IsKey(nbh)) { continue; }
      const TInt E2Dat = SrcNI.GetNbhEDat(NbhIdSet.GetKeyId(nbh));
      const TInt E3Dat = DstNI.GetNbhEDat(ed);
      //printf("triad: %d -- %d -- %d  :  %d %d %d\n", SrcNI.GetId(), DstNI.GetId(), nbh, E1Dat, E2Dat, E3Dat);
      //TriadCntH.AddDat(E1Dat+E2Dat+E3Dat) += 1;
      TriadCntH.AddDat(TIntTr(TMath::Mx(E1Dat, E2Dat, E3Dat),
        TMath::Median(E1Dat, E2Dat, E3Dat), TMath::Mn(E1Dat, E2Dat, E3Dat))) += 1;
    }
  }
  TriadCntH.SortByKey(false);
  printf("triad counts (all counts times 3):\n");
  int SumTriad = 0, SignTriad=0;
  for (int i = 0; i < TriadCntH.Len(); i++) {
    SumTriad += TriadCntH[i];
    TIntTr SignTr = TriadCntH.GetKey(i);
    if (SignTr.Val1!=0 && SignTr.Val2!=0 && SignTr.Val3!=0) {
      SignTriad += TriadCntH[i];; }
  }
  for (int i = 0; i < TriadCntH.Len(); i++) {
    TIntTr SignTr = TriadCntH.GetKey(i);
    printf("\t%2d %2d %2d\t%8d\t%f", SignTr.Val1, SignTr.Val2, SignTr.Val3,
      TriadCntH[i], TriadCntH[i]/double(SumTriad));
    if (SignTr.Val1!=0 && SignTr.Val2!=0 && SignTr.Val3!=0) {
      printf("\t%f", TriadCntH[i]/double(SignTriad)); }
    printf("\n");
  }
}

void TWikiVoteNet::GetTwoPartScore() const {
  TIntFltH NIdScoreH, NIdNewScoreH;
  TInt::Rnd.PutSeed(0);
  double sum = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NIdScoreH.AddDat(NI.GetId(), 1-2*TInt::Rnd.GetUniDev()); }
  for (int i = 0; i < NIdScoreH.Len(); i++) { sum += fabs(NIdScoreH[i]); }
  for (int i = 0; i < NIdScoreH.Len(); i++) { NIdScoreH[i] /= sum; }

  for (int i = 0; i < 100; i++) {
    double delta = 0;
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      TMom Mom;
      double sum = 0;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const double s = NI.GetOutEDat(e) * NIdScoreH.GetDat(NI.GetOutNId(e));
        IAssert(s <= 1 && s >= -1);
        Mom.Add(s);  sum += s;
      }
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const double s = NI.GetInEDat(e)() * NIdScoreH.GetDat(NI.GetInNId(e));
        Mom.Add(s); sum += s;
      }
      Mom.Def();
      delta += fabs(NIdScoreH.GetDat(NI.GetId()) - Mom.GetMean());
      //NIdNewScoreH.AddDat(NI.GetId(), Mom.GetMean());
      NIdNewScoreH.AddDat(NI.GetId(), sum);
    }
    NIdScoreH = NIdNewScoreH;
    sum = 0;
    for (int i = 0; i < NIdScoreH.Len(); i++) { sum += fabs(NIdScoreH[i]); }
    for (int i = 0; i < NIdScoreH.Len(); i++) { NIdScoreH[i] /= sum; }
    printf("delta %f\n", delta);
  }
  TIntH ScoreH;
  for (int i = 0; i < NIdScoreH.Len(); i++) {
    ScoreH.AddDat(int(NIdScoreH[i]*100.0)) += 1;
    if (int(NIdScoreH[i]*100) != 0 || i < 100) {
      TNodeI NI = GetNI(NIdScoreH.GetKey(i));
      //printf("%g\to:%d\ti:%d\n", NIdScoreH[i], NI.GetOutDeg(), NI.GetInDeg());
    }
  }
  TGnuPlot::PlotValCntH(ScoreH, "partyScore-wiki", "Distribution of party score", "Score", "Number of people");
}

// return the number of ok edges - number of conflicting edges
// edge is ok if + edge links same sign nodes, - edge links opposite sign nodes
int TWikiVoteNet::GetEnergy() const {
  TInt::Rnd.PutSeed(0);
  TIntH NIdSignH;
  TIntV NIdV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NIdSignH.AddDat(NI.GetId(), TInt::Rnd.GetUniDev()<0.5?1:-1);
    NIdV.Add(NI.GetId());
  }
  int FulEnergy;
  for (int iters =0; iters < 20; iters++) {
    int Energy = 0, PosCnt=0;
    NIdV.Shuffle(TInt::Rnd);
    for (int n = 0; n < NIdV.Len(); n++) {
      TNodeI NI = GetNI(NIdV[n]);
      const int MySign = NIdSignH.GetDat(NI.GetId());
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        Energy += MySign * NI.GetOutEDat(e) * NIdSignH.GetDat(NI.GetOutNId(e)); }
      if (MySign == +1) { PosCnt++; }
    }
    printf("  Energy\t%d\t+%d\t-%d", Energy, PosCnt, NIdSignH.Len()-PosCnt);
    FulEnergy = Energy;
    int NFlips = 0;
    //for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int n = 0; n < NIdV.Len(); n++) {
      TNodeI NI = GetNI(NIdV[n]);
      int MyEnerg = 0;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        MyEnerg += NI.GetOutEDat(e) * NIdSignH.GetDat(NI.GetOutNId(e)); }
      int NewSign = MyEnerg < 0 ? -1 : 1;
      //if (MyEnerg == 0) { MyEnerg = TInt::Rnd.GetUniDev()<0.5?1:-1; }
      Energy = abs(MyEnerg);
      if (NIdSignH.GetDat(NI.GetId()) != NewSign) {
        NIdSignH.AddDat(NI.GetId(), NewSign);
        NFlips++;
      }
    }
    printf("\tflips\t%d\n", NFlips);
    if (NFlips == 0) { break; }
  }
  for (int n = 0; n < NIdSignH.Len(); n++) {
    //printf("  nid sign: %d:  %d\n", NIdSignH.GetKey(n), NIdSignH[n]);
  }
  return FulEnergy;
}

// keep only edges of a given sign
PWikiVoteNet TWikiVoteNet::GetEdgeSubNet(const int& EdgeDat) const {
  PWikiVoteNet Net = TWikiVoteNet::New();
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    Net->AddNode(NI.GetId(), NI.GetDat()); }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NI.GetOutEDat(e) == EdgeDat) {
        Net->AddEdge(NI.GetId(), NI.GetOutNId(e), NI.GetOutEDat(e)); }
    }
  }
  return Net;
}

PWikiVoteNet TWikiVoteNet::GetSubGraph(const TIntV& NIdV, const bool& RenumberNodes) const {
  PWikiVoteNet Net = TWikiVoteNet::New();
  IAssert(! RenumberNodes);
  for (int node = 0; node < NIdV.Len(); node++) {
    Net->AddNode(NIdV[node], GetNDat(NIdV[node])); } // also copy the node data
  for (int node = 0; node < NIdV.Len(); node++) {
    const TNodeI NI = GetNI(NIdV[node]);
    const int SrcNId = NI.GetId();
    for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
      const int OutNId = NI.GetOutNId(edge);
      if (Net->IsNode(OutNId)) {
        Net->AddEdge(SrcNId, OutNId, NI.GetOutEDat(edge)); }
    }
  }
  return Net;
}

PWikiVoteNet TWikiVoteNet::GetSignedSubNet(const bool& OnlySignEdges) const {
  TIntSet SignedSet;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (NI.GetOutEDat(i) != 0) {
        SignedSet.AddKey(NI.GetId());
        SignedSet.AddKey(NI.GetOutNId(i));
      }
    }
  }
  TIntV NIdV;
  SignedSet.GetKeyV(NIdV);
  if (! OnlySignEdges) {
    return GetSubGraph(NIdV);
  }
  PWikiVoteNet Net = TWikiVoteNet::New();
  for (int n = 0; n < NIdV.Len(); n++) {
    Net->AddNode(GetNI(NIdV[n])); }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI() != 0) {
      IAssert(Net->IsNode(EI.GetSrcNId()));
      IAssert(Net->IsNode(EI.GetDstNId()));
      Net->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), EI());
    }
  }
  return Net;
}

PWikiVoteNet TWikiVoteNet::GetSubSignGraph(const int& EdgeSign) const {
  TIntSet SignedSet;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      if (NI.GetOutEDat(i) == EdgeSign) {
        SignedSet.AddKey(NI.GetId());
        SignedSet.AddKey(NI.GetOutNId(i));
      }
    }
  }
  TIntV NIdV;
  SignedSet.GetKeyV(NIdV);
  PWikiVoteNet Net = TWikiVoteNet::New();
  for (int n = 0; n < NIdV.Len(); n++) {
    Net->AddNode(GetNI(NIdV[n])); }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (EI() == EdgeSign) {
      IAssert(Net->IsNode(EI.GetSrcNId()));
      IAssert(Net->IsNode(EI.GetDstNId()));
      Net->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), EI());
    }
  }
  return Net;
}

// plot the span of a particular edge:
//   number of common friends of the endpoints
//   path length between the endpoints
void TWikiVoteNet::PlotSignRange(const TStr& OutFNm) const {
  TFltFltH SupRngH, OppRngH; // path length
  TFltFltH SupCmnH, OppCmnH; // common friends
  THash<TFlt, TMom> RngFracH, CmnFracH; // fraction of supporters
  PWikiVoteNet ThisPt = PWikiVoteNet((TWikiVoteNet*) this);
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int C = TGAlg::GetCmnNbhs(ThisPt, EI.GetSrcNId(), EI.GetDstNId(), false);
    const int R = TGAlg::GetRangeDist(ThisPt, EI.GetSrcNId(), EI.GetDstNId(), false);
    if (EI() == 1) {
      SupRngH.AddDat(R)++;  RngFracH.AddDat(R).Add(1);
      SupCmnH.AddDat(C)++;  CmnFracH.AddDat(C).Add(1);
    } else if (EI() == -1) {
      OppRngH.AddDat(R)++;  RngFracH.AddDat(R).Add(0);
      OppCmnH.AddDat(C)++;  CmnFracH.AddDat(C).Add(0);
    }
  }
  TGnuPlot::PlotValCntH(SupRngH, "rngSup-"+OutFNm, "range of support votes", "range (path length after edge is removed)", "count");
  TGnuPlot::PlotValCntH(OppRngH, "rngOpp-"+OutFNm, "range of opposing votes", "range (path length after edge is removed)", "count");
  TGnuPlot::PlotValCntH(SupCmnH, "cmnSup-"+OutFNm, "number of common friends of support votes", "number of common friends", "count", gpsLog);
  TGnuPlot::PlotValCntH(OppCmnH, "cmnOpp-"+OutFNm, "number of common friends of opposing votes", "number of common friends", "count", gpsLog);
  TGnuPlot::PlotValMomH(RngFracH, "fracRng-"+OutFNm, "fraction of support edges spanning range X", "range", "fraction of support edges");
  TGnuPlot::PlotValMomH(CmnFracH, "fracCmn-"+OutFNm, "fraction of support edges having X common neighbors", "number of common neighbors", "fraction of support edges", gpsLog);
}

void TWikiVoteNet::SaveMatlabSparseMtx(const TStr& OutFNm, const bool& IsDir) const {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (IsDir) {
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        fprintf(F, "%d\t%d\t%d\n", NI.GetId(), NI.GetOutNId(e), NI.GetOutEDat(e).Val); }
    }
  } else {
    THash<TIntPr, TInt> EdgeH;
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        EdgeH.AddDat(TIntPr(NI.GetId(), NI.GetOutNId(e)), NI.GetOutEDat(e));
        EdgeH.AddDat(TIntPr(NI.GetOutNId(e), NI.GetId()), NI.GetOutEDat(e));
      }
    }
    EdgeH.SortByKey();
    for (int i = 0; i < EdgeH.Len(); i++) {
      fprintf(F, "%d\t%d\t%d\n", EdgeH.GetKey(i).Val1(), EdgeH.GetKey(i).Val2(), EdgeH[i].Val);
    }
  }
  fclose(F);
}

//    1---2
//     \ /|
//  4---0-3
PWikiVoteNet TWikiVoteNet::GetSmallNet() {
  PWikiVoteNet Net = TWikiVoteNet::New();
  /*for (int i = 0; i < 5; i++) { Net->AddNode(i, i); }
  Net->AddEdge(0,1, 1);
  Net->AddEdge(0,2, 1);
  Net->AddEdge(1,2, 1);
  Net->AddEdge(0,3, -1);
  Net->AddEdge(2,3, 1);
  Net->AddEdge(0,4, -1);
  return Net;
}

PWikiVoteNet TWikiVoteNet::LoadSlashdot(const TStr& InFNm, const TStr& PosEdge, const TStr& NegEdge) {
  PWikiVoteNet Net = TWikiVoteNet::New();
  TStrHash<TInt> StrNIdH;
  StrNIdH.AddKey(" ");
  TSsParser Ss(InFNm, ssfTabSep);
  while (Ss.Next()) {
    const int SrcNId = StrNIdH.AddKey(Ss[0]);
    if (! Net->IsNode(SrcNId)) {
      Net->AddNode(SrcNId, TChA(Ss[0]).ToLc()); }
    int EdgeVal = 0;
    if (PosEdge == Ss[1]) { EdgeVal=+1; }
    else if (NegEdge == Ss[1]) { EdgeVal=-1; }
    else { continue; } // only add signed edges
    for (int s = 2; s < Ss.Len(); s++) {
      const int DstNId = StrNIdH.AddKey(Ss[s]);
      if (! Net->IsNode(DstNId)) {
        Net->AddNode(DstNId, TChA(Ss[s]).ToLc()); }
      if (SrcNId != DstNId) { // skip self edges
        Net->AddEdge(SrcNId, DstNId, EdgeVal); }
    }
  }
  TSnap::PrintInfo(Net, "Slashdot network", false);
  return Net;
}

// there is a directed edge if one user edited the talk page of another user
// user and node ids between the TWikiElecBs and TWikiVoteNet match
/*PWikiVoteNet TWikiMetaHist::LoadUserTalkNet(const TStr& FNm, const TWikiElecBs& ElecBs) {
  PWikiVoteNet Net = TWikiVoteNet::New();
  TZipIn FIn(FNm);
  TStrSet UsrNIdH;
  // add nodes of users ids of people that casted votes
  for (int u = 0; u < ElecBs.GetUsrs(); u++) {
    UsrNIdH.AddKey(ElecBs.GetUsr(u));
  }
  // add remaining users and edges
  for (TChA LnStr; FIn.GetNextLn(LnStr); ) {
    if (! LnStr.IsPrefix("REVISION ")) { continue; }
    const int b = LnStr.SearchStr("User_talk:")+(int)strlen("User_talk:");
    int e1 = LnStr.SearchCh(' ', b)-1;
    int e2 = LnStr.SearchCh('/', b)-1;
    if (e1<0) { e1=TInt::Mx; }
    if (e2<0) { e2=TInt::Mx; }
    const TChA Dst = LnStr.GetSubStr(b, TMath::Mn(e1, e2)).ToLc();
    const int e = LnStr.SearchChBack(' ');
    const TChA Src = LnStr.GetSubStr(LnStr.SearchChBack(' ', e-1)+1, e-1).ToLc();
    if (Src != Dst) { // skip self edges
      const int src = UsrNIdH.AddKey(Src);
      const int dst = UsrNIdH.AddKey(Dst);
      if (! Net->IsNode(src)) { Net->AddNode(src, Src); }
      if (! Net->IsNode(dst)) { Net->AddNode(dst, Dst); }
      Net->AddEdge(src, dst);
    }
  }
  // add vote data on the edges
  printf("Set edge signs...\n");
  int nset = 0, nfound=0;
  TIntSet NodeSet;
  printf("%d elections\n", ElecBs.Len());
  for (int e = 0; e < ElecBs.Len(); e++) {
    const TWikiElec& E = ElecBs.GetElec(e);
    const int nid1 = E.GetUId();
    if (! Net->IsNode(nid1)) { continue; } // not a node
    TWikiVoteNet::TNodeI NI = Net->GetNI(nid1);
    printf("  %d,%d", NI.GetOutDeg(), NI.GetInDeg());
    nfound++;
    // set out edge signs
    NodeSet.Clr(false);
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      NodeSet.AddKey(NI.GetOutNId(i)); }
    for (int v = 0; v < E.Len(); v++) {
      const int dst = NodeSet.GetKeyId(E.GetVote(v).GetUId());
      if (Net->IsNode(E.GetVote(v).GetUId())) {
        const int id = E.GetVote(v).GetUId();
        if (Net->GetNDat(id) != ElecBs.GetUsr(id)) { printf("ERR: %s  %s\n", Net->GetNDat(id).CStr(), ElecBs.GetUsr(id)); }
      }
      if (dst != -1) { nset++;
        NI.GetOutEDat(dst) = E.GetVote(v).GetVote(); }
    }
    // also set in-edge signes (if they are not set)
    NodeSet.Clr(false);
    for (int i = 0; i < NI.GetInDeg(); i++) {
      NodeSet.AddKey(NI.GetInNId(i)); }
    for (int v = 0; v < E.Len(); v++) {
      const int dst = NodeSet.GetKeyId(E.GetVote(v).GetUId());
      if (Net->IsNode(E.GetVote(v).GetUId())) {
        const int id = E.GetVote(v).GetUId();
        if (Net->GetNDat(id) != ElecBs.GetUsr(id)) { printf("ERR: %s  %s\n", Net->GetNDat(id).CStr(), ElecBs.GetUsr(id)); }
      }
      if (dst != -1) { nset++;
        TInt& Vote = NI.GetInEDat(dst);
        if (Vote == 0) { Vote = E.GetVote(v).GetVote(); }
      }
    }
  }
  printf("\n%d admins on vote found\n", nfound);
  printf("%d edges with sign set\n\n", nset);
  TSnap::PrintInfo(Net, "WIKI USER TALK NET");
  return Net;
}*/
