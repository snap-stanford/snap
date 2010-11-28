///////////////////////////////////////////////////////////////////////
// BagOfWords-Column-Matrix
TBowMatrix::TBowMatrix(const TVec<PBowSpV>& BowSpV): TMatrix() {
    RowN = 0;
    ColSpVV.Gen(BowSpV.Len(), 0);
    for (int i = 0; i < BowSpV.Len(); i++) {
        ColSpVV.Add(BowSpV[i]);
        if (BowSpV[i]->Len() > 0) {
            RowN = TInt::GetMx(RowN, BowSpV[i]->GetWId(BowSpV[i]->GetWIds()-1)+1);
        }
    }
}

TBowMatrix::TBowMatrix(PBowDocWgtBs BowDocWgtBs): TMatrix() {
    RowN = BowDocWgtBs->GetWords();
    ColSpVV.Gen(BowDocWgtBs->GetDocs(), 0);
    for (int i = 0; i < BowDocWgtBs->GetDocs(); i++) {
        ColSpVV.Add(BowDocWgtBs->GetSpV(i));
    }
}

TBowMatrix::TBowMatrix(PBowDocWgtBs BowDocWgtBs, const TIntV& DIdV): TMatrix() {
    RowN = BowDocWgtBs->GetWords();
    ColSpVV.Gen(DIdV.Len(), 0);
    for (int i = 0; i < DIdV.Len(); i++) {
        ColSpVV.Add(BowDocWgtBs->GetSpV(DIdV[i]));
    }
}

TBowMatrix::TBowMatrix(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TStr& CatNm,  const TIntV& DIdV, TFltV& ClsV): TMatrix() {

    RowN = BowDocBs->GetWords();
    ClsV.Gen(DIdV.Len(), 0);
    ColSpVV.Gen(DIdV.Len(), 0);
    IAssert(BowDocBs->IsCatNm(CatNm));
    int CatId = BowDocBs->GetCId(CatNm);
    for (int i = 0; i < DIdV.Len(); i++) {
        ColSpVV.Add(BowDocWgtBs->GetSpV(DIdV[i]));
        ClsV.Add(BowDocBs->IsCatInDoc(DIdV[i], CatId) ? 0.99 : -0.99);
    }
}

void TBowMatrix::PMultiply(const TFltVV& B, int ColId, TFltV& Result) const {
    IAssert(B.GetXDim() >= PGetCols() && Result.Len() >= PGetRows());
    int RowN = PGetRows(), ColN = PGetCols();
    int i, j; //TFlt *ResV = Result.BegI();
    for (i = 0; i < RowN; i++) Result[i] = 0.0;
    for (j = 0; j < ColN; j++) {
        PBowSpV ColV = ColSpVV[j]; int len = ColV->Len();
        for (i = 0; i < len; i++) {
            Result[ColV->GetWId(i)] += ColV->GetWgt(i) * B(j,ColId);
        }
    }
}

void TBowMatrix::PMultiply(const TFltV& Vec, TFltV& Result) const {
    IAssert(Vec.Len() >= PGetCols() && Result.Len() >= PGetRows());
    int RowN = PGetRows(), ColN = PGetCols();
    int i, j; //TFlt *ResV = Result.BegI();
    for (i = 0; i < RowN; i++) Result[i] = 0.0;
    for (j = 0; j < ColN; j++) {
        PBowSpV ColV = ColSpVV[j]; int len = ColV->Len();
        for (i = 0; i < len; i++) {
            Result[ColV->GetWId(i)] += ColV->GetWgt(i) * Vec[j];
        }
    }
}

void TBowMatrix::PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const {
    IAssert(B.GetXDim() >= PGetRows() && Result.Len() >= PGetCols());
    int ColN = PGetCols();
    int i, j, len; //TFlt *ResV = Result.BegI();
    for (j = 0; j < ColN; j++) {
        PBowSpV ColV = ColSpVV[j];
        len = ColV->Len(); Result[j] = 0.0;
        for (i = 0; i < len; i++) {
            Result[j] += ColV->GetWgt(i) * B(ColV->GetWId(i), ColId);
        }
    }
}

void TBowMatrix::PMultiplyT(const TFltV& Vec, TFltV& Result) const {
    IAssert(Vec.Len() >= PGetRows() && Result.Len() >= PGetCols());
    int ColN = PGetCols();
    int i, j, len; //TFlt *VecV = Vec.BegI(), *ResV = Result.BegI();
    for (j = 0; j < ColN; j++) {
        PBowSpV ColV = ColSpVV[j];
        len = ColV->Len(); Result[j] = 0.0;
        for (i = 0; i < len; i++) {
            Result[j] += ColV->GetWgt(i) * Vec[ColV->GetWId(i)];
        }
    }
}

///////////////////////////////////////////////////////////////////////
// BagOfWords-Linear-Algebra
double TBowLinAlg::DotProduct(PBowSpV x, PBowSpV y) {
    TBowWIdWgtKd* vec1 = x->BegI();
    TBowWIdWgtKd* vec2 = y->BegI();
    int len1 = x->Len(), len2 = y->Len();
    double res = 0.0; int j1 = 0, j2 = 0;

    while (j1 < len1 && j2 < len2) {
        if (vec1[j1].Key < vec2[j2].Key) { j1++; } 
        else if (vec1[j1].Key > vec2[j2].Key) { j2++; }
        else { res += vec1[j1].Dat * vec2[j2].Dat; j1++; j2++; }
    }

    return res;
}

double TBowLinAlg::DotProduct(const TFltV& x, PBowSpV y) {
    double res = 0.0; int len = y->Len();
    for (int i = 0; i < len; i++) {
       if (y->GetWId(i) < x.Len())
           res += x[y->GetWId(i)] * y->GetWgt(i);
    }
    return res;
}

double TBowLinAlg::DotProduct(const TIntFltKdV& x, PBowSpV y) {
    TBowWIdWgtKd* vec2 = y->BegI();
    int len1 = x.Len(), len2 = y->Len();
    double res = 0.0; int j1 = 0, j2 = 0;

    while (j1 < len1 && j2 < len2) {
        if (x[j1].Key < vec2[j2].Key) { j1++; } 
        else if (x[j1].Key > vec2[j2].Key) { j2++; } 
        else { res += x[j1].Dat * vec2[j2].Dat; j1++; j2++; }
    }

    return res;
}

void TBowLinAlg::AddVec(const double& k, PBowSpV x, TFltV& y) {
    int Len = x->Len();
    for (int i = 0; i < Len; i++) {
        const int ii = x->GetWId(i);
        if (ii < y.Len()) {
            y[ii] += k * x->GetWgt(i);
        }
    }
}

void TBowLinAlg::GetDual(const PBowDocWgtBs& X,
        const PBowSpV& x, TFltV& y, const int& _Docs) {

    const int Docs = (_Docs == -1) ? X->GetDocs() : _Docs;
    y.Gen(Docs, 0); // prepare space
    for (int DId = 0; DId < Docs; DId++) {
        y.Add(TBowLinAlg::DotProduct(x, X->GetSpV(DId)));
    }   
}

void TBowLinAlg::GetDual(const PBowDocWgtBs& X,
        const TIntFltKdV& x, TFltV& y, const int& _Docs) {

    const int Docs = (_Docs == -1) ? X->GetDocs() : _Docs;
    y.Gen(Docs, 0); // prepare space
    for (int DId = 0; DId < Docs; DId++) {
        y.Add(TBowLinAlg::DotProduct(x, X->GetSpV(DId)));
    }   
}

void TBowLinAlg::GetDual(const PBowDocWgtBs& X,
        const TFltV& x, TFltV& y, const int& _Docs) {

    const int Docs = (_Docs == -1) ? X->GetDocs() : _Docs;
    y.Gen(Docs, 0); // prepare space
    for (int DId = 0; DId < Docs; DId++) {
        y.Add(TBowLinAlg::DotProduct(x, X->GetSpV(DId)));
    }   
}
