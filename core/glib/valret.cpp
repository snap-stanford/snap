/////////////////////////////////////////////////
// Overloaded-Value-Retriever
TOvlValRet::TOvlValRet(const PDmHd& _DmHd, const PValRet& _ValRet):
  TValRet(_DmHd), ValRet(_ValRet), AttrToAttrV(){
  PDmHd NewDmHd=GetDmHd(); PDmHd OvlDmHd=ValRet->GetDmHd();
  for (int AttrN=0; AttrN<NewDmHd->GetAttrs(); AttrN++){
    int OvlAttrN=OvlDmHd->GetAttrN(NewDmHd->GetAttr(AttrN)->GetNm());
    Assert(*OvlDmHd->GetAttr(OvlAttrN) == *NewDmHd->GetAttr(AttrN));
    AttrToAttrV.Add(OvlAttrN);
  }
}

