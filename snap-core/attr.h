class TSparseAttrSingle {
public:
  enum TAttrType { atUndef, atInt, atFlt, atStr, atAny };
  typedef TPair<TStr, TAttrType> TAttrPr;
  typedef TVec<TAttrPr> TAttrPrV;
private:
  // Sparse Attributes Name Mapping
  THash<TStr, TPair<TInt, TInt> > AttrNameToId;
  THash<TInt, TPair<TInt, TStr> > AttrIdToName;

  THash<TPair<TInt, TInt>, TInt> IntAttrVals;
  THash<TPair<TInt, TInt>, TFlt> FltAttrVals;
  THash<TPair<TInt, TInt>, TStr> StrAttrVals;
public:
  TSparseAttrSingle() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TSparseAttrSingle(const TSparseAttrSingle& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  /// Constructor for loading attributes from a (binary) stream SIn.
  TSparseAttrSingle(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  /// Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }

  /// Add functions
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val); 
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val);

  /// Get functions
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TInt& Val) const; 
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TInt& Val) const;

  /// Del functions
  int DelSAttrDat(const TInt& Id, const TStr& AttrName); 
  int DelSAttrDat(const TInt& Id, const TInt& AttrId);

  /// GetNames
  void GetSAttrV(const TInt& Id, TAttrType AttrType, TAttrPrV& AttrV);

  /// Get values over all nodes
  int GetIdVSAttr(const TStr& AttrName, TIntV& IdV);

  /// Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};


class TSparseAttrPair {
public:
  enum TAttrType { atUndef, atInt, atFlt, atStr, atAny };
  typedef TPair<TStr, TAttrType> TAttrPr;
  typedef TVec<TAttrPr> TAttrPrV;
private:
  // Sparse Attributes Name Mapping
  THash<TStr, TPair<TInt, TInt> > AttrNameToId;
  THash<TInt, TPair<TInt, TStr> > AttrIdToName;

  THash<TPair<TInt, TPair<TInt, TInt> >, TInt> IntAttrVals;
  THash<TPair<TInt, TPair<TInt, TInt> >, TFlt> FltAttrVals;
  THash<TPair<TInt, TPair<TInt, TInt> >, TStr> StrAttrVals;
public:
  TSparseAttrPair() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TSparseAttrPair(const TSparseAttrPair& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  /// Constructor for loading attributes from a (binary) stream SIn.
  TSparseAttrPair(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  /// Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }

  /// Add functions
  int AddSAttrDat(const TPair<TInt, TInt>& Id, const TStr& AttrName, TInt& Val); 
  int AddSAttrDat(const TPair<TInt, TInt>& Id, const TInt& AttrId, TInt& Val);

  /// Get functions
  int GetSAttrDat(const TPair<TInt, TInt>& Id, const TStr& AttrName, TInt& Val) const; 
  int GetSAttrDat(const TPair<TInt, TInt>& Id, const TInt& AttrId, TInt& Val) const;

  /// Del functions
  int DelSAttrDat(const TPair<TInt, TInt>& Id, const TStr& AttrName); 
  int DelSAttrDat(const TPair<TInt, TInt>& Id, const TInt& AttrId);

  /// GetNames
  void GetSAttrV(const TPair<TInt, TInt>& Id, TAttrType AttrType, TAttrPrV& AttrV);

  /// Get values over all nodes
  int GetIdVSAttr(const TStr& AttrName, TVec<TPair<TInt, TInt> >& IdV);

  /// Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};