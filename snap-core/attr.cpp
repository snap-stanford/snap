/// Add functions
int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  TPair<TInt, TInt> Key(AttrId, Id);
  IntAttrVals.AddDat(Key, Val);
  return 0;
}

/// Get functions
int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val) const {
  TPair<TInt, TInt> Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    Val = IntAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

/// Del functions
int TSparseAttrSingle::DelSAttrDat(const TInt& Id, const TStr& AttrName) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return DelSAttrDat(Id, AttrId);
} 
int TSparseAttrSingle::DelSAttrDat(const TInt& Id, const TInt& AttrId) {
  TPair<TInt, TInt> Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    IntAttrVals.DelKey(Key);
    return 0;
  }
  return -1;
}

/// GetNames
void TSparseAttrSingle::GetSAttrV(const TInt& Id, TAttrType AttrType, TAttrPrV& AttrV) {
  // TStr, TAttrType
  for (THash<TStr, TPair<TInt, TInt> >::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    if (CurType == AttrType || AttrType == atAny) {
      TPair<TInt, TInt> AttrKey(it.GetDat().GetVal2(), Id);
      if (CurType == atInt) {
        if (IntAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }

      } else if (CurType == atFlt) {
        if (FltAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      } else if (CurType == atStr) {
        if (StrAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      }
    }
  }
}

/// Get values over all nodes
int TSparseAttrSingle::GetIdVSAttr(const TStr& AttrName, TIntV& IdV) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TPair<TInt, TInt> TypeAndId = AttrNameToId.GetDat(AttrName);
  TAttrType Type = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  TInt AttrId = TypeAndId.GetVal2();
  if (Type == atInt) {
    for (THash<TPair<TInt, TInt>, TInt>::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atFlt) {
    for (THash<TPair<TInt, TInt>, TFlt>::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atStr) {
    for (THash<TPair<TInt, TInt>, TStr>::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else {
    return -1;
  }
  return 0;
}

/// Add Attribute mapping
int TSparseAttrSingle::AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  if (AttrType != atInt && AttrType != atFlt && AttrType != atStr) {
    return -1; // type must be defined and can only have a single type
  }
  AttrId = AttrNameToId.GetMxKeyIds();
  TPair<TInt, TInt> TypeAndId(AttrType, AttrId);
  AttrNameToId.AddDat(Name, TypeAndId);
  TPair<TInt, TStr> TypeAndName(AttrType, Name);
  AttrIdToName.AddDat(AttrId, TypeAndName);
  return 0;
}

/// Attribute Name to ID conversion and vice versa
int TSparseAttrSingle::GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  if (!AttrNameToId.IsKey(Name)) {
    return -1;
  }
  TPair<TInt, TInt> TypeAndId = AttrNameToId.GetDat(Name);
  AttrType = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  AttrId = TypeAndId.GetVal2();
  return 0;
}
int TSparseAttrSingle::GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  if (!AttrIdToName.IsKey(AttrId)) {
    return -1;
  }
  TPair<TInt, TStr> TypeAndName = AttrIdToName.GetDat(AttrId);
  AttrType = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  Name = TypeAndName.GetVal2();
  return 0;
}



/// Add functions
int TSparseAttrPair::AddSAttrDat(const TPair<TInt, TInt>& Id, const TStr& AttrName, TInt& Val) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::AddSAttrDat(const TPair<TInt, TInt>& Id, const TInt& AttrId, TInt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  TPair<TInt, TPair<TInt, TInt> > Key(AttrId, Id);
  IntAttrVals.AddDat(Key, Val);
  return 0;
}

/// Get functions
int TSparseAttrPair::GetSAttrDat(const TPair<TInt, TInt>& Id, const TStr& AttrName, TInt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::GetSAttrDat(const TPair<TInt, TInt>& Id, const TInt& AttrId, TInt& Val) const {
  TPair<TInt, TPair<TInt, TInt> > Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    Val = IntAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

/// Del functions
int TSparseAttrPair::DelSAttrDat(const TPair<TInt, TInt>& Id, const TStr& AttrName) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return DelSAttrDat(Id, AttrId);
} 
int TSparseAttrPair::DelSAttrDat(const TPair<TInt, TInt>& Id, const TInt& AttrId) {
  TPair<TInt, TPair<TInt, TInt> > Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    IntAttrVals.DelKey(Key);
    return 0;
  }
  return -1;
}

/// GetNames
void TSparseAttrPair::GetSAttrV(const TPair<TInt, TInt>& Id, TAttrType AttrType, TAttrPrV& AttrV) {
  // TStr, TAttrType
  for (THash<TStr, TPair<TInt, TInt> >::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    if (CurType == AttrType || AttrType == atAny) {
      TPair<TInt, TPair<TInt, TInt> > AttrKey(it.GetDat().GetVal2(), Id);
      if (CurType == atInt) {
        if (IntAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }

      } else if (CurType == atFlt) {
        if (FltAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      } else if (CurType == atStr) {
        if (StrAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      }
    }
  }
}

/// Get values over all nodes
int TSparseAttrPair::GetIdVSAttr(const TStr& AttrName, TVec<TPair<TInt, TInt> >& IdV) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TPair<TInt, TInt> TypeAndId = AttrNameToId.GetDat(AttrName);
  TAttrType Type = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  TInt AttrId = TypeAndId.GetVal2();
  if (Type == atInt) {
    for (THash<TPair<TInt, TPair<TInt, TInt> >, TInt>::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atFlt) {
    for (THash<TPair<TInt, TPair<TInt, TInt> >, TFlt>::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atStr) {
    for (THash<TPair<TInt, TPair<TInt, TInt> >, TStr>::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else {
    return -1;
  }
  return 0;
}

/// Add Attribute mapping
int TSparseAttrPair::AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  if (AttrType != atInt && AttrType != atFlt && AttrType != atStr) {
    return -1; // type must be defined and can only have a single type
  }
  AttrId = AttrNameToId.GetMxKeyIds();
  TPair<TInt, TInt> TypeAndId(AttrType, AttrId);
  AttrNameToId.AddDat(Name, TypeAndId);
  TPair<TInt, TStr> TypeAndName(AttrType, Name);
  AttrIdToName.AddDat(AttrId, TypeAndName);
  return 0;
}

/// Attribute Name to ID conversion and vice versa
int TSparseAttrPair::GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  if (!AttrNameToId.IsKey(Name)) {
    return -1;
  }
  TPair<TInt, TInt> TypeAndId = AttrNameToId.GetDat(Name);
  AttrType = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  AttrId = TypeAndId.GetVal2();
  return 0;
}
int TSparseAttrPair::GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  if (!AttrIdToName.IsKey(AttrId)) {
    return -1;
  }
  TPair<TInt, TStr> TypeAndName = AttrIdToName.GetDat(AttrId);
  AttrType = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  Name = TypeAndName.GetVal2();
  return 0;
}