#ifndef MGRAPH_H
#define MGRAPH_H

#include "network.h"
#include "networkmp.h"

template<class TNode>
class TMNet;

class TSVNode {
private:
  TInt TypeId;
  TInt Id;
  TVec<TIntV > InEIdVV, OutEIdVV;
  TInt InDeg, OutDeg;
public:
  TSVNode() : TypeId(-1), Id(-1), InEIdVV(), OutEIdVV(), InDeg(0), OutDeg(0) { }
  TSVNode(const int& NTypeId, const int& NId) : TypeId(NTypeId), Id(NId), InEIdVV(), OutEIdVV(), InDeg(0), OutDeg(0) { }
  TSVNode(const TSVNode& Node) : TypeId(Node.TypeId), Id(Node.Id), InEIdVV(Node.InEIdVV), OutEIdVV(Node.OutEIdVV), InDeg(Node.InDeg), OutDeg(Node.OutDeg) { }
  TSVNode(const TSVNode& Node, const TIntV& InETypeIdV, const TIntV& OutETypeIdV) :
                TypeId(Node.TypeId), Id(Node.Id), InEIdVV(Node.InEIdVV.Len()), OutEIdVV(Node.OutEIdVV.Len()),
                InDeg(0), OutDeg(0) {
    for (int i = 0; i < InETypeIdV.Len(); i++) {
      int ETypeId = InETypeIdV[i];
      InEIdVV[ETypeId] = Node.InEIdVV[ETypeId];
      InDeg += Node.InEIdVV[ETypeId].Len();
    }
    for (int i = 0; i < OutETypeIdV.Len(); i++) {
      int ETypeId = OutETypeIdV[i];
      OutEIdVV[ETypeId] = Node.OutEIdVV[ETypeId];
      OutDeg += Node.OutEIdVV[ETypeId].Len();
    }
  }
  TSVNode(TSIn& SIn) : TypeId(SIn), Id(SIn), InEIdVV(SIn), OutEIdVV(SIn), InDeg(0), OutDeg(0) { }
  void Save(TSOut& SOut) const { TypeId.Save(SOut); Id.Save(SOut); InEIdVV.Save(SOut); OutEIdVV.Save(SOut); InDeg.Save(SOut); OutDeg.Save(SOut); }
  int GetTypeId() const { return TypeId; }
  int GetId() const { return Id; }
  int GetDeg() const { return GetInDeg() + GetOutDeg(); }
  int GetInDeg(const int& ETypeId) const {return InEIdVV[ETypeId].Len();}
  int GetInDeg() const { return InDeg; }
  int GetOutDeg(int ETypeId) const {return OutEIdVV[ETypeId].Len();}
  int GetOutDeg() const { return OutDeg; }
  void AddInETypeIds(const TIntV& ETypeIds) {
    int MxETypeId = -1;
    for (int i = 0; i < ETypeIds.Len(); i++) {
      if (MxETypeId < ETypeIds[i]) { MxETypeId = ETypeIds[i]; }
    }
    InEIdVV.Reserve(MxETypeId+1, MxETypeId+1);
    for (int i = 0; i < ETypeIds.Len(); i++) {
      InEIdVV[ETypeIds[i]] = TIntV();
    }
  }
  void AddOutETypeIds(const TIntV& ETypeIds) {
    int MxETypeId = -1;
    for (int i = 0; i < ETypeIds.Len(); i++) {
      if (MxETypeId < ETypeIds[i]) { MxETypeId = ETypeIds[i]; }
    }
    OutEIdVV.Reserve(MxETypeId+1, MxETypeId+1);
    for (int i = 0; i < ETypeIds.Len(); i++) {
      OutEIdVV[ETypeIds[i]] = TIntV();
    }
  }
  void AddInNbr(const int& ETypeId, const int& EId) { InEIdVV[ETypeId].Add(EId); InDeg++; }
  void AddOutNbr(const int& ETypeId, const int& EId) { OutEIdVV[ETypeId].Add(EId); OutDeg++; }
  void DelInNbr(const int& ETypeId, const int& EId) { InEIdVV[ETypeId].DelIfIn(EId); InDeg--; }
  void DelOutNbr(const int& ETypeId, const int& EId) { OutEIdVV[ETypeId].DelIfIn(EId); OutDeg--; }
  int GetInEId(const int& EdgeN) const {
    int CumSum = 0;
    int ETypeId = 0;
    for (; ETypeId < InEIdVV.Len(); ETypeId++) {
      CumSum += InEIdVV[ETypeId].Len();
      if (CumSum > EdgeN) { CumSum -= InEIdVV[ETypeId].Len(); break; }
    }
    return InEIdVV[ETypeId][EdgeN-CumSum];
  }
  int GetOutEId(const int& EdgeN) const {
    int CumSum = 0;
    int ETypeId = 0;
    for (; ETypeId < OutEIdVV.Len(); ETypeId++) {
      CumSum += OutEIdVV[ETypeId].Len();
      if (CumSum > EdgeN) { CumSum -= OutEIdVV[ETypeId].Len(); break; }
    }
    return OutEIdVV[ETypeId][EdgeN-CumSum];
  }
  void GetInEIdV(TIntV& EIdV) const {
    EIdV.Gen(InDeg, 0);
    for (int i = 0; i < InEIdVV.Len(); i++) {
      EIdV.AddV(InEIdVV[i]);
    }
  }
  void GetOutEIdV(TIntV& EIdV) const {
    EIdV.Gen(OutDeg, 0);
    for (int i = 0; i < OutEIdVV.Len(); i++) {
      EIdV.AddV(OutEIdVV[i]);
    }
  }
  void GetInEIdV(const TInt ETypeId, TIntV& EIdV) const {
    EIdV = InEIdVV[ETypeId.Val];
  }
  void GetOutEIdV(const TInt ETypeId, TIntV& EIdV) const {
    EIdV = OutEIdVV[ETypeId.Val];
  }
  void GetInEIdV(const TIntV& ETypeIdV, TIntV& EIdV) const {
    EIdV.Reserve(InDeg, 0);
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      EIdV.AddV(InEIdVV[ETypeIdV[k].Val]);
    }
  }
  void GetOutEIdV(const TIntV& ETypeIdV, TIntV& EIdV) const {
    EIdV.Reserve(OutDeg, 0);
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      EIdV.AddV(OutEIdVV[ETypeIdV[k].Val]);
    }
  }
  friend class TMNet<TSVNode>;
};

class TMVNode {
private:
  TInt TypeId; // Node type ID
  TInt Id; // Get global ID
  TIntV InEIdV, OutEIdV; // Vectors of EIds
  TIntV InETypeIdV, OutETypeIdV; // Vectors of ETypeIds
public:
  TMVNode() : TypeId(-1), Id(-1), InEIdV(), OutEIdV(), InETypeIdV(), OutETypeIdV() { }
  TMVNode(const int& NTypeId, const int& NId) : TypeId(NTypeId), Id(NId), InEIdV(), OutEIdV(), InETypeIdV(), OutETypeIdV() { }
  TMVNode(const TMVNode& Node) : TypeId(Node.TypeId), Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV),
                                        InETypeIdV(Node.InETypeIdV), OutETypeIdV(Node.OutETypeIdV) { }
  TMVNode(const TMVNode& Node, const TIntV& InETypeIdV, const TIntV& OutETypeIdV) :
            TypeId(Node.TypeId), Id(Node.Id), InEIdV(Node.InEIdV.Len()), OutEIdV(Node.OutEIdV.Len()),
            InETypeIdV(Node.InETypeIdV.Len()), OutETypeIdV(Node.OutETypeIdV.Len()) {
    TIntSet InETypeIdSet(InETypeIdV);
    for (int i = 0; i < Node.InEIdV.Len(); i++) {
      if (InETypeIdSet.IsKey(Node.InETypeIdV[i])) {
        InEIdV.Add(Node.InEIdV[i]);
      }
    }
    TIntSet OutETypeIdSet(OutETypeIdV);
    for (int i = 0; i < Node.OutEIdV.Len(); i++) {
      if (OutETypeIdSet.IsKey(Node.OutETypeIdV[i])) {
        OutEIdV.Add(Node.OutEIdV[i]);
      }
    }
  }
  TMVNode(TSIn& SIn) : TypeId(SIn), Id(SIn), InEIdV(SIn), OutEIdV(SIn), InETypeIdV(SIn), OutETypeIdV(SIn) { }
  void Save(TSOut& SOut) const { TypeId.Save(SOut); Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut);
                                  InETypeIdV.Save(SOut); OutETypeIdV.Save(SOut); }
  int GetTypeId() const { return TypeId; }
  int GetId() const { return Id; }
  int GetDeg() const { return GetInDeg() + GetOutDeg(); }
  int GetInDeg() const { return InEIdV.Len(); }
  int GetOutDeg() const { return OutEIdV.Len(); }
  int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
  int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
  int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
  void GetInEIdV(TIntV& EIdV) const { EIdV = InEIdV; }
  void GetOutEIdV(TIntV& EIdV) const { EIdV = OutEIdV; }
  bool IsInEId(const int& EId) const { return InEIdV.SearchForw(EId) != -1; }
  bool IsOutEId(const int& EId) const { return OutEIdV.SearchForw(EId) != -1; }
  void AddInETypeIds(const TIntV& ETypeIds) { } // Do nothing.
  void AddOutETypeIds(const TIntV& ETypeIds) { } // Do nothing.
  void AddInNbr(const int& ETypeId, const int& EId) {
    InETypeIdV.Add(ETypeId);
    InEIdV.Add(EId);
  }
  void AddOutNbr(const int& ETypeId, const int& EId) {
    OutETypeIdV.Add(ETypeId);
    OutEIdV.Add(EId);
  }
  void DelInNbr(const int& ETypeId, const int& EId) {
    int EIdN = InEIdV.SearchBack(EId);
    InETypeIdV.Del(EIdN);
    InEIdV.Del(EIdN);
  }
  void DelOutNbr(const int& ETypeId, const int& EId) {
    int EIdN = OutEIdV.SearchBack(EId);
    OutETypeIdV.Del(EIdN);
    OutEIdV.Del(EIdN);
  }
  void GetInEIdV(const TInt& ETypeId, TIntV& EIdV) const {
    EIdV.Reduce(0); // Clear
    for (int i = 0; i < InEIdV.Len(); i++) {
      if (InETypeIdV[i] == ETypeId) { EIdV.Add(InEIdV[i]); }
    }
  }
  void GetOutEIdV(const TInt& ETypeId, TIntV& EIdV) const {
    EIdV.Reduce(0); // Clear
    for (int i = 0; i < OutEIdV.Len(); i++) {
      if (OutETypeIdV[i] == ETypeId) { EIdV.Add(OutEIdV[i]); }
    }
  }
  void GetInEIdV(const TIntV& ETypeIdV, TIntV& EIdV) const {
    EIdV.Reserve(InEIdV.Len(), 0);
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      TInt ETypeId = ETypeIdV[k];
      for (int i = 0; i < InETypeIdV.Len(); i++) {
        if (InETypeIdV[i] == ETypeId) { EIdV.Add(InEIdV[i]); }
      }
    }
  }
  void GetOutEIdV(const TIntV& ETypeIdV, TIntV& EIdV) const {
    EIdV.Reserve(OutEIdV.Len(), 0);
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      TInt ETypeId = ETypeIdV[k];
      for (int i = 0; i < OutETypeIdV.Len(); i++) {
        if (OutETypeIdV[i] == ETypeId) { EIdV.Add(OutEIdV[i]); }
      }
    }
  }
  friend class TMNet<TMVNode>;
};

class TCVNode {
public:
  static const int DEF_WEIGHT;
  static const int DEF_WEIGHT_COEFF;
  static const int DEF_EXPAND_RATIO;
private:
  static void RedistributeEIds(const TIntV& Weights, TIntV& EIdV, TIntV& TypeIndexV, TIntV& TypeDegV) {
    IAssertR(TypeIndexV.Len() == TypeDegV.Len(), TStr::Fmt("The node is in inconsistent state."));
    // Get new TypeIndex
    int NTypes = Weights.Len();
    TIntV NewTypeIndexV(NTypes); // number of types
    int CumSum = 0; // cumulative sum of weights
    for (int ETypeId = 0; ETypeId < NTypes; ETypeId++) {
      NewTypeIndexV[ETypeId] = CumSum;
      CumSum += Weights[ETypeId] * DEF_WEIGHT_COEFF;
    }
    TIntV NewEIdV(CumSum);
    // Copy data from old positions to new positions
    for (int ETypeId = TypeIndexV.Len() - 1; ETypeId >= 0; ETypeId--) {
      IAssertR(CumSum >= NewTypeIndexV[ETypeId] + TypeDegV[ETypeId], TStr::Fmt("The node is in inconsistent state."));
      for (int i = 0; i < TypeDegV[ETypeId]; i++) {
        NewEIdV[NewTypeIndexV[ETypeId] + i] = EIdV[TypeIndexV[ETypeId] + i];
      }
    }
    TypeDegV.Reserve(NTypes, NTypes);
    TypeIndexV = NewTypeIndexV;
    EIdV = NewEIdV;
  }
private:
  TInt TypeId; // Node type ID
  TInt Id; // Get global ID
  TIntV InEIdV, OutEIdV;
  TInt InDeg, OutDeg;
  TIntV InTypeIndexV, OutTypeIndexV;
  TIntV InTypeDegV, OutTypeDegV;
public:
  TCVNode() : TypeId(-1), Id(-1), InEIdV(), OutEIdV(), InDeg(0), OutDeg(0),
                                    InTypeIndexV(), OutTypeIndexV(), InTypeDegV(), OutTypeDegV() { }
  TCVNode(const int& NTypeId, const int& NId) : TypeId(NTypeId), Id(NId), InEIdV(), OutEIdV(), InDeg(0), OutDeg(0),
                                    InTypeIndexV(), OutTypeIndexV(), InTypeDegV(), OutTypeDegV() { }
  TCVNode(const TCVNode& Node) : TypeId(Node.TypeId), Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV),
                                    InDeg(Node.InDeg), OutDeg(Node.OutDeg), InTypeIndexV(Node.InTypeIndexV), OutTypeIndexV(Node.OutTypeIndexV),
                                    InTypeDegV(Node.InTypeDegV), OutTypeDegV(Node.OutTypeDegV) { }
  TCVNode(const TCVNode& Node, const TIntV& InETypeIdV, const TIntV& OutETypeIdV) :
                                    TypeId(Node.TypeId), Id(Node.Id), InDeg(0), OutDeg(0),
                                    InTypeIndexV(Node.InTypeIndexV.Len()), OutTypeIndexV(Node.OutTypeIndexV.Len()),
                                    InTypeDegV(Node.InTypeDegV.Len()), OutTypeDegV(Node.OutTypeDegV.Len()) {
    for (TIntV::TIter iter = InETypeIdV.BegI(); iter < InETypeIdV.EndI(); iter++) {
      InDeg += Node.InTypeDegV[*iter];
      InTypeDegV[*iter] = Node.InTypeDegV[*iter];
    }
    int index = 0;
    InEIdV.Gen(InDeg);
    TIntSet InETypeIdSet(InETypeIdV);
    for (int ETypeId = 0; ETypeId < InTypeIndexV.Len(); ETypeId++) {
      InTypeIndexV[ETypeId] = index;
      if (InETypeIdSet.IsKey(ETypeId)) {
        for (int i = Node.InTypeIndexV[ETypeId]; i < Node.InTypeIndexV[ETypeId] + Node.InTypeDegV[ETypeId]; i++) {
          InEIdV[index++] = Node.InEIdV[i];
        }
      }
    }
    IAssert(index == InDeg);

    for (TIntV::TIter iter = OutETypeIdV.BegI(); iter < OutETypeIdV.EndI(); iter++) {
      OutDeg += Node.OutTypeDegV[*iter];
      OutTypeDegV[*iter] = Node.OutTypeDegV[*iter];
    }
    index = 0;
    OutEIdV.Gen(OutDeg);
    TIntSet OutETypeIdSet(OutETypeIdV);
    for (int ETypeId = 0; ETypeId < OutTypeIndexV.Len(); ETypeId++) {
      OutTypeIndexV[ETypeId] = index;
      if (OutETypeIdSet.IsKey(ETypeId)) {
        for (int i = Node.OutTypeIndexV[ETypeId]; i < Node.OutTypeIndexV[ETypeId] + Node.OutTypeDegV[ETypeId]; i++) {
          OutEIdV[index++] = Node.OutEIdV[i];
        }
      }
    }
    IAssert(index == OutDeg);
  }
  TCVNode(TSIn& SIn) : TypeId(SIn), Id(SIn), InEIdV(SIn), OutEIdV(SIn), InDeg(SIn), OutDeg(SIn),
                                    InTypeIndexV(SIn), OutTypeIndexV(SIn), InTypeDegV(SIn), OutTypeDegV(SIn) { }
  void Save(TSOut& SOut) const { TypeId.Save(SOut); Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut);
                                    InDeg.Save(SOut); OutDeg.Save(SOut); InTypeIndexV.Save(SOut); OutTypeIndexV.Save(SOut);
                                    InTypeDegV.Save(SOut); OutTypeDegV.Save(SOut); }
  int GetTypeId() const { return TypeId; }
  int GetId() const { return Id; }
  int GetDeg() const { return InDeg + OutDeg; }
  int GetInDeg() const { return InDeg; }
  int GetOutDeg() const { return OutDeg; }
  int GetInDeg(const int& ETypeId) const { return InTypeDegV[ETypeId]; }
  int GetOutDeg(const int& ETypeId) const { return OutTypeDegV[ETypeId]; }
  int GetInEId(const int& EdgeN) const {
    int CumSum = 0;
    int ETypeId = 0;
    for (; ETypeId < InTypeDegV.Len(); ETypeId++) {
      CumSum += InTypeDegV[ETypeId];
      if (CumSum > EdgeN) { CumSum -= InTypeDegV[ETypeId]; break; }
    }
    return InEIdV[InTypeIndexV[ETypeId] + EdgeN - CumSum];
  }
  int GetOutEId(const int& EdgeN) const {
    int CumSum = 0;
    int ETypeId = 0;
    for (; ETypeId < OutTypeDegV.Len(); ETypeId++) {
      CumSum += OutTypeDegV[ETypeId];
      if (CumSum > EdgeN) { CumSum -= OutTypeDegV[ETypeId]; break; }
    }
    return OutEIdV[OutTypeIndexV[ETypeId] + EdgeN - CumSum];
  }
  int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
  void GetInEIdV(TIntV& EIdV) const {
    EIdV.Gen(InDeg, 0);
    for (int ETypeId = 0; ETypeId < InTypeDegV.Len(); ETypeId++) {
      for (int i = InTypeIndexV[ETypeId]; i < InTypeIndexV[ETypeId] + InTypeDegV[ETypeId]; i++) {
        EIdV.Add(InEIdV[i]);
      }
    }
  }
  void GetOutEIdV(TIntV& EIdV) const {
    EIdV.Gen(OutDeg, 0);
    for (int ETypeId = 0; ETypeId < OutTypeDegV.Len(); ETypeId++) {
      for (int i = OutTypeIndexV[ETypeId]; i < OutTypeIndexV[ETypeId] + OutTypeDegV[ETypeId]; i++) {
        EIdV.Add(OutEIdV[i]);
      }
    }
  }
  //bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
  //bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
  void AddInETypeIds(const TIntV& ETypeIds) {
    if (ETypeIds.Len() == 0) { return; }

    int MxETypeId = InTypeIndexV.Len() - 1;
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      if (MxETypeId < *iter) { MxETypeId = *iter; }
    }
    TIntV InWeights(MxETypeId + 1);
    for (int ETypeId = 0; ETypeId < InTypeDegV.Len(); ETypeId++) {
      InWeights[ETypeId] = InTypeDegV[ETypeId];
    }
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      InWeights[*iter] = DEF_WEIGHT;
    }
    RedistributeEIds(InWeights, InEIdV, InTypeIndexV, InTypeDegV);
  }
  void AddOutETypeIds(const TIntV& ETypeIds) {
    if (ETypeIds.Len() == 0) { return; }

    int MxETypeId = OutTypeIndexV.Len() - 1;
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      if (MxETypeId < *iter) { MxETypeId = *iter; }
    }
    TIntV OutWeights(MxETypeId + 1);
    for (int ETypeId = 0; ETypeId < OutTypeDegV.Len(); ETypeId++) {
      OutWeights[ETypeId] = OutTypeDegV[ETypeId];
    }
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      OutWeights[*iter] = DEF_WEIGHT;
    }
    RedistributeEIds(OutWeights, OutEIdV, OutTypeIndexV, OutTypeDegV);
  }
  void AddInNbr(const int& ETypeId, const int& EId) {
    int Deg = InTypeDegV[ETypeId];
    int Capacity = (ETypeId == (InTypeIndexV.Len()-1)) ? InEIdV.Len() : InTypeIndexV[ETypeId+1].Val;
    Capacity -= InTypeIndexV[ETypeId];
    if (Deg >= Capacity) {
      IAssertR(Deg == Capacity, TStr::Fmt("The node is in inconsistent state."));
      TIntV Weights(InTypeDegV);
      Weights[ETypeId] = (Weights[ETypeId] + 4) * DEF_EXPAND_RATIO;
      RedistributeEIds(Weights, InEIdV, InTypeIndexV, InTypeDegV);
    }
    InEIdV[InTypeIndexV[ETypeId] + Deg] = EId;
    InTypeDegV[ETypeId]++;
    InDeg++;
  }
  void AddOutNbr(const int& ETypeId, const int& EId) {
    int Deg = OutTypeDegV[ETypeId];
    int Capacity = (ETypeId == (OutTypeIndexV.Len()-1)) ? OutEIdV.Len() : OutTypeIndexV[ETypeId+1].Val;
    Capacity -= OutTypeIndexV[ETypeId];
    if (Deg >= Capacity) {
      IAssertR(Deg == Capacity, TStr::Fmt("The node is in inconsistent state."));
      TIntV Weights(OutTypeDegV);
      Weights[ETypeId] = (Weights[ETypeId] + 4) * DEF_EXPAND_RATIO; // + 4 to avoid 0
      RedistributeEIds(Weights, OutEIdV, OutTypeIndexV, OutTypeDegV);
    }
    OutEIdV[OutTypeIndexV[ETypeId] + Deg] = EId;
    OutTypeDegV[ETypeId]++;
    OutDeg++;
  }
  /// Delete an edge with ID EId and type ETypeId.
  void DelInNbr(const int& ETypeId, const int& EId) {
    int ValN = InEIdV.SearchForw(EId, InTypeIndexV[ETypeId]);
    for (int MValN=ValN+1; MValN<InTypeIndexV[ETypeId]+InTypeDegV[ETypeId]; MValN++){
      InEIdV[MValN-1]=InEIdV[MValN];
    }
    InDeg--;
    InTypeDegV[ETypeId]--;
  }
  void DelOutNbr(const int& ETypeId, const int& EId) {
    int ValN = OutEIdV.SearchForw(EId, OutTypeIndexV[ETypeId]);
    for (int MValN=ValN+1; MValN<OutTypeIndexV[ETypeId]+OutTypeDegV[ETypeId]; MValN++){
      OutEIdV[MValN-1]=OutEIdV[MValN];
    }
    OutDeg--;
    OutTypeDegV[ETypeId]--;
  }
  void GetInEIdV(const TInt& ETypeId, TIntV& EIdV) const {
    int Sz = InTypeDegV[ETypeId].Val;
    EIdV.Reserve(Sz, Sz);
    int Ind = InTypeIndexV[ETypeId].Val;
    for (int i = 0; i < Sz; i++) {
      EIdV[i] = InEIdV[Ind+i];
    }
  }
  void GetOutEIdV(const TInt& ETypeId, TIntV& EIdV) const {
    int Sz = OutTypeDegV[ETypeId].Val;
    EIdV.Reserve(Sz, Sz);
    int Ind = OutTypeIndexV[ETypeId].Val;
    for (int i = 0; i < Sz; i++) {
      EIdV[i] = OutEIdV[Ind+i];
    }
  }
  void GetInEIdV(const TIntV& ETypeIdV, TIntV& EIdV) const {
    int Sz = 0;
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      Sz += InTypeDegV[ETypeIdV[k]].Val;
    }
    EIdV.Reserve(Sz, 0);
    int Ind;
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      Ind = InTypeIndexV[ETypeIdV[k]].Val;
      for (int i = 0; i < InTypeDegV[ETypeIdV[k]]; i++) {
        EIdV.Add(InEIdV[Ind]);
      }
    }
  }
  void GetOutEIdV(const TIntV& ETypeIdV, TIntV& EIdV) const {
    int Sz = 0;
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      Sz += OutTypeDegV[ETypeIdV[k]].Val;
    }
    EIdV.Reserve(Sz, 0);
    int Ind;
    for (int k = 0; k < ETypeIdV.Len(); k++) {
      Ind = OutTypeIndexV[ETypeIdV[k]].Val;
      for (int i = 0; i < OutTypeDegV[ETypeIdV[k]]; i++) {
        EIdV.Add(OutEIdV[Ind]);
      }
    }
  }
  friend class TMNet<TCVNode>;
};

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes.
template<class TNode>
class TMNet {
public:
  typedef TMNet TNet;
  typedef TPt<TMNet> PNet;
public:
  class TEdge {
  private:
    TInt TypeId, Id, SrcNId, DstNId;
  public:
    TEdge() : TypeId(-1), Id(-1), SrcNId(-1), DstNId(-1) { }
    TEdge(const int& ETypeId, const int& EId, const int& SourceNId, const int& DestNId) : TypeId(ETypeId), Id(EId), SrcNId(SourceNId), DstNId(DestNId) { }
    TEdge(const TEdge& Edge) : TypeId(Edge.TypeId), Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId) { }
    TEdge(TSIn& SIn) : TypeId(SIn), Id(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { TypeId.Save(SOut), Id.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetTypeId() const { return TypeId; }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    friend class TMNet;
  };

  class TNodeType {
  private:
    TInt Id;
    TStr Name;
    TInt MxNId;
    THash<TInt, TNode> NodeH;
  public:
    TNodeType() : Id(-1), Name(), MxNId(0), NodeH(){ }
    TNodeType(const int& NTypeId, const TStr& NTypeName) : Id(NTypeId), Name(NTypeName), MxNId(0), NodeH(){ }
    TNodeType(const TNodeType& NodeType) : Id(NodeType.Id), Name(NodeType.Name), MxNId(NodeType.MxNId), NodeH(NodeType.NodeH) { }
    TNodeType(const TNodeType& NodeType, const TIntV& InETypeIdV, const TIntV& OutETypeIdV) :
              Id(NodeType.Id), Name(NodeType.Name), MxNId(NodeType.MxNId), NodeH(NodeType.NodeH.Len()) {
      for (typename THash<TInt,TNode>::TIter iter = NodeType.NodeH.BegI(); iter < NodeType.NodeH.EndI(); iter++) {
        TNode NewNode(iter.GetDat(), InETypeIdV, OutETypeIdV);
        NodeH.AddDat(iter.GetKey(), NewNode);
      }
    }
    TNodeType(TSIn& SIn) : Id(SIn), Name(SIn), MxNId(SIn), NodeH(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); Name.Save(SOut); MxNId.Save(SOut); NodeH.Save(SOut); }
    int GetId() const { return Id; }
    TStr GetName() const { return Name; }
    int GetMxNId() const { return MxNId; }
    friend class TMNet;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  template<class TEdge>
  class TMNodeI {
  private:
    typedef typename THash<TInt, TNode>::TIter THashIter;
    typedef typename TVec<TNodeType>::TIter TTypeIter;
    TTypeIter VecI;
    TTypeIter VecEndI;
    THashIter HashI;
    const TMNet *Graph;
  private:
    THashIter VecElemBegI() {
      return (*VecI).NodeH.BegI();
    }
    void FindNextNonEmptyHashI() {
      while (HashI.IsEnd() && VecI < VecEndI) {
        VecI++;
        HashI = VecElemBegI();
      }
    }
  public:
    TMNodeI() : VecI(), VecEndI(), HashI(), Graph(NULL) { }
    TMNodeI(const TTypeIter& TypeIter, const THashIter& NodeIter, const TMNet* GraphPt)
              : VecI(TypeIter), VecEndI(GraphPt->TypeNodeV.EndI()), HashI(NodeIter), Graph(GraphPt) { }
    TMNodeI(const TTypeIter& TypeIter, const TMNet* GraphPt) : VecI(TypeIter), VecEndI(GraphPt->TypeNodeV.EndI()), Graph(GraphPt) {
      if (VecI < VecEndI) {
        HashI = VecElemBegI();
        FindNextNonEmptyHashI();
      } else {
        HashI = THashIter();
      }
    }
    TMNodeI(const TMNodeI& NodeI) : VecI(NodeI.VecI), VecEndI(NodeI.VecEndI), HashI(NodeI.HashI), Graph(NodeI.Graph) { }
    TMNodeI& operator = (const TMNodeI& NodeI) { VecI=NodeI.VecI; VecEndI=NodeI.VecEndI; HashI=NodeI.HashI; Graph=NodeI.Graph; return *this; }
    /// Increment iterator.
    TMNodeI& operator++ (int) {
      HashI++;
      FindNextNonEmptyHashI();
      return *this;
    }
    bool operator < (const TMNodeI& NodeI) const { return VecI < NodeI.VecI || HashI < NodeI.HashI; }
    bool operator == (const TMNodeI& NodeI) const { return VecI == NodeI.VecI && HashI == NodeI.HashI; }

    /// Returns ID of the current node.
    int GetId() const { return HashI.GetDat().GetId(); }
    /// Returns the type-wise ID of the current node.
    int GetLocalId() const { return TMNet::GetLocalNId(GetId()); }
    /// Returns type ID of the current node.
    int GetTypeId() const { return HashI.GetDat().GetTypeId(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return HashI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return HashI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return HashI.GetDat().GetOutDeg(); }
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node).
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(HashI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to).
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(HashI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node.
    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(HashI.GetDat().GetNbrEId(EdgeN)); return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const {
      const TNode& Node = HashI.GetDat();
      for (int edge = 0; edge < Node.GetInDeg(); edge++) {
        if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId()) { return true; }
      }
      return false;
    }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const {
      const TNode& Node = HashI.GetDat();
      for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
        if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId()) { return true; }
      }
      return false;
    }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    /// Returns ID of EdgeN-th in-edge.
    int GetInEId(const int& EdgeN) const { return HashI.GetDat().GetInEId(EdgeN); }
    /// Returns ID of EdgeN-th out-edge.
    int GetOutEId(const int& EdgeN) const { return HashI.GetDat().GetOutEId(EdgeN); }
    /// Returns ID of EdgeN-th in or out-edge.
    int GetNbrEId(const int& EdgeN) const { return HashI.GetDat().GetNbrEId(EdgeN); }
    /// Tests whether the edge with ID EId is an in-edge of current node.
    bool IsInEId(const int& EId) const { return HashI.GetDat().IsInEId(EId); }
    /// Tests whether the edge with ID EId is an out-edge of current node.
    bool IsOutEId(const int& EId) const { return HashI.GetDat().IsOutEId(EId); }
    /// Tests whether the edge with ID EId is an in or out-edge of current node.
    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    /*
    /// Gets vector of attribute names.
    void GetAttrNames(TStrV& Names) const { Graph->AttrNameNI(GetId(), Names); }
    /// Gets vector of attribute values.
    void GetAttrVal(TStrV& Val) const { Graph->AttrValueNI(GetId(), Val); }
    /// Gets vector of int attribute names.
    void GetIntAttrNames(TStrV& Names) const { Graph->IntAttrNameNI(GetId(), Names); }
    /// Gets vector of int attribute values.
    void GetIntAttrVal(TIntV& Val) const { Graph->IntAttrValueNI(GetId(), Val); }
    /// Gets vector of str attribute names.
    void GetStrAttrNames(TStrV& Names) const { Graph->StrAttrNameNI(GetId(), Names); }
    /// Gets vector of str attribute values.
    void GetStrAttrVal(TStrV& Val) const { Graph->StrAttrValueNI(GetId(), Val); }
    /// Gets vector of flt attribute names.
    void GetFltAttrNames(TStrV& Names) const { Graph->FltAttrNameNI(GetId(), Names); }
    /// Gets vector of flt attribute values.
    void GetFltAttrVal(TFltV& Val) const { Graph->FltAttrValueNI(GetId(), Val); }
    */
  };

  typedef TMNodeI<TEdge> TNodeI;

  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef typename THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TMNet *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TMNet *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Returns edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Returns edge's type ID
    int GetTypeId() const { return EdgeHI.GetDat().GetTypeId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    /*
    /// Gets vector of attribute names.
    void GetAttrNames(TStrV& Names) const { Graph->AttrNameEI(GetId(), Names); }
    /// Gets vector of attribute values.
    void GetAttrVal(TStrV& Val) const { Graph->AttrValueEI(GetId(), Val); }
    /// Gets vector of int attribute names.
    void GetIntAttrNames(TStrV& Names) const { Graph->IntAttrNameEI(GetId(), Names); }
    /// Gets vector of int attribute values.
    void GetIntAttrVal(TIntV& Val) const { Graph->IntAttrValueEI(GetId(), Val); }
    /// Gets vector of str attribute names.
    void GetStrAttrNames(TStrV& Names) const { Graph->StrAttrNameEI(GetId(), Names); }
    /// Gets vector of str attribute values.
    void GetStrAttrVal(TStrV& Val) const { Graph->StrAttrValueEI(GetId(), Val); }
    /// Gets vector of flt attribute names.
    void GetFltAttrNames(TStrV& Names) const { Graph->FltAttrNameEI(GetId(), Names); }
    /// Gets vector of flt attribute values.
    void GetFltAttrVal(TFltV& Val) const { Graph->FltAttrValueEI(GetId(), Val); }
    */
    friend class TMNet;
  };

private:
  static const int NTYPEID_NBITS = 3; // The number of types must be at most 2^NTYPEID_NBITS
  static const int NTYPEID_FLAG = (1 << NTYPEID_NBITS) - 1;
  static int GetGlobalNId(const int& NTypeId, const int& NId) { return (NId << NTYPEID_NBITS) + NTypeId;}

private:
  TCRef CRef;
  TInt MxNId;
  TInt MxEId;
  THash<TStr, int> NTypeH;
  THash<TStr, int> ETypeH;
  TVec<TNodeType> TypeNodeV;
  TIntV EdgeSzV; // maintain the number of edges of each type
  THash<TInt, TEdge> EdgeH;
  int Sz;
  TVec<TIntV> InETypes;
  TVec<TIntV> OutETypes;
  /// KeyToIndexType[N|E]: Key->(Type,Index).
  TStrIntPrH KeyToIndexTypeN, KeyToIndexTypeE;
  enum { IntType, StrType, FltType };

private:
  TNode& GetNode(const int&NId) {
    int NTypeId = GetNTypeId(NId);
    int LocalNId = GetLocalNId(NId);
    return GetNode(NTypeId, LocalNId);
  }
  const TNode& GetNode(const int&NId) const {
    int NTypeId = GetNTypeId(NId);
    int LocalNId = GetLocalNId(NId);
    return GetNode(NTypeId, LocalNId);
  }
  TNode& GetNode(const int& NTypeId, const int& NId) { return TypeNodeV[NTypeId].NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NTypeId, const int& NId) const { return TypeNodeV[NTypeId].NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }
  void AssertNTypeId(const int NTypeId) const {
    IAssertR(IsNTypeId(NTypeId), TStr::Fmt("NodeTypeId %d does not exist", NTypeId));
  }

public:
  TMNet() : CRef(), MxEId(0), NTypeH(), ETypeH(), TypeNodeV(), EdgeH(), Sz(0), InETypes(), OutETypes(),
    KeyToIndexTypeN(), KeyToIndexTypeE() { }

  TMNet(const TMNet& Graph) : MxEId(Graph.MxEId),
    NTypeH(Graph.NTypeH), ETypeH(Graph.ETypeH), TypeNodeV(Graph.TypeNodeV), EdgeH(Graph.EdgeH), Sz(Graph.Sz),
    InETypes(Graph.InETypes), OutETypes(Graph.OutETypes), KeyToIndexTypeN(), KeyToIndexTypeE() { }
  /// Static cons returns pointer to graph. Ex: PNEANet Graph=TNEANet::New().
  static TPt<TMNet<TNode> > New() {
    return TPt<TMNet<TNode> >(new TMNet());
  }
  TMNet& operator = (const TMNet& Graph) { if (this!=&Graph) {
    MxEId=Graph.MxEId; NTypeH=Graph.NTypeH; ETypeH=Graph.ETypeH; TypeNodeV=Graph.TypeNodeV; EdgeH=Graph.EdgeH;
    Sz=Graph.Sz; InETypes=Graph.InETypes; OutETypes=Graph.OutETypes;
    KeyToIndexTypeN=Graph.KeyToIndexTypeN; KeyToIndexTypeE=Graph.KeyToIndexTypeE;}
    return *this; }

  bool HasFlag(const TGraphFlag& Flag) const {
    if (Flag == gfDirected) { return true; }
    else if (Flag == gfMultiGraph) { return true; }
    else return false;
  }

  /// Gets the NTypeId
  static int GetNTypeId(const int& NId) { return NId & NTYPEID_FLAG; } // Assuming that GlobalNId is positive here
  static int GetLocalNId(const int& GlobalNId) { return GlobalNId >> NTYPEID_NBITS; }

  /// Returns an ID that is larger than any node type ID in the network.
  int GetMxNTypeId() const { return TypeNodeV.Len(); }

  /// Adds a new type with the given string into the graph.
  int AddNType(const TStr& NTypeName) {
    int KeyId = NTypeH.GetKeyId(NTypeName);
    // Has the type been added?
    if (KeyId == -1) {
      // Not added.
      int NTypeId = GetMxNTypeId();
      NTypeH.AddDat(NTypeName, NTypeId);
      TypeNodeV.Add(TNodeType(NTypeId, NTypeName));
      IAssertR(NTypeId == InETypes.Len(), TStr::Fmt("InETypes has inconsistent length."));
      IAssertR(NTypeId == OutETypes.Len(), TStr::Fmt("OutETypes has inconsistent length."));
      InETypes.Add(TIntV());
      OutETypes.Add(TIntV());
      return NTypeId;
    } else {
      // Added. Return the stored id.
      TStr TempKey;
      int NTypeId;
      NTypeH.GetKeyDat(KeyId, TempKey, NTypeId);
      return NTypeId;
    }
  }
  /// Gets the typeId of a type
  int GetNTypeId(const TStr& NTypeStr) { return NTypeH.GetDat(NTypeStr); }
  /// Gets the type name
  TStr GetNTypeName(const int NTypeId) {
    AssertNTypeId(NTypeId);
    return TypeNodeV[NTypeId].Name;
  }
  /// Validates the TypeId
  bool IsNTypeId(const int NTypeId) const { return NTypeId >= 0 && NTypeId < TypeNodeV.Len(); }

  /// Returns the number of nodes in the graph.
  int GetNodes() const { return Sz; }
  /// Returns the number of nodes of a specific type in the graph.
  int GetNodes(const int& NTypeId) const { return TypeNodeV[NTypeId].NodeH.Len(); }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }
  /// Returns an ID that is larger than any node ID of the given type in the network.
  int GetMxNId(const int& NTypeId) const {
    AssertNTypeId(NTypeId);
    return TypeNodeV[NTypeId].MxNId;
  }
  /// Adds a node of ID NId to the graph.
  int AddNode(const int& NTypeId, int NId = -1) {
    AssertNTypeId(NTypeId);
    TNodeType* NodeType = &TypeNodeV[NTypeId];
    if (NId == -1) {
      NId = NodeType->MxNId; NodeType->MxNId++;
    } else {
      IAssertR(!IsNode(NTypeId, NId), TStr::Fmt("NodeId %d with type %d already exists", NId, NTypeId));
      NodeType->MxNId = TMath::Mx(NId+1, NodeType->GetMxNId());
    }
    TNode NewNode(NTypeId, GetGlobalNId(NTypeId, NId));
    NewNode.AddInETypeIds(InETypes[NTypeId]);
    NewNode.AddOutETypeIds(OutETypes[NTypeId]);
    NodeType->NodeH.AddDat(NId, NewNode);
    int GlobalNId = GetGlobalNId(NTypeId, NId);
    MxNId = TMath::Mx(GlobalNId+1, MxNId());
    Sz++;
    return GlobalNId;
  }
  int AddNode(const TStr& NTypeStr) { return AddNode(GetNTypeId(NTypeStr)); }
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetTypeId(), NodeId.GetId()); }
  /// Validates the global NId
  bool IsNode(const int& NId) const { return IsNode(GetNTypeId(NId), GetLocalNId(NId)); }
  /// Validates the NTypeId and NId
  bool IsNode(const int& NTypeId, const int& NId) const {
    if (!IsNTypeId(NTypeId)) { return false; }
    return TypeNodeV[NTypeId].NodeH.IsKey(NId);
  }

  void DelNode(const int& NTypeId, const int& NId) {
    const TNode& Node = GetNode(NTypeId, NId);
    TIntV EIdV;
    Node.GetOutEIdV(EIdV);
    for (int out = 0; out < EIdV.Len(); out++) {
      DelEdge(EIdV[out]);
    }
    Node.GetInEIdV(EIdV);
    for (int in = 0; in < EIdV.Len(); in++) {
      DelEdge(EIdV[in]);
    }
    TypeNodeV[NTypeId].NodeH.DelKey(NId);
    Sz--;
  }
  /// Deletes node of ID NId from the graph.
  void DelNode(const int& NId) { DelNode(GetNTypeId(NId), GetLocalNId(NId)); }

  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetTypeId(), NodeI.GetId()); }

  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const {
    return TNodeI(TypeNodeV.BegI(), this);
  }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(TypeNodeV.EndI(), this); }

  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const {
    int NTypeId = GetNTypeId(NId);
    int LocalNId = GetLocalNId(NId);
    return GetNI(NTypeId, LocalNId);
  }

  TNodeI BegNI(const int& NTypeId) const { return TNodeI(TypeNodeV.GetI(NTypeId), this); }

  TNodeI EndNI(const int& NTypeId) const { return TNodeI(TypeNodeV.GetI(NTypeId), TypeNodeV[NTypeId].NodeH.EndI(), this); }

  TNodeI GetNI(const int& NTypeId, const int& NId) const {
    return TNodeI(TypeNodeV.GetI(NTypeId), TypeNodeV[NTypeId].NodeH.GetI(NId), this);
  }

  void GetNIdV(TIntV& NIdV) const {
    NIdV.Gen(GetNodes(), 0);
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
      NIdV.Add(NI.GetId());
    }
  }

  int AddEType(const TStr& ETypeName, const TStr& SrcNTypeName, const TStr& DstNTypeName) {
    int KeyId = ETypeH.GetKeyId(ETypeName);
    // Has the type been added?
    if (KeyId == -1) {
      // Not added.
      int ETypeId = ETypeH.Len();
      ETypeH.AddDat(ETypeName, ETypeId);
      InETypes[GetNTypeId(DstNTypeName)].Add(ETypeId);
      OutETypes[GetNTypeId(SrcNTypeName)].Add(ETypeId);
      EdgeSzV.Reserve(ETypeId+1, ETypeId+1);
      EdgeSzV[ETypeId] = 0;
      return ETypeId;
    } else {
      // Added. Return the stored id.
      TStr TempKey;
      int ETypeId;
      ETypeH.GetKeyDat(KeyId, TempKey, ETypeId);
      return ETypeId;
    }
  }
  /// Gets the typeId of an edge type
  int GetETypeId(const TStr& ETypeStr) { return ETypeH.GetDat(ETypeStr); }

  /// Returns an ID that is larger than any edge ID in the network.
  int GetMxEId() const { return MxEId; }
  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeH.Len(); }
  /// Returns the number of edges of a specific type in the graph.
  int GetEdges(const int& ETypeId) const { return EdgeSzV[ETypeId].Val; }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph.
  int AddEdge(const int& SrcNId, const int& DstNId, const int& ETypeId, int EId  = -1) {
    if (EId == -1) { EId = MxEId;  MxEId++; }
    else { MxEId = TMath::Mx(EId+1, MxEId()); }

    IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
    IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
    EdgeH.AddDat(EId, TEdge(ETypeId, EId, SrcNId, DstNId));
    GetNode(SrcNId).AddOutNbr(ETypeId, EId);
    GetNode(DstNId).AddInNbr(ETypeId, EId);
    EdgeSzV[ETypeId] += 1;

    return EId;
  }
  int AddEdge(const int& SrcNId, const int& DstNId, const TStr& ETypeStr) { return AddEdge(SrcNId, DstNId, GetETypeId(ETypeStr)); }
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetTypeId(), EdgeI.GetId()); }

  /// Deletes an edge with edge ID EId from the graph.
  void DelEdge(const int& EId) {
    IAssert(IsEdge(EId));
    TEdge Edge = GetEdge(EId);
    int ETypeId = Edge.GetTypeId();
    const int SrcNId = Edge.GetSrcNId();
    const int DstNId = Edge.GetDstNId();
    GetNode(SrcNId).DelOutNbr(ETypeId, EId);
    GetNode(DstNId).DelInNbr(ETypeId, EId);
    EdgeH.DelKey(EId);
    EdgeSzV[ETypeId] -= 1;
  }
  /// Deletes all edges between node IDs SrcNId and DstNId from the graph.
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) {
    int EId;
    IAssert(IsEdge(SrcNId, DstNId, EId, IsDir)); // there is at least one edge
    while (IsEdge(SrcNId, DstNId, EId, IsDir)) {
      DelEdge(EId);
    }
  }

  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId; return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph. if an edge exists, return its edge ID in EId
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const {
    const TNode& SrcNode = GetNode(SrcNId);
    for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
      const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
      if (DstNId == Edge.GetDstNId()) {
        EId = Edge.GetId();
        return true;
      }
    }
    if (! IsDir) {
      for (int edge = 0; edge < SrcNode.GetInDeg(); edge++) {
        const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
        if (DstNId == Edge.GetSrcNId()) {
          EId = Edge.GetId();
          return true;
        }
      }
    }
    return false;
  }

  /// Returns an edge ID between node IDs SrcNId and DstNId, if such an edge exists. Otherwise, return -1.
  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  /// Returns an iterator referring to edge with edge ID EId.
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) {
    int RandN = Rnd.GetUniDevInt(Sz);
    int Ct = 0;
    int NTypeId = 0;
    for (; NTypeId < TypeNodeV.Len(); NTypeId++) {
      Ct += TypeNodeV[NTypeId].NodeH.Len();
      if (Ct > RandN) { break; }
    }
    return GetRndNId(NTypeId, Rnd);
  }
  /// Returns an iterator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Returns an ID of a random node with the given type in the graph.
  int GetRndNId(const int& NTypeId, TRnd& Rnd=TInt::Rnd) {
    return TypeNodeV[NTypeId].NodeH.GetKey(TypeNodeV[NTypeId].NodeH.GetRndKeyId(Rnd, 0.8));
  }
  /// Returns an iterator referring to a random node with the given type in the graph.
  TNodeI GetRndNI(const int& NTypeId, TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(NTypeId, Rnd)); }
  /// Returns an ID of a random edge in the graph.
  int GetRndEId(TRnd& Rnd=TInt::Rnd) { return EdgeH.GetKey(EdgeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an iterator referring to a random edge in the graph.
  TEdgeI GetRndEI(TRnd& Rnd=TInt::Rnd) { return GetEI(GetRndEId(Rnd)); }
/*
  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr() { MxNId=0; MxEId=0; NodeH.Clr(); EdgeH.Clr(),
    KeyToIndexTypeN.Clr(), KeyToIndexTypeE.Clr(), IntDefaultsN.Clr(), IntDefaultsE.Clr(),
    StrDefaultsN.Clr(), StrDefaultsE.Clr(), FltDefaultsN.Clr(), FltDefaultsE.Clr(),
    VecOfIntVecsN.Clr(), VecOfIntVecsE.Clr(), VecOfStrVecsN.Clr(), VecOfStrVecsE.Clr(),
    VecOfFltVecsN.Clr(), VecOfFltVecsE.Clr();}
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) { NodeH.Gen(Nodes/2); } if (Edges>0) { EdgeH.Gen(Edges/2); } }
  /// Defragments the graph.
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency.
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;

  // Get the sum of the weights of all the outgoing edges of the node.
  TFlt GetWeightOutEdges(const TNodeI& NI, const TStr& attr);
  // Check if there is an edge attribute with name attr.
  bool IsFltAttrE(const TStr& attr);
  // Check if there is an edge attribute with name attr.
  bool IsIntAttrE(const TStr& attr);
  // Check if there is an edge attribute with name attr.
  bool IsStrAttrE(const TStr& attr);
  // Get Vector for the Flt Attribute attr.
  TVec<TFlt>& GetFltAttrVecE(const TStr& attr);
  // Get keyid for edge with id EId.
  int GetFltKeyIdE(const int& EId);
  //Fills OutWeights with the outgoing weight from each node.
  void GetWeightOutEdgesV(TFltV& OutWeights, const TFltV& AttrVal) ;
*/

  TPt<TMNet<TNode> > GetSubGraph(const TIntV& NTypeIdV) {
    TPt<TMNet<TNode> > PNewGraph = New();
    TMNet<TNode>& NewGraph = *PNewGraph;
    TIntSet NTypeIdSet(NTypeIdV);
    // Initialize node types
    for (typename THash<TStr,int>::TIter iter = NTypeH.BegI(); iter < NTypeH.EndI(); iter++) {
      if (NTypeIdSet.IsKey(TInt(iter.GetDat()))) { NewGraph.NTypeH.AddDat(iter.GetKey(), iter.GetDat()); }
    }
    // Find relevant edges
    TIntIntH EdgeCounter;
    for (int i = 0; i < InETypes.Len(); i++) {
      if (!NTypeIdSet.IsKey(TInt(i))) { continue; }
      for (int j = 0; j < InETypes[i].Len(); j++) {
        EdgeCounter.AddDat(InETypes[i][j], TInt(1));
      }
    }
    for (int i = 0; i < OutETypes.Len(); i++) {
      if (!NTypeIdSet.IsKey(TInt(i))) { continue; }
      for (int j = 0; j < OutETypes[i].Len(); j++) {
        if (EdgeCounter.IsKey(OutETypes[i][j])) { EdgeCounter.AddDat(OutETypes[i][j], TInt(2)); }
      }
    }
    TIntSet ETypeIdSet;
    for (typename TIntIntH::TIter iter = EdgeCounter.BegI(); iter < EdgeCounter.EndI(); iter++) {
      if (iter.GetDat().Val == 2) { ETypeIdSet.AddKey(iter.GetKey()); }
    }

    for (typename THash<TStr,int>::TIter iter = ETypeH.BegI(); iter < ETypeH.EndI(); iter++) {
      if (ETypeIdSet.IsKey(TInt(iter.GetDat()))) { NewGraph.ETypeH.AddDat(iter.GetKey(), iter.GetDat()); }
    }
    NewGraph.InETypes.Gen(InETypes.Len());
    for (int i = 0; i < InETypes.Len(); i++) {
      for (int j = 0; j < InETypes[i].Len(); j++) {
        int ETypeId = InETypes[i][j];
        if (ETypeIdSet.IsKey(ETypeId)) { NewGraph.InETypes[i].Add(ETypeId); }
      }
    }
    NewGraph.OutETypes.Gen(OutETypes.Len());
    for (int i = 0; i < OutETypes.Len(); i++) {
      for (int j = 0; j < OutETypes[i].Len(); j++) {
        int ETypeId = OutETypes[i][j];
        if (ETypeIdSet.IsKey(ETypeId)) { NewGraph.OutETypes[i].Add(ETypeId); }
      }
    }

    NewGraph.Sz = 0;
    NewGraph.TypeNodeV.Gen(TypeNodeV.Len());
    for (int NTypeId = 0; NTypeId < TypeNodeV.Len(); NTypeId++) {
      if (NTypeIdSet.IsKey(NTypeId)) {
        NewGraph.TypeNodeV[NTypeId] = TNodeType(TypeNodeV[NTypeId], NewGraph.InETypes[NTypeId], NewGraph.OutETypes[NTypeId]);
        NewGraph.Sz += NewGraph.TypeNodeV[NTypeId].NodeH.Len();
      } else {
        NewGraph.TypeNodeV[NTypeId] = TNodeType(TypeNodeV[NTypeId].GetId(), TypeNodeV[NTypeId].GetName());
      }
    }

    NewGraph.MxNId = MxNId;
    int MaxEId = 0;
    for (TEdgeI iter = BegEI(); iter < EndEI(); iter++) {
      if (!ETypeIdSet.IsKey(iter.GetTypeId())) { continue; }
      int EId = iter.GetId();
      NewGraph.EdgeH.AddDat(EId, TEdge(iter.GetTypeId(), EId, iter.GetSrcNId(), iter.GetDstNId()));
      if (MaxEId < EId) { MaxEId = EId; }
    }
    NewGraph.MxEId = MaxEId + 1;
    return PNewGraph;
  }

  TPt<TMNet<TNode> > GetSubGraph(const TStrV& NTypeNameV) {
    TIntV NTypeIdV;
    for (int i = 0; i < NTypeNameV.Len(); i++) {
      NTypeIdV.Add(NTypeH.GetDat(NTypeNameV[i]));
    }
    return GetSubGraph(NTypeIdV);
  }

  PNEANet GetSubGraphTNEANet(const TIntV& NTypeIdV) {
    // Find relevant edge types
    TIntSet NTypeIdSet(NTypeIdV);
    TIntIntH EdgeCounter;
    for (int i = 0; i < InETypes.Len(); i++) {
      if (!NTypeIdSet.IsKey(TInt(i))) { continue; }
      for (int j = 0; j < InETypes[i].Len(); j++) {
        EdgeCounter.AddDat(InETypes[i][j], TInt(1));
      }
    }
    for (int i = 0; i < OutETypes.Len(); i++) {
      if (!NTypeIdSet.IsKey(TInt(i))) { continue; }
      for (int j = 0; j < OutETypes[i].Len(); j++) {
        if (EdgeCounter.IsKey(OutETypes[i][j])) { EdgeCounter.AddDat(OutETypes[i][j], TInt(2)); }
      }
    }
    TIntV ETypeIdV;
    for (typename TIntIntH::TIter iter = EdgeCounter.BegI(); iter < EdgeCounter.EndI(); iter++) {
      if (iter.GetDat().Val == 2) {
        ETypeIdV.Add(iter.GetKey());
      }
    }

    return GetSubGraphTNEANet2(NTypeIdV, ETypeIdV);
  }

  /// Extracts the subgraph by scanning and filtering all edges (edge-based)
  PNEANet GetSubGraphTNEANet(const TIntV& NTypeIdV, const TIntV& ETypeIdV) {
    PNEANet PNewGraph = PNEANet::New();
    for (int i = 0; i < NTypeIdV.Len(); i++) {
      TInt NTypeId = NTypeIdV[i];
      for (typename THash<TInt,TNode>::TIter iter = TypeNodeV[NTypeId].NodeH.BegI(); iter < TypeNodeV[NTypeId].NodeH.EndI(); iter++) {
        PNewGraph->AddNode(GetGlobalNId(NTypeId, iter.GetKey().Val));
      }
    }

    TIntSet ETypeIdSet(ETypeIdV);
    // Add edges
    for (TEdgeI iter = BegEI(); iter < EndEI(); iter++) {
      if (ETypeIdSet.IsKey(iter.GetTypeId())) {
        PNewGraph->AddEdge(iter.GetSrcNId(), iter.GetDstNId(), iter.GetId());
      }
    }
    return PNewGraph;
  }

  /// Extracts the subgraph by finding the nodes and then finding all neighbors (node-based)
  PNEANet GetSubGraphTNEANet2(const TIntV& NTypeIdV, const TIntV& ETypeIdV) {
    PNEANet PNewGraph = PNEANet::New();
    // Add nodes
    for (int i = 0; i < NTypeIdV.Len(); i++) {
      TInt NTypeId = NTypeIdV[i];
      for (typename THash<TInt,TNode>::TIter iter = TypeNodeV[NTypeId].NodeH.BegI(); iter < TypeNodeV[NTypeId].NodeH.EndI(); iter++) {
        PNewGraph->AddNode(GetGlobalNId(NTypeId, iter.GetKey().Val));
      }
    }
    // Add edges
    TIntSet ETypeIdSet(ETypeIdV);
    TIntV EIdV; // Use same vector to save memory
    for (int i = 0; i < NTypeIdV.Len(); i++) {
      TInt NTypeId = NTypeIdV[i];
      TIntV* POutETypes = &(OutETypes[NTypeId]);
      TIntV OutETypeIdV;
      for (TIntV::TIter iter = POutETypes->BegI(); iter < POutETypes->EndI(); iter++) {
        if (ETypeIdSet.IsKey(*iter)) { OutETypeIdV.Add(*iter); }
      }
      for (typename THash<TInt,TNode>::TIter iter = TypeNodeV[NTypeId].NodeH.BegI(); iter < TypeNodeV[NTypeId].NodeH.EndI(); iter++) {
        TNode* PNode = &(iter.GetDat());
        for (int j = 0; j < OutETypeIdV.Len(); j++) {
          PNode->GetOutEIdV(OutETypeIdV.GetVal(j).Val, EIdV);
          for (int k = 0; k < EIdV.Len(); k++) {
            TInt EId = EIdV[k];
            PNewGraph->AddEdge(PNode->GetId(), GetEdge(EId).GetDstNId(), EId);
          }
        }
      }
    }
    return PNewGraph;
  }

#ifdef GCC_ATOMIC
  PNEANetMP GetSubGraphTNEANetMP2(const TIntV& NTypeIdV) {
    TStopwatch* Sw = TStopwatch::GetInstance();
    Sw->Start(TStopwatch::ComputeETypes);
    // Find relevant edge types
    TIntSet NTypeIdSet(NTypeIdV);
    TIntIntH EdgeCounter;
    for (int i = 0; i < InETypes.Len(); i++) {
      if (!NTypeIdSet.IsKey(TInt(i))) { continue; }
      for (int j = 0; j < InETypes[i].Len(); j++) {
        EdgeCounter.AddDat(InETypes[i][j], TInt(1));
      }
    }
    for (int i = 0; i < OutETypes.Len(); i++) {
      if (!NTypeIdSet.IsKey(TInt(i))) { continue; }
      for (int j = 0; j < OutETypes[i].Len(); j++) {
        if (EdgeCounter.IsKey(OutETypes[i][j])) { EdgeCounter.AddDat(OutETypes[i][j], TInt(2)); }
      }
    }
    TIntV ETypeIdV;
    for (typename TIntIntH::TIter iter = EdgeCounter.BegI(); iter < EdgeCounter.EndI(); iter++) {
      if (iter.GetDat().Val == 2) {
        ETypeIdV.Add(iter.GetKey());
      }
    }
    Sw->Stop(TStopwatch::ComputeETypes);

    return GetSubGraphTNEANetMP2(NTypeIdV, ETypeIdV);
  }

  /// Extracts the subgraph by finding the nodes and then finding all neighbors (node-based)
  PNEANetMP GetSubGraphTNEANetMP(const TIntV& NTypeIdV, const TIntV& ETypeIdV) {
    TStopwatch* Sw = TStopwatch::GetInstance();
    Sw->Start(TStopwatch::EstimateSizes);
    int SubgraphSz = 0;
    for (TIntV::TIter iter = NTypeIdV.BegI(); iter < NTypeIdV.EndI(); iter++) {
      SubgraphSz += GetNodes((*iter).Val);
    }
    int SubgraphEdgeSz = 0;
    for (TIntV::TIter iter = ETypeIdV.BegI(); iter < ETypeIdV.EndI(); iter++) {
      SubgraphEdgeSz += GetEdges((*iter).Val);
    }
    Sw->Stop(TStopwatch::EstimateSizes);

    Sw->Start(TStopwatch::InitGraph);
    PNEANetMP PNewGraph = TNEANetMP::New(SubgraphSz, SubgraphEdgeSz);
    TIntSet ETypeIdSet(ETypeIdV);
    Sw->Stop(TStopwatch::InitGraph);

    TIntV OutETypeIdVV[NTypeIdV.Len()];
    TIntV InETypeIdVV[NTypeIdV.Len()];
    Sw->Start(TStopwatch::ExtractNbrETypes);
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < NTypeIdV.Len(); i++) {

      TInt NTypeId = NTypeIdV[i];
      TIntV* POutETypes = &(OutETypes[NTypeId]);
      for (TIntV::TIter iter = POutETypes->BegI(); iter < POutETypes->EndI(); iter++) {
        if (ETypeIdSet.IsKey(*iter)) { OutETypeIdVV[i].Add(*iter); }
      }
      TIntV* PInETypes = &(InETypes[NTypeId]);
      for (TIntV::TIter iter = PInETypes->BegI(); iter < PInETypes->EndI(); iter++) {
        if (ETypeIdSet.IsKey(*iter)) { InETypeIdVV[i].Add(*iter); }
      }
    }
    Sw->Stop(TStopwatch::ExtractNbrETypes);

    TIntV Offsets(NTypeIdV.Len()+1);
    Offsets[0] = 0;
    for (int i = 0; i < NTypeIdV.Len(); i++) {
      Offsets[i+1] = Offsets[i] + TypeNodeV[NTypeIdV[i]].NodeH.GetMxKeyIds();
    }

    Sw->Start(TStopwatch::PopulateGraph);
    #pragma omp parallel for schedule(static)
    for (int j = 0; j < Offsets[NTypeIdV.Len()]; j++) {
      int i;
      Offsets.SearchBinLeft(j, i);
      THash<TInt,TNode> *NodeHPtr = &(TypeNodeV[NTypeIdV[i]].NodeH);
      int KeyId = j - Offsets[i];

      if (!NodeHPtr->IsKeyId(KeyId)) { continue; }
      TNode* PNode = &((*NodeHPtr)[KeyId]);
      int NId = PNode->GetId();
      TIntV EIdV;
      TIntV OutEIdV;
      for (TIntV::TIter iter = OutETypeIdVV[i].BegI(); iter < OutETypeIdVV[i].EndI(); iter++) {
        PNode->GetOutEIdV((*iter).Val, EIdV);
        OutEIdV.AddV(EIdV);
      }
      TIntV InEIdV;
      for (TIntV::TIter iter = InETypeIdVV[i].BegI(); iter < InETypeIdVV[i].EndI(); iter++) {
        PNode->GetInEIdV((*iter).Val, EIdV);
        InEIdV.AddV(EIdV);
      }
      PNewGraph->AddNodeWithEdges(NId, InEIdV, OutEIdV);
      for (TIntV::TIter iter = OutEIdV.BegI(); iter < OutEIdV.EndI(); iter++) {
        PNewGraph->AddEdgeUnchecked((*iter), NId, GetEdge(*iter).GetDstNId());
      }

    }
    Sw->Stop(TStopwatch::PopulateGraph);

    PNewGraph->SetNodes(SubgraphSz);
    PNewGraph->SetEdges(SubgraphEdgeSz);
    return PNewGraph;
  }

  /// Extracts the subgraph by finding the nodes and then finding all neighbors (node-based)
  PNEANetMP GetSubGraphTNEANetMP2(const TIntV& NTypeIdV, const TIntV& ETypeIdV) {
    TStopwatch* Sw = TStopwatch::GetInstance();
    Sw->Start(TStopwatch::EstimateSizes);
    int SubgraphSz = 0;
    for (TIntV::TIter iter = NTypeIdV.BegI(); iter < NTypeIdV.EndI(); iter++) {
      SubgraphSz += GetNodes((*iter).Val);
    }
    int SubgraphEdgeSz = 0;
    for (TIntV::TIter iter = ETypeIdV.BegI(); iter < ETypeIdV.EndI(); iter++) {
      SubgraphEdgeSz += GetEdges((*iter).Val);
    }
    Sw->Stop(TStopwatch::EstimateSizes);

    Sw->Start(TStopwatch::InitGraph);
    PNEANetMP PNewGraph = TNEANetMP::New(SubgraphSz, SubgraphEdgeSz);
    TIntSet ETypeIdSet(ETypeIdV);
    Sw->Stop(TStopwatch::InitGraph);

//    int NThreads = omp_get_max_threads();
//    TIntV VectorPool[2*NThreads];
    for (int i = 0; i < NTypeIdV.Len(); i++) {
      Sw->Start(TStopwatch::ExtractNbrETypes);
      TInt NTypeId = NTypeIdV[i];
      TIntV* POutETypes = &(OutETypes[NTypeId]);
      TIntV OutETypeIdV;
      for (TIntV::TIter iter = POutETypes->BegI(); iter < POutETypes->EndI(); iter++) {
        if (ETypeIdSet.IsKey(*iter)) { OutETypeIdV.Add(*iter); }
      }
      TIntV* PInETypes = &(InETypes[NTypeId]);
      TIntV InETypeIdV;
      for (TIntV::TIter iter = PInETypes->BegI(); iter < PInETypes->EndI(); iter++) {
        if (ETypeIdSet.IsKey(*iter)) { InETypeIdV.Add(*iter); }
      }
      Sw->Stop(TStopwatch::ExtractNbrETypes);

      Sw->Start(TStopwatch::PopulateGraph);
      THash<TInt,TNode> *NodeHPtr = &(TypeNodeV[NTypeId].NodeH);

//      omp_set_num_threads(NThreads);
      #pragma omp parallel for schedule(static)
      for (int KeyId = 0; KeyId < NodeHPtr->GetMxKeyIds(); KeyId++) {
        if (!NodeHPtr->IsKeyId(KeyId)) { continue; }

        TIntV OutEIdV;
        TIntV InEIdV;

//        Sw->Start(TStopwatch::ExtractEdges);
        TNode* PNode = &((*NodeHPtr)[KeyId]);
        int NId = PNode->GetId();

        //Sw->Start(TStopwatch::ExtractEdges);
//        OutEIdV.Reduce(0);
//        for (TIntV::TIter iter = OutETypeIdV.BegI(); iter < OutETypeIdV.EndI(); iter++) {
//          PNode->GetOutEIdV((*iter).Val, *EIdV);
//          OutEIdV->AddV(*EIdV);
//        }
//        InEIdV.Reduce(0);
//        for (TIntV::TIter iter = InETypeIdV.BegI(); iter < InETypeIdV.EndI(); iter++) {
//          PNode->GetInEIdV((*iter).Val, *EIdV);
//          InEIdV->AddV(*EIdV);
//        }
        PNode->GetOutEIdV(OutETypeIdV, OutEIdV);
        PNode->GetInEIdV(InETypeIdV, InEIdV);
//        Sw->Stop(TStopwatch::ExtractEdges);

//        Sw->Start(TStopwatch::BuildSubgraph);
        PNewGraph->AddNodeWithEdges(NId, InEIdV, OutEIdV);

        for (TIntV::TIter iter = OutEIdV.BegI(); iter < OutEIdV.EndI(); iter++) {
          PNewGraph->AddEdgeUnchecked((*iter), NId, GetEdge(*iter).GetDstNId());
        }
//        Sw->Stop(TStopwatch::BuildSubgraph);
      }
      Sw->Stop(TStopwatch::PopulateGraph);
    }

    PNewGraph->SetNodes(SubgraphSz);
    PNewGraph->SetEdges(SubgraphEdgeSz);
    return PNewGraph;
  }
#endif // GCC_ATOMIC

  friend class TPt<TMNet>;
};

typedef TMNet<TSVNode> TSVNet;
typedef TPt<TSVNet> PSVNet;
typedef TMNet<TMVNode> TMVNet;
typedef TPt<TMVNet> PMVNet;
typedef TMNet<TCVNode> TCVNet;
typedef TPt<TCVNet> PCVNet;
#endif // MGRAPH_H
