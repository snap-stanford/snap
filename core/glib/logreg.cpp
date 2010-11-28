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

PLogRegMd TLogReg::CalcLogReg(PSVMTrainSet Set, const double& ChangeEps, const int& MaxStep) {
    TFltV y, bb; TSTSetMatrix TermDocM(Set, y); 
    IRLS(TermDocM, y, bb, ChangeEps, MaxStep, 2); 
    return new TLogRegMd(bb); 
};

PLogRegMd TLogReg::CalcLogReg(TMatrix& Set, TFltV& y, const double& ChangeEps, const int& MaxStep) {
    TFltV bb; 
    IRLS(Set, y, bb, ChangeEps, MaxStep, 2);
    return new TLogRegMd(bb); 
};

PBowMd TLogReg::CalcLogReg(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs, const TStr& CatNm, const TIntV& DIdV, 
        const double& Treshold, const double& ChangeEps, const int& MaxStep, const int& Verbosity) { 

    if (Verbosity > 0) printf("Loading matrix...");
    TFltV y, bb; TBowMatrix TermDocM(BowDocBs, BowDocWgtBs, CatNm, DIdV, y); 
    if (Verbosity > 0) printf("done\n");

    if (Verbosity > 0) printf("Learning...\n");
    IRLS(TermDocM, y, bb, ChangeEps, MaxStep, Verbosity); 

    return new TBowLogRegMd(BowDocBs, CatNm, Treshold, bb);     
} 

///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model
double TLogRegMd::GetCfy(const TIntFltKdV& AttrV) {
    int len = AttrV.Len();
    double res = bb.Last();
    for (int i = 0; i < len; i++) {
        if (AttrV[i].Key < bb.Len())
            res += AttrV[i].Dat * bb[AttrV[i].Key];
    }
    double mu = 1/(1 + exp(-res));
    return mu;
}

TCfyRes TLogRegMd::Test(PSVMTrainSet Set, const double& threshold) {
    TCountCfyRes Result;
    for (int i = 0; i < Set->Len(); i++)
        Result.Add(GetCfy(Set->GetAttrSparseV(i)) - threshold, Set->GetVecParam(i));

    return Result.ToTCfyRes();
}

///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model
bool TBowLogRegMd::IsReg=TBowLogRegMd::MkReg();

void TBowLogRegMd::GetCfy(const PBowSpV& QueryBowSpV, TFltStrPrV& WgtCatNmPrV) {
    int len = QueryBowSpV->Len(), wlen = WeightV.Len();
    double res = WeightV.Last();
    for (int i = 0; i < len; i++) {
        if (QueryBowSpV->GetWId(i) < wlen)
            res += QueryBowSpV->GetWgt(i) * WeightV[QueryBowSpV->GetWId(i)];
    }
    double mu = 1/(1 + exp(-res));
    WgtCatNmPrV.Clr();
    if (mu >= Treshold) WgtCatNmPrV.Add(TFltStrPr(mu, CatNm));
}
