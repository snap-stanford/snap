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
  /// Clears the contents of the attribute map.
  void Clr() { AttrNameToId.Clr(); AttrIdToName.Clr(); IntAttrVals.Clr(); FltAttrVals.Clr(); StrAttrVals.Clr(); }

  /// Returns the amount of memory used by sparse attributes.
  size_t GetMemUsed() const { return AttrNameToId.GetMemUsed() + AttrIdToName.GetMemUsed() + IntAttrVals.GetMemUsed() + FltAttrVals.GetMemUsed() + StrAttrVals.GetMemUsed(); }

  /// Add Int attribute with name \c AttrName for the given id \c Id.
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, const TInt& Val);
  /// Add Int attribute with attribute id \c AttrId for the given id \c Id.
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, const TInt& Val);

  /// Add Flt attribute with name \c AttrName for the given id \c Id.
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, const TFlt& Val);
  /// Add Flt attribute with attribute id \c AttrId for the given id \c Id.
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, const TFlt& Val);

  /// Add Str attribute with name \c AttrName for the given id \c Id.
  int AddSAttrDat(const TInt& Id, const TStr& AttrName, const TStr& Val);
  /// Add Str attribute with attribute id \c AttrId for the given id \c Id.
  int AddSAttrDat(const TInt& Id, const TInt& AttrId, const TStr& Val);

  /// Get Int attribute with name \c AttrName for the given id \c Id.
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TInt& ValX) const;
  /// Get Int attribute with attribute id \c AttrId for the given id \c Id.
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TInt& ValX) const;

  /// Get Flt attribute with name \c AttrName for the given id \c Id.
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TFlt& ValX) const;
  /// Get Flt attribute with attribute id \c AttrId for the given id \c Id.
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TFlt& ValX) const;

  /// Get Str attribute with name \c AttrName for the given id \c Id.
  int GetSAttrDat(const TInt& Id, const TStr& AttrName, TStr& ValX) const;
  /// Get Str attribute with attribute id \c AttrId for the given id \c Id.
  int GetSAttrDat(const TInt& Id, const TInt& AttrId, TStr& ValX) const;

  /// Delete attribute with name \c AttrName for the given id \c Id.
  int DelSAttrDat(const TInt& Id, const TStr& AttrName); 
  /// Delete attribute with attribute id \c AttrId for the given id \c Id.
  int DelSAttrDat(const TInt& Id, const TInt& AttrId);

  /// Delete all attributes for the given id \c Id.
  void DelSAttrId(const TInt& Id);

  /// Get a list of all attributes of type \c AttrType for the given id \c Id.
  void GetSAttrV(const TInt& Id, const TAttrType AttrType, TAttrPrV& AttrV) const;

  /// Get a list of all ids that have an attribute with name \c AttrName.
  int GetIdVSAttr(const TStr& AttrName, TIntV& IdV) const;
  /// Get a list of all ids that have an attribute with attribute id \c AttrId.
  int GetIdVSAttr(const TInt& AttrId, TIntV& IdV) const;

  /// Adds a mapping for an attribute with name \c Name and type \c AttrType.
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrIdX);

  /// Given the attribute name \c Name, get the attribute id.
  int GetSAttrId(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Given the attribute id \c AttrId, get the attribute name.
  int GetSAttrName(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;
};


class TAttrPair {
private:
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
  /// Constructor for loading attributes from a (binary) stream SIn.
  TAttrPair(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  /// Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }
  /// Clears the contents of the attribute map.
  void Clr() { AttrNameToId.Clr(); AttrIdToName.Clr(); IntAttrVals.Clr(); FltAttrVals.Clr(); StrAttrVals.Clr(); }

  /// Add Int attribute with name \c AttrName for the given id \c Id.
  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TInt& Val);
  /// Add Int attribute with attribute id \c AttrId for the given id \c Id.
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TInt& Val);

  /// Add Flt attribute with name \c AttrName for the given id \c Id.
  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TFlt& Val);
  /// Add Flt attribute with attribute id \c AttrId for the given id \c Id.
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TFlt& Val);

  /// Add Str attribute with name \c AttrName for the given id \c Id.
  int AddSAttrDat(const TIntPr& Id, const TStr& AttrName, const TStr& Val);
  /// Add Str attribute with attribute id \c AttrId for the given id \c Id.
  int AddSAttrDat(const TIntPr& Id, const TInt& AttrId, const TStr& Val);

  /// Get Int attribute with name \c AttrName for the given id \c Id.
  int GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TInt& ValX) const;
  /// Get Int attribute with attribute id \c AttrId for the given id \c Id.
  int GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TInt& ValX) const;

  /// Get Flt attribute with name \c AttrName for the given id \c Id.
  int GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TFlt& ValX) const;
  /// Get Flt attribute with attribute id \c AttrId for the given id \c Id.
  int GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TFlt& ValX) const;

  /// Get Str attribute with name \c AttrName for the given id \c Id.
  int GetSAttrDat(const TIntPr& Id, const TStr& AttrName, TStr& ValX) const;
  /// Get Str attribute with attribute id \c AttrId for the given id \c Id.
  int GetSAttrDat(const TIntPr& Id, const TInt& AttrId, TStr& ValX) const;

  /// Delete attribute with name \c AttrName for the given id \c Id.
  int DelSAttrDat(const TIntPr& Id, const TStr& AttrName);
  /// Delete attribute with attribute id \c AttrId for the given id \c Id.
  int DelSAttrDat(const TIntPr& Id, const TInt& AttrId);

  /// Delete all attributes for the given id \c Id.
  void DelSAttrId(const TIntPr& Id);

  /// Get a list of all attributes of the given type \c AttrType for the given id \c Id.
  void GetSAttrV(const TIntPr& Id, const TAttrType AttrType, TAttrPrV& AttrV) const;

  /// Get a list of all ids that have an attribute with name \c AttrName.
  int GetIdVSAttr(const TStr& AttrName, TIntPrV& IdV) const;
  /// Get a list of all ids that have an attribute with attribute id \c AttrId.
  int GetIdVSAttr(const TInt& AttrId, TIntPrV& IdV) const;

  /// Adds a mapping for an attribute with name Name and type \c AttrType.
  int AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt& AttrIdX);

  /// Given the attribute name \c Name, get the attribute id and type.
  int GetSAttrId(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Given the attribute id \c AttrId, get the attribute name and type.
  int GetSAttrName(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;
};
