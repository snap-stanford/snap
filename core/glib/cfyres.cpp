//////////////////////////////////////////////////////////////////////////
// Classification Results
TCfyRes::TCfyRes() {
    AccMom = TMom::New(); PrecMom = TMom::New(); 
    RecMom = TMom::New(); F1Mom = TMom::New(); 
    BreakEvenPointMom = TMom::New(); AvgPrecMom = TMom::New();
}

TCfyRes::TCfyRes(TCountCfyRes& CfyRes) {
    PrecMom = TMom::New(); PrecMom->Add(CfyRes.Prec()); PrecMom->Def();
    RecMom = TMom::New(); RecMom->Add(CfyRes.Rec()); RecMom->Def();
    F1Mom = TMom::New(); F1Mom->Add(CfyRes.F1()); F1Mom->Def();

    AccMom = TMom::New(); AccMom->Add(CfyRes.Acc()); AccMom->Def();
    AvgPrecMom = TMom::New(); AvgPrecMom->Add(CfyRes.AvgPrec());  AvgPrecMom->Def();

    BreakEvenPointMom = TMom::New();
    BreakEvenPointMom->Add(CfyRes.BreakEvenPoint());
    BreakEvenPointMom->Def();
}; 

void TCfyRes::Add(TCfyRes& CfyRes) {
    AccMom->Add(CfyRes.Acc()); PrecMom->Add(CfyRes.Prec());
    RecMom->Add(CfyRes.Rec()); F1Mom->Add(CfyRes.F1());
    BreakEvenPointMom->Add(CfyRes.BreakEvenPoint());
    AvgPrecMom->Add(CfyRes.AvgPrec());
}

void TCfyRes::Def() {
    AccMom->Def(); PrecMom->Def(); RecMom->Def();
    F1Mom->Def(); BreakEvenPointMom->Def(); AvgPrecMom->Def();
}

TStr TCfyRes::GetStatStr(const TStr& Desc) {
    TStr StatStr = Desc + ":\n";
    StatStr += TStr::Fmt("prec: [%5.2f%%,%5.2f%%] rec: [%5.2f%%,%5.2f%%] F1:[%5.2f%%,%5.2f%%]\n",
        100.0*Prec(), 100.0*PrecStDiv(), 100.0*Rec(), 100.0*RecStDiv(), 100.0*F1(), 100.0*F1StDiv());
    StatStr += TStr::Fmt("acc: [%5.2f%%,%5.2f%%] avgprec: [%5.2f%%,%5.2f%%] BEP:[%5.2f%%,%5.2f%%]",
        100.0*Acc(), 100.0*AccStDiv(), 100.0*AvgPrec(), 100.0*AvgPrecStDiv(), 100.0*BreakEvenPoint(), 100.0*BreakEvenPointStDiv());
    return StatStr;
}

void TCfyRes::PrTabStat(const TStr& StatFNm, const TStr& Nm) {
    PSOut SOut = TFOut::New(StatFNm);
    PrTabHeader(SOut);
    PrTabVals(SOut, Nm);
}

void TCfyRes::PrTabHeader(PSOut SOut) {
    SOut->PutStr("Nm\tPrec\t\tRec\t\tF1\t\tAcc\t\tBEP\t");
    for (int StatN = 0; StatN < GetStats(); StatN++) {
        SOut->PutStr("\t" + GetStatNm(StatN)); }
    SOut->PutLn();
}

void TCfyRes::PrTabVals(PSOut SOut, const TStr& Nm) {
    SOut->PutStr(Nm + "\t");
    SOut->PutFlt(Prec()); SOut->PutCh('\t'); SOut->PutFlt(PrecStDiv()); SOut->PutCh('\t');
    SOut->PutFlt(Rec()); SOut->PutCh('\t'); SOut->PutFlt(RecStDiv()); SOut->PutCh('\t');
    SOut->PutFlt(F1()); SOut->PutCh('\t'); SOut->PutFlt(F1StDiv()); SOut->PutCh('\t');
    SOut->PutFlt(Acc()); SOut->PutCh('\t'); SOut->PutFlt(AccStDiv()); SOut->PutCh('\t');
    SOut->PutFlt(BreakEvenPoint()); SOut->PutCh('\t'); SOut->PutFlt(BreakEvenPointStDiv()); 
    for (int StatN = 0; StatN < GetStats(); StatN++) {
        SOut->PutCh('\t'); SOut->PutFlt(GetStatWgt(StatN)); }    
    SOut->PutCh('\n');
}

//////////////////////////////////////////////////////////////////////////
// Count -- Classification Results
TCfyRes TCountCfyRes::ToMacroTCfyRes(TVec<TCountCfyRes>& CfyResV) {
    TCfyRes CfyRes;
    for (int CfyResN = 0; CfyResN < CfyResV.Len(); CfyResN++) {
        CfyResV[CfyResN].PrepareResultV();
        Fail;
        //TODO temporary-used warning CfyRes.Add(CfyResV[CfyResN].ToTCfyRes());
    }
    CfyRes.Def();
    return CfyRes;
}

void TCountCfyRes::Add(const double& PredCls, const double& TrueCls) {
	ResultV.Add(TFltKd(PredCls, TrueCls));
	ResultVSorted = false;
	if (TrueCls > 0.0) { 
        if (PredCls > 0.0) { TPos++; }
        else { FNeg++; }
	} else if (TrueCls < 0.0) { 
        if (PredCls < 0.0) { TNeg++; }
        else { FPos++; }
    } else { Fail; }
}

double TCountCfyRes::AvgPrec() {
	PrepareResultV();
    int PosN = (TPos+FNeg), PosC = 0, i = 0;
    double ap = 0.0;
    while (PosC < PosN) {
        i++;
        if (ResultV[i-1].Dat > 0.0) {
            PosC++;
            ap += (double)PosC/(double)i;
        }
    }
    if (PosN > 0.0) { ap /= PosN; }
    return ap;
}

double TCountCfyRes::BreakEvenPoint() {
	PrepareResultV();
    const int Len = ResultV.Len();
    int TP = 0, FP = 0, TN = FPos + TNeg, FN = TPos + FNeg;
    double Prec = 1.0, Rec = 0.0, Diff = 1.0, BEP = 0.0; 

    for (int i = 0; i < Len; i++) {
        if (Diff > TFlt::Abs(Prec - Rec) && Prec != 0.0 && Rec != 0.0) {
            Diff = TFlt::Abs(Prec - Rec);
            BEP = (Prec + Rec)/2;
        }
        if (ResultV[i].Dat > 0.0) {
            FN--; TP++;
        } else {
            TN--; FP++; 
        }
        Prec = (double)TP/(TP+FP); 
        Rec = (double)TP/(TP+FN);
    }

    return BEP;
}

void TCountCfyRes::PrecRec(TFltV& PrecV, TFltV& RecV) {
	PrepareResultV();
    const int Len = ResultV.Len();
    int TP = 0, FP = 0, TN = FPos + TNeg, FN = TPos + FNeg;
    double Prec = 1.0, Rec = 0.0; 
    PrecV.Gen(Len+1, 0); RecV.Gen(Len+1,0);

    PrecV.Add(Prec); RecV.Add(Rec);
    for (int i = 0; i < Len; i++) {
        if (ResultV[i].Dat > 0.0) { FN--; TP++; }
        else { TN--; FP++; }
        Prec = (double)TP/(TP+FP); Rec = (double)TP/(TP+FN);
        PrecV.Add(Prec); RecV.Add(Rec);
    }
}
