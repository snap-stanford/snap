#ifndef MdTr_h
#define MdTr_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "dmdata.h"
#include "md.h"

/////////////////////////////////////////////////
// Decision-Node
class TDNd{
private:
  TCRef CRef;
public:
  TDNd(){}
  virtual ~TDNd(){}
  TDNd(TSIn&){}
  static TPt<TDNd> Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut);}

  virtual PTbValDs GetPostrValDs(const PValRet& ValRet) const=0;
  virtual void Print() const=0;

  friend TPt<TDNd>;
};
typedef TPt<TDNd> PDNd;

/////////////////////////////////////////////////
// Decision-Node-Leaf
class TDNdLeaf: public TDNd{
private:
  PMd Md;
public:
  TDNdLeaf(const PMd& _Md): TDNd(), Md(_Md){}
  TDNdLeaf(TSIn& SIn): TDNd(SIn), Md(SIn){}
  void Save(TSOut& SOut){TDNd::Save(SOut); Md.Save(SOut);}

  PTbValDs GetPostrValDs(const PValRet& ValRet) const;
  void Print() const {printf("Node-Leaf\n"); Md->Print();}
};

/////////////////////////////////////////////////
// Decision-Node-Attribute-Branch
class TDNdABr: public TDNd{
private:
  TInt AttrN;
  PTbValSet OrTbValSet;
  TVec<PDNd> DNdV;
public:
  TDNdABr(const int& _AttrN, const PTbValSet& _TbValSet):
    TDNd(), AttrN(_AttrN), OrTbValSet(_TbValSet), DNdV(){}
  TDNdABr(TSIn& SIn): TDNd(SIn), AttrN(SIn), OrTbValSet(SIn), DNdV(SIn){}
  void Save(TSOut& SOut){
    TDNd::Save(SOut); AttrN.Save(SOut); OrTbValSet.Save(SOut); DNdV.Save(SOut);}

  PTbValDs GetPostrValDs(const PValRet& ValRet) const;
  void Print() const;
};

/////////////////////////////////////////////////
// Model-Tree
class TMdTree: public TMd{
private:
  PDmHd DmHd;
  TInt ClassN;
  PDNd DNd;
public:
  TMdTree(const PDmHd& _DmHd, const int& _ClassN, const PDNd& _DNd):
    TMd(_DmHd), DmHd(_DmHd), ClassN(_ClassN), DNd(_DNd){Def();}
  TMdTree(TSIn& SIn): TMd(SIn), DmHd(SIn), ClassN(SIn), DNd(SIn){}
  void Save(TSOut& SOut){Assert(IsDef());
    TMd::Save(SOut); DmHd.Save(SOut); ClassN.Save(SOut); DNd.Save(SOut);}

  PDmHd GetDmHd() const {Assert(IsDef()); return DmHd;}
  int GetClassN() const {Assert(IsDef()); return ClassN;}
  
  PTbValDs GetPriorValDs(const int& /*ClassN*/) const {return NULL;}//**
  PTbValDs GetPostrValDs(const PValRet& ValRet, const int& CfClassN) const {
    Assert(IsDef()&&(ClassN==CfClassN)); return DNd->GetPostrValDs(ValRet);}
  void Print() const {Assert(IsDef()); printf("Tree\n"); DNd->Print();}
};

#endif

