/////////////////////////////////////////////////////////////////////////
// Feature-Generator
THash<TStr, TFtrGen::TFtrGenLoadF> TFtrGen::TypeToLoadFH;

bool TFtrGen::Reg(const TStr& TypeNm, const TFtrGenLoadF& LoadF){
	IAssert(!TypeToLoadFH.IsKey(TypeNm));
	TypeToLoadFH.AddDat(TypeNm, LoadF);
	return true;
}

PFtrGen TFtrGen::Load(TSIn& SIn) {
	TStr TypeNm(SIn);
	int TypeKeyId = -1;
	if (TypeToLoadFH.IsKey(TypeNm, TypeKeyId)){
		TFtrGenLoadF LoadF = TypeToLoadFH[TypeKeyId];
		return (*LoadF())(SIn);
	} else {
		TExcept::Throw("Unknown feature generator " + TypeNm);
		return NULL;
	}
}

void TFtrGen::AddWds(const TStr& Prefix, 
        const PBowDocBs& BowDocBs, int& Offset) const {

    const int Vals = GetVals();
    for (int ValN = 0; ValN < Vals; ValN++) {
        const int WId = BowDocBs->AddWordStr(
            TStr::Fmt("%s-%s", Prefix.CStr(), GetVal(ValN).CStr()));
        IAssert(Offset == WId); Offset++;
    }
}

///////////////////////////////////////
// Numeric-Feature-Generator
bool TFtrGenNumeric::IsReg = TFtrGenNumeric::MkReg();

double TFtrGenNumeric::Trans(const double& Val) const { 
        return MnVal != MxVal ? (double(Val) - MnVal) / (MxVal - MnVal) : 0.0; 
}

double TFtrGenNumeric::GetFlt(const TStr& Str) const {
    double Val = 0.0;
    if (!Str.IsFlt(Val)) { 
        TExcept::Throw(TStr::Fmt("Number expected  but '%s' found!", Str.CStr())); }
    return Val;
}

void TFtrGenNumeric::Update(const TStr& Val) { 
    double Flt = GetFlt(Val);
    MnVal = TFlt::GetMn(MnVal, Flt); 
    MxVal = TFlt::GetMx(MxVal, Flt); 
}

void TFtrGenNumeric::Add(
        const TStr& Val, TIntFltKdV& SpV, int& Offset) const {

    double Flt = GetFlt(Val);
    SpV.Add(TIntFltKd(Offset, Trans(Flt))); 
    Offset++; 
}

///////////////////////////////////////
// Nominal-Feature-Generator
bool TFtrGenNominal::IsReg = TFtrGenNominal::MkReg();

void TFtrGenNominal::Update(const TStr& Val) { 
    if (!Val.Empty()) { ValH.AddKey(Val); }
}

void TFtrGenNominal::Add(
        const TStr& Val, TIntFltKdV& SpV, int& Offset) const {

    if (ValH.IsKey(Val)) { 
        SpV.Add(TIntFltKd(Offset + ValH.GetKeyId(Val), 1.0)); 
    } 
    Offset += ValH.Len(); 
}

///////////////////////////////////////
// Tokenizable-Feature-Generator
bool TFtrGenToken::IsReg = TFtrGenToken::MkReg();

void TFtrGenToken::GetTokenV(const TStr& Str, TStrV& TokenStrV) const {
    THtmlLx HtmlLx(TStrIn::New(Str));
    while (HtmlLx.Sym != hsyEof){
        if (HtmlLx.Sym == hsyStr){ 
            TStr TokenStr = HtmlLx.UcChA;
            if (SwSet.Empty() || !SwSet->IsIn(TokenStr)) { 
                if (!Stemmer.Empty()) { 
                    TokenStr = Stemmer->GetStem(TokenStr); } 
                TokenStrV.Add(TokenStr);
            }
        }
        // get next symbol
        HtmlLx.GetSym();
    }
}

TFtrGenToken::TFtrGenToken(TSIn& SIn): TFtrGen(SIn) { 
	SwSet = PSwSet(SIn);
	Stemmer = PStemmer(SIn);
	Docs.Load(SIn);
	TokenH.Load(SIn);
}

void TFtrGenToken::Save(TSOut& SOut) const { 
	TFtrGen::Save(SOut);
	SwSet.Save(SOut);
	Stemmer.Save(SOut);
	Docs.Save(SOut);
	TokenH.Save(SOut);
}

void TFtrGenToken::Update(const TStr& Val) {
    TStrV TokenStrV; GetTokenV(Val, TokenStrV); TStrH TokenStrH;
    for (int TokenStrN = 0; TokenStrN < TokenStrV.Len(); TokenStrN++) {
        const TStr& TokenStr = TokenStrV[TokenStrN];
        TokenStrH.AddKey(TokenStr);
    }
    int KeyId = TokenStrH.FFirstKeyId();
    while (TokenStrH.FNextKeyId(KeyId)) {
        const TStr& TokenStr = TokenStrH.GetKey(KeyId);
        TokenH.AddDat(TokenStr)++;
    }
    Docs++;
}

void TFtrGenToken::Add(const TStr& Val, TIntFltKdV& SpV, int& Offset) const {
    // step (1): tokenize
    TStrV TokenStrV; GetTokenV(Val, TokenStrV);
    // step (2): aggregate token counts
    TIntH TokenFqH;
    for (int TokenStrN = 0; TokenStrN < TokenStrV.Len(); TokenStrN++) {
        const TStr& TokenStr = TokenStrV[TokenStrN];
        if (TokenH.IsKey(TokenStr)) { 
            const int TokenId = TokenH.GetKeyId(TokenStr);
            TokenFqH.AddDat(TokenId)++;
        }
    }
    // step (3): make a sparse vector out of it
    TIntFltKdV ValSpV(TokenFqH.Len(), 0);
    int KeyId = TokenFqH.FFirstKeyId();
    while (TokenFqH.FNextKeyId(KeyId)) {
        const int TokenId = TokenFqH.GetKey(KeyId);
        const int TokenFq = TokenFqH[KeyId];
        const int TokenDocFq = TokenH[TokenId];
        const double IDF = log(double(Docs) / double(TokenDocFq));
        ValSpV.Add(TIntFltKd(TokenId, double(TokenFq) * IDF));
    }
    ValSpV.Sort(); TLinAlg::NormalizeL1(ValSpV);
    // step (4): add the sparse vector to the final feature vector  
    for (int ValSpN = 0; ValSpN < ValSpV.Len(); ValSpN++) {
        const int Key = ValSpV[ValSpN].Key + Offset;
        const double Dat = ValSpV[ValSpN].Dat;
        SpV.Add(TIntFltKd(Key, Dat));
    }
    Offset += TokenH.Len(); 
}

///////////////////////////////////////
// Sparse-Numeric-Feature-Generator
bool TFtrGenSparseNumeric::IsReg = TFtrGenSparseNumeric::MkReg();

void TFtrGenSparseNumeric::Split(
        const TStr& Str, int& Id, TStr& Val) const {

    if (!Str.IsChIn(':')) { TExcept::Throw("Wrong sparse numeric '" + Str + "'!"); }
    //split
    TStr IdStr; Str.SplitOnCh(IdStr, ':', Val);
    // parse id
    if (!IdStr.IsInt(Id)) { TExcept::Throw("Wrong sparse numeric '" + Str + "'!"); }
}

void TFtrGenSparseNumeric::Update(const TStr& Str) { 
    TStrV EltV; Str.SplitOnAllCh(';', EltV);
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        int Id; TStr Val; Split(EltV[EltN], Id, Val);
        MxId = TInt::GetMx(Id, MxId);
        FtrGen->Update(Val);
    }
}

void TFtrGenSparseNumeric::Add(const TStr& Str, TIntFltKdV& SpV, int& Offset) const {
    TStrV EltV; Str.SplitOnAllCh(';', EltV); TIntH UsedIdH;
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        int Id; TStr Val; Split(EltV[EltN], Id, Val);
        EAssertR(!UsedIdH.IsKey(Id), "Field ID repeated in '" + Str + "'!");
        int TmpOffset = Offset + Id;
        FtrGen->Add(Val, SpV, TmpOffset);
        UsedIdH.AddKey(Id);
    }
    Offset += GetVals();
}

TStr TFtrGenSparseNumeric::GetVal(const int& ValN, const bool& PrefixP) const { 
    const int FtrGenId = ValN / FtrGen->GetVals();
    const int FtrGenValN = ValN % FtrGen->GetVals();
    return TStr::Fmt("SP-%d-%d-%s", FtrGenId, 
        FtrGenValN, FtrGen->GetVal(FtrGenValN));
}

///////////////////////////////////////
// Multi-Feature-Generator
bool TFtrGenMultiNom::IsReg = TFtrGenMultiNom::MkReg();

void TFtrGenMultiNom::Update(const TStr& Str) { 
    TStrV EltV; Str.SplitOnAllCh(';', EltV);
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        FtrGen->Update(EltV[EltN]);
    }
}

void TFtrGenMultiNom::Add(const TStr& Str, TIntFltKdV& SpV, int& Offset) const {
    TStrV EltV; Str.SplitOnAllCh(';', EltV);
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        int TmpOffset = Offset;
        FtrGen->Add(EltV[EltN], SpV, TmpOffset);
    }
    Offset += GetVals();
}

TStr TFtrGenMultiNom::GetVal(const int& ValN, const bool& PrefixP) const { 
    return PrefixP ? 
		TStr::Fmt("MULTI-%d-%s", ValN, FtrGen->GetVal(ValN).CStr()) :
		FtrGen->GetVal(ValN, PrefixP);
}

void TFtrGenMultiNom::GetValV(const TStr& Str, TStrV& ValV) const {
    TStrV EltV; Str.SplitOnAllCh(';', EltV); ValV.Clr();
    for (int EltN = 0; EltN < EltV.Len(); EltN++) {
        const TStr& Val = EltV[EltN];
        TStrV SubValV; FtrGen->GetValV(Val, SubValV);
        ValV.AddV(SubValV);
    }
}

///////////////////////////////////////
// Feature-Generator-Base
TFtrGenBs::TFtrGenBs(TSIn& SIn) {
	State = TFtrGenState(TInt(SIn).Val);
	FtrGenV.Load(SIn);
	ClsFtrGen = PFtrGen(SIn);
}

void TFtrGenBs::Save(TSOut& SOut) const {
	TInt(State).Save(SOut);
	FtrGenV.Save(SOut);
	ClsFtrGen.Save(SOut);
}

int TFtrGenBs::AddFtrGen(const PFtrGen& FtrGen) { 
	EAssert(State == fgsInit);
	return FtrGenV.Add(FtrGen);
}

void TFtrGenBs::PutClsFtrGen(const PFtrGen& FtrGen) {
	EAssert(State == fgsInit); 
	ClsFtrGen = FtrGen; 
}

void TFtrGenBs::Update(const TStrV& FtrValV) {
	EAssert(State == fgsUpdate);
	EAssert(FtrValV.Len() == FtrGenV.Len());
	try {
		for (int FtrValN = 0; FtrValN < FtrValV.Len(); FtrValN++) {
			FtrGenV[FtrValN]->Update(FtrValV[FtrValN]);
		}
	} catch (PExcept Ex) {
		TExcept::Throw(TStr::Fmt("Error feature generation: '%s'!", Ex->GetMsgStr().CStr()));
	}
}

void TFtrGenBs::UpdateCls(const TStr& ClsFtrVal) {
	EAssert(State == fgsUpdate);
	EAssert(!ClsFtrGen.Empty());
	try {
		ClsFtrGen->Update(ClsFtrVal);
	} catch (PExcept Ex) {
		TExcept::Throw(TStr::Fmt("Error feature generation: '%s'!", Ex->GetMsgStr().CStr()));
	}
}

void TFtrGenBs::GenFtrV(const TStrV& FtrValV, TIntFltKdV& FtrSpV) const {
	EAssert(State == fgsGen);
	EAssert(FtrValV.Len() == FtrGenV.Len());
    try {
		int Offset = 0;
        for (int FtrValN = 0; FtrValN < FtrValV.Len(); FtrValN++) {
			const PFtrGen& FtrGen = FtrGenV[FtrValN];
			const TStr& FtrVal = FtrValV[FtrValN];
            FtrGen->Add(FtrVal, FtrSpV, Offset);
        }
    } catch (PExcept Ex) {
		TExcept::Throw(TStr::Fmt("Error feature generation: '%s'!", Ex->GetMsgStr().CStr()));
    }
}

void TFtrGenBs::GenClsV(const TStr& ClsFtrVal, TStrV& ClsV) const {
	EAssert(State == fgsGen);
	EAssert(!ClsFtrGen.Empty());
    try {
        ClsFtrGen->GetValV(ClsFtrVal, ClsV);
    } catch (PExcept Ex) {
		TExcept::Throw(TStr::Fmt("Error feature generation: '%s'!", Ex->GetMsgStr().CStr()));
    }
}

PBowDocBs TFtrGenBs::MakeBowDocBs() const { 
	EAssert(State == fgsGen);
	PBowDocBs BowDocBs = TBowDocBs::New();
    int WdsOffset = 0;
	for (int FtrGenN = 0; FtrGenN < FtrGenV.Len(); FtrGenN++) {
        const PFtrGen& FtrGen = FtrGenV[FtrGenN];
		TStr Prefix = TInt::GetStr(FtrGenN);
        FtrGen->AddWds(Prefix, BowDocBs, WdsOffset);
    }
	return BowDocBs;
}

void TFtrGenBs::AddBowDoc(const PBowDocBs& BowDocBs,
		const TStr& DocNm, const TStrV& FtrValV) const {

    TIntFltKdV FtrSpV; GenFtrV(FtrValV, FtrSpV);
    // make KdV to PrV
    const int WIds = FtrSpV.Len(); TIntFltPrV WIdWgtPrV(WIds, 0);
    for (int WIdN = 0; WIdN < WIds; WIdN++) {
        WIdWgtPrV.Add(TIntFltPr(FtrSpV[WIdN].Key, FtrSpV[WIdN].Dat));
    }
    // add the feature vector to trainsets
    BowDocBs->AddDoc(DocNm, TStrV(), WIdWgtPrV);
}

void TFtrGenBs::AddBowDoc(const PBowDocBs& BowDocBs, const TStr& DocNm,
		const TStrV& FtrValV, const TStr& ClsFtrVal) const {

    TIntFltKdV FtrSpV; GenFtrV(FtrValV, FtrSpV);
	TStrV CatNmV; GenClsV(ClsFtrVal, CatNmV);
    // make KdV to PrV
    const int WIds = FtrSpV.Len(); TIntFltPrV WIdWgtPrV(WIds, 0);
    for (int WIdN = 0; WIdN < WIds; WIdN++) {
        WIdWgtPrV.Add(TIntFltPr(FtrSpV[WIdN].Key, FtrSpV[WIdN].Dat));
    }
    // add the feature vector to trainsets
	if (!WIdWgtPrV.Empty()) { BowDocBs->AddDoc(DocNm, CatNmV, WIdWgtPrV); }
}

void TFtrGenBs::GetAllCatNmV(TStrV& CatNmV) const {
	const int Vals = ClsFtrGen->GetVals();
	for (int ValN = 0; ValN < Vals; ValN++) {
		CatNmV.Add(ClsFtrGen->GetVal(ValN, false));
	}
}

PBowDocBs TFtrGenBs::LoadCsv(TStr& FNm, const int& ClassId, 
        const TIntV& IgnoreIdV, const int& TrainLen) {

    // feature generators
	PFtrGenBs FtrGenBs = TFtrGenBs::New();
    // CSV parsing stuff
    PSIn SIn = TFIn::New(FNm); 
    char SsCh = ' '; TStrV FldValV;
    // read the headers and initialise the feature generators
    TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);  
    for (int FldValN = 0; FldValN < FldValV.Len(); FldValN++) {
        const TStr& FldVal = FldValV[FldValN];
        if (FldValN == ClassId) { 
            if (FldVal == "NOM") {
                FtrGenBs->PutClsFtrGen(TFtrGenNominal::New());
            } else if (FldVal == "MULTI-NOM") {
                FtrGenBs->PutClsFtrGen(TFtrGenMultiNom::New());
            } else {
                TExcept::Throw("Wrong class type '" + FldVal + "', should be NOM or MULTI-NOM!");
            }
        } else if (!IgnoreIdV.IsIn(FldValN)) {
            if (FldVal == TFtrGenNumeric::GetType()) {
				FtrGenBs->AddFtrGen(TFtrGenNumeric::New());
            } else if (FldVal == TFtrGenNominal::GetType()) { 
				FtrGenBs->AddFtrGen(TFtrGenNominal::New());
            } else if (FldVal == TFtrGenToken::GetType()) { 
				FtrGenBs->AddFtrGen(TFtrGenToken::New(
                    TSwSet::New(swstNone), TStemmer::New(stmtNone)));
            } else if (FldVal == TFtrGenSparseNumeric::GetType()) { 
				FtrGenBs->AddFtrGen(TFtrGenSparseNumeric::New());
            } else if (FldVal == TFtrGenMultiNom::GetType()) { 
				FtrGenBs->AddFtrGen(TFtrGenMultiNom::New());
            } else {
                TExcept::Throw("Wrong type '" + FldVal + "'!");
            }
        }
    }
    const int Flds = FldValV.Len();
    // read the lines and feed them to the feature generators
    int Recs = 0;
    while (!SIn->Eof()) {
        if (Recs == TrainLen) { break; }
        Recs++; printf("%7d\r", Recs);
        TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);
        // make sure line still has the same number of fields as the header
        EAssertR(FldValV.Len() == Flds, 
            TStr::Fmt("Wrong number of fields in line %d! Found %d and expected %d!",
            Recs + 1, FldValV.Len(), Flds));
        // go over lines
        try {
			TStrV FtrValV;
            for (int FldValN = 0; FldValN < FldValV.Len(); FldValN++) {
                const TStr& FldVal = FldValV[FldValN];
                if (FldValN == ClassId) { 
					FtrGenBs->UpdateCls(FldVal);
                } else if (!IgnoreIdV.IsIn(FldValN)) {
                    FtrValV.Add(FldVal);
                }
            }
			FtrGenBs->Update(FtrValV);
        } catch (PExcept Ex) {
            TExcept::Throw(TStr::Fmt("Error in line %d: '%s'!", 
                Recs+1, Ex->GetMsgStr().CStr()));
        }
    }
    // read the file again and feed it to the training set
    PBowDocBs BowDocBs = FtrGenBs->MakeBowDocBs();
    // we read and ignore the headers since we parsed them already 
    SIn = TFIn::New(FNm); SsCh = ' ';
    TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);  
    // read the lines and feed them to the training set
    Recs = 0;
    while (!SIn->Eof()){
        Recs++; printf("%7d\r", Recs);
        TSs::LoadTxtFldV(ssfCommaSep, SIn, SsCh, FldValV, false);
        // make sure line still has the same number of fields as the header
        EAssertR(FldValV.Len() == Flds, 
            TStr::Fmt("Wrong number of fields in line %s! Found %d and expected %d!",
            Recs + 1, FldValV.Len(), Flds));
        // go over lines and construct the sparse vector
		TStrV FtrValV; TStr ClsFtrVal;
        try {
            for (int FldValN = 0; FldValN < FldValV.Len(); FldValN++) {
                const TStr& FldVal = FldValV[FldValN];
                if (FldValN == ClassId) { 
                    ClsFtrVal = FldVal;
                } else if (!IgnoreIdV.IsIn(FldValN)) {
                    FtrValV.Add(FldVal);
                }
            }
        } catch (PExcept Ex) {
            TExcept::Throw(TStr::Fmt("Error in line %d: '%s'!", 
                Recs+1, Ex->GetMsgStr().CStr()));
        }
        // add the feature vector to trainsets
		FtrGenBs->AddBowDoc(BowDocBs, TStr::Fmt("Line-%d", Recs), FtrValV, ClsFtrVal);
    }
	// prepare training and testing doc ids
	TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV); IAssert(AllDIdV.IsSorted());
	TIntV TrainDIdV = AllDIdV; TrainDIdV.Trunc(TrainLen);
	BowDocBs->PutTrainDIdV(TrainDIdV);
	TIntV TestDIdV = AllDIdV; TestDIdV.Minus(TrainDIdV);
	BowDocBs->PutTestDIdV(TestDIdV);

    return BowDocBs;
}
