#ifndef snap_memenet_h
#define snap_memenet_h

#include "Snap.h"
#include "memes.h"

class TMemeNet;
typedef TPt<TMemeNet> PMemeNet;

class TMemeNetBs;
typedef TPt<TMemeNetBs> PMemeNetBs;

/////////////////////////////////////////////////
// Meme Url network data
class TMemeNetDat {
//private:
public:
  TInt UrlId;
  TSecTm Tm;
  TIntV QtIdV;
  TUrlTy UrlTy;
public:
  TMemeNetDat() : UrlId(-1), Tm(), QtIdV(), UrlTy(utUndef) { }
  TMemeNetDat(TSIn& SIn) : UrlId(SIn), Tm(SIn), QtIdV(SIn), UrlTy(utUndef) { UrlTy=TUrlTy(TInt(SIn).Val);}
  void Save(TSOut& SOut) const { UrlId.Save(SOut); Tm.Save(SOut); QtIdV.Save(SOut); TInt(UrlTy).Save(SOut); }
  int GetUrlId() const { return UrlId; }
  TUrlTy GetUrlTy() const { return UrlTy; }
  TSecTm GetTm() const { return Tm; }
};

/////////////////////////////////////////////////
// Meme Url network
class TMemeNet : public TNodeNet<TMemeNetDat> { // node id == url id
private:
  TStr QtStr;
public:
  TMemeNet(const TStr& QuoteStr=TStr()) : QtStr(QuoteStr) { } 
  TMemeNet(TSIn& SIn) : TNet(SIn), QtStr(SIn) { }
  void Save(TSOut& SOut) { TNet::Save(SOut); QtStr.Save(SOut); }
  static PMemeNet New(const TStr& QtStr=TStr()) { return new TMemeNet(QtStr); }
  static PMemeNet Load(TSIn& SIn) { return new TMemeNet(SIn); }

  TStr GetStr() const { return QtStr; }
  int GetSameDomLinks(const PMemeNetBs& NetBs) const;
  void DelSameDomLinks(const PMemeNetBs& NetBs);

  void DrawNet(const TStr& OutFNm) const;
  void DrawNet(const TStr& OutFNm, const PMemeNetBs& MemeNetBs, const int& MinCcSize) const;
  
  friend class TPt<TMemeNet>;
};

/////////////////////////////////////////////////
// Meme Url Network Base
class TMemeNetBs { 
private:
  TCRef CRef;
  TStrHash<TInt> UrlH; // quotes and urls, data: url type
  THash<TInt, PMemeNet> QtIdNetH;
public:
  TMemeNetBs() { }
  TMemeNetBs(TSIn& SIn) : UrlH(SIn), QtIdNetH(SIn) { }
  void Save(TSOut& SOut) const { UrlH.Save(SOut); QtIdNetH.Save(SOut); }
  static PMemeNetBs New() { return new TMemeNetBs(); }
  static PMemeNetBs Load(TSIn& SIn) { return new TMemeNetBs(SIn); }
  
  int GetUrls() const { return UrlH.Len(); }
  const char *GetUrl(const int& UrlId) const { return UrlH.GetKey(UrlId); }
  TChA GetDomNm(const int& UrlId) const { return TStrUtil::GetDomNm2(UrlH.GetKey(UrlId)); }
  TChA GetWebsite(const int& UrlId) const { return TStrUtil::GetWebsite(UrlH.GetKey(UrlId)); }
  bool IsUrlId(const int& UrlId) const { return UrlId < UrlH.Len(); }
  bool IsUrl(const char* UrlStr) const { return UrlH.IsKey(UrlStr); }
  TUrlTy GetUrlTy(const int& UrlId) const { return (TUrlTy) UrlH[UrlId].Val; }
  TUrlTy GetUrlTy(const char* UrlStr) const { return (TUrlTy) UrlH.GetDat(UrlStr).Val; }
  int GetUrlId(const char* UrlStr) const { return UrlH.GetKeyId(UrlStr); }
  int AddUrl(const char* UrlStr, const TUrlTy& UrlTy) { return UrlH.AddDat(UrlStr, UrlTy); }

  int GetNets() const { return QtIdNetH.Len(); }
  TIntV GetNetByNzNodes() const;
  PMemeNet GetNet(const int& NetN) const { return QtIdNetH[NetN]; }

  void QuoteToDomainSvd() const;
  void LinkDomainSvd() const;

  void SaveNetStat(const TStr& OutFNm) const;
  
  static PMemeNetBs Build(const PQuoteBs& QtBs, const TStr& MemesFNmWc, const int& TakeTopN);
  friend class TPt<TMemeNetBs>;
};

/////////////////////////////////////////////////
// Post Net
class TBlogPostNet;
typedef TPt<TBlogPostNet> PBlogPostNet;

class TBlogPostNet : public TNGraph {
private:
  TCRef CRef;
  //TStrHash<TSecTm> UrlH; // quotes and urls, data: post time
  THash<TMd5Sig, TSecTm> UrlH; // quotes and urls, data: post time
public:
  TBlogPostNet() { }
  TBlogPostNet(TSIn& SIn) : UrlH(SIn) { }
  void Save(TSOut& SOut) const { UrlH.Save(SOut); }
  static PBlogPostNet New() { return new TBlogPostNet(); }
  static PBlogPostNet Load(TSIn& SIn) { return new TBlogPostNet(SIn); }

  int GetUrls() const { return UrlH.Len(); }
  //const char *GetUrl(const int& UrlId) const { return UrlH.GetKey(UrlId); }
  //TChA GetDomNm(const int& UrlId) const { return TStrUtil::GetDomNm2(UrlH.GetKey(UrlId)); }
  //TChA GetWebsite(const int& UrlId) const { return TStrUtil::GetWebsite(UrlH.GetKey(UrlId)); }
  //bool IsUrlId(const int& UrlId) const { return UrlId < UrlH.Len(); }
  //bool IsUrl(const char* UrlStr) const { return UrlH.IsKey(UrlStr); }
  //int GetUrlId(const char* UrlStr) const { return UrlH.GetKeyId(UrlStr); }
  //int AddUrl(const char* UrlStr, const TSecTm& PostTm) { return UrlH.AddDat(UrlStr, PostTm); }
  int GetUrlId(const char* UrlStr) const { return UrlH.GetKeyId(TMd5Sig(UrlStr)); }
  int AddUrl(const char* UrlStr, const TSecTm& PostTm) { 
    const int id=UrlH.AddKey(TMd5Sig(UrlStr)); UrlH[id]=PostTm; return id; }
  
  static PBlogPostNet BuildFromQt(const TStr& InFNmWc) {
    PBlogPostNet Net = TBlogPostNet::New();
	{ TMemesDataLoader MDL(InFNmWc);
	while (MDL.LoadNext()) { 
	  const int UId = Net->AddUrl(MDL.PostUrlStr.CStr(), MDL.PubTm);
	  Net->AddNode(UId);
	} }
    TMemesDataLoader MDL(InFNmWc);
    while (MDL.LoadNext()) {
      const int UId = Net->GetUrlId(MDL.PostUrlStr.CStr());
	  for (int l = 0; l < MDL.LinkV.Len(); l++) {
		int UId2 = Net->GetUrlId(MDL.LinkV[l].CStr());
        if (UId2 == -1) { continue; }
        Net->AddEdge(UId, UId2);
      }
    }
    TSnap::PrintInfo(Net);
    return Net;
  }
  
  friend class TPt<TBlogPostNet>;
};

#endif