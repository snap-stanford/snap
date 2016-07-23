/////////////////////////////////////////////////
// Conversion Functions
namespace TSnap {


int LoadModeNetToNet(PMMNet Graph, const TStr& Name, PTable Table, const TStr& NCol,
  TStrV& NodeAttrV) {
  Graph->AddModeNet(Name);
  TModeNet& Net = Graph->GetModeNetByName(Name);
  return LoadMode(Net, Table, NCol, NodeAttrV);
}


int LoadMode(TModeNet& Graph, PTable Table, const TStr& NCol,
  TStrV& NodeAttrV) {

  const TAttrType NodeType = Table->GetColType(NCol);
  const TInt NColIdx = Table->GetColIdx(NCol);

  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }

    // add src and dst nodes to graph if they are not seen earlier
    TInt NVal;
    if (NodeType == atFlt) {
      return -1;
    } else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        NVal = (Table->IntCols)[NColIdx][CurrRowIdx];
      } else {
        NVal = (Table->StrColMaps)[NColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(NVal)) == 0) { continue; }  //illegal value
      }
      if (!Graph.IsNode(NVal)) {Graph.AddNode(NVal); }
    }

    // Aggregate edge attributes and add to graph
    for (TInt i = 0; i < NodeAttrV.Len(); i++) {
      TStr ColName = NodeAttrV[i];
      TAttrType T = Table->GetColType(ColName);
      TInt Index = Table->GetColIdx(ColName);
      switch (T) {
        case atInt:
          Graph.AddIntAttrDatN(NVal, Table->IntCols[Index][CurrRowIdx], ColName);
          break;
        case atFlt:
          Graph.AddFltAttrDatN(NVal, Table->FltCols[Index][CurrRowIdx], ColName);
          break;
        case atStr:
          Graph.AddStrAttrDatN(NVal, Table->GetStrVal(Index, CurrRowIdx), ColName);
          break;
      }
    }
  }
  return 1;
}

int LoadCrossNetToNet(PMMNet Graph, const TStr& Mode1, const TStr& Mode2, const TStr& CrossName,
 PTable Table, const TStr& SrcCol, const TStr& DstCol, TStrV& EdgeAttrV)
{
  Graph->AddCrossNet(Mode1, Mode2, CrossName);
  TCrossNet& Net = Graph->GetCrossNetByName(CrossName);
  return LoadCrossNet(Net, Table, SrcCol, DstCol, EdgeAttrV);
}


int LoadCrossNet(TCrossNet& Graph, PTable Table, const TStr& SrcCol, const TStr& DstCol,
  TStrV& EdgeAttrV)
{

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);

  // node values - i.e. the unique values of src/dst col
  //THashSet<TInt> IntNodeVals; // for both int and string node attr types.
  THash<TFlt, TInt> FltNodeVals;

  // make single pass over all rows in the table
  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }

    // add src and dst nodes to graph if they are not seen earlier
    TInt SVal, DVal;
    if (NodeType == atFlt) {
      return -1;
    } else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        SVal = (Table->IntCols)[SrcColIdx][CurrRowIdx];
        DVal = (Table->IntCols)[DstColIdx][CurrRowIdx];
      } else {
        SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
        DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      }
    }

    // add edge and edge attributes
    if (Graph.AddEdge(SVal, DVal, CurrRowIdx) == -1) { return -1; }

    // Aggregate edge attributes and add to graph
    for (TInt i = 0; i < EdgeAttrV.Len(); i++) {
      TStr ColName = EdgeAttrV[i];
      TAttrType T = Table->GetColType(ColName);
      TInt Index = Table->GetColIdx(ColName);
      switch (T) {
        case atInt:
          Graph.AddIntAttrDatE(CurrRowIdx, Table->IntCols[Index][CurrRowIdx], ColName);
          break;
        case atFlt:
          Graph.AddFltAttrDatE(CurrRowIdx, Table->FltCols[Index][CurrRowIdx], ColName);
          break;
        case atStr:
          Graph.AddStrAttrDatE(CurrRowIdx, Table->GetStrVal(Index, CurrRowIdx), ColName);
          break;
      }
    }
  }
  return 1;
}

}; //namespace TSnap