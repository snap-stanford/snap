#ifndef MKCCA_H
#define MKCCA_H

#include "mine.h"

class TMKCCASemSpace: TSemSpace {
private:
    static bool IsReg;
    static bool MkReg(){return TSemSpace::Reg(TTypeNm<TMKCCASemSpace>(), &Load);}
private:
    // calculating
    static void ConjugGrad(const TMatrix& Matrix, const TFltV& b, TFltV& x, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones,const int& LangN,  const double& Kapa, TFltV& CGpom1, TFltV& CGr, TFltV& CGp, TFltV& CGa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z,
        const int& CGMxIter = 20, const double& RelErr = TFlt::EpsHalf,
        const TFltV& x0 = TFltV());
	static void ConjugGrad2(const TMatrix& Matrix, const TFltV& b, TFltV& x, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones,const int& LangN,  const double& Kapa, TFltV& CGpom1, TFltV& CGr, TFltV& CGp, TFltV& CGa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z,
        const int& CGMxIter = 20, const double& RelErr = TFlt::EpsHalf,
        const TFltV& x0 = TFltV());
    static void CalcKCCA(const TVec<TBowMatrix>& BowMatrixV, const int& Dims, 
        const double& Kapa, const int& CGMxIter, const int& HorstMxIter, 
        TVec<TVec<TFltV> >& AlphaVV);
	static void CalcKCCA2(const TVec<TBowMatrix>& BowMatrixV, const int& Dims, 
        const double& Kapa, const int& CGMxIter, const int& HorstMxIter, 
        TVec<TVec<TFltV> >& AlphaVV);
	static void CenterMultiply3(const TMatrix& Matrix, const TFltV& x,TFltV& y, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones,const int& LangN,  const double& Kapa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z);
	static void CenterMultiply2(const TMatrix& Matrix, const TFltV& x,TFltV& y, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones,const int& LangN,  const double& Kapa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z);
	static void CenterMultiply(const TMatrix& Matrix, const TFltV& x,TFltV& y, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones, const int& LangN, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1);

private:
    //PBowDocWgtBs BowDocWgtBs;
    //TBowMatrix BowMatrix;
    //TVec<TFltV> AlphaV;


	//TVec<TFltV> CMpom; //Lang(i) Rows(j)
	//TFltV CMpom2;      //Docs
	//TFltV CMy1;        //Docs
	//
	//TFltV CM2z;        //Docs

	//TFltV CGpom1;      //Docs
	//TFltV CGr;         //Docs
	//TFltV CGp;         //Docs
	//TFltV CGa;         //Docs

    
    
public:
    //TVec<TVec<TFltV> > AlphaV;
	
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

    static void Calc(const TVec<PBowDocBs>& BowDocBsV, TVec<PSemSpace>& SemSpaceV, const TStrV& InBowFNmV, const int& trainsize,
        const int& Dims = 100, const double& Kapa = 0.5, const int& CGMxIter = 20,  const int& HorstMxIter = 20);

	//void SaveAlpha(const TStrV& InBowFNmV);
};

#endif