#ifndef LSIONTO_H
#define LSIONTO_H

#include "mine.h"
#include "graph.h"

//////////////////////////////////////////////////////////////////////////
// Ontology-Concept
ClassTPV(TOntoConcept, POntoConcept, TOntoConceptV) //{
private:
  // parameters defining this concept
    // unique ID of this concept
    TInt Id;
    // user given name of the concept (initialized with just keywords)
    TStr CptNm;
    // documents belonging to this concept
    TIntV DIdV;
    // centroid of this concept
    TBool IsValid;
    PBowSpV ConceptSpV;
    // svm-keywords sparse vector
    PBowSpV SvmKeyWdSpV;
    // medioids of this concept (top 10 documents most similar to the centroid)
    TIntFltKdV MedoidDIdWgtV;

  // classification into ontology
    PSVMModel CptMd;

  // statistics
    // concept compactenss
    TFlt Compactness;
    // concept clarity
    TFlt Clarity;

private:
    void GenMedoid(PBowDocWgtBs BowDocWgtBs);

    UndefCopyAssign(TOntoConcept);
public:
    TOntoConcept(): Id(-1), CptNm("New Concept") { }
    
    TOntoConcept(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const int _Id, const TIntV& _DIdV);
    static POntoConcept New(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const int _Id, const TIntV& _DIdV) { 
            return new TOntoConcept(BowDocBs, BowDocWgtBs, _Id, _DIdV); }

    TOntoConcept(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const int _Id, const TStr& _CptNm, const TIntV& _DIdV);
    static POntoConcept New(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const int _Id, const TStr& _CptNm, const TIntV& _DIdV) { 
            return new TOntoConcept(BowDocBs, BowDocWgtBs, _Id, _CptNm, _DIdV); }


    // concept unique id (read-only!)
    int GetId() const { return Id; }
    // concept name
    TStr GetName() const { return CptNm; }
    void SetName(const TStr& NewCptNm) { CptNm = NewCptNm; }
    
    // centroid keywords vector
    bool IsCentroidValid() const { return IsValid; }
    PBowSpV GetCentroidSpV() const { return ConceptSpV; }
    void ResetCentroidSpV(PBowDocWgtBs BowDocWgtBs);
    // svm keywords vector
    PBowSpV GetSvmSpV() const { return SvmKeyWdSpV; }
    void SetSvmSpV(PBowSpV NewSvmSpV) { SvmKeyWdSpV = NewSvmSpV; }

    // documents
    int GetDocs() const { return DIdV.Len(); }
    int GetDId(const int& DIdN) const { return DIdV[DIdN]; }
    void GetDIdV(TIntV& _DIdV) const { _DIdV=DIdV; }
    TIntV& GetDIdVRef() { return DIdV; }
    // set new set of documents for this concept
    void SetDIdV(PBowDocWgtBs BowDocWgtBs, const TIntV& _DIdV);
    // adds given set of documents to the concept
    void AddDIdV(PBowDocWgtBs BowDocWgtBs, const TIntV& SubDIdV);
    // adds a document to the concept
    void AddDId(PBowDocWgtBs BowDocWgtBs, const int& DId);
    // removes given set of documents from the concept
    void DelDIdV(PBowDocWgtBs BowDocWgtBs, const TIntV& SubDIdV);
    // removes a document from the concept
    bool DelDId(PBowDocWgtBs BowDocWgtBs, const int& DId);

    // generates main keywords from centroid vector
    TStr GetKeyWdStr(PBowDocBs BowDocBs, const int& TopWords = 3,
      const double& TopWordsWgtPrc = 1.0, const TStr& SepStr = ", ",
      const bool& ShowWeights = false, const bool& UseMedoidP = false) const;
    // generates main keywords from centroid vector
    TStr GetSvmKeyWdStr(PBowDocBs BowDocBs, const int& TopWords = 3,
      const double& TopWordsWgtPrc = 1.0, const TStr& SepStr = ", ",
      const bool& ShowWeights = false) const;

    // prepare classifier
    void CalcCptMd(PBowDocBs BowDocBs, const TIntV& ContextDIdV,
        const double& SvmC = 1.0, const double& SvmJ = 5.0, 
        const int& SvmTime = -1);
    // has classisfier
    bool IsCptMd() const { return !CptMd.Empty(); }
    // classify document
    void CfyDocSpV(PBowSpV DocSpV, bool& IsDocInCpt, double& Prob) { 
        IsDocInCpt = CptMd->GetCfyBool(DocSpV); Prob = CptMd->GetProbCfy(DocSpV); }

    // calculate compactness
    void SetCompactness(const double& NewVal) { Compactness = NewVal; }
    double GetCompactness() const { return Compactness; }
    // calculate clarity
    void SetClarity(const double& NewVal) { Clarity = NewVal; }
    double GetClarity() const { return Clarity; }
};

//////////////////////////////////////////////////////////////////////////
// Ontology-Relation-Type
ClassTV(TOntoRltType, TOntoRltTypeV) //{
private:
    TStr TypeNm;
    TBool Directed;
    TBool Transitive;

public:
  // hard-coded relation types:
    // sub-concept of relation
    static TStr SubCptOfRltStr;
    // similar relation
    static TStr SimilarRltStr;

public:
    TOntoRltType(): TypeNm("") {}
    TOntoRltType(const TStr& _TypeNm, const bool& _Directed, const bool& _Transitive):
        TypeNm(_TypeNm), Directed(_Directed), Transitive(_Transitive) { }

    const TStr& GetName() const { return TypeNm; }
    bool IsType(const TStr& RltTypeNm) const { return (RltTypeNm == TypeNm); }
    bool IsDir() const { return Directed; }
    bool IsTransitive() const { return Transitive; }
};

//////////////////////////////////////////////////////////////////////////
// Unique-Id-Generator
ClassTP(TUniqueId, PUniqueId)//{
private:
    TInt IdCounter;
public:
    TUniqueId(const int& StartId = 0): IdCounter(StartId) { };
    static PUniqueId New(const int& StartId = 0) { return new TUniqueId(StartId); }

    TUniqueId(TSIn& SIn): IdCounter(SIn) { }
    static PUniqueId Load(TSIn &SIn) { return new TUniqueId(SIn); }
    void Save(TSOut& SOut) const { IdCounter.Save(SOut); }
    
    int GetNextId() { const int NextId = IdCounter; IdCounter++; return NextId; }
};

//////////////////////////////////////////////////////////////////////////
// Ontology-Algortihms
typedef enum { osatKMeans, osatCat, osatLwOntoCfier } TOntoSuggestAlgType;

class TOntoAlg {
public:
    static void SuggestKMeans(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, const int& _NewConceptN, PUniqueId UniqueId, 
        TOntoConceptV& NewConceptV); 

    static void SuggestCat(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, PUniqueId UniqueId, TOntoConceptV& NewConceptV);

    static void SuggestLwOntoCfier(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, PLwOntoCfier LwOntoCfier, const int& _NewConcepts, 
        PUniqueId UniqueId, TOntoConceptV& NewConceptV);
};

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter
ClassTP(TOntoExport, POntoExport)//{
public:
    virtual void StartExport() {};
    virtual void EndExport() {};

    virtual void PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, 
        const TIntV& DIdV, PBowDocBs BowDocBs, PBowSpV KeyWdSpV, 
        PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV) {};
    virtual bool IsCptSubCptLink() { return false; }
    virtual bool IsCptSuperCptLink() { return false; }
    virtual bool IsCptDocLink() { return false; }
    virtual bool IsCptAlwaysSvm() { return false; }

    virtual void PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV) {};
    virtual bool IsDocCptLink() { return false; }

    virtual void PutRltType(const TStr& RltNm, const bool& DirectedP,
        const bool& TransitiveP) {};
    virtual void PutRlt(const TStr& RltNm, const int& SrcCptId, 
        const TStr SrcCptNm, const int& DstCptId, const TStr DstCptNm) {};
    virtual bool IsRlt() { return false; }
};

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter (Proton Topic Ontology)
class TOntoExportPTO: TOntoExport {
private:
    PSOut SOut;
    bool DigLibP; 
    TStr BowFNmStr;

    TStr GetUri(const int& Id, const TStr& Nm);

public:
    TOntoExportPTO(PSOut _SOut, const bool& _DigLibP, const TStr& _BowFNmStr):
        SOut(_SOut), DigLibP(_DigLibP), BowFNmStr(_BowFNmStr) { }
    static POntoExport New(PSOut SOut, const bool& DigLibP, const TStr& BowFNmStr) { 
        return new TOntoExportPTO(SOut, DigLibP, BowFNmStr); }

    virtual void StartExport();
    virtual void EndExport();

    virtual void PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, 
        const TIntV& DIdV, PBowDocBs BowDocBs, PBowSpV KeyWdSpV, 
        PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV);
    virtual bool IsCptSubCptLink() { return false; }
    virtual bool IsCptSuperCptLink() { return true; }
    virtual bool IsCptDocLink() { return DigLibP; }
    virtual bool IsCptAlwaysSvm() { return false; }

    virtual void PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV);
    virtual bool IsDocCptLink() { return true; }
};

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter (Prolog)
class TOntoExportProlog: TOntoExport {
private:
    PSOut SOut;

    TStr GetCptLabel(const int& CptId) { return TStr::Fmt("c%d", CptId); }
    TStr GetInstLabel(const int& InstId) { return TStr::Fmt("id%d", InstId); }
public:
    TOntoExportProlog(PSOut _SOut): SOut(_SOut) { }
    static POntoExport New(PSOut SOut) { return new TOntoExportProlog(SOut); }

    virtual void StartExport();
    virtual void EndExport();

    virtual void PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, 
        const TIntV& DIdV, PBowDocBs BowDocBs, PBowSpV KeyWdSpV, 
        PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV);
    virtual bool IsCptSubCptLink() { return false; }
    virtual bool IsCptSuperCptLink() { return true; }
    virtual bool IsCptDocLink() { return false; }
    virtual bool IsCptAlwaysSvm() { return false; }

    virtual void PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV);
    virtual bool IsDocCptLink() { return true; }
};

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter (OWL)
class TOntoExportOWL: TOntoExport {
private:
    PSOut SOut;
    TBool StoreDocsP;
    TBool StoreAbstractsP;

    TStr GetUri(const int& Id, const TStr& Nm);

public:
    TOntoExportOWL(PSOut _SOut, const bool& _StoreDocsP, const bool& _StoreAbstractsP): 
      SOut(_SOut), StoreDocsP(_StoreDocsP), StoreAbstractsP(_StoreAbstractsP) { }
    static POntoExport New(PSOut SOut, const bool& StoreDocsP, const bool& StoreAbstractsP) { 
        return new TOntoExportOWL(SOut, StoreDocsP, StoreAbstractsP); }

    virtual void StartExport();
    virtual void EndExport();

    virtual void PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, 
        const TIntV& DIdV, PBowDocBs BowDocBs, PBowSpV KeyWdSpV, 
        PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV);
    virtual bool IsCptSubCptLink() { return false; }
    virtual bool IsCptSuperCptLink() { return true; }
    virtual bool IsCptDocLink() { return false; }
    virtual bool IsCptAlwaysSvm() { return false; }

    virtual void PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV);
    virtual bool IsDocCptLink() { return true; }

    virtual void PutRltType(const TStr& RltNm, const bool& DirectedP,
        const bool& TransitiveP);
    virtual void PutRlt(const TStr& RltNm, const int& SrcCptId, 
        const TStr SrcCptNm, const int& DstCptId, const TStr DstCptNm);
    virtual bool IsRlt() { return true; }
};

//////////////////////////////////////////////////////////////////////////
// Ontology
ClassTP(TOntology, POntology)//{
private:
    // document space and similarity meassure
    PBowDocBs BowDocBs;
    PBowDocWgtBs BowDocWgtBs;
    PBowSim BowSim;
    // flat list of concepts from this ontology
    TOntoConceptV ConceptV;
    // list of relation-types in this ontology
    TOntoRltTypeV RltTypeV;
    // relations between the concepts in this ontology
    PGraph OntoGraph;
    // generator of unique IDs for concepts
    PUniqueId UniqueId;


private:
    // prepares an empty ontology with root concept only
    void GenerateEmptyOnto();
    // hash table with concept ids as Key and its positions in ConceptV as Dat
    TIntH CptIdPosH;
    // refreshes CptIdPosH
    void RefreshCptIdPosH();

    // checks for any node without input-edges and connects it to root node
    void ReconnectToRoot();
    // merges lists from previous function into a set of relations (all against all) 
    // and adds them to OntoGraph if no such relation exists already
    void AddRltsFromList(TIntStrPrV& InRltV, 
        TIntStrPrV& OutRltV, TIntStrPrV& UndirRltV);
    // gathers a list of documents from all the sons, grand-sons, ...
    void AddSonDIdV(const int& CptId, TIntV& SonDIdV);
    // recurselvy classifies document into a given set of concepts and their sons
    void CfyDIdR(const int& DId, const TIntV& CptIdV, TIntFltKdV& CfyResV);

    // exports ontology using given ontology exporter
    void Export(POntoExport OntoExport);

    UndefDefaultCopyAssign(TOntology);
public:
    TOntology(PBowDocBs _BowDocBs, PBowDocWgtBs _BowDocWgtBs,
        const TBowSimType& BowSimType);
    static POntology New(PBowDocBs _BowDocBs, PBowDocWgtBs _BowDocWgtBs, 
        const TBowSimType& BowSimType) { 
            return new TOntology(_BowDocBs, _BowDocWgtBs, BowSimType); }

    PBowDocBs GetBowDocBs() const { return BowDocBs; }
    PBowDocWgtBs GetBowDocWgtBs() const { return BowDocWgtBs; }

    // number of all concepts
    int GetConcepts() const { return ConceptV.Len(); }
    // returns the Id of the CptIdN-th concept in the ontology
    int GetCptId(const int& CptIdN) const { return ConceptV[CptIdN]->GetId(); }
    // returns concept with CptId
    POntoConcept GetConcept(const int& CptId) const {
        IAssertR(IsCptId(CptId), "There is no such concept!"); 
        return ConceptV[CptIdPosH.GetDat(CptId)]; }
    // returns a list of Ids of all the concepts in the ontology
    void GetCptIdV(TIntV& CptIdV);
    // returns if CptId is an ID of existing concetp
    bool IsCptId(const int& CptId) const { return CptIdPosH.IsKey(CptId); }
    // tells if the concept id is for root concept
    bool IsRootCpt(const int& CptId) const { return CptId == 0; }
    // returns root concept
    POntoConcept GetRootCpt() const { return GetConcept(0); }

    // the name of the concept
    TStr GetCptName(const int& CptId) const { return GetConcept(CptId)->GetName(); }
    void SetCptName(const int& CptId, const TStr& NewName);
    // the number of documents in the concept CptId
    int GetCptDocs(const int& CptId) const { return GetConcept(CptId)->GetDocs(); }
    // the documents which belong to concept
    void GetCptDIdV(const int& CptId, TIntV& DIdV) const {
        GetConcept(CptId)->GetDIdV(DIdV); }
    void SetCptDIdV(const int& CptId, const TIntV& _NewDIdV, const bool& PropagateP);
    // keywords
    TStr GetCptKeyWdStr(const int& CptId, const int WordN, const bool& UseMedoidP) const {
        return GetConcept(CptId)->GetKeyWdStr(BowDocBs, WordN, 1.0, ", ", false, UseMedoidP); }
    TStr GetCptSvmKeyWdStr(const int& CptId, const int WordN) const {
        return GetConcept(CptId)->GetSvmKeyWdStr(BowDocBs, WordN); }
    // sorted vector of pairs (Similarity, CptId) for given concept
    void GetCptSimV(const int& CptId, TFltIntKdV& SimCptIdV);
    // statistics
    double GetCptCompactness(const int& CptId) const {
        return GetConcept(CptId)->GetCompactness(); }
    double GetCptClarity(const int& CptId) const {
        return GetConcept(CptId)->GetClarity(); }

    // learn classifiers for a given concept
    void CalcCptMd(const int& CptId, const double& SvmC, 
        const double& SvmJ, const int& SvmTime);
    // classify given document to concepts of the ontology starting from root
    void CfyDId(const int& DId, TIntFltKdV& CfyResV);

    // include documents from line-document format to ontology knowledge base
    void IncludeLnDocs(const TStr& LnDocFNm, TIntV& NewDIdV);
    // include documents from line-document format to ontology knowledge base
    void IncludeFolder(const TStr& FPath, TIntV& NewDIdV, const bool& RecurseDirP);
    // insert document into the given list of concepts
    void AddDocToCptV(const int& DId, const TIntV& CptIdV, const bool& UpdateCentroidP);
    //HACK recalculate BowDocWgtBs
    void ReCalcWgtBs() { BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF); }
    // reset concepts centorid keywords
    void ResetAllCptKeyWd();

    // returns a list of Ids of all the subconcepts of the given concept
    void GetSubCptIdV(const int& CptId, TIntV& SubCptIdV);
    // returns a Id of the superconcept of the given concept
    void GetSuperCptIdV(const int& CptId, TIntV& SuperCptIdV);
    // tells if a concept has a superconcept
    bool HasSuperCpt(const int& CptId) { TIntV SuperCptIdV; 
        GetSuperCptIdV(CptId, SuperCptIdV); return (SuperCptIdV.Len() > 0); }
    // number of the documents in concept CptId which are not in any of its subconcepts
    int GetCptUnusedDocs(const int& CptId) {
        TIntV DIdV; GetCptUnusedDIdV(CptId, DIdV); return DIdV.Len(); }
    // list of documents in the concept CptId which are not in any of its subconcepts
    void GetCptUnusedDIdV(const int& CptId, TIntV& UnusedDIdV);
    // list of documents in the concepts and its super-concets
    void GetCptContextDIdV(const int& CptId, TIntV& ContextDIdV, 
        const bool& AddSonDIdP = true);
    // list of documents from the sones that do not appear in the concept
    void GetInconsistDIdV(const int& CptId, TIntV& InconsistDIdV);
    // lost of similarities of the given documents and concept
    void GetCptDocSimV(const int& CptId, const TIntV& DIdV, TFltV& CptDocSimV);
    // propagate set of documents up towards the root
    void AddUpDIdV(const int& CptId, const TIntV& SubDIdV);
    // delete set of documents up towards the root
    void DelUpDIdV(const int& CptId, const TIntV& SubDIdV);
    // delete set of documents down towards the leaves
    void DelDownDIdV(const int& CptId, const TIntV& SubDIdV);
    // delete a document down towards the leaves
    void DelDownDId(const int& CptId, const int& DId);

    // makes a sorted list of all input relations
    //   elements of vector are pairs (CptId, NameOfRelation)
    void GetCptInRltV(const int& CptId, TIntStrPrV& InRltV);
    // makes a sorted list of all output relations
    //   elements of vector are pairs (CptId, NameOfRelation)
    void GetCptOutRltV(const int& CptId, TIntStrPrV& OutRltV);
    // makes a sorted list of undirected relations
    //   elements of vector are pairs (CptId, NameOfRelation)
    void GetCptUndirRltV(const int& CptId, TIntStrPrV& UndirRltV);
    // makes a sorted list of all the relations
    void GetCptRltV(const int& CptId, TIntStrPrV& InRltV, 
        TIntStrPrV& OutRltV, TIntStrPrV& UndirRltV);

    // returns the number of differnet relation types
    int GetRltTypes() const { return RltTypeV.Len(); }
    // tells if there exists a relation with a given name
    int GetRltTypeN(const TStr& RltTypeNm) const;
    // tells if RltTypeNm is a valid name for a relation-type
    bool IsRltType(const TStr& RltTypeNm) const { 
        return GetRltTypeN(RltTypeNm) != -1; }
    // tells the name if relation on RltTypeId-th place in RltTypeV
    const TStr& GetRltTypeNm(const int& RltTypeN) const { 
        return RltTypeV[RltTypeN].GetName(); }
    // tells if RltTypeNm is directed relation
    bool IsRltDir(const TStr& RltTypeNm) const {
        IAssertR(IsRltType(RltTypeNm), "There is no such relation!"); 
        return RltTypeV[GetRltTypeN(RltTypeNm)].IsDir(); }
    // tells if RltTypeNm is transitive relation
    bool IsRltTransitive(const TStr& RltTypeNm) const  {
        IAssertR(IsRltType(RltTypeNm), "There is no such relation!"); 
        return RltTypeV[GetRltTypeN(RltTypeNm)].IsTransitive(); }
    // tells if there is relation between two concepts of a given type
    bool IsRlt(const int& CptId1, const int& CptId2, const TStr& RltTypeNm);
    // adds relation between two concepts and if wanted, and relation is directed, 
    // adds all the documents from sun to the father (in case not there yet)
    void AddRlt(const int& CptId1, const int& CptId2, const TStr& RltTypeNm);
    // deletes relation between two concepts
    bool DelRlt(const int& CptId1, const int& CptId2, const TStr& RltTypeNm);

    // suggests what are the possible subconcepts of a given set of documents
    void SuggestConcepts(const TIntV& DIdV, const TOntoSuggestAlgType& AlgType,
        const int& NewConceptN, TOntoConceptV& NewConceptV);
    // suggests what are the possible subconcepts of a given concept
    void SuggestSubconcepts(const int& CptId, const TOntoSuggestAlgType& AlgType,
        const int& NewConceptN, const bool& UnusedDocsP, TOntoConceptV& NewConceptV);
    // suggests what are the possible subconcepts of a given concept
    void SuggestSubconcepts(const int& CptId, const PLwOntoCfier& LwOntoCfier,
        const int& NewConceptN, const bool& UnusedDocsP, TOntoConceptV& NewConceptV);
    // generates a concept given its name and document set
    POntoConcept GenSubconcept(const TStr& NewCptNm, const TIntV& NewCptDIdV);
    // generates a concept given a document set
    POntoConcept GenSubconcept(const TIntV& NewCptDIdV);

    // suggest names using OntoLight classifier
    void SuggestConceptNm(const int& CptId, const TVec<PLwOntoCfier>& LwOntoCfierV, 
        const int& MxSuggestNms, TStrV& SuggestNmV, TIntV& SuggestSuppV,
        TIntV& SuggestVocNV);

    // adds concept to the ontology and adds a subtopic-of relation to its father 
    void AddConcept(const POntoConcept& NewCpt, const int& FatherCptId);
    // deletes the concepts and replaces it with a list on new concepts
    // relations for the old concept are transfared to all new concepts
    void BreakConcept(const int& OldCptId, const TOntoConceptV& NewCptV);
    // deletes the concept from the list and removes all its relations 
    void DeleteConcept(const int& CptId, const bool& DoRedirect);
    // removes a subconcept from a concept
    void PruneSubconcept(const POntoConcept& SubCpt, const int& FatherCptId); 
    // TODO unites two concepts and its relations, removes relations between the two
    void UniteConcepts(const int& CptId1, const int& CptId2);
    // moves one concept from one father to the other father
    void CopyConcept(const int& CptId, const int NewFatherCptId, const bool& MoveP);
    // generates new empty concept only related (subtopic-of) to the root
    void NewConcept();

    // extracts keywords using SVM classifier which best seperate
    // given sub-concept from its mates in its father concept
    void ExtractKeywords(const int& CptId);

    // calculates concept compactness
    void CalcCptCompactness(const int& CptId);
    // calculates concept clarity
    void CalcCptClarity(const int& CptId);

    // returns the graph with concepts as nodes and relations as vertices
    PGraph GetGraph() const { return OntoGraph; }
    // prepares graph for drawing
    void PrepareGraph(const int& SelectedCptId);
    // calculates node positions
    void PlaceGraph();

    // TODO saves text-statistics of this "ontology"
    void SaveTxtStat(const TStr& FNm) { }
    
    // save structure of concepts as Proton Topic Ontology RDF file
    void ExportPTO(const TStr& FNm, const bool& DigLibP);
    // save structure of concepts as Prolog file
    void ExportProlog(const TStr& FNm);
    // save structure of concepts as OWL file
    void ExportOwl(const TStr& FNm, const bool& StoreDocsP, const bool& StoreAbstractsP);
    // load topic ontology from RDF
    static POntology ImportPTO(const TStr& RdfFNm, const TStr& BowFNm);
};

#endif