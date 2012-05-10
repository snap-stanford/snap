#ifndef wordnet_h
#define wordnet_h

#include "base.h"

//-----------------------------------------------------------------------------
// WordNet

/**********************************************************
WordNet 2.0 statistics:
  Unique Words: 258844 (glosses are also words)
  Synsets: 115424.  Pointers: 337984
    Noun synsets: 79689.  Pointers: 245255
    Verb synsets: 13508.  Pointers: 52753
    Adjective synsets: 18563.  Pointers: 35932
    Adverb synsets: 3664.  Pointers: 4044
  Words with a sense: 144308 (words occuring in synsets)
  Words with no sense: 114536 (primarily glosses)

  Data structure sizes:
    Word-to-Id hash: len: 258844
                   ports: 196613 ints
        string pool size: 10478469 bytes
    SynId-to-SynsetOffset: len: 115424
    SynsetStorage len: 1118251 ints
    Sense vector: len: 347455 ints
**********************************************************/

// Syntactic category
typedef enum {
    wscUndef = 0, wscNoun = 1, wscVerb = 2, wscAdj = 3, wscAdv = 4, wscMx
} TWnSynCat;

// Semantic pointer types
typedef enum {
  wptUndef = 0, //PtrSym  #define         Search
  wptAnt,        // !   ANTPTR          Antonyms
  wptHyper,      // @   HYPERPTR        Hypernyms
  wptHypo,       // ~   HYPOPTR         Hyponyms
  wptEntail,     // *   ENTAILPTR       Entailment
  wptSim,        // &   SIMPTR          Similarity

  wptIsMember,   // #m  ISMEMBERPTR     Member Holonyms
  wptIsStuff,    // #s  ISSTUFFPTR      Substance Holonyms
  wptIsPart,     // #p  ISPARTPTR       Part Holonyms

  wptHasMember,  // %m  HASMEMBERPTR    Member Meronyms
  wptHasStuff,   // %s  HASSTUFFPTR     Substance Meronyms
  wptHasPart,    // %p  HASPARTPTR      Part Meronyms

  wptMero,       // %  MERONYM          Meronyms
  wptHolo,       // #  HOLONYM          Holonyms
  wptCause,      // >  CAUSETO          Verb Cause To
  wptParticiple, // <  PPLPTR	        Participle of verb
  wptSeeAlso,    // ^  SEEALSOPTR	[NA]  Also see
  wptPert,       // \  PERTPTR	        Pertainyms
  wptAttr,       // =  ATTRIBUTE        Attributes
  wptVrbGrp,     // $  VERBGROUP  [NA]  Verb group
  wptNomin,      // +  NOMINALIZATIONS  Derivationally related form
  wptClsCat,     // ;c CLASSIFICATION   Domain of synset - CATEGORY
  wptClsReg,     // ;r CLASSIFICATION   Domain of synset - REGION
  wptClsUsg,     // ;u CLASSIFICATION   Domain of synset - USAGE
  wptMemCat,     // -c CLASS            Member of this domain - CATEGORY
  wptMemReg,     // -r CLASS            Member of this domain - REGION
  wptMemUsg,     // -u CLASS            Member of this domain - USAGE
  wptMx
} TWnPtrTy;

/////////////////////////////////////////////////
// WordNet Synset Pointer
ClassT(TSynPtr) //{
public:
  int SynId; // target synset id
  TWnPtrTy PtrTy; // pointer type
  TWnSynCat SynCat; // syntactic category of the target synset
public:
  TSynPtr() : SynId(-1), PtrTy(wptUndef), SynCat(wscUndef) { }
  TSynPtr(const int& Id, const TWnPtrTy& WnPtrTy, const TWnSynCat& WnSynCat) : SynId(Id), PtrTy(WnPtrTy), SynCat(WnSynCat) { }
  TSynPtr(const TSynPtr& Ptr) : SynId(Ptr.SynId), PtrTy(Ptr.PtrTy), SynCat(Ptr.SynCat) { }
  TSynPtr(const int& Code) { Decode(Code); }
  TSynPtr& operator = (const TSynPtr& Ptr) { if (this != &Ptr) { SynId = Ptr.SynId; PtrTy = Ptr.PtrTy; SynCat = Ptr.SynCat; }  return *this; }
  // convert TSynPtr to 4 byte integer signature: bits: 0..23 Id, 24..28 PtrTy, 29..31 SynCat
  int Code() const { return Code(SynId, PtrTy, SynCat); }
  void Decode(const int& Code) { Decode(Code, SynId, PtrTy, SynCat); }
  static int Code(const int& Id, const TWnPtrTy& PtrTy, const TWnSynCat& SynCat);
  static void Decode(const int& Code, int& Id, TWnPtrTy& PtrTy, TWnSynCat& SynCat);
  static int DecodeId(const int& Code) { return Code & 0x00ffffff; }
};

/////////////////////////////////////////////////
// WordNet
ClassTP(TWordNet, PWordNet) //{
private:
  class TSynsetStorage {
  private:
    // Synset is described by integers:  {[LexFNm, GlossStrId, SynCat, Wrds, Wrd1Id, ..., Ptrs, Ptr1, ...], ...}
    //   Wrd1Id ... words contained in synset (synonyms)
    //   Ptr1 ... integer coded TSynPtr
    TIntV Storage;
  public:
    TSynsetStorage() : Storage() { }
    TSynsetStorage(const TSynsetStorage& SynS) : Storage(SynS.Storage) { }
    TSynsetStorage(TSIn& SIn) : Storage(SIn) { }
    void Save(TSOut& SOut) const { Storage.Save(SOut); }
    TSynsetStorage& operator = (const TSynsetStorage& SynS) { if (this != &SynS) Storage = SynS.Storage; return *this; }
    int Len() const { return Storage.Len(); }

    int GetLexFNm(const int& SynOff) const { return Storage[SynOff + 0]; }
    int GetGlossId(const int& SynOff) const { return Storage[SynOff + 1]; }
    TWnSynCat GetSynCat(const int& SynOff) const { return TWnSynCat(Storage[SynOff + 2]()); }
    int GetWrds(const int& SynOff) const { return Storage[SynOff + 3]; }
    int GetPtrs(const int& SynOff) const { return Storage[SynOff + 4 + GetWrds(SynOff)]; }
    int GetWId(const int& SynOff, const int& WrdN) const { Assert(WrdN < GetWrds(SynOff)); return Storage[SynOff + 4 + WrdN]; }
    int GetPtrCode(const int& SynOff, const int& PtrN) const { Assert(PtrN < GetPtrs(SynOff)); return Storage[SynOff + 4 + GetWrds(SynOff) + 1 + PtrN]; }
    TSynPtr GetPtr(const int& SynOff, const int& PtrN) const { return TSynPtr(GetPtrCode(SynOff, PtrN)); }
    int AddSyn(const int& LexFNm, const int& GlossStrId, const TWnSynCat& SynCat, const TIntV& WrdV, const TVec<TSynPtr> SynPtrV);
  };

private:
  TStrHash<TInt> WrdToIdH; // KeyId is WordId; Dat is SenseOff (offset in SenseV)
  TIntV SynIdToOffV; // maps SynId to SynOff (synset offset to SynsetS)
  TSynsetStorage SynsetS;
  TIntV SenseV; // Word to its senses - vector of vectors {[Senses, SynId1, SynId2, ...], [...], ... }

private:
  TWordNet(const TWordNet&);
  TWordNet& operator = (const TWordNet&);
  int AddSenseV(const int& WId, const TIntV& WrdSenseV);

public:
  TWordNet() : WrdToIdH(), SynsetS(), SenseV() { }
  TWordNet(TSIn& SIn) : WrdToIdH(SIn, true), SynIdToOffV(SIn), SynsetS(SIn), SenseV(SIn) { }
  static PWordNet New() { return PWordNet(new TWordNet()); }
  static PWordNet Load(TSIn& SIn) { return PWordNet(new TWordNet(SIn)); }
  void Save(TSOut& SOut) const { WrdToIdH.Save(SOut, true);  SynIdToOffV.Save(SOut);  SynsetS.Save(SOut);  SenseV.Save(SOut); }

  bool IsSynset(const TStr& Wrd) const;
  int GetWId(const TStr& Wrd) const { return WrdToIdH.GetKeyId(Wrd.GetLc()); }
  TStr GetWrd(const int& WId) const { return WrdToIdH.GetKey(WId); }

  int GetWords() const { return WrdToIdH.Len(); }
  int GetSynsets() const { return SynIdToOffV.Len(); }
  int GetSynsets(const int& WId) const { return SenseV[WrdToIdH[WId]]; }
  int GetSynsets(const TStr& Wrd) const { int WId = WrdToIdH.GetKeyId(Wrd); if (WId == -1) return -1; else return SenseV[WrdToIdH[WId]]; }

  int GetLexFId(const int& SynId) const { return SynsetS.GetLexFNm(SynIdToOffV[SynId]); }
  TStr GetLexFNm(const int& SynId) const { return GetLxFNm(SynsetS.GetLexFNm(SynIdToOffV[SynId])); }
  int GetGlossId(const int& SynId) const { return SynsetS.GetGlossId(SynIdToOffV[SynId]); }
  TStr GetGloss(const int& SynId) const { return WrdToIdH.GetKey(SynsetS.GetGlossId(SynIdToOffV[SynId])); }
  TWnSynCat GetSynCat(const int& SynId) const { return SynsetS.GetSynCat(SynIdToOffV[SynId]); }
  int GetWords(const int& SynId) const { if (SynIdToOffV[SynId] == -1) return 0;  return SynsetS.GetWrds(SynIdToOffV[SynId]); }
  int GetWId(const int& SynId, const int& WrdN) const { return SynsetS.GetWId(SynIdToOffV[SynId], WrdN); }
  TStr GetWord(const int& SynId, const int& WrdN) const { return GetWrd(SynsetS.GetWId(SynIdToOffV[SynId], WrdN)); }

  int GetPtrs(const int& SynId) const { return SynsetS.GetPtrs(SynIdToOffV[SynId]); }
  TSynPtr GetPtr(const int& SynId, const int& PtrN) const { return SynsetS.GetPtr(SynIdToOffV[SynId], PtrN); }
  int GetSynId(const int& SynId, const int& PtrN) const { return SynsetS.GetPtr(SynIdToOffV[SynId], PtrN).SynId; }
  TWnPtrTy GetPtrTy(const int& SynId, const int& PtrN) const { return SynsetS.GetPtr(SynIdToOffV[SynId], PtrN).PtrTy; }

  void GetSynsetV(const int& WrdId, TIntV& SynIdV) const;
  void GetSynsetV(const TStr& Word, TIntV& SynIdV) const;

  void PrintSyn(const int& SynId, bool Gloss = false, int MxWrds = 5, FILE *OutF = stdout) const;
  void PrintStats(FILE *OutF = stdout) const;

  static TWnPtrTy GetPtrTy(const TStr& PtrChStr);
  static TWnSynCat GetSynCat(char SynCatCh);
  static TStr GetPtrStrDef(const TWnPtrTy& PtrTy) { return PtrStrDef[int(PtrTy)]; }
  static TStr GetPtrStr(const TWnPtrTy& PtrTy) { return PtrTyStr[int(PtrTy)]; }
  static TStr GetPtrDesc(const TWnPtrTy& PtrTy) { return PtrTyDesc[int(PtrTy)]; }
  static TStr GetSynCatStr(const TWnSynCat& SynCat) { return SynCatStr[int(SynCat)]; }
  static char GetSynCatCh(const TWnSynCat& SynCat) { return SynCatCh[int(SynCat)]; }
  static TStr GetLxFNm(const int& FlN) { return LexFNms[FlN]; }

  static PWordNet LoadWordNetTxt(const TStr& FPath);
  static void WordNetSample(const TStr& SearchWrd, const TStr& WnFNm="wn20.bin");

private:
  static char *(SynCatStr []);
  static char  SynCatCh [];
  static char *(PtrTyStr []);
  static char *(PtrTyDesc []);
  static char *(PtrStrDef []);
  static char *(LexFNms []);
};

#endif
