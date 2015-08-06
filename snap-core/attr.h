
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
  THash<TPair<TInt, TInt>, TInt> FltAttrVals;
  THash<TPair<TInt, TInt>, TInt> StrAttrVals;
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
  int AddSAttrDat(const int& Id, const TStr& AttrName, TInt& Val); 
  int AddSAttrDat(const int& Id, const TInt& AttrId, TInt& Val);

  /// Get functions
  int GetSAttrDat(const int& Id, const TStr& AttrName, TInt& Val) const; 
  int GetSAttrDat(const int& Id, const TInt& AttrId, TInt& Val) const;

  /// Del functions
  int DelSAttrDat(const int& Id, const TStr& AttrName); 
  int DelSAttrDat(const int& Id, const TInt& AttrId);

  /// GetNames
  void GetSAttrV(const TInt& Id, TAttrType AttrType, TAttrPrV& AttrV);

  /// Get values over all nodes
  int GetIdVSAttr(const TStr& AttrName, TIntV& IdV, TIntV& ValV);

  /// Add Attribute mapping
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Attribute Name to ID conversion and vice versa
  int GetSAttrId(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const;
  int GetSAttrName(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const;
};