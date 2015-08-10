enum TAttrType { atUndef, atInt, atFlt, atStr, atAny };
typedef TPair<TStr, TAttrType> TAttrPr;
typedef TVec<TAttrPr> TAttrPrV;

class TSparseAttrSingle {
private:
  // Sparse Attributes Name Mapping
  TStrIntPrH AttrNameToId;
  TIntIntStrPrH AttrIdToName;

  TIntPrIntH IntAttrVals;
  TIntPrFltH FltAttrVals;
  TIntPrStrH StrAttrVals;
private:
  int GetIdVSAttr(const TInt& AttrId, const TAttrType Type, TIntV& IdV);
public:
  TSparseAttrSingle() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TSparseAttrSingle(const TSparseAttrSingle& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  // Constructor for loading attributes from a (binary) stream SIn.
  TSparseAttrSingle(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  // Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }

  // Add functions
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val); 
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val);

  int AddSAttrDat(const TInt& Id, const TStr& AttrName, TFlt& Val); 
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, TFlt& Val);

  int AddSAttrDat(const TInt& Id, const TStr& AttrName, TStr& Val); 
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, TStr& Val);

  // Get functions
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val) const; 
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val) const;

  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TFlt& Val) const; 
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TFlt& Val) const;

  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TStr& Val) const; 
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TStr& Val) const;

  // Del functions
  int DelSAttrDat(const TInt& Id, const TStr& AttrName); 
  int DelSAttrDat(const TInt& Id, const TInt& AttrId);

  // GetNames
  void GetSAttrV(const TInt& Id, TAttrType AttrType, TAttrPrV& AttrV);

  // Get ids of all items with given attribute
  int GetIdVSAttr(const TStr& AttrName, TIntV& IdV);
  int GetIdVSAttr(const TInt& AttrId, TIntV& IdV);

  // Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  // Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};


class TSparseAttrPair {
private:
  // Sparse Attributes Name Mapping
  TStrIntPrH AttrNameToId;
  TIntIntStrPrH AttrIdToName;

  TIntIntPrPrIntH IntAttrVals;
  TIntIntPrPrFltH FltAttrVals;
  TIntIntPrPrStrH StrAttrVals;
private:
  int GetIdVSAttr(const TInt& AttrId, const TAttrType Type, TIntPrV& IdV);
public:
  TSparseAttrPair() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TSparseAttrPair(const TSparseAttrPair& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  // Constructor for loading attributes from a (binary) stream SIn.
  TSparseAttrPair(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  // Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }

  // Add functions
  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, TInt& Val); 
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, TInt& Val);

  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, TFlt& Val); 
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, TFlt& Val);

  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, TStr& Val); 
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, TStr& Val);

  // Get functions
  int GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TInt& Val) const; 
  int GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TInt& Val) const;

  int GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TFlt& Val) const; 
  int GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TFlt& Val) const;

  int GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TStr& Val) const; 
  int GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TStr& Val) const;

  // Del functions
  int DelSAttrDat(const TIntPr& Id, const TStr& AttrName); 
  int DelSAttrDat(const TIntPr& Id, const TInt& AttrId);

  // GetNames
  void GetSAttrV(const TIntPr& Id, TAttrType AttrType, TAttrPrV& AttrV);

  // Get ids of all items with given attribute
  int GetIdVSAttr(const TStr& AttrName, TIntPrV& IdV);
  int GetIdVSAttr(const TInt& AttrId, TIntPrV& IdV);

  // Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  // Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};