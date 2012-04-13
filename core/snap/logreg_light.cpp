///////////////////////////////////////////////////////////////////////
// Logistic regression by gradient ascent
#include "logreg_light.h"

void TLogRegFit::GetNewtonStep(TFltVV& HVV, const TFltV& GradV, TFltV& DeltaLV){
	bool HSingular = false;
	for (int i = 0; i < HVV.GetXDim(); i++) {
		if (HVV(i,i) == 0.0) {
			HVV(i,i) = 0.001;
			HSingular = true;
		}
		DeltaLV[i] = GradV[i] / HVV(i, i);
	}
	if (! HSingular) {
		if (HVV(0, 0) < 0) { // if Hessian is negative definite, convert it to positive definite
			for (int r = 0; r < Theta.Len(); r++) {
				for (int c = 0; c < Theta.Len(); c++) {
					HVV(r, c) = - HVV(r, c);
				}
			}
			TNumericalStuff::SolveSymetricSystem(HVV, GradV, DeltaLV);
		}
		else {
			TNumericalStuff::SolveSymetricSystem(HVV, GradV, DeltaLV);
			for (int i = 0; i < DeltaLV.Len(); i++) {
				DeltaLV[i] = - DeltaLV[i];
			}
		}

	}
}

void TLogRegFit::Hessian(TFltVV& HVV) {
	HVV.Gen(Theta.Len(), Theta.Len());
	TFltV OutV;
	TLogRegPredict::GetCfy(X, OutV, Theta);
	for (int i = 0; i < X.Len(); i++) {
		for (int r = 0; r < Theta.Len(); r++) {
			HVV.At(r, r) += - (X[i][r] * OutV[i] * (1 - OutV[i]) * X[i][r]);
			for (int c = r + 1; c < Theta.Len(); c++) {
				HVV.At(r, c) += - (X[i][r] * OutV[i] * (1 - OutV[i]) * X[i][c]);
				HVV.At(c, r) += - (X[i][r] * OutV[i] * (1 - OutV[i]) * X[i][c]);
			}
		}
	}
	/*
	printf("\n");
	for (int r = 0; r < Theta.Len(); r++) {
		for (int c = 0; c < Theta.Len(); c++) {
			printf("%f\t", HVV.At(r, c).Val);
		}
		printf("\n");
	}
	*/
}

int TLogRegFit::MLENewton(const double& ChangeEps, const int& MaxStep, const TStr PlotNm) {
	TExeTm ExeTm;
	TFltV GradV(Theta.Len()), DeltaLV(Theta.Len());
	TFltVV HVV(Theta.Len(), Theta.Len());
	int iter = 0;
	double MinVal = -1e10, MaxVal = 1e10;
	for(iter = 0; iter < MaxStep; iter++) {
		Gradient(GradV);
		Hessian(HVV);
		GetNewtonStep(HVV, GradV, DeltaLV);
		double Increment = TLinAlg::DotProduct(GradV, DeltaLV);
		if (Increment <= ChangeEps) {break;}
		double LearnRate = GetStepSizeByLineSearch(DeltaLV, GradV, 0.15, 0.5);//InitLearnRate/double(0.01*(double)iter + 1);
		for(int i = 0; i < Theta.Len(); i++) {
			double Change = LearnRate * DeltaLV[i];
			Theta[i] += Change;
			if(Theta[i] < MinVal) { Theta[i] = MinVal;}
			if(Theta[i] > MaxVal) { Theta[i] = MaxVal;}
		}
	}
	if (! PlotNm.Empty()) {
		printf("MLE with Newton method completed with %d iterations(%s)\n",iter,ExeTm.GetTmStr());
	}

	return iter;
}

int TLogRegFit::MLEGradient(const double& ChangeEps, const int& MaxStep, const TStr PlotNm) {
	TExeTm ExeTm;
	TFltV GradV(Theta.Len());
	int iter = 0;
	TIntFltPrV IterLV, IterGradNormV;
	double InitLearnRate = 0.01;
	double MinVal = -1e10, MaxVal = 1e10;
	double GradCutOff = 100000;
	for(iter = 0; iter < MaxStep; iter++) {
		Gradient(GradV);		//if gradient is going out of the boundary, cut off
		for(int i = 0; i < Theta.Len(); i++) {
			if (GradV[i] < -GradCutOff) { GradV[i] = -GradCutOff; }
			if (GradV[i] > GradCutOff) { GradV[i] = GradCutOff; }
			if (Theta[i] <= MinVal && GradV[i] < 0) { GradV[i] = 0.0; }
			if (Theta[i] >= MaxVal && GradV[i] > 0) { GradV[i] = 0.0; }
		}
		double Alpha = 0.15, Beta = 0.9;
		//double LearnRate = 0.1 / (0.1 * iter + 1); //GetStepSizeByLineSearch(GradV, GradV, Alpha, Beta);
		double LearnRate = GetStepSizeByLineSearch(GradV, GradV, Alpha, Beta);
		if (TLinAlg::Norm(GradV) < ChangeEps) { break; }
		for(int i = 0; i < Theta.Len(); i++) {
			double Change = LearnRate * GradV[i];
			Theta[i] += Change;
			if(Theta[i] < MinVal) { Theta[i] = MinVal;}
			if(Theta[i] > MaxVal) { Theta[i] = MaxVal;}
		}
		if (! PlotNm.Empty()) {
			double L = Likelihood();
			IterLV.Add(TIntFltPr(iter, L));
			IterGradNormV.Add(TIntFltPr(iter, TLinAlg::Norm(GradV)));
		}
		
	}
	if (! PlotNm.Empty()) {
		TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
		TGnuPlot::PlotValV(IterGradNormV, PlotNm + ".gradnorm_Q");
		printf("MLE for Lambda completed with %d iterations(%s)\n",iter,ExeTm.GetTmStr());
	}
	return iter;
}

double TLogRegFit::GetStepSizeByLineSearch(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta) {
	double StepSize = 1.0;
	double InitLikelihood = Likelihood();
	IAssert(Theta.Len() == DeltaV.Len());
	TFltV NewThetaV(Theta.Len());
	double MinVal = -1e10, MaxVal = 1e10;
	for(int iter = 0; ; iter++) {
		for (int i = 0; i < Theta.Len(); i++){
			NewThetaV[i] = Theta[i] + StepSize * DeltaV[i];
			if (NewThetaV[i] < MinVal) { NewThetaV[i] = MinVal;	}
			if (NewThetaV[i] > MaxVal) { NewThetaV[i] = MaxVal; }
		}
		if (Likelihood(NewThetaV) < InitLikelihood + Alpha * StepSize * TLinAlg::DotProduct(GradV, DeltaV)) {
			StepSize *= Beta;
		} else {
			break;
		}
	}
	return StepSize;
}

double TLogRegFit::Likelihood(const TFltV& NewTheta) {
	TFltV OutV;
	TLogRegPredict::GetCfy(X, OutV, NewTheta);
	double L = 0;
	for (int r = 0; r < OutV.Len(); r++) {
		L += Y[r] * log(OutV[r]);
		L += (1 - Y[r]) * log(1 - OutV[r]);
	}
	return L;
}

void TLogRegFit::Gradient(TFltV& GradV) {
	TFltV OutV;
	TLogRegPredict::GetCfy(X, OutV, Theta);
	GradV.Gen(M);
	for (int r = 0; r < X.Len(); r++) {
		//printf("Y[%d] = %f, Out[%d] = %f\n", r, Y[r].Val, r, OutV[r].Val);
		for (int m = 0; m < M; m++) {
			GradV[m] += (Y[r] - OutV[r]) * X[r][m];
		}
	}
	//for (int m = 0; m < M; m++) {	printf("Theta[%d] = %f, GradV[%d] = %f\n", m, Theta[m].Val, m, GradV[m].Val); }
}

PLogRegPredict TLogRegFit::CalcLogRegNewton(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm, const double& ChangeEps, const int& MaxStep, const bool Intercept) {

	X = XPt;
	Y = yPt;
	IAssert(X.Len() == Y.Len());
	if (Intercept == false) { // if intercept is not included, add it
		for (int r = 0; r < X.Len(); r++) {	X[r].Add(1); }
	}
	M = X[0].Len();
	for (int r = 0; r < X.Len(); r++) {	IAssert(X[r].Len() == M); }
	for (int r = 0; r < Y.Len(); r++) {	
		if (Y[r] >= 0.99999) { Y[r] = 0.99999; }
		if (Y[r] <= 0.00001) { Y[r] = 0.00001; }
	}
	Theta.Gen(M);
	MLENewton(ChangeEps, MaxStep, PlotNm);
  return new TLogRegPredict(Theta); 
};

PLogRegPredict TLogRegFit::CalcLogRegGradient(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm, const double& ChangeEps, const int& MaxStep, const bool Intercept) {
	X = XPt;
	Y = yPt;
	IAssert(X.Len() == Y.Len());
	if (Intercept == false) { // if intercept is not included, add it
		for (int r = 0; r < X.Len(); r++) {	X[r].Add(1); }
	}
	M = X[0].Len();
	for (int r = 0; r < X.Len(); r++) {	IAssert(X[r].Len() == M); }
	for (int r = 0; r < Y.Len(); r++) {	
		if (Y[r] >= 0.99999) { Y[r] = 0.99999; }
		if (Y[r] <= 0.00001) { Y[r] = 0.00001; }
	}
	Theta.Gen(M);
	MLEGradient(ChangeEps, MaxStep, PlotNm);
  return new TLogRegPredict(Theta); 
};

///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model

double TLogRegPredict::GetCfy(const TFltV& AttrV, const TFltV& NewTheta) {
    int len = AttrV.Len();
    double res = 0;
		if (len < NewTheta.Len()) { res = NewTheta.Last(); } //if feature vector is shorter, add an intercept
    for (int i = 0; i < len; i++) {
			if (i < NewTheta.Len()) { res += AttrV[i] * NewTheta[i]; }
    }
    double mu = 1 / (1 + exp(-res));
    return mu;
}

void TLogRegPredict::GetCfy(const TVec<TFltV>& X, TFltV& OutV, const TFltV& NewTheta) {
	OutV.Gen(X.Len());
	for (int r = 0; r < X.Len(); r++) {
		OutV[r] = GetCfy(X[r], NewTheta);
	}
}
/*
/////////////////////////////////////////////////////////////////////
// old logreg.cpp in Glib
///////////////////////////////////////////////////////////////////////
// Fast-Robust-Logistic-Regression
void TLogReg::CG(const TMatrix& Matrix, const TFltV& w, const TFltV& b, 
                 TFltV& x, const int& MaxStep, const int& Verb) { // x == bb, b == z
    int M = x.Len(), R = b.Len(), i;
    TFltV r(M), p(M), q(M), tmp(R);

    x.PutAll(0.0);
    // calculate right side of system
    for (i = 0; i < R; i++) tmp[i] = w[i] * b[i];
    Matrix.Multiply(tmp, r); r[M-1] = TLAMisc::SumVec(tmp);

    double nro, ro, alpha, beta;
    const double eps = 0.000001;

    // conjugate gradient method - CG
    // from "Templates for the soltuion of linear systems" (M == eye)
    ro = nro = TLinAlg::Norm2(r); int StepN=0;
    for (int k = 1; k <= MaxStep && nro > eps && k <= M; k++) {
        if ((Verb > 1) && (k%10 == 0)) printf(".");
        if (k == 1) {
            p = r;
        } else {
            beta = nro / ro;
            for (i = 0; i < M; i++)
                p[i] = r[i] + beta*p[i];
        }

        // q = A*p = (X'*W*X)*p = (Matrix*W*Matrix')*p
        Matrix.MultiplyT(p, tmp);
        for (i = 0; i < R; i++) tmp[i] = (tmp[i] + p[M-1]) * w[i];
        Matrix.Multiply(tmp, q); q[M-1] = TLAMisc::SumVec(tmp);

        // calcualte new x and residual
        alpha = nro / TLinAlg::DotProduct(p, q);
        for (i = 0; i < M; i++) {
            x[i] = x[i] + alpha * p[i];
            r[i] = r[i] - alpha * q[i];
        }

        ro = nro;
        nro = TLinAlg::Norm2(r);
        StepN=k;
    }
    if (Verb > 1) printf("\nnorm(r) = %.5f at k = %d\n", nro, StepN-1);
}

void TLogReg::IRLS(const TMatrix& Matrix, TFltV& y, TFltV& bb, 
                   const double& ChangeEps, const int& MaxStep, const int& Verb) {
    IAssert(Matrix.GetCols() == y.Len());

    int M = Matrix.GetRows(), R = Matrix.GetCols(), i;
    if (bb.Len() != M+1) { bb.Gen(M+1); bb.PutAll(0.0); }

    TFltV mu(R), w(R), z(R), delta;

    // adjust y
    for (i = 0; i < R; i++) {
        if (y[i] >= 1.0)
            y[i] = 0.999;
        else if (y[i] <= 0.0)
            y[i] = 0.001;
    }

		TFltVV X;
		X.Gen(
    //const double eps = 0.01;
    double NewDEV = 0.0, OldDEV = -100.0;
    forever {
        Matrix.MultiplyT(bb, z);
        for (i = 0; i < R; i++) {
            z[i] += bb[M];
            // evaluate current model
            mu[i] = 1/(1 + exp(-z[i]));
            // calculate weights
            w[i] = mu[i] * (1 - mu[i]);
            // calculate adjusted dependent variables
            z[i] += (y[i] - mu[i]) / w[i];
        }
        // get new aproximation for bb
        CG(Matrix, w, z, bb, MaxStep, Verb);
        // calculate deviance (error measurement)
        NewDEV = 0.0;
        for (i = 0; i < R; i++) {
            double yi = y[i], mui = mu[i];
            NewDEV += yi*log(yi / mui) + (1 - yi)*log((1 - yi)/(1 - mui));
        }
        
        if (Verb == 1) printf(" -> %.5f\n", NewDEV);
        else if (Verb > 1) printf("NewDEV = %.5f\n", NewDEV);

        // do we stop?
        if (fabs(NewDEV - OldDEV) < ChangeEps) break;
        OldDEV = NewDEV;
    }
}



PLogRegPredict TLogReg::CalcLogReg(TMatrix& Set, TFltV& y, const double& ChangeEps, const int& MaxStep) {
    TFltV bb; 
    IRLS(Set, y, bb, ChangeEps, MaxStep, 2);
    return new TLogRegPredict(bb); 
};
*/