#ifndef wordnet_h
#define wordnet_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// WordNet-Definitions

typedef enum {wsstUndef, wsstNoun, wsstVerb, wsstAdjective,
 wsstAdjectiveSatellite, wsstAdverb, wsstMx} TWnSynSetType;

typedef enum {wrtUndef,
 wrtSenseKey, // sk(synset_id,w_num,'sense_key'). A sk operator is present for every word sense in WordNet. This gives the WordNet sense key for each word sense.
 wrtGloss, // g(synset_id,'gloss'). The g operator specifies the gloss for a synset.
 wrtSyntax, // syntax(synset_id,w_num,syntax). The syntax operator specifies the syntactic marker for a given word sense if one is specified.
 wrtHypernym, // hyp(synset_id,synset_id). The hyp operator specifies that the second synset is a hypernym of the first synset. This relation holds for nouns and verbs. The reflexive operator, hyponym, implies that the first synset is a hyponym of the second synset.
 wrtInstance, // ins(synset_id,synset_id). The ins operator specifies that the first synset is an instance of the second synset. This relation holds for nouns. The reflexive operator, has_instance, implies that the second synset is an instance of the first synset.
 wrtEntailment, // ent(synset_id,synset_id). The ent operator specifies that the second synset is an entailment of first synset. This relation only holds for verbs.
 wrtSimilar, // sim(synset_id,synset_id). The sim operator specifies that the second synset is similar in meaning to the first synset. This means that the second synset is a satellite the first synset, which is the cluster head. This relation only holds for adjective synsets contained in adjective clusters.
 wrtMemberMeronym, // mm(synset_id,synset_id). The mm operator specifies that the second synset is a member meronym of the first synset. This relation only holds for nouns. The reflexive operator, member holonym, can be implied.
 wrtSubstanceMeronym, // ms(synset_id,synset_id). The ms operator specifies that the second synset is a substance meronym of the first synset. This relation only holds for nouns. The reflexive operator, substance holonym, can be implied.
 wrtPartMeronym, // mp(synset_id,synset_id). The mp operator specifies that the second synset is a part meronym of the first synset. This relation only holds for nouns. The reflexive operator, part holonym, can be implied.
 wrtDerivation, // der(synset_id,synset_id). The der operator specifies that there exists a reflexive lexical morphosemantic relation between the first and second synset terms representing derivational morphology.
 wrtClass, // cls(synset_id,w_num,synset_id,w_num,class_type). The cls operator specifies that the first synset has been classified as a member of the class represented by the second synset. Either of the w_num's can be 0, reflecting that the pointer is semantic in the original WordNet database.
 wrtCause, // cs(synset_id,synset_id). The cs operator specifies that the second synset is a cause of the first synset. This relation only holds for verbs.
 wrtVerbGroup, // vgp(synset_id,w_num,synset_id,w_num). The vgp operator specifies verb synsets that are similar in meaning and should be grouped together when displayed in response to a grouped synset search.
 wrtAttribute, // at(synset_id,synset_id). The at operator defines the attribute relation between noun and adjective synset pairs in which the adjective is a value of the noun. For each pair, both relations are listed (ie. each synset_id is both a source and target).
 wrtAntonym, // ant(synset_id,w_num,synset_id,w_num). The ant operator specifies antonymous word s. This is a lexical relation that holds for all syntactic categories. For each antonymous pair, both relations are listed (ie. each synset_id,w_num pair is both a source and target word.)
 wrtAdditionalInfo, // sa(synset_id,w_num,synset_id,w_num). The sa operator specifies that additional information about the first word can be obtained by seeing the second word. This operator is only defined for verbs and adjectives. There is no reflexive relation (ie. it cannot be inferred that the additional information about the second word can be obtained from the first word).
 wrtParticiple, // ppl(synset_id,w_num,synset_id,w_num). The ppl operator specifies that the adjective first word is a participle of the verb second word. The reflexive operator can be implied.
 wrtPosRel, // per(synset_id,w_num,synset_id,w_num). The per operator specifies two different relations based on the parts of speech involved. If the first word is in an adjective synset, that word pertains to either the noun or adjective second word. If the first word is in an adverb synset, that word is derived from the adjective second word.
 wrtFrame, // fr(synset_id,f_num,w_num). The fr operator specifies a generic sentence frame for one or all words in a synset. The operator is defined only for verbs.
 wrtMx} TWnRelType;

/////////////////////////////////////////////////
// WordNet-SynSet
ClassTP(TWnSynSet, PWnSynSet)//{
public:
  TIntV WordStrPV;
  TInt SynSetType; // TWnSynSetType
  TIntTrV RelIntIntTrV;
  TIntTrV InvRelIntIntTrV;
  UndefCopy(TWnSynSet);
public:
  TWnSynSet():
    WordStrPV(), SynSetType(wsstUndef),
    RelIntIntTrV(), InvRelIntIntTrV(){}
  static PWnSynSet New(){return new TWnSynSet();}
  TWnSynSet(TSIn& SIn):
    WordStrPV(SIn), SynSetType(SIn),
    RelIntIntTrV(SIn), InvRelIntIntTrV(SIn){}
  static PWnSynSet Load(TSIn& SIn){return new TWnSynSet(SIn);}
  void Save(TSOut& SOut){
    WordStrPV.Save(SOut); SynSetType.Save(SOut);
    RelIntIntTrV.Save(SOut); InvRelIntIntTrV.Save(SOut);}

  TWnSynSet& operator=(const TWnSynSet& WnSynSet){
    if (this!=&WnSynSet){
      WordStrPV=WnSynSet.WordStrPV; SynSetType=WnSynSet.SynSetType;
      RelIntIntTrV=WnSynSet.RelIntIntTrV;
      InvRelIntIntTrV=WnSynSet.InvRelIntIntTrV;}
    return *this;}

  int GetWords() const {return WordStrPV.Len();}
  int GetWordStrP(const int& WordN) const {return WordStrPV[WordN];}

  void AddRel(const TWnRelType& RelType, const int& SrcSynSetP, const int& DstSynSetP){
    RelIntIntTrV.AddUnique(TIntTr(RelType, SrcSynSetP, DstSynSetP));}
  void AddInvRel(const int& RelType, const int& SrcSynSetP, const int& DstSynSetP){
    InvRelIntIntTrV.AddUnique(TIntTr(RelType, SrcSynSetP, DstSynSetP));}
  void GetDstSynSetPV(const TWnRelType& RelType, TIntV& DstSynSetPV) const;
};
typedef TStrHash<PWnSynSet> TIdToWnSynSetSH;

/////////////////////////////////////////////////
// WordNet-Base
ClassTP(TWnBs, PWnBs)//{
private:
  TIdToWnSynSetSH IdToWnSynSetSH;
  TStrToIntVSH EWordStrToSynSetPVSH;
  UndefAssign(TWnBs);
public:
  TWnBs(): IdToWnSynSetSH(), EWordStrToSynSetPVSH(){}
  static PWnBs New(){return new TWnBs();}
  TWnBs(const TWnBs& WnBs):
    IdToWnSynSetSH(WnBs.IdToWnSynSetSH),
    EWordStrToSynSetPVSH(WnBs.EWordStrToSynSetPVSH){}
  ~TWnBs(){}
  TWnBs(TSIn& SIn):
    IdToWnSynSetSH(SIn), EWordStrToSynSetPVSH(SIn){}
  static PWnBs Load(TSIn& SIn){return new TWnBs(SIn);}
  void Save(TSOut& SOut){
    IdToWnSynSetSH.Save(SOut); EWordStrToSynSetPVSH.Save(SOut);}

  // synsets
  int GetSynSets() const {return IdToWnSynSetSH.Len();}
  bool IsSynSetId(const TStr& SynSetId) const {return IdToWnSynSetSH.IsKey(SynSetId);}
  TStr GetSynSetId(const int& SynSetP) const {return IdToWnSynSetSH.GetKey(SynSetP);}
  int GetSynSetP(const TStr& SynSetId) const {return IdToWnSynSetSH.GetKeyId(SynSetId);}
  PWnSynSet GetSynSet(const TStr& SynSetId){return IdToWnSynSetSH.GetDat(SynSetId);}
  PWnSynSet GetSynSetFromP(const int& SynSetP){return IdToWnSynSetSH[SynSetP];}

  // extended-words
  int GetEWords() const {return EWordStrToSynSetPVSH.Len();}
  TStr GetEWordStrP(const int& EWordP) const {return EWordStrToSynSetPVSH.GetKey(EWordP);}
  static TStr GetEWordStr(const TStr WordStr, const TStr& SynSetTypeStr){
    return WordStr+","+SynSetTypeStr;}
  static TWnSynSetType GetSynSetTypeFromStr(const TStr& SynSetTypeStr);

  // relations
  void AddRel(const TWnRelType& RelType, const TStr& SrcSynSetId, const TStr& DstSynSetId);
  static TStr GetRelTypeNm(const TWnRelType& RelType);

  // prolog-base
  void LoadFromPlBs(const PPlBs& PlBs);

  // binary files
  static PWnBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}

  // text file
  void SaveTxtSynSet(const int& SynSetP, FILE* fOut, const int& LevelN=0);
  void SaveTxtRel(
   const TWnRelType& RelType, const int& SynSetP, const bool& Recurse, FILE* fOut,
   const int& LevelN=0, TIntS& SynSetPS=TIntS());
  void SaveTxt(const TStr& FNm);
};

#endif
