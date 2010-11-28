#ifndef snap_imdb_net_h
#define snap_imbd_net_h

#include "Snap.h"

/////////////////////////////////////////////////
// IMDB Network
class TImdbNet;
typedef TPt<TImdbNet> PImdbNet;

/////////////////////////////////////////////////
// IMDB Node
typedef enum { mtyUndef, mtyActor, mtyMovie, mtyTv, mtyV, mtyVg, mtyA,
  mtyScene, mtyMini, mtyArch, mtyVoice, mtySing, mtyAls, mtyEpisod } TMovieTy;

class TImdbNode {
private:
  TInt Name, Year, Position; // movie title or actor name
  TCh Type; TBool IsMaleAct;
  TInt Cntry, Lang;          // movie country and language
public:
  TImdbNode() : Type(mtyUndef) { }
  TImdbNode(const int& MovieName, const int& MovieYear, const TMovieTy& MovieTy);
  TImdbNode(const int& ActorName, const int& ActorYear, const int& ActorPos, const bool& IsMaleActor);
  TImdbNode(const PXmlTok& XmlTok) { Fail; }
  PXmlTok GetXmlTok() const { Fail; return NULL; }
  TImdbNode(TSIn& SIn);
  void Save(TSOut& SOut) const;

  int GetNm() const { return Name; }
  int GetYear() const { return Year; }
  int GetPos() const { return Position; }
  TMovieTy GetTy() const { return TMovieTy(Type.Val); }
  bool IsMale() const { return IsMaleAct; }
  int GetCntry() const { return Cntry; }
  int GetLang() const { return Lang; }
  bool IsActor() const { return GetTy() == mtyActor; }
  bool IsMovie() const { return GetTy() != mtyActor; }

  friend class TImdbNet;
};

/////////////////////////////////////////////////
// IMDB Network (actors-to-movies bipartite network)
class TImdbNet : public TNodeNet<TImdbNode> {
public:
  typedef TNodeNet<TImdbNode> TNet;
  typedef TPt<TNodeNet<TImdbNode> > PNet;
private:
  TStrHash<TInt> StrIdH;
public:
  TImdbNet() : StrIdH() { }
  TImdbNet(TSIn& SIn, const bool& LoadStrH=true) : TNet(SIn) {
    if (LoadStrH) StrIdH.Load(SIn); printf("load.\n"); }
  void Save(TSOut& SOut) const { TNet::Save(SOut);  StrIdH.Save(SOut); }
  static PImdbNet New() { return new TImdbNet(); }
  static PImdbNet Load(TSIn& SIn, const bool& LoadStrH=true) {
    return new TImdbNet(SIn, LoadStrH); }
  // strings
  int GetStrs() const { return StrIdH.Len(); }
  bool IsStr(const TStr& Str) const { return StrIdH.IsKey(Str); }
  bool IsStr(const TStr& Str, int& StrId) const { StrId=StrIdH.GetKeyId(Str); return StrId!=-1; }
  int GetStrId(const TStr& Str) const { return StrIdH.GetKeyId(Str); }
  TStr GetStr(const int& StrId) const { IAssert(! StrIdH.GetPool().Empty()); return StrIdH.GetKey(StrId); }
  int AddStr(const TStr& Str) { return StrIdH.AddKey(Str); }

  //PTimeNet GetTimeNet() const;
  PUNGraph GetActorGraph() const;
  void SetLangCntryByMajority();
  void SetActorCntryLangByMajority();
  static PImdbNet LoadTxt(const TStr& ActorFNm);
  static PImdbNet LoadFromImdb(const TStr& DataDir);

  static TMovieTy GetMovieTy(const TStr& MovieTyStr);
  static TStr GetMovieTyStr(const TMovieTy& MovieTy);
  friend class TPt<TImdbNet>;
};

#endif
