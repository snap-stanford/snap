//////////////////////////////////////////////////////////////////////////
// Semantic-Space
THash<TStr, TSemSpace::TSemSpcLoadF> TSemSpace::TypeToLoadFH;

bool TSemSpace::Reg(const TStr& TypeNm, const TSemSpcLoadF& LoadF) {
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

PSemSpace TSemSpace::Load(TSIn& SIn) {
    TStr TypeNm(SIn);
    int TypeKeyId=-1;
    if (TypeToLoadFH.IsKey(TypeNm, TypeKeyId)){
        TSemSpcLoadF LoadF=TypeToLoadFH[TypeKeyId];
        return (*LoadF())(SIn);
    } else {
        return NULL;
    }
}

double TSemSpace::GetAvgApprox(const TVec<PBowSpV>& BowSpV, const int& Dim) const {

    PMom Mom = TMom::New();
    for (int DocN = 0; DocN < BowSpV.Len(); DocN++) {
        //printf("d:%d\r", DocN);
        PBowSpV ProjSpV = this->ProjectSpV(BowSpV[DocN], Dim, false);
        Mom->Add(ProjSpV->GetNorm());
    }
    //printf("\n");
    Mom->Def();            
    return Mom->GetMean();
}

int TSemSpace::GetDimNeededForApprox(const TVec<PBowSpV>& BowSpV, 
        const double& ApproxThresh, double& ApproxVal, const int& MxDimDiff) const {
    
    IAssert(0.0 < ApproxThresh && ApproxThresh < 1.0);
    int BottomDim = 0, TopDim = this->GetVecs();
    // check if we have enough dimensions
    ApproxVal = GetAvgApprox(BowSpV, TopDim);
    if (ApproxVal < ApproxThresh) { return TopDim; }
    forever {
        if (TopDim - BottomDim < MxDimDiff) {  return TopDim; }
        IAssert(TopDim > BottomDim);
        const int SemSpcDim = (TopDim + BottomDim) / 2;
        ApproxVal = GetAvgApprox(BowSpV, SemSpcDim);
        if (ApproxVal < ApproxThresh) {
            IAssert(BottomDim < SemSpcDim);
            BottomDim = SemSpcDim;
        } else {
            IAssert(TopDim > SemSpcDim);
            TopDim = SemSpcDim;
        }
    }
    //return -1;
}

PBowDocPart TSemSpace::GetBowDocPart(const PBowDocBs& BowDocBs,
        const int& StartVecN, const int _Vecs, const double& Eps) const {
    PBowDocPart BowDocPart = TBowDocPart::New();
    const int Vecs = _Vecs == -1 ? this->GetVecs() : _Vecs;
    IAssert(Vecs < this->GetVecs());
    for (int VecN = StartVecN; VecN < Vecs; VecN++) {
        PBowSpV PosSpV, NegSpV; 
        this->GetVecBowSpV(VecN, PosSpV, NegSpV, Eps);

        if (PosSpV->Len() > 0) {
            PBowDocPartClust Clust = TBowDocPartClust::New(
                BowDocBs, TStr::Fmt("Vec%dPos", VecN), 0.0, TIntV(), PosSpV, NULL);
            BowDocPart->AddClust(Clust);
        }
        if (NegSpV->Len() > 0) {
            PBowDocPartClust Clust = TBowDocPartClust::New(
                BowDocBs, TStr::Fmt("Vec%dPos", VecN), 0.0, TIntV(), NegSpV, NULL);
            BowDocPart->AddClust(Clust);
        }
    }
    return BowDocPart;
}

void TSemSpace::GetVecBowSpV(const int& VecN, PBowSpV& PosSpV, 
        PBowSpV& NegSpV, const double& Eps) const {

    IAssert(0.0 <= Eps && Eps <= 1.0);
    TFltV Vec; GetVec(VecN, Vec);
    PosSpV = TBowSpV::New(); NegSpV = TBowSpV::New();
    const int Len = Vec.Len();
    for (int i = 0; i < Len; i++) {
        if (Vec[i] > Eps) {
            PosSpV->AddWIdWgt(i, Vec[i]);
        } else if (-Vec[i] > Eps) {
            NegSpV->AddWIdWgt(i, -Vec[i]);
        }
    }
}

void TSemSpace::GetWords(PBowDocBs BowDocBs, const int& VecN, 
        const int& TopWords, const double& TopWordsWgtPrc, 
        TStrFltPrV& PosWordStrWgtV, TStrFltPrV& NegWordStrWgtV) const {
    
    PBowSpV PosSpV, NegSpV; this->GetVecBowSpV(VecN, PosSpV, NegSpV);

    PosWordStrWgtV.Clr();
    if (PosSpV->Len() > 0) {
        PosSpV->GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtPrc, PosWordStrWgtV);
    }
    NegWordStrWgtV.Clr();
    if (NegSpV->Len() > 0) {
        NegSpV->GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtPrc, NegWordStrWgtV);
    }
}

void TSemSpace::SaveTxt(const TStr& FNm, PBowDocBs BowDocBs, const int& TopWords, 
        const double& TopWordsWgtPrc, const bool& ShowWgt) const {

    PSOut Out = TFOut::New(FNm);
    Out->PutStrLn("Semantic Space generation method: " + MethodeType);
    Out->PutLn();

    const int Vecs = GetVecs();
    for (int VecN = 0; VecN < Vecs; VecN++) {
        Out->PutStrLn(TStr::Fmt("Vector no.\t%d:", VecN));
        TStrFltPrV PosWordStrWgtV, NegWordStrWgtV;
        this->GetWords(BowDocBs, VecN, TopWords, TopWordsWgtPrc, 
            PosWordStrWgtV, NegWordStrWgtV);
        
        Out->PutStr("+\t");
        for (int i = 0; i < PosWordStrWgtV.Len(); i++) {
            if (ShowWgt) {
                Out->PutStr(TStr::Fmt("%s:%g\t", 
                    PosWordStrWgtV[i].Val1.CStr(), PosWordStrWgtV[i].Val2.Val));
            } else {
                Out->PutStr(TStr::Fmt("%s\t", PosWordStrWgtV[i].Val1.CStr()));
            }
        }
        Out->PutLn(); 
        Out->PutStr("-\t");
        for (int i = 0; i < NegWordStrWgtV.Len(); i++) {
            if (ShowWgt) {
                Out->PutStr(TStr::Fmt("%s:%g\t", 
                    NegWordStrWgtV[i].Val1.CStr(), NegWordStrWgtV[i].Val2.Val));
            } else {
                Out->PutStr(TStr::Fmt("%s\t", NegWordStrWgtV[i].Val1.CStr()));
            }
        }
        Out->PutLn(2);
    }
}

void TSemSpace::SaveTxtStat(const TStr& FNm, PSemSpace SemSp1, PBowDocBs BowBs1, 
        PBowDocWgtBs BowWgt1, PSemSpace SemSp2, PBowDocBs BowBs2, 
        PBowDocWgtBs BowWgt2, const bool& NormP, const TIntV& DIdV) {

    // project first set
    printf("project first set...\n");
    TIntV DIdV1 = DIdV; if (DIdV.Empty()) BowBs1->GetAllDIdV(DIdV1);
    PBowDocBs Set1 = SemSp1->Project(BowBs1, BowWgt1, DIdV1, SemSp1->GetVecs(), NormP);
    // project second set
    printf("project second set...\n");
    TIntV DIdV2 = DIdV; if (DIdV.Empty()) BowBs2->GetAllDIdV(DIdV2);
    PBowDocBs Set2 = SemSp2->Project(BowBs2, BowWgt2, DIdV2, SemSp2->GetVecs(), NormP);
    IAssert(Set1->GetDocs() == Set2->GetDocs());

    PSOut SOut = TFOut::New(FNm);
    printf("comparing %s vs. %s...\n", 
        SemSp1->GetMethodeType().CStr(), SemSp2->GetMethodeType().CStr());
    SOut->PutStrLn("DId, ||Vec1||, ||Vec2||, <Vec1/||Vec1||,Vec2/||Vec2||>");
    for (int DocC = 0, DocN = DIdV.Len(); DocC < DocN; DocC++) {
        printf("%d\r", DocC);
        TStr DocNm1 = BowBs1->GetDocNm(DIdV[DocC]), DocNm2 = BowBs2->GetDocNm(DIdV[DocC]);
        IAssert(DocNm1.Left(DocNm1.Len() - 3) == DocNm2.Left(DocNm2.Len() - 3));
        TStr NewDocNm1 = Set1->GetDocNm(DocC), NewDocNm2 = Set2->GetDocNm(DocC);
        IAssertR(DocNm1 == NewDocNm1, DocNm1 + "!=" + NewDocNm1);
        IAssertR(DocNm2 == NewDocNm2, DocNm2 + "!=" + NewDocNm2);

        PBowSpV Vec1 = Set1->GetDocSpV(DocC);
        PBowSpV Vec2 = Set2->GetDocSpV(DocC);
        const double n1 = Vec1->GetNorm();
        const double n2 = Vec2->GetNorm();
        const double dp = (n1*n2 > 0.0) ? TBowLinAlg::DotProduct(Vec1, Vec2)/(n1*n2) : 0.0;
        
        SOut->PutStrLn(TStr::Fmt("%d\t%.5f\t%.5f\t%.5f", DIdV[DocC].Val, n1, n2, dp));
    }
    SOut->Flush();
    printf("\ndone\n");
}

void TSemSpace::SaveApproxStat(const TStr& FNm, 
        const TVec<PBowSpV>& BowSpV, const int& Step) const {

    PSOut SOut = TFOut::New(FNm);
    SOut->PutStrLn("Testing approximation quality");
    SOut->PutStrLn(TStr::Fmt("Average of norms should converge to 1.0 as dim->%d", GetVecs()));
    SOut->PutStrLn("==============================================================");
    for (int Dims = Step; Dims < GetVecs(); Dims+=Step) {  
        PMom Mom = TMom::New();
        for (int DocN = 0; DocN < BowSpV.Len(); DocN++) {
            PBowSpV ProjSpV = ProjectSpV(BowSpV[DocN], Dims, false);
            Mom->Add(ProjSpV->GetNorm());
        }
        Mom->Def();
        SOut->PutStrLn(TStr::Fmt("Norm(dim:%d) = [avg: %.3f, sdev: %.3f]", Dims, Mom->GetMean(), Mom->GetSDev()));
    }
}

//////////////////////////////////////////////////////////////////////////
// Vector-Semantic-Space
bool TVecSemSpace::IsReg=TVecSemSpace::MkReg();

TVecSemSpace::TVecSemSpace(const TFltVV& BasisVV, 
        const TStr& _MethodeType): TSemSpace(_MethodeType) {

    const int Len = BasisVV.GetXDim();
    const int Dim = BasisVV.GetYDim();

    BasisV.Gen(Dim);
    for (int VecN = 0; VecN < Dim; VecN++) {
        BasisV[VecN].Gen(Len,0);
        for (int i = 0; i < Len; i++) {
            BasisV[VecN].Add(BasisVV(i, VecN)); }
    }
}

TVecSemSpace::TVecSemSpace(PSVMTrainSet Set, PPartialGS R, 
        const TVec<TFltV>& DrctV, const TStr& _MethodeType): TSemSpace(_MethodeType) { 

    IAssert(Set->Len() == R->GetCols()); 

    const int Dim = DrctV.Len();
    const int DocN = R->GetCols(), AproxN = R->GetRows();
    const TIntV& IdV = R->GetIdV();
    BasisV.Gen(Dim, 0); TFltV AlphV(DocN);
    
    // get basis Q from GS written in basis of vectors from Set
    TVec<TFltV> q(AproxN); R->GetBasisV(q);

    TFltV ww(AproxN); 
    for (int i = 0; i < Dim; i++) {
        printf("\r%d", i);
        const TFltV& w = DrctV[i];
        IAssert(w.Len() <= AproxN);
        // first we rewrite w into basis of vectors from Set
        ww.PutAll(0.0);
        for (int j = 0; j < w.Len(); j++)
            TLinAlg::AddVec(w[j], q[j], ww, ww);
        // than we sum linear combination of vectors from set
        AlphV.PutAll(0.0);
        for (int j = 0; j < AproxN; j++)
            AlphV[IdV[j]] = ww[j];
        TFltV Col; Set->LinComb(AlphV, Col); BasisV.Add(Col);
    }
    printf("\n");
};

TVecSemSpace::TVecSemSpace(const TStr& MatlabFName): TSemSpace("Matlab") {
    TLAMisc::LoadMatlabTFltVV(MatlabFName, BasisV);
}

TVecSemSpace::TVecSemSpace(const TStr& MatlabFName, 
        const TStr& MapFName): TSemSpace("Matlab") {

    // load matlab matrix
    TLAMisc::LoadMatlabTFltVV(MatlabFName, BasisV);
    // parse map
    PSIn SIn = TFIn::New(MapFName);
    TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloExcept);
    int Words = BasisV[0].Len(); TIntV WIdV(Words, 0);
    for (int WdN = 0; WdN < Words; WdN++) {
        if (WdN % 100 == 0) { printf("%d\r", WdN); }
        Lx.GetSym(syInt); WIdV.Add(Lx.Int);
        Lx.GetSym(syInt); IAssertR((Lx.Int-1) == WdN, TStr::Fmt("line %d", WdN));
        Lx.GetSym(syStr, syIdStr, syQStr);
        Lx.GetSym(syEof, syEoln);
    }
    printf("\n"); IAssert(WIdV.IsSorted());
    // remap matlab vectors
    const int NewSize = WIdV.Last()+1;
    for (int VecN = 0; VecN < BasisV.Len(); VecN++) {
        TFltV NewVec(NewSize); NewVec.PutAll(0.0);
        for (int WdN = 0; WdN < Words; WdN++) {
            NewVec[WIdV[WdN]] = BasisV[VecN][WdN];
        }
        BasisV[VecN] = NewVec;
    }
}

PBowDocBs TVecSemSpace::Project(const PBowDocBs& BowDocBs, 
        const PBowDocWgtBs& BowDocWgtBs, const TIntV& DIdV, 
        const int& Vecs, const bool& NormP) const {

    IAssert(Vecs <= BasisV.Len());
    PBowDocBs NewBowDocBs = TBowDocBs::New();

    // name dimensions in new bow
    for (int i = 0; i < Vecs; i++) {
        NewBowDocBs->AddWordStr("Comp" + TInt::GetStr(i)); }  // HACK

    // load projected documents into new bow
    for (int DocC = 0, DocN = DIdV.Len(); DocC < DocN; DocC++) {
        const int DId = DIdV[DocC];
        // make sparse vector for document
        TIntFltPrV PSpV(Vecs, 0); double VecNorm = 0.0;
        for (int i = 0; i < Vecs; i++) {
            const double Elt = TBowLinAlg::DotProduct(
                BasisV[i], BowDocWgtBs->GetSpV(DId));
            PSpV.Add(TIntFltPr(i, Elt)); 
            VecNorm += TMath::Sqr(Elt);
        }
        if (NormP && VecNorm > 0.0) {
            VecNorm = sqrt(VecNorm);
            for (int i = 0; i < Vecs; i++) {
                PSpV[i].Val2 = PSpV[i].Val2 / VecNorm;
            }
        }
        // load document categories
        const int CatN = BowDocBs->GetDocCIds(DId);
        TStrV CatNmV(CatN, 0);
        for (int CatC = 0; CatC < CatN; CatC++) { 
            CatNmV.Add(BowDocBs->GetCatNm(BowDocBs->GetDocCId(DId, CatC))); }
        // add document to new Bow
        NewBowDocBs->AddDoc(BowDocBs->GetDocNm(DId), CatNmV, PSpV); 
    }

    NewBowDocBs->AssertOk();
    return NewBowDocBs;
}

PBowSpV TVecSemSpace::ProjectSpV(const PBowSpV& SpV, 
        const int& Vecs, const bool& NormP) const {

    PSVMTrainSet Set = TBowTrainSet::New(1); 
    Set->AddAttrV(0, SpV, 1.0);
    PBowSpV NewSpV = TBowSpV::New(-1, Vecs);
    for (int i = 0; i < Vecs; i++) {
        NewSpV->AddWIdWgt(i, TBowLinAlg::DotProduct(BasisV[i], SpV)); }
    if (NormP) { NewSpV->PutUnitNorm(); }
    return NewSpV;
}

void TVecSemSpace::ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const {

    ProjSpV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjSpV.Add(TIntFltKd(i, TBowLinAlg::DotProduct(BasisV[i], SpV))); }
    if (NormP) { TLinAlg::Normalize(ProjSpV); }
}

void TVecSemSpace::ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const {

    ProjV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjV.Add(TBowLinAlg::DotProduct(BasisV[i], SpV)); }
    if (NormP) { TLinAlg::Normalize(ProjV); }
}

void TVecSemSpace::ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const {

    ProjSpV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjSpV.Add(TIntFltKd(i, TLinAlg::DotProduct(BasisV[i], SpV))); }
    if (NormP) { TLinAlg::Normalize(ProjSpV); }   
}

void TVecSemSpace::ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const {

    ProjV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjV.Add(TLinAlg::DotProduct(BasisV[i], SpV)); }
    if (NormP) { TLinAlg::Normalize(ProjV); }   
}

void TVecSemSpace::ProjectV(const TFltV& FullV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const {

    ProjV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjV.Add(TLinAlg::DotProduct(BasisV[i], FullV)); }
    if (NormP) { TLinAlg::Normalize(ProjV); }
}

//////////////////////////////////////////////////////////////////////////
// Sparse-Vector-Semantic-Space
bool TSpVecSemSpace::IsReg=TSpVecSemSpace::MkReg();

TSpVecSemSpace:: TSpVecSemSpace(const TVec<TIntFltKdV>& _BasisV, const TStr& _MethodeType): 
      TSemSpace(_MethodeType), BasisV(_BasisV) { 

    const int Vecs = BasisV.Len();
    VecDim = 0;
    for (int VecN = 0; VecN < Vecs; VecN++) {
        VecDim = TInt::GetMx(VecDim, BasisV.Last().Last().Key);
    }
    VecDim++;
};

PBowDocBs TSpVecSemSpace::Project(const PBowDocBs& BowDocBs, 
        const PBowDocWgtBs& BowDocWgtBs, const TIntV& DIdV, 
        const int& Vecs, const bool& NormP) const {

    IAssert(Vecs <= BasisV.Len());
    PBowDocBs NewBowDocBs = TBowDocBs::New();

    // name dimensions in new bow
    for (int i = 0; i < Vecs; i++) {
        NewBowDocBs->AddWordStr("Comp" + TInt::GetStr(i)); } // HACK

    // load projected documents into new bow
    for (int DocC = 0, DocN = DIdV.Len(); DocC < DocN; DocC++) {
        const int DId = DIdV[DocC];
        // make sparse vector for document
        TIntFltPrV PSpV(Vecs, 0); double VecNorm = 0.0;
        for (int i = 0; i < Vecs; i++) {
            const double Elt = TBowLinAlg::DotProduct(
                BasisV[i], BowDocWgtBs->GetSpV(DId));
            PSpV.Add(TIntFltPr(i, Elt)); 
            VecNorm += TMath::Sqr(Elt);
        }
        if (NormP && VecNorm > 0.0) {
            VecNorm = sqrt(VecNorm);
            for (int i = 0; i < Vecs; i++) {
                PSpV[i].Val2 = PSpV[i].Val2 / VecNorm;
            }
        }
        // load document categories
        const int CatN = BowDocBs->GetDocCIds(DId);
        TStrV CatNmV(CatN, 0);
        for (int CatC = 0; CatC < CatN; CatC++) { 
            CatNmV.Add(BowDocBs->GetCatNm(BowDocBs->GetDocCId(DId, CatC))); }
        // add document to new Bow
        NewBowDocBs->AddDoc(BowDocBs->GetDocNm(DId), CatNmV, PSpV); 
    }

    NewBowDocBs->AssertOk();
    return NewBowDocBs;
}

PBowSpV TSpVecSemSpace::ProjectSpV(const PBowSpV& SpV, 
        const int& Vecs, const bool& NormP) const {

    PBowSpV NewSpV = TBowSpV::New(-1, Vecs);
    for (int i = 0; i < Vecs; i++) {
        NewSpV->AddWIdWgt(i, TBowLinAlg::DotProduct(BasisV[i], SpV)); }
    if (NormP) { NewSpV->PutUnitNorm(); }
    return NewSpV;
}

void TSpVecSemSpace::ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const {

    ProjSpV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjSpV.Add(TIntFltKd(i, TBowLinAlg::DotProduct(BasisV[i], SpV))); }
    if (NormP) { TLinAlg::Normalize(ProjSpV); }
}

void TSpVecSemSpace::ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const {

    ProjV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjV.Add(TBowLinAlg::DotProduct(BasisV[i], SpV)); }
    if (NormP) { TLinAlg::Normalize(ProjV); }
}

void TSpVecSemSpace::ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const {

    ProjSpV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjSpV.Add(TIntFltKd(i, TLinAlg::DotProduct(BasisV[i], SpV))); }
    if (NormP) { TLinAlg::Normalize(ProjSpV); }  
}

void TSpVecSemSpace::ProjectSpV(const TIntFltKdV& SpV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const {

    ProjV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjV.Add(TLinAlg::DotProduct(BasisV[i], SpV)); }
    if (NormP) { TLinAlg::Normalize(ProjV); }  
}

void TSpVecSemSpace::ProjectV(const TFltV& FullV, const int& Vecs, 
        const bool& NormP, TFltV& ProjV) const {

    ProjV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        ProjV.Add(TLinAlg::DotProduct(FullV, BasisV[i])); }
    if (NormP) { TLinAlg::Normalize(ProjV); }     
}

int TSpVecSemSpace::GetVecLen() const {
    const int Vecs = GetVecs(); int MxDim = 0;
    for (int i = 0; i < Vecs; i++) {
        MxDim = TInt::GetMx(BasisV[i].Last().Key, 0); }
    return MxDim;
}

//////////////////////////////////////////////////////////////////////////
// KCCA-Semantic-Space
bool TKCCASemSpace::IsReg=TKCCASemSpace::MkReg();

void TKCCASemSpace::ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltPrV& ProjSpV) const {
    /* First we project SpV into GS generated subspace (spanned by ONS {qi} = GSBasisV):
            z = (y'q1, y'q2, ..., y'qn)
        where vectors qi are written in basis from vectors x:
            qi = (qi1, ... qin)
        now we calculate:
            y'qi = y'(qi1*x1 + ... + qin*xn) = qi1(y'x1) + ... qin(y'xn) */

    const int N = TrainSet->Len(); TFltV z(N, 0);
    TFltV DotProdV(N, 0);
    for (int i = 0; i < N; i++)
        DotProdV.Add(TrainSet->DotProduct(i, SpV));
    for (int i = 0; i < N; i++) 
        z.Add(TLinAlg::DotProduct(GSBasisV[i], DotProdV));

    // than we project z to semantic space basis
    ProjSpV.Gen(Vecs, 0); double Norm = 0.0;
    for (int i = 0; i < Vecs; i++) {
        const double Wgt = TLinAlg::DotProduct(z, BasisV[i]);
        ProjSpV.Add(TIntFltPr(i, Wgt));
        Norm += TMath::Sqr(Wgt);
    }

    if (NormP) {
        Norm = sqrt(Norm);
        for (int i = 0; i < Vecs; i++) { ProjSpV[i].Val2 /= Norm; }
    }
}

TKCCASemSpace::TKCCASemSpace(PSVMTrainSet Set, PPartialGS R, const TVec<TFltV>& DrctV, const TStr& _MethodeType): 
        TSemSpace(_MethodeType), TrainSet(Set) { 
    IAssert(Set->Len() == R->GetCols()); 
    // get semantic space basis
    BasisV = DrctV;
    // get basis from Partial Gram Schmidt
    R->GetBasisV(GSBasisV);
    // get "support vectors" from Partial GS
    const TIntV& DIdV = R->GetIdV();
    const int Dim = R->GetRows();
    TrainSet = TBowTrainSet::New(Dim);
    for (int VecC = 0; VecC < Dim; VecC++) {
        const int DocId = DIdV[VecC];
        TrainSet->AddAttrV(Set->GetVecDId(DocId), Set->GetAttrBowV(DocId), Set->GetVecParam(DocId));
    }

    IAssert(GSBasisV.Len() == TrainSet->Len() && GSBasisV[0].Len() == TrainSet->Len());
}

PBowDocBs TKCCASemSpace::Project(const PBowDocBs& BowDocBs, 
        const PBowDocWgtBs& BowDocWgtBs, const TIntV& DIdV, 
        const int& Vecs, const bool& NormP) const {

    IAssert(Vecs <= BasisV.Len());
    PBowDocBs NewBowDocBs = TBowDocBs::New();

    // name dimensions in new bow
    for (int i = 0; i < Vecs; i++) {
        NewBowDocBs->AddWordStr("Comp" + TInt::GetStr(i)); } // HACK

    // load projected documents into new bow
    PSVMTrainSet Set = TBowDocBs2TrainSet::NewBowNoCat(BowDocWgtBs, DIdV);
    for (int DocC = 0, DocN = DIdV.Len(); DocC < DocN; DocC++) {
        //printf("%d\r", DocC);
        const int DId = DIdV[DocC];
        // project document sparse vector into semantic space
        TIntFltPrV ProjVec; 
        ProjectSpV(BowDocWgtBs->GetSpV(DId), Vecs, NormP, ProjVec);
        // load document categories
        const int CatN = BowDocBs->GetDocCIds(DId);
        TStrV CatNmV(CatN, 0);
        for (int CatC = 0; CatC < CatN; CatC++) { 
            CatNmV.Add(BowDocBs->GetCatNm(BowDocBs->GetDocCId(DId, CatC))); }
        // add projected document to new Bow
        NewBowDocBs->AddDoc(BowDocBs->GetDocNm(DId), CatNmV, ProjVec); 
    }
    printf("\n");

    NewBowDocBs->AssertOk();
    return NewBowDocBs;
}

PBowSpV TKCCASemSpace::ProjectSpV(const PBowSpV& SpV, 
        const int& Vecs, const bool& NormP) const {

    TIntFltPrV ProjVec; ProjectSpV(SpV, Vecs, NormP, ProjVec);
    PBowSpV NewSpV = TBowSpV::New(-1, Vecs);
    for (int i = 0; i < Vecs; i++) {
        const TIntFltPr Wgt = ProjVec[i];
        NewSpV->AddWIdWgt(Wgt.Val1, Wgt.Val2);
    }
    return NewSpV;
}

void TKCCASemSpace::ProjectSpV(const PBowSpV& SpV, const int& Vecs, 
        const bool& NormP, TIntFltKdV& ProjSpV) const {
    /* First we project SpV into GS generated subspace (spanned by ONS {qi} = GSBasisV):
            z = (y'q1, y'q2, ..., y'qn)
        where vectors qi are written in basis from vectors x:
            qi = (qi1, ... qin)
        now we calculate:
            y'qi = y'(qi1*x1 + ... + qin*xn) = qi1(y'x1) + ... qin(y'xn) */

    const int N = TrainSet->Len(); TFltV z(N, 0);
    TFltV DotProdV(N, 0);
    for (int i = 0; i < N; i++)
        DotProdV.Add(TrainSet->DotProduct(i, SpV));
    for (int i = 0; i < N; i++) 
        z.Add(TLinAlg::DotProduct(GSBasisV[i], DotProdV));

    // than we project z to semantic space basis
    ProjSpV.Gen(Vecs, 0); double Norm = 0.0;
    for (int i = 0; i < Vecs; i++) {
        const double Wgt = TLinAlg::DotProduct(z, BasisV[i]);
        ProjSpV.Add(TIntFltKd(i, Wgt));
        Norm += TMath::Sqr(Wgt);
    }

    if (NormP) {
        Norm = sqrt(Norm);
        for (int i = 0; i < Vecs; i++) { ProjSpV[i].Dat /= Norm; }
    }
}

void TKCCASemSpace::GetVec(const int& VecN, TFltV& VecV) const {
    // basis vector written in GS basis
    const int GSDim = GSBasisV[0].Len();
    const TFltV& w = BasisV[VecN];
    IAssert(w.Len() == GSDim);
    // first we rewrite w into basis of vectors from Set
    TFltV AlphV(GSDim); AlphV.PutAll(0.0);
    for (int j = 0; j < GSDim; j++)
        TLinAlg::AddVec(w[j], GSBasisV[j], AlphV, AlphV);
    // than we sum linear combination of vectors from set
    TrainSet->LinComb(AlphV, VecV);
}

//////////////////////////////////////////////////////////////////////////
// Semantic-Space-Algorithms

//////////////////////////////////////////////////////////////////////////
// Kernel Canonical Correlation Analysis
void TSemSpaceAlg::CalcKCCA(const TFltVV& Ra, const TFltVV& Rb, 
        const double& ta, const double& tb, const TKCCACorrNrmType& CorrNrmType, 
        TVec<TFltV>& DrctAV, TVec<TFltV>& DrctBV, TFltV& CorrV) {

    IAssert(Ra.GetCols() == Rb.GetCols());
    const int Size = Ra.GetCols();
    IAssert(Ra.GetRows() >= Rb.GetRows());
    const int SubSizeA = Ra.GetRows();
    const int SubSizeB = Rb.GetRows();

    // STEP (2)
    // cholesky decomposition of ((1-tb)*Rb*Rb' + tb*I)
    printf("step (2)...\n");
    TFltVV R(SubSizeB, SubSizeB); // we calculate only upper triangle
    for (int i = 0; i < SubSizeB; i++) {
        for (int j = i; j < SubSizeB; j++) {
            double res = 0.0;
            for (int k = 0; k < Size; k++) {
                res += Rb(i,k) * Rb(j,k);  //HACK
            }
            R(i,j) = (1-tb)*res;
        }
        R(i,i) += tb;
    }
    // decomposition will be stored in lower triangle, diagonal is stored in p
    TFltV Rp(SubSizeB);
    printf("choleksy decomposition...");
    TNumericalStuff::CholeskyDecomposition(R, Rp);
    // save cholesky factor to upper triangle of R so we can feed it to InverseTriagonal
    for (int i = 0; i < SubSizeB; i++) {
        R(i,i) = Rp[i];  // change diagonal
        for (int j = i+1; j < SubSizeB; j++)
            R(i,j) = R(j,i); // lower triangle goes to upper triangle
    }
    //inverts R (we need it later), inverse is stored in upper triangle
    printf("invR...\n");
    TNumericalStuff::InverseTriagonal(R);

    // STEP (3)
    printf("step (3)...\n");
    // calculates upper triangle of ((1-ta)Ra*Ra' + ta*I)
    TFltVV A(SubSizeA, SubSizeA);
    printf("calculating: A...");
    for (int i = 0; i < SubSizeA; i++) {
        for (int j = i; j < SubSizeA; j++) {
            double res = 0.0;
            for (int k = 0; k < Size; k++) {
                res += Ra(i,k) * Ra(j,k);  //HACK
            }
            A(j,i) = A(i,j) = (1-ta)*res;
        }
        A(i,i) += ta;
    }
    // inverts A
    printf("invA..."); // inverse is in upper triangle
    TNumericalStuff::InverseSymetric(A);
    // copy upper triangle to lower one (as A is symetric), also save
    // lower triangle of A to R (as we'll need it later) and diagonal to Ad;
    TFltVV invA(SubSizeA, SubSizeA); //HACK invA instead of Ad!
    for (int i = 0; i < SubSizeA; i++) {
        invA(i,i) = A(i,i);
        for (int j = i+1; j < SubSizeA; j++)
            A(j,i) = invA(j,i) = invA(i,j) = A(i,j);
    }

    // goal: A <- invR' * Rb * Ra' * A * Ra * Rb' * invR
    printf("invR'*Rb*Ra'*invA*Ra*Rb'*invR...");
    // first Rb*Ra' (we'll use it several times, so we remember it)
    printf("1..");
    TFltVV RbRaT(SubSizeB, SubSizeA);
    for (int i = 0; i < SubSizeB; i++) {
        for (int j = 0; j < SubSizeA; j++) {
            double res = 0.0;
            for (int k = 0; k < Size; k++)
                res += Rb(i,k) * Ra(j,k);
            RbRaT(i,j) = res;
        }
    }
    // than B := Rb*Ra' * A
    printf("2..");
    TFltVV B(SubSizeB, SubSizeA);
    TLinAlg::Multiply(RbRaT, A, B);
    // than A := B * Ra*Rb' = B * (Rb*Ra')'
    A.Gen(SubSizeB, SubSizeB);
    for (int i = 0; i < SubSizeB; i++) {
        for (int j = 0; j < SubSizeB; j++) {
            double sum = 0.0;
            for (int k = 0; k < SubSizeA; k++)
                sum += B(i,k)*RbRaT(j,k);
            A(i,j) = sum;
        }
    }
    // than B = invR'*A
    printf("3..");
    B.Gen(SubSizeB, SubSizeB);
    for (int i = 0; i < SubSizeB; i++) {
        for (int j = 0; j < SubSizeB; j++) {
            double sum = 0.0;
            for (int k = 0; k <= i; k++)
                sum += R(k,i)*A(k,j);
            B(i,j) = sum;
        }
    }
    // than A = B*invR
    printf("4..");
    for (int i = 0; i < SubSizeB; i++) {
        for (int j = 0; j < SubSizeB; j++) {
            double sum = 0.0;
            for (int k = 0; k <= j; k++)
                sum += B(i,k)*R(k,j);
            A(i,j) = sum;
        }
    }

    // calculate eigen vectors and eigen values of A
    printf("\nsolving eigneproblem...");
    TFltV d(SubSizeB+1), e(SubSizeB+1);
    // first make A to tridiagonal
    printf("1..");
    TNumericalStuff::SymetricToTridiag(A, SubSizeB, d, e);
    // A is now transformation that made previous A tridiagonal
    // set B to identity matrix
    TLAMisc::FillIdentity(B);
    // find eigenvectors of tridiagonal matrix
    printf("2..");
    TNumericalStuff::EigSymmetricTridiag(d, e, SubSizeB, B);
    // B now has eigenvectors of tridiagonal matrix

    // calcualte eigen vectors (A*B), store in V
    printf("3..");
    TFltVV V(SubSizeB, SubSizeB);
    TLinAlg::Multiply(A, B, V);
    // delete first element of d
    for (int i = 1; i <= SubSizeB; i++) d[i-1] = d[i];
    d.DelLast(); //EigV = d;

    // STEP (4)
    printf("\nstep (4)...\n");
    // B = Wb = invR*V
    printf("Wb...");
    for (int i = 0; i < SubSizeB; i++) {
        for (int j = 0; j < SubSizeB; j++) {
            double sum = 0.0;
            for (int k = i; k < SubSizeB; k++)
                sum += R(i,k)*V(k,j);
            B(i,j) = sum;
        }
    }
    // normalize columns of B
    for (int j = 0; j < SubSizeB; j++) {
        double norm = 0.0;
        for (int i = 0; i < SubSizeB; i++)
            norm += TMath::Sqr(B(i,j));
        IAssert(norm > 0.0); norm = sqrt(norm);
        for (int i = 0; i < SubSizeB; i++)
            B(i,j) /= norm;
    }

    // A = Wa = 1/d[i] * invA * Ra * Rb' * B
    printf("Wa...");
    // first V := invA * (RbRaT')' (as we don't need V anymore)
    V.Gen(SubSizeA, SubSizeB);
    for (int i = 0; i < SubSizeA; i++) {
        for (int j = 0; j < SubSizeB; j++) {
            double sum = 0.0;
            for (int k = 0; k < SubSizeA; k++)
                sum += invA(i,k)*RbRaT(j,k);
            V(i,j) = sum;
        }
    }
    // than A := V * B
    A.Gen(SubSizeA, SubSizeB);
    TLinAlg::Multiply(V, B, A);
    // normalize columns in A
    for (int j = 0; j < SubSizeB; j++) {
        double norm = 0.0;
        for (int i = 0; i < SubSizeA; i++)
            norm += TMath::Sqr(A(i,j));
        IAssert(norm > 0.0); norm = sqrt(norm);
        for (int i = 0; i < SubSizeA; i++)
            A(i,j) /= norm;
    }

    // free unneeded space
    Rp.Clr(); invA.Clr(); e.Clr(); R.Clr(); RbRaT.Clr(); V.Clr();
    printf("\ndone...\n");

    // calculate direction pairs with highest correlation (puf...)
    // find bigest eigenvalues (they are all nonnegative)
    TFltIntKdV EigV(SubSizeB, 0);
    for (int i = 0; i < SubSizeB; i++) EigV.Add(TFltIntKd(d[i], i));
    EigV.Sort(false);

    // calculate first SubSize directions (since we did full eigenproblem)
    // we ignore first direction because kernel matrices weren't centralized!
    const int DrctN = SubSizeB-1; printf("SubSizeB = %d\n", SubSizeB);
    DrctAV.Gen(DrctN); DrctBV.Gen(DrctN); CorrV.Gen(DrctN, 0);
    TFltV RaTwa(Size), RbTwb(Size);
    for (int i = 0; i < DrctN; i++) {
        TFltV& wa = DrctAV[i]; wa.Gen(SubSizeA);
        TFltV& wb = DrctBV[i]; wb.Gen(SubSizeB);
        const int EigId = EigV[i+1].Dat;
        CorrV.Add(EigV[i+1].Key);
        for (int j = 0; j < SubSizeA; j++)
            wa[j] = A(j, EigId);
        for (int j = 0; j < SubSizeB; j++)
            wb[j] = B(j, EigId);

        // normalize direcion vectors so correlation is 1.0:
        // wa'*Caa*wa = wa'*Ra*Ra'*wa = (Ra'*wa)*(Ra'*wa) = ||Ra'*wa||^2 = Corr
        // x = k*y; ||x||^2 = Corr => (k*||y||)^2 = Corr => 
        // k^2 = Corr/(||y||^2) => k = sqrt(Corr/(||y||^2));
        if (CorrNrmType != kcntNone) {
            double Corr = 1.0; 
            if (CorrNrmType == kcntOne) { Corr = 1.0; }
            else if (CorrNrmType == kcntEigVal) { Corr = CorrV.Last()/CorrV[0]; }
            IAssert(Corr > 0.0);
            // normalize correlation of wa
            TLinAlg::MultiplyT(Ra, wa, RaTwa);
            const double KoefA = sqrt(Corr/TLinAlg::Norm2(RaTwa));
            TLinAlg::MultiplyScalar(KoefA, wa, wa);
            // assert new wa
            TLinAlg::MultiplyT(Ra, wa, RaTwa); 
            IAssert(TFlt::Abs(TLinAlg::Norm2(RaTwa) - Corr) < 1e-7);
            // normalize correlation of wb
            TLinAlg::MultiplyT(Rb, wb, RbTwb);
            const double KoefB = sqrt(Corr/TLinAlg::Norm2(RbTwb));
            TLinAlg::MultiplyScalar(KoefB, wb, wb);
            // assert new wb
            TLinAlg::MultiplyT(Rb, wb, RbTwb); 
            IAssert(TFlt::Abs(TLinAlg::Norm2(RbTwb) - Corr) < 1e-7);
        }
    }
}

void TSemSpaceAlg::CalcKCCA(PSVMTrainSet SetA, PSVMTrainSet SetB, const double& t, 
        const int& SpaceDim, const double& Eps, PPartialGS& Ra, PPartialGS& Rb, 
        const TKCCACorrNrmType& CorrNrmType, TVec<TFltV>& DrctAV, 
        TVec<TFltV>& DrctBV, TFltV& EigV) {

    IAssertR(SetA->Len() == SetB->Len() && SetB->Len() >= SpaceDim, 
        TStr::Fmt("(SetA->Len() == %d) != (SetB->Len() == %d)", SetA->Len(), SetB->Len()));
    IAssertR(0.0 <= t && t <= 1.0, TStr::Fmt("t=%d must be between 0.0 and 1.0!", t));
    IAssertR(0.0 <= Eps && Eps < 1.0, TStr::Fmt("Eps=%d must be between 0.0 and 1.0!", Eps));

    // STEP (1) - incomplete Cholesky decomposition (== partial GS in feature space)
    printf("step (1)...\n");
    TFltVV RaVV, RbVV; 
    printf("incomplete Cholesky... SetA\n");
    Ra = TPartialGS::New(SetA(), SetA->Len(), Eps); Ra->GetDocVV(RaVV);
    printf("incomplete Cholesky... SetB\n");
    Rb = TPartialGS::New(SetB(), SetB->Len(), Eps); Rb->GetDocVV(RbVV);
    const int NewSpaceDim = TInt::GetMn(RaVV.GetRows(), RbVV.GetRows());

    //TVec<TFltV> DrctAV, DrctBV; TFltV EigV;
    if (RaVV.GetRows() > RbVV.GetRows()) {
        CalcKCCA(RaVV, RbVV, t, t, CorrNrmType, DrctAV, DrctBV, EigV);
    } else {
        CalcKCCA(RbVV, RaVV, t, t, CorrNrmType, DrctBV, DrctAV, EigV);
    }

    IAssertR(DrctAV.Len() == NewSpaceDim-1  && DrctAV.Len() == NewSpaceDim-1,
        TStr::Fmt("%d, %d != %d", DrctAV.Len(), DrctBV.Len(), NewSpaceDim-1));

    if (DrctAV.Len() < SpaceDim) {
        printf("SpaceDim is lowerd to %d!\n", DrctAV.Len());
    } else {
        DrctAV.Trunc(SpaceDim); DrctBV.Trunc(SpaceDim);
    }
}

void TSemSpaceAlg::CalcKCCA(const TVec<PBowSpV>& BowSpVA, const TVec<PBowSpV>& BowSpVB, 
        const double& t, const int& SpaceDim, const double& Eps, PSemSpace& SpA, 
        PSemSpace& SpB, TFltV& EigV, const TKCCACorrNrmType& CorrNrmType) {

    // BowDocBs -> SVMTrainSet
    PSVMTrainSet SetA = TBowDocBs2TrainSet::NewBowNoCat(BowSpVA);
    PSVMTrainSet SetB = TBowDocBs2TrainSet::NewBowNoCat(BowSpVB);
    // calc KCCA
    PPartialGS Ra, Rb; TVec<TFltV> DrctAV, DrctBV;
    CalcKCCA(SetA, SetB, t, SpaceDim, Eps, Ra, Rb, CorrNrmType, DrctAV, DrctBV, EigV);
    // generate KCCA semantic space (only training set and GS-directions saved)
    SpA = TKCCASemSpace::New(SetA, Ra, DrctAV, "KCCA");
    SpB = TKCCASemSpace::New(SetB, Rb, DrctBV, "KCCA");
}

void TSemSpaceAlg::CalcVecKCCA(const TVec<PBowSpV>& BowSpVA, const TVec<PBowSpV>& BowSpVB, 
        const double& t, const int& SpaceDim, const double& Eps, PSemSpace& SpA, 
        PSemSpace& SpB, const TKCCACorrNrmType& CorrNrmType) {

    // BowDocBs -> SVMTrainSet
    PSVMTrainSet SetA = TBowDocBs2TrainSet::NewBowNoCat(BowSpVA);
    PSVMTrainSet SetB = TBowDocBs2TrainSet::NewBowNoCat(BowSpVB);
    // Calc KCCA
    PPartialGS Ra, Rb; TVec<TFltV> DrctAV, DrctBV; TFltV EigV;
    CalcKCCA(SetA, SetB, t, SpaceDim, Eps, Ra, Rb, CorrNrmType, DrctAV, DrctBV, EigV);
    // Generate Vector-basis semantic space (full vectors for basis computed)
    SpA = TVecSemSpace::New(SetA, Ra, DrctAV, "KCCA-Vec");
    SpB = TVecSemSpace::New(SetB, Rb, DrctBV, "KCCA-Vec");
}

//////////////////////////////////////////////////////////////////////////
// Decompose kernel matrix
void TSemSpaceAlg::DecomposeKernelMatrix(PSVMTrainSet Set, const int& Dim, 
        TVec<TFltV>& CompV, const bool& CenterMatrix) {
    const int Size = Set->Len();

    // calcualte kernel matrix
    //printf("calculating kernel matrix...\n");
    TFltVV K; TKernelUtil::CalcKernelMatrix(Set, K);
    //printf("  kernel matrix: %d x %d\n", K.GetRows(), K.GetCols());
    // center kernel matrix
    if (CenterMatrix) {
        //printf("center kernel matrix...\n");
        TKernelUtil::CenterKernelMatrix(K);
    }
    // calculate eigen vectors of K
    //printf("tridiagonalise kernel matrix...\n");
    TFltV d(Size+1), e(Size+1);
    TNumericalStuff::SymetricToTridiag(K, Size, d, e);
    TFltVV V(Size, Size); TLAMisc::FillIdentity(V);
    //printf("get eigen vectors...\n");
    TNumericalStuff::EigSymmetricTridiag(d, e, Size, V);
    // eigen vectors are now columns of (K*V)

    // sort, inv and sqrt diagonal
    TFltIntKdV EigV(Size, 0);
    for (int i = 1; i <= Size; i++) {
        EigV.Add(TFltIntKd(d[i], i-1));
        d[i-1] = sqrt(1.0/d[i]);
    }
    EigV.Sort(false); d.DelLast();

    // calculates coeficients for first principal components
    //printf("calc principal components...\n");
    CompV.Gen(Dim);
    for (int i = 0; i < Dim; i++) {
        //printf("<%.5f>", sqrt(EigV[i].Key.Val));
        const int ColId = EigV[i].Dat;
        TFltV& v = CompV[i]; v.Gen(Size);
        TLinAlg::Multiply(K, V, ColId, v); 
        //double k = d[ColId];
        //for (j = 0; j < Size; j++) v[i] *= k;
        TLinAlg::MultiplyScalar(d[ColId], v, v);
    }
    //printf("done...\n");
}

//////////////////////////////////////////////////////////////////////////
// Kernel Principal Component Analysis
PSemSpace TSemSpaceAlg::CalcKPCA(PSVMTrainSet Set) {
    const int Dim = Set->Len(); TVec<TFltV> CompV;
    DecomposeKernelMatrix(Set, Dim, CompV, true);
    TVec<TFltV> PrCompV(CompV.Len());
    for (int i = 0; i < PrCompV.Len(); i++)
        Set->LinComb(CompV[i], PrCompV[i]);

    return TVecSemSpace::New(PrCompV, "KPCA");
}

PSemSpace TSemSpaceAlg::CalcKPCA(const TVec<PBowSpV>& BowSpV) {
    PSVMTrainSet Set = TBowDocBs2TrainSet::NewBowNoCat(BowSpV);
    return CalcKPCA(Set);
}

PSemSpace TSemSpaceAlg::CalcKPCA(PBowDocWgtBs BowDocWgtBs, const TIntV& DIdV) {
    PSVMTrainSet Set = TBowDocBs2TrainSet::NewBowNoCat(BowDocWgtBs, DIdV);
    return CalcKPCA(Set);
}

//////////////////////////////////////////////////////////////////////////
// Full Latent Semantic Indexing
PSemSpace TSemSpaceAlg::CalcFullLSI(PSVMTrainSet Set, const int& SpaceDim) {
    const int Dim = Set->Len(); TVec<TFltV> CompV;
    //printf("decomposing...\n");
    DecomposeKernelMatrix(Set, Dim, CompV, false);
    const int CompN = TInt::GetMn(CompV.Len(), SpaceDim+1);
    //printf("transforming dual representation to primal...\n");
    TVec<TFltV> PrCompV(CompN-1);
    for (int i = 1; i < CompN ; i++) {
        Set->LinComb(CompV[i], PrCompV[i-1]);
    }
    //printf("FullLSI done...\n");
    return TVecSemSpace::New(PrCompV, "FullLSI");
}

PSemSpace TSemSpaceAlg::CalcFullLSI(const TVec<PBowSpV>& BowSpV, const int& SpaceDim) {
    PSVMTrainSet Set = TBowDocBs2TrainSet::NewBowNoCat(BowSpV);
    return CalcFullLSI(Set, SpaceDim);
}

PSemSpace TSemSpaceAlg::CalcFullLSI(PBowDocWgtBs BowDocWgtBs, 
        const TIntV& DIdV, const int& SpaceDim) {
    PSVMTrainSet Set = TBowDocBs2TrainSet::NewBowNoCat(BowDocWgtBs, DIdV);
    return CalcFullLSI(Set, SpaceDim);
}

//////////////////////////////////////////////////////////////////////////
// Sparse Latent Semantic Indexing (calls TSparseSVD for Lanczos algorith!)
PSemSpace TSemSpaceAlg::CalcSparseLSI(TMatrix& TermDocM, const int& _SpaceDim, 
        const int& _MaxLanczos, const TSpSVDReOrtoType& ReOrtoType) {
    //IAssertR(SpaceDim <= TermDocM.GetCols(), TStr::Fmt("%d <= %d", SpaceDim, TermDocM.GetCols()));
    const int SpaceDim = TInt::GetMn(_SpaceDim, TermDocM.GetCols());
    const int MaxLanczos = TInt::GetMn(_MaxLanczos, TermDocM.GetCols());
    IAssertR(SpaceDim <= MaxLanczos, TStr::Fmt("%d <= %d", SpaceDim, MaxLanczos));
    // calculate singular triplets
    TFltV SgnV; TFltVV U, V; const int SgnN = SpaceDim;
    if (TermDocM.GetCols() <= TermDocM.GetRows()) {
        // more words than docs, we calculate eigen vectors of A'A
        TSparseSVD::LanczosSVD(TermDocM, SgnN, 
            TInt::GetMn(SgnN*4, MaxLanczos), ReOrtoType, SgnV, U, V);
    } else {
	    // more docs than words, we calculate eigen vectors of AA'
	    TermDocM.Transpose();
	    TSparseSVD::LanczosSVD(TermDocM, SgnN, 
            TInt::GetMn(SgnN*4, MaxLanczos), ReOrtoType, SgnV, V, U);
	    TermDocM.Transpose();
    }
    IAssert(U.GetYDim() <= SgnN);
    //TLinAlg::AssertOrtogonality(U, 1e-3);
    // make semantic space
    return TVecSemSpace::New(U, "SparseLSI");
}

PSemSpace TSemSpaceAlg::CalcSparseLSI(PSVMTrainSet Set, const int& SpaceDim, 
        const int& MaxLanczos, const TSpSVDReOrtoType& ReOrtoType) {
    TSTSetMatrix TermDocM(Set);
    return CalcSparseLSI(TermDocM, SpaceDim, 
        TInt::GetMn(MaxLanczos, Set->Len()), ReOrtoType);
}

PSemSpace TSemSpaceAlg::CalcSparseLSI(const TVec<PBowSpV>& BowSpV,
        const int& SpaceDim, const int& MaxLanczos, const TSpSVDReOrtoType& ReOrtoType) {
    TBowMatrix TermDocM(BowSpV);
    return CalcSparseLSI(TermDocM, SpaceDim, 
        TInt::GetMn(MaxLanczos, BowSpV.Len()), ReOrtoType);
}

PSemSpace TSemSpaceAlg::CalcSparseLSI(PBowDocWgtBs BowDocWgtBs, const TIntV& DIdV,
        const int& SpaceDim, const int& MaxLanczos, const TSpSVDReOrtoType& ReOrtoType) {
    TBowMatrix TermDocM(BowDocWgtBs, DIdV);
    return CalcSparseLSI(TermDocM, SpaceDim, 
        TInt::GetMn(MaxLanczos, DIdV.Len()), ReOrtoType);
}

