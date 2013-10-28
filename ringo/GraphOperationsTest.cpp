#include "Snap.h"

int main(){
  TTableContext Context;
  // create scheme
  Schema AnimalS;
  AnimalS.Add(TPair<TStr,TAttrType>("Animal", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Size", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Location", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Number", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);
  RelevantCols.Add(2);
  RelevantCols.Add(3);

  PTable P = TTable::LoadSS("Animals", AnimalS, "tests/animals.txt", Context, RelevantCols);
  P->SetSrcCol("Animal");
  P->SetDstCol("Location");
  P->AddSrcNodeAttr("Animal");
  P->AddDstNodeAttr("Location");
  P->AddEdgeAttr("Size");
  P->AddEdgeAttr("Number");

  PNEANet N = P->ToGraph(FIRST);
  for (TNEANet::TNodeI nodeI = N->BegNI(); nodeI < N->EndNI(); nodeI++){
    printf("%d %s\n", nodeI.GetId(), N->GetStrAttrDatN(nodeI, "Animal").CStr());
    printf("%d %s\n", nodeI.GetId(), N->GetStrAttrDatN(nodeI, "Location").CStr());
  }
  PTable NodeTable = TTable::GetNodeTable(N, "NodeTable", Context);
  PTable EdgeTable = TTable::GetEdgeTable(N, "EdgeTable", Context);

  NodeTable->SaveSS("tests/graph_node_table.txt");
  EdgeTable->SaveSS("tests/graph_edge_table.txt");
  return 0;
}
