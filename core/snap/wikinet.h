#ifndef snap_wikinet_h
#define snap_wikinet_h

#include "Snap.h"
#include <signnet.h>

class TWikiElecBs;
class TWikiMetaHist;

class TWikiTalkNet;
typedef TPt<TWikiTalkNet> PWikiTalkNet;

/////////////////////////////////////////////////
// Wikipedia Talk Network
class TWikiUsr {
//private:
public:
  TChA Usr;
  TBool Admin;
  TSecTm ElecTm;               // time of election
  TInt BarnStars;
  TInt MnEdCnt, MnEdWrds;      // number of edits, changed words (in main namespace)
  TInt MnTkEdCnt, MnTkEdWrds;  // number of edits, changed words (in talk pages of main namespace)
  TInt WkEdCnt, WkEdWrds;      // number of edits, changed words (in wikipedia namespace)
  TInt WkTkEdCnt, WkTkEdWrds;  // number of edits, changed words (in talk pages of wikipedia namespace)
  TInt MnRevCnt, MnRevWrds;    // number of times a user verted a page (in main wikipedia space)
public:
  TWikiUsr();
  TWikiUsr(const TChA& UsrStr);
  TWikiUsr(const TWikiUsr& WikiUsr);
  TWikiUsr(TSIn& SIn);
  void Save(TSOut& SOut) const;
  const TChA& GetUsr() const { return Usr; }
  bool IsAdmin() const { return Admin; }
  bool IsElec() const { return ElecTm.IsDef(); }
  int GetStars() const { return BarnStars; }
  int GetEdCnt() const { return MnEdCnt+WkEdCnt; }
  int GetWrdCnt() const { return MnEdWrds+WkEdCnt; }
  int GetTkEdCnt() const { return MnTkEdCnt+WkTkEdCnt; }
  int GetTkWrdCnt() const { return MnTkEdWrds+WkTkEdWrds; }
  int GetAllEdCnt() const { return MnEdCnt+WkEdCnt+MnTkEdCnt+WkTkEdCnt; }
  int GetAllWrdCnt() const { return MnEdWrds+WkEdWrds+MnTkEdWrds+WkTkEdWrds; }
  int GetRevCnt() const { return MnRevCnt; }
  int GetRevWrds() const { return MnRevWrds; }
  bool operator < (const TWikiUsr& WUsr) const { 
    return strcmp(Usr.CStr(), WUsr.Usr.CStr())<0; }
  void Dump() const { 
    //printf("%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", Usr.CStr(), Admin?"A":" ", BarnStars, MnEdCnt, MnEdWrds, MnTkEdCnt, MnTkEdWrds, WkEdCnt, WkEdWrds, WkTkEdCnt, WkTkEdWrds); }
    printf("%s\n", Usr.CStr()); }
  friend class TWikiTalkNet;
};

class TWikiTalkEdge {
private:
  TInt TotTalks, TotWords, TalksBE, WordsBE, TalksAE, WordsAE; // number of talks before/after election (vote)
  TInt VoteSign; // vote is only if VoteTm.Def()==true
  TSecTm FirstTalk, LastTalk, VoteTm;
public:
  TWikiTalkEdge();
  TWikiTalkEdge(const int& _VoteSign);
  TWikiTalkEdge(const TSecTm& FTalk, const TSecTm& LTalk, const int& NTalks, const int& NWords);
  TWikiTalkEdge(const TWikiTalkEdge& Talk);
  TWikiTalkEdge(TSIn& SIn);
  void Save(TSOut& SOut) const;
  bool operator < (const TWikiTalkEdge& Edge) const { return TotTalks < Edge.TotTalks; }
  bool IsTalkE() const { return TotTalks>0; }
  bool IsVoteE() const { return VoteTm.IsDef(); }
  bool IsVoteTalkE() const { return IsTalkE()&&IsVoteE(); }
  int GetTalks() const { return TotTalks; }
  int GetWords() const { return TotWords; }
  int GetTalksBE() const { return TalksBE; }
  int GetWordsBE() const { return WordsBE; }
  int GetTalksAE() const { return TalksAE; }
  int GetWordsAE() const { return WordsAE; }
  int GetVote() const { return VoteSign; }
  TSecTm GetFTalk() const { return FirstTalk; }
  TSecTm GetLTalk() const { return LastTalk; }
  TSecTm GetVoteTm() const { return VoteTm; }
  friend class TWikiTalkNet;
};

class TWikiTalkNet : public TNodeEDatNet<TWikiUsr, TWikiTalkEdge> {
private:
  THash<TStr, TInt> UsrNIdH;
public:
  TWikiTalkNet() : TNet() { } 
  TWikiTalkNet(TSIn& SIn) : TNet(SIn), UsrNIdH(SIn) { 
    if (UsrNIdH.Empty()) { for (TNodeI NI=BegNI(); NI<EndNI(); NI++) {
      UsrNIdH.AddDat(NI().GetUsr(), NI.GetId()); } } } 
  void Save(TSOut& SOut) const { TNet::Save(SOut); UsrNIdH.Save(SOut); }
  static PWikiTalkNet New() { return new TWikiTalkNet; }
  static PWikiTalkNet Load(TSIn& SIn) { return new TWikiTalkNet(SIn); }

  int GetUsrNId(const TStr& UsrStr) const;
  bool IsUsr(const TStr& UsrStr) const;
  void PermuteAllVoteSigns(const bool& OnlyVotes);
  void PermuteOutVoteSigns(const bool& OnlyVotes);
  void CountStructBalance() const;

  PWikiTalkNet GetSubGraph(const TIntV& NIdV, const bool& RenumberNodes=false) const;
  PWikiTalkNet GetVoteSubNet(const int& VoteSign, const bool& VoteOnly, const bool& TalkOnly) const;
  PSignNet GetSignNet(const int& VoteSign, const bool& VoteOnly, const bool& TalkOnly) const;
  
  void FindPartitions(const int& NPart, const bool& OnlyMinus) const;
  void GetPartStat(const TVec<TIntV>& PartNIdV) const;
  void TestPartitions(const TStr& OutFNm);
    
  void PlotBarnStarDelta(const TStr& OutFNm) const;
  void PlotFracPosVsWords(const TStr& OutFNm) const;
  void PlotFracPosVsWords2(const TStr& OutFNm) const;
  void PlotNodeAttrDistr(const TStr& OutFNm) const;
  void PlotFracPosVsEdgeAttr(const TStr& OutFNm) const;
  void PlotVoteSignCmnFriends(const TStr& OutFNm) const;

  void SaveAreaUTrailAttr(const TStr& OutFNm, const int& MinUsrVotes, const TWikiElecBs& ElecBs) const;

  void DumpEdgeStat() const;
  void ImposeElecNet(const TWikiElecBs& ElecBs, const THash<TChA, TChA>& UsrChageH, const bool& AddVoteOnlyEdges);
  void ClearElecData();
  static PWikiTalkNet LoadTalkNet(const TStr& ParsedWikiDir, const TWikiElecBs& ElecBs);
  static PWikiTalkNet LoadSlashdot(const TStr& InFNm);
  static PWikiTalkNet LoadOldNet(const TStr& InFNm);
  friend class TPt<TWikiTalkNet>;
};

/////////////////////////////////////////////////
// Wikipedia Talk Network over time
class TWikiTimeTalkNet;
typedef TPt<TWikiTimeTalkNet> PWikiTimeTalkNet;

class TWikiTalkEdge2 {
public:
  TSecTm Tm;
  TInt Words;
public:
  TWikiTalkEdge2() { }
  TWikiTalkEdge2(const TSecTm& TalkTm, const int& TalkWords) : Tm(TalkTm), Words(TalkWords) { }
  TWikiTalkEdge2(TSIn& SIn) : Tm(SIn), Words(SIn) { }
  void Save(TSOut& SOut) const { Tm.Save(SOut); Words.Save(SOut); }
};

class TWikiTimeTalkNet : public TNodeEdgeNet<TChA, TWikiTalkEdge2> { // (talk time, number of words)
private:
  TStrHash<TInt> UsrNIdH;
public:
  TWikiTimeTalkNet() : TNet() { } 
  TWikiTimeTalkNet(TSIn& SIn) : TNet(SIn), UsrNIdH(SIn) { } 
  void Save(TSOut& SOut) const { TNet::Save(SOut); UsrNIdH.Save(SOut); }
  static PWikiTimeTalkNet New() { return new TWikiTimeTalkNet; }
  static PWikiTimeTalkNet Load(TSIn& SIn) { return new TWikiTimeTalkNet(SIn); }

  PNGraph GetBeforeTimeG(const TSecTm& EdgeTm) const;
  PSignNet GetBeforeTimeNet(const TSecTm& EdgeTm) const;
  void SaveDataset(const TWikiElecBs& ElecBs, const TStr& OutFNm) const;

  static PWikiTimeTalkNet LoadWikiTimeTalkNet();
  friend class TPt<TWikiTimeTalkNet>;
};

/////////////////////////////////////////////////
// Number of Barn Stars of a user
class TBarnStars {
private:
  THash<TStr, TSecTmV> StarsH;
public:
  TBarnStars() {
    const TStr UsrChanges = "W:\\data\\wiki20080103-parseByGuerogy\\enwiki.important-username-changes.2007-08-06";
    const TStr BarnStars = "W:\\Data\\wiki20080103-parseByGuerogy\\barnstars.history.unsorted";
    THash<TChA, TChA> UsrMapH;
    for (TSsParser Ss(UsrChanges, ssfSpaceSep); Ss.Next(); ) { 
      TChA U1=Ss[3], U2=Ss[4]; if (U1.ToLc()!=U2.ToLc()) { UsrMapH.AddDat(U1,U2); } } 
    for (TSsParser Ss(BarnStars, ssfSpaceSep); Ss.Next(); ) {
      TStr U = Ss[3]; U.ToTrunc(); U.ToLc();
      if (UsrMapH.IsKey(U)) { U = UsrMapH.GetDat(U); }
      StarsH.AddDat(U).Add(TSecTm::GetDtTmFromStr(TStr(Ss[0])+TStr(" ")+TStr(Ss[1])));
    }
    for (int u = 0; u < StarsH.Len(); u++) { StarsH[u].Sort(); }
  }
  int GetBarnStars(const TStr& Usr, const TSecTm& Tm = TSecTm()) {
    if (! StarsH.IsKey(Usr)) { return 0; }
    const TSecTmV& TmV = StarsH.GetDat(Usr);
    int Stars=0;
    for (int t = 0; t < TmV.Len(); t++) {
      if (! Tm.IsDef() || TmV[t] <= Tm) { Stars++; } }
    return Stars;
  }
};

// user edit counts
class TWikiEditCnt {
public:
  struct TEditCnt {
    TInt MainE, MainW, MainTE, MainTW;
    TInt WikiE, WikiW, WikiTE, WikiTW;
    TInt RevE, RevW; // reverts in the main namespace
    TEditCnt() { memset(this, 0, sizeof(int)); }
    TEditCnt(TSIn& SIn) { SIn.LoadBf(this, sizeof(TEditCnt)); }
    void Save(TSOut& SOut) const { SOut.SaveBf(this, sizeof(TEditCnt)); }
  };
  THash<TChA, TEditCnt> RfaEdCntH; // UId to edit count
public:
  TWikiEditCnt() { }
  TWikiEditCnt(TSIn& SIn) : RfaEdCntH(SIn) { }
  void Save(TSOut& SOut) const { RfaEdCntH.Save(SOut); }
  void LoadTxtBeforeElec(const TWikiElecBs& ElecBs);
  void LoadTxtAll(const TWikiElecBs& ElecBs);
  void SaveTxt(const TWikiElecBs& ElecBs, const TStr& OutFNm);
};

/////////////////////////////////////////////////
// Wikipedia Vote
class TWikiVote {
private:
  TInt UsrId;      // user id (see TWikiElecBs::UsrH)
  TInt UsrVote;    // -1:oppose, 0:neutral, 1:support
  TInt UsrIndent;  // indentation level of the vote
  TBool IsAVote;   // is vote or a comment
  TInt TxtLen;     // length of the text explaining the vote
  TSecTm VoteTm;
public:
  TWikiVote() { }
  TWikiVote(const int& UsrID, const int _UsrVote, const int& _UsrIndent, const int& TextLen, const TSecTm& VoteTime) : 
    UsrId(UsrID), UsrVote(_UsrVote), UsrIndent(_UsrIndent), TxtLen(TextLen), VoteTm(VoteTime) { }
  TWikiVote(TSIn& SIn) { SIn.LoadBf(this, sizeof(TWikiVote)); } 
  void Save(TSOut& SOut) const { SOut.SaveBf(this, sizeof(TWikiVote)); } 
  bool operator < (const TWikiVote& WV) const { return VoteTm<WV.VoteTm; }
  int GetUId() const { return UsrId; }
  int GetVote() const { return UsrVote; }
  bool IsVote() const { return IsAVote; }
  int GetIndent() const { return UsrIndent; }
  int GetTxtLen() const { return TxtLen; }
  TSecTm GetTm() const { return VoteTm; }
  friend class TWikiElec;
};
typedef TVec<TWikiVote> TWikiVoteV;

/////////////////////////////////////////////////
// Wikipedia Election (a vote to become an administrator)
class TWikiElec {
public:
  TStr RfaTitle;
  TInt UsrId, NomUId, BurUId; // user, nominator, bureaucrat user-id
  TBool IsSucc;    // was the election outcome successful
  TSecTm ElecTm;
  TWikiVoteV VoteV; // all votes
public:
  TWikiElec() : UsrId(-1), NomUId(-1), BurUId(-1), ElecTm() { }
  TWikiElec(const int& Usr, const TSecTm& Tm) : UsrId(Usr), NomUId(-1), BurUId(-1), ElecTm(Tm) { }
  TWikiElec(TSIn& SIn);
  void Save(TSOut& SOut) const;
  bool operator < (const TWikiElec& WE) const;
  void SetIsVoteFlag();
  
  int GetUId() const { return UsrId; }
  TSecTm GetTm() const { return ElecTm; }
  int Len() const { return VoteV.Len(); }
  const TWikiVote& GetVote(const int& VoteN) const { return VoteV[VoteN]; }
  const TWikiVote& operator [] (const int& VoteN) const { return GetVote(VoteN); }
  
  double GetFracSup(const bool& OnlyVotes=true) const;
  double GetFracSup(int VoteId1, int VoteId2) const;
  double GetTrend(int VoteId1, int VoteId2) const;
  TIntTr GetVotes(const bool& OnlyVotes=true) const;
  void GetVotesOt(TWikiVoteV& WVoteV, const bool& OnlyVotes=true) const;
  int GetAvgVoteOt(TFltV& AvgVoteV, const bool& OnlyVotes=true) const;
  int GetAvgVoteDevOt(TFltV& AvgVoteV, const bool& OnlyVotes=true) const;
  int GetRunLen(const int& VoteId) const;
  void PermuteVotes();
  void KeepVotes(const TIntSet& UIdSet);
  void GetOnlyVotes(TWikiElec& NewElec, const bool& OnlySupOpp) const;
  void RemoveSelfVotes();
  void Dump(const TStrHash<TInt>& UsrH=TStrHash<TInt>()) const;
};
typedef TVec<TWikiElec> TWikiElecV;

/////////////////////////////////////////////////
// Wikipedia Election Base
class TWikiElecBs {
public:
  struct TElecSum {
    TStr Usr, RfA, NominatedBy, Bureaucrat;
    TInt Sup, Opp, Neu;
  };
public:
  TStrHash<TInt> UsrH;      // UId (election id == user id)
  TVec<TWikiElec> ElecV;    // Vector of elections
public:
  TWikiElecBs() { }
  TWikiElecBs(TSIn& SIn) : UsrH(SIn), ElecV(SIn) { }
  void Save(TSOut& SOut) const { UsrH.Save(SOut); ElecV.Save(SOut); }
  
  int GetUsrs() const { return UsrH.Len(); }
  const char *GetUsr(const int& UId) const { return UsrH.GetKey(UId); }
  bool IsUId(const int& UId) const { return UId < UsrH.Len(); }
  bool IsUsr(const char* CStr) const { return UsrH.IsKey(CStr); }
  bool IsUsr(const TChA& Str) const { return UsrH.IsKey(Str.CStr()); }
  int GetUId(const char* CStr) const { return UsrH.GetKeyId(CStr); }
  int GetUId(const TChA& Str) const { return UsrH.GetKeyId(Str.CStr()); }
  int AddUsr(const char* CStr) { Assert(TChA(CStr)==TChA(CStr).ToLc() && TChA(CStr).SearchCh(' ')==-1);
    return UsrH.AddDatId(CStr); }
  int AddUsr(const TChA& ChA) { Assert(TChA(ChA).SearchCh(' ')==-1 && TChA(ChA)==TChA(ChA).ToLc());
    return UsrH.AddDatId(ChA.CStr()); }

  int Len() const { return ElecV.Len(); }
  int GetVotes() const { int votes=0; for (int e=0; e<Len(); e++) { votes+=GetElec(e).Len(); } return votes; }
  TWikiElec& GetElec(const int& EId) { return ElecV[EId]; }
  const TWikiElec& GetElec(const int& EId) const { return ElecV[EId]; }
  const TWikiElec& operator [] (const int& EId) const { return ElecV[EId]; }
  void GetEIdByVotes(TIntV& EIdV, const bool& AscNumVotes=true) const;
  void GetEIdByVotes(TIntV& EIdV, const int& MinLen, const double& FracPos, const double AboveFrac, const bool& AscNumVotes=true) const;
  void GetEIdByFrac(TIntV& EIdV, const int& MinLen, const double& MnFracSup, const double& MxFracSup) const;
  
  void GetUsrV(TIntV& UIdV) const;
  void GetElecUsrV(TIntV& ElecUIdV) const;
  void GetElecAdminUsrV(TIntV& ElecAdminUIdV) const;
  void GetElecNonAdminUsrV(TIntV& ElecNonAdminUIdV) const;
  void GetFqVoters(TIntSet& FqVoterSet, const int& MinVotes=10, const int& MinElecLen=0, const bool& OnlyAdmins=false) const;
  void GetUsrVotes(TIntPrV& VoteUIdV) const;
  void GetAdminSet(TIntSet& AdminSet) const;
  void GetFqVoterSet(TIntSet& FqVoterSet) const;
  void GetAdminTmSet(THash<TInt, TSecTm>& AdminSet) const;
  
  void KeepFqVoters(const int& MinVotes, const int& MinElecLen, const bool& OnlyAdmins);
  void KeepVoters(const bool& OnlyAdmins, const bool& OnlyNonAdmins);
  void KeepTopVoters(const int& Votes, const bool& KeepTop);
  int GetVoteTrails(const int& MinUsrVotes, const bool& No01Prob, TIntV& UIdV, TVec<TFltPrV>& ProbSupTmV, 
    TVec<TFltPrV>& FracSupTmV, TVec<TFltPrV>& ProbSupFracSupV, TVec<TFltPrV>& VotesTmV) const;
  void GetVoteTrails2(const int& MinUsrVotes, const bool& No01Prob, TIntV& UIdV, 
    TVec<TFltPrV>& VoteIdxFracSupV, TVec<TFltPrV>& NVotesFracSupV) const;
  void GetUsrVoteTrail(const TIntV& UIdV, TVec<TFltPrV>& ProbPosFracPosV) const;
  void GetUsrAreaUTrail(const TIntV& UIdV, TFltV& AreaV) const;
  void PermuteVotes();
  void SortVotesByTm();
  
  // new plots
  void PlotElecSupOppOt(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const;
  void PlotElecLenDistr(const TStr& OutFNm) const;
  void PlotRunLenStat(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const;
  void DrawElecTree(const TStr& OutFNm, const int& MinVotes) const;
  void PlotVotesOt(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const;
  void PlotCovotingUsers(const TStr& OutFNm, const TStr& MinSupStr, const int& TakeOnlyVotes) const;
  void PlotFracBeforeAfterVote(const TStr& OutFNm) const;
  void PlotDeltaFracSupOt(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const;
  void PlotUsrVoteVsTime(const TStr& OutFNm, const int& MinUsrVotes, const TIntSet& UsrSplitSet) const;
  void PlotFirstOppOutcome(const TStr& OutFNm, const int& NVotes) const;
  void PlotVoteTrails(const TStr& OutFNm, const int& MinUsrVotes, const bool& No01Prob) const;
  void PlotVoteTrailGlobal(const TStr& OutFNm) const;
  void PlotFinalFracVoteCnt(const TStr& OutFNm, const int& MinUsrVotes, const TIntSet& UsrSplitSet) const;
  void PlotConfusionMatrix(const TStr& OutFNm, const int& MinUsrVotes) const;
  void PlotSlopeHist(const TStr& OutFNm, const int& MinElecLen) const;

  // old plots
  void PlotBarnStarsDelta(const TStr& OutFNm) const;
  void PlotAdminVotes(const TStr& OutFNm) const;
  void PlotAvgVote(const TStr& OutFNm, const int& MinVotes, const int& MaxVotes) const;
  void PlotAvgVote(const TIntV& ElecIdV, const TStr& OutFNm, const TStr& Desc=TStr()) const;
  void PlotAvgVoteDev(const TIntV& ElecIdV, const TStr& OutFNm, const TStr& Desc=TStr()) const;
  void PlotAvgSupFrac(const TIntV& ElecIdV, const TStr& OutFNm, const TStr& Desc=TStr()) const;
  void PlotOutcomes(const TStr& OutFNm) const;
  void PlotSupFracVsElecLen(const TStr& OutFNm) const;
  void PlotSupOpp(const TStr& OutFNm) const;
  void PlotVoteDistr(const TStr& OutFNm) const;
  
  PSignNet GetAdminUsrVoteNet() const;
  PSignNet GetElecUsrVoteNet() const;
  PSignNet GetAllUsrVoteNet() const;
  PSignNet GetVoteNet(const TIntV& UsrIdV) const;
  
  void GetOnlyVoteElecBs(TWikiElecBs& NewElecBs, const bool& OnlySupOpp) const;
  bool AddElecRes(const TWikiMetaHist& WMH, const THash<TStr, TStr>& UsrMapH, const THash<TStr, TElecSum>& ElecSumH);
  void ParseVotes(const TWikiMetaHist& WMH, const THash<TStr, TStr>& UsrMapH, TWikiElec& WikiElec);
  void SetUserIdFromRfa();

  void Dump() const;
  void SaveElecUserVotes(const TStr& OutFNm) const;
  void SaveTxt(const TStr& OutFNm);
  void SaveOnlyVotes();
  static bool GetUsrTm(char* LineStr, TChA& Usr, TSecTm& Tm, int& Indent);
  static int GetWikiTxtLen(char* LineStr);
  static void LoadElecSumTxt(const TStr& FNm, THash<TStr, TElecSum>& ElecSumH);
};

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
class TWikiMetaLoader {
private:
  PSIn SInPt;
public:
  TInt ArticleId, RevisionId;
  TChA Title, Usr;
  TBool MonorEdit;
  TInt RevWrds, UsrId;
  TSecTm RevTm;
  TChA CatStr, ImgStr, TemplateStr; // list of categories, images and templates
  TChA MainLStr, TalkLStr, UserLStr, UserTalkLStr, OtherLStr, ExternalLStr; // links to other namespaces and external links
  TChA CommentStr; // categories, images, templates and comment
public:
  TWikiMetaLoader(const TStr& InFNm);
  bool IsIpAddrUsr() const;
  bool Next();
  static bool IsIpAddr(const TChA& Usr);  
};

/////////////////////////////////////////////////
// Wikipedia pages-meta-history parser
//   loads enwiki-20080103-pages-meta-history.xml.7z 
class TWikiMetaHist {
private:
  PSIn SInPt;
  PXmlParser XmlInPt;
  int PageCnt;  TExeTm ExeTm;
public:
  int PageId, RevId, UsrId;
  TSecTm RevTm;
  TChA Usr, Cmt;
  TChA Title, Text, Tmp; // page title and text
public:
  TWikiMetaHist() : XmlInPt() { }
  TWikiMetaHist(const TStr& InFNm); // for text loading
  TWikiMetaHist(const PSIn& SIn); // for binary loading
  void Save(TSOut& SOut) const;
  
  void Clr();
  bool LoadNextBin();
  bool LoadNextTxt();
  void Dump(const bool& AlsoText=false) const;
  void DumpNextXmlTags(const int& DumpN);
 // static PWikiVoteNet LoadUserTalkNet(const TStr& FNm, const TWikiElecBs& ElecBs);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TWikiElecBsOLD {
public:
  TStrHash<TInt> UsrH;
  THash<TInt, TWikiElecV> UsrElecH; // keep all versions of the election page (last is the newest)
public:
  TWikiElecBsOLD() { }
  TWikiElecBsOLD(TSIn& SIn) : UsrH(SIn), UsrElecH(SIn) { }
  void Save2(TSOut& SOut) const { UsrH.Save(SOut); UsrElecH.Save(SOut); }
  void Dump() const { TIntH ElecLen; int v=0;for (int e=0;e<UsrElecH.Len();e++) {ElecLen.AddDat(UsrElecH[e].Last().Len())++; v+=UsrElecH[e].Last().Len();}
  printf("%d users, %d elections, %d all votes\n", UsrH.Len(), UsrElecH.Len(),v);
  ElecLen.SortByKey(); for(int e=0;e<ElecLen.Len(); e++){printf("  %4d\t%d\t%d\n",e, ElecLen.GetKey(e), ElecLen[e]);} }
};
/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wikipedia Voting Network
class TWikiVoteNet : public TNodeEDatNet<TStr, TInt> {
public:
  TWikiVoteNet() { }
  TWikiVoteNet(TSIn& SIn) : TNodeEDatNet<TStr, TInt>(SIn) { }
  static PWikiVoteNet New() { return new TWikiVoteNet; }
  static PWikiVoteNet Load(TSIn& SIn) { return new TWikiVoteNet(SIn); }

  /*void PermuteOutEdgeSigns(const bool& OnlySigns);
  void PermuteAllEdgeSigns(const bool& OnlySigns);
  void CountTriads() const;

  void GetTwoPartScore() const;
  int GetEnergy() const; // how bipartite is the graph

  PWikiVoteNet GetEdgeSubNet(const int& EdgeDat) const;
  PWikiVoteNet GetSubGraph(const TIntV& NIdV, const bool& RenumberNodes=false) const;
  PWikiVoteNet GetSignedSubNet(const bool& OnlySignEdges=false) const;
  PWikiVoteNet GetSubSignGraph(const int& EdgeSign) const;

  void PlotSignRange(const TStr& OutFNm) const;

  void SaveMatlabSparseMtx(const TStr& OutFNm, const bool& IsDir=true) const;
  static PWikiVoteNet GetSmallNet();
  static PWikiVoteNet LoadSlashdot(const TStr& InFNm, const TStr& PosEdge, const TStr& NegEdge); 
  
  friend class TPt<TWikiVoteNet>;
}; */

#endif
