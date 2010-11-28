/////////////////////////////////////////////////
// BagOfWords-Active-Learning
TBowAL::TBowAL(const PBowDocBs& _BowDocBs, const int& _CId, const double& _SvmC,
        const double& _SvmJ, const TIntV& _DIdV): BowDocBs(_BowDocBs),
        SvmC(_SvmC), SvmJ(_SvmJ), CId(_CId) {

    ALType = baltCat;
    // check existence of the target category
    IAssert(BowDocBs->IsCId(CId));
    // get all document-ids
    TIntV DIdV = _DIdV;
    if (DIdV.Len() == 0) { BowDocBs->GetAllDIdV(DIdV); }
    // collect labeled and unlabeled documents
    for (int DIdN = 0; DIdN < DIdV.Len(); DIdN++){
        int DId = DIdV[DIdN];
        if (BowDocBs->GetDocCIds(DId) > 0){
            LabeledDIdV.Add(DIdN);
        } else {
            UnlabeledDIdV.Add(DIdN);
        }
    }
    // collect document weights
    BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF, 0.0, 0, DIdV);
    DocSet = TBowDocBs2TrainSet::NewBowAllCat(BowDocBs, BowDocWgtBs, CId, DIdV);
}

TBowAL::TBowAL(const PBowDocBs& _BowDocBs, const TStr& _QueryStr,
        const int& _MnPosDocs, const int& _MnNegDocs, const double& _SvmC,
        const double& _SvmJ, const TIntV& _DIdV): BowDocBs(_BowDocBs),
        SvmC(_SvmC), SvmJ(_SvmJ),
        QueryStr(_QueryStr), MnPosDocs(_MnPosDocs), MnNegDocs(_MnNegDocs) {

    Assert((MnPosDocs > 0) && (MnNegDocs > 0));
    ALType = baltQuery;
    // we start in IR mode -- user must first find some pos and neg docs
    InfoRetModeP = true;
    // get all document-ids
    TIntV DIdV = _DIdV;
    if (DIdV.Len() == 0) { BowDocBs->GetAllDIdV(DIdV); }
    // collect document weights
    BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF, 0.0, 0, DIdV);
    QuerySpV = BowDocBs->GetSpVFromHtmlStr(QueryStr, BowDocWgtBs);
    DocSet = TBowDocBs2TrainSet::NewBowNoCat(BowDocWgtBs, DIdV, 0.0);
    // first everything is unlabeled
    LabeledDIdV.Clr();
    UnlabeledDIdV.Gen(DIdV.Len(), 0);
    for (int DIdN = 0; DIdN < DIdV.Len(); DIdN++) {
        UnlabeledDIdV.Add(DIdN);
        IAssert(DIdV[DIdN] == DocSet->GetAttrBowV(DIdN)->GetDId());
    }
}


bool TBowAL::GenQueryDIdV(const bool& GenStatP){
    DistDIdPrV.Clr();
    // stop if everything labeled
    if (UnlabeledDIdV.Empty()){return false;}
    // if in query mode than we do nearest-neighbour search
    if ((ALType == baltQuery) && InfoRetModeP) {
        Assert((ALType == baltQuery) && InfoRetModeP);
        const bool HasPosDocs = DocSet->HasPosVecs(MnPosDocs);
        const bool HasNegDocs = DocSet->HasNegVecs(MnNegDocs);
        if (HasPosDocs && HasNegDocs) {
            InfoRetModeP = false;
        } else {
            PBowSim BowSim = TBowSim::New(bstCos);
            for (int UnLblDIdN = 0; UnLblDIdN < UnlabeledDIdV.Len(); UnLblDIdN++){
                PBowSpV DocSpV = DocSet->GetAttrBowV(UnlabeledDIdV[UnLblDIdN]);
                double Dist = 1 - BowSim->GetSim(QuerySpV, DocSpV);
                DistDIdPrV.Add(TFltIntPr(Dist, DocSpV->GetDId()));
            }
            DistDIdPrV.Sort();
            if (HasPosDocs) { DistDIdPrV.Reverse(); }
        }

    }
    // otherwise we see who is the closest to the hyperplane
    if ((ALType == baltCat) || ((ALType == baltQuery)&&(!InfoRetModeP))) {
        // build the model from labeled documents
        PSVMModel Md = TSVMModel::NewClsLinear(DocSet, SvmC, SvmJ, LabeledDIdV);
        // create set of query-document-ids sorted by closeness
        for (int UnLblDIdN = 0; UnLblDIdN < UnlabeledDIdV.Len(); UnLblDIdN++){
            PBowSpV DocSpV = DocSet->GetAttrBowV(UnlabeledDIdV[UnLblDIdN]);
            double Cfy = Md->GetRes(DocSpV);
            double Dist = fabs(Cfy);
            DistDIdPrV.Add(TFltIntPr(Dist, DocSpV->GetDId()));
        }
        // sort query documents-ids (closest to hyperplane have lower index)
        DistDIdPrV.Sort();
        // prepare statistics
        if (GenStatP) {
            // keywords
            TFltV NormalV; Md->GetWgtV(NormalV); TIntFltKdV WIdWgtV;
            DocSet->GetKeywords(NormalV, WIdWgtV, LabeledDIdV);
            if (!WIdWgtV.Empty()) {
                KeyWdStr = BowDocBs->GetWordStr(WIdWgtV[0].Key);
                for (int WIdN = 1; WIdN < WIdWgtV.Len(); WIdN++) {
                    KeyWdStr += ", " + BowDocBs->GetWordStr(WIdWgtV[WIdN].Key); }
                KeyWdStr.ToLc(); KeyWdStr.ChangeChAll('_', ' ');
            } else {
                KeyWdStr = "";
            }
            // labeled document count
            PosDocN = NegDocN = 0;
            for (int DocN = 0; DocN < DocSet->Len(); DocN++) {
                if (DocSet->GetVecParam(DocN) > 0.1) { PosDocN++; }
                else if (DocSet->GetVecParam(DocN) < -0.1) { NegDocN++; }
            }
            // unlabeled document count
            for (int UnLblDIdN = 0; UnLblDIdN < UnlabeledDIdV.Len(); UnLblDIdN++){
                PBowSpV DocSpV = DocSet->GetAttrBowV(UnlabeledDIdV[UnLblDIdN]);
                double Dist = Md->GetRes(DocSpV);
                if (Dist > 0.0) { PosDocN++; }
                else { NegDocN++; }
            }
        }
    }
    // return
    return true;
}

void TBowAL::MarkQueryDoc(const int& QueryDId, const bool& AssignToCatP){
    // first we find the position of QueryDId in the DocSet
    int QueryDIdN = -1;
    for (int DocN = 0; DocN < DocSet->Len(); DocN++) {
        if (DocSet->GetAttrBowV(DocN)->GetDId() == QueryDId) {
            QueryDIdN = DocN; break; }
    }
    // if we have such a document in DocSet, than move it to labeled part
    if (QueryDIdN >= 0) {
        // remove the document from the unlabeled-list and add to the labeled-list
        UnlabeledDIdV.DelIfIn(QueryDIdN);
        LabeledDIdV.AddUnique(QueryDIdN);
        // add the category information to Bow and to TrainSet
        if (AssignToCatP){
            // adding query-document to category
            if (ALType == baltCat) { BowDocBs->AddDocCId(QueryDId, CId); }
            DocSet->SetVecParam(QueryDIdN, 1.0);
        } else {
            // marking the document is not in the category
            DocSet->SetVecParam(QueryDIdN, -1.0);
        }
    } else {
        InfoNotify(TStr::Fmt("Wrong QueryDId (%d)!", QueryDId)); //HACK!
    }
}

void TBowAL::MarkUnlabeledPosDocs(){
    // mark the rest of the positive documents with the target category
    PSVMModel Md = TSVMModel::NewClsLinear(DocSet, SvmC, SvmJ, LabeledDIdV);
    for (int UnLblDIdN = 0; UnLblDIdN < UnlabeledDIdV.Len(); UnLblDIdN++){
        PBowSpV DocSpV = DocSet->GetAttrBowV(UnlabeledDIdV[UnLblDIdN]);
        double Dist = Md->GetRes(DocSpV);
        if (Dist>0){
            BowDocBs->AddDocCId(DocSpV->GetDId(), CId);
        }
    }
}

void TBowAL::GetAllPosDocs(TIntV& PosDIdV){
    // first we collect all the positive documents from the labeled part
    for (int LblDIdN = 0; LblDIdN < LabeledDIdV.Len(); LblDIdN++) {
        if (DocSet->GetVecDId(LabeledDIdV[LblDIdN]) > 0.0) {
            PosDIdV.Add(DocSet->GetAttrBowV(LabeledDIdV[LblDIdN])->GetDId());
        }
    }
    // than we  classify positivne docs from the unlabeled part
    PSVMModel Md = TSVMModel::NewClsLinear(DocSet, SvmC, SvmJ, LabeledDIdV);
    for (int UnLblDIdN = 0; UnLblDIdN < UnlabeledDIdV.Len(); UnLblDIdN++){
        PBowSpV DocSpV = DocSet->GetAttrBowV(UnlabeledDIdV[UnLblDIdN]);
        double Dist = Md->GetRes(DocSpV);
        if (Dist>0){
            PosDIdV.Add(DocSpV->GetDId());
        }
    }
    // finalise
    PosDIdV.Pack(); PosDIdV.Sort();
}
