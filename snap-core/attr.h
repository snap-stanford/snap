enum TAttrType { atUndef, atInt, atFlt, atStr, atAny };
typedef TPair<TStr, TAttrType> TAttrPr;
typedef TVec<TAttrPr> TAttrPrV;

class TAttr {
private:
  // Sparse Attributes Name Mapping
  TStrIntPrH AttrNameToId;
  TIntIntStrPrH AttrIdToName;

  TIntPrIntH IntAttrVals;
  TIntPrFltH FltAttrVals;
  TIntPrStrH StrAttrVals;
private:
  int GetIdVSAttr(const TInt& AttrId, const TAttrType Type, TIntV& IdV) const;
public:
  TAttr() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TAttr(const TAttr& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  /// Constructor for loading attributes from a (binary) stream SIn.
  TAttr(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  /// Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }

  /// Add Int attribute for the given id with name AttrName.
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, const TInt& Val);
  /// Add Int attribute for the given id with attribute id AttrId.
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, const TInt& Val);

  /// Add Flt attribute for the given id with name AttrName.
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, const TFlt& Val);
  /// Add Flt attribute for the given id with attribute id AttrId.
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, const TFlt& Val);

  /// Add Str attribute for the given id with name AttrName.
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, const TStr& Val);
  /// Add Str attribute for the given id with attribute id AttrId. 
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, const TStr& Val);

  /// Get Int attribute for the given id with name AttrName.
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val) const;
  /// Get Int attribute for the given id with attribute id AttrId.
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val) const;

  /// Get Flt attribute for the given id with name AttrName.
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TFlt& Val) const;
  /// Get Flt attribute for the given id with attribute id AttrId.
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TFlt& Val) const;

  /// Get Str attribute for the given id with name AttrName.
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TStr& Val) const;
  /// Get Str attribute for the given id with attribute id AttrId.
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TStr& Val) const;

  /// Delete attribute for the give id with name AttrName.
  int DelSAttrDat(const TInt& Id, const TStr& AttrName); 
  /// Delete attribute for the give id with attribute id AttrId.
  int DelSAttrDat(const TInt& Id, const TInt& AttrId);

  // GetNames
  void GetSAttrV(const TInt& Id, const TAttrType AttrType, TAttrPrV& AttrV) const;

  // Get ids of all items with given attribute
  int GetIdVSAttr(const TStr& AttrName, TIntV& IdV) const;
  int GetIdVSAttr(const TInt& AttrId, TIntV& IdV) const;

  // Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  // Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};


class TAttrPair {
private:
  // Sparse Attributes Name Mapping
  TStrIntPrH AttrNameToId;
  TIntIntStrPrH AttrIdToName;

  TIntIntPrPrIntH IntAttrVals;
  TIntIntPrPrFltH FltAttrVals;
  TIntIntPrPrStrH StrAttrVals;
private:
  int GetIdVSAttr(const TInt& AttrId, const TAttrType Type, TIntPrV& IdV) const;
public:
  TAttrPair() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TAttrPair(const TAttrPair& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  // Constructor for loading attributes from a (binary) stream SIn.
  TAttrPair(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  // Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }

  // Add functions
  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TInt& Val); 
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TInt& Val);

  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TFlt& Val); 
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TFlt& Val);

  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TStr& Val); 
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TStr& Val);

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
  void GetSAttrV(const TIntPr& Id, const TAttrType AttrType, TAttrPrV& AttrV) const;

  // Get ids of all items with given attribute
  int GetIdVSAttr(const TStr& AttrName, TIntPrV& IdV) const;
  int GetIdVSAttr(const TInt& AttrId, TIntPrV& IdV) const;

  // Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  // Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};