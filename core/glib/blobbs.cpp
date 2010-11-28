/////////////////////////////////////////////////
// Blob-Pointer
const int TBlobPt::Flags=24;

void TBlobPt::PutFlag(const int& FlagN, const bool& Val){
  IAssert((0<=FlagN)&&(FlagN<Flags));
  switch (FlagN/8){
    case 0: FSet1.SetBit(7-FlagN%8, Val); break;
    case 1: FSet2.SetBit(7-FlagN%8, Val); break;
    case 2: FSet3.SetBit(7-FlagN%8, Val); break;
    default: Fail;
  }
}

bool TBlobPt::IsFlag(const int& FlagN) const {
  IAssert((0<=FlagN)&&(FlagN<Flags));
  switch (FlagN/8){
    case 0: return FSet1.GetBit(7-FlagN%8);
    case 1: return FSet2.GetBit(7-FlagN%8);
    case 2: return FSet3.GetBit(7-FlagN%8);
    default: Fail; return false;
  }
}

void TBlobPt::PutFSet(const int& FSetN, const TB8Set& FSet){
  switch (FSetN){
    case 1: FSet1=FSet; break;
    case 2: FSet2=FSet; break;
    case 3: FSet3=FSet; break;
    default: Fail;
  }
}

TB8Set TBlobPt::GetFSet(const int& FSetN){
  switch (FSetN){
    case 1: return FSet1;
    case 2: return FSet2;
    case 3: return FSet3;
    default: Fail; return TB8Set();
  }
}

TStr TBlobPt::GetStr() const {
  TChA ChA;
  ChA+='[';
  if (Empty()){
    ChA+="Null";
  } else {
    ChA+=TUInt::GetStr(uint(Seg)); ChA+=':'; ChA+=TUInt::GetStr(Addr);
    for (int FlagN=0; FlagN<Flags; FlagN++){
      if (IsFlag(FlagN)){
        ChA+='{'; ChA+=TInt::GetStr(FlagN); ChA+='}';}
    }
  }
  ChA+=']';
  return ChA;
}

/////////////////////////////////////////////////
// Blob-Base
const int TBlobBs::MnBlobBfL=16;
const int TBlobBs::MxBlobFLen=1000000000;

void TBlobBs::PutVersionStr(const PFRnd& FBlobBs){
  FBlobBs->PutStr(GetVersionStr());
}

void TBlobBs::AssertVersionStr(const PFRnd& FBlobBs){
  TStr CorrVersionStr=GetVersionStr();
  bool IsOk=false;
  TStr TestVersionStr=FBlobBs->GetStr(CorrVersionStr.Len(), IsOk);
  IAssert(IsOk && (CorrVersionStr==TestVersionStr));
}

TStr TBlobBs::GetBlobBsStateStr(const TBlobBsState& BlobBsState){
  TStr StateStr;
  switch (BlobBsState){
    case bbsOpened: StateStr="Opened"; break;
    case bbsClosed: StateStr="Closed"; break;
    default: Fail; return TStr();
  }
  IAssert(StateStr.Len()==GetStateStrLen());
  return StateStr;
}

void TBlobBs::PutBlobBsStateStr(const PFRnd& FBlobBs, const TBlobBsState& State){
  FBlobBs->PutStr(GetBlobBsStateStr(State));
}

void TBlobBs::AssertBlobBsStateStr(
 const PFRnd& FBlobBs, const TBlobBsState& State){
  TStr CorrStateStr=GetBlobBsStateStr(State);
  bool IsOk;
  TStr TestStateStr=FBlobBs->GetStr(GetStateStrLen(), IsOk);
  IAssert(IsOk && (CorrStateStr==TestStateStr));
}

const TStr TBlobBs::MxSegLenVNm="MxSegLen";

void TBlobBs::PutMxSegLen(const PFRnd& FBlobBs, const int& MxSegLen){
  FBlobBs->PutStr(MxSegLenVNm);
  FBlobBs->PutInt(MxSegLen);
}

int TBlobBs::GetMxSegLen(const PFRnd& FBlobBs){
  IAssert(FBlobBs->GetStr(MxSegLenVNm.Len())==MxSegLenVNm);
  return FBlobBs->GetInt();
}

const TStr TBlobBs::BlockLenVNm="BlockLenV";

void TBlobBs::GenBlockLenV(TIntV& BlockLenV){
  BlockLenV.Clr();
  for (int P2Exp=0; P2Exp<TB4Def::MxP2Exp; P2Exp++){
    BlockLenV.Add(TInt(TB4Def::GetP2(P2Exp)));}
  IAssert(int(BlockLenV.Last())<2000000000);

  {for (int Len=10; Len<100; Len+=10){BlockLenV.Add(Len);}}
  {for (int Len=100; Len<10000; Len+=100){BlockLenV.Add(Len);}}
  {for (int Len=10000; Len<100000; Len+=1000){BlockLenV.Add(Len);}}
  {for (int Len=100000; Len<1000000; Len+=25000){BlockLenV.Add(Len);}}
  {for (int Len=1000000; Len<10000000; Len+=1000000){BlockLenV.Add(Len);}}
  {for (int Len=10000000; Len<100000000; Len+=10000000){BlockLenV.Add(Len);}}

  BlockLenV.Sort();
}

void TBlobBs::PutBlockLenV(const PFRnd& FBlobBs, const TIntV& BlockLenV){
  FBlobBs->PutStr(BlockLenVNm);
  FBlobBs->PutInt(BlockLenV.Len());
  for (int BlockLenN=0; BlockLenN<BlockLenV.Len(); BlockLenN++){
    FBlobBs->PutInt(BlockLenV[BlockLenN]);}
  FBlobBs->PutInt(-1);
}

void TBlobBs::GetBlockLenV(const PFRnd& FBlobBs, TIntV& BlockLenV){
  IAssert(FBlobBs->GetStr(BlockLenVNm.Len())==BlockLenVNm);
  BlockLenV.Gen(FBlobBs->GetInt());
  for (int BlockLenN=0; BlockLenN<BlockLenV.Len(); BlockLenN++){
    BlockLenV[BlockLenN]=FBlobBs->GetInt();}
  IAssert(FBlobBs->GetInt()==-1);
}

const TStr TBlobBs::FFreeBlobPtVNm="FFreeBlobPtV";

void TBlobBs::GenFFreeBlobPtV(const TIntV& BlockLenV, TBlobPtV& FFreeBlobPtV){
  FFreeBlobPtV.Gen(BlockLenV.Len()+1);
}

void TBlobBs::PutFFreeBlobPtV(const PFRnd& FBlobBs, const TBlobPtV& FFreeBlobPtV){
  FBlobBs->PutStr(FFreeBlobPtVNm);
  FBlobBs->PutInt(FFreeBlobPtV.Len());
  for (int BlockLenN=0; BlockLenN<FFreeBlobPtV.Len(); BlockLenN++){
    FFreeBlobPtV[BlockLenN].Save(FBlobBs);}
  FBlobBs->PutInt(-1);
}

void TBlobBs::GetFFreeBlobPtV(const PFRnd& FBlobBs, TBlobPtV& FFreeBlobPtV){
  IAssert(FBlobBs->GetStr(FFreeBlobPtVNm.Len())==FFreeBlobPtVNm);
  FFreeBlobPtV.Gen(FBlobBs->GetInt());
  for (int FFreeBlobPtN=0; FFreeBlobPtN<FFreeBlobPtV.Len(); FFreeBlobPtN++){
    FFreeBlobPtV[FFreeBlobPtN]=TBlobPt::Load(FBlobBs);}
  IAssert(FBlobBs->GetInt()==-1);
}

void TBlobBs::GetAllocInfo(
 const int& BfL, const TIntV& BlockLenV, int& MxBfL, int& FFreeBlobPtN){
  int BlockLenN=0;
  while ((BlockLenN<BlockLenV.Len())&&(BfL>BlockLenV[BlockLenN])){
    BlockLenN++;}
  IAssert(BlockLenN<BlockLenV.Len());
  MxBfL=BlockLenV[BlockLenN]; FFreeBlobPtN=BlockLenN;
}

void TBlobBs::PutBlobTag(const PFRnd& FBlobBs, const TBlobTag& BlobTag){
  switch (BlobTag){
    case btBegin: FBlobBs->PutUInt(GetBeginBlobTag()); break;
    case btEnd: FBlobBs->PutUInt(GetEndBlobTag()); break;
    default: Fail;
  }
}

void TBlobBs::AssertBlobTag(const PFRnd& FBlobBs, const TBlobTag& BlobTag){
  switch (BlobTag){
    case btBegin: IAssert(FBlobBs->GetUInt()==GetBeginBlobTag()); break;
    case btEnd: IAssert(FBlobBs->GetUInt()==GetEndBlobTag()); break;
    default: Fail;
  }
}

void TBlobBs::PutBlobState(const PFRnd& FBlobBs, const TBlobState& State){
  FBlobBs->PutCh(char(State));
}

TBlobState TBlobBs::GetBlobState(const PFRnd& FBlobBs){
  return TBlobState(int(FBlobBs->GetCh()));
}

void TBlobBs::AssertBlobState(const PFRnd& FBlobBs, const TBlobState& State){
  IAssert(TBlobState(FBlobBs->GetCh())==State);
}

void TBlobBs::AssertBfCsEqFlCs(const TCs& BfCs, const TCs& FCs){
  if (BfCs!=FCs){
    printf("[%d:%d]\n", BfCs.Get(), FCs.Get());}
  //IAssert(BfCs==FCs);
}

/////////////////////////////////////////////////
// General-Blob-Base
TStr TGBlobBs::GetNrBlobBsFNm(const TStr& BlobBsFNm){
  TStr NrBlobBsFNm=BlobBsFNm;
  if (NrBlobBsFNm.GetFExt().Empty()){
    NrBlobBsFNm=NrBlobBsFNm+".gbb";}
  return NrBlobBsFNm;
}

TGBlobBs::TGBlobBs(
 const TStr& BlobBsFNm, const TFAccess& _Access, const int& _MxSegLen):
  TBlobBs(), FBlobBs(), Access(_Access), MxSegLen(_MxSegLen),
  BlockLenV(), FFreeBlobPtV(TB4Def::B4Bits), FirstBlobPt(){
  if (MxSegLen==-1){MxSegLen=MxBlobFLen;}
  TStr NrBlobBsFNm=GetNrBlobBsFNm(BlobBsFNm);
  switch (Access){
    case faCreate:
      FBlobBs=TFRnd::New(NrBlobBsFNm, faCreate, true); break;
    case faUpdate:
    case faRdOnly:
    case faRestore:
      FBlobBs=TFRnd::New(NrBlobBsFNm, faUpdate, true); break;
    default: Fail;
  }
  if (FBlobBs->Empty()){
    FBlobBs->SetFPos(0);
    PutVersionStr(FBlobBs);
    PutBlobBsStateStr(FBlobBs, bbsOpened);
    PutMxSegLen(FBlobBs, MxSegLen);
    GenBlockLenV(BlockLenV);
    PutBlockLenV(FBlobBs, BlockLenV);
    GenFFreeBlobPtV(BlockLenV, FFreeBlobPtV);
    PutFFreeBlobPtV(FBlobBs, FFreeBlobPtV);
  } else {
    FBlobBs->SetFPos(0);
    AssertVersionStr(FBlobBs);
    int FPos=FBlobBs->GetFPos();
    if (Access!=faRestore){
      AssertBlobBsStateStr(FBlobBs, bbsClosed);}
    if (Access!=faRdOnly){
      FBlobBs->SetFPos(FPos);
      PutBlobBsStateStr(FBlobBs, bbsOpened);
    }
    MxSegLen=GetMxSegLen(FBlobBs);
    GetBlockLenV(FBlobBs, BlockLenV);
    GetFFreeBlobPtV(FBlobBs, FFreeBlobPtV);
  }
  FirstBlobPt=TBlobPt(FBlobBs->GetFPos());
  FBlobBs->Flush();
}

TGBlobBs::~TGBlobBs(){
  if (Access!=faRdOnly){
    FBlobBs->SetFPos(0);
    PutVersionStr(FBlobBs);
    PutBlobBsStateStr(FBlobBs, bbsClosed);
    PutMxSegLen(FBlobBs, MxSegLen);
    PutBlockLenV(FBlobBs, BlockLenV);
    PutFFreeBlobPtV(FBlobBs, FFreeBlobPtV);
  }
  FBlobBs->Flush();
  FBlobBs=NULL;
}

TBlobPt TGBlobBs::PutBlob(const PSIn& SIn){
  IAssert((Access==faCreate)||(Access==faUpdate)||(Access==faRestore));
  int BfL=SIn->Len();
  int MxBfL; int FFreeBlobPtN;
  GetAllocInfo(BfL, BlockLenV, MxBfL, FFreeBlobPtN);
  TBlobPt BlobPt; TCs Cs;
  if (FFreeBlobPtV[FFreeBlobPtN].Empty()){
    int FLen=FBlobBs->GetFLen();
    if (FLen<=MxSegLen){
      IAssert(FLen<=MxBlobFLen);
      BlobPt=TBlobPt(FLen);
      FBlobBs->SetFPos(BlobPt.GetAddr());
      PutBlobTag(FBlobBs, btBegin);
      FBlobBs->PutInt(MxBfL);
      PutBlobState(FBlobBs, bsActive);
      FBlobBs->PutInt(BfL);
      FBlobBs->PutSIn(SIn, Cs);
      FBlobBs->PutCh(TCh::NullCh, MxBfL-BfL);
      FBlobBs->PutCs(Cs);
      PutBlobTag(FBlobBs, btEnd);
    }
  } else {
    BlobPt=FFreeBlobPtV[FFreeBlobPtN];
    FBlobBs->SetFPos(BlobPt.GetAddr());
    AssertBlobTag(FBlobBs, btBegin);
    int MxBfL=FBlobBs->GetInt();
    int FPos=FBlobBs->GetFPos();
    AssertBlobState(FBlobBs, bsFree);
    FFreeBlobPtV[FFreeBlobPtN]=TBlobPt::LoadAddr(FBlobBs);
    FBlobBs->SetFPos(FPos);
    PutBlobState(FBlobBs, bsActive);
    FBlobBs->PutInt(BfL);
    FBlobBs->PutSIn(SIn, Cs);
    FBlobBs->PutCh(TCh::NullCh, MxBfL-BfL);
    FBlobBs->PutCs(Cs);
    AssertBlobTag(FBlobBs, btEnd);
  }
  FBlobBs->Flush();
  return BlobPt;
}

TBlobPt TGBlobBs::PutBlob(const TBlobPt& BlobPt, const PSIn& SIn){
  IAssert((Access==faCreate)||(Access==faUpdate)||(Access==faRestore));
  int BfL=SIn->Len();

  FBlobBs->SetFPos(BlobPt.GetAddr());
  AssertBlobTag(FBlobBs, btBegin);
  int MxBfL=FBlobBs->GetInt();
  AssertBlobState(FBlobBs, bsActive);
  if (BfL>MxBfL){
    DelBlob(BlobPt);
    return PutBlob(SIn);
  } else {
    TCs Cs;
    FBlobBs->PutInt(BfL);
    FBlobBs->PutSIn(SIn, Cs);
    FBlobBs->PutCh(TCh::NullCh, MxBfL-BfL);
    FBlobBs->PutCs(Cs);
    PutBlobTag(FBlobBs, btEnd);
    FBlobBs->Flush();
    return BlobPt;
  }
}

PSIn TGBlobBs::GetBlob(const TBlobPt& BlobPt){
  FBlobBs->SetFPos(BlobPt.GetAddr());
  AssertBlobTag(FBlobBs, btBegin);
  int MxBfL=FBlobBs->GetInt();
  AssertBlobState(FBlobBs, bsActive);
  int BfL=FBlobBs->GetInt();
  TCs BfCs; PSIn SIn=FBlobBs->GetSIn(BfL, BfCs);
  FBlobBs->MoveFPos(MxBfL-BfL);
  TCs FCs=FBlobBs->GetCs();
  AssertBlobTag(FBlobBs, btEnd);
  AssertBfCsEqFlCs(BfCs, FCs);
  return SIn;
}

void TGBlobBs::DelBlob(const TBlobPt& BlobPt){
  IAssert((Access==faCreate)||(Access==faUpdate)||(Access==faRestore));
  FBlobBs->SetFPos(BlobPt.GetAddr());
  AssertBlobTag(FBlobBs, btBegin);
  int MxBfL=FBlobBs->GetInt();
  int FPos=FBlobBs->GetFPos();
  AssertBlobState(FBlobBs, bsActive);
  /*int BfL=*/FBlobBs->GetInt();
  FBlobBs->SetFPos(FPos);
  PutBlobState(FBlobBs, bsFree);
  int _MxBfL; int FFreeBlobPtN;
  GetAllocInfo(MxBfL, BlockLenV, _MxBfL, FFreeBlobPtN);
  IAssert(MxBfL==_MxBfL);
  FFreeBlobPtV[FFreeBlobPtN].SaveAddr(FBlobBs);
  FFreeBlobPtV[FFreeBlobPtN]=BlobPt;
  FBlobBs->PutCh(TCh::NullCh, MxBfL+sizeof(TCs));
  AssertBlobTag(FBlobBs, btEnd);
  FBlobBs->Flush();
}

TBlobPt TGBlobBs::FFirstBlobPt(){
  return FirstBlobPt;
}

bool TGBlobBs::FNextBlobPt(TBlobPt& TrvBlobPt, TBlobPt& BlobPt, PSIn& BlobSIn){
  forever {
    uint TrvBlobAddr=TrvBlobPt.GetAddr();
    if (TrvBlobAddr>=uint(FBlobBs->GetFLen())){
      TrvBlobPt.Clr(); BlobPt.Clr(); BlobSIn=NULL;
      return false;
    } else {
      FBlobBs->SetFPos(TrvBlobAddr);
      AssertBlobTag(FBlobBs, btBegin);
      int MxBfL=FBlobBs->GetInt();
      TBlobState BlobState=GetBlobState(FBlobBs);
      switch (BlobState){
        case bsActive:{
          int BfL=FBlobBs->GetInt();
          TCs BfCs; BlobSIn=FBlobBs->GetSIn(BfL, BfCs);
          FBlobBs->MoveFPos(MxBfL-BfL);
          TCs FCs=FBlobBs->GetCs();
          AssertBlobTag(FBlobBs, btEnd);
          AssertBfCsEqFlCs(BfCs, FCs);
          BlobPt=TrvBlobPt;
          TrvBlobPt=TBlobPt(FBlobBs->GetFPos());
          return true;}
        case bsFree:
          FBlobBs->MoveFPos(sizeof(uint)+MxBfL+sizeof(TCs));
          AssertBlobTag(FBlobBs, btEnd);
          TrvBlobPt=TBlobPt(FBlobBs->GetFPos());
          break;
        default: Fail; return false;
      }
    }
  }
}

bool TGBlobBs::Exists(const TStr& BlobBsFNm){
  TStr NrBlobBsFNm=GetNrBlobBsFNm(BlobBsFNm);
  return TFile::Exists(NrBlobBsFNm);
}

/////////////////////////////////////////////////
// Multiple-File-Blob-Base
void TMBlobBs::GetNrFPathFMid(
 const TStr& BlobBsFNm, TStr& NrFPath, TStr& NrFMid){
  NrFPath=TStr::GetNrFPath(BlobBsFNm.GetFPath());
  NrFMid=TStr::GetNrFMid(BlobBsFNm.GetFMid());
}

TStr TMBlobBs::GetMainFNm(
 const TStr& NrFPath, const TStr& NrFMid){
  return NrFPath+NrFMid+".mbb";
}

TStr TMBlobBs::GetSegFNm(
 const TStr& NrFPath, const TStr& NrFMid, const int& SegN){
  return NrFPath+NrFMid+".mbb"+""+TStr::GetNrNumFExt(SegN);
}

void TMBlobBs::LoadMain(int& Segs){
  PSIn SIn=TFIn::New(GetMainFNm(NrFPath, NrFMid));
  TILx Lx(SIn, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);
  IAssert(Lx.GetVarStr("Version")==GetVersionStr());
  MxSegLen=Lx.GetVarInt("MxSegLen");
  Segs=Lx.GetVarInt("Segments");
}

void TMBlobBs::SaveMain() const {
  PSOut SOut=TFOut::New(GetMainFNm(NrFPath, NrFMid));
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens);
  Lx.PutVarStr("Version", GetVersionStr());
  Lx.PutVarInt("MxSegLen", MxSegLen);
  Lx.PutVarInt("Segments", SegV.Len());
}

TMBlobBs::TMBlobBs(
 const TStr& BlobBsFNm, const TFAccess& _Access, const int& _MxSegLen):
  TBlobBs(), Access(_Access), MxSegLen(_MxSegLen),
  NrFPath(), NrFMid(), SegV(), CurSegN(0){
  if (MxSegLen==-1){MxSegLen=MxBlobFLen;}
  GetNrFPathFMid(BlobBsFNm, NrFPath, NrFMid);
  switch (Access){
    case faCreate:{
      TFile::DelWc(BlobBsFNm+".*");
      TStr SegFNm=GetSegFNm(NrFPath, NrFMid, 0);
      PBlobBs Seg=TGBlobBs::New(SegFNm, faCreate, MxSegLen);
      SegV.Add(Seg);
      SaveMain(); break;}
    case faUpdate:
    case faRdOnly:{
      int Segs; LoadMain(Segs);
      for (int SegN=0; SegN<Segs; SegN++){
        TStr SegFNm=GetSegFNm(NrFPath, NrFMid, SegN);
        SegV.Add(TGBlobBs::New(SegFNm, Access, MxSegLen));
      }
      break;}
    case faRestore:{
      // assume no segments
      int Segs=-1;
      // if main-file exists
      if (TFile::Exists(GetMainFNm(NrFPath, NrFMid))){
        // load main file
        int _Segs; LoadMain(_Segs);
        // load segment-files which exist
        Segs=0;
        forever {
          // get segment file-name
          TStr SegFNm=GetSegFNm(NrFPath, NrFMid, Segs);
          // if segment-file exists then add segment else break check-loop
          if (TFile::Exists(SegFNm)){
            SegV.Add(TGBlobBs::New(SegFNm, Access, MxSegLen));
            Segs++;
          } else {
            break;
          }
        }
      }
      // if no segments exist then create blob-base from scratch
      if (Segs==-1){
        TStr SegFNm=GetSegFNm(NrFPath, NrFMid, 0);
        PBlobBs Seg=TGBlobBs::New(SegFNm, faCreate, MxSegLen);
        SegV.Add(Seg);
        SaveMain();
      }
      break;}
    default: Fail;
  }
}

TMBlobBs::~TMBlobBs(){
  if (Access!=faRdOnly){
    SaveMain();
  }
}

TBlobPt TMBlobBs::PutBlob(const PSIn& SIn){
  IAssert((Access==faCreate)||(Access==faUpdate)||(Access==faRestore));
  TBlobPt BlobPt=SegV[CurSegN]->PutBlob(SIn);
  if (BlobPt.Empty()){
    for (uchar SegN=0; SegN<SegV.Len(); SegN++){
      BlobPt=SegV[CurSegN=SegN]->PutBlob(SIn);
      if (!BlobPt.Empty()){break;}
    }
    if (BlobPt.Empty()){
      TStr SegFNm=GetSegFNm(NrFPath, NrFMid, SegV.Len());
      PBlobBs Seg=TGBlobBs::New(SegFNm, faCreate, MxSegLen);
      CurSegN=SegV.Add(Seg); IAssert(CurSegN<=255);
      BlobPt=SegV[CurSegN]->PutBlob(SIn);
    }
  }
  if (!BlobPt.Empty()){
    BlobPt.PutSeg(uchar(CurSegN));}
  return BlobPt;
}

TBlobPt TMBlobBs::PutBlob(const TBlobPt& BlobPt, const PSIn& SIn){
  IAssert((Access==faCreate)||(Access==faUpdate)||(Access==faRestore));
  int SegN=BlobPt.GetSeg();
  TBlobPt NewBlobPt=SegV[SegN]->PutBlob(BlobPt, SIn);
  if (NewBlobPt.Empty()){
    NewBlobPt=PutBlob(SIn);
  } else {
    NewBlobPt.PutSeg(BlobPt.GetSeg());
  }
  return NewBlobPt;
}

PSIn TMBlobBs::GetBlob(const TBlobPt& BlobPt){
  int SegN=BlobPt.GetSeg();
  return SegV[SegN]->GetBlob(BlobPt);
}

void TMBlobBs::DelBlob(const TBlobPt& BlobPt){
  int SegN=BlobPt.GetSeg();
  SegV[SegN]->DelBlob(BlobPt);
}

TBlobPt TMBlobBs::GetFirstBlobPt(){
  return SegV[0]->GetFirstBlobPt();
}

TBlobPt TMBlobBs::FFirstBlobPt(){
  return SegV[0]->FFirstBlobPt();
}

bool TMBlobBs::FNextBlobPt(TBlobPt& TrvBlobPt, TBlobPt& BlobPt, PSIn& BlobSIn){
  uchar SegN=TrvBlobPt.GetSeg();
  if (SegV[SegN]->FNextBlobPt(TrvBlobPt, BlobPt, BlobSIn)){
    TrvBlobPt.PutSeg(SegN);
    BlobPt.PutSeg(SegN);
    return true;
  } else
  if (SegN==SegV.Len()-1){
    return false;
  } else {
    SegN++;
    TrvBlobPt=SegV[SegN]->FFirstBlobPt();
    TrvBlobPt.PutSeg(SegN);
    return FNextBlobPt(TrvBlobPt, BlobPt, BlobSIn);
  }
}

bool TMBlobBs::Exists(const TStr& BlobBsFNm){
  TStr NrFPath; TStr NrFMid; GetNrFPathFMid(BlobBsFNm, NrFPath, NrFMid);
  TStr MainFNm=GetMainFNm(NrFPath, NrFMid);
  return TFile::Exists(MainFNm);
}
