//////////////////////////////////////////////////////////////////////////
// Cross-Validation-Tools
void TCrossValid::PrepareFolds(const int& Folds, TIntV& DIdV,
        const int& Seed, TVec<TIntV>& DIdVFoldV) {

    IAssert(Folds > 1);
    IAssertR(DIdV.Len()/Folds > 1,
        TStr::Fmt("To little documents per fold (%d/%d)", DIdV.Len(), Folds));
    // reserve space
    DIdVFoldV.Gen(Folds);
    const int Len = DIdV.Len();
    for (int FoldN = 0; FoldN < Folds; FoldN++)
        DIdVFoldV[FoldN].Gen(Len/Folds, 0);
    // fill folds
    int DocN = 0;
    TRnd Rnd(Seed); DIdV.Shuffle(Rnd);
    while (DocN < Len) {
        int FoldN = 0;
        while (DocN < Len && FoldN < Folds) {
            DIdVFoldV[FoldN].Add(DIdV[DocN]);
            FoldN++; DocN++;
        }
    }
    // arrange id-s
    DIdV.Sort();
    for (int FoldN = 0; FoldN < Folds; FoldN++) {
        DIdVFoldV[FoldN].Sort();
    }
}

void TCrossValid::PrepareSplit(const int& FoldN, const TVec<TIntV>& DIdVFoldV,
                               TIntV& TrainSubDIdV, TIntV& TestSubDIdV) {
    TrainSubDIdV.Clr(); TestSubDIdV.Clr();
    for (int n = 0; n < DIdVFoldV.Len(); n++) {
        if (n == FoldN) {
            TestSubDIdV.AddV(DIdVFoldV[n]);
        } else {
            TrainSubDIdV.AddV(DIdVFoldV[n]);
        }
    }
    TrainSubDIdV.Sort(); TestSubDIdV.Sort();
}

void TCrossValid::PrepareSubDIdV(const TIntV& SubSet, const int& DataSetLen, TIntV& DIdV) {
    if (SubSet.Empty()) {
        DIdV.Gen(DataSetLen, 0);
        for (int i = 0; i < DataSetLen; i++) DIdV.Add(i);
    } else {
        DIdV = SubSet;
    }
}

//////////////////////////////////////////////////////////////////////////
// Sparse-Training-Set
bool TSparseTrainSet::IsReg=TSparseTrainSet::MkReg();

TSparseTrainSet::TSparseTrainSet(const TIntV& VecIdV, const int& _MaxDim, const TFltV& _ClsV,
        const TFltV& _NormV, const TVec<TIntFltKdV>& _TrainV): TSVMTrainSet(ststSparse) {

    if (VecIdV.Empty()) {
        MaxDim = _MaxDim; ClsV = _ClsV;
        NormV = _NormV; TrainV = _TrainV;
    } else {
        MaxDim = _MaxDim;
        const int Vecs = VecIdV.Len();
        ClsV.Gen(Vecs, 0); NormV.Gen(Vecs, 0); TrainV.Gen(Vecs, 0);
        for (int VecN = 0; VecN < Vecs; VecN++) {
            const int VecId = VecIdV[VecN];
            ClsV.Add(_ClsV[VecId]);
            NormV.Add(_NormV[VecId]);
            TrainV.Add(_TrainV[VecId]);
        }
    }
}

void TSparseTrainSet::SaveTxt(const TStr& OutFNm) const {
    PSOut SOut = TFOut::New(OutFNm);
    for (int VecN = 0; VecN < TrainV.Len(); VecN++) {
        const TIntFltKdV& AttrV = TrainV[VecN];
        SOut->PutStr(TStr::Fmt("%f", GetVecParam(VecN)));
        for (int a = 0; a < AttrV.Len(); a++) {
            SOut->PutStr(TStr::Fmt(" %d:%f", AttrV[a].Key(), AttrV[a].Dat()));
        }
        SOut->PutLn();
    }
    SOut->Flush();
}

void TSparseTrainSet::SaveMatlab(const TStr& FName) const {
    PSOut out = TFOut::New(FName);
    int ColN = Len();
    for (int ColId = 0; ColId < ColN; ColId++) {
        const TIntFltKdV& Vec = TrainV[ColId];
        for (int RowId = 0, RowN = Vec.Len(); RowId < RowN; RowId++) {
            out->PutStr(TInt::GetStr(Vec[RowId].Key + 1) + TStr(" ") + TInt::GetStr(ColId + 1) + TStr(" "));
            out->PutStr(TFlt::GetStr(Vec[RowId].Dat, 20, 18));
            out->PutCh('\n');
        }
    }
}

double TSparseTrainSet::DotProduct(const int& VecId1, double* vec2, const int& n) const {
    double Res = 0.0;
    const TIntFltKdV& V1 = TrainV[VecId1];
    for (int i = 0; i < V1.Len(); i++) {
        const int key = V1[i].Key;
        if (key < n) Res += V1[i].Dat * vec2[key];
    }
    return Res;
}

void TSparseTrainSet::AddVec(const int& VecId1, double* vec2,
        const int& n, const double& K) const {
    const TIntFltKdV& V1 = TrainV[VecId1];
    for (int i = 0; i < V1.Len(); i++) {
        Assert(V1[i].Key < n);
        vec2[V1[i].Key] += K * V1[i].Dat;
    }
}

int TSparseTrainSet::AddAttrV(const TIntFltKdV& AttrV,
        const double& Cls, const bool& Normalize) {

    double Norm = TLinAlg::Norm2(AttrV);
    IAssertR(Norm >= 0.0, TStr::Fmt("%g (len = %d)", Norm, AttrV.Len()));
    NormV.Add(Norm); TrainV.Add(AttrV); TrainV.Last().Pack(); ClsV.Add(Cls);

    if (AttrV.Empty()) {
        TrainV.Last().Add(TIntFltKd(1, 0.0));
    } else {
        MaxDim = TInt::GetMx(int(MaxDim), AttrV.Last().Key+1);
        if (Normalize) { TLinAlg::Normalize(TrainV.Last()); }
    }

    return TrainV.Len()-1;
}

void TSparseTrainSet::Shuffle(TRnd& Rnd) {
    const int Vals = TrainV.Len();
    for (int ValN=0; ValN<Vals-1; ValN++) {
        const int NewValN = ValN+Rnd.GetUniDevInt(Vals-ValN);
        TrainV.Swap(ValN, NewValN);
        ClsV.Swap(ValN, NewValN);
        NormV.Swap(ValN, NewValN);
    }
}

//void TSparseTrainSet::Attr2Cls(const int& AttrId) {
//    const int VecN = TrainV.Len();
//    for (int VecC = 0; VecC < VecN; VecC++) {
//        TIntFltKdV& Vec = TrainV[VecC];
//        const int AttrPos = Vec.SearchBin(TIntFltKd(AttrId));
//        if (AttrPos != -1) {
//            ClsV[VecC] = Vec[AttrPos].Dat;
//            Vec[AttrPos].Dat = 0.0;
//        } else {
//            ClsV[VecC] = 0.0;
//        }
//    }
//}
//
//void TSparseTrainSet::Cls2Attr(const int& AttrId) {
//    const int VecN = TrainV.Len();
//    for (int VecC = 0; VecC < VecN; VecC++) {
//        TIntFltKdV& Vec = TrainV[VecC];
//        const int AttrPos = Vec.SearchBin(TIntFltKd(AttrId));
//        if (AttrPos != -1) {
//            Vec[AttrPos].Dat = ClsV[VecC];
//        } else {
//            IAssert(TFlt::Abs(ClsV[VecC]) < 1e-7);
//        }
//    }
//}
//
//void TSparseTrainSet::Attr2Vec(const int& AttrId, TFltV& AttrVec) {
//    const int VecN = TrainV.Len(); AttrVec.Gen(VecN, 0);
//    for (int VecC = 0; VecC < VecN; VecC++) {
//        TIntFltKdV& Vec = TrainV[VecC];
//        const int AttrPos = Vec.SearchBin(TIntFltKd(AttrId));
//        if (AttrPos != -1) {
//            AttrVec.Add(Vec[AttrPos].Dat);
//            Vec[AttrPos].Dat = 0.0;
//        } else {
//            AttrVec.Add(0.0);
//        }
//    }
//}
//
//void TSparseTrainSet::Vec2Attr(const int& AttrId, TFltV& AttrVec) {
//    const int VecN = TrainV.Len(); IAssert(VecN == AttrVec.Len());
//    for (int VecC = 0; VecC < VecN; VecC++) {
//        TIntFltKdV& Vec = TrainV[VecC];
//        const int AttrPos = Vec.SearchBin(TIntFltKd(AttrId));
//        if (AttrPos != -1) {
//            Vec[AttrPos].Dat = AttrVec[VecC];
//        } else {
//            IAssert(TFlt::Abs(AttrVec[VecC]) < 1e-7);
//        }
//    }
//}

PSVMTrainSet TSparseTrainSet::LoadTxt(PSIn SIn, const bool& Normalize, const int& verbosity) {
    //TODO rewrite using TLx
    TMem buffer;
    TMem::LoadMem(SIn, buffer);
    if (buffer[buffer.Len()-1] != '\n') buffer += '\n';  //we add new line at the end
    int len = buffer.Len(), n = 0;

    char ch, *s; int vecsP = 0, vecsM = 0, line = 0;
    double cls; TIntFltKdV vec;
    PSVMTrainSet docs = TSparseTrainSet::New();

    //if input file empty, do nothing
    if (len == 0) return docs;

    //reads vectors from file
    //if line begins with # than it's a comment
    //if # appears in the middle of the line than text on the right is comment
    //in file one line is one vector, it begins with it's class [ 1 | -1 ]
    //followed with it's nonzero components in form: ID:VAL
    ch = buffer[n++];
    while (n < len) {
        line++;
        vec.Clr(false);

        if (ch != '#') {
            //we read vector from this line
            while (ch == ' ') ch = buffer[n++]; //ignores all spaces

            //first we read it's class
            TChA clsChA;
            while (ch != ' ' && ch != '\r' && ch != '\n') {
                clsChA += ch;
                ch = buffer[n++];
            }

            if (clsChA.Len() > 0) {
                cls = strtod(clsChA.CStr(), &s);

                //we read nonzero components of the vector
                int part = 0;
                TChA idChA, valChA;
                while (ch != '\r' && ch != '\n' && ch != '#') {
                    if (part == 0 && ch == ' ') {
                        //do nothing, we ignore spaces
                    } else if (part == 0 && ch != ' ') {
                        part = 1;     //now we started reading ID
                        Assert('0' <= ch && ch <= '9');
                        idChA += ch;
                    } else if (part == 1 && ch != ':') {
                        Assert('0' <= ch && ch <= '9');
                        idChA += ch;  //reding ID
                    } else if (part == 1 && ch == ':') {
                        part = 2;     //we stoped reading ID, swich to value
                    } else if (part == 2 && ch != ' ') {
                        Assert(('0' <= ch && ch <= '9') || ch == '.');
                        valChA += ch; //reading value
                    } else if (part == 2 && ch == ' ') {
                        //we just read one component, now we save it
                        int id = strtol(idChA.CStr(), &s, 10);
                        double val = strtod(valChA.CStr(), &s);
                        vec.Add(TIntFltKd(id, val));
                        //ids.Add(id); vals.Add(val);
                        idChA.Clr(); valChA.Clr();
                        part = 0;
                    } else {
                        printf("error in %d, unexpected charater %c!\n", line, ch);
                        Assert(false);
                    }

                    ch = buffer[n++];
                    if (part == 2 && (ch == '\r' || ch == '\n' || ch == '#')) {
                        //this is the last component in this line
                        int id = strtol(idChA.CStr(), &s, 10);
                        double val = strtod(valChA.CStr(), &s);
                        vec.Add(TIntFltKd(id, val));
                        //ids.Add(id); vals.Add(val);
                        idChA.Clr(); valChA.Clr();
                        part = 0;
                    }
                } //while
                if (part != 0) printf("unexpected end of line %d\n", line);
                Assert(part == 0); //we shouldn't stop in the middle of reading!

                docs->AddAttrV(vec, cls, Normalize);
                if (cls > 0) vecsP++;
                else vecsM++;

                if (verbosity > 0 && (vecsP + vecsM)%100 == 0) printf("%d\r", vecsP + vecsM);

                while (ch != '\n') ch = buffer[n++]; //we read line till the end
            }
        } else {
            //this line is a comment, we ignore it
            while (ch != '\n') ch = buffer[n++];
        }
        if (n < len) ch = buffer[n++];
    }
    printf("\n");

    if (verbosity > 1)
        printf("\ninput: %d = %d + %d vectors\n", vecsP + vecsM, vecsP, vecsM);
    else if (verbosity > 0)
        printf("\n");
    return docs;
}

//////////////////////////////////////////////////////////////////////////
// Dense-Training-Set
bool TDenseTrainSet::IsReg=TDenseTrainSet::MkReg();

TDenseTrainSet::TDenseTrainSet(const TIntV& VecIdV, const int& _MaxDim, const TFltV& _ClsV,
        const TFltV& _NormV, const TVec<TFltV>& _TrainV): TSVMTrainSet(ststDense) {

    if (VecIdV.Empty()) {
        MaxDim = _MaxDim; ClsV = _ClsV;
        NormV = _NormV; TrainV = _TrainV;
    } else {
        MaxDim = _MaxDim;
        const int Vecs = VecIdV.Len();
        ClsV.Gen(Vecs, 0); NormV.Gen(Vecs, 0); TrainV.Gen(Vecs, 0);
        for (int VecN = 0; VecN < Vecs; VecN++) {
            const int VecId = VecIdV[VecN];
            ClsV.Add(_ClsV[VecId]);
            NormV.Add(_NormV[VecId]);
            TrainV.Add(_TrainV[VecId]);
        }
    }
}

PSVMTrainSet TDenseTrainSet::LoadFromSparse(PSVMTrainSet SparseSet) {
    const int Vecs = SparseSet->Len();
    const int Dims = SparseSet->Dim();
    PSVMTrainSet DenseSet = TDenseTrainSet::New(Vecs);
    TFltV AttrDenseV(Dims);
    for (int VecN = 0; VecN < SparseSet->Len(); VecN++) {
        const TIntFltKdV& AttrSparseV = SparseSet->GetAttrSparseV(VecN);
        AttrDenseV.PutAll(0.0);
        TLinAlg::AddVec(1.0, AttrSparseV, AttrDenseV);
        DenseSet->AddAttrV(AttrDenseV, SparseSet->GetVecParam(VecN), false);
    }
    return DenseSet;
}

void TDenseTrainSet::SaveTxt(const TStr& OutFNm) const {
    PSOut SOut = TFOut::New(OutFNm);
    for (int VecN = 0; VecN < TrainV.Len(); VecN++) {
        const TFltV& AttrV = TrainV[VecN];
        SOut->PutStr(TStr::Fmt("%f", GetVecParam(VecN)));
        for (int a = 0; a < AttrV.Len(); a++) {
            SOut->PutStr(TStr::Fmt(" %d:%f", a+1, AttrV[a]()));
        }
        SOut->PutLn();
    }
    SOut->Flush();
}

int TDenseTrainSet::AddAttrV(const TFltV& AttrV, const double& Cls, const bool& Normalize) {

    IAssert(!AttrV.Empty());
    IAssert(MaxDim == 0 || AttrV.Len() == MaxDim);

    MaxDim = AttrV.Len();
    double Norm = TLinAlg::Norm2(AttrV); NormV.Add(Norm);
    TrainV.Add(AttrV); TrainV.Last().Pack(); ClsV.Add(Cls);
    if (Normalize) { TLinAlg::Normalize(TrainV.Last()); }

    return TrainV.Len()-1;
}

double TDenseTrainSet::DotProduct(const int& VecId1, double* vec2, const int& n) const {
    IAssert(n == MaxDim);
    const TFltV& V1 = TrainV[VecId1];
    double Res = V1[0]*vec2[0];
    for (int i = 1; i < MaxDim; i++)
        Res += V1[i]*vec2[i];
    return Res;
}

void TDenseTrainSet::AddVec(const int& VecId1, double* vec2,
                            const int& n, const double& K) const {
    IAssert(n == MaxDim);
    const TFltV& V1 = TrainV[VecId1];
    for (int i = 0; i < MaxDim; i++)
        vec2[i] += K*V1[i];
}


void TDenseTrainSet::Shuffle(TRnd& Rnd) {
    const int Vals = TrainV.Len();
    for (int ValN=0; ValN<Vals-1; ValN++) {
        const int NewValN = ValN+Rnd.GetUniDevInt(Vals-ValN);
        TrainV.Swap(ValN, NewValN);
        ClsV.Swap(ValN, NewValN);
        NormV.Swap(ValN, NewValN);
    }
}

PSVMTrainSet TDenseTrainSet::ProjectLin(PSVMTrainSet Set,
        const TFltVV& Basis, const bool& Normalize, const int& NewDim) {

    IAssert(NewDim == -1 || (NewDim >= 0 && NewDim <= Basis.GetYDim()));
    const int Dim = (NewDim == -1) ? Basis.GetYDim() : NewDim; // new dimensinality
    const int Len = Basis.GetXDim();

    // rewrite basis vectors so we can feed them to PSVMTrainSet
    TVec<TFltV> NewBasis(Dim);
    for (int VecC = 0; VecC < Dim; VecC++) {
        NewBasis[VecC].Gen(Len,0);
        for (int i = 0; i < Len; i++) {
            NewBasis[VecC].Add(Basis(i, VecC));
        }
    }

    return ProjectLin(Set, NewBasis, Normalize);
}

PSVMTrainSet TDenseTrainSet::ProjectLin(PSVMTrainSet Set,
        const TVec<TFltV>& Basis, const bool& Normalize, const int& NewDim) {

    IAssert(NewDim == -1 || (NewDim >= 0 && NewDim <= Basis.Len()));
    const int VecN = Set->Len(); // number of elements
    const int Dim = (NewDim == -1) ? Basis.Len() : NewDim; // new dimensinality
    PSVMTrainSet NewSet = TDenseTrainSet::New(VecN);

    // project
    for (int VecC = 0; VecC < VecN; VecC++) {
        if (VecC % 100 == 0) printf("%d\r", VecC);
        TFltV Vec(Dim, 0);
        // project elements of Set to VecC-th basis vector
        for (int i = 0; i < Dim; i++) {
            Vec.Add(Set->DotProduct(VecC, Basis[i]));
        }
        NewSet->AddAttrV(Vec, Set->GetVecParam(VecC), Normalize);
    }
    printf("\n");

    return NewSet;
}

//////////////////////////////////////////////////////////////////////////
// Bow-Training-Set
bool TBowTrainSet::IsReg=TBowTrainSet::MkReg();

TBowTrainSet::TBowTrainSet(const TIntV& VecIdV, const int& _MaxDim, const TIntV& _DIdV,
        const TVec<PBowSpV>& _TrainV, const TFltV& _ClsV): TSVMTrainSet(ststBow) {

    if (VecIdV.Empty()) {
        MaxDim = _MaxDim; DIdV = _DIdV;
        TrainV = _TrainV; ClsV = _ClsV;
    } else {
        MaxDim = _MaxDim;
        const int Vecs = VecIdV.Len();
        DIdV.Gen(Vecs, 0); TrainV.Gen(Vecs, 0); ClsV.Gen(Vecs, 0);
        for (int VecN = 0; VecN < Vecs; VecN++) {
            const int VecId = VecIdV[VecN];
            DIdV.Add(_DIdV[VecId]);
            TrainV.Add(_TrainV[VecId]);
            ClsV.Add(_ClsV[VecId]);
        }
    }
}

void TBowTrainSet::SaveTxt(const TStr& OutFNm) const {
    PSOut SOut = TFOut::New(OutFNm);
    for (int VecN = 0; VecN < TrainV.Len(); VecN++) {
        PBowSpV AttrV = TrainV[VecN];
        SOut->PutStr(TStr::Fmt("%f", GetVecParam(VecN)));
        for (int a = 0; a < AttrV->Len(); a++) {
            SOut->PutStr(TStr::Fmt(" %d:%f", AttrV->GetWId(a), AttrV->GetWgt(a).Val));
        }
        SOut->PutLn();
    }
    SOut->Flush();
}

double TBowTrainSet::DotProduct(const int& VecId1, double* vec2, const int& n) const {
    double res = 0.0;
    TBowWIdWgtKd* vec1 = TrainV[VecId1]->BegI();
    int len1 = TrainV[VecId1]->Len();
    for (int i = 0; i < len1; i++) {
        Assert(vec1[i].Key < n);
        res += vec2[vec1[i].Key] * vec1[i].Dat;
    }
    return res;
}

void TBowTrainSet::AddVec(const int& VecId1, double* vec2,
                          const int& n, const double& K) const {

    TBowWIdWgtKd* vec1 = TrainV[VecId1]->BegI();
    int len1 = TrainV[VecId1]->Len();
    for (int i = 0; i < len1; i++) {
        Assert(vec1[i].Key < n);
        vec2[vec1[i].Key] += K * vec1[i].Dat;
    }
}

void TBowTrainSet::AddVec(const int& VecId1, TFltV& vec2, const double& K) const {
    PBowSpV vec1 = TrainV[VecId1]; int len1 = vec1->Len();
    for (int i = 0; i < len1; i++) {
        Assert(vec1->GetWId(i) < vec2.Len());
        vec2[vec1->GetWId(i)] += K * vec1->GetWgt(i);
    }
}

int TBowTrainSet::AddAttrV(const int& DId, PBowSpV SpVec, const double& Cls) {
    DIdV.Add(DId); TrainV.Add(SpVec); ClsV.Add(Cls);
    if (SpVec->GetWIds() > 0) {
        int VecDim = SpVec->GetWId(SpVec->GetWIds()-1);
        MaxDim = MaxDim > VecDim ? MaxDim() : VecDim + 1;
    }
    return TrainV.Len()-1;
}

void TBowTrainSet::Shuffle(TRnd& Rnd) {
    const int Vals = TrainV.Len();
    for (int ValN=0; ValN<Vals-1; ValN++) {
        const int NewValN = ValN+Rnd.GetUniDevInt(Vals-ValN);
        DIdV.Swap(ValN, NewValN);
        TrainV.Swap(ValN, NewValN);
        ClsV.Swap(ValN, NewValN);
    }
}

//////////////////////////////////////////////////////////////////////////
// String-Kernel-Training-Set
bool TStringTrainSet::IsReg=TStringTrainSet::MkReg();

void TStringTrainSet::UpdateAlphN(const TIntV& Vec) {
    for (int i = 0, l = Vec.Len(); i < l; i++)
        AlphN = TInt::GetMx(AlphN, Vec[i]+1);
    StrKer->UpdateAlphN(AlphN);
}

TStringTrainSet::TStringTrainSet(const TIntV& VecIdV, const TFltV& _ClsV,
        const TFltV& _NormV, const TVec<TIntV>& _TrainV, PStrParser _Parser,
        const int& _AlphN, PStringKernel _StrKer): TSVMTrainSet(ststString) {

    if (VecIdV.Empty()) {
        ClsV = _ClsV; NormV = _NormV; TrainV = _TrainV;
        Parser = _Parser; AlphN = _AlphN; StrKer = _StrKer;
    } else {
        const int Vecs = VecIdV.Len();
        ClsV.Gen(Vecs, 0); NormV.Gen(Vecs, 0); TrainV.Gen(Vecs, 0);
        for (int VecN = 0; VecN < Vecs; VecN++) {
            const int VecId = VecIdV[VecN];
            ClsV.Add(_ClsV[VecId]);
            NormV.Add(_NormV[VecId]);
            TrainV.Add(_TrainV[VecId]);
        }
        Parser = _Parser; AlphN = _AlphN; StrKer = _StrKer;
    }
    printf("AlphN = [%d => %d]\n", _AlphN, AlphN.Val);
}

int TStringTrainSet::AddAttrV(const TStr& AttrV, const double& Cls) {
    TIntV ParsedAttrV; Parser->ProcessDocStr(AttrV, ParsedAttrV);
    return AddAttrV(ParsedAttrV, Cls);
}

int TStringTrainSet::AddAttrV(const TIntV& AttrV, const double& Cls) {
    IAssert(!AttrV.Empty());

    UpdateAlphN(AttrV);
    const double Norm = StrKer->CalcKernel(AttrV, AttrV);
    ClsV.Add(Cls); NormV.Add(Norm);
    TrainV.Add(AttrV); TrainV.Last().Pack();

    return TrainV.Len()-1;
}

double TStringTrainSet::DotProduct(const int& VecId1, const TStr& Vec2) const {
    TIntV ParsedVec2; Parser->ProcessDocStr(Vec2, ParsedVec2);
    return DotProduct(VecId1, ParsedVec2);
}

double TStringTrainSet::DotProduct(const int& VecId1, const TIntV& Vec2) const {
    const double Norm = StrKer->CalcKernel(Vec2, Vec2);
    return StrKer->CalcKernel(TrainV[VecId1],Vec2) / sqrt(NormV[VecId1]*Norm);
}

void TStringTrainSet::Shuffle(TRnd& Rnd) {
    const int Vals = TrainV.Len();
    for (int ValN=0; ValN<Vals-1; ValN++) {
        const int NewValN = ValN+Rnd.GetUniDevInt(Vals-ValN);
        TrainV.Swap(ValN, NewValN);
        ClsV.Swap(ValN, NewValN);
        NormV.Swap(ValN, NewValN);
    }
}

//////////////////////////////////////////////////////////////////////////
// Similarity-Matrix-Training-Set
TSimMatrixTrainSet::TSimMatrixTrainSet(const TIntV& VecIdV,
        const TFltV& _ClsV, const TFltVV& _SimMatrix): TSVMTrainSet(ststSimMatrix) {

    const int Vecs = VecIdV.Len();
    SimMatrix.Gen(Vecs, Vecs); ClsV.Gen(Vecs, 0);
    for (int i = 0; i < Vecs; i++) {
        const int VecId1 = VecIdV[i];
        ClsV.Add(_ClsV[VecId1]);
        for (int j = 0; j < Vecs; j++) {
            const int VecId2 = VecIdV[j];
            SimMatrix(i, j) = _SimMatrix(VecId1, VecId2);
        }
    }
}

TSimMatrixTrainSet::TSimMatrixTrainSet(const TFltVV& _SimMatrix,
        const TFltV& _ClsV): TSVMTrainSet(ststSimMatrix), SimMatrix(_SimMatrix) {

    if (_ClsV.Empty()) {
        ClsV.Gen(SimMatrix.GetXDim());
        ClsV.PutAll(0.0);
    } else {
        ClsV = _ClsV;
        Assert(ClsV.Len() == SimMatrix.GetXDim());
    }
}


//////////////////////////////////////////////////////////////////////////
// Bag-Of-Words-Base to SVM-Train-Set
void TBowDocBs2TrainSet::MakeSpVec(PBowSpV BowVec, TIntFltKdV& SpVec) {
    const int BowVecLen = BowVec->GetWIds(); SpVec.Gen(BowVecLen, 0);
    for (int WgtC = 0, WgtN = BowVecLen; WgtC < WgtN; WgtC++) {
        SpVec.Add(TIntFltKd(BowVec->GetWId(WgtC), BowVec->GetWgt(WgtC).Val));
    }
}

PSVMTrainSet TBowDocBs2TrainSet::NewBowNoCat(
        const TVec<PBowSpV>& BowSpV, const double& DefParam) {
    int DocN = BowSpV.Len();
    PSVMTrainSet docs = TBowTrainSet::New(DocN);
    for (int DocC = 0; DocC < DocN; DocC++) {
        docs->AddAttrV(DocC, BowSpV[DocC], DefParam);
    }
    return docs;
}

PSVMTrainSet TBowDocBs2TrainSet::NewBowNoCat(PBowDocWgtBs BowDocWgtBs,
        const TIntV& DIdV, const double& DefParam) {
    int DocN = DIdV.Len();
    PSVMTrainSet docs = TBowTrainSet::New(DocN);
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        PBowSpV SpV = BowDocWgtBs->GetSpV(DId);
        docs->AddAttrV(DId, SpV, DefParam);
    }
    return docs;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparseNoCat(
        const TVec<PBowSpV>& BowSpV, const double& DefParam) {
    int DocN = BowSpV.Len();
    PSVMTrainSet Set = TSparseTrainSet::New(DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        MakeSpVec(BowSpV[DocC], SpVec);
        Set->AddAttrV(SpVec, DefParam, false);
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparseNoCat(PBowDocWgtBs BowDocWgtBs,
        const TIntV& DIdV, const double& DefParam) {
    int DocN=DIdV.Len();
    PSVMTrainSet Set = TSparseTrainSet::New(DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        MakeSpVec(BowDocWgtBs->GetSpV(DId), SpVec);
        Set->AddAttrV(SpVec, DefParam, false);
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewBowOneCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV) {
    int DocN = DIdV.Len();
    PSVMTrainSet docs = TBowTrainSet::New(DocN);
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        if (BowDocBs->IsCatInDoc(DId, CatId)) {
            PBowSpV SpV = BowDocWgtBs->GetSpV(DId);
            docs->AddAttrV(DId, SpV, 1.0);
        }
    }
    return docs;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparseOneCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV) {
    int DocN=DIdV.Len();
    PSVMTrainSet Set = TSparseTrainSet::New(DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        if (BowDocBs->IsCatInDoc(DId, CatId)) {
            MakeSpVec(BowDocWgtBs->GetSpV(DId), SpVec);
            Set->AddAttrV(SpVec, 1.0, false);
        }
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewStringOneCat(const PBowDocBs& BowDocBs,
        const int& CatId, const TIntV& DIdV, PStrParser Parser, PStringKernel StrKer) {
    int DocN=DIdV.Len();
    PSVMTrainSet Set = TStringTrainSet::New(Parser, StrKer, DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        if (BowDocBs->IsCatInDoc(DId, CatId)) {
            TStr DocStr = BowDocBs->GetDocStr(DId);
            Set->AddAttrV(DocStr, 1.0);
        }
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewBowAllCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV,
        const double& SampleNegP) {

    int Docs = DIdV.Len();
    TIntV PosDIdV, NegDIdV;
    for (int DocN = 0; DocN < Docs; DocN++) {
        const int DId = DIdV[DocN];
        if(BowDocBs->IsCatInDoc(DId, CatId)) {
            PosDIdV.Add(DId);
        } else {
            NegDIdV.Add(DId);
        }
    }

    // should we sample negatives?
    const int SampleSize = TFlt::Round(PosDIdV.Len() * SampleNegP);
    if ((SampleSize > 0) && (NegDIdV.Len() > SampleSize)) {
        TRnd Rnd(1); NegDIdV.Shuffle(Rnd); NegDIdV.Trunc(SampleSize);
    }

    PSVMTrainSet docs = TBowTrainSet::New(NegDIdV.Len() + PosDIdV.Len());
    for (int PosDocN = 0; PosDocN < PosDIdV.Len(); PosDocN++) {
        const int DId = PosDIdV[PosDocN]; docs->AddAttrV(DId, BowDocWgtBs->GetSpV(DId), 1.0); }
    for (int NegDocN = 0; NegDocN < NegDIdV.Len(); NegDocN++) {
        const int DId = NegDIdV[NegDocN]; docs->AddAttrV(DId, BowDocWgtBs->GetSpV(DId), -1.0); }
    return docs;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparseAllCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& CatId, const TIntV& DIdV) {
    int DocN = DIdV.Len();
    PSVMTrainSet Set = TSparseTrainSet::New(DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        double param = BowDocBs->IsCatInDoc(DId, CatId) ? 1.0 : -1.0;
        MakeSpVec(BowDocWgtBs->GetSpV(DId), SpVec);
        Set->AddAttrV(SpVec, param, false);
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewStringAllCat(const PBowDocBs& BowDocBs,
        const int& CatId, const TIntV& DIdV, PStrParser Parser, PStringKernel StrKer) {
    int DocN = DIdV.Len();
    PSVMTrainSet Set = TStringTrainSet::New(Parser, StrKer, DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = DIdV[DocC];
        double DocParam = BowDocBs->IsCatInDoc(DId, CatId) ? 1.0 : -1.0;
        TStr DocStr = BowDocBs->GetDocStr(DId);
        Set->AddAttrV(DocStr, DocParam);
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewBowAllCatV(PBowDocWgtBs BowDocWgtBs,
        const TIntV& AllDIdV, const TIntV& CatDIdV) {
    int DocN = AllDIdV.Len();
    PSVMTrainSet docs = TBowTrainSet::New(DocN);
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = AllDIdV[DocC];
        double param = CatDIdV.SearchBin(DId) == -1 ? -1.0 : 1.0;
        PBowSpV SpV = BowDocWgtBs->GetSpV(DId);
        docs->AddAttrV(DId, SpV, param);
    }
    return docs;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparseAllCatV(PBowDocWgtBs BowDocWgtBs,
        const TIntV& AllDIdV, const TIntV& CatDIdV) {
    int DocN = AllDIdV.Len();
    PSVMTrainSet Set = TSparseTrainSet::New(DocN); TIntFltKdV SpVec;
    for (int DocC = 0; DocC < DocN; DocC++) {
        int DId = AllDIdV[DocC];
        double param = CatDIdV.SearchBin(DId) == -1 ? -1.0 : 1.0;
        MakeSpVec(BowDocWgtBs->GetSpV(DId), SpVec);
        Set->AddAttrV(SpVec, param, false);
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewBowFromCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& AllDIdV) {
    Fail; return NULL;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparseFromCat(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& AllDIdV) {
    Fail; return NULL;
}

PSVMTrainSet TBowDocBs2TrainSet::NewBow(PBowDocPart BowDocPart) {
    int Clusts = BowDocPart->GetClusts();
    PSVMTrainSet Set = TBowTrainSet::New(Clusts);
    for (int ClustN = 0; ClustN < Clusts; ClustN++) {
        IAssert(BowDocPart->GetClust(ClustN)->IsConceptSpV());
        Set->AddAttrV(ClustN, BowDocPart->GetClust(ClustN)->GetConceptSpV(), 1.0);
    }
    return Set;
}

PSVMTrainSet TBowDocBs2TrainSet::NewSparse(PBowDocPart BowDocPart) {
    int Clusts = BowDocPart->GetClusts();
    PSVMTrainSet Set = TSparseTrainSet::New(Clusts); TIntFltKdV SpVec;
    for (int ClustN = 0; ClustN < Clusts; ClustN++) {
        IAssert(BowDocPart->GetClust(ClustN)->IsConceptSpV());
        MakeSpVec(BowDocPart->GetClust(ClustN)->GetConceptSpV(), SpVec);
        Set->AddAttrV(SpVec, 1.0, false);
    }
    return Set;
}

//////////////////////////////////////////////////////////////////////////
// SVM-Model
PSVMModel TSVMModel::MakeModel(const bool& Linear,
        const PKernel& ker, const PSVMTrainSet& TrainSet,
        const TIntV& SubSet, const TSVMModelParam& ModelParam,
        const TSVMLearnParam& LearnParam) {

    PSVMModel Model = TSVMModel::New();
    Model->Linear = Linear;

    TIntV DIdV; int Len;
    if (SubSet.Len() != 0) {
        TSVMFactory::train(Model->AlphaV, Model->Thresh.Val, Linear, ker,
            TSVMTrainSubSet::New(TrainSet(), SubSet), ModelParam, LearnParam);
        DIdV = SubSet; Len = SubSet.Len();
    } else {
        TSVMFactory::train(Model->AlphaV, Model->Thresh.Val,
            Linear, ker, TrainSet(), ModelParam, LearnParam);

        Len = TrainSet->Len(); DIdV.Gen(Len);
        for (int i = 0; i < Len; i++) { DIdV[i] = i; }
    }

    if (ModelParam.ModelType == smtClassifier) {
        for (int i = 0; i < Len; i++) {
            Model->AlphaV[i] = TrainSet->GetVecParam(DIdV[i]) * Model->AlphaV[i];
        }
    } else if (ModelParam.ModelType == smtRegression) {
        // rearranges alphas for case of regression
        IAssert(Model->AlphaV.Len() == 2*Len);
        for (int i = 0; i < Len; i++) {
            Model->AlphaV[i] = Model->AlphaV[i+Len] - Model->AlphaV[i];
        }
        Model->AlphaV.Trunc(Len);
        // and invert the threshold
        Model->Thresh = -1.0 * Model->Thresh;
    }

    if (Linear) {
        TrainSet->LinComb(DIdV, Model->AlphaV, Model->WgtV);
    } else {
        if (TrainSet->Type() != ststSimMatrix) {
            TFltV NewAlphaV; TIntV VecIdV;
            for (int VecN = 0; VecN < Len; VecN++) {
                if (TFlt::Abs(Model->AlphaV[VecN]) > EPSILON) {
                    NewAlphaV.Add(Model->AlphaV[VecN]);
                    VecIdV.Add(DIdV[VecN]);
                }
            }
            Model->AlphaV = NewAlphaV; // support vectors alphas
            Model->SupVecs = TrainSet->Clone(VecIdV);
        } else {
            TFltV NewAlphaV(TrainSet->Len()); NewAlphaV.PutAll(0.0);
            for (int VecN = 0; VecN < Len; VecN++) {
                NewAlphaV[DIdV[VecN]] = Model->AlphaV[VecN];
            }
            Model->AlphaV = NewAlphaV; // alphas for all vectors in the trainset
            Model->SupVecs = TrainSet;
        }
        Model->Kernel = ker;
    }

    Model->AlphaV.Pack();
    return Model;
}

TCfyRes TSVMModel::CrossValidation(const int& Folds, const int&Seed, const bool& Linear,
        const PKernel& ker, const PSVMTrainSet& TrainSet, const TIntV& SubSet,
        const TSVMModelParam& ModelParam, const TSVMLearnParam& LearnParam) {

    // first we prepare vectors if ids for each fold
    TIntV DIdV; TCrossValid::PrepareSubDIdV(SubSet, TrainSet->Len(), DIdV);
    TVec<TIntV> DIdVFoldV; TCrossValid::PrepareFolds(Folds, DIdV, Seed, DIdVFoldV);
    // Folds-Fold Cross Validation
    TIntV TrainSubDIdV, TestSubDIdV; TCfyRes CfyRes;
    double MarginSum = 0.0;
    for (int FoldN = 0; FoldN < Folds; FoldN++) {
        printf("%d.. ", FoldN);
        // prepare train/test split for curent fold
        TCrossValid::PrepareSplit(FoldN, DIdVFoldV, TrainSubDIdV, TestSubDIdV);
        //TIntV InterV; TrainSubDIdV.Intrs(TestSubDIdV, InterV);
        // remove all negatives from train if one-class SVM
        if (ModelParam.ModelType == smtOneClass) {
            TIntV PosTrainSubDIdV;
            for (int DIdN = 0; DIdN < TrainSubDIdV.Len(); DIdN++) {
                const int DId = TrainSubDIdV[DIdN];
                if (TrainSet->GetVecParam(DId) > 0.0) {
                    PosTrainSubDIdV.Add(DId);
                }
            }
            TrainSubDIdV = PosTrainSubDIdV;
        }
        // learn model
        PSVMModel FoldModel = MakeModel(Linear, ker,
            TrainSet, TrainSubDIdV, ModelParam, LearnParam);
        // validate model
        TCfyRes FoldCfyRes = FoldModel->Test(TrainSet, TestSubDIdV);
        MarginSum += FoldModel->GetMargin();
        CfyRes.Add(FoldCfyRes);
    } printf("\n");
    // finalize the results
    CfyRes.Def();
    CfyRes.AddStat("AvgMargin", MarginSum / Folds);
    return CfyRes;
}

TSVMModel::TSVMModel(TSIn& SIn) {
    AlphaV.Load(SIn);
    Thresh.Load(SIn);
    Linear.Load(SIn);
    if (Linear) {
        WgtV.Load(SIn);
    } else {
        SupVecs = TSVMTrainSet::Load(SIn);
        Kernel = TKernel::Load(SIn);
    }
}

void TSVMModel::Save(TSOut& SOut) {
    AlphaV.Save(SOut);
    Thresh.Save(SOut);
    Linear.Save(SOut);
    if (Linear) {
        WgtV.Save(SOut);
    } else {
        SupVecs->Save(SOut);
        Kernel->Save(SOut);
    }
}

double TSVMModel::GetRes(const TIntFltKdV& Vec) const {
    double Result = Thresh;
    if (Linear) {
        Result += TLinAlg::DotProduct(WgtV, Vec);
    } else {
        IAssert(!SupVecs.Empty() && ! Kernel.Empty());
        IAssert(SupVecs->Len() == AlphaV.Len());

        const double Norm2 = TLinAlg::Norm2(Vec);
        for (int i = 0; i < AlphaV.Len(); i++) {
            Result += AlphaV[i] * Kernel->Calc(
                SupVecs->DotProduct(i, Vec),
                Norm2, SupVecs->GetNorm2(i));
        }
    }
    return Result;
}

double TSVMModel::GetRes(const TFltV& Vec) const {
    double Result = Thresh;
    if (Linear) {
        Result += TLinAlg::DotProduct(WgtV, Vec);
    } else {
        IAssert(!SupVecs.Empty() && ! Kernel.Empty());
        IAssert(SupVecs->Len() == AlphaV.Len());

        const double Norm2 = TLinAlg::Norm2(Vec);
        for (int i = 0; i < AlphaV.Len(); i++) {
            Result += AlphaV[i] * Kernel->Calc(
                SupVecs->DotProduct(i, Vec),
                Norm2, SupVecs->GetNorm2(i));
        }
    }
    return Result;
}

double TSVMModel::GetRes(const TStr& Vec) const {
    IAssert(!Linear); // can not be linear if Vec is strings
    IAssert(!SupVecs.Empty() && ! Kernel.Empty());
    IAssert(SupVecs->Len() == AlphaV.Len());
    IAssert(SupVecs->Type() == ststString);

    double Result = Thresh;
    const double Norm2 = 1.0; // always 1.0 at string kernels!
    for (int i = 0; i < AlphaV.Len(); i++) {
        Result += AlphaV[i] * Kernel->Calc(
            SupVecs->DotProduct(i, Vec),
            Norm2, SupVecs->GetNorm2(i));
    }
    return Result;
}

double TSVMModel::GetRes(const TIntV& Vec) const {
    IAssert(!Linear); // can not be linear if Vec is strings
    IAssert(!SupVecs.Empty() && ! Kernel.Empty());
    IAssert(SupVecs->Len() == AlphaV.Len());
    IAssert(SupVecs->Type() == ststString);

    double Result = Thresh;
    const double Norm2 = 1.0; // always 1.0 at string kernels!
    for (int i = 0; i < AlphaV.Len(); i++) {
        Result += AlphaV[i] * Kernel->Calc(
            SupVecs->DotProduct(i, Vec),
            Norm2, SupVecs->GetNorm2(i));
    }
    return Result;
}

double TSVMModel::GetRes(PBowSpV Vec) const {
    double Result = Thresh;
    if (Linear) {
        Result += TBowLinAlg::DotProduct(WgtV, Vec);
    } else {
        IAssert(!SupVecs.Empty() && ! Kernel.Empty());
        IAssert(SupVecs->Len() == AlphaV.Len());
        IAssert(SupVecs->Type() == ststBow);

        const double Norm2 = TMath::Sqr(Vec->GetNorm());
        for (int i = 0; i < AlphaV.Len(); i++) {
            Result += AlphaV[i] * Kernel->Calc(
                SupVecs->DotProduct(i, Vec),
                Norm2, SupVecs->GetNorm2(i));
        }
    }
    return Result;
}

double TSVMModel::GetRes(const int& VecId) const {
    IAssert(!Linear); // can not be linear if Vec is strings
    IAssert(!SupVecs.Empty() && ! Kernel.Empty());
    IAssert(SupVecs->Len() == AlphaV.Len());
    IAssert(0 <= VecId && VecId < AlphaV.Len());
    IAssert(SupVecs->Type() == ststSimMatrix);

    double Result = Thresh;
    const double Norm2 = SupVecs->GetNorm2(VecId);
    for (int i = 0; i < AlphaV.Len(); i++) {
        Result += AlphaV[i] * Kernel->Calc(
            SupVecs->DotProduct(i, VecId),
            Norm2, SupVecs->GetNorm2(i));
    }
    return Result;
}

TCfyRes TSVMModel::Test(const PSVMTrainSet& TestSet, const TIntV& SubSet) const {
    TIntV DIdV; TCrossValid::PrepareSubDIdV(SubSet, TestSet->Len(), DIdV);
    int Len = DIdV.Len(); TCountCfyRes Result;
    for (int DocC = 0; DocC < Len; DocC++) {
        if (TestSet->Type() == ststSparse) {
            Result.Add(GetRes(TestSet->GetAttrSparseV(DIdV[DocC])),
                TestSet->GetVecParam(DIdV[DocC]));
        } else if (TestSet->Type() == ststDense) {
            Result.Add(GetRes(TestSet->GetAttrDenseV(DIdV[DocC])),
                TestSet->GetVecParam(DIdV[DocC]));
        } else if (TestSet->Type() == ststBow) {
            Result.Add(GetRes(TestSet->GetAttrBowV(DIdV[DocC])),
                TestSet->GetVecParam(DIdV[DocC]));
        } else if (TestSet->Type() == ststString) {
            Result.Add(GetRes(TestSet->GetAttrStringV(DIdV[DocC])),
                TestSet->GetVecParam(DIdV[DocC]));
        } else if (TestSet->Type() == ststSimMatrix) {
            Result.Add(GetRes(DIdV[DocC]), TestSet->GetVecParam(DIdV[DocC]));
        }
    }
    return Result.ToTCfyRes();
}

void TSVMModel::MakeProb(const PSVMTrainSet& TrainSet, const TIntV& SubSet) {
    TIntV DIdV; TCrossValid::PrepareSubDIdV(SubSet, TrainSet->Len(), DIdV);
    int Docs = DIdV.Len();
    TFltIntKdV DistParamV(Docs, 0);
    for (int DocC = 0; DocC < Docs; DocC++) {
        double DistToHyperplane = 0.0;
        if (TrainSet->Type() == ststSparse) {
            DistToHyperplane = GetRes(TrainSet->GetAttrSparseV(DIdV[DocC]));
        } else if (TrainSet->Type() == ststDense) {
            DistToHyperplane = GetRes(TrainSet->GetAttrDenseV(DIdV[DocC]));
        } else if (TrainSet->Type() == ststBow) {
            DistToHyperplane = GetRes(TrainSet->GetAttrBowV(DIdV[DocC]));
        } else if (TrainSet->Type() == ststString) {
            DistToHyperplane = GetRes(TrainSet->GetAttrStringV(DIdV[DocC]));
        } else if (TrainSet->Type() == ststSimMatrix) {
            DistToHyperplane = GetRes(DIdV[DocC]);
        }
        DistParamV.Add(TFltIntKd(DistToHyperplane,
            TFlt::Round(TrainSet->GetVecParam(DIdV[DocC]))));
    }
    Sigmoid = TSigmoid(DistParamV);
    IsSigDef = true;
}

double TSVMModel::GetMargin() const {
    if (IsLinear()) {
        const double WgtNorm = TLinAlg::Norm(WgtV);
        if (WgtNorm > 0.0) { return 1.0 / WgtNorm; }
        else { return 0.0; }
    } else {
        double WgtNorm2 = 0.0;
        for (int i = 0; i < AlphaV.Len(); i++) {
            for (int j = 0; j < AlphaV.Len(); j++) {
                WgtNorm2 += AlphaV[i]*AlphaV[j] * Kernel->CalcSet(SupVecs, i, j);
            }
        }

        if (WgtNorm2 > 0.0) { return 1.0 / sqrt(WgtNorm2); }
        else { return 0.0; }
    }
}

//////////////////////////////////////////////////////////////////////////
// BagOfWords-SVM-Model
bool TBowSVMMd::IsReg=TBowSVMMd::MkReg();

TBowSVMMd::TBowSVMMd(TSIn& SIn): TBowMd(SIn) {
    CatNm.Load(SIn);
    Model = TSVMModel::Load(SIn);
}

void TBowSVMMd::Save(TSOut &SOut) {
    TBowMd::Save(SOut);
    CatNm.Save(SOut);
    Model->Save(SOut);
}

void TBowSVMMd::GetLinComb(
 const PBowDocBs& BowDocBs, TFltStrPrV& WgtStrPrV, double& Tsh) const {
  IAssert(IsLinComb());
  //WgtStrPrV;
  TFltV WgtV; Model->GetWgtV(WgtV);
  for (int WordId=0; WordId<WgtV.Len(); WordId++){
    if (WgtV[WordId]!=0.0){
      if (BowDocBs.Empty()){
        WgtStrPrV.Add(TFltStrPr(WgtV[WordId], TInt::GetStr(WordId)));
      } else {
        TStr Str=BowDocBs->GetWordStr(WordId);
        WgtStrPrV.Add(TFltStrPr(WgtV[WordId], Str));
      }
    }
  }
  Tsh=Model->GetThresh();
}

PSVMTrainSet TBowSVMMd::PrepareClsTrainSet(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& SampleNegP) {

    IAssertR(BowDocBs->IsCatNm(CatNm), CatNm);
    const int CId = BowDocBs->GetCId(CatNm);
    return TBowDocBs2TrainSet::NewBowAllCat(BowDocBs,
        BowDocWgtBs, CId, TrainDIdV, SampleNegP);
}

PSVMTrainSet TBowSVMMd::PrepareOneClassTrainSet(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV) {

    IAssertR(BowDocBs->IsCatNm(CatNm), CatNm);
    const int CId = BowDocBs->GetCId(CatNm);
    return TBowDocBs2TrainSet::NewBowOneCat(BowDocBs, BowDocWgtBs, CId, TrainDIdV);
}

inline PSVMTrainSet TBowSVMMd::PrepareClsTrainSet(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& AllDIdV, const TIntV& CatDIdV) {

    return TBowDocBs2TrainSet::NewSparseAllCatV(BowDocWgtBs, AllDIdV, CatDIdV);
}


PBowSpV TBowSVMMd::GetKeywords(const PSVMTrainSet& TrainSet,
        const PSVMModel& SVMModel, const int WdN,
        const double& VecSign, const double& WgtSign, const bool& AvgOverSet) {

    // calculate weigths for all words
    TFltV NormalV; SVMModel->GetWgtV(NormalV);
    TIntFltKdV WIdWgtKdV;
    TrainSet->GetKeywords(NormalV, WIdWgtKdV,
        TIntV(), WdN, VecSign, WgtSign, AvgOverSet);
    // make sparse vector with results
    PBowSpV BowSpV = TBowSpV::New(-1, WIdWgtKdV.Len());
    for (int WdN = 0; WdN < WIdWgtKdV.Len(); WdN++) {
        BowSpV->AddWIdWgt(WIdWgtKdV[WdN].Key, WIdWgtKdV[WdN].Dat);
    }
    return BowSpV;
}

inline void TBowSVMMd::GetCfy(const PBowSpV& QueryBowSpV, TFltStrPrV& WgtCatNmPrV) {
    WgtCatNmPrV = TFltStrPrV::GetV(TFltStrPr(Model->IsProb() ?
        Model->GetProbCfy(QueryBowSpV) : Model->GetRes(QueryBowSpV), CatNm));
}

TCfyRes TBowSVMMd::Test(PBowMd BowMd, PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TIntV& DIdV, const TStr& CatNm) {

    TCountCfyRes Result; TFltStrPrV ResV;
    IAssertR(BowDocBs->IsCatNm(CatNm), CatNm);
    const int CId = BowDocBs->GetCId(CatNm);
    for (int VecC = 0, VecN = DIdV.Len(); VecC < VecN; VecC++) {
        PBowSpV DocSpV = BowDocWgtBs->GetSpV(DIdV[VecC]);
        ResV.Clr(); BowMd->GetCfy(DocSpV, ResV);
        double PredCls = ResV.Len() > 0 ? ResV[0].Val1() : -1.0;
        double TrueCls = BowDocBs->IsCatInDoc(DIdV[VecC], CId) ? 1.0 : -1.0;
        Result.Add(PredCls, TrueCls);
    }
    return Result.ToTCfyRes();
}

void TBowSVMMd::SaveTxt(const TStr& FNm, const PBowDocBs& BowDocBs,
        const PBowDocWgtBs& BowDocWgtBs) {

    PSOut SOut = TFOut::New(FNm);
    TIntV DIdV; BowDocBs->GetAllDIdV(DIdV);
    TCfyRes Res = Test(this, BowDocBs, BowDocWgtBs, DIdV, this->GetCatNm());
    Res.PrStat("Results on the training data", SOut);

    SOut->PutLn();
}

PBowMd TBowSVMMd::NewClsLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& C, const double& j,
        const bool& ProbabilisticP, const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    PSVMModel Model = TSVMModel::NewClsLinear(TrainSet, C, j, TIntV(), LearnParam);
    if (ProbabilisticP) Model->MakeProb(TrainSet);
    return TBowSVMMd::New(BowDocBs, CatNm, Model);
}

PBowMd TBowSVMMd::NewClsLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& TrainDIdV, const TIntV& CatDIdV, const double& C, const double& j,
        const bool& ProbabilisticP, const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, TrainDIdV, CatDIdV);
    PSVMModel Model = TSVMModel::NewClsLinear(TrainSet, C, j, TIntV(), LearnParam);
    if (ProbabilisticP) Model->MakeProb(TrainSet);
    return TBowSVMMd::New(BowDocBs, "unknown", Model);
}

PBowMd TBowSVMMd::NewCls(const PKernel& ker, const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& C, const double& j, const bool& ProbabilisticP,
        const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    PSVMModel Model = TSVMModel::NewCls(TrainSet, ker, C, j, TIntV(), LearnParam);
    if (ProbabilisticP) Model->MakeProb(TrainSet);
    return TBowSVMMd::New(BowDocBs, CatNm, Model);
}

TCfyRes TBowSVMMd::CrossValidClsLinear(const int& Folds, const int&Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs, const TStr& CatNm,
        const TIntV& TrainDIdV, const double& C, const double& j,
        const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    return TSVMModel::CrossValidClsLinear(Folds, Seed, TrainSet, C, j, TIntV(), LearnParam);
}

TCfyRes TBowSVMMd::CrossValidClsLinear(const int& Folds, const int&Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs, const TIntV& TrainDIdV,
        const TIntV& CatDIdV, const double& C, const double& j,
        const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, TrainDIdV, CatDIdV);
    return TSVMModel::CrossValidClsLinear(Folds, Seed, TrainSet, C, j, TIntV(), LearnParam);
}

TCfyRes TBowSVMMd::CrossValidCls(const int& Folds, const int&Seed,
        const PKernel& ker, const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& C,
        const double& j, const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    return TSVMModel::CrossValidCls(Folds, Seed, TrainSet, ker, C, j, TIntV(), LearnParam);
}

PBowMd TBowSVMMd::NewMultiClsLinear(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& TrainDIdV, const double& C, const double& j,
        const bool& SampleNegP, const TSVMLearnParam& LearnParam) {

    // train a model for each category
    const int Cats = BowDocBs->GetCats(); TBowMdV BowMdV(Cats, 0);
    for (int CatN = 0; CatN < Cats; CatN++) {
        const TStr& CatNm = BowDocBs->GetCatNm(CatN);
        PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs,
            BowDocWgtBs, CatNm, TrainDIdV, (SampleNegP ? j : -1.0));
        printf("(%4d/ %4d (Docs: %9d\r) ", CatN+1, Cats, TrainSet->Len());
        PSVMModel Model = TSVMModel::NewClsLinear(
            TrainSet, C, j, TIntV(), LearnParam);
        Model->MakeProb(TrainSet);
        BowMdV.Add(TBowSVMMd::New(BowDocBs, CatNm, Model));
    } printf("\n");
    return new TBowMultiMd(BowDocBs, BowMdV);
}

TCfyRes TBowSVMMd::CrossValidMultiClsLinear(const int& Folds, const int& Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs, const double& C,
        const double& j, const bool& SampleNegP, const TSVMLearnParam& LearnParam) {

    // prepare folds
    TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV); AllDIdV.Sort();
    TIntPrV CIdDIdV; const int DIds = AllDIdV.Len();
    for (int DIdN = 0; DIdN < DIds; DIdN++) {
        const int DId = AllDIdV[DIdN];
        const int CIds = BowDocBs->GetDocCIds(DId);
        for (int CIdN = 0; CIdN < CIds; CIdN++) {
            const int CId = BowDocBs->GetDocCId(DId, CIdN);
            CIdDIdV.Add(TIntPr(CId, DId));
        }
    }
    CIdDIdV.Sort();
    TVec<TIntV> FoldV(Folds); int FoldN = 0;
    for (int CIdDIdN = 0; CIdDIdN < CIdDIdV.Len(); CIdDIdN++) {
        // add document to the fold
        const int DId = CIdDIdV[CIdDIdN].Val2;
        FoldV[FoldN].Add(DId);
        // go to next fold
        FoldN++;
        if (FoldN >= Folds) { FoldN = 0; }
    }
    for (int FoldN = 0; FoldN < Folds; FoldN++) { FoldV[FoldN].Sort(); }
    // evaluate on folds
    const int Cats = BowDocBs->GetCats(); TBowMdV BowMdV(Cats, 0);
    TVec<TCountCfyRes> CatCfyResV(Cats);
    for (int FoldN = 0; FoldN < Folds; FoldN++) {
        printf("Fold %d ...\n", FoldN+1);
        // prepare fold split
        TIntV TestDIdV = FoldV[FoldN];
        TIntV TrainDIdV; AllDIdV.Minus(TestDIdV, TrainDIdV);
        // train
        PBowMd FoldBowMd = TBowSVMMd::NewMultiClsLinear(BowDocBs,
            BowDocWgtBs, TrainDIdV, C, j, SampleNegP, LearnParam);
        // test
        for (int TestDIdN = 0; TestDIdN < TestDIdV.Len(); TestDIdN++) {
            const int TestDId = TestDIdV[TestDIdN];
            TFltStrPrV WgtCatNmV;
            FoldBowMd->GetCfy(BowDocWgtBs->GetSpV(TestDId), WgtCatNmV);
            // get predicted classes
            TIntH PredCIdH;
            if (!WgtCatNmV.Empty()) {
                PredCIdH.AddKey(BowDocBs->GetCId(WgtCatNmV[0].Val2)); }
            // match them against correct classes
            const int CIds = BowDocBs->GetDocCIds(TestDId);
            for (int CIdN = 0; CIdN < CIds; CIdN++) {
                const int CId = BowDocBs->GetDocCId(TestDId, CIdN);
                if (PredCIdH.IsKey(CId)) {
                    CatCfyResV[CId].Add(1.0, 1.0);
                    PredCIdH.DelKey(CId);
                } else {
                    CatCfyResV[CId].Add(-1.0, 1.0);
                }
            }
            // are there any false positives left?
            if (!PredCIdH.Empty()) {
                int KeyId = PredCIdH.FFirstKeyId();
                while (PredCIdH.FNextKeyId(KeyId)) {
                    const int CId = PredCIdH.GetKey(KeyId);
                    CatCfyResV[CId].Add(1.0, -1.0);
                }
            }
        }
    }
    // get averages
    return TCountCfyRes::ToMacroTCfyRes(CatCfyResV);
}

PBowMd TBowSVMMd::NewOneClassLinear(const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& nu, const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareOneClassTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    PSVMModel Model = TSVMModel::NewOneClassLinear(TrainSet, nu, TIntV(), LearnParam);
    return TBowSVMMd::New(BowDocBs, CatNm, Model);
}

PBowMd TBowSVMMd::NewOneClass(const PKernel& ker, const PBowDocBs& BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& TrainDIdV,
        const double& nu, const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareOneClassTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    PSVMModel Model = TSVMModel::NewOneClass(TrainSet, ker, nu, TIntV(), LearnParam);
    return TBowSVMMd::New(BowDocBs, CatNm, Model);
}

TCfyRes TBowSVMMd::CrossValidOneClassLinear(const int& Folds, const int&Seed,
        const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs, const TStr& CatNm,
        const TIntV& TrainDIdV, const double& nu, const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    return TSVMModel::CrossValidOneClassLinear(Folds, Seed, TrainSet, nu, TIntV(), LearnParam);
}

TCfyRes TBowSVMMd::CrossValidOneClass(const int& Folds, const int&Seed,
        const PKernel& ker, const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm, const TIntV& TrainDIdV, const double& nu,
        const TSVMLearnParam& LearnParam) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    return TSVMModel::CrossValidOneClass(Folds, Seed, TrainSet, ker, nu, TIntV(), LearnParam);
}

PBowSpV TBowSVMMd::GetKeywords(const PBowDocBs& BowDocBs, const TIntV& TrainDIdV,
        const TIntV& CatDIdV, const int WdN, const double& C, const double& j,
        const int& Time, const TBowWordWgtType& SvmWordWgtType, const double& VecSign,
        const double& WgtSign, const bool& AvgOverSet) {

    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, SvmWordWgtType);
    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, TrainDIdV, CatDIdV);
    PSVMModel Model = TSVMModel::NewClsLinear(TrainSet, C, j, TIntV(), TSVMLearnParam::Lin(Time));
    return GetKeywords(TrainSet, Model, WdN, VecSign, WgtSign, AvgOverSet);
}

PBowSpV TBowSVMMd::GetKeywords(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& TrainDIdV, const TIntV& CatDIdV, const int WdN, const double& C,
        const double& j, const int& Time, const double& VecSign, const double& WgtSign,
        const bool& AvgOverSet) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, TrainDIdV, CatDIdV);
    PSVMModel Model = TSVMModel::NewClsLinear(TrainSet, C, j, TIntV(), TSVMLearnParam::Lin(Time));
    return GetKeywords(TrainSet, Model, WdN, VecSign, WgtSign, AvgOverSet);
}

PBowSpV TBowSVMMd::GetKeywords(const PBowDocBs& BowDocBs, const TIntV& TrainDIdV,
        const TStr& CatNm, const int WdN, const double& C, const double& j,
        const int& Time, const TBowWordWgtType& SvmWordWgtType, const double& VecSign,
        const double& WgtSign, const bool& AvgOverSet) {

    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, SvmWordWgtType);
    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    PSVMModel Model = TSVMModel::NewClsLinear(TrainSet, C, j, TIntV(), TSVMLearnParam::Lin(Time));
    return GetKeywords(TrainSet, Model, WdN, VecSign, WgtSign, AvgOverSet);
}

PBowSpV TBowSVMMd::GetKeywords(const PBowDocBs& BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TIntV& TrainDIdV, const TStr& CatNm, const int WdN, const double& C,
        const double& j, const int& Time, const double& VecSign, const double& WgtSign,
        const bool& AvgOverSet) {

    PSVMTrainSet TrainSet = PrepareClsTrainSet(BowDocBs, BowDocWgtBs, CatNm, TrainDIdV);
    PSVMModel Model = TSVMModel::NewClsLinear(TrainSet, C, j, TIntV(), TSVMLearnParam::Lin(Time));
    return GetKeywords(TrainSet, Model, WdN, VecSign, WgtSign, AvgOverSet);
}

//////////////////////////////////////////////////////////////////////////
// Csv-Feature-File-Loader

void TSVMTrainSetCsv::TFtrGen::AddWds(const TStr& Prefix,
        const PBowDocBs& BowDocBs, int& Offset) const {

    const int Vals = GetVals();
    for (int ValN = 0; ValN < Vals; ValN++) {
        const int WId = BowDocBs->AddWordStr(
            TStr::Fmt("%s-%s", Prefix.CStr(), GetVal(ValN).CStr()));
        IAssert(Offset == WId); Offset++;
    }
}

///////////////////////////////////////
// Numeric-Feature-Generator
TStr TSVMTrainSetCsv::TFtrGenNumeric::Type = "NUM";

double TSVMTrainSetCsv::TFtrGenNumeric::Trans(const double& Val) const {
        return MnVal != MxVal ? (double(Val) - MnVal) / (MxVal - MnVal) : 0.0;
}

double TSVMTrainSetCsv::TFtrGenNumeric::GetFlt(const TStr& Str) const {
    double Val = 0.0;
    if (!Str.IsFlt(Val)) {
        TExcept::Throw(TStr::Fmt("Number expected  but '%s' found!", Str.CStr())); }
    return Val;
}

void TSVMTrainSetCsv::TFtrGenNumeric::Update(const TStr& Val) {
    double Flt = GetFlt(Val);
    MnVal = TFlt::GetMn(MnVal, Flt);
    MxVal = TFlt::GetMx(MxVal, Flt);
}

void TSVMTrainSetCsv::TFtrGenNumeric::Add(
        const TStr& Val, TIntFltKdV& SpV, int& Offset) const {

    double Flt = GetFlt(Val);
    SpV.Add(TIntFltKd(Offset, Trans(Flt)));
    Offset++;
}

///////////////////////////////////////
// Nominal-Feature-Generator
TStr TSVMTrainSetCsv::TFtrGenNominal::Type = "NOM";

void TSVMTrainSetCsv::TFtrGenNominal::Update(const TStr& Val) {
    if (!Val.Empty()) { ValH.AddKey(Val); }
}

void TSVMTrainSetCsv::TFtrGenNominal::Add(
        const TStr& Val, TIntFltKdV& SpV, int& Offset) const {

    if (ValH.IsKey(Val)) {
        SpV.Add(TIntFltKd(Offset + ValH.GetKeyId(Val), 1.0));
    }
    Offset += ValH.Len();
}

///////////////////////////////////////
// Tokenizable-Feature-Generator
TStr TSVMTrainSetCsv::TFtrGenToken::Type = "TOK";

void TSVMTrainSetCsv::TFtrGenToken::GetTokenV(
        const TStr& Str, TStrV& TokenStrV) const {

    THtmlLx HtmlLx(TStrIn::New(Str));
    while (HtmlLx.Sym != hsyEof){
        if (HtmlLx.Sym == hsyStr){
            TStr TokenStr = HtmlLx.UcChA;
            if (SwSet.Empty() || !SwSet->IsIn(TokenStr)) {
                if (!Stemmer.Empty()) {
                    TokenStr = Stemmer->GetStem(TokenStr); }
                TokenStrV.Add(TokenStr);
            }
        }
        // get next symbol
        HtmlLx.GetSym();
    }
}

void TSVMTrainSetCsv::TFtrGenToken::Update(const TStr& Val) {
    TStrV TokenStrV; GetTokenV(Val, TokenStrV);
    for (int TokenStrN = 0; TokenStrN < TokenStrV.Len(); TokenStrN++) {
        const TStr& TokenStr = TokenStrV[TokenStrN];
        TokenH.AddDat(TokenStr)++;
    }
    Docs++;
}

void TSVMTrainSetCsv::TFtrGenToken::Add(
        const TStr& Val, TIntFltKdV& SpV, int& Offset) const {

    // step (1): tokenize
    TStrV TokenStrV; GetTokenV(Val, TokenStrV);
    // step (2): aggregate token counts
    TIntH TokenFqH;
    for (int TokenStrN = 0; TokenStrN < TokenStrV.Len(); TokenStrN++) {
        const TStr& TokenStr = TokenStrV[TokenStrN];
        if (TokenH.IsKey(TokenStr)) {
            const int TokenId = TokenH.GetKeyId(TokenStr);
            TokenFqH.AddDat(TokenId)++;
        }
    }
    // step (3): make a sparse vector out of it
    TIntFltKdV ValSpV(TokenFqH.Len(), 0);
    int KeyId = TokenFqH.FFirstKeyId();
    while (TokenFqH.FNextKeyId(KeyId)) {
        const int TokenId = TokenFqH.GetKey(KeyId);
        const int TokenFq = TokenFqH[KeyId];
        const int TokenDocFq = TokenH[TokenId];
        const double IDF = log(double(Docs) / double(TokenDocFq));
        ValSpV.Add(TIntFltKd(TokenId, double(TokenFq) * IDF));
    }
    ValSpV.Sort(); TLinAlg::NormalizeL1(ValSpV);
    // step (4): add the sparse vector to the final feature vector
    for (int ValSpN = 0; ValSpN < ValSpV.Len(); ValSpN++) {
        const int Key = ValSpV[ValSpN].Key + Offset;
        const double Dat = ValSpV[ValSpN].Dat;
        SpV.Add(TIntFltKd(Key, Dat));
    }
    Offset += TokenH.Len();
}

///////////////////////////////////////
// Sparse-Numeric-Feature-Generator
TStr TSVMTrainSetCsv::TFtrGenSparseNumeric::Type = "SP-NUM";

void TSVMTrainSetCsv::TFtrGenSparseNumeric::Split(
        const TStr& Str, int& Id, TStr& Val) const {

    if (!Str.IsChIn(':')) { TExcept::Throw("Wrong sparse numeric '" + Str + "'!"); }
    //split
    TStr IdStr; Str.SplitOnCh(IdStr, ':', Val);
    // parse id
    if (!IdStr.IsInt(Id)) { TExcept::Throw("Wrong sparse numeric '" + Str + "'!"); }
}

void TSVMTrainSetCsv::TFtrGenSparseNumeric::Update(const TStr& Str) {
    TStrV EltV; Str.SplitOnAllCh(';', EltV);
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        int Id; TStr Val; Split(EltV[EltN], Id, Val);
        MxId = TInt::GetMx(Id, MxId);
        FtrGen.Update(Val);
    }
}

void TSVMTrainSetCsv::TFtrGenSparseNumeric::Add(
        const TStr& Str, TIntFltKdV& SpV, int& Offset) const {

    TStrV EltV; Str.SplitOnAllCh(';', EltV); TIntH UsedIdH;
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        int Id; TStr Val; Split(EltV[EltN], Id, Val);
        EAssertR(!UsedIdH.IsKey(Id), "Field ID repeated in '" + Str + "'!");
        int TmpOffset = Offset + Id;
        FtrGen.Add(Val, SpV, TmpOffset);
        UsedIdH.AddKey(Id);
    }
    Offset += GetVals();
}

TStr TSVMTrainSetCsv::TFtrGenSparseNumeric::GetVal(const int& ValN) const {
    const int FtrGenId = ValN / FtrGen.GetVals();
    const int FtrGenValN = ValN % FtrGen.GetVals();
    return TStr::Fmt("SP-%d-%d-%s", FtrGenId,
        FtrGenValN, FtrGen.GetVal(FtrGenValN).CStr());
}

///////////////////////////////////////
// Multi-Feature-Generator
TStr TSVMTrainSetCsv::TFtrGenMultiNom::Type = "MULTI-NOM";

void TSVMTrainSetCsv::TFtrGenMultiNom::Update(const TStr& Str) {
    TStrV EltV; Str.SplitOnAllCh(';', EltV);
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        FtrGen.Update(EltV[EltN]);
    }
}

void TSVMTrainSetCsv::TFtrGenMultiNom::Add(
        const TStr& Str, TIntFltKdV& SpV, int& Offset) const {

    TStrV EltV; Str.SplitOnAllCh(';', EltV);
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        int TmpOffset = Offset;
        FtrGen.Add(EltV[EltN], SpV, TmpOffset);
    }
    Offset += GetVals();
}

TStr TSVMTrainSetCsv::TFtrGenMultiNom::GetVal(const int& ValN) const {
    return TStr::Fmt("MULTI-%d-%s", ValN, FtrGen.GetVal(ValN).CStr());
}

void TSVMTrainSetCsv::TFtrGenMultiNom::GetValV(const TStr& Str, TStrV& ValV) const {
    TStrV EltV; Str.SplitOnAllCh(';', EltV); ValV.Clr();
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        const TStr& Val = EltV[EltN];
        TStrV SubValV; FtrGen.GetValV(Val, SubValV);
        ValV.AddV(SubValV);
    }
}

///////////////////////////////////////
// Feature-Loader
PBowDocBs TSVMTrainSetCsv::LoadCsv(TStr& FNm, const int& ClassId,
        const TIntV& IgnoreIdV, const int& TrainLen) {

    // feature generators
    THash<TInt, PFtrGen> FtrGenH;
    PFtrGen ClassFtrGen;
    // CSV parsing stuff
    PSIn SIn = TFIn::New(FNm);
    char SsCh = ' '; TStrV FldValV;
    // read the headers and initialise the feature generators
    TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);
    for (int FldValN = 0; FldValN < FldValV.Len(); FldValN++) {
        const TStr& FldVal = FldValV[FldValN];
        if (FldValN == ClassId) {
            if (FldVal == "NOM") {
                ClassFtrGen = TFtrGenNominal::New();
            } else if (FldVal == "MULTI-NOM") {
                ClassFtrGen = TFtrGenMultiNom::New();
            } else {
                TExcept::Throw("Wrong class type '" + FldVal + "', should be NOM or MULTI-NOM!");
            }
        } else if (!IgnoreIdV.IsIn(FldValN)) {
            if (FldVal == TFtrGenNumeric::Type) {
                FtrGenH.AddDat(FldValN, TFtrGenNumeric::New());
            } else if (FldVal == TFtrGenNominal::Type) {
                FtrGenH.AddDat(FldValN, TFtrGenNominal::New());
            } else if (FldVal == TFtrGenToken::Type) {
                FtrGenH.AddDat(FldValN, TFtrGenToken::New(
                    TSwSet::New(swstNone), TStemmer::New(stmtNone)));
            } else if (FldVal == TFtrGenSparseNumeric::Type) {
                FtrGenH.AddDat(FldValN, TFtrGenSparseNumeric::New());
            } else if (FldVal == TFtrGenMultiNom::Type) {
                FtrGenH.AddDat(FldValN, TFtrGenMultiNom::New());
            } else {
                TExcept::Throw("Wrong type '" + FldVal + "'!");
            }
        }
    }
    const int Flds = FldValV.Len();
    // read the lines and feed them to the feature generators
    int Recs = 0;
    while (!SIn->Eof()) {
        if (Recs == TrainLen) { break; }
        Recs++; printf("%7d\r", Recs);
        TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);
        // make sure line still has the same number of fields as the header
        EAssertR(FldValV.Len() == Flds,
            TStr::Fmt("Wrong number of fields in line %d! Found %d and expected %d!",
            Recs + 1, FldValV.Len(), Flds));
        // go over lines
        try {
            for (int FldValN = 0; FldValN < FldValV.Len(); FldValN++) {
                const TStr& FldVal = FldValV[FldValN];
                if (FldValN == ClassId) {
                    ClassFtrGen->Update(FldVal);
                } else if (FtrGenH.IsKey(FldValN)) {
                    PFtrGen FtrGen = FtrGenH.GetDat(FldValN);
                    FtrGen->Update(FldVal);
                }
            }
        } catch (PExcept Ex) {
            TExcept::Throw(TStr::Fmt("Error in line %d: '%s'!",
                Recs+1, Ex->GetMsgStr().CStr()));
        }
    }
    // read the file again and feed it to the training set
    PBowDocBs BowDocBs = TBowDocBs::New();
    int WdsOffset = 0;
    for (int FtrGenId = 0; FtrGenId < FtrGenH.Len(); FtrGenId++) {
        PFtrGen FtrGen = FtrGenH[FtrGenId];
        TStr Prefix = FtrGenH.GetKey(FtrGenId).GetStr();
        FtrGen->AddWds(Prefix, BowDocBs, WdsOffset);
    }
    // we read and ignore the headers since we parsed them already
    SIn = TFIn::New(FNm); SsCh = ' ';
    TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);
    // read the lines and feed them to the training set
    Recs = 0;
    while (!SIn->Eof()){
        Recs++; printf("%7d\r", Recs);
        TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);
        // make sure line still has the same number of fields as the header
        EAssertR(FldValV.Len() == Flds,
            TStr::Fmt("Wrong number of fields in line %s! Found %d and expected %d!",
            Recs + 1, FldValV.Len(), Flds));
        // go over lines and construct the sparse vector
        TIntFltKdV FtrSpV; TStrV CatNmV; int Offset = 0;
        try {
            for (int FldValN = 0; FldValN < FldValV.Len(); FldValN++) {
                const TStr& FldVal = FldValV[FldValN];
                if (FldValN == ClassId) {
                    ClassFtrGen->GetValV(FldVal, CatNmV);
                } else if (FtrGenH.IsKey(FldValN)) {
                    PFtrGen FtrGen = FtrGenH.GetDat(FldValN);
                    FtrGen->Add(FldVal, FtrSpV, Offset);
                }
            }
        } catch (PExcept Ex) {
            TExcept::Throw(TStr::Fmt("Error in line %d: '%s'!",
                Recs+1, Ex->GetMsgStr().CStr()));
        }
        // make KdV to PrV
        const int WIds = FtrSpV.Len();
        TIntFltPrV WIdWgtPrV(WIds, 0);
        for (int WIdN = 0; WIdN < WIds; WIdN++) {
            WIdWgtPrV.Add(TIntFltPr(FtrSpV[WIdN].Key, FtrSpV[WIdN].Dat));
        }
        // add the feature vector to trainsets
        BowDocBs->AddDoc(TStr::Fmt("Line-%d", Recs), CatNmV, WIdWgtPrV);
    }
    return BowDocBs;
}
