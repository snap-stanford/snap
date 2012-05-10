#ifndef MKCCA_H
#define MKCCA_H

#include "mine.h"

class TMKCCASemSpace: TSemSpace {
private:
    static bool IsReg;
    static bool MkReg(){return TSemSpace::Reg(TTypeNm<TMKCCASemSpace>(), &Load);}
private:
    // calculating
    static void ConjugGrad(const TMatrix& Matrix, const TFltV& b, TFltV& x, 
        const int& CGMxIter = 20, const double& RelErr = TFlt::EpsHalf,
        const TFltV& x0 = TFltV());
    static void CalcKCCA(const TVec<TBowMatrix>& BowMatrixV, const int& Dims, 
        const double& Kapa, const int& CGMxIter, const int& HorstMxIter, 
        TVec<TFltVV>& AlphaVV);

private:
    PBowDocWgtBs BowDocWgtBs;
    TBowMatrix BowMatrix;
    TVec<TFltV> AlphaV;
    
public:
    TMKCCASemSpace();
    static PSemSpace New() {
        return new TMKCCASemSpace(); }

    TMKCCASemSpace(TSIn &SIn): TSemSpace(SIn) { }
    static PSemSpace Load(TSIn &SIn) { return new TMKCCASemSpace(SIn); }
    void Save(TSOut &SOut) const { }

    PBowDocBs Project(PBowDocBs BowDocBs, 
        PBowDocWgtBs BowDocWgtBs, const TIntV& DIdV, 
        const int& Dim, const bool& Normalize) { return TBowDocBs::New(); }
    PBowSpV ProjectSpV(PBowSpV SpV, const int& Dim, 
        const bool& Normalize) { return TBowSpV::New(); }
    void ProjectSpV(PBowSpV SpV, const int& Dim, 
        const bool& Normalize, TIntFltPrV& ProjSpV) { }
    
    void GetBasisV(const int& VecN, TFltV& VecV) { }
    int GetDim() { return 0; }

    static void Calc(const TVec<PBowDocBs>& BowDocBsV, TVec<PSemSpace>& SemSpaceV,
        const int& Dims = 100, const double& Kapa = 0.5, const int& CGMxIter = 20, 
        const int& HorstMxIter = 20);
};

#endif