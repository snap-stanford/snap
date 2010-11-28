//////////////////////////////////////////////////////////////////////////
// String-To-Words
void TStrParser::DocStrToWIdV(const TStr& _DocStr, TIntV& WordIdV, const bool& Stemm) {
    TStr DocStr = _DocStr.GetUc();  // to upper case
    TStrV WordV; DocStr.SplitOnWs(WordV); int WordN = WordV.Len();
    WordIdV.Reserve(WordN, 0);

    PStemmer Stemmer = TStemmer::New(stmtPorter);
    TIntH WordsInDoc;
    for (int WordC = 0; WordC < WordN; WordC++) {
        TStr WordStr;
        if (Stemm) {
            WordStr = Stemmer->GetStem(WordV[WordC]);
        } else {
            WordStr = WordV[WordC];
        }
        int WId = GetWId(WordStr);
        if (WId == -1) {
            WId = WordToIdH.AddKey(WordStr);
            WordToIdH[WId] = 0;
        }
        WordIdV.Add(WId);
        
        // is it first time we see this word in this doc?
        if (!WordsInDoc.IsKey(WId)) WordsInDoc.AddKey(WId);
    }

    //do some statistics for DF
    DocsParsed++;
    for (int i = 0, l = WordsInDoc.Len(); i < l; i++)
        WordToIdH[WordsInDoc.GetKey(i)]++;

    Assert(WordV.Len() == WordIdV.Len());
}

void TStrParser::DocStrToSylIdV(const TStr& _DocStr, TIntV& SyllablesIdV) {
    TStr DocStr = _DocStr.GetUc();  // to upper case
    Fail;
}

void TStrParser::DocStrToChIdV(const TStr& _DocStr, TIntV& ChIdV) {
    TStr DocStr = _DocStr.GetUc();  // to upper case
    int ChN = DocStr.Len();
    ChIdV.Reserve(ChN, 0);
    for (int ChC = 0; ChC < ChN; ChC++) {
        TStr ChStr = DocStr.GetSubStr(ChC,ChC);
        int ChId = GetWId(ChStr);
        if (ChId != -1) {
            WordToIdH[ChId]++;
        } else {
            ChId = WordToIdH.AddKey(ChStr);
            WordToIdH[ChId] = 1;
        }
        ChIdV.Add(ChId);
    }
}

void TStrParser::WhoAmI(const TStr& intro) const {
    switch (Type.Val) {
        case 0: printf("%s: Words [AlphN = %d]\n", intro.CStr(), GetAlphabetSize()); break;
        case 1: printf("%s: Syllabels  [AlphN = %d]\n", intro.CStr(), GetAlphabetSize()); break;
        case 2: 
            printf("%s: Chararcters [AlphN = %d]\n", intro.CStr(), GetAlphabetSize()); 
            for (int i = 0; i < WordToIdH.Len(); i++) {
                printf("[%s]  ", WordToIdH.GetKey(i).CStr());
            }
            printf("\n");
            break;
    }
}

void TStrParser::GetIDFWeightV(TFltV& WeightV) {
    int AlphN = GetAlphabetSize();
    WeightV.Gen(AlphN);
    for (int AlphC = 0; AlphC < AlphN; AlphC++) 
        WeightV[AlphC] = log((double)DocsParsed / WordToIdH[AlphC]);
    double MaxVal = WeightV[WeightV.GetMxValN()];
    for (int AlphC = 0; AlphC < AlphN; AlphC++) 
        WeightV[AlphC] /= MaxVal;
}

////////////////////////////////////////////////////////////////////////////
//// Similarity-Matrix
//void TSimMatrix::SetIdV(PStrParser Parser) {
//    int PIdN = Parser->GetWords(); PIdToWId.Gen(PIdN);
//    for (int PIdC = 0; PIdC < PIdN; PIdC++) {
//        if (DOPRINT && PIdC % 5 == 0) printf("\r%d", PIdC);
//        PIdToWId[PIdC] = WordNet->GetWId(Parser->GetWordStr(PIdC));
//    }
//}
//
//double TSimMatrix::GetSim(const int& WId1, const int& WId2) {
//    // if words equal, than 1.0
//    if (WId1 == WId2) return 1.0;
//    
//    // check if related
//    int WNId1 = PIdToWId[WId1], WNId2 = PIdToWId[WId2];
//    if (WNId1 == -1 || WNId2 == -1) return 0.0;
//    TIntV SynIdV1, SynIdV2, InterV;
//    WordNet->GetSynsetV(WNId1, SynIdV1);
//    WordNet->GetSynsetV(WNId2, SynIdV2);
//    SynIdV1.Intrs(SynIdV2, InterV);
//    if (InterV.Len() > 0) return 0.6;
//
//    // calculate distance in WordNet graph
////    TIntV SynHp
//
//    // else zero...
//    return 0.0;
//}

//////////////////////////////////////////////////////////////////////////
// String-Kernels
double *TStringKernel::Buf1 = NULL; 
double *TStringKernel::Buf2 = NULL; 
int TStringKernel::BufN = 0;
int TStringKernel::CountEval = 0;

double TStringKernel::KDynamic(const TIntV& s, const TIntV& t, const TFltV& lc, const double& lb) {
    const int k = lc.Len() + 1;
    int x,y,i;

    int ls = s.Len(), lt = t.Len();
    
    //TVec<TFltVV> Kd(2);
    //for (i = 0; i < 2; i++) Kd[i].Gen(ls+1, lt+1);

    // s is on X-axis and t is on Y-axis
    TVec<double *> Kd(2);
    if ((ls+1)*(lt+1) > BufN) {
        if (Buf1 != NULL) delete[] Buf1;
        if (Buf2 != NULL) delete[] Buf2;
        BufN = (ls+2)*(lt+2) + 10;
        Buf1 = new double[BufN];
        Buf2 = new double[BufN];
    }
    Kd[0] = Buf1; Kd[1] = Buf2;
    double *Kdii, *Kdi; //ii == (i-1)%2, i == i%2

    // initialize Kd for i == 0
    int MxSize = (ls+1)*(lt+1) + 10;
    for (i = 0, Kdi = Kd[0]; i < MxSize; i++) Kdi[i] = 1.0;
    //for (x = 0; x <= ls; x++) {
    //    for (y = 0; y <= lt; y++) {
    //        Kd[0](x,y) = 1.0;
    //    }
    //}

    // calculate Kd for i == 1..k-1
    double K = 0.0; 
    for (i = 1; i < k; i++) {
        Kdi = Kd[i%2]; Kdii = Kd[(i-1)%2];
        for (x = 0; x <= ls; x++) Kdi[x*(lt+1) + (i-1)] = 0.0;
        for (y = 0; y <= lt; y++) Kdi[(i-1)*(lt+1) + y] = 0.0;

        //for (x = 0; x <= ls; x++) Kd[i%2](x,i-1) = 0.0;
        //for (y = 0; y <= lt; y++) Kd[i%2](i-1,y) = 0.0;

        double Ki = 0.0;
        for (x = i; x <= ls; x++) {
            double Kdd = 0.0; int u = s[x-1];
            for (y = i; y <= lt; y++) {
                if (u == t[y-1]) {
                    Kdd = lb * (Kdd + lb*Kdii[(x-1)*(lt+1) + (y-1)]);
                    Ki += lb*lb * Kdi[(x-1)*(lt+1) + (y-1)];
                    //Kdd = lb * (Kdd + lb*Kd[(i-1)%2](x-1,y-1));
                    //Ki += lb*lb * Kd[i%2](x-1, y-1);
                } else {
                    Kdd *= lb;
                }
                Kdi[x*(lt+1) + y] = lb*Kdi[(x-1)*(lt+1) + y] + Kdd;
                //Kd[i%2](x,y) = lb*Kd[i%2](x-1, y) + Kdd;
            }
        }
        K += lc[i-1] * Ki;
    }

    return K;
}

//double TStringKernel::KDynamicSM(const TIntV& s, const TIntV& t, const TFltV& lc, const double& lb, PSimMatrix Sim) {
//    const int k = lc.Len() + 1;
//    int x,y,i;
//    int ls = s.Len(), lt = t.Len();
//
//    // s is on X-axis and t is on Y-axis
//    TVec<double *> Kd(2);
//    if ((ls+1)*(lt+1) > BufN) {
//        if (Buf1 != NULL) delete[] Buf1;
//        if (Buf2 != NULL) delete[] Buf2;
//        BufN = (ls+2)*(lt+2) + 10;
//        Buf1 = new double[BufN];
//        Buf2 = new double[BufN];
//    }
//    Kd[0] = Buf1; Kd[1] = Buf2;
//    double *Kdii, *Kdi; //ii == (i-1)%2, i == i%2
//
//    // initialize Kd for i == 0
//    int MxSize = (ls+1)*(lt+1) + 10;
//    for (i = 0, Kdi = Kd[0]; i < MxSize; i++) Kdi[i] = 1.0;
//
//    // calculate Kd for i == 1..k-1
//    double K = 0.0;
//    for (i = 1; i < k; i++) {
//        Kdi = Kd[i%2]; Kdii = Kd[(i-1)%2];
//        for (x = 0; x <= ls; x++) Kdi[x*(lt+1) + (i-1)] = 0.0;
//        for (y = 0; y <= lt; y++) Kdi[(i-1)*(lt+1) + y] = 0.0;
//
//        double Ki = 0.0;
//        for (x = i; x <= ls; x++) {
//            double Kdd = 0.0; int u = s[x-1];
//            for (y = i; y <= lt; y++) {
//                double Similarity = Sim->GetSim(u, t[y-1]);
//                Kdd = lb * (Kdd + lb*Kdii[(x-1)*(lt+1) + (y-1)]) * Similarity;
//                Kdi[x*(lt+1) + y] = lb*Kdi[(x-1)*(lt+1) + y] + Kdd;
//                Ki = Ki + lb*lb * Kdi[(x-1)*(lt+1) + (y-1)]  * Similarity;
//            }
//        }
//        K += lc[i-1] * Ki;
//    }
//
//    return K;
//}

double TStringKernel::KDynamicW(const TIntV& s, const TIntV& t, const TFltV& lc, const TFltV& wgt) {
    int x,y,i;
    const int k = lc.Len() + 1;

    int ls = s.Len(), lt = t.Len();
    TVec<TFltVV> Kd(2);
    // s is on X-axis and t is on Y-axis
    for (i = 0; i < 2; i++) Kd[i].Gen(ls+1, lt+1);

    // initialize Kd for i == 0
    for (x = 0; x <= ls; x++) {
        for (y = 0; y <= lt; y++) {
            Kd[0](x,y) = 1.0;
        }
    }

    // calculate Kd for i == 1..k-1
    double K = 0.0;
    for (i = 1; i < k; i++) {
        for (x = 0; x <=ls; x++) Kd[i%2](x,i-1) = 0.0;
        for (y = 0; y <=lt; y++) Kd[i%2](i-1,y) = 0.0;
    
        double Ki = 0.0;
        for (x = i; x <= ls; x++) {
            double Kdd = 0.0; int u = s[x-1];
            for (y = i; y <= lt; y++) {
                double w = wgt[t[y-1]];
                if (u == t[y-1]) {
                    Kdd = w * (Kdd + wgt[u]*Kd[(i-1)%2](x-1,y-1));
                    Ki += w*w * Kd[i%2](x-1, y-1);
                } else {
                    Kdd *= (1-w); // u is a gap! (w -> (1-w) ?)
                }
                Kd[i%2](x,y) = wgt[u]*Kd[i%2](x-1, y) + Kdd;
            }
        }
        K += lc[i-1] * Ki;
    }

    return K;
}

double TStringKernel::KTrie(const TIntV& s, const TIntV& t, const double& lb, const int& p, int m, const int& AlphN) {
    int ls = s.Len(), lt = t.Len();
    if (ls < p || lt < p) return 0.0;
    m = TInt::GetMn(m, ls-p, lt-p);

    TVec<TTrieNodeP> Ls, Lt; TIntV v(p), x(p+m); double Kern = 0.0;

    // precalculate weights
    TFltV lbV(m+1); lbV[0] = 1;
    for (int i = 0; i < p; i++) lbV[0] *= lb;
    for (int i = 1; i <= m; i++) lbV[i] = lb * lbV[i-1];

    for (int AlphC = 0; AlphC < AlphN; AlphC++) {
        Ls.Clr(); Lt.Clr();
        for (int i = 0; i <= ls - p; i++) {
            if (s[i] == AlphC) {
                int j = TInt::GetMn(ls, i+p+m);
                Ls.Add(TTrieNodeP(TIntPr(i, j-i), 0, 0)); // i == 0 becasue strings start with 0 (not 1 as in Matlab!)
            }
        }
        for (int i = 0; i <= lt - p; i++) {
            if (t[i] == AlphC) {
                int j = TInt::GetMn(lt, i+p+m);
                Lt.Add(TTrieNodeP(TIntPr(i, j-i), 0, 0)); // i == 0 becasue strings start with 0 (not 1 as in Matlab!)
            }
        }

        v[0] = AlphC; KTrieR(s, t, Ls, Lt, v, 1, Kern, lbV, p, m, AlphN); //depth == 1, not 0 !!!!
    }

    return Kern;
}

void TStringKernel::KTrieR(const TIntV& s, const TIntV& t, const TVec<TTrieNodeP>& Ls, const TVec<TTrieNodeP>& Lt, 
        TIntV& v, const int& depth, double& Kern, const TFltV& lbV, const int& p, const int& m, const int& AlphN) {

    int lls = Ls.Len(), llt = Lt.Len();
    if (lls == 0 || llt == 0) return;

    if (depth == p) {
        double Ks = 0.0, Kt = 0.0;
        for (int i = 0; i < lls; i++) Ks += lbV[Ls[i].Val3];
        for (int i = 0; i < llt; i++) Kt += lbV[Lt[i].Val3];
        Kern = Kern + Ks*Kt;
    } else {
        TVec<TTrieNodeP> Lss, Ltt;

        for (int AlphC = 0; AlphC < AlphN; AlphC++) {
            Lss.Clr(); Ltt.Clr(); v[depth] = AlphC;
            for (int n = 0; n < lls; n++) {
                const TTrieNodeP& node = Ls[n];
                const TIntPr& u = node.Val1; 
                const int i = node.Val2, j = node.Val3;
                const int ustart = u.Val1, lu = u.Val2;
                int lk = TInt::GetMn(m-j, lu-i-2);

                for (int k = 0; k <= lk; k++) {
                    if (s[ustart + i + k + 1] == AlphC)
                        Lss.Add(TTrieNodeP(u, i+k+1, k+j));
                }
            }

            for (int n = 0; n < llt; n++) {
                const TTrieNodeP& node = Lt[n];
                const TIntPr& u = node.Val1;
                const int i = node.Val2, j = node.Val3;
                const int ustart = u.Val1, lu = u.Val2;
                int lk = TInt::GetMn(m-j, lu-i-2);

                for (int k = 0; k <= lk; k++) {
                    if (t[ustart + i + k + 1] == AlphC)
                        Ltt.Add(TTrieNodeP(u, i+k+1, k+j));
                }
            }

            KTrieR(s, t, Lss, Ltt, v, depth+1, Kern, lbV, p, m, AlphN);
        }
    }
}

double TStringKernel::KTrie2(const TIntV& s, const TIntV& t, const double& lb, const int& p, int m, const int& AlphN) {
    int ls = s.Len(), lt = t.Len();
    if (ls < p || lt < p) return 0.0;
    m = TInt::GetMn(m, ls-p, lt-p);

    TVec<TVec<TTrieNodeP> > LsV(AlphN), LtV(AlphN); 
    TIntV v(p), x(p+m); double Kern = 0.0;

    // precalculate weights
    TFltV lbV(m+1); lbV[0] = 1;
    for (int i = 0; i < p; i++) lbV[0] *= lb;
    for (int i = 1; i <= m; i++) lbV[i] = lb * lbV[i-1];
    
    for (int i = 0; i <= ls - p; i++) {
        int j = TInt::GetMn(ls, i+p+m);
        LsV[s[i]].Add(TTrieNodeP(TIntPr(i, j-i), 0, 0)); // i == 0 becasue strings start with 0 (not 1 as in Matlab!)
    }
    for (int i = 0; i <= lt - p; i++) {
        int j = TInt::GetMn(lt, i+p+m);
        LtV[t[i]].Add(TTrieNodeP(TIntPr(i, j-i), 0, 0)); // i == 0 becasue strings start with 0 (not 1 as in Matlab!)
    }

    for (int AlphC = 0; AlphC < AlphN; AlphC++) {
        v[0] = AlphC; 
        KTrieR2(s, t, LsV[AlphC], LtV[AlphC], v, 1, Kern, lbV, p, m, AlphN); //depth == 1, not 0 !!!!
    }

    return Kern;
}

void TStringKernel::KTrieR2(const TIntV& s, const TIntV& t, const TVec<TTrieNodeP>& Ls, const TVec<TTrieNodeP>& Lt, 
        TIntV& v, const int& depth, double& Kern, const TFltV& lbV, const int& p, const int& m, const int& AlphN) {

    int lls = Ls.Len(), llt = Lt.Len();
    if (lls == 0 || llt == 0) return;

    if (depth == p) {
        double Ks = 0.0, Kt = 0.0;
        for (int i = 0; i < lls; i++) Ks += lbV[Ls[i].Val3];
        for (int i = 0; i < llt; i++) Kt += lbV[Lt[i].Val3];
        Kern = Kern + Ks*Kt;
    } else {
        TVec<TVec<TTrieNodeP> > LssV(AlphN), LttV(AlphN);

        for (int n = 0; n < lls; n++) {
            const TTrieNodeP& node = Ls[n];
            const TIntPr& u = node.Val1; 
            const int i = node.Val2, j = node.Val3;
            const int ustart = u.Val1, lu = u.Val2;
            int lk = TInt::GetMn(m-j, lu-i-2);

            for (int k = 0; k <= lk; k++) {
                LssV[s[ustart + i + k + 1]].Add(TTrieNodeP(u, i+k+1, k+j));
            }
        }

        for (int n = 0; n < llt; n++) {
            const TTrieNodeP& node = Lt[n];
            const TIntPr& u = node.Val1;
            const int i = node.Val2, j = node.Val3;
            const int ustart = u.Val1, lu = u.Val2;
            int lk = TInt::GetMn(m-j, lu-i-2);

            for (int k = 0; k <= lk; k++) {
                LttV[t[ustart + i + k + 1]].Add(TTrieNodeP(u, i+k+1, k+j));
            }
        }

        for (int AlphC = 0; AlphC < AlphN; AlphC++) {
            v[depth] = AlphC;
            KTrieR2(s, t, LssV[AlphC], LttV[AlphC], v, depth+1, Kern, lbV, p, m, AlphN);
        }
    }
}

TStringKernel::TStringKernel(TSIn& SIn) {
    KernelType = TInt(SIn); Lambda = TFlt(SIn); SeqLen = TInt(SIn);
    GapLen = TInt(SIn); AlphN = TInt(SIn);
    LinCombV.Load(SIn); WeightV.Load(SIn);
}

void TStringKernel::Save(TSOut& SOut) {
    KernelType.Save(SOut); Lambda.Save(SOut); SeqLen.Save(SOut);
    GapLen.Save(SOut); AlphN.Save(SOut);
    LinCombV.Save(SOut); WeightV.Save(SOut);
}

double TStringKernel::CalcKernel(const TIntV& Vec1, const TIntV& Vec2) {
    CountEval++; //if (CountEval % 1000 == 0) printf("         - %d        \r", CountEval);
    switch (KernelType.Val) {
        case 0: return KDynamic(Vec1, Vec2, LinCombV, Lambda);
        case 1: return KDynamicW(Vec1, Vec2, LinCombV, WeightV);
        case 2: return KTrie(Vec1, Vec2, Lambda, SeqLen, GapLen, AlphN);
        case 3: return KTrie2(Vec1, Vec2, Lambda, SeqLen, GapLen, AlphN);
        //case 4: return KDynamicSM(Vec1, Vec2, LinCombV, Lambda, SimMatrix);
    }
    Fail; return 0.0;
}

void TStringKernel::WhoAmI(const TStr& intro) const {
    switch (KernelType.Val) {
        case 0: printf("%s KDynamic [Lambda=%.3f, LinComb=", intro.CStr(), Lambda());
                for (int i = 0; i < LinCombV.Len(); i++) printf(" %d:%g", i+2, LinCombV[i]());
                printf("]\n"); break;
        case 1: printf("%s [KDynamicLCW]: LinComb = ", intro.CStr());
                for (int i = 0; i < LinCombV.Len(); i++) printf("%d:%g ", i+2, LinCombV[i]());
                printf("\n"); break;
        case 2: printf("%s KTrie [Lambda = %g, SeqLen = %d, GapLen = %d, AlphN = %d]\n", 
                    intro.CStr(), Lambda(), SeqLen(), GapLen(), AlphN()); break;
        case 3: printf("%s KTrie2 [Lambda = %g, SeqLen = %d, GapLen = %d, AlphN = %d]\n", 
                    intro.CStr(), Lambda(), SeqLen(), GapLen(), AlphN()); break;
        case 4: printf("%s KDynamicSM+WordNet [Lambda=%.3f LinComb=", intro.CStr(), Lambda());
                for (int i = 0; i < LinCombV.Len(); i++) printf(" %d:%g", i+2, LinCombV[i]());
                printf("]\n"); break;
    }
}

int TStringKernel::GetSeqLen() const {
    if (KernelType == 1) return LinCombV.Len() + 1;
    return SeqLen;
}

////////////////////////////////////////////////////////////////////////////
//// Ortogonal-System
//TOrtoSystem::TOrtoSystem(PStringTrainSet BigSet, const int& Dim, int Len) {
//    StrKer = BigSet->StrKer;
//    int AlphN = BigSet->AlphN;
//    if (Len < 1)Len = StrKer->GetSeqLen();
//
//    printf("init...");
//    int FeatureN = AlphN;
//    for (int i = 1; i < Len; i++) FeatureN *= AlphN;
//    printf("%d^%d...", AlphN, Len);
//    THash<TIntV, TFlt> FtrCountH;
//    
//    // count all countinuous features of length Len
//    //double PosW = 1.0/BigSet->PosN, NegW = 1.0/BigSet->NegN;
//    //printf("counting[%g,%g]...", PosW, NegW);
//    printf("counting...");
//    for (int DocC = 0, DocN = BigSet->Len(); DocC < DocN; DocC++) {
//        const TIntV& DocVec = BigSet->GetDocVec(DocC);
//        double Wgt = 1.0; //BigSet->GetVecParam(DocC) > 0.0 ? PosW : NegW;
//        for (int ChC = 0, ChN = DocVec.Len() - Len; ChC < ChN; ChC++) {                        
//            TIntV Ftr(Len); 
//            for (int i = 0; i < Len; i++) {
//                Ftr[i] = DocVec[ChC + i];
//            }
//            int FtrId = FtrCountH.GetKeyId(Ftr);
//            if (FtrId != -1) {
//                FtrCountH[FtrId] += Wgt;
//            } else  {
//                FtrId = FtrCountH.AddKey(Ftr);
//                FtrCountH[FtrId] = Wgt;
//            }            
//        }
//    }
//    
//    // find features with highest frequency
//    int NonZeroN = FtrCountH.Len();  printf("nonzero(%d)...sorting...", NonZeroN);
//    TVec<TPair<TFlt, TIntV> > FeatureV(NonZeroN, 0);
//    for (int FtrC = 0; FtrC < NonZeroN; FtrC++) 
//        FeatureV.Add(TPair<TFlt, TIntV>(FtrCountH[FtrC], FtrCountH.GetKey(FtrC)));
//    FeatureV.Sort(false);
//
//    int l = TInt::GetMn(Dim, FeatureV.Len());
//    for (int FeatureC = 0; FeatureC < l; FeatureC++) {
//        TIntV Ftr = FeatureV[FeatureC].Val2;
//        IAssert(Ftr.Len() == Len);
//        Basis.Add(Ftr); 
//        Norm2V.Add(StrKer->CalcKernel(Ftr, Ftr));
//    }
//    Basis.Pack(); Norm2V.Pack();
//}
//
//void TOrtoSystem::Project(const TIntV& x, const double& Norm2, TFltV& y) {
//    y.Reserve(Basis.Len(), 0);
//    for (int i = 0; i < Basis.Len(); i++)
//        y.Add(StrKer->CalcKernel(x, Basis[i]) / sqrt(Norm2 * Norm2V[i]));
//}
//
//PDenseTrainSet TOrtoSystem::Project(PStringTrainSet BigSet) {
//    PDenseTrainSet SmallSet = TDenseTrainSet::New();
//    for (int VecC = 0, VecN = BigSet->Len(); VecC < VecN; VecC++) {
//        if (VecC % 5 == 4) printf("\r%d", VecC+1);
//        TFltV y; Project(BigSet->GetDocVec(VecC), BigSet->GetNorm2(VecC), y);
//        SmallSet->AddAttrV(y, BigSet->GetVecParam(VecC), false);
//    }
//    printf("\n");
//    return SmallSet;
//}
//
