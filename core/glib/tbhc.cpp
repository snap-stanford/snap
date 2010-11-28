/////////////////////////////////////////////////
// Includes
#include "tbhc.h"

/////////////////////////////////////////////////
// Html-Coded-Table
THTb::THTb(const PHcBase& _HcBase):
  TTb(), HcBase(_HcBase),
  VarNToWordIdV(HcBase->GetWords()), WordIdToVarNV(HcBase->GetMxWordIds()),
  TupNToDocIdV(HcBase->GetDocs()), DocIdToTupNV(HcBase->GetMxDocIds()){

  int WordId=HcBase->FFirstWordId(); int VarN=0-1;
  while (HcBase->FNextWordId(WordId)){VarN++;
    VarNToWordIdV[VarN]=WordId; WordIdToVarNV[WordId]=VarN;}
}

/////////////////////////////////////////////////
// Html-Coded-Binary-Table
THBTb::THBTb(const PHcBase& _HcBase):
  THTb(_HcBase), TupV(HcBase->GetDocs()){
  int DocId=HcBase->FFirstDocId(); int TupN=0-1;
  while (HcBase->FNextDocId(DocId)){TupN++;
    TupNToDocIdV[TupN]=DocId; DocIdToTupNV[DocId]=TupN;
    PHcDoc Doc=HcBase->GetDoc(DocId);
    PBSet Tup=new TBSet(VarNToWordIdV.Len());
    for (int WordN=0; WordN<Doc->Len(); WordN++){
      Tup->Incl(WordIdToVarNV[Doc->Get(WordN).GetWordId()]);}
    TupV[TupN]=Tup;
  }
}

/////////////////////////////////////////////////
// Html-Coded-Sequence-Table
THSTb::THSTb(const PHcBase& _HcBase):
  THTb(_HcBase), TupV(HcBase->GetDocs()){
  THash<TInt, TInt> DocWordsH(1000);
  int DocId=HcBase->FFirstDocId(); int TupN=0-1;
  while (HcBase->FNextDocId(DocId)){TupN++;
    TupNToDocIdV[TupN]=DocId; DocIdToTupNV[DocId]=TupN;
    PHcDoc Doc=HcBase->GetDoc(DocId);

    DocWordsH.Clr();
    for (int WordN=0; WordN<Doc->Len(); WordN++){
      DocWordsH.AddDat(Doc->Get(WordN).GetWordId())++;}

    TupV[TupN].Reserve(DocWordsH.Len());
    int DocWordsP=DocWordsH.FFirstKeyId();
    while (DocWordsH.FNextKeyId(DocWordsP)){
      TupV[TupN].Add(DocWordsH.GetKey(DocWordsP));}
    TupV[TupN].Sort();
  }
}

/////////////////////////////////////////////////
// Html-Coded-Frequency-Table
THFTb::THFTb(const PHcBase& _HcBase):
  THTb(_HcBase), TupV(HcBase->GetDocs()){
  THash<TInt, TInt> DocWordsH(1000);
  int DocId=HcBase->FFirstDocId(); int TupN=0-1;
  while (HcBase->FNextDocId(DocId)){TupN++;
    TupNToDocIdV[TupN]=DocId; DocIdToTupNV[DocId]=TupN;
    PHcDoc Doc=HcBase->GetDoc(DocId);

    DocWordsH.Clr();
    for (int WordN=0; WordN<Doc->Len(); WordN++){
      DocWordsH.AddDat(Doc->Get(WordN).GetWordId())++;}

    TupV[TupN].Reserve(DocWordsH.Len());
    int DocWordsP=DocWordsH.FFirstKeyId();
    while (DocWordsH.FNextKeyId(DocWordsP)){
      TupV[TupN].Add(DocWordsH.GetKeyDat(DocWordsP));}
    TupV[TupN].Sort();
  }
}

