#ifndef _xmlser_h
#define _xmlser_h

// !bn: pobrisan 'explicit' - je pa se v bd.h - a je prov tko?
template <class TRec>
TPt<TRec>::TPt(TSIn& SIn):
    Addr(NULL){TBool IsNull(SIn);
    if (!IsNull){TPt Pt=TRec::Load(SIn); Addr=Pt.Addr; MkRef();}}

template <class TRec>
TPt<TRec>::TPt(TSIn& SIn, void* ThisPt):
    Addr(NULL){TBool IsNull(SIn);
    if (!IsNull){TPt Pt=TRec::Load(SIn, ThisPt); Addr=Pt.Addr; MkRef();}}

template <class TRec>
void TPt<TRec>::Save(TSOut& SOut) const {
    if (Addr==NULL){TBool(true).Save(SOut);}
    else {TBool(false).Save(SOut); Addr->Save(SOut);}}

template <class TRec>
void TPt<TRec>::LoadXml(const TPt<TXmlTok>& XmlTok, const TStr& Nm){
  XLoadHd(Nm);
  bool NullP=TXmlObjSer::GetBoolArg(XmlTok, "Null");
  if (NullP){
    Addr=NULL;
  } else {
    Addr=new TRec(); MkRef();
    Addr->LoadXml(XmlTok, "-");
  }
}

template <class TRec>
void TPt<TRec>::SaveXml(TSOut& SOut, const TStr& Nm) const {
  if (Addr==NULL){
    XSaveBETagArg(Nm, "Null", TBool::GetStr(true));
  } else {
    XSaveHdArg(Nm, "Null", TBool::GetStr(false));
    Addr->SaveXml(SOut, "-");
  }
}

/////////////////////////////////////////////////
// Xml-Object-Serialization
template <class TRec>
bool IsXLoadFromFileOk(const TStr& FNm, const TStr& Nm, TRec& Rec, TStr& MsgStr){
  bool Ok=true;
  try {
    PXmlDoc XmlDoc=TXmlDoc::LoadTxt(FNm);
    if (XmlDoc->IsOk()){
      PXmlTok XmlTok=XmlDoc->GetTok();
      Rec.LoadXml(XmlTok, Nm);
    } else {
      Ok=false; MsgStr=XmlDoc->GetMsgStr();
    }
  }
  catch (PExcept Except){
    Ok=false; MsgStr=Except->GetMsgStr();
  }
  return Ok;
}

template <class TRec>
void XLoadFromFile(const TStr& FNm, const TStr& Nm, TRec& Rec){
  TStr MsgStr;
  if (!IsXLoadFromFileOk(FNm, Nm, Rec, MsgStr)){
    TExcept::Throw(MsgStr);
  }
}

template <class TVal1, class TVal2>
void TPair<TVal1, TVal2>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
	XLoadHd(Nm); XLoad(Val1); XLoad(Val2);}

template <class TVal1, class TVal2>
void TPair<TVal1, TVal2>::SaveXml(TSOut& SOut, const TStr& Nm) const {
	XSaveHd(Nm); XSave(Val1); XSave(Val2);}

template <class TVal1, class TVal2, class TVal3>
void TTriple<TVal1, TVal2, TVal3>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
	XLoadHd(Nm); XLoad(Val1); XLoad(Val2); XLoad(Val3);}

template <class TVal1, class TVal2, class TVal3>
void TTriple<TVal1, TVal2, TVal3>::SaveXml(TSOut& SOut, const TStr& Nm) const {
	XSaveHd(Nm); XSave(Val1); XSave(Val2); XSave(Val3);}

template <class TVal1, class TVal2, class TVal3, class TVal4>
void TQuad< TVal1, TVal2, TVal3, TVal4>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
	XLoadHd(Nm); XLoad(Val1); XLoad(Val2); XLoad(Val3); XLoad(Val4);}

template <class TVal1, class TVal2, class TVal3, class TVal4>
void TQuad< TVal1, TVal2, TVal3, TVal4>::SaveXml(TSOut& SOut, const TStr& Nm) const {
	XSaveHd(Nm); XSave(Val1); XSave(Val2); XSave(Val3); XSave(Val4);}

template <class TKey, class TDat>
void TKeyDat< TKey, TDat>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
	XLoadHd(Nm); XLoad(Key); XLoad(Dat);}

template <class TKey, class TDat>
void TKeyDat< TKey, TDat>::SaveXml(TSOut& SOut, const TStr& Nm) const {
	XSaveHd(Nm); XSave(Key); XSave(Dat);}

template <class TVal>
void TVec<TVal>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
  XLoadHd(Nm);
  int SubToks=XmlTok->GetSubToks(); Gen(SubToks, 0);
  for (int SubTokN=0; SubTokN<SubToks; SubTokN++){
    PXmlTok SubTok=XmlTok->GetSubTok(SubTokN);
    TVal Val; Val.LoadXml(SubTok, TStr()); Add(Val);
  }
}

template <class TVal>
void TVec<TVal>::SaveXml(TSOut& SOut, const TStr& Nm) const {
  XSaveHdArg(Nm, "Vals", TInt::GetStr(Vals));
  for (int ValN=0; ValN<Vals; ValN++){ValT[ValN].SaveXml(SOut, TStr());}
}

template <class TKey, class TDat>
void THashKeyDat< TKey, TDat>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
	XLoadHd(Nm); XLoad(Key); XLoad(Dat);}

template <class TKey, class TDat>
void THashKeyDat< TKey, TDat>::SaveXml(TSOut& SOut, const TStr& Nm) const {
	XSaveHd(Nm); XSave(Key); XSave(Dat);}

template<class TKey, class TDat, class THashFunc>
void THash< TKey, TDat, THashFunc>::LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
  XLoadHd(Nm); TVec<THashKeyDat<TKey, TDat> > KeyDatV; XLoad(KeyDatV); XLoad(AutoSizeP);
	for (int KeyDatN=0; KeyDatN<KeyDatV.Len(); KeyDatN++){
		AddDat(KeyDatV[KeyDatN].Key, KeyDatV[KeyDatN].Dat);}}

template<class TKey, class TDat, class THashFunc>
void THash< TKey, TDat, THashFunc>::SaveXml(TSOut& SOut, const TStr& Nm){
  Defrag(); XSaveHd(Nm); XSave(KeyDatV); XSave(AutoSizeP);}

#endif
