//////////////////////////////////////////////////////////////////////////
// Kernel utilities
void TKernelUtil::CalcKernelMatrix(PSVMTrainSet Set, TFltVV& K) {
    const int Size = Set->Len(); K.Gen(Size, Size);
    for (int i = 0; i < Size; i++) {
        for (int j = i; j < Size; j++)
            K(i,j) = K(j,i) = Set->DotProduct(i,j);
    }
}

void TKernelUtil::CenterKernelMatrix(TFltVV& K) {
    IAssert(K.GetXDim() == K.GetYDim());
    const int l = K.GetYDim();

    TFltV jK(l);   // j'K
    double jKj = 0.0; // j'Kj
    for (int j = 0; j < l; j++) {
        jK[j] = 0.0;
        for (int i = 0; i < l; i++)
            jK[j] += K(i,j);
        jKj += jK[j];
    }

    double invl = 1.0/l;
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < l; j++)
            K(i,j) = K(i,j) - invl*jK[j] - invl*jK[i] + invl*invl*jKj;
    }
}

//////////////////////////////////////////////////////////////////////////
// Partial-Gram-Schmidt
TPartialGS::TPartialGS(PSVMTrainSet BigSet, const int& Dim, const double& Eps) {
    IAssert(Dim <= BigSet->Len() && 0.0 <= Eps && Eps < 1.0);
    int Len = BigSet->Len();

    TVec<TKeyDat<TFlt, TBool> > NiV(Len);
    for (int i = 0; i < Len; i++) {
        //NiV[i].Key = BigSet->DotProduct(i, i);
        NiV[i].Key = BigSet->GetNorm2(i);
        NiV[i].Dat = false;
        IAssertR(NiV[i].Key.Val > 0.0 && _isnan(NiV[i].Key.Val) == 0, 
                 TInt::GetStr(i) + TStr(":") + TFlt::GetStr(NiV[i].Key));
    }
    R.Gen(Dim, 0);
    //for (i = 0; i < Dim; i++) R[i].Gen(Len-i);
    IdV.Gen(Len);
    for (int i = 0; i < Len; i++) IdV[i] = i;

    TFltV BlufV(Dim, 0); int max = -1;
    for (int j = 0; j < Dim; j++) {
        // find element with bigest residual norm
        max = -1;
        for (int t = 0, l = Len; t < l; t++)
            if (!NiV[t].Dat && (max == -1 || NiV[t].Key > NiV[max].Key)) max = t;

        // if max residual norm is reached
        if (NiV[max].Key.Val < Eps) break;
        //printf("(%.2f)", NiV[max].Key.Val);

        // permute j-th and max-th column of R
        NiV[max].Dat = true;
        int mid = IdV.SearchForw(max, j);
        { int tmp = IdV[j]; IdV[j] = max; IdV[mid] = tmp; }
        for (int t = 0; t < j; t++) {
            double tmp = R[t][j-t];
            R[t][j-t] = R[t][mid-t];
            R[t][mid-t] = tmp;
        }

        // calculate j-th row of R and update NiV (residual norms)
        if (-0.001 < NiV[max].Key.Val && NiV[max].Key.Val < 0) NiV[max].Key.Val = 0.0;
        IAssertR(NiV[max].Key.Val >= 0.0, TInt::GetStr(j) + TStr(":") + TFlt::GetStr(NiV[max].Key.Val));
        IAssert(R.Len() == j);
        R.Add(TFltV()); R[j].Gen(Len-j); // NEW
        R[j][0] = sqrt(NiV[max].Key.Val);
        BlufV.Add(NiV[IdV[j]].Key.Val);
        for (int i = j+1; i < Len; i++) {
            double RR = BigSet->DotProduct(IdV[i], IdV[j]);
            for (int t = 0; t < j; t++)
                RR -= R[t][j-t] * R[t][i-t];
            IAssertR(NiV[IdV[j]].Key.Val>0, TInt::GetStr(i));
            RR /= sqrt(NiV[IdV[j]].Key.Val);
            IAssertR(_isnan(RR) == 0, TInt::GetStr(IdV[j]) + TStr(":") + TFlt::GetStr(NiV[IdV[j]].Key.Val));
            R[j][i-j] = RR;
            NiV[IdV[i]].Key -= RR*RR;
        }
    }

    if (max == -1) max = 0;
    printf("stoped at %d/%d with residual norm %.3f\n", R.Len(), BigSet->Len(), NiV[max].Key.Val);

    NormV.Gen(Len);
    VecNormV.Gen(Len);
    for (int i = 0; i < Len; i++) {
        NormV[i] = NiV[IdV[i]].Key;
        VecNormV[i] = GetKernel(i,i);
    }
}

double TPartialGS::GetKernel(const int& VecId1, const int& VecId2) {
    double Result = 0.0;

    int Id1 = IdV.SearchForw(VecId1), Id2 = IdV.SearchForw(VecId2);
    int l = TInt::GetMn(R.Len()-1, Id1, Id2);
    for (int i = 0; i <= l; i++) {
        Result += R[i][Id1 - i] * R[i][Id2 - i];
    }

    return Result;
}

void TPartialGS::GetDocVV(TFltVV& DocVV) {
    IAssert(R.Len() > 0);
    const int DocN = R[0].Len();
    const int DimN = R.Len();
    DocVV.Gen(DimN, DocN);

    for (int DocC = 0; DocC < DocN; DocC++) {
        const int DId = IdV[DocC];
        int l = TInt::GetMn(DocC, DimN-1);
        for (int i = 0; i <= l; i++)
            DocVV(i,DId) = R[i][DocC - i];    
        for (int i = l+1; i < DimN; i++)
            DocVV(i,DId) = 0.0;
    }
}

void TPartialGS::GetBasisV(TVec<TFltV>& q) {
    const int l = R.Len();

    q.Gen(l);
    for (int i = 0; i < l; i++) {
        q[i].Gen(l); 
        q[i].PutAll(0.0);
    }

    for (int n = 0; n < l; n++) {
        double Rnn = R[n][0];
        for (int j = 0; j < n; j++) {
            TLinAlg::AddVec(-R[j][n-j]/Rnn, q[j], q[n], q[n]);
        }
        q[n][n] = 1/Rnn;
    }
}

double TPartialGS::DotProdoctWithCol(const int& ColId, const TFltV& w) {
    Assert(w.Len() == R.Len());
    int l = TInt::GetMn(ColId, R.Len()-1);
    double Result = 0.0;
    for (int i = 0; i <= l; i++)
        Result += R[i][ColId - i] * w[i];
    return Result;
}

void TPartialGS::Dump(const TStr& FName) {
    PSOut out = TFOut::New(FName);
    for (int i = 0; i < R.Len(); i++) {
        for (int j = 0; j < i; j++) {
            out->PutStr(TFlt::GetStr(0.0, 20, 18)); out->PutCh(' '); }
        for (int j = 0; j < R[i].Len(); j++) {
            out->PutStr(TFlt::GetStr(R[i][j], 20, 18)); out->PutCh(' '); }
        out->PutCh('\n');
    }
}
