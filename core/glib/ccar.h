#ifndef CCAR_H
#define CCAR_H

class TCCAR{

public:
	// Maps(translates) a query in the first language to a second language by using correlation analysis
	//
    // Query vector's length equals the number of rows of source language matrix SourceL
    // Result vector's lenth equals the number of rows of the target language matrix TargetL
	//
	// SourceL and TargetL represent two sets of aligned documents (columns) of two languages
	// Regparam regularises the variance matrix of the target language
	// Cgsiter = number of inner iterations of the conjugate gradient method for matrix inverse
	static void Calc(const TFltV& Query, const TMatrix& SourceL, const TMatrix& TargetL, TFltV& Result,
       const double& Regparam = 0.5, const int& Cgsiter = 10);

private:

	static void ConjugGrad(const TMatrix& Matrix, const TFltV& b, TFltV& x,
	    const TFltV& Center, const TFltV& Ones, const double& Regparam,
	    const double& Cgsiter ,TFltV& CM2p, TFltV& CGpom1, TFltV& CGr, TFltV& CGp, TFltV& CGa,
        const double& RelErr = TFlt::EpsHalf, const TFltV& x0 = TFltV());

	static void CenterMultiply(const TMatrix& Matrix, const TFltV& x,TFltV& y, 
        const TFltV& Center, const bool& transposed, const TFltV& Ones);

	static void CenterMultiply2(const TMatrix& Matrix, const TFltV& x,TFltV& y, 
        const TFltV& Center, const TFltV& Ones, const double& Regparam, TFltV& CM2p);


};

class TAlignPairMapCcar : public TAlignPairMap {
public:
    void operator()(const TIntFltKdV& InSpV, const TMatrix& InMatrix, 
            const TMatrix& OutMatrix, TIntFltKdV& OutSpV) const {

        printf("%d - %d\n", InMatrix.GetCols(), OutMatrix.GetCols());
        // prepare input and placeholders for output
        const int InDims = InMatrix.GetRows();
        const int OutDims = OutMatrix.GetRows();
        TFltV InV(InDims), OutV(OutDims);
        InV.PutAll(0.0); OutV.PutAll(0.0);
        for (int InSpN = 0; InSpN < InSpV.Len(); InSpN++) {
            InV[InSpV[InSpN].Key] = InSpV[InSpN].Dat; }
        // do the calculation
        TCCAR::Calc(InV, InMatrix, OutMatrix, OutV);
        // preprocess the output to a sparse vector
        OutSpV.Clr();
        for (int OutN = 0; OutN < OutV.Len(); OutN++) {
            if (OutV[OutN] > TFlt::EpsHalf) {
                OutSpV.Add(TIntFltKd(OutN, OutV[OutN])); 
            }
        }
    }
};

#endif
