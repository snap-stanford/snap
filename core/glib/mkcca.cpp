#include "mkcca.h"

static void ConjugGrad(const TMatrix& Matrix, const TFltV& b, TFltV& x, 
        const int& CGMxIter, const double& RelErr, const TFltV& x0) {

    // prepare start vector
    x.Gen(Matrix.GetCols());
    if (x0.Empty()) { x.PutAll(0.0); }
    else { x = x0; }
    // do the magic
}

void TMKCCASemSpace::CalcKCCA(const TVec<TBowMatrix>& BowMatrixV, const int& Dims, 
        const double& Kapa, const int& CGMxIter, const int& HorstMxIter, 
        TVec<TFltVV>& AlphaVV) {

    // some basic constants
    const int Langs = BowMatrixV.Len();
    IAssert(Langs > 1);
    const int Docs = BowMatrixV[0].GetCols();
    // reservate space on the output
    AlphaVV.Gen(Langs);
    for (int LangN = 0; LangN < Langs; LangN++) {
        IAssert(BowMatrixV[LangN].GetCols() == Docs);
        AlphaVV[LangN].Gen(Docs, Dims);
        AlphaVV[LangN].PutAll(0.0);
    }

    // par vektorjev
    TFltV x(Docs);  // vektor dolzine docs
    TLAMisc::FillRnd(x, TRnd(1)); // napolnemo z random elementi
    TLinAlg::Normalize(x); // normiramo vektor
    TFltV y(BowMatrixV[1].GetRows()); // nek vektor
    // spreminanje elemnta v matriki
    AlphaVV[1](5,4) = 4.43;
    // skalarni produkt 5 stolpca matrike alphaVV[1] z vektorjem x
    TLinAlg::DotProduct(AlphaVV[1], 5, x);
    // pristeje 0.4 * prvi stolpec drugemu stoplcu
    TLinAlg::AddVec(0.4, AlphaVV[1], 0, AlphaVV[1], 1);
    // pristeje 0.4 * prvi stoplec vektorju x
    TLinAlg::AddVec(0.4, AlphaVV[1], 0, x);
    // naracunamo normo petega stoplca
    double Norm = TLinAlg::Norm(AlphaVV[1], 5);
    // preberemo cetrto vrstico matrike
    TFltV z; AlphaVV[1].GetRow(4, z);
    // preberemo cetrti stoplec matrike
    TFltV zz; AlphaVV[1].GetCol(4, zz);
    // mnozimo vektor x z matriko BowMatrixV[1]
    BowMatrixV[1].Multiply(x, y);
    BowMatrixV[1].MultiplyT(y, x);
    BowMatrixV[1].Multiply(AlphaVV[1], 0, y);
}

void TMKCCASemSpace::Calc(const TVec<PBowDocBs>& BowDocBsV, TVec<PSemSpace>& SemSpaceV,
        const int& Dims, const double& Kapa, const int& CGMxIter, const int& HorstMxIter) {

    // prepare matrix with TFIDF vectors for each language
    TVec<TBowMatrix> BowMatrixV;
    for (int BowDocBsN = 0; BowDocBsN < BowDocBsV.Len(); BowDocBsN++) {
        PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(
            BowDocBsV[BowDocBsN], bwwtLogDFNrmTFIDF);
        BowMatrixV.Add(TBowMatrix(BowDocWgtBs));
    }
    // calculate
    TVec<TFltVV> AlphaVV;
    CalcKCCA(BowMatrixV, Dims, Kapa, CGMxIter, HorstMxIter, AlphaVV);
}