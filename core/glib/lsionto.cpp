#include "lsionto.h"

//////////////////////////////////////////////////////////////////////////
// Ontology-Concept
void TOntoConcept::GenMedoid(PBowDocWgtBs BowDocWgtBs) {
    // calculate similarity of documents towards centroid
    TFltIntKdV WgtDIdV;
    for (int DIdN = 0; DIdN < DIdV.Len(); DIdN++) {
        const int DId = DIdV[DIdN];
        PBowSpV DocSpV = BowDocWgtBs->GetSpV(DId);
        const double Wgt = TBowSim::GetCosSim(ConceptSpV, DocSpV);
        WgtDIdV.Add(TFltIntKd(Wgt, DId));
    }
    // get top TopN mendoids
    const int TopN = 33;
    TFltIntKdV TopWgtDIdV;
    if (WgtDIdV.Len() > TopN) {
        TFltIntKdHp::GetTopV(htMin, TopN, WgtDIdV, TopWgtDIdV, true);
    } else { 
        TopWgtDIdV = WgtDIdV; TopWgtDIdV.Sort(false);
    }
    // store TopN medoids
    GetSwitchedKdV<TFlt, TInt>(TopWgtDIdV, MedoidDIdWgtV);
}

TOntoConcept::TOntoConcept(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const int _Id, const TIntV& _DIdV): Id(_Id), DIdV(_DIdV)  { 
    
    ResetCentroidSpV(BowDocWgtBs);
    CptNm = GetKeyWdStr(BowDocBs, 3, 1.0, ", ", false, false);
    SvmKeyWdSpV = TBowSpV::New();
    if (!DIdV.IsSorted()) { DIdV.Sort(); }
    Compactness = Clarity = -1.0;
};

TOntoConcept::TOntoConcept(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, const int _Id, 
        const TStr& _CptNm, const TIntV& _DIdV): Id(_Id), CptNm(_CptNm), DIdV(_DIdV) { 
    
    ResetCentroidSpV(BowDocWgtBs);
    SvmKeyWdSpV = TBowSpV::New();
    if (!DIdV.IsSorted()) { DIdV.Sort(); }
    Compactness = Clarity = -1.0;
};

void TOntoConcept::ResetCentroidSpV(PBowDocWgtBs BowDocWgtBs) {
    ConceptSpV = TBowClust::GetConceptSpV(BowDocWgtBs, NULL, DIdV);
    GenMedoid(BowDocWgtBs);
    IsValid = true;
}

void TOntoConcept::SetDIdV(PBowDocWgtBs BowDocWgtBs, const TIntV& _DIdV) {
    if (Id == 0) return; // we can not change the documents in root concept!
    DIdV = _DIdV; if (!DIdV.IsSorted()) { DIdV.Sort(); }
    ResetCentroidSpV(BowDocWgtBs);
}

void TOntoConcept::AddDIdV(PBowDocWgtBs BowDocWgtBs, const TIntV& SubDIdV) {
    DIdV.Union(SubDIdV);
    if (!BowDocWgtBs.Empty()) { ResetCentroidSpV(BowDocWgtBs); }
    else { IsValid = false; }
}

void TOntoConcept::AddDId(PBowDocWgtBs BowDocWgtBs, const int& DId) {
    if (DIdV.SearchBin(DId) == -1) { DIdV.AddSorted(DId); }
    if (!BowDocWgtBs.Empty()) { ResetCentroidSpV(BowDocWgtBs); }
    else { IsValid = false; }
}

void TOntoConcept::DelDIdV(PBowDocWgtBs BowDocWgtBs, const TIntV& SubDIdV) {
    if (Id == 0) return; // we can not delete the documents in root concept!
    DIdV.Diff(SubDIdV);
    if (!BowDocWgtBs.Empty()) { ResetCentroidSpV(BowDocWgtBs); }
    else { IsValid = false; }
}

bool TOntoConcept::DelDId(PBowDocWgtBs BowDocWgtBs, const int& DId) {
    if (Id == 0) return false; // we can not delete the documents in root concept!
    bool DocDeletedP = DIdV.DelIfIn(DId);
    if (DocDeletedP && !BowDocWgtBs.Empty()) { ResetCentroidSpV(BowDocWgtBs); }
    else { IsValid = false; }
    return DocDeletedP;
}

// generates main keywords from centroid vector
TStr TOntoConcept::GetKeyWdStr(PBowDocBs BowDocBs, const int& TopWords, 
        const double& TopWordsWgtPrc, const TStr& SepStr, 
        const bool& ShowWeights, const bool& UseMedoidP) const { 

    if (!UseMedoidP) {
        return ConceptSpV->GetStr(BowDocBs, TopWords, TopWordsWgtPrc, SepStr, ShowWeights); 
    } else {
        if (MedoidDIdWgtV.Empty()) { return "Empty concept"; }
        TStr KeyWdStr = BowDocBs->GetDocNm(MedoidDIdWgtV[0].Key);
        const int DIds = TInt::GetMn(TopWords, MedoidDIdWgtV.Len());
        for (int DIdN = 1; DIdN < DIds; DIdN++) {
            const int DId = MedoidDIdWgtV[DIdN].Key;
            TStr DocNm = BowDocBs->GetDocNm(DId);
            KeyWdStr += SepStr; KeyWdStr += DocNm;
        }
        return KeyWdStr;
    }
}

// generates main keywords from centroid vector
TStr TOntoConcept::GetSvmKeyWdStr(PBowDocBs BowDocBs, const int& TopWords,
        const double& TopWordsWgtPrc, const TStr& SepStr, const bool& ShowWeights) const {

    return SvmKeyWdSpV->GetStr(BowDocBs, TopWords, TopWordsWgtPrc, SepStr, ShowWeights); 
}

void TOntoConcept::CalcCptMd(PBowDocBs BowDocBs, const TIntV& ContextDIdV,
        const double& SvmC, const double& SvmJ, const int& SvmTime) {

    if (Id == 0) return;
    // prepare sparse vectors
    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, 
        bwwtLogDFNrmTFIDF, 0.0, 0, ContextDIdV);
    PSVMTrainSet TrainSet = TBowDocBs2TrainSet::NewBowAllCatV(
        BowDocWgtBs, ContextDIdV, DIdV);
    // learn model
    CptMd = TSVMModel::NewClsLinear(TrainSet, SvmC, SvmJ, TIntV(), 
        TSVMLearnParam::Lin(SvmTime));
    // fit sigmoid to make probabilistic
    CptMd->MakeProb(TrainSet);
}

//////////////////////////////////////////////////////////////////////////
// Ontology-Relation-Type
TStr TOntoRltType::SubCptOfRltStr = "SubConcept-Of";
TStr TOntoRltType::SimilarRltStr = "Similar";

//////////////////////////////////////////////////////////////////////////
// Ontology-Algortihms
void TOntoAlg::SuggestKMeans(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, const int& _NewConceptN, PUniqueId UniqueId, 
        TOntoConceptV& NewConceptV) {

    // check if we want to do more 
    const int NewConceptN = TInt::GetMn(DIdV.Len(), _NewConceptN);
    NewConceptV.Clr(); if (NewConceptN < 2) return;
    // cluster documents from DIdV
    TIntV FullDIdV; BowDocWgtBs->GetDIdV(FullDIdV);
    BowDocWgtBs->SetDIdV(DIdV);
    PBowSim BowSim = TBowSim::New(bstCos);
    PBowDocPart BowDocPart=TBowClust::GetKMeansPartForDocWgtBs(
        TNotify::StdNotify, BowDocWgtBs, BowDocBs, 
        BowSim, TRnd(1), NewConceptN, 1, 10, 1);
    BowDocWgtBs->SetDIdV(FullDIdV);
    IAssert(BowDocPart->GetClusts() == NewConceptN);
    // Convert clusters to TOntoConcept
    //PBowDocWgtBs TempBowDocWgtBs = 
    //    TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF, 0, 0, DIdV);
    NewConceptV.Gen(NewConceptN, 0);
    for (int ClustC = 0; ClustC < NewConceptN; ClustC++) {
        PBowDocPartClust Clust = BowDocPart->GetClust(ClustC);
        TIntV CptDIdV; Clust->GetDIdV(CptDIdV);
        NewConceptV.Add(TOntoConcept::New(BowDocBs,
            BowDocWgtBs, UniqueId->GetNextId(), CptDIdV));
    }
}

void TOntoAlg::SuggestCat(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& DIdV, PUniqueId UniqueId, TOntoConceptV& NewConceptV) {

    // find categories in the DIdV
    TIntIntVH CIdDIdVH;
    for (int DocN = 0; DocN < DIdV.Len(); DocN++) {
        const int DId = DIdV[DocN];
        const int DocCIds = BowDocBs->GetDocCIds(DId);
        for (int DocCIdN = 0; DocCIdN < DocCIds; DocCIdN++) {
            const int DocCId = BowDocBs->GetDocCId(DId, DocCIdN);
            if (CIdDIdVH.IsKey(DocCId)) {
                CIdDIdVH.GetDat(DocCId).Add(DId);
            } else {
                CIdDIdVH.AddDat(DocCId, TIntV::GetV(DId));
            }
        }
    }
    // sort categories
    TIntKdV DocsKeyIdV(CIdDIdVH.Len(), 0);
    int KeyId = CIdDIdVH.FFirstKeyId();
    while (CIdDIdVH.FNextKeyId(KeyId)) {
        const int Docs = CIdDIdVH[KeyId].Len();
        DocsKeyIdV.Add(TIntKd(Docs, KeyId));
    }
    DocsKeyIdV.Sort(false);
    // Convert clusters to TOntoConcept
    //PBowDocWgtBs TempBowDocWgtBs = 
    //    TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF, 0, 0, DIdV);
    NewConceptV.Gen(CIdDIdVH.Len(), 0);
    for (int CptN = 0; CptN < DocsKeyIdV.Len(); CptN++) {
        const int CptKeyId = DocsKeyIdV[CptN].Dat;
        const TIntV& CptDIdV = CIdDIdVH[CptKeyId];
        TStr CptNm = BowDocBs->GetCatNm(CIdDIdVH.GetKey(CptKeyId));
        NewConceptV.Add(TOntoConcept::New(BowDocBs, BowDocWgtBs, 
            UniqueId->GetNextId(), CptNm, CptDIdV));
    }
}

void TOntoAlg::SuggestLwOntoCfier(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, PLwOntoCfier LwOntoCfier, const int& _NewConcepts, 
        PUniqueId UniqueId, TOntoConceptV& NewConceptV) {

    // iterate over all the documents from the concept and count classifications
    TIntH TermIdFqH; TIntIntVH TermIdDIdVH;
    for (int DIdN = 0; DIdN < DIdV.Len(); DIdN++) {
        const int DId = DIdV[DIdN];
        TStr DocStr = BowDocBs->GetDocStr(DId);
        TSimTermIdPrV SimTermIdPrV; SimTermIdPrV.Sort(false);
        LwOntoCfier->ClassifyStr(DocStr, SimTermIdPrV);
        if (!SimTermIdPrV.Empty()) { 
            const int TermId = SimTermIdPrV[0].Val2;
            if (TermIdFqH.IsKey(TermId)) { 
                TermIdFqH(TermId)++; 
                TermIdDIdVH(TermId).Add(DId);
            } else { 
                TermIdFqH.AddDat(TermId, 1); 
                TermIdDIdVH.AddDat(TermId, TIntV::GetV(DId));
            }
        }
    }
    // load hash table to vector indexed by frequency
    TIntPrV TermFqIdV; TermIdFqH.GetDatKeyPrV(TermFqIdV); TermFqIdV.Sort(false);
    // load them to the output parameters
    const int NewConcepts = TInt::GetMn(_NewConcepts, TermFqIdV.Len());
    NewConceptV.Gen(NewConcepts, 0);
    PLwTermBs TermBs = LwOntoCfier->GetLwOnto()->GetTermBs();
    for (int NewConceptN = 0; NewConceptN < NewConcepts; NewConceptN++) {
        const int TermId = TermFqIdV[NewConceptN].Val2;
        const TIntV& CptDIdV = TermIdDIdVH(TermId);
        TStr TermNm = TermBs->GetTerm(TermId)->GetTermNm();
        NewConceptV.Add(TOntoConcept::New(BowDocBs, BowDocWgtBs, 
            UniqueId->GetNextId(), TermNm, CptDIdV));
    }
}

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter (Proton Topic Ontology)
TStr TOntoExportPTO::GetUri(const int& Id, const TStr& Nm) {
    TChA CleanChA; bool ToUc = true;
    for (int ChN = 0; ChN < Nm.Len(); ChN++) {
        char Ch = Nm[ChN];
        if (TCh::IsAlpha(Ch)) { 
            if (ToUc) { Ch = TCh::GetUc(Ch); ToUc = false;}
            CleanChA += Ch; 
        } else { ToUc = true; }
    }
    return TInt::GetStr(Id) + "_" + TStr(CleanChA);
}

void TOntoExportPTO::StartExport() {
    SOut->PutStrLn("<?xml version='1.0' encoding='UTF-8'?>");
    SOut->PutStrLn("");
    SOut->PutStrLn("<!DOCTYPE rdf:RDF [");
    SOut->PutStrLn("    <!ENTITY rdf  'http://www.w3.org/1999/02/22-rdf-syntax-ns#'>");
    SOut->PutStrLn("    <!ENTITY rdfs 'http://www.w3.org/2000/01/rdf-schema#'>");
    SOut->PutStrLn("    <!ENTITY owl  'http://www.w3.org/2002/07/owl#'>");
    SOut->PutStrLn("    <!ENTITY xsd  'http://www.w3.org/2001/XMLSchema#' >");
    SOut->PutStrLn("    <!ENTITY ptop 'http://proton.semanticweb.org/2005/04/protont#'>");
    SOut->PutStrLn("    <!ENTITY psys 'http://proton.semanticweb.org/2005/04/protons#'>");
    SOut->PutStrLn("    <!ENTITY protonkm  'http://proton.semanticweb.org/2005/04/protonkm#'>");
    if (DigLibP) { SOut->PutStrLn("    <!ENTITY diglib 'http://proton.semanticweb.org/2005/04/protons#'>"); }
    SOut->PutStrLn("    <!ENTITY jsikm  'http://kt.ijs.si/blazf/jsikm#'>");
    SOut->PutStrLn("]>");
    SOut->PutStrLn("");
    SOut->PutStrLn("<rdf:RDF");
    SOut->PutStrLn("    xmlns:owl=\"&owl;\"");
    SOut->PutStrLn("    xmlns:rdf=\"&rdf;\"");
    SOut->PutStrLn("    xmlns:rdfs=\"&rdfs;\"");
    SOut->PutStrLn("    xmlns:psys=\"&psys;\"");
    SOut->PutStrLn("    xmlns:ptop=\"&ptop;\"");
    SOut->PutStrLn("    xmlns:protonkm=\"&protonkm;\"");
    SOut->PutStrLn("    xmlns=\"&protonkm;\"");
    if (DigLibP) { SOut->PutStrLn("    xmlns:diglib=\"&diglib;\""); }
    SOut->PutStrLn("    xmlns:jsikm=\"&jsikm;\"");
    SOut->PutStrLn("    xml:base=\"&protonkm;\""); 
    SOut->PutStrLn(">");
    SOut->PutStrLn("");
    SOut->PutStrLn("<owl:Ontology rdf:about=\"\">");
    SOut->PutStrLn("  <rdfs:comment>PROTON Topics (from Inspec Thesaurus) ordered by algorithm X</rdfs:comment>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protons\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protont\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protonu\"/>");
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://proton.semanticweb.org/2005/04/protonkm\"/>");
    if (DigLibP) { SOut->PutStrLn("  <owl:imports rdf:resource=\"http://kt.ijs.si/blazf/jsikm\"/>"); }
    SOut->PutStrLn("  <owl:imports rdf:resource=\"http://kt.ijs.si/blazf/jsikm\"/>");
    SOut->PutStrLn("  <owl:versionInfo>\"0.1\"</owl:versionInfo>");
    SOut->PutStrLn("</owl:Ontology>");
    SOut->PutStrLn("");
}
    
void TOntoExportPTO::EndExport() {
    SOut->PutStrLn("</rdf:RDF>");
    SOut->Flush();
}

void TOntoExportPTO::PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, const TIntV& DIdV, 
        PBowDocBs BowDocBs, PBowSpV KeyWdSpV, PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV) {

    SOut->PutStrLn("<ptop:Topic rdf:about=\"#Cpt" + GetUri(CptId, CptNm) + "\">");
    SOut->PutStrLn("  <psys:description>" + CptNm + "</psys:description>");
    for (int SuperCptN = 0; SuperCptN < SuperCptIdNmV.Len(); SuperCptN++) {
        const int SuperCptId = SuperCptIdNmV[SuperCptN].Val1;
        TStr SuperCptNm = SuperCptIdNmV[SuperCptN].Val2;
        SOut->PutStrLn("  <ptop:subTopicOf rdf:resource=\"#Cpt" + 
            GetUri(SuperCptId, SuperCptNm) + "\" />");
    }
    if (DigLibP) {
        for (int DIdN = 0; DIdN < DIdV.Len(); DIdN++) {
            const int DId = DIdV[DIdN];
            TStr DNm = BowDocBs->GetDocNm(DId);
            SOut->PutStrLn("  <diglib:hasArticle rdf:resource=\"#Doc" + 
                GetUri(DId, DNm) + "\" />");
        }
    }
    SOut->PutStrLn(TStr::Fmt("  <jsikm:hasOntoGenClassProperties rdf:resource=\"#CLS_PROP_%d\" />", CptId));
    SOut->PutStrLn("</ptop:Topic>");

    SOut->PutStrLn(TStr::Fmt("<jsikm:OntoGenClassProperties rdf:about=\"#CLS_PROP_%d\">", CptId));
    TStr KeyWdStr = KeyWdSpV->GetStr(BowDocBs, 10, 1.0, ", ", false, true);
    SOut->PutStrLn("  <jsikm:hasCentroidKeywords>" + KeyWdStr + "</jsikm:hasCentroidKeywords>");
    TStr SvmKeyWdStr = SvmKeyWdSpV->GetStr(BowDocBs, 10, 1.0, ", ", false, true);
    SOut->PutStrLn("  <jsikm:hasSVMKeywords>" + SvmKeyWdStr + "</jsikm:hasSVMKeywords>");
    SOut->PutStrLn("</jsikm:OntoGenClassProperties>");
    SOut->PutStrLn("");
}

void TOntoExportPTO::PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV) {

    SOut->PutStrLn("<ptop:Document rdf:about=\"#Doc" + GetUri(DId, DocTitle) + "\">");
    for (int CptIdN = 0; CptIdN < CptIdNmV.Len(); CptIdN++) {
        const int CptId = CptIdNmV[CptIdN].Val1;
        TStr CptNm = CptIdNmV[CptIdN].Val2;
        SOut->PutStrLn("  <ptop:hasSubject rdf:resource=\"#Cpt" + 
            GetUri(CptId, CptNm) + "\" />");
    }
    TStr CleanDocAbstract = TXmlDoc::GetXmlStr(DocAbstract);
    CleanDocAbstract.ChangeChAll('\n', ' '); CleanDocAbstract.ChangeChAll('\n', ' ');
    SOut->PutStrLn(TStr::Fmt("  <ptop:documentAbstract>%s</ptop:documentAbstract>", CleanDocAbstract.CStr()));
    SOut->PutStrLn(TStr::Fmt("  <jsikm:hasOntoGenInstanceProperties rdf:resource=\"#INST_PROP_%d\" />", DId));
    SOut->PutStrLn("</ptop:Document>");

    TStr ShortBowFNm = BowFNmStr.GetFMid() + BowFNmStr.GetFExt();
    SOut->PutStrLn(TStr::Fmt("<jsikm:OntoGenInstanceProperties rdf:about=\"#INST_PROP_%d\">", DId));
    SOut->PutStrLn(TStr::Fmt("  <jsikm:locationOfInstance>%s#%d</jsikm:locationOfInstance>", ShortBowFNm.CStr(), DId));
    SOut->PutStrLn("</jsikm:OntoGenInstanceProperties>");
    SOut->PutStrLn("");
}

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter (Prolog)
void TOntoExportProlog::StartExport() {
    SOut->PutStrLn("% OntoGen export in Prolog.");
}
    
void TOntoExportProlog::EndExport() {
    SOut->Flush();
}

void TOntoExportProlog::PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, 
        const TIntV& DIdV, PBowDocBs BowDocBs, PBowSpV KeyWdSpV, 
        PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV) {

    SOut->PutStrLn(TStr::Fmt("class( %s, label, '%s' ).", 
        GetCptLabel(CptId).CStr(), CptNm.CStr()));
    for (int SuperCptN = 0; SuperCptN < SuperCptIdNmV.Len(); SuperCptN++) {
        const int SuperCptId = SuperCptIdNmV[SuperCptN].Val1;
        SOut->PutStrLn(TStr::Fmt("subClassOf( %s, %s ).", 
            GetCptLabel(CptId).CStr(), GetCptLabel(SuperCptId).CStr()));
    }
}

void TOntoExportProlog::PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV) {

    TStr CleanDocTitle = DocTitle; CleanDocTitle.DelChAll('\'');
    CleanDocTitle.DelChAll('\n'); CleanDocTitle.DelChAll('\r');

    TStr CleanDocAbstract = TXmlDoc::GetXmlStr(DocAbstract);
    CleanDocAbstract.DelChAll('\''); CleanDocAbstract.DelChAll('\n'); 
    CleanDocAbstract.DelChAll('\r');

    SOut->PutStrLn(TStr::Fmt("instance( %s, label, '%s' ).", 
        GetInstLabel(DId).CStr(), CleanDocTitle.CStr()));
    SOut->PutStrLn(TStr::Fmt("instance( %s, text, '%s' ).", 
        GetInstLabel(DId).CStr(), CleanDocAbstract.CStr()));

    for (int CptIdN = 0; CptIdN < CptIdNmV.Len(); CptIdN++) {
        const int CptId = CptIdNmV[CptIdN].Val1;
        SOut->PutStrLn(TStr::Fmt("instanceOf( %s, %s ).", 
            GetInstLabel(DId).CStr(), GetCptLabel(CptId).CStr()));
    }
}

//////////////////////////////////////////////////////////////////////////
// Ontology-Exporter (OWL)
TStr TOntoExportOWL::GetUri(const int& Id, const TStr& Nm) {
    TChA CleanChA; bool ToUc = true;
    for (int ChN = 0; ChN < Nm.Len(); ChN++) {
        char Ch = Nm[ChN];
        if (TCh::IsAlpha(Ch)) { 
            if (ToUc) { Ch = TCh::GetUc(Ch); ToUc = false;}
            CleanChA += Ch; 
        } else { ToUc = true; }
    }
    return TInt::GetStr(Id) + "_" + TStr(CleanChA);
}

void TOntoExportOWL::StartExport() {
    SOut->PutStrLn("<?xml version=\"1.0\"?>");
    SOut->PutStrLn("<rdf:RDF");
    SOut->PutStrLn("    xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"");
    SOut->PutStrLn("    xmlns:xsd=\"http://www.w3.org/2001/XMLSchema#\"");
    SOut->PutStrLn("    xmlns:rdfs=\"http://www.w3.org/2000/01/rdf-schema#\"");
    SOut->PutStrLn("    xmlns:owl=\"http://www.w3.org/2002/07/owl#\"");
    SOut->PutStrLn("    xmlns=\"http://www.owl-ontologies.com/Ontology1150050933.owl#\"");
    SOut->PutStrLn("    xml:base=\"http://www.owl-ontologies.com/Ontology1150050933.owl\">");
    SOut->PutStrLn("");
    SOut->PutStrLn("<owl:Ontology rdf:about=\"OntoGen OWL export\"/>");
    SOut->PutStrLn("");
}
    
void TOntoExportOWL::EndExport() {
    SOut->PutStrLn("");
    SOut->PutStrLn("</rdf:RDF>");
    SOut->Flush();
}

void TOntoExportOWL::PutCpt(const int& CptId, const TStr CptNm, 
        const TIntStrPrV& SubCptIdNmV, const TIntStrPrV& SuperCptIdNmV, 
        const TIntV& DIdV, PBowDocBs BowDocBs, PBowSpV KeyWdSpV, 
        PBowSpV SvmKeyWdSpV, PBowSpV SvmNormalSpV) {

    SOut->PutStrLn("<owl:Class rdf:ID=\"Cpt" + GetUri(CptId, CptNm) + "\">");
    SOut->PutStrLn("  <rdfs:label>" + CptNm + "</rdfs:label>");

    for (int SuperCptN = 0; SuperCptN < SuperCptIdNmV.Len(); SuperCptN++) {
        const int SuperCptId = SuperCptIdNmV[SuperCptN].Val1;
        TStr SuperCptNm = SuperCptIdNmV[SuperCptN].Val2;
        SOut->PutStrLn("  <rdfs:subClassOf rdf:resource=\"#Cpt" + 
            GetUri(SuperCptId, SuperCptNm) + "\"/>");
    }
    SOut->PutStrLn("</owl:Class>");
    SOut->PutStrLn("");
}

void TOntoExportOWL::PutDoc(const int& DId, const TStr& DocTitle, 
        const TStr& DocAbstract, const TIntStrPrV& CptIdNmV) {
    
    // do we want to store the documents?
    if (!StoreDocsP) { return; }
    
    for (int CptIdN = 0; CptIdN < CptIdNmV.Len(); CptIdN++) {
        const int CptId = CptIdNmV[CptIdN].Val1;
        TStr CptNm = CptIdNmV[CptIdN].Val2;
        SOut->PutStrLn("<Cpt" + GetUri(CptId, CptNm) + " rdf:ID=\"Doc" + 
            GetUri(DId, DocTitle) + "\">");
        
        if (CptIdN == 0) {
            // we write full info only the first time
            TStr CleanDocTitle = DocTitle; CleanDocTitle.DelChAll('\'');
            CleanDocTitle.DelChAll('\n'); CleanDocTitle.DelChAll('\r');
            SOut->PutStrLn("  <rdfs:label>" + CleanDocTitle + "</rdfs:label>");
            
            if (StoreAbstractsP) {
                TStr CleanDocAbstract = TXmlDoc::GetXmlStr(DocAbstract);
                CleanDocAbstract.DelChAll('\''); CleanDocAbstract.DelChAll('\n'); 
                CleanDocAbstract.DelChAll('\r');
                SOut->PutStrLn("  <rdfs:comment>" + CleanDocAbstract + "</rdfs:label>");
            }
        }

        SOut->PutStrLn("</Cpt" + GetUri(CptId, CptNm) + ">");
    }
    SOut->PutStrLn("");
}

void TOntoExportOWL::PutRltType(const TStr& RltNm, const bool& DirectedP,
        const bool& TransitiveP) {

    SOut->PutStrLn("<owl:ObjectProperty rdf:ID=\"" + RltNm + "\"/>");
    SOut->PutStrLn("");    
}

void TOntoExportOWL::PutRlt(const TStr& RltNm, const int& SrcCptId, 
        const TStr SrcCptNm, const int& DstCptId, const TStr DstCptNm) {

    SOut->PutStrLn("<owl:Class rdf:ID=\"Cpt" + GetUri(SrcCptId, SrcCptNm) + "\">");
    SOut->PutStrLn("  <rdfs:subClassOf><owl:Restriction>");
    SOut->PutStrLn("    <owl:onProperty rdf:resource=\"#" + RltNm + "\"/>");
    SOut->PutStrLn("    <owl:someValuesFrom>");
    SOut->PutStrLn("      <owl:Class rdf:ID=\"Cpt" + GetUri(DstCptId, DstCptNm) + "\"/>");
    SOut->PutStrLn("    </owl:someValuesFrom>");
    SOut->PutStrLn("  </owl:Restriction></rdfs:subClassOf>");
    SOut->PutStrLn("</owl:Class>");
    SOut->PutStrLn("");    
}

//////////////////////////////////////////////////////////////////////////
// Ontology
TOntology::TOntology(PBowDocBs _BowDocBs, PBowDocWgtBs _BowDocWgtBs, 
    const TBowSimType& BowSimType): BowDocBs(_BowDocBs), 
        BowDocWgtBs(_BowDocWgtBs) {

    // initialize compoments
    BowSim = TBowSim::New(BowSimType);
    OntoGraph = TGGraph::New();
    UniqueId = TUniqueId::New(0);
    GenerateEmptyOnto();
}

void TOntology::GenerateEmptyOnto() {
    // generate root concept
    TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV);
    POntoConcept RootCpt = TOntoConcept::New(BowDocBs, 
        BowDocWgtBs, UniqueId->GetNextId(), AllDIdV);
    RootCpt->SetName("root");
    AddConcept(RootCpt, -1);
    OntoGraph->GetPp()->AddPpStr("BoldVNm", "", "root");

    // make basic relations
    RltTypeV.Add(TOntoRltType(TOntoRltType::SubCptOfRltStr, true, true));
    RltTypeV.Add(TOntoRltType(TOntoRltType::SimilarRltStr, false, false));
}

void TOntology::RefreshCptIdPosH() {
    CptIdPosH.Clr();
    for (int CptC = 0; CptC < ConceptV.Len(); CptC++) {
        const int CptId = ConceptV[CptC]->GetId();
        IAssertR(!CptIdPosH.IsKey(CptId), "Concept ID not unique!");
        CptIdPosH.AddDat(CptId, CptC);
    }
}

void TOntology::ReconnectToRoot() {
    for (int CptC = 0; CptC < ConceptV.Len(); CptC++) {
        const int CptId = ConceptV[CptC]->GetId();
        // if not root and has no super concept
        if (!IsRootCpt(CptId) && !HasSuperCpt(CptId)) {
            // we conect it to the root
            AddRlt(CptId, 0, TOntoRltType::SubCptOfRltStr);
        }
    }
}

void TOntology::AddRltsFromList(TIntStrPrV& InRltV, 
        TIntStrPrV& OutRltV, TIntStrPrV& UndirRltV) {
    // first we add all possible directed relations
    for (int InEdN = 0; InEdN < InRltV.Len(); InEdN++) {
        const int CptId1 = InRltV[InEdN].Val1;
        TStr RltNm1 = InRltV[InEdN].Val2; Assert(IsRltDir(RltNm1));
        for (int OutEdN = 0; OutEdN < OutRltV.Len(); OutEdN++) {
            const int CptId2 = OutRltV[OutEdN].Val1;
            TStr RltNm2 = OutRltV[OutEdN].Val2; Assert(IsRltDir(RltNm2));
            if (CptId1 != CptId2 && RltNm1 == RltNm2) {
                // if there already is such realtion, we go to the next one
                if (IsRlt(CptId1, CptId2, RltNm1)) continue;
                // if relation is not transitive, we go to the next one
                if (!IsRltTransitive(RltNm1)) continue;
                // add new edge to the graph
                AddRlt(CptId1, CptId2, RltNm1);
            }
        }
    }
    // than we add all the possible undirected relations
    for (int InEdN = 0; InEdN < UndirRltV.Len(); InEdN++) {
        const int CptId1 = UndirRltV[InEdN].Val1;
        TStr RltNm1 = UndirRltV[InEdN].Val2; Assert(!IsRltDir(RltNm1));
        for (int OutEdN = 0; OutEdN < UndirRltV.Len(); OutEdN++) {
            const int CptId2 = UndirRltV[OutEdN].Val1;
            TStr RltNm2 = UndirRltV[OutEdN].Val2; Assert(!IsRltDir(RltNm2));
            if (CptId1 != CptId2 && RltNm1 == RltNm2) {
                // if there already is such realtion, we go to the next one
                if (IsRlt(CptId1, CptId2, RltNm1)) continue;
                // if relation is not transitive, we go to the next one
                if (!IsRltTransitive(RltNm1)) continue;
                // add new edge to the graph
                AddRlt(CptId1, CptId2, RltNm1);
            }
        }
    }
}

void TOntology::AddSonDIdV(const int& CptId, TIntV& SonDIdV) {
    TIntV SubCptIdV; GetSubCptIdV(CptId, SubCptIdV);
    for (int SubCptIdN = 0; SubCptIdN < SubCptIdV.Len(); SubCptIdN++) {
        const int SubCptId = SubCptIdV[SubCptIdN];
        TIntV SubCptDIdV; GetCptDIdV(SubCptId, SubCptDIdV);
        SonDIdV.Union(SubCptDIdV);
        AddSonDIdV(SubCptId, SonDIdV);
    }
}

void TOntology::CfyDIdR(const int& DId, const TIntV& CptIdV, TIntFltKdV& CfyResV) {
    for (int CptIdN = 0; CptIdN < CptIdV.Len(); CptIdN++) {
        const int CptId = CptIdV[CptIdN];
        POntoConcept Cpt = GetConcept(CptId);
        if (Cpt->IsCptMd()) {
            bool IsDocInCpt; double Prob;
            Cpt->CfyDocSpV(BowDocWgtBs->GetSpV(DId), IsDocInCpt, Prob);
            if (IsDocInCpt) {
                CfyResV.Add(TIntFltKd(CptId, Prob));
                TIntV SubCptIdV; GetSubCptIdV(CptId, SubCptIdV);
                CfyDIdR(DId, SubCptIdV, CfyResV);
            }
        }
    }
}

void TOntology::Export(POntoExport OntoExport) {
    OntoExport->StartExport();

    // concepts
    TIntV CptIdV; GetCptIdV(CptIdV);
    TIntIntVH DIdCptIdVH;
    for (int CptN = 0; CptN < CptIdV.Len(); CptN++) {
        // basic concept properties
        const int CptId = CptIdV[CptN];
        TStr CptNm = TXmlDoc::GetXmlStr(GetCptName(CptId));
        if (OntoExport->IsCptAlwaysSvm()) { ExtractKeywords(CptId); }
        // prepare list of sub-concepts (if needed)
        TIntStrPrV SubCptIdNmV;
        if (OntoExport->IsCptSubCptLink()) {
            TIntStrPrV CptInRltV; GetCptInRltV(CptId, CptInRltV);
            for (int SubCptN = 0; SubCptN < CptInRltV.Len(); SubCptN++) {
                TStr RltNm = CptInRltV[SubCptN].Val2;
                if (RltNm == TOntoRltType::SubCptOfRltStr) {
                    const int SubCptId = CptInRltV[SubCptN].Val1;
                    TStr SubCptNm = GetCptName(SubCptId);
                    SubCptIdNmV.Add(TIntStrPr(SubCptId, SubCptNm));
                }
            }
        }
        // prepare list of super-concepts (if needed)
        TIntStrPrV SuperCptIdNmV;
        if (OntoExport->IsCptSuperCptLink()) {
            TIntStrPrV CptOutRltV; GetCptOutRltV(CptId, CptOutRltV);
            for (int SuperCptN = 0; SuperCptN < CptOutRltV.Len(); SuperCptN++) {
                TStr RltNm = CptOutRltV[SuperCptN].Val2;
                if (RltNm == TOntoRltType::SubCptOfRltStr) {
                    const int SuperCptId = CptOutRltV[SuperCptN].Val1;
                    TStr SuperCptNm = GetCptName(SuperCptId);
                    SuperCptIdNmV.Add(TIntStrPr(SuperCptId, SuperCptNm));
                }
            }
        }
        // prepare list of concept documents (if needed)
        TIntV CptDIdV; GetCptDIdV(CptId, CptDIdV);
        for (int DocN = 0; DocN < CptDIdV.Len(); DocN++) {
            const int DId = CptDIdV[DocN];
            if (DIdCptIdVH.IsKey(DId)) {
                DIdCptIdVH.GetDat(DId).Add(CptId);
            } else {
                DIdCptIdVH.AddDat(DId, TIntV::GetV(CptId));
            }
        }
        if (!OntoExport->IsCptDocLink()) { CptDIdV.Clr(); }
        // output concept
        OntoExport->PutCpt(CptId, CptNm, SubCptIdNmV, SuperCptIdNmV, 
            CptDIdV, BowDocBs, GetConcept(CptId)->GetCentroidSpV(), 
            GetConcept(CptId)->GetSvmSpV(), TBowSpV::New(-1));
    }

    // documents 
    TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV);
    for (int DocN = 0; DocN < AllDIdV.Len(); DocN++) {
        const int DId = AllDIdV[DocN];
        TStr DocTitle = BowDocBs->GetDocNm(DId);
        TStr DocAbstract = BowDocBs->GetDocStr(DId);
        // prepare list of docuemnt's concepts
        TIntStrPrV CptIdNmV;
        if (DIdCptIdVH.IsKey(DId) && OntoExport->IsDocCptLink()) {
            TIntV& DocCptIdV = DIdCptIdVH.GetDat(DId);
            for (int DocCptIdN = 0; DocCptIdN < DocCptIdV.Len(); DocCptIdN++) {
                const int CptId = DocCptIdV[DocCptIdN];
                TStr CptNm = GetCptName(CptId);
                CptIdNmV.Add(TIntStrPr(CptId, CptNm));
            }
        }
        // output document
        OntoExport->PutDoc(DId, DocTitle, DocAbstract, CptIdNmV);
    }        
    
    // relations
    if (OntoExport->IsRlt()) {
        // first we define all the relations
        const int RltTypes = GetRltTypes();
        for (int RltTypeN = 0; RltTypeN < RltTypes; RltTypeN++) {
            TStr RltTypeNm = GetRltTypeNm(RltTypeN);
            if (RltTypeNm != TOntoRltType::SubCptOfRltStr) {
                OntoExport->PutRltType(RltTypeNm, 
                    IsRltDir(RltTypeNm), IsRltTransitive(RltTypeNm));
            }
        }
        // than we add relations between concepts
        for (int CptN = 0; CptN < CptIdV.Len(); CptN++) {
            const int SrcCptId = CptIdV[CptN];
            TStr SrcCptNm = GetCptName(SrcCptId);
            // directed relations
            TIntStrPrV CptOutRltV; GetCptOutRltV(SrcCptId, CptOutRltV);
            for (int OutRltN = 0; OutRltN < CptOutRltV.Len(); OutRltN++) {
                const TStr& RltTypeNm = CptOutRltV[OutRltN].Val2;
                if (RltTypeNm != TOntoRltType::SubCptOfRltStr) {
                    const int DstCptId = CptOutRltV[OutRltN].Val1;
                    OntoExport->PutRlt(RltTypeNm, SrcCptId, SrcCptNm,
                        DstCptId, GetCptName(DstCptId));
                }
            }
            // undirected relations
            TIntStrPrV CptUndirRltV; GetCptUndirRltV(SrcCptId, CptUndirRltV);
            for (int UndirRltN = 0; UndirRltN < CptUndirRltV.Len(); UndirRltN++) {
                const TStr& RltTypeNm = CptUndirRltV[UndirRltN].Val2;
                const int DstCptId = CptUndirRltV[UndirRltN].Val1;
                if (SrcCptId < DstCptId) {
                    OntoExport->PutRlt(RltTypeNm, SrcCptId, SrcCptNm,
                        DstCptId, GetCptName(DstCptId));
                }
            }
        }
    }

    OntoExport->EndExport();
}

void TOntology::GetCptIdV(TIntV& CptIdV) {
    CptIdV.Gen(ConceptV.Len(), 0);
    for (int CptIdN = 0; CptIdN < ConceptV.Len(); CptIdN++) {
        CptIdV.Add(ConceptV[CptIdN]->GetId());
    }
    CptIdV.Sort();
}

void TOntology::SetCptName(const int& CptId, const TStr& NewName) {
    // change concept name
    GetConcept(CptId)->SetName(NewName);
    // change vertice name in graph
    OntoGraph->GetVrtx(CptId)->PutDNm(NewName);
}

void TOntology::SetCptDIdV(const int& CptId, const TIntV& _NewDIdV, const bool& PropagateP) {
    if (PropagateP) {
        // first we sort the list of new documents
        TIntV NewDIdV = _NewDIdV;
        if (!NewDIdV.IsSorted()) { NewDIdV.Sort(); }

        TIntV OldDIdV; GetCptDIdV(CptId, OldDIdV);
        // union of old and new documents
        TIntV BothDIdV; OldDIdV.Union(NewDIdV, BothDIdV);

        // list of documents that were added to the concept
        TIntV AddDIdV; BothDIdV.Diff(OldDIdV, AddDIdV);
        // we propagate addition of documents up towards the root
        AddUpDIdV(CptId, AddDIdV);

        // list of documents that were deleted from the concept
        TIntV DelDIdV; BothDIdV.Diff(NewDIdV, DelDIdV);
        // we propagate delition of documents down towards the leaves
        DelDownDIdV(CptId, DelDIdV);
    } else {
        // we just replace the old list with the new list
        GetConcept(CptId)->SetDIdV(BowDocWgtBs, _NewDIdV);
    }
}

void TOntology::GetCptSimV(const int& CptId, TFltIntKdV& SimCptIdV) {
    PBowSpV CptSpV = GetConcept(CptId)->GetCentroidSpV();
    SimCptIdV.Gen(GetConcepts() - 1, 0);
    for (int CptN = 0; CptN < GetConcepts(); CptN++) {
        const int CptId2 = ConceptV[CptN]->GetId();
        if (CptId != CptId2) {
            PBowSpV CptSpV2 = ConceptV[CptN]->GetCentroidSpV();
            SimCptIdV.Add(TFltIntKd(BowSim->GetSim(CptSpV, CptSpV2), CptId2));
        }
    }
    SimCptIdV.Sort(false);
}

void TOntology::CalcCptMd(const int& CptId, const double& SvmC, 
        const double& SvmJ, const int& SvmTime) {
    if (IsRootCpt(CptId)) return;
    TIntV ContextDIdV; BowDocBs->GetAllDIdV(ContextDIdV);
    //TIntV ContextDIdV; GetCptContextDIdV(CptId, ContextDIdV, false);
    GetConcept(CptId)->CalcCptMd(BowDocBs, ContextDIdV, SvmC, SvmJ, SvmTime);
}

void TOntology::CfyDId(const int& DId, TIntFltKdV& CfyResV) {
    //TIntV SubCptIdV; GetSubCptIdV(0, SubCptIdV);
    //CfyResV.Clr(); CfyDIdR(DId, SubCptIdV, CfyResV); 
    //CfyResV.Sort();

    const int CptIds = GetConcepts(); CfyResV.Gen(CptIds, 0);
    for (int CptIdN = 0; CptIdN < CptIds; CptIdN++) {
        const int CptId = GetCptId(CptIdN);
        POntoConcept Cpt = GetConcept(CptId);
        if (Cpt->IsCptMd()) {
            bool IsDocInCpt; double Prob;
            Cpt->CfyDocSpV(BowDocWgtBs->GetSpV(DId), IsDocInCpt, Prob);
            if (IsDocInCpt) {
                CfyResV.Add(TIntFltKd(CptId, Prob));
            } else {
                CfyResV.Add(TIntFltKd(CptId, Prob - 1.0));
            }
        } else if (IsRootCpt(CptId)) {
            CfyResV.Add(TIntFltKd(CptId, 1.0));
        } else {
            CfyResV.Add(TIntFltKd(CptId, -1.0));
        }
    }
    CfyResV.Sort();
}

void TOntology::IncludeLnDocs(const TStr& LnDocFNm, TIntV& NewDIdV) {
    TBowFl::LoadLnDocTxt(BowDocBs, LnDocFNm, NewDIdV, true, -1, true);    
    NewDIdV.Sort(); ReCalcWgtBs(); //TODO add it to the BowDocWgtBs
    GetRootCpt()->AddDIdV(BowDocWgtBs, NewDIdV); // everything is in root
}

void TOntology::IncludeFolder(const TStr& FPath, TIntV& NewDIdV, const bool& RecurseDirP) {
    TBowFl::LoadHtmlTxt(BowDocBs, FPath, NewDIdV, RecurseDirP, -1, true);
    NewDIdV.Sort(); ReCalcWgtBs(); //TODO add it to the BowDocWgtBs
    GetRootCpt()->AddDIdV(BowDocWgtBs, NewDIdV); // everything is in root
}

void TOntology::AddDocToCptV(const int& DId, const TIntV& CptIdV, const bool& UpdateCentroidP) {
    for (int CptIdN = 0; CptIdN < CptIdV.Len(); CptIdN++) {
        const int CptId = CptIdV[CptIdN];
        if (UpdateCentroidP) {
            GetConcept(CptId)->AddDId(BowDocWgtBs, DId);
        } else {
            GetConcept(CptId)->AddDId(NULL, DId);
        }
    }
}
   
void TOntology::ResetAllCptKeyWd() {
    const int CptIds = GetConcepts();
    for (int CptIdN = 0; CptIdN < CptIds; CptIdN++) {
        const int CptId = GetCptId(CptIdN);
        if (!GetConcept(CptId)->IsCentroidValid()) {
            GetConcept(CptId)->ResetCentroidSpV(BowDocWgtBs);
        }
    }
}

void TOntology::GetSubCptIdV(const int& CptId, TIntV& SubCptIdV) {
    // concepts which have an In-Relation from concept CptId
    // and the relation type name is TOntoRltType::SubCptOfRltStr
    TIntStrPrV InRltV; GetCptInRltV(CptId, InRltV);
    SubCptIdV.Gen(InRltV.Len(), 0);
    for (int InRltN = 0; InRltN < InRltV.Len(); InRltN++) {
        if (InRltV[InRltN].Val2 == TOntoRltType::SubCptOfRltStr) {
            SubCptIdV.Add(InRltV[InRltN].Val1);
        }
    }
    SubCptIdV.Sort();
}
    
void TOntology::GetSuperCptIdV(const int& CptId, TIntV& SuperCptIdV) {
    // concepts which have an Out-Relation from concept CptId
    // and the relation type name is TOntoRltType::SubCptOfRltStr
    TIntStrPrV OutRltV; GetCptOutRltV(CptId, OutRltV);
    SuperCptIdV.Gen(OutRltV.Len(), 0);
    for (int OutRltN = 0; OutRltN < OutRltV.Len(); OutRltN++) {
        if (OutRltV[OutRltN].Val2 == TOntoRltType::SubCptOfRltStr) {
            SuperCptIdV.Add(OutRltV[OutRltN].Val1);
        }
    }
    SuperCptIdV.Sort();
}

void TOntology::GetCptUnusedDIdV(const int& CptId, TIntV& DIdV) {
    // first get a list of all documents
    GetConcept(CptId)->GetDIdV(DIdV);
    // now we remove all the documents which are also in the
    // sub-concepts of the concept CptId
    TIntV SubCptIdV; GetSubCptIdV(CptId, SubCptIdV);
    for (int SubCptN = 0; SubCptN < SubCptIdV.Len(); SubCptN++) {
        const int SubCptId = SubCptIdV[SubCptN];
        TIntV SubDIdV; GetConcept(SubCptId)->GetDIdV(SubDIdV);
        DIdV.Diff(SubDIdV);
    }   
}

void TOntology::GetCptContextDIdV(const int& CptId, 
        TIntV& ContextDIdV, const bool& AddSonDIdP) {

    ContextDIdV.Clr();
    // first get a list of all documents
    GetConcept(CptId)->GetDIdV(ContextDIdV);
    // than all the documents that are in the super-concept (father)
    if (HasSuperCpt(CptId)) {
        TIntV SuperCptIdV, SuperDIdV; GetSuperCptIdV(CptId, SuperCptIdV);
        for (int SuperCptIdN = 0; SuperCptIdN < SuperCptIdV.Len(); SuperCptIdN++) {
            const int SuperCptId = SuperCptIdV[SuperCptIdN];
            GetConcept(SuperCptId)->GetDIdV(SuperDIdV);    
            ContextDIdV.Union(SuperDIdV);
        }
    }
    // and finaly all the documents that are in the sons
    if (AddSonDIdP) AddSonDIdV(CptId, ContextDIdV);
}

void TOntology::GetInconsistDIdV(const int& CptId, TIntV& InconsistDIdV) {
    InconsistDIdV.Clr();
    TIntV SonDIdV; AddSonDIdV(CptId, SonDIdV);
    TIntV CptDIdV; GetCptDIdV(CptId, CptDIdV);
    SonDIdV.Union(CptDIdV); 
    SonDIdV.Diff(CptDIdV, InconsistDIdV);
}

void TOntology::GetCptDocSimV(const int& CptId, const TIntV& DIdV, TFltV& CptDocSimV) {
    PBowSpV CptSpV = GetConcept(CptId)->GetCentroidSpV();
    const int Docs = DIdV.Len(); CptDocSimV.Gen(Docs, 0);
    for (int DocN = 0; DocN < Docs; DocN++) {
        PBowSpV DocSpV = BowDocWgtBs->GetSpV(DIdV[DocN]);
        CptDocSimV.Add(BowSim->GetSim(CptSpV, DocSpV));
    }
}

void TOntology::AddUpDIdV(const int& CptId, const TIntV& SubDIdV) {
    // first we add the documents to the current node
    GetConcept(CptId)->AddDIdV(BowDocWgtBs, SubDIdV);
    // than we propagate the change up towards root
    TIntV SuperCptIdV; GetSuperCptIdV(CptId, SuperCptIdV);
    for (int SuperCptIdN = 0; SuperCptIdN < SuperCptIdV.Len(); SuperCptIdN++) {
        const int SuperCptId = SuperCptIdV[SuperCptIdN];
        if (!IsRootCpt(SuperCptId)) { AddUpDIdV(SuperCptId, SubDIdV); }
    }
}

void TOntology::DelUpDIdV(const int& CptId, const TIntV& SubDIdV) {
    // first we delete the documents to the current node
    GetConcept(CptId)->DelDIdV(BowDocWgtBs, SubDIdV);
    // than we propagate the change up towards root
    TIntV SuperCptIdV; GetSuperCptIdV(CptId, SuperCptIdV);
    for (int SuperCptIdN = 0; SuperCptIdN < SuperCptIdV.Len(); SuperCptIdN++) {
        const int SuperCptId = SuperCptIdV[SuperCptIdN];
        if (!IsRootCpt(SuperCptId)) { DelUpDIdV(SuperCptId, SubDIdV); }
    }
}

void TOntology::DelDownDIdV(const int& CptId, const TIntV& SubDIdV) {
    // first we add the documents to the current node
    GetConcept(CptId)->DelDIdV(BowDocWgtBs, SubDIdV);
    // than we propagate the change down towards leaves
    TIntV SubCptIdV; GetSubCptIdV(CptId, SubCptIdV);
    for (int SubCptIdN = 0; SubCptIdN < SubCptIdV.Len(); SubCptIdN++) {
        const int SubCptId = SubCptIdV[SubCptIdN];
        DelDownDIdV(SubCptId, SubDIdV);
    }
}

void TOntology::DelDownDId(const int& CptId, const int& DId) {
    // first we delete the document from the current node
    GetConcept(CptId)->DelDId(BowDocWgtBs, DId);
    // than we propagate the change down towards leaves
    TIntV SubCptIdV; GetSubCptIdV(CptId, SubCptIdV);
    for (int SubCptIdN = 0; SubCptIdN < SubCptIdV.Len(); SubCptIdN++) {
        const int SubCptId = SubCptIdV[SubCptIdN];
        DelDownDId(SubCptId, DId);
    }
}

void TOntology::GetCptInRltV(const int& CptId, TIntStrPrV& InRltV) {
    POntoConcept Cpt = GetConcept(CptId);
    PVrtx CptVrtx = OntoGraph->GetVrtx(CptId);
    // load relations
    InRltV.Gen(CptVrtx->GetInEIds(), 0);
    for (int InEIdN = 0; InEIdN < CptVrtx->GetInEIds(); InEIdN++) {
        PEdge InEdge = OntoGraph->GetEdge(CptVrtx->GetInEId(InEIdN));
        if (InEdge->IsDir()) // must be directed
            InRltV.Add(TIntStrPr(InEdge->GetOtherVId(CptVrtx), InEdge->GetDNm()));
    }
    InRltV.Sort();
}

void TOntology::GetCptOutRltV(const int& CptId, TIntStrPrV& OutRltV) {
    POntoConcept Cpt = GetConcept(CptId);
    PVrtx CptVrtx = OntoGraph->GetVrtx(CptId);
    // load relations
    OutRltV.Gen(CptVrtx->GetOutEIds(), 0);
    for (int OutEIdN = 0; OutEIdN < CptVrtx->GetOutEIds(); OutEIdN++) {
        PEdge OutEdge = OntoGraph->GetEdge(CptVrtx->GetOutEId(OutEIdN));
        if (OutEdge->IsDir()) // must be directed 
            OutRltV.Add(TIntStrPr(OutEdge->GetOtherVId(CptVrtx), OutEdge->GetDNm()));
    }
    OutRltV.Sort();
}

void TOntology::GetCptUndirRltV(const int& CptId, TIntStrPrV& UndirRltV) {
    POntoConcept Cpt = GetConcept(CptId);
    PVrtx CptVrtx = OntoGraph->GetVrtx(CptId);
    // load relations
    UndirRltV.Gen(CptVrtx->GetOutEIds(), 0);
    for (int OutEIdN = 0; OutEIdN < CptVrtx->GetOutEIds(); OutEIdN++) {
        PEdge OutEdge = OntoGraph->GetEdge(CptVrtx->GetOutEId(OutEIdN));
        if (!OutEdge->IsDir()) // must NOT be directed
            UndirRltV.Add(TIntStrPr(OutEdge->GetOtherVId(CptVrtx), OutEdge->GetDNm()));
    }
    UndirRltV.Sort();
}

void TOntology::GetCptRltV(const int& CptId, TIntStrPrV& InRltV, 
        TIntStrPrV& OutRltV, TIntStrPrV& UndirRltV) {

    GetCptInRltV(CptId, InRltV);
    GetCptOutRltV(CptId, OutRltV); 
    GetCptUndirRltV(CptId, UndirRltV); 
}

int TOntology::GetRltTypeN(const TStr& RltTypeNm) const {
    for (int RltTypeN = 0; RltTypeN < RltTypeV.Len(); RltTypeN++) {
        if (RltTypeV[RltTypeN].IsType(RltTypeNm)) {
            return RltTypeN;
        }
    }
    return -1;
}

bool TOntology::IsRlt(const int& CptId1, const int& CptId2, const TStr& RltTypeNm) {
    Assert(IsCptId(CptId1) && IsCptId(CptId2));
    PEdge Edge; 
    if (OntoGraph->IsVrtxsEdge(CptId1, CptId2, IsRltDir(RltTypeNm), Edge)) {
        return (Edge->GetDNm() == RltTypeNm);
    }
    return false;
}

void TOntology::AddRlt(const int& CptId1, const int& CptId2, const TStr& RltTypeNm) {
    Assert(IsCptId(CptId1) && IsCptId(CptId2));
    const bool IsDir = IsRltDir(RltTypeNm);
    // we add the relation to the graph
    PEdge NewEdge = TGEdge::New(OntoGraph->GetVrtx(CptId1), 
        OntoGraph->GetVrtx(CptId2), RltTypeNm, IsDir);
    NewEdge->PutDNm(RltTypeNm);
    OntoGraph->AddEdge(NewEdge);
}

bool TOntology::DelRlt(const int& CptId1, const int& CptId2, const TStr& RltTypeNm) {
    Assert(IsCptId(CptId1) && IsCptId(CptId2));
    PEdge Edge; bool DeletedP = false;
    if (OntoGraph->IsVrtxsEdge(CptId1, CptId2, IsRltDir(RltTypeNm), Edge)) {
        if (Edge->GetDNm() == RltTypeNm) {
            OntoGraph->DelEdge(Edge);
            DeletedP = true;
        } else {
            InfoNotify("There is no such relationship!!");
        }
    } else {
        InfoNotify("There is no such relationship");
        AssertR(false, TStr("There is no such relationship"));
    }
    ReconnectToRoot();
    return DeletedP;
}


void TOntology::SuggestConcepts(const TIntV& DIdV, const TOntoSuggestAlgType& AlgType, 
        const int& NewConceptN, TOntoConceptV& NewConceptV) {

    NewConceptV.Clr();
    if ((DIdV.Len() > NewConceptN) || (NewConceptN == -1)) {
        if (AlgType == osatKMeans) {
            TOntoAlg::SuggestKMeans(BowDocBs, BowDocWgtBs, DIdV, 
                NewConceptN, UniqueId, NewConceptV);
        } else if (AlgType == osatCat) {
            TOntoAlg::SuggestCat(BowDocBs, BowDocWgtBs, DIdV, 
                UniqueId, NewConceptV);
        }
    }
}

void TOntology::SuggestSubconcepts(const int& CptId, const TOntoSuggestAlgType& AlgType, 
        const int& NewConceptN, const bool& UnusedDocsP, TOntoConceptV& NewConceptV) {

    TIntV DIdV; 
    if (UnusedDocsP){ GetCptUnusedDIdV(CptId, DIdV); }
    else {  GetConcept(CptId)->GetDIdV(DIdV); }
    SuggestConcepts(DIdV, AlgType, NewConceptN, NewConceptV);
}

void TOntology::SuggestSubconcepts(const int& CptId, const PLwOntoCfier& LwOntoCfier,
        const int& NewConceptN, const bool& UnusedDocsP, TOntoConceptV& NewConceptV) {

    TIntV DIdV; 
    if (UnusedDocsP){ GetCptUnusedDIdV(CptId, DIdV); }
    else {  GetConcept(CptId)->GetDIdV(DIdV); }
 
    NewConceptV.Clr();
    if (DIdV.Len() > NewConceptN) {
        TOntoAlg::SuggestLwOntoCfier(BowDocBs, BowDocWgtBs,
            DIdV, LwOntoCfier, NewConceptN, UniqueId, NewConceptV);
    }
}

POntoConcept TOntology::GenSubconcept(const TStr& NewCptNm, const TIntV& NewCptDIdV) {
    return TOntoConcept::New(BowDocBs, BowDocWgtBs, 
        UniqueId->GetNextId(), NewCptNm, NewCptDIdV);
}

POntoConcept TOntology::GenSubconcept(const TIntV& NewCptDIdV) {
    return TOntoConcept::New(BowDocBs, BowDocWgtBs, 
        UniqueId->GetNextId(), NewCptDIdV);
}

void TOntology::SuggestConceptNm(const int& CptId, const TVec<PLwOntoCfier>& LwOntoCfierV, 
        const int& MxSuggestNms, TStrV& SuggestNmV, TIntV& SuggestSuppV, TIntV& SuggestVocNV) {

    // iterate over all the documents from the concept and count classifications
    TIntPrIntH TermIdVocNFqH; TIntV DIdV; GetCptDIdV(CptId, DIdV); 
    for (int VocN = 0; VocN < LwOntoCfierV.Len(); VocN++) {
        PLwOntoCfier LwOntoCfier = LwOntoCfierV[VocN];
        PLwTermBs LwTermBs = LwOntoCfier->GetLwOnto()->GetTermBs();
        for (int DIdN = 0; DIdN < DIdV.Len(); DIdN++) {
            const int DId = DIdV[DIdN];
            TStr DocStr = BowDocBs->GetDocStr(DId);
            TSimTermIdPrV SimTermIdPrV; SimTermIdPrV.Sort(false);
            LwOntoCfier->ClassifyStr(DocStr, SimTermIdPrV);
            const int TermIds = TInt::GetMn(3, SimTermIdPrV.Len());
            for (int TermIdN = 0; TermIdN < TermIds; TermIdN++) {
                const int TermId = SimTermIdPrV[TermIdN].Val2;
                TIntPr TermIdVocN(TermId, VocN);
                TStr TermNm = LwTermBs->GetTerm(TermId)->GetTermNm();
                if (TermIdVocNFqH.IsKey(TermIdVocN)) { TermIdVocNFqH(TermIdVocN)++; } 
                else { TermIdVocNFqH.AddDat(TermIdVocN, 1); }
            }             
        }
    }
    // load hash table to vector indexed by frequency
    TIntIntPrPrV TermFqIdVocNV; TermIdVocNFqH.GetDatKeyPrV(TermFqIdVocNV); 
    // find the top terms
    TermFqIdVocNV.Sort(false);
    // load them to the output parameters
    SuggestNmV.Clr(); SuggestSuppV.Clr(); SuggestVocNV.Clr();
    const int SuggestNms = TInt::GetMn(MxSuggestNms, TermFqIdVocNV.Len());
    for (int SuggestNmN = 0; SuggestNmN < SuggestNms; SuggestNmN++) {
        const int TermId = TermFqIdVocNV[SuggestNmN].Val2.Val1;
        const int Supp = TermFqIdVocNV[SuggestNmN].Val1;
        const int VocN = TermFqIdVocNV[SuggestNmN].Val2.Val2;
        PLwTermBs LwTermBs = LwOntoCfierV[VocN]->GetLwOnto()->GetTermBs();
        TStr TermNm = LwTermBs->GetTerm(TermId)->GetTermNm();
        SuggestNmV.Add(TermNm); 
        SuggestSuppV.Add(Supp); 
        SuggestVocNV.Add(VocN);
    }
}


void TOntology::AddConcept(const POntoConcept& NewCpt, const int& FatherCptId) {
    const int NewCptId = NewCpt->GetId();
    // check if we don't already have it
    IAssertR(!IsCptId(NewCptId), "There already is a conceptwith the same ID!");
    // add concept to the list
    ConceptV.Add(NewCpt); RefreshCptIdPosH();
    // add concept to the graph
    TStr NewCptIdStr = TStr::Fmt("Cpt%d", NewCptId);
    PVrtx NewCptVrtx = TGVrtx::New(NewCptId, NewCptIdStr);
    TStr Name = NewCpt->GetName(); Name.ChangeChAll(' ', '\\');
    NewCptVrtx->PutDNm(Name); 
    NewCptVrtx->PutShape("Rect");
    OntoGraph->AddVrtx(NewCptVrtx);
    // add subtopic-of relation to its father
    if (FatherCptId >= 0) {
        IAssertR(IsCptId(FatherCptId), "There is no such concept!");
        AddRlt(NewCptId, FatherCptId, TOntoRltType::SubCptOfRltStr);
    } else {
        // father can be <0 only when adding 'root' concept
        IAssert(ConceptV.Len() == 1);
    }
}

void TOntology::BreakConcept(const int& OldCptId, const TOntoConceptV& NewCptV) {
    if (IsRootCpt(OldCptId)) return; // we can not break the root concept
    IAssert(!NewCptV.Empty());
    // check if we have the concept we want to break in the ontology
    IAssertR(IsCptId(OldCptId), "There is no such concept!");
    // first we add new concepts to the ontology (list and graph)
    for (int CptC = 0; CptC < NewCptV.Len(); CptC++) {
        POntoConcept NewCpt = NewCptV[CptC];
        const int NewCptId = NewCpt->GetId();
        IAssertR(!IsCptId(NewCptId), 
            "There already is a concept with the same ID!");
        // add concept to the list
        ConceptV.Add(NewCpt); RefreshCptIdPosH();
        // add concept to the graph
        TStr Name = NewCpt->GetName(); Name.ChangeStrAll(" ", "\\");
        PVrtx v = TGVrtx::New(NewCptId, Name.CStr());
        OntoGraph->AddVrtx(v);
    }
    // than we redirect all relations from OldCpt to NewCptV
    TIntStrPrV InRltV, OutRltV, UndirRltV;
    GetCptRltV(OldCptId, InRltV, OutRltV, UndirRltV);
    // first input-edges
    for (int InRltN = 0; InRltN < InRltV.Len(); InRltN++) { 
        const int VrtxId1 = InRltV[InRltN].Val1;
        TStr RltNm = InRltV[InRltN].Val2;
        for (int CptC = 0; CptC < NewCptV.Len(); CptC++) {
            const int NewCptId = NewCptV[CptC]->GetId();
            AddRlt(VrtxId1, NewCptId, RltNm);
        }
    }
    // than output-edges
    for (int OutRltN = 0; OutRltN < OutRltV.Len(); OutRltN++) {
        const int VrtxId2 = OutRltV[OutRltN].Val1;
        TStr RltNm = OutRltV[OutRltN].Val2;
        for (int CptC = 0; CptC < NewCptV.Len(); CptC++) {
            const int NewCptId = NewCptV[CptC]->GetId();
            AddRlt(NewCptId, VrtxId2, RltNm);
        }
    }
    // and undirected edges at the end :)
    for (int UndirRltN = 0; UndirRltN < UndirRltV.Len(); UndirRltN++) {
        const int VrtxId2 = UndirRltV[UndirRltN].Val1;
        TStr RltNm = UndirRltV[UndirRltN].Val2;
        for (int CptC = 0; CptC < NewCptV.Len(); CptC++) {
            const int NewCptId = NewCptV[CptC]->GetId();
            AddRlt(NewCptId, VrtxId2, RltNm);
        }
    }
    // at the end we delete the old concept
    ConceptV.Del(CptIdPosH.GetDat(OldCptId)); RefreshCptIdPosH();
    OntoGraph->DelVrtx(OldCptId); ReconnectToRoot();
}

void TOntology::PruneSubconcept(const POntoConcept& SubCpt, const int& FatherCptId) {
    if (FatherCptId == 0) return; // we can not prune from the root concept
    // check if we have the concept we want to break in the ontology
    IAssertR(IsCptId(FatherCptId), "There is no such concept!");
    // remove the documents from SubCpt from FatherCptId
    TIntV SubCptDIdV; SubCpt->GetDIdV(SubCptDIdV);
    GetConcept(FatherCptId)->DelDIdV(BowDocWgtBs, SubCptDIdV);
}

void TOntology::DeleteConcept(const int& CptId, const bool& DoRedirect) {
    if (IsRootCpt(CptId)) return; // we can not delete root concept
    // check if we have the concept we want to delete from the ontology
    IAssertR(IsCptId(CptId), "There is no such concept!");
    if (DoRedirect) {
        // here we redirect all the links that go trough this node
        TIntStrPrV InRltV, OutRltV, UndirRltV;
        GetCptRltV(CptId, InRltV, OutRltV, UndirRltV);
        AddRltsFromList(InRltV, OutRltV, UndirRltV);
    }
    // at the end we delete the old concept
    ConceptV.Del(CptIdPosH.GetDat(CptId)); RefreshCptIdPosH();
    OntoGraph->DelVrtx(CptId); ReconnectToRoot();
}

void TOntology::UniteConcepts(const int& CptId1, const int& CptId2) {
    // TODO unite
}

void TOntology::CopyConcept(const int& CptId, const int NewFatherCptId, const bool& MoveP) {
    if (IsRootCpt(CptId)) return; // we can not move root concept
    IAssertR(IsCptId(CptId), "There is no such concept!");
    IAssertR(IsCptId(NewFatherCptId), "There is no such concept!");
    // check if NewFatherCptId is already a father of CptId
    TIntV SuperCptIdV; GetSuperCptIdV(CptId, SuperCptIdV);
    SuperCptIdV.DelIfIn(NewFatherCptId);
    if (SuperCptIdV.Empty()) return; // no need to move anything
    // we do the moving by adding and deleting relations
    AddRlt(CptId, NewFatherCptId, TOntoRltType::SubCptOfRltStr);
    for (int SuperCptIdN = 0; SuperCptIdN < SuperCptIdV.Len(); SuperCptIdN++) {
        const int SuperCptId = SuperCptIdV[SuperCptIdN];
        DelRlt(CptId, SuperCptId, TOntoRltType::SubCptOfRltStr);
    }
    // first we delete the concepts from the old super concepts
    TIntV CptDIdV; GetCptDIdV(CptId, CptDIdV);
    if (MoveP) {
        for (int SuperCptIdN = 0; SuperCptIdN < SuperCptIdV.Len(); SuperCptIdN++) {
            const int SuperCptId = SuperCptIdV[SuperCptIdN];
            DelUpDIdV(SuperCptId, CptDIdV);
        }
    }
    // and than we propagate the documents to the new super concepts
    AddUpDIdV(NewFatherCptId, CptDIdV);
}

void TOntology::NewConcept() {
    POntoConcept Cpt = TOntoConcept::New(BowDocBs, 
        BowDocWgtBs, UniqueId->GetNextId(), "New Concept", TIntV());
    IAssert(ConceptV[0]->GetId() == 0);
    AddConcept(Cpt, 0);
}

void TOntology::ExtractKeywords(const int& CptId) {
    if (IsRootCpt(CptId)) return;
    // load documents
    TIntV ContextDIdV; GetCptContextDIdV(CptId, ContextDIdV);
    TIntV CptDIdV; GetCptDIdV(CptId, CptDIdV); 
    // learn important words
    PBowSpV SvmSpV;
    if (CptDIdV.Len() > 0) {
        SvmSpV = TBowSVMMd::GetKeywords(BowDocBs, ContextDIdV, CptDIdV, 50, 1.0, 5.0);
    } else {
        SvmSpV = TBowSpV::New();
    }
    // save
    GetConcept(CptId)->SetSvmSpV(SvmSpV);
}

void TOntology::CalcCptCompactness(const int& CptId) {
    TIntV CptDIdV; GetCptDIdV(CptId, CptDIdV);
    if (CptDIdV.Len() > 0) {
        PBowSpV CentroidSpV = GetConcept(CptId)->GetCentroidSpV();
        double Qual = 0.0; 
        for (int DIdN = 0; DIdN < CptDIdV.Len(); DIdN++) {
            const int DId = CptDIdV[DIdN];
            PBowSpV DocSpV = BowDocWgtBs->GetSpV(DId);
            Qual += BowSim->GetSim(CentroidSpV, DocSpV);
        }
        GetConcept(CptId)->SetCompactness(Qual / CptDIdV.Len());
    } else {
        GetConcept(CptId)->SetCompactness(1.0);
    }
}

void TOntology::CalcCptClarity(const int& CptId) {
    if (IsRootCpt(CptId)) return;
    // load documents
    TIntV ContextDIdV; GetCptContextDIdV(CptId, ContextDIdV);
    TIntV CptDIdV; GetCptDIdV(CptId, CptDIdV); 
    if (CptDIdV.Len() > 20) {
        // calculate BEP for 5-fold cross validation
        TCfyRes CfyRes = TBowSVMMd::CrossValidClsLinear(
            5, 1, BowDocBs, BowDocWgtBs, ContextDIdV, CptDIdV, 1, 5.0);
        const double Clarity = CfyRes.BreakEvenPoint();
        // save
        GetConcept(CptId)->SetClarity(Clarity);
    }
}

void TOntology::PrepareGraph(const int& SelectedCptId) {
    IAssert(GetConcepts() > 0);
    // mark the selected CptId
    if (IsCptId(SelectedCptId)) {
        TStr VrtxNm = OntoGraph->GetVrtx(SelectedCptId)->GetDNm();
        OntoGraph->GetPp()->PutValStr("BoldVNm", VrtxNm);
    }
}

void TOntology::PlaceGraph() {
    IAssert(GetConcepts() > 0);
    // place graph as a tree
    PVrtx RootVrtx = OntoGraph->GetVrtx(0);
    OntoGraph->PlaceGraphAsStar(RootVrtx, TOntoRltType::SubCptOfRltStr);
    // reascale coordinates to fit
    OntoGraph->RescaleXY(0.1, RootVrtx);
}

void TOntology::ExportPTO(const TStr& FNm, const bool& DigLibP) { 
    PSOut SOut = TFOut::New(FNm);
    TStr BowFNm = FNm + ".bow"; BowDocBs->SaveBin(BowFNm);
    POntoExport OntoExport = TOntoExportPTO::New(SOut, DigLibP, BowFNm);
    this->Export(OntoExport);
}

void TOntology::ExportProlog(const TStr& FNm) {
    PSOut SOut = TFOut::New(FNm);
    POntoExport OntoExport = TOntoExportProlog::New(SOut);
    this->Export(OntoExport);
}

void TOntology::ExportOwl(const TStr& FNm, const bool& StoreDocsP, const bool& StoreAbstractsP) {
    PSOut SOut = TFOut::New(FNm);
    POntoExport OntoExport = TOntoExportOWL::New(SOut, StoreDocsP, StoreAbstractsP);
    this->Export(OntoExport);
}

POntology TOntology::ImportPTO(const TStr& RdfFNm, const TStr& BowFNm) {
    // load bow and generate empty ontology on that bow
    PBowDocBs BowDocBs = TBowDocBs::LoadBin(BowFNm);
    const int MnWordFq = BowDocBs->GetDocs() > 111 ? 5 : 3;
    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(
        BowDocBs, bwwtLogDFNrmTFIDF, 0.2, MnWordFq);
    POntology Onto = TOntology::New(BowDocBs, BowDocWgtBs, bstCos);

    // now we parse RDF in following steps
    //  (1) load topics topics and add them as empty concepts to
    //      the Onto with root as father, also prepare for adding docs
    //  (2) load relations between topics and add them to Onto
    //      + delete realtion to father that don't exist
    //  (3) load lists of documents for each concept and add them

    // prolog
    PXmlDoc Rdf = TXmlDoc::LoadTxt(RdfFNm);
    IAssertR(Rdf->IsOk(), "Wrong RDF format");
    // map between topics URI in RDF to topic ID assigned in the OntoGen
    TStrIntH TopicUriIdH;
    // seperate XML in more vectors
    PXmlTok RootTok; IAssert(Rdf->IsTagTok("rdf:RDF", RootTok));
    TXmlTokV TopicV; RootTok->GetTagTokV("ptop:Topic", TopicV);
    TXmlTokV DocV; RootTok->GetTagTokV("ptop:Document", DocV);
    TXmlTokV DocPropV; RootTok->GetTagTokV("jsikm:OntoGenInstanceProperties", DocPropV);

    // step (1)
    // there is always root node, we add everything to it later
    TIntIntVH TopicIdDocIdVH; TopicIdDocIdVH.AddDat(0, TIntV());
    for (int TopicN = 0; TopicN < TopicV.Len(); TopicN++) {
        // load topic info
        PXmlTok TopicTok = TopicV[TopicN];
        TStr TopicURI = TopicTok->GetArgVal("rdf:about");
        TStr TopicName = TopicTok->GetTagTok("psys:description")->GetTagTokStr("");
        int UnderScorePos = TopicURI.SearchCh('_');
        TStr TopicURIPrefix = TopicURI.Left(UnderScorePos);
        if (TopicURIPrefix != "#Cpt0" && TopicURI != "#TOP_0") {
            const int TopicId = Onto->UniqueId->GetNextId(); 
            // add it as empty subconcept of root concept
            POntoConcept TopicCpt = TOntoConcept::New(Onto->BowDocBs, 
                Onto->BowDocWgtBs, TopicId, TopicName, TIntV());
            Onto->AddConcept(TopicCpt, 0);
            // remember Uri-Id link
            TopicUriIdH.AddDat(TopicURI, TopicId);
            // prepare place for documents
            TopicIdDocIdVH.AddDat(TopicId, TIntV());
        } else {
            // root node of saved ontology
            Onto->SetCptName(0, TopicName);
            TopicUriIdH.AddDat(TopicURI, 0);
        }        
    }

    // step (2)
    for (int TopicN = 0; TopicN < TopicV.Len(); TopicN++) {
        // load topic info
        PXmlTok TopicTok = TopicV[TopicN];
        TStr TopicURI = TopicTok->GetArgVal("rdf:about");
        const int TopicId = TopicUriIdH.GetDat(TopicURI);
        bool ConnectedToRoot = false;
        // we don't mess with supertopics of ROOT
        if (TopicId > 0) {
            TXmlTokV SuperTopicV; TopicTok->GetTagTokV("ptop:subTopicOf", SuperTopicV);
            for (int SupTopN = 0; SupTopN < SuperTopicV.Len(); SupTopN++) {
                TStr SuperTopicURI = SuperTopicV[SupTopN]->GetArgVal("rdf:resource");
                const int SuperTopicId = TopicUriIdH.GetDat(SuperTopicURI);
                if (SuperTopicId == 0) {
                    ConnectedToRoot = true;
                } else {            
                    Onto->AddRlt(TopicId, SuperTopicId, TOntoRltType::SubCptOfRltStr);
                }
            }
            if (!ConnectedToRoot) {
                Onto->DelRlt(TopicId, 0, TOntoRltType::SubCptOfRltStr);
            }
        }
    }

    // step (3)
    // load document ids
    TStrIntH DocPropIdH;
    for (int DocN = 0; DocN < DocPropV.Len(); DocN++) {
        PXmlTok DocPropTok = DocPropV[DocN];
        TStr DocPropURI = DocPropTok->GetArgVal("rdf:about");
        TStr Link = DocPropTok->GetTagTok("jsikm:locationOfInstance")->GetTagTokStr("");
        TStrV LinkPartV; Link.SplitOnAllCh('#', LinkPartV);
        IAssert(LinkPartV.Len() == 2);
        const int DocId = LinkPartV[1].GetInt();
        IAssert(BowDocBs->IsDId(DocId));
        DocPropIdH.AddDat(DocPropURI, DocId);
    }
    // add documents to topics
    for (int DocN = 0; DocN < DocV.Len(); DocN++) {
        PXmlTok DocTok = DocV[DocN];
        TStr DocPropURI = DocTok->GetTagTok("jsikm:hasOntoGenInstanceProperties")->GetArgVal("rdf:resource");
        const int DocId = DocPropIdH.GetDat(DocPropURI);
        // we add document to the root node
        TopicIdDocIdVH.GetDat(0).Add(DocId);
        // search for other topics to which we add the document
        TXmlTokV SubjectTopicV; DocTok->GetTagTokV("ptop:hasSubject", SubjectTopicV);
        for (int SubjectN = 0; SubjectN < SubjectTopicV.Len(); SubjectN++) {
            PXmlTok SubjectTok = SubjectTopicV[SubjectN];
            TStr SubjectUri = SubjectTok->GetArgVal("rdf:resource");
            const int SubjectId = TopicUriIdH.GetDat(SubjectUri);
            if (SubjectId > 0) {
                IAssert(TopicIdDocIdVH.IsKey(SubjectId));
                TopicIdDocIdVH.GetDat(SubjectId).Add(DocId);
            }
        }
    }
    // update Onto with subject-of relations
    int KeyId = TopicIdDocIdVH.FFirstKeyId();
    while (TopicIdDocIdVH.FNextKeyId(KeyId)) {
        const int TopicId = TopicIdDocIdVH.GetKey(KeyId);
        Onto->SetCptDIdV(TopicId, TopicIdDocIdVH.GetDat(TopicId), false);
    }

    // epilog
    return Onto;
}



























