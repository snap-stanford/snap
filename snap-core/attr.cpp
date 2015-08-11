// Add functions
int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TStr& AttrName, const TInt& Val) {
  TInt AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atInt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TInt& AttrId, const TInt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atInt) { return -2; }
  TIntPr Key(AttrId, Id);
  IntAttrVals.AddDat(Key, Val);
  return 0;
}

int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TStr& AttrName, const TFlt& Val) {
  TInt AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atFlt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TInt& AttrId, const TFlt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atFlt) { return -2; }
  TIntPr Key(AttrId, Id);
  FltAttrVals.AddDat(Key, Val);
  return 0;
}

int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TStr& AttrName, const TStr& Val) {
  TInt AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atStr, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::AddSAttrDat(const TInt& Id, const TInt& AttrId, const TStr& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atStr) { return -2; }
  TIntPr Key(AttrId, Id);
  StrAttrVals.AddDat(Key, Val);
  return 0;
}

// Get functions
int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val) const {
  TIntPr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    Val = IntAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TStr& AttrName, TFlt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TInt& AttrId, TFlt& Val) const {
  TIntPr Key(AttrId, Id);
  if (FltAttrVals.IsKey(Key)) {
    Val = FltAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TStr& AttrName, TStr& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrSingle::GetSAttrDat(const TInt& Id, const TInt& AttrId, TStr& Val) const {
  TIntPr Key(AttrId, Id);
  if (StrAttrVals.IsKey(Key)) {
    Val = StrAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

// Del functions
int TSparseAttrSingle::DelSAttrDat(const TInt& Id, const TStr& AttrName) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return DelSAttrDat(Id, AttrId);
} 
int TSparseAttrSingle::DelSAttrDat(const TInt& Id, const TInt& AttrId) {
  TIntPr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    IntAttrVals.DelKey(Key);
    return 0;
  } else if (FltAttrVals.IsKey(Key)) {
    FltAttrVals.DelKey(Key);
    return 0;
  } else if (StrAttrVals.IsKey(Key)) {
    StrAttrVals.DelKey(Key);
    return 0;
  }
  return -1;
}

// GetNames
void TSparseAttrSingle::GetSAttrV(const TInt& Id, const TAttrType AttrType, TAttrPrV& AttrV) const {
  for (TStrIntPrH::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    if (CurType == AttrType || AttrType == atAny) {
      TIntPr AttrKey(it.GetDat().GetVal2(), Id);
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

// Get ids of all items with given attribute
int TSparseAttrSingle::GetIdVSAttr(const TInt& AttrId, const TAttrType Type, TIntV& IdV) const {
  if (Type == atInt) {
    for (TIntPrIntH::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atFlt) {
    for (TIntPrFltH::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atStr) {
    for (TIntPrStrH::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else {
    return -1;
  }
  return 0;
}

int TSparseAttrSingle::GetIdVSAttr(const TInt& AttrId, TIntV& IdV) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  TIntStrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  TAttrType Type = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  return GetIdVSAttr(AttrId, Type, IdV);
}

int TSparseAttrSingle::GetIdVSAttr(const TStr& AttrName, TIntV& IdV) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TIntPr TypeAndId = AttrNameToId.GetDat(AttrName);
  TAttrType Type = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  TInt AttrId = TypeAndId.GetVal2();
  return GetIdVSAttr(AttrId, Type, IdV);
}

// Add Attribute mapping
int TSparseAttrSingle::AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  if (AttrType != atInt && AttrType != atFlt && AttrType != atStr) {
    return -1; // type must be defined and can only have a single type
  }
  if (AttrNameToId.IsKey(Name)) { return -1; }
  AttrId = AttrNameToId.GetMxKeyIds();
  TIntPr TypeAndId(AttrType, AttrId);
  AttrNameToId.AddDat(Name, TypeAndId);
  TIntStrPr TypeAndName(AttrType, Name);
  AttrIdToName.AddDat(AttrId, TypeAndName);
  return 0;
}

// Attribute Name to ID conversion and vice versa
int TSparseAttrSingle::GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  if (!AttrNameToId.IsKey(Name)) {
    return -1;
  }
  TIntPr TypeAndId = AttrNameToId.GetDat(Name);
  AttrType = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  AttrId = TypeAndId.GetVal2();
  return 0;
}
int TSparseAttrSingle::GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  if (!AttrIdToName.IsKey(AttrId)) {
    return -1;
  }
  TIntStrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  AttrType = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  Name = TypeAndName.GetVal2();
  return 0;
}



// Add functions
int TSparseAttrPair::AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TInt& Val) {
  TInt AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atInt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TInt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atInt) { return -2; }
  TIntIntPrPr Key(AttrId, Id);
  IntAttrVals.AddDat(Key, Val);
  return 0;
}

int TSparseAttrPair::AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TFlt& Val) {
  TInt AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atFlt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TFlt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atFlt) { return -2; }
  TIntIntPrPr Key(AttrId, Id);
  FltAttrVals.AddDat(Key, Val);
  return 0;
}

int TSparseAttrPair::AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TStr& Val) {
  TInt AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atStr, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TStr& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atStr) { return -2; }
  TIntIntPrPr Key(AttrId, Id);
  StrAttrVals.AddDat(Key, Val);
  return 0;
}

// Get functions
int TSparseAttrPair::GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TInt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TInt& Val) const {
  TIntIntPrPr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    Val = IntAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int TSparseAttrPair::GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TFlt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TFlt& Val) const {
  TIntIntPrPr Key(AttrId, Id);
  if (FltAttrVals.IsKey(Key)) {
    Val = FltAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int TSparseAttrPair::GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TStr& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int TSparseAttrPair::GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TStr& Val) const {
  TIntIntPrPr Key(AttrId, Id);
  if (StrAttrVals.IsKey(Key)) {
    Val = StrAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

// Del functions
int TSparseAttrPair::DelSAttrDat(const TIntPr& Id, const TStr& AttrName) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return DelSAttrDat(Id, AttrId);
} 
int TSparseAttrPair::DelSAttrDat(const TIntPr& Id, const TInt& AttrId) {
  TIntIntPrPr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    IntAttrVals.DelKey(Key);
    return 0;
  } else if (FltAttrVals.IsKey(Key)) {
    FltAttrVals.DelKey(Key);
    return 0;
  } else if (StrAttrVals.IsKey(Key)) {
    StrAttrVals.DelKey(Key);
    return 0;
  }
  return -1;
}

// GetNames
void TSparseAttrPair::GetSAttrV(const TIntPr& Id, const TAttrType AttrType, TAttrPrV& AttrV) const {
  for (TStrIntPrH::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    if (CurType == AttrType || AttrType == atAny) {
      TIntIntPrPr AttrKey(it.GetDat().GetVal2(), Id);
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

// Get ids of all items with given attribute
int TSparseAttrPair::GetIdVSAttr(const TInt& AttrId, const TAttrType Type, TIntPrV& IdV) const {
  if (Type == atInt) {
    for (TIntIntPrPrIntH::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atFlt) {
    for (TIntIntPrPrFltH::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atStr) {
    for (TIntIntPrPrStrH::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else {
    return -1;
  }
  return 0;
}

int TSparseAttrPair::GetIdVSAttr(const TStr& AttrName, TIntPrV& IdV) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TIntPr TypeAndId = AttrNameToId.GetDat(AttrName);
  TAttrType Type = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  TInt AttrId = TypeAndId.GetVal2();
  return GetIdVSAttr(AttrId, Type, IdV);
}

int TSparseAttrPair::GetIdVSAttr(const TInt& AttrId, TIntPrV& IdV) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  TIntStrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  TAttrType Type = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  return GetIdVSAttr(AttrId, Type, IdV);
}

// Add Attribute mapping
int TSparseAttrPair::AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  if (AttrType != atInt && AttrType != atFlt && AttrType != atStr) {
    return -1; // type must be defined and can only have a single type
  }
  AttrId = AttrNameToId.GetMxKeyIds();
  TIntPr TypeAndId(AttrType, AttrId);
  AttrNameToId.AddDat(Name, TypeAndId);
  TIntStrPr TypeAndName(AttrType, Name);
  AttrIdToName.AddDat(AttrId, TypeAndName);
  return 0;
}

// Attribute Name to ID conversion and vice versa
int TSparseAttrPair::GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  if (!AttrNameToId.IsKey(Name)) {
    return -1;
  }
  TIntPr TypeAndId = AttrNameToId.GetDat(Name);
  AttrType = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  AttrId = TypeAndId.GetVal2();
  return 0;
}

int TSparseAttrPair::GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  if (!AttrIdToName.IsKey(AttrId)) {
    return -1;
  }
  TIntStrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  AttrType = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  Name = TypeAndName.GetVal2();
  return 0;
}