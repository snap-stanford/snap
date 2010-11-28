/////////////////////////////////////////////////
// Word-Inverted-Index-Character-Definition
void TWixChDef::AddBit4CdStr(const TStr& Str){
  int Cd=Bit4CdToStrV.Add(Str);
  IAssert(Bit4CdToStrV.Len()<=16);
  for (int ChN=0; ChN<Str.Len(); ChN++){
    char Ch=Str[ChN];
    IAssert(ChToBit4CdV[Ch-TCh::Mn]==-1);
    ChToBit4CdV[Ch-TCh::Mn]=Cd;
  }
}

void TWixChDef::AddBit6CdCh(const char& Ch){
  int Cd=Bit6CdToChV.Add(Ch);
  IAssert(Bit6CdToChV.Len()<=64);
  IAssert(ChToBit6CdV[Ch-TCh::Mn]==-1);
  ChToBit6CdV[Ch-TCh::Mn]=Cd;
}

void TWixChDef::AddChSet(const TStr& _ChSetStr){
  IAssert(_ChSetStr.LastCh()==TCh::HashCh);
  TChA ChSetChA;
  TChA Bit4ChA;
  for (int ChN=0; ChN<_ChSetStr.Len(); ChN++){
    char Ch=_ChSetStr[ChN];
    if (Ch==TCh::HashCh){
      AddBit4CdStr(Bit4ChA);
      Bit4ChA.Clr();
    } else {
      ChSetChA+=Ch;
      AddBit6CdCh(Ch);
      Bit4ChA+=Ch;
    }
  }
  ChSetStr=ChSetChA;
}

void TWixChDef::SetUcCh(const TStr& Str){
  char UcCh=Str[0];
  IAssert(GetBit6VCd(UcCh)!=-1);
  for (int CC=1; CC<Str.Len(); CC++){
    char Ch=Str[CC];
    IAssert(GetBit6VCd(Ch)==-1);
    UcChV[Ch-TCh::Mn]=TCh(UcCh);
  }
}

void TWixChDef::SetMnMxCh(){
  MnCh=Bit6CdToChV[0];
  MxCh=Bit6CdToChV.Last();
}

void TWixChDef::ConvVecToTb(){
  ChToBit4CdT=new int[ChToBit4CdV.Len()];
  for (int ChToBit4CdN=0; ChToBit4CdN<ChToBit4CdV.Len(); ChToBit4CdN++){
    ChToBit4CdT[ChToBit4CdN]=ChToBit4CdV[ChToBit4CdN];}
  Bit6CdToChT=new char[Bit6CdToChV.Len()];
  for (int Bit6CdToChN=0; Bit6CdToChN<Bit6CdToChV.Len(); Bit6CdToChN++){
    Bit6CdToChT[Bit6CdToChN]=Bit6CdToChV[Bit6CdToChN];}
  ChToBit6CdT=new int[ChToBit6CdV.Len()];
  for (int ChToBit6CdN=0; ChToBit6CdN<ChToBit6CdV.Len(); ChToBit6CdN++){
    ChToBit6CdT[ChToBit6CdN]=ChToBit6CdV[ChToBit6CdN];}
  UcChT=new char [UcChV.Len()];
  for (int UcChN=0; UcChN<UcChV.Len(); UcChN++){
    UcChT[UcChN]=UcChV[UcChN];}
}

TWixChDef::TWixChDef():
  ChSetStr(),
  Bit4CdToStrV(16, 0), ChToBit4CdV(TCh::Vals),
  Bit6CdToChV(64, 0), ChToBit6CdV(TCh::Vals),
  UcChV(TCh::Vals), MnCh(), MxCh(),
  ChToBit4CdT(NULL), Bit6CdToChT(NULL), ChToBit6CdT(NULL), UcChT(NULL){
  ChToBit4CdV.PutAll(TInt(-1)); ChToBit6CdV.PutAll(TInt(-1));
  AddChSet(" #0#123#456#789#AB#C^]D\\#EF#GH#IJKL#MN#OPQ#RS[#TU#VWXY#Z@#");

  for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){UcChV[Ch-TCh::Mn]=TCh(char(Ch));}
  SetUcCh("Aa"); SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("^~"); SetUcCh("]}");
  SetUcCh("Dd"); SetUcCh("\\|"); SetUcCh("Ee"); SetUcCh("Ff"); SetUcCh("Gg");
  SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("Jj"); SetUcCh("Kk"); SetUcCh("Ll");
  SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("Oo"); SetUcCh("Pp"); SetUcCh("Qq");
  SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("[{"); SetUcCh("Tt"); SetUcCh("Uu");
  SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx"); SetUcCh("Yy"); SetUcCh("Zz");
  SetUcCh("@`");

  // windows-1250
  SetUcCh("^\xC8\xE8");
  SetUcCh("]\xC6\xE6");
  SetUcCh("\\\xD0\xF0");
  SetUcCh("[\x8A\x9A");
  SetUcCh("@\x8E\x9E");

  // latin-2 (differences to windows-1250)
  SetUcCh("[\xA9\xB9");
  SetUcCh("@\xAE\xBE");

  // set minimal & maximal character
  SetMnMxCh();

  // construct tables from vectors
  ConvVecToTb();
}

TWixChDef::~TWixChDef(){
  delete[] ChToBit4CdT;
  delete[] Bit6CdToChT;
  delete[] ChToBit6CdT;
  delete[] UcChT;
}

void TWixChDef::GetNrWordChA(
 const TChA& WordChA, bool& IsWordOk, TChA& NrWordChA) const {
  NrWordChA.Clr();
  for (int ChN=0; ChN<WordChA.Len(); ChN++){
    char Ch=GetUcTCh(WordChA[ChN]);
    if (GetBit6TCd(Ch)==-1){IsWordOk=false; return;}
    NrWordChA+=Ch;
  }
  IsWordOk=true;
}

bool TWixChDef::IsNrWordChA(const TChA& NrWordChA) const {
  for (int ChN=0; ChN<NrWordChA.Len(); ChN++){
    if (GetBit6TCd(NrWordChA[ChN])==-1){return false;}}
  return true;
}

void TWixChDef::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("ChDef", true, true);
  Lx.PutVarStr("ChSetStr", ChSetStr);
  Lx.PutVar("Bit4CdToStrV", true, true);
  for (int Cd=0; Cd<Bit4CdToStrV.Len(); Cd++){
    Lx.PutVarStr(TInt::GetStr(Cd), Bit4CdToStrV[Cd], true, false);}
  Lx.PutVarEnd(true, true);
  Lx.PutVar("ChToBit4CdV", true, true);
  {for (int Cd=0; Cd<ChToBit4CdV.Len(); Cd++){
    Lx.PutVarInt(TInt::GetStr(Cd), ChToBit4CdV[Cd], (Cd+1)%10==0, false);}}
  Lx.PutVarEnd(true, true);
  Lx.PutVar("Bit6CdToChV", true, true);
  {for (int Cd=0; Cd<Bit6CdToChV.Len(); Cd++){
    Lx.PutVarInt(TInt::GetStr(Cd), int(char(Bit6CdToChV[Cd])), (Cd+1)%10==0, false);}}
  Lx.PutVarEnd(true, true);
  Lx.PutVar("ChToBit6CdV", true, true);
  {for (int Cd=0; Cd<ChToBit6CdV.Len(); Cd++){
    Lx.PutVarInt(TInt::GetStr(Cd), ChToBit6CdV[Cd], (Cd+1)%10==0, false);}}
  Lx.PutVarEnd(true, true);
  Lx.PutVar("UcChV", true, true);
  {for (int Cd=0; Cd<UcChV.Len(); Cd++){
    Lx.PutVarInt(TInt::GetStr(Cd), UcChV[Cd], (Cd+1)%10==0, false);}}
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Word-Inverted-Index-Coded-Array
TWixCdA::TWixCdA(const TChA& NrWordChA, const TWixChDef& ChDef){
//  IAssert(ChDef.IsNrWordChA(NrWordChA));
  int NrWordChALen=NrWordChA.Len();
  IAssert(TWix::MxChsPerWord==16);
//  TIntV Bit6CdV(TWix::MxChsPerWord); char Ch; // slower variant
  int Bit6CdT[16]; char Ch; // faster variant
  for (int Bit6CdN=0; Bit6CdN<TWix::MxChsPerWord; Bit6CdN++){
    if (Bit6CdN<NrWordChALen){Ch=NrWordChA[Bit6CdN];} else {Ch=' ';}
    Bit6CdT[Bit6CdN]=ChDef.GetBit6TCd(Ch);
  }

  BSet1.PutInt(26, 31, Bit6CdT[0]);
  BSet1.PutInt(20, 25, Bit6CdT[1]);
  BSet1.PutInt(14, 19, Bit6CdT[2]);
  BSet1.PutInt(8, 13, Bit6CdT[3]);
  BSet1.PutInt(2, 7, Bit6CdT[4]);
  BSet1.PutInt(0, 1, Bit6CdT[5]>>4);

  BSet2.PutInt(28, 31, Bit6CdT[5]);
  BSet2.PutInt(22, 27, Bit6CdT[6]);
  BSet2.PutInt(16, 21, Bit6CdT[7]);
  BSet2.PutInt(10, 15, Bit6CdT[8]);
  BSet2.PutInt(4, 9, Bit6CdT[9]);
  BSet2.PutInt(0, 3, Bit6CdT[10]>>2);

  BSet3.PutInt(30, 31, Bit6CdT[10]);
  BSet3.PutInt(24, 29, Bit6CdT[11]);
  BSet3.PutInt(18, 23, Bit6CdT[12]);
  BSet3.PutInt(12, 17, Bit6CdT[13]);
  BSet3.PutInt(6, 11, Bit6CdT[14]);
  BSet3.PutInt(0, 5, Bit6CdT[15]);
}

int TWixCdA::GetBit6Cd(const int& ChN) const {
  switch (ChN){
    case 0: return BSet1.GetInt(26, 31);
    case 1: return BSet1.GetInt(20, 25);
    case 2: return BSet1.GetInt(14, 19);
    case 3: return BSet1.GetInt(8, 13);
    case 4: return BSet1.GetInt(2, 7);
    case 5: return (BSet1.GetInt(0, 1)<<4)+BSet2.GetInt(28, 31);
    case 6: return BSet2.GetInt(22, 27);
    case 7: return BSet2.GetInt(16, 21);
    case 8: return BSet2.GetInt(10, 15);
    case 9: return BSet2.GetInt(4, 9);
    case 10: return (BSet2.GetInt(0, 3)<<2)+BSet3.GetInt(30, 31);
    case 11: return BSet3.GetInt(24, 29);
    case 12: return BSet3.GetInt(18, 23);
    case 13: return BSet3.GetInt(12, 17);
    case 14: return BSet3.GetInt(6, 11);
    case 15: return BSet3.GetInt(0, 5);
    default: Fail; return -1;
  }
}

bool TWixCdA::IsPrefix(const int& Chs, const TWixCdA& CdA) const {
  IAssert(TWix::MxChsPerWord==16);
  IAssert(Chs>=0);
  switch (Chs){
    case 0: return true;
    case 1: return BSet1.IsPrefix(CdA.BSet1, 26);
    case 2: return BSet1.IsPrefix(CdA.BSet1, 20);
    case 3: return BSet1.IsPrefix(CdA.BSet1, 14);
    case 4: return BSet1.IsPrefix(CdA.BSet1, 8);
    case 5: return BSet1.IsPrefix(CdA.BSet1, 2);
    case 6: return (BSet1==CdA.BSet1)&&(BSet2.IsPrefix(CdA.BSet2, 28));
    case 7: return (BSet1==CdA.BSet1)&&(BSet2.IsPrefix(CdA.BSet2, 22));
    case 8: return (BSet1==CdA.BSet1)&&(BSet2.IsPrefix(CdA.BSet2, 16));
    case 9: return (BSet1==CdA.BSet1)&&(BSet2.IsPrefix(CdA.BSet2, 10));
    case 10: return (BSet1==CdA.BSet1)&&(BSet2.IsPrefix(CdA.BSet2, 4));
    case 11: return (BSet1==CdA.BSet1)&&(BSet2==CdA.BSet2)&&(BSet3.IsPrefix(CdA.BSet3, 30));
    case 12: return (BSet1==CdA.BSet1)&&(BSet2==CdA.BSet2)&&(BSet3.IsPrefix(CdA.BSet3, 24));
    case 13: return (BSet1==CdA.BSet1)&&(BSet2==CdA.BSet2)&&(BSet3.IsPrefix(CdA.BSet3, 18));
    case 14: return (BSet1==CdA.BSet1)&&(BSet2==CdA.BSet2)&&(BSet3.IsPrefix(CdA.BSet3, 12));
    case 15: return (BSet1==CdA.BSet1)&&(BSet2==CdA.BSet2)&&(BSet3.IsPrefix(CdA.BSet3, 6));
    default: return (BSet1==CdA.BSet1)&&(BSet2==CdA.BSet2)&&(BSet3==CdA.BSet3);
  }
}

void TWixCdA::GetNrWordChA(const TWixChDef& ChDef, TChA& NrWordChA) const {
  IAssert(TWix::MxChsPerWord==16);
  NrWordChA.Clr();
  char Ch;
  Ch=ChDef.GetBit6TCh(BSet1.GetInt(26, 31));
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet1.GetInt(20, 25));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet1.GetInt(14, 19));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet1.GetInt(8, 13));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet1.GetInt(2, 7));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh((BSet1.GetInt(0, 1)<<4)+BSet2.GetInt(28, 31));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet2.GetInt(22, 27));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet2.GetInt(16, 21));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet2.GetInt(10, 15));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet2.GetInt(4, 9));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh((BSet2.GetInt(0, 3)<<2)+BSet3.GetInt(30, 31));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet3.GetInt(24, 29));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet3.GetInt(18, 23));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet3.GetInt(12, 17));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet3.GetInt(6, 11));}
  if (Ch!=' '){NrWordChA+=Ch; Ch=ChDef.GetBit6TCh(BSet3.GetInt(0, 5));}
  if (Ch!=' '){NrWordChA+=Ch;}
}

int TWixCdA::GetPrimHashCd() const {
  int HashCd=BSet1.GetUInt()^BSet2.GetUInt()^BSet3.GetUInt();
  HashCd&=0x0FFFFFFF;
  return HashCd;
}

int TWixCdA::GetSecHashCd() const {
  int HashCd=BSet1.GetUInt();
  HashCd&=0x0FFFFFFF;
  return HashCd;
}

/////////////////////////////////////////////////
// Word-Inverted-Index-Document-Id-Vector
int TWixDocIdSet::GetDocIds(){
  IAssert(Wix->GetWord(WordId).GetDocIds()==DocIdV.Len());
  return DocIdV.Len();
}

void TWixDocIdSet::AddDocId(const TBlobPt& DocId, const bool& DoIncDocIds){
  Modified=true;
  if (DoIncDocIds){Wix->GetWord(WordId).IncDocIds();}
  DocIdV.AddSorted(DocId);
}

void TWixDocIdSet::OnDelFromCache(const TBlobPt& BlobPt, void*){
  IAssert(Wix->GetWord(WordId).GetDocIds()>TWix::MxDocIdsInWord);
  if (Modified){
    TMOut MOut; Save(MOut);
    TBlobPt NewBlobPt=Wix->GetDocIdBBs()->PutBlob(BlobPt, MOut.GetSIn());
    Wix->GetWord(WordId).PutDocIdSetBPt(NewBlobPt);
  }
}

void TWixDocIdSet::SaveTxt(TOLx& Lx, const TWixChDef& ChDef){
  TChA NrWordChA;
  Wix->GetWord(WordId).GetNrWordChA(ChDef, NrWordChA);
  Lx.PutQStr(NrWordChA);
  for (int DocIdN=0; DocIdN<DocIdV.Len(); DocIdN++){
    Lx.PutUQStr(DocIdV[DocIdN].GetStr());
  }
}

/////////////////////////////////////////////////
// Word-Inverted-Index-Word
void TWixWord::AddDocId(const TBlobPt& DocId){
  IAssert(int(DocIds)<TWix::MxDocIdsInWord);
  if (DocIds==0){
    BPt1=DocId; DocIds++;
  } else
  if (DocIds==1){
    if (BPt1<DocId){BPt2=DocId;} else {BPt1=DocId; BPt2=BPt1;}
    DocIds++;
  } else {
    IAssert(TWix::MxDocIdsInWord==4);
    TBlobPtV BPtV(TWix::MxDocIdsInWord, 0);
    if (DocIds>=1){BPtV.Add(BPt1);}
    if (DocIds>=2){BPtV.Add(BPt2);}
    if (DocIds>=3){BPtV.Add(BPt3);}
    BPtV.AddSorted(DocId);
    DocIds++;
    if (DocIds>=1){BPt1=BPtV[0];}
    if (DocIds>=2){BPt2=BPtV[1];}
    if (DocIds>=3){BPt3=BPtV[2];}
    if (DocIds>=4){BPt4=BPtV[3];}
  }
}

PWixDocIdSet TWixWord::GetDocIdSet(TWix* Wix, const int& WordId){
  IAssert(int(DocIds)<=TWix::MxDocIdsInWord);
  PWixDocIdSet DocIdSet=PWixDocIdSet(new TWixDocIdSet(Wix, WordId, DocIds+3));
  IAssert(TWix::MxDocIdsInWord==4);
  if (DocIds>=1){DocIdSet->AddDocId(BPt1, false);}
  if (DocIds>=2){DocIdSet->AddDocId(BPt2, false);}
  if (DocIds>=3){DocIdSet->AddDocId(BPt3, false);}
  if (DocIds>=4){DocIdSet->AddDocId(BPt4, false);}
  return DocIdSet;
}

void TWixWord::PutDocIdSetBPt(const TBlobPt& DocIdSetBPt){
  IAssert(DocIds>TWix::MxDocIdsInWord);
  BPt1=DocIdSetBPt;
}

TBlobPt TWixWord::GetDocIdSetBPt(){
  IAssert(DocIds>TWix::MxDocIdsInWord);
  return BPt1;
}

/////////////////////////////////////////////////
// Word-Inverted-Index

// word port vector constants
const int& TWix::WordPortVIxChs=5;
const int& TWix::WordPortVBitsPerCh=4;
const int& TWix::WordPortVLen=TB4Def::GetP2(WordPortVIxChs*WordPortVBitsPerCh);

// word representation constants
const int& TWix::MxChsPerWord=16;
const int& TWix::MxDocIdsInWord=4;

// file name constants
const TStr TWix::WixFExt=".wix";
const TStr TWix::WixDocIdFExt=".wdi";

// blob-pointer flag-number constants
const int TWix::InTitle_BPtFlagN=0;
const int TWix::WordFqMsb_BPtFlagN=1;
const int TWix::WordFqLsb_BPtFlagN=2;
const int TWix::InA_BPtFlagN=3;

int TWix::GetWordPortN(const TChA& WordChA, const bool& DoMaximize){
  IAssert(TWix::WordPortVIxChs==5);
  TWixChDef& RChDef=*ChDef;
  int WordChALen=WordChA.Len();
  char Ch1; char Ch2; char Ch3; char Ch4; char Ch5;
  if (DoMaximize){
    Ch1=Ch2=Ch3=Ch4=Ch5=RChDef.GetMxCh();
  } else {
    Ch1=Ch2=Ch3=Ch4=Ch5=RChDef.GetMnCh();
  }
  if (WordChALen>=1){Ch1=WordChA[0];
    if (WordChALen>=2){Ch2=WordChA[1];
      if (WordChALen>=3){Ch3=WordChA[2];
        if (WordChALen>=4){Ch4=WordChA[3];
          if (WordChALen>=5){Ch5=WordChA[4];}}}}}
  return
   RChDef.GetBit4TCd(Ch1)*65536+
   RChDef.GetBit4TCd(Ch2)*4096+
   RChDef.GetBit4TCd(Ch3)*256+
   RChDef.GetBit4TCd(Ch4)*16+
   RChDef.GetBit4TCd(Ch5);
}

int TWix::GetWordPortN(const TWixCdA& WordCdA){
  IAssert(TWix::WordPortVIxChs==5);
  TWixChDef& RChDef=*ChDef;
  char Ch1=RChDef.GetBit6TCh(WordCdA.GetBit6Cd(0));
  char Ch2=RChDef.GetBit6TCh(WordCdA.GetBit6Cd(1));
  char Ch3=RChDef.GetBit6TCh(WordCdA.GetBit6Cd(2));
  char Ch4=RChDef.GetBit6TCh(WordCdA.GetBit6Cd(3));
  char Ch5=RChDef.GetBit6TCh(WordCdA.GetBit6Cd(4));
  return
   RChDef.GetBit4TCd(Ch1)*65536+
   RChDef.GetBit4TCd(Ch2)*4096+
   RChDef.GetBit4TCd(Ch3)*256+
   RChDef.GetBit4TCd(Ch4)*16+
   RChDef.GetBit4TCd(Ch5);
}

int TWix::GetWordId(const TWixCdA& WordCdA, const bool& CreateIfNo){
  int WordPortN=GetWordPortN(WordCdA);
  int WordId=WordPortV[WordPortN];
  TWixWord Word(WordCdA);

  int PrevWordId=-1;
  while ((WordId!=-1)&&(Word<WordV[WordId])){
    PrevWordId=WordId; WordId=WordV[WordId].GetNext();}

  if (CreateIfNo){
    if ((WordId==-1)||(Word>WordV[WordId])){
      int NextWordId=WordId;
      WordId=WordV.Add(Word);
      if (NextWordId!=-1){
        WordV[WordId].PutNext(NextWordId);}
      if (PrevWordId==-1){
        WordPortV[WordPortN]=WordId;
      } else {
        WordV[PrevWordId].PutNext(WordId);
      }
    }
  } else {
    if (WordId!=-1){
      if (Word!=WordV[WordId]){WordId=-1;}
    }
  }

  return WordId;
}

int TWix::AddDocId(const int& WordId, const TBlobPt& DocId){
  TWixWord& Word=WordV[WordId];
  if (Word.GetDocIds()<TWix::MxDocIdsInWord){
    // if local place not full
    Word.AddDocId(DocId);
  } else
  if (Word.GetDocIds()==TWix::MxDocIdsInWord){
    // if local place full
    PWixDocIdSet DocIdSet=Word.GetDocIdSet(this, WordId);
    DocIdSet->AddDocId(DocId);
    TMOut MOut; DocIdSet->Save(MOut);
    TBlobPt DocIdSetBPt=DocIdBBs->PutBlob(MOut.GetSIn());
    Word.PutDocIdSetBPt(DocIdSetBPt);
    DocIdSet->PutModified(false);
    DocIdSetCache.Put(DocIdSetBPt, DocIdSet);
    return DocIdSet->GetMemUsed();
  } else {
    // if in blob-base
    TBlobPt DocIdSetBPt=Word.GetDocIdSetBPt();
    PWixDocIdSet DocIdSet; int OldDocIdSetSize = 0;
    if (DocIdSetCache.Get(DocIdSetBPt, DocIdSet)){
      OldDocIdSetSize = DocIdSet->GetMemUsed();
    } else {
      PSIn SIn=DocIdBBs->GetBlob(DocIdSetBPt);
      DocIdSet=PWixDocIdSet(new TWixDocIdSet(*SIn, this, WordId));
    }
    DocIdSetCache.Put(DocIdSetBPt, DocIdSet);
    DocIdSet->AddDocId(DocId);
    return DocIdSet->GetMemUsed() - OldDocIdSetSize;
  }
  return 0;
}

TWix::TWix(const TStr& Nm, const TStr& FPath, 
 const TFAccess& _Access, const int64& CacheSize):
  WixFNm(), WixDocIdFNm(), Access(_Access),
  ChDef(), SwCdAH(10000),
  WordPortV(), WordV(),
  DocIdBBs(), DocIdSetCache(CacheSize, 1000000, GetVoidThis()), 
  WordCdA_FqDocIdPrH(100000){ //B: added 0 to each big number
  // prepare file-names
  GetFNms(Nm, FPath, WixFNm, WixDocIdFNm);

  // prepare word-port-vector
  WordPortV.Gen(WordPortVLen);
  WordPortV.PutAll(TInt(-1));

  if (Access==faCreate){
    // prepare empty character definition
    ChDef=PWixChDef(new TWixChDef());
    // create doc-id blob-base
    DocIdBBs=PBlobBs(new TMBlobBs(WixDocIdFNm, faCreate)); //TGBlobBs -> TMBlobBs
  } else {
    IAssert((Access==faUpdate)||(Access==faRdOnly));
    // prepare blob-base for reading
    PBlobBs WixBBs=PBlobBs(new TGBlobBs(WixFNm, faRdOnly));
    // prepare blob-base traversal
    TBlobPt TrvBlobPt=WixBBs->FFirstBlobPt();
    PSIn BlobSIn;
    // read first blob with character definitions
    IAssert(WixBBs->FNextBlobPt(TrvBlobPt, BlobSIn));
    ChDef=PWixChDef(*BlobSIn);
    // read second blob with word-port-vector
    IAssert(WixBBs->FNextBlobPt(TrvBlobPt, BlobSIn));
    TIntV _WordPortV(*BlobSIn);
    IAssert(_WordPortV.Len()==WordPortV.Len());
    WordPortV=_WordPortV;
    // read rest of the blobs with word-vector pieces
    while (WixBBs->FNextBlobPt(TrvBlobPt, BlobSIn)){
      TWixWordV _WordV(*BlobSIn);
      WordV.AddV(_WordV);
    }
    // prepare doc-id blob-base
    DocIdBBs=PBlobBs(new TMBlobBs(WixDocIdFNm, Access)); //TGBlobBs -> TMBlobBs
  }

  // prepare stop-words
  PSwSet SwSet=PSwSet(new TSwSet(swstNone));
  //PSwSet SwSet=PSwSet(new TSwSet(swstWebIndex, 3));
  int SwId=SwSet->FFirstSwId();
  TStr SwStr; TChA SwChA; bool IsWordOk; TChA NrSwChA;
  while (SwSet->FNextSwId(SwId, SwStr)){
    ChDef->GetNrWordChA(SwStr, IsWordOk, NrSwChA);
    if (IsWordOk){
      TWixCdA SwCdA(NrSwChA, *ChDef);
      SwCdAH.AddKey(SwCdA);
    }
  }

  // prepare cache increase counters
  CacheResetThreshold = int64(0.1 * double(CacheSize));
  NewCacheSizeInc = 0;
}

TWix::~TWix(){
  if ((Access==faCreate)||(Access==faUpdate)){
    // flush doc-id cache
    DocIdSetCache.Flush();
    // prepare blob-base for writing
    PBlobBs WixBBs=PBlobBs(new TGBlobBs(WixFNm, faCreate));
    // save character definition
    {TMOut ChDefSOut; ChDef.Save(ChDefSOut);
    WixBBs->PutBlob(ChDefSOut.GetSIn());}
    // save word-port-vector
    {TMOut WordPortVSOut; WordPortV.Save(WordPortVSOut);
    WixBBs->PutBlob(WordPortVSOut.GetSIn());}
    // save word-vector pieces
    int WordN=0;
    while (WordN<WordV.Len()){
      TWixWordV SegWordV(10000, 0);
      while ((WordN<WordV.Len())&&(SegWordV.Len()<10000)){
        SegWordV.Add(WordV[WordN]); WordN++;}
      TMOut SegWordVSOut; SegWordV.Save(SegWordVSOut);
      WixBBs->PutBlob(SegWordVSOut.GetSIn());
    }
  }
}

TStr TWix::GetMemUsedStr(){
  return TStr()+
   "WordPortV:"+TInt::GetMegaStr(WordPortV.GetMemUsed())+
   " WordV:"+TInt::GetMegaStr(WordV.GetMemUsed())+
   " DocIdSetCache:"+TUInt64::GetMegaStr(DocIdSetCache.GetMemUsed());
}

void TWix::AddHtmlDoc(const TBlobPt& DocId, const PSIn& HtmlSIn){
  // clear word hash table with data (word-frequency, doc-id with flags)
  WordCdA_FqDocIdPrH.Clr(false, 1000);
  // wix flags
  bool InTitleTag=false; bool InATag=false;

  // construct word-list with frequencies and flags
  THtmlLx Lx(HtmlSIn, false); // html lexical
  bool IsWordOk; TChA NrWordChA; // extract to optimize
  while (Lx.GetSym()!=hsyEof){
    if (Lx.Sym==hsyStr){
      ChDef->GetNrWordChA(Lx.ChA, IsWordOk, NrWordChA);
      if (IsWordOk){
        TWixCdA WordCdA(NrWordChA, *ChDef);
        if (!SwCdAH.IsKey(WordCdA)){
          int WordKeyId=WordCdA_FqDocIdPrH.AddKey(WordCdA);
          TFqDocIdPr& FqDocIdPr=WordCdA_FqDocIdPrH[WordKeyId];
          TInt& WordFq=FqDocIdPr.Val1; // get reference to word-frequency
          TBlobPt& WordDocId=FqDocIdPr.Val2; // get reference to blob-pointer
          if (WordFq==0){WordDocId=DocId;} // initial blob-pointer is doc-id
          WordFq++; // increment word frequency
          // turn active flags for the word
          if (InTitleTag){WordDocId.PutFlag(InTitle_BPtFlagN, true);}
          if (InATag){WordDocId.PutFlag(InA_BPtFlagN, true);}
          if (WordCdA_FqDocIdPrH.Len()>200){break;} //HACK
        }
      }
    } else
    if ((Lx.Sym==hsyBTag)||(Lx.Sym==hsyETag)){
      if (Lx.ChA==THtmlTok::ATagNm){InATag=(Lx.Sym==hsyBTag);} else
      if (Lx.ChA==THtmlTok::TitleTagNm){InTitleTag=(Lx.Sym==hsyBTag);}
    }
  }

  // add word-frequency to blob-pointer flags
  int WordKeyId=WordCdA_FqDocIdPrH.FFirstKeyId();
  while (WordCdA_FqDocIdPrH.FNextKeyId(WordKeyId)){
    const TWixCdA& WordCdA=WordCdA_FqDocIdPrH.GetKey(WordKeyId);
    TFqDocIdPr& FqDocIdPr=WordCdA_FqDocIdPrH[WordKeyId];
    int WordFq=FqDocIdPr.Val1; // get word-frequency
    TBlobPt& DocId=FqDocIdPr.Val2; // get reference to blob-pointer
    // assign word-frequency flags
    bool WordFqMsbFlag; bool WordFqLsbFlag;
    if (WordFq<=1){WordFqMsbFlag=false; WordFqLsbFlag=false;}
    else if (WordFq<=2){WordFqMsbFlag=false; WordFqLsbFlag=true;}
    else if (WordFq<=4){WordFqMsbFlag=true; WordFqLsbFlag=false;}
    else {WordFqMsbFlag=true; WordFqLsbFlag=true;}
    DocId.PutFlag(WordFqMsb_BPtFlagN, WordFqMsbFlag);
    DocId.PutFlag(WordFqLsb_BPtFlagN, WordFqLsbFlag);
    // add doc-id to word
    int WordId=GetWordId(WordCdA, true);
    NewCacheSizeInc += int64(AddDocId(WordId, DocId));
  }

  // refresh cache memory size
  if (NewCacheSizeInc > CacheResetThreshold) { 
    printf("   Cache cleanup (%s:%s)\n", 
        TUInt64::GetMegaStr(NewCacheSizeInc).CStr(),
        TUInt64::GetMegaStr(CacheResetThreshold).CStr());
    DocIdSetCache.RefreshMemUsed();
    NewCacheSizeInc = 0;
  }
}

PWixDocIdSet TWix::GetDocIdSet(const int& WordId){
  TWixWord& Word=WordV[WordId];
  if (Word.GetDocIds()<=MxDocIdsInWord){
    return Word.GetDocIdSet(this, WordId);
  } else {
    PWixDocIdSet DocIdSet;
    TBlobPt DocIdSetBPt=Word.GetDocIdSetBPt();
    if (!DocIdSetCache.Get(DocIdSetBPt, DocIdSet)){
      PSIn SIn=DocIdBBs->GetBlob(DocIdSetBPt);
      DocIdSet=PWixDocIdSet(new TWixDocIdSet(*SIn, this, WordId));
    }
    DocIdSetCache.Put(DocIdSetBPt, DocIdSet);
    return DocIdSet;
  }
}

PWixDocIdSet TWix::GetDocIdSet(const TWixCdA& WordCdA){
  int WordId=GetWordId(WordCdA, false);
  if (WordId==-1){
    return NULL;
  } else {
    return GetDocIdSet(WordId);
  }
}

void TWix::GetDocIdV(const TChA& WordChA, const bool& IsExactMatch,
 const int& MxWcWords, TBlobPtV& DocIdV){
  // clear output vector
  DocIdV.Clr();

  // normalize the word
  bool IsWordOk; TChA NrWordChA;
  ChDef->GetNrWordChA(WordChA, IsWordOk, NrWordChA);
  if (!IsWordOk){return;}
  TWixCdA NrWordCdA(NrWordChA, *ChDef);

  // construction of doc-id vector
  if (IsExactMatch){
    // if exact word match required
    PWixDocIdSet DocIdSet=GetDocIdSet(NrWordCdA);
    if (!DocIdSet.Empty()){
      DocIdSet->GetDocIdV(DocIdV);}
  } else {
    // if wild-card at the end of the word assumed
    int MnWordPortN=GetWordPortN(NrWordChA, false);
    int MxWordPortN=GetWordPortN(NrWordChA, true);
    int WcWords=0;
    for (int WordPortN=MnWordPortN; WordPortN<=MxWordPortN; WordPortN++){
      int WordId=WordPortV[WordPortN];
      while (WordId!=-1){
        TWixWord& Word=WordV[WordId];
        if (Word.IsPrefixChA(NrWordChA, NrWordCdA)){
          PWixDocIdSet DocIdSet=GetDocIdSet(WordId);
          DocIdSet->OrDocIdV(DocIdV);
          WcWords++;
          if (WcWords>MxWcWords){break;}
        }
        WordId=Word.GetNext();
      }
      if (WcWords>MxWcWords){break;}
    }
  }
}

void TWix::GetFqWordV(TIntStrPrV& FqWordV){
  FqWordV.Gen(GetWords());
  for (int WordId=0; WordId<GetWords(); WordId++){
    FqWordV[WordId]=TIntStrPr(GetWordFq(WordId), GetWordStr(WordId));
  }
  FqWordV.Sort(false);
}

void TWix::SaveTxt(const PSOut& SOut){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);
  TChA NrWordChA;
  ChDef->SaveTxt(Lx);
  {for (int WordPortN=0; WordPortN<WordPortV.Len(); WordPortN++){
    int WordId=WordPortV[WordPortN];
    if (WordId!=-1){
      Lx.PutVar(TInt::GetStr(WordPortN), true, false, false);
      while (WordId!=-1){
        TWixWord& Word=WordV[WordId];
        Word.GetNrWordChA(*ChDef, NrWordChA);
        Lx.PutQStr(NrWordChA);
        Lx.PutSym(syColon);
        Lx.PutInt(TInt(Word.GetDocIds()));
        if (Word.GetDocIds()>TWix::MxDocIdsInWord){
          Lx.PutUQStr(Word.GetDocIdSetBPt().GetStr());}
        WordId=Word.GetNext();
      }
      Lx.PutVarEnd(true, true);
    }
  }}
  Lx.PutVar("WordV", true, true);
  {for (int WordPortN=0; WordPortN<WordPortV.Len(); WordPortN++){
    int WordId=WordPortV[WordPortN];
    if (WordId!=-1){
      Lx.PutVar(TInt::GetStr(WordPortN), true, true, false);
      while (WordId!=-1){
        TWixWord& Word=WordV[WordId];
        Word.GetNrWordChA(*ChDef, NrWordChA);
        TWixCdA NrWordCdA(NrWordChA, *ChDef);
        Lx.PutQStr(NrWordChA);
        Lx.PutSym(syLParen); Lx.PutInt(WordId); Lx.PutSym(syRParen);
        PWixDocIdSet DocIdSet=GetDocIdSet(NrWordCdA);
        Lx.PutInt(TInt(DocIdSet->GetDocIds()));
        DocIdSet->SaveTxt(Lx, *ChDef);
        Lx.PutLn();
        WordId=Word.GetNext();
      }
      Lx.PutVarEnd(true, true);
    }
  }}
  Lx.PutVarEnd(true, true);
  Lx.PutVar("WordFqV", true, true);
  TIntStrPrV FqWordV;
  GetFqWordV(FqWordV);
  for (int FqWordN=0; FqWordN<FqWordV.Len(); FqWordN++){
    Lx.PutQStr(FqWordV[FqWordN].Val2);
    Lx.PutInt(FqWordV[FqWordN].Val1);
    Lx.PutLn();
  }
  Lx.PutVarEnd(true, true);
}

void TWix::SaveTxtWordFq(const PSOut& SOut){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);
  TIntStrPrV FqWordV;
  GetFqWordV(FqWordV);
  for (int FqWordN=0; FqWordN<FqWordV.Len(); FqWordN++){
    Lx.PutQStr(FqWordV[FqWordN].Val2);
    Lx.PutInt(FqWordV[FqWordN].Val1);
    Lx.PutLn();
  }
}

void TWix::GetFNms(
 const TStr& Nm, const TStr& FPath,
 TStr& WixFNm, TStr& WixDocIdFNm){
  TStr NrFPath=TStr::GetNrFPath(FPath);
  TStr NrNm=TStr::GetNrFMid(Nm);
  WixFNm=NrFPath+NrNm+WixFExt;
  WixDocIdFNm=NrFPath+NrNm+WixDocIdFExt;
}

bool TWix::Exists(const TStr& Nm, const TStr& FPath){
  TStr WixFNm; TStr WixDocIdFNm;
  GetFNms(Nm, FPath, WixFNm, WixDocIdFNm);
  return
   TFile::Exists(WixFNm)&&
   TFile::Exists(WixDocIdFNm);
}

void TWix::Del(const TStr& Nm, const TStr& FPath){
  if (Exists(Nm, FPath)){
    TStr WixFNm; TStr WixDocIdFNm;
    GetFNms(Nm, FPath, WixFNm, WixDocIdFNm);
    TFile::Del(WixFNm);
    TFile::Del(WixDocIdFNm);
  }
}

