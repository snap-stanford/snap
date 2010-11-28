/////////////////////////////////////////////////
// Includes
#include "mdtr.h"

/////////////////////////////////////////////////
// Decision-Node
PDNd TDNd::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TDNdLeaf>()){return new TDNdLeaf(SIn);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Decision-Node-Leaf
PTbValDs TDNdLeaf::GetPostrValDs(const PValRet& ValRet) const {
  return Md->GetPostrValDs(PValRet(new TOvlValRet(Md->GetDmHd(), ValRet)));}

/////////////////////////////////////////////////
// Decision-Node-Attribute-Branch
PTbValDs TDNdABr::GetPostrValDs(const PValRet& ValRet) const {
  return DNdV[OrTbValSet->InN(ValRet->GetVal(AttrN))]->GetPostrValDs(ValRet);}

void TDNdABr::Print() const {
  printf("Node-AttrBr\n");
  for (int BrN=0; BrN<DNdV.Len(); BrN++){DNdV[BrN]->Print();}
}

