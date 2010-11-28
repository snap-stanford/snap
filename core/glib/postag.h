#ifndef POSTAG_H
#define POSTAG_H

#include <mine.h>

////////////////////////////////////////////
// Forward-Declarations
ClassHdTP(TPosHmm, PPosHmm);

////////////////////////////////////////////
// Part-of-Speach Tag-Types
typedef enum { pttUndef, pttNoun, pttVerb, pttAdjective, pttAdverb, pttPronoun, 
  pttAdposition, pttConjunction, pttParticle, pttInterjection, pttNumeral, 
  pttAbbreviation, pttResidual, pttBegining, pttEnd, pttPunctuation } TPosTagType;

////////////////////////////////////////////
// Part-of-Speach Element
ClassTV(TPosElt, TPosEltV)//{
private:
    TStr WordStr;
    TStr LemmaStr;
    TPosTagType PosTagType;
public:
    TPosElt() { };
    TPosElt(const TPosTagType& _PosTagType):
      WordStr(""), LemmaStr(""), PosTagType(_PosTagType) { }
    TPosElt(const TStr& _WordStr, const TPosTagType& _PosTagType):
      WordStr(_WordStr), LemmaStr(""), PosTagType(_PosTagType) { }
    TPosElt(const TStr& _WordStr, const TStr& _LemmaStr, const TPosTagType& _PosTagType):
      WordStr(_WordStr), LemmaStr(_LemmaStr), PosTagType(_PosTagType) { }

    TStr GetWordStr() const { return WordStr; }
    TStr GetLemmaStr() const { return LemmaStr; }
    TPosTagType GetPosTagType() const { return PosTagType; }
    void PutPosTagType(const TPosTagType& _PosTagType) { PosTagType = _PosTagType; }

    static TPosTagType GetTypeFromAna(const TStr& AnaType);
};

////////////////////////////////////////////
// Part-of-Speach Sentence
ClassTV(TPosSent, TPosSentV)//{
private:
    TPosEltV PosEltV;
public:
    TPosSent() { };

    int AddElt(const TPosElt& PosElt) { return PosEltV.Add(PosElt); }
    int GetElts() const { return PosEltV.Len(); }
    const TPosElt& GetElt(const int& EltN) const { return PosEltV[EltN]; }
    TPosElt& GetElt(const int& EltN) { return PosEltV[EltN]; }
};

////////////////////////////////////////////
// Part-of-Speach Sentence Base
ClassTP(TPosSentBs, PPosSentBs)//{
private:
    TPosSentV PosSentV;
    TInt TrainSents;
    TInt TestSents;
public:
    TPosSentBs() { };
    static PPosSentBs New() { return new TPosSentBs; }

    int AddSent(const TPosSent& PosSent) { return PosSentV.Add(PosSent); }
    int GetSents() const { return PosSentV.Len(); }
    const TPosSent& GetSent(const int& SentN) const { return PosSentV[SentN]; }

    void MakeSplit(const int& Train, const int& Test);
    int GetTrainSents() const { return TrainSents; }
    const TPosSent& GetTrainSent(const int& SentN) const { return PosSentV[SentN]; }
    int GetTestSents() const { return TestSents; }
    const TPosSent& GetTestSent(const int& SentN) const { 
        return PosSentV[PosSentV.Len() - TestSents + SentN]; }

    int CompareSent(const TPosSent& OrgSent, const TPosSent& PredSent) const;
    int CompareSentKnown(const TPosSent& OrgSent, const TPosSent& PredSent, const PPosHmm& Hmm) const;
    int CompareSentUnknown(const TPosSent& OrgSent, const TPosSent& PredSent, const PPosHmm& Hmm) const;

    static PPosSentBs LoadMte(const TStr& XmlFNm);
};

////////////////////////////////////////////
// Part-of-Speach Hidden-Markov-Model
ClassTP(TPosHmm, PPosHmm)//{
private:
    TStrH LemmaH; // all the lemmas
    TIntH StateH; // all the states
    TIntFltH StateProbH;     // eq. (2)
    TIntPrFltH BigramProbH;  // eq. (3)
    TIntTrFltH TrigramProbH; // eq. (4)
    TIntPrFltH LexicalProbH; // eq. (5)
    TFltV LambdaV; // trigram smoothing lambdas
    TStrH SuffixH;
    TIntPrFltH SuffixStateProbH;
    TFltV ThetaV; // suffix smoothing theta

    void LearnProb(const PPosSentBs& PosSentBs, int& Tokens, 
        TIntPrIntH& BigramCountH, TIntTrIntH& TrigramCountH,
        TIntPrIntH& LexicalCountH);
    void LearnLambdas(const int& Tokens, const TIntPrIntH& BigramCountH, 
        const TIntTrIntH& TrigramCountH, const TIntPrIntH& LexicalCountH);
    void LearnUnknown(const PPosSentBs& PosSentBs, 
        const int& MxLemmaFq, const int& MxSuffixLen);

    double GetTrigramProb(const int& OldOldState, 
        const int& OldState, const int& State) const;
    double GetLexicalProb(const TStr& Lemma, const int& State) const;

public:
    TPosHmm(const PPosSentBs& PosSentBs);
    static PPosHmm New(const PPosSentBs& PosSentBs) { return new TPosHmm(PosSentBs); }

    void Tag(TPosSent& Sent);

    bool IsKnown(const TStr& Lemma) const { return LemmaH.IsKey(Lemma); }
    int GetKnowns(const TPosSent& Sent) const;
    int GetUnknowns(const TPosSent& Sent) const {
        return Sent.GetElts() - GetKnowns(Sent); }
};

#endif POSTAG_H
