/////////////////////////////////////////////////
// Stemmer
TStr TStemmer::GetStem(const TStr& WordStr){
  // convert to upper-case
  TStr UCWordStr=WordStr.GetUc();
  // check for synonyms
  TStr SynomymStr;
  if (SynonymStrToWordStr.IsKeyGetDat(UCWordStr, SynomymStr)){
    UCWordStr=SynomymStr;
  }

  // get stem
  TStemmerType _StemmerType=TStemmerType(int(StemmerType));
  TStr StemStr;
  if (_StemmerType==stmtPorter){
    StemStr=TPorterStemmer::StemX(UCWordStr);
  } else {
    return UCWordStr;
  }

  // get real word from stem
  if (bool(RealWordP)){
    TStr RealWordStr;
    if (StemStrToRealWordStrH.IsKeyGetDat(StemStr, RealWordStr)){
      StemStr=RealWordStr;
    } else {
      StemStrToRealWordStrH.AddDat(StemStr, UCWordStr);
      StemStr=UCWordStr;
    }
  }

  // return stem
  return StemStr;
}

void TStemmer::GetStemmerTypeNmV(TStrV& StemmerTypeNmV, TStrV& StemmerTypeDNmV) {
  StemmerTypeNmV.Clr(); StemmerTypeDNmV.Clr();
  StemmerTypeNmV.Add("none"); StemmerTypeDNmV.Add("None");
  StemmerTypeNmV.Add("porter"); StemmerTypeDNmV.Add("English-Porter");
}

TStr TStemmer::GetStemmerTypeNmVStr(){
  TChA ChA;
  ChA+='(';
  ChA+="none"; ChA+=", ";
  ChA+="porter"; ChA+=")";
  return ChA;
}

TStemmerType TStemmer::GetStemmerType(const TStr& StemmerTypeNm){
  TStr LcStemmerTypeNm=StemmerTypeNm.GetLc();
  if (LcStemmerTypeNm=="none"){return stmtNone;}
  else if (LcStemmerTypeNm=="porter"){return stmtPorter;}
  else {return stmtNone;}
}

/////////////////////////////////////////////////
// Porter-Stemmer
char *TPorterStemmer::StemInPlace(char *pWord){
  char* pStart=pWord; char* pEnd=pWord;
  int len=0; while (*pEnd){pEnd++; len++;}
  if (len <= 2) return pWord;

  // Step 1a.
  if (pEnd[-1] == 'S')
  {
    char c2 = pEnd[-2];
    if (c2 == 'E')
    {
      char c3 = pEnd[-3];
      if (len >= 4 && c3 == 'S' && pEnd[-4] == 'S')
        len -= 2, pEnd -= 2; // SSES -> SS
      else if (c3 == 'I')
        len -= 2, pEnd -= 2; // IES -> I
      else
        len--, pEnd--; // S -> eps, as it's not preceded by another 'S' but by an 'E'
    }
    else if (c2 != 'S') // S -> eps, except SS -> SS
      len--, pEnd--;
  }

  // Step 1b.
  if (len >= 3) // the rules here require at least one
    // character in the stem, and at least two in the suffix
  {
    char c1 = pEnd[-1], c2 = pEnd[-2], c3 = pEnd[-3]; bool b = false;
    if (c1 == 'D' && c2 == 'E')
    {
      if (c3 == 'E') 
        { if (MeasureG0(pStart, pEnd - 3)) pEnd--, len--; } // (m > 0) EED -> EE
      else if (HasVowels(pStart, pEnd - 2)) 
        pEnd -= 2, len -= 2, b = true; // (*v*) ED -> eps
    }
    else if (c1 == 'G' && c2 == 'N' && c3 == 'I' && HasVowels(pStart, pEnd - 3))
      pEnd -= 3, len -= 3, b = true; // (*v*) ING -> eps

    // Additional processing for step 1b.  If b is true, we have stripped
    // at least two letters by now, and can safely append an E in the same buffer.
    if (b && len >= 2)
    {
      c1 = pEnd[-1]; c2 = pEnd[-2];
      if ((c1 == 'T' && c2 == 'A') ||
        (c1 == 'L' && c2 == 'B') ||
        (c1 == 'Z' && c2 == 'I'))
        *pEnd++ = 'E', len++; // AT -> ATE, BL -> BLE, IZ -> IZE
      else if (c1 == c2 && c1 != 'L' && c1 != 'S' && c1 != 'Z' && c1 != 'A' &&
        c1 != 'E' && c1 != 'I' && c1 != 'O' && c1 != 'U' && c1 != 'Y')
        pEnd--, len--;
      else // (m == 1 && *o), meaning: [C]VC[V] and at the same time ends in cvc.
        // This means it must be of the form Cvc.  We append E if this succeeds.
        if (IsCvc(pStart, pEnd)) *pEnd++ = 'E', len++;
    }
  }

  // Step 1c.
  if (len >= 2) if (pEnd[-1] == 'Y' && HasVowels(pStart, pEnd - 1))
    pEnd[-1] = 'I'; // (*v*) Y -> I

  // Step 2.  These rules all require m > 0 (meaning a stem of at lest
  // two characters), and suffixes of length three or more.  Thus, the total
  // length has to be at least 5 characters.
  if (len >= 5)
  {
    char c1 = pEnd[-1], c2 = pEnd[-2], c3 = pEnd[-3]; 
    if (c1 == 'L' && c2 == 'A' && c3 == 'N' && len >= 8 &&
      pEnd[-4] == 'O' && pEnd[-5] == 'I' && pEnd[-6] == 'T')
    {
      if (len >= 9 && pEnd[-7] == 'A' && MeasureG0(pStart, pEnd - 7))
        pEnd[-5] = 'E', pEnd -= 4, len -= 4; // (m > 0) ATIONAL -> ATE
      else if (MeasureG0(pStart, pEnd - 6))
        pEnd -= 2, len -= 2; // (m > 0) TIONAL -> TION
    }
    else if (c1 == 'I' && c2 == 'C' && c3 == 'N' && len >= 6)
    {
      char c4 = pEnd[-4]; if (c4 == 'A' || c4 == 'E')
        if (MeasureG0(pStart, pEnd - 4))
          pEnd[-1] = 'E'; // (m > 0) ENCI -> ENCE, (m > 0) ANCI -> ANCE
    }
    else if (c1 == 'R' && c2 == 'E' && c3 == 'Z' && len >= 6 && pEnd[-4] == 'I' &&
      MeasureG0(pStart, pEnd - 4)) pEnd--, len--; // (m > 0) IZER -> IZE.
    else if (c1 == 'I' && c2 == 'L')
    {
      if (c3 == 'B' && MeasureG0(pStart, pEnd - 3)) 
        pEnd[-1] = 'E'; // (m > 0) BLI -> BLE
      else if (c3 == 'L' && pEnd[-4] == 'A' && MeasureG0(pStart, pEnd - 4)) 
        pEnd -= 2, len -= 2; // (m > 0) ALLI -> AL
      else if (len >= 7 && c3 == 'T' && pEnd[-4] == 'N' && pEnd[-5] == 'E' && MeasureG0(pStart, pEnd - 5))
        pEnd -= 2, len -= 2; // (m > 0) ENTLI -> ENT
      else if (c3 == 'E' && MeasureG0(pStart, pEnd - 3))
        pEnd -= 2, len -= 2; // (m > 0) ELI -> E
      else if (len >= 7 && c3 == 'S' && pEnd[-4] == 'U' && pEnd[-5] == 'O' && MeasureG0(pStart, pEnd - 5))
        pEnd -= 2, len -= 2; // (m > 0) OUSLI -> OUS
    }
    else if (c1 == 'I' && c2 == 'T' && c3 == 'I' && len >= 7)
    {
      int c4 = pEnd[-4], c5 = pEnd[-5];
      if (c4 == 'L' && c5 == 'A' && MeasureG0(pStart, pEnd - 5))
        pEnd -= 3, len -= 3; // (m > 0) ALITI -> AL
      else if (c4 == 'V' && c5 == 'I' && MeasureG0(pStart, pEnd - 5))
        pEnd[-3] = 'E', pEnd -= 2, len -= 2; // (m > 0) IVITI -> IVE
      else if (c4 == 'L' && c5 == 'I' && len >= 8 && pEnd[-6] == 'B' &&
        MeasureG0(pStart, pEnd - 6)) pEnd[-5] = 'L', pEnd[-4] = 'E', 
        pEnd -= 3, len -= 3; // (m > 0) BILITI -> BLE
    }
    else if (c1 == 'N' && c2 == 'O' && c3 == 'I' && len >= 7 && pEnd[-4] == 'T' &&
      pEnd[-5] == 'A')
    {
      if (len >= 9 && pEnd[-6] == 'Z' && pEnd[-7] == 'I' && // (m > 0) IZATION -> IZE
        MeasureG0(pStart, pEnd - 7)) pEnd[-5] = 'E', pEnd -= 4, len -= 4;
      else if (MeasureG0(pStart, pEnd - 5)) pEnd[-3] = 'E', pEnd -= 2, len -= 2; // (m > 0) ATION -> ATE
    }
    else if (c1 == 'R' && c2 == 'O' && c3 == 'T' && len >= 6 && pEnd[-4] == 'A' &&
      MeasureG0(pStart, pEnd - 4)) pEnd[-2] = 'E', pEnd -= 1, len--; // (m > 0) ATOR -> ATE
    else if (c1 == 'M' && c2 == 'S' && c3 == 'I' && len >= 7 && pEnd[-4] == 'L' &&
      pEnd[-5] == 'A' && MeasureG0(pStart, pEnd - 5)) pEnd -= 3, len -= 3; // (m > 0) ALISM -> AL
    else if (c1 == 'S' && c2 == 'S' && c3 == 'E' && len >= 9 && pEnd[-4] == 'N')
    {
      char c5 = pEnd[-5], c6 = pEnd[-6], c7 = pEnd[-7];
      if ((c5 == 'E' && c6 == 'V' && c7 == 'I') || // (m > 0) IVENESS -> IVE
        (c5 == 'L' && c6 == 'U' && c7 == 'F') || // (m > 0) FULNESS -> FUL
        (c5 == 'S' && c6 == 'U' && c7 == 'O'))   // (m > 0) OUSNESS -> OUS
        if (MeasureG0(pStart, pEnd - 7)) pEnd -= 4, len -= 4; 
    }
    else if (c1 == 'I' && c2 == 'G' && c3 == 'O' && len >= 6 && pEnd[-4] == 'L' &&
      MeasureG0(pStart, pEnd - 4)) pEnd--, len--; // (m > 0) LOGY -> LOG


  }

  // Step 3.
  if (len >= 5)
  {
    char c1 = pEnd[-1], c2 = pEnd[-2], c3 = pEnd[-3]; 
    if (c1 == 'E' && c2 == 'T' && c3 == 'A' && len >= 7 &&
      pEnd[-4] == 'C' && pEnd[-5] == 'I' && MeasureG0(pStart, pEnd - 5))
      pEnd -= 3, len -= 3; // (m > 0) ICATE -> IC
    else if (c1 == 'E' && c2 == 'V' && c3 == 'I' && len >= 7 &&
      pEnd[-4] == 'T' && pEnd[-5] == 'A' && MeasureG0(pStart, pEnd - 5))
      pEnd -= 5, len -= 5; // (m > 0) ATIVE -> eps
    else if (c1 == 'E' && c2 == 'Z' && c3 == 'I' && len >= 7 &&
      pEnd[-4] == 'L' && pEnd[-5] == 'A' && MeasureG0(pStart, pEnd - 5))
      pEnd -= 3, len -= 3; // (m > 0) ALIZE -> AL
    else if (c1 == 'I' && c2 == 'T' && c3 == 'I' && len >= 7 &&
      pEnd[-4] == 'C' && pEnd[-5] == 'I' && MeasureG0(pStart, pEnd - 5))
      pEnd -= 3, len -= 3; // (m > 0) ICITI -> IC
    else if (c1 == 'L' && c2 == 'A' && c3 == 'C' && len >= 6 &&
      pEnd[-4] == 'I' && MeasureG0(pStart, pEnd - 4))
      pEnd -= 2, len -= 2; // (m > 0) ICAL -> IC
    else if (c1 == 'L' && c2 == 'U' && c3 == 'F' && MeasureG0(pStart, pEnd - 3))
      pEnd -= 3, len -= 3; // (m > 0) FUL -> eps
    else if (c1 == 'S' && c2 == 'S' && c3 == 'E' && len >= 6 &&
      pEnd[-4] == 'N' && MeasureG0(pStart, pEnd - 4))
      pEnd -= 4, len -= 4; // (m > 0) NESS -> eps
  }

  // Step 4.  Here we require m > 1, with suffixes of length 2 or more.
  // Thus the total length must be at least 6.
  if (len >= 6)
  {
    char c1 = pEnd[-1], c2 = pEnd[-2];
    if (c1 == 'L' && c2 == 'A' && Measure(pStart, pEnd - 2) > 1)
      pEnd -= 2, len -= 2; // (m > 1) AL -> eps
    else if (c1 == 'E' && c2 == 'C' && len >= 8 && pEnd[-3] == 'N')
    {
      char c4 = pEnd[-4]; if (c4 == 'A' || c4 == 'E') if (Measure(pStart, pEnd - 4) > 1)
        pEnd -= 4, len -= 4; // (m > 1) ANCE -> eps, (m > 1) ENCE -> eps
    }
    else if (c1 == 'R' && c2 == 'E' && Measure(pStart, pEnd - 2) > 1)
      pEnd -= 2, len -= 2; // (m > 1) ER -> eps
    else if (c1 == 'C' && c2 == 'I' && Measure(pStart, pEnd - 2) > 1)
      pEnd -= 2, len -= 2; // (m > 1) IC -> eps
    else if (c1 == 'E' && c2 == 'L' && len >= 8 && pEnd[-3] == 'B')
    {
      char c4 = pEnd[-4]; if (c4 == 'A' || c4 == 'I') if (Measure(pStart, pEnd - 4) > 1)
        pEnd -= 4, len -= 4; // (m > 1) ABLE -> eps, (m > 1) IBLE -> eps
    }
    else if (c1 == 'T' && c2 == 'N' && len >= 7)
    {
      char c3 = pEnd[-3];
      if (c3 == 'A' && Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1) ANT -> eps
      else if (c3 == 'E')
      {
        int c4 = pEnd[-4], c5 = pEnd[-5];
        // Note that if EMENT matches but does not have m > 1, we
        // should not try MENT and ENT.  This would change "agreement"
        // into "agreem", which we don't like.
        if (c4 == 'M' && c5 == 'E')
          { if (len >= 9 && Measure(pStart, pEnd - 5) > 1) pEnd -= 5, len -= 5; } // (m > 1) EMENT -> eps
        else if (c4 == 'M')
          { if (len >= 8 && Measure(pStart, pEnd - 4) > 1) pEnd -= 4, len -= 4; } // (m > 1) MENT -> eps
        else
          { if (Measure(pStart, pEnd - 3) > 1) pEnd -= 3, len -= 3; } // (m > 1) ENT -> eps
      }
    }
    else if (c1 == 'N' && c2 == 'O' && len >= 7 && pEnd[-3] == 'I')
    {
      char c4 = pEnd[-4]; if (c4 == 'S' || c4 == 'T') if (Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1 and (*S or *T)) ION -> eps
    }
    else if (c1 == 'U' && c2 == 'O' && Measure(pStart, pEnd - 2) > 1)
      pEnd -= 2, len -= 2; // (m > 1) OU -> eps
    else if (len >= 7)
    {
      char c3 = pEnd[-3];
      if (c1 == 'M' && c2 == 'S' && c3 == 'I' && Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1) ISM -> eps
      else if (c1 == 'E' && c2 == 'T' && c3 == 'A' && Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1) ATE -> eps
      else if (c1 == 'I' && c2 == 'T' && c3 == 'I' && Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1) ITI -> eps
      else if (c1 == 'S' && c2 == 'U' && c3 == 'O' && Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1) OUS -> eps
      else if (c1 == 'E' && c3 == 'I' && (c2 == 'V' || c2 == 'Z') && Measure(pStart, pEnd - 3) > 1)
        pEnd -= 3, len -= 3; // (m > 1) IVE -> eps
    }
  }

  // Step 5a.
  if (len >= 5 && pEnd[-1] == 'E' && Measure(pStart, pEnd - 1) > 1)
    pEnd--, len--; // (m > 1) E -> eps
  else if (len >= 3 && pEnd[-1] == 'E' && Measure(pStart, pEnd - 1) == 1 &&
    ! IsCvc(pStart, pEnd - 1)) pEnd--, len--; // (m == 1 and not *o) E -> eps

  // Step 5b.
  if (len >= 4 && pEnd[-1] == 'L' && pEnd[-2] == 'L' && Measure(pStart, pEnd) > 1)
    pEnd--, len--; // (m > 1 and *d and *L) -> single letter

  *pEnd = '\0'; return pWord;
}

TStr TPorterStemmer::Stem(const TStr& s){
  TChA buf = s; buf.ToUc();
  return StemInPlace(buf.CStr());
}

TStr TPorterStemmer::StemX(const TStr& s){
  TChA buf = s; buf.ToUc();
  int len = buf.Len(); char *p = buf.CStr();
  if (len > 1 && p[len - 1] == 'S' && p[len - 2] == '\'')
    p[len - 2] = '\0';
  else if (len > 0 && p[len - 1] == '\'')
    p[len - 1] = '\0';
  return StemInPlace(p);
}

