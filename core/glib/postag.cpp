#include <postag.h>

////////////////////////////////////////////
// Part-of-Speach Element
TPosTagType TPosElt::GetTypeFromAna(const TStr& AnaType) {
    if (AnaType.Empty()) {  return pttUndef; }
    switch (AnaType[0]) {
        case 'N': return pttNoun;
        case 'V': return pttVerb;
        case 'A': return pttAdjective;
        case 'R': return pttAdverb;
        case 'P': return pttPronoun;
        case 'S': return pttAdposition;
        case 'C': return pttConjunction;
        case 'Q': return pttParticle;
        case 'I': return pttInterjection;
        case 'M': return pttNumeral;
        case 'Y': return pttAbbreviation;
        case 'X': return pttResidual;
        default: return pttUndef;
    }
}

////////////////////////////////////////////
// Part-of-Speach Sentence Base
void TPosSentBs::MakeSplit(const int& Train, const int& Test) {
    if (Train == -1) {
        TestSents = TInt::GetMn(Test, GetSents());
        TrainSents = GetSents() - TestSents;
    } else if (Test == -1) { 
        TrainSents = TInt::GetMn(Train, GetSents());
        TestSents = GetSents() - TrainSents;
    } else {
        TrainSents = TInt::GetMn(Train, GetSents());
        TestSents = TInt::GetMn(Test, GetSents()-TrainSents);
    }
}

int TPosSentBs::CompareSent(const TPosSent& OrgSent, const TPosSent& PredSent) const {
    IAssert(OrgSent.GetElts() == PredSent.GetElts());
    int MatchCount = 0;
    for (int EltN = 2; EltN < (OrgSent.GetElts()-1); EltN++) {
        const TPosTagType OrgTagType = OrgSent.GetElt(EltN).GetPosTagType();
        const TPosTagType PredTagType = PredSent.GetElt(EltN).GetPosTagType();
        if (OrgTagType == PredTagType) { MatchCount++; }
    }
    return MatchCount;
}

int TPosSentBs::CompareSentKnown(const TPosSent& OrgSent, 
        const TPosSent& PredSent, const PPosHmm& Hmm) const {

    IAssert(OrgSent.GetElts() == PredSent.GetElts());
    int MatchCount = 0;
    for (int EltN = 2; EltN < (OrgSent.GetElts()-1); EltN++) {
        if (!Hmm->IsKnown(OrgSent.GetElt(EltN).GetLemmaStr())) { continue; }
        const TPosTagType OrgTagType = OrgSent.GetElt(EltN).GetPosTagType();
        const TPosTagType PredTagType = PredSent.GetElt(EltN).GetPosTagType();
        if (OrgTagType == PredTagType) { MatchCount++; }
    }
    return MatchCount;
}

int TPosSentBs::CompareSentUnknown(const TPosSent& OrgSent, 
        const TPosSent& PredSent, const PPosHmm& Hmm) const {

    IAssert(OrgSent.GetElts() == PredSent.GetElts());
    int MatchCount = 0;
    for (int EltN = 2; EltN < (OrgSent.GetElts()-1); EltN++) {
        if (Hmm->IsKnown(OrgSent.GetElt(EltN).GetLemmaStr())) { continue; }
        const TPosTagType OrgTagType = OrgSent.GetElt(EltN).GetPosTagType();
        const TPosTagType PredTagType = PredSent.GetElt(EltN).GetPosTagType();
        if (OrgTagType == PredTagType) { MatchCount++; }
    }
    return MatchCount;
}

PPosSentBs TPosSentBs::LoadMte(const TStr& XmlFNm) {
    // prepare sentence base
    PPosSentBs PosSentBs = TPosSentBs::New();
    // load and parse the XML (small enough so no bother with memory)
    printf("Loading XML ... ");
    PSIn XmlSIn = TFIn::New(XmlFNm);
    PXmlDoc XmlDoc = TXmlDoc::LoadTxt(XmlSIn);
    PXmlTok TopTok = XmlDoc->GetTok();
    printf("done\n");
    // iterate over parts
    TXmlTokV PartTokV; TopTok->GetTagTokV("text|body|div", PartTokV);
    printf("Loading %d parts ... ", PartTokV.Len());
    int AllSubParts = 0, AllParas = 0, AllSents = 0, AllWords = 0;
    // iterate over parts
    for (int PartN = 0; PartN < PartTokV.Len(); PartN++) {
        PXmlTok PartTok = PartTokV[PartN];
        const int SubParts = PartTok->GetSubToks(); AllSubParts += SubParts;
        // iterate over chapters
        for (int SubPartN = 0; SubPartN < SubParts; SubPartN++) {
            PXmlTok SubPartTok = PartTok->GetSubTok(SubPartN);
            if (SubPartTok->GetStrArgVal("type", "") == "appendix") { continue; }
            const int Paras = SubPartTok->GetSubToks();
            // iterate over paragraphs
            for (int ParaN = 0; ParaN < Paras; ParaN++) {
                PXmlTok ParaTok = SubPartTok->GetSubTok(ParaN);
                if (!ParaTok->IsTag()) { continue; } AllParas++;
                const int Sents = ParaTok->GetSubToks();
                // iterate over sentences
                for (int SentN = 0; SentN < Sents; SentN++) {
                    PXmlTok SentTok = ParaTok->GetSubTok(SentN);
                    if (!SentTok->IsTag()) { continue; } AllSents++;
                    const int Words = SentTok->GetSubToks();
                    // iterate over words and add them to the sentence vector
                    TPosSent PosSent;
                    PosSent.AddElt(TPosElt(pttBegining)); // t_-1
                    PosSent.AddElt(TPosElt(pttBegining)); // t_0
                    for (int WordN = 0; WordN < Words; WordN++) {
                        PXmlTok WordTok = SentTok->GetSubTok(WordN);
                        if (!WordTok->IsTag()) { continue; } AllWords++;
                        if (WordTok->GetTagNm() == "w") {
                            TStr WordStr = WordTok->GetTokStr(false);
                            TStr LemmaStr = WordTok->GetStrArgVal("lemma", "");
                            TStr AnaType = WordTok->GetStrArgVal("ana", "");
                            const TPosTagType PosTagType = TPosElt::GetTypeFromAna(AnaType);
                            PosSent.AddElt(TPosElt(WordStr, LemmaStr, PosTagType));
                        //} else if (WordTok->GetTagNm() == "c") {
                            //TStr WordStr = WordTok->GetTokStr(false);
                            //PosSent.AddElt(TPosElt(WordStr, pttPunctuation));
                        }
                    }
                    PosSent.AddElt(TPosElt(pttEnd)); // t_T+1
                    if (PosSent.GetElts() > 3) { PosSentBs->AddSent(PosSent); }
                }
            }
        }
    }
    printf("Done (Chapters:%d, Paragraphs:%d, Sentences:%d, Words:%d)\n",
        AllSubParts, AllParas, AllSents, AllWords);
    // finish
    return PosSentBs;
}

////////////////////////////////////////////
// Part-of-Speach Hidden-Markov-Model
void TPosHmm::LearnProb(const PPosSentBs& PosSentBs, int& Tokens, 
        TIntPrIntH& BigramCountH, TIntTrIntH& TrigramCountH,
        TIntPrIntH& LexicalCountH) {

    // clear previous states and words
    LemmaH.Clr(); StateH.Clr();
    // load counts of words, bigrams and trigrams
    Tokens = 0; BigramCountH.Clr();
    TrigramCountH.Clr(); LexicalCountH.Clr(); 
    const int Sents = PosSentBs->GetTrainSents();
    printf("Counting %d sentences ... ", Sents);
    for (int SentN = 0; SentN < Sents; SentN++) {
        const TPosSent& Sent = PosSentBs->GetTrainSent(SentN);
        const int Elts = Sent.GetElts();
        int OldState = -1, OldOldState = -1;
        for (int EltN = 0; EltN < Elts; EltN++) {
            const TPosElt& Elt = Sent.GetElt(EltN);           
            // remember the word
            const int LemmaId = LemmaH.AddKey(Elt.GetLemmaStr());
            LemmaH[LemmaId]++; // count the word
            // remember the state
            const int State = int(Elt.GetPosTagType());
            StateH.AddDat(State)++;
            // count
            Tokens++;
            if (EltN > 0) { 
                IAssert(OldState != -1);
                BigramCountH.AddDat(TIntPr(OldState, State))++;
            }
            if (EltN > 1) {
                IAssert(OldState != -1 && OldOldState != -1);
                TrigramCountH.AddDat(TIntTr(OldOldState, OldState, State))++;
            }
            LexicalCountH.AddDat(TIntPr(LemmaId, State))++;
            // go to next state
            OldOldState = OldState;
            OldState = State;
        }
    }
    printf("Done (Lemmas:%d States:%d, Bigrams:%d, Trigrams:%d, Lexical:%d)\n", LemmaH.Len(), 
        StateH.Len(), BigramCountH.Len(), TrigramCountH.Len(), LexicalCountH.Len());
    // clear previous probs
    StateProbH.Clr(); BigramProbH.Clr();
    TrigramProbH.Clr(); LexicalProbH.Clr();
    // calculate probabilities
    printf("Calculating probabilities ... ");
    {printf("States ");
    IAssert(Tokens > 0);
    int KeyId = StateH.FFirstKeyId(); double ProbSum = 0.0; 
    while (StateH.FNextKeyId(KeyId)) {
        const int State = StateH.GetKey(KeyId);
        const int StateCount = StateH[KeyId];        
        const double Prob = double(StateCount) / double(Tokens);
        StateProbH.AddDat(State, Prob);
        ProbSum += Prob;
    }printf("(%g) ... ", ProbSum);}
    {printf("Bigrams ");
    int KeyId = BigramCountH.FFirstKeyId(); double ProbSum = 0.0;
    while (BigramCountH.FNextKeyId(KeyId)) {
        const TIntPr& Bigram = BigramCountH.GetKey(KeyId);
        const int BigramCount = BigramCountH[KeyId];
        const int OldStateCount = StateH.GetDat(Bigram.Val1); 
        IAssert(OldStateCount > 0);        
        const double Prob = double(BigramCount) / double (OldStateCount);
        BigramProbH.AddDat(Bigram, Prob);
        ProbSum += Prob;
    }printf("(%g) ... ", ProbSum);}
    {printf("Trigrams ");
    int KeyId = TrigramCountH.FFirstKeyId(); double ProbSum = 0.0;
    while (TrigramCountH.FNextKeyId(KeyId)) {
        const TIntTr& Trigram = TrigramCountH.GetKey(KeyId);
        const int TrigramCount = TrigramCountH[KeyId];
        TIntPr OldBigram(Trigram.Val1, Trigram.Val2);
        const int OldBigramCount = BigramCountH.GetDat(OldBigram);
        IAssert(OldBigramCount > 0);
        const double Prob = double(TrigramCount) / double(OldBigramCount);
        TrigramProbH.AddDat(Trigram, Prob);
        ProbSum += Prob;
    }printf("(%g) ... ", ProbSum);}
    {printf("Lexical ");
    int KeyId = LexicalCountH.FFirstKeyId(); double ProbSum = 0.0;
    while (LexicalCountH.FNextKeyId(KeyId)) {
        const TIntPr& Lexical = LexicalCountH.GetKey(KeyId);
        const int LexicalCount = LexicalCountH[KeyId];
        const int StateCount = StateH.GetDat(Lexical.Val2); 
        IAssert(StateCount > 0);
        const double Prob = double(LexicalCount) / double(StateCount);
        LexicalProbH.AddDat(Lexical, Prob);
        ProbSum += Prob;
    } printf("(%g) ", ProbSum);}
    printf("Done\n");
}

void TPosHmm::LearnLambdas(const int& Tokens, const TIntPrIntH& BigramCountH, 
        const TIntTrIntH& TrigramCountH, const TIntPrIntH& LexicalCountH) {

    printf("Learning lambdas ... ");
    double Lambda1 = 0.0, Lambda2 = 0.0, Lambda3 = 0.0;
    int KeyId = TrigramProbH.FFirstKeyId(); IAssert(Tokens > 1);
    while (TrigramProbH.FNextKeyId(KeyId)) {
        // get counts
        const TIntTr& Trigram = TrigramProbH.GetKey(KeyId);
        const int TrigramCount = TrigramCountH[KeyId];
        TIntPr OldBigram(Trigram.Val1, Trigram.Val2);
        const int OldBigramCount = BigramCountH.GetDat(OldBigram);
        TIntPr Bigram(Trigram.Val2, Trigram.Val3);
        const int BigramCount = BigramCountH.GetDat(Bigram);
        const int OldStateCount = StateH.GetDat(Trigram.Val2); 
        const int StateCount = StateH.GetDat(Trigram.Val3); 
        // probs
        const double Val3 = OldBigramCount > 1 ? 
            double(TrigramCount - 1) / double(OldBigramCount - 1) : 0.0;
        const double Val2 = OldStateCount > 1 ?
            double(BigramCount - 1) / double (OldStateCount - 1) : 0.0;
        const double Val1 = double(StateCount - 1) / double(Tokens - 1);
        // get max
        if (Val3 > Val1 && Val3 > Val2) { Lambda3 += TrigramCount; }
        else if (Val2 > Val1 && Val2 > Val3) { Lambda2 += TrigramCount; }
        else if (Val1 > Val2 && Val1 > Val3) { Lambda1 += TrigramCount; }
    } 
    LambdaV = TFltV::GetV(Lambda1, Lambda2, Lambda3);
    TLinAlg::NormalizeL1(LambdaV);
    printf("Done (L1:%.6f, L2:%.6f, L3:%.6f)\n", 
        LambdaV[0].Val, LambdaV[1].Val, LambdaV[2].Val);
}

void TPosHmm::LearnUnknown(const PPosSentBs& PosSentBs, 
        const int& MxLemmaFq, const int& MxSuffixLen) {

    // load counts of words, bigrams and trigrams
    SuffixH.Clr(); TIntPrIntH StateSuffixCountH;
    const int Sents = PosSentBs->GetTrainSents();
    int AllSuffixFq = 0; TIntH StateCountH;
    printf("Counting %d sentences ... ", Sents);
    for (int SentN = 0; SentN < Sents; SentN++) {
        const TPosSent& Sent = PosSentBs->GetTrainSent(SentN);
        const int Elts = Sent.GetElts();
        for (int EltN = 0; EltN < Elts; EltN++) {
            const TPosElt& Elt = Sent.GetElt(EltN);           
            TStr Lemma = Elt.GetLemmaStr();
            // if word to frequent we ignore it
            if (LemmaH.GetDat(Lemma) > MxLemmaFq) { continue; }
            // load suffixes of this word
            const int MxChs = TInt::GetMn(Lemma.Len(), MxSuffixLen);
            for (int Chs = 1; Chs <= MxChs; Chs++) {
                // count suffix
                AllSuffixFq++;
                // get the state
                const int State = int(Elt.GetPosTagType());
                StateCountH.AddDat(State)++;
                // get the suffix
                TStr Suffix = Lemma.Right(Chs);
                const int SuffixId = SuffixH.AddKey(Suffix);
                SuffixH[SuffixId]++; 
                // count (State,Suffix) pair
                TIntPr StateSuffix(State, SuffixId);
                StateSuffixCountH.AddDat(StateSuffix)++;
            }
        }
    }
    // clear previous suffix probabilities
    SuffixStateProbH.Clr();
    // calcualte probability of suffix given state
    printf("Probabilities (Suffix:%d) ... ", SuffixH.Len());
    int KeyId = StateSuffixCountH.FFirstKeyId(); 
    double ProbSum = 0.0, InvProbSum = 0.0;
    while (StateSuffixCountH.FNextKeyId(KeyId)) {
        const TIntPr& StateSuffix = StateSuffixCountH.GetKey(KeyId);
        const int StateSuffixCount = StateSuffixCountH[KeyId];
        const int SuffixCount = SuffixH[StateSuffix.Val2];
        const double Prob = double(StateSuffixCount) / double(SuffixCount);
        const double SuffixProb = double(SuffixCount) / double(AllSuffixFq);
        const int StateCount = StateCountH.GetDat(StateSuffix.Val1);
        const double StateProb = double(StateCount) / double(AllSuffixFq);
        const double InvProb = Prob * SuffixProb / StateProb;
        TIntPr SuffixState(StateSuffix.Val2, StateSuffix.Val1);
        SuffixStateProbH.AddDat(SuffixState, InvProb);
        ProbSum += Prob; InvProbSum += InvProb;
    }
    printf("Done (Prob:%g, Inv:%g)\n", ProbSum, InvProbSum);
    // learn theta weights
    printf("Learning smoothing weights ... ");
    // get most probable overall state
    int MxProbState = -1; double MxProbStateWgt = 0.0;
    {double MxProbStateProb = 0.0, MxProbStateSumProb = 0.0;
    int StateKeyId = StateH.FFirstKeyId();
    while (StateH.FNextKeyId(StateKeyId)) { 
        const int CurrState = StateH.GetKey(StateKeyId);
        const double Prob = StateProbH.GetDat(CurrState);
        MxProbStateSumProb += Prob;
        if (Prob > MxProbStateProb) { 
            MxProbState = CurrState; MxProbStateProb = Prob; 
        }
    } MxProbStateWgt = MxProbStateProb / MxProbStateSumProb; }
    // count words and see which thetas are the best for them
    ThetaV.Gen(MxSuffixLen+1); ThetaV.PutAll(0.0);
    for (int SentN = 0; SentN < Sents; SentN++) {
        const TPosSent& Sent = PosSentBs->GetTrainSent(SentN);
        const int Elts = Sent.GetElts();
        for (int EltN = 0; EltN < Elts; EltN++) {
            const TPosElt& Elt = Sent.GetElt(EltN);           
            const int EltState = int(Elt.GetPosTagType());
            TStr Lemma = Elt.GetLemmaStr();
            // if word to frequent we ignore it
            if (LemmaH.GetDat(Lemma) > MxLemmaFq) { continue; }
            // update base theta
            if (EltState == MxProbState) { ThetaV[0] += MxProbStateWgt; }
            // load suffixes of this word
            const int MxChs = TInt::GetMn(Lemma.Len(), MxSuffixLen);
            for (int Chs = 1; Chs <= MxChs; Chs++) {
                // get the state and suffix
                const int State = int(Elt.GetPosTagType());
                const int SuffixId = SuffixH.GetKeyId(Lemma.Right(Chs));
                // get most probable state
                int MxState = -1; double MxProb = 0.0, SumProb = 0.0;
                {int StateKeyId = StateH.FFirstKeyId();
                while (StateH.FNextKeyId(StateKeyId)) { 
                    const int CurrState = StateH.GetKey(StateKeyId);
                    TIntPr SuffixState(SuffixId, CurrState);
                    if (!SuffixStateProbH.IsKey(SuffixState)) { continue; }
                    const double Prob = SuffixStateProbH.GetDat(SuffixState);
                    SumProb += Prob;
                    if (Prob > MxProb) { MxState = CurrState; MxProb = Prob; }
                }}
                // update weight if good
                if (MxState == State) { ThetaV[Chs] += double(Chs) * MxProb / SumProb; }
            }
        }
    }
    TLinAlg::NormalizeL1(ThetaV);
    printf("Done\n");
    TLAMisc::PrintTFltV(ThetaV, "Theta");
}

double TPosHmm::GetTrigramProb(const int& OldOldState, 
        const int& OldState, const int& State) const {

    // get learned probabilities
    TIntPr Bigram(OldState, State);
    TIntTr Trigram(OldOldState, OldState, State);
    const double UnigramProb = StateProbH.IsKey(State) ? 
        StateProbH.GetDat(State) : 0.0;
    const double BigramProb = BigramProbH.IsKey(Bigram) ? 
        BigramProbH.GetDat(Bigram) : 0.0;
    const double TrigramProb = TrigramProbH.IsKey(Trigram) ? 
        TrigramProbH.GetDat(Trigram) : 0.0; 
    // smooth them and return results
    return LambdaV[0] * UnigramProb + LambdaV[1] * BigramProb + LambdaV[2] * TrigramProb;
}

double TPosHmm::GetLexicalProb(const TStr& Lemma, const int& State) const {
    if (LemmaH.IsKey(Lemma)) {
        // known word
        const int LemmaId = LemmaH.GetKeyId(Lemma);
        TIntPr Lexical(LemmaId, State);
        return LexicalProbH.IsKey(Lexical) ? LexicalProbH.GetDat(Lexical) : 0.0;
    } else {
        // unknown word
        double Prob = ThetaV[0] * StateProbH.GetDat(State);
        const int MxChs = Lemma.Len(); int Chs;
        for (Chs = 1; Chs <= MxChs; Chs++) {
            TStr Suffix = Lemma.Right(Chs);
            if (!SuffixH.IsKey(Suffix)) { break; }
            const int SuffixId = SuffixH.GetKeyId(Suffix);
            TIntPr SuffixState(SuffixId, State);
            if (!SuffixStateProbH.IsKey(SuffixState)) { break; }
            const double SuffixProb = SuffixStateProbH.GetDat(SuffixState);
            Prob += ThetaV[Chs] * SuffixProb;
        }
        return Prob;
    }
    return 0.0;
}

TPosHmm::TPosHmm(const PPosSentBs& PosSentBs) {
    int Tokens; TIntPrIntH BigramCountH; 
    TIntTrIntH TrigramCountH; TIntPrIntH LexicalCountH;
    // learn probabilities
    LearnProb(PosSentBs, Tokens, BigramCountH, TrigramCountH, LexicalCountH);
    // learn smoothing parameters
    LearnLambdas(Tokens, BigramCountH, TrigramCountH, LexicalCountH);
    // learn suffix probabilities for unknown words
    LearnUnknown(PosSentBs, 10, 10);
}

void TPosHmm::Tag(TPosSent& Sent) {
    // if no content, we return
    if (Sent.GetElts() < 4) { return; }
    // basic counts and states
    const int States = StateH.Len();
    const int BeginState = int(pttBegining), EndState = int(pttEnd);
    const int BeginStateId = StateH.GetKeyId(BeginState);
    const int EndStateId = StateH.GetKeyId(EndState);
    const int Elts = Sent.GetElts();
    // initialization
    TFltVV MxProbVV(Elts, States); MxProbVV.PutAll(0.0);
    TIntVV BackVV(Elts, States); BackVV.PutAll(-1);
    { // initialization of probabilities for the first token
        const int FirstEltN = 2;
        TStr Lemma = Sent.GetElt(FirstEltN).GetLemmaStr();
        for (int StateId = 0; StateId < States; StateId++) {
            const int State = StateH.GetKey(StateId);
            const double TrigramProb = GetTrigramProb(BeginState, BeginState, State);
            const double LexicalProb = GetLexicalProb(Lemma, State);
            const double Prob = TrigramProb * LexicalProb;
            MxProbVV(FirstEltN, StateId) = Prob;
            BackVV(FirstEltN, StateId) = BeginStateId;
            // initialize first two elements
            BackVV(0, StateId) = BackVV(1, StateId) = -1;
        }
        BackVV(1, BeginStateId) = BeginStateId;
    }
    // recursion
    for (int EltN = 3; EltN < (Elts-1); EltN++) {
        TStr Lemma = Sent.GetElt(EltN).GetLemmaStr();
        bool PositiveMoveP = false;
        for (int StateId = 0; StateId < States; StateId++) {
            const int State = StateH.GetKey(StateId);
            // find maximal prob value for this state
            int MxOldStateId = -1; double MxProb = 0.0;
            for (int OldStateId = 0; OldStateId < States; OldStateId++) {
                // get two previous best states
                const int OldState = StateH.GetKey(OldStateId);
                const int OldOldStateId = BackVV(EltN-1, OldStateId);
                if (OldOldStateId == -1) { continue; }
                const int OldOldState = StateH.GetKey(OldOldStateId);
                // get previous max probability
                const double OldProb = MxProbVV(EltN-1, OldStateId);
                // calculate current probability
                const double TrigramProb = GetTrigramProb(OldOldState, OldState, State);
                const double LexicalProb = GetLexicalProb(Lemma, State);
                const double Prob = OldProb * TrigramProb * LexicalProb;
                if (Prob > MxProb) { MxProb = Prob; MxOldStateId = OldStateId; }
            }
            // remember max probability and how to backtrack
            MxProbVV(EltN, StateId) = MxProb;
            BackVV(EltN, StateId) = MxOldStateId;
            PositiveMoveP = PositiveMoveP || (MxOldStateId != -1);
        }
        IAssert(PositiveMoveP);
    }
    // transitions to last element with predefined state (pttEnd)
    const int LastEltN = Elts-1;
    int MxOldStateId = -1; double MxProb = 0.0;
    for (int OldStateId = 0; OldStateId < States; OldStateId++) {
        // get two previous best states
        const int OldState = StateH.GetKey(OldStateId);
        const int OldOldStateId = BackVV(LastEltN-1, OldStateId);
        if (OldOldStateId == -1) { continue; }
        const int OldOldState = StateH.GetKey(OldOldStateId);
        // get previous max probability
        const double OldProb = MxProbVV(LastEltN-1, OldStateId);
        // calculate current probability
        const double TrigramProb = GetTrigramProb(OldOldState, OldState, EndState);
        const double Prob = OldProb * TrigramProb;
        if (Prob > MxProb) { MxProb = Prob; MxOldStateId = OldStateId; }
    } IAssert(MxOldStateId != -1);
    // backtrack
    Sent.GetElt(LastEltN).PutPosTagType(pttEnd); // tag last token
    for (int EltN = Elts-2; EltN >= 0; EltN--) {
        const int MxOldState = StateH.GetKey(MxOldStateId);
        const TPosTagType MxTagType = TPosTagType(MxOldState);
        Sent.GetElt(EltN).PutPosTagType(MxTagType);
        MxOldStateId = BackVV(EltN, MxOldStateId);
        IAssert((MxOldStateId != -1) || (EltN == 0));
    }
}

int TPosHmm::GetKnowns(const TPosSent& Sent) const {
    int Knowns = 0;
    for (int EltN = 2; EltN < (Sent.GetElts()-1); EltN++) {
        if (IsKnown(Sent.GetElt(EltN).GetLemmaStr())) { Knowns++; }
    }
    return Knowns;
}
